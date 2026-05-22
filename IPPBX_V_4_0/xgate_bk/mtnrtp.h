#ifndef _MTN_RTP_H_
#define _MTN_RTP_H_


#include "rtpchan.h"

#include "../MTN_Lib/src/mtn_lib.h"
#include "../MTN_Lib/src/mtn_mob.h"
#include "../MTN_Lib/src/mtn_channel.h"
#include "rtcp.h"

#include "mapletree.h"

#include "serviceprovider.h"
#include "termcmd.h"
#include "term.h"

#if 0
#include <ace/Synch.h>
#include "../lace/ace/OS.h"
#include "../lace/ace/INET_Addr.h"
#include "../lace/ace/SOCK_Acceptor.h"
#include "../lace/ace/SOCK_Connector.h"
#include "../lace/ace/SOCK_Stream.h"
#include "../lace/ace/SOCK_Dgram.h"
#include "../lace/ace/Reactor.h"
#include "../lace/ace/Token.h"
#include "../lace/ace/Synch.h"
#else
#include <ace/Synch.h>
#include <ace/OS.h>
#include <ace/INET_Addr.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Connector.h>
#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Dgram.h>
#include <ace/Reactor.h>
#include <ace/Token.h>
#include <ace/Synch.h>
#endif

//#define XGATE_MTN_HAS_JIITER_SIM

union ntp_time_t
{
    struct {
	/* This arrangement is suitable for little-endian */
	unsigned frac;
	unsigned sec;
    };
    //unsigned __int64 time64;
    //unsigned int64_t time64;
    int64_t time64;
};

inline ntp_time_t operator-(const ntp_time_t & t1, const ntp_time_t & t2)
{
    ntp_time_t result;
    result.sec = t1.sec - t2.sec;
    if (t1.frac >= t2.frac)
	result.frac = t1.frac - t2.frac;
    else {
	result.frac = 0xFFFFFFFF - (t2.frac - t1.frac);
	--result.sec;
    }
    return result;	
}

#define MAX_BUF (8*1024)

struct RTP_Pci_Udp_Callback;
class RTP_Pci_Stream_Thread;
class Rtp_Interface;

//#include "ace/thread.h"
//#include "thread.h"

///////////////////////////////////////////////////////////////////////////////
/**  Implementation of validator, to be used for 
 */
class MTN_TermBoardEnum : public CVariantDynamicChoiceValidator {
public:
    MTN_TermBoardEnum() {}
    MTN_TermBoardEnum( const MTN_TermBoardEnum & rhs )
	    : choice_(rhs.choice_) {}

private:
    mutable List<ValidatorChoice> choice_;

    virtual CVariantValidator *Clone() const;
    virtual void AcquireLock() const;
    virtual void ReleaseLock() const;
    virtual const List<ValidatorChoice> & GetChoiceList() const;
};


///////////////////////////////////////////////////////////////////////////////
class RTP_Udp_Sock : public ACE_Event_Handler 
{
public:
    RTP_Udp_Sock (RTP_Pci_Udp_Callback* callback);
    virtual ~RTP_Udp_Sock ();

    virtual ACE_HANDLE get_handle (void) const;
    virtual int handle_input (ACE_HANDLE fd);
    virtual int handle_close (ACE_HANDLE handle,
			  ACE_Reactor_Mask close_mask);

    bool Open(unsigned short port);
    void Close();

    ACE_SOCK_Dgram& GetSocket();

    unsigned short GetPort() const;
private:
    ACE_SOCK_Dgram dgram_;
    RTP_Pci_Udp_Callback* callback_;
    char buffer_[MAX_BUF];
    bool opened_;
};

///////////////////////////////////////////////////////////////////////////////
struct RTP_Pci_Udp_Callback
{
    virtual void OnPacketArrived(char* buff, int len, RTP_Udp_Sock* socket,
			ACE_INET_Addr& addr)=0;
};

///////////////////////////////////////////////////////////////////////////////
class RTP_Pci_Stream_Callback
{
public:
    virtual int  ReadFromBus(char** packet, int* dtype) = 0;
    virtual void WriteToBus(const char* packet, int len, int dtype) = 0;
};

///////////////////////////////////////////////////////////////////////////////
class RTP_Pci_Stream : public RTP_Pci_Udp_Callback
{
    friend class RTP_Pci_Stream_Thread;

public:
    enum RTP_Packet_Type
    {
	PTYPE_RTP,
	PTYPE_RTCP
    };

    RTP_Pci_Stream( RTP_Pci_Stream_Callback* callback,
		    bool manualread);
		    
    void GetLocalAddress(ACE_INET_Addr& addr);
    void SetManualPCIRead(bool manual);
    void SendToEthernet(const char* data, int len, RTP_Packet_Type type);
    bool Open();
    void Close();
    bool Activate(const char* remoteip, unsigned short remoteport);

    virtual void OnPacketArrived(char* buff, int len, RTP_Udp_Sock* socket,
			ACE_INET_Addr& addr);

    unsigned short GetLocalRtpPort() const;
    unsigned short IncRtpPort();

    unsigned short GetRtpSocketPort() const;

private:
    bool AllocateLocalAddress();

    RTP_Udp_Sock    rtp_;
    RTP_Udp_Sock    rtcp_;
    unsigned short  localport_;

    ACE_INET_Addr	    remote_rtp_, remote_rtcp_;

    ACE_INET_Addr	    nat_public_rtp_addr_;
    unsigned		    nat_counter_;

    RTP_Pci_Stream_Callback *callback_;

    bool    manual_;    
};

///////////////////////////////////////////////////////////////////////////////
class MTN_Rtp_Channel : public RTP_Channel, public Mapletree_Channel, 
				RTP_Pci_Stream_Callback,
				MTN_Message_Callback
{ 
    class Rtcp_Callback : public MTN_Callback_Notification 
    {
    public:
	Rtcp_Callback(MTN_Rtp_Channel * channel);
	static void callback_notification(MTN_Callback_Notification * mcn);
	void OnNotification();
    private:
	MTN_Rtp_Channel * channel_;
    };

    class MobSet_Callback : public MTN_Callback_Notification 
    {
    public:
	MobSet_Callback(MTN_Rtp_Channel * channel);
	static void callback_notification(MTN_Callback_Notification * mcn);
	void OnNotification();
    private:
	MTN_Rtp_Channel * channel_;
    };

    friend class PortState_Callback;
    friend class Rtcp_Callback;
    friend class RTP_Pci_Stream_Thread;

public:
    enum state_e {
	MRC_Idle,
	MRC_Allocating,
	MRC_Allocated,
	MRC_SettingPort,
	MRC_Initiating,
	MRC_Voice,
	MRC_DTMF_Send,
	MRC_Terminating,
	MRC_GetIpAddr
    };

    MTN_Rtp_Channel(Rtp_Interface *rtp_itf, Mapletree_Module* mod, MTN_Channel* chan);
    virtual ~MTN_Rtp_Channel();

    //Call setting port message callback
    VoiceEncodingT  TranslatePayload(Rtp_Payload_Code code, 
    				     bool low, int* ms, 
				     int packingfactor);

    //channel message callback
    virtual int on_message ( MTN_Queue_Type queue_id, const MTN_Message * msg);

    void OnMobSetNotification(bool statusok);    

    const char* GetModuleName() { return GetModule()->GetName();}    

    virtual int  ReadFromBus(char** packet, int* dtype);
    virtual void WriteToBus(const char* packet, int len, int dtype);

    bool IsUsingHostEth();

    bool OnActivate();

    void SendSR(int srsize);

    /////////////////////////////////////////////////////////////////
    //RTP_Channel implementation 
    virtual void   SetCodec(Rtp_Payload_Code code);    
    virtual bool   Open(RTP_Config& config);    
    virtual bool   Activate(const ACE_INET_Addr& addr, Rtp_Payload_Code codec, bool low);
    virtual void   Close();
    virtual int    GetInitializationStatus() const;
    virtual time_t GetLastActivateTime();
    virtual void   DialDtmf(const char* dtmf);
    virtual void   ReleaseChannel(CallDirection cd, CallParameter* cp, CallCause cc);
    virtual bool   PrepareSwitching(RTP_Config* conf);
    virtual int    GetMediaID();
    virtual void   SetRtpConfig(RTP_Config& config);
    virtual int     GetChannelNumber(void);

    virtual RTP_Board_Type   GetBoardType() const { return RTP_Board_MTN;};   
    virtual Rtp_Payload_Code GetCodec();   
    virtual SwitchDevice *   GetSwitchDevice(const char* name);
    virtual const void	     GetLocalAddress(ACE_INET_Addr& addr);
    virtual Rtp_Payload_Code GetSelectedPayload();
    virtual const sockaddr_in& GetLocalAddress();
            
    virtual const AcRtcpStat& GetRtcpStat();
    virtual const RTP_Config& GetRtpConfig();  
    
    const char *  GetChannelName() const;
    bool IsActive() const;
    unsigned short EthLocalPort() const;
    unsigned short EthRemotePort() const;

    unsigned long GetPacketsTx() const;
    unsigned long GetPacketsRx() const;

    unsigned long GetOctetsTx() const;
    unsigned long GetOctetsRx() const;

    const char * GetPortStateString() ;
    static const char * PortStateToString(int state);

    bool OnRecvRtcp(const rtcp_t *rtcp);
    bool OnSendRtcp(rtcp_t *rtcp);

    void OnRtcpCallback(bool status);

    void SetEventCallback(RTP_Channel_Callback *callback);

protected:    
    MTN_Event_Notification event_;
    SwitchDevice *switchdevice_;

private:
    void AllocatePort();
    void InitiatePort();
    bool CommitPortSetting();

    Rtp_Interface *rtp_itf_;

    MTN_Voice_Rtp_Call_Super_Bundle_Mob_Data portset_;
    RTP_Pci_Stream rtpstream_;

    bool    g723low_;
    state_e state_;

    unsigned short  rtp_seq_no_;
    unsigned long   last_send_rtcp_;
    unsigned	    our_ssrc_;
    int		    probation_;

    ntp_time_t	    lsr_sent_;		// time when LSR is sent.
    ntp_time_t	    lsr_recv_;		// NTP timestamp of last SR recv
    ntp_time_t	    lsr_recv_time_;	// time when last SR recv

    ACE_INET_Addr remoteaddr_;  
    ACE_INET_Addr boardaddr_;	
    MTN_Ip_Setting_Mob_Data ip_mob_;

    MTN_Rtcp_Status_Mob_Data rtcpstat_mob_;
    //MTN_Port_Status_Mob_Data portstat_mob_;
    

    Rtcp_Callback	rtcp_callback_;
    MobSet_Callback     mobset_callback_;


    /** Statistic aparatus
     */
    void ResetStatisticAparatus();

    AcRtcpStat rtcpstat_;
    time_t lastactive_;

    unsigned packets_rx_;
    unsigned packets_tx_;
    unsigned packets_rx_cum_lost_;
    unsigned packets_rx_frac_lost_;
    unsigned packets_rx_frac_;
    unsigned last_seq_;

    unsigned long octets_rx_;
    unsigned long octets_tx_;

    RTP_Channel_Callback * rtpcallback_;


    bool OnRecvRtp(rtp_hdr_t *rtp, unsigned size);

#ifdef XGATE_MTN_HAS_JIITER_SIM
    struct Jitter_Sim_Packet
    {
	void	 *pkt;
	unsigned  size;
	DWORD	  arrival_time;
	DWORD     tx_rx_time;
    };

    List<Jitter_Sim_Packet> net_tx_queue_;
    List<Jitter_Sim_Packet> net_rx_queue_;
#endif
};

/////////////////////////////////////////////////////////////////////////////
/** Factory class for MTN_Rtp_Channel
 */

class MTN_Rtp_Channel_Factory
{
public:
    MTN_Rtp_Channel_Factory();
    virtual ~MTN_Rtp_Channel_Factory();
    
    static MTN_Rtp_Channel_Factory *Instance();
    static void Destroy();

    unsigned EnumBoard(const char** boards, int maxboards);
    MTN_Rtp_Channel *Allocate( Rtp_Interface *itf, const char *board_name );
    void Release(const char *board_name, MTN_Rtp_Channel *Ch);

protected:
    static MTN_Rtp_Channel_Factory *Create();    
    static MTN_Rtp_Channel_Factory *instance_;
    
private:

};

///////////////////////////////////////////////////////////////////////////////
enum MR_Manager{
    MR_Manager,
    MR_Pool
};

class MTN_Rtp_Manager : public ServiceProvider
{
public:
    enum Term_Command {
	TC_Show
    };
    MTN_Rtp_Manager();
    virtual ~MTN_Rtp_Manager();

    static MTN_Rtp_Manager *Create();
    static MTN_Rtp_Manager *Instance();

    void    print_pools(String& output);
    const char* GetLocalIP();

    void AddActivatingChannel(MTN_Rtp_Channel* channel);

    void AddChannel(MTN_Rtp_Channel* channel);
    void RemoveChannel(MTN_Rtp_Channel* channel);
    void ChannelActivated(MTN_Rtp_Channel* channel);
    void ChannelDeactivated(MTN_Rtp_Channel* channel);

protected:
    Cfg_Tree_Group * GetConfigTree();
    virtual bool Load();
    virtual bool Unload();
    virtual bool CommitConfiguration();
    virtual void OnShutdown(bool cancel);
    virtual bool AttachTask(CTask *);
    virtual void DetachTask(CTask *);
    virtual bool CancelJob(CTask *);
    virtual bool OnShowStatus(String & output);
    virtual void DoTermCmd(Term_Session *, bool no, Term_Cmd *);

    virtual bool OnShowConfig(
	const Cfg_Tree_Entry *entry,
	String & output, 
	const String & indent, 
	int indent_size) const;    

    virtual void HandleEvents();    
    
    void init_config();    
    List<MTN_Rtp_Channel*> channels_;

    List<MTN_Rtp_Channel*> idlechannels_;
    List<MTN_Rtp_Channel*> activechannels_;

private:
    String localip_;
    static MTN_Rtp_Manager *instance_;
    
    Cfg_Tree_Group *config_;
};


#endif // _MTN_RTP_H_
