#ifndef BUSINESS_SWITCHING
#define BUSINESS_SWITCHING
#include "BSInterface.h"
#include "BSCDS.h"

class BSSwitching : public BSInterface
{
    public:
        BSSwitching()=default;
        virtual ~BSSwitching();
        BSSwitching(BSParam* bsp, std::string profile);
        void postToCDS();
        std::string xlatnb;

        retCode InitiateSwitching(CA::CAMap<CA::DBInterface*>& dblist, BSParam* bsp);
        std::string ProcPrefix(std::string newprefix,std::string prefix, std::string nb);
        XLatCallOut m_dbes5XlatCallout;
        template <typename T>
        inline std::string to_string(const T& x) {
            std::ostringstream ss;
            ss << x;
            return ss.str();
        }
        std::time_t m_callConnect;
        std::time_t m_callDisconnect;

    private:
        std::string m_strSiteCode;
        std::string m_strCli;
        std::string m_strDdi;
        std::string m_strSwitchingProfile;
        std::string m_strTrunkName;
        std::string m_strSwitchCode;
        std::string m_stroutGoingGroup;
        std::string m_strSwitchingSitecode;
        std::string m_strCliAdd;
        std::string m_strDdiAdd;
        std::string m_strCRF; // call reference ID
        std::string m_strInterTrunkgroup;
        std::string m_strTrace;
        std::string m_strAppID;
        // DB Profiles
        //retCode DB_es3_getxlat(CA::DBInterface* dbInt,std::string sitecode, std::string trunk, std::string &didnum);
        retCode DB_get_outTrunkInterface(CA::DBInterface* dbInt,std::string sitecode, std::string group);
        retCode DB_es5_xlat_callout(CA::DBInterface* dbInt,std::string sitecode, std::string switchcode,std::string didnum, std::string trunk, std::string group);
        retCode DB_es5_get_config(CA::DBInterface* dbInt,std::string trunk, std::string didnum);
        retCode DB_cs_get_xlatout(CA::DBInterface* dbInt,std::string sitecode, std::string switchcode,std::string trunk, std::string &cli, int num1, int num2);

        
};

#endif
