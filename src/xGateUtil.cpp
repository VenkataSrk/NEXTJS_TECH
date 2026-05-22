#include "xglog.h"
#include "xGateUtil.h"
#include "URCBController.h"
#include "xGateSchedulerService.h"
#include "xGateSipService.h"

#include <netinet/in.h>
#include <netdb.h>
#include <ifaddrs.h>

char xGateUtil::m_localIp4Addr[INET_ADDRSTRLEN] = {0};
char xGateUtil::m_localIp6Addr[INET6_ADDRSTRLEN] = {0};
char *xGateUtil::LocalIPadress = NULL;

xGateUtil::xGateUtil()
{
}

xGateUtil::~xGateUtil(void)
{
}

bool xGateUtil::post(xGateBaseMsg *pBaseMsg)
{
  IURDefines::MODULE_ID dstId = pBaseMsg->getDstModuleId();

  ACE_Message_Block* pAmb = NULL;
  pAmb = static_cast<xGateBaseMsg*>(pBaseMsg);
  ACE_Time_Value tval ((time(NULL)));
  tval += ACE_Time_Value(0,1);

  string serviceName("");
  string uid = pBaseMsg->getUid();
  ACE_Task<ACE_MT_SYNCH>* taskPtr = NULL;
  switch(dstId) {
    case IURDefines::UR_MODULE_SIPSIGNAL_SERVICE:
    {
      serviceName = "SipService";
      break; 
    }
    case IURDefines::UR_MODULE_SERVICE_CONTROLLER:
    {
      serviceName = "URConfBridgeController";
      taskPtr = URCBCONTROLLER; 
      break; 
    }
    case IURDefines::UR_MODULE_SCHEDULER_SERVICE:
    {
      serviceName = "SchedulerService";
      taskPtr = SCHEDULERSERVICE;
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

xGateRetVal xGateUtil::findLocalIpAddr(char *ipAddr, unsigned short AddrType)
{
  struct ifaddrs * ifAddrStruct=NULL;
  struct ifaddrs * ifa=NULL;
  void * tmpAddrPtr=NULL;

  if(ipAddr == NULL) {
    return EN_XGATE_STATUS_ERROR;
  }

  getifaddrs(&ifAddrStruct);
  for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
    if (!ifa->ifa_addr || \
        strcmp(ifa->ifa_name, "lo") == 0 || \
        ifa->ifa_addr->sa_family != AddrType) {
      continue;
    }
    std::string startStr = "fe80";
    if (AF_INET == AddrType) { // check it is IP4
      // is a valid IP4 Address
      tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
      inet_ntop(AF_INET, tmpAddrPtr, ipAddr, INET_ADDRSTRLEN);
      printf("xGateUtil::findLocalIpAddr, %s IP4 Address %s\n", ifa->ifa_name, ipAddr);
    } else if (AF_INET6 == AddrType) { // check it is IP6
      // is a valid IP6 Address
      tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
      inet_ntop(AF_INET6, tmpAddrPtr, ipAddr, INET6_ADDRSTRLEN);
      if (std::string(ipAddr).substr(0, startStr.length()) == startStr) continue;
      printf("xGateUtil::findLocalIpAddr, %s IP6 Address %s\n", ifa->ifa_name, ipAddr);
      break;
    }
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
  return nullptr;
}

#if 0
bool xGateUtil::create_thread(void (*ptFunction)(void *),string class_name)
{
  ACE_thread_t tid;
  if(ACE_Thread_Manager::instance()->spawn((ACE_THR_FUNC)ptFuntion,THR_NEW_LWP|THR_DETACHED,&tid)< 0)
  {
   return true;
  }
  else 
  {
   XGLOG_ERROR("create_thread failed !");  
  }
}
#endif
