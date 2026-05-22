#include "BSmomtGatewayService.h"
#include "BusinessServer.h"
#include "LogHandler.h"
/* loop function to keep service running*/
bool BSmomtGatewayService::run(){
    const std::string thread_name = "momtGatewaySer";
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
            if(pMsg){delete pMsg; pMsg =  nullptr;}
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
return true;
}
/*event handling function from other services*/
bool BSmomtGatewayService::handleMsg(CA::CAMessageBlock* pMsg){
    if(!pMsg){
        logerror("","BSmomtGatewayService::handleMsg Message Block Object is NULL" );
        return false;
    }
    
     unsigned long msgType = pMsg->getMsgType();
     switch(msgType){
        case MESSAGEEVENT::MSG_INIT:
            loginfo("","BSmomtGatewayService::handleMsg MESSAGEEVENT::MSG_INIT" );
            init(pMsg);
        break;
        case MESSAGEEVENT::MSG_BSP2MOMT:
            loginfo("","BSmomtGatewayService::handleMsg MESSAGEEVENT::MSG_BSP2MOMT" );
            processBSPRequest(pMsg);
        break;
        case MESSAGEEVENT::MSG_CHARG2MOMT:
            loginfo("","BSmomtGatewayService::handleMsg MESSAGEEVENT::MSG_CHARG2MOMT" );
            processRouteRequest(pMsg);
        break;
        case MESSAGEEVENT::MSG_GMSC2MOMT:
            loginfo("","BSmomtGatewayService::handleMsg MESSAGEEVENT::MSG_GMSC2MOMT" );
            processRouteRequest(pMsg);
        break;
        default:
            loginfo("","BSmomtGatewayService::handleMsg MESSAGEEVENT %d unkwon Message Event",msgType);
        break;
     }
    
    return true;
}
bool BSmomtGatewayService::processBSPRequest(CA::CAMessageBlock* pMsg){
    if(!pMsg){
        logerror("","BSmomtGatewayService::processBSPRequest() Message Block is NULL");
        return false;
    }
    BSOperationMessage* bsOprMsg =  (BSOperationMessage*)pMsg;
    if(bsOprMsg->m_bsOperation == nullptr){
        logerror("","BSmomtGatewayService::processBSPRequest() Operation Object is NULL");
        return false;
    }
    switch((OPERATIONEVENT)pMsg->getEvent()){
        case OPERATIONEVENT::OPE_TASK_INIT:
            loginfo(bsOprMsg->m_bsOperation->getUuid(),"BSmomtGatewayService::processBSPRequest() OPERATIONEVENT::OPE_TASK_INIT");
            callInRoute(bsOprMsg->m_bsOperation.release()); 
        break;
        default:
        break;


    }
    return false;
}
bool BSmomtGatewayService::processRouteRequest(CA::CAMessageBlock* pMsg){
    if(!pMsg){
        logerror("","BSmomtGatewayService::processRouteRequest Message Block is NULL" );
        return false;
    }
    BSOperationMessage* routOprMsg =  (BSOperationMessage*)pMsg;
    if(routOprMsg->m_bsOperation == nullptr){
        logerror("","BSmomtGatewayService::processRouteRequest Operation Object is NULL" );
        return false;
    }
    bool retVal;
    switch((OPERATIONEVENT)routOprMsg->getEvent()){
        
        case OPERATIONEVENT::OPE_MOMT_SPAWNTASK: // spawn next application(create new app)
            loginfo(routOprMsg->m_bsOperation->getUuid(),"BSmomtGatewayService::processRouteRequest() OPERATIONEVENT::OPE_MOMT_SPAWNTASK");
            retVal = callNextRoute(routOprMsg->m_bsOperation.release());
        break;
        default:
             loginfo(routOprMsg->m_bsOperation->getUuid(),"BSmomtGatewayService::processRouteRequest() unkown operation event %d", routOprMsg->getEvent());
             retVal =  false;
        break;
        
        
    }
return retVal;
}
/* start funtion to inilizing the member variable and get requerd connections*/
bool BSmomtGatewayService::init(CA::CAMessageBlock* pMsg){
    loginfo("","BSmomtGatewayService::init()");
    BSConfigMsgBlock *pConfMsgBlock =  (BSConfigMsgBlock *)pMsg;
    if(pConfMsgBlock){
        std::unique_ptr<BusinessConfig> pconf (std::move(pConfMsgBlock->m_configDetails));
        m_pBSConfiguration =  std::move(pconf);
        return cliProfileDBConnect();
        //return true;
    }
    return false;
}
/*inital DB connection for cli check functionality*/
bool BSmomtGatewayService::cliProfileDBConnect(){
    loginfo("","BSmomtGatewayService::cliProfileDBConnect()");
    if(m_pBSConfiguration){
        for(CA::CAMap<Trunk*>::ca_itr it = m_pBSConfiguration->getTrunkList()->Begin() ; it != m_pBSConfiguration->getTrunkList()->End(); it++){
            Trunk* trk =  it->second;
            if(trk){
                DBProfile* dbProfile = m_pBSConfiguration->getDBProfile(trk->getCliDBProfile());
                std::cout << trk->getTrunkName() << " --" << trk->getCliDBProfile() << std::endl;
                if(dbProfile){
                    CA::DBInterface *dbInterface =  nullptr;
                     if(dbProfile->getDBType().compare("mssql") == 0){
                        dbInterface = new CA::DBMSSql();
                        if(dbInterface)
                        dbInterface->setDbType("mssql");
                     }else if(dbProfile->getDBType().compare("mysql") == 0){
                        dbInterface = new CA::DBMSSql();
                        if(dbInterface)
                        dbInterface->setDbType("mysql");
                     }
                    if(dbInterface){
                        if(dbInterface->openConnection(dbProfile->getConnectionString(),dbProfile->getConntetionTimeout()) !=  CA::CASQLRETURN::CASQL_FAIL){
                            std::cout << "Last Known Error "  << dbInterface << " " << dbInterface->getLastKnownError() << std::endl;
                            m_dbConnections.InsertToMap(trk->getTrunkName(),dbInterface);
                        } // DB connection open check
                    } // DB Interface check
                } // db profile check
            } //trunk object check
        } // for Loop
        return true; 
    }//configuration Object check
    return false;
}
std::string BSmomtGatewayService::getTrunkNameWithDdiPrefix(std::string strIncomingDDI){
loginfo("","BSmomtGatewayService::getTrunkNameWithDdiPrefix()");
std::list<prefix*>* ddiPrefix =  m_pBSConfiguration->getPrefixList();

    if(ddiPrefix){
        for(prefix* prf : *ddiPrefix ){
            if(prf){
                if(std::regex_match (strIncomingDDI.c_str(),std::regex(prf->getddiPrefix().c_str()))){
                    //std::cout  << "Trunk  Name " << prf->gettrunkName() << std::endl;
                    return prf->gettrunkName();
                }
            }
        }
    }
    
    return "";
}
bool BSmomtGatewayService::getProfiles(std::string trunkName, BSOperation* bsopr){
    loginfo("","BSmomtGatewayService::getProfiles()");
    CA::CAMap<Trunk*> *trunkList =  m_pBSConfiguration->getTrunkList();
    if(!trunkList){
        bsopr->updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_UNKNOWN,"Trunk List is empty",500);
        return false;
    }
    Trunk* trk = trunkList->FindValue(trunkName);
    if(!trk){
        bsopr->updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_UNKNOWN,"Trunk configuration not found",500);
        
        return false;
    }
        // Get Site Code 
    bsopr->getInParam()->setSiteCode(trk->getSiteCode());
        
    bsopr->getInParam()->setSwitchCode(std::stoi(m_pBSConfiguration->getSwitchCode()));
    bsopr->getInParam()->setIwmscPrefix(trk->getIwmscPrefix());
    bsopr->getInParam()->setMvnoPrefix(trk->getMvnoPrefix());
    bsopr->getInParam()->setHlrUrl(trk->getHrlUrl());
    
    CA::CAMap<Application*>* appList = m_pBSConfiguration->getApplicationList();
    if(!appList){
        bsopr->updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_UNKNOWN,"Applist is empty",500);
        return false;
    }
    Application* app = appList->FindValue(trk->getTaskName());
    if(!app){
        bsopr->updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_UNKNOWN,"App configuration not found",500);
        return false;
    }
    bsopr->m_app.setEspDB(app->getEspDB()) ;
    bsopr->m_app.setEspParamDB(app->getEspParamDB());
    bsopr->m_app.setRoutingDB(app->getRoutingDB());
    bsopr->m_app.setHlrDB(app->getHlrDB());
    bsopr->m_app.setMnpDB(app->getMnpDB());
    bsopr->m_app.setSwitchDB(app->getSwitchDB());
    bsopr->m_app.setTaskName(app->getTaskName());
    bsopr->m_app.setAppName(app->getAppName());
    return true;
}
int BSmomtGatewayService::db_mvno_check_home_subscriber(CA::DBInterface *dbInt, BSOperation* bsopr ){
    loginfo("","BSmomtGatewayService::db_mvno_check_home_subscriber()");
    if(!bsopr)
        return -1;
    if(!bsopr->getInParam())
        return -1;
    
    std::string strIncomingCLI =  bsopr->getInParam()->getCli();

    if(!dbInt || strIncomingCLI.empty())
    return -1;

    logdebug(bsopr->getInParam()->getUid(), "BSmomtGatewayService::db_mvno_check_home_subscriber()");
    BSInterface *bsInterface = new BSInterface();
    std::string spName = "mvno_check_home_subscriber";
    std::string spValue = "'" + strIncomingCLI + "'";
    CA::DBMAP dbmap;
    retCode ret = bsInterface->executeQuery(dbInt,spName,spValue,dbmap);

    logdebug(bsopr->getInParam()->getUid(),"BSmomtGatewayService::db_mvno_check_home_subscriber() DataBase: %s, Query %s",bsInterface->getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    
    if(ret == retCode::BS_SUCCESS){
        if(dbmap.size() > 0){
            int nSub = 0;
            bsInterface->getData("is_home_subscriber",0,dbmap,nSub);
            bsInterface->ClearDBData(dbmap);
            return nSub;
        }else{

            return 0;
        }
    }
    bsInterface->ClearDBData(dbmap);
    return -1;
}

bool BSmomtGatewayService::spawnApplication(BSOperation* pbsOpr){
    if(!pbsOpr){
        logerror("", "BSmomtGatewayService::spawnApplication Operatin Object is NULL");
        return false;
    }
    loginfo(pbsOpr->getUuid(),"BSmomtGatewayService::spawnApplication()");
    std::string trunkName = getTrunkNameWithDdiPrefix( pbsOpr->getInParam()->getDdi());
	    if(trunkName.empty()){
        pbsOpr->updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_UNKNOWN,"Trunk Name found empty",404);
        return postServiceEvent(pbsOpr);
    }
    pbsOpr->getInParam()->setTrunkName(trunkName);

    CA::DBInterface *dbInterface =  m_dbConnections.FindValue(trunkName);
    if(!dbInterface){
    pbsOpr->updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_UNKNOWN,"DB Connection Failure or DB Obj not found in list",500);
    return postServiceEvent(pbsOpr);
   }
   getProfiles(trunkName,pbsOpr);
   loginfo(pbsOpr->getUuid(),"BSmomtGatewayService::spawnApplication() CLI : %s DDI %s Trunk Name : %s  Spawn App %s",pbsOpr->getInParam()->getCli().c_str(),pbsOpr->getInParam()->getDdi().c_str(),trunkName.c_str(), pbsOpr->m_app.getAppName().c_str() );
   
   
    int subState = this->db_mvno_check_home_subscriber(dbInterface,pbsOpr);
    switch(subState){
    case -1:
        loginfo(pbsOpr->getUuid(),"BSmomtGatewayService::spawnApplication() DB Connectin Failure Terminate %s",trunkName.c_str());
        pbsOpr->updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_UNKNOWN,"Subscriber DB connection failure or internal failure",500);
        
    break;
    case 1: // Since the DDI is Vectone Number spawn GMSC
        loginfo(pbsOpr->getUuid(),"BSmomtGatewayService::spawnApplication() Load GMSC Task Trunk Name %s",trunkName.c_str());
        pbsOpr->updateExecutionStatus(BSCallState::STATE_PROCESSING,OPERATIONEVENT::OPE_GMSC_INIT,"",200);
        
    break;
    default:
    // check the ddi prefix for VMD & call fwd
        loginfo(pbsOpr->getUuid(),"BSmomtGatewayService::spawnApplication() Task Unkown Trunk Name %s",trunkName.c_str());
        pbsOpr->updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_UNKNOWN,"Route No Found",404);
    break;
    }
    return postServiceEvent(pbsOpr);
}
/*choosing the service for spawing on call setup Indication */
bool BSmomtGatewayService::callInRoute(BSOperation* pbsOpr){
    if(!pbsOpr){
        logerror("", "BSmomtGatewayService::callInRoute Operatin Object is NULL");
        return false;
    }
    loginfo(pbsOpr->getUuid(),"BSmomtGatewayService::callInRoute()");
    // get the trunk Name from DDI 
     std::string trunkName = getTrunkNameWithDdiPrefix(  pbsOpr->getInParam()->getDdi());
    if(trunkName.empty()){
        pbsOpr->updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_UNKNOWN,"Trunk Name found empty",404);
        return postServiceEvent(pbsOpr);
    }
    
    pbsOpr->getInParam()->setTrunkName(trunkName);
    getProfiles(trunkName,pbsOpr);
    if(pbsOpr->getInParam()->getTrace().empty())
    {
        pbsOpr->getInParam()->setTrace(pbsOpr->getInParam()->getCli()+":"+pbsOpr->getInParam()->getDdi()+":"+pbsOpr->getInParam()->getTrunkName());
    }

    // using trunk Name fetch the cli DB connection
   CA::DBInterface *dbInterface =  m_dbConnections.FindValue(trunkName);

   if(!dbInterface){
        pbsOpr->updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_UNKNOWN,"DB Connection Failure or DB Obj not found in list",500);
        return postServiceEvent(pbsOpr);
   }
    // Query cli for subscribtion check
    int subState = db_mvno_check_home_subscriber(dbInterface,pbsOpr);
    switch(subState){
        case -1:
            loginfo(pbsOpr->getUuid(),"BSmomtGatewayService::callInRoute() DB Connectin Failure Terminate %s",trunkName.c_str());
            pbsOpr->updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_UNKNOWN,"Subscriber DB connection failure or internal failure",500);
        break;
        case 0:
            loginfo(pbsOpr->getUuid(),"BSmomtGatewayService::callInRoute() Load GMSC Trunk Name  %s",trunkName.c_str());
            pbsOpr->updateExecutionStatus(BSCallState::STATE_PROCESSING,OPERATIONEVENT::OPE_GMSC_INIT,"Success Executed InRoute to GMSC Servic",200);
        break;
        case 1:
            loginfo(pbsOpr->getUuid(),"BSmomtGatewayService::callInRoute() Load Charging Trunk Name  %s",trunkName.c_str());
            pbsOpr->updateExecutionStatus(BSCallState::STATE_PROCESSING,OPERATIONEVENT::OPE_CHARING_INIT,"Success Executed InRoute to Charging Servic",200);
        break;
        default:
            loginfo(pbsOpr->getUuid(),"BSmomtGatewayService::callInRoute() Home Subscriber executing error  %s",trunkName.c_str());
            pbsOpr->updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_UNKNOWN," home Subscriber DB query Unkown Error",500);
        break;
    }
    return postServiceEvent(pbsOpr);
}
bool BSmomtGatewayService::callNextRoute(BSOperation * bsp){
    if(!bsp){
        logerror("", "BSmomtGatewayService::callNextRoute Operatin Object is NULL");
        return false;
    }
    loginfo(bsp->getUuid(),"BSmomtGatewayService::callNextRoute() Opertion Event %d", bsp->getOperationEvent());
    if(bsp->checkNextAvaiableTask() == false ) {
        // create new application to spwan 
        switch(bsp->getOperationEvent()){
            case OPERATIONEVENT::OPE_MOMT_SPAWNTASK:{
                loginfo(bsp->getUuid(),"BSmomtGatewayService::callNextRoute() CLI : %s DDI: %s Trunk Name: %s",bsp->getInParam()->getCli().c_str(), bsp->getInParam()->getDdi().c_str(),bsp->getInParam()->getTrunkName().c_str());
                return spawnApplication(bsp);
            break;
            }
        }

    }
    return false;
}
MESSAGEEVENT BSmomtGatewayService::getThreadMessageType(OPERATIONEVENT oprEvt){
    loginfo("","BSmomtGatewayService::getThreadMessageType()");
    MESSAGEEVENT msgType = MESSAGEEVENT::MSG_ERROR;
    switch(oprEvt){ 
        case OPERATIONEVENT::OPE_CHARING_INIT:
            msgType = MESSAGEEVENT::MSG_MOMT2CHARG;
        break;
        case OPERATIONEVENT::OPE_GMSC_INIT:
            msgType = MESSAGEEVENT::MSG_MOMT2GMSC;
        break;
        default:
        msgType = MESSAGEEVENT::MSG_MOMT2BSP;
        break;
    }
    return msgType;
}
bool BSmomtGatewayService::postServiceEvent(BSOperation* pbsOp){
    loginfo("","BSmomtGatewayService::postServiceEvent()");
    if(!pbsOp){
        loginfo("","BSmomtGatewayService::postServiceEvent() Operation is NULL");
        return false;
    }
    std::unique_ptr<BSOperationMessage> pMsg (new BSOperationMessage );
    if(!pMsg){
        // send to exception
        loginfo("","BSmomtGatewayService::postServiceEvent() Operation Message creatin failed");
        return false; 
    }
    
    MESSAGEEVENT threadType = getThreadMessageType(pbsOp->getOperationEvent());
    
    if(pbsOp->getSipErrorCode()  == 200){
        loginfo(pbsOp->getInParam()->getUid(),"BSmomtGatewayService::postServiceEvent %d", threadType  );
    }else{
        logerror(pbsOp->getInParam()->getUid(),"BSmomtGatewayService::postServiceEvent %d Error %s", threadType, pbsOp->getLastKnownError().c_str() );
    }
    
    std::unique_ptr<BSOperation> tempOperation (pbsOp);
    if(!tempOperation)
        return false;
    pMsg->m_bsOperation =  std::move(tempOperation);
    pMsg->setMsgType(threadType);
    pMsg->setEvent((unsigned long )pMsg->m_bsOperation->getOperationEvent());
    BUSINESS_THREADS threadID;
    switch (threadType){
        case MESSAGEEVENT::MSG_MOMT2GMSC:
        threadID = BUSINESS_THREADS::MVNO_GMSC;
        break;
        case MESSAGEEVENT::MSG_MOMT2CHARG:
        threadID =  BUSINESS_THREADS::MVNO_CHARGE;
        break;
        default:
        threadID = BUSINESS_THREADS::MVNO_BSP;
        break;
    }
    return  BusinessServer::post(threadID,std::move(pMsg));
    
}
