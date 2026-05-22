#include "xglog.h"
#include <ifaddrs.h>
#include "xGateUtil.h"
#include "XGController.h"
#include "xGateMediaService.h"

char xGateUtil::m_localIp4Addr[INET_ADDRSTRLEN] = {0};
char xGateUtil::m_localIp6Addr[INET6_ADDRSTRLEN] = {0};


xGateRetVal xGateUtil::findLocalIpAddr(char *ipAddr, unsigned short AddrType)
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
      printf("xGateUtil::findLocalIpAddr, %s IP4 Address %s\n", ifa->ifa_name, ipAddr);
    } else if (AF_INET6 == ifa->ifa_addr->sa_family) { // check it is IP6
      // is a valid IP6 Address
      tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
      inet_ntop(AF_INET6, tmpAddrPtr, ipAddr, INET6_ADDRSTRLEN);
      printf("xGateUtil::findLocalIpAddr, %s IP6 Address %s\n", ifa->ifa_name, ipAddr);
    }
    break;
  }
  if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
  return EN_XGATE_STATUS_SUCCESS;
}

char * xGateUtil::getLocalIpAddr(unsigned short AddrType) {
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
        printf("xGateUtil::getLocalIpAddr, given address type not supported\n");
        break;
      }
  }
	return NULL;
}

void xGateUtil::setLocalIpAddr(const char *localIpAddr)
{
  if(strlen(localIpAddr) > 16) {
    memset(m_localIp6Addr, '\0', INET6_ADDRSTRLEN);
    memcpy(m_localIp6Addr, localIpAddr, INET6_ADDRSTRLEN);
  } else {
    memset(m_localIp4Addr, '\0', INET_ADDRSTRLEN);
    memcpy(m_localIp4Addr, localIpAddr, INET_ADDRSTRLEN);
  }
}

StunConfig::StunConfig():
 m_userName(""), m_password(""), m_stunDomain(""), m_port(0)
{
}

StunConfig::~StunConfig()
{
}

StunConfig& StunConfig::operator=(StunConfig &src)
{
	m_userName = src.m_userName;
	m_password = src.m_password;
	m_stunDomain = src.m_stunDomain;
	m_port = src.m_port;
  return *this;
}

AudioDetail::AudioDetail():
  m_mediaType(0), m_fmtp(""), m_codec(0), m_codecName(""), m_ptime(0),m_playFile(""),
  m_clientIp(""), m_clientPort(0), m_serverIp(""), m_serverPort(0), m_relayIp(""), m_relayPort(0),
  m_rflxIp(""), m_rflxPort(0), m_dialIp(""), m_dialPort(0), m_respIp(""), m_respPort(0)
{
}

AudioDetail::~AudioDetail()
{
}

AudioDetail& AudioDetail::operator=(AudioDetail &src)
{
  m_mediaType = src.m_mediaType;
  m_fmtp = src.m_fmtp;
  m_codec = src.m_codec;
  m_codecName = src.m_codecName;
  m_ptime = src.m_ptime;
  m_playFile = src.m_playFile;
  m_clientIp = src.m_clientIp;
  m_clientPort = src.m_clientPort;
  m_serverIp = src.m_serverIp;
  m_serverPort = src.m_serverPort;
  m_relayIp = src.m_relayIp;
  m_relayPort = src.m_relayPort;
  m_rflxIp = src.m_rflxIp;
  m_rflxPort = src.m_rflxPort;
  m_secureDetail = src.m_secureDetail;
  m_dialIp = src.m_dialIp;
  m_dialPort = src.m_dialPort;
  m_respIp = src.m_respIp;
  m_respPort = src.m_respPort;
  return *this;
}

VideoDetail::VideoDetail():
  m_mediaType(0), m_fmtp(""), m_codec(0), m_codecName(""), m_frameRate(0),
  m_imageattr_x(0), m_imageattr_y(0), m_playFile(""), m_clientIp(), m_clientPort(0),
  m_serverIp(""), m_serverPort(0), m_relayIp(""), m_relayPort(0), m_rflxIp(""), m_rflxPort(0),
  m_dialIp(""), m_dialPort(0), m_respIp(""), m_respPort(0)
{
}

VideoDetail::~VideoDetail()
{
}

VideoDetail& VideoDetail::operator=(VideoDetail &src)
{
  m_mediaType = src.m_mediaType;
  m_fmtp = src.m_fmtp;
  m_codec = src.m_codec;
  m_codecName = src.m_codecName;
  m_frameRate = src.m_frameRate;
  m_imageattr_x = src.m_imageattr_x;
  m_imageattr_y = src.m_imageattr_y;
  m_playFile = src.m_playFile;
  m_clientIp = src.m_clientIp;
  m_clientPort = src.m_clientPort;
  m_serverIp = src.m_serverIp;
  m_serverPort = src.m_serverPort;
  m_relayIp = src.m_relayIp;
  m_relayPort = src.m_relayPort;
  m_rflxIp = src.m_rflxIp;
  m_rflxPort = src.m_rflxPort;
  m_secureDetail = src.m_secureDetail;
  m_dialIp = src.m_dialIp;
  m_dialPort = src.m_dialPort;
  m_respIp = src.m_respIp;
  m_respPort = src.m_respPort;
  return *this;
}


SslCertificate::SslCertificate() :
  pkey(NULL), x509(NULL), exponent(NULL), rsa(NULL)
{
}

SslCertificate::~SslCertificate()
{
   pkey = NULL;
   x509 = NULL;
   exponent = NULL;
   rsa = NULL;
}

/*  MediaSecureDetail class implementation - start
*/
MediaSecureDetail::MediaSecureDetail() :
  m_lIceUfrag(""), m_lIcePwd(""), m_rIceUfrag(""), m_rIcePwd(""),
  m_lFingerPrint(""), m_rFingerPrint(""), m_pemFile(""), m_lSsrc(""),
  m_rSsrc(""), m_lCname(""), m_rCname(""), m_lMsLabel(""), m_rMsLabel(""),
  m_lLabel(""), m_rLabel("")
{
}

MediaSecureDetail::~MediaSecureDetail()
{
}

MediaSecureDetail& MediaSecureDetail::operator=(MediaSecureDetail &src)
{
  m_candidateDetail = src.m_candidateDetail;
  m_pemFile = src.m_pemFile;
  m_lIceUfrag = src.m_lIceUfrag;
  m_lIcePwd = src.m_lIcePwd;
  m_rIceUfrag = src.m_rIceUfrag;
  m_rIcePwd = src.m_rIcePwd;
  m_lFingerPrint = src.m_lFingerPrint;
  m_rFingerPrint = src.m_rFingerPrint;
  m_pemFile = src.m_pemFile;
  m_lSsrc = src.m_lSsrc;
  m_rSsrc = src.m_rSsrc;
  m_lCname = src.m_lCname;
  m_rCname = src.m_rCname;
  m_lMsLabel = src.m_lMsLabel;
  m_rMsLabel = src.m_rMsLabel;
  m_lLabel = src.m_lLabel;
  m_rLabel = src.m_rLabel;
  return *this;
}

