#ifndef _IURMODULEFACTORY_H
#define _IURMODULEFACTORY_H

#include "xGateMrcpCliAppService.h"
#include "MrcpRedisDbConnector.h"
#include "MrcpRedisDbSubscriber.h"
#include "xGateSchedulerService.h"

class URModuleFactory
{
	public:

		static void CreateModuleInterface( IURDefines::MODULE_ID ModuleId, IURModule *& ptModule)
		{
			//ptModule = NULL;
			switch(ModuleId)
			{
				case   IURDefines::UR_MODULE_UNKNOWN :
					{
						ptModule = NULL;
					}
					break;
				case   IURDefines::UR_MODULE_MRCP_CLI_APP_SERVICE:
					{
						ptModule = dynamic_cast<IURModule*>(MrcpCliAp_Srvice::instance());
					}break;
				case IURDefines::UR_MODULE_MRCP_REDIS_DB_SERVICE:
					{
						ptModule = dynamic_cast<IURModule*>(RedisDbService::instance());
					}break;
				case IURDefines::UR_MODULE_MRCP_REDIS_DB_SUBSCRIBE:
					{
						ptModule = dynamic_cast<IURModule*>(RedisDbSubService::instance());
					}break;
				 case IURDefines::UR_MODULE_SCHEDULER_SERVICE:
					{
						ptModule = dynamic_cast<IURModule*>(SchedulerService::instance());
					}break;
			}
		}
};



#endif
