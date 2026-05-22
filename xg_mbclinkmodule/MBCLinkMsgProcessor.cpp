#include <iostream>
#include "xglog.h"
#include "xGateBaseMsg.h"
#include "MBCLinkMsgProcessor.h"
#include "xGateMBCLinkService.h"
#include "SipMediahandler.h"

MBCLinkMsgProcessor::MBCLinkMsgProcessor() :
  ACE_Task<ACE_MT_SYNCH>(),
  m_run(false)
{
}

MBCLinkMsgProcessor::~MBCLinkMsgProcessor(void)
{
}

bool MBCLinkMsgProcessor::stop(void)
{
  m_run = false;
  msg_queue()->deactivate();
  wait();
  XGLOG_INFO("MBCLinkMsgProcessor stopped !");
  return true;
}

bool MBCLinkMsgProcessor::init()
{
  m_run = true;
  if(activate(THR_NEW_LWP, 1) == -1) {
    XGLOG_ERROR("MBCLinkMsgProcessor::init() failed to start VMS controller thread !");
    return false;
  }
  XGLOG_INFO("Initialized MBCLinkMsgProcessor successfully !");
  return true;
}

int MBCLinkMsgProcessor::svc(void)
{
  ACE_Message_Block *pAmb = 0;

   int rc;
   rc = pthread_setname_np(ACE_Thread::self(), "MBCLinkMsgProc");
   if(rc!=0)
     printf("Failed to set name for MBCLinkMsgProcessor thread");

  while(m_run) {
    ACE_Time_Value tv((time(NULL) + 20));
    int result = getq(pAmb, &tv);
    if((result == -1) && (errno == EWOULDBLOCK)) {
      continue;
    }

    if((result != -1)) {
      handle_msg(pAmb);
    } else {
      XGLOG_ERROR("MBCLinkMsgProcessor::svc Error in Message Queue");
      m_run = false;
    }

    // delete the message received
    if(pAmb) {
      delete pAmb;
      pAmb = NULL;
    }
  }

  XGLOG_INFO("MBCLinkMsgProcessor::svc() stopped");
  return 0;
}

bool MBCLinkMsgProcessor::handle_msg(ACE_Message_Block *pAmb)
{
  xGateMBCLinkServiceMsg *pMsg = dynamic_cast<xGateMBCLinkServiceMsg*>(pAmb);

  const char* buf = pMsg->getMsg();
  size_t bufLen = strlen(buf);

  if(!decode_msg(buf, bufLen)) {
    XGLOG_ERROR("handle_msg failed due to decode message failure !");
    return false; 
  }

#if 0
  if(pMsg)
  {
     delete pMsg;
     pMsg = NULL;
  }
#endif
  return true;
}

bool MBCLinkMsgProcessor::decode_msg(const char* buf, size_t bufLen)
{
  if(!buf) {
    XGLOG_ERROR("decode_msg failed. given data is invalid and data length is %d !", bufLen);
    return false;
  }
 
  Document doc;

  if(!create_doc_object(doc, buf)) {
    XGLOG_ERROR("decode_msg failed while creating json document object !");
    return false;
  }

  MgMediaDetail mediaDetail;
  mediaDetail.mgmsg_type = doc["msg_type"].GetUint();
  const Value &valData = doc["data"];
  string strMediaType = MgMediaDetail::getMediaMsgType(mediaDetail.mgmsg_type);

  XGLOG_DEBUG("decoding %d length of message type %s json buffer: %s",bufLen,strMediaType.c_str(),buf);

  if(!read_media_detail(valData, mediaDetail)) {
    XGLOG_ERROR("decode_msg failed while reading mbcmedia detail from json buffer !");
    return false;
  }
  
  
  if(post(mediaDetail) != EN_XGATE_STATUS_SUCCESS) {
    return EN_XGATE_STATUS_ERROR;
  }
  return false;
}

bool MBCLinkMsgProcessor::create_doc_object(Document &doc, const char *jsonBuf)
{
  ParseResult ok = doc.Parse<kParseCommentsFlag>(jsonBuf);
  if(!ok) {
    XGLOG_ERROR("create_doc_object failed. due to json parse error: %s and jsondata:%s!", GetParseError_En(ok.Code()), jsonBuf);
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

bool MBCLinkMsgProcessor::read_media_detail(const Value& val, MgMediaDetail& detail)
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
    detail.call_type = val["call_type"].GetUint();
  } else {
    XGLOG_ERROR("read_media_detail failed. 'call_type' field is missing !");
    return false;
  }

  if(val.HasMember("context_id")) {
    detail.context_id = val["context_id"].GetInt();
  } else {
    XGLOG_ERROR("read_media_detail failed. 'context_id' field is missing !");
  }

  if(val.HasMember("gateway_id")) {
    detail.gateway_id = val["gateway_id"].GetUint();
  } else {
    XGLOG_ERROR("read_media_detail failed. 'gateway_id' field is missing !");
    return false;
  }

  if(val.HasMember("media_state")) {
    detail.media_event = val["media_state"].GetUint();
  } else {
    XGLOG_ERROR("read_media_detail failed. 'media_state' field is missing !");
  }

  if(val.HasMember("leg_id")) {
    detail.leg_id = val["leg_id"].GetUint();
  } else {
    XGLOG_ERROR("read_media_detail failed. 'leg_id' field is missing !");
  }

  if(val.HasMember("sig_type")) {
    detail.sig_type = val["sig_type"].GetUint();
  } else {
    XGLOG_ERROR("read_media_detail failed. 'sig_type' field is missing !");
  }

  if(val.HasMember("call_dir")) {
    detail.call_dir = val["call_dir"].GetUint();
  } else {
    XGLOG_ERROR("read_media_detail failed. 'call_dir' field is missing !");
  }

  if(val.HasMember("media_proto")) {
    detail.media_proto = val["media_proto"].GetString();
  } else {
    XGLOG_ERROR("read_media_detail failed. 'media_proto' field is missing !");
  }

  if(val.HasMember("out_proto")) {
    detail.out_proto = val["out_proto"].GetString();
  } else {
    XGLOG_ERROR("read_media_detail failed. 'out_proto' field is missing !");
  }

  if(val.HasMember("dtmf_digits")) {
    detail.dtmf_digits = val["dtmf_digits"].GetString();
  } else {
    XGLOG_ERROR("read_media_detail failed. 'out_proto' field is missing !");
  }

  if(val.HasMember("sdp_str")) {
    detail.sdp_str = val["sdp_str"].GetString();
  } else {
    XGLOG_ERROR("read_media_detail failed. 'sdp_str' field is missing !");
    return false;
  }

  if(val.HasMember("d_r_e")) {
    detail.dst_rtp_ep = val["d_r_e"].GetString();
  } else {
    XGLOG_ERROR("read_media_detail failed. 'Dst_rtp_endpoint' field is missing !");
  }

  if(val.HasMember("o_r_e")) {
    detail.org_rtp_ep = val["o_r_e"].GetString();
  } else {
    XGLOG_ERROR("read_media_detail failed. 'Org_rtp_endpoint' field is missing !");
  }

  if(val.HasMember("sdpinfo")) {
    const Value& valSdpInfo = val["sdpinfo"];
    read_sdpinfo(valSdpInfo, detail);
  } else {
    XGLOG_ERROR("read_media_detail failed. 'sdpinfo' field is missing !");
    return false;
  }
  return true;
}

bool MBCLinkMsgProcessor::read_sdpinfo(const Value& val, MgMediaDetail& detail)
{
  if(!val.IsArray() || val.Size() <= 0) {
    XGLOG_WARN("read_sdpinfo failed. It's not an array object or sdpinfo array is empty !");
    return false;
  }

  for(int i=0; i<val.Size(); i++) {
    const Value& valSdpInfo = val[i];   
    if(!valSdpInfo.IsObject()) {
      XGLOG_ERROR("fill_sdpinfo failed. not able to fetch valid sdpinfo object from an array list !");
      return false;   
    }

    if(valSdpInfo.HasMember("media_type")) {
      unsigned int sdpType = valSdpInfo["media_type"].GetUint();
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

bool MBCLinkMsgProcessor::read_audio_sdpinfo(const Value& val, MgMediaDetail& detail)
{
  AudioDetail &audioSdpInfo = detail.audioDetail; 
  if(val.HasMember("fmtp")) {
    audioSdpInfo.m_fmtp = val["fmtp"].GetString(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'fmtp' field is missing !");
  }

  if(val.HasMember("codec")) {
    audioSdpInfo.m_codec = val["codec"].GetUint(); 
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
    audioSdpInfo.m_ptime = val["ptime"].GetUint(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'ptime' field is missing !");
  }

  if(val.HasMember("play_file")) {
    audioSdpInfo.m_playFile = val["play_file"].GetString(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'play_file' field is missing !");
  }
  
  if(val.HasMember("ip_addr")) {
    audioSdpInfo.m_clientIp = val["ip_addr"].GetString(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'ip_addr' field is missing !");
  }

  if(val.HasMember("ip_port")) {
    audioSdpInfo.m_clientPort = val["ip_port"].GetUint(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'ip_port' field is missing !");
  }

  if(val.HasMember("relayip_addr")) {
    audioSdpInfo.m_relayIp = val["relayip_addr"].GetString(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'relayIp' field is missing !");
  }

  if(val.HasMember("relay_port")) {
    audioSdpInfo.m_relayPort = val["relay_port"].GetUint(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'relay_port' field is missing !");
  }

  if(val.HasMember("reflexip_addr")) {
    audioSdpInfo.m_rflxIp = val["reflexip_addr"].GetString(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'reflexip_addr' field is missing !");
  }

  if(val.HasMember("reflex_port")) {
    audioSdpInfo.m_rflxPort = val["reflex_port"].GetUint(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'reflex_port' field is missing !");
  }

  if(val.HasMember("ice_ufrag")) {
    audioSdpInfo.m_secureDetail.m_rIceUfrag = val["ice_ufrag"].GetString(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'ice_ufrag' field is missing !");
  }

  if(val.HasMember("ice_pwd")) {
    audioSdpInfo.m_secureDetail.m_rIcePwd = val["ice_pwd"].GetString(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'ice_pwd' field is missing !");
  }

  if(val.HasMember("fingerprint")) {
    audioSdpInfo.m_secureDetail.m_rFingerPrint = val["fingerprint"].GetString(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'fingerprint' field is missing !");
  }

  if(val.HasMember("ssrc")) {
    audioSdpInfo.m_secureDetail.m_rSsrc = val["ssrc"].GetString(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'ssrc' field is missing !");
  }

  if(val.HasMember("cname")) {
    audioSdpInfo.m_secureDetail.m_rCname = val["cname"].GetString(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'cname' field is missing !");
  }

  if(val.HasMember("mslable")) {
    audioSdpInfo.m_secureDetail.m_rMsLabel = val["mslable"].GetString(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'mslable' field is missing !");
  }

  if(val.HasMember("lable")) {
    audioSdpInfo.m_secureDetail.m_rLabel = val["lable"].GetString(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'lable' field is missing !");
  }

  if(val.HasMember("dial_ip")) {
    audioSdpInfo.m_dialIp = val["dial_ip"].GetString(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'dial_ip' field is missing !");
  }

  if(val.HasMember("dial_port")) {
    audioSdpInfo.m_dialPort = val["dial_port"].GetUint(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'dial_port' field is missing !");
  }

  if(val.HasMember("resp_ip")) {
    audioSdpInfo.m_respIp = val["resp_ip"].GetString(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'resp_ip' field is missing !");
  }

  if(val.HasMember("resp_port")) {
    audioSdpInfo.m_respPort = val["resp_port"].GetUint(); 
  } else {
    XGLOG_ERROR("read_audio_sdpinfo failed. 'resp_port' field is missing !");
  }
  return true;
}

bool MBCLinkMsgProcessor::read_video_sdpinfo(const Value& val, MgMediaDetail& detail)
{
  VideoDetail &videoSdpInfo = detail.videoDetail; 
  if(val.HasMember("fmtp")) {
    videoSdpInfo.m_fmtp = val["fmtp"].GetString(); 
  } else {
    XGLOG_ERROR("read_video_sdpinfo failed. 'fmtp' field is missing !");
  }

  if(val.HasMember("codec")) {
    videoSdpInfo.m_codec = val["codec"].GetUint(); 
  } else {
    XGLOG_ERROR("read_video_sdpinfo failed. 'codec' field is missing !");
    return false;
  }

  if(val.HasMember("codec_name")) {
    videoSdpInfo.m_codecName = val["codec_name"].GetString(); 
  } else {
    XGLOG_ERROR("read_video_sdpinfo failed. 'codec_name' field is missing !");
    return false;
  }

  if(val.HasMember("ptime")) {
    videoSdpInfo.m_ptime = val["ptime"].GetUint(); 
  } else {
    XGLOG_ERROR("read_video_sdpinfo failed. 'ptime' field is missing !");
  }

  if(val.HasMember("play_file")) {
    videoSdpInfo.m_playFile = val["play_file"].GetString(); 
  } else {
    XGLOG_ERROR("read_video_sdpinfo failed. 'play_file' field is missing !");
  }
  
  if(val.HasMember("ip_addr")) {
    videoSdpInfo.m_clientIp = val["ip_addr"].GetString(); 
  } else {
    XGLOG_ERROR("read_video_sdpinfo failed. 'ip_addr' field is missing !");
  }

  if(val.HasMember("ip_port")) {
    videoSdpInfo.m_clientPort = val["ip_port"].GetUint(); 
  } else {
    XGLOG_ERROR("read_video_sdpinfo failed. 'ip_port' field is missing !");
  }

  if(val.HasMember("relayip_addr")) {
    videoSdpInfo.m_relayIp = val["relayip_addr"].GetString(); 
  } else {
    XGLOG_ERROR("read_video_sdpinfo failed. 'relayIp' field is missing !");
  }

  if(val.HasMember("relay_port")) {
    videoSdpInfo.m_relayPort = val["relay_port"].GetUint(); 
  } else {
    XGLOG_ERROR("read_video_sdpinfo failed. 'relay_port' field is missing !");
  }

  if(val.HasMember("reflexip_addr")) {
    videoSdpInfo.m_rflxIp = val["reflexip_addr"].GetString(); 
  } else {
    XGLOG_ERROR("read_video_sdpinfo failed. 'reflexip_addr' field is missing !");
  }

  if(val.HasMember("reflex_port")) {
    videoSdpInfo.m_rflxPort = val["reflex_port"].GetUint(); 
  } else {
    XGLOG_ERROR("read_video_sdpinfo failed. 'reflex_port' field is missing !");
  }

  if(val.HasMember("ice_ufrag")) {
    videoSdpInfo.m_secureDetail.m_rIceUfrag = val["ice_ufrag"].GetString(); 
  } else {
    XGLOG_ERROR("read_video_sdpinfo failed. 'ice_ufrag' field is missing !");
  }

  if(val.HasMember("ice_pwd")) {
    videoSdpInfo.m_secureDetail.m_rIcePwd = val["ice_pwd"].GetString(); 
  } else {
    XGLOG_ERROR("read_video_sdpinfo failed. 'ice_pwd' field is missing !");
  }

  if(val.HasMember("fingerprint")) {
    videoSdpInfo.m_secureDetail.m_rFingerPrint = val["fingerprint"].GetString(); 
  } else {
    XGLOG_ERROR("read_video_sdpinfo failed. 'fingerprint' field is missing !");
  }

  if(val.HasMember("ssrc")) {
    videoSdpInfo.m_secureDetail.m_rSsrc = val["ssrc"].GetString(); 
  } else {
    XGLOG_ERROR("read_video_sdpinfo failed. 'ssrc' field is missing !");
  }

  if(val.HasMember("cname")) {
    videoSdpInfo.m_secureDetail.m_rCname = val["cname"].GetString(); 
  } else {
    XGLOG_ERROR("read_video_sdpinfo failed. 'cname' field is missing !");
  }

  if(val.HasMember("mslable")) {
    videoSdpInfo.m_secureDetail.m_rMsLabel = val["mslable"].GetString(); 
  } else {
    XGLOG_ERROR("read_video_sdpinfo failed. 'mslable' field is missing !");
  }

  if(val.HasMember("lable")) {
    videoSdpInfo.m_secureDetail.m_rLabel = val["lable"].GetString(); 
  } else {
    XGLOG_ERROR("read_video_sdpinfo failed. 'lable' field is missing !");
  }

  if(val.HasMember("dial_ip")) {
    videoSdpInfo.m_dialIp = val["dial_ip"].GetString(); 
  } else {
    XGLOG_ERROR("read_video_sdpinfo failed. 'dial_ip' field is missing !");
  }

  if(val.HasMember("dial_port")) {
    videoSdpInfo.m_dialPort = val["dial_port"].GetUint(); 
  } else {
    XGLOG_ERROR("read_video_sdpinfo failed. 'dial_port' field is missing !");
  }

  if(val.HasMember("resp_ip")) {
    videoSdpInfo.m_respIp = val["resp_ip"].GetString(); 
  } else {
    XGLOG_ERROR("read_video_sdpinfo failed. 'resp_ip' field is missing !");
  }

  if(val.HasMember("resp_port")) {
    videoSdpInfo.m_respPort = val["resp_port"].GetUint(); 
  } else {
    XGLOG_ERROR("read_video_sdpinfo failed. 'resp_port' field is missing !");
  }

  if(val.HasMember("framerate")) {
    videoSdpInfo.m_frameRate = val["framerate"].GetUint(); 
  } else {
    XGLOG_ERROR("read_video_sdpinfo failed. 'framerate' field is missing !");
  }

  if(val.HasMember("imageattr_x")) {
    videoSdpInfo.m_imageattr_x = val["imageattr_x"].GetUint(); 
  } else {
    XGLOG_ERROR("read_video_sdpinfo failed. 'imageattr_x' field is missing !");
  }
  
	if(val.HasMember("imageattr_y")) {
    videoSdpInfo.m_imageattr_y = val["imageattr_y"].GetUint(); 
  } else {
    XGLOG_ERROR("read_video_sdpinfo failed. 'imageattr_y' field is missing !");
  }
  return true;
}

bool MBCLinkMsgProcessor::post(MgMediaDetail& mediaDetail)
{
	string uid = mediaDetail.mgresource_id;
	XGLOG_DEBUG("posting mbcmedia service msg for uid: %s!", uid.c_str());
	//find out to which service to post this message
	xGateMBCLinkServiceMsg *pMsg = new xGateMBCLinkServiceMsg();
	pMsg->setUid(uid);

	switch(mediaDetail.call_type) {
		case EN_XGATE_CALL_TYPE_CONFERENCE:
			{
				XGLOG_DEBUG("processing mbcmedia response for Audio conference Need to Implement");
			}
		case EN_XGATE_CALL_TYPE_CONFERENCE_VIDEO:
			{
				XGLOG_DEBUG("processing mbcmedia response for Video conference: %s", uid.c_str());
				switch(mediaDetail.mgmsg_type) {
					case EN_XGATE_MG_RELEASE:
					case EN_XGATE_MG_MEDIA_CREATE:
						{
							pMsg->setOpCode(EN_MBC_LINK_EVENT_CONF_PORT_ALLOCATION_RES);
							pMsg->setDstModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
							pMsg->setMsgType(EN_XGATE_MSG_MGC_MG_OUT);
							break;
						}
					case EN_XGATE_MG_ADD_PARTICIPANT:
						{
							pMsg->setOpCode(EN_MBC_LINK_EVENT_CONF_ADD_PARTICIPANT_REQ);
							pMsg->setDstModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
							pMsg->setMsgType(EN_XGATE_MSG_MGC_MG_OUT);
							break;
						}
                                        case EN_XGATE_MG_DTLS_RECONNECT:
                                                {
                                                        pMsg->setOpCode(EN_MBC_LINK_EVENT_DTLS_RECONNECT_RES);
                                                        pMsg->setDstModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
                                                        pMsg->setMsgType(EN_XGATE_MSG_MGC_MG_OUT);
                                                        break;
                                                }
                                        case EN_XGATE_MG_SDP_UPDATE_RESPONSE:
                                                {
                                                        pMsg->setOpCode(EN_MBC_LINK_EVENT_CONF_USER_SDP_UPDATE_RES);
                                                        pMsg->setDstModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
                                                        pMsg->setMsgType(EN_XGATE_MSG_MGC_MG_OUT);
                                                        break;
                                                }
                                        case EN_XGATE_MG_SDP_UPDATE_REQUEST:
                                                {
                                                        pMsg->setOpCode(EN_MBC_LINK_EVENT_CONF_USER_MEDIA_UPDATE_REQ);
                                                        pMsg->setDstModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
                                                        pMsg->setMsgType(EN_XGATE_MSG_MGC_MG_OUT);
                                                        break;
                                                }
                                        case EN_XGATE_MG_SFU_CONN_CLOSED:
                                                {
                                                        pMsg->setOpCode(EN_MBC_LINK_EVENT_SFU_CONN_CLOSED);
                                                        pMsg->setDstModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
                                                        pMsg->setMsgType(EN_XGATE_MSG_MGC_MG_OUT);
                                                        break;
                                                }
                                        case EN_XGATE_MG_DTLS_CONN_FAILED:
                                                {
                                                        pMsg->setOpCode(EN_MBC_LINK_EVENT_DTLS_CONN_FAILED);
                                                        pMsg->setDstModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
                                                        pMsg->setMsgType(EN_XGATE_MSG_MGC_MG_OUT);
                                                        break;
                                                }
					case EN_XGATE_MG_MEDIA_DELETE:
						{
							break;
						}
					default:
						{
							XGLOG_ERROR("got unknown message type in mbcmedia response for uid: %s", uid.c_str());
							delete pMsg;
							return false;
						}
				}
				break;
			}
		default:
			{
				XGLOG_ERROR("got mbcmedia response for unknown call type for uid: %s", uid.c_str());
				delete pMsg;
				return false;
			}
	}

	pMsg->set_media_detail(mediaDetail);
	MBCLINKSERVICE->getConfig()->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(pMsg));
}
