
//local include
#include "log.h"
#include "klog.h"

//self include
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
  m_voHmpBinA = NULL;
  m_voTrmBinA = NULL;
  m_voTrmBinB = NULL;
  m_voHmpBinB = NULL;
}

xGateHmpGstPipeline::~xGateHmpGstPipeline()
{
  CLog::Detail(THISMODULE, "xGateHmpGstPipeline::~xGateHmpGstPipeline");
}

gboolean xGateHmpGstPipeline::support_comfort_noise(Client *sClient, Client *dClient)
{
  GstElement *cnfUdpSinkA, *cnfUdpSinkB = NULL;
  gchar directClientA[50] = {0};
  gchar directClientB[50] = {0};

  snprintf(directClientA, sizeof(directClientA), "%s:%u", \
      sClient->m_voiceChannel->m_voiceBin->m_clientIp.c_str(), \
      sClient->m_voiceChannel->m_voiceBin->m_clientPort);
  snprintf(directClientB, sizeof(directClientB), "%s:%u", \
      dClient->m_voiceChannel->m_voiceBin->m_clientIp.c_str(), \
      dClient->m_voiceChannel->m_voiceBin->m_clientPort);

  cnfUdpSinkA = sClient->m_voiceChannel->m_voiceBin->m_cnfUdpSink;
  if(cnfUdpSinkA) {
    g_object_set(G_OBJECT(cnfUdpSinkA), "clients", directClientB, NULL);
    g_object_set(G_OBJECT(cnfUdpSinkA), "socket", dClient->m_voiceChannel->m_voiceBin->m_udpSocket, NULL);
    g_object_set(G_OBJECT(cnfUdpSinkA), "close-socket", FALSE, NULL);
  }

  cnfUdpSinkB = dClient->m_voiceChannel->m_voiceBin->m_cnfUdpSink;
  if(cnfUdpSinkB) {
    g_object_set(G_OBJECT(cnfUdpSinkB), "clients", directClientA, NULL);
    g_object_set(G_OBJECT(cnfUdpSinkB), "socket", sClient->m_voiceChannel->m_voiceBin->m_udpSocket, NULL);
    g_object_set(G_OBJECT(cnfUdpSinkB), "close-socket", FALSE, NULL);
  }
  if(GST_DEBUG) {
    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(sClient->m_voiceChannel->m_sendVoicePipeline), \
        GST_DEBUG_GRAPH_SHOW_ALL, sClient->m_voiceChannel->m_sendVoicePipelineName.c_str());
    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(sClient->m_voiceChannel->m_recvVoicePipeline), \
        GST_DEBUG_GRAPH_SHOW_ALL, sClient->m_voiceChannel->m_recvVoicePipelineName.c_str());
  }
}

gboolean xGateHmpGstPipeline::pipeline_connect_clients(Client *sClient, Client *dClient)
{
  printf("**** pipeline_connect_clients - start \n");
  CLog::Detail(THISMODULE, "xGateHmpGstPipeline::pipeline_connect_clients start");
  gchar clintaddA[50];
  gchar clintaddB[50];

  m_voTrmBinA = sClient->m_voiceChannel->m_termBin;
  if(!m_voTrmBinA) {
    return false;
  }
  m_voTrmPortA = m_voTrmBinA->m_termPort;
  snprintf(clintaddA, sizeof(clintaddA), "%s:%u", "127.0.0.1", m_voTrmPortA);
  
  m_voTrmBinB = dClient->m_voiceChannel->m_termBin;
  if(!m_voTrmBinB) {
    return false;
  }
  m_voTrmPortB = m_voTrmBinB->m_termPort;
  snprintf(clintaddB, sizeof(clintaddB), "%s:%u", "127.0.0.1", m_voTrmPortB);

#if 1 //YOGA
  if(sClient->m_detail.m_codec == EN_PCMA_CODEC && dClient->m_detail.m_codec == EN_PCMA_CODEC) {
    support_comfort_noise(sClient, dClient);
  }
#endif

  g_object_set(G_OBJECT(m_voTrmBinA->m_udpSend),"clients", clintaddB, NULL);

  if(sClient->m_voiceChannel->m_sendVoicePipeline) {
    gst_element_set_state(sClient->m_voiceChannel->m_sendVoicePipeline, GST_STATE_PLAYING);
  }

  if(sClient->m_voiceChannel->m_recvVoicePipeline) {
    gst_element_set_state(sClient->m_voiceChannel->m_recvVoicePipeline,GST_STATE_PLAYING);
  }

  g_object_set(G_OBJECT(m_voTrmBinB->m_udpSend),"clients", clintaddA, NULL);
  
  if(dClient->m_voiceChannel->m_sendVoicePipeline) {
    gst_element_set_state(dClient->m_voiceChannel->m_sendVoicePipeline, GST_STATE_PLAYING);
  }

  if(dClient->m_voiceChannel->m_recvVoicePipeline) {
    gst_element_set_state(dClient->m_voiceChannel->m_recvVoicePipeline,GST_STATE_PLAYING);
  }
  return true;
}

gboolean xGateHmpGstPipeline::disconnect_simple_client(Client *sClient, Client *dClient)
{
  CLog::Detail(THISMODULE, "xGateHmpGstPipeline::disconnect_simple_client");

  if(sClient->m_voiceChannel->m_sendVoicePipeline) {
     gst_element_set_state(GST_ELEMENT(sClient->m_voiceChannel->m_sendVoicePipeline), GST_STATE_NULL);
  }
  if(sClient->m_voiceChannel->m_recvVoicePipeline) {
     gst_element_set_state(GST_ELEMENT(sClient->m_voiceChannel->m_recvVoicePipeline), GST_STATE_NULL);
  }

  if(dClient->m_voiceChannel->m_sendVoicePipeline) {
    gst_element_set_state(GST_ELEMENT(dClient->m_voiceChannel->m_sendVoicePipeline), GST_STATE_NULL);
  }
  if(dClient->m_voiceChannel->m_recvVoicePipeline) {
    gst_element_set_state(GST_ELEMENT(dClient->m_voiceChannel->m_recvVoicePipeline), GST_STATE_NULL);
  }

  CLog::Detail(THISMODULE, "xGateHmpGstPipeline::disconnect_simple_client end");
  return true;
}


HmpGstVoiceChannel::HmpGstVoiceChannel(string pipelineName)
{
  CLog::Detail(THISMODULE, "HmpGstVoiceChannel::HmpGstVoiceChannel %s", pipelineName.c_str());

  m_sendVoiceBusId = 0;
  m_recvVoiceBusId = 0;
  gchar sendVoicePipeline[30];
  sprintf(sendVoicePipeline,"Send_%s", pipelineName.c_str());
  m_sendVoicePipelineName = sendVoicePipeline;

  gchar recvVoicePipeline[30];
  sprintf(recvVoicePipeline,"Recv_%s", pipelineName.c_str());
  m_recvVoicePipelineName = recvVoicePipeline;

  m_binFactory = NULL;
  m_voiceBin = NULL;
  m_termBin = NULL;
  m_plTrmBin = NULL;
  m_recvVoicBin = NULL; 
  m_sendTermBin = NULL;
  m_sendVoicBin = NULL;
  m_recvTermBin = NULL;
  m_sendVoicePipeline = NULL;
  m_sendVoiceBus = NULL;
  m_recvVoicePipeline = NULL;
  m_recvVoiceBus = NULL;
  memset(m_dtmfBuffer,'\0', sizeof(m_dtmfBuffer));
}

HmpGstVoiceChannel::~HmpGstVoiceChannel()
{
  CLog::Detail(THISMODULE, "HmpGstVoiceChannel::~HmpGstVoiceChannel");
 
  if(m_sendVoicePipeline) {
    gst_element_set_state(GST_ELEMENT(m_sendVoicePipeline), GST_STATE_NULL);
    gst_object_unref (GST_OBJECT (m_sendVoicePipeline));
    CLog::Detail(THISMODULE, "HmpGstVoiceChannel::~HmpGstVoiceChannel %s", m_sendVoicePipelineName.c_str());
  }

  if(m_recvVoicePipeline) {
    gst_element_set_state(GST_ELEMENT(m_recvVoicePipeline), GST_STATE_NULL);
    gst_object_unref (GST_OBJECT (m_recvVoicePipeline));
    CLog::Detail(THISMODULE, "HmpGstVoiceChannel::~HmpGstVoiceChannel %s", m_recvVoicePipelineName.c_str());
  }

  if(m_sendVoiceBus) {
    gst_bus_set_sync_handler(m_sendVoiceBus, NULL, NULL, NULL);
    gst_object_unref(GST_OBJECT (m_sendVoiceBus));
  }

  if(m_recvVoiceBus) {
    gst_bus_set_sync_handler(m_recvVoiceBus, NULL, NULL, NULL);
    gst_object_unref(GST_OBJECT (m_recvVoiceBus));
  }
}

gboolean HmpGstVoiceChannel::create_voice_channel()
{
  CLog::Detail(THISMODULE, "Enter HmpGstVoiceChannel::create_voice_channel ");

  gboolean retVal = FALSE;
  m_sendVoicePipeline = gst_pipeline_new(m_sendVoicePipelineName.c_str());
  m_recvVoicePipeline = gst_pipeline_new(m_recvVoicePipelineName.c_str());

  if(!m_sendVoicePipeline || !m_recvVoicePipeline) {
    CLog::Error(THISMODULE, "HmpGstVoiceChannel::create_voice_channel failed !!");
    retVal = FALSE;
  } else retVal = TRUE;

  m_sendVoiceBus = gst_pipeline_get_bus(GST_PIPELINE(m_sendVoicePipeline));
  m_recvVoiceBus = gst_pipeline_get_bus(GST_PIPELINE(m_recvVoicePipeline));

  CLog::Detail(THISMODULE, "Enter HmpGstVoiceChannel::create_voice_channel successfull");

  return retVal;
}


gboolean HmpGstVoiceChannel::activate_voice_channel(Client *sClient)
{
  CLog::Detail(THISMODULE, "HmpGstVoiceChannel::activate_voice_channel in %s", m_sendVoicePipelineName.c_str());
  gchar clintadd[50];

  m_plTrmBin = sClient->m_playChannel->m_pbTermBin;
  m_plTermPort = m_plTrmBin->m_termPort;
  snprintf(clintadd, sizeof(clintadd), "%s:%u", "127.0.0.1", m_plTermPort);

  m_voiceBin = sClient->m_voiceChannel->m_voiceBin;
  m_termBin = sClient->m_voiceChannel->m_termBin;

  if(!m_voiceBin || !m_termBin)
  {
    CLog::Detail(THISMODULE, "xGateHmpGstChannel::activate_voice_channel failed wilt Bin");
    return FALSE;
  }
  m_recvVoicBin = GST_ELEMENT(m_voiceBin->m_recvBin);
  m_sendTermBin = GST_ELEMENT(m_termBin->get_termSendBin());
  m_sendVoicBin = GST_ELEMENT(m_voiceBin->m_sendBin);
  m_recvTermBin = GST_ELEMENT(m_termBin->get_termRecvBin());

  m_sendVoicePipeline = sClient->m_voiceChannel->m_sendVoicePipeline;
  m_recvVoicePipeline = sClient->m_voiceChannel->m_recvVoicePipeline;

  gst_bin_add_many(GST_BIN(m_sendVoicePipeline), m_recvVoicBin, m_sendTermBin, NULL);
  gst_element_link_many(m_recvVoicBin, m_sendTermBin, NULL);

  gst_bin_add_many(GST_BIN(m_recvVoicePipeline), m_recvTermBin, m_sendVoicBin, NULL);
  gst_element_link_many(m_recvTermBin, m_sendVoicBin, NULL);
  
  m_sendVoiceBus = sClient->m_voiceChannel->m_sendVoiceBus;

  m_sendVoiceBusId = gst_bus_add_watch(m_sendVoiceBus, &HmpGstVoiceChannel::handle_event_dispatcher, sClient);

  g_object_set(G_OBJECT(m_termBin->m_udpSend),"clients", clintadd, NULL);

  gst_element_set_state(m_sendVoicePipeline, GST_STATE_PLAYING);
  gst_element_set_state(m_recvVoicePipeline, GST_STATE_PLAYING);

  if(GST_DEBUG) {
    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(m_sendVoicePipeline), GST_DEBUG_GRAPH_SHOW_ALL, m_sendVoicePipelineName.c_str());
    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(m_recvVoicePipeline), GST_DEBUG_GRAPH_SHOW_ALL, m_recvVoicePipelineName.c_str());
  }

  return TRUE;
}

gboolean HmpGstVoiceChannel::deactivate_voice_channel(Client *sClient)
{

  CLog::Detail(THISMODULE, "HmpGstVoiceChannel::activate_voice_channel in %s", m_sendVoicePipelineName.c_str());
  if(!sClient->m_voiceChannel) {
      CLog::Detail(THISMODULE, "xGateHmpGstChannel::activate_voice_channel failed wilt Bin");
      return FALSE;
  }

  m_sendVoicePipeline = sClient->m_voiceChannel->m_sendVoicePipeline;
  m_recvVoicePipeline = sClient->m_voiceChannel->m_recvVoicePipeline;

  if(m_sendVoicePipeline) {
    gst_element_set_state(GST_ELEMENT(m_sendVoicePipeline), GST_STATE_NULL);
  }
  if(m_recvVoicePipeline) {
    gst_element_set_state(GST_ELEMENT(m_recvVoicePipeline), GST_STATE_NULL);
  }
  return TRUE;

}

gboolean  HmpGstVoiceChannel::handle_event_dispatcher(GstBus* bus, GstMessage* msg, gpointer data)
{
  char dtmfDigit[10];
  gint number;
  const GstStructure* structure;

  Client *client = (Client *)data;
  HmpGstVoiceChannel *hmpVoiceChannel = client->m_voiceChannel;
  HmpGstPlayChannel *hmpPlayChannel = client->m_playChannel;
  GstElement *pBackRecvPipeline = hmpPlayChannel->m_pbRecvPipeline;
  GstElement *pBackSendPipeline = hmpPlayChannel->m_pbSendPipeline;
  
  switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_ERROR:
      {
        CLog::Detail(THISMODULE, "HmpGstVoiceChannel::handle_event_dispatcher got GST_MESSAGE_ERROR message");
        GError *err = NULL;
        gchar *dbgInfo = NULL;
        gst_message_parse_error(msg, &err, &dbgInfo);
        if(err) {
          g_printerr("ERROR from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
          CLog::Error(THISMODULE, "ERROR from element %s: %s", GST_OBJECT_NAME(msg->src), err->message);
          g_error_free(err);
        }
        if(dbgInfo) {
          CLog::Error(THISMODULE, "Debugging info: %s\n", dbgInfo);
          g_free(dbgInfo);
        }
        break;
      }

    case GST_MESSAGE_ELEMENT:
      {
        structure=gst_message_get_structure(msg);
        if(!strcmp(gst_structure_get_name (structure),"dtmf-event"))
        {
           gst_structure_get_int(structure, "number", &number);
        }
        snprintf(dtmfDigit, sizeof(dtmfDigit), "%d", number);
        if (strcmp(dtmfDigit,"11") == 0) {
            client->m_detail.m_pbState = EN_XGATE_PB_RECVDIGIT;
            strcpy(client->m_detail.m_dtmfBuffer.c_str(), hmpVoiceChannel->m_dtmfBuffer);
            CLog::Detail(THISMODULE,"HmpGstVoiceChannel::handle_event_dispatcher : until # in DTMF digits %s ", hmpVoiceChannel->m_dtmfBuffer);
            hmpVoiceChannel->post_playback_eos(client);
            memset(hmpVoiceChannel->m_dtmfBuffer,'\0', sizeof(hmpVoiceChannel->m_dtmfBuffer));
        } else {
            gst_element_set_state(pBackRecvPipeline, GST_STATE_NULL);
            gst_element_set_state(pBackSendPipeline, GST_STATE_NULL);
            strcat(hmpVoiceChannel->m_dtmfBuffer, dtmfDigit);
            CLog::Detail(THISMODULE,"HmpGstVoiceChannel::handle_event_dispatcher : pressed DTMF digits %s  Copy in %s", dtmfDigit, hmpVoiceChannel->m_dtmfBuffer);
        }
        break;
      }
    case GST_MESSAGE_UNKNOWN:
    default:
      {
        //CLog::Detail(THISMODULE, "HmpGstVoiceChannel::handle_event_dispatcher got GST_MESSAGE_UNKNOWN message");
        break;
      }
  }
  return TRUE; //it's important return TRUE from this function
}

void HmpGstVoiceChannel::get_dtmf_digit(Client *client)
{
    memset(m_dtmfBuffer,'\0', sizeof(m_dtmfBuffer));
    client->m_detail.m_dtmfBuffer.erase(0, string::npos);
}

void HmpGstVoiceChannel::send_dtmf_digit(Client *client)
{
    client->m_detail.m_pbState = EN_XGATE_PB_RECVDIGIT;
    strcpy(client->m_detail.m_dtmfBuffer.c_str(), m_dtmfBuffer);
    memset(m_dtmfBuffer,'\0', sizeof(m_dtmfBuffer));
}

void HmpGstVoiceChannel::post_playback_eos(Client *client)
{
  CLog::Detail(THISMODULE,"HmpGstVoiceChannel::post_playback_eos Posting EOS ");
  cout << endl << "HmpGstVoiceChannel::post_playback_eos Posting EOS " << endl;
  xGateMgMsg *pMgMsg = new xGateMgMsg();
  MgMediaDetail mediaDetail;

  pMgMsg->setMsgType(EN_XGATE_MSG_MGC_MG_IN);

  mediaDetail.call_id = client->m_detail.m_callId;
  mediaDetail.leg_id = client->m_detail.m_legid;
//  mediaDetail.ip_addr = client->m_detail.m_ip;
//  mediaDetail.port = client->m_detail.m_port;
//  mediaDetail.codec = client->m_detail.m_codec;
//  mediaDetail.ptime = client->m_detail.m_pTime;
  mediaDetail.pb_state = client->m_detail.m_pbState;
  mediaDetail.dtmf_digits = client->m_detail.m_dtmfBuffer;
  mediaDetail.media_proto = client->m_detail.m_mediaMode;
  mediaDetail.out_proto = client->m_detail.m_outProto;
  mediaDetail.call_dir = client->m_detail.m_callDir;

  pMgMsg->set_media_detail(mediaDetail);

  ACE_Task<ACE_MT_SYNCH>* taskPtr = NULL;
  taskPtr = (xGateSipSignalProcessor*)GetSipPostHandler(XGATEPOSTTYPE::EN_PROCESS);
  if(!taskPtr) {
    CLog::Error(THISMODULE, "HmpGstVoiceChannel::post_playback_eos SipSignalProcessor task not found");
    return EN_XGATE_STATUS_ERROR;
  }

  ACE_Message_Block* pAmb = NULL;
  pAmb = static_cast<xGateMgMsg*> (pMgMsg);
  ACE_Time_Value tval ((time(NULL) ));
  tval += ACE_Time_Value(0,1);

  if (taskPtr->putq(pAmb, &tval)) {
    CLog::Detail(THISMODULE, "HmpGstVoiceChannel::post_playback_eos SipSignalProcessor enqueue message success");
  } else {
    CLog::Error(THISMODULE, "HmpGstVoiceChannel::post_playback_eos SipSignalProcessor enqueue message failed");
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
        //pHmpGstPipeline->simple_pipeline_create();
        break;

    case EN_PIPELINE_CONF_CALL:
        CLog::Detail(THISMODULE, "HmpGstPipelineFactory::conf_pipeline_create creating PIPELINE_CONF_CALL");
        pHmpGstPipeline = new xGateHmpGstPipeline(pipelineName, pipelineType);
        //pHmpGstPipeline->conf_pipeline_create();
        break;

    case EN_PIPELINE_UNKNOWN:
    default:
        CLog::Detail(THISMODULE, "HmpGstPipelineFactory::pipeline_create unable to create UNKNOWN_BIN type");
        break;
  }

  return pHmpGstPipeline;
}

/*
 *  HmpGstPlayBackPipeline class for playing file - Start
 */
HmpGstPlayChannel::HmpGstPlayChannel(string pipelineName, HmpGstPipelineType pipelineType) :
  m_pbSendPipelineType(pipelineType), m_pbSendPipeline(NULL), m_pbRecvPipeline(NULL), m_pbSendBus(NULL), 
  m_pbRecvBus(NULL), m_pbSendBusId(0), m_hmpPlayFileBin(NULL), m_autoRepeat(FALSE)
{
  gchar pbSendPipeline[30];
  sprintf(pbSendPipeline, "Send_%s", pipelineName.c_str());
  m_pbSendPipelineName = pbSendPipeline;

  gchar pbRecvPipeline[30];
  sprintf(pbRecvPipeline, "Recv_%s", pipelineName.c_str());
  m_pbRecvPipelineName = pbRecvPipeline;

  
}

HmpGstPlayChannel::~HmpGstPlayChannel()
{
  cout << endl << "HmpGstPlayChannel::~HmpGstPlayChannel start " << endl;

  if(m_pbSendPipeline) {
    gst_element_set_state(m_pbSendPipeline, GST_STATE_NULL);
    gst_object_unref (GST_OBJECT (m_pbSendPipeline));
    CLog::Detail(THISMODULE, "HmpGstPlayChannel::~HmpGstPlayChannel %s", m_pbSendPipelineName.c_str());
  }

  if(m_pbRecvPipeline) {
    gst_element_set_state(m_pbRecvPipeline, GST_STATE_NULL);
    gst_object_unref (GST_OBJECT (m_pbRecvPipeline));
    CLog::Detail(THISMODULE, "HmpGstPlayChannel::~HmpGstPlayChannel %s", m_pbRecvPipelineName.c_str());
  }

  if(m_pbSendBus) {
    gst_bus_set_sync_handler(m_pbSendBus, NULL, NULL, NULL);
    gst_object_unref(GST_OBJECT (m_pbSendBus));
  }

  if(m_pbRecvBus) {
    gst_bus_set_sync_handler(m_pbRecvBus, NULL, NULL, NULL);
    gst_object_unref(GST_OBJECT (m_pbRecvBus));
  }

  cout << endl << "HmpGstPlayChannel::~HmpGstPlayChannel end " << endl;
}

gboolean HmpGstPlayChannel::create()
{
  CLog::Detail(THISMODULE, "Enter HmpGstPlayBackPipeline::create");

  m_pbSendPipeline = gst_pipeline_new(m_pbSendPipelineName.c_str());
  m_pbRecvPipeline = gst_pipeline_new(m_pbRecvPipelineName.c_str());
  

  if(!m_pbSendPipeline && !m_pbRecvPipeline) {
    CLog::Error(THISMODULE, "HmpGstPlayBackPipeline::create failed!!");
    return FALSE;
  }

  m_pbRecvBus = gst_pipeline_get_bus(GST_PIPELINE(m_pbRecvPipeline));
  m_pbSendBus = gst_pipeline_get_bus(GST_PIPELINE(m_pbSendPipeline));
  CLog::Detail(THISMODULE, "HmpGstPlayBackPipeline::create successfull");

  return TRUE; 
}

gboolean HmpGstPlayChannel::setting_playfile_channel(Client *sClient)
{
  GstPad *sinkPad, *srcPad = NULL;
  gchar clintadd[50];

  m_pbVoTrmBin = sClient->m_voiceChannel->m_termBin;
  m_pbTermPort = m_pbVoTrmBin->m_termPort;
  snprintf(clintadd, sizeof(clintadd), "%s:%u", "127.0.0.1", m_pbTermPort);

  m_pbTermBin = sClient->m_playChannel->m_pbTermBin;
  m_hmpPlayFileBin = sClient->m_playChannel->m_hmpPlayFileBin;

  if (!m_pbTermBin || !m_hmpPlayFileBin) {
     return false; 
  }

  m_pbRecvBin = GST_ELEMENT(m_pbTermBin->get_termRecvBin());
  m_pbFakeBin = GST_ELEMENT(m_hmpPlayFileBin->get_fakeBin());

  m_pbSendBin = GST_ELEMENT(m_pbTermBin->get_termSendBin());
  m_pbFileBin = GST_ELEMENT(m_hmpPlayFileBin->get_fileBin());

  m_pbSendPipeline = sClient->m_playChannel->m_pbSendPipeline;
  m_pbRecvPipeline = sClient->m_playChannel->m_pbRecvPipeline;

  gst_bin_add_many(GST_BIN(m_pbSendPipeline), m_pbRecvBin, m_pbFakeBin, NULL);
  gst_element_link_many(m_pbRecvBin, m_pbFakeBin, NULL);

  gst_bin_add_many(GST_BIN(m_pbRecvPipeline), m_pbFileBin, m_pbSendBin, NULL);
  gst_element_link_many(m_pbFileBin, m_pbSendBin, NULL);

  g_object_set(G_OBJECT(m_pbTermBin->m_udpSend),"clients", clintadd, NULL);
  
  return true;
}

void HmpGstPlayChannel::activate_fileplay_channel(Client *sClient, string playWavfile, gboolean autoRepeat)
{
  CLog::Detail(THISMODULE, "HmpGstPlayBackPipeline::activate_fileplay_channel file %s is ", playWavfile.c_str());

  FILE * rbtFile = fopen(playWavfile.c_str(), "rb");
  if(!rbtFile) {
    CLog::Detail(THISMODULE, "HmpGstPlayBackPipeline::activate_fileplay_channel file %s is OPEN FAILED", playWavfile.c_str());
    sClient->m_detail.m_pbState = EN_XGATE_PB_EOS;
    sClient->m_playChannel->post_playback_eos(sClient);
    return;
  }
  CLog::Detail(THISMODULE, "HmpGstPlayBackPipeline::activate_fileplay_channel file %s OPENED SUCCESSFULLY autoRepeat %d", playWavfile.c_str(), autoRepeat);
  m_hmpPlayFileBin = sClient->m_playChannel->m_hmpPlayFileBin;
 
  m_pbSendPipeline = sClient->m_playChannel->m_pbSendPipeline;
  m_pbRecvPipeline = sClient->m_playChannel->m_pbRecvPipeline;

  g_object_set(G_OBJECT(m_hmpPlayFileBin->m_fileSrc),"location", playWavfile.c_str(), NULL);
  
  sClient->m_playChannel->m_autoRepeat = autoRepeat;

  m_pbRecvBus = sClient->m_playChannel->m_pbRecvBus;

  m_pbRecvBusId = gst_bus_add_watch(m_pbRecvBus, &HmpGstPlayChannel::handle_event_dispatcher, sClient);

  gst_element_set_state(m_pbSendPipeline, GST_STATE_PLAYING);
  gst_element_set_state(m_pbRecvPipeline, GST_STATE_PLAYING);

  if(GST_DEBUG) {
    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(m_pbSendPipeline), GST_DEBUG_GRAPH_SHOW_ALL, m_pbSendPipelineName.c_str());
    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(m_pbRecvPipeline), GST_DEBUG_GRAPH_SHOW_ALL, m_pbRecvPipelineName.c_str());
  }
}

void HmpGstPlayChannel::deactivate_fileplay_channel(Client *sClient)
{
  CLog::Detail(THISMODULE, "xGateHmpGstPipeline::deactivate_fileplay_channel");

  m_pbRecvPipeline = sClient->m_playChannel->m_pbRecvPipeline;
  m_pbSendPipeline = sClient->m_playChannel->m_pbSendPipeline;

  gst_element_set_state(m_pbRecvPipeline, GST_STATE_NULL);
  gst_element_set_state(m_pbSendPipeline, GST_STATE_NULL);

}

gboolean  HmpGstPlayChannel::handle_event_dispatcher(GstBus* bus, GstMessage* msg, gpointer data)
{
  char dtmfDigit[10];
  gint number;
  const GstStructure* structure;

  Client *client = (Client *)data; 
  HmpGstPlayChannel *hmpPlayBackPipeline = client->m_playChannel;

  switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_ERROR:
      {
        CLog::Detail(THISMODULE, "HmpGstPlayBackPipeline::handle_event_dispatcher got GST_MESSAGE_ERROR message");
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
        break;
      }
    case GST_MESSAGE_EOS:
      {
        CLog::Detail(THISMODULE, "HmpGstPlayBackPipeline::handle_event_dispatcher got GST_MESSAGE_EOS message");
        if(hmpPlayBackPipeline) {
          GstElement *playBackPipeline = hmpPlayBackPipeline->m_pbRecvPipeline;
          gboolean autoRepeat = hmpPlayBackPipeline->m_autoRepeat;
          if (autoRepeat) {
            CLog::Detail(THISMODULE, "HmpGstPlayBackPipeline::handle_event_dispatcher EOS encountered. Playing again.");

            gst_element_set_state(playBackPipeline, GST_STATE_NULL);
            gst_element_set_state(playBackPipeline, GST_STATE_PLAYING);

          } else {
            CLog::Detail(THISMODULE, "HmpGstPlayBackPipeline::handle_event_dispatcher EOS encountered. Not playing again. \n");
            hmpPlayBackPipeline->deactivate_fileplay_channel(client);
            client->m_detail.m_pbState = EN_XGATE_PB_EOS;
            hmpPlayBackPipeline->post_playback_eos(client); //TODO Yoga, do we need to do this every time?
          }
        }
        break;
      }

    case GST_MESSAGE_UNKNOWN:
    default:
      {
        //CLog::Detail(THISMODULE, "HmpGstPlayBackPipeline::handle_event_dispatcher got GST_MESSAGE_UNKNOWN message");
        break;
      }
  }
  return TRUE; //it's important return TRUE from this function
}

void HmpGstPlayChannel::post_playback_eos(Client *client)
{
  CLog::Detail(THISMODULE,"HmpGstPlayBackPipeline::post_playback_eos Posting EOS ");
  xGateMgMsg *pMgMsg = new xGateMgMsg();
  MgMediaDetail mediaDetail;

  pMgMsg->setMsgType(EN_XGATE_MSG_MGC_MG_IN);

  mediaDetail.call_id = client->m_detail.m_callId;
  mediaDetail.leg_id = client->m_detail.m_legid;
//  mediaDetail.ip_addr = client->m_detail.m_ip;
//  mediaDetail.port = client->m_detail.m_port;
//  mediaDetail.codec = client->m_detail.m_codec;
//  mediaDetail.ptime = client->m_detail.m_pTime;
  mediaDetail.pb_state = client->m_detail.m_pbState;
  mediaDetail.dtmf_digits = client->m_detail.m_dtmfBuffer;
  mediaDetail.media_proto = client->m_detail.m_mediaMode;
  mediaDetail.out_proto = client->m_detail.m_outProto;
  mediaDetail.call_dir = client->m_detail.m_callDir;

  pMgMsg->set_media_detail(mediaDetail);

  ACE_Task<ACE_MT_SYNCH>* taskPtr = NULL;
  taskPtr = (xGateSipSignalProcessor*)GetSipPostHandler(XGATEPOSTTYPE::EN_PROCESS);
  if(!taskPtr) {
    CLog::Error(THISMODULE, "HmpGstPlayBackPipeline::post_playback_eos SipSignalProcessor task not found");
    return EN_XGATE_STATUS_ERROR;
  }

  ACE_Message_Block* pAmb = NULL;
  pAmb = static_cast<xGateMgMsg*> (pMgMsg);
  ACE_Time_Value tval ((time(NULL) ));
  tval += ACE_Time_Value(0,1);

  if (taskPtr->putq(pAmb, &tval)) {
    CLog::Detail(THISMODULE, "HmpGstPlayBackPipeline::post_playback_eos SipSignalProcessor enqueue message success");
  } else {
    CLog::Error(THISMODULE, "HmpGstPlayBackPipeline::post_playback_eos SipSignalProcessor enqueue message failed");
  }
}

/*
 *  HmpGstPlayBackPipeline class for playing file - End
 */

/* HmpGstBin base class implementation
*/
xGateHmpGstBin::xGateHmpGstBin(string binName, HmpGstBinType binType) 
{
  CLog::Detail(THISMODULE, "xGateHmpGstBin::xGateHmpGstBin %s",binName.c_str());
  m_sendBin = NULL;
  m_recvBin = NULL;
  m_sendUdp = NULL;
  m_recvUdp = NULL;
  m_recvUdp = NULL;
  m_rtpJitterBuffer = NULL;
  m_rtpDepay = NULL;
  m_payDec = NULL;
  m_audioConvert = NULL;
  m_payEnc  = NULL;
  m_rtpPay = NULL;
  m_sendUdp = NULL;
  m_encFilter = NULL;
  m_dtlssrtpEnc = NULL;
  m_dtlssrtpDec = NULL;
  m_decFilter = NULL;
  m_udpSocket = NULL;
  m_udpPort = 0;
  m_binType = binType;

  gchar sendBin[30];
  sprintf(sendBin,"%s_Send", binName.c_str());
  m_sendBinName = sendBin;

  gchar recvBin[30];
  sprintf(recvBin,"%s_Recv", binName.c_str());
  m_recvBinName = recvBin;

  gstSrtpCaps = (GstSrtpCaps *) malloc(sizeof(GstSrtpCaps));
  gstSrtpCaps->key_is_set =false;
 
  //params = (SecurityParams *) malloc(sizeof(SecurityParams));
  srtp_auth = 0;

  latency = 60;
  m_pTime = 0;
}

xGateHmpGstBin::~xGateHmpGstBin()
{
  CLog::Detail(THISMODULE, "xGateHmpGstBin::~xGateHmpGstBin ");
  cout << endl << "xGateHmpGstBin::~xGateHmpGstBin start " << endl;

  if(m_udpSocket) {
    g_socket_close(m_udpSocket, NULL);
    g_object_unref(G_OBJECT(m_udpSocket));
    m_udpSocket = NULL;
  }
  m_udpPort = 0;

  switch (m_mediaMode)
  {
    case EN_MEDIA_RTP:

         gst_element_unlink_many(m_audioConvert, m_payEnc, m_rtpPay, m_sendUdp, NULL);
         gst_bin_remove_many(m_sendBin, m_audioConvert, m_payEnc, m_rtpPay, m_sendUdp, NULL);
#if 0 //Yoga
         gst_element_unlink_many(m_recvUdp, m_rtpJitterBuffer, m_rtpDepay, m_payDec, NULL);
         gst_bin_remove_many(m_recvBin, m_recvUdp, m_rtpJitterBuffer, m_rtpDepay, m_payDec,  NULL);
#else
         gst_element_unlink_many(m_recvUdp, m_rtpBin, m_rtpDepay, m_payDec, NULL);
         gst_bin_remove_many(m_recvBin, m_recvUdp, m_rtpBin, m_rtpDepay, m_payDec,  NULL);
#endif
        
         break;
    case EN_MEDIA_SRTP:
         gst_element_unlink_many(m_audioConvert, m_payEnc, m_rtpPay, m_capsFilter, m_srtpEnc, m_sendUdp, NULL);
         gst_bin_remove_many(m_sendBin, m_audioConvert, m_payEnc, m_rtpPay, m_capsFilter, m_srtpEnc, m_sendUdp, NULL);

         gst_element_unlink_many(m_recvUdp, m_srtpDec, m_rtpJitterBuffer, m_rtpDepay, m_payDec, NULL);
         gst_bin_remove_many(m_recvBin, m_recvUdp, m_srtpDec, m_rtpJitterBuffer, m_rtpDepay, m_payDec, NULL);
         
         break;
    case EN_MEDIA_DTLS:
         gst_element_unlink_many(m_audioConvert, m_payEnc, m_rtpPay, m_encFilter, m_dtlssrtpEnc, m_sendUdp, NULL);
          gst_bin_remove_many(m_sendBin, m_audioConvert, m_payEnc, m_rtpPay, m_encFilter, m_dtlssrtpEnc, m_sendUdp, NULL);

         gst_element_unlink_many(m_recvUdp, m_dtlssrtpDec, /*m_decFilter,*/ m_rtpDepay, m_payDec, NULL);
         gst_bin_remove_many(m_recvBin, m_recvUdp, m_dtlssrtpDec,/* m_decFilter,*/ m_rtpDepay, m_payDec, NULL);
         break;
     default:
         break;
 }

 cout << endl << "xGateHmpGstBin::~xGateHmpGstBin end " << endl; 
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

GSocket* xGateHmpGstBin::create_udp_socket(unsigned int udpPort)
{
  CLog::Detail(THISMODULE, "xGateHmpGstBin::create_udp_socket");
  GSocket* udpSocket = NULL;
  gboolean retVal = FALSE;

  do {
    udpSocket = NULL;
    retVal = FALSE;
    udpSocket = g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_DATAGRAM, G_SOCKET_PROTOCOL_UDP, NULL);
    if(!udpSocket) {
      CLog::Error(THISMODULE, "xGateHmpGstBin::create_udp_socket g_socket_new failed!");
      return NULL;
    }
    GInetAddress* address = g_inet_address_new_any (G_SOCKET_FAMILY_IPV4);
    GSocketAddress* gsockaddress =g_inet_socket_address_new(address, udpPort);
    retVal = g_socket_bind(udpSocket, gsockaddress, TRUE, NULL);
    g_object_unref(gsockaddress);
    g_object_unref(address);
  } while(!retVal);

  if(!udpSocket) {
    CLog::Error(THISMODULE, "xGateHmpGstBin::create_udp_socket failed to create valide udp socket");
    return NULL;
  }

  CLog::Detail(THISMODULE, "xGateHmpGstBin::create_udp_socket success for udp port: %d", udpPort);
  return udpSocket;
}

void xGateHmpGstBin::set_client_details(ClientDetail &cliDetail)
{
  gchar iceUfrag[100];
  m_mediaMode  = cliDetail.m_mediaMode;
  m_pTime = cliDetail.m_pTime;
 
  if (m_mediaMode == EN_MEDIA_DTLS) {
    m_clientIp = cliDetail.m_relayIpaddr;
    m_clientPort = cliDetail.m_relayPort;
  } else {
    m_clientIp = cliDetail.m_ip;
    m_clientPort = cliDetail.m_port;
  }

  m_udpPort = cliDetail.m_serverPort;
  m_udpSocket = create_udp_socket(m_udpPort);

  //if (m_mediaMode = EN_MEDIA_DTLS) {
     sprintf(iceUfrag, "%s:%s", cliDetail.m_locIceUfrag.c_str(), cliDetail.m_remIceUfrag.c_str());
     strcpy(m_params.iceufrag,iceUfrag);

     strcpy(m_params.icepwd, cliDetail.m_icePwd.c_str());
     sprintf(m_params.cryptoLine, cliDetail.m_fingerPrint.c_str());

     strcpy(m_params.pemFile, cliDetail.m_pemFile.c_str());
     generateConnectId(m_params.connectId,15);
  //}

  CLog::Detail(THISMODULE," xGateHmpGstBin::set_client_details Media mode %d  ICE_UFRAG %s  ICE_PWD %s PTime %d Conn Id %s\n",m_mediaMode, m_params.iceufrag, m_params.icepwd, m_pTime, m_params.connectId);
  printf(" xGateHmpGstBin::set_client_details Media mode %d PTime %d \n",m_mediaMode,m_pTime);
}

GstBin* xGateHmpGstBin::sendBin_create() {
  CLog::Detail(THISMODULE, "xGateHmpGstBin::sendBin_create bass class implementation return NULL");
  gboolean retVal = FALSE;
  char clintadd[30] = {0};
  snprintf(clintadd, 30, "%s:%d", m_clientIp.c_str(), m_clientPort);

  m_sendBin = GST_BIN(gst_bin_new(m_sendBinName.c_str()));
  m_sendUdp = gst_element_factory_make("multiudpsink", NULL);

  if(!m_sendBin || !m_sendUdp) {
      CLog::Error(THISMODULE, "xGateHmpGstBin::sendBin_create  bass class gst_element_make failed");
      return FALSE;
  }

  g_object_set(G_OBJECT(m_sendUdp), "socket", m_udpSocket, NULL); 
  g_object_set(G_OBJECT(m_sendUdp), "clients", clintadd, NULL);
  g_object_set(G_OBJECT(m_sendUdp), "close-socket", FALSE, NULL);
  
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
   CLog::Detail(THISMODULE, "xGateHmpGstBin::sendBin_update Started");

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
  CLog::Detail(THISMODULE, "xGateHmpGstBin::recvBin_create bass class implementation return NULL");
  gboolean retVal = FALSE;
  GstPad *pad;

  m_recvBin = GST_BIN(gst_bin_new(m_recvBinName.c_str()));
  
  m_recvUdp = gst_element_factory_make("udpsrc", NULL);
  m_rtpJitterBuffer = gst_element_factory_make("rtpjitterbuffer", NULL);

  if(!m_recvBin || !m_recvUdp || !m_rtpJitterBuffer) {
      CLog::Error(THISMODULE, "xGateHmpGstBin::recvBin_create bass class gst_element_make failed");
      return FALSE;
  }

  g_object_set(G_OBJECT(m_recvUdp), "socket", m_udpSocket, NULL);
  g_object_set(G_OBJECT(m_recvUdp),"timeout", 0, NULL);
  g_object_set(G_OBJECT(m_recvUdp),"reuse", TRUE, NULL);
  g_object_set(G_OBJECT(m_recvUdp), "close-socket", FALSE, NULL);

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
   CLog::Detail(THISMODULE, "xGateHmpGstBin::recvBin_update Started");
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
  printf("on_pad_added triggered for pad: %s\n", tmpBuff);
  //recv_rtp_src_0_4239506697_8 or recv_rtp_sink_0 //sample rtp bin pad names
  sscanf(tmpBuff, "recv_rtp_src_%u_%u_%u", &sn, &ssrc, &pt);
  g_free(tmpBuff);

  padDir = gst_pad_get_direction(pad);
  //TODO: Yoga, we can optimize the switch case code blocks
  if(padDir == GST_PAD_SRC) {
    CLog::Detail(THISMODULE, "xGateHmpGstBin::on_pad_added handling GST_PAD_SRC event");
    switch(pt) {
      case EN_CNF_CODEC:
        {
          HmpG711Bin *hmpBin = (HmpG711Bin *)data;
          if(!gst_pad_is_linked(pad)) {
            sinkPad = gst_element_get_static_pad(hmpBin->m_cnfUdpSink, "sink");
            if(sinkPad) {
              g_object_set(G_OBJECT(hmpBin->m_cnfUdpSink), "close-socket", TRUE, NULL);
              gst_bin_add_many(hmpBin->m_recvBin, hmpBin->m_cnfUdpSink, NULL);
              isPadLinked =  gst_pad_link(pad, sinkPad);
              gst_element_sync_state_with_parent(hmpBin->m_cnfUdpSink);
              gst_object_unref(sinkPad);
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
      case EN_OPUS_CODEC:
        {
          CLog::Detail(THISMODULE, "xGateHmpGstBin::on_pad_added handle OPUS codec packets");
          HmpOpusBin *hmpBin = (HmpOpusBin *)data;
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
                  peerElem = GST_ELEMENT(gst_pad_get_parent_element(peerPad));
                  gst_object_unref(peerPad);
                }
              }
            }
          }
          if(GST_DEBUG) {
            GstObject * parentObj = gst_object_get_parent(GST_OBJECT_CAST(hmpBin->m_recvBin));
            tmpBuff = gst_object_get_name(parentObj);
            if(parentObj) {
              GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(parentObj), GST_DEBUG_GRAPH_SHOW_ALL, gst_object_get_name(parentObj));
              gst_object_unref(parentObj);
            }
            g_free(tmpBuff);
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
  if(sockfd) {
    g_source_remove(sockfd);
  }
}

GstBin * HmpG711Bin::sendBin_create() 
{
  CLog::Detail(THISMODULE, "HmpG711SendBin::sendBin_create (creating G711SendBin)");
  gint64 ptime;
  gboolean retVal = FALSE;
  char clintadd[30] = {0};
  snprintf(clintadd, 30, "%s:%d", m_clientIp.c_str(), m_clientPort);
 
  m_sendBin = GST_BIN(gst_bin_new(m_sendBinName.c_str()));

  m_audioConvert = gst_element_factory_make("audioconvert", NULL);
  m_payEnc = gst_element_factory_make("alawenc", NULL);
  m_rtpPay = gst_element_factory_make("rtppcmapay", NULL);
  m_sendUdp = gst_element_factory_make("multiudpsink", NULL);

  if(!m_audioConvert || !m_payEnc || !m_rtpPay || !m_sendUdp) {
    CLog::Error(THISMODULE, "HmpG711SendBin::sendBin_create gst_element_make failed !!");
    return NULL;  
  }

  if (m_pTime >= 10){
    ptime = m_pTime*1000000;
  } else {
    ptime = 10*1000000;
  }
  g_object_set(G_OBJECT(m_rtpPay),"min-ptime",ptime,"max-ptime",ptime,NULL);
  g_object_set(G_OBJECT(m_sendUdp), "socket", m_udpSocket, NULL);
  g_object_set(G_OBJECT(m_sendUdp), "clients", clintadd, NULL);
  g_object_set(G_OBJECT(m_sendUdp), "close-socket", FALSE, NULL);

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
 
  m_recvUdp = gst_element_factory_make("udpsrc", NULL); 
#if 0 //Yoga
  m_rtpJitterBuffer = gst_element_factory_make("rtpjitterbuffer", NULL);
#else
  create_rtpbin();
  m_cnfUdpSink = gst_element_factory_make("multiudpsink", NULL);
#endif
  m_rtpDepay = gst_element_factory_make("rtppcmadepay", NULL);
  m_payDec = gst_element_factory_make("alawdec", NULL);
  m_dtmfDetect = gst_element_factory_make ("dtmfdetect", NULL);

#if 0 //Yoga
  if(!m_recvUdp || !m_rtpJitterBuffer || !m_rtpDepay || !m_payDec || !m_dtmfDetect) {
    CLog::Error(THISMODULE, "HmpG711RecvBin::recvBin_create gst_element_make failed !!");
    return NULL;  
  }
#else
  if(!m_recvUdp || !m_rtpBin || !m_rtpDepay || !m_payDec || !m_dtmfDetect) {
    CLog::Error(THISMODULE, "HmpG711RecvBin::recvBin_create gst_element_make failed !!");
    return NULL;  
  }
#endif
  sprintf(targetIp,"%s",m_clientIp.c_str());
  targetPort = m_clientPort;
  sockfd = g_socket_get_fd(m_udpSocket);

  g_object_set(G_OBJECT(m_recvUdp), "socket", m_udpSocket, NULL);
  g_object_set(G_OBJECT(m_recvUdp),"timeout", 0, NULL);
  g_object_set(G_OBJECT(m_recvUdp),"reuse", TRUE, NULL);
  g_object_set(G_OBJECT(m_recvUdp), "close-socket", FALSE, NULL);

#if 0 //Yoga
  g_object_set(G_OBJECT(m_rtpJitterBuffer), "latency", latency, NULL);
  g_object_set(G_OBJECT(m_rtpJitterBuffer), "do-lost", true, NULL);
#endif  

  switch (m_mediaMode)
  {
    case EN_MEDIA_RTP:

         caps = gst_caps_from_string(UDP_CAPS_G711A);
         g_object_set(G_OBJECT(m_recvUdp), "caps", caps, NULL);
         gst_caps_unref(caps);
         
#if 0 //Yoga
         gst_bin_add_many(m_recvBin, m_recvUdp, m_rtpJitterBuffer, m_rtpDepay, m_payDec,  m_dtmfDetect, NULL);
         retVal = gst_element_link_many(m_recvUdp, m_rtpJitterBuffer, m_rtpDepay, m_payDec, m_dtmfDetect, NULL);
#else
         gst_bin_add_many(m_recvBin, m_recvUdp, m_rtpBin, m_rtpDepay, m_payDec,  m_dtmfDetect, NULL);
         retVal = gst_element_link_many(m_recvUdp, m_rtpBin, NULL);
         retVal = gst_element_link_many(m_rtpDepay, m_payDec, m_dtmfDetect, NULL);
#endif
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
         g_object_set (G_OBJECT (m_dtlssrtpDec), "sock-des", sockfd, NULL);
         g_object_set (G_OBJECT (m_dtlssrtpDec), "stun-host", targetIp, NULL);
         g_object_set (G_OBJECT (m_dtlssrtpDec), "stun-port", targetPort, NULL);
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

//  GstPad *pad = gst_element_get_static_pad(m_payDec, "src");
  GstPad *pad = gst_element_get_static_pad( m_dtmfDetect, "src");
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

gboolean HmpG711Bin::create_rtpbin()
{
  m_rtpBin = gst_element_factory_make("rtpbin", NULL);
  if(!m_rtpBin) {
    CLog::Error(THISMODULE, "HmpG711RecvBin::create_rtpbin gst_element_make rtpbin failed !!");
    return FALSE;
  }

  g_object_set(G_OBJECT(m_rtpBin), "autoremove", true, NULL);
  g_object_set(G_OBJECT(m_rtpBin), "latency", latency, NULL);
  g_object_set(G_OBJECT(m_rtpBin), "do-lost", true, NULL);

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
  if(sockfd) {
    g_source_remove(sockfd);
  }

}

GstBin * HmpG729Bin::sendBin_create() {
  CLog::Detail(THISMODULE, "HmpG729SendBin::sendBin_create (creating G729SendBin)");
  gint64 ptime;
  gboolean retVal = FALSE;
  char clintadd[30] = {0};
  snprintf(clintadd, 30, "%s:%d", m_clientIp.c_str(), m_clientPort);

  m_sendBin = GST_BIN(gst_bin_new(m_recvBinName.c_str()));

  m_audioConvert = gst_element_factory_make("audioconvert", NULL);
  m_payEnc = gst_element_factory_make("g729enc", NULL);
  m_rtpPay = gst_element_factory_make("rtpg729pay", NULL);
  m_sendUdp = gst_element_factory_make("multiudpsink", NULL);

  if(!m_audioConvert || !m_payEnc || !m_rtpPay || !m_sendUdp) {
    CLog::Error(THISMODULE, "HmpG729SendBin::sendBin_create gst_element_make failed !!");
    return NULL;  
  }

  if (m_pTime >= 10){
    ptime = m_pTime*1000000;
  } else {
    ptime = 10*1000000;
  }

  g_object_set(G_OBJECT(m_rtpPay),"min-ptime",ptime,"max-ptime",ptime,NULL);
  g_object_set(G_OBJECT(m_sendUdp), "socket", m_udpSocket, NULL);
  g_object_set(G_OBJECT(m_sendUdp), "clients", clintadd, NULL);
  g_object_set(G_OBJECT(m_sendUdp), "close-socket", FALSE, NULL);
  
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
  CLog::Detail(THISMODULE, "HmpG729RecvBin::recvBin_create (creating G711RecvBin)");
  gboolean retVal = FALSE;

  m_recvBin = GST_BIN(gst_bin_new(m_sendBinName.c_str()));

  m_recvUdp = gst_element_factory_make("udpsrc", NULL);
  m_rtpJitterBuffer = gst_element_factory_make("rtpjitterbuffer", NULL);
  m_rtpDepay = gst_element_factory_make("rtpg729depay", NULL);
  //m_g729Dec = gst_element_factory_make("g729dec", NULL);
  m_payDec = gst_element_factory_make("avdec_g729", NULL);

  if(!m_recvUdp || !m_rtpJitterBuffer || !m_rtpDepay || !m_payDec) {
    CLog::Error(THISMODULE, "HmpG729RecvBin::recvBin_create gst_element_make failed !!");
    return NULL;  
  }

  sprintf(targetIp,"%s",m_clientIp.c_str());
  targetPort = m_clientPort;
  sockfd = g_socket_get_fd(m_udpSocket);

  g_object_set(G_OBJECT(m_recvUdp), "socket", m_udpSocket, NULL);
  g_object_set(G_OBJECT(m_recvUdp),"timeout", 0, NULL);
  g_object_set(G_OBJECT(m_recvUdp),"reuse", TRUE, NULL);
  g_object_set(G_OBJECT(m_recvUdp), "close-socket", FALSE, NULL);

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
         g_object_set (G_OBJECT (m_dtlssrtpDec), "sock-des", sockfd, NULL);
         g_object_set (G_OBJECT (m_dtlssrtpDec), "stun-host",targetIp, NULL);
         g_object_set (G_OBJECT (m_dtlssrtpDec), "stun-port",targetPort, NULL);
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
  gchar sendBin[30];
  sprintf(sendBin,"%s_Send", binName.c_str());
  m_sendBinName = sendBin;

  gchar recvBin[30];
  sprintf(recvBin,"%s_Recv", binName.c_str());
  m_recvBinName = recvBin;
  targetPort = 0; 
  sockfd = 0;
}

HmpOpusBin::~HmpOpusBin()
{
  CLog::Detail(THISMODULE, "HmpOpusBin::~HmpOpusBin");
  if(sockfd) {
    g_source_remove(sockfd);
  }
}

GstBin * HmpOpusBin::sendBin_create() {
  CLog::Detail(THISMODULE, "HmpOpusSendBin::bin_create (creating OpusSendBin)");
  gint64 mnptime;
  gint64 mxptime;
  gint64 bitrate;
  int codec;
  gboolean retVal = FALSE;
  char clintadd[30] = {0};
  snprintf(clintadd, 30, "%s:%d", m_clientIp.c_str(), m_clientPort);

  m_sendBin = GST_BIN(gst_bin_new(m_sendBinName.c_str()));

  m_audioConvert = gst_element_factory_make("audioconvert", NULL);
  m_audioResample = gst_element_factory_make("audioresample", NULL);
  m_payEnc = gst_element_factory_make("opusenc", NULL);
  m_rtpPay = gst_element_factory_make("rtpopuspay", NULL);
  m_sendUdp = gst_element_factory_make("multiudpsink", NULL);
  
  if(!m_sendBin || !m_audioConvert || !m_payEnc || !m_rtpPay || !m_sendUdp) {
    CLog::Error(THISMODULE, "HmpOpusSendBin::sendBin_create gst_element_make failed !!");
    return NULL;
  }

  mnptime = 10*1000000;
  mxptime = 40*1000000;

  g_object_set(G_OBJECT(m_rtpPay),"min-ptime",mnptime,"max-ptime",mxptime,NULL);
  g_object_set(G_OBJECT(m_sendUdp), "socket", m_udpSocket, NULL);
  g_object_set(G_OBJECT(m_sendUdp), "clients", clintadd, NULL);
  g_object_set(G_OBJECT(m_sendUdp), "close-socket", FALSE, NULL);

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
        bitrate = 48000;

        g_object_set(G_OBJECT(m_rtpPay),"pt", codec, NULL);

        m_encFilter =gst_element_factory_make("capsfilter", NULL);
        m_dtlssrtpEnc = gst_element_factory_make ("stundtlssrtpenc", NULL);
        if( m_dtlssrtpEnc == NULL )
          CLog::Info(THISMODULE, "\n DTLS_SRTP_PROTO dtlssrtpEnc Factory not created\n");

        g_object_set (G_OBJECT (m_dtlssrtpEnc),"connection-id", m_params.connectId, NULL);
        g_object_set (G_OBJECT (m_dtlssrtpEnc), "is-client", FALSE, NULL);
        g_object_set(G_OBJECT(m_payEnc), "inband-fec", true, NULL); 
        g_object_set(G_OBJECT(m_payEnc), "bitrate-type", 1, NULL);
        //g_object_set(G_OBJECT(m_payEnc), "hard-resync", true, NULL);
        //g_object_set(G_OBJECT(m_payEnc), "audio-type", 2048, NULL);
        g_object_set(G_OBJECT(m_payEnc), "bitrate", bitrate, NULL);
 
        caps = gst_caps_from_string(CAPS_ENC_OPUS);
        g_object_set(G_OBJECT(m_encFilter),"caps",caps,NULL);
        gst_caps_unref(caps);

        gst_bin_add_many(m_sendBin, m_audioConvert, m_payEnc, m_rtpPay, m_encFilter, m_dtlssrtpEnc, m_sendUdp, NULL);
        retVal = gst_element_link_many(m_audioConvert, m_payEnc, m_rtpPay, m_encFilter, m_dtlssrtpEnc, m_sendUdp, NULL);
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
  CLog::Detail(THISMODULE, "HmpOpusRecvBin::recvBin_create (creating OpusRecvBin)");
  gboolean retVal = FALSE;

  m_recvBin = GST_BIN(gst_bin_new(m_recvBinName.c_str()));

  m_recvUdp = gst_element_factory_make("udpsrc", NULL);
  m_rtpJitterBuffer = gst_element_factory_make("rtpjitterbuffer", NULL);
  m_rtpDepay = gst_element_factory_make("rtpopusdepay", NULL);
  m_payDec = gst_element_factory_make("opusdec", NULL);

  if(!m_recvBin || !m_recvUdp || !m_rtpJitterBuffer || !m_rtpDepay || !m_payDec) {
    CLog::Error(THISMODULE, "HmpOpusRecvBin::bin_create gst_element_make failed !!");
    return NULL;
  }

  sprintf(targetIp,"%s",m_clientIp.c_str());
  targetPort = m_clientPort;
  sockfd = g_socket_get_fd(m_udpSocket);

  g_object_set(G_OBJECT(m_recvUdp), "socket", m_udpSocket, NULL);
  g_object_set(G_OBJECT(m_recvUdp),"timeout", 0, NULL);
  g_object_set(G_OBJECT(m_recvUdp),"reuse", TRUE, NULL);
  g_object_set(G_OBJECT(m_recvUdp), "close-socket", FALSE, NULL);

  g_object_set(G_OBJECT(m_rtpJitterBuffer), "latency", latency, NULL);
  g_object_set(G_OBJECT(m_rtpJitterBuffer), "do-lost", true, NULL);

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

         create_rtpbin();

         m_dtlssrtpDec = gst_element_factory_make("stundtlssrtpdec", NULL);
#if 0 
         m_decFilter=gst_element_factory_make("capsfilter", NULL);

         caps = gst_caps_from_string(CAPS_DEC_OPUS);
         g_object_set(G_OBJECT(m_decFilter),"caps",caps,NULL);
         gst_caps_unref(caps);
#endif

         if(!m_dtlssrtpDec || !m_rtpJitterBuffer)
            CLog::Info(THISMODULE, "\n DTLS_SRTP_PROTO dtlssrtpDec Factory not created\n");

         g_object_set (G_OBJECT (m_dtlssrtpDec), "connection-id", m_params.connectId, NULL);
         g_object_set (G_OBJECT (m_dtlssrtpDec), "sock-des", sockfd, NULL);
         g_object_set (G_OBJECT (m_dtlssrtpDec), "stun-host", targetIp, NULL);
         g_object_set (G_OBJECT (m_dtlssrtpDec), "stun-port", targetPort, NULL);
         g_object_set (G_OBJECT (m_dtlssrtpDec), "stun-uname", m_params.iceufrag, NULL);
         g_object_set (G_OBJECT (m_dtlssrtpDec), "stun-upass", m_params.icepwd, NULL);
         g_object_set (G_OBJECT (m_dtlssrtpDec), "pem",m_params.pemFile, NULL);

//         gst_bin_add_many(m_recvBin, m_recvUdp, m_dtlssrtpDec, m_decFilter, m_rtpDepay, m_payDec, NULL);
	 gst_bin_add_many(m_recvBin, m_recvUdp, m_dtlssrtpDec, m_rtpBin, m_rtpDepay, m_payDec, NULL);
//         retVal = gst_element_link_many(m_recvUdp, m_dtlssrtpDec, m_decFilter, m_rtpDepay, m_payDec, NULL);
         retVal = gst_element_link_many(m_recvUdp,  m_dtlssrtpDec, m_rtpBin,/* m_rtpDepay, m_payDec,*/ NULL);
         gst_element_link_many(m_rtpDepay, m_payDec, NULL);
         gst_element_link_pads(m_rtpBin,"recv_rtp_src_0", m_rtpDepay,"sink");
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
  return  m_recvBin;
}

gboolean HmpOpusBin::create_rtpbin()
{
  m_rtpBin = gst_element_factory_make("rtpbin", NULL);
  if(!m_rtpBin) {
    CLog::Error(THISMODULE, "HmpOpusRecvBin::create_rtpbin gst_element_make rtpbin failed !!");
    return FALSE;
  }

  g_object_set(G_OBJECT(m_rtpBin), "autoremove", true, NULL);
  g_object_set(G_OBJECT(m_rtpBin), "latency", latency, NULL);
  g_object_set(G_OBJECT(m_rtpBin), "do-lost", true, NULL);

  g_signal_connect(G_OBJECT(m_rtpBin), "pad-added", G_CALLBACK(&on_pad_added), this);
  g_signal_connect(G_OBJECT(m_rtpBin), "request-pt-map", G_CALLBACK(&on_request_pt_map), this);
  g_signal_connect(G_OBJECT(m_rtpBin), "pad-removed", G_CALLBACK(&on_pad_removed), this);

  return TRUE;
}
 
/*
 *  HmpUdpTermBin Class for Managing Legs independantly - Start  
*/

HmpUdpTermBin::HmpUdpTermBin(string binName)
:m_termSendBin(NULL), m_termRecvBin(NULL), m_termSocket(NULL), m_termPort(0)
{
   CLog::Detail(THISMODULE, "HmpUdpTermBin::HmpUdpTermBin() Called");
   gchar stermBin[30];
   sprintf(stermBin, "Send_Term_%s", binName.c_str());
   m_termSendBinName = stermBin;

   gchar rtermBin[30];
   sprintf(rtermBin, "Recv_Term_%s", binName.c_str());
   m_termRecvBinName = rtermBin;

   m_rtpTout = 20000000;
   
   m_udpSend = NULL;
   m_udpRecv = NULL;
   m_rtpL16Pay = NULL; 
   m_rtpL16Depay = NULL;
   m_rAudioConvert = NULL;
   m_sAudioConvert = NULL;
   m_sCapsFilter = NULL;
}

HmpUdpTermBin::~HmpUdpTermBin()
{
  CLog::Detail(THISMODULE, "HmpUdpTermBin::~HmpUdpTermBin() Called");
  if(m_termSocket) {
    g_socket_close(m_termSocket, NULL);
    g_object_unref(G_OBJECT(m_termSocket));
    m_termSocket = NULL;
  }
  m_termPort = 0;

  gst_element_unlink_many(m_sAudioConvert, m_sCapsFilter, m_rtpL16Pay, m_udpSend, NULL);
  gst_bin_remove_many(m_termSendBin, m_sAudioConvert, m_sCapsFilter,  m_rtpL16Pay, m_udpSend, NULL);

  gst_element_unlink_many(m_udpRecv, m_rtpL16Depay, m_rAudioConvert, NULL);
  gst_bin_remove_many(GST_BIN(m_termRecvBin), m_udpRecv, m_rtpL16Depay, m_rAudioConvert, NULL);
}

GSocket* HmpUdpTermBin::create_udp_socket(unsigned int termPort)
{
  CLog::Detail(THISMODULE, "HmpUdpTermBin::create_udp_socket");
  GSocket* udpSocket = NULL;
  gboolean retVal = FALSE;

  do {
    udpSocket = NULL;
    retVal = FALSE;
    udpSocket = g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_DATAGRAM, G_SOCKET_PROTOCOL_UDP, NULL);
    if(!udpSocket) {
      CLog::Error(THISMODULE,  "HmpUdpTermBin::create_udp_socket::create_udp_socket g_socket_new failed!");
      return NULL;
    }
    GInetAddress* address = g_inet_address_new_any (G_SOCKET_FAMILY_IPV4);
    GSocketAddress* gsockaddress =g_inet_socket_address_new(address, termPort);
    retVal = g_socket_bind(udpSocket, gsockaddress, TRUE, NULL);
    g_object_unref(gsockaddress);
    g_object_unref(address);
  } while(!retVal);

  if(!udpSocket) {
    CLog::Error(THISMODULE, "HmpUdpTermBin::create_udp_socket failed to create valid udp socket");
    return NULL;
  }

  CLog::Detail(THISMODULE, "HmpUdpTermBin::create_udp_socket success for udp port: %d", termPort);
  return udpSocket;
}

gboolean HmpUdpTermBin::create_termBin()
{
  CLog::Debug(THISMODULE,"HmpUdpTermBin::create_termBin for %s\n", m_termSendBinName.c_str());
   
  gboolean retVal = FALSE;
  m_termPort = HOSTGST_TRM_GET_PORT(); 
  m_termSocket = create_udp_socket(m_termPort);
  retVal = create_sendBin();
  retVal = create_recvBin();

  if (retVal)  
  {
     CLog::Error(THISMODULE,"HmpUdpTermBin::create_termBin failed for %s\n", m_termSendBinName.c_str());
     return false;
  }
  return true;
}

gboolean HmpUdpTermBin::create_sendBin()
{
   CLog::Debug(THISMODULE,"HmpUdpTermBin::create_sendBin for %s\n", m_termSendBinName.c_str());

   m_termSendBin = GST_BIN(gst_bin_new(m_termSendBinName.c_str())); 
   m_sAudioConvert = gst_element_factory_make("audioconvert", NULL);
   m_sCapsFilter = gst_element_factory_make("capsfilter", NULL);
   m_rtpL16Pay = gst_element_factory_make("rtpL16pay", NULL);
   m_udpSend = gst_element_factory_make("multiudpsink", NULL);
   
   if(!m_termSendBin || !m_sAudioConvert || !m_rtpL16Pay || !m_udpSend) {
     CLog::Error(THISMODULE, "HmpOpusSendBin::sendBin_create gst_element_make failed !!");
     return false;
   }

   GstCaps *tcaps = gst_caps_from_string(UDP_CAPS_L16);
   g_object_set(G_OBJECT(m_sCapsFilter),"caps",tcaps,NULL);
   gst_caps_unref(tcaps);
  
   g_object_set(G_OBJECT(m_udpSend),"async", FALSE, "sync", TRUE, NULL);
   g_object_set(G_OBJECT(m_udpSend),"socket", m_termSocket, "close-socket", FALSE, NULL);

   gst_bin_add_many(m_termSendBin, m_sAudioConvert, m_sCapsFilter,  m_rtpL16Pay, m_udpSend, NULL);
   gst_element_link_many(m_sAudioConvert, m_sCapsFilter, m_rtpL16Pay, m_udpSend, NULL);

   GstPad *pad = gst_element_get_static_pad (m_sAudioConvert, "sink");
   gst_element_add_pad (GST_ELEMENT(m_termSendBin), gst_ghost_pad_new ("sink", pad));
   gst_object_unref (GST_OBJECT (pad));

   if(GST_DEBUG) {
    GST_DEBUG_BIN_TO_DOT_FILE(m_termSendBin, GST_DEBUG_GRAPH_SHOW_ALL, m_termSendBinName.c_str());
   }

   CLog::Detail(THISMODULE, "HmpUdpTermBin::create_sendBin created successfully");
   return true;
}

gboolean HmpUdpTermBin::create_recvBin()
{
   CLog::Debug(THISMODULE,"HmpUdpTermBin::create_recvBin for %s\n", m_termRecvBinName.c_str());
   char termHost[100];
   char termUri[50];
   guint latency = 60;
   m_termRecvBin = GST_BIN(gst_bin_new(m_termRecvBinName.c_str()));
   m_udpRecv = gst_element_factory_make("udpsrc", NULL);
   m_rtpJitterBuffer = gst_element_factory_make("rtpjitterbuffer", NULL);
   m_rtpL16Depay = gst_element_factory_make("rtpL16depay", NULL);
   m_rAudioConvert = gst_element_factory_make("audioconvert", NULL);

   if(!m_termRecvBin || !m_udpRecv || !m_rtpL16Depay || !m_rAudioConvert) {
     CLog::Error(THISMODULE, "HmpUdpTermBin::create_recvBin gst_element_make failed !!");
     return false;
   }

   snprintf(termHost, sizeof(termHost), "%s:%u", "127.0.0.1", m_termPort);
   sprintf(termUri, "udp://%s",termHost);

   g_object_set(G_OBJECT(m_udpRecv),"socket", m_termSocket, "close-socket", FALSE, NULL);
   g_object_set(G_OBJECT(m_udpRecv),"uri", termUri, NULL);
   //g_object_set(G_OBJECT(m_udpRecv),"timeout", m_rtpTout, NULL); 

   GstCaps *tcaps= gst_caps_from_string(UDP_CAPS_RTPL16);
   g_object_set(G_OBJECT(m_udpRecv),"caps", tcaps, NULL);
   gst_caps_unref(tcaps);

   g_object_set(G_OBJECT(m_rtpJitterBuffer), "latency", latency, NULL);
   g_object_set(G_OBJECT(m_rtpJitterBuffer), "do-lost", true, NULL);

   gst_bin_add_many(GST_BIN(m_termRecvBin), m_udpRecv, m_rtpJitterBuffer, m_rtpL16Depay, m_rAudioConvert, NULL);
   gst_element_link_many(m_udpRecv, m_rtpJitterBuffer, m_rtpL16Depay, m_rAudioConvert, NULL);


   GstPad *pad = gst_element_get_static_pad (m_rAudioConvert, "src");
   gst_element_add_pad (GST_ELEMENT(m_termRecvBin), gst_ghost_pad_new ("src", pad));
   gst_object_unref (GST_OBJECT (pad));

   if(GST_DEBUG) {
    GST_DEBUG_BIN_TO_DOT_FILE(m_termRecvBin, GST_DEBUG_GRAPH_SHOW_ALL, m_termRecvBinName.c_str());
   }
   CLog::Detail(THISMODULE, "HmpUdpTermBin::create_recvBin created successfully");
   return true;
}

/*
 *  HmpPlayFileBin class for playing file implemntation - Start
 */

HmpPlayFileBin::HmpPlayFileBin(string binName, HmpGstBinType binType) :
  m_fileBin(NULL), m_fakeBin(NULL), m_binType(binType), 
  m_fileSrc(NULL), m_wavParse(NULL),m_ghostSrcPad(NULL), m_ghostSinkPad(NULL) 
{
  gchar fileBin[30];
  sprintf(fileBin,"File_%s", binName.c_str());
  m_fileBinName= fileBin;

  gchar fakeBin[30];
  sprintf(fakeBin,"Fake_%s", binName.c_str());
  m_fakeBinName= fakeBin;
}

HmpPlayFileBin::~HmpPlayFileBin()
{
  if(m_ghostSrcPad) {
    gst_object_unref (GST_OBJECT (m_ghostSrcPad));
  }
  if(m_ghostSinkPad) { 
    gst_object_unref (GST_OBJECT (m_ghostSinkPad));
  }

  gst_element_unlink_many(m_fileSrc, m_wavParse, NULL);
  gst_bin_remove_many(m_fileBin, m_fileSrc, m_wavParse, NULL);

  gst_element_unlink_many(GST_ELEMENT(m_fakeBin), m_fakeSink, NULL);
  gst_bin_remove_many(m_fakeBin, m_fakeSink, NULL);
}

gboolean HmpPlayFileBin::create_bin()
{
  CLog::Debug(THISMODULE,"HmpPlayFileBin::create_bin for %s\n", m_fileBinName.c_str());

  m_fileBin = GST_BIN(gst_bin_new(m_fileBinName.c_str()));
  m_fileSrc = gst_element_factory_make ("filesrc", NULL);
  m_wavParse = gst_element_factory_make ("wavparse", NULL);

  //if(!m_bin || !m_fileSrc || !m_decodeBin) {
  if(!m_fileBin || !m_fileSrc || !m_wavParse) {
    CLog::Error(THISMODULE, "HmpPlayFileBin::create_bin gst_element_factory_make m_fileBin failed");
    return FALSE;
  }

  gst_bin_add_many(m_fileBin, m_fileSrc, m_wavParse, NULL);
  gst_element_link_many(m_fileSrc, m_wavParse, NULL);

  GstPad *pad = gst_element_get_static_pad (m_wavParse, "src");
  if(pad) {
    m_ghostSrcPad = gst_ghost_pad_new("src", pad);
    gst_element_add_pad (GST_ELEMENT(m_fileBin), m_ghostSrcPad);
    gst_object_unref (GST_OBJECT (pad));
  }
  CLog::Debug(THISMODULE,"HmpPlayFileBin::create_bin for %s\n", m_fakeBinName.c_str());
  m_fakeBin = GST_BIN(gst_bin_new(m_fakeBinName.c_str()));
  m_fakeSink = gst_element_factory_make ("fakesink", NULL);

  if(!m_fakeBin || !m_fakeSink) {
    CLog::Error(THISMODULE, "HmpPlayFileBin::create_bin gst_element_factory_make m_fakeBin failed");
  }
  
  gst_bin_add_many(m_fakeBin, m_fakeSink, NULL);
  gst_element_link_many(GST_ELEMENT(m_fakeBin), m_fakeSink, NULL);

  pad = gst_element_get_static_pad (m_fakeSink, "sink");
  if(pad) {
      m_ghostSinkPad = gst_ghost_pad_new("sink", pad);
      gst_element_add_pad (GST_ELEMENT(m_fakeBin), m_ghostSinkPad);
      gst_object_unref (GST_OBJECT (pad));
  }

  if(GST_DEBUG) {
    GST_DEBUG_BIN_TO_DOT_FILE(m_fileBin, GST_DEBUG_GRAPH_SHOW_ALL, m_fileBinName.c_str());
    GST_DEBUG_BIN_TO_DOT_FILE(m_fakeBin, GST_DEBUG_GRAPH_SHOW_ALL, m_fakeBinName.c_str());
  }

  return TRUE;
}

/*
 *  HmpPlayFileBin class for playing file implemntation - End
 */

HmpPlayFileBin * HmpGstBinFactory::bin_playfile_create(string& binName, HmpGstBinType binType)
{
  CLog::Detail(THISMODULE, "Enter HmpGstFactory::bin_playfile_create");
  HmpPlayFileBin *pHmpPlayFileBin = NULL;
  switch(binType) {
    case EN_BIN_PLAY_FILE:
      {
        CLog::Detail(THISMODULE, "HmpGstFactory::bin_playfile_create creating PLAY_FILE bin");
        pHmpPlayFileBin = new HmpPlayFileBin(binName, binType);
        if(!pHmpPlayFileBin->create_bin()) {
          CLog::Error(THISMODULE, "HmpGstFactory::bin_playfile_create unable to create play file bin");
          delete pHmpPlayFileBin;
        }
        break;
      }
    case EN_BIN_UNKNOWN:
    default:
      {
        CLog::Error(THISMODULE, "HmpGstFactory::bin_playfile_create unable to create UNKNOWN_BIN type");
        break;
      }
  }

  //increase the ref count of this gstbin for reuse, so after called
  //gst_bin_remove_many(), this gstbin object will be in scope.
  if(pHmpPlayFileBin) {
    gst_object_ref(pHmpPlayFileBin->get_fileBin());
  } else {
    CLog::Error(THISMODULE, "HmpGstFactory::bin_playfile_create failed!!");
  }
  return pHmpPlayFileBin;  
}

xGateHmpGstBin * HmpGstBinFactory::bin_create(string& binName, HmpGstBinType binType, ClientDetail &cliDetail)
{

  CLog::Detail(THISMODULE, "Enter HmpGstFactory::bin_create");
  xGateHmpGstBin *pHmpGstBin = NULL;
  switch(binType) {

    case EN_BIN_RTP:
        CLog::Detail(THISMODULE, "HmpGstFactory::bin_create creating RTP_BIN");
        pHmpGstBin = new xGateHmpGstBin(binName, binType);
        pHmpGstBin->set_client_details(cliDetail);
        pHmpGstBin->sendBin_create();
        pHmpGstBin->recvBin_create();
        break;

    case EN_BIN_G711:
        CLog::Detail(THISMODULE, "HmpGstFactory::bin_create creating G711_BIN");
        pHmpGstBin = new HmpG711Bin(binName);
        pHmpGstBin->set_client_details(cliDetail);
        pHmpGstBin->sendBin_create();
        pHmpGstBin->recvBin_create();
        break;

    case EN_BIN_G729:
        CLog::Detail(THISMODULE, "HmpGstFactory::bin_create creating G729_BIN");
        pHmpGstBin = new HmpG729Bin(binName);
        pHmpGstBin->set_client_details(cliDetail);
        pHmpGstBin->sendBin_create();
        pHmpGstBin->recvBin_create();
        break;
    case EN_BIN_OPUS:
        CLog::Detail(THISMODULE, "HmpGstFactory::bin_create creating OPUS_BIN");
        pHmpGstBin = new HmpOpusBin(binName);
        pHmpGstBin->set_client_details(cliDetail);
        pHmpGstBin->recvBin_create();
        pHmpGstBin->sendBin_create();
        break;
    case EN_BIN_UNKNOWN:
    default:
        CLog::Error(THISMODULE, "HmpGstFactory::bin_create unable to create UNKNOWN_BIN type");
        break;
  }

  //increase the ref count of this gstbin for reuse, so after called
  //gst_bin_remove_many(), this gstbin object will be in scope.
  if(pHmpGstBin) {
    //gst_object_ref(pHmpGstBin->get_bin());
  } else {
    CLog::Error(THISMODULE, "HmpGstFactory::bin_create failed!!");
  }
  return pHmpGstBin;  
}
