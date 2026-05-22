
//local includes
#include "xGateConnector.h"
#include "xGateTCPSocketHandler.h"
#include "xGateMGDispatcher.h"
#include "xGateUtil.h"
#include "xglog.h"

//self includes
#include "xGateMGCConnector.h"
#include "xGateUtil.h"
#define THISMODULE "MGCConn"

/**
 * Default Constructor
 */
xGateMGCConnector::xGateMGCConnector(ACE_Task<ACE_MT_SYNCH>* taskPtr): 
  xGateConnector("MG-Conn"),
  m_taskPtr(taskPtr)
{ 
  XGLOG_INFO( "xGateMGConnector::xGateMGConnector");
}//end default constructor

/**
 * Destructor
 */
xGateMGCConnector::~xGateMGCConnector (void)
{
  XGLOG_INFO( "xGateMGConnector::~xGateMGConnector"); 
}//end destructor 

/**
 *  Start a Timer
 **/
//xGateRetVal xGateMGCConnector::connect_to_peer(xGateTCPConnectionStr &conStr)

xGateRetVal xGateMGCConnector::connect_to_peer()
{
  /*XGLOG_INFO( "xGateMGConnector::connectToPeer to connect to"\
    " Address %s", conStr.peerIPAddress.c_str()); */

  xGateTimerData *pData = new xGateTimerData();
  pData->eTimerType = EN_XGATE_TIMER_RECONNECT; 
  long int timerId = m_reactor->schedule_timer(this, (void*)pData, ACE_Time_Value(0,0));
  XGLOG_INFO( "xGateMGConnector::connectToPeer scheduled timer Timer Id :%ld", timerId);
  return EN_XGATE_STATUS_SUCCESS; 
}

/// Call back method on Timer Expiry
int xGateMGCConnector::handle_timeout(const ACE_Time_Value&, const void *arg)
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
    xGateTCPSocketHandler *pHandler = NULL;
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
      xGateTimerData *pData = new xGateTimerData();
      pData->eTimerType = EN_XGATE_TIMER_RECONNECT; 
      /// If any entry does not get a connection reschedule the timer
      m_reactor->schedule_timer(this, (void*)pData, ACE_Time_Value (m_timerInterval, 0)); 
      XGLOG_WARN( "xGateMGConnector::handle_timeout "\
          "scheduled timer for MGC connection");
    } else { 
      XGLOG_INFO( "xGateMGConnector::handle_timeout "\
          "successfully connected MGC server !!");
      if(post_mg_register_msg() != EN_XGATE_STATUS_SUCCESS) {
        XGLOG_ERROR( "xGateMGConnector::handle_timeout "\
            "posting register messsage failed !!");
      }
    }
  }
  delete pTimerData;
  return 0;
}

/// Make TCP Connection to list 
xGateRetVal xGateMGCConnector::make_connection()
{
  XGLOG_INFO( "Enter xGateMGConnector::make_connection"); 

  /// If the Connection is successfull store the handler in the corresponding
  /// Receivers 
  string activeLocalIP = xGateUtil::getLocalIpAddr();
  string activeLocalIPv6("");
  // use in6addr_any
  if (ACE::ipv6_enabled ()) {
    activeLocalIPv6 = xGateUtil::getLocalIpAddr(AF_INET6);
  }

  unsigned int index= 0;
  unsigned int connectedCount = 0;
  for(index=0;index<m_ConnectorList.size();index++) 
  {
    string activeIP=m_ConnectorList[index].ipAddress;

    /// Populate conAddress String
    bool isConnPresent = false;

    /// Check if the connection is already Present
    isConnPresent = xGateUtil::isPresentInMgConnectionMap(activeIP, MGC_MG_SIGNAL_PORT);

    if(isConnPresent) /// Connection is already Present
    {
      XGLOG_INFO( "xGateMGConnector::make_connection Connection Already Present to Address :%s" ,(char*)ur_log_string(activeIP));
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
	

    snprintf(conAddress, SERVER_ADDRESS_LENGTH,"%s",activeIP.c_str());
    snprintf(conLocalAddress, SERVER_ADDRESS_LENGTH,"%s",activeLocalIP.c_str());

    size_t found = activeIP.find("::");	
	
    if (found != string::npos ) {
      //snprintf(conLocalAddress, SERVER_ADDRESS_LENGTH,"%s:%d",activeLocalIPv6.c_str(), MGC_MG_SIGNAL_PORT);
      continue;
    }

    ACE_INET_Addr     remAddr(MGC_MG_SIGNAL_PORT,conAddress,PF_INET);
    ACE_INET_Addr     localAddr(0,conLocalAddress,PF_INET);
    ACE_SOCK_Connector      connector;
	
    ACE_SOCK_Stream         socketStream;

    errno = 0;
    ACE_Time_Value tv(m_connectTimeOut, 0);

    int retCode = connector.connect(socketStream, remAddr,
        &tv, localAddr);

    if (retCode == -1)
    {
      #include <errno.h>
      #include <string.h>
      //std::cout<<"Oh dear, something went wrong with read()! " << strerror(errno) <<std::endl ;
      XGLOG_ERROR( "xGateMGConnector::make_connection Connect failed to " \
          "destination :%s  errno: %d Error :%s",(char*)ur_log_string(conAddress),errno,(char*)ur_log_string(strerror(errno))); 
      socketStream.close();
      continue;
    }
    else
    {
      // Socket connected, now register with the approprate receiver for read event on this socket
      XGLOG_INFO( "xGateMGConnector::make_connection Connection is " \
          "success to :%s",(char*)ur_log_string(conAddress)); 

      xGateUtil::m_tcpHandler= new xGateTCPSocketHandler("MG-MGC", this->getReactor(), this->getTask());
      if(xGateUtil::m_tcpHandler->init(socketStream) != EN_XGATE_STATUS_SUCCESS)
      {
        XGLOG_ERROR( "xGateMGConnector::make_connection Failed " \
            " Register the Connection handler");
        socketStream.close();
        delete xGateUtil::m_tcpHandler;
        xGateUtil::m_tcpHandler = NULL;
        return EN_XGATE_STATUS_ERROR;
      } 
      ++connectedCount;
    }  
  }  
  if(connectedCount < m_ConnectorList.size()) {
    //still some MGC connection is pending. so return ERROR will schedule the timer
    return EN_XGATE_STATUS_ERROR; 
  }
  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateMGCConnector::post_mg_register_msg()
{
  XGLOG_INFO( "Enter xGateMGCConnector::post_mg_register_msg");

  ACE_Task<ACE_MT_SYNCH>* taskPtr = NULL;
  taskPtr = xGateUtil::getMgDispatcher();
  if(!taskPtr) {
    XGLOG_ERROR( "xGateMGCConnector::post_mg_register_msg MgDispatcher task not found");
    return EN_XGATE_STATUS_ERROR;
  }

  xGateMgMsg* pMgMsg = NULL;
  pMgMsg = new xGateMgMsg();
  if(!pMgMsg) {
    XGLOG_ERROR( "xGateMGCConnector::post_mg_register_msg xGateMgMsg initiation failed");
    return EN_XGATE_STATUS_ERROR;
  }

  pMgMsg->setMsgType(EN_XGATE_MSG_MGC_MG_OUT);
  pMgMsg->set_mg_msg_type(EN_XGATE_MG_REGISTER);

  int msgLen = 0;
  msgLen = sizeof(json_mg_register_tmpl);

  if(msgLen <= 0) {
    XGLOG_ERROR( "xGateMGCConnector::post_mg_register_msg json_mg_register_tmpl template length problem");
    return EN_XGATE_STATUS_ERROR;
  }

  Document doc;
  StringBuffer s;
  Writer<StringBuffer> writer(s);
  char buffer[msgLen];
  string temp = "";
  memcpy(buffer, json_mg_register_tmpl, msgLen);

  if (doc.ParseInsitu(buffer).HasParseError()) {
    XGLOG_ERROR( "xGateMGCConnector::post_mg_register_msg json parse error");
    return 0;
  }
  if(!doc.IsObject()) {
    XGLOG_ERROR( "xGateMGCConnector::post_mg_register_msg dom creation failed");
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
  //RtpInterfaceManager::Instance()->GetResourceRecordList(temp, rlsCount);
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
    XGLOG_INFO( "xGateMGCConnector::post_mg_register_msg MgDispatcher enqueue message success");
  } else {
    XGLOG_ERROR( "xGateMGCConnector::post_mg_register_msg MgDispatcher enqueue message failed");
    delete pMgMsg;
    return EN_XGATE_STATUS_ERROR;
  }

  return EN_XGATE_STATUS_SUCCESS;
}
