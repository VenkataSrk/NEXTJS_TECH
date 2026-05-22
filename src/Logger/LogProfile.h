#ifndef _LOG_PROFILE_H
#define _LOG_PROFILE_H

//system includes
#include <iostream>
#include <map>
#include <vector>
#include "../../utilities/CAUtilites.h"
//ace includes
//#include <ace/Singleton.h>

//local includes
#include "CAlog.h"
#include "CAExceplog.h"
//#include "xGateUtil.h"

//using namespace rapidjson;
using namespace std;


#define CONFIGSERVICE LOG_PROFILE::instance()


class XGConfig
{
  public:
    bool init(...);
    bool deInit();

    //member functions
    //Document 		         	m_configDoc;
    string 				m_configPath;
	
    LogProfile 				m_logProfile;
  private: 
    bool readLogProfile();
};

typedef ACE_Singleton<XGConfig, ACE_Recursive_Thread_Mutex > LOG_PROFILE;
#endif
