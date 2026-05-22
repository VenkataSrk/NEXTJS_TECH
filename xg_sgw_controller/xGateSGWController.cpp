#include "xGateSGWController.h"
#include "xGateUtil.h"
#include "URModuleFactory.h"

xGateSGWController::xGateSGWController() : ACE_Task<ACE_MT_SYNCH>()
{
	arrayModules.resize(IURDefines::UR_MAX_LEN);
}

xGateSGWController::~xGateSGWController(void)
{
}

bool xGateSGWController::init()
{
	XGLOG_FUNCTION_EN_EX("");

	if (activate(THR_NEW_LWP, 1) == -1)
	{
		XGLOG_ERROR("xGateSGWController::startThread ACE returned error while spawning");
		return false;
	}
	else
	{
		XGLOG_INFO("xGateSGWController::init Success");
	}

	m_Json = std::move(xGateJsonMgr::GenerateJsonObj());//it will give the object 
	initDBService();
	initSchedulerService();
	//initMediaService();
	initNetIOService();
	//FetchIPToWhiteList();

	return true;
}

int xGateSGWController::svc(void)
{
	XGLOG_FUNCTION_EN_EX("");
	int rc;
	rc = pthread_setname_np(ACE_Thread::self(), "SGWController");
	if (rc != 0)
		printf("Failed to set name for WSS controller thread");

	ACE_Message_Block *pAmb = 0;

	while (true)
	{
		ACE_Time_Value tv((time(nullptr) + 20));
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
			XGLOG_ERROR("xGateSGWController::svc() Error in Message Queue");
			break;
		}

		if (pAmb)
		{
			delete pAmb;
			pAmb = nullptr;
		}
	}

	return 0;
}

bool xGateSGWController::stop(void)
{
	XGLOG_FUNCTION_EN_EX("");
	msg_queue()->deactivate();
	wait();
	return true;
}

bool xGateSGWController::handleModuleCallbackMsg(IURModuleMsg *pctrlMsg)
{
	XGLOG_FUNCTION_EN_EX("");

	if (pctrlMsg)
	{
		XGLOG_INFO("handleModuleCallbackMsg Message Received!");
		if (IURDefines::UR_MODULE_SERVICE_CONTROLLER == pctrlMsg->getDstModuleId())
		{
			SGWCONTROLLER->putq(dynamic_cast<ACE_Message_Block *>(pctrlMsg));
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

bool xGateSGWController::initDBService()
{
	XGLOG_FUNCTION_EN_EX("");
	//DB Service
	IURModule *pDBService = nullptr;
	URModuleFactory::CreateModuleInterface(IURDefines::UR_MODULE_DB_SERVICE, pDBService);
	if (!pDBService)
	{
		XGLOG_ERROR("Initializing DB service module in SGW controller failed !");
		return false;
	}
	XGConfig *pDBConfig = new XGConfig();
	if (!pDBConfig)
	{
		XGLOG_ERROR("Initializing configuration for DB service module in SGW controller failed !");
		return false;
	}
	pDBConfig->setCallBack(this);
	arrayModules[pDBService->getModuleID()] = pDBService;
	if (!pDBService->initModule(pDBConfig) == IURDefines::UR_RETURN_ERROR)
	{
		XGLOG_ERROR("init() failed while initializing 'DB Service' !");
		return false;
	}

	XGLOG_INFO("init() successfully initialized 'DB Service' !");

	return true;
}

bool xGateSGWController::initSchedulerService()
{
	XGLOG_FUNCTION_EN_EX("");
	// Scheduler module
	IURModule *pSchedulerService = nullptr;
	URModuleFactory::CreateModuleInterface(IURDefines::UR_MODULE_SCHEDULER_SERVICE, pSchedulerService);
	if (!pSchedulerService)
	{
		XGLOG_ERROR("Initializing scheduler service module in SGW controller failed !");
		return false;
	}
	XGConfig *pSchedulerConfig = new XGConfig();
	if (!pSchedulerConfig)
	{
		XGLOG_ERROR("Initializing configuration for scheduler service module in SGW controller failed !");
		return false;
	}
	pSchedulerConfig->setCallBack(this);
	arrayModules[pSchedulerService->getModuleID()] = pSchedulerService;
	if (!pSchedulerService->initModule(pSchedulerConfig) == IURDefines::UR_RETURN_ERROR)
	{
		XGLOG_ERROR("init() failed while initializing 'SchedulerService' !");
		return false;
	}
	XGLOG_INFO("init() successfully initialized 'SchedulerService' !");

	return true;
}
/*********************************************************
*ToDo::Harshith commented only required for edge-nexus
**********************************************************
bool xGateSGWController::initMediaService()
{
	// Siva ::Media service and currently using for port monitoring purpose
	IURModule *pMediaService = NULL;
	URModuleFactory::CreateModuleInterface(IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE, pMediaService);
	if (!pMediaService)
	{
		XGLOG_ERROR("Initializing Media Service module in URWss controller failed !");
		return false;
	}

	XGConfig *pMediaConfig = new XGConfig();
	if (!pMediaConfig)
	{
		XGLOG_ERROR("Initializing configuration for Media config  in URWss controller failed !");
		return false;
	}
	pMediaConfig->setCallBack(this);
	arrayModules[pMediaService->getModuleID()] = pMediaService;

	if (!pMediaService->initModule(pMediaConfig) == IURDefines::UR_RETURN_ERROR)
	{
		XGLOG_ERROR("init() failed while initializing 'Media Config ' !");
		return false;
	}
	XGLOG_INFO("init() successfully initialized 'Media/Monitor Service' !");
	return true;
}
***********************************************/
bool xGateSGWController::initNetIOService()
{
	XGLOG_FUNCTION_EN_EX("");
	IURModule *pNetIoService = nullptr;
	URModuleFactory::CreateModuleInterface(IURDefines::UR_MODULE_NETIO_SERVICE, pNetIoService);
	if (!pNetIoService)
	{
		XGLOG_ERROR("Initializing netio module failed !");
		return false;
	}

	CONFIGSERVICE->setCallBack(this);
	arrayModules[pNetIoService->getModuleID()] = pNetIoService;

	if (!pNetIoService->initModule(CONFIGSERVICE) == IURDefines::UR_RETURN_ERROR)
	{
		XGLOG_ERROR("init() failed while initializing 'NetIOService' !");
		return false;
	}
	else
	{
		XGLOG_INFO("init() successfully initialized 'NetIOService' !");
		sleep(2);
		CreateTcpClient();
	}
	return true;
}

bool xGateSGWController::initSipService()
{
	// SIP Service Module
	IURModule *pSipSigProcessor = nullptr;
	URModuleFactory::CreateModuleInterface(IURDefines::UR_MODULE_SIPSIGNAL_SERVICE, pSipSigProcessor);
	if (!pSipSigProcessor)
	{
		XGLOG_ERROR("Initializing Sip Signal Processor in SGW controller failed !");
		return false;
	}

	XGConfig *psipConfig = new XGConfig();
	if (!psipConfig)
	{
		XGLOG_ERROR("Initializing configuration for Sip Signal Processor in SGW controller failed !");
		return false;
	}
	psipConfig->setCallBack(this);
	arrayModules[pSipSigProcessor->getModuleID()] = pSipSigProcessor;

	if (!pSipSigProcessor->initModule(psipConfig) == IURDefines::UR_RETURN_ERROR)
	{
		XGLOG_ERROR("init() failed while initializing 'SipSignalProcessor' !");
		return false;
	}
	else
	{
		XGLOG_INFO("init() successfully initialized 'SipSignalProcessor' !");
	}
	return true;
}

bool xGateSGWController::initMonitorService()
{
	// Monitor Service Module
	IURModule *pMonitorService = nullptr;
	URModuleFactory::CreateModuleInterface(IURDefines::UR_MODULE_MONITOR_SERVICE, pMonitorService);
	if (!pMonitorService)
	{
		XGLOG_ERROR("Initializing Monitor Service  in SGW controller failed !");
		return false;
	}

	XGConfig *pMonitorConfig = new XGConfig();
	if (!pMonitorConfig)
	{
		XGLOG_ERROR("Initializing configuration for Monitor Service in SGW controller failed !");
		return false;
	}
	pMonitorConfig->setCallBack(this);
	arrayModules[pMonitorService->getModuleID()] = pMonitorService;

	if (!pMonitorService->initModule(pMonitorConfig) == IURDefines::UR_RETURN_ERROR)
	{
		XGLOG_ERROR("init() failed while initializing 'Monitor Service' !");
		return false;
	}
	else
	{
		XGLOG_INFO("init() successfully initialized 'Monitor Service' !");
	}
	return true;
}

bool xGateSGWController::handle_msg(ACE_Message_Block *pAmb)
{
	XGLOG_FUNCTION_EN_EX("In SGW Controller thread queue");

	xGateBaseMsg *pMsg = dynamic_cast<xGateBaseMsg *>(pAmb);

	switch (pMsg->getMsgType())
	{
	case EN_XGATE_MSG_SIP_MODULE: // handle messages from Sip Service Module
	{
		XGLOG_INFO("xGateSGWController handling 'EN_XGATE_MSG_SIP_MODULE' event from sipservice");
		handleSipModuleMsg(pMsg);
		break;
	}
	case EN_XGATE_MSG_SERVICE_CONTROLLER: // handle messages from Controller Module
	{
		XGLOG_INFO("xGateSGWController handling 'EN_XGATE_MSG_SERVICE_CONTROLLER' event");
		break;
	}
	case EN_XGATE_MSG_NETIO_MODULE:
	{
		XGLOG_INFO("xGateSGWController handling 'EN_XGATE_MSG_NETIO_MODULE' event from NetIoService");
		handleNetIOModuleMsg(pMsg);
		break;
	}
	case EN_XGATE_MSG_DB_MODULE:
	{
		XGLOG_INFO("xGateSGWController handling 'EN_XGATE_MSG_DB_MODULE' event from DBService");
		handleDBModuleMsg(pMsg);
		break;
	}
	case EN_XGATE_MSG_TIMER:
	{
		XGLOG_INFO("xGateSGWController handling 'EN_XGATE_MSG_TIMER' event from ScheduleService");
		handleTimerModuleMsg(pMsg);
		break;
	}
	case EN_XGATE_MSG_UNKNOWN:
	default:
	{
		XGLOG_ERROR("handle_msg failed to handle invalid message");
		break;
	}
	}

	return true;
}

bool xGateSGWController::handleSipModuleMsg(xGateBaseMsg *pMsg)
{
	XGLOG_FUNCTION_EN_EX("");

	xGateSipMsg *ptSipmsg = dynamic_cast<xGateSipMsg *>(pMsg);

	switch (ptSipmsg->getSipEventType())
	{
	case xGateSipMsg::SIP_EVENT_DO_USER_REG_REQ:
	{
		XGLOG_INFO("xGateSGWController handling 'SIP_EVENT_DO_USER_REG_REQ' event from sipservice");
		handleUserRegistrationRequest(pMsg);
		break;
	}
	case xGateSipMsg::SIP_EVENT_USER_DEREG_REQ:
	{
		XGLOG_INFO("xGateSGWController handling 'SIP_EVENT_USER_DEREG_REQ' event from sipservice");
		handleSipDeRegRequest(pMsg);
		break;
	}
	case xGateSipMsg::SIP_EVENT_USER_REG_UPDATE_AUTH_SUCCESS:
	case xGateSipMsg::SIP_EVENT_USER_REG_UPDATE_AUTH_FAILED:
	{
		XGLOG_INFO("xGateSGWController handling 'SIP_EVENT_USER_REG_UPDATE' event from sipservice");
		handleSipUpdateUserRegRequest(pMsg);
		break;
	}
	case xGateSipMsg::SIP_EVENT_INVITE_REQUEST:
	{
		XGLOG_INFO("xGateSGWController handling 'SIP_EVENT_INVITE_REQUEST' event from sipservice");
		handleInviteRequest(pMsg);
		break;
	}
	case xGateSipMsg::SIP_EVENT_INVITE_INBOUND_PSTN_REQUEST:
	{
		XGLOG_INFO("xGateSGWController handling 'SIP_EVENT_INVITE_INBOUND_PSTN_REQUEST' event from sipservice");
		handleInviteInboundPstnRequest(pMsg);
		break;
	}
	case xGateSipMsg::SIP_EVENT_INVITE_MS_TEAMS_REQUEST:
	{
		XGLOG_INFO("xGateSGWController handling 'SIP_EVENT_INVITE_MS_TEAMS_REQUEST' event from sipservice");
		handleInviteMSTeamsRequest(pMsg);
		break;
	}
	case xGateSipMsg::SIP_EVENT_UNKNOWN:
	default:
	{
		XGLOG_ERROR("xGateSGWController failed to handle SipModule message: %d !", ptSipmsg->getSipEventType());
		break;
	}
	}

	return true;
}

bool xGateSGWController::handleSipDeRegRequest(xGateBaseMsg *pMsg)
{
	return true;
}

bool xGateSGWController::handleUserRegistrationRequest(xGateBaseMsg *pMsg)
{
	XGLOG_FUNCTION_EN_EX("");
	int con_id ;
	auto jstring = EncodeJson(pMsg,JsonOPRType::EN_JSON_REG_REQ);
	std::cout << jstring << std::endl;
	con_id = ChooseNetIOId(pMsg->getUid());
	if(con_id == -1)
	{	
		XGLOG_ERROR("xGateSGWController::handleUserRegistrationRequest transid:(%s) failed no interragator connection found !",\
					pMsg->getUid().c_str());
	}
	sendMsgToInterrogator(jstring,con_id);//sending message to netio service

	return true;
}

/*
* handle Registeration update request and send to netio servcie
*/
bool xGateSGWController::handleSipUpdateUserRegRequest(xGateBaseMsg *pMsg)
{
	XGLOG_FUNCTION_EN_EX("");

	if (!pMsg)
	{
		XGLOG_ERROR("xGateSGWController::handleSipUpdateUserRegRequest pMsg is nullptr !");
		return false;
	}

	xGateSipMsg *ptSipmsg = dynamic_cast<xGateSipMsg *>(pMsg);
	std::string jstring{""};
	int con_id;
	con_id = ChooseNetIOId(pMsg->getUid());
	if (con_id == -1)
	{
		XGLOG_ERROR("xGateSGWController::handleSipUpdateUserRegRequest transid:(%s) failed no interragator connection found !",
					pMsg->getUid().c_str());
	}
	if (ptSipmsg->getSipEventType() == xGateSipMsg::SIP_EVENT_USER_REG_UPDATE_AUTH_SUCCESS)
	{
		jstring = EncodeJson(pMsg,JsonOPRType::EN_JSON_REG_UPDATE_AUTH_SUCCESS);
	}
	else
	{
		jstring = EncodeJson(pMsg, JsonOPRType::EN_JSON_REG_UPDATE_AUTH_FAILED);
	}

	sendMsgToInterrogator(jstring,con_id);
	RemoveConID(pMsg->getUid());

	return true;
}

/*
* handle Invite request and send to netio servcie
*/
bool xGateSGWController::handleInviteRequest(xGateBaseMsg *pMsg)
{
	XGLOG_FUNCTION_EN_EX("");
	int con_id;
	if (!pMsg)
	{
		XGLOG_ERROR("xGateSGWController::handleInviteRequest pMsg is nullptr !");
	}
	auto jstring = EncodeJson(pMsg, JsonOPRType::EN_JSON_INV_REQ);
	con_id = ChooseNetIOId(pMsg->getUid());
	if(con_id == -1)
	{
		XGLOG_ERROR("xGateSGWController::handleInviteRequest transid:(%s) failed no interragator connection found !",
					pMsg->getUid().c_str());
	}
	sendMsgToInterrogator(jstring, con_id);
	RemoveConID(pMsg->getUid());

	return true;
}

/*
* handle Inbound PSTN Invite request and send to netio servcie
*/
bool xGateSGWController::handleInviteInboundPstnRequest(xGateBaseMsg *pMsg)
{
	XGLOG_FUNCTION_EN_EX("");
	int con_id;
	if (!pMsg)
	{
		XGLOG_ERROR("xGateSGWController::handleInviteInboundPstnRequest pMsg is nullptr !");
	}
	auto jstring = EncodeJson(pMsg, JsonOPRType::EN_JSON_INV_INB_PSTN_REQ);
	con_id = ChooseNetIOId(pMsg->getUid());
	if (con_id == -1)
	{
		XGLOG_ERROR("xGateSGWController::handleInviteInboundPstnRequest transid:(%s) failed no interragator connection found !",
					pMsg->getUid().c_str());
	}
	sendMsgToInterrogator(jstring, con_id);
	RemoveConID(pMsg->getUid());

	return true;
}

/*
* handle Inbound MS Teams Invite request and send to netio servcie
*/
bool xGateSGWController::handleInviteMSTeamsRequest(xGateBaseMsg *pMsg)
{
	int con_id;
	if (!pMsg)
	{
		XGLOG_ERROR("xGateSGWController::handleInviteMSTeamsRequest pMsg is nullptr !");
	}
	auto jstring = EncodeJson(pMsg, JsonOPRType::EN_JSON_INV_MS_TEAMS_REQ);
	con_id = ChooseNetIOId(pMsg->getUid());
	if (con_id == -1)
	{
		XGLOG_ERROR("xGateSGWController::handleInviteMSTeamsRequest transid:(%s) failed no interragator connection found !",
					pMsg->getUid().c_str());
	}
	sendMsgToInterrogator(jstring, con_id);
	RemoveConID(pMsg->getUid());
	return true;
}

/*
* handle NetIO Message from NetIO servcie 
*/
bool xGateSGWController::handleNetIOModuleMsg(xGateBaseMsg *pMsg)
{
	XGLOG_FUNCTION_EN_EX("");
	xGateNetIOServiceMsg *ptNetIOMsg = dynamic_cast<xGateNetIOServiceMsg *>(pMsg);

	if (!ptNetIOMsg)
	{
		XGLOG_ERROR("handleNetIOModuleMsg is NULL!!");
		return false;
	}

	switch (ptNetIOMsg->getNetIOMsgType())
	{
	case xGateNetIOMsgType::EN_XGATE_MSG_RECV:
	{
		handleNetIORecvMsg(ptNetIOMsg);
		break;
	}
	case xGateNetIOMsgType::EN_XGATE_MSG_CONN_CLOSED:
	{
		handleNetIOConClosedMsg(ptNetIOMsg);
		break;
	}
	case xGateNetIOMsgType::EN_XGATE_MSG_CLIENT_CONNECTED:
	{
		handleNetIOConnnectedMsg(ptNetIOMsg);
		break;
	}
	default:
	{
		XGLOG_ERROR("handleNetIOModuleMsg xGateNetIOMsgType Event type not found!!");
		return false;
	}
	}
	return true;
}

/*
* handle DB Message from DB servcie 
*/
bool xGateSGWController::handleDBModuleMsg(xGateBaseMsg *pMsg)
{
	return true;
}

/*
* handle Timeout from Schedule module
*/
bool xGateSGWController::handleTimerModuleMsg(xGateBaseMsg *pMsg)
{
	XGLOG_FUNCTION_EN_EX("");

	xGateSchedulerMsg *pSchMsg = dynamic_cast<xGateSchedulerMsg *>(pMsg);

	if (!pSchMsg)
	{
		XGLOG_ERROR("handleTimerModuleMsg is NULL!!");
		return false;
	}

	xGateTimerData *pTdata = pSchMsg->getTimerData();

	if (!pTdata)
	{
		XGLOG_ERROR("handleTimerModuleMsg xGateTimerData is NULL!!");
		return false;
	}
	switch(pTdata->m_timerType)
	{
		case EN_SCHEDULER_WHITE_LIST_REFRESH:
		{
			XGLOG_INFO("xGateSGWController::handleTimerModuleMsg received EN_SCHEDULER_WHITE_LIST_REFRESH");
			FetchIPToWhiteList();
			break;
		}
		default:
		{
			XGLOG_ERROR("handleTimerModuleMsg failed. invalid TimerType !");
			break;
		}
	}

	return true;
}

/*
* Creating TCP client for SGW controller
*/
bool xGateSGWController::CreateTcpClient()
{
	XGLOG_FUNCTION_EN_EX("");

	std::vector<NetIOModuleProfile> netio_profile;
	netio_profile = URENGINE_CONFIG::instance()->m_interrogatorProfileList;
	for(auto profile:netio_profile)
	{
	  xGateNetIOServiceMsg *ptNetIOMsg = new xGateNetIOServiceMsg();
	  xGateNetConnection netConnInfo;
	  netConnInfo.m_IpAdd = profile.m_sIP;
	  netConnInfo.m_Port = profile.m_iPort;
	  ptNetIOMsg->set_netConnInfo(netConnInfo);
	  ptNetIOMsg->setSrcModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
	  ptNetIOMsg->setDstModuleId(IURDefines::UR_MODULE_NETIO_SERVICE);
	  ptNetIOMsg->setNetIOMsgType(xGateNetIOMsgType::EN_XGATE_MSG_CONNECT_CLIENT);

	  this->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(ptNetIOMsg));
	}

	return true;
}

/*
* Reconnecting the TCP Client if any TCP server failure
*/
bool xGateSGWController::TcpClientReconnect(std::string sIPAddress, unsigned int iPort)
{
	XGLOG_FUNCTION_EN_EX("");

	xGateNetIOServiceMsg *ptNetIOMsg = new xGateNetIOServiceMsg();
	xGateNetConnection netConnInfo;
	netConnInfo.m_IpAdd = sIPAddress.c_str();
	netConnInfo.m_Port = iPort;
	ptNetIOMsg->set_netConnInfo(netConnInfo);
	ptNetIOMsg->setSrcModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
	ptNetIOMsg->setDstModuleId(IURDefines::UR_MODULE_NETIO_SERVICE);
	ptNetIOMsg->setNetIOMsgType(xGateNetIOMsgType::EN_XGATE_MSG_CONNECT_CLIENT);


	this->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(ptNetIOMsg));
	XGLOG_INFO("xGateSGWController::TcpClientReconnect Tcp Client Reconnection (%s) and (%d)",sIPAddress.c_str(),iPort);

	return true;
}

/*
* Get IP address from DB to white list 
*/
bool xGateSGWController::FetchIPToWhiteList()
{
	XGLOG_FUNCTION_EN_EX("");

	if(!URENGINE_CONFIG::instance()->m_lightWeightWSSProfile.m_isReadWhiteListFromDb)
	{
		XGLOG_INFO("xGateSGWController::FetchIPToWhiteList Fetching whitelist from DB disabled !");	
		return false;
	}

	xGateDBServiceMsg *ptDBMsg = new xGateDBServiceMsg();
	DBRequestInfo dbReqInfo;
	dbReqInfo.m_spName = "worktual_get_ip_domain_whitelist";
	dbReqInfo.m_dbReqMethod = DBReqMethod::EN_DB_METHOD_READ;
	// dbReqInfo.m_dbEvent = DBServiceEvent::EN_DB_EVENT_GET_WHITE_LIST_IP;
	dbReqInfo.m_contextId = "";//Todo need to add 
	dbReqInfo.m_requestorModule = IURDefines::UR_MODULE_SERVICE_CONTROLLER;
	ptDBMsg->set_db_request_info(dbReqInfo);
	ptDBMsg->setSrcModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
	ptDBMsg->setDstModuleId(IURDefines::UR_MODULE_DB_SERVICE);

	this->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(ptDBMsg));
	XGLOG_INFO("xGateSGWController::FecthIPWhiteList posted to DB Module");	

	return true;
}

/*
* Add IP/Domain list to whitelist container
*/
bool xGateSGWController::AddIPToWhiteList(std::vector<std::string> list)
{
	XGLOG_FUNCTION_EN_EX("");
	if (!list.empty())
	{
	  CONFIGSERVICE->m_lightWeightWSSProfile.addIPListToWhiteList(list);
	  return true;
	}

	XGLOG_ERROR("xGateSGWController::UpdateIPToWhiteList vector is empty!");
	return false;
}

/*
* Add IP/Domain to whitelist container
*/
bool xGateSGWController::AddIPToWhiteList(std::string ip_addr)
{
	XGLOG_FUNCTION_EN_EX("");
	if (!ip_addr.empty())
	{
	  CONFIGSERVICE->m_lightWeightWSSProfile.addIPToWhiteList(ip_addr.c_str());
	  return true;
	}
	XGLOG_ERROR("xGateSGWController::UpdateIPToWhiteList IP is empty!");
	return false;
}

/*
* Remove IP List from the white list
*/
bool xGateSGWController::RemoveIPFromWhiteList(std::vector<std::string> list)
{
	XGLOG_FUNCTION_EN_EX("");
	if(!list.empty())
	{
		CONFIGSERVICE->m_lightWeightWSSProfile.removeIPFromWhiteList(list);
		return true;
	}
	XGLOG_ERROR("xGateSGWController::RemoveIPFromWhiteList IP list is empty!");
	return false;
}

/*
* Remove IP from the white list
*/
bool xGateSGWController::RemoveIPFromWhiteList(std::string ip_addr)
{
	XGLOG_FUNCTION_EN_EX("");
	if (!ip_addr.empty())
	{
		CONFIGSERVICE->m_lightWeightWSSProfile.removeIPFromWhiteList(ip_addr.c_str());
		return true;
	}
	XGLOG_ERROR("xGateSGWController::RemoveIPFromWhiteList IP is empty!");
	return false;
}

/*
* handle NetIO Recived Message 
*/
bool xGateSGWController::handleNetIORecvMsg(xGateNetIOServiceMsg *ptNetIOMsg)
{
	XGLOG_FUNCTION_EN_EX("");

	DecodeJson(ptNetIOMsg->getNetIOmsg());
	return true;
}

/*
* handle NetIO Connection closed message 
*/
bool xGateSGWController::handleNetIOConClosedMsg(xGateNetIOServiceMsg *ptNetIOMsg)
{
	XGLOG_FUNCTION_EN_EX("");

	if (!ptNetIOMsg)
	{
	  XGLOG_ERROR("handleNetIOConClosedMsg ptNetIOMsg is empty !!");
	}
	int con_id = ptNetIOMsg->getConnectionId();
	xGateNetConnection netConnInfo = ptNetIOMsg->get_netConnInfo();

	for(auto it = m_conList.begin();it!=m_conList.end();it++)
	{
		if((*it).m_connid == con_id)
		{
			XGLOG_INFO("xGateSGWController::handleNetIOConClosedMsg removed (%s) and (%d) from connection map",netConnInfo.m_IpAdd.c_str(),netConnInfo.m_connid);
			m_conList.erase(it);
			TcpClientReconnect(netConnInfo.m_IpAdd,netConnInfo.m_Port);
			return true;
		}
	}

	XGLOG_ERROR("xGateSGWController::handleNetIOConClosedMsg (%s) and (%d) not found !",netConnInfo.m_IpAdd.c_str(),netConnInfo.m_connid);

	return false;
}

/*
* handle NetIO connected message
*/
bool xGateSGWController::handleNetIOConnnectedMsg(xGateNetIOServiceMsg *ptNetIOMsg)
{
	XGLOG_FUNCTION_EN_EX("");

	if (!ptNetIOMsg)
	{
		XGLOG_ERROR("handleNetIOConnnected ptNetIOMsg is empty !!");
	}
	xGateNetConnection netConnInfo = ptNetIOMsg->get_netConnInfo();
	int conid = ptNetIOMsg->getConnectionId();
	netConnInfo.m_connid = conid;

	XGLOG_INFO("xGateSGWController::handleNetIOConnnectedMsg received connection success status for IP:(%s) FD(%d) Conn ID (%d)",\
			   netConnInfo.m_IpAdd.c_str(), netConnInfo.m_fd, netConnInfo.m_connid);
	m_conList.push_back(netConnInfo);
	//Siva ::after interrogator connection only we need to load sip module or else some of calls will fail due to no interrogator connection
	if(!m_isSipModuleInitialized)
    { 
		initSipService();
		initMonitorService();
		m_isSipModuleInitialized = true;
	}
	return true;
}

/*
* Sending Message to NetIO service 
*/
bool xGateSGWController::sendMsgToInterrogator(std::string sMsg,int conn_id)
{
	XGLOG_FUNCTION_EN_EX("");
	// below code only for testing
	//auto it = m_conMap.find("10.150.0.184");
	xGateNetIOServiceMsg *ptNetIOMsg = new xGateNetIOServiceMsg();

	ptNetIOMsg->setNetIOmsg(sMsg);
	//ptNetIOMsg->setConnectionId(it->second);
	ptNetIOMsg->setConnectionId(conn_id);//testing purpose 
	ptNetIOMsg->setDstModuleId(IURDefines::UR_MODULE_NETIO_SERVICE);
	ptNetIOMsg->setSrcModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
	ptNetIOMsg->setNetIOMsgType(xGateNetIOMsgType::EN_XGATE_MSG_SEND);

	XGLOG_INFO("sendMsgToInterrogator sending Message to NetIO servcice connid(%d) Msg(%s)",conn_id,sMsg.c_str());
	this->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(ptNetIOMsg));

	return true;
}

/*
* Encoding the json string and sending to NetIO services
*/
std::string xGateSGWController::EncodeJson(xGateBaseMsg *pMsg, JsonOPRType jType)
{
	XGLOG_FUNCTION_EN_EX("");

	xGateSipMsg *ptSipmsg = dynamic_cast<xGateSipMsg *>(pMsg);
	std::unique_ptr<xGateJsonMsg> uptJson = std::make_unique<xGateJsonMsg>();
	std::string jstring{""};
	switch (jType)
	{
	case JsonOPRType::EN_JSON_REG_REQ:
	{
		std::unique_ptr<JsonRegMsg> uptRegJson = std::make_unique<JsonRegMsg>();
		RegCallInfo &tRegInfo = ptSipmsg->getRegInfoDetails();
		uptRegJson->m_sTransId = tRegInfo.regTransId.c_str();
		uptRegJson->m_sCallid = tRegInfo.regCallId.c_str();
		uptRegJson->m_sDomainName = tRegInfo.domainName.c_str();
		uptRegJson->m_sContactAdd = tRegInfo.contactAddress.c_str();
		uptRegJson->m_sUserName = tRegInfo.regUserName.c_str();
		uptRegJson->m_sUserID = tRegInfo.regUserId.c_str();
		uptRegJson->m_sDeviceType = tRegInfo.regDeviceType.c_str();
		uptRegJson->m_sAppType = tRegInfo.regAppType.c_str();
		uptRegJson->m_sIpAddress = tRegInfo.routeIpAddress.c_str();
		uptRegJson->m_sExpires = to_string(tRegInfo.expires);
		uptRegJson->m_sCseq = to_string(tRegInfo.cseq);
		uptRegJson->m_PuName = tRegInfo.proxyUserName.c_str();
		uptRegJson->m_sTranstype = tRegInfo.regTransportType.c_str();
		uptJson->m_uptReg = std::move(uptRegJson);
		uptJson->set_JOprType(jType);
		jstring = m_Json->Encode(std::move(uptJson));
		break;
	}
	case JsonOPRType::EN_JSON_REG_UPDATE_AUTH_SUCCESS:
	{
		std::unique_ptr<JsonRegMsg> uptRegJson = std::make_unique<JsonRegMsg>();
		RegCallInfo &tRegInfo = ptSipmsg->getRegInfoDetails();
		uptRegJson->m_sTransId = tRegInfo.regTransId.c_str();
		uptRegJson->m_sCallid = tRegInfo.regCallId.c_str();
		uptJson->m_uptReg = std::move(uptRegJson);
		uptJson->set_JOprType(jType);
		jstring = m_Json->Encode(std::move(uptJson));
		break;
	}
	case JsonOPRType::EN_JSON_REG_UPDATE_AUTH_FAILED:
	{
		std::unique_ptr<JsonRegMsg> uptRegJson = std::make_unique<JsonRegMsg>();
		RegCallInfo &tRegInfo = ptSipmsg->getRegInfoDetails();
		uptRegJson->m_sTransId = tRegInfo.regTransId.c_str();
		uptRegJson->m_sCallid = tRegInfo.regCallId.c_str();
		uptJson->m_uptReg = std::move(uptRegJson);	
		uptJson->set_JOprType(jType);
		jstring = m_Json->Encode(std::move(uptJson));
		break;
	}
	case JsonOPRType::EN_JSON_INV_REQ:
	{
		std::unique_ptr<JsonInvMsg> uptInvJson = std::make_unique<JsonInvMsg>();
		SIPCallInfo &tCallInfo = ptSipmsg->getSipInfoDetails();
		uptInvJson->m_sTransId = tCallInfo.uid.c_str();
		uptInvJson->m_sCallid = tCallInfo.callid.c_str();
		uptInvJson->m_sFromNumber = tCallInfo.m_sCallingNumber.c_str();
		uptInvJson->m_sToNumber = tCallInfo.m_sCalledNumber.c_str();
		uptInvJson->m_sDomainName = tCallInfo.m_sDomainname.c_str();
		if (!tCallInfo.m_sKey.empty())
			uptInvJson->m_sKey = tCallInfo.m_sKey.c_str();
		uptJson->m_uptInv = std::move(uptInvJson);
		uptJson->set_JOprType(jType);
		jstring = m_Json->Encode(std::move(uptJson));
		break;
	}
	case JsonOPRType::EN_JSON_INV_INB_PSTN_REQ:
	{
		std::unique_ptr<JsonInvMsg> uptInvJson = std::make_unique<JsonInvMsg>();
		SIPCallInfo &tCallInfo = ptSipmsg->getSipInfoDetails();
		uptInvJson->m_sTransId = tCallInfo.uid.c_str();
		uptInvJson->m_sCallid = tCallInfo.callid.c_str();
		uptInvJson->m_sFromNumber = tCallInfo.m_sCallingNumber.c_str();
		uptInvJson->m_sToNumber = tCallInfo.m_sCalledNumber.c_str();
		uptInvJson->m_sDomainName = tCallInfo.m_sDomainname.c_str();
		uptJson->m_uptInv = std::move(uptInvJson);
		uptJson->set_JOprType(jType);
		jstring = m_Json->Encode(std::move(uptJson));
		break;
	}
	case JsonOPRType::EN_JSON_INV_MS_TEAMS_REQ:
	{
		std::unique_ptr<JsonInvMsg> uptInvJson = std::make_unique<JsonInvMsg>();
		SIPCallInfo &tCallInfo = ptSipmsg->getSipInfoDetails();
		uptInvJson->m_sTransId = tCallInfo.uid.c_str();
		uptInvJson->m_sCallid = tCallInfo.callid.c_str();
		uptInvJson->m_sFromNumber = tCallInfo.m_sCallingNumber.c_str();
		uptInvJson->m_sToNumber = tCallInfo.m_sCalledNumber.c_str();
		uptInvJson->m_sDomainName = tCallInfo.m_sDomainname.c_str();
		if (!tCallInfo.m_sKey.empty())
			uptInvJson->m_sKey = tCallInfo.m_sKey.c_str();
		if(!tCallInfo.m_sCallingExt.empty())
			uptInvJson->m_sExt = tCallInfo.m_sCallingExt.c_str();
		uptJson->m_uptInv = std::move(uptInvJson);
		uptJson->set_JOprType(jType);
		jstring = m_Json->Encode(std::move(uptJson));
		break;
	}

	default:
		break;
	}

	return jstring;

}

/*
* Decode the Json string from NetIO service
*/
bool xGateSGWController::DecodeJson(std::string sJson)
{
	XGLOG_FUNCTION_EN_EX("");
	if (sJson.empty())
	{
		XGLOG_ERROR("xGateSGWController::DecodeJson json string is empty!");
		return false;
	}

	XGLOG_INFO("xGateSGWController::DecodeJson json(%s)", sJson.c_str());

	std::unique_ptr<xGateJsonMsg> uptJMsg = std::make_unique<xGateJsonMsg>();
	uptJMsg = m_Json->Decode(sJson);

	switch (uptJMsg->get_JOprType())
	{
	case JsonOPRType::EN_JSON_INVALID:
	{
		XGLOG_ERROR("xGateSGWController::DecodeJson INVALID JSON format");
		break;
	}
	case JsonOPRType::EN_JSON_REG_RES:
	{
		std::cout << "EN_JSON_REG_RES msg"<< sJson << std::endl;
		sendUserRegResponse(std::move(uptJMsg));
		break;
	}
	case JsonOPRType::EN_JSON_REG_RES_ERROR:
	{
		std::cout << "EN_JSON_REG_RES_ERROR msg"<< sJson << std::endl;
		sendUserRegResponseError(std::move(uptJMsg));
		break;
	}
	case JsonOPRType::EN_JSON_INV_RES:
	{
		std::cout << "EN_JSON_INV_RES msg"<< sJson << std::endl;
		sendInviteResponse(std::move(uptJMsg));
	    break;
	}
	case JsonOPRType::EN_JSON_INV_RES_ERROR:
	{
		std::cout << "EN_JSON_INV_RES_ERROR msg"<< sJson << std::endl;
		sendInviteResponseError(std::move(uptJMsg));
		break;
	}
	case JsonOPRType::EN_JSON_INV_INB_PSTN_RES:
	{
		std::cout << "EN_JSON_INV_INB_PSTN_RES msg"<< sJson << std::endl;
		sendInviteInboundPstnResponse(std::move(uptJMsg));
		break;
	}
	case JsonOPRType::EN_JSON_INV_INB_PSTN_RES_ERROR :
	{
		std::cout << "EN_JSON_INV_INB_PSTN_RES_ERROR msg"<< sJson << std::endl;
		sendInviteInboundPstnResponseError(std::move(uptJMsg));
		break;
	}
	case JsonOPRType::EN_JSON_INV_MS_TEAMS_RES:
	{
		std::cout << "EN_JSON_INV_MS_TEAMS_RES msg"<< sJson << std::endl;
		sendInviteMSTeamsResponse(std::move(uptJMsg));
		break;
	}
	case JsonOPRType::EN_JSON_ADD_TRUNK_IP_TO_WL:
	{
		std::cout << "EN_JSON_ADD_TRUNK_IP_TO_WL  msg"<< sJson << std::endl;
		sendTrunkAddRequest(std::move(uptJMsg));
	    break;
	}
	case JsonOPRType::EN_JSON_REMOVE_TRUNK_IP_FROM_WL:
	{
		std::cout << "EN_JSON_REMOVE_TRUNK_IP_FROM_WL  msg"<< sJson << std::endl;
		sendTrunkRemoveRequest(std::move(uptJMsg));
		break;
	}
	case JsonOPRType::EN_JSON_ADD_MS_TEAMS_DOMAIN_TO_WL:
	{
		std::cout << "EN_JSON_REMOVE_TRUNK_IP_FROM_WL  msg"<< sJson << std::endl;
		AddIPToWhiteList(uptJMsg->m_uptWL->m_sSbcFqdn);
		AddSbcFqdnToMsTeamsDomainList(uptJMsg->m_uptWL->m_sSbcFqdn);
		break;
	}
	case JsonOPRType::EN_JSON_REMOVE_MS_TEAMS_DOMAIN_FROM_WL:
	{
		std::cout << "EN_JSON_REMOVE_MS_TEAMS_DOMAIN_FROM_WL  msg"<< sJson << std::endl;
		RemoveIPFromWhiteList(uptJMsg->m_uptWL->m_sSbcFqdn);
		RemoveSbcFqdnFromMsTeamsDomainList(uptJMsg->m_uptWL->m_sSbcFqdn);
		break;
	}
	default:
		break;
	}
	return true;
}

/*
* Sending Registeration response message to Wss sip siganl 
*/
bool xGateSGWController::sendUserRegResponse(std::unique_ptr<xGateJsonMsg> uptJsonMsg)
{
	XGLOG_FUNCTION_EN_EX("");

	if(!uptJsonMsg)
	{
		XGLOG_ERROR("sendUserRegResponse failed becase uptJsonMsg is nullptr");
		return false;
	}

	xGateSipMsg *ptSipMsg = new xGateSipMsg(IURDefines::UR_MODULE_SERVICE_CONTROLLER,
										  IURDefines::UR_MODULE_SIPSIGNAL_SERVICE,
										  xGateSipMsg::SIP_EVENT_TYPE::SIP_EVENT_DO_USER_REG_RES,
										  xGateSipMsg::SIP_MSG_TYPE::SIP_MSG_COMMAND);
	if(!ptSipMsg)
	{
		XGLOG_ERROR("OOM Issue. sendUserRegResponse failed to create xGateSipMsg memory! !");
		return false;
	}

	std::unique_ptr<JsonRegMsg> uptReg = std::make_unique<JsonRegMsg>();
	uptReg = std::move(uptJsonMsg->m_uptReg);
	ptSipMsg->setUid(uptReg->m_sTransId.c_str());
	//ptSipMsg->setAorDetails(uptReg->m_sAor.c_str());
	ptSipMsg->setUserPassword(uptReg->m_sPassword.c_str());
	ptSipMsg->setDstModuleId(IURDefines::UR_MODULE_SIPSIGNAL_SERVICE);
	ptSipMsg->setMsgType(EN_XGATE_MSG_SIP_MODULE);

	this->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(ptSipMsg));

	return true;
}

/*
* Sending Registeration response error message to Wss sip siganl 
*/
bool xGateSGWController::sendUserRegResponseError(std::unique_ptr<xGateJsonMsg> uptJsonMsg)
{
	XGLOG_FUNCTION_EN_EX("");

	if (!uptJsonMsg)
	{
		XGLOG_ERROR("sendUserRegResponseError failed because uptJsonMsg is nullptr");
		return false;
	}

	xGateSipMsg *ptSipMsg = new xGateSipMsg(IURDefines::UR_MODULE_SERVICE_CONTROLLER,
											IURDefines::UR_MODULE_SIPSIGNAL_SERVICE,
											xGateSipMsg::SIP_EVENT_TYPE::SIP_EVENT_USER_REG_RESPONSE_ERROR,
											xGateSipMsg::SIP_MSG_TYPE::SIP_MSG_COMMAND);

	std::unique_ptr<JsonRegMsg> uptReg = std::make_unique<JsonRegMsg>();
	uptReg = std::move(uptJsonMsg->m_uptReg);
	ptSipMsg->setUid(uptReg->m_sTransId.c_str());
	ptSipMsg->setDstModuleId(IURDefines::UR_MODULE_SIPSIGNAL_SERVICE);
	ptSipMsg->setMsgType(EN_XGATE_MSG_SIP_MODULE);

	this->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(ptSipMsg));

	return true;
}

/*
* Sending Invite response message to Wss sip siganl 
*/
bool xGateSGWController::sendInviteResponse(std::unique_ptr<xGateJsonMsg> uptJsonMsg)
{
	XGLOG_FUNCTION_EN_EX("");

	if (!uptJsonMsg)
	{
		XGLOG_ERROR("sendUserRegResponseError failed because uptJsonMsg is nullptr");
		return false;
	}

	xGateSipMsg *ptSipMsg = new xGateSipMsg(IURDefines::UR_MODULE_SERVICE_CONTROLLER,
											IURDefines::UR_MODULE_SIPSIGNAL_SERVICE,
											xGateSipMsg::SIP_EVENT_TYPE::SIP_EVENT_INVITE_RESPONSE,
											xGateSipMsg::SIP_MSG_TYPE::SIP_MSG_COMMAND);

	std::unique_ptr<JsonInvMsg> uptInv = std::make_unique<JsonInvMsg>();
	uptInv = std::move(uptJsonMsg->m_uptInv);
	ptSipMsg->setUid(uptInv->m_sTransId.c_str());
	ptSipMsg->setRouteIP(uptInv->m_sRouteIP);
	ptSipMsg->setUserPassword(uptInv->m_sPassword.c_str());
	if(!uptInv->m_sCallInfo.empty())
		ptSipMsg->setCallInfo(uptInv->m_sCallInfo.c_str());
	ptSipMsg->setDstModuleId(IURDefines::UR_MODULE_SIPSIGNAL_SERVICE);
	ptSipMsg->setMsgType(EN_XGATE_MSG_SIP_MODULE);

	this->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(ptSipMsg));

	return true;
}

/*
 * Sending Invite response message to Wss sip siganl
 */
bool xGateSGWController::sendInviteResponseError(std::unique_ptr<xGateJsonMsg> uptJsonMsg)
{
	XGLOG_FUNCTION_EN_EX("");

	if (!uptJsonMsg)
	{
		XGLOG_ERROR("sendInviteResponseError failed because uptJsonMsg is nullptr");
		return false;
	}

	xGateSipMsg *ptSipMsg = new xGateSipMsg(IURDefines::UR_MODULE_SERVICE_CONTROLLER,
											IURDefines::UR_MODULE_SIPSIGNAL_SERVICE,
											xGateSipMsg::SIP_EVENT_TYPE::SIP_EVENT_INVITE_RESPONSE_ERROR,
											xGateSipMsg::SIP_MSG_TYPE::SIP_MSG_COMMAND);

	std::unique_ptr<JsonInvMsg> uptInv = std::make_unique<JsonInvMsg>();
	uptInv = std::move(uptJsonMsg->m_uptInv);
	ptSipMsg->setUid(uptInv->m_sTransId.c_str());
	ptSipMsg->setDstModuleId(IURDefines::UR_MODULE_SIPSIGNAL_SERVICE);
	ptSipMsg->setMsgType(EN_XGATE_MSG_SIP_MODULE);

	this->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(ptSipMsg));

	return true;
}

/*
 * Sending Invite Inbound PSTN response message to Wss sip siganl
 */
bool xGateSGWController::sendInviteInboundPstnResponse(std::unique_ptr<xGateJsonMsg> uptJsonMsg)
{
	XGLOG_FUNCTION_EN_EX("");

	if (!uptJsonMsg)
	{
		XGLOG_ERROR("sendInviteInboundPstnResponse failed because uptJsonMsg is nullptr");
		return false;
	}

	xGateSipMsg *ptSipMsg = new xGateSipMsg(IURDefines::UR_MODULE_SERVICE_CONTROLLER,
											IURDefines::UR_MODULE_SIPSIGNAL_SERVICE,
											xGateSipMsg::SIP_EVENT_TYPE::SIP_EVENT_INVITE_INBOUND_PSTN_RESPONSE,
											xGateSipMsg::SIP_MSG_TYPE::SIP_MSG_COMMAND);

	std::unique_ptr<JsonInvMsg> uptInv = std::make_unique<JsonInvMsg>();
	uptInv = std::move(uptJsonMsg->m_uptInv);
	ptSipMsg->setUid(uptInv->m_sTransId.c_str());
	ptSipMsg->setRouteIP(uptInv->m_sRouteIP);
	if(!uptInv->m_sCallInfo.empty())
		ptSipMsg->setCallInfo(uptInv->m_sCallInfo.c_str());
	ptSipMsg->setDstModuleId(IURDefines::UR_MODULE_SIPSIGNAL_SERVICE);
	ptSipMsg->setMsgType(EN_XGATE_MSG_SIP_MODULE);

	this->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(ptSipMsg));

	return true;
}

bool xGateSGWController::sendInviteMSTeamsResponse(std::unique_ptr<xGateJsonMsg> uptJsonMsg)
{
	XGLOG_FUNCTION_EN_EX("");

	if (!uptJsonMsg)
	{
		XGLOG_ERROR("sendInviteInboundPstnResponse failed because uptJsonMsg is nullptr");
		return false;
	}

	xGateSipMsg *ptSipMsg = new xGateSipMsg(IURDefines::UR_MODULE_SERVICE_CONTROLLER,
											IURDefines::UR_MODULE_SIPSIGNAL_SERVICE,
											xGateSipMsg::SIP_EVENT_TYPE::SIP_EVENT_INVITE_MS_TEAMS_RESPONSE,
											xGateSipMsg::SIP_MSG_TYPE::SIP_MSG_COMMAND);

	std::unique_ptr<JsonInvMsg> uptInv = std::make_unique<JsonInvMsg>();
	uptInv = std::move(uptJsonMsg->m_uptInv);
	ptSipMsg->setUid(uptInv->m_sTransId.c_str());
	ptSipMsg->setRouteIP(uptInv->m_sRouteIP);
	if(!uptInv->m_sCallInfo.empty())
		ptSipMsg->setCallInfo(uptInv->m_sCallInfo.c_str());
	ptSipMsg->setDstModuleId(IURDefines::UR_MODULE_SIPSIGNAL_SERVICE);
	ptSipMsg->setMsgType(EN_XGATE_MSG_SIP_MODULE);

	this->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(ptSipMsg));

	return true;
}

/*
 * Sending Invite Inbound PSTN response message to Wss sip siganl
 */
bool xGateSGWController::sendInviteInboundPstnResponseError(std::unique_ptr<xGateJsonMsg> uptJsonMsg)
{
	XGLOG_FUNCTION_EN_EX("");

	if (!uptJsonMsg)
	{
		XGLOG_ERROR("sendInviteInboundPstnResponseError failed because uptJsonMsg is nullptr");
		return false;
	}
    xGateSipMsg *ptSipMsg = new xGateSipMsg(IURDefines::UR_MODULE_SERVICE_CONTROLLER,
											IURDefines::UR_MODULE_SIPSIGNAL_SERVICE,
											xGateSipMsg::SIP_EVENT_TYPE::SIP_EVENT_INVITE_INBOUND_PSTN_ERROR,
											xGateSipMsg::SIP_MSG_TYPE::SIP_MSG_COMMAND);

    std::unique_ptr<JsonInvMsg> uptInv = std::make_unique<JsonInvMsg>();
	uptInv = std::move(uptJsonMsg->m_uptInv);
	ptSipMsg->setUid(uptInv->m_sTransId.c_str());
	ptSipMsg->setDstModuleId(IURDefines::UR_MODULE_SIPSIGNAL_SERVICE);
	ptSipMsg->setMsgType(EN_XGATE_MSG_SIP_MODULE);

    this->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(ptSipMsg));
	return true;
}

/*
 * Send trunk add new request to monitor service
 */
bool xGateSGWController::sendTrunkAddRequest(std::unique_ptr<xGateJsonMsg> uptJsonMsg)
{
	XGLOG_FUNCTION_EN_EX("");

	if (!uptJsonMsg)
	{
		XGLOG_ERROR("sendTrunkAddRequest failed because uptJsonMsg is nullptr");
		return false;
	}

	xGateMonitorServiceMsg *pMntrMsg = new xGateMonitorServiceMsg();
	xGateMonitorInfo mntrInfo;
	std::unique_ptr<JsonWLUpdateMsg> uptWL = std::make_unique<JsonWLUpdateMsg>();
	uptWL = std::move(uptJsonMsg->m_uptWL);
	mntrInfo.m_iTrunkId = uptWL->m_iTrunkId;
	mntrInfo.m_IpAddressList = uptWL->m_IPAddrList;//copy the list of ip address
	mntrInfo.m_sDcType = uptWL->m_sDCType.c_str();
	mntrInfo.m_TrunkEvent = xGateMonitorEvent::EN_MONITOR_ADD_NEW_TRUNK;
	pMntrMsg->setMonitorInfo(mntrInfo);
	pMntrMsg->setSrcModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
	pMntrMsg->setDstModuleId(IURDefines::UR_MODULE_MONITOR_SERVICE);
	pMntrMsg->setMsgType(EN_XGATE_MSG_NETIO_MODULE);

	this->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(pMntrMsg));
	return true;
}

/*
 * Send trunk remove request to monitor service
 */
bool xGateSGWController::sendTrunkRemoveRequest(std::unique_ptr<xGateJsonMsg> uptJsonMsg)
{
	XGLOG_FUNCTION_EN_EX("");

	if (!uptJsonMsg)
	{
		XGLOG_ERROR("sendTrunkAddRequest failed because uptJsonMsg is nullptr");
		return false;
	}

	xGateMonitorServiceMsg *pMntrMsg = new xGateMonitorServiceMsg();
	xGateMonitorInfo mntrInfo;
	std::unique_ptr<JsonWLUpdateMsg> uptWL = std::make_unique<JsonWLUpdateMsg>();
	uptWL = std::move(uptJsonMsg->m_uptWL);
	mntrInfo.m_iTrunkId = uptWL->m_iTrunkId;
	mntrInfo.m_IpAddressList = uptWL->m_IPAddrList; // copy the list of ip address
	mntrInfo.m_sDcType = uptWL->m_sDCType.c_str();
	mntrInfo.m_TrunkEvent = xGateMonitorEvent::EN_MONITOR_REMOVE_TRUNK;
	pMntrMsg->setMonitorInfo(mntrInfo);
	pMntrMsg->setSrcModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
	pMntrMsg->setDstModuleId(IURDefines::UR_MODULE_MONITOR_SERVICE);
	pMntrMsg->setMsgType(EN_XGATE_MSG_NETIO_MODULE);

	this->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(pMntrMsg));
	return true;
}

/*
* Choose Interrogator for Con ID
*/
int xGateSGWController::ChooseNetIOId(std::string trans_id)
{
	if (m_conList.empty())
		return -1;
	int con_id = 0;
	auto it = m_conMap.find(trans_id);
	if (it == m_conMap.end())
	{
		std::vector<xGateNetConnection>::iterator it;
		it = m_conList.begin();
		con_id = it->m_connid;
		std::shuffle(std::begin(m_conList), std::end(m_conList), std::default_random_engine()); // shuffle the vector for round robin
		m_conMap.insert({trans_id, con_id});
	}
	else
	{
		if (!m_conMap.empty())
			con_id = it->second;
	}
	return con_id;
}

/*
* Remove Con ID from connection map
*/
bool xGateSGWController::RemoveConID(std::string trans_id)
{
	if (m_conMap.erase(trans_id))
	{
		XGLOG_INFO("xGateSGWController::RemoveConID Removed trans_id(%s) from Connection Map",trans_id.c_str());
	}
	else
	{
		XGLOG_ERROR("xGateSGWController::RemoveConID trans_id(%s) not found from Connection Map",trans_id.c_str());
	}
	return true;
}

/*
* Add SBC FQDN to MS Teams list container
*/
bool xGateSGWController::AddSbcFqdnToMsTeamsDomainList(std::string sbc_fqdn)
{
	XGLOG_FUNCTION_EN_EX("");
	if (!sbc_fqdn.empty())
	{
	  CONFIGSERVICE->m_lightWeightWSSProfile.addSbcFqdnToTeamsDomainList(sbc_fqdn.c_str());
	  return true;
	}
	XGLOG_ERROR("xGateSGWController::AddSbcFqdnToMsTeamsDomainList sbc_fqdn is empty!");
	return false;
}

/*
* Add SBC FQDN to MS Teams list container
*/
bool xGateSGWController::RemoveSbcFqdnFromMsTeamsDomainList(std::string sbc_fqdn)
{
	XGLOG_FUNCTION_EN_EX("");
	if (!sbc_fqdn.empty())
	{
	  CONFIGSERVICE->m_lightWeightWSSProfile.removeSbcFqdnFromTeamsDomainList(sbc_fqdn.c_str());
	  return true;
	}
	XGLOG_ERROR("xGateSGWController::RemoveSbcFqdnFromMsTeamsDomainList sbc_fqdn is empty!");
	return false;
}