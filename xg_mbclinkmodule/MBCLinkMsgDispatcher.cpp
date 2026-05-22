#include<iostream>
#include "xglog.h"
#include "MBCLinkMsgDispatcher.h"
#include <map>

MBCLinkDispatcher::MBCLinkDispatcher() :
  m_uid(""), m_fd(0)
{
}

MBCLinkDispatcher::~MBCLinkDispatcher(void)
{
}

bool MBCLinkDispatcher::init()
{
  int len = 0;
  len = sizeof(json_mbclink_tmpl);
  char buff[len];
  memcpy(buff, json_mbclink_tmpl, len);

  if(!XGUTIL->parseJson(buff, m_doc)) {
    XGLOG_ERROR("parsing mbc link json template failed !"); 
    return false;
  }

  m_strBuf.Clear();
  return true;
}

void MBCLinkDispatcher::reset()
{
  m_uid.clear();
  m_strBuf.Clear();
  m_fd = 0; 
}

bool MBCLinkDispatcher::process_msg(xGateMBCLinkServiceMsg *pMsg)
{
  if(!pMsg){
     XGLOG_FATAL("MBCLinkDispatcher::process_msg failed to get xGateMBCLinkServiceMsg for %s!",m_uid.c_str());
     return false;
  }

  reset();
  m_uid = pMsg->getUid();
  
  if(m_uid.empty()) {
    XGLOG_ERROR("process_msg failed 'uid' is empty !");
    return false;
  }
  
  //encode json message from given mbcmedia details
  if(!encode_msg(pMsg)) {
    XGLOG_ERROR("process_msg failed while encoding json message for uid: %s !", m_uid.c_str()); 
    return false;
  }

  //fetch mbclink gateway connection detail
  if(!fetch_fd(pMsg)) {
    XGLOG_ERROR("process_msg failed while fetching media-gateway connection detail for uid: %s !", m_uid.c_str());
    return false;
  }

  if(!send_msg_on_tcp()) {
    XGLOG_ERROR("send_msg_on_tcp failed after fetching media-gateway connection detail for uid: %s !", m_uid.c_str());
    return false;
  }
  
 return true;
}

bool MBCLinkDispatcher::encode_msg(xGateMBCLinkServiceMsg *pMsg)
{
  if(!pMsg){
    XGLOG_FATAL("MBCLinkDispatcher::encode_msg failed to get xGateMBCLinkServiceMsg!");
    return false;
  }
  Writer<StringBuffer> writer(m_strBuf);

  MgMediaDetail &mediaDetail = pMsg->get_media_detail(); 
  
	m_strMsgTypeName  = MgMediaDetail::getMediaMsgType(mediaDetail.mgmsg_type);
	
  
  if(m_doc.HasMember("msg_type")) {
    m_doc["msg_type"] = XGUTIL->setJsonParam(mediaDetail.mgmsg_type);
  } 

  if(!m_doc.HasMember("data")) {
    XGLOG_ERROR("encode_msg failed. 'data' segment is missing in mbclink gateway json template !"); 
    return false;
  }

  Value &valData = m_doc["data"];
  if(!fill_media_detail(valData, mediaDetail)) {
    XGLOG_ERROR("encode_msg failed while filling mbcmedia detail as a json buffer !");
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

bool MBCLinkDispatcher::fill_media_detail(Value& val, MgMediaDetail& detail)
{
  val["mgresource_id"] = XGUTIL->setJsonParam(detail.mgresource_id, m_doc);
  val["call_type"] = XGUTIL->setJsonParam(detail.call_type);
  val["call_id"] = XGUTIL->setJsonParam(detail.call_id, m_doc);
  val["conn_id"] = XGUTIL->setJsonParam(detail.conn_id, m_doc);
  val["device_id"] = XGUTIL->setJsonParam(detail.device_id, m_doc);
  val["meeting_name"] = XGUTIL->setJsonParam(detail.meeting_name, m_doc);
  val["sip_id"] = XGUTIL->setJsonParam(detail.sip_id, m_doc);
  val["domain_id"] = XGUTIL->setJsonParam(detail.domain_id);
  val["agent_id"] = XGUTIL->setJsonParam(detail.agent_id);
  val["is_ccaas"] = XGUTIL->setJsonParam(detail.is_ccaas);
  val["gateway_id"] = XGUTIL->setJsonParam(detail.gateway_id);
  val["context_id"] = XGUTIL->setJsonParam(detail.context_id);
  val["leg_id"] = XGUTIL->setJsonParam(detail.leg_id);
  val["media_state"] = XGUTIL->setJsonParam(detail.media_event); 
  val["media_proto"] = XGUTIL->setJsonParam(detail.media_proto, m_doc);
  val["out_proto"] = XGUTIL->setJsonParam(detail.out_proto, m_doc);
  val["browser_type"] = XGUTIL->setJsonParam(detail.browser_type, m_doc);
  val["record_file"] = XGUTIL->setJsonParam(detail.record_file, m_doc);
  val["record_url"] = XGUTIL->setJsonParam(detail.record_url, m_doc);
  val["record_id"] = XGUTIL->setJsonParam(detail.record_id, m_doc);
  val["customer_num"] = XGUTIL->setJsonParam(detail.customer_num, m_doc);
  val["file_size"] = XGUTIL->setJsonParam(detail.file_size);
  val["joiner_type"] = XGUTIL->setJsonParam(detail.joiner_type);
  val["plan_type"] = XGUTIL->setJsonParam(detail.plan_type);
  val["asr_type"] = XGUTIL->setJsonParam(detail.plan_type);
  val["screenshare_id"] = XGUTIL->setJsonParam(detail.screenshare_id, m_doc);
  val["sig_type"] = XGUTIL->setJsonParam(detail.sig_type);
  val["call_dir"] = XGUTIL->setJsonParam(detail.call_dir);
  val["sdp_str"] = XGUTIL->setJsonParam(detail.sdp_str, m_doc);
  val["sdpinfo"] = setSdpInfoParam(detail, m_doc);
  val["participant_list"] = setParticipantInfoParam(detail, m_doc);
  val["urmeet_user_list"] = setURMeetUserListParam(detail, m_doc);
  return true;
}

Value MBCLinkDispatcher::setParticipantInfoParam(MgMediaDetail& detail,Document &m_doc)
{
  Value retVal;
  Document::AllocatorType& allocate =  m_doc.GetAllocator();
  Value vArray(kArrayType);

  if(!vArray.IsArray()) {
    XGLOG_ERROR("setParticipantListInfoParam failed. It's not an array object !");
    return retVal;
  }
  if(detail.participantList.size()>0) {
    std::list<ParticipantDetail>::iterator itr = detail.participantList.begin();
    for(itr; itr != detail.participantList.end(); itr++) {
      Value objectValue;
      objectValue.SetObject();
      objectValue.AddMember("call_id",XGUTIL->setJsonParam(itr->m_callId,m_doc),allocate);
      objectValue.AddMember("joiner_type",XGUTIL->setJsonParam(itr->m_joinerType),allocate);
      vArray.PushBack(objectValue,allocate);
    }
  }
  return vArray;
}

Value MBCLinkDispatcher::setURMeetUserListParam(MgMediaDetail& detail,Document &m_doc)
{
  Value retVal;
  Document::AllocatorType& allocate =  m_doc.GetAllocator();
  Value vArray(kArrayType);

  if(!vArray.IsArray()) {
    XGLOG_ERROR("setURMeetUserListParam failed. It's not an array object !");
    return retVal;
  }
  if(detail.meetUserList.size()>0) {
    std::list<URMeetUserDetail>::iterator itr = detail.meetUserList.begin();
    for(itr; itr != detail.meetUserList.end(); itr++) {
      Value objectValue;
      objectValue.SetObject();
      objectValue.AddMember("email",XGUTIL->setJsonParam(itr->m_strEmail,m_doc),allocate);
      objectValue.AddMember("profile_img",XGUTIL->setJsonParam(itr->m_strProfileImg,m_doc),allocate);
      objectValue.AddMember("sip_id",XGUTIL->setJsonParam(itr->m_strSipId,m_doc),allocate);
      objectValue.AddMember("name",XGUTIL->setJsonParam(itr->m_strName,m_doc),allocate);
      objectValue.AddMember("ext",XGUTIL->setJsonParam(itr->m_strExtn,m_doc),allocate);
      objectValue.AddMember("user_type",XGUTIL->setJsonParam(itr->m_strUserType,m_doc),allocate);
      objectValue.AddMember("role_type",XGUTIL->setJsonParam(itr->m_iRoleType),allocate);
      vArray.PushBack(objectValue,allocate);
    }
  }
  return vArray;
}

Value MBCLinkDispatcher::setSdpInfoParam(MgMediaDetail& detail,Document &m_doc)
{
	Value retVal;
	Document::AllocatorType& allocate =  m_doc.GetAllocator();
  Value vArray(kArrayType);
  if(!vArray.IsArray()) {
    XGLOG_ERROR("fill_sdpinfo failed. It's not an array object !");
    return retVal;
  }

  for(int i=0; i<2; i++) {
		Value valSdpInfo;   
		if(i == 0){
      valSdpInfo = fill_audio_sdpinfo(detail, m_doc,allocate);
		  vArray.PushBack(valSdpInfo,allocate);
		}
		if (i == 1){
      valSdpInfo = fill_video_sdpinfo(detail, m_doc,allocate);
		  vArray.PushBack(valSdpInfo,allocate);
		}
  }
  return vArray;
}

Value MBCLinkDispatcher::fill_audio_sdpinfo(MgMediaDetail& detail, Document &doc,Document::AllocatorType& allocate)
{
	Value val;
	val.SetObject();
  AudioDetail &audioDetail = detail.audioDetail; 
  val.AddMember("media_type",XGUTIL->setJsonParam(audioDetail.m_mediaType),allocate);
	val.AddMember("fmtp",XGUTIL->setJsonParam(audioDetail.m_fmtp, m_doc),allocate);
	val.AddMember("codec",XGUTIL->setJsonParam(audioDetail.m_codec),allocate);
	val.AddMember("codec_name",XGUTIL->setJsonParam(audioDetail.m_codecName, m_doc),allocate);
	val.AddMember("ptime",XGUTIL->setJsonParam(audioDetail.m_ptime),allocate);
	val.AddMember("play_file",XGUTIL->setJsonParam(audioDetail.m_playFile, m_doc),allocate);
	val.AddMember("ip_addr",XGUTIL->setJsonParam(audioDetail.m_clientIp, m_doc),allocate);
	val.AddMember("ip_port",XGUTIL->setJsonParam(audioDetail.m_clientPort),allocate);
	val.AddMember("relayip_addr",XGUTIL->setJsonParam(audioDetail.m_relayIp, m_doc),allocate);
	val.AddMember("relay_port",XGUTIL->setJsonParam(audioDetail.m_relayPort),allocate);
	val.AddMember("reflexip_addr",XGUTIL->setJsonParam(audioDetail.m_rflxIp, m_doc),allocate);
	val.AddMember("reflex_port",XGUTIL->setJsonParam(audioDetail.m_rflxPort),allocate);
	val.AddMember("ice_ufrag",XGUTIL->setJsonParam(audioDetail.m_secureDetail.m_rIceUfrag, m_doc),allocate);
	val.AddMember("ice_pwd",XGUTIL->setJsonParam(audioDetail.m_secureDetail.m_rIcePwd, m_doc),allocate);
	val.AddMember("fingerprint",XGUTIL->setJsonParam(audioDetail.m_secureDetail.m_rFingerPrint, m_doc),allocate);
	val.AddMember("ssrc",XGUTIL->setJsonParam(audioDetail.m_secureDetail.m_rSsrc, m_doc),allocate);
	val.AddMember("cname",XGUTIL->setJsonParam(audioDetail.m_secureDetail.m_rCname, m_doc),allocate);
	val.AddMember("mslable",XGUTIL->setJsonParam(audioDetail.m_secureDetail.m_rMsLabel, m_doc),allocate);
	val.AddMember("lable",XGUTIL->setJsonParam(audioDetail.m_secureDetail.m_rLabel, m_doc),allocate);
	val.AddMember("dial_ip",XGUTIL->setJsonParam(audioDetail.m_dialIp, m_doc),allocate);
	val.AddMember("dial_port",XGUTIL->setJsonParam(audioDetail.m_dialPort),allocate);
	val.AddMember("resp_ip",XGUTIL->setJsonParam(audioDetail.m_respIp, m_doc),allocate);
	val.AddMember("resp_port",XGUTIL->setJsonParam(audioDetail.m_respPort),allocate);
  return val;  
}

Value MBCLinkDispatcher::fill_video_sdpinfo(MgMediaDetail& detail, Document &doc,Document::AllocatorType& allocate)
{
	Value val;
	val.SetObject();
  VideoDetail &videoDetail = detail.videoDetail; 
  val.AddMember("media_type",XGUTIL->setJsonParam(videoDetail.m_mediaType),allocate);
  val.AddMember("fmtp",XGUTIL->setJsonParam(videoDetail.m_fmtp, m_doc),allocate);
  val.AddMember("codec",XGUTIL->setJsonParam(videoDetail.m_codec),allocate);
  val.AddMember("codec_name",XGUTIL->setJsonParam(videoDetail.m_codecName, m_doc),allocate);
  val.AddMember("ptime",XGUTIL->setJsonParam(videoDetail.m_ptime),allocate);
  val.AddMember("play_file",XGUTIL->setJsonParam(videoDetail.m_playFile, m_doc),allocate);
  val.AddMember("ip_addr",XGUTIL->setJsonParam(videoDetail.m_clientIp, m_doc),allocate);
  val.AddMember("ip_port",XGUTIL->setJsonParam(videoDetail.m_clientPort),allocate);
  val.AddMember("relayip_addr",XGUTIL->setJsonParam(videoDetail.m_relayIp, m_doc),allocate);
  val.AddMember("relay_port",XGUTIL->setJsonParam(videoDetail.m_relayPort),allocate);
  val.AddMember("reflexip_addr",XGUTIL->setJsonParam(videoDetail.m_rflxIp, m_doc),allocate);
  val.AddMember("reflex_port",XGUTIL->setJsonParam(videoDetail.m_rflxPort),allocate);
  val.AddMember("ice_ufrag",XGUTIL->setJsonParam(videoDetail.m_secureDetail.m_rIceUfrag, m_doc),allocate);
  val.AddMember("ice_pwd",XGUTIL->setJsonParam(videoDetail.m_secureDetail.m_rIcePwd, m_doc),allocate);
  val.AddMember("fingerprint",XGUTIL->setJsonParam(videoDetail.m_secureDetail.m_rFingerPrint, m_doc),allocate);
  val.AddMember("ssrc",XGUTIL->setJsonParam(videoDetail.m_secureDetail.m_rSsrc, m_doc),allocate);
  val.AddMember("cname",XGUTIL->setJsonParam(videoDetail.m_secureDetail.m_rCname, m_doc),allocate);
  val.AddMember("mslable",XGUTIL->setJsonParam(videoDetail.m_secureDetail.m_rMsLabel, m_doc),allocate);
  val.AddMember("lable",XGUTIL->setJsonParam(videoDetail.m_secureDetail.m_rLabel, m_doc),allocate);
  val.AddMember("dial_ip",XGUTIL->setJsonParam(videoDetail.m_dialIp, m_doc),allocate);
  val.AddMember("dial_port",XGUTIL->setJsonParam(videoDetail.m_dialPort),allocate);
  val.AddMember("resp_ip",XGUTIL->setJsonParam(videoDetail.m_respIp, m_doc),allocate);
  val.AddMember("resp_port",XGUTIL->setJsonParam(videoDetail.m_respPort),allocate);
  val.AddMember("framerate",XGUTIL->setJsonParam(videoDetail.m_frameRate),allocate);
  val.AddMember("imageattr_x",XGUTIL->setJsonParam(videoDetail.m_imageattr_x),allocate);
  val.AddMember("imageattr_y",XGUTIL->setJsonParam(videoDetail.m_imageattr_y),allocate);
  return val;  
}

bool MBCLinkDispatcher::fetch_fd(xGateMBCLinkServiceMsg *pMsg)
{
  if(!pMsg){
    XGLOG_ERROR("MBCLinkDispatcher::fetch_fd failed to get xGateMBCLinkServiceMsg!");
    return false;
  }
  MgMediaDetail &mediaDetail = pMsg->get_media_detail(); 
  unsigned short gatewayId = mediaDetail.gateway_id;
  return MBCLINKUTIL->getFromMgConnectionMap(gatewayId, m_fd);
}

bool MBCLinkDispatcher::send_msg_on_tcp()
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

  XGLOG_DEBUG("call id (%s). Trying to send msgtype [%s]. with size of %d bytes of data: <%s>",m_uid.c_str(), m_strMsgTypeName.c_str(), len, data.c_str());
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

