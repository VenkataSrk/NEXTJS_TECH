
//local includes
#include "Connector.h"
#include "TcpSocketHandler.h"
#include "xglog.h"
#include "NetIOServiceDefine.h"
#include "xGateNetIOServiceMsg.h"
#include "xGateNetIOService.h"
#include "xGateUtil.h"

//self includes
#include "TcpConnector.h"

/**
 * Default Constructor
 */
TcpConnector::TcpConnector(): 
  Connector("MG-Conn"),
  m_taskPtr(nullptr),m_isConnectionPending(false)
{ 
  XGLOG_INFO( "TcpConnector::TcpConnector");
}//end default constructor

/**
 * Destructor
 */
TcpConnector::~TcpConnector (void)
{
  XGLOG_INFO( "TcpConnector::~Tcponnector"); 
}//end destructor 


bool TcpConnector::add_new_client(string ip,int port,int moduleId)
{
  XGLOG_INFO( "Inside TcpConnector::add_new_client"); 
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
  XGLOG_INFO( "========> TcpConnector::add_new_client trigerring EN_XGATE_TIMER_RECONNECT timer <=========");
  connect_to_peer();
 }
#endif
 conn_addr.clear ();
 ip.clear ();
 return true;
} 
//xGateRetVal TcpConnector::connect_to_peer(xGateTCPConnectionStr &conStr)

xGateRetVal TcpConnector::connect_to_peer()
{
  xGateSocketTimerData *pData = new xGateSocketTimerData();
  pData->eTimerType = EN_XGATE_TIMER_RECONNECT; 
  long int timerId = m_reactor->schedule_timer(this, (void*)pData, ACE_Time_Value(0,0));
  XGLOG_INFO( "TcpConnector::connectToPeer scheduled timer Timer Id :%ld", timerId);
  return EN_XGATE_STATUS_SUCCESS; 
}

/// Call back method on Timer Expiry
int TcpConnector::handle_timeout(const ACE_Time_Value&, const void *arg)
{
  XGLOG_INFO( "TcpConnector::handle_timeout");
  xGateSocketTimerData *pTimerData = (xGateSocketTimerData*)arg;
  if (!pTimerData)
  {
    XGLOG_ERROR( "TcpConnector::handle_timeout "\
        "Timerdata not present");
    return 0;
  }

  if (pTimerData->eTimerType == EN_XGATE_TIMER_TIMEOUT)
  {
    int fd = pTimerData->fd;
    TcpSocketHandler *pHandler = nullptr;
    if (EN_XGATE_STATUS_SUCCESS == getHandlerFromFdToSocketHandlerMap(fd, pHandler))
    {
      if (pHandler)
      {
        XGLOG_INFO( "TcpConnector::handle_timeout "\
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
      xGateSocketTimerData *pData = new xGateSocketTimerData();
      pData->eTimerType = EN_XGATE_TIMER_RECONNECT; 
      /// If any entry does not get a connection reschedule the timer
      m_reactor->schedule_timer(this, (void*)pData, ACE_Time_Value (m_timerInterval, 0)); 
      XGLOG_WARN( "TcpConnector::handle_timeout "\
          "scheduled timer for MGC connection");
    } else { 
      XGLOG_INFO( "TcpConnector::handle_timeout "\
          "successfully connected MGC server !!");
      m_isConnectionPending = false;
#if 0     
      if(post_mg_register_msg() != EN_XGATE_STATUS_SUCCESS) {
        XGLOG_ERROR( "TcpConnector::handle_timeout "\
            "posting register messsage failed !!");
      }
#endif
    }
  }
  delete pTimerData;
  return 0;
}

xGateRetVal TcpConnector::send_client_connected(unsigned int connectId,int dstModuleId,string Ip,int port)
{
#if 1
	xGateNetIOServiceMsg *pMsg = new xGateNetIOServiceMsg();

	xGateNetConnection netConnInfo;
	netConnInfo.m_IpAdd = Ip;
	netConnInfo.m_Port = port;
  pMsg->setConnectionId(connectId);
	pMsg->set_netConnInfo(netConnInfo);
  pMsg->setNetIOMsgType(EN_XGATE_MSG_CLIENT_CONNECTED);
	pMsg->setMsgType(EN_XGATE_MSG_NETIO_MODULE);
	pMsg->setSrcModuleId(IURDefines::UR_MODULE_NETIO_SERVICE);
  pMsg->setDstModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
  
  XGLOG_INFO("TcpConnector::send_client_connected client connected info connid:(%d),IP:(%s)",connectId,Ip.c_str());
	NETIOSERVICE->getConfig()->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(pMsg));
#endif
	return EN_XGATE_STATUS_SUCCESS;
}	

/// Make TCP Connection to list 
xGateRetVal TcpConnector::make_connection()
{
  XGLOG_INFO( "Enter TcpConnector::make_connection"); 

  /// If the Connection is successfull store the handler in the corresponding
  /// Receivers 
  string activeLocalIP = xGateUtil::getLocalIpAddr(AF_INET,EN_XGATE_INTERFACE_PRIVATE);
  unsigned int index= 0;
  unsigned int connectedCount = 0;
  ACE_Read_Guard<ACE_Recursive_Thread_Mutex> m_read_lock(m_tMtxConnectionList,true); 	
  for(index=0;index<m_ConnectorList.size();index++) 
  {
    string activeIP=m_ConnectorList[index].m_ip;
    int port = m_ConnectorList[index].m_port;

    /// Populate conAddress String
    bool isConnPresent = false;

    XGLOG_INFO( "TcpConnector::make_connection IP:%s, Port:%d",activeIP.c_str(),port); 
    /// Check if the connection is already Present
    isConnPresent = NETIOUTIL->isPresentInMgConnectionMap(activeIP,port);

    if(isConnPresent) /// Connection is already Present
    {
      XGLOG_INFO( "TcpConnector::make_connection Connection Already Present to Address :%s" ,activeIP.c_str());
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
    snprintf(conLocalAddress, SERVER_ADDRESS_LENGTH,"%s:%d",activeLocalIP.c_str(),port);

    ACE_INET_Addr     remAddr(conAddress);
    ACE_INET_Addr     localAddr(conLocalAddress);
    ACE_SOCK_Connector      connector;
    ACE_SOCK_Stream         socketStream;

    errno = 0;
    ACE_Time_Value tv(m_connectTimeOut, 0);
    int retCode = connector.connect(socketStream, remAddr);
    //    &tv, localAddr,1);
    //&tv, ACE_Addr::sap_any,1);

    if (retCode == -1)
    {
      XGLOG_ERROR( "TcpConnector::make_connection Connect failed to " \
          "destination :%s  errno: %d Error :%s",conAddress,errno,strerror(errno)); 
      socketStream.close();
      continue;
    }
    else
    {
      // Socket connected, now register with the approprate receiver for read event on this socket
      XGLOG_INFO( "TcpConnector::make_connection Connection is " \
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
       if(!NETIOUTIL->getConnectionId(connId)){
	       XGLOG_ERROR("Could not get Connection Id");
         continue;	 
       }	 
      XGLOG_INFO("TcpConnector::make_connection connection id (%d) and IP(%s)",connId,activeIP.c_str());      
      TcpSocketHandler *m_mediaTcpSocketHandler = new TcpSocketHandler(this->getReactor(),moduleId,connId);
      if(m_mediaTcpSocketHandler->init(socketStream,connId) != EN_XGATE_STATUS_SUCCESS)
      {
        XGLOG_ERROR( "TcpConnector::make_connection Failed " \
            " Register the Connection handler");
        socketStream.close();
        delete m_mediaTcpSocketHandler;
        return EN_XGATE_STATUS_ERROR;
      }
      send_client_connected(connId,moduleId,activeIP,port); 
      ++connectedCount;
    }  
  }  
  if(connectedCount < m_ConnectorList.size()) {
    //still some MGC connection is pending. so return ERROR will schedule the timer
    XGLOG_ERROR( "TcpConnector::make_connection Failed _ConnectorList.size:%d",m_ConnectorList.size()); 
    return EN_XGATE_STATUS_ERROR; 
  }
    XGLOG_ERROR( "TcpConnector::make_connection Failed connectedCount:%d ,m_ConnectorList.size:%d",connectedCount,m_ConnectorList.size()); 
  //return EN_XGATE_STATUS_SUCCESS;
  return EN_XGATE_STATUS_ERROR; 
}

bool TcpConnector::remove_entry_from_conn_moduleId_map(string & m_connDetails)
{
  if(m_connDetails.empty())
  {
    XGLOG_ERROR( "TcpConnector::remove_entry_from_conn_moduleId_map failed. m_connDetails is empty !");
    return false;
  }

  ACE_Write_Guard<ACE_Recursive_Thread_Mutex> m_write_lock(m_tMtxConnectionList,true);
  map<string,int>::iterator it = m_conn_moduleId.find(m_connDetails);
  if(it != m_conn_moduleId.end())
  {
     m_conn_moduleId.erase(it);
     XGLOG_INFO( "TcpConnector::remove_entry_from_conn_moduleId_map success for m_connDetails(%s) !",m_connDetails.c_str());
     return true;
  }

  return false;
}


