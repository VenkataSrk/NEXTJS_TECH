#ifndef _IURMODULEFACTORY_H
#define _IURMODULEFACTORY_H

#define  NO_MEDIA_GATEWAY 0


#include "xGateMediaService.h"
#include "XGController.h"
#include "xGateSFUService.h"
#include "xGateMBCService.h"

class URModuleFactory
{
  public:

    static void CreateModuleInterface( IURDefines::MODULE_ID ModuleId, IURModule *& ptModule)
    {
      ptModule = NULL;
      switch(ModuleId)
      {
        case IURDefines::UR_MODULE_UNKNOWN :
          {
            ptModule = NULL;
          }
          break;
#if NO_MEDIA_GATEWAY
        case IURDefines::UR_MODULE_SERVICE_CONTROLLER :
          {
            ptModule = dynamic_cast<IURModule*>(XGCONTROLLER_TASK::instance());
          }
          break;
#endif
        case IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE :
          {
            ptModule = dynamic_cast<IURModule*>(MediaService::instance());

          }break;

        case IURDefines::UR_MODULE_SFU_SERVICE:
	  {
	    ptModule = dynamic_cast<IURModule*>(SFUService::instance());	  	  
          }break;
#if 1
	case IURDefines::UR_MODULE_XG_MBC_SERVICE:
	  {
            ptModule = dynamic_cast<IURModule*>(XGMBC_Service::instance());		  
          }break;  
#endif
      }
    }   
};

#endif
