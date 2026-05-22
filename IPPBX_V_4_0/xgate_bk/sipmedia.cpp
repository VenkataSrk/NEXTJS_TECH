/* $Header: /xGate/sipmedia.cpp 8     5/21/01 12:12p Bennylp $
 *   
 * CTENGINE
 * (C) 2001 Switchlab, Ltd
 *
 * CTENGINE Session Initiation Protocol (SIP) media.
 *
 * $Log: /xGate/sipmedia.cpp $
 * 
 * 8     5/21/01 12:12p Bennylp
 * Rollback SIP changes to stable version.
 * 
 * 6     4/27/01 1:38a Bennylp
 * Implement switching.
 * 
 * 5     4/27/01 12:25a Bennylp
 * Change in SIP media negotiator.
 * 
 * 4     4/26/01 4:14a Bennylp
 * Splitting SIP signalling into SIP and XGATE call signalling.
 * 
 * 3     4/25/01 10:06a Bennylp
 * Continue media works.
 * 
 * 2     4/24/01 7:22a Bennylp
 * Changed to new media interface that supports async.
 * 
 * 1     4/24/01 7:10a Bennylp
 * Initially created.
 *
 */
#include "stdafx.h"
#include "sipmedia.h"
#include "sipagent.h"
#include "sipcall.h"
#include "sipauth.h"
#include "sip.h"
#include "sipnetcoder.h"
#include "sipacm.h"
#include "sipneg.h"
#include "sipchannel.h"
#include "log.h"
#include "aculabswitch.h"
//#include "./ipcall/nullrtp.h"
#include "nullrtp.h"

#define THISMODULE	"SIP"


///////////////////////////////////////////////////////////////////////////////

SIP_Media_Interface_Manager::SIP_Media_Interface_Manager ( const String & name,
							   const IString & media_type,
							   const IString & network_type,
							   const IString & addr_type,
							   const IString & transport_type,
							   bool attach_to_task,
							   bool always_poll,
							   ServiceProvider::Priority priority)
: ServiceProvider(name, attach_to_task, always_poll, priority),
  mediaType(media_type), networkType(network_type), addressType(addr_type),
  transportType(transport_type)
{
}

/** Register this manager to the SIP signalling.
 */
void SIP_Media_Interface_Manager::RegisterInterfaceManager()
{
#ifndef __linux__
    SIP_Signalling::Instance()->RegisterInterfaceManager(this);
#endif
}

/** Unregister this manager to the SIP signalling.
 */
void SIP_Media_Interface_Manager::UnregisterInterfaceManager()
{
#ifndef __linux__
    SIP_Signalling::Instance()->UnregisterInterfaceManager(this);
#endif
}


///////////////////////////////////////////////////////////////////////////////
/** SIP_Media_Channel constructor.
 */
SIP_Media_Channel::SIP_Media_Channel ( const String & name, 
				       MediaId id,
				       SIP_Media_Interface *itf,
				       SIP_Media_Interface_Manager *mgr,
				       SwitchDevice *switching_device)
: Media(name.c_str(), id, itf, switching_device), itfManager(mgr), 
  mediaStatus(StatusNull)
{
}

/** SIP_Media_Channel Destructor.
 */
SIP_Media_Channel::~SIP_Media_Channel()
{
}


/** Setup the media.
     */
SIP_Media_Channel::Status SIP_Media_Channel::SetupMedia(SIP_Media_Address_Type addr_type,
							const String & remote_addr, 
							unsigned remote_port)
{
    mediaStatus = OnSetupMedia(addr_type, remote_addr, remote_port);
    return mediaStatus;
}


/** Notifications from media's network connection that the media has been
    connected.
 */
void SIP_Media_Channel::OnMediaConnected()
{
    mediaStatus = StatusConnected;
    XGATE_SIP_Call *sip_call = (XGATE_SIP_Call*) GetCall();

    // right now, we don't support orphan media.
    // so just throw an error.
    assert (sip_call != NULL);
    if (sip_call == NULL) {
	CLog::Error ( THISMODULE, "Media is connected, but the call has been cleared");
	return;
    }

    // notify the call.
#ifndef __linux__
    sip_call->OnMediaStatus();
#endif
}

/** Notifications from media's network connection that the media has been
    disconnected.
 */
void SIP_Media_Channel::OnMediaDisconnected()
{
    mediaStatus = StatusError;
    XGATE_SIP_Call *sip_call = (XGATE_SIP_Call*) GetCall();

    // right now, we don't support orphan media.
    // so just throw an error.
    assert (sip_call != NULL);
    if (sip_call == NULL) {
	CLog::Error ( THISMODULE, "Media is connected, but the call has been cleared");
	return;
    }
#ifndef __linux__
    // notify the call.
    sip_call->OnMediaStatus();
#endif
}


//-----------------------------------------------------------------------------
/** Constructor.
 */
SIP_Internal_Media_Channel::SIP_Internal_Media_Channel ( SIP_Media_Channel * media )
: sipMedia(media)
{
}

//-----------------------------------------------------------------------------
/** Create the media channel. After the media channel is created, the local
    media address should be allocated or at least the local media address
    should be determined, because the GetLocalAddress() member will be
    called.
  */
bool SIP_Internal_Media_Channel::CreateChannel()
{
    return sipMedia != NULL;
}

//-----------------------------------------------------------------------------
/** Start the media channel. 
    This member will in turn call OnStartChannel() member to actually start
    the media channel.
  */
bool SIP_Internal_Media_Channel::StartChannel( const String & remote_addr,
					       unsigned short port,
					       const SipCapability &capability )
{
    assert (sipMedia != NULL);
    if (sipMedia == NULL)
	return false;

    return sipMedia->StartMedia (remote_addr, port);
}

//-----------------------------------------------------------------------------
/** Stop the media channel. */
bool SIP_Internal_Media_Channel::StopChannel()
{
    if (sipMedia)
	sipMedia->CloseConnection();
    return true;
}

//-----------------------------------------------------------------------------
/** Get local media address.
    Get address and port which media stream will be sent/received.
  */
bool SIP_Internal_Media_Channel::GetLocalAddress(String & addr, unsigned short & port) const
{
    addr = sipMedia->GetLocalAddress();
    port = sipMedia->GetLocalPort();
    return true;
}


///////////////////////////////////////////////////////////////////////////////


static List<SipCapability*> empty;

/** Get master capability list.
    The default behaviour is to get agent's capability list.
 */
const List<SipCapability*>& SIP_Media_Neg::GetCapabilityList()
{
    assert (mediaChannel);
    if (mediaChannel)
	return mediaChannel->GetCapabilityList();
    return empty;
}

/** Instantiate a SipCapability class based on parameters given. 
 */
SipCapability * SIP_Media_Neg::CreateCapability (   SipCapability::Direction dir,
						    const IString & med_type,
						    const IString & transport_type,
						    unsigned payload_type,
						    const IString & encoding_name,
						    int clock_rate,
						    int num_port) const
{
    assert (mediaChannel);
    if (mediaChannel == NULL)
	return NULL;

    SipCapability * new_cap = NULL;
 
    // Find a match in capabilities in the media.
    const List <SipCapability*>& caps = mediaChannel->GetCapabilityList();
    List <SipCapability*>::const_iterator it, end;
    end = caps.end();
    for (it=caps.begin(); it!=end; ++it) {
	const SipCapability *cap = *it;
	if (cap->GetCapabilityType() == med_type &&
	    cap->GetTransportType() == transport_type &&
	    cap->GetPayloadType() == payload_type &&
	    cap->GetClockRate() == (unsigned)clock_rate) {

	    new_cap = cap->Clone();
	    new_cap->SetDirection(dir);
	    break;
	}
    }

    if (new_cap==NULL) {
	CLog::Info (THISMODULE, "Unable to create capabilility: %s (pt:%u)", 
				encoding_name.c_str(), 
				payload_type);
    }

    return new_cap;
}
