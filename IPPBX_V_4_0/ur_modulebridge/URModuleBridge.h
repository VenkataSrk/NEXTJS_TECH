#ifndef __URModuleBridge_H__
#define __URModuleBridge_H__

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <ace/Task.h>
#include <ace/Singleton.h>

#include "URModuleFactory.h"
#include "URModuleBridgeMsg.h"
#include "xGateHttpServiceMsg.h"
#include "xGateUtil.h"
#include "xGateDBServiceMsg.h"
#include "xGateRedisService.h"
#include "RedisServiceMsg.h"


using namespace std;

class URHTTPConfig : public IURModuleConfig
{
public:
	URHTTPConfig();
	~URHTTPConfig();
	int m_nHTTPThreadPool;
	virtual void setCallBack(IURModuleCallBack *ptCallback);
	virtual IURModuleCallBack *getCallBack();
};

// class URModuleBridge: public /*ACE_Task<ACE_MT_SYNCH>,*/ IURModuleCallback
class URModuleBridge : public ACE_Task<ACE_MT_SYNCH>, public IURModuleCallBack
{
	string m_strNameServerDetails;
	IURModule *m_ptModule;

public:
	URModuleBridge();
	~URModuleBridge();
	bool init();
	int svc();

	bool stop(void);
	bool handle_msg(ACE_Message_Block *pAmb);

	bool initHttpService();
	bool initDBService();
	bool initRedisService();

	bool handleModuleCallbackMsg(IURModuleMsg *ptMsg);
	xGateHttpServiceMsg *getHttpRequestMsg(string &strContext, string &strURL, HttpReqMethod tHttpmethod, string stData);
	xGateDBServiceMsg *getDBRequestMsg(std::string &strContext, std::string &spName, std::string &strData, DBServiceEvent event);
	RedisServiceMsg *getRedisPushMsg(std::string &strContext,std::string &strData);

	bool PostMessagetoHTTP(string strCallId, string strSsrvURL, string strData);
	bool postMessageToPBX(URModuleBridgeMsg *pbridgeMsg);
	bool PostMessageToDB(string strCallId, string strData ,xGateDBEvent dbEvent);
	bool PostMessageToRedis(std::string strCallId, std::string strData);

	vector< IURModule*> arrayModules;
};

// typedef ACE_Singleton<MBCHandler, ACE_Resursive_Mutex> MBCHANDLER;
typedef ACE_Singleton<URModuleBridge, ACE_Null_Mutex> URMODULE_BRIDGE;

#endif

