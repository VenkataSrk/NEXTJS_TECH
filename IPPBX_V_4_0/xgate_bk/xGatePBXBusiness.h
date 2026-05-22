#include "xGateProcessor.h"
#include "SofiaSiphandler.h"
#include "xGateMgMsg.h"
#include "xGateUtil.h"
//#include "dbdriver.h"
#include "xGateDBManager.h"
#include <vector>
//#include <variant>

typedef enum
{
  EN_FETCH_FEATURELIST = 100,
  EN_CALL_CONNECT
}xGateBusinessFeatureEvents;

/**
struct DBQueryResultData
{
	union Data
	{
		struct
		{
			int int_value, y, w, h;
		}int_data;

		struct
		{
			char* string_value;
			//char* pos;
			//char* size;
		} string_data;
	};
};
***/

/****
struct callFeatureParam
{
	std::string 	m_callid;

	// members from Sysstem.conf
	std::string	gm_site_code;
	std::string	gm_switch_code;
	std::string	gm_db_selector;
	std::string	gm_trace_level;
	unsigned int 	gm_dialTimeout;
	std::string	gm_trunk_in;
	std::string	gm_port_in;

	std::string	gm_trunk_out;
	std::string	gm_port_out;

	// database-query members
	bool 		gm_isSwitchBoardCall;
	bool 		gm_isThisCallForwarded_CompanyHours;
	bool 		gm_isThisCallForwarded_AfterHours;
	std::string 	gm_A_domain_id;
	std::string 	gm_user_A_DOMAIN_NAME;

	std::string 	gm_B_domain_id;
	std::string 	gm_user_B_DOMAIN_NAME;

	int		gm_user_company_hour;
	std::string	gm_user_greeting_file_name_user_B;
	std::string	gm_user_greeting_file_name_user_B_After;
	std::string	gm_user_connecting_file_name_user_B;
	int		gm_user_rbt_user_B_type;
	std::string	gm_user_rbt_file_name_user_B;

	callFeatureParam();
	callFeatureParam(const callFeatureParam &rhs);
	callFeatureParam &operator=(const callFeatureParam &rhs);
	
	callFeatureParam(xGateDBObj* callObj);
};

callFeatureParam::callFeatureParam(xGateDBObj* callObj) :
	m_callid(callObj->callid)
{
}
*****/

class xGatePBXBusiness
{
public:
	xGateDBManager* gm_db_manager_pbx;
	xGateDBManager* gm_db_manager_selector;
	xGateDBManager* gm_db_manager_registrar;

	//callFeatureParam* xgPBXBusiness;
	//callFeatureParam xgPBXBusiness(xGateDBObj* callObj);

	xGatePBXBusiness();
	xGatePBXBusiness(xGateDBObj* callObj);
	~xGatePBXBusiness();

	std::string system_conf_getParam(std::string sysconf_param);
	std::string system_conf_getParam_interface_rtp(std::string sysconf_param, char* rtp_interface);
	void fetch_SysconfParams(xGateDBObj* dbObj);
	void fetch_featureListParams_from_DBmanager( xGateDBObj* dbObj, xGateDBManager* dbm);
	void fetch_switchbaordData_from_DBmanager( xGateDBObj* dbObj, xGateDBManager* dbm);
	void fetch_conferenceData_from_DBmanager( xGateDBObj* dbObj, xGateDBManager* dbm);
	void process_switchbaordMainMenuIVRTimeOut( xGateDBObj* dbObj, xGateDBManager* dbm);
	void process_switchbaordEventRequest( xGateDBObj* dbObj, xGateDBManager* dbm);
	void setUserInfo(xGateDBObj* dbObj);
	bool  CheckAndSet_CallForwardingNumber_if_enabled(xGateDBObj* dbObj, bool playCallForwardAnnounce_inEarlyMedia, xGateDBManager* dbm);
	void setDirectUserInfo(xGateDBObj* dbObj);
	void setForkedUserInfo(xGateDBObj* dbObj, int totalNoOfRegisteredUsers);
	void SendAppNotification(const char* from, const char* to, const char* user_cli, const char* device_type,const char* call_type);
	void SendAppNotification(const char* from, const char* to, const char* user_cli, const char* device_type,const char* call_type,const char* iuid);
	void CancelAppNotificationIOS(const char* from, const char* to, const char* user_cli, const char* device_type,const char* call_type,const char* iuid);

	int SendAppNotificationsAll(int for_loop_counter, xGateDBObj* dbObj, int EN_DB_COMBINEDSPREGISTAR_RES, xGateDBManager* dbm);
	void xGatePBXBusiness::db_call_handling(xGateDBObj* dbObj, xGateDBManager* gm_db_manager);
	void xGatePBXBusiness::GetRegistrationDataFromDB(int en_db_combinedspregistar_res, xGateDBObj* dbObj, xGateDBManager* gm_db_manager);

	void set_proxyUserName(xGateDBObj* dbObj, int for_loop_counter);
	void SetMappedUserNumber_ProxyName_SignalAddress_OutInterface(xGateDBObj* dbObj, const char* mappedUserKey, const char*     userNumber, const char* ProxyName, const char* SignalAddress, const char* OutInterface, const char* routeIpAddress);
	void SetMappedUserDeviceType(xGateDBObj* dbObj, const char* mappedUserKey, const char* value);

	void xGatePBXBusiness::db_combined_query_initial( xGateDBObj* dbObj, xGateDBManager* dbm);
	void xGatePBXBusiness::db_combined_query_selector(xGateDBObj* dbObj, xGateDBManager* dbm);
	void xGatePBXBusiness::db_combined_query_swb(xGateDBObj* dbObj, xGateDBManager* dbm);
	void xGatePBXBusiness::db_combined_query_callForward(xGateDBObj* dbObj, xGateDBManager* dbm);
	void xGatePBXBusiness::db_combined_query_conf_info( xGateDBObj* dbObj, xGateDBManager* dbm);
	void xGatePBXBusiness::db_combined_query_conf_Id( xGateDBObj* dbObj, xGateDBManager* dbm);
	void xGatePBXBusiness::db_combined_query_conf_validate_pin( xGateDBObj* dbObj, xGateDBManager* dbm);
	void xGatePBXBusiness::db_combined_query_conf_get_callid_mguid( xGateDBObj* dbObj, xGateDBManager* dbm);
	bool xGatePBXBusiness::PostMessage(xGateSipMsgBlock *SipMsg, XGATEPOSTTYPE destType);
        void xGatePBXBusiness::GetRegisrtationDeatils(xGateDBObj* dbObj, xGateDBManager* dbm);

	void PostMessageToProcessor(xGateDBObj *dbobj);

	void SWITCHBOARD_FEATURE_MAIN_LOOP(xGateDBObj* dbObj);
	void SWB_Company_hour_greeting(xGateDBObj* dbObj);

	void xGatePBXBusiness::Swb_MainMenuIvrTimeout(xGateDBObj* dbObj);
	void xGatePBXBusiness::Conf_MainMenuIvrTimeout(xGateDBObj* dbObj);
	void xGatePBXBusiness::AppNotification_TimerStarted(xGateDBObj* dbObj);
	void xGatePBXBusiness::callHandling_NoOfTimersStarted(bool order, xGateDBObj* dbObj);
	void xGatePBXBusiness::Conf_DTMFDigitTimeout(xGateDBObj* dbObj);
	void xGatePBXBusiness::DialTimeout(xGateDBObj* dbObj);

	// conference 
	void xGatePBXBusiness::START_CONFERENCE_BRIDGE(xGateDBObj* dbObj);

	void processInviteRequest(callInfoParameter cinfoparam); // Inital call process on Invite
        void processSwitchBoardRequest(callInfoParameter cp ); // process switch board event
        void processCallConferenceRequest(callInfoParameter cp); // 

	int xGatePBXBusiness::GetCodecForDeviceType(char* deviceType);

	void xGatePBXBusiness::GetConfId(xGateDBObj *dbObj, xGateDBManager* gm_db_manager, int maxDtmfDigits);
	void xGatePBXBusiness::GetPin(xGateDBObj *dbObj, xGateDBManager* gm_db_manager, int maxDtmfDigits);
	bool xGatePBXBusiness::isIOSDevice(xGateDBObj *dbObj, char* temp_calledDeviceType, char* temp_proxyUserName);
	void xGatePBXBusiness::db_combined_query_DirectNumber(xGateDBObj *dbObj, xGateDBManager* gm_db_manager);
        void xGatePBXBusiness::fetch_HuntGroupData_from_DBmanager(xGateDBObj *dbObj, xGateDBManager* gm_db_manager);

	void populateOneDeviceData(int EN_DB_COMBINEDSPREGISTAR_RES, xGateDBObj* dbObj, xGateDBManager* dbm);
	bool xGatePBXBusiness::checkForCookieChangeInURApp(xGateDBObj* dbObj);
	void xGatePBXBusiness::CheckCookieAndDeleteDeviceAndCancelAppNotification(xGateDBObj* dbObj);
	void GetRegisteredUsersDataFromDB(int EN_DB_COMBINEDSPREGISTAR_RES, xGateDBObj* dbObj, xGateDBManager* dbm);
	void processRegisteredUsersData(int EN_DB_COMBINEDSPREGISTAR_RES, xGateDBObj* dbObj, xGateDBManager* dbm);
	void xGatePBXBusiness::sendAppNotificationToURAPPDevices(xGateDBObj* dbObj);
	inline std::string  createmappedUserKey(xGateDBObj* dbObj, char* calledDeviceType);
        void updateMappedUserList(xGateDBObj* dbObj);
	void xGatePBXBusiness::TrimLeadingZeroPlus(xGateDBObj* dbObj);
	void xGatePBXBusiness::deleteConferenceDBRecords(xGateDBObj* dbObj, xGateDBManager* gm_db_manager);
        void GetAddressofrecords(xGateDBObj* dbObj);
        void SetRegisterRecordFromDB(xGateDBObj* dbObj,std::string dbOut);
        void jsonParser(xGateDBObj* dbObj,std::string dbOutput);
        void populateData( xGateDBObj* dbObj);
        void populateTransferData( xGateDBObj* dbObj);
        void checkCalledNumberType(xGateDBObj *dbObj, xGateDBManager* gm_db_manager);
        void switchBoardProcess(xGateDBObj *dbObj, xGateDBManager* gm_db_manager);
	void xGatePBXBusiness::db_combined_query_get_pstn_operator_details(xGateDBObj* dbObj, xGateDBManager* gm_db_manager);
};

