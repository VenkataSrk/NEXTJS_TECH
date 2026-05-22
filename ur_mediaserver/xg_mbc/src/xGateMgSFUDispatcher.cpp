
//local includes
#include "xGateMgSFUDispatcher.h"
#include "xGateMgMsg.h"
#include "xGateUtil.h"
#include "xglog.h"
#include "xGateMBCUtil.h"
#define THISMODULE "MGSFUDispatcher"
#include "ClientService.h"
extern ClientService *pClntService;
xGateMgSFUDispatcher::xGateMgSFUDispatcher(const char* serverType) : xGateDispatcher(serverType)
{
  memset(m_serverType,0,SERVER_TYPE_SIZE);
  strcpy(m_serverType,serverType);
  memset(m_peerAddress,0,SERVER_ADDRESS_LENGTH);
  XGLOG_INFO( "xGateMgSFUDispatcher::xGateMgSFUDispatcher(%s)", m_serverType);
}

xGateMgSFUDispatcher::~xGateMgSFUDispatcher(void)
{
  XGLOG_INFO( "xGateMgSFUDispatcher::~xGateMgSFUDispatcher(%s)", m_serverType);
}

int xGateMgSFUDispatcher::svc(void)
{
  XGLOG_INFO( "xGateMgSFUDispatcher::svc(%s)", m_serverType);
  ACE_Message_Block *pAmb = 0;

  while(m_run)
  {
    // Get message from the queue
    int result = getq(pAmb, NULL);
    if((result == -1) && (errno == EWOULDBLOCK))
    {
      continue;
    }

    if ((result != -1))
    {
      process_msg(pAmb);
    }
    else
    {
      // This is the case where the message queue is de-activated.
      // Need to stop the thread
      XGLOG_INFO( "xGateMgSFUDispatcher::svc(%s) Error in Message Queue", m_serverType);
      m_run = false;
    }

    // delete the message received
    if(pAmb)
    {
      delete pAmb;
      pAmb = NULL;
    }
  }
  XGLOG_INFO( "xGateMgSFUDispatcher::svc(%s) Stopped Dispatch Task Thread", m_serverType);
  return 0;
}

//This function handle msg request from sofia.cpp(main thread) and post to MGDispatcher thread
xGateRetVal xGateMgSFUDispatcher::process_msg(ACE_Message_Block *pAmb)
{
  XGLOG_INFO( "MBC::xGateMgSFUDispatcher::process_msg(%s)", m_serverType);
  xGateRetVal retval = EN_XGATE_STATUS_SUCCESS;

  if(pAmb != NULL)
  {
    xGateMgMsg* pMgMsg = NULL;
    pMgMsg = dynamic_cast<xGateMgMsg*>(pAmb);
    if(pMgMsg  == NULL)
    {
      XGLOG_INFO( "xGateMgSFUDispatcher::process_msg recvd invalid xGateMgMsg Message");
    }

    if(encode_msg(pMgMsg) != EN_XGATE_STATUS_SUCCESS) {
      XGLOG_INFO( "xGateMgSFUDispatcher::process_msg not able to encode MG message");
    }
  }
  return retval;
}

xGateRetVal xGateMgSFUDispatcher::encode_msg(xGateMgMsg* pMgMsg)
{
  XGLOG_INFO( "Enter xGateMgSFUDispatcher::encode_msg(%s)", m_serverType);
  printf("\n myDbg xGateMgSFUDispatcher::encode_msg 18");
  xGateMgMsgType mgMsgType = pMgMsg->get_mg_msg_type();
  xGateNetConnection netConInfo; 
  pMgMsg->get_net_con_info(netConInfo);
  int len = 0, gatewayId = 0;

  //EN_XGATE_MG_REGISTER, we are getting direct json data from xGateMGCConnector thread
  if(mgMsgType == EN_XGATE_MG_REGISTER) {
    //TODO: Yoga, we don't need this registration concept in new MGC-MG server
    /*const char *data = 0;
    pMgMsg->get_data(data);
    len = pMgMsg->get_data_len();
    dispatch_msg(data, len, netConInfo);*/
    return EN_XGATE_STATUS_SUCCESS;
  }

  //Make jsonData for other messages
  string jsonBuff("");
  printf("\n19\n");
  len = create_media_msg(pMgMsg, jsonBuff, gatewayId);
  if(len <= 0) {
    XGLOG_ERROR( "xGateMgSFUDispatcher::encode_msg create_media_response_msg failed");
    return EN_XGATE_STATUS_ERROR;
  }

  printf("\n20\n");
  //dispatch json data
  if(dispatch_msg(jsonBuff.c_str(), len, netConInfo, pMgMsg) != EN_XGATE_STATUS_SUCCESS) {
    XGLOG_ERROR( "xGateMgSFUDispatcher::encode_msg dispatch failed");
    return EN_XGATE_STATUS_ERROR;
  }

  return EN_XGATE_STATUS_SUCCESS;
}

Value xGateMgSFUDispatcher::setURMeetUserListParam(MgMediaDetail& detail, Document& m_doc)
{
    Value retVal;
    Document::AllocatorType& allocator = m_doc.GetAllocator();
    Value vArray(kArrayType);

    if (!vArray.IsArray()) {
        XGLOG_ERROR("setURMeetUserListParam failed. It's not an array object!");
        return retVal;
    }

    for (const auto& userInfo : detail.meetuser_info) {
        Value objectValue(kObjectType);
        objectValue.AddMember("email", setJsonParam(userInfo.email, m_doc), allocator);
        objectValue.AddMember("profile_img", setJsonParam(userInfo.profile_img, m_doc), allocator);
        objectValue.AddMember("sip_id", setJsonParam(userInfo.sip_id, m_doc), allocator);
        objectValue.AddMember("name", setJsonParam(userInfo.name, m_doc), allocator);
        objectValue.AddMember("ext", setJsonParam(userInfo.ext, m_doc), allocator);
        objectValue.AddMember("user_type", setJsonParam(userInfo.user_type, m_doc), allocator);
        objectValue.AddMember("role_type", setJsonParam(userInfo.role_type, m_doc), allocator);
        vArray.PushBack(objectValue, allocator);
    }

    return vArray;
}

Value xGateMgSFUDispatcher::setJsonParam(string strValue,Document &doc ){
 Value retVal;
 retVal.SetString(strValue.c_str(), strValue.length(), doc.GetAllocator());
 return retVal;
}
Value xGateMgSFUDispatcher::setJsonParam(unsigned int iValue, Document &doc){
 Value retVal;
 retVal = iValue;
 return retVal;
}

Value xGateMgSFUDispatcher::setJsonParam(MgMediaDetail& media, Document &doc){
  Value vArray(kArrayType);
  Document::AllocatorType& allocate =  doc.GetAllocator();
  for (int i =0 ; i< 2 ; i++){
   //if(media.audioDetail.m_mediaType > 0){
     Value objectValue;
     objectValue.SetObject();
     if(i == 0 && media.audioDetail.m_mediaType == 2) {
        objectValue.AddMember("media_type",setJsonParam(media.audioDetail.m_mediaType,doc),allocate);
        objectValue.AddMember("fmtp",setJsonParam(media.audioDetail.m_fmtp,doc),allocate);
        objectValue.AddMember("codec",setJsonParam(media.audioDetail.m_codec,doc),allocate);
        objectValue.AddMember("codec_name",setJsonParam(media.audioDetail.m_codecName,doc),allocate);
        objectValue.AddMember("ptime",setJsonParam(media.audioDetail.m_ptime,doc),allocate);
        objectValue.AddMember("play_file",setJsonParam(media.audioDetail.m_playFile,doc),allocate);
        objectValue.AddMember("ip_addr",setJsonParam(media.audioDetail.m_clientIp,doc),allocate);
        objectValue.AddMember("ip_port",setJsonParam(media.audioDetail.m_clientPort,doc),allocate);
        objectValue.AddMember("relayip_addr",setJsonParam(media.audioDetail.m_relayIp,doc),allocate);
        objectValue.AddMember("relay_port",setJsonParam(media.audioDetail.m_relayPort,doc),allocate);
        objectValue.AddMember("reflexip_addr",setJsonParam(media.audioDetail.m_rflxIp,doc),allocate);
        objectValue.AddMember("reflex_port",setJsonParam(media.audioDetail.m_rflxPort,doc),allocate);
        objectValue.AddMember("ice_ufrag",setJsonParam(media.audioDetail.m_secureDetail.m_lIceUfrag,doc),allocate);
        objectValue.AddMember("ice_pwd",setJsonParam(media.audioDetail.m_secureDetail.m_lIcePwd,doc),allocate);
        objectValue.AddMember("fingerprint",setJsonParam(media.audioDetail.m_secureDetail.m_lFingerPrint,doc),allocate);
        objectValue.AddMember("ssrc",setJsonParam(media.audioDetail.m_secureDetail.m_lSsrc,doc),allocate);
        objectValue.AddMember("cname",setJsonParam(media.audioDetail.m_secureDetail.m_lCname,doc),allocate);
        objectValue.AddMember("mslable",setJsonParam(media.audioDetail.m_secureDetail.m_lMsLabel,doc),allocate);
        objectValue.AddMember("lable",setJsonParam(media.audioDetail.m_secureDetail.m_lLabel,doc),allocate);
        objectValue.AddMember("dial_ip",setJsonParam(media.audioDetail.m_dialIp,doc),allocate);
        objectValue.AddMember("dial_port",setJsonParam(media.audioDetail.m_dialPort,doc),allocate);
        objectValue.AddMember("resp_ip",setJsonParam(media.audioDetail.m_respIp,doc),allocate);
        objectValue.AddMember("resp_port",setJsonParam(media.audioDetail.m_respPort,doc),allocate);
        vArray.PushBack(objectValue,allocate);
     } else if(i == 1 && media.videoDetail.m_mediaType == 3) {
        objectValue.AddMember("media_type",setJsonParam(media.videoDetail.m_mediaType,doc),allocate);
        objectValue.AddMember("fmtp",setJsonParam(media.videoDetail.m_fmtp,doc),allocate);
        objectValue.AddMember("codec",setJsonParam(media.videoDetail.m_codec,doc),allocate);
        objectValue.AddMember("codec_name",setJsonParam(media.videoDetail.m_codecName,doc),allocate);
        objectValue.AddMember("framerate",setJsonParam(media.videoDetail.m_frameRate,doc),allocate);
        objectValue.AddMember("imageattr_x",setJsonParam(media.videoDetail.m_imageattr_x,doc),allocate);
        objectValue.AddMember("imageattr_y",setJsonParam(media.videoDetail.m_imageattr_y,doc),allocate);
        objectValue.AddMember("play_file",setJsonParam(media.videoDetail.m_playFile,doc),allocate);
        objectValue.AddMember("ip_addr",setJsonParam(media.videoDetail.m_clientIp,doc),allocate);
        objectValue.AddMember("ip_port",setJsonParam(media.videoDetail.m_clientPort,doc),allocate);
        objectValue.AddMember("relayip_addr",setJsonParam(media.videoDetail.m_relayIp,doc),allocate);
        objectValue.AddMember("relay_port",setJsonParam(media.videoDetail.m_relayPort,doc),allocate);
        objectValue.AddMember("reflexip_addr",setJsonParam(media.videoDetail.m_rflxIp,doc),allocate);
        objectValue.AddMember("reflex_port",setJsonParam(media.videoDetail.m_rflxPort,doc),allocate);
        objectValue.AddMember("ice_ufrag",setJsonParam(media.videoDetail.m_secureDetail.m_lIceUfrag,doc),allocate);
        objectValue.AddMember("ice_pwd",setJsonParam(media.videoDetail.m_secureDetail.m_lIcePwd,doc),allocate);
        objectValue.AddMember("fingerprint",setJsonParam(media.videoDetail.m_secureDetail.m_lFingerPrint,doc),allocate);
        objectValue.AddMember("ssrc",setJsonParam(media.videoDetail.m_secureDetail.m_lSsrc,doc),allocate);
        objectValue.AddMember("cname",setJsonParam(media.videoDetail.m_secureDetail.m_lCname,doc),allocate);
        objectValue.AddMember("mslable",setJsonParam(media.videoDetail.m_secureDetail.m_lMsLabel,doc),allocate);
        objectValue.AddMember("lable",setJsonParam(media.videoDetail.m_secureDetail.m_lLabel,doc),allocate);
        objectValue.AddMember("dial_ip",setJsonParam(media.videoDetail.m_dialIp,doc),allocate);
        objectValue.AddMember("dial_port",setJsonParam(media.videoDetail.m_dialPort,doc),allocate);
        objectValue.AddMember("resp_ip",setJsonParam(media.videoDetail.m_respIp,doc),allocate);
        objectValue.AddMember("resp_port",setJsonParam(media.videoDetail.m_respPort,doc),allocate);
        vArray.PushBack(objectValue,allocate);
     }
     //vArray.PushBack(objectValue,allocate);
   //}
  }     
  return vArray;
}

int xGateMgSFUDispatcher::create_media_msg(xGateMgMsg *pMgMsg, string &jsonBuff, int &gatewayId)
{
  XGLOG_INFO( "Enter xGateMgSFUDispatcher::create_media_msg(%s)", m_serverType);
  Document doc;
  StringBuffer s;
  Writer<StringBuffer> writer(s);
  MgMediaDetail &mediaDetail = pMgMsg->get_media_detail();
  XGLOG_INFO("sdp_str:%s",mediaDetail.sdp_str.c_str());

  int len = 0;
  len = sizeof(json_mg_media_tmpl);
  char buff[len];
  memcpy(buff, json_mg_media_tmpl, len);

  if (doc.ParseInsitu(buff).HasParseError()) {
    XGLOG_ERROR( "xGateMgSFUDispatcher::create_media_msg json parse error");
    return EN_XGATE_STATUS_ERROR;
  }
  if(!doc.IsObject()) {
    XGLOG_ERROR( "xGateMgSFUDispatcher::create_media_msg doc object creation failed");
    return EN_XGATE_STATUS_ERROR;
  }

  Value valMsgType;
  valMsgType = pMgMsg->get_mg_msg_type();
  doc["msg_type"] = valMsgType;

 // doc["msg_type"] = pMgMsg->get_mg_msg_type();
  
  doc["data"]["mgresource_id"] = setJsonParam(mediaDetail.mgresource_id,doc);
  doc["data"]["call_type"] = setJsonParam(mediaDetail.call_type,doc);
  doc["data"]["call_id"] = setJsonParam(mediaDetail.call_id,doc);
  doc["data"]["gateway_id"] = setJsonParam(mediaDetail.gateway_id,doc);
  doc["data"]["context_id"] = setJsonParam(mediaDetail.context_id,doc);
  doc["data"]["leg_id"] = setJsonParam(mediaDetail.leg_id,doc);
  doc["data"]["retrieved_audiossrc"] = setJsonParam(mediaDetail.retrieved_audiossrc,doc);
  doc["data"]["retrieved_videossrc"] = setJsonParam(mediaDetail.retrieved_videossrc,doc);
  doc["data"]["media_proto"] = setJsonParam(mediaDetail.media_proto,doc);
  doc["data"]["out_proto"] = setJsonParam(mediaDetail.out_proto,doc);
  doc["data"]["record_file"] = setJsonParam(mediaDetail.record_file,doc);
  doc["data"]["browser_type"] = setJsonParam(mediaDetail.browser_type,doc);
  doc["data"]["record_url"] = setJsonParam(mediaDetail.record_url,doc);
  doc["data"]["meeting_name"] = setJsonParam(mediaDetail.meeting_name,doc);
  doc["data"]["sip_id"] = setJsonParam(mediaDetail.sip_id,doc);
  doc["data"]["file_size"] = setJsonParam(mediaDetail.file_size,doc);
  doc["data"]["sig_type"] = setJsonParam(mediaDetail.sig_type,doc);
  doc["data"]["call_dir"]= setJsonParam(mediaDetail.call_dir,doc);
  doc["data"]["d_r_e"]= setJsonParam(mediaDetail.d_rtp_ep,doc);
  doc["data"]["dtmf_digits"]= setJsonParam(mediaDetail.dtmf_digits,doc);
  doc["data"]["joiner_type"]= setJsonParam(mediaDetail.joiner_type,doc);
  doc["data"]["media_state"] = setJsonParam(mediaDetail.media_event,doc);
  doc["data"]["sdpinfo"] = setJsonParam(mediaDetail,doc);
  doc["data"]["sdp_str"] = setJsonParam(mediaDetail.sdp_str,doc);  
  doc["data"]["call_Id_msg10"]= setJsonParam(mediaDetail.call_Id_msg10,doc);
  doc["data"]["pClient"]= setJsonParam(mediaDetail.pClient,doc);
  doc["data"]["urmeet_user_list"]= setURMeetUserListParam(mediaDetail,doc);
  doc["data"]["prev_act_spkr"] = setJsonParam(mediaDetail.prev_act_spkr,doc);
  doc.Accept(writer);
  //get the length of created message
  len = s.GetLength();

  if ( len > 999) {
    doc["msg_len"] = len+1;
  } else {
    doc["msg_len"] = len;
  }
  s.Clear();
  doc.Accept(writer);
  printf("\n***********************************\n");
  puts(s.GetString());
  jsonBuff = s.GetString();
  string delimiter("\r\n\r\n");
  jsonBuff.append(delimiter.c_str(), delimiter.length());
  printf("***********************************\n");
  XGLOG_ERROR( "xGateMgSFUDispatcher::create_media_msg created media msg: %s", jsonBuff.c_str());
  return jsonBuff.length();
}

xGateRetVal xGateMgSFUDispatcher::dispatch_msg(const char *data, int len, xGateNetConnection &netConInfo, xGateMgMsg* pMgMsg)
{
  XGLOG_INFO( "Enter xGateMgSFUDispatcher::dispatch_msg(%s)",m_serverType);
  printf("\n21\n");
  xGateRetVal retval = EN_XGATE_STATUS_SUCCESS;
  printf("\n22\n");
  ACE_SOCK_Stream socket_id;
  XGLOG_INFO( "Enter MBCControllerProcessor::post_media_detail_sfu_bridge: mbc: before sfu.SendMsg pClntService");
  int sfu_id  = pMgMsg->get_sfu_id();
  while(1)
  {
    //retval = xGateMBCUtil::getSocketIdFromSfuIdMap(sfu_id, socket_id);
    //if(pClntService == NULL)
    if(retval == EN_XGATE_STATUS_NOT_PRESENT)
    {
      sleep(1);
      printf("\n pClntService is NULL");
      continue;
    }
    printf("\n pClntService is NOT NULL, calling SendMsg");
    pClntService->SendMsg((char *)data, len, netConInfo, pMgMsg);
    break; 
  }
  return retval;
}

xGateRetVal xGateMgSFUDispatcher::send_msg_on_tcp(unsigned int fd, const char *pBuffer, int len)
{
  XGLOG_INFO( "Enter xGateMgSFUDispatcher::send_msg_on_tcp(%s) FD:%d",m_serverType,fd);
  errno = 0;
  int retVal = send(fd, pBuffer, len, MSG_DONTWAIT);
  //TODO: Yoga, we may need send timeout mechanism and then retry logic here
  if (retVal == -1)
  {
    XGLOG_ERROR( "xGateMgSFUDispatcher::send_msg_on_tcp(%s)" \
        " send failed Error:%s", m_serverType, strerror(errno));
    return EN_XGATE_STATUS_ERROR;
  }  

  XGLOG_INFO( "xGateMgSFUDispatcher::send_msg_on_tcp(%s)" \
      " Success Fd:%d Message:\n:%s", m_serverType, fd, pBuffer);
  return EN_XGATE_STATUS_SUCCESS;
}
