/* $Header: /xGate/xgconfig.h 32    1/27/05 1:06p Bennylp $
 *   
 * XGATE
 * (C) 2001 Switchlab, Ltd
 *
 * XGATE CONFIGURATION
 *
 * $Log: /xGate/xgconfig.h $
 * 
 * 32    1/27/05 1:06p Bennylp
 * 
 * 31    1/25/05 7:08p Bennylp
 * New layout for easier release.
 * 
 * 30    3/08/04 16:24 Sonny
 * 
 * 29    1/09/04 12:09p Sonny
 * 
 * 28    7/14/03 2:07p Bennylp
 * Version 54.6
 * 
 * 27    5/08/03 11:33a Bennylp
 * Added option to include/exclude HTTP
 * 
 * 26    4/24/03 6:00p Bennylp
 * Fix name inconsistency: XG_HAS_ changed to XGATE_HAS_
 * 
 * 25    4/24/03 2:17p Bennylp
 * Added modem config
 * 
 * 24    3/01/03 7:04p Bennylp
 * 
 * 23    11/01/02 8:03p Bennylp
 * Added announce resource.
 * 
 * 22    4/10/02 11:11 Adham
 * Added XGATE_H323_V2
 * 
 * 21    9/23/02 2:47p Bennylp
 * Added XGATE_USE_NEW_RTP
 * 
 * 20    9/05/02 4:04p Bennylp
 * Change text name for new ATM.
 * 
 * 19    9/02/02 4:37p Sonny
 * 
 * 18    5/08/02 19:06 Adham
 * Add MTN RTP pre processor
 * 
 * 17    5/03/02 7:58p Bennylp
 * Put back default LM is disabled.
 * 
 * 16    2/05/02 19:43 Junanto
 * 
 * 15    4/23/02 7:01p Bennylp
 * For now, by default XGATE_HAS_LM is off
 * 
 * 14    23/04/02 18:59 Iwanj
 * add XGATE_HAS_LM
 * 
 * 13    4/23/02 6:41p Bennylp
 * Disable H.323 and SIP by default
 * 
 * 12    18/04/02 10:51 Adham
 * 
 * 11    4/04/02 3:33p Bennylp
 * Version 38.19
 * 
 * 10    3/04/02 11:05a Bennylp
 * Rename PPPOE macro
 * 
 * 9     20/02/02 17:28 Sonny
 * 
 * 8     20/02/02 17:10 Sonny
 * add echocanceller and poweraccess
 * 
 * 7     20/02/02 15:28 Sonny
 * 
 * 6     20/02/02 15:26 Sonny
 * 
 * 5     2/15/02 3:41p Bennylp
 * Mapletree integration to xGate
 * 
 * 4     11/01/02 12:10 Adham
 * 
 * 3     12/17/01 7:02p Bennylp
 * Added initial/experimental support for Mapletree modules.
 * 
 * 2     8/28/01 7:57p Bennylp
 * Option to include/exclide H.323, SIP, and ATM in compilation.
 * 
 * 1     8/28/01 6:39p Bennylp
 */

#if !defined __XGATE_CONFIG_H__
#define __XGATE_CONFIG_H__


/* Uncomment one of these releases */
#define XGATE_RELEASE XGATE_RELEASE_LINUX

/******************************************************************************
 * Normally doesn't need to modify anything beyond this point, 
 * except when adding new functionalities.
 */

#define XGATE_RELEASE_LINUX 10  // XGATE on Linux Platform


#if XGATE_RELEASE == XGATE_RELEASE_LINUX

    #   define XGATE_HAS_SIP
//////    #   define XGATE_HAS_SOFIA // commented for ippbx
    #   define XGATE_HAS_SIP_IPPBX
    #   define XGATE_HAS_DNSSERVICE
    #   define XGATE_USE_NEW_RTP
    #   define XGATE_HAS_MTN            // needed for virtual RTP
    #   define XGATE_HAS_MTNRTP         // needed for virtual RTP
    #   define XGATE_HAS_HMP		//host media processing
    //#   define XGATE_HAS_HTTP_SERVER
    //#   define XGATE_HAS_HTTP_API
    #   define XGATE_HAS_AUDIOCODES //DG added
    #   define XGATE_HAS_NEW_AUDIOCODES
    //#   define XGATE_HAS_RTP_SIGNALLING //DG added
    #   define XGATE_HAS_PIPE   //DG added
    #   define XGATE_HAS_RTP_INTERFACE//DG added
  //  #   define XGATE_HAS_PROSODY_RTP
    //#   define XGATE_HAS_ISUP           //DG added
//    #   define XGATE_HAS_ACULAB         //DG added
 //   #   define XGATE_HAS_PROSODY      //DG added
	//   # define XGATE_HAS_VOICE          //DG added
  //  #   define XGATE_HAS_ACULABSWITCH   //DG added
    //#   define XGATE_HAS_VLINKAGENT
    #   define XGATE_HAS_FILE_DRIVER //DG added
    #   define XGATE_HAS_DB_DRIVER//DG added
    #   define XGATE_HAS_ACME_SBC
    # define XGATE_HAS_GSTREAMER
//    #   define XGATE_HAS_MGC_MG_COMMUNICATION 
#else

    #	error "xGate release is unknown!"

#endif



inline const char *Xgate_Get_Feature_Info()
{
    return ""

#ifdef XGATE_HAS_FILE_DRIVER
	"  File Driver\n"
#endif

#ifdef XGATE_HAS_DB_DRIVER
	"  Database Driver\n"
#endif

#ifdef XGATE_HAS_ACME_SBC
  " ACME SBC Support\n"
#endif

#ifdef XGATE_HAS_PIPE
	"  Pipe Driver\n"
#endif
#ifdef XGATE_HAS_ISUP
	"  ISUP\n"
#endif

#ifdef XGATE_HAS_ACULAB
	"  Aculab\n"
#endif

#ifdef XGATE_HAS_PROSODY
	"  Prosody\n"
#endif

#ifdef XGATE_HAS_VOICE
	"  Voice\n"
#endif

#ifdef XGATE_HAS_ASR
	"  Automatic Speech Recognition\n"
#endif

#ifdef XGATE_HAS_RTP_SIGNALLING
	//"  Proprietary VOIP signalling (q931oIP)\n"
#endif


#ifdef XGATE_HAS_RADH323
#ifdef XGATE_H323_V2
	"  H.323 signalling Version 2 (type 1)\n"
#else
	"  H.323 signalling (RadVision)\n"
#endif
#endif

#ifdef XGATE_HAS_OPENH323
	"  H.323 signalling Version 2 (type 2 v1.19.0)\n"
#endif

#ifdef XGATE_HAS_SIP
	"  Session Initiation Protocol signalling (SIP)\n"
#endif

#ifdef XGATE_HAS_SOFIA
	"  SIP signalling using Sofia\n"
#endif

#ifdef XGATE_HAS_DNSSERVICE
	"  Asynchronous A and SRV DNS Cliend\n"
#endif

#ifdef XGATE_USE_NEW_RTP
	"  RTP Framework v2.0\n"
#endif


#ifdef XGATE_HAS_ATM
#ifdef XGATE_NEW_ATM_LIB
	"  ATM interface (ATM Lib 2.0)\n"
#else
	"  ATM interface\n"
#endif
#endif


#ifdef XGATE_HAS_MTN
	"  Mapletree Networks modules\n"
#endif

#ifdef XGATE_HAS_PPPoE
	"  PPPoE on Ethernet support\n"
#endif

#ifdef XGATE_HAS_GATEKEEPER
	"  Gatekeeper\n"
#endif

#ifdef XGATE_HAS_EC
	"  Echo Canceller\n"
#endif

#ifdef XGATE_HAS_POWERACCESS
	"  IML PowerAccess boards\n"
#endif

#ifdef XGATE_HAS_MTNRTP
	"  Mapletree RTP\n"
#endif

#ifdef XGATE_HAS_NEW_AUDIOCODES	
	"  New AudioCodes\n"
#endif

#ifdef XGATE_HAS_LM
	"  Licence Management\n"
#endif

#ifdef XGATE_HAS_ANNOUNCE_RES
	"  Announcement resource\n"
#endif

#ifdef  XGATE_HAS_MODEM
	"  Modem Pool Resource\n"
#endif

#ifdef XGATE_HAS_BUGSLAYER
	"  Bug-Slayer (version 1.0)\n"
#endif

#ifdef XGATE_HAS_HTTP_API
	"  HTTP API\n"
#endif

	

#ifdef XGATE_HAS_TRUNK
	"  Trunking Framework\n"
#endif



#ifdef XGATE_HAS_PROSODY_RTP
	"  Prosody/X RTP\n"
#endif

#ifdef  XGATE_HAS_GSMMAP
	"  GSM MAP driver\n"
#endif
#ifdef  XGATE_HAS_BSSAP
	"  GSM BSSAP driver\n"
#endif

#ifdef  XGATE_HAS_INAP_ROUTER
	"  GSM INAP-router driver\n"
#endif

	;
}   


#ifdef  XGATE_HAS_ACULAB
#define XGATE_HAS_ACULABSWITCH
#endif

#ifdef XGATE_HAS_ISUP
#define XGATE_HAS_VLINKAGENT
#endif


//#ifdef dependencies checking
#ifdef  XGATE_HAS_ISUP
	#if !defined (XGATE_HAS_ACULAB) && !defined (XGATE_HAS_DIALOGIC_DTI)
	#error "ISUP depends on Aculab or Dialogic Digital Telephony"
	#endif
#endif

#ifdef  XGATE_HAS_PROSODY
	#ifndef XGATE_HAS_ACULABSWITCH
	#error "Prosody depends on Aculab Switch"
	#endif
#endif

#ifdef  XGATE_HAS_VOICE
//	#if !defined (XGATE_HAS_DIALOGIC) && !defined (XGATE_HAS_PROSODY)
//	#error "VOICE depends on Prosody OR Dialogic"
//	#endif
#endif

#ifdef  XGATE_HAS_NEW_AUDIOCODES
	#ifndef XGATE_HAS_AUDIOCODES 
	#error "New Audiocodes depends on Audiocodes"
	#endif
#endif

#ifdef  XGATE_USE_NEW_RTP
	#ifndef XGATE_HAS_NEW_AUDIOCODES 
	#error "Rtp Interface depends on New Audiocodes"
	#endif
#endif

#ifdef  XGATE_HAS_PROSODY_RTP
	//#if !defined(XGATE_HAS_PROSODY) || !defined(XGATE_USE_NEW_RTP) || !defined(XGATE_HAS_RTP_SIGNALLING) || !defined(XGATE_HAS_ACULABSWITCH)
	#if !defined(XGATE_HAS_PROSODY) || !defined(XGATE_USE_NEW_RTP) || !defined(XGATE_HAS_ACULABSWITCH)
	#error "Prosody RTP depends on Aculab Switch, Prosody and RTP"
	#endif
#endif


#endif // __XGATE_CONFIG_H__
