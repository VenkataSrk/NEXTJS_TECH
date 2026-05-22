/* $Header: /xGate/mtnrtp.cpp 39    7/26/04 11:41a Bennylp $
 *   
 * XGATE
 * (C) 2001 Switchlab, Ltd
 *
 * MTN RTP framework
 *
 * $Log: /xGate/mtnrtp.cpp $
 * 
 * 39    7/26/04 11:41a Bennylp
 * Problem with MTN: if too many calls, error too many pending message for
 * channel. Increase timer resolution (mtnrtp.cpp)
 * 
 * 38    4/16/04 10:09a Bennylp
 * Support for NAT detection/workaround in RTP/RTCP streams
 * 
 * 37    3/08/04 7:12p Bennylp
 * Initial support for RTP virtual channel
 * 
 * 36    1/09/04 6:27p Bennylp
 * Removed annoying log info about RTCP SR/RR jitter
 * 
 * 35    7/14/03 2:03p Bennylp
 * Added delay/jitter/lost simulation
 * 
 * 34    6/10/03 1:01p Bennylp
 * Added average
 * 
 * 33    6/06/03 7:17p Bennylp
 * Tidy up mtn-rtp output
 * 
 * 32    6/06/03 7:04p Bennylp
 * Add downstream_lost_duration in total talk time
 * 
 * 31    6/06/03 6:40p Bennylp
 * Fixed bug in calculation, tidy up output.
 * 
 * 30    6/06/03 4:28p Bennylp
 * Tidy up a bit the output of mtn-rtp voice-quality command.
 * 
 * 29    6/06/03 4:07p Bennylp
 * Lots of changes to incorporate mapletree diagnostic (Version 54.4)
 * 
 * 28    4/10/03 9:41a Bennylp
 * Support for CNG transmission for G.711
 * 
 * 27    4/09/03 11:08a Bennylp
 * Fixed memory leak (RTP factory instance is not deleted)
 * 
 * 26    4/04/03 11:59a Bennylp
 * Alloc and tx to TDM early when channel is created (cross-talk problem)
 * 
 * 25    2/17/03 3:26p Bennylp
 * Added volume setting to Mapletree RTP channel
 * 
 * 24    2/12/03 10:21a Bennylp
 * Added probation
 * 
 * 23    2/04/03 6:05p Bennylp
 * Support for DTMF/telephony event receipt over RTP
 * 
 * 22    1/31/03 2:25p Bennylp
 * Proper lock to 'mtn-rtp-channel' command.
 * 
 * 21    12/24/02 5:04p Bennylp
 * Some optimization (Quantify)
 * 
 * 20    12/06/02 3:44p Bennylp
 * Protect against divided by zero in RTP_Pci_Stream_Thread::start
 * 
 * 19    12/06/02 11:50a Bennylp
* Add TDM settings, fix sequence numbering, remove bad payload msg.
* 
* 18    11/04/02 7:02p Bennylp
* Show resource name in switch device.
* 
* 17    11/04/02 5:29p Bennylp
* Changed some warning to fatal.
* 
  * 16    11/01/02 7:58p Bennylp
* Bug fixes (47.1)
  * 
  * 15    10/31/02 4:55p Bennylp
  * Added hardware revision info.
  * 
  * 14    10/29/02 1:05p Bennylp
  * Stable version for MTN2100/1024P, tested.
  * 
  * 13    10/24/02 8:34p Bennylp
  * Latest stable checkin.
  * 
  * 12    10/23/02 5:01p Bennylp
  * Some more fixes
  * 
  * 11    10/21/02 6:40p Bennylp
  * Changes to adapt the new MTN framework changes.
  * 
  * 10    9/25/02 5:12p Bennylp
  * - Fixed RTCP and quality monitoring bugs
  * - Improved timing of the RTP packet dispatching
  * 
  * 9     9/23/02 2:46p Bennylp
  * Changed macro USE_NEW_RTP to xgconfig.h
  * 
  * 8     9/20/02 5:16p Bennylp
  * Fixes: full quality monitoring, fixes to work with q931 over IP.
  * 
  * 7     17/09/02 18:51 Adham
  * - fix on RTP_Pci_Stream::remoteport_ initialization
  * - guard added in SendToEth. If the stream hasn't been activated, event
  * from mapletree channel will be ignored.
  * 
  * 6     4/09/02 12:10 Adham
  * asynch termination
  * 
  * 5     19/08/02 15:52 Adham
  * fix bug on sendinng stream to bos don't send if data length is invalid
  * 
  * 4     16/08/02 17:24 Adham
  * Updates on Quality stat
  * 
  * 3     5/08/02 18:53 Adham
  * First release MTN RTP framework
  *  
  */

#include "stdafx.h"

#include "xgconfig.h"
#include <pthread.h>
#ifdef XGATE_HAS_MTN
#ifdef XGATE_HAS_MTNRTP
#include "../MTN_Lib/src/mtn_msg.h"
#include "mtnrtp.h"
#include "hrtimer.h"
#include "../MTN_Lib/src/mtn_fwd.h"
#include "thread_process.h"
#include "rtpmedia.h"
#include <regex.h>
#include "Reactor_Imp_Sock.h"

#define THISMODULE "MTNRTP"
#include "kernel.h"

#define FIVE_SECOND	5000
#define TIMER_INTERVAL	3
#define TIMER_RES	1

#ifdef XGATE_MTN_HAS_JIITER_SIM
#define NET_RX_DELAY_MIN    0
#define NET_RX_DELAY_MAX    0
#define NET_RX_PERCENT_LOST 0
#define NET_TX_DELAY_MIN    0
#define NET_TX_DELAY_MAX    0
#define NET_TX_PERCENT_LOST 0

#if (NET_RX_DELAY_MAX < NET_RX_DELAY_MIN)
#error Check the delay settings
#endif

#if (NET_TX_DELAY_MAX < NET_TX_DELAY_MIN)
#error Check the delay settings
#endif

#endif

  ////////////////////////////////////////////////////////////////////////////////
  /** utility function for rtp rtcp
  */

#include <time.h>
#define SECS_BETWEEN_1900_1970	2208988800u
#define USECS_IN_SECOND		1000000
  //#define TRACE_RTCP
  //#define USE_OWN_LOST_CALC

  static High_Res_Timer hrtimer;

  extern "C" {
    void *rtp_pci_stream_thread_starter(void *instance);
  };

static unsigned long GetTickCount()
{
  struct timeval tv;
  if( gettimeofday(&tv, NULL) != 0 )
    return 0;

  return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}


extern char* FormatData(char* str, float val, const char* format);

int init_timer()
{
  hrtimer.init();
  hrtimer.start();
  return 0;
}

  static int 
get_timeofday(struct timeval *tp, void *t)
{
  static int dummy = init_timer();

  hrtimer.stop();
  Time_Value tv;
  hrtimer.get_elapsed(&tv);
  tp->tv_sec = tv.sec();
  tp->tv_usec = tv.usec();
  return 1;
}

  static void 
ntp64_time(const struct timeval *now, ntp_time_t *ntp)
{
  /* NB ntp_frac is in units of 1 / (2^32 - 1) secs. */
  ntp->sec  = now->tv_sec + SECS_BETWEEN_1900_1970;
  ntp->frac = (now->tv_usec << 12) + (now->tv_usec << 8) - 
    ((now->tv_usec * 3650) >> 6);
}


///////////////////////////////////////////////////////////////////////////////
/** This singleton class manages threads to poll both PCI interface of the
  board and the UDP socket.
  */
class RTP_Pci_Stream_Thread
{
  public:
    ~RTP_Pci_Stream_Thread();
    static RTP_Pci_Stream_Thread* instance();

    bool add_channel( MTN_Rtp_Channel *ch );
    bool remove_channel( MTN_Rtp_Channel *ch );

    bool start();
    void stop();

  private:
    struct Thread_Instance
    {
      ACE_Thread_Mutex	mutex_;
      int			count_;
      bool			running_;
      ACE_thread_t		ithread_;
      ACE_hthread_t		hthread_;
      HANDLE			timer_event_;
      unsigned long           timer_id_;
      ACE_Socket_Reactor	reactor_;
      Thread_Instance	       *next_;
      Mapletree_Module       *module_;

      Thread_Instance()
        : count_(0), running_(false), next_(NULL), module_(NULL)
      {
      }

      bool start(unsigned thread_flag, unsigned timer_interval, unsigned timer_res);
      bool resume();
      bool stop();

      static void* thread_func(void*);
      void *thread_main();
    };

    Thread_Instance *first_thread_;

    RTP_Pci_Stream_Thread();
    bool is_channel_owner(Thread_Instance *t, MTN_Rtp_Channel *ch) const;

    friend void *rtp_pci_stream_thread_starter(void *instance);
};


  RTP_Pci_Stream_Thread::RTP_Pci_Stream_Thread()
: first_thread_(NULL)
{
}

RTP_Pci_Stream_Thread::~RTP_Pci_Stream_Thread()
{
  // stop all threads, just to make sure.
  assert(first_thread_ == NULL);
}

/*
 * 
 */
bool RTP_Pci_Stream_Thread::start()
{
  // Create instances of threads.
  // Threads are created suspended, and then resumed with interval of few
  // milliseconds so that they're not waking up at the same time (theoritically
  // at least).

  Mapletree_Module_Manager *mtn_mgr = Mapletree_Module_Manager::Instance();
  if (!mtn_mgr)
    return true;

  int i, count = mtn_mgr->GetModuleCount();
  Thread_Instance *t;

  for (i=0; i<count; ++i) {
    Mapletree_Module *mod = mtn_mgr->GetModule(i);
    assert(mod);

    t = new Thread_Instance;
    t->module_ = mod;
    if (!t->start(THR_SUSPENDED, TIMER_INTERVAL, TIMER_RES)) {
      delete t;
      return false;
    }

    t->next_ = first_thread_;    
    first_thread_ = t;
  }

  // Resume threads, separated by interval
  unsigned interval = count ? TIMER_INTERVAL / count : TIMER_INTERVAL;
  Highest_Res_Timer timer;
  timer.init();
  timer.start();

  t = first_thread_;
  while (t) {
    if (!t->resume())
      return false;

    do {
      timer.stop();
    } while (timer.get_elapsed_msec() < interval);

    t = t->next_;
  }
  return true;
}

/*
 * Stop all worker threads.
 */
void RTP_Pci_Stream_Thread::stop()
{
  Thread_Instance *t = first_thread_;
  while (t) {
    Thread_Instance *next = t->next_;
    t->stop();
    delete t;
    t = next;
  }
  first_thread_ = NULL;
}

/*
 * Get the singleton instance of the RTP_Pci_Stream_Thread
 */
RTP_Pci_Stream_Thread* RTP_Pci_Stream_Thread::instance()
{ 
  static RTP_Pci_Stream_Thread instance;
  return &instance;	
}

/*
 * Add one channel to be polled by the threads.
 * The send_sock argument will be filled by socket handle which can be used 
 * to send UDP data (by channel).
 */
bool RTP_Pci_Stream_Thread::add_channel( MTN_Rtp_Channel *ch /* , int *send_sock */ )
{
  //CLog::Info(THISMODULE, "Channel %s is added to thread", ch->GetChannelName());

  // Finds the thread which has the specified module/board.
  Mapletree_Module *mod = ch->GetModule();
  assert(mod != NULL);
  if (!mod) {
    CLog::Error( THISMODULE, "RTP_Pci_Stream_Thread::add_channel(%s) error: "
        "module is not set", ch->GetChannelName());
    return false;
  }

  Thread_Instance *t = first_thread_;
  while (t && !is_channel_owner(t, ch))
    t = t->next_;

  if (!t) {
    //assert(false);
    t = new Thread_Instance;
    t->module_ = mod;
    if (!t->start(0, TIMER_INTERVAL, TIMER_RES)) {
      delete t;
      return false;
    }

    t->next_ = first_thread_;    
    first_thread_ = t;
  }

  ACE_Guard<ACE_Thread_Mutex> guard(t->mutex_);
  if (t->reactor_.register_handler( &ch->rtpstream_.rtp_, ACE_Event_Handler::READ_MASK)) {
    CLog::Error( THISMODULE, "Error in registering channel %s to reactor", ch->GetChannelName());
    return false;
  }
  if (t->reactor_.register_handler( &ch->rtpstream_.rtcp_, ACE_Event_Handler::READ_MASK)) {
    CLog::Error( THISMODULE, "Error in registering channel %s to reactor", ch->GetChannelName());
    t->reactor_.remove_handler( &ch->rtpstream_.rtp_, ACE_Event_Handler::DONT_CALL | 
        ACE_Event_Handler::READ_MASK);
    return false;
  }
  ++t->count_;
  return true;
}

bool RTP_Pci_Stream_Thread::remove_channel( MTN_Rtp_Channel *ch )
{
  //CLog::Info(THISMODULE, "Channel %s is removed from thread", ch->GetChannelName());

  // Finds the thread which has the specified module/board.
  Thread_Instance *t = first_thread_;
  while (t && !is_channel_owner(t, ch))
    t = t->next_;

  if (t == NULL) {
    CLog::Fatal(THISMODULE, "Error in RTP_Pci_Stream_Thread::remove_channel: channel not found (%s)",
        ch->GetChannelName());
    return false;
  }

  ACE_Guard<ACE_Thread_Mutex> guard(t->mutex_);
  t->reactor_.remove_handler( &ch->rtpstream_.rtp_, ACE_Event_Handler::DONT_CALL | 
      ACE_Event_Handler::READ_MASK);
  t->reactor_.remove_handler( &ch->rtpstream_.rtcp_, ACE_Event_Handler::DONT_CALL | 
      ACE_Event_Handler::READ_MASK);
  --t->count_;
  return true;
}

bool RTP_Pci_Stream_Thread::is_channel_owner(Thread_Instance *t, MTN_Rtp_Channel *ch) const
{
  return t->module_ == ch->GetModule();
  //return true;
}

void *rtp_pci_stream_thread_starter(void *instance)
{
  RTP_Pci_Stream_Thread::Thread_Instance *t = (RTP_Pci_Stream_Thread::Thread_Instance*)instance;
  return t->thread_main();
}

bool RTP_Pci_Stream_Thread::Thread_Instance::start(unsigned thread_flag, 
    unsigned timer_interval, unsigned timer_res)
{
#ifndef __linux__
  timer_event_ = CreateEvent(NULL, FALSE, FALSE, NULL);
  if (!timer_event_) {
    CLog::Error( THISMODULE, "PCI stream thread: unable to create event");
    return false;
  }
#endif

#ifndef __linux__
  timer_id_ = timeSetEvent(timer_interval, timer_res, 
      (LPTIMECALLBACK)timer_event_, 0, 
      TIME_PERIODIC | TIME_CALLBACK_EVENT_SET);
  if (timer_id_ == NULL) {
    CLog::Error( THISMODULE, "PCI stream thread: unable to create timer");
    CloseHandle(timer_event_);
    return false;
  }

  if (SPAWN_THREAD( &rtp_pci_stream_thread_starter, this, THR_NEW_LWP|THR_JOINABLE|thread_flag, &ithread_, &hthread_)) {
    CLog::Error( THISMODULE, "PCI stream thread: unable to create thread");
    timeKillEvent(timer_id_);
    CloseHandle(timer_event_);
    return false;
  }

  SetThreadPriority(hthread_, THREAD_PRIORITY_HIGHEST);
#else
  pthread_t rtp_thread;
  int rc;
  /* Write linux equivalent code here */
  rc = pthread_create(&rtp_thread,NULL,&rtp_pci_stream_thread_starter,(void *) this);
  if (rc != 0) {
    printf("PCI stream thread: unable to create thread\n");
    CLog::Error( THISMODULE, "PCI stream thread: unable to create thread"); 
    exit(EXIT_FAILURE);
  }
#endif
  running_ = true;
  return true;
}

bool RTP_Pci_Stream_Thread::Thread_Instance::resume()
{
  return ACE_Thread::resume(hthread_) == 0;
}

bool RTP_Pci_Stream_Thread::Thread_Instance::stop()
{
  if (!running_)
    return true;

  // Set the flag to quit the thread.
  running_ = false;

#ifndef __linux__
  // Wait thread to stop.
  ACE_Thread::join(hthread_);

  // Cleaning up.
  CloseHandle(timer_event_);
  timeKillEvent(timer_id_);
#else
  /* Write linux equivalent code here */
#endif

  return true;
}

/*
   void * RTP_Pci_Stream_Thread::Thread_Instance::thread_func(void *instance)
   {
   Thread_Instance *t = (Thread_Instance*)instance;
   return t->thread_main();
   }
   */

void * RTP_Pci_Stream_Thread::Thread_Instance::thread_main()
{
  static ACE_Time_Value timeout(0, 0);
  //printf("******** Thread_Instance::thread_main ************ \n");
  // disable ioctl to detect closing socket.
  reactor_.enable_ioctl_read(false);

  while (running_) {
    //ResetEvent(timer_event_);
#ifndef __linux__
    WaitForSingleObject( timer_event_, INFINITE);
#endif

    // Check that Kernel is still running.
    /*
       Kernel *kernel = Kernel::Instance();
       if (!kernel || !kernel->IsInRunningState())
       break;
       */

    // Check that the board is still running and enabled.
    if (!module_->IsEnabled() || !module_->IsRunning())
      continue;

    // Finally, check events both from reactor and from the board.
    module_->HandleData();
    //printf("******** Thread_Instance::thread_main mutex_.acquire() ************ \n");
    mutex_.acquire();
    reactor_.handle_events(timeout);
    mutex_.release();
  }
  CLog::Info(THISMODULE, "RTP thread %x quitting..", ithread_);
  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
/**  Implementation of validator, to be used for 
*/
CVariantValidator *MTN_TermBoardEnum::Clone() const
{
  return new MTN_TermBoardEnum(*this);
}

void MTN_TermBoardEnum::AcquireLock() const
{
  ACQUIRE_SYSTEM_LOCK();
}

void MTN_TermBoardEnum::ReleaseLock() const
{
  RELEASE_SYSTEM_LOCK();
}

const List<ValidatorChoice> & MTN_TermBoardEnum::GetChoiceList() const
{
  choice_.clear();
  choice_.push_back(ValidatorChoice("virtual", "Virtual RTP channel"));
  choice_.push_back(ValidatorChoice("host", "Host media(HMP) RTP channel"));

  MTN_Rtp_Channel_Factory *fact = MTN_Rtp_Channel_Factory::Instance();
  if (!fact)
    return choice_;

  const char *boards[32];
  int count=fact->EnumBoard(boards, 32);
  for (int j=0; j<count; ++j) {
    choice_.push_back(ValidatorChoice(boards[j], ""));
  }    

  return choice_;
}

///////////////////////////////////////////////////////////////////////////////
/**
  MTN_Rtp_Manager implementation
  */

/** static singleton instance of MTN_Rtp_Manager
*/
MTN_Rtp_Manager *MTN_Rtp_Manager::instance_ = NULL;

MTN_Rtp_Manager::MTN_Rtp_Manager () 
  : ServiceProvider("MTN Rtp Mgr", 
      false, true, Normal_Priority, "MTN Rtp manager")
{
  config_ = NULL;
}

MTN_Rtp_Manager::~MTN_Rtp_Manager()
{
  if (config_)
    delete config_;
}

MTN_Rtp_Manager *MTN_Rtp_Manager::Create()
{    
  if (!instance_)
    instance_ = new MTN_Rtp_Manager();

  return instance_;
}

MTN_Rtp_Manager *MTN_Rtp_Manager::Instance()
{
  return instance_;
}

void MTN_Rtp_Manager::init_config()
{    
  Term_Cmd* cmd;

  /* Mapletree RTP managers setup menu
   *
   */    

  cmd = new Term_Cmd(this, TC_Show,
      "mtn-rtp", "Show Mapletree RTP channels status.",
      PrivilegeLevelAdmin, TermModeNormal, false);    

  cmd->Add( new Term_Cmd_Arg("@1", CVariant(""), CVariantSingleValueValidator("voice-quality", "Show voice quality")) );
  cmd->Add( new Term_Cmd_Arg("@2", CVariant(""), CVariantValidator("", "Regular expression to match name")));    
  RegisterTermCmd(cmd, Kernel::Instance()->RootConfig());

  //GetConfigTree()->RegisterShowHandler(this);
  Kernel::Instance()->RootConfig()->RegisterShowHandler(this);
}

void MTN_Rtp_Manager::print_pools(String& output)
{

}

bool MTN_Rtp_Manager::OnShowConfig(
    const Cfg_Tree_Entry *entry,
    String & output, 
    const String & indent, 
    int indent_size) const
{

  return OK;
}

Cfg_Tree_Group * MTN_Rtp_Manager::GetConfigTree()
{
  return config_;
}

bool MTN_Rtp_Manager::Load()
{
  char name[128];

  memset(name,0,sizeof(name));
  gethostname(name, 128);
  hostent* he = gethostbyname(name);

  struct in_addr addr;
  memcpy(&addr, he->h_addr_list[0],4);
  localip_ = inet_ntoa(addr);

  init_config();
  return true;
}

bool MTN_Rtp_Manager::Unload()
{
  RTP_Pci_Stream_Thread::instance()->stop();
  return true;
}

bool MTN_Rtp_Manager::CommitConfiguration()
{
  return RTP_Pci_Stream_Thread::instance()->start();
}

void MTN_Rtp_Manager::OnShutdown(bool cancel)
{
}

bool MTN_Rtp_Manager::AttachTask(CTask *pt)
{

  return true;
}

void MTN_Rtp_Manager::DetachTask(CTask *pt)
{
}

bool MTN_Rtp_Manager::CancelJob(CTask *pt)
{
  return true;
}

bool MTN_Rtp_Manager::OnShowStatus(String & output)
{
  return true;
}

void MTN_Rtp_Manager::DoTermCmd(Term_Session * session, bool no, Term_Cmd * cmd)
{
  CVariant v;
  String arg0;
  struct avg_quality
  {
    double count;
    double pkt;
    double miss;
    double late;
    double early;
    double bad;
    double blocked;
    double lost;
  } avg;

  switch (cmd->GetId())
  {
    case TC_Show:
      cmd->GetArg((size_t)0)->GetValue(v);
      arg0 = v.AsString();

      if (strcasecmp(arg0.c_str(), "voice-quality") == 0) {
        char temp[512];
        int len;
        bool active = false;
        bool idle = false;
        String output;
        regex_t reg;

        avg.count = avg.pkt = avg.miss = avg.late = avg.early = avg.bad = avg.blocked = avg.lost = 0.0;

        memset(&reg, 0, sizeof(reg));
        cmd->GetArg(1)->GetValue(v);

        if (regcomp(&reg, v.AsString(), REG_ICASE|REG_EXTENDED) != 0) {
          session->SendErrorf("Invalid regular expression format\n");
          return;
        }

        output  = "\n";
        output += " Name            U State Dir Durat- Sil Pkts Byte Miss Late Erly Bad  Blck Lost\n";
        output += "                              ion    %             %    %    %    %    %    % \n";
        output += " -------------------------------------------------------------------------------\n";


        active = true;
        idle = true;

        if (!ACQUIRE_SYSTEM_LOCK()) {
          regfree(&reg);
          return;
        }

        Vector<MTN_Rtp_Channel*> channels;

        if (active) {
          List<MTN_Rtp_Channel*>::iterator it, end=activechannels_.end();
          for (it=activechannels_.begin(); it!=end; ++it) {
            channels.push_back(*it);
          }
        }
        if (idle) {
          List<MTN_Rtp_Channel*>::iterator it, end=idlechannels_.end();
          for (it=idlechannels_.begin(); it!=end; ++it) {
            channels.push_back(*it);
          }
        }

        Vector<MTN_Rtp_Channel*>::iterator it, end;
        end = channels.end();
        for (it=channels.begin(); it != end; ++it) {
          char pkts[10], bytes[10];
          char name[32];
          unsigned ttotal;
          unsigned tot_pkt;
          unsigned tot_lost;
          double pct;
          MTN_Rtp_Channel *rtp_ch = *it;
          MTN_Channel *mtn_ch = rtp_ch->GetChannel();
          const MTN_Voice_Status_Mob_Data *vstatus;

          if (mtn_ch->has_voice_status())
            vstatus = mtn_ch->get_voice_status();
          else
            vstatus = NULL;

          // Build full name.
          strcpy(name, rtp_ch->GetSwitchDevice("") ? rtp_ch->GetSwitchDevice("")->GetName() :
              rtp_ch->GetChannelName());

          // Match name.
          if (regexec(&reg, name, 0, NULL, 0) != 0) {
            continue;
          }

          // Name
          len = sprintf(temp, " %-15s", name);
          output.append(temp, len);

          // Used?
          if (rtp_ch->IsActive()) {
            output.append(" Y", 2);
          } else {
            output.append(" N", 2);
          }

          // State
          len = sprintf(temp, " %5.5s", MTN_Util::port_state_name(mtn_ch->get_state()));
          output.append(temp, len);

          //
          // RX
          // 
          // Duration.
          if (vstatus) {
            ttotal = vstatus->get_downstream_speech_duration() + 
              vstatus->get_downstream_silence_duration() +
              vstatus->get_downstream_lost_speech_duration();
          } else {
            ttotal = 0;
          }
          // Silence percentage.
          if (ttotal < 0.1) {
            pct = 0;
          } else {
            pct = vstatus->get_downstream_silence_duration();
            pct = pct * 100 / ttotal;
          }
          // Total packet.
          if (vstatus) {
            tot_pkt = vstatus->get_downstream_packets() +
              vstatus->get_missing_packets() -
              vstatus->get_duplicated_packets();
          } else {
            tot_pkt = 0;
          }
          // Total lost.
          if (rtp_ch->GetRtcpStat().rx.ready) {
            tot_lost = rtp_ch->GetRtcpStat().rx.cummulativeLost;
          } else {
            tot_lost = 0;
          }
          double pct_missing = (tot_pkt ? vstatus->get_missing_packets() * 100.0 / tot_pkt : 0.0);
          double pct_late = (tot_pkt ? vstatus->get_late_packets() * 100.0 / tot_pkt : 0.0);
          double pct_early = (tot_pkt ? vstatus->get_early_packets() * 100.0 / tot_pkt : 0.0);
          double pct_bad = (tot_pkt ? vstatus->get_bad_packets() * 100.0 / tot_pkt : 0.0);
          len = sprintf(temp, "  RX %3d:%02d %3.0f %4s %4s %4.1f %4.1f %4.1f %4.1f      %4.1f\n",
              ttotal/60000, (ttotal / 1000) % 60,   // Duration
              pct,		    // Silence percentage
              FormatData(pkts, vstatus ? vstatus->get_downstream_packets() : 0.0, "%3.0f"), // Pkts
              FormatData(bytes, vstatus ? vstatus->get_downstream_octets() : 0.0, "%3.0f"), // Octets
              pct_missing,	// Missing
              pct_late,    // Late
              pct_early, // Early
              pct_bad,   // Bad
              /*0.0,	// Blocked
              */
              (tot_pkt ? tot_lost * 100.0 / tot_pkt : 0));	// Total
          output.append(temp, len);

          if (ttotal >= 10000) {
            avg.miss = (avg.miss * avg.count + pct_missing) / (avg.count + 1);
            avg.late = (avg.late * avg.count + pct_late) / (avg.count + 1);
            avg.early = (avg.early * avg.count + pct_early) / (avg.count + 1);
            avg.bad = (avg.bad * avg.count + pct_bad) / (avg.count + 1);
            avg.pkt += tot_pkt;
            avg.lost += tot_lost;
          }

          //
          // TX
          // 
          output.append(24, ' ');

          // Duration.
          if (vstatus) {
            ttotal = vstatus->get_upstream_speech_duration() + 
              vstatus->get_upstream_silence_duration();
          } else {
            ttotal = 0;
          }
          // Silence percentage.
          if (ttotal < 0.1) {
            pct = 0;
          } else {
            pct = vstatus->get_upstream_silence_duration();
            pct = pct * 100 / ttotal;
          }
          // Total packet.
          if (vstatus) {
            tot_pkt = vstatus->get_upstream_packets();
          } else {
            tot_pkt = 0;
          }
          // Total lost.
          if (rtp_ch->GetRtcpStat().tx.ready) {
            tot_lost = rtp_ch->GetRtcpStat().tx.cummulativeLost;
          } else {
            tot_lost = 0;
          }

          double pct_blocked = (tot_pkt ? vstatus->get_blocked_packets() * 100.0 / tot_pkt : 0.0);
          len = sprintf(temp, "  TX %3d:%02d %3.0f %4s %4s                     %4.1f %4.1f\n",
              ttotal/60000, (ttotal/1000)%60,   // Duration
              pct,		    // Silence percentage
              FormatData(pkts, vstatus ? vstatus->get_upstream_packets() : 0, "%3.0f"), // Pkts
              FormatData(bytes, vstatus ? vstatus->get_upstream_octets() : 0, "%3.0f"), // Octets
              /*0.0,	// Missing
                0.0,    // Late
                0.0, // Early
                0.0,   // Bad
                */
              pct_blocked,	// Blocked
              (tot_pkt ? tot_lost * 100.0 / tot_pkt : 0.0));	// Total
          output.append(temp, len);

          if (ttotal >= 10000) {
            avg.blocked = (avg.blocked * avg.count + pct_blocked) / (avg.count + 1);
            avg.pkt += tot_pkt;
            avg.lost += tot_lost;
            avg.count = avg.count + 1;
          }

        }
        RELEASE_SYSTEM_LOCK();
        regfree(&reg);

        if (avg.pkt > 1) {
          // avg.pkt is total pkt in BOTH RX and TX.
          // So is avg.lost.
          avg.lost = avg.lost * 100 / avg.pkt;
        }
        len = sprintf(temp, " -- Average: --         TX/RX                     "
            "%4.1f %4.1f %4.1f %4.1f %4.1f %4.1f\n",
            avg.miss,	// Missing
            avg.late,    // Late
            avg.early, // Early
            avg.bad,   // Bad
            avg.blocked,
            avg.lost);	// Total
        output.append(temp, len);
        output.append("\n", 1);
        session->Send( output.c_str(), output.size());

      }
      break;
    default: ;
  }
}

void MTN_Rtp_Manager::HandleEvents()
{
  List<MTN_Rtp_Channel*>::iterator it = channels_.begin();

  for (;it!=channels_.end();) {
    MTN_Rtp_Channel* mtn = *it;

    if (mtn->OnActivate())
      it = channels_.erase(it);
    else
      it++;
  }
}

const char* MTN_Rtp_Manager::GetLocalIP()
{
  return localip_.c_str();   
}    

void MTN_Rtp_Manager::AddActivatingChannel(MTN_Rtp_Channel* channel)
{
  channels_.push_back(channel);
}

void MTN_Rtp_Manager::AddChannel(MTN_Rtp_Channel* channel)
{
  idlechannels_.push_back(channel);
}

void MTN_Rtp_Manager::ChannelDeactivated(MTN_Rtp_Channel* channel)
{
  activechannels_.remove(channel);
  idlechannels_.push_back(channel);
}

void MTN_Rtp_Manager::RemoveChannel(MTN_Rtp_Channel* channel)
{
  channels_.remove(channel);
  activechannels_.remove(channel);
  idlechannels_.remove(channel);
}

void MTN_Rtp_Manager::ChannelActivated(MTN_Rtp_Channel* channel)
{
  assert(std::find(activechannels_.begin(), activechannels_.end(), channel) == activechannels_.end());
  activechannels_.push_back(channel);
  idlechannels_.remove(channel);
}

///////////////////////////////////////////////////////////////////////////////
/**
*/
MTN_Rtp_Channel_Factory *MTN_Rtp_Channel_Factory::instance_=NULL;

MTN_Rtp_Channel_Factory::MTN_Rtp_Channel_Factory()
{

}

MTN_Rtp_Channel_Factory::~MTN_Rtp_Channel_Factory()
{
}


MTN_Rtp_Channel_Factory *MTN_Rtp_Channel_Factory::Instance()
{
  static MTN_Rtp_Channel_Factory instance_;
  return &instance_;
}

unsigned MTN_Rtp_Channel_Factory::EnumBoard(const char** boards, int maxboards)
{
  Mapletree_Module_Manager *mm = Mapletree_Module_Manager::Instance();

  int count = maxboards < mm->GetModuleCount() ? maxboards : mm->GetModuleCount();

  for (int i=0; i<count; ++i) {
    boards[i] = mm->GetModule(i)->GetName();
  }

  return count;
}

/** Created MTN_Rtp_Channel from MTN_Channel
*/
MTN_Rtp_Channel *MTN_Rtp_Channel_Factory::Allocate(Rtp_Interface *itf, const char *board_name)
{
  Mapletree_Module *module = Mapletree_Module_Manager::Instance()->FindModule(board_name);
  if (!module) {
    CLog::Error(THISMODULE, "Unable to find board '%s'", board_name);
    return NULL;
  }

  MTN_Channel *mtnch = NULL;

  if (module->GetFreeChannel() == 0 || !module->IsEnabled() || !module->IsRunning()) {
    CLog::Debug(THISMODULE, "Unable to create channel from board '%s': "
        "no more channels available or module is disabled", board_name);
    return NULL;
  }

  mtnch = module->AllocateChannel();

  if (!mtnch) {
    CLog::Error(THISMODULE, "Unable to create channel from board '%s'", board_name);
    return NULL;
  }

  ////return new MTN_Rtp_Channel(itf, module, mtnch); // commented by ippbx
}

void MTN_Rtp_Channel_Factory::Release(const char *board_name, MTN_Rtp_Channel *ch)
{
  Mapletree_Module *module = Mapletree_Module_Manager::Instance()->FindModule(board_name);
  if (!module) {
    CLog::Error(THISMODULE, "Unable to find board '%s'", board_name);
    return;
  }

  // by deleting the channel, channel is automatically released to the
  // module by destructor of Mapletree_Channel
  delete ch;
}

////////////////////////////////////////////////////////////////////////////////
/** Implementation of MTN_Rtp_Channel
*/
  MTN_Rtp_Channel::MTN_Rtp_Channel(Rtp_Interface *rtp_itf, Mapletree_Module* mod, MTN_Channel* chan) 
: Mapletree_Channel(mod, chan),
  rtp_itf_(rtp_itf),
  state_(MRC_Idle),
  switchdevice_(NULL),
  rtpstream_(this, false), g723low_(false), 
  rtcp_callback_(this), mobset_callback_(this), 
  rtpcallback_(NULL), lastactive_(0)
{
  //Is we're not using host eth, acquire the board eth address
  if (!IsUsingHostEth()) {
    state_ = MRC_GetIpAddr;
    ip_mob_.set_user_notification(&mobset_callback_);
    chan->get_mob_async(&ip_mob_);	
  }

  ResetStatisticAparatus();
  MTN_Rtp_Manager::Instance()->AddChannel(this);

  // Create the switch device early here, so that the MTN channel
  // can occupy one timeslot as early as possible. Otherwise probably the
  // channel will transmit to arbitrary timeslot, which is bad. (benny)
  switchdevice_ = CreateSwitchDevice(rtp_itf_->GetName());
}

MTN_Rtp_Channel::~MTN_Rtp_Channel()
{
  if (IsUsingHostEth())
    rtpstream_.Close();

  GetChannel()->set_callback(NULL, false);
  MTN_Rtp_Manager::Instance()->RemoveChannel(this);
}

/** Set the MTN port to Alocated state. This is an asynch prosess
  the state completion handled in CallbackNotification() 
  */
void MTN_Rtp_Channel::AllocatePort()
{
  CLog::Debug(THISMODULE, "%s: Allocate port (set to PortState_ALLOCATED)", GetChannelName());
  state_ = MRC_Allocating;
  GetChannel()->force_state_async(PortState_ALLOCATED, 0, &mobset_callback_);
}

/** Determine whether the board has ethernet interface or not.
  This should be detected by MTN_Lib
  */
bool MTN_Rtp_Channel::IsUsingHostEth()
{    
  return !GetChannel()->get_module()->has_capability(MTN_Module::ON_BOARD_ETHERNET);
}

/** Set the codec to be used with this channel
  This is only applicable before the channel activated.
  */
void MTN_Rtp_Channel::SetCodec(Rtp_Payload_Code code)
{
  config_.RTP.Codec = code;
}

/** Get the codec used by this channel     
*/
Rtp_Payload_Code MTN_Rtp_Channel::GetCodec()
{
  return config_.RTP.Codec;
}

/** Open the channel by given config.
  This means, in MTN, setting the channel to allocated state.
  */
bool MTN_Rtp_Channel::Open(RTP_Config& config)
{    
  config_ = config;
  probation_ = 3;

  AllocatePort();

  if (rtpstream_.GetLocalRtpPort() == 0) {
    if (!rtpstream_.Open())
      return false;
  }

  if (!RTP_Pci_Stream_Thread::instance()->add_channel(this)) {
    rtpstream_.Close();
    return false;
  }

  return true;
}

/** Asynch activate completion. 
  Called when the activation is asynch and the port is ready
  */
bool MTN_Rtp_Channel::OnActivate()
{
  if (state_ == MRC_Allocated) {
    CLog::Debug(THISMODULE, "%s: Async port is ready, activating %s:%d", 
        GetChannelName(), remoteaddr_.get_host_addr(), remoteaddr_.get_port_number());

    if (!CommitPortSetting())
      return false;

    MTN_Rtp_Manager::Instance()->ChannelActivated(this);
    return true;
  }
  else if (state_ == MRC_Terminating) {
    return true;
  }
  else
    return false;
}

/** Activate the channel using codec, addr.
  If the channel isn't ready by the time this function calles,
  asynh activation is performed. OnActivate will be called when the chanel ready
  */
bool MTN_Rtp_Channel::Activate(const ACE_INET_Addr& addr, Rtp_Payload_Code codec, bool low)
{        
  config_.RTP.Codec = codec;
  g723low_ = low;
  remoteaddr_ = addr;

  if (state_ == MRC_Allocated) {
    CLog::Debug(THISMODULE, "%s: Activating %s:%d localport %d", 
        GetChannelName(),
        remoteaddr_.get_host_addr(), remoteaddr_.get_port_number(), 
        rtpstream_.GetLocalRtpPort());
    MTN_Rtp_Manager::Instance()->ChannelActivated(this);
    if (!CommitPortSetting())
      return false;
  } else
    MTN_Rtp_Manager::Instance()->AddActivatingChannel(this);

  time(&lastactive_);
  return true;
}

/** Lazzy contruction of MapleTree_SwitchDevice. 
  The switch device will be destroyed by Media
  */
SwitchDevice *MTN_Rtp_Channel::GetSwitchDevice(const char* name)
{
  if (!switchdevice_) {
    switchdevice_ = CreateSwitchDevice(rtp_itf_->GetName());
  }

  return switchdevice_;
}

/** Close the channel.
  Switchdevice is deleted by Media, so we don;t have to delete it
  */
void MTN_Rtp_Channel::Close()
{

  ResetStatisticAparatus();

  // Unregister from thread.
  if (rtpstream_.GetLocalRtpPort() != 0)
    RTP_Pci_Stream_Thread::instance()->remove_channel(this);

  // close the socket    
  if (IsUsingHostEth()) {
    rtpstream_.Close();
  }

  ACE_INET_Addr addr;
  remoteaddr_ = addr;

  // set the switch device to NULL
  // switch device is deleted by media
  switchdevice_=NULL;

  // set the channel to terminating
  GetChannel()->set_callback(NULL, false);

  state_ = MRC_Terminating; 
  if (state_!=MRC_Idle) {
    GetChannel()->force_state_async(PortState_TERMINATING, 0, &mobset_callback_);
  } else {
    if (rtpcallback_)
      rtpcallback_->OnTerminated(this);
  }
}

/** Provided to support compatibility with AcChannel     
*/
int MTN_Rtp_Channel::GetInitializationStatus() const
{      
  return lastactive_ ? 2 : 0;
}

time_t MTN_Rtp_Channel::GetLastActivateTime()
{
  return lastactive_;
}

void MTN_Rtp_Channel::DialDtmf(const char* dtmf)
{
  MTN_Tone_Gen_Mob_Data dtmfmob;

  dtmfmob.set_tones(dtmf);
  dtmfmob.set_user_notification(&mobset_callback_);
  state_ = MRC_DTMF_Send;

  GetChannel()->set_mob_async(&dtmfmob);
}

/** Provided to support compatibility with AcChannel     
*/
void MTN_Rtp_Channel::ReleaseChannel(CallDirection cd, CallParameter* cp, CallCause cc)
{

}

const sockaddr_in& MTN_Rtp_Channel::GetLocalAddress()
{
  static sockaddr_in addr;

  ACE_INET_Addr inetaddr;
  GetLocalAddress(inetaddr);

  //return *inetaddr.getsockaddr_in(); //Yoga
  const sockaddr_in *sck_addr_in = (sockaddr_in *)inetaddr.get_addr();
  return *sck_addr_in;
}

const void MTN_Rtp_Channel::GetLocalAddress(ACE_INET_Addr& addr)
{
  if (IsUsingHostEth()) {	
    if (rtpstream_.GetLocalRtpPort() == 0) {
      rtpstream_.Open();
    }

    addr = ACE_INET_Addr(rtpstream_.GetLocalRtpPort(), 
        MTN_Rtp_Manager::Instance()->GetLocalIP());

  } else {
    //assert(!"MTN non rtp-host get-local-address not implemented");
    CLog::Fatal(THISMODULE, "MTN_Rtp_Channel::GetLocalAddress: local address is not set!");
    boardaddr_.set_port_number(rtpstream_.IncRtpPort());
    addr = boardaddr_;
  }
}

Rtp_Payload_Code MTN_Rtp_Channel::GetSelectedPayload()
{
  return config_.RTP.Codec;
}

/** Provided to support compatibility with AcChannel     
*/    
bool MTN_Rtp_Channel::PrepareSwitching(RTP_Config* conf)
{

  return true;
}

/** Provided to support compatibility with AcChannel     
*/
int MTN_Rtp_Channel::GetMediaID()
{
  return (int)this;
} 

int MTN_Rtp_Channel::GetChannelNumber(void)
{
  MTN_Channel *mtn_ch = Mapletree_Channel::GetChannel();
  if (mtn_ch)
    return mtn_ch->get_channel_id();

  return 0;
}

const AcRtcpStat& MTN_Rtp_Channel::GetRtcpStat()
{    
  return rtcpstat_;    
}

const RTP_Config& MTN_Rtp_Channel::GetRtpConfig()
{
  return config_;
}

void MTN_Rtp_Channel::SetRtpConfig(RTP_Config& config)
{
  config_ = config;
}

/** Callback handler for messages from the channel.
*/
int MTN_Rtp_Channel::on_message( MTN_Queue_Type queue_id, const MTN_Message * msg)
{
  if (!IsUsingHostEth())
    return 0;

  if (last_send_rtcp_ == 0) {
#ifndef __linux__
    last_send_rtcp_ = ::GetTickCount();
#else
    last_send_rtcp_ = GetTickCount();
#endif
  } else if ((GetTickCount() - last_send_rtcp_) > FIVE_SECOND) {
#ifndef __linux__
    last_send_rtcp_ = ::GetTickCount();
#else
    last_send_rtcp_ = GetTickCount();
#endif
    rtcpstat_mob_.clear();
    rtcpstat_mob_.set_user_notification(&rtcp_callback_);
    GetChannel()->get_mob_async(&rtcpstat_mob_);
    GetChannel()->update_voice_status();
  }

  if (queue_id == MTN_Q_DataNotification && msg->get_type() == MsgType_N_DATA) {
    char* udp_data = (char*)msg->get_buffer();

    rtp_hdr_t *rtp = (rtp_hdr_t*)udp_data;

    // Check RTP version.
    if (rtp->version != 2) {
      CLog::Fatal(THISMODULE, "%s: RTP packet with invalid RTP version from board (dword=%08x)", 
          GetChannelName(), *(unsigned long*)rtp);
      return 0;
    }
    // Check payload type. 13 is for CNG packet for G.711
    if (rtp->pt != config_.RTP.Codec && rtp->pt != 13) {
      CLog::Fatal(THISMODULE, "%s: RTP packet with bad payload type from board (pt=%d, expected:%d)", 
          GetChannelName(), rtp->pt, config_.RTP.Codec);
      return 0;
    }

    int len = msg->get_arg1();
    packets_tx_++;
    octets_tx_ += len;

    // Get seq number
    unsigned seq = *(unsigned short*)(udp_data+2);
    seq = ntohs(seq);

    if (rtp_seq_no_ ==0)
      rtp_seq_no_ = seq;
    else {
      if (seq == 0) {
        rtp_seq_no_ = seq;
      } else if (seq < rtp_seq_no_+1 && rtp_seq_no_ - seq > 20) {
        rtp_seq_no_ = seq;
      } else if (seq < rtp_seq_no_+1) {
        // Duplicate frame!
        CLog::Debug(THISMODULE, "%s: Old RTP packet received from board (seq=%u, expecting=%u)", 
            GetChannelName(), seq, rtp_seq_no_+1);
        return 0;
      } else if (seq != rtp_seq_no_+1) {
        CLog::Debug(THISMODULE, "%s: Invalid RTP Seq %u received from board (expected: %u)", 
            GetChannelName(), seq, rtp_seq_no_+1);
        rtp_seq_no_ = seq;
      } else
        ++rtp_seq_no_;
    }

    if (len > 0) {
#ifdef XGATE_MTN_HAS_JIITER_SIM
      /* We're simulating jitter/delay */
      Jitter_Sim_Packet jp;
      DWORD now;
      int jitter;

      now = GetTickCount();
      jp.pkt = malloc(len);
      memcpy(jp.pkt, udp_data, len);
      jp.size = len;
      jp.arrival_time = now;

      if (ntohs(rtp->seq) & 0x01) {
        jitter = (NET_TX_DELAY_MAX - NET_TX_DELAY_MIN);
      } else {
        jitter = 0;
      }

      jp.tx_rx_time = now + NET_TX_DELAY_MIN + jitter;

      net_tx_queue_.push_back(jp);

      List<Jitter_Sim_Packet>::iterator it, end;
      end = net_tx_queue_.end();

      for (it=net_tx_queue_.begin(); it != end;) {
        Jitter_Sim_Packet & t = *it;
        if (now >= t.tx_rx_time) {
          //CLog::Info(THISMODULE, "TX pkt delayed by %d ms", now - t.arrival_time);
          if (rand() % 100 >= NET_TX_PERCENT_LOST) {
            rtpstream_.SendToEthernet((char*)t.pkt,  t.size, 
                RTP_Pci_Stream::PTYPE_RTP);
          }
          free( t.pkt );
          it = net_tx_queue_.erase(it);
        } else {
          //break;
          ++it;
        }
      }
#else
      rtpstream_.SendToEthernet(udp_data,  len, RTP_Pci_Stream::PTYPE_RTP);
#endif
    }

  } else if (queue_id == MTN_Q_TrapNotification && 
      msg->get_arg0() == Trap_PORT_STATE_CHANGE &&
      GetChannel()->get_state() == PortState_VOICE) 
  {
    if (state_ != MRC_Initiating)
      CLog::Fatal(THISMODULE, "%s: port state changed to VOICE in state %d",
          GetChannelName(), state_);
    OnMobSetNotification(true);
  }

  return 0;
}

bool MTN_Rtp_Channel::OnRecvRtp(rtp_hdr_t *rtp, unsigned size)
{
  if (rtp->pt == RTP_PT_RED) {
    MTN_Event_Over_Rtp_Receiver *er = GetChannel()->get_event_over_rtp_receiver();
    if (er) {
      rtp_event_queue_info info[32];
      int count;

      count = er->extract_events(rtp, size, info, 32);
      if (count > 0)
        er->handle_events(info, count);

    }
    return false;

  } else {
    /* Accept the correct payload type or 13 (CNG) */
    /*if (rtp->pt == config_.RTP.Codec || rtp->pt == 13) {*/
    int seq;
    seq = ntohs(rtp->seq);

    if (probation_) {
      if (seq == last_seq_+1)
        --probation_;
      else
        probation_ = 3;

      last_seq_ = seq;
      /*return false;*/
      return true;

    } else {
      if (seq > last_seq_) {
        int lost = seq - last_seq_ - 1;
        packets_rx_cum_lost_ += lost;
        packets_rx_frac_lost_ += lost;
        last_seq_ = seq;
        packets_rx_frac_ += lost;
      } else if (seq < last_seq_ && last_seq_ - seq > 100) 
        last_seq_ = seq;

      ++packets_rx_;
      ++packets_rx_frac_;
      octets_rx_ += size;
      return true;
    }

    /*} */
  }
}

bool MTN_Rtp_Channel::OnRecvRtcp(const rtcp_t *rtcp)
{
  const rtcp_rr_t *rr_rec = NULL;

  if (rtcp->common.version != 2) {
    CLog::Debug(THISMODULE, "%s: Received RTCP packet from network with invalid version (dword=%08x)", 
        GetChannelName(), *(unsigned long*)rtcp);
    return false;
  }

  if (rtcp->common.pt == RTCP_SR) {
    /* Update lsr_recv_ */
    lsr_recv_.sec = ntohl(rtcp->r.sr.ntp_sec);
    lsr_recv_.frac = ntohl(rtcp->r.sr.ntp_frac);

    struct timeval now;
    ntp_time_t ntp_now;
    get_timeofday(&now, NULL);
    ntp64_time(&now, &ntp_now);
    lsr_recv_time_ = ntp_now;

    /* update octet and packet sent */
    rtcpstat_.rx.octetCount = ntohl(rtcp->r.sr.osent);
    rtcpstat_.rx.packetCount = ntohl(rtcp->r.sr.psent);

    if (rtcp->common.count)
      rr_rec = &rtcp->r.sr.rr[0];

  } else if (rtcp->common.pt == RTCP_RR) {
    rr_rec = &rtcp->r.rr.rr[0];
  }

  if (rr_rec) {
    if (rr_rec->ssrc != our_ssrc_ && our_ssrc_!=(unsigned)-1) {
      CLog::Debug(THISMODULE, "%s: Received RTCP packet from network with invalid SSRC (%08x, ours=%08x)", 
          GetChannelName(), rr_rec->ssrc, our_ssrc_);
      return false;
    }

    rtcpstat_.tx.ready = true;
    float cum_lost = 0, frac_lost = 0;
    unsigned fraction_lost = 0;
    long n_packet_lost = 0;

    const unsigned char *dump = &rr_rec->fraction;

    /*
       CLog::Info( THISMODULE, "RX RTCP %s: dump at 0x%x: 0x%02x 0x%02x 0x%02x 0x%02x",
       (rtcp->common.pt == RTCP_SR ? "SR" : "RR"),
       dump-(unsigned char*)rtcp,
     *(dump+0) & 0xFF, *(dump+1) & 0xFF, 
     *(dump+2) & 0xFF, *(dump+3) & 0xFF);
     */


    n_packet_lost = 0;
    n_packet_lost |= (*(dump+3) & 0xFF);
    n_packet_lost |= ((*(dump+2) & 0xFF)<<8);
    n_packet_lost |= ((*(dump+1) & 0xFF)<<16);
    if ((*(dump+1) & 0xFF) & 128) // test the highest bit (to check if it's a negative number)
      n_packet_lost = 0;

    /* Count packet rx loss and get pact tx loss */
    if (packets_tx_ && n_packet_lost <= packets_tx_) {
      cum_lost = n_packet_lost * 1.0 / packets_tx_;
      rtcpstat_.tx.cummulativeLost = n_packet_lost;
    }
    //CLog::Info(THISMODULE, "RX SR/RR: Cumulative packet lost: %u", n_packet_lost);
    fraction_lost = rr_rec->fraction;
    frac_lost = fraction_lost * 1.0 / 256;
    rtcpstat_.tx.fractionLost = frac_lost;

    /* Update jitter. */
    unsigned jitter = ntohl(rr_rec->jitter) / 8;
    //CLog::Info(THISMODULE, "RX SR/RR, jitter=%d", jitter);
    if (jitter < 1000) {
      if (rtcpstat_.tx.n_jitter == -1)
        rtcpstat_.tx.n_jitter = 0;

      rtcpstat_.tx.jitter = 
        (rtcpstat_.tx.jitter * rtcpstat_.tx.n_jitter + jitter) /
        (rtcpstat_.tx.n_jitter+1);
      ++rtcpstat_.tx.n_jitter;
    }

    /* Calculate round-trip */
    unsigned rtt = 0;
    if (rr_rec->lsr) {
      ntp_time_t ntp_lsr, ntp_dlsr, ntp_now_fixed, ntp_rtt;
      unsigned temp32;
      struct timeval now;

      // Get NTP time, but clear the high 16 bits.
      get_timeofday(&now,NULL);
      ntp64_time(&now, &ntp_now_fixed);
      ntp_now_fixed.sec &= 0xFFFF;
      ntp_now_fixed.frac &= 0xFFFF0000;

      // Convert LSR to NTP.
      // LSR is the middle 32bit of NTP timestamp.
      temp32 = ntohl(rr_rec->lsr);
      ntp_lsr.sec = (temp32 >> 16);
      ntp_lsr.frac = (temp32 << 16);

      // Convert DLSR to NTP.
      // DLSR is in 1/65536 sec.
      temp32 = ntohl(rr_rec->dlsr);
      ntp_dlsr.sec = temp32 / 65536;
      ntp_dlsr.frac = (temp32 % 65536) * 65536;

      // Calculate RTT (still in NTP timestamp).
#ifdef TRACE_RTCP
      CLog::Info(THISMODULE, "[%d] RTT_CALC SSRC=%u: now=%u.%u, lsr=%u.%u, dlsr=%u.%u",
          boardaddr_.get_port_number(),
          ntohl(rr_rec->ssrc),
          ntp_now_fixed.sec, (ntp_now_fixed.frac >> 16) * 1000 / 65536, 
          ntp_lsr.sec, (ntp_lsr.frac >> 16) * 1000 / 65536, 
          ntp_dlsr.sec, (ntp_dlsr.frac >> 16) * 1000 / 65536);
#endif
      ntp_rtt = ntp_now_fixed - ntp_lsr - ntp_dlsr;

      // Convert RTT in miliseconds.
      rtt = (unsigned) (ntp_rtt.sec * 1000 +
          ((ntp_rtt.frac >> 16) * 1000 / 65536));

      // average round-trip
      if (rtt < 3000) {
        double g_rtt = 0;
        if (rtcpstat_.rx.n_roundTrip != 0) {
          // Must be like this, because initially roundTrip is set to
          // NAND value, which can't be multiplied even with zero.
          g_rtt = rtcpstat_.rx.n_roundTrip;
          g_rtt = g_rtt * rtcpstat_.rx.roundTrip;
        }
        g_rtt = (g_rtt + rtt) / (rtcpstat_.rx.n_roundTrip + 1);

        ++rtcpstat_.rx.n_roundTrip;
        ++rtcpstat_.tx.n_roundTrip;

        rtcpstat_.rx.roundTrip = g_rtt;
        rtcpstat_.tx.roundTrip = g_rtt;
      }
    }
#ifdef TRACE_RTCP
    CLog::Info(THISMODULE, "[%d] RX %s %u: TX pkt=%u, cumlost=%f (%u/0x%x), fraclost=%f(%d), jit=%d, rtt=%d",
        boardaddr_.get_port_number(),
        (rtcp->common.pt == RTCP_SR ? "RTCP_SR" : "RTCP_RR"),
        ntohl(rr_rec->ssrc),
        packets_tx_,
        cum_lost, n_packet_lost, n_packet_lost,
        frac_lost, fraction_lost,
        jitter, rtt);
#endif
  }

  return true;
}

bool MTN_Rtp_Channel::OnSendRtcp(rtcp_t *rtcp)
{
  rtcp_rr_t *rr_rec = NULL;

  if (rtcp->common.version != 2) {
    CLog::Fatal(THISMODULE,  "%s: Received RTCP packet from board with invalid version (dword=%08x)", 
        GetChannelName(), *(unsigned long*)rtcp);
    return false;
  }

  if(rtcp->common.pt == RTCP_SR) {
    /* update octet and packet sent */
    rtcpstat_.tx.octetCount = ntohl(rtcp->r.sr.osent);
    rtcpstat_.tx.packetCount = ntohl(rtcp->r.sr.psent);

    if (rtcp->common.count)
      rr_rec = &rtcp->r.sr.rr[0];

    if (our_ssrc_ == (unsigned)-1) {
      our_ssrc_ = rtcp->r.sr.ssrc;
    } else {
      if (rtcp->r.sr.ssrc != our_ssrc_) {
        CLog::Debug(THISMODULE, "%s: Received RTCP packet from board with invalid SSRC (%08x, ours=%08x)", 
            GetChannelName(), rtcp->r.sr.ssrc, our_ssrc_);
        return false;
      }
    }

  } else if (rtcp->common.pt == RTCP_RR ) {
    rr_rec = &rtcp->r.rr.rr[0];
  }

  if (rr_rec) {
    rtcpstat_.rx.ready = true;

    /* Jitter is in timestamp unit */
    unsigned jitter = ntohl(rr_rec->jitter) / 8;
    if (rtcpstat_.rx.n_jitter == -1)
      rtcpstat_.rx.n_jitter = 0;

    if (jitter < 1000) {
      rtcpstat_.rx.jitter = (rtcpstat_.rx.jitter * rtcpstat_.rx.n_jitter + jitter) /
        (rtcpstat_.rx.n_jitter+1);
      ++rtcpstat_.rx.n_jitter;
      CLog::Debug(THISMODULE, "%s: jitter value from board: %d ms (n=%d)", 
          GetChannelName(), jitter, rtcpstat_.rx.n_jitter);
    } else {
      CLog::Debug(THISMODULE, "%s: Probably invalid jitter value from board: %d ms (n=%d)", 
          GetChannelName(), jitter, rtcpstat_.rx.n_jitter);
    }

    long n_packet_lost;
    unsigned fraction_lost;

#ifdef USE_OWN_LOST_CALC
    n_packet_lost = packets_rx_cum_lost_;

    /* Update RR fraction and cumulative lost with our record */
    rr_rec->lost[2] = n_packet_lost & 0xFF;
    rr_rec->lost[1] = (n_packet_lost>>8) & 0xFF;
    rr_rec->lost[0] = (n_packet_lost>>16) & 0xFF;
    if (packets_rx_frac_)
      fraction_lost = packets_rx_frac_lost_ * 256 / packets_rx_frac_;
    else
      fraction_lost = 0;
    rr_rec->fraction = (unsigned char)fraction_lost;
#endif

    /* Reset fraction lost counter */
    packets_rx_frac_lost_ = 0;
    packets_rx_frac_ = 0;

    /* Update quality for fraction and cumulative lost */
    float cum_lost = 0, frac_lost = 0;

    const unsigned char *dump = &rr_rec->fraction;
    n_packet_lost = 0;
    n_packet_lost |= (*(dump+3) & 0xFF);
    n_packet_lost |= ((*(dump+2) & 0xFF)<<8);
    n_packet_lost |= ((*(dump+1) & 0xFF)<<16);
    if ((*(dump+1) & 0xFF) & 128) // test the highest bit (to check if it's a negative number)
      n_packet_lost = 0;

    if (packets_rx_ && n_packet_lost < packets_rx_) {
      cum_lost = n_packet_lost * 1.0 / packets_rx_;
      rtcpstat_.rx.cummulativeLost = n_packet_lost;
    }
    fraction_lost = rr_rec->fraction;
    frac_lost = fraction_lost * 1.0/256;
    rtcpstat_.rx.fractionLost = frac_lost;

    /*
     * Calculate LSR and DLSR to be sent in SR.
     */
    struct timeval now;
    unsigned lsr, dlsr;
    ntp_time_t ntp_now, ntp_dlsr;

    get_timeofday(&now,NULL);
    ntp64_time(&now, &ntp_now);

    //Get LSR values
    if (lsr_recv_.sec == 0) {
      lsr = 0;
      dlsr = 0;
    } else {
      /* LSR is the middle 32bit of NTP timestamp */
      lsr = ((lsr_recv_.sec & 0xFFFF) << 16) + (lsr_recv_.frac >> 16);

      /* Calculate DLSR, in NTP time scale */
      ntp_dlsr = ntp_now - lsr_recv_time_;

#ifdef TRACE_RTCP
      CLog::Info(THISMODULE, "[%d] SEND RTCP %u: now=%u, lsr=%u, dlsr=%u", 
          boardaddr_.get_port_number(),
          ntohl(rr_rec->ssrc),
          ntp_now.sec & 0xFFFF, lsr >> 16, ntp_dlsr.sec);
#endif

      /* DLSR is in 1/65536 secs */
      dlsr = (unsigned)(ntp_dlsr.sec * 65536.0 + ntp_dlsr.frac * 65536.0 / 0xFFFFFFFF);
    }    

    lsr_sent_ = ntp_now;

    if (rtcp->common.pt == RTCP_SR) {
      rtcp->r.sr.ntp_sec = htonl(ntp_now.sec);
      rtcp->r.sr.ntp_frac = htonl(ntp_now.frac);
    }

    rr_rec->lsr  = htonl(lsr);
    rr_rec->dlsr = htonl(dlsr);

#ifdef TRACE_RTCP
    CLog::Info(THISMODULE, "[%d] TX %s %u: RX pkt=%u, cumlost=%f (%d/0x%x), fraclost=%f (%d), jit=%d",
        boardaddr_.get_port_number(),
        (rtcp->common.pt == RTCP_SR ? "RTCP_SR" : "RTCP_RR"),
        ntohl(rr_rec->ssrc),
        packets_rx_,
        cum_lost, n_packet_lost, n_packet_lost,
        frac_lost, fraction_lost,
        jitter);
#endif
  }

  return true;
}

void MTN_Rtp_Channel::SendSR(int srsize)
{
  /** Reply from the board for retrieving SSRC block(s)
  */
  rtcp_t *rtcp;

  unsigned char* lst = (unsigned char*)rtcpstat_mob_.get_rtcp();
  /** Move the bytes stream two bytes forward to skip proprietary msg 
    from Mapletree
    */
  lst++;lst++;

  rtcp = (rtcp_t*) lst;
  if (OnSendRtcp(rtcp))
    rtpstream_.SendToEthernet((const char*)rtcp, sizeof(*rtcp),RTP_Pci_Stream::PTYPE_RTCP);
}

/** Handler for asynch MTN commands/messages.
  Idle -(allocate)->Allocated
  Allocated->(set port)->Allocated->(set voice)->Allocated->(Set RTP)->Allocated->(Init)->Voice
  */
void MTN_Rtp_Channel::OnMobSetNotification(bool statusok)
{    
  switch(state_) {
    case MRC_DTMF_Send:
      state_ = MRC_Voice;
      break;
    case MRC_Allocating:
      if (GetChannel()->get_state() != PortState_ALLOCATED) {
        CLog::Warning(THISMODULE, "%s: set state failed, state=%s",
            GetChannelName(), 
            MTN_Util::port_state_name(GetChannel()->get_state()));
        return;
      } else {
        CLog::Debug(THISMODULE, "%s: port state success (PortState_ALLOCATED)", GetChannelName());
      }
      if (IsUsingHostEth()) 
        GetChannel()->set_callback(this, false);

      state_ = MRC_Allocated;	
      break;
    case MRC_SettingPort:
      if (statusok) {
        CLog::Debug(THISMODULE, "%s: parameters set up successfull. Initiating..", GetChannelName());
      } else {
        CLog::Warning(THISMODULE, "%s: port settings failed.", GetChannelName());
      }
      InitiatePort();
      break;
    case MRC_Initiating:
      if ( GetChannel()->get_state() == PortState_VOICE ) {
        CLog::Debug(THISMODULE, "%s: initiated successfully. Port state is VOICE.", GetChannelName());
        state_ = MRC_Voice;
        if (IsUsingHostEth())
          rtpstream_.Activate(remoteaddr_.get_host_addr(), 
              remoteaddr_.get_port_number());
      } else {
        CLog::Warning(THISMODULE, "%s: OnMobSetNotification() in state MRC_Initiating "
            "while port state is %s", 
            GetChannelName(), 
            MTN_Util::port_state_name(GetChannel()->get_state()));
      }
      break;
    case MRC_Terminating:	
      state_ = MRC_Idle;
      MTN_Rtp_Manager::Instance()->ChannelDeactivated(this);
      if (rtpcallback_)
        rtpcallback_->OnTerminated(this);
      break;
    case MRC_GetIpAddr:

      state_ = MRC_Idle;

      if (statusok)
        boardaddr_.set(ip_mob_.get_ip_addr());

    default:	
      break;
  }
}

/** Commit channel port setting
*/
bool MTN_Rtp_Channel::CommitPortSetting()
{
  assert(GetChannel()->get_state() == PortState_ALLOCATED);
  CLog::Debug(THISMODULE, "%s: Setting up port parameters", GetChannelName());

  state_ = MRC_SettingPort;

  portset_.set_country_code(GetModule()->GetCountryCode());
  MTN_Call_Types call_types;
  call_types.reset();
  call_types.set_bit(CallType_VOICE);
  portset_.set_call_types(call_types );
  portset_.set_call_role(CallRole_ANSWER);

  int ms=20;
  portset_.set_voice_input_gain( config_.InputGain );
  portset_.set_voice_output_gain( config_.OutputGain );
  portset_.set_voice_encapsulation_type(VoiceEncap_RTP);
  portset_.set_voice_encoding_type(
      TranslatePayload(config_.RTP.Codec, g723low_,&ms, config_.RTP.PackingFactor));

  portset_.set_vad_cng_enabled(config_.SilenceCompression);
  portset_.set_voice_upstream_mute(false);
  portset_.set_dtmf_squelched(false);
  //portset_.set_dtmf_volume(200);    
  portset_.set_packet_interval(ms);

  portset_.set_jib_hold_time_min(MTN_Time_Value(0, config_.RTP.MinJitter * 1000));
  portset_.set_jib_hold_time_max(MTN_Time_Value(0, 200 * 1000));
  portset_.set_jib_hold_time_goal(MTN_Time_Value(0, (config_.RTP.MinJitter+60) * 1000));
  portset_.set_echo_canc_span(32);
  portset_.set_echo_canc_enabled(true);
  portset_.set_rtt_estimate(120);   // must be zero, GEN_FAILURE otherwise

  /*
     unsigned long r_addr = inet_addr(remoteaddr_.get_host_addr());
     portset_.set_local_udp_port(boardaddr_.get_port_number());
     portset_.set_remote_addr(r_addr);
     portset_.set_remote_udp_port(remoteaddr_.get_port_number());
     portset_.set_data_path_to_eth(false);
     */

  portset_.set_user_notification(&mobset_callback_);

  return GetChannel()->set_mob_async(&portset_);
}

/** Activate the channel, set it to initiate state.
  When ready, it'll changed to Voice state
  */
void MTN_Rtp_Channel::InitiatePort()
{
  state_ = MRC_Initiating;
  GetChannel()->set_callback(this, false);
  GetChannel()->force_state_async(PortState_INITIATING, 0, NULL);    // &mobset_callback_
}

/** Static member to translate Codec code 
*/
VoiceEncodingT  MTN_Rtp_Channel::TranslatePayload(Rtp_Payload_Code code, 
    bool low, int* ms, int packingfactor)
{
  switch(code) {
    case RTP_PT_PCMU:
      *ms = 20;
      if (packingfactor != 1) {
        CLog::Info(THISMODULE, "%s: PCMU multiplication is set to 1", GetChannelName());
      }
      return VoiceEncoding_G711_ULAW;
      break;
    case RTP_PT_PCMA:
      *ms = 20;
      if (packingfactor != 1) {
        CLog::Info(THISMODULE, "%s: PCMA multiplication is set to 1", GetChannelName());
      }
      return VoiceEncoding_G711_ALAW;
      break;
    case RTP_PT_G729:
      *ms = 20*packingfactor;
      return VoiceEncoding_G729_A;
      break;
    case RTP_PT_G7231:
      *ms = 30*packingfactor;
      if (low)
        return VoiceEncoding_G7231_53;
      else
        return VoiceEncoding_G7231_63;	
      break;
    default :
      CLog::Error(THISMODULE, "%s: The codec specified in RTP interface "
          "is not supported by mtn module", GetChannelName());
      *ms = 20*packingfactor;
      return VoiceEncoding_G729_A;
  }
}

/** Method for manual bus read by the dispathcer
*/
int  MTN_Rtp_Channel::ReadFromBus(char** packet, int* dtype)
{
  return 0;
}

/** Method to put received packet from eth to pci bus
*/
void MTN_Rtp_Channel::WriteToBus(const char* packet, int len, int dtype)
{
  bool status = false;

  /** When switchdevice is null, the channel state is already invalid        
  */
  if (switchdevice_== 0)
    return;

  /** Only write when the length of the packet is more than zero
  */
  if (len <= 0)
    return;

  /** Only write message from ethernet to bus if channel in Voice State
  */
  if (GetChannel()->get_state() != PortState_VOICE)
    return;

  if (dtype == RTP_Pci_Stream::PTYPE_RTCP) {	

    rtcp_t *rtcp = (rtcp_t *)packet;
    OnRecvRtcp(rtcp);

  } else {
    /** This is an RTP packet. Increment packet rx count
    */
    rtp_hdr_t *rtp = (rtp_hdr_t *)packet;
    my_rtp_hdr_t *my_rtp = (my_rtp_hdr_t *)packet;
    status = OnRecvRtp(rtp, len);
  }


  if (status) {
#ifdef XGATE_MTN_HAS_JIITER_SIM
    /* We're simulating jitter/delay */
    Jitter_Sim_Packet jp;
    DWORD now;

    now = GetTickCount();
    jp.pkt = malloc(len);
    memcpy(jp.pkt, packet, len);
    jp.size = len;
    jp.arrival_time = now;

    if (NET_RX_DELAY_MAX > NET_RX_DELAY_MIN) {
      rtp_hdr_t *rtp = (rtp_hdr_t *)packet;
      int jitter = 0;

      if (ntohs(rtp->seq) & 0x01) {
        jitter = (NET_RX_DELAY_MAX - NET_RX_DELAY_MIN);
      } else {
        jitter = 0;
      }

      jp.tx_rx_time = now + NET_RX_DELAY_MIN + jitter;
    } else {
      jp.tx_rx_time = now + NET_RX_DELAY_MIN;
    }

    net_rx_queue_.push_back(jp);

    List<Jitter_Sim_Packet>::iterator it, end;
    end = net_rx_queue_.end();

    for (it=net_rx_queue_.begin(); it != end;) {
      Jitter_Sim_Packet & t = *it;
      if (now >= t.tx_rx_time) {
        //CLog::Info(THISMODULE, "RX pkt delayed by %d ms", now - t.arrival_time);
        if (rand() % 100 >= NET_RX_PERCENT_LOST) {
          GetChannel()->send_data_async(t.pkt, t.size);
        }
        free( t.pkt );
        it = net_rx_queue_.erase(it);
      } else {
        //break;
        ++it;
      }
    }

#else
    GetChannel()->send_data_async(packet, len);
#endif
  }
}

void MTN_Rtp_Channel::ResetStatisticAparatus()
{
  lsr_sent_.time64 = lsr_recv_.time64 = lsr_recv_time_.time64 = 0;
  last_send_rtcp_ = 0;
  our_ssrc_ = (unsigned)-1;
  rtp_seq_no_ = 0;

  rtcpstat_.Reset();
  rtcpstat_.rx.n_jitter    = 0;
  rtcpstat_.tx.n_jitter    = 0;
  rtcpstat_.rx.jitter    = 0;
  rtcpstat_.tx.jitter    = 0;
  rtcpstat_.rx.ready	     = true;
  rtcpstat_.tx.ready	     = true;
  rtcpstat_.rx.n_roundTrip = 0;
  rtcpstat_.rx.roundTrip   = 0;
  rtcpstat_.tx.n_roundTrip = 0;
  rtcpstat_.tx.roundTrip   = 0;
  rtcpstat_.rx.fractionLost = 0;
  rtcpstat_.tx.fractionLost = 0;
  rtcpstat_.rx.cummulativeLost = 0;
  rtcpstat_.tx.cummulativeLost = 0;
  rtcpstat_.rx.packetCount = 0;
  rtcpstat_.tx.packetCount = 0;
  rtcpstat_.rx.octetCount = 0;
  rtcpstat_.tx.octetCount = 0;

  packets_rx_ = 0;
  packets_tx_ = 0;
  packets_rx_cum_lost_ = packets_rx_frac_lost_ = 0;
  packets_rx_frac_ = 0;
  last_seq_ = 0;

  octets_rx_ = 0;
  octets_tx_ = 0;

  lastactive_ = 0;
}

const char *   MTN_Rtp_Channel::GetChannelName() const
{
  return GetChannel()->get_channel_name();
}

bool MTN_Rtp_Channel::IsActive() const
{
  return GetChannel()->get_state() != PortState_IDLE; 
}

unsigned short MTN_Rtp_Channel::EthLocalPort() const
{
  return rtpstream_.GetRtpSocketPort();
}

unsigned short MTN_Rtp_Channel::EthRemotePort() const
{
  return remoteaddr_.get_port_number();
}

unsigned long MTN_Rtp_Channel::GetPacketsTx() const
{
  return packets_tx_;
}

unsigned long MTN_Rtp_Channel::GetPacketsRx() const
{
  return packets_rx_;
}

unsigned long MTN_Rtp_Channel::GetOctetsTx() const
{
  return octets_tx_;
}

unsigned long MTN_Rtp_Channel::GetOctetsRx() const
{
  return octets_rx_;
}

const char * MTN_Rtp_Channel::GetPortStateString()
{
  //Not necessary: port state is always synchronized
  //GetChannel()->get_state_async();
  return PortStateToString(GetChannel()->get_state());    
}

const char * MTN_Rtp_Channel::PortStateToString(int state)
{
  static char* status[] = {
    "IDLE",
    "SIG",
    "INIT",
    "LINK",
    "TRAIN",
    "ECNEG",
    "DATA",
    "RSYNC",
    "FAX",
    "CMESC",
    "TERM",
    "VOICE",
    "P_RST",
    "D_RST",
    "ALLOC",
    "OUTSV",
    "RCONF",
    "HOLD",
    "END"
  };

  if (state>19)
    return "UNK";

  return status[state];
}

void MTN_Rtp_Channel::OnRtcpCallback(bool status)
{
  /** Reply from the board for SSRC block(s) status
    This is only available when we have asked the board
    to retrieve this mob.
    */

  if (status){
    CLog::Debug(THISMODULE, "%s: RTCP count %d size %d, %d", 
        GetChannelName(),
        rtcpstat_mob_.get_ssrc_count(),
        rtcpstat_mob_.get_ssrc_size(), sizeof(rtcp_t));

    if (rtcpstat_mob_.get_ssrc_count()>0) {	
      SendSR(rtcpstat_mob_.get_ssrc_size());		    
    }
  } else {
    CLog::Debug(THISMODULE, "%s: Error on getting rtcp", GetChannelName());
  }

  rtcpstat_mob_.clear();

}

void MTN_Rtp_Channel::SetEventCallback(RTP_Channel_Callback *callback)
{
  rtpcallback_ = callback;
}

////////////////////////////////////////////////////////////////////////////////////////
  MTN_Rtp_Channel::MobSet_Callback::MobSet_Callback(MTN_Rtp_Channel * channel)
: channel_(channel),
  MTN_Callback_Notification(MobSet_Callback::callback_notification)
{
}

void MTN_Rtp_Channel::MobSet_Callback::callback_notification(MTN_Callback_Notification * mcn)
{
  MobSet_Callback* pcb = (MobSet_Callback*)mcn;
  pcb->OnNotification();
}

void MTN_Rtp_Channel::MobSet_Callback::OnNotification()
{
  if (get_status() != MsgStatus_OK) {
    CLog::Warning(THISMODULE, "%s: port setting error: %s",
        channel_->GetChannelName(),
        MTN_Util::msg_status_name(get_status()));
  } else {
    channel_->OnMobSetNotification(get_status() == MsgStatus_OK);
  }
}

////////////////////////////////////////////////////////////////////////////////////////
  MTN_Rtp_Channel::Rtcp_Callback::Rtcp_Callback(MTN_Rtp_Channel * channel)
: channel_(channel),
  MTN_Callback_Notification(Rtcp_Callback::callback_notification)
{
}

void MTN_Rtp_Channel::Rtcp_Callback::callback_notification(MTN_Callback_Notification * mcn)
{
  Rtcp_Callback* pcb = (Rtcp_Callback*)mcn;
  pcb->OnNotification();
}

void MTN_Rtp_Channel::Rtcp_Callback::OnNotification()
{
  channel_->OnRtcpCallback(get_status() == MsgStatus_OK);
}

////////////////////////////////////////////////////////////////////////////////
/*
   const VoiceEncodingT MTN_Rtp_Config::ve_pcmu	  = VoiceEncoding_G711_ULAW;
   const VoiceEncodingT MTN_Rtp_Config::ve_pcma	  = VoiceEncoding_G711_ALAW;
   const VoiceEncodingT MTN_Rtp_Config::ve_g729	  = VoiceEncoding_G729_A;
   const VoiceEncodingT MTN_Rtp_Config::ve_g723_53   = VoiceEncoding_G7231_53;
   const VoiceEncodingT MTN_Rtp_Config::ve_g723_63   = VoiceEncoding_G7231_63;
   */
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
  RTP_Pci_Stream::RTP_Pci_Stream(RTP_Pci_Stream_Callback* callback, bool manualread)
: callback_(callback), manual_(manualread),
  rtp_(this), rtcp_(this), localport_(0),
  nat_counter_(0)
{
  //nat_public_rtp_addr_.set(0, 0); //Yoga do we need initialization? what about ACE_INET_Addr constructor
}

void RTP_Pci_Stream::SetManualPCIRead(bool manual)
{
  manual_ = manual;
}

void RTP_Pci_Stream::SendToEthernet(const char* data, int len, RTP_Packet_Type type)
{
  if (localport_ == 0) // this means that the stream is closed already
    return; 

  if (remote_rtp_.get_port_number() == 0) // this means that the stream hasn't been activated
    return; 

  int sent=0;
  if (type == PTYPE_RTP) {

    sent = rtp_.GetSocket().send(data, len, remote_rtp_);

  } else if (type == PTYPE_RTCP) {
    sent = rtcp_.GetSocket().send(data, len, remote_rtcp_);
  }

  if (sent != len) {
    ACE_INET_Addr *p_addr = (type == PTYPE_RTP) ? &remote_rtp_ : &remote_rtcp_;
    CLog::Warning(THISMODULE, "Unable to send %d bytes data to %s:%d (sent=%d)",
        len, p_addr->get_host_addr(), p_addr->get_port_number(), sent);
  }
}

bool RTP_Pci_Stream::Open()
{
  if (!AllocateLocalAddress())
    return false;

  // Set buffer size.
  // int udp_sock = rtp_.GetSocket().get_handle();

  return true;
}

void RTP_Pci_Stream::Close()
{
  localport_=0;
  rtp_.Close();
  rtcp_.Close();
}

enum
{
  RTP_START_PORT = 40000,
  RTP_END_PORT = 41000,
};

unsigned short RTP_Pci_Stream::IncRtpPort()
{
  static unsigned short port = RTP_START_PORT;
  unsigned short res = port;

  port+= 2;
  if (port>=0xfffa)
    port=RTP_START_PORT;

  return res;
}

bool RTP_Pci_Stream::AllocateLocalAddress()
{
  enum { MAX_RETRY = 32 };
  static unsigned short last_port_ = RTP_START_PORT;

  for (int i=0; i<MAX_RETRY; ++i) {
    if (last_port_ >= RTP_END_PORT)
      last_port_ = RTP_START_PORT;

    if (rtp_.Open(last_port_)) {
      if (rtcp_.Open(last_port_ + 1)) {
        localport_ = last_port_;
        last_port_ += 2;
        return true;
      }
      rtp_.Close();
    }
    last_port_ += 2;
  }

  CLog::Error(THISMODULE, "RTP_Pci_Stream: unable to find free UDP ports for RTP and RTCP socket");
  return false;
}

unsigned short RTP_Pci_Stream::GetLocalRtpPort() const
{
  return localport_;
}

bool RTP_Pci_Stream::Activate(const char* remoteip, unsigned short remoteport)
{
  remote_rtp_	=  ACE_INET_Addr(remoteport, remoteip);
  remote_rtcp_ = ACE_INET_Addr(remoteport+1, remoteip);
  return true;
}

void RTP_Pci_Stream::GetLocalAddress(ACE_INET_Addr& addr)
{
  rtp_.GetSocket().get_local_addr(addr);
}

void RTP_Pci_Stream::OnPacketArrived( char* buff, int len, 
    RTP_Udp_Sock* socket,
    ACE_INET_Addr& addr)
{
  assert(callback_);

  /* NAT detection for RTP packet. */
  if (socket == &rtp_ && remote_rtp_.get_port_number() != 0) {

    /* Only detect NAT if IP address is different. Note that if only the
     * port number is different, then we won't consider it as NAT.
     */
    if (addr.get_ip_address() != remote_rtp_.get_ip_address()) {
      if (addr.get_ip_address() == nat_public_rtp_addr_.get_ip_address()) {
        nat_counter_++;
        if (nat_counter_ >= 5) {
          /* We've got consecutive packets with different IP.
           * Switch transmission to the source address.
           */
          String private_addr = remote_rtp_.get_host_addr();
          String public_addr = addr.get_host_addr();

          CLog::Info(THISMODULE, "NAT Address %s:%d detected in RTP packet, switching to public address %s:%d",
              private_addr.c_str(), remote_rtp_.get_port_number(),
              public_addr.c_str(), addr.get_port_number());
          remote_rtp_ = nat_public_rtp_addr_;
          nat_counter_ = 0;
          //nat_public_rtp_addr_.set(0, 0); //Yoga do we need to re-initialize, what about ACE_INET_Addr constructor?
        }
      } else {
        nat_public_rtp_addr_ = addr;
        nat_counter_ = 1;
      }
    }
  }

  /* NAT detection for RTCP packet. */
  if (socket == &rtcp_ &&  remote_rtp_.get_port_number() != 0) {
    if (addr.get_ip_address() != remote_rtcp_.get_ip_address()) {
      /* For RTCP we immediately switch to public address as soon as
       * we receive a packet.
       */
      String private_addr = remote_rtcp_.get_host_addr();
      String public_addr = addr.get_host_addr();
      CLog::Info(THISMODULE, "NAT Address %s:%d detected in RTCP packet, switching to public address %s:%d",
          private_addr.c_str(), remote_rtcp_.get_port_number(),
          public_addr.c_str(), addr.get_port_number());

      remote_rtcp_ = addr;
    }
  }

  callback_->WriteToBus(buff, len, socket == &rtp_?PTYPE_RTP:PTYPE_RTCP);
}

unsigned short RTP_Pci_Stream::GetRtpSocketPort() const
{
  return rtp_.GetPort();
}

///////////////////////////////////////////////////////////////////////////////
/**
*/
  RTP_Udp_Sock::RTP_Udp_Sock(RTP_Pci_Udp_Callback* callback)
: callback_(callback), opened_(false)
{
}

RTP_Udp_Sock::~RTP_Udp_Sock()
{
  Close();    
}

ACE_HANDLE RTP_Udp_Sock::get_handle (void) const
{
  return dgram_.get_handle();
}

int RTP_Udp_Sock::handle_input (ACE_HANDLE fd = ACE_INVALID_HANDLE)
{
  ACE_INET_Addr addr;

  int len = dgram_.recv(buffer_, MAX_BUF, addr, 0, NULL);

  if (len>0) {
    callback_->OnPacketArrived(buffer_, len, this, addr);
    return 0;
  }
  // TODO: should return -1?
  return 0;
}

int RTP_Udp_Sock::handle_close (ACE_HANDLE handle,
    ACE_Reactor_Mask close_mask)
{
  return 0;
}

ACE_SOCK_Dgram& RTP_Udp_Sock::GetSocket()
{
  return dgram_;
}

bool RTP_Udp_Sock::Open(unsigned short port)
{    
  if (dgram_.open(ACE_INET_Addr(port)) != 0)
    return false;    

  opened_ = true;
  return opened_;
}

void RTP_Udp_Sock::Close()
{
  if (!opened_)
    return;

  opened_ = false;
  dgram_.close();    
}

unsigned short RTP_Udp_Sock::GetPort() const
{
  if (!opened_)
    return 0;

  ACE_INET_Addr addr;
  dgram_.get_local_addr(addr);
  return addr.get_port_number();
}

//#endif //XGATE_USE_NEW_RTP
#endif //XGATE_HAS_MTNRTP
#endif //XGATE_HAS_MTN
