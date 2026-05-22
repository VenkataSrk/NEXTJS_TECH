#ifndef BS_DB_SERVICE_H
#define BS_DB_SERVICE_H
#include "BusinessConfigObject.h"

class DBConProfile{
    public:
    DBConProfile()=default;
    virtual ~DBConProfile()=default;
    DBProfile m_dbDetails;
    CA::CAVector<CA::DBInterface*> dbVect;
    int connectionCount;




};
class DBService :  public CA::CAThread{

    private:
    CA::CAMap<DBConProfile*> m_profile;
    public:
    DBService() ;
    virtual ~DBService() = default;
    virtual bool run();
    bool init();
    bool handleMsg(CA::CAMessageBlock* pMsg);
    bool createConnection(BSDBProfileObject*);
    bool requestDBConnectionHandles(BSCallConfObject*); // shares profile based connection to call request
    bool testSQlQueryAndDBConnection(BSCallConfObject*); // testing function tempory
};

#endif