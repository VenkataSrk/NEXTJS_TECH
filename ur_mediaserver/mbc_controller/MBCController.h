#ifndef _MBC_CONTROLLER_H__
#define _MBC_CONTROLLER_H___

#pragma once 
#include <ace/Task.h>
#include <ace/Singleton.h>
#include <map>
#include <vector>

//local includes
//
#include "xGateBaseMsg.h"
#include "XGConfig.h"
#include "IURModule.h"

#define MBCCONTROLLER MBC_CONTROLLER::instance()

class MBCController : public ACE_Task<ACE_MT_SYNCH>, public IURModuleCallBack 
{	
  public:  
  MBCController();
  ~MBCController(void);

  bool init();
  int svc(void);
  bool stop();
  bool handleModuleCallbackMsg(IURModuleMsg * pctrlMsg);

  private:
  //member functions
  int initModule();
  bool handle_msg(ACE_Message_Block *pAmb);  
  bool handle_MBCModuleMsg(xGateBaseMsg *pMsg);
  bool handle_MediaServiceModuleMsg(xGateBaseMsg *pMsg); 
  bool handleMBCClientConnectionReq(xGateBaseMsg *pMsg);
  bool handleMBCTcpListenRequest(xGateBaseMsg *pMsg);
  bool postMBCMsgToMediaService(xGateBaseMsg *pMsg);

  //member variables
  bool m_run;
  XGConfig * m_ptconfig;
  string m_localIpAddress;
  string m_registerUserName;
  vector< IURModule*> arrayModules;
};

typedef ACE_Unmanaged_Singleton<MBCController, ACE_Null_Mutex> MBC_CONTROLLER;
#endif
