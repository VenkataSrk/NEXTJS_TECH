#pragma once
#include "IURDefines.h"
#include "xGateUtil.h"

enum class xGateMonitorEvent
{
    EN_MONITOR_UNKNWON = 0,
    EN_MONITOR_TRUNK_SEND_OPTIONS,
    EN_MONITOR_TRUNK_OPTIONS_SUCCESS,
    EN_MONITOR_TRUNK_OPTIONS_FAILED,
    EN_MONITOR_TRUNK_OPTIONS_TIMEOUT,
    EN_MONITOR_TRUNK_STATUS_SUCCESS,
    EN_MONITOR_TRUNK_STATUS_FAILED,
    EN_MONITOR_FRAUD_BLOCK,
    EN_MONITOR_ADD_NEW_TRUNK,
    EN_MONITOR_REMOVE_TRUNK
};

enum class xGateTrunkStatus
{
    EN_TRUNK_STATUS_NULL = 0,
    EN_TRUNK_STATUS_INIT ,
    EN_TRUNK_STATUS_OPTIONS_SENT,
    EN_TRUNK_STATUS_OPTIONS_SUCCESS,
    EN_TURNK_STATUS_OPTIONS_FAILED,
    EN_TURNK_STATUS_OPTIONS_TIMEOUT
};

class MonitorContext
{
    public:
        MonitorContext() = default;
        ~MonitorContext() = default;
        MonitorContext(const MonitorContext &rhs)
        {
            m_trunkInfo = rhs.m_trunkInfo;
            m_trunkStatus = rhs.m_trunkStatus;
        };
        MonitorContext &operator=(const MonitorContext &rhs)
        {
            m_trunkInfo = rhs.m_trunkInfo;
            m_trunkStatus = rhs.m_trunkStatus;
            return *this;
        };

        TrunkInfo m_trunkInfo;
        xGateTrunkStatus m_trunkStatus = xGateTrunkStatus::EN_TRUNK_STATUS_INIT;
};

class xGateMonitorInfo
{
    public:
        xGateMonitorInfo() = default;
        ~xGateMonitorInfo() = default;

        xGateMonitorInfo(const xGateMonitorInfo &rhs)
        {
            m_iTrunkId = rhs.m_iTrunkId;
            m_uiPort = rhs.m_uiPort;
            m_sCallId = rhs.m_sCallId.c_str();
            m_sDcType = rhs.m_sDcType.c_str();
            m_IpAddressList = rhs.m_IpAddressList;
            m_TrunkEvent = rhs.m_TrunkEvent;
        };

        xGateMonitorInfo &operator=(const xGateMonitorInfo &rhs)
        {
            m_iTrunkId = rhs.m_iTrunkId;
            m_uiPort = rhs.m_uiPort;
            m_sCallId = rhs.m_sCallId.c_str();
            m_sDcType = rhs.m_sDcType.c_str();
            m_IpAddressList = rhs.m_IpAddressList;
            m_TrunkEvent = rhs.m_TrunkEvent;
            return *this;
        };

        int m_iTrunkId;
        unsigned int m_uiPort;
        std::string m_sCallId;
        std::string m_sDcType;
        std::vector<std::string> m_IpAddressList;
        xGateMonitorEvent m_TrunkEvent;
};