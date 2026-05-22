/* $Header: /xGate/sip.cpp 56    8/04/04 7:03p Bennylp $
 *   
 * XGATE
 * (C) 2001 Switchlab, Ltd
 *
 * SIP signalling.
 *
 * $Log: /xGate/sip.cpp $
 * 
 * 56    8/04/04 7:03p Bennylp
 * Fix layout for "q" parameter in Contact (digits shouldn't exceed 3)
 * 
 * 55    8/04/04 11:33a Bennylp
 * Fixed registration GetSourceAddress()
 * 
 * 54    8/03/04 7:30p Bennylp
 * Support sending message body in REGISTER response
 * 
 * 53    7/28/04 4:24p Bennylp
 * Fixed the source address of SIP registration
 * 
 * 52    7/26/04 12:11p Bennylp
 * Added API in registration to get source IP address of message
 * 
 * 51    4/30/04 8:39p Bennylp
 * Fixed bug: media not negotiated
 * 
 * 50    3/17/04 3:32p Bennylp
 * Commented out bogus warning messages
 * 
 * 49    3/16/04 10:50a Bennylp
 * Added proxy type of call (i.e. media is copied from leg A) support
 * 
 * 48    3/08/04 7:05p Bennylp
 * Added:
 * - Registrar API (support spawning task for incoming registration)
 * - Authentication API
 * 
 * 47    30/12/03 16:44 Junanto
 * 1st attempt adding g729 to SIP
 * 
 * 46    2/17/03 3:26p Bennylp
 * Fixed require session-timer bug.
 * 
 * 45    2/07/03 6:21p Bennylp
 * Fixed bug when task is notified of disconnected calls
 * 
 * 44    1/28/03 11:18a Bennylp
 * Fixed bug in SIP's OnShowConfig (missing "top" comand)
 * 
 * 43    1/22/03 10:54a Bennylp
 * Added command to configure SIP extension (timer)
 * 
 * 42    5/28/02 11:31a Bennylp
 * Call DetachTask() before DestroyCall().
 * 
 * 41    5/22/02 12:46p Bennylp
 * Set the src addr for ACL from socket addr rather than from cparam.
 * 
 * 40    5/16/02 5:43p Bennylp
 * Fix the logging text for unsupported Suspend request
 * 
 * 39    3/18/02 12:38p Bennylp
 * Version 38.16: fixed bug SIP session timeout was not implemented
 * 
 * 38    1/03/02 2:07p Bennylp
 * Fixed bug in API SetDisconnectMessage implementation
 * 
 * 37    12/11/01 5:05p Bennylp
 * Changed to support digest authentication in xGate
 * 
 * 36    12/03/01 8:01p Bennylp
 * Remove code that erase call session id
 * 
 * 35    11/15/01 11:56a Bennylp
 * Implement CallSetDisconnectMsg
 * 
 * 34    11/07/01 5:53p Adham
 * 
 * 33    8/28/01 7:57p Bennylp
 * Option to include/exclide H.323, SIP, and ATM in compilation.
 * 
 * 32    16/08/01 11:21 Junanto
 * Removed timeout from HandleEvents and loop only once
 * 
 * 31    8/13/01 11:39a Bennylp
 * Fixed bug in StartMedia doesn't set isChannelStarted flag.
 * 
 * 30    8/09/01 6:12p Bennylp
 * Changed DispatchIncomingCall interface.
 * 
 * 29    8/08/01 7:00p Bennylp
 * Initial (alpha) version, without configuration.
 * 
 */
#include "stdafx.h"
#include "xgconfig.h"

#ifdef XGATE_HAS_SIP

#ifndef SDP_TYPES_INLINE
  #define SDP_TYPES_INLINE
#endif

#ifdef __linux__
String NullString="";
#endif


#include "sipagent.h"
#include "sipcall.h"
#include "sipauth.h"
#include "sip.h"
#include "sipnetcoder.h"
#include "sipacm.h"
#include "sipneg.h"
#include "sipchannel.h"
#include "rtpmedia.h"
#include "klog.h"
#include "callmanager.h"
#include "kernel.h"
#include "registrar.h"

#define THISMODULE  "sip"

Log_Trace *traceSipMsg;

//#define SIP_AUTH_DEFAULT_DOMAIN	    "switchlab.net"
#define SIP_AUTH_DEFAULT_REALM	    "xGate access"

//#define MY_DEBUG(p,s)	CLog::Info(THISMODULE, "%p: %s", p, s)
#define MY_DEBUG(p,s)

///////////////////////////////////////////////////////////////////////////////

XgSipCall *GetXgSipCallFromCParam(const CallParameter *cp)
{
    if (cp->extraInfo.data.size() != sizeof(XgSipCall*))
	return NULL;
    if (cp->extraInfo.owner != THISMODULE)
	return NULL;
    return (XgSipCall*) (* cp->extraInfo.data.begin());
}

///////////////////////////////////////////////////////////////////////////////

class SipRtpChannelSip : public SipRtpBidirectionalChannel {
public:
    SipRtpChannelSip(XgSipCall *xgcall, Rtp_Media *rtpmed)
	: xgCall(xgcall), rtpMedia(rtpmed), channelIsStarted(false) {}

    virtual bool CreateChannel();
    virtual bool StartChannel(const String & remote_addr,
			      unsigned short port,
			      const SipCapability &capability);
    virtual bool StopChannel();
    virtual bool IsStarted() const;
    virtual bool GetLocalAddress(String & addr, unsigned short & port) const;

private:
    XgSipCall *xgCall;
    Rtp_Media *rtpMedia;
    bool       channelIsStarted;
};

///////////////////////////////////////////////////////////////////////////////

class SipTextBody : public SipBody
{
public:
    SipTextBody(const String & type, const String & text) : type_(type), text_(text) {}

    virtual SipMediaType GetType() const
    {
	SipMediaType t;
	t.typeSubtype = type_;
	return t;
    }

    virtual SipObject *Clone() const
    {
	return new SipTextBody(type_, text_);
    }

    virtual void PrintOn(OStream &strm) const
    {
	strm << text_;
    }

private:
    String  type_;
    String  text_;
};

///////////////////////////////////////////////////////////////////////////////

class SipNegSip : public SipMediaNegotiator {
public:
    SipNegSip();
    ~SipNegSip();

    /** Attach Xgate call.
     */
    void AttachCall(XgSipCall *call);

    /** Get master capability list.
     */
    virtual const List<SipCapability*> & GetCapabilityList();

    /** Instantiate a SipCapability class based on parameters given. 
     */
    virtual SipCapability * CreateCapability (
	SipCapability::Direction dir,
	const IString & med_type,
	const IString & transport_type,
	unsigned payload_type,
	const IString & encoding_name = NullString,
	int clock_rate = 8000,
	int num_port = 1
    ) const;

    virtual bool Initialize (
	const SipSdpBody & sdp_body
    );
    virtual SipSdpBody * CreateSdp () const;

    virtual bool Negotiate (const SipSdpBody & sdp_body);
    virtual bool Negotiate (const SipMediaNegotiator & neg);
    virtual void SelectOne ();

    void	       SetSdp(const SipSdpBody *body);
    const SipSdpBody * GetSdp() const;

private:
    XgSipCall		*xgCall;
    List<SipCapability*> capList;
    bool		 isMediaVirtualRtp;
    SipSdpBody		*mediaSdp;

    void UpdateCapabilityList();
};


///////////////////////////////////////////////////////////////////////////////

class SipCallSip : public SipCall 
{
public:
    SipCallSip(SipTransport &, const String &call_id);

    void AttachCall(XgSipCall *call) {
	xgCall = call;
    }

    virtual bool OnIncomingInvitation(const SipRequestMessage &req_msg);
    virtual bool RequireAuthorization();
    virtual bool OnIncomingCall(const SipRequestMessage &req_msg);
    virtual bool OnCallProceeding(SipStatusCode status_code, 
				  const String& phrase, 
				  const SipResponseMessage &res_msg);
    virtual void OnReceivedInfo (const SipRequestMessage & info_request_msg);
    virtual void OnCallRedirected(const List<SipContactElement > &new_destinations,
				  const SipResponseMessage &res_msg);
    virtual bool OnCallEstablished();
    virtual void OnCallDisconnected();
    virtual SipRtpBidirectionalChannel* OnCreateChannel();
    virtual SipMediaNegotiator * CreateMediaNegotiator(Negotiator_Type type) const;

    virtual bool OnTransactionSuccess (
	SipTransaction &,		    /// the transaction
	SipTransaction::Direction msg_dir,  /// direction of the message
	const SipMessage *		    /// the final response message, if any.
    );
    virtual void OnTransactionReceivedAck2xx(SipTransaction &,
					     const SipRequestMessage *);
    virtual bool OnTransactionToSendAck2xx( SipTransaction &trans,
					    unsigned cseq,
					    unsigned option);


    SipNegSip *GetLocalNegotiator();
    SipNegSip *GetRemoteNegotiator();

    virtual bool OnStartChannel();

    XgSipAuthEntry *GetAuthorization();
    bool	    HasReceivedAck() const;
    bool	    HasReceivedBodyInAck() const;

private:
    XgSipCall *xgCall;
    bool       hasSipAuth;
    XgSipAuthEntry *sipAuth;
    bool       hasReceivedAck;
    bool       hasReceivedBodyInAck;
};


///////////////////////////////////////////////////////////////////////////////

class SipRegistrarCall : public SipBasicCall
{
public:
    SipRegistrarCall(SipTransport &t, const String &call_id);
    ~SipRegistrarCall();

    const char * GetStateStr() const;

    virtual bool OnTransactionStarted (
	SipTransaction &,		    /// the transaction.
	SipTransaction::Direction msg_dir,  /// direction of the message
	const SipMessage *		    /// the request message, if any.
    );
    virtual bool OnTransactionProgressing (
	SipTransaction &,		    /// the transaction.
	SipTransaction::Direction msg_dir,  /// direction of the message
	const SipMessage *		    /// the response message, if any.
    );
    virtual bool OnTransactionSuccess (
	SipTransaction &,		    /// the transaction
	SipTransaction::Direction msg_dir,  /// direction of the message
	const SipMessage *		    /// the final response message, if any.
    );
    virtual void OnTransactionFailed (
	SipTransaction &,		    /// the transaction.
	SipTransaction::Direction msg_dir,  /// direction of the message
	const SipMessage *		    /// the response that causes the failure, if any.
    );
    virtual void OnTransactionCleared(
	SipTransaction &	/// the transaction.
    );

    bool	   IsValid() const;
    time_t	   GetStartTime() const;
    const SipUrl & GetRequestUrl() const;
    int		   LastAnswer() const;

private:
    enum { SIGNATURE = 0xA1B2C3D4 };
    unsigned	    signature_;
    time_t	    start_time_;
    SipUrl	    request_url_;
    int		    last_answer_;
};

/* Implementation for SipRegistrarCall at the bottom part of this file. */

///////////////////////////////////////////////////////////////////////////////

/*
 * SIP Registrar service.
 */
class XgSipRegistrar : public Registrar_Agent, public ServiceProvider
{
public:
    XgSipRegistrar();
    ~XgSipRegistrar();

    static XgSipRegistrar *Instance();

    virtual bool Load();
    virtual bool CommitConfiguration();
    virtual bool Unload();
    virtual void HandleEvents();
    virtual bool OnShowStatus(String & output);

    void	  AddCall( SipRegistrarCall*);
    void	  RemoveCall( SipRegistrarCall*);

    void          OnIncomingRegistration(SipRegistrarCall *call, 
					 const ACE_INET_Addr & src_addr,
					 SipRequestMessage *req_msg);
    virtual void  CancelRegistration(const Reg_Info *reg_info);
    virtual bool  AnswerRegistration(const Reg_Info *reg_info,
				    int status);

private:
    static XgSipRegistrar *instance;
    enum { CHECK_INTERVAL = 1, CALL_TIMEOUT_INTERVAL = 30 };
    time_t  time_to_check_;
    std::set<SipRegistrarCall*> calls_;
    unsigned total_requests_, total_answered_, total_success_, total_timeout_;
};

XgSipRegistrar *XgSipRegistrar::instance;

/* Implementation for XgSipRegistrar at the bottom part of this file. */


///////////////////////////////////////////////////////////////////////////////

class SipAgentSip : public SipAgent {
public:
    virtual SipCall * OnCreateCall(
	SipTransport &transport,    /// transport to be used by the call
	const String &call_id,	    /// the call id
	CallDirection dir,	    /// direction of the new call
	void * user_data	    /// user data if this is outgoing call
    );

    /** Notification when a packet is received.
     */
    virtual bool OnReceivedPDU(
	SipTransport & src_tr,		/// transport from which the PDU received
	const ACE_INET_Addr & src_addr, /// the source address of the packet
	const char *pdu,		/// the PDU
	size_t pdu_size			/// the size of the PDU
    );

    /** Notification when the agent is about to send a message.
     */
    virtual void OnSendingMessage(
	SipTransport & transport,
	const char *msg,
	size_t length,
	const ACE_INET_Addr & dst_addr
    );
};


///////////////////////////////////////////////////////////////////////////////

bool SipRtpChannelSip::CreateChannel()
{
    return rtpMedia != NULL;
}

bool SipRtpChannelSip::StartChannel(const String & remote_addr,
				    unsigned short port,
				    const SipCapability &capability)
{
    if (channelIsStarted)
	return true;

    /* Enable RFC 2833, if required. */
    if (xgCall->rfc2833_rx_pt > 0 && xgCall->rfc2833_tx_pt > 0) {
	TRACE_RFC2833_("sip", "RFC2833: activating media, rx_pt=%d, tx_pt=%d",
			      xgCall->rfc2833_rx_pt, xgCall->rfc2833_tx_pt);
	rtpMedia->DTMFSetRfc2833(xgCall->rfc2833_rx_pt, xgCall->rfc2833_tx_pt);
    }

    ACE_INET_Addr addr(port, remote_addr.c_str());

    return channelIsStarted = rtpMedia->Open() &&
			      rtpMedia->Activate(addr, (Rtp_Payload_Code)capability.GetPayloadType(), 20);
}

bool SipRtpChannelSip::StopChannel()
{
    channelIsStarted=false;
    return rtpMedia->Close();
}

bool SipRtpChannelSip::IsStarted() const 
{ 
    return channelIsStarted; 
}

bool SipRtpChannelSip::GetLocalAddress(String & addr, unsigned short & port) const
{
    ACE_INET_Addr local_addr;
    rtpMedia->GetLocalAddress(local_addr);
    addr = local_addr.get_host_addr();
    port = local_addr.get_port_number();
    return true;
}

///////////////////////////////////////////////////////////////////////////////

SipNegSip::SipNegSip() 
: xgCall(NULL), isMediaVirtualRtp(NULL), mediaSdp(NULL)
{
}

SipNegSip::~SipNegSip()
{
    delete mediaSdp;
}

void SipNegSip::AttachCall(XgSipCall *call) 
{
    xgCall = call;
    if (xgCall) {
	Rtp_Media *med = (Rtp_Media *) call->GetMedia();
	assert(med);
	if (med) {
	    isMediaVirtualRtp = (med->GetChannel()->GetBoardType() == RTP_Board_Virtual);
	}
    }
}

bool SipNegSip::Negotiate(const SipSdpBody & sdp_body)
{
    ++negotiationCount;

    // Also check the presence of RFC 2833 in response.
    const SdpMediaDescriptor &m = sdp_body.mediaDescriptor[0];
    List<SdpRtpmapAttribute>::const_iterator it, end;

    end = m.attrRtpmapList.end();
    for (it=m.attrRtpmapList.begin(); it!=end; ++it) {
	const SdpRtpmapAttribute &rtpmap = *it;

	if (rtpmap.encodingName == "telephone-event") {
	    if (xgCall) {
		xgCall->rfc2833_tx_pt = rtpmap.payloadType;
		CLog::Info("sip", "GOT telephone-event, pt=%d", xgCall->rfc2833_tx_pt);
	    }
	    break;
	}
    }

    return isMediaVirtualRtp ? true : SipMediaNegotiator::Negotiate(sdp_body);
}

bool SipNegSip::Negotiate(const SipMediaNegotiator & neg)
{
    ++negotiationCount;
    return isMediaVirtualRtp ? true : SipMediaNegotiator::Negotiate(neg);
}

void SipNegSip::SelectOne()
{
    if (!isMediaVirtualRtp)
	SipMediaNegotiator::SelectOne();
}

// Verify capability in agent that match capability of the
// interface, and save the result to capList.
void SipNegSip::UpdateCapabilityList()
{
    if (capList.size())
	return;

    assert(xgCall);
    if (!xgCall)
	return;
    
    Media *med = xgCall->GetMedia();
    if (!med) {
	assert(false);
	return;
    }

    Rtp_Interface *rtp_itf = (Rtp_Interface *)med->GetInterface();
    assert(rtp_itf);

    Rtp_Payload_Code payload[RTP_MAX_CODEC_LIST];
    int pnum = rtp_itf->GetSupportedPayload(payload, sizeof(payload)/sizeof(payload[0]));

    if (pnum < 1)
	return;

    List<SipCapability*>::iterator it, end;
    end = SipAgent::Instance()->GetCapabilityList().end();
    for (it=SipAgent::Instance()->GetCapabilityList().begin(); it!=end; ++it) {
	SipCapability *cap = *it;
	
	if (std::find(payload, payload+pnum, cap->GetPayloadType()) != payload+pnum)
	    capList.push_back(cap);
    }
}

/** Get master capability list.
 */
const List<SipCapability*> & SipNegSip::GetCapabilityList()
{
    UpdateCapabilityList();
    return capList;
}

class MatchCap {
public:
    MatchCap(unsigned d) : pt(d) {}
    bool operator() (const SipCapability *cap) {
	return cap->GetPayloadType()==pt;
    }
private:
    unsigned pt;
};

/** Instantiate a SipCapability class based on parameters given. 
 */
SipCapability * SipNegSip::CreateCapability ( SipCapability::Direction dir,
					      const IString & med_type,
					      const IString & transport_type,
					      unsigned payload_type,
					      const IString & encoding_name,
					      int clock_rate,
					      int num_port) const
{
    ((SipNegSip*)this)->UpdateCapabilityList();

    if (xgCall==NULL || !xgCall->IsUsingVirtualRtp()) {
	List<SipCapability*> & clist = ((SipNegSip*)this)->capList;

	List<SipCapability*>::iterator it;
	it = std::find_if(clist.begin(), clist.end(), MatchCap(payload_type));
	if (it==clist.end())
	    return NULL;
	return (*it)->Clone();

    } else {
	List<SipCapability*> & clist = ((SipNegSip*)this)->capList;
	if (clist.empty())
	    return NULL;
	return clist.front()->Clone();
    }
}

void SipNegSip::SetSdp(const SipSdpBody *body)
{
    //
    // Note: NULL may be passed as parameter
    // 
    if (isMediaVirtualRtp) {
	if (body) {
	    delete mediaSdp;
	    mediaSdp = (SipSdpBody*) body->Clone();
	} else {
	    CLog::Warning(THISMODULE, "SetLocalMedia() is called with invalid NULL media. "
				      "The virtual RTP interface won't have valid media description.");
	}

    } else {
	CLog::Info(THISMODULE, "SetLocalMedia() has no effect for this SIP call "
			       "because RTP interface is not virtual");
    }
}

bool SipNegSip::Initialize (const SipSdpBody & sdp_body)
{
    if (isMediaVirtualRtp) {
	SetSdp(&sdp_body);
    }

    return SipMediaNegotiator::Initialize(sdp_body);
}

const SipSdpBody * SipNegSip::GetSdp() const
{
    return mediaSdp;
}

SipSdpBody * SipNegSip::CreateSdp() const
{
    if (isMediaVirtualRtp) {
	if (mediaSdp) {
	    return (SipSdpBody*) mediaSdp->Clone();
	} else {
	    CLog::Error(THISMODULE, "Application error: media is not "
				    "initialized for Virtual RTP");
	    return NULL;
	}
    } else {
	SipSdpBody *sdp = SipMediaNegotiator::CreateSdp();
	if (!sdp)
	    return sdp;

	/* Skip if we don't have xgCall (unlikely) */
	if (!xgCall)
	    return sdp;

	/* Check if RFC 2833 is supported and add telephone-events payload type
	 * in SDP if so
	 */
	if (xgCall->rfc2833_rx_pt > 0) {
	    SdpMediaDescriptor &m = sdp->mediaDescriptor[0];
	    
	    /* Add new payload type in the media line */
	    m.formatList.push_back(xgCall->rfc2833_rx_pt);

	    /* Add new rtpmap attribute */
	    SdpRtpmapAttribute rtpmap;

	    rtpmap.payloadType = xgCall->rfc2833_rx_pt;
	    rtpmap.encodingName = "telephone-event";
	    rtpmap.clockRate = 8000;

	    m.attrRtpmapList.push_back(rtpmap);

	}

	/** Check silence compression for G729 codec
	 *  this is to configure fmtp annexb display in SDP
	 */
	if (!xgCall->silenceCompression) {
	    SdpMediaDescriptor &m = sdp->mediaDescriptor[0];
	    List<SdpRtpmapAttribute>::iterator it;
	    for (it=m.attrRtpmapList.begin(); it!=m.attrRtpmapList.end(); ++it) {
		if (it->payloadType == (int)SipCapability::PT_G729) {
		    it->silenceCompression = false;
		    break;
		}
	    }
 	}

	return sdp;
    }
}

///////////////////////////////////////////////////////////////////////////////

SipCallSip::SipCallSip(SipTransport &transport, const String &call_id)
: SipCall(transport, call_id), xgCall(NULL), hasSipAuth(false), sipAuth(NULL),
  hasReceivedAck(false), hasReceivedBodyInAck(false)
{
}

SipNegSip *SipCallSip::GetLocalNegotiator()
{
    return (SipNegSip *)localCap;
}

SipNegSip *SipCallSip::GetRemoteNegotiator()
{
    return (SipNegSip *)remoteCap;
}

XgSipAuthEntry *SipCallSip::GetAuthorization()
{
    if (hasSipAuth)
	return sipAuth;
    else
	return NULL;
}

bool SipCallSip::HasReceivedAck() const
{
    return hasReceivedAck;
}

bool SipCallSip::HasReceivedBodyInAck() const
{
    return hasReceivedBodyInAck;
}

bool SipCallSip::OnStartChannel()
{
    //
    // Examine virtual RTP configuration.
    //

    assert(xgCall);
    if (!xgCall) {
	CLog::Warning(THISMODULE, "OnStartChannel(): not attached to Call");
	return false;
    }

    // Skip if we're not using virtual RTP.
    if (!xgCall->IsUsingVirtualRtp())
	return true;

    // Skip if our local media is already set.
    if (xgCall->GetLocalMedia() != NULL)
	return true;

    // Check that we should have peer RTP call.
    XgSipCall *peer_xg_call = xgCall->GetPeerSipCall();
    if (peer_xg_call == NULL || !peer_xg_call->IsValid()) {
	CLog::Warning(THISMODULE, "OnStartChannel(): unable to start media on Virtual RTP. "
				  "Peer SIP call is not available");
	return false;
    }

    // We can only proceed if peer is also using virtual RTP.
    if (!peer_xg_call->IsUsingVirtualRtp()) {
	CLog::Warning(THISMODULE, "OnStartChannel(): unable to start media on Virtual RTP. "
				  "Peer SIP call is not using virtual RTP.");
	return false;
    }

    // Get remote media description in peer, and make it our local media.
    const SipSdpBody *remote_body = peer_xg_call->GetRemoteMedia();
    if (remote_body == NULL) {
	CLog::Debug(THISMODULE, "OnStartChannel(): unable to start media on Virtual RTP. "
				  "Remote media is not available in peer SIP call.");
	return false;
    }

    MY_DEBUG(this, "SetLocalMedia");
    xgCall->SetLocalMedia(remote_body);
    return true;
}

/* Must override OnIncomingInvitation, because when OnCreateChannel is called,
   this call must already have an xGate media.

   The problem is, when the call failed just after OnIncomingInvitation (such
   as when authorization failed), the SIP framework wouldn't call OnCallDisconnected,
   because OnIncomingCall hasn't been called yet.
 */
bool SipCallSip::OnIncomingInvitation(const SipRequestMessage &req_msg) 
{
    const SipAuthorization *h_auth = 
	(const SipAuthorization*) req_msg.FindHeader(SipHeader::H_Authorization);
    if (h_auth) {
	hasSipAuth = true;
	sipAuth = new XgSipAuthEntry;
	sipAuth->operator =(h_auth->authEntry);
    } else {
	hasSipAuth = false;
    }

    if (!hasSipAuth) {
	const SipProxyAuthorization *p_auth =
	    (const SipProxyAuthorization*) req_msg.FindHeader(SipHeader::H_ProxyAuthorization);
	if (p_auth) {
	    hasSipAuth = true;
	    sipAuth = new XgSipAuthEntry;
	    sipAuth->operator =(p_auth->authEntry);
	} else {
	    hasSipAuth = false;
	}
    }

    MY_DEBUG(this, "OnIncomingInvitation");
    return XgSipSignalling::Instance()->OnIncomingInvitation(this, req_msg);
}

bool SipCallSip::RequireAuthorization()
{
    assert(xgCall);
    if (!xgCall)
	return false;

    Rtp_Interface *itf = (Rtp_Interface *) xgCall->GetMedia()->GetInterface();
    assert(itf);
    return itf->RequireSipAuthorization();
}

bool SipCallSip::OnIncomingCall(const SipRequestMessage &req_msg) 
{
    /*
    if (!XgSipSignalling::Instance()->OnIncomingInvitation(this, req_msg))
	return false;
    */

    if (xgCall) {
	// If authorization is configured, then make sure user is authorized
	// to use the specified interface.
	Rtp_Interface *itf = (Rtp_Interface *) xgCall->GetMedia()->GetInterface();
	if (!itf->Authorize(GetAuthUser())) {
	    SetTerminationReason(scForbidden);
	    return false;
	}

	// Authorization success, notify call manager.
	return xgCall->OnIncomingCall();

    } else {
	SetTerminationReason(scInternalServerError);
	return false;
    }
}

bool SipCallSip::OnCallProceeding(SipStatusCode status_code, const String& phrase, 
				  const SipResponseMessage &res_msg) 
{
    if (xgCall)
	return xgCall->OnCallProceeding(status_code,phrase,res_msg);
    else {
	SetTerminationReason(scRequestCancelled);
	return false;
    }
}

void SipCallSip::OnReceivedInfo (const SipRequestMessage & info_request_msg) 
{
    if (xgCall)
	xgCall->OnReceivedInfo(info_request_msg);
    else {
	SetTerminationReason(scInternalServerError);
	DisconnectCall();
    }
}

void SipCallSip::OnCallRedirected(const List<SipContactElement > &new_destinations,
				  const SipResponseMessage &res_msg) 
{
    if (xgCall)
	xgCall->OnCallRedirected(new_destinations, res_msg);
}

bool SipCallSip::OnCallEstablished() 
{
    if (xgCall)
	return xgCall->OnCallEstablished();
    else {
	SetTerminationReason(scInternalServerError);
	return false;
    }
}

void SipCallSip::OnCallDisconnected() 
{
    if (xgCall)
	xgCall->OnCallDisconnected();
}

SipRtpBidirectionalChannel* SipCallSip::OnCreateChannel() 
{
    if (xgCall) return new SipRtpChannelSip(xgCall, (Rtp_Media*)xgCall->GetMedia());
    else return NULL;
}

SipMediaNegotiator * SipCallSip::CreateMediaNegotiator(Negotiator_Type type) const
{
    SipNegSip *neg = new SipNegSip;
    neg->AttachCall(xgCall);

    MY_DEBUG(type, "CreateMediaNegotiator");

    if (type == LOCAL_NEGOTIATOR && xgCall) {

	// If we're using Virtual RTP, get the SDP from peer call, if any.
	if (xgCall->IsUsingVirtualRtp() && xgCall->GetPeerSipCall()) {
	    XgSipCall *peer = xgCall->GetPeerSipCall();
	    if (peer == NULL || !peer->IsUsingVirtualRtp()) {
		CLog::Error(THISMODULE, "Unable to make SIP call on virtual RTP "
					"(peer SIP call is not available, or peer is not using "
					"virtual RTP)");
		delete neg;
		return NULL;
	    }

	    const SipSdpBody *body = peer->GetRemoteMedia();
	    if (body == NULL) {
		CLog::Error(THISMODULE, "Unable to make SIP call on virtual RTP "
					"(remote media is not available in peer SIP call)");
		delete neg;
		return NULL;
	    }
	    neg->SetSdp(body);
	}
    }
    return neg;
}

bool SipCallSip::OnTransactionSuccess (SipTransaction &tsx,
				       SipTransaction::Direction msg_dir,
				       const SipMessage *msg)
{
    /* Handle SDP that's coming in 200/response of outgoing INVITE */
    if (xgCall!=NULL && 
	tsx.GetMethod().GetMethod() == SipMethod::InviteMethod &&
	tsx.GetDirection() == SipTransaction::Outgoing &&
	msg && msg->GetType() == SipMessage::ResponseMessage &&
	((SipResponseMessage*)msg)->GetStatusClass() == 2 &&
	msg->GetBody().get() != NULL)
    {

	// Also check the presence of RFC 2833 in response.
	const SipSdpBody *sdp = (const SipSdpBody*)msg->GetBody().get();

	const SdpMediaDescriptor &m = sdp->mediaDescriptor[0];
	List<SdpRtpmapAttribute>::const_iterator it, end;

	end = m.attrRtpmapList.end();
	for (it=m.attrRtpmapList.begin(); it!=end; ++it) {
	    const SdpRtpmapAttribute &rtpmap = *it;

	    if (rtpmap.encodingName == "telephone-event") {
		xgCall->rfc2833_tx_pt = rtpmap.payloadType;
		TRACE_RFC2833_("sip", "RFC2833 telephone-event support detected in incoming 200/OK,"
				      "rx_pt=%d, tx_pt=%d", 
				      xgCall->rfc2833_rx_pt,
				      xgCall->rfc2833_tx_pt);
		break;
	    }
	}

    }

    return SipCall::OnTransactionSuccess(tsx, msg_dir, msg);
}

void SipCallSip::OnTransactionReceivedAck2xx(SipTransaction &,
					     const SipRequestMessage *msg)
{
    if (hasReceivedAck)
	return;

    hasReceivedAck = true;

    if (!xgCall)
	return;

    // Ignore if we're not using virtual RTP.
    if (!xgCall->IsUsingVirtualRtp())
	return;

    // Ignore if doesn't have body.
    if (!msg->GetBody().get()) {
	//CLog::Warning(THISMODULE, "Received ACK without body. Will not send ACK to peer SIP call");
	return;
    }

    // Check Content-Type (must be SDP).
    SipContentType *ct = (SipContentType*) msg->FindHeader(SipHeader::H_ContentType);
    if (ct == NULL) {
	//CLog::Warning(THISMODULE, "Received ACK without body. Will not send ACK to peer SIP call");
	return;
    }

    if (ct->mediaType.typeSubtype != "application/sdp") {
	//CLog::Warning(THISMODULE, "Received ACK with non-SDP body. Will not send ACK to peer SIP call");
	return;
    }

    SipNegSip *neg = (SipNegSip*) remoteCap;
    SipSdpBody *sdp = (SipSdpBody *)msg->GetBody().get();
    neg->SetSdp(sdp);

    hasReceivedBodyInAck = true;
}


bool SipCallSip::OnTransactionToSendAck2xx( SipTransaction &trans,
					    unsigned cseq,
					    unsigned option)
{
    MY_DEBUG(0, "OnTransactionToSendAck2xx");

    // Remove 'IncludeBody' from option.
    option &= ~(IncludeBodyOption);

    // If we're using virtual RTP, then only send ACK if peer SIP call has
    // received ACK.
    XgSipCall *peer_xg = NULL;
    SipCallSip *peer_sip = NULL;
    SipSdpBody *sdp = NULL;
    SipNegSip *neg = NULL;

    if (!xgCall || !xgCall->IsValid() || !xgCall->IsUsingVirtualRtp()) 
	goto up_call;

    peer_xg = xgCall->GetPeerSipCall();
    if (!peer_xg || !peer_xg->IsValid() || !peer_xg->IsUsingVirtualRtp())
	goto up_call;

    peer_sip = peer_xg->GetSipCall();
    if (!peer_sip)
	goto up_call;

    // If peer hasn't received any SDP, then we can't send either.
    if (!peer_sip->HasReceivedAck())
	return true;

    if (peer_sip->HasReceivedBodyInAck()) {
	neg = (SipNegSip*)peer_sip->remoteCap;
	if (!neg)
	    return true;

	sdp = (SipSdpBody*) neg->GetSdp()->Clone();
    }

    return SendRequest(SipMethod::AckMethod, cseq, option, NULL,
		      sdp, NULL);

up_call:
    return SipCall::OnTransactionToSendAck2xx(trans, cseq, option);
}



///////////////////////////////////////////////////////////////////////////////

SipCall * SipAgentSip::OnCreateCall(SipTransport &transport,
				    const String &call_id,
				    CallDirection dir,
				    void * user_data)
{
    SipCall *sipcall = NULL;

    if (dir == IncomingCall || dir == OutgoingCall) {
	NEW_RETURN_ (sipcall, SipCallSip(transport, call_id));

    } else if (dir == IncomingRegister) {
	NEW_RETURN_ (sipcall, SipRegistrarCall(transport, call_id));

    } 
    return sipcall;
}

/** Notification when a packet is received.
 */
bool SipAgentSip::OnReceivedPDU(
    SipTransport & src_tr,		/// transport from which the PDU received
    const ACE_INET_Addr & src_addr,	/// the source address of the packet
    const char *pdu,			/// the PDU
    size_t pdu_size			/// the size of the PDU
)
{
    KLog::Instance()->TraceStandard(traceSipMsg, NULL, "RX <-- %s:%d\n%s", 
				    src_addr.get_host_addr(), src_addr.get_port_number(), pdu);
    return true;
}

/** Notification when the agent is about to send a message.
 */
void SipAgentSip::OnSendingMessage(
    SipTransport & transport,
    const char *msg,
    size_t length,
    const ACE_INET_Addr & dst_addr
)
{
    KLog::Instance()->TraceStandard(traceSipMsg, NULL, "TX --> %s:%d\n%s", 
				    dst_addr.get_host_addr(), dst_addr.get_port_number(), msg);
}

///////////////////////////////////////////////////////////////////////////////

XgSipCall::XgSipCall(const CallParameter & cparam, CallDirection dir, const char *itf_name)
: Call(dir, itf_name), signature1(SIGNATURE),
  sipCall(NULL), disconnectReq(false),
  disconnectReqCause(CC_NormalUnspecified), cmIsNotified(false), signature2(SIGNATURE+1),
  peerSipCall(NULL)
{
    _callParam = cparam;

    peerSipCall = GetXgSipCallFromCParam(&cparam);
    if (peerSipCall) {
	peerSipCall->peerSipCall = this;
    }

    _callParam.extraInfo.data.resize(sizeof(this));
    * _callParam.extraInfo.data.begin() = (unsigned)this;

    rfc2833_rx_pt = rfc2833_tx_pt = -1;
}

XgSipCall::~XgSipCall()
{
    signature1 = signature2 = 0;
    if (peerSipCall && peerSipCall->peerSipCall==this)
	peerSipCall->peerSipCall = NULL;
}

bool XgSipCall::IsValid() const
{
    try {
	return (signature1 == SIGNATURE) && (signature2 == SIGNATURE+1);
    } catch (...) {
	return false;
    }
}

bool XgSipCall::IsUsingVirtualRtp() const
{
    XgSipCall *this_ = (XgSipCall*)this;
    Rtp_Media *med = (Rtp_Media*) this_->GetMedia();
    if (!med)
	return false;

    return med->GetChannel()->GetBoardType() == RTP_Board_Virtual;
}

void XgSipCall::AttachSipCall(SipCallSip *sipcall)
{
    // Detach first.
    if (sipCall) {
	sipCall->AttachCall(NULL);
    }

    sipCall = sipcall;
    if (sipCall) {
	sipCall->AttachCall(this);
    }
}

#define XG_SIP_UNSUP(f)		CLog::Warning(THISMODULE, "Unsupported function '%s'", #f)
#define XG_SIP_CHECK_CALL(f)	if (sipCall==NULL) { assert(false); CLog::Warning(THISMODULE, "%s: call has been cleared", #f); \
						     RelComInd(CC_CallHasBeenCleared); return false; }

bool XgSipCall::DeviceOverlapReq(const char* additionalDigits,bool complete)
{
    CLog::Error(THISMODULE, "Overlap call is not supported in SIP");
    return false;
}

bool XgSipCall::DeviceProceedingReq(CallProgress progress, CallExtraInfo* exinfo)
{
    XG_SIP_CHECK_CALL(DeviceProceedingReq);

    // Send PROGRESS instead
    return DeviceProgressReq(progress, exinfo);
}

bool XgSipCall::DeviceProgressReq(CallProgress progress, CallExtraInfo* exinfo)
{
    XG_SIP_CHECK_CALL(DeviceProgressReq);
    sipCall->StartMediaChannel();
    XG_SIP_CHECK_CALL(DeviceProgressReq);
    return sipCall->SendRingingIndication(scProgress);
}

bool XgSipCall::DeviceAlertingReq(CallProgress progress, CallExtraInfo* exinfo)
{
    XG_SIP_CHECK_CALL(DeviceAlertingReq);
    sipCall->StartMediaChannel();
    XG_SIP_CHECK_CALL(DeviceAlertingReq);
    return sipCall->SendRingingIndication(scRinging);
}

bool XgSipCall::DeviceSendInfo(CallExtraInfo* exinfo)
{
        return 0;
}

bool XgSipCall::DeviceConnectReq(CallExtraInfo* exinfo)
{
    XG_SIP_CHECK_CALL(DeviceConnectReq);
    if (!sipCall->StartMediaChannel()) {
	int i=0;
    }
    XG_SIP_CHECK_CALL(DeviceConnectReq);
    return sipCall->AnswerCall();
}

bool XgSipCall::DeviceSuspendReq()
{
    XG_SIP_CHECK_CALL(DeviceSuspendReq);
    XG_SIP_UNSUP(DeviceSuspendReq);
    XG_SIP_CHECK_CALL(DeviceSuspendReq);
    return false;
}

bool XgSipCall::DeviceResumeReq()
{
    XG_SIP_CHECK_CALL(DeviceResumeReq);
    XG_SIP_UNSUP(DeviceResumeReq);
    return false;
}


bool XgSipCall::DeviceDisconnectReq(CallCause cause,CallProgress progress)
{
    disconnectReq = true;
    disconnectReqCause = cause;

    if (!sipCall) {
	RelComInd(cause);
	return true;
    }

    SipStatusCode code;
    String reason;
    code = sip_ctengine_cause_mapping.GetSipStatus(cause, reason);
    sipCall->SetTerminationReason(code, disconnectMessage);
    sipCall->SetQ850TerminationCode(cause, reason);
    return sipCall->HangupCall();
}

void XgSipCall::SetDisconnectMsg(const char *msg)
{
    if (!sipCall) {
	CLog::Debug (THISMODULE, "Null sipCall object in XgSipCall::SetDisconnectMsg");
	return;
    }
    SipStatusCode code = sipCall->GetTerminationCode();
    disconnectMessage = msg;
    sipCall->SetTerminationReason (code, disconnectMessage);
}

int XgSipCall::GetAuthorizationHandle()
{
    if (sipCall) {
	return (int) sipCall->GetAuthorization();
    } else {
	return 0;
    }
}

/** Notifications from internal SipCall.
 */
bool XgSipCall::OnIncomingCall()
{
    cmIsNotified = true;
    SetupInd(*GetCallParameter());
    return true;
}

bool XgSipCall::OnCallProceeding(SipStatusCode status_code, const String& phrase, 
				 const SipResponseMessage &res_msg)
{
    CallProgress progress;
    if (res_msg.GetBody().get()) {
	progress = CP_InBandInfoAvailable;


	// Also check the presence of RFC 2833 in response.
	const SipSdpBody *sdp = (const SipSdpBody*)res_msg.GetBody().get();

	const SdpMediaDescriptor &m = sdp->mediaDescriptor[0];
	List<SdpRtpmapAttribute>::const_iterator it, end;

	end = m.attrRtpmapList.end();
	for (it=m.attrRtpmapList.begin(); it!=end; ++it) {
	    const SdpRtpmapAttribute &rtpmap = *it;

	    if (rtpmap.encodingName == "telephone-event") {
		rfc2833_tx_pt = rtpmap.payloadType;
		TRACE_RFC2833_("sip", "RFC2833: telephone-event support detected in %d response, tx_pt=%d", 
				      status_code, rfc2833_tx_pt);
		break;
	    }
	}


    } else
	progress = CP_NoIndication;

    switch (status_code) {
    case scTrying:
	// Only send proceeding IF media description is available
	if (progress==CP_InBandInfoAvailable) {
	    sipCall->StartMediaChannel();
	    ProceedingInd(progress, 0);
	}
	break;
    case scRinging:
	sipCall->StartMediaChannel();
	AlertingInd(progress, 0);
	break;
    case scProgress:
	sipCall->StartMediaChannel();
	/**
	if (GetState() < CS_Proceeding) ProceedingInd(progress);
	ProgressInd(progress);
	**/
	if (progress==CP_InBandInfoAvailable) AlertingInd(progress, 0);
	else {
	    if (GetState() < CS_Proceeding) ProceedingInd(progress, 0);
	    ProgressInd(progress, 0);
	}
	break;
    }
    return true;
}

void XgSipCall::OnReceivedInfo (const SipRequestMessage & info_request_msg)
{
    const AutoPtr<SipBody> & a_body = info_request_msg.GetBody();
    const SipBody * body = a_body.get();
    if (!body) 
	return;

    if (body->GetType().typeSubtype != "application/dtmf")
	return;

    OStringStream ostrm;
    //ostrm << *body;
    /* ostrm << *body; */

    GetRtpMedia()->DialDtmf(ostrm.Str());
}

void XgSipCall::OnCallRedirected(const List<SipContactElement > &new_destinations,
				 const SipResponseMessage &res_msg)
{
    AttachSipCall(NULL);
    DisconnectInd(CC_UnallocatedNumber, CP_NoIndication);
}

bool XgSipCall::OnCallEstablished()
{
    if (GetDirection()==CD_Outgoing)
	ConnectInd(0);
    else
	ConnectCnf();
    return true;
}

void XgSipCall::OnCallDisconnected()
{
    CallCause cause;

    if (sipCall) {
	cause = (CallCause)sipCall->GetQ850TerminationCode();
	if (cause == 0) cause = 
		sip_ctengine_cause_mapping.GetCtengineCause(sipCall->GetTerminationCode(),
							    sipCall->GetTerminationReason());
	AttachSipCall(NULL);
    } else {
	assert(false);
	cause = CC_NormalUnspecified;
    }

    if (GetDirection()==CD_Outgoing || cmIsNotified) {
	if (disconnectReq) {
	    RelComInd(disconnectReqCause);
	} else
	    DisconnectInd(cause, CP_NoIndication);
    } else {
	DetachMedia();
	DetachTask();
	CallManager::Instance()->DestroyCall(this);
    }
}


/** Tools function for internal SipCall.
 */
void XgSipCall::GetLocalMediaAddr(ACE_INET_Addr & addr)
{
    Rtp_Media *rtp = GetRtpMedia();
    rtp->GetLocalAddress(addr);
}

const SipSdpBody *XgSipCall::GetLocalMedia()
{
    assert(sipCall);
    if (!sipCall) {
	CLog::Warning(THISMODULE, "SIP call is not available in GetLocalMedia()"
				  " (call handle=%d)", this);
	return NULL;
    }

    SipNegSip *neg = sipCall->GetLocalNegotiator();
    if (!neg) {
	assert(false);
	CLog::Warning(THISMODULE, "SIP media negotiator is not available in GetLocalMedia()");
	return NULL;
    }

    return neg->GetSdp();
}

const SipSdpBody *XgSipCall::GetRemoteMedia()
{
    assert(sipCall);
    if (!sipCall) {
	CLog::Warning(THISMODULE, "SIP call is not available in GetRemoteMedia()"
				  " (call handle=%d)", this);
	return NULL;
    }

    SipNegSip *neg = sipCall->GetRemoteNegotiator();
    if (!neg) {
	//assert(false);
	CLog::Debug(THISMODULE, "SIP media negotiator is not available in GetRemoteMedia()");
	return NULL;
    }

    return neg->GetSdp();
}

void XgSipCall::SetLocalMedia(const SipSdpBody *body)
{
    //
    // Note that we allow NULL to be passed as parameter
    //
    assert(sipCall);
    if (!sipCall) {
	CLog::Warning(THISMODULE, "SIP call is not available in SetLocalMedia()"
				  " (call handle=%d)", this);
	return;
    }

    SipNegSip *neg = sipCall->GetLocalNegotiator();
    if (!neg) {
	assert(false);
	CLog::Warning(THISMODULE, "SIP media negotiator is not available in SetLocalMedia()");
	return;
    }

    neg->SetSdp(body);
}

///////////////////////////////////////////////////////////////////////////////

enum 
{
    Cmd_Require,
    Cmd_Session_Timer_Timeout,
};
#define S_REQUIRE		"require"
#define S_SES_TIMER_TIMEOUT	"session-timer-timeout"
#define S_SES_TIMER		"session-timer"


XgSipSignalling *XgSipSignalling::instance;

XgSipSignalling * XgSipSignalling::Create()
{
    if (instance == NULL)
	instance = new XgSipSignalling;

    if (XgSipRegistrar::Instance() == NULL)
	new XgSipRegistrar;

    return instance;
}

XgSipSignalling::XgSipSignalling()
: SignallingDriver(SigType_SIP, "SIP", "SIP VoIP Signalling", true, Low_Priority)
{
    requireTimer = true;
}

XgSipSignalling::~XgSipSignalling()
{
    instance = NULL;
}

bool XgSipSignalling::Load()
{
    traceSipMsg = KLog::Instance()->CreateTrace("sip-msg", "SIP", "SIP messages");

    SipAgentSip *agent;
    NEW_RETURN_(agent, SipAgentSip);
    if (!agent)
	return false;

    // Must start listener before can set active interface
    //Yoga commented this part due below compiler error
    //siptransport.h:295: error: default argument for parameter of type ‘ACE_INET_Addr’ has type ‘int’
    /*agent->StartListener(new SipUdpListener(5080));
    agent->StartListener(new SipTcpListener(5080));*/


    // create auth. server with static username list, and register to agent
    SipStaticAuthServer *auth = new SipStaticAuthServer;
    auth->SetNonceExpirationTimeout(60);
    //auth->SetDefaultDomain(SIP_AUTH_DEFAULT_DOMAIN);
    auth->SetDefaultRealm(SIP_AUTH_DEFAULT_REALM);
    agent->SetAuthServer(auth);
    
    if (agent->GetIfAddresses().size())
	agent->SetActiveInterface(agent->GetIfAddresses()[0].get_host_addr());
    else
	agent->SetAutoSetActiveInterface(true);

    agent->AddCapability(new SipNetCoderCapability(SipCapability::SendRecv, SipNetCoderCapability::Rate_6_4));
    agent->AddCapability(new SipNetCoderCapability(SipCapability::SendRecv, SipNetCoderCapability::Rate_7_2));
    agent->AddCapability(new SipNetCoderCapability(SipCapability::SendRecv, SipNetCoderCapability::Rate_8_0));
    agent->AddCapability(new SipNetCoderCapability(SipCapability::SendRecv, SipNetCoderCapability::Rate_8_8));
    agent->AddCapability(new SipNetCoderCapability(SipCapability::SendRecv, SipNetCoderCapability::Rate_9_6));
    agent->AddCapability(new SipG711_ALaw_Capability(SipCapability::SendRecv));
    agent->AddCapability(new SipG711_MuLaw_Capability(SipCapability::SendRecv));
    agent->AddCapability(new Sip_AcmG723Capability(SipCapability::SendRecv));
    agent->AddCapability(new SipG729_Capability(SipCapability::SendRecv));
    agent->AddCapability(new SipGSM0610Capability(SipCapability::SendRecv));

    agent->SetRequireExtension( SipAgent::TimerExtension, true );
    agent->SetSessionExpiresTimeout( 300 );


    // Create "require" command.
    Term_Cmd *cmd;
    cmd = new Term_Cmd(this, Cmd_Require, S_REQUIRE, "Enable/disable SIP extensions", 
		       PrivilegeLevelAdmin, TermModeConfig, true);
    CVariantChoiceValidator vreq(S_SES_TIMER, "Session Timer extension");
    cmd->Add (new Term_Cmd_Arg("@0", "", vreq));
    RegisterTermCmd(cmd, GetConfigTree());

    // Create "session-timer-timeout" command
    cmd = new Term_Cmd(this, Cmd_Session_Timer_Timeout, S_SES_TIMER_TIMEOUT, 
		       "Configure Session Timer interval",
		       PrivilegeLevelAdmin, TermModeConfig, false);
    cmd->Add( new Term_Cmd_Arg("@0", 300, CVariantRangeValidator(60, 86400, "Timer in seconds")) );
    RegisterTermCmd(cmd, GetConfigTree());

    // Register show handler.
    GetConfigTree()->RegisterShowHandler(this);
    Kernel::Instance()->RootConfig()->RegisterShowHandler(this);

    return true;
}

bool XgSipSignalling::Unload()
{
    KLog::Instance()->DestroyTrace(traceSipMsg);

    delete SipAgent::Instance();
    return true;
}

void XgSipSignalling::HandleEvents()
{
    static ACE_Time_Value timeout(0,0);
    SipAgent::Instance()->HandleEvents(timeout);
    /**
    for (unsigned i=0; i<8; ++i) {
	if (SipAgent::Instance()->HandleEvents(timeout) < 1)
	    break;
    }
    */
}

/** Show service status.
 */
bool XgSipSignalling::OnShowStatus(String & output)
{
    String indent(SHOW_STATUS_INDENT);
    SipAgent *agent = SipAgent::Instance();
    if (!agent)
	return false;

    OStringStream strm;
#ifndef __linux__
    agent->Dump(strm);
    output += strm.Str();
#endif

    return true;
}

void XgSipSignalling::DoTermCmd(Term_Session *session, bool no, Term_Cmd *cmd)
{
    IString arg0;
    CVariant v;
    SipAgent *agent = SipAgent::Instance();

    switch (cmd->GetId()) {
    case Cmd_Require:
	cmd->GetArg((int)0)->GetValue(v);
	arg0 = v.AsString();
	if (arg0 == S_SES_TIMER) {
	    if (no) {
		requireTimer = false;
	    } else {
		requireTimer = true;
	    }
	    agent->SetRequireExtension( SipAgent::TimerExtension, requireTimer );
	}
	break;
    case Cmd_Session_Timer_Timeout:
	cmd->GetArg((int)0)->GetValue(v);
	int timeout;
	timeout = v.AsInt();
	agent->SetSessionExpiresTimeout(timeout);
	agent->SetRequireExtension( SipAgent::TimerExtension, requireTimer );
	break;
    }
}

bool XgSipSignalling::OnShowConfig(const Cfg_Tree_Entry *entry,
				     String & output, 
				     const String & indent, 
				     int indent_size) const
{
    String new_indent(indent);
    new_indent.append(indent_size, ' ');

    ACQUIRE_SYSTEM_LOCK();
    SipAgent *agent = SipAgent::Instance();
    bool ses_timer_required = agent->IsExtensionRequired(SipAgent::TimerExtension);
    int ses_timer_interval = agent->GetSessionExpiresTimeout();
    RELEASE_SYSTEM_LOCK();

    char temp[80];
    output += indent + "signalling sip\n";
    if (ses_timer_required)
	output += new_indent + "require " S_SES_TIMER "\n";
    else
	output += new_indent + "no require " S_SES_TIMER "\n";

    sprintf(temp, " %d", ses_timer_interval);
    output += new_indent + S_SES_TIMER_TIMEOUT + temp + "\n";

    output += new_indent + "top\n\n";
    return OK;
}

/** AttachInterface: scan interface codec capability
 */
bool XgSipSignalling::AttachInterface(Interface *itf) 
{
    /*
    int codec[20], i1[10], i2[10];
    std::set_intersection(i1, i1+10, i2, i2+10, codec);
    */
    return true;
}

/** DetachInterface: re-scan interface codec capability
 */
void XgSipSignalling::DetachInterface(Interface *itf) 
{
    // Do nothing ?
}

/** SetupReq: Initiate a new outgoing call in a specific Interface
    using the specified call parameter
 */
Call *XgSipSignalling::SetupReq(Interface *itf,SelectionStrategy selection,CallParameter *cparam)
{
    // Create call
    XgSipCall *xcall;
    NEW_RETURN_ (xcall, XgSipCall(*cparam, CD_Outgoing, itf->Name().c_str()));
    if (!xcall)
	return NULL;

    // Allocate one RTP media.
    Rtp_Interface *rtp_itf = (Rtp_Interface *)itf;
    Media *rtp_media = NULL;
    if (!rtp_itf->AllocateMedia(xcall, rtp_media, 0, CD_Outgoing, cparam)) {
	assert(rtp_media == NULL);
	xcall->SetCause(CC_ResourceUnavailable);
	return xcall;
    }
    
    xcall->MediaAvailableInd(rtp_media);

    // Check if RTP interface wants to use RFC 2833
    xcall->rfc2833_rx_pt = rtp_itf->DTMFGetRfc2833();
    TRACE_RFC2833_("sip", "RFC2833 support is %s for outgoing call, rx_pt=%d",
			 (xcall->rfc2833_rx_pt>0 ? "enabled" : "disabled"),
			 xcall->rfc2833_rx_pt);

    // Pass silence compression setting to SDP for G.729 attribute
    RTP_Config config;
    rtp_itf->GetRtpChannelConfig(config);
    xcall->silenceCompression = config.SilenceCompression;

    // Create URLS from CallParameters.
    SipNameAddress dst, src;
    List <SipHeader *> additional_headers;
    CallParam2Url(cparam, src, dst, additional_headers);

    SipCall *sipcall = SipAgent::Instance()->CreateCall(SipAgent::OutgoingCall, dst, NULL, &src);

	// setting max forwards based on target RTP interface setting
	sipcall->SetMaxForwards(rtp_itf->GetMaxForwards());

    if (!sipcall) {
	//bugs!!
	//SipStatusCode code = sipcall->GetTerminationCode();
	xcall->SetCause(CC_ResourceUnavailable);
	xcall->DetachMedia();
	DestroyList(additional_headers);
	return xcall;
    }

    // Must attach before make call (==> negotiator)
    xcall->AttachSipCall((SipCallSip*)sipcall);

    // Don't send SDP in ACK if we're using Virtual RTP.
    /*
    if (xcall->IsUsingVirtualRtp()) {
	sipcall->SetNoSdpInAck(true);
    }
    */

    if (!SipAgent::Instance()->MakeCall(sipcall, &additional_headers)) {
	xcall->AttachSipCall(NULL);
	SipStatusCode code = sipcall->GetTerminationCode();
	xcall->SetCause(sip_ctengine_cause_mapping.GetCtengineCause(code, NullString));
	xcall->DetachMedia();
	DestroyList(additional_headers);
	return xcall;
    }

    DestroyList(additional_headers);
    return xcall;
}

/** Notifications from SIP that an incoming call has arrived.
 */
bool XgSipSignalling::OnIncomingInvitation(SipCallSip *sipcall, const SipRequestMessage & req_msg)
{
    // Allocate one media.
    CallParameter cparam;
    CreateCallParam(cparam, req_msg);

    ACE_INET_Addr remote_sig_addr = sipcall->GetRemoteAddress();
    ACE_INET_Addr local_sig_addr;   // dummy

    Rtp_Interface *itf = RtpInterfaceManager::Instance()->DispatchIncomingCall(SigType_SIP, 
									       remote_sig_addr,
									       local_sig_addr,
									       cparam);
    Media *med = NULL;
    if (itf==NULL || !itf->AllocateMedia(NULL, med, 0, CD_Incoming, &cparam) || med==NULL) {
	sipcall->SetTerminationReason(scServiceUnavailable);

	OStringStream str;
	const SipFrom * from = req_msg.GetHeaders().From();
#ifndef __linux__
	str << *from;
#endif

	CLog::Warning(THISMODULE, "Unable to allocate RTP media for incoming SIP call from '%s'",
				  str.Str());
	return false;
    }

    Rtp_Media *rtp_med = (Rtp_Media*)med;

    // Create a new call.
    XgSipCall *xcall;
    NEW_RETURN_(xcall, XgSipCall(cparam, CD_Incoming, med->GetInterface()->Name().c_str()));
    if (!xcall) {
	med->GetInterface()->ReleaseMedia(med, CD_Incoming, &cparam, CC_ResourceUnavailable);
	sipcall->SetTerminationReason(scServiceUnavailable);
	return false;
    }

    xcall->MediaAvailableInd(med);
    xcall->AttachSipCall(sipcall);

    // Disable SDP body in ACK if we're using Virtual RTP.
    if (xcall->IsUsingVirtualRtp()) {
	/*
	sipcall->SetNoSdpInAck(true);
	*/
    }

    /* Check if RTP interface supports RFC 2833 */
    xcall->rfc2833_rx_pt = itf->DTMFGetRfc2833();

    /* Check if remote supports RFC 2833 */
    const SipSdpBody *sdp = (const SipSdpBody*)req_msg.GetBody().get();
    if (sdp) {
	const SdpMediaDescriptor &m = sdp->mediaDescriptor[0];
	List<SdpRtpmapAttribute>::const_iterator it, end;

	end = m.attrRtpmapList.end();
	for (it=m.attrRtpmapList.begin(); it!=end; ++it) {
	    const SdpRtpmapAttribute &rtpmap = *it;

	    if (rtpmap.encodingName == "telephone-event") {
		xcall->rfc2833_tx_pt = rtpmap.payloadType;
		break;
	    }
	}
    }


    if (xcall->rfc2833_rx_pt < 0 && xcall->rfc2833_tx_pt > 0) {
	TRACE_RFC2833_("sip", "RFC2833 support enabled for incoming call, "
			      "rx_pt=%d, tx_pt=%d",
			      xcall->rfc2833_rx_pt, 
			      xcall->rfc2833_tx_pt);
    }
    return true;
}

/** CreateDefaultCallParameter: Returns a CallParameter structure containing
    default values for this signalling protocol
 */
CallParameter *XgSipSignalling::CreateDefaultCallParameter()
{
    CallParameter *cparam;
    NEW_RETURN_(cparam, CallParameter);
    if (!cparam)
	return NULL;

    InitDefaultCallParameter(*cparam);
    return cparam;
}

/** Initialize default call parameter.
 */
void XgSipSignalling::InitDefaultCallParameter( CallParameter & cparam )
{
    cparam.cct = CCT_Speech;		// connection type
    cparam.ccl = CCL_NoIndication;	// companding law

    cparam.cpcCalled = CPC_Ordinary;	// party category
    cparam.cnpCalled = CNP_SIP;		// call number plan
    cparam.cntCalled = CNT_Unknown;	// call number type
    cparam.cnCalled.erase();
    cparam.complete = true;
    cparam.csCalled.erase();

    cparam.cui.erase();

    cparam.cpcCalling = CPC_Ordinary;
    cparam.cnpCalling = CNP_SIP;
    cparam.cntCalling = CNT_Unknown;
    cparam.csCalling.erase();
    cparam.ccp = CCP_Allowed;		// call calling presentation.
    cparam.ccs = CCS_Network;		// screening.

    cparam.extraInfo.owner = THISMODULE;
    cparam.extraInfo.sendable = false;
}

///////////////////////////////////////////////////////////////////////////////
/*
 * SIP Registrar service.
 */
XgSipRegistrar::XgSipRegistrar()
: ServiceProvider("SIPReg", false, true, Lowest_Priority, "SIP Registrar Server"), 
  Registrar_Agent("REGISTER")
{
    instance = this;
    time_to_check_ = time(NULL) + CHECK_INTERVAL;
    total_requests_ = 0;
    total_answered_ = 0;
    total_success_ = 0;
    total_timeout_ = 0;
}

XgSipRegistrar::~XgSipRegistrar()
{
    instance = NULL;
}

XgSipRegistrar *XgSipRegistrar::Instance()
{
    return instance;
}

bool XgSipRegistrar::Load()
{
    return true;
}

bool XgSipRegistrar::CommitConfiguration()
{
    return true;
}

bool XgSipRegistrar::Unload()
{
    return true;
}

void XgSipRegistrar::AddCall(SipRegistrarCall *call)
{
    calls_.insert(call);
}

void XgSipRegistrar::RemoveCall(SipRegistrarCall *call)
{
    calls_.erase(call);
}

void XgSipRegistrar::HandleEvents()
{
    time_t now = time(NULL);
    if (now >= time_to_check_) {
	std::set<SipRegistrarCall*>::iterator it, end;
	end = calls_.end();
	for (it=calls_.begin(); it!=end; ++it) {
	    SipRegistrarCall *c = *it;
	    if (now - c->GetStartTime() > CALL_TIMEOUT_INTERVAL) {
		++total_timeout_;
		++total_answered_;
		CLog::Warning(THISMODULE, "SIP Registration timeout waiting response from task");
		c->SendResponse(scInternalServerError, "Server Timed Out", NULL);
		calls_.erase(it);
		break;
	    }
	}
	time_to_check_ = now + CHECK_INTERVAL;
    }
}

void XgSipRegistrar::OnIncomingRegistration( SipRegistrarCall *call, 
					     const ACE_INET_Addr & src_addr,
					     SipRequestMessage *req_msg)
{
    const SipTo *to = req_msg->GetHeaders().To();
    const SipFrom *from = req_msg->GetHeaders().From();
    const SipCallId *cid = req_msg->GetHeaders().CallId();
    const SipCSeq *cseq = req_msg->GetHeaders().CSeq();

    ++total_requests_;

    CLog::Detail(THISMODULE, "Incoming SIP REGISTER request for %s@%s from %s:%d",
		  to->address.GetUser().c_str(), to->address.GetHost().c_str(),
		  src_addr.get_host_addr(), src_addr.get_port_number());

    /*
     * Steps for processing REGISTER request.
     *	- must inspect domain (skipped).
     *  - check Require header to see if we're required to do something.
     */
    const SipRequire *require = (const SipRequire *)req_msg->FindHeader(SipHeader::H_Require);
    if (require && !require->option.empty()) {
	SipStatusCode response = scBadExtension;

	SipUnsupported *unsupported = new SipUnsupported;
	unsupported->option = require->option;

	List<SipHeader*> hdrs;
	hdrs.push_back(unsupported);

	CLog::Warning(THISMODULE, "Rejecting SIP REGISTER request for %s@%s from %s:%d (bad extension)",
				  to->address.GetUser().c_str(), to->address.GetHost().c_str(),
				  src_addr.get_host_addr(), src_addr.get_port_number());
	call->SendResponse(scBadExtension, "", &hdrs);
	return;
    }


    Reg_Info info;

    /* Fill in basic information. 
     */
    /* Narasimham commented */
    /*info.agent = this;
    info.token = (Reg_Token)call;
     */
    info.ses_id = cid->callId;
    info.sequence = cseq->cSeq;
    info.domain = call->GetRequestUrl().GetHost();

    SipUserAgent *hua = (SipUserAgent*) req_msg->FindHeader(SipHeader::H_UserAgent);
    if (hua) {
	info.user_agent = hua->userAgent;
    }

    /* Next step:
     *	- should authenticate user.
     *	- we'll leave further authentication to applicatoin.
     */
    const SipAuthorization *sipau = (const SipAuthorization *)req_msg->FindHeader(
					SipHeader::H_Authorization, NULL);
    if (sipau) {
	info.has_auth = true;
	info.auth = sipau->authEntry;
    } else {
	info.has_auth = false;
    }

    if (!info.has_auth) {
	const SipProxyAuthorization *p_au =
	    (const SipProxyAuthorization*) req_msg->FindHeader(SipHeader::H_ProxyAuthorization, NULL);
	if (p_au) {
	    info.has_auth = true;
	    info.auth = p_au->authEntry;
	}
    }

    /* Next step:
     *	- extract Address of Record from To: header.
     */
    info.reg_rec_addr = to->address.GetHost();
    info.reg_rec_id = to->address.GetUser();

    /* Fill in source address and id.
     */
    char addr[80];
    sprintf(addr, "%s:%d", src_addr.get_host_addr(), src_addr.get_port_number());
    //info.src_addr = from->address.GetHost();
    info.src_addr = addr;
    info.src_id = from->address.GetUser();

    /* Next step:
     *	- check Contact header.
     */
    const SipContact *contact = (const SipContact *)req_msg->FindHeader(SipHeader::H_Contact, NULL);
    const SipExpires *expires = (const SipExpires *)req_msg->FindHeader(SipHeader::H_Expires, NULL);

    if (contact == NULL) {
	CLog::Info(THISMODULE, "Received invalid REGISTER (no Contact header) from %s", addr);
	call->SendResponse(scBadRequest, "", NULL);
	return;
    }

    enum { DEFAULT_EXPIRATION = 3600 };

    while (contact != NULL) {
	if (contact->wildcardFlag) {

	    if (req_msg->FindHeader(SipHeader::H_Contact, contact) != NULL ||
		info.regs.size() != 0) 
	    {
		CLog::Warning(THISMODULE, "Rejecting SIP REGISTER request for %s@%s from %s:%d (bad Contact)",
			      to->address.GetUser().c_str(), to->address.GetHost().c_str(),
			      src_addr.get_host_addr(), src_addr.get_port_number());

		call->SendResponse(scBadRequest, "", NULL);
		return;
	    }
	    if (expires && expires->timeSpec.GetAsInterval() != 0) {
		CLog::Warning(THISMODULE, "Rejecting SIP REGISTER request for %s@%s from %s:%d (bad Contact)",
			      to->address.GetUser().c_str(), to->address.GetHost().c_str(),
			      src_addr.get_host_addr(), src_addr.get_port_number());

		call->SendResponse(scBadRequest, "", NULL);
		return;
	    }

	    Reg_Entry entry;
	    entry.addr = "";
	    entry.user = "*";
	    entry.expiration = 0;
	    entry.priority = 1;
	    info.regs.push_back(entry);

	} else {
	    List<SipContactElement>::const_iterator it, end = contact->contactElement.end();
	    for (it=contact->contactElement.begin(); it!=end; ++it) {
		Reg_Entry entry;
		const SipContactElement & e = *it;
		entry.user = e.nameAddress.GetUser();

		sprintf(addr, "%s:%d", e.nameAddress.GetHost().c_str(), e.nameAddress.GetPort());
		entry.addr = addr;
		entry.priority = strtod(e.qValue.c_str(), NULL);
		if (e.expiresParam.IsValid()) {
		    entry.expiration = e.expiresParam.GetAsInterval();
		} else if (expires) {
		    entry.expiration = expires->timeSpec.GetAsInterval();
		} else {
		    entry.expiration = DEFAULT_EXPIRATION;
		}

		info.regs.push_back(entry);
	    }

	    contact = (const SipContact *)req_msg->FindHeader(SipHeader::H_Contact, contact);
	}
    }

    /* Send trying */
    call->SendResponse(scTrying, "", NULL);

    /* Notify registrar manager. */
    if (!Registrar_Manager::Instance()->OnRegistration(&info)) {
	CLog::Warning(THISMODULE, "Unable to process SIP REGISTER for %s@%s from %s:%d",
		      to->address.GetUser().c_str(), to->address.GetHost().c_str(),
		      src_addr.get_host_addr(), src_addr.get_port_number());

	call->SendResponse(scInternalServerError, "", NULL);
	return;
    }

    CLog::Detail(THISMODULE, "SIP REGISTER request for %s@%s from %s:%d dispatched successfully",
		  to->address.GetUser().c_str(), to->address.GetHost().c_str(),
		  src_addr.get_host_addr(), src_addr.get_port_number());

}

void XgSipRegistrar::CancelRegistration(const Reg_Info *reg_info)
{
    AnswerRegistration(reg_info, Reg_Info::STATUS_SERVER_ERROR);
}

bool XgSipRegistrar::AnswerRegistration( const Reg_Info *reg_info,
					 int status)
{
    int i;

    SipRegistrarCall *call = (SipRegistrarCall *)reg_info->token;
    if (!call->IsValid()) {
	CLog::Info(THISMODULE, "SipRegistrar::InswerRegistration is called for an invalid call.");
	return false;
    }

    ++total_answered_;
    if (status == Reg_Info::STATUS_OK)
	++total_success_;

    List<SipHeader*> hdrs;
    SipBody *body = NULL;

    if (status / 100 == 2) {
	/* If response is 2xx, Build Contact header. */
	for (i=0; i<reg_info->regs.size(); ++i) {
	    const Reg_Entry & e = reg_info->regs[i];
	    SipContact *contact = new SipContact;

	    if (e.user == "*" || e.addr == "*") {
		contact->wildcardFlag = true;
		hdrs.push_back(contact);
		break;
	    } else {
		contact->wildcardFlag = false;

		SipContactElement el;
		el.nameAddress.user = e.user;

		String::size_type pos;
		if ((pos=e.addr.find(':')) != String::npos) {
		    el.nameAddress.host = e.addr.substr(0, pos);
		    el.nameAddress.port = atoi(e.addr.c_str() + (pos+1));
		} else {
		    el.nameAddress.host = e.addr;
		}
		if (e.priority != 0.0) {
		    char q[128];
		    double prio = e.priority;
		    sprintf(q, "%4.3lf", prio);
		    el.qValue = q;
		}
		el.expiresParam.SetAsInterval(e.expiration);

		contact->contactElement.push_back(el);
		hdrs.push_back(contact);
	    }
	}

	/* Build expires header based on the least expiration. */
	SipExpires *expires = new SipExpires;
	unsigned least_expiration = 0xFFFFFFFF;
	if (reg_info->regs.empty()) {
	    least_expiration = 0;
	} else {
	    for (i=0; i<reg_info->regs.size(); ++i) {
		if (reg_info->regs[i].expiration < least_expiration) {
		    least_expiration = reg_info->regs[i].expiration;
		}
	    }
	}

	expires->timeSpec.SetAsInterval(least_expiration);
	hdrs.push_back(expires);

	/* Build Date header. */
	SipDate *date = new SipDate;
	date->date.SetAsAbsoluteFromLocal(time(NULL));
	hdrs.push_back(date);


	/* Build body if extra info is specified. */
	if (reg_info->extra_info.size()) {
	    body = new SipTextBody("text/x-vectone-info", reg_info->extra_info);
	}

    } else if (status == scUnauthorized) {

	 /* If response is 401 (Unauthorized), send WWW-Authenticate header. */
	SipAuthServer *auth = SipAgent::Instance()->GetAuthServer();
	assert(auth);
	
	SipWWWAuthenticate * www_auth = new SipWWWAuthenticate;
	hdrs.push_back(www_auth);
    
	// create challenge
	auth->CreateChallenge(www_auth->authEntry, false);

    } else if (status == scProxyAuthenticationRequired) {
	 /* If response is 407 , send Proxy-Authenticate header. */
	SipAuthServer *auth = SipAgent::Instance()->GetAuthServer();
	assert(auth);
	
	SipProxyAuthenticate * p_auth = new SipProxyAuthenticate;
	hdrs.push_back(p_auth);
    
	// create challenge
	auth->CreateChallenge(p_auth->authEntry, false);

    }

    return call->SendResponse((SipStatusCode)status, "", &hdrs, body);
}


bool XgSipRegistrar::OnShowStatus(String & output)
{
    char line[80];
    int percent, f_percent;
    output += SHOW_STATUS_INDENT "SIP Registrar Agent\n";
    
    if (total_requests_ == 0) {
	percent = 0;
	f_percent = 0;
    } else {
	percent = total_success_ * 100 / total_requests_;
	f_percent  = (total_answered_ - total_success_) * 100 / total_requests_;
    }

    sprintf(line, SHOW_STATUS_INDENT "Total %u requests, %u success (%u%%), %u failed (%u%%), %u pending\n",
		  total_requests_, total_success_, percent, 
		  total_answered_ - total_success_, f_percent,
		  total_requests_ - total_answered_);
    output += line;

    sprintf(line, SHOW_STATUS_INDENT "Timeout interval is %d seconds\n", CALL_TIMEOUT_INTERVAL);
    output += line;

    if (!calls_.empty()) {
	time_t now;

	now = time(NULL);
	output += "\n";
	output += SHOW_STATUS_INDENT "Pending incoming registrations:\n";
	output += SHOW_STATUS_INDENT "   No Time Source Address  Port  Account Name       Domain                  \n";
	output += SHOW_STATUS_INDENT "  --------------------------------------------------------------------------\n";

	std::set<SipRegistrarCall*>::iterator it, end;
	end = calls_.end();
	int i;
	for (i=0, it=calls_.begin(); it!=end; ++it, ++i) {
	    SipRegistrarCall *c = *it;

	    if (c->LastAnswer() >= 200)
		continue;

	    int printed;
	    c->GetStateStr();

	    int elapsed = now - c->GetStartTime();
	    printed = _snprintf( line, sizeof(line), SHOW_STATUS_INDENT 
				 " %4d %3ds %-15s %-5d %-18s %-24s",
				 i+1, elapsed, 
				 c->GetRemoteAddress().get_host_addr(),
				 c->GetRemoteAddress().get_port_number(),
				 c->GetRemoteInfo().GetUser().c_str(),
				 c->GetRequestUrl().GetHost().c_str(),
				 c->GetStateStr());
	    line[printed] = '\0';
	    output += line;
	    output += "\n";
	}

    }	// calls_.empty()

    output += "\n";
    return true;
}

////////////////////////////////////////////////////////////////////////////////
SipRegistrarCall::SipRegistrarCall(SipTransport &t, const String &call_id)
: SipBasicCall(t, call_id), signature_(SIGNATURE), last_answer_(0)
{
    start_time_ = time(NULL);
    XgSipRegistrar::Instance()->AddCall(this);
}

SipRegistrarCall::~SipRegistrarCall()
{
    signature_ = 0;
    XgSipRegistrar::Instance()->RemoveCall(this);
}

bool SipRegistrarCall::IsValid() const
{
    try {
	return signature_ == SIGNATURE;
    } catch (...) {
	return false;
    }
}

time_t SipRegistrarCall::GetStartTime() const
{
    return start_time_;
}

const SipUrl & SipRegistrarCall::GetRequestUrl() const
{
    return request_url_;
}

const char * SipRegistrarCall::GetStateStr() const
{
    return SipCall::GetStateStr();
}

int SipRegistrarCall::LastAnswer() const
{
    return last_answer_;
}

bool SipRegistrarCall::OnTransactionStarted ( SipTransaction &tsx,
					      SipTransaction::Direction msg_dir,
					      const SipMessage *msg)
{
    assert(msg_dir == SipTransaction::Incoming);
    assert(msg->GetType() == SipMessage::RequestMessage);

    SipRequestMessage *req = (SipRequestMessage*)msg;
    request_url_ = req->requestLine.requestUri;

    SipVia *via = (SipVia*)msg->FindHeader(SipHeader::H_Via);
    SipViaElement &ve = via->viaElement.front();
    
    const char *addr = ve.receivedParam.size() ? ve.receivedParam.c_str() : ve.host.c_str();
    int port = ve.receivedPortParam ? ve.receivedPortParam : ve.port;
    if (!port)
	port = 5060;

    ACE_INET_Addr via_addr((unsigned short)port, addr);
    /*
    ACE_INET_Addr src_addr;
    src_addr = GetResponseAddress(*(SipRequestMessage*)msg, via_addr);
    */
    XgSipRegistrar::Instance()->OnIncomingRegistration(this, via_addr, req);

    return true;
}

bool SipRegistrarCall::OnTransactionProgressing ( SipTransaction &tsx,
						  SipTransaction::Direction msg_dir,
						  const SipMessage *msg)
{
    assert(msg->GetType() == SipMessage::ResponseMessage);
    SipResponseMessage *res = (SipResponseMessage*)msg;

    last_answer_ = res->statusLine.GetStatusCode();
    return SipBasicCall::OnTransactionProgressing(tsx, msg_dir, msg);
}

bool SipRegistrarCall::OnTransactionSuccess ( SipTransaction &tsx,
					      SipTransaction::Direction msg_dir,
					      const SipMessage *msg)
{
    assert(msg->GetType() == SipMessage::ResponseMessage);
    SipResponseMessage *res = (SipResponseMessage*)msg;

    last_answer_ = res->statusLine.GetStatusCode();

    return SipBasicCall::OnTransactionSuccess(tsx, msg_dir, msg);
}

void SipRegistrarCall::OnTransactionFailed (SipTransaction &tsx,
					    SipTransaction::Direction msg_dir,
					    const SipMessage *msg)
{
    assert(msg->GetType() == SipMessage::ResponseMessage);
    SipResponseMessage *res = (SipResponseMessage*)msg;

    last_answer_ = res->statusLine.GetStatusCode();
}

void SipRegistrarCall::OnTransactionCleared(SipTransaction &tsx)
{
    SipBasicCall::OnTransactionCleared(tsx);
}


#endif	// XGATE_HAS_SIP
