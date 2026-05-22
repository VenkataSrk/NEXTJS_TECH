#ifndef BUSINESS_SERVER
#define BUSINESS_SERVER
#include "CAUtilityInternal.h"
#include "BusinessConfigService.h"
#include "MVNOBusinessService.h"
#include "BSRequestProcessor.h"
#include "BSDBService.h"
#include "ChargingService.h"
#include "RoutingService.h"
#include "BSmomtGatewayService.h"
#include "ReportingService.h"
#include "BSLocalDBService.h"
#include "BSHttpService.h"
#include "GmscService.h"

enum class BUSINESS_THREADS{
    UNKNOWN,
    MVNO_BUS,
    MVNO_CONF,
    MVNO_BSP,
    MVNO_BSDB,
    MVNO_CHARGE,
    MVNO_GMSC,
    MVNO_ROUTE,
    MVNO_BSMOMT,
    MVNO_BSREP,
    MVNO_BSLOCDB,
    MVNO_HTTP,

};

enum class BS_SERVICES{
    BS_NONE=0,
    BS_MVNO_SOCKET,
    BS_REQ_PROCESS,
    BS_CHARGING_PROCESS,
    BS_SWITCHROUTING_PROCESS,
    BS_GMSC_PROCESS,
    BS_MOMT_GATEWAY,
    BS_EXCEPTION_PROCESS,
    BS_LOCAL_DB_PROCESS,
    BS_HTTP_PROCESS,
};
enum class BS_TIMER_EVENT{
    BS_TIME_NONE = 0,
    BS_CALL_TIMEOUT, // event before connection
    BS_CALL_TIMECHARGING, // evnt only for charging service on regular cycle after connect
    BS_CALL_TIMEMAXDURATION, // event for a call on max duration call connection
    BS_CALL_TIMECLEARIND, // event to handle clear indication to delay delete process of call object 
    BS_DB_TIMEOUT 
};

class BSCallTimerMsg : public CA::CAMessageBlock{
    public:
    int event;
    int taskId;
    std::string m_uuid;
    
    BSCallTimerMsg() =default;
    virtual ~BSCallTimerMsg()=default;

};

class BusinessServer{
    
    public:
    BusinessServer()=default;
    virtual ~BusinessServer()=default;
    static void setBusinessService(MVNOBusinessService* businessService){
        if(businessService){
            m_businessService =  businessService;

        }else
            m_businessService = nullptr;
    }

    static MVNOBusinessService* getBusinessService(){
        if(m_businessService)
            return m_businessService;
        
        return nullptr;
    }
    static void setConfigService(BusinessConfigService* configService){
        if(configService){
            m_configService =  configService;
        }else{
            m_configService =  nullptr;
        }

    }
    static BusinessConfigService* getConfigService(){
        if(m_configService)
            return m_configService;
        return nullptr;
    }
    static void setBusinessProcessor(BSRequestProcessor* bspThread){
        m_bsrProcessor = bspThread;
    }

    static BSRequestProcessor* getBusinessProcessor(){
        if(m_bsrProcessor){
            return m_bsrProcessor;
        }
        return nullptr;
    }
    static void setDBService(DBService* dbSer){
        m_dbService =  dbSer;
    }

    static DBService* getDBService(){
        if(m_dbService)
            return m_dbService;
        
        return nullptr;
    }
    static void setChargingService(ChargingService* chargingSer){
        m_chargingService =  chargingSer;
    }

    static ChargingService* getChargingService(){
        if(m_chargingService)
            return m_chargingService;
        
        return nullptr;
    }
    static void setRoutingService(RoutingService* routingSer){
        m_routingService =  routingSer;
    }

    static RoutingService* getRoutingService(){
        if(m_routingService)
            return m_routingService;
        
        return nullptr;
    }

    static void setBSmomtGWService(BSmomtGatewayService* momtService){
        m_momtService =  momtService;
    }

    static BSmomtGatewayService* getmomtGWService(){
        if(m_momtService)
            return m_momtService;
        return nullptr;
    }
    
    static void setBSLocalDBService(BSLocalDBService* localDBService){
        m_localDBService =  localDBService;
    }

    static BSLocalDBService* getBSLocalDBService(){
        if(m_localDBService)
            return m_localDBService;
        return nullptr;
    }

    static void setBSReportingService(BSReportingService* reportService){
        m_reportService =  reportService;
    }

    static BSReportingService* getBSReportingService(){
        if(m_reportService)
            return m_reportService;
        return nullptr;
    }

    static void setHttpService(BSHttpService* httpService){
        m_httpService = httpService;
    }

    static BSHttpService* getHttpService(){
        if(m_httpService)
            return m_httpService;
        return nullptr;
    }

    static GmscService* getGmscService(){
        if(m_gmscService)
        {
            return m_gmscService;
        }
        return nullptr;
    }

    static void setGmscService(GmscService* gmscService)
    {
        m_gmscService = gmscService;
    }
    

    static bool post(BUSINESS_THREADS enTid,std::unique_ptr<CA::CAMessageBlock> pMsg){

        switch (enTid){
        case BUSINESS_THREADS::MVNO_BUS:
            if(getBusinessService())
                getBusinessService()->PostMessage(std::move(pMsg));
            break;
        case BUSINESS_THREADS::MVNO_CONF:
        if(getConfigService())
            getConfigService()->PostMessage(std::move(pMsg));
        break;
        case BUSINESS_THREADS::MVNO_BSP:
        if(getBusinessProcessor())
            getBusinessProcessor()->PostMessage(std::move(pMsg));
        break;
        case BUSINESS_THREADS::MVNO_BSDB:
        if(getDBService())
            getDBService()->PostMessage(std::move(pMsg));
        break;
        case BUSINESS_THREADS::MVNO_CHARGE:
        if(getChargingService())
            getChargingService()->PostMessage(std::move(pMsg));
        break;
        case BUSINESS_THREADS::MVNO_ROUTE:
        if(getRoutingService())
            getRoutingService()->PostMessage(std::move(pMsg));
        break;
        case BUSINESS_THREADS::MVNO_BSMOMT:
        if(getmomtGWService())
            getmomtGWService()->PostMessage(std::move(pMsg));
        break;
        case BUSINESS_THREADS::MVNO_BSREP:
        if(getBSReportingService())
            getBSReportingService()->PostMessage(std::move(pMsg));
        case BUSINESS_THREADS::MVNO_BSLOCDB:
        if(getBSLocalDBService())
            getBSLocalDBService()->PostMessage(std::move(pMsg));
            break;
        case BUSINESS_THREADS::MVNO_HTTP:
        if(getHttpService())
            getHttpService()->PostMessage(std::move(pMsg));
        break;
        case BUSINESS_THREADS::MVNO_GMSC:
        {
            if(getGmscService())
            {
                getGmscService()->PostMessage(std::move(pMsg));
            }
            break;
        }
        default:
            break;
        }
        return true;
    }

    static void setTimerEvent(int timeOut,int timeEvent,std::string uuid, int taskID){
         
    std::string callid = uuid;
    int timer =  timeOut;
    int event = timeEvent;
    int taskId =  taskID;
    CA::CAThread bstimer(([callid, timer,event,taskId](){
        std::this_thread::sleep_for(std::chrono::seconds(timer));
        std::unique_ptr<BSCallTimerMsg> pMsg(new BSCallTimerMsg);
        pMsg->event =  event;
        pMsg->m_uuid =  callid;
        pMsg->taskId =  taskId;
        pMsg->setMsgType(MESSAGEEVENT::MSG_TIMER);
        
        BusinessServer::post(BUSINESS_THREADS::MVNO_BSP,std::move(pMsg));
    }));
    }

    private:
    static MVNOBusinessService* m_businessService ;
    static BusinessConfigService* m_configService;
    static BSRequestProcessor* m_bsrProcessor;
    static DBService* m_dbService;
    static ChargingService* m_chargingService;
    static RoutingService* m_routingService;
    static BSmomtGatewayService* m_momtService;
    static BSReportingService* m_reportService;
    static BSLocalDBService* m_localDBService; 
    static BSHttpService* m_httpService;
    static GmscService* m_gmscService;
    
};





#endif
