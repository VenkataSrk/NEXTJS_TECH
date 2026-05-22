#include "BSRequestProcessor.h"
#include "CAlog.h"
#include "BusinessServer.h"
#include "LogHandler.h"
#define  MAXROUTNGSERVICE 10
bool BSRequestProcessor::run(){
    loginfo("","BSRequestProcessor::run()" );
    const std::string thread_name = "ReqProcessor";
    int rc = pthread_setname_np(pthread_self(), thread_name.c_str());
    if (rc != 0) {
	    std::cerr << "Failed to set name for thread: " << strerror(rc) << std::endl;
    } else {
	    std::cout << "Thread name set successfully to: " << thread_name << std::endl;
    }
    while(getThreadStatus()){
   CA::CAMessageBlock * pMsg = (CA::CAMessageBlock *)RecvMessage();
    if(pMsg){
        handleMsg(pMsg);
        // check the pMsg may be handleMsg could have fwd the msg to other service 
        if(pMsg){delete pMsg; pMsg =  nullptr;}
    }
        
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
    return true;
}
bool BSRequestProcessor::handleMsg(CA::CAMessageBlock *pMsg){
    loginfo("","BSRequestProcessor::handleMsg()" );
    if(pMsg){
         unsigned long msgType = pMsg->getMsgType();
         logdebug("","BSRequestProcessor::handleMsg() %d", msgType );
        switch(msgType){
            case MESSAGEEVENT::MSG_INIT:
                init(pMsg);
            break;
            case MESSAGEEVENT::MSG_CONFUPDATE:
            updateConfig(pMsg);
            break;
            case MESSAGEEVENT::MSG_SHUTDOWN:
            break;
            case MESSAGEEVENT::MSG_TIMER:
                handleTimerMessage(pMsg);
            break;
            case MESSAGEEVENT::MSG_BS2BSP:
                handleMessageFromBS(pMsg);
            break;
            case MESSAGEEVENT::MSG_CONF2BSP:
                handleMessageFromConf(pMsg);
            break;
            case MESSAGEEVENT::MSG_BDB2BSP:
                handleMessageFromDBS(pMsg);
            break;
            case MESSAGEEVENT::MSG_HTTP2BSP:
                 handleMessageFromHttp(pMsg);
            break;
            default:
            break;
        }
        return true;
    }
        

    return false;
}
/* common function for all response to call agent from MOMT, Charging Serive Routing Service GMSC Service*/
bool BSRequestProcessor::responseToCallAgent(CA::CAMessageBlock *pMsg){
if(!pMsg){
        logerror("","BSRequestProcessor::responseToCallAgent() Message Block is Null");
        return false;
    }
    BSOperationMessage* bsOpr =  (BSOperationMessage*)pMsg;
    if(!bsOpr){
        logerror("","BSRequestProcessor::responseToCallAgent() Message type cast error");
        return false;
    }
    if(!bsOpr->m_bsOperation ){
        logerror("","BSRequestProcessor::responseToCallAgent() Operation Object found empty");
        return false;
    }
       
    BSCallObject* bsCallObj =  getFromRequestMap(bsOpr->m_bsOperation->getUuid());
    if(!bsCallObj){
        logerror(bsOpr->m_bsOperation->getUuid(),"BSRequestProcessor::responseToCallAgent() Call Object not found in MAP");
        return false;
    }

    bool retVal =  false;
    bsCallObj->checkCallStateUpdate(std::move(bsOpr->m_bsOperation));
    bsCallObj->getOperationObj()->getInParam()->setBusinessResponseTime(getTimeStrFormated(std::time(0)));

   switch(bsCallObj->getCallState()){
    case BSCallState::STATE_DIALING:
    loginfo(bsCallObj->getUid(),"BSRequestProcessor::responseToCallAgent() --  Dialout send" );
        retVal =   formJsonResponse(bsCallObj);
    break;
    case BSCallState::STATE_TERMINATE:
        loginfo(bsCallObj->getUid(),"BSRequestProcessor::responseToCallAgent() --  Terminate  send" );
        retVal =   formJsonResponse(bsCallObj);
        clearCallObject(bsCallObj);
    break;
    case BSCallState::STATE_CLEAR: // stop session log
        loginfo(bsCallObj->getUid(),"BSRequestProcessor::responseToCallAgent() --    clear state (clear call Object Object)" );
        clearCallObject(bsCallObj);
        retVal = true;
    break;
   }
    
   return retVal;
}
void BSRequestProcessor::clearCallObject(BSCallObject* bsCallObj){
    if(!bsCallObj)
        return ;
    loginfo(bsCallObj->getUid(),"BSRequestProcessor::clearCallObject()");
   // logsessiondeinit(bsCallObj->getUid(),"","","","");
    deleteFromRequestMap(bsCallObj->getUid());
    delete bsCallObj;
    bsCallObj = nullptr;
    loginfo("","BSRequestProcessor::clearCallObject total Call in Queue %d", m_bsRequestMap.GetMapSize());
}

void BSRequestProcessor::clearParamObject(BSParam* bsp){
	if(!bsp)
		return;
	delete bsp;
	bsp = nullptr;
}

bool BSRequestProcessor::init(CA::CAMessageBlock* pMsg){
    loginfo("","BSRequestProcessor::init()" );
    m_strcallTimeOut =  "40";
    if(!pMsg)
        return false;
    BSConfigMsgBlock* confMsg = (BSConfigMsgBlock*)pMsg;
    m_bsconfiguration =  std::move(confMsg->m_configDetails);
    m_strcallTimeOut =  m_bsconfiguration->getCallTimeOut();
    m_strDBWaitTimeOut =  m_bsconfiguration->getDBWaitTimeOut();
    // start the pool 
    startRoutingPool(MAXROUTNGSERVICE);
    //m_strcallTimeOut =  ((BSGlobalConfObject*)pMsg)->m_callTimeOut;
    return true;
}
bool BSRequestProcessor::handleMessageFromMoMt(CA::CAMessageBlock* pMsg){
    logdebug("","BSRequestProcessor::handleMessageFromMoMt()");
    return responseToCallAgent(pMsg);
    
}
bool BSRequestProcessor::handleMessageFromHttp(CA::CAMessageBlock* pMsg){
    loginfo("", "BSRequestProcessor::handleMessageFromHttp()");
    if(!pMsg){
        logerror("", "BSRequestProcessor::handleMessageFromHttp() OperationMSG  Object is NULL");
        return false;
    }
    BSOperationMessage *bsOprMsg = (BSOperationMessage*)pMsg;

    
    //BSOperation *bsOpr =  bsOprMsg->m_bsOperation.get();
    if(!bsOprMsg->m_bsOperation){
        logerror("", "BSRequestProcessor::handleMessageFromHttp() Operation Object is NULL");
        return false;
    }
    loginfo(bsOprMsg->m_bsOperation->getUuid(),"BSRequestProcessor::handleMessageFromHttp()");
    BSCallObject* obj =     getFromRequestMap(bsOprMsg->m_bsOperation->getUuid());
    
    if(!obj){
        loginfo(bsOprMsg->m_bsOperation->getUuid(),"BSRequestProcessor::handleMessageFromHttp() Call Object is NULL (call may be terminated)");
        return false;
    }
    
    loginfo(obj->getUid(),"BSRequestProcessor::handleMessageFromHttp() Call State %d",obj->getCallState());
    if(obj->getCallState() == BSCallState::STATE_PROCESSING){
       BSOperationMessage* pHttpMsg = obj->callHttpResponseInd(bsOprMsg->m_bsOperation.release());
       postMessage(pHttpMsg,obj->getThreadHandler());
    }
    return true;
}
bool BSRequestProcessor::handleTimerMessage(CA::CAMessageBlock* pMsg){
    loginfo("", "BSRequestProcessor::handleTimerMessage()");
    if (!pMsg)
        return false;
    BSCallTimerMsg *timeOutMsg = (BSCallTimerMsg*)pMsg;
    BSCallObject* obj = getFromRequestMap(timeOutMsg->m_uuid);
    if(obj){
        loginfo(obj->getUid(), "BSRequestProcessor::handleTimerMessage() Call State is %d ",obj->getCallState() );
        switch((BS_TIMER_EVENT)timeOutMsg->event){
            case BS_TIMER_EVENT::BS_CALL_TIMEOUT:{
                BSOperationMessage* bsOprMsg  = obj->callTimeOutInd();
                loginfo(obj->getUid(),"BSRequestProcessor::handleTimerMessage() BS_TIMER_EVENT::BS_CALL_TIMEOUT Message %s",obj->getLastKnownError().c_str());
                postMessage(bsOprMsg,obj->getThreadHandler());
                if(obj->getCallState() < BSCallState::STATE_CONNECTED){
                    deleteFromRequestMap(obj->getUid());
                    delete obj; obj = nullptr;
                }
            }
            break;
            case BS_TIMER_EVENT::BS_CALL_TIMECHARGING:{
                BSOperationMessage* bsOprMsg = obj->callChargingcycleInd(timeOutMsg->taskId);
                postMessage(bsOprMsg,obj->getThreadHandler());
                loginfo(obj->getUid(), "BSRequestProcessor::handleTimerMessage() BS_TIMER_EVENT::BS_CALL_TIMECHARGING %s",obj->getLastKnownError().c_str());
            }
            break;
            case BS_TIMER_EVENT::BS_CALL_TIMEMAXDURATION: // send to Momt Service
            break;
            case BS_TIMER_EVENT::BS_CALL_TIMECLEARIND: // clear the call
                
            default:
            break;

        }
    }else{
        loginfo("", "BSRequestProcessor::handleTimerMessage Not Valid Call Object or Call Object not found");
        if(BS_TIMER_EVENT::BS_DB_TIMEOUT==(BS_TIMER_EVENT)timeOutMsg->event)
        {
            loginfo("", "BSRequestProcessor::handleTimerMessage triggered BS_DB_TIMEOUT evnt");
            auto itr = m_bsThreadPool.Begin();
            for(itr;itr!=m_bsThreadPool.End();++itr)
            {
                loginfo("", "BSRequestProcessor::handleTimerMessage BS_DB_TIMEOUT thread id(%s)", itr->first.c_str());
                if(itr->second)
                {
                    std::unique_ptr<BSConfigMsgBlock> pMsg (new BSConfigMsgBlock);
                    if(pMsg)
                    {
                        pMsg->setMsgType(MESSAGEEVENT::MSG_DBWAITTIMEOUT);
                        itr->second->PostMessage(std::move(pMsg));
                    }
                }
            }
            loginfo("", "BSRequestProcessor::handleTimerMessage Again going to start DB timer");
            BusinessServer::setTimerEvent(std::stoi(m_strDBWaitTimeOut),(int)BS_TIMER_EVENT::BS_DB_TIMEOUT,"GlobalUID",0);
		}
	}
    return true;
}



bool BSRequestProcessor::handleMessageFromCharging(CA::CAMessageBlock* pMsg){
    logdebug("","BSRequestProcessor::handleMessageFromCharging()");
    return responseToCallAgent(pMsg);
    
}
bool BSRequestProcessor::handleMessageFromRouting(CA::CAMessageBlock* pMsg){
    logdebug("","BSRequestProcessor::handleMessageFromRouting()");
    return responseToCallAgent(pMsg);
    
}
bool BSRequestProcessor::handleMessageFromGmsc(CA::CAMessageBlock* pMsg){
    logdebug("","BSRequestProcessor::handleMessageFromGmsc()");
    
    return responseToCallAgent(pMsg);
    
}
bool BSRequestProcessor::handleMessageFromConf(CA::CAMessageBlock* pMsg){
    loginfo("","BSRequestProcessor::handleMessageFromConf()" );
    if(!pMsg){
        logerror("","BSRequestProcessor::handleMessageFromConf() MSG is NULL" );
        return false;
    }
    BSOperationMessage* bsOpr =  (BSOperationMessage*)pMsg;
    //BSCallConfObject* bsConfig =  (BSCallConfObject*)pMsg;
   #if 0
    if(bsOpr && bsOpr->m_bsOperation && bsOpr->m_bsOperation->getConfigObj())
        if(bsOpr->m_bsOperation->getConfigObj()->m_errorCode != BSERRDEF::BSError::NOERROR){
            CAlog_ERROR("BSRequestProcessor::handleMessageFromConf() %s",BSERRDEF::BSException::getBSerrorstr (bsOpr->m_bsOperation->getConfigObj()->m_errorCode).c_str());
            switch(bsOpr->m_bsOperation->getConfigObj()->m_errorCode){
                case BSERRDEF::BSError::CONFIG_NOAPP:
                case BSERRDEF::BSError::CONFIG_LOADFAILURE:
                case BSERRDEF::BSError::CONFIG_NOGROUP:
                case BSERRDEF::BSError::CONFIG_NOGROUPS:
                {
                    
                    BSParam* bsp = bsOpr->m_bsOperation->getInParam();
                    bsp->setErrorCode(500);
                    bsp->setState((int)EN_CA_BUSINESS_STATE::EN_CA_BUSINESS_STATE_PROCESS_FAILED);
                    BSCallObject* bsCallObj =  getFromRequestMap(bsp->getUid());
                    std::unique_ptr<BSParam> spbsp (bsp);
                    bsOpr->m_bsOperation->updateoutParam(std::move(spbsp));
                    
                    formJsonResponse(bsCallObj);

                }

                break;


            }
                
        }
        #endif
    return true;
}

bool BSRequestProcessor::handleMessageFromBS(CA::CAMessageBlock* pMsg){
loginfo("","BSRequestProcessor::handleMessageFromBS()" );;
    if(pMsg){
        BSCallMsg* msg =  (BSCallMsg*)pMsg;
        BSParam *pbsp = msg->pbsp.release();
        if(pbsp){
           return processSignalEvent(pbsp,"",0);
        }
    }
    return false;
}
bool BSRequestProcessor::handleMessageFromDBS(CA::CAMessageBlock* pMsg){
    loginfo("","BSRequestProcessor::handleMessageFromDBS()" );;
   
    
return true;
}
bool BSRequestProcessor::TestsendMsg(CA::CAMessageBlock* pMsg){
#if 0
    if(pMsg){
        BSCallConfObject * bsCallConfObject =  (BSCallConfObject*)pMsg;
        if(bsCallConfObject){
            BSCallObject* bsCallObject = nullptr;
            bsCallObject = getFromRequestMap(bsCallConfObject->m_uid);
            if(bsCallObject){
                std::cout << bsCallObject->getCallAgentIP() << " " << bsCallObject->getCallAgentPort() << std::endl;
                std::unique_ptr<BSCallMsg> bsCallMsg(new BSCallMsg);
                if(bsCallMsg){
                    bsCallMsg->m_signalClientIP = bsCallObject->getCallAgentIP();
                    bsCallMsg->m_signalClientPort = bsCallObject->getCallAgentPort();
                    //bsCallMsg->m_jsonInString = "{\"msg_len\":348,\"data\":{\"uid\":\"FA37jNCchRYdSBZA\",\"cli\":\"4368861008888\",\"ddi\":\"917893513512\",\"src_ip\":\"10.22.7.184\",\"dest_ip\":\"10.22.7.184\",\"state\":2,\"cause_code\":31,\"display_cli\":\"4368861008888\",\"mod_ddi\":\"917893513512\",\"prefix\":\"\",\"route\":\"10.22.7.184\",\"sip_event\":480,\"brs_ip\":\"10.22.3.20\",\"user_info\":\"\",\"private_info\":\"\",\"ca_ip\":\"10.150.3.55\"}}\r\n\r\n";
                    // Harding coding the values for testing -TODO need proper outParam creation 
                    bsCallObject->getInParam()->setState(2);
                    bsCallMsg->m_jsonInString = bsCallObject->getInParam()->encodeJson();
                    std::cout << " Json Msg " << bsCallMsg->m_jsonInString << std::endl;
                    bsCallMsg->setMsgType(MESSAGEEVENT::MSG_SOCDATA_SEND);
                    BusinessServer::post(BUSINESS_THREADS::MVNO_BUS, std::move(bsCallMsg));

                }
                
            }
        }
        return true;
    }
    #endif 
return false;
}
bool BSRequestProcessor::formJsonResponse(BSCallObject* bsCallObj){
if(bsCallObj){
     //std::unique_ptr<BSCallMsg> bsCallMsg(new BSCallMsg);
     if(bsCallObj->getOperationObj()){
        switch (bsCallObj->getOperationObj()->getSipErrorCode()){
            case 200:
                if(bsCallObj->getOperationObj()->getOutParam()){
                    bsCallObj->getOperationObj()->getOutParam()->setErrorCode(200);
                }
            break;
            default:
                if(bsCallObj->getOperationObj()->getOutParam()){
                    bsCallObj->getOperationObj()->getOutParam()->setErrorCode(bsCallObj->getOperationObj()->getSipErrorCode());
                }
            break;

        }
        BSCallMsg* bsCallMsg = new BSCallMsg();
        if(bsCallMsg){
            bsCallMsg->m_signalClientIP = bsCallObj->getCallAgentIP();
            bsCallMsg->m_signalClientPort = bsCallObj->getCallAgentPort();
            bsCallMsg->m_jsonInString =  bsCallObj->getOperationObj()->getOutParam()->encodeJson();
            bsCallMsg->setMsgType(MESSAGEEVENT::MSG_SOCDATA_SEND);
            postMessage(bsCallMsg,nullptr);

        }
     }
}

    return true;
}
bool BSRequestProcessor::processSignalEvent(BSParam* pbsp,std::string caIP, unsigned short caPort){
    
    loginfo("","BSRequestProcessor::processSignalEvent() ");
    loginfo(pbsp->getUid(),"BSRequestProcessor::processSignalEvent() ");
    if(!pbsp){
        logerror("","BSRequestProcessor::processSignalEvent BSParam not found 0x0");
        return false;
    }
    CA::CAMessageBlock* pMsg =  nullptr;
    STR trace = pbsp->getCli()+":"+pbsp->getDdi()+":"+pbsp->getTrunkName();;
    BSCallObject* callObject = getFromRequestMap(pbsp->getUid()); 
    
    switch ((EN_CA_BUSINESS_STATE)pbsp->getBSstate()){
        case EN_CA_BUSINESS_STATE::EN_CA_BUSINESS_STATE_INITIATE:{
            //start the cli based logging
            //logsessioninit(pbsp->getUid(),"",pbsp->getCli(),pbsp->getDdi(),"");
            loginfo(pbsp->getUid(),"BSRequestProcessor::processSignalEvent() EN_CA_BUSINESS_STATE_INITIATE");
            if(callObject){ // this object could be of leak
                delete callObject;
                callObject = nullptr;
            }
            callObject = new BSCallObject(pbsp->getUid(),caIP,caPort);
            if(callObject){
                insertToRequestMap(callObject);
		pbsp->setBusinessRequestTime(getTimeStrFormated(std::time(0)));
                pMsg = (CA::CAMessageBlock*)callObject->callSetupInd(pbsp);
                callObject->setThreadHandler(getAvailableThread());
                // get available routingservice thread from pool
                postMessage(pMsg,callObject->getThreadHandler());
		#if 0
		//The timer does not need to start from here. It should start once we send the operator's IP to the PBX
                BusinessServer::setTimerEvent(std::stoi(m_strcallTimeOut),(int)BS_TIMER_EVENT::BS_CALL_TIMEOUT,pbsp->getUid(),0);
		#endif 
                loginfo(pbsp->getUid(),"BSRequestProcessor::processSignalEvent() %s",callObject->getLastKnownError().c_str());
            }
            
        }
        break;
        case EN_CA_BUSINESS_STATE::EN_CA_BUSINESS_STATE_PROCEEDING:{
            loginfo(pbsp->getUid(),"BSRequestProcessor::processSignalEvent() EN_CA_BUSINESS_STATE_PROCEEDING");
            if(callObject){
                pMsg = (CA::CAMessageBlock*)callObject->callProceedingInd(pbsp);
                loginfo(pbsp->getUid(),"BSRequestProcessor::processSignalEvent() %s",callObject->getLastKnownError().c_str());
            }
        }
        break;
        case EN_CA_BUSINESS_STATE::EN_CA_BUSINESS_STATE_ALERTING:{
            loginfo(pbsp->getUid(),"BSRequestProcessor::processSignalEvent() EN_CA_BUSINESS_STATE_ALERTING");
            if(callObject){
                pMsg = (CA::CAMessageBlock*)callObject->callAlertingInd(pbsp);
                loginfo(pbsp->getUid(),"BSRequestProcessor::processSignalEvent() %s",callObject->getLastKnownError().c_str());
            }
            
        }
        break;
        case EN_CA_BUSINESS_STATE::EN_CA_BUSINESS_STATE_CONNECTED:{
            loginfo(pbsp->getUid(),"BSRequestProcessor::processSignalEvent() EN_CA_BUSINESS_STATE_CONNECTED");
           if(callObject){
                pMsg = (CA::CAMessageBlock*)callObject->callConnectedInd(pbsp);
                postMessage(pMsg,callObject->getThreadHandler());
                loginfo(pbsp->getUid(),"BSRequestProcessor::processSignalEvent() %s",callObject->getLastKnownError().c_str());
            } 
        }
        
        break;
	case EN_CA_BUSINESS_STATE::EN_CA_BUSINESS_STATE_ROUTE_ERROR:{
            loginfo(pbsp->getUid(),"BSRequestProcessor::processSignalEvent() EN_CA_BUSINESS_STATE_ROUTE_ERROR ddi:%s",pbsp->getDdi().c_str());
            if(callObject){
                pMsg = (CA::CAMessageBlock*)callObject->callRouteErrorInd(pbsp);
                postMessage(pMsg,callObject->getThreadHandler());
                loginfo(pbsp->getUid(),"BSRequestProcessor::processSignalEvent() %s",callObject->getLastKnownError().c_str());
            }
        }
        break;
        case EN_CA_BUSINESS_STATE::EN_CA_BUSINESS_STATE_DISCONNECTED:{
            loginfo(pbsp->getUid(),"BSRequestProcessor::processSignalEvent() EN_CA_BUSINESS_STATE_DISCONNECTED");
            if(callObject){
                callObject->getOperationObj()->setSipErrorCode(pbsp->getErrorCode()); 
                pMsg = (CA::CAMessageBlock*)callObject->callDisconnectedInd(pbsp);
                postMessage(pMsg,callObject->getThreadHandler());
                loginfo(pbsp->getUid(),"BSRequestProcessor::processSignalEvent() %s",callObject->getLastKnownError().c_str());
                clearCallObject(callObject);
	    }else{
		    loginfo("","BSRequestProcessor::processSignalEvent() No callObject is found for UID");
                    clearParamObject(pbsp);
	    }
        }
        break;
        case EN_CA_BUSINESS_STATE::EN_CA_BUSINESS_STATE_PROCESS_FAILED:{
             loginfo(pbsp->getUid(),"BSRequestProcessor::processSignalEvent() EN_CA_BUSINESS_STATE_FAILED");
             if(callObject){
                     loginfo(pbsp->getUid(),"BSRequestProcessor::processSignalEvent() %s",callObject->getLastKnownError().c_str());
                     loginfo(pbsp->getUid(),"*******************************ERROR : CALL LOG END ******************************************");
                     logsessiondeinit(pbsp->getUid(),"","","","");
                     deleteFromRequestMap(pbsp->getUid());
                     clearParamObject(pbsp);
                     clearCallObject(callObject);
             }else{
                     loginfo("","BSRequestProcessor::processSignalEvent() No callObject is found for UID(%s)");
                     clearParamObject(pbsp);
             }
        }
        break;
        case EN_CA_BUSINESS_STATE::EN_CA_BUSINESS_STATE_CLEAR:{
            loginfo(pbsp->getUid(),"BSRequestProcessor::processSignalEvent() EN_CA_BUSINESS_STATE_CLEAR");
            if(callObject){
               
                pMsg = (CA::CAMessageBlock*)callObject->callClearInd();
                loginfo(pbsp->getUid(),"BSRequestProcessor::processSignalEvent() %s",callObject->getLastKnownError().c_str());
                postMessage(pMsg,callObject->getThreadHandler());
                clearCallObject(callObject);
                clearParamObject(pbsp);
	    }else{
		    loginfo("","BSRequestProcessor::processSignalEvent() No callObject is found for UID");
		    clearParamObject(pbsp);
	    }
        }
        break;
        case EN_CA_BUSINESS_STATE::EN_CA_BUSINESS_STATE_PROCESS_SUCCESS:{
            loginfo(pbsp->getUid(),"BSRequestProcessor::processSignalEvent() EN_CA_BUSINESS_STATE_PROCESS_SUCCESS (dialing)");
            BusinessServer::setTimerEvent(std::stoi(m_strcallTimeOut),(int)BS_TIMER_EVENT::BS_CALL_TIMEOUT,pbsp->getUid(),0);
            if(callObject){
                callObject->callDialingInd(pbsp);
                loginfo(pbsp->getUid(),"BSRequestProcessor::processSignalEvent() %s",callObject->getLastKnownError().c_str());
            }
        }
        break;
        default:
        break;
        
    }
    return true;
}


bool BSRequestProcessor::postMessage(CA::CAMessageBlock* pMsg,CA::CAThread* th){
loginfo("","BSRequestProcessor::postMessage() ");
    if(!pMsg)
        return false;
    
    std::unique_ptr<CA::CAMessageBlock> ptrMsgBlock(pMsg);
    logdebug("","BSRequestProcessor::postMessage() %d", pMsg->getMsgType());
    switch(pMsg->getMsgType()){
        case MESSAGEEVENT::MSG_BSP2CONF:
         
            BusinessServer::post(BUSINESS_THREADS::MVNO_CONF, std::move(ptrMsgBlock));
        break;
        case MESSAGEEVENT::MSG_BSP2CHARG:
           
            BusinessServer::post(BUSINESS_THREADS::MVNO_CHARGE, std::move(ptrMsgBlock));
        break;
        case MESSAGEEVENT::MSG_SOCDATA_SEND:
        
            BusinessServer::post(BUSINESS_THREADS::MVNO_BUS, std::move(ptrMsgBlock));
        break;
        case MESSAGEEVENT::MSG_BSP2MOMT:
            
            BusinessServer::post(BUSINESS_THREADS::MVNO_BSMOMT, std::move(ptrMsgBlock));
        break;
        case MESSAGEEVENT::MSG_BSP2GMSC:
            BusinessServer::post(BUSINESS_THREADS::MVNO_GMSC, std::move(ptrMsgBlock));
        break;
        case MESSAGEEVENT::MSG_BSP2BSOP:
        if(th){
            RoutingService* rsTh =  (RoutingService*)th;
            rsTh->PostMessage(std::move(ptrMsgBlock));
        }
        break;
        default:
        break;
    }
    return true;
}


bool BSRequestProcessor::insertToRequestMap(BSCallObject* bcCallObj){

if(!bcCallObj){
    logerror("","BSRequestProcessor::insertToRequestMap CallObject is Null 0x0");
    return false;
}
if(bcCallObj->getUid().empty()){
    logerror("","BSRequestProcessor::insertToRequestMap Unique ID Empty");
    return false;
}

m_bsRequestMap.InsertToMap(bcCallObj->getUid(),bcCallObj);
    return true;
}
BSCallObject* BSRequestProcessor::getFromRequestMap(std::string uuid){
    loginfo(uuid,"BSRequestProcessor::getFromRequestMap and size is %d",m_bsRequestMap.GetMapSize());
    if(uuid.empty()){
        return nullptr;
    }
    BSCallObject* bsCallObject = m_bsRequestMap.FindValue(uuid);
    return bsCallObject;
}

bool BSRequestProcessor::deleteFromRequestMap(std::string uuid){
    loginfo("","BSRequestProcessor::deleteFromRequestMap");
    return m_bsRequestMap.DeleteFromMap(uuid);
}

void BSRequestProcessor::startRoutingPool(int maxTP){
   loginfo("","BSRequestProcessor::startRoutingPool() "); 
    if (maxTP > 0){
        for (int tp = 0; tp < maxTP; tp++){
            RoutingService* prsThread =  new RoutingService();
            if(prsThread){
                prsThread->setThreadStatus(true);
                if(prsThread->spwan(*prsThread)){
                    m_bsThreadPool.InsertToMap(std::to_string(tp),(CA::CAThread*)prsThread);
                    std::unique_ptr<BSConfigMsgBlock> pMsg (new BSConfigMsgBlock);
                    if(pMsg){
                        std::unique_ptr<BusinessConfig> confdt (new BusinessConfig);
                        if(confdt){
                            confdt->copyConfig(*(m_bsconfiguration.get()));
                            pMsg->m_configDetails = std::move(confdt);
                            pMsg->setMsgType(MESSAGEEVENT::MSG_INIT);
                            prsThread->PostMessage(std::move(pMsg));
                        }
                    }
                    
                }
            }
        }
    }
   BusinessServer::setTimerEvent(std::stoi(m_strDBWaitTimeOut),(int)BS_TIMER_EVENT::BS_DB_TIMEOUT,"GlobalUID",0);
}
void BSRequestProcessor::updateConfigRoutingPool(int maxThread){
    loginfo("","BSRequestProcessor::updateConfigRoutingPool() "); 
    if(maxThread > 0){
        for(int tp =0 ;tp < maxThread; tp++){
           RoutingService* proutService = ( RoutingService*)m_bsThreadPool.FindValue(std::to_string(tp));
           if(proutService) {
                std::unique_ptr<BSConfigMsgBlock> pMsg (new BSConfigMsgBlock);
                if(pMsg){
                    std::unique_ptr<BusinessConfig> confdt (new BusinessConfig);
                    if(confdt){
                        confdt->copyConfig(*(m_bsconfiguration.get()));
                        pMsg->m_configDetails = std::move(confdt);
                        pMsg->setMsgType(MESSAGEEVENT::MSG_CONFUPDATE);
                        proutService->PostMessage(std::move(pMsg));
                    }
                }
           }
        }
    }
}
void BSRequestProcessor::updateConfig(CA::CAMessageBlock *pMsg){

    m_strcallTimeOut =  "40";
    if(!pMsg)
        return ;
    BSConfigMsgBlock* confMsg = (BSConfigMsgBlock*)pMsg;
    if(m_bsconfiguration){
        m_bsconfiguration.reset(nullptr);
    }
    m_bsconfiguration =  std::move(confMsg->m_configDetails);
    m_strcallTimeOut =  m_bsconfiguration->getCallTimeOut();
    m_strDBWaitTimeOut = m_bsconfiguration->getDBWaitTimeOut();
    updateConfigRoutingPool(MAXROUTNGSERVICE);

}

CA::CAThread* BSRequestProcessor::getAvailableThread(){
    if(getThreadId() < MAXROUTNGSERVICE){
        loginfo("","BSRequestProcessor::getAvailableThread() Total thread count  %d  Current Thread count %d", getThreadId(), lastVistedThreadId);
       CA::CAThread* th = m_bsThreadPool.FindValue(std::to_string(getThreadId()));
       if(th){
            lastVistedThreadId++;
            return th;
       }
    }else{
        lastVistedThreadId = 0;
        CA::CAThread* th = m_bsThreadPool.FindValue(std::to_string(getThreadId()));
        lastVistedThreadId++;
        return th;
    }

    return nullptr;
}

std::string BSRequestProcessor::getTimeStrFormated(std::time_t frtTime){
     std::time_t now =  frtTime;
     char buf[24] = {0};
    std::strftime(buf, sizeof(buf),"%Y-%0m-%0d %H:%M:%S", std::localtime(&now));
    return  std::string(buf);
}
