#include <fstream>
#include <stdexcept>
#include "xGateUtil.h"
#include "logmsg.h"
#include "xglog.h"
#include "LogProfile.h"

#define HAVE_THREAD_FOR_LOGGING
XGLog::XGLog() :
  ACE_Task<ACE_MT_SYNCH>(),
  m_run(false), m_logStream(NULL), m_strDirName(""), m_activeLogDate(""),
  m_filePath(""),m_fileCount(0)
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

bool XGLog::init(std::string proc_name)
{
  m_run = true;
  m_proc_name = proc_name;
  printf ("Entered XGLog::init proc_name: \n");
  create_directory();
  create_file();
#ifdef HAVE_THREAD_FOR_LOGGING
  // Start the log thread.
  if (activate(THR_NEW_LWP, 1) == -1 )
  {
    return false;
  }
  else
  {
  }
#endif
  printf("XGLog::init() success !\n");
  return true;
}

bool XGLog::loadlogProfile(LogProfile &profile){
  m_logProfile = profile;
  return true;
}

bool XGLog::create_directory()
{
  char buffer [80]={0};
  char dirName [80]={0};
  timeval curTime;
  ::gettimeofday(&curTime, NULL);
  int milli = curTime.tv_usec / 1000;
  strftime(buffer, 80, "%d-%m-%y", localtime(&curTime.tv_sec));
  snprintf(dirName, 91, "VID_REC_%s", buffer);
  m_strDirName = string("/var/log/xgate/log") + string("/")  + dirName;
  ACE_OS::mkdir(m_strDirName.c_str(),0777);
  m_activeLogDate = buffer;
  return true;
}

bool XGLog::create_file()
{
        ACE_LOG_MSG->priority_mask(LM_INFO | LM_DEBUG | LM_ERROR | LM_WARNING | LM_CRITICAL | LM_EMERGENCY, ACE_Log_Msg::PROCESS);
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
        char file_name[100]={0};
        memset(file_name,0,sizeof(file_name));
        time_t timenow;
        strftime(buffer, 80, "%d-%m-%y_%H.%M.%S", localtime(&curTime.tv_sec));
        snprintf(file_name, sizeof(file_name) , "%s_%s_LOG-%03d.TXT", buffer,m_proc_name.c_str(),m_fileCount);
        /* clear the memory allocated for string before assigning */
        //if (m_filePath.size()){
        //   m_filePath.clear();
        //}
        m_filePath= m_strDirName + string("/")  + file_name ;

        list<string> :: iterator itr;
        m_logStream = new std::ofstream();
        m_logStream->open(m_filePath.c_str(), ios::out | ios::app);
        ACE_LOG_MSG->msg_ostream(m_logStream);
//        ACE_LOG_MSG->clr_flags(ACE_Log_Msg::STDERR | ACE_Log_Msg::LOGGER);//To disable Console Log
        ACE_LOG_MSG->set_flags(ACE_Log_Msg::OSTREAM);
        return true;
}




int XGLog::svc(void)
{
  ACE_Message_Block *pAmb = 0;

  while(m_run)
  {
    pAmb = NULL;
    int result = getq(pAmb, NULL);
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
  }

  return 0;
}

bool XGLog::handle_msg(ACE_Message_Block *pAmb)
{
  xGateLogMsg *pMsg = dynamic_cast<xGateLogMsg*>(pAmb);
  LogLevel logLevel = pMsg->getLogLevel(); 
  std::string &fmt = pMsg->getFmt();
  bool retVal;
  retVal = SendLogMsgToAce (logLevel, (char* )fmt.c_str());
  /* Delete the message after using */
  delete pMsg;
  return retVal;
}

int XGLog::LogDataNTime (char *log)
{
  int bytes_wrote = -1;
  char buffer [80];
  memset(buffer,0,sizeof(buffer));
  timeval curTime;
  int bytes_to_write = 0;
  ::gettimeofday(&curTime, NULL);
  int milli = curTime.tv_usec / 1000;
  strftime(buffer, 80, "%y-%m-%d %H:%M:%S", localtime(&curTime.tv_sec));
  bytes_wrote = snprintf(log, 84, "%s.%03d ", buffer, milli);
  return bytes_wrote;
}

bool XGLog::SendLogMsgToAce (LogLevel  level, char *logMsg)
{
   char buffer[80] = {0};
   timeval curTime;
   ::gettimeofday(&curTime, NULL);
   strftime(buffer, 80, "%d-%m-%y", localtime(&curTime.tv_sec));
   if (strcmp(m_activeLogDate.c_str(), buffer) != 0){
       if (m_logStream && m_logStream->is_open()){
         printf ("closing the m_logStream as date changed and created new directory\n");
         m_logStream->close();
       }
       create_directory();
       create_file();
   }

   ifstream in_file (m_filePath,ios::binary);
   in_file.seekg(0, ios::end);
   int file_size = in_file.tellg();
   m_logProfile.m_fileSize = 20000000 ; //fize size KB

   if (file_size>=m_logProfile.m_fileSize )
   {
       if (m_logStream && m_logStream->is_open()){
         printf ("closing the m_logStream as file_size: %d > m_logProfile.m_fileSize \n",file_size);
         m_logStream->close();
      }
      // rotateLog();
       create_file();
   }

  // On the server processor, we handle  
  switch(level)
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
    default:
      {
        XGLOG_ERROR("Unknown log message type %d might not been implemented yet !", \
            level);
        break;
      }
  }
  return true;
}

bool XGLog::write(LogLevel logLevel, const char* verbos, const char* fileName, \
    unsigned int lineNum, const char* fmt...)
{
  bool retVal = false;
  va_list argList;
  char info[MAX_SIZE_PER_MSG + 1] = {'\0'};
  char fullInfo[MAX_SIZE_PER_MSG + 1] = {'\0'};
  int bytes_wrote = 0;
  try { //TODO: Yoga need to handle the exception properly
    va_start(argList, fmt);

    vsnprintf(info, sizeof(info) , fmt, argList);
    va_end(argList);

    //TODO: calculate the timer in milliseconds
    bytes_wrote = LogDataNTime (fullInfo);
    if (bytes_wrote < 0)
      bytes_wrote = 0;
    snprintf(&fullInfo[bytes_wrote], sizeof(fullInfo) - bytes_wrote, "%s %s:%d %s %s",
              verbos, fileName, lineNum, info, NEWLINE);
  } catch(...) {
    snprintf(fullInfo, sizeof(fullInfo), "Invalid log format '%s'\n", fmt);
    logLevel = EN_LOG_LEVEL_ERROR;
  }
#ifdef HAVE_THREAD_FOR_LOGGING
  retVal = post(logLevel, fullInfo);
#else
  retVal = SendLogMsgToAce (logLevel, fullInfo);
#endif
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

