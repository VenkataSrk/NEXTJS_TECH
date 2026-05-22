#include "xglog.h"
#include "xGateBaseMsg.h"
#include "MediaMsgProcessor.h"
#include "xGateMediaService.h"
#include "xGateMultiSessionTracker.h"

MediaMsgProcessor::MediaMsgProcessor() :
  ACE_Task<ACE_MT_SYNCH>(),
  m_run(false)
{
}

MediaMsgProcessor::~MediaMsgProcessor(void)
{
}

bool MediaMsgProcessor::stop(void)
{
  m_run = false;
  msg_queue()->deactivate();
  wait();
  XGLOG_INFO("MediaMsgProcessor stopped !");
  return true;
}

bool MediaMsgProcessor::init()
{
  m_run = true;
  if(activate(THR_NEW_LWP, 1) == -1) {
    XGLOG_ERROR("MediaMsgProcessor::init() failed to start VMS controller thread !");
    return false;
  }
  XGLOG_INFO("Initialized MediaMsgProcessor successfully !");
  return true;
}

int MediaMsgProcessor::svc(void)
{
  ACE_Message_Block *pAmb = 0;

  while(m_run) {
    ACE_Time_Value tv((time(NULL) + 20));
    int result = getq(pAmb, &tv);
    if((result == -1) && (errno == EWOULDBLOCK)) {
      continue;
    }

    if((result != -1)) {
      handle_msg(pAmb);
    } else {
      XGLOG_ERROR("MediaMsgProcessor::svc Error in Message Queue");
      m_run = false;
    }

    // delete the message received
    if(pAmb) {
      delete pAmb;
      pAmb = NULL;
    }
  }

  XGLOG_INFO("MediaMsgProcessor::svc() stopped");
  return 0;
}

bool MediaMsgProcessor::handle_msg(ACE_Message_Block *pAmb)
{
  const char* buf = pAmb->base();
  size_t bufLen = pAmb->length();

  if(!decode_msg(buf, bufLen)) {
    XGLOG_ERROR("handle_msg failed due to decode message failure !");
    return false; 
  }

  return true;
}

bool MediaMsgProcessor::decode_msg(const char* buf, size_t bufLen)
{
  if(!buf && bufLen <=0) {
    XGLOG_ERROR("decode_msg failed. given data is invalid and data length is %d !", bufLen);
    return false;
  }
 
  Document doc;

  if(!create_doc_object(doc, buf)) {
    XGLOG_ERROR("decode_msg failed while creating json document object !");
    return false;
  }

  MgMediaDetail mediaDetail;
  mediaDetail.mgmsg_type = doc["msg_type"].GetInt();
  const Value &valData = doc["data"];
  string strMediaType = MgMediaDetail::getMediaMsgType(mediaDetail.mgmsg_type);

  XGLOG_DEBUG("decoding %d length of message type %s json buffer: %s",bufLen,strMediaType.c_str(),  buf);

  if(!read_media_detail(valData, mediaDetail)) {
    XGLOG_ERROR("decode_msg failed while reading media detail from json buffer !");
    return false;
  }
  
  
  if(post(mediaDetail) != EN_XGATE_STATUS_SUCCESS) {
    return EN_XGATE_STATUS_ERROR;
  }
  return false;
}

bool MediaMsgProcessor::create_doc_object(Document &doc, const char *jsonBuf)
{
  ParseResult ok = doc.Parse<kParseCommentsFlag>(jsonBuf);
  if(!ok) {
    XGLOG_ERROR("create_doc_object failed. due to json parse error: %s !", GetParseError_En(ok.Code()));
    return false;
  } else if(!doc.IsObject()) {
    XGLOG_ERROR("create_doc_object failed. not able to create document object !");
    return false;
  } else if(!doc.HasMember("msg_type")) {
    XGLOG_ERROR("create_doc_object failed. 'msg_type' field is missing in json message !");
    return false;
  } else if(!doc.HasMember("data")) {
    XGLOG_ERROR("create_doc_object failed. 'data' field is missing in json message !");
    return false;
  }
  XGLOG_DEBUG("successfully created json document object and extracted the 'data' json object");
  return true;
}

bool MediaMsgProcessor::read_media_detail(const Value& val, MgMediaDetail& detail)
{
  if(val.HasMember("mgresource_id")) {
    detail.mgresource_id = val["mgresource_id"].GetString();
  } else {
    XGLOG_ERROR("read_media_detail failed. 'mgresource_id' field is missing !");
    return false;
  }

  if(val.HasMember("call_id")) {
    detail.call_id = val["call_id"].GetString();
  } else {
    XGLOG_ERROR("read_media_detail failed. 'call_id' field is missing !");
    return false;
  }

  if(val.HasMember("call_type")) {
    detail.call_type = val["call_type"].GetInt();
  } else {
    XGLOG_ERROR("read_media_detail failed. 'call_type' field is missing !");
    return false;
  }

  if(val.HasMember("context_id")) {
    detail.context_id = val["context_id"].GetInt();
  } else {
    XGLOG_ERROR("read_media_detail failed. 'context_id' field is missing !");
    return false;
  }

  if(val.HasMember("gateway_id")) {
    detail.gateway_id = val["gateway_id"].GetInt();
  } else {
    XGLOG_ERROR("read_media_detail failed. 'gateway_id' field is missing !");
    return false;
  }

  if(val.HasMember("media_state")) {
    detail.media_event = val["media_state"].GetInt();
  } else {
    XGLOG_ERROR("read_media_detail failed. 'media_state' field is missing !");
    return false;
  }

  if(val.HasMember("leg_id")) {
    detail.leg_id = val["leg_id"].GetInt();
  } else {
    XGLOG_ERROR("read_media_detail failed. 'leg_id' field is missing !");
    return false;
  }

  if(val.HasMember("sig_type")) {
    detail.sig_type = val["sig_type"].GetInt();
  } else {
    XGLOG_ERROR("read_media_detail failed. 'sig_type' field is missing !");
    return false;
  }

  if(val.HasMember("call_dir")) {
    detail.call_dir = val["call_dir"].GetInt();
  } else {
    XGLOG_ERROR("read_media_detail failed. 'call_dir' field is missing !");
    return false;
  }

  if(val.HasMember("media_proto")) {
    detail.media_proto = val["media_proto"].GetString();
  } else {
    XGLOG_ERROR("read_media_detail failed. 'media_proto' field is missing !");
    return false;
  }

  if(val.HasMember("out_proto")) {
    detail.out_proto = val["out_proto"].GetString();
  } else {
    XGLOG_ERROR("read_media_detail failed. 'out_proto' field is missing !");
    return false;
  }

  if(val.HasMember("dtmf_digits")) {
    detail.dtmf_digits = val["dtmf_digits"].GetString();
  } else {
    XGLOG_ERROR("read_media_detail failed. 'out_proto' field is missing !");
    return false;
  }

  if(val.HasMember("sdpinfo")) {
    const Value& valSdpInfo = val["sdpinfo"];
#if 0
    if(!read_sdpinfo(valSdpInfo, detail)) {
      XGLOG_ERROR("read_media_detail failed while reading sdpinfo !");
      return false;
    }
#else
    read_sdpinfo(valSdpInfo, detail);
#endif
  } else {
    XGLOG_ERROR("read_media_detail failed. 'sdpinfo' field is missing !");
    return false;
  }
  return true;
}

bool MediaMsgProcessor::read_sdpinfo(const Value& val, MgMediaDetail& detail)
{
  if(!val.IsArray() || val.Size() <= 0) {
    XGLOG_WARN("read_sdpinfo failed. It's not an array object !");
    return false;
  }

  for(int i=0; i<val.Size(); i++) {
    const Value& valSdpInfo = val[i];   
    if(!valSdpInfo.IsObject()) {
      XGLOG_ERROR("fill_sdpinfo failed. not able to fetch valid sdpinfo object from an array list !");
      return false;   
    }

    if(valSdpInfo.HasMember("media_type")) {
      unsigned int sdpType = valSdpInfo["media_type"].GetInt();
      switch(sdpType) {
        case EN_MEDIA_SDPINFO_AUDIO: 
          {
            if(!read_audio_sdpinfo(valSdpInfo, detail)) {
              XGLOG_ERROR("read_sdpinfo failed while reading audio sdp info !");
              return false;
            }
            break;
          }
        case EN_MEDIA_SDPINFO_VIDEO:
          {
            if(!read_video_sdpinfo(valSdpInfo, detail)) {
              XGLOG_ERROR("read_sdpinfo failed while reading video sdp info !");
              return false;
            }
            break;
          }
        case EN_MEDIA_SDPINFO_UNKNOWN:
        default:
          {
            XGLOG_ERROR("read_sdpinfo failed. Unknown / Unsupported sdp type !");
            return false;
          }   
      }
    }
  }

  return true;
}

bool MediaMsgProcessor::read_audio_sdpinfo(const Value& val, MgMediaDetail& detail)
{
  AudioDetail &audioSdpInfo = detail.audioDetail; 
  if(val.HasMember("fmtp")) {
    audioSdpInfo.m_fmtp = val["fmtp"].GetString(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'fmtp' field is missing !");
  }

  if(val.HasMember("codec")) {
    audioSdpInfo.m_codec = val["codec"].GetInt(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'codec' field is missing !");
    return false;
  }

  if(val.HasMember("codec_name")) {
    audioSdpInfo.m_codecName = val["codec_name"].GetString(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'codec_name' field is missing !");
    return false;
  }

  if(val.HasMember("ptime")) {
    audioSdpInfo.m_ptime = val["ptime"].GetInt(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'ptime' field is missing !");
    return false;
  }

  if(val.HasMember("play_file")) {
    audioSdpInfo.m_playFile = val["play_file"].GetString(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'play_file' field is missing !");
    return false;
  }
  
  if(val.HasMember("ip_addr")) {
    audioSdpInfo.m_clientIp = val["ip_addr"].GetString(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'ip_addr' field is missing !");
    return false;
  }

  if(val.HasMember("ip_port")) {
    audioSdpInfo.m_clientPort = val["ip_port"].GetInt(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'ip_port' field is missing !");
    return false;
  }

  if(val.HasMember("relayip_addr")) {
    audioSdpInfo.m_relayIp = val["relayip_addr"].GetString(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'relayIp' field is missing !");
    return false;
  }

  if(val.HasMember("relay_port")) {
    audioSdpInfo.m_relayPort = val["relay_port"].GetInt(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'relay_port' field is missing !");
    return false;
  }

  if(val.HasMember("reflexip_addr")) {
    audioSdpInfo.m_rflxIp = val["reflexip_addr"].GetString(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'reflexip_addr' field is missing !");
    return false;
  }

  if(val.HasMember("reflex_port")) {
    audioSdpInfo.m_rflxPort = val["reflex_port"].GetInt(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'reflex_port' field is missing !");
    return false;
  }

  if(val.HasMember("ice_ufrag")) {
    audioSdpInfo.m_secureDetail.m_rIceUfrag = val["ice_ufrag"].GetString(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'ice_ufrag' field is missing !");
    return false;
  }

  if(val.HasMember("ice_pwd")) {
    audioSdpInfo.m_secureDetail.m_rIcePwd = val["ice_pwd"].GetString(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'ice_pwd' field is missing !");
    return false;
  }

  if(val.HasMember("fingerprint")) {
    audioSdpInfo.m_secureDetail.m_rFingerPrint = val["fingerprint"].GetString(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'fingerprint' field is missing !");
    return false;
  }

  if(val.HasMember("ssrc")) {
    audioSdpInfo.m_secureDetail.m_rSsrc = val["ssrc"].GetString(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'ssrc' field is missing !");
    return false;
  }

  if(val.HasMember("cname")) {
    audioSdpInfo.m_secureDetail.m_rCname = val["cname"].GetString(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'cname' field is missing !");
    return false;
  }

  if(val.HasMember("mslable")) {
    audioSdpInfo.m_secureDetail.m_rMsLabel = val["mslable"].GetString(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'mslable' field is missing !");
    return false;
  }

  if(val.HasMember("lable")) {
    audioSdpInfo.m_secureDetail.m_rLabel = val["lable"].GetString(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'lable' field is missing !");
    return false;
  }

  if(val.HasMember("dial_ip")) {
    audioSdpInfo.m_dialIp = val["dial_ip"].GetString(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'dial_ip' field is missing !");
    return false;
  }

  if(val.HasMember("dial_port")) {
    audioSdpInfo.m_dialPort = val["dial_port"].GetInt(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'dial_port' field is missing !");
    return false;
  }

  if(val.HasMember("resp_ip")) {
    audioSdpInfo.m_respIp = val["resp_ip"].GetString(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'resp_ip' field is missing !");
    return false;
  }

  if(val.HasMember("resp_port")) {
    audioSdpInfo.m_respPort = val["resp_port"].GetInt(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'resp_port' field is missing !");
    return false;
  }
  return true;
}

bool MediaMsgProcessor::read_video_sdpinfo(const Value& val, MgMediaDetail& detail)
{
  XGLOG_WARN("read_video_sdpinfo not yet been implemented !");
  return false;
}

bool MediaMsgProcessor::post(MgMediaDetail& mediaDetail)
{
  string uid = mediaDetail.mgresource_id;
  XGLOG_DEBUG("posting media service msg for uid: %s!", uid.c_str());
  //find out to which service to post this message
  xGateMediaServiceMsg *pMsg = new xGateMediaServiceMsg();
  pMsg->setUid(uid);

#if 0
  string uid("yoga");
  SessionDetail *details = SESSIONTRACKER->accessSession(uid);
  SESSIONTRACKER->dropSession(uid);
  if(!details){
    XGLOG_ERROR("post message failed. SessionDetails not found the for uid %s !",\
        uid.c_str());
    return false;
  }
  if((details->m_srcModuleId)==IURDefines::UR_MODULE_SIPSIGNAL_SERVICE){
    pMsg->setMsgType(EN_XGATE_MSG_MGC_MG_OUT);
    delete details;
  }
#endif

  switch(mediaDetail.call_type) {
    case EN_XGATE_CALL_TYPE_VMS:
      {
        XGLOG_DEBUG("processing media response for VMS call: %s", uid.c_str());
        switch(mediaDetail.mgmsg_type) {
          case EN_XGATE_MG_ALLOCATE_TERMINATE:
            {
              pMsg->setDstModuleId(IURDefines::UR_MODULE_SIPSIGNAL_SERVICE);
              pMsg->setMsgType(EN_XGATE_MSG_MGC_MG_OUT);
              break;
            }
          case EN_XGATE_MG_PLAY_BACK:
          case EN_XGATE_MG_DTMF_DIGITS:
          case EN_XGATE_MG_CALL_RECORD:
            {
              //pMsg->setDstModuleId(IURDefines::UR_MODULE_IVR_SERVICE);
              break;
            }
          default:
          {
            XGLOG_ERROR("got unknown media message type in media response for uid: %s", uid.c_str());
            delete pMsg;
            return false;
          }
        }
        break;
      }
    default:
      {
        XGLOG_ERROR("got media response for unknown call type for uid: %s", uid.c_str());
        delete pMsg;
        return false;
      }
  }

  pMsg->set_media_detail(mediaDetail);
  MEDIASERVICE->getConfig()->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(pMsg));
  return true;
}
