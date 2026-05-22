#include "xglog.h"
#include "MediaMsgDispatcher.h"
#include "xGateMultiSessionTracker.h"
#include <map>

MediaMsgDispatcher::MediaMsgDispatcher() :
  ACE_Task<ACE_MT_SYNCH>(),
  m_run(false)
{
}

MediaMsgDispatcher::~MediaMsgDispatcher(void)
{
}

bool MediaMsgDispatcher::stop(void)
{
  m_run = false;
  msg_queue()->deactivate();
  wait();
  XGLOG_INFO("MediaMsgDispatcher stopped !");
  return true;
}

bool MediaMsgDispatcher::init()
{
  m_run = true;
  if(activate(THR_NEW_LWP, 1) == -1) {
    XGLOG_ERROR("MediaMsgDispatcher::init() failed to start VMS controller thread !");
    return false;
  }
  XGLOG_INFO("Initialized MediaMsgDispatcher successfully !");
  return true;
}

int MediaMsgDispatcher::svc(void)
{
  ACE_Message_Block *pAmb = 0;

  while(m_run) {
    int result = getq(pAmb, NULL);
    if((result == -1) && (errno == EWOULDBLOCK)) {
      continue;
    }

    if((result != -1)) {
      process_msg(pAmb);
    } else {
      XGLOG_ERROR("MediaMsgDispatcher::svc Error in Message Queue");
      m_run = false;
    }

    if(pAmb) {
      delete pAmb;
      pAmb = NULL;
    }
  }
  XGLOG_INFO("MediaMsgDispatcher::svc() stopped");
  return 0;
}

bool MediaMsgDispatcher::parseJson(const char *jsonData, Document &doc)
{
  ParseResult ok = doc.Parse(jsonData);
  if(!ok) {
    XGLOG_ERROR("parseJson failed due to parse error: %s !", GetParseError_En(ok.Code()));
    return false;
  } else if(!doc.IsObject()) {
    XGLOG_ERROR("parseJson failed while creating json documnet object !");
    return false;
  }
  return true;
}

Value MediaMsgDispatcher::setJsonParam(const string &strValue, Document &doc)
{
  Value retVal;
  retVal.SetString(strValue.c_str(), strValue.length(), doc.GetAllocator());
  return retVal;
}

Value MediaMsgDispatcher::setJsonParam(unsigned int iValue)
{
  Value retVal;
  retVal = iValue;
  return retVal;
}
#if 0
bool MediaMsgDispatcher::init()
{
  int len = 0;
  len = sizeof(json_mg_media_tmpl);
  char buff[len];
  memcpy(buff, json_mg_media_tmpl, len);

  if(parseJson(buff, m_doc)) {
    XGLOG_ERROR("parsing media gateway json template failed !"); 
    return false;
  }

  m_strBuf.Clear();
  return true;
}
#endif
void MediaMsgDispatcher::reset()
{
  m_uid.clear();
  m_strBuf.Clear();
  m_fd = 0; 
}

bool MediaMsgDispatcher::process_msg(ACE_Message_Block *pAmb)
{
  xGateMediaServiceMsg *pMsg = dynamic_cast<xGateMediaServiceMsg*>(pAmb);
  bool return_val = true;
  char *data = NULL;
  if(!fetch_fd(pMsg)) {
    XGLOG_ERROR("process_msg failed while fetching media-gateway connection detail for uid: %s !", m_uid.c_str());
    return_val = false;
  }

  if(!send_msg_on_tcp(pMsg)) {
    XGLOG_ERROR("send_msg_on_tcp failed after fetching media-gateway connection detail for uid: %s !", m_uid.c_str());
    return_val = false;
  }
  data = static_cast<char *>(pMsg->get_msg ());
  XGLOG_TRACE ("MediaMsgDispatcher::process_msg deleting data ptr: %p len: %d pMsg: %p pAmb: %p",
                       data, pMsg->getMsgLen (), pMsg, pAmb);
  if (data){
    delete[] data;
    pMsg->set_msg (NULL);
    pMsg->setMsgLen (0);
  }
  return return_val;
}

bool MediaMsgDispatcher::encode_msg(xGateMediaServiceMsg *pMsg)
{
  Writer<StringBuffer> writer(m_strBuf);

  MgMediaDetail &mediaDetail = pMsg->get_media_detail(); 
  
	m_strMsgTypeName  = MgMediaDetail::getMediaMsgType(mediaDetail.mgmsg_type);
	
  
  if(m_doc.HasMember("msg_type")) {
    m_doc["msg_type"] = setJsonParam(mediaDetail.mgmsg_type);
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
  val["mgresource_id"] = setJsonParam(detail.mgresource_id, m_doc);
  val["call_type"] = setJsonParam(detail.call_type);
  val["call_id"] = setJsonParam(detail.call_id, m_doc);
  val["gateway_id"] = setJsonParam(detail.gateway_id);
  val["context_id"] = setJsonParam(detail.context_id);
  val["leg_id"] = setJsonParam(detail.leg_id);
  val["media_state"] = setJsonParam(detail.media_event); 
  val["media_proto"] = setJsonParam(detail.media_proto, m_doc);
  val["out_proto"] = setJsonParam(detail.out_proto, m_doc);
  val["record_file"] = setJsonParam(detail.record_file, m_doc);
  val["record_url"] = setJsonParam(detail.record_url, m_doc);
  val["meeting_name"] = setJsonParam(detail.meeting_name, m_doc);
  val["sip_id"] = setJsonParam(detail.sip_id, m_doc);
  val["is_ccaas"] = setJsonParam(detail.is_ccaas);
  val["customer_num"] = setJsonParam(detail.customer_num, m_doc);
  val["domain_id"] = setJsonParam(detail.domain_id);
  val["agent_id"] = setJsonParam(detail.agent_id);
  val["file_size"] = setJsonParam(detail.file_size);

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
  val["media_type"] = setJsonParam(audioDetail.m_mediaType);
  val["fmtp"] = setJsonParam(audioDetail.m_fmtp, m_doc);
  val["codec"] = setJsonParam(audioDetail.m_codec);
  val["codec_name"] = setJsonParam(audioDetail.m_codecName, m_doc);
  val["ptime"] = setJsonParam(audioDetail.m_ptime);
  val["play_file"] = setJsonParam(audioDetail.m_playFile, m_doc);
  val["ip_addr"] = setJsonParam(audioDetail.m_clientIp, m_doc);
  val["ip_port"] = setJsonParam(audioDetail.m_clientPort);
  val["relayip_addr"] = setJsonParam(audioDetail.m_relayIp, m_doc);
  val["relay_port"] = setJsonParam(audioDetail.m_relayPort);
  val["reflexip_addr"] = setJsonParam(audioDetail.m_rflxIp, m_doc);
  val["reflex_port"] = setJsonParam(audioDetail.m_rflxPort);
  val["ice_ufrag"] = setJsonParam(audioDetail.m_secureDetail.m_rIceUfrag, m_doc);
  val["ice_pwd"] = setJsonParam(audioDetail.m_secureDetail.m_rIcePwd, m_doc);
  val["fingerprint"] = setJsonParam(audioDetail.m_secureDetail.m_rFingerPrint, m_doc);
  val["ssrc"] = setJsonParam(audioDetail.m_secureDetail.m_rSsrc, m_doc);
  val["cname"] = setJsonParam(audioDetail.m_secureDetail.m_rCname, m_doc);
  val["mslable"] = setJsonParam(audioDetail.m_secureDetail.m_rMsLabel, m_doc);
  val["lable"] = setJsonParam(audioDetail.m_secureDetail.m_rLabel, m_doc);
  val["dial_ip"] = setJsonParam(audioDetail.m_dialIp, m_doc);
  val["dial_port"] = setJsonParam(audioDetail.m_dialPort);
  val["resp_ip"] = setJsonParam(audioDetail.m_respIp, m_doc);
  val["resp_port"] = setJsonParam(audioDetail.m_respPort);
  return true;  
}

bool MediaMsgDispatcher::fetch_fd(xGateMediaServiceMsg *pMsg)
{
  unsigned int connectionId = pMsg->get_connectionId();
  TcpConTuple connTuple;
  int retVal;
  if(!MEDIAUTIL->getConnTupleInfo(connectionId,connTuple))
  {	  
    XGLOG_ERROR("Could not find connectionInfo for connectionId:%d",connectionId);
    return false;
  } 
  m_fd = connTuple.m_fd;  
 return true; 
}

bool MediaMsgDispatcher::send_msg_on_tcp(xGateMediaServiceMsg *pMsg)
{
  //check the data is not empty
  int len = pMsg->getMsgLen();
  char *data = (char*) pMsg->get_msg();
  int bytes_sent = 0;
  int bytes_left = len;
  int retval = 0;
  int retry_count = 0;
  //check the fd is some what valid
  if(m_fd <= 0) {
     XGLOG_ERROR("send message to media-gateway failed. invalid fd: %d fetched for uid: %s !", \
	  m_fd, m_uid.c_str());
     return false;
  }
  //append '\r\n\r\n' at end of the each json message
#if 0
  string delimiter("\r\n\r\n");
  data.append(delimiter.c_str(), delimiter.length());
  len = data.length();
#endif
  errno = 0;
  while (bytes_sent < len)
  {
     /*use blocking call instead of returning immediately */
     retval = send(m_fd, data + bytes_sent ,bytes_left, 0);
     if (retval < 0){
        XGLOG_ERROR ("Failed to send data over socket error: %d bytes sent till failure: %d error: %s",
			retval, bytes_sent, strerror(errno));
	retry_count++;
        usleep (20000);
	continue;
      }
      bytes_sent += retval;
      bytes_left -= bytes_sent;
  }

  if(bytes_sent == len) {
    XGLOG_INFO("successfully sent FROM MG %d bytes on fd: %hu data: (%s) on ", len, m_fd, data);
    //XGLOG_TRACE("successfully sent FROM MG %d bytes on fd: %hu", len, m_fd);
  }else{
     XGLOG_ERROR("send messsage to media-gatway on fd: %d failed for uid: %s with error: %s", \
			  m_fd, m_uid.c_str(), strerror(errno));
     return false;
  }
  return true;
}
