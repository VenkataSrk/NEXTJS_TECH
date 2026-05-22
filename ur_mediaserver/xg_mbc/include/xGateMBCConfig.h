#ifndef _MEDIA_BROADCAST_CONFIG_H
#define _MEDIA_BROADCAST_CONFIG_H

//system includes
#include <iostream>

//ace includes
#include "IURModuleConfig.h"

using namespace rapidjson;
using namespace std;


#define MEDIABROADCASTCONFIGSERVICE xGateMBCConfig::instance()

class xGateMBCConfig:public IURModuleConfig {
  public:

	void setCallBack(IURModuleCallBack* ptmediaBroadcastCallBack)
	{
		moduleCallBack = ptmediaBroadcastCallBack;
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
