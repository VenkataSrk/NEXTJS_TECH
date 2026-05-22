#include <fstream>
#include <stdexcept>
#include <filesystem>
#include "xGateUtil.h"
#include "logmsg.h"
#include "xglog.h"
#include "LogProfile.h"

XGLog::XGLog() :
  ACE_Task<ACE_MT_SYNCH>(),
  m_run(false), m_logStream(NULL), m_filePath(""),m_fileCount(0)
{
}

XGLog::~XGLog(void)
{
}

bool XGLog::stop(void)
{

  m_run = false;
  msg_queue()->deactivate();
  wait();

  return true;
}

bool XGLog::init()
{
  m_run = true;
  create_file();

  // Start the log thread.
  if (activate(THR_NEW_LWP, 1) == -1 )
  {
    return false;
  }
  else
  {
  }
  printf("XGLog::init() !\n");
  return true;
}


bool XGLog::loadlogProfile(LogProfile &profile){
  m_logProfile = profile;
  return true;
}

bool XGLog::create_file()
{
        ACE_LOG_MSG->priority_mask(LM_INFO | LM_DEBUG | LM_ERROR | LM_WARNING | LM_CRITICAL | LM_EMERGENCY | LM_NOTICE | LM_TRACE, ACE_Log_Msg::PROCESS);
        if(m_fileCount>=m_logProfile.m_fileCount)
        {
                m_fileCount=1;
        }
        else
        {
                m_fileCount+=1;
        }
        timeval curTime;
        ::gettimeofday(&curTime, NULL);
        char buffer[80]={0};
        memset(buffer,0,sizeof(buffer));
        char file_name[85]={0};
        memset(file_name,0,sizeof(file_name));
        char current_date[80] = {0};
        memset(current_date, 0, sizeof(current_date));
        time_t timenow;
        strftime(current_date, 80, "%d-%m-%y", localtime(&curTime.tv_sec));
        m_currentDate = current_date;
        std::string logDir = string("/var/log/xgate/log/") + m_currentDate + string("/");
        std::filesystem::create_directories(logDir);

        strftime(buffer, 80, "%d_%m_%y-%H%M%S", localtime(&curTime.tv_sec));
        snprintf(file_name,85 , "%s_SRS_LOG-%03d.TXT", buffer,m_fileCount);

        m_filePath = logDir + file_name;

        list<string> :: iterator itr;
        m_logStream = new std::ofstream();
        m_logStream->open(m_filePath.c_str(), ios::out | ios::app);
        ACE_LOG_MSG->msg_ostream(m_logStream);
        ACE_LOG_MSG->clr_flags(ACE_Log_Msg::STDERR | ACE_Log_Msg::LOGGER);//To disable Console Log 
        ACE_LOG_MSG->set_flags(ACE_Log_Msg::OSTREAM);
        return true;
}




int XGLog::svc(void)
{
  ACE_Message_Block *pAmb = 0;

  while(m_run)
  {
    pAmb = NULL;
    ACE_Time_Value tv ((time(NULL) + 20));
    int result = getq(pAmb, &tv);
    if ( ( result == -1 ) && ( errno == EWOULDBLOCK ) )
    {
      continue;
    }
    if ((result != -1))
    {
      handle_msg(pAmb);
    }
    else
    {
     m_run = false;
    }
    // delete the message received
    if(pAmb)
    {
      delete pAmb;
      pAmb = NULL;
    }
  }

  return 0;
}

bool XGLog::handle_msg(ACE_Message_Block *pAmb)
{
  xGateLogMsg *pMsg = dynamic_cast<xGateLogMsg*>(pAmb);
  LogLevel logLevel = pMsg->getLogLevel(); 
  std::string &fmt = pMsg->getFmt();


  char current_time[84] = "";
  memset(current_time,0,sizeof(current_time));
  char buffer [80];
  memset(buffer,0,sizeof(buffer));
  char current_date[80] = {0};
  memset(current_date, 0, sizeof(current_date));
  timeval curTime;
  ::gettimeofday(&curTime, NULL);
  int milli = curTime.tv_usec / 1000;
  strftime(buffer, 80, "%y-%m-%d %H:%M:%S", localtime(&curTime.tv_sec));
  snprintf(current_time, 84, "%s.%03d", buffer, milli);
  strftime(current_date, 80, "%d-%m-%y", localtime(&curTime.tv_sec));

  char logMsg[MAX_SIZE_PER_MSG] = {0};
  memset(logMsg,0,MAX_SIZE_PER_MSG);
  snprintf(logMsg, MAX_SIZE_PER_MSG, "%s %s", current_time, fmt.c_str());


#if 1 //Log Rotation Logic based on file size
        ifstream in_file (m_filePath,ios::binary);
        in_file.seekg(0, ios::end);
        int file_size = in_file.tellg();
        m_logProfile.m_fileSize = 20000000 ; //fize size KB 
        if (file_size>=m_logProfile.m_fileSize || m_currentDate != current_date)
        {
          m_logStream->close();
         // rotateLog();
          create_file();
        } 
#endif


  // On the server processor, we handle  
  switch(logLevel)
  {
    case EN_LOG_LEVEL_TRACE:
      {
        ACE_DEBUG((LM_TRACE, ACE_TEXT(logMsg)));
        break;
      }
    case EN_LOG_LEVEL_DEBUG:
      {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT(logMsg)));
        break;
      }
    case EN_LOG_LEVEL_INFO:
      {
        ACE_DEBUG((LM_INFO, ACE_TEXT(logMsg)));
        break;
      }
    case EN_LOG_LEVEL_WARN:
      {
        ACE_DEBUG((LM_WARNING, ACE_TEXT(logMsg)));
        break;
      }
    case EN_LOG_LEVEL_ERROR:
      {
        ACE_DEBUG((LM_ERROR, ACE_TEXT(logMsg)));
        break;
      }
    case EN_LOG_LEVEL_CRITICAL:
      {
        ACE_DEBUG((LM_CRITICAL, ACE_TEXT(logMsg)));
        break;
      }
    case EN_LOG_LEVEL_FATAL:
      {
        ACE_DEBUG((LM_EMERGENCY, ACE_TEXT(logMsg)));
        break;
      }
    case EN_LOG_LEVEL_NOTICE:
      {
       ACE_DEBUG((LM_NOTICE, ACE_TEXT(logMsg)));
       break;
      }
    default:
      {
        XGLOG_ERROR("Unknown log message type %d might not been implemented yet !", \
            logLevel);
        break;
      }
  }
  return true;
}

#if 0
bool XGLog::rotateLog()
{
    string temp_file;
    list<string> :: iterator itr;
    if(m_fileList.size()>=m_logProfile.m_fileCount)
    {
    itr=m_fileList.begin();
    temp_file=*itr;
//    std::cout<<temp_file<<std::endl;
    m_fileList.erase(itr);
    remove(temp_file.c_str());
  }

  create_file();
  return true;
}
#endif 


bool XGLog::write(LogLevel logLevel, const char* verbos, const char* fileName, \
    unsigned int lineNum, const char* fmt...)
{
  bool retVal = false;
  va_list argList;
  char info[MAX_SIZE_PER_MSG] = {0};
  memset(info,0,MAX_SIZE_PER_MSG);
  char fullInfo[MAX_SIZE_PER_MSG] = {0};
  memset(fullInfo,0,MAX_SIZE_PER_MSG);

  try { //TODO: Yoga need to handle the exception properly
    va_start(argList, fmt);
    vsnprintf(info, MAX_SIZE_PER_MSG, fmt, argList);
    va_end(argList);

    //TODO: calculate the timer in milliseconds

    snprintf(fullInfo, MAX_SIZE_PER_MSG, "%s %s:%d %s %s", verbos, fileName, lineNum, info, NEWLINE);
    //ACE_DEBUG((LM_DEBUG,  ACE_TEXT(fullInfo))); //testing purpose
  } catch(...) {
    snprintf(fullInfo, MAX_SIZE_PER_MSG, "Invalid log format '%s'\n", fmt);
    logLevel = EN_LOG_LEVEL_ERROR;
  }

  retVal = post(logLevel, fullInfo);
  return retVal;
}

bool XGLog::post(LogLevel logLevel, const char* fmt)
{
  xGateLogMsg *msg = new xGateLogMsg(logLevel, fmt);  
  ACE_Message_Block* pAmb = NULL;
  pAmb = static_cast<xGateLogMsg*> (msg);
  ACE_Time_Value tval ((time(NULL) ));
  tval += ACE_Time_Value(0,1);
  if(!XGLOG_TASK::instance()->putq(pAmb,&tval)) {
    printf("pushing log message to logger thread failed !!\n");
    return false;
  } 
  return true;
}

__ur_function_log__::__ur_function_log__(const char* ptfunction,const char* filename, int line, const char* ptMsg...)
{
   bool retVal = false;
   va_list argList;
   char info[MAX_SIZE_PER_MSG] = {0};
   memset(info,0,MAX_SIZE_PER_MSG);
   char fullInfo[MAX_SIZE_PER_MSG] = {0};
   memset(fullInfo,0,MAX_SIZE_PER_MSG);

   ptFunctionName = ptfunction;
   ptFileName = filename;
   ptLineNumber = line;

   LogLevel logLevel = EN_LOG_LEVEL_TRACE;

   try {
      if (ptMsg) {

	 va_start(argList, ptMsg);
	 vsnprintf(info, MAX_SIZE_PER_MSG, ptMsg, argList);
	 va_end(argList);

	 snprintf(fullInfo, MAX_SIZE_PER_MSG, "%s %s:%d Calling function %s! %s %s", TRACE_PREFIX, ptFileName,
                     ptLineNumber, ptFunctionName, info, NEWLINE);
      } else {
	 snprintf(fullInfo, MAX_SIZE_PER_MSG, "%s %s:%d Calling function %s! %s", TRACE_PREFIX, ptFileName, 
                     ptLineNumber, ptFunctionName, NEWLINE);
      }
   } catch(...) {
      snprintf(fullInfo, MAX_SIZE_PER_MSG, "Invalid log format\n");
      logLevel = EN_LOG_LEVEL_ERROR;
   }


#if 1
   xGateLogMsg *msg = new xGateLogMsg(logLevel, fullInfo);
   ACE_Message_Block* pAmb = NULL;

   pAmb = static_cast<xGateLogMsg*> (msg);
   ACE_Time_Value tval ((time(NULL) ));
   tval += ACE_Time_Value(0,1);

   if(!XGLOGINSTANCE->putq(pAmb,&tval)) {
      printf("pushing log message to logger thread failed !!\n");
   }
#endif
}

__ur_function_log__::~__ur_function_log__()
{
   char fullInfo[MAX_SIZE_PER_MSG] = {0};
   LogLevel logLevel = EN_LOG_LEVEL_TRACE;

   try {
      snprintf(fullInfo, MAX_SIZE_PER_MSG, "%s %s:%d Exiting function %s! %s", TRACE_PREFIX, ptFileName, 
               ptLineNumber, ptFunctionName, NEWLINE);
   } catch(...) {
      snprintf(fullInfo, MAX_SIZE_PER_MSG, "Invalid log format\n");
      logLevel = EN_LOG_LEVEL_ERROR;
   }

   xGateLogMsg *msg = new xGateLogMsg(logLevel, fullInfo);
   ACE_Message_Block* pAmb = NULL;

   pAmb = static_cast<xGateLogMsg*> (msg);
   ACE_Time_Value tval ((time(NULL) ));
   tval += ACE_Time_Value(0,1);

   if(!XGLOGINSTANCE->putq(pAmb,&tval)) {
      printf("pushing log message to logger thread failed !!\n");
   }

}


