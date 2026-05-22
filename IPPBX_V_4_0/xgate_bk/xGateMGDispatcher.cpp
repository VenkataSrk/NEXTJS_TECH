
//local includes
#include "log.h"
#include "klog.h"
#include "xGateMGDispatcher.h"
#include "xGateMgMsg.h"
#include "xGateUtil.h"
#define THISMODULE "MGDispatcher"

xGateMGDispatcher::xGateMGDispatcher(const char* serverType) : xGateDispatcher(serverType)
{
  memset(m_serverType,0,SERVER_TYPE_SIZE);
  strcpy(m_serverType,serverType);
  memset(m_peerAddress,0,SERVER_ADDRESS_LENGTH);
  CLog::Detail(THISMODULE, "xGateMGDispatcher::xGateMGDispatcher(%s)", m_serverType);
}

xGateMGDispatcher::~xGateMGDispatcher(void)
{
  CLog::Detail(THISMODULE, "xGateMGDispatcher::~xGateMGDispatcher(%s)", m_serverType);
}

int xGateMGDispatcher::svc(void)
{
  CLog::Detail(THISMODULE, "xGateMGDispatcher::svc(%s)", m_serverType);
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
      CLog::Detail(THISMODULE, "xGateMGDispatcher::svc(%s) Error in Message Queue", m_serverType);
      m_run = false;
    }

    // delete the message received
    if(pAmb)
    {
      delete pAmb;
      pAmb = NULL;
    }
  }
  CLog::Detail(THISMODULE, "xGateMGDispatcher::svc(%s) Stopped Dispatch Task Thread", m_serverType);
  return 0;
}

//This function handle msg request from sofia.cpp(main thread) and post to MGDispatcher thread
xGateRetVal xGateMGDispatcher::process_msg(ACE_Message_Block *pAmb)
{
  CLog::Detail(THISMODULE, "xGateMGDispatcher::process_msg(%s)", m_serverType);
  xGateRetVal retval = EN_XGATE_STATUS_SUCCESS;

  if(pAmb != NULL)
  {
    xGateMgMsg* pMgMsg = NULL;
    pMgMsg = dynamic_cast<xGateMgMsg*>(pAmb);
    if(pMgMsg  == NULL)
    {
      CLog::Detail(THISMODULE, "xGateMGDispatcher::process_msg recvd invalid xGateMgMsg Message");
    }

    if(encode_msg(pMgMsg) != EN_XGATE_STATUS_SUCCESS) {
      CLog::Detail(THISMODULE, "xGateMGDispatcher::process_msg not able to encode MG message");
    }

  }
  return retval;
}

xGateRetVal xGateMGDispatcher::encode_msg(xGateMgMsg* pMgMsg)
{
  CLog::Detail(THISMODULE, "Enter xGateMGDispatcher::encode_msg(%s)", m_serverType);
  xGateMgMsgType mgMsgType = pMgMsg->get_mg_msg_type();
  xGateNetConnection netConInfo; 
  pMgMsg->get_net_con_info(netConInfo);
  int len = 0;

  //EN_XGATE_MG_REGISTER, we are getting direct json data from xGateMGCConnector thread
  if(mgMsgType == EN_XGATE_MG_REGISTER || mgMsgType == EN_XGATE_MG_PLAY_BACK) {
    CLog::Detail(THISMODULE, "xGateMGDispatcher::encode_msg failed EN_XGATE_MG_REGISTER || EN_XGATE_MG_PLAY_BACK");
    //TODO: we don't need this registration concept in new MGC-MG server
    //TODO: we don't need to replay for EN_XGATE_MG_PLAY_BACK 
    /*const char *data = 0;
    pMgMsg->get_data(data);
    len = pMgMsg->get_data_len();
    dispatch_msg(data, len, netConInfo);*/
    return EN_XGATE_STATUS_SUCCESS;
  }

  //Make jsonData for other messages
  string jsonBuff("");
  len = create_media_msg(pMgMsg, jsonBuff);
  if(len <= 0) {
    CLog::Error(THISMODULE, "xGateMGDispatcher::encode_msg create_media_response_msg failed");
    return EN_XGATE_STATUS_ERROR;
  }

  //dispatch json data
  if(dispatch_msg(jsonBuff.c_str(), len, netConInfo) != EN_XGATE_STATUS_SUCCESS) {
    CLog::Error(THISMODULE, "xGateMGDispatcher::encode_msg dispatch failed");
    return EN_XGATE_STATUS_ERROR;
  }

  return EN_XGATE_STATUS_SUCCESS;
}

int xGateMGDispatcher::create_media_msg(xGateMgMsg *pMgMsg, string &jsonBuff)
{
  /*CLog::Detail(THISMODULE, "Enter xGateMGDispatcher::create_media_msg(%s)", m_serverType);
  Document doc;
  StringBuffer s;
  Writer<StringBuffer> writer(s);
  MgMediaDetail &mediaDetail = pMgMsg->get_media_detail();

  int len = 0;
  len = sizeof(json_mg_media_tmpl);
  char buff[len];
  memcpy(buff, json_mg_media_tmpl, len);

  if (doc.ParseInsitu(buff).HasParseError()) {
    CLog::Error(THISMODULE, "xGateMGDispatcher::create_media_msg json parse error");
    return EN_XGATE_STATUS_ERROR;
  }
  if(!doc.IsObject()) {
    CLog::Error(THISMODULE, "xGateMGDispatcher::create_media_msg doc object creation failed");
    return EN_XGATE_STATUS_ERROR;
  }

  doc["msg_type"] = pMgMsg->get_mg_msg_type();
  doc["msg_len"] = 400;

  Value valCallId;
  string call_id = mediaDetail.call_id;
  valCallId.SetString(call_id.c_str(), call_id.length(), doc.GetAllocator());
  doc["data"]["call_id"] = valCallId;

  Value valContextId;
  valContextId = mediaDetail.context_id;
  doc["data"]["context_id"] = valContextId;

  Value valIpAddr;
  string ip_addr = mediaDetail.ip_addr;
  valIpAddr.SetString(ip_addr.c_str(), ip_addr.length(), doc.GetAllocator());
  doc["data"]["ip_addr"] = valIpAddr;

  Value valPort;
  u_short port = mediaDetail.port ;
  valPort = (int) port;
  doc["data"]["ip_port"] = valPort;

  Value valrelIpAddr;
  string relayip_addr = mediaDetail.relayip_addr;
  valrelIpAddr.SetString(relayip_addr.c_str(), relayip_addr.length(), doc.GetAllocator());
  doc["data"]["relayip_addr"] = valrelIpAddr;

  Value valrelPort;
  u_short relay_port = mediaDetail.relay_port ;
  valrelPort = (int) relay_port;
  doc["data"]["relay_port"] = valrelPort;

  Value valrefIpAddr;
  string reflexip_addr = mediaDetail.relayip_addr;
  valrefIpAddr.SetString(reflexip_addr.c_str(), reflexip_addr.length(), doc.GetAllocator());
  doc["data"]["reflexip_addr"] = valrefIpAddr;

  Value valrefPort;
  u_short reflex_port = mediaDetail.reflex_port ;
  valrefPort = (int) reflex_port;
  doc["data"]["reflex_port"] = valrefPort;
 
  Value valCodec;
  valCodec = mediaDetail.codec;
  doc["data"]["codec"] = valCodec;      

  Value valiceUfrag;
  string ice_ufrag = mediaDetail.ice_ufrag;
  valiceUfrag.SetString(ice_ufrag.c_str(), ice_ufrag.length(), doc.GetAllocator());
  doc["data"]["ice_ufrag"] = valiceUfrag;

  Value valicePwd;
  string ice_pwd = mediaDetail.ice_pwd;
  valicePwd.SetString(ice_pwd.c_str(), ice_pwd.length(), doc.GetAllocator());
  doc["data"]["ice_pwd"] = valicePwd;

  Value valFigprnt;
  string fingerprint = mediaDetail.fingerprint;
  valFigprnt.SetString(fingerprint.c_str(), fingerprint.length(), doc.GetAllocator());
  doc["data"]["fingerprint"] = valFigprnt;

  Value valpTime;
  u_short ptime = mediaDetail.ptime ;
  valpTime = (int) ptime;
  doc["data"]["ptime"] = valpTime;

  Value valMedprot;
  string media_proto = mediaDetail.media_proto;
  valMedprot.SetString(media_proto.c_str(), media_proto.length(), doc.GetAllocator());
  doc["data"]["media_proto"] = valMedprot;

  Value valOutprot;
  string out_proto = mediaDetail.out_proto;
  valOutprot.SetString(out_proto.c_str(), out_proto.length(), doc.GetAllocator());
  doc["data"]["out_proto"] = valOutprot;

  Value valSigtype;
  valSigtype = mediaDetail.sig_type;
  doc["data"]["sig_type"] = valSigtype;

  Value valDir;
  valDir = mediaDetail.call_dir;
  doc["data"]["call_dir"] = valDir;

  Value valRtpEp;
  string ep = mediaDetail.rtp_ep;
  valRtpEp.SetString(ep.c_str(), ep.length(), doc.GetAllocator());
  doc["data"]["rtp_endpoint"] = valRtpEp;

  doc.Accept(writer);
  //get the length of created message
  len = s.GetLength();

  doc["msg_len"] = len;
  s.Clear();
  doc.Accept(writer);
  printf("\n***********************************\n");
  puts(s.GetString());
  jsonBuff = s.GetString();
  string delimiter("\r\n\r\n");
  jsonBuff.append(delimiter.c_str(), delimiter.length());
  printf("***********************************\n");
  CLog::Error(THISMODULE, "xGateMGDispatcher::create_media_msg created media msg: %s", jsonBuff.c_str());
  return jsonBuff.length();
*/
return 0;
}

//TODO: Yoga, this function not yet completed fully
xGateRetVal xGateMGDispatcher::dispatch_msg(const char *data, int len, xGateNetConnection &netConInfo)
{
  CLog::Detail(THISMODULE, "Enter xGateMGDispatcher::dispatch_msg(%s)",m_serverType);
  xGateRetVal retval = EN_XGATE_STATUS_SUCCESS;

  int fd;
  unsigned int index;
  //for(index=0;index<xGateUtil::m_MgcIpAddressList.size();index++)
  //{
    //string ipAddress     = xGateUtil::m_MgcIpAddressList[index];
    string ipAddress     = netConInfo.recvIp_;
    unsigned short port  = 32779;
    unsigned short mgcPort = 32779;
    CLog::Detail(THISMODULE, "xGateMGDispatcher::dispatch_msg(%s)" \
        " Sending MgMsg to TCP Socket Address %s:%d from MGC port: %d",m_serverType,
        ipAddress.c_str(), port, mgcPort);

    retval = xGateUtil::getFromMgConnectionMap(ipAddress,port,fd);
    if(retval != EN_XGATE_STATUS_SUCCESS) {
      CLog::Error(THISMODULE, "xGateMGDispatcher::dispatch_msg(%s)"\
          " No connection Present for the address:%s:%d", m_serverType, ipAddress.c_str(), port);
      return EN_XGATE_STATUS_ERROR;
    }
    retval = send_msg_on_tcp(fd, data, len);
    return retval;
  //}
}

xGateRetVal xGateMGDispatcher::send_msg_on_tcp(unsigned int fd, const char *pBuffer, int len)
{
  CLog::Detail(THISMODULE, "Enter xGateMGDispatcher::send_msg_on_tcp(%s) FD:%d",m_serverType,fd);
  errno = 0;
  int retVal = send(fd, pBuffer, len, MSG_DONTWAIT);
  //TODO: Yoga, we may need send timeout mechanism and then retry logic here
  if (retVal == -1)
  {
    CLog::Error(THISMODULE, "xGateMGDispatcher::send_msg_on_tcp(%s)" \
        " send failed Error:%s", m_serverType, strerror(errno));
    return EN_XGATE_STATUS_ERROR;
  }  

  CLog::Detail(THISMODULE, "xGateMGDispatcher::send_msg_on_tcp(%s)" \
      " Success Fd:%d Message:\n:%s", m_serverType, fd, pBuffer);
  return EN_XGATE_STATUS_SUCCESS;
}
