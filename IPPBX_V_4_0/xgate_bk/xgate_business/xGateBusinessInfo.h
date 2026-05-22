#ifndef _XGATE_BUSINESS_INFO_H
#define _XGATE_BUSINESS_INFO_H

//local includes
#include "xGateUtil.h"
#include "xGateBusinessUtil.h"
#include "xGateBusinessInterface.h"

class xGateBusinessInfo 
{
  public:
    xGateBusinessInfo(string &uid, string &cli, string &ddi, string &domainName);
    ~xGateBusinessInfo(); 
    xGateBusinessInfo(const &);
    const xGateBusinessInfo &operator=(const &); 
    
    //member functions
    bool init();
    bool get_aor(const Value &val);
    bool use_prequery_result(const Value &val);
    bool use_aor_result(const Value &val);
    bool handle_timeout_event(const Value &val);
    bool handle_cancel_event(const Value &val);

    //memebr variables
    string m_uid, m_cli, m_ddi, m_domainName;
    unsigned int m_domainId;
    LegInfo m_cliInfo; //calling party info
    LegInfo m_ddiInfo; //called party info
    CallFeatureType m_callFeatureType; //call feature loaded/required at this momemt
    xGateBusinessSigEvent m_currentSigEvent;
    unsigned int m_isForkingEnabled;
    unsigned long m_pushNotifyTimerId;
    unsigned long m_callHandleTimerId;
    unsigned long m_dialOutTimerId;


  private:
    //memeber variables
    xGateCallFeature* m_callFeatureList[EN_CALL_FEATURE_TYPE_MAX];
    map<unsigned int, AddressOfRecord> m_MappedUserList;
    long int m_timerList[EN_BUSINESS_TIMER_MAX];

    //member functions
    bool sendToDb(int event);
    bool load_call_feature(const Value &val);
    bool load_internal_call_feature(const Value &val);
    bool load_external_call_feature(const Value &val);
    bool load_vms_call_feature(const Value &val);
    bool load_swb_call_feature(const Value &val);
    bool load_conf_call_feature(const Value &val);
    bool load_aor(const Value &val);
    bool check_send_app_notification(AddressOfRecord &aor);
    bool send_business_init_response(const Value &val);
};


#endif
