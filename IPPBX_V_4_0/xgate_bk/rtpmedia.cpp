/* $Header: /xGate/rtpmedia.cpp 79    5/19/05 7:13p Bennylp $
 *   
 * XGATE
 * (C) 2001 Switchlab, Ltd
 *
 * RTP virtual media.
 *
 * $Log: /xGate/rtpmedia.cpp $
 * 
 * 79    5/19/05 7:13p Bennylp
 * Don't ignore audiocodes error
 * 
 * 78    5/19/05 3:34p Bennylp
 * Don't call OnDisconnectInd in OnMediaError if state has changed
 * 
 * 77    3/18/05 6:37p Bennylp
 * Version 60.7
 * 
 * 76    3/08/04 7:12p Bennylp
 * Initial support for RTP virtual channel
 * 
 * 75    6/10/03 3:07p Bennylp
 * Set releasing period to 500 ms instead of 200 or 1000
 * 
 * 74    6/06/03 4:07p Bennylp
 * Lots of changes to incorporate mapletree diagnostic (Version 54.4)
 * 
 * 73    6/04/03 3:46p Bennylp
 * Fixed bug when terminating channel is not re-tried.
 * 
 * 72    4/10/03 11:38a Bennylp
 * Fix compilation error when MTN is excluded in xgconfig.h
 * 
 * 71    4/09/03 2:59p Bennylp
 * Don't set port state to IDLE if it is currently terminating
 * 
 * 70    4/09/03 11:07a Bennylp
 * Fixed memory leak and add interface type
 * 
 * 69    12/24/02 5:04p Bennylp
 * Enable round-robin and change spec during runtime.
 * 
 * 68    11/04/02 7:02p Bennylp
 * Show resource name in switch device.
 * 
 * 67    11/01/02 7:58p Bennylp
 * Bug fixes (47.1)
 * 
 * 66    10/29/02 3:52p Bennylp
 * Fixed compilation error when RTP is not used in xgconfig.h
 * 
 * 65    10/29/02 1:05p Bennylp
 * Stable version for MTN2100/1024P, tested.
 * 
 * 64    10/24/02 8:34p Bennylp
 * Latest stable checkin.
 * 
 * 63    9/25/02 5:12p Bennylp
 * - Fixed RTCP and quality monitoring bugs
 * - Improved timing of the RTP packet dispatching
 * 
 * 62    9/23/02 2:46p Bennylp
 * Changed macro USE_NEW_RTP to xgconfig.h
 * 
 * 61    9/18/02 11:57a Bennylp
 * Fixed media count error
 * 
 * 60    4/09/02 12:04 Adham
 * Asynch channel termination
 * 
 * 59    21/08/02 12:30 Adham
* Added asynch mtn channel termination
* 
* 58    5/08/02 18:55 Adham
* Added changes to support new rtp framework, updates is marked by #ifdef
* XGATE_USE_NEW_RTP pre-processor, this switch resides in rtpchan.h
* 
* 57    10/07/02 20:04 Adham
* Added code to support new RTP, the code can be disabled using
* XGATE_USE_NEW_RTP define on rtpchan.h
* 
* 56    5/22/02 12:46p Bennylp
* Set the src addr for ACL from socket addr rather than from cparam.
* 
  * 55    2/22/02 12:39p Bennylp
* Disable TDM transmit on Open()
  * 
  * 54    11/02/02 16:46 Sonny
  * q-interval -> stat-interval
  * 
  * 53    11/02/02 16:30 Sonny
  * quality monitoring updates
  * 
  * 52    6/02/02 19:01 Sonny
  * optimizations for quality monitoring
  * 
  * 51    4/02/02 12:33 Sonny
  * calls get quality from media
  * 
  * 50    1/02/02 12:20 Sonny
  * statistics monitor updates
  * 
  * 49    31/01/02 13:12 Sonny
  * add call quality monitoring
  * 
  * 48    24/01/02 20:58 Sonny
  * Quality monitoring support
  * 
  * 47    12/17/01 7:03p Bennylp
  * Compilation bug: unresolved link if SIP is excluded from project
  * 
  * 46    13/12/01 11:50 Bennylp
  * Fixed bug in G.723.1 low bitrate implementation
  * 
  * 45    12/11/01 5:05p Bennylp
  * Changed to support digest authentication in xGate
  * 
  * 44    11/06/01 7:53p Bennylp
  * Change RTP media initialization to two stages: Open and Activate
  * 
  * 43    10/30/01 4:43p Bennylp
  * Fixed error: task is not notified when AudioCodes gets error
  * 
  * 42    10/09/01 9:19p Bennylp
  * Fixed some crashes bugs.
  * 
  * 41    8/28/01 7:57p Bennylp
  * Option to include/exclide H.323, SIP, and ATM in compilation.
  * 
  * 40    8/28/01 7:16p Adham
  * Option to exclude/include H.323 from compilation
  * 
  * 39    8/27/01 12:55p Adham
  * 
  * 38    8/24/01 4:58p Adham
  * 
  * 37    8/21/01 6:27p Adham
  * Adding H323 Signalling 
  * 
  * 36    8/20/01 4:26p Bennylp
  * RTP codec negotiation
  * 
  * 35    8/13/01 2:50p Bennylp
  * Fixed RTCP stat calculations, do not include bad values.
  * 
  * 34    8/13/01 11:38a Bennylp
  * Added RTCP statistics monitoring.
  * 
  * 33    8/09/01 6:12p Bennylp
  * Changed DispatchIncomingCall interface.
  * 
  * 32    8/08/01 7:00p Bennylp
  * Added DialDTMF().
  * 
  * 31    8/08/01 5:29p Bennylp
  * Supports SIP signalling.
  * 
  * 30    8/08/01 1:48p Bennylp
  * Temporary checkin
  * 
  * 29    8/02/01 7:57p Bennylp
  * RTP interface now uses access list.
  * 
  * 28    8/01/01 7:00p Bennylp
  * AudioCodes will loopback to itself again.
  * 
  * 27    7/31/01 6:52p Bennylp
  * Barebones for AudioCodes voice driver.
  * 
  * 26    7/30/01 10:20p Bennylp
  * Disconnect call in case of Destination Unreachable.
  * 
  * 25    28/07/01 21:28 Bennylp
  * Turns out that AllocateAddress is always called.
  * 
  * 24    7/28/01 8:40p Bennylp
  * AudioCodes will pre-allocate switch addresses.
  * 
  * 23    7/28/01 11:23a Bennylp
  * StartTransmit/Receive: do nothing if told to tx/rx to the same addr.
  * 
  * 22    7/27/01 11:04a Bennylp
  * Change switch signalling; rtp is activated on first call to switching.
  * 
  * 21    7/20/01 9:08p Bennylp
  * Fixed bug: rtp media is not deleted!
  * 
  * 20    7/20/01 6:19p Bennylp
  * Restore voice volume settings in StartTransmit().
  * 
  * 19    7/20/01 5:29p Bennylp
  * Remove assertion in allocate address.
  * 
  * 18    7/20/01 4:31p Bennylp
  * Changed AudioCodes TDM handling (don't use park address).
  * 
  * 17    7/19/01 7:00p Bennylp
  * Show board identifier in rtp media name.
  * 
  * 16    7/19/01 6:18p Bennylp
  * Interface source will be used in round robin fashion.
  * 
  * 15    7/19/01 4:59p Bennylp
  * RTP interface can be deleted.
  * 
  * 14    7/19/01 3:40p Bennylp
  * Use audiocodes interface in round-robin.
  * 
  * 13    7/19/01 2:18p Bennylp
  * Changes to MediaManager functions to support rtp/aud shared stats.
  * 
  * 12    7/18/01 10:26p Bennylp
  * AudioCodes interface can be shutdown at runtime.
  * 
  * 11    7/17/01 8:34p Bennylp
  * Fixed bug: media count is not updated when itf is deleted.
  * 
  * 10    7/17/01 6:42p Bennylp
  * Fixed description command.
  * 
  * 9     7/17/01 11:39a Bennylp
  * Adhere to enable/disable flag.
  * 
  * 8     7/13/01 4:19p Bennylp
  * Fixed bug: 'accept 0.0.0.0' is always created.
  * 
  * 7     7/12/01 1:33p Bennylp
  * Added TDM switching logging info.
  * 
  * 6     6/30/01 11:36p Bennylp
  * Misc. bug fixes and improvement (see rel notes in ver. 32).
  * 
  * 5     6/29/01 12:30p Bennylp
  * Added owner info in SCbusManager::AllocateAddress.
  * 
  * 4     6/27/01 10:37p Bennylp
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

  //#include "stdafx.h"
#include "xgconfig.h"

#ifdef XGATE_HAS_AUDIOCODES

#include <poll.h>
#ifdef XGATE_HAS_H323
#include "h323sig.h"
#undef OK
#endif

#include "rtpmedia.h"
#include "term.h"
#include "rtpchan.h"
#ifdef XGATE_HAS_RTP_SIGNALLING
#include "rtpsig.h"
#endif

#include "kernel.h"
#include "accesslist.h"
#include "rtpvirtual.h"

#ifdef XGATE_HAS_SIP
#include "sip.h"
#include "sipagent.h"
#include "sipauth.h"
#include "sipcall.h"
#else
#define DEFAULT_MAX_FORWARDS 20
#endif

#include "h248media.h"
//# include "xGateMgcMgMedia.h"
//# include "xGateUtil.h"
#ifdef XGATE_HAS_SOFIA
#include "sofia.h"
#endif

#include "siputil.h"

#ifdef XGATE_HAS_MTNRTP
#include "mtnrtp.h"
#endif

#ifdef XGATE_HAS_KALLASTRA
#   include "kallastra.h"
#endif

#ifdef XGATE_HAS_HMP
#   include "hostmedia.h"
#else
#include "prosody.h"
#endif

#define THISMODULE	"rtp"
#define ARRAY_SIZE(a)	(sizeof(a)/sizeof(a[0]))
#define AC_DEBUG_SWITCHING
#define FALSE 0
#define TRUE 1

#ifdef XGATE_HAS_ACME_SBC
unsigned int Rtp_Interface::m_selectedSbcIndex = 0;
#endif

  const char *ac_make_name (int id);
  const char *ac_make_chname (int boardid, int chid);

#define stricmp strcasecmp
#define strnicmp strncasecmp
#define strcmpi strcasecmp

  class H323_Signalling;

void Sleep( unsigned long ms )
{
#ifndef __linux__
  ::Sleep(ms);
#else
  poll(0,0,ms); // __linux__
#endif
};

static unsigned long GetTickCount()
{
  struct timeval tv;
  if( gettimeofday(&tv, NULL) != 0 )
    return 0;

  return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

  static inline MediaId 
ac_make_mediaid(AcChannel *ch)
{
  return (ch->GetBoard()->GetNumber() << 16) + ch->GetNumber();
}

  static inline const char *
rtp_make_chname(const char *rtp_itf, const char *board_itf, unsigned chid)
{
  static char temp[80];
  sprintf(temp, "%s:%s/%u", rtp_itf, board_itf, chid);
  return temp;
}

static String scramble_password(const String & input)
{
  char buf[10];
  String result;

  sprintf(buf, "%2x", input.size());
  result += buf;

}

////extern int gethostname (char *__name, size_t __len);
static const char *HostName()
{
    static char hostname[128];
    gethostname(hostname,sizeof(hostname)-1);
    return hostname;
}
/** Tool function to get the IP address from hostname/IP address string.
 */
static unsigned long get_ip_addr(const char *host)
{
    if (!*host)
        return 0xFFFFFFFF;

    unsigned long ip = inet_addr(host);
    if (ip!=INADDR_NONE)
        return ip;

    hostent *he = gethostbyname(host);
    if (!he)
        return 0xFFFFFFFF;
    return *(unsigned long*)he->h_addr;
}

static const char* getLocalInterface()
{
  CLog::Detail(THISMODULE, "IPPBX : inside getLocalInterface() ***************************\n");
  char hostname[32];
  //if (gethostname(hostname,sizeof(hostname) )==SOCKET_ERROR)
   // return "";
  gethostname(hostname,sizeof(hostname));

  unsigned long ipaddr = inet_addr(hostname);

  if (ipaddr == INADDR_NONE) {
    hostent* phe = gethostbyname(hostname);
    if (phe)
      ipaddr = *((unsigned long*)phe->h_addr);
  }
  CLog::Detail(THISMODULE, "local interface = %s\n", inet_ntoa(*((in_addr*)&ipaddr)));
  return inet_ntoa(*((in_addr*)&ipaddr));
}

#if 0
static const char* getLocalInterface()
{
  CLog::Detail(THISMODULE, "IPPBX : inside getLocalInterface() ***************************\n");
  char hostname[32];
  hostname = HostName();
  ////if (gethostname(hostname,sizeof(hostname) )==SOCKET_ERROR)
  //if (gethostname(hostname,sizeof(hostname) )==-1)
  if (gethostname(hostname,sizeof(hostname) )==-1)
    return "";

  unsigned long ipaddr = inet_addr(hostname);

  if (ipaddr == INADDR_NONE) {
    hostent* phe = gethostbyname(hostname);
    if (phe)
      ipaddr = *((unsigned long*)phe->h_addr);
  }
	char* temp = inet_ntoa(*((in_addr*)&ipaddr));
  CLog::Detail(THISMODULE, "local interface = %s\n", inet_ntoa(*((in_addr*)&ipaddr)));
  return inet_ntoa(*((in_addr*)&ipaddr));
}
#endif


enum {
  Rtp_Cmd_Description,
  Rtp_Cmd_Interface,
  Rtp_Cmd_Accept,
  Rtp_Cmd_AccessList,
  Rtp_Cmd_Mode,
  Rtp_Cmd_Signalling,
  Rtp_Cmd_Dtmf,
  Rtp_Cmd_Fax,
  Rtp_Cmd_Tdm,
  Rtp_Cmd_Use,
  Rtp_Cmd_Modem,
  Rtp_Cmd_Ip,
  Rtp_Cmd_Rtp,
  Rtp_Cmd_Shutdown,
  Rtp_Cmd_Rap,
  Rtp_Cmd_Group,
  Rtp_Cmd_Enable,
  Rtp_Cmd_Sip,
  Rtp_Cmd_QInterval,
  Rtp_Cmd_Board,
  Rtp_Cmd_EchoTail,
  Rtp_Cmd_Max_Forwards,
  Rtp_Cmd_Strip_Private_Info,
  Rtp_Cmd_ResourceType,
#ifdef XGATE_HAS_ACME_SBC
  Rtp_Cmd_Sbc_Address,
#endif
};

struct ac_item_t {
  int  id;
  int	 id2;
  const char *name;
  const char *formalName;
  const char *desc;
};


/* Codec mapping.
*/
static ac_item_t rtp_codec[] = {
  { RTP_PT_PCMU,	    acG711Mulaw,    "pcmu",	"PCMU", "G.711 Ulaw (PCM Ulaw) @ 64 Kbps"},
  { RTP_PT_G726,	    acG726_16,	    "g726",	"G726", "G.726 @ 16 Kbps"},
  { RTP_PT_GSM,	    acGSM610,	    "gsm-fr", "GSM",	"GSM Full Rate 06.10 @ 13 Kbps"},
  //    { RTP_PT_GSM,	    acGSM610MS,	    "gsm-ms",	"MS GSM @ 13 Kbps"},
  //{ RTP_PT_GSM,	    acGSM_EFR,	    "gsm-efr",	"MS Enhanced Full Rate"},
  { RTP_PT_G7231,	    acG723High,	    "g723",	"G723", "G.723.1 @ 6.3/5.3 Kbps"},
  //    { RTP_PT_G7231,	    acG723Low,	    "g723",	"G.723.1 @ 5.3 Kbps"},
  { RTP_PT_DVI4_8000,	    -1,		    "dvi8",	"DVI4", "DVI4 @ 8 Khz"},
  { RTP_PT_DVI4_16000,    -1,		    "dvi15", "DVI5", "DVI4 @ 16 Khz"},
  { RTP_PT_LPC,	    -1,		    "lpc",	"LPC", "LPC"},
  { RTP_PT_PCMA,	    acG711Alaw_64,  "pcma",	"PCMA", "G.711 Alaw (PCM Alaw) @ 64 Kbps"},
  { RTP_PT_G722,	    -1,		    "g722",	"G722", "G.722"},
  { RTP_PT_L16_44100_2,   -1,		    "l16.44.2", "L16",	"Raw 16 bit @ 44100 Hz stereo"},
  { RTP_PT_L16_44100_1,   -1,		    "l16.44.1",	"L16", "Raw 16 bit @ 44100 Hz mono"},
  { RTP_PT_QCELP,	    -1,		    "qcelp",  "QCELP",  "QCELP"},
  { RTP_PT_G728,	    acG728,	    "g728",	"G728", "G.728"},
  { RTP_PT_DVI4_11025,    -1,		    "dvi11", "DVI4",  "DVI4 @ 11025 Hz"},
  { RTP_PT_DVI4_22050,    -1,		    "dvi22", "DVI4",  "DVI4 @ 22050 Hz"},
  { RTP_PT_G729,	    acG729,	    "g729",	"G729", "G.729"},
  { RTP_PT_Netcoder_4,    acNetCoder_4_8, "nc4.8","NC4.8",	"Netcoder @ 4.8 Kbps"},
  { RTP_PT_Netcoder_5,    acNetCoder_5_6, "nc5.6","NC5.6",	"Netcoder @ 5.6 Kbps"},
  { RTP_PT_Netcoder_6,    acNetCoder_6_4, "nc6.4","NC6.4",	"Netcoder @ 6.4 Kbps"},
  { RTP_PT_Netcoder_7,    acNetCoder_7_2, "nc7.2","NC7.2",	"Netcoder @ 7.2 Kbps"},
  { RTP_PT_Netcoder_8,    acNetCoder_8,   "nc8.0","NC8.0",	"Netcoder @ 8 Kbps"},
  { RTP_PT_Netcoder_88,   acNetCoder_8_8, "nc8.8","NC8.8",	"Netcoder @ 8.8 Kbps"},
  { RTP_PT_Netcoder_9,    acNetCoder_9_6, "nc9.6","NC9.6",	"Netcoder @ 9.6 Kbps"},
  { 0, 0, NULL, NULL }
};


/* DTMF transfer settings.
*/
static ac_item_t ac_dtmftransfer[] = {
  // { acMuteDTMF}
  { acRelayDTMF,	acRelayDTMF,	    "rtp",   "RTP", "Transfer DTMF with proprietary RTP extension"},
  { acTransparentDTMF,acTransparentDTMF,  "inband", "inband", "Transfer DTMF in audio stream"},
  { acRelayDTMFOverRTP, acRelayDTMFOverRTP, "rfc2833", "RFC2833", "Transfer DTMF using RFC 2833" },
  { 0, 0, NULL, NULL}
};


/* Fax transfer settings.
*/
static ac_item_t ac_faxtransfer[] = {
  { acFaxModemTransparent, acFaxModemTransparent, "inband", "inband",   "Use current codec to transmit fax"},
  { acFaxModemRelay,	acFaxModemRelay,  "t38", "T38", "Switch to T.38 on fax detection"},
  { acFaxModemBypass, acFaxModemBypass, "g711", "G711", "Switch to G.711/pcmu on fax detection" },
  { 0, 0, NULL, NULL}
};



/* Fax/modem relay rate.
*/
static ac_item_t ac_rate[] = {
  { acRate2400bps,	acRate2400bps,	"2400bps", "", "2400 bps"},
  { acRate4800bps,	acRate4800bps,  "4800bps", "", "4800 bps"},
  { acRate7200bps,	acRate7200bps,	"7200bps", "", "7200 bps"},
  { acRate9600bps,	acRate9600bps,	"9600bps", "", "9600 bps"},
  { acRate12000bps,	acRate12000bps,	"12000bps","", "12000 bps" },
  { acRate14400bps,	acRate14400bps,	"14400bps", "","14400 bps" },
  { 0, 0, NULL, NULL}
};


RtpInterfaceManager *RtpInterfaceManager::rtpItfManagerInstance;

  static inline const char *
ac_id2name (ac_item_t *a, int id)
{
  while (a->name && a->id2!=id)
    ++a;
  return a->name ? a->name : "unknown";
}

  static inline int
ac_name2id (ac_item_t *a, const char *name)
{
  while (a->name && stricmp(a->name, name))
    ++a;
  return a->name ? a->id2 : -1;
}


acTCoders ac_getcodec (Rtp_Payload_Code pt)
{
  for (unsigned i=0;i<ARRAY_SIZE(rtp_codec); ++i) {
    if (rtp_codec[i].id == pt)
      return (acTCoders) rtp_codec[i].id2;
  }
  return (acTCoders)-1;
}

Rtp_Payload_Code ac_getpayloadcode(int ac_codec)
{
  for (unsigned i=0;i<ARRAY_SIZE(rtp_codec); ++i) {
    if (rtp_codec[i].id2 == ac_codec)
      return (Rtp_Payload_Code)rtp_codec[i].id;
  }
  return ac_codec == acG723Low ? RTP_PT_G7231 : RTP_PT_INVALID;
}

/** Get the name of a codec.
*/
const char *Rtp_Payload_GetName (Rtp_Payload_Code code)
{
  for (unsigned i=0;i<ARRAY_SIZE(rtp_codec); ++i) {
    if (rtp_codec[i].id == code)
      return rtp_codec[i].name;
  }
  return "invalid_payload_code";
}

/** Get the name of a codec.(formal SDP name)
*/
const char *Rtp_Payload_GetFormalName (Rtp_Payload_Code code)
{
  for (unsigned i=0;i<ARRAY_SIZE(rtp_codec); ++i) {
    if (rtp_codec[i].id == code)
      return rtp_codec[i].formalName;
  }
  return "invalid_payload_code";
}


///////////////////////////////////////////////////////////////////////////////

/** Construct the Rtp_Media instance.
*/
Rtp_Media::Rtp_Media(const char *name,
    MediaId id,
    Rtp_Interface *itf,
    Interface *phy_itf,
    RTP_Channel *channel,
    void *itf_src)
: Media(name, channel->GetMediaID(), itf, channel->GetSwitchDevice(name)),	
  acChannel(channel), phyInterface(phy_itf),
  itfSource(itf_src),  
  eventCallback(*this, channel),
  payloadCurrent(RTP_PT_INVALID)
{
}


Rtp_Media::~Rtp_Media()
{
}


/** Get list of supported codec.
  Upon return, the 'payloads' array parameter will be filled in with
  the codecs, up to 'maxnum' number.
  @return the number of codecs returned.
  */
size_t Rtp_Media::GetSupportedPayload(Rtp_Payload_Code payloads[], size_t maxnum)
{
  if (acChannel && acChannel->GetInitializationStatus() >= 2) {
    const RTP_Config& conf = acChannel->GetRtpConfig();	
    payloads[0] = conf.RTP.Codec;
    if (payloads[0] != RTP_PT_INVALID)
      return 1;
    else
      assert(false);
  }

  Rtp_Interface *rtp_itf = (Rtp_Interface *)GetInterface();
  return rtp_itf->GetSupportedPayload(payloads, maxnum);
}

/** Get the local address and port which this channel can be contacted.
  This function can be called even when the Rtp_Media instance has not
  been initialized.
  */
void   Rtp_Media::GetLocalAddress(ACE_INET_Addr & addr)
{
  acChannel->GetLocalAddress(addr);
}


Interface *Rtp_Media::GetPhysicalInterface()
{
  return phyInterface;
}

/** Starts RTP transmitter/receiver using the specified codec to the
  specified destination.
  */
bool   Rtp_Media::Open()
{
  /*
     CLog::Detail (THISMODULE, "[%s] Open remote=%s:%d, paycode=%d",
     GetName(),
     remote_rtp.get_host_addr(), remote_rtp.get_port_number(),
     payload);
     */
  assert (acChannel && acChannel->GetInitializationStatus() < 2);

  Rtp_Interface *itf = (Rtp_Interface *)GetInterface();
  AcChannelParam param = itf->GetChannelParameter();

  param.GetParam().TransportSettings.UseNIorPCI = 1;
  param.GetParam().Active = 1;

  RTP_Config config = acChannel->GetRtpConfig();
  itf->GetRtpChannelConfig(config);
  acChannel->Open(config);

  //
  // Allocate switch address, and set the TDM stream/timeslot output
  //
  AudioCodes_Switch_Device *rtp_switch = (AudioCodes_Switch_Device *)GetSwitchDevice();
  assert(rtp_switch);
  if (rtp_switch->SupportSCbus(NULL)) {
    SCbusAddress *sc_addr = (SCbusAddress *)rtp_switch->GetAddress(SF_SCbus);
    assert (sc_addr);
    if (sc_addr != NULL)
      param.SetTxTimeslot(sc_addr->GetTimeslot());

  } else if (rtp_switch->SupportCTbus(NULL)) {
    CTbusAddress *ct_addr = (CTbusAddress *)rtp_switch->GetAddress(SF_CTbus);
    assert (ct_addr);
    if (ct_addr != NULL) {
      param.GetParam().TDMBusSettings.TDMBusOutputPort = ct_addr->GetStream();
      param.GetParam().TDMBusSettings.TDMBusOutputChannel = ct_addr->GetTimeslot();
    }

  }
#ifdef XGATE_HAS_HMP
  else if(dynamic_cast<HostSwitchDevice*>(GetSwitchDevice()))
  {

  }
#endif
  else
    return false;

#ifdef AC_LOOPBACK_TIMESLOT
  /*
   * Initial channel configuration:
   *	- output timeslot is set to the allocated address (done above)
   *	- output disabled
   -	- input listens to itself
   */
  param.EnableTDMOutput(false);
  param.GetParam().TDMBusSettings.TDMBusInputPort = param.GetParam().TDMBusSettings.TDMBusOutputPort;
  param.GetParam().TDMBusSettings.TDMBusInputChannel = param.GetParam().TDMBusSettings.TDMBusOutputChannel;
#else
  param.EnableTDMOutput(false);
  //
  // TODO:
  //	the channel initially listens to timeslot 1023, whathever that timeslot
  //	contains.
  //
  // param.SetRxTimeslot(1023);
#endif

  return true;
}


/** Start the RTP transmitter/receiver using the specified codec to the
  specified destination.
  */
bool   Rtp_Media::Activate(const ACE_INET_Addr & remote_rtp, 
    Rtp_Payload_Code payload, int ptime)
{
  if (!acChannel) {
    assert(false);
    CLog::Fatal(THISMODULE, "Channel is not present in RTP_Media::Activate [%s]", this->GetName());
    return false;
  }

  //
  // Make sure the payload is among the one supported by this interface
  //
  Rtp_Interface *itf = (Rtp_Interface *)GetInterface();

  Rtp_Payload_Code payload_list[RTP_MAX_CODEC_LIST];
  unsigned ncodec = itf->GetSupportedPayload(payload_list, RTP_MAX_CODEC_LIST);
  if (ncodec==0 || std::find(payload_list, payload_list+ncodec, payload)==payload_list+ncodec) {
    CLog::Detail(THISMODULE, "Unsupported payload %d (remote RTP addr:%s)",
        payload, remote_rtp.get_host_addr());
    return false;
  }

  payloadCurrent = payload;
  _quality_data._payload = payload;
  CLog::Detail(THISMODULE, "Rtp_Media::Activate rtpMedia acChannel = %02x \n", acChannel);
  return acChannel->Activate(remote_rtp, payload, 0, 0); // added 3rd argument as 0 ; added for ippbx
}


/** Close the channel.
*/
bool   Rtp_Media::Close()
{
  //CLog::Detail (THISMODULE, "[%s] Close", GetName());
  acChannel->Close();
  return true;
}

#if 0
void Rtp_Media::GetDtmfDigit(int number_of_digits, int wait_time, char* dtmf_digits)
{
  if (!acChannel) {
    assert(false);
    CLog::Fatal(THISMODULE, "Channel is not present in RTP_Media::getDtmfDigit [%s]", this->GetName());
    return false;
  }
  cout << "*** Inside rtpmedia::getDtmfDigit, calling acChannle->getDtmfDigit *****"<< endl;
  return acChannel->getDtmfDigit(number_of_digits, wait_time, dtmf_digits);

}
////#else commented for ippbx
int Rtp_Media::GetDtmfDigit(char* dtmf_digits)
{
  CLog::Detail(THISMODULE, "sink_event_handler GetDtmfDigit %02x \n\n", acChannel);
  if (!acChannel) {
    assert(false);
    CLog::Fatal(THISMODULE, "Channel %s is not present in RTP_Media::getDtmfDigit", this->GetName());
    return false;
  }
  return acChannel->GetDtmfDigit(dtmf_digits);
}
#endif 


void Rtp_Media::SetDtmfDigitTimeout(int max_time)
{
  if (!acChannel) {
    assert(false);
    CLog::Fatal(THISMODULE, "Channel %s is not present in RTP_Media::SetDtmfDigitTimeout", this->GetName());
    return false;
  }
  return acChannel->SetDtmfDigitTimeout(max_time);
}


//bool Rtp_Media::PlayFileWav(char const *filename, const char* targetHost, bool autorepeat)
bool Rtp_Media::PlayFileWav(char const *filename, bool autorepeat)
{

  cout << "*************** rtp_media::PlayFileWav *******************" << endl;
  if (!acChannel) {
    assert(false);
    CLog::Fatal(THISMODULE, "Channel is not present in RTP_Media::PlayFileWav [%s]", this->GetName());
    //  cout << "*** Inside rtpmedia::PlayFileWav, Not valid acChannle object  *****"<< endl;
    return false;
  }
  // cout << "*** Inside rtpmedia::PlayFileWav, calling acChannle->PlayFileWav  *****"<< endl;
  CLog::Detail(THISMODULE, "Rtp_Media::PlayFileWav %s \n", filename);
  return acChannel->PlayFileWav(filename, autorepeat);
  // return acChannel->PlayFileWav(filename, targetHost, true);
}


bool Rtp_Media::PlayFileWav(char const *filename, bool autorepeat, void *tmpCall)
//bool Rtp_Media::PlayFileWav(char const *filename, bool autorepeat)
{

  cout << "*************** rtp_media::PlayFileWav *******************" << endl;
  if (!acChannel) {
    assert(false);
    CLog::Fatal(THISMODULE, "Channel is not present in RTP_Media::PlayFileWav [%s]", this->GetName());
    //  cout << "*** Inside rtpmedia::PlayFileWav, Not valid acChannle object  *****"<< endl;
    return false;
  }
  // cout << "*** Inside rtpmedia::PlayFileWav, calling acChannle->PlayFileWav  *****"<< endl;

  CLog::Detail(THISMODULE, "Rtp_Media::PlayFileWav tmpCall %s \n", filename);
  return acChannel->PlayFileWav(filename, autorepeat,tmpCall);
  // return acChannel->PlayFileWav(filename, autorepeat);
}


bool Rtp_Media::AttachTask(void *tmpCall)
  //bool Rtp_Media::PlayFileWav(char const *filename, bool autorepeat)
{

  //   cout << "*************** rtp_media::PlayFileWav *******************" << endl;
  if (!acChannel) {
    assert(false);
    CLog::Fatal(THISMODULE, "Channel is not present in RTP_Media::HmpAttachTask [%s]", this->GetName());
    //  cout << "*** Inside rtpmedia::PlayFileWav, Not valid acChannle object  *****"<< endl;
    return false;
  }
  // cout << "*** Inside rtpmedia::PlayFileWav, calling acChannle->PlayFileWav  *****"<< endl;

  return acChannel->AttachTask(tmpCall);
  // return acChannel->PlayFileWav(filename, autorepeat);
}

bool Rtp_Media::StopFileWav(int waitTime)
{
  if (!acChannel) {
    assert(false);
    CLog::Detail(THISMODULE, "rtpMedia Channel is not present in RTP_Media::PlayFileWav [%s]", this->GetName());
    //        cout << "*** Inside rtpmedia::PlayFileWav, Not valid acChannle object  *****"<< endl;
    return false;
  } else {
    //  cout << "*** rtpmedia::StopFileWav, calling acChannle->StopFileWav  *****"<< endl;
    CLog::Detail(THISMODULE, "rtpMedia Channel is present in RTP_Media::PlayFileWav [%s]", this->GetName());
    return acChannel->StopFileWav(1);
  }
}


bool Rtp_Media::RecordFile(const char *filename, Rtp_Media *rtpMedia2)
{
  if (!acChannel) {
    assert(false);
    CLog::Fatal(THISMODULE, "Channel 1 %s is not able to obtained in RTP_Media::RecordFile", this->GetName());
    return false;
  }

  RTP_Channel *acChannel2 = rtpMedia2->GetChannel();
  if (!acChannel2) {
    assert(false);
    CLog::Fatal(THISMODULE, "Channel 2 %s is not able to obtained in RTP_Media::RecordFile", this->GetName());
    return false;
  }

  return acChannel->RecordFile(filename, acChannel2);
}

void Rtp_Media::EndRecordFile()
{
  if (!acChannel) {
    assert(false);
    CLog::Fatal(THISMODULE, "Channel %s is not able to obtained in RTP_Media::EndRecordFile", this->GetName());
    return false;
  }
  acChannel->EndRecordFile();
}

bool Rtp_Media::ConfCreateRoom(const char *room_name)
{
  if (!acChannel) {
    assert(false);
    CLog::Fatal(THISMODULE, "Channel %s is not able to obtained in RTP_Media::ConfCreateRoom", this->GetName());
    return false;
  }
  return acChannel->ConfCreateRoom(room_name);
}

bool Rtp_Media::ConfAddParty(const char *room_name)
{
  CLog::Detail(THISMODULE, "*** Rtp_Media::ConfAddParty: Adding party ***");
  if (!acChannel) {
    assert(false);
    CLog::Fatal(THISMODULE, "Channel %s is not able to obtained in RTP_Media::ConfAddParty", this->GetName());
    return false;
  }
  return acChannel->ConfAddParty(room_name);
}

bool Rtp_Media::ConfRecordFile(const char *room_name, const char *filename)
{
  CLog::Detail(THISMODULE, "*** Rtp_Media::ConfRecordFile: ***");
  if (!acChannel) {
    assert(false);
    CLog::Fatal(THISMODULE, "Channel %s is not able to obtained in RTP_Media::ConfAddParty", this->GetName());
    return false;
  }
  return acChannel->ConfRecordFile(room_name,filename);
}

void Rtp_Media::ConfEndRecordFile(const char* room_name)
{
  if (!acChannel) {
    assert(false);
    CLog::Fatal(THISMODULE, "Channel %s is not able to obtained in RTP_Media::ConfEndRecordFile", this->GetName());
    return false;
  }
  acChannel->ConfEndRecordFile(room_name);
}


/** Dial DTMF digits.
*/
void   Rtp_Media::DialDtmf(const char *digits)
{
  if (acChannel)
    acChannel->DialDtmf(digits);
}

/** Determine the channel status.
*/
Rtp_Media::Rtp_Status Rtp_Media::GetMediaStatus() const
{
  if (!acChannel)
    return Rtp_Not_Opened;
  if (acChannel->GetInitializationStatus() < 0 || eventCallback.destUnreach)
    return Rtp_Error;
  return acChannel->GetInitializationStatus() > 0 ? Rtp_Opened : Rtp_Not_Opened;
}


/** Notification from AudioCodes on media error.
  This will disconnect the call.
  */
void Rtp_Media::OnMediaError()
{
  Call *call = GetCall();

  if (call && call->GetState() < CS_Connected) {
    acChannel->SetEventCallback(NULL);

    CLog::Detail(THISMODULE, "[%s] Media error, disconnecting call %s..", 
        GetName(), call->GetName());

    try {
      call->DeviceDisconnectReq(CC_ResourceUnavailable, CP_NoIndication);
      if (call->GetState() < CS_Connected && call->GetState() != NULL)
        call->DisconnectInd(CC_ResourceUnavailable, CP_NoIndication);
    }
    catch (...) {
      CLog::Fatal(THISMODULE, "--FATAL--: Caught unhandled exception in OnMediaError()");
      Sleep(1000);
      throw;
    }
  }
}

void Rtp_Media::UpdateQualityData(CallQualityParameter param) {
  const AcRtcpStat * rtcpStat = NULL;

  if (!acChannel) return;
  rtcpStat = &acChannel->GetRtcpStat();
  if (!_quality_data._ready) {
    if (!rtcpStat->rx.ready || !rtcpStat->tx.ready) 
      return;
    _quality_data.Enable();
    param = CQ_ALL; // calculate all for the first time
  }

  _quality_data._payload = payloadCurrent;
  switch (param) {
    case CQ_ALL:
    case CQ_RX_PACKETS:
      _quality_data._rx_packets = rtcpStat->rx.packetCount;
      if (param) break;
    case CQ_RX_BYTES:
      _quality_data._rx_bytes = rtcpStat->rx.octetCount;
      if (param) break;
    case CQ_RX_RTT:
      _quality_data._rx_rtt = rtcpStat->rx.roundTrip;
      if (param) break;
    case CQ_RX_JITTER:
      _quality_data._rx_jitter = rtcpStat->rx.jitter;
      if (param) break;
    case CQ_RX_LOSS:
      _quality_data._rx_loss = rtcpStat->rx.cummulativeLost * 100.0 / rtcpStat->rx.packetCount;
      if (param) break;
    case CQ_RX_FRACLOSS:
      _quality_data._rx_fracloss = rtcpStat->rx.fractionLost;
      if (param) break;
    case CQ_TX_PACKETS:
      _quality_data._tx_packets = rtcpStat->tx.packetCount;
      if (param) break;
    case CQ_TX_BYTES:
      _quality_data._tx_bytes = rtcpStat->tx.octetCount;
      if (param) break;
    case CQ_TX_RTT:
      _quality_data._tx_rtt = rtcpStat->tx.roundTrip;
      if (param) break;
    case CQ_TX_JITTER:
      _quality_data._tx_jitter = rtcpStat->tx.jitter;
      if (param) break;
    case CQ_TX_LOSS:
      _quality_data._tx_loss = rtcpStat->tx.cummulativeLost * 100.0 / rtcpStat->tx.packetCount;
      if (param) break;
    case CQ_TX_FRACLOSS:
      _quality_data._tx_fracloss = rtcpStat->tx.fractionLost;
      if (param) break;
    default:
      break;
  }
}


float Rtp_Media::GetQuality(CallQualityParameter param) { 
  return _quality_data.GetValue(param); 
}


/* Enable RFC 2833 in RTP_Config */
void Rtp_Media::DTMFSetRfc2833(int rx_pt, int tx_pt)
{
  if (acChannel) {
    RTP_Config config = acChannel->GetRtpConfig();
    config.DTMF.Transport_Type = RTP_Config::DTMF_RFC2833;
    config.DTMF.rfc2833_rx_pt = rx_pt;
    config.DTMF.rfc2833_tx_pt = tx_pt;
    acChannel->SetRtpConfig(config);
  }
}

// enable/disable RX or TX side transmission
void Rtp_Media::enableTransmission(bool isRtpRxEnable, bool isRtpTxEnable)
{
  if(acChannel)
  {
    acChannel->enableTransmission(isRtpRxEnable, isRtpTxEnable);
  }
}

///////////////////////////////////////////////////////////////////////////////

  Rtp_Interface::Rtp_Interface(const char *name)
: Interface(ITF_RTP, name), useG7231Low(false), maxForwards(DEFAULT_MAX_FORWARDS), stripPrivateInfo(true)
{
  // Fills in with default values.
  channelParam.SetDefaultParam();

  // By default, RAP is enabled.
  EnableRAP();

#ifdef XGATE_HAS_RTP_SIGNALLING
  //    SetSignallingDriver(RTP_Signalling::Instance());
#endif

  payloadCount = 1;
  payloadList[0] = ac_getpayloadcode(channelParam.GetParam().VoiceSettings.Coder);

  lastItfSourceUsed = itfSourceList.end();
  RtpInterfaceManager::Instance()->RegisterRtpInterface (this);

  _QMonitor.SetPackingFactor(this->GetPackingFactor());
  _statisticsInterval = 6;
}

Rtp_Interface::~Rtp_Interface()
{
  RtpInterfaceManager::Instance()->UnregisterRtpInterface (this);
}


/** Allocate one media channel from the interface.
  It returns false when there is no free media available
  */
bool Rtp_Interface::AllocateMedia(Call *call,Media *&media,SelectionStrategy sel,CallDirection dir,CallParameter *cparam)
{
    CLog::Detail(THISMODULE, "Inside Rtp_Interface::AllocateMedia : ************"); 
  media = NULL;

  if (!IsActive())
  {
    CLog::Detail(THISMODULE, "Rtp_Interface::AllocateMedia : !IsActive() ************"); 
	cout << "Rtp_Interface::AllocateMedia : !IsActive() ************";
    return false;
  }

  List <Interface_Source*>::iterator iit, iend;
  iend = itfSourceList.end();
  RTP_Channel *channel = NULL;
  Interface_Source * isrc = NULL;


  //    cout << " ************** Inside AllocateMedia , rtptimeout is "  << rtpTimeout << endl;

  //  cout << "******** I am inside AllocateMedia ************" << endl;

  /* Use interface source in round robin fashion
  */
  unsigned nitfsrc = itfSourceList.size();
  iit = lastItfSourceUsed;
  for (unsigned i=0; i<nitfsrc; ++i) {
    if (iit == iend)
      iit = lastItfSourceUsed = itfSourceList.begin();
    if (iit == iend)
      break;
    isrc = *iit;
    if (isrc->active && isrc->GetFreeCh()) {
      channel = RtpInterfaceManager::Instance()->AllocateChannel(isrc->boardInterfaceName, 
          dir, cparam);
      if (channel) {
        ++isrc->usedCh;
        ++lastItfSourceUsed;
        if (lastItfSourceUsed == itfSourceList.end())
          lastItfSourceUsed = itfSourceList.begin();
        break;
      }
    }
    ++iit;
  }


  if (!channel){
    //		printf("********** Channel not  allocated *********\n");
#ifdef XGATE_HAS_MTNRTP
    List<Board_Source*>::iterator boardit = brdSourceList.begin(), end=brdSourceList.end();
    for (; boardit!=end; ++boardit) {
      Board_Source* src = *boardit;

      // Move channels from releasing list to idle list.
      src->ReleaseIdleChannels();

      if (src->active_.size()+src->releasing_.size() < src->total_) {
        /*	if (src->name_ == "virtual") {
        // Allocate virtual RTP channel
        RTP_Virtual_Channel *vch = new RTP_Virtual_Channel;
        char media_name[80];
        sprintf(media_name, "%s:virt", GetName());
        media = new Rtp_Media(media_name, (MediaId)vch, this, NULL, vch, src);

        // Round robin source
        brdSourceList.erase(boardit);
        brdSourceList.push_back(src);
        break;

        } */ if (src->name_ == "host") {
          // Allocate virtual RTP channel
          ;
          char media_name[80];
          sprintf(media_name, "%s:host", GetName());
#ifdef XGATE_HAS_HMP
          //Host_Rtp_Channel *hostChannel = new Host_Rtp_Channel(media_name);
          bool dtmfEvent = true;
          dtmfEvent = (DTMFGetRfc2833()>0) ? true : false;
          //		Host_Rtp_Channel *hostChannel = new Host_Rtp_Channel(media_name, dtmfEvent);
          Host_Rtp_Channel *hostChannel = new Host_Rtp_Channel(media_name, dtmfEvent, rtpTimeout);


          media = new Rtp_Media(media_name, (MediaId)hostChannel, this, NULL, hostChannel, src);
#endif

          // Round robin source
          brdSourceList.erase(boardit);
          brdSourceList.push_back(src);
          break;
        }
      /*                   else {
      // Allocate from Mapletree
      MTN_Rtp_Channel *mrc = 
      MTN_Rtp_Channel_Factory::Instance()->Allocate(this, src->name_.c_str());
      if (mrc) {
      src->active_.push_back(mrc);

      char media_name[80];
      sprintf(media_name, "%s:%s", GetName(), mrc->GetChannelName());
      media = new Rtp_Media(media_name,
      (MediaId)mrc, 
      this, NULL, mrc, src);

      // Round robin source
      brdSourceList.erase(boardit);
      brdSourceList.push_back(src);
      break;
      }
      }*/
      }
    }
#endif	// XGATE_HAS_MTNRTP
  } else {
    //		printf("********** Channel allocated *********\n");
    //	printf("******* Calling Rtp_Media *************\n");
    media = new Rtp_Media(rtp_make_chname(_name.c_str(), 
          isrc->boardInterfaceName.c_str(), 
          channel->GetChannelNumber()),
        channel->GetMediaID(), 
        this, 
        RtpInterfaceManager::Instance()->FindInterface(isrc->boardInterfaceName),
        channel, isrc);
  }

  if (media) {
    DecMediaCount(media, dir, cparam);
    _QMonitor.Register((Rtp_Media*)media);

    return true;
  } else {
    CLog::Detail(THISMODULE, "%s: Unable to allocate RTP media channel for %s call", 
        GetName(), (dir==CD_Outgoing ? "outgoing" : "incoming"));
    return false;
  }
}


/** AllocateMedia: A variant of AllocateMedia that specify a particular media
  to be allocated. It returns false when there is such media is not available
  */
bool Rtp_Interface::AllocateMedia(Call *call,Media *&media,MediaId id,CallDirection dir,CallParameter *cparam)
{
  return AllocateMedia(call, media, SS_Ascending, dir, cparam);
}


/** ReleaseMedia: Called by Call object when the call becomes idle. The underlying
  channel is then ready to be used for another call.
  */
void Rtp_Interface::ReleaseMedia(Media *media,CallDirection dir,CallParameter *cparam,CallCause cc)
{
//  Rtp_Media *rtp_media = (Rtp_Media*)media;
//  RTP_Channel *ac_ch = rtp_media->GetChannel();

  IncMediaCount(media, dir, cparam, cc);
 /* _QMonitor.Unregister(rtp_media); //Yadav
  rtp_media->Close();

  if(rtp_media->GetPhysicalInterface() != NULL){

    Interface_Source *isrc = (Interface_Source*) rtp_media->GetItfSource();

    assert(isrc && isrc->usedCh > 0);
    --isrc->usedCh;
    if (!isrc->active && isrc->usedCh == 0) {
      assert(GetMediaCount() >= isrc->maxCh);
      SetMediaCount(GetMediaCount() - isrc->maxCh);
      List <Interface_Source*>::iterator it = std::find(itfSourceList.begin(), itfSourceList.end(), isrc);
      assert(it != itfSourceList.end());
      if (it != itfSourceList.end()) {
        List <Interface_Source*>::iterator end = itfSourceList.end();
        itfSourceList.remove(isrc);
        delete isrc;

        // Update lastItfSourceUsed after the list has been updated
        if (it == lastItfSourceUsed)
          lastItfSourceUsed = itfSourceList.begin();
        else if (lastItfSourceUsed == end)
          lastItfSourceUsed = itfSourceList.begin();
      }
    }

    rtp_media->ReleaseChannel(dir, cparam, cc);

  } 
#ifdef XGATE_HAS_MTNRTP
  else if (ac_ch->GetBoardType() == RTP_Board_MTN) {
    MTN_Rtp_Channel *mtn_ch = static_cast<MTN_Rtp_Channel *>(ac_ch);
    Board_Source* bsrc = (Board_Source*) rtp_media->GetItfSource();	
    rtp_media->ReleaseChannel(dir, cparam, cc);
    bsrc->ReturnChannel(mtn_ch);
  } 
#endif	// XGATE_HAS_MTNRTP
  else if (ac_ch->GetBoardType() == RTP_Board_Virtual) {
    rtp_media->ReleaseChannel(dir, cparam, cc);
    delete ac_ch;
  }

  CLog::Detail(THISMODULE, "Rtp_Interface::ReleaseMedia rtp_media = %02x ", rtp_media);
  delete rtp_media; // T 20012018   */
}


/** Register all available commands to this interface.
*/
void Rtp_Interface::InitTermCmd()
{
  unsigned i;
  Cfg_Tree_Group *cfg_tree = GetConfigTree();
  assert (cfg_tree);

  /* Interface command.
  */
  Term_Cmd *cmd = new Term_Cmd (this, Rtp_Cmd_Interface, "interface",
      "Specify the physical interface",
      PrivilegeLevelAdmin, TermModeConfig, true);
  cmd->Add(new Term_Cmd_Arg("@0", "", 
        CVariantValidator("", "Interface name (e.g. \"aud0\")")));
  cmd->Add(new Term_Cmd_Arg("@1", "channel-count", 
        CVariantSingleValueValidator("channel-count", "")));
  cmd->Add(new Term_Cmd_Arg("@2", 1, 
        CVariantRangeValidator(1, 2048,"Number of channels allocated from this interface")));
  RegisterTermCmd (cmd, cfg_tree);

  // Board command.
#ifdef XGATE_HAS_MTNRTP
  cmd = new Term_Cmd( this, Rtp_Cmd_Board, "board", "Configure board sources",
      PrivilegeLevelAdmin, TermModeConfig, true);
  cmd->Add( new Term_Cmd_Arg("@0", CVariant(""), MTN_TermBoardEnum()) );
  cmd->Add( new Term_Cmd_Arg("@1", CVariant(""), CVariantSingleValueValidator("channel-count", "")) );
  cmd->Add( new Term_Cmd_Arg("@2", CVariant(0),  CVariantRangeValidator(0, 2048, "Number of channels to allocate from this board")) );
  RegisterTermCmd( cmd, cfg_tree );
#endif

  /* Description
  */
  cmd = new Term_Cmd(this, Rtp_Cmd_Description, "description", "Specify the description for this interface", PrivilegeLevelAdmin, TermModeConfig, true);
  cmd->Add(new Term_Cmd_Arg("@0", "", CVariantValidator("", "Description for this interface"),TermArgOptional));
  RegisterTermCmd(cmd,cfg_tree);


  /* Rtp_Cmd_Accept
  */
  /*
   *** OBSOLETE ***
   cmd = new Term_Cmd(this, Rtp_Cmd_Accept, "accept", "Accept source IP address specification", PrivilegeLevelAdmin, TermModeConfig, true);
   cmd->Add(new Term_Cmd_Arg("@0", "", CVariantValidator("", "Address")));
   cmd->Add(new Term_Cmd_Arg("@1", "", CVariantValidator("", "Network mask/submask")));
   RegisterTermCmd(cmd, cfg_tree);
   */

  /* Rtp_Cmd_AccessList
  */
  cmd = new Term_Cmd(this, Rtp_Cmd_AccessList, "access-list", "Access list to match for this interface", PrivilegeLevelAdmin, TermModeConfig, true);
  cmd->Add(new Term_Cmd_Arg("@0", "", ACL_Validator(), TermArgOptional));
  RegisterTermCmd(cmd, cfg_tree);

  /* Rtp_Cmd_Mode
  */
  cmd = new Term_Cmd (this, Rtp_Cmd_Mode, "mode",
      "Specify channel usage mode",
      PrivilegeLevelAdmin, TermModeConfig, false);
  CVariantChoiceValidator mode_vdtor("network", "Use network interface to send/receive packet");
  mode_vdtor.Add("pci", "Use PCI bus to send/receive packet");
  cmd->Add(new Term_Cmd_Arg("@0", "", mode_vdtor));
  RegisterTermCmd (cmd, cfg_tree);


  /* Rtp_Cmd_Dtmf
  */
  cmd = new Term_Cmd (this, Rtp_Cmd_Dtmf, "dtmf",
      "Specify DTMF tone parameters",
      PrivilegeLevelAdmin, TermModeConfig, false);
  CVariantChoiceValidator dtmf_cmd_vdtor("volume", "DTMF tone volume");
  dtmf_cmd_vdtor.Add("length", "The length of the tone (ON interval), in miliseconds");
  dtmf_cmd_vdtor.Add("interval", "Interval between digits  (OFF interval), in miliseconds");
  dtmf_cmd_vdtor.Add("transfer", "DTMF transfer type to remote destination");

  cmd->Add(new Term_Cmd_Arg("@0", "", dtmf_cmd_vdtor));

  // DTMF transfer.
  CVariantChoiceValidator dtmf_xfer_vdtor(ac_dtmftransfer[0].name, ac_dtmftransfer[0].desc);
  for (i=1; i<ARRAY_SIZE(ac_dtmftransfer) && ac_dtmftransfer[i].name; ++i)
    dtmf_xfer_vdtor.Add(ac_dtmftransfer[i].name, ac_dtmftransfer[i].desc);
  cmd->AddDynamic(1, 0, "transfer",
      new Term_Cmd_Arg("@1", "", dtmf_xfer_vdtor));

  // DTMF volume.
  cmd->AddDynamic(1, 0, "volume",
      new Term_Cmd_Arg("@1", 16, CVariantRangeValidator(1, 32, "DTMF tone volume")));

  // DTMF length
  cmd->AddDynamic(1, 0, "length",
      new Term_Cmd_Arg("@1", 20, CVariantRangeValidator(5, 100, "Length in miliseconds")));

  // DTMF interval
  cmd->AddDynamic(1, 0, "interval",
      new Term_Cmd_Arg("@1", 50, CVariantRangeValidator(5, 100, "Interval in miliseconds")));

  RegisterTermCmd (cmd, cfg_tree);


  /* Rtp_Cmd_Fax */
  cmd = new Term_Cmd(this, Rtp_Cmd_Fax, "fax",
      "Specify Fax transfer settings",
      PrivilegeLevelAdmin, TermModeConfig, false);
  CVariantChoiceValidator fax_cmd_vdtor("transfer", "Configure Fax transfer settings");
  dtmf_cmd_vdtor.Add("bandwidth", "Max bandwidth for relay/t38 mode");
  cmd->Add(new Term_Cmd_Arg("@0", "", fax_cmd_vdtor));


  // Fax transfer.
  CVariantChoiceValidator fax_xfer_vdtor(ac_faxtransfer[0].name, ac_faxtransfer[0].desc);
  for (i=1; i<ARRAY_SIZE(ac_faxtransfer) && ac_faxtransfer[i].name; ++i)
    fax_xfer_vdtor.Add(ac_faxtransfer[i].name, ac_faxtransfer[i].desc);
  cmd->AddDynamic(1, 0, "transfer",
      new Term_Cmd_Arg("@1", "", fax_xfer_vdtor));

  // Fax relay bandwidth
  CVariantChoiceValidator fax_rate_vdtor(ac_rate[0].name, ac_rate[0].desc);
  for (i=1; i<ARRAY_SIZE(ac_rate) && ac_rate[i].name; ++i)
    fax_xfer_vdtor.Add(ac_rate[i].name, ac_rate[i].desc);
  cmd->AddDynamic(1, 0, "bandwidth",
      new Term_Cmd_Arg("@1", "", fax_rate_vdtor));


  RegisterTermCmd (cmd, cfg_tree);



  /* Rtp_Cmd_Signalling
  */
  cmd = new Term_Cmd(this, Rtp_Cmd_Signalling, "signalling",
      "Specify call control/signalling type",
      PrivilegeLevelAdmin, TermModeConfig, false);

  CVariantChoiceValidator sigVdtor("","");

#ifdef XGATE_HAS_RTP_SIGNALLING
  sigVdtor.Add("q931", "Proprietary Q.931 signalling (Q931oIP)");
#endif

#ifdef XGATE_HAS_H323
  sigVdtor.Add("h323", "ITU H.323");
#endif

#ifdef XGATE_HAS_SIP
  sigVdtor.Add("sip", "Session Initiation Protocol");
#endif

#ifdef XGATE_HAS_SOFIA
  sigVdtor.Add("sofia", "Sofia-SIP");
#endif
  cmd->Add(new Term_Cmd_Arg("@0", "", sigVdtor));
  RegisterTermCmd(cmd, cfg_tree);

  /* Rtp_Cmd_Max_Forwards
  */
  cmd = new Term_Cmd(this, Rtp_Cmd_Max_Forwards, 
      "sip-max-forwards",
      "Specify maximum of signalling packet forwarding",
      PrivilegeLevelAdmin, 
      TermModeConfig, false);
  cmd->Add (new Term_Cmd_Arg("number", CVariant(DEFAULT_MAX_FORWARDS),
        CVariantRangeValidator( 10, 100, "maximum number of packet forwarding")));
  RegisterTermCmd (cmd, cfg_tree);

  /* strip-outgoing-private-info
  */
  cmd = new Term_Cmd(this, Rtp_Cmd_Strip_Private_Info, 
      "strip-outgoing-private-info",
      "strip all remaining xgate's private info exported by app",
      PrivilegeLevelAdmin, 
      TermModeConfig, true);
  RegisterTermCmd (cmd, cfg_tree);


  /* Rtp_Cmd_Tdm
  */
  cmd = new Term_Cmd (this, Rtp_Cmd_Tdm, "tdm",
      "Specify TDM bus parameters",
      PrivilegeLevelAdmin, TermModeConfig, false);
  CVariantChoiceValidator tdm_cmd_vdtor("input-gain", "TDM bus input volume");
  tdm_cmd_vdtor.Add("output-gain", "TDM bus output volume");

  cmd->Add(new Term_Cmd_Arg("@0", "", tdm_cmd_vdtor));

  // TDM input gain
  cmd->AddDynamic(1, 0, "input-gain",
      new Term_Cmd_Arg("@1", 32, CVariantRangeValidator(1,63,"Input gain")));

  // TDM output gain
  cmd->AddDynamic(1, 0, "output-gain",
      new Term_Cmd_Arg("@1", 32, CVariantRangeValidator(1,63,"Output gain")));

  RegisterTermCmd (cmd, cfg_tree);


  /* Rtp_Cmd_Use
  */
  cmd = new Term_Cmd (this, Rtp_Cmd_Use, "use",
      "Specify additional channel features",
      PrivilegeLevelAdmin, TermModeConfig, true);
  CVariantChoiceValidator use_vdtor("echo-canceller", "Enable echo canceller");
  use_vdtor.Add("high-pass-filter", "Enable high pass filtering");
  use_vdtor.Add("post-filter", "Enable post filtering");
  use_vdtor.Add("silence-compression", "Enable RTP silence compression");
  use_vdtor.Add("g723-lo", "G.723.1 codec use the low bitrate (5.3 Kbps)");
  cmd->Add(new Term_Cmd_Arg("@0", "", use_vdtor));
  RegisterTermCmd (cmd, cfg_tree);

  /* Rtp_Cmd_EchoTail
  */
  cmd = new Term_Cmd (this, Rtp_Cmd_EchoTail, "echo-tail",
      "Specify the length of echo tail in ms if EC is enabled",
      PrivilegeLevelAdmin, TermModeConfig, true);
  cmd->Add(new Term_Cmd_Arg("@0", 25, CVariantRangeValidator(10, 128, "Echo tail in msec")));
  RegisterTermCmd (cmd, cfg_tree);

  /* Rtp_Cmd_Rtp
  */
  cmd = new Term_Cmd (this, Rtp_Cmd_Rtp, "rtp",
      "Specify RTP parameters",
      PrivilegeLevelAdmin, TermModeConfig, false);
  CVariantChoiceValidator rtp_cmd_vdtor("codec", "Specify RTP codec types");
  rtp_cmd_vdtor.Add("packing-factor", "Number of frames per one RTP packet");
  rtp_cmd_vdtor.Add("min-jitter", "Mininum delay jitter buffer size (in msec)");
  rtp_cmd_vdtor.Add("optimization-factor", "Buffer optimization factor");
  rtp_cmd_vdtor.Add("rtp-timeout", "rtp timeout in seconds");
  cmd->Add (new Term_Cmd_Arg("@0", "", rtp_cmd_vdtor));

  // RTP codec
  CVariantChoiceValidator codec_vdtor(rtp_codec[0].name, rtp_codec[0].desc);
  for (i=1; i<ARRAY_SIZE(rtp_codec) && rtp_codec[i].name; ++i) {
    if (rtp_codec[i].id2 != -1)
      codec_vdtor.Add(rtp_codec[i].name, rtp_codec[i].desc);
  }
  cmd->AddDynamic (1, 0, "codec", new Term_Cmd_Arg("@1", "", codec_vdtor));
  cmd->AddDynamic (2, 0, "codec", new Term_Cmd_Arg("@2", "", codec_vdtor, TermArgOptional));
  cmd->AddDynamic (3, 0, "codec", new Term_Cmd_Arg("@3", "", codec_vdtor, TermArgOptional));
  cmd->AddDynamic (4, 0, "codec", new Term_Cmd_Arg("@4", "", codec_vdtor, TermArgOptional));

  // RTP packing-factor
  cmd->AddDynamic (1, 0, "packing-factor", 
      new Term_Cmd_Arg("@1", 2, CVariantRangeValidator(1,15,"Packing factor")));

  // RTP min jitter
  cmd->AddDynamic (1, 0, "min-jitter",
      new Term_Cmd_Arg("@1", 40, CVariantRangeValidator(0, 150, "Minimum delay jitter buffer size (msec)")));

  // RTP optimization factor.
  cmd->AddDynamic (1, 0, "optimization-factor",
      new Term_Cmd_Arg("@1", 7, CVariantRangeValidator(1,15,"Buffer optimization factor")));

  cmd->AddDynamic (1, 0, "rtp-timeout",
      new Term_Cmd_Arg("@1", 7, CVariantRangeValidator(10,90,"rtp timeout duration")));

  RegisterTermCmd(cmd, cfg_tree);


  /* Rtp_Cmd_Rap
  */
  cmd = new Term_Cmd (this, Rtp_Cmd_Rap, "resource-locator", "Enable/disable resource locator service",
      PrivilegeLevelAdmin, TermModeConfig, true);
  RegisterTermCmd(cmd, cfg_tree);

  /* resource-type */
  Interface::AddResourceTypeCmd(this, Rtp_Cmd_ResourceType, cfg_tree);

  /* Rtp_Cmd_Group
  */
  cmd = new Term_Cmd (this, Rtp_Cmd_Group, "group", "Set the group name",
      PrivilegeLevelAdmin, TermModeConfig, true);
  cmd->Add(new Term_Cmd_Arg("@0", "", CVariantValidator("", "Group name")));
  RegisterTermCmd (cmd, cfg_tree);


  /* Rtp_Cmd_Shutdown
  */
  cmd = new Term_Cmd (this, Rtp_Cmd_Shutdown, "shutdown",
      "Shutdown interface",
      PrivilegeLevelAdmin, TermModeConfig, true);
  RegisterTermCmd(cmd, cfg_tree);

#ifdef XGATE_HAS_SIP
  /* Rtp_Cmd_Sip
  */
  cmd = new Term_Cmd (this, Rtp_Cmd_Sip, "sip",
      "SIP related signalling configuration",
      PrivilegeLevelAdmin, TermModeConfig, false);

  CVariantChoiceValidator sip_auth_vdtor("authorize", "Digest authorization configuration");
  sip_auth_vdtor.Add("realm", "Set authorization realm");
  cmd->Add(new Term_Cmd_Arg("@0", "", sip_auth_vdtor));

  CVariantChoiceValidator passwd_choice_vdtor(0, "To specify clear text password");
  passwd_choice_vdtor.Add (1, "To specify scrambled password");
  cmd->AddDynamic (1, 0, "authorize", new Term_Cmd_Arg("@1", "", CVariantValidator("", "Username")));
  cmd->AddDynamic (2, 0, "authorize", new Term_Cmd_Arg("@2", "", CVariantSingleValueValidator("password", ""), TermArgOptional));
  cmd->AddDynamic (3, 0, "authorize", new Term_Cmd_Arg("@3", "", passwd_choice_vdtor));
  cmd->AddDynamic (4, 0, "authorize", new Term_Cmd_Arg("@4", "", CVariantValidator("", "The user's password"), TermArgOptional));
  cmd->AddDynamic (1, 0, "realm", new Term_Cmd_Arg("@1", "", CVariantValidator("", "Authorization realm")));
  RegisterTermCmd(cmd, cfg_tree);
#endif

#ifdef XGATE_HAS_ACME_SBC
  cmd = new Term_Cmd (this,
      Rtp_Cmd_Sbc_Address,
      "sbc-address",
      "set the sbc ip address",
      PrivilegeLevelAdmin,
      TermModeConfig,
      true);
  cmd->Add(new Term_Cmd_Arg("sbc-address", CVariant(""), CVariantValidator("", "SBC IP Address")) );
  RegisterTermCmd(cmd, cfg_tree);
#endif

  /* Rtp_Cmd_QInterval
  */
  /*
     cmd = new Term_Cmd(this, Rtp_Cmd_QInterval, "q-interval", "Show quality interval", PrivilegeLevelAdmin, TermModeConfig, false);
     CVariantRangeValidator idrange(0, 24, "interval (0 to 24)");
     cmd->Add (new Term_Cmd_Arg ("arg1", CVariant(0), idrange));

     RegisterTermCmd(cmd,_cfgTree);
     */

}


/** Load: Called once at initialization time
*/
bool Rtp_Interface::Load()
{
#ifdef XGATE_HAS_SIP
// TODO Need to have verification before pushing to final git repo Yadav
//  SipStaticAuthServer *sip_auth_server = 
 //   (SipStaticAuthServer*)SipAgent::Instance()->GetAuthServer();

//  sipDigestAuthRealm = sip_auth_server->GetDefaultRealm();
#endif

  InitTermCmd();
  // 	cout << " *** Load: Rtp_Interface ... loading the interface ***" << endl;
  return Interface::Load();
}


/** Unload: Called once at termination time
*/
bool Rtp_Interface::Unload()
{
  // Delete all interface sources
  while (!itfSourceList.empty()) {
    delete itfSourceList.back();
    itfSourceList.pop_back();
  }
#ifdef XGATE_HAS_MTNRTP
  while (!brdSourceList.empty()) {
    delete brdSourceList.back();
    brdSourceList.pop_back();
  }
#endif // XGATE_HAS_MTNRTP

  lastItfSourceUsed = itfSourceList.end();
  return Interface::Unload();
}


/** Commit Configuration.
*/
bool Rtp_Interface::CommitConfiguration()
{
  return true;
}

/** Open: May be called several times to start the object
  The object may be then in In-Service (enabled/disabled) state
  */
bool Rtp_Interface::Start()
{
  if (itfSourceList.empty())
    CLog::Detail (THISMODULE, "Interface has no channels.");

  // Calculate current capacity
  unsigned capacity = 0;
  List <Interface_Source*>::iterator it, end;
  end = itfSourceList.end();
  for (it=itfSourceList.begin(); it!=end; ++it) {
    Interface_Source *isrc = *it;
    capacity += isrc->maxCh;
  }

#ifdef XGATE_HAS_MTNRTP
  List<Board_Source*>::iterator bit = brdSourceList.begin();
  for(;bit!=brdSourceList.end();bit++){
    capacity+= (*bit)->total_;
  }
#endif


  lastItfSourceUsed = itfSourceList.begin();
  Interface::SetMediaCount(capacity);
  Interface::Enable();
  return Interface::Start();
}


/** Close: Once started, an object may be requested to close
  The object goes to the Out-Of-Service state
  */
bool Rtp_Interface::Stop()
{
  Interface::Disable(DC_Shutdown);
  return Interface::Stop();
}


/** HandleEvents: Descendent shall implement this abstract method.
  It is called by Service Provider Manager
  */
void Rtp_Interface::HandleEvents()
{
#ifdef XGATE_HAS_MTNRTP
  List<Board_Source*>::iterator boardit = brdSourceList.begin(), end=brdSourceList.end();
  for (; boardit!=end; ) {
    Board_Source* bsrc = *boardit;
    bsrc->ReleaseIdleChannels();
    if (bsrc->total_ == 0 && bsrc->active_.empty() && bsrc->releasing_.empty()) {
      boardit = brdSourceList.erase(boardit);
    } else
      ++boardit;
  }
#endif
}


/** Get supported payload list.
*/
size_t Rtp_Interface::GetSupportedPayload(Rtp_Payload_Code payloads[], size_t maxnum) 
{
  if (maxnum > payloadCount)
    maxnum = payloadCount;
  memcpy(payloads, payloadList, maxnum*sizeof(Rtp_Payload_Code));
  return maxnum;
}

/** Determine whether this interface can accept the incoming call based on
  the call's source address.
  */
bool Rtp_Interface::CanAcceptCall(SignallingType sig_type, const ACL_Token & token) const
{
  if (GetMediaCount()==0){
    cout << "Mediacount is false , so exiting " << endl;
    //	return false;
  }
/*
  if (!((Rtp_Interface *)this)->GetSignallingDriver())
    return false;

  if (((Rtp_Interface *)this)->GetSignallingDriver()->GetType() != sig_type)
    return false;
*/
  /* Yogeesh commented out: IP-PBX scenario */
#if 1
  if (aclName.size())
    return ACL_Manager::Instance()->IsMatch(aclName.c_str(), token);
else // T 04092018 - differs from windows code^M
  {
    printf("aclName.size() is 0, returning false ; \n");
    return false;
  }

#endif

  return true;
}

/** Determine whether this interface can be deleted.
*/
bool Rtp_Interface::SupportDynamicDeletion()
{
  return GetCurrentBusyCount(CD_Both)==0;
}

/** Called to destroy this instance.
*/
void Rtp_Interface::DestroyInterface()
{
  delete this;
}


/** Get Interface Quality
*/
float Rtp_Interface::GetQuality(CallQualityParameter param, time_t start, time_t end, bool active)
{
  return _QMonitor.GetQuality(param, start, end, active);
}


/** Get RTP_Config from the RTP interface settings. */
void Rtp_Interface::GetRtpChannelConfig(RTP_Config &config)
{
  AudioCodes_Channel::AcParam2RtpConfig(channelParam, config);
}

/** See if RFC 2833 is wanted */
int Rtp_Interface::DTMFGetRfc2833()
{
  if (channelParam.GetParam().IBSSettings.DTMFTransportType == acRelayDTMFOverRTP)
    return RTP_PT_RFC_2833;
  else
    return -1;
}


///////////////////////////////////////////////////////////////////////////////

#define RTP_UPDATE_COUNTER2(FUNC2)   \
  Rtp_Media *rtp_med = (Rtp_Media *)med; \
assert(rtp_med); \
/*CLog::Detail("RTP", "%s(%d)", #FUNC2, dir);*/ \
if (rtp_med) { \
  Interface_Source *isrc = (Interface_Source *)rtp_med->GetItfSource(); \
  assert(isrc); \
  if (isrc) { \
    Interface *itf = RtpInterfaceManager::Instance()->FindInterface(isrc->boardInterfaceName); \
    assert(itf); \
    if (itf) \
    itf->FUNC2(med, dir); \
  } \
}

#define RTP_UPDATE_COUNTER3(FUNC2,arg3)   \
  Rtp_Media *rtp_med = (Rtp_Media *)med; \
assert(rtp_med); \
/*CLog::Detail("RTP", "%s(%d)", #FUNC2, dir);*/ \
if (rtp_med) { \
  Interface_Source *isrc = (Interface_Source *)rtp_med->GetItfSource(); \
  assert(isrc); \
  if (isrc) { \
    Interface *itf = RtpInterfaceManager::Instance()->FindInterface(isrc->boardInterfaceName); \
    assert(itf); \
    if (itf) \
    itf->FUNC2(med, dir, arg3); \
  } \
}


#define RTP_UPDATE_COUNTER4(FUNC2,arg3,arg4)   \
  Rtp_Media *rtp_med = (Rtp_Media *)med; \
assert(rtp_med); \
/*CLog::Detail("RTP", "%s(%d)", #FUNC2, dir);*/ \
if (rtp_med) { \
  Interface_Source *isrc = (Interface_Source *)rtp_med->GetItfSource(); \
  assert(isrc); \
  if (isrc) { \
    Interface *itf = RtpInterfaceManager::Instance()->FindInterface(isrc->boardInterfaceName); \
    assert(itf); \
    if (itf) \
    itf->FUNC2(med, dir, arg3, arg4); \
  } \
}

#define RTP_UPDATE_COUNTER5(FUNC2,arg3,arg4,arg5)   \
  Rtp_Media *rtp_med = (Rtp_Media *)med; \
assert(rtp_med); \
/*CLog::Detail("RTP", "%s(%d)", #FUNC2, dir);*/ \
if (rtp_med) { \
  Interface_Source *isrc = (Interface_Source *)rtp_med->GetItfSource(); \
  assert(isrc); \
  if (isrc) { \
    Interface *itf = RtpInterfaceManager::Instance()->FindInterface(isrc->boardInterfaceName); \
    assert(itf); \
    if (itf) \
    itf->FUNC2(med, dir, arg3, arg4, arg5); \
  } \
}



void Rtp_Interface::IncMediaCount(Media *med, CallDirection dir,CallParameter *cparam,CallCause cc,bool dontUseRLS)
{
  Interface::IncMediaCount(med, dir, cparam, cc, dontUseRLS);
  Rtp_Media* rtpmedia = (Rtp_Media*)med;
  if(rtpmedia->GetPhysicalInterface() == NULL)
    return;

  RTP_UPDATE_COUNTER5(IncMediaCount, cparam, cc, dontUseRLS)
}

void Rtp_Interface::DecMediaCount(Media *med, CallDirection dir,CallParameter *cparam,bool dontUseRLS)
{
  Interface::DecMediaCount(med, dir, cparam, dontUseRLS);

  Rtp_Media* rtpmedia = (Rtp_Media*)med;
  if(rtpmedia->GetPhysicalInterface() == NULL)
    return;

  RTP_UPDATE_COUNTER4(DecMediaCount, cparam, dontUseRLS)
}

/** Increment current busy counter.
*/
void Rtp_Interface::IncCurrentBusyCount(Media *med, CallDirection dir) 
{
  Interface::IncCurrentBusyCount(med, dir);

  // This will be updated in DecMediaCount
  //RTP_UPDATE_COUNTER2(IncCurrentBusyCount)
}

/** Decrement current busy counter.
*/
void Rtp_Interface::DecCurrentBusyCount(Media *med, CallDirection dir) 
{
  Interface::DecCurrentBusyCount(med, dir);

  // This will be updated in IncMediaCount
  //RTP_UPDATE_COUNTER2(DecCurrentBusyCount)
}

/** Increment current connected counter.
*/
void Rtp_Interface::IncCurrentConnectedCount(Media *med, CallDirection dir) 
{
  Interface::IncCurrentConnectedCount(med, dir);

  Rtp_Media* rtpmedia = (Rtp_Media*)med;
  if(rtpmedia->GetPhysicalInterface() == NULL)
    return;

  RTP_UPDATE_COUNTER2(IncCurrentConnectedCount)
}

/** Decrement current connected counter.
*/
void Rtp_Interface::DecCurrentConnectedCount(Media *med, CallDirection dir) 
{
  Interface::DecCurrentConnectedCount(med, dir);

  Rtp_Media* rtpmedia = (Rtp_Media*)med;
  if(rtpmedia->GetPhysicalInterface() == NULL)
    return;

  RTP_UPDATE_COUNTER2(DecCurrentConnectedCount)
}

/** Increment total call attempts counter.
*/
void Rtp_Interface::IncTotalCallAttempts(Media *med, CallDirection dir) 
{
  Interface::IncTotalCallAttempts(med, dir);

  // This will be updated in DecMediaCount
  //RTP_UPDATE_COUNTER2(IncTotalCallAttempts)
}

/** Increment total call connected counter.
*/
void Rtp_Interface::IncTotalCallConnected(Media *med, CallDirection dir) 
{
  Interface::IncTotalCallConnected(med, dir);

  Rtp_Media* rtpmedia = (Rtp_Media*)med;
  if(rtpmedia->GetPhysicalInterface() == NULL)
    return;

  RTP_UPDATE_COUNTER2(IncTotalCallConnected)
}

/** Increment total call succree counter.
*/
void Rtp_Interface::IncTotalCallSuccess(Media *med, CallDirection dir)
{
  Interface::IncTotalCallSuccess(med, dir);

  Rtp_Media* rtpmedia = (Rtp_Media*)med;
  if(rtpmedia->GetPhysicalInterface() == NULL)
    return;

  RTP_UPDATE_COUNTER2(IncTotalCallSuccess)
}

/** Increment total call failed counter.
*/
void Rtp_Interface::IncTotalCallFailed(Media *med, CallDirection dir)
{
  Interface::IncTotalCallFailed(med, dir);

  Rtp_Media* rtpmedia = (Rtp_Media*)med;
  if(rtpmedia->GetPhysicalInterface() == NULL)
    return;

  RTP_UPDATE_COUNTER2(IncTotalCallFailed)
}

/** Increment total call holding time.
*/
void Rtp_Interface::IncTotalCallHoldingTime(Media *med, CallDirection dir, unsigned seconds) 
{
  Interface::IncTotalCallHoldingTime(med, dir, seconds);

  Rtp_Media* rtpmedia = (Rtp_Media*)med;
  if(rtpmedia && rtpmedia->GetChannel() && rtpmedia->GetPhysicalInterface() == NULL)
    return;

  RTP_UPDATE_COUNTER3(IncTotalCallHoldingTime, seconds)
}

/** Increment total call connected time.
*/
void Rtp_Interface::IncTotalCallConnectedTime(Media *med, CallDirection dir, unsigned seconds) 
{
  Interface::IncTotalCallConnectedTime(med, dir, seconds);

  Rtp_Media* rtpmedia = (Rtp_Media*)med;
  if(rtpmedia->GetPhysicalInterface() == NULL)
    return;

  RTP_UPDATE_COUNTER3(IncTotalCallConnectedTime, seconds)
}

/** Show configuration for the specified entry.
*/
bool Rtp_Interface::OnShowConfig(const Cfg_Tree_Entry *entry, String & output, const String & indent, int indent_size) const
{
  char s[64];
  unsigned i;

  ACQUIRE_SYSTEM_LOCK();

  const acTExtChannelParam & param = channelParam.GetParam();

  // Accept specification
  /*
     if (acceptList.empty()) {
     output += indent + "# Warning: no 'accept' specifications present\n";
     } else {
     List <Accept_Entry>::const_iterator ait, aend;
     aend = acceptList.end();

     for (ait=acceptList.begin(); ait!=aend; ++ait) {
     const Accept_Entry & entry = *ait;
     strcpy(s, inet_ntoa(*(in_addr*)&entry.first));
     output += indent + "accept " + s + " " + inet_ntoa(*(in_addr*)&entry.second) + "\n";
     }
     }
     */
  if (aclName.size())
    output += indent + "access-list " + aclName + "\n";
  else
    output += indent + "no access-list\n";

  // Description
  if (GetDescription().size())
    output += indent + "description \"" + GetDescription() + "\"\n";

  // DTMF volume
  sprintf(s, "%d", param.IBSSettings.DTMFVolume);
  output += indent + "dtmf volume " + s + "\n";

  // DTMF transport
  output += indent + "dtmf transfer " + ac_id2name(ac_dtmftransfer, param.IBSSettings.DTMFTransportType) + "\n";

  // FAX transfer
  output += indent + "fax transfer " + ac_id2name(ac_faxtransfer, param.FaxModemSettings.FAXTransportType) + "\n";

  // FAX bandwidth
  output += indent + "fax bandwidth " + ac_id2name(ac_rate, param.FaxModemSettings.FaxRelayMaxRate) + "\n";

  // Max Forwards
  sprintf(s, "%u", GetMaxForwards());
  output += indent + "sip-max-forwards " + s + "\n";

  // Outgoing Strip Info
  output += indent + (IsPrivateInfoAllowed()? "no " : "") + "strip-outgoing-private-info\n";

  // RAP
  if (!RAPEnabled())
    output += indent + "group \"" + GetGroupName() + "\"\n";

  // Interface list specification
  if (itfSourceList.empty()) {
    output += indent + "# Warning: no 'interface' specifications present\n";
  } else {
    List <Interface_Source*>::const_iterator iit, iend;
    iend = itfSourceList.end();
    for (iit=itfSourceList.begin(); iit!=iend; ++iit) {
      Interface_Source *isrc = *iit;
      sprintf(s, "%d", isrc->maxCh);
      output += indent + "interface " + isrc->boardInterfaceName + " channel-count " + s + "\n";
    }
  }


  // Board list specification
#ifdef XGATE_HAS_MTNRTP
  if (brdSourceList.empty()) {
    output += indent + "# Warning: no 'board' specifications present\n";
  } else {
    List<Board_Source*>::const_iterator brdit = brdSourceList.begin();	

    for (; brdit!=brdSourceList.end(); ++brdit) {
      Board_Source* src = *brdit;
      sprintf(s, "%d", src->total_);
      output += indent + "board " + src->name_ + " channel-count " + s + "\n";
    }   
  }
#endif	// XGATE_HAS_MTNRTP

  // Interface Mode
  if (param.TransportSettings.UseNIorPCI == 1)
    output += indent + "mode network\n";
  else
    output += indent + "mode pci\n";

  // Resource locator
  output += indent + ( RAPEnabled() ? "" : "no ") + "resource-locator\n";

  // Resource-type
  output += indent + "resource-type "+GetResourceTypeStr()+"\n";

  // Mult-fact
  sprintf(s, "%d", param.VoiceSettings.M);
  output += indent + "rtp packing-factor " + s + "\n";

  // Min jitter
  sprintf(s, "%d", param.DJBSettings.DJBufMinDelay);
  output += indent + "rtp min-jitter " + s + "\n";

  // Opt fact
  sprintf(s, "%d", param.DJBSettings.DJBufOptFactor);
  output += indent + "rtp optimization-factor " + s + "\n";

  // RTP codec
  assert (payloadCount);
  output += indent + "rtp codec";
  for (i=0; i<payloadCount; ++i) {
    output += String(" ") + Rtp_Payload_GetName(payloadList[i]);
  }
  output += "\n";


  // Signalling.
#ifdef XGATE_HAS_RTP_SIGNALLING
  //    if (((Rtp_Interface*)this)->GetSignallingDriver()==RTP_Signalling::Instance())
  //	output += indent + "signalling q931\n";
#else	
  //	if (false)
  //	;//DG just incase there is no RTP signalling #ifdefs will mess up
#endif

#ifdef XGATE_HAS_SIP
  //else if (((Rtp_Interface*)this)->GetSignallingDriver()==XgSipSignalling::Instance())
  if (((Rtp_Interface*)this)->GetSignallingDriver()==XgSipSignalling::Instance())
    output += indent + "signalling sip\n";
#endif

#ifdef XGATE_HAS_SOFIA
  else if (((Rtp_Interface*)this)->GetSignallingDriver()==SofiaSIPSignalling::Instance())
    output += indent + "signalling sofia\n";
#endif

#ifndef __linux__
#ifdef XGATE_HAS_H323
  else if (((Rtp_Interface*)this)->GetSignallingDriver()==H323_Signalling::Instance())
    output += indent + "signalling h323\n";
#endif
#endif

  // SIP configuration
  List<StringPair>::const_iterator sauth_it, sauth_end;
  sauth_end = sipDigestAuthList.end();
  for (sauth_it = sipDigestAuthList.begin(); sauth_it != sauth_end; ++sauth_it) {
    const StringPair & entry = *sauth_it;
    output += indent + "sip authorize " + entry.first + " password 1 " + StringManip::StrToBase64(entry.second) + "\n";
  }
  output += indent + "sip realm \"" + sipDigestAuthRealm + "\"\n";


  // TDM input gain.
  sprintf(s, "%d", param.VoiceSettings.InputGain);
  output += indent + "tdm input-gain " + s + "\n";

  // TDM output gain
  sprintf(s, "%d", param.VoiceSettings.VoiceVolume);
  output += indent + "tdm output-gain " + s + "\n";

  // Echo canceller
  output += indent + (param.VoiceSettings.ECE==0 ? "no " : "") + "use echo-canceller\n";

  // echo tail length
  int echo_tail;
  switch (param.VoiceSettings.ECLength) {
    case acECLength15MSec:
      echo_tail = 15;
      break;
    case acECLength20MSec:
      echo_tail = 20;
      break;
    case acECLength32MSec:
      echo_tail = 32;
      break;
    case acECLength35MSec:
      echo_tail = 35;
      break;
    case acECLength40MSec:
      echo_tail = 40;
      break;
    case acECLength45MSec:
      echo_tail = 45;
      break;
    case acECLength50MSec:
      echo_tail = 50;
      break;
    case acECLength55MSec:
      echo_tail = 55;
      break;
    case acECLength60MSec:
      echo_tail = 60;
      break;
    case acECLength64MSec:
      echo_tail = 64;
      break;
    case acECLength128MSec:
      echo_tail = 128;
      break;
    case acECLength25MSec:
    default:
      echo_tail = 25;
      break;
  }
  sprintf(s, "echo-tail %d", echo_tail);
  output += indent + s + "\n";

  // G.723 Low
  output += indent + (IsUsingG7231Low()==false ? "no " : "") + "use g723-lo\n";

  // High pass filter
  output += indent + (param.VoiceSettings.HPFE==0 ? "no " : "") + "use high-pass-filter\n";

  // Post-filter
  output += indent + (param.VoiceSettings.PFE==0 ? "no " : "") + "use post-filter\n";

  // Silence compression
  output += indent + (param.VoiceSettings.SCE==0 ? "no " : "") + "use silence-compression\n";

  // Statistics interval
  sprintf(s, "%d", _statisticsInterval);
  output += indent + "stat-interval " + s + "\n";

#ifdef XGATE_HAS_ACME_SBC
  for(unsigned int i=0; i< m_sbcAddrList.size();i++) {
    output += indent + "sbc-address " + m_sbcAddrList[i] + "\n";
  }
  // MGC Address
#endif

  // Shutdown status
  output += indent + (GetState()==OS_InService && IsEnabled() ? "no " : "") + "shutdown\n";

  RELEASE_SYSTEM_LOCK();
  return OK;
}


/** Display interface specific information to the terminal user.
  Decendants MUST implement this function.
  */
void Rtp_Interface::OnShowInterface(Term_Session * session )
{
  String temp;
  char s[128];

  ACQUIRE_SYSTEM_LOCK();

#ifdef XGATE_HAS_MTNRTP
  List<Board_Source*>::iterator boardit = brdSourceList.begin();
  for (;boardit!=brdSourceList.end();boardit++) {
    Board_Source* src = *boardit;
    src->ReleaseIdleChannels();
    int free_channel = src->total_ - (src->active_.size()+src->releasing_.size());
    if (free_channel < 0)
      free_channel = 0;
    sprintf(s, SHOW_INTERFACE_INDENT "  %s: used=%3d, closing=%3d, free=%3d\n",
        src->name_.c_str(),
        src->active_.size(),
        src->releasing_.size(),
        free_channel);
    temp += s;
  }
#endif	// XGATE_HAS_MTNRTP

  List <Interface_Source*>::const_iterator iit, iend;
  iend = itfSourceList.end();
  for (iit=itfSourceList.begin(); iit!=iend; ++iit) {
    Interface_Source *isrc = *iit;
    sprintf(s, SHOW_INTERFACE_INDENT "  %s: used=%3d, free=%3d, max=%3d, status=%s\n",
        isrc->boardInterfaceName.c_str(),
        isrc->usedCh,
        isrc->GetFreeCh(),
        isrc->maxCh,
        (isrc->active ? "active" : "closing"));
    temp += s;
  }

  /*
   *	Copy channel param to temporary.
   */
  acTExtChannelParam param = channelParam.GetParam();

  /* Copy aclName
  */
  IString acl_name(aclName);

  /* Copy signalling name.
  */
  String sig_name(this->GetSignallingDriver() ? this->GetSignallingDriver()->Name() : "<unknown>");
  RELEASE_SYSTEM_LOCK();

  /* Display signalling settings.
  */
  session->Sendf( SHOW_INTERFACE_INDENT "Using signalling %s\n", sig_name.c_str());

  /* Display access list settings.
  */
  if (acl_name.empty())
    session->Sendf( SHOW_INTERFACE_INDENT "Access is not configured, interface will accept any calls\n");
  else
    session->Sendf( SHOW_INTERFACE_INDENT "Using access list '%s'\n", acl_name.c_str());

  /* Display physical interface.
  */
  session->Sendf (SHOW_INTERFACE_INDENT "Physical interface status: \n");
  session->Sendf(temp.c_str());

  /*
   * Dump channel settings.
   */

  /* Voice settings.
  */
  acTVoiceSettings & voice = param.VoiceSettings;
  session->Sendf (SHOW_INTERFACE_INDENT "Voice settings: \n"
      SHOW_INTERFACE_INDENT "  ECE=%d, SCE=%d, PFE=%d, HPFE=%d, TestMd=%d, M=%d\n"
      SHOW_INTERFACE_INDENT "  in-gain=%d, out-gain=%d, RFC2198=%d, eclength=%d, echybloss=%d\n",
      voice.ECE, voice.SCE, voice.PFE, voice.HPFE, voice.TestMode, voice.M,
      voice.InputGain, voice.VoiceVolume, voice.RTPRedundancyDepth, 
      voice.ECLength, voice.ECHybridLoss);

  int faxmodem_rate[10];
  faxmodem_rate[acRate2400bps] = 2400;
  faxmodem_rate[acRate4800bps] = 4800;
  faxmodem_rate[acRate7200bps] = 7200;
  faxmodem_rate[acRate9600bps] = 9600;
  faxmodem_rate[acRate12000bps] = 12000;
  faxmodem_rate[acRate14400bps] = 14400;

  /* Fax transfer settings.
  */ 
  acTFaxModemSettings & faxmodem = param.FaxModemSettings;
  session->Sendf (SHOW_INTERFACE_INDENT "Fax transport mode: ");
  if (faxmodem.FAXTransportType == acFaxModemTransparent)
    session->Sendf ("transparent\n");
  else if (faxmodem.FAXTransportType == acFaxModemRelay) {
    session->Sendf ("relay, encoding=%s, rate=%d bps\n"
        SHOW_INTERFACE_INDENT "  ecm=%d, t38-protect=%s, redundancy=%d, eh-redcy=%d, vol=%d\n",
        (faxmodem.UseT38orFRF11==1 ? "T38" : "FRF.11"),
        faxmodem_rate[faxmodem.FaxRelayMaxRate],
        faxmodem.FaxRelayECMEnable, 
        (faxmodem.T38FaxRelayProtectionMode==0 ? "redundancy" : "FEC"),
        faxmodem.FaxRelayRedundancyDepth,
        faxmodem.EnhancedFaxRelayRedundancyDepth,
        faxmodem.FaxModemRelayVolume);
  } else if (faxmodem.FAXTransportType == acFaxModemBypass) {
    session->Sendf ("bypass, codec=%s, M=%d\n",
        Rtp_Payload_GetName (ac_getpayloadcode(faxmodem.FaxModemBypassCoderType)),
        faxmodem.FaxModemBypassM);
  }

  /* Modem transfer settings.
  */
  session->Sendf (SHOW_INTERFACE_INDENT "Modem transport mode: \n");
  bool have_relay = faxmodem.BELLModemTransportType==acFaxModemRelay ||
    faxmodem.V21ModemTransportType==acFaxModemRelay ||
    faxmodem.V22ModemTransportType==acFaxModemRelay ||
    faxmodem.V23ModemTransportType==acFaxModemRelay ||
    faxmodem.V32ModemTransportType==acFaxModemRelay ||
    faxmodem.V34ModemTransportType==acFaxModemRelay;
  bool have_bypass = faxmodem.BELLModemTransportType==acFaxModemBypass ||
    faxmodem.V21ModemTransportType==acFaxModemBypass ||
    faxmodem.V22ModemTransportType==acFaxModemBypass ||
    faxmodem.V23ModemTransportType==acFaxModemBypass ||
    faxmodem.V32ModemTransportType==acFaxModemBypass ||
    faxmodem.V34ModemTransportType==acFaxModemBypass;

  char *transfer_type[10];
  transfer_type[acFaxModemRelay] =  "relay";
  transfer_type[acFaxModemBypass] =  "bypass";
  transfer_type[acFaxModemTransparent] =  "transparent";
  session->Sendf (SHOW_INTERFACE_INDENT "  bell=%s, v21=%s, v22=%s\n"
      SHOW_INTERFACE_INDENT "  v23=%s, v32=%s, v34=%s\n",
      transfer_type[faxmodem.BELLModemTransportType],
      transfer_type[faxmodem.V21ModemTransportType],
      transfer_type[faxmodem.V22ModemTransportType],
      transfer_type[faxmodem.V23ModemTransportType],
      transfer_type[faxmodem.V32ModemTransportType],
      transfer_type[faxmodem.V34ModemTransportType]);
  if (have_relay)
    session->Sendf (SHOW_INTERFACE_INDENT "  relay max-rate=%d bps, redundancy=%d, vol=%d\n",
        faxmodem.ModemRelayMaxRate, faxmodem.ModemRelayRedundancyDepth,
        faxmodem.FaxModemRelayVolume);

  if (have_bypass)
    session->Sendf (SHOW_INTERFACE_INDENT "  bypass codec=%s, packing-factor=%d\n",
        Rtp_Payload_GetName (ac_getpayloadcode(faxmodem.FaxModemBypassCoderType)),
        faxmodem.FaxModemBypassM);

  /* Jitter buffer settings.
  */
  acTDJBSettings & djb = param.DJBSettings;
  session->Sendf (SHOW_INTERFACE_INDENT "Jitter buffer: minumum delay=%d ms, optimization=%d\n",
      djb.DJBufMinDelay, djb.DJBufOptFactor);

  /* Transport settings.
  */
  acTTransportSettings & tr = param.TransportSettings;
  session->Sendf (SHOW_INTERFACE_INDENT "Transport: mode=%s, soft-loop=%s, dir=%s\n",
      (tr.UseNIorPCI==1 ? "network" : "pci"),
      (tr.DisableSoftIPLoopback ? "disabled" : "enabled"),
      (tr.UniDirectionalRTP==acRTPTxRx ? "both" : (tr.UniDirectionalRTP==acRTPTxOnly ? "recvonly" : "sendonly")));

  /* IBS settings */
  const char *mf_xfer[10];
  mf_xfer[acMuteDTMF] = "mute";
  mf_xfer[acRelayDTMF] = "rtp/relay";
  mf_xfer[acTransparentDTMF] = "inband";
  mf_xfer[acRelayDTMFOverRTP] = "rfc2833";
  mf_xfer[acRFC2833RelayDecoderMute] = "relay/mute";
  acTIBSSettings & ibs = param.IBSSettings;
  session->Sendf (SHOW_INTERFACE_INDENT "DTMF vol=%d, transfer=%s, MF transfer=%s\n",
      ibs.DTMFVolume, mf_xfer[ibs.DTMFTransportType], mf_xfer[ibs.MFTransportType]);
  session->Sendf (SHOW_INTERFACE_INDENT "  Detection: DTMF=%d, MFR1=%d, MFR2-fwd=%d, MFR2-bwd=%d, linesig=%d, call-prg=%d\n",
      ibs.IBSDetectorsSettings.EnableDTMFDetection,
      ibs.IBSDetectorsSettings.EnableMFR1,
      ibs.IBSDetectorsSettings.EnableMFR2Forward,
      ibs.IBSDetectorsSettings.EnableMFR2Backward,
      ibs.IBSDetectorsSettings.EnableLineSignaling,
      ibs.IBSDetectorsSettings.EnableCallProgress);

  _QMonitor.Show(session, _statisticsInterval);
}

///////////////////////////////////////////////////////////////////////////////
/*
   int  Rtp_Interface::Board_Source::ReleaseChannel(int count)
   {
   int c=0;
   for(; idle_.size()>0 && c<count;c++){
   MTN_Rtp_Channel* channel = idle_.front();
   idle_.pop_front();
   channel->SetEventCallback(NULL);
   delete channel;
   }

   return c;

   }

   void Rtp_Interface::Board_Source::AddChannel(MTN_Rtp_Channel* channel)
   {
   channel->SetEventCallback(this);
   idle_.push_front(channel);
   }
   */

#ifdef XGATE_HAS_MTNRTP
// Move channels from releasing list to idle list.
void Rtp_Interface::Board_Source::ReleaseIdleChannels()
{
  channel_l::iterator it, end = releasing_.end();
  for (it=releasing_.begin(); it!=end; ) {
    MTN_Rtp_Channel *mtn_rtp = *it;
    if (mtn_rtp->Mapletree_Channel::GetChannel()->get_state() == PortState_IDLE) {
      it = releasing_.erase(it);
      //	    MTN_Rtp_Channel_Factory::Instance()->Release(name_.c_str(), mtn_rtp);
    } else
      ++it;
  }

  if (releasing_.empty() && last_first_releasing_ch_) {
    last_first_releasing_ch_ = NULL;
    last_first_releasing_time_ = 0;
  } else if (!releasing_.empty() && last_first_releasing_ch_ == releasing_.front()) {
    if (GetTickCount() - last_first_releasing_time_ >= 500) {
      releasing_.pop_front();

      MTN_Rtp_Channel *ch = last_first_releasing_ch_;
      last_first_releasing_ch_ = 0;
      last_first_releasing_time_ = 0;

      ReReturnChannel(ch);
    }
  } 

  if (!releasing_.empty() && last_first_releasing_ch_ != releasing_.front()) {
    last_first_releasing_ch_ = releasing_.front();
    last_first_releasing_time_ = GetTickCount();
  }
}

void Rtp_Interface::Board_Source::ReturnChannel(MTN_Rtp_Channel* channel)
{
  active_.remove(channel);

  MTN_Channel *ch = channel->Mapletree_Channel::GetChannel();
  if (ch->get_state() != PortState_IDLE) {
    if (ch->get_state() == PortState_OUT_OF_SERVICE) {
      /*	    CLog::Detail(THISMODULE, "Channel %s:%s is OUT-OF-SERVICE. DSP reset is performed."
              "This will also reset the neighboring channel",
              name_.c_str(), channel->GetChannelName());*/
      ch->force_state_async(PortState_DSP_RESET);
    } else if (ch->get_state() != PortState_TERMINATING) {
      ch->force_state_async(PortState_TERMINATING);
    }
    releasing_.push_back(channel);
  } else {
    //	MTN_Rtp_Channel_Factory::Instance()->Release(name_.c_str(), channel);
  }

  ReleaseIdleChannels();
}

void Rtp_Interface::Board_Source::ReReturnChannel(MTN_Rtp_Channel* channel)
{
  active_.remove(channel);

  MTN_Channel *ch = channel->Mapletree_Channel::GetChannel();
  if (ch->get_state() != PortState_IDLE) {
    if (ch->get_state() == PortState_OUT_OF_SERVICE) {
      /*	    CLog::Detail(THISMODULE, "Channel %s:%s is OUT-OF-SERVICE. DSP reset is performed."
              "This will also reset the neighboring channel",
              name_.c_str(), channel->GetChannelName());*/
      ch->force_state_async(PortState_DSP_RESET);
    } else if (ch->get_state() != PortState_TERMINATING) {
      ch->force_state_async(PortState_TERMINATING);
    } else {
      /* State must be in TERMINATING.. */
      /*	    CLog::Detail(THISMODULE, "Channel %s:%s is still TERMINATING. State is queried.",
              name_.c_str(), channel->GetChannelName());*/
      ch->get_state_async();
    }
    releasing_.push_back(channel);
  } else {
    //	MTN_Rtp_Channel_Factory::Instance()->Release(name_.c_str(), channel);
  }
}

#endif	// XGATE_HAS_MTNRTP

///////////////////////////////////////////////////////////////////////////////

/** Terminal command callback
*/
void Rtp_Interface::DoTermCmd(Term_Session *session, bool no, Term_Cmd *cmd)
{
  CVariant v0, v1, v2, v3;
  List <Interface_Source*>::iterator it, end;
  unsigned u0, u1;

  if (!ACQUIRE_SYSTEM_LOCK()) {
    session->SendErrorf("Unable to acquire lock!\n");
    return;
  }

  switch (cmd->GetId()) {
#ifdef XGATE_HAS_MTNRTP
    case Rtp_Cmd_Board:
      {
        CVariant v1;
        CVariant v2;

        //    cout << "**** in Rtp_Cmd_Board *****"  << endl;
        cmd->GetArg(0)->GetValue(v1);
        cmd->GetArg(2)->GetValue(v2);	   	    

        int bcount = v2.AsInt();
        const char *bname = v1.AsString();

        if (no)
          bcount = 0;

        Board_Source*  bsrc;
        bsrc = NULL;

        // Find Mapletree_Module with the specified name.
        if ((strcmp(bname, "virtual") != 0) &&(strcmp(bname, "host") != 0)) {
          //		cout << "**** boardname is host *****"  << endl;
          Mapletree_Module *mod = Mapletree_Module_Manager::Instance()->FindModule(bname);
          if (mod == NULL) {
            session->SendErrorf("Unable to find the specified module (%s).\n", 
                bname);
            RELEASE_SYSTEM_LOCK();
            return;
          }
        }

        // First find board source with the name.
        List <Board_Source*>::iterator it = brdSourceList.begin(),
             end = brdSourceList.end();
        for (; it!=end; ++it) {
          Board_Source*  src = *it;
          if(src->name_ == bname) {
            bsrc = src;
            break;
          }
        }

        if (!bsrc) {
          bsrc = new Board_Source(bname);
          brdSourceList.clear();
          brdSourceList.push_back(bsrc);
        }

        if (bsrc) {
          // Move channels from releasing list to idle list.
          bsrc->ReleaseIdleChannels();

          // Modification to existing list.
          int prev_count = bsrc->total_;
          int release_count = prev_count - bcount;

          //if (GetMediaCount() - release_count < GetCapacity() - GetMediaCount()) {
          if ((int)(GetMediaCount() )< release_count) {
            release_count = /*GetCapacity() -*/ GetMediaCount();
            session->Sendf("Only %d channels can be released at this time.\n", release_count);	
          }

          // Update media count
          bsrc->total_ = bsrc->total_-release_count;//bcount;
          //		cout << " ***** boardname is host , SetMediaCount *****"  << endl;
          SetMediaCount(GetMediaCount() - release_count);

          if (bsrc->total_ == 0) {
            if (bsrc->active_.empty() && bsrc->releasing_.empty()) {
              brdSourceList.remove(bsrc);
              delete bsrc;
              session->Sendf("Board specification has been deleted successfully.\n");	
            } else {
              session->Sendf("Board specification will be deleted when all channels are freed.\n");	
            }
          } else {
            session->Sendf("Board specification has been updated successfully.\n");	
          }
        } 

        }
        break;
#endif	// XGATE_HAS_MTNRTP

        case Rtp_Cmd_Interface:
        cmd->GetArg((size_t)0)->GetValue(v0);
        cmd->GetArg((size_t)1)->GetValue(v1);
        cmd->GetArg((size_t)2)->GetValue(v2);

        Interface *itf_src;
        itf_src = RtpInterfaceManager::Instance()->FindInterface(v0.AsString());

        if (itf_src==NULL) {
          session->SendErrorf("Invalid board name '%s'\n", v0.AsString());
          goto on_return;
        }

        // Find interface source which match the board spec.
        it=itfSourceList.begin();
        end = itfSourceList.end();
        Interface_Source *isrc;
        isrc = NULL;
        for (; it!=end; ++it) {
          if ((*it)->boardInterfaceName == itf_src->GetName()) {
            isrc = *it;
            break;
          }
        }

        if (no) {
          if (!isrc) {
            session->SendErrorf("Can not find the interface specification.\n");
            goto on_return;
          }
          isrc->active = false;
          if (isrc->usedCh == 0) {
            itfSourceList.erase(it);
            // Update lastItfSourceUsed after list has been updated
            if (it == lastItfSourceUsed)
              lastItfSourceUsed = itfSourceList.begin();
            assert(GetMediaCount() >= isrc->maxCh);
            SetMediaCount(GetMediaCount() - isrc->maxCh);
            delete isrc;
            isrc = NULL;
            session->Sendf("Interface specification deleted\n");
          } else {
            session->Sendf("Interface specification will be deleted when all the channels are released.\n");
          }
        } else {
          if (!stricmp(v1.AsString(), "channel-count")) {
            int max = v2.AsInt();
            if (isrc==NULL) {
              isrc = new Interface_Source(v0.AsString(), max);
              itfSourceList.push_back(isrc);

              // Add media count
              SetMediaCount(GetMediaCount() + max);

              session->Sendf("New specification added\n");

            } else {
              if (max < isrc->usedCh) {
                session->SendErrorf("Can not set channel count below %d (currently being used)\n", isrc->usedCh);
                goto on_return;
              }

              int prev_max = isrc->maxCh;
              isrc->maxCh = max;
              isrc->active = true;

              // Update media count
              assert(GetMediaCount() + max - prev_max >= 0);
              SetMediaCount(GetMediaCount() + max - prev_max);

              session->Sendf("Specification has been updated successfully.\n");
            }
          }
        }
        break;

        case Rtp_Cmd_Signalling:
        cmd->GetArg((size_t)0)->GetValue(v0);
        if (!stricmp(v0.AsString(), "q931")) {
#ifdef XGATE_HAS_RTP_SIGNALLING
          //	    SetSignallingDriver(RTP_Signalling::Instance());
#else
          session->SendErrorf("RTP signalling is unsupported with this xGate software\n");
#endif
        } else if (!stricmp(v0.AsString(), "sip")) {
#ifdef XGATE_HAS_SIP
          SetSignallingDriver(XgSipSignalling::Instance());
#else
          session->SendErrorf("SIP signalling is unsupported with this xGate software\n");
#endif
        } else if (!stricmp(v0.AsString(), "sofia")) {
#ifdef XGATE_HAS_SOFIA
          SetSignallingDriver(SofiaSIPSignalling::Instance());
#else
          session->SendErrorf("Sofia signalling is unsupported with this xGate software\n");
#endif
        } else if (!stricmp(v0.AsString(), "h323")) {
#ifndef __linux__
#ifdef XGATE_HAS_H323
          SetSignallingDriver(H323_Signalling::Instance());
#else
          session->SendErrorf("H.323 signalling is unsupported with this xGate software\n");
#endif
#endif
        }
        break;

        case Rtp_Cmd_Max_Forwards:
        cmd->GetArg(0)->GetValue(v0);
        maxForwards=v0.AsInt();
        break;

        case Rtp_Cmd_Strip_Private_Info:
        if (no) {			
          stripPrivateInfo=false;
        } else {
          stripPrivateInfo=true;
        }
        break;

        case Rtp_Cmd_Accept:
        cmd->GetArg((size_t)0)->GetValue(v0);
        cmd->GetArg((size_t)1)->GetValue(v1);
        u0 = inet_addr(v0.AsString());
        u1 = inet_addr(v1.AsString());

        if (no) {
          Accept_Entry entry(u0, u1);
          List <Accept_Entry>::iterator it = std::find(acceptList.begin(), acceptList.end(), entry);
          if (it == acceptList.end()) {
            session->SendErrorf("The specified specification is not found.\n");
          } else {
            acceptList.erase(it);
            if (acceptList.empty()) {
              session->Sendf("Warning: interface doesn't have any accept specifications!\n");
            }
          }
        } else {
          Accept_Entry entry(u0, u1);
          List <Accept_Entry>::iterator it = std::find(acceptList.begin(), acceptList.end(), entry);
          if (it == acceptList.end()) {
            acceptList.push_back(entry);
          } else {
            session->Sendf("Duplicate entry found.\n");
          }
        }
        break;

        case Rtp_Cmd_AccessList:
        if (no) {
          aclName.erase();
        } else if (!cmd->GetArg((size_t)0)->HasValue()) {
          RELEASE_SYSTEM_LOCK();
          session->SendErrorf("Access list name required.\n");
          return;
        } else {
          cmd->GetArg((size_t)0)->GetValue(v0);
          aclName = v0.AsString();
        }
        break;

        case Rtp_Cmd_Description:
        if (no || !cmd->GetArg((size_t)0)->HasValue())
          Interface::SetDescription("");
        else {
          cmd->GetArg((size_t)0)->GetValue(v0);
          Interface::SetDescription(v0.AsString());
        }
        break;

        case Rtp_Cmd_Mode:
        cmd->GetArg((size_t)0)->GetValue(v0);
        if (!stricmp(v0.AsString(), "pci")) {
          channelParam.SetPciMode();
        } else {
          channelParam.SetNiMode();
        }
        break;

        case Rtp_Cmd_Dtmf:

        //   cout << "**** in Rtp_Cmd_dtmf *****"  << endl;

        cmd->GetArg((size_t)0)->GetValue(v0);
        cmd->GetArg((size_t)1)->GetValue(v1);

        if (!stricmp(v0.AsString(), "volume")) {
          channelParam.GetParam().IBSSettings.DTMFVolume = v1.AsInt();
        } else if (!stricmp(v0.AsString(), "length")) {
          // Not used now.
          // Only usable when we invoke acDial
        } else if (!stricmp(v0.AsString(), "interval")) {
          // Not used now.
          // Only usable when we invoke acDial
        } else if (!stricmp(v0.AsString(), "transfer")) {
          channelParam.GetParam().IBSSettings.DTMFTransportType = 
            (acTDTMFTransport) ac_name2id(ac_dtmftransfer, v1.AsString());
        } else {
          // Unknown choice, should not happen
          assert (false);
        }
        break;

        case Rtp_Cmd_Fax:
        cmd->GetArg((size_t)0)->GetValue(v0);
        cmd->GetArg((size_t)1)->GetValue(v1);

        if (!stricmp(v0.AsString(), "transfer")) {

          channelParam.GetParam().FaxModemSettings.FAXTransportType = 
            (acFaxModemTransportType) ac_name2id(ac_faxtransfer, v1.AsString());

        } else if (!stricmp(v0.AsString(), "bandwidth")) {

          channelParam.GetParam().FaxModemSettings.FaxRelayMaxRate = 
            (acTFaxModemRelayRate) ac_name2id(ac_rate, v1.AsString());

        } else {
          assert(false);
        }
        break;

        case Rtp_Cmd_Tdm:
        cmd->GetArg((size_t)0)->GetValue(v0);
        cmd->GetArg((size_t)1)->GetValue(v1);

        if (!stricmp(v0.AsString(), "input-gain")) {
          channelParam.GetParam().VoiceSettings.InputGain = v1.AsInt();
        } else if (!stricmp(v0.AsString(), "output-gain")) {
          channelParam.GetParam().VoiceSettings.VoiceVolume = v1.AsInt();
        } else {
          // Unknown choice, should not happen
          assert(false);
        }
        break;

        case Rtp_Cmd_EchoTail:
        cmd->GetArg((size_t)0)->GetValue(v0);
        if (v0.AsInt() <= 15) channelParam.GetParam().VoiceSettings.ECLength = acECLength15MSec;
        else if (v0.AsInt() <= 20) channelParam.GetParam().VoiceSettings.ECLength = acECLength20MSec;
        else if (v0.AsInt() <= 25) channelParam.GetParam().VoiceSettings.ECLength = acECLength25MSec;
        else if (v0.AsInt() <= 32) channelParam.GetParam().VoiceSettings.ECLength = acECLength32MSec;
        else if (v0.AsInt() <= 35) channelParam.GetParam().VoiceSettings.ECLength = acECLength35MSec;
        else if (v0.AsInt() <= 40) channelParam.GetParam().VoiceSettings.ECLength = acECLength40MSec;
        else if (v0.AsInt() <= 45) channelParam.GetParam().VoiceSettings.ECLength = acECLength45MSec;
        else if (v0.AsInt() <= 50) channelParam.GetParam().VoiceSettings.ECLength = acECLength50MSec;
        else if (v0.AsInt() <= 55) channelParam.GetParam().VoiceSettings.ECLength = acECLength55MSec;
        else if (v0.AsInt() <= 60) channelParam.GetParam().VoiceSettings.ECLength = acECLength60MSec;
        else if (v0.AsInt() <= 64) channelParam.GetParam().VoiceSettings.ECLength = acECLength64MSec;
        else channelParam.GetParam().VoiceSettings.ECLength = acECLength128MSec;
        break;

        case Rtp_Cmd_Use:
        cmd->GetArg((size_t)0)->GetValue(v0);

        if (!stricmp(v0.AsString(), "echo-canceller")) {
          if (no)
            channelParam.GetParam().VoiceSettings.ECE = 0;
          else
            channelParam.GetParam().VoiceSettings.ECE = 1;

        } else if (!stricmp(v0.AsString(), "high-pass-filter")) {
          if (no)
            channelParam.GetParam().VoiceSettings.HPFE = 0;
          else
            channelParam.GetParam().VoiceSettings.HPFE = 1;

        } else if (!stricmp(v0.AsString(), "post-filter")) {
          if (no)
            channelParam.GetParam().VoiceSettings.PFE = 0;
          else
            channelParam.GetParam().VoiceSettings.PFE = 1;

        } else if (!stricmp(v0.AsString(), "silence-compression")) {
          if (no)
            channelParam.GetParam().VoiceSettings.SCE = 
              (acTSilenceCompressionEnable) 0;
          else
            channelParam.GetParam().VoiceSettings.SCE = 
              (acTSilenceCompressionEnable) 1;

        } else if (!stricmp(v0.AsString(), "g723-lo")) {
          if (no)
            this->useG7231Low = false;
          else
            this->useG7231Low = true;

        } else {
          // Unknown choice, should not happen
          assert(false);
        }
        break;

        case Rtp_Cmd_Rtp:
        //	cout << " **** Inside Rtp_Cmd_Rtp *************"  << endl;
        cmd->GetArg((size_t)0)->GetValue(v0);
        cmd->GetArg((size_t)1)->GetValue(v1);

        //	cout << "***********Inside Rtp_Cmd_Rtp ,  v0 is " << v0.AsString() << endl;
        if (!stricmp(v0.AsString(), "codec")) {
          int id = ac_name2id(rtp_codec, v1.AsString());
          channelParam.GetParam().VoiceSettings.Coder = (acTCoders) id;
          payloadList[0] = ac_getpayloadcode(id);
          payloadCount = 1;

          if (cmd->GetArg(2) && cmd->GetArg(2)->HasValue()) {
            CVariant v;
            cmd->GetArg(2)->GetValue(v);
            id = ac_name2id(rtp_codec, v.AsString());
            payloadList[1] = ac_getpayloadcode(id);
            payloadCount = 2;
          }

          if (cmd->GetArg(3) && cmd->GetArg(3)->HasValue()) {
            CVariant v;
            cmd->GetArg(3)->GetValue(v);
            id = ac_name2id(rtp_codec, v.AsString());
            payloadList[2] = ac_getpayloadcode(id);
            payloadCount = 3;
          }

          if (cmd->GetArg(4) && cmd->GetArg(4)->HasValue()) {
            CVariant v;
            cmd->GetArg(4)->GetValue(v);
            id = ac_name2id(rtp_codec, v.AsString());
            payloadList[3] = ac_getpayloadcode(id);
            payloadCount = 4;
          }

        } else if (!stricmp(v0.AsString(), "packing-factor")) {
          channelParam.GetParam().VoiceSettings.M = v1.AsInt();
          _QMonitor.SetPackingFactor(v1.AsInt());
        } else if (!stricmp(v0.AsString(), "min-jitter")) {
          channelParam.GetParam().DJBSettings.DJBufMinDelay = v1.AsInt() / 10 * 10;
          _QMonitor.SetMinJitter(v1.AsInt());
        } else if (!stricmp(v0.AsString(), "optimization-factor")) {
          channelParam.GetParam().DJBSettings.DJBufOptFactor = v1.AsInt();
        } else if (!stricmp(v0.AsString(), "rtp-timeout")){
          //		cout << "****** Checking for rtp-timeout parameter ********" << endl;
          rtpTimeout = v1.AsInt();
          //		cout << "**** rtpTimeout value is "  << rtpTimeout << endl;
        }
        else{
          // Unknown choice, should not happen
          assert(false);
        }
        break;

        case Rtp_Cmd_Rap:
        if (no)
          DisableRAP();
        else
          EnableRAP();
        break;

        // resource-type
        case Rtp_Cmd_ResourceType:
        cmd->GetArg((size_t)0)->GetValue(v0);
        SetResourceTypeStr(v0.AsString());
        break;

        case Rtp_Cmd_Group:
        if (RAPEnabled()) {
          CLog::Detail( THISMODULE, "Setting group while RAP is enabled. This will override "
              "the group name configured in RAP.");
        }

        if (no)
          SetGroupName("");
        else {
          cmd->GetArg((size_t)0)->GetValue(v0);
          SetGroupName(v0.AsString());
        }
        break;

        case Rtp_Cmd_Shutdown:
        if (no) {
          if (!IsEnabled())
            Enable();

          if (GetState()!=OS_InService) {
            Start();
          } 
        } else {
          Disable(DC_Administrative);
          //session->SendErrorf("Sorry, shutdown RTP interface is not supported in this version\n");
        }
        break;

#ifdef XGATE_HAS_SIP
        case Rtp_Cmd_Sip:
        cmd->GetArg((size_t)0)->GetValue(v0);
        if (!stricmp(v0.AsString(), "authorize")) {

          // Must supply password if not removing entry.
          if (!no && !cmd->GetArg(3)->HasValue()) {
            session->SendErrorf("Password is required when inserting new entry\n");
            break;
          }

          CVariant v4;

          cmd->GetArg(1)->GetValue(v1);
          cmd->GetArg(2)->GetValue(v2);
          cmd->GetArg(3)->GetValue(v3);
          cmd->GetArg(4)->GetValue(v4);

          StringPair searched_entry;
          searched_entry.first = v1.ToString();

          if (v3.AsInt() == 0)
            searched_entry.second = v4.ToString();
          else if (v3.AsInt() == 1)
            searched_entry.second = StringManip::Base64ToStr( v4.ToString() );

          // Always remove first, to make sure no duplicate entries.
          List<StringPair>::iterator sauth_it, sauth_end;
          sauth_end = sipDigestAuthList.end();
          for (sauth_it = sipDigestAuthList.begin(); sauth_it != sauth_end; ++sauth_it) {
            const StringPair & entry = *sauth_it;
            if (entry.first == searched_entry.first)
              break;
          }
          if (sauth_it != sauth_end)
            sipDigestAuthList.erase(sauth_it);
          else if (no)
            session->SendErrorf("User entry not found\n");

          // Add a new entry, if requested
          if (no == false) {
            sipDigestAuthList.push_back(searched_entry);
            sipDigestAuthList.sort();
          }

          SipStaticAuthServer *sip_auth_server = 
            (SipStaticAuthServer*)SipAgent::Instance()->GetAuthServer();

          if (no) {
            sip_auth_server->DeleteEntry(searched_entry.first, "");
          } else {
            sip_auth_server->AddEntry(searched_entry.first, 
                searched_entry.second, 
                sipDigestAuthRealm,
                sip_auth_server->GetDefaultDomain());
          }
          //#endif
        } else if (!stricmp(v0.AsString(), "realm")) {

          cmd->GetArg(1)->GetValue(v1);

          //#ifdef XGATE_HAS_SIP
          SipStaticAuthServer *sip_auth_server = 
            (SipStaticAuthServer*)SipAgent::Instance()->GetAuthServer();
          //#endif

          if (no) {
            //#ifdef XGATE_HAS_SIP
            sipDigestAuthRealm = sip_auth_server->GetDefaultRealm();
            //#else
            sipDigestAuthRealm = "";
            //#endif
          } else
            sipDigestAuthRealm = v1.ToString();

          //#ifdef XGATE_HAS_SIP
          // Replace all auth user entries in this interface with the new realm.
          List<StringPair>::iterator sauth_it, sauth_end;
          sauth_end = sipDigestAuthList.end();
          for (sauth_it = sipDigestAuthList.begin(); sauth_it != sauth_end; ++sauth_it) {
            const StringPair & entry = *sauth_it;
            sip_auth_server->DeleteEntry(entry.first, "");
            sip_auth_server->AddEntry(entry.first, 
                entry.second, 
                sipDigestAuthRealm,
                sip_auth_server->GetDefaultDomain());

          }
        } else
          assert(false);
        break;
#endif
#ifdef XGATE_HAS_ACME_SBC
        case Rtp_Cmd_Sbc_Address:
        cmd->GetArg((size_t)0)->GetValue(v0);
        m_sbcAddrList.push_back(v0.AsString()); 
        break;
#endif
        default:
        Interface::DoTermCmd(session, no, cmd);
        break;
      }

on_return:
      RELEASE_SYSTEM_LOCK();
  }


  /** Authorize the call against configured user list.
  */
  bool Rtp_Interface::Authorize (const String & user_name) const
  {
    // Check if no authorization is required.
    if (sipDigestAuthList.empty())
      return true;

    List<StringPair>::const_iterator sauth_it, sauth_end;
    sauth_end = sipDigestAuthList.end();
    for (sauth_it = sipDigestAuthList.begin(); sauth_it != sauth_end; ++sauth_it) {
      const StringPair & entry = *sauth_it;
      if (entry.first == user_name)
        return true;
    }
    return false;
  }


  ///////////////////////////////////////////////////////////////////////////////

  /** Create a singleton instance of this class.
  */
  RtpInterfaceManager * RtpInterfaceManager::Create()
  {
    Rtcp_Monitor::Create();

    if (!rtpItfManagerInstance)
      return new RtpInterfaceManager;
    return rtpItfManagerInstance;
  }

  /** Constructor
  */
  RtpInterfaceManager::RtpInterfaceManager()
    : InterfaceManager("rtp"), ServiceProvider("rtp-itf-mgr", false, true, Lowest_Priority)
  {
    rtpItfManagerInstance = this;
    lastRtpInterfaceUsed = rtpInterfaceList.end();
  }


  RtpInterfaceManager::~RtpInterfaceManager()
  {
    rtpItfManagerInstance = NULL;
  }


  /** Handle events.
  */
  void RtpInterfaceManager::HandleEvents()
  {
    List <Rtp_Interface*>::iterator it=rtpInterfaceList.begin(), end=rtpInterfaceList.end();
    for ( ; it!=end; ++it) {
      Rtp_Interface *rtp = *it;
      rtp->HandleEvents();
    }
  }

  /** Determine whether this interface manager can create the specified
    interface name. For this class, it will return true for interface 
    'rtpXX'.
    */
  bool RtpInterfaceManager::CanCreateInterface(const char *name)
  {
    if (strncasecmp(name, "rtp", 3))
      return false;
    const char *p=name+3;
    while (*p) {
      if (!isdigit(*p++)) {
        CLog::Detail ( THISMODULE, "RTP interface must be number");
        return false;
      }
    }
    return true;
  }

  /** Create a new rtp interface.
  */
  Interface *RtpInterfaceManager::CreateInterface(const char *name)
  {
    assert (CanCreateInterface(name));

    //   cout << "************** Creating a new rtp interface "  << name << " *****"<< endl;
    // Create new interface, but convert the name to lowercase.
    String itfname = String("rtp") + (name+3);

    //   cout << "itfname is "  << itfname << endl;
    return new Rtp_Interface(itfname.c_str());
  }


  /** Dispatch a new call to the interfaces, according to their 'accept'
    specification.
    */
  Rtp_Interface *RtpInterfaceManager::DispatchIncomingCall (SignallingType sig_type,
      const ACE_INET_Addr & src_addr,
      const ACE_INET_Addr & dst_addr,
      const CallParameter & c_cparam)
  {
    List <Rtp_Interface*>::iterator it, end;
    end = rtpInterfaceList.end();

    size_t nitf = rtpInterfaceList.size();

	it = lastRtpInterfaceUsed; // added for multithreaded_ippbx

    //it = rtpInterfaceList.begin(); // commented for multithreaded_ippbx
   // lastRtpInterfaceUsed = rtpInterfaceList.begin();// commented for multithreaded_ippbx

    // Copy call parameter, change the source signal address to the 
    // IP address of the peer (not the original caller).
    CallParameter cparam = c_cparam;

    // added 4 lines below for multithreaded_ippbx
    if(cparam.csCallAgenttype == 1)
         cparam.csaCalling = cparam.csCallingContact;
    else
    {
//         cparam.csaCalling = xGateUtil::getLocalIpAddr();//xGateUtil::GetLocalIPAddress();
        cparam.csaCalling = getLocalInterface();
	cout << "cparam.csaCalling = " << cparam.csaCalling.c_str();
    }

    ACL_Token acl_token;
    CLog::Detail(THISMODULE, "ACL_Manager::Instance()->Compile starts");
////#if 0 // commented for multithreaded_ippbx
    if (!ACL_Manager::Instance()->Compile(acl_token, CD_Incoming, cparam, "", ""))
    {
      return NULL;
    }
////#endif
    CLog::Detail(THISMODULE, "ACL_Manager::Instance()->Compile ends");

    for (size_t i=0; i<nitf; ++i) {
      if (it == end)
        it = lastRtpInterfaceUsed = rtpInterfaceList.begin();
      if (it == end) {
        break;
      }

      Rtp_Interface *itf = *it;
      if(itf->IsActive()){
        	cout << "itf "<< i << " is Active" << endl;
      }
      if (itf->IsActive() && itf->CanAcceptCall(sig_type, acl_token)) {
        //lastRtpInterfaceUsed = it; // old code commented for multithreaded_ippbx
	++lastRtpInterfaceUsed; // new code added for multithreaded_ippbx
        return itf;
      }

      ++it;
    }

  // added for multithreaded_ippbx
  //TODO: This work needs to be reworked
//  it = rtpInterfaceList.begin();
 // ++it; // TODO need some more validatio for further use yadav
  //Rtp_Interface *itf = *it;
  //return itf;

    return NULL;
  }


  /**
   * This function is provided for RTP interface to find physcal VoIP
   * board with the specified name.
   */
  Interface *RtpInterfaceManager::FindInterface(const IString &itf_name)
  {
    if (!strncasecmp(itf_name.c_str(), "aud", 3)) {

      //
      // Get AudioCodes interface
      //
      const char *p = itf_name.c_str() + 3;
      char *err;
      int board_id;

      board_id = strtol(p, &err, 10);
      if (*err)
        return NULL;

      return AudioCodes_Manager::Instance()->GetInterface(board_id);

    } 

#ifdef XGATE_HAS_KALLASTRA    
    else if (!strncasecmp(itf_name.c_str(), "kal", 3)) {

      //
      // Get Kallastra interface
      //
      const char *p = itf_name.c_str() + 3;
      char *err;
      int board_id;

      board_id = strtol(p, &err, 10);
      if (*err)
        return NULL;
      /* return Kallastra_Manager::Instance()->GetInterface(board_id); */

    }
#endif

#ifdef XGATE_HAS_PROSODY_RTP
    else if (!strncasecmp(itf_name.c_str(), "prx", 3)) {
      // Get Prosody/X interface
      return ProsodyDriver::Instance()->GetInterface(itf_name.c_str());

    }
#endif //XGATE_HAS_PROSODY_RTP

    return NULL;
  }

  /**
   * This function is provided for RTP interface to allocate RTP channel
   * from the specified physical VoIP board with the specified name.
   */
  RTP_Channel *RtpInterfaceManager::AllocateChannel(const IString &itf_name, 
      CallDirection dir,
      CallParameter *cparam)
  {
    if (!strncasecmp(itf_name.c_str(), "aud", 3)) {

      //
      // Allocate AudioCodes channel
      //
      const char *p = itf_name.c_str() + 3;
      char *err;
      int board_id;

      board_id = strtol(p, &err, 10);
      if (*err)
        return NULL;

      return AudioCodes_Manager::Instance()->AllocateChannel(board_id, dir, cparam, -1);

    }

#ifdef XGATE_HAS_PROSODY_RTP
    else if (!strncasecmp(itf_name.c_str(), "prx", 3)) {
      // Allocate a Prosody/X IP chanel
      return ProsodyDriver::Instance()->AllocateChannel(itf_name.c_str());
    }
#endif //XGATE_HAS_PROSODY_RTP


    return NULL;
  }

 //// commented following code : multithreaded_ippbx
bool Rtp_Interface::allocate_interface(Call *call, Media *&media, CallDirection dir, CallParameter *cparam, char *epName)
{
  media = NULL;
  char media_name[50]={0};

  if(epName != NULL)  {
    strcpy(media_name,epName);
  } 
 if(GetMediaCount() <= 0){
     CLog::Detail(THISMODULE, "RTP Interface count Exhausted no more channel available for allocation %s",cparam->call_id.c_str());
     return false;
   }
  Remote_Rtp_Channel *hostChannel = new Remote_Rtp_Channel(media_name); //TODO: Yoga, can we avoid this object creation?
  //Yadav TODO need revist this line ****************
  //Host_Rtp_Channel *hostChannel = new Host_Rtp_Channel(media_name, false); //TODO: Yoga, can we avoid this object creation?
  if(hostChannel != NULL) {
    media = new Rtp_Media(media_name, (MediaId)hostChannel->GetMediaID(), this, (Interface *)this, hostChannel, NULL);
  }
  if(media == NULL)
  {
    CLog::Detail(THISMODULE, "Rtp_Interface::allocate_interface return NULL while Allocate H248Media, return fail");
    return false;
  }
  CLog::Detail(THISMODULE, "Rtp_Interface::allocate_interface Allocate H248Media,: Return SUCCESS: MediaName(ep_name) %s \n",media_name);

  if (media) {
    //printf("Rtp_Interface::allocate_interface med = %02x \n", media);
    DecMediaCount(media, dir, cparam);//SRI CLI ISSUE SRI ADDED THESE TWO LINES
    return true;
  } else {
    CLog::Detail(THISMODULE, "Rtp_Interface::allocate_interface %s: Unable to allocate RTP media channel for %s call",
        GetName(), (dir==CD_Outgoing ? "outgoing" : "incoming"));
    return false;
  }
  return true;
}



  ///////////////////////////////////////////////////////////////////////////////

  enum {
    Rtcp_Cmd
  };

  Rtcp_Monitor *Rtcp_Monitor::theInstance;

  Rtcp_Monitor * Rtcp_Monitor::Create()
  {
    if (theInstance==NULL)
      theInstance = new Rtcp_Monitor;
    return theInstance;
  }

  Rtcp_Monitor::Rtcp_Monitor()
    : ServiceProvider("rtcp", false, false), lastClearCounter(time(NULL))
  {
  }

  Rtcp_Monitor::~Rtcp_Monitor()
  {
    theInstance = NULL;
  }

  bool Rtcp_Monitor::Load()
  {
    Term_Cmd *cmd;
    cmd = new Term_Cmd(this, Rtcp_Cmd, 
        "rtcp", "RTCP monitoring",
        PrivilegeLevelAdmin, TermModeConfig, true);
    CVariantChoiceValidator rtcpVdtor("monitor", "Add new monitoring specification");
    rtcpVdtor.Add("show", "Show RTCP monitoring results");
    cmd->Add(new Term_Cmd_Arg("@0", "", rtcpVdtor));

    cmd->AddDynamic(1, 0, "monitor", new Term_Cmd_Arg("@1", "", CVariantValidator("", "IP address")));
    cmd->AddDynamic(2, 0, "monitor", new Term_Cmd_Arg("@2", "", CVariantValidator("", "Address mask")));
    //cmd->AddDynamic(3, 0, "monitor", new Term_Cmd_Arg("@3", "persistent", CVariantSingleValueValidator("persistent", "Keep average data of previous calls"), TermArgOptional));
    RegisterTermCmd(cmd, Kernel::Instance()->RootConfig());

    Kernel::Instance()->RootConfig()->RegisterShowHandler(this);
    return true;
  }

  bool Rtcp_Monitor::Unload()
  {
    return true;
  }

  void Rtcp_Monitor::DoTermCmd(Term_Session *session, bool no, Term_Cmd *cmd)
  {
    CVariant arg0, arg1, arg2, arg3;

    switch (cmd->GetId()) {
      case Rtcp_Cmd:
        cmd->GetArg((size_t)0)->GetValue(arg0);
        if (!stricmp(arg0.AsString(), "monitor")) {
          cmd->GetArg((size_t)1)->GetValue(arg1);
          cmd->GetArg((size_t)2)->GetValue(arg2);

          SpecData spec;
          spec.addr = inet_addr(arg1.AsString());
          spec.mask = inet_addr(arg2.AsString());
          ACQUIRE_SYSTEM_LOCK();
          List<SpecData>::iterator it = std::find(specList.begin(), specList.end(), spec);
          bool found = it != specList.end();
          if (found) {
            if (no) {
              specList.erase(it);
            } else {
              RELEASE_SYSTEM_LOCK();
              session->SendErrorf("the specification is already exist\n");
              return;
            }
          } else {
            if (no) {
              RELEASE_SYSTEM_LOCK();
              session->SendErrorf("the specification was not found\n");
              return;
            } else {
              specList.push_back(spec);
            }
          }
          RELEASE_SYSTEM_LOCK();
        } else
          Show(session);
        break;
    }
  }

  bool Rtcp_Monitor::OnShowConfig(const Cfg_Tree_Entry *entry,
      String & output, 
      const String & indent, 
      int indent_size) const
  {
    List<SpecData>::const_iterator it, end;
    end = specList.end();
    for (it=specList.begin(); it!=end; ++it) {
      const SpecData & spec = *it;
      String addr = inet_ntoa(*(in_addr*)&spec.addr);
      const char *mask = inet_ntoa(*(in_addr*)&spec.mask);
      output += indent + "rtcp monitor " + addr + " " + mask + "\n";
    }
    output += "\n";
    return OK;
  }

  void Rtcp_Monitor::PrintList(Term_Session *session, const char *title, const List<SpecData> & data)
  {
    session->Sendf("%s:\n", title);
    List<SpecData>::const_iterator it, end;
    it = data.begin();
    end = data.end();

    if (it==end) {
      session->Sendf(" - no entry\n");
      return;
    }

    session->Sendf("                            Outgoing Stream            Incoming Stream\n"
        "   IP addr & mask   Calls Pkts Jitt RTT  FrLo ToLo | Pkts Jitt RTT  FrLo ToLo\n"
        "  -------------------------------------------------+--------------------------\n");
    for (; it != end; ++it)
      PrintData(session, *it);
  }

  // print number fraction in 4 digits space
  static const char * print_fraction(double num, char *s)
  {
    int val = num;
    if (num < 0 || num > 1000000) {
      strcpy(s, "  -1");
    } else if (num < 0.00001) {
      strcpy(s, "   0");
    } else if (num < 1) {
      int val1000 = num * 1000;
      sprintf(s, ".%03d", val1000);
    } else if (num < 10000)
      sprintf(s, "%4d", val);
    else if (num >= 1000 && num < 1000000)
      sprintf(s, "%3dK", val / 1000);
    else
      sprintf(s, "%3dM", val / 1000000);

    return s;
  }

  void Rtcp_Monitor::PrintData(Term_Session *session, const SpecData & data)
  {
    char s[30], s_packet_count[30], s_jitter[30], s_round_trip[30], s_fraction_lost[30], s_cum_lost[30];
    const AcRtcpStat *r = &data.stat;

    in_addr masked;
    masked.s_addr = data.addr & data.mask;
    session->Sendf("  %15s    %s ", inet_ntoa(masked), print_fraction(data.ncalls, s));
    session->Sendf("%s %s %s%s%%%s%% | ",print_fraction(r->tx.packetCount, s_packet_count),
        print_fraction(r->tx.jitter / 1000.0, s_jitter),
        print_fraction(r->tx.roundTrip/1000.0, s_round_trip),
        print_fraction(r->tx.fractionLost*100, s_fraction_lost),
        print_fraction(r->tx.cummulativeLost * 100.0/r->tx.packetCount, s_cum_lost));
    session->Sendf("%s %s %s%s%%%s%%\n",print_fraction(r->rx.packetCount, s_packet_count),
        print_fraction(r->rx.jitter / 1000.0, s_jitter),
        print_fraction(r->rx.roundTrip/1000.0, s_round_trip),
        print_fraction(r->rx.fractionLost*100, s_fraction_lost),
        print_fraction(r->rx.cummulativeLost * 100.0/r->rx.packetCount, s_cum_lost));
  }

  void Rtcp_Monitor::RecordChStat(List<SpecData> & persistent, AcChannel *ch)
  {
    List<SpecData>::iterator it, end;
    it = persistent.begin();
    end = persistent.end();

    for (; it!=end; ++it) {
      SpecData & data = *it;
      unsigned long masked_spec = data.addr & data.mask;
      unsigned long addr_masked = ch->GetRemoteAddress().sin_addr.s_addr & data.mask;

      if (masked_spec != addr_masked)
        continue;

      data.stat.AddAverage(data.ncalls++, ch->GetRtcpStat());
    }
  }

  void Rtcp_Monitor::OnChannelClosing(AcChannel *ch)
  {
    time_t now = time(NULL);

    // Only records statistics if activation time is greater than 10 seconds and
    // total number of transmitted/received is greater than 1000
    if (ch->GetInitializationStatus()==2 && now-ch->GetLastActivateRtpTime() > 10 &&
        (ch->GetRtcpStat().tx.packetCount >= 1000 || ch->GetRtcpStat().rx.packetCount >= 1000)) {
      RecordChStat(specList, ch);
    }
  }

  void Rtcp_Monitor::Show(Term_Session *session)
  {
    // Print persistent data
    ACQUIRE_SYSTEM_LOCK();
    List<SpecData> persistent = specList;
    time_t last_clear = lastClearCounter;
    RELEASE_SYSTEM_LOCK();

    session->Sendf("RTCP average statistics\n"
        "  Calls   number of calls recorded in the statistic\n"
        "          (only calls which have been connected for some time are recorded)\n"
        "  Pkt     total packets transmitted/received in the direction\n"
        "  Jitt    RTP packet jiter (in msec)\n"
        "  RTT     round trip time\n"
        "  FrLo    fraction of packet lost\n"
        "  ToLo    total packet lost\n\n");

    if (last_clear == 0)
      session->Sendf("Last clear counter: never\n");
    else {
      time_t now = time(NULL);
      String clear_time = ctime(&last_clear);
      clear_time.erase(clear_time.size()-1);
      String interval = ConvertInterval2String(now - last_clear);
      session->Sendf("Last clear counter: %s [%s ago]\n", clear_time.c_str(), interval.c_str());
    }

    PrintList(session, "Previous Calls", persistent);

    List<SpecData>::iterator it, end;
    end = persistent.end();
    for (it=persistent.begin(); it!=end; ++it) {
      SpecData & data = *it;
      data.ncalls = 0;
      data.stat.Reset();
    }

    // Scan all active channels.
    // For channels that have been activated for more than 10 seconds, add them
    // to the list.
    unsigned nboard = AcLibrary::Instance()->GetNumberOfBoards();
    time_t now = time(NULL);

    for (unsigned iboard=0; iboard<nboard; ++iboard) {
      ACQUIRE_SYSTEM_LOCK();

      AcBoard *board = AcLibrary::Instance()->GetBoard(iboard);
      if (!board || board->GetInitializationStatus() != 1) {
        RELEASE_SYSTEM_LOCK();
        continue;
      }

      unsigned nchan = board->GetNumberOfChannel();
      for (unsigned ichan=0; ichan < nchan; ++ichan) {
        AcChannel *ch = board->GetChannel(ichan);
        if (!ch || ch->GetInitializationStatus()!=2 || now-ch->GetLastActivateRtpTime() < 10)
          continue;
        RecordChStat(persistent, ch);
      }

      RELEASE_SYSTEM_LOCK();
    }

    PrintList(session, "\nActive Calls", persistent);
  }


  /** Clear RTCP monitor counter.
  */
  void Rtcp_Monitor::OnClearCounter()
  {
    //ACQUIRE_SYSTEM_LOCK();
    List<SpecData>::iterator it, end;
    end = specList.end();
    for (it=specList.begin(); it!=end; ++it) {
      SpecData & data = *it;
      data.ncalls = 0;
      data.stat.Reset();
    }
    lastClearCounter = time(NULL);
    //RELEASE_SYSTEM_LOCK();
  }

#endif // #ifdef XGATE_HAS_AUDIOCODES
