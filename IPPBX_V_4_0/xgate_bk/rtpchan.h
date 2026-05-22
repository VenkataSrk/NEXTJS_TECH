#ifndef _RTPCHANNEL_H_
#define _RTPCHANNEL_H_

#include "xgconfig.h"
#include "quality.h"

#if 0
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



#ifdef XGATE_HAS_AUDIOCODES
#ifdef XGATE_HAS_NEW_AUDIOCODES
#   include "../AudioCodes.v46/include/audiocodes.h"
#else
#   include "../audiocodes/include/audiocodes.h"
#endif //XGATE_HAS_NEW_AUDIOCODES
#endif //XGATE_HAS_AUDIOCODES

#include "Switching.h"


class RTP_Channel;
//class ACE_INET_Addr; //linux porting

#define TRACE_RFC2833_	CLog::Debug


class RTP_Channel_Callback
{
public:
    virtual void OnError(RTP_Channel * channel)=0;
    virtual void OnTerminated(RTP_Channel * channel)=0;
};

struct RTP_Config
{
    unsigned char Active;
    unsigned char NIorPCI;
    unsigned int OutputGain;
    unsigned int InputGain;

    enum DTMF_Transport_Type
    {
	DTMF_Inband,	    // transfer DTMF inband
	DTMF_Proprietary,   // transfer DTMF using proprietary (AudioCodes only)
	DTMF_RFC2833,	    // transfer DTMF using RFC 2833
    };

    struct {
	unsigned int Volume;
	unsigned int Length;
	unsigned int Interfal;
	DTMF_Transport_Type Transport_Type;

	int	rfc2833_tx_pt;	// outgoing payload type when RFC2833 is selected.
	int	rfc2833_rx_pt;	// incoming payload type when RFC2833 is selected.

    } DTMF;
    
    struct {
	Rtp_Payload_Code Codec;
	unsigned int MinJitter;
	unsigned int PackingFactor;
	unsigned int OptimiztionFactor;
    } RTP;

    bool EchoCanceller;
    unsigned ECTailLen;
    bool HighPassFilter;
    bool PosFilter;
    bool SilenceCompression;
};

enum RTP_Board_Type
{
    RTP_Board_AC,
    RTP_Board_MTN,
    RTP_Board_Kallastra,
    RTP_Board_Virtual,
    RTP_Board_Prosody,
    RTP_Board_Host,
};

///////////////////////////////////////////////////////////////////////////////
/** RTP channel. Abstract class to define interface for RTP Channels. This class
    extracted from AcChannel class, to provide generalization of rtp channel.
 */
class RTP_Channel
{
public:
    RTP_Channel()
    {
    }

    virtual ~RTP_Channel()
    {
    }

    /** Determine the type of the current rtp board type
     */
    virtual RTP_Board_Type GetBoardType() const = 0;

    /** Requiered for AudioCode interface event. 
        This will be discarded if not necessary.
     */
    virtual void SetEventCallback(RTP_Channel_Callback *callback)=0;
		 

    /** Set payload code, supported to provided backward 
        compatibility with current RTP_Media	
     */
    virtual void SetCodec(Rtp_Payload_Code code) = 0;    
    virtual Rtp_Payload_Code GetCodec() = 0;

    /** Open the rtp channel. Some code on the RTP_Media::Open() that 
        depends on AudioCode frame work extracted to RTP_Channel decendant.
	Notes:
	From this state the channel ready to be activated
	For Audiocodes, it sets certain param and switch device props
	For Mapletree, it sets certain param and initialize the port
     */
    virtual bool Open(RTP_Config& config) = 0;    

    /** Activate the rtp channel. Some code on the RTP_Media that 
        depends on AudioCode frame work extracted to RTP_Channel. decendant.
     */
    virtual bool Activate(const ACE_INET_Addr& addr, Rtp_Payload_Code codec, bool low, int ptime)= 0; // removed as pure virtual : changed for multithreaded-ippbx

    /** Create the switch device for the channel. 
        This is extracted from RTP_Media to remove the dependency to AudioCode.
     */
    virtual SwitchDevice *GetSwitchDevice(const char* name) = 0;

    /** Close the channel. This is extracted from RTP_Media. 
        The code will be placed in the RTP_Channel dependant.
     */
    virtual void Close() = 0;

    /** Compatibility with AcChannel.	
     */
    virtual int GetInitializationStatus() const = 0;

    /** Compatibility with AcChannel.
     */
    virtual time_t GetLastActivateTime() = 0;

    /** Compatibility with AcChannel.
     */
    virtual void DialDtmf(const char* dtmf) = 0;

    /** Release Rtp Channel. Code extracted to RTP_Media decendants 
        to remove dependency.
     */
    virtual void ReleaseChannel(CallDirection cd, CallParameter* cp, CallCause cc) = 0;

    /** Compatibility with AcChannel.
     */
    virtual const sockaddr_in& GetLocalAddress() = 0;

    /** Compatibility with AcChannel.
     */
    virtual const void GetLocalAddress(ACE_INET_Addr& addr) = 0;

    /** Compatibility with AcChannel.
     */
    virtual Rtp_Payload_Code GetSelectedPayload() = 0;

    /** Extracted method from RTP_Media to prepare switching device.
     */
    virtual bool PrepareSwitching(RTP_Config* conf) = 0;

    /** Get the channel number (only need to be unique in the same board)
     */
    virtual int GetChannelNumber()=0;

    /** Provided to support backward compatibility with AcChannel
     */
    virtual int GetMediaID()=0;

    /** Provided to support backward compatibility with AcChannel
     */
    virtual const AcRtcpStat& GetRtcpStat()=0;

    /** Get/Set interface to change the config parameteres
        the behaviour of this method is vary. AudioCodes implmentation
	might change some value instantly while Mapletree might
	only commit the changes when Activated.
     */
    virtual const RTP_Config& GetRtpConfig()=0;
    virtual void SetRtpConfig(RTP_Config& config)=0;
    virtual void enableTransmission(bool isRtpRxEnable, bool isRtpTxEnable){};

#ifdef XGATE_HAS_HMP

    virtual bool PlayFileWav(char const *filename, bool autorepeat){}
    virtual bool PlayFileWav(char const *filename, bool autorepeat, void *call){}
    virtual bool AttachTask(void *call){}
    //virtual bool PlayFileWav(char const *filename, const char* targetHost, bool autorepeat){};
    virtual bool StopFileWav(int waitTime){}
	virtual void SetDtmfDigitTimeout(int max_time){}
//    virtual void getDtmfDigit(int number_of_digits, int wait_time, char* dtmf_digits){}
	virtual int  GetDtmfDigit(char* dtmf_digits){};
	virtual bool RecordFile(const char *filename, RTP_Channel *rtpchan2){}
    virtual void EndRecordFile(void){}
	virtual bool ConfCreateRoom(const char *room_name){}
    virtual bool ConfAddParty(const char *room_name){}
    virtual bool ConfRecordFile(const char *room_name, const char *filename){}
    virtual void ConfEndRecordFile(const char *room_name){};


#endif




protected:
    //RTP_Channel_Callback * rtpcallback_;
    RTP_Config		  config_;
    
};

#endif
