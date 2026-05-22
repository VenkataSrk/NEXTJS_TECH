#ifndef _XGATE_MRCP_CLI_APP_HANDLER_H
#define _XGATE_MRCP_CLI_APP_HANDLER_H

#pragma once
#include<ace/Task.h>
#include <ace/Singleton.h>
#include <map>
#include <vector>
#include <sstream>

#include "IURModule.h"

#include "MrcpConfig.h"

#include "UniMrcpCliAppFw.h"
#include "UniMrcpCliAppFwSpeechRecog.h"

#include "apt_pool.h"
#include "uni_revision.h"
#include "apt_log.h"
#include "URMRCPBaseMsg.h"
#include "SttTtsServiceMsg.h"
#include "xGateInhouseTtsClient.h"

//#define MRCP_CLI_APP_HANDLER MrcpCliAppHandler::instance()

class xGateMrcpCliAppHandler 
{
   public:
      xGateMrcpCliAppHandler();
      virtual ~xGateMrcpCliAppHandler();

	bool init();
	bool checkConfigPath();
	bool process_msg(xGateDbConfigMgrServiceMsg *mrcpServiceMsg);
	bool process_msg_from_controller(URMRCPBaseMsg *pBaseMsg);



   private:
	struct UmcOptions
	{
		const char*        m_RootDirPath;
		const char*        m_DirLayoutConf;
		const char*        m_LogPriority;
		const char*        m_LogOutput;

		UmcOptions() : 
			m_RootDirPath(NULL), m_DirLayoutConf(NULL), 
			m_LogPriority(NULL), m_LogOutput(NULL) {}
	};

	UmcOptions      m_Options;
	UniMrcpCliAppFw*   m_pFramework;
    UniMrcpCliAppFwSpeechRecog* m_pFrameworkRecog;
	std::map<std::string, xGateInhouseTtsClient*> m_mapClientSession;
	apr_pool_t* m_pool;

};

//typedef ACE_Singleton<xGateMrcpCliAppHandler, ACE_Recursive_Thread_Mutex> MrcpCliAppHandler;

#endif
