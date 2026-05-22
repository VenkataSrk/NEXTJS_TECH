#ifndef LOG_HANDLER
#define LOG_HANDLER
#include "CAUtilityInternal.h"
#include "logger.h"



enum class  LOG_LEVEL{
    LOGTRACE=0,
    LOGDEBUG,
    LOGINFO,
    LOGWARN,
    LOGERR,
    LOGCRIT,
    LOGFAT,
    LOGEXP,
    LOGOFF
};
enum class LOGEVENT{
    LOG_INIT_GLOBAL = 0,
    LOG_INIT_SESSION,
    LOG_DEINIT_GLOBAL,
    LOG_DEINIT_SESSION,
    LOG_DATA


};

class logMsgBlock : public CA::CAMessageBlock{
    public :
    logMsgBlock()=default;
    virtual ~logMsgBlock()=default;
    logMsgBlock(const std::string &uuid,const std::string &loglvl,const std::string &app,const std::string &cli,const std::string &ddi,const std::string &trunk){
        m_logLevel = loglvl; 
        m_uuid   = uuid;
        m_AppName = app;
        m_Cli =  cli;
        m_DDi = ddi;
        m_trunk = trunk;
    }
    logMsgBlock(std::string &uuid,std::string &loglvl){
       m_logLevel = loglvl; 
        m_uuid   = uuid; 
    }

    void setData(const char* ldata){
        //m_Data = ldata;
        try{
            m_Data.assign(ldata, strlen(ldata));
        }catch(...){
            
        }
    }

    std::string m_logLevel;
    std::string m_uuid;
    std::string m_AppName;
    std::string m_Cli;
    std::string m_DDi;
    std::string m_trunk;
    std::string m_Data;

};

class LogHandler : public CA::CAThread{
    private:
    std::string m_logLevel;
    std::string m_logPath;
    std::string m_logSize;
    std::string m_logRetention;
    CA::CAMap<logger*> m_fdList;
    public:
    bool m_run;
    LogHandler()=default;
    virtual ~LogHandler()=default;
    LogHandler(std::string level, std::string path,std::string size, std::string lifetime);
    virtual bool run();
    void handleMsg(CA::CAMessageBlock*);
    void processLogRequest(CA::CAMessageBlock*);
    void init();
    void sessionInit(logMsgBlock*pMsg);
    void sessionDeInit(logMsgBlock*pMsg);
    void insertLogger(logger* logObj,std::string uuid);
    void removerLogger(std::string uuid);
    logger* getLoggerObject(std::string uuid);
    void wrtieLog(logMsgBlock*);
    std::string getConfLogLevel(){return m_logLevel;}
};
class vlog{
    private:
    static LogHandler* m_loghld ;
    public:
    static void logstart(std::string level, std::string path,std::string size, std::string lifetime);
    //static void formatLog(LOG_LEVEL lvl, std::string uuid,std::string app,std::string cli,std::string ddi,std::string trunk, const char* fmt...);
    static void formatLog(LOG_LEVEL lvl, std::string uuid, const char* fmt...);
    static LogHandler* getloghandle(){ return m_loghld;}
    static void logservice(LOGEVENT evt,std::string uuid,std::string app,std::string cli,std::string ddi,std::string trunk);
};








#define logdebug(uid,fmt,...) vlog::formatLog(LOG_LEVEL::LOGDEBUG,uid,fmt,##__VA_ARGS__) 
#define logtrace(uid,fmt,...) vlog::formatLog(LOG_LEVEL::LOGTRACE,uid,fmt,##__VA_ARGS__) 
#define loginfo(uid,fmt,...) vlog::formatLog(LOG_LEVEL::LOGINFO,uid,fmt,##__VA_ARGS__) 
#define logerror(uid,fmt,...) vlog::formatLog(LOG_LEVEL::LOGERR,uid,fmt,##__VA_ARGS__)
#define logwarn(uid,fmt,...) vlog::formatLog(LOG_LEVEL::LOGWARN,uid,fmt,##__VA_ARGS__) 
#define logcritial(uid,fmt,...) vlog::formatLog(LOG_LEVEL::LOGCRIT,uid,fmt,##__VA_ARGS__) 
#define logexp(uid,fmt,...) vlog::formatLog(LOG_LEVEL::LOGEXP,uid,fmt,##__VA_ARGS__) 
#define logsessioninit(uid,app,cli,ddi,trunk) vlog::logservice(LOGEVENT::LOG_INIT_SESSION,uid,app,cli,ddi,trunk)
#define logsessiondeinit(uid,app,cli,ddi,trunk) vlog::logservice(LOGEVENT::LOG_DEINIT_SESSION,uid,app,cli,ddi,trunk)
#define logstart(lvl,path,size,retPrd) vlog::logstart(lvl,path,size,retPrd)
#endif
