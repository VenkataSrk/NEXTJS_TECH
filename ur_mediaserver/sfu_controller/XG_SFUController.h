#ifndef _XGATE_SFU_CONTROLLER_H__
#define _XGATE_SFU_CONTROLLER_H___

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

#define XGSFUCONTROLLER XGSFUController::instance()

class XG_SFUController : public ACE_Task<ACE_MT_SYNCH>, public IURModuleCallBack 
{	

  typedef enum __xgcontroller_status{
    XGCONTROLLER_STATUS_DOWN = 0,
    XGCONTROLLER_STATUS_REQ_SRV_INFO = 1, // rquesting the server info to db
    XGCONTROLLER_STATUS_REQ_USR_INFO = 2,
    XGCONTROLLER_STATUS_MODULES_INIT = 3, // server info is available init modules and all the modules are ready
    XGCONTROLLER_STATUS_MODULES_UNINIT =4,// uninitialization process in prgress.
  }XGCONTROLLER_STATUS;

  typedef enum __xgcontroller_internal_msg{
    XGCONTROLLER_CTRL_INT_UNKNOWN = 0,
    XGCONTROLLER_CTRL_INT_SRV_INFO = 1, // rquesting the server info to db
    XGCONTROLLER_CTRL_INT_USR_INFO = 2, // server info is available init modules and all the modules are ready
    XGCONTROLLER_CTRL_INT_SAV_VMAIL_INFO =3,// uninitialization process in prgress.
    XGCONTROLLER_CTRL_INT_VALIDATE_EXTN = 4,
    XGCONTROLLER_CTRL_INT_VALIDATE_PIN = 5,
  }XGCONTROLLER_INTERNAL_MSG;

  public:  
  XG_SFUController();
  ~XG_SFUController(void);

  //bool init(int);
  bool init();
  int svc(void);
  bool stop();
  bool PushMsgFromCtrltoModule();
  bool handleModuleCallbackMsg(IURModuleMsg * pctrlMsg);

  private:
  XGCONTROLLER_STATUS		m_eCtrlStatus;

  int initHttpService();
  bool m_run;
  XGConfig * 	m_ptconfig;
  string 		m_localIpAddress;
  string 		m_registerUserName;
  vector< IURModule*> arrayModules;
  std::string getCurrentDateTime();
  bool initMediaModules();
  bool initDBModule();  
  bool initRedisModule();
  //bool initSFUModule(int); 
  bool initSFUModule(); 
  bool handle_ms_sfu(IURModuleMsg*);
  bool handle_sfu_ms(IURModuleMsg*);
  bool handle_InternalCtrlMsg(xGateBaseMsg *pMsg);
  bool handle_HttpResponse(xGateBaseMsg *pMsg);
  bool handleCallSaveVMailNotify(xGateBaseMsg *pMsg);
  bool handle_msg(ACE_Message_Block *pAmb);   
  bool postMsgToModule(xGateBaseMsg *pMsg);

  bool handle_db_response(IURModuleMsg *pctrlMsg);
  bool post_msg_to_redis_mod(std::string strMsg,std::string strCallId);
  //util
  bool getSipUNandDN(std::string sipURL, std::string& strUN, std::string& strDNS);
  void splitValues(std::vector<string> &Values, std::string strValue, std::string delim);
	Value setJsonParam(string &strValue, Document &doc);
  Value setJsonParam(int iValue, Document &doc);
};

typedef ACE_Unmanaged_Singleton<XG_SFUController, ACE_Null_Mutex> XGSFUController;
#endif
