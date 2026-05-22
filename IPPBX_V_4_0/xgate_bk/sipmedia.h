/* $Header: /xGate/sipmedia.h 7     5/21/01 12:12p Bennylp $
 *   
 * CTENGINE
 * (C) 2001 Switchlab, Ltd
 *
 * CTENGINE Session Initiation Protocol (SIP) media.
 *
 * $Log: /xGate/sipmedia.h $
 * 
 * 7     5/21/01 12:12p Bennylp
 * Rollback SIP changes to stable version.
 * 
 * 5     4/27/01 1:38a Bennylp
 * Implement switching.
 * 
 * 4     4/27/01 12:25a Bennylp
 * Change in SIP media negotiator.
 * 
 * 3     4/26/01 4:14a Bennylp
 * Splitting SIP signalling into SIP and XGATE call signalling.
 * 
 * 2     4/25/01 10:06a Bennylp
 * Continue media works.
 * 
 * 1     4/24/01 7:10a Bennylp
 * Initially created.
 *
 */
#ifndef __XGATE_SIP_MEDIA_H__
#define __XGATE_SIP_MEDIA_H__


#include "callmanager.h"
#include "MediaManager.h"
#include "sipneg.h"		// SipMediaNegotiator
#include "sipchannel.h"		// SipXxxChannel


/* Forward decl.
 */
class XGATE_SIP_Call;
class SIP_Media_Channel;
class SIP_Media_Interface_Manager;
class SwitchDevice;
typedef Interface SIP_Media_Interface;


///////////////////////////////////////////////////////////////////////////////
/** Type of address specified in SetupMedia() of SIP_Media_Channel.
 */
enum SIP_Media_Address_Type {
    /// The specified address is the signalling address.
    SIP_Media_Address_Type_Signalling,

    /// The specified address is the media address.
    SIP_Media_Address_Type_Media
};


///////////////////////////////////////////////////////////////////////////////
/** This class is the base class for all SIP media interfaces manager.
    The media interface manager is used by the SIP signalling to allocate a
    media channel for the incoming calls.
 */
class SIP_Media_Interface_Manager : public ServiceProvider {
public:
    /** Media type ("audio", "video", etc.)
     */
    const IString mediaType;

    /** Type of network that the media communication occurs ("IP4" for TCP/IP version 4).
     */
    const IString networkType;

    /** Addressing type ("IN" for TCP/IP).
     */
    const IString addressType;

    /** Transport type that carries the media communication ("RTP/AVP" for RTP).
     */
    const IString transportType;

    /** Get the priority of this interface type. Higher priority is more preferable
	to use. When the priority is zero, the interface will not be used.
     */
    unsigned GetPriority() const {
	return priority;
    }

    /** Allocate a media interface based on the information in the incoming call
	description.
     */
    virtual SIP_Media_Channel *AllocateSIPMedia (
	XGATE_SIP_Call *call,		    // the incoming call.
	const String & sig_addr,	    // source signalling address
	int sig_port,			    // source signalling port number
	const String & med_addr,	    // source media address
	int med_port			    // source media port number
    ) = 0;

protected:
    SIP_Media_Interface_Manager ( 
	const String & sp_name,
	const IString & media_type,
	const IString & network_type,
	const IString & addr_type,
	const IString & transport_type,
	bool attach_to_task,
	bool always_poll,
	ServiceProvider::Priority priority = ServiceProvider::Normal_Priority
    );

    /** Register this manager to the SIP signalling.
     */
    void RegisterInterfaceManager();

    /** Unregister this manager to the SIP signalling.
     */
    void UnregisterInterfaceManager();

private:
    unsigned priority;

};


///////////////////////////////////////////////////////////////////////////////
/** This class is the base class for all media channel that use SIP as the
    signalling protocol.
 */
class SIP_Media_Channel : public Media {
public:
    /** Media status.
     */
    enum Status {
	/// Null.
	StatusNull,

	/// Resolving remote media address.
	StatusResolving,

	/// Waiting for connection from remote.
	StatusWaitConnect,

	/// Connecting to remote endpoint
	StatusConnecting,

	/// Media is connected.
	StatusConnected,

	/// Error.
	StatusError
    };

    /** Destructor.
     */
    virtual ~SIP_Media_Channel();

    /** Get local address of the media channel.
     */
    virtual const String & GetLocalAddress() const = 0;

    /** Get local port of the media channel.
     */
    virtual unsigned GetLocalPort() const = 0;

    /** Get capability list.
     */
    virtual const List <SipCapability*> & GetCapabilityList () const = 0;

    /** Close connection.
     */
    virtual void CloseConnection() = 0;

    /** Setup the media.
     */
    Status SetupMedia(
	SIP_Media_Address_Type addr_type,
	const String & remote_addr, 
	unsigned remote_port
    );

    /** Start the media to the remote media address.
     */
    bool StartMedia(
	const String & remote_med_addr, 
	unsigned remote_med_port
    ) {
	if (OnStartMedia (remote_med_addr, remote_med_port)) {
	    mediaStatus = StatusConnected;
	    return true;
	}
	mediaStatus = StatusError;
	return false;
    }

    /** Get the media state.
     */
    Status GetStatus() const {
	return mediaStatus;
    }

    /** Get the parent SIP Interface manager.
     */
    const SIP_Media_Interface_Manager & GetSIPInterfaceManager() const {
	return *itfManager;
    }

    /** Get the parent SIP Interface manager.
     */
    SIP_Media_Interface_Manager & GetSIPInterfaceManager() {
	return *itfManager;
    }

    /** Get the media type ("audio", "video", etc.)
     */
    const IString & GetMediaType() const {
	return GetSIPInterfaceManager().mediaType;
    }

    /** Get the type of network that the media communication 
	occurs ("IN" for TCP/IP, "ATM" for proprierary ATM media network).
     */
    const IString & GetNetworkType() const {
	return GetSIPInterfaceManager().networkType;
    }

    /** Get the addressing type ("IP4" for TCP/IP version 4, "XNSAP for 
	proprietary ATM addressing").
     */
    const IString & GetAddressType() const {
	return GetSIPInterfaceManager().addressType;
    }

    /** Get transport type that carries the media communication 
	("RTP/AVP" for RTP, "XSVC" for proprietary ATM SVC transport).
     */
    const IString & GetTransportType() const {
	return GetSIPInterfaceManager().transportType;
    }

    /** Notifications from media's network connection that the media has been
	connected.
     */
    void OnMediaConnected();

    /** Notifications from media's network connection that the media has been
	disconnected.
     */
    void OnMediaDisconnected();

protected:
    /** Constructor.
     */
    SIP_Media_Channel (
	const String & name, 
	MediaId id,
	SIP_Media_Interface *itf,
	SIP_Media_Interface_Manager *mgr,
	SwitchDevice *switching_device
    );

    /** Setup the media.
     */
    virtual Status OnSetupMedia(
	SIP_Media_Address_Type addr_type,
	const String & remote_sig_addr, 
	unsigned remote_sig_port
    ) = 0;

    /** Start the media.
     */
    virtual bool OnStartMedia(
	const String & remote_media_addr, 
	unsigned remote_media_port
    ) = 0;

private:
    SIP_Media_Interface_Manager *itfManager;
    Status mediaStatus;
};


///////////////////////////////////////////////////////////////////////////////
/** This class describes media channel that is requried by the SIP signalling.
 */
class SIP_Internal_Media_Channel : public SipRtpBidirectionalChannel {
public:
    /** Constructor.
     */
    SIP_Internal_Media_Channel ( SIP_Media_Channel * media );

    /** Create the media channel. After the media channel is created, the local
        media address should be allocated or at least the local media address
	should be determined, because the GetLocalAddress() member will be
	called.

	This function will in turn call OnCreateChannel() member to actually
	create the channel.
      */
    virtual bool CreateChannel();

    /** Start the media channel. 
        This member will in turn call OnStartChannel() member to actually start
	the media channel.
      */
    virtual bool StartChannel(
	const String & remote_addr,
	unsigned short port,
	const SipCapability &capability
    );

    /** Stop the media channel. */
    virtual bool StopChannel();

    /** Get local media address.
        Get address and port which media stream will be sent/received.
      */
    virtual bool GetLocalAddress(String & addr, unsigned short & port) const;

private:
    SIP_Media_Channel *sipMedia;
};



///////////////////////////////////////////////////////////////////////////////
/** Media capabilities negotiator.
 */
class SIP_Media_Neg : public SipMediaNegotiator {
public:
    /** Constructor.
     */
    SIP_Media_Neg(SIP_Media_Channel *media_channel) 
    : mediaChannel(media_channel) {}

private:
    SIP_Media_Channel *mediaChannel;

    /** Get master capability list.
	The default behaviour is to get agent's capability list.
     */
    virtual const List<SipCapability*> &GetCapabilityList();

    /** Instantiate a SipCapability class based on parameters given. 
	@param dir		The direction of the media flows.
	@param med_type		The media type.
	@param transport_type	Transport mechanism used to transfer the media.
	@param payload_type	The RTP payload type.
	@param encoding_name	(Optional) The name of the codec to be instantiated.
	@param clock_rate	(Optional) The sampling clock rate.
	@param num_port		(Optional) Number of RTP port (always 1).
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
};


#endif	// __XGATE_SIP_MEDIA_H__

