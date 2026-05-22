#ifndef __IURDEFINE__H__
#define __IURDEFINE__H__

#pragma once

class IURDefines
{
public:
	IURDefines();
	~IURDefines();

	typedef enum __URMODULE_ENUM {
		
	  UR_MODULE_UNKNOWN = 0,// not using
	  UR_MODULE_SERVICE_CONTROLLER = 1,
	  UR_MODULE_SIPSIGNAL_SERVICE = 2,
      UR_MODULE_SCHEDULER_SERVICE = 3,
	  UR_MODULE_NETIO_SERVICE = 4,
	  UR_MODULE_MEDIA_PROXY_SERVICE = 5,
	  UR_MODULE_DB_SERVICE = 6,
	  UR_MODULE_MONITOR_SERVICE = 7,
	  UR_MAX_LEN,// using as boundary
	} MODULE_ID;
	
	typedef enum __URRETURN_ENUM {

		UR_RETURN_ERROR = -1,
		UR_RETURN_SUCCESS = 0,// not using
		}RETURN_VALUES;
};
#endif

