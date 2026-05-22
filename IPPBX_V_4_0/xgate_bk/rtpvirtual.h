/* $Header: /xGate/rtpvirtual.h 1     3/08/04 7:11p Bennylp $
 *   
 * XGATE
 * (C) 2001 Switchlab, Ltd
 *
 * RTP virtual channel.
 *
 * $Log: /xGate/rtpvirtual.h $
 * 
 * 1     3/08/04 7:11p Bennylp
 * Initial version
 */
#ifndef __VIRTUAL_RTP_H__
#define __VIRTUAL_RTP_H__

//#include "ace/INET_Addr.h"
#include "rtpchan.h"

///////////////////////////////////////////////////////////////////////////////
class RTP_Virtual_Channel : public RTP_Channel
{
public:
    RTP_Virtual_Channel();
    ~RTP_Virtual_Channel();

    /** Determine the type of the current rtp board type
     */
    virtual RTP_Board_Type GetBoardType() const;

    /** Requiered for AudioCode interface event. 
        This will be discarded if not necessary.
     */
    virtual void SetEventCallback(RTP_Channel_Callback *callback);
		 

    /** Set payload code, supported to provided backward 
        compatibility with current RTP_Media	
     */
    virtual void SetCodec(Rtp_Payload_Code code);
    virtual Rtp_Payload_Code GetCodec();

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
    virtual bool Activate(const ACE_INET_Addr& addr, Rtp_Payload_Code codec, bool low);

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

    /** Get channel number */
    int GetChannelNumber(void) { return 0; }

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
    virtual const void GetLocalAddress(ACE_INET_Addr& addr);

    /** Compatibility with AcChannel.
     */
    virtual Rtp_Payload_Code GetSelectedPayload();

    /** Extracted method from RTP_Media to prepare switching device.
     */
    virtual bool PrepareSwitching(RTP_Config* conf);

    /** Provided to support backward compatibility with AcChannel
     */
    virtual int GetMediaID();

    /** Provided to support backward compatibility with AcChannel
     */
    virtual const AcRtcpStat& GetRtcpStat();

    /** Get/Set interface to change the config parameteres
        the behaviour of this method is vary. AudioCodes implmentation
	might change some value instantly while Mapletree might
	only commit the changes when Activated.
     */
    virtual const RTP_Config& GetRtpConfig();
    virtual void SetRtpConfig(RTP_Config& config);

private:
    int			     status_;
    time_t		     activate_time_;
    RTP_Config		     config_;
    ACE_INET_Addr	     local_addr_;
};

#endif	/* __VIRTUAL_RTP_H__ */

