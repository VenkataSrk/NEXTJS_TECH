/* $Header: /xGate/audiocodes.cpp 25    4/09/03 11:03a Bennylp $
 *   
 * XGATE
 * (C) 2001 Switchlab, Ltd
 *
 * AudioCodes
 *
 * $Log: /xGate/audiocodes.cpp $
 * 
 * 25    4/09/03 11:03a Bennylp
 * Added interface type for interface (for SNMP purpose)
 * 
 * 24    9/23/02 2:46p Bennylp
 * Changed macro USE_NEW_RTP to xgconfig.h
 * 
 * 23    4/09/02 12:02 Adham
 * 
 * 22    10/07/02 20:15 Adham
 * Added code to support new RTP, the code can be disabled using
 * XGATE_USE_NEW_RTP define on rtpchan.h
 * 
 * 21    5/13/02 11:58a Bennylp
 * Fixed bug: GetChannel: invalid channel number 10814 in board 3
 * 
 * 20    5/02/02 10:06a Bennylp
 * Protect LM function calls with #ifdef
 * 
 * 19    5/01/02 10:29a Bennylp
 * Updated for the new RegisterSerialNumber() function.
 * 
 * 18    4/29/02 5:50p Bennylp
 * Changed board startup at CommitCfg, and added serial num reg.
 * 
 * 17    4/24/02 6:45p Bennylp
 * Use next channel in round-robin fashion.
 * 
 * 16    10/30/01 4:42p Bennylp
 * Don't use channel if not idle for more than 150 msec
 * 
 * 15    16/10/01 14:22 Bennylp
 * Crash when 'show run' is called before initialization completes.
 * 
 * 14    8/09/01 4:07p Bennylp
 * Added ping command.
 * 
 * 13    7/31/01 7:41p Bennylp
 * Added barebone voice module for configuration entry point.
 * 
 * 12    7/31/01 6:52p Bennylp
 * Barebones for AudioCodes voice driver.
 * 
 * 11    7/30/01 3:51p Bennylp
 * Fixed GPF bug when CallManager is shutdown.
 * 
 * 10    28/07/01 21:28 Bennylp
 * SwitchAddresses will not be freed until interface is Unload()-ed.
 * 
 * 9     7/28/01 8:40p Bennylp
 * AudioCodes will pre-allocate switch addresses.
 * 
 * 8     7/23/01 1:39p Bennylp
 * Implement OnClearCounter.
 * 
 * 7     7/20/01 4:56p Bennylp
 * Bug: Aud interface is destroyed but still used by RTP to update stat.
 * 
 * 6     7/19/01 2:27p Bennylp
 * Added audioCodes board statistics.
 * 
 * 5     7/18/01 10:26p Bennylp
 * AudioCodes interface can be shutdown at runtime.
* 
* 4     7/17/01 6:36p Bennylp
* Add description command.
* 
* 3     6/30/01 11:36p Bennylp
* Misc. bug fixes and improvement (see rel notes in ver. 32).
* 
* 2     6/26/01 5:14p Bennylp
* Miscelanous bug fixes.
* 
* 1     6/22/01 7:43p Bennylp
* Initial coding.
* 
*/
//#include "stdafx.h"
#include "xgconfig.h"
#ifdef XGATE_HAS_AUDIOCODES
#ifdef XGATE_HAS_NEW_AUDIOCODES

#include "ace/INET_Addr.h"
#include "audiocodes.h"
#include "term.h"
#include "kernel.h"
#include "SCbusMgr.h"
#include "CTbusMgr.h"
//#include "licence.h"


AudioCodes_Manager *AudioCodes_Manager::acInstance;

#define AC_NAME	    "aud"
#define THISMODULE  AC_NAME

static unsigned long GetTickCount()
{
  struct timeval tv;
  if( gettimeofday(&tv, NULL) != 0 )
    return 0;

  return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

  const char *
ac_make_name (int id)
{
  static char name[20];
  sprintf(name, AC_NAME "%d", id);
  return name;
}

  const char *
ac_make_chname (int boardid, int chid)
{
  static char name[20];
  sprintf(name, AC_NAME "%d:%d", boardid, chid);
  return name;
}

struct ac_pair_t {
  int first;
  const char *second;
  const char *desc;
};


#define MAKE_PAIR(a,b,c)		   { a, b, c }

static ac_pair_t ac_bustype[] = {
  //MAKE_PAIR(acMVIP_BUS, "mvip",    "MVIP bus"),
  MAKE_PAIR(acSC_BUS,   "scbus",   "SC bus"),
  //MAKE_PAIR(acFRAMERS,  "framers", "Framers"),
  MAKE_PAIR(acH100_BUS, "ctbus",   "H.100/H.110 bus"),
  MAKE_PAIR(0,	  NULL,	     NULL)
};

static ac_pair_t ac_clocksrc[] = {
  MAKE_PAIR(acH110BusClockSource_Internal,      "internal", "Use/generate internal clock"),
  MAKE_PAIR(acH110BusClockSource_Network,       "network",  "Synchronize clock from PSTN network"),
  MAKE_PAIR(acH110BusClockSource_PrimaryMaster, "master",  "Synchronize from SCBUS/H.100 master"),
  MAKE_PAIR(acH110BusClockSource_SecondaryMaster, "secondary-master", "Synchronize from H100/H.110 secondary master"),
  MAKE_PAIR(acH110BusClockSource_NetReference,  "network-ref", "Synchronize from external Net reference clock"),
  MAKE_PAIR(0, NULL, NULL)
};

static ac_pair_t ac_busmaster[] = {
  MAKE_PAIR(acTDMBusMasterMode, "master", "Set as TDM bus master"),
  MAKE_PAIR(acTDMBusSlaveMode,  "slave", "Set as TDM bus slave"),
  MAKE_PAIR(0, NULL, NULL)
};

static ac_pair_t ac_law[] = {
  MAKE_PAIR(acALaw,  "alaw", "ALAW"),
  MAKE_PAIR(acMuLaw, "ulaw", "ULAW"),
  MAKE_PAIR(0, NULL, NULL)
};


  static inline const char *
ac_id2name (ac_pair_t *a, int id)
{
  while (a->second && a->first!=id)
    ++a;
  return a->second ? a->second : "unknown";
}

  static inline int
ac_name2id (ac_pair_t *a, const char *name)
{
  while (a->second && strcasecmp(a->second, name))
    ++a;
  return a->second ? a->first : -1;
}

  static inline const char *
ac_inet_ntoa(unsigned long addr)
{
  addr = htonl(addr);
  return inet_ntoa(*(in_addr*)&addr);
}


///////////////////////////////////////////////////////////////////////////////
AudioCodes_Manager::AudioCodes_Manager()
  : ServiceProvider("AudioCodes", false, false)
{
  acInstance = this;
  memset (channelAllocationMap, 0, sizeof(channelAllocationMap));
  for (int i=0; i<AC_MAX_BOARD; ++i)
    nextChannel[i] = &channelAllocationMap[i][0];
}


AudioCodes_Manager::~AudioCodes_Manager()
{
  acInstance = NULL;
}

AudioCodes_Manager * AudioCodes_Manager::Create()
{
  if (acInstance == NULL) {
    new AudioCodes_Manager;

    // Initialize library.
    acInstance->acLib.Open();

    unsigned i;

    // Create interfaces.
    for (i=0; i<acInstance->acLib.GetNumberOfBoards(); ++i) {
      acInstance->acAudInterface.push_back(new AudioCodes_Interface(i));
      //nextChannel[i] = &channelAllocationMap[i].[acLib.GetBoard(i)->GetNumberOfChannel()];
    }

    if (acInstance->acLib.GetNumberOfBoards() > 0) {
#ifdef XGATE_HAS_VOICE
      AudioCodes_VoiceModule::Create();
      AudioCodes_VoiceDriver::Create();
#endif
    }

  }
  return acInstance;
}


/** This function will be called by the service framework after 
  instantiation of this class, to allow the instance to initialize
  itself.
  */
bool AudioCodes_Manager::Load()
{
  return true;
}


/** This function will be called by the service framework before it is
  destroyed/deleted, to allow the instance to cleanup itself.
  */
bool AudioCodes_Manager::Unload()
{
  acAudInterface.clear();
  return acLib.Close();
}


/** HandleEvents: Descendent shall implement this abstract method.
  It is called by Service Provider Manager
  */
void AudioCodes_Manager::HandleEvents()
{
  acLib.HandleEvents();
}


/** Find a board from a name.
*/
AcBoard *AudioCodes_Manager::FindBoard(const char *name)
{
  static int name_len = strlen(AC_NAME);

  if (strncasecmp(name, AC_NAME, name_len))
    return NULL;

  char *err;
  int id = strtol(name+name_len, &err, 10);
  if (id < 0 || id >= acLib.GetNumberOfBoards() || *err)
    return NULL;

  return acLib.GetBoard(id);
} 


/** Allocate a channel from the specified board.
*/
AcChannel * AudioCodes_Manager::AllocateAcChannel(int boardId, CallDirection dir, CallParameter *cparam, int chId)
{
  assert (boardId >= 0 && boardId < AC_MAX_BOARD);

  AudioCodes_Interface *itf = acAudInterface[boardId];
  if (!itf || !itf->IsActive()) {
    CLog::Debug (THISMODULE, "Can't allocate channel: interface aud%d doesn't exist "
        "or is shutdown", boardId);
    return NULL;
  }

  if (chId == -1) {
    bool found=false;

    //
    // Find an unused channel, remembering the next channel allocated.
    //
    bool *status = nextChannel[boardId],
         *next = nextChannel[boardId],
         *end = &channelAllocationMap[boardId][0] + acLib.GetBoard(boardId)->GetNumberOfChannel();

    Time_Value idle_delay(0, 150000);   // 150-ms idle is required before channel can be used
    Time_Value now = Time_Value::get();

    do {
      if (*status) {
        do {
          ++status;
          if (status==end)
            status = &channelAllocationMap[boardId][0];
        } while (status != next && *status);

        if (status == next)
          break;
      }

      AcChannel *ch = acLib.GetBoard(boardId)->GetChannel(status-channelAllocationMap[boardId]);
      if (ch) {
        const timeb & tb_close_time = ch->GetLastCloseTime();
        Time_Value close_time(tb_close_time.time, tb_close_time.millitm*1000);
        if (now > close_time && now-close_time < idle_delay) {
          CLog::Detail(THISMODULE, "Channel aud%d:%u is not idle", boardId, ch->GetNumber());
          ++status;
          if (status==end)
            status = &channelAllocationMap[boardId][0];
        } else {
          found = true;
          break;
        }
      } else {
        ++status;
        if (status==end)
          status = &channelAllocationMap[boardId][0];
      }
    } while (status != next);

    if (!found) {
      CLog::Debug (THISMODULE, "Can't allocate channel: no more channel is available in interface aud%d", boardId);
      return NULL;    // full!
    }

    ++next;
    if (next==end)
      next = &channelAllocationMap[boardId][0];
    nextChannel[boardId] = next;

    if (!acLib.GetBoard(boardId)) {
      CLog::Debug (THISMODULE, "Can't allocate channel: board aud%d has not been created", boardId);
      return NULL;    // board has not been created!
    }

    AcChannel *ch = acLib.GetBoard(boardId)->GetChannel(status-channelAllocationMap[boardId]);
    if (ch) {
      *status = true;
    }
    return ch;
  }

  return acLib.GetBoard(boardId)->GetChannel(chId);
}


/** Allocate channel
*/
AudioCodes_Channel *AudioCodes_Manager::AllocateChannel(int boardId, 
    CallDirection dir, 
    CallParameter *cparam, 
    int chId)
{
  AcChannel *ac = AllocateAcChannel(boardId, dir, cparam, chId);
  if (!ac)
    return NULL;

  ////return new AudioCodes_Channel(ac); // commented by ippbx
}

/** Release channel.
*/
void AudioCodes_Manager::ReleaseChannel (AcChannel *ch,CallDirection dir,CallParameter *cparam,CallCause cc)
{
  int board_id = ch->GetBoard()->GetNumber();
  int ch_id = ch->GetNumber();

  assert(channelAllocationMap[board_id][ch_id]);
  channelAllocationMap[board_id][ch_id] = false;

  AudioCodes_Interface *itf = acAudInterface[board_id];
}


/** Get channel's local address.
*/
bool AudioCodes_Manager::GetChannelLocalAddress(AcChannel *channel, ACE_INET_Addr & addr)
{
  assert (channel);
  addr.set( channel->GetBoard()->GetRunningParam().GetParam().NetworkSettings.BoardBaseUDPPort + channel->GetNumber() * 10,
      channel->GetBoard()->GetRunningParam().GetParam().NetworkSettings.BoardIPAddr);
  return true;
}


///////////////////////////////////////////////////////////////////////////////
  AudioCodes_Interface::AudioCodes_Interface (int board_id)
: Interface(ITF_AUDIOCODES, ac_make_name(board_id), true), boardId(board_id),
  acBoard(NULL), inStartProgram(true)
{
  DisableRAP();
  boardParam.GetParam().DownloadableFiles.ImageFile = "";
}


AudioCodes_Interface::~AudioCodes_Interface()
{
  AudioCodes_Manager::Instance()->UnregisterInterface(this);
}


/** AllocateMedia: Called by CallManager to allocate a media in this itf.
  If a free media can be allocated, it may be returned synchronously or
  asynhronously (by calling MediaAllocatedInd method of Call object).
  NULL may be specified in call argument in the case of synchronous
  allocation.
  It returns false when there is no free media available
  */
bool AudioCodes_Interface::AllocateMedia(Call *call,Media *&media,
    SelectionStrategy sel,
    CallDirection dir,
    CallParameter *cparam)
{
  assert (false);
  media = NULL;
  return false;
}


/** AllocateMedia: A variant of AllocateMedia that specify a particular media
  to be allocated. It returns false when there is such media is not available
  */
bool AudioCodes_Interface::AllocateMedia(Call *call,Media *&media,
    MediaId id,
    CallDirection dir,
    CallParameter *cparam)
{
  assert (false);
  media = NULL;
  return false;
}



/** ReleaseMedia: Called by Call object when the call becomes idle. The underlying
  channel is then ready to use for another call.
  */
void AudioCodes_Interface::ReleaseMedia(Media *media,
    CallDirection dir,
    CallParameter *cparam,
    CallCause cc)
{
  assert (false);
}

enum {
  Cmd_Firmware,
  Cmd_FullDuplex,
  Cmd_HalfDuplex,
  Cmd_IP,
  Cmd_RTP,
  Cmd_TDM,
  Cmd_Shutdown,
  Cmd_Description,
  Cmd_Reload,
  Cmd_AcPing,
};

void AudioCodes_Interface::InitTermCommand()
{
  Cfg_Tree_Group *cfg_tree = this->GetConfigTree();
  ac_pair_t *pa;

  /* Cmd_AcPing
  */
  Term_Cmd *cmd_ping = new Term_Cmd(this, Cmd_AcPing,
      "ping", "Ping an IP host using the AudioCodes board",
      PrivilegeLevelUser, TermModeConfig, false);
  cmd_ping->Add(new Term_Cmd_Arg("@0", "", CVariantValidator("", "Remote IP address/hostname")));
  RegisterTermCmd(cmd_ping, cfg_tree);


  /* Firmware command
  */
  Term_Cmd *cmd_firmware = new Term_Cmd (this, 
      Cmd_Firmware, 
      "firmware", 
      "Set the firmware filename",
      PrivilegeLevelAdmin,
      TermModeConfig,
      false);
  char def_firmware[256];
  AcLibrary & aclib = AudioCodes_Manager::Instance()->AcLib();
  sprintf(def_firmware, "C:/AudioCodes/Voiplib/download/Ram%s.hex", aclib.GetBoardTypeString(aclib.GetBoardType(boardId)).c_str());

  cmd_firmware->Add (new Term_Cmd_Arg("@0", 
        CVariant(def_firmware), 
        CVariantValidator(CVariant(def_firmware), "Firmware filename"), 
        TermArgRequired));
  RegisterTermCmd (cmd_firmware, cfg_tree);

  /* Full-duplex command.
  */
  Term_Cmd *cmd_fullduplex = new Term_Cmd (this,
      Cmd_FullDuplex,
      "full-duplex",
      "Set the Ethernet interface to full duplex",
      PrivilegeLevelAdmin,
      TermModeConfig,
      true);
  RegisterTermCmd (cmd_fullduplex, cfg_tree);

  /* Half-duplex command.
  */
  Term_Cmd *cmd_halfduplex = new Term_Cmd (this,
      Cmd_HalfDuplex,
      "half-duplex",
      "Set the Ethernet interface to half duplex",
      PrivilegeLevelAdmin,
      TermModeConfig,
      true);
  RegisterTermCmd (cmd_halfduplex, cfg_tree);

  /* IP command.
  */
  Term_Cmd *cmd_ip = new Term_Cmd (this,
      Cmd_IP,
      "ip",
      "Set TCP/IP parameters",
      PrivilegeLevelAdmin,
      TermModeConfig,
      false);
  CVariantChoiceValidator ip_vdtor(CVariant("address"), "Set the Ethernet interface IP address");
  ip_vdtor.Add (CVariant("gateway"), "Set the default IP gateway");
  cmd_ip->Add(new Term_Cmd_Arg("@0",
        CVariant(""),
        ip_vdtor,
        TermArgRequired));
  cmd_ip->AddDynamic (1, 0, "address", 
      new Term_Cmd_Arg("@1", CVariant(""), CVariantValidator("", "IP address"), TermArgRequired));
  cmd_ip->AddDynamic (1, 0, "gateway", 
      new Term_Cmd_Arg("@1", CVariant(""), CVariantValidator("", "IP address of the default gateway"), TermArgRequired));
  cmd_ip->AddDynamic (2, 0, "address", 
      new Term_Cmd_Arg("@2", CVariant(""), CVariantValidator("", "Network/subnet mask"), TermArgRequired));
  RegisterTermCmd (cmd_ip, cfg_tree);

  /* RTP command.
  */
  Term_Cmd *cmd_rtp = new Term_Cmd (this,
      Cmd_RTP,
      "rtp",
      "Set RTP parameters",
      PrivilegeLevelAdmin,
      TermModeConfig,
      false);
  CVariantChoiceValidator rtp_vdtor("start-port", "Specify the UDP port number for the first channel");
  rtp_vdtor.Add ("port-increment", "Specify the port increment for subsequent channels");
  cmd_rtp->Add (new Term_Cmd_Arg("@0", CVariant(""), rtp_vdtor, TermArgRequired));
  cmd_rtp->AddDynamic (1, 0, "start-port", 
      new Term_Cmd_Arg("@1", CVariant(40000), CVariantRangeValidator(2, 60000, "UDP port"), TermArgRequired));
  /*
     cmd_rtp->AddDynamic (1, 0, "port-increment", 
     new Term_Cmd_Arg("@1", CVariant(10), CVariantRangeValidator(2, 20, "Port increment"), TermArgRequired));
     */
  RegisterTermCmd (cmd_rtp, cfg_tree);

  /* TDM command
  */
  Term_Cmd *cmd_tdm = new Term_Cmd (this,
      Cmd_TDM,
      "tdm",
      "Set TDM bus settings",
      PrivilegeLevelAdmin,
      TermModeConfig,
      false);
  CVariantChoiceValidator tdm_vdtor("law", "Set the PCM Law");
  tdm_vdtor.Add("bus-type", "Set the TDM bus type");
  tdm_vdtor.Add("clock-source", "Set the TDM bus clock source");
  cmd_tdm->Add (new Term_Cmd_Arg("@0", CVariant(""), tdm_vdtor, TermArgRequired));

  CVariantChoiceValidator law_vdtor(ac_law[0].second, ac_law[0].desc);
  pa = ac_law + 1;
  while (pa->second) {
    law_vdtor.Add (pa->second, pa->desc);
    ++pa;
  }
  cmd_tdm->AddDynamic (1, 0, "law",
      new Term_Cmd_Arg("@1", "", law_vdtor, TermArgRequired));

  CVariantChoiceValidator bustype_vdtor(ac_bustype[0].second, ac_bustype[0].desc);
  pa = ac_bustype + 1;
  while (pa->second) {
    bustype_vdtor.Add (pa->second, pa->desc);
    ++pa;
  }
  cmd_tdm->AddDynamic (1, 0, "bus-type",
      new Term_Cmd_Arg("@1", "", bustype_vdtor, TermArgRequired));
  CVariantChoiceValidator master_vdtor(ac_busmaster[0].second, ac_busmaster[0].desc);
  pa = ac_busmaster + 1;
  while (pa->second) {
    master_vdtor.Add (pa->second, pa->desc);
    ++pa;
  }
  cmd_tdm->AddDynamic (2, 0, "bus-type",
      new Term_Cmd_Arg("@2", "", master_vdtor, TermArgRequired));

  CVariantChoiceValidator clocksrc_vdtor(ac_clocksrc[0].second, ac_clocksrc[0].desc);
  pa = ac_clocksrc + 1;
  while (pa->second) {
    clocksrc_vdtor.Add (pa->second, pa->desc);
    ++pa;
  }
  cmd_tdm->AddDynamic (1, 0, "clock-source",
      new Term_Cmd_Arg("@1", "", clocksrc_vdtor, TermArgRequired));


  RegisterTermCmd (cmd_tdm, cfg_tree);


  /* Shutdown command.
  */
  Term_Cmd *cmd_shut = new Term_Cmd (this,
      Cmd_Shutdown,
      "shutdown",
      "Enable/disable this interface",
      PrivilegeLevelAdmin,
      TermModeConfig,
      true);
  RegisterTermCmd (cmd_shut, cfg_tree);

  /* Cmd_Reload command
  */
  Term_Cmd *cmd_reload = new Term_Cmd (this,
      Cmd_Reload,
      "reload",
      "Reload and reinitialize board",
      PrivilegeLevelAdmin,
      TermModeConfig,
      false);
  RegisterTermCmd (cmd_reload, cfg_tree);

  /* Description command.
  */
  Term_Cmd *cmd_desc = new Term_Cmd (this,
      Cmd_Description,
      "description",
      "Set interface description",
      PrivilegeLevelAdmin,
      TermModeConfig,
      true);
  cmd_desc->Add (new Term_Cmd_Arg("@0", "", CVariantValidator("", "Description"), TermArgOptional));
  RegisterTermCmd(cmd_desc, cfg_tree);
}


/** Load: Called once at initialization time
*/
bool AudioCodes_Interface::Load()
{
  if (!acBoard) {
    AcLibrary & aclib = AudioCodes_Manager::Instance()->AcLib();
    acBoard = aclib.CreateBoard(boardId);
    boardParam.SetDefaultParam(aclib.GetBoardType(boardId));

    // Override some default parameters with ours.
    boardParam.GetParam().TDMBusSettings.TDMBusMasterSlaveSelection = 0;
    boardParam.GetParam().TDMBusSettings.TDMBusType = acSC_BUS;
    // Luckily acTTDMBusClockSource has the same value as acTH110BusClockSource
    boardParam.GetParam().TDMBusSettings.TDMBusClockSource = 
      (acTTDMBusClockSource) acH110BusClockSource_PrimaryMaster;
    boardParam.SetNetworkSettings("0.0.0.0", "0.0.0.0", "0.0.0.0", false);

    char def_firmware[256];
    sprintf(def_firmware, "C:\\AudioCodes\\Voiplib\\download\\Ram%s.hex", aclib.GetBoardTypeString(aclib.GetBoardType(boardId)).c_str());
    boardParam.SetImageFile(def_firmware);

  }
  if (acBoard) {
    SetMediaCount(acBoard->GetNumberOfChannel());
  }
  InitTermCommand();
  return acBoard!=NULL && Interface::Load();
}


/** Unload: Called once at termination time
*/
bool AudioCodes_Interface::Unload()
{
  StopBoard();
  FreeChannelAddresses();
  return Interface::Unload();
}


/** This function will be called by the service framework once the 
  configuration file has been fully read.
  */
bool AudioCodes_Interface::CommitConfiguration()
{
  inStartProgram = false;

  if (IsEnabled()) {
    if (acBoard == NULL)
      acBoard = AcLibrary::Instance()->CreateBoard(boardId);
    if (acBoard == NULL) {
      CLog::Error( THISMODULE, "Can not create interface %s: internal error.", GetName());
      return false;
    }

    CLog::Detail( THISMODULE, "Starting interface %s...", GetName());
    if (!StartBoard()) {
      CLog::Error( THISMODULE, "Error in starting board %s.", GetName());
    }

    if (acBoard->GetInitializationStatus()==1) {
      Enable();
      Start();
#ifdef XGATE_HAS_LM
      String ser=acBoard->GetSerialNumber();
      LicenceManager::Instance()->RegisterSerialNum(Fg_Audiocodes, this->boardId, ser.c_str(), ser.size());
#endif
      CLog::Detail( THISMODULE, "Interface %s started", GetName());
    } else
      Disable(DC_Shutdown);

  } else {
    //
    // Must start the board to get the serial number.
    // We can't just put 'XXXX' here since:
    // - if we put 'XXXX' during registration, then next time if the board
    //   is started, we will get the valid serial number which will be
    //   rejected.
    // - if we put 'XXXX' during runtime, then next time the board is started
    //   with different serial number, it will be accepted.
    //
    /*
       if (acBoard == NULL)
       acBoard = AcLibrary::Instance()->CreateBoard(boardId);
       if (acBoard == NULL) {
       CLog::Error( THISMODULE, "Can not create interface %s: internal error.", GetName());
       return false;
       }
       AcBoardparam param = acBoard->GetRunningParam();
       */
#ifdef XGATE_HAS_LM
    LicenceManager::Instance()->RegisterSerialNum(Fg_Audiocodes, this->boardId, "XXXX", 4);
#endif
  }

  return true;
}


void AudioCodes_Interface::FreeChannelAddresses()
{
#ifdef AC_PREALLOCATE_SW_ADDR
  switch (boardParam.GetParam().TDMBusSettings.TDMBusType) {
    case acSC_BUS:
      while (chSwitchAddress.size()) {
        SCbusManager::Instance()->FreeAddress((SCbusAddress*)chSwitchAddress.back());
        chSwitchAddress.pop_back();
      }
      break;
    case acH100_BUS:
      while (chSwitchAddress.size()) {
        CTbusManager::Instance()->FreeAddress((CTbusAddress*)chSwitchAddress.back());
        chSwitchAddress.pop_back();
      }
      break;
  }
#endif
}

bool AudioCodes_Interface::StartBoard()
{
  assert (acBoard);
  if (acBoard->GetInitializationStatus() > 0)
    return true;

  // Set this thread priority to lower, so that main thread
  // don't starve.
#ifndef __linux__
  int old_prio = ::GetThreadPriority(GetCurrentThread());
#endif
  //::SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);

  // Open the board
  if (!acBoard->Open(boardParam)) {
    //::SetThreadPriority(GetCurrentThread(), old_prio);
    return false;
  }

  //::SetThreadPriority(GetCurrentThread(), old_prio);

  // Pre-allocate switch addresses
  // The preallocated address will only be released when the interface
  // is unloaded, not when the board is closed.
#ifdef AC_PREALLOCATE_SW_ADDR
  unsigned i;
  switch (boardParam.GetParam().TDMBusSettings.TDMBusType) {
    case acSC_BUS:
      for (i=chSwitchAddress.size(); i<acBoard->GetNumberOfChannel(); ++i) {
        char addr_name[32];
        sprintf(addr_name, "%s:%u", Name().c_str(), i);
        SCbusAddress *addr = SCbusManager::Instance()->AllocateAddress(addr_name);
        if (addr)
          chSwitchAddress.push_back (addr);
        else
          break;
      }
      break;
    case acH100_BUS:
      for (i=chSwitchAddress.size(); i<acBoard->GetNumberOfChannel(); ++i) {
        char addr_name[32];
        sprintf(addr_name, "%s:%u", Name().c_str(), i);
        CTbusAddress *addr = CTbusManager::Instance()->AllocateAddress(addr_name);
        if (addr)
          chSwitchAddress.push_back (addr);
        else
          break;
      }
      break;
  }

  if (chSwitchAddress.size() != acBoard->GetNumberOfChannel()) {
    CLog::Error (THISMODULE, "Unable to allocate %d SCBUS addreses for %s", 
        acBoard->GetNumberOfChannel(), Name().c_str());
    StopBoard();
    return false;
  }
#endif

  return true;
}


void AudioCodes_Interface::StopBoard()
{
  //FreeChannelAddresses();

  if (acBoard) {
    acBoard->Close();
    acBoard = NULL;
    AudioCodes_Manager::Instance()->AcLib().DestroyBoard(boardId);
  }
}


/** Open: May be called several times to start the object
  The object may be then in In-Service (enabled/disabled) state
  */
bool AudioCodes_Interface::Start()
{
  assert (acBoard);

  // Open the board
  if (!StartBoard())
    return false;

  // Instantiate all the channels.
  for (unsigned i=0; i<acBoard->GetNumberOfChannel(); ++i)
    acBoard->CreateChannel(i);

  Interface::Enable();
  return Interface::Start();
}


/** Close: Once started, an object may be requested to close
  The object goes to the Out-Of-Service state
  */
bool AudioCodes_Interface::Stop()
{
  if (acBoard && acBoard->GetInitializationStatus()==1) {
    acBoard->Close();
  }

  Interface::Disable(DC_Shutdown);
  return Interface::Stop();
}


/** HandleEvents: Descendent shall implement this abstract method.
  It is called by Service Provider Manager
  */
void AudioCodes_Interface::HandleEvents()
{
  if (acBoard && acBoard->GetInitializationStatus()==1)
    acBoard->HandleEvents();
}


/** Clear error counters.
*/
void AudioCodes_Interface::OnClearCounter()
{
  AcBoardStat & stat = AcLibrary::Instance()->GetBoardStat(boardId);
  memset(&stat, 0, sizeof(stat));
  Interface::OnClearCounter();
}


/**
  Show configuration for the specified entry.
  This function will be called by the framework whenever it needs to display the
  specified entry. Previously this instance should tell the entry that it wishes to
  handle displaying the entry by calling entry->RegisterShowHandler().

  When this instance is deleted, it should call entry->UnregisterShowHandler()
  so that the handler will not be called (which will cause GPF).
  */
bool AudioCodes_Interface::OnShowConfig( const Cfg_Tree_Entry *entry,
    String & output, 
    const String & indent, 
    int indent_size) const
{
  const acTBoardParam & p = boardParam.GetParam();
  char s[64];

  if (GetDescription().size())
    output += indent + "description \"" + GetDescription() + "\"\n";


  /* Show firmware config.
  */ 
  output += indent + "firmware \"" +
    p.DownloadableFiles.ImageFile + "\"\n";


  /* Duplex settings.
  */
  if (p.NetworkSettings.Full_HalfDuplexEthernet)
    output += indent + "full-duplex\n";
  else
    output += indent + "half-duplex\n";

  /* IP configuration.
  */
  output += indent + "ip address " + ac_inet_ntoa(p.NetworkSettings.BoardIPAddr);
  output += String(" ") + ac_inet_ntoa(p.NetworkSettings.BoardSubNetAddr) + "\n";
  output += indent + "ip gateway " + ac_inet_ntoa(p.NetworkSettings.BoardDefaultGatewayAddr) + "\n";

  /* RTP configuration.
  */
#ifndef __linux__
  output += indent + "rtp start-port " + itoa(p.NetworkSettings.BoardBaseUDPPort, s, 10) + "\n";
#else
  char tmp[40];
  sprintf(tmp,"%d",p.NetworkSettings.BoardBaseUDPPort);
  output += indent + "rtp start-port " + (std::string) tmp + "\n";
#endif

  /* TDM settings.
  */
  output += indent + "tdm bus-type " + ac_id2name(ac_bustype, p.TDMBusSettings.TDMBusType) + " " + 
    ac_id2name(ac_busmaster, p.TDMBusSettings.TDMBusMasterSlaveSelection) + "\n";
  output += indent + "tdm clock-source " + ac_id2name(ac_clocksrc, p.TDMBusSettings.TDMBusClockSource) + "\n";
  output += indent + "tdm law " + ac_id2name(ac_law, p.TDMBusSettings.PCMLawSelect) + "\n";

  /* State
  */
  if (IsActive())
    output += indent + "no shutdown\n";
  else
    output += indent + "shutdown\n";

  return OK;
}


/** Display interface specific information to the terminal user.
  Decendants MUST implement this function.
  */
void AudioCodes_Interface::OnShowInterface(Term_Session * session )
{
  /* Copy everything to a temporary variables.
  */
  ACQUIRE_SYSTEM_LOCK();
  String ipaddr, mask, gw;
  ipaddr = ac_inet_ntoa(boardParam.GetParam().NetworkSettings.BoardIPAddr);
  mask = ac_inet_ntoa(boardParam.GetParam().NetworkSettings.BoardSubNetAddr);
  gw = ac_inet_ntoa(boardParam.GetParam().NetworkSettings.BoardDefaultGatewayAddr);

  acTBoardParam board_param = boardParam.GetParam();

  bool board_exist = acBoard != NULL && acBoard->GetInitializationStatus() >= 1;
  String board_type, board_sn;
  AcBoardStat board_stat = AcLibrary::Instance()->GetBoardStat(boardId);
  if (board_exist) {
    board_type = acBoard->GetName();
    board_sn = acBoard->GetSerialNumber();
  }

  RELEASE_SYSTEM_LOCK();

  session->Sendf ( SHOW_INTERFACE_INDENT
      "IP address: %s/%s, default gw: %s\n",
      ipaddr.c_str(), mask.c_str(), gw.c_str());
  session->Sendf ( SHOW_INTERFACE_INDENT
      "TDM bus type: %s, clock type: %s, clock source: %s\n",
      ac_id2name(ac_bustype, board_param.TDMBusSettings.TDMBusType),
      ac_id2name(ac_busmaster, board_param.TDMBusSettings.TDMBusMasterSlaveSelection),
      ac_id2name(ac_clocksrc, board_param.TDMBusSettings.TDMBusClockSource));
  if (board_exist) {
    session->Sendf ( SHOW_INTERFACE_INDENT 
        "Hardware is %s, serial number: %s\n", board_type.c_str(), board_sn.c_str());
  }
  session->Sendf ( SHOW_INTERFACE_INDENT "Board errors: \n"
      SHOW_INTERFACE_INDENT "  can't send cmd to board: %u, cmd overrun: %u\n"
      SHOW_INTERFACE_INDENT "  Channel open error: %u, change error: %u, activate error: %u\n"
      SHOW_INTERFACE_INDENT "  dest unreachables: %u, rx too many reordered: %u, rx invalid payload: %u\n"
      SHOW_INTERFACE_INDENT "  TDM: %u, DSP crash: %u, event overrun: %u\n",
      board_stat.cmdTxError, board_stat.cmdBufferError,
      board_stat.openChError, board_stat.changeParamError, board_stat.activateRtpError,
      board_stat.destUnreachError, board_stat.rxReorderError, board_stat.rxUnknownPayloadError,
      board_stat.tdmBusError, board_stat.dspError, board_stat.evtOverrunError);
  session->Sendf ( SHOW_INTERFACE_INDENT "Total number of carrier transitions: %u",
      board_stat.carrierTransition);
  session->Sendf ("\n");
}

/** AcPing callback
*/
class AcPingCallback {
  public:	
    AcPingCallback()
      : success(false)
    {
#ifndef __linux__
      hSem = ::CreateSemaphore(NULL, 0, 1000, NULL);
#endif
    }
    ~AcPingCallback() {
#ifndef __linux__
      ::CloseHandle(hSem);
#endif
    }
    enum Status {
      Timeout,
      Success,
      Failed
    };

    Status Wait(unsigned msec, unsigned *elapsed) {
      Status status;
      unsigned long start = GetTickCount();
#ifndef __linux__
      if (::WaitForSingleObject(hSem, msec) != WAIT_OBJECT_0) {
        status=Timeout;
      } else if (success) {
        status=Success;
      } else {
        status=Failed;
      }
#else
      status=Success;
#endif
      if (elapsed)
        *elapsed = GetTickCount()-start;
      return status;
    }

    static void Callback(bool reachable, unsigned user_data) {
      AcPingCallback *cb = (AcPingCallback *)user_data;

      try {
        if (cb) cb->Callback(reachable);
      } catch (...) {
        // AudioCodes tries to call this while the session has gone.
      }
    }

  private:
    HANDLE hSem;
    bool success;

    void Callback(bool reachable) {
      success = reachable;
      long prev;
#ifndef __linux__
      ::ReleaseSemaphore(hSem, 1, &prev);
#endif
    }
};


/**
  Terminal command callback
  */
void AudioCodes_Interface::DoTermCmd(Term_Session *session, bool no, Term_Cmd *cmd)
{
  CVariant v0, v1, v2;

  ACQUIRE_SYSTEM_LOCK();

  switch (cmd->GetId()) {
    case Cmd_Description:
      if (no || !cmd->GetArg((size_t)0)->HasValue())
        SetDescription("");
      else {
        cmd->GetArg((size_t)0)->GetValue(v0);
        SetDescription(v0.AsString());
      }
      break;
    case Cmd_Firmware:
      if (IsActive()) {
        RELEASE_SYSTEM_LOCK();
        session->SendErrorf("Can not change settings while the board is running.\n");
        return;
      }
      cmd->GetArg((size_t)0)->GetValue(v0);
      boardParam.SetImageFile(v0.AsString());
      break;
    case Cmd_FullDuplex:
      if (IsActive()) {
        RELEASE_SYSTEM_LOCK();
        session->SendErrorf("Can not change settings while the board is running.\n");
        return;
      }
      if (no)
        boardParam.GetParam().NetworkSettings.Full_HalfDuplexEthernet = 0;
      else
        boardParam.GetParam().NetworkSettings.Full_HalfDuplexEthernet = 1;
      break;
    case Cmd_HalfDuplex:
      if (IsActive()) {
        RELEASE_SYSTEM_LOCK();
        session->SendErrorf("Can not change settings while the board is running.\n");
        return;
      }
      if (no)
        boardParam.GetParam().NetworkSettings.Full_HalfDuplexEthernet = 1;
      else
        boardParam.GetParam().NetworkSettings.Full_HalfDuplexEthernet = 0;
      break;
    case Cmd_IP:
      if (IsActive()) {
        RELEASE_SYSTEM_LOCK();
        session->SendErrorf("Can not change settings while the board is running.\n");
        return;
      }
      cmd->GetArg((size_t)0)->GetValue(v0);
      if (!strcasecmp(v0.AsString(), "address")) {
        cmd->GetArg((size_t)1)->GetValue(v1);
        cmd->GetArg((size_t)2)->GetValue(v2);
        boardParam.GetParam().NetworkSettings.BoardIPAddr = ntohl(inet_addr(v1.AsString()));
        boardParam.GetParam().NetworkSettings.BoardSubNetAddr = ntohl(inet_addr(v2.AsString()));
      } else if (!strcasecmp(v0.AsString(), "gateway")) {
        cmd->GetArg((size_t)1)->GetValue(v1);
        boardParam.GetParam().NetworkSettings.BoardDefaultGatewayAddr = ntohl(inet_addr(v1.AsString()));
      }
      break;
    case Cmd_RTP:
      if (IsActive()) {
        RELEASE_SYSTEM_LOCK();
        session->SendErrorf("Can not change settings while the board is running.\n");
        return;
      }
      cmd->GetArg((size_t)0)->GetValue(v0);
      if (!strcasecmp(v0.AsString(), "start-port")) {
        cmd->GetArg((size_t)1)->GetValue(v1);
        boardParam.GetParam().NetworkSettings.BoardBaseUDPPort = v1.AsInt();
      }
      break;
    case Cmd_TDM:
      if (IsActive()) {
        RELEASE_SYSTEM_LOCK();
        session->SendErrorf("Can not change settings while the board is running.\n");
        return;
      }
      cmd->GetArg((size_t)0)->GetValue(v0);
      cmd->GetArg((size_t)1)->GetValue(v1);

      if (!strcasecmp(v0.AsString(), "law")) {
        boardParam.GetParam().TDMBusSettings.PCMLawSelect = 
          (acTPCMLawSelect)ac_name2id(ac_law, v1.AsString());
      } else if (!strcasecmp(v0.AsString(), "bus-type")) {

        acTTDMBusType oldbustype = boardParam.GetParam().TDMBusSettings.TDMBusType;
        acTTDMBusType newbustype = boardParam.GetParam().TDMBusSettings.TDMBusType = 
          (acTTDMBusType)ac_name2id(ac_bustype, v1.AsString());

        if (oldbustype != newbustype)
          FreeChannelAddresses();

        switch (newbustype) {
          case acMVIP_BUS:
            boardParam.GetParam().TDMBusSettings.TDMBusSpeed = acTDMBusSpeed_2048kbps;
            break;
          case acSC_BUS:
            boardParam.GetParam().TDMBusSettings.TDMBusSpeed = acTDMBusSpeed_4096kbps;
            break;
          case acH100_BUS:
            boardParam.GetParam().TDMBusSettings.TDMBusSpeed = acTDMBusSpeed_8192kbps;
            break;
        }
        cmd->GetArg((size_t)2)->GetValue(v2);
        boardParam.GetParam().TDMBusSettings.TDMBusMasterSlaveSelection = ac_name2id(ac_busmaster, v2.AsString());

      } else if (!strcasecmp(v0.AsString(), "clock-source")) {
        boardParam.GetParam().TDMBusSettings.TDMBusClockSource = 
          (acTTDMBusClockSource) ac_name2id(ac_clocksrc, v1.AsString());
      }
      break;
    case Cmd_Shutdown:
      if (!no) {
        if (IsActive()) {
          if (GetMediaCount() != acBoard->GetNumberOfChannel()) {
            RELEASE_SYSTEM_LOCK();
            session->SendErrorf("Can not shutdown interface because channels are being used.\n");
            return;
          }
          StopBoard();
          //Stop();
          Disable(DC_Administrative);

        } else
          session->Sendf("Interface is not running\n");
      } else {
        if (IsActive()) 
          session->Sendf("Interface is already running\n");
        else if (!inStartProgram) {
          if (acBoard == NULL)
            acBoard = AcLibrary::Instance()->CreateBoard(boardId);
          if (acBoard == NULL) {
            RELEASE_SYSTEM_LOCK();
            session->SendErrorf("Can not create interface: internal error.\n");
            return;
          }

          RELEASE_SYSTEM_LOCK();

          session->Sendf("Starting interface %s...\n", GetName());
          if (!StartBoard()) {
            session->SendErrorf ("Error in starting board.\n");
          }

          ACQUIRE_SYSTEM_LOCK();

          if (acBoard->GetInitializationStatus()==1) {
            Enable();
            Start();
#ifdef XGATE_HAS_LM
            String ser=acBoard->GetSerialNumber();
            LicenceManager::Instance()->RegisterSerialNum(Fg_Audiocodes, 
                this->boardId, 
                ser.c_str(), 
                ser.size());
#endif
          }

        } else {
          Enable();
        }
      }
      break;
    case Cmd_Reload:
      if (acBoard) {
        if (GetMediaCount() != acBoard->GetNumberOfChannel()) {
          RELEASE_SYSTEM_LOCK();
          session->SendErrorf("Can't reinitialize interface because some channels are being used.\n");
          return;
        }

        // Shutdown board
        StopBoard();
      }

      assert(acBoard == NULL);
      acBoard = AcLibrary::Instance()->CreateBoard(boardId);
      //Stop();
      Disable(DC_Administrative);
      RELEASE_SYSTEM_LOCK();

      session->Sendf("Starting interface...\n");
      if (!StartBoard()) {
        session->SendErrorf ("Error in starting board.\n");
      }

      ACQUIRE_SYSTEM_LOCK();
      if (acBoard->GetInitializationStatus()==1) {
        Enable();
        Start();
      }

      break;
    case Cmd_AcPing:
      RELEASE_SYSTEM_LOCK();
      if (acBoard==NULL || acBoard->GetInitializationStatus() != 1) {
        session->SendErrorf("board is not initialized\n");
        return;
      }

      String host;
      cmd->GetArg((size_t)0)->GetValue(v0);
      host = v0.ToString();

      AcPingCallback *callback;
      callback = new AcPingCallback;

      enum {
        PING_COUNT = 5
      };
      unsigned count=PING_COUNT;
      unsigned min=2000, tot=0, max=0, success=0;

      session->Sendf("Pinging %s: ", host.c_str());

      while (count-- > 0) {
        unsigned elapsed=0;

        if (!ACQUIRE_SYSTEM_LOCK())
          break;
        if (acBoard==NULL || acBoard->GetInitializationStatus() != 1)
          break;
        bool status = acBoard->TestIpConnection(host, (unsigned)callback, &AcPingCallback::Callback);
        if (status)
          acBoard->HandleEvents();
        RELEASE_SYSTEM_LOCK();

        if (!status)
          session->Sendf("E");
        else {
          switch (callback->Wait(10000, &elapsed)) {
            case AcPingCallback::Timeout:
              session->Sendf(".");
              break;
            case AcPingCallback::Success:
              session->Sendf("!");
              ++success;
              break;
            case AcPingCallback::Failed:
              session->Sendf("U");
              break;
          }
          if (elapsed < min)
            min = elapsed;
          if (elapsed > max)
            max = elapsed;
          tot += elapsed;
        }
      }
      session->Sendf("\nSuccess rate is %u percent (%u/%u), time min/avg/max = %u/%u/%u ms\n"
          "RTT is less than the time reported above\n", 
          success*100/PING_COUNT, success, PING_COUNT,
          min, tot/PING_COUNT, max);
      delete callback;
      return;
      break;
  }

  RELEASE_SYSTEM_LOCK();
}



///////////////////////////////////////////////////////////////////////////////
#define AC_VOX_UNIMPLEMENTED(f)	    CLog::Info(THISMODULE, GetTask(), "Unimplemented function %s", #f);

#ifdef XGATE_HAS_VOICE

  AudioCodes_VoiceDevice::AudioCodes_VoiceDevice(AcChannel *channel)
: VoiceDevice(ac_make_chname(channel->GetBoard()->GetNumber(), channel->GetNumber()), 
    new AudioCodes_Switch_Device(ac_make_chname(channel->GetBoard()->GetNumber(), channel->GetNumber()), channel)),
  acChannel(channel), lastTermDigit(0), state(IdleState), eventCallback(*this, channel)
{
}

bool AudioCodes_VoiceDevice::Open()
{
  AcChannelParam copy_param = acChannel->GetRunningParam();

  copy_param.SetCodec(acG711Alaw_64);
  copy_param.SetCodecSettings(1, 0, 1);

  copy_param.GetParam().TransportSettings.UseNIorPCI = 1;
  copy_param.GetParam().Active = 1;

  AudioCodes_Switch_Device * sw = (AudioCodes_Switch_Device*) GetSwitchDevice();
  if (sw->SupportSCbus(NULL)) {
    SCbusAddress *sc_addr = (SCbusAddress *)sw->GetAddress(SF_SCbus);
    assert (sc_addr);
    if (sc_addr != NULL)
      copy_param.SetTxTimeslot(sc_addr->GetTimeslot());

  } else if (sw->SupportCTbus(NULL)) {
    CTbusAddress *ct_addr = (CTbusAddress *)sw->GetAddress(SF_CTbus);
    assert (ct_addr);
    if (ct_addr != NULL) {
      copy_param.GetParam().TDMBusSettings.TDMBusOutputPort = ct_addr->GetStream();
      copy_param.GetParam().TDMBusSettings.TDMBusOutputChannel = ct_addr->GetTimeslot();
    }

  } else
    return false;


  bool success = acChannel->Open(copy_param);
  acChannel->GetBoard()->HandleEvents();
  return success;
}

void AudioCodes_VoiceDevice::Free()
{
  if (acChannel->GetInitializationStatus() >= 1) {
    acChannel->Close();
    acChannel->GetBoard()->HandleEvents();
  }
  AudioCodes_VoiceDriver::Instance()->FreeVoiceDevice(this);
}

bool AudioCodes_VoiceDevice::IsDeviceIdle()
{
  return state==IdleState;
}

bool AudioCodes_VoiceDevice::CancelJob()
{
  if (state != IdleState) {
    VoiceManager::Instance()->ResumeTask(this);
    switch (state) {
      case DialingState:
        //acChannel->StopPlay();
        break;
    }
    state = IdleState;
  }
  return true;
}

void AudioCodes_VoiceDevice::OnAcEvent(int event, const acTEventInfo & event_info)
{
  switch (event) {
    case acEV_DIGIT:
      for (event=0; event < event_info.DigitsReport.NumDigits; ++event) {
        if (event_info.DigitsReport.Digit[0].DigitOnTime)
          digitBuffer.append(1, acChannel->AcDigit2Digit(event_info.DigitsReport.Digit[event]));
      }
      break;
    case acEV_DIAL_ENDED:
      if (state == DialingState)
        CancelJob();
      break;
  }
}

STATUS AudioCodes_VoiceDevice::SetVoiceFormat(VoiceFormat format)
{
  AC_VOX_UNIMPLEMENTED(SetVoiceFormat)
    return OK;
}

STATUS AudioCodes_VoiceDevice::GetVoiceFormat(VoiceFormat *format)
{
  AC_VOX_UNIMPLEMENTED(GetVoiceFormat)
    return OK;
}

STATUS AudioCodes_VoiceDevice::EnableInterrupt()
{
  AC_VOX_UNIMPLEMENTED(EnableInterrupt)
    return OK;
}

STATUS AudioCodes_VoiceDevice::DisableInterrupt()
{
  AC_VOX_UNIMPLEMENTED(DisableInterrupt)
    return OK;
}

STATUS AudioCodes_VoiceDevice::SetMaxRecTime(int maxrectime)
{
  AC_VOX_UNIMPLEMENTED(SetMaxRecTime)
    return OK;
}

STATUS AudioCodes_VoiceDevice::GetMaxRecTime(int *maxrectime)
{
  *maxrectime = 0;
  AC_VOX_UNIMPLEMENTED(GetMaxRecTime)
    return OK;
}

STATUS AudioCodes_VoiceDevice::SetInterDigitTimeout(int maxidtime)
{
  AC_VOX_UNIMPLEMENTED(SetInterDigitTimeout)
    return OK;
}

STATUS AudioCodes_VoiceDevice::GetInterDigitTimeout(int *maxidtime)
{
  AC_VOX_UNIMPLEMENTED(GetInterDigitTimeout)
    return OK;
}

STATUS AudioCodes_VoiceDevice::SetDigitTimeout(int maxidtime)
{
  AC_VOX_UNIMPLEMENTED(SetDigitTimeout)
    return OK;
}

STATUS AudioCodes_VoiceDevice::GetDigitTimeout(int *maxidtime)
{
  AC_VOX_UNIMPLEMENTED(GetDigitTimeout)
    return OK;
}

STATUS AudioCodes_VoiceDevice::SetTermDigits(char *termdigs)
{
  termDigits = termdigs;
  return OK;
}

STATUS AudioCodes_VoiceDevice::GetTermDigits(char *termdigs)
{
  safe_strcpy(termdigs, termDigits.c_str(), MAXAPPSTRLEN);
  return OK;
}

STATUS AudioCodes_VoiceDevice::PlayFileArray(int nbfiles,char const **filename,bool bkgnd,bool autorepeat)
{
  AC_VOX_UNIMPLEMENTED(PlayFileArray)
    return OK;
}

STATUS AudioCodes_VoiceDevice::StopPlayFileBkGnd(bool waiteof)
{
  AC_VOX_UNIMPLEMENTED(StopPlayFileBkGnd)
    return OK;
}

STATUS AudioCodes_VoiceDevice::PlayTone(int freq1,int db1,int freq2,int db2,int duration)
{
  AC_VOX_UNIMPLEMENTED(PlayTone)
    return OK;
}

STATUS AudioCodes_VoiceDevice::PlayDtmf(char *dtmf)
{
  if (strlen(dtmf) == 0) 
    return OK;

  if (acChannel->DialDtmf(dtmf)) {
    state = DialingState;
    VoiceManager::Instance()->SuspendTask(this);
  }
  return OK;
}

STATUS AudioCodes_VoiceDevice::RecordFile(char *filename)
{
  AC_VOX_UNIMPLEMENTED(RecordFile)
    return OK;
}

STATUS AudioCodes_VoiceDevice::GetDigits(int length,char *buffer)
{
  // WE DON'T SUPPORT SLEEPING TASK AT THE MOMENT

  *buffer = '\0';

  if (digitBuffer.empty())
    return OK;

  if (length >= MAXAPPSTRLEN)
    length = MAXAPPSTRLEN - 1;

  // do we have enough digits in the buffer ?
  if (digitBuffer.size() >= length) {
    safe_strcpy(buffer, digitBuffer.c_str(), MAXAPPSTRLEN);
    buffer[length] = '\0';
    digitBuffer.erase(0, length);
    return OK;
  }

  // do we have term digits in the buffer ?
  for (unsigned i=0; i<digitBuffer.size(); ++i) {
    if (std::find(termDigits.begin(), termDigits.end(), digitBuffer[i]) != termDigits.end()) {
      // Found it.
      // Copy including the termination digit.
      safe_strcpy(buffer, digitBuffer.c_str(), MAXAPPSTRLEN);
      buffer[i] = '\0';
      digitBuffer.erase(0, i+1);
      return OK;
    }
  }
  return OK;
}

STATUS AudioCodes_VoiceDevice::GetLastTermDigit(char *buffer)
{
  buffer[0] = lastTermDigit;
  buffer[1] = '\0';
  return OK;
}

STATUS AudioCodes_VoiceDevice::ClearDigits()
{
  digitBuffer.erase();
  return OK;
}

STATUS AudioCodes_VoiceDevice::PeekDigit(char *buffer)
{
  safe_strcpy(buffer, digitBuffer.c_str(), MAXAPPSTRLEN);
  return OK;
}


///////////////////////////////////////////////////////////////////////////////

enum {
  Cmd_Vox_Shutdown
};

AudioCodes_VoiceModule *AudioCodes_VoiceModule::theInstance;

AudioCodes_VoiceModule * AudioCodes_VoiceModule::Create()
{
  if (theInstance == NULL)
    theInstance = new AudioCodes_VoiceModule;
  return theInstance;
}

AudioCodes_VoiceModule::AudioCodes_VoiceModule()
  : VoiceModule("audiocodes") 
{
  SetConfigTree(new Cfg_Tree_Group(Name(), "", VoiceManager::Instance()->GetConfigTree()));

  Term_Cmd *cmd = new Term_Cmd(this,Cmd_Vox_Shutdown,
      "shutdown",
      "Enable/disable AudioCodes voice driver",
      PrivilegeLevelAdmin,
      TermModeConfig,
      true);
  RegisterTermCmd(cmd, GetConfigTree());

  // By default it is disabled
  Disable();
}

/** Show configuration for the specified entry.
  This function will be called by the framework whenever it needs to display the
  specified entry. Previously this instance should tell the entry that it wishes to
  handle displaying the entry by calling entry->RegisterShowHandler().
  */
bool AudioCodes_VoiceModule::OnShowConfig( const Cfg_Tree_Entry* entry,
    String& output, 
    const String& indent, 
    int indent_size) const
{
  output += indent + String(indent_size, ' ') + "# Experimental AudioCodes voice driver (for testing only)\n";
  if (IsEnabled())
    output += indent + String(indent_size, ' ') + "no shutdown\n";
  else
    output += indent + String(indent_size, ' ') + "shutdown\n";
  return OK;
}

/** Terminal command callback
*/
void AudioCodes_VoiceModule::DoTermCmd(Term_Session *session, bool no, Term_Cmd *cmd)
{
  switch (cmd->GetId()) {
    case Cmd_Vox_Shutdown:
      ACQUIRE_SYSTEM_LOCK();
      if (no)
        Enable();
      else
        Disable();
      RELEASE_SYSTEM_LOCK();
      break;
  }
}

/** Load: Called once at initialization time
*/
bool AudioCodes_VoiceModule::Load()
{
  return true;
}

/** Unload: Called once at termination time
*/
bool AudioCodes_VoiceModule::Unload()
{
  return true;
}


///////////////////////////////////////////////////////////////////////////////

AudioCodes_VoiceDriver * AudioCodes_VoiceDriver::acVoiceDriverInstance;

AudioCodes_VoiceDriver::AudioCodes_VoiceDriver()
  : VoiceDriver("aud_voice", false, false), lastInterfaceRequest(0) 
{
}

AudioCodes_VoiceDriver::~AudioCodes_VoiceDriver() 
{
  acVoiceDriverInstance = NULL;
}

AudioCodes_VoiceDriver * AudioCodes_VoiceDriver::Create()
{
  if (acVoiceDriverInstance==NULL)
    acVoiceDriverInstance = new AudioCodes_VoiceDriver;
  return acVoiceDriverInstance;
}

#define VOICE_DIR   CD_Outgoing

VoiceDevice *AudioCodes_VoiceDriver::AllocVoiceDevice()
{
  if (!AudioCodes_VoiceModule::Instance()->IsEnabled())
    return NULL;

  AcChannel *ch = NULL;
  unsigned max_itf = AudioCodes_Manager::Instance()->GetInterfaceCount();
  for (unsigned i=0; i<max_itf; ++i) {
    if (lastInterfaceRequest >= max_itf)
      lastInterfaceRequest = 0;
    ch = AudioCodes_Manager::Instance()->AllocateAcChannel(lastInterfaceRequest, VOICE_DIR, NULL, -1);
    if (ch != NULL)
      break;

    ++lastInterfaceRequest;
  }

  if (!ch)
    return NULL;

  AudioCodes_VoiceDevice *dev = new AudioCodes_VoiceDevice(ch);
  AudioCodes_Manager::Instance()->GetInterface(ch->GetBoard()->GetNumber())->DecMediaCount(NULL, VOICE_DIR, NULL, true);

  if (!dev->Open()) {
    dev->Free();
    ++lastInterfaceRequest;
    return NULL;
  }

  ++lastInterfaceRequest;
  return dev;
}


void AudioCodes_VoiceDriver::FreeVoiceDevice(VoiceDevice *device)
{
  AudioCodes_VoiceDevice *ac_dev = (AudioCodes_VoiceDevice *)device;
  AudioCodes_Manager::Instance()->GetInterface(ac_dev->GetAcChannel()->GetBoard()->GetNumber())->IncMediaCount(NULL, VOICE_DIR, NULL, CC_NormalCallClearing, true);
  AudioCodes_Manager::Instance()->ReleaseChannel(ac_dev->GetAcChannel(), CD_Outgoing, NULL, CC_NormalCallClearing);

  delete ac_dev;
}

#endif //XGATE_HAS_VOICE


///////////////////////////////////////////////////////////////////////////////
// AudioCodes_Channel implementation

void AudioCodes_Channel::AcParam2RtpConfig(AcChannelParam& param, RTP_Config& config)
{    
  config.NIorPCI =param.GetParam().TransportSettings.UseNIorPCI;
  config.DTMF.Volume = param.GetParam().IBSSettings.DTMFVolume;

  switch (param.GetParam().IBSSettings.DTMFTransportType) {
    case acTransparentDTMF:
      config.DTMF.Transport_Type = RTP_Config::DTMF_Inband;
      break;
    case acRelayDTMF:
      config.DTMF.Transport_Type = RTP_Config::DTMF_Proprietary;
      break;
    case acRelayDTMFOverRTP:
      config.DTMF.Transport_Type = RTP_Config::DTMF_RFC2833;

      /* Set default payload types for RFC 2833.
       * Note that RX payload type is always RTP_PT_RFC_2833, while
       *        TX payload type is filled up by signaling.
       */
      config.DTMF.rfc2833_rx_pt = RTP_PT_RFC_2833;

      // Do not set TX payload type, as this may have been filled up
      // by signaling.
      // config.DTMF.rfc2833_tx_pt = RTP_PT_RFC_2833
      break;
    default:
      config.DTMF.Transport_Type = RTP_Config::DTMF_Inband;
      break;
  }

  config.EchoCanceller = param.GetParam().VoiceSettings.ECE?true:false;
  config.HighPassFilter = param.GetParam().VoiceSettings.HPFE?true:false;
  config.InputGain = param.GetParam().VoiceSettings.InputGain;
  config.OutputGain = param.GetParam().VoiceSettings.VoiceVolume;    
  config.PosFilter = param.GetParam().VoiceSettings.PFE?true:false;
  config.RTP.Codec = AC2PayloadCode(param.GetParam().VoiceSettings.Coder);
  config.RTP.MinJitter = param.GetParam().DJBSettings.DJBufMinDelay;
  config.RTP.OptimiztionFactor = param.GetParam().DJBSettings.DJBufOptFactor;
  config.RTP.PackingFactor = param.GetParam().VoiceSettings.M;
  config.SilenceCompression = param.GetParam().VoiceSettings.SCE?true:false;
  switch (param.GetParam().VoiceSettings.ECLength) {
    case acECLength15MSec:
      config.ECTailLen = 15;
      break;
    case acECLength20MSec:
      config.ECTailLen = 20;
      break;
    case acECLength25MSec:
      config.ECTailLen = 25;
      break;
    case acECLength32MSec:
      config.ECTailLen = 32;
      break;
    case acECLength35MSec:
      config.ECTailLen = 35;
      break;
    case acECLength40MSec:
      config.ECTailLen = 40;
      break;
    case acECLength45MSec:
      config.ECTailLen = 45;
      break;
    case acECLength50MSec:
      config.ECTailLen = 50;
      break;
    case acECLength55MSec:
      config.ECTailLen = 55;
      break;
    case acECLength60MSec:
      config.ECTailLen = 60;
      break;
    case acECLength64MSec:
      config.ECTailLen = 64;
      break;
    case acECLength128MSec:
      config.ECTailLen = 128;
      break;
    default:
      config.ECTailLen = 0;
  }
}

void AudioCodes_Channel::RtpConfig2AcParam(RTP_Config& config, AcChannelParam& param)
{
  acTExtChannelParam acparam = param.GetParam();

  acparam.TransportSettings.UseNIorPCI = config.NIorPCI;
  acparam.IBSSettings.DTMFVolume = config.DTMF.Volume;

  switch (config.DTMF.Transport_Type) {
    case RTP_Config::DTMF_Inband:
      acparam.IBSSettings.DTMFTransportType = acTransparentDTMF;
      break;
    case RTP_Config::DTMF_Proprietary:
      acparam.IBSSettings.DTMFTransportType = acRelayDTMF;
      break;
    case RTP_Config::DTMF_RFC2833:
      acparam.IBSSettings.DTMFTransportType = acRelayDTMFOverRTP;
      acparam.IBSSettings.MFTransportType = acRelayMFOverRTP;
      acparam.IBSSettings.Rfc2833RtpTxPayloadType = config.DTMF.rfc2833_tx_pt;
      acparam.IBSSettings.Rfc2833RtpRxPayloadType = config.DTMF.rfc2833_rx_pt;
      acparam.IBSSettings.RTPDTMFRFC2833PayloadType = config.DTMF.rfc2833_rx_pt;
      TRACE_RFC2833_("ac", "RFC2833 support activated, tx pt=%d, rx pt=%d (%d)",
          config.DTMF.rfc2833_tx_pt, 
          config.DTMF.rfc2833_rx_pt,
          acparam.IBSSettings.RTPDTMFRFC2833PayloadType);
      break;
    default:
      acparam.IBSSettings.DTMFTransportType = acTransparentDTMF;
      break;
  }


  acparam.VoiceSettings.ECE = config.EchoCanceller?1:0; 
  acparam.VoiceSettings.HPFE = config.HighPassFilter?1:0;
  acparam.VoiceSettings.InputGain = config.InputGain;
  acparam.VoiceSettings.VoiceVolume = config.OutputGain;

  acparam.VoiceSettings.PFE = config.PosFilter ? 1:0;
  acparam.VoiceSettings.Coder = PayloadCode2AC(config.RTP.Codec);
  acparam.DJBSettings.DJBufMinDelay = config.RTP.MinJitter;
  acparam.DJBSettings.DJBufOptFactor = config.RTP.OptimiztionFactor;
  acparam.VoiceSettings.M = config.RTP.PackingFactor;
  acparam.VoiceSettings.SCE = 
    (acTSilenceCompressionEnable) (config.SilenceCompression?1:0);
  if (config.ECTailLen <= 10) acparam.VoiceSettings.ECLength = acECLengthDefault;
  else if (config.ECTailLen <= 15) acparam.VoiceSettings.ECLength = acECLength15MSec;
  else if (config.ECTailLen <= 20) acparam.VoiceSettings.ECLength = acECLength20MSec;
  else if (config.ECTailLen <= 25) acparam.VoiceSettings.ECLength = acECLength25MSec;
  else if (config.ECTailLen <= 32) acparam.VoiceSettings.ECLength = acECLength32MSec;
  else if (config.ECTailLen <= 35) acparam.VoiceSettings.ECLength = acECLength35MSec;
  else if (config.ECTailLen <= 40) acparam.VoiceSettings.ECLength = acECLength40MSec;
  else if (config.ECTailLen <= 45) acparam.VoiceSettings.ECLength = acECLength45MSec;
  else if (config.ECTailLen <= 50) acparam.VoiceSettings.ECLength = acECLength50MSec;
  else if (config.ECTailLen <= 55) acparam.VoiceSettings.ECLength = acECLength55MSec;
  else if (config.ECTailLen <= 60) acparam.VoiceSettings.ECLength = acECLength60MSec;
  else if (config.ECTailLen <= 64) acparam.VoiceSettings.ECLength = acECLength64MSec;
  else acparam.VoiceSettings.ECLength = acECLength128MSec;

  param = AcChannelParam(acparam);
}


  AudioCodes_Channel::AudioCodes_Channel(AcChannel* acchannel)
: channel_(acchannel), switchdevice_(NULL), eventCallback(*this, acchannel),
  rtpcallback_(NULL)
{
}

void AudioCodes_Channel::SetCodec(Rtp_Payload_Code codec)
{
  AcChannelParam & param = channel_->GetRunningParam();
  param.SetCodec(PayloadCode2AC(codec));    
}

bool AudioCodes_Channel::Open(RTP_Config& config)
{
  config_ = config;

  AcChannelParam param = channel_->GetRunningParam();

  RtpConfig2AcParam(config, param);

  if (switchdevice_->SupportSCbus(NULL)) {
    SCbusAddress *sc_addr = (SCbusAddress *)switchdevice_->GetAddress(SF_SCbus);
    assert (sc_addr);
    if (sc_addr != NULL)
      param.SetTxTimeslot(sc_addr->GetTimeslot());

  } else if (switchdevice_->SupportCTbus(NULL)) {
    CTbusAddress *ct_addr = (CTbusAddress *)switchdevice_->GetAddress(SF_CTbus);
    assert (ct_addr);
    if (ct_addr != NULL) {
      param.GetParam().TDMBusSettings.TDMBusOutputPort = ct_addr->GetStream();
      param.GetParam().TDMBusSettings.TDMBusOutputChannel = ct_addr->GetTimeslot();
    }

  } else
    return false;

#ifdef AC_LOOPBACK_TIMESLOT
  /*
   * Initial channel configuration:
   *	- output timeslot is set to the allocated address (done above)
   *	- output disabled
   -	- input listens to itself
   */
  param.EnableTDMOutput(false);
  param.GetParam().TDMBusSettings.TDMBusInputPort = param.GetParam().TDMBusSettings.TDMBusOutputPort;
  param.GetParam().TDMBusSettings.TDMBusInputChannel = param.GetParam().TDMBusSettings.TDMBusOutputChannel;
#else
  param.EnableTDMOutput(false);
  //
  // TODO:
  //	the channel initially listens to timeslot 1023, whathever that timeslot
  //	contains.
  //
  // param.SetRxTimeslot(1023);
#endif

  channel_->SetRunningParam(param);

  return true;
}

bool AudioCodes_Channel::Activate(const ACE_INET_Addr& addr, Rtp_Payload_Code code, bool low)
{
  if(!switchdevice_)
    return false;

  // Open the channel.
  AcChannelParam & param = channel_->GetRunningParam();
  acTCoders codec = PayloadCode2AC(code);

  if (codec == acG723High && low)
    codec = acG723Low;

  param.SetCodec(codec);  
  if (!channel_->Open(param))
    return false;

  channel_->GetBoard()->HandleEvents();

  // Activate switching   
  //switchdevice_->Activate(*addr.getsockaddr_in()); //Yoga
  const sockaddr_in *sck_addr_in = (sockaddr_in *)addr.get_addr();
  switchdevice_->Activate(*sck_addr_in);
  return true;
}

SwitchDevice *AudioCodes_Channel::GetSwitchDevice(const char* name)
{
  //lazy creation of switch device
  if(!switchdevice_){
    switchdevice_ = new AudioCodes_Switch_Device(name, channel_);
  }

  return switchdevice_;
}

void AudioCodes_Channel::Close()
{
  AcBoard *board = channel_->GetBoard();
  channel_->SetEventCallback(NULL);
  channel_->Close();
  board->HandleEvents();
  //channel_ = NULL;
}

int AudioCodes_Channel::GetInitializationStatus() const
{
  return channel_->GetInitializationStatus();
}

time_t AudioCodes_Channel::GetLastActivateTime()
{
  return channel_->GetLastActivateRtpTime();
}

void AudioCodes_Channel::DialDtmf(const char* dtmf)
{
  channel_->DialDtmf(dtmf);
}

void AudioCodes_Channel::ReleaseChannel(CallDirection cd, CallParameter* cp, CallCause cc)
{
  AudioCodes_Manager::Instance()->ReleaseChannel(channel_, cd, cp, cc);
}

const sockaddr_in& AudioCodes_Channel::GetLocalAddress()
{
  return channel_->GetLocalAddress();
}

int ARRAY_SIZE(AudioCodes_Channel::ac_item_t* arr)
{
  for(int i=0;arr[i].name != NULL;i++);

  return i;
}

Rtp_Payload_Code AudioCodes_Channel::AC2PayloadCode(int ac_codec)
{
  for (unsigned i=0;i<ARRAY_SIZE(rtp_codec_); ++i) {
    if (rtp_codec_[i].id2 == ac_codec)
      return (Rtp_Payload_Code)rtp_codec_[i].id;
  }

  return ac_codec == acG723Low ? RTP_PT_G7231 : RTP_PT_INVALID;
}

Rtp_Payload_Code AudioCodes_Channel::GetSelectedPayload()
{
  const AcChannelParam & param = channel_->GetRunningParam();
  return AC2PayloadCode(param.GetParam().VoiceSettings.Coder);
}

bool AudioCodes_Channel::PrepareSwitching(RTP_Config* conf)
{
  return false;
}

acTCoders AudioCodes_Channel::PayloadCode2AC(Rtp_Payload_Code pt)
{
  for (unsigned i=0;i<ARRAY_SIZE(rtp_codec_); ++i) {
    if (rtp_codec_[i].id == pt)
      return (acTCoders) rtp_codec_[i].id2;
  }
  return (acTCoders)-1;
}

AudioCodes_Channel::ac_item_t AudioCodes_Channel::rtp_codec_[] = {
  { RTP_PT_PCMU,	    acG711Mulaw,    "pcmu",	"G.711 Ulaw (PCM Ulaw) @ 64 Kbps"},
  { RTP_PT_G726,	    acG726_16,	    "g726",	"G.726 @ 16 Kbps"},
  { RTP_PT_GSM,	    acGSM610,	    "gsm",	"GSM 06.10 @ 13 Kbps"},
  { RTP_PT_G7231,	    acG723High,	    "g723",	"G.723.1 @ 6.3/5.3 Kbps"},
  //    { RTP_PT_G7231,	    acG723Low,	    "g723",	"G.723.1 @ 5.3 Kbps"},
  { RTP_PT_DVI4_8000,	    -1,		    "dvi8",	"DVI4 @ 8 Khz"},
  { RTP_PT_DVI4_16000,    -1,		    "dvi15",    "DVI4 @ 16 Khz"},
  { RTP_PT_LPC,	    -1,		    "lpc",	"LPC"},
  { RTP_PT_PCMA,	    acG711Alaw_64,  "pcma",	"G.711 Alaw (PCM Alaw) @ 64 Kbps"},
  { RTP_PT_G722,	    -1,		    "g722",	"G.722"},
  { RTP_PT_L16_44100_2,   -1,		    "l16.44.2",	"Raw 16 bit @ 44100 Hz stereo"},
  { RTP_PT_L16_44100_1,   -1,		    "l16.44.1",	"Raw 16 bit @ 44100 Hz mono"},
  { RTP_PT_QCELP,	    -1,		    "qcelp",    "QCELP"},
  { RTP_PT_G728,	    acG728,	    "g728",	"G.728"},
  { RTP_PT_DVI4_11025,    -1,		    "dvi11",    "DVI4 @ 11025 Hz"},
  { RTP_PT_DVI4_22050,    -1,		    "dvi22",    "DVI4 @ 22050 Hz"},
  { RTP_PT_G729,	    acG729,	    "g729",	"G.729"},
  { RTP_PT_Netcoder_4,    acNetCoder_4_8, "nc4.8",	"Netcoder @ 4.8 Kbps"},
  { RTP_PT_Netcoder_5,    acNetCoder_5_6, "nc5.6",	"Netcoder @ 5.6 Kbps"},
  { RTP_PT_Netcoder_6,    acNetCoder_6_4, "nc6.4",	"Netcoder @ 6.4 Kbps"},
  { RTP_PT_Netcoder_7,    acNetCoder_7_2, "nc7.2",	"Netcoder @ 7.2 Kbps"},
  { RTP_PT_Netcoder_8,    acNetCoder_8,   "nc8.0",	"Netcoder @ 8 Kbps"},
  { RTP_PT_Netcoder_88,   acNetCoder_8_8, "nc8.8",	"Netcoder @ 8.8 Kbps"},
  { RTP_PT_Netcoder_9,    acNetCoder_9_6, "nc9.6",	"Netcoder @ 9.6 Kbps"},
  { 0, 0, NULL, NULL }
};

int AudioCodes_Channel::GetChannelNumber()
{
  return channel_->GetNumber();
}

int AudioCodes_Channel::GetMediaID()
{
  return (channel_->GetBoard()->GetNumber() << 16) + channel_->GetNumber();

}

const RTP_Config& AudioCodes_Channel::GetRtpConfig()
{
  AcChannelParam& param = channel_->GetRunningParam();
  AcParam2RtpConfig(param, config_);
  return config_;
}

const void AudioCodes_Channel::GetLocalAddress(ACE_INET_Addr& addr)
{
  AudioCodes_Manager::Instance()->GetChannelLocalAddress(channel_, addr);
}

const AcRtcpStat& AudioCodes_Channel::GetRtcpStat()
{
  return channel_->GetRtcpStat();
}

void AudioCodes_Channel::SetRtpConfig(RTP_Config& config)
{
  AcChannelParam param = channel_->GetRunningParam();
  RtpConfig2AcParam(config, param);
  config_ = config;
  channel_->SetRunningParam(param);
}

Rtp_Payload_Code AudioCodes_Channel::GetCodec()
{
  return AC2PayloadCode(channel_->GetRunningParam().GetParam().VoiceSettings.Coder);
}

#endif //XGATE_HAS_NEW_AUDIOCODES
#endif //XGATE_HAS_AUDIOCODES
