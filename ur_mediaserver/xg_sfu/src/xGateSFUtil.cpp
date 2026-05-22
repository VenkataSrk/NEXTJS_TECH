//local includes

#include <netinet/in.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <uuid/uuid.h>
#include "xglog.h"
#include "xGateUtil.h"
#include "SFUClient.h"
#include "xGateSFUtil.h"
#define THISMODULE "SFUtil"

vector<string> xGateSFUtil::m_MgcIpAddressList;
char xGateSFUtil::m_localIp4Addr[INET_ADDRSTRLEN] = {0};
char xGateSFUtil::m_localIp6Addr[INET6_ADDRSTRLEN] = {0};

#ifdef XGATE_HAS_GSTREAMER
MBCSForwarderController* xGateSFUtil::m_mbcSFctrl = NULL;
SFUController* xGateSFUtil::m_sfuController = NULL;
SFUBridgeProcessor* xGateSFUtil:: m_sfuBridgeProcessor = NULL;
SFUClient* xGateSFUtil::m_sfuClient = NULL;
int xGateSFUtil::m_sfu_id;
#endif

xGateRetVal xGateSFUtil::findLocalIpAddr(char *ipAddr, unsigned short AddrType)
{
  struct ifaddrs * ifAddrStruct=NULL;
  struct ifaddrs * ifa=NULL;
  void * tmpAddrPtr=NULL;

  if(ipAddr == NULL) {
    return EN_XGATE_STATUS_ERROR;
  }

  getifaddrs(&ifAddrStruct);

  for(ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
    if(!ifa->ifa_addr || ifa->ifa_addr->sa_family != AddrType || strcmp(ifa->ifa_name, "lo") == 0){
      continue;
    }
    if (AF_INET ==ifa->ifa_addr->sa_family) { // check it is IP4
      // is a valid IP4 Address
      tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
      inet_ntop(AF_INET, tmpAddrPtr, ipAddr, INET_ADDRSTRLEN);
      printf("xGateSFUtil::findLocalIpAddr, %s IP4 Address %s\n", ifa->ifa_name, ipAddr);
    } else if (AF_INET6 == ifa->ifa_addr->sa_family) { // check it is IP6
      // is a valid IP6 Address
      tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
      inet_ntop(AF_INET6, tmpAddrPtr, ipAddr, INET6_ADDRSTRLEN);
      printf("xGateSFUtil::findLocalIpAddr, %s IP6 Address %s\n", ifa->ifa_name, ipAddr);
    }
    break;
  }
  if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
  return EN_XGATE_STATUS_SUCCESS;
}

char * xGateSFUtil::getLocalIpAddr(unsigned short AddrType) {
  switch(AddrType) {
    case AF_INET:
      {
        if(m_localIp4Addr[0] != 0) {
          return m_localIp4Addr;
        } else {
          findLocalIpAddr(m_localIp4Addr, AF_INET);
          return m_localIp4Addr;
        }
        break;
      }
    case AF_INET6:
      {
        if(m_localIp6Addr[0] != 0) {
          return m_localIp6Addr;
        } else {
          findLocalIpAddr(m_localIp6Addr, AF_INET6);
          return m_localIp6Addr;
        }
        break;
      }
    default:
      {
        printf("xGateSFUtil::getLocalIpAddr, given address type not supported\n");
        break;
      }
  }
}

void xGateSFUtil::setLocalIpAddr(char *localIpAddr)
{
  if(strlen(localIpAddr) > 16) {
    memset(m_localIp6Addr, '\0', INET6_ADDRSTRLEN);
    memcpy(m_localIp6Addr, localIpAddr, INET6_ADDRSTRLEN);
  } else {
    memset(m_localIp4Addr, '\0', INET_ADDRSTRLEN);
    memcpy(m_localIp4Addr, localIpAddr, INET_ADDRSTRLEN);
  }
}

void xGateSFUtil::makeDir(const char *dir)
{
  char tmp[256];
  char *p = NULL;
  size_t len;

  snprintf(tmp, sizeof(tmp),"%s",dir);
  len = strlen(tmp);
  if(tmp[len - 1] == '/')
    tmp[len - 1] = 0;
  for(p = tmp + 1; *p; p++)
    if(*p == '/') {
      *p = 0;
      mkdir(tmp, S_IRWXU);
      *p = '/';
    }
  mkdir(tmp, S_IRWXU);
}

string xGateSFUtil::generate_random_string(int length)
{
 char letters[26] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n',
                     'o','p','q','r','s','t','u','v','w','x','y','z'};
 string ran = "";
 for(int i = 0; i < length; i++){
   ran = ran + letters[rand() % 26];
 }
 return ran;
}

string xGateSFUtil::generate_callId()
{
   uuid_t id;
   size_t len;
   char _string[100];

   uuid_generate(id);

   memset(_string, 0, sizeof(_string));
   uuid_unparse(id, _string);
   len = strlen(_string);

   return _string;
}
