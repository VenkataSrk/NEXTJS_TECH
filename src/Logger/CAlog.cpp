#include <fstream>
#include <stdexcept>
#include "logmsg.h"
#include "CAlog.h"
#include "LogProfile.h"
#include "CAUtilityInternal.h"
//#include "Server.h"


#define MBSIZE ((8*1000)*1000)
CAlog::CAlog() :
  ACE_Task<ACE_MT_SYNCH>(),
  m_run(false), m_logStream(NULL), m_filePath(""),m_fileCount(0), m_maxFileSize(0), m_enabledLogLevel(""),
  m_lifeTime(0)
{
}

CAlog::~CAlog(void)
{
  if (m_logStream)
  {
    delete m_logStream;
    m_logStream = NULL;
  }

}

bool CAlog::stop(void)
{

  while(m_queueSize > 0){
    usleep(1);
  }
  
  m_run = false;
  
  msg_queue()->deactivate();
  wait();

  return true;
}

bool CAlog::init(std::string level, std::string path, unsigned short size, unsigned short lifeTime)
{
  m_run = true;
  m_enabledLogLevel = level;
  m_path = path;
  m_maxFileSize = size;
  m_lifeTime = lifeTime;
  create_file();

  // Start the log thread.
  if (activate(THR_NEW_LWP, 1) == -1 )
  {
    return false;
  }
  else
  {
  }
  printf("CAlog::init() !\n");
  return true;
}

bool CAlog::loadlogProfile(LogProfile &profile){
  m_logProfile = profile;
  return true;
}

bool CAlog::create_file()
{
        ACE_LOG_MSG->priority_mask(LM_INFO | LM_DEBUG | LM_ERROR | LM_WARNING | LM_CRITICAL | LM_EMERGENCY | LM_ALERT, ACE_Log_Msg::PROCESS);
        if(m_fileCount>=(int)m_logProfile.m_fileCount)
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
       // time_t timenow;  // temperorly comment
        strftime(buffer, 80, "%d_%m_%y_%H%M%S", localtime(&curTime.tv_sec));
        snprintf(file_name, 85, "%s_CA_LOG-%03d.TXT", buffer,m_fileCount);
        m_filePath=m_path  + string("/")  + file_name ;

        list<string> :: iterator itr;
        m_logStream = new std::ofstream();
        m_logStream->open(m_filePath.c_str(), ios::out | ios::app);
        ACE_LOG_MSG->msg_ostream(m_logStream);
        ACE_LOG_MSG->clr_flags(ACE_Log_Msg::STDERR | ACE_Log_Msg::LOGGER);//To disable Console Log 
        ACE_LOG_MSG->set_flags(ACE_Log_Msg::OSTREAM);
        return true;
}

int CAlog::svc(void)
{
  ACE_Message_Block *pAmb = 0;

  while(m_run)
  {
    pAmb = NULL;
    ACE_Time_Value tv ((time(NULL) + 20));
    m_queueSize = getq(pAmb, &tv);
    
    if ( ( m_queueSize == -1 ) && ( errno == EWOULDBLOCK ) )
    {
      continue;
    }
    if ((m_queueSize != -1))
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

bool CAlog::handle_msg(ACE_Message_Block *pAmb)
{
  xGateLogMsg *pMsg = dynamic_cast<xGateLogMsg*>(pAmb);
  LogLevel logLevel = pMsg->getLogLevel();
  std::string &fmt = pMsg->getFmt();

  char current_time[84] = "";
  memset(current_time,0,sizeof(current_time));
  char buffer [80];
  memset(buffer,0,sizeof(buffer));
  timeval curTime;
  ::gettimeofday(&curTime, NULL);
  int milli = curTime.tv_usec / 1000;
  strftime(buffer, 80, "%y-%m-%d %H:%M:%S", localtime(&curTime.tv_sec));
  snprintf(current_time, 84, "%s.%03d", buffer, milli);

  char logMsg[MAX_SIZE_PER_MSG] = {0};
  memset(logMsg,0,MAX_SIZE_PER_MSG);
  snprintf(logMsg, MAX_SIZE_PER_MSG, "%s %s", current_time, fmt.c_str());

//Log Rotation Logic based on file size 
  ifstream in_file (m_filePath,ios::binary);
  in_file.seekg(0, ios::end);
  int file_size = in_file.tellg();
  //m_logProfile.m_fileSize = 20000000 ; //fize size KB 
  m_logProfile.m_fileSize = m_maxFileSize*MBSIZE ;// file size in MB
  if (file_size>=(int)m_logProfile.m_fileSize )
  {
    m_logStream->close();
    create_file();
    executeFileCleanup();
  }

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
    case EN_LOG_LEVEL_EXCEP:
      {
        ACE_DEBUG((LM_ALERT, ACE_TEXT(logMsg)));
        break;
      }
    default:
      {
        CAlog_ERROR("Unknown log message type %d might not been implemented yet !", \
            logLevel);
        break;
      }
  }
  return true;
}

#if 0
bool CAlog::rotateLog()
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

bool CAlog::write(LogLevel logLevel, const char* verbos, const char* fileName, \
    unsigned int lineNum, const char* fmt...)
{
  bool retVal = false;

  std::size_t found = m_enabledLogLevel.find(to_string(logLevel));
  if(found==std::string::npos)
  {
    return retVal;
  }

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
    

   char fileInfo[30] = "\0";
   snprintf(fileInfo,30,"%s:%d", fileName, lineNum);
   snprintf(fullInfo, MAX_SIZE_PER_MSG, "%6s [%-30s] %-s %s", verbos, fileInfo, info, NEWLINE);
   // snprintf(fullInfo, MAX_SIZE_PER_MSG, "%s %s:%d %s %s", verbos, fileName, lineNum, info, NEWLINE);
    //snprintf(fullInfo, MAX_SIZE_PER_MSG, "%s %s  %s:%d %s", verbos, info, fileName, lineNum, NEWLINE);
    //ACE_DEBUG((LM_DEBUG,  ACE_TEXT(fullInfo))); //testing purpose
  } catch(...) {
    snprintf(fullInfo, MAX_SIZE_PER_MSG, "Invalid log format '%s'\n", fmt);
    logLevel = EN_LOG_LEVEL_ERROR;
  }

  retVal = post(logLevel, fullInfo);
  return retVal;
}

bool CAlog::post(LogLevel logLevel, const char* fmt)
{
  xGateLogMsg *msg = new xGateLogMsg(logLevel, fmt, "");
  ACE_Message_Block* pAmb = NULL;
  pAmb = static_cast<xGateLogMsg*> (msg);
  ACE_Time_Value tval ((time(NULL) ));
  tval += ACE_Time_Value(0,1);
  if(!CAlog_TASK::instance()->putq(pAmb,&tval)) {
    printf("pushing log message to logger thread failed !!\n");
    return false;
  } 
  return true;
}

//SMT delete files based on days
bool CAlog::executeFileCleanup()
{
  try
  {
    struct dirent *entry;
    DIR *dir = opendir(m_path.c_str());
    if (dir == NULL) 
    {
      return false;
    }
    std::string path("");
    path.assign(m_path.c_str());
    path.append("/");
    while ((entry = readdir(dir)) != NULL) 
    {
      if(!strstr(entry->d_name, "CA_LOG"))
      {
        continue;
      }
      struct stat t_stat;
      std::string filePath("");
      filePath.assign(path);
      filePath.append(entry->d_name);
      stat(filePath.c_str(), &t_stat);
      struct tm fileTm = *localtime(&t_stat.st_mtime);
      time_t now;
      time(&now);
      struct tm currentTm = *localtime(&now);
      //cout << asctime(&fileTm) << endl << asctime(&currentTm) << endl;
      double seconds = difftime(now, mktime(&fileTm));
      if(seconds > (m_lifeTime*86400))
      {
        remove(filePath.c_str());
      }
    }
    closedir(dir);
  }
  catch(...)
  {
    return false;
  }
  return true;
}
