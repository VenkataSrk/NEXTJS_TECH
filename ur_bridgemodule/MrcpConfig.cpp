//rapidjson includes
#include "filereadstream.h"
#include "reader.h"
#include "error/en.h"

//local includes
#include "xGateUtil.h"
#include "xglog.h"
#include "MrcpConfig.h"

#define MAX_CONF_BUFFER_SIZE 2048 * 4

using namespace rapidjson;

/* 
 * XGConfig class implementation - start
 */
bool MrcpConfig::init(...)
{
	if(!checkConfigPath()) {
		return false;
	}
	if(!readConfig()) {
		XGLOG_ERROR("read configuration file '%s' failed !", m_configPath.c_str());
		return false;
	}
	return true;
}

bool MrcpConfig::deInit()
{
	return true;
}

bool MrcpConfig::checkConfigPath() 
{
	char* configPath;
	configPath = getenv(MRCP_ENV_BASE);

	if(configPath) {
		XGLOG_INFO("checkConfigPath:: MRCP will read 'mrcp.conf' file from path '%s'", configPath);
		m_configPath = configPath;
		m_configPath += MRCP_CONFIG_FILE_NAME;
	} else {
		XGLOG_ERROR("checkConfigPath:: 'MRCP_BASE' environment variable is missing !!");
		return false;
	}

	return true;
}

bool MrcpConfig::readConfig(void)
{
	if(!parseJsonConfigFile()) {
		XGLOG_ERROR("readConfig failed while parsing json config file %s !", m_configPath.c_str());
		return false;
	} else if(!readCommonDetails()) {
		XGLOG_ERROR("readConfig failed while reading common details !");
		return false;
	} else if(!readRedisProfile()) {
		XGLOG_ERROR("readConfig failed while reading common details !");
		return false;
	}else if(!readSttProfile()) {
		XGLOG_ERROR("readConfig failed while reading common details !");
		return false;
	}else if(!readTtsProfile()) {
		XGLOG_ERROR("readConfig failed while reading common details !");
		return false;
	}else if(!readTimerProfile()){
    XGLOG_ERROR("readConfig failed while reading common details !");
    return false;
  }
	return true;
}

bool MrcpConfig::parseJsonConfigFile()
{
	char confBuffer[MAX_CONF_BUFFER_SIZE];
	FILE* fp = fopen(m_configPath.c_str(), "rb");
	if(fp == NULL) {
		XGLOG_ERROR("not able to open file %s", m_configPath.c_str());
		return false;
	}
	FileReadStream is(fp, confBuffer, MAX_CONF_BUFFER_SIZE);
	fclose(fp);

	//ParseResult ok = m_configDoc.Parse(confBuffer);
	ParseResult ok = m_configDoc.Parse<kParseCommentsFlag>(confBuffer);
	if(!ok) {
		XGLOG_ERROR("parseJsonConfigFile failed. due to json parse error: %s !", GetParseError_En(ok.Code()));
		return false;
	} else if(!m_configDoc.IsObject()) {
		XGLOG_ERROR("parseJsonConfigFile failed. not able to create document object !");
		return false;
	}/* else if(!m_configDoc.HasMember("log_profile")) {
		XGLOG_ERROR("parseJsonConfigFile failed. 'log_profile' details is missing in %s file !", m_configPath.c_str());
		return false;
	} else if(!m_configDoc.HasMember("ivr_controller_profile")) {
		XGLOG_ERROR("parseJsonConfigFile failed. 'ivr_controller_profile' details is missing in %s file !", m_configPath.c_str());
		return false;
	}*/

	XGLOG_INFO("Successfully parsed json data from %s file", m_configPath.c_str());
	return true; 
}

bool MrcpConfig::readCommonDetails()
{
	if(m_configDoc.HasMember("no_response_from_avamo_ivr")){
		m_no_resp_from_avamo_ivr = m_configDoc["no_response_from_avamo_ivr"].GetString();
	} else {
		XGLOG_ERROR("readCommonDetails failed. 'no_response_from_avamo_ivr' detail is missing in '%s' file !", m_configPath.c_str());
		return false;
	}
	if(m_configDoc.HasMember("no_query_from_user_ivr")){
		m_no_query_from_user_ivr = m_configDoc["no_query_from_user_ivr"].GetString();
	} else {
		XGLOG_ERROR("readCommonDetails failed. 'no_query_from_user_ivr' detail is missing in '%s' file !", m_configPath.c_str());
		return false;
	}
  if(m_configDoc.HasMember("ipv4_addr")){
		m_strIpV4Addr = m_configDoc["ipv4_addr"].GetString();
	} else {
		XGLOG_ERROR("readCommonDetails failed. 'ipv4_addr' detail is missing in '%s' file !", m_configPath.c_str());
		return false;
	}
  if(m_configDoc.HasMember("public_ip_addr")){
		m_strPublicAddr = m_configDoc["public_ip_addr"].GetString();
	} else {
		XGLOG_ERROR("readCommonDetails failed. 'public_ip_addr' detail is missing in '%s' file !", m_configPath.c_str());
		return false;
	}
	XGLOG_INFO("Successfully read common configuration details from '%s' file", m_configPath.c_str());
	return true;

}

bool MrcpConfig::readRedisProfile()
{
  if(!m_configDoc.HasMember("redis_profile"))
  {
    XGLOG_ERROR("readRedisProfile failed. 'redis_profile' details is missing in %s file !", m_configPath.c_str());
    return false;
  }
  const Value &redisProfile = m_configDoc["redis_profile"];
  if(redisProfile.HasMember("sub"))
  {
    const Value &redis = redisProfile["sub"];
    for(Value::ConstValueIterator itr = redis.Begin(); itr != redis.End(); ++itr)
    {
      RedisConnInfo *redisConnInfo = new RedisConnInfo();
      for(Value::ConstMemberIterator iter = itr->MemberBegin(); iter != itr->MemberEnd(); ++iter)
      {
        if(strncmp(iter->name.GetString(),"cluster", sizeof(iter->name.GetString()))==0)
        {
          redisConnInfo->m_iCluster = iter->value.GetInt();
        }
        else if(strncmp(iter->name.GetString(),"sub_type", sizeof(iter->name.GetString()))==0)
        {
          redisConnInfo->m_enPubType = (EN_REDIS_TYPE)iter->value.GetInt();
        }
        else if(strncmp(iter->name.GetString(),"msg_type", sizeof(iter->name.GetString()))==0)
        {
          redisConnInfo->m_enMsgType = (EN_REDIS_MSG_TYPE)iter->value.GetInt();
        }
        else if(strncmp(iter->name.GetString(),"server_host", sizeof(iter->name.GetString()))==0)
        {
          redisConnInfo->m_strServerHost = iter->value.GetString();
        }
        else if(strncmp(iter->name.GetString(),"server_port", sizeof(iter->name.GetString()))==0)
        {
          redisConnInfo->m_iServerPort = iter->value.GetInt();
        }
        else if(strncmp(iter->name.GetString(),"password", sizeof(iter->name.GetString()))==0)
        {
          redisConnInfo->m_strPassword = iter->value.GetString();
        }
        else if(strncmp(iter->name.GetString(),"socket_timeout", sizeof(iter->name.GetString()))==0)
        {
          redisConnInfo->m_iSocketTimeout = iter->value.GetInt();
        }
        else if(strncmp(iter->name.GetString(),"channel", sizeof(iter->name.GetString()))==0)
        {
          const Value &channelList = iter->value;
          for(Value::ConstValueIterator chItr = channelList.Begin(); chItr != channelList.End(); ++chItr)
          {
            redisConnInfo->m_vChannel.push_back(chItr->GetString());
          }
        }
        else
        {
          XGLOG_ERROR("readRedisProfile failed. detail is missing in '%s' file !", m_configPath.c_str());
        }
      }
       m_redisProfile.m_mapSubRedisConnInfo.insert(std::pair<EN_REDIS_TYPE, RedisConnInfo*>(redisConnInfo->m_enPubType, redisConnInfo));
    }
  }
  if(redisProfile.HasMember("pub"))
  {
    const Value &pubRedis = redisProfile["pub"];
    for(Value::ConstValueIterator itr = pubRedis.Begin(); itr != pubRedis.End(); ++itr)
    {
      RedisConnInfo *redisConnInfo = new RedisConnInfo();
      for(Value::ConstMemberIterator iter = itr->MemberBegin(); iter != itr->MemberEnd(); ++iter)
      {
        if(strncmp(iter->name.GetString(),"cluster", sizeof(iter->name.GetString()))==0)
        {
          redisConnInfo->m_iCluster = iter->value.GetInt();
        }
        else if(strncmp(iter->name.GetString(),"pub_type", sizeof(iter->name.GetString()))==0)
        {
          redisConnInfo->m_enPubType = (EN_REDIS_TYPE)iter->value.GetInt();
        }
        else if(strncmp(iter->name.GetString(),"msg_type", sizeof(iter->name.GetString()))==0)
        {
          redisConnInfo->m_enMsgType = (EN_REDIS_MSG_TYPE)iter->value.GetInt();
        }
        else if(strncmp(iter->name.GetString(),"server_host", sizeof(iter->name.GetString()))==0)
        {
          redisConnInfo->m_strServerHost = iter->value.GetString();
        }
        else if(strncmp(iter->name.GetString(),"server_port", sizeof(iter->name.GetString()))==0)
        {
          redisConnInfo->m_iServerPort = iter->value.GetInt();
        }
        else if(strncmp(iter->name.GetString(),"password", sizeof(iter->name.GetString()))==0)
        {
          redisConnInfo->m_strPassword = iter->value.GetString();
        }
        else if(strncmp(iter->name.GetString(),"socket_timeout", sizeof(iter->name.GetString()))==0)
        {
          redisConnInfo->m_iSocketTimeout = iter->value.GetInt();
        }
        else if(strncmp(iter->name.GetString(),"channel", sizeof(iter->name.GetString()))==0)
        {
          redisConnInfo->m_strChannel = iter->value.GetString();
        }
        else
        {
          XGLOG_ERROR("readRedisProfile failed. detail is missing in '%s' file !", m_configPath.c_str());
        }
      }
      m_redisProfile.m_mapPubRedisConnInfo.insert(std::pair<EN_REDIS_TYPE, RedisConnInfo*>(redisConnInfo->m_enPubType, redisConnInfo));
    }
  }
  return true;
}

bool MrcpConfig::readSttProfile()
{
  if(!m_configDoc.HasMember("stt_profile"))
  {
    XGLOG_ERROR("readSttProfile failed. 'stt_profile' details is missing in %s file !", m_configPath.c_str());
    return false;
  }
  const Value &sttProfile = m_configDoc["stt_profile"];
  if(sttProfile.HasMember("mrcp_conf_xml_path"))
  {
    m_sttProfile.m_mrcp_xml_path = sttProfile["mrcp_conf_xml_path"].GetString();
  }
  else
  {
    XGLOG_ERROR("readSttProfile failed. 'mrcp_conf_xml_path' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }
  if(sttProfile.HasMember("cc_to_worker_node"))
  {
    m_sttProfile.m_iSendCcToWorkerNode = sttProfile["cc_to_worker_node"].GetInt();
  }
  else
  {
    XGLOG_ERROR("readSttProfile failed. 'cc_to_worker_node' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }
  if(sttProfile.HasMember("speechmatics"))
  {
    const Value &smaticInfo = sttProfile["speechmatics"];
    if(smaticInfo.HasMember("host_url"))
    {
      m_sttProfile.m_smaticsConnInfo.m_strServerHostUrl = smaticInfo["host_url"].GetString();
    }
    else
    {
      XGLOG_ERROR("readSttProfile failed. speechmatics 'host' detail is missing in '%s' file !", m_configPath.c_str());
      return false;
    }
    if(smaticInfo.HasMember("port"))
    {
      m_sttProfile.m_smaticsConnInfo.m_strServerPort = smaticInfo["port"].GetString();
    }
    else
    {
      XGLOG_ERROR("readSttProfile failed. speechmatics 'port' detail is missing in '%s' file !", m_configPath.c_str());
      return false;
    }
  }
  else
  {
    XGLOG_ERROR("readSttProfile failed. 'speechmatics' detail is missing in '%s' file !", m_configPath.c_str());
  }
  if(sttProfile.HasMember("inhouse"))
  {
    const Value &InhouseInfo = sttProfile["inhouse"];
    if(InhouseInfo.HasMember("host_ip"))
    {
      m_sttProfile.m_inhouseConnInfo.m_strServerHostIp = InhouseInfo["host_ip"].GetString();
    }
    else
    {
      XGLOG_ERROR("readSttProfile failed. inhouse 'host_ip' detail is missing in '%s' file !", m_configPath.c_str());
      return false;
    }
    if(InhouseInfo.HasMember("port"))
    {
      m_sttProfile.m_inhouseConnInfo.m_strServerPort = InhouseInfo["port"].GetString();
    }
    else
    {
      XGLOG_ERROR("readSttProfile failed. inhouse 'port' detail is missing in '%s' file !", m_configPath.c_str());
      return false;
    }
    if(InhouseInfo.HasMember("host_url"))
    {
      m_sttProfile.m_inhouseConnInfo.m_strServerHostUrl = InhouseInfo["host_url"].GetString();
    }
    else
    {
      XGLOG_ERROR("readSttProfile failed. inhouse 'host_url' detail is missing in '%s' file !", m_configPath.c_str());
      return false;
    }
    if(InhouseInfo.HasMember("type"))
    {
      m_sttProfile.m_inhouseConnInfo.m_eConnectType = (EN_ENGINE_CONNECT_TYPE)InhouseInfo["type"].GetInt();
    }
    else
    {
      XGLOG_ERROR("readSttProfile failed. inhouse 'type' detail is missing in '%s' file !", m_configPath.c_str());
      return false;
    }
  }
  else
  {
    XGLOG_ERROR("readSttProfile failed. 'inhouse' detail is missing in '%s' file !", m_configPath.c_str());
  }
  return true;
}

bool MrcpConfig::readTtsProfile()
{
  if(!m_configDoc.HasMember("tts_profile"))
  {
    XGLOG_ERROR("readTtsProfile failed. 'tts_profile' details is missing in %s file !", m_configPath.c_str());
    return false;
  }
  const Value &ttsProfile = m_configDoc["tts_profile"];
  if(ttsProfile.HasMember("mrcp_conf_xml_path"))
  {
    m_ttsProfile.m_mrcp_xml_path = ttsProfile["mrcp_conf_xml_path"].GetString();
  }
  else
  {
    XGLOG_ERROR("readTtsProfile failed. 'mrcp_conf_xml_path' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }
  if(ttsProfile.HasMember("inhouse"))
  {
    const Value &InhouseInfo = ttsProfile["inhouse"];
    if(InhouseInfo.HasMember("host_ip"))
    {
      m_ttsProfile.m_inhouseConnInfo.m_strServerHostIp = InhouseInfo["host_ip"].GetString();
    }
    else
    {
      XGLOG_ERROR("readTtsProfile failed. inhouse 'host_ip' detail is missing in '%s' file !", m_configPath.c_str());
      return false;
    }
    if(InhouseInfo.HasMember("port"))
    {
      m_ttsProfile.m_inhouseConnInfo.m_strServerPort = InhouseInfo["port"].GetString();
    }
    else
    {
      XGLOG_ERROR("readTtsProfile failed. inhouse 'port' detail is missing in '%s' file !", m_configPath.c_str());
      return false;
    }
    if(InhouseInfo.HasMember("host_url"))
    {
      m_ttsProfile.m_inhouseConnInfo.m_strServerHostUrl = InhouseInfo["host_url"].GetString();
    }
    else
    {
      XGLOG_ERROR("readTtsProfile failed. inhouse 'host_url' detail is missing in '%s' file !", m_configPath.c_str());
      return false;
    }
    if(InhouseInfo.HasMember("type"))
    {
      m_ttsProfile.m_inhouseConnInfo.m_eConnectType = (EN_ENGINE_CONNECT_TYPE)InhouseInfo["type"].GetInt();
    }
    else
    {
      XGLOG_ERROR("readTtsProfile failed. inhouse 'type' detail is missing in '%s' file !", m_configPath.c_str());
      return false;
    }
  }
  else
  {
    XGLOG_ERROR("readTtsProfile failed. 'inhouse' detail is missing in '%s' file !", m_configPath.c_str());
  }
  return true;
}


/*
* Read mrcp conf timer profile read ..
*/

bool MrcpConfig::readTimerProfile()
{
    if(!m_configDoc.HasMember("timer"))
    {
        XGLOG_ERROR("readTimerProfile failed. 'timer' details is missing in %s file !", m_configPath.c_str());
        return false;
    }
    const Value &timerProfile = m_configDoc["timer"];

    if(timerProfile.HasMember("dtmf_timeout"))
    {
        m_timerProfile.m_iDtmfTimeOut = timerProfile["dtmf_timeout"].GetInt();
    }
    else
    {
        XGLOG_ERROR("readTimerProfile failed. 'dtmf_timeout' detail is missing in '%s' file !", m_configPath.c_str());
        return false;
    }

    if(timerProfile.HasMember("bot_response_timeout"))
    {
        m_timerProfile.m_iBotResponseTimeOut = timerProfile["bot_response_timeout"].GetInt();
    }
    else
    {
        XGLOG_ERROR("readTimerProfile failed. 'bot_response_timeout' detail is missing in '%s' file !", m_configPath.c_str());
        return false;
    }

    if(timerProfile.HasMember("customer_response_timeout"))
    {
        m_timerProfile.m_iCustomerResponseTimeOut = timerProfile["customer_response_timeout"].GetInt();
    }
    else
    {
        XGLOG_ERROR("readTimerProfile failed. 'customer_response_timeout' detail is missing in '%s' file !", m_configPath.c_str());
        return false;
    }
    XGLOG_INFO("Successfully read readTimerProfile details from '%s' file", m_configPath.c_str());
    return true;
}

void MrcpConfig::setCallBack(IURModuleCallBack* pCallBack)
{
	moduleCallBack = pCallBack;
}

IURModuleCallBack* MrcpConfig::getCallBack()
{
	return moduleCallBack;
}
