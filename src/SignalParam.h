#ifndef SIGNAL_PARAM_H
#define SIGNAL_PARAM_H
#include "CAUtilityInternal.h"
enum class BSCallDirection{
    DIR_UNKOWN=0,
    DIR_IN,
    DIR_OUT,
    
};
enum class BSCallState{
STATE_UNKNOWN=0,
STATE_INIT,
STATE_PROCESSING,
STATE_DIALING,
STATE_PROCEEDING,
STATE_ALERTING,
STATE_CONNECTED,
STATE_DISCONNECTED,
STATE_TERMINATE,
STATE_ROUTE_ERROR,
STATE_CLEAR
};

enum class CauseCode
{
    CC_UNASSIGNED=0,
    CC_UNALLOCATEDNUMBER=1,
    CC_NOROUTETOTRANSITNETWORK=2,
    CC_NOROUTETODESTINATION=3,
    CC_CONNECTED=7,
    CC_PROCEEDING=8,
    CC_PORTEDOUTNUMBER=14,
    CC_NORMALCALLCLEARING=16,
    CC_USERBUSY=17,
    CC_NOUSERRESPONDING=18,
    CC_NOANSWERFROMUSER=19,
    CC_ABSENT_SUBSCRIBER=20,
    CC_EXCHAGEROUTINGERROR=25,
    CC_NORMALUNSPECIFIED=31,
    CC_NOCIRCUITAVAILABLE=34,
    CC_NETWORKOUTOFORDER=38,
    CC_TEMPORARYFAILURE=41,
    CC_SWITCHINGEQUIPMENTCONGESTION=42,
    CC_REQUESTEDCIRCUITUNAVAILABLE=44,
    CC_CALLBARRING=54,
};

enum class CallNumberType
{
    CNT_UNKNOWN=0,
    CNT_INTERNATIONAL=1,
    CNT_NATIONAL=2,
    CNT_NETWORKSPECIFIC=3,
    CNT_SUBSCRIBERNUMBER=4,
    CNT_ABBREVIATED=6,
};

class BSParam{
    
    // memeber varibale
    private:

    BSCallState m_state;
    int m_bsState;
    int m_errCode;
    int m_switchCode;
    int m_connectionState;
    BSCallDirection m_callDirection;
    CauseCode m_causeCode;
    std::string m_uid;
    std::string m_cli;
    std::string m_ddi;
    std::string m_cliAddr;
    std::string m_ddiAddr;
    std::string m_lastKnownError;
    std::string m_rn;
    std::string m_trunkName;
    std::string m_trace;
    std::string m_interTrunkgroup;
    std::string m_siteCode;
    std::string m_caIP;
    std::string m_bsIP;
    std::string m_iwmscPrefix;
    std::string m_mvnoPrefix;
    std::string m_HlrUrl;
    std::string m_uinfo;
    std::string m_destCode;
    std::string m_pinfo;
    std::string m_tarrifClass;
    double m_callCost;
    double m_unitsCharged;
    int m_isPBXCall;
    bool m_isCcaas;
    std::string m_shortCli;
    std::string m_RedirectingNumber;
    std::string m_callConnectedTime;
    std::string m_callDisconnectedTime;
    std::string m_strBusinessRequestTime;
    std::string m_strBusinessResponseTime;
    
    
    public:
    BSParam()=default;
    virtual ~BSParam()=default;
    BSParam(BSParam&);
    void setCallDirection(BSCallDirection bsCD){m_callDirection =  bsCD;}
    void setCauseCode(CauseCode bsCC){m_causeCode = bsCC;}
    void setState(BSCallState state){m_state = state;}
    void setconnectionState(int state){m_connectionState = state;}
    void setErrorCode(int code){m_errCode =  code;}
    void setSwitchCode(int code){m_switchCode =  code;}
    void setUid(std::string str){m_uid = str;}
    void setCli(std::string str){m_cli =  str;}
    void setDdi(std::string str){m_ddi =  str;}
    void setCliAddr(std::string str){m_cliAddr =  str;}
    void setDdiAddr(std::string str){m_ddiAddr =  str;}
    void setRN(std::string str){m_rn =  str;}
    void setTrunkName(std::string str){m_trunkName = str;}
    void setSiteCode(std::string str){m_siteCode =  str;}
    void setCallAgentIP(std::string str){ m_caIP = str;}
    void setBSIP(std::string str){ m_bsIP= str;}
    void setDestcode(std::string str){m_destCode= str;}
    void setInterTrunkGroup(std::string str){m_interTrunkgroup = str;}
    void setIwmscPrefix(std::string str){m_iwmscPrefix = str;}
    void setMvnoPrefix(std::string str){m_mvnoPrefix = str;}
    void setHlrUrl(std::string str){m_HlrUrl =  str;}    
    void setBSstate(int st) { m_bsState =  st;}
    void setLastKnownError(std::string err){m_lastKnownError =  err;}
    void setTrace(std::string str){m_trace = str;}
    void setUinfo(std::string str){m_uinfo = str;}
    void setPinfo(std::string str){m_pinfo = str;}
    void setcallCost(double cost){m_callCost = cost;}
    void setUnitsCharged(double units){m_unitsCharged = units;}
    void setRedirectingNumber(std::string str){m_RedirectingNumber = str;}
    void setCallConnectedTime(std::string str){m_callConnectedTime =  str;}
    void setCallDisconnectedTime(std::string str){m_callDisconnectedTime = str;}
    void setTarrifClass(std::string str){m_tarrifClass = str;}
    void setBusinessRequestTime(std::string str){m_strBusinessRequestTime = str;}
    void setBusinessResponseTime(std::string str){m_strBusinessResponseTime = str;}
    void setIsPbxCall(int call){m_isPBXCall = call;}
    void setShortCli(std::string str){m_shortCli = str;}
    void setIsCcaasCall(bool val){m_isCcaas = val;}

    BSCallDirection getCallDirection(){return m_callDirection;}
    CauseCode getCauseCode(){return m_causeCode;}
    int getconnectionState(){return m_connectionState;}
    BSCallState getState(){return m_state;}
    int getErrorCode(){return m_errCode;}
    int getSwitchCode(){return m_switchCode;}
    std::string getStrSwitchCode(){ return std::to_string(m_switchCode);}
    std::string getUid(){return m_uid;}
    std::string getCli(){return m_cli;}
    std::string getDdi(){return m_ddi;}
    std::string getCliAddr(){return m_cliAddr;}
    std::string getDdiAddr(){return m_ddiAddr;}
    std::string getRN(){return m_rn;}
    std::string getTrunkName(){return m_trunkName;}
    std::string getSiteCode(){return m_siteCode;}
    std::string getCallAgentIP(){return m_caIP;}
    std::string getBSIP(){return m_bsIP;}
    std::string getDestcode(){return m_destCode;}
    std::string getInterTrunkgroup(){return m_interTrunkgroup;}
    int getBSstate(){return m_bsState;}
    std::string getIwmscPrefix(){return m_iwmscPrefix;}
    std::string getMvnoPrefix(){return m_mvnoPrefix;}
    std::string getHlrUrl(){return m_HlrUrl;}
    std::string getLastKnownError(){return m_lastKnownError;}
    std::string getTrace(){return m_trace;}
    std::string getUinfo(){return m_uinfo;}
    std::string getPinfo(){return m_pinfo;}
    double getcallCost(){return m_callCost;}
    double getUnitsCharged(){return m_unitsCharged;}
    std::string getRedirectingNumber(){return m_RedirectingNumber;}
    std::string getCallConnectedTime(){return m_callConnectedTime;}
    std::string getCallDisconnectedTime(){return m_callDisconnectedTime;}
    std::string getTarrifClass(){return m_tarrifClass;}
    int getIsPbxCall(){return m_isPBXCall;}
    std::string getShortCli(){return m_shortCli;}
    bool getIsCcaasCall(){return m_isCcaas;}

    // Non Memeber variable action functions
    bool decodeJson(const char* josnString);
    std::string encodeJson();
    Value setJsonParam(STR strValue,Document &doc);
	Value setJsonParam(unsigned int iValue, Document &doc);
    void setBSStateFromCallState(BSCallState bsState);
    void updateBSParam(BSParam& bsParam);
    std::string parseTelephoneNumber(std::string str);
    
};
#endif
