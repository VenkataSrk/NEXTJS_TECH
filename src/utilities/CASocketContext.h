#ifndef CA_SOCKET_CONTEXT_H
#define CA_SOCKET_CONTEXT_H

#include "CAUtilites.h"
#include "CAThread.h"

namespace CA
{
class CAConnInfo;

typedef std::vector<CAConnInfo> SOCKET_CONN_INFO;
typedef SOCKET_CONN_INFO::iterator ITR_SOCKET_CONN_INFO;

enum class EN_CA_SOCKET_TYPE
{
   EN_CA_SOCKET_TYPE_UNKNOWN,
   EN_CA_SOCKET_TYPE_CLIENT,
   EN_CA_SOCKET_TYPE_SERVER
};

class CAConnInfo
{
   public:
      CAConnInfo()
      {
	 port     = 0;
	 fd       = -1;
	 srcPort  = 0;
	 timeoutCount = 0;
	 timerId = 0;
      }
      ~CAConnInfo()
      {}

      std::string     ipAddress;
      unsigned short  port;
      int             fd;
      unsigned short  srcPort;
      unsigned short timeoutCount;
      long int timerId;

      bool operator==(CAConnInfo &rhs)
      {
	 if(ipAddress == rhs.ipAddress && port == rhs.port && fd == rhs.fd && srcPort == rhs.srcPort)
	 {
	    return true;
	 }
	 else
	 {
	    return false;
	 }
      }
};

class CASocketContext
{
   public:
      CASocketContext()=default;
      ~CASocketContext()=default;

      //add new connection info into map
      bool insertConnectionInfo(CAConnInfo urConnInfo);
      //fetch connection info from map
      bool getConnectionInfo(EN_CA_SOCKET_TYPE socType, std::string ipAddr, unsigned short port, int& handle);
      //find connection info in map based on ip and port
      bool findConnectionInfo(EN_CA_SOCKET_TYPE socType, std::string ipAddr, unsigned short port);
      //delete connection info from map
      bool eraseConnectionInfo(CAConnInfo urConnInfo);

   private:
      //map for storing connection info
      SOCKET_CONN_INFO m_vecCAConnInfo;
};
};
#endif

