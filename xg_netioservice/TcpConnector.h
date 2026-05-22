#ifndef _XGATE_MG_CONNECTOR_H
#define _XGATE_MG_CONNECTOR_H

//Ace Includes
#include <ace/Reactor.h>
#include <ace/SOCK_Stream.h>
#include <ace/Task_T.h>
#include <ace/SOCK_Connector.h>

// Local Includes
#include "xGateUtil.h"
#include "Connector.h"

#define TCPCONNECTOR XGTcpConnector::instance()

  class TcpConnector : public Connector
  {			    
  
public:	  
    TcpConnector();   

    virtual ~TcpConnector (void);

    /// Interface to schedule the timer
    //xGateRetVal connect_to_peer(xGateTCPConnectionStr&); 
    xGateRetVal connect_to_peer(); 

    /// Called back on handle_time out
    int handle_timeout(const ACE_Time_Value&, const void *); 

    ACE_Task<ACE_MT_SYNCH>* getTask(void)
    {
      return m_taskPtr;
    }
    bool add_new_client(string ip,int port,int ModuleId);

  private:

    /**
     * Private copy constructor - disallow copies and automatic methods.
     **/
    TcpConnector(const TcpConnector& rhs);

    /**
     * Private assignment operator - disallow assignment and automatic methods.
     **/
    TcpConnector& operator= (const TcpConnector& rhs);

    /// Interface to make the connections
    xGateRetVal make_connection();
    xGateRetVal send_client_connected(unsigned int,int,string,int);
    bool remove_entry_from_conn_moduleId_map(string & m_connDetails);

    /// Timer Interval in Sec's
    //int  m_timerCount;


    /// Lock For Connector Timer
    //    ACE_Thread_Mutex  m_conTimerLock;
    ACE_Task<ACE_MT_SYNCH>* m_taskPtr;

    ACE_Recursive_Thread_Mutex m_tMtxConnectionList;
    map<string,int> m_conn_moduleId;
    bool m_isConnectionPending;
    //xGateRetVal post_mg_register_msg(); 

};
typedef ACE_Unmanaged_Singleton<TcpConnector, ACE_Null_Mutex> XGTcpConnector;
#endif
