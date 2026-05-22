/*----------------------------------------------------------------* 
 * $Archive: /xGate/prosody.cpp $
 * $Date: 2/01/05 5:48p $
 * $Revision: 51 $
 *
 * $History: prosody.cpp $
 * 
 * *****************  Version 51  *****************
 * User: Bennylp      Date: 2/01/05    Time: 5:48p
 * Updated in $/xGate
 * grunt  latency set to 2 seconds
 * 
 * *****************  Version 50  *****************
 * User: Junanto      Date: 1/02/05    Time: 16:23
 * Updated in $/xGate
 * 
 * *****************  Version 49  *****************
 * User: Junanto      Date: 1/02/05    Time: 15:55
 * Updated in $/xGate
 * added API WaitForSilence (only for Prosody)
 * 
 * *****************  Version 48  *****************
 * User: Bennylp      Date: 12/07/04   Time: 2:27p
 * Updated in $/xGate
 * Added support for serial number registering in LM
 * 
 * *****************  Version 47  *****************
 * User: Sonny        Date: 1/22/03    Time: 10:28a
 * Updated in $/xGate
 * Implemented VoxPlayBkgnd functionality
 * 
 * *****************  Version 46  *****************
 * User: Bennylp      Date: 10/22/02   Time: 5:14p
 * Updated in $/xGate
 * Keep track of thread creation ("show threads")
 * 
 * *****************  Version 45  *****************
 * User: Junanto      Date: 24/06/02   Time: 21:25
 * Updated in $/xGate
 * enable dtmf recording if interrupt is disabled
 * 
 * *****************  Version 44  *****************
 * User: Junanto      Date: 17/05/02   Time: 11:07
 * Updated in $/xGate
 * PlayDtmf and PlayTone is now interruptible
 * 
 * *****************  Version 43  *****************
 * User: Junanto      Date: 30/04/02   Time: 17:39
 * Updated in $/xGate
 * Aculab switching changed:
 * - added option "looped back when idle" for Prosody
 * - at destruction, put back original switching mode
 * 
 * *****************  Version 42  *****************
 * User: Junanto      Date: 8/03/02    Time: 19:02
 * Updated in $/xGate
 * 
 * *****************  Version 41  *****************
 * User: Junanto      Date: 25/10/01   Time: 11:20
 * Updated in $/xGate
 * Put back event based dtmf recognition
 * 
 * *****************  Version 40  *****************
 * User: Junanto      Date: 24/10/01   Time: 15:16
 * Updated in $/xGate
 * A bit of optimization during CheckPlayDtmf/CheckPlayTone
 * 
 * *****************  Version 39  *****************
 * User: Junanto      Date: 22/10/01   Time: 19:30
 * Updated in $/xGate
 * Removed call to WaitForSingleObjects
 * 
 * *****************  Version 38  *****************
 * User: Sonny        Date: 22/10/01   Time: 11:25
 * Updated in $/xGate
 * bug fixes on conference and echocanceller
 * 
 * *****************  Version 37  *****************
 * User: Junanto      Date: 19/10/01   Time: 13:35
 * Updated in $/xGate
 * sm_reset_channel is no longer called during allocation
 * since it is a blocking and long process
 * 
 * *****************  Version 36  *****************
 * User: Sonny        Date: 18/10/01   Time: 11:59
 * Updated in $/xGate
 * add echocanceller
 * 
 * *****************  Version 35  *****************
 * User: Junanto      Date: 17/10/01   Time: 14:58
 * Updated in $/xGate
 * Attempt to optimize events polling method (polling only if
 * necessary)
 * 
 * *****************  Version 34  *****************
 * User: Junanto      Date: 16/10/01   Time: 20:27
 * Updated in $/xGate
 * Priority set to very low
 * 
 * *****************  Version 33  *****************
 * User: Sonny        Date: 18/09/01   Time: 21:50
 * Updated in $/xGate
 * Conferece device does not detect dtmf
 * 
 * *****************  Version 32  *****************
 * User: Sonny        Date: 12/09/01   Time: 13:01
 * Updated in $/xGate
 * add conferencing capability
 * 
 * *****************  Version 31  *****************
 * User: Sonny        Date: 6/09/01    Time: 13:56
 * Updated in $/xGate
 * temporary checkin for kholis
 * 
 * *****************  Version 30  *****************
 * User: Junanto      Date: 16/08/01   Time: 21:12
 * Updated in $/xGate
 * Integrated with DeviceUsage class
 * 
 * *****************  Version 29  *****************
 * User: Junanto      Date: 27/06/01   Time: 17:40
 * Updated in $/xGate
 * Changed prosody device naming
 * 
 * *****************  Version 28  *****************
 * User: Junanto      Date: 27/06/01   Time: 17:37
 * Updated in $/xGate
 * Fixed error handling during hardware error in PlayFile
 * 
 * *****************  Version 27  *****************
 * User: Junanto      Date: 8/06/01    Time: 10:37
 * Updated in $/xGate
 * Removed some CLog::Detail
 * 
 * *****************  Version 26  *****************
 * User: Junanto      Date: 4/06/01    Time: 13:53
 * Updated in $/xGate
 * Added new command tone and priority set to Low
 * 
 * *****************  Version 25  *****************
 * User: Junanto      Date: 3/06/01    Time: 21:07
 * Updated in $/xGate
 * Fixed bug when a completed PlayFile is cancelled
 * 
 * *****************  Version 24  *****************
 * User: Junanto      Date: 2/06/01    Time: 23:27
 * Updated in $/xGate
 * Fixed bug when a Prosody device is cancelled
 * twice
 * 
 * *****************  Version 23  *****************
 * User: Junanto      Date: 2/06/01    Time: 15:25
 * Updated in $/xGate
 * 
 * *****************  Version 22  *****************
 * User: Junanto      Date: 2/06/01    Time: 14:36
 * Updated in $/xGate
 * Fixed minor bugs during cancel job
 * 
 * *****************  Version 21  *****************
 * User: Junanto      Date: 2/06/01    Time: 1:09
 * Updated in $/xGate
 * replay_start is kicked from the I/O thread instead
 * from the main thread
 * 
 * *****************  Version 20  *****************
 * User: Junanto      Date: 2/06/01    Time: 0:47
 * Updated in $/xGate
 * Changed PlayFile implementation to multi-threading
 * 
 * *****************  Version 19  *****************
 * User: Junanto      Date: 1/06/01    Time: 17:09
 * Updated in $/xGate
 * Some little bug fixes
 * 
 * *****************  Version 18  *****************
 * User: Junanto      Date: 31/05/01   Time: 19:17
 * Updated in $/xGate
 * Removed multi-thread architecture for
 * asynchronous play dtmf
 * 
 * *****************  Version 17  *****************
 * User: Junanto      Date: 31/05/01   Time: 15:20
 * Updated in $/xGate
 * AsyncThread set to 32 and no semaphore used to 
 * start a job
 * 
 * *****************  Version 16  *****************
 * User: Bennylp      Date: 5/30/01    Time: 6:27p
 * Updated in $/xGate
 * More restrictive use of system lock (i.e. recursive is not allowed)
 * 
 * *****************  Version 15  *****************
 * User: Junanto      Date: 27/05/01   Time: 18:43
 * Updated in $/xGate
 * - Fixed bug during DetachTask resulting from "kill"
 * - Added parameter dtmf-detection
 * 
 * *****************  Version 14  *****************
 * User: Junanto      Date: 27/05/01   Time: 17:11
 * Updated in $/xGate
 * Fixed bug when a prosody is cancelled during 
 * GetDigits
 * 
 * *****************  Version 13  *****************
 * User: Bennylp      Date: 5/21/01    Time: 7:49p
 * Updated in $/xGate
 * Fix: ignore initialization error in Load() when there's no board.
 * 
 * *****************  Version 12  *****************
 * User: Junanto      Date: 21/05/01   Time: 14:43
 * Updated in $/xGate
 * Inidividual Prosody module is now configurable
 * 
 * *****************  Version 11  *****************
 * User: Junanto      Date: 17/05/01   Time: 12:56
 * Updated in $/xGate
 * Changed some xlat table to static
 * 
 * *****************  Version 10  *****************
 * User: Junanto      Date: 8/05/01    Time: 20:28
 * Updated in $/xGate
 * Unload quits if not initialized
 * 
 * *****************  Version 9  *****************
 * User: Junanto      Date: 8/05/01    Time: 19:45
 * Updated in $/xGate
 * Debug mode uses less threads
 * 
 * *****************  Version 8  *****************
 * User: Junanto      Date: 8/05/01    Time: 19:21
 * Updated in $/xGate
 * On Unload, the driver waits until all threads
 * are terminated or timeout
 * 
 * *****************  Version 7  *****************
 * User: Sonny        Date: 5/01/01    Time: 6:16a
 * Updated in $/xGate
 * Changed default volume attenuation to 0 db
 * 
 * *****************  Version 6  *****************
 * User: Junanto      Date: 30/04/01   Time: 11:33
 * Updated in $/xGate
 * Corrected some log display
 * 
 * *****************  Version 5  *****************
 * User: Junanto      Date: 29/04/01   Time: 15:05
 * Updated in $/xGate
 * added parameter need-voice in 
 * application to configure voice
 * resource requirement per 
 * application
 * 
 * *****************  Version 4  *****************
 * User: Junanto      Date: 24/04/01   Time: 20:04
 * Updated in $/xGate
 * added mecanism to deallocate voice device by VoiceManager 
 * and release the switching resource
 * 
 * *****************  Version 3  *****************
 * User: Junanto      Date: 24/04/01   Time: 18:25
 * Updated in $/xGate
 * first link between Prosody/Aculab Media and switching framework
 * 
 * *****************  Version 2  *****************
 * User: Junanto      Date: 22/04/01   Time: 21:27
 * Updated in $/xGate
 * voxAlloc and voxPlayFile tested on Prosody
 * 
 * *****************  Version 1  *****************
 * User: Junanto      Date: 21/04/01   Time: 23:55
 * Created in $/xGate
 * First day of Prosody and voice driver intergration
 * 
 *----------------------------------------------------------------*/

/*----------------------------------------------------------------* 
 * Description:
 * This module contains definition for Prosody voice driver
 *----------------------------------------------------------------*/

#include <sys/stat.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "stdafx.h"
#include "xgconfig.h"

#ifdef XGATE_HAS_PROSODY

#include "licence.h" //DG Added
#include <algorithm>

#include "prosody.h"
#include "kernel.h"
#include "log.h"
#include "klog.h"
#include "term.h"

#include "thread_process.h"
#include <stdlib.h>

#include "smrtp.h"
#include "smpath.h"


#ifdef XGATE_HAS_ACULABSWITCH
#include "aculabswitch.h"
#include <errno.h>

#undef PROSODY_DEBUG

//-------------------------------------------------------------------
#ifdef _DEBUG
    #define PROSODY_DEBUG
#else
    //#define PROSODY_DEBUG
#endif

#define THISMODULE		__modname__
static const char __modname__[] = "Prosody";
#define NEWLINE			"\n"
#define MEMZERO(x)		memset(&(x), 0, sizeof(x))
#define kSMChannelCapsCondStandAlone (1<<12)
#define kSMChannelCapsCondRefInput (1<<11)
#define SM_ERROR_NAME_NAME "sm_error_name_PcPsicr80_"

#define _startEvent  	1
#define _cancelEvent 	2 
#define _quitEvent   	3
#define _readParams     4

//unsigned event_flags = 0;  //moved to Driver Class

int idx = 0;

/* Below defines are mentioned in .c file and hence defined here
 * to avoid redifinitions. 
 */
#ifdef __linux__
#define  WAVE_FORMAT_ADPCM      0x0002  /*  Microsoft Corporation  */
#define  WAVE_FORMAT_ALAW       0x0006  /*  Microsoft Corporation  */
#define  WAVE_FORMAT_MULAW      0x0007  /*  Microsoft Corporation  */
#define  WAVE_FORMAT_OKI_ADPCM  0x0010  /*  OKI  */
#define  WAVE_FORMAT_IMA_ADPCM  0x0011  /*  IMA  */
#define  WAVE_FORMAT_DSPGROUP_TRUESPEECH        0x0022  /*  DSP Group, Inc  */
#define  WAVE_FORMAT_PROSODY_1612  0x0027 /* Registered tag for Prosody 16kbps and 12kbps data compression */
#define  WAVE_FORMAT_PROSODY_8KBPS 0x0094 /* Registered tag for Prosody 8kbps data compression */
#define  WAVE_FORMAT_ACURATE_16 0xAC16   /* Defunct development tag, until registered with Microsoft */
#define  WAVE_FORMAT_G721_ADPCM 0x0040   /* Antex Electronics Corporation  */
#define  WAVE_FORMAT_DIALOGIC_OKI_ADPCM  0x0017
#define  WAVE_FORMAT_SPEEX 0xa109
#endif

 pthread_cond_t   eventCond,completionCond;        //signal start, cancel, completion 
 pthread_mutex_t _voxMutex,completionMutex;         // set to condtion wait above signals
// pthread_t _hIOThread;              //handle of I/O thread
 pthread_mutex_t lock_t;


//-------------------------------------------------------------------
ACUAPI char *ACUTiNG_WINAPI sm_error_name(struct sm_error_name_parms *errp)
{

}


//---------------------------------------------------------------------


ProsodyDriver *ProsodyDriver::_theProsodyDriver = NULL;
ProsodyFactory *ProsodyFactory::theProsodyFactory = NULL;

int	ProsodyFactory::_num_cards = 0;
int	ProsodyFactory::_num_modules = 0;
std::vector<tSMCardId>	ProsodyFactory::_vCardId = std::vector<tSMCardId>();

void Sleep( unsigned long ms )
{
#ifndef __linux__
        ::Sleep(ms);
#else
        poll(0,0,ms); // __linux__
#endif
};


//-------------------------------------------------------------------
struct WAV_Header
{
    char riff[4];		// "RIFF"
    unsigned file_length;	// File length - 8
    char wave[4];		// "WAVE"

    char fmt_[4];		// "fmt "
    unsigned chunk_1_len;	// usually 16
    unsigned short encoding;	// voice format
    unsigned short channels;	// mono or stereo
    unsigned sample_rate;	// samples per second e.g.: 8000, 44100
    unsigned bytes_second;	// bytes/second
    unsigned short blk_align;	//
    unsigned short bits_sample;	// bits per sample e.g.: 8, 16

    char data[4];		// "data"
    unsigned chunk_2_len;	// payload size
};

//-------------------------------------------------------------------
#ifdef PROSODY_DEBUG
    #define CHKFUNC(x,y)    x
#else
    #define CHKFUNC(x,y)    x
#endif


static int file_length(FILE* fp)
{
   fseek(fp,0L,SEEK_END);
   int file_size=ftell(fp);
   fseek(fp,0L,SEEK_SET);

  return file_size;
}

//-------------------------------------------------------------------
// Helper function
//-------------------------------------------------------------------
int *a_zero()
{
    // user by CrashMePlease
    return (int*)0;
}

//-------------------------------------------------------------------
static const char *DeviceName(ProsodyDevice *dev)
{
    if (dev) return dev->GetName();
    else return "(null)";
}

//-------------------------------------------------------------------
const char *errcode_sm(int err)
{
    /**
    switch (err) 
    {
    case ERR_SM_BAD_DATA_LENGTH: return "ERR_SM_BAD_DATA_LENGTH";
    case ERR_SM_BAD_PARAMETER: return "ERR_SM_BAD_PARAMETER";
    case ERR_SM_CHANNEL_ALLOCATED: return "ERR_SM_CHANNEL_ALLOCATED";
    case ERR_SM_DEVERR: return "ERR_SM_DEVERR";
    case ERR_SM_DISCONNECTED: return "ERR_SM_DISCONNECTED";
    case ERR_SM_DOWNLOAD: return "ERR_SM_DOWNLOAD";
    case ERR_SM_FILE_ACCESS: return "ERR_SM_FILE_ACCESS";
    case ERR_SM_FILE_FORMAT: return "ERR_SM_FILE_FORMAT";
    case ERR_SM_FIRMWARE_NOT_RUNNING: return "ERR_SM_FIRMWARE_NOT_RUNNING";
    case ERR_SM_FIRMWARE_PROBLEM: return "ERR_SM_FIRMWARE_PROBLEM";
    case ERR_SM_INCOMPATIBLE_APP: return "ERR_SM_INCOMPATIBLE_APP";
    case ERR_SM_INCOMPATIBLE_DRIVER: return "ERR_SM_INCOMPATIBLE_DRIVER";
    case ERR_SM_MODULE_ACCESS: return "ERR_SM_MODULE_ACCESS";
    case ERR_SM_MODULE_ALREADY_RUNNING: return "ERR_SM_MODULE_ALREADY_RUNNING";
    case ERR_SM_NOT_IMPLEMENTED: return "ERR_SM_NOT_IMPLEMENTED";
    case ERR_SM_NOT_SAME_GROUP: return "ERR_SM_NOT_SAME_GROUP";
    case ERR_SM_NOT_SAME_MODULE: return "ERR_SM_NOT_SAME_MODULE";
    case ERR_SM_NO_ASSOCIATED_SWITCH: return "ERR_SM_NO_ASSOCIATED_SWITCH";
    case ERR_SM_NO_CAPACITY: return "ERR_SM_NO_CAPACITY";
    case ERR_SM_NO_DATA_AVAILABLE: return "ERR_SM_NO_DATA_AVAILABLE";
    case ERR_SM_NO_LICENCE: return "ERR_SM_NO_LICENCE";
    case ERR_SM_NO_RECORD_IN_PROGRESS: return "ERR_SM_NO_RECORD_IN_PROGRESS";
    case ERR_SM_NO_REPLAY_IN_PROGRESS: return "ERR_SM_NO_REPLAY_IN_PROGRESS";
    case ERR_SM_NO_RESOURCES: return "ERR_SM_NO_RESOURCES";
    case ERR_SM_NO_SUCH_CHANNEL: return "ERR_SM_NO_SUCH_CHANNEL";
    case ERR_SM_NO_SUCH_DIGIT: return "ERR_SM_NO_SUCH_DIGIT";
    case ERR_SM_NO_SUCH_FIRMWARE: return "ERR_SM_NO_SUCH_FIRMWARE";
    case ERR_SM_NO_SUCH_GROUP: return "ERR_SM_NO_SUCH_GROUP";
    case ERR_SM_NO_SUCH_MODULE: return "ERR_SM_NO_SUCH_MODULE";
    case ERR_SM_OS_RESOURCE_PROBLEM: return "ERR_SM_OS_RESOURCE_PROBLEM";
    case ERR_SM_OS_OTHER_PROBLEM: return "ERR_SM_OS_OTHER_PROBLEM";
    case ERR_SM_PENDING: return "ERR_SM_PENDING";
    case ERR_SM_WRONG_CHANNEL_STATE: return "ERR_SM_WRONG_CHANNEL_STATE";
    case ERR_SM_WRONG_CHANNEL_TYPE: return "ERR_SM_WRONG_CHANNEL_TYPE";
    case ERR_SM_WRONG_FIRMWARE_TYPE: return "ERR_SM_WRONG_FIRMWARE_TYPE";
    case ERR_SM_WRONG_MODULE_TYPE: return "ERR_SM_WRONG_MODULE_TYPE";
    case ERR_SM_IWR_NO_NOISE: return "ERR_SM_IWR_NO_NOISE";
    case ERR_SM_IWR_EXCESS_NOISE: return "ERR_SM_IWR_EXCESS_NOISE";
    case ERR_SM_IWR_NO_MODEL: return "ERR_SM_IWR_NO_MODEL";
    case ERR_SM_IWR_EXCESS_MODELS: return "ERR_SM_IWR_EXCESS_MODELS";
    case ERR_SM_IWR_EXCESS_STATES: return "ERR_SM_IWR_EXCESS_STATES";
    case ERR_SM_IWR_MODEL_CHANGED: return "ERR_SM_IWR_MODEL_CHANGED";
    case ERR_SM_IWR_MISSING: return "ERR_SM_IWR_MISSING";
    default: return "(unknown error)";
    }
    **/
    static SM_ERROR_NAME_PARMS errname;
    MEMZERO(errname);
    errname.code = err;
    return sm_error_name(&errname);
}

//-------------------------------------------------------------------
static const char *ErrMsg(int rc)
{
    return errcode_sm(rc);
}


#ifdef XGATE_HAS_PROSODY_RTP

/***************************************************
 *    Start of Prosody/X RTP                       *
 ***************************************************/


#include "MediaManager.h"
#include "rtpmedia.h"


class VMP_RTP_Channel : public RTP_Channel
{
    /** Determine the type of the current rtp board type
    **/
    virtual RTP_Board_Type GetBoardType() const
    {
	return RTP_Board_Prosody;
    }

    /** Requiered for AudioCode interface event. 
        This will be discarded if not necessary.
    **/
    virtual void SetEventCallback(RTP_Channel_Callback*)
    {
	// not applicable
    }
		 

    /** Set/get payload code, supported to provided backward 
        compatibility with current RTP_Media	
    **/
    virtual void SetCodec(Rtp_Payload_Code code)
    {
	//CLog::Info(THISMODULE, "[%s] SetCodec()", _channel_name);
	_config.RTP.Codec = code;
    }


    /** Set/get payload code, supported to provided backward 
        compatibility with current RTP_Media	
    **/
    virtual Rtp_Payload_Code GetCodec()
    {
	//CLog::Info(THISMODULE, "[%s] GetCodec()", _channel_name);
	return _config.RTP.Codec;
    }


    /** Open the rtp channel. Some code on the RTP_Media::Open() that 
        depends on AudioCode frame work extracted to RTP_Channel decendant.
	Notes:
	From this state the channel ready to be activated
	For Audiocodes, it sets certain param and switch device props
	For Mapletree, it sets certain param and initialize the port
     */
    virtual bool Open(RTP_Config& config)
    {
	//CLog::Info(THISMODULE, "[%s] Open()", _channel_name);

	switch (config.RTP.Codec)
	{
	case RTP_PT_PCMA:
	case RTP_PT_PCMU:
	case RTP_PT_G7231:
	case RTP_PT_G729:
	    break;

	default:
	    CLog::Warning(THISMODULE, "[%s] Attempt to open with unsupported codec %d", _channel_name, config.RTP.Codec);
	    //*** return false;
	}

	switch (_config.DTMF.Transport_Type)
	{
	case RTP_Config::DTMF_RFC2833:
	case RTP_Config::DTMF_Inband:
	    break;

	default:
	    CLog::Warning(THISMODULE, "[%s] Attempt to open with unsupported dtmf transfer mode %d", _channel_name, _config.DTMF.Transport_Type);
	    //*** return false;
	}

	_config = config;
	_init_state = 1;
	return true;
    }


    /** Activate the rtp channel. Some code on the RTP_Media that 
        depends on AudioCode frame work extracted to RTP_Channel descendant.
     */
    virtual bool Activate(const ACE_INET_Addr& addr, Rtp_Payload_Code codec, bool low)
    {
	CLog::Info(THISMODULE, "inside Activate() : [%s] Activate()", _channel_name);

	// set local address for TX
	SM_VMPTX_CONFIG_PARMS tx_addr;
	
	MEMZERO(tx_addr);
	tx_addr.vmptx = _vmptx;
	tx_addr.source_rtp = *_local_addr.getsockaddr_in();

	tx_addr.destination_rtp = *addr.getsockaddr_in();

	ACE_INET_Addr rtcp = addr;
	rtcp.set_port_number(rtcp.get_port_number()+1);
	tx_addr.destination_rtcp = *rtcp.getsockaddr_in();

	SOCKADDR_IN any_addr = *_local_addr.getsockaddr_in();
	any_addr.sin_port = htons(ntohs(any_addr.sin_port)+1);
	tx_addr.source_rtcp = any_addr;

	int rc = sm_vmptx_config(&tx_addr);
	if (rc) {
	    CLog::Error(THISMODULE, "[%s] Can't configure VMPtx, err=%s", _channel_name, errmsg(rc));
	    return false;
	}

	// set RTCP handler for RX
	SM_VMPRX_SET_RTCPHAND_PARMS rxrtcp;
	MEMZERO(rxrtcp);
	rxrtcp.vmprx = _vmprx;
	rxrtcp.rtcphand = _rtcphand;
	rc = sm_vmprx_set_rtcphand(&rxrtcp);
	if (rc) {
	    CLog::Error(THISMODULE, "[%s] Can't enable RTCP handler on VMPrx, err=%s", _channel_name, errmsg(rc));
	    return false;
	}

	// set RTCP handler for TX
	SM_VMPTX_SET_RTCPHAND_PARMS txrtcp;
	MEMZERO(txrtcp);
	txrtcp.vmptx = _vmptx;
	txrtcp.rtcphand = _rtcphand;
	rc = sm_vmptx_set_rtcphand(&txrtcp);
	if (rc) {
	    CLog::Error(THISMODULE, "[%s] Can't enable RTCP handler on VMPtx, err=%s", _channel_name, errmsg(rc));
	    return false;
	}

	SM_TDMRX_DATAFEED_PARMS tdmrx_df;
	MEMZERO(tdmrx_df);
	tdmrx_df.tdmrx = _tdmrx;
	rc = sm_tdmrx_get_datafeed(&tdmrx_df);
	if (rc) {
	    CLog::Error(THISMODULE, "[%s] Can't get TDMrx datafeed, err=%s", _channel_name, errmsg(rc));
	    return false;
	}

#if 0
	/****  THIS ATTENUATOR IS ONLY USED TO VALIDATE WHETHER ACULAB ECHO CANCELLER IS WORKING OR NOT ****/
	SM_PATH_CREATE_PARMS attenuator;
	MEMZERO(attenuator);
	attenuator.module = _module_id;
	rc = sm_path_create(&attenuator);
	if (rc) {
	    CLog::Error(THISMODULE, "[%s] Can't create attenuator for EC, err=%s", _channel_name, errmsg(rc));
	    return false;
	}

	SM_PATH_DATAFEED_CONNECT_PARMS attenuator_conn;
	MEMZERO(attenuator_conn);
	attenuator_conn.path = attenuator.path;
	attenuator_conn.data_source = tdmrx_df.datafeed;
	rc = sm_path_datafeed_connect(&attenuator_conn);
	if (rc) {
	    CLog::Error(THISMODULE, "[%s] Can't connect attenuator to path object, err=%s", _channel_name, errmsg(rc));
	    return false;
	}

	SM_PATH_AGC_PARMS agc;
	MEMZERO(agc);
	agc.path = attenuator.path;
	agc.volume = -24;
	rc = sm_path_agc(&agc);
	if (rc) {
	    CLog::Error(THISMODULE, "[%s] Can't set AGC, err=%s", _channel_name, errmsg(rc));
	    return false;
	}

	SM_PATH_DATAFEED_PARMS attenuator_df;
	MEMZERO(attenuator_df);
	attenuator_df.path = attenuator.path;
	rc = sm_path_get_datafeed(&attenuator_df);
	if (rc) {
	    CLog::Error(THISMODULE, "[%s] Can't get attenuator datafeed, err=%s", _channel_name, errmsg(rc));
	    return false;
	}
	tdmrx_df.datafeed = attenuator_df.datafeed;
	/********************************************************************/
#endif

	// configure echo-canceller if needed
	if (_config.EchoCanceller) {

	    // create a path object and establish the following:
	    // vmptx <-- path <-- tdmrx
	    SM_PATH_CREATE_PARMS path;
	    MEMZERO(path);
	    path.module = _module_id;
	    rc = sm_path_create(&path);
	    if (rc) {
		CLog::Error(THISMODULE, "[%s] Can't create echo canceller, err=%s", _channel_name, errmsg(rc));
		return false;
	    }
	    _path = path.path;

	    SM_PATH_DATAFEED_CONNECT_PARMS path_df_conn;
	    MEMZERO(path_df_conn);
	    path_df_conn.path = _path;
	    path_df_conn.data_source = tdmrx_df.datafeed;
	    rc = sm_path_datafeed_connect(&path_df_conn);
	    if (rc) {
		CLog::Error(THISMODULE, "[%s] Can't connect TDMrx to echo canceller, err=%s", _channel_name, errmsg(rc));
		return false;
	    }

	    SM_PATH_DATAFEED_PARMS path_df;
	    MEMZERO(path_df);
	    path_df.path = _path;
	    rc = sm_path_get_datafeed(&path_df);
	    if (rc) {
		CLog::Error(THISMODULE, "[%s] Can't get EC datafeed, err=%s", _channel_name, errmsg(rc));
		return false;
	    }

	    SM_VMPTX_DATAFEED_CONNECT_PARMS vmptx_df_conn;
	    MEMZERO(vmptx_df_conn);
	    vmptx_df_conn.data_source = path_df.datafeed;
	    vmptx_df_conn.vmptx = _vmptx;
	    rc = sm_vmptx_datafeed_connect(&vmptx_df_conn);
	    if (rc) {
		CLog::Error(THISMODULE, "[%s] Can't connect EC to VMPtx, err=%s", _channel_name, errmsg(rc));
		return false;
	    }

	    // now that all connection is done, activate echo-canceller
	    SM_PATH_ECHOCANCEL_PARMS echo;
	    MEMZERO(echo);
	    echo.path = _path;
	    echo.enable = 1;
	    echo.reference = _vmprx_df;
	    echo.non_linear = 0;
	    echo.use_agc = 0;
	    echo.fix_agc = 0;
	    echo.span = _config.ECTailLen;
	    rc = sm_path_echocancel(&echo);
	    if (rc) {
		CLog::Warning(THISMODULE, "[%s] Can't enable EC, err=%s", _channel_name, errmsg(rc));
		// carry on
	    }

	} else {

	    // just connect TDMrx to VMPtx (without echo canceller)
	    SM_VMPTX_DATAFEED_CONNECT_PARMS vmptx_df_conn;
	    MEMZERO(vmptx_df_conn);
	    vmptx_df_conn.data_source = tdmrx_df.datafeed;
	    vmptx_df_conn.vmptx = _vmptx;
	    rc = sm_vmptx_datafeed_connect(&vmptx_df_conn);
	    if (rc) {
		CLog::Error(THISMODULE, "[%s] Can't connect TDMrx to VMPtx, err=%s", _channel_name, errmsg(rc));
		return false;
	    }
	}

	// configure codec to use
	switch (codec) {
	case RTP_PT_PCMA:
	    {
		// configure codec for RX
		SM_VMPRX_CODEC_ALAW_PARMS rxp;
		MEMZERO(rxp);
		rxp.vmprx = _vmprx;
		rxp.payload_type = RTP_PT_PCMA;
		rc = sm_vmprx_config_codec_alaw(&rxp);
		if (rc) {
		    CLog::Error(THISMODULE, "[%s] Can't configure A-law for VMPrx, err=%s", _channel_name, errmsg(rc));
		    return false;
		}

		// configure codec for TX
		SM_VMPTX_CODEC_ALAW_PARMS txp;
		MEMZERO(txp);
		txp.vmptx = _vmptx;
		txp.payload_type = RTP_PT_PCMA;
		txp.VADMode = _config.SilenceCompression ? kSMVMPTxVADModeEnabled : kSMVMPTxVADModeDisabled;
		txp.ptime = _config.RTP.PackingFactor * 10;
		rc = sm_vmptx_config_codec_alaw(&txp);
		if (rc) {
		    CLog::Error(THISMODULE, "[%s] Can't configure A-law for VMPtx, err=%s", _channel_name, errmsg(rc));
		    return false;
		}

	    }
	    break;

	case RTP_PT_PCMU:
	    {
		// configure codec for RX
		SM_VMPRX_CODEC_MULAW_PARMS rxp;
		MEMZERO(rxp);
		rxp.vmprx = _vmprx;
		rxp.payload_type = RTP_PT_PCMU;
		rc = sm_vmprx_config_codec_mulaw(&rxp);
		if (rc) {
		    CLog::Error(THISMODULE, "[%s] Can't configure u-law for VMPrx, err=%s", _channel_name, errmsg(rc));
		    return false;
		}

		// configure codec for TX
		SM_VMPTX_CODEC_MULAW_PARMS txp;
		MEMZERO(txp);
		txp.vmptx = _vmptx;
		txp.payload_type = RTP_PT_PCMU;
		txp.VADMode = _config.SilenceCompression ? kSMVMPTxVADModeEnabled : kSMVMPTxVADModeDisabled;
		txp.ptime = _config.RTP.PackingFactor * 10;
		rc = sm_vmptx_config_codec_mulaw(&txp);
		if (rc) {
		    CLog::Error(THISMODULE, "[%s] Can't configure u-law for VMPtx, err=%s", _channel_name, errmsg(rc));
		    return false;
		}
	    }
	    break;

	case RTP_PT_G7231:
	    {
		// configure codec for RX
		SM_VMPRX_CODEC_G723_1_PARMS rxp;
		MEMZERO(rxp);
		rxp.vmprx = _vmprx;
		rxp.payload_type = RTP_PT_G7231;
		rxp.post_filter = _config.PosFilter ? 1 : 0;
		rxp.plc_mode = kSMPLCModeEnabled;
		rc = sm_vmprx_config_codec_g723_1(&rxp);
		if (rc) {
		    CLog::Error(THISMODULE, "[%s] Can't configure G.723 for VMPrx, err=%s", _channel_name, errmsg(rc));
		    return false;
		}

		// configure codec for TX
		SM_VMPTX_CODEC_G723_1_PARMS txp;
		MEMZERO(txp);
		txp.vmptx = _vmptx;
		txp.payload_type = RTP_PT_G7231;
		txp.high_pass_filter = _config.HighPassFilter ? 1 : 0;
		txp.rate = low ? 5300 : 6300;
		txp.silence_compression = _config.SilenceCompression ? 1 : 0;
		txp.frames_per_packet = _config.RTP.PackingFactor;
		rc = sm_vmptx_config_codec_g723_1(&txp);
		if (rc) {
		    CLog::Error(THISMODULE, "[%s] Can't configure G.723 for VMPtx, err=%s", _channel_name, errmsg(rc));
		    return false;
		}
	    }
	    break;

	case RTP_PT_G729:
	    {
		// configure codec for RX
		SM_VMPRX_CODEC_G729AB_PARMS rxp;
		MEMZERO(rxp);
		rxp.vmprx = _vmprx;
		rxp.payload_type = RTP_PT_G729;
		rxp.plc_mode = kSMPLCModeEnabled;
		rc = sm_vmprx_config_codec_g729ab(&rxp);
		if (rc) {
		    CLog::Error(THISMODULE, "[%s] Can't configure G.729 for VMPrx, err=%s", _channel_name, errmsg(rc));
		    return false;
		}

		// configure codec for TX
		SM_VMPTX_CODEC_G729AB_PARMS txp;
		MEMZERO(txp);
		txp.vmptx = _vmptx;
		txp.payload_type = RTP_PT_G729;
		txp.VADMode = _config.SilenceCompression ? kSMVMPTxVADModeComfortNoise : kSMVMPTxVADModeDisabled;
		txp.ptime = _config.RTP.PackingFactor * 10;
		rc = sm_vmptx_config_codec_g729ab(&txp);
		if (rc) {
		    CLog::Error(THISMODULE, "[%s] Can't configure G.729 for VMPtx, err=%s", _channel_name, errmsg(rc));
		    return false;
		}
	    }
	    break;

	default:
	    CLog::Warning(THISMODULE, "[%s] Unsupported codec %d", _channel_name, codec);
	    return false;

	}

	// always enable RFC2833 for RX dtmf processing
	SM_VMPRX_CODEC_RFC2833_PARMS rxcodec;
	MEMZERO(rxcodec);
	rxcodec.vmprx = _vmprx;
	rxcodec.payload_type = RTP_PT_RFC_2833;
	rxcodec.plc_mode = kSMPLCModeEnabled;
	rc = sm_vmprx_config_codec_rfc2833(&rxcodec);
	if (rc) {
	    CLog::Error(THISMODULE, "[%s] Can't configure rfc2833 for VMPrx, err=%s", _channel_name, errmsg(rc));
	    return false;
	}

	// always regenerate tones on receipt of RFC2833
	SM_VMPRX_TONE_PARMS rxtone;
	MEMZERO(rxtone);
	rxtone.vmprx = _vmprx;
	rxtone.regen_tones = 1;
	rc = sm_vmprx_config_tones(&rxtone);
	if (rc) {
	    CLog::Error(THISMODULE, "[%s] Can't configure tones for VMPrx, err=%s", _channel_name, errmsg(rc));
	    return false;
	}

	// configure dtmf processing for RX if RFC2833 is selected
	switch (_config.DTMF.Transport_Type)
	{
	case RTP_Config::DTMF_RFC2833:
	    SM_VMPTX_CODEC_RFC2833_PARMS txcodec;
	    MEMZERO(txcodec);
	    txcodec.vmptx = _vmptx;
	    txcodec.payload_type = RTP_PT_RFC_2833;
	    rc = sm_vmptx_config_codec_rfc2833(&txcodec);
	    if (rc) {
		CLog::Error(THISMODULE, "[%s] Can't configure rfc2833 for VMPtx, err=%s", _channel_name, errmsg(rc));
		return false;
	    }

	    SM_VMPTX_TONE_PARMS txtone;
	    MEMZERO(txtone);
	    txtone.vmptx = _vmptx;
	    txtone.convert_tones = 1;
	    txtone.elim_tones = 0;	//shall we clip dtmf?
	    txtone.tone_set_id = _tone_set;
	    rc = sm_vmptx_config_tones(&txtone);
	    if (rc) {
		CLog::Error(THISMODULE, "[%s] Can't configure tones for VMPtx, err=%s", _channel_name, errmsg(rc));
		return false;
	    }
	    break;

	case RTP_Config::DTMF_Inband:
	    // do nothing
	    break;

	default:
	    CLog::Warning(THISMODULE, "[%s] Unsupported dtmf transfer mode %d, default to inband", _channel_name, _config.DTMF.Transport_Type);
	    //*** return false;
	}

	_init_state = 2;
	_config.RTP.Codec = codec;

	CLog::Info(THISMODULE, "Activate() : return true;\n");
	return true;
    }


    /** Create the switch device for the channel. 
        This is extracted from RTP_Media to remove the dependency to AudioCode.
     */
    virtual SwitchDevice *GetSwitchDevice(const char* name)
    {
	return _swdev;
    }


    /** Close the channel. This is extracted from RTP_Media. 
        The code will be placed in the RTP_Channel dependant.
     */
    virtual void Close()
    {
	//CLog::Info(THISMODULE, "[%s] Close()", _channel_name);
	_init_state = 0;
    }


    /** Compatibility with AcChannel.	
	Initially, the channel should return zero.
	After Open() is called and physical channel is allocated, the channel
	should return 1.
	After Activate() is called and bidirectional communication is
	active, the channel should return 2.
     */
    virtual int GetInitializationStatus() const
    {
	return _init_state;
    }


    /** Compatibility with AcChannel.
     */
    virtual time_t GetLastActivateTime()
    {
	// not applicable
	return 0;
    }


    /** Compatibility with AcChannel.
     */
    virtual void DialDtmf(const char*)
    {
	// do nothing
    }


    /** Release Rtp Channel. Code extracted to RTP_Media decendants 
        to remove dependency.
     */
    virtual void ReleaseChannel(CallDirection, CallParameter*, CallCause)
    {
	//CLog::Info(THISMODULE, "[%s] ReleaseChannel()", _channel_name);
        delete this->_swdev;  //T 10072016 Puvan
	delete this;
    }


    /** Compatibility with AcChannel.
     */
    virtual const sockaddr_in& GetLocalAddress()
    {
	return *_local_addr.getsockaddr_in();
    }


    /** Compatibility with AcChannel.
     */
    virtual const void GetLocalAddress(ACE_INET_Addr& addr)
    {
	addr = _local_addr;
    }


    /** Compatibility with AcChannel.
     */
    virtual Rtp_Payload_Code GetSelectedPayload()
    {
	return _config.RTP.Codec;
    }


    /** Extracted method from RTP_Media to prepare switching device.
     */
    virtual bool PrepareSwitching(RTP_Config*)
    {
	// not applicable
	return 0;
    }


    /** Get the channel number (only need to be unique in the same board)
     */
    virtual int GetChannelNumber()
    {
	return _rtp_port;
    }


    /** Provided to support backward compatibility with AcChannel
     */
    virtual int GetMediaID()
    {
	return _rtp_port;
    }


    /** Provided to support backward compatibility with AcChannel
     */
    virtual const AcRtcpStat& GetRtcpStat()
    {
	return _stat;
    }


    /** Get/Set interface to change the config parameteres
        the behaviour of this method is vary. AudioCodes implmentation
	might change some value instantly while Mapletree might
	only commit the changes when Activated.
     */
    virtual const RTP_Config& GetRtpConfig()
    {
	return _config;
    }


    /** Get/Set interface to change the config parameteres
        the behaviour of this method is vary. AudioCodes implmentation
	might change some value instantly while Mapletree might
	only commit the changes when Activated.
     */
    virtual void SetRtpConfig(RTP_Config& config)
    {
	//CLog::Info(THISMODULE, "[%s] SetRtpConfig()", _channel_name);
	_config = config;
    }


    /** Convert Prosody error code into meaningful message
    **/
    const char *errmsg(int rc)
    {
	MEMZERO(err);
	err.code = rc;
	return sm_error_name(&err);
    }

    // member variables
    RTP_Config _config;
    ACE_INET_Addr _local_addr;
    AculabSwitch* _swdev;
    tSMModuleId _module_id;
    int _init_state;
    unsigned short _rtp_port;
    AcRtcpStat _stat;
    tSMVMPrxId _vmprx;
    tSMVMPtxId _vmptx;
    tSMTDMtxId _tdmtx;
    tSMTDMrxId _tdmrx;
    tSMPathId _path;
    tSMRTCPHandId _rtcphand;
    kSMTimeslotType _companding;
    tSMDatafeedId _vmprx_df;
    tSMVMPTxToneSetId _tone_set;
    char _itf_name[64];
    char _serial_num[64];
    char _channel_name[64];
    int _card_id;
    SM_ERROR_NAME_PARMS err;

public:

    /** VMP_RTP_Channel constructor
    **/
    VMP_RTP_Channel(int card_id, const char* serial_num, tSMModuleId module_id, const char* itf_name, tSMVMPrxId vmprx, tSMVMPtxId vmptx, kSMTimeslotType companding, tSMVMPTxToneSetId tone_set)
    {
	_vmprx = vmprx;
	_vmptx = vmptx;
	_tdmtx = 0;
	_tdmrx = 0;
	_path = 0;
	_rtcphand = 0;

	_module_id = module_id;
	_init_state = 0;
	_tone_set = tone_set;
	strcpy(_itf_name, itf_name);
	strcpy(_serial_num, serial_num);
	MEMZERO(_config);
	MEMZERO(_stat);
	_swdev = 0;
	_companding = companding;
	_card_id = card_id;
    }


    /** Class destructor
    **/
    ~VMP_RTP_Channel()
    {
	sm_vmptx_destroy(_vmptx);
	sm_vmprx_destroy(_vmprx);
	if (_path) sm_path_destroy(_path);
	if (_rtcphand) sm_rtcphand_destroy(_rtcphand);
	if (_tdmtx) sm_tdmtx_destroy(_tdmtx);
	if (_tdmrx) sm_tdmrx_destroy(_tdmrx);
    }


    /** Initialize the Prosody/X channel, return false if failure
    **/
    bool Init()
    {
	    //printf("inside Init() : Initialize the Prosody/X channel\n");
	    CLog::Error(THISMODULE, "inside Init() : Initialize the Prosody/X channel");
	// retrieve local address for RX
	SM_VMPRX_PORT_PARMS rx_addr;
	MEMZERO(rx_addr);
	rx_addr.vmprx = _vmprx;
	int rc = sm_vmprx_get_ports(&rx_addr);
	if (rc) {
	    CLog::Error(THISMODULE, "[%s] Can't retrieve RX ports, err=%s", _itf_name, errmsg(rc));
	    return false;
	}
	_rtp_port = rx_addr.RTP_port;
/* Narsim: prosody version 6 has no member named S_un */
#ifdef __linux__
	_local_addr.set(ntohs(rx_addr.RTP_port), rx_addr.address.s_addr, 0);
#else
	_local_addr.set(ntohs(rx_addr.RTP_port), rx_addr.address.S_un.S_addr, 0);
#endif

	// create a unique name based on the module index and RTP port
	sprintf(_channel_name, "%s/%u", _itf_name, _rtp_port);

	//CLog::Info(THISMODULE, "[%s] Init()", _channel_name);

	// create the associated switching device for this channel
	_swdev = new AculabSwitch(_channel_name, _serial_num, _card_id, -1, -1, true, _module_id);
	AculabAddress* switching_address = (AculabAddress*)_swdev->GetAddress(SF_Aculab);
	int stream = switching_address->GetStream();
	int tslot = switching_address->GetTimeslot();

	// create switching for VMPrx and establish the following: 
	// vmprx --> datafeed --> tdmtx
	SM_TDMTX_CREATE_PARMS tdmtx;
	MEMZERO(tdmtx);
	tdmtx.module = _module_id;
	tdmtx.stream = stream;
	tdmtx.timeslot = tslot;
	tdmtx.type = _companding;
	rc = sm_tdmtx_create(&tdmtx);
	if (rc) {
	    CLog::Error(THISMODULE, "[%s] Can't create TDMtx, err=%s", _channel_name, errmsg(rc));
	    return false;
	}
	_tdmtx = tdmtx.tdmtx;

	SM_VMPRX_DATAFEED_PARMS vmprx_df;
	MEMZERO(vmprx_df);
	vmprx_df.vmprx = _vmprx;
	rc = sm_vmprx_get_datafeed(&vmprx_df);
	if (rc) {
	    CLog::Error(THISMODULE, "[%s] Can't get VMPrx datafeed, err=%s", _channel_name, errmsg(rc));
	    return false;
	}
	_vmprx_df = vmprx_df.datafeed;

	SM_TDMTX_DATAFEED_CONNECT_PARMS tdmtx_df_conn;
	MEMZERO(tdmtx_df_conn);
	tdmtx_df_conn.data_source = _vmprx_df;
	tdmtx_df_conn.tdmtx = _tdmtx;
	rc = sm_tdmtx_datafeed_connect(&tdmtx_df_conn);
	if (rc) {
	    CLog::Error(THISMODULE, "[%s] Can't connect datafeed to TDMtx, err=%s", _channel_name, errmsg(rc));
	    return false;
	}

	// create switching for VMPtx and establish the following: 
	// vmptx <-- datafeed <-- tdmrx
	SM_TDMRX_CREATE_PARMS tdmrx;
	MEMZERO(tdmrx);
	tdmrx.module = _module_id;
	tdmrx.stream = stream;
	tdmrx.timeslot = tslot;
	tdmrx.type = _companding;
	rc = sm_tdmrx_create(&tdmrx);
	if (rc) {
	    CLog::Error(THISMODULE, "[%s] Can't create TDMrx, err=%s", _channel_name, errmsg(rc));
	    return false;
	}
	_tdmrx = tdmrx.tdmrx;

	// RTCP handler
	SM_RTCPHAND_CREATE_PARMS rtcphand;
	MEMZERO(rtcphand);
	rtcphand.module = _module_id;
	rc = sm_rtcphand_create(&rtcphand);
	if (rc) {
	    CLog::Error(THISMODULE, "[%s] Can't create RTCP handler, err=%s", _channel_name, errmsg(rc));
	    return false;
	}
	_rtcphand = rtcphand.rtcphand;

	static const char* sdes_name = "xGate";
	SM_RTCPHAND_CONFIG_SDES_PARMS sdes;
	MEMZERO(sdes);
	sdes.rtcphand = _rtcphand;
	sdes.itemnum = 1;
	sdes.stringlen = strlen(sdes_name);
	sdes.stringval = (char*)sdes_name;
	rc = sm_rtcphand_config_sdes(&sdes);
	if (rc) {
	    CLog::Error(THISMODULE, "[%s] Can't configure SDES on RTCP, err=%s", _channel_name, errmsg(rc));
	    return false;
	}

	SM_RTCPHAND_CONFIG_REPORTS_PARMS reports;
	MEMZERO(reports);
	reports.rtcphand = _rtcphand;
	reports.reports = 15;
	rc = sm_rtcphand_config_reports(&reports);
	if (rc) {
	    CLog::Error(THISMODULE, "[%s] Can't configure report on RTCP, err=%s", _channel_name, errmsg(rc));
	    return false;
	}
	    CLog::Error(THISMODULE, "Init() : return true;");
	return true;
    }
};

class VMP_Interface;
static std::list<VMP_Interface*> list_vmps;

// One "interface" represents one DSP in Prosody/X
class VMP_Interface : public Interface
{
    friend class ProsodyModule;

    VMP_RTP_Channel* chan;

    enum {
	Cmd_Max_Channels,
	Cmd_ShowStatus,
	Cmd_Description,
    } VMP_Interface_Cmd;


    /** The class destructor
    **/
    ~VMP_Interface()
    {
	SM_VMPTX_DESTROY_TONESET_PARMS tone;
	MEMZERO(tone);
	tone.tone_set_id = _tone_set;
	sm_vmptx_destroy_toneset(&tone);
	list_vmps.remove(this);
    delete chan;
    }

    /** This function will be called by the service framework after 
	instantiation of this class, to allow the instance to initialize
	itself.
    **/
    bool Load() 
    {
	Term_Cmd *cmd;

	// max-channels
	cmd = new Term_Cmd(this, Cmd_Max_Channels, "max-channels", "Specify the maximum number of channels", PrivilegeLevelAdmin, TermModeConfig, false);
	cmd->Add(new Term_Cmd_Arg("arg1", CVariant(0), CVariantValidator(CVariant(0), "number of channels")));
	RegisterTermCmd(cmd, _cfgTree);

	cmd = new Term_Cmd(this, Cmd_ShowStatus, "status", "Show the status of this interface", PrivilegeLevelAdmin, TermModeConfig, false);
	RegisterTermCmd(cmd, _cfgTree);

	// (no) description
	cmd = new Term_Cmd(this, Cmd_Description, "description", "Specify the description for this interface", PrivilegeLevelAdmin, TermModeConfig, true);
	cmd->Add(new Term_Cmd_Arg("arg1", "", CVariantValidator("", "Description for this interface"), TermArgOptional));
	RegisterTermCmd(cmd, _cfgTree);

	return Interface::Load() && Start();
    }


    /** Terminal command callback
     */
    void DoTermCmd(Term_Session *session, bool no, Term_Cmd *cmd)
    {
	CVariant v;

	switch (cmd->GetId()) {

	case Cmd_Max_Channels:
	    ACQUIRE_SYSTEM_LOCK();
	    cmd->GetArg((unsigned)0)->GetValue(v);
	    if (SetCapacity(v.AsInt())) _max_channels = v.AsInt();
	    else session->SendError("Can't set max channels to lower than current usage" NEWLINE);
	    RELEASE_SYSTEM_LOCK();
	    break;

	case Cmd_ShowStatus:
	    ShowInterface(session);
	    break;

	case Cmd_Description:
	    ACQUIRE_SYSTEM_LOCK();
	    if (no) {
		SetDescription("");
	    } else {
		cmd->GetArg((unsigned)0)->GetValue(v);
		SetDescription(v.ToString());
	    }
	    RELEASE_SYSTEM_LOCK();
	    break;


	}

    }


    /** Show configuration for the specified entry.
    **/
    bool OnShowConfig(
	       const Cfg_Tree_Entry *entry,
	       String & output, 
	       const String & indent, 
	       int indent_size) const
    {
	if (GetDescription().size()) output += indent + "description \"" + GetDescription() + "\"" + NEWLINE;

	char buffer[64];
	sprintf(buffer, "%d", _max_channels);
	output += indent + "max-channel " + buffer + NEWLINE;
	return OK;
    }


    /** This function will be called by the service framework before it is
	destroyed/deleted, to allow the instance to cleanup itself.
    **/
    bool Unload()
    {
	return Interface::Unload();
    }


    /** Display interface specific information to the terminal user.
    **/
    void OnShowInterface(class Term_Session *session)
    {
	session->Sendf( SHOW_INTERFACE_INDENT "Card name:   %s" NEWLINE, _prosm->_card_name);
	session->Sendf( SHOW_INTERFACE_INDENT "Card desc:   %s" NEWLINE, _prosm->_card_desc);
	session->Sendf( SHOW_INTERFACE_INDENT "H/W version: %s" NEWLINE, _prosm->_hw_version);
	session->Sendf( SHOW_INTERFACE_INDENT "Serial No:   %s" NEWLINE, _prosm->_serial_no);
	session->Sendf( SHOW_INTERFACE_INDENT "IP address:  %s" NEWLINE, _prosm->_ip_address);
    }


    /** This is not real media, so this should not be called directly
    **/
    bool AllocateMedia(class Call *, class Media *& , int, enum CallDirection, struct CallParameter *)
    {
	return false;
    }


    /** This is not real media, so this should not be called directly
    **/
    bool AllocateMedia(class Call *, class Media *& , enum SelectionStrategy, enum CallDirection, struct CallParameter *)
    {
	return false;
    }


    /** This is not real media, so this should not be called directly
    **/
    void ReleaseMedia(class Media *, enum CallDirection, struct CallParameter *, enum CallCause)
    {
    }


    /** Open: May be called several times to start the object
	The object may be then in In-Service (enabled/disabled) state
    **/
    bool Start()
    {
	return Interface::Start();
    }


    /** Close: Once started, an object may be requested to close
	The object goes to the Out-Of-Service state
    **/
    bool Stop()
    {
	return Interface::Stop();
    }


    /** Convert Prosody error code into meaningful message
    **/
    const char *errmsg(int rc)
    {
	MEMZERO(err);
	err.code = rc;
	return sm_error_name(&err);
    }


    ProsodyModule* _prosm;
    ACU_CARD_ID _card_id;
    tSMVMPTxToneSetId _tone_set;
    SM_ERROR_NAME_PARMS err;
    int _max_channels;

public:

    /** The class constructor
    **/
    VMP_Interface(ACU_CARD_ID card_id, const char* name, ProsodyModule* module, int channel_count) : 
	Interface(ITF_PROSODYX, name)
    {
	_prosm = module;
	_card_id = card_id;
	_max_channels = channel_count;
	SetMediaCount(_max_channels);
	Enable();

	// configures the dtmf tones to be detected for use in rfc 2833
	SM_VMPTX_CREATE_TONESET_PARMS tone;
	MEMZERO(tone);
	tone.toneset = kSMVMPTxDefaultToneSet;
	tone.module = _prosm->GetModule();
	sm_vmptx_create_toneset(&tone);
	_tone_set = tone.tone_set_id;

	list_vmps.push_back(this);
    }


    /** Allocate one RTP channel from this VMP
    **/
    VMP_RTP_Channel* Allocate_VMP_RTP_Channel()
    {
	    CLog::Warning(THISMODULE, "inside Allocate_VMP_RTP_Channel()\n");
	if (!_prosm->IsEnabled() || !_prosm->_initialized) {
	    return 0;
	}

	tSMModuleId module_id = _prosm->GetModule();

	if (AculabBusManager::Instance()->GetNumAddressRemaining(module_id) == 0) {
	    CLog::Warning(THISMODULE, "[%s] runs out of H-Bus switching", Name().c_str());
	    return 0;
	}

	if (GetMediaCount() == 0) {
	    return 0;
	}

	// create RX 
	SM_VMPRX_CREATE_PARMS vmprx;
	MEMZERO(vmprx);
	vmprx.module = module_id;
	struct in_addr addr;
#ifdef __linux__
	addr.s_addr = INADDR_ANY;
#else
	addr.S_un.S_addr = INADDR_ANY;
#endif
	vmprx.address = addr;
	int rc = sm_vmprx_create(&vmprx);
	if (rc) {
	    CLog::Error(THISMODULE, "[%s] Can't create VMPrx, err=%s", Name().c_str(), errmsg(rc));
	    return 0;
	}

	// create TX
	SM_VMPTX_CREATE_PARMS vmptx;
	MEMZERO(vmptx);
	vmptx.module = module_id;
	rc = sm_vmptx_create(&vmptx);
	if (rc) {
	    sm_vmprx_destroy(vmprx.vmprx);
	    CLog::Error(THISMODULE, "[%s] Can't create VMPtx, err=%s", Name().c_str(), errmsg(rc));
	    return 0;
	}

//VMP_RTP_Channel* chan = new VMP_RTP_Channel(_card_id, _prosm->_serial_no, module_id, GetName(),  // T 09 -- Commented 
//					    vmprx.vmprx, vmptx.vmptx, _prosm->GetCompanding(), _tone_set);  // T 09 -- Commented 
        chan = new VMP_RTP_Channel(_card_id, _prosm->_serial_no, module_id, GetName(),
     		                   vmprx.vmprx, vmptx.vmptx, _prosm->GetCompanding(), _tone_set);


	if (!chan)  {
		return 0;
	}
	if (chan->Init()) {
	    	CLog::Warning(THISMODULE, "Allocate_VMP_RTP_Channel() ; return chan;\n");
		 return chan;
	}
	delete chan;
	chan = NULL;
	return 0;
    }

};



/******************************
 *    End of Prosody/X RTP    *
 ******************************/
#endif //XGATE_HAS_PROSODY_RTP



//-------------------------------------------------------------------
// Thread job structure
//-------------------------------------------------------------------
struct AsyncJob
{
    enum JobType {
	JOBTYPE_QUIT,
	JOBTYPE_PLAY_DTMF,
	JOBTYPE_PLAY_TONE,
    } _jobType;
    ProsodyDevice *_dev;
    char _digits[VOICE_MAXDIGITS+1];
    int _toneId;
    int _duration;

    AsyncJob() : _jobType(JOBTYPE_QUIT), _dev(NULL), _toneId(0), _duration(0) {
	_digits[0] = '\0';
    }

    AsyncJob(JobType jobType,ProsodyDevice *dev,const char *digits,int toneId,int duration) :
	_jobType(jobType), _dev(dev), _toneId(toneId), _duration(duration)
    {
	if (digits) safe_strcpy(_digits,digits,sizeof(_digits));
	else _digits[0] = '\0';
    }
};


//-------------------------------------------------------------------
// Thread startup parameter
//-------------------------------------------------------------------
struct ThreadStartupParam
{
    int _threadID;
    HANDLE _hSem;
    HANDLE _hPipe;
};

/*----------------------------------------------------------------* 
 * Implementation of ProsodyModule
 *----------------------------------------------------------------*/
/** ProsodyModule's constructor/destructor
 */
ProsodyModule::ProsodyModule(
	ACU_CARD_ID cardID, 
	tSMModuleId module, 
	int module_ix, 
	const char* name, 
	kSMTimeslotType companding, 
	int ip_channel_count,
	const char* serial_no,
	const char* hw_version,
	const char* card_name,
	const char* card_desc,
	const char* ip_address,
	const char* card_key) :
    _module(module),
    _companding(companding),
    _module_ix(module_ix),
    _card_id(cardID),
    VoiceModule(name),
    DeviceUsage(name)
{
    _volume = 0;
    _agc_play = 1;
    _initialized = false;
    _dtmfdet = kSMToneEndDetectionMinDuration64;

    // load predefined tones (4)
    _tone[0].id = -1;
    _tone[0].freq = 1000;
    _tone[0].ampl = 0;
    _tone[1].id = -1;
    _tone[1].freq = 1500;
    _tone[1].ampl = 0;
    _tone[2].id = -1;
    _tone[2].freq = 2000;
    _tone[2].ampl = 0;
    _tone[3].id = -1;
    _tone[3].freq = 2500;
    _tone[3].ampl = 0;

    _support_voice = true;
    _support_conference = true;
    _support_echo_canceller = false;

#ifdef XGATE_HAS_PROSODY_RTP
    if (ip_channel_count) {
	char itf_name[64];
	sprintf(itf_name, "prx%02d", module_ix);
		_vmp_interface = new VMP_Interface(cardID, itf_name, this, ip_channel_count);
    } else {
	_vmp_interface = 0;
    }
#endif //XGATE_HAS_PROSODY_RTP

    strcpy(_serial_no, serial_no);
    strcpy(_hw_version, hw_version);
    strcpy(_card_name, card_name);
    strcpy(_card_desc, card_desc);
    strcpy(_ip_address, ip_address);
    strcpy(_card_key, card_key);

    ProsodyDriver::Instance()->InsertProsodyModule(this);
}

ProsodyModule::~ProsodyModule()
{
    ProsodyDriver::Instance()->RemoveProsodyModule(this);
}


/** Show configuration for the specified entry.
    This function will be called by the framework whenever it needs to display the
    specified entry. Previously this instance should tell the entry that it wishes to
    handle displaying the entry by calling entry->RegisterShowHandler().
 */
bool ProsodyModule::OnShowConfig(const Cfg_Tree_Entry* entry,String& output,const String& indent,int indent_size) const
{
    ProsodyModule *This = (ProsodyModule*)this;

    char buffer[32];
    sprintf(buffer,"%d",This->_volume);
    output += indent + "volume " + buffer + NEWLINE;

    output += indent + "dtmf-detection ";
    switch (_dtmfdet) {
    case kSMToneEndDetectionMinDuration40:
	output += "40ms";
	break;
    case kSMToneEndDetectionNoMinDuration:
	output += "none";
	break;
    case kSMToneEndDetectionMinDuration64:
    default:
	output += "64ms";
	break;
    }
    output += NEWLINE;

    output += indent + "companding ";
    switch( _companding )
    {
    case kSMTimeslotTypeALaw:
	output += "ALaw";
	break;
    case kSMTimeslotTypeMuLaw:
	output += "MuLaw";
    }
    output += NEWLINE;

    output += indent + (This->_agc_play ? "" : "no ") + "automatic-gain" + NEWLINE;

    for (int i=0;i<MAXTONE;i++) {
	char buffer[32];

	sprintf(buffer,"tone %d %d %d",i,This->_tone[i].freq,This->_tone[i].ampl);
	output += indent + buffer + NEWLINE;
    }

    output += indent + (_support_voice ? "" : "no ") + "voice" + NEWLINE;
    output += indent + (_support_conference ? "" : "no ") + "conference" + NEWLINE;
    output += indent + (_support_echo_canceller ? "" : "no ") + "echo-canceller" + NEWLINE;
    output += indent + (This->IsEnabled() ? "" : "no ") + "enable" + NEWLINE;

    return OK;
}

/** Terminal command callback
 */
void ProsodyModule::DoTermCmd(Term_Session *session, bool no, Term_Cmd *cmd)
{
    CVariant v;

    switch (cmd->GetId()) {

    case Cmd_Volume:
	cmd->GetArg((unsigned)0)->GetValue(v);
	_volume = v.AsInt();
	break;

    case Cmd_Companding:
	cmd->GetArg((unsigned)0)->GetValue(v);
	if(stricmp(v.AsString(), "ALaw") == 0)
	    _companding = kSMTimeslotTypeALaw;
	else if(stricmp(v.AsString(), "MuLaw") == 0 )
	    _companding = kSMTimeslotTypeMuLaw;
	else
	    assert(0);
	break;


    case Cmd_AGC:
	_agc_play = no ? 0 : 1;
	break;

    case Cmd_DtmfDet:
	if (!no) {
	    cmd->GetArg((unsigned)0)->GetValue(v);
	    if (strcasecmp(v.AsString(),"40ms") == 0) _dtmfdet = kSMToneEndDetectionMinDuration40;
	    else if (strcasecmp(v.AsString(),"none") == 0) _dtmfdet = kSMToneEndDetectionNoMinDuration;
	    else _dtmfdet = kSMToneEndDetectionMinDuration64;
	}
	break;

    case Cmd_Tone:
	cmd->GetArg((unsigned)0)->GetValue(v);
	if (!no) {
	    CVariant freq,ampl;
	    cmd->GetArg((unsigned)1)->GetValue(freq);
	    cmd->GetArg((unsigned)2)->GetValue(ampl);
	    _tone[v.AsInt()].freq = freq.AsInt();
	    _tone[v.AsInt()].ampl = ampl.AsInt();
	} else {
	    _tone[v.AsInt()].id = -1;
	    _tone[v.AsInt()].freq = 0;
	    _tone[v.AsInt()].ampl = 0;
	}
	break;

    case Cmd_Voice:
	if (no)  { _support_voice = false; }
	else {  _support_voice = true; }
	break;

    case Cmd_Conference:
	if (no) _support_conference = false;
	else _support_conference = true;
	break;

    case Cmd_Ecr:
	if (no) _support_echo_canceller = false;
	else {
	    if (EC_Capable()) {
		_support_echo_canceller = true;
	    }
	    else {
		CLog::Info(THISMODULE, "This module is not capable for echo-cancelling");
		_support_echo_canceller = false;
	    }
	}
	break;

    case Cmd_Enable:
	if (no) {
	    Disable();
#ifdef XGATE_HAS_PROSODY_RTP
	    if (_vmp_interface) _vmp_interface->Disable();
#endif //XGATE_HAS_PROSODY_RTP
	} else {
	    Enable();
#ifdef XGATE_HAS_PROSODY_RTP
	    if (_vmp_interface) _vmp_interface->Enable();
#endif //XGATE_HAS_PROSODY_RTP
	}
	break;
    }
}

/** Load: Called once at initialization time
 */
bool ProsodyModule::Load()
{
	    	CLog::Warning(THISMODULE, "inside ProsodyModule::Load() ; \n");
    Cfg_Tree_Group *cfgTree = new Cfg_Tree_Group(Name(),"",VoiceManager::Instance()->GetConfigTree());

    // Register configuration commands
    Term_Cmd *cmd;

    // volume
    cmd = new Term_Cmd(this, Cmd_Volume, "volume", "Volume amplification/attenuation in dB", PrivilegeLevelAdmin, TermModeConfig, false);
    cmd->Add(new Term_Cmd_Arg("arg1", CVariant(0), CVariantValidator(CVariant(0), "Amplification/attenuation in dB")));
    RegisterTermCmd(cmd,cfgTree);

    // (no) automatic-gain
    cmd = new Term_Cmd(this, Cmd_AGC, "automatic-gain", "Automatic volume gain during playback", PrivilegeLevelAdmin, TermModeConfig, true);
    RegisterTermCmd(cmd,cfgTree);

    // dtmf-detection
    cmd = new Term_Cmd (this, Cmd_DtmfDet, "dtmf-detection","Dtmf detection method", PrivilegeLevelAdmin, TermModeConfig, false);
    CVariantChoiceValidator ctValidator("64ms", "Minimum duration 64 msec");
    ctValidator.Add ("40ms", "Minimum duration 40 msec");
    ctValidator.Add ("none", "No minum duration");
    cmd->Add (new Term_Cmd_Arg ("method", CVariant(""), ctValidator));
    RegisterTermCmd (cmd,cfgTree);

    // companding
    cmd = new Term_Cmd (this, Cmd_Companding, "companding","Companding algorithm", PrivilegeLevelAdmin, TermModeConfig, false);
    CVariantChoiceValidator compandingValidator("Alaw", "Alaw algorithm");
    compandingValidator.Add ("MuLaw", "MuLaw algorithm");
    cmd->Add (new Term_Cmd_Arg ("method", CVariant(""), compandingValidator));
    RegisterTermCmd (cmd,cfgTree);

    // tone
    cmd = new Term_Cmd (this, Cmd_Tone, "tone","Define freqency/amplitude used in tone generation", PrivilegeLevelAdmin, TermModeConfig, true);
    CVariantRangeValidator idrange(0, MAXTONE-1, "Tone identifier");
    cmd->Add (new Term_Cmd_Arg ("arg1", CVariant(0), idrange));
    CVariantRangeValidator freqrange(150, 3600, "Tone frequency in Hz");
    cmd->Add(new Term_Cmd_Arg("arg2", CVariant(0), freqrange, TermArgOptional));
    CVariantRangeValidator amplrange(-35, +3, "Tone amplitude in dB");
    cmd->Add(new Term_Cmd_Arg("arg3", CVariant(0), amplrange, TermArgOptional));
    RegisterTermCmd (cmd,cfgTree);

    // (no) voice
    cmd = new Term_Cmd(this, Cmd_Voice, "voice", "voice capability", PrivilegeLevelAdmin, TermModeConfig, true);
    RegisterTermCmd(cmd,cfgTree);

    // (no) conference
    cmd = new Term_Cmd(this, Cmd_Conference, "conference", "conference capability", PrivilegeLevelAdmin, TermModeConfig, true);
    RegisterTermCmd(cmd,cfgTree);

    // (no) echo-canceller
    cmd = new Term_Cmd(this, Cmd_Ecr, "echo-canceller", "echo-canceller capability", PrivilegeLevelAdmin, TermModeConfig, true);
    RegisterTermCmd(cmd,cfgTree);

    // (no) enable
    cmd = new Term_Cmd(this, Cmd_Enable, "enable", "Include this module in resource selection", PrivilegeLevelAdmin, TermModeConfig, true);
    RegisterTermCmd(cmd,cfgTree);

    cfgTree->RegisterShowHandler(this);

    SetConfigTree(cfgTree);

    return true;
}

/** Unload: Called once at termination time
 */
bool ProsodyModule::Unload()
{
    return true;
}

/** Load firmware and initialize the Prosody module
 */
bool ProsodyModule::InitModule()
{
    //We download the firmware using an application, not in code
    //that's why we don't use sm_download_fmw() or sm_reset_module() in here
    int	    rc;

    // load tones
    CLog::Detail(THISMODULE,"Adding tones to module %d",_module_ix);
    SM_OUTPUT_FREQ_PARMS output_freq_parms;
    SM_OUTPUT_TONE_PARMS output_tone_parms;

    INIT_ACU_SM_STRUCT(&output_freq_parms);
    INIT_ACU_SM_STRUCT(&output_tone_parms);

    output_freq_parms.module = _module;
    output_tone_parms.module = _module;
    for (int tone=0;tone<MAXTONE;tone++) {
	if (_tone[tone].freq == 0) continue;

	output_freq_parms.freq = _tone[tone].freq;
	output_freq_parms.amplitude = _tone[tone].ampl;

	rc = sm_add_output_freq(&output_freq_parms);
	if (rc) {
	    CLog::Warning(THISMODULE,"Unable to add new frequency to module %d, freq=%d Hz, amplitude=%d dB, err=%s",_module_ix,_tone[tone].freq,_tone[tone].ampl,ErrMsg(rc));
	    continue;
	}

	output_tone_parms.component1_id = output_freq_parms.id;
	output_tone_parms.component2_id = output_freq_parms.id;
	rc = sm_add_output_tone(&output_tone_parms);
	if (rc) {
	    CLog::Warning(THISMODULE,"Unable to add new tone to module %d, freq=%d Hz, amplitude=%d dB, err=%s",_module_ix,_tone[tone].freq,_tone[tone].ampl,ErrMsg(rc));
	    continue;
	}

	_tone[tone].id = output_tone_parms.id;
    }

    _initialized = true;
    CLog::Detail(THISMODULE,"Module %d initialized",_module_ix);

    return true;
}

/** This function will be called by the service framework once the 
    configuration file has been fully read.
 */
bool ProsodyModule::CommitConfiguration()
{
    if (!InitModule())
	return false;

    return true;
}

/** Returns the module index
 */
tSMModuleId ProsodyModule::GetModule()
{
    return _module;
}

int ProsodyModule::GetModuleIndex()
{
    return _module_ix;
}

/** Allocate a prosody voice resource in this module
 */
ProsodyDevice* ProsodyModule::AllocProsodyDevice()
{
    //printf("inside ProsodyModule::AllocProsodyDevice()\n");
    CLog::Warning(THISMODULE,"inside ProsodyModule::AllocProsodyDevice()");

    if (!IsEnabled() || !_initialized) return NULL;

    if (AculabBusManager::Instance()->GetNumAddressRemaining(_module) == 0) return 0;

    SM_CHANNEL_ALLOC_PLACED_PARMS sm_channel_alloc_parms;
    INIT_ACU_SM_STRUCT(&sm_channel_alloc_parms);
    
    sm_channel_alloc_parms.type = kSMChannelTypeHalfDuplex;
    sm_channel_alloc_parms.caps_mask = 0;
    sm_channel_alloc_parms.module = _module;
    
    int rc = CHKFUNC(sm_channel_alloc_placed(&sm_channel_alloc_parms),"channel_alloc");

    // no more resource
    if (rc == ERR_SM_NO_RESOURCES) return 0;

    if (rc == 0) {

	tSMChannelId channel;
	INIT_ACU_SM_STRUCT(&channel);
	
	channel = sm_channel_alloc_parms.channel;

        CLog::Detail(THISMODULE,"<<<<<<<<< [ Alloc Channel Prosody Device ] Channel %02x \n", channel);

	/****
	// reset channel (blocking and lengthy process)
	rc = CHKFUNC(sm_reset_channel(channel),"reset_channel");
	if (rc) {
	    CLog::Error(THISMODULE,"Unable to reset channel %d, err=%s",channel,ErrMsg(rc));
	    goto errorLbl;
	}
	****/

	// attach this channel to the recognition event 
	SM_CHANNEL_SET_EVENT_PARMS recog_event_parms;
	INIT_ACU_SM_STRUCT(&recog_event_parms);
	
	recog_event_parms.channel = channel;
	recog_event_parms.event_type = kSMEventTypeRecog;
	recog_event_parms.issue_events = kSMAnyChannelEvent;
	rc = CHKFUNC(sm_channel_set_event(&recog_event_parms),"channel_set_event");
	if (rc) {
	    CLog::Error(THISMODULE,"Unable to attach recognition event on channel %d, err=%s",channel,ErrMsg(rc));
	    return (ProsodyDevice*) errorLbl(rc,channel);
	}

	// set the channel to be ready to listen for any dtmf
	SM_LISTEN_FOR_PARMS sm_listen_for_parms;
	INIT_ACU_SM_STRUCT(&sm_listen_for_parms);
	
	sm_listen_for_parms.channel = channel;

	// detect DTMF
	sm_listen_for_parms.tone_detection_mode = (kSMToneDetection)_dtmfdet;
	sm_listen_for_parms.map_tones_to_digits = kSMDTMFToneSetDigitMapping;

	// detect voice activity (silence detector)
	sm_listen_for_parms.enable_grunt_detection = 1;
	sm_listen_for_parms.grunt_latency = 2000;

	rc = CHKFUNC(sm_listen_for(&sm_listen_for_parms),"listen_for");
	if (rc) {
	    CLog::Error(THISMODULE,"Unable to listen for dtmf on channel %d, err=%s",channel,ErrMsg(rc));
	    return reinterpret_cast<ProsodyDevice*>(errorLbl(rc,channel));
	}

	/*****
	SM_CHANNEL_INFO_PARMS sm_channel_info_parms;
	INIT_ACU_SM_STRUCT(&sm_channel_info_parms);
	
	sm_channel_info_parms.channel = channel;
	rc = CHKFUNC(sm_channel_info(&sm_channel_info_parms),"channel_info");
	if (rc) {
	    CLog::Error(THISMODULE,"Unable to get information on channel %d, err=%s",channel,ErrMsg(rc));
	    return (ProsodyDevice*) errorLbl(rc,channel);
	}
	*****/

	//NOTE: swdrv and _card_id is the same thing in V6
	ACU_INT		swdrv		= _card_id; 
	int		stream		= -1;
	int		timeslot	= -1;
	char		devname[32];
	
	sprintf(devname,"pros%u-%x",_module_ix, channel);


	// allocate a device and attach it to the allocated channel
	return new ProsodyDevice(devname, _serial_no, channel, swdrv, stream, timeslot, ProsodyDriver::Instance(), this);
    } else {
	CLog::Error(THISMODULE, "Unable to allocate channel err=%s", ErrMsg(rc));
    }

    return 0;
}

int errorLbl(int rc, tSMChannelId channel )
{
    CLog::Error(THISMODULE, "Unable to allocate new channel, err=%s", ErrMsg(rc));

    rc = CHKFUNC(sm_channel_release(channel),"channel_release");
    if ( rc ) 
	CLog::Fatal(THISMODULE, "Unable to release channel %d, err=%s", channel, ErrMsg(rc));

    return 0;
}

/** Allocate a prosody voice resource in this module for conference
 */
ProsodyDevice* ProsodyModule::AllocProsodyChannel(tSM_INT channel_type, tSM_INT channel_caps)
{
    CLog::Error(THISMODULE,"inside ProsodyModule::AllocProsodyChannel()");
    if (!IsEnabled() || !_initialized) return NULL;

    if (AculabBusManager::Instance()->GetNumAddressRemaining(_module) == 0) return NULL;

    SM_CHANNEL_ALLOC_PLACED_PARMS sm_channel_alloc_parms;
	INIT_ACU_SM_STRUCT(&sm_channel_alloc_parms);

    sm_channel_alloc_parms.type = (kSMChannelType)channel_type;
    sm_channel_alloc_parms.caps_mask = channel_caps;
    sm_channel_alloc_parms.module = _module;
    int rc = CHKFUNC(sm_channel_alloc_placed(&sm_channel_alloc_parms),"channel_alloc");

    // no more resource
    if (rc == ERR_SM_NO_RESOURCES) return NULL;

    if (rc == 0) {

	tSMChannelId channel;
	INIT_ACU_SM_STRUCT(&channel);
	
	channel = sm_channel_alloc_parms.channel;

	/*****
	// reset channel
	rc = CHKFUNC(sm_reset_channel(channel),"reset_channel");
	if (rc) {
	    CLog::Error(THISMODULE,"Unable to reset channel %d, err=%s",channel,ErrMsg(rc));
	    goto errorLbl;
	}
	*****/

	/*****
	SM_CHANNEL_INFO_PARMS sm_channel_info_parms;
	INIT_ACU_SM_STRUCT(&sm_channel_info_parms);

	sm_channel_info_parms.channel = channel;
	rc = CHKFUNC(sm_channel_info(&sm_channel_info_parms),"channel_info");
	if (rc) {
	    CLog::Error(THISMODULE,"Unable to get information on channel %d, err=%s",channel,ErrMsg(rc));
	    return reinterpret_cast<ProsodyDevice*>(errorLbl(rc,channel));
	}
	*****/

	//NOTE: swdrv and _card_id is the same thing in V6
	ACU_INT		swdrv		= _card_id; 
	int		stream		= -1;
	int		timeslot	= -1;
	char devname[32];

	sprintf(devname,"pros%u-%x",_module_ix, channel);


	// allocate a device and attach it to the allocated channel
	return new ProsodyDevice(devname, _serial_no, channel, swdrv, stream, timeslot, ProsodyDriver::Instance(), this);
    }

    return 0;
}


EchoCanceller* ProsodyModule::AllocProsodyEchoCanceller() 
{
    ProsodyDevice* primaryChannel;
    ProsodyDevice* referenceChannel;

    if (!IsEnabled() || !_initialized) return NULL;

    primaryChannel = AllocProsodyChannel(kSMChannelTypeFullDuplex, kSMChannelCapsCondStandAlone);
    if (!primaryChannel) {
	return NULL;
    }

    referenceChannel = AllocProsodyChannel(kSMChannelTypeInput, kSMChannelCapsCondRefInput);
    if (!referenceChannel) {
	primaryChannel->Free();
	return NULL;
    }

    return new ProsodyEchoCanceller(primaryChannel, referenceChannel);
}

bool ProsodyModule::EC_Capable() 
{
/*
    int version, length;
    sm_fwcaps_parms sm_fwcaps_parms;
    sm_fwcaps_parms.module = _module;

    sm_get_firmware_caps(&sm_fwcaps_parms);
    if (smdPSSMFFindSection(&sm_fwcaps_parms,-1,"CND0",&version,&length))
	return true;
*/
	//JAVI : and this?
    /*if (strcmp(_fwfile, "esp30a.smf") == 0 || strcmp(_fwfile, "ecr24a.smf") == 0)
	return true;*/

    return false;
}

/** To enable TiNG trace, set the environment variable TINGTRACE to:

    0 (default) No tracing is enabled.
    1 Tracing of errors is enabled. Whenever an API function returns an error code, a line 
      of output is printed (on standard error) showing the error and the place where the error 
      was generated (a file name and line number withing the API library itself). Since the API 
      library is supplied as source code, you can see why an error is being returned (this is 
      most useful with an error like ERR_SM_BAD_PARAMETER since you can see which parameter 
      caused it to return the error).
    2 Tracing of most API calls is enabled. In addition to the trace produced by level 1, this 
      reports every API function called (except those which read or write data) with the 
      parameters passed and the result returned. This can be used to verify that channels 
      have been set up correctly.
    3 Tracing of all API calls is enabled. This is the same as level 2, except that all API 
      functions are reported.
    4 Tracing of all API calls is enabled and call-progess detection reports the list of simple 
      tones it has receives and that it is comparing against the cadences in the call progress 
      tone table.

    Trace will be written in C:\Switchlab\log\TiNG_traceXX.txt
**/
#include "Critical_Section.h"

extern ACUAPI unsigned TiNGtrace;
extern ACUAPI int (*TiNG_showtrace)(const char *fmt, va_list ap);

#define MAX_TRACEFILE_INDEX	8
#define MAX_TRACEFILE_SIZE	(8*1024*1024)
#ifdef __linux__
#define TRACEFILE_NAME_TEMPLATE	"/var/log/xgate/log/TiNG_trace%02d.txt"
#else
#define TRACEFILE_NAME_TEMPLATE "C:\\Switchlab\\log\\TiNG_trace%02d.txt"
#endif

static Critical_Section cs;

int get_first_tracefile_index()
{
    char filename[128];
    time_t file_time[MAX_TRACEFILE_INDEX];

    // fill the file modification time array
    for (int i=0; i<MAX_TRACEFILE_INDEX; ++i) {
	struct stat st;
	sprintf(filename, TRACEFILE_NAME_TEMPLATE, i);
	if (stat(filename, &st))file_time[i] = 0;
	else file_time[i] = st.st_mtime;
    };

    // determine which is the oldest
    unsigned int choosen_index = 0;
    for (int j=1; j<MAX_TRACEFILE_INDEX; j++) {
	if (file_time[j] < file_time[choosen_index])
	    choosen_index = j;
    }

    return choosen_index;
}

int write_trace_to_file(const char *fmt, va_list ap)
{
    static FILE* outf = 0;
    static int file_num = get_first_tracefile_index();
    static int file_size = 0;

    Lock l(cs);

    if (outf == 0 || file_size > MAX_TRACEFILE_SIZE) {
	if (outf) fclose(outf);
	file_size = 0;

	char fname[128];
	sprintf(fname, TRACEFILE_NAME_TEMPLATE, file_num);
	outf = fopen(fname, "wt");

	if (++file_num == MAX_TRACEFILE_INDEX) file_num = 0;
    }

    int rc = 0;
    if (outf) {
	rc = vfprintf(outf, fmt, ap);
	if (rc >= 0) file_size += rc;
	fflush(outf);
    }

    return rc;
}

void TiNG_set_loglevel(int new_level)
{
    TiNGtrace = new_level;
}

int TiNG_get_loglevel()
{
    return TiNGtrace;
}


/*----------------------------------------------------------------* 
 * Implementation of ProsodyFactory
 *----------------------------------------------------------------*/
ProsodyFactory::ProsodyFactory() : ServiceProvider("ProsodyFactory",false,false)
{
    // redirect TiNG debug output to file
    TiNG_showtrace = write_trace_to_file;

    // get the number of prosody modules installed
    ACU_ERR		error;
    
    /* Interrogate System */
    ACU_SNAPSHOT_PARMS rSnapshotParms;
    INIT_ACU_STRUCT(&rSnapshotParms);
    error = acu_get_system_snapshot(&rSnapshotParms);
    if (error) {
	/////CLog::Error(THISMODULE,"Unable to interrogate system (%s)", ErrMsg(error)); // original code commented by IPPBX
	return;
    }
    
    /* Open all available cards */
    ACU_OPEN_CARD_PARMS		rOpenParms;
    ACU_CARD_INFO_PARMS		rCardInfo;
    ACU_OPEN_SWITCH_PARMS	rOpenSwitch;
    ACU_OPEN_PROSODY_PARMS	rOpenSpeech;
    SM_OPEN_MODULE_PARMS	sOpenModule;
    SM_CARD_INFO_PARMS		sCardInfo;
    SM_MODULE_INFO_PARMS	mod_info;
    
    int currentModule = 0;
    for (ACU_UINT c = 0; c < rSnapshotParms.count; c++)
    {
	INIT_ACU_STRUCT(&rOpenParms);
	strcpy(rOpenParms.serial_no, rSnapshotParms.serial_no[c]);
	
	/* Open card */
	error = acu_open_card(&rOpenParms);
	if (error) {
	    CLog::Error(THISMODULE,"Unable to open card %s; Error = (%s)", rSnapshotParms.serial_no[c], ErrMsg(error));
	    continue;
	}
	
	/* Get Card Info */
	INIT_ACU_STRUCT(&rCardInfo);
	rCardInfo.card_id = rOpenParms.card_id;
	error = acu_get_card_info(&rCardInfo);
	if (error) {
	    CLog::Error(THISMODULE,"Unable to obtain card %s resource info; Error = (%s)", rSnapshotParms.serial_no[c], ErrMsg(error));
	    continue;
	}
	
	if (rCardInfo.resources_available & ACU_RESOURCE_SWITCH)
	{
	    /* Open Switch Driver Interface */
	    INIT_ACU_STRUCT(&rOpenSwitch);
	    rOpenSwitch.card_id = rOpenParms.card_id;
	    error = acu_open_switch(&rOpenSwitch);
	    if(error) {
		CLog::Error(THISMODULE,"Unable to open switch driver of card %s; Error = (%s)", rSnapshotParms.serial_no[c], ErrMsg(error));
		continue;
	    }
	}
	
	if (rCardInfo.resources_available & ACU_RESOURCE_SPEECH)
	{
	    /* Open Speech Driver Interface */
	    INIT_ACU_STRUCT(&rOpenSpeech);
	    rOpenSpeech.card_id = rOpenParms.card_id;
	    error = acu_open_prosody(&rOpenSpeech);
	    
	    if (error) {
		CLog::Error(THISMODULE,"Unable to open prosody in card %s; Error = (%s)", rSnapshotParms.serial_no[c], ErrMsg(error));
		continue;
	    } else {
		_num_cards++;
	    }
	    
	    /* Get Speech Module Count */
	    INIT_ACU_SM_STRUCT(&sCardInfo);
	    sCardInfo.card = rOpenParms.card_id;
	    error = sm_get_card_info(&sCardInfo);
	    if (error) {
		CLog::Error(THISMODULE,"Unable to obtain card %s speech info; Error = (%s)", rSnapshotParms.serial_no[c], ErrMsg(error));
	    }
	    
	    _vCardId.push_back(sCardInfo.card);
	    _num_modules += sCardInfo.module_count; 
	    
	    for(int i=0;i<sCardInfo.module_count;i++,currentModule++)
	    {
		INIT_ACU_SM_STRUCT(&sOpenModule);
		sOpenModule.module_ix = i;
		sOpenModule.card_id = rOpenParms.card_id;
		error = sm_open_module(&sOpenModule);
		if (error) {
		    CLog::Error(THISMODULE,"Unable to open card %s speech module; Error = (%s)", rSnapshotParms.serial_no[c], ErrMsg(error));
		    continue;
		}
		
		INIT_ACU_SM_STRUCT( &mod_info );
		mod_info.module = sOpenModule.module_id;
		error = sm_get_module_info( &mod_info );
		if (error) {
		    CLog::Error(THISMODULE,"Unable to obtain information of module %d on card %d; Error = (%s)", i, sOpenModule.card_id, ErrMsg(error));
		    continue;
		}
		
		AculabBusManager::Instance()->RegisterModule( rSnapshotParms.serial_no[c], sOpenModule.card_id, sOpenModule.module_id, mod_info.min_stream, 
		    (rCardInfo.card_type == ACU_PROSODY_X_CARD)? K_PROSODY_X_TIMESLOTS_PER_STREAM : K_PROSODY_CPCI_TIMESLOTS_PER_STREAM );
		
		char name[32];
		sprintf(name,"prosody%u",currentModule);
		new ProsodyModule(
		    sOpenModule.card_id,
		    sOpenModule.module_id,
		    currentModule,
		    name, 
		    kSMTimeslotTypeALaw, 
		    (rCardInfo.card_type == ACU_PROSODY_X_CARD)? 180 : 0,
		    rCardInfo.serial_no,
		    rCardInfo.hw_version,
		    rCardInfo.card_name,
		    rCardInfo.card_desc,
		    rCardInfo.ip_address,
		    rCardInfo.card_key);
	    }
	}
    }
    
    if(_num_modules <= 0) 
    {
	CLog::Info(THISMODULE,"Prosody not installed");
	return;
    }
    /* Narsim added */
    TiNGtrace = 4; 
}

ProsodyFactory::~ProsodyFactory()
{
}

/** Create: create one instance first before returning a pointer to it
 */
ProsodyFactory* ProsodyFactory::Create()
{
    assert(theProsodyFactory == NULL);
    theProsodyFactory = new ProsodyFactory;
    return Instance();
}

/** Instance: Returns a pointer to this singleton instance, if necessary
    create one instance first before returning a pointer to it
 */
ProsodyFactory* ProsodyFactory::Instance()
{
    return theProsodyFactory;
}

void ProsodyFactory::Destroy()
{
    delete theProsodyFactory;
    theProsodyFactory = NULL;
}

/** This function will be called by the service framework after 
    instantiation of this class, to allow the instance to initialize
    itself.
    Initialize Prosody driver and for each Prosody port, create the associated
    Interface object
 */
bool ProsodyFactory::Load()
{
    return true;
}

/** This function will be called by the service framework before it is
    destroyed/deleted, to allow the instance to cleanup itself.
    Close Prosody driver
 */
bool ProsodyFactory::Unload()
{
    return true;
}

int ProsodyFactory::GetNumCards()
{
    return _num_cards;
}

int ProsodyFactory::GetNumModules()
{
    return _num_modules;
}

bool ProsodyFactory::CommitConfiguration()
{
#ifdef XGATE_HAS_LM
    int cards;

    cards = GetNumCards();

    if (cards < 1)
	return true;

    for (int i=0; i<cards; ++i) {
    	ACU_CARD_INFO_PARMS	    rCardInfo;
	INIT_ACU_STRUCT(&rCardInfo);
    	
	char sernum[20];

    	rCardInfo.card_id = _vCardId[i];

   	int rc = acu_get_card_info(&rCardInfo);
	if (rc == 0) {
    	    memcpy(sernum, rCardInfo.serial_no, kSMMaxSerialNoText);
	    sernum[kSMMaxSerialNoText] = '\0';
	} else {
	    sprintf(sernum, "PROS%02d", i);
	}

	LicenceManager::Instance()->RegisterSerialNum(Fg_Prosody, i/*this->GetModule() DG changed*/, sernum, strlen(sernum));
    }


#endif

    return true;
}

/*----------------------------------------------------------------* 
 * Implementation of VoiceDevice
 *----------------------------------------------------------------*/


/** ProsodyDriver's constructor
 */
ProsodyDriver::ProsodyDriver() : 
    VoiceDriver(THISMODULE, false, true, Lowest_Priority)
{
    _bInitialized = false;
    _tmlasttimer = 0;
    _nextModule = 0;

    //Removed ifndef __linux__ 10092016
    _eventQuit = NULL;
    _eventCancel = NULL;
    _eventStart = NULL;
    _eventRecord = NULL; // T REC
    _eventCompletion = NULL;
    
    /* Handles events like ivr, dtmf, tone, status events */
    _event_flags = 0;
    _activePlayDtmf = 0;
    _activePlayTone = 0;

    _bkgndJobCount = 0;
    _allocCount = 0;

    _poll_counter = 0;

    // Default values for runtime parameters
    _distMethod = PROS_DISTRIBUTION_CYCLIC;

    INIT_ACU_SM_STRUCT(&_recogParms);
    INIT_ACU_SM_STRUCT(&_writeParms);
    INIT_ACU_SM_STRUCT(&_readParms);
}

/** ProsodyDriver's destructor
 */
ProsodyDriver::~ProsodyDriver()
{
    CloseHandles();
}

/** Create the only instance of this class
 */
ProsodyDriver *ProsodyDriver::Create()
{
    assert(!_theProsodyDriver);
    _theProsodyDriver = new ProsodyDriver();
    return _theProsodyDriver;
}

/** Return a pointer to the only instance of this class
 */
ProsodyDriver *ProsodyDriver::Instance()
{
    return _theProsodyDriver;
}

/** Increment background job counter
 */
void ProsodyDriver::IncBkgndJobCounter()
{
    _bkgndJobCount++;
}

/** Decrement background job counter
 */
void ProsodyDriver::DecBkgndJobCounter()
{
    _bkgndJobCount--;
}

/** Retrieve the number of current background jobs
 */
unsigned ProsodyDriver::GetBkgndJobCounter()
{
    return _bkgndJobCount;
}

/** Increment the number of allocated voice device
 */
void ProsodyDriver::IncAllocCounter()
{
    _allocCount++;
}

/** Decrement the number of allocated voice device
 */
void ProsodyDriver::DecAllocCounter()
{
    _allocCount--;
}

/** Retrieve the number of allocated voice device
 */
unsigned ProsodyDriver::GetAllocCounter()
{
    return _allocCount;
}

/** This function will be called by the service framework after 
    instantiation of this class, to allow the instance to initialize
    itself.
 */
bool ProsodyDriver::Load()
{
    if (ProsodyFactory::GetNumModules() <= 0) {
	CLog::Info(THISMODULE,"Skipping Prosody initialization");
	return true;
    }

    int rc;

    CLog::Detail(THISMODULE,"Initializing Prosody");


    /** Initializes recognition event which is triggered for 
	each DTMF detection
     */
    _recogParms = new m_event_t_;
    _recogParms->_parms.event = new tSMEventId;

    rc = smd_ev_create(
	    (tSMEventId *)_recogParms->_parms.event,
	    kSMNullChannelId,
	    kSMEventTypeRecog,
	    kSMAnyChannelEvent);
    if (rc) {
	CLog::Error(THISMODULE,"Unable to create recognition event object, err=%s",ErrMsg(rc));
	CloseHandles();
	//printf("Unable to create recognition event\n");
	return false;
    }

    CLog::Detail(THISMODULE,"Initializing Prosody 1");
    _recogParms->_parms.channel = kSMNullChannelId;
    _recogParms->_parms.event_type = kSMEventTypeRecog;
    _recogParms->_parms.issue_events = kSMAnyChannelEvent;
    rc = sm_channel_set_event((sm_channel_set_event_parms *)&_recogParms->_parms);
    if (rc) {
	CLog::Error(THISMODULE,"Unable to set recognition event object, err=%s",ErrMsg(rc));
	CloseHandles();
	return false;
    }

    /** Initializes write data event which is triggered several times
	during play file
     */
    _writeParms = new m_event_t_;
    _writeParms->_parms.event = new tSMEventId;

    rc = smd_ev_create(
	    (tSMEventId *)_writeParms->_parms.event,
	    kSMNullChannelId,
	    kSMEventTypeWriteData,
	    kSMAnyChannelEvent);

    if (rc) {
	CLog::Error(THISMODULE,"Unable to create write event object, err=%s",ErrMsg(rc));
	CloseHandles();
	return false;
    }
    _writeParms->_parms.channel = kSMNullChannelId;
    _writeParms->_parms.event_type = kSMEventTypeWriteData;
    _writeParms->_parms.issue_events = kSMAnyChannelEvent;
    rc = sm_channel_set_event((sm_channel_set_event_parms *)&_writeParms->_parms);
    
    if (rc) {
	CLog::Error(THISMODULE,"Unable to set write event object, err=%s",ErrMsg(rc));
	CloseHandles();
	return false;
    }

    CLog::Detail(THISMODULE,"Initializing Prosody 2");
    /** Initializes read data event which is triggered several times
	during record file
     */
    _readParms = new m_event_t_;
    _readParms->_parms.event = new tSMEventId;
    rc = smd_ev_create(
	    (tSMEventId *)_readParms->_parms.event,
	    kSMNullChannelId,
	    kSMEventTypeReadData,
	    kSMAnyChannelEvent);
    if (rc) {
	CLog::Error(THISMODULE,"Unable to create read event object, err=%s",ErrMsg(rc));
	CloseHandles();
	return false;
    }
    _readParms->_parms.channel = kSMNullChannelId;
    _readParms->_parms.event_type = kSMEventTypeReadData;
    _readParms->_parms.issue_events = kSMAnyChannelEvent;
    rc = sm_channel_set_event((sm_channel_set_event_parms *)&_readParms->_parms);
    if (rc) {
	CLog::Error(THISMODULE,"Unable to set read event object, err=%s",ErrMsg(rc));
	CloseHandles();
	return false;
    }

    // event to signal xGate termination
#ifndef __linux__
    _eventQuit = CreateEvent(NULL,TRUE,FALSE,NULL);
    if (_eventQuit == NULL) {
	CLog::Error(THISMODULE,"Unable to create termination event, error=%d",GetLastError());
	CloseHandles();
	return false;
    }

    // event to signal a job abortion
    _eventCancel = CreateEvent(NULL,FALSE,FALSE,NULL);
    if (_eventCancel == NULL) {
	CLog::Error(THISMODULE,"Unable to create termination event, error=%d",GetLastError());
	CloseHandles();
	return false;
    }

    _eventStart = CreateEvent(NULL,FALSE,FALSE,NULL);
    if (_eventStart == NULL) {
	CLog::Error(THISMODULE,"Unable to create job start event, error=%d",GetLastError());
	CloseHandles();
	return false;
    }

    // event to signal the end of a job
    _eventCompletion = CreateEvent(NULL,FALSE,FALSE,NULL);
    if (_eventCompletion == NULL) {
	CLog::Error(THISMODULE,"Unable to create completion event, error=%d",GetLastError());
	CloseHandles();
	return false;
    }

    // to protect access to shared resources
    InitializeCriticalSection(&_csection);

    DWORD tid;
    _hIOThread = CREATE_THREAD(NULL,0,IOThread,0,0,&tid);
    if (_hIOThread == NULL) {
	CLog::Error(THISMODULE,"Unable to start I/O thread, errno=%d",errno);
	CloseHandles();
	return false;
    }
    
    SetThreadPriority(_hIOThread, THREAD_PRIORITY_HIGHEST);
#else
#ifdef COND_WAIT
    _eventQuit = CreateEvent();
    if (_eventQuit == NULL) {
//	CLog::Error(THISMODULE,"Unable to create termination event, error=%d",GetLastError());
	DestroyEvent(_eventQuit);
	return false;
    }

    PLOG_INFO("\n Quit Event Created = %02x \n", _eventQuit);

    // event to signal a job abortion
    _eventCancel = CreateEvent();
    if (_eventCancel == NULL) {
//	CLog::Error(THISMODULE,"Unable to create termination event, error=%d",GetLastError());
	DestroyEvent(_eventQuit);
	DestroyEvent(_eventCancel);
	return false;
    }

    PLOG_INFO("\n Cancel Event Created = %02x \n", _eventCancel);

    _eventStart = CreateEvent();
    if (_eventStart == NULL) {
//	CLog::Error(THISMODULE,"Unable to create job start event, error=%d",GetLastError());
	DestroyEvent(_eventQuit);
	DestroyEvent(_eventCancel);
	DestroyEvent(_eventStart);
	return false;
    }

    PLOG_INFO("\n Start Event Created = %02x \n", _eventStart);

    // T REC
    _eventRecord = CreateEvent();
    if (_eventRecord == NULL) {
    DestroyEvent(_eventQuit);
    DestroyEvent(_eventCancel);
    DestroyEvent(_eventStart);
    return false;
    }
    PLOG_INFO("\n Record Event Created = %02x \n", _eventRecord);

    // event to signal the end of a job
    _eventCompletion = CreateEvent();
    if (_eventCompletion == NULL) {
//	CLog::Error(THISMODULE,"Unable to create completion event, error=%d",GetLastError());
	DestroyEvent(_eventQuit);
	DestroyEvent(_eventCancel);
	DestroyEvent(_eventStart);
	DestroyEvent(_eventCompletion);
	return false;
    }

    PLOG_INFO("\n Completion Event Created = %02x \n", _eventCompletion);
    // to protect access to shared resources
//    InitializeCriticalSection(&_csection);
#endif

    if( pthread_create(&_hIOThread,NULL,&IOThread,(void *) this) != 0) {  // thread_process argument passed here is NULL, CHANGE according to the actual scenario
	CLog::Error(THISMODULE,"Unable to start I/O thread, errno=%d",errno);
        CloseHandles();
        return false;
    }
    
#endif

    _bInitialized = true;

    return true;
}

#ifdef COND_WAIT
m_event_t ProsodyDriver::CreateEvent(bool manualReset, bool initialState)
{
    m_event_t event = new m_event_t_;

    PLOG("CREATE EVENT called");

    int result = pthread_cond_init(&event->CVariable, 0);
    assert(result == 0);

    result = pthread_mutex_init(&event->Mutex, 0);
    assert(result == 0);

    event->State = false;
    event->AutoReset = !manualReset;

    if (initialState)
    {
        result = SetEvent(event);
        std::cout << "EVENT = " << event << std::endl;
        assert(result == 0);
    }

    return event;
}

bool RemoveExpiredWaitHelper(m_wfmo_info_t_ wait)
{

   //PLOG(" wait helper wait = %02x", wait);
#if 1
   int result = pthread_mutex_trylock(&wait.Waiter->Mutex);

   if (result == EBUSY)
   {
       return false;
   }

   assert(result == 0);

   //PLOG(" Still waiting = %d \n", wait.Waiter->StillWaiting);
   if (wait.Waiter->StillWaiting == false)
   {
       --wait.Waiter->RefCount;
       assert(wait.Waiter->RefCount >= 0);
       if (wait.Waiter->RefCount == 0)
       {
           wait.Waiter->Destroy();
           delete wait.Waiter;
       }
       else
       {
           result = pthread_mutex_unlock(&wait.Waiter->Mutex);
           assert(result == 0);
       }

       return true;
   }

   PLOG(" pthread_mutex_unlock \n");
   result = pthread_mutex_unlock(&wait.Waiter->Mutex);
   assert(result == 0);
#endif

   return 0;
}

int ProsodyDriver::DestroyEvent(m_event_t event)
{
   int result = 0;
   m_wfmo_info_t_ wait;

   PLOG(" Destroy Event = %02x\n", event);
#ifdef WFMO
   result = pthread_mutex_lock(&event->Mutex);
   assert(result == 0);
   PLOG(" Registered Wait Erase ");
   event->RegisteredWaits.erase(std::remove_if(event->RegisteredWaits.begin(), event->RegisteredWaits.end(), RemoveExpiredWaitHelper), event->RegisteredWaits.end());

   
   result = pthread_mutex_unlock(&event->Mutex);
   assert(result == 0);
#endif

   PLOG(" pthread_cond_destroy");
   result = pthread_cond_destroy(&event->CVariable);
   assert(result == 0);

   PLOG(" pthread_mutex_destroy");
   result = pthread_mutex_destroy(&event->Mutex);
   assert(result == 0);

   delete event;

   return 0;
}

int ProsodyDriver::DestroyEvents()
{
   
   PLOG(" pthread_mutex_destroy Events");
   return 0;
}

int ProsodyDriver::WaitForEvent(m_event_t event, uint64_t milliseconds)
{
   int tempResult;
   PLOG(" Event = %02x", event);
   if (milliseconds == 0)
   {
       tempResult = pthread_mutex_trylock(&event->Mutex);
       if (tempResult == EBUSY)
       {
           PLOG(" Wait Time out tempResult = %d", tempResult);
           return WAIT_TIMEOUT;
       }
   }
   else
   {
       tempResult = pthread_mutex_lock(&event->Mutex);
	   PLOG(" tempResult = %d", tempResult);
   }

   assert(tempResult == 0);

   int result = UnlockedWaitForEvent(event, milliseconds);
   PLOG(" UnlockedWaitForEvent result = %d", result);

   tempResult = pthread_mutex_unlock(&event->Mutex);
   PLOG(" mutex Unlock = %d", tempResult);
   assert(tempResult == 0);

   return result;
}

int ProsodyDriver::SetEvent(m_event_t event)
{
   int result = pthread_mutex_lock(&event->Mutex);
   PLOG(" Set event mutex lock = %d", result);
   assert(result == 0);

   event->State = true;
   PLOG(" event state = %d, Autoreset = %d",  event->State, event->AutoReset);
   //PLOG_INFO(" event state = %d, Autoreset = %d",  event->State, event->AutoReset);

   //Depending on the event type, we either trigger everyone or only one
   if (event->AutoReset)
   {
       PLOG(" Event Auto reset True");
#ifdef WFMO
       while (!event->RegisteredWaits.empty())
       {
           PLOG(" Event RegisteredWaits NOT empty");
           m_wfmo_info_t i = &event->RegisteredWaits.front();

           result = pthread_mutex_lock(&i->Waiter->Mutex);
           assert(result == 0);

           --i->Waiter->RefCount;
           assert(i->Waiter->RefCount >= 0);
           if (!i->Waiter->StillWaiting)
           {
               if (i->Waiter->RefCount == 0)
               {
                   i->Waiter->Destroy();
                   delete i->Waiter;
               }
               else
               {
                   result = pthread_mutex_unlock(&i->Waiter->Mutex);
                   assert(result == 0);
               }
               event->RegisteredWaits.pop_front();
               continue;
           }

           event->State = false;
           PLOG(" event->State = %d, i->Waiter->WaitAll = %d", event->State, i->Waiter->WaitAll);

           if (i->Waiter->WaitAll)
           {
               --i->Waiter->Status.EventsLeft;
               assert(i->Waiter->Status.EventsLeft >= 0);
               //We technically should do i->Waiter->StillWaiting = Waiter->Status.EventsLeft != 0
               //but the only time it'll be equal to zero is if we're the last event, so no one
               //else will be checking the StillWaiting flag. We're good to go without it.
           }
           else
           {
               i->Waiter->Status.FiredEvent = i->WaitIndex;
               i->Waiter->StillWaiting = false;
               PLOG("FiredEvent = %d", i->Waiter->Status.FiredEvent); 
           }

           PLOG("Mutex Unlock");
           result = pthread_mutex_unlock(&i->Waiter->Mutex);
           assert(result == 0);

           PLOG_INFO("pthread_cond_signal ");
           PLOG_INFO("pthread_cond_signal %02x", &i->Waiter->CVariable);
           result = pthread_cond_signal(&i->Waiter->CVariable);
           assert(result == 0);

           PLOG("RegisteredWaits.pop_front");
           event->RegisteredWaits.pop_front();

           PLOG("Mutex Unlock...");
           result = pthread_mutex_unlock(&event->Mutex);
           assert(result == 0);

           return 0;
       }
#endif
       PLOG_INFO("event->State = %d...", event->State);
       if (event->State)
       {
           PLOG("event->State = %d..., mutex unlock ", event->State);
           result = pthread_mutex_unlock(&event->Mutex);
           assert(result == 0);

           PLOG_INFO("event->State = %d..., Cond Signal... %02x", event->State, &event->CVariable);
           result = pthread_cond_signal(&event->CVariable);
           assert(result == 0);

           return 0;
       }
   }
   else
   {
#ifdef WFMO
       PLOG(" RegisteredWaits.size = %d \n",  event->RegisteredWaits.size());
       for (size_t i = 0; i < event->RegisteredWaits.size(); ++i)
       {
           m_wfmo_info_t info = &event->RegisteredWaits[i];

           PLOG(" RegisteredWaits i = %d\n", i);
           result = pthread_mutex_lock(&info->Waiter->Mutex);
           assert(result == 0);

           --info->Waiter->RefCount;
           assert(info->Waiter->RefCount >= 0);

           if (!info->Waiter->StillWaiting)
           {
               PLOG(" RegisteredWaits NOT Still waiting\n");
               if (info->Waiter->RefCount == 0)
               {
                   PLOG(" RegisteredWaits RefCount == 0... Calling waiter Destroy\n");
                   info->Waiter->Destroy();
                   delete info->Waiter;
               }
               else
               {
                   PLOG(" RegisteredWaits Mutex Unlock");
                   result = pthread_mutex_unlock(&info->Waiter->Mutex);
                   assert(result == 0);
               }
               continue;
           }

           if (info->Waiter->WaitAll)
           {
               --info->Waiter->Status.EventsLeft;
               assert(info->Waiter->Status.EventsLeft >= 0);
               //We technically should do i->Waiter->StillWaiting = Waiter->Status.EventsLeft != 0
               //but the only time it'll be equal to zero is if we're the last event, so no one
               //else will be checking the StillWaiting flag. We're good to go without it.
           }
           else
           {
               info->Waiter->Status.FiredEvent = info->WaitIndex;
               info->Waiter->StillWaiting = false;
           }

           result = pthread_mutex_unlock(&info->Waiter->Mutex);
           assert(result == 0);

           PLOG_INFO("Cond Signal... %02x", &info->Waiter->CVariable);
           result = pthread_cond_signal(&info->Waiter->CVariable);
           assert(result == 0);
       }
       event->RegisteredWaits.clear();
#endif
       result = pthread_mutex_unlock(&event->Mutex);
       assert(result == 0);

       result = pthread_cond_broadcast(&event->CVariable);
       assert(result == 0);
   }

   PLOG("Return 0");
   return 0;
}

int ProsodyDriver::ResetEvent(m_event_t event)
{
   int result = pthread_mutex_lock(&event->Mutex);
   PLOG("result = %d\n", result);
   assert(result == 0);

   event->State = false;

   result = pthread_mutex_unlock(&event->Mutex);
   assert(result == 0);

   return 0;
}

int ProsodyDriver::UnlockedWaitForEvent(m_event_t event, uint64_t milliseconds)
{
   int result = 0;

   //PLOG("result = %d, event->state = %d, event->AutoReset = %d\n", result, event->State, event->AutoReset);

   if (!event->State)
   {
       //Zero-timeout event state check optimization
       if (milliseconds == 0)
       {
           return WAIT_TIMEOUT;
       }

       timespec ts;
       if (milliseconds != (uint64_t) -1)
       {
           timeval tv;
           gettimeofday(&tv, NULL);

           uint64_t nanoseconds = ((uint64_t) tv.tv_sec) * 1000 * 1000 * 1000 + milliseconds * 1000 * 1000 + ((uint64_t) tv.tv_usec) * 1000;

           ts.tv_sec = nanoseconds / 1000 / 1000 / 1000;
           ts.tv_nsec = (nanoseconds - ((uint64_t) ts.tv_sec) * 1000 * 1000 * 1000);
       }

       do
       {
           //Regardless of whether it's an auto-reset or manual-reset event:
           //wait to obtain the event, then lock anyone else out
           if (milliseconds != (uint64_t) -1)
           {
               PLOG("Calling pthread_cond_timedwait\n");
               result = pthread_cond_timedwait(&event->CVariable, &event->Mutex, &ts);
           }
           else
           {
               PLOG("Calling pthread_cond_wait %02x\n", &event->CVariable);
               result = pthread_cond_wait(&event->CVariable, &event->Mutex);
           }
       } while (result == 0 && !event->State);

       if (result == 0 && event->AutoReset)
       {
           //We've only accquired the event if the wait succeeded
           event->State = false;
           PLOG("Setting event state to False\n");
       }
   }
   else if (event->AutoReset)
   {
       //It's an auto-reset event that's currently available;
       //we need to stop anyone else from using it
       result = 0;
       event->State = false;
       PLOG("Auto Resett True...Setting event state to False\n");
   }
   //Else we're trying to obtain a manual reset event with a signaled state;
   //don't do anything

   PLOG("Return Result = %d\n", result);
   return result;
}

int ProsodyDriver::WaitForMultipleEvents(m_event_t *events, int count, bool waitAll, uint64_t milliseconds)
{
   int unused = 123;
   PLOG("unused = %d\n", unused);
   return WaitForMultipleEvents(events, count, waitAll, milliseconds, unused);
}

int ProsodyDriver::WaitForMultipleEvents(m_event_t *events, int count, bool waitAll, uint64_t milliseconds, int &waitIndex)
{
   m_wfmo_t wfmo = new m_wfmo_t_;
   PLOG_INFO("WAIT FOR Multiple Events \n");

   int result = 0;
   int tempResult = pthread_mutex_init(&wfmo->Mutex, 0);
   assert(tempResult == 0);

   tempResult = pthread_cond_init(&wfmo->CVariable, 0);
   assert(tempResult == 0);


   m_wfmo_info_t_ waitInfo;
   waitInfo.Waiter = wfmo;
   waitInfo.WaitIndex = -1;

   wfmo->WaitAll = waitAll;
   wfmo->StillWaiting = true;
   wfmo->RefCount = 1;

   if (waitAll)
   {
       wfmo->Status.EventsLeft = count;
   }
   else
   {
       wfmo->Status.FiredEvent = -1;
   }

//   tempResult = pthread_mutex_lock(&wfmo->Mutex);
//   assert(tempResult == 0);

   bool done = false;
   waitIndex = -1;

   PLOG("WAIT FOR Multiple Events count = %d\n", count);
   for (int i = 0; i < count; ++i)
   {
       waitInfo.WaitIndex = i;

       PLOG(" MUTEX LOCK i = %d\n", i);
       //Must not release lock until RegisteredWait is potentially added
       tempResult = pthread_mutex_lock(&events[i]->Mutex);
       assert(tempResult == 0);

       //Before adding this wait to the list of registered waits, let's clean up old, expired waits while we have the event lock anyway
       PLOG("\n RegisteredWaits Erase\n");
       events[i]->RegisteredWaits.erase(std::remove_if (events[i]->RegisteredWaits.begin(), events[i]->RegisteredWaits.end(), RemoveExpiredWaitHelper), events[i]->RegisteredWaits.end());

       //PLOG("WAIT FOR Multiple Events Unlocked Wait for event called = %d\n", count);
       if (UnlockedWaitForEvent(events[i], 0) == 0)
       {
           tempResult = pthread_mutex_unlock(&events[i]->Mutex);
           assert(tempResult == 0);

           if (waitAll)
           {
               --wfmo->Status.EventsLeft;
               assert(wfmo->Status.EventsLeft >= 0);
           }
           else
           {
               wfmo->Status.FiredEvent = i;
               waitIndex = i;
               done = true;
               break;
           }
       }
       else
       {
           events[i]->RegisteredWaits.push_back(waitInfo);
           ++wfmo->RefCount;

           tempResult = pthread_mutex_unlock(&events[i]->Mutex);
           assert(tempResult == 0);
       }
   }

   PLOG_INFO("WAIT FOR Multiple Events done = %d\n", !done);
   timespec ts;
   if (!done)
   {
       if (milliseconds == 0)
       {
           result = WAIT_TIMEOUT;
           done = true;
       }
       else if (milliseconds != (uint64_t) -1)
       {
           timeval tv;
           gettimeofday(&tv, NULL);

           uint64_t nanoseconds = ((uint64_t) tv.tv_sec) * 1000 * 1000 * 1000 + milliseconds * 1000 * 1000 + ((uint64_t) tv.tv_usec) * 1000;

           ts.tv_sec = nanoseconds / 1000 / 1000 / 1000;
           ts.tv_nsec = (nanoseconds - ((uint64_t) ts.tv_sec) * 1000 * 1000 * 1000);
       }
   }

   PLOG_INFO("done = %d\n", done);
   while (!done)
   {
       //One (or more) of the events we're monitoring has been triggered?

       //If we're waiting for all events, assume we're done and check if there's an event that hasn't fired
       //But if we're waiting for just one event, assume we're not done until we find a fired event
       done = (waitAll && wfmo->Status.EventsLeft == 0) || (!waitAll && wfmo->Status.FiredEvent != -1);

       //PLOG("done = %d milliseconds = %d\n", done, milliseconds);
       if (!done)
       {
           if (milliseconds != (uint64_t) -1)
           {
               PLOG_INFO("done = %d cond timed wait \n", done);
               result = pthread_cond_timedwait(&wfmo->CVariable, &wfmo->Mutex, &ts);
           }
           else
           {
               PLOG_INFO("cond wait %02x\n", &wfmo->CVariable);
               result = pthread_cond_wait(&wfmo->CVariable, &wfmo->Mutex);
               PLOG_INFO("done = %d result %d\n", done, result);
           }

           if (result != 0)
           {
               break;
           }
       }
   }

   waitIndex = wfmo->Status.FiredEvent;
   wfmo->StillWaiting = false;

   --wfmo->RefCount;
   assert(wfmo->RefCount >= 0);
   if (wfmo->RefCount == 0)
   {
       wfmo->Destroy();
       delete wfmo;
   }
   else
   {
       tempResult = pthread_mutex_unlock(&wfmo->Mutex);
       assert(tempResult == 0);
   }

   return result;
}



#endif

/** This function will be called by the service framework before it is
    destroyed/deleted, to allow the instance to cleanup itself.
 */
bool ProsodyDriver::Unload()
{
    if (!_bInitialized) return true;


    MapProsody::iterator it=_devices.begin();
    for (; it != _devices.end(); it++) {
	ProsodyDevice *dev = (*it).second;
	if (dev) dev->DecRefCount();
    }

    // instruct I/O thread to suicide
#ifndef __linux__
    SetEvent(_eventQuit);
    if (WaitForSingleObject(_hIOThread,5000) == WAIT_TIMEOUT) {
	CLog::Warning(THISMODULE,"Unable to stop I/O thread");
    }
#else
      SetEvent(Instance()->_eventQuit);
      /* Signal Event Quit */
//      ProsodyDriver::Instance()->_event_flags |= _quitEvent;
      //pthread_cond_signal(&eventCond);
#endif

    CloseHandles();
    _bInitialized = false;

    return true;
}

/** Insert a module to the active list
 */
void ProsodyDriver::InsertProsodyModule(ProsodyModule *module)
{
    while (_modules.size() < module->GetModuleIndex()+1) 
	{
		_modules.push_back(0);
	}

    _modules[module->GetModuleIndex()] = module;
}

/** Take out a module from the list of active modules
 */
void ProsodyDriver::RemoveProsodyModule(ProsodyModule *module)
{
    _modules[module->GetModuleIndex()] = NULL;
}


#ifdef XGATE_HAS_PROSODY_RTP

/** Find a Prosody/X interface from name
 */
Interface* ProsodyDriver::GetInterface(const char* prosm_name)
{
    std::list<VMP_Interface*>::iterator it;
    for (it = list_vmps.begin(); it != list_vmps.end(); it++)
	if (strcmp((*it)->GetName(), prosm_name) == 0)
	    return *it;

    return 0;
}

/** Allocate a VoIP channel from a given Prosody/X module
 */
RTP_Channel* ProsodyDriver::AllocateChannel(const char* prosm_name)
{
    VMP_Interface* itf = (VMP_Interface*) GetInterface(prosm_name);
    if (!itf) return 0;

    return itf->Allocate_VMP_RTP_Channel();
}

#endif //XGATE_HAS_PROSODY_RTP


/** Close all opened handles
 */
void ProsodyDriver::CloseHandles()
{
    PLOG("CLOSE HANDLES \n");
#ifndef __linux__
    if (_eventQuit) CloseHandle(_eventQuit);
    if (_eventCancel) CloseHandle(_eventCancel);
    if (_eventStart) CloseHandle(_eventStart);
    if (_eventCompletion) CloseHandle(_eventCompletion);
    if (_bInitialized) DeleteCriticalSection(&_csection);
#else
    //printf("Yogeesh ********- IOThread is killed **************\n");
    pthread_join(_hIOThread,NULL);
#endif
#ifndef __linux__
    _eventQuit = NULL;
    _eventCancel = NULL;
    _eventStart = NULL;
    _eventCompletion = NULL;
#else
    _eventQuit = NULL;
    _eventCancel = NULL;
    _eventStart = NULL;
    _eventRecord = NULL;
    _eventCompletion = NULL;
#endif
}

/** Return a pointer to a Prosody voice device or NULL if the driver
    is unable to allocate a free voice device
 */
VoiceDevice *ProsodyDriver::AllocVoiceDevice()
{
    //printf("inside ProsodyDriver::AllocVoiceDevice()\n");
    //CLog::Detail(THISMODULE,"inside ProsodyDriver::AllocVoiceDevice()");
    ProsodyDevice *dev = NULL;
    if (!_bInitialized) return NULL;

    if (_distMethod == PROS_DISTRIBUTION_LOWEST) _nextModule = 0;

    for (int m=0;m<_modules.size();m++) {
	int mod = _nextModule;
	if (_modules[mod]->SupportVoice()) {
		//JAVI : If you wanna hack what module to use, this is the place
	    dev = _modules[mod]->AllocProsodyDevice();
	}

	// select next module
	_nextModule=(_nextModule+1)%_modules.size();

	if (dev) {
	    _devices[dev->_channel] = dev;
	    return dev;
	}
    }

    //***CLog::Detail(THISMODULE,"Unable to allocate new channel, no resource available");
    return NULL;
}


//-------------------------------------------------------------------
void ProsodyDriver::FreeVoiceDevice(VoiceDevice *device)
{
    ProsodyDevice *pdev = (ProsodyDevice*)device;
    _timed_devices.remove(pdev);
    //delete pdev;
    if ( pdev) 
    	pdev->DecRefCount();
}


/** AllocConferenceRoom: allocate resources for conference
 */
ConferenceRoom* ProsodyDriver::AllocConferenceRoom(int room_size) {
    ListVoiceDevice device_list;
    VoiceDevice *dev = NULL;
    ProsodyConferenceRoom* conf_room = NULL;
    tSMConference tSMConf;
	INIT_ACU_SM_STRUCT(&tSMConf);

    if (!_bInitialized) return NULL;

    if (_distMethod == PROS_DISTRIBUTION_LOWEST) _nextModule = 0;

    for (int m=0;m<_modules.size();m++) {
	int mod = _nextModule;
	if (_modules[mod]->SupportConference()) {
	    for (int d=0;d<room_size;d++) {
		dev = _modules[mod]->AllocProsodyChannel(kSMChannelTypeHalfDuplex, 0);
		if (!dev) {
		    break;
		}
		device_list.push_back(dev);
	    }
	    tSMConf = sm_conference_create();
	    if (tSMConf && device_list.size() == room_size) {
		conf_room = new ProsodyConferenceRoom(tSMConf);
		while (!device_list.empty()) {
		    dev = device_list.front();
		    device_list.pop_front();
		    conf_room->InsertDevice(dev);
		}
		return conf_room;
	    }
	    else {
		while (!device_list.empty()) {
		    dev = device_list.front();
		    device_list.pop_front();
		    FreeVoiceDevice(dev);
		}
	    }
	}

	// select next module
	_nextModule=(_nextModule+1)%_modules.size();
    }

    //***CLog::Detail(THISMODULE,"Unable to allocate new channel, no resource available");
    return NULL;
}

/** AllocEchoCanceller: allocate resources for echocanceller
 */
EchoCanceller* ProsodyDriver::AllocEchoCanceller() {
    EchoCanceller* ecr = NULL;
    if (!_bInitialized) return NULL;

    if (_distMethod == PROS_DISTRIBUTION_LOWEST) _nextModule = 0;

    for (int m=0;m<_modules.size();m++) {
	int mod = _nextModule;
	if (_modules[mod]->SupportEchoCanceller()) {
	    ecr = _modules[mod]->AllocProsodyEchoCanceller();
	}

	// select next module
	_nextModule=(_nextModule+1)%_modules.size();

	if (ecr) {
	    return ecr;
	}
    }

    //***CLog::Detail(THISMODULE,"Unable to allocate new channel, no resource available");
    return NULL;

}


/** ProcessRecogEvents: Called when a channel recognize a dtmf and convert it
    to the ASCII character
 */
void ProsodyDriver::ProcessRecogEvents()
{
	//printf ("inside ProsodyDriver::ProcessRecogEvents()\n");
    SM_RECOGNISED_PARMS recog_parms;
    INIT_ACU_SM_STRUCT(&recog_parms);

    // loop until there is no available recognition event or error
    for (;;) {
	recog_parms.channel = kSMNullChannelId;
	int rc = CHKFUNC(sm_get_recognised(&recog_parms),"get_recognised");
	if (rc) {
	    // ignore ERR_SM_NO_SUCH_CHANNEL
//	    if (rc != ERR_SM_NO_SUCH_CHANNEL)
//	    CLog::Error(THISMODULE,"Unable to get recognition, err=%s",ErrMsg(rc));
		//printf ("Unable to get recognition\n");
            usleep(50000); // T 20092016 - 50 millisec
	    break;
	}

	if (recog_parms.type == kSMRecognisedNothing) {
#ifdef PROSODY_DEBUG
	    //CLog::Detail(THISMODULE,"recognition_event -> nothing");
#endif
		//printf ("recognition_event -> nothing\n");
	    break;
	}

	ProsodyDevice *dev;
	dev = _devices[recog_parms.channel];

#ifdef PROSODY_DEBUG
	//CLog::Detail(THISMODULE,"recognition_event -> channel=%d (%s),type=%d",recog_parms.channel,DeviceName(dev),recog_parms.type);
#endif

	if (!dev) {
	    CLog::Fatal(THISMODULE,"No device found for channel %d while recognizing event",recog_parms.channel);
		//printf ("No device found for channel %d while recognizing event\n");
	    break;
	}

	switch (recog_parms.type) {
	case kSMRecognisedDigit:
#ifdef PROSODY_DEBUG
	    CLog::Detail(THISMODULE,"DTMF '%c' received on %s",recog_parms.param0,DeviceName(dev));
#endif
	    dev->_nbDetectedDtmf++;
	    (dev->*(dev->_fHandler))(ProsodyDevice::EV_DTMF,(char)recog_parms.param0);
	    break;

	case kSMRecognisedGruntStart:
	case kSMRecognisedGruntEnd:
#ifdef PROSODY_DEBUG
	    CLog::Detail(THISMODULE,"Grunt '%s' received on %s",(recog_parms.type == kSMRecognisedGruntStart) ? "on" : "off",DeviceName(dev));
#endif

	    dev->_silence = (recog_parms.type == kSMRecognisedGruntStart) ? false : true;

	    // check if the appl is waiting to be woken
	    // up when silence is detected
	    if (dev->_state == ProsodyDevice::ST_WAITSIL) {
		(dev->*(dev->_fHandler))(
		    (recog_parms.type == kSMRecognisedGruntStart) ? 
			ProsodyDevice::EV_SILOFF : 
			ProsodyDevice::EV_SILON,
		    0);
	    }

	    break;

	default:
	    CLog::Warning(THISMODULE,"Unknown recognition %d detected on %s",recog_parms.type,DeviceName(dev));
	}
    }
}

/** Lock: Protect access to shared data between main thread and I/O thread
 */
void ProsodyDriver::Lock()
{
#ifdef __linux__
    pthread_mutex_lock(&_csection);
#else
    EnterCriticalSection(&_csection);
#endif
}

/** Unlock: Protect access to shared data between main thread and I/O thread
 */
void ProsodyDriver::Unlock()
{
#ifdef __linux__
    pthread_mutex_unlock(&_csection);
#else
    LeaveCriticalSection(&_csection);
#endif
}

/** ProcessCancelJob: Cancel active play-file on some channels
 */
void ProsodyDriver::ProcessCancelJob()
{
    Instance()->Lock();
	//printf ("Inside ProcessCancelJob()\n");
    for (ListProsody::iterator it=Instance()->_cancelled.begin(); it!=Instance()->_cancelled.end(); it++) {
	ProsodyDevice *dev = *it;

	switch (dev->_state) {
	case ProsodyDevice::ST_PLAYFILE:
	case ProsodyDevice::ST_CANCEL_PLAYFILE:
	    {
		//printf("Yogeesh: Called sm_replay_abort in ProcessCancelJob \n");
		// abort playing a file
		SM_REPLAY_ABORT_PARMS abort_parms;
		INIT_ACU_SM_STRUCT(&abort_parms);
		
		abort_parms.channel = dev->_channel;
		abort_parms.nowait = 1;
		int rc = CHKFUNC(sm_replay_abort(&abort_parms),"replay_abort");
		if (rc) CLog::Error(THISMODULE,"Unable to abort file replay on %s, err=%s",dev->GetName(),ErrMsg(rc));
		else dev->SetState(ProsodyDevice::ST_CANCEL_PLAYFILE);	//** jnt **
	    }
	    break;

	case ProsodyDevice::ST_RECORDFILE:
        case ProsodyDevice::ST_CANCEL_RECORDFILE:
	    {
		// abort recording a file
		SM_RECORD_ABORT_PARMS abort_parms;
		INIT_ACU_SM_STRUCT(&abort_parms);
		
		abort_parms.channel = dev->_channel;

		int rc = CHKFUNC(sm_record_abort(&abort_parms),"record_abort");
		if (rc) {
                    //CLog::Error(THISMODULE,"Unable to abort record file on %s, err=%s",dev->GetName(),ErrMsg(rc)); // T REC
                } else {
                    dev->SetState(ProsodyDevice::ST_CANCEL_RECORDFILE);
                }
	    }
	    break;
	}
    }
    Instance()->_cancelled.clear();
    Instance()->Unlock();
}

/** CancelAsyncJob: It instructs the I/O thread to cancel the current job
 */
void ProsodyDriver::CancelAsyncJob(ProsodyDevice *device)
{
#if 1
    bool cancelling = false;
    Instance()->Lock();

    // first find if this device has previously cancelled the same job
    // or that the job has been completed
    ListProsody::iterator cancel_begin = Instance()->_cancelled.begin();
    ListProsody::iterator cancel_end = Instance()->_cancelled.end();
    ListProsody::iterator complete_begin = Instance()->_completed.begin();
    ListProsody::iterator complete_end = Instance()->_completed.end();
    if (std::find(cancel_begin,cancel_end,device) == cancel_end && 
	std::find(complete_begin,complete_end,device) == complete_end) {

	// add it to the cancelled device and inform the worker thread
        Instance()->_cancelled.push_back(device);
	cancelling = true;
    } else {
	/* CLog::Detail(THISMODULE, "CancelJob receive for a completed job on %s ignored",device->GetName()); */
    }
    Instance()->Unlock();
#ifndef __linux__
    if (cancelling) SetEvent(Instance()->_eventCancel);
#else
    if(cancelling)  //T 04072016 Puvan
    {               
          SetEvent(Instance()->_eventCancel);

//        ProsodyDriver::Instance()->_event_flags = 0;  //T 08
//        ProsodyDriver::Instance()->_event_flags |= _cancelEvent;  //T 08
//        pthread_cond_signal(&eventCond); //T 04072016 Puvan -- Commented
    }
#endif
#endif
}

/** ProcessWriteEvents: Called when there is a channel which needs to be
    filled up
 */
bool ProsodyDriver::ProcessWriteEvents()
{
    bool any_event = false;
    bool any_completed = false;

    //static unsigned char check_flag = 0;


//    any_completed = true;
#if 0
	check_flag++;
	if(check_flag == 5){
		any_completed = true;
		check_flag = 0;
	}
#endif
		
    // first check playfile
    for (;;) {
	   int loopflag = 0;

       int err = smd_ev_wait(*_writeParms->_parms.event);

       if(err)break;

   #if 1
   	   SM_REPLAY_STATUS_PARMS statusm_parms;
   	   INIT_ACU_SM_STRUCT(&statusm_parms);

   	   statusm_parms.channel = kSMNullChannelId;
   #endif

        int rc = CHKFUNC(sm_replay_status(&statusm_parms),"replay_status");
 
	    if (rc) {
	       // ignore ERR_SM_NO_SUCH_CHANNEL
	       if (rc != ERR_SM_NO_SUCH_CHANNEL)
           {
		      CLog::Error(THISMODULE,"Unable to get replay status");
           }
           PLOG_INFO("*********************************** Unable to get replay status %d ***************", statusm_parms.channel);
       
           usleep(2000);
   	       break;
   	    }

        PLOG_INFO("-----------------> got replay status channel --> %d",statusm_parms.channel);
	    // no more channel has finished playing file
	    if (statusm_parms.channel == kSMNullChannelId) break;

	    any_event = true;

	    ProsodyDevice *dev;
	    Instance()->Lock();
	    dev = Instance()->_devices[statusm_parms.channel];
	    Instance()->Unlock();

	    if (!dev) {
	        CLog::Fatal(THISMODULE,"No device found for channel %d while handling write event",statusm_parms.channel);
            PLOG_INFO("No device found for channel %d while handling write event");
	        break;
	    }
	
#ifdef PROSODY_DEBUG
	CLog::Detail(THISMODULE,"replay_status -> %s, status=%d", DeviceName(dev), statusm_parms.status);
#endif

	    switch (dev->_state) {
	    /** jnt **
	        case ProsodyDevice::ST_PLAYFILE:
	        case ProsodyDevice::ST_CANCEL_PLAYFILE:
	        break;
	     **/

	    case ProsodyDevice::ST_PLAYFILE:
	        break;

	    //default:
	    case ProsodyDevice::ST_CANCEL_PLAYFILE:
	        if (statusm_parms.status == kSMReplayStatusComplete) {

		        // disable write data event from this channel
#ifdef PROSODY_DEBUG
		        CLog::Detail(THISMODULE,"disabling write data event from %s",dev->GetName());
#endif
		        SM_CHANNEL_SET_EVENT_PARMS write_event_parms;
		        INIT_ACU_SM_STRUCT(&write_event_parms);

		        write_event_parms.channel = dev->_channel;
		        write_event_parms.event_type = kSMEventTypeWriteData;
		        write_event_parms.issue_events = kSMChannelNoEvent;
		        sm_channel_set_event(&write_event_parms);

		        // notify the device that the job has been completed
		        Instance()->Lock();
		        Instance()->_completed.push_back(dev);

		        // remove from cancelled if any
		        Instance()->_cancelled.remove(dev);
		        Instance()->Unlock();

		        any_completed = true;
	        }
	        continue;

	    default: // commented by Yogeesh
	        CLog::Fatal(THISMODULE,"Unexpected replay_status event on %s in state %s",DeviceName(dev),dev->StrState(dev->_state));
	        //JAVI : we don't have sm_reset_channel in V6
		    /*rc = CHKFUNC(sm_reset_channel(dev->_channel),"reset_channel");
	        if (rc) {
		        CLog::Error(THISMODULE,"Unable to reset channel %d, err=%s",dev->_channel,ErrMsg(rc));
	        }*/
	        continue;
	    }

	    if (dev->FillUpReplayBuffer(statusm_parms.status)) {
	        // disable write data event from this channel
#ifdef PROSODY_DEBUG
	        CLog::Detail(THISMODULE,"disabling write data event from %s",dev->GetName());
#endif
	        SM_CHANNEL_SET_EVENT_PARMS write_event_parms;
	        INIT_ACU_SM_STRUCT(&write_event_parms);

	        write_event_parms.channel = dev->_channel;
	        write_event_parms.event_type = kSMEventTypeWriteData;
    	    write_event_parms.issue_events = kSMChannelNoEvent;
    	    sm_channel_set_event(&write_event_parms);

#ifdef PROSODY_DEBUG
    	    CLog::Detail(THISMODULE,"updating the completion data of %s",dev->GetName());
#endif
    	    Instance()->Lock();

    	    // signal the end of play file to the main thread
    	    Instance()->_completed.push_back(dev);

#ifdef PROSODY_DEBUG
    	    /**
    	    ListProsody::iterator begin = Instance()->_cancelled.begin();
    	    ListProsody::iterator end = Instance()->_cancelled.end();
    	    if (std::find(begin,end,dev) != end) {
        		CLog::Detail(THISMODULE,"%s has finished PlayFile before cancelled, ignored",dev->GetName());
    	    }
	        **/
#endif

	        // remove from cancelled if any
    	    Instance()->_cancelled.remove(dev);

    	    Instance()->Unlock();
    	    any_completed = true;
    	    break; /* Narsim added */
    	}
    }

	//CLog::Detail(THISMODULE,"inside ProcessWriteEvents() inside AFTER for  loop 2");

    if (any_completed) {
#ifdef PROSODY_DEBUG
	CLog::Detail(THISMODULE,"informing main thread of the completion");
#endif
#ifndef __linux__
	    SetEvent(Instance()->_eventCompletion);
#else
	    /* Indication of play file completed */
        SetEvent(Instance()->_eventCompletion);

	    //ProsodyDriver::Instance()->_event_flags ^= _startEvent;  //T 08
	    //pthread_cond_signal(&completionCond); 
#endif
	}
//    PLOG_INFO("BREAKING Process Write Events\n");
    return any_event;
}

/** ProcessReadEvents: Called when there is a channel whose buffer needs to be
    emptied
 */
//bool ProsodyDriver::ProcessReadEvents() // T REC
DWORD ProsodyDriver::ProcessReadEvents(void *arg)
{
    bool any_event = false;
    bool any_completed = false;


    for (;;) {
	// check the job completion
	SM_RECORD_STATUS_PARMS status;
	INIT_ACU_SM_STRUCT(&status);

	status.channel = kSMNullChannelId;
	int rc = CHKFUNC(sm_record_status(&status),"sm_record_status");
	if (rc) {
	    if (rc != ERR_SM_NO_SUCH_CHANNEL) 
		CLog::Error(THISMODULE,"Unable to get record status, err=%s",ErrMsg(rc));
        usleep(300); // T REC
        continue; // T REC
	    //break;
	}

	// no more channel has got any buffer to be emptied
	if (status.channel == kSMNullChannelId) break;

	any_event = true;

	ProsodyDevice *dev;
	Instance()->Lock();
	dev = Instance()->_devices[status.channel];
	Instance()->Unlock();

	if (!dev) {
	    CLog::Fatal(THISMODULE,"No device found for channel %d while handling read event",status.channel);
	    break;
	}
	
#ifdef PROSODY_DEBUG
	CLog::Detail(THISMODULE,"record_data -> channel=%d (%s), status=%d",status.channel,DeviceName(dev),status.status);
#endif

	switch (status.status) {
	case kSMRecordStatusNoData:
	    break;

	// an overrun has occurred, we're not fast enough
	case kSMRecordStatusOverrun:
	    CLog::Warning(THISMODULE,"Buffer overrun on %s",DeviceName(dev));
	    // fall-through

	// get the data now
	case kSMRecordStatusCompleteData:
	case kSMRecordStatusData:
	    {
		// check if there's any data available
		SM_TS_DATA_PARMS data_parms;
		INIT_ACU_SM_STRUCT(&data_parms);
		
		char data[kSMMaxRecordDataBufferSize];
		data_parms.channel = status.channel;
		data_parms.data = data;
		data_parms.length = sizeof(data);
		int rc = CHKFUNC(sm_get_recorded_data(&data_parms),"sm_get_recorded_data");
		if (rc) {
		    CLog::Error(THISMODULE,"Unable to get recorded data, err=%s",ErrMsg(rc));
		    dev->_lastTaskError = VErr_File;
		    dev->AbortRecordFile(); // T REC
		    Instance()->CancelAsyncJob(dev);
		    continue;
		}

		// write the buffer content to file
		int nwritten = write(dev->_recordFile, data, data_parms.length);
		if (nwritten != data_parms.length) {
		    // error while writing to file
		    CLog::Error(THISMODULE,"Unable to write recorded data to file, %s",strerror(errno));
		    dev->_lastTaskError = VErr_File;
		    dev->AbortRecordFile(); // T REC
		    Instance()->CancelAsyncJob(dev);
		    continue;
		}
		dev->_recorded_size += nwritten;

	    }
	    break;

	case kSMRecordStatusComplete:
	    {
		//NOTE: AbortRecordFile() should be called only when an error occurs, not
                //when one of the termination conditions we specifiy in sm_record_start happens( such as "max_elapsed_time" or "max_silence" )
   
                //** jfg **
                //dev->AbortRecordFile();
                //****	
#ifdef PROSODY_DEBUG
		CLog::Detail(THISMODULE,"RecordFile completed on %s",dev->GetName());
#endif

		// disable read data event from this channel
#ifdef PROSODY_DEBUG
	        CLog::Detail(THISMODULE,"disabling read data event from %s",dev->GetName());
#endif
		SM_CHANNEL_SET_EVENT_PARMS event_parms;
		INIT_ACU_SM_STRUCT(&event_parms);

		event_parms.channel = status.channel;
		event_parms.event_type = kSMEventTypeReadData;
		event_parms.issue_events = kSMChannelNoEvent;
		sm_channel_set_event(&event_parms);
		
#ifdef PROSODY_DEBUG
	        CLog::Detail(THISMODULE,"updating the completion data of %s",dev->GetName());
#endif
		Instance()->Lock();
		
		// signal the end of play file to the main thread
		Instance()->_completed.push_back(dev);
		
#ifdef PROSODY_DEBUG
		/***
		ListProsody::iterator begin = Instance()->_cancelled.begin();
		ListProsody::iterator end = Instance()->_cancelled.end();
		if (std::find(begin,end,dev) != end) {
		    CLog::Detail(THISMODULE,"%s has finished RecordFile before cancelled, ignored",dev->GetName());
		}
		***/
#endif
		
		// remove from cancelled if any
		Instance()->_cancelled.remove(dev);
		
		Instance()->Unlock();
		
		any_completed = true;
	    }
	    break;
	}
        if (any_completed == true) { // T REC
            ProsodyDriver::Instance()->SetEvent(ProsodyDriver::Instance()->_eventCancel);
            break;
         }
    }

    if (any_completed) {
#ifdef PROSODY_DEBUG
	CLog::Detail(THISMODULE,"informing main thread of the completion");
#endif
	    /* Identify equivalent linux call */
	    /* SetEvent(Instance()->_eventCompletion); */
	     //SetEvent(Instance()->_eventCompletion); 
	     Instance()->SetEvent(Instance()->_eventCompletion); // T REC
	}

    return any_event;
}

/** IOThread: worker thread for refilling play buffer
 */
#ifndef __linux__
DWORD WINAPI ProsodyDriver::IOThread(void *arg)
#else
DWORD ProsodyDriver::IOThread(void *arg)
#endif
{
    pthread_t tid; // T REC
    PLOG("\n IOThread Started \n");
    /* CLog::Detail(THISMODULE,"Prosody I/O thread started"); */
    
    //SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_ABOVE_NORMAL);
    
    bool finished = false;
    int idle_count = 0;
/* thread affinity: Yogeesh added */
#if 0
    cpu_set_t pro_set;

    CPU_ZERO(&pro_set); /* Initialize it all to 0, i.e. no CPUs selected. */
    CPU_SET(5, &pro_set); /* set the bit that represents core 5. */

    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &pro_set);
#endif
    //printf("Yogeesh **** - pthread id =%d\n",pthread_self());

    //Yogeesh commented
    while (!finished) {
	
	bool any_event = false;
    static int cnt = 0;

#ifndef __linux__
	// wait for write data event or quit notification
	HANDLE events[] = { 
	    Instance()->_writeParms.event, 
		Instance()->_eventStart,
		Instance()->_eventCancel,
		Instance()->_eventQuit,
		Instance()->_readParms.event, 
	};
	DWORD rc = 
	    WaitForMultipleObjects(sizeof(events)/sizeof(events[0]),events,FALSE,60000);

	//printf("****** IPPBX : pthread id =%d\n",pthread_self());
	// no event, continue waiting
	if (rc == WAIT_TIMEOUT)
	    continue;
	
	switch (rc) {
	    // a channel needs to be filled up
	case WAIT_OBJECT_0:
	case WAIT_OBJECT_0+1:
	    any_event = ProcessWriteEvents();
	    break;
	    
	    // a job is cancelled
	case WAIT_OBJECT_0+2:
	    ProcessCancelJob();
	    any_event = true;
	    continue;
	    
	    // xGate is quitting
	case WAIT_OBJECT_0+3:
	    finished = true;
	    any_event = true;
	    continue;
	    
	    // a record data available
	case WAIT_OBJECT_0+4:
	    any_event = ProcessReadEvents();
	    break;
	}
#else  /* Linux Platform specific scenario */
#ifdef COND_WAIT
    HANDLE_T events[] = {
        Instance()->_writeParms,
        Instance()->_eventStart,
        Instance()->_eventCancel,
        Instance()->_eventQuit,
        Instance()->_eventRecord, // T REC
        Instance()->_readParms
    };

    PLOG("Wait for Multiple Events rc Called \n");
    PLOG_INFO("Wait for Multiple Events rc Called \n");

    int index = -1;
    int rc = Instance()->WaitForMultipleEvents(events, sizeof(events)/sizeof(events[0]), false, 100, index);
    PLOG_INFO("Wait for Multiple Events rc = %d, WAIT_TIMEOUT = %d\n", rc, WAIT_TIMEOUT);
	if (rc == WAIT_TIMEOUT)
    {
        any_event = Instance()->ProcessWriteEvents();
	    continue;
    }

    PLOG_INFO("Wait for Multiple Events rc = %d\n", index);
    //PLOG_INFO("GOT SIGNAL rc = %d\n", rc);

    switch (index) {
        // a channel needs to be filled up
    case WAIT_OBJECT_0:
    case WAIT_OBJECT_0+1:
        PLOG_INFO("\n IN case WAIT_OBJECT_0 : Calling ProcessWriteEvents\n");
        any_event = Instance()->ProcessWriteEvents();
        break;

        // a job is cancelled
    case WAIT_OBJECT_0+2:
        PLOG_INFO("\n IN case WAIT_OBJECT_0+2 : Calling ProcessCancelJob\n");
        CLog::Detail(THISMODULE,"\n IN case WAIT_OBJECT_0+2 : Calling ProcessCancelJob\n");
        Instance()->ProcessCancelJob();
        any_event = true;
        continue;

        // xGate is quitting
    case WAIT_OBJECT_0+3:
        PLOG_INFO("\n IN case WAIT_OBJECT_0+3 : QUIT EVENT\n");
//        finished = true;
        any_event = true;
        continue;

        // a record data available
    case WAIT_OBJECT_0+4:
        PLOG_INFO("\n IN case WAIT_OBJECT_0+3 : ProcessReadEvents\n");
        CLog::Detail(THISMODULE,"\n IN case WAIT_OBJECT_0+4 : ProcessReadEvents\n");
        //any_event = Instance()->ProcessReadEvents(); // T REC
        pthread_create(&tid,NULL,&(Instance()->ProcessReadEvents),NULL);
        any_event = true;
        break;

    case WAIT_OBJECT_0+5:
        CLog::Detail(THISMODULE,"\n IN case WAIT_OBJECT_0+5 :_eventRecord  ProcessReadEvents\n");
        pthread_create(&tid,NULL,&(Instance()->ProcessReadEvents),NULL);
        any_event = true;
        ProsodyDriver::Instance()->SetEvent(ProsodyDriver::Instance()->_eventRecord);
        break;

    default :
        PLOG_INFO("\n DEFAULT \n");
        break;
    }


#endif

	/* Above logic needs fine tuning in Linux based on test results */
	//pthread_cond_wait(&eventCond,&_voxMutex);
#if 0
        if (ProsodyDriver::Instance()->_event_flags & _startEvent) {
            any_event = ProcessWriteEvents();
            any_event = true;
	    ProsodyDriver::Instance()->_event_flags ^= _startEvent;
	    continue;
	}
        else if (ProsodyDriver::Instance()->_event_flags & _cancelEvent) {
	    //printf("********************************************ProcessCancelJob is called \n");
            ProcessCancelJob();
            any_event = true;
	    ProsodyDriver::Instance()->_event_flags ^= _cancelEvent;
	    ProsodyDriver::Instance()->_event_flags |= _startEvent;
            continue;
	}
        else if (ProsodyDriver::Instance()->_event_flags & _quitEvent) {
            // xGate is quitting
            finished = true;
            any_event = true;
	    ProsodyDriver::Instance()->_event_flags ^= _quitEvent;
            continue;
	} 
        else if (ProsodyDriver::Instance()->_event_flags & _readParams) {
            // a record data available
            //any_event = ProcessReadEvents();
	    any_event = true;
	   ProsodyDriver::Instance()->_event_flags ^= _readParams; // commented by IPPBX
	    continue;
        }
#endif
#endif

	/* Yogeesh added Sleep */
	//Sleep(100);	
	if (!any_event) {
	    if (++idle_count == 500) {
		 Sleep(2); 
	 	idle_count = 0;
	    }
	} else {
	    idle_count = 0;
	}
    }
    
    /* CLog::Detail(THISMODULE,"Prosody I/O thread stopped"); */
    
    //ExitThread(0);
    
    // never reached
    return 0;
}

/** CheckPlayDtmfCompletion: Poll channels which are currently playing dtmf
 */
void ProsodyDriver::CheckPlayDtmfCompletion()
{
    if (_activePlayDtmf == 0) return;
    
    for (int i=0;i<16;i++) {
	SM_PLAY_DIGITS_STATUS_PARMS status;
	INIT_ACU_SM_STRUCT(&status);
	
	status.channel = kSMNullChannelId;
	int rc = sm_play_digits_status(&status);
	
	if (rc) {
	    if (rc != ERR_SM_NO_SUCH_CHANNEL)
		CLog::Error(THISMODULE,"Unable to get write status, err=%s",ErrMsg(rc));
	    break;
	}
	
	// no more channel has finished playing file
	if ((status.channel == kSMNullChannelId) || 
	    (status.status != kSMPlayDigitsStatusComplete)) {
	    break;
	}
	
	ProsodyDevice *dev;
	dev = _devices[status.channel];
	if (!dev) {
	    CLog::Fatal(THISMODULE,"No device found for channel %d while playing digits",status.channel);
	    continue;
	}
	
	switch (dev->_state) {
	case ProsodyDevice::ST_PLAYDTMF:
	    if (status.status == kSMPlayDigitsStatusComplete) {
#ifndef __linux__
		(dev->*(dev->_fHandler))(ProsodyDevice::EV_ENDDTMF);
#else
		(dev->*(dev->_fHandler))(ProsodyDevice::EV_ENDDTMF,0);
#endif
		_activePlayDtmf--;
		
		//*** added *** (jnt)
		// disable write data event from this channel
		SM_CHANNEL_SET_EVENT_PARMS write_event_parms;
		INIT_ACU_SM_STRUCT(&write_event_parms);
		
		write_event_parms.channel = dev->_channel;
		write_event_parms.event_type = kSMEventTypeWriteData;
		write_event_parms.issue_events = kSMChannelNoEvent;
		CHKFUNC(sm_channel_set_event(&write_event_parms),"channel_set_event");
		//*** end *** (jnt)
	    }
	    break;
	    
	default:
	    CLog::Fatal(THISMODULE,"Unexpected play_digits_status event on %s in state %s",DeviceName(dev),dev->StrState(dev->_state));
	    break;
	}
    }
}

/** CheckPlayToneCompletion: Poll channels which are currently playing tone
 */
void ProsodyDriver::CheckPlayToneCompletion()
{
    if (_activePlayTone == 0) return;
    
    for (int i=0;i<16;i++) {
	SM_PLAY_TONE_STATUS_PARMS status;
	INIT_ACU_SM_STRUCT(&status);
	
	status.channel = kSMNullChannelId;
	int rc = sm_play_tone_status(&status);
	
	if (rc) {
	    if (rc != ERR_SM_NO_SUCH_CHANNEL)
		CLog::Error(THISMODULE,"Unable to get write status, err=%s",ErrMsg(rc));
	    break;
	}
	
	// no more channel has finished playing file
	if ((status.channel == kSMNullChannelId) ||
	    (status.status != kSMPlayToneStatusComplete)) {
	    break;
	}
	
	ProsodyDevice *dev;
	dev = _devices[status.channel];
	
	if (!dev) {
	    CLog::Fatal(THISMODULE,"No device found for channel %d while playing tone",status.channel);
	    continue;
	}
	
	switch (dev->_state) {
	case ProsodyDevice::ST_PLAYTONE:
	    if (status.status == kSMPlayToneStatusComplete) {
#ifndef __linux__
		(dev->*(dev->_fHandler))(ProsodyDevice::EV_ENDTONE);
#else
		(dev->*(dev->_fHandler))(ProsodyDevice::EV_ENDTONE,0);
#endif
		_activePlayTone--;
		
		//*** added *** (jnt)
		// disable write data event from this channel
		SM_CHANNEL_SET_EVENT_PARMS write_event_parms;
		INIT_ACU_SM_STRUCT(&write_event_parms);
		
		write_event_parms.channel = dev->_channel;
		write_event_parms.event_type = kSMEventTypeWriteData;
		write_event_parms.issue_events = kSMChannelNoEvent;
		CHKFUNC(sm_channel_set_event(&write_event_parms),"channel_set_event");
		//*** end *** (jnt)
	    }
	    break;
	    
	default:
	    CLog::Fatal(THISMODULE,"Unexpected play_tone_status event on %s in state %s",DeviceName(dev),dev->StrState(dev->_state));
	    break;
	}
    }
}

// poll DTMF recognition and play/record file completion events
bool ProsodyDriver::CheckRecognitionEvent()
{
#ifndef __linux__
    if (WaitForSingleObject(_recogParms.event,0) == WAIT_OBJECT_0) { 
	ProcessRecogEvents();
	return true;
    } 
#else
//    if(WaitForEvent(_recogParms,100) == WAIT_OBJECT_0) // T 20092016
//    {
       ProcessRecogEvents();
       return true;
//    }
#endif

    return false;
}

// poll DTMF recognition and play/record file completion events
bool ProsodyDriver::CheckCompletionEvent()
{
#ifndef __linux__
    if (WaitForSingleObject(_eventCompletion,0) == WAIT_OBJECT_0) 
#else
    if(WaitForEvent(_eventCompletion,0) == WAIT_OBJECT_0)
#endif
{
#ifdef PROSODY_DEBUG
	CLog::Detail(THISMODULE,"some device has completed job, checking");
#endif

	/* Wait on Completion conditional signal */
//	pthread_cond_wait(&completionCond,&completionMutex);	 //CBM
//	Lock(); //CBM
     
        ////if ( event_flags & _cancelEvent ) {
	while (!_completed.empty()) {
	     ProsodyDevice *dev = _completed.front();

		if (! dev->_BkGnd)
		{
			//printf ("dev->_BkGnd == FALSE\n");
	    		//pthread_cond_signal(&completionCond); 
			//pthread_cond_wait(&completionCond,&completionMutex);     //CBM
			//Lock(); //CBM
		}
		else
                {
			//printf ("dev->_BkGnd == TRUE\n");
                }
	     	_completed.remove(dev); 
	    
#ifdef PROSODY_DEBUG
	    CLog::Detail(THISMODULE,"%s has completed the job",dev->GetName());
#endif
	    switch (dev->_state) {
	    case ProsodyDevice::ST_PLAYFILE:
	    case ProsodyDevice::ST_CANCEL_PLAYFILE:

		dev->EndPlayFile();
		dev->DecRefCount();
		DecBkgndJobCounter();
		
		break;
		
	    case ProsodyDevice::ST_RECORDFILE:
            case ProsodyDevice::ST_CANCEL_RECORDFILE:


		dev->EndRecordFile();
		dev->DecRefCount();
		DecBkgndJobCounter();
		break;
		
	    default:
		//CLog::Warning(THISMODULE,"%s received more than once completion event in state %s",DeviceName(dev),dev->StrState(dev->_state));
		continue;
	    }
		/*******
		if (! dev->_BkGnd)
		{
			Unlock(); //CBM
		}
		*******/
	}
   ////}
	
//	Unlock(); //CBM
	return true;
    }
    return false;
}

/** HandleEvents: It is called regularly by Service Provider Manager
 */
void ProsodyDriver::HandleEvents()
{
    time_t now ;

    if (!_bInitialized || !GetAllocCounter()) return;

	//printf ("inside ProsodyDriver::HandleEvents() ..................\n");
    // do not poll too often
    if (++_poll_counter % 4)return;

    CheckPlayDtmfCompletion();

    CheckPlayToneCompletion();

    /*************
     old method replaced with individual WaitForSingleObject for each type of event
     *************
    HANDLE events[] = { _recogParms.event, _eventCompletion };
    time_t now = time(NULL);
    switch (WaitForMultipleObjects(sizeof(events)/sizeof(events[0]),events,FALSE,0)) {
    case WAIT_TIMEOUT:
	// fire timer if necessary
	if (now-_tmlasttimer >= 1) {
	    ListProsody::iterator itdev;
	    for (itdev=_timed_devices.begin();itdev!=_timed_devices.end();) {
		ProsodyDevice *dev = *itdev;
		itdev++;
		(dev->*(dev->_fHandler))(ProsodyDevice::EV_TIMER,now);
	    }
	    _tmlasttimer = now;
	}
	break;

    case WAIT_OBJECT_0:
	// a dtmf is recognized
	ProcessRecogEvents();
	break;

    case WAIT_OBJECT_0+1:
	// a PlayFile has completed
	{
	    Lock();
	    while (!_completed.empty()) {
		ProsodyDevice *dev = _completed.front();
		_completed.remove(dev);
#ifdef PROSODY_DEBUG
		CLog::Detail(THISMODULE,"%s has completed the job",dev->GetName());
#endif
		switch (dev->_state) {
		case ProsodyDevice::ST_PLAYFILE:
		case ProsodyDevice::ST_CANCEL_PLAYFILE:
		    dev->EndPlayFile();
		    dev->DecRefCount();
		    break;

		default:
		    CLog::Warning(THISMODULE,"%s received more than once completion event in state %s",DeviceName(dev),dev->StrState(dev->_state));
		    continue;
		}
	    }
	    Unlock();
	}
	break;

    default:
	CLog::Error(THISMODULE,"Unexpected event has occurent");
	break;
    }
    ************/


    // Check if we detect any DTMF (checked if at least one device is allocated)
    // or if any ongoing PlayFile has been completed (checked if at least there is one
    // active PlayFile)
    CheckRecognitionEvent();
    if (GetBkgndJobCounter()) CheckCompletionEvent();
    
    now = time(NULL);
    if (now-_tmlasttimer >= 1) {
	ListProsody::iterator itdev;
	for (itdev=_timed_devices.begin();itdev!=_timed_devices.end();) {
	    ProsodyDevice *dev = *itdev;
	    itdev++;
	    (dev->*(dev->_fHandler))(ProsodyDevice::EV_TIMER,now);
	}
	_tmlasttimer = now;
    }
}

//-------------------------------------------------------------------
// ProsodyDevice:
//-------------------------------------------------------------------

//-------------------------------------------------------------------
ProsodyDevice::ProsodyDevice(char *devname, const char* serial_num, tSMChannelId channel, int swdrv, int stream, int tslot, ProsodyDriver *driver, ProsodyModule *module) :
    VoiceDevice(devname,new AculabSwitch(devname, serial_num, swdrv, stream, tslot, true, module->GetModule()))
{
    _channel = channel;
    _driver = driver;
    _module = module;
    SetState(ST_IDLE);
    _asyncJob = NULL;
    _refCount = 0;
    _recordFile = -1;
    _recorded_size = 0;
    _wav_mode = false;
    _tmalloc = time(NULL);
    _module->StartUsage();
    _silence = false;

    memset(dtmf_digits,0,3);
    dtmf_index = 0; 

    SetOutputVolume(module->_volume);
    SetOutputAGC(module->_agc_play);
    SetInputVolume(module->_volume);
    SetInputAGC(module->_agc_play);
    
    IncRefCount();
    
    _driver->IncAllocCounter();
    
    InitVars();
    
    AculabSwitch* sw = (AculabSwitch*)GetSwitchDevice();
    int	localStream = ((AculabAddress*)sw->GetAddress(SF_Aculab))->GetStream();
    int	localTimeslot = ((AculabAddress*)sw->GetAddress(SF_Aculab))->GetTimeslot();
    
    SM_CHANNEL_INFO_PARMS cip;
    INIT_ACU_SM_STRUCT( &cip );
    
    // fetch channel type for later use
    int type = sm_get_channel_type(channel);
    
    // find out current position
    cip.channel = channel;
    
    int rc = sm_channel_info(&cip);
    if( rc ) 
    {
	CLog::Error(THISMODULE,"Error retrieving channel information, err=%s" ,ErrMsg(rc));
	return;
    }
    
    // Version 1 would have assigned local timeslots so
    // we check the timeslot details returned
    //
    // the channel needs an output timeslot unless
    // it can only be used for input
    if( type != kSMChannelTypeInput && cip.ost == -1) 
    {
	SM_SWITCH_CHANNEL_PARMS swp;
	INIT_ACU_SM_STRUCT( &swp );
	
	cip.ost = localStream;
	cip.ots = localTimeslot;
	swp.channel = channel;
	swp.st = cip.ost;
	swp.ts = cip.ots;
	swp.type = _module->GetCompanding();
	
	rc = sm_switch_channel_output(&swp);
	
	if (rc) 
	{
	    CLog::Error(THISMODULE,"sw_switch_channel_output failed, err=%s" ,ErrMsg(rc));
	    return;
	} else {
	    CLog::Detail(THISMODULE,"%s output switched to %d, %d", devname, localStream, localTimeslot);
	}
    }
    // the channel needs an input timeslot unless
    // it can only be used for output
    if( type != kSMChannelTypeOutput && cip.ist == -1 ) 
    {
	SM_SWITCH_CHANNEL_PARMS swp;
	INIT_ACU_SM_STRUCT( &swp );
	
	cip.ist = localStream;
	cip.its = localTimeslot;
	
	swp.channel = channel;
	swp.st = cip.ist;
	swp.ts = cip.its;
	swp.type = _module->GetCompanding();
	
	rc = sm_switch_channel_input(&swp);
	
	if( rc ) 
	{
	    CLog::Error(THISMODULE,"sw_switch_channel_input failed, err=%s" ,ErrMsg(rc));
	    return;
	} else {
	    /* CLog::Detail(THISMODULE,"%s input switched to %d, %d", devname, localStream, localTimeslot); */
	}
    }
    
}


//-------------------------------------------------------------------
ProsodyDevice::~ProsodyDevice()
{
    int rc;

    CLog::Detail(THISMODULE,">>>>>>>>>>>>>>> [ DESTRUCTOR Prosody Device ] Channel %02x Release \n", _channel);
    //printf(">>>>>>>>>>>>>>> [ DESTRUCTOR Prosody Device ] Channel %02x Release \n", _channel);

    _driver->_devices.erase(_channel);
    _driver->DecAllocCounter();

    rc = CHKFUNC(sm_channel_release(_channel),"channel_release");
    if (rc) CLog::Fatal(THISMODULE,"Unable to release %s, err=%s",GetName(),ErrMsg(rc));

    /* CLog::Detail(THISMODULE,"%s freed",GetName()); */
    _module->StopUsage(time(NULL)-_tmalloc);
}

//-------------------------------------------------------------------
int ProsodyDevice::IncRefCount()
{
#ifdef PROSODY_DEBUG
    CLog::Detail(THISMODULE,"%s incremented to %d",GetName(),_refCount+1);
#endif
    //Added to avoid memory leak, when call from SWB to operator and user A disconnects
    if(_refCount < 2)   
    {
        return ++_refCount;
    }
    else
    {
        return _refCount;
    }

//    return ++_refCount;
}

//-------------------------------------------------------------------
int ProsodyDevice::DecRefCount()
{
#ifdef PROSODY_DEBUG
    CLog::Detail(THISMODULE,"%s decremented to %d",GetName(),_refCount-1);
#endif
    int rc = --_refCount;
    if (rc == 0) delete this;
    return rc;
}

//-------------------------------------------------------------------
void ProsodyDevice::InitVars()
{
    SetState(ST_IDLE);
    _lasttermdigit = '\0';
    _taskbuff = NULL;
    _termdigs[0] = '\0';
    _bInterruptible = true;
    _format = VFmt_ADPCM_32K;
    _maxrectime = 30;	//*****
    _maxidtime = 4;	//*****
    _maxtime = 5;	//*****
	_maxsilence = 0;
    _maxDigToWait = 1;
    _digbuf.ClrDigit();
    _nbDetectedDtmf = 0;
}

//-------------------------------------------------------------------
bool ProsodyDevice::CancelJob()
{
    if (_state != ST_IDLE) {
#ifdef PROSODY_DEBUG
	CLog::Detail(THISMODULE,"Cancelling current job on %s while in state %s",GetName(),StrState(_state));
#endif
#ifndef __linux__
	(this->*_fHandler)(EV_STOP);
#else
	(this->*_fHandler)(EV_STOP,0);
#endif
    }
    return true;
}


//-------------------------------------------------------------------
const char *ProsodyDevice::StrState(ProsodyState state)
{
    static struct {
	ProsodyState state;
	char *name;
    } xlat[] = {
#define ST_NAMES(x)	{ x, #x }
	ST_NAMES(ST_IDLE),
	ST_NAMES(ST_PLAYFILE),
	ST_NAMES(ST_CANCEL_PLAYFILE),
	ST_NAMES(ST_PLAYTONE),
	ST_NAMES(ST_PLAYDTMF),
	ST_NAMES(ST_RECORDFILE),
        ST_NAMES(ST_CANCEL_RECORDFILE),
	ST_NAMES(ST_GETDIG),
	ST_NAMES(ST_WAITSIL),
#undef ST_NAMES
    };

    for (int i=0;i<sizeof(xlat)/sizeof(xlat[0]);i++) {
	if (state == xlat[i].state) return xlat[i].name+3;
    }
    return "*UNKNOWN*";
}


//-------------------------------------------------------------------
void ProsodyDevice::SetState(ProsodyState state)
{
    static struct {
	ProsodyState state;
	bool (ProsodyDevice::*func)(ProsodyEvent,unsigned);
    } handlerByState[] = {
	{ ST_IDLE,		&ProsodyDevice::EH_Idle },
	{ ST_PLAYFILE,		&ProsodyDevice::EH_PlayFile },
	{ ST_CANCEL_PLAYFILE,	&ProsodyDevice::EH_PlayFile },
	{ ST_PLAYTONE,		&ProsodyDevice::EH_PlayTone },
	{ ST_PLAYDTMF,		&ProsodyDevice::EH_PlayDtmf },
	{ ST_RECORDFILE,	&ProsodyDevice::EH_Record },
        { ST_CANCEL_RECORDFILE,	&ProsodyDevice::EH_Record },
	{ ST_GETDIG,		&ProsodyDevice::EH_GetDig },
	{ ST_WAITSIL,		&ProsodyDevice::EH_WaitForSilence },
    };

    for (int idx=0;idx<sizeof(handlerByState)/sizeof(handlerByState[0]);idx++) {
	if (handlerByState[idx].state == state) {

	    _state = state;
	    _fHandler = handlerByState[idx].func;

	    //**CLog::Detail(THISMODULE,"Set %s to state %s",GetName(),StrState(state));

	    if (state == ST_IDLE && _driver != NULL) 
		_driver->_timed_devices.remove(this);

	    return;
	}
    }

    // It should not happen
    CLog::Fatal(THISMODULE,"Changed to undefined state %d on %s",state,GetName());
    _state = state;
}


//-------------------------------------------------------------------
bool ProsodyDevice::IsDeviceIdle()
{
    return (_state == ST_IDLE);
}


//-------------------------------------------------------------------
void ProsodyDevice::Free()
{
    ProsodyDriver::Instance()->FreeVoiceDevice(this);
}


//-------------------------------------------------------------------
// Telephony action: get/retrieve a voice parameter
STATUS ProsodyDevice::SetVoiceFormat(VoiceFormat format)
{
    _format = format;
    return OK;
}


//-------------------------------------------------------------------
// Telephony action: get/retrieve a voice parameter
STATUS ProsodyDevice::GetVoiceFormat(VoiceFormat *format)
{
    *format = _format;
    return OK;
}


//-------------------------------------------------------------------
// Telephony action: get/retrieve a voice parameter
STATUS ProsodyDevice::EnableInterrupt()
{
    _bInterruptible = true;
    return OK;
}


//-------------------------------------------------------------------
// Telephony action: get/retrieve a voice parameter
STATUS ProsodyDevice::DisableInterrupt()
{
    _bInterruptible = false;
    return OK;
}


//-------------------------------------------------------------------
// Telephony action: get/retrieve a voice parameter
STATUS ProsodyDevice::SetMaxRecTime(int maxrectime)
{
    _maxrectime = maxrectime;
    return OK;
}


//-------------------------------------------------------------------
// Telephony action: get/retrieve a voice parameter
STATUS ProsodyDevice::GetMaxRecTime(int *maxrectime)
{
    *maxrectime = _maxrectime;
    return OK;
}


//-------------------------------------------------------------------
// Telephony action: get/retrieve a voice parameter
STATUS ProsodyDevice::SetInterDigitTimeout(int maxidtime)
{
    _maxidtime = maxidtime;
    return OK;
}


//-------------------------------------------------------------------
// Telephony action: get/retrieve a voice parameter
STATUS ProsodyDevice::GetInterDigitTimeout(int *maxidtime)
{
    *maxidtime = _maxidtime;
    return OK;
}


//-------------------------------------------------------------------
// Telephony action: get/retrieve a voice parameter
STATUS ProsodyDevice::SetDigitTimeout(int maxtime)
{
    _maxtime = maxtime;
    return OK;
}


//-------------------------------------------------------------------
// Telephony action: get/retrieve a voice parameter
STATUS ProsodyDevice::GetDigitTimeout(int *maxtime)
{
    *maxtime = _maxtime;
    return OK;
}


//-------------------------------------------------------------------
// Telephony action: get/retrieve a voice parameter
STATUS ProsodyDevice::SetTermDigits(char *termdigs)
{
    safe_strcpy(_termdigs,termdigs,sizeof(_termdigs));
    return OK;
}


//-------------------------------------------------------------------
// Telephony action: get/retrieve a voice parameter
STATUS ProsodyDevice::GetTermDigits(char *termdigs)
{
    safe_strcpy(termdigs,_termdigs,sizeof(_termdigs));
    return OK;
}


//-------------------------------------------------------------------
// Telephony action: get/retrieve a voice parameter
STATUS ProsodyDevice::SetRecordMaxSilence(int maxsil)
{// maxsil is in second
    _maxsilence=maxsil;
    return OK;
}


//-------------------------------------------------------------------
// Telephony action: get/retrieve a voice parameter
STATUS ProsodyDevice::GetRecordMaxSilence(int *maxsil)
{// maxsil is in second
    *maxsil=_maxsilence;
    return OK;
}

//-------------------------------------------------------------------
// Handler function
bool ProsodyDevice::EH_Idle(ProsodyEvent event,unsigned info)
{
    switch (event) {
    case EV_DTMF:
	_digbuf.AddDigit((char)info);
	break;
    default:
	return false;
    }
    return true; 
}


//-------------------------------------------------------------------
// Handler function
bool ProsodyDevice::EH_WaitForSilence(ProsodyEvent event,unsigned info)
{
    /* CLog::Debug(THISMODULE,"WaitForSilence(%s): event=%d info=%d", GetName(), event, info);
     */

    switch (event) {
    case EV_DTMF:
	_digbuf.AddDigit((char)info);
	break;

    case EV_SILOFF:
	// normally should not happen
	// ignore
	break;

    case EV_SILON:
    case EV_STOP:
	// wake up task
	VoiceManager::Instance()->ResumeTask(this);
	SetState(ST_IDLE);
	break;

    case EV_TIMER:
	// decrement timer (if not 0) until it reaches 0
	if (*_silence_timer) {
	    (*_silence_timer)--;
	    if (*_silence_timer == 0) {
		// wake up task
		VoiceManager::Instance()->ResumeTask(this);
		SetState(ST_IDLE);
	    }
	}
	break;

    default:
	return false;
    }
    return true; 
}


//-------------------------------------------------------------------
// Handler function
bool ProsodyDevice::EH_PlayFile(ProsodyEvent event,unsigned info)
{
    char dtmf;
    time_t now;

    switch (event) {
    case EV_DTMF:
        ProsodyDriver::Instance()->CancelAsyncJob(this);
        SetState(ST_CANCEL_PLAYFILE);
        ProsodyDriver::Instance()->ProcessCancelJob();  //T 04072016 Puvan -- Commented

        dtmf = (char)info;
        CLog::Detail(THISMODULE,"ProsodyDevice::this address =%02x,maxDgtWait=%d",this,_maxDigToWait);
        CLog::Detail(THISMODULE,"ProsodyDevice::EH_PlayFile DTMF Index =%d,dtmf digit=%c",dtmf_index,dtmf);
        if ( dtmf_index < 3 && dtmf_index <= _maxDigToWait)  {
                dtmf_digits[dtmf_index] = dtmf;
                dtmf_index++;
                CLog::Detail(THISMODULE,"ProsodyDevice::EH_PlayFile : dtmf index=%d,dtmf digit=%d",dtmf_index,dtmf_digits[dtmf_index-1]);
                if ( dtmf_index == _maxDigToWait) {
                        VoiceManager::Instance()->ResumeTask(this);
                        SetState(ST_IDLE);
                        break;
                }
        }
        else {
            dtmf_index = 0;
            CLog::Detail(THISMODULE,"ProsodyDevice::EH_PlayFile : else case of dtmf digit index 0");
        }

        break;

	_digbuf.AddDigit((char)info);
	if (!_bInterruptible) break;
	// continue with aborting replay_file

     case EV_STOP:
	// tell the I/O thread to cancel the current job
	// the task will be woken by the event handle
	if (_state == ST_PLAYFILE) {
            ProsodyDriver::Instance()->_event_flags = 0;  //T 08
            ProsodyDriver::Instance()->_event_flags |= _cancelEvent;  //T 08
	    ProsodyDriver::Instance()->CancelAsyncJob(this);
	    //** jnt **SetState(ST_CANCEL_PLAYFILE);
	} else {
	    /* CLog::Detail(THISMODULE,"PlayFile on %s has been cancelled more than once",GetName());
	     */
	}
	break;

    default:
	return false;
    }

    return true; 
}

//-------------------------------------------------------------------
bool ProsodyDevice::AbortPlayFile()
{
    int ret = 0;
    // abort playing a file
    SM_REPLAY_ABORT_PARMS replay_abort_parms;
	INIT_ACU_SM_STRUCT(&replay_abort_parms);

    replay_abort_parms.channel = _channel;
    replay_abort_parms.nowait = 1; // T 27062016
    int rc = CHKFUNC(sm_replay_abort(&replay_abort_parms),"replay_abort");
    if (rc) CLog::Error(THISMODULE,"Unable to abort file replay on %s, err=%s",GetName(),ErrMsg(rc));

    return rc == 0;
}
//-------------------------------------------------------------------
bool ProsodyDevice::AbortRecordFile()
{
    // abort recording a file
    SM_RECORD_ABORT_PARMS abort_parms;
    INIT_ACU_SM_STRUCT(&abort_parms);
    
    abort_parms.channel = _channel;

    pthread_mutex_lock(&lock_t);  //KANNAN
    int rc = CHKFUNC(sm_record_abort(&abort_parms),"record_abort");
    pthread_mutex_unlock(&lock_t);//KANNAN

    if (rc) CLog::Error(THISMODULE,"Unable to abort record file on %s, err=%s",GetName(),ErrMsg(rc));

    return rc == 0;
}

/** Fill up the buffer of a Prosody channel from file.
    Returns true if the job is done
 */
bool ProsodyDevice::FillUpReplayBuffer(int status)
{
    int rc;
    FileReplayParam *prm;
    char buffer[kSMMaxReplayDataBufferSize];

    SM_TS_DATA_PARMS dataprm;
    INIT_ACU_SM_STRUCT(&dataprm);

    int toread;

    switch (status) {

	case kSMReplayStatusUnderrun:
	    CLog::Warning(THISMODULE,"Buffer underrun on %s",GetName());
	    // continue to the next statement

	case kSMReplayStatusHasCapacity:
	    prm = *_asyncJob->current;
	    toread = min(kSMMaxReplayDataBufferSize,prm->replay_parms.data_length-prm->offset);

#ifdef PROSODY_DEBUG
	    CLog::Detail(THISMODULE,"Filling up replay  buffer on %s (%u%%)",GetName(),(prm->offset+toread)*100/prm->replay_parms.data_length);
#endif
	    // read from file
	    if ((lseek(prm->fh,prm->offset,SEEK_SET) == -1) ||
		(read(prm->fh,buffer,toread) != toread)) {
		CLog::Error(THISMODULE,"File error on %s: %s, errno=%d",GetName(),strerror(errno),errno);
		_lastTaskError = VErr_File;
		AbortPlayFile();
		return true;
	    }

	    prm->offset += toread;

	    dataprm.channel = _channel;
	    dataprm.data = buffer;
	    dataprm.length = toread;

	    // fill up prosody buffer
	rc = (prm->offset == (long)prm->replay_parms.data_length) ?
	    CHKFUNC(sm_put_last_replay_data(&dataprm),"put_last_replay_data") :
	    CHKFUNC(sm_put_replay_data(&dataprm),"put_replay_data");
	    if (rc) {
		CLog::Error(THISMODULE,"Unable to fill prosody buffer on %s, err=%s",GetName(),ErrMsg(rc));
		_lastTaskError = VErr_Hardware;
		AbortPlayFile();
		return true;
	    }
	    break;

	case kSMReplayStatusCompleteData:
	break;

	case kSMReplayStatusComplete:
	    if (!_asyncJob->_repeat_flag) {
		_asyncJob->current++;
		if (_asyncJob->current == _asyncJob->lrp.end()) {
#ifdef PROSODY_DEBUG
		CLog::Detail(THISMODULE,"PlayFile completed on %s",GetName());
#endif
		    return true;
		}
	    }

	    /* Yogeesh added */
	    //printf("Yogeesh: vox play file background sleep executed\n");
	    usleep(30);  /* for play background file -- T 06072016 Puvan */

	    // kick a new playfile
	    prm = *_asyncJob->current;
	    if (_asyncJob->_repeat_flag) {
#ifdef PROSODY_DEBUG
		CLog::Detail(THISMODULE,"PlayFile repeated on %s",GetName());
#endif
		prm->offset = 0;
	    }
	    rc = CHKFUNC(sm_replay_start(&prm->replay_parms),"replay_start");
	    if (rc) {
		CLog::Error(THISMODULE,"Unable to start play file on %s, err=%s",GetName(),ErrMsg(rc));
		_lastTaskError = VErr_Hardware;
		AbortPlayFile();
		return true;
	    }
	    break;

	case kSMReplayStatusNoCapacity:
	break;

	default:
	CLog::Error(THISMODULE,"Unsupported replay file status (%d) on %s", status, GetName());
	    AbortPlayFile();
	    return true;
	}

    return false;
}

//-------------------------------------------------------------------
// close all files and wake up task at completion of playfile 
void ProsodyDevice::EndPlayFile()
{
    if (_asyncJob) {
	ListReplayParam::iterator it;
	for (it=_asyncJob->lrp.begin();it!=_asyncJob->lrp.end();) {
	    FileReplayParam *replay_parms= *it++;
		//printf ("EndPlayFile() : close(replay_parms->fh\n");
	    close(replay_parms->fh);
	    delete replay_parms;
	    replay_parms = NULL;
	}

	// asynch. operation completed, resume task and set task lase error if suspended
	if (_asyncJob->_task_suspended) {
	    VoiceManager::Instance()->ResumeTask(this);
	    VoiceManager::Instance()->SetTaskLastError(this,_lastTaskError,0);
	}

	delete _asyncJob;
	_asyncJob = NULL;

	// go to idle state
	SetState(ST_IDLE);
    } else {
	// asynch. operation completed, resume task and go to idle state
	VoiceManager::Instance()->SetTaskLastError(this,_lastTaskError,0);
	VoiceManager::Instance()->ResumeTask(this);
	SetState(ST_IDLE);
    }
}

//-------------------------------------------------------------------
// close file and wake up task at completion of recordfile 
void ProsodyDevice::EndRecordFile()
{
    _wav_mode=false;
    if (_wav_mode) {
	lseek(_recordFile, (long)&(((WAV_Header*)0)->chunk_2_len), SEEK_SET);
	write(_recordFile, &_recorded_size, 4);

	_recorded_size += 36;
	lseek(_recordFile, (long)&(((WAV_Header*)0)->file_length), SEEK_SET);
	write(_recordFile, &_recorded_size, 4);
    }

    close(_recordFile);

    _recordFile = -1;
    _wav_mode = false;
    _recorded_size = 0;

    // asynch. operation completed, resume task and go to idle state
    VoiceManager::Instance()->SetTaskLastError(this,_lastTaskError,0);
    VoiceManager::Instance()->ResumeTask(this);
    SetState(ST_IDLE);
}
STATUS ProsodyDevice::EndRecordFile_t()
{
    CLog::Error(THISMODULE,"ProsodyDevice::EndRecordFile_t.. \n");
    ProsodyDriver::Instance()->CancelAsyncJob(this);
    SetState(ST_CANCEL_RECORDFILE);
    ProsodyDriver::Instance()->ProcessCancelJob();
    ProsodyDevice *dev = ProsodyDriver::Instance()->_completed.front();
    CLog::Detail(THISMODULE,"ProsodyDevice::EndRecordFile_t Ending the Record");
    ProsodyDriver::Instance()->_completed.clear();
    VoiceManager::Instance()->ResumeTask(this);
}

//-------------------------------------------------------------------
// Handler function
bool ProsodyDevice::EH_PlayTone(ProsodyEvent event,unsigned info)
{
    int rc;

    switch (event) {
    case EV_DTMF:
	_digbuf.AddDigit((char)info);
	break;

    case EV_ENDTONE:
	// asynch. operation completed, resume task and go to idle state
	VoiceManager::Instance()->ResumeTask(this);
	SetState(ST_IDLE);
#ifdef PROSODY_DEBUG
	CLog::Detail(THISMODULE,"End-of-play tone on %s",GetName());
#endif
	break;

    case EV_STOP:
	rc = CHKFUNC(sm_play_tone_abort(_channel),"play_tone_abort");
	if (rc) {
	    CLog::Error(THISMODULE,"Unable to abort playtone on %s",GetName());
	}
	break;

    default:
	return false;
    }
    return true; 
}


//-------------------------------------------------------------------
// Handler function
bool ProsodyDevice::EH_PlayDtmf(ProsodyEvent event,unsigned info)
{
    switch (event) {
    case EV_DTMF:
	_digbuf.AddDigit((char)info);
	break;

    case EV_ENDDTMF:
	// asynch. operation completed, resume task and go to idle state
#ifdef PROSODY_DEBUG
	CLog::Detail(THISMODULE,"End-of-play dtmf on %s",GetName());
#endif
	VoiceManager::Instance()->ResumeTask(this);
	SetState(ST_IDLE);
	break;

    case EV_STOP:
	// Can't be cancelled
	break;

    default:
	return false;
    }
    return true; 
}


//-------------------------------------------------------------------
// Handler function
bool ProsodyDevice::EH_Record(ProsodyEvent event,unsigned info)
{
    char dtmf;
    dtmf_index = 0;

    switch (event) {
    case EV_DTMF:
         //printf ("DTMF 7777 InsideEH_Record  ProsodyDriver::CancelAsyncJob()\n");
        ProsodyDriver::Instance()->CancelAsyncJob(this);
        //SetState(ST_CANCEL_PLAYFILE);
        SetState(ST_CANCEL_RECORDFILE);
        ProsodyDriver::Instance()->ProcessCancelJob();

        dtmf = (char)info;
        //printf ("7777 InsideEH_Record  dtmf digits = %c. dtmf_index = %d, _maxDigToWait =%d\n", dtmf, dtmf_index, _maxDigToWait);
        CLog::Detail(THISMODULE,"ProsodyDevice::this address =%02x,maxDgtWait=%d",this,_maxDigToWait);
        CLog::Detail(THISMODULE,"ProsodyDevice::EH_PlayFile DTMF Index =%d,dtmf digit=%c",dtmf_index,dtmf);
        if ( dtmf_index < 3 && dtmf_index <= _maxDigToWait)  {
                dtmf_digits[dtmf_index] = dtmf;
                dtmf_index++;
                CLog::Detail(THISMODULE,"ProsodyDevice::EH_RecordFile : dtmf index=%d,dtmf digit=%d",dtmf_index,dtmf_digits[dtmf_index-1]);
                //printf ("7777 InsideEH_Record  dtmf digits = %c", dtmf);
                ProsodyDevice *dev = ProsodyDriver::Instance()->_completed.front();
                if(dtmf == '#')
                {
                    //printf("\n Ending the Record \n");
                    //dev->EndRecordFile();
                    //ProsodyDriver::Instance()->_completed.clear();
                    // T REC 
                    ProsodyDriver::Instance()->_completed.clear();
                    VoiceManager::Instance()->ResumeTask(this);
                    SetState(ST_CANCEL_RECORDFILE);
                    break;
                }
                if ( dtmf_index == _maxDigToWait) {
                        VoiceManager::Instance()->ResumeTask(this);
                        SetState(ST_CANCEL_RECORDFILE);
                        //SetState(ST_IDLE);
                        break;
                }
        }
        else {
            dtmf_index = 0;
            CLog::Detail(THISMODULE,"ProsodyDevice::EH_PlayFile : else case of dtmf digit index 0");
        }

        break;

	_digbuf.AddDigit((char)info);
	if (!_bInterruptible) break;
	// continue with aborting record_file

    case EV_STOP:
        if (_state == ST_RECORDFILE) 
        {
	ProsodyDriver::Instance()->CancelAsyncJob(this);
	} 
        else 
        {
	    /* CLog::Detail(THISMODULE,"RecordFile on %s has been cancelled more than once",GetName());
	     */
        }
	break;

    default:
	return false;
    }
    return true; 
}


//-------------------------------------------------------------------
// Handler function
bool ProsodyDevice::EH_GetDig(ProsodyEvent event,unsigned info)
{
    char dtmf;
    time_t now;

    switch (event) {
    case EV_DTMF:
	dtmf = (char)info;

	CLog::Detail(THISMODULE,"ProsodyDevice::this address =%02x,maxDgtWait=%d",this,_maxDigToWait);
	CLog::Detail(THISMODULE,"ProsodyDevice::EH_GetDig DTMF Index =%d,dtmf digit=%c",dtmf_index,dtmf);
	if ( dtmf_index < 3 && dtmf_index <= _maxDigToWait)  {
                dtmf_digits[dtmf_index] = dtmf;
                dtmf_index++;
		CLog::Detail(THISMODULE,"ProsodyDevice::dtmf index=%d,dtmf digit=%d",dtmf_index,dtmf_digits[dtmf_index-1]);
		if ( dtmf_index == _maxDigToWait) {
                        VoiceManager::Instance()->ResumeTask(this);
                        SetState(ST_IDLE);
                        break;
                }
        }
        else {
            dtmf_index = 0;
	    CLog::Detail(THISMODULE,"ProsodyDevice::else case of dtmf digit index 0");
	}

	// do we have a terminator dtmf?
	if (strchr(_termdigs,dtmf)) {
	    _lasttermdigit = dtmf;

	    // copy to task's buffer
	    _digbuf.CopyDigits(_taskbuff,_digbuf.nbelt);
	    _taskbuff = NULL;

	    // asynch. operation completed, resume task and go to idle state
	    //***CLog::Detail(THISMODULE,"Got termination digit '%c'",dtmf);
	    VoiceManager::Instance()->ResumeTask(this);
	    SetState(ST_IDLE);
	    break;
	}

	// insert this digit to the internal buffer
	_digbuf.AddDigit(dtmf);

	CLog::Error(THISMODULE,"EH_GetDig : dtmf = %c",dtmf);

	// do we have enough digits?
	if (_digbuf.nbelt >= _maxDigToWait) {
	    // copy to task's buffer
	    _digbuf.CopyDigits(_taskbuff,_maxDigToWait);
	    _taskbuff = NULL;

	    // asynch. operation completed, resume task and go to idle state
	    //***CLog::Detail(THISMODULE,"Got enough (%d) digits",_maxDigToWait);
	    VoiceManager::Instance()->ResumeTask(this);
	    SetState(ST_IDLE);
	    break;
	}

	_tmlastdigit = time(NULL);
	break;

    case EV_TIMER:
	now = (time_t)info;

	// check time-out
	if (((_maxidtime < 0) && (now >= _tmlastdigit) && (now-_tmlastdigit > -_maxidtime)) ||
	    ((_maxidtime > 0) && (now-_tmlastdigit > _maxidtime)) ||
	    (now-_tmfirstcall > _maxtime)) {
	    _lasttermdigit = '\0';

	    // copy to task's buffer
	    _digbuf.CopyDigits(_taskbuff,_digbuf.nbelt);
	    _taskbuff = NULL;

	    // asynch. operation completed, resume task and go to idle state
	    //***CLog::Detail(THISMODULE,"Digit timeout");
	    VoiceManager::Instance()->ResumeTask(this);
	    SetState(ST_IDLE);
	    break;
	}
	break;

    case EV_STOP:
	VoiceManager::Instance()->ResumeTask(this);
	SetState(ST_IDLE);
	break;

    default:
	return false;
    }
    return true; 
}

//-------------------------------------------------------------------
bool ProsodyDevice::DevicePlayDtmf(const char *digits)
{
#ifdef PROSODY_DEBUG
    CLog::Detail(THISMODULE,"Playing dtmf %s on %s",digits,GetName());
#endif

    int rc;

    //*** added *** (jnt)
    //***WARNING: this chunk of code causes the CPU to go 100%!!!!
    //		  but without this, play dtmf does not work

    // attach writedata event to this channel
    SM_CHANNEL_SET_EVENT_PARMS write_event_parms;
    INIT_ACU_SM_STRUCT(&write_event_parms);

    write_event_parms.channel = _channel;
    write_event_parms.event_type = kSMEventTypeWriteData;
    write_event_parms.issue_events = kSMAnyChannelEvent;
    rc = CHKFUNC(sm_channel_set_event(&write_event_parms),"channel_set_event");
    if (rc) {
	CLog::Error(THISMODULE,"Unable to attach WriteData event on %s, err=%s",GetName(),ErrMsg(rc));
	VoiceManager::Instance()->SetTaskLastError(this,VErr_Hardware,rc);
	return false;
    }
    //*** end *** (jnt)

    SM_PLAY_DIGITS_PARMS play_digits_parms;
    INIT_ACU_SM_STRUCT(&play_digits_parms);

    play_digits_parms.channel = _channel;
    play_digits_parms.wait_for_completion = 0;
    play_digits_parms.digits.type = kSMDTMFDigits;
    strcpy(play_digits_parms.digits.digit_string,digits);

    rc = CHKFUNC(sm_play_digits(&play_digits_parms),"play_digits");
    if (rc) {
	CLog::Error(THISMODULE,"Unable to play digit on %s, err=%s",GetName(),ErrMsg(rc));
	VoiceManager::Instance()->SetTaskLastError(this,VErr_Hardware,rc);
	return false;
    }

    ProsodyDriver::Instance()->_activePlayDtmf++;

    return true;
}

//-------------------------------------------------------------------
bool ProsodyDevice::DevicePlayTone(int toneId,int duration)
{
#ifdef PROSODY_DEBUG
    CLog::Detail(THISMODULE,"Playing tone on %s",GetName());
#endif

    int rc;

    //*** added *** (jnt)
    // attach writedata event to this channel
    SM_CHANNEL_SET_EVENT_PARMS write_event_parms;
    INIT_ACU_SM_STRUCT(&write_event_parms);

    write_event_parms.channel = _channel;
    write_event_parms.event_type = kSMEventTypeWriteData;
    write_event_parms.issue_events = kSMAnyChannelEvent;
    rc = CHKFUNC(sm_channel_set_event(&write_event_parms),"channel_set_event");
    if (rc) {
	CLog::Error(THISMODULE,"Unable to attach WriteData event on %s, err=%s",GetName(),ErrMsg(rc));
	VoiceManager::Instance()->SetTaskLastError(this,VErr_Hardware,rc);
	return false;
    }
    //*** end *** (jnt)

    SM_PLAY_TONE_PARMS play_tone_parms;
    INIT_ACU_SM_STRUCT(&play_tone_parms);
    
    play_tone_parms.channel = _channel;
    play_tone_parms.duration = duration;
    play_tone_parms.wait_for_completion = 0;
    play_tone_parms.tone_id = toneId;

    rc = CHKFUNC(sm_play_tone(&play_tone_parms),"play_tone");
    if (rc) {
#ifndef __linux__
	CLog::Error(THISMODULE,"Unable to start play tone on %s, err=%s",GetName,ErrMsg(rc));
#endif
	VoiceManager::Instance()->SetTaskLastError(this,VErr_Hardware,rc);
	return false;
    }

    ProsodyDriver::Instance()->_activePlayTone++;

    return true;
}

//-------------------------------------------------------------------
// Check the header of a WAV file and return the encoding format in Prosody notation
int ProsodyDevice::get_wav_encoding_format(int file_handle)
{
    WAV_Header hdr;

    if (read(file_handle, &hdr, sizeof(hdr)) != sizeof(hdr))
	return 0;

    if (memcmp(hdr.riff, "RIFF", 4) || memcmp(hdr.wave, "WAVE", 4) || memcmp(hdr.fmt_, "fmt ", 4) || memcmp(hdr.data, "data", 4))
	return 0;

    // only support mono files
    if (hdr.channels != 1)
	return 0;
    // supported encodings
    if (hdr.encoding == WAVE_FORMAT_ALAW && hdr.sample_rate == 8000)
	return kSMDataFormat8KHzALawPCM;

    if (hdr.encoding == WAVE_FORMAT_MULAW && hdr.sample_rate == 8000)
	return kSMDataFormat8KHzULawPCM;

    if (hdr.encoding == WAVE_FORMAT_OKI_ADPCM && hdr.sample_rate == 8000)
	return kSMDataFormat8KHzOKIADPCM;

    if (hdr.encoding == WAVE_FORMAT_OKI_ADPCM && hdr.sample_rate == 6000)
	return kSMDataFormat6KHzOKIADPCM;

    if (hdr.encoding == 1 /*WAVE_FORMAT_PCM*/ && hdr.sample_rate == 8000)
	return kSMDataFormat8KHz16bitMono;

    if (hdr.encoding == 1 /*WAVE_FORMAT_PCM*/ && hdr.sample_rate == 8000)
	return kSMDataFormat8KHz8bitMono;

    // the rest of encoding format is unsupported
    return 0;
}

//-------------------------------------------------------------------
STATUS ProsodyDevice::PlayFileArray(int nbfiles,char const **filename,bool bkgnd,bool autorepeat)
{
    
    return PlayFileArrayWav(nbfiles,filename,bkgnd,autorepeat,false);
}

//-------------------------------------------------------------------
STATUS ProsodyDevice::PlayFileArrayWav(int nbfiles,char const **filename,bool bkgnd,bool autorepeat,bool wav_mode)
{
    int rc;

    // if the action is interruptible and there is 
    // at least one dtmf pending, return immediately
    if (_bInterruptible && _digbuf.nbelt) { 
//	return OK;  //PUVAN -- for #9 not played
    }

	    //printf ("file = %s, \n",*filename);
    /**
    To support background file:
    - add new instance variables:
      - repeat flag: 
          Set to true if the file must be repeated, false otherwise.
	  It is also set to false when VoxStopPlayFileBkgnd is called.
      - task suspended:
          Set to true is the calling task has been suspended so it can
	  be resumed when the operation is over.
	  Set to true in these cases:
	  - not background play
	  - when VoxStopPlayFileBkgnd() is called
    - In FillUpReplayBuffer():
      - If kSMReplayStatusComplete is received and repeat flag is
        true, start playing the same file again
      - Otherwise, proceed as normal
    - In EndPlayFile():
      - ResumeTask() if task has been suspended
    - In StopPlayFileBkGnd(waiteof):
      - SuspendTask()
      - set task suspended flag (=1)
      - reset repeat flag (=0)
      - if !waiteof call CancelAsyncJob(this);
    **/

/*
    if (bkgnd) {
	CLog::Warning(THISMODULE,"PlayFileBkGnd not supported at this version");
	VoiceManager::Instance()->SetTaskLastError(this,VErr_InvalidOperation,0);
	return OK;
    }
*/
    _asyncJob = new ProsodyAsyncJob;

    // open each files and fill-up the job parameters
    for (int i=0;i<nbfiles;i++) {
#ifndef __linux__
	int f = fopen(*filename,"rw");
#else
	int f = open(*filename,O_RDONLY);
	FILE *fp = fopen(*filename,"r");
#endif

	if (f == -1) {
	    VoiceManager::Instance()->SetTaskLastError(this,VErr_File,errno);
	    CLog::Warning(THISMODULE,"Unable to open file %s, %s (errno=%d)",*filename,errno ? strerror(errno) : "",errno);
	    filename++;
	    continue;
	}

	// check file format
	int wav_format = 0;
	if (wav_mode) {
	    wav_format = get_wav_encoding_format(f);
	    if (!wav_format) {
		close(f);
		CLog::Warning(THISMODULE,"Skipping file %s, unsupported WAV format",*filename);
		filename++;
		continue;
	    }
	} else {
	    if (file_length(fp) == 0) {
		close(f);
		/* CLog::Detail(THISMODULE,"Skipping file %s, size=0",*filename); 
*/

		filename++;
		continue;
	    }
	}

	FileReplayParam *replay_parms = new FileReplayParam;
	replay_parms->fh = f;
	replay_parms->offset = wav_mode ? sizeof(WAV_Header) : 0; // if WAV skip the header, go directly to the payload
	replay_parms->replay_parms.data_length = file_length(fp);
	replay_parms->replay_parms.channel = _channel;
	replay_parms->replay_parms.background = kSMNullChannelId;
	replay_parms->replay_parms.volume = _output_dB;
	replay_parms->replay_parms.agc = _output_agc;
	replay_parms->replay_parms.speed = 100;
	fclose(fp); /* narsim added */

	//printf("File length=%d\n",replay_parms->replay_parms.data_length);

	if (wav_mode) {
	    // use the format as specified in the WAV header
	    replay_parms->replay_parms.type = (kSMDataFormat)wav_format;
	} else {
	    switch (_format) {
	    case VFmt_ADPCM_24K:
		replay_parms->replay_parms.type = kSMDataFormat6KHzOKIADPCM;
		break;
	    case VFmt_ADPCM_32K:
		replay_parms->replay_parms.type = kSMDataFormat8KHzOKIADPCM;
		break;
	    case VFmt_PCM_ALAW_48K:
		replay_parms->replay_parms.type = kSMDataFormat6KHzPCM;
		break;
	    case VFmt_PCM_MULAW_48K:
		replay_parms->replay_parms.type = kSMDataFormat6KHzPCM;
		break;
	    case VFmt_PCM_ALAW_64K:
		replay_parms->replay_parms.type = kSMDataFormat8KHzALawPCM;
		break;
	    case VFmt_PCM_MULAW_64K:
		replay_parms->replay_parms.type = kSMDataFormat8KHzULawPCM;
		break;
	    }
	}
	_asyncJob->lrp.push_back(replay_parms);

	filename++;
    }

    // check if at least we have something to play
    if (_asyncJob->lrp.empty()) {
	//***CLog::Info(THISMODULE,"No file to play");
	delete _asyncJob;
	_asyncJob = NULL;
	return OK;
    }

    _asyncJob->dev = this;
    _asyncJob->current = _asyncJob->lrp.begin();

    _lastTaskError = VErr_NoError;

    // attach writedata event to this channel
    SM_CHANNEL_SET_EVENT_PARMS write_event_parms;
    INIT_ACU_SM_STRUCT(&write_event_parms);

    write_event_parms.channel = _channel;
    write_event_parms.event_type = kSMEventTypeWriteData;
    write_event_parms.issue_events = kSMAnyChannelEvent;
    rc = CHKFUNC(sm_channel_set_event(&write_event_parms),"channel_set_event");
    if (rc) {
	CLog::Error(THISMODULE,"Unable to attach WriteData event on %s, err=%s",GetName(),ErrMsg(rc));
	VoiceManager::Instance()->SetTaskLastError(this,VErr_Hardware,rc);
	return OK;
    }

    // increment reference count to avoid early destruction
    IncRefCount();
    SetState(ST_PLAYFILE);

    ////bkgnd = false;
    CLog::Error(THISMODULE,"narsim: bkgnd flag status=%d",bkgnd);
    if (bkgnd) {
	_asyncJob->_repeat_flag = autorepeat;
	_asyncJob->_task_suspended = false;
    } else {
	VoiceManager::Instance()->SuspendTask(this);
	_asyncJob->_repeat_flag = false;
	_asyncJob->_task_suspended = true;
    }


    // kick-off the playfile now
    FileReplayParam *prm = _asyncJob->lrp.front();
    rc = CHKFUNC(sm_replay_start(&prm->replay_parms),"replay_start");
    if (rc) {
	CLog::Error(THISMODULE,"Unable to start play file =%d\n",rc);
	VoiceManager::Instance()->SetTaskLastError(this,VErr_Hardware,rc);
	while (!_asyncJob->lrp.empty()) {
	    FileReplayParam* rp = _asyncJob->lrp.front();
	    close(rp->fh);
	    delete rp;
	    rp = NULL;
	    _asyncJob->lrp.pop_front();
	}
	delete _asyncJob;
	_asyncJob = NULL;

	SetState(ST_IDLE);
	VoiceManager::Instance()->ResumeTask(this);
	DecRefCount();

	return OK;
    }

    ProsodyDriver::Instance()->IncBkgndJobCounter();
#ifndef __linux__
    SetEvent(ProsodyDriver::Instance()->_eventStart);
#else
    /* pthread_cond_wait for _eventStart exist in IOThread */
    PLOG("SetEvent Called for EVENT Start %02x \n", ProsodyDriver::Instance()->_eventStart);
//    ProsodyDriver::Instance()->SetEvent(ProsodyDriver::Instance()->_recogParms); // T 20092016
    ProsodyDriver::Instance()->SetEvent(ProsodyDriver::Instance()->_eventStart);

    //ProsodyDriver::Instance()->_event_flags |= _startEvent;  //T 08
    //pthread_cond_signal(&eventCond);
    /* Avoid running in main thread. It stops everything */
    //ProsodyDriver::Instance()->ProsodyDriver::ProcessWriteEvents();
#endif

    /** The asynchronous job has been started and the I/O thread
	will take over the responsability to fill up Prosody channel
	buffer until completion or cancelled
     */
 
    return OK;
}

//-------------------------------------------------------------------
STATUS ProsodyDevice::StopPlayFileBkGnd(bool waiteof)
{
/*
    CLog::Warning(THISMODULE,"PlayFileBkGnd not supported at this version");
    VoiceManager::Instance()->SetTaskLastError(this,VErr_InvalidOperation,0);
*/
    if (_state != ST_PLAYFILE) {
	/* CLog::Debug(THISMODULE,"%s: StopPlayFileBkGnd called while no file is played", GetName());
	*/
	return OK;
    }

    if (!waiteof) {
	//printf ("StopPlayFileBkGnd() : !waiteof\n");
	SetState(ProsodyDevice::ST_CANCEL_PLAYFILE);
    	ProsodyDriver::Instance()->_event_flags |= _cancelEvent; // T 08
    	//pthread_cond_signal(&eventCond); // Yogeesh
	ProsodyDriver::Instance()->CancelAsyncJob(this);
//	EndPlayFile(); // Yogeesh T 04072016 Puvan -- Commented
//	DecRefCount(); // Yogeesh T 04072016 Puvan -- Commented
//	AbortPlayFile(); // T 04072016 Puvan -- Commented
//	ProsodyDriver::Instance()->ProcessCancelJob(); T 04072016 Puvan -- Commented
	return OK;
    }

    VoiceManager::Instance()->SuspendTask(this);
    if (_asyncJob) {
	_asyncJob->_repeat_flag = false;
	_asyncJob->_task_suspended = true;
    }
    	ProsodyDriver::Instance()->_event_flags |= _cancelEvent; 
    	//pthread_cond_signal(&eventCond); // Yogeesh
    return OK;
}

//-------------------------------------------------------------------
STATUS ProsodyDevice::PlayTone(int freq1,int db1,int freq2,int db2,int duration)
{
    int tone = 0;
    // if the action is interruptible and there is 
    // at least one dtmf pending, return immediately
    if (_bInterruptible && _digbuf.nbelt) {
	return OK;
    }

    if ((freq2 != 0) && (freq2 != 0)) {
	CLog::Warning(THISMODULE,"Dual-frequency is not supported in this release");
	VoiceManager::Instance()->SetTaskLastError(this,VErr_InvalidParameter,0);
	return OK;
    }

    int freq = freq1 ? freq1 : freq2;

    for (tone=0;freq && (tone<4);tone++) {
	if (_module->_tone[tone].freq == freq) break;
    }

    if (!freq) {
	CLog::Warning(THISMODULE,"Invalid frequency (0 Hz) for %s",GetName());
	VoiceManager::Instance()->SetTaskLastError(this,VErr_InvalidParameter,0);
	return OK;
    }

    if (tone == 4) {
	CLog::Warning(THISMODULE,"Freq %d must be defined first in startup parameter",freq);
	VoiceManager::Instance()->SetTaskLastError(this,VErr_InvalidParameter,0);
	return OK;
    }

    // if tone duration is less or equal to 50 msec, make it 50 msec
    if (duration < 50) duration = 50;

    // Pick a thread to do the job synchronously
    if (DevicePlayTone(_module->_tone[tone].id,duration)) {
	VoiceManager::Instance()->SuspendTask(this);
	SetState(ST_PLAYTONE);
    }

    return OK;
}

//-------------------------------------------------------------------
STATUS ProsodyDevice::PlayDtmf(char *dtmf)
{
    // nothing to play?
    if (strlen(dtmf) == 0) return OK;

    // if the action is interruptible and there is 
    // at least one dtmf pending, return immediately
    if (_bInterruptible && _digbuf.nbelt) {
	return OK;
    }

    // do the job asynchronously
    if (DevicePlayDtmf(dtmf)) {
	VoiceManager::Instance()->SuspendTask(this);
	SetState(ST_PLAYDTMF);
    }

    return OK;
}

//-------------------------------------------------------------------
STATUS ProsodyDevice::WaitForSilence(int max_wait_time, int* remaining_time)
{
    // in seconds
    _silence_timer = remaining_time;
    *_silence_timer = max_wait_time;

    if (_silence) return OK;

    VoiceManager::Instance()->SuspendTask(this);
    _driver->_timed_devices.push_back(this);
    SetState(ST_WAITSIL);

    return OK;
}

//-------------------------------------------------------------------
bool ProsodyDevice::create_wav_header(int file_handle, int encoding, int channels, int sample_rate, int bytes_second, int blk_align, int bits_sample)
{
    int value = 0;
    write(file_handle, "RIFF", 4);
    write(file_handle, &value, 4);
    write(file_handle, "WAVE", 4);
    write(file_handle, "fmt ", 4);
    value = 16;
    write(file_handle, &value, 4);
    write(file_handle, &encoding, 2);
    write(file_handle, &channels, 2);
    write(file_handle, &sample_rate, 4);
    write(file_handle, &bytes_second, 4);
    write(file_handle, &blk_align, 2);
    write(file_handle, &bits_sample, 2);
    write(file_handle, "data", 4);
    value = 0;
    write(file_handle, &value, 4);

    return true;
}


//-------------------------------------------------------------------
STATUS ProsodyDevice::RecordFile(char *filename)
{
    return RecordFileWav(filename,false);
}

//-------------------------------------------------------------------
STATUS ProsodyDevice::RecordFileWav(char *filename,bool wav_mode)
{
    // of the action is interruptible and there is 
    // at least one dtmf pending, return immediately
	//printf ("**********************IPPBX : inside ProsodyDevice::RecordFileWav()\n");
    /* T REC
    CLog::Warning(THISMODULE,"Recording is not supported for wave files");
    return OK;
    */

    if (_bInterruptible && _digbuf.nbelt) {
	return OK;
    }

    // open file for write
#ifndef __linux__
    int f = open(filename,O_WRONLY|_O_TRUNC|_O_CREAT |O_BINARY,_S_IREAD|_S_IWRITE);
#else
    int f = open(filename,O_WRONLY|O_CREAT|O_TRUNC);
#endif
    if (f == -1) {
	VoiceManager::Instance()->SetTaskLastError(this,VErr_File,errno);
	CLog::Warning(THISMODULE,"Unable to open file %s for recording, %s (errno=%d)",filename,errno ? strerror(errno) : "",errno);
	return OK;
    }

    // prepare recording parameters
    SM_RECORD_PARMS record_parms;
    INIT_ACU_SM_STRUCT(&record_parms);

    record_parms.channel = _channel;
    record_parms.alt_data_source = kSMNullChannelId;
    record_parms.agc = 0;
    record_parms.volume = 0; 

	//JAVI : More technical background needed to solve this
    /*if (_bInterruptible) 
    	record_parms.elimination = kSMDRecordToneElimination;*/
    //record_parms.max_elapsed_time = _maxrectime*1000;
    record_parms.max_elapsed_time = _maxrectime*4000000;
    record_parms.max_silence = _maxsilence*1000;
	// hardcoded noise*grunt detection tuning
	#ifdef PROSODY_TiNG
	//record_parms.min_noise_level=-55.0; //default -55dBm
	//record_parms.grunt_threshold=15.0; // default 15 dB
	#endif

    #ifdef PROSODY_TiNG
	record_parms.tone_elimination_mode=kSMToneDetectionMinDuration40;
    #endif
    // concert encoding format to Prosody and WAV notation
    int wav_encoding = WAVE_FORMAT_ALAW;
    int wav_sample_rate = 8000;
    int wav_bytes_second = 8000;
    int wav_bits_sample = 8;
    switch (_format) {
    case VFmt_ADPCM_24K:
	record_parms.type = kSMDataFormat6KHzOKIADPCM;
	wav_encoding = WAVE_FORMAT_OKI_ADPCM;
	wav_sample_rate = 6000;
	wav_bytes_second = 3000;
	wav_bits_sample = 4;
	break;
    case VFmt_ADPCM_32K:
	record_parms.type = kSMDataFormat8KHzOKIADPCM;
	wav_encoding = WAVE_FORMAT_OKI_ADPCM;
    // T REC
	//wav_bytes_second = 4000;
	//wav_bits_sample = 4;
	break;
    case VFmt_PCM_ALAW_64K:
	record_parms.type = kSMDataFormat8KHzALawPCM;
	wav_encoding = WAVE_FORMAT_ALAW;
	break;
    case VFmt_PCM_MULAW_64K:
	record_parms.type = kSMDataFormat8KHzULawPCM;
	wav_encoding = WAVE_FORMAT_MULAW;
	break;
    case VFmt_PCM_ALAW_48K:
	record_parms.type = kSMDataFormat6KHzPCM;
	wav_encoding = WAVE_FORMAT_ALAW;
	wav_bytes_second = wav_sample_rate = 6000;
	break;
    case VFmt_PCM_MULAW_48K:
	record_parms.type = kSMDataFormat6KHzPCM;
	wav_encoding = WAVE_FORMAT_MULAW;
	wav_bytes_second = wav_sample_rate = 6000;
	break;
    }

    // write the header if wav_mode
    int wav_blk_align = 1 * wav_bits_sample/8;	// channels x bits_sample / 8;
    if (wav_mode) {
	if (!create_wav_header(f, wav_encoding, 1, wav_sample_rate, wav_bytes_second, wav_blk_align, wav_bits_sample)) {
	    CLog::Error(THISMODULE,"Unable to write WAV header %s",filename);
	    VoiceManager::Instance()->SetTaskLastError(this,VErr_File,errno);
	    close(f);
	    return OK;
	}
    }

    // attach readdata event to this channel
    SM_CHANNEL_SET_EVENT_PARMS event_parms;
    INIT_ACU_SM_STRUCT(&event_parms);

    event_parms.channel = _channel;
    event_parms.event_type = kSMEventTypeReadData;
    event_parms.issue_events = kSMAnyChannelEvent;
    int rc = CHKFUNC(sm_channel_set_event(&event_parms),"channel_set_event");
    if (rc) {
	CLog::Error(THISMODULE,"Unable to attach ReadData event on %s, err=%s",GetName(),ErrMsg(rc));
	VoiceManager::Instance()->SetTaskLastError(this,VErr_Hardware,rc);
	close(f);
	return OK;
    }

    // increment reference count to avoid early destruction
    IncRefCount();

    // set state to recording and suspend task
    SetState(ST_RECORDFILE);
    //VoiceManager::Instance()->SuspendTask(this); // T REC
    _recordFile = f;
    _wav_mode = wav_mode;
    _recorded_size = 0;

    // start the recording now
    rc = sm_record_start(&record_parms);
    if (rc) {
	CLog::Error(THISMODULE,"Unable to start recording file on %s, err=%s",GetName(),ErrMsg(rc));
	VoiceManager::Instance()->SetTaskLastError(this,VErr_Hardware,rc);
	_recordFile = -1;
	close(f);
	//printf("Unable to start recording file on %s\n",GetName());

	SetState(ST_IDLE);
	VoiceManager::Instance()->ResumeTask(this);
	DecRefCount();

	return OK;
    }

    // increment reference count to avoid early destruction
    ProsodyDriver::Instance()->IncBkgndJobCounter();
    /* pthread_cond_wait for _eventStart exist in IOThread */
    /* Recording is disabled */
    //_event_flags |= _readParams;
    // T REC
    CLog::Detail(THISMODULE,"API call SetEvent _eventRecord \n");
    ProsodyDriver::Instance()->SetEvent(ProsodyDriver::Instance()->_eventRecord);

    // The rest of the job will be taken care by the IOThread
    return OK;
}

//-------------------------------------------------------------------
STATUS ProsodyDevice::GetDigits(int length,char *buffer)
{
    int i;

#if 1

    CLog::Detail(THISMODULE,"Get digit this=%02x,dtmf_index=%d,length=%d",this,dtmf_index,length);
    if ( dtmf_index > 0 && dtmf_index == length ) {
        memcpy(buffer,dtmf_digits,dtmf_index);
        buffer[dtmf_index]='\0';
        dtmf_index = 0;
        memset(dtmf_digits,0,3);
        return OK;
    }
#endif

    // otherwise start asynch. action
    time_t now = time(NULL);
    if (_maxidtime >= 0) _tmlastdigit = now;
    else _tmlastdigit = now + _maxtime + 1;
    _tmfirstcall = now;
    _taskbuff = buffer;
    _maxDigToWait = length;

    VoiceManager::Instance()->SuspendTask(this);
    _driver->_timed_devices.push_back(this);
    SetState(ST_GETDIG);

    return OK;
}

//-------------------------------------------------------------------
STATUS ProsodyDevice::GetLastTermDigit(char *buffer)
{
    buffer[0] = _lasttermdigit;
    buffer[1] = '\0';

    return OK;
}

//-------------------------------------------------------------------
STATUS ProsodyDevice::PeekDigit(char *buffer)
{
    if (_digbuf.nbelt) {
	buffer[0] = _digbuf.buffer[0];
	buffer[1] = '\0';
    } else {
	buffer[0] = '\0';
    }
    return OK;
}

//-------------------------------------------------------------------
STATUS ProsodyDevice::ClearDigits()
{
    _digbuf.ClrDigit();
    dtmf_index = 0; // T 1710
    return OK;
}


//-------------------------------------------------------------------
// ProsodyConferenceRoom implementation
//-------------------------------------------------------------------

//-------------------------------------------------------------------
ProsodyConferenceRoom::ProsodyConferenceRoom(tSMConference tSMConf)
: ConferenceRoom()
{
    _handle = (int) tSMConf;
}

ProsodyConferenceRoom::~ProsodyConferenceRoom()
{
    conf_device* conf_dev;
    while (!_conf_devices.empty()) {
	conf_dev = _conf_devices.front();
	assert(conf_dev);
	if (conf_dev->_conferenced)
	    this->RemoveParty(conf_dev->_device);
	conf_dev->_device->Free();
	_conf_devices.pop_front();
    	delete conf_dev;
	conf_dev = NULL;
    }
    sm_conference_delete((tSMConference) _handle);
}


STATUS ProsodyConferenceRoom::AddParty(VoiceDevice* dev)
{
    List<conf_device*>::iterator it;
    conf_device* conf_dev = NULL;

    if (!dev) {
	CLog::Info(THISMODULE,"Room %s: invalid (null) device ", _name.c_str());
	return NOT_OK;
    }

    for (it = _conf_devices.begin(); it != _conf_devices.end(); it++) {
	if ((*it)->_device == dev) {
	    conf_dev = *it;
	    if (conf_dev->_conferenced) {
		CLog::Info(THISMODULE,"Room %s: %s is already conferenced", _name.c_str(), dev->GetName());
		return NOT_OK;
	    }
	}
    }

    if (!conf_dev) {
	CLog::Error(THISMODULE,"Room %s: %s is not allocated by this room", _name.c_str(), dev->GetName());
	return NOT_OK;
    }

    SM_CONFERENCE_ADD_PARTY_PARMS sm_conference_add_party_parms;
	INIT_ACU_SM_STRUCT(&sm_conference_add_party_parms);

    sm_conference_add_party_parms.conf = (tSMConference) _handle;
    sm_conference_add_party_parms.channelIn = ((ProsodyDevice*) dev)->GetChannel();
    sm_conference_add_party_parms.channelOut = ((ProsodyDevice*) dev)->GetChannel();
    sm_conference_add_party_parms.out_volume = dev->_output_dB;
    sm_conference_add_party_parms.out_agc = dev->_output_agc;
    sm_conference_add_party_parms.in_volume = dev->_input_dB;
    sm_conference_add_party_parms.in_agc = dev->_input_agc;

    if (sm_conference_add_party(&sm_conference_add_party_parms) != 0)
    {
	CLog::Fatal(THISMODULE,"Room %s: could not add %s to conference!", _name.c_str(), dev->GetName());
	return NOT_OK;
    }

    /* CLog::Detail(THISMODULE, "Room %s: %s has been added to conference", _name.c_str(), dev->GetName());
     */
    conf_dev->_conferenced = true;

    return OK;
}

STATUS ProsodyConferenceRoom::RemoveParty(VoiceDevice* dev)
{
    List<conf_device*>::iterator it;
    conf_device* conf_dev = NULL;

    if (!dev) {
	CLog::Info(THISMODULE,"Room %s: invalid (null) device ", _name.c_str());
	return NOT_OK;
    }

    for (it = _conf_devices.begin(); it != _conf_devices.end(); it++) {
	if ((*it)->_device == dev) {
	    conf_dev = *it;
	    if (!conf_dev->_conferenced) {
		CLog::Info(THISMODULE,"Room %s: %s is not conferenced", _name.c_str(), dev->GetName());
		return NOT_OK;
	    }
	}
    }

    if (!conf_dev) {
	CLog::Error(THISMODULE,"Room %s: %s is not allocated by this room", _name.c_str(), dev->GetName());
	return NOT_OK;
    }

    SM_CONFERENCE_REMOVE_PARTY_PARMS sm_conference_remove_party_parms;
	INIT_ACU_SM_STRUCT(&sm_conference_remove_party_parms);

    sm_conference_remove_party_parms.conf = (tSMConference) _handle;
    sm_conference_remove_party_parms.channelIn = ((ProsodyDevice*) dev)->GetChannel();
    sm_conference_remove_party_parms.channelOut = ((ProsodyDevice*) dev)->GetChannel();

    if (sm_conference_remove_party(&sm_conference_remove_party_parms) != 0)
    {
	CLog::Fatal(THISMODULE,"Room %s: cannot remove %s from conference", _name.c_str(), dev->GetName());
	return NOT_OK;
    }

    /* CLog::Detail(THISMODULE, "Room %s: %s has been removed from conference", _name.c_str(), dev->GetName());
     */

    conf_dev->_conferenced = false;

    return OK;
}

//-------------------------------------------------------------------
// ProsodyEchoCanceller implementation
//-------------------------------------------------------------------

//-------------------------------------------------------------------

ProsodyEchoCanceller::ProsodyEchoCanceller(
    ProsodyDevice* ioChannel, ProsodyDevice* refChannel)
    : EchoCanceller()
{

    primary_channel = ioChannel;
    reference_channel = refChannel;
    
    cancelling = false;
}

ProsodyEchoCanceller::~ProsodyEchoCanceller() {
    if (cancelling) StopCancelling();
    if (primary_channel) {
	primary_channel->GetSwitchDevice()->DisconnectListeners();
	primary_channel->GetSwitchDevice()->Unlisten();
	primary_channel->DetachTask();
	primary_channel->Free();
	primary_channel = NULL;
    }
    if (reference_channel) {
	reference_channel->GetSwitchDevice()->DisconnectListeners();
	reference_channel->GetSwitchDevice()->Unlisten();
	reference_channel->DetachTask();
	reference_channel->Free();
	reference_channel = NULL;
    }
}

void ProsodyEchoCanceller::AttachTask(CTask* task) {
    EchoCanceller::AttachTask(task);
    primary_channel->AttachTask(task);
    reference_channel->AttachTask(task);
}

VoiceDevice* ProsodyEchoCanceller::GetPrimaryChannel() {
    return primary_channel;
}

VoiceDevice* ProsodyEchoCanceller::GetReferenceChannel() {
    return reference_channel;
}

void ProsodyEchoCanceller::StartCancelling() {
    SM_CONDITION_INPUT_PARMS sm_condition_input_parms;
	INIT_ACU_SM_STRUCT(&sm_condition_input_parms);

    sm_condition_input_parms.channel = primary_channel->GetChannel();
    sm_condition_input_parms.reference = reference_channel->GetChannel();
    sm_condition_input_parms.reference_type = kSMInputCondRefUseInput;
    sm_condition_input_parms.conditioning_type = kSMInputCondEchoCancelation;
    sm_condition_input_parms.conditioning_param = 0;
    sm_condition_input_parms.alt_data_dest = primary_channel->GetChannel();
    sm_condition_input_parms.alt_dest_type = kSMInputCondAltDestOutput;

    int rc = CHKFUNC(sm_condition_input(&sm_condition_input_parms),"sm_condition_input");
    if (rc) {
	CLog::Error(THISMODULE,"Unable to start cancelling err=%s",ErrMsg(rc));
	return;
    }

    cancelling = true;
}

void ProsodyEchoCanceller::ReinitCancelling() {
    int rc = CHKFUNC(sm_condition_reinit(primary_channel->GetChannel()),"sm_condition_reinit");
    if (rc) {
	CLog::Error(THISMODULE,"Unable to reinit cancelling err=%s",ErrMsg(rc));
    }

    cancelling = true;
}

void ProsodyEchoCanceller::StopCancelling() {
    SM_CONDITION_INPUT_PARMS sm_condition_input_parms;
	INIT_ACU_SM_STRUCT(&sm_condition_input_parms);

    if (cancelling) {
	sm_condition_input_parms.channel = primary_channel->GetChannel();
	sm_condition_input_parms.reference = 0;
	sm_condition_input_parms.reference_type = kSMInputCondRefNone;
	sm_condition_input_parms.conditioning_type = kSMInputCondNone;
	sm_condition_input_parms.conditioning_param = 0;
	sm_condition_input_parms.alt_data_dest = primary_channel->GetChannel();
	sm_condition_input_parms.alt_dest_type = kSMInputCondAltDestOutput;
	int rc = CHKFUNC(sm_condition_input(&sm_condition_input_parms),"sm_condition_input");
	if (rc) {
	    CLog::Error(THISMODULE,"Unable to stop cancelling err=%s",ErrMsg(rc));
	}

	cancelling = false;
    }
}


//-------------------------------------------------------------------
// ProsodyMixer implementation
//-------------------------------------------------------------------

//-------------------------------------------------------------------
class ProsodyMixer : public VoiceMixer
{
    SupportSwitching* _dev_inout;	// this is for input and output signal (we only use the TDM part)
    SupportSwitching* _dev_mixin;	// this is the input to be mixed in (we only use the TDM part)

    tSMPathId _mixerp;
    tSMPathId _volumep;
    tSMTDMrxId _inout_tdmrx;
    tSMTDMtxId _inout_tdmtx;
    tSMTDMrxId _mixin_tdmrx;

public:
    //----------------------------------------
    ProsodyMixer(tSMPathId mixerp, tSMPathId volumep, tSMTDMrxId inout_tdmrx, tSMTDMtxId inout_tdmtx, tSMTDMrxId mixin_tdmrx, AculabSwitch* sw_inout, AculabSwitch* sw_mixin)
    {
	_dev_inout = new SupportSwitching(sw_inout);
	_dev_mixin = new SupportSwitching(sw_mixin);

	_mixerp = mixerp;
	_volumep = volumep;
	_inout_tdmrx = inout_tdmrx;
	_inout_tdmtx = inout_tdmtx;
	_mixin_tdmrx = mixin_tdmrx;
    }

    //----------------------------------------
    ~ProsodyMixer()
    {
	sm_tdmtx_destroy(_inout_tdmtx);
	sm_tdmrx_destroy(_inout_tdmrx);
	sm_tdmrx_destroy(_mixin_tdmrx);
	sm_path_destroy(_mixerp);
	sm_path_destroy(_volumep);

	delete _dev_inout;
	_dev_inout = NULL;
	delete _dev_mixin;
	_dev_mixin = NULL;
    }

    //----------------------------------------
    SupportSwitching* GetInoutDevice()
    {
	return IsValid() ? _dev_inout : 0;
    }

    //----------------------------------------
    SupportSwitching* GetMixinDevice()
    {
	return IsValid() ? _dev_mixin : 0;
    }
};

const char *errmsg(int rc)
{
    static SM_ERROR_NAME_PARMS err;
    MEMZERO(err);
    err.code = rc;
    return sm_error_name(&err);
}


ProsodyMixer* ProsodyModule::AllocateMixer(int volume)
{
    SM_PATH_CREATE_PARMS mixerp;
    SM_PATH_CREATE_PARMS volumep;
    VoiceDevice* inout = 0;
    VoiceDevice* mixin = 0;
    AculabSwitch* sw_inout;
    AculabSwitch* sw_mixin;

    if (!IsEnabled() || !_initialized)
	return 0;

    // we need two addresses here
    if (AculabBusManager::Instance()->GetNumAddressRemaining(_module) < 2) 
	return 0;

    // create the mixer path
    INIT_ACU_SM_STRUCT(&mixerp);
    mixerp.module = _module;
    int rc = sm_path_create(&mixerp);

    // no more resource
    if (rc == ERR_SM_NO_RESOURCES)
	return 0;

    if (rc) {
	CLog::Error(THISMODULE, "[%s] Can't create mixer path, err=%s", GetName(), errmsg(rc));
	return 0;
    }

    // create the volume path
    INIT_ACU_SM_STRUCT(&volumep);
    volumep.module = _module;
    rc = sm_path_create(&volumep);

    // no more resource
    if (rc == ERR_SM_NO_RESOURCES) {
	sm_path_destroy(mixerp.path);
	return 0;
    }

    if (rc) {
	sm_path_destroy(mixerp.path);
	CLog::Error(THISMODULE, "[%s] Can't create volume path, err=%s", GetName(), errmsg(rc));
	return 0;
    }

    // allocate two TDM address; one for input/output and another for mixin
    char devname[64];
    sprintf(devname, "pros%u-%x-inout", _module_ix, mixerp.path);
    sw_inout = new AculabSwitch(devname, _serial_no, _card_id, -1, -1, true, _module);

    sprintf(devname, "pros%u-%x-mixin", _module_ix, mixerp.path);
    sw_mixin = new AculabSwitch(devname, _serial_no, _card_id, -1, -1, true, _module);

    // mixerp TDM RX/TX objects for input/output
    SM_TDMRX_CREATE_PARMS inout_tdmrx;
    MEMZERO(inout_tdmrx);
    inout_tdmrx.module = _module;
    inout_tdmrx.stream = ((AculabAddress*)sw_inout->GetAddress(SF_Aculab))->GetStream();
    inout_tdmrx.timeslot = ((AculabAddress*)sw_inout->GetAddress(SF_Aculab))->GetTimeslot();
    inout_tdmrx.type = _companding;
    rc = sm_tdmrx_create(&inout_tdmrx);

    SM_TDMTX_CREATE_PARMS inout_tdmtx;
    MEMZERO(inout_tdmtx);
    inout_tdmtx.module = _module;
    inout_tdmtx.stream = ((AculabAddress*)sw_inout->GetAddress(SF_Aculab))->GetStream();
    inout_tdmtx.timeslot = ((AculabAddress*)sw_inout->GetAddress(SF_Aculab))->GetTimeslot();
    inout_tdmtx.type = _companding;
    rc = sm_tdmtx_create(&inout_tdmtx);

    // mixerp TDM RX object for mixin
    SM_TDMRX_CREATE_PARMS mixin_tdmrx;
    MEMZERO(mixin_tdmrx);
    mixin_tdmrx.module = _module;
    mixin_tdmrx.stream = ((AculabAddress*)sw_mixin->GetAddress(SF_Aculab))->GetStream();
    mixin_tdmrx.timeslot = ((AculabAddress*)sw_mixin->GetAddress(SF_Aculab))->GetTimeslot();
    mixin_tdmrx.type = _companding;
    rc = sm_tdmrx_create(&mixin_tdmrx);

    // now make this mixer object to listen from TDM in
    SM_TDMRX_DATAFEED_PARMS inout_tdmrx_df;
    MEMZERO(inout_tdmrx_df);
    inout_tdmrx_df.tdmrx = inout_tdmrx.tdmrx;
    rc = sm_tdmrx_get_datafeed(&inout_tdmrx_df);

    SM_PATH_DATAFEED_CONNECT_PARMS mixerp_conn;
    MEMZERO(mixerp_conn);
    mixerp_conn.path = mixerp.path;
    mixerp_conn.data_source = inout_tdmrx_df.datafeed;
    rc = sm_path_datafeed_connect(&mixerp_conn);

    // now make TDM out to listen from the mixer object
    SM_PATH_DATAFEED_PARMS mixerp_df;
    MEMZERO(mixerp_df);
    mixerp_df.path = mixerp.path;
    rc = sm_path_get_datafeed(&mixerp_df);

    SM_TDMTX_DATAFEED_CONNECT_PARMS tdmtx_connect;
    MEMZERO(tdmtx_connect);
    tdmtx_connect.tdmtx = inout_tdmtx.tdmtx;
    tdmtx_connect.data_source = mixerp_df.datafeed;
    rc = sm_tdmtx_datafeed_connect(&tdmtx_connect);

    // get volume datafeed needed to create a mixer object
    SM_PATH_DATAFEED_PARMS volumep_df;
    MEMZERO(mixerp_df);
    volumep_df.path = volumep.path;
    rc = sm_path_get_datafeed(&volumep_df);

    // now mixerp the mixer object
    SM_PATH_MIX_PARMS mixer;
    MEMZERO(mixer);
    mixer.path = mixerp.path;
    mixer.enable = 1;
    mixer.mixin = volumep_df.datafeed;
    mixer.volume = -3;
    rc = sm_path_mix(&mixer);

    // now make volume to listen from mixin
    SM_TDMRX_DATAFEED_PARMS mixin_tdmrx_df;
    MEMZERO(mixin_tdmrx_df);
    mixin_tdmrx_df.tdmrx = mixin_tdmrx.tdmrx;
    rc = sm_tdmrx_get_datafeed(&mixin_tdmrx_df);

    SM_PATH_DATAFEED_CONNECT_PARMS volumep_conn;
    MEMZERO(volumep_conn);
    volumep_conn.path = volumep.path;
    volumep_conn.data_source = mixin_tdmrx_df.datafeed;
    rc = sm_path_datafeed_connect(&volumep_conn);

    // adjust volume to allowable limit
    if (volume > 8) volume = 8;
    else if (volume < -24) volume = -24;

    // and set mixin volume
    SM_PATH_AGC_PARMS agc;
    MEMZERO(agc);
    agc.path = volumep.path;
    agc.agc = 0;
    agc.volume = volume;
    rc = sm_path_agc(&agc);

    return new ProsodyMixer(mixerp.path, volumep.path, inout_tdmrx.tdmrx, inout_tdmtx.tdmtx, mixin_tdmrx.tdmrx, sw_inout, sw_mixin);
}

//-------------------------------------------------------------------
VoiceMixer* ProsodyDriver::AllocateMixer(int volume)
{
    ProsodyMixer* dev = 0;

    if (!_bInitialized)
	return 0;

    if (_distMethod == PROS_DISTRIBUTION_LOWEST)
	_nextModule = 0;

    for (int m=0;m<_modules.size();m++) {
	int mod = _nextModule;
	if (_modules[mod]->SupportVoice()) {
	    dev = _modules[mod]->AllocateMixer(volume);
	}

	// select next module
	_nextModule=(_nextModule+1)%_modules.size();

	if (dev)
	    return dev;
    }

    return 0;
}

//-------------------------------------------------------------------
void ProsodyDriver::FreeMixer(VoiceMixer *mixer)
{
    if (mixer->IsValid()) {
	delete mixer;
	mixer = NULL;
    }
}

#endif //XGATE HAS ACULABSWITCH
#endif //XGATE HAS PROSODY



