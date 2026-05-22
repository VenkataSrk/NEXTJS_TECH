#include "GmscService.h"
#include "BusinessServer.h"
#include "LogHandler.h"

bool GmscService::init(){
    return true;
}

bool GmscService::run(){
    const std::string thread_name = "GmscService";
    int rc = pthread_setname_np(pthread_self(), thread_name.c_str());
    if (rc != 0) {
	    std::cerr << "Failed to set name for thread: " << strerror(rc) << std::endl;
    } else { 
	    std::cout << "Thread name set successfully to: " << thread_name << std::endl;
    }
    while(getThreadStatus()){
        CA::CAMessageBlock * pMsg = (CA::CAMessageBlock *)RecvMessage();
        if(pMsg)
        {
            handleMsg(pMsg); 
        }
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return true;
}

bool GmscService::handleMsg(CA::CAMessageBlock *pMsg){
    loginfo("","GmscService::handleMsg()");
    if(!pMsg){
        return false;
    }
    unsigned long msgType = pMsg->getMsgType();
    
    switch(msgType)
    {
        case MESSAGEEVENT::MSG_INIT:
            loginfo("","GmscService::handleMsg()  MESSAGEEVENT::MSG_INIT");
            ConnectDBProfile(pMsg);
        break;
        case MESSAGEEVENT::MSG_SHUTDOWN:
        break;
        case MESSAGEEVENT::MSG_MOMT2GMSC:
            loginfo("","GmscService::handleMsg()  MESSAGEEVENT::MSG_MOMT2GMSC");
            handleGmscEvent(pMsg);
        break;
        case MESSAGEEVENT::MSG_CHARG2GMSC:
            loginfo("","GmscService::handleMsg()  MESSAGEEVENT::MSG_CHARG2GMSC");
            handleGmscEvent(pMsg);
        break;
        case MESSAGEEVENT::MSG_BSP2GMSC: // only to handle HTTP response
            loginfo("","GmscService::handleMsg()  MESSAGEEVENT::MSG_BSP2GMSC");
            handleGmscEvent(pMsg);
            //handleHttpEvent(pMsg);
        break;
        case MESSAGEEVENT::MSG_HTTP2GMSC:
        loginfo("","GmscService::handleMsg()  MESSAGEEVENT::MSG_HTTP2GMSC");
            handleHttpEvent(pMsg);
        break;
        default:
            loginfo("","GmscService::handleMsg() %d",msgType);
        break;
    }
    return true;
}

bool GmscService::ConnectDBProfile(CA::CAMessageBlock* pMsg){
    loginfo("","GmscService::ConnectDBProfile()");
    BSDBProfileObject* dbconnection = (BSDBProfileObject*)pMsg;
    if(dbconnection)
    {
        for(CA::CAMap<DBProfile*>::ca_itr it = dbconnection->m_dbProObject.Begin(); it != dbconnection->m_dbProObject.End(); it++)
        {
            DBProfile* dbprofile =  it->second;
            if(dbprofile)
            {
                CA::DBInterface *dbInterface =  nullptr;
                if(dbprofile->getDBType().compare("mssql") == 0){
                    dbInterface = new CA::DBMSSql();
                    if(dbInterface)
                     dbInterface->setDbType("mssql");
                }else if(dbprofile->getDBType().compare("mysql") == 0){
                    dbInterface = new CA::DBMYSql();
                    if(dbInterface)
                     dbInterface->setDbType("mysql");
                }
                if(dbInterface){
                    if(dbInterface->openConnection(dbprofile->getConnectionString(),dbprofile->getConntetionTimeout()) !=  CA::CASQLRETURN::CASQL_FAIL){
                        m_dbConnections.InsertToMap(dbprofile->getProfileName(),dbInterface);
                        loginfo("","GmscService::ConnectDBProfile() Profile %s   Connected %x  DB type %s",dbprofile->getProfileName().c_str(),dbInterface ,dbInterface->getDbType().c_str());
                    }else{
                        logerror("","GmscService::ConnectDBProfile() Profile %s  not Connected",dbprofile->getProfileName().c_str());
                    }
                }
            }
        }
        loginfo("","GmscService::ConnectDBProfile() Total Connection Profile %d", m_dbConnections.GetMapSize());
    }
    if(pMsg){ delete pMsg; pMsg = nullptr;};
    return true;
}

CA::DBInterface* GmscService::getDBInstance(std::string profile){
    if(profile.empty())
    {
        return nullptr;
    }
     
    return (CA::DBInterface*)m_dbConnections.FindValue(profile);
}
bool GmscService::handleHttpEvent(CA::CAMessageBlock* pMsg){
    loginfo("", "GmscService::hanldeHttpEvent()");
    if(!pMsg)
        return false;
    BSOperationMessage *bsOprMsg =  static_cast<BSOperationMessage*>(pMsg);

    if(!bsOprMsg)
        return false;
    if(!bsOprMsg->m_bsOperation)
        return false;
    BSOperation* bsp = bsOprMsg->m_bsOperation.get();
    // if(!bsp->getHttpRequestMsg())
    //     return false;
    
    //BSGmsc* bsGmsc =  (BSGmsc*) bsp->getTask(bsp->getHttpRequestMsg()->getAppId());
        //BSGmsc* bsGmsc =  (BSGmsc*) bsp->getTask(bsp->getTaskID());
    BSGmsc* bsGmsc =  (BSGmsc*)getGmscInterface(bsp->getTaskKey());
    if(!bsGmsc)
        return false;

    retCode ret;
    ret = bsGmsc->HLRResponse(m_dbConnections,bsp,bsp->getInParam());
    switch(ret){
        case retCode::BS_SUCCESS:
            bsp->updateExecutionStatus(BSCallState::STATE_PROCESSING,OPERATIONEVENT::OPE_ROUTING_INIT,"",200);
        break;
        case retCode::BS_HLR_FAILED:
            bsp->updateExecutionStatus(BSCallState::STATE_TERMINATE, OPERATIONEVENT::OPE_UNKNOWN, "HLR Response Error", 500);
        break;
        case retCode::BS_ABSENT_SUBSCRIBER:
            bsp->updateExecutionStatus(BSCallState::STATE_TERMINATE, OPERATIONEVENT::OPE_UNKNOWN, "HLR Response Error Absent Subscriber", 480);
        break;
        case retCode::BS_OUTGOING_BARRED:
            bsp->updateExecutionStatus(BSCallState::STATE_TERMINATE, OPERATIONEVENT::OPE_UNKNOWN, "HLR Response Error Outgoing Barred", 403);
        break;
        case retCode::BS_NO_ROUTE:
            bsp->updateExecutionStatus(BSCallState::STATE_TERMINATE, OPERATIONEVENT::OPE_UNKNOWN, "HLR Response Error No route destination", 404);
        break;
        case retCode::BS_ERROR_UNKOWN:
            bsp->updateExecutionStatus(BSCallState::STATE_TERMINATE, OPERATIONEVENT::OPE_UNKNOWN, "HLR Response Error", 500);
        break;

    }
    return postServiceEvent(bsp);
}

bool GmscService::handleGmscEvent(CA::CAMessageBlock* pMsg){
    loginfo("","GmscService::handleGmscEvent()");
    if(!pMsg)
    {
        logerror("","GmscService::handleGmscEvent() Message Block is NULL");
        return false;
    }
    BSOperationMessage * pbsOpr =  (BSOperationMessage*)pMsg;
    if(pbsOpr->m_bsOperation == nullptr){
        logerror("","GmscService::handleGmscEvent() Operation Object is NULL");
        return false;
    }
    bool retVal = false;
    switch((OPERATIONEVENT)pbsOpr->getEvent()){
        case OPERATIONEVENT::OPE_GMSC_INIT:
            loginfo(pbsOpr->m_bsOperation->getUuid(),"GmscService::handleGmscEvent() OPERATIONEVENT::OPE_GMSC_INIT");
           retVal=  loadGmscApp(pbsOpr->m_bsOperation.release());
        break;
        case OPERATIONEVENT::OPE_GMSC_CONNECTED:
            loginfo(pbsOpr->m_bsOperation->getUuid(),"GmscService::handleGmscEvent() OPERATIONEVENT::OPE_GMSC_CONNECTED");
            updateGmscApp(pbsOpr->m_bsOperation.release());
        break;
        case OPERATIONEVENT::OPE_GMSC_DEINIT:
            loginfo(pbsOpr->m_bsOperation->getUuid(),"GmscService::handleGmscEvent() OPERATIONEVENT::OPE_GMSC_DEINIT");
           retVal =  unloadGmscApp(pbsOpr->m_bsOperation.release());
        break;
        case OPERATIONEVENT::OPE_HTTP_RESPONSE:
            loginfo(pbsOpr->m_bsOperation->getUuid(),"GmscService::handleGmscEvent() OPERATIONEVENT::OPE_HTTP_RESPONSE");
            retVal = handleHttpEvent((CA::CAMessageBlock*)pbsOpr);
        break;
        case OPERATIONEVENT::OPE_HTTP_RESPONSE:
             loginfo("","GmscService::handleGmscEvent() OPERATIONEVENT::OPE_HTTP_RESPONSE");
             handleHttpEvent(pMsg);
             break;
        default:
            loginfo(pbsOpr->m_bsOperation->getUuid(),"GmscService::handleGmscEvent() %d unknown Operation Event", pbsOpr->getEvent());
            retVal =  false;
        break;
        
    }
    return retVal;
}

bool GmscService::loadGmscApp(BSOperation* bsOpr){
    if(bsOpr == nullptr){
        logerror("", "GmscService::loadGmscApp Operation Objt is NULL");
        return false;
    }
    loginfo(bsOpr->getUuid(),"GmscService::loadGmscApp()");
 
    BSGmsc* bsGmsc = new BSGmsc(bsOpr->getInParam(),bsOpr->m_app);
    if(!bsGmsc)
    {
        logerror(bsOpr->getUuid(),"GmscService::loadGmscApp() gmsc Object creation error");
        bsOpr->updateExecutionStatus(BSCallState::STATE_UNKNOWN,OPERATIONEVENT::OPE_UNKNOWN,"gmsc Object creation error",500);
        return postServiceEvent(bsOpr);
    }
    retCode retVal = bsGmsc->Load(m_dbConnections,bsOpr->getInParam(),bsOpr->m_app);
    int err = bsGmsc->getEquivalentSipError(retVal);
    switch(retVal){
        case retCode::BS_HLR_REQUEST:
            loginfo(bsOpr->getUuid(),"GmscService::loadGmscApp() BS_HLR_REQUEST --  OPE_HTTP_REQUEST");
            bsOpr->updateExecutionStatus(BSCallState::STATE_PROCESSING,OPERATIONEVENT::OPE_HTTP_REQUEST,"HTTP Requested",err);
            bsOpr->setHttpRequest(std::move(bsGmsc->m_pHttpMsg));
            insertGmscInterface(bsGmsc);
            bsOpr->createBSTask(bsGmsc->getAppKey(), bsGmsc->getAppType()) ;
        break;
        case retCode::BS_SUCCESS:
        case retCode::BS_SUCCESS_DIRECTDIAL:
            loginfo(bsOpr->getUuid(),"GmscService::loadGmscApp() BS_SUCCESS_DIRECTDIAL -- OPE_BSP_DIAL");
            bsOpr->updateExecutionStatus(BSCallState::STATE_DIALING,OPERATIONEVENT::OPE_BSP_DIAL,"GMSC SUCCESS",err);
            insertGmscInterface(bsGmsc);
            bsOpr->createBSTask(bsGmsc->getAppKey(), bsGmsc->getAppType()) ;
        break;
        case retCode::BS_NO_SIGNALIP:
            loginfo(bsOpr->getUuid(),"GmscService::loadGmscApp() BS_NO_SIGNALIP -  OPE_MOMT_SPAWNTASK %s", bsOpr->getLastKnownError().c_str() );
            bsOpr->updateExecutionStatus(BSCallState::STATE_PROCESSING,OPERATIONEVENT::OPE_MOMT_SPAWNTASK,"No signal IP call Forward Check",err);
            insertGmscInterface(bsGmsc);
            bsOpr->createBSTask(bsGmsc->getAppKey(), bsGmsc->getAppType()) ;
        break;
        default:
            bsOpr->updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_UNKNOWN,"Error in Gmsc App",err);
            delete bsGmsc;
            bsGmsc = nullptr;
        break;
    }
    return postServiceEvent(bsOpr);
}
bool GmscService::updateGmscApp(BSOperation* bsOpr){

    if(!bsOpr){
        loginfo("", "GmscService::updateGmscApp Operation Object is NULL");
    }
    loginfo(bsOpr->getUuid(),"GmscService::updateGmscApp()");
    switch(bsOpr->getOperationEvent()){
        case OPERATIONEVENT::OPE_GMSC_CONNECTED:

        break;
    }
    return true;
}

bool GmscService::postServiceEvent(BSOperation* bsOpr){
    if(!bsOpr){
        logerror("", "GmscService::postServiceEvent Operation Object is NULL");
        return false;
    }
    std::unique_ptr<BSOperationMessage> pMsg (new BSOperationMessage );
    if(!pMsg)
        return false;
    BUSINESS_THREADS threadID = BUSINESS_THREADS::UNKNOWN;
    switch((OPERATIONEVENT)bsOpr->getOperationEvent()){
        case OPERATIONEVENT::OPE_HTTP_REQUEST:
            pMsg->setMsgType(MESSAGEEVENT::MSG_GMSC2HTTP);
            threadID = BUSINESS_THREADS::MVNO_HTTP;
        break;
        case OPERATIONEVENT::OPE_MOMT_SPAWNTASK:
            pMsg->setMsgType(MESSAGEEVENT::MSG_GMSC2MOMT);
            threadID = BUSINESS_THREADS::MVNO_BSMOMT;
        break;
        case OPERATIONEVENT::OPE_TASK_NEXT:
        switch(bsOpr->getNextTask()){
            case APPLICATIONTYPE::TYPE_CHARGING:
                pMsg->setMsgType(MESSAGEEVENT::MSG_GMSC2CHARG);
                threadID = BUSINESS_THREADS::MVNO_CHARGE;
            break;
            default:
                pMsg->setMsgType(MESSAGEEVENT::MSG_GMSC2BSP);
                threadID = BUSINESS_THREADS::MVNO_BSP;
            break;
        }
        break;
        case OPERATIONEVENT::OPE_BSP_DIAL:
            pMsg->setMsgType(MESSAGEEVENT::MSG_GMSC2BSP);
            threadID = BUSINESS_THREADS::MVNO_BSP;
        break;
        default:
            pMsg->setMsgType(MESSAGEEVENT::MSG_GMSC2BSP);
            threadID = BUSINESS_THREADS::MVNO_BSP;
        break;
    }
     pMsg->setEvent((int)bsOpr->getOperationEvent());
    std::unique_ptr<BSOperation> tempOperation (bsOpr);
    pMsg->m_bsOperation =  std::move(tempOperation);
    return  BusinessServer::post(threadID,std::move(pMsg));
}
bool GmscService::unloadGmscApp(BSOperation* bsOpr){
    if(bsOpr == nullptr)
    {
        // post Message to BSRequestProcessor for failure case
        logerror("", "GmscService::unloadGmscApp Operation Objt is NULL");
        // post to exception
        return false;
    }
    loginfo(bsOpr->getUuid(),"GmscService::unloadGmscApp()");
    BSGmsc* bsgmsc =  (BSGmsc*)getGmscInterface(bsOpr->getTaskKey());
    if(!bsgmsc){
        bsOpr->updateExecutionStatus(BSCallState::STATE_CLEAR,OPERATIONEVENT::OPE_UNKNOWN,"GMSC Object is NULL",500);
        return postServiceEvent(bsOpr);
    }

    retCode ret = bsgmsc->UnLoad(bsOpr->getInParam(),bsOpr->getConnectedTime(),bsOpr->getDisconnectedTime(),bsOpr->getConnectedTime());
    if(ret == retCode::BS_SUCCESS){
         bsOpr->updateExecutionStatus(BSCallState::STATE_DISCONNECTED,OPERATIONEVENT::OPE_ROUTING_DEINIT,"",200);
    }
    return postServiceEvent(bsOpr);
}

bool GmscService::insertGmscInterface(BSInterface* bsInt){
    if(!bsInt)
        return false;
    m_GmscList.InsertToMap(bsInt->generateAppKey(),bsInt);
    return true;
}
BSInterface* GmscService::getGmscInterface(std::string appKey){
    if(appKey.empty())
        return nullptr;
    BSInterface* bsInt =  m_GmscList.FindValue(appKey);
    return bsInt;

}
bool GmscService::deleteGmscInterface(std::string appKey){
if(appKey.empty())
        return false;
    return m_GmscList.DeleteFromMap(appKey);
}

