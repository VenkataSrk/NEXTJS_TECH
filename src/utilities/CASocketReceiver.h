#ifndef CA_SOCKET_RECEIVER_H
#define CA_SOCKET_RECEIVER_H

#define SOCKET_READ_SIZE  2048

#include "CASocketContext.h"
#include "CASocket.h"

namespace CA
{
class CompleteMsgBuf
{
   public:
      std::string msg;
      unsigned int msgLen;

      CompleteMsgBuf() : msg(""), msgLen(0){};
      ~CompleteMsgBuf() 
      {
	 msg.clear();
	 msgLen = 0;
      };
      CompleteMsgBuf(const CompleteMsgBuf& rhs) 
      {
	 msg = rhs.msg;
	 msgLen = rhs.msgLen;
      };
      CompleteMsgBuf& operator= (const CompleteMsgBuf& rhs) 
      {
	 msg = rhs.msg;
	 msgLen = rhs.msgLen;
	 return *this;
      };
};

class SocketPendingMsg
{
   public:
      bool isPending;
      std::string delimiter;
      std::string pendingMsg;
      unsigned int pendingMsgLen;
      CompleteMsgBuf completeMsg;
      SocketPendingMsg();
      ~SocketPendingMsg();
      bool getCompleteMsg();
};

class CASocket;
class CASocketReceiver : public ACE_Event_Handler
{
   public:
      CASocketReceiver(ACE_Reactor*, CASocket*);
      virtual ~CASocketReceiver(void);

      ACE_HANDLE get_handle(void) const;
      ACE_HANDLE get_sctpHandle(void) const;
      ACE_Reactor* getReactor()
      {
	 return m_reactor;
      }
      CASocket* getSocketInstance()
      {
	 return m_ptCASocket;
      }
   
      ACE_SOCK_Stream& peer() const;
      ACE_SOCK_SEQPACK_Association& peerSctp() const;
      ACE_INET_Addr& get_remote_conn_addr() const;

      //register stream with receiver
      bool registerHandle(ACE_SOCK_Stream& peerStream);
      bool registerSctpHandle(ACE_SOCK_SEQPACK_Association& peerStream);
      //handle close request
      int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask);
      //handle input request
      int handle_input(ACE_HANDLE handle);

   private:
      CASocketReceiver(const CASocketReceiver& rhs);
      CASocketReceiver& operator= (const CASocketReceiver& rhs);

      ACE_SOCK_Stream m_peer;
      ACE_SOCK_SEQPACK_Association m_peerSctp;
      ACE_Reactor *m_reactor;
      CASocket* m_ptCASocket;
      SocketPendingMsg *m_recvdBuf;
      char m_socketData[SOCKET_READ_SIZE];
      ACE_INET_Addr m_localAddr;
      ACE_INET_Addr m_remoteAddr;
};
};
#endif
