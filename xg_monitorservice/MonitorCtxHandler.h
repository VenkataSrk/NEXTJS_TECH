#pragma once

//c++ includes
#include <map>
//local includes
#include "xGateMonitorServiceDefine.h"

class MonitorCtxHandler final
{
    public:
        MonitorCtxHandler() = default;
        ~MonitorCtxHandler() = default;
        bool insertMonitorCtx(std::string &ip,int &trunk_id,MonitorContext *mnctx);
        bool deleteMonitorCtx(std::string &ip,int &trunk_id);
        bool getMonitorCtx(std::string &ip,int &trunk_id ,MonitorContext  *& mnctx);

    private:
        std::map<std::pair<std::string, int>, MonitorContext*> m_mntrHctx;
        std::mutex _ctx_mutex;
};