#ifndef CHARGING_SERVICE
#define CHARGING_SERVICE
#include "CAUtilityInternal.h"
#include "BSOperations.h"
#include "BSCharging.h"


class ChargingService :  public CA::CAThread{
    private:
     CA::CAMap<CA::DBInterface*> m_dbConnections;
     CA::CAMap<BSInterface*> m_chargingList;

public:
ChargingService()=default;
virtual ~ChargingService()=default;
bool init();

virtual bool run();
bool handleMsg(CA::CAMessageBlock* pMsg);
bool ConnectDBProfile(CA::CAMessageBlock* pMsg);
bool handleChargingEvent(CA::CAMessageBlock* pMsg);
CA::DBInterface* getDBInstance(std::string profile);
bool loadChargingApp(BSOperation*);
bool OnCallConnect(BSOperation*);
bool OnCallDisconnect(BSOperation*);
bool OnCallCycleEvent(BSOperation*);
bool postServiceEvent(BSOperation* pbsOp);

bool insertchargingInterface(BSInterface*);
BSInterface* getChargingInterface(std::string appKey);
bool deleteChargingInterface(std::string appKey);




};
#endif