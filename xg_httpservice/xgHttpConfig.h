#ifndef _XGHTTP_CONFIG_H
#define _XGHTTP_CONFIG_H

//system includes
#include <iostream>

//ace includes
#include "IURModuleConfig.h"

using namespace rapidjson;
using namespace std;


#define HTTPCONFIGSERVICE xgHttpConfig::instance()

class XGHttpConfig:public IURModuleConfig {
  public:

	void setCallBack(IURModuleCallBack* phttpCallBack)
	{
		moduleCallBack = phttpCallBack;
	}
	IURModuleCallBack* getCallBack()
	{
		return moduleCallBack;
	}

  private:
    //member functions
   // void setCallBack(IURModuleCallBack* ptCallBack);
 
};

#endif
