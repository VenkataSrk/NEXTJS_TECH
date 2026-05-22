#ifndef BS_CALL_OBJECT_H
#define BS_CALL_OBJECT_H
#include "BSOperations.h"



class BSCallObject {
    private:
    BSCallState m_callState;
    std::string m_lastKnownError;
    std::string m_uid;
    std::string m_caIp;
    unsigned short m_caPort;
    unsigned int m_ApplicationPostion;
    BSOperation* m_pBusinessOperations; 
    int threadid;
    CA::CAThread* m_threadHandler;
    
    
    
    
    public:
    BSCallObject()=default;
    virtual ~BSCallObject();
    BSCallObject(std::string callID,std::string caIP, unsigned short caPort);
    void setUid(std::string str){m_uid =  str;}
    std::string getUid(){return m_uid;}
    void setLastKnownError(std::string err){m_lastKnownError =  err;}
    std::string getLastKnownError(){return m_lastKnownError;}
   
    void setCallState(BSCallState state){ m_callState =  state;}
    

    BSCallState getCallState(){return m_callState;}
    void setCallAgentIP(std::string ip){ m_caIp = ip;}
    std::string getCallAgentIP(){ return m_caIp;}
    void setCAllAgentPort(unsigned short port){ m_caPort =  port;}
    unsigned short getCallAgentPort(){ return m_caPort;}
    
   
   bool copyBSOperation(BSOperation* destOpr,BSOperation* srcOpr );
   
    // object execution function
    
     // handles the sip event
    BSOperationMessage* callSetupInd(BSParam* bsp);
    BSOperationMessage* callProceedingInd(BSParam* bsp);
    BSOperationMessage* callAlertingInd(BSParam* bsp);
    BSOperationMessage* callConnectedInd(BSParam* bsp);
    BSOperationMessage* callDisconnectedInd(BSParam* bsp);
    BSOperationMessage* callRouteErrorInd(BSParam* bsp);
    // other event handler
    BSOperationMessage*  callClearInd();
    void callDialingInd(BSParam* bsp);
    BSOperationMessage* callHttpResponseInd(BSOperation*);
    BSOperationMessage* callChargingcycleInd(int);
    BSOperationMessage* callTimeOutInd();
    
    
    
    // response event 
    
    bool checkCallStateUpdate(std::unique_ptr<BSOperation> );
    
    

    CA::CAMessageBlock* handleConfigUpdateEvent(BSCallConfObject*); // handles configuration update 
    BSOperation* getOperationObj(){ return (m_pBusinessOperations) ? m_pBusinessOperations : nullptr; };

    void setThreadHandler(CA::CAThread* th){(th) ? m_threadHandler =  th : m_threadHandler =  nullptr;}
    CA::CAThread* getThreadHandler(){return m_threadHandler;}
   

    

private:


    
};

class BSCallMsg : public CA::CAMessageBlock{
    public:
    std::string m_signalClientIP;
    unsigned short m_signalClientPort;
    std::string m_jsonInString;
    std::unique_ptr<BSParam> pbsp;
    BSCallMsg()=default;
    virtual ~BSCallMsg()=default;
};




#endif
