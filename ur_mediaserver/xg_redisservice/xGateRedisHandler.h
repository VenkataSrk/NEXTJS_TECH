#ifndef _XGATE_REDIS_HANDLER_H
#define _XGATE_REDIS_HANDLER_H

#pragma once
#include<ace/Task.h>
#include <ace/Singleton.h>
#include <map>
#include <vector>
#include <thread>

//local includes
#include "XGConfig.h"
#include "IURModule.h"
#include "xGateUtil.h"
#include "RedisServiceMsg.h"
#include "XG_SFUController.h"

//redis includes
#include <sw/redis++/redis++.h>

using namespace sw::redis;

class xGateRedisHandler 
{
   public:
      xGateRedisHandler(RedisConnInfo * redisConnInfo);
      virtual ~xGateRedisHandler();
      
      int init(void);
      void consume_msg();
      bool process_msg(RedisServiceMsg *pMsg);
      bool process_msg_from_controller(RedisServiceMsg *pBaseMsg);
      RedisConnInfo * m_redisConnInfo;

   private:
      //member functions
      ConnectionOptions create_redis_pub_connection();
      ConnectionOptions set_sub_connection_opts(RedisConnInfo &redisConnInfo);
      bool postMsgToController(std::string msg, std::string channel);
      bool postGetResponseToController(RedisInfo &t_redisInfo);
      bool sendConnFailedNotify();

      //member variables
      ConnectionOptions m_pubConn;
      Redis m_pubRedis;
      //RedisCluster m_pubRedis;
      RedisConnInfo m_pubSocketRedisConnInfo;
      RedisConnInfo m_pubConfBridgeRedisConnInfo;

      public:
      std::vector<std::string> m_vsubChannel;
};

#endif//_XGATE_REDIS_HANDLER_H

