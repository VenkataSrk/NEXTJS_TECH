#ifndef _XGATE_CONNECTOR_H
#define _XGATE_CONNECTOR_H

//system includes

//ace includes
#include <ace/Event_Handler.h>
#include <ace/Acceptor.h>
#include <ace/SOCK_Acceptor.h>

//local includes
#include "xGateUtil.h"
#include "MediaTcpSocketHandler.h"

class xGateConnector : public ACE_Event_Handler
{
  public:

    /**
     *  Default Constructor
     */
    xGateConnector (const char*);

    /**
     * Destructor
     */
    virtual ~xGateConnector (void);

    /**
     * Initialize the SipProxyConnector
     */
   // xGateRetVal init (void);
    xGateRetVal init ( );

    /// Returns the Reactor pointer
    ACE_Reactor * getReactor(void)
    {
      return m_reactor;
    }

    /**
     * Start the event loop for processing messages/notifications. This method
     * must be static in order to start event loop processing in a newly spawned thread.
     * Also, this is an infinitely blocking call.
     */
    static void run (void* arguments);

    /**
     *  stopThread( void );
     */ 
    xGateRetVal stopThread( void );

    /// Interface to schedule the timer
    virtual  xGateRetVal connectToPeer(xGateTCPConnectionStr&); 

    /// Map to store FD to SocketHandler Map
    void addFdToSocketHandlerMap(int ,MediaTcpSocketHandler * const);
    xGateRetVal getHandlerFromFdToSocketHandlerMap(int ,MediaTcpSocketHandler *&); 
    void removeFromFdToSocketHandlerMap(int);

  private:

    /**
     * Private copy constructor - disallow copies and automatic methods.
     **/
    xGateConnector(const xGateConnector& rhs);

    /**
     * Private assignment operator - disallow assignment and automatic methods.
     **/
    xGateConnector& operator= (const xGateConnector& rhs);


    /** Boolean Flag
     * True  - The Thread is running
     * False - The Thread is stopped.
     */
    bool m_running;

    /// Char Server Type
    char m_serverType[SERVER_TYPE_SIZE];

  protected: 
    /**
     * The Reactor instance 
     **/
    ACE_Reactor *m_reactor;

    /// Timer Interval in Sec's
    int  m_timerInterval;
    vector<TcpLisentAddr> m_ConnectorList;
    /// Connection Time Out in Sec's    
    int  m_connectTimeOut;

    map<int, MediaTcpSocketHandler*>m_fdToSocketHandlerMap;

};

#endif

