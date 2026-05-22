#include <string>
#include "xglog.h"
#include "UREngineConfig.h"
#include "URCBController.h"
#include "ReadConfigFromProxyConfFile.h"

#define THISMODULE "ACE_TMAIN"

using namespace std;


int ACE_TMAIN (int, ACE_TCHAR *[])
{
 
 
GlobalClass *ConfigLoad=GlobalClass::instance();
 ConfigLoad->loadConfig();
 
 
XGLOG_INFO( THISMODULE,"REGTAG(%s),PUBTAG(%s),\
		   	MBSERVERIP(%s),TOPICA(%s),TOPICB(%s),GROUPA(%s),\
		   	GROUPB(%s)",ConfigLoad->m_REGTAG.c_str(),ConfigLoad->m_PUBTAG.c_str(),\
		   	ConfigLoad->m_MBSERVERIP.c_str(),ConfigLoad->m_TOPICA.c_str(),ConfigLoad->m_TOPICB.c_str(),\
			ConfigLoad->m_GROUPA.c_str());
 
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
  retVal = URCBCONTROLLER->init();
  if(!retVal) {
    XGLOG_ERROR("URConf controller module initialization failed !");
  } else {
    XGLOG_INFO("URConf controller module successfully initialized !");
  }
  unsigned int count = 0;
  do {
    //XGLOG_WARN("now %s logger count is %d !", "error", ++count);
    usleep(500000);
  }while(1);

  return 0;
}
