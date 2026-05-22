#include "xglog.h"
#include "xGateSipSignalProcessor.h"
#include "URModuleBridge.h"
#include "xGateHttpHandler.h"
#include "xgHttpConfig.h"

#define THISMODULE "URMODULEBRIDGE"

using namespace std;

URHTTPConfig::URHTTPConfig()
{
  m_nHTTPThreadPool = 1;
}
URHTTPConfig::~URHTTPConfig() {}
void URHTTPConfig::setCallBack(IURModuleCallBack *ptCallback)
{
  moduleCallBack = ptCallback;
}
IURModuleCallBack *URHTTPConfig::getCallBack()
{
  return moduleCallBack;
}

URModuleBridge::URModuleBridge()
{
  m_ptModule = NULL, m_strNameServerDetails = "";
  arrayModules.resize(IURDefines::UR_MAX_LEN);
}
URModuleBridge::~URModuleBridge()
{
}
bool URModuleBridge::init()
{

  if (activate(THR_NEW_LWP, 1) == -1)
  {
    XGLOG_ERROR("URModuleBridge::init() failed to start  URModuleBridge thread !");
    return false;
  }

  initHttpService();
  initDBService();
  initRedisService();
  return true;
}
bool URModuleBridge::handleModuleCallbackMsg(IURModuleMsg *pctrlMsg)
{
  if (pctrlMsg)
  {
    XGLOG_INFO("handleModuleCallbackMsg Message Received!");
    if (IURDefines::UR_MODULE_SERVICE_CONTROLLER == pctrlMsg->getDstModuleId())
    {
      URMODULE_BRIDGE::instance()->putq(dynamic_cast<ACE_Message_Block *>(pctrlMsg));
    }
    else if (arrayModules[pctrlMsg->getDstModuleId()])
    {
      arrayModules[pctrlMsg->getDstModuleId()]->pushModuleMsg(pctrlMsg);
    }
    else
    {
      XGLOG_ERROR("handleModuleCallbackMsg failed, destination Unknown!");
    }
  }

  return true;
}

int URModuleBridge::svc(void)
{
  ACE_Message_Block *pAmb = 0;

  while (true)
  {
    ACE_Time_Value tv((time(NULL) + 20));
    int result = getq(pAmb, &tv);
    if ((result == -1) && (errno == EWOULDBLOCK))
    {
      continue;
    }
    if ((result != -1))
    {
      handle_msg(pAmb);
    }
    else
    {
      XGLOG_ERROR("URModuleBridge::svc Error in Message Queue");
      break;
    }
    if (pAmb)
    {

      // delete pAmb;// message are passing in to the another thread.
      // pAmb = NULL;
    }
  }

  XGLOG_INFO("URModuleBridge::svc() stopped");
  return 0;
}

bool URModuleBridge::stop(void)
{
  msg_queue()->deactivate();
  wait();
  XGLOG_INFO("URModuleBridge::stop() stopped !");
  return true;
}
bool URModuleBridge::handle_msg(ACE_Message_Block *pAmb)
{
  if (pAmb != NULL)
  {

    xGateHttpServiceMsg *pthttpServiceMsg = dynamic_cast<xGateHttpServiceMsg *>(pAmb);

    if (pthttpServiceMsg == NULL)
    {
      XGLOG_ERROR("URModuleBridge::handle_msg recvd invalid Message class in MBC Handler");
      return false;
    }

#if 0
	if(pthttpServiceMsg->getHttpmsgType()== xGateHttpServiceMsg::HTTP_SRV_MSG_TYPE_RESPONSE){
	// json parser
	// push the PBX message in to the SIPSignale Processor.
   	  XGLOG_INFO("URModuleBridge::handle_msg Msg Received : %s", pthttpServiceMsg->get_mbc_info().msg.c_str());
	  URModuleBridgeMsg *pbridgeMsg = new URModuleBridgeMsg();
	    if( pbridgeMsg == NULL ) {
		XGLOG_ERROR( "URModuleBridge::handle_msg URModuleBridgeMsg NULL");
		return false;
	    }
	  pbridgeMsg->setMsgType(EN_XGATE_MSG_FROM_HTTP);
	  pbridgeMsg->set_topic_name(pthttpServiceMsg->get_mbc_info().topicName.c_str());
	  pbridgeMsg->set_msg(pthttpServiceMsg->get_mbc_info().msg.c_str());

	    if(postMessageToPBX(pbridgeMsg))
	      {
		XGLOG_ERROR( "URModuleBridge::handle_msg Post message to PBX SUCCESS");
	      }
	    else{
		XGLOG_ERROR( "URModuleBridge::handle_msg Post message to PBX FAILED");
	 	return false;
		}
	}
	else{
	    XGLOG_ERROR( "URModuleBridge::handle_msg  UNKNOWN MESSAGE");
	    return false;
	}
#endif
  }

  return true;
}

/*
* Initializing HTTP Service 
*/
bool URModuleBridge::initHttpService()
{
  IURModule *pHttpService = NULL;
  URModuleFactory::CreateModuleInterface(IURDefines::UR_MODULE_HTTP_SERVICE, pHttpService);
  if (!pHttpService)
  {
    XGLOG_ERROR("URModuleBridge::init() Initializing URModuleBridge failed !");
    return false;
  }

  IURModuleConfig *phttpConfig = new URHTTPConfig();

  if (!phttpConfig)
  {
    XGLOG_ERROR("URModuleBridge::init() Initializing configuration for MBC service module failed !");
    return false;
  }
  phttpConfig->setCallBack(dynamic_cast<IURModuleCallBack *>(URMODULE_BRIDGE::instance()));
  arrayModules[pHttpService->getModuleID()] = pHttpService;

  if (!pHttpService->initModule(phttpConfig) == IURDefines::UR_RETURN_ERROR) {
    XGLOG_ERROR("URModuleBridge::init() failed while initializing 'URModuleBridge' !");
    return false;
  } else {

    XGLOG_INFO("URModuleBridge::init() successfully initialized 'URModuleBridge' !");
  }
  return true;
}

/*
* Initializing Database Service 
*/
bool URModuleBridge::initDBService()
{

  IURModule *pDBService = NULL;
  URModuleFactory::CreateModuleInterface(IURDefines::UR_MODULE_DB_SERVICE, pDBService);
  if (!pDBService)
  {
    XGLOG_ERROR("URModuleBridge::init() Initializing URModuleBridge failed !");
    return false;
  }

  IURModuleConfig *pDBConfig = new URHTTPConfig();

  if (!pDBConfig)
  {
    XGLOG_ERROR("URModuleBridge::init() Initializing configuration for MBC service module failed !");
    return false;
  }
  pDBConfig->setCallBack(dynamic_cast<IURModuleCallBack *>(URMODULE_BRIDGE::instance()));
  arrayModules[pDBService->getModuleID()] = pDBService;

  if (!pDBService->initModule(pDBConfig) == IURDefines::UR_RETURN_ERROR) {
    XGLOG_ERROR("URModuleBridge::init() failed while initializing 'URModuleBridge' !");
    return false;
  } else {

    XGLOG_INFO("URModuleBridge::init() successfully initialized 'URModuleBridge' !");
  }

  return true;
}

/*
* Initializing Redis Service 
*/
bool URModuleBridge::initRedisService()
{
  IURModule *pRedisService = NULL;
  URModuleFactory::CreateModuleInterface(IURDefines::UR_MODULE_REDIS_SERVICE, pRedisService);
  if (!pRedisService)
  {
    XGLOG_ERROR("URModuleBridge::init() Initializing URModuleBridge failed !");
    return false;
  }

  IURModuleConfig *pDBConfig = new URHTTPConfig();

  if (!pDBConfig)
  {
    XGLOG_ERROR("URModuleBridge::init() Initializing configuration for Redis service module failed !");
    return false;
  }
  pDBConfig->setCallBack(dynamic_cast<IURModuleCallBack *>(URMODULE_BRIDGE::instance()));
  arrayModules[pRedisService->getModuleID()] = pRedisService;

  if (!pRedisService->initModule(pDBConfig) == IURDefines::UR_RETURN_ERROR) {
    XGLOG_ERROR("URModuleBridge::init() failed while initializing 'URModuleBridge' !");
    return false;
  } else {

    XGLOG_INFO("URModuleBridge::init() successfully initialized 'URModuleBridge' !");
  }
  return true;
}

bool URModuleBridge::postMessageToPBX(URModuleBridgeMsg *pbridgeMsg)
{
  ACE_Message_Block *pAmb = NULL;
  pAmb = static_cast<URModuleBridgeMsg *>(pbridgeMsg);
  if (!pAmb)
  {
    XGLOG_ERROR("URModuleBridge::PostMessage to PBX  Failed. static casting to ACE_Message_Block failed");
    return false;
  }
  ACE_Time_Value tval((time(NULL)));
  tval += ACE_Time_Value(0, 1);
  ACE_Task<ACE_MT_SYNCH> *taskPtr = NULL;
  taskPtr = (xGateSipSignalProcessor *)GetSipPostHandler(XGATEPOSTTYPE::EN_PROCESS);
  if (taskPtr)
  {
    if (taskPtr->putq(pAmb, &tval))
    {
      XGLOG_INFO("URModuleBridge::PostMessage to PBX Success");
    }
    else
    {
      XGLOG_ERROR("URModuleBridge::PostMessage to PBX  Failed");
      return false;
    }
  }
  else
  {
    XGLOG_ERROR("URModuleBridge::PostMessage to PBX  Failed");
    return false;
  }
  return true;
}

bool URModuleBridge::PostMessagetoHTTP(string strCallId, string strSsrvURL, string strData)
{

  xGateHttpServiceMsg *pthttpServiceMsg = getHttpRequestMsg(strCallId, strSsrvURL, EN_HTTP_METHOD_POST, strData);
  if (!pthttpServiceMsg)
  {
    XGLOG_ERROR("URModuleBridge::PublishMessagetoMSGBroker. OOM issue.");
    return false;
  }
  this->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(pthttpServiceMsg));

  return true;
}

xGateHttpServiceMsg *URModuleBridge::getHttpRequestMsg(string &strContext, string &strURL, HttpReqMethod tHttpmethod, string strData)
{
  XGLOG_INFO("Inside URModuleBridge::getHttpRequestMsg");

  xGateHttpServiceMsg *ptHttpMsg = new xGateHttpServiceMsg();
  if (!ptHttpMsg)
  {
    XGLOG_ERROR("OOM Issue. Creating HTTP request failed");
    return NULL;
  }
  ptHttpMsg->setDstModuleId(IURDefines::UR_MODULE_HTTP_SERVICE);
  ptHttpMsg->setSrcModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
  ptHttpMsg->setHttpMsgType(xGateHttpServiceMsg::HTTP_SRV_MSG_TYPE_REQUEST);
  HttpRequestInfo &httpReqInfo = ptHttpMsg->get_http_request_info();
  httpReqInfo.m_url = strURL.c_str();
  httpReqInfo.m_contextId = strContext.c_str();
  // httpReqInfo.m_requestorModule           = ptHttpMsg->getSrcModuleId();
  httpReqInfo.m_contentType = EN_CONTENT_TYPE_JSON;
  httpReqInfo.m_httpReqMethod = tHttpmethod;
  httpReqInfo.m_data = strData.c_str();

  return ptHttpMsg;
}

bool URModuleBridge::PostMessageToDB(string strCallId, string strData, xGateDBEvent dbEvent)
{
  switch (dbEvent)
  {
  case xGateDBEvent::EN_XGATE_DB_QUERY_STORE_RECORD_DATA:
  {
    std::string spName = "Worktual_insert_call_recording_details";
    DBServiceEvent event = EN_DB_EVENT_STORE_CALL_REC_DATA_QUERY;
    xGateDBServiceMsg *ptDbMsg = getDBRequestMsg(strCallId, spName, strData, event);
    if (!ptDbMsg)
    {
      XGLOG_ERROR("OOM Issue. Creating DB request info for Server info failed");
      return false;
    }
    XGLOG_INFO("URModuleBridge  sending request to DB Module");
    this->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(ptDbMsg));
    break;
  }
  case xGateDBEvent::EN_XGATE_DB_QUERY_STORE_CCAAS_RECORD_DATA:
  {
    std::string spName = XGCONFIG().m_ccaasDbProfile.m_db_ccaas_rec_info_sp;
    //std::string spName = "ccaas_user_create_voice_record_info";
    DBServiceEvent event = EN_DB_EVENT_STORE_CCAAS_CALL_REC_DATA_QUERY;
    xGateDBServiceMsg *ptDbMsg = getDBRequestMsg(strCallId, spName, strData, event);
    if (!ptDbMsg)
    {
      XGLOG_ERROR("OOM Issue. Creating DB request info for Server info failed");
      return false;
    }
    XGLOG_INFO("URModuleBridge  sending request to DB Module");
    this->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(ptDbMsg));
    break;
  }
  case xGateDBEvent::EN_XGATE_DB_QUERY_STORE_CRM_RECORD_DATA:
  {
    std::string spName = XGCONFIG().m_crmDbProfile.m_db_crm_rec_info_sp;
    DBServiceEvent event = EN_DB_EVENT_STORE_CRM_CALL_REC_DATA_QUERY;
    xGateDBServiceMsg *ptDbMsg = getDBRequestMsg(strCallId, spName, strData, event);
    if (!ptDbMsg)
    {
      XGLOG_ERROR("OOM Issue. Creating DB request info for Server info failed");
      return false;
    }
    XGLOG_INFO("URModuleBridge  sending request to DB Module");
    this->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(ptDbMsg));
    break;
  }
  default:
    XGLOG_ERROR("URModuleBridge::PostMessageToDB unknown DB event callid(%s)", strCallId.c_str());
  }

  return true;
}
//Currently we are using this function only for rpush 
bool URModuleBridge::PostMessageToRedis(std::string strCallId, std::string strData)
{
  RedisServiceMsg *ptRedisMsg = getRedisPushMsg(strCallId, strData);
  if (!ptRedisMsg)
  {
    XGLOG_ERROR("OOM Issue. Creating DB request info for Server info failed");
    return false;
  }
  XGLOG_INFO("URModuleBridge  sending request to Redis Module");
  this->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(ptRedisMsg));

  return true;
}

xGateDBServiceMsg *URModuleBridge::getDBRequestMsg(std::string &strContext, std::string &spName, std::string &strData, DBServiceEvent event)
{
  XGLOG_DEBUG("Inside URModuleBridge::getDBRequestMsg");
  xGateDBServiceMsg *ptdbMsg = new xGateDBServiceMsg();
  if (!ptdbMsg)
  {
    XGLOG_ERROR("OOM Issue. Creating DB request failed");
    return NULL;
  }
  ptdbMsg->setDstModuleId(IURDefines::UR_MODULE_DB_SERVICE);
  ptdbMsg->setSrcModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
  ptdbMsg->setDbMsgType(xGateDBServiceMsg::DB_SRV_MSG_REQ);
  DBRequestInfo &dbReqInfo = ptdbMsg->get_db_request_info();
  dbReqInfo.m_contextId = strContext.c_str();
  dbReqInfo.m_dbEvent = event;
  dbReqInfo.m_requestorModule = IURDefines::UR_MODULE_SERVICE_CONTROLLER;
  dbReqInfo.m_spName = spName.c_str();
  dbReqInfo.m_data = strData.c_str();

  return ptdbMsg;
}

RedisServiceMsg *URModuleBridge::getRedisPushMsg(std::string &strContext,std::string &strData)
{
   RedisInfo redisInfo;
   redisInfo.m_strContextId = strContext.c_str();
   redisInfo.m_enReqModule = IURDefines::MODULE_ID::UR_MODULE_SERVICE_CONTROLLER;
   redisInfo.m_strMsg = strData.c_str();
   RedisServiceMsg *redisMsg = new RedisServiceMsg();
   if (!redisMsg)
   {
      XGLOG_ERROR("post_msg_to_redis_mod message failed - %s", strContext.c_str());
      return NULL;
   }
   redisMsg->set_redis_info(redisInfo);
   redisMsg->set_redis_event(EN_REDIS_EVENT_RPUSH);
   redisMsg->set_redis_msg_type(EN_REDIS_MSG_TYPE_RPUSH_REC_DATA);
   redisMsg->setDstModuleId(IURDefines::UR_MODULE_REDIS_SERVICE);
   redisMsg->setSrcModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);

   return redisMsg;
}
