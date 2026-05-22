
//local include
#include "log.h"
#include "klog.h"

//self include
#include "xGateUtil.h"

#include "xGateHmpGstBin.h"
#include "xGateSipSignalProcessor.h"

#define THISMODULE "HmpBin"

/*TODO: Yoga 
 * 1. implement gstreamer error handling logic
 * 2. make a max pool configuration as dynamic
 */
xGateHmpGstPipeline::xGateHmpGstPipeline(string pipelineName, HmpGstPipelineType pipelineType) 
{
  CLog::Detail(THISMODULE, "xGateHmpGstPipeline::xGateHmpGstPipeline %s", pipelineName.c_str());

  m_sendBusId = 0;
  m_recvBusId = 0;
  m_clientCount = 0;
  gchar sendPipeline[30];
  sprintf(sendPipeline,"Send_%s", pipelineName.c_str());
  m_sendPipelineName = sendPipeline;

  gchar recvPipeline[30];
  sprintf(recvPipeline,"Recv_%s", pipelineName.c_str());
  m_recvPipelineName = recvPipeline;

  gchar confPipeline[30];
  sprintf(confPipeline,"Conf_%s", pipelineName.c_str());
  m_confPipelineName = confPipeline;

  gchar rbtPipeline[30];
  sprintf(rbtPipeline, "Rbt_%s", pipelineName.c_str());
  m_rbtPipelineName = rbtPipeline; 
 
  clientA_ip= "";
  clientB_ip="";
  clientA_port =0;
  clientB_port = 0;
  hmpBinA = NULL;
  hmpBinB = NULL;
  udpSockA = NULL;
  udpSockB = NULL;
  srcClient = NULL;
  dstClient = NULL;
  m_sendPipeline = NULL;
  m_sendBus = NULL;
  m_sendBusId = NULL;
  m_recvPipeline = NULL;
  m_recvBus = NULL;
  m_recvBusId = NULL; 
  m_rbtPipeline = NULL;
  m_rbtBus = NULL;
  m_rbtBusId = NULL;
  m_autoRepeat = false;
  m_rbtActive = false;
  m_dtmfBuffer ="";
  m_dtmfTimeout = 0;
}

xGateHmpGstPipeline::~xGateHmpGstPipeline()
{
  CLog::Detail(THISMODULE, "xGateHmpGstPipeline::~xGateHmpGstPipeline");
  if(m_sendPipeline) {
    gst_element_set_state(m_sendPipeline, GST_STATE_NULL); 
    gst_object_unref (GST_OBJECT (m_sendPipeline));
    CLog::Detail(THISMODULE, "xGateHmpGstPipeline::~xGateHmpGstPipeline %s", m_sendPipelineName.c_str());
  }
  
  if(m_recvPipeline) {
    gst_element_set_state(m_recvPipeline, GST_STATE_NULL);
    gst_object_unref (GST_OBJECT (m_recvPipeline));
    CLog::Detail(THISMODULE, "xGateHmpGstPipeline::~xGateHmpGstPipeline %s", m_recvPipelineName.c_str());
  }

  if(m_rbtPipeline) {
    gst_element_set_state(m_rbtPipeline, GST_STATE_NULL);
    gst_object_unref (GST_OBJECT (m_rbtPipeline));
    CLog::Detail(THISMODULE, "xGateHmpGstPipeline::~xGateHmpGstPipeline %s", m_rbtPipelineName.c_str());
  }

  if(m_sendBus) {
    gst_bus_set_sync_handler(m_sendBus, NULL, NULL, NULL);
    gst_object_unref(GST_OBJECT (m_sendBus));
  }

  if(m_recvBus) {
    gst_bus_set_sync_handler(m_recvBus, NULL, NULL, NULL);
    gst_object_unref(GST_OBJECT (m_recvBus));
  }

  if(m_rbtBus) {
    gst_bus_set_sync_handler(m_rbtBus, NULL, NULL, NULL);
    gst_object_unref(GST_OBJECT (m_rbtBus));
  }

  if(m_sendBusId) {
    g_source_remove (m_sendBusId);
  }

  if(m_recvBusId) {
    g_source_remove (m_recvBusId);
  }

  if(m_rbtBusId) {
    g_source_remove (m_rbtBusId);
  }
}

gboolean xGateHmpGstPipeline::simple_pipeline_create() 
{
  CLog::Detail(THISMODULE, "Enter xGateHmpGstPipeline::simple_pipeline_create ");

  gboolean retVal = FALSE;
  m_sendPipeline = gst_pipeline_new(m_sendPipelineName.c_str());
  m_recvPipeline = gst_pipeline_new(m_recvPipelineName.c_str());

  if(!m_sendPipeline || !m_recvPipeline) {
    CLog::Error(THISMODULE, "xGateHmpGstPipeline::simple_pipeline_create failed !!");
    retVal = FALSE;
  } else retVal = TRUE;

  m_sendBus = gst_pipeline_get_bus(GST_PIPELINE(m_sendPipeline));
  m_recvBus = gst_pipeline_get_bus(GST_PIPELINE(m_recvPipeline));

  CLog::Detail(THISMODULE, "Enter xGateHmpGstPipeline::simple_pipeline_create successfull");

  return retVal; 
}

gboolean xGateHmpGstPipeline::conf_pipeline_create()
{
  CLog::Detail(THISMODULE, "Enter xGateHmpGstPipeline::confPipeline_create %s", m_confPipelineName.c_str());
  gboolean retVal = FALSE;

  m_confPipeline = gst_pipeline_new(m_confPipelineName.c_str());

  if(!m_confPipeline) {
    CLog::Error(THISMODULE, "xGateHmpGstPipeline::confPipeline_create failed !!");
    retVal = FALSE;   
  }

  m_pipelineClock = gst_system_clock_obtain();
  //create common elements in pipeline
  m_audioMixerIn = gst_element_factory_make("audiomixer", NULL);
  m_audioMixerOut = gst_element_factory_make("tee", NULL);

  if(!m_audioMixerIn || m_audioMixerOut) {
    CLog::Error(THISMODULE, "xGateHmpGstPipeline::confPipeline_create element make failed !!");
    retVal = FALSE;  
  }

  gst_bin_add_many(GST_BIN(m_confPipeline), m_audioMixerIn, m_audioMixerOut, NULL);
  retVal = gst_element_link_many (m_audioMixerIn, m_audioMixerOut, NULL);

  if(!retVal) {
    CLog::Error(THISMODULE, "xGateHmpGstPipeline::confPipeline_create linking elements failed !!");
    retVal = FALSE;
  }

  m_confBus = gst_pipeline_get_bus(GST_PIPELINE(m_confPipeline));
  m_confBusId = gst_bus_add_watch(m_confBus, &xGateHmpGstPipeline::handle_event_dispatcher, this);

  if(GST_DEBUG) {
    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(m_confPipeline), GST_DEBUG_GRAPH_SHOW_ALL, m_confPipelineName.c_str());
  }
  CLog::Detail(THISMODULE, "xGateHmpGstPipeline::confPipeline_create %s success", m_confPipelineName.c_str()); 
  return TRUE;
}

gboolean xGateHmpGstPipeline::rbt_pipeline_create()
{
  gboolean retVal = FALSE;
  m_rbtPipeline = gst_pipeline_new(m_rbtPipelineName.c_str());

  if(!m_rbtPipeline) {
    CLog::Error(THISMODULE, "xGateHmpGstPipeline::rbt_pipeline_create failed !!");
    retVal = FALSE;
  } else retVal = TRUE;

  m_rbtBus = gst_pipeline_get_bus(GST_PIPELINE(m_rbtPipeline));

  CLog::Detail(THISMODULE, "Enter xGateHmpGstPipeline::rbt_pipeline_create successfull");

  return retVal;
}

gboolean xGateHmpGstPipeline::play_hmp_wavfile(Client *sClient, char* playWavfile)
{
   string client_ip("");
   unsigned int client_port = 0;
   xGateHmpGstBin *hmpBinrbt = NULL;
   GSocket *udpSockrbt = NULL;
   CLog::Detail(THISMODULE, "xGateHmpGstPipeline::play_ringback_tone in %s, File %s", m_rbtPipelineName.c_str(), playWavfile);

   
   /* check wav file is physically available */
   FILE * rbtFile = fopen(playWavfile, "rb");
   if(!rbtFile)
   {
     CLog::Error(THISMODULE, "xGateHmpGstPipeline::play_ringback_tone file %s not able to open", playWavfile);
     ClientDetail &client_Detail = sClient->m_detail;
     client_ip = client_Detail.m_ip;
     client_port = client_Detail.m_port;
   hmpBinrbt = sClient->m_hmpBin;
   udpSockrbt = sClient->m_hmpBin->m_udpSocket;
   hmpBinrbt->add_client_rbt(client_ip, client_port);

     return false;
   }

   ClientDetail &client_Detail = sClient->m_detail;
   client_ip = client_Detail.m_ip;
   client_port = client_Detail.m_port;

   hmpBinrbt = sClient->m_hmpBin;
   udpSockrbt = sClient->m_hmpBin->m_udpSocket;

   ringbkBin = GST_ELEMENT(hmpBinrbt->m_ringbkBin);

   gst_bin_add_many(GST_BIN(m_rbtPipeline), ringbkBin, NULL);
   gst_element_link(GST_ELEMENT(m_rbtPipeline), ringbkBin);

   if(!client_ip.empty() && client_port > 0) {
      hmpBinrbt->add_client_rbt(client_ip, client_port);
      g_object_set(G_OBJECT(hmpBinrbt->m_fileSrc),"location", playWavfile, NULL);
      g_object_set(G_OBJECT(hmpBinrbt->m_rbtpUdp), "socket", udpSockrbt, NULL);
   }

   m_rbtBusId = gst_bus_add_watch (m_rbtBus, &xGateHmpGstPipeline::handle_event_dispatcher, this);

   gst_element_set_state(m_rbtPipeline, GST_STATE_PLAYING);
  
   m_rbtActive = true;

   CLog::Detail(THISMODULE, "xGateHmpGstPipeline::play_ringback_tone started playing");

  if(GST_DEBUG) {
    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(m_rbtPipeline), GST_DEBUG_GRAPH_SHOW_ALL, m_rbtPipelineName.c_str());
  }
  
  return TRUE;
}

gboolean xGateHmpGstPipeline::pipeline_connect_clients(Client *sClient, Client *dClient)
{
  CLog::Detail(THISMODULE, "xGateHmpGstPipeline::pipeline_connect_clients in %s", m_sendPipelineName.c_str());
 
  srcClient = sClient;
  ClientDetail &clientA_Detail = srcClient->m_detail;

  if (clientA_Detail.m_mediaMode == EN_MEDIA_DTLS) {
    clientA_ip = clientA_Detail.m_relayIpaddr;
    clientA_port = clientA_Detail.m_relayPort;
  } else {
    clientA_ip = clientA_Detail.m_ip;
    clientA_port = clientA_Detail.m_port;
  }
  hmpBinA = sClient->m_hmpBin;
  udpSockA = hmpBinA->m_udpSocket;

  dstClient = dClient;
  ClientDetail &clientB_Detail = dstClient->m_detail;
  if (clientB_Detail.m_mediaMode == EN_MEDIA_DTLS) {
    clientB_ip = clientB_Detail.m_relayIpaddr;
    clientB_port = clientB_Detail.m_relayPort;
  } else {
    clientB_ip = clientB_Detail.m_ip;
    clientB_port = clientB_Detail.m_port;
  }
  hmpBinB = dClient->m_hmpBin;
  udpSockB = hmpBinB->m_udpSocket;

  recvBinA = GST_ELEMENT(hmpBinA->m_recvBin);
  sendBinA = GST_ELEMENT(hmpBinA->m_sendBin);
  recvBinB = GST_ELEMENT(hmpBinB->m_recvBin);
  sendBinB = GST_ELEMENT(hmpBinB->m_sendBin);

  gst_bin_add_many(GST_BIN(m_sendPipeline), recvBinA, sendBinB, NULL);
  gst_element_link_many(recvBinA, sendBinB, NULL);

  gst_bin_add_many(GST_BIN(m_recvPipeline), recvBinB, sendBinA, NULL);
  gst_element_link_many(recvBinB, sendBinA, NULL);

  //switch client address and set in udpSink 
  if(hmpBinA && udpSockB && !clientA_ip.empty() && clientA_port > 0) {
      hmpBinA->add_client_addr(clientA_ip, clientA_port);     
      g_object_set(G_OBJECT(hmpBinA->m_sendUdp), "socket", udpSockB, NULL);
      g_object_set(G_OBJECT(hmpBinA->m_recvUdp), "socket", udpSockB, NULL);
      if (clientA_Detail.m_mediaMode == EN_MEDIA_DTLS) {
         hmpBinA->set_stun_target_client(clientA_ip, clientA_port);        
      }
  } else {
    CLog::Error(THISMODULE, "pipeline_connect_clients switch socket failed for client A");
    return FALSE; 
  }

  if(hmpBinB && udpSockA && !clientB_ip.empty() && clientB_port > 0) {
      hmpBinB->add_client_addr(clientB_ip, clientB_port);     
      g_object_set(G_OBJECT(hmpBinB->m_sendUdp), "socket", udpSockA, NULL);
      g_object_set(G_OBJECT(hmpBinB->m_recvUdp), "socket", udpSockA, NULL);
      if (clientB_Detail.m_mediaMode == EN_MEDIA_DTLS) {
         hmpBinB->set_stun_target_client(clientB_ip, clientB_port);      
      }
  } else {
    CLog::Error(THISMODULE, "pipeline_connect_clients switch socket failed for client B");
    return FALSE;
  }

  if(m_rbtActive){
    gst_element_set_state(m_rbtPipeline, GST_STATE_NULL);
    m_rbtActive = false;
  }

  m_sendBusId = gst_bus_add_watch(m_sendBus, &xGateHmpGstPipeline::handle_event_dispatcher, this);
  m_recvBusId = gst_bus_add_watch(m_recvBus, &xGateHmpGstPipeline::handle_event_dispatcher, this);

  gst_element_set_state(m_sendPipeline, GST_STATE_PLAYING);
  gst_element_set_state(m_recvPipeline, GST_STATE_PLAYING);

  if(GST_DEBUG) {
    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(m_sendPipeline), GST_DEBUG_GRAPH_SHOW_ALL, m_sendPipelineName.c_str());
    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(m_recvPipeline), GST_DEBUG_GRAPH_SHOW_ALL, m_recvPipelineName.c_str());
  }

  return TRUE;
}

gboolean xGateHmpGstPipeline::stop_hmp_wavfile(Client *client)
{
  xGateHmpGstBin *hmpBinrbt = NULL;
  CLog::Detail(THISMODULE, "HostGSTRtpBin::stop_hmp_wavfile stoping ringback tone\n");\

    if(m_rbtPipeline) {
      gst_element_set_state(m_rbtPipeline, GST_STATE_NULL);
      m_rbtActive = false;

      hmpBinrbt = client->m_hmpBin;

      if(hmpBinrbt) {
        ringbkBin = GST_ELEMENT(hmpBinrbt->m_ringbkBin);
      } else return false;

      gst_element_unlink_many(m_rbtPipeline, ringbkBin, NULL);
      gst_bin_remove_many(GST_BIN(m_rbtPipeline), ringbkBin, NULL);
    } else {
      CLog::Detail(THISMODULE, "HostGSTRtpBin::stop_hmp_wavfile Stop playing ringback tone\n");\
        return false;
    }

  return true;
}

gboolean xGateHmpGstPipeline::disconnect_simple_bins(Client *sClient)
{
   CLog::Detail(THISMODULE, "xGateHmpGstPipeline::disconnect_simple_bins");
   hmpBinA = sClient->m_hmpBin;

   if(!hmpBinA) {
     CLog::Detail(THISMODULE, "xGateHmpGstPipeline::disconnect_simple_bins valid bins and udp reference");
     return false;
   }
   
   udpsrc   = GST_ELEMENT(hmpBinA->m_recvUdp);
   recvBinA = GST_ELEMENT(hmpBinA->m_recvBin);
   sendBinA = GST_ELEMENT(hmpBinA->m_sendBin);
   udpsink   = GST_ELEMENT(hmpBinA->m_sendUdp);

   if(!recvBinA || !sendBinA) {
     CLog::Detail(THISMODULE, "xGateHmpGstPipeline::disconnect_simple_bins no proper send and receive bins");
     return false;
   }

   gst_element_unlink_many(GST_ELEMENT(sendBinA), udpsink, NULL);
   gst_bin_remove_many(GST_BIN(sendBinA), udpsink, NULL);

   gst_element_unlink_many(GST_ELEMENT(recvBinA), udpsrc, NULL);
   gst_bin_remove_many(GST_BIN(recvBinA), udpsrc, NULL);

   CLog::Detail(THISMODULE, "disconnect_simple_bins from bins %s and %s success", hmpBinA->m_sendBinName.c_str(), hmpBinA->m_recvBinName.c_str());
   return true;
}


gboolean xGateHmpGstPipeline::disconnect_simple_client(Client *sClient, Client *dClient)
{
  CLog::Detail(THISMODULE, "xGateHmpGstPipeline::disconnect_simple_client");
  gst_element_set_state(m_sendPipeline, GST_STATE_NULL);
  gst_element_set_state(m_recvPipeline, GST_STATE_NULL);

  hmpBinA = sClient->m_hmpBin;
  hmpBinB = dClient->m_hmpBin;

  if(hmpBinA) {
     recvBinA = GST_ELEMENT(hmpBinA->m_recvBin);
     sendBinA = GST_ELEMENT(hmpBinA->m_sendBin);
  } else {
     CLog::Error(THISMODULE, "xGateHmpGstPipeline::disconnect_transcode_client failed, due to orgination bin not yet created");
     return FALSE;
  }

  if(hmpBinB) {
     recvBinB = GST_ELEMENT(hmpBinB->m_recvBin);
     sendBinB = GST_ELEMENT(hmpBinB->m_sendBin);
  } else {
     CLog::Error(THISMODULE, "xGateHmpGstPipeline::disconnect_transcode_client failed, due to termination bin not yet created");
     if(recvBinA) {
       gst_bin_remove_many(GST_BIN(m_sendPipeline), recvBinA, NULL);
     }
     if(sendBinA) {
       gst_bin_remove_many(GST_BIN(m_recvPipeline), sendBinA, NULL);
     }
     return FALSE;
  }
  if(!recvBinB || !sendBinB || !recvBinA || !sendBinA) {
    CLog::Detail(THISMODULE, "xGateHmpGstPipeline::disconnect_transcode_client failed");
    return FALSE;
  }
  
  //unlink sendBin with udpSink
  gst_element_unlink_many(recvBinA, sendBinB, NULL);
  gst_bin_remove_many(GST_BIN(m_sendPipeline), recvBinA, sendBinB, NULL);
  
  gst_element_unlink_many(recvBinB, sendBinA, NULL);
  gst_bin_remove_many(GST_BIN(m_recvPipeline), recvBinB, sendBinA, NULL);
  
  CLog::Detail(THISMODULE, "disconnect_simple_client from pipeline %s and %s success", m_sendPipelineName.c_str(), m_recvPipelineName.c_str());

  return true;
}

gboolean xGateHmpGstPipeline::handle_event(GstBus* bus, GstMessage* msg, Client *client)
{
  char dtmfDigit[10];
  gint number;
  const GstStructure* structure;

  switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_ERROR:
      {
#if 1
        CLog::Detail(THISMODULE, "xGateHmpGstPipeline::handle_event got GST_MESSAGE_ERROR message");
        GError *err = NULL;
        gchar *dbgInfo = NULL;
        gst_message_parse_error(msg, &err, &dbgInfo);
        if(err) {
          g_printerr("ERROR from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
          CLog::Error(THISMODULE, "ERROR from element %s: %s", GST_OBJECT_NAME(msg->src), err->message);
          g_error_free(err);
        }
        if(dbgInfo) {
          g_printerr("Debugging info: %s\n", (dbgInfo) ? dbgInfo : "none");
          CLog::Error(THISMODULE, "Debugging info: %s\n", dbgInfo);
          g_free(dbgInfo);
        }
#endif
        break;
      }
    case GST_MESSAGE_EOS:
      {
        CLog::Detail(THISMODULE, "xGateHmpGstPipeline::handle_event got GST_MESSAGE_EOS message");
        if(m_rbtPipeline) {
           if (m_autoRepeat) { 
              gst_element_set_state(m_rbtPipeline, GST_STATE_NULL);
              gst_element_set_state(m_rbtPipeline, GST_STATE_PLAYING);
              CLog::Detail(THISMODULE, "xGateHmpGstPipeline::handle_event EOS encountered. Playing file again.");
           } else {
              gst_element_set_state(m_rbtPipeline, GST_STATE_NULL);
              CLog::Detail(THISMODULE, "HostGSTRtpBin::handle_event : EOS encountered. Don't play again. \n");

              post_playback_eos(client);
           }
        }
        break;
      }
    case GST_MESSAGE_ELEMENT:
      {
        CLog::Detail(THISMODULE, "xGateHmpGstPipeline::handle_event got GST_MESSAGE_ELEMENT message");
        structure=gst_message_get_structure(msg);
        if(!strcmp(gst_structure_get_name (structure),"dtmf-event"))
        {
           gst_structure_get_int(structure, "number", &number);
           CLog::Detail(THISMODULE, "HostGSTRtpBin::handle_event : DTMF event came. digit %d \n", number);
        }
        snprintf(dtmfDigit, sizeof(dtmfDigit), "%d", number);
        strcat(m_dtmfBuffer, dtmfDigit);
        break;
      }
    case GST_MESSAGE_STATE_CHANGED:
      {
        //CLog::Detail(THISMODULE, "xGateHmpGstPipeline::handle_event got GST_MESSAGE_STATE_CHANGED message");
        break;
      }
    case GST_MESSAGE_UNKNOWN:
    default:
      {
        //CLog::Detail(THISMODULE, "xGateHmpGstPipeline::handle_event got GST_MESSAGE_UNKNOWN message");
        break;
      }
  }
  return TRUE; //it's important return TRUE from this function
}

gboolean xGateHmpGstPipeline::handle_event_dispatcher(GstBus* bus, GstMessage* msg, gpointer data) 
{
   xGateHmpGstPipeline* hmpGstPipeline = (xGateHmpGstPipeline*) data;
   Client *client = hmpGstPipeline->m_clientList[0];
 
   if(hmpGstPipeline) 
     return hmpGstPipeline->handle_event(bus, msg, client);
   
   CLog::Error(THISMODULE, "xGateHmpGstPipeline::handle_event_dispatcher, not valid GST Pipeline\n");
   return FALSE;
}

void xGateHmpGstPipeline::post_playback_eos(Client *client)
{
   xGateMgMsg *pMgMsg = new xGateMgMsg();
   MgMediaDetail mediaDetail;

   pMgMsg->setMsgType(EN_XGATE_MSG_MGC_MG_IN);

   mediaDetail.call_id = client->m_detail.m_callId;
   mediaDetail.leg_id = client->m_detail.m_legid;
   mediaDetail.ip_addr = client->m_detail.m_ip;
   mediaDetail.port = client->m_detail.m_port;
   mediaDetail.codec = client->m_detail.m_codec; 
   mediaDetail.ptime = client->m_detail.m_pTime;
   mediaDetail.pb_state = EN_XGATE_PB_EOS;
   mediaDetail.media_proto = client->m_detail.m_mediaMode;
   mediaDetail.out_proto = client->m_detail.m_outProto;
   mediaDetail.call_dir = client->m_detail.m_callDir;
   
   pMgMsg->set_media_detail(mediaDetail);

   ACE_Task<ACE_MT_SYNCH>* taskPtr = NULL;
   taskPtr = (xGateSipSignalProcessor*)GetSipPostHandler(XGATEPOSTTYPE::EN_PROCESS);
   if(!taskPtr) {
     CLog::Error(THISMODULE, "xGateHmpGstPipeline::handle_pbend_stream SipSignalProcessor task not found");
     return EN_XGATE_STATUS_ERROR;
   }

   ACE_Message_Block* pAmb = NULL;
   pAmb = static_cast<xGateMgMsg*> (pMgMsg);
   ACE_Time_Value tval ((time(NULL) ));
   tval += ACE_Time_Value(0,1);

   if (taskPtr->putq(pAmb, &tval)) {
      CLog::Detail(THISMODULE, "xGateHmpGstPipeline::handle_pbend_stream SipSignalProcessor enqueue message success");
   } else {
      CLog::Error(THISMODULE, "xGateHmpGstPipeline::handle_pbend_stream SipSignalProcessor enqueue message failed");
   }
}

xGateHmpGstPipeline * HmpGstPipelineFactory::pipeline_create(string& pipelineName, HmpGstPipelineType pipelineType)
{

  CLog::Detail(THISMODULE, "Enter HmpGstPipelineFactory::pipeline_create");
  xGateHmpGstPipeline *pHmpGstPipeline = NULL;
  switch(pipelineType) {
    case EN_PIPELINE_SINGLE_CALL:
        CLog::Detail(THISMODULE, "HmpGstPipelineFactory::simple_pipeline_create creating PIPELINE_SINGLE_CALL");
        pHmpGstPipeline = new xGateHmpGstPipeline(pipelineName, pipelineType);
        pHmpGstPipeline->simple_pipeline_create();
        break;

    case EN_PIPELINE_CONF_CALL:
        CLog::Detail(THISMODULE, "HmpGstPipelineFactory::conf_pipeline_create creating PIPELINE_CONF_CALL");
        pHmpGstPipeline = new xGateHmpGstPipeline(pipelineName, pipelineType);
        pHmpGstPipeline->conf_pipeline_create();
        break;
    
    case EN_PIPELINE_UNKNOWN:
    default:
        CLog::Detail(THISMODULE, "HmpGstPipelineFactory::pipeline_create unable to create UNKNOWN_BIN type");
        break;
  }

  return pHmpGstPipeline;  
}


/* HmpGstBin base class implementation
*/
xGateHmpGstBin::xGateHmpGstBin(string binName, HmpGstBinType binType) 
{
  CLog::Detail(THISMODULE, "xGateHmpGstBin::xGateHmpGstBin %s",binName.c_str());
  m_sendBin = NULL;
  m_recvBin = NULL;
  m_ringbkBin = NULL;
 
  m_sendUdp = NULL;
  m_recvUdp = NULL;
  m_rbtpUdp = NULL;
  m_udpSocket = NULL;
  m_udpPort = 0;
  m_binType = binType;

  gchar sendBin[30];
  gchar ringbkBin[30];
  sprintf(sendBin,"%s_Send", binName.c_str());
  sprintf(ringbkBin,"%s_Rbt", binName.c_str());
  m_sendBinName = sendBin;
  m_ringbkBinName = ringbkBin;
  CLog::Detail(THISMODULE, "xGateHmpGstBin::xGateHmpGstBin %s",m_sendBinName.c_str());

  gchar recvBin[30];
  sprintf(recvBin,"%s_Recv", binName.c_str());
  m_recvBinName = recvBin;
  CLog::Detail(THISMODULE, "xGateHmpGstBin::xGateHmpGstBin %s",m_recvBinName.c_str());

  gstSrtpCaps = (GstSrtpCaps *) malloc(sizeof(GstSrtpCaps));
  gstSrtpCaps->key_is_set =false;
 
  //params = (SecurityParams *) malloc(sizeof(SecurityParams));
  srtp_auth = 0;
  latency = 40;
  m_pTime = 0;
}

xGateHmpGstBin::~xGateHmpGstBin()
{
  CLog::Detail(THISMODULE, "xGateHmpGstBin::~xGateHmpGstBin ");
  if(m_udpSocket) {
    g_socket_close(m_udpSocket, NULL);
    g_object_unref(G_OBJECT(m_udpSocket));
    m_udpSocket = NULL;
  }
  m_clientIp = "";
  m_udpPort = 0;
}

gboolean xGateHmpGstBin::create_udp_src_sink(HmpGstCodecType codecType, unsigned int serverPort) 
{
  CLog::Detail(THISMODULE, "xGateHmpGstUdpSrc::create_udp_src");
  gboolean retVal = FALSE;
  m_codecType = codecType;

  m_udpPort = serverPort;

  //create and bind socket
  retVal = create_udp_socket();
  if(retVal) {
    m_recvUdp = gst_element_factory_make("udpsrc", NULL);
    if(!m_recvUdp) {
      CLog::Error(THISMODULE, "xGateHmpGstUdpSrc::xGateHmpGstUdpSrc gst_element_make failed");
      return FALSE;
    }
  
    //setting properties
    g_object_set(G_OBJECT(m_recvUdp),"socket", m_udpSocket, NULL);
    g_object_set(G_OBJECT(m_recvUdp),"timeout", 0, NULL);
    g_object_set(G_OBJECT(m_recvUdp),"reuse", TRUE, NULL);
    g_object_set(G_OBJECT(m_recvUdp), "close-socket", FALSE, NULL);
  }

  m_sendUdp = gst_element_factory_make("multiudpsink", NULL);
  if(!m_sendUdp) {
    CLog::Error(THISMODULE, "xGateHmpGstUdpSink::create_udp_sink gst_element_make failed");
    return FALSE;
  }
  g_object_set(G_OBJECT(m_sendUdp), "close-socket", FALSE, NULL);

  return TRUE;
}

gboolean xGateHmpGstBin::create_udprbt_sink() 
{
  m_rbtpUdp = gst_element_factory_make("multiudpsink", NULL);
  if(!m_rbtpUdp) {
    CLog::Error(THISMODULE, "xGateHmpGstUdp::create_udprbt_sink gst_element_make failed");
    return FALSE;
  }
  g_object_set(G_OBJECT(m_rbtpUdp), "close-socket", FALSE, NULL);  
  return TRUE;
}

gboolean xGateHmpGstBin::create_udp_socket() 
{
  CLog::Detail(THISMODULE, "xGateHmpGstUdpSrc::create_udp_socket");
  gboolean retVal = FALSE;
  do {
    m_udpSocket = NULL;
    retVal = FALSE;
    m_udpSocket = g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_DATAGRAM, G_SOCKET_PROTOCOL_UDP, NULL);
    if(!m_udpSocket) {
      CLog::Error(THISMODULE, "xGateHmpGstUdpSrc::create_udp_socket g_socket_new failed!");
      m_udpPort = 0;
      return NULL;
    }
    GInetAddress *address = g_inet_address_new_any (G_SOCKET_FAMILY_IPV4);
    GSocketAddress* gsockaddress=g_inet_socket_address_new(address, m_udpPort);
    retVal = g_socket_bind(m_udpSocket, gsockaddress, TRUE, NULL);
    g_object_unref(gsockaddress);
    g_object_unref(address);
  } while(!retVal);

  if(!m_udpSocket) {
    CLog::Error(THISMODULE, "xGateHmpGstUdpSrc::create_udp_socket failed to create valide udp socket");
    m_udpPort = 0;
    return NULL;
  }

  CLog::Detail(THISMODULE, "xGateHmpGstUdpSrc::create_udp_socket success for udp port: %d", m_udpPort);
  return retVal;
} 

void xGateHmpGstBin::set_server_addr(string serverIp, unsigned int serverPort)
{
  m_serverIp = serverIp;
  m_udpPort = serverPort;
  CLog::Detail(THISMODULE, "xGateHmpGstUdpSink::get_server_addr %s:%d", serverIp.c_str(), serverPort);
}

void xGateHmpGstBin::set_client_addr(string clientIp, unsigned int clientPort)
{
  m_clientIp = clientIp;
  m_clientPort = clientPort;
  CLog::Detail(THISMODULE, "xGateHmpGstUdpSink::set_client_addr %s:%d", m_clientIp.c_str(), m_clientPort);
}

void xGateHmpGstBin::add_client_addr(string clientIp, unsigned int clientPort)
{
  CLog::Detail(THISMODULE, "xGateHmpGstUdpSink::add_client_addr %s:%d", clientIp.c_str(), clientPort);
  char buff[30] = {0};
  snprintf(buff, 30, "%s:%d", clientIp.c_str(), clientPort);
  g_object_set(G_OBJECT(m_sendUdp), "clients", buff, NULL);
}

void xGateHmpGstBin::add_client_rbt(string clientIp, unsigned int clientPort)
{
  CLog::Detail(THISMODULE, "xGateHmpGstUdp::add_client_rbt %s:%d", clientIp.c_str(), clientPort);
  char client_rbt[30] = {0};
  snprintf(client_rbt, 30, "%s:%d", clientIp.c_str(), clientPort);
  g_object_set(G_OBJECT(m_rbtpUdp), "clients", client_rbt, NULL);
  g_object_set(G_OBJECT(m_rbtpUdp),"async", FALSE, "sync", TRUE, NULL);
}

void xGateHmpGstBin::remove_client_addr()
{
  CLog::Detail(THISMODULE, "xGateHmpGstUdpSink::remove_client_addr");
  g_signal_emit_by_name(m_sendUdp, "clear", this);
}

void xGateHmpGstBin::update_socket(GSocket *udpSocket)
{
  CLog::Detail(THISMODULE, "xGateHmpGstUdpSink::update_socket");
  g_object_set(G_OBJECT(m_sendUdp), "socket", udpSocket, NULL);
}

void xGateHmpGstBin::generateConnectId(char *pout, int len)
{
  int i;
  for(i = 0; i < len; i++)
  {
    pout[i] = CONNECTSTR[rand()%CONNECTSTR_LEN];
  }
  pout[i]='\0';
  CLog::Info(THISMODULE,"GENERATED CONNECTEDID STR %s\n",pout);
}

void xGateHmpGstBin::set_client_details(ClientDetail &cliDetail)
{
  gchar iceUfrag[100];
  m_mediaMode  = cliDetail.m_mediaMode;
  sprintf(iceUfrag, "%s:%s", cliDetail.m_locIceUfrag.c_str(), cliDetail.m_remIceUfrag.c_str());
  strcpy(m_params.iceufrag,iceUfrag);

  strcpy(m_params.icepwd, cliDetail.m_icePwd.c_str());
  sprintf(m_params.cryptoLine, cliDetail.m_fingerPrint.c_str());

  strcpy(m_params.pemFile, cliDetail.m_pemFile.c_str());
  m_pTime = cliDetail.m_pTime;
  generateConnectId(m_params.connectId,15);

  CLog::Detail(THISMODULE," xGateHmpGstBin::set_client_details Media mode %d  ICE_UFRAG %s  ICE_PWD %s PTime %d Conn Id %s\n",m_mediaMode, m_params.iceufrag, m_params.icepwd, m_pTime, m_params.connectId);
}

gboolean xGateHmpGstBin::ringbkBin_create()
{
    m_ringbkBin = GST_BIN(gst_bin_new(m_ringbkBinName.c_str()));
    gint64 ptime;

    m_fileSrc = gst_element_factory_make ("filesrc", NULL);

    m_wavParse = gst_element_factory_make ("wavparse", NULL);
    m_rbtEnc = gst_element_factory_make ("alawenc", NULL);
    m_rtpRbtPay = gst_element_factory_make("rtppcmapay", NULL);

    if (m_pTime > 10){
      ptime = m_pTime*1000000;
    } else {
      ptime = 10*1000000;
    }
    g_object_set(G_OBJECT(m_rtpRbtPay),"min-ptime",ptime,"max-ptime",ptime,NULL);

    gst_bin_add_many(m_ringbkBin, m_fileSrc, m_wavParse, m_rbtEnc, m_rtpRbtPay, m_rbtpUdp, NULL);
    gst_element_link_many(m_fileSrc, m_wavParse, m_rbtEnc, m_rtpRbtPay, m_rbtpUdp, NULL);

    GstPad *pad = gst_element_get_static_pad (m_fileSrc, "sink");
    if(pad) {
      m_ghostRtpSinkPad = gst_ghost_pad_new("sink", pad);
      gst_element_add_pad (GST_ELEMENT(m_ringbkBin), m_ghostRtpSinkPad);
      gst_object_unref (GST_OBJECT (pad));
    }

    CLog::Debug(THISMODULE,"xGateHmpGstBin::ringbkBin_create: for %s\n", m_ringbkBinName.c_str());

    if(GST_DEBUG) {
      GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN (m_ringbkBin), GST_DEBUG_GRAPH_SHOW_ALL, m_ringbkBinName.c_str());
    }

  return TRUE;
}

GstBin* xGateHmpGstBin::sendBin_create() {
  CLog::Detail(THISMODULE, "xGateHmpGstBin::bin_create bass class implementation return NULL");
  gboolean retVal = FALSE;

  m_sendBin = GST_BIN(gst_bin_new(m_sendBinName.c_str()));
  
  //m_sendUdp = GST_ELEMENT(udpSink->m_udpSink);
  gst_bin_add_many(m_sendBin, m_sendUdp, NULL);
  retVal = gst_element_link_many(GST_ELEMENT(m_sendBin), m_sendUdp,NULL);

  GstPad *pad = gst_element_get_static_pad(m_sendUdp, "sink");
  if(pad) {
    gst_element_add_pad(GST_ELEMENT(m_sendBin),gst_ghost_pad_new("sink", pad));
    gst_object_unref(pad);
  }

  pad = gst_element_get_static_pad(m_sendUdp, "src");
  if(pad) {
    gst_element_add_pad (GST_ELEMENT(m_sendBin), gst_ghost_pad_new ("src", pad));
    gst_object_unref(pad);
  }

  if(GST_DEBUG) {
    GST_DEBUG_BIN_TO_DOT_FILE(m_sendBin, GST_DEBUG_GRAPH_SHOW_ALL, m_sendBinName.c_str());
  }

  return m_sendBin;
}

gboolean xGateHmpGstBin::sendBin_remove()
{
   CLog::Detail(THISMODULE, "xGateHmpGstBin::sendBin_remove Started");

   switch (m_mediaMode)
   {
     case EN_MEDIA_RTP:
        gst_element_unlink_many(m_audioConvert, m_payEnc, m_rtpPay, m_sendUdp, NULL);
        gst_bin_remove_many(m_sendBin, m_audioConvert, m_payEnc, m_rtpPay, m_sendUdp, NULL);
        break;

     case EN_MEDIA_SRTP:
        gst_element_unlink_many(m_audioConvert, m_payEnc, m_rtpPay, m_capsFilter, m_srtpEnc, m_sendUdp, NULL);
        gst_bin_remove_many(m_sendBin, m_audioConvert, m_payEnc, m_rtpPay, m_capsFilter, m_srtpEnc, m_sendUdp, NULL);
        break;

     case EN_MEDIA_DTLS:
        gst_element_unlink_many(m_audioConvert, m_payEnc, m_rtpPay, m_capsFilter, m_dtlssrtpEnc, m_sendUdp, NULL);
        gst_bin_remove_many(m_sendBin, m_audioConvert, m_payEnc, m_rtpPay, m_capsFilter, m_dtlssrtpEnc, m_sendUdp, NULL);
        break;

     default:
      break;
   }
   CLog::Detail(THISMODULE, "xGateHmpGstBin::sendBin_remove Ended");

   return TRUE; 
}

gboolean xGateHmpGstBin::sendBin_update()
{

   switch (m_mediaMode)
   {
     case EN_MEDIA_RTP:
        gst_element_unlink_many(m_audioConvert, m_payEnc, m_rtpPay, m_sendUdp, NULL);
        gst_bin_remove_many(m_sendBin, m_audioConvert, m_payEnc, m_rtpPay, NULL);
        
        gst_element_link_many(GST_ELEMENT(m_sendBin), m_sendUdp, NULL);
        break;

     case EN_MEDIA_SRTP:

        gst_element_unlink_many(m_audioConvert, m_payEnc, m_rtpPay, m_capsFilter, m_srtpEnc, m_sendUdp, NULL);
        gst_bin_remove_many(m_sendBin, m_audioConvert, m_payEnc, m_rtpPay, m_capsFilter, m_srtpEnc, NULL);

        gst_element_link_many(GST_ELEMENT(m_sendBin), m_sendUdp, NULL);
        break;

     case EN_MEDIA_DTLS:

        gst_element_unlink_many(m_audioConvert, m_payEnc, m_rtpPay, m_capsFilter, m_dtlssrtpEnc, m_sendUdp, NULL);
        gst_bin_remove_many(m_sendBin, m_audioConvert, m_payEnc, m_rtpPay, m_capsFilter, m_dtlssrtpEnc, NULL);

        gst_element_link_many(GST_ELEMENT(m_sendBin), m_sendUdp, NULL);
        break;

     default:
      break;
  }

  GstPad *pad = gst_element_get_static_pad (m_sendUdp, "sink");
  if(pad) {
     gst_ghost_pad_set_target((GstGhostPad*)m_ghostRtpSinkPad, pad);
     gst_object_unref (GST_OBJECT (pad));
  }

  if(GST_DEBUG) {
    GST_DEBUG_BIN_TO_DOT_FILE(m_sendBin, GST_DEBUG_GRAPH_SHOW_ALL, m_sendBinName.c_str());
  }
   return TRUE;
}


GstBin * xGateHmpGstBin::recvBin_create() {
  CLog::Detail(THISMODULE, "xGateHmpGstBin::bin_create bass class implementation return NULL");
  gboolean retVal = FALSE;
  GstPad *pad;

  m_recvBin = GST_BIN(gst_bin_new(m_recvBinName.c_str()));
  
  m_rtpJitterBuffer = gst_element_factory_make("rtpjitterbuffer", NULL);

  g_object_set(G_OBJECT(m_rtpJitterBuffer), "latency", latency, NULL);
  g_object_set(G_OBJECT(m_rtpJitterBuffer), "do-lost", TRUE, NULL);

  gst_bin_add_many(m_recvBin, m_recvUdp, m_rtpJitterBuffer, NULL);
  retVal = gst_element_link_many(m_recvUdp, m_rtpJitterBuffer, NULL);

  if(!retVal) {
    CLog::Error(THISMODULE, "HmpG711SendBin::bin_create gst_element_link failed !!");
    gst_object_unref(m_recvBin);
    return NULL;
  }

  pad = gst_element_get_static_pad(m_rtpJitterBuffer, "src");
  if(pad) {
    gst_element_add_pad(GST_ELEMENT(m_recvBin),gst_ghost_pad_new("src", pad));
    gst_object_unref(pad);
  }

  pad = gst_element_get_static_pad(m_recvUdp, "sink");
  if(pad) {
    gst_element_add_pad(GST_ELEMENT(m_recvBin),gst_ghost_pad_new("sink", pad));
    gst_object_unref(pad);
  }

  if(GST_DEBUG) {
    GST_DEBUG_BIN_TO_DOT_FILE(m_recvBin, GST_DEBUG_GRAPH_SHOW_ALL, m_recvBinName.c_str());
  }

  return m_sendBin;
}

gboolean xGateHmpGstBin::recvBin_remove()
{
   CLog::Detail(THISMODULE, "xGateHmpGstBin::recvBin_remove Started");

   switch (m_mediaMode)
   {
     case EN_MEDIA_RTP:
        gst_element_unlink_many(m_recvUdp, m_rtpJitterBuffer, m_rtpDepay, m_payDec, NULL);
        gst_bin_remove_many(m_recvBin, m_recvUdp, m_rtpJitterBuffer, m_rtpDepay, m_payDec, NULL);
        break;

     case EN_MEDIA_SRTP:
        gst_element_unlink_many(m_recvUdp, m_srtpDec, m_rtpJitterBuffer, m_rtpDepay, m_payDec, NULL);
        gst_bin_remove_many(m_recvBin, m_recvUdp, m_srtpDec, m_rtpJitterBuffer, m_rtpDepay, m_payDec, NULL);
        break;

     case EN_MEDIA_DTLS:
        gst_element_unlink_many(m_recvUdp, m_dtlssrtpDec, m_rtpJitterBuffer, m_rtpDepay, m_payDec, NULL);
        gst_bin_remove_many(m_recvBin, m_recvUdp, m_dtlssrtpDec, m_rtpJitterBuffer, m_rtpDepay, m_payDec, NULL);
        break; 
 
     default:
      break;
   }
   CLog::Detail(THISMODULE, "xGateHmpGstBin::recvBin_remove Ended");
   return TRUE;
}

gboolean xGateHmpGstBin::recvBin_update()
{
   switch (m_mediaMode)
   {
     case EN_MEDIA_RTP:
        gst_element_unlink_many(m_recvUdp, m_rtpJitterBuffer, m_rtpDepay, m_payDec, NULL);
        gst_bin_remove_many(m_recvBin, m_rtpDepay, m_payDec, NULL);

        gst_element_link_many(m_recvUdp, m_rtpJitterBuffer, NULL);
        break;

     case EN_MEDIA_SRTP:

        gst_element_unlink_many(m_recvUdp, m_srtpDec, m_rtpJitterBuffer, m_rtpDepay, m_payDec, NULL);
        gst_bin_remove_many(m_recvBin, m_srtpDec, m_rtpDepay, m_payDec, NULL);

        gst_element_link_many(m_recvUdp, m_rtpJitterBuffer, NULL);
        break;

     case EN_MEDIA_DTLS:

        gst_element_unlink_many(m_recvUdp, m_dtlssrtpDec, m_rtpJitterBuffer, m_rtpDepay, m_payDec, NULL);
        gst_bin_remove_many(m_recvBin, m_dtlssrtpDec, m_rtpDepay, m_payDec, NULL);
    
        gst_element_link_many(m_recvUdp, m_rtpJitterBuffer, NULL);
        break;
     default:
      break;
  }
  
  GstPad *pad = gst_element_get_static_pad(m_rtpJitterBuffer, "src");
  if(pad) {
    gst_ghost_pad_set_target((GstGhostPad*)m_ghostRtpSrcPad, pad);
    gst_object_unref(pad);
  }

  if(GST_DEBUG) {
    GST_DEBUG_BIN_TO_DOT_FILE(m_recvBin, GST_DEBUG_GRAPH_SHOW_ALL, m_recvBinName.c_str());
  }
  return TRUE;
}

void xGateHmpGstBin::set_stun_target_client(string client_ip, unsigned int client_port)
{
  CLog::Detail(THISMODULE, "xGateHmpGstBin::set_stun_target_client");
}

void xGateHmpGstBin::on_pad_removed(GstElement *rtpBin, GstPad *pad, gpointer data)
{
  gchar *tmpBuff = NULL;
  tmpBuff = gst_pad_get_name(pad);
  CLog::Detail(THISMODULE, "xGateHmpGstBin::on_pad_removed triggered for pad: %s", tmpBuff);
  g_free(tmpBuff);
}

void xGateHmpGstBin::on_pad_added(GstElement *rtpBin, GstPad *pad, gpointer data)
{
  gchar *tmpBuff = NULL;
  GstPadDirection padDir = GST_PAD_UNKNOWN; 
  unsigned int sn, ssrc = 0;
  int pt = -1;
  GstPad *sinkPad = NULL;
  GstPadLinkReturn isPadLinked;

  tmpBuff = gst_pad_get_name(pad);
  CLog::Detail(THISMODULE, "xGateHmpGstBin::on_pad_added triggered for pad: %s", tmpBuff);
  //recv_rtp_src_0_4239506697_8 or recv_rtp_sink_0 //sample rtp bin pad names
  sscanf(tmpBuff, "recv_rtp_src_%u_%u_%u", &sn, &ssrc, &pt);
  g_free(tmpBuff);

  padDir = gst_pad_get_direction(pad);
  //TODO: Yoga, we can optimize the switch case code blocks
  if(padDir == GST_PAD_SRC) {
    CLog::Detail(THISMODULE, "xGateHmpGstBin::on_pad_added handling GST_PAD_SRC event");
    switch(pt) {
      case EN_PCMU_CODEC:
        {
          CLog::Detail(THISMODULE, "xGateHmpGstBin::on_pad_added handle PCMU codec packets");
          break;
        } 
      case EN_PCMA_CODEC:
        {
          CLog::Detail(THISMODULE, "xGateHmpGstBin::on_pad_added handle PCMA codec packets");
          HmpG711Bin *hmpBin = (HmpG711Bin *)data;
          sinkPad = gst_element_get_static_pad(hmpBin->m_rtpDepay, "sink");
          if(sinkPad) {
            isPadLinked = gst_pad_link(pad, sinkPad);
            gst_object_unref(sinkPad);
            if(isPadLinked != GST_PAD_LINK_OK) {
              CLog::Error(THISMODULE, "Error xGateHmpGstBin::on_pad_added rtpbin and rtpdepay link failed with error: %d", isPadLinked); 
              if(isPadLinked == GST_PAD_LINK_WAS_LINKED) {
                GstPad * peerPad = NULL;
                GstElement * peerElem = NULL;
                peerPad = gst_pad_get_peer(sinkPad);
                if(peerPad && gst_pad_unlink(peerPad, sinkPad)) {
                  isPadLinked = gst_pad_link (pad, sinkPad);
                  //g_print("depayloader sink pad added with newly created rtpbin src host pad!!! \n");
                  //g_print ("newly created rtpbin pad-> %s\n", gst_element_get_name (peerPad));
                  peerElem = GST_ELEMENT(gst_pad_get_parent_element(peerPad));
                  gst_object_unref(peerPad);
                }
              }
            }
          }
          if(GST_DEBUG) {
            GstObject * parentObj = gst_object_get_parent(GST_OBJECT_CAST(hmpBin->m_recvBin));
            if(parentObj) {
              GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(parentObj), GST_DEBUG_GRAPH_SHOW_ALL, gst_object_get_name(parentObj));
              gst_object_unref(parentObj);
            }
          }
          break;
        } 
      case EN_G729_CODEC:
        {
          CLog::Detail(THISMODULE, "xGateHmpGstBin::on_pad_added handle G729 codec packets");
          HmpG729Bin *hmpBin = (HmpG729Bin *)data;
          sinkPad = gst_element_get_static_pad(hmpBin->m_rtpDepay, "sink");
          if(sinkPad) {
            isPadLinked = gst_pad_link(pad, sinkPad);
            gst_object_unref(sinkPad);
            if(isPadLinked != GST_PAD_LINK_OK) {
              CLog::Error(THISMODULE, "Error xGateHmpGstBin::on_pad_added rtpbin and rtpdepay link failed with error: %d", isPadLinked); 
              if(isPadLinked == GST_PAD_LINK_WAS_LINKED) {
                GstPad * peerPad = NULL;
                GstElement * peerElem = NULL;
                peerPad = gst_pad_get_peer(sinkPad);
                if(peerPad && gst_pad_unlink(peerPad, sinkPad)) {
                  isPadLinked = gst_pad_link (pad, sinkPad);
                  //g_print("depayloader sink pad added with newly created rtpbin src host pad!!! \n");
                  //g_print ("newly created rtpbin pad-> %s\n", gst_element_get_name (peerPad));
                  peerElem = GST_ELEMENT(gst_pad_get_parent_element(peerPad));
                  gst_object_unref(peerPad);
                }
              }
            }
          }
          if(GST_DEBUG) {
            GstObject * parentObj = gst_object_get_parent(GST_OBJECT_CAST(hmpBin->m_recvBin));
            if(parentObj) {
              GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(parentObj), GST_DEBUG_GRAPH_SHOW_ALL, gst_object_get_name(parentObj));
              gst_object_unref(parentObj);
            }
          }
          break;
        } 
      default: 
        {
          CLog::Detail(THISMODULE, "xGateHmpGstBin::on_pad_added unsupported codec: %d", pt);
          break;
        }
    }  
  } else if(padDir == GST_PAD_SINK) {
    CLog::Detail(THISMODULE, "xGateHmpGstBin::on_pad_added event for GST_PAD_SINK not yet implemented");
    return;
  } else {
    CLog::Error(THISMODULE, "xGateHmpGstBin::on_pad_added unknow pad direction"); 
    return;
  }

  return;
}

void xGateHmpGstBin::on_request_pt_map(GstElement *rtpBin, guint session, guint pt, gpointer data)
{
  CLog::Detail(THISMODULE, "Handling xGateHmpGstBin::on_request_pt_map event for session: %u, pt: %u", session, pt);
  GstCaps * caps = NULL;
  switch(pt) {
    case 0:
    case 8:
    case 13:
    case 18:
      caps = gst_caps_new_simple("application/x-rtp",
          "media",G_TYPE_STRING,"audio",
          "clock-rate",G_TYPE_INT,8000,
          NULL);
      break;
    case 101:
      caps = gst_caps_new_simple("application/x-rtp",
          "media",G_TYPE_STRING,"audio",
          "clock-rate",G_TYPE_INT,8000,
          "encoding-name",G_TYPE_STRING, "TELEPHONE-EVENT",
          NULL);
      break;
    case 111:
      caps = gst_caps_new_simple("application/x-rtp",
          "media",G_TYPE_STRING,"audio",
          "clock-rate",G_TYPE_INT,48000,
          "encoding-name",G_TYPE_STRING,"X-GST-OPUS-DRAFT-SPITTKA-00",
          NULL);
      break;
    default:
      caps = gst_caps_new_simple("application/x-rtp",
          "media",G_TYPE_STRING,"audio",
          "clock-rate",G_TYPE_INT,8000,
          NULL);
      CLog::Warning(THISMODULE, "on_request_pt_map, no caps setting for unsupported payload type: %u, pt: %u", session, pt);
      break;
  }

  return caps;
}

void xGateHmpGstBin::on_new_ssrc(GstElement *rtpBin, guint session, guint ssrc, gpointer data)
{
  CLog::Detail(THISMODULE, "Handling xGateHmpGstBin::on_new_ssrc event for session: %u, ssrc: %u", session, ssrc);
  return;
}

void xGateHmpGstBin::on_bye_ssrc(GstElement *rtpBin, guint session, guint ssrc, gpointer data)
{
  CLog::Detail(THISMODULE, "Handling xGateHmpGstBin::on_bye_ssrc event for session: %u, ssrc: %u", session, ssrc);
  return;
}

void xGateHmpGstBin::on_ssrc_collision(GstElement *rtpBin, guint session, guint ssrc, gpointer data)
{
  CLog::Detail(THISMODULE, "Handling xGateHmpGstBin::on_ssrc_collision event for session: %u, ssrc: %u", session, ssrc);
}

void xGateHmpGstBin::on_ssrc_validated(GstElement *rtpBin, guint session, guint ssrc, gpointer data)
{
  CLog::Detail(THISMODULE, "Handling xGateHmpGstBin::on_ssrc_validated event for session: %u, ssrc: %u", session, ssrc);
}


/*
 * HmpG711GstBin class implementation start here
 * It will endcode G711 codec and send it to client
 */
HmpG711Bin::HmpG711Bin(string binName) : xGateHmpGstBin(binName, EN_BIN_G711)
{
  CLog::Detail(THISMODULE, "HmpG711Bin::HmpG711Bin");
  m_audioConvert = NULL;
  m_payEnc = NULL;
  m_rtpPay = NULL;
  m_rtpDepay = NULL;
  m_payDec = NULL;
  sockfd = 0;
  
  gchar sendBin[30];
  sprintf(sendBin,"%s_Send", binName.c_str());
  m_sendBinName = sendBin;
  CLog::Detail(THISMODULE, "HmpG711Bin::HmpG711Bin %s",m_sendBinName.c_str());

  gchar recvBin[30];
  sprintf(recvBin,"%s_Recv", binName.c_str());
  m_recvBinName = recvBin;
  CLog::Detail(THISMODULE, "HmpG711Bin::HmpG711Bin %s",m_recvBinName.c_str());
}

HmpG711Bin::~HmpG711Bin()
{
  CLog::Detail(THISMODULE, "HmpG711SendBin::~HmpG711SendBin");
}

gboolean HmpG711Bin::ringbkBin_create()
{
    CLog::Debug(THISMODULE,"xGateHmpGstBin::ringbkBin_create: Start \n");
    m_ringbkBin = GST_BIN(gst_bin_new(m_ringbkBinName.c_str()));
    gint64 ptime;

    m_fileSrc = gst_element_factory_make ("filesrc", NULL);

    m_wavParse = gst_element_factory_make ("wavparse", NULL);
    m_rbtEnc = gst_element_factory_make ("alawenc", NULL);
    m_rtpRbtPay = gst_element_factory_make("rtppcmapay", NULL);

    if (m_pTime > 10){
      ptime = m_pTime*1000000;
    } else {
      ptime = 10*1000000;
    }

    g_object_set(G_OBJECT(m_rtpRbtPay),"min-ptime",ptime,"max-ptime",ptime,NULL);

    gst_bin_add_many(m_ringbkBin, m_fileSrc, m_wavParse, m_rbtEnc, m_rtpRbtPay, m_rbtpUdp, NULL);
    gst_element_link_many(m_fileSrc, m_wavParse, m_rbtEnc, m_rtpRbtPay, m_rbtpUdp, NULL);

    GstPad *pad = gst_element_get_static_pad (m_fileSrc, "sink");
    if(pad) {
      m_ghostRtpSinkPad = gst_ghost_pad_new("sink", pad);
      gst_element_add_pad (GST_ELEMENT(m_ringbkBin), m_ghostRtpSinkPad);
      gst_object_unref (GST_OBJECT (pad));
    }

    CLog::Debug(THISMODULE,"HmpG711Bin::ringbkBin_create: for %s\n", m_ringbkBinName.c_str());

    if(GST_DEBUG) {
      GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN (m_ringbkBin), GST_DEBUG_GRAPH_SHOW_ALL, m_ringbkBinName.c_str());
    }
  return true;
}


GstBin * HmpG711Bin::sendBin_create() 
{
  CLog::Detail(THISMODULE, "HmpG711SendBin::bin_create (creating G711SendBin)");
  gint64 ptime;
  gboolean retVal = FALSE;
 
  m_sendBin = GST_BIN(gst_bin_new(m_sendBinName.c_str()));

  m_audioConvert = gst_element_factory_make("audioconvert", NULL);
  m_payEnc = gst_element_factory_make("alawenc", NULL);
  m_rtpPay = gst_element_factory_make("rtppcmapay", NULL);

  if(!m_audioConvert || !m_payEnc || !m_rtpPay) {
    CLog::Error(THISMODULE, "HmpG711SendBin::bin_create gst_element_make failed !!");
    return NULL;  
  }

  if (m_pTime >= 10){
    ptime = m_pTime*1000000;
  } else {
    ptime = 10*1000000;
  }
  g_object_set(G_OBJECT(m_rtpPay),"min-ptime",ptime,"max-ptime",ptime,NULL);

  switch (m_mediaMode)
  {
     case EN_MEDIA_RTP:  
        gst_bin_add_many(m_sendBin, m_audioConvert, m_payEnc, m_rtpPay, m_sendUdp, NULL);
        retVal = gst_element_link_many(m_audioConvert, m_payEnc, m_rtpPay, m_sendUdp, NULL);
        break;

     case EN_MEDIA_SRTP:
        m_capsFilter=gst_element_factory_make("capsfilter", NULL);
        m_srtpEnc = gst_element_factory_make("srtpenc", NULL);
        g_object_set(G_OBJECT(m_srtpEnc),"key",gstSrtpCaps->srtp_key, NULL);
        g_object_set(G_OBJECT(m_srtpEnc), "rtp-auth",gstSrtpCaps-> srtp_auth, NULL);

        caps = gst_caps_from_string(RTP_CAPS);
        g_object_set(G_OBJECT(m_capsFilter),"caps",caps,NULL);
        gst_caps_unref(caps);

        gst_bin_add_many(m_sendBin, m_audioConvert, m_payEnc, m_rtpPay, m_capsFilter, m_srtpEnc, m_sendUdp, NULL);
        retVal = gst_element_link_many(m_audioConvert, m_payEnc, m_rtpPay, m_capsFilter, m_srtpEnc, m_sendUdp, NULL);
        break;

     case EN_MEDIA_DTLS:
       
        m_capsFilter=gst_element_factory_make("capsfilter", NULL);
        m_dtlssrtpEnc = gst_element_factory_make ("stundtlssrtpenc", NULL);
        if( m_dtlssrtpEnc == NULL )
          CLog::Info(THISMODULE, "\n DTLS_SRTP_PROTO dtlssrtpEnc Factory not created\n");

        g_object_set (G_OBJECT (m_dtlssrtpEnc),"connection-id", m_params.connectId, NULL);
        g_object_set (G_OBJECT (m_dtlssrtpEnc), "is-client", FALSE, NULL);

        caps = gst_caps_from_string(RTP_CAPS);
        g_object_set(G_OBJECT(m_capsFilter),"caps",caps,NULL);
        gst_caps_unref(caps);

        gst_bin_add_many(m_sendBin, m_audioConvert, m_payEnc, m_rtpPay, m_capsFilter, m_dtlssrtpEnc, m_sendUdp, NULL);
        retVal = gst_element_link_many(m_audioConvert, m_payEnc, m_rtpPay, m_capsFilter, m_dtlssrtpEnc, m_sendUdp, NULL);
        break;
     default:
      CLog::Info(THISMODULE, "\n DEFAULT MEDIA PROTO, Receiver side unsupported %d ",m_mediaMode);
      break;
  }

  if(!retVal) {
    CLog::Error(THISMODULE, "HmpG711SendBin::bin_create gst_element_link failed !!");
    gst_object_unref(m_sendBin);
    return NULL;  
  }

  GstPad *pad = gst_element_get_static_pad (m_audioConvert, "sink");
  if(pad) {
     m_ghostRtpSinkPad = gst_ghost_pad_new("sink", pad);
     gst_element_add_pad (GST_ELEMENT(m_sendBin), m_ghostRtpSinkPad);
     gst_pad_set_active(m_ghostRtpSinkPad, TRUE);
     gst_object_unref (GST_OBJECT (pad));
  }

  if(GST_DEBUG) {
    GST_DEBUG_BIN_TO_DOT_FILE(m_sendBin, GST_DEBUG_GRAPH_SHOW_ALL, m_sendBinName.c_str());
  }

  CLog::Detail(THISMODULE, "HmpG711SendBin::bin_create created successfully");
  return m_sendBin; 
}

GstBin * HmpG711Bin::recvBin_create() {
  CLog::Detail(THISMODULE, "HmpG711Bin::recvBin_create (creating G711RecvBin)");
  gboolean retVal = FALSE;

  m_recvBin = GST_BIN(gst_bin_new(m_recvBinName.c_str()));
  
  m_rtpJitterBuffer = gst_element_factory_make("rtpjitterbuffer", NULL);
  m_rtpDepay = gst_element_factory_make("rtppcmadepay", NULL);
  m_payDec = gst_element_factory_make("alawdec", NULL);

  if(!m_rtpJitterBuffer || !m_rtpDepay || !m_payDec) {
    CLog::Error(THISMODULE, "HmpG711RecvBin::bin_create gst_element_make failed !!");
    return NULL;  
  }

  g_object_set(G_OBJECT(m_rtpJitterBuffer), "latency", latency, NULL);
  g_object_set(G_OBJECT(m_rtpJitterBuffer), "do-lost", TRUE, NULL);
  
  switch (m_mediaMode)
  {
    case EN_MEDIA_RTP:

         caps = gst_caps_from_string(UDP_CAPS_G711A);
         g_object_set(G_OBJECT(m_recvUdp), "caps", caps, NULL);
         gst_caps_unref(caps);
         
         gst_bin_add_many(m_recvBin, m_recvUdp, m_rtpJitterBuffer, m_rtpDepay, m_payDec,  NULL);
         retVal = gst_element_link_many(m_recvUdp, m_rtpJitterBuffer, m_rtpDepay, m_payDec, NULL);
         CLog::Detail(THISMODULE, "HmpG711Bin::recvBin_create creating EN_MEDIA_RTP");
         break;

    case EN_MEDIA_SRTP:
         //rcaps = gst_caps_from_string(UDP_CAPS);   // Need to set in UDPSRC
         m_srtpDec = gst_element_factory_make("srtpdec", NULL);
         g_object_set(G_OBJECT(m_recvUdp),"caps", rcaps, NULL);
         gst_caps_unref(rcaps);

         gst_bin_add_many(m_recvBin, m_recvUdp, m_srtpDec, m_rtpJitterBuffer, m_rtpDepay, m_payDec, NULL);
         retVal = gst_element_link_many(m_recvUdp, m_srtpDec, m_rtpJitterBuffer, m_rtpDepay, m_payDec, NULL);      
         break;

    case EN_MEDIA_DTLS:

         rcaps = gst_caps_from_string(UDP_CAPS_DTLS);   
         g_object_set(G_OBJECT(m_recvUdp), "caps", rcaps, NULL);
	 gst_caps_unref(rcaps);

	 m_dtlssrtpDec = gst_element_factory_make("stundtlssrtpdec", NULL);

         if(!m_dtlssrtpDec || !m_rtpJitterBuffer)
            CLog::Info(THISMODULE, "\n DTLS_SRTP_PROTO dtlssrtpDec Factory not created\n");

         g_object_set (G_OBJECT (m_dtlssrtpDec), "connection-id", m_params.connectId, NULL);
         g_object_set (G_OBJECT (m_dtlssrtpDec), "stun-uname", m_params.iceufrag, NULL);
         g_object_set (G_OBJECT (m_dtlssrtpDec), "stun-upass", m_params.icepwd, NULL);
         g_object_set (G_OBJECT (m_dtlssrtpDec), "pem",m_params.pemFile, NULL);

         gst_bin_add_many(m_recvBin, m_recvUdp, m_rtpJitterBuffer, m_dtlssrtpDec, m_rtpDepay, m_payDec, NULL);
         retVal = gst_element_link_many(m_recvUdp, m_rtpJitterBuffer, m_dtlssrtpDec, m_rtpDepay, m_payDec, NULL);
         break;
     default:
         CLog::Info(THISMODULE, "\n DEFAULT MEDIA PROTO, Receiver side unsupported %d ",m_mediaMode);
         break;
  }

  GstPad *pad = gst_element_get_static_pad(m_payDec, "src");
  if(pad) {
    m_ghostRtpSrcPad = gst_ghost_pad_new("src", pad);
    gst_element_add_pad(GST_ELEMENT(m_recvBin), m_ghostRtpSrcPad);
    gst_object_unref(pad);
  }

  if(GST_DEBUG) {
    GST_DEBUG_BIN_TO_DOT_FILE(m_recvBin, GST_DEBUG_GRAPH_SHOW_ALL, m_recvBinName.c_str());
  }

  CLog::Detail(THISMODULE, "HmpG711RecvBin::bin_create created successfully");
  return m_recvBin; 
}

void HmpG711Bin::set_stun_target_client(string client_ip, unsigned int client_port)
{
   sockfd = g_socket_get_fd(m_udpSocket);
   sprintf(targetIp,client_ip.c_str());
   targetPort = client_port;

   g_object_set (G_OBJECT (m_dtlssrtpDec), "sock-des", sockfd, NULL);
   g_object_set (G_OBJECT (m_dtlssrtpDec), "stun-host", targetIp, NULL);
   g_object_set (G_OBJECT (m_dtlssrtpDec), "stun-port", targetPort, NULL);
}

gboolean HmpG711Bin::create_rtpbin()
{
  m_rtpBin = gst_element_factory_make("rtpbin", NULL);
  if(!m_rtpBin) {
    CLog::Error(THISMODULE, "HmpG711RecvBin::create_rtpbin gst_element_make rtpbin failed !!");
    return FALSE;
  }

  g_object_set(G_OBJECT(m_rtpBin),"autoremove", TRUE, NULL);

  g_signal_connect(G_OBJECT(m_rtpBin), "pad-added", G_CALLBACK(&on_pad_added), this);
  g_signal_connect(G_OBJECT(m_rtpBin), "request-pt-map", G_CALLBACK(&on_request_pt_map), this);
  g_signal_connect(G_OBJECT(m_rtpBin), "pad-removed", G_CALLBACK(&on_pad_removed), this);
  /*g_signal_connect(G_OBJECT(m_rtpBin), "on-new-ssrc", G_CALLBACK(&on_new_ssrc), this);
    g_signal_connect(G_OBJECT(m_rtpBin), "on-bye-ssrc", G_CALLBACK(&on_bye_ssrc), this);
    g_signal_connect(G_OBJECT(m_rtpBin), "on-ssrc-collision", G_CALLBACK(&on_ssrc_collision), this);
    g_signal_connect(G_OBJECT(m_rtpBin), "on-ssrc-validated", G_CALLBACK(&on_ssrc_validated), this);*/
  return TRUE;
}


/*
 * HmpG729Bin class implementation start here
 * It will endcode G729 codec and send it to client
 */
HmpG729Bin::HmpG729Bin(string binName) : xGateHmpGstBin(binName, EN_BIN_G729)
{
  CLog::Detail(THISMODULE, "HmpG729Bin::HmpG729Bin");
  m_audioConvert = NULL;
  m_payEnc = NULL;
  m_rtpPay = NULL;
  m_rtpDepay = NULL; 
  m_payDec = NULL;

  gchar sendBin[30];
  sprintf(sendBin,"%s_Send", binName.c_str());
  m_sendBinName = sendBin;

  gchar recvBin[30];
  sprintf(recvBin,"%s_Recv", binName.c_str());
  m_recvBinName = recvBin;
}

HmpG729Bin::~HmpG729Bin()
{
  CLog::Detail(THISMODULE, "HmpG729SendBin::~HmpG729SendBin");
}

gboolean HmpG729Bin::ringbkBin_create()
{
 return TRUE;
}

GstBin * HmpG729Bin::sendBin_create() {
  CLog::Detail(THISMODULE, "HmpG729SendBin::bin_create (creating G729SendBin)");

  gint64 ptime;
  gboolean retVal = FALSE;

  m_recvBin = GST_BIN(gst_bin_new(m_recvBinName.c_str()));

  m_audioConvert = gst_element_factory_make("audioconvert", NULL);
  m_payEnc = gst_element_factory_make("g729enc", NULL);
  m_rtpPay = gst_element_factory_make("rtpg729pay", NULL);

  if(!m_audioConvert || !m_payEnc || !m_rtpPay) {
    CLog::Error(THISMODULE, "HmpG729SendBin::bin_create gst_element_make failed !!");
    return NULL;  
  }

  if (m_pTime >= 10){
    ptime = m_pTime*1000000;
  } else {
    ptime = 10*1000000;
  }

  g_object_set(G_OBJECT(m_rtpPay),"min-ptime",ptime,"max-ptime",ptime,NULL);
  
  switch (m_mediaMode)
  {
     case EN_MEDIA_RTP:
        gst_bin_add_many(m_sendBin, m_audioConvert, m_payEnc, m_rtpPay, m_sendUdp, NULL);
        retVal = gst_element_link_many(m_audioConvert, m_payEnc, m_rtpPay, m_sendUdp, NULL);
        break;

     case EN_MEDIA_SRTP:
        m_capsFilter=gst_element_factory_make("capsfilter", NULL);
        m_srtpEnc = gst_element_factory_make("srtpenc", NULL);
        g_object_set(G_OBJECT(m_srtpEnc),"key",gstSrtpCaps->srtp_key, NULL);
        g_object_set(G_OBJECT(m_srtpEnc), "rtp-auth", srtp_auth, NULL);

        caps = gst_caps_from_string(RTP_CAPS);
        g_object_set(G_OBJECT(m_capsFilter),"caps",caps,NULL);
        gst_caps_unref(caps);

        gst_bin_add_many(m_sendBin, m_audioConvert, m_payEnc, m_rtpPay, m_capsFilter, m_srtpEnc, m_sendUdp, NULL);
        gst_element_link_many(m_audioConvert, m_payEnc, m_rtpPay, m_capsFilter, m_srtpEnc, m_sendUdp, NULL);
        break;

     case EN_MEDIA_DTLS:

        m_capsFilter=gst_element_factory_make("capsfilter", NULL);
        m_dtlssrtpEnc = gst_element_factory_make ("stundtlssrtpenc", NULL);
        if( m_dtlssrtpEnc == NULL )
          CLog::Info(THISMODULE, "\n DTLS_SRTP_PROTO dtlssrtpEnc Factory not created\n");

        g_object_set (G_OBJECT (m_dtlssrtpEnc),"connection-id", m_params.connectId, NULL);
        g_object_set (G_OBJECT (m_dtlssrtpEnc), "is-client", FALSE, NULL);
 
        caps = gst_caps_from_string(RTP_CAPS);
        g_object_set(G_OBJECT(m_capsFilter),"caps",caps,NULL);
        gst_caps_unref(caps);

        gst_bin_add_many(m_sendBin, m_audioConvert, m_payEnc, m_rtpPay, m_capsFilter, m_dtlssrtpEnc, m_sendUdp, NULL);
        gst_element_link_many(m_audioConvert, m_payEnc, m_rtpPay, m_capsFilter, m_dtlssrtpEnc, m_sendUdp, NULL);
        break;

     default:
      CLog::Info(THISMODULE, "\n DEFAULT MEDIA PROTO, Receiver side unsupported %d ",m_mediaMode);
      break;
  }
  if(!retVal) {
    CLog::Error(THISMODULE, "HmpG711SendBin::bin_create gst_element_link failed !!");
    gst_object_unref(m_sendBin);
    return NULL;
  }

  GstPad *pad = gst_element_get_static_pad (m_audioConvert, "sink");
  if(pad) {
     m_ghostRtpSinkPad = gst_ghost_pad_new("sink", pad);
     gst_element_add_pad (GST_ELEMENT(m_sendBin),m_ghostRtpSinkPad );
     gst_pad_set_active(m_ghostRtpSinkPad, TRUE);
     gst_object_unref (GST_OBJECT (pad));
  }

  if(GST_DEBUG) {
    GST_DEBUG_BIN_TO_DOT_FILE(m_sendBin, GST_DEBUG_GRAPH_SHOW_ALL, m_sendBinName.c_str());
  }

  CLog::Detail(THISMODULE, "HmpG729SendBin::bin_create created successfully");
  return m_sendBin; 
}

GstBin * HmpG729Bin::recvBin_create() {
  CLog::Detail(THISMODULE, "HmpG729RecvBin::bin_create (creating G711RecvBin)");
  gboolean retVal = FALSE;

  m_sendBin = GST_BIN(gst_bin_new(m_sendBinName.c_str()));

  m_rtpJitterBuffer = gst_element_factory_make("rtpjitterbuffer", NULL);
  m_rtpDepay = gst_element_factory_make("rtpg729depay", NULL);
  //m_g729Dec = gst_element_factory_make("g729dec", NULL);
  m_payDec = gst_element_factory_make("avdec_g729", NULL);

  if(!m_rtpJitterBuffer || !m_rtpDepay || !m_payDec) {
    CLog::Error(THISMODULE, "HmpG729RecvBin::bin_create gst_element_make failed !!");
    return NULL;  
  }

  g_object_set(G_OBJECT(m_rtpJitterBuffer), "latency", latency, NULL);
  g_object_set(G_OBJECT(m_rtpJitterBuffer), "do-lost", TRUE, NULL);

  switch (m_mediaMode)
  {
    case EN_MEDIA_RTP:
         caps = gst_caps_from_string(UDP_CAPS_G729);
         g_object_set(G_OBJECT(m_recvUdp), "caps", caps, NULL);
         gst_caps_unref(caps);

         gst_bin_add_many(m_recvBin, m_recvUdp, m_rtpJitterBuffer, m_rtpDepay, m_payDec,  NULL);
         retVal = gst_element_link_many(m_recvUdp, m_rtpJitterBuffer, m_rtpDepay, m_payDec, NULL);
         break;

    case EN_MEDIA_SRTP:
         rcaps = gst_caps_from_string(UDP_CAPS_G729);    
         m_srtpDec = gst_element_factory_make("srtpdec", NULL);

         gst_bin_add_many(m_recvBin, m_recvUdp, m_srtpDec, m_rtpJitterBuffer, m_rtpDepay, m_payDec, NULL);
         retVal = gst_element_link_many(m_recvUdp, m_srtpDec, m_rtpJitterBuffer, m_rtpDepay, m_payDec, NULL);
         break;

    case EN_MEDIA_DTLS:

         rcaps = gst_caps_from_string(UDP_CAPS_DTLS);  
         g_object_set(G_OBJECT(m_recvUdp), "caps", rcaps, NULL);
         gst_caps_unref(rcaps);

         m_dtlssrtpDec = gst_element_factory_make("stundtlssrtpdec", NULL);

         if(!m_dtlssrtpDec)
            CLog::Info(THISMODULE, "\n DTLS_SRTP_PROTO dtlssrtpDec Factory not created\n");

         g_object_set (G_OBJECT (m_dtlssrtpDec), "connection-id", m_params.connectId, NULL);
         g_object_set (G_OBJECT (m_dtlssrtpDec), "stun-uname", m_params.iceufrag, NULL);
         g_object_set (G_OBJECT (m_dtlssrtpDec), "stun-upass", m_params.icepwd, NULL);
         g_object_set (G_OBJECT (m_dtlssrtpDec), "pem",m_params.pemFile, NULL);

         gst_bin_add_many(m_recvBin, m_recvUdp, m_rtpJitterBuffer, m_dtlssrtpDec, m_rtpDepay, m_payDec, NULL);
         retVal = gst_element_link_many(m_recvUdp, m_rtpJitterBuffer, m_dtlssrtpDec, m_rtpDepay, m_payDec, NULL);
         break;
     default:
         CLog::Info(THISMODULE, "\n DEFAULT MEDIA PROTO, Receiver side unsupported %d ",m_mediaMode);
         break;
  }

  GstPad *pad = gst_element_get_static_pad(m_payDec, "src");
  if(pad) {
     m_ghostRtpSrcPad = gst_ghost_pad_new("src", pad);
    gst_element_add_pad(GST_ELEMENT(m_recvBin), m_ghostRtpSrcPad);
    gst_object_unref(pad);
  }

  if(GST_DEBUG) {
    GST_DEBUG_BIN_TO_DOT_FILE(m_recvBin, GST_DEBUG_GRAPH_SHOW_ALL, m_recvBinName.c_str());
  }

  CLog::Detail(THISMODULE, "HmpG729RecvBin::bin_create created successfully");
  return m_recvBin; 
}

void HmpG729Bin::set_stun_target_client(string client_ip, unsigned int client_port)
{
   sockfd = g_socket_get_fd(m_udpSocket);
   sprintf(targetIp,client_ip.c_str());
   targetPort = client_port;

   g_object_set (G_OBJECT (m_dtlssrtpDec), "sock-des", sockfd, NULL);
   g_object_set (G_OBJECT (m_dtlssrtpDec), "stun-host", targetIp, NULL);
   g_object_set (G_OBJECT (m_dtlssrtpDec), "stun-port", targetPort, NULL);
}

gboolean HmpG729Bin::create_rtpbin()
{
  m_rtpBin = gst_element_factory_make("rtpbin", NULL);
  if(!m_rtpBin) {
    CLog::Error(THISMODULE, "HmpG729RecvBin::create_rtpbin gst_element_make rtpbin failed !!");
    return FALSE;
  }

  g_object_set(G_OBJECT(m_rtpBin),"autoremove", TRUE, NULL);

  g_signal_connect(G_OBJECT(m_rtpBin), "pad-added", G_CALLBACK(&on_pad_added), this);
  g_signal_connect(G_OBJECT(m_rtpBin), "request-pt-map", G_CALLBACK(&on_request_pt_map), this);
  g_signal_connect(G_OBJECT(m_rtpBin), "pad-removed", G_CALLBACK(&on_pad_removed), this);
  /*g_signal_connect(G_OBJECT(m_rtpBin), "on-new-ssrc", G_CALLBACK(&on_new_ssrc), this);
    g_signal_connect(G_OBJECT(m_rtpBin), "on-bye-ssrc", G_CALLBACK(&on_bye_ssrc), this);
    g_signal_connect(G_OBJECT(m_rtpBin), "on-ssrc-collision", G_CALLBACK(&on_ssrc_collision), this);
    g_signal_connect(G_OBJECT(m_rtpBin), "on-ssrc-validated", G_CALLBACK(&on_ssrc_validated), this);*/
  return TRUE;
}

/*
 *  HmpOPUSBin class implementation start here
 *  It will endcode OPUS codec and send it to client
 */
HmpOpusBin::HmpOpusBin(string binName) : xGateHmpGstBin(binName, EN_BIN_OPUS)
{
  CLog::Detail(THISMODULE, "HmpOpusBin::HmpOpusBin");
  m_audioConvert = NULL;
  m_payEnc = NULL; 
  m_rtpPay = NULL; 
  m_rtpDepay = NULL;
  m_payDec = NULL;

  gchar sendBin[30];
  sprintf(sendBin,"%s_Send", binName.c_str());
  m_sendBinName = sendBin;

  gchar recvBin[30];
  sprintf(recvBin,"%s_Recv", binName.c_str());
  m_recvBinName = recvBin;
}

HmpOpusBin::~HmpOpusBin()
{
  CLog::Detail(THISMODULE, "HmpOpusBin::~HmpOpusBin");
}
gboolean HmpOpusBin::ringbkBin_create()
{
  return TRUE;
}

GstBin * HmpOpusBin::sendBin_create() {
  CLog::Detail(THISMODULE, "HmpOpusSendBin::bin_create (creating OpusSendBin)");
  gint64 ptime;
  int codec;
  gboolean retVal = FALSE;

  m_audioConvert = gst_element_factory_make("audioconvert", NULL);
  m_payEnc = gst_element_factory_make("opusenc", NULL);
  m_rtpPay = gst_element_factory_make("rtpopuspay", NULL);
  
  if(!m_audioConvert || !m_payEnc || !m_rtpPay) {
    CLog::Error(THISMODULE, "HmpOpusSendBin::bin_create gst_element_make failed !!");
    return NULL;
  }

  m_sendBin = GST_BIN(gst_bin_new(m_sendBinName.c_str()));

  if (m_pTime >= 10){
    ptime = m_pTime*1000000;
  } else {
    ptime = 10*1000000;
  }
  g_object_set(G_OBJECT(m_rtpPay),"min-ptime",ptime,"max-ptime",ptime,NULL);

  switch (m_mediaMode)
  {
     case EN_MEDIA_RTP:
        codec = 111;
        g_object_set(G_OBJECT(m_rtpPay),"pt", codec, NULL);

        gst_bin_add_many(m_sendBin, m_audioConvert, m_payEnc, m_rtpPay, m_sendUdp, NULL);
        retVal = gst_element_link_many(m_audioConvert, m_payEnc, m_rtpPay, m_sendUdp, NULL);
        break;

     case EN_MEDIA_SRTP:
        codec = 111;
        g_object_set(G_OBJECT(m_rtpPay),"pt", codec, NULL);

        m_capsFilter=gst_element_factory_make("capsfilter", NULL);
        m_srtpEnc = gst_element_factory_make("srtpenc", NULL);
        g_object_set(G_OBJECT(m_srtpEnc),"key",gstSrtpCaps->srtp_key, NULL);
        g_object_set(G_OBJECT(m_srtpEnc), "rtp-auth", srtp_auth, NULL);

        caps = gst_caps_from_string(UDP_CAPS_OPUS);
        g_object_set(G_OBJECT(m_capsFilter),"caps",caps,NULL);
        gst_caps_unref(caps);

        gst_bin_add_many(m_sendBin, m_audioConvert, m_payEnc, m_rtpPay, m_capsFilter, m_srtpEnc, m_sendUdp, NULL);
        retVal = gst_element_link_many(m_audioConvert, m_payEnc, m_rtpPay, m_capsFilter, m_srtpEnc, m_sendUdp, NULL);
        break;

     case EN_MEDIA_DTLS:
        codec = 111;
        g_object_set(G_OBJECT(m_rtpPay),"pt", codec, NULL);

        m_capsFilter=gst_element_factory_make("capsfilter", NULL);
        m_dtlssrtpEnc = gst_element_factory_make ("stundtlssrtpenc", NULL);
        if( m_dtlssrtpEnc == NULL )
          CLog::Info(THISMODULE, "\n DTLS_SRTP_PROTO dtlssrtpEnc Factory not created\n");

        g_object_set (G_OBJECT (m_dtlssrtpEnc),"connection-id", m_params.connectId, NULL);
        g_object_set (G_OBJECT (m_dtlssrtpEnc), "is-client", FALSE, NULL);
 
        caps = gst_caps_from_string(UDP_CAPS_OPUS);
        g_object_set(G_OBJECT(m_capsFilter),"caps",caps,NULL);
        gst_caps_unref(caps);

        gst_bin_add_many(m_sendBin, m_audioConvert, m_payEnc, m_rtpPay, m_capsFilter, m_dtlssrtpEnc, m_sendUdp, NULL);
        retVal = gst_element_link_many(m_audioConvert, m_payEnc, m_rtpPay, m_capsFilter, m_dtlssrtpEnc, m_sendUdp, NULL);
        break;

     default:
      CLog::Info(THISMODULE, "\n DEFAULT MEDIA PROTO, Receiver side unsupported %d ",m_mediaMode);
      break;
  }
  if(!retVal) {
    CLog::Error(THISMODULE, "HmpOpusSendBin::bin_create gst_element_link failed !!");
    gst_object_unref(m_sendBin);
    return NULL;
  }

  GstPad *pad = gst_element_get_static_pad (m_audioConvert, "sink");
  if(pad) {
     m_ghostRtpSinkPad = gst_ghost_pad_new("sink", pad);
     gst_element_add_pad (GST_ELEMENT(m_sendBin), m_ghostRtpSinkPad);
     gst_pad_set_active(m_ghostRtpSinkPad,TRUE);
     gst_object_unref (GST_OBJECT (pad));
  }

  if(GST_DEBUG) {
    GST_DEBUG_BIN_TO_DOT_FILE(m_sendBin, GST_DEBUG_GRAPH_SHOW_ALL, m_sendBinName.c_str());
  }

  CLog::Detail(THISMODULE, "HmpOpusRecvBin::bin_create created successfully");
  return m_sendBin;
}


GstBin * HmpOpusBin::recvBin_create() {
  CLog::Detail(THISMODULE, "HmpOpusRecvBin::bin_create (creating OpusRecvBin)");
  gboolean retVal = FALSE;

  m_recvBin = GST_BIN(gst_bin_new(m_recvBinName.c_str()));

  m_rtpJitterBuffer = gst_element_factory_make("rtpjitterbuffer", NULL);
  m_rtpDepay = gst_element_factory_make("rtpopusdepay", NULL);
  m_payDec = gst_element_factory_make("opusdec", NULL);

  if(!m_rtpJitterBuffer || !m_rtpDepay || !m_payDec) {
    CLog::Error(THISMODULE, "HmpOpusRecvBin::bin_create gst_element_make failed !!");
    return NULL;
  }

  g_object_set(G_OBJECT(m_rtpJitterBuffer), "latency", latency, NULL);
  g_object_set(G_OBJECT(m_rtpJitterBuffer), "do-lost", TRUE, NULL);

  switch (m_mediaMode)
  {
    case EN_MEDIA_RTP:
         rcaps = gst_caps_from_string(UDP_CAPS_OPUS);
         g_object_set(G_OBJECT(m_recvUdp), "caps", rcaps, NULL);
         gst_caps_unref(rcaps);

         gst_bin_add_many(m_recvBin, m_recvUdp, m_rtpJitterBuffer, m_rtpDepay, m_payDec,  NULL);
         retVal = gst_element_link_many(m_recvUdp, m_rtpJitterBuffer, m_rtpDepay, m_payDec, NULL);
         break;

    case EN_MEDIA_SRTP:
         rcaps = gst_caps_from_string(UDP_CAPS_OPUS);   
         g_object_set(G_OBJECT(m_recvUdp), "caps", rcaps, NULL);
         gst_caps_unref(rcaps);
          
         m_srtpDec = gst_element_factory_make("srtpdec", NULL);

         gst_bin_add_many(m_recvBin, m_recvUdp, m_srtpDec, m_rtpJitterBuffer, m_rtpDepay, m_payDec, NULL);
         retVal = gst_element_link_many(m_recvUdp, m_srtpDec, m_rtpJitterBuffer, m_rtpDepay, m_payDec, NULL);
         break;

    case EN_MEDIA_DTLS:

         rcaps = gst_caps_from_string(UDP_CAPS_DTLS);
         g_object_set(G_OBJECT(m_recvUdp), "caps", rcaps, NULL);
	 gst_caps_unref(rcaps);

         m_dtlssrtpDec = gst_element_factory_make("stundtlssrtpdec", NULL);
         m_rtpJitterBuffer = gst_element_factory_make("rtpjitterbuffer", NULL);

         if(!m_dtlssrtpDec || !m_rtpJitterBuffer)
            CLog::Info(THISMODULE, "\n DTLS_SRTP_PROTO dtlssrtpDec Factory not created\n");

         g_object_set (G_OBJECT (m_dtlssrtpDec), "connection-id", m_params.connectId, NULL);
         g_object_set (G_OBJECT (m_dtlssrtpDec), "stun-uname", m_params.iceufrag, NULL);
         g_object_set (G_OBJECT (m_dtlssrtpDec), "stun-upass", m_params.icepwd, NULL);
         g_object_set (G_OBJECT (m_dtlssrtpDec), "pem",m_params.pemFile, NULL);

         gst_bin_add_many(m_recvBin, m_recvUdp, m_dtlssrtpDec, m_rtpDepay, m_payDec, NULL);
         retVal = gst_element_link_many(m_recvUdp,  m_dtlssrtpDec, m_rtpDepay, m_payDec, NULL);
         break;
     default:
         CLog::Info(THISMODULE, "\n DEFAULT MEDIA PROTO, Receiver side unsupported %d ",m_mediaMode);
         break;
  }

  GstPad *pad = gst_element_get_static_pad(m_payDec, "src");
  if(pad) {
    m_ghostRtpSrcPad = gst_ghost_pad_new("src", pad);
    gst_element_add_pad(GST_ELEMENT(m_recvBin), m_ghostRtpSrcPad);
    gst_pad_set_active(m_ghostRtpSrcPad, TRUE);
    gst_object_unref(pad);
  }

  if(GST_DEBUG) {
    GST_DEBUG_BIN_TO_DOT_FILE(m_recvBin, GST_DEBUG_GRAPH_SHOW_ALL, m_recvBinName.c_str());
  }   
  CLog::Detail(THISMODULE, "HmpG729RecvBin::bin_create created successfully");
  return m_sendBin;
}

 
void HmpOpusBin::set_stun_target_client(string client_ip, unsigned int client_port)
{
   sockfd = g_socket_get_fd(m_udpSocket);
   sprintf(targetIp,client_ip.c_str());
   targetPort = client_port;

   g_object_set (G_OBJECT (m_dtlssrtpDec), "sock-des", sockfd, NULL);
   g_object_set (G_OBJECT (m_dtlssrtpDec), "stun-host", targetIp, NULL);
   g_object_set (G_OBJECT (m_dtlssrtpDec), "stun-port", targetPort, NULL);
}

xGateHmpGstBin * HmpGstBinFactory::bin_create(string& binName, HmpGstBinType binType, ClientDetail &cliDetail)
{

  CLog::Detail(THISMODULE, "Enter HmpGstFactory::bin_create");
  xGateHmpGstBin *pHmpGstBin = NULL;
  switch(binType) {

    case EN_BIN_RTP:
        CLog::Detail(THISMODULE, "HmpGstFactory::bin_create creating RTP_BIN");
        pHmpGstBin = new xGateHmpGstBin(binName, binType);
        pHmpGstBin->create_udp_src_sink(cliDetail.m_codec, cliDetail.m_serverPort);
        pHmpGstBin->set_server_addr(cliDetail.m_serverIp, cliDetail.m_serverPort);
        if(cliDetail.m_mediaMode == EN_MEDIA_DTLS) {
          pHmpGstBin->set_client_addr(cliDetail.m_relayIpaddr, cliDetail.m_relayPort);
        } else if(cliDetail.m_mediaMode == EN_MEDIA_RTP) {
          pHmpGstBin->set_client_addr(cliDetail.m_ip, cliDetail.m_port);
        }
        pHmpGstBin->set_client_details(cliDetail);
        pHmpGstBin->sendBin_create();
        pHmpGstBin->recvBin_create();
        break;

    case EN_BIN_G711:
        CLog::Detail(THISMODULE, "HmpGstFactory::bin_create creating G711_BIN");
        pHmpGstBin = new HmpG711Bin(binName);
        pHmpGstBin->create_udp_src_sink(cliDetail.m_codec, cliDetail.m_serverPort);
        pHmpGstBin->set_server_addr(cliDetail.m_serverIp, cliDetail.m_serverPort);
        if(cliDetail.m_mediaMode == EN_MEDIA_DTLS) {
          pHmpGstBin->set_client_addr(cliDetail.m_relayIpaddr, cliDetail.m_relayPort);
        } else if(cliDetail.m_mediaMode == EN_MEDIA_RTP) {
          pHmpGstBin->set_client_addr(cliDetail.m_ip, cliDetail.m_port);
        }
        pHmpGstBin->set_client_details(cliDetail);
        pHmpGstBin->sendBin_create();
        pHmpGstBin->recvBin_create();
        break;

    case EN_BIN_G729:
        CLog::Detail(THISMODULE, "HmpGstFactory::bin_create creating G729_BIN");
        pHmpGstBin = new HmpG729Bin(binName);
        pHmpGstBin->create_udp_src_sink(cliDetail.m_codec, cliDetail.m_serverPort);
        pHmpGstBin->set_server_addr(cliDetail.m_serverIp, cliDetail.m_serverPort);
        if(cliDetail.m_mediaMode == EN_MEDIA_DTLS) {
          pHmpGstBin->set_client_addr(cliDetail.m_relayIpaddr, cliDetail.m_relayPort);
        } else if(cliDetail.m_mediaMode == EN_MEDIA_RTP) {
          pHmpGstBin->set_client_addr(cliDetail.m_ip, cliDetail.m_port);
        }
        pHmpGstBin->set_client_details(cliDetail);
        pHmpGstBin->sendBin_create();
        pHmpGstBin->recvBin_create();
        break;
    case EN_BIN_OPUS:
        CLog::Detail(THISMODULE, "HmpGstFactory::bin_create creating OPUS_BIN");
        pHmpGstBin = new HmpOpusBin(binName);
        pHmpGstBin->create_udp_src_sink(cliDetail.m_codec, cliDetail.m_serverPort);
        pHmpGstBin->set_server_addr(cliDetail.m_serverIp, cliDetail.m_serverPort);
        if(cliDetail.m_mediaMode == EN_MEDIA_DTLS) {
          pHmpGstBin->set_client_addr(cliDetail.m_relayIpaddr, cliDetail.m_relayPort);
        } else if(cliDetail.m_mediaMode == EN_MEDIA_RTP) {
          pHmpGstBin->set_client_addr(cliDetail.m_ip, cliDetail.m_port);
        }
        pHmpGstBin->set_client_details(cliDetail);
        pHmpGstBin->recvBin_create();
        pHmpGstBin->sendBin_create();
        break;
    case EN_BIN_UNKNOWN:
    default:
        CLog::Error(THISMODULE, "HmpGstFactory::bin_create unable to create UNKNOWN_BIN type");
        break;
  }

  if(!pHmpGstBin) {
    CLog::Error(THISMODULE, "HmpGstFactory::bin_create failed!!");
  }
  return pHmpGstBin;  
}
