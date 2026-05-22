
#include "xglog.h"
#include "MrcpRedisDbSubscriber.h"
#include "MrcpRedisDbConnector.h"
#include "URMRCPModuleBridge.h"

MrcpRedisDbSubscriber::MrcpRedisDbSubscriber() :
	ACE_Task<ACE_MT_SYNCH>(),
	m_run(false), m_redisDbConfig(NULL)
{
	m_strModuleName = "MrcpDbSubscriber";
}

MrcpRedisDbSubscriber::~MrcpRedisDbSubscriber(void)
{
}

int MrcpRedisDbSubscriber::initModule(IURModuleConfig * pdbConfig)
{
	XGLOG_INFO("MrcpRedisDbSubscriber initModule initialized !");  
	m_redisDbConfig = pdbConfig;
	if(!init()) {
		return IURDefines::UR_RETURN_ERROR;
	}
	return IURDefines::UR_RETURN_SUCCESS;
}

int MrcpRedisDbSubscriber::unitModule()
{
	stop();
	return IURDefines::UR_RETURN_SUCCESS;
}

void MrcpRedisDbSubscriber::getModuleName(std::string& moduleName){
	moduleName=m_strModuleName;
}

IURDefines::MODULE_ID MrcpRedisDbSubscriber::getModuleID(){
	return IURDefines::UR_MODULE_MRCP_REDIS_DB_SUBSCRIBE;
}

IURModuleConfig * MrcpRedisDbSubscriber::getModuleConfig(){
	return m_redisDbConfig;
}

bool MrcpRedisDbSubscriber::stop(void)
{
	m_run = false;
	msg_queue()->deactivate();
	wait();
	XGLOG_INFO("MrcpRedisDbSubscriber stopped !");
	return true;
}

bool MrcpRedisDbSubscriber::init()
{
  XGLOG_INFO("MrcpRedisDbSubscriber::init() called");

  botRedisInfo = MRCPCONFIGSERVICE->m_redisProfile.m_mapSubRedisConnInfo[EN_REDIS_TYPE::EN_REDIS_TYPE_CCAAS_BOT];
  ccaasRedisInfo = MRCPCONFIGSERVICE->m_redisProfile.m_mapSubRedisConnInfo[EN_REDIS_TYPE::EN_REDIS_TYPE_CCAAS_ACD];
  sttRedisInfo = MRCPCONFIGSERVICE->m_redisProfile.m_mapSubRedisConnInfo[EN_REDIS_TYPE::EN_REDIS_TYPE_CCAAS_VMS_STT];

  if(!botRedisInfo && !ccaasRedisInfo && !sttRedisInfo)
  {
	XGLOG_ERROR("MrcpRedisDbSubscriber::init() failed redis conf info is missing");
	return false;
  }

  std::map<EN_REDIS_TYPE,RedisConnInfo*> redisInfo = MRCPCONFIGSERVICE->m_redisProfile.m_mapSubRedisConnInfo;

  for(auto it : redisInfo)
  {
	switch(it.second->m_enMsgType)
	{
	  case EN_REDIS_MSG_TYPE::EN_REDIS_MSG_TYPE_PUBSUB:
	  {
		if(!consume_from_subscribe_channel(it.second))
		{
		  XGLOG_ERROR("MrcpRedisDbSubscriber::init subscribed failed");
		}
		break;
	  }
	  case EN_REDIS_MSG_TYPE::EN_REDIS_MSG_TYPE_PUSHPOP:
	  {
		if(!consume_from_pop_channel(it.second))
		{
		  XGLOG_ERROR("MrcpRedisDbSubscriber::init pop failed");
		}
		break;
	  }
	}
  }


  return true;
}

bool MrcpRedisDbSubscriber::consume_from_subscribe_channel(RedisConnInfo* redisInfo)
{
  XGLOG_INFO("MrcpRedisDbSubscriber::consume_from_subscribe_channel() called");
  std::thread redisThrd([=]()
   {
	  if(!redisInfo->m_strServerHost.empty())
	  {
		 reloadSub:
		 try
		 {
			int cluster = redisInfo->m_iCluster;
			ConnectionOptions connectionOptions = connectToRedis(redisInfo);
			if(cluster==1)
			{
			   RedisCluster subRedis(connectionOptions);
			   auto sub = subRedis.subscriber();
			   sub.on_message([this](std::string channel, std::string msg)
			   {
				  cout<<"!! Channel - " << channel.c_str() << " msg - " << msg.c_str() << endl;
				  std::string text = msg.c_str();
				  findAndTrim(text);
				  XGLOG_INFO("!! Channel (%s) Msg (%s) ",channel.c_str(),text.c_str());
				  if((!strncmp(channel.c_str(),botRedisInfo->m_strChannel.c_str(),botRedisInfo->m_strChannel.length())) || (!strncmp(channel.c_str(),ccaasRedisInfo->m_strChannel.c_str(),ccaasRedisInfo->m_strChannel.length())))
				  {
					handle_redis_response(msg);
				  }
				  else if(!strncmp(channel.c_str(),sttRedisInfo->m_strChannel.c_str(),sttRedisInfo->m_strChannel.length()))
				  {
					handle_redis_vms_request(msg);
				  }
			   });
			   
			   for(auto channel : redisInfo->m_vChannel)
			   {
				sub.subscribe(channel.c_str());
			   }
			   cout<<"!! Subscribed Channel - " << redisInfo->m_strChannel.c_str();
			   XGLOG_INFO("!! Subscribed Channel (%s)  ",redisInfo->m_strChannel.c_str());
			   
			   while(true) 
			   {
				  try 
				  {
					 sub.consume();
				  }
				  catch (const TimeoutError &e)
				  {
					 continue;
				  } 
				  catch (const Error &err)
				  {
					 XGLOG_ERROR("subscribeChannel redis consume message failed. Error - %s",err.what());
					 usleep(5000000);
					 goto reloadSub;
				  }
			   }
			   
				XGLOG_DEBUG("subscribeChannel channel unsubscribed - %s",redisInfo->m_strChannel.c_str());
				sub.unsubscribe(redisInfo->m_strChannel.c_str());               
			}
			else
			{
			   Redis subRedis(connectionOptions);
			   auto sub = subRedis.subscriber();
			   sub.on_message([this](std::string channel, std::string msg)
			   {
				  cout<<"!! Channel - " << channel.c_str() << " msg - " << msg.c_str() << endl;
				  std::string text = msg.c_str();
				  findAndTrim(text);
				  XGLOG_INFO("!! Channel (%s) Msg (%s) ",channel.c_str(),text.c_str());
				  if(!strncmp(channel.c_str(),sttRedisInfo->m_vChannel[0].c_str(),sttRedisInfo->m_vChannel[0].length()))
				  {
					handle_redis_vms_request(msg);
				  }
				  if(!strncmp(channel.c_str(),sttRedisInfo->m_vChannel[1].c_str(),sttRedisInfo->m_vChannel[1].length()))
				  {
					  handle_redis_vms_response(msg);
				  }
				  else
				  {
					handle_redis_response(msg);
				  }
			   });

			   for(auto channel : redisInfo->m_vChannel)
			   {
				XGLOG_INFO("!! Subscribed Channel (%s)  ",channel.c_str());
				sub.subscribe(channel.c_str());
			   }
			   cout<<"!! Subscribed Channel - " << redisInfo->m_strChannel.c_str() << endl;
			   XGLOG_INFO("!! Subscribed Channel (%s)  ",redisInfo->m_strChannel.c_str());

			   while(true) 
			   {
				  try 
				  {
					 sub.consume();
				  } 
				  catch (const TimeoutError &e)
				  {
					 continue;
				  } 
				  catch (const Error &err)
				  {
					 XGLOG_ERROR("subscribeChannel redis consume message failed. Error - %s",err.what());
					 usleep(5000000);
					 goto reloadSub;
				  }
			   }

			   XGLOG_DEBUG("subscribeChannel channel unsubscribed - %s",redisInfo->m_strChannel.c_str());
			   sub.unsubscribe(redisInfo->m_strChannel.c_str());
			}
		 }
		 catch(const Error &err)
		 {
			XGLOG_ERROR("subscribeChannel redis subscribe to channel failed. Exception caught - %s",err.what());
			usleep(5000000);
			goto reloadSub;
		 }
	  }
   });
   if(redisThrd.joinable())
   {
	  redisThrd.detach();
   }
	return true;
}

bool MrcpRedisDbSubscriber::findAndTrim(std::string &text)
{
  XGLOG_INFO("MrcpRedisDbSubscriber::findAndTrim");
  std::string del("%");
  size_t found = text.find(del);
  while(found != string::npos)
  {
	  text.erase(text.begin() + found);
	  found = text.find(del);
  }
}

bool MrcpRedisDbSubscriber::consume_from_pop_channel(RedisConnInfo* redisInfo)
{
  #if 0
  std::thread redisThrd([=]()
  {
	if(!redisInfo->m_strServerHost.empty())
	{
	  reloadSub:
		 try
		 {
			int cluster = redisInfo->m_iCluster;
			int timeoutSeconds = 0;
			ConnectionOptions connectionOptions = connectToRedis(redisInfo);
			std::string msg("");
			if(cluster==1)
			{
			  while(true)
			  {                
				RedisCluster subRedis(connectionOptions);
				auto val = subRedis.blpop(redisInfo->m_strChannel,timeoutSeconds);
				if(val.has_value())
				{
				  std::string key = val.value().first;
				  msg = val.value().second;
				  cout<<"!! key - "<<key<<" msg - "<<msg<<endl;
				}
				if((!strncmp(redisInfo->m_strChannel.c_str(),botRedisInfo->m_strChannel.c_str(),botRedisInfo->m_strChannel.length())) || (!strncmp(redisInfo->m_strChannel.c_str(),ccaasRedisInfo->m_strChannel.c_str(),ccaasRedisInfo->m_strChannel.length())))
				{
					handle_redis_response(msg);
				}
				else if(!strncmp(redisInfo->m_strChannel.c_str(),botRedisInfo->m_strChannel.c_str(),botRedisInfo->m_strChannel.length()))
				{
				  handle_redis_vms_request(msg);
				}
			  }
			}
			else 
			{
			  while(true)
			  {                
				Redis subRedis(connectionOptions);
				auto val = subRedis.blpop(redisInfo->m_strChannel,timeoutSeconds);
				if(val.has_value())
				{
				  std::string key = val.value().first;
				  msg = val.value().second;
				  cout<<"!! key - "<<key<<" msg - "<<msg<<endl;
				}
				if((!strncmp(redisInfo->m_strChannel.c_str(),botRedisInfo->m_strChannel.c_str(),botRedisInfo->m_strChannel.length())) || (!strncmp(redisInfo->m_strChannel.c_str(),ccaasRedisInfo->m_strChannel.c_str(),ccaasRedisInfo->m_strChannel.length())))
				{
					handle_redis_response(msg);
				}
				else if(!strncmp(redisInfo->m_strChannel.c_str(),sttRedisInfo->m_strChannel.c_str(),sttRedisInfo->m_strChannel.length()))
				{
				  handle_redis_vms_request(msg);
				}
			  }
			}
		 }
		 catch(const Error &err)
		 {
			XGLOG_ERROR("subscribeChannel redis subscribe to channel failed. Exception caught - %s",err.what());
			usleep(5000000);
			goto reloadSub;
		 }
	}
  })
  #endif
}

ConnectionOptions MrcpRedisDbSubscriber::connectToRedis(RedisConnInfo* redisInfo)
{
   ConnectionOptions m_pConnectionOptions;
   m_pConnectionOptions.host = redisInfo->m_strServerHost.c_str();
   m_pConnectionOptions.port = (redisInfo->m_iServerPort > 0) ? redisInfo->m_iServerPort : 6379;
   if(!redisInfo->m_strPassword.empty())
   {
	  m_pConnectionOptions.password = redisInfo->m_strPassword.c_str();
   }
   m_pConnectionOptions.socket_timeout = (redisInfo->m_iSocketTimeout > 0) ? std::chrono::milliseconds(redisInfo->m_iSocketTimeout) : std::chrono::milliseconds(0);
   m_pConnectionOptions.keep_alive = true;
   return m_pConnectionOptions;
}


int MrcpRedisDbSubscriber::pushModuleMsg(IURModuleMsg * pdbConnectorMsg)
{
	XGLOG_INFO("MrcpRedisDbSubscriber::inside pushModuleMsg");
	if (!pdbConnectorMsg) {
		XGLOG_ERROR("pushModuleMsg::Message not received from controller.!");
		return IURDefines::UR_RETURN_ERROR;
	}
	else if(IURDefines::UR_MODULE_MRCP_REDIS_DB_SUBSCRIBE == pdbConnectorMsg->getDstModuleId()) {
		XGLOG_DEBUG("MrcpRedisDbSubscriber::Message received from controller to ivrservice");
		REDISDBSUBSERVICE->putq(dynamic_cast<ACE_Message_Block*>(pdbConnectorMsg));
	}
	return IURDefines::UR_RETURN_SUCCESS;
}

void MrcpRedisDbSubscriber::redisCcaasResponse()
{
  while(m_run)
  {
	redisReply* reply;
	if(redisGetReply(m_ccaasSubContext,(void **)&reply) == REDIS_OK)
	{
	  handle_channel_msg(reply);
	}
  }
}

int MrcpRedisDbSubscriber::svc(void)
{
	ACE_Message_Block *pAmb = 0;

	while(m_run)
	{
		redisReply* reply;
		if(redisGetReply(m_subContext,(void **)&reply) == REDIS_OK)
		{
			handle_channel_msg(reply);
		}
#if 0
		ACE_Time_Value tv ((time(NULL) + 20));
		int result = getq(pAmb, &tv);
		if ((result == -1) && (errno == EWOULDBLOCK))
		{
			continue;
		}
		if ((result != -1))
		{
			XGLOG_INFO("MrcpRedisDbSubscriber::process handle_msg success !");
			handle_msg(pAmb);
		}
		else
		{
			XGLOG_ERROR("MrcpRedisDbSubscriber::svc Error in Message Queue");
			m_run = false;
		}
		// delete the message received
		if(pAmb)
		{
			delete pAmb;
			pAmb = NULL;
		}
#endif
	}

	XGLOG_INFO("MrcpRedisDbSubscriber::svc() stopped");
	return 0;
}

bool MrcpRedisDbSubscriber::handle_msg(redisReply* reply)
{
   UrMrcpDbServiceMsg *dbService = NULL;
   dbService = decode_transcript_request_msg(reply->element[2]->str);
   if(dbService)
   {
	 dbService->set_message_type(EN_MESSAGE_TYPE::EN_MESSAGE_TYPE_VMS_STT_REQ);
	 dbService->setSrcModuleId(IURDefines::UR_MODULE_MRCP_REDIS_DB_SUBSCRIBE);
	 dbService->setDstModuleId(IURDefines::UR_MODULE_SERVICE_BRIDGE);
	 UR_MRCP_MODULE_BRIDGE::instance()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(dbService));
   }
   return true;
}

bool MrcpRedisDbSubscriber::handle_redis_vms_response(std::string msg)
{
  if(msg.empty())
  {
	XGLOG_ERROR("MrcpRedisDbSubscriber::handle_redis_vms_response msg is empty!");
	return false;
  }
  XGLOG_INFO("MrcpRedisDbSubscriber::handle_redis_vms_response msg(%s)",msg.c_str());
  UrMrcpDbServiceMsg *dbService = new UrMrcpDbServiceMsg;
  dbService = decode_transcript_response_msg(msg.c_str());
  if(dbService)
  {
	 dbService->set_display_text(msg.c_str());
	 dbService->set_message_type(EN_MESSAGE_TYPE::EN_MESSAGE_TYPE_VMS_STT_RESP);
	 dbService->setSrcModuleId(IURDefines::UR_MODULE_MRCP_REDIS_DB_SUBSCRIBE);
	 dbService->setDstModuleId(IURDefines::UR_MODULE_SERVICE_BRIDGE);
	 UR_MRCP_MODULE_BRIDGE::instance()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(dbService));
  }
  return true;
}

bool MrcpRedisDbSubscriber::handle_redis_vms_request(std::string msg)
{
  if(msg.empty())
  {
	XGLOG_ERROR("MrcpRedisDbSubscriber::handle_redis_vms_request msg is empty!");
	return false;
  }
  XGLOG_INFO("MrcpRedisDbSubscriber::handle_redis_vms_request msg(%s)",msg.c_str());
   UrMrcpDbServiceMsg *dbService = NULL;
   dbService = decode_transcript_request_msg(msg.c_str());
   if(dbService)
   {
	 dbService->set_message_type(EN_MESSAGE_TYPE::EN_MESSAGE_TYPE_VMS_STT_REQ);
	 dbService->setSrcModuleId(IURDefines::UR_MODULE_MRCP_REDIS_DB_SUBSCRIBE);
	 dbService->setDstModuleId(IURDefines::UR_MODULE_SERVICE_BRIDGE);
	 UR_MRCP_MODULE_BRIDGE::instance()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(dbService));
   }
   return true;
}

UrMrcpDbServiceMsg * MrcpRedisDbSubscriber::decode_transcript_response_msg(const char* json_msg)
{
  if(!json_msg)
  {
	XGLOG_INFO("MrcpRedisDbSubscriber::decode_transcript_response_msg recieved msg is empty!");
	return NULL;
  }
  XGLOG_INFO("MrcpRedisDbSubscriber::decode_transcript_response_msg msg(%s)",json_msg);
  UrMrcpDbServiceMsg *dbService = new UrMrcpDbServiceMsg;
  XGLOG_INFO( "parsing json data: %s", json_msg);
  std::cout<<"parsing json data: "<<json_msg<<endl;

  Document doc;
  ParseResult ok = doc.Parse(json_msg);

  if(!ok) {
	XGLOG_ERROR( "decode_transcript_response_msg parseJsonMsg failed. Encountered with json parse error !");
	return nullptr;
  } else if(!doc.IsObject()) {
	XGLOG_ERROR( "decode_transcript_response_msg parseJsonMsg failed. Not able create documnet object !");
	return nullptr;
  }

  if(doc.HasMember("session_id"))
  {
	if(doc["session_id"].GetString())
	{
	  dbService->set_session_uid(doc["session_id"].GetString());
	}
	else
	{
	  XGLOG_ERROR("decode_msg failed. 'session_id' detail is empty !");
	  return NULL;
	}
  }
  else
  {
	XGLOG_ERROR("decode_msg failed. 'session_id' detail is missing in json message !");
	return NULL;
  }

  if(doc.HasMember("vmsTranscript"))
  {
	std::string vmsText = doc["vmsTranscript"].GetString();
	if(vmsText.empty())
	{
	  XGLOG_ERROR("decode_msg failed. 'vmsTranscript' detail is empty !");
	  return NULL;
	}
  }
  else
  {
	XGLOG_ERROR("decode_msg failed. 'vmsTranscript' detail is missing in json message !");
	return NULL;
  }
  return dbService;
}

UrMrcpDbServiceMsg * MrcpRedisDbSubscriber::decode_transcript_request_msg(const char* json_msg)
{
  if(!json_msg)
	return NULL;
  XGLOG_INFO("MrcpRedisDbSubscriber::decode_transcript_request_msg msg(%s)",json_msg);
  UrMrcpDbServiceMsg *dbService = new UrMrcpDbServiceMsg;
  XGLOG_INFO( "parsing json data: %s", json_msg);
  std::cout<<"parsing json data: "<<json_msg<<endl;

  Document doc;
  ParseResult ok = doc.Parse(json_msg);

  if(!ok) {
	XGLOG_ERROR( "decode_transcript_request_msg parseJsonMsg failed. Encountered with json parse error !");
	return nullptr;
  } else if(!doc.IsObject()) {
	XGLOG_ERROR( "decode_transcript_request_msg parseJsonMsg failed. Not able create documnet object !");
	return nullptr;
  }
  ccaasVmsInfo *ccaasInfo = new ccaasVmsInfo;

  if(doc.HasMember("session_id"))
  {
	if(doc["session_id"].GetString())
	{
	  ccaasInfo->set_session_id(doc["session_id"].GetString());
	}
  }
  else
  {
	XGLOG_ERROR("decode_msg failed. 'session_id' detail is missing in json message !");
	return NULL;
  }
  if(doc.HasMember("stt_engine"))
  {
	ccaasInfo->set_asr_type(doc["stt_engine"].GetInt());
  }
  else
  {
	XGLOG_ERROR("decode_msg failed. 'stt_engine' detail is missing in json message !");
	return NULL;
  }

  if(doc.HasMember("file_name"))
  {
	if(doc["file_name"].GetString())
	{
	  ccaasInfo->set_file_name(doc["file_name"].GetString());
	}
  }
  else
  {
	XGLOG_ERROR("decode_msg failed. 'file_name' detail is missing in json message !");
	return NULL;
  }

  UrMrcpDbServiceMsg* pMsg = new UrMrcpDbServiceMsg;
  pMsg->set_ccaas_vms_info(ccaasInfo);
  return pMsg;
}

bool MrcpRedisDbSubscriber::handle_channel_msg(redisReply* reply)
{
	  time_t current_time;
	current_time = time(NULL);
	std::cout <<"!!! got response ------- "<< ctime(&current_time)<<std::endl;
	if(reply && reply->elements == 3)
	{
		UrMrcpDbServiceMsg *dbService = NULL;
		dbService = decode_msg(reply->element[2]->str);
		if(dbService)
		{
			URMRCPModuleBridge *URModule = UR_MRCP_MODULE_BRIDGE::instance();
			std::map<std::string,UrMrcpSessionDetails *>::iterator it = URModule->m_sessionDetailMap.find(dbService->m_uid);
			if(it!=URModule->m_sessionDetailMap.end())
			{
				dbService->setSrcModuleId(IURDefines::UR_MODULE_MRCP_REDIS_DB_SUBSCRIBE);
				dbService->setDstModuleId(IURDefines::UR_MODULE_SERVICE_BRIDGE);
				UR_MRCP_MODULE_BRIDGE::instance()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(dbService));
			}
		}
	}
	return true;
}

bool MrcpRedisDbSubscriber::handle_redis_response(std::string msg)
{
	XGLOG_INFO("MrcpRedisDbSubscriber::handle_redis_response");
	UrMrcpDbServiceMsg *dbService = NULL;
	dbService = decode_msg(msg.c_str());
	if(dbService)
	{
		URMRCPModuleBridge *URModule = UR_MRCP_MODULE_BRIDGE::instance();
		std::map<std::string,UrMrcpSessionDetails *>::iterator it = URModule->m_sessionDetailMap.find(dbService->m_uid);
		if(it!=URModule->m_sessionDetailMap.end())
		{
			dbService->setSrcModuleId(IURDefines::UR_MODULE_MRCP_REDIS_DB_SUBSCRIBE);
			dbService->setDstModuleId(IURDefines::UR_MODULE_SERVICE_BRIDGE);
			UR_MRCP_MODULE_BRIDGE::instance()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(dbService));
		}
		else
		{
			XGLOG_ERROR("MrcpRedisDbSubscriber::handle_redis_response SESSION not found for uID[%s]",dbService->m_uid.c_str());
			return false;
		}
	}
	else
	{
		XGLOG_ERROR("MrcpRedisDbSubscriber::handle_redis_response dbService not found");
	}
  return true;
}

UrMrcpDbServiceMsg * MrcpRedisDbSubscriber::decode_msg(const char* json_msg)
{
	if(!json_msg)
	{
		XGLOG_ERROR("MrcpRedisDbSubscriber::decode_msg Got empyt JsonMsg");
		return NULL;
	}
	UrMrcpDbServiceMsg *dbService = new UrMrcpDbServiceMsg;
	std::cout<<"parsing json data: "<<json_msg<<endl;
	XGLOG_INFO("MrcpRedisDbSubscriber::decode_msg msg[%s]",json_msg);

	Document doc;
	ParseResult ok = doc.Parse(json_msg);

	if(!ok) 
	{
		XGLOG_ERROR( "parseJsonMsg failed. Encountered with json parse error !");
		return nullptr;
	} else if(!doc.IsObject()) 
	{
		XGLOG_ERROR( "parseJsonMsg failed. Not able create documnet object !");
		return nullptr;
	}

	if(doc.HasMember("text"))
	{
		if(doc["text"].GetString())
		{
			dbService->set_display_text(doc["text"].GetString());
			std::cout<<"received text to play: "<<dbService->get_display_text()<<std::endl;
		}
	}
	else
	{
		XGLOG_ERROR("decode_msg failed. 'text' detail is missing in json message !");
		return NULL;
	}
	if(doc.HasMember("session_id"))
	{
		if(doc["session_id"].GetString())
		{
			dbService->set_session_uid(doc["session_id"].GetString());
		}
	}
	else
	{
		XGLOG_ERROR("decode_msg failed. 'session_id' detail is missing in json message !");
		return NULL;
	}
	/* if(doc.HasMember("isOriginal"))
	{
		dbService->set_orignal(doc["isOriginal"].GetBool());

	}
	else
	{
		XGLOG_ERROR("decode_msg failed. 'isOriginal' detail is missing in json message !");
		return NULL;
	} */
	if(doc.HasMember("indent"))
	{
		if(doc["indent"].GetString())
		{
			dbService->m_strBotIndent = doc["indent"].GetString();
		}
	}
	else
	{
		XGLOG_ERROR("decode_msg failed. 'indent' detail is missing in json message !");
		/* return NULL; */
	}

	if(doc.HasMember("event"))
    {
        if(doc["event"].GetInt())
        {
            dbService->setBotEvent((EN_BOT_TRANSFER_EVENT)doc["event"].GetInt());
        }
    }
    else
    {
        XGLOG_ERROR("decode_msg failed. 'event' detail is missing in json message !");
        return NULL;
    }

	if(doc.HasMember("qid"))
    {
        if(doc["qid"].GetString())
        {
            dbService->setQid(doc["qid"].GetString());
        }
    }
    else
    {
        XGLOG_ERROR("decode_msg failed. 'qid' detail is missing in json message !");
        // return NULL;
    }

	if(doc.HasMember("ext"))
    {
        if(doc["ext"].GetString())
        {
            dbService->setUserExt(doc["ext"].GetString());
        }
    }
    else
    {
        XGLOG_ERROR("decode_msg failed. 'ext' detail is missing in json message !");
        // return NULL;
    }

#if 0
	if(doc.HasMember("data"))
	{
		const Value &data = doc["data"].GetObject();
		if(data.HasMember("user"))
		{
			const Value &user = data["user"];
			if(user.HasMember("call_session_id"))
			{
				if(user["call_session_id"].GetString())
				{
					dbService->set_session_uid(user["call_session_id"].GetString());
				}
			}
			else
			{
				XGLOG_ERROR("decode_msg failed. 'call_session_id' detail is missing in json message !");
				return NULL;
			}
		}
		if(doc.HasMember("type"))
		{
			dbService->set_avaamo_response_type(doc["type"].GetInt());
		}
		else
		{
			XGLOG_ERROR("decode_msg failed. 'type' detail is missing in json message !");
			return NULL;
		}
		if(dbService->get_avaamo_response_type() == AVAAMO_RESPONSE_TYPE_MSG)
		{
			if(data.HasMember("message")) {
				const Value &reply = data["message"];
				if(reply.HasMember("text"))
				{
					if(reply["text"].GetString())
					{
						dbService->set_display_text(reply["text"].GetString());
						std::cout<<"received text to play: "<<dbService->get_display_text()<<std::endl;
					}
				}
				else
				{
					XGLOG_ERROR("decode_msg failed. 'text' detail is missing in json message !");
					return NULL;
				}
				if(reply.HasMember("sequence"))
				{
					dbService->set_response_msg_seqence(reply["sequence"].GetInt());
				}
				else
				{
					XGLOG_ERROR("decode_msg failed. 'sequence' detail is missing in json message !");
					return NULL;
				}
				if(reply.HasMember("total_messages"))
				{
					dbService->set_response_total_msg(reply["total_messages"].GetInt());
				}
				else
				{
					XGLOG_ERROR("decode_msg failed. 'total_messages' detail is missing in json message !");
					return NULL;
				}
			} else {
				XGLOG_ERROR("decode_msg failed. 'message' detail is missing in json message !");
				return NULL;
			}
		}
		else if(dbService->get_avaamo_response_type() == AVAAMO_RESPONSE_TYPE_TRANS)
		{
			if(doc.HasMember("routingto"))
			{
				dbService->set_avaamo_transfer_no(doc["routingto"].GetString());
			}
		}
		else 
		{
			XGLOG_ERROR("decode_msg failed. unsupported message type");
			return NULL;
		}
	}
#endif

	return dbService;
}

bool MrcpRedisDbSubscriber::connectToRedisDb(std::string host, int port, std::string pwd)
{
		this->m_subContext = redisConnect(host.c_str(),port);

		if(this->m_subContext != NULL && this->m_subContext->err)
		{
				printf("connect error: %s\n", this->m_subContext->errstr);
				XGLOG_INFO("connect error: %s\n", this->m_subContext->errstr);
				return false;
	}
	if(!pwd.empty())
	{
		redisDbDatabaseAuthorize(pwd.c_str());
	}
	return true;
}

bool MrcpRedisDbSubscriber::connectToCcaasRedisDb(std::string host, int port)
{
		this->m_ccaasSubContext = redisConnect(host.c_str(),port);

		if(this->m_ccaasSubContext != NULL && this->m_ccaasSubContext->err)
		{
				printf("connect error: %s\n", this->m_ccaasSubContext->errstr);
				XGLOG_INFO("connect error: %s\n", this->m_ccaasSubContext->errstr);
				return false;
		}
		return true;
}

bool MrcpRedisDbSubscriber::ccaasRedisDbDatabaseAuthorize(std::string pwd)
{
		if(pwd.empty())
		{
				XGLOG_INFO("MrcpRedisDbSubscriber::redisDbDatabaseAuthorize password is empty");
				return false;
		}
		redisReply* reply = (redisReply*) redisCommand(this->m_ccaasSubContext, "AUTH %s",pwd.c_str());

		if(reply)
		{
				if(reply->type != REDIS_REPLY_ERROR)
				{
						std::cout << "Authorization Successfully " << std::endl;
						XGLOG_INFO("MrcpRedisDbSubscriber::redisDbDatabaseAuthorize Authosization Successfull");
				}
				else
				{
						XGLOG_ERROR("MrcpRedisDbSubscriber::redisDbDatabaseAuthorize Authosization Failed");
				}
				freeReplyObject(reply);
		}
		else
		{
				std::cout << "Authorization Failed" << std::endl;
				XGLOG_INFO("Authosization Failed");
				return false;
		}
		return true;

}

bool MrcpRedisDbSubscriber::redisDbDatabaseAuthorize(std::string pwd)
{
		redisReply* reply = (redisReply*) redisCommand(this->m_subContext, "AUTH %s",pwd.c_str());

		if(reply)
		{
				if(reply->type != REDIS_REPLY_ERROR)
				{
						std::cout << "Authorization Successfully " << std::endl;
						XGLOG_INFO("MrcpRedisDbSubscriber::redisDbDatabaseAuthorize Authosization Successfull");
				}
				else
				{
						XGLOG_ERROR("MrcpRedisDbSubscriber::redisDbDatabaseAuthorize Authosization Failed");
				}
				freeReplyObject(reply);
		}
	else
	{
		std::cout << "Authorization Failed" << std::endl;
		XGLOG_INFO("Authosization Failed");
		return false;
	}
	return true;
}

bool MrcpRedisDbSubscriber::subcribeAvaamoResponseChannel(std::string channelName)
{
	redisReply* reply = (redisReply*) redisCommand(m_subContext,"SUBSCRIBE %s",channelName.c_str());

		if(reply)
		{
				if(reply->type != REDIS_REPLY_ERROR)
				{
						std::cout << "Subscribed Successfully " << std::endl;
						XGLOG_INFO("MrcpRedisDbSubscriber::subcribeAvaamoResponseChannel Subscribe Successfull");
				}
				else
				{
						XGLOG_ERROR("MrcpRedisDbSubscriber::subcribeAvaamoResponseChannel Subscribe Failed");
				}
				freeReplyObject(reply);
		}
		else
		{
				std::cout << "Not Subscribed" << std::endl;
				XGLOG_INFO("Subscribed Failure");
		return false;
	}
return true;
}

bool MrcpRedisDbSubscriber::subcribeCcaasResponseChannel(std::string channelName)
{
		redisReply* reply = (redisReply*) redisCommand(m_ccaasSubContext,"SUBSCRIBE %s",channelName.c_str());

		if(reply)
		{
				if(reply->type != REDIS_REPLY_ERROR)
				{
						std::cout << "Subscribed Successfully " << std::endl;
						XGLOG_INFO("MrcpRedisDbSubscriber::subcribeCcaasResponseChannel Subscribe Successfull");
				}
				else
				{
						XGLOG_ERROR("MrcpRedisDbSubscriber::subcribeCcaasResponseChannel Subscribe Failed");
				}
				freeReplyObject(reply);
		}
		else
		{
				std::cout << "Not Subscribed" << std::endl;
				XGLOG_INFO("Subscribed Failure");
				return false;
		}
		return true;
}


bool MrcpRedisDbSubscriber::free()
{
		redisFree(this->m_subContext);
		redisFree(this->m_ccaasSubContext);
}

bool MrcpRedisDbSubscriber::connectToDb(std::string host, int port, std::string pwd, std::string channel)
{
	   redisContext* subContext = redisConnect(host.c_str(),port);

		if(subContext != NULL && subContext->err)
		{
				printf("connect error: %s\n", subContext->errstr);
				XGLOG_INFO("connect error: %s\n", subContext->errstr);
				return false;
		}
		if(!pwd.empty())
		{
		  if(!authorizeConnection(subContext, pwd))
		  {
			XGLOG_ERROR("MrcpRedisDbSubscriber::connectToDb authorizeConnection failed");
			return false;
		  }
		}
		else
		{
		  XGLOG_DEBUG("MrcpRedisDbSubscriber::connectToDb password is empty");
		}

		subcribeChannel(subContext, channel);
		return true;

}

bool MrcpRedisDbSubscriber::authorizeConnection(redisContext* context, std::string pwd)
{
		redisReply* reply = (redisReply*) redisCommand(context, "AUTH %s",pwd.c_str());

		if(reply)
		{
				if(reply->type != REDIS_REPLY_ERROR)
				{
						std::cout << "Authorization Successfully " << std::endl;
						XGLOG_INFO("MrcpRedisDbSubscriber::redisDbDatabaseAuthorize Authosization Successfull");
				}
				else
				{
						XGLOG_ERROR("MrcpRedisDbSubscriber::redisDbDatabaseAuthorize Authosization Failed");
				}
				freeReplyObject(reply);
		}
		else
		{
				std::cout << "Authorization Failed" << std::endl;
				XGLOG_INFO("Authosization Failed");
				return false;
		}
		return true;
}

bool MrcpRedisDbSubscriber::subcribeChannel(redisContext* context, std::string channelName)
{
		redisReply* reply = (redisReply*) redisCommand(context,"SUBSCRIBE %s",channelName.c_str());

		if(reply)
		{
				if(reply->type != REDIS_REPLY_ERROR)
				{
						std::cout << "Subscribed Successfully " << std::endl;
						XGLOG_INFO("MrcpRedisDbSubscriber::subcribeCcaasResponseChannel Subscribe Successfull");
						std::thread(&MrcpRedisDbSubscriber::onMessage, this, context).detach();
				}
				else
				{
						XGLOG_ERROR("MrcpRedisDbSubscriber::subcribeCcaasResponseChannel Subscribe Failed");
				}
				freeReplyObject(reply);
		}
		else
		{
				std::cout << "Not Subscribed" << std::endl;
				XGLOG_INFO("Subscribed Failure");
				return false;
		}
		return true;
}

void MrcpRedisDbSubscriber::onMessage(redisContext* context)
{
  while(m_run)
  {
	redisReply* reply;
	if(redisGetReply(context,(void **)&reply) == REDIS_OK)
	{
	  handle_msg(reply);
	}
  }
}


