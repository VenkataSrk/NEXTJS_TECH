/* $Header: /xGate/sip.h 20    3/16/04 10:48a Bennylp $
 *   
 * XGATE
 * (C) 2001 Switchlab, Ltd
 *
 * SIP signalling.
 *
 * $Log: /xGate/sip.h $
 * 
 * 20    3/16/04 10:48a Bennylp
 * Added peer call for proxy support, and authentication
 * 
 * 19    3/08/04 7:05p Bennylp
 * Added:
 * - Registrar API (support spawning task for incoming registration)
 * - Authentication API
 * 
 * 18    2/17/03 3:26p Bennylp
 * Fixed require session-timer bug.
 * 
 * 17    1/22/03 10:54a Bennylp
 * Added command to configure SIP extension (timer)
 * 
 * 16    1/03/02 2:07p Bennylp
 * Fixed bug in API SetDisconnectMessage implementation
 * 
 * 15    12/11/01 5:05p Bennylp
 * Changed to support digest authentication in xGate
 * 
 * 14    11/15/01 11:56a Bennylp
 * Implement CallSetDisconnectMsg
 * 
 * 13    8/08/01 7:00p Bennylp
 * Initial (alpha) version, without configuration.
 * 
 */
#ifndef __XGATE_SIP_H__
#define __XGATE_SIP_H__

#include "call.h"
#include "SignallingDriver.h"
#include "sipmsg.h"

/* Forward decl.
 */
class Rtp_Interface;
class Rtp_Media;
class SipCallSip;
class XgSipAuthEntry;

void ParseUserPassword (const String & src, String & token1, String & token2);
bool ParseHostPort (const String & src, String & host, unsigned short & port);

///////////////////////////////////////////////////////////////////////////////
class XgSipCall : public Call {
public:
    XgSipCall(const CallParameter & cparam, CallDirection dir, const char *itf_name);
    ~XgSipCall();

    /** Attachment of this class to the internal SipCall.
     */
    void AttachSipCall(SipCallSip *sipcall);

    /** Requests from CallManager.
     */
    virtual bool DeviceOverlapReq(const char* additionalDigits,bool complete);
    virtual bool DeviceProceedingReq(CallProgress progress, CallExtraInfo* exinfo);
    virtual bool DeviceProgressReq(CallProgress progress, CallExtraInfo* exinfo);
    virtual bool DeviceAlertingReq(CallProgress progress, CallExtraInfo* exinfo);
    virtual bool DeviceConnectReq(CallExtraInfo* exinfo);
    virtual bool DeviceSendInfo(CallExtraInfo* exinfo);
    virtual bool DeviceSuspendReq();
    virtual bool DeviceResumeReq();
    virtual bool DeviceDisconnectReq(CallCause cause,CallProgress progress);
    virtual void SetDisconnectMsg(const char *msg);
    virtual int  GetAuthorizationHandle();

    /** Notifications from internal SipCall.
     */
    virtual bool OnIncomingCall();
    virtual bool OnCallProceeding(SipStatusCode status_code, const String& phrase, const SipResponseMessage &res_msg);
    virtual void OnReceivedInfo (const SipRequestMessage & info_request_msg);
    virtual void OnCallRedirected(const List<SipContactElement > &new_destinations,const SipResponseMessage &res_msg);
    virtual bool OnCallEstablished();
    virtual void OnCallDisconnected();

    /** Tools function for internal SipCall.
     */
    void GetLocalMediaAddr(ACE_INET_Addr & addr);

    /* Media manipulation.
     */
    const SipSdpBody *GetLocalMedia();
    const SipSdpBody *GetRemoteMedia();
    void	      SetLocalMedia(const SipSdpBody *body);

    bool IsValid() const;
    bool IsUsingVirtualRtp() const;
    SipCallSip *GetSipCall() {
	return sipCall;
    }
    XgSipCall *GetPeerSipCall() {
	if (peerSipCall && !peerSipCall->IsValid())
	    peerSipCall = NULL;
	return peerSipCall;
    }

    // Sorry for putting these on public.. :)
    int		rfc2833_rx_pt, rfc2833_tx_pt;
    bool	silenceCompression;

private:
    enum { SIGNATURE = 0x864BC52 };

    unsigned    signature1;
    SipCallSip *sipCall;
    bool	disconnectReq;
    CallCause	disconnectReqCause;
    bool	cmIsNotified;
    String	disconnectMessage;
    unsigned	signature2;

    XgSipCall	*peerSipCall;

    /** Internal tools function. 
     */
    Rtp_Media * GetRtpMedia() {
	return (Rtp_Media*) Call::GetMedia();
    }
};


///////////////////////////////////////////////////////////////////////////////
class XgSipSignalling : public SignallingDriver {
public:
    static XgSipSignalling * Create();
    static XgSipSignalling * Instance() {
	////return instance; // commented for ippbx
    }

    virtual bool Load();
    virtual bool Unload();
    virtual void HandleEvents();

    /** SetupReq: Initiate a new outgoing call in a specific Interface
	using the specified call parameter
     */
    virtual Call *SetupReq(Interface *itf,SelectionStrategy selection,CallParameter *cparam);

    /** Notifications from SIP that an incoming call has arrived.
     */
    bool OnIncomingInvitation(SipCallSip *sipcall, const SipRequestMessage & req_msg);

    /** CreateDefaultCallParameter: Returns a CallParameter structure containing
	default values for this signalling protocol
     */
    virtual CallParameter *CreateDefaultCallParameter();

    /** Initialize default call parameter.
     */
    void InitDefaultCallParameter( CallParameter & cparam );

    /** AttachInterface: A new Interface object is attached to this 
	SignallingDriver object
     */
    virtual bool AttachInterface(Interface *itf);

    /** DetachInterface: Remove association between a Interface object and this 
	SignallingDriver object
     */
    virtual void DetachInterface(Interface *itf);

    /** Show service status.
     */
    virtual bool OnShowStatus(String & output);

    virtual void DoTermCmd(Term_Session *session, bool no, Term_Cmd *cmd);
    virtual bool OnShowConfig(const Cfg_Tree_Entry *entry,
				    String & output, 
				    const String & indent, 
				    int indent_size) const;
private:
    static XgSipSignalling *instance;
    bool   requireTimer;
    //List <Rtp_Interface *> rtpInterface;


    XgSipSignalling();
    ~XgSipSignalling();
};


///////////////////////////////////////////////////////////////////////////////
/*
 *
 *    UTILITY FUNCTIONS
 *
 */

/** Parse host:port string to host and port.
 */
bool ParseHostPort (const String & src, String & host, unsigned short & port);

/** Create SIP URL from CallParameter structure.
 */
void CallParam2Url(CallParameter *param, SipNameAddress & src, SipNameAddress &dest, List <SipHeader *> &additional_headers);

/** Create call param from an INVITE request.
 */
void CreateCallParam ( CallParameter & cparam, const SipRequestMessage & invite);

/** Class to convert SIP <--> XGATE cause codes.
 */
class SIP_CTENGINE_Cause_Mapping {
public:
    /** Construct.
     */
    SIP_CTENGINE_Cause_Mapping();

    /** Get SIP status code and reason from XGATE disconnect cause.
     */
    SipStatusCode GetSipStatus (CallCause cause, String & reason) const;

    /** Get XGATE disconnect cause from SIP status/reason.
     */
    CallCause     GetCtengineCause (SipStatusCode code, const String & reason) const;

private:
    typedef std::pair <SipStatusCode, String> SIP_Code_T;
    typedef Map <CallCause, SIP_Code_T> CTENGINE_2_SIP_T;
    typedef Map <SipStatusCode,CallCause> SIP_2_CTENGINE_T;

    CTENGINE_2_SIP_T  ctengineMapping;
    SIP_2_CTENGINE_T  sipMapping;

    void InsertMapping (CallCause ct_cause, SipStatusCode sip_cause, const String & sip_reason)
    {
	SIP_Code_T  sipReason(sip_cause, sip_reason);
	ctengineMapping[ct_cause] = sipReason;
	sipMapping [sip_cause] = ct_cause;
    }
};


// external global var.
extern SIP_CTENGINE_Cause_Mapping sip_ctengine_cause_mapping;


bool ParseTelephoneNumber(const char* inp, CallNumber& o_number, CallNumberType& o_type);

#endif	/* __XGATE_SIP_H__ */
