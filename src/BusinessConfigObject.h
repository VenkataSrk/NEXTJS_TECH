#ifndef CA_BUSINESS_CONFIG_OBJECT_H
#define CA_BUSINESS_CONFIG_OBJECT_H
#include "CAUtilityInternal.h"
#include "BusinessConfigHandler.h"
#include "CADBDriver.h"
#include "BSException.h"
 class SPAWNTASKINFO{
public:
    std::string taskName;
    std::string appName;
    std::string espdb;
    std::string espParamdb;
    std::string hlrdb;
    std::string npoprdb;
    std::string routingdb;
    std::string switchdb;
};
// Call Related Configuration will be maintened
class BSCallConfObject : public CA::CAMessageBlock{
    public:
    
    std::string m_trunkName;
    std::string m_siteCode;
    std::string m_switchCode;
    
    int m_maxChannel;

    std::string m_groupName;
    std::string m_cli;
    std::string m_ddi;
    std::string m_url;
    std::string m_uid;  
    int m_currentAppPos;
    BSERRDEF::BSError m_errorCode;
    //std::list<SPAWNTASKINFO*> m_spawnTaskInfo;
    std::unique_ptr<CA::CAMap<SPAWNTASKINFO*>> m_spawnTaskInfo;
    std::unique_ptr<CA::CAVector<std::string>> m_taskName;

    // DB handle details
    using DBINT = CA::CAMap<CA::DBInterface*> ;
    std::unique_ptr<CA::CAMap<DBINT*>> m_db;
    public:
    BSCallConfObject()=default;
    virtual ~BSCallConfObject()=default;
    BSCallConfObject(const BSCallConfObject&);

};
class BSGlobalConfObject : public CA::CAMessageBlock{
public:
BSGlobalConfObject()=default;
virtual ~BSGlobalConfObject()=default;
std::string m_bsListenPort;
std::string m_callTimeOut;


};
class BSDBProfileObject : public CA::CAMessageBlock{
    public:
    BSDBProfileObject()=default;
    virtual ~BSDBProfileObject() = default;
    CA::CAMap<DBProfile*> m_dbProObject;
    void copyDBProfilList(CA::CAMap<DBProfile*> *m_dblist);
    void getDBProfilDetails();


};
class BSConfigMsgBlock : public CA::CAMessageBlock{
    public:
    BSConfigMsgBlock()=default;
    virtual ~BSConfigMsgBlock() =  default;
    std::unique_ptr<BusinessConfig> m_configDetails;


};

class BSConfigMonitor : public CA::CAMessageBlock
{
    public:
        BSConfigMonitor()=default;
        virtual ~BSConfigMonitor()=default;
        SET(unsigned short, i, Port);
        GET(i, Port);
        SET(unsigned short, i, ConnectionTimeout);
        GET(i, ConnectionTimeout);
        SET(unsigned short, i, ResponseTimeout);
        GET(i, ResponseTimeout);
        SET(STR, str, Ip);
        GET(str, Ip);

    private:
        unsigned short m_iPort;
        unsigned short m_iConnectionTimeout;
        unsigned short m_iResponseTimeout;
        STR m_strIp;
};


#endif
