#pragma once
#include "IURDefines.h"

typedef enum
{
  EN_DB_METHOD_UNKNOWN = 0,
  EN_DB_METHOD_CREATE = 1,
  EN_DB_METHOD_READ = 2,
  EN_DB_METHOD_UPDATE = 3,
  EN_DB_METHOD_DELETE = 4,
  EN_DB_METHOD_EXECUTE = 5,
} DBReqMethod;

typedef enum
{
  EN_DB_EVENT_UNKNOWN = 0,
  EN_DB_EVENT_CALL_FORWARDING_QUERY = 1,
  EN_DB_EVENT_STORE_CALL_REC_DATA_QUERY = 2,
  EN_DB_EVENT_STORE_CCAAS_CALL_REC_DATA_QUERY = 3,
  EN_DB_EVENT_STORE_CRM_CALL_REC_DATA_QUERY = 4
} DBServiceEvent;

typedef enum
{
  EN_DB_STATUS_UNKNOWN = 0,
  EN_DB_STATUS_SUCCESS = 1,
  EN_DB_STATUS_FAILED = 2,
} DBResponseStatus;

typedef enum
{
  EN_INCOMING_CALL_TYPE = 1,
  EN_RING_TYPE = 2,
  EN_NAME = 3,
  EN_FORWARD_TYPE = 4,
  EN_FORWARD_NUMBER = 5,
  EN_CALL_FORWARD_ORDER = 6,
} CallForwardFieldIndex;

class DBRequestInfo
{
public:
  DBRequestInfo()
  {
    m_data = "";
    m_contextId = "";
    m_output = "";
    m_requestorModule = IURDefines::UR_MODULE_UNKNOWN;
    m_dbReqMethod = EN_DB_METHOD_UNKNOWN;
    m_dbRespStatus = EN_DB_STATUS_UNKNOWN;
  }

  ~DBRequestInfo()
  {
    m_data = "";
    m_contextId = "";
    m_output = "";
    m_requestorModule = IURDefines::UR_MODULE_UNKNOWN;
    m_dbReqMethod = EN_DB_METHOD_UNKNOWN;
    m_dbRespStatus = EN_DB_STATUS_UNKNOWN;
  }

  DBRequestInfo(const DBRequestInfo &rhs)
  {
    m_data = rhs.m_data;
    m_output = rhs.m_output;
    m_contextId = rhs.m_contextId;
    m_requestorModule = rhs.m_requestorModule;
    m_dbReqMethod = rhs.m_dbReqMethod;
    m_dbRespStatus = rhs.m_dbRespStatus;
  }

  DBRequestInfo &operator=(const DBRequestInfo &rhs)
  {
    m_data = rhs.m_data;
    m_output = rhs.m_output;
    m_contextId = rhs.m_contextId;
    m_requestorModule = rhs.m_requestorModule;
    m_dbReqMethod = rhs.m_dbReqMethod;
    m_dbRespStatus = rhs.m_dbRespStatus;
  }

  string m_data;
  string m_output;
  string m_contextId;
  string m_spName;
  IURDefines::MODULE_ID m_requestorModule;
  DBReqMethod m_dbReqMethod;
  DBResponseStatus m_dbRespStatus;
  DBServiceEvent m_dbEvent;
};

class DBConnectionInfo
{
public:
  DBConnectionInfo()
  {
    m_sDBSrvIp = "";
    m_sDBSrvUname = "";
    m_sDBSrvPassword = "";
    m_sDBSrvPort ;
    m_sDBName = "";
  }
  ~DBConnectionInfo()
  {
    m_sDBSrvIp = "";
    m_sDBSrvUname = "";
    m_sDBSrvPassword = "";
    m_sDBSrvPort ;
    m_sDBName = "";
  }
  DBConnectionInfo(const DBConnectionInfo &rhs)
  {
    m_sDBSrvIp = rhs.m_sDBSrvIp.c_str();
    m_sDBSrvUname = rhs.m_sDBSrvUname.c_str();
    m_sDBSrvPassword = rhs.m_sDBSrvPassword.c_str();
    m_sDBSrvPort = rhs.m_sDBSrvPort;
    m_sDBName = rhs.m_sDBName.c_str();
  }
  DBConnectionInfo &operator=(const DBConnectionInfo &rhs)
  {
    m_sDBSrvIp = rhs.m_sDBSrvIp.c_str();
    m_sDBSrvUname = rhs.m_sDBSrvUname.c_str();
    m_sDBSrvPassword = rhs.m_sDBSrvPassword.c_str();
    m_sDBSrvPort = rhs.m_sDBSrvPort;
    m_sDBName = rhs.m_sDBName.c_str();
    return *this;
  }
  std::string m_sDBSrvIp;
  std::string m_sDBSrvUname;
  std::string m_sDBSrvPassword;
  unsigned int m_sDBSrvPort;
  std::string m_sDBName;
};