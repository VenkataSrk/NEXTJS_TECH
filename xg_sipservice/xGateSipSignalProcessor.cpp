#include "xGateSipSignalProcessor.h"
#include "xGateSipController.h"
#include "xGateSipMsg.h"
#include "xGateSipService.h"
#include "xGateSipSignalReceiver.h"
#include <uuid/uuid.h>

xGateSipSignalProcessor::xGateSipSignalProcessor() : 
  xGateProcessor("SipProcessor")
{
  m_run = true;
  m_strSipModuleName = "SIP Module";
  m_ptModuleCofig =  nullptr;
}

/*
*/
xGateSipSignalProcessor::~xGateSipSignalProcessor()
{
}

int xGateSipSignalProcessor::initModule(IURModuleConfig * ptConfig){
	if(!ptConfig)
		return IURDefines::UR_RETURN_ERROR;
	m_ptModuleCofig = ptConfig;
	
	this->init(1);

#if 1
        //DebugVM start
        //if(SIP_SIGNAL_RECEIVER::instance()->init() == nullptr){
        if(SIP_SIGNAL_RECEIVER::instance()->init("SipReceiver") != 0){
           return IURDefines::UR_RETURN_ERROR;
        }

        //if(SIP_SIGNAL_DISPATCHER::instance()->init() !=  nullptr){
        //if(SIP_SIGNAL_DISPATCHER::instance()->init() !=  EN_XGATE_STATUS_SUCCESS){
        //  return IURDefines::UR_RETURN_ERROR;
        //}
        //DebugVM end
#endif

	return IURDefines::UR_RETURN_SUCCESS;
}
int xGateSipSignalProcessor::unitModule() {
	return IURDefines::UR_RETURN_SUCCESS;
}
int xGateSipSignalProcessor::pushModuleMsg(IURModuleMsg *ptMsg)
{
  XGLOG_INFO("xGateSipSignalProcessor::pushModuleMsg");
  if(!ptMsg)
  {
    XGLOG_ERROR("pushModuleMsg::Message not received from controller.!");
    return IURDefines::UR_RETURN_ERROR;
  }
  else // push the msg in  Queue
  {
    XGLOG_DEBUG("xGateSipSignalProcessor::pushModuleMsg Message received from controller to SipService");
    ACE_Message_Block* pAmb = nullptr;
    pAmb = dynamic_cast<ACE_Message_Block*>(ptMsg);
    ACE_Time_Value tval ((time(nullptr) ));
    tval += ACE_Time_Value(0,1);
    SIP_SIGNAL_MODULE::instance()->putq(pAmb,&tval);
    //xGateSipService *sipService = SIPSERVICE;
    //sipService->putq(dynamic_cast<ACE_Message_Block*>(ptMsg));
  }
   
  return IURDefines::UR_RETURN_SUCCESS;
}
void xGateSipSignalProcessor::getModuleName(std::string& moduleName) {
	moduleName = m_strSipModuleName;
}
IURDefines::MODULE_ID xGateSipSignalProcessor::getModuleID(){
	return IURDefines::UR_MODULE_SIPSIGNAL_SERVICE;
}

IURModuleConfig * xGateSipSignalProcessor::getModuleConfig()
{
  return m_ptModuleCofig;
}
/*
*/
int xGateSipSignalProcessor::svc( void )
{
  // Message block in which the message is read
  ACE_Message_Block *pAmb = 0;
  // Till the flag run_ is true this loop will continue and the thread will
  // exist
  while(m_run) {
    // Read the message from the queue. If no message is present then this
    // method will block. This methods returns when the message is read or
    // the queue is deactivated
    ACE_Time_Value tv ((time(nullptr) + 20));

    // Get message from the queue
    int result = getq(pAmb, &tv);
    if((result == -1) && (errno == EWOULDBLOCK)) {
      continue;
    }

    // check the result
    if((result != -1)) {
      ///Process and Dispatch Message
      process_msg(pAmb);
    } else {
      // This is the case where the message queue is de-activated.
      // Need to stop the thread
      //DebugVM CLog::Error(THISMODULE, "xGateSipSignalProcessor::svc(%s) Error in Message Queue", "SPQ");
      m_run = false;
    }

    // delete the message received
    if(pAmb) {
      delete pAmb;
      pAmb = nullptr;
    }
  }
  return 0;
}

xGateRetVal xGateSipSignalProcessor::process_msg(ACE_Message_Block *pAmb)
{
	XGLOG_INFO("xGateSipSignalProcessor::process_msg Start Function");
	xGateRetVal retval = EN_XGATE_STATUS_SUCCESS;
	if (pAmb)
	{
		XGLOG_INFO("xGateSipSignalProcessor::process_msg pAmb exits");
		// debugAC CLog::Info(THISMODULE,"multithreaded-ippbx : xGateSipSignalProcessor::process_msg(%s)", "Received Msg");
		xGateBaseMsg *pBaseMsg = nullptr;

		pBaseMsg = dynamic_cast<xGateBaseMsg *>(pAmb);
		XGLOG_INFO("xGateSipSignalProcessor::process_msg msgType=%d", pBaseMsg->getMsgType());
		switch (pBaseMsg->getMsgType())
		{
		case EN_XGATE_MSG_SIP_IN: // inbound  call indication
		{
			// XGLOG_INFO("xGateSipSignalProcessor::process_msg processing EN_XGATE_MSG_SIP_IN");
			xGateSipMsgBlock *pSipMsg = dynamic_cast<xGateSipMsgBlock *>(pAmb);
			if (pSipMsg)
			{
				// SendToMainProcessor(pSipMsg->m_callObj);
				// DebugVM processBusinessRequest(pSipMsg->m_callObj);
				handleSipMessage(pSipMsg->m_callObj);
			}
			break;
		}
		case EN_XGATE_MSG_SIP_MODULE: // messaage from outside of sip module
		{
			xGateSipMsg *pSipMsg = dynamic_cast<xGateSipMsg *>(pAmb);
			if (nullptr != pSipMsg)
			{
				handleIncomingSIPmsg(pSipMsg);
			}
			else
			{
				XGLOG_ERROR("xGateSipSignalProcessor::process_msg Error callobj nullptr");
			}
			break;
		}
		case EN_XGATE_MSG_TIMER: // message from Scheduler service
		{
			// xGateSipMsg *pSipMsg = dynamic_cast<xGateSipMsg*>(pAmb);
			if (nullptr != pBaseMsg)
			{
				handleIncomingSchedulerMsg(pBaseMsg);
			}
			else
			{
				XGLOG_ERROR("xGateSipSignalProcessor::process_msg Error callobj nullptr");
			}
			break;
		}
		case EN_XGATE_MSG_UNKNOWN:
     break;
    case EN_XGATE_MSG_MONITOR_MODULE :
    {
      xGateSipMsg *pSipMsg = dynamic_cast<xGateSipMsg *>(pAmb);
      if (nullptr != pSipMsg)
			{
				handleMoniterMessage(pSipMsg);
			}
			else
			{
				XGLOG_ERROR("xGateSipSignalProcessor::process_msg Error callobj nullptr");
			}
			break;
    }
		default:
		{
			// CLog::Error(THISMODULE, "multithreaded-ippbx : xGateSipSignalProcessor::process_msg failed due to EN_XGATE_MSG_UNKNOWN");
			break;
		}
		}
	}
	return retval;
}

bool xGateSipSignalProcessor::handleSipInviteRequest(xGateCallObj *callObj)
{
  xGateSipMsg *sMsg = nullptr;
  if(callObj->sipeventtype == EN_INVITE_REQ)
  {
    sMsg = new xGateSipMsg(IURDefines::UR_MODULE_SIPSIGNAL_SERVICE, IURDefines::UR_MODULE_SERVICE_CONTROLLER, xGateSipMsg::SIP_EVENT_TYPE::SIP_EVENT_INVITE_REQUEST, xGateSipMsg::SIP_MSG_TYPE::SIP_MSG_REQUEST);
    XGLOG_INFO("xGateSipSignalProcessor::handleSipInviteRequest EN_INVITE_REQ callid(%s)",callObj->uid.c_str());
  }
  else if(callObj->sipeventtype == EN_INVITE_INBOUND_PSTN_REQ)
  {
    sMsg = new xGateSipMsg(IURDefines::UR_MODULE_SIPSIGNAL_SERVICE, IURDefines::UR_MODULE_SERVICE_CONTROLLER, xGateSipMsg::SIP_EVENT_TYPE::SIP_EVENT_INVITE_INBOUND_PSTN_REQUEST, xGateSipMsg::SIP_MSG_TYPE::SIP_MSG_REQUEST);
    XGLOG_INFO("xGateSipSignalProcessor::handleSipInviteRequest EN_INVITE_REQ_INBOUND_PSTN callid(%s)",callObj->uid.c_str());
  }
  else
  {
    sMsg = new xGateSipMsg(IURDefines::UR_MODULE_SIPSIGNAL_SERVICE, IURDefines::UR_MODULE_SERVICE_CONTROLLER, xGateSipMsg::SIP_EVENT_TYPE::SIP_EVENT_INVITE_MS_TEAMS_REQUEST, xGateSipMsg::SIP_MSG_TYPE::SIP_MSG_REQUEST);
    XGLOG_INFO("xGateSipSignalProcessor::handleSipInviteRequest SIP_EVENT_INVITE_MS_TEAMS_REQUEST callid(%s)",callObj->uid.c_str());
  }
  sMsg->setUid(callObj->uid);
  sMsg->setDstModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
  sMsg->setMsgType(EN_XGATE_MSG_SIP_MODULE);
  sMsg->fillInviteRequestDetails(sMsg, callObj);

  // Making on entry is map based on uid
  auto itr = m_callcontext.find(callObj->uid.c_str());
  if (itr != m_callcontext.end())
  {
    XGLOG_INFO("xGateSipSignalProcessor::handleSipInviteRequest Editing value in m_callcontext list");
    itr->second = callObj;
  }
  else
  {
    XGLOG_INFO("xGateSipSignalProcessor::handleSipInviteRequest Inserting value in m_callcontext list");
    // m_callcontext.insert(std::pair<std::string, xGateCallObj*>(callObj->uid, callObj));
    std::string tuid = callObj->uid.c_str();
    m_callcontext.insert(std::make_pair(tuid, callObj));
  }

  if (!sMsg)
  {
    XGLOG_ERROR("xGateSipSignalProcessor::handleSipInviteRequest ");
    return false;
  }

  IURModuleConfig *pModCofig = nullptr;
  pModCofig = getConfig();
  if (!pModCofig)
  {
    XGLOG_ERROR("xGateSipSignalProcessor::handleSipMessage Pointer to Module Config is nullptr");
    return false;
  }
  XGLOG_INFO("xGateSipSignalProcessor::handleSipMessage Sending New Call Notify to Controller");
  pModCofig->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(sMsg));
  return true;
}

bool xGateSipSignalProcessor::handleSipRegistrationProcess(xGateSofiaSipRegobj *sipregobj)
{
	if (!sipregobj)
	{
		XGLOG_ERROR("xGateSipSignalProcessor::handleSipRegistrationProcess failed!.Registration object is empty");
		return false;
	}

	XGLOG_INFO("xGateSipSignalProcessor::handleSipRegistrationProcess for callId(%s)", sipregobj->callid.c_str());
	switch (sipregobj->Regevent)
	{
	case EN_REGISTER:
	{
		XGLOG_INFO("xGateSipSignalProcessor::handleSipRegistrationProcess regevent EN_REGISTER");
		SendUserRegistrationRequestToController(sipregobj);
	}
	break;
	case EN_DEREGISTER:
	{
		XGLOG_INFO("xGateSipSignalProcessor::handleSipRegistrationProcess regevent EN_DEREGISTER");
		ProcessDeRegistrationRequest(sipregobj);
	}
	break;
	case EN_REG_UPDATE_AUTH_SUCCESS:
	case EN_REG_UPDATE_AUTH_FAILED:
	{
		XGLOG_INFO("xGateSipSignalProcessor::handleSipRegistrationProcess regevent %d", sipregobj->Regevent);
		UpdateRegisterationStatusToController(sipregobj);
	}
	break;
	case EN_UNKNOWN:
	default:
	{
		XGLOG_INFO("xGateSipSignalProcessor::handleSipRegistrationProcess regevent EN_UNKNOWN");
	}
	break;
	}
	return true;
}

bool xGateSipSignalProcessor::SendUserRegistrationRequestToController(xGateSofiaSipRegobj *sipregobj)
{
	if(!sipregobj)
	{
		XGLOG_ERROR("xGateSipSignalProcessor::SendUserRegistrationRequestToController failed!.Registration object is empty");
		return false;
	}
        XGLOG_INFO("xGateSipSignalProcessor::SendUserRegistrationRequestToController for callid(%s)",sipregobj->callid.c_str());

        xGateSipMsg *sMsg = nullptr;
        sMsg = new xGateSipMsg(IURDefines::UR_MODULE_SIPSIGNAL_SERVICE,IURDefines::UR_MODULE_SERVICE_CONTROLLER, xGateSipMsg::SIP_EVENT_TYPE::SIP_EVENT_DO_USER_REG_REQ,xGateSipMsg::SIP_MSG_TYPE::SIP_MSG_REQUEST);

        sipregobj->transid = generateTransid();

        sMsg->setUid(sipregobj->transid.c_str());
        sMsg->fillRegistrationInfoDetails(sMsg,sipregobj);
        sMsg->setDstModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
        sMsg->setMsgType(EN_XGATE_MSG_SIP_MODULE);
		    sMsg->getSipEventType();

        //Making on Registration entry in map based on Reg Callid
        auto itr = m_regcontext.find(sipregobj->transid.c_str());
        if(itr != m_regcontext.end())
        {
                XGLOG_INFO("xGateSipSignalProcessor::SendUserRegistrationRequestToController value entry exists in m_regcontext list for callid (%s) and trans_id (%s)",sipregobj->callid.c_str(),sipregobj->transid.c_str());
                if(itr->second != nullptr)
                  delete itr->second;
                itr->second = sipregobj;
        }
        else
        {
                XGLOG_INFO("xGateSipSignalProcessor::SendUserRegistrationRequestToController value Inserting entry in m_regcontext list for callid(%s) and trans_id:(%s)",sipregobj->callid.c_str(), sipregobj->transid.c_str());
                m_regcontext.insert(std::make_pair(sipregobj->transid,sipregobj));
        }

        IURModuleConfig * pModCofig = nullptr;
        pModCofig = getConfig();
        if(!pModCofig)
        {
                XGLOG_ERROR("xGateSipSignalProcessor::SendUserRegistrationRequestToController Pointer to Module Config is nullptr");
                return false;
        }
        XGLOG_INFO("xGateSipSignalProcessor::SendUserRegistrationRequestToController Sending user Registration Request to Controller");

        pModCofig->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(sMsg));

        return true;
}

bool xGateSipSignalProcessor::ProcessDeRegistrationRequest(xGateSofiaSipRegobj *pSipregobj)
{
	if(!pSipregobj)
	{
		XGLOG_ERROR("xGateSipSignalProcessor::ProcessDeRegistrationRequest failed!.Registration object is empty");
		return false;
	}
	XGLOG_INFO("xGateSipSignalProcessor::ProcessDeRegistrationRequest callId=%s", pSipregobj->callid.c_str());

        xGateSipMsg *sMsg = nullptr;
        sMsg = new xGateSipMsg(IURDefines::UR_MODULE_SIPSIGNAL_SERVICE,IURDefines::UR_MODULE_SERVICE_CONTROLLER, xGateSipMsg::SIP_EVENT_TYPE::SIP_EVENT_USER_DEREG_REQ,xGateSipMsg::SIP_MSG_TYPE::SIP_MSG_REQUEST);

        pSipregobj->transid = generateTransid();

		    sMsg->setUid(pSipregobj->transid.c_str());
        sMsg->fillRegistrationInfoDetails(sMsg,pSipregobj);
        sMsg->setDstModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
        sMsg->setMsgType(EN_XGATE_MSG_SIP_MODULE);

        IURModuleConfig * pModCofig = nullptr;
        pModCofig = getConfig();
        if(!pModCofig)
        {
                XGLOG_ERROR("xGateSipSignalProcessor::ProcessDeRegistrationRequest Pointer to Module Config is nullptr");
                return false;
        }
        XGLOG_INFO("xGateSipSignalProcessor::ProcessDeRegistrationRequest Sending user Registration Request to Controller");
        pModCofig->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(sMsg));
		    m_regcontext.erase(pSipregobj->transid.c_str());
	return true;
}

bool xGateSipSignalProcessor::UpdateRegisterationStatusToController(xGateSofiaSipRegobj *sipregobj)
{
  if (!sipregobj)
  {
    XGLOG_ERROR("xGateSipSignalProcessor::SendUserRegistrationUpdateToController sipregobj is nullptr");
    return false;
  }

  xGateSipMsg *sMsg = nullptr;
  if (sipregobj->Regevent == EN_REG_UPDATE_AUTH_SUCCESS)
  {
    sMsg = new xGateSipMsg(IURDefines::UR_MODULE_SIPSIGNAL_SERVICE, IURDefines::UR_MODULE_SERVICE_CONTROLLER,
                           xGateSipMsg::SIP_EVENT_TYPE::SIP_EVENT_USER_REG_UPDATE_AUTH_SUCCESS, xGateSipMsg::SIP_MSG_TYPE::SIP_MSG_REQUEST);
  }
  else
  {
    sMsg = new xGateSipMsg(IURDefines::UR_MODULE_SIPSIGNAL_SERVICE, IURDefines::UR_MODULE_SERVICE_CONTROLLER,
                           xGateSipMsg::SIP_EVENT_TYPE::SIP_EVENT_USER_REG_UPDATE_AUTH_FAILED, xGateSipMsg::SIP_MSG_TYPE::SIP_MSG_REQUEST);
  }

  sMsg->setUid(sipregobj->transid.c_str());
  sMsg->fillRegistrationInfoDetails(sMsg, sipregobj);
  sMsg->setDstModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
  sMsg->setMsgType(EN_XGATE_MSG_SIP_MODULE);

  IURModuleConfig *pModCofig = nullptr;
  pModCofig = getConfig();

  if (!pModCofig)
  {
    XGLOG_ERROR("xGateSipSignalProcessor::UpdateRegisterationStatusToController Pointer to Module Config is nullptr");
    return false;
  }
  m_regcontext.erase(sipregobj->transid.c_str());//after sending update status removing from map
  XGLOG_INFO("xGateSipSignalProcessor::SendAuthRequestToController Sending Authentication Request to Controller");
  delete sipregobj;
  sipregobj = nullptr;
  pModCofig->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(sMsg));
  return true;
}

void xGateSipSignalProcessor::handleMoniterMessage(xGateSipMsg *pSipMsg)
{
  if (!pSipMsg)
  {
    XGLOG_ERROR("xGateSipSignalProcessor::handleMoniterMessage failed!.pSipMsg object is nullptr");
    return;
  }
  XGLOG_INFO("xGateSipSignalProcessor:handleMoniterMessage Function start");
  switch (pSipMsg->getSipEventType())
  {
  case xGateSipMsg::SIP_EVENT_TYPE::SIP_EVENT_OPTIONS_REQUEST:
  {
    XGLOG_INFO("xGateSipSignalProcessor:handleMoniterMessage event type is SIP_EVENT_OPTIONS_REQUEST");
    xGateCallObj *callObj = new xGateCallObj();
    callObj->callid = pSipMsg->getUid();
    callObj->m_iTrunkId = pSipMsg->getTrunkId();
    callObj->m_sRouteIP = pSipMsg->getTrunkIPAddress();
    callObj->m_sCallingNumber = pSipMsg->getTrunkUName();
    callObj->sipeventtype = EN_OPTIONS_REQUEST;
    WriteToReceiver(callObj);
    break;
  }
  default:
  {
    XGLOG_ERROR("xGateSipSignalProcessor:handleMoniterMessage unknown event");
  }
    break;
  }
}

void xGateSipSignalProcessor::handleIncomingSIPmsg(xGateSipMsg *pSipMsg)
{
  if (!pSipMsg)
  {
	XGLOG_ERROR("xGateSipSignalProcessor::handleIncomingSIPmsg failed!.Sip Message object is empty");
	return;
  }

  XGLOG_INFO("xGateSipSignalProcessor:handleIncomingSIPmsg Function start");

  XGLOG_INFO("xGateSipSignalProcessor:handleIncomingSIPmsg event type is %d uid=%s", pSipMsg->getSipEventType(), pSipMsg->getUid().c_str());
  switch (pSipMsg->getSipEventType())
  {

  case xGateSipMsg::SIP_EVENT_TYPE::SIP_EVENT_DO_USER_REG_RES:
  {
	XGLOG_INFO("xGateSipSignalProcessor:handleIncomingSIPmsg event type is SIP_EVENT_DO_USER_REG_RES");
	std::string aorDetails = pSipMsg->getAorDetails();
	std::string password = pSipMsg->getUserPassword();
  std::string transid  = pSipMsg->getUid();

  auto itr= m_regcontext.find(transid.c_str());

  if(itr != m_regcontext.end())
  {
    itr->second->Addressofrecords = aorDetails.c_str();
    itr->second->password = password.c_str();
    ProcessDoUserRegistrationResponse(itr->second);
  }
  else
  {
    XGLOG_INFO("xGateSipSignalProcessor:handleIncomingSIPmsg no entry found in m_regcontext for (%s)",transid.c_str());
  }

	break;
  }
  case xGateSipMsg::SIP_EVENT_TYPE::SIP_EVENT_USER_REG_RESPONSE_ERROR:
  {
	XGLOG_INFO("xGateSipSignalProcessor:handleIncomingSIPmsg event type is SIP_EVENT_USER_REG_RESPONSE_ERROR");
  std::string transid  = pSipMsg->getUid();

  auto itr = m_regcontext.find(transid.c_str());

    if (itr != m_regcontext.end())
    {
      ProcessRegistrationResponseError(itr->second);
    }
    else
    {
    XGLOG_INFO("xGateSipSignalProcessor:handleIncomingSIPmsg SIP_EVENT_USER_REG_RESPONSE_ERROR no entry found in m_regcontext for (%s)",transid.c_str());
  }

	break;
  }
  case xGateSipMsg::SIP_EVENT_TYPE::SIP_EVENT_INVITE_RESPONSE:
  {
  XGLOG_INFO("xGateSipSignalProcessor:handleIncomingSIPmsg event type is SIP_EVENT_INVITE_RESPONSE");
	std::string tuid = pSipMsg->getUid().c_str();
	std::string password = pSipMsg->getUserPassword();
	std::string route_ip = pSipMsg->getRouteIP();
  std::string call_info = "";
  if(!pSipMsg->getCallInfo().empty())
    call_info = pSipMsg->getCallInfo();
	auto itr = m_callcontext.find(tuid);
	if (itr != m_callcontext.end())
	{
			XGLOG_INFO("xGateSipSignalProcessor::handleIncomingSIPmsg uid matches");
			xGateCallObj *callObj = itr->second;
			callObj->m_sPassword = password;
			callObj->m_sRouteIP = route_ip;
      if(!call_info.empty())
        callObj->m_sCallInfo = call_info;
			callObj->sipeventtype = EN_INVITE_RES;
			WriteToReceiver(callObj);
	}
	else
	{
			XGLOG_ERROR("xGateSipSignalProcessor::handleIncomingSIPmsg uid match NOT FOUND");
	}
	break;
  }
  case xGateSipMsg::SIP_EVENT_TYPE::SIP_EVENT_INVITE_RESPONSE_ERROR:
  {
  XGLOG_INFO("xGateSipSignalProcessor:handleIncomingSIPmsg event type is SIP_EVENT_INVITE_RESPONSE_ERROR");
	std::string tuid = pSipMsg->getUid().c_str();
	auto itr = m_callcontext.find(tuid);
	if (itr != m_callcontext.end())
	{
			XGLOG_INFO("xGateSipSignalProcessor::handleIncomingSIPmsg uid matches");
			xGateCallObj *callObj = itr->second;
			callObj->sipeventtype = EN_INVITE_RES_ERROR;
			WriteToReceiver(callObj);
	}
	else
	{
			XGLOG_ERROR("xGateSipSignalProcessor::handleIncomingSIPmsg uid match NOT FOUND");
	}
	break;
  }
  case xGateSipMsg::SIP_EVENT_TYPE::SIP_EVENT_INVITE_INBOUND_PSTN_RESPONSE:
  {
  XGLOG_INFO("xGateSipSignalProcessor:handleIncomingSIPmsg event type is SIP_EVENT_INVITE_INBOUND_PSTN_RESPONSE");
  std::string tuid = pSipMsg->getUid().c_str();
  std::string route_ip = pSipMsg->getRouteIP();
  auto itr = m_callcontext.find(tuid);
  if (itr != m_callcontext.end())
  {
      XGLOG_INFO("xGateSipSignalProcessor::handleIncomingSIPmsg uid matches");
      xGateCallObj *callObj = itr->second;
      callObj->m_sRouteIP = route_ip;
      callObj->sipeventtype = EN_INVITE_INBOUND_PSTN_RES;
      WriteToReceiver(callObj);
  }
  else
  {
      XGLOG_ERROR("xGateSipSignalProcessor::handleIncomingSIPmsg uid match NOT FOUND");
  }
  break;
  }
  case xGateSipMsg::SIP_EVENT_TYPE::SIP_EVENT_INVITE_INBOUND_PSTN_ERROR:
  {
    XGLOG_INFO("xGateSipSignalProcessor:handleIncomingSIPmsg event type is SIP_EVENT_INVITE_INBOUND_PSTN_ERROR");
    std::string tuid = pSipMsg->getUid().c_str();
    auto itr = m_callcontext.find(tuid);
    if (itr != m_callcontext.end())
    {
      XGLOG_INFO("xGateSipSignalProcessor::handleIncomingSIPmsg uid matches");
      xGateCallObj *callObj = itr->second;
      callObj->sipeventtype = EN_INVITE_INBOUND_PSTN_ERROR;
      WriteToReceiver(callObj);
    }
    else
    {
      XGLOG_ERROR("xGateSipSignalProcessor::handleIncomingSIPmsg uid match NOT FOUND");
    }
    break;
  }
  case xGateSipMsg::SIP_EVENT_TYPE::SIP_EVENT_INVITE_MS_TEAMS_RESPONSE:
  {
    XGLOG_INFO("xGateSipSignalProcessor:handleIncomingSIPmsg event type is SIP_EVENT_INVITE_MS_TEAMS_RESPONSE");
    std::string tuid = pSipMsg->getUid().c_str();
    std::string route_ip = pSipMsg->getRouteIP();
    auto itr = m_callcontext.find(tuid);
    if (itr != m_callcontext.end())
    {
      XGLOG_INFO("xGateSipSignalProcessor::handleIncomingSIPmsg uid matches");
      xGateCallObj *callObj = itr->second;
      callObj->m_sRouteIP = route_ip;
      callObj->sipeventtype = EN_INVITE_MS_TEAMS_RES;
      WriteToReceiver(callObj);
    }
    else
    {
      XGLOG_ERROR("xGateSipSignalProcessor::handleIncomingSIPmsg uid match NOT FOUND");
    }
    break;
  }
  case xGateSipMsg::SIP_EVENT_TYPE::SIP_EVENT_OPTIONS_REQUEST:
  {
    XGLOG_INFO("xGateSipSignalProcessor:handleIncomingSIPmsg event type is SIP_EVENT_OPTIONS_REQUEST");
    xGateCallObj *callObj = new xGateCallObj();
    callObj->callid = pSipMsg->getUid();
    callObj->m_iTrunkId = pSipMsg->getTrunkId();
    callObj->m_sRouteIP = pSipMsg->getTrunkIPAddress();
    callObj->m_sCallingNumber = pSipMsg->getTrunkUName();
    callObj->sipeventtype = EN_OPTIONS_REQUEST;
     WriteToReceiver(callObj);
    break;
  }
  default:
  {
	XGLOG_ERROR("xGateSipSignalProcessor:handleIncomingSIPmsg unknown event");
  }
  }
  return;
}

bool xGateSipSignalProcessor::ProcessDoUserRegistrationResponse(xGateSofiaSipRegobj *pSipMsg)
{
	if(!pSipMsg)
	{
		XGLOG_ERROR("xGateSipSignalProcessor::ProcessDoUserRegistrationResponse failed!,Registration Object is empty");
		return false;
	}

	XGLOG_INFO("xGateSipSignalProcessor::ProcessDoUserRegistrationResponse callId=%s", pSipMsg->callid.c_str());
	pSipMsg->Regevent = EN_REG_RESPONSE;
	//m_regcontext.erase(pSipMsg->transid.c_str());
	PostToReceiver(pSipMsg);
	return true;
}

bool xGateSipSignalProcessor::ProcessRegistrationResponseError(xGateSofiaSipRegobj *pSipMsg)
{
    if(!pSipMsg)
    {
      XGLOG_ERROR("xGateSipSignalProcessor::ProcessRegistrationResponseError failed!,Registration Object is empty");
      return false;
    }

    XGLOG_INFO("xGateSipSignalProcessor::ProcessRegistrationResponseError callId=%s", pSipMsg->callid.c_str());
    pSipMsg->Regevent = EN_REG_RESPONSE_ERROR;
    m_regcontext.erase(pSipMsg->transid.c_str());//after receving error from controller removing from map
    PostToReceiver(pSipMsg);

  return true;
}

/*
*/
xGateRetVal xGateSipSignalProcessor::stop(){
  m_run = false;
  msg_queue()->deactivate();
  return EN_XGATE_STATUS_SUCCESS;
}

void xGateSipSignalProcessor::handleSipMessage(xGateCallObj* callObj)
{
  if(!callObj) {
    XGLOG_INFO("xGateSipSignalProcessor::handleSipMessage Error callobj nullptr");
    return ;
  }

  XGLOG_INFO("xGateSipSignalProcessor::handleSipMessage uid=%s", callObj->uid.c_str());
  switch(callObj->sipeventtype)
  {
    case EN_INVITE_REQ:
    case EN_INVITE_INBOUND_PSTN_REQ:
    case EN_INVITE_MS_TEAMS_REQ:
    {
      XGLOG_INFO("xGateSipSignalProcessor::handleSipMessage processing Invite request");
      handleSipInviteRequest(callObj);
      break;
    }
    case EN_REGISTRATION_REQ:
    {
      XGLOG_INFO("xGateSipSignalProcessor::handleSipMessage processing Registration request");
      handleSipRegistrationProcess(callObj->m_regObj);
      if (callObj)
      {
        delete callObj;
        callObj = nullptr;
      }
      break;
    }
    case EN_INVITE_SET_TIMER:
    {
     XGLOG_INFO("xGateSipSignalProcessor::handleSipMessage EN_INVITE_SET_TIMER received.");
     SendStartTimerRequest(callObj);
     break;
    }
    case EN_INVITE_CANCEL_TIMER:
    {
     XGLOG_INFO("xGateSipSignalProcessor::handleSipMessage EN_INVITE_CANCEL_TIMER received.");
     SendCancelTimerRequest(callObj);
     break;
    }
    case EN_OPTIONS_SUCCESS_RESPONSE:
    {
      XGLOG_INFO("xGateSipSignalProcessor::handleSipMessage EN_OPTIONS_SUCCESS_RESPONSE received.");
      sendOptionsSuccResponstoMoniterService(callObj);
      break;
    }
    case EN_OPTIONS_FAILED_RESPONSE:
    {
      XGLOG_INFO("xGateSipSignalProcessor::handleSipMessage EN_OPTIONS_FAILED_RESPONSE received.");
      sendOptionsFailedResponstoMoniterService(callObj);
      break;
    }

    default:
    {
      break;
    }
  }
}



void xGateSipSignalProcessor::SendStartTimerRequest(xGateCallObj *callObj)
{
   XGLOG_FUNCTION_EN_EX("");

   xGateSchedulerServiceMsg *pSchedulerMsg = new xGateSchedulerServiceMsg();

   if(!pSchedulerMsg)
   {
      XGLOG_ERROR("xGateSipSignalProcessor::SendStartTimerRequest to create xGateSchedulerServiceMsg!");
      return;
   } 
   SchedulerInfo schedulerInfo;
   schedulerInfo.m_contextId        = callObj->uid.c_str();
   schedulerInfo.m_schedulerType    = EN_SCHEDULER_INVITE_TIMER;
   schedulerInfo.m_retryCount       = 0;
   schedulerInfo.m_timeoutSecond    = callObj->iTimerValue;
   schedulerInfo.m_requestorModule  = IURDefines::UR_MODULE_SIPSIGNAL_SERVICE; 
   schedulerInfo.m_enumTimerFunc = SCHEDULER_FUNCTION_SET_TIMER;
   pSchedulerMsg->setUid(callObj->uid);
   pSchedulerMsg->setDstModuleId(IURDefines::UR_MODULE_SCHEDULER_SERVICE);
   pSchedulerMsg->setSrcModuleId(IURDefines::UR_MODULE_SIPSIGNAL_SERVICE);
   pSchedulerMsg->set_scheduler_info(schedulerInfo);

   IURModuleConfig * pModCofig = nullptr;
   pModCofig = getConfig();

   if(!pModCofig)
   {
      XGLOG_ERROR("xGateSipSignalProcessor::SendStartTimerRequest Pointer to Module Config is nullptr");
      return;
   }
   XGLOG_INFO("xGateSipSignalProcessor::SendStartTimerRequest to Scheduler service");

   pModCofig->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg*>(pSchedulerMsg));
}

void xGateSipSignalProcessor::SendCancelTimerRequest(xGateCallObj *callObj)
{
   XGLOG_FUNCTION_EN_EX("");

   xGateSchedulerServiceMsg *pSchedulerMsg = new xGateSchedulerServiceMsg();

   if(!pSchedulerMsg)
   {
      XGLOG_ERROR("xGateSipSignalProcessor::SendStartTimerRequest to create xGateSchedulerServiceMsg!");
      return;
   } 
   SchedulerInfo schedulerInfo;
   schedulerInfo.m_contextId        = callObj->uid.c_str();
   schedulerInfo.m_schedulerType    = EN_SCHEDULER_INVITE_TIMER;
   schedulerInfo.m_retryCount       = 0;
   schedulerInfo.m_timeoutSecond    = callObj->iTimerValue;
   schedulerInfo.m_requestorModule  = IURDefines::UR_MODULE_SIPSIGNAL_SERVICE; 
   schedulerInfo.m_enumTimerFunc    = SCHEDULER_FUNCTION_CANCEL_TIMER;
   pSchedulerMsg->setUid(callObj->uid);
   pSchedulerMsg->setDstModuleId(IURDefines::UR_MODULE_SCHEDULER_SERVICE);
   pSchedulerMsg->setSrcModuleId(IURDefines::UR_MODULE_SIPSIGNAL_SERVICE);
   pSchedulerMsg->set_scheduler_info(schedulerInfo);

   IURModuleConfig * pModCofig = nullptr;
   pModCofig = getConfig();

   if(!pModCofig)
   {
      XGLOG_ERROR("xGateSipSignalProcessor::SendStartTimerRequest Pointer to Module Config is nullptr");
      return;
   }
   XGLOG_INFO("xGateSipSignalProcessor::SendStartTimerRequest to Scheduler service");

   pModCofig->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg*>(pSchedulerMsg));
}

/*
 * Send Options Success Response to Moniter Service Module
 */
void xGateSipSignalProcessor::sendOptionsSuccResponstoMoniterService(xGateCallObj *callObj)
{
  if (!callObj)
  {
    XGLOG_WARN("xGateSipSignalProcessor::sendOptionSuccResponstoMoniterService failed callObj is nullptr");
    return;
  }
  xGateSipMsg *ptSipMsg = new xGateSipMsg(IURDefines::UR_MODULE_SERVICE_CONTROLLER,
                                          IURDefines::UR_MODULE_MONITOR_SERVICE,
                                          xGateSipMsg::SIP_EVENT_TYPE::SIP_EVENT_OPTION_SUCCESS_RESPONSE,
                                          xGateSipMsg::SIP_MSG_TYPE::SIP_MSG_COMMAND);
  ptSipMsg->setUid(callObj->uid);
  ptSipMsg->setTrunkId(callObj->m_iTrunkId);
  ptSipMsg->setTrunkIPAddress(callObj->m_sRouteIP);
  ptSipMsg->setTrunkUname(callObj->m_sCallingNumber);
  ptSipMsg->setResponseCode(callObj->m_RespCode);
  ptSipMsg->setSrcModuleId(IURDefines::UR_MODULE_SIPSIGNAL_SERVICE);
  ptSipMsg->setDstModuleId(IURDefines::UR_MODULE_MONITOR_SERVICE);
  ptSipMsg->setMsgType(EN_XGATE_MSG_NETIO_MODULE);
  IURModuleConfig *pModCofig = nullptr;
  pModCofig = getConfig();
  delete callObj;
  callObj = nullptr;
  if (!pModCofig)
  {
    XGLOG_ERROR("xGateSipSignalProcessor::sendOptionSuccResponstoMoniterService Pointer to Module Config is nullptr");
    return;
  }
  XGLOG_INFO("xGateSipSignalProcessor::sendOptionSuccResponstoMoniterService Successfully Post to Moniter service callid(%s) TrunkId(%d) IpAddress(%s) ",ptSipMsg->getUid().c_str(), ptSipMsg->getTrunkId(),ptSipMsg->getTrunkIPAddress().c_str());
  pModCofig->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(ptSipMsg));
}

/*
 * Send Options Failed  Response to Moniter Service Module
 */
void xGateSipSignalProcessor::sendOptionsFailedResponstoMoniterService(xGateCallObj *callObj)
{
  if (!callObj)
  {
    XGLOG_WARN("xGateSipSignalProcessor::sendOptionFailedResponstoMoniterService failed callObj is nullptr");
    return;
  }
  xGateSipMsg *ptSipMsg = new xGateSipMsg(IURDefines::UR_MODULE_SERVICE_CONTROLLER,
                                          IURDefines::UR_MODULE_MONITOR_SERVICE,
                                          xGateSipMsg::SIP_EVENT_TYPE::SIP_EVENT_OPTIONS_FAILED_RESPONSE,
                                          xGateSipMsg::SIP_MSG_TYPE::SIP_MSG_COMMAND);
  ptSipMsg->setUid(callObj->uid);
  ptSipMsg->setTrunkId(callObj->m_iTrunkId);
  ptSipMsg->setTrunkIPAddress(callObj->m_sRouteIP);
  ptSipMsg->setTrunkUname(callObj->m_sCallingNumber);
  ptSipMsg->setResponseCode(callObj->m_RespCode);
  ptSipMsg->setSrcModuleId(IURDefines::UR_MODULE_SIPSIGNAL_SERVICE);
  ptSipMsg->setDstModuleId(IURDefines::UR_MODULE_MONITOR_SERVICE);
  ptSipMsg->setMsgType(EN_XGATE_MSG_NETIO_MODULE);
  IURModuleConfig *pModCofig = nullptr;
  pModCofig = getConfig();
  delete callObj;
  callObj = nullptr;
  if (!pModCofig)
  {
    XGLOG_ERROR("xGateSipSignalProcessor::sendOptionFailedResponstoMoniterService Pointer to Module Config is nullptr");
    return;
  }
  XGLOG_INFO("xGateSipSignalProcessor::sendOptionFailedResponstoMoniterService Successfully Post to Moniter service callid(%s) TrunkId(%d) IpAddress(%s)",ptSipMsg->getUid().c_str(), ptSipMsg->getTrunkId(),ptSipMsg->getTrunkIPAddress().c_str());
  pModCofig->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(ptSipMsg));
}

void xGateSipSignalProcessor::handleIncomingSchedulerMsg(xGateBaseMsg *pBaseMsg)
{
   XGLOG_FUNCTION_EN_EX("");

   xGateSchedulerMsg *pMsg = dynamic_cast<xGateSchedulerMsg*>(pBaseMsg);

   if(!pMsg)
   {
      XGLOG_ERROR("handleIncomingSchedulerMsg failed. Invalid SchedulerMsg pointer !");
      return;
   }
   xGateTimerData *data = pMsg->getTimerData();
   if(!data)
   {
      XGLOG_ERROR("handleIncomingSchedulerMsg failed. invalid xGateTimerData !");
      return;
   }

   switch(data->m_timerType)
   {
     case EN_SCHEDULER_INVITE_TIMER:
     {
       xGateCallObj *callObj = new xGateCallObj();

       callObj->uid         = data->m_uid;
       callObj->sipeventtype = EN_INVITE_TIMEOUT;
       WriteToReceiver(callObj);//pushing to receiver thread
      break;
     }
     default:
     {
      XGLOG_ERROR("handleIncomingSchedulerMsg failed. invalid TimerType !");
      break;
     }
	
   }
   if (data)
   {
     delete data;
     data = nullptr;
   }
}

std::string xGateSipSignalProcessor::generateTransid()
{
	uuid_t id;
	size_t    len;

	uuid_generate(id);

	char _string[100];
	memset(_string, 0, sizeof(_string));
	uuid_unparse(id, _string);
	len = strlen(_string);

	//cout << _string << std::endl;
	return UUIDToSessionId((char*)_string, len);
}

std::string xGateSipSignalProcessor::UUIDToSessionId(const char *uuid, const size_t len)
{
	char session_uuid[33] = { 0 }; /*"local-uuid", per rfc7989*/
	if ((!uuid) || len != 36) return nullptr;

	for (size_t i = 0, j = 0; i < len; i++)
	{
		if (uuid[i] != '-')
			session_uuid[j++] = uuid[i];
	}
	session_uuid[len + 1] = '\0';

	if (!isValidSessionID(session_uuid, strlen(session_uuid))) return nullptr;
	std::string s(session_uuid);

	return s;
}

bool xGateSipSignalProcessor::isValidSessionID(const char* uuid, const size_t len)
{
	for (size_t i = 0; i < len; i++)
	{
		if ((uuid[i] < '0' || uuid[i] > '9') && (uuid[i] < 'a' || uuid[i] > 'f')) return false;
	}
	return true;
}
