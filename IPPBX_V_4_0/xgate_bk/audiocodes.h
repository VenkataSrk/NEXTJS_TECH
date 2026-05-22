/* $Header: /xGate/audiocodes.h 20    9/23/02 2:46p Bennylp $
 *   
 * XGATE
 * (C) 2001 Switchlab, Ltd
 *
 * AudioCodes
 *
 * $Log: /xGate/audiocodes.h $
 * 
 * 20    9/23/02 2:46p Bennylp
 * Changed macro USE_NEW_RTP to xgconfig.h
 * 
 * 19    4/09/02 12:02 Adham
 * 
 * 18    21/08/02 11:03 Adham
 * Fix interface RTP_Channel changes.
 * 
 * 17    17/07/02 14:30 Adham
 * Add rtp board type
 * 
 * 16    10/07/02 20:15 Adham
 * Added code to support new RTP, the code can be disabled using
 * XGATE_USE_NEW_RTP define on rtpchan.h
 * 
 * 15    4/29/02 5:50p Bennylp
 * Changed board startup at CommitCfg, and added serial num reg.
 * 
 * 14    4/24/02 6:45p Bennylp
 * Use next channel in round-robin fashion.
 * 
 * 13    7/11/01 19:35 Bennylp
 * Fixed misplaced assertion
 * 
 * 12    11/06/01 7:53p Bennylp
 * Change RTP media initialization to two stages: Open and Activate
 * 
 * 11    8/13/01 11:38a Bennylp
 * Added RTCP statistics monitoring.
 * 
 * 10    8/01/01 7:00p Bennylp
 * AudioCodes will loopback to itself again.
 * 
 * 9     7/31/01 7:41p Bennylp
 * Added barebone voice module for configuration entry point.
 * 
 * 8     7/31/01 6:52p Bennylp
 * Barebones for AudioCodes voice driver.
 * 
 * 7     7/28/01 8:40p Bennylp
 * AudioCodes will pre-allocate switch addresses.
 * 
 * 6     7/23/01 1:39p Bennylp
 * Implement OnClearCounter.
 * 
 * 5     7/20/01 4:56p Bennylp
 * Bug: Aud interface is destroyed but still used by RTP to update stat.
 * 
 * 4     7/19/01 2:27p Bennylp
 * Added audioCodes board statistics.
 * 
 * 3     7/18/01 10:26p Bennylp
 * AudioCodes interface can be shutdown at runtime.
 * 
 * 2     6/26/01 5:14p Bennylp
 * Miscelanous bug fixes.
 * 
 * 1     6/22/01 7:43p Bennylp
 * Initial coding.
 * 
 */
#ifndef __XGATE_AUDIOCODES_H__
#define __XGATE_AUDIOCODES_H__
#define XGATE_HAS_NEW_AUDIOCODES
#ifdef XGATE_HAS_NEW_AUDIOCODES
#   include "../AudioCodes.v46/include/audiocodes.h"
#else
#   include "../AudioCodes/include/audiocodes.h"
#endif
#include "MediaManager.h"
#include "SCbusMgr.h"
#include "CTbusMgr.h"

#ifdef XGATE_HAS_VOICE
#include "voicedevice.h"
#include "voicemanager.h"
#endif

#include "rtpchan.h"

#define AC_MAX_BOARD	32
#define AC_MAX_CHANNEL	2048

#ifndef XGATE_HAS_NEW_AUDIOCODES
#  define AC_PREALLOCATE_SW_ADDR
#endif


/* Forward decl.
*/
class AudioCodes_Interface;
class AudioCodes_Switch_Device;

///////////////////////////////////////////////////////////////////////////////

class AudioCodes_Channel : public RTP_Channel
{
  public:    
    AudioCodes_Channel(AcChannel* acchannel);    

    virtual const AcRtcpStat& GetRtcpStat();
    virtual const RTP_Config& GetRtpConfig();
    virtual void SetRtpConfig(RTP_Config& config);
    virtual int GetChannelNumber();
    virtual int GetMediaID();
    virtual const void GetLocalAddress(ACE_INET_Addr& addr);
    virtual Rtp_Payload_Code GetCodec();

    RTP_Board_Type GetBoardType() const { return RTP_Board_AC;};
    /** Set payload code, supported to provided backward 
      compatibility with current RTP_Media	
      */
    virtual void SetCodec(Rtp_Payload_Code code);

    /** Open the rtp channel. Some code on the RTP_Media::Open() that 
      depends on AudioCode frame work extracted to RTP_Channel decendant.
Notes:
From this state the channel ready to be activated
For Audiocodes, it sets certain param and switch device props
For Mapletree, it sets certain param and initialize the port
*/
    virtual bool Open(RTP_Config& config);

    /** Activate the rtp channel. Some code on the RTP_Media that 
      depends on AudioCode frame work extracted to RTP_Channel. decendant.
      */
    virtual bool Activate(const ACE_INET_Addr& addr, Rtp_Payload_Code code, bool low);

    /** Create the switch device for the channel. 
      This is extracted from RTP_Media to remove the dependency to AudioCode.
      */
    virtual SwitchDevice *GetSwitchDevice(const char* name);

    /** Close the channel. This is extracted from RTP_Media. 
      The code will be placed in the RTP_Channel dependant.
      */
    virtual void Close();

    /** Compatibility with AcChannel.	
    */
    virtual int GetInitializationStatus() const;

    /** Compatibility with AcChannel.
    */
    virtual time_t GetLastActivateTime();

    /** Compatibility with AcChannel.
    */
    virtual void DialDtmf(const char* dtmf);

    /** Release Rtp Channel. Code extracted to RTP_Media decendants 
      to remove dependency.
      */
    virtual void ReleaseChannel(CallDirection cd, CallParameter* cp, CallCause cc);

    /** Compatibility with AcChannel.
    */
    virtual const sockaddr_in& GetLocalAddress();

    /** Compatibility with AcChannel.
    */
    virtual Rtp_Payload_Code GetSelectedPayload();

    /** Extracted method from RTP_Media to prepare switching device.
    */
    virtual bool PrepareSwitching(RTP_Config* config);

    void SetEventCallback(RTP_Channel_Callback *callback) {rtpcallback_=callback;}


    static Rtp_Payload_Code AC2PayloadCode(int ac_codec);
    static acTCoders PayloadCode2AC(Rtp_Payload_Code pt);
    static void AcParam2RtpConfig(AcChannelParam& param, RTP_Config& config);
    static void RtpConfig2AcParam(RTP_Config& config, AcChannelParam& param);

    struct ac_item_t {
      int  id;
      int	 id2;
      const char *name;
      const char *desc;
    };

  protected:
    void OnChannelError(){
      if(rtpcallback_)
        rtpcallback_->OnError(this);
    }

    static ac_item_t rtp_codec_[];
    AcChannel* channel_;
    AudioCodes_Switch_Device* switchdevice_;

    struct EventCallback : public AcChannelEventCallback {
      AudioCodes_Channel   &ac;
      AcChannel *channel;
      bool       destUnreach;

      EventCallback (AudioCodes_Channel& cha, AcChannel *ch) : ac(cha), channel(ch), destUnreach(false) {
        channel->SetEventCallback(this);
      }
      ~EventCallback() {
        channel->SetEventCallback(NULL);
      }
      virtual void OnEvent(int event, const acTEventInfo & event_info) 
      {
        if ((event==acEV_DEBUG || event==acEV_ERROR) && 
            event_info.ErrorReport.ErrorCode == TP_DESTINATION_IP_ADDRESS_UNREACHABLE_ERROR) 
        {
          destUnreach = true;
          ac.OnChannelError();
        }
      }
    } eventCallback;

    RTP_Channel_Callback * rtpcallback_;
    friend struct EventCallback;    
};



///////////////////////////////////////////////////////////////////////////////
class AudioCodes_Manager : public ServiceProvider {
  public:
    static AudioCodes_Manager * Create();
    static AudioCodes_Manager * Instance() {
      return acInstance;
    }

    /** This function will be called by the service framework after 
      instantiation of this class, to allow the instance to initialize
      itself.
      */
    virtual bool Load();

    /** This function will be called by the service framework before it is
      destroyed/deleted, to allow the instance to cleanup itself.
      */
    virtual bool Unload();

    /** HandleEvents: Descendent shall implement this abstract method.
      It is called by Service Provider Manager
      */
    virtual void HandleEvents();

    /** Get AcLibrary instance.
    */
    AcLibrary & AcLib() {
      return acLib;
    }

    /** Find a board from a name.
    */
    AcBoard *FindBoard(const char *name);

    /** Allocate a channel from the specified board.
    */
    AcChannel * AllocateAcChannel(int boardId, CallDirection dir, CallParameter *cparam, int chId);

    /** Allocate a channel from the specified board.
    */
    AudioCodes_Channel *AllocateChannel(int boardId, CallDirection dir, CallParameter *cparam, int chId);

    /** Release channel.
    */
    void ReleaseChannel (AcChannel *ch,CallDirection dir,CallParameter *cparam,CallCause cc);

    /** Get channel's local address.
    */
    bool GetChannelLocalAddress(AcChannel *channel, ACE_INET_Addr & addr);

    /** Get number of interfaces.
    */
    unsigned GetInterfaceCount() const {
      return acAudInterface.size();
    }

    /** Get audiocodes interface.
    */
    AudioCodes_Interface * GetInterface(unsigned id) {
      if (id >= acAudInterface.size()) {
        assert(false);
        return NULL;
      }
      return acAudInterface[id];
    }

    /** Remove interface from the interface list.
    */
    void UnregisterInterface(AudioCodes_Interface *itf) {
      Vector<AudioCodes_Interface*>::iterator it = 
        std::find(acAudInterface.begin(), acAudInterface.end(), itf);
      if (it != acAudInterface.end())
        *it = NULL;
    }

  private:
    Vector <AcBoardParam> acBoardParam;
    Vector <AudioCodes_Interface*> acAudInterface;
    AcLibrary acLib;
    static AudioCodes_Manager *acInstance;
    bool channelAllocationMap[AC_MAX_BOARD][AC_MAX_CHANNEL];
    bool *nextChannel[AC_MAX_BOARD];

    AudioCodes_Manager();
    ~AudioCodes_Manager();
};


///////////////////////////////////////////////////////////////////////////////
class AudioCodes_Interface : public Interface {
  public:
    AudioCodes_Interface (int board_index);
    ~AudioCodes_Interface();

    /** AllocateMedia: this interface doesn't create media!
    */
    virtual bool AllocateMedia(Call*,Media *&,SelectionStrategy,CallDirection,CallParameter*);

    /** AllocateMedia: this interface doesn't create media!
    */
    virtual bool AllocateMedia(Call*,Media*&,MediaId,CallDirection,CallParameter*);

    /** ReleaseMedia: this interface doesn't create media!
    */
    virtual void ReleaseMedia(Media*,CallDirection,CallParameter*,CallCause);

    /** Load: Called once at initialization time
    */
    virtual bool Load();

    /** Unload: Called once at termination time
    */
    virtual bool Unload();

    /** This function will be called by the service framework once the 
      configuration file has been fully read.
      */
    virtual bool CommitConfiguration();

    /** Open: May be called several times to start the object
      The object may be then in In-Service (enabled/disabled) state
      */
    virtual bool Start();

    /** Close: Once started, an object may be requested to close
      The object goes to the Out-Of-Service state
      */
    virtual bool Stop();

    /** HandleEvents: Descendent shall implement this abstract method.
      It is called by Service Provider Manager
      */
    virtual void HandleEvents();

#ifdef AC_PREALLOCATE_SW_ADDR
    /** Get switch address for the specified channel ID.
    */
    SwitchAddress * GetChannelSwitchAddress(unsigned id) {
      if (id >= chSwitchAddress.size())
        return NULL;
      return chSwitchAddress[id];
    }
#endif


    /** Show configuration for the specified entry.
    */
    virtual bool OnShowConfig(
        const Cfg_Tree_Entry *entry,
        String & output, 
        const String & indent, 
        int indent_size) const;

    /** Display interface specific information to the terminal user.
    */
    virtual void OnShowInterface(Term_Session * session );

    /** Clear error counters.
    */
    virtual void OnClearCounter();

    /** Terminal command callback
    */
    virtual void DoTermCmd(Term_Session *, bool no, Term_Cmd *);

  private:
    unsigned boardId;
    AcBoard *acBoard;
    AcBoardParam boardParam;
#ifdef AC_PREALLOCATE_SW_ADDR
    Vector<SwitchAddress*> chSwitchAddress;
#endif
    bool inStartProgram;

    void InitTermCommand();

    bool StartBoard();
    void StopBoard();
    void FreeChannelAddresses();
};


///bennylp: commented out for new TP260
///#define AC_LOOPBACK_TIMESLOT
///#define AC_DEBUG_SWITCHING

#define AC_HAS_DELAYED_SWITCHING
//#define AC_TRACE_		    CLog::Info
#define AC_TRACE_		    CLog::Debug


///////////////////////////////////////////////////////////////////////////////
/** TDM bus switching for AudioCodes media.
 *
 *  General AudioCodes switching mechanism:
 *  
 */
class AudioCodes_Switch_Device : public SwitchDevice {
  public:
    AudioCodes_Switch_Device(const char *name, AcChannel *channel)
      : SwitchDevice(name), acChannel(channel), txSCAddress(NULL), txCTAddress(NULL),
      delayedActivation(false), delayedRxAddr(NULL)
  {
    memset(&remoteAddr, 0, sizeof(remoteAddr));

    unsigned board_id = channel->GetBoard()->GetNumber();
    unsigned ch_id = channel->GetNumber();

    // Save the corresponding switch address
    const AcBoardParam & bparam = channel->GetBoard()->GetRunningParam();
#ifdef AC_PREALLOCATE_SW_ADDR
    switch (bparam.GetParam().TDMBusSettings.TDMBusType) {
      case acSC_BUS:
        txSCAddress = (SCbusAddress*) AudioCodes_Manager::Instance()->GetInterface(board_id)->GetChannelSwitchAddress(ch_id);
        break;
      case acH100_BUS:
        txCTAddress = (CTbusAddress*) AudioCodes_Manager::Instance()->GetInterface(board_id)->GetChannelSwitchAddress(ch_id);
        break;
    }
#else
    switch (bparam.GetParam().TDMBusSettings.TDMBusType) {
      case acSC_BUS:
        txSCAddress = (SCbusAddress*) SCbusManager::Instance()->AllocateAddress(name);
        break;
      case acH100_BUS:
        txCTAddress = (CTbusAddress*) CTbusManager::Instance()->AllocateAddress(name);
        break;
    }
#endif
  }

    ~AudioCodes_Switch_Device()
    {
#ifndef AC_PREALLOCATE_SW_ADDR
      if (txSCAddress)
        SCbusManager::Instance()->FreeAddress(txSCAddress);
      if (txCTAddress)
        CTbusManager::Instance()->FreeAddress(txCTAddress);
#endif
    }

    /** Set remote RTP media address.
    */
    bool Activate(const sockaddr_in & addr) 
    {
      // Don't try to call this twice!
      if (remoteAddr.sin_addr.s_addr != 0) {
        assert(false);
        return false;
      }

      remoteAddr = addr;

      //
      // If previously application has asked us to transmit/listen, then 
      // activate RTP now.
      //
      if (delayedActivation) {
        SwitchFabric fabric = SupportCTbus(NULL) ? SF_CTbus :
          (SupportSCbus(NULL) ? SF_SCbus : SF_Invalid);

        if (fabric == SF_Invalid)
          return false;

        AC_TRACE_("aud", "Activating delayed switching");

        SwitchAddress *tx_addr = NULL;
        if (fabric==SF_SCbus)
          tx_addr = txSCAddress;
        else
          tx_addr = txCTAddress;
        if (tx_addr)
          DeviceStartTransmit(fabric, tx_addr);
        if (delayedRxAddr)
          DeviceStartReceive(fabric, delayedRxAddr);
      }
      return true;
    }

    /** The framework will query the device to find out which 
      switching fabric is supported by this device to exchange TDM
      data with another device.
      */
    virtual SwitchFabric GetCompatibleFabric(SwitchDevice *dev)
    {
      assert (acChannel);
      if (acChannel==NULL)
        return SF_Invalid;

      const AcBoardParam & board_param = acChannel->GetBoard()->GetRunningParam();

      if (board_param.GetParam().TDMBusSettings.TDMBusType == acSC_BUS) {
        if (dev->SupportSCbus(this))
          return SF_SCbus;
      }
      if (board_param.GetParam().TDMBusSettings.TDMBusType == acH100_BUS) {
        if (dev->SupportCTbus(this))
          return SF_CTbus;
      }
      return SF_Invalid;
    }


    /** It must be overridden by the descendant if it support
      H.100/H.110 switching fabric
      */
    virtual bool SupportCTbus(SwitchDevice *requester)
    {
      assert (acChannel);
      if (acChannel==NULL)
        return false;

      const AcBoardParam & board_param = acChannel->GetBoard()->GetRunningParam();
      return board_param.GetParam().TDMBusSettings.TDMBusType == acH100_BUS;
    }

    /** It must be overridden by the descendant if it support
      SCSA switching fabric
      */
    virtual bool SupportSCbus(SwitchDevice *requester)
    {
      assert (acChannel);
      if (acChannel==NULL)
        return false;

      const AcBoardParam & board_param = acChannel->GetBoard()->GetRunningParam();
      return board_param.GetParam().TDMBusSettings.TDMBusType == acSC_BUS;
    }


    /** This function is called when the framework decides that it is
      the first time the specific device transmits to a particular
      fabric.
      */
    virtual SwitchAddress *AllocateAddress(SwitchFabric fabric)
    {
      return GetAddress(fabric);
    }

    /** This function is called when the framework detects that no device
      is currently listening to this device.
      */
    virtual void FreeAddress(SwitchFabric fabric)
    {
      // Do nothing.
    }

    /** It returns the address used for switching in the specified fabric
    */
    virtual SwitchAddress *GetAddress(SwitchFabric fabric)
    {
      if (fabric==SF_SCbus) {
        assert (txSCAddress!=NULL);
        return txSCAddress;
      } else if (fabric==SF_CTbus) {
        assert (txCTAddress!=NULL);
        return txCTAddress;
      } else {
        // Invalid fabric requested!
        assert (false);
        return NULL;
      }
    }


    /** It instructs the device to start transmitting to a certain location 
      using the specified switch fabric.
      */
    virtual void DeviceStartTransmit(SwitchFabric fabric,SwitchAddress *addr)
    {
      AcChannelParam & param = acChannel->GetRunningParam();

      // If we don't have remote RTP address yet, delay the transmit.
#ifdef AC_HAS_DELAYED_SWITCHING
      if (acChannel->GetInitializationStatus() < 2 && remoteAddr.sin_addr.s_addr == 0) {

        AC_TRACE_("aud", "%s: DeviceStartTransmit is delayed because remote RTP "
            "address is not available", this->GetName());
        delayedActivation = true;
        return;

        // Activate RTP if it is not activated
      } else if (acChannel->GetInitializationStatus() < 2 && remoteAddr.sin_addr.s_addr!=0) {
        assert(acChannel->GetInitializationStatus() >= 1);
        acChannel->ActivateRtp(remoteAddr);
        acChannel->GetBoard()->HandleEvents();
      }
#endif

      assert (acChannel && acChannel->GetInitializationStatus() >= 1);

      if (fabric==SF_SCbus) {
        SCbusAddress *scaddr = (SCbusAddress*)addr;
        assert(scaddr->GetTimeslot()==txSCAddress->GetTimeslot());

        int current_ts = AcLibrary::PortChannel2ScTs(param.GetParam().TDMBusSettings.TDMBusOutputPort,
            param.GetParam().TDMBusSettings.TDMBusOutputChannel);

        if (current_ts != scaddr->GetTimeslot() || param.GetParam().TDMBusSettings.TDMBusOutputDisable) {
          param.EnableTDMOutput(true);
          param.SetTxTimeslot(txSCAddress->GetTimeslot());
          acChannel->ChangeParameter();
          acChannel->GetBoard()->HandleEvents();

          AC_TRACE_("aud", "TDM StartTransmit to SCBUS %s, outgain=%d", 
              addr->ToString(), 
              param.GetParam().VoiceSettings.VoiceVolume);

        }
      } else if (fabric==SF_CTbus) {
        CTbusAddress *ctaddr = (CTbusAddress*)addr;
        assert(ctaddr->GetStream()==txCTAddress->GetStream() &&
            ctaddr->GetTimeslot()==txCTAddress->GetTimeslot());

        if (param.GetParam().TDMBusSettings.TDMBusOutputPort != ctaddr->GetStream() ||
            param.GetParam().TDMBusSettings.TDMBusOutputChannel != ctaddr->GetTimeslot() || 
            param.GetParam().TDMBusSettings.TDMBusOutputDisable) 
        {
          param.EnableTDMOutput(true);
          param.GetParam().TDMBusSettings.TDMBusOutputPort = ctaddr->GetStream();
          param.GetParam().TDMBusSettings.TDMBusOutputChannel = ctaddr->GetTimeslot();
          acChannel->ChangeParameter();
          acChannel->GetBoard()->HandleEvents();

          AC_TRACE_("aud", "TDM StartTransmit to CTBUS %s, outgain=%d", 
              addr->ToString(), 
              param.GetParam().VoiceSettings.VoiceVolume);
        }
      } else
        assert(false);
    }

    /** It instructs the device to start listening from a certain location 
      using the specified switch fabric.
      */
    virtual void DeviceStartReceive(SwitchFabric fabric,SwitchAddress *addr)
    {
      AcChannelParam & param = acChannel->GetRunningParam();

#ifdef AC_HAS_DELAYED_SWITCHING
      // If we don't have remote RTP address yet, delay the transmit.
      if (acChannel->GetInitializationStatus() < 2 && remoteAddr.sin_addr.s_addr == 0) {

        AC_TRACE_("aud", "%s: DeviceStartReceive is delayed because remote RTP "
            "address is not available", this->GetName());
        delayedActivation = true;
        delayedRxAddr = addr;
        return;

        // Activate RTP if it is not activated
      } else if (acChannel->GetInitializationStatus() < 2 && remoteAddr.sin_addr.s_addr!=0) {
        assert(acChannel->GetInitializationStatus() >= 1);
        acChannel->ActivateRtp(remoteAddr);
        acChannel->GetBoard()->HandleEvents();
      }
#endif

      // The channel automatically starts transmitting when opened.
      assert (acChannel && acChannel->GetInitializationStatus() >= 1);

      if (fabric==SF_SCbus) {
        SCbusAddress *scaddr = (SCbusAddress *)addr;

        int current_ts = AcLibrary::PortChannel2ScTs(param.GetParam().TDMBusSettings.TDMBusOutputPort,
            param.GetParam().TDMBusSettings.TDMBusOutputChannel);

        if (current_ts != scaddr->GetTimeslot()) {
          param.SetRxTimeslot(scaddr->GetTimeslot());
          acChannel->ChangeParameter();
          acChannel->GetBoard()->HandleEvents();

          AC_TRACE_("aud", "TDM StartReceive from SCBUS %s, ingain=%d", 
              addr->ToString(), param.GetParam().VoiceSettings.InputGain);
        }

      } else if (fabric==SF_CTbus) {
        CTbusAddress *ctaddr = (CTbusAddress *)addr;

        if (param.GetParam().TDMBusSettings.TDMBusInputPort != ctaddr->GetStream() ||
            param.GetParam().TDMBusSettings.TDMBusInputChannel != ctaddr->GetTimeslot()) 
        {
          param.GetParam().TDMBusSettings.TDMBusInputPort = ctaddr->GetStream();
          param.GetParam().TDMBusSettings.TDMBusInputChannel = ctaddr->GetTimeslot();
          acChannel->ChangeParameter();
          acChannel->GetBoard()->HandleEvents();

          AC_TRACE_("aud", "TDM StartReceive from CTBUS %s, ingain=%d", 
              addr->ToString(), param.GetParam().VoiceSettings.InputGain);

        }

      } else
        assert(false);
    }

    /** It instructs the device to stop transmitting to a certain location 
      using the specified switch fabric. It is called when the reference
      count of the specified switch fabric reach 0.
      */
    virtual void DeviceStopTransmit(SwitchFabric fabric,SwitchAddress *addr)
    {
      // Check if channel is already closed.
      if (!acChannel || acChannel->GetInitializationStatus() < 1)
        return;

      AC_TRACE_("aud", "TDM StopTransmit to %s", addr->ToString());

      ///bennylp: commented out for new TP260
      ///#ifdef AC_LOOPBACK_TIMESLOT
      //
      // On stop transmit, disable TDM output
      //
      AcChannelParam & param = acChannel->GetRunningParam();
      param.EnableTDMOutput(false);
      acChannel->ChangeParameter();
      acChannel->GetBoard()->HandleEvents();
      ///#else
      // Do nothing
      ///#endif
    }

    /** It instructs the device to stop listening from a certain location 
      using the specified switch fabric.
      This function needs to be overridden by all descendant classes.
      */
    virtual void DeviceStopReceive(SwitchFabric fabric,SwitchAddress *addr)
    {
      // Check if channel is already closed.
      if (!acChannel || acChannel->GetInitializationStatus() < 1)
        return;

      AC_TRACE_("aud", "TDM StopReceive from %s", addr->ToString());

#ifdef AC_LOOPBACK_TIMESLOT
      //
      // On stop receive, set TDM input to itself.
      //
      AcChannelParam & param = acChannel->GetRunningParam();
      param.GetParam().TDMBusSettings.TDMBusInputPort = param.GetParam().TDMBusSettings.TDMBusOutputPort;
      param.GetParam().TDMBusSettings.TDMBusInputChannel = param.GetParam().TDMBusSettings.TDMBusOutputChannel;
      acChannel->ChangeParameter();
      acChannel->GetBoard()->HandleEvents();
#else
      // Do nothing
#endif
    }


  private:
    AcChannel *acChannel;
    SCbusAddress *txSCAddress;
    CTbusAddress *txCTAddress;
    sockaddr_in	remoteAddr;
    bool delayedActivation;
    SwitchAddress *delayedRxAddr;
};


#ifdef XGATE_HAS_VOICE
///////////////////////////////////////////////////////////////////////////////
/** AudioCodes voice device.
*/
class AudioCodes_VoiceDevice : public VoiceDevice {
  public:
    AudioCodes_VoiceDevice(AcChannel *channel);

    bool Open();

    // deallocate this voice device
    virtual void Free();

    // must be implemented in derived class
    virtual bool IsDeviceIdle();
    virtual bool CancelJob();

    // Telephony actions, must be implemented in derived class
    virtual STATUS SetVoiceFormat(VoiceFormat format);
    virtual STATUS GetVoiceFormat(VoiceFormat *format);
    virtual STATUS EnableInterrupt();
    virtual STATUS DisableInterrupt();
    virtual STATUS SetMaxRecTime(int maxrectime);
    virtual STATUS GetMaxRecTime(int *maxrectime);
    virtual STATUS SetInterDigitTimeout(int maxidtime);
    virtual STATUS GetInterDigitTimeout(int *maxidtime);
    virtual STATUS SetDigitTimeout(int maxidtime);
    virtual STATUS GetDigitTimeout(int *maxidtime);
    virtual STATUS SetTermDigits(char *termdigs);
    virtual STATUS GetTermDigits(char *termdigs);
    virtual STATUS PlayFileArray(int nbfiles,char const **filename,bool bkgnd,bool autorepeat);
    virtual STATUS StopPlayFileBkGnd(bool waiteof);
    virtual STATUS PlayTone(int freq1,int db1,int freq2,int db2,int duration);
    virtual STATUS PlayDtmf(char *dtmf);
    virtual STATUS RecordFile(char *filename);
    virtual STATUS GetDigits(int length,char *buffer);
    virtual STATUS GetLastTermDigit(char *buffer);
    virtual STATUS ClearDigits();
    virtual STATUS PeekDigit(char *buffer);

    void OnAcEvent(int event, const acTEventInfo & event_info);
    AcChannel *GetAcChannel() {
      return acChannel;
    }
  private:
    AcChannel *acChannel;
    String     digitBuffer;
    String     termDigits;
    char       lastTermDigit;

    enum State {
      IdleState,
      DialingState,
    } state;

    struct EventCallback : public AcChannelEventCallback {
      AudioCodes_VoiceDevice & voiceDev;
      AcChannel *channel;

      EventCallback (AudioCodes_VoiceDevice & dev, AcChannel *ch) 
        : voiceDev(dev), channel(ch) {
          channel->SetEventCallback(this);
        }
      ~EventCallback() {
        channel->SetEventCallback(NULL);
      }
      virtual void OnEvent(int event, const acTEventInfo & event_info) 
      {
        voiceDev.OnAcEvent(event, event_info);
      }
    } eventCallback;
};

///////////////////////////////////////////////////////////////////////////////
/** AudioCodes voice module, which doesn't do anything except to accept
  command disable/enable from configuration.
  */

class AudioCodes_VoiceModule : public VoiceModule {
  public:
    static AudioCodes_VoiceModule * Create();
    static AudioCodes_VoiceModule * Instance() {
      return theInstance;
    }

    /** Show configuration for the specified entry.
      This function will be called by the framework whenever it needs to display the
      specified entry. Previously this instance should tell the entry that it wishes to
      handle displaying the entry by calling entry->RegisterShowHandler().
      */
    virtual bool OnShowConfig(
        const Cfg_Tree_Entry* entry,
        String& output, 
        const String& indent, 
        int indent_size) const;

    /** Terminal command callback
    */
    virtual void DoTermCmd(Term_Session *session, bool no, Term_Cmd *cmd);

    /** Load: Called once at initialization time
    */
    virtual bool Load();

    /** Unload: Called once at termination time
    */
    virtual bool Unload();


  private:
    static AudioCodes_VoiceModule *theInstance;

    AudioCodes_VoiceModule();
    ~AudioCodes_VoiceModule() 
    {
      theInstance = NULL;
    }
};


///////////////////////////////////////////////////////////////////////////////
/** Manager for AudioCodes voice devices.
*/
class AudioCodes_VoiceDriver : public VoiceDriver {
  public:
    static AudioCodes_VoiceDriver * Create();

    static AudioCodes_VoiceDriver * Instance() {
      return acVoiceDriverInstance;
    }

    /** This function will be called by the service framework after 
      instantiation of this class, to allow the instance to initialize
      itself.
      */
    virtual bool Load() {
      return true;
    }

    /** This function will be called by the service framework before it is
      destroyed/deleted, to allow the instance to cleanup itself.
      */
    virtual bool Unload() {
      return true;
    }

    virtual VoiceDevice *AllocVoiceDevice();
    virtual void FreeVoiceDevice(VoiceDevice *device);

  private:
    static AudioCodes_VoiceDriver *acVoiceDriverInstance;
    unsigned lastInterfaceRequest;

    AudioCodes_VoiceDriver();
    ~AudioCodes_VoiceDriver();
};

#endif //XGATE_HAS_VOICE

#endif	// __XGATE_AUDIOCODES_H__

