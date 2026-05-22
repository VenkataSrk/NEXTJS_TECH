/* $Header: /xGate/rtpmedia.h 35    6/06/03 4:07p Bennylp $
 *   
 * XGATE
 * (C) 2001 Switchlab, Ltd
 *
 * RTP virtual media/interface.
 *
 * $Log: /xGate/rtpmedia.h $
 * 
 * 35    6/06/03 4:07p Bennylp
 * Lots of changes to incorporate mapletree diagnostic (Version 54.4)
 * 
 * 34    6/04/03 3:47p Bennylp
 * Fixed bug when terminating channel is not re-tried.
 * 
 * 33    10/29/02 1:05p Bennylp
 * Stable version for MTN2100/1024P, tested.
 * 
 * 32    10/24/02 8:34p Bennylp
 * Latest stable checkin.
 * 
 * 31    9/23/02 2:46p Bennylp
 * Changed macro USE_NEW_RTP to xgconfig.h
 * 
 * 30    4/09/02 12:04 Adham
 * Asynch channel termination
 * 
 * 29    21/08/02 12:30 Adham
 * Added asynch mtn channel termination
 * 
 * 28    5/08/02 18:55 Adham
 * Added changes to support new rtp framework, updates is marked by #ifdef
 * XGATE_USE_NEW_RTP pre-processor, this switch resides in rtpchan.h
 * 
 * 27    10/07/02 20:04 Adham
 * Added code to support new RTP, the code can be disabled using
 * XGATE_USE_NEW_RTP define on rtpchan.h
 * 
 * 26    5/22/02 12:46p Bennylp
 * Set the src addr for ACL from socket addr rather than from cparam.
 * 
 * 25    11/02/02 16:30 Sonny
 * quality monitoring updates
 * 
 * 24    6/02/02 19:01 Sonny
 * optimizations for quality monitoring
 * 
 * 23    4/02/02 12:33 Sonny
 * calls get quality from media
 * 
 * 22    31/01/02 13:12 Sonny
 * add call quality monitoring
 * 
 * 21    24/01/02 20:58 Sonny
 * Quality monitoring support
 * 
 * 20    12/11/01 5:05p Bennylp
 * Changed to support digest authentication in xGate
 * 
 * 19    11/06/01 7:53p Bennylp
 * Change RTP media initialization to two stages: Open and Activate
 * 
 * 18    10/08/01 6:19p Bennylp
 * Added method to get RTP packing factor in RTP interface.
 * 
 * 17    8/13/01 2:50p Bennylp
 * Fixed RTCP stat calculations, do not include bad values.
 * 
 * 16    8/13/01 11:38a Bennylp
 * Added RTCP statistics monitoring.
 * 
* 15    8/09/01 6:12p Bennylp
* Changed DispatchIncomingCall interface.
* 
* 14    8/08/01 7:00p Bennylp
* Added DialDTMF().
* 
* 13    8/08/01 1:48p Bennylp
* Temporary checkin
* 
* 12    8/02/01 7:57p Bennylp
* RTP interface now uses access list.
* 
* 11    7/30/01 10:20p Bennylp
* Disconnect call in case of Destination Unreachable.
* 
* 10    7/20/01 9:08p Bennylp
* Fixed bug: rtp media is not deleted!
* 
* 9     7/19/01 6:18p Bennylp
* Interface source will be used in round robin fashion.
* 
* 8     7/19/01 4:59p Bennylp
* RTP interface can be deleted.
* 
* 7     7/19/01 3:40p Bennylp
* Use audiocodes interface in round-robin.
* 
* 6     7/19/01 2:18p Bennylp
* Changes to MediaManager functions to support rtp/aud shared stats.
* 
* 5     7/18/01 10:26p Bennylp
* AudioCodes interface can be shutdown at runtime.
* 
* 4     6/27/01 10:38p Bennylp
* Added incoming source address specification.
* 
* 3     6/26/01 5:14p Bennylp
* Miscelanous bug fixes.
* 
* 2     6/24/01 7:44p Bennylp
* Added more configuration parameters.
* 
* 1     6/22/01 7:43p Bennylp
* Initial coding.
* 
*/
#ifndef __XGATE_RTPMEDIA_H__
#define __XGATE_RTPMEDIA_H__


#include "MediaManager.h"
#include "Switching.h"

#ifdef XGATE_HAS_AUDIOCODES
#include "audiocodes.h"
#endif


#include "SignallingDriver.h"
#include "quality.h"

#include "mtnrtp.h"

//#ifdef XGATE_HAS_H248
#include "h248rls.h"
//#endif

class Rtp_Interface;
class SCbusAddress;
class CTbusAddress;
struct ACL_Token;


#define RTP_MAX_CODEC_LIST  32





/** Get the name of a codec.
*/
const char *Rtp_Payload_GetName (Rtp_Payload_Code code);

/** Get the name of a codec(formal name)
*/
const char *Rtp_Payload_GetFormalName (Rtp_Payload_Code code);

/** Get description of a codec.
*/
const char *Rtp_Payload_GetDescription (Rtp_Payload_Code code);

/** Convert a codec name to its codec code.
  @return RTP_PT_INVALID if the codec name doesn't match any of the known
  codec names.
  */
Rtp_Payload_Code Rtp_Payload_GetCode (const char *name);


///////////////////////////////////////////////////////////////////////////////
/** RTP media channel.
*/
class Rtp_Media : public Media, public Rtp_Quality_Source {
  public:
    /** Construct the Rtp_Media instance.
    */
    Rtp_Media(const char *name,
        MediaId id,
        Rtp_Interface *itf,
        Interface *phy_itf,
        RTP_Channel *channel,
        void *itf_src);

    ~Rtp_Media();

    /** Get list of supported codec.
      Upon return, the 'payloads' array parameter will be filled in with
      the codecs, up to 'maxnum' number.
      @return the number of codecs returned.
      */
    size_t GetSupportedPayload(Rtp_Payload_Code payloads[], size_t maxnum);

    /** Get the local address and port which this channel can be contacted.
      This function can be called even when the Rtp_Media instance has not
      been initialized.
      */
    void   GetLocalAddress(ACE_INET_Addr & addr);

    /**
     * Get the physical interface (e.g. "aud0") where this media belong.
     * This may return NULL when media doesn't have physical interface.
     */
    Interface *GetPhysicalInterface();

    /** Initialize media channel.
      If the remote_rtp parameter is not zero and payload is valid, then
      this will start RTP transmitter/receiver using the specified codec to the
      specified destination.
      */
    bool   Open();

    /** Start the RTP transmitter/receiver using the specified codec to the
      specified destination.
      */
    bool   Activate(const ACE_INET_Addr & remote_rtp, 
        Rtp_Payload_Code payload);

    /** Close the channel.
    */
    bool   Close();

    /** Dial DTMF digits.
    */
    void   DialDtmf(const char *digits);

    /** Determine the channel status.
    */
    enum Rtp_Status {
      Rtp_Not_Opened,	// channel has not been Open()-ed
      Rtp_Opened,	// channel has been Open()-ed
      Rtp_Error	// channel has error.
    };
    Rtp_Status GetMediaStatus() const;

    /** Get channel.
    */
    RTP_Channel *GetChannel() {
      return acChannel;
    }
    void ReleaseChannel(CallDirection dir,CallParameter *cparam,CallCause cc) {
      acChannel->SetEventCallback(NULL);
      acChannel->ReleaseChannel(dir, cparam, cc);
      acChannel = NULL;
      eventCallback.channel = NULL;
    }


    /** Get interface source specification token.
    */
    void  *GetItfSource() {
      return itfSource;
    }

    /** Notification from AudioCodes on media error.
      This will disconnect the call.
      */
    void OnMediaError();


    /** Get statistics data from rtp channel
    */
    virtual void UpdateQualityData(CallQualityParameter param);

    /** Export statistics data
    */
    virtual float GetQuality(CallQualityParameter param);

    /** Called by signaling to configure DTMF transfer to use RFC 2833.
     *  If this function is not called, DTMF transfer will use whatever the
     *  settings in RTP interface.
     */
    void DTMFSetRfc2833(int rx_pt, int tx_pt);

    // function to pause half/full transmission
    void enableTransmission(bool isRtpRxEnable, bool isRtpTxEnable);



  private:
    RTP_Channel *acChannel;
    Interface *phyInterface;

    void      *itfSource;
    Rtp_Payload_Code payloadCurrent;

    struct EventCallback : public RTP_Channel_Callback
  {
    Rtp_Media  &rtpMedia;
    RTP_Channel *channel;
    bool       destUnreach;

    EventCallback (Rtp_Media & rtp, RTP_Channel *ch) : 
      rtpMedia(rtp), 
      channel(ch), 
      destUnreach(false) {

        if (channel->GetBoardType()==RTP_Board_AC)
          channel->SetEventCallback(this);
      }
    ~EventCallback() {
      if (channel && channel->GetBoardType()==RTP_Board_AC)
        channel->SetEventCallback(NULL);
    }
    virtual void OnError(RTP_Channel * channel) 
    {	    
      destUnreach = true;
      rtpMedia.OnMediaError();	    
    }

    virtual void OnTerminated(RTP_Channel * channel) 
    {	    

    }

  } eventCallback;
};


///////////////////////////////////////////////////////////////////////////////
/** This class describes RTP interface, which can be created dynamically by
  user. An RTP interface is only a logical entity, to group together channels
  from physical RTP devices (eg. AudioCodes boards).
  */
class Rtp_Interface : public Interface {
  public:        

    Rtp_Interface(const char *name);
    ~Rtp_Interface();

    /** Allocate one media channel from the interface.
      It returns false when there is no free media available
      */
    virtual bool AllocateMedia(Call *call,Media *&media,SelectionStrategy sel,CallDirection dir,CallParameter *cparam);

    /** AllocateMedia: A variant of AllocateMedia that specify a particular media
      to be allocated. It returns false when there is such media is not available
      */
    virtual bool AllocateMedia(Call *call,Media *&media,MediaId id,CallDirection dir,CallParameter *cparam);


#ifdef  XGATE_HAS_H248
#ifdef  H248_MG
    virtual bool AllocateMediaSpecific(Call *call,Media *&media,MediaId id,CallDirection dir,
        CallParameter *cparam, char *mediaName);
#else
    //virtual bool AllocateMediaMGC(Call *call,Media *&media,CallDirection dir,CallParameter* cparam ,ACE_INET_Addr &addr, Rtp_Payload_Code& codec,char *mediaName);
    virtual bool AllocateMediaMGC(Call *call,Media *&media,CallDirection dir,CallParameter* cparam ,char *mediaName);
    void SetInterfaceConfig(RLS_RECORD *rec);
    bool IsMatchSigType(SignallingType sigType);
#endif
#endif


    /** ReleaseMedia: Called by Call object when the call becomes idle. The underlying
      channel is then ready to be used for another call.
      */
    virtual void ReleaseMedia(Media *media,CallDirection dir,CallParameter *cparam,CallCause cc);

    /** Determine whether this interface can be deleted.
    */
    virtual bool SupportDynamicDeletion();

    /** Called to destroy this instance.
    */
    virtual void DestroyInterface();

    /** Load: Called once at initialization time
    */
    virtual bool Load();

    /** Unload: Called once at termination time
    */
    virtual bool Unload();

    /** Open: May be called several times to start the object
      The object may be then in In-Service (enabled/disabled) state
      */
    virtual bool Start();

    /** Close: Once started, an object may be requested to close
      The object goes to the Out-Of-Service state
      */
    virtual bool Stop();

    /** Commit Configuration.
    */
    virtual bool CommitConfiguration();

    /** HandleEvents: Descendent shall implement this abstract method.
      It is called by Service Provider Manager
      */
    virtual void HandleEvents();

    /** Show configuration for the specified entry.
    */
    virtual bool OnShowConfig(const Cfg_Tree_Entry *entry, String & output, const String & indent, int indent_size) const;

    /** Display interface specific information to the terminal user.
      Decendants MUST implement this function.
      */
    virtual void OnShowInterface(Term_Session * session );

    /** Terminal command callback
    */
    virtual void DoTermCmd(Term_Session *, bool no, Term_Cmd *);

    /** Get channel parameter.
    */
    AcChannelParam & GetChannelParameter() {
      return channelParam;
    }

    /** Get channel parameter.
    */
    const AcChannelParam & GetChannelParameter() const {
      return channelParam;
    }

    /** Does this interface use G.723.1 low?
    */
    bool IsUsingG7231Low() const {
      return useG7231Low;
    }

    /** Get supported payload list.
    */
    size_t GetSupportedPayload(Rtp_Payload_Code payloads[], size_t maxnum);

    /** Determine whether this interface can accept the incoming call based on
      the call's source address.
      */
    bool CanAcceptCall(SignallingType type, const ACL_Token & acl_token) const;

    /** Get the default RTP packing factor parameter for this interface.
    */
    unsigned GetPackingFactor() const {
      return channelParam.GetParam().VoiceSettings.M;
    }

    /** Get the maximum allowable packet forwarding for signalling
    */
    int GetMaxForwards() const{
      return maxForwards;
    }
    //Sri
    /*int GetMgcAddress() const{
      return mgc_address;
      }
      int GetMgcPort() const{
      return mgc_listen_port;
      }
      int GetMgAddress() const{
      return mg_address;
      }
      int GetMgPort() const{
      return mg_listen_port;
      }*/



    //Sri
    /** See if the private info allowed to be exported
    */
    bool IsPrivateInfoAllowed() const{
      return !stripPrivateInfo;
    }

    /** Returns whether SIP authorization has been configured for this interface.
    */
    bool RequireSipAuthorization() const {
      return !sipDigestAuthList.empty();
    }

    /** Authorize the call against configured user list.
    */
    bool Authorize (const String & user_name) const;

    /** Get Interface Quality
    */
    virtual float GetQuality(CallQualityParameter param, time_t start, time_t end, bool active);

    /**
     * Get RTP_Config from RTP interface settings.
     */
    void GetRtpChannelConfig(RTP_Config &config);

    /** Called to see if RTP interface is configured to use RFC 2833.
     *  Returns the payload type number, or -1 if RFC 2833 is not used.
     */
    int DTMFGetRfc2833();

    /**
     * Get RTP_Config from RTP interface settings.
     */
    String GetDefaultRealm() {return sipDigestAuthRealm;};

    #ifdef XGATE_HAS_MGC_MG_COMMUNICATION
    void StoreResourceRecord(RLS_RECORD &rls) {
      rlsEntry = rls;
      cout << "StoreResourceRecord called " << endl;
    }

    void DisplayResourceRecord()
    {
      cout << rlsEntry;
    }
    RLS_RECORD &GetResourceRecord() {  return rlsEntry; }
    #endif

  private:
    AcChannelParam channelParam;
    Rtp_Quality_Monitor _QMonitor;

    void PrintData(Term_Session * session, time_t start, time_t end, bool now);
    void ShowCallStatistics(Term_Session* session, int interval);

    struct Interface_Source {
      bool	  active;
      IString	  boardInterfaceName;
      size_t	  usedCh, maxCh;

      Interface_Source(const char *board_name, size_t m)
        : active(true), boardInterfaceName(board_name), usedCh(0), maxCh(m) {
        }
      size_t GetFreeCh() const {
        return maxCh - usedCh;
      }
    };

#ifdef XGATE_HAS_MTNRTP
    class Board_Source : public RTP_Channel_Callback {
      public:
        Board_Source(String name) 
          : name_(name), total_(0), last_first_releasing_time_(0), last_first_releasing_ch_(NULL) {}
        IString name_;

        typedef List<MTN_Rtp_Channel*> channel_l;
        unsigned  total_;
        channel_l active_;
        channel_l releasing_;

        unsigned    last_first_releasing_time_;
        MTN_Rtp_Channel *last_first_releasing_ch_;

        /* Call this periodically to check channels in releasing_ list.
           If the channel is IDLE, then release the channel to the 
           Factory.
           */
        void ReleaseIdleChannels();

        /* Return back used channel to this board source. */
        void ReturnChannel(MTN_Rtp_Channel* channel);
        void ReReturnChannel(MTN_Rtp_Channel* channel);

        void OnError(RTP_Channel * channel){};
        void OnTerminated(RTP_Channel * channel)  
        { ReturnChannel((MTN_Rtp_Channel*)channel); }
    };
    List <Board_Source*> brdSourceList;
#endif

    int maxForwards;
    //SRI
    /*
       int mgc_address;
       int mgc_listen_port;
       int mg_address;
       int mg_listen_port;
       */
    //SRI
    bool stripPrivateInfo;
    List <Interface_Source*> itfSourceList;
    List <Interface_Source*>::iterator lastItfSourceUsed;
    Rtp_Payload_Code payloadList[32];
    size_t	     payloadCount;
    typedef std::pair<unsigned,unsigned> Accept_Entry;
    List <Accept_Entry> acceptList;
    IString	     aclName;
    bool	     useG7231Low;
    typedef std::pair<String,String> StringPair;
    String	     sipDigestAuthRealm;
    List<StringPair> sipDigestAuthList;

    //#ifdef XGATE_HAS_H248
#ifdef H248_MG
    RLS_RECORD rlsEntry;
#endif
    //#endif

    void InitTermCmd();


    /**************************************************************************
     *
     * Overriden from Interface to manipulate interface statistics.
     * Rtp_Interface will also update the statistic of the corresponding
     * AudioCodes_Interface.
     *
     *************************************************************************/
    virtual void IncMediaCount(Media *med, CallDirection dir,CallParameter *cparam,CallCause cc,bool dontUseRLS=false);
    virtual void DecMediaCount(Media *med, CallDirection dir,CallParameter *cparam,bool dontUseRLS=false);
    virtual void IncCurrentBusyCount(Media *med, CallDirection dir);
    virtual void DecCurrentBusyCount(Media *med, CallDirection dir);
    virtual void IncCurrentConnectedCount(Media *med, CallDirection dir);
    virtual void DecCurrentConnectedCount(Media *med, CallDirection dir);
    virtual void IncTotalCallAttempts(Media *med, CallDirection dir);
    virtual void IncTotalCallConnected(Media *med, CallDirection dir);
    virtual void IncTotalCallSuccess(Media *med, CallDirection dir);
    virtual void IncTotalCallFailed(Media *med, CallDirection dir);
    virtual void IncTotalCallHoldingTime(Media *med, CallDirection dir, unsigned seconds);
    virtual void IncTotalCallConnectedTime(Media *med, CallDirection dir, unsigned seconds);
};




///////////////////////////////////////////////////////////////////////////////

/** This class describes rtp interface manager.
*/
class RtpInterfaceManager : public InterfaceManager, public ServiceProvider
{
  public:
    /** Create a singleton instance of this class.
    */
    static RtpInterfaceManager * Create();

    /** Get the singleton instance of this class.
    */
    static RtpInterfaceManager * Instance() {
      return rtpItfManagerInstance;
    }

    /** Initialize this instance.
    */
    virtual bool Load() {
      return true;
    }

    /** Denintialize this instance.
    */
    virtual bool Unload() {
      return true;
    }

    /** Handle events.
    */
    virtual void HandleEvents();

    /** Determine whether this interface manager can create the specified
      interface name. For this class, it will return true for interface 
      'rtpXX'.
      */
    virtual bool CanCreateInterface(const char *name);

    /** Create a new rtp interface.
    */
    virtual Interface *CreateInterface(const char *name);

    /** Dispatch a new call to the interfaces, according to their 'accept'
      specification.
      @param src_addr Source address, in network byte order.
      */
    Rtp_Interface *DispatchIncomingCall (SignallingType sig_type,
        const ACE_INET_Addr & src_addr,
        const ACE_INET_Addr & dst_addr,
        const CallParameter & cparam);

    /** Register a new RTP interface.
    */
    void RegisterRtpInterface (Rtp_Interface *itf) {
      rtpInterfaceList.push_back(itf);
    }

    /** Unregister RTP interface.
    */
    void UnregisterRtpInterface (Rtp_Interface *itf) {
      List <Rtp_Interface*>::iterator it = std::find(rtpInterfaceList.begin(), rtpInterfaceList.end(), itf);
      assert (it != rtpInterfaceList.end());
      if (it != rtpInterfaceList.end()) {
        if (it == lastRtpInterfaceUsed)
          lastRtpInterfaceUsed = rtpInterfaceList.begin();
        rtpInterfaceList.erase(it);
      }
    }

    /**
     * This function is provided for RTP interface to find physcal VoIP
     * board with the specified name.
     */
    Interface *FindInterface(const IString &itf_name);

    /**
     * This function is provided for RTP interface to allocate RTP channel
     * from the specified physical VoIP board with the specified name.
     */
    RTP_Channel *AllocateChannel(const IString &itf_name, 
        CallDirection dir, 
        CallParameter *cparam);

    //#ifdef H248_MG
#ifdef XGATE_HAS_H248
    void GetResourceRecordList(GwRscVector &);
#endif
    //void GetResourceRecordList(char *buff,int &count);
    void GetResourceRecordList(string &outBuff,int &count);
    //#endif

  private:
    static RtpInterfaceManager *rtpItfManagerInstance;

    List <Rtp_Interface*> rtpInterfaceList;
    List <Rtp_Interface*>::iterator lastRtpInterfaceUsed;

    /** Constructor
    */
    RtpInterfaceManager();

    /** Destructor.
    */
    ~RtpInterfaceManager();
};


///////////////////////////////////////////////////////////////////////////////

class Rtcp_Monitor : public ServiceProvider {
  public:
    static Rtcp_Monitor * Create();
    static Rtcp_Monitor * Instance() {
      return theInstance;
    }

    void OnChannelClosing(AcChannel *ch);
    void Show(Term_Session *session);

    virtual bool Load();
    virtual bool Unload();

    /** Clear RTCP monitor counter.
    */
    virtual void OnClearCounter();
    virtual void DoTermCmd(Term_Session *, bool no, Term_Cmd *);
    virtual bool OnShowConfig(
        const Cfg_Tree_Entry *entry,
        String & output, 
        const String & indent, 
        int indent_size) const;

  private:
    static Rtcp_Monitor *theInstance;

    struct SpecData {
      unsigned long addr, mask;
      unsigned      ncalls;
      AcRtcpStat    stat;

      SpecData() : ncalls(0) {}

      bool operator == (const SpecData & rhs) const {
        return addr==rhs.addr && mask==rhs.mask;
      }
    };
    List<SpecData> specList;
    time_t lastClearCounter;

    Rtcp_Monitor();
    ~Rtcp_Monitor();

    void RecordChStat(List<SpecData> & persistent, AcChannel *ch);
    void PrintList(Term_Session *session, const char *title, const List<SpecData> & data);
    void PrintData(Term_Session *session, const SpecData & data);
};

///////////////////////////////////////////////////////////////////////////////

#endif	// __XGATE_RTPMEDIA_H__

