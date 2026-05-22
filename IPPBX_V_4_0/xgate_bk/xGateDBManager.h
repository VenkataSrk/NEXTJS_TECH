#ifndef XGATE_DB_MANAGER
#define XGATE_DB_MANAGER
#include "xGateProcessor.h"
#include "SofiaSiphandler.h"

// #ifdef XGATE_HAS_BUSINESS_SUPPORT
// #include "xGateBusinessMsg.h"
// #endif

#include "RegisDBConnector.h"

#ifdef XGATE_HAS_BUSINESS_SUPPORT
typedef enum
{
  EN_DOMAIN_ID = 1,           //domain_id
  EN_IS_SWB = 2,              //is_switch_board 
  EN_SWB_DDI = 3,             //switch board did
  EN_SWB_DOMAIN_ID = 4,       //switch board domain id
  EN_DIRECTORY_ENABLED = 5,   //directory enabled
  EN_IS_HUNT_GROUP = 6,       //
  EN_FIND_FOLLOW_ME_NUM = 7,
  EN_IS_CONFERENCE = 8,
  EN_VOICE_MAIL = 9,
  EN_INTERNAL_FOUND = 10,
  EN_HUND_GROUP_DOMAIN_ID = 11,
  EN_IS_DIRECT_CALL_STATUS = 12,
  EN_IS_DIRECT_CALL_PREQ = 13,
  EN_DIRECT_CALL_PIN = 14,
  EN_LEG_A_DESKPHONE_STATUS = 15,
  EN_LEG_B_DESKPHONE_STATUS = 16,
  EN_BLOCK_TYPE = 17,
  EN_BLOCKED_NUM_IVR = 18,
  EN_IS_NUM_BLOCKED = 19,
  EN_CALLER_ID = 20,
  EN_CALL_REC_OUT_CLI = 21,
  EN_CALL_REC_OUT_DDI = 22,
  EN_ONDEMAND_REC_START_IVR = 23,
  EN_ONDEMAND_REC_STOP_IVR = 24,
  EN_USER_GREETING_FILE = 25,
  EN_USER_CALL_SCREENING_TYPE = 26,
  EN_DDI_USER_HOLD_MUSIC = 27,
  EN_CLI_USER_HOLD_MUSIC = 28,
  EN_COMPANY_HOUR = 29,
  EN_AFTER_GREETING_FILE = 30,
  EN_CALL_SCREENING_TYPE = 31,
  EN_CONNECTING_IVR = 32,
  EN_AUDIO_CONNECTION_TYPE = 33,
  EN_AUDIO_CONNECTING_FILE = 34,
  EN_DID_USER_HOLD_MUSIC_TYPE = 35,
  EN_CLI_USER_HOLD_MUSIC_TYPE = 36,
  EN_DID_EXT = 37,
  EN_CLI_USER_VIDEO_GREETING = 38,
  EN_DID_USER_VIDEO_GREETING = 39,
  EN_HOST_CODE = 40,
  EN_PARTICIPANT_CODE = 41,
  EN_IS_EMERGENCY = 42,  
  EN_DOMAIN_NAME = 43,        //domain_name
  EN_CLI_REC_FLAG = 44,
  EN_DID_REC_FLAG = 45,
  EN_ERR_MSG = 46,
  EN_IS_VOICEMAIL_DID = 47,
  EN_IS_VOICEMAIL_CLI = 48,
  EN_VOICEMAIL_PIN = 49,
  EN_IS_VOICEMAIN_PIN_NEED = 50,
  EN_EXTERNALNB_DOMAIN_NAME = 51,
  EN_EXTERNALNB_EXTN = 52,
  EN_EXTERNALNB_EXTN_PWD = 53,
  EN_EXTERNALNB_DOMAIN_ID = 54,
}CombinedQueryFieldIndex;
#endif

class xGateDBManager : public xGateProcessor {
  public:
    DB_Connection* db_connection;
    DB_Connection* db_connection_pbx;
    DB_Connection* db_connection_selector;
    DB_Connection* db_connection_registrar;

    typedef ACE_Hash_Map_Manager<ACE_TString, xGateDBObj*, ACE_SYNCH_RW_MUTEX> HASH_CALL_MAP;
    HASH_CALL_MAP m_dbregHashTable ;

    xGateDBManager();
    virtual ~xGateDBManager();
    virtual int svc( void );
    virtual xGateRetVal process_msg(ACE_Message_Block*);

    xGateRetVal stop();
    bool m_run; 

    xGateRetVal xGateDBManager::handle_regis_process(xGateSofiaSipRegobj *pSipRegobj);
    xGateRetVal xGateDBManager::handle_db_process(xGateDBObj *pSipDBObj);

// #ifdef XGATE_HAS_BUSINESS_SUPPORT
//     //member variables
//     string m_uid;
// 
//     //member functions
//     bool parseJsonMsg(const char *jsonData, Document &doc);
//     bool getInputDoc(Document &doc, const Value &inputVal);
//     bool process_business_request(xGateBusinessMsg *pMsg);
//     bool do_prequery_request(Document &doc, const Value &inputVal);
//     bool get_prequery_result(HSTMT hstmt, string &output); 
//     bool do_aor_request(Document &doc, const Value &inputVal);
//     bool post_result(Document &doc, string &output);
// 
//     bool handle_db_response(DbmReqDetail &reqDetail);
// #endif

    std::string xGateDBManager::DATABASE_QUERY_GET_DOMAIN_ID(char* domainName);
    std::vector<DBQueryResultData>* xGateDBManager::DATABASE_QUERY_COMBINED(const char* user_a, const char* user_b,const char* domainName);
    std::vector<DBQueryResultData>* xGateDBManager::DATABASE_QUERY_SWB_COMBINED(const char* domain_id, const char* switchboard_number);

    std::string xGateDBManager::DATABASE_QUERY_COMBINED_StringValue_test(char* user_a, char* user_b,char* domainName, int DbColumnNumber);
    int xGateDBManager::DATABASE_QUERY_COMBINED_IntegerValue_test(const char* user_a, const char* user_b,const char* domainName, int DbColumnNumber);
    std::string xGateDBManager::DATABASE_QUERY_get_SwitchBoard_domainName(char* domainName);
    std::string xGateDBManager::DATABASE_QUERY_get_TRUNKOUT(char* site_code, char* switch_code, char* a_did, char* a_cli, char* trunk_in, char* rtp_interface);
    std::string xGateDBManager::DATABASE_QUERY_get_PORTOUT(char* site_code, char* switch_code, char* a_did, char* a_cli, char* trunk_in, char* rtp_interface);
    std::vector<DBQueryResultData>* xGateDBManager::DATABASE_QUERY_get_TRUNKOUT_PORTOUT(char* site_code, char* switch_code, char* a_did, char* a_cli, char* trunk_in, char* rtp_interface);
    int xGateDBManager::DATABASE_QUERY_get_totalNoOfRegisteredUsers(char* extNum,char* domainName);
    std::string xGateDBManager::DATABASE_QUERY_COMBINED_DirectUserInfo_get_device_type_Recordwise(char* extNum, char* domainName, int DbColumnNumber, int DbRecordNumber);
    int xGateDBManager::DATABASE_QUERY_COMBINED_DirectUserInfo_get_app_status_Recordwise(char* extNum, char* domainName, int DbColumnNumber, int DbRecordNumber);
    std::vector<DBQueryResultData>* xGateDBManager::DATABASE_QUERY_combined_SendAppNotification(char* extNum, char* domainName, int DbColumnNumber, int DbRecordNumber);
    std::vector<DBQueryResultData>* xGateDBManager::DATABASE_QUERY_CALL_HANDLING(char* extNum, char* domainName);

    std::string xGateDBManager::DATABASE_QUERY_COMBINED_DirectUserInfo_get_Contact_address_Recordwise(char* extNum, char* domainName, int DbColumnNumber, int DbRecordNumber);
    std::string xGateDBManager::DATABASE_QUERY_COMBINED_DirectUserInfo_get_proxy_username_Recordwise(char* extNum, char* domainName, int DbColumnNumber, int DbRecordNumber);
    int xGateDBManager::DATABASE_QUERY_GET_COMPANY_HOUR_USER(char* domainId, char* extNum, int DbColumnNumber);
    int xGateDBManager::DATABASE_QUERY_INCOMING_CALL_TYPE(char* domainId, char* extNum, int DbColumnNumber);
    std::vector<DBQueryResultData>* xGateDBManager::DATABASE_QUERY_INCOMING_CALL_TYPE_COMBINED_GET_CALLFORWARD_NUMBER(char* domainId, char* extNum);
    int xGateDBManager::DATABASE_QUERY_FORWARD_STATUS_ENABLED(char* domainId, char* extNum, char* flag,int DbColumnNumber);
    std::string xGateDBManager::DATABASE_QUERY_GET_CALLFORWARD_NUMBER_USER(char* domainId, char* extNum, int DbColumnNumber);
    bool xGateDBManager::DATABASE_QUERY_IS_CALLSCREENING_ENABLED(char* extNum, char* domainId);
    std::string xGateDBManager::DATABASE_QUERY_GET_DIRECT_NUMBER(char* extNum, char* domainId);
    std::vector<DBQueryResultData>*  xGateDBManager::DATABASE_QUERY_GET_DIRECT_NUMBER_COMBINEDSP(char* extNum, char* domainId);
    bool xGateDBManager::PostMessage(xGateDBObj *callobj, XGATEPOSTTYPE destType);

    // Conference DB-queries
    bool  xGateDBManager::DATABASE_QUERY_IS_VALID_CONFERENCE_BRIDGE_CONF_ID(char* domain_id, char* conference_id);
    std::string  xGateDBManager::DATABASE_QUERY_GET_CONFERENCE_BRIDGE_PIN(char* domain_id, char* conference_id);
    std::string  xGateDBManager::DATABASE_QUERY_GET_CONFERENCE_BRIDGE_PARTICIPANT_PIN(char* domain_id, char* conference_id);
    std::string  xGateDBManager::DATABASE_QUERY_INSERT_CONFERENCE_MGRESOURCEID(char* conf_id, char* call_id, char* conf_status, char* conf_mg_ip_address, char* pbx_ip_address, char* connection_time, int participant_type, char* domain_name, char* mg_uid);
    std::string  xGateDBManager::DATABASE_QUERY_GET_CONFERENCE_MGRESOURCEID(char* conf_id, char* domain_name);
    std::vector<DBQueryResultData>* xGateDBManager::DATABASE_QUERY_COMBINED_CONF_INFO(const char* conf_id, const char* b_domain_id);
    std::vector<DBQueryResultData>* xGateDBManager::DATABASE_QUERY_COMBINED_CONF_ID(const char* b_domain_id, const char* conf_id);
    std::vector<DBQueryResultData>* xGateDBManager::DATABASE_QUERY_COMBINED_CONF_VALIDATE_PIN(const char* domain_id, const char* pin);
    std::vector<DBQueryResultData>* xGateDBManager::DATABASE_QUERY_COMBINED_CONF_GET_CALLID_MGUID(const char* host_extension, const char* domain_name);
    std::vector<DBQueryResultData>* xGateDBManager::DATABASE_QUERY_COMBINED_GET_PSTN_OPERATOR_DETAILS(char* ip_addr);
    std::string xGateDBManager::DATABASE_QUERY_CONF_REGISTER_MG(char* did, char* call_id, char* conf_mg_ip_address, char* pbx_ip_address, char* conf_mg_uid);

    bool  xGateDBManager::DATABASE_QUERY_CHECK_PIN_TO_CONNECT(char* domain_id, char* extension);
    std::string  xGateDBManager::DATABASE_QUERY_CONF_DELETE_DB_RECORDS(char* conf_id, char* domain_name);

    std::string xGateDBManager::DATABASE_QUERY_GET_CALLER_ID(char* domainid, char* extension);
    int xGateDBManager::DATABASE_QUERY_IS_DND_ENABLED_FOR_APP(char* domainid, char* extension);

    int xGateDBManager::DATABASE_QUERY_CONF_VALIDATE_PIN(char* domainid, char* pin);
    std::string xGateDBManager::DATABASE_QUERY_GET_DOMAIN_NAME(char* domainId);
    void  handle_regdb_process(xGateDBObj *dbObj,std::string dbOutput);
    xGateDBObj *GetDbObjInfo(std::string transid);
    bool InsertDbObjlInfo(xGateDBObj *dbObj);
    bool DeleteDbObjInfo(xGateDBObj *dbObj);
    std::string generateTransid();

    void xGateDBManager::DATABASE_QUERY_VOICEMAIL_DEPOSIT(long long unsigned int date, const char* mboxid, const char* cid_number, const char* uuid, const char* msgfile, int msglen);
    std::vector<DBQueryResultData>*  xGateDBManager::DATABASE_QUERY_VOICEMAIL_RETRIEVAL(const char* mboxID, unsigned int mode);
    void xGateDBManager::DATABASE_QUERY_CALLRECORD_INSERT(const char* clip_id, const char* clip_path, unsigned int clip_size, long long unsigned int time_epoch, const char* username, const char* extension, const char* domain);
    std::vector<DBQueryResultData>* xGateDBManager::DATABASE_QUERY_CALLRECORD_GETINFO(const char* extension, const char* domain);
  private:
    Registrar *regobj;    
};

#endif
