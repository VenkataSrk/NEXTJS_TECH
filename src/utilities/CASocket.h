#ifndef CA_SOCKET_H
#define CA_SOCKET_H

#pragma once

#define SERVER_ADDRESS_LENGTH 50
#define SET(type, vartype, varname) void set##varname(type varname) {m_##vartype##varname = varname;}
#define GET(vartype, varname) auto get##varname() {return m_##vartype##varname;}
#define GETSTR(vartype, varname) STR get##varname() {return m_##vartype##varname.c_str();}

#include "CASocketReceiver.h"
#include "CAThread.h"

namespace CA
{

enum class EN_CA_TIMER_TYPE
{
   EN_CA_TIMER_NULL,
   EN_CA_TIMER_RECONNECT,
   EN_CA_TIMER_SOCKET_VALIDATE,
};

enum class EN_CA_SOCKET_PROTOCOL
{
   EN_CA_SOCKET_PROTOCOL_UNKNOWN,
   EN_CA_SOCKET_PROTOCOL_UDP,
   EN_CA_SOCKET_PROTOCOL_TCP
};

class SocketTimerData
{
   public:
      SocketTimerData()=default;
      ~SocketTimerData()=default;

      EN_CA_TIMER_TYPE m_timerType;
      STR m_strData;
};

class CASocketMsg : public CAMessageBlock
{
   public:
      CASocketMsg()=default;
      virtual ~CASocketMsg()=default;

      SET(STR, str, Data);
      GET(str, Data);
      SET(STR,str,ClientIP);
      GET(str,ClientIP);
      SET(unsigned short,us,ClientPort);
      GET(us,ClientPort);
      
   private:
      STR m_strData;
      STR m_strClientIP;
      unsigned short m_usClientPort;
      
};

class CASocketContext;
class CASocket : public ACE_Event_Handler
{
   public:
      CASocket()=default;
      CASocket(EN_CA_SOCKET_TYPE socketType, unsigned short port, EN_CA_SOCKET_PROTOCOL protocol, \
		      CAThread* ptCASocketController, unsigned short connTimeout, unsigned short resTimeout, \
		      std::vector<std::string> ipList={}, int srcMsgType=0, int closeMsgType=0);
      virtual ~CASocket();

      ACE_Reactor* getReactor()
      {
	 return m_reactor;
      }
      CASocketContext* getSocketHandler()
      {
	 return m_ptCASocketContext;
      }
      CAThread* getSocketController()
      {
         return (m_ptCASocketController!=nullptr) ? m_ptCASocketController : nullptr;
      }
      EN_CA_SOCKET_TYPE getSocketType()
      {
         return m_enSocketType;
      }
      void insertConnectedList(STR ip)
      {
         auto itr = std::find(m_vConnectedList.begin(), m_vConnectedList.end(), ip);
         if(itr==m_vConnectedList.end())
         {
            m_vConnectedList.push_back(ip);
         }
         auto itrs = std::find(m_vConnectList.begin(), m_vConnectList.end(), ip);
         if(itrs==m_vConnectList.end())
         {
            m_vConnectList.push_back(ip);
         }
      }
      void removeConnectedList(STR ip)
      {
         
         m_vConnectedList.erase(std::remove(m_vConnectedList.begin(), m_vConnectedList.end(), ip), m_vConnectedList.end());
      }
      VECTOR<STR> getConnectedList()
      {
         return m_vConnectedList;
      }
      int getSrcMsgType()
      {
	 return m_iSrcMsgType;
      }
      int getCloseMsgType()
      {
         return m_iCloseMsgType;
      }

      static void run (void* arguments);
      bool stopThread(void);
      bool init();
      bool acceptor();
      bool sendData(const char *data, int len, std::string ipAddr, unsigned short port);
      int handle_close (ACE_HANDLE handle, ACE_Reactor_Mask close_mask);
      int handle_input(ACE_HANDLE aceHandle);
      bool validateSocketConnection(STR data);

      static char m_localIp4Addr[INET_ADDRSTRLEN];
      static char m_localIp6Addr[INET6_ADDRSTRLEN];
      static char * getLocalIpAddr(unsigned short AddrType) ;
      static bool findLocalIpAddr(char *ipAddr, unsigned short AddrType);

      long int m_iTimerId;
      unsigned short m_iConnectionTimeout;
      unsigned short m_iResponseTimeout;

   private:
      CASocket(const CASocket& rhs);
      CASocket operator=(const CASocket& rhs);
      bool m_running;

   protected:
      ACE_Reactor *m_reactor;
      EN_CA_SOCKET_TYPE m_enSocketType;
      std::vector<ACE_SOCK_Acceptor*> m_acceptorList;
      std::vector<ACE_SOCK_SEQPACK_Acceptor*> m_acceptorListSctp;
      std::vector<std::string> m_vConnectList;
      std::vector<std::string> m_vConnectedList;
      unsigned short m_iConnectPort;
      EN_CA_SOCKET_PROTOCOL m_enConnectProtocol;
      CASocketContext* m_ptCASocketContext;
      CAThread* m_ptCASocketController;
      int m_iSrcMsgType;
      int m_iCloseMsgType;
};
};
#endif
