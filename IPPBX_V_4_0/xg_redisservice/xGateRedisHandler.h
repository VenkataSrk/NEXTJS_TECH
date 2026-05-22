#ifndef _XGATE_REDIS_HANDLER_H
#define _XGATE_REDIS_HANDLER_H

#pragma once
#include<ace/Task.h>
#include <ace/Singleton.h>
#include <map>
#include <vector>
#include <thread>

//local includes
#include "IURModule.h"
#include "xGateUtil.h"
#include "RedisServiceMsg.h"
#include "xGConfig.h"

//redis includes
#include <sw/redis++/redis++.h>

using namespace sw::redis;

class xGateRedisHandler 
{
   public:
      xGateRedisHandler();
      virtual ~xGateRedisHandler();
      
      int init(void);
      void consume_msg();
      bool process_msg(RedisServiceMsg *pMsg);
      bool process_msg_from_controller(RedisServiceMsg *pBaseMsg);

   private:
      //member functions
      ConnectionOptions create_redis_pub_connection();
      ConnectionOptions set_sub_connection_opts(RedisProfile &redisProfile);
      bool sendSubscribeNotifyMsgToCtrl(std::string msg);
      bool sendGetResponseMsgToCtrl(std::string ctx_id,std::string key,std::string value,RedisMsgType redisMType);
      bool sendSetResponseMsgToCtrl(std::string ctx_id,RedisMsgType redisMType);
      bool sendErrorResponseToCtrl(std::string ctx_id,RedisMsgType redisMType);

      bool sendConnFailedNotify();

      //member variables
      ConnectionOptions m_pubConn;
      //RedisCluster m_pubRedis;
      Redis m_pubRedis;
      RedisProfile m_pubRedisConnInfo;
};

#endif//_XGATE_REDIS_HANDLER_H

