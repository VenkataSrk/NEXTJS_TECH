
//local includes
#include "xGateConnector.h"
#include "xGateTCPSocketHandler.h"

//self includes
#include "xGateMGConnector.h"

#define THISMODULE "MGConn"

/**
 * Default Constructor
 */
xGateMGConnector::xGateMGConnector(ACE_Task<ACE_MT_SYNCH>* taskPtr): 
        xGateConnector("MG-Conn"),
        m_taskPtr(taskPtr)
{ 
  XGLOG_INFO( "xGateMGConnector::xGateMGConnector");
}//end default constructor

/**
 * Destructor
 */
xGateMGConnector::~xGateMGConnector (void)
{
  XGLOG_INFO( "MGConnector::~xGateMGConnector"); 
}//end destructor 

/**
 *  Start a Timer
 **/
xGateRetVal xGateMGConnector::connect_to_peer(xGateTCPConnectionStr &conStr) 
{
  XGLOG_INFO( "MGConnector::connectToPeer to connect to"\
      " Address %s", conStr.peerIPAddress.c_str()); 

  xGateTimerData *pData = new xGateTimerData();
  pData->eTimerType = EN_XGATE_TIMER_RECONNECT; 
  long int timerId = m_reactor->schedule_timer(this, (void*)pData, ACE_Time_Value(0,0));
  XGLOG_INFO( "MGConnector::connectToPeer scheduled timer Timer Id :%ld", timerId);
  return EN_XGATE_STATUS_SUCCESS; 
}

/// Call back method on Timer Expiry
int xGateMGConnector::handle_timeout(const ACE_Time_Value&, const void *arg)
{
  XGLOG_INFO( "MGConnector::handle_timeout");
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
        XGLOG_INFO( "MGConnector::handle_timeout "\
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
    } 
  }
  delete pTimerData;
  return 0;
}

/// Make TCP Connection to list 
xGateRetVal xGateMGConnector::make_connection()
{
  XGLOG_INFO( " xGateMGConnector::make_connection"); 

  /// If the Connection is successfull store the handler in the corresponding
  /// Receivers 
  string activeIP = "82.113.66.84"; //TODO: MG Ip make it as configurable

  /// Populate conAddress String
  bool isConnPresent = false;

  /// Check if the connection is already Present
  isConnPresent = xGateUtil::isPresentInMgConnectionMap(activeIP, MGC_MG_SIGNAL_PORT);

  if(isConnPresent) /// Connection is already Present
  {
    XGLOG_INFO( "MGConnector::make_connection Connection Already Present to Address :%s" ,activeIP.c_str());
    return EN_XGATE_STATUS_SUCCESS;
  } 

  char conAddress[SERVER_ADDRESS_LENGTH];
  memset(conAddress,0, SERVER_ADDRESS_LENGTH);
  snprintf(conAddress, SERVER_ADDRESS_LENGTH,"%s:%d",activeIP.c_str(), MGC_MG_SIGNAL_PORT);

  ACE_INET_Addr     remAddr(conAddress);
  ACE_SOCK_Connector      connector;
  ACE_SOCK_Stream         socketStream;

  errno = 0;
  ACE_Time_Value tv(m_connectTimeOut, 0);

  int retCode = connector.connect(socketStream, remAddr,
      &tv, ACE_Addr::sap_any,1);

  if (retCode == -1)
  {
   XGLOG_ERROR( "xGateMGConnector::make_connection Connect failed to" \
        "destination :%s  errno: %d Error :%s",conAddress,errno,strerror(errno)); 
    return EN_XGATE_STATUS_ERROR;
  }
  else
  {
    // Socket connected, now register with the approprate receiver for read event on this socket
    XGLOG_INFO( "MGConnector::make_connection Connection is " \
        " success to :%s",conAddress); 

    /// Create Connection Handler 
    /// Create a new socket handler object for encapsulating the socket
    xGateTCPSocketHandler* eventHandler =
      new xGateTCPSocketHandler("Mg-Conn", this->getReactor(), this->getTask());

    //Initialize the new eventHandler object with the handle to the socket stream
    if(eventHandler->init(socketStream) != EN_XGATE_STATUS_SUCCESS)
    {
     XGLOG_ERROR( "xGateMGConnector::make_connection Failed " \
          " Register the Connection handler");
      socketStream.close();
      return EN_XGATE_STATUS_ERROR;
    } 
    //eventHandler->setPeerFQDN(xGateUtil::getMgFQDN()); //TODO: now we don't want this functionality
    return EN_XGATE_STATUS_SUCCESS;
  } 
}


