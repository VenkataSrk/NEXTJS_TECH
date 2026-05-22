#ifndef _IURMODULEFACTORY_H
#define _IURMODULEFACTORY_H

#define  NO_MEDIA_GATEWAY 0

#include "xGateMediaService.h"
#include "xGateHttpService.h"
#include "xGateRedisService.h"
//#include "XGController.h"
#include "XG_SFUController.h"
#include "xGateSFUService.h"
#include "xGateDBService.h"

//#include "xGateMBCService.h"

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
//#if NO_MEDIA_GATEWAY
#if 1
				case IURDefines::UR_MODULE_SERVICE_CONTROLLER :
					{
						ptModule = dynamic_cast<IURModule*>(XGSFUController::instance());
					}
					break;
#endif
				case IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE :
					{
						ptModule = dynamic_cast<IURModule*>(MediaService::instance());

					}
					break;
        case IURDefines::UR_MODULE_HTTP_SERVICE :
          {
            ptModule = dynamic_cast<IURModule*>(HttpService::instance());
          }
          break;
					#if 1
        case IURDefines::UR_MODULE_REDIS_SERVICE :
          {
            ptModule = dynamic_cast<IURModule*>(RedisService::instance());
          }
          break;
					#endif
        case IURDefines::UR_MODULE_SFU_SERVICE:
	        {
            ptModule = dynamic_cast<IURModule*>(SFUService::instance());
          }
          break;
        case IURDefines::UR_MODULE_DB_SERVICE :
          {
            ptModule = dynamic_cast<IURModule*>(DBService::instance());

          }
          break;
      }
    }   
};

#endif
