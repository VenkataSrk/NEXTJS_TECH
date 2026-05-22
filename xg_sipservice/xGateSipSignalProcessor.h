#ifndef SIP_SIGNAL_PROCESSOR
#define SIP_SIGNAL_PROCESSOR
//DebugVM start
#define SERVER_TYPE_SIZE 20
#define ENCODE_BUF_SIZE 4*1024
////DebugVM end
//
#include "xGateProcessor.h"
#include "xGateSipController.h" 
#include "SofiaSiphandler.h"

#include "IURModule.h"
#include "xGateSipMsg.h"
//#include "xGateMediaServiceMsg.h"

#include "SchedulerServiceDefine.h"
#include "xGateSchedulerServiceMsg.h"
#include "xGateSchedulerMsg.h"

#include <map> 
#include <uuid/uuid.h>

#define SIPSIGSERVICE SIP_SIGNAL_MODULE::instance()

class xGateSipSignalProcessor: public xGateProcessor,public IURModule
{
	
	string  m_strSipModuleName;
	IURModuleConfig * m_ptModuleCofig;
  public:
    xGateSipSignalProcessor();
    virtual ~xGateSipSignalProcessor();
    //xGateRetVal init();
    virtual int svc( void );
    virtual xGateRetVal process_msg(ACE_Message_Block*);
    void handleIncomingSIPmsg(xGateSipMsg *pSipMsg);
    void handleIncomingSchedulerMsg(xGateBaseMsg *pBaseMsg);
    void handleSipMessage(xGateCallObj* callObj);
    void handleMoniterMessage(xGateSipMsg *pSipMsg);
    void SendStartTimerRequest(xGateCallObj *callObj);
    void SendCancelTimerRequest(xGateCallObj *callObj);
    void sendOptionsSuccResponstoMoniterService(xGateCallObj *callObj);
    void sendOptionsFailedResponstoMoniterService(xGateCallObj *callObj);
	
    virtual int initModule(IURModuleConfig * ptConfig) ;
    virtual int unitModule() ;
    virtual int pushModuleMsg(IURModuleMsg * ptMsg) ;
    virtual void getModuleName(std::string& moduleName) ;
    virtual IURDefines::MODULE_ID getModuleID() ;
    virtual IURModuleConfig * getModuleConfig();
    IURModuleConfig *getConfig(){return m_ptModuleCofig; }
	
    bool handleSipInviteRequest(xGateCallObj* callObj);
    bool handleSipRegistrationProcess(xGateSofiaSipRegobj *sipregobj);
    bool SendUserRegistrationRequestToController(xGateSofiaSipRegobj *sipregobj);
    bool ProcessDoUserRegistrationResponse(xGateSofiaSipRegobj *pSipMsg);
    bool ProcessDeRegistrationRequest(xGateSofiaSipRegobj *pSipMsg);
    bool ProcessRegistrationResponseError(xGateSofiaSipRegobj *pSipMsg);
    bool UpdateRegisterationStatusToController(xGateSofiaSipRegobj *sipregobj);

    std::string generateTransid();
    std::string UUIDToSessionId(const char *uuid, const size_t len);
    bool isValidSessionID(const char* uuid, const size_t len);

    xGateRetVal stop();
    bool m_run;
    std::map<std::string, xGateCallObj*> m_callcontext;
    std::map<std::string, xGateSofiaSipRegobj*> m_regcontext;

};
typedef ACE_Singleton<xGateSipSignalProcessor, ACE_SYNCH_RECURSIVE_MUTEX> SIP_SIGNAL_MODULE;
#endif
