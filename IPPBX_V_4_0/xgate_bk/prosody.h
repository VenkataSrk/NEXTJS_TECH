/*----------------------------------------------------------------* 
 * $Archive: /xGate/prosody.h $
 * $Date: 1/02/05 15:55 $
 * $Revision: 25 $
 *
 * $History: prosody.h $
 * 
 * *****************  Version 25  *****************
 * User: Junanto      Date: 1/02/05    Time: 15:55
 * Updated in $/xGate
 * added API WaitForSilence (only for Prosody)
 * 
 * *****************  Version 24  *****************
 * User: Bennylp      Date: 12/07/04   Time: 2:27p
 * Updated in $/xGate
 * Added support for serial number registering in LM
 * 
 * *****************  Version 23  *****************
 * User: Sonny        Date: 1/22/03    Time: 10:28a
 * Updated in $/xGate
 * Implemented VoxPlayBkgnd functionality
 * 
 * *****************  Version 22  *****************
 * User: Junanto      Date: 8/03/02    Time: 19:02
 * Updated in $/xGate
 * 
 * *****************  Version 21  *****************
 * User: Junanto      Date: 19/10/01   Time: 13:35
 * Updated in $/xGate
 * sm_reset_channel is no longer called during allocation
 * since it is a blocking and long process
 * 
 * *****************  Version 20  *****************
 * User: Sonny        Date: 18/10/01   Time: 11:59
 * Updated in $/xGate
 * add echocanceller
 * 
 * *****************  Version 19  *****************
 * User: Junanto      Date: 17/10/01   Time: 14:58
 * Updated in $/xGate
 * Attempt to optimize events polling method (polling only if
 * necessary)
 * 
 * *****************  Version 18  *****************
 * User: Sonny        Date: 18/09/01   Time: 21:50
 * Updated in $/xGate
 * Conferece device does not detect dtmf
 * 
 * *****************  Version 17  *****************
 * User: Sonny        Date: 12/09/01   Time: 13:01
 * Updated in $/xGate
 * add conferencing capability
 * 
 * *****************  Version 16  *****************
 * User: Sonny        Date: 6/09/01    Time: 13:56
 * Updated in $/xGate
 * temporary checkin for kholis
 * 
 * *****************  Version 15  *****************
 * User: Junanto      Date: 16/08/01   Time: 21:12
 * Updated in $/xGate
 * Integrated with DeviceUsage class
 * 
 * *****************  Version 14  *****************
 * User: Junanto      Date: 27/06/01   Time: 17:37
 * Updated in $/xGate
 * Fixed error handling during hardware error in PlayFile
 * 
 * *****************  Version 13  *****************
 * User: Junanto      Date: 4/06/01    Time: 13:53
 * Updated in $/xGate
 * Added new command tone and priority set to Low
 * 
 * *****************  Version 12  *****************
 * User: Junanto      Date: 3/06/01    Time: 21:07
 * Updated in $/xGate
 * Fixed bug when a completed PlayFile is cancelled
 * 
 * *****************  Version 11  *****************
 * User: Junanto      Date: 2/06/01    Time: 1:09
 * Updated in $/xGate
 * replay_start is kicked from the I/O thread instead
 * from the main thread
 * 
 * *****************  Version 10  *****************
 * User: Junanto      Date: 2/06/01    Time: 0:47
 * Updated in $/xGate
 * Changed PlayFile implementation to multi-threading
 * 
 * *****************  Version 9  *****************
 * User: Junanto      Date: 31/05/01   Time: 19:17
 * Updated in $/xGate
 * Removed multi-thread architecture for
 * asynchronous play dtmf
 * 
 * *****************  Version 8  *****************
 * User: Junanto      Date: 27/05/01   Time: 18:43
 * Updated in $/xGate
 * - Fixed bug during DetachTask resulting from "kill"
 * - Added parameter dtmf-detection
 * 
 * *****************  Version 7  *****************
 * User: Junanto      Date: 21/05/01   Time: 14:43
 * Updated in $/xGate
 * Inidividual Prosody module is now configurable
 * 
 * *****************  Version 6  *****************
 * User: Junanto      Date: 8/05/01    Time: 19:21
 * Updated in $/xGate
 * On Unload, the driver waits until all
 * threads are terminated or timeout
 * 
 * *****************  Version 5  *****************
 * User: Junanto      Date: 24/04/01   Time: 20:04
 * Updated in $/xGate
 * added mecanism to deallocate voice device by VoiceManager 
 * and release the switching resource
 * 
 * *****************  Version 4  *****************
 * User: Junanto      Date: 24/04/01   Time: 18:25
 * Updated in $/xGate
 * first link between Prosody/Aculab Media and switching framework
 * 
 * *****************  Version 3  *****************
 * User: Bennylp      Date: 4/23/01    Time: 10:44p
 * Updated in $/xGate
 * Fixed std namespace conflicts.
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
 * *****************  Version 1  *****************
 * User: Junanto      Date: 21/04/01   Time: 23:08
 * Created in $/xGate
 * Integrated
 * 
 *----------------------------------------------------------------*/

/*----------------------------------------------------------------* 
 * Description:
 *  
 *----------------------------------------------------------------*/


#if !defined(AFX_PROSODY_H__INCLUDED_)
#define AFX_PROSODY_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <pthread.h>
#include <algorithm>          //05092016  Event
#include <deque>              //05092016  Event
#include "voicedevice.h"
#include "voicemanager.h"
#include "devusage.h"
#include "/usr/local/aculab/v6/TiNG/highapi/smhlib.h"
#include "/usr/local/aculab/v6/TiNG/highapi/smclib.h"
#include "/usr/local/aculab/v6/include/res_lib.h"

#define WAIT_TIMEOUT ETIMEDOUT  //05092016  Event

#define PROS_DISTRIBUTION_LOWEST	0
#define PROS_DISTRIBUTION_CYCLIC	1
#define PROS_DISTRIBUTION_MAX		PROS_DISTRIBUTION_CYCLIC

#define	K_PROSODY_X_TIMESLOTS_PER_STREAM 128	
#define	K_PROSODY_CPCI_TIMESLOTS_PER_STREAM	 32

/** Forward declaration
 */
class ProsodyAsyncJob;
class ProsodyModule;
class ProsodyDriver;
class VMP_Interface;
class Interface;
class RTP_Channel;

#define COND_WAIT
#define WFMO
#define WAIT_OBJECT_0   0

#define LOG_DETAIL 0
#define LOG_INFO   0
#define PLOG(fmt, ...)   if(LOG_DETAIL)printf("\n [ DETAIL ] %s:%d:%s "fmt,__FILE__, __LINE__,__FUNCTION__,##__VA_ARGS__)
#define PLOG_INFO(fmt, ...)   if(LOG_INFO)printf("\n [ DETAIL ] %s:%d:%s "fmt,__FILE__, __LINE__,__FUNCTION__,##__VA_ARGS__)

typedef struct tSMEventId_t {
    int fd;
    int mode;
  //  tSM_EVENT_INTERNALS eip;
} tSMEventId_t;

typedef struct sm_channel_set_event_parms_t {
    void *channel;
    int event_type;
    int issue_events;
    tSMEventId *event;
} SM_CHANNEL_SET_EVENT_PARMS_t;


#ifdef COND_WAIT

    struct m_wfmo_t_
    {
        pthread_mutex_t Mutex;
        pthread_cond_t CVariable;
        int RefCount;
        union
        {
            int FiredEvent; //WFSO
            int EventsLeft; //WFMO
        } Status;
        bool WaitAll;
        bool StillWaiting;

        void Destroy()
        {
            pthread_mutex_destroy(&Mutex);
            pthread_cond_destroy(&CVariable);
        }
    };
    typedef m_wfmo_t_ *m_wfmo_t;

    struct m_wfmo_info_t_
    {
        m_wfmo_t Waiter;
        int WaitIndex;
    };
    typedef m_wfmo_info_t_ *m_wfmo_info_t;

    struct m_event_t_
    {
        pthread_cond_t CVariable;
        pthread_mutex_t Mutex;
        bool AutoReset;
        bool State;
    #ifdef WFMO
        std::deque<m_wfmo_info_t_> RegisteredWaits;
    #endif
        SM_CHANNEL_SET_EVENT_PARMS_t _parms;
    };

    typedef void* HANDLE_T;
    typedef m_event_t_* m_event_t;
#endif

int		errorLbl( int rc, tSMChannelId channel );

/** ProsodyDevice
 */
class ProsodyDevice : public VoiceDevice
{
    friend class ProsodyDriver;

private:
    //------------------------------------------------------
    // Event/state handler function
    enum ProsodyState {
	ST_IDLE,
	ST_PLAYFILE,
	ST_CANCEL_PLAYFILE,
	ST_PLAYTONE,
	ST_PLAYDTMF,
	ST_RECORDFILE,
        ST_CANCEL_RECORDFILE,
	ST_GETDIG,
	ST_WAITSIL,
    };

    enum ProsodyEvent {
	EV_ENDPLAY,
	EV_ENDTONE,
	EV_ENDDTMF,
	EV_ENDRECORD,
	EV_DTMF,
	EV_TIMER,
	EV_STOP,
	EV_SILON,
	EV_SILOFF,
    };

    //------------------------------------------------------
    // DTMF handling
    class DigitsBuffer {
    public:
	int nbelt;
	char buffer[VOICE_MAXDIGITS];

	DigitsBuffer() { ClrDigit(); }
	void AddDigit(char dtmf) { if (nbelt<VOICE_MAXDIGITS-1) buffer[nbelt++] = dtmf; }
	void ClrDigit() { nbelt = 0; }
	void CopyDigits(char *dest,int count) {
	    if (!dest) return;

	    if (count > nbelt) count = nbelt;
	    
	    memcpy(dest,buffer,count);
	    dest[count] = '\0';

	    nbelt -= count;
	    for (int i=0;i<nbelt;i++) buffer[i] = buffer[count+i];
	}
    };

    tSMChannelId _channel;


    ProsodyDriver* _driver;
    ProsodyModule* _module;
    int _lastTaskError;
    bool _bInterruptible;	// true if play-file can be interrupted by dtmf
    VoiceFormat _format;	// ADPCM/PCM/ALaw/uLaw
    int _maxrectime;		// in seconds
    int _maxidtime;			// inter digit delay in seconds
    int _maxtime;			// max. time of get digit in seconds
	int _maxsilence;        // maximum silence detected time in seconds after which
	                        // recording should be terminated(0 for unlimited)
    time_t _tmalloc;		// time this device is allocated
    ProsodyState _state;
    ProsodyAsyncJob* _asyncJob;
    int _recordFile;			// handle of file for recording
    bool _wav_mode;
    unsigned _recorded_size;
    DigitsBuffer _digbuf;
    char _lasttermdigit;
    char _termdigs[VOICE_MAXDIGITS+1];
    char* _taskbuff;			// for asynch. GetDig operation
    int _maxDigToWait;			// for asynch. GetDig operation
    time_t _tmlastdigit;		// for asynch. GetDig operation
    time_t _tmfirstcall;		// for asynch. GetDig operation
    int _refCount;			// reference counting
    bool (ProsodyDevice::*_fHandler)(ProsodyEvent,unsigned=0);

    bool _silence;			// true when device detects silence
    int *_silence_timer;		// ret-val for WaitForSilence API

    // Statistics and diagnostics purpose
    int	_nbDetectedDtmf;

    char dtmf_digits[3];
    int dtmf_index;

    const char *StrState(ProsodyState);
    void SetState(ProsodyState);

    bool EH_Idle(ProsodyEvent,unsigned);
    bool EH_WaitForSilence(ProsodyEvent,unsigned);
    bool EH_PlayFile(ProsodyEvent,unsigned);
    bool EH_PlayTone(ProsodyEvent,unsigned);
    bool EH_PlayDtmf(ProsodyEvent,unsigned);
    bool EH_Record(ProsodyEvent,unsigned);
    bool EH_GetDig(ProsodyEvent,unsigned);

    int IncRefCount();
    int DecRefCount();

    bool FillUpReplayBuffer(int status);
    void EndPlayFile();

    void EndRecordFile();

    bool AbortPlayFile();
    bool AbortRecordFile();

public:
    ProsodyDevice(char *devname, const char* serial_num, tSMChannelId channel, int swdrv, int stream, int tslot, ProsodyDriver *driver, ProsodyModule *module);
    virtual ~ProsodyDevice();
    void InitVars();

    tSMChannelId GetChannel() { return _channel; }

    bool DevicePlayTone(int toneId,int duration);
    bool DevicePlayDtmf(const char *dtmf);

    virtual bool CancelJob();

    // deallocate this voice device
    virtual void Free();

    virtual bool IsDeviceIdle();

    // telephony actions
    virtual STATUS SetVoiceFormat(VoiceFormat format);
    virtual STATUS GetVoiceFormat(VoiceFormat *format);
    virtual STATUS EnableInterrupt();
    virtual STATUS DisableInterrupt();
    virtual STATUS SetInterDigitTimeout(int maxidtime);
    virtual STATUS GetInterDigitTimeout(int *maxidtime);
    virtual STATUS SetDigitTimeout(int maxidtime);
    virtual STATUS GetDigitTimeout(int *maxidtime);
    virtual STATUS SetMaxRecTime(int maxrectime);
    virtual STATUS GetMaxRecTime(int *maxrectime);
    virtual STATUS SetTermDigits(char *termdigs);
    virtual STATUS GetTermDigits(char *termdigs);
	virtual STATUS SetRecordMaxSilence(int maxsil);
    virtual STATUS GetRecordMaxSilence(int *maxsil);
    virtual STATUS PlayFileArray(int nbfiles,char const **filename,bool bkgnd,bool autorepeat);
    virtual STATUS StopPlayFileBkGnd(bool waiteof);
    virtual STATUS PlayTone(int freq1,int db1,int freq2,int db2,int duration);
    virtual STATUS PlayDtmf(char *dtmf);
    virtual STATUS RecordFile(char *filename);
    virtual STATUS EndRecordFile_t();
    virtual STATUS GetDigits(int length,char *buffer);
    virtual STATUS GetLastTermDigit(char *buffer);
    virtual STATUS PeekDigit(char *buffer);
    virtual STATUS ClearDigits();
    virtual STATUS WaitForSilence(int max_wait_time, int* remaining_time);

    // We support WAV files
    virtual bool SupportPlayWav() {
	return true;
    }
    virtual bool SupportRecordWav() {
	return true;
    }
    virtual STATUS PlayFileArrayWav(int nbfiles,char const **filename,bool bkgnd,bool autorepeat,bool wav_mode);
    virtual STATUS RecordFileWav(char *filename,bool wav_mode);

    bool _BkGnd; // to be used only in PlayFileBkGnd() API

private:
    int get_wav_encoding_format(int file_handle);
    bool create_wav_header(int file_handle, int encoding, int channels, int sample_rate, int bytes_second, int blk_align, int bits_sample);
};


/** Prosody Conference Room
 */
class ProsodyConferenceRoom : public ConferenceRoom {

    ProsodyConferenceRoom(tSMConference tSMConf);
    ~ProsodyConferenceRoom();

    virtual STATUS AddParty(VoiceDevice*);
    virtual STATUS RemoveParty(VoiceDevice*);

    friend class ProsodyDriver;
    
};

/** Prosody EchoCanceller
 */

class ProsodyEchoCanceller : public EchoCanceller {

    ProsodyDevice* primary_channel;
    ProsodyDevice* reference_channel;

    bool cancelling;

    ProsodyEchoCanceller(ProsodyDevice* ioChannel, ProsodyDevice* refChannel);
    ~ProsodyEchoCanceller();

    virtual VoiceDevice* GetPrimaryChannel();
    virtual VoiceDevice* GetReferenceChannel();

    virtual void StartCancelling();
    virtual void ReinitCancelling();
    virtual void StopCancelling();

    virtual void AttachTask(CTask* task);

    friend class ProsodyModule;
};

/** ProsodyDriver
 */
class ProsodyDriver : public VoiceDriver
{
    friend class ProsodyDevice;
    typedef Vector<ProsodyDevice*> VectorProsody;
    typedef Map<tSMChannelId, ProsodyDevice*> MapProsody;
    typedef Vector<ProsodyModule*> VectorModule;
    typedef List<ProsodyDevice*> ListProsody;
    typedef List<tSMConference> ListConference;

private:
    bool _bInitialized;
    time_t _tmlasttimer;
    int _nextModule;
    int _distMethod;
    int _activePlayDtmf;
    int _activePlayTone;

    ListConference _conf_rooms;
    MapProsody _devices;
    ListProsody _timed_devices;
    VectorModule _modules;

    // create events to monitor all channel
    //SM_CHANNEL_SET_EVENT_PARMS_t _recogParms;
    //SM_CHANNEL_SET_EVENT_PARMS_t _writeParms;
    //SM_CHANNEL_SET_EVENT_PARMS_t _readParms;

    m_event_t_ *_recogParms;
    m_event_t_ *_writeParms;
    m_event_t_ *_readParms;
    // points to this singleton
    static ProsodyDriver* _theProsodyDriver;

    // synchronization objects and functions
#ifndef __linux__
    HANDLE _eventQuit;			// set to signalled when xGate is quitting (main -> I/O)
    HANDLE _eventCancel;		// set to signalled when a job is cancelled (main -> I/O)
    HANDLE _eventStart;			// set to signalled when a job is started (main -> I/O)
    HANDLE _eventCompletion;	// set to signalled when a job is completed (I/O -> main)
    HANDLE _hIOThread;			// handle of I/O thread
#else
  #ifdef COND_WAIT
      HANDLE_T _eventQuit;		   // set to signalled when xGate is quitting (main -> I/O)
      HANDLE_T _eventCancel;	   // set to signalled when a job is cancelled (main -> I/O)
      HANDLE_T _eventStart;		   // set to signalled when a job is started (main -> I/O)
      HANDLE_T _eventRecord;       // set to signalled when a job is recording (main -> I/O)
      HANDLE_T _eventCompletion;   // set to signalled when a job is completed (I/O -> main)
      pthread_t _hIOThread;        //handle of I/O thread
  #endif
#endif
    //CRITICAL_SECTION _csection;  // protect access to shared data
    pthread_mutex_t	_csection;     // protect access to shared data
    ListProsody _completed;		   // contains devices with completed job (I/O -> main)
    ListProsody _cancelled;		   // contains devices with cancelled job (main -> I/O)
    unsigned _bkgndJobCount;       // background job (play file) counter
    unsigned _allocCount;		   // tracks the current number of devices allocated

    unsigned _poll_counter;
    unsigned _event_flags;

    void Lock();
    void Unlock();

#if 0
    // worker thread for refilling play buffer
    static void ProcessCancelJob();
    static bool ProcessWriteEvents();
    static bool ProcessReadEvents();
    //static DWORD WINAPI IOThread(void *arg);
    static DWORD IOThread(void *arg);
#else
    // worker thread for refilling play buffer
    void ProcessCancelJob();
    bool ProcessWriteEvents();
    //bool ProcessReadEvents();
    static DWORD ProcessReadEvents(void *arg);

    //static DWORD WINAPI IOThread(void *arg);
    static DWORD IOThread(void *arg);
#endif

    /** CancelAsyncJob: It instructs the I/O thread to cancel the current job
     */
    void CancelAsyncJob(ProsodyDevice *);

    /** ProcessRecogEvents: Called when a channel detects a DTMF
     */
    void ProcessRecogEvents();

    /** CheckPlayDtmfCompletion: Poll channels which are currently playing dtmf
     */
    void CheckPlayDtmfCompletion();

    /** CheckPlayToneCompletion: Poll channels which are currently playing tone
     */
    void CheckPlayToneCompletion();

    void CloseHandles();

    // poll DTMF recognition and play/record file completion events
    bool CheckRecognitionEvent();
    bool CheckCompletionEvent();

    // private constructor
    ProsodyDriver();
public:

    // public destructor
    virtual ~ProsodyDriver();

    /** Create the only instance of this class
     */
    static ProsodyDriver *Create();

    /** Return a pointer to the only instance of this class
     */
    static ProsodyDriver *Instance();

#ifdef COND_WAIT
    /*** Event handling functions ***/
    m_event_t CreateEvent(bool manualReset = false, bool initialState = false);
    int DestroyEvent(m_event_t event);
    int DestroyEvents();
    int WaitForEvent(m_event_t event, uint64_t milliseconds = -1);
    int UnlockedWaitForEvent(m_event_t event, uint64_t milliseconds);
    int SetEvent(m_event_t event);
    int ResetEvent(m_event_t event);
#ifdef WFMO
    int WaitForMultipleEvents(m_event_t *events, int count, bool waitAll, uint64_t milliseconds);
    int WaitForMultipleEvents(m_event_t *events, int count, bool waitAll, uint64_t milliseconds, int &index);
#endif
#ifdef PULSE
    int PulseEvent(m_event_t event);
#endif
#endif

    /** This function will be called by the service framework after 
        instantiation of this class, to allow the instance to initialize
	itself.
     */
    virtual bool Load();

    /** This function will be called by the service framework before it is
        destroyed/deleted, to allow the instance to cleanup itself.
     */
    virtual bool Unload();

    /** HandleEvents: It is called regularly by Service Provider Manager
     */

    virtual void HandleEvents();

    virtual VoiceDevice *AllocVoiceDevice();
    virtual ConferenceRoom* AllocConferenceRoom(int room_size);
    virtual EchoCanceller* AllocEchoCanceller();
    virtual VoiceMixer* AllocateMixer(int volume);

    virtual void FreeVoiceDevice(VoiceDevice *);
    virtual void FreeMixer(VoiceMixer *mixer);

    void InsertProsodyModule(ProsodyModule *module);
    void RemoveProsodyModule(ProsodyModule *module);

    /** Tracks the number of ongoing background jobs (PlayFile)
     */
    void IncBkgndJobCounter();
    void DecBkgndJobCounter();
    unsigned GetBkgndJobCounter();

    /** Tracks the number of allocated voice device
     */
    void IncAllocCounter();
    void DecAllocCounter();
    unsigned GetAllocCounter();

#ifdef XGATE_HAS_PROSODY_RTP
    Interface* GetInterface(const char* prosm_name);
    RTP_Channel* AllocateChannel(const char* prosm_name);
#endif //XGATE_HAS_PROSODY_RTP
};


/** FileReplayParam
 */
class FileReplayParam {
public:
    int			fh;				// file handle
    long		offset;			// current offset
    SM_REPLAY_PARMS	replay_parms;	// prosody replay_parms

    /////// following 6 lines code added by IPPBX
    SM_RECORD_PARMS	record_parms;	// prosody replay_parms

	FileReplayParam() : fh(0), offset(0)
	{
		INIT_ACU_SM_STRUCT(&replay_parms);
		INIT_ACU_SM_STRUCT(&record_parms); //////// this code line added by IPPBX
	}
};

typedef List<FileReplayParam*> ListReplayParam;
class ProsodyAsyncJob {
    bool _repeat_flag;
    bool _task_suspended;

    friend class ProsodyDevice;
public:
    ProsodyDevice *dev;
    ListReplayParam lrp;
    ListReplayParam::iterator current;
};


/** ProsodyFactory: A singleton class which :
    - Initialize Prosody library
    - Instantites all ProsodyInterface
    - Close Prosody library
 */
class ProsodyFactory : public ServiceProvider
{
    bool	_fOpened;
	static	int		_num_cards;
    static	int		_num_modules;
	static	std::vector<tSMCardId>	_vCardId;

    static ProsodyFactory *theProsodyFactory;


    ProsodyFactory();

public:
    virtual ~ProsodyFactory();

    /** Create: create one instance first before returning a pointer to it
     */
    static ProsodyFactory *Create();

    /** Instance: Returns a pointer to this singleton instance, if necessary
	create one instance first before returning a pointer to it
     */
    static ProsodyFactory *Instance();

	static int		GetNumCards();
	static int		GetNumModules();

    void Destroy();

    /** This function will be called by the service framework after 
        instantiation of this class, to allow the instance to initialize
	itself.
	Initialize Prosody driver and for each Prosody port, create the associated
	Interface object
     */
    virtual bool Load();

    /** This function will be called by the service framework before it is
        destroyed/deleted, to allow the instance to cleanup itself.
	Close Prosody driver
     */
    virtual bool Unload();

    /** Commit configuration. */
    virtual bool CommitConfiguration();
};

class ProsodyMixer;

/** ProsodyModule: It represents one physical Prosody module
 */
class ProsodyModule : public VoiceModule, public DeviceUsage
{
    friend class ProsodyDevice;
    friend class VMP_Interface;

    bool EC_Capable();

public:
    /** ProsodyModule's constructor/destructor
     */
    ProsodyModule(
	ACU_CARD_ID cardID, 
	tSMModuleId module, 
	int module_ix, 
	const char* name, 
/*
	kSMTimeslotType companding = kSMTimeslotTypeALaw, 
	int channel_count = 64,
	const char* serial_no = "",
	const char* hw_version = "",
	const char* card_name = "",
	const char* card_desc = "",
	const char* ip_address = "",
	const char* card_key = "");
*/
	kSMTimeslotType companding, 
	int channel_count,
	const char* serial_no,
	const char* hw_version,
	const char* card_name,
	const char* card_desc,
	const char* ip_address,
	const char* card_key);

    virtual ~ProsodyModule();

    /** Show configuration for the specified entry.
	This function will be called by the framework whenever it needs to display the
	specified entry. Previously this instance should tell the entry that it wishes to
	handle displaying the entry by calling entry->RegisterShowHandler().
     */
    virtual bool OnShowConfig(
	const Cfg_Tree_Entry* entry,
	String& output, 
	const String& indent, 
	int indent_size) const;

    /** Terminal command callback
     */
    virtual void DoTermCmd(Term_Session *session, bool no, Term_Cmd *cmd);

    /** Load: Called once at initialization time
     */
    virtual bool Load();

    /** Unload: Called once at termination time
     */
    virtual bool Unload();

    /** Returns the module
     */
    tSMModuleId GetModule();
	
    /** Returns the module index
     */
    int	    GetModuleIndex();

    kSMTimeslotType	GetCompanding(){ return _companding; }

    /** Allocate a prosody voice resource in this module
     */
    ProsodyDevice* AllocProsodyDevice();
    ProsodyDevice* AllocProsodyChannel(tSM_INT channel_type, tSM_INT channel_caps);
    EchoCanceller* AllocProsodyEchoCanceller();

    ProsodyMixer* AllocateMixer(int volume);

#ifdef XGATE_HAS_PROSODY_RTP
    VMP_Interface* GetInterface() {
	return _vmp_interface;
    }
#endif //XGATE_HAS_PROSODY_RTP

private:
    enum CommandId {
	Cmd_Volume,
	Cmd_Companding,
	Cmd_AGC,
	Cmd_DtmfDet,
	Cmd_Tone,
	Cmd_Enable,
	Cmd_Voice,
	Cmd_Conference,
	Cmd_Ecr,

    };

    struct Tone {
	int id;
	int freq;
	int ampl;
    };

    enum { MAXTONE=4 };

    tSMModuleId _module;
	int	_module_ix;
	ACU_CARD_ID	_card_id;

    bool _initialized;

    // configurable options
    int	_volume;
    int	_agc_play;
    Tone _tone[MAXTONE];
    int	_dtmfdet;
    kSMTimeslotType _companding;
#ifdef XGATE_HAS_PROSODY_RTP
    VMP_Interface* _vmp_interface;
#endif //XGATE_HAS_PROSODY_RTP
    char _serial_no[128];
    char _hw_version[128];
    char _card_name[128];
    char _card_desc[128];
    char _ip_address[128];
    char _card_key[128];

    /** Load firmware and initialize the Prosody module
     */
    virtual bool InitModule();

    /** This function will be called by the service framework once the 
	configuration file has been fully read.
     */

    virtual bool CommitConfiguration();
};


#endif // !defined(AFX_PROSODY_H__INCLUDED_)
