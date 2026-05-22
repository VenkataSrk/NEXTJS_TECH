#include "BusinessConfigService.h"
#include "BusinessServer.h"
#include <regex>
#include "LogHandler.h"

/*
*/
BusinessConfigService::BusinessConfigService(){
    
m_configHandler.LoadConfiguration();
}
/*
*/
BusinessConfigService::~BusinessConfigService()
{
   
}

/* Method Name: init
 ** Description: To initialize config interface
 ** Parameters: nil
 ** return: boolean(true|false)
 **/
bool BusinessConfigService::init()
{
   loginfo("","CAConfigHandler::init ()");
   StartBusinessServices((int)BS_SERVICES::BS_REQ_PROCESS);
   StartBusinessServices((int)BS_SERVICES::BS_MVNO_SOCKET);
   StartBusinessServices((int)BS_SERVICES::BS_LOCAL_DB_PROCESS);
   /********* Commented by Dayana: Currently, we are not using the Monitor process ***********/
   //StartBusinessServices((int)BS_SERVICES::BS_MONITOR_PROCESS);
   #if 0
   std::string fileName =  "../conf.xml";
   cafNote =  new CAFileNotifier(fileName,NOTIFIER_EVENT::NE_ONLYFILEMODIFY,(CA::CAThread*)this,(int)MESSAGEEVENT::MSG_CONFUPDATE);
    if(cafNote){
      cafNote->startNotification();
   }
   #endif
   return true;
}

/* Method Name: run
 ** Description: To run event loop for receiving messages
 ** Parameters: nil
 ** return: boolean(true|false)
 **/
bool BusinessConfigService::run()
{
   //ConfigObject* pConfigObj = nullptr;
   const std::string thread_name = "BSConfigSer";
   int rc = pthread_setname_np(pthread_self(),thread_name.c_str());
   if(rc != 0)
   {
     std::cerr << "Failed to set name for thread " << strerror(rc) << std::endl;
   }
   else{
     std::cout << "Thread name set succesfully to :" << thread_name << std::endl; 
   }

   while(getThreadStatus())
   {
      
       handle_msg((CA::CAMessageBlock*)RecvMessage());
       std::this_thread::sleep_for(std::chrono::milliseconds(100));
   }
   //Server::setThreadBitEnable(EN_CA_THREAD_NAME::CON,{0});
   
   return true;
}

/* Method Name: handle_msg
 ** Description: To handle message received and process it
 ** Parameters: CAConfigInterfaceMsg* pConfigMsg
 ** return: boolean(true|false)
 **/
bool BusinessConfigService::handle_msg(CA::CAMessageBlock* pMsg)
{
   if(!pMsg)
      return false;
   unsigned long msgType = pMsg->getMsgType();
   switch(msgType){
      case MESSAGEEVENT::MSG_INIT:
      init();
      break;
      case MESSAGEEVENT::MSG_CONFUPDATE: // file is modified update conf again
      #if 1
         
         reLoadConfig();
         sleep(1);
         cafNote->startNotification();
      #endif
      break;
      case MESSAGEEVENT::MSG_SHUTDOWN:
      break;
      case MESSAGEEVENT::MSG_MOMT2CONF: // Reloading the configuration 
      break;
      case MESSAGEEVENT::MSG_BS2CONF: // Load Global Setting 
      break;
      case MESSAGEEVENT::MSG_BSP2CONF: // load call settings
      break;
      case MESSAGEEVENT::MSG_REP2CONF:
      {
         BusinessServer::getBSReportingService()->init(m_configHandler.getConfigObj()->getCdrPath());
         break;
      }
      
   }
   if(pMsg){ delete pMsg; pMsg = nullptr;}
  
   return true;
}
bool BusinessConfigService::LoadBusinessProcessor(){
   std::unique_ptr<BSRequestProcessor> pBsp (new BSRequestProcessor );
   pBsp->setThreadStatus(true);
   if(pBsp->spwan(*pBsp)){
      BusinessServer::setBusinessProcessor(pBsp.release());
      std::unique_ptr<BSConfigMsgBlock> pMsg (new BSConfigMsgBlock());
      pMsg->m_configDetails = std::move(m_configHandler.fetchConfigObject());
      pMsg->setMsgType(MESSAGEEVENT::MSG_INIT);
      BusinessServer::post(BUSINESS_THREADS::MVNO_BSP,std::move(pMsg));
      return true;
   }
   return false;
}
bool BusinessConfigService::LoadMVNOService(){
   
   std::unique_ptr<MVNOBusinessService> pBus (new MVNOBusinessService);
	pBus->setThreadStatus(true);
	if(pBus->spwan(*pBus)){
		BusinessServer::setBusinessService(pBus.release());
      std::unique_ptr<BSGlobalConfObject> confObject (new BSGlobalConfObject());
      confObject->m_bsListenPort = m_configHandler.getServerPort();
      confObject->m_callTimeOut =  m_configHandler.getCallTimeOut();
      confObject->setMsgType(MESSAGEEVENT::MSG_CONF2BS);
      BusinessServer::post(BUSINESS_THREADS::MVNO_BUS,std::move(confObject)); 
		
	}
   return true;
}



bool BusinessConfigService::loadDBList(CA::CAMessageBlock* pMsg){
   std::unique_ptr<BSDBProfileObject> dbList (new BSDBProfileObject);
   dbList->copyDBProfilList(m_configHandler.getConfigObj()->getDBProfileList());
	dbList->setMsgType(MESSAGEEVENT::MSG_INIT);
	BusinessServer::post(BUSINESS_THREADS::MVNO_BSDB,std::move(dbList));
   return true;
   
}
bool BusinessConfigService::StartBusinessServices(int bsService){

   switch((BS_SERVICES)bsService){
      
      case BS_SERVICES::BS_REQ_PROCESS:
         LoadBusinessProcessor();
      break;
      case BS_SERVICES::BS_MVNO_SOCKET:
         LoadMVNOService();
      break;
      case BS_SERVICES::BS_LOCAL_DB_PROCESS:
         LoadLocalDBService();
      break;
      case BS_SERVICES::BS_NONE:
      default:
      break;
   }
   return true;
}

bool BusinessConfigService::LoadLocalDBService(){
   std::unique_ptr<BSLocalDBService> pLocalDBService (new BSLocalDBService);
   pLocalDBService->setThreadStatus(true);
   if(pLocalDBService->spwan(*pLocalDBService)){
      BusinessServer::setBSLocalDBService(pLocalDBService.release());
      BusinessServer::getBSLocalDBService()->init(m_configHandler.getConfigObj()->getLocalDBType(), m_configHandler.getConfigObj()->getConnString(), m_configHandler.getConfigObj()->getUserId(), m_configHandler.getConfigObj()->getPassword());
   }
   return true;
}

bool BusinessConfigService::reLoadConfig(){
    
    m_configHandler.reLoadConfiguraton();
    std::unique_ptr<BusinessConfig> pBusConf(new BusinessConfig());
    if(pBusConf){
      pBusConf->copyConfig(*m_configHandler.getConfigObj());
      std::unique_ptr<BSConfigMsgBlock> pMsg (new BSConfigMsgBlock());
      if(pMsg){
         //pMsg->m_configDetails = std::move(m_configHandler.fetchConfigObject());
         pMsg->m_configDetails =  std::move(pBusConf);
         pMsg->setMsgType(MESSAGEEVENT::MSG_CONFUPDATE);
         BusinessServer::post(BUSINESS_THREADS::MVNO_BSP,std::move(pMsg));
      }
    }
   return true;
}

#if 0 // this code snippnet not required this has to be automate in MOMT service Yadav
bool BusinessConfigService::loadCallConf(CA::CAMessageBlock* pMsg){
   if(!pMsg){
      // send error to exception and disconnect call.(500 internal error)
      return false;
   }
   BSOperationMessage*  bsOpr = (BSOperationMessage*)pMsg;
   BSParam* bsParam ;
   if(bsOpr && bsOpr->m_bsOperation ){

      bsParam = bsOpr->m_bsOperation->getInParam();
      
   } else { return false;}
   if(bsParam){
   
      std::unique_ptr<BSCallConfObject> confObject (new BSCallConfObject());
      if(confObject == nullptr){
         return false;
      }
      confObject->m_errorCode =  BSERRDEF::BSError::NOERROR;
      Trunk* trk = m_configHandler.getConfigObj()->getTrunkInfo(bsParam->getCliAddr());
      if(trk){ 
         confObject->m_trunkName = trk->getTrunkName();
         confObject->m_maxChannel = trk->getMaxChannel();
         confObject->m_siteCode =  trk->getSiteCode();
         confObject->m_uid = bsParam->getUid();
         confObject->m_cli =  bsParam->getCli();
         confObject->m_ddi =  bsParam->getDdi();
         confObject->m_url =  bsParam->getCliAddr();
         confObject->m_switchCode =  m_configHandler.getConfigObj()->getSwitchCode();
         std::list<std::string>* m_grpList =  trk->getGroupList();
         if(m_grpList && m_grpList->size() > 0){
            std::unique_ptr<CA::CAMap<SPAWNTASKINFO*>> tempspawnTask(new CA::CAMap<SPAWNTASKINFO*>);
            confObject->m_spawnTaskInfo =  std::move(tempspawnTask);
            for(std::list<std::string>::iterator it = m_grpList->begin() ; it != m_grpList->end(); it++){
               std::string tgrpName =  *it;
               Group* grp = m_configHandler.getConfigObj()->getGroupInfo(*it);
               if(grp){
                  if(std::regex_match (bsParam->getCli().c_str(),std::regex(grp->getCli().c_str()))){
                     if(std::regex_match(bsParam->getDdi().c_str(),std::regex(grp->getDdi().c_str()))){
                        confObject->m_groupName =  *it;
                        std::unique_ptr<CA::CAVector<std::string>> tempTaskName(new CA::CAVector<std::string>);
                        if(tempTaskName){
                          confObject->m_taskName =  std::move(tempTaskName) ;
                        }
                        std::list<std::string>* spawntaskList = grp->getSpawnTaskList();
                        if(spawntaskList && spawntaskList->size() > 0){
                           for(std::list<std::string>::iterator itr = spawntaskList->begin() ;itr != spawntaskList->end(); itr++){
                              std::string taskName = *itr;
                              if(!taskName.empty()){
                                 confObject->m_taskName->PushBack(taskName);
                              }
                              
                              Application* apps = m_configHandler.getConfigObj()->getApplicationInfo(taskName);
                              if(apps){
                                 SPAWNTASKINFO* appInfo =  new SPAWNTASKINFO;
                                 
                                 if(appInfo){
                                    appInfo->taskName =  taskName;
                                    appInfo->appName = apps->getAppName();
                                    appInfo->espdb  =  apps->getEspDB();
                                    appInfo->espParamdb = apps->getEspParamDB();
                                    appInfo->hlrdb =  apps->getHlrDB();
                                    appInfo->npoprdb = apps->getMnpDB();
                                    appInfo->routingdb = apps->getRoutingDB();
                                    appInfo->switchdb =  apps->getSwitchDB();
                                    //confObject->m_spawnTaskInfo.push_back(appInfo);
                                    confObject->m_spawnTaskInfo->InsertToMap(appInfo->taskName,appInfo);
                                 }
                              }
                           }
                           confObject->m_errorCode = (confObject->m_spawnTaskInfo->GetMapSize() == 0 && confObject->m_errorCode == BSERRDEF::BSError::NOERROR ) ? BSERRDEF::BSError::CONFIG_NOAPP : confObject->m_errorCode;
                        }else{confObject->m_errorCode = BSERRDEF::BSError::CONFIG_NOAPP;break;}
                        break;
                     }
                  }
               }
            }            
            confObject->m_errorCode = (confObject->m_spawnTaskInfo->GetMapSize() == 0 && confObject->m_errorCode == BSERRDEF::BSError::NOERROR ) ? BSERRDEF::BSError::CONFIG_NOGROUPS : confObject->m_errorCode;
         }else{confObject->m_errorCode = BSERRDEF::BSError::CONFIG_NOGROUP;}
      }else {confObject->m_errorCode =  BSERRDEF::BSError::CONFIG_NOTRUNK; }
      
      if(confObject->m_errorCode == BSERRDEF::BSError::NOERROR){

         bsOpr->m_bsOperation->updateConfig(confObject.release());
         CA::CAMessageBlock* ptrMsg = bsOpr->m_bsOperation->spawnTask();
         //ptrMsg->setMsgType(MESSAGEEVENT::MSG_CONF2CHARG);
         postMessage(ptrMsg);
         
      }else{
         bsOpr->m_bsOperation->updateConfig(confObject.release());
         BSOperationMessage* pOpCharging=  new BSOperationMessage();
         //std::unique_ptr<BSOperation> boperation();
         pOpCharging->m_bsOperation =  std::move(bsOpr->m_bsOperation);
         pOpCharging->setMsgType(MESSAGEEVENT::MSG_CONF2BSP);
         postMessage((CA::CAMessageBlock*)pOpCharging);
      }

   }
   
   
   return true;
}
#endif
#if 0 // since previous code is comment this functionality no more needed
bool BusinessConfigService::postMessage(CA::CAMessageBlock* pMsg){

   if(pMsg){
      std::unique_ptr<CA::CAMessageBlock> ptrMsgBlock(pMsg);
      switch(pMsg->getMsgType()){
         case MESSAGEEVENT::MSG_CONF2CHARG:
            CAlog_DEBUG("BusinessConfigService::postMessage() MESSAGEEVENT::MSG_CONF2CHARG");
            BusinessServer::post(BUSINESS_THREADS::MVNO_CHARGE, std::move(ptrMsgBlock));
        break;
        case MESSAGEEVENT::MSG_CONF2BSP:
        CAlog_DEBUG("BusinessConfigService::postMessage() MESSAGEEVENT::MSG_CONF2BSP");
         BusinessServer::post(BUSINESS_THREADS::MVNO_BSP, std::move(ptrMsgBlock));
        break;
        default:
        break;
    }
   }
}
#endif




   
