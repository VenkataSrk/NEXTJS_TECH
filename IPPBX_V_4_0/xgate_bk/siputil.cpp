/* $Header: /xGate/siputil.cpp 16    8/03/04 7:30p Bennylp $
 *   
 * CTENGINE
 * (C) 2001 Switchlab, Ltd
 *
 * SIP Utility.
 *
 * $Log: /xGate/siputil.cpp $
 * 
 * 16    8/03/04 7:30p Bennylp
 * Fix printing %3a if CLI/DDI contains ':' character (password)
 * 
 * 15    3/16/04 10:56a Bennylp
 * Added mapping Bearer Capability Unauthorized <--> SIP Unauthorized
 * 
 * 14    2/25/02 6:08p Bennylp
 * Fixed bug in SIP DDI/CLI: colon character
 * 
 * 13    11/14/01 1:12p Bennylp
 * Set username/passwd in CLI/DDI, host/port in signal addr
 * 
 * 12    7/11/01 19:35 Bennylp
 * Added status code mapping for CC_NormalUnspecified
 * 
 * 11    8/28/01 7:57p Bennylp
 * Option to include/exclide H.323, SIP, and ATM in compilation.
 * 
 * 10    8/28/01 2:00p Bennylp
 * Fixed host:port parsing
 * 
 * 9     8/09/01 6:12p Bennylp
 * Changed DispatchIncomingCall interface.
 * 
 * 8     8/09/01 4:08p Bennylp
 * Added more call causes.
 * 
 * 7     8/08/01 1:48p Bennylp
 * Temporary checkin
 * 
 * 6     5/21/01 12:12p Bennylp
 * Rollback SIP changes to stable version.
 * 
 * 3     5/01/01 10:46p Bennylp
 * Fixed bug in call parameter construction from SipUrl.
 * 
 * 2     4/26/01 5:29a Bennylp
 * Include in compilation.
 * 
 * 1     4/24/01 7:10a Bennylp
 * Initially created.
 *
 */
#include "xgconfig.h"
#include "stdafx.h"
//#include "../slsip2/include/siputil.h"//DG added
//#include "../slsip2/include/sipparser.h"//DG added
#include "siputil.h"//DG added
#include "sipparser.h"//DG added

//-----------------------------------------------------------------------------
/** Parse token[:token]
 */
void ParseUserPassword (const String & src, String & token1, String & token2)
{
    char *err = NULL;

	String url = StringManip::Encode(src,PCSX_TOKEN);//DG added

    String::size_type pos = url.find(':');
    if (pos == String::npos) {
	token1 = url;
	token2 = "";
    } else {
	token1 = url.substr(0, pos);
	token2 = url.substr(pos+1);
    }
}


//-----------------------------------------------------------------------------
/** Parse host:port string to host and port.
 */
bool ParseHostPort (const String & src, String & host, unsigned short & port)
{
    char *err = NULL;

    String::size_type pos = src.find(':');
    if (pos == String::npos) {
	host = src;
    } else {
	host = src.substr(0, pos);
	String sport = src.substr(pos+1);
	unsigned short temp_port = (unsigned short) strtol(sport.c_str(),&err, 10);
	if (*err == '\0') {
	    err = NULL;
	    port = temp_port;
	} 
    }
    return err==NULL;
}


#ifdef XGATE_HAS_SIP

#include "log.h"
#include "sip.h"	    
#include "sipagent.h"

#define THISMODULE  "SIP"

//-----------------------------------------------------------------------------
/** Create SIP URL from CallParameter structure.
 */
void CallParam2Url(CallParameter *param, SipNameAddress & src, SipNameAddress &dest, List <SipHeader *> &additional_headers)
{
    if (ParseHostPort (param->csaCalled, dest.host, dest.port) == false)
	CLog::Warning ( THISMODULE, "Invalid destination signal address format in call parameter: '%s'",
				    param->csaCalled.c_str());

    //dest.SetUser (param->cnCalled);
    // Convert phone number to International format if necessary (3GPP TS 29.163)
    String user;
    ParseUserPassword(param->cnCalled, user, dest.password);
    if (param->cnpCalled == CNP_ISDN) {
	//dest.SetParameter("user", "phone");

	/** Commented out temporarily due to backward compatibility issue
	if (param->cntCalled == CNT_International && user.length()>0 && user.substr(0, 1) != "+")
	    user = "+" + user;
	**/
    }

    dest.SetUser (user);
    dest.SetDisplayName (user);
    
    //src.SetUser (param->cnCalling);
    // Convert phone number to International format if necessary (3GPP TS 29.163)
    ParseUserPassword(param->cnCalling, user, src.password);
    if (param->cnpCalling == CNP_ISDN) {

	if (user == "") {
	    src.SetUser("unavailable");    // unavailable@unknown.invalid
	    src.SetHost("unknown.invalid");
	    src.SetDisplayName("Unavailable");
	} else {
	    /** Commented out temporarily due to backward compatibility issue
	    if (param->cntCalling == CNT_International && user.length()>0 && user.substr(0, 1) != "+")
		user = "+" + user;
	    **/

	    if (param->ccp == CCP_Restricted) {
		src.SetUser("anonymous");	// anonymous@anonymous.invalid
		src.SetHost("anonymous.invalid");
		src.SetDisplayName("Anonymous");

		// Add P-Asserted-Identity header
		SipPAssertedIdentity *pai = new SipPAssertedIdentity();
		pai->address.SetUser(user);
		pai->address.SetDisplayName(user);
		pai->address.SetHost(SipAgent::Instance()->GetActiveInterface());
		pai->address.SetParameter("user", "phone");
		additional_headers.push_back(pai);

		// Add Privacy header
		SipPrivacy* priv = new SipPrivacy;
		priv->privacy = "id";
		additional_headers.push_back(priv);
	    } else {
		src.SetUser(user);
		src.SetDisplayName(user);
		src.SetHost (SipAgent::Instance()->GetActiveInterface());

		// Add P-Asserted-Identity header
		SipPAssertedIdentity *pai = new SipPAssertedIdentity();
		pai->address.SetUser(user);
		pai->address.SetDisplayName(user);
		pai->address.SetHost(SipAgent::Instance()->GetActiveInterface());
		pai->address.SetParameter("user", "phone");
		additional_headers.push_back(pai);
	    }
	}
	
    } else {
	src.SetUser(user);
	src.SetDisplayName(user);
	src.SetHost (SipAgent::Instance()->GetActiveInterface());
    }

    if (param->cnRedirectingNumber != "") {
	SipHistoryInfo *phi = new SipHistoryInfo;
	phi->address.SetUser(param->cnRedirectingNumber);
	phi->address.SetHost(SipAgent::Instance()->GetActiveInterface());
	phi->address.SetParameter("index", "1");
	additional_headers.push_back(phi);
    }
}

bool ParseTelephoneNumber(const char* inp, CallNumber& o_number, CallNumberType& o_type)
{
    char c;
    const char* s = inp;

    if (*s == '+') {
	o_type = CNT_International;
	s++;
    } else  {
	o_type = CNT_Unknown;
    }

    while ((c = tolower(*s++)) != 0) {
	// copy all digits
	if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'D') || (c == '*') || (c == '#'))
	    o_number += c;

	// skip visual
	else if ((c == '-') || (c == '.') || (c == '(') || (c == ')'))
	    continue;

	// password delimiter
	else if (c == ':') {
	    o_number += s;
	    return true;

	// invalid character, this is not a telephone number
	} else {
	    o_number = inp;
	    o_type = CNT_Unknown;
	    return false;
	}
    }

    return true;
}

void CreateCallParam ( CallParameter & cparam, const SipRequestMessage & invite)
{
    ////XgSipSignalling::Instance()->InitDefaultCallParameter(cparam); // commented for ippbx

    char s[128];
    const SipUrl & url = invite.requestLine.requestUri;

    // Called Signal Address
    snprintf (s, sizeof(s), "%s:%u", url.GetHost().c_str(), url.GetPort());
    cparam.csaCalled = s;

    // Called Number Address & Type
    if (url.password.size()) {
	snprintf (s, sizeof(s), "%s:%s", url.GetUser().c_str(), url.password.c_str());
    } else
	snprintf (s, sizeof(s), "%s", url.GetUser().c_str());
    if (ParseTelephoneNumber(s, cparam.cnCalled, cparam.cntCalled))
	cparam.cnpCalled = CNP_ISDN;

    // Calling Signal Address
    const SipUrl & from = invite.GetHeaders().From()->address;
    snprintf (s, sizeof(s), "%s:%u", from.GetHost().c_str(), from.GetPort());
    cparam.csaCalling = s;

    // Derive CLI from P-Asserted-Identity & Privacy headers if possible
    SipPAssertedIdentity* sip_pai = (SipPAssertedIdentity*)invite.GetHeaders().FindHeader(SipHeader::H_PAssertedIdentity);
    if (sip_pai) {
	if (ParseTelephoneNumber(sip_pai->address.GetUser().c_str(), cparam.cnCalling, cparam.cntCalling))
	    cparam.cnpCalling = CNP_ISDN;

	SipPrivacy* sip_privacy = (SipPrivacy*)invite.GetHeaders().FindHeader(SipHeader::H_Privacy);
	if (sip_privacy) {
	    cparam.ccp = (sip_privacy->privacy == "none") ? CCP_Allowed : CCP_Restricted;
	} else {
	    cparam.ccp = CCP_Allowed;
	}

    // Otherwise derive CLI from From header
    } else {
	if (from.GetUser() == "unavailable") {
	    cparam.cnpCalling = CNP_ISDN;
	    cparam.cntCalling = CNT_Unknown;
	    cparam.cnCalling = "";
	    cparam.ccp = CCP_NotAvailable;
	} else if (from.GetUser() == "anonymous") {
	    cparam.cnpCalling = CNP_ISDN;
	    cparam.cntCalling = CNT_Unknown;
	    cparam.cnCalling = "";
	    cparam.ccp = CCP_Restricted;
	} else {
	    if (from.password.size()) 
		snprintf (s, sizeof(s), "%s:%s", from.GetUser().c_str(), from.password.c_str());
	    else
		snprintf (s, sizeof(s), "%s", from.GetUser().c_str());
	    if (ParseTelephoneNumber(s, cparam.cnCalling, cparam.cntCalling))
		cparam.cnpCalling = CNP_ISDN;
	}
    }

    SipHistoryInfo *sip_hi = (SipHistoryInfo*)invite.GetHeaders().FindHeader(SipHeader::H_HistoryInfo);
    if (sip_hi) {

	if (ParseTelephoneNumber(sip_hi->address.GetUser().c_str(), cparam.cnOriginalCalledNumber, cparam.cntOriginalCalledNumber))
	    cparam.cnpOriginalCalledNumber = CNP_ISDN;
	else cparam.cnpOriginalCalledNumber = cparam.cnpCalling;

	// copy OCN to RN
	cparam.cnRedirectingNumber = cparam.cnOriginalCalledNumber;
	cparam.cntRedirectingNumber = cparam.cntOriginalCalledNumber;
	cparam.cnpRedirectingNumber = cparam.cnpOriginalCalledNumber;

	// set other redirection parameters
	cparam.redirectionIndicator = 3;
	cparam.redirectionOrigReason = 0;
	cparam.redirectionReason = 0;
	cparam.redirectionCounter = 1;
    }
}

//-----------------------------------------------------------------------------
/** This class converts from/to CTENGINE/SIP cause codes.
 */
SIP_CTENGINE_Cause_Mapping sip_ctengine_cause_mapping;


//-----------------------------------------------------------------------------
/** Constructor of SIP_CTENGINE_Cause_Mapping class.
 */
SIP_CTENGINE_Cause_Mapping::SIP_CTENGINE_Cause_Mapping()
{
    InsertMapping ( CC_UnallocatedNumber,	    scGone,			"Unallocated number");

    InsertMapping ( CC_DestinationOutOfOrder,	    scNotFound,			"Destination out of order");
    InsertMapping ( CC_NoRouteToTransitNetwork,	    scNotFound,			"No route to transit network");
    InsertMapping ( CC_NoRouteToDestination,	    scNotFound,			"No route to destination");
    InsertMapping ( CC_NormalUnspecified,	    scNotFound,			"Normal/unspecified");

    InsertMapping ( CC_NormalCallClearing,	    scDecline,			"Normal call clearing");

    InsertMapping ( CC_UserBusy,		    scBusyHere,			"User busy");

    InsertMapping ( CC_NoUserResponding,	    scTemporarilyNotAvailable,	"No user responding");
    InsertMapping ( CC_NoAnswerFromUser,	    scTemporarilyNotAvailable,	"No answer from user");

    InsertMapping ( CC_CallRejected,		    scDecline,			"Call rejected");

    InsertMapping ( CC_InvalidMessageUnspecified,   scBadRequest,		"Invalid message unspecified");
    InsertMapping ( CC_NumberChanged,		    scMovedPermanently,		"Number changed");

    InsertMapping ( CC_InvalidNumberFormat,	    scAddressIncomplete,	"Invalid number format");

    InsertMapping ( CC_TemporaryFailure,	    scServiceUnavailable,	"Temporary failure");
    InsertMapping ( CC_NoCircuitAvailable,	    scServiceUnavailable,	"No circuit available");
    InsertMapping ( CC_NetworkOutOfOrder,	    scServiceUnavailable,	"Network out of order");
    InsertMapping ( CC_SwitchingEquipmentCongestion,scServiceUnavailable,	"Switching equipment congestion");
    InsertMapping ( CC_RequestedCircuitUnavailable, scServiceUnavailable,	"Requested circuit not available");
    InsertMapping ( CC_ResourceUnavailable,	    scServiceUnavailable,	"Resource unavailable");
    InsertMapping ( CC_QOSUnavailable,		    scServiceUnavailable,	"QOS unavailable");
    InsertMapping ( CC_RequestedFacilityUnsubscribed,scServiceUnavailable,	"Requested facility unsubscribed"); //
    InsertMapping ( CC_BearerCapabilityUnauthorized,scUnauthorized,		"Bearer capability not presently authorized");
    InsertMapping ( CC_BearerCapabilityUnavailable, scServiceUnavailable,	"Bearer capability not available");
    InsertMapping ( CC_OptionUnavailable,	    scServiceUnavailable,	"Service/option not available");

    InsertMapping ( CC_BearerCapabilityUnimplemented,scNotImplemented,		"Bearer capability not implemented");
    InsertMapping ( CC_RequestedFacilityUnimplemented,scNotImplemented,		"Requested facility unimplemented"); //
    InsertMapping ( CC_ServiceOrOptionNotImplementedUnspecified, scNotImplemented, "Requested facility unimplemented"); //

    InsertMapping ( CC_CallHasBeenCleared,	    scCallDoesNotExist,		"Call has been cleared"); //

    InsertMapping ( CC_IncompatibleDestination,	    scNotAcceptable,		"Incompatible destination");
    InsertMapping ( CC_ChannelUnacceptable,	    scNotAcceptable,		"Channel not acceptable");	//
    InsertMapping ( CC_MissingParameter,	    scNotAcceptable,		"Missing parameter");	//
    InsertMapping ( CC_InvalidParameter,	    scNotAcceptable,		"Invalid parameter"); //
    InsertMapping ( CC_MissingInformation,	    scNotAcceptable,		"Missing information");	//

    InsertMapping ( CC_RecoveryOnTimeout,	    scRequestTimeout,		"Recovery on timer expiry"); 

    InsertMapping ( CC_ProtocolError,		    scInternalServerError,	"Protocol error"); 
    InsertMapping ( CC_ChannelNonExistent,	    scInternalServerError,	"Channel non existent"); //
    InsertMapping ( CC_NoCallSuspended,		    scInternalServerError,	"No call suspended"); //
    InsertMapping ( CC_Interworking,		    scInternalServerError,	"Interworking unspecified"); 

}


SipStatusCode SIP_CTENGINE_Cause_Mapping::GetSipStatus (CallCause cause, String & reason) const
{
    static String default_unknown_reason = "Internal server error";

    CTENGINE_2_SIP_T::const_iterator it;
    it = ctengineMapping.find(cause);
    //assert (it != ctengineMapping.end());
    if (it != ctengineMapping.end()) {
	reason = it->second.second;
	return it->second.first;
    }

    reason = default_unknown_reason;
    return scInternalServerError;
}


CallCause SIP_CTENGINE_Cause_Mapping::GetCtengineCause (SipStatusCode sip_code, const String & reason) const
{
    // find out whether there's an ISUP code in the reason.
    unsigned cause = 0;

    const char *s = reason.c_str();
    const char *pos;
    if ((pos = strstr(s, "(code:")) != NULL) {
	pos += 6;
	while (*pos && isdigit(*pos)) {
	    int c = (*pos - '0') & 0xFF;
	    cause *= 10;
	    cause += c;
	    ++pos;
	}
    }
    if (cause != 0)
	return (CallCause) cause;
    
    // if the ISUP code is not in the reason, then find the compatible ISUP 
    // cause from the table.
    SIP_2_CTENGINE_T::const_iterator it;
    it = sipMapping.find (sip_code);
    if (it == sipMapping.end()) {
	return CC_NormalUnspecified;
    }
    return it->second;
}

#endif	// XGATE_HAS_SIP

