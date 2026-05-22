#include "RoutingService.h"
#include "BusinessServer.h"
#include "LogHandler.h"
bool RoutingService::init(){
    loginfo("","RoutingService::init()");

    return true;
}

bool RoutingService::run(){
    const std::string thread_name = "RoutingSer";
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
            if(pMsg){ delete pMsg; pMsg = nullptr;}
        }
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return true;
}
bool RoutingService::setupDBProfile(CA::CAMap<DBProfile*>* dbProlst,REQUESTTYPE rt){

    loginfo("","RoutingService::setupDBProfile()");
    if(dbProlst){
        loginfo("","RoutingService::setupDBProfile() Total Connection Profile %d", dbProlst->GetMapSize());
        for(CA::CAMap<DBProfile*>::ca_itr it = dbProlst->Begin(); it != dbProlst->End(); it++){
            DBProfile* dbprofile =  it->second;
            switch(rt){
                case REQUESTTYPE::RT_CREATE:
                connectDB(dbprofile);
                break;
                case REQUESTTYPE::RT_UPDATE:
                reconnectDB(dbprofile);
                break;
                default:
                break;
            }
        }
    }
    return true;
}
bool RoutingService::connectDB(DBProfile* dbprofile){
    if(dbprofile == nullptr){
        logerror("","RoutingService::connectDB Failed DBProfile  is NULL");
        return false;
    }
     CA::DBInterface *dbInterface = nullptr;
    if(dbprofile->getDBType().compare("mssql") == 0){
        dbInterface = new CA::DBMSSql(dbprofile->getConnectionString(),dbprofile->getConntetionTimeout(),"mssql");
    }else if(dbprofile->getDBType().compare("mysql") == 0){
        dbInterface = new CA::DBMYSql(dbprofile->getConnectionString(),dbprofile->getConntetionTimeout(),"mysql");
    }
    if(dbInterface){
        if(dbInterface->openConnection() != CA::CASQLRETURN::CASQL_FAIL){
            m_dbConnections.InsertToMap(dbprofile->getProfileName(),dbInterface);
            loginfo("","RoutingService::connectDB Profile %s DB type %s -- Connection Success",dbprofile->getProfileName().c_str(),dbInterface->getDbType().c_str());
            return true;
        }
        logerror("","RoutingService::connectDB Profile %s DB type %s -- Connection Failed Reason %s",dbprofile->getProfileName().c_str(),dbInterface->getDbType().c_str(), dbInterface->getLastKnownError().c_str());
    }else{
        logerror("","RoutingService::connectDB Failed DBInterface  Not created");
    }
    
    return false;
}
bool RoutingService::reconnectDB(DBProfile* dbprofile){
    if(dbprofile == nullptr){
        logerror("","RoutingService::reconnectDB Failed DBProfile  is NULL");
        return false;
    }
    CA::DBInterface *dbInterface = m_dbConnections.FindValue(dbprofile->getProfileName());
    if(dbInterface){
        dbInterface->closeConnection();
        dbInterface->setConnectionInfo(dbprofile->getConnectionString(),dbprofile->getConntetionTimeout(),dbprofile->getDBType());
        if(dbInterface->openConnection() != CA::CASQLRETURN::CASQL_FAIL){
            loginfo("","RoutingService::reconnectDB Profile %s DB type %s -- Connection Success",dbprofile->getProfileName().c_str(),dbInterface->getDbType().c_str());
            return true;
        }
        logerror("","RoutingService::reconnectDB Profile %s DB type %s -- Connection Failed Reason %s",dbprofile->getProfileName().c_str(),dbInterface->getDbType().c_str(), dbInterface->getLastKnownError().c_str());

    }else{
        loginfo("","RoutingService::reconnectDB profile %s Failed DBInterface Not found in list",dbprofile->getProfileName().c_str());
        return connectDB(dbprofile);
    }
    return false;
}

bool RoutingService::handleMsg(CA::CAMessageBlock *pMsg){
    loginfo("","RoutingService::handleMsg()");
    if(!pMsg){
         logerror("","RoutingService::handleMsg() Message Block is NULL");
        return false;
    }
    unsigned long msgType = pMsg->getMsgType();
    switch(msgType){
        case MESSAGEEVENT::MSG_INIT: // update Config object
            handleIntMsg(pMsg);
        break;
        case MESSAGEEVENT::MSG_CONFUPDATE:
        handleConfigUpdate(pMsg);
        break;
        case MESSAGEEVENT::MSG_SHUTDOWN:
        break;
        case MESSAGEEVENT::MSG_BSP2BSOP:
            handleMessageFromBSP(pMsg);
        break;
        case MESSAGEEVENT::MSG_DBWAITTIMEOUT:
            handleDBWaitTimeOut();
        break;
    }
    return true;
    
}
bool RoutingService::handleDBWaitTimeOut(){
	loginfo("","RoutingService::handleDummyQueryUpdat()");
	for(auto it = m_dbConnections.Begin(); it != m_dbConnections.End(); it++){
        const std::string& profileName = it->first;
        CA::DBMAP db_wait_time_out;
        logdebug("", "RoutingService::handleDBWaitTimeOut() DB: %s", profileName.c_str());
        it->second->executeQuery("SELECT NOW();", db_wait_time_out);
	}
	return true;
}
bool RoutingService::handleIntMsg(CA::CAMessageBlock* pMsg){
    loginfo("","RoutingService::handleIntMsg()");
    
    if(!pMsg){
         logerror("","RoutingService::handleIntMsg() Message Block is NULL");
        return false;
    }
    
    BSConfigMsgBlock* pconfMsg =  (BSConfigMsgBlock*)pMsg;
    if(!pconfMsg){
         logerror("","RoutingService::handleIntMsg() Message Block Type Cast failure");
        return false;
    }
    std::unique_ptr<BusinessConfig> upConf(std::move(pconfMsg->m_configDetails));
    m_conf =  std::move(upConf);

    if(!m_conf){
         logerror("","RoutingService::handleIntMsg() BusinessConfig is NULL");
        return false;
    }

    return setupDBProfile(m_conf.get()->getDBProfileList(),REQUESTTYPE::RT_CREATE);
}
bool RoutingService::handleConfigUpdate(CA::CAMessageBlock* pMsg){
    loginfo("","RoutingService::handleConfigUpdate()");
    if(!pMsg){
         logerror("","RoutingService::handleConfigUpdate() Message Block is NULL");
        return false;
    }
    BSConfigMsgBlock* pconfMsg =  (BSConfigMsgBlock*)pMsg;
    if(!pconfMsg){
         logerror("","RoutingService::handleConfigUpdate() Message Block Type Cast failure");
        return false;
    }
    if(m_conf){
        m_conf.reset(nullptr);
    }
    //std::unique_ptr<BusinessConfig> upConf());
    m_conf =  std::move(pconfMsg->m_configDetails);
    if(!m_conf){
         logerror("","RoutingService::handleConfigUpdate() BusinessConfig is NULL");
        return false;
    }

    return setupDBProfile(m_conf.get()->getDBProfileList(),REQUESTTYPE::RT_UPDATE);
}
bool RoutingService::taskInit(BSOperation* bsOpr){
    if(!bsOpr){
      logerror("","RoutingService::taskInit() Operatin Object is NULL");
        return false;  
    }
    loginfo(bsOpr->getUuid(),"RoutingService::taskInit() Operation state %d", bsOpr->getOperationEvent());
    if(bsOpr->callInRoute(m_conf.get(),m_dbConnections)){
       loginfo(bsOpr->getUuid(), "RoutingService::taskInit() Opertion State %d Execution Msg %s",bsOpr->getOperationEvent(),bsOpr->getLastKnownError().c_str());
       insertBusinessOperation(bsOpr);
        
    }else{ 
        logerror(bsOpr->getUuid(),"RoutingService::taskInit() %s", bsOpr->getLastKnownError().c_str());
    }
    bsOpr->setCallConnectedStatus(0);
    return executeReqOperationEvent(bsOpr);
}
 bool RoutingService::taskConnect(BSOperation* bsOpr){
    if(!bsOpr){
        logerror("", "RoutingService::taskConnect Operation Object is NULL");
        return false;
    }
    loginfo(bsOpr->getUuid(),"RoutingService::taskConnect() Operation state %d", bsOpr->getOperationEvent());
    BSOperation* lbsOpr =  getBusinessOperation(bsOpr->getUuid());
    if(!lbsOpr){
        logerror(bsOpr->getUuid(), "RoutingService::taskConnect() Operation not found in Local List");
        return false;
    }
    lbsOpr->callConnectState(m_conf.get(),m_dbConnections);
    return executeReqOperationEvent(lbsOpr);
 }

 bool RoutingService::taskAlternateRoute(BSOperation* bsOpr){
    if(!bsOpr){
        logerror("", "RoutingService::taskAlternateRoute Operation Object is NULL");
        return false;
    }
    loginfo(bsOpr->getUuid(),"RoutingService::taskAlternateRoute() Operation state %d", bsOpr->getOperationEvent());
    BSOperation* lbsOpr =  getBusinessOperation(bsOpr->getUuid());
    if(!lbsOpr){
        logerror(bsOpr->getUuid(), "RoutingService::taskAlternateRoute() Operation not found in Local List");
        return false;
    }
    if(lbsOpr->loadAlternateRoute(m_dbConnections) == false)
    {
	    lbsOpr->callDisconnectState(m_conf.get(),m_dbConnections);
    }
    return executeReqOperationEvent(lbsOpr);
 }

bool RoutingService::taskDeInit(BSOperation* bsOpr){
    if(!bsOpr){
        logerror("", "RoutingService::taskDeInit Operation Object is NULL");
        return false;
    }
    loginfo(bsOpr->getUuid(),"RoutingService::taskDeInit() Operation state %d", bsOpr->getOperationEvent());
    BSOperation* lbsOpr =  getBusinessOperation(bsOpr->getUuid());
    if(!lbsOpr){
        logerror(bsOpr->getUuid(), "RoutingService::taskDeInit() Operation not found in Local List");
        return false;
    }
    lbsOpr->setSipErrorCode(bsOpr->getSipErrorCode());
    lbsOpr->callDisconnectState(m_conf.get(),m_dbConnections);
    return executeReqOperationEvent(lbsOpr);
}
bool RoutingService::taskHttpResponse(BSOperation* bsOpr){
    if(!bsOpr){
        logerror("", "RoutingService::taskHttpResponse Operation Object is NULL");
        return false;
    }
    loginfo(bsOpr->getUuid(),"RoutingService::taskHttpResponse() Operation state %d", bsOpr->getOperationEvent());
    BSOperation* lbsOpr =  getBusinessOperation(bsOpr->getUuid());
    if(!lbsOpr){
        logerror(bsOpr->getUuid(), "RoutingService::taskHttpResponse() Operation not found in Local List");
        return false;
    }

    lbsOpr->setHttpOutput(bsOpr->getHttpOutput());
    lbsOpr->setHttpStatusCode(bsOpr->getHttpStatusCode());
    lbsOpr->callHttpUpdate(m_conf.get(),m_dbConnections);
    return executeReqOperationEvent(lbsOpr);

}
bool RoutingService::taskChargingCycle(BSOperation*bsOpr){
    if(!bsOpr){
        logerror("", "RoutingService::taskChargingCycle Operation Object is NULL");
        return false;
    }
    loginfo(bsOpr->getUuid(),"RoutingService::taskChargingCycle() Operation state %d", bsOpr->getOperationEvent());
    BSOperation* lbsOpr =  getBusinessOperation(bsOpr->getUuid());
    if(!lbsOpr){
        logerror(bsOpr->getUuid(), "RoutingService::taskChargingCycle() Operation not found in Local List");
        return false;
    }
    lbsOpr->setTaskId(bsOpr->getTaskId());
    lbsOpr->callChargingCycleUpdate(m_conf.get(),m_dbConnections);
    return executeReqOperationEvent(lbsOpr);

}
bool RoutingService::taskClean(BSOperation* bsOpr){
    if(!bsOpr){
        logerror("","RoutingService::taskClean BSOperation is null" );
        return false;
    }
    bsOpr->deleteInterfaces();
    removeBusinessOperation(bsOpr->getUuid());
    delete bsOpr;
    bsOpr =  nullptr;
    return true;
}

bool RoutingService::handleMessageFromBSP(CA::CAMessageBlock* pMsg){
     loginfo("","RoutingService::handleMessageFromBSP()");
     if(!pMsg){
        logerror("","RoutingService::handleMessageFromBSP() Message Block Type Cast failure");
        return false;
     }
     BSOperationMessage* bsOprMsg =  (BSOperationMessage*)pMsg;
     if(!bsOprMsg){
         logerror("","RoutingService::handleMessageFromBSP() Message Block Type Cast failure");
        return false;
     }
     if(!bsOprMsg->m_bsOperation){
        logerror("","RoutingService::handleMessageFromBSP() Operatin Object is NULL");
        return false;
     }
     switch((OPERATIONEVENT)bsOprMsg->getEvent()){
        case OPERATIONEVENT::OPE_TASK_INIT : // The First Step of the call
            taskInit(bsOprMsg->m_bsOperation.release());
        break;
        case OPERATIONEVENT::OPE_TASK_CONNECTED: 
            taskConnect(bsOprMsg->m_bsOperation.get());
        break;
        case OPERATIONEVENT::OPE_TASK_DEINIT: // final step of the call and update bsparam.
            taskDeInit(bsOprMsg->m_bsOperation.get());
        break;
        case OPERATIONEVENT::OPE_HTTP_RESPONSE:
            taskHttpResponse(bsOprMsg->m_bsOperation.get());
        break;
        case OPERATIONEVENT::OPE_CHARING_CYCLE:
            taskChargingCycle(bsOprMsg->m_bsOperation.get());
        break;
	case OPERATIONEVENT::OPE_TASK_ROUTE_ERROR:
	    taskAlternateRoute(bsOprMsg->m_bsOperation.get());
	break;

     }
     return true;
}
bool RoutingService::executeReqOperationEvent(BSOperation* bsOpr){
    if(!bsOpr){
        logerror("","RoutingService::executeReqOperationEvent() Operation Object is null");
        return false;
    }
    loginfo(bsOpr->getUuid(), "RoutingService::executeReqOperationEvent %d -  %s", bsOpr->getOperationEvent(), bsOpr->getLastKnownError().c_str());
    switch(bsOpr->getOperationEvent()){
        case OPERATIONEVENT::OPE_CHARING_INIT:
        // load charging
            loginfo(bsOpr->getUuid(), "RoutingService::executeReqOperationEvent Load Charging");
            bsOpr->loadCharing(m_dbConnections);
            
        break;
        case OPERATIONEVENT::OPE_GMSC_INIT:
        // load gmsc
            loginfo(bsOpr->getUuid(), "RoutingService::executeReqOperationEvent Load Gmsc");
            bsOpr->loadGmsc(m_dbConnections);
           
        break;
        default:
            loginfo(bsOpr->getUuid(), "RoutingService::executeReqOperationEvent On Default %s", bsOpr->getLastKnownError().c_str());
        break;
    }
    return executeResOperationEvent(bsOpr);
}

bool RoutingService::executeResOperationEvent(BSOperation* bsOpr){
    if(!bsOpr){
        logerror("","RoutingService::executeResOperationEvent() Operation Object is null");
        return false;
    }
    loginfo(bsOpr->getUuid(), "RoutingService::executeResOperationEvent %d -  %s", bsOpr->getOperationEvent(), bsOpr->getLastKnownError().c_str());
    switch (bsOpr->getOperationEvent()){
        case OPERATIONEVENT::OPE_TASK_SPAWN:
        // spawn a new task
            bsOpr->callSpawnRoute(m_conf.get(),m_dbConnections);
            executeReqOperationEvent(bsOpr);
        break;
        case OPERATIONEVENT::OPE_TASK_TERMINATE:
            bsOpr->callDisconnectState(m_conf.get(),m_dbConnections);
        case OPERATIONEVENT::OPE_TASK_CLEAR:
        // stop all interface & post to MVNO_BUS thread
            postServiceEvent(bsOpr);
            taskClean(bsOpr);
        break;
        case OPERATIONEVENT::OPE_BSP_DIAL:
            postServiceEvent(bsOpr);
        break;
        case OPERATIONEVENT::OPE_HTTP_REQUEST:
        // post to HTTP thread
        postServiceEvent(bsOpr);
        loginfo(bsOpr->getUuid(),"RoutingService::executeResOperationEvent Suspend on HTTP REQUEST");
        break;
    }


    return true;
}
bool RoutingService::sendHTTPRequest(BSOperation* bsp){
    std::unique_ptr<BSOperation> bsOpr(new BSOperation());
    if(bsOpr){
        bsOpr->copyOperation(bsp);
        bsOpr->setHttpRequest(bsp->relHttpRequestMsg());
        std::unique_ptr<BSOperationMessage> pMsg (new BSOperationMessage());
        if(pMsg){
            pMsg->m_bsOperation =  std::move(bsOpr);
            pMsg->setMsgType(MESSAGEEVENT::MSG_ROUT2HTTP);
            BusinessServer::post(BUSINESS_THREADS::MVNO_HTTP,std::move(pMsg));
        }
    }
    return true;

}

bool RoutingService::postServiceEvent(BSOperation* bsp){

    if(!bsp){
        logerror("", "RoutingService::postServiceEvent() BSoperation is null");
        return false;
    }
    loginfo(bsp->getUuid(),"RoutingService::postServiceEvent() %s",bsp->getLastKnownError().c_str());
    switch(bsp->getOperationEvent()){
        case OPERATIONEVENT::OPE_BSP_DIAL:
        case OPERATIONEVENT::OPE_TASK_TERMINATE:
        case OPERATIONEVENT::OPE_TASK_CLEAR:{
            std::unique_ptr<BSCallMsg> pMsg(new BSCallMsg);
            BSParam *pbspram = new BSParam();
            pbspram->updateBSParam(*bsp->getInParam());
            std::unique_ptr<BSParam> tempbsp(pbspram);
            pMsg->pbsp =  std::move(tempbsp);
            pMsg->setMsgType(MESSAGEEVENT::MSG_SOCDATA_SEND);
            BusinessServer::post(BUSINESS_THREADS::MVNO_BUS,std::move(pMsg));
        }
        break;
        case OPERATIONEVENT::OPE_HTTP_REQUEST:{
            sendHTTPRequest(bsp);
        }
        break;

        default:
        break;
    }
    return true;
}
bool RoutingService::getNextTask(BSOperationMessage* pMsg,BSOperation* bsp){
    return true;

}
bool RoutingService::insertBusinessOperation(BSOperation* bsOpr){
    if(!bsOpr){
        logerror("", "RoutingService::insertBusinessOperation Business Operation is NULL");
        return false;
    }
    if(bsOpr->getUuid().empty()){
        logerror("", "RoutingService::insertBusinessOperation Business Operation Unique ID is empty");
        return false;
    }
    m_OprList.InsertToMap(bsOpr->getUuid(),bsOpr);
    return true;
}
BSOperation* RoutingService::getBusinessOperation(std::string uuid){
    if(uuid.empty())
        return nullptr;

    BSOperation* bsOpr = m_OprList.FindValue(uuid);
    return bsOpr;
}
bool RoutingService::removeBusinessOperation(std::string uuid){
    return m_OprList.DeleteFromMap(uuid);
}
