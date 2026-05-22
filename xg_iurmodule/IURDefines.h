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
	  UR_MODULE_MEDIA_PROXY_SERVICE = 2,
	  UR_MODULE_SIPSIGNAL_SERVICE = 3,
	  UR_MODULE_HTTP_SERVICE = 4,
	  UR_MODULE_IVR_SERVICE = 5,
	  UR_MODULE_RECORDER_MEDIA_MODULE = 6,
      UR_MODULE_SCHEDULER_SERVICE = 7,
      UR_MODULE_MBC_SERVICE = 8,
	  UR_MODULE_CALLQ_SERVICE = 9,
	  UR_MODULE_CALL_PARK_SERVICE = 10,
	  UR_MODULE_MBC_LINK_SERVICE = 11,
	  UR_MODULE_REG_SERVICE = 12,
	  UR_MODULE_REDIS_SERVICE = 13,
	  UR_MAX_LEN,// using as boundary
	} MODULE_ID;
	
	typedef enum __URRETURN_ENUM {

		UR_RETURN_ERROR = -1,
		UR_RETURN_SUCCESS = 0,// not using
		}RETURN_VALUES;
};
#endif

