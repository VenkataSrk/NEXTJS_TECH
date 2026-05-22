#pragma once
#include "IURDefines.h"

enum class DBReqMethod
{
  EN_DB_METHOD_UNKNOWN = 0,
  EN_DB_METHOD_CREATE = 1,
  EN_DB_METHOD_READ = 2,
  EN_DB_METHOD_UPDATE = 3,
  EN_DB_METHOD_DELETE = 4,
  EN_DB_METHOD_EXECUTE = 5,
};

enum class DBServiceEvent
{
  EN_DB_EVENT_UNKNOWN = 0,
  EN_DB_EVENT_GET_TRUNK_LIST = 1,
  EN_DB_EVENT_UPDATE_TRUNK_STATUS = 2,
  EN_DB_EVENT_GET_MSTEAMS_DOMAIN_LIST = 3
};

enum class DBResponseStatus
{
  EN_DB_STATUS_UNKNOWN = 0,
  EN_DB_STATUS_SUCCESS = 1,
  EN_DB_STATUS_FAILED = 2,
};

class DBRequestInfo
{
public:
  DBRequestInfo()
  {
    m_data = "";
    m_contextId = "";
    m_output = "";
    m_spName = "";
    m_requestorModule = IURDefines::UR_MODULE_UNKNOWN;
    m_dbReqMethod = DBReqMethod::EN_DB_METHOD_UNKNOWN;
    m_dbRespStatus = DBResponseStatus::EN_DB_STATUS_UNKNOWN;
    m_dbEvent = DBServiceEvent::EN_DB_EVENT_UNKNOWN;
  }

  ~DBRequestInfo()
  {
    m_data = "";
    m_contextId = "";
    m_output = "";
    m_spName = "";
    m_requestorModule = IURDefines::UR_MODULE_UNKNOWN;
    m_dbReqMethod = DBReqMethod::EN_DB_METHOD_UNKNOWN;
    m_dbRespStatus = DBResponseStatus::EN_DB_STATUS_UNKNOWN;
    m_dbEvent = DBServiceEvent::EN_DB_EVENT_UNKNOWN;
  }

  DBRequestInfo(const DBRequestInfo &rhs)
  {
    m_data = rhs.m_data;
    m_output = rhs.m_output;
    m_contextId = rhs.m_contextId;
    m_spName = rhs.m_spName;
    m_requestorModule = rhs.m_requestorModule;
    m_dbReqMethod = rhs.m_dbReqMethod;
    m_dbRespStatus = rhs.m_dbRespStatus;
    m_dbEvent = rhs.m_dbEvent;
    m_trunk_list = deep_copy(rhs.m_trunk_list);
  }

  DBRequestInfo &operator=(const DBRequestInfo &rhs)
  {
    m_data = rhs.m_data;
    m_output = rhs.m_output;
    m_contextId = rhs.m_contextId;
    m_spName = rhs.m_spName;
    m_requestorModule = rhs.m_requestorModule;
    m_dbReqMethod = rhs.m_dbReqMethod;
    m_dbRespStatus = rhs.m_dbRespStatus;
    m_dbEvent = rhs.m_dbEvent;
    m_trunk_list = deep_copy(rhs.m_trunk_list);
    return *this;
  }

  string m_data;
  string m_output;
  string m_contextId;
  string m_spName;
  std::vector<TrunkInfo> m_trunk_list;
  IURDefines::MODULE_ID m_requestorModule;
  DBReqMethod m_dbReqMethod;
  DBResponseStatus m_dbRespStatus;
  DBServiceEvent m_dbEvent;

  template <typename T>
  std::vector<T> deep_copy(std::vector<T> const &vec)
  {
    std::vector<T> v(vec.begin(), vec.end());
    return v;
  }
};

class DBConnectionInfo
{
public:
  DBConnectionInfo()
  {
    m_sDBSrvIp = "";
    m_sDBSrvUname = "";
    m_sDBSrvPassword = "";
    m_sDBSrvPort = 0;
    m_sDBName = "";
  }
  ~DBConnectionInfo()
  {
    m_sDBSrvIp = "";
    m_sDBSrvUname = "";
    m_sDBSrvPassword = "";
    m_sDBSrvPort = 0;
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
  unsigned short m_sDBSrvPort;
  std::string m_sDBName;
};