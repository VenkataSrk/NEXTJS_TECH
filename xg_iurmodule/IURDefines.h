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
	  UR_MODULE_SERVICE_BRIDGE = 1,
	  UR_MODULE_MRCP_CLI_APP_SERVICE = 2,
	  UR_MODULE_DBCONFIG_MGR_SERVICE = 3,
	  UR_MODULE_MRCP_REDIS_DB_SERVICE = 4,
	  UR_MODULE_MRCP_REDIS_DB_SUBSCRIBE = 5,
	  UR_MODULE_SCHEDULER_SERVICE = 6,
	  UR_MAX_LEN,// using as boundary
	} MODULE_ID;
	
	typedef enum __URRETURN_ENUM {

		UR_RETURN_ERROR = -1,
		UR_RETURN_SUCCESS = 0,// not using
		}RETURN_VALUES;
};
#endif

