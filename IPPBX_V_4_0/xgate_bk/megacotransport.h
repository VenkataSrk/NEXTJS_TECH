

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <netdb.h>
#include <list>
#include <assert.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
 
#ifndef __MEGACO_TRANSPORT_H__
#define __MEGACO_TRANSPORT_H__
using namespace std;

#define H248_STANDARD_PORT 2944

 
class MegacoMasterAcceptor;

class MegacoTransport
{
 
  public:
 
    ~MegacoTransport();
 
    static size_t SendMessage(int sd, char* buffer, size_t len);
    static size_t ReceiveMessage(int sd, char* buffer, size_t len);
 
    int GetPeerIP() { return m_peerIP;}
    int GetSocketId(){ return megaco_sd;}
    int SetSocketId(int nSd){ megaco_sd = nSd;}
    int GetPeerPort(){ return m_peerPort; }
    bool  operator == (const MegacoTransport *rhs) {
           return ( (rhs) &&( m_peerIP == rhs->m_peerIP)&& (m_peerPort == rhs->m_peerPort));
    }
 
    MegacoTransport(int ipaddress, int port);
    MegacoTransport(){}
    MegacoTransport(const MegacoTransport& stream){
      m_peerIP = stream.m_peerIP;
      m_peerPort = stream.m_peerPort;
      printf("PeerIP %d : peerPort %d \n",m_peerIP,m_peerPort);
    }
   
  protected:
    int  megaco_sd;
    int  m_peerIP;
    int  own_ip;
    int  own_port;
    int  m_peerPort;
    int  transportType;
};


/*
size_t MegacoTransport::SendMessage(int sd,char* buffer, size_t len)
{
  assert(sd  != 0);
 return (::send(sd, buffer, len, 0));

}

size_t MegacoTransport::RecieveMessage(int sd, char* buffer, size_t len)
{
  assert(sd != 0);
 return (::recv(sd, buffer, len, 0));
}

*/

MegacoTransport::MegacoTransport(int myip, int myport) {
    m_peerIP = myip;
    m_peerPort = myport;
}

MegacoTransport::~MegacoTransport()
{
    close(megaco_sd);
}

 
class MegacoClientConnector
{
  public:
    MegacoClientConnector(){}
    static int SocketOpen(int family, int protocol, int flag );
    static int SocketConnect( int sd, const char* server, int port);
    static int resolveHostName(const char* host, struct in_addr* addr);
    static int SocketBind(int sd, char *bAddr);
};


int MegacoClientConnector::SocketBind(int sd, char *bindAddress)
{
  struct sockaddr_in addr;
  addr.sin_addr.s_addr = inet_addr(bindAddress);
  addr.sin_port = H248_STANDARD_PORT;

 if( bind(sd, (struct sockaddr *)&addr,sizeof(addr))<0)
 {
    printf("CLient SOCK Bind failedi: errno %d\n", errno);
   return -1;
 }
  return 0;
}




int MegacoClientConnector::SocketOpen(int family, int protocol, int flag )
{
    int sd = socket(family, protocol, 0);
    if(sd) return sd; 
    return -1;
}


int MegacoClientConnector::SocketConnect( int sd, const char* server, int port)
{
    struct sockaddr_in address;
 
    memset (&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = (port);
    if (resolveHostName(server, &(address.sin_addr)) != 0) {
        inet_pton(PF_INET, server, &(address.sin_addr));        
    } 
    if (::connect(sd, (struct sockaddr*)&address, sizeof(address)) != 0) {
        printf("CONNECT System call failed : %d\n", errno);
        return NULL;
    }
    printf("CONNECT Success\n"); 
    
    return 0;
}

int MegacoClientConnector::resolveHostName(const char* hostname, struct in_addr* addr) 
{
    struct addrinfo *res;
 
    int result = getaddrinfo (hostname, NULL, NULL, &res);
    if (result == 0) {
        memcpy(addr, &((struct sockaddr_in *) res->ai_addr)->sin_addr, 
               sizeof(struct in_addr));
        freeaddrinfo(res);
    }
    return result;
}

using namespace std;
 
class MegacoMasterAcceptor  : public MegacoTransport 
{
 
  public:
    ~MegacoMasterAcceptor();
    MegacoMasterAcceptor(){}
    MegacoMasterAcceptor(string acpt_address, int port);
 
    int accept( MegacoTransport *newStream);
    int start();
 
  private:
    string own_address;
    int    acceptor_port;
#if 0
    bool   m_listening;
    int    m_lsd;
#endif
};

MegacoMasterAcceptor::MegacoMasterAcceptor(string address, int port) 
    { own_address = address; acceptor_port=port;}

MegacoMasterAcceptor::~MegacoMasterAcceptor()
{
#if 0
    if (m_lsd > 0) {
        close(m_lsd);
    }
#endif
}

int MegacoMasterAcceptor::start()
{
    int nbio;
#if 0
    if (m_listening == true) {
        return 0;
    }
#endif
 
    megaco_sd = socket(AF_INET, SOCK_STREAM, 0);
    printf("ACCEPTOR sockfd %d \n", megaco_sd);
 
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = (acceptor_port);
    address.sin_addr.s_addr = inet_addr(own_address.c_str());
#if 0
    if (m_address) {
        inet_pton(PF_INET, m_address, &(address.sin_addr));
        printf("OWN ADDRESS %s PORT %d\n",m_address,address.sin_port);
    }
    else {
        address.sin_addr.s_addr = INADDR_ANY;
    }
#endif
 
    int optval = 1;
    setsockopt(megaco_sd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
 
    //ioctl(m_lsd, FIONBIO, &nbio);
    int result = bind(megaco_sd, (struct sockaddr*)&address, sizeof(address));
    if (result != 0) {
        perror("bind() failed");
        return result;
    }
    result = listen(megaco_sd, 5);
    if (result != 0) {
        perror("listen() failed");
        return result;
    }
    //m_listening = true;
    return result;
}

int MegacoMasterAcceptor::accept( MegacoTransport *newStream)
{
    struct sockaddr_in address;
    socklen_t len = sizeof(address);
    memset(&address, 0, sizeof(address));
    printf("SOCKFD in accept %d \n",megaco_sd);
    int new_sd = ::accept(megaco_sd, (struct sockaddr*)&address, &len);
    if (new_sd < 0) {
        perror("accept() failed");
        return NULL;
   }
   printf("OLD SD %d new SD %d \n",megaco_sd,new_sd);
   printf("CLIENT IP %x CLIENT PORT %x \n",address.sin_addr.s_addr, address.sin_port);
   
   newStream = new MegacoTransport(address.sin_addr.s_addr, address.sin_port);
   newStream->SetSocketId(new_sd);
   if(new_sd>0) return new_sd;
    return 0;
}

#endif
