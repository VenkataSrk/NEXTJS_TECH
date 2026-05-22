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

#ifdef XGATE_HAS_BUSINESS_SUPPORT
#include "xGateBusinessHandler.h"
#endif

#include "IURModule.h"
#include "SipMediahandler.h"
#include <map> 

//Queue for communication between Processor and Main::callmanager
//TODO: CTask processing need to analysied and remodified
#if 0 //DebugVM start
std::list<xGateCallObj*> callidQ;
ACE_Token mutex_;

void xGateEnQcallobj(xGateCallObj*callobj) {
  ACE_Guard<ACE_Token>gaurd(mutex_);
  callidQ.push_back(callobj);
}

xGateCallObj *xGateDeQcallobj() {
  xGateCallObj *ret = NULL;
  ACE_Guard<ACE_Token>gaurd(mutex_);
  if(!callidQ.empty()) {
    ret = callidQ.front();
    callidQ.pop_front();
  }
  return ret;
}

bool xGateRemoveCallObj(std::string callid)
{
  for(std::list<xGateCallObj*>::iterator it =callidQ.begin(); it !=callidQ.end(); ++it){
    if((*it)->callid.compare(callid) == 0){
      callidQ.remove(*it);
      return true;
    }
  }
  return false;
}
#endif //DebugVM end


class xGateSipSignalProcessor: public xGateProcessor,public IURModule
{
	
	string  m_strSipModuleName;
	IURModuleConfig * m_ptModuleCofig;
  public:
    xGateSipSignalProcessor();
    virtual ~xGateSipSignalProcessor();
    virtual int svc( void );
    virtual xGateRetVal process_msg(ACE_Message_Block*);
    void handleIncomingSIPmsg(xGateSIPMsg *pSipMsg);
    void handleSipMessage(xGateCallObj* callObj);
    void sendCallDisconnectRequestToReceiver(xGateCallObj *callObj);
    void sendCallConnectedRequest(xGateCallObj *callObj);
    void sendCallDisconnectedRequest(xGateCallObj *callObj);
	
    virtual int initModule(IURModuleConfig * ptConfig) ;
    virtual int unitModule() ;
    virtual int pushModuleMsg(IURModuleMsg * ptMsg) ;
    virtual void getModuleName(std::string& moduleName) ;
    virtual IURDefines::MODULE_ID getModuleID() ;
    virtual IURModuleConfig * getModuleConfig();
    IURModuleConfig *getConfig(){return m_ptModuleCofig; }
	
    //Added for URAVBridge Conference
    bool SendAnswerResponseForConfCall(xGateCallObj *callObj,std::string updatedSdp,std::string strRouteAddr);
    bool SendRedirectRequestForConfCall(xGateCallObj *callObj,std::string strRedirectIp); 
    bool handleErrorMessageResponse(xGateCallObj *callObj,xGateSIPMsg *pSipMsg);

    xGateRetVal stop();
    bool m_run;
    std::map<std::string, xGateCallObj*> m_callcontext;
};
typedef ACE_Singleton<xGateSipSignalProcessor, ACE_SYNCH_RECURSIVE_MUTEX> SIP_SIGNAL_MODULE;
#endif
