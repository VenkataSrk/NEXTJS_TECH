#ifndef SIP_SIGNAL_PROCESSOR
#define SIP_SIGNAL_PROCESSOR
#include "xGateProcessor.h"
#include "xGateSipController.h" 
#include "SofiaSiphandler.h"
#include "xGateMgMsg.h"
#include "URModuleBridge.h"
//#include "xGatePBXBusiness.h" 

//#ifdef XGATE_HAS_BUSINESS_SUPPORT
//#include "xGateBusinessHandler.h"
//#endif

//Queue for communication between Processor and Main::callmanager
//TODO: CTask processing need to analysied and remodified
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

class xGateSipSignalProcessor: public xGateProcessor
{
  public:
    xGateSipSignalProcessor();
    virtual ~xGateSipSignalProcessor();
    //xGateRetVal init();
    virtual int svc( void );
    virtual xGateRetVal process_msg(ACE_Message_Block*);
    void SendToMainProcessor(xGateCallObj *callobj);
    void SendToReceiver(xGateCallObj *callobj); // Loop BAck withoutProcessing
    void processBusinessRequest(xGateCallObj* callobj);
#if 0 //SRC
// #if 0 //TODO: Yoga disabled
//     void processBusinessResponse(xGateDBObj *dbobj); 
// #else
//     bool processBusinessResponse(xGateBusinessMsg *pMsg);
// #endif
#endif
    void PostMessageToDBManager(xGateDBObj* dbObj);

    xGateRetVal stop();
    bool m_run;

// #ifdef XGATE_HAS_BUSINESS_SUPPORT
//     xGateRetVal initBusinessHandler();
// #endif
    xGateRetVal initHTTPBridge();

  private:
    xGateRetVal handle_media_response(xGateMgMsg *pMgMsg);         
    /*void PostMessageToReceiver(int cs, std::string callid,\
        CallParameter cp, XGATESIPEVENT sipevent, \
        std::string ipaddr, unsigned int port, unsigned int codec, unsigned int calldir);*/
        std::string EncodeSRSServerInfo();
        Value setJsonParam(string &strValue, Document &doc);
        Value setJsonParam(unsigned int iValue, Document &doc); 
// #ifdef XGATE_HAS_BUSINESS_SUPPORT
//     //member variables
//     xGateBusinessHandler *m_businessHandler;
// #endif
};
#endif
