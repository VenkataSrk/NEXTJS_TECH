/* $Header: /xGate/rtpsig.h 10    17/07/02 12:18 Adham $
 *   
 * XGATE
 * (C) 2001 Switchlab, Ltd
 *
 * RTP Signalling
 *
 * $Log: /xGate/rtpsig.h $
 * 
 * 10    17/07/02 12:18 Adham
 * Added include xgconfig.h for conditional pre processor in rtpmedia.h
 * that included from this file
 * 
 * 9     28/03/02 23:17 Junanto
 * version 38.18
 * 
 * 8     4/02/02 12:33 Sonny
 * calls get quality from media
 * 
 * 7     24/01/02 20:58 Sonny
 * Quality monitoring support
 * 
 * 6     8/28/01 1:57p Bennylp
 * Added keepalive feature
 * 
 * 5     8/13/01 5:40p Bennylp
 * Supports for configurable port (requires restart).
 * 
 * 4     7/30/01 10:20p Bennylp
 * Disconnect call in case of Destination Unreachable.
 * 
 * 3     7/14/01 5:23p Bennylp
 * Configurable retransmission rules/timeout.
 * 
 * 2     24/06/01 20:08 Junanto
 * Changed AttachInterface to return bool
 * 
 * 1     6/22/01 7:43p Bennylp
 * Initial coding.
 */
#ifndef __XGATE_RTPSIG_H__
#define __XGATE_RTPSIG_H__

#include "callmanager.h"
#include "MediaManager.h"
#include "SignallingDriver.h"
#include "../SPT/SPT_Q931/spt_q931sm.h"
#include "mtnrtp.h"
/* Forward decl.
 */
class Rtp_Media;


///////////////////////////////////////////////////////////////////////////////
class RTP_Call : public Call {
public:
    RTP_Call (CallDirection dir);
    ~RTP_Call();

    /** Override from Call.
     */
	//virtual void SetupInd_(CallParameter &cparam);
	//virtual void OverlapInd_(const char *additionalDigits,bool complete);
    virtual bool DeviceOverlapReq(const char *additionalDigits,bool complete);
    virtual bool DeviceProceedingReq(CallProgress progress, CallExtraInfo* exinfo);
    virtual bool DeviceProgressReq(CallProgress progress, CallExtraInfo* exinfo);
    virtual bool DeviceAlertingReq(CallProgress progress, CallExtraInfo* exinfo);
    virtual bool DeviceConnectReq(CallExtraInfo* exinfo);
    virtual bool DeviceSendInfo(CallExtraInfo* exinfo);
    virtual bool DeviceSuspendReq();
    virtual bool DeviceResumeReq();
    virtual bool DeviceDisconnectReq(CallCause cause,CallProgress progress);

    /* Commands to Q.931 signalling */
    void SetupReq (Interface *itf,
		   SelectionStrategy selection,
		   CallParameter *cparam);
    
    /* Notifications from underlying Q.931 call control */
    bool q931_OnSetupInd (Q931_call_t call, Q931_msg_rec *msg);
    void q931_OnMoreInfoInd (Q931_msg_rec *msg);
    void q931_OnInfoInd (Q931_msg_rec *msg);
    void q931_OnProceedingInd (Q931_msg_rec *msg);
    void q931_OnAlertingInd (Q931_msg_rec *msg);
    void q931_OnProgressInd (Q931_msg_rec *msg);
    void q931_OnConnectInd (Q931_msg_rec *msg);
    void q931_OnConnectAckInd (Q931_msg_rec *msg);
    void q931_OnNop();
    void q931_OnDisconnectInd (Q931_msg_rec *msg);
    void q931_OnReleaseCompInd (Q931_msg_rec *msg);

private:
    Q931_call_t  callHandle;
    bool cmIsNotified;

    // Saved dialed number for incoming overlap.
    bool    overlapReceiving;
    String  overlapNumber;
    bool    overlapComplete;


    /* Commands to Q.931 signalling */
    void q931_MakeCall();
    int  q931_Disconnect(Q931_call_t callhnd, CallCause cause);

    void NotifyIncomingCall();
    bool StartMedia(Q931_msg_rec *msg);

    Rtp_Media * GetRtpMedia() {
	return (Rtp_Media *)GetMedia();
    }

    bool IsMediaStarted();
    bool IsMediaError();
};


///////////////////////////////////////////////////////////////////////////////
class RTP_Signalling : public SignallingDriver {
public:
    static RTP_Signalling *Create();
    static RTP_Signalling *Instance() {
	return rtpSignallingInstance;
    }

    virtual void HandleEvents();
    virtual Call *SetupReq (Interface *itf,
			    SelectionStrategy selection,
			    CallParameter *cparam);
    virtual bool Load();
    virtual bool Unload();
    virtual bool CommitConfiguration();

    /** CreateDefaultCallParameter: Returns a CallParameter structure containing
	default values for this signalling protocol
     */
    virtual CallParameter *CreateDefaultCallParameter();

    /** AttachInterface: A new Interface object is attached to this 
	SignallingDriver object
     */
    virtual bool AttachInterface(Interface *itf) { return true; }

    /** DetachInterface: Remove association between a Interface object and this 
	SignallingDriver object
     */
    virtual void DetachInterface(Interface *itf) {}

    /** Display service information.
     */
    virtual bool OnShowStatus(String & output);

    /** Show configuration for the signalling.
     */
    virtual bool OnShowConfig(
	const Cfg_Tree_Entry *entry,
	String & output, 
	const String & indent, 
	int indent_size) const;

    /** Terminal command callback
     */
    virtual void DoTermCmd(Term_Session *, bool no, Term_Cmd *);

private:
    static RTP_Signalling *rtpSignallingInstance;
    Q931_retransmit_rec retrConfig, retrRun;
    unsigned short serverPort;

    RTP_Signalling();

    /* Notifications from Q.931 */
    static void q931_OnSetupInd (Q931_call_t call, Q931_msg_rec *msg);
    static void q931_OnMoreInfoInd (Q931_call_t call, Q931_msg_rec *msg);
    static void q931_OnInfoInd (Q931_call_t call, Q931_msg_rec *msg);
    static void q931_OnProceedingInd (Q931_call_t call, Q931_msg_rec *msg);
    static void q931_OnAlertingInd (Q931_call_t call, Q931_msg_rec *msg);
    static void q931_OnProgressInd (Q931_call_t call, Q931_msg_rec *msg);
    static void q931_OnConnectInd (Q931_call_t call, Q931_msg_rec *msg);
    static void q931_OnConnectAckInd (Q931_call_t call, Q931_msg_rec *msg);
    static void q931_OnDisconnectInd (Q931_call_t call, Q931_msg_rec *msg);
    static void q931_OnReleaseCompInd (Q931_call_t call, Q931_msg_rec *msg);
    static void q931_OnNop (Q931_call_t call);
    static void q931_OnCallClearedInd (Q931_call_t call);
    static void q931_OnCallSuspendedInd (Q931_call_t call);
    static void q931_OnCallResumedInd (Q931_call_t call);
    static void q931_OnNotifyMessage (Q931_call_t call, spt_stateevent_dir dir, Q931_msg_rec *msg);
};


extern Q931_session_rec  rtpQ931Session;


#endif	/* __XGATE_RTPSIG_H__ */
