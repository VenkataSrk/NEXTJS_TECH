#include "CAUtilityInternal.h"
//#include "MVNOBusinessService.h"
//#include "BusinessConfigService.h"
#include "BusinessServer.h"
static volatile int keepRunning = 1;
void CASignalHandler(int signalEvent){

	std::cout << "The interrupt signal is (" << signalEvent  << "). \n";
	keepRunning =  0;
	std::unique_ptr<CA::CAMessageBlock> pMsg(new CA::CAMessageBlock);
	pMsg->setMsgType(MESSAGEEVENT::MSG_SHUTDOWN);
	BusinessServer::post(BUSINESS_THREADS::MVNO_BUS,std::move(pMsg));

    signal(signalEvent, SIG_DFL);
	kill(getpid(),signalEvent);

}
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]){

    
    std::signal(SIGABRT, CASignalHandler);
	std::signal(SIGINT,CASignalHandler);
	std::signal(SIGTERM,CASignalHandler);
	std::signal(SIGSEGV,CASignalHandler);

	
	std::cout << "MVNO Service Start In Progress.....\n";
	
	
	std::unique_ptr<BusinessConfigService> pCon (new BusinessConfigService);
	pCon->setThreadStatus(true);
	if(pCon->spwan(*pCon)){
		BusinessServer::setConfigService(pCon.release());
		sleep(1);
		std::unique_ptr<CA::CAMessageBlock> pMsg(new CA::CAMessageBlock);
		pMsg->setMsgType(MESSAGEEVENT::MSG_INIT);
		BusinessServer::post(BUSINESS_THREADS::MVNO_CONF,std::move(pMsg));

	}


	
	#if 0 // code moved to Business configuration thread.
	std::unique_ptr<MVNOBusinessService> pBus (new MVNOBusinessService);
	pBus->setThreadStatus(true);
	if(pBus->spwan(*pBus)){
		BusinessServer::setBusinessService(pBus.release());
		std::unique_ptr<CA::CAMessageBlock> pMsg(new CA::CAMessageBlock);
		pMsg->setMsgType(MESSAGEEVENT::MSG_INIT);
		BusinessServer::post(BUSINESS_THREADS::MVNO_BUS,std::move(pMsg));
	}
	#endif
	#if 0 // code moved to MVNO Business Service thread.
	std::unique_ptr<BSRequestProcessor> pBsp (new BSRequestProcessor );
	pBsp->setThreadStatus(true);
	if(pBsp->spwan(*pBsp)){
		BusinessServer::setBusinessProcessor(pBsp.release());
		std::unique_ptr<CA::CAMessageBlock> pMsg(new CA::CAMessageBlock);
		pMsg->setMsgType(MESSAGEEVENT::MSG_INIT);
		BusinessServer::post(BUSINESS_THREADS::MVNO_BSP,std::move(pMsg));
	}
	#endif
	#if 0
	std::unique_ptr<DBService> db (new DBService);
	if(db){
		db->setThreadStatus(true);
		if(db->spwan(*db)){
			BusinessServer::setDBService(db.release());
		}
	}
        #endif
	#if 0
	// New thread for report module
	std::unique_ptr<BSReportingService> pBSRep (new BSReportingService);
		if (pBSRep)
		{
			pBSRep->setThreadStatus(true);
			if(pBSRep->spwan(*pBSRep) == true)
			{
				BusinessServer::setBSReportingService(pBSRep.release());
				std::unique_ptr<CA::CAMessageBlock> pMsg (new CA::CAMessageBlock );
				pMsg->setMsgType(MESSAGEEVENT::MSG_INIT);
				BusinessServer::post(BUSINESS_THREADS::MVNO_BSREP,std::move(pMsg));
			}
		}
		
	#endif
        #if 0 //TODO::Dayana::Commented, legacy code  
	std::unique_ptr<BSHttpService> pBSHttp (new BSHttpService);
	pBSHttp->setThreadStatus(true);
	if(pBSHttp->spwan(*pBSHttp)){
		BusinessServer::setHttpService(pBSHttp.release());
		std::unique_ptr<CA::CAMessageBlock> pMsg(new CA::CAMessageBlock);
		pMsg->setMsgType(MESSAGEEVENT::MSG_INIT);
		BusinessServer::post(BUSINESS_THREADS::MVNO_HTTP,std::move(pMsg));
	}
	#endif
	while (keepRunning){
		if(keepRunning == 0)
		break;
           std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	
	

	


    return 0;
}
