#include "xglog.h"
#include "MrcpRedisDbConnector.h"
#include "URMRCPModuleBridge.h"
#include <list>
#include <iterator>
#include "sw/redis++/redis++.h"
#include "../rapidjson/document.h"
#include "../rapidjson/error/en.h"
#include "../rapidjson/writer.h"
#include "../rapidjson/reader.h"
#include "../rapidjson/stringbuffer.h"
#include "../rapidjson/prettywriter.h"
#include "../rapidjson/ostreamwrapper.h"
#include <ctime>
#include "document.h"
#include "error/en.h"
#include "writer.h"
#include "reader.h"
#include "stringbuffer.h"
#include "prettywriter.h"
#include "ostreamwrapper.h"

using namespace std;
using namespace rapidjson;


MrcpRedisDbConnector::MrcpRedisDbConnector() :
	ACE_Task<ACE_MT_SYNCH>(),
	m_run(false), m_redisDbConfig(NULL)
{
	m_strModuleName = "MrcpDbConnector";
}

MrcpRedisDbConnector::~MrcpRedisDbConnector(void)
{
}

int MrcpRedisDbConnector::initModule(IURModuleConfig * pdbConfig)
{
	XGLOG_INFO("MrcpRedisDbConnector initModule initialized !");  
	m_redisDbConfig = pdbConfig;
	if(!init()) {
		return IURDefines::UR_RETURN_ERROR;
	}
	return IURDefines::UR_RETURN_SUCCESS;
}
int MrcpRedisDbConnector::unitModule()
{
	stop();
	return IURDefines::UR_RETURN_SUCCESS;
}
void MrcpRedisDbConnector::getModuleName(std::string& moduleName){
	moduleName=m_strModuleName;
}

IURDefines::MODULE_ID MrcpRedisDbConnector::getModuleID(){
	return IURDefines::UR_MODULE_MRCP_REDIS_DB_SERVICE;
}

IURModuleConfig * MrcpRedisDbConnector::getModuleConfig(){
	return m_redisDbConfig;
}

bool MrcpRedisDbConnector::stop(void)
{
	m_run = false;
	msg_queue()->deactivate();
	wait();
	XGLOG_INFO("MrcpRedisDbConnector stopped !");
	return true;
}

bool MrcpRedisDbConnector::init()
{
	XGLOG_INFO("MrcpRedisDbConnector::init() called");
	m_run = true;

  m_tbotPubRedisInfo = MRCPCONFIGSERVICE->m_redisProfile.m_mapPubRedisConnInfo[EN_REDIS_TYPE::EN_REDIS_TYPE_BOT];
  m_tccaasPubRedisInfo = MRCPCONFIGSERVICE->m_redisProfile.m_mapPubRedisConnInfo[EN_REDIS_TYPE::EN_REDIS_TYPE_CCAAS_ACD];
  m_tccaasBotRedisInfo = MRCPCONFIGSERVICE->m_redisProfile.m_mapPubRedisConnInfo[EN_REDIS_TYPE::EN_REDIS_TYPE_CCAAS_BOT];
  m_tCcaasVmsSttInfo = MRCPCONFIGSERVICE->m_redisProfile.m_mapPubRedisConnInfo[EN_REDIS_TYPE::EN_REDIS_TYPE_CCAAS_NODE];
  m_tCcaasConv = MRCPCONFIGSERVICE->m_redisProfile.m_mapPubRedisConnInfo[EN_REDIS_TYPE::EN_REDIS_TYPE_CCAAS_CONV];
  m_tCcaasVmsTranscriptInfo = MRCPCONFIGSERVICE->m_redisProfile.m_mapPubRedisConnInfo[EN_REDIS_TYPE::EN_REDIS_TYPE_CCAAS_VMS_TRANSCRIPT];
  m_tUcaasConv = MRCPCONFIGSERVICE->m_redisProfile.m_mapPubRedisConnInfo[EN_REDIS_TYPE::EN_REDIS_TYPE_UCAAS];
  m_tpbxPubRedisInfo = MRCPCONFIGSERVICE->m_redisProfile.m_mapPubRedisConnInfo[EN_REDIS_TYPE::EN_REDIS_TYPE_PBX];
  
	// Start the log thread.
	if(activate(THR_NEW_LWP, 1) == -1 ) {
		XGLOG_ERROR("MrcpRedisDbConnector::init() failed to start IvrService thread !");
		return false;
	} else {
		XGLOG_INFO("MrcpRedisDbConnector::init() success !");
	}

	return true;
}


int MrcpRedisDbConnector::pushModuleMsg(IURModuleMsg * pdbConnectorMsg)
{
	XGLOG_INFO("MrcpRedisDbConnector::inside pushModuleMsg");
	if (!pdbConnectorMsg) {
		XGLOG_ERROR("pushModuleMsg::Message not received from controller.!");
		return IURDefines::UR_RETURN_ERROR;
	}
	else if(IURDefines::UR_MODULE_MRCP_REDIS_DB_SERVICE == pdbConnectorMsg->getDstModuleId()) {
		XGLOG_DEBUG("MrcpRedisDbConnector::Message received from controller to ivrservice");
		REDISDBSERVICE->putq(dynamic_cast<ACE_Message_Block*>(pdbConnectorMsg));
	}
	return IURDefines::UR_RETURN_SUCCESS;
}


int MrcpRedisDbConnector::svc(void)
{
	ACE_Message_Block *pAmb = 0;

	while(m_run)
	{
		ACE_Time_Value tv ((time(NULL) + 20));
		int result = getq(pAmb, &tv);
		if ((result == -1) && (errno == EWOULDBLOCK))
		{
			continue;
		}
		if ((result != -1))
		{
			XGLOG_INFO("MrcpRedisDbConnector::process handle_msg success !");
			handle_msg(pAmb);
		}
		else
		{
			XGLOG_ERROR("MrcpRedisDbConnector::svc Error in Message Queue");
			m_run = false;
		}
		// delete the message received
		if(pAmb)
		{
			delete pAmb;
			pAmb = NULL;
		}
	}

	XGLOG_INFO("MrcpRedisDbConnector::svc() stopped");
	return 0;
}

bool MrcpRedisDbConnector::publishChannel(RedisConnInfo* redisConnInfo, ccaasVmsInfo *ccaasInfo)
{
  if(!redisConnInfo)
  {
    XGLOG_ERROR("MrcpRedisDbConnector::publishChannel received invalid redis info ");
    return false;
  }

  if(redisConnInfo->m_strChannel.empty())
  {
    XGLOG_ERROR("MrcpRedisDbConnector::publishChannel received empty redis channel");
    return false;
  }

  ConnectionOptions m_pConnectionOptions;
  m_pConnectionOptions.host = redisConnInfo->m_strServerHost.c_str();
  m_pConnectionOptions.port = (redisConnInfo->m_iServerPort > 0) ? redisConnInfo->m_iServerPort : 6379;
  if(!redisConnInfo->m_strPassword.empty())
  {
    m_pConnectionOptions.password = redisConnInfo->m_strPassword.c_str();
  }
  m_pConnectionOptions.socket_timeout = (redisConnInfo->m_iSocketTimeout > 0) ? std::chrono::milliseconds(redisConnInfo->m_iSocketTimeout) : std::chrono::milliseconds(0);
  int cluster = redisConnInfo->m_iCluster;
  if(cluster==1)
  {
    RedisCluster pubRedis(m_pConnectionOptions);
    pubRedis.publish(redisConnInfo->m_strChannel.c_str(), ccaasInfo->get_data().c_str());
    std::cout<<"msg "<<ccaasInfo->get_data()<<std::endl;
  }
  else
  {
    Redis pubRedis(m_pConnectionOptions);
    pubRedis.publish(redisConnInfo->m_strChannel.c_str(), ccaasInfo->get_data().c_str());
    std::cout<<"msg "<<ccaasInfo->get_data()<<std::endl;
  }
  XGLOG_INFO("MrcpRedisDbConnector::publishChannel Publishing to channel(%s), msg(%s)",redisConnInfo->m_strChannel.c_str(),ccaasInfo->get_data().c_str());

  return true;
}

bool MrcpRedisDbConnector::encodeVmsSttMsg(ccaasVmsInfo *ccaasInfo)
{
  Document doc;
  StringBuffer strBuf;
  Writer<StringBuffer> writer(strBuf);
  Value outputVal(kObjectType);
  Document::AllocatorType& allocator = doc.GetAllocator();
  outputVal.AddMember("session_id", setJsonParam(ccaasInfo->get_session_id().c_str(),doc),allocator);
  outputVal.AddMember("vmsTranscript", setJsonParam(ccaasInfo->get_data().c_str(),doc),allocator);

  outputVal.Accept(writer);
  ccaasInfo->set_data(strBuf.GetString());
  return true;
}

bool MrcpRedisDbConnector::handle_msg(ACE_Message_Block *pAmb)
{
	UrMrcpDbServiceMsg *dbServiceMsg = dynamic_cast<UrMrcpDbServiceMsg*>(pAmb);
	if(!dbServiceMsg) 
    {
		XGLOG_ERROR("handle_msg failed. IvrBaseMsg pointer is NULL !");
		return false;
	}
    std::string broadCastText("");

    ccaasVmsInfo* vmsInfo = dbServiceMsg->get_ccaas_vms_info();
    if(dbServiceMsg->m_pubChannelId == xGatePubChannelId::EN_CCAAS_VMS_TRANSCRIPT_REQ)
    {
        if(!vmsInfo)
        {
            XGLOG_ERROR("handle_msg failed. vmsInfo pointer is NULL !");
            return false;
        }
    }
    #if 0
    if(!vmsInfo)
    {
        encodeVmsSttMsg(dbServiceMsg->get_ccaas_vms_info());
        XGLOG_INFO("MrcpRedisDbConnector::handle_msg encodeVmsSttMsg success for ccaasId(%d) ",dbServiceMsg->get_ccaas_vms_info()->get_session_id());
        publishChannel(m_tCcaasVmsSttInfo,dbServiceMsg->get_ccaas_vms_info());
        return true;
    }
    #endif

    XGLOG_INFO("MrcpRedisDbConnector::handle_msg isCcaasAgentConnected - %d ",dbServiceMsg->m_isCcaasAgentConnected);

    switch(dbServiceMsg->m_pubChannelId)
    {
        case xGatePubChannelId::EN_CCAAS_BOT:
        {
            broadCastText = encodeJsonMessageForBotConv(dbServiceMsg);
            post_to_redis(m_tccaasBotRedisInfo,m_tccaasBotRedisInfo->m_strChannel.c_str(),broadCastText.c_str());
            break;
        }
        case xGatePubChannelId::EN_CCAAS_AGENT_CONNECTED:
        {
            broadCastText = encodeJsonMessageForCcaasConv(dbServiceMsg);
            post_to_redis(m_tCcaasConv,m_tCcaasConv->m_strChannel.c_str(),broadCastText.c_str());
            break;
        }
        case xGatePubChannelId::EN_CCAAS_ACD_MG_COMM:
        {
            broadCastText = encodeJsonMessageForBotConv(dbServiceMsg);
            post_to_redis(m_tccaasPubRedisInfo,m_tccaasPubRedisInfo->m_strChannel.c_str(),broadCastText.c_str());
            break;
        }
        case xGatePubChannelId::EN_CCAAS_VMS_TRANSCRIPT_REQ:
        {
        post_to_redis(m_tCcaasVmsTranscriptInfo,m_tCcaasVmsTranscriptInfo->m_strChannel.c_str(),vmsInfo->get_data().c_str());
        break;
        }
        case xGatePubChannelId::EN_CCAAS_VMS_TRANSCRIPT_NODE:
        {
            post_to_redis(m_tCcaasVmsSttInfo,m_tCcaasVmsSttInfo->m_strChannel.c_str(),dbServiceMsg->get_display_text().c_str());
            break;
        }
        case xGatePubChannelId::EN_PUB_ID_UCAAS:
        {
            broadCastText= encodeJsonMessageForUcaasConv(dbServiceMsg);
            post_to_redis(m_tUcaasConv,m_tUcaasConv->m_strChannel.c_str(),broadCastText.c_str());
            break;
        }
        case xGatePubChannelId::EN_PUB_ID_PBX:
        {
            broadCastText = encodeJsonMessageForPbx(dbServiceMsg);
            post_to_redis(m_tpbxPubRedisInfo,m_tpbxPubRedisInfo->m_strChannel.c_str(),broadCastText.c_str());
            break;
        }
        default:
        {
            XGLOG_ERROR("MrcpRedisDbConnector::handle_msg 'Unknown Channel Id' for uid(%s)",(char*)ur_log_string(dbServiceMsg->m_uid));
        }
    }

    if(!dbServiceMsg->m_isCcaasAgentConnected && strcmp(dbServiceMsg->m_ccaasCallStatus.c_str(),"init") && dbServiceMsg->m_pubChannelId == xGatePubChannelId::EN_CCAAS_BOT)
    {
        //Added for set a timer
        UrMrcpDbServiceMsg *dbService = new UrMrcpDbServiceMsg;
        dbService->set_session_uid(dbServiceMsg->get_session_uid());
        dbService->setSrcModuleId(IURDefines::UR_MODULE_MRCP_REDIS_DB_SERVICE);
        dbService->setDstModuleId(IURDefines::UR_MODULE_SERVICE_BRIDGE);
        UR_MRCP_MODULE_BRIDGE::instance()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(dbService));
    }
	return true;
}

std::string MrcpRedisDbConnector::encodeJsonMessageForUcaasConv(UrMrcpDbServiceMsg *dbServiceMsg)
{
  string broadcastJsonData("");
  if(!dbServiceMsg)
  {
    XGLOG_ERROR("MrcpRedisDbConnector::encodeJsonMessageForUcaasConv dbServiceMsg is null");
    return broadcastJsonData.c_str();
  }

  XGLOG_INFO("MrcpRedisDbConnector::encodeJsonMessageForUcaasConv uID[%s]",(char*)ur_log_string(dbServiceMsg->m_uid));
  Document doc;
  StringBuffer strBuf;
  Writer<StringBuffer> writer(strBuf);
  Value outputVal(kObjectType);
  Document::AllocatorType& allocator = doc.GetAllocator();

  Value userList(kArrayType);
  if(dbServiceMsg->m_ldisplayUsers.size() > 0)
  {
    std::list<Display_Info>::iterator it = dbServiceMsg->m_ldisplayUsers.begin();
    for(it;it != dbServiceMsg->m_ldisplayUsers.end();it++)
    {
      Value outputVal_(kObjectType);
      outputVal_.AddMember("user",setJsonParam((*it).ext,doc),allocator);
      outputVal_.AddMember("domainId",setJsonParam((*it).domain_id,doc),allocator);
      outputVal_.AddMember("device",setJsonParam((*it).device,doc),allocator);
      userList.PushBack(outputVal_,allocator);
    }
    outputVal.AddMember("Users",userList, allocator);
  }
  outputVal.AddMember("speaker", setJsonParam(dbServiceMsg->m_speaker, doc), allocator);
  outputVal.AddMember("uId", setJsonParam(dbServiceMsg->m_uid, doc), allocator);
  outputVal.AddMember("text", setJsonParam(dbServiceMsg->m_displayText, doc), allocator);

  outputVal.Accept(writer);
  broadcastJsonData = strBuf.GetString();
  return broadcastJsonData.c_str();
}

Value MrcpRedisDbConnector::setJsonParam(std::string strValue, Document &doc)
{
  Value retVal;
  retVal.SetString(strValue.c_str(), strValue.length(), doc.GetAllocator());
  return retVal;
}

Value MrcpRedisDbConnector::setJsonParam(unsigned int ivalue, Document &doc)
{
  Value retVal;
  retVal = ivalue;
  return retVal;
}

std::string MrcpRedisDbConnector::encodeJsonMessageForCcaasConv(UrMrcpDbServiceMsg *dbServiceMsg)
{
    Document doc;
    StringBuffer strBuf;
    Writer<StringBuffer> writer(strBuf);
    Value outputVal(kObjectType);

    if(!dbServiceMsg->m_strDocMessage.empty())
    {
        ParseResult ok = doc.Parse(dbServiceMsg->m_strDocMessage.c_str());
        Document::AllocatorType& allocator = doc.GetAllocator(); 
        if(!ok) 
        {
            XGLOG_ERROR( "parseJsonMsg encountered with json parse error !");
            return NULL;
        }

        if(doc.HasMember("speaker"))
        {
            const Value& val = doc["speaker"];
            val.SetString(dbServiceMsg->display.ext.c_str(),dbServiceMsg->display.ext.length());
        }

        doc.AddMember("text",setJsonParam(dbServiceMsg->m_displayText,doc),allocator);
    }

    doc.Accept(writer);
    string broadcastJsonData("");
    broadcastJsonData = strBuf.GetString();

    return broadcastJsonData;
}

std::string MrcpRedisDbConnector::formJsonMessageForRedisDb(UrMrcpDbServiceMsg *dbServiceMsg)
{

        Document doc;
        StringBuffer strBuf;
        Writer<StringBuffer> writer(strBuf);
        Value outputVal(kObjectType);
        Document::AllocatorType& allocator = doc.GetAllocator();

	std::string called_number = "vmtest";
	Value outp1(kObjectType);
	outp1.AddMember("text", setJsonParam(dbServiceMsg->m_displayText,doc), allocator);
	outputVal.AddMember("text", outp1, allocator);
	outputVal.AddMember("callerid", setJsonParam(dbServiceMsg->m_speaker,doc),allocator);
	//outputVal.AddMember("bot", setJsonParam(called_number,doc),allocator);
	outputVal.AddMember("bot", setJsonParam(dbServiceMsg->m_avaamoBotName,doc),allocator);
	outputVal.AddMember("type", setJsonParam(dbServiceMsg->m_avaamoRequestType,doc),allocator);
	outputVal.AddMember("call_session_id", setJsonParam(dbServiceMsg->m_uid,doc),allocator);
	outputVal.AddMember("session_id", setJsonParam(dbServiceMsg->m_uid,doc),allocator);
        outputVal.Accept(writer);
        string broadcastJsonData("");
        broadcastJsonData = strBuf.GetString();
	
  return broadcastJsonData;
}

std::string MrcpRedisDbConnector::encodeJsonMessageForBotConv(UrMrcpDbServiceMsg *dbServiceMsg)
{
  Document doc;
  StringBuffer strBuf;
  Writer<StringBuffer> writer(strBuf);
  Value outputVal(kObjectType);
  Document::AllocatorType& allocator = doc.GetAllocator();

  switch(dbServiceMsg->get_eventId())
  {
    case xGateMgAcdEvent::EN_CCAAS_ACD_MG_BOT_FAILED:
    {
      XGLOG_INFO("MrcpRedisDbConnector::encodeJsonMessageForBotConv 'EN_CCAAS_ACD_MG_BOT_FAILED' uid(%s)",(char*)ur_log_string(dbServiceMsg->m_uid));
      outputVal.AddMember("session_id", setJsonParam(dbServiceMsg->m_uid,doc),allocator);
      outputVal.AddMember("event_id", setJsonParam((unsigned int)dbServiceMsg->get_eventId(),doc),allocator);
      break;
    }
    case xGateMgAcdEvent::EN_CCAAS_ACD_MG_BASE64_FILE_NAME:
    {
      XGLOG_INFO("MrcpRedisDbConnector::encodeJsonMessageForBotConv 'EN_CCAAS_ACD_MG_BASE64_FILE_NAME' uid(%s), filename(%s)",(char*)ur_log_string(dbServiceMsg->m_uid),(char*)ur_log_string(dbServiceMsg->get_base64Path()));
      outputVal.AddMember("session_id", setJsonParam(dbServiceMsg->m_uid,doc),allocator);
      outputVal.AddMember("event_id", setJsonParam((unsigned int)dbServiceMsg->get_eventId(),doc),allocator); 
      outputVal.AddMember("file_name", setJsonParam(dbServiceMsg->m_base64Path,doc),allocator); 
      break;
    }
    case xGateMgAcdEvent::EN_CCAAS_ACD_MG_GET_VARIABLE:
    {
      XGLOG_INFO("MrcpRedisDbConnector::encodeJsonMessageForBotConv 'EN_CCAAS_ACD_MG_GET_VARIABLE' uid(%s), filename(%s)",(char*)ur_log_string(dbServiceMsg->m_uid),(char*)ur_log_string(dbServiceMsg->get_base64Path()));
      outputVal.AddMember("session_id", setJsonParam(dbServiceMsg->m_uid,doc),allocator);
      outputVal.AddMember("event_id", setJsonParam((unsigned int)dbServiceMsg->get_eventId(),doc),allocator);
      outputVal.AddMember("text", setJsonParam(dbServiceMsg->m_displayText,doc),allocator);
      break;
    }
    case xGateMgAcdEvent::EN_CCAAS_ACD_MG_BOT_INDENT:
    {
      XGLOG_INFO("MrcpRedisDbConnector::encodeJsonMessageForBotConv 'EN_CCAAS_ACD_MG_BOT_INDENT' uid(%s)",(char*)ur_log_string(dbServiceMsg->m_uid));
      outputVal.AddMember("session_id", setJsonParam(dbServiceMsg->m_uid,doc),allocator);
      outputVal.AddMember("event_id", setJsonParam((unsigned int)dbServiceMsg->get_eventId(),doc),allocator);
      /* outputVal.AddMember("bot_indent", setJsonParam(dbServiceMsg->m_strBotIndent,doc),allocator); */
      outputVal.AddMember("bot_indent", setJsonParam(dbServiceMsg->getQid(),doc),allocator);
      break;
    }
    case xGateMgAcdEvent::EN_CCAAS_ACD_MG_BOT_SHUTDOWN:
    {
      XGLOG_INFO("MrcpRedisDbConnector::encodeJsonMessageForBotConv 'EN_CCAAS_ACD_MG_BOT_SHUTDOWN   ' uid(%s)",(char*)ur_log_string(dbServiceMsg->m_uid));
      outputVal.AddMember("session_id", setJsonParam(dbServiceMsg->m_uid,doc),allocator);
      outputVal.AddMember("event_id", setJsonParam((unsigned int)dbServiceMsg->get_eventId(),doc),allocator);
      break;
    }
    default:
    {
      XGLOG_INFO("MrcpRedisDbConnector::encodeJsonMessageForBotConv uid(%s)",(char*)ur_log_string(dbServiceMsg->m_uid));
      outputVal.AddMember("session_id", setJsonParam(dbServiceMsg->m_uid,doc),allocator);
      outputVal.AddMember("text", setJsonParam(dbServiceMsg->m_displayText,doc),allocator);
      outputVal.AddMember("speaker", setJsonParam(dbServiceMsg->display.ext,doc),allocator);
      /* outputVal.AddMember("channel_id", setJsonParam(dbServiceMsg->m_ccaasChannelId,doc),allocator);
      outputVal.AddMember("agent_id", setJsonParam(dbServiceMsg->m_ccaasAgentId,doc),allocator);
      outputVal.AddMember("bot_url", setJsonParam(dbServiceMsg->m_ccaasBotUrl,doc),allocator);
      outputVal.AddMember("Type", setJsonParam("voice",doc),allocator); */
      outputVal.AddMember("status", setJsonParam(dbServiceMsg->m_ccaasCallStatus,doc),allocator);
      outputVal.AddMember("bot_id", setJsonParam(dbServiceMsg->m_iCcaasBotId,doc),allocator);
      outputVal.AddMember("domain_id", setJsonParam(dbServiceMsg->display.domain_id,doc),allocator);
      outputVal.AddMember("auto_attendant", setJsonParam(dbServiceMsg->m_iIsCcaasAutoAttendant,doc),allocator);
      outputVal.AddMember("virtual_agent", setJsonParam(dbServiceMsg->m_bIsCcaasVirtualAgent,doc),allocator);
      outputVal.AddMember("ccaas_number", setJsonParam(dbServiceMsg->m_strCcaasDdi,doc),allocator);
      outputVal.AddMember("type", setJsonParam((int)dbServiceMsg->getBotType(),doc),allocator);
    }
  }

  outputVal.Accept(writer);
  string broadcastJsonData("");
  broadcastJsonData = strBuf.GetString();
	
  return broadcastJsonData; 
}

bool MrcpRedisDbConnector::mrcpConnectToRedisDb(std::string host, int port)
{
        this->m_dbContext = redisConnect(host.c_str(),port);
        std::cout<<"!!! dp ip - "<<host.c_str()<<std::endl;
        //this->m_dbContext = redisConnect("10.22.2.12",6379);

        if(this->m_dbContext != NULL && this->m_dbContext->err)
        {
                printf("connect error: %s\n", this->m_dbContext->errstr);
                XGLOG_ERROR("connect error: %s\n", this->m_dbContext->errstr);
                return 0;
        }

        return true;
}
bool MrcpRedisDbConnector::mrcpConnectToCcaasRedisDb(std::string host, int port)
{
        this->m_ccaasDbContext = redisConnect(host.c_str(),port);
        //this->m_ccaasDbContext = redisConnect("10.30.3.28",6379);
        if(this->m_ccaasDbContext != NULL && this->m_ccaasDbContext->err)
        {
                printf("ccaas connect error: %s\n", this->m_ccaasDbContext->errstr);
                XGLOG_ERROR("ccaas db connection error: %s\n", this->m_ccaasDbContext->errstr);
                return 0;
        }

        return true;
}

bool MrcpRedisDbConnector::ccaasBotRedisDbConnect(std::string host, int port)
{
  this->m_ccaasBotContext = redisConnect(host.c_str(),port);
  std::cout<<"!!! Connected with redis db ip - "<<host.c_str()<<std::endl;
  if(this->m_ccaasBotContext != NULL && this->m_ccaasBotContext->err)
  {
    printf("ccaas bot connect error: %s\n", this->m_ccaasBotContext->errstr);
    XGLOG_ERROR("ccaas db connection error: %s\n", this->m_ccaasBotContext->errstr);
    return 0;   
  }
  return true;
}

bool MrcpRedisDbConnector::mrcpDatabaseAuthorize(std::string pwd)
{
        redisReply* reply = (redisReply*) redisCommand(this->m_dbContext, "AUTH %s",pwd.c_str());

        if(reply)
        {
          if(reply->type != REDIS_REPLY_ERROR)
          {
            std::cout << "Authorization Successfully " << std::endl;
            XGLOG_INFO("xGateSttDbService::Authosization Successfull");
          }
          else
          {
            XGLOG_ERROR("Authosization Failed");
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


bool MrcpRedisDbConnector::mrcpCcaasRedisDbAuthorize(std::string pwd)
{
  if(pwd.empty())
  {
    XGLOG_INFO("xGateSttDbService::mrcpCcaasRedisDbAuthorize pwd is empty");
    return false;
  }

  redisReply* reply = (redisReply*) redisCommand(this->m_ccaasDbContext, "AUTH %s",pwd.c_str());

  if(reply)
  {
    if(reply->type != REDIS_REPLY_ERROR)
    {
            std::cout << "Authorization Successfully " << std::endl;
            XGLOG_INFO("xGateSttDbService::mrcpCcaasRedisDbAuthorize Authosization Successfull");
    }
    else
    {
            XGLOG_ERROR("MrcpRedisDbConnector::mrcpCcaasRedisDbAuthorize Authosization Failed");
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

bool MrcpRedisDbConnector::ccaasBotRedisDbAuthorize(std::string pwd)
{
  if(pwd.empty())
  {
    XGLOG_INFO("xGateSttDbService::ccaasBotRedisDbAuthorize pwd is empty");
    return false;
  }

  redisReply* reply = (redisReply*) redisCommand(this->m_ccaasBotContext, "AUTH %s",pwd.c_str());

  if(reply)
  {
    if(reply->type != REDIS_REPLY_ERROR)
    {
      std::cout << "Authorization Successfully " << std::endl;
      XGLOG_INFO("xGateSttDbService::m_ccaasBotContext Authosization Successfull");
    }
    else
    {
      XGLOG_ERROR("MrcpRedisDbConnector::m_ccaasBotContext Authosization Failed");
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

ConnectionOptions MrcpRedisDbConnector::connectToRedis(RedisConnInfo* redisInfo)
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

bool MrcpRedisDbConnector::post_to_redis(RedisConnInfo* redisInfo,std::string channel,std::string msg)
{
    XGLOG_INFO("MrcpRedisDbConnector::post_to_redis");
    if(redisInfo == nullptr)
    {
        XGLOG_ERROR("! MrcpRedisDbConnector::post_to_redis redisInfo is null");
        return false;
    }

    if(msg.empty())
    {
        XGLOG_ERROR("! MrcpRedisDbConnector::post_to_redis publishing msg is empty");
        return false;
    }

    if(channel.empty())
    {
        XGLOG_ERROR("! MrcpRedisDbConnector::post_to_redis recieved channel is empty");
        return false;
    }

    ConnectionOptions connectionOptions = connectToRedis(redisInfo);
    std::string text = msg.c_str();
    findAndTrim(text);

    try
    {
        if(redisInfo->m_enMsgType == EN_REDIS_MSG_TYPE::EN_REDIS_MSG_TYPE_PUBSUB)
        {
            XGLOG_INFO("MrcpRedisDbConnector::post_to_redis [PUB] Channel(%s) msg(%s)",channel.c_str(),text.c_str());
            if(redisInfo->m_iCluster)
            {
                RedisCluster redisHandle(connectionOptions);
                redisHandle.publish(channel.c_str(),msg.c_str());
            }
            else 
            {
                Redis redisHandle(connectionOptions);
                redisHandle.publish(channel.c_str(),msg.c_str());
            }
        }
        else if(redisInfo->m_enMsgType == EN_REDIS_MSG_TYPE::EN_REDIS_MSG_TYPE_PUSHPOP)
        {
            XGLOG_INFO("MrcpRedisDbConnector::post_to_redis [PUSH] Channel(%s) msg(%s)",channel.c_str(),text.c_str());
            if(redisInfo->m_iCluster)
            {
                RedisCluster redisHandle(connectionOptions);
                redisHandle.lpush(channel.c_str(),msg.c_str());
            }
            else 
            {
                Redis redisHandle(connectionOptions);
                redisHandle.lpush(channel.c_str(),msg.c_str());
            }
        }
        else if(redisInfo->m_enMsgType == EN_REDIS_MSG_TYPE::EN_REDIS_MSG_TYPE_STREAM)
        {
            std::vector<std::pair<STR,STR>> vData;
            STR id("");
            vData.push_back(std::make_pair("message",msg.c_str()));
            if(redisInfo->m_iCluster)
            {
                RedisCluster redisHandle(connectionOptions);
                id = redisHandle.xadd(channel.c_str(),"*",vData.begin(),vData.end());
            }
            else
            {
                Redis redisHandle(connectionOptions);
                id = redisHandle.xadd(channel.c_str(),"*",vData.begin(),vData.end());
            }

            XGLOG_INFO("MrcpRedisDbConnector::post_to_redis [STREAM] Channel(%s) msg(%s) id(%s)",channel.c_str(),text.c_str(),id.c_str());
        }
    }
    catch(const Error &err)
    {
        XGLOG_ERROR("post_to_redis() posting to redis failed. Exception caught - %s",err.what());
        return false;
    }
    return true;
}

bool MrcpRedisDbConnector::findAndTrim(std::string &text)
{
  XGLOG_INFO("MrcpRedisDbConnector::findAndTrim text - %s",text.c_str());
  std::string del("%");
  size_t found = text.find(del);
  while(found != string::npos)
  {
      text.erase(text.begin() + found);
      found = text.find(del);
  }
}

bool MrcpRedisDbConnector::PostToRedis(std::string channel,const char* msg,redisContext* context,EN_REDIS_MSG_TYPE m_enMsgType)
{
  std::cout<<"! MrcpRedisDbConnector::PostToRedis --> "<<msg<<" channel -->"<<channel<<std::endl;
  redisReply* reply;
  time_t current_time;
  current_time = time(NULL);
  std::cout << std:: endl <<"!!! publishing ------- "<< ctime(&current_time)<<std::endl;

  if(m_enMsgType == EN_REDIS_MSG_TYPE::EN_REDIS_MSG_TYPE_PUBSUB)
  {
    XGLOG_INFO("MrcpRedisDbConnector::PostToRedis Publishing to channel(%s), msg(%s)",channel.c_str(),msg);
    reply = (redisReply*) redisCommand(context, "PUBLISH %s %s ",channel.c_str(),msg);
  }
  else if(m_enMsgType == EN_REDIS_MSG_TYPE::EN_REDIS_MSG_TYPE_PUSHPOP)
  {
    reply = (redisReply*) redisCommand(context, "RPUSH %s %s ",channel.c_str(),msg);
    XGLOG_INFO("MrcpRedisDbConnector::PostToRedis Pushing to channel(%s), msg(%s)",channel.c_str(),msg);
  }

  if(reply)
  {
    if(reply->type != REDIS_REPLY_ERROR)
    {
      std::cout << "Published Successfully " << std::endl;
      XGLOG_INFO("MrcpRedisDbConnector::PostToRedis msg(%s) Posted Successfully",msg);
    }
    else
    {
      XGLOG_ERROR("MrcpRedisDbConnector::PostToRedis msg(%s) failed",msg);
    }
    freeReplyObject(reply);
  }
  else
  {
    std::cout << "Not Published" << std::endl;
    XGLOG_INFO("Publishing Failure");
    return false;
  }
  return true;
}

#if 0
bool MrcpRedisDbConnector::PostToRedis(const char* channel,const char* msg, bool isCcaas, bool isCcaasAgentConnected)
{
  std::cout<<msg<<endl;
  redisReply* reply;
    time_t current_time;
    current_time = time(NULL);
    std::cout << std:: endl <<"!!! publishing ------- "<< ctime(&current_time)<<std::endl;
  if(isCcaas)
  {
    if(isCcaasAgentConnected)
    {
      XGLOG_INFO("xGateSttDbService::publishToDb Publishing to channel(%s), msg(%s)",m_vCcaasPubChannel[1].c_str(),msg);
      reply = (redisReply*) redisCommand(this->m_ccaasDbContext, "PUBLISH %s %s ",m_vCcaasPubChannel[1].c_str(),msg);
    }
    else
    {
      // XGLOG_INFO("xGateSttDbService::publishToDb Publishing to channel(%s), msg(%s)",m_vCcaasPubChannel[0].c_str(),msg);
      // reply = (redisReply*) redisCommand(this->m_ccaasDbContext, "PUBLISH %s %s ",m_vCcaasPubChannel[0].c_str(),msg);
      XGLOG_INFO("xGateSttDbService::publishToDb Publishing to channel(%s), msg(%s)",m_vCcaasBotPubChannel[0].c_str(),msg);
      reply = (redisReply*) redisCommand(this->m_ccaasBotContext, "PUBLISH %s %s ",m_vCcaasBotPubChannel[0].c_str(),msg);
    }
  }
  else
  {
    XGLOG_INFO("xGateSttDbService::publishToDb Publishing to Avaamo_Request channel");
    reply = (redisReply*) redisCommand(this->m_dbContext, "PUBLISH %s %s ",m_vBotPubChannel[0].c_str(),msg);
  }

  if(reply)
  {
    if(reply->type != REDIS_REPLY_ERROR)
    {
      std::cout << "Published Successfully " << std::endl;
      XGLOG_INFO("xGateSttDbService::publishToDb msg(%s) Published Successful",msg);
    }
    else
    {
      XGLOG_ERROR("xGateSttDbService::publishToDb msg(%s) failed",msg);
    }
    freeReplyObject(reply);
  }
  else
  {
    std::cout << "Not Published" << std::endl;
    XGLOG_INFO("Publishing Failure");
    return false;
  }
  return true;
}
#endif

bool MrcpRedisDbConnector::free()
{
    redisFree(this->m_dbContext);
    redisFree(this->m_ccaasDbContext);
}

STR MrcpRedisDbConnector::encodeJsonMessageForPbx(UrMrcpDbServiceMsg *dbServiceMsg)
{
    if(!dbServiceMsg)
    {
        XGLOG_ERROR("MrcpRedisDbConnector::encodeJsonMessageForPbx is null");
        return nullptr;
    }
    Document doc;
    StringBuffer strBuf;
    Writer<StringBuffer> writer(strBuf);

    Value outVal(kObjectType);
    Value outputVal(kObjectType);
    Document::AllocatorType& allocator = doc.GetAllocator();

    outputVal.AddMember("session_id", setJsonParam(dbServiceMsg->m_uid,doc),allocator);
    outputVal.AddMember("event_type", setJsonParam((int)dbServiceMsg->getSmartAssistEvent(),doc),allocator);
    outputVal.AddMember("ext", setJsonParam(dbServiceMsg->getUserExt(),doc),allocator);

    outVal.AddMember("voicebot_request",outputVal,allocator);

    outVal.Accept(writer);
    string broadcastJsonData("");
    broadcastJsonData = strBuf.GetString();

    return broadcastJsonData;
}

