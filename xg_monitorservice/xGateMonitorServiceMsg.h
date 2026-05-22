#pragma once

//ace includes
#include <ace/Message_Block.h>

//local includes
#include "xGateUtil.h"
#include"xGateMonitorServiceDefine.h"

class xGateMonitorServiceMsg : public xGateBaseMsg
{
    public:
        xGateMonitorServiceMsg(void) : xGateBaseMsg(IURDefines::UR_MODULE_DB_SERVICE){};
        virtual ~xGateMonitorServiceMsg(){};

        inline void setMonitorInfo(xGateMonitorInfo &monitorinfo)
        {
            m_monitorInfo = monitorinfo;
        }

        inline xGateMonitorInfo &getMonitorInfo()
        {   
            return m_monitorInfo;
        }

    protected:
        xGateMonitorServiceMsg &operator=(const xGateMonitorServiceMsg &);

    private:
        xGateMonitorInfo m_monitorInfo;
};
