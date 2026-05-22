#include "xGConfig.h"

xGConfig::xGConfig(typename xGSingleton<xGConfig>::token):m_sConfigPath("")
{
}

bool xGConfig::Initialize()
{
    if (!get_config())
        return false;
    if (!read_config()) {
        XGLOG_ERROR("read configuration file '%s' failed !", m_sConfigPath.c_str());
        return false;
    }

    return true;
}

bool xGConfig::get_config()
{
    char *configPath;
    configPath = getenv(ENV_BASE);

    if (configPath)
    {
        XGLOG_INFO("checkConfigPath:: Conf Bridge Service will read 'Recorder.conf' file from path '%s'", configPath);
        m_sConfigPath = configPath;
        m_sConfigPath += CONF_FILE_NAME;
    }
    else
    {
        XGLOG_ERROR("checkConfigPath:: 'SRS_CONF_PATH' environment variable is missing !!");
        return false;
    }

    return true;
}

bool xGConfig::read_config()
{
    if (!parseJsonConfigFile())
    {
        XGLOG_ERROR("readConfig failed while parsing json config file %s !", m_sConfigPath.c_str());
        return false;
    }
    else if(!readSRSServerInfoProfile())
    {
        XGLOG_ERROR("readConfig failed while reading SRS Serevr Info profile details !");
        return false;
    }
    else if(!readDBProfile())
    {
        XGLOG_ERROR("readConfig failed while reading DB profile details !");
        return false;
    }
    else if(!readRedisProfile())
    {
        XGLOG_ERROR("readConfig failed while reading Redis profile details !");
        return false;
    }
    else if(!readCcaasDBProfile())
    {
        XGLOG_ERROR("readConfig failed while reading Ccaas DB profile details !");
        return false;
    }
    else if(!readCrmDBProfile())
    {
        XGLOG_ERROR("readConfig failed while reading CRM DB profile details !");
        return false;
    }
    return true;
}

bool xGConfig::parseJsonConfigFile()
{
    char confBuffer[MAX_CONF_BUFFER_SIZE];
    FILE *fp = fopen(m_sConfigPath.c_str(), "rb");
    if (fp == nullptr)
    {
        XGLOG_ERROR("not able to open file %s", m_sConfigPath.c_str());
        return false;
    }
    FileReadStream is(fp, confBuffer, MAX_CONF_BUFFER_SIZE);
    fclose(fp);
    ParseResult ok = m_configDoc.Parse<kParseCommentsFlag>(confBuffer);
    if (!ok)
    {
        XGLOG_ERROR("parseJsonConfigFile failed. due to json parse error: %s !", GetParseError_En(ok.Code()));
        return false;
    }
    else if (!m_configDoc.IsObject())
    {
        XGLOG_ERROR("parseJsonConfigFile failed. not able to create document object !");
        return false;
    }

    XGLOG_INFO("Successfully parsed json data from %s file", m_sConfigPath.c_str());

    return true;
}

bool xGConfig::readSRSServerInfoProfile()
{
    if (!m_configDoc.HasMember("SRSServerInfoProfile"))
    {
        XGLOG_ERROR("SRSServerInfoProfile failed. 'SRSServerInfoProfile' details is missing in %s file !", m_sConfigPath.c_str());
        return false;
    }

    const Value &SRSServerInfoProfile = m_configDoc["SRSServerInfoProfile"];

    if (SRSServerInfoProfile.HasMember("server_id"))
    {
        m_srsServerInfoProfile.m_server_id = SRSServerInfoProfile["server_id"].GetString();
    }
    else
    {
        XGLOG_ERROR("SRSServerInfoProfile failed. 'server_id' detail is missing in '%s' file !", m_sConfigPath.c_str());
        return false;
    }

    if (SRSServerInfoProfile.HasMember("server_dns"))
    {
        m_srsServerInfoProfile.m_server_dns = SRSServerInfoProfile["server_dns"].GetString();
    }
    else
    {
        XGLOG_ERROR("SRSServerInfoProfile failed. 'server_dns' detail is missing in '%s' file !", m_sConfigPath.c_str());
        return false;
    }

    if (SRSServerInfoProfile.HasMember("server_ip"))
    {
        m_srsServerInfoProfile.m_server_ip = SRSServerInfoProfile["server_ip"].GetString();
    }
    else
    {
        XGLOG_ERROR("SRSServerInfoProfile failed. 'server_ip' detail is missing in '%s' file !", m_sConfigPath.c_str());
        return false;
    }

    if (SRSServerInfoProfile.HasMember("server_mac"))
    {
        m_srsServerInfoProfile.m_server_mac = SRSServerInfoProfile["server_mac"].GetString();
    }
    else
    {
        XGLOG_ERROR("SRSServerInfoProfile failed. 'server_mac' detail is missing in '%s' file !", m_sConfigPath.c_str());
        return false;
    }

    if (SRSServerInfoProfile.HasMember("present_call_load"))
    {
        m_srsServerInfoProfile.m_present_call_load = SRSServerInfoProfile["present_call_load"].GetString();
    }
    else
    {
        XGLOG_ERROR("SRSServerInfoProfile failed. 'present_call_load' detail is missing in '%s' file !", m_sConfigPath.c_str());
        return false;
    }

    if (SRSServerInfoProfile.HasMember("max_call_load"))
    {
        m_srsServerInfoProfile.m_max_call_load = SRSServerInfoProfile["max_call_load"].GetString();
    }
    else
    {
        XGLOG_ERROR("SRSServerInfoProfile failed. 'max_call_load' detail is missing in '%s' file !", m_sConfigPath.c_str());
        return false;
    }

    if (SRSServerInfoProfile.HasMember("callrec_srv_local_weburl"))
    {
        m_srsServerInfoProfile.m_callrec_srv_local_weburl = SRSServerInfoProfile["callrec_srv_local_weburl"].GetString();
    }
    else
    {
        XGLOG_ERROR("SRSServerInfoProfile failed. 'callrec_srv_local_weburl' detail is missing in '%s' file !", m_sConfigPath.c_str());
        return false;
    }

    if (SRSServerInfoProfile.HasMember("callrec_srv_sip_login_number"))
    {
        m_srsServerInfoProfile.m_callrec_srv_sip_login_number = SRSServerInfoProfile["callrec_srv_sip_login_number"].GetString();
    }
    else
    {
        XGLOG_ERROR("SRSServerInfoProfile failed. 'callrec_srv_sip_login_number' detail is missing in '%s' file !", m_sConfigPath.c_str());
        return false;
    }

    if (SRSServerInfoProfile.HasMember("callsrv_sip_login_password"))
    {
        m_srsServerInfoProfile.m_callsrv_sip_login_password = SRSServerInfoProfile["callsrv_sip_login_password"].GetString();
    }
    else
    {
        XGLOG_ERROR("SRSServerInfoProfile failed. 'callsrv_sip_login_password' detail is missing in '%s' file !", m_sConfigPath.c_str());
        return false;
    }

    if (SRSServerInfoProfile.HasMember("callsrv_registrar_sip_url"))
    {
        m_srsServerInfoProfile.m_callsrv_registrar_sip_url = SRSServerInfoProfile["callsrv_registrar_sip_url"].GetString();
    }
    else
    {
        XGLOG_ERROR("SRSServerInfoProfile failed. 'callsrv_registrar_sip_url' detail is missing in '%s' file !", m_sConfigPath.c_str());
        return false;
    }

    if (SRSServerInfoProfile.HasMember("callsrv_registration_enabled"))
    {
        m_srsServerInfoProfile.m_callsrv_registration_enabled = SRSServerInfoProfile["callsrv_registration_enabled"].GetString();
    }
    else
    {
        XGLOG_ERROR("SRSServerInfoProfile failed. 'callsrv_registration_enabled' detail is missing in '%s' file !", m_sConfigPath.c_str());
        return false;
    }

    if (SRSServerInfoProfile.HasMember("callsrv_server_enabled"))
    {
        m_srsServerInfoProfile.m_callsrv_server_enabled = SRSServerInfoProfile["callsrv_server_enabled"].GetString();
    }
    else
    {
        XGLOG_ERROR("SRSServerInfoProfile failed. 'callsrv_server_enabled' detail is missing in '%s' file !", m_sConfigPath.c_str());
        return false;
    }

    if (SRSServerInfoProfile.HasMember("callsrv_server_preference_order"))
    {
        m_srsServerInfoProfile.m_callsrv_server_preference_order = SRSServerInfoProfile["callsrv_server_preference_order"].GetString();
    }
    else
    {
        XGLOG_ERROR("SRSServerInfoProfile failed. 'callsrv_server_preference_order' detail is missing in '%s' file !", m_sConfigPath.c_str());
        return false;
    }

    if (SRSServerInfoProfile.HasMember("save_server_info_url"))
    {
        m_srsServerInfoProfile.m_save_server_info_url = SRSServerInfoProfile["save_server_info_url"].GetString();
    }
    else
    {
        XGLOG_ERROR("SRSServerUrlProfile failed. 'save_server_info_url' detail is missing in '%s' file !", m_sConfigPath.c_str());
        return false;
    }

    if (SRSServerInfoProfile.HasMember("save_record_info_url"))
    {
        m_srsServerInfoProfile.m_save_record_info_url = SRSServerInfoProfile["save_record_info_url"].GetString();
    }
    else
    {
        XGLOG_ERROR("SRSServerUrlProfile failed. 'save_record_info_url' detail is missing in '%s' file !", m_sConfigPath.c_str());
        return false;
    }

    XGLOG_INFO("Succesfully read SRSServerInfoProfile profile details from '%s' file", m_sConfigPath.c_str());


    return true;
}

bool xGConfig::readDBProfile()
{
    if (!m_configDoc.HasMember("DBProfile"))
    {
        XGLOG_ERROR("DBProfile failed. 'DBProfile' details is missing in %s file !", m_sConfigPath.c_str());
        return false;
    }

    const Value &DbProfile = m_configDoc["DBProfile"];

    if (DbProfile.HasMember("db_server_ip"))
    {
        m_dbProfile.m_db_srv_ip = DbProfile["db_server_ip"].GetString();
    }
    else
    {
        XGLOG_ERROR("DBProfile failed. 'db_server_ip' detail is missing in '%s' file !", m_sConfigPath.c_str());
        return false;
    }

    if (DbProfile.HasMember("db_server_port"))
    {
        m_dbProfile.m_db_srv_port = DbProfile["db_server_port"].GetInt();
    }
    else
    {
        XGLOG_ERROR("DBProfile failed. 'db_server_port' detail is missing in '%s' file !", m_sConfigPath.c_str());
        return false;
    }

    if (DbProfile.HasMember("db_user_name"))
    {
        m_dbProfile.m_db_srv_uname = DbProfile["db_user_name"].GetString();
    }
    else
    {
        XGLOG_ERROR("DBProfile failed. 'db_user_name' detail is missing in '%s' file !", m_sConfigPath.c_str());
        return false;
    }

    if (DbProfile.HasMember("db_password"))
    {
        m_dbProfile.m_db_srv_password = DbProfile["db_password"].GetString();
    }
    else
    {
        XGLOG_ERROR("DBProfile failed. 'db_password' detail is missing in '%s' file !", m_sConfigPath.c_str());
        return false;
    }

    if (DbProfile.HasMember("db_name"))
    {
        m_dbProfile.m_db_name = DbProfile["db_name"].GetString();
    }
    else
    {
        XGLOG_ERROR("DBProfile failed. 'db_name' detail is missing in '%s' file !", m_sConfigPath.c_str());
        return false;
    }

    XGLOG_INFO("Succesfully read DBProfile profile details from '%s' file", m_sConfigPath.c_str());

    return true;
}

bool xGConfig::readRedisProfile()
{
    if (!m_configDoc.HasMember("RedisProfile"))
    {
        XGLOG_ERROR("RedisProfile failed. 'RedisProfile' details is missing in %s file !", m_sConfigPath.c_str());
        return false;
    }

     const Value &redisprofile = m_configDoc["RedisProfile"];

     if (redisprofile.HasMember("redis_server_ip"))
     {
        m_redisProfile.m_strServerHost = redisprofile["redis_server_ip"].GetString();
     }
     else
     {
        XGLOG_ERROR("RedisProfile failed. 'redis_server_ip' detail is missing in '%s' file !", m_sConfigPath.c_str());
        return false;
     }

     if (redisprofile.HasMember("redis_server_port"))
     {
        m_redisProfile.m_iServerPort = redisprofile["redis_server_port"].GetInt();
     }
     else
     {
        XGLOG_ERROR("RedisProfile failed. 'redis_server_port' detail is missing in '%s' file !", m_sConfigPath.c_str());
        return false;
     }

     if (redisprofile.HasMember("redis_server_password"))
     {
        m_redisProfile.m_strPassword = redisprofile["redis_server_password"].GetString();
     }
     else
     {
        XGLOG_ERROR("RedisProfile failed. 'redis_server_password' detail is missing in '%s' file !", m_sConfigPath.c_str());
        return false;
     }

     if (redisprofile.HasMember("redis_pub_channel"))
     {
        m_redisProfile.m_strChannel = redisprofile["redis_pub_channel"].GetString();
     }
     else
     {
        XGLOG_ERROR("RedisProfile failed. 'redis_pub_channel' detail is missing in '%s' file !", m_sConfigPath.c_str());
        return false;
     }

     XGLOG_INFO("Succesfully read RedisProfile profile details from '%s' file", m_sConfigPath.c_str());

    return true;
}

/*
*To Read Ccaas_DB_Profile
*/
bool xGConfig::readCcaasDBProfile()
{
    if (!m_configDoc.HasMember("CcaasDBProfile"))
    {
        XGLOG_ERROR("CcaasDBProfile failed. 'CcaasDBProfile' details is missing in %s file !", m_sConfigPath.c_str());
        return false;
    }

    const Value &CcaasDbProfile = m_configDoc["CcaasDBProfile"];

    if (CcaasDbProfile.HasMember("db_server_ip"))
    {
        m_ccaasDbProfile.m_db_srv_ip = CcaasDbProfile["db_server_ip"].GetString();
    }
    else
    {
        XGLOG_ERROR("CcaasDbProfile failed. 'db_server_ip' detail is missing in '%s' file !", m_sConfigPath.c_str());
        return false;
    }

    if (CcaasDbProfile.HasMember("db_server_port"))
    {
        m_ccaasDbProfile.m_db_srv_port = CcaasDbProfile["db_server_port"].GetInt();
    }
    else
    {
        XGLOG_ERROR("CcaasDbProfile failed. 'db_server_port' detail is missing in '%s' file !", m_sConfigPath.c_str());
        return false;
    }

    if (CcaasDbProfile.HasMember("db_user_name"))
    {
        m_ccaasDbProfile.m_db_srv_uname = CcaasDbProfile["db_user_name"].GetString();
    }
    else
    {
        XGLOG_ERROR("CcaasDbProfile failed. 'db_user_name' detail is missing in '%s' file !", m_sConfigPath.c_str());
        return false;
    }

    if (CcaasDbProfile.HasMember("db_password"))
    {
        m_ccaasDbProfile.m_db_srv_password = CcaasDbProfile["db_password"].GetString();
    }
    else
    {
        XGLOG_ERROR("CcaasDbProfile failed. 'db_password' detail is missing in '%s' file !", m_sConfigPath.c_str());
        return false;
    }

    if (CcaasDbProfile.HasMember("db_name"))
    {
        m_ccaasDbProfile.m_db_name = CcaasDbProfile["db_name"].GetString();
    }
    else
    {
        XGLOG_ERROR("CcaasDbProfile failed. 'db_name' detail is missing in '%s' file !", m_sConfigPath.c_str());
        return false;
    }

    if (CcaasDbProfile.HasMember("db_ccaas_rec_info_sp"))
    {
        m_ccaasDbProfile.m_db_ccaas_rec_info_sp = CcaasDbProfile["db_ccaas_rec_info_sp"].GetString();
    }
    else
    {
        XGLOG_ERROR("CcaasDbProfile failed. 'db_ccaas_rec_info_sp' detail is missing in '%s' file !", m_sConfigPath.c_str());
        return false;
    }

    XGLOG_INFO("Succesfully read CcaasDbProfile profile details from '%s' file", m_sConfigPath.c_str());

    return true;
}

/*
*To Read CRM_DB_Profile
*/
bool xGConfig::readCrmDBProfile()
{
    if (!m_configDoc.HasMember("CrmDBProfile"))
    {
        XGLOG_ERROR("CrmDBProfile failed. 'CrmDBProfile' details is missing in %s file !", m_sConfigPath.c_str());
        return false;
    }

    const Value &CrmDbProfile = m_configDoc["CrmDBProfile"];

    if (CrmDbProfile.HasMember("db_server_ip"))
    {
        m_crmDbProfile.m_db_srv_ip = CrmDbProfile["db_server_ip"].GetString();
    }
    else
    {
        XGLOG_ERROR("CrmDBProfile failed. 'db_server_ip' detail is missing in '%s' file !", m_sConfigPath.c_str());
        return false;
    }

    if (CrmDbProfile.HasMember("db_server_port"))
    {
        m_crmDbProfile.m_db_srv_port = CrmDbProfile["db_server_port"].GetInt();
    }
    else
    {
        XGLOG_ERROR("CrmDBProfile failed. 'db_server_port' detail is missing in '%s' file !", m_sConfigPath.c_str());
        return false;
    }

    if (CrmDbProfile.HasMember("db_user_name"))
    {
        m_crmDbProfile.m_db_srv_uname = CrmDbProfile["db_user_name"].GetString();
    }
    else
    {
        XGLOG_ERROR("CrmDBProfile failed. 'db_user_name' detail is missing in '%s' file !", m_sConfigPath.c_str());
        return false;
    }

    if (CrmDbProfile.HasMember("db_password"))
    {
        m_crmDbProfile.m_db_srv_password = CrmDbProfile["db_password"].GetString();
    }
    else
    {
        XGLOG_ERROR("CrmDBProfile failed. 'db_password' detail is missing in '%s' file !", m_sConfigPath.c_str());
        return false;
    }

    if (CrmDbProfile.HasMember("db_name"))
    {
        m_crmDbProfile.m_db_name = CrmDbProfile["db_name"].GetString();
    }
    else
    {
        XGLOG_ERROR("CrmDBProfile failed. 'db_name' detail is missing in '%s' file !", m_sConfigPath.c_str());
        return false;
    }

    if (CrmDbProfile.HasMember("db_crm_rec_info_sp"))
    {
        m_crmDbProfile.m_db_crm_rec_info_sp = CrmDbProfile["db_crm_rec_info_sp"].GetString();
    }
    else
    {
        XGLOG_ERROR("CrmDBProfile failed. 'db_crm_rec_info_sp' detail is missing in '%s' file !", m_sConfigPath.c_str());
        return false;
    }

    XGLOG_INFO("Succesfully read CrmDBProfile profile details from '%s' file", m_sConfigPath.c_str());

    return true;
}