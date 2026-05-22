#ifndef _REDIS_SERVICE_DEFINE_H
#define _REDIS_SERVICE_DEFINE_H

#include "IURDefines.h"

typedef enum
{
   EN_REDIS_EVENT_UNKNOWN=0,
   EN_REDIS_EVENT_PUBLISH,
   EN_REDIS_EVENT_SUBSCRIBE_NOTIFY,
   EN_REDIS_EVENT_SET,
   EN_REDIS_EVENT_GET,
   EN_REDIS_EVENT_RPUSH,
   EN_REDIS_EVENT_SET_RESPONSE,
   EN_REDIS_EVENT_GET_RESPONSE,
   EN_REDIS_EVENT_ERROR_RESPONSE,
} RedisServiceEvent;

typedef enum
{
   EN_REDIS_MSG_TYPE_GET_REG_PWD = 0,
   EN_REDIS_MSG_TYPE_INSERT_AOR = 1,
   EN_REDIS_MSG_TYPE_GET_INV_PWD = 2,
   EN_REDIS_MSG_TYPE_RPUSH_REC_DATA = 3
} RedisMsgType;

class RedisInfo
{
public:
   RedisInfo()
   {
      m_strContextId = "";
      m_enReqModule = IURDefines::MODULE_ID::UR_MODULE_REDIS_SERVICE;
      m_strMsg = "";
      m_strKey = "";
      m_strValue = "";
   }
   ~RedisInfo()
   {
      m_strContextId = "";
      m_enReqModule = IURDefines::MODULE_ID::UR_MODULE_REDIS_SERVICE;
      m_strMsg = "";
      m_strKey = "";
      m_strValue = "";
   }
   RedisInfo(const RedisInfo &rhs)
   {
      m_strContextId = rhs.m_strContextId.c_str();
      m_enReqModule = rhs.m_enReqModule;
      m_strMsg = rhs.m_strMsg.c_str(); 
      m_strKey = rhs.m_strKey.c_str();
      m_strValue = rhs.m_strValue.c_str();
   }
   RedisInfo &operator=(const RedisInfo &rhs)
   {
      m_strContextId = rhs.m_strContextId.c_str();
      m_enReqModule = rhs.m_enReqModule;
      m_strMsg = rhs.m_strMsg.c_str();
      m_strKey = rhs.m_strKey.c_str();
      m_strValue = rhs.m_strValue.c_str();
      return *this;
   }

   std::string m_strContextId;
   IURDefines::MODULE_ID m_enReqModule;
   std::string m_strMsg;
   //for Redis
   std::string m_strKey;
   std::string m_strValue;
};
#endif

