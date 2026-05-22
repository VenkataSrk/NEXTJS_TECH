#include "CAlog.h"
#include "BusinessServer.h"
#include "BSDBService.h"
DBService::DBService(){
    std::unique_ptr<BSDBProfileObject> dbList (new BSDBProfileObject);
    dbList->setMsgType(MESSAGEEVENT::MSG_BDB2CONF);
    BusinessServer::post(BUSINESS_THREADS::MVNO_CONF,std::move(dbList));

}
bool DBService::run(){
    const std::string thread_name = "DBService";
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
            pMsg = nullptr;
        }
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
     }
     return true;
}
bool DBService::handleMsg(CA::CAMessageBlock * pMsg){
    
    if(pMsg){
        unsigned long msgType = pMsg->getMsgType();
        switch(msgType){
            case MESSAGEEVENT::MSG_INIT:{
                BSDBProfileObject *dbList =  (BSDBProfileObject*)pMsg;
                createConnection(dbList) ;    
            }
            break;
            case MESSAGEEVENT::MSG_SHUTDOWN:
            break;
            case MESSAGEEVENT::MSG_CONF2BDB:{
            std::cout << "Call Info Request " << std::endl;
            BSCallConfObject* bsCallConfObject = (BSCallConfObject*)pMsg;
            requestDBConnectionHandles(bsCallConfObject);
            
            }
            break;
            default:
            break;
        }

    }
    return true;
}
bool DBService::createConnection(BSDBProfileObject *dbList){
    if(dbList){
        for(CA::CAMap<DBProfile*>::ca_itr it = dbList->m_dbProObject.Begin(); it != dbList->m_dbProObject.End(); it++){
            DBConProfile* dbCon = new DBConProfile ();
            if(dbCon){
                DBProfile* dbprofile =  it->second;
                if(dbprofile){
                    dbCon->m_dbDetails.setConnectionCount(dbprofile->getConnectionCount());
                    dbCon->m_dbDetails.setConnectionString(dbprofile->getConnectionString());
                    dbCon->m_dbDetails.setConnectionTimeout(dbprofile->getConntetionTimeout());
                    dbCon->m_dbDetails.setProfileName(dbprofile->getProfileName());
                    dbCon->m_dbDetails.setDBType(dbprofile->getDBType());
                }
                for(int count =  0 ; count < dbprofile->getConnectionCount(); count++)
                if(dbprofile->getDBType().compare("mssql") == 0){
                    CA::DBInterface *dbInterface = new CA::DBMSSql();
                    if(dbInterface){
                        dbInterface->openConnection(dbprofile->getConnectionString(),dbprofile->getConntetionTimeout());
                        dbCon->dbVect.PushBack(dbInterface);
                        std::cout << dbprofile->getConnectionString() << std::endl;
                    }
                }
                m_profile.InsertToMap(dbprofile->getProfileName(),dbCon);
                std::cout << dbprofile->getProfileName() << "Create Connection" << std::endl;
            }
        }
    }
    return true;
}
bool DBService::requestDBConnectionHandles(BSCallConfObject* ConfObj){
    std::cout << "requestDBConnectionHandles called" << std::endl;
    if(ConfObj ==  nullptr){
        // Log and send negative message to exception handler.

        return false;
    }
std::unique_ptr<BSCallConfObject> bsConfObj(new BSCallConfObject(*ConfObj));
if(bsConfObj){
    //std::unique_ptr<CA::CAMap<CA::CAMap<CA::DBInterface*>*>> temDB(new CA::CAMap<CA::CAMap<CA::DBInterface*>*>);
    std::unique_ptr<CA::CAMap<BSCallConfObject::DBINT*>> temDB(new CA::CAMap<BSCallConfObject::DBINT*>);
    bsConfObj->m_db = std::move(temDB);

    for(CA::CAMap<SPAWNTASKINFO*>::ca_itr it = ConfObj->m_spawnTaskInfo->Begin() ; it != ConfObj->m_spawnTaskInfo->End(); it++){
        if(it->second){
            SPAWNTASKINFO* task =  it->second;
            //bsConfObj->m_spawnTaskInfo.InsertToMap(task->taskName,task);
            DBConProfile* dbCon =  nullptr;
            CA::CAMap<CA::DBInterface*>* m_dbConnections =  new CA::CAMap<CA::DBInterface*>;
            dbCon = m_profile.FindValue(task->espdb);
            if(dbCon){
            m_dbConnections->InsertToMap(task->espdb,dbCon->dbVect.GetValueByPosition(dbCon->connectionCount)); 
            std::cout << task->espdb << std::endl;
            dbCon = nullptr;
            }

            dbCon = m_profile.FindValue(task->espParamdb);
            if(dbCon){
            m_dbConnections->InsertToMap(task->espParamdb,dbCon->dbVect.GetValueByPosition(dbCon->connectionCount)); 
            std::cout << task->espdb << std::endl;
            dbCon = nullptr;
            }
            dbCon = m_profile.FindValue(task->hlrdb);
            if(dbCon){
            m_dbConnections->InsertToMap(task->hlrdb,dbCon->dbVect.GetValueByPosition(dbCon->connectionCount)); 
            std::cout << task->espdb << std::endl;
            dbCon = nullptr;
            }
            dbCon = m_profile.FindValue(task->npoprdb);
            if(dbCon){
            m_dbConnections->InsertToMap(task->npoprdb,dbCon->dbVect.GetValueByPosition(dbCon->connectionCount)); 
            std::cout << task->espdb << std::endl;
            dbCon = nullptr;
            }
            dbCon = m_profile.FindValue(task->routingdb);
            if(dbCon){
            m_dbConnections->InsertToMap(task->routingdb,dbCon->dbVect.GetValueByPosition(dbCon->connectionCount)); 
            std::cout << task->espdb << std::endl;
            dbCon = nullptr;
            }
            dbCon = m_profile.FindValue(task->switchdb);
            if(dbCon){
            m_dbConnections->InsertToMap(task->switchdb,dbCon->dbVect.GetValueByPosition(dbCon->connectionCount)); 
            std::cout << task->espdb << std::endl;
            dbCon = nullptr;
            }
        bsConfObj->m_db->InsertToMap(task->taskName,m_dbConnections); 
        }
    }
    if(ConfObj->m_spawnTaskInfo)
    bsConfObj->m_spawnTaskInfo = std::move(ConfObj->m_spawnTaskInfo);
    std::cout << "connectin Count " << bsConfObj->m_db->GetMapSize() << std::endl;
    //testSQlQueryAndDBConnection(bsConfObj.release());
    bsConfObj->setMsgType(MESSAGEEVENT::MSG_BDB2BSP);
    BusinessServer::post(BUSINESS_THREADS::MVNO_BSP,std::move(bsConfObj));
}else{
    return false;
}


    return true;
}

bool DBService::testSQlQueryAndDBConnection(BSCallConfObject* testObj){

    if(testObj){
       CA::CAMap<CA::DBInterface*>* dbInterfaceMap = testObj->m_db->FindValue("esp");
        if(dbInterfaceMap){
             std::cout << "DB Interface Map " << testObj->m_db->GetMapSize() << std::endl;
            CA::DBInterface* dbInterface = dbInterfaceMap->FindValue("espdb_a1");
            if(dbInterface){
                std::cout << "DB Interface  " << dbInterfaceMap->GetMapSize() << std::endl;
                CA::DBMAP dbMap;
                //es6_GetParamDb 'BAU','438937100917893513512','tmasmvno','386'
                dbInterface->executeQuery("esp_getaccount_mvno_v2 '4368861008887'",dbMap);
                if(dbMap.size() > 0){
                    CA::DBCOL* dbcol = dbMap[0];
                    std::cout << "column Count " << dbcol->size() << std::endl;

                    CA::Data cd = (*dbcol)["custcode"];
                    std::cout << cd.data << std::endl;
                    std::cout << cd.idata  << std::endl;
                    std::cout << cd.lidata  << std::endl;
                    std::cout << cd.fdata  << std::endl;
                    std::cout << cd.ddata  << std::endl;
                    
                }
            }
        }
    }


    return true;
}
