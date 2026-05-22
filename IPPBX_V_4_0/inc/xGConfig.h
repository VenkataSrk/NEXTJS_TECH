#ifndef __XGCONFIG_H__
#define __XGCONFIG_H__
#pragma once

#include <iostream>

//rapidjson includes
#include "document.h"
#include "error/en.h"
#include "writer.h"
#include "reader.h"
#include "stringbuffer.h"
#include "prettywriter.h"
#include "ostreamwrapper.h"
#include "filereadstream.h"




#include "xGSingleton.h"
#include "xglog.h"

using namespace rapidjson;
using namespace std;

#define XGCONFIG() xGConfig::instance()
#define XGCONFIG_INIT() xGConfig::instance().Initialize()

#define ENV_BASE "SRS_CONF_PATH"
#define CONF_FILE_NAME "/Recorder.conf"
#define MAX_CONF_BUFFER_SIZE 4096*2

class SRSServerInfoProfile
{
public:
    SRSServerInfoProfile() = default;
    ~SRSServerInfoProfile() = default;
    SRSServerInfoProfile(const SRSServerInfoProfile &src);

    // member variables
    std::string m_server_id ;
    std::string m_server_dns;
    std::string m_server_ip ;
    std::string m_server_mac;
    std::string m_present_call_load;
    std::string m_max_call_load ;
    std::string m_callrec_srv_local_weburl;
    std::string m_callrec_srv_sip_login_number;
    std::string m_callsrv_sip_login_password;
    std::string m_callsrv_registrar_sip_url ;
    std::string m_callsrv_registration_enabled;
    std::string m_callsrv_server_enabled ;
    std::string m_callsrv_server_preference_order;
    std::string m_save_server_info_url;
    std::string m_save_record_info_url;
};

class DBProfile
{
public:
    DBProfile() = default;
    ~DBProfile() = default;
    DBProfile(const DBProfile &src);

    // member variables
    std::string m_db_srv_ip ;
    unsigned int m_db_srv_port;
    std::string m_db_srv_uname ;
    std::string m_db_srv_password;
    std::string m_db_name ;
};

class CcaasDBProfile
{
public:
    CcaasDBProfile() = default;
    ~CcaasDBProfile() = default;
    CcaasDBProfile(const CcaasDBProfile &src);

    // member variables
    std::string m_db_srv_ip ;
    unsigned int m_db_srv_port;
    std::string m_db_srv_uname ;
    std::string m_db_srv_password;
    std::string m_db_name;
    std::string m_db_ccaas_rec_info_sp;
};

class CrmDBProfile
{
public:
    CrmDBProfile() = default;
    ~CrmDBProfile() = default;
    CrmDBProfile(const CrmDBProfile &src);

    // member variables
    std::string m_db_srv_ip ;
    unsigned int m_db_srv_port;
    std::string m_db_srv_uname ;
    std::string m_db_srv_password;
    std::string m_db_name;
    std::string m_db_crm_rec_info_sp;
};

class RedisProfile
{
public:
    RedisProfile() = default;
    ~RedisProfile() = default ;
    RedisProfile(const RedisProfile &src);

    // member variables
    std::string m_strServerHost ;
    unsigned int m_iServerPort ;
    std::string m_strPassword ;
    std::string m_strChannel;
    unsigned int m_iSocketTimeout;
};

class xGConfig final : public xGSingleton<xGConfig>
{
public:
    explicit xGConfig(typename xGSingleton<xGConfig>::token);
    bool Initialize();

    SRSServerInfoProfile m_srsServerInfoProfile;
    DBProfile m_dbProfile;
    RedisProfile m_redisProfile;
    CcaasDBProfile m_ccaasDbProfile;
    CrmDBProfile m_crmDbProfile;
private:
    std::string m_sConfigPath;
    Document m_configDoc;
    bool get_config();
    bool read_config();
    bool parseJsonConfigFile();
    bool readSRSServerInfoProfile();
    bool readDBProfile();
    bool readRedisProfile();
    bool readCcaasDBProfile();
    bool readCrmDBProfile();
};

#endif // __XGCONFIG_H__