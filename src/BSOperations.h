#ifndef BUSINESS_OPERATIONS
#define BUSINESS_OPERATIONS
//#include "CAUtilityInternal.h"
#include "SignalParam.h"
#include "BusinessConfigObject.h"
#include "BSInterface.h"
#include "BSCharging.h"
#include "BSGmsc.h"                     
enum class OPERATIONEVENT{
    OPE_ERROR=-1,
    OPE_UNKNOWN,
    OPE_TASK_INIT, // call init or processing 
    OPE_TASK_CONNECTED,
    OPE_TASK_DEINIT,
    OPE_TASK_SPAWN, // create new task(applicaiton)
    OPE_TASK_TERMINATE, // termination from business to signal
    OPE_TASK_CLEAR,
    OPE_HTTP_REQUEST,
    OPE_HTTP_RESPONSE,
    OPE_CONNECT_DB,
    OPE_CHARING_INIT,
    OPE_CHARING_START,
    OPE_CHARING_CYCLE,
    OPE_CHARING_STOP,
    OPE_GMSC_INIT,
    OPE_GMSC_CONNECTED,
    OPE_GMSC_DEINIT,
    OPE_BSP_DIAL,
    OPE_TASK_ROUTE_ERROR
};
 
 

class BSOperation{
    private:
    
    std::unique_ptr<BSParam> m_inParam; // incoming call paramter 
    std::unique_ptr<BSHttpServiceMsg> m_phttpSerMsg;
    std::string m_uuid;
    std::string m_lastKnownError;
    int m_sipErrorCode;
    OPERATIONEVENT m_operationEvent;
    int m_taskID;
    int m_isCallConnected;
    std::string m_strcallTimeOutValue;
    int m_iHttpStatusCode;
    std::string m_strHttpOutput;
    std::string m_strMOMTDBProfile;
    CA::CAVector<BSInterface*> m_taskList; 
    std::time_t m_callConnectedTime;
    std::time_t m_callDisconnetedTime;
    public: // member variables
    Application m_app;
    public: // member functions
    BSOperation()=default;
    ~BSOperation()=default;
    BSOperation(std::string callRefID);
    void copyOperation(BSOperation*);
    void setLastKnownError(std::string err){m_lastKnownError = err;};
    std::string getLastKnownError(){return m_lastKnownError;};
    void setTaskId(int id){m_taskID =  id;}
    int getTaskId(){return m_taskID;}
    void setUuid(std::string uid){ m_uuid = uid;};
    std::string getUuid(){return m_uuid;}
    void updateInParam(std::unique_ptr<BSParam> pbsp){ m_inParam =  std::move(pbsp);}
    void updateoutParam(std::unique_ptr<BSParam> pbsp){ m_inParam =  std::move(pbsp);}
    BSParam* getInParam(){return m_inParam.get();}
    BSParam* getOutParam(){return m_inParam.get();}
    void setHttpStatusCode(int code){m_iHttpStatusCode=code;}
    void setHttpOutput(std::string out){m_strHttpOutput=out;}
    void setSipErrorCode(int error){m_sipErrorCode =  error;}
    int getSipErrorCode(){return m_sipErrorCode;}
    void setOperationEvent(OPERATIONEVENT evt){m_operationEvent =  evt;}
    OPERATIONEVENT getOperationEvent(){return m_operationEvent;}
    void setCallConnectedStatus(int st){m_isCallConnected =  st;}
    int getCallConnectedStatus() {return m_isCallConnected;}
    void setConnectedTime( std::time_t stTime){m_callConnectedTime =  stTime; }
    void setDisconnectedtime(std::time_t stTime){m_callDisconnetedTime =  stTime;}
    std::time_t getConnectedTime() { return m_callConnectedTime; }
    std::time_t getDisconnectedTime() {return m_callDisconnetedTime; }
    void setCallTimeOut(std::string str){m_strcallTimeOutValue = str;}
    std::string getstrCallTimeout(){return m_strcallTimeOutValue;}
    int getCallTimeOut(){std::size_t pos{};return std::stoi(m_strcallTimeOutValue,&pos);}
    int getHttpStatusCode(){return m_iHttpStatusCode;}
    std::string getHttpOutput(){return m_strHttpOutput;}
    void setCallTime(BSCallState bsstate){
        switch(bsstate){
            case BSCallState::STATE_CONNECTED:
                m_callConnectedTime =  std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            break;
            case BSCallState::STATE_TERMINATE:
            case BSCallState::STATE_DISCONNECTED:
                m_callDisconnetedTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            break;
        }
        
    }
    void setHttpRequest(std::unique_ptr<BSHttpServiceMsg> phttp){m_phttpSerMsg =  std::move(phttp); }
    std::unique_ptr<BSHttpServiceMsg> relHttpRequestMsg(){return std::move(m_phttpSerMsg);}
    std::string getstrCallTime(std::time_t now){
         char buf[16] = {0};
         std::strftime(buf, sizeof(buf),"%H:%M:%S", std::localtime(&now));
         std::string retTime(buf);
         return retTime;
    }
    

    //supporting function 
    std::string getTrunkNameWithDdiPrefix(BusinessConfig*);
    bool getTrunkProfile(BusinessConfig*);
    int db_mvno_check_home_subscriber(CA::DBInterface*,std::string strNumber,std::string strNumber1);
    void updateExecutionStatus(BSCallState state, OPERATIONEVENT event, std::string errStr,int sipErrCode);
    
   // sub-Operation functions
   bool loadCharing(CA::CAMap<CA::DBInterface*>&);
   bool loadGmsc(CA::CAMap<CA::DBInterface*>& );
   bool spawnDefault(Application app);
  

    // operation function
    bool callInRoute(BusinessConfig*,CA::CAMap<CA::DBInterface*>&);
    bool callSpawnRoute(BusinessConfig*,CA::CAMap<CA::DBInterface*>&);
    bool callConnectState(BusinessConfig*,CA::CAMap<CA::DBInterface*>&);
    bool callDisconnectState(BusinessConfig*,CA::CAMap<CA::DBInterface*>&);
    bool callHttpUpdate(BusinessConfig*, CA::CAMap<CA::DBInterface*>&);
    bool callChargingCycleUpdate(BusinessConfig*,CA::CAMap<CA::DBInterface*>&);
    bool loadAlternateRoute(CA::CAMap<CA::DBInterface*>& dblist);
   
   // Interface List handlers
   bool insertInterface(BSInterface*);
   BSInterface* getInterface(int taskId);
   bool deleteInterfaces(); 
   

    
   
       

    

};

class BSOperationMessage : public CA::CAMessageBlock{
    public:
    BSOperationMessage()=default;
    virtual ~BSOperationMessage()=default;
    std::unique_ptr<BSOperation> m_bsOperation;
};

#endif
