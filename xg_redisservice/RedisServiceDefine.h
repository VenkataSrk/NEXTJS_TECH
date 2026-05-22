#ifndef _REDIS_SERVICE_DEFINE_H
#define _REDIS_SERVICE_DEFINE_H

#include "IURDefines.h"

typedef enum
{
   EN_REDIS_EVENT_UNKNOWN=0,
   EN_REDIS_EVENT_PUBLISH_TO_UCAAS_SIO,
   EN_REDIS_EVENT_PUBLISH_TO_CONFBRIDGE,
   EN_REDIS_EVENT_PUBLISH_TO_CCAAS_SIO,
   EN_REDIS_EVENT_PUBLISH_TO_ACD,
   EN_REDIS_EVENT_SUBSCRIBE,
   EN_REDIS_EVENT_GET_ACD_CHANNEL,
   EN_REDIS_EVENT_SET_ACD_CHANNEL,
} RedisServiceEvent;

class RedisInfo
{
   public:
      RedisInfo()
      {
        m_strContextId = "";
        m_enRedisEvent = EN_REDIS_EVENT_UNKNOWN;
        m_enReqModule = IURDefines::MODULE_ID::UR_MODULE_REDIS_SERVICE;
	m_enRedisMethod = EN_REDIS_METHOD::EN_REDIS_METHOD_UNKNOWN;
	m_strRedisIp = "";
        m_strChannel = "";
        m_strMsg = "";
	m_strKey = "";
	m_strValue = "";
      }
      ~RedisInfo()
      {
        m_strContextId = "";
        m_enRedisEvent = EN_REDIS_EVENT_UNKNOWN;
        m_enReqModule = IURDefines::MODULE_ID::UR_MODULE_REDIS_SERVICE;
	m_enRedisMethod = EN_REDIS_METHOD::EN_REDIS_METHOD_UNKNOWN;
	m_strRedisIp = "";
        m_strChannel = "";
        m_strMsg = "";
	m_strKey = "";
	m_strValue = "";
      }
      RedisInfo(const RedisInfo& rhs)
      {
	 m_strContextId = rhs.m_strContextId.c_str();
	 m_enRedisEvent = rhs.m_enRedisEvent;
	 m_enReqModule = rhs.m_enReqModule;
	 m_enRedisMethod = rhs.m_enRedisMethod;
	 m_strRedisIp = rhs.m_strRedisIp.c_str();
         m_strChannel = rhs.m_strChannel.c_str();
         m_strMsg = rhs.m_strMsg.c_str();
	 m_strKey = rhs.m_strKey.c_str();
	 m_strValue = rhs.m_strValue.c_str();
      }
      RedisInfo& operator = (const RedisInfo& rhs)
      {
	 m_strContextId = rhs.m_strContextId.c_str();
	 m_enRedisEvent = rhs.m_enRedisEvent;
	 m_enReqModule = rhs.m_enReqModule;
	 m_enRedisMethod = rhs.m_enRedisMethod;
	 m_strRedisIp = rhs.m_strRedisIp.c_str();
         m_strChannel = rhs.m_strChannel.c_str();
         m_strMsg = rhs.m_strMsg.c_str();
	 m_strKey = rhs.m_strKey.c_str();
	 m_strValue = rhs.m_strValue.c_str();
   	 return *this;
      }

      std::string m_strContextId;
      RedisServiceEvent m_enRedisEvent;
      IURDefines::MODULE_ID m_enReqModule;
      EN_REDIS_METHOD m_enRedisMethod;
      std::string m_strRedisIp;
      std::string m_strChannel;
      std::string m_strMsg;
      std::string m_strKey;
      std::string m_strValue;
};
#endif

