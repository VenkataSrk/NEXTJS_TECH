#include "xGateSipSignalProcessor.h"
#include "xGateSipController.h"
#define THISMODULE "SPQ"
#include "SipMediahandler.h"
#include "xGateSipService.h"
#include "xGateSipSignalReceiver.h"
#include "xGateSipSignalDispatcher.h"
/*
*/


xGateSipSignalProcessor::xGateSipSignalProcessor() : 
  xGateProcessor("SipProcessor")//DebugVM, m_businessHandler(NULL) 
{
  //DebugVM CLog::Error(THISMODULE, "multithreaded-ippbx : xGateSipSignalProcessor::xGateProcessor(%s)", "SPQ Thread Started");
  m_run = true;
  m_strSipModuleName = "SIP Module";
  m_ptModuleCofig =  NULL;
}

/*
*/
xGateSipSignalProcessor::~xGateSipSignalProcessor()
{
}

int xGateSipSignalProcessor::initModule(IURModuleConfig * ptConfig)
{
	if(!ptConfig) {
		return IURDefines::UR_RETURN_ERROR;
  }
	m_ptModuleCofig = ptConfig;	
	this->init(1);
#if 1
    if(SIP_SIGNAL_RECEIVER::instance()->init("SipReceiver") != 0){
        return IURDefines::UR_RETURN_ERROR;
    }

    if(SIP_SIGNAL_DISPATCHER::instance()->init() !=  EN_XGATE_STATUS_SUCCESS){
      return IURDefines::UR_RETURN_ERROR;
    }
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
    ACE_Message_Block* pAmb = NULL;
    pAmb = dynamic_cast<ACE_Message_Block*>(ptMsg);
    ACE_Time_Value tval ((time(NULL) ));
    tval += ACE_Time_Value(0,1);
    SIP_SIGNAL_MODULE::instance()->putq(pAmb,&tval);
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
   int rc;
   rc = pthread_setname_np(ACE_Thread::self(), "SipSignalProc");
   if(rc!=0) {
     printf("Failed to set name for SipSignalProcessor thread");
   }

  ACE_Message_Block *pAmb = 0;
  while(m_run) 
  {
    ACE_Time_Value tv ((time(NULL) + 20));
    int result = getq(pAmb, &tv);
    if((result == -1) && (errno == EWOULDBLOCK)) {
      continue;
    }
    if((result != -1)) {
      process_msg(pAmb);
    } else {
      m_run = false;
    }
    if(pAmb) {
      delete pAmb;
      pAmb = NULL;
    }
  }
  return 0;
}

xGateRetVal xGateSipSignalProcessor::process_msg(ACE_Message_Block *pAmb)
{
  XGLOG_INFO("xGateSipSignalProcessor::process_msg Start Function");
  xGateRetVal retval = EN_XGATE_STATUS_SUCCESS;
  if(pAmb) {
    XGLOG_INFO("xGateSipSignalProcessor::process_msg pAmb exits");
    xGateBaseMsg *pBaseMsg = NULL;	 
    pBaseMsg = dynamic_cast<xGateBaseMsg*>(pAmb);
    XGLOG_INFO("xGateSipSignalProcessor::process_msg msgType=%d", pBaseMsg->getMsgType());
    switch(pBaseMsg->getMsgType()) {
      case EN_XGATE_MSG_SIP_IN://inbound  call indication
        {
          xGateSipMsgBlock *pSipMsg = dynamic_cast<xGateSipMsgBlock*>(pAmb);
          if(pSipMsg) {
            handleSipMessage(pSipMsg->m_callObj);
          }
          break;
        }
      case EN_XGATE_MSG_SIP_MODULE:// messaage from outside of sip module
        {
          xGateSIPMsg *pSipMsg = dynamic_cast<xGateSIPMsg*>(pAmb);
          if(NULL != pSipMsg) {
          handleIncomingSIPmsg(pSipMsg);
          } else {
            XGLOG_ERROR("xGateSipSignalProcessor::process_msg Error callobj NULL");
          }
          break;
        }
      case EN_XGATE_MSG_UNKNOWN:
      default:
        {
          break;
        }      

    }
  }
  return retval;
}

void xGateSipSignalProcessor::handleIncomingSIPmsg(xGateSIPMsg *pSipMsg)
{
	if(!pSipMsg)
	{
		XGLOG_ERROR("xGateSipSignalProcessor::handleIncomingSIPmsg failed!.Sip Message object is empty");
		return;
	}

	XGLOG_INFO("xGateSipSignalProcessor:handleIncomingSIPmsg Function start");

	XGLOG_INFO("xGateSipSignalProcessor:handleIncomingSIPmsg event type is %d uid=%s", pSipMsg->getSipEventType(), pSipMsg->getUid().c_str());
	switch(pSipMsg->getSipEventType())
	{
		case xGateSIPMsg::SIP_EVENT_TYPE::SIP_EVENT_ANSWER_CALL:
      {
        XGLOG_INFO("xGateSipSignalProcessor:handleIncomingSIPmsg event type is SIP_EVENT_ANSWER_CALL");
        std::map<std::string, xGateCallObj*>::iterator itr;
        std::string tuid = pSipMsg->getUid().c_str();
        itr = m_callcontext.find(tuid);
        if(itr != m_callcontext.end())
        {
          if(pSipMsg->getFeatureType() == XGATECALLFEATURE::EN_URCBCONF_CALL)
          {
            XGLOG_INFO("xGateSipSignalProcessor:handleIncomingSIPmsg Identified as URConfBridge Call");
            std::string sdpStr = pSipMsg->getSdpInfo().c_str();
            std::string strRouteAddr = pSipMsg->getRouteAddr().c_str();
            SendAnswerResponseForConfCall(itr->second,sdpStr,strRouteAddr);
            break;
          }
        }
        else
        {
          XGLOG_ERROR("xGateSipSignalProcessor::handleIncomingSIPmsg uid matches NOT FOUND");
        }
        break;
      }
    case xGateSIPMsg::SIP_EVENT_TYPE::SIP_EVENT_REDIRECT_CALL:
      {
        XGLOG_INFO("xGateSipSignalProcessor:handleIncomingSIPmsg event type is SIP_EVENT_REDIRECT_CALL");
        std::map<std::string, xGateCallObj*>::iterator itr;
        std::string tuid = pSipMsg->getUid().c_str();
        itr = m_callcontext.find(tuid);
        if(itr != m_callcontext.end())
        {
          if(pSipMsg->getFeatureType() == XGATECALLFEATURE::EN_URCBCONF_CALL)
          {
            XGLOG_INFO("xGateSipSignalProcessor:handleIncomingSIPmsg Identified as URConfBridge Call");
            std::string strRedirectIp = pSipMsg->getRedirectIp().c_str();
            SendRedirectRequestForConfCall(itr->second,strRedirectIp);
            break;
          }
        }
        else
        {
          XGLOG_ERROR("xGateSipSignalProcessor::handleIncomingSIPmsg uid matches NOT FOUND");
        }
        break;
      }
		case xGateSIPMsg::SIP_EVENT_TYPE::SIP_EVENT_CALL_DISCONNECTED:
      {
        XGLOG_INFO("xGateSipSignalProcessor:handleIncomingSIPmsg event type is SIP_EVENT_CALL_DISCONNECTED");
        std::map<std::string, xGateCallObj*>::iterator itr;
        std::string temp = pSipMsg->getUid().c_str();
        itr = m_callcontext.find(temp);
        if(itr != m_callcontext.end())
        {
          XGLOG_INFO("xGateSipSignalProcessor::handleIncomingSIPmsg uid matches");
          xGateCallObj *callObj = itr->second;
          //Send Terminated Request to Receiver
          sendCallDisconnectRequestToReceiver(itr->second);
        }
        else
        {
        XGLOG_ERROR("xGateSipSignalProcessor::handleIncomingSIPmsg uid match NOT FOUND");
        } 
        break;
      }
    case xGateSIPMsg::SIP_EVENT_TYPE::SIP_EVENT_ERROR_RESPONSE:
      {
        XGLOG_INFO("xGateSipSignalProcessor:handleIncomingSIPmsg event type is SIP_EVENT_ERROR_RESPONSE");
        std::map<std::string, xGateCallObj *>::iterator itr = m_callcontext.begin();
        for (itr; itr != m_callcontext.end(); ++itr)
        {
          if(itr->first == pSipMsg->getUid()) 
          {
            if(!itr->second) {
              XGLOG_INFO("xGateSipSignalProcessor:handleIncomingSIPmsg sip message structure is empty for uid(%s)",itr->first.c_str());
              break;
            }
            handleErrorMessageResponse(itr->second,pSipMsg);
            break;
          }
        }
        break;
      }
		default:
		{
			XGLOG_ERROR("xGateSipSignalProcessor:handleIncomingSIPmsg unknown event");
		}
	}
	return;
}

bool xGateSipSignalProcessor::handleErrorMessageResponse(xGateCallObj *callObj,xGateSIPMsg *pSipMsg)
{
	if(!pSipMsg)
	{
		XGLOG_INFO("xGateSipSignalProcessor::handleErrorMessageResponse failed!Sip Message is NULL");
		return false;
	}

	XGLOG_INFO("xGateSipSignalProcessor::handleErrorMessageResponse uid=%s", callObj->uid.c_str());

	switch(pSipMsg->getSipErrorEvent())
	{
		case xGateSIPMsg::SIP_ERROR_EVENT_TYPE::SIP_ERROR_EVENT_SERVER_TIMEOUT:
			{
				XGLOG_INFO("xGateSipSignalProcessor::handleErrorMessageResponse uid=%s", callObj->uid.c_str());
				callObj->cp.sipErrResponse = "SIP_ERROR_EVENT_SERVER_TIMEOUT";
				break;
			}
		case xGateSIPMsg::SIP_ERROR_EVENT_TYPE::SIP_ERROR_EVENT_VALIDATION_FAILURE:
			{
				XGLOG_INFO("xGateSipSignalProcessor::handleErrorMessageResponse uid=%s", callObj->uid.c_str());
				callObj->cp.sipErrResponse = "SIP_ERROR_EVENT_VALIDATION_FAILURE";
				break;
			}
		case xGateSIPMsg::SIP_ERROR_EVENT_TYPE::SIP_ERROR_EVENT_REQ_NOT_ACCEPTABLE:
			{
				XGLOG_INFO("xGateSipSignalProcessor::handleErrorMessageResponse uid=%s", callObj->uid.c_str());
				callObj->cp.sipErrResponse = "SIP_ERROR_EVENT_REQ_NOT_ACCEPTABLE";
				break;
			}
		default :
			{
				XGLOG_INFO("xGateSipSignalProcessor::handleErrorMessageResponse uid=%s", callObj->uid.c_str());
				break;
			}
	}
	callObj->sipeventtype = EN_ERROR_NOTIFY_REL;
	WriteToReceiver(callObj);
	return true;
}

bool xGateSipSignalProcessor::SendAnswerResponseForConfCall(xGateCallObj *callObj,std::string updatedSdp,std::string strRouteAddr)
{
	if(!callObj)
	{
		XGLOG_INFO("xGateSipSignalProcessor::SendAnswerResponseForConfCall failed!Object is NULL");
		return false;
	}

	XGLOG_INFO("xGateSipSignalProcessor::SendAnswerResponseForConfCall uid=%s", callObj->uid.c_str());
	callObj->sipeventtype= EN_SETMEDIA_EREL;
	callObj->cp.sdpStr = updatedSdp.c_str();
        callObj->cp.routeIpAddress = strRouteAddr.c_str();
	callObj->cp.xgateCallFeature = XGATECALLFEATURE::EN_URCBCONF_CALL;
	WriteToReceiver(callObj);
	return true;
}

bool xGateSipSignalProcessor::SendRedirectRequestForConfCall(xGateCallObj *callObj,std::string strRedirectIp)
{
  if(!callObj) {
          XGLOG_INFO("xGateSipSignalProcessor::SendRedirectRequestForConfCall failed!Object is NULL");
          return false;
  }
  XGLOG_INFO("xGateSipSignalProcessor::SendRedirectRequestForConfCall uid=%s", callObj->uid.c_str());
  callObj->sipeventtype = EN_REDIRECT_REQUEST_EVENT_REL;
  callObj->cp.redirectIp = strRedirectIp.c_str();
  callObj->cp.xgateCallFeature = XGATECALLFEATURE::EN_URCBCONF_CALL;
  WriteToReceiver(callObj);
  return true;
}

void xGateSipSignalProcessor::sendCallDisconnectRequestToReceiver(xGateCallObj *callObj)
{
  XGLOG_INFO("xGateSipSignalProcessor::sendCallDisconnectRequestToReceiver Start of Function");
  callObj->sipeventtype= EN_TERMINATED_REL;
  //Erasing the entry from map
  m_callcontext.erase(callObj->uid.c_str());
  WriteToReceiver(callObj);
}

void xGateSipSignalProcessor::sendCallConnectedRequest(xGateCallObj *callObj)
{
  XGLOG_INFO("xGateSipSignalProcessor::sendCallConnectedRequest");
  xGateSIPMsg *sMsg = NULL;
  sMsg = new xGateSIPMsg(IURDefines::UR_MODULE_SIPSIGNAL_SERVICE,IURDefines::UR_MODULE_SERVICE_CONTROLLER, xGateSIPMsg::SIP_EVENT_TYPE::SIP_EVENT_CALL_CONNECTED, xGateSIPMsg::SIP_MSG_TYPE::SIP_MSG_REQUEST);
  sMsg->setDstModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
  sMsg->setMsgType(EN_XGATE_MSG_SIP_MODULE);
  sMsg->setUid(callObj->uid.c_str());
  if(!callObj->cp.sdpStr.empty()) {
    sMsg->setSdpInfo(callObj->cp.sdpStr.c_str());
  }
  if(callObj) {
    delete callObj;
    callObj = NULL;
  }
  IURModuleConfig * pModCofig = NULL;
  pModCofig = getConfig();
  if(!pModCofig) {
    XGLOG_ERROR("xGateSipSignalProcessor::sendCallConnectedRequest Pointer to Module Config is NULL");
    return;
  }
  XGLOG_INFO("xGateSipSignalProcessor::sendCallConnectedRequest Sending Call Connected Request to Controller");
  pModCofig->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(sMsg));
  return;
}

void xGateSipSignalProcessor::sendCallDisconnectedRequest(xGateCallObj *callObj)
{
  XGLOG_INFO("xGateSipSignalProcessor::sendCallDisconnectedRequest");
  xGateSIPMsg *sMsg = NULL;
  sMsg = new xGateSIPMsg(IURDefines::UR_MODULE_SIPSIGNAL_SERVICE,IURDefines::UR_MODULE_SERVICE_CONTROLLER, xGateSIPMsg::SIP_EVENT_TYPE::SIP_EVENT_CALL_DISCONNECTED, xGateSIPMsg::SIP_MSG_TYPE::SIP_MSG_REQUEST);
  sMsg->setDstModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
  sMsg->setMsgType(EN_XGATE_MSG_SIP_MODULE);
  sMsg->setUid(callObj->uid);
  if(!callObj->cp.sdpStr.empty()) { 
    sMsg->setSdpInfo(callObj->cp.sdpStr.c_str());
  }
  if(callObj) {
    delete callObj;
    callObj = NULL;
  }
  IURModuleConfig * pModCofig = NULL;
  pModCofig = getConfig();
  if(!pModCofig) {
    XGLOG_ERROR("xGateSipSignalProcessor::sendCallDisconnectedRequest Pointer to Module Config is NULL");
    return;
  }
  XGLOG_INFO("xGateSipSignalProcessor::sendCallDisconnectedRequest Sending Call Disconnect Request to Controller");
  pModCofig->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(sMsg));
}

/*
*/
xGateRetVal xGateSipSignalProcessor::stop()
{
  m_run = false;
  msg_queue()->deactivate();
  return EN_XGATE_STATUS_SUCCESS;
}

void xGateSipSignalProcessor::handleSipMessage(xGateCallObj* callObj)
{
  if(!callObj) {
    XGLOG_INFO("xGateSipSignalProcessor::handleSipMessage Error callobj NULL");
    return ;
  }
  XGLOG_INFO("xGateSipSignalProcessor::handleSipMessage uid=%s", callObj->uid.c_str());
  switch(callObj->sipeventtype)
  {
    case EN_INVITE_REQ:
      {
        if ((strlen(callObj->cp.cnCalled.c_str()) > 5 ||  strlen(callObj->cp.cnCalling.c_str())> 10)) {
            callObj->cp.isPstnNumber = true;
        }
        xGateSIPMsg *sMsg = NULL;
        sMsg = new xGateSIPMsg(IURDefines::UR_MODULE_SIPSIGNAL_SERVICE,IURDefines::UR_MODULE_SERVICE_CONTROLLER, xGateSIPMsg::SIP_EVENT_TYPE::SIP_EVENT_NEW_CALL, xGateSIPMsg::SIP_MSG_TYPE::SIP_MSG_NOTIFY);
        sMsg->fillMediaDetails(sMsg, callObj);
        sMsg->setDstModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
        sMsg->setMsgType(EN_XGATE_MSG_SIP_MODULE);
        sMsg->setUid(callObj->uid.c_str());

        if(!callObj->cp.sdpStr.empty()) {
          XGLOG_INFO("xGateSipSignalProcessor::handleSipMessage uid=%s filling sdp info (%s)", callObj->uid.c_str(),callObj->cp.sdpStr.c_str());
                sMsg->setSdpInfo(callObj->cp.sdpStr.c_str());
        }
        if(callObj->cp.tUnKnownHeaderMap.size() > 0) {
          sMsg->setUnKnownHeaderInfo(callObj->cp.tUnKnownHeaderMap);
        }
        std::map<std::string, xGateCallObj*>::iterator itr;
        itr = m_callcontext.find(callObj->uid.c_str());
        if(itr != m_callcontext.end())
        {
          XGLOG_INFO("xGateSipSignalProcessor::handleSipMessage Editing value in m_callcontext list");
          itr->second = callObj;
        }
        else
        {
          XGLOG_INFO("xGateSipSignalProcessor::handleSipMessage Inserting value in m_callcontext list");
          std::string tuid = callObj->uid.c_str();
          m_callcontext.insert(std::make_pair(tuid, callObj));
        }     
        if(!sMsg)
        {
          XGLOG_ERROR("xGateSipSignalProcessor::handleSipMessage");
          return;
        }
        IURModuleConfig * pModCofig = NULL;
        pModCofig = getConfig();
        if(!pModCofig) {
          XGLOG_ERROR("xGateSipSignalProcessor::handleSipMessage Pointer to Module Config is NULL");
          return;
        }
        XGLOG_INFO("xGateSipSignalProcessor::handleSipMessage Sending New Call Notify to Controller");
        pModCofig->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(sMsg));
        break;
      }
    case EN_200_REL:
      {
        XGLOG_INFO("xGateSipSignalProcessor::handleSipMessage ACK received for uid : %s",callObj->uid.c_str());
        sendCallConnectedRequest(callObj);
        break;
      }
    case EN_BYE_REQ:
      {
        XGLOG_INFO("xGateSipSignalProcessor::handleSipMessage BYE received.");
        sendCallDisconnectedRequest(callObj);
        break;
      }
    default:
      {
        break;
      }
  }
}
