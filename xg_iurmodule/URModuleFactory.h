#ifndef _IURMODULEFACTORY_H
#define _IURMODULEFACTORY_H

#include "xGateSipSignalProcessor.h"
#include "xGateSGWController.h"
#include "xGateSchedulerService.h"
#include "xGateNetIOService.h"
#include "xGateDBService.h"
#include "xGateMonitorService.h"

class URModuleFactory
{
public:
	static void CreateModuleInterface(IURDefines::MODULE_ID ModuleId, IURModule *&ptModule)
	{
		ptModule = nullptr;
		switch (ModuleId)
		{
		case IURDefines::UR_MODULE_UNKNOWN:
		{
			ptModule = nullptr;
		}
		break;
		case IURDefines::UR_MODULE_SERVICE_CONTROLLER:
		{
			ptModule = dynamic_cast<IURModule *>(SGW_CONTROLLER::instance());
		}
		break;
		case IURDefines::UR_MODULE_SIPSIGNAL_SERVICE:
		{
			ptModule = dynamic_cast<IURModule *>(SIP_SIGNAL_MODULE::instance());
		}
		break;
		case IURDefines::UR_MODULE_SCHEDULER_SERVICE:
		{
			ptModule = dynamic_cast<IURModule *>(SchedulerService::instance());
		}
		break;
		case IURDefines::UR_MODULE_NETIO_SERVICE:
		{
			ptModule = dynamic_cast<IURModule *>(NetIOService::instance());
		}
		break;
		case IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE:
		{
			//ptModule = dynamic_cast<IURModule *>(MediaService::instance());
		}
		break;
		case IURDefines::UR_MODULE_DB_SERVICE:
		{
			ptModule = dynamic_cast<IURModule *>(DBService::instance());
		}
		break;
		case IURDefines::UR_MODULE_MONITOR_SERVICE:
		{
			ptModule = dynamic_cast<IURModule *>(MonitorService::instance());
		}
		break;
		}
	}
};

#endif
