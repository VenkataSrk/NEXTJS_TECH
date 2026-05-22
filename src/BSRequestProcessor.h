#ifndef BS_REQUEST_PROCESSOR_H
#define BS_REQUEST_PROCESSOR_H
#include "CAUtilityInternal.h"
#include "BusinessConfigObject.h"
#include "BSCallObject.h"

class BSRequestProcessor : public CA::CAThread{
    public:
    bool m_run;
    bool responseToCallAgent(CA::CAMessageBlock* pMsg);

    //void setCallTimer(int timeOut,std::string uqid);
    public:
    BSRequestProcessor()=default;
    virtual ~BSRequestProcessor()=default;
    bool init(CA::CAMessageBlock* pMsg);
	virtual bool run();
	bool handleMsg(CA::CAMessageBlock* pMsg);
    bool handleTimerMessage(CA::CAMessageBlock* pMsg);
    bool handleMessageFromBS(CA::CAMessageBlock* pMsg);
    bool handleMessageFromConf(CA::CAMessageBlock* pMsg);
    bool handleMessageFromDBS(CA::CAMessageBlock* pMsg);
    bool handleMessageFromMoMt(CA::CAMessageBlock* pMsg);
    bool handleMessageFromCharging(CA::CAMessageBlock* pMsg);
    bool handleMessageFromRouting(CA::CAMessageBlock* pMsg);
    bool handleMessageFromHttp(CA::CAMessageBlock* pMsg);
    bool handleMessageFromGmsc(CA::CAMessageBlock* pMsg);
    bool processSignalEvent(BSParam*,std::string caIP, unsigned short caPort);
    bool insertToRequestMap(BSCallObject*);
    BSCallObject* getFromRequestMap(std::string uuid);
    bool deleteFromRequestMap(std::string uuid);
    bool formJsonResponse(BSCallObject*);
    
    bool postMessage(CA::CAMessageBlock*,CA::CAThread* th);
    bool TestsendMsg(CA::CAMessageBlock*);

    std::string getTimeStrFormated(std::time_t); 
    
    private: // member variable
    CA::CAMap<BSCallObject*> m_bsRequestMap;
    CA::CAMap<CA::CAThread*> m_bsThreadPool;
    std::string m_strcallTimeOut;
    std::string m_strDBWaitTimeOut;
    std::unique_ptr<BusinessConfig> m_bsconfiguration;
    int lastVistedThreadId = 0;
    private: // member function
    void clearCallObject(BSCallObject*);
    void clearParamObject(BSParam*);
    //void startThreadPool(int Maxthreadcount);
    void startRoutingPool(int Maxthreadcount);
    void updateConfigRoutingPool(int);
    void updateConfig(CA::CAMessageBlock *pMsg);
    CA::CAThread* getAvailableThread();
    void stopThreadPool();
    int getThreadId(){ return lastVistedThreadId;}
    


    


};

#endif
