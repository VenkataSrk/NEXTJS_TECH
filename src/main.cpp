
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <csignal>
#include <errno.h>
#include <iostream>
#include <xgconfig.h>

#ifdef XGATE_HAS_MGC_MG_COMMUNICATION
#include "xGateUtil.h"
#include "xGateMgProcessor.h"
#include "xGateTCPReceiver.h"
#include "xGateMGDispatcher.h"
#include "xGateMGCConnector.h"
#include "xGateUtil.h"
#endif

#ifdef XGATE_HAS_GSTREAMER
#include "xGateHmpProcessor.h"
#include "xGateHmpGstManager.h"
#endif

#ifdef XGATE_HAS_SELF_AUDIT
#include "xGateAuditService.h"
#include "xGateAuditScheduler.h"
#include "xGateAuditUtil.h"
#endif
#include "Base64.h"
#if 1
#include "xGateServerConfig.h"
//#include "kernel.h"
#include "xglog.h"
#endif 

#define __linux__

#define E_LINE_DROP 1

//rapidjson includes
//#include "rapidjson/reader.h"


using namespace std;
// Version information
char buffer[200];
//map <CTask *, ip_info *> ip_list;

//class Kernel;

const char*  ENGINE_VERSION =	    "2";	    // major number
unsigned short BUILD_MAJOR_NUM =     68;	// major numbering
unsigned short BUILD_MINOR_NUM =     82;	// minor version
#define ERROR_ALREADY_EXISTS 0
#define MAX_PATH 4096
std::vector<std::string> vCcaasPubChannel;

//#include "compileinfo.h"



class VersionBuilder {
  public:
    char version_[128];
    VersionBuilder() { 
      /*	
          sprintf(version_, "xGate version %s (build %u.%u at %s %s at host %s)", 
          ENGINE_VERSION, BUILD_MAJOR_NUM, BUILD_MINOR_NUM,
          COMPILE_DATE, COMPILE_TIME, COMPILE_HOST);*/
    }
};

static const VersionBuilder _ver;
const char * Xgate_Version = _ver.version_;

void SpawnXgateRestarter(bool on_off)
{
  //static HANDLE hRestarterProcess;
  pthread_mutex_t hRestarterProcess;

  if (on_off == false) {
    if (kill(getpid(), SIGUSR1)) {

      pthread_mutex_destroy(&hRestarterProcess);
    } else
      sprintf(buffer,"Unable to terminate restarter process\n");
    return;
  }

  if ( pthread_mutex_lock(&hRestarterProcess)) {
    pthread_mutex_destroy(&hRestarterProcess);
  }


  char xgate_cmd[40];
  //sprintf(xgate_cmd, "xgate.exe %u", GetCurrentProcessId());
  sprintf(xgate_cmd, "xgate %u", getpid());

}


//-----------------------------------------------------------------------------
// memory to preallocate (and freed if exceptions occured)
//#define NUM_GUARD_MEM	1000000


//-----------------------------------------------------------------------------
// CTEngine return values
//
#define EXIT_NORMAL	        0	// shutdown properly
#define EXIT_INIT_ERROR	    1	// error in initialization
#define EXIT_NO_MEM	        2	// new_bad_alloc exception
#define EXIT_ERROR	        3	// unexpected run-time error (unhandled exceptions, etc)
#define EXIT_RESTART_REQ    4



//-----------------------------------------------------------------------------
// Run the engine in the console
//
int SL_ConsoleMain(unsigned major, unsigned minor)
{
 srand(time(NULL));



#ifdef XGATE_HAS_MGC_MG_COMMUNICATION
  //1. Dispatcher Thread Initialization
  xGateMGDispatcher *pMgDispatcher = new xGateMGDispatcher("MgDispatcher");
  if(pMgDispatcher->init() != EN_XGATE_STATUS_SUCCESS)
  {
    cout << "xGateMGDispatcher initialized from main thread" << endl;
    return EXIT_INIT_ERROR;
  }
  xGateUtil::setMgDispatcher(pMgDispatcher);

  //2. MgProcessor Thread Initialization
  xGateMgProcessor *pMgProcessor = new xGateMgProcessor("SrvrProcessor");
  int numOfThread = 1;
  if(pMgProcessor->init(numOfThread) != EN_XGATE_STATUS_SUCCESS)
  {
    cout << "xGateMgProcessor initialized from main thread" << endl;
    return EXIT_INIT_ERROR;
  }
  xGateUtil::setMgProcessor(pMgProcessor);


  //3. MgcConnector Thread Initialization
  xGateMGCConnector *pMgcConnector = new xGateMGCConnector(pMgProcessor);
  //Gopal Changes 
  if(pMgcConnector)
  {
    vector<xGateListenAddress>listenAddressList;
    unsigned int index; 
    for(index=0;index<xGateUtil::m_MgcIpAddressList.size();index++)
    {
      xGateTCPConnectionStr connectionStr;
      xGateListenAddress listenAddress;
      listenAddress.port = (unsigned short) 32779;
      listenAddress.ipAddress = xGateUtil::m_MgcIpAddressList[index];
      listenAddressList.push_back(listenAddress);
      // use in6addr_any
      //if (ACE::ipv6_enabled ()) {
      //   std::string srvListenAddress = xGateUtil::getLocalIpAddr(AF_INET6);
      //   //srvListenAddress = std::string("::"); //("2001:1b88:50::d");
      //   //listenAddress.port = (unsigned short) srvPort;
      //   listenAddress.ipAddress = srvListenAddress;
      //   listenAddressList.push_back(listenAddress);
      //} else {
      //    printf("xGateUtil::setSrvrProcessor, ACE::ipv6_enable() false\n");
      //}
    }

    if(pMgcConnector->init(listenAddressList) != EN_XGATE_STATUS_SUCCESS)
    {
      cout << "xGateMGCConnector initialized from main thread" << endl;
      delete pMgcConnector;
      return EXIT_INIT_ERROR;
    }
  }
  //up to here
  /*if(pMgcConnector->init() != EN_XGATE_STATUS_SUCCESS)
    {
    cout << "xGateMGCConnector initialized from main thread" << endl;
    return EXIT_INIT_ERROR;
    }*/

  //xGateTCPConnectionStr connectionStr;;
  //connectionStr.conType = EN_XGATE_MG_CONNECTION;
  //connectionStr.peerIPAddress = "10.22.3.92";

  //if (EN_XGATE_STATUS_SUCCESS != pMgcConnector->connect_to_peer(connectionStr)) {
  if (EN_XGATE_STATUS_SUCCESS != pMgcConnector->connect_to_peer()) {
    cout << "main MgConncector connect_to_peer API failed." << endl;
    return EXIT_INIT_ERROR;
  }
  xGateUtil::setMgcConnector(pMgcConnector);

#endif

#ifdef XGATE_HAS_GSTREAMER
  //4. HmpProcessor Thread Initialization 
  xGateHmpProcessor *pHmpProcessor = new xGateHmpProcessor("HmpProcessor");
  int HmpProcessorCount = 1;
  if(pHmpProcessor->init(HmpProcessorCount) != EN_XGATE_STATUS_SUCCESS)
  {
    cout << "xGateMgProcessor initialized from main thread" << endl;
    return EXIT_INIT_ERROR;
  }

  if(pHmpProcessor->initMRCPBridge() != EN_XGATE_STATUS_SUCCESS)
  {
    cout << "xGateMgProcessor initialized from main thread" << endl;
    return EXIT_INIT_ERROR;
  }
  xGateUtil::setHmpProcessor(pHmpProcessor);

  xGateHmpGstManager *pHmpGstManager = new xGateHmpGstManager("HmpGstManager");
  if(pHmpGstManager->init() != EN_XGATE_STATUS_SUCCESS)
  {
    cout << "xGateMgProcessor initialized from main thread" << endl;
    return EXIT_INIT_ERROR;
  }

  xGateUtil::setHmpGstManager(pHmpGstManager);
#endif

/*
#ifdef XGATE_HAS_SELF_AUDIT
  //5. AuditProcessor Thread Initialization
  if(!AUDITSERVICE->init()) {
    cout << "Audit service initialization failed from main thread !!" << endl;
    return EXIT_INIT_ERROR;
  }
#endif
*/

  // start kernel main loop
  //Kernel::Instance()->Start();

  // shutdown
  //** already done in MainLoopProc ** Kernel::Instance()->Shutdown(KShutdown::DEINIT);
  //Kernel::Destroy();

  //printf("End of SL_ConsoleMain function\n");
         do
        {
                usleep(600000);
        }while(true);

  return EXIT_NORMAL;
}

//-----------------------------------------------------------------------------
// Exception class and handler if new failed to alloc memory
//
class new_bad_alloc : public std::exception {
  public:
    size_t  size;

    new_bad_alloc(size_t n) throw() : size(n) {};
    new_bad_alloc(const new_bad_alloc &rhs ) throw()
      : size(rhs.size) {};
    bool operator = (const new_bad_alloc &rhs) throw()
    {
      size = rhs.size;
      exception::operator =(rhs);
    };
};

int new_failed_handler(size_t size)
{
  throw new_bad_alloc(size);
};


void printlabel(void){

  char welcomestring[] = "\
                          **********************************************************************************************************\n\
                          **********************************************************************************************************\n\	
                          \n\
                          \n\
                          8888888 888888   88888     8888  8888    8888                                   \n\  
                          88       88   88 88         88  88  88  88   88                                  \n\
                          88888	  888888  88     888 88  88  88  88                                       \n\
                          88 88   88 88         88  88  88  88  8888                                 \n\
                          88 88   88 88         88      88  88    88                                 \n\
                          8888888  888888   88888     88      88   888888                                  \n\
                          \n\
                          VECTONE MOBILE   \n\
                          **********************************************************************************************************\n\
                          **********************************************************************************************************\n";

  printf("%s",welcomestring);

}

void ConfigvariableInit()
{
 GlobalClass *ConfigLoad = GlobalClass::instance();
 ConfigLoad->loadConfig();

//Insert PBX IP into List for TCP connection
 for ( int i = 0 ;i<ConfigLoad->m_PBXIpAddressList.size();i++){
     //cout << ConfigLoad->m_PBXIpAddressList[i]<<endl;
     xGateUtil::m_MgcIpAddressList.push_back(ConfigLoad->m_PBXIpAddressList[i]);
 }


//Setting Config variables 
   xGateHmpGstUtil::setDtmfLowDurationLimit(ConfigLoad->m_DTMF_LOW_DURATION);
   xGateHmpGstUtil::setenabledecresampler(ConfigLoad->m_DEC_RESAMPLER);
   xGateHmpGstUtil::setenabledecconverter(ConfigLoad->m_DEC_CONVERTER);
   xGateHmpGstUtil::setenableg729decresampler(ConfigLoad->m_G729_DEC_AUDIORESAMPLER);
   xGateHmpGstUtil::setenableg729decconverter(ConfigLoad->m_G729_DEC_AUDIOCONVERTER);
   xGateHmpGstUtil::setenableg729decrate(ConfigLoad->m_G729_DEC_AUDIORATE);
   AUDITUTIL->setAuditStartTime(ConfigLoad->m_AUDIT_START_TIME);
   AUDITUTIL->setAuditAttemptCount(ConfigLoad->m_AUDIT_MAX_RETRY);
}  

namespace
{
  volatile sig_atomic_t gSignalStatus;
}

void signal_handler(int signum)
{
  gSignalStatus = signum;
  std::cout<<"Inside Signal_Handler SignalValue: "<<gSignalStatus<<std::endl;
  XGLOG_INFO ("Inside Signal_Handler Signal: %d", signum);

  try
  {
    RedisConnInfo* redisConnInfo = MRCPCONFIGSERVICE->m_redisProfile.m_mapPubRedisConnInfo[EN_REDIS_TYPE::EN_REDIS_TYPE_CCAAS_ACD];
    ConnectionOptions m_pConnectionOptions;
    if(!redisConnInfo)
    {
      XGLOG_ERROR("signal handler redisConnInfo is null");
      return NULL;
    }
    m_pConnectionOptions.host = redisConnInfo->m_strServerHost.c_str();
    m_pConnectionOptions.port = (redisConnInfo->m_iServerPort > 0) ? redisConnInfo->m_iServerPort : 6379;
    if(!redisConnInfo->m_strPassword.empty())
    {
      m_pConnectionOptions.password = redisConnInfo->m_strPassword.c_str();
    }
    m_pConnectionOptions.socket_timeout = (redisConnInfo->m_iSocketTimeout > 0) ? std::chrono::milliseconds(redisConnInfo->m_iSocketTimeout) : std::chrono::milliseconds(0);
    int cluster = redisConnInfo->m_iCluster;
    if(cluster==1)
    {
      RedisCluster pubRedis(m_pConnectionOptions);
      pubRedis.publish(redisConnInfo->m_strChannel.c_str(), "STT_TTS_MG_CRASHED");
    }
    else
    {
      Redis pubRedis(m_pConnectionOptions);
      pubRedis.publish(redisConnInfo->m_strChannel.c_str(), "STT_TTS_MG_CRASHED");
    }
  }
  catch(...)
  {
    std::cout<<"signal_handler Redis DB not connected"<<std::endl;
  }

  signal(signum, SIG_DFL);
  kill(getpid(), signum); 
}

int main(int argc, char* argv[])
{

ConfigvariableInit();

#if 1//Ace Logger Module initialization
  bool retVal = XGLOG_INIT();
  if(!retVal) {
    printf("ERROR: XGLogger module initialization failed !!\n");
  } else {
    XGLOG_INFO("XGLogger successfully initialized !");
    XGLOG_TRACE("XGLogger successfully initialized !");
    XGLOG_DEBUG("XGLogger successfully initialized !");
    XGLOG_WARN("XGLogger successfully initialized !");
    XGLOG_ERROR("XGLogger successfully initialized !");
    XGLOG_CRIT("XGLogger successfully initialized !");
    XGLOG_FATAL("XGLogger successfully initialized !");
  }
#endif

  int retcode = 1;
  printlabel();
#if 1
  std::signal(SIGINT, signal_handler);
  std::signal(SIGABRT, signal_handler);
  std::signal(SIGTERM, signal_handler);
  std::signal(SIGSEGV, signal_handler);
  std::signal(SIGILL, signal_handler);
  std::signal(SIGKILL, signal_handler);
#endif
#if 1
  try {
    retcode = SL_ConsoleMain(BUILD_MAJOR_NUM,BUILD_MINOR_NUM);
  }catch (...) {
    printf("\n Invalid Task detected \n");
  }
  return retcode;
#endif 

}

