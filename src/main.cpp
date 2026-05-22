#include <string>
#include "xglog.h"
#include "UREngineConfig.h"
#include "xGateSGWController.h"

#define THISMODULE "ACE_TMAIN"

using namespace std;


int ACE_TMAIN (int, ACE_TCHAR *[])
{ 
  //initialize xglogger module
  bool retVal = XGLOG_INIT();
  if(!retVal) {
    printf("ERROR: XGLogger module initialization failed !!\n");
  } else {
    XGLOG_INFO("XGLogger successfully initialized !");
  }

  //initialize configuration module 
  retVal = CONFIGSERVICE->init();
  if(!retVal) {
    XGLOG_ERROR("Ivr configuration module initialization failed !");
  } else {
    XGLOG_INFO("Ivr configuration module successfully initialized !");
  }

  //initialize acd controller module
  retVal = SGWCONTROLLER->init();
  if(!retVal) {
    XGLOG_ERROR("URWss controller module initialization failed !");
  } else {
    XGLOG_INFO("URWss controller module successfully initialized !");
  }
  unsigned int count = 0;
  do {
    //XGLOG_WARN("now %s logger count is %d !", "error", ++count);
    usleep(500000);
  }while(1);

  return 0;
}
