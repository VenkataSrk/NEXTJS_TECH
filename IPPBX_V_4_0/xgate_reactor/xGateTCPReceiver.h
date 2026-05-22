#ifndef _XGATE_TCP_RECEIVER_H
#define _XGATE_TCP_RECEIVER_H

//ace includes
#include <ace/Reactor.h>
#include <ace/Svc_Handler.h>
#include <ace/Synch.h>
#include <ace/OS.h>
#include <ace/Task.h>

//local includes
#include "xGateUtil.h"
#include "xGateTCPSocketHandler.h"

class xGateTCPReceiver : public ACE_Event_Handler
{
  public:

    xGateTCPReceiver (const char*, ACE_Task<ACE_MT_SYNCH>*);

    virtual ~xGateTCPReceiver (void);

    xGateRetVal init (void);

    static void run (void* arguments);

    xGateRetVal stop_thread( void );

    ACE_Reactor * get_reactor(void)
    {
      return m_reactor;
    }

    ACE_Task<ACE_MT_SYNCH>* get_task(void)
    {
      return m_taskPtr;
    }

    int handle_timeout(const ACE_Time_Value&, const void *arg);

    /// Map to store FD to SocketHandler Map
    void addFdToSocketHandlerMap(int ,xGateTCPSocketHandler * const);
    xGateRetVal getHandlerFromFdToSocketHandlerMap(int ,xGateTCPSocketHandler *&);
    void removeFromFdToSocketHandlerMap(int);

  private:

    /**
     * Private copy constructor - disallow copies and automatic methods.
     **/
    xGateTCPReceiver(const xGateTCPReceiver& rhs);

    /**
     * Private assignment operator - disallow assignment and automatic methods.
     **/
    xGateTCPReceiver& operator= (const xGateTCPReceiver& rhs);

    /**
     * The Reactor instance
     **/
    ACE_Reactor *m_reactor;

    /** Boolean Flag
     * True  - The Thread is running
     * False - The Thread is stopped.
     */
    bool m_running;

    char m_serverType[SERVER_TYPE_SIZE];

    map<int, xGateTCPSocketHandler*>m_fdToSocketHandlerMap; //TODO: need to implement
    ACE_Task<ACE_MT_SYNCH>* m_taskPtr;

};


#endif
