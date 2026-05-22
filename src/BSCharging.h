#ifndef BUSINESS_CHARGING
#define BUSINESS_CHARGING
//#include "BSInterface.h"
#include "BSRouting.h"
#include "BSCDR.h"

enum class packageType{
	PT_MAS = 0,
	PT_BUN
};
class ChargingDetails{
	public: 
	ChargingDetails(){
		m_allocUnit = 0.0;
		m_totalAllocUnit = 0.0;
		m_sharedDelay = 0;
		m_nextChargingTime = 0;
		m_totalCharedTime = 0;
		m_packChargedTime = 0;
		m_bundleCycleCount = 0;
		m_cnxUnitAdded =  false;
		
	};
	virtual ~ChargingDetails() = default;
	double m_allocUnit; // allocate Balance on single package
	double m_totalAllocUnit; // allocate balance on all package
	int m_sharedDelay; // allocated pulse rate
	int m_nextChargingTime; // next call time in sec for charging
	int m_totalCharedTime; // total talk time in all package
	int m_packChargedTime; // total talk time in each package
	int m_bundleCycleCount; // no of cycle in  each bundle 
	bool m_cnxUnitAdded;
	
	


};
class BSCharging: public BSInterface{
    private:
    std::string m_strSiteCode;
    std::string m_strCli;
    std::string m_strDdi;
    std::string m_strAccountProfile;
    std::string m_strParamProfile;
    std::string m_strNpOprProfile;
    std::string m_strCcaasProfile;
    std::string m_strTrunkName; // initial trunk entry
    std::string m_strTrace;	
    std::string m_strSwitchCode;
	std::string m_strSharedDelay;
    CA::DBMAP m_dbPackByMaster;
	std::string m_strXlatCli;
	std::string m_strXlatDdi;
	std::time_t m_packageConnectedTime;
	packageType m_pt;
	std::unique_ptr<BSRouting> m_rsInterface;
	int m_iPackByMasRowID ;
	std::string m_strAppID;
	 
	int m_isPbx;
	std::string m_shortCli;
	
	
    private: // member function
    retCode getAccountDetails(CA::DBInterface*,BSParam*);
    retCode validateAccount(CA::DBInterface*,CA::DBInterface*,CA::DBInterface*,BSParam*);
    retCode verifyCallInParam();                   
	retCode onActiveAccount(CA::DBInterface*,CA::DBInterface*,CA::DBInterface*,BSParam*);
	retCode verifyCardFeature(CA::DBInterface*);
	retCode InitiateCharging(CA::DBInterface*,CA::DBInterface* dbParamInt); // validate and start pre paid charging before connect 
	retCode findBundlePackage(CA::DBInterface*,CA::DBInterface* dbParamInt);
	retCode findMasterPackage(CA::DBInterface*,CA::DBInterface* dbParamInt);
	retCode updateBundlePackage(CA::DBInterface*,CA::DBInterface* dbParamInt);
	retCode updateMasterPackage(CA::DBInterface*,CA::DBInterface* dbParamInt);
    retCode revertCharging(CA::DBInterface*,CA::DBInterface* dbParamInt);

    retCode getAccountBalance(CA::DBInterface*,std::string teleCode, std::string custCode, int bachCode, int serialCode);
    retCode getPackageByMasterData(int rowID,PackageByMaster& );
    std::string checkRedirection(BSParam*);
    std::string prefixNumber(CA::DBInterface*,CALL_TYPE callType,std::string trunkName, std::string number, int direction);
	retCode bundlePackageCharging(CA::DBInterface* dbInt,CA::DBInterface* dbParamInt)  ;
	retCode masterPackageCharging(CA::DBInterface* dbInt,CA::DBInterface* dbParamInt);
	retCode CalculateChargingCycle(GetTariff);
	retCode calculateCallUnit(GetTariff);
	int getPackByMasRowID(){return m_iPackByMasRowID;}
	void setPackByMasRowID(int id){m_iPackByMasRowID = id;}
	
	


	
    
    /*----------------------------------------------------------------*/
    retCode DB_es6_GetParamDb(CA::DBInterface*);
    retCode DB_es5_get_access(CA::DBInterface*,BSParam*);
    retCode DB_esp_getaccount_mvno_v2(CA::DBInterface*,std::string cli);
    retCode DB_es2_account_by_cbs(CA::DBInterface*,std::string teleCode, std::string custCode, int bachCode, int serialCode,AccountByCbs& accbyCBS);
    retCode DB_esp_get_mvnosubscriber_info(CA::DBInterface*,std::string cli);
    retCode DB_esp_get_promptpath_byMVNO(CA::DBInterface*,std::string sitecode, std::string cli);
    retCode DB_es5_get_xlat(CA::DBInterface*,std::string sitecode,std::string number,std::string trunkcode, int direction, std::string switchcode,int nbtype,int xlatSet);
    retCode DB_mnp_get_call_forward(CA::DBInterface* dbInt,std::string trunkcode,std::string didnum, std::string did_nbtype, std::string xlatednum);
	retCode DB_esp_getdest_byMVNO_did(CA::DBInterface* dbInt,std::string number, std::string xlatenum);
	retCode DB_es2_get_batch(CA::DBInterface* dbInt,std::string telco_code, std::string cust_code, int batch_code);
	retCode DB_esp_get_cardfamily(CA::DBInterface* dbInt,int card_id);
	retCode DB_esp_get_cardfeatures(CA::DBInterface* dbInt,int cardId,CA::DBMAP& dbcardfeatures);
	retCode DB_esp_getmvno_notif_config(CA::DBInterface* dbInt,std::string sitecode, int card_id, int mvno_brandid,
		 std::string tariff_class);
	retCode DB_es1_get_tariffclass(CA::DBInterface* dbInt,std::string telco_code, std::string sitecode, 
		std::string tariff_class);
	retCode DB_esp_get_maintenancecharge2(CA::DBInterface* dbInt,std::string telco_code, std::string sitecode, 
		std::string tariff_class, double accountbal);
	retCode DB_es1_get_alttariffclass(CA::DBInterface* dbInt,std::string telco_code, std::string sitecode, 
		std::string tariff_class);
	retCode DB_esp_get_exchangerate(CA::DBInterface* dbInt,std::string curr_code, std::string subres_currcode);
	retCode DB_account_expiry_charge(CA::DBInterface* dbInt,std::string cust_code, int batch_code, int serial_code);
	retCode DB_esp_get_topup_info(CA::DBInterface* dbInt,std::string cust_code, int batch_code, int serial_code);
	retCode DB_es_get_pack6_info(CA::DBInterface* dbInt,std::string cust_code, int batch_code, int serial_code);
	retCode DB_esp_get_package_bymaster(CA::DBInterface* dbInt,std::string telco_code, std::string cust_code,
	 					int batch_code, int serial_code);
	retCode DB_esp_check_special_number(CA::DBInterface* dbInt,int cardid, std::string rawDDI, int nbtype);
	retCode DB_MNP_get_info(CA::DBInterface* dbInt,std::string nb);
	retCode DB_esp_xlat_bybrand(CA::DBInterface* dbInt,std::string mnpNum);
	retCode DB_es3_get_prefix(CA::DBInterface* dbInt,std::string sitecode, std::string destnum, int prefix_set);
	retCode DB_esp_get_tariffcode(CA::DBInterface* dbInt,std::string destnum);
	retCode DB_esp_checkroaming_dest(CA::DBInterface* dbInt,std::string sitecode, std::string dest_code, std::string xlatednum);
	retCode DB_esp_is_prefixgroup_blocked_bycli(CA::DBInterface* dbInt,std::string a_cli, std::string xlatednum);
	retCode DB_es_isprefixblocked(CA::DBInterface* dbInt,std::string xlatednum);
	retCode DB_es1_get_premiumnb(CA::DBInterface* dbInt,std::string nb);
	retCode DB_es2_gettimeclass(CA::DBInterface* dbInt,std::string sitecode, std::string tariff_class);
	retCode DB_es2_get_tariff(CA::DBInterface* dbInt,std::string telco_code, std::string sitecode, 
		std::string dest_code, std::string tariff_class, int access_charge, int time_prd, int routeclass,GetTariff &getTariffInfo);
	retCode DB_es1_get_progsampl(CA::DBInterface* dbInt,std::string telco_code, std::string site_code, std::string mast_tariff_class,std::string dest_code, int charge, int period, int talktime);
	retCode DB_esp_get_package_exception(CA::DBInterface* dbInt,std::string sitecode, int mvno_brandid, 
		std::string tariff_class, int package_id, std::string dest_code, std::string xlated_num);
	retCode DB_esp_package_authorize_dest(CA::DBInterface* dbInt,std::string telco_code, std::string cust_code,
	int batch_code, int serial_code, std::string m_custcode, int m_batchcode, int m_serialcode,
	int package_id, std::string destMSISDN);
	retCode DB_es5_update_balance_infront(CA::DBInterface* dbInt,std::string telco_code, std::string cust_code,
	int batch_code, int serial_code, double v_allocbal, double account_cnxcount, int nof_charge, 
	std::string switchcode, std::string task_id);
        retCode DB_es5_update_ccaas_balance_infront(CA::DBInterface* dbInt, BSParam* bsp);
	retCode DB_esp_update_packagelimit_v2(CA::DBInterface* dbInt,std::string cust_code,
	int batch_code, int serial_code,int limit_mode,int limit_delay,double v_allocabal,int v_min);
	retCode DB_esp_get_package_prefixdest(CA::DBInterface* dbInt,std::string telco_code, 
	std::string cust_code, int batch_code, int serial_code, std::string a_cli, 
	std::string &inxlatednum);
	retCode DB_es1_max_sampunit(CA::DBInterface* dbInt,std::string tele_code, std::string sitecode, std::string tariffclass,std::string dest_code , int access_charge, int time_prd);
	retCode DB_esp_get_balance_infront(CA::DBInterface* dbInt,std::string sitecode,std::string switchcode, int task_id, std::string telco_code, std::string cust_code, int batch_code, int serial_code,int conf_talkcharge);
	retCode DB_esp_package_authorize_dest_insertlog(CA::DBInterface* dbInt,std::string telco_code, std::string cust_code,
	int batch_code, int serial_code, std::string m_custcode, int m_batchcode, int m_serialcode,
	int package_id, std::string destMSISDN);
	retCode DB_esp_package_bundle_changedate(CA::DBInterface* dbInt,std::string sitecode,std::string switchcode, int task_id, std::string telco_code, std::string cust_code, int batch_code, int serial_code,int resmon_packid,int usertype,std::string xlatednum);
	retCode DB_esp_get_sms_campaign(CA::DBInterface* dbInt,std::string sitecode,std::string mvno_phone,std::string xlated_num);	
	retCode DB_es1_get_activation(CA::DBInterface* dbInt,std::string telco_code, std::string cust_code, 
					int batch_code, int serial_code);
	retCode DB_esp_get_package_prefixdest_bydid(CA::DBInterface* dbInt,std::string telco_code, 
		std::string cust_code, int batch_code, int serial_code, int packid, int prefixdestMode, 
		std::string prefixdest, std::string cli, std::string &inxlatednum);
	retCode DB_customquery(CA::DBInterface* dbInt,std::string dbProfileName, std::string dbTable, 
				std::string selQuery);
	retCode DB_esp_get_packagelimit_v2(CA::DBInterface* dbInt,std::string cust_code,int batch_code,int serial_code,
				int limit_mode,int limit_delay,double limit_amount,int limit_minute,int limit_call);
	// for PBX we are introduce below SP's
	retCode DB_PBX_esp_es2_get_ani(CA::DBInterface* dbInt,std::string sitecode, std::string didnum, std::string cli);
    retCode DB_PBX_esp_es1_get_clicat(CA::DBInterface* dbInt,std::string sitecode,std::string trunkName,std::string cli);
	retCode CheckPBXCall(CA::DBInterface* dbInt,BSParam*);



    

    public: // member variable
    Access m_access;
    AccountMvnoV2 m_accountMvnoV2;
    AccountByCbs m_accountByCBS;
	AccountByCbs m_accountByCBSBundle;
    MvnoSubscriberInfo m_mvnoSubsInfo;
    Xlat m_xlat;
    CARD_FEATURE m_cardFeature;
    TariffClass m_tariffClass;
    topupTariff m_topupTariff;
    PackTariff m_packTariff;
    PackageByMaster m_packByMaster;
    MnpDetails m_mnpDet;
    byBrandXlat m_byBrandxlat;
    TariffCode m_tariffCode;

    GetTariff m_getTariffMas;
	GetTariff m_getTariffBundle;

    UpdateBalanceInfront m_updateBalInFront;
    Activation m_activation;
    PackageLimitV2 m_packageLimitv2;
	TimeClass m_timeClass;
	GetAuthorizeDest m_authDest;
	std::string m_strCRF; // call reference ID
	ChargingDetails m_charDt;
	Es2AniInfo m_es2aniinfo;
	Es1ClicatInfo m_es1clicatinfo;
    public:
    BSCharging()=default;
    virtual ~BSCharging();
 //   BSCharging(BSCallConfObject* bsc);
	BSCharging(BSParam* bsp ,Application app);
    std::string getAccountProfile(){return m_strAccountProfile;}
    std::string getParamProfile(){return m_strParamProfile;}
	void setPackageType(packageType pt) {m_pt = pt;}
	packageType getPackageType(){ return m_pt;}
	
    
    retCode InitCharingAccount(CA::CAMap<CA::DBInterface*>&,BSParam*, Application); // Load the account DB and call Info details
	retCode StartCharging(CA::CAMap<CA::DBInterface*>&,BSParam*); // On call connected
	retCode StopCharging(CA::CAMap<CA::DBInterface*>&,BSParam*,std::time_t ,std::time_t, std::time_t, int /*revert or charge*/); // On Call Disconnected
	retCode CharingCycle(CA::CAMap<CA::DBInterface*>&,BSParam*); // In Call on every plus trigger
	retCode FetchAlternateRoute(CA::CAMap<CA::DBInterface*>& dblist, BSParam* bsp,Application app);
        bool ClearMasterDBData();
	
	
		
	
};
#endif
