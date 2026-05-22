/* $Header: /lace/ace/Reactor_Imp_Sock.cpp 21    10/04/04 2:58p Bennylp $ */

#include "Reactor_Imp_Sock.h"
#include "ace/Event_Handler.h"
//#include "ace/ws2sctp.h"
#undef WIN32
#ifndef WIN32
#include <asm/ioctls.h>
#include <sys/ioctl.h>
#include <unistd.h>
#define BOOL int
#define TRUE 1
#define FALSE 0
#include <linux/net.h>
#include <sys/errno.h>
#define SO_ACCEPTCONN SO_ACCEPTCON
#endif

#ifndef NDEBUG
#include <stdio.h>
#endif

#ifdef _MSC_VER
#pragma warning( disable: 4018)
#endif

bool isSocketUseSctp(const ACE_HANDLE handle)
{
#ifndef __linux__
  WSAPROTOCOL_INFO info;
  int info_len=sizeof(info);
  if(handle<0)return false;
  if(getsockopt(handle, SOL_SOCKET, SO_PROTOCOL_INFO,(char*)&info, &info_len))
    return true;
  if(info.iProtocol!=IPPROTO_SCTP)
    return false;
#endif
  return true;
}


ACE_Socket_Reactor::ACE_Socket_Reactor()
{	
  //ACE_TRACE_("ACE_Socket_Reactor::ACE_Socket_Reactor");

  nReadFd = nWriteFd = nExceptFd = 0;
  FD_ZERO (&readFd);
  FD_ZERO (&writeFd);
  FD_ZERO (&exceptFd);

#if !LIST_USE_STD_LIST
  handlerList.next = handlerList.prev = &handlerList;
#endif
  ioctl_read_enabled_ = true;
}


ACE_Socket_Reactor::~ACE_Socket_Reactor()
{
  //ACE_TRACE_("ACE_Socket_Reactor::~ACE_Socket_Reactor");

#if !defined(NDEBUG) && defined(_WIN32)
  // Dump unfreed handlers
  unsigned i;
  char temp[80];
  int nfd[3] = {nReadFd, nWriteFd, nExceptFd};
  fd_set *fd[3] = {&readFd, &writeFd, &exceptFd};
  const char *name[3] = {"READ_MASK", "WRITE_MASK", "EXCEPT_MASK"};

  for (unsigned count=0; count < sizeof(name)/sizeof(name[0]); ++count) {
    if (nfd[count] != 0) {
      sprintf(temp, "Dumping remaining %d %s ACE_Event_Handlers:\n", nfd[count], name[count]);
      ::OutputDebugString(temp);
    } else {
      sprintf(temp, "ACE_Socket_Reactor: %s handlers leak check ok\n", name[count]);
      ::OutputDebugString(temp);
    }
    for (i=0;i<fd[count]->fd_count;++i) {
      int sock = fd[count]->fd_array[i];
      HandlerEntry *he = find_handler(sock);
      ACE_Event_Handler *h = he ? he->handler : NULL;
      sprintf(temp, "  %s ACE_Event_Handler at 0x%p is still registered\n", name[count], h);
      ::OutputDebugString(temp);
    }
  }
#endif

}


ACE_Socket_Reactor::HandlerEntry *ACE_Socket_Reactor::find_handler(int sock)
{
#if LIST_USE_STD_LIST
  std::list<HandlerEntry*>::iterator it, end = handlerList.end();
  for (it = handlerList.begin(); it!=end; ++it) {
    HandlerEntry *e = *it;
    if (e->sock == sock)
      return e;
  }
#else
  register HandlerEntry *entry = handlerList.next;
  while (entry != &handlerList) {
    if (entry->sock == sock)
      return entry;
    entry = entry->next;
  }
#endif
  return NULL;
}

int ACE_Socket_Reactor::get_handler_count() const
{
#if LIST_USE_STD_LIST
  int count = handlerList.size();
#else
  register int count = 0;
  register HandlerEntry *entry = handlerList.next;
  while (entry != &handlerList) {
    ++count;
    entry = entry->next;
  }
#endif
  return count;
}

void ACE_Socket_Reactor::enable_ioctl_read(bool enabled)
{
  ioctl_read_enabled_ = enabled;
}

void ACE_Socket_Reactor::enable_sctp(bool enabled)
{
  useSctp= enabled;
}

int ACE_Socket_Reactor::handle_events(ACE_Time_Value &max_wait_time)
{
  if (nReadFd == 0 && nWriteFd == 0 && nExceptFd == 0) 
    return 0;

  fd_set *fdr = NULL;
  if (nReadFd) {
    fdr_static = readFd;
    fdr = &fdr_static;
  }

  fd_set *fdw = NULL;
  if (nWriteFd) {
    fdw_static = writeFd;
    fdw = &fdw_static;
  }

  fd_set *fdx = NULL;
  if (nExceptFd) {
    fdx_static = exceptFd;
    fdx = &fdx_static;
  }

  //timeval tv  = max_wait_time.gettimeval(); //Yoga Modified this logic
  timeval tv;
  tv.tv_sec = max_wait_time.sec();
  tv.tv_usec = max_wait_time.usec();
  int result = ::select ( FD_SETSIZE, fdr, fdw, fdx,&tv);
  if (result < 1) return result;

#if LIST_USE_STD_LIST
  std::list<HandlerEntry*>::iterator it = handlerList.begin(), end = handlerList.end();
  while (it != end) {
    HandlerEntry *entry = *it;
    int sock = entry->sock;
    ACE_Event_Handler *handler = entry->handler;

#else
    HandlerEntry *it= handlerList.next;
    while (it!=&handlerList) {
      int sock = it->sock;
      ACE_Event_Handler *handler = it->handler;
      it = it->next;
#endif

      bool has_process = false;

      /** jnt: Each process_socket() may eventually remove handler from
       *       the handler_list (or worst, the handler instance may get
       *       destroyed). Solution: 
       *       - only one event is processed in each iteration, or
       *       - check if handler is still in the list before process_socket()
       **/
      if (fdr && FD_ISSET(sock,fdr)) {
        process_socket( sock, handler, ACE_Event_Handler::READ_MASK);
        FD_CLR(sock, fdr);
        has_process = true;

      } else if (fdw && FD_ISSET(sock,fdw)) {
        process_socket( sock, handler, ACE_Event_Handler::WRITE_MASK);
        FD_CLR(sock, fdw);
        has_process = true;

      } else if (fdx && FD_ISSET(sock,fdx)) {
        process_socket( sock, handler, ACE_Event_Handler::EXCEPT_MASK);
        FD_CLR(sock, fdx);
        has_process = true;
      }

      if (has_process) {
#if LIST_USE_STD_LIST
        it = handlerList.begin();
#else
        it = handlerList.next;
#endif
      } else {
#if LIST_USE_STD_LIST
        ++it;
#endif
      }
    }

    return result;
  }


  int ACE_Socket_Reactor::register_handler (ACE_Event_Handler *handler, 
      const ACE_Reactor_Mask mask)
  {
    //ACE_TRACE_("ACE_Socket_Reactor::register_handler");
    int sock = handler->get_handle();

    HandlerEntry *entry = find_handler(sock);
    if (entry==NULL) {
      try {
        entry = new HandlerEntry;
      } catch (...) {
        //ACE_TRACE_("ACE_Socket_Reactor::register_handler: EXCEPTION in new Operator");
        return -1;
      }

      // init entry
      entry->sock = sock;
      entry->handler = handler;

      // insert to the list
#if LIST_USE_STD_LIST
      handlerList.push_back(entry);
#else
      entry->next = handlerList.next;
      entry->prev = &handlerList;
      handlerList.next = entry;
      entry->next->prev = entry;
#endif

      //handler->mask = 0; //Yoga temporarly commented during ACE lib include
    } else {
      //assert (entry->handler == handler);
      entry->handler = handler;
    }

    //const ACE_Reactor_Mask add_mask = (mask & handler->mask) ^ mask; //Yoga
    const ACE_Reactor_Mask add_mask = mask;

    if ((add_mask & ACE_Event_Handler::READ_MASK) != 0) {
      ++nReadFd;
      //assert(nReadFd <= FD_SETSIZE);
      FD_SET (sock, &readFd);
    }
    if ((add_mask & ACE_Event_Handler::WRITE_MASK) != 0) {
      ++nWriteFd;
      //assert(nWriteFd <= FD_SETSIZE);
      FD_SET (sock, &writeFd);
    }
    if ((add_mask & ACE_Event_Handler::EXCEPT_MASK) != 0) {
      ++nExceptFd;
      //assert(nExceptFd <= FD_SETSIZE);
      FD_SET (sock, &exceptFd);
    }

    //handler->mask |= add_mask; //Yoga temporarly commented during ACE lib include

    return 0;
  }


  int ACE_Socket_Reactor::remove_handler (ACE_Event_Handler *event_handler, const ACE_Reactor_Mask mask)
  {
    //ACE_TRACE_("ACE_Socket_Reactor::remove_handler");
    int sock = event_handler->get_handle();

    //const ACE_Reactor_Mask remove_mask = mask & event_handler->mask; //Yoga
    const ACE_Reactor_Mask remove_mask = mask;

    HandlerEntry *it = find_handler(sock);
    if (it == NULL)
      goto on_fini;	// not found

    if ((remove_mask & ACE_Event_Handler::READ_MASK) != 0) {
      --nReadFd;
      FD_CLR (sock, &readFd);
    }
    if ((remove_mask & ACE_Event_Handler::WRITE_MASK) != 0) {
      --nWriteFd;
      FD_CLR (sock, &writeFd);
    }
    if ((remove_mask & ACE_Event_Handler::EXCEPT_MASK) != 0) {
      --nExceptFd;
      FD_CLR (sock, &exceptFd);
    }

    // Update mask
    //event_handler->mask ^= remove_mask; //Yoga

#if 1
    // Only remove Event_Handler from our record if all masked handlers have
    // been removed.
    /*if ((event_handler->mask & ACE_Event_Handler::READ_MASK) == 0 &&
        (event_handler->mask & ACE_Event_Handler::WRITE_MASK) == 0 &&
        (event_handler->mask & ACE_Event_Handler::EXCEPT_MASK) == 0) */ //Yoga commented
    if (ACE_Event_Handler::READ_MASK == 0 && ACE_Event_Handler::WRITE_MASK == 0 && ACE_Event_Handler::EXCEPT_MASK == 0) 
#else
      // Always remove
      if (1)
#endif
      {
        // erase from the list
#if LIST_USE_STD_LIST
        handlerList.remove(it);
#else
        it->next->prev = it->prev;
        it->prev->next = it->next;
#endif
        delete it;
      }


on_fini:
    // Always call 
    if ((mask & ACE_Event_Handler::DONT_CALL) != ACE_Event_Handler::DONT_CALL)
      event_handler->handle_close(sock, mask);
    return 0;
  }


  void ACE_Socket_Reactor::process_socket (int sock, 
      ACE_Event_Handler *handler,
      ACE_Reactor_Mask mask)
  {
    //ACE_TRACE_("ACE_Socket_Reactor::process_socket");

#ifdef WIN32
    unsigned long value = 0;

    // If ioctl_read_enabled_ is enabled, then get the number of available
    // bytes in the socket. The purpose of this is to see whether socket
    // has been closed. But this has no purpose for UDP socket, so if
    // ioctl_read_enabled_ is disabled, always pretend that data is 
    // available. The application then will have to detect whether the socket
    // has been closed (by examining return value of recv/send etc.)
    if (ioctl_read_enabled_&!useSctp) {
      if (::ioctlsocket(sock, FIONREAD, &value)) {
        // Invalid socket
        remove_handler (handler, ACE_Event_Handler::RWE_MASK);
        return;
      }
    } else
      value = 1;

    if ((mask & ACE_Event_Handler::READ_MASK) != 0) {
      if (value < 0) {
        remove_handler (handler, ACE_Event_Handler::RWE_MASK);
      } else if (value == 0) {
        BOOL is_listening = FALSE;
        int optlen = sizeof(is_listening);

        if (::getsockopt(sock, SOL_SOCKET, SO_ACCEPTCONN, (char*)&is_listening, &optlen))
          is_listening = FALSE;


        if (is_listening) {
          if (handler->handle_input() < 0) {
            remove_handler (handler, ACE_Event_Handler::RWE_MASK);
          }
        } else
          remove_handler (handler, ACE_Event_Handler::RWE_MASK);
      } else {
        if (handler->handle_input() < 0) {
          remove_handler (handler, ACE_Event_Handler::RWE_MASK);
        }
      }

#else
      //socket readable
      if ((mask & ACE_Event_Handler::READ_MASK) != 0) {
        //iwanj:
        //cause:
        //FIONREAD always fail for server socket, and
        //SO_ACCEPTCON not available as well
        //let's make bad assumption:
        //if FIONREAD fail and errno==EINVAL then sock probably is the server type

        int value;
        if (::ioctl(sock, FIONREAD, &value) && errno==EINVAL) {
          if (handler->handle_input() < 0) {
            remove_handler (handler, ACE_Event_Handler::RWE_MASK);
          }
          return;
        }

        //regular socket
        if (value>0)
        {
          if (handler->handle_input() < 0) 
            remove_handler (handler, ACE_Event_Handler::RWE_MASK);
        }
        else
        {
          remove_handler (handler, ACE_Event_Handler::RWE_MASK);
        }
#endif	/* WIN32 */

        //socket writeable
      } else if ((mask & ACE_Event_Handler::WRITE_MASK) != 0) {
        if (handler->handle_output() != 0)
          remove_handler (handler, ACE_Event_Handler::RWE_MASK);

        //socket exception 
      } else if ((mask & ACE_Event_Handler::EXCEPT_MASK) != 0) {
        if (handler->handle_exception() != 0)
          remove_handler (handler, ACE_Event_Handler::RWE_MASK);
      }
    }

