#include "xGateMonitorHandler.h"

xGateMonitorHandler::xGateMonitorHandler(typename XGSingleton<xGateMonitorHandler>::token) {}

xGateMonitorHandler::~xGateMonitorHandler() {}

/*
 * Initializing handler class
 */
bool xGateMonitorHandler::init()
{
    XGLOG_INFO("xGateMonitorHandler::init() success");
    pCtxHndlr = new MonitorCtxHandler();
    FetchTrunkDetailsFromDBSrv();
    FetchMSTeamsDomainListFromDBSrv();
    return true;
}

/*
 * Processing messages from service class
 */
bool xGateMonitorHandler::ProcessMsg(xGateBaseMsg *pMsg)
{
    XGLOG_INFO("xGateDBHandler::ProcessMsg");

    IURDefines::MODULE_ID srcModuleId = pMsg->getSrcModuleId();
    switch (srcModuleId)
    {
    case IURDefines::UR_MODULE_SERVICE_CONTROLLER:
    {
        return ProcessMsgFromCtrl(pMsg);
    }
    case IURDefines::UR_MODULE_DB_SERVICE:
    {
        return ProcessMsgFromDBSrv(pMsg);
    }
    case IURDefines::UR_MODULE_SIPSIGNAL_SERVICE:
    {
        return ProcessMsgFromSipSrv(pMsg);
    }
    case IURDefines::UR_MODULE_SCHEDULER_SERVICE:
    {
        return ProcessMsgFromSchedulerSrv(pMsg);
    }
    default:
    {
        XGLOG_ERROR("ProcessMsg failed. Unknown source module id !");
        return false;
    }
    }
    return true;
}

/*
 * Processing messages from SGW controller
 */
bool xGateMonitorHandler::ProcessMsgFromCtrl(xGateBaseMsg *pBaseMsg)
{
    XGLOG_FUNCTION_EN_EX("");

    if (!pBaseMsg)
    {
        XGLOG_ERROR("xGateMonitorHandler::ProcessMsgFromCtrl pBaseMsg is  NULL !");
        return false;
    }

    switch (pBaseMsg->getMsgType())
    {
    case EN_XGATE_MSG_NETIO_MODULE:
    {
        XGLOG_INFO("xGateMonitorHandler::ProcessMsgFromCtrl received EN_XGATE_MSG_NETIO_MODULE");
        ProcessMsgFromNetIOSrv(pBaseMsg);
        break;
    }
    default:
        break;
    }
    return true;
}

/*
 * Processing messages from DB service
 */
bool xGateMonitorHandler::ProcessMsgFromDBSrv(xGateBaseMsg *pBaseMsg)
{
    XGLOG_FUNCTION_EN_EX("");

    if (!pBaseMsg)
    {
        XGLOG_ERROR("xGateMonitorHandler::ProcessMsgFromDBSrv pBaseMsg is  NULL !");
        return false;
    }

    xGateDBServiceMsg *pDBMsg = dynamic_cast<xGateDBServiceMsg *>(pBaseMsg);
    DBRequestInfo reqinfo;
    reqinfo = pDBMsg->get_db_request_info();

    switch (reqinfo.m_dbEvent)
    {
    case DBServiceEvent::EN_DB_EVENT_GET_TRUNK_LIST:
    {
        handleTrunkLoadMsg(reqinfo);
        break;
    }
    case DBServiceEvent::EN_DB_EVENT_GET_MSTEAMS_DOMAIN_LIST:
    {
        handleMsTeamsDomainListMsg(reqinfo);
        break;
    }
    default:
        XGLOG_ERROR("xGateMonitorHandler::ProcessMsgFromDBSrv unknown event");
        break;
    }

    return true;
}

/*
 * Processing messages from SIP service
 */
bool xGateMonitorHandler::ProcessMsgFromSipSrv(xGateBaseMsg *pBaseMsg)
{
    XGLOG_FUNCTION_EN_EX("");

    if (!pBaseMsg)
    {
        XGLOG_ERROR("xGateMonitorHandler::ProcessMsgFromSchedulerSrv pBaseMsg is nullptr !");
        return false;
    }

    xGateSipMsg *pSipMsg = dynamic_cast<xGateSipMsg *>(pBaseMsg);

    switch (pSipMsg->getSipEventType())
    {
    case xGateSipMsg::SIP_EVENT_OPTION_SUCCESS_RESPONSE:
    {
        XGLOG_INFO("xGateMonitorHandler::ProcessMsgFromSipSrv received SIP_EVENT_OPTION_SUCCESS_RESPONSE");
        handleOptionsSuccessMsg(pSipMsg);
        break;
    }
    case xGateSipMsg::SIP_EVENT_OPTIONS_FAILED_RESPONSE:
    {
        XGLOG_INFO("xGateMonitorHandler::ProcessMsgFromSipSrv received SIP_EVENT_OPTIONS_FAILED_RESPONSE");
        handleOptionsFailedMsg(pSipMsg);
        break;
    }
    case xGateSipMsg::SIP_EVENT_OPTIONS_TIMEOUT_RESPONSE:
    {
        XGLOG_INFO("xGateMonitorHandler::ProcessMsgFromSipSrv received SIP_EVENT_OPTIONS_TIMEOUT_RESPONSE");
        handleOptionsTimeoutMsg(pSipMsg);
        break;
    }
    default:
    {
        XGLOG_ERROR("xGateMonitorHandler::ProcessMsgFromSipSrv unknown event");
        break;
    }
    }

    return true;
}

/*
 * Processing messages from scheduler service
 */
bool xGateMonitorHandler::ProcessMsgFromSchedulerSrv(xGateBaseMsg *pBaseMsg)
{
    XGLOG_FUNCTION_EN_EX("");
    if (!pBaseMsg)
    {
        XGLOG_ERROR("xGateMonitorHandler::ProcessMsgFromSchedulerSrv pBaseMsg is nullptr !");
    }

    xGateSchedulerMsg *pSchedulerMsg = dynamic_cast<xGateSchedulerMsg *>(pBaseMsg);

    if (!pSchedulerMsg)
    {
        XGLOG_ERROR("xGateMonitorHandler::ProcessMsgFromSchedulerSrv failed. Invalid SchedulerMsg pointer !");
        return false;
    }

    xGateTimerData *data = pSchedulerMsg->getTimerData();
    if (!data)
    {
        XGLOG_ERROR("xGateMonitorHandler::ProcessMsgFromSchedulerSrv failed. invalid xGateTimerData !");
        return false;
    }

    switch (data->m_timerType)
    {
    case EN_SCHEDULER_OPTIONS_TIMER:
    {
        XGLOG_INFO("xGateMonitorHandler::ProcessMsgFromSchedulerSrv received EN_SCHEDULER_OPTIONS_TIMER timeout");
        handleSchedulerTimeoutMsg(data);
        break;
    }
    default:
    {
        XGLOG_ERROR("xGateMonitorHandler::ProcessMsgFromSchedulerSrv unknown event");
        break;
    }
    }
    delete_obj(data);
    return true;
}

/*
 * Processing messages from scheduler service
 */
bool xGateMonitorHandler::ProcessMsgFromNetIOSrv(xGateBaseMsg *pBaseMsg)
{
    XGLOG_FUNCTION_EN_EX("");
    xGateMonitorServiceMsg *pMntrMsg = dynamic_cast<xGateMonitorServiceMsg *>(pBaseMsg);
    xGateMonitorInfo mntrInfo;
    mntrInfo = pMntrMsg->getMonitorInfo();
    switch (mntrInfo.m_TrunkEvent)
    {
    case xGateMonitorEvent::EN_MONITOR_ADD_NEW_TRUNK:
    {
        XGLOG_INFO("xGateMonitorHandler::ProcessMsgFromNetIOSrv received EN_MONITOR_ADD_NEW_TRUNK");
        addNewTrunk(mntrInfo);
        break;
    }
    case xGateMonitorEvent::EN_MONITOR_REMOVE_TRUNK:
    {
        XGLOG_INFO("xGateMonitorHandler::ProcessMsgFromNetIOSrv received EN_MONITOR_REMOVE_TRUNK");
        removeTrunk(mntrInfo);
        break;
    }
    default:
        XGLOG_ERROR("xGateMonitorHandler::ProcessMsgFromCtrl unknown event");
        break;
    }
    return true;
}

/*
 * Fetch trunk details from DB service
 */
bool xGateMonitorHandler::FetchTrunkDetailsFromDBSrv()
{
    XGLOG_FUNCTION_EN_EX("");

    xGateDBServiceMsg *ptDBMsg = new xGateDBServiceMsg();

    if(!ptDBMsg)
    {
        XGLOG_ERROR("xGateMonitorHandler::FetchTrunkDetailsFromDBSrv OOM issue occured !");
        return false;
    }

    DBRequestInfo dbReqInfo;
    dbReqInfo.m_spName = "worktual_get_ip_domain_whitelist";
    dbReqInfo.m_dbReqMethod = DBReqMethod::EN_DB_METHOD_READ;
    dbReqInfo.m_dbEvent = DBServiceEvent::EN_DB_EVENT_GET_TRUNK_LIST;
    dbReqInfo.m_contextId = ""; // Todo need to add
    dbReqInfo.m_requestorModule = IURDefines::UR_MODULE_MONITOR_SERVICE;
    ptDBMsg->set_db_request_info(dbReqInfo);
    ptDBMsg->setSrcModuleId(IURDefines::UR_MODULE_MONITOR_SERVICE);
    ptDBMsg->setDstModuleId(IURDefines::UR_MODULE_DB_SERVICE);

    MNTRSERVICE->getConfig()->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(ptDBMsg));
    XGLOG_INFO("xGateMonitorHandler::FetchTrunkDetailsFromDBSrv posted to DB Module");
    return true;
}

/*
 * Send Options Request to SIP service for Trunk Monitoring
 */
bool xGateMonitorHandler::sendOptionsRequestToSipSrv(MonitorContext &mntrctx)
{
    XGLOG_FUNCTION_EN_EX("");

    xGateSipMsg *ptSipMsg = new xGateSipMsg(IURDefines::UR_MODULE_MONITOR_SERVICE,
                                            IURDefines::UR_MODULE_SIPSIGNAL_SERVICE,
                                            xGateSipMsg::SIP_EVENT_TYPE::SIP_EVENT_OPTIONS_REQUEST,
                                            xGateSipMsg::SIP_MSG_TYPE::SIP_MSG_COMMAND);
    std::string uid = XGUTIL->generateTransid();
    ptSipMsg->setUid(uid);
    ptSipMsg->setTrunkId(mntrctx.m_trunkInfo.m_iTrunk_id);
    ptSipMsg->setTrunkIPAddress(mntrctx.m_trunkInfo.m_sIpAddress);
    ptSipMsg->setTrunkUname("siptest");
    ptSipMsg->setDstModuleId(IURDefines::UR_MODULE_SIPSIGNAL_SERVICE);
    ptSipMsg->setMsgType(EN_XGATE_MSG_MONITOR_MODULE);
    mntrctx.m_trunkStatus = xGateTrunkStatus::EN_TRUNK_STATUS_OPTIONS_SENT;
    MNTRSERVICE->getConfig()->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(ptSipMsg));

    XGLOG_INFO("xGateMonitorHandler::sendOptionsRequestToSipSrv OPTIONS request sent for uid(%s) TrunkId(%d) IpAddress(%s) ",
             uid.c_str(), mntrctx.m_trunkInfo.m_iTrunk_id,mntrctx.m_trunkInfo.m_sIpAddress.c_str());
    return true;
}

/*
 * Send start timer request to scheduler service
 */
bool xGateMonitorHandler::startTimerRequestToSchedulerSrv(MonitorContext &mntrctx)
{
    XGLOG_FUNCTION_EN_EX("");

    auto OptionsTimeoutVal = URENGINE_CONFIG::instance()->m_lightWeightWSSProfile.m_sipOptionsTimeout;
    if (OptionsTimeoutVal <= 0)
    {
        XGLOG_ERROR("xGateMonitorHandler::startTimerRequestToSchedulerSrv failed to set the Option timeout value ! OptionstimeoutVal(%d) ", OptionsTimeoutVal);
        return false;
    }

    xGateSchedulerServiceMsg *pSchedulerMsg = new xGateSchedulerServiceMsg();

    if (!pSchedulerMsg)
    {
        XGLOG_ERROR("xGateMonitorHandler::startTimerRequestToSchedulerSrv failed to create xGateSchedulerServiceMsg!");
        return false;
    }

    MonitorContext *pMntrctx = new MonitorContext();
    pMntrctx->m_trunkInfo = mntrctx.m_trunkInfo;

    SchedulerInfo schedulerInfo;
    schedulerInfo.m_contextId = XGUTIL->generateTransid();
    schedulerInfo.m_schedulerType = EN_SCHEDULER_OPTIONS_TIMER;
    schedulerInfo.m_retryCount = 0;
    schedulerInfo.m_timeoutSecond = OptionsTimeoutVal;
    schedulerInfo.m_pCtxdata = (void *)pMntrctx;
    schedulerInfo.m_requestorModule = IURDefines::UR_MODULE_MONITOR_SERVICE;
    schedulerInfo.m_enumTimerFunc = SCHEDULER_FUNCTION_SET_TIMER;
    pSchedulerMsg->setUid(schedulerInfo.m_contextId);
    pSchedulerMsg->setDstModuleId(IURDefines::UR_MODULE_SCHEDULER_SERVICE);
    pSchedulerMsg->setSrcModuleId(IURDefines::UR_MODULE_MONITOR_SERVICE);
    pSchedulerMsg->set_scheduler_info(schedulerInfo);

    XGLOG_INFO("xGateMonitorHandler::startTimerRequestToSchedulerSrv for TrunkId(%d) IpAddress(%s) ", pMntrctx->m_trunkInfo.m_iTrunk_id , pMntrctx->m_trunkInfo.m_sIpAddress.c_str());
    MNTRSERVICE->getConfig()->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(pSchedulerMsg));

    return true;
}

/*
 * Update Trunk status to DB
 */
bool xGateMonitorHandler::updateTrunkStatusToDB(MonitorContext &mntrctx, int status, int respcode, std::string uid)
{

    XGLOG_FUNCTION_EN_EX("");

    std::ostringstream oss{};
    std::string sLocalIP = CONFIGSERVICE->m_lightWeightWSSProfile.m_contactWssIp.c_str();
     std::string sDcType = URENGINE_CONFIG::instance()->m_lightWeightWSSProfile.m_dcType.c_str();

    oss << jbegin << "trunk_id" << jicolon << mntrctx.m_trunkInfo.m_iTrunk_id << jicomma
        << "dc_type" << jcolon << sDcType << jcomma
        << "provider_ip" << jcolon << mntrctx.m_trunkInfo.m_sIpAddress << jcomma
        << "local_ip" << jcolon << sLocalIP << jcomma
        << "isActive" << jicolon << status << jicomma
        << "respons_code" << jicolon << respcode << jiend;

    xGateDBServiceMsg *ptDBMsg = new xGateDBServiceMsg();

    if (!ptDBMsg)
    {
        XGLOG_ERROR("xGateMonitorHandler::updateTrunkStatusToDB OOM issue occured !");
        return false;
    }

    DBRequestInfo dbReqInfo;
    dbReqInfo.m_spName = "worktual_InsertIPConnectivity_info";
    dbReqInfo.m_dbReqMethod = DBReqMethod::EN_DB_METHOD_READ;
    dbReqInfo.m_dbEvent = DBServiceEvent::EN_DB_EVENT_UPDATE_TRUNK_STATUS;
    dbReqInfo.m_contextId = uid.c_str();
    dbReqInfo.m_data = oss.str();
    dbReqInfo.m_requestorModule = IURDefines::UR_MODULE_MONITOR_SERVICE;
    ptDBMsg->set_db_request_info(dbReqInfo);
    ptDBMsg->setSrcModuleId(IURDefines::UR_MODULE_MONITOR_SERVICE);
    ptDBMsg->setDstModuleId(IURDefines::UR_MODULE_DB_SERVICE);

    MNTRSERVICE->getConfig()->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(ptDBMsg));
    XGLOG_INFO("xGateMonitorHandler::updateTrunkStatusToDB posted to DB Module");

    return true;
}

/*
 * Update Trunk status to NetIo
 */
bool xGateMonitorHandler::updateTrunkStatusToNetIO(MonitorContext &mntrctx)
{
    return true;
}

/*
 * handle Trunk load DB message
 */
bool xGateMonitorHandler::handleTrunkLoadMsg(DBRequestInfo &reqinfo)
{
    XGLOG_FUNCTION_EN_EX("");
    std::string data = reqinfo.m_data.c_str();
    std::string dcType =  CONFIGSERVICE->m_lightWeightWSSProfile.m_dcType.c_str();

    if (data.length() <= 0)
    {
        XGLOG_ERROR("xGateMonitorHandler::handleTrunkLoadMsg data is empty!");
        return false;
    }

    Document doc;
    if (!parseJsonMsg(data.c_str(), doc))
    {
        XGLOG_ERROR("handleTrunkLoadMsg failed to parse DB message !");
        return false;
    }

    // Lamda Expression to print the error
    auto parsing_failed = [&](std::string error) -> bool
    {
        XGLOG_ERROR("xGateMonitorHandler::handleTrunkLoadMsg parsing failed '%s' is not available from json !", error.c_str());
        return false;
    };

    for (SizeType i = 0; i < doc.Size(); ++i)
    {
        const Value &value = doc[i];
        MonitorContext *mntrctx = new MonitorContext();

        if (value.HasMember("domain_whitelist")) // ip address
            mntrctx->m_trunkInfo.m_sIpAddress = value["domain_whitelist"].GetString();
        else
            return parsing_failed("domain_whitelist");

        if (value.HasMember("dc_type"))
            mntrctx->m_trunkInfo.m_sDcType = value["dc_type"].GetString();
        else
            return parsing_failed("dc_type");

        if (value.HasMember("isActive"))
            mntrctx->m_trunkInfo.isActive = value["isActive"].GetInt();
        else
            return parsing_failed("isActive");

        if (value.HasMember("trunk_id"))
            mntrctx->m_trunkInfo.m_iTrunk_id = value["trunk_id"].GetInt();
        else
            return parsing_failed("trunk_id");

        XGLOG_INFO("Trunk info data domain_whitelist(%s),DcType(%s),IsActive(%d),TrunkId(%d)",
                   mntrctx->m_trunkInfo.m_sIpAddress.c_str(),
                   mntrctx->m_trunkInfo.m_sDcType.c_str() ,
                   mntrctx->m_trunkInfo.isActive,
                   mntrctx->m_trunkInfo.m_iTrunk_id);

        if (strcasecmp(mntrctx->m_trunkInfo.m_sDcType.c_str(), dcType.c_str()) != 0)
        {
            XGLOG_TRACE("Trunk info not matched with DC Type domain_whitelist(%s) TrunkId(%d)", mntrctx->m_trunkInfo.m_sIpAddress.c_str(),
                        mntrctx->m_trunkInfo.m_iTrunk_id);
            delete_obj(mntrctx);
            continue;
        }

        if( true == pCtxHndlr->insertMonitorCtx(mntrctx->m_trunkInfo.m_sIpAddress,mntrctx->m_trunkInfo.m_iTrunk_id,mntrctx))
        {
            CONFIGSERVICE->m_lightWeightWSSProfile.insertTrunkInfo(mntrctx->m_trunkInfo.m_sIpAddress,mntrctx->m_trunkInfo.m_iTrunk_id);
            sendOptionsRequestToSipSrv(*mntrctx);
        }        
    }

    return true;
}

/*
 * Handle Options success from Sip service
 */
bool xGateMonitorHandler::handleOptionsSuccessMsg(xGateSipMsg *sipmsg)
{
    if (!sipmsg)
    {
        XGLOG_ERROR("xGateMonitorHandler::handleOptionsSuccessMsg sipmsg is nullptr !");
        return false;
    }

    std::string ipAddress = sipmsg->getTrunkIPAddress();
    int trunkId = sipmsg->getTrunkId();
    int rescode = sipmsg->getResponseCode();
    XGLOG_INFO("xGateMonitorHandler::handleOptionsSuccessMsg for callid(%s),TrunkId(%d) IpAddress(%s) and ResponseCode(%d)", sipmsg->getUid().c_str(),trunkId,ipAddress.c_str(),rescode);
    MonitorContext *mntrctx = nullptr;
    if (true == (pCtxHndlr->getMonitorCtx(ipAddress, trunkId, mntrctx)))
    {
        mntrctx->m_trunkStatus = xGateTrunkStatus::EN_TRUNK_STATUS_OPTIONS_SUCCESS;
        startTimerRequestToSchedulerSrv(*mntrctx);
        updateTrunkStatusToDB(*mntrctx,1,rescode,sipmsg->getUid());
        updateTrunkStatusToNetIO(*mntrctx);
        return true;
    }

    return false;
}

/*
 * Handle Options failed from Sip service
 */
bool xGateMonitorHandler::handleOptionsFailedMsg(xGateSipMsg *sipmsg)
{
    if (!sipmsg)
    {
        XGLOG_ERROR("xGateMonitorHandler::handleOptionsFailedMsg sipmsg is nullptr !");
        return false;
    }

    std::string ipAddress = sipmsg->getTrunkIPAddress();
    int trunkId = sipmsg->getTrunkId();
    int rescode = sipmsg->getResponseCode();
    MonitorContext *mntrctx = nullptr;
    XGLOG_INFO("xGateMonitorHandler::handleOptionsFailedMsg for callid(%s) TrunkId(%d) IpAddress(%s) and ResponseCode(%d)", sipmsg->getUid().c_str(),trunkId,ipAddress.c_str(),rescode);
    if (true == (pCtxHndlr->getMonitorCtx(ipAddress, trunkId, mntrctx)))
    {
        mntrctx->m_trunkStatus = xGateTrunkStatus::EN_TURNK_STATUS_OPTIONS_FAILED;
        startTimerRequestToSchedulerSrv(*mntrctx);
        updateTrunkStatusToDB(*mntrctx,0,rescode,sipmsg->getUid());
        updateTrunkStatusToNetIO(*mntrctx);
        return true;
    }

    return false;
}

/*
 * Handle Options timeout from Sip service
 */
bool xGateMonitorHandler::handleOptionsTimeoutMsg(xGateSipMsg *sipmsg)
{
    if (!sipmsg)
    {
        XGLOG_ERROR("xGateMonitorHandler::handleOptionsTimeoutMsg sipmsg is nullptr !");
        return false;
    }

    std::string ipAddress = sipmsg->getTrunkIPAddress();
    int trunkId = sipmsg->getTrunkId();
    int rescode = sipmsg->getResponseCode();
    MonitorContext *mntrctx = nullptr;
    XGLOG_INFO("xGateMonitorHandler::handleOptionsTimeoutMsg for callid(%s),TrunkId(%d) IpAddress(%s) and ResponseCode(%d)", sipmsg->getUid().c_str(),trunkId ,ipAddress.c_str(),rescode);
    if (true == (pCtxHndlr->getMonitorCtx(ipAddress, trunkId, mntrctx)))
    {
        mntrctx->m_trunkStatus = xGateTrunkStatus::EN_TURNK_STATUS_OPTIONS_FAILED;
        startTimerRequestToSchedulerSrv(*mntrctx);
        updateTrunkStatusToDB(*mntrctx,0,rescode,sipmsg->getUid());
        updateTrunkStatusToNetIO(*mntrctx);
        return true;
    }

    return false;
}

/*
 * Handle Scheduler Timeout Message
 */
bool xGateMonitorHandler::handleSchedulerTimeoutMsg(xGateTimerData *timerdata)
{
    XGLOG_FUNCTION_EN_EX("");

    MonitorContext *ptctx = (MonitorContext *)timerdata->m_pCtxdata;

    if (!ptctx)
    {
        XGLOG_ERROR("xGateMonitorHandler::handleSchedulerTimeoutMsg timer data is nullptr");
        return false;
    }
    XGLOG_INFO("xGateMonitorHandler::handleSchedulerTimeoutMsg triggered for callid(%s) TrunkId(%d) IpAddress(%s)",
             timerdata->m_uid.c_str(),ptctx->m_trunkInfo.m_iTrunk_id,ptctx->m_trunkInfo.m_sIpAddress.c_str());

    MonitorContext *mntrctx = nullptr;
    if (true == (pCtxHndlr->getMonitorCtx(ptctx->m_trunkInfo.m_sIpAddress, ptctx->m_trunkInfo.m_iTrunk_id, mntrctx)))
    {
        sendOptionsRequestToSipSrv(*mntrctx);
        delete_obj(ptctx);
        return true;
    }

    delete_obj(ptctx);
    return false;
}

/*
 * Add new trunk into context
 */
bool xGateMonitorHandler::addNewTrunk(xGateMonitorInfo mntrInfo)
{
    XGLOG_FUNCTION_EN_EX("");
    int trunk_id = mntrInfo.m_iTrunkId;
    std::string sDCType = mntrInfo.m_sDcType.c_str();

    for (auto itr : mntrInfo.m_IpAddressList)
    {
        MonitorContext *mntrctx = new MonitorContext();
        mntrctx->m_trunkInfo.m_iTrunk_id = trunk_id;
        mntrctx->m_trunkInfo.m_sDcType = sDCType.c_str();
        mntrctx->m_trunkInfo.m_sIpAddress = itr;
        // Todo Need to insert based on dc type from configuration
        XGLOG_INFO("Trunk info data domain_whitelist(%s),TrunkId(%d),DcType(%s),IsActive(%d)",
                   mntrctx->m_trunkInfo.m_sIpAddress.c_str(),
                   mntrctx->m_trunkInfo.m_iTrunk_id,
                   mntrctx->m_trunkInfo.m_sDcType.c_str(),
                   mntrctx->m_trunkInfo.isActive);
        if (true == pCtxHndlr->insertMonitorCtx(mntrctx->m_trunkInfo.m_sIpAddress, mntrctx->m_trunkInfo.m_iTrunk_id, mntrctx))
        {
            CONFIGSERVICE->m_lightWeightWSSProfile.insertTrunkInfo(mntrctx->m_trunkInfo.m_sIpAddress, mntrctx->m_trunkInfo.m_iTrunk_id);
            sendOptionsRequestToSipSrv(*mntrctx);
        }
    }

    return true;
}

/*
 * Remove trunk from context
 */
bool xGateMonitorHandler::removeTrunk(xGateMonitorInfo mntrInfo)
{
    XGLOG_FUNCTION_EN_EX("");
    int trunk_id = mntrInfo.m_iTrunkId;

    for (auto itr : mntrInfo.m_IpAddressList)
    {
        pCtxHndlr->deleteMonitorCtx(itr, trunk_id);
        CONFIGSERVICE->m_lightWeightWSSProfile.removeTrunkInfo(itr, trunk_id);
    }
    return true;
}

/*
 * Fetch teams domain details from DB service
 */
bool xGateMonitorHandler::FetchMSTeamsDomainListFromDBSrv()
{
    XGLOG_FUNCTION_EN_EX("");

    xGateDBServiceMsg *ptDBMsg = new xGateDBServiceMsg();

    if(!ptDBMsg)
    {
        XGLOG_ERROR("xGateMonitorHandler::FetchMSTeamsDomainListFromDBSrv OOM issue occured !");
        return false;
    }

    DBRequestInfo dbReqInfo;
    dbReqInfo.m_spName = "ur_myacc_get_teams_whitelisted_sbc_fqdn";
    dbReqInfo.m_dbReqMethod = DBReqMethod::EN_DB_METHOD_READ;
    dbReqInfo.m_dbEvent = DBServiceEvent::EN_DB_EVENT_GET_MSTEAMS_DOMAIN_LIST;
    dbReqInfo.m_contextId = ""; // Todo need to add
    dbReqInfo.m_requestorModule = IURDefines::UR_MODULE_MONITOR_SERVICE;
    ptDBMsg->set_db_request_info(dbReqInfo);
    ptDBMsg->setSrcModuleId(IURDefines::UR_MODULE_MONITOR_SERVICE);
    ptDBMsg->setDstModuleId(IURDefines::UR_MODULE_DB_SERVICE);

    MNTRSERVICE->getConfig()->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(ptDBMsg));
    XGLOG_INFO("xGateMonitorHandler::FetchMSTeamsDomainListFromDBSrv posted to DB Module");
    return true;
}

/*
* Handling Ms-teams domain list response from DB
*/
bool xGateMonitorHandler::handleMsTeamsDomainListMsg(DBRequestInfo &reqinfo)
{
    XGLOG_FUNCTION_EN_EX("");
    std::string data = reqinfo.m_data.c_str();
    if (data.length() <= 0)
    {
        XGLOG_ERROR("xGateMonitorHandler::handleMsTeamsDomainListMsg data is empty!");
        return false;
    }

    Document doc;
    if (!parseJsonMsg(data.c_str(), doc))
    {
        XGLOG_ERROR("handleMsTeamsDomainListMsg failed to parse DB message !");
        return false;
    }

    // Lamda Expression to print the error
    auto parsing_failed = [&](std::string error) -> bool
    {
        XGLOG_ERROR("xGateMonitorHandler::handleMsTeamsDomainListMsg parsing failed '%s' is not available from json !", error.c_str());
        return false;
    };

    // Iterate over the array elements
    for (SizeType i = 0; i < doc.Size(); ++i)
    {
        if (doc[i].IsString())
        {
            CONFIGSERVICE->m_lightWeightWSSProfile.addIPToWhiteList(doc[i].GetString());
            CONFIGSERVICE->m_lightWeightWSSProfile.addSbcFqdnToTeamsDomainList(doc[i].GetString());
        }
    }
    return true;
}

/*
 * Parse json message as doc
 */
bool xGateMonitorHandler::parseJsonMsg(const char *jsonData, Document &doc)
{
    XGLOG_INFO("parsing business json data: %s", jsonData);
    ParseResult ok = doc.Parse(jsonData);

    if (!ok)
    {
        XGLOG_ERROR("parseJsonMsg failed. Encountered with json parse error !");
        return false;
    }
    else if (!doc.IsArray())
    {
        XGLOG_ERROR("parseJsonMsg failed. Not able create documnet array !");
        return false;
    }
    return true;
}

/*
* Generalized functions to delete object
*/
template <typename T>
inline void xGateMonitorHandler::delete_obj(T *obj)
{
    if (obj)
    {
        delete obj;
        obj = nullptr;
    }
}