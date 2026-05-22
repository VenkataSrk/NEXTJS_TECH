#pragma once

#include "xGateMonitorServiceMsg.h"
#include "xGateMonitorService.h"
#include "xGateSingleton.h"
#include "xGateDBServiceMsg.h"
#include "MonitorCtxHandler.h"
#include "UREngineConfig.h"
#include "xGateSipMsg.h"
#include "xGateSchedulerServiceMsg.h"
#include "xGateSchedulerMsg.h"

constexpr const char* jbegin {"{\""};
constexpr const char* jend{"\"}"};
constexpr const char* jcomma{"\",\""};
constexpr const char* jcolon{"\":\""};
constexpr const char* jicolon{"\":"};
constexpr const char* jicomma{",\""};
constexpr const char* jiscomma{","};
constexpr const char* jiend{"}"};


#define MNTRHNDLR xGateMonitorHandler::instance()

class xGateMonitorHandler final : public XGSingleton<xGateMonitorHandler>
{
    public:
        explicit xGateMonitorHandler(typename XGSingleton<xGateMonitorHandler>::token);
        ~xGateMonitorHandler();
        
        bool init();
        bool ProcessMsg(xGateBaseMsg *pMsg);

    private:
        bool ProcessMsgFromCtrl(xGateBaseMsg *pBaseMsg);
        bool ProcessMsgFromDBSrv(xGateBaseMsg *pBaseMsg);
        bool ProcessMsgFromSipSrv(xGateBaseMsg *pBaseMsg);
        bool ProcessMsgFromSchedulerSrv(xGateBaseMsg *pBaseMsg);
        bool ProcessMsgFromNetIOSrv(xGateBaseMsg *pBaseMsg);
        bool FetchTrunkDetailsFromDBSrv();
        bool FetchMSTeamsDomainListFromDBSrv();
        bool updateTrunkStatusToDB(MonitorContext &mntrctx,int status,int respcode,std::string uid);
        bool updateTrunkStatusToNetIO(MonitorContext &mntrctx);
        bool sendOptionsRequestToSipSrv(MonitorContext &mntrctx);
        bool startTimerRequestToSchedulerSrv(MonitorContext &mntrctx);

        bool handleTrunkLoadMsg(DBRequestInfo &reqinfo);
        bool handleOptionsSuccessMsg(xGateSipMsg *sipmsg);        
        bool handleOptionsFailedMsg(xGateSipMsg *sipmsg);
        bool handleOptionsTimeoutMsg(xGateSipMsg *sipmsg);
        bool handleSchedulerTimeoutMsg(xGateTimerData *timerdata);
        bool addNewTrunk(xGateMonitorInfo mntrInfo);
        bool removeTrunk(xGateMonitorInfo mntrInfo);
        bool handleMsTeamsDomainListMsg(DBRequestInfo &reqinfo);

        //Rapid Json helper functions 
        bool parseJsonMsg(const char *jsonData, Document &doc);
        template <typename T>
        inline void delete_obj(T *obj);

        MonitorCtxHandler *pCtxHndlr;    
};