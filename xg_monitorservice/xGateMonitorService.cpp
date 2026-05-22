#include "xGateMonitorHandler.h"
#include "xGateMonitorService.h"
#include "xGateUtil.h"
#include "xglog.h"

xGateMonitorService::xGateMonitorService() : ACE_Task<ACE_MT_SYNCH>(),
                                             m_run(false), m_dbConfig(0)
{
    m_strModuleName = "MntrServiceModule";
}

xGateMonitorService::~xGateMonitorService(void)
{
}

int xGateMonitorService::initModule(IURModuleConfig *pMntrConfig)
{
    m_dbConfig = pMntrConfig;
    init();
    return IURDefines::UR_RETURN_SUCCESS;
}

int xGateMonitorService::unitModule()
{
    stop();
    return IURDefines::UR_RETURN_SUCCESS;
}

void xGateMonitorService::getModuleName(std::string &moduleName)
{
    moduleName = m_strModuleName;
}

IURDefines::MODULE_ID xGateMonitorService::getModuleID()
{
    return IURDefines::UR_MODULE_MONITOR_SERVICE;
}

IURModuleConfig *xGateMonitorService::getModuleConfig()
{
    return m_dbConfig;
}

bool xGateMonitorService::stop(void)
{
    m_run = false;
    msg_queue()->deactivate();
    wait();
    XGLOG_INFO("xGateMonitorService stopped !");
    return true;
}

bool xGateMonitorService::init()
{
    XGLOG_INFO("xGateMonitorService::init() called");
    m_run = true;

    // Start the Monitor thread.
    if (activate(THR_NEW_LWP, 1) == -1)
    {
        XGLOG_ERROR("xGateMonitorService::init() failed to start thread !");
        return false;
    }
    else
    {
        XGLOG_INFO("xGateMonitorService::init() success !");
    }

    MNTRHNDLR.init();

    return true;
}

int xGateMonitorService::svc(void)
{
    int rc;
    rc = pthread_setname_np(ACE_Thread::self(), "MntrSrvc");
    if (rc != 0)
        printf("Failed to set name for monitor service thread");

    ACE_Message_Block *pAmb = 0;

    while (m_run)
    {
        ACE_Time_Value tv((time(nullptr) + 20));
        int result = getq(pAmb, &tv);
        if ((result == -1) && (errno == EWOULDBLOCK))
        {
            continue;
        }
        if ((result != -1))
        {
            handle_msg(pAmb);
        }
        else
        {
            XGLOG_ERROR("xGateMonitorService::svc Error in Message Queue");
            m_run = false;
        }
        // delete the message received
        if (pAmb)
        {
            delete pAmb;
            pAmb = nullptr;
        }
    }

    XGLOG_INFO("xGateMonitorService::svc() stopped");
    return 0;
}

bool xGateMonitorService::handle_msg(ACE_Message_Block *pAmb)
{
    xGateBaseMsg *pMsg = nullptr;
    pMsg = dynamic_cast<xGateBaseMsg *>(pAmb);
    if (pMsg == nullptr)
    {
        XGLOG_ERROR("xGateMonitorService::handle_msg recvd invalid xGBaseMsg Message");
        return false;
    }
    MNTRHNDLR.ProcessMsg(pMsg);

    return true;
}

int xGateMonitorService::pushModuleMsg(IURModuleMsg *pdbMsg)
{
    XGLOG_INFO("xGateMonitorService::inside pushModuleMsg");
    if (!pdbMsg)
    {
        XGLOG_ERROR("pushModuleMsg::Message not received from controller.!");
        return IURDefines::UR_RETURN_ERROR;
    }
    else
    {
        XGLOG_DEBUG("xGateMonitorService::Message received from controller to Monitor service");
        xGateMonitorService *mtrService = MNTRSERVICE;
        mtrService->putq(dynamic_cast<ACE_Message_Block *>(pdbMsg));
    }
    return IURDefines::UR_RETURN_SUCCESS;
}
