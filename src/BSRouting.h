#ifndef BUSINESS_ROUTING
#define BUSINESS_ROUTING
//#include "BSInterface.h"
#include "BSSwitching.h"
#include "BSCRR.h"
class BSRouting: public BSInterface{
      public:
        BSRouting()=default;
        virtual ~BSRouting();
        BSRouting(BSParam* bsp ,Application app);
        void postToCRR();
	void UnLoad(BSParam* bsp,std::time_t connecttime,std::time_t disconnecttime,std::time_t starttime);
        retCode InitiateRouting(CA::CAMap<CA::DBInterface*>& dblist, BSParam* bsp);
	retCode GetAlternateRoute(CA::CAMap<CA::DBInterface*>& dblist, BSParam* bsp);
	void setUserInfo(BSParam* bsp);
        void setPrivateInfo(BSParam* bsp);
        
        std::string ProcPrefix(std::string newprefix,std::string prefix, std::string nb);
        retCode DirectSwitching(CA::CAMap<CA::DBInterface*>& dblist,BSParam* bsp,int recPos);
        retCode DirectSwitchingBypass(CA::DBInterface* dbInt,BSParam* bsp,int recPos);
        template <typename T>
        inline std::string to_string(const T& x) {
            std::ostringstream ss;
            ss << x;
            return ss.str();
        }
        
        
        Es3Xlat m_es3xlat;
        MRouteSharedData m_routingSharedData;
        PrefixRefData m_prefixRefData;
        CRouteCodecData m_routeCodecData;
        R25GetRoute m_r25GetRoute;
        XLatCallOut m_xlatCallout;
        TdmIpAddress m_tdmipaddress;
        CodecDetails m_codecdetails;
        std::vector<R25GetRoute*> validRoute;
        std::string m_strCRF; // call reference ID
        std::string m_strInterTrunkgroup; //  between applications(PIPER,PIPES etc)
        std::time_t m_callConnect;
        std::time_t m_callDisconnect;	
    private:
        std::string m_strSiteCode;
        std::string m_strCli;
        std::string m_strDdi;
        std::string m_strRoutingProfile;
        std::string m_strSwitchingProfile;
        std::string m_strTrunkName;
        std::string m_strSwitchCode;
        std::string m_strGradeCode;
        int m_RouteCount;
        std::unique_ptr<BSSwitching> m_switchPtr;
        std::string m_strTrace;
        std::string m_strAppID;
    
        // Db profile 
        retCode DB_es3_getxlat(CA::DBInterface* dbInt,std::string sitecode, std::string trunk, std::string &didnum);
        retCode DB_sl_checkddiblocked(CA::DBInterface* dbInt,std::string didnum);
        retCode DB_r01_get_prefix_ref(CA::DBInterface* dbInt,std::string didnum);
        retCode DB_cr_get_operatorset(CA::DBInterface* dbInt,int num);
        retCode DB_r25_get_route(CA::DBInterface* dbInt,std::string mnpNum, std::string group,
            std::string tmp1, std::string sitecode, int num1, std::string tmp2, int num2);
        retCode DB_esp_get_routelimit(CA::DBInterface* dbInt,std::string sitecode, std::string mnpNum);
        retCode DB_mroute_get_tdm_ip_address(CA::DBInterface* dbInt,std::string sitecode,std::string switchcode,std::string trunckcode,std::string mnpNum);
        retCode DB_croute_get_codec_details(CA::DBInterface* dbInt,std::string group);
        retCode DB_esp_getxlat_interdomain (CA::DBInterface* dbInt,std::string siteout, std::string trunkout, std::string inxlat,std::string &xlatout);
	retCode DB_r25_get_india_did_route(CA::DBInterface* dbInt,std::string mnpNum);

        unsigned int GetRouteAccessIndex(std::string accessType);  

    
};
#endif
