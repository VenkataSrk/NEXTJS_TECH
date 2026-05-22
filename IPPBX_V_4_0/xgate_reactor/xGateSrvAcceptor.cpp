
//local includes
#include "xglog.h"
#include "xGateUtil.h"
#include "xGateTCPReceiver.h"
#include "xGateSrvAcceptor.h"

#define THISMODULE "SrvAccep"

xGateSrvAcceptor::xGateSrvAcceptor (void):
  xGateTCPServer("SrvAcceptor")
{
 XGLOG_WARN( "xGateSrvAcceptor::xGateSrvAcceptor");
}

xGateSrvAcceptor::~xGateSrvAcceptor (void)
{
 XGLOG_WARN( "xGateSrvAcceptor::~xGateSrvAcceptor");
}

int xGateSrvAcceptor::handle_connection(ACE_HANDLE aceHandle)
{
 XGLOG_WARN( "Enter xGateSrvAcceptor::handle_connection Fd:%d",aceHandle);

  ACE_SOCK_Stream newPeerStream;
  unsigned int index= 0;

  for(index=0;index<m_acceptorList.size();index++)
  {
    if(aceHandle == m_acceptorList[index]->get_handle())
    {
      break;
    }
  }
  if(index >= m_acceptorList.size())
  {
   XGLOG_ERROR( "xGateSrvAcceptor::handle_connection Received"\
        " connection handle is invalid :%d", aceHandle);
    return 0;
  }
  m_acceptorList[index]->accept(newPeerStream);

  ACE_INET_Addr remoteAddr;
  newPeerStream.get_remote_addr(remoteAddr);
  xGateTCPReceiver *pReceiver = NULL; 
  string serverType("");

  if(MGC_MG_SIGNAL_PORT == remoteAddr.get_port_number()) {
    serverType = "MG-MGC";
    pReceiver = xGateUtil::getMgReceiver();
   XGLOG_WARN( "xGateSrvAcceptor::handleConnection registering" \
        " Connection with Mg-Receiver");
  }

  if(pReceiver == NULL || pReceiver->get_reactor() == NULL)
  {
   XGLOG_ERROR( "xGateSrvAcceptor::handleConnection Failed" \
        " to get the TCPReceiver");
    newPeerStream.close();
    return 0;
  }

  /// Create the Connection handler
  /// Register the connection with appropriate Receiver
  xGateTCPSocketHandler* eventHandler =
    new xGateTCPSocketHandler((char*)serverType.c_str(),pReceiver->get_reactor(),pReceiver->get_task());

  //TODO: check evenHandler pointer

  //Initialize the new eventHandler object with the handle to the socket stream
  if(eventHandler->init(newPeerStream) != EN_XGATE_STATUS_SUCCESS)
  {
   XGLOG_ERROR( "xGateSrvAcceptor::handleConnection Failed"\
        " to  Register the Connection handler");
    newPeerStream.close();
    delete eventHandler;
    return 0;
  }

 XGLOG_WARN( "Exit xGateSrvAcceptor::handle_connection sucessfully"\
      " handled Fd:%d",aceHandle);
  return 0;
}


