
//local includes
#include "xglog.h"
#include "xGateMGDispatcher.h"
#include "xGateMgMsg.h"

#define THISMODULE "MGDispatcher"

xGateMGDispatcher::xGateMGDispatcher(const char* serverType) : xGateDispatcher(serverType)
{
  memset(m_serverType,0,SERVER_TYPE_SIZE);
  strcpy(m_serverType,serverType);
  memset(m_peerAddress,0,SERVER_ADDRESS_LENGTH);
 XGLOG_INFO("xGateMGDispatcher::xGateMGDispatcher(%s)", m_serverType);
}

xGateMGDispatcher::~xGateMGDispatcher(void)
{
 XGLOG_INFO("xGateMGDispatcher::~xGateMGDispatcher(%s)", m_serverType);
}

int xGateMGDispatcher::svc(void)
{
 XGLOG_INFO("xGateMGDispatcher::svc(%s)", m_serverType);

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
      XGLOG_ERROR( "xGateMGDispatcher::svc(%s) Error in Message Queue", m_serverType);
      m_run = false;
    }

    // delete the message received
    if(pAmb)
    {
      delete pAmb;
      pAmb = NULL;
    }
  }
 XGLOG_INFO("xGateMGDispatcher::svc(%s) Stopped Dispatch Task Thread", m_serverType);
  return 0;
}

//This function handle msg request from sofia.cpp(main thread) and post to MGDispatcher thread
xGateRetVal xGateMGDispatcher::process_msg(ACE_Message_Block *pAmb)
{
 XGLOG_INFO("xGateMGDispatcher::process_msg(%s)", m_serverType);
  xGateRetVal retval = EN_XGATE_STATUS_SUCCESS;

  if(pAmb != NULL)
  {
    xGateMgMsg* pMgMsg = NULL;
    pMgMsg = dynamic_cast<xGateMgMsg*>(pAmb);
    if(pMgMsg  == NULL)
    {
      XGLOG_ERROR( "xGateMGDispatcher::process_msg recvd invalid xGateMgMsg Message");
    }

    if(encode_msg(pMgMsg) != EN_XGATE_STATUS_SUCCESS) {
      XGLOG_ERROR( "xGateMGDispatcher::process_msg not able to encode MG message");
    }

  }
  return retval;
}

xGateRetVal xGateMGDispatcher::encode_msg(xGateMgMsg* pMgMsg)
{
 XGLOG_INFO("Enter xGateMGDispatcher::encode_msg(%s)",m_serverType);
  
  //Make jsonData for other messages
  int len, gatewayId = 0;
  string jsonBuff("");
  len = create_media_msg(pMgMsg, jsonBuff,gatewayId);
  if(len <= 0) {
    XGLOG_ERROR( "xGateMGDispatcher::encode_msg create_media_response_msg failed");
    return EN_XGATE_STATUS_ERROR;
  }

  //string testJsonBuff = "{'msg_type':3,'msg_len':383,'data':{'call_id':'33759293198@10.22.2.82;19117SIPpTag001-1-19117@10.22.2.82','gateway_id':0,'context_id':0,'ip_addr':'10.22.2.82','ip_port':6010,'relayip_addr':'','relay_port':0,'reflexip_addr':'','reflex_port':0,'codec':8,'ice_ufrag':'','ice_pwd':'','fingerprint':'','ptime':0,'annexb':0,'media_proto':'RTP','sig_type':3,'call_dir':1,'rtp_endpoint':''}}\r\n\r\n{'msg_type':4,'msg_len':368,'data':{'call_id':'33759293198@10.22.2.82;19117SIPpTag001-1-19117@10.22.2.82','gateway_id':0,'context_id':0,'ip_addr':'','ip_port':0,'relayip_addr':'','relay_port':0,'reflexip_addr':'','reflex_port':0,'codec':0,'ice_ufrag':'','ice_pwd':'','fingerprint':'','ptime':20,'annexb':0,'media_proto':'','sig_type':3,'call_dir':0,'rtp_endpoint':''}}\r\n\r\n";

  //dispatch json data
  if(dispatch_msg(jsonBuff.c_str(), len, gatewayId) != EN_XGATE_STATUS_SUCCESS) {
  //if(dispatch_msg(testJsonBuff.c_str(), testJsonBuff.length(), gatewayId) != EN_XGATE_STATUS_SUCCESS) {
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
Value xGateMGDispatcher::setJsonParam(MgMediaDetail& media, Document &doc){
Value vArray(kArrayType);
Document::AllocatorType& allocate =  doc.GetAllocator();
for (int i =0 ; i< 2 ; i++){
if(media.sdpinfo[i].mediaType > 0){
 Value objectValue;
 ;
 objectValue.SetObject();
 
 objectValue.AddMember("media_type",setJsonParam(media.sdpinfo[i].mediaType,doc),allocate);
 objectValue.AddMember("ip_addr",setJsonParam(media.sdpinfo[i].ip_addr,doc),allocate);
 objectValue.AddMember("ip_port",setJsonParam(media.sdpinfo[i].port,doc),allocate);
 objectValue.AddMember("dial_ip",setJsonParam(media.sdpinfo[i].dial_ip,doc),allocate);
 objectValue.AddMember("dial_port",setJsonParam(media.sdpinfo[i].dial_port,doc),allocate);
 objectValue.AddMember("resp_ip",setJsonParam(media.sdpinfo[i].resp_ip,doc),allocate);
 objectValue.AddMember("resp_port",setJsonParam(media.sdpinfo[i].resp_port,doc),allocate);
 objectValue.AddMember("relayip_addr",setJsonParam(media.sdpinfo[i].relayip_addr,doc),allocate);
 objectValue.AddMember("relay_port",setJsonParam(media.sdpinfo[i].relay_port,doc),allocate);
 objectValue.AddMember("reflexip_addr",setJsonParam(media.sdpinfo[i].reflexip_addr,doc),allocate);
 objectValue.AddMember("reflex_port",setJsonParam(media.sdpinfo[i].reflex_port,doc),allocate);
 objectValue.AddMember("codec",setJsonParam(media.sdpinfo[i].codec,doc),allocate);
 objectValue.AddMember("codec_name",setJsonParam(media.sdpinfo[i].codecname,doc),allocate);
 objectValue.AddMember("fmtp",setJsonParam(media.sdpinfo[i].fmtp,doc),allocate);
 objectValue.AddMember("ice_ufrag",setJsonParam(media.sdpinfo[i].ice_ufrag,doc),allocate);
 objectValue.AddMember("ice_pwd",setJsonParam(media.sdpinfo[i].ice_pwd,doc),allocate);
 objectValue.AddMember("fingerprint",setJsonParam(media.sdpinfo[i].fingerprint,doc),allocate);
 objectValue.AddMember("ptime",setJsonParam(media.sdpinfo[i].ptime,doc),allocate);
 objectValue.AddMember("framerate",setJsonParam(media.sdpinfo[i].framerate,doc),allocate);
 objectValue.AddMember("imageattr_x",setJsonParam(media.sdpinfo[i].imageattr_x,doc),allocate);
 objectValue.AddMember("imageattr_y",setJsonParam(media.sdpinfo[i].imageattr_y,doc),allocate);
 objectValue.AddMember("play_file",setJsonParam(media.sdpinfo[i].play_file,doc),allocate);
 objectValue.AddMember("ssrc",setJsonParam(media.sdpinfo[i].ssrc,doc),allocate);
 objectValue.AddMember("cname",setJsonParam(media.sdpinfo[i].cname,doc),allocate);
 objectValue.AddMember("mslable",setJsonParam(media.sdpinfo[i].mslabel,doc),allocate);
 objectValue.AddMember("lable",setJsonParam(media.sdpinfo[i].label,doc),allocate);

 vArray.PushBack(objectValue,allocate);

 }
}

return vArray;

}
int xGateMGDispatcher::create_media_msg(xGateMgMsg *pMgMsg, string &jsonBuff, int &gatewayId)
{
 XGLOG_INFO("Enter xGateMGDispatcher::create_media_msg(%s)", m_serverType);
  Document doc ;
  StringBuffer s;
  Writer<StringBuffer> writer(s);
  MgMediaDetail &mediaDetail = pMgMsg->get_media_detail();

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
  doc["data"]["mgresource_id"] = setJsonParam(mediaDetail.mediaResource_id,doc);
  doc["data"]["call_type"] = setJsonParam(mediaDetail.call_type,doc);
  doc["data"]["call_id"] = setJsonParam(mediaDetail.call_id,doc);
  doc["data"]["gateway_id"] = setJsonParam( mediaDetail.gateway_id,doc);
  doc["data"]["context_id"] = setJsonParam(mediaDetail.context_id,doc);
  doc["data"]["leg_id"] = setJsonParam(mediaDetail.leg_id,doc);
  doc["data"]["joiner_type"] =setJsonParam(mediaDetail.joiner_type,doc);
  doc["data"]["sig_type"] = setJsonParam(mediaDetail.sig_type,doc);
  doc["data"]["call_dir"]= setJsonParam(mediaDetail.call_dir,doc);
  doc["data"]["media_state"] = setJsonParam(mediaDetail.pb_state,doc);
  doc["data"]["rtp_endpoint"]= setJsonParam(mediaDetail.rtp_ep,doc);
  doc["data"]["media_proto"] = setJsonParam(mediaDetail.media_proto,doc);
  doc["data"]["out_proto"] = setJsonParam(mediaDetail.out_proto,doc);
  doc["data"]["sdpinfo"] = setJsonParam(mediaDetail,doc);
 
  if(!mediaDetail.mg_ip.empty())
     gatewayId = xGateUtil::getFromMgConnectionMap(mediaDetail.mg_ip); 
else
     gatewayId = mediaDetail.gateway_id; 
 	
  doc.Accept(writer);
  //get the length of created message
  len = s.GetLength();
if(len > 999)
  doc["msg_len"] = len+1;
else 
 doc["msg_len"] = len;
  s.Clear();
  doc.Accept(writer);
  printf("\n***********************************\n");
  puts(s.GetString());
  
  jsonBuff = s.GetString();
  string delimiter("\r\n\r\n");
  jsonBuff.append(delimiter.c_str(), delimiter.length());
  printf("***********************************\n");
  XGLOG_ERROR( "xGateMGDispatcher::create_media_msg created media msg: %s", jsonBuff.c_str());

  XGLOG_ERROR( "xGateMGDispatcher::create_media_msg created media mg ip %s mg id", mediaDetail.mg_ip.c_str() ,gatewayId);
  return jsonBuff.length();
//return len;  
}


//TODO: Yoga, this function not yet completed fully
xGateRetVal xGateMGDispatcher::dispatch_msg(const char *data, int len, int mgId)
{
 XGLOG_INFO("Enter xGateMGDispatcher::dispatch_msg(%s)",m_serverType);
  xGateRetVal retval = EN_XGATE_STATUS_SUCCESS;

  int fd;
  //retval = xGateUtil::getFromMgConnectionMap(fd, mgId);
  retval = xGateUtil::getFromMgConnectionMap(mgId,fd);
 // string ipAddr("82.113.66.84");
  //string ipAddr("80.74.228.170");
  unsigned short port = 32779;
//if(mgId > -1)
//retval = xGateUtil::getFromMgConnectionMap(mgId,fd);
//else
//retval = xGateUtil::getFromMgConnectionMap(ipAddr, port, fd);

  if(retval != EN_XGATE_STATUS_SUCCESS) {
    XGLOG_ERROR( "xGateMGDispatcher::dispatch_msg(%s)"\
        " No connection Present for mgId:%d", m_serverType, mgId);
    return EN_XGATE_STATUS_ERROR;
  }

  retval = send_msg_on_tcp(fd, data, len);
  return retval;
}

xGateRetVal xGateMGDispatcher::send_msg_on_tcp(unsigned int fd, const char *pBuffer, int len)
{
 XGLOG_INFO("Enter xGateMGDispatcher::send_msg_on_tcp(%s) FD:%d",m_serverType,fd);
  errno = 0;
  int retVal = send(fd, pBuffer, len, MSG_DONTWAIT);
  //TODO: Yoga, we may need send timeout mechanism and then retry logic here
  if (retVal == -1)
  {
    XGLOG_ERROR( "xGateMGDispatcher::send_msg_on_tcp(%s)" \
        " send failed Error:%s", m_serverType, strerror(errno));
    return EN_XGATE_STATUS_ERROR;
  }  

 XGLOG_INFO("xGateMGDispatcher::send_msg_on_tcp(%s)" \
      " Success Fd:%d Message:\n:%s", m_serverType, fd, pBuffer);
  return EN_XGATE_STATUS_SUCCESS;
}

