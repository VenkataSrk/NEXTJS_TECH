#ifndef _REDIS_SERVICE_MSG_H
#define _REDIS_SERVICE_MSG_H

//ace includes
#include <ace/Message_Block.h>

//local includes
#include "xGateUtil.h"
#include "RedisServiceDefine.h"
#include "xGBaseMsg.h"

class RedisServiceMsg : public xGBaseMsg
{
public:
   RedisServiceMsg(void) : xGBaseMsg(IURDefines::UR_MODULE_REDIS_SERVICE){};
   virtual ~RedisServiceMsg(){};
   RedisInfo &get_redis_info()
   {
      return m_redisInfo;
   }
   void set_redis_info(RedisInfo &redisInfo)
   {
      m_redisInfo = redisInfo;
   }
   RedisServiceEvent get_redis_event()
   {
      return m_enRedisEvent;  
   }
   RedisServiceEvent set_redis_event(RedisServiceEvent redisevent)
   {
      m_enRedisEvent = redisevent;
   }
   RedisMsgType get_redis_msg_type()
   {
      return m_enRedisMsgType;  
   }
   RedisMsgType set_redis_msg_type(RedisMsgType redisMsgType)
   {
      m_enRedisMsgType = redisMsgType;
   }
   std::string err_msg;
protected:
   RedisServiceMsg &operator=(const RedisServiceMsg &);

private:
   RedisServiceMsg(const RedisServiceMsg &);
   RedisServiceEvent m_enRedisEvent;
   RedisMsgType m_enRedisMsgType;
   RedisInfo m_redisInfo;
};
#endif

