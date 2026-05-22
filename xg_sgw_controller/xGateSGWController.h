#ifndef _WSSBRIDGE_CONTROLLER_H__
#define _WSSBRIDGE_CONTROLLER_H__

#pragma once
#include<ace/Task.h>
#include <ace/Singleton.h>
#include <map>
#include <vector>

#include "UREngineConfig.h"
#include "IURModule.h"
#include "xGateSipMsg.h"
#include "xGateSipSignalProcessor.h"
#include "xGateNetIOServiceMsg.h"
#include "xGateJsonDefines.h"
#include "xGateJson.h"
#include "xGateJsonMgr.h"
#include "xGateDBServiceMsg.h"
#include "xGateMonitorServiceMsg.h"



#define SGWCONTROLLER SGW_CONTROLLER::instance()
typedef std::vector<xGateNetConnection>::iterator CON_IT;

class xGateSGWController : public ACE_Task<ACE_MT_SYNCH>, public IURModuleCallBack
{

public:
  xGateSGWController();
  virtual ~xGateSGWController();

  bool init(void);
  virtual int svc(void);
  bool stop();

  // Member functions to initialize modules
  bool initDBService();
  bool initSchedulerService();
  //bool initMediaService();
  bool initNetIOService();
  bool initSipService();
  bool initMonitorService();
  // Member functions to process msg from other modules
  bool handle_msg(ACE_Message_Block *pAmb);
  bool handleSipModuleMsg(xGateBaseMsg *pMsg);
  bool handleNetIOModuleMsg(xGateBaseMsg *pMsg);
  bool handleDBModuleMsg(xGateBaseMsg *pMsg);
  bool handleTimerModuleMsg(xGateBaseMsg *pMsg);
  bool CreateTcpClient();
  bool TcpClientReconnect(std::string sIPAddress, unsigned int iPort);
  bool FetchIPToWhiteList();
  bool AddIPToWhiteList(std::vector<std::string> list);
  bool AddIPToWhiteList(std::string ip_addr);
  bool RemoveIPFromWhiteList(std::vector<std::string> list);
  bool RemoveIPFromWhiteList(std::string ip_addr);
  bool AddSbcFqdnToMsTeamsDomainList(std::string sbc_fqdn);
  bool RemoveSbcFqdnFromMsTeamsDomainList(std::string sbc_fqdn);

  bool handleUserRegistrationRequest(xGateBaseMsg *pMsg);
  bool handleInviteRequest(xGateBaseMsg *pMsg);
  bool handleInviteInboundPstnRequest(xGateBaseMsg *pMsg);
  bool handleInviteMSTeamsRequest(xGateBaseMsg *pMsg);
  bool handleSipUpdateUserRegRequest(xGateBaseMsg *pMsg);
  bool handleSipDeRegRequest(xGateBaseMsg *pMsg);
  bool handleNetIORecvMsg(xGateNetIOServiceMsg *ptNetIOMsg);
  bool handleNetIOConClosedMsg(xGateNetIOServiceMsg *ptNetIOMsg);
  bool handleNetIOConnnectedMsg(xGateNetIOServiceMsg *ptNetIOMsg);
  bool sendMsgToInterrogator(std::string sMsg,int conn_id);
  std::string EncodeJson(xGateBaseMsg *pMsg, JsonOPRType jType);
  bool DecodeJson(std::string sJson);
  bool sendUserRegResponse(std::unique_ptr<xGateJsonMsg> uptJsonMsg);
  bool sendUserRegResponseError(std::unique_ptr<xGateJsonMsg> uptJsonMsg);
  bool sendInviteResponse(std::unique_ptr<xGateJsonMsg> uptJsonMsg);
  bool sendInviteResponseError(std::unique_ptr<xGateJsonMsg> uptJsonMsg);
  bool sendInviteInboundPstnResponse(std::unique_ptr<xGateJsonMsg> uptJsonMsg);
  bool sendInviteInboundPstnResponseError(std::unique_ptr<xGateJsonMsg> uptJsonMsg);
  bool sendInviteMSTeamsResponse(std::unique_ptr<xGateJsonMsg> uptJsonMsg);
  bool sendTrunkAddRequest(std::unique_ptr<xGateJsonMsg> uptJsonMsg);
  bool sendTrunkRemoveRequest(std::unique_ptr<xGateJsonMsg> uptJsonMsg);
  int ChooseNetIOId(std::string trans_id);
  bool RemoveConID(std::string trans_id);
  bool StartWhiteListRefreshTimer();
  // IUR module call back function
  bool handleModuleCallbackMsg(IURModuleMsg *pctrlMsg);
  vector<IURModule *> arrayModules;

private:
  bool m_isSipModuleInitialized;
  std::unique_ptr<xGateJson> m_Json;
  std::map<std::string, int> m_conMap;
  std::vector<xGateNetConnection> m_conList;
};

typedef ACE_Singleton<xGateSGWController, ACE_Recursive_Thread_Mutex> SGW_CONTROLLER;

#endif
