
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <iostream>

#define __linux__

#define E_LINE_DROP 1

#include "xGateUtil.h"

// New implemntation on Sip Stack
#include "xGateSipUtil.h"
#include "xGateSipSignalReceiver.h"
#include "xGateSipSignalDispatcher.h"
#include "xGateSipSignalProcessor.h"
//#include "xGateDBManager.h" // commented for ippbxi
#include "xGateHmpProcessor.h"
#include "xGateHmpGstManager.h"
#ifdef XGATE_HAS_DBM_SUPPORT
#include "xGateDbmProcessor.h"
#include "xGateDbmDispatcher.h"
#include "xGateDbmReceiver.h"
#endif
#include "xglog.h"
#include "xGConfig.h"
//
// Include automatically generated compile information
//
#include <sofia-sip/nua.h>
//-----------------------------------------------------------------
unsigned short BUILD_MAJOR_NUM =     68;        // major numbering
unsigned short BUILD_MINOR_NUM =     82;        // minor version
//-----------------------------------------------------------------

#define EXIT_NORMAL         0   // shutdown properly
#define EXIT_INIT_ERROR     1   // error in initialization
#define EXIT_NO_MEM         2   // new_bad_alloc exception
#define EXIT_ERROR          3   // unexpected run-time error (unhandled exceptions, etc)
#define EXIT_RESTART_REQ    4


//-----------------------------------------------------------------------------
// Run the engine in the console
//
int SL_ConsoleMain(unsigned major, unsigned minor)
{
	//initialize xglogger module
	bool retVal = XGLOG_INIT();
	if(!retVal) {
		printf("ERROR: XGLogger module initialization failed !!\n");
	} else {
		XGLOG_INFO("XGLogger successfully initialized !");
	}

	//Initialize Configuration module
	bool retValConfig = XGCONFIG_INIT();
	if(!retValConfig) {
		cout << "Config Init Failed! " << endl;
	} else {
		cout << "Config Init Success" << endl;
	}


	// Sip Reciver implementing nofifier or recort pattern
	xGateSipSignalReceiver *pSipSignalReceiver =  new xGateSipSignalReceiver("SipReceiver",NULL);
	if(pSipSignalReceiver->init() != 0){
		return EXIT_INIT_ERROR;
	}

	xGateSipSignalDispatcher *pSipSignalDispatcher =  new xGateSipSignalDispatcher();
	if(pSipSignalDispatcher->init() !=  EN_XGATE_STATUS_SUCCESS){
		return EXIT_INIT_ERROR;
	}

	SetSipPostHandler((void*)pSipSignalDispatcher,XGATEPOSTTYPE::EN_DISPATCH);
	cout << "Dispatcher handle set pSipSignalDispatcher - " << pSipSignalDispatcher << endl;

	xGateSipSignalProcessor *pSipSignalProcessor =  new xGateSipSignalProcessor();
	if(pSipSignalProcessor->init(1) !=  EN_XGATE_STATUS_SUCCESS){
		return EXIT_INIT_ERROR;
	} else {
	}

	if(pSipSignalProcessor->initHTTPBridge() != EN_XGATE_STATUS_SUCCESS) {
		return EXIT_INIT_ERROR;
	}
	SetSipPostHandler((void*)pSipSignalProcessor,XGATEPOSTTYPE::EN_PROCESS);
	cout << "processor handle set pSipSignalProcessor - " << pSipSignalProcessor << endl;


#if 1 // Media service separated to another server
	xGateHmpProcessor *pHmpProcessor = new xGateHmpProcessor("HmpProcessor");
	int HmpProcessorCount = 1;
	if(pHmpProcessor->init(HmpProcessorCount) != EN_XGATE_STATUS_SUCCESS)
	{
		cout << "xGateMgProcessor initialized from main thread" << endl;
		return EXIT_INIT_ERROR;
	}
	xGateUtil::setHmpProcessor(pHmpProcessor);


	xGateHmpGstManager *pHmpGstManager = new xGateHmpGstManager("HmpGstManager");
	if(pHmpGstManager->init() != EN_XGATE_STATUS_SUCCESS)
	{
		cout << "xGateMgProcessor initialized from main thread" << endl;
		return EXIT_INIT_ERROR;
	}

	xGateUtil::setHmpGstManager(pHmpGstManager);
	//////////////// Changes for IPPBX-Signalling
#endif
	cout << "****In  SL_ConsoleMain, going to start the loop *******" << endl;

	do
	{
		usleep(600000);
	}while(true);
	return EXIT_NORMAL;


}

void printlabel(void){

  char welcomestring[] = "\
********************************************************************************************************\n\
********************************************************************************************************\n\				
                                \n\
                                \n\
                                88888888  8888888   88888888                                   \n\  
                                88    88  88    88  88    88                                   \n\
                                88	  88    88  88                                         \n\
                                88888888  8888888   88888888                                   \n\
                                      88  88 88           88                                   \n\
                                88    88  88  88    88    88                                   \n\
                                88888888  88   88   88888888                                   \n\
                                \n\
                                WORKTUAL  \n\
********************************************************************************************************\n\
********************************************************************************************************\n";

  printf("%s",welcomestring);

}

int main(int argc, char* argv[])
{
	int retcode = 1;
	pthread_t tid;

	printlabel();


	retcode = SL_ConsoleMain(BUILD_MAJOR_NUM,BUILD_MINOR_NUM);
	return retcode;
}
