#ifndef MVNO_BUSINESS_SERVICE
#define MVNO_BUSINESS_SERVICE
#include "CAUtilityInternal.h"
#include "BusinessConfigObject.h"
#include "BSCallObject.h"
class BSRawRequest{
    public:
    std::string m_strUid;
    std::string m_strJson;
    std::string m_strIP;
    int port;
    int state; 
};
class MVNOBusinessService : public CA::CAThread{
private:
bool m_serviceStatus;
std::unique_ptr<CA::CASocket> m_serverSocket;
std::unique_ptr<BSGlobalConfObject> m_bsConfObj;
CA::CAMap<BSRawRequest*> m_rawRequest;
public:
MVNOBusinessService()=default;
~MVNOBusinessService()=default;
inline void setServiceStatus(bool status){
    m_serviceStatus =  status;
}
inline bool getServiceStatus(){
    return m_serviceStatus;
}
virtual bool run();
bool init();
bool handleMsg(CA::CAMessageBlock* pMsg);
bool getConfigurationDetails(CA::CAMessageBlock* pMsg);
bool initServerSocket();
bool receiveSocketData(CA::CAMessageBlock* pMsg);
bool sendSocketData(CA::CAMessageBlock* pMsg);
bool loadBSRequestProcessor(CA::CAMessageBlock* pMsg);
bool postCallMessage(BSParam* pbsp);
bool insertRequest(BSRawRequest*);
BSRawRequest* getRequest(std::string uid);
bool deleteRequest(std::string uid);

};
#endif