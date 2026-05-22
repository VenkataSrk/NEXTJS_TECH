#include <string>
#include "xglog.h"
#include "XGConfig.h"
#include "XGController.h"
#include "XG_SFUController.h"
#include "MBCController.h"

using namespace std;
void INThandler(int sig);
void INThandler(int sig)
{
      printf("Caught SigSegV \n");
}

int ACE_TMAIN (int argc, ACE_TCHAR **argv)
{
  struct sched_param param;
// signal(SIGSEGV, INThandler);
 if(argc < 2){
   cout << "Insufficient number of arguments" << endl;
   exit(0);
 }

#if 0
 if(strcmp(argv[1],"sfu") || strcmp(argv[1],"mbc")){
   cout << "Invalid module specified  " << " Please specify either mbc or sfu " << endl;
   exit(0);
 }	 
#endif
 if(strcmp(argv[1],"sfu") == 0 && argc < 3){
   cout << "Insufficient number of arguments(%d) for loading sfu " << argc << endl;
   exit(0);   
 }


  string log_name;
  if(strcmp(argv[1],"mbc") == 0){
    log_name = "MBC";
  }else if(strcmp(argv[1],"sfu") == 0) {
    log_name = "SFU_";
    log_name += argv[2];
    param.sched_priority = 5;
    sched_setscheduler (getpid(), SCHED_RR, &param);
  }else{
    log_name = "MBC";
  }

  //initialize configuration module
  bool retVal = CONFIGSERVICE->init();
  if(!retVal) {
    cout << "XG configuration module initialization failed !" << endl;
  } else {
    XGLOG_INFO("XG configuration module successfully initialized !");
  }

  retVal = XGLOG_INIT->init(log_name.c_str());
  if(!retVal) {
    printf("ERROR: XGLogger module initialization failed !!\n");
  } else {
    XGLOG_INFO("XGLogger successfully initialized !");
  }

  //initialize vms controller module 
 if(strcmp(argv[1],"sfu") == 0)
 {	 
   int sfu_id = atoi(argv[2]);	 
   bool retVal = XGSFUCONTROLLER->init(sfu_id);
   if(!retVal) {
    XGLOG_ERROR("XG SFU controller module initialization failed !");
  } else {
    XGLOG_INFO("XG SFU controller module successfully initialized !");
  }
 }
 else
 {
#if 1 
  bool retVal = MBCCONTROLLER->init();
  if(!retVal){
    XGLOG_ERROR("XG MBC controller module initialization failed !");    
  }
  else
  {
   XGLOG_INFO("XG MBC controller module initialization successful !");	  
  }
#endif  
 } 	 
  unsigned int count = 0;
  do {
    //XGLOG_WARN("now %s logger count is %d !", "error", ++count);
    usleep(500000);
  }while(1);

  return 0;
}
