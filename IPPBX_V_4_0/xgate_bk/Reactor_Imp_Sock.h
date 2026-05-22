/* $Header: /lace/ace/Reactor_Imp_Sock.h 12    8/07/04 6:59p Bennylp $ */

#ifndef __ACE_SOCKET_REACTOR_H__
#define __ACE_SOCKET_REACTOR_H__


#ifndef WIN32
#include <netdb.h>
#endif // WIN32


#include "ace/OS.h"
#include "ace/Event_Handler.h"

class ACE_Event_Handler;

#ifdef WIN32
#if FD_SETSIZE < ACE_FD_SETSIZE
# error "FD_SETSIZE probably is too small"
#endif
#endif

#define LIST_USE_STD_LIST   1

#if LIST_USE_STD_LIST
#include <list>
#endif

///////////////////////////////////////////////////////////////////////////////
class ACE_Socket_Reactor {
  public:
    ACE_Socket_Reactor();
    ~ACE_Socket_Reactor();

    int handle_events(ACE_Time_Value &max_wait_time);
    int register_handler (ACE_Event_Handler *handler, const ACE_Reactor_Mask mask);
    int remove_handler (ACE_Event_Handler *event_handler, const ACE_Reactor_Mask mask);

    // Not in ACE:
    int  get_handler_count() const;
    void enable_ioctl_read(bool enabled=true);
    void enable_sctp(bool enabled);

  private:
    int	    nReadFd, nWriteFd, nExceptFd;
    fd_set  fdr_static, fdw_static, fdx_static;
    fd_set  readFd, writeFd, exceptFd;
    bool    ioctl_read_enabled_;
    bool    useSctp;
    struct HandlerEntry {
#if !LIST_USE_STD_LIST
      HandlerEntry *next, *prev;
#endif
      int sock;
      ACE_Event_Handler *handler;
    };

#if !LIST_USE_STD_LIST
    HandlerEntry handlerList;
#else
    std::list<HandlerEntry*> handlerList;
#endif

    void process_socket (int sock, 
        ACE_Event_Handler *handler, 
        ACE_Reactor_Mask mask);
    HandlerEntry *find_handler(int sock);
};



#endif	// __ACE_SOCKET_REACTOR_H__

///////////////////////////////////////////////////////////////////////////////
