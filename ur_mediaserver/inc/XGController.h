#ifndef _XGATE_XG_CONTROLLER_H
#define _XGATE_XG_CONTROLLER_H
#include <ace/Task.h>
#include <ace/Singleton.h>

//local includes
#include "VmsConfig.h"
#define XGCONTROLLER XGCONTROLLER_TASK::instance()
class XGController : public ACE_Task<ACE_MT_SYNCH> {

	//VMSServerInfo m_tVMSServerInfo;
	public:  
	XGController();
	~XGController(void);

	bool init();
	virtual int svc(void);
	bool stop();

	private:
	bool m_run;
	//VmsControllerProfile m_vmsControllerProfile;
	int initHttpService();
	bool handle_msg(ACE_Message_Block *pAmb);
};

typedef ACE_Unmanaged_Singleton<XGController, ACE_Null_Mutex> XGCONTROLLER_TASK;
#endif
