
//local includes
#include "xGateConnector.h"
#include "MediaTcpSocketHandler.h"
#include "xglog.h"
#include "MediaServiceDefine.h"
#include "xGateMediaServiceMsg.h"
#include "xGateMediaService.h"
#include "xGateUtil.h"

//self includes
#include "MediaTcpConnector.h"
#include "XGConfig.h"

#define THISMODULE "MGCConn"

/**
 * Default Constructor
 */
MediaTcpConnector::MediaTcpConnector(): 
  xGateConnector("MG-Conn"),
  m_taskPtr(NULL),m_isConnectionPending(false)
{ 
  XGLOG_INFO( "xGateMGConnector::xGateMGConnector");
}//end default constructor

/**
 * Destructor
 */
MediaTcpConnector::~MediaTcpConnector (void)
{
  XGLOG_INFO( "xGateMGConnector::~xGateMGConnector"); 
}//end destructor 


bool MediaTcpConnector::add_new_client(string ip,int port,int moduleId)
{
  XGLOG_INFO( "Inside MediaTcpConnector::add_new_client"); 
 TcpLisentAddr newAddress;
 newAddress.m_ip = ip;
 newAddress.m_port = port;
 string conn_addr = ip + ":" + std::to_string(port);
 ACE_Write_Guard<ACE_Recursive_Thread_Mutex> m_write_lock(m_tMtxConnectionList,true); 	
 m_ConnectorList.push_back(newAddress);
 m_conn_moduleId.insert({conn_addr,moduleId});
 //TODO:Trigger the RECONNECT timer if there is no pending connections
#if 1
 if(!m_isConnectionPending)
 {
  XGLOG_INFO( "========> MediaTcpConnector::add_new_client trigerring EN_XGATE_TIMER_RECONNECT timer <=========");
  connect_to_peer();
 }
#endif
 conn_addr.clear ();
 ip.clear ();
 return true;
} 
//xGateRetVal MediaTcpConnector::connect_to_peer(xGateTCPConnectionStr &conStr)

xGateRetVal MediaTcpConnector::connect_to_peer()
{
  xGateTimerData *pData = new xGateTimerData();
  pData->eTimerType = EN_XGATE_TIMER_RECONNECT; 
  long int timerId = m_reactor->schedule_timer(this, (void*)pData, ACE_Time_Value(0,0));
  XGLOG_INFO( "xGateMGConnector::connectToPeer scheduled timer Timer Id :%ld", timerId);
  return EN_XGATE_STATUS_SUCCESS; 
}

/// Call back method on Timer Expiry
int MediaTcpConnector::handle_timeout(const ACE_Time_Value&, const void *arg)
{
  XGLOG_INFO( "xGateMGConnector::handle_timeout");
  xGateTimerData *pTimerData = (xGateTimerData*)arg;
  if (!pTimerData)
  {
    XGLOG_ERROR( "xGateMGConnector::handle_timeout "\
        "Timerdata not present");
    return 0;
  }

  if (pTimerData->eTimerType == EN_XGATE_TIMER_TIMEOUT)
  {
    int fd = pTimerData->fd;
    MediaTcpSocketHandler *pHandler = NULL;
    if (EN_XGATE_STATUS_SUCCESS == getHandlerFromFdToSocketHandlerMap(fd, pHandler))
    {
      if (pHandler)
      {
        XGLOG_INFO( "xGateMGConnector::handle_timeout "\
            "Fd obtained to close the handler");
        removeFromFdToSocketHandlerMap(fd);
        if (pHandler->getReactor())
        {
          pHandler->getReactor()->remove_handler(fd, ACE_Event_Handler::READ_MASK);
        }
      } 
    }
  }
  else
  {
    /// Make Connection
    xGateRetVal retVal = EN_XGATE_STATUS_ERROR;
    retVal  =  make_connection(); 
    if(retVal != EN_XGATE_STATUS_SUCCESS)
    {
      m_isConnectionPending = true;
      xGateTimerData *pData = new xGateTimerData();
      pData->eTimerType = EN_XGATE_TIMER_RECONNECT; 
      /// If any entry does not get a connection reschedule the timer
      m_reactor->schedule_timer(this, (void*)pData,
                 ACE_Time_Value (0, TCP_RE_CONNECTOR_TIME_OUT_MS * 1000));
      XGLOG_WARN( "xGateMGConnector::handle_timeout "\
          "scheduled timer for MGC connection");
    } else { 
      XGLOG_INFO( "xGateMGConnector::handle_timeout "\
          "successfully connected MGC server !!");
      m_isConnectionPending = false;
#if 0     
      if(post_mg_register_msg() != EN_XGATE_STATUS_SUCCESS) {
        XGLOG_ERROR( "xGateMGConnector::handle_timeout "\
            "posting register messsage failed !!");
      }
#endif
    }
  }
  delete pTimerData;
  return 0;
}

xGateRetVal MediaTcpConnector::send_client_connected(unsigned int connectId,int dstModuleId,
                                                     string Ip,int port,xGateMSMsgType ClientType)
{
 xGateMediaServiceMsg *pMsg = new xGateMediaServiceMsg();
 xGateNetConnection netConnInfo;
 pMsg->setMsgType((xGateMsgType)ClientType);
 pMsg->setSrcModuleId(IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE);
 pMsg->setDstModuleId((IURDefines::MODULE_ID)dstModuleId);
 pMsg->setConnectionId(connectId);
 netConnInfo.recvIp_ = Ip;
 netConnInfo.recvPort_ = port;
 pMsg->set_netConnInfo(netConnInfo);
 MEDIASERVICE->getConfig()->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(pMsg));
 return EN_XGATE_STATUS_SUCCESS;
}	

/// Make TCP Connection to list 
xGateRetVal MediaTcpConnector::make_connection()
{
  XGLOG_INFO( "Enter xGateMGConnector::make_connection"); 

  /// If the Connection is successfull store the handler in the corresponding
  /// Receivers 
  //string activeLocalIP = xGateUtil::getLocalIpAddr();
  string activeLocalIP = CONFIGSERVICE->m_localIp4Addr;
  unsigned int index= 0;
  unsigned int connectedCount = 0;
  ACE_Read_Guard<ACE_Recursive_Thread_Mutex> m_read_lock(m_tMtxConnectionList,true);
  xGateMSMsgType ClientType = EN_XGATE_MSG_CLIENT_CONNECTED;
  
  for(index=0;index<m_ConnectorList.size();index++) 
  {
    string activeIP=m_ConnectorList[index].m_ip;
    int port = m_ConnectorList[index].m_port;

    /// Populate conAddress String
    bool isConnPresent = false;

    XGLOG_INFO( "xGateMGConnector::make_connection IP:%s, Port:%d",activeIP.c_str(),port); 
    /// Check if the connection is already Present
    isConnPresent = MEDIAUTIL->isPresentInMgConnectionMap(activeIP,port);

    if(isConnPresent) /// Connection is already Present
    {
      XGLOG_INFO( "xGateMGConnector::make_connection Connection Already Present to Address :%s" ,activeIP.c_str());
      ++connectedCount;
      if(connectedCount < m_ConnectorList.size()) {
        continue;
      }
      return EN_XGATE_STATUS_SUCCESS;
    } 

    char conAddress[SERVER_ADDRESS_LENGTH];
    char conLocalAddress[SERVER_ADDRESS_LENGTH];
    memset(conAddress,0, SERVER_ADDRESS_LENGTH);
    memset(conLocalAddress,0, SERVER_ADDRESS_LENGTH);
    snprintf(conAddress, SERVER_ADDRESS_LENGTH,"%s:%d",activeIP.c_str(),port );
    //snprintf(conLocalAddress, SERVER_ADDRESS_LENGTH,"%s:%d",activeLocalIP.c_str(),port);
    snprintf(conLocalAddress, SERVER_ADDRESS_LENGTH,"%s",activeLocalIP.c_str());

    ACE_INET_Addr     remAddr(conAddress);
    ACE_INET_Addr     localAddr(conLocalAddress);
    ACE_SOCK_Connector      connector;
    ACE_SOCK_Stream         socketStream;

    errno = 0;
    ACE_Time_Value tv(m_connectTimeOut, 0);

    int retCode = connector.connect(socketStream, remAddr
        ,&tv, localAddr,1);
    //&tv, ACE_Addr::sap_any,1);

    if (retCode == -1)
    {
      XGLOG_ERROR( "xGateMGConnector::make_connection Connect failed to " \
          "destination :%s  errno: %d Error :%s",conAddress,errno,strerror(errno)); 
      socketStream.close();
      continue;
    }
    else
    {
      // Socket connected, now register with the approprate receiver for read event on this socket
      XGLOG_INFO( "xGateMGConnector::make_connection Connection is " \
          "success to :%s",conAddress); 

      map<string,int>::iterator it;
      int moduleId = 0;
      string m_connDetails = activeIP + ":" + std::to_string(port);

      if((it = m_conn_moduleId.find(m_connDetails)) != m_conn_moduleId.end()){
	 moduleId = it->second;   
         //TODO:Erase entry from m_conn_moduleId map
         //remove_entry_from_conn_moduleId_map(m_connDetails);
       }
       else
       {
          XGLOG_ERROR("Could not find srcModuleId in map");
          continue;   	  
       }
       unsigned int connId = 0;
       if(!MEDIAUTIL->getConnectionId(connId)){
	 XGLOG_ERROR("Could not get Connection Id");
         continue;	 
       }	       
      MediaTcpSocketHandler *m_mediaTcpSocketHandler = new MediaTcpSocketHandler(this->getReactor(),moduleId,connId);
      if(m_mediaTcpSocketHandler->init(socketStream) != EN_XGATE_STATUS_SUCCESS)
      {
        XGLOG_ERROR( "xGateMGConnector::make_connection Failed " \
            " Register the Connection handler");
        socketStream.close();
        delete m_mediaTcpSocketHandler;
        return EN_XGATE_STATUS_ERROR;
      }
      if(port == REC_CONNECT_PORT)
        ClientType = EN_XGATE_MSG_REC_CLIENT_CONNECTED;
      send_client_connected(connId,moduleId,activeIP,port,ClientType); 
      ++connectedCount;
    }  
  }  
  if(connectedCount < m_ConnectorList.size()) {
    //still some MGC connection is pending. so return ERROR will schedule the timer
    XGLOG_ERROR( "xGateMGConnector::make_connection Failed _ConnectorList.size:%d",m_ConnectorList.size()); 
    return EN_XGATE_STATUS_ERROR; 
  }
    XGLOG_ERROR( "xGateMGConnector::make_connection Failed connectedCount:%d ,m_ConnectorList.size:%d",connectedCount,m_ConnectorList.size()); 
  //return EN_XGATE_STATUS_SUCCESS;
  return EN_XGATE_STATUS_ERROR; 
}

bool MediaTcpConnector::remove_entry_from_conn_moduleId_map(string & m_connDetails)
{
  if(m_connDetails.empty())
  {
    XGLOG_ERROR( "MediaTcpConnector::remove_entry_from_conn_moduleId_map failed. m_connDetails is empty !");
    return false;
  }

  ACE_Write_Guard<ACE_Recursive_Thread_Mutex> m_write_lock(m_tMtxConnectionList,true);
  map<string,int>::iterator it = m_conn_moduleId.find(m_connDetails);
  if(it != m_conn_moduleId.end())
  {
     m_conn_moduleId.erase(it);
     XGLOG_INFO( "MediaTcpConnector::remove_entry_from_conn_moduleId_map success for m_connDetails(%s) !",m_connDetails.c_str());
     return true;
  }

  return false;
}

#if 0
xGateRetVal MediaTcpConnector::post_mg_register_msg()
{
  XGLOG_INFO( "Enter MediaTcpConnector::post_mg_register_msg");

  ACE_Task<ACE_MT_SYNCH>* taskPtr = NULL;
  taskPtr = xGateUtil::getMgDispatcher();
  if(!taskPtr) {
    XGLOG_ERROR( "MediaTcpConnector::post_mg_register_msg MgDispatcher task not found");
    return EN_XGATE_STATUS_ERROR;
  }

  xGateMgMsg* pMgMsg = NULL;
  pMgMsg = new xGateMgMsg();
  if(!pMgMsg) {
    XGLOG_ERROR( "MediaTcpConnector::post_mg_register_msg xGateMgMsg initiation failed");
    return EN_XGATE_STATUS_ERROR;
  }

  pMgMsg->setMsgType(EN_XGATE_MSG_MGC_MG_OUT);
  pMgMsg->set_mg_msg_type(EN_XGATE_MG_REGISTER);

  int msgLen = 0;
  msgLen = sizeof(json_mg_register_tmpl);

  if(msgLen <= 0) {
    XGLOG_ERROR( "MediaTcpConnector::post_mg_register_msg json_mg_register_tmpl template length problem");
    return EN_XGATE_STATUS_ERROR;
  }

  Document doc;
  StringBuffer s;
  Writer<StringBuffer> writer(s);
  char buffer[msgLen];
  string temp = "";
  memcpy(buffer, json_mg_register_tmpl, msgLen);

  if (doc.ParseInsitu(buffer).HasParseError()) {
    XGLOG_ERROR( "MediaTcpConnector::post_mg_register_msg json parse error");
    return 0;
  }
  if(!doc.IsObject()) {
    XGLOG_ERROR( "MediaTcpConnector::post_mg_register_msg dom creation failed");
    return 0;
  }

  doc["msg_type"] = EN_XGATE_MG_REGISTER;
  doc["msg_len"] = 500;

  Value valGwName;
  temp = "MG"; //TODO: Yoga, it should unique across all MG server
  valGwName.SetString(temp.c_str(), temp.length(), doc.GetAllocator());
  doc["data"]["gw_name"] = valGwName;

  Value valGwProfile;
  temp = "rtp0:prx00;rtp1:prx01"; //TODO: Yoga, why we need this hardcoded value?
  valGwProfile.SetString(temp.c_str(), temp.length(), doc.GetAllocator());
  doc["data"]["gw_profile"] = valGwProfile;

  Value valChannel;
  valChannel = 10;
  doc["data"]["channels_count"] = valChannel;

  temp = "";
  int rlsCount = 0;
  RtpInterfaceManager::Instance()->GetResourceRecordList(temp, rlsCount);
  Value valRlsCount;
  valRlsCount = rlsCount;
  doc["data"]["rls_count"] = valRlsCount;

  Value valRlsRecord;
  valRlsRecord.SetString(temp.c_str(), temp.length(), doc.GetAllocator());
  doc["data"]["rls_record"] = valRlsRecord;

  doc.Accept(writer);
  //puts(s.GetString()); //this is only for debuggin purpose

  char *data = s.GetString();
  pMgMsg->set_data(data);
  pMgMsg->set_data_len(s.GetLength());

  ACE_Message_Block* pAmb = NULL;
  pAmb = static_cast<xGateMgMsg*> (pMgMsg);
  ACE_Time_Value tval ((time(NULL) ));
  tval += ACE_Time_Value(0,1);

  if (taskPtr->putq(pAmb, &tval)) {
    XGLOG_INFO( "MediaTcpConnector::post_mg_register_msg MgDispatcher enqueue message success");
  } else {
    XGLOG_ERROR( "MediaTcpConnector::post_mg_register_msg MgDispatcher enqueue message failed");
    delete pMgMsg;
    return EN_XGATE_STATUS_ERROR;
  }

  return EN_XGATE_STATUS_SUCCESS;
}
#endif
