#include "MVNOBusinessService.h"
#include "BusinessServer.h"
#include "CAlog.h"
#include "LogHandler.h"
bool MVNOBusinessService::run(){
    loginfo("", "MVNOBusinessService::run()");
    const std::string thread_name = "MVNOBSser";
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
            delete pMsg;
            pMsg =  nullptr;
        }
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
return true;
}

bool MVNOBusinessService::init(){
    // Load the Default common Business Configuration 
    loginfo("", "MVNOBusinessService::init()");
    std::unique_ptr<CA::CAMessageBlock> pMsg ( new CA::CAMessageBlock());
    pMsg->setMsgType(MESSAGEEVENT::MSG_BS2CONF);
    BusinessServer::post(BUSINESS_THREADS::MVNO_CONF,std::move(pMsg));    

    /*
    }*/
    return true;

}
bool MVNOBusinessService::loadBSRequestProcessor(CA::CAMessageBlock* pMsg ){
    #if 0
    loginfo("", "MVNOBusinessService::loadBSRequestProcessor()");
if(!pMsg)
    return false;
bool retVal = false;;
    std::unique_ptr<BSRequestProcessor> pBsp (new BSRequestProcessor );
	pBsp->setThreadStatus(true);
	if(pBsp->spwan(*pBsp)){
		BusinessServer::setBusinessProcessor(pBsp.release());
		//std::unique_ptr<CA::CAMessageBlock> pMsg(new CA::CAMessageBlock);
        std::unique_ptr<BSGlobalConfObject> confObj (new BSGlobalConfObject() );
        confObj->m_bsListenPort = ((BSGlobalConfObject*)pMsg)->m_bsListenPort;
        confObj->m_callTimeOut =  ((BSGlobalConfObject*)pMsg)->m_callTimeOut;
		confObj->setMsgType(MESSAGEEVENT::MSG_INIT);
		retVal =BusinessServer::post(BUSINESS_THREADS::MVNO_BSP,std::move(confObj));
	}
    return retVal;
    #endif
    return false;
}

bool MVNOBusinessService::handleMsg(CA::CAMessageBlock* pMsg){
    loginfo("", "MVNOBusinessService::handleMsg()");
    if(pMsg){
        unsigned long msgType = pMsg->getMsgType();
    logdebug("", "MVNOBusinessService::handleMsg() Event %d", pMsg->getMsgType());    
    switch(pMsg->getMsgType()){
        case MESSAGEEVENT::MSG_INIT:
        
        break;
        case MESSAGEEVENT::MSG_SHUTDOWN:
        break;
        case MESSAGEEVENT::MSG_CONF2BS:
            if(getConfigurationDetails(pMsg)){
                initServerSocket();
               // loadBSRequestProcessor(pMsg);
            }
        break;
        case MESSAGEEVENT::MSG_SOCDATA:
        // Send the data to BS Request Processor
        receiveSocketData(pMsg);

        break;
        case MESSAGEEVENT::MSG_SOCDATA_SEND:
        sendSocketData(pMsg);
        break;
        default:
        break;

    }
    
        return true;
    }
    return false;

    
}
bool MVNOBusinessService::getConfigurationDetails(CA::CAMessageBlock* pMsg){
    loginfo("", "MVNOBusinessService::getConfigurationDetails()");
    if(pMsg){
        
        std::unique_ptr<BSGlobalConfObject> confObj (new BSGlobalConfObject() );
        confObj->m_bsListenPort = ((BSGlobalConfObject*)pMsg)->m_bsListenPort;
        confObj->m_callTimeOut =  ((BSGlobalConfObject*)pMsg)->m_callTimeOut;
        m_bsConfObj =  std::move(confObj);
    }
    
    return true;
}

bool MVNOBusinessService::initServerSocket(){
    loginfo("", "MVNOBusinessService::initServerSocke()");
    if(m_serverSocket == nullptr && m_bsConfObj ){
        logdebug("", "MVNOBusinessService::initServerSocke() Port %s",m_bsConfObj->m_bsListenPort.c_str());
        std::unique_ptr<CA::CASocket> pTemp (new CA::CASocket(SOC_SERVER, atoi(m_bsConfObj->m_bsListenPort.c_str()), SOC_TCP, this, 5, 5, {},(int)MESSAGEEVENT::MSG_SOCDATA));
        if(pTemp)
            m_serverSocket =  std::move(pTemp);
        return true;
    }
    return false;

}
bool MVNOBusinessService::receiveSocketData(CA::CAMessageBlock* pMsg){
    loginfo("", "MVNOBusinessService::receiveSocketData()");
    if(pMsg){
        CA::CASocketMsg* pSockData = (CA::CASocketMsg*)pMsg;
        loginfo("","MVNOBusinessService::receiveSocketData() Client IP %s  Client Port %d", pSockData->getClientIP().c_str() ,pSockData->getClientPort());
        loginfo("","MVNOBusinessService::receiveSocketData() Data %s", pSockData->getData().c_str());
        BSParam *pbsp =  new BSParam();
          if(pbsp){
            if(pbsp->decodeJson(pSockData->getData().c_str())){
                switch((EN_CA_BUSINESS_STATE)pbsp->getBSstate()){
                    case EN_CA_BUSINESS_STATE::EN_CA_BUSINESS_STATE_INITIATE:
                        if(getRequest(pbsp->getUid()) == nullptr){
                            logsessioninit(pbsp->getUid(),"",pbsp->getCli(),pbsp->getDdi(),"");
                            BSRawRequest* bsRR =  new BSRawRequest();
                            bsRR->m_strUid = pbsp->getUid();
                            bsRR->m_strIP = pSockData->getClientIP();
                            bsRR-> m_strJson =  pSockData->getData();
                            bsRR->port = pSockData->getClientPort();
                            insertRequest(bsRR);
                            loginfo(pbsp->getUid(),"MVNOBusinessService::receiveSocketData() Data %s", pSockData->getData().c_str());
                            postCallMessage(pbsp);
                            loginfo(pbsp->getUid(),"MVNOBusinessService::receiveSocketData() posted msg to BSRequestProcessor");
                        }
                    break;
                    case EN_CA_BUSINESS_STATE::EN_CA_BUSINESS_STATE_PROCEEDING:
                    case EN_CA_BUSINESS_STATE::EN_CA_BUSINESS_STATE_ALERTING:
                    case EN_CA_BUSINESS_STATE::EN_CA_BUSINESS_STATE_CONNECTED:
                    case EN_CA_BUSINESS_STATE::EN_CA_BUSINESS_STATE_DISCONNECTED:
                    case EN_CA_BUSINESS_STATE::EN_CA_BUSINESS_STATE_CLEAR:
		    case EN_CA_BUSINESS_STATE::EN_CA_BUSINESS_STATE_ROUTE_ERROR:
                    if(getRequest(pbsp->getUid()) != nullptr){
			    loginfo(pbsp->getUid(),"MVNOBusinessService::receiveSocketData() Data %s", pSockData->getData().c_str());
                        postCallMessage(pbsp);
                        loginfo(pbsp->getUid(),"MVNOBusinessService::receiveSocketData() posted msg to BSRequestProcessor");
		    }else{
			    if(pbsp){
				    delete pbsp;
				    pbsp = nullptr;
			    }
		    }
                    break;
                }
            }
          }
        }
    return true;
}
bool MVNOBusinessService::sendSocketData(CA::CAMessageBlock* pMsg){
    loginfo("", "MVNOBusinessService::sendSocketData()");
    if(pMsg){
        if(m_serverSocket){
            BSCallMsg* bsmsg = (BSCallMsg*)pMsg;
            BSParam* bsp =  bsmsg->pbsp.get();
            if(bsp){
                BSRawRequest* bsRR =  getRequest(bsp->getUid());
                if(bsRR){
                    std::string returnJson = bsp->encodeJson();
                    switch((BSCallState)bsp->getState()){
                        case BSCallState::STATE_TERMINATE:
                        case BSCallState::STATE_CLEAR: 
                            loginfo(bsp->getUid(),"MVNOBusinessService::sendSocketData() Data %s", returnJson.c_str());
                            m_serverSocket->sendData(returnJson.c_str(),returnJson.length(),bsRR->m_strIP,atoi(m_bsConfObj->m_bsListenPort.c_str()));
                            loginfo("","MVNOBusinessService::sendSocketData() Send  IP %s Port %s",bsRR->m_strIP.c_str(),m_bsConfObj->m_bsListenPort.c_str());
			    loginfo(bsp->getUid(),"*******************************ERROR : CALL LOG END ******************************************");
                            logsessiondeinit(bsp->getUid(),"","","","");
                            deleteRequest(bsp->getUid());
                            delete bsRR;
                            bsRR =  nullptr;
                        break;
                        case BSCallState::STATE_ROUTE_ERROR:
                             loginfo(bsp->getUid(),"MVNOBusinessService::sendSocketData() Data %s", returnJson.c_str());
			     m_serverSocket->sendData(returnJson.c_str(),returnJson.length(),bsRR->m_strIP,atoi(m_bsConfObj->m_bsListenPort.c_str()));
                             postCallMessage(bsmsg->pbsp.release());
                             deleteRequest(bsp->getUid());
                             delete bsRR;
                             bsRR = nullptr;
                        break;
                        case BSCallState::STATE_DISCONNECTED:
                             loginfo(bsp->getUid(),"MVNOBusinessService::sendSocketData() Call Got Disconnected, Data %s",returnJson.c_str());
                             loginfo(bsp->getUid(),"*******************************ERROR : CALL LOG END ******************************************");
                             logsessiondeinit(bsp->getUid(),"","","","");
                             deleteRequest(bsp->getUid());
                             delete bsRR;
                             bsRR =  nullptr;
                        break;
                        case BSCallState::STATE_DIALING: // just for update
                        loginfo(bsp->getUid(),"MVNOBusinessService::sendSocketData() Data %s", returnJson.c_str());
                        m_serverSocket->sendData(returnJson.c_str(),returnJson.length(),bsRR->m_strIP,atoi(m_bsConfObj->m_bsListenPort.c_str()));
                        loginfo("","MVNOBusinessService::sendSocketData() Send  IP %s Port %s",bsRR->m_strIP.c_str(),m_bsConfObj->m_bsListenPort.c_str());
                        // update to BSP 
                        postCallMessage(bsmsg->pbsp.release());
                        break;
                        default:
                        break;

                    }
                }else{ loginfo("","MVNOBusinessService::sendSocketData Request not in Queue");}
                
            }
        }
    }
    return true;
}
bool MVNOBusinessService::postCallMessage(BSParam* bBSp){
    if(bBSp){
        std::unique_ptr<BSCallMsg> pCallMsg (new BSCallMsg);
        if(pCallMsg){
            //  pCallMsg->m_signalClientIP = pSockData->getClientIP();
            //pCallMsg->m_signalClientPort =  pSockData->getClientPort();
            //pCallMsg->m_jsonInString = pSockData->getData();
            pCallMsg->setMsgType(MESSAGEEVENT::MSG_BS2BSP);
            std::unique_ptr<BSParam> tempParam (bBSp);
            pCallMsg->pbsp = std::move(tempParam);
            BusinessServer::post(BUSINESS_THREADS::MVNO_BSP,std::move(pCallMsg));
            return true;
        }

    }
    return false;
}

bool MVNOBusinessService::insertRequest(BSRawRequest* bsRR){
    if(bsRR){
        m_rawRequest.InsertToMap(bsRR->m_strUid,bsRR);
        return true;
    }
return false;
}
BSRawRequest* MVNOBusinessService::getRequest(std::string uid){
    if(!uid.empty()){
        BSRawRequest* bsRR = m_rawRequest.FindValue(uid);
        return bsRR;
    }
return nullptr;
}
bool MVNOBusinessService::deleteRequest(std::string uid){
    if(getRequest(uid) != nullptr){
        m_rawRequest.DeleteFromMap(uid);
        return true;
    }
return false;
}





