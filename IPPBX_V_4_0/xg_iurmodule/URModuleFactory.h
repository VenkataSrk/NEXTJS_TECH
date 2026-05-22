#ifndef _IURMODULEFACTORY_H
#define _IURMODULEFACTORY_H

#include "xGateHttpService.h"
#include "xGateDBService.h"
#include "xGateRedisService.h"
class URModuleFactory
{
public:
  static void CreateModuleInterface(IURDefines::MODULE_ID ModuleId, IURModule *&ptModule)
  {
    //ptModule = NULL;
    switch (ModuleId)
    {
    case IURDefines::UR_MODULE_UNKNOWN:
    {
      ptModule = NULL;
    }
    break;
    case IURDefines::UR_MODULE_HTTP_SERVICE:
    {
      xGateHttpService *ptSrv = HttpService::instance();
      if (ptSrv)
      {
        ptModule = dynamic_cast<IURModule *>(ptSrv);
      }
    }
    break;
    case IURDefines::UR_MODULE_DB_SERVICE:
    {
      xGateDBService *ptSrv = DBService::instance();
      if(ptSrv)
      {
        ptModule = dynamic_cast<IURModule *>(ptSrv);
      }
    }
    break;
    case IURDefines::UR_MODULE_REDIS_SERVICE:
    { 
      xGateRedisService *ptSrv = RedisService::instance();
      if(ptSrv)
      {
        ptModule = dynamic_cast<IURModule *>(ptSrv);
      }
    }
    break;
    }
  }
};
#endif
