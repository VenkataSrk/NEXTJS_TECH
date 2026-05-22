#include "CAUtilites.h"
#include "CAThread.h"


//#include "CASocket.h"

/*enum class CAprotocaltype: unsigned int {
    _AF_INET = 0,  // IPv4 protocol
    _AF_INET6 // IPv6 protocol
};
 enum class CAsockettype: unsigned int{
    _SOCK_STREAM = 0, // for TCP (reliable, connection oriented)
    _SOCK_DGRAM // for UDP (unreliable, connectionless)
};*/

typedef enum CAProtocaltype
{
   _AF_INET = 0,  // IPv4 protocol
    _AF_INET6 // IPv6 protocol
}CAprotocaltype;

typedef enum CASockettype
{
   _SOCK_STREAM = 0,  // IPv4 protocol
    _SOCK_DGRAM // IPv6 protocol
}CAsockettype;

class clientInfo{
    public:
    clientInfo()=default;
    virtual~clientInfo()=default;

    int socketfd;
    std::string ipAddr;
    bool status;
 
};

class CAServerSocket 
{
    private:
    std::map<int, clientInfo*> m_clientInfo;   
    bool m_SocketStatus;
  public:
    CAServerSocket()=default;
    //CAServerSocket(CASocket*);
    virtual ~CAServerSocket() = default;
    inline void setSocketStatus(bool status){
        m_SocketStatus =  status;
    }
    inline bool getSocketStatus(){
    return m_SocketStatus;
    }
    
    struct sockaddr_in server_addr;
    int protocaltype,sockettype;
    socklen_t size;
    int client, server,socketfd;
    char buffer[256];
    int bufsize = 1024;
    std::string data;
    bool isExit = false;
    bool CreateServerSockethandle(std::string ipaddr,int port, CAprotocaltype _protocaltype, CAsockettype _sockettype);
    
    bool insertToSocketMap(clientInfo*);

    void ReciveData(clientInfo*);
    void SendData(std::string data);
    void CloseSockethandle();
};
