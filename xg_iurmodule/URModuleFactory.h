#ifndef _IURMODULEFACTORY_H
#define _IURMODULEFACTORY_H

#include "xGateMBCLinkService.h"
#include "xGateSipSignalProcessor.h"
#include "URCBController.h"
#include "xGateSchedulerService.h"
#include "xGateRedisService.h"

class URModuleFactory
{
  public:

    static void CreateModuleInterface( IURDefines::MODULE_ID ModuleId, IURModule *& ptModule)
    {
      ptModule = NULL;
      switch(ModuleId)
      {
        case   IURDefines::UR_MODULE_UNKNOWN :
          {
            ptModule = NULL;
          }
          break;
        case   IURDefines::UR_MODULE_SERVICE_CONTROLLER :
          {
            ptModule = dynamic_cast<IURModule*>(URCB_CONTROLLER::instance());
          }
          break;
        case   IURDefines::UR_MODULE_SIPSIGNAL_SERVICE :
          {
            ptModule = dynamic_cast<IURModule*>(SIP_SIGNAL_MODULE::instance());
          }break;
        case   IURDefines::UR_MODULE_SCHEDULER_SERVICE:
          {
	   				 ptModule = dynamic_cast<IURModule*>(SchedulerService::instance());
          }break;
        case   IURDefines::UR_MODULE_MBC_LINK_SERVICE:
          {
	   				 ptModule = dynamic_cast<IURModule*>(MBCLinkService::instance());
          }break;
		case IURDefines::UR_MODULE_REDIS_SERVICE:
		  {
		             ptModule = dynamic_cast<IURModule*>(RedisService::instance());
		  }
       } 
    }

};

#endif
