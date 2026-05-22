#ifndef _XGATE_TCP_SOCKET_HANDLER_H
#define _XGATE_TCP_SOCKET_HANDLER_H

//ace includes
#include <ace/Event_Handler.h>
#include <ace/Reactor.h>
#include <ace/Task_T.h>

struct completedMsgBuf 
{
   string msg; //one extracted complete msg
   unsigned int msgLen; //length of one extracted complete msg

   completedMsgBuf() : msg(""), msgLen(0){};
   ~completedMsgBuf() {
      msg.clear();
      msgLen = 0;
   };
   completedMsgBuf(const completedMsgBuf& rhs) {
      msg = rhs.msg;
      msgLen = rhs.msgLen;
   };
   completedMsgBuf& operator= (const completedMsgBuf& rhs) {
      msg = rhs.msg;
      msgLen = rhs.msgLen;
      return *this;
   };
};

class TcpJsonPendingMsg
{
   public:
      bool isPending; //pending msg is available or not
      string delimiter;
      string pendingMsg; //incomplete pending json msg in buffer
      unsigned int pendingMsgLen; //length of incomplete pending json msg in buffer
      completedMsgBuf completedMsg;
      TcpJsonPendingMsg();
      ~TcpJsonPendingMsg();
      xGateRetVal get_one_complete_msg();
};

struct TcpDecodePendingBuf
{
   bool            status;          // initially false, true if string "\r\n\r\n" received 
   unsigned int    total_len;       // total length of SIP messager
   bool            pending;         // initially true, false if total length received
   unsigned int    recvdLength;     // length of data recvd for current SIP msg
   unsigned int    nextRecvdLength; // length of data recvd for Next SIP msg
   unsigned int    cid;             // Connection Id
   char*           pData;           // ptr to recevied data for current SIP msg
   char*           pNextMsg;        // ptr to received data for next SIP msg.
   //CTOR
   TcpDecodePendingBuf()
   {
      status      = false;
      total_len         = 0;
      pending     = true;
      recvdLength       = 0;
      nextRecvdLength   = 0;
      cid         = 0;
      pData       = 0;
      pNextMsg    = 0;
   }
   //DTOR
   ~TcpDecodePendingBuf()
   {
      status      = false;
      total_len         = 0;
      pending     = false;
      recvdLength       = 0;
      nextRecvdLength   = 0;
      cid         = 0;
      delete [] pData;
      delete []pNextMsg;
   } 
};

class xGateTCPSocketHandler : public ACE_Event_Handler
{
   public:
      /**
       *  Default Constructor
       */
      xGateTCPSocketHandler (const char* , ACE_Reactor*,ACE_Task<ACE_MT_SYNCH>*);

      /**
       * Destructor
       */
      virtual ~xGateTCPSocketHandler (void);
      /**
       * Initialize the SipProxyTCPSocketHandler with the ACE_HANDLER (socket descriptor)
       * @parm ACE_HANDLE socketHandle to the connection's socket.
       */
      xGateRetVal init (ACE_SOCK_Stream& peerStream);

      /**
       * Called back automatically when a connection has been dropped
       **/
      int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask);

      /**
       * Called upon receiving data
       **/
      int handle_input(ACE_HANDLE handle);

      /// Methods to decode the TCP Msg and post into Queue
      void  post(char *,int);

      /// Interface to set the Peer FQDN
      void  set_peer_fqdn(string fqdn)
      {
	 m_peerFQDN = fqdn;
      }

      /// Interface to return the Peer FQDN
      string&  get_peer_fqdn(string fqdn)
      {
	 return m_peerFQDN;
      }

      /**
       * Method which returns the handle to the peer socket 
       **/
      ACE_HANDLE get_handle(void) const;

      /**
       * Accessor for the reactor
       **/
      //  ACE_Reactor* getReactor(void) const;

      ACE_Reactor* getReactor() 
      { 
	 return m_reactor;
      }
      /**
       * Method returns a reference to the peer stream
       **/
      ACE_SOCK_Stream& peer() const;

      ACE_INET_Addr& get_remote_conn_addr() const; 

   private:
      /**
       * Private copy constructor - disallow copies and automatic methods.
       **/
      xGateTCPSocketHandler(const xGateTCPSocketHandler& rhs);

      /**
       * Private assignment operator - disallow assignment and automatic methods.
       **/
      xGateTCPSocketHandler& operator= (const xGateTCPSocketHandler& rhs);

      /**
       * Add a new attribute for the underlying stream which will be used by the Event Handler
       **/
      ACE_SOCK_Stream m_peer; 

      ACE_Reactor *m_reactor;

      char m_serverType[SERVER_TYPE_SIZE]; 
      TcpJsonPendingMsg *m_recvdBuf;
      unsigned char m_socketData[XGATE_TCP_READ_SIZE];
      ACE_INET_Addr m_localAddr;
      ACE_INET_Addr m_remoteAddr;
      ACE_Task<ACE_MT_SYNCH>* m_taskPtr;

      string m_remoteIpAddr;
      string m_peerFQDN;
};

#endif
