
//local includes
#include "xglog.h"
#include "xGateMGDispatcher.h"
#include "xGateMgMsg.h"
#include "xGateUtil.h"
#define THISMODULE "MGDispatcher"

xGateMGDispatcher::xGateMGDispatcher(const char* serverType) : xGateDispatcher(serverType)
{
  memset(m_serverType,0,SERVER_TYPE_SIZE);
  strcpy(m_serverType,serverType);
  memset(m_peerAddress,0,SERVER_ADDRESS_LENGTH);
  XGLOG_INFO( "xGateMGDispatcher::xGateMGDispatcher(%s)", (char*)ur_log_string (m_serverType));
}

xGateMGDispatcher::~xGateMGDispatcher(void)
{
  XGLOG_INFO( "xGateMGDispatcher::~xGateMGDispatcher(%s)", (char*)ur_log_string (m_serverType));
}

int xGateMGDispatcher::svc(void)
{
  XGLOG_INFO( "xGateMGDispatcher::svc(%s)", (char*)ur_log_string (m_serverType));
  ACE_Message_Block *pAmb = 0;

  while(m_run)
  {
    ACE_Time_Value tv ((time(NULL) + 20));
    // Get message from the queue
    int result = getq(pAmb, &tv);
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
      XGLOG_INFO( "xGateMGDispatcher::svc(%s) Error in Message Queue", (char*)ur_log_string (m_serverType));
      m_run = false;
    }

    // delete the message received
    if(pAmb)
    {
      delete pAmb;
      pAmb = NULL;
    }
  }
  XGLOG_INFO( "xGateMGDispatcher::svc(%s) Stopped Dispatch Task Thread", (char*)ur_log_string (m_serverType));
  return 0;
}

//This function handle msg request from sofia.cpp(main thread) and post to MGDispatcher thread
xGateRetVal xGateMGDispatcher::process_msg(ACE_Message_Block *pAmb)
{
  XGLOG_INFO( "xGateMGDispatcher::process_msg(%s)", (char*)ur_log_string (m_serverType));
  xGateRetVal retval = EN_XGATE_STATUS_SUCCESS;

  if(pAmb != NULL)
  {
    xGateMgMsg* pMgMsg = NULL;
    pMgMsg = dynamic_cast<xGateMgMsg*>(pAmb);
    if(pMgMsg  == NULL)
    {
      XGLOG_INFO( "xGateMGDispatcher::process_msg recvd invalid xGateMgMsg Message");
    }

    if(encode_msg(pMgMsg) != EN_XGATE_STATUS_SUCCESS) {
      XGLOG_INFO( "xGateMGDispatcher::process_msg not able to encode MG message");
    }

  }
  return retval;
}

xGateRetVal xGateMGDispatcher::encode_msg(xGateMgMsg* pMgMsg)
{
  XGLOG_INFO( "Enter xGateMGDispatcher::encode_msg(%s)", (char*)ur_log_string (m_serverType));
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
  len = create_media_msg(pMgMsg, jsonBuff, gatewayId);
  if(len <= 0) {
    XGLOG_ERROR( "xGateMGDispatcher::encode_msg create_media_response_msg failed");
    return EN_XGATE_STATUS_ERROR;
  }

  //dispatch json data
  if(dispatch_msg(jsonBuff.c_str(), len, netConInfo) != EN_XGATE_STATUS_SUCCESS) {
    XGLOG_ERROR( "xGateMGDispatcher::encode_msg dispatch failed");
    return EN_XGATE_STATUS_ERROR;
  }

  return EN_XGATE_STATUS_SUCCESS;
}

Value xGateMGDispatcher::setJsonParam(string strValue,Document &doc ){
 Value retVal;
 retVal.SetString(strValue.c_str(), strValue.length(), doc.GetAllocator());
 return retVal;
}
Value xGateMGDispatcher::setJsonParam(unsigned int iValue, Document &doc){
 Value retVal;
 retVal = iValue;
 return retVal;
}

Value xGateMGDispatcher::setJsonParam(TtsDisconnectInfo& disconn_info, Document &doc){
	Value objectValue(kObjectType);
	Document::AllocatorType& allocate =  doc.GetAllocator();
	objectValue.AddMember("disconn_code",setJsonParam(disconn_info.disconnCode,doc),allocate);
	objectValue.AddMember("disconn_reason",setJsonParam(disconn_info.disconnReason,doc),allocate);
	return objectValue;
}

Value xGateMGDispatcher::setJsonParam(MgMediaDetail& media, Document &doc){
  Value vArray(kArrayType);
  Document::AllocatorType& allocate =  doc.GetAllocator();
  for (int i =0 ; i< 2 ; i++){
   //if(media.audioDetail.m_mediaType > 0){
     Value objectValue;
     objectValue.SetObject();
     if(i == 0 && media.audioDetail.m_mediaType == 2) {
        objectValue.AddMember("media_type",setJsonParam(media.audioDetail.m_mediaType,doc),allocate);
        objectValue.AddMember("fmtp",setJsonParam(media.audioDetail.m_fmtp,doc),allocate);
        objectValue.AddMember("sampling_rate",setJsonParam(media.audioDetail.m_samplingRate,doc),allocate);
        objectValue.AddMember("dtmf_sampling_rate",setJsonParam(media.audioDetail.m_dtmfSamplingRate,doc),allocate);
        objectValue.AddMember("dtmf_pt",setJsonParam(media.audioDetail.m_dtmfpt,doc),allocate);
        objectValue.AddMember("dtmf_fmtp",setJsonParam(media.audioDetail.m_dtmfFmtp,doc),allocate);
        objectValue.AddMember("cn_pt",setJsonParam(media.audioDetail.m_cnPt,doc),allocate);
        objectValue.AddMember("cn_sampling_rate",setJsonParam(media.audioDetail.m_cnSamplingRate,doc),allocate);
        objectValue.AddMember("rtcp_fp_payload",setJsonParam(media.audioDetail.m_rtcpFpPayload,doc),allocate);
        objectValue.AddMember("rtcp_fp_mechanism",setJsonParam(media.audioDetail.m_rtcpFpMechanism,doc),allocate);
        objectValue.AddMember("codec",setJsonParam(media.audioDetail.m_codec,doc),allocate);
        objectValue.AddMember("codec_name",setJsonParam(media.audioDetail.m_codecName,doc),allocate);
        objectValue.AddMember("ptime",setJsonParam(media.audioDetail.m_ptime,doc),allocate);
        objectValue.AddMember("ip_addr",setJsonParam(media.audioDetail.m_clientIp,doc),allocate);
        objectValue.AddMember("ip_port",setJsonParam(media.audioDetail.m_clientPort,doc),allocate);
        objectValue.AddMember("relayip_addr",setJsonParam(media.audioDetail.m_relayIp,doc),allocate);
        objectValue.AddMember("relay_port",setJsonParam(media.audioDetail.m_relayPort,doc),allocate);
        objectValue.AddMember("reflexip_addr",setJsonParam(media.audioDetail.m_rflxIp,doc),allocate);
        objectValue.AddMember("reflex_port",setJsonParam(media.audioDetail.m_rflxPort,doc),allocate);
        objectValue.AddMember("dial_ip",setJsonParam(media.audioDetail.m_dialIp,doc),allocate);
        objectValue.AddMember("dial_port",setJsonParam(media.audioDetail.m_dialPort,doc),allocate);
        objectValue.AddMember("resp_ip",setJsonParam(media.audioDetail.m_respIp,doc),allocate);
        objectValue.AddMember("resp_port",setJsonParam(media.audioDetail.m_respPort,doc),allocate);
        objectValue.AddMember("inband_dtmf",setJsonParam(media.audioDetail.m_inBandDtmf,doc),allocate);
        vArray.PushBack(objectValue,allocate);
     }
     //vArray.PushBack(objectValue,allocate);
   //}
  }     
  return vArray;
}

int xGateMGDispatcher::create_media_msg(xGateMgMsg *pMgMsg, string &jsonBuff, int &gatewayId)
{
  XGLOG_INFO( "Enter xGateMGDispatcher::create_media_msg(%s)", (char*)ur_log_string (m_serverType));
  Document doc;
  StringBuffer s;
  Writer<StringBuffer> writer(s);
  MgMediaDetail &mediaDetail = pMgMsg->get_media_detail();
  string ipv4(xGateUtil::getLocalIpAddr(AF_INET));
  string ipv6(xGateUtil::getLocalIpAddr(AF_INET6));
  mediaDetail.mg_ipv4_intf = ipv4;
  mediaDetail.mg_ipv6_intf = ipv6;

  int len = 0;
  len = sizeof(json_mg_media_tmpl);
  char buff[len];
  memcpy(buff, json_mg_media_tmpl, len);

  if (doc.ParseInsitu(buff).HasParseError()) {
    XGLOG_ERROR( "xGateMGDispatcher::create_media_msg json parse error");
    return EN_XGATE_STATUS_ERROR;
  }
  if(!doc.IsObject()) {
    XGLOG_ERROR( "xGateMGDispatcher::create_media_msg doc object creation failed");
    return EN_XGATE_STATUS_ERROR;
  }

  Value valMsgType;
  valMsgType = pMgMsg->get_mg_msg_type();
  doc["msg_type"] = valMsgType;

 // doc["msg_type"] = pMgMsg->get_mg_msg_type();
  
  doc["data"]["mgresource_id"] = setJsonParam(mediaDetail.mgresource_id,doc);
  doc["data"]["call_type"] = setJsonParam(mediaDetail.call_type,doc);
  doc["data"]["call_id"] = setJsonParam(mediaDetail.call_id,doc);
  doc["data"]["group_call_id"] = setJsonParam(mediaDetail.group_call_id,doc);
  doc["data"]["switch_call_id"] = setJsonParam(mediaDetail.switch_call_id,doc);
  doc["data"]["gateway_id"] = setJsonParam(mediaDetail.gateway_id,doc);
  doc["data"]["context_id"] = setJsonParam(mediaDetail.context_id,doc);
  doc["data"]["leg_id"] = setJsonParam(mediaDetail.leg_id,doc);
  doc["data"]["media_proto"] = setJsonParam(mediaDetail.media_proto,doc);
  doc["data"]["out_proto"] = setJsonParam(mediaDetail.out_proto,doc);
  doc["data"]["record_file"] = setJsonParam(mediaDetail.record_file,doc);
  doc["data"]["record_type"] = setJsonParam(mediaDetail.record_type,doc);
  doc["data"]["file_size"] = setJsonParam(mediaDetail.file_size,doc);
  doc["data"]["mg_ipv4_intf"] = setJsonParam(mediaDetail.mg_ipv4_intf,doc);
  doc["data"]["mg_ipv6_intf"] = setJsonParam(mediaDetail.mg_ipv6_intf,doc);
  doc["data"]["sig_type"] = setJsonParam(mediaDetail.sig_type,doc);
  doc["data"]["call_dir"]= setJsonParam(mediaDetail.call_dir,doc);
  doc["data"]["rtp_endpoint"]= setJsonParam(mediaDetail.rtp_ep,doc);
  doc["data"]["dtmf_digits"]= setJsonParam(mediaDetail.dtmf_digits,doc);
  doc["data"]["joiner_type"]= setJsonParam(mediaDetail.joiner_type,doc);
  doc["data"]["reset_flag"]= setJsonParam(mediaDetail.reset_flag,doc);
  doc["data"]["media_state"] = setJsonParam(mediaDetail.media_event,doc);
  doc["data"]["mrcp_state"] = setJsonParam((int)mediaDetail.mrcp_state,doc);
  doc["data"]["mrcp_port_alocate"]= setJsonParam(mediaDetail.mrcp_port_allocate,doc);
  doc["data"]["mrcp_port_activate"] = setJsonParam(mediaDetail.mrcp_port_activate,doc);
  doc["data"]["mrcp_port"]= setJsonParam(mediaDetail.mrcp_port,doc);
  doc["data"]["mrcp_ip_info"] = setJsonParam(mediaDetail.mrcp_ip_info,doc);
  doc["data"]["sdpinfo"] = setJsonParam(mediaDetail,doc);
  doc["data"]["tts_disconnect"] = setJsonParam(mediaDetail.tts_disconnect_info,doc);
  doc["data"]["transfer_to"] = setJsonParam(mediaDetail.transfer_to,doc);
  doc["data"]["ccaas_voice_recog_port"]= setJsonParam(mediaDetail.ccaas_voice_recog_port,doc);
  
 cout<<"The ccaas_voice_recog_port data is"<<mediaDetail.ccaas_voice_recog_port<<endl;
  
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
  XGLOG_ERROR( "xGateMGDispatcher::create_media_msg created media msg: %s", (char*)ur_log_string (jsonBuff));
  return jsonBuff.length();
}

//TODO: Yoga, this function not yet completed fully
xGateRetVal xGateMGDispatcher::dispatch_msg(const char *data, int len, xGateNetConnection &netConInfo)
{
  XGLOG_INFO( "Enter xGateMGDispatcher::dispatch_msg(%s)",(char*)ur_log_string (m_serverType));
  xGateRetVal retval = EN_XGATE_STATUS_SUCCESS;

  int fd;
  //unsigned int index = 0;
  //for(index=0;index<xGateUtil::m_MgcIpAddressList.size();index++)
  //{
    //string ipAddress     = xGateUtil::m_MgcIpAddressList[index];
    string ipAddress     = netConInfo.recvIp_; //TODO: Yoga, testing purpose temporarly we getting first connected MGC ip
    //unsigned short port  = 32779;
    unsigned short port  = netConInfo.recvPort_;
    //unsigned short mgcPort = 32779;
    XGLOG_INFO( "xGateMGDispatcher::dispatch_msg(%s)" \
        " Sending MgMsg to TCP Socket Address %s:%d ",(char*)ur_log_string (m_serverType),
        (char*)ur_log_string (ipAddress), port);

    retval = xGateUtil::getFromMgConnectionMap(ipAddress,port,fd);
    if(retval != EN_XGATE_STATUS_SUCCESS) {
      XGLOG_ERROR( "xGateMGDispatcher::dispatch_msg(%s)"\
          " No connection Present for the address:%s:%d", (char*)ur_log_string (m_serverType), (char*)ur_log_string (ipAddress), port);
      return EN_XGATE_STATUS_ERROR;
    }
    retval = send_msg_on_tcp(fd, data, len);
    return retval;
  //}
}

xGateRetVal xGateMGDispatcher::send_msg_on_tcp(unsigned int fd, const char *pBuffer, int len)
{
  XGLOG_INFO( "Enter xGateMGDispatcher::send_msg_on_tcp(%s) FD:%d",(char*)ur_log_string (m_serverType),fd);
  errno = 0;
  int retVal = send(fd, pBuffer, len, MSG_DONTWAIT);
  //TODO: Yoga, we may need send timeout mechanism and then retry logic here
  if (retVal == -1)
  {
    XGLOG_ERROR( "xGateMGDispatcher::send_msg_on_tcp(%s)" \
        " send failed Error:%s", (char*)ur_log_string (m_serverType), (char*)ur_log_string (strerror(errno)));
    return EN_XGATE_STATUS_ERROR;
  }  

  XGLOG_INFO( "xGateMGDispatcher::send_msg_on_tcp(%s)" \
      " Success Fd:%d Message:\n:%s", (char*)ur_log_string (m_serverType), fd, (char*)ur_log_string (pBuffer));
  return EN_XGATE_STATUS_SUCCESS;
}
