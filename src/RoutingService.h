#ifndef ROUTING_SERVICE
#define ROUTING_SERVICE
#include "CAUtilityInternal.h"
#include "BSOperations.h"
#include "BSRouting.h"
#include "BSCharging.h"
enum class REQUESTTYPE{
    RT_NONE= -1,
    RT_CREATE = 0,
    RT_UPDATE,
    RT_DELETE,
    
};
class RoutingService :  public CA::CAThread{
    private:
    CA::CAMap<BSOperation*> m_OprList;
    std::unique_ptr<BusinessConfig> m_conf;
    CA::CAMap<CA::DBInterface*> m_dbConnections;
    private:
        bool insertBusinessOperation(BSOperation*);
        BSOperation* getBusinessOperation(std::string );
        bool removeBusinessOperation(std::string );
        bool setupDBProfile(CA::CAMap<DBProfile*>* , REQUESTTYPE);
        bool taskInit(BSOperation*);
        bool taskConnect(BSOperation*); 
        bool taskDeInit(BSOperation*);
        bool taskHttpResponse(BSOperation*);
        bool taskChargingCycle(BSOperation*);
        bool taskClean(BSOperation*);
        bool executeReqOperationEvent(BSOperation*);
        bool executeResOperationEvent(BSOperation*);
        bool sendHTTPRequest(BSOperation*);
        bool reconnectDB(DBProfile*);
        bool connectDB(DBProfile*);
	bool taskAlternateRoute(BSOperation* bsOpr);
        
    
        
public:
RoutingService()=default;
virtual ~RoutingService()=default;
bool init();



virtual bool run();
bool handleMsg(CA::CAMessageBlock* pMsg);
bool handleIntMsg(CA::CAMessageBlock* pMsg);
bool handleConfigUpdate(CA::CAMessageBlock* pMsg);
bool handleMessageFromBSP(CA::CAMessageBlock* pMsg);
bool postServiceEvent(BSOperation*);
bool getNextTask(BSOperationMessage* pMsg, BSOperation* bsp);
bool handleDBWaitTimeOut();

};
#endif 
