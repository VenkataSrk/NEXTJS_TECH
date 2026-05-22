
#include "ChargingService.h"
#include "BusinessServer.h"
#include "BusinessConfigObject.h" // Tempory for testing
#include "BSException.h"
#include "LogHandler.h"
#define CA_TRY try {
#define CA_CATCH }catch (std::exception &e){ \
    std::cout << " Exception Throw caught" << std::endl; \
      return false;}
bool ChargingService::init(){
    return true;
}

bool ChargingService::run(){
    loginfo("","ChargingService::run()");
    const std::string thread_name = "ChargingSer";
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
            
        
        }
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return true;
}

bool ChargingService::handleMsg(CA::CAMessageBlock *pMsg){
    if(!pMsg){
        return false;
    }
    loginfo("","ChargingService::handleMsg()");
    unsigned long msgType = pMsg->getMsgType();
    logdebug("","ChargingService::handleMsg() %d",msgType);
    switch(msgType){
        case MESSAGEEVENT::MSG_INIT:
            ConnectDBProfile(pMsg);
        break;
        case MESSAGEEVENT::MSG_SHUTDOWN:
        break;
        case MESSAGEEVENT::MSG_MOMT2CHARG:
        case MESSAGEEVENT::MSG_BSP2CHARG:
            handleChargingEvent(pMsg);
        break;

        
    }
    return true;
}
bool ChargingService::ConnectDBProfile(CA::CAMessageBlock* pMsg){
    loginfo("","ChargingService::ConnectDBProfile()");
    BSDBProfileObject* dbconnection = (BSDBProfileObject*)pMsg;
    if(dbconnection){
        for(CA::CAMap<DBProfile*>::ca_itr it = dbconnection->m_dbProObject.Begin(); it != dbconnection->m_dbProObject.End(); it++){
            DBProfile* dbprofile =  it->second;
            if(dbprofile){
                CA::DBInterface *dbInterface =  nullptr;
                if(dbprofile->getDBType().compare("mssql") == 0){
                        dbInterface = new CA::DBMSSql();
                        if(dbInterface)
                            dbInterface->setDbType("mssql");
                }else if (dbprofile->getDBType().compare("mysql") == 0){
                    dbInterface = new CA::DBMYSql();
                        if(dbInterface)
                            dbInterface->setDbType("mysql");
                }
                if(dbInterface){
                    if(dbInterface->openConnection(dbprofile->getConnectionString(),dbprofile->getConntetionTimeout()) !=  CA::CASQLRETURN::CASQL_FAIL){
                        
                        m_dbConnections.InsertToMap(dbprofile->getProfileName(),dbInterface);
                        loginfo("","ChargingService::ConnectDBProfile() Profile %s   Connected %x  DB type %s",dbprofile->getProfileName().c_str(),dbInterface ,dbInterface->getDbType().c_str());
                    }else{
                        logerror("","ChargingService::ConnectDBProfile() Profile %s  not Connected",dbprofile->getProfileName().c_str());
                    }
                }
            }
        }
        loginfo("","ChargingService::ConnectDBProfile() Total Connection Profile %d", m_dbConnections.GetMapSize());
    }
    if(pMsg){ delete pMsg; pMsg = nullptr;};
    return true;
}
CA::DBInterface* ChargingService::getDBInstance(std::string profile){
    if(profile.empty()){
        return nullptr;
    }
     
    return (CA::DBInterface*)m_dbConnections.FindValue(profile);
}
bool ChargingService::handleChargingEvent(CA::CAMessageBlock* pMsg){
    loginfo("","ChargingService::handleChargingEvent()");
    if(!pMsg)
        return false;
    BSOperationMessage * pbsOpr =  (BSOperationMessage*)pMsg;
    if(pbsOpr->m_bsOperation ==  nullptr){
        loginfo("","ChargingService::handleChargingEvent() Operation Object is NULL");
        return false;
    }
    switch((OPERATIONEVENT)pbsOpr->getEvent()){
        case OPERATIONEVENT::OPE_CHARING_INIT: // validate cli and ddi
            loginfo(pbsOpr->m_bsOperation->getUuid(),"ChargingService::handleChargingEvent() OPERATIONEVENT::OPE_CHARING_INIT");
            loadChargingApp(pbsOpr->m_bsOperation.release()) ;
        break;
        case OPERATIONEVENT::OPE_CHARING_START: // call connected 
            loginfo(pbsOpr->m_bsOperation->getUuid(),"ChargingService::handleChargingEvent() OPERATIONEVENT::OPE_CHARING_START");
            OnCallConnect(pbsOpr->m_bsOperation.release());
        break;
        case OPERATIONEVENT::OPE_CHARING_STOP: // call disconnected
            loginfo(pbsOpr->m_bsOperation->getUuid(),"ChargingService::handleChargingEvent() OPERATIONEVENT::OPE_CHARING_STOP");
            OnCallDisconnect(pbsOpr->m_bsOperation.release());
        break;
        case OPERATIONEVENT::OPE_CHARING_CYCLE:
            loginfo(pbsOpr->m_bsOperation->getUuid(),"ChargingService::handleChargingEvent() OPERATIONEVENT::OPE_CHARING_CYCLE");
            OnCallCycleEvent(pbsOpr->m_bsOperation.release());
        default:
            loginfo(pbsOpr->m_bsOperation->getUuid(),"ChargingService::handleChargingEvent() %d (unkown event) ", pbsOpr->getEvent());
        break;
    }

    return true;
    
}
bool ChargingService::loadChargingApp(BSOperation* bsOpr){
    if(bsOpr == nullptr){
        logerror("", "ChargingService::loadChargingApp Operation Objt is NULL");
        return false;
    }
    loginfo(bsOpr->getUuid(),"ChargingService::loadChargingApp()");
    // Inilizing the Charging object
    BSCharging* bcCharging = new BSCharging(bsOpr->getInParam(),bsOpr->m_app);
    //bcCharging->postToCDR(); // bala for testing
    if(!bcCharging){
        logerror(bsOpr->getUuid(),"ChargingService::loadChargingApp() charging Object creation error");
        bsOpr->updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_UNKNOWN,"charging Object creation error",500);
        return postServiceEvent(bsOpr);
    }
    retCode retVal = bcCharging->InitCharingAccount(m_dbConnections,bsOpr->getInParam(),bsOpr->m_app);
    int err = bcCharging->getEquivalentSipError(retVal);
    switch(retVal){
        case retCode::BS_NO_SIGNALIP:
            loginfo(bsOpr->getUuid(),"ChargingService::loadChargingApp() BS_NO_SIGNALIP -  OPE_MOMT_SPAWNTASK %s", bsOpr->getLastKnownError().c_str() );
            bsOpr->updateExecutionStatus(BSCallState::STATE_PROCESSING,OPERATIONEVENT::OPE_MOMT_SPAWNTASK,"No signal IP Check ONNET Call procedure",err);
            insertchargingInterface(bcCharging);
            bsOpr->createBSTask(bcCharging->getAppKey(), bcCharging->getAppType());
        break;
        case retCode::BS_SUCCESS: 
            loginfo(bsOpr->getUuid(),"ChargingService::loadChargingApp() BS_SUCCESS -  OPE_BSP_DIAL %s", bsOpr->getLastKnownError().c_str() );
            bsOpr->updateExecutionStatus(BSCallState::STATE_DIALING,OPERATIONEVENT::OPE_BSP_DIAL,"Dial Request",err);
            insertchargingInterface(bcCharging);
            bsOpr->createBSTask(bcCharging->getAppKey(), bcCharging->getAppType());
        break;
        default:
            logerror(bsOpr->getUuid(),"ChargingService::loadChargingApp() %d - OPE_UNKNOWN %s", retVal, bsOpr->getLastKnownError().c_str());
            bsOpr->updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_UNKNOWN,bsOpr->getLastKnownError(),err);
            if(bcCharging) {delete bcCharging; bcCharging =  nullptr;}
        break;
    }
    return postServiceEvent(bsOpr);
}
bool ChargingService::OnCallConnect(BSOperation* bsOpr){
    if(!bsOpr){
        return false;
    }
    loginfo(bsOpr->getUuid(),"CharingService::OnCallConnect()");
    BSCharging* bcCharging = (BSCharging*)getChargingInterface(bsOpr->getTaskKey());
    if(!bcCharging){
        logerror(bsOpr->getUuid(),"CharingService::OnCallConnect() Charing Object is NULL");
        bsOpr->updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_UNKNOWN,"Charging Object is NULL",500);
        return postServiceEvent(bsOpr);
    }
    retCode ret =  bcCharging->StartCharging(m_dbConnections,bsOpr->getInParam());
    
    if(ret == retCode::BS_SUCCESS){
        loginfo(bsOpr->getUuid(),"CharingService::OnCallConnect() Next Charing Cycle at %d", bcCharging->m_charDt.m_nextChargingTime);
        bsOpr->updateExecutionStatus(BSCallState::STATE_CONNECTED,OPERATIONEVENT::OPE_TASK_NEXT,"Success Executed Charging Call connected",200);
        BusinessServer::setTimerEvent(bcCharging->m_charDt.m_nextChargingTime,(int)BS_TIMER_EVENT::BS_CALL_TIMECHARGING,bsOpr->getUuid());
        return postServiceEvent(bsOpr);
    }    
   return false;
}
bool ChargingService::OnCallCycleEvent(BSOperation* bsOpr){
    
    if(!bsOpr){
        return false;
    }
    loginfo(bsOpr->getUuid(),"CharingService::OnCallCycleEvent()");
    BSCharging* bcCharging = (BSCharging*)getChargingInterface(bsOpr->getTaskKey());
    if(!bcCharging){
        logerror(bsOpr->getUuid(),"ChargingService::OnCallCycleEvent Charging Object not found App Key %s",bsOpr->getTaskKey().c_str() );
        bsOpr->updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_TASK_NEXT,"Charging Object is NULL",500);
        return postServiceEvent(bsOpr);
    }
    retCode ret = bcCharging->CharingCycle(m_dbConnections,bsOpr->getInParam());
    int err = bcCharging->getEquivalentSipError(ret);
    switch(ret){
        case retCode::BS_TIME_EXCEED:
            loginfo(bsOpr->getUuid(),"CharingService::OnCallCycleEvent retCode::BS_TIME_EXCEED" );
            bsOpr->updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_TASK_NEXT, "Max time exceed",err);
        break;
        case retCode::BS_NO_BUNDLE_BALANCE:
        case retCode::BS_NO_MASTER_BALANCE:
            loginfo(bsOpr->getUuid(),"CharingService::OnCallCycleEvent No Balance" );
            bsOpr->updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_TASK_NEXT, "No Balance",err);
        break;
        case retCode::BS_SUCCESS:
            loginfo(bsOpr->getUuid(),"CharingService::OnCallCycleEvent Next Charing Cycle at %d ",bcCharging->m_charDt.m_nextChargingTime );
            BusinessServer::setTimerEvent(bcCharging->m_charDt.m_nextChargingTime,(int)BS_TIMER_EVENT::BS_CALL_TIMECHARGING,bsOpr->getUuid());
            bsOpr->updateExecutionStatus(BSCallState::STATE_CONNECTED,OPERATIONEVENT::OPE_TASK_NEXT,"",err);
        break;
        default:
            logerror(bsOpr->getUuid(),"CharingService::OnCallCycleEvent other ret vale  %d", ret );
            bsOpr->updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_TASK_NEXT, "Error On Charing Cycle",err);
        break;
    }
    return postServiceEvent(bsOpr);
}
bool ChargingService::OnCallDisconnect(BSOperation* bsOpr){

    if(!bsOpr){

        return false;
    }
    BSCharging* bcCharging = (BSCharging*)getChargingInterface(bsOpr->getTaskKey());
    if(!bcCharging){
        logerror(bsOpr->getUuid(),"ChargingService::OnCallDisconnect Charging Object not found App Key %s",bsOpr->getTaskKey().c_str() );
        return false;
    }
    retCode ret = bcCharging->StopCharging(m_dbConnections,bsOpr->getInParam(),bsOpr->getConnectedTime(),bsOpr->getDisconnectedTime(),!bsOpr->getCallConnectedStatus());

    if(ret == retCode::BS_SUCCESS){
        loginfo(bsOpr->getUuid(),"ChargingService::OnCallDisconnect executed success");
        bsOpr->updateExecutionStatus(BSCallState::STATE_DISCONNECTED,OPERATIONEVENT::OPE_TASK_NEXT,"Success Executed Call Disconnected",200);     
    }else{
        loginfo(bsOpr->getUuid(),"ChargingService::OnCallDisconnect executed with error %s", bsOpr->getLastKnownError().c_str());
        bsOpr->updateExecutionStatus(BSCallState::STATE_DISCONNECTED,OPERATIONEVENT::OPE_TASK_NEXT,bsOpr->getLastKnownError(),200);     
    }
    
    deleteChargingInterface(bsOpr->getTaskKey());
    delete bcCharging;
    bcCharging =  nullptr;
    bcCharging->postToCDR();
  //  return postServiceEvent(bsOpr);
    return postServiceEvent(bsOpr);
}
bool ChargingService::postServiceEvent(BSOperation* bsOpr){

    if(!bsOpr){
        logerror("", "ChargingService::postServiceEvent Operation Object is NULL");
        return false; 
    }
    logdebug(bsOpr->getUuid(),"ChargingService::postServiceEvent Task ID %d App Key %s Last Known Message %s ",
                                bsOpr->getTaskID(), bsOpr->getTaskKey().c_str(),bsOpr->getLastKnownError().c_str());  
    std::unique_ptr<BSOperationMessage> pMsg (new BSOperationMessage );
    if(!pMsg){
        logerror(bsOpr->getUuid(),"ChargingService::postServiceEvent Memory Error On Operation Message Object Creation");
        return false;
    }
    
    BUSINESS_THREADS threadID = BUSINESS_THREADS::UNKNOWN;
    switch(bsOpr->getOperationEvent()){
        case OPERATIONEVENT::OPE_MOMT_SPAWNTASK:
            pMsg->setMsgType(MESSAGEEVENT::MSG_CHARG2MOMT);
            threadID = BUSINESS_THREADS::MVNO_BSMOMT;
        break;
        case OPERATIONEVENT::OPE_TASK_NEXT:
        switch(bsOpr->getNextTask()){
            case APPLICATIONTYPE::TYPE_GMSC:
                pMsg->setMsgType(MESSAGEEVENT::MSG_CHARG2GMSC);
                threadID = BUSINESS_THREADS::MVNO_GMSC;
            break;
            default:
                pMsg->setMsgType(MESSAGEEVENT::MSG_CHARG2BSP);
                threadID = BUSINESS_THREADS::MVNO_BSP;
            break;
        }
        break;
        case OPERATIONEVENT::OPE_BSP_DIAL:
            pMsg->setMsgType(MESSAGEEVENT::MSG_CHARG2BSP);
            threadID = BUSINESS_THREADS::MVNO_BSP;
        default:
            pMsg->setMsgType(MESSAGEEVENT::MSG_CHARG2BSP);
            threadID = BUSINESS_THREADS::MVNO_BSP;
        break;
    }
    
    pMsg->setEvent((int)bsOpr->getOperationEvent());
    std::unique_ptr<BSOperation> tempOperation (bsOpr);
    pMsg->m_bsOperation =  std::move(tempOperation);
    return  BusinessServer::post(threadID,std::move(pMsg));
}



bool ChargingService::insertchargingInterface(BSInterface* bsInt){
    if(!bsInt)
        return false;
    m_chargingList.InsertToMap(bsInt->generateAppKey(),bsInt);
    return true;

}

BSInterface* ChargingService::getChargingInterface(std::string appKey){
    if(appKey.empty())
        return nullptr;
    BSInterface* bsInt =  m_chargingList.FindValue(appKey);
    return bsInt;
}

bool ChargingService::deleteChargingInterface(std::string appKey){
    if(appKey.empty())
        return false;
    return m_chargingList.DeleteFromMap(appKey);
}
