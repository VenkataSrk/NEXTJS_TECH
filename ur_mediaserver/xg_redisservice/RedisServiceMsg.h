#ifndef _REDIS_SERVICE_MSG_H
#define _REDIS_SERVICE_MSG_H

//ace includes
#include <ace/Message_Block.h>

//local includes
#include "xGateUtil.h"
#include "xGateBaseMsg.h"
#include "RedisServiceDefine.h"

class RedisServiceMsg : public xGateBaseMsg
{
   public:
      RedisServiceMsg(void) :
	  xGateBaseMsg(IURDefines::UR_MODULE_REDIS_SERVICE)
      {};
      virtual ~RedisServiceMsg()
      {};
      RedisInfo& get_redis_info() 
      {
	 return m_redisInfo;
      }
      void set_redis_info(RedisInfo &redisInfo) 
      {
	 m_redisInfo = redisInfo;
      }

   protected:
      RedisServiceMsg& operator=(const RedisServiceMsg&);

   private:
      RedisServiceMsg(const RedisServiceMsg&);
      RedisInfo m_redisInfo;
};
#endif

