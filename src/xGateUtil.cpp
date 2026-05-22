#include "xglog.h"
#include "xGateUtil.h"
#include "xGateSGWController.h"
#include "xGateSipService.h"
#include "xGateMonitorService.h"

#include <netinet/in.h>
#include <netdb.h>
#include <ifaddrs.h>

char xGateUtil::m_localIp4Addr[INET_ADDRSTRLEN] = {0};
char xGateUtil::m_localIp6Addr[INET6_ADDRSTRLEN] = {0};
char *xGateUtil::LocalIPadress = nullptr;

xGateUtil::xGateUtil()
{
}

xGateUtil::~xGateUtil(void)
{
}

bool xGateUtil::post(xGateBaseMsg *pBaseMsg)
{
  IURDefines::MODULE_ID dstId = pBaseMsg->getDstModuleId();

  ACE_Message_Block* pAmb = nullptr;
  pAmb = static_cast<xGateBaseMsg*>(pBaseMsg);
  ACE_Time_Value tval ((time(nullptr)));
  tval += ACE_Time_Value(0,1);

  string serviceName("");
  string uid = pBaseMsg->getUid();
  ACE_Task<ACE_MT_SYNCH>* taskPtr = nullptr;
  switch(dstId) {
    case IURDefines::UR_MODULE_SIPSIGNAL_SERVICE:
    {
      serviceName = "SipService";
      taskPtr = SIPSIGSERVICE;
      break;
    }
    case IURDefines::UR_MODULE_SERVICE_CONTROLLER:
    {
      serviceName = "SGWController";
      taskPtr = SGWCONTROLLER; 
      break; 
    }
    case IURDefines::UR_MODULE_MONITOR_SERVICE:
    {
      serviceName = "MonitorService";
      taskPtr = MNTRSERVICE;
      break;
    }
    case IURDefines::UR_MODULE_UNKNOWN:
    default:
    {
      XGLOG_ERROR("post message faile. Unknown/Unsupported %s destination module !", serviceName.c_str());
      return false; 
    }
  }

  if(!taskPtr) {
    XGLOG_ERROR("post message to '%s' failed for uid: %s !", serviceName.c_str(), uid.c_str());
    return false;
  }

  if(taskPtr->putq(pAmb, &tval)) {
    XGLOG_DEBUG("successfully posted message to %s", serviceName.c_str());
  } else {
    XGLOG_ERROR("posting message to %s failed !", serviceName.c_str());
    return false;
  }

  return true;
}

bool xGateUtil::parseJson(const char *jsonData, Document &doc)
{
  ParseResult ok = doc.Parse(jsonData);
  if(!ok) {
    XGLOG_ERROR("parseJson failed due to parse error: %s !", GetParseError_En(ok.Code()));
    return false;
  } else if(!doc.IsObject()) {
    XGLOG_ERROR("parseJson failed while creating json documnet object !");
    return false;
  }
  return true;
}

Value xGateUtil::setJsonParam(const string &strValue, Document &doc)
{
  Value retVal;
  retVal.SetString(strValue.c_str(), strValue.length(), doc.GetAllocator());
  return retVal;
}

Value xGateUtil::setJsonParam(unsigned int iValue)
{
  Value retVal;
  retVal = iValue;
  return retVal;
}

xGateRetVal xGateUtil::findLocalIpAddr(char *ipAddr, unsigned short AddrType, xGateInterfaceType eInterfaceType)
{
  struct ifaddrs * ifAddrStruct=nullptr;
  struct ifaddrs * ifa=nullptr;
  void * tmpAddrPtr=nullptr;

  if(ipAddr == nullptr) {
    return EN_XGATE_STATUS_ERROR;
  }

  getifaddrs(&ifAddrStruct);
  for (ifa = ifAddrStruct; ifa != nullptr; ifa = ifa->ifa_next) {
    if (!ifa->ifa_addr || \
        strcmp(ifa->ifa_name, "lo") == 0 || \
        ifa->ifa_addr->sa_family != AddrType) {
      continue;
    }
    std::string startStr = "fe80";
    //std::string inetStartStr = "10";
    if (AF_INET == AddrType) { // check it is IP4
      if (eInterfaceType == EN_XGATE_INTERFACE_PRIVATE)
      {
        tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
        inet_ntop(AF_INET, tmpAddrPtr, ipAddr, INET_ADDRSTRLEN);
        //if (std::string(ipAddr).substr(0, inetStartStr.length()) == inetStartStr) //Commented by Harshith
        if(isPrivateIPv4(std::string(ipAddr)))
        {
          XGLOG_INFO("%s(), Got private IP: %s",__func__,ipAddr);
          printf("xGateUtil::findLocalIpAddr, %s Private IP4 Address %s\n", ifa->ifa_name, ipAddr);
          break;
        }
      }
      else if (eInterfaceType == EN_XGATE_INTERFACE_PUBLIC)
      {
        // is a valid IP4 Address
        tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
        inet_ntop(AF_INET, tmpAddrPtr, ipAddr, INET_ADDRSTRLEN);
        printf("xGateUtil::findLocalIpAddr, %s IP4 Address %s\n", ifa->ifa_name, ipAddr);
      }
    } else if (AF_INET6 == AddrType) { // check it is IP6
      // is a valid IP6 Address
      tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
      inet_ntop(AF_INET6, tmpAddrPtr, ipAddr, INET6_ADDRSTRLEN);
      if (std::string(ipAddr).substr(0, startStr.length()) == startStr) continue;
      printf("xGateUtil::findLocalIpAddr, %s IP6 Address %s\n", ifa->ifa_name, ipAddr);
      break;
    }
  }
  if (ifAddrStruct!=nullptr) freeifaddrs(ifAddrStruct);
  return EN_XGATE_STATUS_SUCCESS;
}



char * xGateUtil::getLocalIpAddr(unsigned short AddrType, xGateInterfaceType eInterfaceType) {
  switch(AddrType) {
    case AF_INET:
      {
        if(m_localIp4Addr[0] != 0) {
          XGLOG_INFO("%s() Local ip: %s", __func__, m_localIp4Addr);
          return m_localIp4Addr;
        } else {
          findLocalIpAddr(m_localIp4Addr, AF_INET, eInterfaceType);
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
  return nullptr;
}

std::string xGateUtil::generateTransid()
{
  uuid_t id;
  size_t len;

  uuid_generate(id);

  char _string[100];
  memset(_string, 0, sizeof(_string));
  uuid_unparse(id, _string);
  len = strlen(_string);

  // cout << _string << std::endl;
  return UUIDToSessionId((char *)_string, len);
}

std::string xGateUtil::UUIDToSessionId(const char *uuid, const size_t len)
{
  char session_uuid[33] = {0}; /*"local-uuid", per rfc7989*/
  if ((!uuid) || len != 36)
        return nullptr;

  for (size_t i = 0, j = 0; i < len; i++)
  {
        if (uuid[i] != '-')
          session_uuid[j++] = uuid[i];
  }
  session_uuid[len + 1] = '\0';

  if (!isValidSessionID(session_uuid, strlen(session_uuid)))
        return nullptr;
  std::string s(session_uuid);

  return s;
}

bool xGateUtil::isValidSessionID(const char *uuid, const size_t len)
{
  for (size_t i = 0; i < len; i++)
  {
        if ((uuid[i] < '0' || uuid[i] > '9') && (uuid[i] < 'a' || uuid[i] > 'f'))
          return false;
  }
  return true;
}

//ToDo::Harshith: Matching private IP pattern
bool xGateUtil::isPrivateIPv4(std::string ip)
{
  XGLOG_FUNCTION_EN_EX("");
    struct in_addr addr;
    if (inet_pton(AF_INET, ip.c_str(), &addr) != 1)
        return false;

    uint32_t ipInt = ntohl(addr.s_addr);

    // Standard private ranges
    if ((ipInt & 0xFF000000) == 0x0A000000) {XGLOG_INFO("%s() matches 10.0.0.0/8",__func__); return true;}          // 10.0.0.0/8
    if ((ipInt & 0xFFF00000) == 0xAC100000) {XGLOG_INFO("%s() matches 172.16.0.0/12",__func__); return true;}      // 172.16.0.0/12
    if ((ipInt & 0xFFFF0000) == 0xC0A80000) {XGLOG_INFO("%s() matches 192.168.0.0/16",__func__); return true;}     // 192.168.0.0/16

    // Optional extra: treat 20.0.0.0/8 as private for your environment
    if ((ipInt & 0xFF000000) == 0x14000000) {XGLOG_INFO("%s() matches 20.0.0.0/8",__func__); return true;}          // 20.0.0.0/8

    return false;
}