#ifndef BSMOMTGATEWAY_SERVICE
#define BSMOMTGATEWAY_SERVICE

#include "CAUtilityInternal.h"
#include "BusinessConfigObject.h"
#include "BSOperations.h"

class BSmomtGatewayService : public CA::CAThread{
    private:
    bool m_run;
    std::unique_ptr<BusinessConfig> m_pBSConfiguration;
    CA::CAMap<CA::DBInterface*> m_dbConnections;
    private:
    bool cliProfileDBConnect();
    bool callInRoute(BSOperation*);
    bool callNextRoute(BSOperation*);
    bool spawnApplication(BSOperation*);
    //bool callInUpdate(BSOperation*);
    //bool callOutDial(BSOperation*); // process or handle the dial request form routing to BSP thread
    
    std::string getTrunkNameWithDdiPrefix(std::string );
    int db_mvno_check_home_subscriber(CA::DBInterface*, BSOperation* bsopr );
    MESSAGEEVENT getThreadMessageType(OPERATIONEVENT oprEvt);
    bool postServiceEvent(BSOperation*);
    bool getProfiles(std::string trunkName, BSOperation* bsopr);
    public:
    BSmomtGatewayService()=default;
    virtual ~BSmomtGatewayService()=default;
    virtual bool run();
    bool handleMsg(CA::CAMessageBlock* pMsg);
    bool init(CA::CAMessageBlock* pMsg);
    bool processBSPRequest(CA::CAMessageBlock* pMsg);
    bool processRouteRequest(CA::CAMessageBlock* pMsg);
    
    

    
    
};
#endif