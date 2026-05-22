#ifndef _XGSCHEDULER_CONFIG_H
#define _XGSCHEDULER_CONFIG_H

//system includes
#include <iostream>

#include "IURModuleConfig.h"

using namespace rapidjson;
using namespace std;


#define SCHEDULERCONFIGSERVICE xgSchedulerConfig::instance()

class XGSchedulerConfig:public IURModuleConfig {
  public:

        void setCallBack(IURModuleCallBack* pschedulerCallBack)
        {
                moduleCallBack = pschedulerCallBack;
        }
        IURModuleCallBack* getCallBack()
        {
                return moduleCallBack;
        }

};
#endif

