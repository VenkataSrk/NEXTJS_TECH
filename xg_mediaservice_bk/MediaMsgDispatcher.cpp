#include "xglog.h"
#include "MediaMsgDispatcher.h"
#include "xGateMultiSessionTracker.h"
#include <map>

MediaMsgDispatcher::MediaMsgDispatcher() :
  m_uid(""), m_fd(0)
{
}

MediaMsgDispatcher::~MediaMsgDispatcher(void)
{
}

bool MediaMsgDispatcher::init()
{
  int len = 0;
  len = sizeof(json_mg_media_tmpl);
  char buff[len];
  memcpy(buff, json_mg_media_tmpl, len);

  if(!XGUTIL->parseJson(buff, m_doc)) {
    XGLOG_ERROR("parsing media gateway json template failed !"); 
    return false;
  }

  m_strBuf.Clear();
  return true;
}

void MediaMsgDispatcher::reset()
{
  m_uid.clear();
  m_strBuf.Clear();
  m_fd = 0; 
}

bool MediaMsgDispatcher::process_msg(xGateMediaServiceMsg *pMsg)
{
  if(!pMsg){
     XGLOG_FATAL("MediaMsgDispatcher::process_msg failed to get xGateMediaServiceMsg for %s!",m_uid.c_str());
     return false;
  }

  reset();
  m_uid = pMsg->getUid();
  
  if(m_uid.empty()) {
    XGLOG_ERROR("process_msg failed 'uid' is empty !");
    return false;
  }
  
  //encode json message from given media details
  if(!encode_msg(pMsg)) {
    XGLOG_ERROR("process_msg failed while encoding json message for uid: %s !", m_uid.c_str()); 
    return false;
  }

  //fetch media gateway connection detail
  if(!fetch_fd(pMsg)) {
    XGLOG_ERROR("process_msg failed while fetching media-gateway connection detail for uid: %s !", m_uid.c_str());
    return false;
  }

  if(!send_msg_on_tcp()) {
    XGLOG_ERROR("send_msg_on_tcp failed after fetching media-gateway connection detail for uid: %s !", m_uid.c_str());
    return false;
  }else{
    string uid = pMsg->getUid();
    IURDefines::MODULE_ID srcModuleId = pMsg->getSrcModuleId();
    SessionDetail *details = new SessionDetail(uid, srcModuleId, NULL);
    //SESSIONTRACKER->keepSession(details);
    delete details;
  }

 return true;
  //send json message to media gateway
//  return send_msg_on_tcp();
}

bool MediaMsgDispatcher::encode_msg(xGateMediaServiceMsg *pMsg)
{
  if(!pMsg){
    XGLOG_FATAL("MediaMsgDispatcher::encode_msg failed to get xGateMediaServiceMsg!");
    return false;
  }
  Writer<StringBuffer> writer(m_strBuf);

  MgMediaDetail &mediaDetail = pMsg->get_media_detail(); 
  
	m_strMsgTypeName  = MgMediaDetail::getMediaMsgType(mediaDetail.mgmsg_type);
	
  
  if(m_doc.HasMember("msg_type")) {
    m_doc["msg_type"] = XGUTIL->setJsonParam(mediaDetail.mgmsg_type);
  } 

  if(!m_doc.HasMember("data")) {
    XGLOG_ERROR("encode_msg failed. 'data' segment is missing in media gateway json template !"); 
    return false;
  }

  Value &valData = m_doc["data"];
  if(!fill_media_detail(valData, mediaDetail)) {
    XGLOG_ERROR("encode_msg failed while filling media detail as a json buffer !");
    return false;
  }

  m_doc.Accept(writer); 
  m_doc["msg_len"] = m_strBuf.GetLength();

  m_strBuf.Clear();
  Writer<StringBuffer> writer2(m_strBuf);
  m_doc.Accept(writer2);
  m_doc["msg_len"] = m_strBuf.GetLength();

  m_strBuf.Clear();
  Writer<StringBuffer> writer3(m_strBuf);
  m_doc.Accept(writer3);
  return true;
}

bool MediaMsgDispatcher::fill_media_detail(Value& val, MgMediaDetail& detail)
{
  val["mgresource_id"] = XGUTIL->setJsonParam(detail.mgresource_id, m_doc);
  val["call_type"] = XGUTIL->setJsonParam(detail.call_type);
  val["call_id"] = XGUTIL->setJsonParam(detail.call_id, m_doc);
  val["gateway_id"] = XGUTIL->setJsonParam(detail.gateway_id);
  val["context_id"] = XGUTIL->setJsonParam(detail.context_id);
  val["leg_id"] = XGUTIL->setJsonParam(detail.leg_id);
  val["media_state"] = XGUTIL->setJsonParam(detail.media_event); 
  val["media_proto"] = XGUTIL->setJsonParam(detail.media_proto, m_doc);
  val["out_proto"] = XGUTIL->setJsonParam(detail.out_proto, m_doc);
  val["record_file"] = XGUTIL->setJsonParam(detail.record_file, m_doc);
  val["file_size"] = XGUTIL->setJsonParam(detail.file_size);

  if(val.HasMember("sdpinfo")) {
    Value& valSdpInfo = val["sdpinfo"];
    if(!fill_sdpinfo(valSdpInfo, detail)) {
      XGLOG_ERROR("fill_media_detail failed while filling sdpinfo !");
      return false;
    }
  } else {
    XGLOG_ERROR("file_media_detail failed. 'sdpinfo' field is missing !");
    return false;
  }
  return true;
}

bool MediaMsgDispatcher::fill_sdpinfo(Value& val, MgMediaDetail& detail)
{
  if(!val.IsArray() || val.Size() <= 0) {
    XGLOG_ERROR("fill_sdpinfo failed. It's not an array object !");
    return false;
  }

  for(int i=0; i<val.Size(); i++) {
    Value& valSdpInfo = val[i];   
    if(!valSdpInfo.IsObject()) {
      XGLOG_ERROR("fill_sdpinfo failed. not able to fetch valid sdpinfo object from an array list !");
      return false;   
    }
    if(!fill_audio_sdpinfo(valSdpInfo, detail)) {
      XGLOG_ERROR("fill_audio_sdpinfo failed while filling audio sdp info !");
      return false;
    }
  }
  return true;
}

bool MediaMsgDispatcher::fill_audio_sdpinfo(Value& val, MgMediaDetail& detail)
{
  AudioDetail &audioDetail = detail.audioDetail; 
  val["media_type"] = XGUTIL->setJsonParam(audioDetail.m_mediaType);
  val["fmtp"] = XGUTIL->setJsonParam(audioDetail.m_fmtp, m_doc);
  val["codec"] = XGUTIL->setJsonParam(audioDetail.m_codec);
  val["codec_name"] = XGUTIL->setJsonParam(audioDetail.m_codecName, m_doc);
  val["ptime"] = XGUTIL->setJsonParam(audioDetail.m_ptime);
  val["play_file"] = XGUTIL->setJsonParam(audioDetail.m_playFile, m_doc);
  val["ip_addr"] = XGUTIL->setJsonParam(audioDetail.m_clientIp, m_doc);
  val["ip_port"] = XGUTIL->setJsonParam(audioDetail.m_clientPort);
  val["relayip_addr"] = XGUTIL->setJsonParam(audioDetail.m_relayIp, m_doc);
  val["relay_port"] = XGUTIL->setJsonParam(audioDetail.m_relayPort);
  val["reflexip_addr"] = XGUTIL->setJsonParam(audioDetail.m_rflxIp, m_doc);
  val["reflex_port"] = XGUTIL->setJsonParam(audioDetail.m_rflxPort);
  val["ice_ufrag"] = XGUTIL->setJsonParam(audioDetail.m_secureDetail.m_rIceUfrag, m_doc);
  val["ice_pwd"] = XGUTIL->setJsonParam(audioDetail.m_secureDetail.m_rIcePwd, m_doc);
  val["fingerprint"] = XGUTIL->setJsonParam(audioDetail.m_secureDetail.m_rFingerPrint, m_doc);
  val["ssrc"] = XGUTIL->setJsonParam(audioDetail.m_secureDetail.m_rSsrc, m_doc);
  val["cname"] = XGUTIL->setJsonParam(audioDetail.m_secureDetail.m_rCname, m_doc);
  val["mslable"] = XGUTIL->setJsonParam(audioDetail.m_secureDetail.m_rMsLabel, m_doc);
  val["lable"] = XGUTIL->setJsonParam(audioDetail.m_secureDetail.m_rLabel, m_doc);
  val["dial_ip"] = XGUTIL->setJsonParam(audioDetail.m_dialIp, m_doc);
  val["dial_port"] = XGUTIL->setJsonParam(audioDetail.m_dialPort);
  val["resp_ip"] = XGUTIL->setJsonParam(audioDetail.m_respIp, m_doc);
  val["resp_port"] = XGUTIL->setJsonParam(audioDetail.m_respPort);
  return true;  
}

bool MediaMsgDispatcher::fetch_fd(xGateMediaServiceMsg *pMsg)
{
  if(!pMsg){
    XGLOG_ERROR("MediaMsgDispatcher::fetch_fd failed to get xGateMediaServiceMsg!");
    return false;
  }
  MgMediaDetail &mediaDetail = pMsg->get_media_detail(); 
  unsigned short gatewayId = mediaDetail.gateway_id;
  return MEDIAUTIL->getFromMgConnectionMap(gatewayId, m_fd);
}

bool MediaMsgDispatcher::send_msg_on_tcp()
{
  //check the data is not empty
  unsigned int len = m_strBuf.GetLength();
  string data = m_strBuf.GetString();
  len = data.length();
  if(data.empty()) {
    XGLOG_ERROR("send message on fd: %d failed for uid: %s. 'data' is empty !", \
        m_fd, m_uid.c_str());
    return false;
  }

  XGLOG_DEBUG("call id (%s). Trying to send media msgtype [%s]. with size of %d bytes of data: <%s>",m_uid.c_str(), m_strMsgTypeName.c_str(), len, data.c_str());

  //check the fd is some what valid
  if(m_fd <= 0) {
    XGLOG_ERROR("send message to media-gateway failed. invalid fd: %d fetched for uid: %s !", \
        m_fd, m_uid.c_str());
    return false;
  }

  //append '\r\n\r\n' at end of the each json message
  string delimiter("\r\n\r\n");
  data.append(delimiter.c_str(), delimiter.length());
  len = data.length();

  errno = 0;
  int retVal = send(m_fd, data.c_str(), len, MSG_DONTWAIT);
  if(retVal == -1) {
    XGLOG_ERROR("send messsage to media-gatway on fd: %d failed for uid: %s with error: %s", \
        m_fd, m_uid.c_str(), strerror(errno));
    return false;
  }

  XGLOG_DEBUG("successfully sent %d bytes of data on fd: %d", len, m_fd);
  return true;
}

