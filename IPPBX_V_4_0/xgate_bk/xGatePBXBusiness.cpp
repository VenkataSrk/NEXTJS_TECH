#include "xGateSipSignalProcessor.h"
#include "xGatePBXBusiness.h"
#include "xGateDbmMsg.h"
#include "xGateDbmDispatcher.h"
//#include "systemapi.h"
//#include "kernel.h"
//#include "RegisDBConnector.h"

#include "xGateSipController.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <ctime>
//#include <pthread.h>

//#include "xGateDBManager.h"

#define THISMODULE "PBXBusiness"

#define UNIFIEDRING_DB_SERVER_IP_ADDRESS	"10.22.2.86"
#define UNIFIEDRING_DB_UID			"smepbx"
#define UNIFIEDRING_DB_PASSWORD			"smeswitch"
#define UNIFIEDRING_DB_DATABASE			"unifiedring"

#define SELECTOR_DB_SERVER_IP_ADDRESS	"10.22.2.182"
#define SELECTOR_DB_UID			"agent"
#define SELECTOR_DB_PASSWORD		"agentsql"
#define SELECTOR_DB_DATABASE		"selector"

#define REGISTRAR_DB_SERVER_IP_ADDRESS	"10.22.2.85"
#define REGISTRAR_DB_UID		"mgc"
#define REGISTRAR_DB_PASSWORD		"mgcsql"
#define REGISTRAR_DB_DATABASE		"xgregistar"

#define	CALL_FORWARDED_ANNOUNCEMENT_FILE	"call_has_been_forwarded.wav"
#define	RBT_DEFAULT_FILE			"UK_ringback_tone-0001.wav"
#define	RBT_FILE_ONE_RING			"UK_ringback_tone_one_ring.wav"
#define	SILENCE_500MS_FILE			"500ms_silence.wav"
#define	CONNECT_TO_OPERATOR_FILE		"please_wait_connect_operator.wav"
#define	NO_INPUT_GOODBYE_FILE			"no_input_goodbye.wav"
#define CONNECTING_FILE_DEFAULT                 "SB_hold_line_connecting.wav"
#define CALLHOLD_FILE_DEFAULT                     "hold_stay_on_line.wav"

#define CONF_USER_NO_INPUT_FILE "/root/wav_files/sounds/vectone/CR_no_input.wav"

#define PBXSHARE_PATH		"/pbxshare/"
#define UG_PATH			"/UG/"
#define CH_PATH			"/CH/"

#define	DEVICE_TYPE_URDESK		"URDESK:"
#define	DEVICE_TYPE_URAPP_ANDROID	"URAPP:AND"
#define	DEVICE_TYPE_URAPP_IOS		"URAPP:IOS"
#define	DEVICE_TYPE_URWEB		"URWEB:WEB"

#define	CONTACT_ADDRESS_DEFAULT		"10.22.5.39:5060"

#define 	DEFAULT_SWB_SLEEPTIMEOUT_MAINLOOP 	40 // seconds
#define 	SWB_IVR_WELCOME_VECTONE			"SB_welcome_vectone.wav"
#define 	MAINMENU_IVR_REPEAT_COUNTER	 	1

void swb_TimerWait(int timeInMs)
{
  CLog::Detail(THISMODULE, "Inside xGatePBXBusiness::swb_TimerWait");

  pthread_mutex_t fakeMutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_cond_t fakeCond = PTHREAD_COND_INITIALIZER;

  struct timespec timeToWait;
  struct timeval now;
  int rt, counter = 3;

  gettimeofday(&now,NULL);

  timeToWait.tv_sec = now.tv_sec+ timeInMs/1000;
  //timeToWait.tv_nsec = (now.tv_usec+1000UL*timeInMs)*1000UL;
  unsigned int extra_seconds = 8;
  pthread_mutex_lock(&fakeMutex);
  do
  {
    rt = pthread_cond_timedwait(&fakeCond, &fakeMutex, &timeToWait);
    CLog::Detail(THISMODULE, "xGatePBXBusiness::swb_TimerWait : waiting for %d seconds...", extra_seconds);
    sleep(extra_seconds);
    counter--;
    //sleep(1);
  } while ((rt == 0) && (counter >= 0));
  pthread_mutex_unlock(&fakeMutex);
}
//void xGatePBXBusiness::swb_timerThread(callFeatureParam& cp, CallParameter& cp, xGateDBObj* dbObj)

void AppNotification_TimerWait(int timeInMs)
{
  CLog::Detail(THISMODULE, "Inside xGatePBXBusiness::AppNotification_TimerWait");

  pthread_mutex_t fakeMutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_cond_t fakeCond = PTHREAD_COND_INITIALIZER;

  struct timespec timeToWait;
  struct timeval now;
  int rt, counter = 3;

  gettimeofday(&now,NULL);

  timeToWait.tv_sec = now.tv_sec+ timeInMs/1000;
  //timeToWait.tv_nsec = (now.tv_usec+1000UL*timeInMs)*1000UL;
  unsigned int extra_seconds = 2;
  pthread_mutex_lock(&fakeMutex);
  do
  {
    rt = pthread_cond_timedwait(&fakeCond, &fakeMutex, &timeToWait);
    CLog::Detail(THISMODULE, "xGatePBXBusiness::AppNotification_TimerWait : waiting for %d seconds...", extra_seconds);
    sleep(extra_seconds);
    counter--;
    //sleep(1);
  } while ((rt == 0) && (counter >= 0));
  pthread_mutex_unlock(&fakeMutex);
}
void callHandling_TimerWait(int timeInMs)
{
  CLog::Detail(THISMODULE, "Inside xGatePBXBusiness::callHandling_TimerWait");

  pthread_mutex_t fakeMutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_cond_t fakeCond = PTHREAD_COND_INITIALIZER;

  struct timespec timeToWait;
  struct timeval now;
  int rt, counter = 3;

  gettimeofday(&now,NULL);

  timeToWait.tv_sec = now.tv_sec+ timeInMs/1000;
  //timeToWait.tv_nsec = (now.tv_usec+1000UL*timeInMs)*1000UL;
  unsigned int extra_seconds = 2;
  pthread_mutex_lock(&fakeMutex);
  do
  {
    rt = pthread_cond_timedwait(&fakeCond, &fakeMutex, &timeToWait);
    CLog::Detail(THISMODULE, "xGatePBXBusiness::callHandling_TimerWait : waiting for %d seconds...", extra_seconds);
    //std::this_thread::sleep_for(extra_seconds);
    sleep(extra_seconds);
    counter--;
    //sleep(1);
  } while ((rt == 0) && (counter >= 0));
  pthread_mutex_unlock(&fakeMutex);
}
void* swb_timerThread(void *arg)
{
  CLog::Detail(THISMODULE, "Inside xGatePBXBusiness::swb_timerThread");
  swb_TimerWait(12000); // 10 seconds
  CLog::Detail(THISMODULE, "xGatePBXBusiness::swb_timerThread : No DTMF-digits received after MAinMenu-IVR played ; connect to operator-extension...");

  xGateDBObj *dbObj = (xGateDBObj*) arg;
  if (!dbObj->cp.gm_userDiallDTMFdigits)
  {
    xGateCallObj *obj = new xGateCallObj();
    //obj->call_id = dbObj->cp.call_id;
    //obj->call_id.assign(dbObj->cp.call_id.c_str());
    obj->callid.assign(dbObj->cp.call_id);
    obj->cp = dbObj->cp;
    obj->uid = dbObj->cp.uid;
    obj->cp.m_mappedUserList = dbObj->cp.m_mappedUserList;
    obj->sipeventtype = EN_SWBMAINMENUIVR_TIMEOUT_RES;
    WriteToReceiver(obj);
    CLog::Detail(THISMODULE, "xGatePBXBusiness::swb_timerThread : No DTMF-digits received after MAinMenu-IVR played ; connect to operator-extension...");
  }
  else
    CLog::Detail(THISMODULE, "xGatePBXBusiness::swb_timerThread : DTMF-digits already dialled");
}

void* conf_timerThread(void *arg)
{
  xGateDBObj *dbObj = (xGateDBObj*) arg;
  CLog::Detail(THISMODULE, "Inside xGatePBXBusiness::conf_timerThread ; DigitTimeOut = %d seconds", dbObj->cp.confinfo.DigitTimeOut);

  unsigned int seconds = 2;
  swb_TimerWait(dbObj->cp.confinfo.DigitTimeOut + (seconds * 1000)); // 8 seconds are extra added in swb-thread; to compensate it; decease by 8
  CLog::Detail(THISMODULE, "xGatePBXBusiness::conf_timerThread : No DTMF-digits received after Welcome-IVR played ...");

  if (!dbObj->cp.gm_userDiallDTMFdigits)
  {
    xGateCallObj *obj = new xGateCallObj();
    obj->callid.assign(dbObj->cp.call_id);
    obj->uid = dbObj->cp.uid;
    obj->cp = dbObj->cp;
    obj->cp.m_mappedUserList = dbObj->cp.m_mappedUserList;
    obj->cp.confinfo = dbObj->cp.confinfo;
    obj->sipeventtype = EN_CONFERENCE_DTMFDIGIT_TIMEOUT_RES;
    WriteToReceiver(obj);
    CLog::Detail(THISMODULE, "xGatePBXBusiness::conf_timerThread : No DTMF-digits received after Welcome-IVR played ...");
  }
  else
    CLog::Detail(THISMODULE, "xGatePBXBusiness::conf_timerThread : DTMF-digits already dialled");
}

void* pushNotofication_timerThread(void *arg)
{
  xGateDBObj *dbObj = (xGateDBObj*) arg;
  CLog::Detail(THISMODULE, "Inside xGatePBXBusiness::pushNotofication_timerThread ");

  //unsigned int seconds = 0;
  AppNotification_TimerWait(5 * 1000); // 8 second are extra added in swb-thread; to compensate it; decease by 8
  CLog::Detail(THISMODULE, "xGatePBXBusiness::pushNotofication_timerThread : After timeout ...");

  xGateCallObj *obj = new xGateCallObj();
  obj->callid.assign(dbObj->cp.call_id);
  obj->uid = dbObj->cp.uid;
  obj->cp = dbObj->cp;
  obj->cp.m_mappedUserList = dbObj->cp.m_mappedUserList;
  obj->cp.confinfo = dbObj->cp.confinfo;
  obj->sipeventtype = EN_APPNOTIFICATION_TIMEOUT_RES;
  WriteToReceiver(obj);
  CLog::Detail(THISMODULE, "xGatePBXBusiness::pushNotofication_timerThread : After timeout ...");
}

void* callHandling_timerThread(void *arg)
{
  xGateDBObj *dbObj = (xGateDBObj*) arg;

  unsigned int currentIndex = 0;

  for (auto it = (*dbObj->cp.ptrCallHandlingVector).begin(); (it < (*dbObj->cp.ptrCallHandlingVector).end()); ++it, ++currentIndex)
  {
    if (pthread_self() == (it->thread_id))
    {
      CLog::Detail(THISMODULE, "xGatePBXBusiness::callHandling_timerThread ; thread_id = %u ; sec_count = %d", it->thread_id, (it->sec_count));
      if(it->sec_count == 0)
        it->sec_count =  30; // default timeout if no timeou set.
      callHandling_TimerWait((it->sec_count) * 1000);
      break;
    }
  }
  // set currentRecordIndex for all devices === currentIndex... to make identify this thread in SofiaSiphandler...
  // each & every thread overwrites its own "currentIndex" for all devices in list. => helps detect this thread in SofiaSiphandler.
  for (auto it = (*dbObj->cp.ptrCallHandlingVector).begin(), temp_index=0; (it < (*dbObj->cp.ptrCallHandlingVector).end()); ++it, ++temp_index)
  {
    (*dbObj->cp.ptrCallHandlingVector)[temp_index].currentRecordIndex = currentIndex; // to be used in SofiaSiphandler

    if (!it->sequentialRingingTimerStarted) // This is simultaneous ringing case...
    {	// else this is sequential ringing casd...
      it->sequentialRingingTimerStarted = true;
      CLog::Detail(THISMODULE,"xGatePBXBusiness::callHandling_NoOfTimersStarted ; currentRecordIndex = thread_no = %d", temp_index);

      pthread_create(&((*dbObj->cp.ptrCallHandlingVector)[temp_index].thread_id), NULL, &callHandling_timerThread, dbObj);
      CLog::Detail(THISMODULE, "xGatePBXBusiness::callHandling_NoOfTimersStarted ; thread_id = %u", (*dbObj->cp.ptrCallHandlingVector)[temp_index].thread_id);
      break; // break until next thread expires... and fires next thread in turn... until all devices covered.
    }
  }

  CLog::Detail(THISMODULE, "xGatePBXBusiness::callHandling_timerThread : After timeout ...");

  xGateCallObj *obj = new xGateCallObj();
  obj->callid.assign(dbObj->cp.call_id);
  obj->cp = dbObj->cp;
  obj->cp.m_mappedUserList = dbObj->cp.m_mappedUserList;
  obj->cp.confinfo = dbObj->cp.confinfo;
  obj->cp.ptrCallHandlingVector = dbObj->cp.ptrCallHandlingVector;
  obj->sipeventtype = EN_CALLHANDLING_TIMEOUT_RES;
  WriteToReceiver(obj);
  CLog::Detail(THISMODULE, "xGatePBXBusiness::callHandling_timerThread : After timeout ...");
}

void xGatePBXBusiness::AppNotification_TimerStarted(xGateDBObj* dbObj)
{
  CLog::Detail(THISMODULE, "Inside xGatePBXBusiness::AppNotification_TimerStarted");
  if (dbObj->cp.appNotification_timer_thread_1 != 0)
    pthread_cancel(dbObj->cp.appNotification_timer_thread_1);

  pthread_t thread;
  pthread_create(&thread, NULL, &pushNotofication_timerThread, dbObj);

  dbObj->cp.appNotification_timer_thread_1 = thread;
}

void xGatePBXBusiness::callHandling_NoOfTimersStarted(bool order, xGateDBObj* dbObj)
{
  CLog::Detail(THISMODULE, "Inside xGatePBXBusiness::callHandling_NoOfTimersStarted");

  if (order == CALLHANDLING_SIMULTANEOUS_RINGING) // Start N number of timer-threadss. how many devices? -so many timer-threads...
  {
    for (auto it = (*dbObj->cp.ptrCallHandlingVector).begin(), index = 0; (index < (it->device_count)) && (it < (*dbObj->cp.ptrCallHandlingVector).end()); ++it, ++index)
    {
      (*dbObj->cp.ptrCallHandlingVector)[index].currentRecordIndex = index; // to be used in SofiaSiphandler
      CLog::Detail(THISMODULE,"xGatePBXBusiness::callHandling_NoOfTimersStarted ; currentRecordIndex = thread_no = %d", index);

      pthread_create(&((*dbObj->cp.ptrCallHandlingVector)[index].thread_id), NULL, &callHandling_timerThread, dbObj);
      CLog::Detail(THISMODULE, "xGatePBXBusiness::callHandling_NoOfTimersStarted ; thread_id = %u", (*dbObj->cp.ptrCallHandlingVector)[index].thread_id);
    }
  }
  else
  {	// else SEQUENTIAL_ORDER...start only one timer thread -at any given point of time...Track each device one by one; one after another.
    for (auto it = (*dbObj->cp.ptrCallHandlingVector).begin(), index = 0; (index < (it->device_count)) && (it < (*dbObj->cp.ptrCallHandlingVector).end()); ++it, ++index)
    {
      if (!it->sequentialRingingTimerStarted)
      {
        it->sequentialRingingTimerStarted = true; // this makes sure timer is started only once for each device.
        (*dbObj->cp.ptrCallHandlingVector)[index].currentRecordIndex = index; // to be used in SofiaSiphandler
        CLog::Detail(THISMODULE,"xGatePBXBusiness::callHandling_NoOfTimersStarted ; currentRecordIndex = thread_no = %d", index);

        pthread_create(&((*dbObj->cp.ptrCallHandlingVector)[index].thread_id), NULL, &callHandling_timerThread, dbObj);
        CLog::Detail(THISMODULE, "xGatePBXBusiness::callHandling_NoOfTimersStarted ; thread_id = %u", (*dbObj->cp.ptrCallHandlingVector)[index].thread_id);
        break; // added temporarily; to be removed later...
      }
    }
  }
}

void* DialTimeoutThread(void *arg)
{
  CLog::Detail(THISMODULE, "Inside xGatePBXBusiness::DialTimeoutThread");

  xGateDBObj *dbObj = (xGateDBObj*) arg;
  swb_TimerWait((dbObj->cp.gm_dialTimeout)*1000); // 10 seconds
  CLog::Detail(THISMODULE, "xGatePBXBusiness::DialTimeoutThread : Dial-TimeOut happened...");
  xGateCallObj *obj = new xGateCallObj();
  //obj->call_id = dbObj->cp.call_id;
  //obj->call_id.assign(dbObj->cp.call_id.c_str());
  obj->callid.assign(dbObj->cp.call_id);
  obj->cp = dbObj->cp;
  obj->cp.m_mappedUserList = dbObj->cp.m_mappedUserList;
  obj->cp.confinfo = dbObj->cp.confinfo;
  obj->sipeventtype = EN_DIAL_TIMEOUT_RES;
  WriteToReceiver(obj);
}


xGatePBXBusiness::xGatePBXBusiness()
{
}

xGatePBXBusiness::~xGatePBXBusiness()
{
}

xGatePBXBusiness::xGatePBXBusiness(xGateDBObj* dbObj)
{
}
void xGatePBXBusiness::PostMessageToProcessor(xGateDBObj *dbObj){
  xGateSipMsgBlock *pSipRegMsg = new xGateSipMsgBlock(EN_XGATE_MSG_DBCONNECT_IN);
  pSipRegMsg->m_dbObj = dbObj;
  PostMessage(pSipRegMsg,XGATEPOSTTYPE::EN_PROCESS);
}
void xGatePBXBusiness::processInviteRequest(callInfoParameter cinfoparam){
}
std::string xGatePBXBusiness::system_conf_getParam_interface_rtp(std::string sysconf_param, char* rtp_interface)
{
  // System.conf parameters to be retrieved here...
  std::string	 sysconf_param_value;
  CT_Application* papp = CT_App_Manager::Instance()->GetApp(rtp_interface);
  papp->GetAppParam(sysconf_param, sysconf_param_value, true);
  CLog::Detail(THISMODULE, "%s = %s ",sysconf_param.c_str(), sysconf_param_value.c_str());
  return sysconf_param_value;
}
std::string xGatePBXBusiness::system_conf_getParam(std::string sysconf_param)
{
  // System.conf parameters to be retrieved here...
  std::string	 sysconf_param_value;
  CT_Application* papp = CT_App_Manager::Instance()->GetApp(SL_APPLICATION_NAME);
  papp->GetAppParam(sysconf_param, sysconf_param_value, true);
  CLog::Detail(THISMODULE, "%s = %s ",sysconf_param.c_str(), sysconf_param_value.c_str());
  return sysconf_param_value;
}
void xGatePBXBusiness::fetch_SysconfParams(xGateDBObj* dbObj)
{
  callInfoParameter cp = dbObj->cp;
  std::string      sysconf_param = "DIAL_TIMEOUT";
  cp.gm_dialTimeout = atoi(system_conf_getParam(sysconf_param.c_str()).c_str());
  CLog::Detail(THISMODULE, "%s = %d", sysconf_param.c_str(), cp.gm_dialTimeout);

  sysconf_param = "SELECTOR_DB";
  cp.gm_db_selector = system_conf_getParam(sysconf_param.c_str());
  CLog::Detail(THISMODULE, "%s = %s", sysconf_param.c_str(), cp.gm_db_selector.c_str());

  sysconf_param = "SWITCH_CODE";
  cp.gm_switch_code = system_conf_getParam(sysconf_param.c_str());
  CLog::Detail(THISMODULE, "%s = %s", sysconf_param.c_str(), cp.gm_switch_code.c_str());

  //	sysconf_param = "MGC_IP";
  //	cp.gm_mgc_ip = system_conf_getParam(sysconf_param.c_str());
  //	CLog::Detail(THISMODULE, "%s = %s", sysconf_param.c_str(), cp.gm_mgc_ip.c_str());

  sysconf_param = "MY_IP_ADDRESS";
  cp.gm_my_ip_address = system_conf_getParam(sysconf_param.c_str());
  CLog::Detail(THISMODULE, "%s = %s", sysconf_param.c_str(), cp.gm_my_ip_address.c_str());

  sysconf_param = "SITE_CODE";
  cp.gm_site_code = system_conf_getParam(sysconf_param.c_str());
  CLog::Detail(THISMODULE, "%s = %s", sysconf_param.c_str(), cp.gm_site_code.c_str());

  sysconf_param = "TRACE_LEVEL";
  //cp.gm_trace_level = system_conf_getParam(sysconf_param.c_str());
  //CLog::Detail(THISMODULE, "%s = %s", sysconf_param.c_str(), cp.gm_trace_level.c_str());

  sysconf_param = "trunk_in";
  cp.gm_trunk_in = system_conf_getParam(sysconf_param.c_str());
  CLog::Detail(THISMODULE, "%s = %s", sysconf_param.c_str(), cp.gm_trunk_in.c_str());

  sysconf_param = "port_in";
  cp.gm_port_in = system_conf_getParam(sysconf_param.c_str());
  CLog::Detail(THISMODULE, "%s = %s", sysconf_param.c_str(), cp.gm_port_in.c_str());
  dbObj->cp = cp;
}

bool xGatePBXBusiness::CheckAndSet_CallForwardingNumber_if_enabled(xGateDBObj* dbObj, bool playCallForwardAnnounce_inEarlyMedia, xGateDBManager* gm_db_manager)
{
  CLog::Detail(THISMODULE, "Inside xGatePBXBusiness::CheckAndSet_CallForwardingNumber_if_enabled");
  callInfoParameter cp = dbObj->cp;
  cp.gm_isThisCallForwarded_CompanyHours = false;
  cp.gm_isThisCallForwarded_AfterHours = false;
  //bool	isThisCallForwarded = false;
  cp.gm_user_company_hour = 0;

  cp.gm_user_company_hour = gm_db_manager->DATABASE_QUERY_GET_COMPANY_HOUR_USER(cp.gm_B_domain_id.c_str(), dbObj->cp.cnCalled.c_str(), 3);
  //PostMessageToDBManager(dbObj, EN_DB_COMPANYHOURUSER_REQ);
  CLog::Detail(THISMODULE, "xGatePBXBusiness::CheckAndSet_CallForwardingNumber_if_enabled : cp.gm_user_company_hour %d",cp.gm_user_company_hour);
  dbObj->cp = cp;
  return;
}

void xGatePBXBusiness::SetMappedUserNumber_ProxyName_SignalAddress_OutInterface(xGateDBObj* dbObj, const char* mappedUserKey, const char* userNumber, const char* ProxyName, const char* SignalAddress, const char* OutInterface, const char* routeIpAddress)
{
  CLog::Detail(THISMODULE,"Inside xGatePBXBusiness::SetMappedUserNumber_ProxyName_SignalAddress_OutInterface");
  callInfoParameter cp = dbObj->cp;
  cp.m_mappedUserList = dbObj->cp.m_mappedUserList;
  multiDevice *mappedUser = NULL;
  std::map<std::string,  multiDevice*>::iterator it;
  it = cp.m_mappedUserList.find(mappedUserKey);
  if(it != cp.m_mappedUserList.end()) { //entry available for this mappedUserKey
    it->second->m_callId.assign(dbObj->cp.call_id.c_str());
    it->second->m_userNumber = userNumber;
    it->second->m_proxyUserName = ProxyName;
    it->second->m_userSignalAddr = SignalAddress;
    it->second->m_outInterface = OutInterface;
    it->second->m_routeIpAddress = routeIpAddress;
  } else { //entry not available for this mappedUserKey
    //create new entry
    mappedUser = new multiDevice();
    mappedUser->m_callId.assign(dbObj->cp.call_id.c_str());
    mappedUser->m_userNumber = userNumber;
    mappedUser->m_proxyUserName = ProxyName;
    mappedUser->m_userSignalAddr = SignalAddress;
    mappedUser->m_outInterface = OutInterface;
    mappedUser->m_routeIpAddress = routeIpAddress;

    cp.m_mappedUserList[mappedUserKey] = mappedUser;
  }
  dbObj->cp = cp;
  dbObj->cp.m_mappedUserList = cp.m_mappedUserList;
}

void xGatePBXBusiness::SetMappedUserDeviceType(xGateDBObj* dbObj, const char* mappedUserKey, const char* value)
{
  CLog::Detail(THISMODULE,"Inside xGatePBXBusiness::SetMappedUserDeviceType");
  callInfoParameter cp = dbObj->cp;
  cp.m_mappedUserList = dbObj->cp.m_mappedUserList;
  //if (/* cp && */ !cp.IsValid()) {
  // CLog::Detail(THISMODULE,"xGatePBXBusiness::SetMappedUserDeviceType, Invalid call parameter specified");
  //return OK;
  //}
  //find out map entry already been created for this mappedUserKey
  multiDevice *mappedUser = NULL;
  std::map<std::string,  multiDevice*>::iterator it;
  it = cp.m_mappedUserList.find(mappedUserKey);

  if(it != cp.m_mappedUserList.end()) {
    //entry available for this mappedUserKey
    //    it->second->m_calledDeviceType = value;
    if(strstr(value, "PSTN")) {
      //it->second->m_userDeviceType = XGATE_UT_PSTN;
      it->second->m_userDeviceType.assign("PSTN",strlen("PSTN"));
      it->second->m_calledcodec =  8;   
    } 
    else if(strstr(value, "URDESK")){
      //it->second->m_userDeviceType = XGATE_UT_DESKPHONE;
      it->second->m_userDeviceType.assign("URDESK:", strlen("URDESK:"));
      it->second->m_calledcodec =  8;
    }
    else if(strstr(value, "URAPP:IOS")) {
      //it->second->m_userDeviceType = XGATE_UT_URAPP_IOS;
      it->second->m_userDeviceType.assign("URAPP:IOS",strlen("URAPP:IOS"));
      it->second->m_calledcodec =  111;
    } else if(strstr(value, "URAPP:AND")) {
      //it->second->m_userDeviceType = XGATE_UT_URAPP_AND;
      it->second->m_userDeviceType.assign("URAPP:AND",strlen("URAPP:AND"));
      it->second->m_calledcodec =  111;
    } else if(strstr(value, "URWEB:WEB")){
      //it->second->m_userDeviceType = XGATE_UT_URWEB;
      it->second->m_userDeviceType.assign("URWEB:WEB",strlen("URWEB:WEB"));
      it->second->m_calledcodec =  111;
    } else {
      //it->second->m_userDeviceType = XGATE_UT_DESKPHONE;
      it->second->m_userDeviceType.assign("URDESK:",strlen("URDESK:"));
      it->second->m_calledcodec =  8;
    }
  } else {
    //entry not available, create new one
    mappedUser = new multiDevice();
    mappedUser->m_calledDeviceType = value;
    mappedUser->m_userDeviceType= value;
    if(strstr(value,"PSTN")|| strstr(value,"URDESK"))
      mappedUser->m_calledcodec = 8;
    else
      mappedUser->m_calledcodec = 111;

    /*   if(strcmp(value, "PSTN")==0) {
    //mappedUser->m_userDeviceType = XGATE_UT_PSTN;
    //it->second->m_userDeviceType.assign("PSTN",strlen("PSTN"));
    mappedUser->m_calledcodec =  8;
    } else if(strcmp(value, "URDESK:")==0){
    //mappedUser->m_userDeviceType = XGATE_UT_DESKPHONE;
    //it->second->m_userDeviceType.assign("URDESK:", strlen("URDESK:"));
    mappedUser->m_calledcodec =  8;
    } else if(strcmp(value, "URAPP:IOS")==0) {
    //it->second->m_userDeviceType.assign("URAPP:IOS",strlen("URAPP:IOS"));
    mappedUser->m_calledcodec =  111;
    } else if(strcmp(value, "URAPP:AND")==0) {
    //it->second->m_userDeviceType.assign("URAPP:AND",strlen("URAPP:AND"));
    mappedUser->m_calledcodec =  111;
    } else if(strcmp(value, "URWEB:WEB")==0){
    //mappedUser->m_userDeviceType = XGATE_UT_URWEB;
    //it->second->m_userDeviceType.assign("URWEB:WEB",strlen("URWEB:WEB"));
    mappedUser->m_calledcodec =  111;
    } else {
    //mappedUser->m_userDeviceType = XGATE_UT_DESKPHONE;
    //it->second->m_userDeviceType.assign("URDESK:",strlen("URDESK:"));
    mappedUser->m_calledcodec =  8;
    }*/
    cp.m_mappedUserList[mappedUserKey] = mappedUser;
  }
  dbObj->cp = cp;
  dbObj->cp.m_mappedUserList = cp.m_mappedUserList;
  return OK;
}
int xGatePBXBusiness::GetCodecForDeviceType(char* deviceType)
{
  if ((!strcmp(deviceType, "URAPP:IOS")) || (!strcmp(deviceType, "URAPP:AND")) || (!strcmp(deviceType, "URWEB:WEB")))
    return 111;
  return 8;
}
int xGatePBXBusiness::SendAppNotificationsAll(int totalNoOfRegisteredUsers, xGateDBObj* dbObj, int en_db_combinedspregistar_res, xGateDBManager* gm_db_manager)
{
  CLog::Detail(THISMODULE, "Inside xGatePBXBusiness::SendAppNotificationsAll ; gm_totalNoOfRegisteredUsers = %d", totalNoOfRegisteredUsers);

  //	callInfoParameter cp =  dbObj->cp;
  //std::vector<DBQueryResultData>* combine_sp_output_SendAppNotification;
  std::vector<DBQueryResultData>* combine_sp_output_SendAppNotification;
  if (en_db_combinedspregistar_res == EN_DB_COMBINEDSPREGISTAR_RES_1)
    combine_sp_output_SendAppNotification = gm_db_manager->DATABASE_QUERY_combined_SendAppNotification(dbObj->cp.cnCalled.c_str(),  dbObj->cp.gm_user_B_DOMAIN_NAME.c_str(), 4, EN_DB_COMBINEDSPREGISTAR_RES_1);
  else
    combine_sp_output_SendAppNotification = gm_db_manager->DATABASE_QUERY_combined_SendAppNotification(dbObj->cp.cnCalled.c_str(),  dbObj->cp.gm_user_B_DOMAIN_NAME.c_str(), 4, EN_DB_COMBINEDSPREGISTAR_RES_2);
  //combine_sp_output_SendAppNotification = cp.ptr_DBQueryResultData;

  //combine_sp_output_SendAppNotification = gm_db_manager_registrar->DATABASE_QUERY_combined_SendAppNotification(cp.cnCalled.c_str(), cp->gm_user_B_DOMAIN_NAME.c_str(), 4, 1);

  int max_columns_in_records = TOTAL_NOOFCOLUMNS_DB_COMBINEDSP_REGISTRAR - 1;
  int index = 1;
  int app_status = -1;

  std::string temp_Contact_address = "";
  std::string temp_proxyUserName = "";
  std::string temp_previousProxyUserName = "";
  std::string temp_calledDeviceType = "";
  std::string temp_routeIpAddress = "";
  //bool deleteIOSDeviceFromMappedUserList = false;

  //int repeat_counter_value = 2;
  //int repeat_counter_value = 1;
  bool	sendAppNotifications = false;

  //for (int repeat_counter = 0; (repeat_counter < repeat_counter_value); repeat_counter++)
  //{	
  for (auto it = (*combine_sp_output_SendAppNotification).begin(), index = 1; (index <= (totalNoOfRegisteredUsers * max_columns_in_records)) && (it < (*combine_sp_output_SendAppNotification).end()); index++)
  {
    //CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output_SendAppNotification ; gm_totalNoOfRegisteredUsers = [%d] = %s", index, (*(it->data.string_data)).c_str());
    switch (index)
    {
      // STRING data-types
      case 1: // Contact_address
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output_SendAppNotification[%d] = %s", index, (*(it->data.string_data)).c_str());
          temp_Contact_address.assign((*it->data.string_data).c_str());
          //CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output_TRUNKOUT_PORTOUT = %s", cp.gm_trunk_out.c_str());
        }
        else
          temp_Contact_address.assign(CONTACT_ADDRESS_DEFAULT);
        ++it;
        break;

      case 2: // proxy_username
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output_SendAppNotification[%d] = %s", index, (*(it->data.string_data)).c_str());
          temp_proxyUserName.assign((*it->data.string_data).c_str());
          //CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output_TRUNKOUT_PORTOUT = %s", cp.gm_trunk_out.c_str());
        }
        ++it;
        break;

      case 4: // device_type
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output_SendAppNotification[%d] = %s", index, (*(it->data.string_data)).c_str());
          temp_calledDeviceType.assign((*it->data.string_data).c_str());
          //CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output_TRUNKOUT_PORTOUT = %s", cp.gm_trunk_out.c_str());
          //if (totalNoOfRegisteredUsers == 1)
          {
            dbObj->cp.csCalledCodec = GetCodecForDeviceType(temp_calledDeviceType.c_str());
            //dbObj->cp.csCalledCodec = cp.csCalledCodec;
            CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : cp.csCalledCodec[%d] = %d", index, dbObj->cp.csCalledCodec);
          }
        }
        ++it;
        break;

      case 5: // previous_proxy_username
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output_SendAppNotification[%d] = %s", index, (*(it->data.string_data)).c_str());
          temp_previousProxyUserName.assign((*it->data.string_data).c_str());
        }
        ++it;
        break;

      case 6: // route_ip_address
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output_SendAppNotification[%d] = %s", index, (*(it->data.string_data)).c_str());
          temp_routeIpAddress.assign((*it->data.string_data).c_str());
        }
        ++it;
        break;

        // INTEGER data-types
      case 3: // status
        CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output_SendAppNotification[%d] = %d", index, it->data.integer_data);
        app_status = it->data.integer_data;
        ++it;
        break;
      default:
        //CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output == DEFAULT")
        break;
    }
    if ((index % max_columns_in_records) == 0) // repeats for every record...
    {
      index -= max_columns_in_records;
      if ((en_db_combinedspregistar_res == EN_DB_COMBINEDSPREGISTAR_RES_1) && (totalNoOfRegisteredUsers == 1))
      {
        dbObj->cp.csaCalled.assign(temp_Contact_address.c_str());
        dbObj->cp.proxyUserName.assign(temp_proxyUserName.c_str());
        dbObj->cp.calledDeviceType.assign(temp_calledDeviceType.c_str());
        dbObj->cp.routeIpAddress.assign(temp_routeIpAddress.c_str());
        CLog::Detail(THISMODULE, "xGatePBXBusiness::combined_sp_registrar : EN_DB_COMBINEDSPREGISTAR_RES_1: Contact_address = %s ; proxyUserName = %s ; calledDeviceType = %s",dbObj->cp.csaCalled.c_str(), dbObj->cp.proxyUserName.c_str(), dbObj->cp.calledDeviceType.c_str());
      }
      if ((en_db_combinedspregistar_res == EN_DB_COMBINEDSPREGISTAR_RES_1) && (((!(strcmp(temp_calledDeviceType.c_str(), DEVICE_TYPE_URAPP_ANDROID))) || (!(strcmp(temp_calledDeviceType.c_str(), DEVICE_TYPE_URAPP_IOS))))))
      {
        CLog::Detail(THISMODULE, "xGatePBXBusiness::SendAppNotificationsAll : EN_DB_COMBINEDSPREGISTAR_RES_1 ; device_type == RAPP:AND or URAPP:IOS");
        std::string push_from;
        push_from.assign(dbObj->cp.gm_B_domain_id.c_str());
        push_from.append("_");
        push_from.append(dbObj->cp.cnCalling.c_str());

        std::string push_to;
        push_to.assign(dbObj->cp.gm_B_domain_id.c_str());
        push_to.append("_");
        push_to.append(dbObj->cp.cnCalled.c_str());
        SendAppNotification(push_from.c_str(), push_to.c_str(), "", temp_calledDeviceType.c_str(), "audio", dbObj->cp.uid.c_str());
        sendAppNotifications = true;
      }

      if ((en_db_combinedspregistar_res == EN_DB_COMBINEDSPREGISTAR_RES_1) && (totalNoOfRegisteredUsers > 1))
      {
        CLog::Detail(THISMODULE, "xGatePBXBusiness::SetMappedUserDeviceType : EN_DB_COMBINEDSPREGISTAR_RES_1 ");
        std::string mappedUserKey;
        mappedUserKey.assign(dbObj->cp.cnCalled.c_str());
        mappedUserKey.append("_");
        mappedUserKey.append(dbObj->cp.gm_B_domain_id.c_str());
        mappedUserKey.append("_");
        mappedUserKey.append(temp_calledDeviceType.c_str());
        CLog::Detail(THISMODULE, "xGatePBXBusiness::SetMappedUserDeviceType : EN_DB_COMBINEDSPREGISTAR_RES_1 ; SetMappedUserDeviceType()");
        SetMappedUserNumber_ProxyName_SignalAddress_OutInterface(dbObj, mappedUserKey.c_str(), dbObj->cp.cnCalled.c_str(), temp_proxyUserName.c_str(), dbObj->cp.csaCalled.c_str(), dbObj->cp.gm_port_out.c_str(),temp_routeIpAddress.c_str() );
        SetMappedUserDeviceType(dbObj, mappedUserKey.c_str(), temp_calledDeviceType.c_str());
        CLog::Detail(THISMODULE, "xGatePBXBusiness::SetMappedUserDeviceType : EN_DB_COMBINEDSPREGISTAR_RES_1 ");
      }
      if ((en_db_combinedspregistar_res == EN_DB_COMBINEDSPREGISTAR_RES_2) && (totalNoOfRegisteredUsers > 1))
      {
        CLog::Detail(THISMODULE, "xGatePBXBusiness::SetMappedUserDeviceType : EN_DB_COMBINEDSPREGISTAR_RES_2 ; SetMappedUserDeviceType()");
        std::string mappedUserKey;
        mappedUserKey.assign(dbObj->cp.cnCalled.c_str());
        mappedUserKey.append("_");
        mappedUserKey.append(dbObj->cp.gm_B_domain_id.c_str());
        mappedUserKey.append("_");
        mappedUserKey.append(temp_calledDeviceType.c_str());
        if (isIOSDevice(dbObj, temp_calledDeviceType.c_str(), temp_proxyUserName.c_str()) == true)
        {	// to be executed all device-types -Except IOS...
          SetMappedUserNumber_ProxyName_SignalAddress_OutInterface(dbObj, mappedUserKey.c_str(), dbObj->cp.cnCalled.c_str(), temp_proxyUserName.c_str(), temp_Contact_address.c_str(), dbObj->cp.gm_port_out.c_str(),temp_routeIpAddress.c_str() );
          SetMappedUserDeviceType(dbObj, mappedUserKey.c_str(), temp_calledDeviceType.c_str());

        }
      }
    }
  }
  //if (sendAppNotifications)
  //	sleep(3); // give sleep-time to update sdatabase // use event instead of sleep in next implementatin.
  //}
}

void xGatePBXBusiness::db_call_handling(xGateDBObj* dbObj, xGateDBManager* gm_db_manager)
{
  CLog::Detail(THISMODULE, "Inside xGatePBXBusiness::db_call_handling");

  std::vector<DBQueryResultData>* sp_output_call_handling;
  sp_output_call_handling = gm_db_manager->DATABASE_QUERY_CALL_HANDLING(dbObj->cp.cnCalled.c_str(), dbObj->cp.gm_B_domain_id.c_str());

  int max_columns_in_records = 6;
  int index = 1;
  int totalNoOfDBRecords = 1; // Let this be == 1 at the begining; and it will be populated by last column "deviceCount

  std::vector<xGateCallHandlingRecord>* ptrCallHandlingVector = new vector<xGateCallHandlingRecord>;
  dbObj->cp.ptrCallHandlingVector = ptrCallHandlingVector;
  xGateCallHandlingRecord temp_callHandling;

  for (auto it = (*sp_output_call_handling).begin(), index = 1; \
      (index <= (totalNoOfDBRecords * max_columns_in_records)) && (it < (*sp_output_call_handling).end()); index++)
  {
    switch (index)
    {
      // STRING data-types
      case 3: // device_type
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : sp_output_call_handling[%d] = %s", index, (*(it->data.string_data)).c_str());
          //dbObj->cp.callHandling.device_type.assign((*it->data.string_data).c_str());
          temp_callHandling.device_type.assign((*it->data.string_data).c_str());
        }
        ++it;
        break;

      case 5: // ring_type_info
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : sp_output_call_handling[%d] = %s", index, (*(it->data.string_data)).c_str());
          //dbObj->cp.callHandling.ring_type_info.assign((*it->data.string_data).c_str());
          temp_callHandling.ring_type_info.assign((*it->data.string_data).c_str());
        }
        ++it;
        break;

        // INTEGER data-types
      case 1: // no_of_ring
        CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : sp_output_call_handling[%d] = %d", index, it->data.integer_data);
        //dbObj->cp.callHandling.no_of_ring = it->data.integer_data;
        temp_callHandling.no_of_ring = it->data.integer_data;
        ++it;
        break;
      case 2: // sec_count
        CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : sp_output_call_handling[%d] = %d", index, it->data.integer_data);
        //dbObj->cp.callHandling.sec_count = it->data.integer_data;
        temp_callHandling.sec_count = it->data.integer_data;
        ++it;
        break;
      case 4: // ring_type
        CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : sp_output_call_handling[%d] = %d", index, it->data.integer_data);
        //dbObj->cp.callHandling.ring_type = it->data.integer_data;
        temp_callHandling.ring_type = it->data.integer_data;
        ++it;
        break;
      case 6: // device_count
        CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : sp_output_call_handling[%d] = %d", index, it->data.integer_data);
        //dbObj->cp.callHandling.device_count = it->data.integer_data;
        temp_callHandling.device_count = it->data.integer_data;
        totalNoOfDBRecords = it->data.integer_data;
        ++it;
        break;
      default:
        CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output == DEFAULT");
        break;
    }
    if ((index % max_columns_in_records) == 0) // repeats for every record...
    {
      index -= max_columns_in_records;
      dbObj->cp.ptrCallHandlingVector->push_back(temp_callHandling);
    }
  }

  /*
  // following code added for testing; to be removed later...
  GetRegistrationDataFromDB(EN_DB_COMBINEDSPREGISTAR_RES_2, dbObj, gm_db_manager);
  for (auto it = (dbObj->cp.ptr_xGateRegistrationVector)->begin(); (it < (dbObj->cp.ptr_xGateRegistrationVector)->end()); ++it)
  {
  if (!(strstr(it->device_type.c_str(), "WEB")))
  {
  temp_callHandling.device_type.assign(it->device_type.c_str());
  temp_callHandling.ring_type_info.assign((*it->data.string_data).c_str());
  temp_callHandling.no_of_ring = it->data.integer_data;
  temp_callHandling.sec_count = it->data.integer_data;
  temp_callHandling.ring_type = it->data.integer_data;
  temp_callHandling.device_count = it->data.integer_data;
  }
  }
  */
}
void xGatePBXBusiness::set_proxyUserName(xGateDBObj* dbObj, int for_loop_counter)
{
  CLog::Detail(THISMODULE, "Inside xGatePBXBusiness::set_proxyUserName");
  callInfoParameter cp = dbObj->cp;
  for (int index = 0; index < for_loop_counter; index++)
  {
    cp.csaCalled = gm_db_manager_registrar->DATABASE_QUERY_COMBINED_DirectUserInfo_get_Contact_address_Recordwise(cp.cnCalled.c_str(), cp.gm_user_B_DOMAIN_NAME.c_str(), 1, index+1);
    CLog::Detail(THISMODULE, "xGatePBXBusiness::set_proxyUserName : Contact_address = %s",cp.csaCalled.c_str());

    cp.proxyUserName = gm_db_manager_registrar->DATABASE_QUERY_COMBINED_DirectUserInfo_get_proxy_username_Recordwise(cp.cnCalled.c_str(), cp.gm_user_B_DOMAIN_NAME.c_str(), 2, index+1);
    CLog::Detail(THISMODULE, "xGatePBXBusiness::set_proxyUserName : proxy_username = %s",cp.proxyUserName.c_str());

    if (for_loop_counter > 1)
    {
      // set mapped user-info...	
      cp.calledDeviceType = gm_db_manager_registrar->DATABASE_QUERY_COMBINED_DirectUserInfo_get_device_type_Recordwise(cp.cnCalled.c_str(), cp.gm_user_B_DOMAIN_NAME.c_str(), 4, index+1);
      CLog::Detail(THISMODULE, "xGatePBXBusiness::set_proxyUserName : device_type = %s",cp.calledDeviceType.c_str());

      std::string mappedUserKey = cp.cnCalled + "_" + cp.gm_B_domain_id + "_" + cp.calledDeviceType;
      // remove hardcoding for port_out = "rtp1"....
      SetMappedUserNumber_ProxyName_SignalAddress_OutInterface(dbObj, mappedUserKey.c_str(), cp.cnCalled.c_str(), cp.proxyUserName.c_str(), cp.csaCalled.c_str(), cp.gm_port_out.c_str(), "" );
      SetMappedUserDeviceType(dbObj, mappedUserKey.c_str(), cp.calledDeviceType.c_str());
    }
  }
}

void xGatePBXBusiness::setDirectUserInfo(xGateDBObj* dbObj)
{
  CLog::Detail(THISMODULE, "Inside xGatePBXBusiness::setDirectUserInfo");

  //xGateSipMsgBlock* pSipRegMsg;
  //pSipRegMsg = new xGateSipMsgBlock(EN_XGATE_MSG_DBCONNECT_IN);
  //pSipRegMsg->m_dbObj = dbObj;
  //pSipRegMsg->m_dbObj->dbevent = EN_DB_COMBINEDSPREGISTAR_REQ_1;
  //PostMessage(pSipRegMsg,XGATEPOSTTYPE::EN_DBMANAGER);
  //PostMessageToDBManager(dbObj, EN_DB_COMBINEDSPREGISTAR_REQ_1);
  return; // to be removed later...
}

void xGatePBXBusiness::setForkedUserInfo(xGateDBObj* dbObj, int totalNoOfRegisteredUsers)
{
  CLog::Detail(THISMODULE, "Inside xGatePBXBusiness::setForkedUserInfo");

  return; // to be removed later...
}

void xGatePBXBusiness::setUserInfo(xGateDBObj* dbObj)
{
  CLog::Detail(THISMODULE, "Inside xGatePBXBusiness::setUserInfo");
  //int totalNoOfRegisteredUsers = 0; // to be removed later...
  callInfoParameter cp = dbObj->cp;
  int totalNoOfRegisteredUsers = cp.gm_totalNoOfRegisteredUsers;
  setDirectUserInfo(dbObj);
  return;
}

void xGatePBXBusiness::fetch_featureListParams_from_DBmanager(xGateDBObj *dbObj, xGateDBManager* gm_db_manager)
{
  // Fetch call-feature data from database...
  CLog::Detail(THISMODULE, "Inside xGatePBXBusiness::fetch_featureListParams_from_DBmanager");

  callInfoParameter cp;
  cp =  dbObj->cp;
  bool is_a_cli_DESKPHONE = true;
  bool is_a_did_DESKPHONE = true;
  bool is_a_did_mobileNumber = false;

  //cp.isSwitchBoardCall_duringDialledExtension = dbObj->cp.isSwitchBoardCall;
  //dbObj->cp.isSwitchBoardCall_duringDialledExtension = dbObj->cp.isSwitchBoardCall;

  // following line added for testing; to be removed later...
  //cp.cnCalled.assign("442071790305");
  //cp.cnCalling.assign("919850753950");

  if (strlen(cp.cnCalling.c_str()) > 6)
    is_a_cli_DESKPHONE = false;

  if (strlen(cp.cnCalled.c_str()) > 6)
    is_a_did_DESKPHONE = false;

  if ((strlen(cp.cnCalled.c_str()) > 11) && (strlen(cp.cnCalled.c_str()) < 15))
    is_a_did_mobileNumber = true;

  TrimLeadingZeroPlus(dbObj);
  /*
     if (strstr(cp.cnCalling.c_str(),"+"))
     {
     std::string temp_str;
     temp_str.assign(cp.cnCalling.c_str()+1);
     cp.cnCalling.assign(temp_str.c_str());
     dbObj->cp.cnCalling.assign(temp_str.c_str());
     }
     if (strstr(cp.cnCalled.c_str(),"+"))
     {
     std::string temp_str;
     temp_str.assign(cp.cnCalled.c_str()+1);
     dbObj->cp.cnCalled.assign(temp_str.c_str());
     }
     */

  std::string temp_cnCalled;

  dbObj->dbevent = EN_DB_PREQUERY_REQ;
  switch (dbObj->dbevent)
  {
    case EN_DB_PREQUERY_REQ:
      CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : EN_DB_PREQUERY_REQ"); 
      cp.isconferenceCall =  false; // First initilization 
      if (strcmp(cp.cnCalling.c_str(), cp.cnCalled.c_str()) == 0)
      { 	// if both caling and called parties are same; don't process this call. Disconnect this call.
        CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : Both caller and called are same extensions; return DB_ERROR"); 
        break;
      }
      //if ((strlen(cp.cnCalled.c_str()) > 7) && (strlen(strlen(cp.cnCalling.c_str())) > 7))
      if ((cp.isPstnNumber) && (!cp.isSwitchBoardCall))
      {
        std::string temp_routeAddr;
        temp_routeAddr.assign("");

        temp_routeAddr.assign(gm_db_manager->DATABASE_QUERY_get_SwitchBoard_domainName(dbObj->cp.cnCalled.c_str()));
        if (strlen(temp_routeAddr.c_str()) > 0)
        {
          cp.isSwitchBoardCall = true;
          cp.swbinfo.SwbCallEvent =  EN_SWB_WELCOME;
          dbObj->cp.swbinfo.SwbCallEvent = EN_SWB_WELCOME ;
          cp.routeAddr.assign(temp_routeAddr.c_str());
          cp.isPstnNumber = false;
          dbObj->cp.isPstnNumber = false;
        }
        else{
          db_combined_query_initial(dbObj,gm_db_manager);
          if ((dbObj->cp.isconferenceCall) || (dbObj->cp.isHuntGroupCall))
          {
            cp.isPstnNumber = false;
            dbObj->cp.isPstnNumber = false;
            if (!is_a_cli_DESKPHONE)
            {
              cp.routeAddr.assign("");
              dbObj->cp.routeAddr.assign("");
            }
          }

          cp.isSwitchBoardCall = false;
          cp.swbinfo.SwbCallEvent =  EN_SWB_NULL;
          dbObj->cp.swbinfo.SwbCallEvent = EN_SWB_NULL ;

          db_combined_query_DirectNumber(dbObj, gm_db_manager);

          dbObj->cp.directNumberRecord.mappedExtension = gm_db_manager->DATABASE_QUERY_GET_DIRECT_NUMBER(dbObj->cp.cnCalled.c_str(), dbObj->cp.gm_B_domain_id.c_str());
          if (strlen(dbObj->cp.directNumberRecord.mappedExtension.c_str()) == 3)
          {
            dbObj->cp.cnCalled.assign(dbObj->cp.directNumberRecord.mappedExtension.c_str());
            cp.cnCalled.assign(dbObj->cp.directNumberRecord.mappedExtension.c_str());

            if (strlen(cp.cnCalling.c_str()) > 6)
            {
              cp.cnCalling.assign(cp.cnCalling.c_str()+1);
              dbObj->cp.cnCalling.assign(cp.cnCalling.c_str()+1);
            }
            else
            {
              dbObj->cp.cnCalling.assign(cp.cnCalling.c_str());
            }

            is_a_did_DESKPHONE = true;
            dbObj->cp.isPstnNumber = cp.isPstnNumber = false;

            cp.routeAddr.assign(dbObj->cp.directNumberRecord.domainName.c_str());
          }
        }

        //cp.routeAddr.assign(gm_db_manager->DATABASE_QUERY_get_SwitchBoard_domainName(dbObj->cp.cnCalled.c_str()));
      }
      // fall-thru here by skippng "break"
    case EN_DB_SWBDOMAINNAME_RES:
      cp.gm_user_A_DOMAIN_NAME.assign(cp.routeAddr.c_str());

      CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : gm_user_A_DOMAIN_NAME = %s", cp.gm_user_A_DOMAIN_NAME.c_str());
      cp.gm_A_domain_id.assign((gm_db_manager->DATABASE_QUERY_GET_DOMAIN_ID(cp.gm_user_A_DOMAIN_NAME.c_str())).c_str());

      // following hard-coding is added for sake of testing; to be removed later...
      if (((dbObj->cp.isconferenceCall) || (dbObj->cp.isHuntGroupCall)) && (!is_a_cli_DESKPHONE))
      {
        //cp.gm_A_domain_id.assign("1923");
        cp.gm_A_domain_id.assign(dbObj->cp.gm_B_domain_id.c_str());
        cp.gm_user_A_DOMAIN_NAME.assign(gm_db_manager->DATABASE_QUERY_GET_DOMAIN_NAME(dbObj->cp.gm_B_domain_id.c_str()));
      }

      if (cp.gm_user_A_DOMAIN_NAME.empty())
      {
        cp.gm_user_A_DOMAIN_NAME.assign(gm_db_manager->DATABASE_QUERY_get_SwitchBoard_domainName(dbObj->cp.cnCalled.c_str()));
        cp.gm_user_B_DOMAIN_NAME.assign(dbObj->cp.gm_user_A_DOMAIN_NAME.c_str());
      }

      dbObj->cp = cp;
      //PostMessageToDBManager(dbObj, EN_DB_DOMAINID_REQ);
      //break;

    case EN_DB_DOMAINID_RES:
      //cp.gm_A_domain_id = cp.callOnHoldFilenameCallingParty;
      CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : gm_user_A_DOMAIN_NAME = %s", cp.gm_user_A_DOMAIN_NAME.c_str());
      CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : EN_DB_DOMAINID_RES : gm_A_domain_id = %s", cp.gm_A_domain_id.c_str());

      cp.gm_user_B_DOMAIN_NAME.assign(cp.gm_user_A_DOMAIN_NAME.c_str());
      cp.gm_B_domain_id.assign(cp.gm_A_domain_id.c_str());
      CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : EN_DB_SWBDOMAINNAME_RES : gm_user_B_DOMAIN_NAME = %s", cp.gm_user_B_DOMAIN_NAME.c_str());
      CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : EN_DB_DOMAINID_RES : gm_B_domain_id = %s", cp.gm_B_domain_id.c_str());

      if (is_a_did_DESKPHONE)	
        temp_cnCalled = gm_db_manager->DATABASE_QUERY_GET_DIRECT_NUMBER((dbObj->cp.cnCalled.c_str()), cp.gm_B_domain_id.c_str());
      else
        //			std::string pstncalled = dbObj->cp.cnCalled.substr(1);
        temp_cnCalled = gm_db_manager->DATABASE_QUERY_GET_DIRECT_NUMBER((dbObj->cp.cnCalled.substr(1).c_str()), cp.gm_B_domain_id.c_str());

      if (strlen(temp_cnCalled.c_str()) == 3)
      {
        dbObj->cp.cnCalled.assign(temp_cnCalled.c_str());
        cp.cnCalled.assign(temp_cnCalled.c_str());

        if (strlen(cp.cnCalling.c_str()) > 6)
        {
          cp.cnCalling.assign(cp.cnCalling.c_str()+1);
          dbObj->cp.cnCalling.assign(cp.cnCalling.c_str()+1);
        }
        else
        {
          dbObj->cp.cnCalling.assign(cp.cnCalling.c_str());
        }

        is_a_did_DESKPHONE = true;
        dbObj->cp.isPstnNumber = cp.isPstnNumber = false;
      }

      dbObj->cp = cp;
      //if (is_a_cli_DESKPHONE)
      if (is_a_did_DESKPHONE)
      {
        // Check Whether this call is forwarded...
        CheckAndSet_CallForwardingNumber_if_enabled(dbObj, true, gm_db_manager); // contains 3 DATABASE_QUERY
      }
      //else // else dialed-number may be a mobile-number or this may be a PSTN-call
      //PostMessageToDBManager(dbObj, EN_DB_COMBINEDSPSELECTOR_REQ);

      //		break;

    case EN_DB_COMPANYHOURUSER_RES:
      //CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : EN_DB_COMPANYHOURUSER_RES ");
      cp = dbObj->cp;
      TrimLeadingZeroPlus(dbObj);
      /*
         if (strstr(cp.cnCalled.c_str(),"+"))
         {
         std::string temp_str;
         temp_str.assign(cp.cnCalled.c_str()+1);
         dbObj->cp.cnCalled.assign(temp_str.c_str());
         }
         */

      if ((cp.gm_user_company_hour == 1) && (is_a_did_DESKPHONE))
      {
        CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : gm_user_COMPANY_hour == 1 ");
        //PostMessageToDBManager(dbObj, EN_DB_INCOMINGCALLTYPE_REQ);
        //cp.gm_incoming_calling_type = gm_db_manager->DATABASE_QUERY_INCOMING_CALL_TYPE(dbObj->cp.gm_B_domain_id.c_str(), dbObj->cp.cnCalled.c_str(), 1);
        dbObj->cp = cp;
        db_combined_query_callForward(dbObj, gm_db_manager);
        //dbObj->cp = cp;
        cp = dbObj->cp;

        TrimLeadingZeroPlus(dbObj);
        /*
           if (strstr(dbObj->cp.cnCalled.c_str(),"+"))
           {
           std::string temp_str;
           temp_str.assign(cp.cnCalled.c_str()+1);
           dbObj->cp.cnCalled.assign(temp_str.c_str());
           }
           */

        if ((gm_db_manager->DATABASE_QUERY_FORWARD_STATUS_ENABLED(dbObj->cp.gm_B_domain_id.c_str(), dbObj->cp.cnCalled.c_str(), "1", 2) == 1) && (is_a_did_DESKPHONE))
        {
          dbObj->cp.gm_isThisCallForwarded_CompanyHours = true;
          cp.gm_isThisCallForwarded_CompanyHours = true;
        }
        cp = dbObj->cp;
      }
      else
      {
        CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : gm_user_AFTER_hour");
        //PostMessageToDBManager(dbObj, EN_DB_FORWARDSTATUSENABLED_REQ);
        dbObj->cp = cp;
        db_combined_query_callForward(dbObj, gm_db_manager);
        cp = dbObj->cp;
        int temp_int;
        //if (1 == gm_db_manager->DATABASE_QUERY_FORWARD_STATUS_ENABLED(dbObj->cp.gm_B_domain_id.c_str(), dbObj->cp.cnCalled.c_str(), "2", 2))
        TrimLeadingZeroPlus(dbObj);
        temp_int = gm_db_manager->DATABASE_QUERY_FORWARD_STATUS_ENABLED(dbObj->cp.gm_B_domain_id.c_str(), dbObj->cp.cnCalled.c_str(), "2", 2);
        cp = dbObj->cp;
        if ((temp_int == 1) && (is_a_did_DESKPHONE))
        {
          cp.gm_isThisCallForwarded_AfterHours = true;
          dbObj->cp.gm_isThisCallForwarded_AfterHours = true;
        }
        cp = dbObj->cp;
      }
      //break;

    case EN_DB_INCOMINGCALLTYPE_RES:
      //CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : EN_DB_INCOMINGCALLTYPE_RES ");
      // fall-thru here by skippng "break"
    case EN_DB_FORWARDSTATUSENABLED_RES:
      if (dbObj->dbevent == EN_DB_FORWARDSTATUSENABLED_RES)
        CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : EN_DB_FORWARDSTATUSENABLED_RES ");
      // cp.gm_isThisCallForwarded_CompanyHours and gm_isThisCallForwarded_AfterHours are already set in DBManager.cpp
      // cp.gm_isThisCallForwarded_CompanyHours should be considered with gm_incoming_calling_type == 2  --AND NOT with gm_isThisCallForwarded_AfterHours -as below...
      dbObj->cp = cp;
      if (((dbObj->cp.gm_incoming_calling_type == 2) && (dbObj->cp.gm_isThisCallForwarded_CompanyHours)) || \
          dbObj->cp.gm_isThisCallForwarded_AfterHours)
      {
        CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : gm_incoming_calling_type == 2 || gm_isThisCallForwarded_CompanyHours || gm_isThisCallForwarded_AfterHours ");
        //cp.isThisCallForwarded = true;
        //dbObj->cp.isThisCallForwarded = true;
        //PostMessageToDBManager(dbObj, EN_DB_CALLFORWARDNUMBER_REQ);
        //dbObj->cp = cp;
        //cp.gm_call_forward_number = gm_db_manager->DATABASE_QUERY_GET_CALLFORWARD_NUMBER_USER(dbObj->cp.gm_B_domain_id.c_str(), dbObj->cp.cnCalled.c_str(), 4);
        //break;
        //}
        // fall-thru here by skippng "break"
    case EN_DB_CALLFORWARDNUMBER_RES:
        //if (dbObj->dbevent == EN_DB_CALLFORWARDNUMBER_RES)
        //{
        CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : EN_DB_CALLFORWARDNUMBER_RES ");
        // Replace a_did with new call_forward_number...
        if (strlen(cp.gm_call_forward_number.c_str()) >= 3)
        {
          cp.cnCalled.assign(cp.gm_call_forward_number.c_str());
          cp.isThisCallForwarded = true;
          cp.callForwardingFile.assign(WAV_FILE_PATH);
          cp.callForwardingFile.append(CALL_FORWARDED_ANNOUNCEMENT_FILE);
          if ((strlen(cp.cnCalled.c_str()) > 5) && (strlen(cp.cnCalled.c_str()) < 15))
          {
            cp.isPstnNumber = true;
            std::string temp_callforward;
            temp_callforward.assign("fwd");
            temp_callforward.append(cp.cnCalled.c_str());
            cp.cnCalled.assign(temp_callforward.c_str());
          }
          dbObj->cp = cp;
          CheckAndSet_CallForwardingNumber_if_enabled(dbObj, true, gm_db_manager); // contains 3 DATABASE_QUERY
          cp = dbObj->cp;
        }
        dbObj->cp = cp;
        //}
        //db_combined_query_selector(cp, cp);
        //PostMessageToDBManager(dbObj, EN_DB_COMBINEDSPSELECTOR_REQ);
        //break;
  }
  if ((dbObj->cp.isPstnNumber) && (!dbObj->cp.isSwitchBoardCall) && (!dbObj->cp.isSwitchBoardCall_duringDialledExtension))
  {
    CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : isPstnNumber == true");
    dbObj->cp.rbtFile.assign(WAV_FILE_PATH);
    dbObj->cp.rbtFile.append(RBT_DEFAULT_FILE);

    dbObj->cp.callOnHoldFilenameCallingParty.assign(WAV_FILE_PATH);
    dbObj->cp.callOnHoldFilenameCallingParty.append(CALLHOLD_FILE_DEFAULT);

    dbObj->cp.userConnectingFile.assign(WAV_FILE_PATH);
    dbObj->cp.userConnectingFile.append(CONNECTING_FILE_DEFAULT);

    dbObj->cp.gm_user_greeting_file_name_user_B.assign(WAV_FILE_PATH);
    dbObj->cp.gm_user_greeting_file_name_user_B.append(SILENCE_500MS_FILE);
    dbObj->cp.gm_user_greeting_file_name_user_B_After.assign(WAV_FILE_PATH);
    dbObj->cp.gm_user_greeting_file_name_user_B_After.append(SILENCE_500MS_FILE);

    dbObj->cp.csfilename.assign(dbObj->cp.userConnectingFile.c_str());

    db_combined_query_get_pstn_operator_details(dbObj, gm_db_manager);
//    dbObj->cp.csCalledCodec = 8; // Get code based on Operator IP
    if(!dbObj->cp.pstnOperatorDetails.codec.empty()){
      if(strcmp(dbObj->cp.pstnOperatorDetails.codec.c_str(),"G729") == 0)
	dbObj->cp.csCalledCodec =  18;
       else if(strcmp(dbObj->cp.pstnOperatorDetails.codec.c_str(),"PCMU") == 0)
	dbObj->cp.csCalledCodec = 0;	
      else
        dbObj->cp.csCalledCodec = 8;
     }

    if ((dbObj->cp.cnCalled.length() > 10) && (!cp.isThisCallForwarded))
    {
      std::string caller_id;
      caller_id.assign("");
      caller_id = gm_db_manager->DATABASE_QUERY_GET_CALLER_ID(dbObj->cp.gm_B_domain_id.c_str(), dbObj->cp.cnCalling.substr(1).c_str());
      if (caller_id.empty())
      {
        cp.cnCalling.assign("+442034440424");
        dbObj->cp.cnCalling.assign("+442034440424");
      }
      else
      {
        cp.cnCalling.assign(caller_id.c_str());
        dbObj->cp.cnCalling.assign(caller_id.c_str());
      }
    }

    dbObj->dbevent = (XGATEDBEVENT) EN_DB_PREQUERY_RES;
    break;
  }


  case EN_DB_COMBINEDSPSELECTOR_RES:
  //CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : EN_DB_COMBINEDSPSELECTOR_RES ");
  //db_combined_query_selector(dbObj, gm_db_manager);

  //db_combined_query_initial(cp, cp); // sets isSwitchBoardCall
  //PostMessageToDBManager(dbObj, EN_DB_COMBINEDSPINITIAL_REQ);
  //break;

  //combine_sp_output = gm_db_manager->DATABASE_QUERY_COMBINED(cp.cnCalling.c_str(),cp.cnCalled.c_str(),cp.gm_user_B_DOMAIN_NAME.c_str());
  //gm_db_manager->DATABASE_QUERY_COMBINED(cp.cnCalling.c_str(),cp.cnCalled.c_str(),cp.gm_user_B_DOMAIN_NAME.c_str());
  case EN_DB_COMBINEDSPINITIAL_RES:
  //CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : EN_DB_COMBINEDSPINITIAL_RES ");
  dbObj->cp = cp;
  TrimLeadingZeroPlus(dbObj);
  /*
     if (strstr(dbObj->cp.cnCalled.c_str(),"+"))
     {
     std::string temp_str;
     temp_str.assign(cp.cnCalled.c_str()+1);
     dbObj->cp.cnCalled.assign(temp_str.c_str());
     }
     */

  db_combined_query_initial(dbObj,gm_db_manager);
  //cp = dbObj->cp;
  if ((!dbObj->cp.isHuntGroupCall) && (!dbObj->cp.isconferenceCall))
  {
    dbObj->cp.gm_pin_to_connect = gm_db_manager->DATABASE_QUERY_CHECK_PIN_TO_CONNECT(dbObj->cp.gm_B_domain_id.c_str(), dbObj->cp.cnCalled.c_str());
    dbObj->cp.gm_is_callscreening_enabled = gm_db_manager->DATABASE_QUERY_IS_CALLSCREENING_ENABLED(dbObj->cp.cnCalled.c_str(),dbObj->cp.gm_B_domain_id.c_str());
    if (gm_db_manager->DATABASE_QUERY_IS_DND_ENABLED_FOR_APP(dbObj->cp.gm_B_domain_id.c_str(), dbObj->cp.cnCalled.c_str()) == 2)
      dbObj->cp.gm_isDNDEnabled = true;
  }
  cp = dbObj->cp;

  //if ((strlen(cp.cnCalled.c_str()) > 3) && (!cp.isSwitchBoardCall))
  if ((cp.cnCalled.length() > 10) && (!cp.isSwitchBoardCall) && (!dbObj->cp.isSwitchBoardCall_duringDialledExtension) && (!cp.isThisCallForwarded))
  {
    std::string caller_id;
    caller_id.assign("");
    caller_id = gm_db_manager->DATABASE_QUERY_GET_CALLER_ID(dbObj->cp.gm_B_domain_id.c_str(), dbObj->cp.cnCalling.c_str()+1);
    if (caller_id.empty())
      cp.cnCalling.assign("+442034440424");
    else
      cp.cnCalling.assign(caller_id.c_str());
  }

  //if ((strlen(cp.cnCalled.c_str()) > 11) && (strlen(cp.cnCalled.c_str()) < 15))
  if ((cp.isPstnNumber) && (!cp.isSwitchBoardCall) && (!dbObj->cp.isSwitchBoardCall_duringDialledExtension))
  {
    std::string temp_string = cp.cnCalled;
    cp.cnCalled.assign("");
    //	cp.cnCalled.assign("+");
    cp.cnCalled.append(temp_string.c_str());
    CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : cnCalled = %s",cp.cnCalled.c_str());
    db_combined_query_get_pstn_operator_details(dbObj, gm_db_manager);
//    dbObj->cp.csCalledCodec = 8; // Get code based on Operator IP
    if(!dbObj->cp.pstnOperatorDetails.codec.empty()){
     if(strcmp(dbObj->cp.pstnOperatorDetails.codec.c_str(),"G729") == 0)
      dbObj->cp.csCalledCodec =  18;
     else if(strcmp(dbObj->cp.pstnOperatorDetails.codec.c_str(),"PCMU") == 0)
      dbObj->cp.csCalledCodec = 0;
     else
      dbObj->cp.csCalledCodec = 8;
     }

 //   cp.csCalledCodec = 8;
    cp.swbinfo.SwbCallEvent =  EN_SWB_NULL;

  }
  //if ((strlen(cp.cnCalled.c_str()) == 3) || (cp.gm_isThisCallForwarded_CompanyHours || cp.gm_isThisCallForwarded_AfterHours))
  cp.gm_totalNoOfRegisteredUsers = 0;
  //if (cp.cnCalled.length() < 10 || cp.gm_isThisCallForwarded_CompanyHours || cp.gm_isThisCallForwarded_AfterHours)
  if (((cp.cnCalled.length() < 10) || cp.isThisCallForwarded) && (!cp.isSwitchBoardCall))
  {
    //	PostMessageToDBManager(dbObj, EN_DB_TOTALNOOFREGISTEREDUSERS_REQ);
   // cp.gm_totalNoOfRegisteredUsers = gm_db_manager->DATABASE_QUERY_get_totalNoOfRegisteredUsers(cp.cnCalled.c_str(), cp.gm_user_B_DOMAIN_NAME.c_str());
      cp.gm_totalNoOfRegisteredUsers = dbObj->cp.gm_totalNoOfRegisteredUsers; // this hardcoding added for testing; to be removed later...

    if (cp.isHuntGroupCall) // this hardcoding added for testing; to be removed later...
      cp.gm_totalNoOfRegisteredUsers = 2; // this hardcoding added for testing; to be removed later...

  dbObj->cp = cp;
  }
  //else
  {
    //dbObj->cp = cp;
    // following line added for testing; to be removed later...
    //if (!(strcmp(cp.cnCalled.c_str(), "+442071711111")))
    //   if (!(strcmp(cp.cnCalled.c_str(), "900")))
    //	{
    //      	cp.isconferenceCall = true; 
    //		cp.isSwitchBoardCall = false;
    //	}

    if (cp.isSwitchBoardCall)
    {
      cp.rbtFile.assign(WAV_FILE_PATH);
      cp.rbtFile.append(RBT_DEFAULT_FILE);
      cp.swbinfo.SwbCallEvent = EN_SWB_DIAL;
      dbObj->cp = cp;
      //	PostMessageToDBManager(dbObj, START_SWITCHBOARD_CALL_REQ);
      fetch_switchbaordData_from_DBmanager(dbObj, gm_db_manager);
      break; // break here -so that the call-flow is switchboard call-flow specific
    }

    else if (cp.isconferenceCall)
    {
      dbObj->cp.isconferenceCall = true;
      dbObj->cp.rbtFile.assign(WAV_FILE_PATH);
      dbObj->cp.rbtFile.append(RBT_DEFAULT_FILE);
      //		PostMessageToDBManager(dbObj, START_CONFERENCE_CALL_REQ);
      //START_CONFERENCE_BRIDGE(dbObj);
      //PostMessageToDBManager(dbObj, START_CONFERENCE_CALL_REQ);
      dbObj->cp.confinfo.ConfCallEvent = EN_CONF_NULL;
      fetch_conferenceData_from_DBmanager(dbObj, gm_db_manager);
      break; // break here -so that the call-flow is conference call-flow specific
    }
    else if (cp.isHuntGroupCall)
    {
      dbObj->cp.isHuntGroupCall = true;
      dbObj->cp.rbtFile.assign(WAV_FILE_PATH);
      dbObj->cp.rbtFile.append(RBT_DEFAULT_FILE);

      dbObj->cp.gm_user_greeting_file_name_user_B.assign(WAV_FILE_PATH);
      dbObj->cp.gm_user_greeting_file_name_user_B.append(SILENCE_500MS_FILE);

      dbObj->cp.userConnectingFile.assign(WAV_FILE_PATH);
      dbObj->cp.userConnectingFile.append(SILENCE_500MS_FILE);
      //fetch_HuntGroupData_from_DBmanager(dbObj, gm_db_manager);
      cp = dbObj->cp;
      //break;  // comment if not calling //fetch_HuntGroupData_from_DBmanager()
    }
    else
    {
      //DialTimeout(dbObj);
    }
  }

  //break;

  case EN_DB_TOTALNOOFREGISTEREDUSERS_RES:
  CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : EN_DB_TOTALNOOFREGISTEREDUSERS_RES : gm_totalNoOfRegisteredUsers = %d", cp.gm_totalNoOfRegisteredUsers);

  if (cp.gm_totalNoOfRegisteredUsers > 1)
    cp.isForkingenabled = 1;

  //if (cp.gm_totalNoOfRegisteredUsers > 0)
  //	setUserInfo(dbObj);
  //break;

  case EN_DB_COMBINEDSPREGISTAR_RES_1:
  CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : EN_DB_COMBINEDSPREGISTAR_RES_1 : gm_totalNoOfRegisteredUsers = %d", cp.gm_totalNoOfRegisteredUsers);
  dbObj->cp = cp;
  if (cp.gm_totalNoOfRegisteredUsers == 1)
  {
populateData(dbObj);
    //SendAppNotificationsAll(cp.gm_totalNoOfRegisteredUsers,dbObj, EN_DB_COMBINEDSPREGISTAR_RES_1, gm_db_manager);
//  populateOneDeviceData(EN_DB_COMBINEDSPREGISTAR_RES_1, dbObj, gm_db_manager);
  }

  cp = dbObj->cp;
  cp.m_mappedUserList =  dbObj->cp.m_mappedUserList;

  //PostMessageToDBManager(dbObj, EN_DB_COMBINEDSPREGISTAR_REQ_2);
  //break;

  case EN_DB_COMBINEDSPREGISTAR_RES_2:
  CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : EN_DB_COMBINEDSPREGISTAR_RES_2 : gm_totalNoOfRegisteredUsers = %d", cp.gm_totalNoOfRegisteredUsers);
  //gm_db_manager->DATABASE_QUERY_combined_SendAppNotification(dbObj->cp.cnCalled.c_str(),  dbObj->cp.gm_user_B_DOMAIN_NAME.c_str(), 4, 1);
  if (cp.gm_totalNoOfRegisteredUsers > 1)
    cp.isForkingenabled = 1;

  dbObj->cp = cp;
  if (cp.gm_totalNoOfRegisteredUsers == 1)
  {

 populateData(dbObj);
    //SendAppNotificationsAll(cp.gm_totalNoOfRegisteredUsers,dbObj, EN_DB_COMBINEDSPREGISTAR_RES_2, gm_db_manager);
 //   populateOneDeviceData(EN_DB_COMBINEDSPREGISTAR_RES_2, dbObj, gm_db_manager);
  }
  cp = dbObj->cp;
  cp.m_mappedUserList = dbObj->cp.m_mappedUserList;
  if (cp.gm_user_company_hour)
  {
    if (strlen(cp.gm_user_greeting_file_name_user_B.c_str()) > 0)
    {
      cp.userGreetingFile.assign(PBXSHARE_PATH);
      cp.userGreetingFile.append(cp.gm_B_domain_id.c_str());
      cp.userGreetingFile.append(UG_PATH);
      cp.userGreetingFile.append(cp.gm_user_greeting_file_name_user_B.c_str());
      //cp.csfilename.assign(cp.userGreetingFile.c_str());
    }
    else
    {
      cp.csfilename.assign(WAV_FILE_PATH);
      cp.csfilename.append(SILENCE_500MS_FILE);
    }
  }
  else
  {
    if (strlen(cp.gm_user_greeting_file_name_user_B_After.c_str()) > 0)
    {
      //cp.csfilename.assign(PBXSHARE_PATH);
      //cp.csfilename.append(cp.gm_B_domain_id.c_str());
      //cp.csfilename.append(UG_PATH);
      cp.userGreetingFile.assign(PBXSHARE_PATH);
      cp.userGreetingFile.append(cp.gm_B_domain_id.c_str());
      cp.userGreetingFile.append(UG_PATH);
      cp.userGreetingFile.append(cp.gm_user_greeting_file_name_user_B_After.c_str());
    }
    else
    {
      cp.csfilename.assign(WAV_FILE_PATH);
      cp.csfilename.append(SILENCE_500MS_FILE);
    }
  }
  if(cp.gm_user_rbt_user_B_type == 1)
  {
    cp.rbtFile.assign(WAV_FILE_PATH);
    cp.rbtFile.append(RBT_DEFAULT_FILE);
  }
  else
  {
    cp.rbtFile.assign(PBXSHARE_PATH);
    cp.rbtFile.append(cp.gm_B_domain_id.c_str());
    cp.rbtFile.append(UG_PATH);
    cp.rbtFile.append(cp.gm_user_rbt_file_name_user_B.c_str());
  }
  CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : user_B RBT-file = %s",cp.rbtFile.c_str());
  /*
     if (!dbObj->cp.gm_legA_deskphone_Status)
     {
     cp.userConnectingFile.assign(WAV_FILE_PATH);
     cp.userConnectingFile.append("not_authorized.wav");
     }
     if (!dbObj->cp.gm_legB_deskphone_Status)
     {
     cp.userConnectingFile.assign(WAV_FILE_PATH);
     cp.userConnectingFile.append("sorry_incorrect_extension.wav");
     }
     if (dbObj->cp.gm_is_blocked_number == 1)
     {
     cp.userConnectingFile.assign(WAV_FILE_PATH);
     cp.userConnectingFile.append(cp.gm_blocked_no_ivr.c_str());
  // Make sure all other files are set to "" as below...
  cp.callForwardingFile.assign("");
  cp.gm_user_greeting_file_name_user_B.assign("");
  cp.gm_user_greeting_file_name_user_B_After.assign("");
  //After playing gm_blocked_no_ivr; Disconnect call
  }
  */
  dbObj->cp = cp;
  // Inform SofiaSiphandler to dialout to leg-B
  if (!dbObj->cp.isSwitchBoardCall_duringDialledExtension)
    dbObj->dbevent = (XGATEDBEVENT) EN_DB_PREQUERY_RES;
  else
  {
    // else this is dial-out for an extension thru SwitchBoard-call
    dbObj->cp.swbinfo.SwbCallEvent = EN_SWB_DIAL;
    dbObj->dbevent = EN_DB_SWB_EVENT_RES;
    CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager  EN_SWB_EVENT_REL ");
  }
  break;

  case EN_DB_CONFERENCECALL_RES:
  CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : EN_DB_CONFERENCECALL_RES ");
  if (cp.confinfo.ConfCallEvent == EN_CONF_GETDTMF_COMPLET)
  {
    CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : EN_CONF_GETDTMF_COMPLET ");
    //PostMessageToDBManager(dbObj, EN_DB_CONFPIN_REQ);

    break;
  }
  // fall-thru here by skippng "break"
  case EN_DB_SWBCOMBINEDSP_RES:
  if (cp.isSwitchBoardCall)
  {
    CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : EN_DB_SWBCOMBINEDSP_RES ");
    db_combined_query_swb(dbObj, gm_db_manager);
    // all global variables of Switchboard-feature should be initialized here...

  }

  cp.rbtFile.assign(WAV_FILE_PATH);
  cp.rbtFile.append(RBT_DEFAULT_FILE);
  dbObj->cp = cp;
  // Inform SofiaSiphandler to connect to SwitchBoard-call
  {
    //This is dial-out for extension dialled thru SwitchBoard
    dbObj->cp.swbinfo.SwbCallEvent = EN_SWB_DIAL;
    dbObj->dbevent = (XGATEDBEVENT) EN_SWB_EVENT_REL;
  }
  break;

  case EN_SWB_EVENT_REQ:
  CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : EN_SWB_EVENT_REQ ");
  // following line is added for testing; to be removed later...
  //START_CONFERENCE_BRIDGE(dbObj); break;
  case EN_SWBMAINMENUIVR_TIMEOUT_RES:
  if (dbObj->dbevent == EN_SWBMAINMENUIVR_TIMEOUT_RES)
    CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : EN_SWBMAINMENUIVR_TIMEOUT_RES ");

  if ((strlen(dbObj->cp.swbinfo.DigitReceived.c_str()) > 0) && (!dbObj->cp.gm_userDiallDTMFdigits))
  {
    dbObj->cp.gm_userDiallDTMFdigits = true;
    CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : EN_SWB_EVENT_REQ ; DigitReceived = %s", dbObj->cp.swbinfo.DigitReceived.c_str());
    if ((!strcmp(dbObj->cp.swbinfo.DigitReceived.c_str(),"0")) || (!(strcmp(dbObj->cp.swbinfo.DigitReceived.c_str(),"$$$"))) || (!(strcmp(dbObj->cp.swbinfo.DigitReceived.c_str(),SWB_MAINMENUIVR_TIMEOUT_WITH_NO_DTMF_DIGITS))))
    {
      if ((!(strcmp(dbObj->cp.swbinfo.DigitReceived.c_str(),SWB_MAINMENUIVR_TIMEOUT_WITH_NO_DTMF_DIGITS))) && (dbObj->cp.gm_swb_auto_routing == 1))
        dbObj->cp.swbinfo.DigitReceived.assign("0");

      // following line added for testing, to be removed later...
      dbObj->cp.swbinfo.DigitReceived.assign("0");

      //PostMessageToDBManager(dbObj, EN_DB_OPERATOREXTN_REQ);
      if (strlen(dbObj->cp.gm_swb_operator_extension.c_str()) <= 0)
        cp.cnCalled.assign("200");
      else
        cp.cnCalled.assign(dbObj->cp.gm_swb_operator_extension.c_str());

      CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : EN_SWB_EVENT_REQ ; Operator-extension = %s", cp.cnCalled.c_str());

      // following code to be un-commented after merging of code...

      if (!(strcmp(dbObj->cp.swbinfo.DigitReceived.c_str(),"0")))
      {
        dbObj->cp.gm_userDiallDTMFdigits = true;
        // Paly IVR "Connecting to operator"
        cp.swbinfo.PlayFile.assign(WAV_FILE_PATH);
        cp.swbinfo.PlayFile.append(CONNECT_TO_OPERATOR_FILE);
        dbObj->cp.swbinfo.SwbCallEvent = EN_SWB_PLAY;
        dbObj->dbevent = (XGATEDBEVENT) EN_SWB_EVENT_REL;
        break;
      }

    }
    else
      cp.cnCalled.assign(dbObj->cp.swbinfo.DigitReceived.c_str());

    //cp.csfilename.assign(WAV_FILE_PATH);
    //cp.csfilename.append(RBT_DEFAULT_FILE);

    // Inform SofiaSiphandler to connect to SwitchBoard-call
    //dbObj->dbevent = EN_DIALOUT_REL;
    //PostMessageToDBManager(dbObj, EN_DB_DOMAINID_REQ);
    //PostMessageToDBManager(dbObj, EN_DB_COMBINEDSPSELECTOR_REQ);

    //cp.swbinfo.SwbCallEvent = EN_SWB_DIAL;
    //dbObj->dbevent = EN_SWB_EVENT_REL;

    //cp.routeAddr.assign("6398.UR.mundio.com");
    dbObj->cp.gm_user_B_DOMAIN_NAME.assign(cp.routeAddr.c_str());
    dbObj->cp.gm_user_A_DOMAIN_NAME.assign(cp.routeAddr.c_str());

    cp.gm_totalNoOfRegisteredUsers = 1;
    //cp.cnCalling.assign("201");

    //setUserInfo(cp,cp,dbObj);
    //PostMessageToDBManager(dbObj, GET_EXTENSION_DATA_REQ);
    //PostMessageToDBManager(dbObj, EN_DB_DOMAINID_REQ);
    //PostMessageToDBManager(dbObj, EN_DB_DOMAINID_REQ);
    //dbObj->dbevent = EN_DIALOUT_REL;
    break;
  }
  if ((dbObj->dbevent == EN_SWBMAINMENUIVR_TIMEOUT_RES) && (!dbObj->cp.gm_userDiallDTMFdigits) && (dbObj->cp.gm_swb_auto_routing != 1))
  {
    // Timeout occurred, and user not dialled anything and NO auto-routing also. Disconnect this call.
    /*
       if (!(strcmp(dbObj->cp.swbinfo.DigitReceived.c_str(),"0")))
       {
       dbObj->cp.gm_userDiallDTMFdigits = true;
       cp.swbinfo.PlayFile.assign(WAV_FILE_PATH);
       cp.swbinfo.PlayFile.append(NO_INPUT_GOODBYE_FILE);
       dbObj->cp.swbinfo.SwbCallEvent = EN_SWB_PLAY;
       dbObj->dbevent = EN_SWB_EVENT_REL;
       break;
       }
     ****/
  }
  SWB_Company_hour_greeting(dbObj);
  SWITCHBOARD_FEATURE_MAIN_LOOP(dbObj);
  dbObj->dbevent = (XGATEDBEVENT) EN_SWB_EVENT_REL;
  break;

  case EN_CONFERENCE_DTMFDIGIT_TIMEOUT_RES:
  CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : EN_CONFERENCEIVR_TIMEOUT_RES ");

  if ((strlen(dbObj->cp.confinfo.DigitReceived.c_str()) > 0) && (!dbObj->cp.gm_userDiallDTMFdigits))
  {
    dbObj->cp.gm_userDiallDTMFdigits = true;
    CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : EN_CONFERENCEIVR_TIMEOUT_RES ; DigitReceived = %s", dbObj->cp.confinfo.DigitReceived.c_str());

    //cp.routeAddr.assign("6398.UR.mundio.com");
    dbObj->cp.gm_user_B_DOMAIN_NAME.assign(cp.routeAddr.c_str());
    dbObj->cp.gm_user_A_DOMAIN_NAME.assign(cp.routeAddr.c_str());
    //cp.cnCalling.assign("201");

    //PostMessageToDBManager(dbObj, START_CONFERENCE_CALL_REQ);
    break;
  }
  break;

  case EN_CONF_EVENT_REQ:
  CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : EN_CONF_EVENT_REQ ");
  //case START_CONFERENCE_CALL_RES:
  //if (dbObj->cp.ConfCallEvent == EN_CONF_GETDTMF_COMPLET)
  if (strlen(dbObj->cp.confinfo.DigitReceived.c_str()) > 3)
  {
    CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : EN_CONF_EVENT_REQ : EN_CONF_GETDTMF_COMPLET");
    //PostMessageToDBManager(dbObj, EN_DB_CONFPIN_REQ);

    break;
  }	
  cp.csfilename.assign("/root/wav_files/sounds/vectone/CR_welcome_prompt.wav");
  cp.confinfo.PlayFile.assign("/root/wav_files/sounds/vectone/CR_welcome_prompt.wav");
  dbObj->dbevent = (XGATEDBEVENT) EN_CONF_EVENT_REL;
  break;

  case EN_DB_CONFPIN_RES:
  CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : EN_DB_CONFPIN_RES ");
  break;

  default:
  CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : DEFAULT!!! ");
  break;
}
// added for tesing; to be removed later...
return;
}

void xGatePBXBusiness::SendAppNotification(const char* from, const char* to, const char* user_cli, const char* device_type,const char* call_type,const char* iuid)
{
  char sys_cmd[800];
  CLog::Warning(THISMODULE,"SendAppNotification from: %s to: %s for call_type: %s and device_type: %s", from, to, call_type, device_type);
  if(device_type ){
    if(strstr(device_type, "DCAPP:IOS")) {
      sprintf(sys_cmd, "curl --max-time 1 'http://gocheapappapi.mundio.com/v1/user/Gcpushnotification?origin=%s&target=%s&isvoip=true&media=%s&calltype=true&callmode=ps&iscancel=0'", from, to, call_type);
    } else if(strstr(device_type, "DCAPP:AND")) {
      sprintf(sys_cmd,"curl --max-time 1 'http://im01.delightcalling.com/wakeup.php?origin=%s&target=%s&calltype=%s'", from, to, call_type);
    } else if(strstr(device_type, "URAPP:IOS")) { 
      sprintf(sys_cmd, "curl --max-time 1 'http://urmyaccount.mundio.com/v1/user/urpushnotification?origin=%s&target=%s&isvoip=true&media=audio&iscancel=0&isLive=true&iuid=%s_%s'", from, to,iuid,xGateUtil::getLocalIpAddr());
    } else if(strstr(device_type, "URAPP:AND")) { 
      //sprintf(sys_cmd,"curl --max-time 1 'http://82.113.74.51/ejabberd/wakeup.php?target=%s&origin=%s'", to, from);
      sprintf(sys_cmd,"curl --max-time 1 'http://urmyaccount.mundio.com/v1/user/urpushnotificationFCM?origin=%s&target=%s&isvoip=false&iscancel=0&iuid=%s_%s'", from, to,iuid,xGateUtil::getLocalIpAddr());
      //sprintf(sys_cmd,"curl --max-time 1 'http://urmyaccount.mundio.com/v1/user/urpushnotificationFCM?origin=%s&target=%s&isvoip=false&iscancel=0'", from, to);
    } else if (strstr(device_type, "VAPP:IOS")) {
      sprintf(sys_cmd,"curl --max-time 1 'http://vectoneapp.mundio.com/api/v1/Vectoneapppushnotification?origin=%s&target=%s&usercli=%s&isvoip=true&media=%s&iscancel=0'", from, to, user_cli, call_type);
    } else if(strstr(device_type, "VAPP:AND")) {
      sprintf(sys_cmd,"curl --max-time 1 'http://im01.vectone.com/vectone/wakeup1.php?origin=%s&target=%s&usercli=%s'", from, to, user_cli);
    }
    system(sys_cmd);
  }
  CLog::Detail(THISMODULE,"Push Norification %s", sys_cmd);
  return OK;
}
void xGatePBXBusiness::CancelAppNotificationIOS(const char* from, const char* to, const char* user_cli, const char* device_type,const char* call_type,const char* iuid)
{
  char sys_cmd[800];
  CLog::Warning(THISMODULE,"CancelAppNotificationIOS from: %s to: %s for call_type: %s and device_type: %s", from, to, call_type, device_type);
  if(device_type ){
    if(strstr(device_type, "URAPP:IOS")) { 
      sprintf(sys_cmd, "curl --max-time 1 'http://urmyaccount.mundio.com/v1/user/urpushnotification?origin=%s&target=%s&isvoip=true&media=audio&iscancel=1&isLive=true&iuid=%s_%s'", from, to,iuid,xGateUtil::getLocalIpAddr());
    }
    system(sys_cmd);
  }
  CLog::Detail(THISMODULE,"Cancel Norification %s", sys_cmd);
  return OK;
}

bool xGatePBXBusiness::PostMessage(xGateSipMsgBlock *SipMsg, XGATEPOSTTYPE destType){
  ACE_Message_Block* pAmb = NULL;
  pAmb = static_cast<xGateSipMsgBlock*> (SipMsg);
  ACE_Time_Value tval ((time(NULL) ));
  tval += ACE_Time_Value(0,1);
  ACE_Task<ACE_MT_SYNCH>* taskPtr = NULL;
  std::string handleName;
  switch(destType){
    case XGATEPOSTTYPE::EN_PROCESS:
      handleName = "SipSignalProcessor";
      taskPtr = (xGateSipSignalProcessor*)GetSipPostHandler(XGATEPOSTTYPE::EN_PROCESS);
      break;
    case XGATEPOSTTYPE::EN_DBMANAGER:
      handleName = "DBManager";
      taskPtr = (xGateSipSignalProcessor*)GetSipPostHandler(XGATEPOSTTYPE::EN_DBMANAGER);
      break;
    default:
      break;
  }
  if(taskPtr){
    if(taskPtr->putq(pAmb,&tval))
      CLog::Detail(THISMODULE,"xGatePBXBusiness::PostMessage(%s) Successfully",handleName.c_str());
    else
      CLog::Detail(THISMODULE,"xGatePBXBusiness::PostMessage(%s) Failed",handleName.c_str());
  }
  else{
    CLog::Detail(THISMODULE,"xGatePBXBusiness::PostMessage(%s) Task Pointer Empty",handleName.c_str());
  }
  return true;
}
void xGatePBXBusiness::db_combined_query_initial(xGateDBObj* dbObj, xGateDBManager* gm_db_manager)
{
  callInfoParameter cp = dbObj->cp;
  // Combined sp...
  //DBQueryResultData combine_sp_output;
  std::vector<DBQueryResultData>* combine_sp_output;
  //combine_sp_output = gm_db_manager->DATABASE_QUERY_COMBINED(cp.cnCalling.c_str(),"442071790305",cp.gm_user_B_DOMAIN_NAME.c_str());
  CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : db_combined_query_initial : gm_user_B_DOMAIN_NAME = %s", cp.gm_user_B_DOMAIN_NAME.c_str());
  if ((strlen(cp.cnCalling.c_str()) > 10) && (dbObj->cp.isSwitchBoardCall))// caller is is PSTN-call
  {
    // Caller should not be a pstn - number; instead pass caller as called number into this sp i.e. first argument == called
    combine_sp_output = gm_db_manager->DATABASE_QUERY_COMBINED(cp.cnCalled.c_str(),cp.cnCalled.c_str(),cp.gm_user_B_DOMAIN_NAME.c_str());
  }
  else
    combine_sp_output = gm_db_manager->DATABASE_QUERY_COMBINED(cp.cnCalling.c_str(),cp.cnCalled.c_str(),cp.gm_user_B_DOMAIN_NAME.c_str());
  //combine_sp_output = cp.ptr_DBQueryResultData;
  //vector<DBQueryResultData> vector_combine_sp_output = *combine_sp_output;
  //int index = 1;
  for (auto it = (*combine_sp_output).begin(), index = 1; (index <= TOTAL_NOOFCOLUMNS_DB_COMBINEDSP_INITIAL) && (it < (*combine_sp_output).end()); index++)
  {
    switch (index)
    {
      // STRING data-types
      case COLUMN_1: // domain_id
        if (it->data_type)
        {
          cp.gm_B_domain_id.assign((*it->data.string_data).c_str());
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output[%d] = %s", index, (*it->data.string_data).c_str());
        }
        ++it;
        break;

      case COLUMN_14: // user_greeting_file
        if (it->data_type)
        {
          cp.gm_direct_call_pin.assign((*it->data.string_data).c_str());
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output[%d] = %s", index, (*it->data.string_data).c_str());
        }
        ++it;
        break;

      case COLUMN_18: // user_greeting_file
        if (it->data_type)
        {
          cp.gm_blocked_no_ivr.assign(WAV_FILE_PATH);
          cp.gm_blocked_no_ivr.append((*it->data.string_data).c_str());
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output[%d] = %s", index, (*it->data.string_data).c_str());
        }
        ++it;
        break;

      case COLUMN_25: // user_greeting_file
        if ((it->data_type) && (!cp.isPstnNumber))
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output[%d] = %s", index, (*(it->data.string_data)).c_str());
          cp.gm_user_greeting_file_name_user_B.assign(PBXSHARE_PATH);
          cp.gm_user_greeting_file_name_user_B.append(cp.gm_B_domain_id.c_str());
          cp.gm_user_greeting_file_name_user_B.append(UG_PATH);
          cp.gm_user_greeting_file_name_user_B.append((*it->data.string_data).c_str());
          //cp.gm_user_greeting_file_name_user_B.append(getDBColumnValue(it));
          //cp.gm_user_greeting_file_name_user_B.assign(WAV_FILE_PATH);
        }
        //if (cp.isPstnNumber)
        else
        {
          cp.gm_user_greeting_file_name_user_B.assign(WAV_FILE_PATH);
          cp.gm_user_greeting_file_name_user_B.append(SILENCE_500MS_FILE);
        }
        ++it;
        break;
      case COLUMN_27: // ddi_user_hold_music
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output[%d] = %s", index, (*it->data.string_data).c_str());
          cp.callOnHoldFilenameCalledParty.assign(WAV_FILE_PATH);
          cp.callOnHoldFilenameCalledParty.append((*it->data.string_data).c_str());
          if (strlen(cp.callOnHoldFilenameCalledParty.c_str()) <= 0)
            cp.callOnHoldFilenameCalledParty.append(CONNECTING_FILE_DEFAULT);
          //cp.callOnHoldFilenameCalledParty = *(it->data.string_data);
          //CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : callOnHoldFilenameCalledParty = %s", cp.callOnHoldFilenameCalledParty.c_str());
        }
        ++it;
        break;
      case COLUMN_28: // cli_user_hold_music
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output[%d] = %s", index, (*it->data.string_data).c_str());
          //cp.callOnHoldFilenameCallingParty.assign((*it->data.string_data).c_str());
          //cp.callOnHoldFilenameCallingParty = *(it->data.string_data);
          cp.callOnHoldFilenameCallingParty.assign(WAV_FILE_PATH);
          cp.callOnHoldFilenameCallingParty.append((*it->data.string_data).c_str());
          if (strlen(cp.callOnHoldFilenameCallingParty.c_str()) <= 0)
            cp.callOnHoldFilenameCallingParty.append(CONNECTING_FILE_DEFAULT);
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : callOnHoldFilenameCallingParty = %s", cp.callOnHoldFilenameCallingParty.c_str());
        }
        ++it;
        break;
      case COLUMN_30: // user_after_greeting_file
        if ((it->data_type) && (!cp.isPstnNumber))
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output[%d] = %s", index, (*it->data.string_data).c_str());
          cp.gm_user_greeting_file_name_user_B_After.assign(PBXSHARE_PATH);
          cp.gm_user_greeting_file_name_user_B_After.append(cp.gm_B_domain_id.c_str());
          cp.gm_user_greeting_file_name_user_B_After.append(UG_PATH);
          cp.gm_user_greeting_file_name_user_B_After.append((*it->data.string_data).c_str());
        }
        else
        {
          cp.userConnectingFile.assign(WAV_FILE_PATH);
          cp.userConnectingFile.append(SILENCE_500MS_FILE);
          cp.gm_user_greeting_file_name_user_B_After.assign(cp.userConnectingFile.c_str());
        }
        ++it;
        break;
      case COLUMN_32: // connecting_ivr
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output[%d] = %s", index, (*it->data.string_data).c_str());
          cp.userConnectingFile.assign(PBXSHARE_PATH);
          cp.userConnectingFile.append(cp.gm_B_domain_id.c_str());
          cp.userConnectingFile.append(UG_PATH);
          cp.userConnectingFile.append((*it->data.string_data).c_str());
          //cp.csfilename.assign(cp.userConnectingFile.c_str());
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : userConnectingFile = %s", cp.userConnectingFile.c_str());
        }
        else
        {
          cp.userConnectingFile.assign(WAV_FILE_PATH);
          cp.userConnectingFile.append(SILENCE_500MS_FILE);
          cp.csfilename.assign(cp.userConnectingFile.c_str());
        }
        if (cp.isPstnNumber)
        {
          cp.userConnectingFile.assign(WAV_FILE_PATH);
          cp.userConnectingFile.append("connectingmessage.wav");
          cp.csfilename.assign(cp.userConnectingFile.c_str());
        }
        ++it;
        break;
      case COLUMN_34: // audio_connecting_file
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output[%d] = %s", index, (*it->data.string_data).c_str());
          cp.gm_user_rbt_file_name_user_B.assign((*it->data.string_data).c_str());
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : cp.gm_user_rbt_file_name_user_B = %s", cp.gm_user_rbt_file_name_user_B.c_str());
        }
        ++it;
        break;

      case COLUMN_40: // host_pin
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output[%d] = %s", index, (*it->data.string_data).c_str());
          cp.confinfo.gm_PIN_from_database.assign((*it->data.string_data).c_str());
        }
        ++it;
        break;

      case COLUMN_41: // participant_pin
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output[%d] = %s", index, (*it->data.string_data).c_str());
          cp.confinfo.gm_PARTICIPANT_PIN_from_database.assign((*it->data.string_data).c_str());
        }
        ++it;
        break;

        // INTEGER data-types
      case COLUMN_2: // is_swith_board
        CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output[%d] = %d", index, it->data.integer_data);
        if (it->data.integer_data == 1)
        {
          cp.isSwitchBoardCall = true;
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : isSwitchBoardCall = true");
        }
        else
        {
          cp.isSwitchBoardCall = false;
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : isSwitchBoardCall = false");
        }
        ++it;
        break;

      case COLUMN_4: // swb_domainid
        CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output[%d] = %d", index, it->data.integer_data);
        ++it;
        break;

      case COLUMN_6: // hunt_group
        CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output[%d] = %d", index, it->data.integer_data);
        if ((it->data.integer_data == 1) || (!strcmp(cp.cnCalled.c_str(),"777"))) // this hardcoding to be removed later..
        {
          cp.isHuntGroupCall = true;
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : isHuntGroupCall = true");
        }
        else
        {
          cp.isHuntGroupCall = false;
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : isHuntGroupCall = false");
        }
        ++it;
        break;

      case COLUMN_8: // isconferenceCall
        CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output[%d] = %d", index, it->data.integer_data);
        if (it->data.integer_data == 1)
        {
          cp.isconferenceCall = true;
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : isconferenceCall = true");
        }
        else
        {
          cp.isconferenceCall = false;
          //CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : isconferenceCall = false");
        }
        ++it;
        break;

      case COLUMN_12: // legA_deskphone_Status
        if (it->data.integer_data == 1)
          cp.gm_is_direct_call_status = true;
        CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output[%d] = %d", index, it->data.integer_data);
        ++it;
        break;
      case COLUMN_13: // legA_deskphone_Status
        if (it->data.integer_data == 1)
          cp.gm_is_direct_call_pin_required = true;
        CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output[%d] = %d", index, it->data.integer_data);
        ++it;
        break;
      case COLUMN_15: // legA_deskphone_Status
        if (it->data.integer_data != 1)
          cp.gm_legA_deskphone_Status = false;
        CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output[%d] = %d", index, it->data.integer_data);
        ++it;
        break;
      case COLUMN_16: // legB_deskphone_Status
        if ((it->data.integer_data != 1) && (!cp.isSwitchBoardCall))
          cp.gm_legB_deskphone_Status = false;
        CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output[%d] = %d", index, it->data.integer_data);
        ++it;
        break;

      case COLUMN_17: // block_type
        cp.gm_block_type = it->data.integer_data;
        CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output[%d] = %d", index, it->data.integer_data);
        ++it;
        break;

      case COLUMN_19: // is_blocked_number
        if (it->data.integer_data == 1)
          cp.gm_is_blocked_number = true;
        else
          cp.gm_is_blocked_number = false;
        CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output[%d] = %d", index, it->data.integer_data);
        ++it;
        break;

      case COLUMN_29: // company_hour
        // Let following line be commented -as gm_user_company_hour is modified during Call-Forwarding above.
        //////cp.gm_user_company_hour = it->data.integer_data;
        CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output[%d] = %d", index, it->data.integer_data);
        ++it;
        break;
      case COLUMN_33: // audio_connecting_Type
        cp.gm_user_rbt_user_B_type = it->data.integer_data;
        CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output[%d] = %d", index, it->data.integer_data);
        ++it;
        break;
      case COLUMN_35:        // ddi_user_hold_music_type
        CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output[%d] = %d", index, it->data.integer_data);
        ++it;
        break;
      case COLUMN_36:        // cli_user_hold_music_type
        CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output[%d] = %d", index, it->data.integer_data);
        ++it;
        break;
      default:
        //CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output == DEFAULT");
        break;
    }
  }
  //CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : cp.cnCalled = %s", cp.cnCalled.c_str());
  dbObj->cp = cp;
}	
void xGatePBXBusiness::GetRegisrtationDeatils(xGateDBObj* dbobj, xGateDBManager* gm_db_manager){
  CLog::Detail(THISMODULE, "xGatePBXBusiness::GetRegisrtationDeatils %s",dbobj->cp.uid.c_str());
  populateOneDeviceData(EN_DB_COMBINEDSPINITIAL_REQ,dbobj,gm_db_manager);
  if(dbobj->cp.ptr_xGateRegistrationVector->size()> 1){
    dbobj->cp.isForkingenabled = 1;
    updateMappedUserList(dbobj);

  }

}
void xGatePBXBusiness::db_combined_query_selector(xGateDBObj* dbObj, xGateDBManager* gm_db_manager)
{
  callInfoParameter cp = dbObj->cp;
  // following db_query is to be achieved using gm_db_manager...
  std::vector<DBQueryResultData>* combine_sp_output_TRUNKOUT_PORTOUT;
  combine_sp_output_TRUNKOUT_PORTOUT = gm_db_manager->DATABASE_QUERY_get_TRUNKOUT_PORTOUT(cp.gm_site_code.c_str(), \
      cp.gm_switch_code.c_str(), cp.cnCalled.c_str(), cp.cnCalling.c_str(), cp.gm_trunk_in.c_str(), cp.gm_port_in.c_str()  );

  combine_sp_output_TRUNKOUT_PORTOUT = cp.ptr_DBQueryResultData;
  int index = 1;
  for (auto it = (*combine_sp_output_TRUNKOUT_PORTOUT).begin(), index = 1; (index <= TOTAL_NOOFCOLUMNS_DB_COMBINEDSP_SELECTOR) && (it < (*combine_sp_output_TRUNKOUT_PORTOUT).end()); index++)
  {
    switch (index)
    {
      // STRING data-types
      case COLUMN_7: // trunk_out
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output_TRUNKOUT_PORTOUT[%d] = %s", index, (*(it->data.string_data)).c_str());
          cp.gm_trunk_out.assign((*it->data.string_data).c_str());
          //CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output_TRUNKOUT_PORTOUT = %s", cp.gm_trunk_out.c_str());
        }
        ++it;
        break;
      case COLUMN_8: // port_out
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output_TRUNKOUT_PORTOUT[%d] = %s", index, (*it->data.string_data).c_str());
          cp.gm_port_out.assign((*it->data.string_data).c_str());
          //CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output_TRUNKOUT_PORTOUT = %s", cp.gm_port_out.c_str());
        }
        ++it;
        break;
      default:
        //CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : combine_sp_output == DEFAULT");
        break;
    }
  }
  dbObj->cp = cp;
}
void xGatePBXBusiness::db_combined_query_swb(xGateDBObj* dbObj, xGateDBManager* gm_db_manager)
{
  callInfoParameter cp = dbObj->cp;
  std::vector<DBQueryResultData>* combine_sp_swb_output;
  combine_sp_swb_output = gm_db_manager->DATABASE_QUERY_SWB_COMBINED(dbObj->cp.gm_A_domain_id.c_str(), dbObj->cp.cnCalled.c_str());
  //combine_sp_swb_output = gm_db_manager->DATABASE_QUERY_SWB_COMBINED("1676", "442071790210");
  //combine_sp_swb_output = cp.ptr_DBQueryResultData;
  for (auto it = (*combine_sp_swb_output).begin(), index = 1; (index <= TOTAL_NOOFCOLUMNS_DB_COMBINEDSP_SWB) && (it < (*combine_sp_swb_output).end()); index++)
  {
    switch (index)
    {
      // STRING data-types
      case 5: // comp_hours_ivr_file
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : SWB_combine_sp_output[%d] = %s", index, (*(it->data.string_data)).c_str());
          cp.gm_swb_custom_ivr.assign((*it->data.string_data).c_str());
          dbObj->cp.gm_swb_custom_ivr.assign((*it->data.string_data).c_str());
        }
        ++it;
        break;

      case 7: // comp_hours_ivr_file_after
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : SWB_combine_sp_output[%d] = %s", index, (*(it->data.string_data)).c_str());
          cp.gm_swb_custom_ivr_after.assign((*it->data.string_data).c_str());
          dbObj->cp.gm_swb_custom_ivr_after.assign((*it->data.string_data).c_str());
        }
        else
          dbObj->cp.gm_swb_custom_ivr_after.assign(dbObj->cp.gm_swb_custom_ivr.c_str());
        ++it;
        break;

      case 10: // gm_swb_comp_hour_extension
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : SWB_combine_sp_output[%d] = %s", index, (*(it->data.string_data)).c_str());
          cp.gm_swb_comp_hour_extension.assign((*it->data.string_data).c_str());
          dbObj->cp.gm_swb_comp_hour_extension.assign((*it->data.string_data).c_str());
        }
        ++it;
        break;

      case 11: // gm_swb_comp_hour_extension
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : SWB_combine_sp_output[%d] = %s", index, (*(it->data.string_data)).c_str());
          cp.gm_swb_comp_hour_extension_after.assign((*it->data.string_data).c_str());
          dbObj->cp.gm_swb_comp_hour_extension_after.assign((*it->data.string_data).c_str());
        }
        ++it;
        break;

      case 15: // switcboardid
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : SWB_combine_sp_output[%d] = %s", index, (*(it->data.string_data)).c_str());
          cp.gm_swb_swb_id.assign((*it->data.string_data).c_str());
          dbObj->cp.gm_swb_swb_id.assign((*it->data.string_data).c_str());
        }
        ++it;
        break;

      case 16: // opr_extension
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : SWB_combine_sp_output[%d] = %s", index, (*(it->data.string_data)).c_str());
          cp.gm_swb_operator_extension.assign((*it->data.string_data).c_str());
          dbObj->cp.gm_swb_operator_extension.assign((*it->data.string_data).c_str());
        }
        ++it;
        break;

        // INTEGER data-types
      case 2: // company_hour
        cp.gm_swb_company_hour = it->data.integer_data;
        dbObj->cp.gm_swb_company_hour = it->data.integer_data;
        CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : SWB_combine_sp_output[%d] = %d", index, it->data.integer_data);
        ++it;
        break;
      case 4: // comp_hours_ivr_type
        CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : SWB_combine_sp_output[%d] = %d", index, it->data.integer_data);
        cp.gm_swb_custom_ivr_present = it->data.integer_data;
        dbObj->cp.gm_swb_custom_ivr_present = it->data.integer_data;
        ++it;
        break;

      case 6: // comp_hours_ivr_type_after
        CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : SWB_combine_sp_output[%d] = %d", index, it->data.integer_data);
        cp.gm_swb_custom_ivr_present_after = it->data.integer_data;
        dbObj->cp.gm_swb_custom_ivr_present_after = it->data.integer_data;
        ++it;
        break;

      case 8: // Greeting_Play_Type
        CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : SWB_combine_sp_output[%d] = %d", index, it->data.integer_data);
        cp.gm_swb_greeting_type = it->data.integer_data;
        dbObj->cp.gm_swb_greeting_type = it->data.integer_data;
        ++it;
        break;

      case 9: // gm_swb_greeting_type_after
        CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : SWB_combine_sp_output[%d] = %d", index, it->data.integer_data);
        cp.gm_swb_greeting_type_after = it->data.integer_data;
        dbObj->cp.gm_swb_greeting_type_after = it->data.integer_data;
        ++it;
        break;

      case 12: // gm_swb_caller_no_action
        CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : SWB_combine_sp_output[%d] = %d", index, it->data.integer_data);
        cp.gm_swb_caller_no_action = it->data.integer_data;
        dbObj->cp.gm_swb_caller_no_action = it->data.integer_data;
        ++it;
        break;

      case 13: // gm_swb_caller_no_action_after
        CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : SWB_combine_sp_output[%d] = %d", index, it->data.integer_data);
        cp.gm_swb_caller_no_action_after = it->data.integer_data;
        dbObj->cp.gm_swb_caller_no_action_after = it->data.integer_data;
        ++it;
        break;

      case 14: // gm_swb_auto_routing
        CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : SWB_combine_sp_output[%d] = %d", index, it->data.integer_data);
        cp.gm_swb_auto_routing = it->data.integer_data;
        dbObj->cp.gm_swb_auto_routing = it->data.integer_data;
        ++it;
        break;

      default:
        break;
    }
  }
  //CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : cp.cnCalled = %s", cp.cnCalled.c_str());
  dbObj->cp = cp;
}	
void xGatePBXBusiness::db_combined_query_conf_info(xGateDBObj* dbObj, xGateDBManager* gm_db_manager)
{
  std::vector<DBQueryResultData>* combine_sp_conf_output;
  combine_sp_conf_output = gm_db_manager->DATABASE_QUERY_COMBINED_CONF_INFO(dbObj->cp.confinfo.gm_conf_id.c_str(),dbObj->cp.gm_A_domain_id.c_str());
  //combine_sp_conf_output = cp.ptr_DBQueryResultData;
  for (auto it = (*combine_sp_conf_output).begin(), index = 1; (index <= TOTAL_NOOFCOLUMNS_DB_COMBINEDSP_CONF_INFO) && (it < (*combine_sp_conf_output).end()); index++)
  {
    switch (index)
    {
      // STRING data-types
      case COLUMN_3: // conference_status
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : db_combined_query_conf_info[%d] = %s", index, (*(it->data.string_data)).c_str());
          dbObj->cp.confinfo.gm_conference_status.assign((*it->data.string_data).c_str());
        }
        ++it;
        break;

      case COLUMN_22: // host_pin
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : db_combined_query_conf_info[%d] = %s", index, (*(it->data.string_data)).c_str());
          //dbObj->cp.confinfo.gm_PIN_from_database.assign((*it->data.string_data).c_str());
        }
        ++it;
        break;

      case COLUMN_23: // participant_pin
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : db_combined_query_conf_info[%d] = %s", index, (*(it->data.string_data)).c_str());
          //dbObj->cp.confinfo.gm_PARTICIPANT_PIN_from_database.assign((*it->data.string_data).c_str());
        }
        ++it;
        break;

        // INTEGER data-types
      case COLUMN_7: // status_flag
        dbObj->cp.confinfo.gm_conf_status_flag = it->data.integer_data;
        CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : db_combined_query_conf_info[%d] = %d", index, it->data.integer_data);
        ++it;
        break;

      default:
        break;
    }
  }
}
void xGatePBXBusiness::db_combined_query_get_pstn_operator_details(xGateDBObj* dbObj, xGateDBManager* gm_db_manager)
{
  std::vector<DBQueryResultData>* combine_sp_conf_output;
  combine_sp_conf_output = gm_db_manager->DATABASE_QUERY_COMBINED_GET_PSTN_OPERATOR_DETAILS(dbObj->cp.csaCalled.c_str());
  for (auto it = (*combine_sp_conf_output).begin(), index = 1; (index <= TOTAL_NOOFCOLUMNS_DB_COMBINEDSP_GETOPERATORDETAILS) && (it < (*combine_sp_conf_output).end()); index++)
  {
    switch (index)
    {
      // STRING data-types
      #if 0
 case COLUMN_1: // operator_name
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : db_combined_query_conf_info[%d] = %s", index, (*(it->data.string_data)).c_str());
          dbObj->cp.pstnOperatorDetails.operatorName.assign((*it->data.string_data).c_str());
        }
        ++it;
        break;
#endif
      case COLUMN_2: // codec
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : db_combined_query_conf_info[%d] = %s", index, (*(it->data.string_data)).c_str());
          dbObj->cp.pstnOperatorDetails.codec.assign((*it->data.string_data).c_str());
        }
        ++it;
        break;

      case COLUMN_3: // vtCode
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : db_combined_query_conf_info[%d] = %s", index, (*(it->data.string_data)).c_str());
          dbObj->cp.pstnOperatorDetails.vtCode.assign((*it->data.string_data).c_str());
        }
        ++it;
        break;

      default:
        break;
    }
  }
}
void xGatePBXBusiness::db_combined_query_conf_validate_pin(xGateDBObj* dbObj, xGateDBManager* gm_db_manager)
{
  std::vector<DBQueryResultData>* combine_sp_conf_output;
  combine_sp_conf_output = gm_db_manager->DATABASE_QUERY_COMBINED_CONF_VALIDATE_PIN(dbObj->cp.gm_B_domain_id.c_str(),dbObj->cp.confinfo.DigitReceived.c_str());
  for (auto it = (*combine_sp_conf_output).begin(), index = 1; (index <= TOTAL_NOOFCOLUMNS_DB_COMBINEDSP_CONF_INFO) && (it < (*combine_sp_conf_output).end()); index++)
  {
    switch (index)
    {
      // STRING data-types
      case COLUMN_2: // extension
        if (it->data_type)
        {
          dbObj->cp.confinfo.gm_conf_host_extension.assign((*it->data.string_data).c_str());
          CLog::Detail(THISMODULE, "xGatePBXBusiness::db_combined_query_conf_validate_pin : db_combined_query_conf_info[%d] = %s", index, dbObj->cp.confinfo.gm_conf_host_extension.c_str());
        }
        ++it;
        break;

        // INTEGER data-types
      case COLUMN_1: // conf_user_type
        dbObj->cp.confinfo.gm_conf_user_type = it->data.integer_data;
        CLog::Detail(THISMODULE, "xGatePBXBusiness::db_combined_query_conf_validate_pin : db_combined_query_conf_info[%d] = %d", index, it->data.integer_data);
        ++it;
        break;

      default:
        break;
    }
  }
}

void xGatePBXBusiness::db_combined_query_conf_get_callid_mguid(xGateDBObj* dbObj, xGateDBManager* gm_db_manager)
{
  std::vector<DBQueryResultData>* combine_sp_conf_output;
  combine_sp_conf_output = gm_db_manager->DATABASE_QUERY_COMBINED_CONF_GET_CALLID_MGUID(dbObj->cp.confinfo.gm_conf_host_extension.c_str(),dbObj->cp.gm_user_B_DOMAIN_NAME.c_str());
  int max_columns_in_records = TOTAL_NOOFCOLUMNS_DB_COMBINEDSP_CONF_REGISTERINFO;
  for (auto it = combine_sp_conf_output->begin(), index = 1; (index <= TOTAL_NOOFCOLUMNS_DB_COMBINEDSP_CONF_REGISTERINFO) && (it < combine_sp_conf_output->end()); index++)
  {
            switch (index)
            {
		// STRING data-types
		/* Not getting data for column 1
                case COLUMN_1: // call_id
			 if (it->data_type)
			 {
	                 	CLog::Detail(THISMODULE, "xGatePBXBusiness::db_combined_query_conf_get_callid_mguid : db_combined_query_conf_info[%d] = %s", index, (*(it->data.string_data)).c_str());
			 	dbObj->cp.confinfo.gm_call_id.assign((*it->data.string_data).c_str());
			 	dbObj->cp.confinfo.gm_mgResourceId.assign((*it->data.string_data).c_str());
			 }
			 ++it;
                         break;
		*/
		case COLUMN_1:
			++it;
			break;

		case COLUMN_2:
		if (it->data_type){
CLog::Detail(THISMODULE, "xGatePBXBusiness::db_combined_query_conf_get_callid_mguid : db_combined_query_conf_info[%d] = %s", index, (*(it->data.string_data)).c_str());
	dbObj->cp.confinfo.gm_conference_status.assign((*it->data.string_data).c_str());
		}
		++it;
		break;
                case COLUMN_7: // MG_uid
			 if (it->data_type)
			 {
	                 	CLog::Detail(THISMODULE, "xGatePBXBusiness::db_combined_query_conf_get_callid_mguid : db_combined_query_conf_info[%d] = %s", index, (*(it->data.string_data)).c_str());
			 	dbObj->cp.confinfo.gm_mgResourceId.assign((*it->data.string_data).c_str());
			 }
			 ++it;
                         break;

		// integer-type
                case COLUMN_6: // participant_type
			 if (it->data.integer_data == 1)
				dbObj->cp.confinfo.gm_is_host_user_already_in_confCall = true;
	                 CLog::Detail(THISMODULE, "xGatePBXBusiness::db_combined_query_conf_get_callid_mguid : combine_sp_output[%d] = %d", index, it->data.integer_data);
			 ++it;
                         break;
                case COLUMN_8: // rec_count
				dbObj->cp.confinfo.noOfConfParticipantsCurrentlyJoined = it->data.integer_data;
	                 CLog::Detail(THISMODULE, "xGatePBXBusiness::db_combined_query_conf_get_callid_mguid : combine_sp_output[%d] = %d", index, it->data.integer_data);
			 ++it;
                         break;
		default:
			break;
            }
                if ((index % max_columns_in_records) == 0) // repeats for every record...
                        index -= max_columns_in_records;
        }
       
        // integer-type
    }


void xGatePBXBusiness::db_combined_query_callForward(xGateDBObj* dbObj, xGateDBManager* gm_db_manager)
{
  callInfoParameter cp = dbObj->cp;
  std::vector<DBQueryResultData>* combine_sp_callforward_output;
  combine_sp_callforward_output = gm_db_manager->DATABASE_QUERY_INCOMING_CALL_TYPE_COMBINED_GET_CALLFORWARD_NUMBER(dbObj->cp.gm_B_domain_id.c_str(),      dbObj->cp.cnCalled.c_str());
  //combine_sp_callforward_output = cp.ptr_DBQueryResultData;
  for (auto it = (*combine_sp_callforward_output).begin(), index = 1; (index <= 9) && (it < (*combine_sp_callforward_output).end()); index++)
  {
    switch (index)
    {
      // STRING data-types
      case COLUMN_4: // forward_to
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : db_combined_query_callForward[%d] = %s", index, (*(it->data.string_data)).c_str());
          cp.gm_call_forward_number.assign((*it->data.string_data).c_str());
        }
        ++it;
        break;

        // INTEGER data-types
      case COLUMN_1: // incoming_call_type
        cp.gm_incoming_calling_type = it->data.integer_data;
        dbObj->cp.gm_incoming_calling_type = it->data.integer_data;
        CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : db_combined_query_callForward[%d] = %d", index, it->data.integer_data);
        ++it;
        break;
      default:
        break;
    }
  }
  dbObj->cp = cp;
}	
void xGatePBXBusiness::db_combined_query_DirectNumber(xGateDBObj* dbObj, xGateDBManager* gm_db_manager)
{
  std::vector<DBQueryResultData>* combine_sp_directNumber_output;
  combine_sp_directNumber_output = gm_db_manager->DATABASE_QUERY_GET_DIRECT_NUMBER_COMBINEDSP(dbObj->cp.cnCalled.c_str(), dbObj->cp.gm_B_domain_id.c_str());

  for (auto it = (*combine_sp_directNumber_output).begin(), index = 1; (index <= TOTAL_NOOFCOLUMNS_DB_COMBINEDSP_DIRECT_NUMBER) && (it < (*combine_sp_directNumber_output).end()); index++)
  {
    switch (index)
    {
#if 0
      // STRING data-types
      case COLUMN_1: // mobile_no
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::db_combined_query_DirectNumber : combine_sp_directNumber_output[%d] = %s", index, (*(it->data.string_data)).c_str());
          dbObj->cp.directNumberRecord.mappedExtension.assign((*it->data.string_data).c_str());
        }
        ++it;
        break;
#endif
      case COLUMN_8: // domain_name
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::db_combined_query_DirectNumber : combine_sp_directNumber_output[%d] = %s", index, (*(it->data.string_data)).c_str());
          dbObj->cp.directNumberRecord.domainName.assign((*it->data.string_data).c_str());
        }
        ++it;
        break;

      default:
        break;
    }
  }
}	
void xGatePBXBusiness::db_combined_query_conf_Id(xGateDBObj* dbObj, xGateDBManager* gm_db_manager)
{
  std::vector<DBQueryResultData>* combine_sp_conf_output;
  combine_sp_conf_output = gm_db_manager->DATABASE_QUERY_COMBINED_CONF_ID(dbObj->cp.gm_A_domain_id.c_str(), dbObj->cp.confinfo.DigitReceived.c_str());

  for (auto it = (*combine_sp_conf_output).begin(), index = 1; (index <= TOTAL_NOOFCOLUMNS_DB_COMBINEDSP_CONF_ID) && (it < (*combine_sp_conf_output).end()); index++)
  {
    switch (index)
    {
      // STRING data-types
      case COLUMN_3: // host_pin
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : db_combined_query_conf_Id[%d] = %s", index, (*(it->data.string_data)).c_str());
          //dbObj->cp.confinfo.gm_PIN_from_database.assign((*it->data.string_data).c_str());
        }
        ++it;
        break;

      case COLUMN_4: // participant_pin
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : db_combined_query_conf_Id[%d] = %s", index, (*(it->data.string_data)).c_str());
          //dbObj->cp.confinfo.gm_PARTICIPANT_PIN_from_database.assign((*it->data.string_data).c_str());
        }
        ++it;
        break;

        // INTEGER data-types
      case COLUMN_1: // err_code
        dbObj->cp.confinfo.gm_valid_conf_id = false;
        if (it->data.integer_data == 0)
        {
          dbObj->cp.confinfo.gm_valid_conf_id = true;
          dbObj->cp.confinfo.gm_conf_id.assign(dbObj->cp.confinfo.DigitReceived.c_str());
        }
        else
        {
          dbObj->cp.confinfo.gm_valid_conf_id = false;
          //dbObj->cp.confinfo.gm_conf_id.assign("");
        }
        CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : db_combined_query_conf_Id[%d] = %d", index, it->data.integer_data);
        ++it;
        break;

      default:
        break;
    }
  }
}

void xGatePBXBusiness::SWITCHBOARD_FEATURE_MAIN_LOOP(xGateDBObj* dbObj)
{
  CLog::Detail(THISMODULE, "Inside xGatePBXBusiness::SWITCHBOARD_FEATURE_MAIN_LOOP");
  callInfoParameter cp = dbObj->cp;

  int swb_MainMenuIvr_repeat_counter = MAINMENU_IVR_REPEAT_COUNTER + 1;

  // Get Current state of switchboard from call_param; g_swb_call_state_current is changed only by SofiaSiphandler.
}
void xGatePBXBusiness::SWB_Company_hour_greeting(xGateDBObj* dbObj)
{
  CLog::Detail(THISMODULE, "Inside xGatePBXBusiness::SWB_Company_hour_greeting");
  //callInfoParameter cp = dbObj->cp;
  if(dbObj->cp.gm_swb_company_hour == 1)
  {
    if(dbObj->cp.gm_swb_greeting_type == 1)
    {
      if(dbObj->cp.gm_swb_custom_ivr_present == 1)
      {
        dbObj->cp.gm_swb_company_greeting_file.assign(PBXSHARE_PATH);
        dbObj->cp.gm_swb_company_greeting_file.append(dbObj->cp.gm_B_domain_id.c_str());
        dbObj->cp.gm_swb_company_greeting_file.append(CH_PATH);
        dbObj->cp.gm_swb_company_greeting_file.append(dbObj->cp.gm_swb_custom_ivr.c_str());
      }
      if (dbObj->cp.gm_swb_caller_no_action == 1)
        dbObj->cp.gm_swb_auto_routing = 1;
    }
    if(dbObj->cp.gm_swb_greeting_type == 2)
    { // DialOut to extension
      if (strlen(dbObj->cp.gm_swb_comp_hour_extension.c_str()) > 0)
        dbObj->cp.cnCalled.assign(dbObj->cp.gm_swb_comp_hour_extension.c_str());
      else
        dbObj->cp.cnCalled.assign(dbObj->cp.gm_swb_operator_extension.c_str());
      //PostMessageToDBManager(dbObj, EN_DB_DOMAINID_REQ);
    }
  }
  else
  {
    if (dbObj->cp.gm_swb_greeting_type_after == 1)
    {
      if(dbObj->cp.gm_swb_custom_ivr_present_after == 1)
      {
        dbObj->cp.gm_swb_company_greeting_file.assign(PBXSHARE_PATH);
        dbObj->cp.gm_swb_company_greeting_file.append(dbObj->cp.gm_B_domain_id.c_str());
        dbObj->cp.gm_swb_company_greeting_file.append(CH_PATH);
        dbObj->cp.gm_swb_company_greeting_file.append(dbObj->cp.gm_swb_custom_ivr_after.c_str());
      }
      if (dbObj->cp.gm_swb_caller_no_action_after == 1)
        dbObj->cp.gm_swb_auto_routing = 1;
    }
    if(dbObj->cp.gm_swb_greeting_type_after == 2)
    { // DialOut to extension
      if (strlen(dbObj->cp.gm_swb_comp_hour_extension_after.c_str()) > 0)
        dbObj->cp.cnCalled.assign(dbObj->cp.gm_swb_comp_hour_extension_after.c_str());
      else
        dbObj->cp.cnCalled.assign(dbObj->cp.gm_swb_operator_extension.c_str());
      //PostMessageToDBManager(dbObj, EN_DB_DOMAINID_REQ);
    }
  }
  if (strlen(dbObj->cp.gm_swb_company_greeting_file.c_str()) <= 0)
    dbObj->cp.gm_swb_company_greeting_file.assign("/pbxshare/1703/CH/1703_Company_hour_ivr1573742773.wav");

  CLog::Detail(THISMODULE, "xGatePBXBusiness::SWB_Company_hour_greeting : gm_swb_company_greeting_file = %s", dbObj->cp.gm_swb_company_greeting_file.c_str());
  dbObj->cp.csfilename.assign(dbObj->cp.gm_swb_company_greeting_file.c_str());
  dbObj->cp.swbinfo.PlayFile.assign(dbObj->cp.gm_swb_company_greeting_file.c_str());

  //dbObj->cp = cp;
}
void xGatePBXBusiness::Swb_MainMenuIvrTimeout(xGateDBObj* dbObj)
{
  CLog::Detail(THISMODULE, "Inside xGatePBXBusiness::Swb_MainMenuIvrTimeout");
  pthread_t thread;
  void *ret;
  pthread_create(&thread, NULL, &swb_timerThread, dbObj);
  //pthread_join(thread,&ret);
}
void xGatePBXBusiness::Conf_DTMFDigitTimeout(xGateDBObj* dbObj)
{
  CLog::Detail(THISMODULE, "Inside xGatePBXBusiness::Conf_DTMFDigitTimeout");
  if (!dbObj->cp.confinfo.gm_conf_id.empty())
#ifdef GET_PIN_ONLY
    CLog::Detail(THISMODULE, "xGatePBXBusiness::Conf_DTMFDigitTimeout::PIN == %s", dbObj->cp.confinfo.gm_conf_id.c_str());
#else
  CLog::Detail(THISMODULE, "xGatePBXBusiness::Conf_DTMFDigitTimeout::CONF-ID == %s", dbObj->cp.confinfo.gm_conf_id.c_str());
#endif;
  if (dbObj->cp.confinfo.conf_timer_thread_1 != 0)
    pthread_cancel(dbObj->cp.confinfo.conf_timer_thread_1);
  if (dbObj->cp.confinfo.conf_timer_thread_2 != 0)
    pthread_cancel(dbObj->cp.confinfo.conf_timer_thread_2);

  pthread_t thread;
  pthread_create(&thread, NULL, &conf_timerThread, dbObj);

  dbObj->cp.confinfo.conf_timer_thread_1 = thread;
  dbObj->cp.confinfo.conf_timer_thread_2 = thread;
  //pthread_join(thread,&ret);
}
void xGatePBXBusiness::DialTimeout(xGateDBObj* dbObj)
{
  CLog::Detail(THISMODULE, "Inside xGatePBXBusiness::DialTimeout");
  pthread_t thread;
  void *ret;
  pthread_create(&thread, NULL, &DialTimeoutThread, dbObj);
  //pthread_join(thread,&ret);
}
void xGatePBXBusiness::START_CONFERENCE_BRIDGE(xGateDBObj* dbObj)
{
  CLog::Detail(THISMODULE, "xGatePBXBusiness::START_CONFERENCE_BRIDGE() ");
  if (strlen(dbObj->cp.confinfo.DigitReceived.c_str()) <= 0)
  {
    dbObj->cp.csfilename.assign("/root/wav_files/sounds/vectone/CR_welcome_prompt.wav");
    dbObj->cp.confinfo.PlayFile.assign("/root/wav_files/sounds/vectone/CR_welcome_prompt.wav");
    dbObj->dbevent = (XGATEDBEVENT) EN_CONF_EVENT_REL;
  }
}
void xGatePBXBusiness::fetch_switchbaordData_from_DBmanager(xGateDBObj *dbObj, xGateDBManager* gm_db_manager)
{
  CLog::Detail(THISMODULE, "Inside xGatePBXBusiness::fetch_switchbaorddata_from_DBmanager");
  callInfoParameter cp = dbObj->cp;
  db_combined_query_swb(dbObj, gm_db_manager);

  //cp.csfilename.assign(WAV_FILE_PATH);
  //cp.csfilename.append(RBT_FILE_ONE_RING);

  //cp.rbtFile.assign(WAV_FILE_PATH);
  //cp.rbtFile.append(RBT_DEFAULT_FILE);

  SWB_Company_hour_greeting(dbObj);

  xGateDBObj *dbObj_copy = new xGateDBObj();	
  dbObj_copy->cp = cp;

  //dbObj->cp.swbinfo = cp.swbinfo;
  //dbObj->cp.swbinfo.SwbCallEvent = cp.swbinfo.SwbCallEvent;
  //SWITCHBOARD_FEATURE_MAIN_LOOP(dbObj);


  //dbObj->dbevent = (XGATEDBEVENT) EN_SWB_EVENT_REL;

  dbObj->cp.swbinfo.PlayFile.assign(cp.gm_swb_company_greeting_file.c_str());
  dbObj->cp.csfilename.assign(cp.gm_swb_company_greeting_file.c_str());
  //dbObj->cp.gm_user_greeting_file_name_user_B.assign(cp.gm_swb_company_greeting_file.c_str());

  // Play Main-menu IVR
  //dbObj->cp.swbinfo.SwbCallEvent = EN_SWB_PLAY_PASSTHROUGH;
  //gm_db_manager->PostMessage(dbObj,XGATEPOSTTYPE::EN_PROCESS);

  // Then send DTMF-digit request...
  dbObj_copy->cp.swbinfo.PlayFile.assign("");
  dbObj_copy->cp.swbinfo.MaxDigit = 3;
  dbObj_copy->cp.swbinfo.DigitTimeOut = 8;
  dbObj_copy->cp.swbinfo.DigitReceived.assign("");

  dbObj_copy->cp.swbinfo.SwbCallEvent = EN_SWB_GETDTMF;

  //dbObj->dbevent = (XGATEDBEVENT) EN_DIALOUT_REL;
  dbObj->dbevent = (XGATEDBEVENT) EN_DB_PREQUERY_RES;
  //gm_db_manager->PostMessage(dbObj_copy,XGATEPOSTTYPE::EN_PROCESS);

  Swb_MainMenuIvrTimeout(dbObj);
  //set cp.isSwitchBoardCall = false ; when dialout to extension in switchboard-call -to make DBManager send it to receiver
}
void xGatePBXBusiness::fetch_conferenceData_from_DBmanager(xGateDBObj *dbObj, xGateDBManager* gm_db_manager)
{
  CLog::Detail(THISMODULE, "Inside xGatePBXBusiness::fetch_conferenceData_from_DBmanager ; ConfCallEvent = %d", dbObj->cp.confinfo.ConfCallEvent);

  dbObj->cp.confinfo.ConfCallEvent = EN_CONF_WELCOME;
  dbObj->dbevent = (XGATEDBEVENT) EN_DB_PREQUERY_RES; // First-time initial response to connect conference-call

  dbObj->cp.confinfo.PlayFile.assign(WAV_FILE_PATH);
#ifdef GET_PIN_ONLY
  //dbObj->cp.confinfo.PlayFile.append("CR_enter_conf_pin.wav");
  dbObj->cp.confinfo.PlayFile.append("welcome_conf.wav");
#else
  dbObj->cp.confinfo.PlayFile.append("CR_welcome_prompt.wav");
#endif
  // set DTMF-digit collection parameters
  dbObj->cp.confinfo.MaxDigit = 5;
  dbObj->cp.confinfo.DigitTimeOut = 12;

	dbObj->cp.confinfo.conf_timer_thread_1 = 0; // this will fix crash occurred while calling from URAPP
	dbObj->cp.confinfo.conf_timer_thread_2 = 0; // this will fix crash occurred while calling from URAPP
       	Conf_DTMFDigitTimeout(dbObj); // start timer-thread to monitor DTMF-digit collection Timeout...

	dbObj->cp.gm_mgc_ip = gm_db_manager->DATABASE_QUERY_CONF_REGISTER_MG(dbObj->cp.gm_B_domain_id.c_str(), dbObj->cp.call_id.c_str(),dbObj->cp.gm_mgc_ip.c_str(), xGateUtil::getLocalIpAddr(), "");
}

void xGatePBXBusiness::process_switchbaordMainMenuIVRTimeOut(xGateDBObj *dbObj, xGateDBManager* gm_db_manager)
{
  CLog::Detail(THISMODULE, "Inside xGatePBXBusiness::process_switchbaordMainMenuIVRTimeOut");
  callInfoParameter cp = dbObj->cp;

  if ((strlen(dbObj->cp.swbinfo.DigitReceived.c_str()) > 0) && (!dbObj->cp.gm_userDiallDTMFdigits))
  {
    dbObj->cp.gm_userDiallDTMFdigits = true;
    CLog::Detail(THISMODULE, "xGatePBXBusiness::process_switchbaordMainMenuIVRTimeOut : EN_SWB_EVENT_REQ ; DigitReceived = %s", dbObj->cp.swbinfo.DigitReceived.c_str());
    if ((!strcmp(dbObj->cp.swbinfo.DigitReceived.c_str(),"0")) || (!(strcmp(dbObj->cp.swbinfo.DigitReceived.c_str(),"$$$"))) || (!(strcmp(dbObj->cp.swbinfo.DigitReceived.c_str(),SWB_MAINMENUIVR_TIMEOUT_WITH_NO_DTMF_DIGITS))))
    {
      if ((!(strcmp(dbObj->cp.swbinfo.DigitReceived.c_str(),SWB_MAINMENUIVR_TIMEOUT_WITH_NO_DTMF_DIGITS))) && (dbObj->cp.gm_swb_auto_routing == 1))
        dbObj->cp.swbinfo.DigitReceived.assign("0");

      // following line added for testing, to be removed later...
      dbObj->cp.swbinfo.DigitReceived.assign("0");

      //PostMessageToDBManager(dbObj, EN_DB_OPERATOREXTN_REQ);
      if (strlen(dbObj->cp.gm_swb_operator_extension.c_str()) <= 0)
        dbObj->cp.cnCalled.assign("200");
      else
        dbObj->cp.cnCalled.assign(dbObj->cp.gm_swb_operator_extension.c_str());

      CLog::Detail(THISMODULE, "xGatePBXBusiness::process_switchbaordMainMenuIVRTimeOut : EN_SWB_EVENT_REQ ; Operator-extension = %s", cp.cnCalled.c_str());

      if (!(strcmp(dbObj->cp.swbinfo.DigitReceived.c_str(),"0")))
      {
        //if (strlen(dbObj->cp.gm_swb_operator_extension.c_str()) <= 0)
        dbObj->cp.swbinfo.DigitReceived.assign("200");
        dbObj->cp.cnCalled.assign("200");
        //else
        //dbObj->cp.swbinfo.DigitReceived.assign(dbObj->cp.gm_swb_operator_extension.c_str());

        dbObj->cp.gm_userDiallDTMFdigits = true;
        // Paly IVR "Connecting to operator"
        dbObj->cp.swbinfo.PlayFile.assign(WAV_FILE_PATH);
        dbObj->cp.swbinfo.PlayFile.append(CONNECT_TO_OPERATOR_FILE);
        dbObj->cp.swbinfo.SwbCallEvent = EN_SWB_PLAY;

        //dbObj->cp.cnCalled.assign(dbObj->cp.swbinfo.DigitReceived.c_str());

        dbObj->dbevent = (XGATEDBEVENT) EN_SWB_EVENT_REL;
        //gm_db_manager->PostMessage(dbObj,XGATEPOSTTYPE::EN_PROCESS);
        return;
      }
    }
    else
    {
      cp.cnCalled.assign(dbObj->cp.swbinfo.DigitReceived.c_str());
      dbObj->cp.cnCalled.assign(dbObj->cp.swbinfo.DigitReceived.c_str());
    }

    //cp.csfilename.assign(WAV_FILE_PATH);
    //cp.csfilename.append(RBT_DEFAULT_FILE);

    // Inform SofiaSiphandler to connect to SwitchBoard-call
    //dbObj->dbevent = EN_DIALOUT_REL;
    //PostMessageToDBManager(dbObj, EN_DB_DOMAINID_REQ);
    //PostMessageToDBManager(dbObj, EN_DB_COMBINEDSPSELECTOR_REQ);

    //cp.swbinfo.SwbCallEvent = EN_SWB_DIAL;
    //dbObj->dbevent = EN_SWB_EVENT_REL;

    //cp.routeAddr.assign("6398.UR.mundio.com");
    dbObj->cp.gm_user_B_DOMAIN_NAME.assign(cp.routeAddr.c_str());
    dbObj->cp.gm_user_A_DOMAIN_NAME.assign(cp.routeAddr.c_str());

    dbObj->cp.gm_totalNoOfRegisteredUsers = 1;
    //cp.cnCalling.assign("201");

    //setUserInfo(cp,cp,dbObj);
    //PostMessageToDBManager(dbObj, GET_EXTENSION_DATA_REQ);
    //PostMessageToDBManager(dbObj, EN_DB_DOMAINID_REQ);
    //PostMessageToDBManager(dbObj, EN_DB_DOMAINID_REQ);
    //dbObj->dbevent = EN_DIALOUT_REL;

    //cp.isSwitchBoardCall = false; // To make sure DBManager DIALOUT to the extension...
    return;
  }
  if ((dbObj->dbevent == EN_SWBMAINMENUIVR_TIMEOUT_RES) && (!dbObj->cp.gm_userDiallDTMFdigits) && (dbObj->cp.gm_swb_auto_routing != 1))
  {
    // Timeout occurred, and user not dialled anything and NO auto-routing also. Disconnect this call.
    /*
       if (!(strcmp(dbObj->cp.swbinfo.DigitReceived.c_str(),"0")))
       {
       dbObj->cp.gm_userDiallDTMFdigits = true;
       cp.swbinfo.PlayFile.assign(WAV_FILE_PATH);
       cp.swbinfo.PlayFile.append(NO_INPUT_GOODBYE_FILE);
       dbObj->cp.swbinfo.SwbCallEvent = EN_SWB_PLAY;
       dbObj->dbevent = EN_SWB_EVENT_REL;
       break;
       }
     ****/
  }
  //SWB_Company_hour_greeting(dbObj);
  //SWITCHBOARD_FEATURE_MAIN_LOOP(dbObj);
  //dbObj->dbevent = (XGATEDBEVENT) EN_SWB_EVENT_REL;
}
void xGatePBXBusiness::process_switchbaordEventRequest(xGateDBObj *dbObj, xGateDBManager* gm_db_manager)
{
  CLog::Detail(THISMODULE, "Inside xGatePBXBusiness::process_switchbaordEventRequest");
  callInfoParameter cp = dbObj->cp;

  dbObj->cp.cnCalled.assign(cp.swbinfo.DigitReceived.c_str());
  //cp.isSwitchBoardCall = false; // To make sure DBManager DIALOUT to the extension...
  //dbObj->dbevent = (XGATEDBEVENT) EN_DB_PREQUERY_RES;
  fetch_featureListParams_from_DBmanager(dbObj, gm_db_manager);
  dbObj->dbevent = (XGATEDBEVENT) EN_DIALOUT_REL;
  dbObj->cp = cp;
}
void xGatePBXBusiness::GetConfId(xGateDBObj *dbObj, xGateDBManager* gm_db_manager, int maxDtmfDigits)
{
  CLog::Detail(THISMODULE, "Inside xGatePBXBusiness::GetConfId");
  dbObj->dbevent = (XGATEDBEVENT) EN_CONF_EVENT_REL; // Response or NEXT conf-event = EN_CONF_EVENT_REL as default common event.

  switch (dbObj->cp.confinfo.ConfDtmfState)
  {
    case EN_CONF_WELCOMECONNECT: // ConfDtmfState EN_CONF_WELCOMECONNECT is required.
    case EN_CONF_DTMFSTART:
      dbObj->cp.confinfo.ConfDtmfState = EN_CONF_VALIDATE;

      //db_combined_query_conf_Id(dbObj, gm_db_manager); // Combined conf quesry ID

#ifndef GET_PIN_ONLY // to be invoked only for Conf-id...
      if (!dbObj->cp.confinfo.gm_valid_conf_id)
      {
        dbObj->cp.confinfo.ConfDtmfState = EN_CONF_INVALID;
        dbObj->cp.confinfo.ConfCallEvent = EN_CONF_PLAY_PASSTHROUGH_GETDTMF;

        dbObj->cp.confinfo.DigitReceived.assign("");
        dbObj->cp.gm_userDiallDTMFdigits = false;
        Conf_DTMFDigitTimeout(dbObj); // start timer-thread to monitor DTMF-digit collection Timeout...
        break;
      }
#endif;
      // check conf-status...
      //db_combined_query_conf_info(dbObj, gm_db_manager); // Combined conf quesry INFO

      // follwoing code is added for testing ; to be removed later...
     // dbObj->cp.confinfo.gm_conference_status.assign("Active");
//      dbObj->cp.confinfo.gm_conf_status_flag = 1;

  /*    if ((strcmp( dbObj->cp.confinfo.gm_conference_status.c_str(), "active")) || ( dbObj->cp.confinfo.gm_conf_status_flag != 1))
      { // not Active conference; play following 2 files and Disconnect...
        dbObj->cp.confinfo.ConfDtmfModule = EN_CONF_CHECKCONFSTATUS;
        // following 2 parameters will play required 2 files for EN_CONF_CHECKCONFSTATUS...
        dbObj->cp.confinfo.ConfCallEvent = EN_CONF_PLAY_PASSTHROUGH_GETDTMF;
        dbObj->cp.confinfo.gm_conf_NoOfValidDialAttempts_by_user = 1; // initialize to 1; it gets decremented to 0 in EN_CONF_PLAY_PASSTHROUGH_GETDTMF
        break;
      }*/
#ifdef GET_PIN_ONLY
      CLog::Detail(THISMODULE, "xGatePBXBusiness::PIN == %s", dbObj->cp.confinfo.DigitReceived.c_str());
#else
      CLog::Detail(THISMODULE, "xGatePBXBusiness::CONF-ID == %s", dbObj->cp.confinfo.DigitReceived.c_str());
#endif;
      dbObj->cp.confinfo.gm_conf_id.assign(dbObj->cp.confinfo.DigitReceived.c_str());

#ifdef GET_PIN_ONLY
      dbObj->cp.confinfo.ConfDtmfState = EN_CONF_DTMFSTART;
#else
      dbObj->cp.confinfo.ConfDtmfState = EN_CONF_WELCOMECONNECT;
#endif;
      GetPin(dbObj, gm_db_manager, 4); //call next module to get PIN; collect 4 digits of PIN

      break;

    case EN_CONF_NO_DTMF:
      dbObj->cp.confinfo.ConfCallEvent = EN_CONF_PLAY_PASSTHROUGH_GETDTMF;
      dbObj->cp.confinfo.DigitReceived.assign("");;
      dbObj->cp.gm_userDiallDTMFdigits = false;
      Conf_DTMFDigitTimeout(dbObj); // start timer-thread to monitor DTMF-digit collection Timeout...
      break;
  }
}
void xGatePBXBusiness::GetPin(xGateDBObj *dbObj, xGateDBManager* gm_db_manager, int maxDtmfDigits)
{
  CLog::Detail(THISMODULE, "Inside xGatePBXBusiness::GetPin");
  dbObj->dbevent = (XGATEDBEVENT) EN_CONF_EVENT_REL; // Response or NEXT conf-event = EN_CONF_EVENT_REL as default common event.

  switch (dbObj->cp.confinfo.ConfDtmfState)
  {
    case EN_CONF_WELCOMECONNECT:
      dbObj->cp.confinfo.ConfDtmfModule = EN_CONF_GETPIN;
      dbObj->cp.confinfo.ConfDtmfState = EN_CONF_DTMFSTART;
      dbObj->cp.confinfo.ConfCallEvent = EN_CONF_PLAY_PASSTHROUGH_GETDTMF;
      dbObj->cp.confinfo.MaxDigit = maxDtmfDigits; // PIN is 4 digit
      dbObj->cp.confinfo.DigitTimeOut = 14; // 14 is equivalent to 10 seconds
      dbObj->cp.confinfo.gm_conf_NoOfValidDialAttempts_by_user = 7; // 7 is equivalent to 3 attempts.
      dbObj->cp.confinfo.DigitReceived.assign("");;
      dbObj->cp.gm_userDiallDTMFdigits = false;
      Conf_DTMFDigitTimeout(dbObj); // start timer-thread to monitor DTMF-digit collection Timeout...
      break;

    case EN_CONF_DTMFSTART:
      dbObj->cp.confinfo.ConfDtmfState = EN_CONF_VALIDATE;

      // code below added for testing only; to be removed later...
      if ((!(strcmp(dbObj->cp.confinfo.DigitReceived.c_str(), dbObj->cp.confinfo.gm_PIN_from_database.c_str()))) ||
          (!(strcmp(dbObj->cp.confinfo.DigitReceived.c_str(), dbObj->cp.confinfo.gm_PARTICIPANT_PIN_from_database.c_str()))))
      {
        dbObj->cp.confinfo.gm_conf_valid_pin = true;
        dbObj->cp.confinfo.gm_conf_leg_isactive = true;
        CLog::Detail(THISMODULE, "xGatePBXBusiness::PIN == %s", dbObj->cp.confinfo.DigitReceived.c_str());
        dbObj->cp.confinfo.gm_PIN_from_user.assign(dbObj->cp.confinfo.DigitReceived.c_str());
      }

      // check whther user dialled another user's participant-code...
      unsigned int conf_user_type;
      char ext_no[15];
      //conf_user_type = gm_db_manager->DATABASE_QUERY_CONF_VALIDATE_PIN(dbObj->cp.gm_B_domain_id.c_str(),dbObj->cp.confinfo.DigitReceived.c_str());
      db_combined_query_conf_validate_pin(dbObj, gm_db_manager);

      if ((dbObj->cp.confinfo.gm_conf_user_type == 2) || (dbObj->cp.confinfo.gm_conf_user_type == 1))
      {
        dbObj->cp.confinfo.gm_conf_valid_pin = true;
        dbObj->cp.confinfo.gm_conf_leg_isactive = true;
        CLog::Detail(THISMODULE, "xGatePBXBusiness::PIN == %s", dbObj->cp.confinfo.DigitReceived.c_str());

        db_combined_query_conf_get_callid_mguid(dbObj, gm_db_manager);
      }
      else
        dbObj->cp.confinfo.gm_conf_valid_pin = false;

      /*
         if (dbObj->cp.confinfo.gm_conf_user_type == 1)
         {       // dialled as a host-PIN. Whether any party has already joined in conf-call as participant???
         dbObj->cp.confinfo.gm_mgResourceId = gm_db_manager->DATABASE_QUERY_GET_CONFERENCE_MGRESOURCEID(dbObj->cp.confinfo.gm_PARTICIPANT_PIN_from_database.c_str());
         dbObj->cp.confinfo.gm_conf_id.assign(dbObj->cp.confinfo.gm_PARTICIPANT_PIN_from_database.c_str()); // This is required as below DATABASE_QUERY_GET_CONFERENCE_MGRESOURCEID() checks on gm_conf_id
         }

         if (strlen(dbObj->cp.confinfo.gm_mgResourceId.c_str()) <= 0)
         {       // NO party has already joined in conf-call as participant. so try this now..
         dbObj->cp.confinfo.gm_mgResourceId = gm_db_manager->DATABASE_QUERY_GET_CONFERENCE_MGRESOURCEID(dbObj->cp.confinfo.DigitReceived.c_str());
         }
         */

      /*
         if (strlen(dbObj->cp.confinfo.gm_mgResourceId.c_str()) > 0)
         {
         CLog::Detail(THISMODULE, "xGatePBXBusiness::PIN == %s", dbObj->cp.confinfo.DigitReceived.c_str());
         CLog::Detail(THISMODULE, "xGatePBXBusiness::gm_mgResourceId == %s", dbObj->cp.confinfo.gm_mgResourceId.c_str());
         dbObj->cp.confinfo.gm_conf_valid_pin = true;
         }
         */

      //if ((!dbObj->cp.confinfo.gm_conf_valid_pin) && (!dbObj->cp.confinfo.gm_is_host_user_already_in_confCall))

      //if (!(strcmp(dbObj->cp.confinfo.gm_PIN_from_database.c_str(), dbObj->cp.confinfo.gm_PIN_from_user.c_str())))
      if (dbObj->cp.confinfo.gm_conf_user_type == 1)
      {
        dbObj->cp.confinfo.gm_conf_host_user = true;
        //dbObj->cp.confinfo.gm_is_host_user_already_in_confCall = true;
        //if (GetHostInfo(conf_id) = 1)
      }else{
        dbObj->cp.confinfo.gm_conf_host_user =  false;
      }
      if ((!dbObj->cp.confinfo.gm_conf_valid_pin) || ((dbObj->cp.confinfo.gm_conf_host_user) && (dbObj->cp.confinfo.gm_is_host_user_already_in_confCall) && (!dbObj->cp.confinfo.gm_mgResourceId.empty())))
        //if (!dbObj->cp.confinfo.gm_conf_valid_pin)
      {
        dbObj->cp.confinfo.ConfDtmfState = EN_CONF_INVALID;
        dbObj->cp.confinfo.ConfCallEvent = EN_CONF_PLAY_PASSTHROUGH_GETDTMF;

        dbObj->cp.confinfo.gm_is_host_user_already_in_confCall = false;
        if (strcmp(dbObj->cp.confinfo.gm_mgResourceId.c_str(), dbObj->cp.call_id.c_str()))
          dbObj->cp.confinfo.gm_mgResourceId.assign(dbObj->cp.call_id.c_str());

        dbObj->cp.confinfo.DigitReceived.assign("");
        dbObj->cp.gm_userDiallDTMFdigits = false;
	CLog::Detail(THISMODULE,"invalid pin & posting DB Message");
        Conf_DTMFDigitTimeout(dbObj); // start timer-thread to monitor DTMF-digit collection Timeout...
        break;
      }



      /*		if (!(strcmp(dbObj->cp.confinfo.gm_PARTICIPANT_PIN_from_database.c_str(), dbObj->cp.confinfo.gm_PIN_from_user.c_str())))
            {
            dbObj->cp.confinfo.gm_conf_participant_user = true;
      // if ((lock_flag ) and (conf_participant_user ))
      }*/

      //if (dbObj->cp.confinfo.noOfConfParticipantsCurrentlyJoined == 0)
      //	gm_db_manager->DATABASE_QUERY_CONF_DELETE_DB_RECORDS(dbObj->cp.confinfo.gm_conf_id.c_str());

      if((!dbObj->cp.confinfo.isConfLocked) && (dbObj->cp.confinfo.noOfConfParticipantsCurrentlyJoined < CONF_MAX_PARTICIPANTS))
      {
        CLog::Detail(THISMODULE, "xGatePBXBusiness::uid = %s", dbObj->cp.uid.c_str());

        //std::string mgResourceId;
        //mgResourceId.assign("");
        // gm_conf_id is the key -that needs to pass while querying the database... (and Not uid)
        //if (dbObj->cp.confinfo.noOfConfParticipantsCurrentlyJoined <= 0)
        //if ((dbObj->cp.g_noOfConfParticipantsCurrentlyJoined <= 0) && (!strcmp(dbObj->cp.confinfo.gm_mgResourceId.c_str(), dbObj->cp.uid.c_str())))
        //mgResourceId = gm_db_manager->DATABASE_QUERY_GET_CONFERENCE_MGRESOURCEID(dbObj->cp.confinfo.gm_conf_id.c_str());

        //db_combined_query_conf_get_callid_mguid(dbObj, gm_db_manager);
        //mgResourceId = gm_db_manager->DATABASE_QUERY_GET_CONFERENCE_MGRESOURCEID(dbObj->cp.confinfo.gm_conf_host_extension.c_str(), dbObj->cp.gm_user_B_DOMAIN_NAME.c_str());
        //if ((dbObj->cp.confinfo.gm_mgResourceId.empty()) || (dbObj->cp.confinfo.gm_call_id.empty()))
        if (dbObj->cp.confinfo.gm_mgResourceId.empty())
        {
          dbObj->cp.confinfo.gm_mgResourceId.assign(dbObj->cp.uid.c_str());
          dbObj->cp.confinfo.noOfConfParticipantsCurrentlyJoined = 0;
        }
        {
          //	dbObj->cp.confinfo.gm_call_id.assign(dbObj->cp.call_id.c_str());
          // this is first participanti to join and create the conference-room...
#ifndef GET_PIN_ONLY
          gm_db_manager->DATABASE_QUERY_CONF_DELETE_DB_RECORDS(dbObj->cp.confinfo.gm_conf_id.c_str());
          gm_db_manager->DATABASE_QUERY_REGISTER_CONFERENCE_MGRESOURCEID(dbObj->cp.confinfo.gm_conf_id.c_str(), dbObj->cp.uid.c_str(), "Active", "10.30.3.97","10.22.3.93","28_January",1);
#else
          //gm_db_manager->DATABASE_QUERY_CONF_DELETE_DB_RECORDS(dbObj->cp.confinfo.gm_PIN_from_database.c_str());
          //gm_db_manager->DATABASE_QUERY_CONF_DELETE_DB_RECORDS(dbObj->cp.confinfo.gm_PARTICIPANT_PIN_from_database.c_str());
          //gm_db_manager->DATABASE_QUERY_CONF_DELETE_DB_RECORDS(dbObj->cp.confinfo.gm_conf_id.c_str());
          //gm_db_manager->DATABASE_QUERY_CONF_DELETE_DB_RECORDS(dbObj->cp.confinfo.gm_conf_host_extension.c_str());
          //gm_db_manager->DATABASE_QUERY_REGISTER_CONFERENCE_MGRESOURCEID(dbObj->cp.confinfo.gm_PIN_from_database.c_str(), dbObj->cp.uid.c_str(), "Active", "10.30.3.97","10.22.3.93","28_January",1);
          //gm_db_manager->DATABASE_QUERY_REGISTER_CONFERENCE_MGRESOURCEID(dbObj->cp.confinfo.gm_PARTICIPANT_PIN_from_database.c_str(), dbObj->cp.uid.c_str(), "Active", "10.30.3.97","10.22.3.93","28_January",1);
          //gm_db_manager->DATABASE_QUERY_REGISTER_CONFERENCE_MGRESOURCEID(dbObj->cp.confinfo.gm_conf_id.c_str(), dbObj->cp.uid.c_str(), "Active", "10.30.3.97","10.22.3.93","28_January",1);
          time_t now = time(0);
          char* current_time = ctime(&now);
          gm_db_manager->DATABASE_QUERY_INSERT_CONFERENCE_MGRESOURCEID(dbObj->cp.confinfo.gm_conf_host_extension.c_str(), dbObj->cp.call_id.c_str(), "inactive", dbObj->cp.gm_mgc_ip.c_str(),dbObj->cp.gm_my_ip_address.c_str(),current_time,dbObj->cp.confinfo.gm_conf_host_user, dbObj->cp.gm_user_B_DOMAIN_NAME.c_str(), dbObj->cp.confinfo.gm_mgResourceId.c_str());
#endif
        }
        /*
           else
           {
        //dbObj->cp.confinfo.gm_mgResourceId = gm_db_manager->DATABASE_QUERY_GET_CONFERENCE_MGRESOURCEID(dbObj->cp.confinfo.gm_conf_host_extension.c_str());
        //dbObj->cp.confinfo.gm_mgResourceId = mgResourceId;
        }
        */
        CLog::Detail(THISMODULE, "xGatePBXBusiness::gm_mgResourceId = %s", dbObj->cp.confinfo.gm_mgResourceId.c_str());

        dbObj->cp.confinfo.ConfCallEvent = EN_CONF_ADDPARTICIPANT;
      }

      break;

    case EN_CONF_GETDTMF_COMPLET:
      if (dbObj->cp.confinfo.ConfDtmfState == EN_CONF_GETHOSTCONTROLDIGIT)
      {
        if (!(strcmp(dbObj->cp.confinfo.DigitReceived.c_str(), "0")))
        {
        }
        if (!(strcmp(dbObj->cp.confinfo.DigitReceived.c_str(), "1")))
        {
        }
        if (!(strcmp(dbObj->cp.confinfo.DigitReceived.c_str(), "2")))
        {
        }
        if (!(strcmp(dbObj->cp.confinfo.DigitReceived.c_str(), "3")))
        {
        }
        if (!(strcmp(dbObj->cp.confinfo.DigitReceived.c_str(), "4")))
        {
        }
        if (!(strcmp(dbObj->cp.confinfo.DigitReceived.c_str(), "5")))
        {
        }
        if (!(strcmp(dbObj->cp.confinfo.DigitReceived.c_str(), "6")))
        {
        }
        if (!(strcmp(dbObj->cp.confinfo.DigitReceived.c_str(), "7")))
        {
        }
        if (!(strcmp(dbObj->cp.confinfo.DigitReceived.c_str(), "8")))
        {
        }
        if (!(strcmp(dbObj->cp.confinfo.DigitReceived.c_str(), "9")))
        {
        }
        if (!(strcmp(dbObj->cp.confinfo.DigitReceived.c_str(), "11")))
        {
        }
        if (!(strcmp(dbObj->cp.confinfo.DigitReceived.c_str(), "12")))
        {
        }
      }
      break;
  }
}

void xGatePBXBusiness::GetRegistrationDataFromDB(int en_db_combinedspregistar_res, xGateDBObj* dbObj, xGateDBManager* gm_db_manager)
{
  CLog::Detail(THISMODULE, "Inside xGatePBXBusiness::GetRegistrationDataFromDB ; gm_totalNoOfRegisteredUsers = %d", dbObj->cp.gm_totalNoOfRegisteredUsers);

  std::vector<DBQueryResultData>* combine_sp_output_Registration;
  if (en_db_combinedspregistar_res == EN_DB_COMBINEDSPREGISTAR_RES_1)
    combine_sp_output_Registration = gm_db_manager->DATABASE_QUERY_combined_SendAppNotification(dbObj->cp.cnCalled.c_str(),  dbObj->cp.gm_user_B_DOMAIN_NAME.c_str(), 4, EN_DB_COMBINEDSPREGISTAR_RES_1);
  else
    combine_sp_output_Registration = gm_db_manager->DATABASE_QUERY_combined_SendAppNotification(dbObj->cp.cnCalled.c_str(),  dbObj->cp.gm_user_B_DOMAIN_NAME.c_str(), 4, EN_DB_COMBINEDSPREGISTAR_RES_2);

  int max_columns_in_records = TOTAL_NOOFCOLUMNS_DB_COMBINEDSP_REGISTRAR - 1;
  int index = 1;

  if (!dbObj->cp.ptr_xGateRegistrationVector)
  {
    std::vector<xGateRegistrationRecord>* ptrRegistrationVector = new vector<xGateRegistrationRecord>;
    dbObj->cp.ptr_xGateRegistrationVector = ptrRegistrationVector;
  }
  xGateRegistrationRecord* temp_registration = new xGateRegistrationRecord();

  for (auto it = (*combine_sp_output_Registration).begin(), index = 1; (index <= (dbObj->cp.gm_totalNoOfRegisteredUsers * max_columns_in_records)) && (it < (*combine_sp_output_Registration).end()); index++)
  {
    switch (index)
    {
      // STRING data-types
      case COLUMN_1: // Contact_address
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::GetRegistrationDataFromDB : combine_sp_output_Registration[%d] = %s", index, (*(it->data.string_data)).c_str());
          temp_registration->Contact_address.assign((*it->data.string_data).c_str());
        }
        else
          temp_registration->Contact_address.assign(CONTACT_ADDRESS_DEFAULT);
        ++it;
        break;

      case COLUMN_2: // proxy_username
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::GetRegistrationDataFromDB : combine_sp_output_Registration[%d] = %s", index, (*(it->data.string_data)).c_str());
          temp_registration->proxy_username.assign((*it->data.string_data).c_str());
        }
        ++it;
        break;

      case COLUMN_4: // device_type
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::GetRegistrationDataFromDB : combine_sp_output_Registration[%d] = %s", index, (*(it->data.string_data)).c_str());
          temp_registration->device_type.assign((*it->data.string_data).c_str());
        }
        ++it;
        break;

      case COLUMN_5: // previous_proxy_username
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::GetRegistrationDataFromDB : combine_sp_output_Registration[%d] = %s", index, (*(it->data.string_data)).c_str());
          temp_registration->prev_proxy_username.assign((*it->data.string_data).c_str());
        }
        ++it;
        break;

        // INTEGER data-types
      case COLUMN_3: // status
        CLog::Detail(THISMODULE, "xGatePBXBusiness::GetRegistrationDataFromDB : combine_sp_output_Registration[%d] = %d", index, it->data.integer_data);
        temp_registration->appStatus = it->data.integer_data;
        ++it;
        break;
      default:
        CLog::Detail(THISMODULE, "xGatePBXBusiness::GetRegistrationDataFromDB : combine_sp_output_Registration == DEFAULT");
        break;
    }
    if ((index % max_columns_in_records) == 0) // repeats for every record...
    {
      index -= max_columns_in_records;
      (dbObj->cp.ptr_xGateRegistrationVector)->push_back(*temp_registration);
    }
  }

  /*	
      for (auto it = (dbObj->cp.ptr_xGateRegistrationVector)->begin(); (it < (dbObj->cp.ptr_xGateRegistrationVector)->end()); ++it)
      {
      CLog::Detail(THISMODULE, "xGatePBXBusiness::GetRegistrationDataFromDB : record-data...");
      CLog::Detail(THISMODULE, "xGatePBXBusiness::GetRegistrationDataFromDB : %s",it->Contact_address.c_str());
      CLog::Detail(THISMODULE, "xGatePBXBusiness::GetRegistrationDataFromDB : %s",it->proxy_username.c_str());
      CLog::Detail(THISMODULE, "xGatePBXBusiness::GetRegistrationDataFromDB : %d",it->appStatus);
      CLog::Detail(THISMODULE, "xGatePBXBusiness::GetRegistrationDataFromDB : %s",it->device_type.c_str());
      CLog::Detail(THISMODULE, "xGatePBXBusiness::GetRegistrationDataFromDB : %s",it->prev_proxy_username.c_str());
      }
      */
}

bool xGatePBXBusiness::isIOSDevice(xGateDBObj *dbObj, char* temp_calledDeviceType, char* temp_proxyUserName)
{
  // Logic to cancel PushNotification...
  CLog::Detail(THISMODULE, "xGatePBXBusiness::isIOSDevice ");

  if ((strcmp(temp_calledDeviceType, DEVICE_TYPE_URAPP_IOS)))
    return true;

  std::string save_IOSproxyUserName;

  std::string mappedUserKey;
  mappedUserKey.assign(dbObj->cp.cnCalled.c_str());
  mappedUserKey.append("_");
  mappedUserKey.append(dbObj->cp.gm_B_domain_id.c_str());
  mappedUserKey.append("_");
  //mappedUserKey.append(temp_calledDeviceType.c_str());
  mappedUserKey.append(temp_calledDeviceType);

  std::map<std::string,  multiDevice*>::iterator it;
  it = dbObj->cp.m_mappedUserList.find(mappedUserKey);
  if (it != dbObj->cp.m_mappedUserList.end()) 
  {
    if (!strcmp(it->second->m_userDeviceType.c_str(),DEVICE_TYPE_URAPP_IOS))
      save_IOSproxyUserName =	it->second->m_proxyUserName;
  }

  CLog::Detail(THISMODULE, "xGatePBXBusiness::isIOSDevice ; temp_proxyUserName = %s ; save_IOSproxyUserName = %s", temp_proxyUserName, save_IOSproxyUserName.c_str());
  std::string push_from;
  push_from.assign(dbObj->cp.gm_B_domain_id.c_str());
  push_from.append("_");
  push_from.append(dbObj->cp.cnCalling.c_str());

  std::string push_to;
  push_to.assign(dbObj->cp.gm_B_domain_id.c_str());
  push_to.append("_");
  push_to.append(dbObj->cp.cnCalled.c_str());
  if (!(strcmp(temp_proxyUserName, save_IOSproxyUserName.c_str()))) // cookies are Same.
  {
    CancelAppNotificationIOS(push_from.c_str(), push_to.c_str(), "", temp_calledDeviceType, "audio", dbObj->cp.uid.c_str());
    // Remove the entry for the IOS-device from the mapped-userlist
    dbObj->cp.m_mappedUserList.erase(it);
    return false;
  }
  return true;
}
void xGatePBXBusiness::fetch_HuntGroupData_from_DBmanager(xGateDBObj *dbObj, xGateDBManager* gm_db_manager)
{
  CLog::Detail(THISMODULE, "Inside xGatePBXBusiness::fetch_HuntGroupData_from_DBmanager");

  dbObj->cp.isForkingenabled = 1;

  dbObj->cp.gm_totalNoOfRegisteredUsers = 2;
  dbObj->cp.gm_totalNoOfHuntGroupMembers = 0; // added for testing... to be removed later...
}

void xGatePBXBusiness::processRegisteredUsersData(int en_db_combinedspregistar_res, xGateDBObj* dbObj, xGateDBManager* gm_db_manager)
{
  CLog::Detail(THISMODULE, "Inside xGatePBXBusiness::processRegisteredUsersData ; gm_totalNoOfRegisteredUsers = %d", dbObj->cp.gm_totalNoOfRegisteredUsers);

  GetRegisteredUsersDataFromDB(en_db_combinedspregistar_res, dbObj, gm_db_manager);

  if (en_db_combinedspregistar_res == EN_DB_COMBINEDSPREGISTAR_RES_1)
    sendAppNotificationToURAPPDevices(dbObj); // send Push-Notification to URAPP devices...

  //During second retrieval of RegistrationData, check first change in cookie for IOS device...before updating MappedUserList 
  if (en_db_combinedspregistar_res == EN_DB_COMBINEDSPREGISTAR_RES_2)
    checkForCookieChangeInURApp(dbObj);//check before updating MappedUserList 2nd-time 

  // update MappedUserList... during both retrieval of RegistrationData... during both retrieval of RegistrationData...
  updateMappedUserList(dbObj); // saves cookie for IOS -during first iteration of retrieving RegistrationDataFromDB.

  if (en_db_combinedspregistar_res == EN_DB_COMBINEDSPREGISTAR_RES_2)
    CheckCookieAndDeleteDeviceAndCancelAppNotification(dbObj); // Send Canvcel Notification and delete IOS device from mappedUaerList...
}
void xGatePBXBusiness::sendAppNotificationToURAPPDevices(xGateDBObj* dbObj)
{
  CLog::Detail(THISMODULE, "xGatePBXBusiness::sendAppNotificationToURAPPDevices ");
  for (auto it = (dbObj->cp.ptr_xGateRegistrationVector)->begin(); (it < (dbObj->cp.ptr_xGateRegistrationVector)->end()); ++it)
  {
    if ((!(strcmp(it->device_type.c_str(), DEVICE_TYPE_URAPP_ANDROID))) || \
        (!(strcmp(it->device_type.c_str(), DEVICE_TYPE_URAPP_IOS))))
    {
      std::string push_from;
      push_from.assign(dbObj->cp.gm_B_domain_id.c_str());
      push_from.append("_");
      push_from.append(dbObj->cp.cnCalling.c_str());

      std::string push_to;
      push_to.assign(dbObj->cp.gm_B_domain_id.c_str());
      push_to.append("_");
      push_to.append(dbObj->cp.cnCalled.c_str());

      SendAppNotification(push_from.c_str(), push_to.c_str(), "", it->device_type.c_str(), "audio", dbObj->cp.uid.c_str());
      it->isSentAppNotification = true;
    }
  }
}
std::string  xGatePBXBusiness::createmappedUserKey(xGateDBObj* dbObj, char* calledDeviceType)
{
  std::string mappedUserKey;
  mappedUserKey.assign(dbObj->cp.cnCalled.c_str());
  mappedUserKey.append("_");
  mappedUserKey.append(dbObj->cp.gm_B_domain_id.c_str());
  mappedUserKey.append("_");
  mappedUserKey.append(calledDeviceType);
  //CLog::Detail(THISMODULE, "xGatePBXBusiness::createmappedUserKey ; mappedUserKey = %s", mappedUserKey.c_str());
  return (mappedUserKey);
}
void xGatePBXBusiness::updateMappedUserList(xGateDBObj* dbObj)
{
  for (auto it = (dbObj->cp.ptr_xGateRegistrationVector)->begin(); (it < (dbObj->cp.ptr_xGateRegistrationVector)->end()); ++it)
  {
    std::string mappedUserKey;
    mappedUserKey = createmappedUserKey(dbObj, it->device_type.c_str());

    SetMappedUserDeviceType(dbObj, mappedUserKey.c_str(), it->device_type.c_str());
    SetMappedUserNumber_ProxyName_SignalAddress_OutInterface(dbObj, mappedUserKey.c_str(), dbObj->cp.cnCalled.c_str(), it->proxy_username.c_str(), it->Contact_address.c_str(), dbObj->cp.gm_port_out.c_str(), it->IpAddress.c_str() );
  }
}
bool xGatePBXBusiness::checkForCookieChangeInURApp(xGateDBObj* dbObj)
{
  CLog::Detail(THISMODULE, "xGatePBXBusiness::checkForCookieChangeInURApp ");
  for (auto it_vector = (dbObj->cp.ptr_xGateRegistrationVector)->begin(); (it_vector < (dbObj->cp.ptr_xGateRegistrationVector)->end()); ++it_vector)
  {
    if ((!(strcmp(it_vector->device_type.c_str(), DEVICE_TYPE_URAPP_IOS))) ||  // check for IOS device
        (!(strcmp(it_vector->device_type.c_str(), DEVICE_TYPE_URAPP_ANDROID)))) // check for ANDROID device
    { // check difference in cookies from mappeduserList and RegisteredUserData...
      std::string mappedUserKey;
      mappedUserKey = createmappedUserKey(dbObj, it_vector->device_type.c_str());

      std::map<std::string,  multiDevice*>::iterator it_mappedUser;
      for (it_mappedUser = dbObj->cp.m_mappedUserList.find(mappedUserKey.c_str()); (it_mappedUser != dbObj->cp.m_mappedUserList.end()) ; ++it_mappedUser)
      {
        if (!(strcmp(it_vector->proxy_username.c_str(), it_mappedUser->second->m_proxyUserName.c_str())))
        {
          it_vector->noChangeInCookie = true;
          //return true; // noChangeInCookieInURAppIOS == true
        }
      }
    }
  }
  return false;
}
void xGatePBXBusiness::CheckCookieAndDeleteDeviceAndCancelAppNotification(xGateDBObj* dbObj)
{
  CLog::Detail(THISMODULE, "xGatePBXBusiness::CheckCookieAndDeleteDeviceAndCancelAppNotification ");

  for (auto it_vector = (dbObj->cp.ptr_xGateRegistrationVector)->begin(); (it_vector < (dbObj->cp.ptr_xGateRegistrationVector)->end()); ++it_vector)
  {
     	if ((it_vector->noChangeInCookie == true) && 
        ((!(strcmp(it_vector->device_type.c_str(), DEVICE_TYPE_URAPP_IOS))) ||   // check for IOS device
         (!(strcmp(it_vector->device_type.c_str(), DEVICE_TYPE_URAPP_ANDROID))))) // check for ANDROID device
    { // check difference in cookies from mappeduserList and RegisteredUserData...
      std::string push_from;
      push_from.assign(dbObj->cp.gm_B_domain_id.c_str());
      push_from.append("_");
      push_from.append(dbObj->cp.cnCalling.c_str());

      std::string push_to;
      push_to.assign(dbObj->cp.gm_B_domain_id.c_str());
      push_to.append("_");
      push_to.append(dbObj->cp.cnCalled.c_str());

      std::string mappedUserKey;
      mappedUserKey = createmappedUserKey(dbObj, it_vector->device_type.c_str());

      //std::map<std::string,  multiDevice*>::iterator it_mappedUser;
      for (auto it_mappedUser = dbObj->cp.m_mappedUserList.find(mappedUserKey.c_str()); (it_mappedUser != dbObj->cp.m_mappedUserList.end()) ; ++it_mappedUser)
      {
        CancelAppNotificationIOS(push_from.c_str(), push_to.c_str(), "", it_mappedUser->second->m_userDeviceType.c_str(), "audio", dbObj->cp.uid.c_str());
        // Remove the entry for the IOS-device from the mapped-userlist
        dbObj->cp.m_mappedUserList.erase(it_mappedUser);
        break; // only one IOS device is deleted!
      }
      it_vector->noChangeInCookie = false;
    }
  }
}

void xGatePBXBusiness::populateOneDeviceData(int en_db_combinedspregistar_res, xGateDBObj* dbObj, xGateDBManager* gm_db_manager)
{
  GetRegisteredUsersDataFromDB(en_db_combinedspregistar_res, dbObj, gm_db_manager);

  for (auto it = (dbObj->cp.ptr_xGateRegistrationVector)->begin(); (it < (dbObj->cp.ptr_xGateRegistrationVector)->end()); ++it)
  {
    // populate Deskphone data...
    //if (strstr(it->device_type.c_str(),"DESK"))
    {
      dbObj->cp.csaCalled.assign(it->Contact_address.c_str());
      dbObj->cp.proxyUserName.assign(it->proxy_username.c_str());
      dbObj->cp.calledDeviceType.assign(it->device_type.c_str());
      dbObj->cp.routeIpAddress.assign(it->IpAddress.c_str());
    }

    // They don't use mappedUserList -if only one device is registered. still better to save data in MappedUserList.
    // If this only device is URAPP (other than URDESK)... better to save data in MappedUserList.
    //if (!strstr(it->device_type.c_str(),"DESK"))
    updateMappedUserList(dbObj);
    if (en_db_combinedspregistar_res == EN_DB_COMBINEDSPREGISTAR_RES_1)
    {
      if ((!(strcmp(it->device_type.c_str(), DEVICE_TYPE_URAPP_ANDROID))) || \
          (!(strcmp(it->device_type.c_str(), DEVICE_TYPE_URAPP_IOS))) || (!(strcmp(it->device_type.c_str(), DEVICE_TYPE_URWEB))))
      {
        dbObj->cp.gm_totalNoOfRegisteredUsers = 2;
        //sendAppNotificationToURAPPDevices(dbObj); // send Push-Notification to URAPP devices...
        //if (it->isSentAppNotification)
        //dbObj->cp.gm_totalNoOfRegisteredUsers = 2;
      }
    }
    break; // adding "break" here because only one device is supposed to be present.
  }
}


void xGatePBXBusiness::populateTransferData( xGateDBObj* dbObj)
{

   if(dbObj->cp.ptr_xGateRegistrationVector->size()> 1){
    dbObj->cp.isForkingenabled = 1;
     }
	 
//  SetRegisterRecordFromDB(dbObj,dbOutput);
  for (auto it = (dbObj->cp.ptr_xGateRegistrationVector)->begin(); (it < (dbObj->cp.ptr_xGateRegistrationVector)->end()); ++it)
  {
    // populate Deskphone data...
    //if (strstr(it->device_type.c_str(),"DESK"))
    {
      dbObj->cp.csaCalled.assign(it->Contact_address.c_str());
      dbObj->cp.proxyUserName.assign(it->proxy_username.c_str());
      dbObj->cp.calledDeviceType.assign(it->device_type.c_str());
    }
    updateMappedUserList(dbObj);
 break; 
 }
}
void xGatePBXBusiness::populateData( xGateDBObj* dbObj)
{
//  SetRegisterRecordFromDB(dbObj,dbOutput);
  for (auto it = (dbObj->cp.ptr_xGateRegistrationVector)->begin(); (it < (dbObj->cp.ptr_xGateRegistrationVector)->end()); ++it)
  {
    // populate Deskphone data...
    //if (strstr(it->device_type.c_str(),"DESK"))
    {
      dbObj->cp.csaCalled.assign(it->Contact_address.c_str());
      dbObj->cp.proxyUserName.assign(it->proxy_username.c_str());
      dbObj->cp.calledDeviceType.assign(it->device_type.c_str());
    }
      if ((!(strcmp(it->device_type.c_str(), DEVICE_TYPE_URAPP_ANDROID))) || \
          (!(strcmp(it->device_type.c_str(), DEVICE_TYPE_URAPP_IOS))))
      {
    dbObj->cp.gm_totalNoOfRegisteredUsers = 2;
     }
 } 
    // They don't use mappedUserList -if only one device is registered. still better to save data in MappedUserList.
    // If this only device is URAPP (other than URDESK)... better to save data in MappedUserList.
    //if (!strstr(it->device_type.c_str(),"DESK"))
    updateMappedUserList(dbObj);
#if 0
    if (en_db_combinedspregistar_res == EN_DB_COMBINEDSPREGISTAR_RES_1)
    {
      if ((!(strcmp(it->device_type.c_str(), DEVICE_TYPE_URAPP_ANDROID))) || \
          (!(strcmp(it->device_type.c_str(), DEVICE_TYPE_URAPP_IOS))))
      {
        dbObj->cp.gm_totalNoOfRegisteredUsers = 2;
        //sendAppNotificationToURAPPDevices(dbObj); // send Push-Notification to URAPP devices...
        //if (it->isSentAppNotification)
        //dbObj->cp.gm_totalNoOfRegisteredUsers = 2;
      }
    }
#endif
  //  break; // adding "break" here because only one device is supposed to be present.
//  }
}

void xGatePBXBusiness::GetRegisteredUsersDataFromDB(int en_db_combinedspregistar_res, xGateDBObj* dbObj, xGateDBManager* gm_db_manager)
{
  CLog::Detail(THISMODULE, "Inside xGatePBXBusiness::GetRegisteredUsersDataFromDB ; gm_totalNoOfRegisteredUsers = %d", dbObj->cp.gm_totalNoOfRegisteredUsers);

  std::vector<DBQueryResultData>* combine_sp_output_Registration;
  if (en_db_combinedspregistar_res == EN_DB_COMBINEDSPREGISTAR_RES_1)
    combine_sp_output_Registration = gm_db_manager->DATABASE_QUERY_combined_SendAppNotification(dbObj->cp.cnCalled.c_str(),  dbObj->cp.gm_user_B_DOMAIN_NAME.c_str(), 4, EN_DB_COMBINEDSPREGISTAR_RES_1);
  else
    combine_sp_output_Registration = gm_db_manager->DATABASE_QUERY_combined_SendAppNotification(dbObj->cp.cnCalled.c_str(),  dbObj->cp.gm_user_B_DOMAIN_NAME.c_str(), 4, EN_DB_COMBINEDSPREGISTAR_RES_2);

  int max_columns_in_records = TOTAL_NOOFCOLUMNS_DB_COMBINEDSP_REGISTRAR - 1;
  int index = 1;

  if (!dbObj->cp.ptr_xGateRegistrationVector)
  {
    std::vector<xGateRegistrationRecord>* ptrRegistrationVector = new vector<xGateRegistrationRecord>;
    dbObj->cp.ptr_xGateRegistrationVector = ptrRegistrationVector;
  }
  xGateRegistrationRecord* temp_registration = new xGateRegistrationRecord();

  for (auto it = (*combine_sp_output_Registration).begin(), index = 1; (index <= (dbObj->cp.gm_totalNoOfRegisteredUsers * max_columns_in_records)) && (it < (*combine_sp_output_Registration).end()); index++)
  {
    switch (index)
    {
      // STRING data-types
      case COLUMN_1: // Contact_address
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::GetRegisteredUsersDataFromDB : combine_sp_output_Registration[%d] = %s", index, (*(it->data.string_data)).c_str());
          temp_registration->Contact_address.assign((*it->data.string_data).c_str());
        }
        else
          temp_registration->Contact_address.assign(CONTACT_ADDRESS_DEFAULT);
        ++it;
        break;

      case COLUMN_2: // proxy_username
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::GetRegisteredUsersDataFromDB : combine_sp_output_Registration[%d] = %s", index, (*(it->data.string_data)).c_str());
          temp_registration->proxy_username.assign((*it->data.string_data).c_str());
        }
        ++it;
        break;

      case COLUMN_4: // device_type
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::GetRegisteredUsersDataFromDB : combine_sp_output_Registration[%d] = %s", index, (*(it->data.string_data)).c_str());
          temp_registration->device_type.assign((*it->data.string_data).c_str());
        }
        ++it;
        break;

      case COLUMN_5: // previous_proxy_username
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::GetRegisteredUsersDataFromDB : combine_sp_output_Registration[%d] = %s", index, (*(it->data.string_data)).c_str());
          temp_registration->prev_proxy_username.assign((*it->data.string_data).c_str());
        }
        ++it;
        break;

      case COLUMN_6: // IpAddress
        if (it->data_type)
        {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::GetRegisteredUsersDataFromDB : combine_sp_output_Registration[%d] = %s", index, (*(it->data.string_data)).c_str());
          temp_registration->IpAddress.assign((*it->data.string_data).c_str());
        }
        ++it;
        break;

        // INTEGER data-types
      case COLUMN_3: // status
        CLog::Detail(THISMODULE, "xGatePBXBusiness::GetRegisteredUsersDataFromDB : combine_sp_output_Registration[%d] = %d", index, it->data.integer_data);
        temp_registration->appStatus = it->data.integer_data;
        ++it;
        break;
      default:
        CLog::Detail(THISMODULE, "xGatePBXBusiness::GetRegisteredUsersDataFromDB : combine_sp_output_Registration == DEFAULT");
        break;
    }
    if ((index % max_columns_in_records) == 0) // repeats for every record...
    {
      index -= max_columns_in_records;
      (dbObj->cp.ptr_xGateRegistrationVector)->push_back(*temp_registration);
    }
  }

  /*	
      for (auto it = (dbObj->cp.ptr_xGateRegistrationVector)->begin(); (it < (dbObj->cp.ptr_xGateRegistrationVector)->end()); ++it)
      {
      CLog::Detail(THISMODULE, "xGatePBXBusiness::GetRegisteredUsersDataFromDB : record-data...");
      CLog::Detail(THISMODULE, "xGatePBXBusiness::GetRegisteredUsersDataFromDB : %s",it->Contact_address.c_str());
      CLog::Detail(THISMODULE, "xGatePBXBusiness::GetRegisteredUsersDataFromDB : %s",it->proxy_username.c_str());
      CLog::Detail(THISMODULE, "xGatePBXBusiness::GetRegisteredUsersDataFromDB : %d",it->appStatus);
      CLog::Detail(THISMODULE, "xGatePBXBusiness::GetRegisteredUsersDataFromDB : %s",it->device_type.c_str());
      CLog::Detail(THISMODULE, "xGatePBXBusiness::GetRegisteredUsersDataFromDB : %s",it->prev_proxy_username.c_str());
      CLog::Detail(THISMODULE, "xGatePBXBusiness::GetRegisteredUsersDataFromDB : %s",it->IpAddress.c_str());
      }
      */
}

void xGatePBXBusiness::TrimLeadingZeroPlus(xGateDBObj* dbObj)
{
	if (dbObj->cp.isPstnNumber)
		return;

        if ((strstr(dbObj->cp.cnCalling.c_str(),"+")) && (strlen(dbObj->cp.cnCalling.c_str()) < 6))
                dbObj->cp.cnCalling.assign((dbObj->cp.cnCalling.substr(1)).c_str());


        if (!(strcmp((dbObj->cp.cnCalled.substr(0,2)).c_str(),"00")))
                dbObj->cp.cnCalled.assign((dbObj->cp.cnCalled.substr(2)).c_str());
}
void xGatePBXBusiness::deleteConferenceDBRecords(xGateDBObj* dbObj, xGateDBManager* gm_db_manager)
{
  CLog::Detail(THISMODULE, "xGatePBXBusiness::deleteConferenceDBRecords()");
  gm_db_manager->DATABASE_QUERY_CONF_DELETE_DB_RECORDS(dbObj->cp.confinfo.gm_conf_host_extension.c_str(),dbObj->cp.call_id.c_str());
}
void xGatePBXBusiness::GetAddressofrecords(xGateDBObj* dbObj){
  std::ostringstream dbInput;
  xGateDbmMsg *pMongoDBMDispatcher = new xGateDbmMsg();

  DbmReqDetail &reqDetail = pMongoDBMDispatcher->get_dbm_request_detail();

  reqDetail.m_transId.assign(dbObj->transid.c_str());
  reqDetail.m_dbType = EN_XGATE_DB_LAYER_MONGODB;
  reqDetail.m_opCode = EN_XGATE_DBM_MSG_DBEXECUTE;
  reqDetail.m_dbName.assign("unifiedring_pbx");

  reqDetail.m_spName.assign("sip_get_Registered_user_info");
  dbInput << "{" << "\"Username\"" << ":" << "\"" <<dbObj->cp.cnCalled.c_str() << "\"" << ","
    << "\"domain_name\"" << ":" << "\""  << dbObj->cp.routeAddr.c_str() << "\"" << "}";
  reqDetail.m_input.assign(dbInput.str());
  ACE_Task<ACE_MT_SYNCH>* taskPtr = (xGateDbmDispatcher*)xGateUtil::getDbmDispatcher();

  pMongoDBMDispatcher->setMsgType(EN_XGATE_MSG_TO_DBM);

  ACE_Message_Block* pAmb = NULL;
  pAmb = static_cast<xGateDbmMsg*>(pMongoDBMDispatcher);
  ACE_Time_Value tval ((time(NULL) ));
  tval += ACE_Time_Value(0,1);

  if(taskPtr->putq(pAmb, &tval)) {
    CLog::Detail(THISMODULE, "MGC-Business-layer-thread posted message to DBM-Network-component");
  } else {
    CLog::Error(THISMODULE, "MGC-Business-layer-thread failed to pose message to DBM-Network-component !");
  }
}
void xGatePBXBusiness::SetRegisterRecordFromDB(xGateDBObj* dbObj,std::string dbOut){
  jsonParser(dbObj,dbOut);
}
void  xGatePBXBusiness::jsonParser(xGateDBObj* dbObj,std::string dbOutput)
{
  Document doc;
  std::string tempdb_value = "";
  if (doc.Parse(dbOutput.c_str()).HasParseError()){
    return;
  }
  string contact_address = "";
  if (!dbObj->cp.ptr_xGateRegistrationVector)
  {
    std::vector<xGateRegistrationRecord>* ptrRegistrationVector = new vector<xGateRegistrationRecord>;
    dbObj->cp.ptr_xGateRegistrationVector = ptrRegistrationVector;
  }
  if(doc.IsObject()) {
    xGateRegistrationRecord* temp_registration = new xGateRegistrationRecord();
    if (!doc.HasMember("Contact_address")){
      return;
    }
    else{
      tempdb_value = doc["Contact_address"].GetString();
      temp_registration->Contact_address.assign(tempdb_value.c_str());
      dbObj->cp.csaCalled.assign(tempdb_value.c_str());
    }
    if (!doc.HasMember("proxy_username")){
      return;
    }
    else{
      tempdb_value = doc["proxy_username"].GetString();
      temp_registration->proxy_username.assign(tempdb_value.c_str());
      dbObj->cp.proxyUserName.assign(tempdb_value.c_str());
    }
    if (!doc.HasMember("device_type")){
      return;
    }
    else{
      tempdb_value = doc["device_type"].GetString();
      temp_registration->device_type.assign(tempdb_value.c_str());
      dbObj->cp.calledDeviceType.assign(tempdb_value.c_str());
    }
    if (!doc.HasMember("prev_proxy_username")){
      return;
    }
    else{
      tempdb_value = doc["prev_proxy_username"].GetString();
      temp_registration->prev_proxy_username.assign(tempdb_value.c_str());
    }
    if (!doc.HasMember("IpAddress")){
      return;
    }
    else{
      tempdb_value = doc["IpAddress"].GetString();
      temp_registration->IpAddress.assign(tempdb_value.c_str());
    }
    if (!doc.HasMember("status")){
      return;
    }
    else{
      int value = doc["status"].GetInt();
      temp_registration->appStatus = value;
    }
    if (!doc.HasMember("rec_count")){
      return;
    }
    else{
      int value = doc["rec_count"].GetInt();
      temp_registration->rec_count = value;
     dbObj->cp.gm_totalNoOfRegisteredUsers = value;
    }
    (dbObj->cp.ptr_xGateRegistrationVector)->push_back(*temp_registration);
  }
  else if (doc.IsArray()) {
    for (SizeType i = 0; i < doc.Size(); i++){
      xGateRegistrationRecord* temp_registration = new xGateRegistrationRecord();
      bool urdesk = false;
      for (Value::ConstMemberIterator itr1 = doc[i].MemberBegin(); itr1 != doc[i].MemberEnd(); itr1++)
      {
        string key = itr1->name.GetString();

        if (key.compare("Contact_address") == 0)
        {
          string tempdb_value = itr1->value.GetString();
          temp_registration->Contact_address.assign(tempdb_value.c_str());
          continue;
        }
        else if (key.compare("proxy_username") == 0)
        {
          string tempdb_value = itr1->value.GetString();
          temp_registration->proxy_username.assign(tempdb_value.c_str());
          continue;
        }
        else if (key.compare("device_type") == 0)
        {
          string tempdb_value = itr1->value.GetString();
          temp_registration->device_type.assign(tempdb_value.c_str());
          dbObj->cp.calledDeviceType.assign(tempdb_value.c_str());
          if(tempdb_value.compare("URDESK") == 0)
          {
            urdesk = true;
          }
          continue;

        }
        else if (key.compare("prev_proxy_username") == 0)
        {
          string tempdb_value = itr1->value.GetString();
          temp_registration->prev_proxy_username.assign(tempdb_value.c_str());
          continue;
        }
        else if (key.compare("Ipaddress") == 0)
        {
          string tempdb_value = itr1->value.GetString();
          temp_registration->IpAddress.assign(tempdb_value.c_str());
          continue;
        }
        else if (key.compare("status") == 0)
        {
          int tempdb_value = itr1->value.GetInt();
          temp_registration->appStatus = tempdb_value;
          continue;
        }
        else if (key.compare("rec_count") == 0)
        {
          int value = itr1->value.GetInt();
          temp_registration->rec_count = value;
          continue;
        }
      }
      if(urdesk != true)
      {
        (dbObj->cp.ptr_xGateRegistrationVector)->push_back(*temp_registration);
          dbObj->cp.gm_totalNoOfRegisteredUsers = i+1;
      }
    }

  }
}
void xGatePBXBusiness::checkCalledNumberType(xGateDBObj *dbObj, xGateDBManager* gm_db_manager)
{
	CLog::Detail(THISMODULE, "Inside xGatePBXBusiness::fetch_featureListParams_from_DBmanager");
	callInfoParameter cp;
	cp =  dbObj->cp;
	bool is_a_cli_DESKPHONE = true;
	bool is_a_did_DESKPHONE = true;
	bool is_a_did_mobileNumber = false;
	if (strlen(cp.cnCalling.c_str()) > 6)
		is_a_cli_DESKPHONE = false;

	if (strlen(cp.cnCalled.c_str()) > 6)
		is_a_did_DESKPHONE = false;

	if ((strlen(cp.cnCalled.c_str()) > 11) && (strlen(cp.cnCalled.c_str()) < 15))
		is_a_did_mobileNumber = true;

	TrimLeadingZeroPlus(dbObj);
	std::string temp_cnCalled;
	if ((cp.isPstnNumber) && (!cp.isSwitchBoardCall))
	{
		std::string temp_routeAddr;
		temp_routeAddr.assign("");

		temp_routeAddr.assign(gm_db_manager->DATABASE_QUERY_get_SwitchBoard_domainName(dbObj->cp.cnCalled.c_str()));
		if (strlen(temp_routeAddr.c_str()) > 0)
		{
			cp.isSwitchBoardCall = true;
			cp.swbinfo.SwbCallEvent =  EN_SWB_WELCOME;
			dbObj->cp.swbinfo.SwbCallEvent = EN_SWB_WELCOME ;
			cp.routeAddr.assign(temp_routeAddr.c_str());
			cp.isPstnNumber = false;
			dbObj->cp.isPstnNumber = false;
		}
		else{
			db_combined_query_initial(dbObj,gm_db_manager);
                        if(dbObj->cp.isconferenceCall)
                        return;

			if (dbObj->cp.isHuntGroupCall)
			{
				cp.isPstnNumber = false;
				dbObj->cp.isPstnNumber = false;
				if (!is_a_cli_DESKPHONE)
				{
					cp.routeAddr.assign("");
					dbObj->cp.routeAddr.assign("");
				}
			}

			cp.isSwitchBoardCall = false;
			cp.swbinfo.SwbCallEvent =  EN_SWB_NULL;
			dbObj->cp.swbinfo.SwbCallEvent = EN_SWB_NULL ;

			db_combined_query_DirectNumber(dbObj, gm_db_manager);

			dbObj->cp.directNumberRecord.mappedExtension = gm_db_manager->DATABASE_QUERY_GET_DIRECT_NUMBER(dbObj->cp.cnCalled.c_str(), dbObj->cp.gm_B_domain_id.c_str());
			if (strlen(dbObj->cp.directNumberRecord.mappedExtension.c_str()) == 3)
			{
				dbObj->cp.cnCalled.assign(dbObj->cp.directNumberRecord.mappedExtension.c_str());
				cp.cnCalled.assign(dbObj->cp.directNumberRecord.mappedExtension.c_str());

				if (strlen(cp.cnCalling.c_str()) > 6)
				{
					cp.cnCalling.assign(cp.cnCalling.c_str()+1);
					dbObj->cp.cnCalling.assign(cp.cnCalling.c_str()+1);
				}
				else
				{
					dbObj->cp.cnCalling.assign(cp.cnCalling.c_str());
				}
				is_a_did_DESKPHONE = true;
				dbObj->cp.isPstnNumber = cp.isPstnNumber = false;

				cp.routeAddr.assign(dbObj->cp.directNumberRecord.domainName.c_str());
			}
		}

		//cp.routeAddr.assign(gm_db_manager->DATABASE_QUERY_get_SwitchBoard_domainName(dbObj->cp.cnCalled.c_str()));
	}
	cp.gm_user_A_DOMAIN_NAME.assign(cp.routeAddr.c_str());

	CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : gm_user_A_DOMAIN_NAME = %s", cp.gm_user_A_DOMAIN_NAME.c_str());
	cp.gm_A_domain_id.assign((gm_db_manager->DATABASE_QUERY_GET_DOMAIN_ID(cp.gm_user_A_DOMAIN_NAME.c_str())).c_str());

	// following hard-coding is added for sake of testing; to be removed later...
	if (((dbObj->cp.isconferenceCall) || (dbObj->cp.isHuntGroupCall)) && (!is_a_cli_DESKPHONE))
	{
		//cp.gm_A_domain_id.assign("1923");
		cp.gm_A_domain_id.assign(dbObj->cp.gm_B_domain_id.c_str());
		cp.gm_user_A_DOMAIN_NAME.assign(gm_db_manager->DATABASE_QUERY_GET_DOMAIN_NAME(dbObj->cp.gm_B_domain_id.c_str()));
	}

	if (cp.gm_user_A_DOMAIN_NAME.empty())
	{
		cp.gm_user_A_DOMAIN_NAME.assign(gm_db_manager->DATABASE_QUERY_get_SwitchBoard_domainName(dbObj->cp.cnCalled.c_str()));
		cp.gm_user_B_DOMAIN_NAME.assign(dbObj->cp.gm_user_A_DOMAIN_NAME.c_str());
	}
	cp.gm_user_B_DOMAIN_NAME.assign(cp.gm_user_A_DOMAIN_NAME.c_str());
	cp.gm_B_domain_id.assign(cp.gm_A_domain_id.c_str());
	CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : EN_DB_SWBDOMAINNAME_RES : gm_user_B_DOMAIN_NAME = %s", cp.gm_user_B_DOMAIN_NAME.c_str());
	CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : EN_DB_DOMAINID_RES : gm_B_domain_id = %s", cp.gm_B_domain_id.c_str());

	if (is_a_did_DESKPHONE)
		temp_cnCalled = gm_db_manager->DATABASE_QUERY_GET_DIRECT_NUMBER((dbObj->cp.cnCalled.c_str()), cp.gm_B_domain_id.c_str());
	else
		//                      std::string pstncalled = dbObj->cp.cnCalled.substr(1);
		temp_cnCalled = gm_db_manager->DATABASE_QUERY_GET_DIRECT_NUMBER((dbObj->cp.cnCalled.substr(1).c_str()), cp.gm_B_domain_id.c_str());

	if (strlen(temp_cnCalled.c_str()) == 3)
	{
		dbObj->cp.cnCalled.assign(temp_cnCalled.c_str());
		cp.cnCalled.assign(temp_cnCalled.c_str());

		if (strlen(cp.cnCalling.c_str()) > 6)
		{
			cp.cnCalling.assign(cp.cnCalling.c_str()+1);
			dbObj->cp.cnCalling.assign(cp.cnCalling.c_str()+1);
		}
		else
		{
			dbObj->cp.cnCalling.assign(cp.cnCalling.c_str());
		}

		is_a_did_DESKPHONE = true;
		dbObj->cp.isPstnNumber = cp.isPstnNumber = false;
	}
	if (is_a_did_DESKPHONE)
	{
		// Check Whether this call is forwarded...
		CheckAndSet_CallForwardingNumber_if_enabled(dbObj, true, gm_db_manager); // contains 3 DATABASE_QUERY
	}
	dbObj->cp = cp;
	return;
}

void xGatePBXBusiness::switchBoardProcess(xGateDBObj *dbObj, xGateDBManager* gm_db_manager)
{
	CLog::Detail(THISMODULE, "Inside xGatePBXBusiness::switchBoardProcess");
	callInfoParameter cp;
	cp =  dbObj->cp;
	if ((cp.isPstnNumber) && (!cp.isSwitchBoardCall) && (cp.isSwitchBoardCall_duringDialledExtension))
	{
		CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : isPstnNumber == true");
		dbObj->cp.rbtFile.assign(WAV_FILE_PATH);
		dbObj->cp.rbtFile.append(RBT_DEFAULT_FILE);

		dbObj->cp.callOnHoldFilenameCallingParty.assign(WAV_FILE_PATH);
		dbObj->cp.callOnHoldFilenameCallingParty.append(CALLHOLD_FILE_DEFAULT);

		dbObj->cp.userConnectingFile.assign(WAV_FILE_PATH);
		dbObj->cp.userConnectingFile.append(CONNECTING_FILE_DEFAULT);

		dbObj->cp.gm_user_greeting_file_name_user_B.assign(WAV_FILE_PATH);
		dbObj->cp.gm_user_greeting_file_name_user_B.append(SILENCE_500MS_FILE);
		dbObj->cp.gm_user_greeting_file_name_user_B_After.assign(WAV_FILE_PATH);
		dbObj->cp.gm_user_greeting_file_name_user_B_After.append(SILENCE_500MS_FILE);

		dbObj->cp.csfilename.assign(dbObj->cp.userConnectingFile.c_str());

		dbObj->cp.csCalledCodec = 8;
		if ((dbObj->cp.cnCalled.length() > 10) && (!cp.isThisCallForwarded))
		{
			std::string caller_id;
			caller_id.assign("");
			caller_id = gm_db_manager->DATABASE_QUERY_GET_CALLER_ID(dbObj->cp.gm_B_domain_id.c_str(), dbObj->cp.cnCalling.substr(1).c_str());
			if (caller_id.empty())
			{
				cp.cnCalling.assign("+442034440424");
				dbObj->cp.cnCalling.assign("+442034440424");
			}
			else
			{
				cp.cnCalling.assign(caller_id.c_str());
				dbObj->cp.cnCalling.assign(caller_id.c_str());
			}
		}

		dbObj->dbevent = (XGATEDBEVENT) EN_DB_PREQUERY_RES;
	}
	if ((cp.cnCalled.length() > 10) && (!cp.isSwitchBoardCall) && (!dbObj->cp.isSwitchBoardCall_duringDialledExtension) && (!cp.isThisCallForwarded))
	{
		std::string caller_id;
		caller_id.assign("");
		caller_id = gm_db_manager->DATABASE_QUERY_GET_CALLER_ID(dbObj->cp.gm_B_domain_id.c_str(), dbObj->cp.cnCalling.c_str()+1);
		if (caller_id.empty())
			cp.cnCalling.assign("+442034440424");
		else
			cp.cnCalling.assign(caller_id.c_str());
	}

	if ((cp.isPstnNumber) && (!cp.isSwitchBoardCall) && (!dbObj->cp.isSwitchBoardCall_duringDialledExtension))
	{
		std::string temp_string = cp.cnCalled;
		cp.cnCalled.assign("");
		//  cp.cnCalled.assign("+");
		cp.cnCalled.append(temp_string.c_str());
		CLog::Detail(THISMODULE, "xGatePBXBusiness::fetch_featureListParams_from_DBmanager : cnCalled = %s",cp.cnCalled.c_str());
		cp.csCalledCodec = 8;
		cp.swbinfo.SwbCallEvent =  EN_SWB_NULL;

	}
	if (cp.isSwitchBoardCall)
	{
		cp.rbtFile.assign(WAV_FILE_PATH);
		cp.rbtFile.append(RBT_DEFAULT_FILE);
		cp.swbinfo.SwbCallEvent = EN_SWB_DIAL;
		dbObj->cp = cp;
		fetch_switchbaordData_from_DBmanager(dbObj, gm_db_manager);
	}
}

