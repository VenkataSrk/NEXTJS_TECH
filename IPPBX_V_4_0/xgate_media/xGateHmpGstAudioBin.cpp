
//local include
#include "xglog.h"

//self include
#include "xGateHmpGstBin.h"
#include "xGateHmpGstAudioBin.h"
#include <sys/stat.h>
#include <string>
#include <bits/stdc++.h>

#define THISMODULE "GstAudio"

/* HmpBin Base class - Start
*/
//constructor and destuctor
HmpBin::HmpBin(string binName, HmpBinType binType) : 
  m_binName(binName), m_binType(binType), m_bin(NULL),
  m_recvPort(0), m_sendPort(0), m_recvSocket(NULL), 
  m_sendSocket(NULL), m_udpRecv(NULL), m_udpSend(NULL) 
{
  m_bin = GST_BIN(gst_bin_new(binName.c_str()));
}

HmpBin::~HmpBin()
{
 XGLOG_INFO("HmpBin::~HmpBin start for bin: %s", (char*)ur_log_string(m_binName));
  if(m_recvSocket) {
   XGLOG_INFO("HmpBin destructing recv port: %d socket", m_recvPort);
    g_socket_close(m_recvSocket, NULL);
    g_object_unref(G_OBJECT(m_recvSocket));
    m_recvSocket = NULL;
    m_recvPort = 0;
  }

  if(m_sendSocket) {
   XGLOG_INFO("HmpBin destructing send port: %d socket", m_sendPort);
    g_socket_close(m_sendSocket, NULL);
    g_object_unref(G_OBJECT(m_sendSocket));
    m_sendSocket = NULL;
    m_sendPort = 0;
  }
}

gboolean HmpBin::create_udpsrc()
{
 XGLOG_INFO("create_udpsrc start");
  if(!m_udpRecv) {
    m_udpRecv = gst_element_factory_make("udpsrc", NULL);
    if(m_udpRecv) {
      if(m_recvSocket) {
        g_object_set(G_OBJECT(m_udpRecv), "socket", m_recvSocket, NULL);
      } else {
       XGLOG_ERROR("create_udpsrc failed due to invalid udp socket !!"); 
        return FALSE;
      }

      GstCaps* caps = get_caps();
      if(caps) {
        g_object_set(G_OBJECT(m_udpRecv), "caps", caps, NULL);
        gst_caps_unref(caps);
      } else {
       XGLOG_ERROR("create_udpsrc setting caps for udpsrc failed !!"); 
        return FALSE;
      }
      g_object_set(G_OBJECT(m_udpRecv), "timeout", 0, "reuse", TRUE, "close-socket", FALSE, NULL);
    } else {
     XGLOG_ERROR("create_udpsrc gst_element_make udpsrc failed !!");
      return FALSE;
    }
  }
  return TRUE;
}

gboolean HmpBin::create_udpsink()
{
 XGLOG_INFO("HmpBin::create_udpsink start");
  gint64 maxLateness = 40000000;
  if(!m_udpSend) {
    m_udpSend = gst_element_factory_make("multiudpsink", NULL);
    if(m_udpSend) {
      if(m_sendSocket) {
        g_object_set(G_OBJECT(m_udpSend), "socket", m_sendSocket, NULL);
      } else {
       XGLOG_ERROR("create_udpsink, failed due to invalid udp socket !!"); 
        return FALSE;
      }
      g_object_set(G_OBJECT(m_udpSend),"async", FALSE, \
          "sync", TRUE, "close-socket", FALSE, "send-duplicates", FALSE, NULL);
      g_object_set(G_OBJECT(m_udpSend),"max-lateness", maxLateness, NULL);
    } else {
     XGLOG_ERROR("create_udpsink, gst_element_make multiudpsink failed !!");
      return FALSE;
    }
  }
  return TRUE;
}

gboolean HmpBin::add_client_addr(string clientIp, gint clientPort, gboolean addExistingClients)
{
 XGLOG_INFO("HmpBin::add_client_addr %s:%d start", (char*)ur_log_string(clientIp), clientPort);
  if(m_udpSend) {
    string allClientAddrs("");
    gchar *existingClientAddr;

    char clientAddr[40] = {0};
    snprintf(clientAddr, 40, "%s:%d", clientIp.c_str(), clientPort);
    allClientAddrs.append(clientAddr);

    if(addExistingClients) {
      g_object_get(G_OBJECT(m_udpSend),"clients", &existingClientAddr, NULL);
      if(existingClientAddr[0] != 0) {
        allClientAddrs += ",";
        allClientAddrs.append(existingClientAddr);
      }
      g_free(existingClientAddr);
    }

    g_object_set(G_OBJECT(m_udpSend),"clients", allClientAddrs.c_str(), NULL);
    return TRUE;
  } else {
    return FALSE;
  }
  return TRUE;
}

gboolean HmpBin::clear_client_addr()
{
  if(m_udpSend) {
    char clientAddr[40] = {0};
    snprintf(clientAddr, 40, "%s", "0.0.0.0:0");   
    g_object_set(G_OBJECT(m_udpSend),"clients", clientAddr, NULL); 
  } else return FALSE;

  return TRUE; 
}

GstCaps * HmpBin::get_caps()
{
  GstCaps *caps = NULL;
  switch (m_binType) {
    case EN_BIN_G711U_RECV:
      {
       XGLOG_INFO("HmpBin::get_caps for G711U_RECV bin from UDP_CAPS_G711U string");
        caps = gst_caps_from_string(UDP_CAPS_G711U);  
        break;
      }
    case EN_BIN_G711A_RECV:
      {
       XGLOG_INFO("HmpBin::get_caps for G711A_RECV bin from UDP_CAPS_G711A string");
        caps = gst_caps_from_string(UDP_CAPS_G711A);  
        break;
      }
    case EN_BIN_G722_RECV:
      {
       XGLOG_INFO("HmpBin::get_caps for G722_RECV bin from UDP_CAPS_G722 string");
        caps = gst_caps_from_string(UDP_CAPS_G722);  
        break;
      }
    case EN_BIN_DTLS_RECV:
      {
       XGLOG_INFO("HmpBin::get_caps for DTLS_RECV bin from UDP_CAPS_DTLS string");
        caps = gst_caps_from_string(UDP_CAPS_DTLS);  
        break;
      }
    case EN_BIN_OPUS_RECV:
      {
       XGLOG_INFO("HmpBin::get_caps for OPUS_RECV bin from UDP_CAPS_OPUS string");
        m_payload = UDP_CAPS_OPUS + m_payload;
        caps = gst_caps_from_string(m_payload.c_str());
        cout<<"The HmpBin::get_caps for OPUS_RECV bin"<<m_payload<<endl;
       // caps = gst_caps_from_string(UDP_CAPS_OPUS);  
        break;
      }
    case EN_BIN_G729_RECV:
      {
       XGLOG_INFO("HmpBin::get_caps for G729_RECV bin from UDP_CAPS_G729 string");
        caps = gst_caps_from_string(UDP_CAPS_G729);  
        break;
      }
    case EN_BIN_DTLS_SEND:
      {
       XGLOG_INFO("HmpBin::get_caps for DTSL_SEND bin from UDP_CAPS_OPUS string");
        caps = gst_caps_from_string(UDP_CAPS_OPUS);  
        break;
      }
    case EN_BIN_G711U_SEND:
    case EN_BIN_G711A_SEND:
    case EN_BIN_G722_SEND:
    case EN_BIN_OPUS_SEND:
    case EN_BIN_G729_SEND:
    case EN_BIN_LOCAL_AUDIO_SEND:
    case EN_BIN_LOCAL_AUDIO_RECV:
      {
       XGLOG_INFO("HmpBin::get_caps for SEND bin from string UDP_CAPS_L16");
        caps = gst_caps_from_string(UDP_CAPS_L16);  
        break;
      }
    default:
      {
       XGLOG_INFO("HmpBin::get_caps failed for Unknown/Unsupported bin");
        break;
      }
  }
  return caps;
}

gboolean HmpBin::draw_dot_file()
{
  if(GST_DEBUG && m_bin && !m_binName.empty()) {
    GST_DEBUG_BIN_TO_DOT_FILE(m_bin, GST_DEBUG_GRAPH_SHOW_ALL, m_binName.c_str());
    return TRUE;
  }
  return FALSE;
}
/* HmpBin Base class - End
*/

/* HmpVoiceRecvBin derived class - End
*/
HmpVoiceRecvBin::HmpVoiceRecvBin(string binName, HmpBinType binType, HmpCodecType codecType, \
    gint recvPort, GSocket *recvSock) : HmpBin(binName, binType), m_codecType(codecType), \
    m_decJitterBuffer(NULL), m_rtpDepay(NULL),  m_audioDecoder(NULL), m_audioConvert(NULL), m_audioResample(NULL)
{
 XGLOG_INFO("HmpVoiceRecvBin::HmpVoiceRecvBin construction start");
  m_recvPort = recvPort;
  m_recvSocket = recvSock;
}

HmpVoiceRecvBin::~HmpVoiceRecvBin()
{
 XGLOG_INFO("HmpVoiceRecvBin::~HmpVoiceRecvBin destruction start");
}

gboolean HmpVoiceRecvBin::construct()
{
 XGLOG_INFO("HmpVoiceRecvBin::construct start");

  if(!create_udpsrc()) {
   XGLOG_ERROR("construct VoiceRecvBin, creating udpsrc failed !!");
    return FALSE;
  }

  //create required elements
  if(!create_elements()) {
   XGLOG_ERROR("construct VoiceRecvBin, creating elements failed !!");
    return FALSE;
  }

  //add and link elements in bin container
  if(!add_and_link_elements()) {
   XGLOG_ERROR("construct VoiceRecvBin, add and linking elements failed !!");
    return FALSE;
  }
  draw_dot_file();
  return TRUE;
}

gboolean HmpVoiceRecvBin::create_elements()
{
 XGLOG_INFO("HmpVoiceRecvBin::create_elements start");
  guint latency = 30;
  //create elements based on bin type
  switch(m_binType) {
    case EN_BIN_G711U_RECV:
      {
       XGLOG_INFO("create_decode_elements for G711U_RECV bin");
        m_rtpDepay = gst_element_factory_make("rtppcmudepay", NULL);
        m_audioDecoder = gst_element_factory_make("mulawdec", NULL);
        break;
      }
    case EN_BIN_G711A_RECV:
      {
       XGLOG_INFO("create_decode_elements for G711A_RECV bin");
        m_rtpDepay = gst_element_factory_make("rtppcmadepay", NULL);
        m_audioDecoder = gst_element_factory_make("alawdec", NULL);
        break;
      }
    case EN_BIN_G722_RECV:
      {
       XGLOG_INFO("create_decode_elements for G722_RECV bin");
        m_rtpDepay = gst_element_factory_make("rtpg722depay", NULL);
        m_audioDecoder = gst_element_factory_make("avdec_g722", NULL);
        break;
      }
    case EN_BIN_G729_RECV:
      {
       XGLOG_INFO("create_decode_elements for G729_RECV bin");
        m_rtpDepay = gst_element_factory_make("rtpg729depay", NULL);
        m_audioDecoder = gst_element_factory_make("avdec_g729", NULL);
        break;
      }
    case EN_BIN_OPUS_RECV:
      {
      XGLOG_INFO("create_decode_elements for OPUS_RECV bin");
        m_rtpDepay = gst_element_factory_make("rtpopusdepay", NULL);
        m_audioDecoder = gst_element_factory_make("opusdec", NULL);
        break;
      }
    default:
      {
       XGLOG_ERROR("HmpVoiceRecvBin::create_elements failed. unknown/unsupported bin type !!");
        return FALSE;
      }
  }
  m_decJitterBuffer = gst_element_factory_make("rtpjitterbuffer", NULL);
  m_audioConvert = gst_element_factory_make("audioconvert", NULL);
  g_object_set(G_OBJECT(m_decJitterBuffer), "latency", latency, NULL);
  //g_object_set(G_OBJECT(m_audioConvert), "noise-shaping", 2, NULL);

  return TRUE;
}

gboolean HmpVoiceRecvBin::add_and_link_elements()
{
 XGLOG_INFO("HmpVoiceRecvBin::add_and_link_elements start");

  if(!m_decJitterBuffer || !m_rtpDepay || !m_audioDecoder || !m_audioConvert) {
   XGLOG_ERROR("add_and_link_elements failed. elements not creaed properly !!");
    return FALSE;
  } else {
    gst_bin_add_many(m_bin, m_udpRecv, m_decJitterBuffer, m_rtpDepay, m_audioDecoder, m_audioConvert, NULL);
    gst_element_link_many(m_udpRecv, m_decJitterBuffer, m_rtpDepay, m_audioDecoder, m_audioConvert, NULL);
  }

  GstPad *pad = gst_element_get_static_pad(m_audioConvert, "src");
  if(pad) {
    m_ghostRtpSrcPad = gst_ghost_pad_new("src", pad);
    gst_element_add_pad(GST_ELEMENT(m_bin), m_ghostRtpSrcPad);
    gst_object_unref(pad);
  }

  return TRUE;
}
/* HmpVoiceRecvBin derived class - End
*/


/* HmpRecordAudioBin derived class - Start
*/
HmpRecordAudioBin::HmpRecordAudioBin(string binName, HmpBinType binType, HmpCodecType codecType) :
  HmpBin(binName, binType), m_mp3Enc(NULL), m_fileSink1(NULL), m_filePath1(""), 
  m_audioResample(NULL), m_filter(NULL) 
{
}

HmpRecordAudioBin::~HmpRecordAudioBin()
{
}

gboolean HmpRecordAudioBin::construct()
{
 XGLOG_INFO("construct RecordAudioBin: %s start", (char*)ur_log_string(m_binName));
  //create required elements
  m_audioResample = gst_element_factory_make("audioresample", NULL);
  m_filter = gst_element_factory_make("capsfilter", NULL);
  GstCaps *audioCaps = gst_caps_from_string("audio/x-raw, channels=(int)1, rate=(int)8000");
  g_object_set(G_OBJECT(m_filter), "caps", audioCaps, NULL);
  m_mp3Enc = gst_element_factory_make("wavenc", NULL);
  //m_mp3Enc = gst_element_factory_make("lamemp3enc", NULL); 
  m_fileSink1 = gst_element_factory_make("filesink", NULL);
  g_object_set(G_OBJECT(m_fileSink1), "append", true, NULL);

  if(!m_audioResample || !m_mp3Enc || !m_fileSink1) {
   XGLOG_ERROR("construct RecordAudioBin, elements creation failed !!");
    return FALSE;
  }

  //add and link elements in bin container
  gst_bin_add_many(m_bin, m_audioResample, m_filter, m_mp3Enc, m_fileSink1, NULL);
  gst_element_link_many(m_audioResample, m_filter, m_mp3Enc, m_fileSink1, NULL);

  GstPad *pad = gst_element_get_static_pad(m_audioResample, "sink");
  if(pad) {
    m_ghostRtpSinkPad = gst_ghost_pad_new("sink", pad);
    gst_element_add_pad(GST_ELEMENT(m_bin), m_ghostRtpSinkPad);
    gst_object_unref(pad);
  }
  gst_caps_unref(audioCaps);
  return TRUE;
}

gboolean HmpRecordAudioBin::set_record_file(string &recordFile)
{
 XGLOG_INFO("HmpRecordAudioBin::set_record_file %s", (char*)ur_log_string(recordFile));
  m_filePath1 = recordFile;
  size_t found = m_filePath1.find_last_of("/\\");
  string path = m_filePath1.substr(0, found);
  xGateHmpGstUtil::makeDir(path.c_str());
  if(m_fileSink1) {
    g_object_set(G_OBJECT(m_fileSink1), "location", m_filePath1.c_str(), NULL);
    path.clear();
    return TRUE;
  } else {
   XGLOG_ERROR("set_record_file failed to set file path in filesink !!");
    return FALSE;
  }
  return FALSE;
}
/* HmpRecordAudioBin derived class - End
*/

/* HmpChannel bass class - Start
*/
HmpChannel::HmpChannel(string pipelineName, HmpChannelType pipelineType) :
  m_pipelineName(pipelineName), m_pipelineType(pipelineType), m_pipeline(NULL),
  m_bus(NULL), m_busId(0) 
{
 XGLOG_INFO("HmpChannel::HmpChannel construction start");
  m_pipeline = gst_pipeline_new(m_pipelineName.c_str());
  if(!m_pipeline) {
   XGLOG_ERROR("HmpChannel::HmpChannel, %s pipeline object creation failed!");
    return;
  }

  m_bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
}

HmpChannel::~HmpChannel()
{
 XGLOG_INFO("HmpChannel::~HmpChannel destructing %s start", (char*)ur_log_string(m_pipelineName));
 //TODO: need to properly dispose commanded for call record testing

  if(m_bus) {
    gst_bus_set_sync_handler(m_bus, NULL, NULL, NULL);
    gst_object_unref(GST_OBJECT (m_bus));
  }

  if(m_busId) {
    g_source_remove (m_busId);
  }

 //TODO: need to properly dispose commanded for call record testing
  if(m_pipeline) {
    gst_element_set_state(m_pipeline, GST_STATE_NULL);
    gst_object_unref (GST_OBJECT (m_pipeline));
    m_pipeline = NULL;
    return;
  }
}

/*gboolean HmpChannel::register_handle_event_callback(gpointer data)
{
  if(m_pipeline && m_bus && data) {
    m_busId = gst_bus_add_watch(m_bus, &handle_event, data); 
    return (m_busId > 0) ? TRUE : FALSE;
  } 
  return FALSE;
}*/

//gboolean HmpChannel::handle_common_event(GstBus* bus, GstMessage* msg, gpointer data)
gboolean HmpChannel::handle_event(GstBus* bus, GstMessage* msg, gpointer data,string CallbackType)
{
  Client *client = (Client *)data;
  string wavStr="wav";
  if(!client) {
   XGLOG_ERROR("handle_event, invalid client object !!");
    return TRUE;
  }
   
  switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_ERROR:
      {
       XGLOG_INFO("handle_event got GST_MESSAGE_ERROR message--%s",(char*)ur_log_string(CallbackType));
        GError *err = NULL;
        gchar *dbgInfo = NULL;
        if(GST_DEBUG) {
          gst_message_parse_error(msg, &err, &dbgInfo);
          if(err) {
            g_printerr("ERROR from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
           XGLOG_ERROR("ERROR from element %s: %s", GST_OBJECT_NAME(msg->src), err->message);
            g_error_free(err);
          }
          if(dbgInfo) {
            g_printerr("Debugging info: %s\n", (dbgInfo) ? dbgInfo : "none");
           XGLOG_ERROR("Debugging info: %s\n", dbgInfo);
            g_free(dbgInfo);
          }
        } if(CallbackType==wavStr){
           client->m_mp3_enc=1;
           }
          else{
          client->post_message_to_HmpProcessor(false);
          delete client;
	  client = NULL;
          }
        break;
      }
    case GST_MESSAGE_EOS:
     {
	XGLOG_INFO("handle_event got GST_MESSAGE_EOS message");
       client->m_mp3_enc = 1;
       client->m_pHmpRecordMP3Channel->stop();

      // MgMediaDetail m_media_detail;   
      // xGateMgMsg* pMgMsg = NULL;
#if 1
      std::string Filepath = client->m_pHmpRecordMP3Channel->m_pHmpRecordMP3Bin->m_mp3File.c_str();
      std::string delemiter;
      size_t found;
      for (int i=0;i<3;i++){
       found = Filepath.find_last_of("/");
       Filepath.erase(found,Filepath.length());
       chmod (Filepath.c_str() ,S_IRWXU | S_IRWXG | S_IRWXO);
      }

#endif
       if(!client->m_pHmpRecordMP3Channel->m_pHmpRecordMP3Bin->m_mp3File.empty())
       chmod ( client->m_pHmpRecordMP3Channel->m_pHmpRecordMP3Bin->m_mp3File.c_str() ,S_IRWXU | S_IRWXG | S_IRWXO );
     /*  ACE_Task<ACE_MT_SYNCH>* taskPtr = NULL;
          xGateHmpProcessor *m_HmpProcessor=NULL;   
          m_HmpProcessor =  xGateUtil::getHmpProcessor();
       if(!m_HmpProcessor){
        XGLOG_ERROR("handle_event m_HmpProcessor not found");
         return FALSE;
       }
       pMgMsg = new xGateMgMsg();
       if(!pMgMsg) {
        XGLOG_ERROR("handle_event xgateMgMsg creation failed");
         return FALSE;
       }
       pMgMsg->set_mg_msg_type(EN_XGATE_MG_MP3_DETAIL);
       m_media_detail.call_id=client->m_callId;
       m_media_detail.mediaResource_id=client->m_mgresourceId;
       m_media_detail.mp3_filesize=xGateHmpGstUtil::file_size(client->m_pHmpRecordMP3Channel->m_pHmpRecordMP3Bin->m_mp3File);
       if( !client->m_pHmpRecordMP3Channel->m_pHmpRecordMP3Bin->m_filePath1.empty())
       m_media_detail.mp3_duration=xGateHmpGstUtil::find_wav_duration(client->m_pHmpRecordMP3Channel->m_pHmpRecordMP3Bin->m_filePath1);
  
        pMgMsg->set_media_detail(m_media_detail);
       ACE_Message_Block* pAmb = NULL;
       pAmb = static_cast<xGateMgMsg*> (pMgMsg);
       ACE_Time_Value tval ((time(NULL) ));
       tval += ACE_Time_Value(0,1);

       if (taskPtr->putq(pAmb, &tval)) {
       XGLOG_ERROR(handle_event postMessageToHmpProcessor enqueue message success ");
        } else {
       XGLOG_ERROR("handle_event PostMessageToHmpProcessor  enqueue message failed");
        delete pMgMsg;
        return FALSE;
         }
        m_HmpProcessor->process_msg(pMgMsg);

        
       }
       else 
        XGLOG_INFO("HmpChannel::handle_event,m_mp3File contain empty string");

/*****/
         client->post_message_to_HmpProcessor(true);     
       delete client;
       client = NULL;
       break;
     }
    case GST_MESSAGE_UNKNOWN:
    default:
      {
       XGLOG_INFO("handle_event got GST_MESSAGE_UNKNOWN/DEFAULT message -%s",(char*)ur_log_string(CallbackType));
        break;
      }
  }
  return TRUE; //it's important to return TRUE from this function
}

gboolean HmpChannel::draw_dot_file()
{
  gboolean retVal = FALSE;
  if(GST_DEBUG && m_pipeline && !m_pipelineName.empty()) {
    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(m_pipeline), GST_DEBUG_GRAPH_SHOW_ALL, m_pipelineName.c_str());
    retVal = TRUE;
  }
  return retVal;
}

gboolean HmpChannel::start()
{
  gboolean retVal = FALSE;
  if(m_pipeline) {
    gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
    retVal = TRUE;
  }
  draw_dot_file();
  return retVal;
}

gboolean HmpChannel::stop()
{
  gboolean retVal = FALSE;
  if(m_pipeline) {
    gst_element_set_state(m_pipeline, GST_STATE_NULL);
    retVal = TRUE;
  }
  draw_dot_file();
  return retVal;
}

gboolean HmpChannel::hold()
{
  gboolean retVal = FALSE;
  if(m_pipeline) {
    gst_element_set_state(m_pipeline, GST_STATE_NULL);
    retVal = TRUE; 
  }
  draw_dot_file();
  return retVal;
}
/* HmpChannel bass class - End
*/

/* HmpVoiceRecvChannel derived class - Start
*/
HmpVoiceRecvChannel::HmpVoiceRecvChannel(string pipelineName, HmpChannelType pipelineType) :
  HmpChannel(pipelineName, pipelineType) , m_pHmpVoiceRecvBin(NULL)
{
 XGLOG_INFO("HmpVoiceRecvChannel::HmpVoiceRecvChannel construction start");
}

HmpVoiceRecvChannel::~HmpVoiceRecvChannel()
{
 XGLOG_INFO("HmpVoiceRecvChannel::~HmpVoiceRecvChannel destruction start");
  if(m_pHmpVoiceRecvBin) {
    delete m_pHmpVoiceRecvBin;
    m_pHmpVoiceRecvBin = NULL;
  }
}

gboolean HmpVoiceRecvChannel::construct_bin(string binName, HmpBinType binType, \
    HmpCodecType codecType, gint recvPort, GSocket *recvSock)
{
 XGLOG_INFO("HmpVoiceRecvChannel::construct_bin %s start", (char*)ur_log_string(binName));
  if(!m_pHmpVoiceRecvBin) {
    m_pHmpVoiceRecvBin = new HmpVoiceRecvBin(binName, binType, codecType, recvPort, recvSock);
  
     
    long long int codec = codecType;
    cout<<"The codec"<<codec<<endl;
    string codecType = to_string(codec);

    m_pHmpVoiceRecvBin->m_payload = ",payload=(int)";
    m_pHmpVoiceRecvBin->m_payload.append(codecType.c_str());

    if(!m_pHmpVoiceRecvBin->construct()) {
     XGLOG_ERROR("construct_bin, VoiceRecvBin construction failed !!");
      return FALSE;
    } else {
      //gst_bin_add_many(GST_BIN(m_pipeline), GST_ELEMENT(m_pHmpVoiceRecvBin->m_bin), NULL); 
    }
  }  
  return TRUE;
}

GSocket * HmpVoiceRecvChannel::get_recv_socket()
{
  GSocket *socket = NULL;
  if(m_pHmpVoiceRecvBin) {
    socket = m_pHmpVoiceRecvBin->m_recvSocket; 
  }
  return socket;
}

void HmpVoiceRecvChannel::set_recv_socket(GSocket *socket)
{
  if(m_pHmpVoiceRecvBin) {
    m_pHmpVoiceRecvBin->m_recvSocket = socket; 
  }
}

gint HmpVoiceRecvChannel::get_recv_port()
{
  gint recvPort = 0;
  if(m_pHmpVoiceRecvBin) {
    recvPort = m_pHmpVoiceRecvBin->m_recvPort; 
  }
  return recvPort;
}

void HmpVoiceRecvChannel::set_recv_port(gint port)
{
  if(m_pHmpVoiceRecvBin) {
    m_pHmpVoiceRecvBin->m_recvPort = port; 
  }
}

gboolean HmpVoiceRecvChannel::add_client_addr(string clientIp, gint clientPort, gboolean addExistingClients)
{
  if(m_pHmpVoiceRecvBin) {
    m_pHmpVoiceRecvBin->add_client_addr(clientIp, clientPort, addExistingClients);
    return TRUE;
  }
  return FALSE;  
}

gboolean HmpVoiceRecvChannel::clear_client_addr()
{
  if(m_pHmpVoiceRecvBin) {
    m_pHmpVoiceRecvBin->clear_client_addr();
    return TRUE;
  }
  return FALSE;
}

/* HmpRecordAudioChannel derived class - Start
*/
HmpRecordAudioChannel::HmpRecordAudioChannel(string pipelineName, HmpChannelType pipelineType) :
  HmpChannel(pipelineName, pipelineType), m_pHmpRecordAudioBin(NULL), m_maxTimeoutId(0),
  m_maxRecordTime(300),m_busId(0)
{
 XGLOG_INFO("HmpRecordAudioChannel::HmpRecordAudioChannel construction start");
}

HmpRecordAudioChannel::~HmpRecordAudioChannel()
{
 XGLOG_INFO("HmpRecordAudioChannel::~HmpRecordAudioChannel destruction start");
  if(m_pHmpRecordAudioBin) {
    delete m_pHmpRecordAudioBin;
    m_pHmpRecordAudioBin = NULL;
  }

  if(m_maxTimeoutId) {
    g_source_remove(m_maxTimeoutId);
    m_maxTimeoutId = 0;
  }

  if(m_busId) {
    g_source_remove (m_busId);
  }
}
gboolean HmpRecordAudioChannel::register_handle_wav_event_callback(gpointer data)
{
  if(m_pipeline && m_bus && data) {
    m_busId = gst_bus_add_watch(m_bus, &handle_wav_event, data);
 // XGLOG_INFO("HmpRecordAudioChannel::register_handle_wav_event_callback start,m_busId--%d",m_busId);
    return (m_busId != nullptr);
  }
  return FALSE;
}
gboolean  HmpRecordAudioChannel::handle_wav_event(GstBus* bus, GstMessage* msg, gpointer data){

string CallbackType="wav";
if(handle_event(bus,msg,data,CallbackType))
return TRUE;

return FALSE;

}
gboolean HmpRecordAudioChannel::construct_bin(string binName, HmpBinType binType,HmpCodecType codecType)
{
 XGLOG_INFO("HmpRecordAudioChannel::construct_bin %s start", (char*)ur_log_string(binName));
  if(!m_pHmpRecordAudioBin) {
    m_pHmpRecordAudioBin = new HmpRecordAudioBin(binName, binType,codecType); 


     /*
     long long int codec = codecType;
     cout<<"The codec"<<codec<<endl;
     string codecType = to_string(codec);

  
     m_pHmpRecordAudioBin->m_payload = ",payload=(int)";
     m_pHmpRecordAudioBin->m_payload.append(codecType.c_str());    
  */
    if(!m_pHmpRecordAudioBin->construct()) {
     XGLOG_ERROR("construct_bin, HmpRecordAudioBin construction failed !!");
      return FALSE;
    } else {
      gst_bin_add_many(GST_BIN(m_pipeline), GST_ELEMENT(m_pHmpRecordAudioBin->m_bin), NULL); 
    }
  }  
  return TRUE;
}

gboolean HmpRecordAudioChannel::set_record_file(string &recordFile)
{
 XGLOG_INFO("HmpRecordAudioChannel:set_record_file %s start", (char*)ur_log_string(recordFile));
  if(m_pHmpRecordAudioBin) {
    return m_pHmpRecordAudioBin->set_record_file(recordFile);
  }
  return FALSE;
}
#if 0
gboolean HmpRecordAudioChannel::add_client(Client *client)
{
 XGLOG_INFO("HmpRecordAudioChannel:add_client start");

  if(!m_pHmpRecordAudioBin) {
   XGLOG_ERROR("HmpRecordAudioChannel::add_client for callid %s failed. Invalid HmpRecordAudioBin object !!", client->m_detail.m_callId.c_str());
    return FALSE;
  }
  HmpVoiceRecvBin *hmpVoiceRecvBin = client->m_pHmpVoiceRecvChannel->m_pHmpVoiceRecvBin;

  if(!hmpVoiceRecvBin) {
   XGLOG_ERROR("HmpRecordAudioChannel::add_client for callid:%s failed. No receive bib created %s", client->m_detail.m_callId.c_str());
    return FALSE;
  } 

  gst_bin_add_many(GST_BIN(m_pipeline), GST_ELEMENT(hmpVoiceRecvBin->m_bin), NULL);  
  gst_element_link_many(GST_ELEMENT(hmpVoiceRecvBin->m_bin), GST_ELEMENT(m_pHmpRecordAudioBin->m_bin), NULL);

  return TRUE;
}
#endif
gboolean HmpRecordAudioChannel::schedule_timeout(Client *client)
{
  //TODO: schedule the max recording time
  if(m_maxTimeoutId == 0) {
    unsigned int legId = client->m_detail.m_legId;
    m_maxTimeoutId = g_timeout_add_seconds(m_maxRecordTime, G_CALLBACK(&timeout_handler), client); 
    g_print("******* scheduled max record timeout ********\n");
   XGLOG_INFO("add_client, successfully schedule max record timeout for leg:%d", legId);
    return TRUE;
  }
  return FALSE;
}

gboolean HmpRecordAudioChannel::timeout_handler(gpointer data)
{
 XGLOG_INFO("HmpRecordAudioChannel:timeout_handler stop call recording");
  Client *client = (Client *)data;
  if(client) {
    unsigned int legId = client->m_detail.m_legId;
    HmpRecordAudioChannel *recordChannel = client->m_pHmpRecordAudioChannel;
    recordChannel->stop();
    g_source_remove(recordChannel->m_maxTimeoutId);
    recordChannel->m_maxTimeoutId = 0;
    g_print("******* max recording time reached. stopped call recording for leg:%d ********\n", legId);
   XGLOG_INFO("timeout_handler stopeed call recording for leg:%d", legId);

    //post the details to pbx
    recordChannel->get_record_details(client);
    client->post_media_event_result(EN_MEDIA_EVENT_RECORD_INFO);
  }
  return FALSE; //return false to avoid repeated callback invoking
}

gboolean HmpRecordAudioChannel::get_record_details(Client *client)
{
  //read the recorded file size
  struct stat st;
  long int fileSize = 0;
  if(stat(m_pHmpRecordAudioBin->m_filePath1.c_str(),&st)==0) {
    fileSize = st.st_size;
  }
  
  client->m_detail.m_filePath = m_pHmpRecordAudioBin->m_filePath1;
  client->m_detail.m_fileSize = fileSize;
  return TRUE;
}

gboolean HmpRecordAudioChannel::send_eos()
{
  gst_element_send_event(GST_ELEMENT(m_pipeline), gst_event_new_eos());
  return TRUE;
}

HmpRecordMP3Channel::HmpRecordMP3Channel(string pipelineName, HmpChannelType pipelineType) :
  HmpChannel(pipelineName, pipelineType), m_pHmpRecordMP3Bin(NULL),m_busId(0)
{
 XGLOG_INFO("HmpRecordMP3Channel::HmpRecordMP3Channel construction start");

}

HmpRecordMP3Channel::~HmpRecordMP3Channel()
{
 XGLOG_INFO("HmpRecordMP3Channel::~HmpRecordMP3Channel destruction start");
  if(m_pHmpRecordMP3Bin) {
    delete m_pHmpRecordMP3Bin;
    m_pHmpRecordMP3Bin = NULL;
  }

  if(m_busId) {
    g_source_remove (m_busId);
  }
}

gboolean HmpRecordMP3Channel::register_handle_mp3_event_callback(gpointer data)
{
  if(m_pipeline && m_bus && data) {
    m_busId = gst_bus_add_watch(m_bus, &handle_mp3_event, data);
  //XGLOG_INFO("HmpRecordAudioChannel::register_handle_mp3_event_callback start,m_busId--%d",m_busId);
    return (m_busId != nullptr);
  }
  return FALSE;
}
gboolean HmpRecordMP3Channel::handle_mp3_event(GstBus* bus, GstMessage* msg, gpointer data){

string CallbackType="mp3";
if(handle_event(bus,msg,data,CallbackType))
return TRUE;

return FALSE;
}
/* HmpRecordAudioChannel derived class - End */
gboolean HmpRecordMP3Channel::construct_bin(string binName, HmpBinType binType)
{
 XGLOG_INFO("HmpRecordMP3Channel::construct_bin %s start", (char*)ur_log_string(binName));
  if(!m_pHmpRecordMP3Bin) {
    m_pHmpRecordMP3Bin = new HmpRecordMP3Bin(binName, binType);
 
    if(!m_pHmpRecordMP3Bin->construct()) {
     XGLOG_ERROR("construct_bin, HmpRecordMP3Bin construction failed !!");
      return FALSE;
    } else {
      gst_bin_add_many(GST_BIN(m_pipeline), GST_ELEMENT(m_pHmpRecordMP3Bin->m_bin), NULL); 
    }
  }  
  return TRUE;
}
 
gboolean HmpRecordMP3Channel::set_record_file(string &recordFile)
{
 XGLOG_INFO("HmpRecordMP3Channel::set_record_file %s start", (char*)ur_log_string(recordFile));
  if(m_pHmpRecordMP3Bin) {
    return m_pHmpRecordMP3Bin->set_record_file(recordFile);
  }
  return FALSE;
}

  HmpRecordMP3Bin::HmpRecordMP3Bin(string binName, HmpBinType binType) :
  HmpBin(binName, binType), m_mp3Encoder(NULL), m_fileSink(NULL), m_fileSrc(NULL), 
  m_audioResample(NULL), m_audioConvert(NULL),m_wavParse(NULL),m_mp3File(""),m_filePath1("") 
 {
 XGLOG_INFO("HmpRecordMP3Bin construction start");
 }
 HmpRecordMP3Bin::~HmpRecordMP3Bin(){
 XGLOG_INFO("HmpRecordMP3Bin destruction start");
 }

 gboolean HmpRecordMP3Bin::set_record_file(string &recordFile)
 {
 XGLOG_INFO("HmpRecordMP3Bin::set_record_file %s", (char*)ur_log_string(recordFile));
  m_filePath1 = recordFile;
  //m_filePath1 ="/home/nfsshare/zero_wav.wav";
  size_t found = m_filePath1.find_last_of(".");
  string path = m_filePath1.substr(0, found);
  path = path + ".mp3";
  m_mp3File = path;
  XGLOG_INFO("HmpRecordMP3Bin::set_record_file mp3 file name is %s",(char*)ur_log_string(path));
  if(m_fileSrc && m_fileSink) {
    g_object_set(G_OBJECT(m_fileSrc), "location", m_filePath1.c_str(), NULL);
    g_object_set(G_OBJECT(m_fileSink), "location", path.c_str(), NULL);
    path.clear();
    return TRUE;
  } else {
   XGLOG_ERROR("set_record_file failed to set file path in filesink !!");
    return FALSE;
  }
  return FALSE;
  }

 gboolean HmpRecordMP3Bin::construct(){
 XGLOG_INFO("construct RecordMP3Bin: %s start", (char*)ur_log_string(m_binName));
   //Make elements here 
   //float mp3EncQuality = 5.0;
   m_fileSrc = gst_element_factory_make("filesrc",NULL);
   m_wavParse = gst_element_factory_make("wavparse",NULL);
   m_audioConvert = gst_element_factory_make("audioconvert",NULL);
   m_audioResample = gst_element_factory_make("audioresample",NULL);
   m_mp3Encoder = gst_element_factory_make("lamemp3enc",NULL);
   //g_object_set(G_OBJECT(m_mp3Encoder),"quality",mp3EncQuality,NULL);
   g_object_set(G_OBJECT(m_mp3Encoder),"target",1,NULL);
   g_object_set(G_OBJECT(m_mp3Encoder),"cbr",TRUE,NULL);
   m_fileSink = gst_element_factory_make("filesink",NULL);
   g_object_set(G_OBJECT(m_fileSink), "append", true, NULL);
  
    if(!m_audioResample || !m_mp3Encoder || !m_fileSink || !m_audioConvert || !m_fileSrc || !m_wavParse) {
      XGLOG_ERROR("construct RecordMP3Bin, elements creation failed !!");
       return FALSE;
     }

  //add and link elements in bin container
  gst_bin_add_many(m_bin,m_fileSrc,m_wavParse,m_audioConvert,m_audioResample,m_mp3Encoder,m_fileSink, NULL);
  gst_element_link_many(m_fileSrc,m_wavParse,m_audioConvert,m_audioResample,m_mp3Encoder,m_fileSink, NULL);
 }

