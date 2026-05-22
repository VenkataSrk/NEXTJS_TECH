#include "LogHandler.h"

LogHandler::LogHandler(std::string level, std::string path,std::string size, std::string lifetime){
    m_logLevel = level;
    m_logPath =  path;
    m_logSize =  size;
    m_logRetention =  lifetime;
}
bool LogHandler::run(){
    const std::string thread_name = "LogHandler";
    int rc = pthread_setname_np(pthread_self(), thread_name.c_str());
    if (rc != 0) {
            std::cerr << "Failed to set name for thread: " << strerror(rc) << std::endl;
    } else {
            std::cout << "Thread name set successfully to: " << thread_name << std::endl;
    }
    while(getThreadStatus()){
        CA::CAMessageBlock * pMsg = (CA::CAMessageBlock *)RecvMessage();
        if(pMsg){
            handleMsg(pMsg);
            delete pMsg;
            pMsg =  nullptr;
        }
        usleep(10);
    }
    return true;
}
void LogHandler::handleMsg(CA::CAMessageBlock*pMsg){
    try{
        
        if(!pMsg)
            throw pMsg;
        switch(pMsg->getMsgType()){
            case MESSAGEEVENT::MSG_INIT:
                init();
            break;
            case MESSAGEEVENT::MSG_LOG:
                processLogRequest(pMsg);
            break;
            default:
            throw "Unkown Message Event";
            break;
        }
            

    }catch(std::exception &e){

    }catch(...){
        // unknown exception or bad pointer error
    }
}
void LogHandler::processLogRequest(CA::CAMessageBlock*pMsg){
    try{
        if(!pMsg)
            throw "Message Object is Null";
        logMsgBlock *pMsgBlock =  (logMsgBlock*)pMsg;
        if(!pMsgBlock)
            throw "Block Msg convertion failed";
        switch((LOGEVENT)pMsgBlock->getEvent()){
            case LOGEVENT::LOG_INIT_SESSION:
                sessionInit(pMsgBlock);
            break;
            case LOGEVENT::LOG_DEINIT_SESSION:
                sessionDeInit(pMsgBlock);
            break;
            case LOGEVENT::LOG_DATA:
            wrtieLog(pMsgBlock);
            break;
            default:
                throw "Unknow Event in Log";
            break;

        }

    }catch(...){

    }
}

void LogHandler::init(){
    try{
        logger* gLog =  new logger(m_logPath);
        if(!gLog)
            throw gLog;
        gLog->updateSessionInfo(GLID,"GLOBAL","","","");
        insertLogger(gLog,GLID);
        

    }catch(...){

    }

}

void LogHandler::wrtieLog(logMsgBlock*pMsgBlock){
    try{
       
        
        if(!pMsgBlock)
            throw "log msg block is NULL";
        if(pMsgBlock->m_uuid.empty()){
            pMsgBlock->m_uuid =  GLID;
        }
        logger* wlog = getLoggerObject(pMsgBlock->m_uuid);
        if(!wlog)
            throw "Log Object Memory creation error";
        wlog->formatLog(pMsgBlock->m_logLevel,pMsgBlock->m_Data );
         //wlog->writeData(pMsgBlock->m_Data);
         
        
         

    }catch(...){
            std::cout << "Exception occured";
    }
}
void LogHandler::sessionDeInit(logMsgBlock*pMsgBlock){
    try{
        if(!pMsgBlock)
            throw "log msg block is NULL";
        logger* wlog = getLoggerObject(pMsgBlock->m_uuid);
        
        if(wlog){
            wlog->closeFile();
            removerLogger(pMsgBlock->m_uuid);
            delete wlog;
            wlog = nullptr;
        }
    }catch(...){

    }
}

void LogHandler::sessionInit(logMsgBlock*pMsgBlock){
    try{
         if(!pMsgBlock)
            throw "log msg block is NULL";
        logger* wlog = getLoggerObject(pMsgBlock->m_uuid);
        if(wlog){
            wlog->updateSessionInfo(pMsgBlock->m_uuid,pMsgBlock->m_AppName,pMsgBlock->m_Cli,pMsgBlock->m_DDi,pMsgBlock->m_trunk);
        }else{
            wlog = new logger();
            if(!wlog)
                throw "Error creation logger object memeory";
            wlog->updateSessionInfo(pMsgBlock->m_uuid,pMsgBlock->m_AppName,pMsgBlock->m_Cli,pMsgBlock->m_DDi,pMsgBlock->m_trunk);
            wlog->createSessionFile(m_logPath,pMsgBlock->m_Cli,pMsgBlock->m_uuid);
            insertLogger(wlog,pMsgBlock->m_uuid);
        }
        

    }catch(...){

    }

}

void LogHandler::insertLogger(logger* logObj,std::string uuid){
    try{
        if(!logObj){
            throw logObj;
        }
        if(uuid.empty())
            throw "no unique ID";
        m_fdList.InsertToMap(uuid,logObj);

    }catch(...){
        // unknown exception 
    }

}
void LogHandler::removerLogger(std::string uuid){
    try{
        if(uuid.empty())
            throw "no unique ID";
        m_fdList.DeleteFromMap(uuid);
        
    }catch(...){

    }

}

logger* LogHandler::getLoggerObject(std::string uuid){
    logger* logObj = nullptr;
    try{

        logObj =  m_fdList.FindValue(uuid);

    }catch(...){
        // unknown exception
        return nullptr;
    }

    return logObj;
}


/*----------------------------------------------------------------------------------------------*/
LogHandler* vlog::m_loghld =  nullptr;

void vlog::logstart(std::string level, std::string path,std::string size, std::string lifetime){
    m_loghld = nullptr;
    m_loghld =  new LogHandler(level,path,size,lifetime);
        if(m_loghld){
            m_loghld->setThreadStatus(true);
            if(m_loghld->spwan(*m_loghld)){
                std::unique_ptr<CA::CAMessageBlock> pMsg(new CA::CAMessageBlock);
                pMsg->setMsgType(MESSAGEEVENT::MSG_INIT);
                m_loghld->PostMessage(std::move(pMsg));
            }
        }
    }
void vlog::formatLog(LOG_LEVEL lvl, std::string uuid, const char* fmt...){
    try{
        if(getloghandle() == nullptr)
            throw "Invalid Config Handle";
        std::string loglvl = getloghandle()->getConfLogLevel();
        if( loglvl.find(std::to_string((int)lvl)) == std::string::npos)
            return;
        
        std::string strLogLvl;
        switch(lvl){
            case LOG_LEVEL::LOGTRACE:
            strLogLvl = "TRACE";
            break;
            case LOG_LEVEL::LOGDEBUG:
            strLogLvl = "DEBUG";
            break;
            case LOG_LEVEL::LOGINFO:
            strLogLvl = "INFO";
            break;
            case LOG_LEVEL::LOGWARN:
            strLogLvl = "WARN";
            break;
            case LOG_LEVEL::LOGERR:
            strLogLvl = "ERROR";
            break;
            case LOG_LEVEL::LOGCRIT:
            strLogLvl = "CRIT";
            break;
            case LOG_LEVEL::LOGFAT:
            strLogLvl = "FATAL";
            break;
            case LOG_LEVEL::LOGEXP:
            strLogLvl = "EXPE";
            break;
            case LOG_LEVEL::LOGOFF:
            default:
            strLogLvl = "GENE";
            break; 
        }
        va_list argList;
        char info[MAXDATA]= {0};
        va_start(argList, fmt);
        vsnprintf(info,MAXDATA,fmt,argList);
        va_end(argList);

        std::unique_ptr<logMsgBlock> pMsg(new logMsgBlock(uuid,strLogLvl));
        if(pMsg){
            pMsg->setMsgType(MESSAGEEVENT::MSG_LOG);
            pMsg->setEvent((unsigned long)LOGEVENT::LOG_DATA);
            pMsg->setData(info);
            if(getloghandle() == nullptr)
                throw "Invalid Config Handle";
            getloghandle()->PostMessage(std::move(pMsg));
        }
    }catch(...){

    }
}
void vlog::logservice(LOGEVENT evt,std::string uuid,std::string app,std::string cli,std::string ddi,std::string trunk){
    try{
        std::unique_ptr<logMsgBlock> pMsg(new logMsgBlock(uuid,"",app,cli,ddi,trunk));
         pMsg->setMsgType(MESSAGEEVENT::MSG_LOG);
        pMsg->setEvent((unsigned long)evt);
        if(getloghandle() == nullptr)
            throw "Invalid Config Handle";
        getloghandle()->PostMessage(std::move(pMsg));
    }catch(...){

    }

}
