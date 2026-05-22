#ifndef GMSC_SERVICE
#define GMSC_SERVICE
#include "CAUtilityInternal.h"
#include "BSOperations.h"
#include "BusinessConfigObject.h"
#include "BSGmsc.h"

class GmscService :  public CA::CAThread{
    private:
    CA::CAMap<BSInterface*> m_GmscList;
    CA::CAMap<CA::DBInterface*> m_dbConnections;
    public:
        GmscService()=default;
        virtual ~GmscService()=default;

        bool init();
        virtual bool run();
        bool handleMsg(CA::CAMessageBlock* pMsg);
        bool ConnectDBProfile(CA::CAMessageBlock* pMsg);
        CA::DBInterface* getDBInstance(std::string profile);
        bool handleGmscEvent(CA::CAMessageBlock* pMsg);
        bool handleHttpEvent(CA::CAMessageBlock* pMsg);
        bool loadGmscApp(BSOperation*);
        bool unloadGmscApp(BSOperation* bsOpr);
        bool updateGmscApp(BSOperation* bsOpr);
        bool postServiceEvent(BSOperation* pbsOp);
        bool insertGmscInterface(BSInterface*);
        BSInterface* getGmscInterface(std::string appKey);
        bool deleteGmscInterface(std::string appKey);

        

};
#endif