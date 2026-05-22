#include "logger.h"

logger::logger(std::string fileName){
    createGlobalfile(fileName);
}

void logger::getFormattedDateTime(){
    std::time_t now =  std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    char buf[16] = {0};
    std::strftime(buf, sizeof(buf),"%d%m%Y", std::localtime(&now));
    m_date = std::string(buf);

    char buf1[16] = {0};
    std::strftime(buf1,sizeof(buf1),"%H%M%S", std::localtime(&now));

    m_time  =  std::string(buf1);
    
}
void logger::createGlobalfile(std::string path){
    try{
        if(path.empty())
            path = "//var//log";
        m_path = path;
        if(path.back() != '/'){
                path += "/";
            }
        
        getFormattedDateTime();
        path += m_date + "/";
        int status;
        std::string cmd  =  "mkdir -p " + path;
        status = system(cmd.c_str());
        if(status == -1)
            throw "No Directory Found";
        // create file name
        
        std::string logName =  path +"buslog_"+m_time + ".log";
        
        if(m_fd.is_open()){
            m_fd.flush();
            closeFile();
        }
        m_fd.open(logName,std::ios::out | std::ios::app);
    }catch(std::exception &e){

    }catch(...){

    }
    
}
void logger::createSessionFile(std::string path, std::string cli,std::string uid){
    try{
        if(path.empty())
                path = "//var//log";
        if(path.back() != '/'){
                    path += "/";
        } 

        getFormattedDateTime();
        path += m_date + "/";
        int status;
        std::string cmd  =  "mkdir -p " + path;
        status = system(cmd.c_str());
        if(status == -1)
            throw "No Directory Created";
        if(cli.empty())
            cli = "sessionlog_" + m_time;
        std::string logName = path + cli + "_"+m_ddi+ ".log"; 
        m_fd.open(logName,std::ios::out | std::ios::app);
    }catch(...){
        
    }
}

void logger::updateSessionInfo(std::string uuid,std::string app,std::string cli,std::string ddi,std::string trunk){
    if(!uuid.empty()){
        m_uuid = uuid;
    }
    if(!app.empty()){
        m_appName = app;
    }
    if(!cli.empty()){
        m_cli = cli;
    }
    if(!ddi.empty()){
        m_ddi = ddi;
    }
    if(!trunk.empty()){
        m_trunk = trunk;
    }
}

void logger::formatLog(std::string loglvl, std::string logdata){
    try{
    if(logdata.empty())
        throw "No Data found";
    std::stringstream strfmt;
// getcurrent time 
    std::time_t now =  std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    char buf[20] = {0};
    char strFormattedData[FOMDATA] =  {0};
    std::strftime(buf,sizeof(buf),"%d-%m-%Y %H:%M:%S", std::localtime(&now));
    std::snprintf(strFormattedData,FOMDATA, "%s [%s] %s %s\n",buf,loglvl.c_str(),m_uuid.c_str(),logdata.c_str());
   
    if(m_uuid.compare(GLID) == 0){
        memset(buf,sizeof(buf),'\0');
        std::strftime(buf,sizeof(buf),"%d%m%Y", std::localtime(&now));
        if(m_date.compare(buf) != 0){
            createGlobalfile(m_path);
        }
    }
    
   
    //ACE_LOG_MSG->msg_ostream(&m_fd,false);
    ACE_LOG_MSG->msg_ostream(&m_fd); //need to observe 
    ACE_LOG_MSG->clr_flags(ACE_Log_Msg::STDERR | ACE_Log_Msg::LOGGER);//To disable Console Log
    ACE_LOG_MSG->set_flags(ACE_Log_Msg::OSTREAM);
    ACE_LOG_MSG->priority_mask(LM_INFO | LM_DEBUG | LM_ERROR | LM_WARNING | LM_CRITICAL | LM_EMERGENCY | LM_NOTICE | LM_TRACE, ACE_Log_Msg::PROCESS);
    ACE_Log_Priority aceLevel = getLogLevelEnum(loglvl);
    ACE_DEBUG((aceLevel, "%s", ACE_TEXT(strFormattedData)));
 
    //writeData(strFormattedData);  

    
    }catch(...){

    }
}

void logger::writeData(std::string Data){
    try{
    if(!Data.empty()){
        if(m_fd.is_open()){
         m_fd.write(Data.c_str(), Data.length());
         m_fd.flush();  
        }
    }
    }catch(...){
        
    }
}

long logger::checkFileSize(){
    if(m_fd){
        return m_fd.tellp();
    }
    return 0;
}

void logger::closeFile(){
    if(m_fd){
        if(m_fd.is_open()){
            m_fd.flush();
            m_fd.close();
        }
    }
}
ACE_Log_Priority logger::getLogLevelEnum(const std::string& levelStr) {
    if (levelStr == "TRACE") return LM_TRACE;
    if (levelStr == "DEBUG") return LM_DEBUG;
    if (levelStr == "INFO")  return LM_INFO;
    if (levelStr == "WARN")  return LM_WARNING;
    if (levelStr == "ERROR") return LM_ERROR;
    if (levelStr == "CRIT")  return LM_CRITICAL;
    if (levelStr == "FATAL") return LM_CRITICAL;
    if (levelStr == "EXPE")  return LM_ERROR;
    if (levelStr == "GENE")  return LM_INFO;
    return LM_INFO; //default one loginfo
}
