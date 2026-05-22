/*----------------------------------------------------------------* 
 * $Archive: /xGate/VoiceManager.h $
 * $Date: 1/02/05 15:55 $
 * $Revision: 15 $
 *
 * $History: VoiceManager.h $
 * 
 * *****************  Version 15  *****************
 * User: Junanto      Date: 1/02/05    Time: 15:55
 * Updated in $/xGate
 * added API WaitForSilence (only for Prosody)
 * 
 * *****************  Version 14  *****************
 * User: Sonny        Date: 18/10/01   Time: 11:59
 * Updated in $/xGate
 * add echocanceller
 * 
 * *****************  Version 13  *****************
 * User: Sonny        Date: 18/09/01   Time: 21:50
 * Updated in $/xGate
 * Conferece device does not detect dtmf
 * 
 * *****************  Version 12  *****************
 * User: Sonny        Date: 13/09/01   Time: 16:28
 * Updated in $/xGate
 * changed conferencing api 
 * 
 * *****************  Version 11  *****************
 * User: Sonny        Date: 12/09/01   Time: 13:01
 * Updated in $/xGate
 * add conferencing capability
 * 
 * *****************  Version 10  *****************
 * User: Sonny        Date: 6/09/01    Time: 13:56
 * Updated in $/xGate
 * temporary checkin for kholis
 * 
 * *****************  Version 9  *****************
 * User: Bennylp      Date: 7/31/01    Time: 7:26p
 * Updated in $/xGate
 * Changed IsEnabled() to const.
 * 
 * *****************  Version 8  *****************
 * User: Junanto      Date: 4/06/01    Time: 13:53
 * Updated in $/xGate
 * VoiceDriver can choose priority
 * 
 * *****************  Version 7  *****************
 * User: Junanto      Date: 21/05/01   Time: 14:43
 * Updated in $/xGate
 * Inidividual Prosody module is now configurable
 * 
 * *****************  Version 6  *****************
 * User: Junanto      Date: 11/05/01   Time: 11:30
 * Updated in $/xGate
 * Switch voice and call automatically during incoming
 * call when a task needs automatic voice allocation
 * 
 * *****************  Version 5  *****************
 * User: Junanto      Date: 24/04/01   Time: 20:04
 * Updated in $/xGate
 * added mecanism to deallocate voice device by VoiceManager 
 * and release the switching resource
 * 
 * *****************  Version 4  *****************
 * User: Bennylp      Date: 4/23/01    Time: 10:47p
 * Updated in $/xGate
 * Fixed std namespace conflicts.
 * 
 * *****************  Version 3  *****************
 * User: Junanto      Date: 22/04/01   Time: 21:27
 * Updated in $/xGate
 * voxAlloc and voxPlayFile tested on Prosody
 * 
 * *****************  Version 2  *****************
 * User: Junanto      Date: 21/04/01   Time: 23:54
 * Updated in $/xGate
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


#if !defined(_VOICEMANAGER_H__INCLUDED_)
#define _VOICEMANAGER_H__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "serviceprovider.h"
#include "voicedevice.h"
#include "api.h"

#define API_CALL __attribute__((stdcall)) //linux porting
#define _MAX_PATH PATH_MAX
#define _MAX_DRIVE          3
#define _MAX_FNAME          256
#define _MAX_DIR            _MAX_FNAME
#define _MAX_EXT            _MAX_FNAME

#define VOXFILEEXT			".vox"


/** Forward declaration
 */
class VoiceAPI;
class VoiceDriver;
class VoiceModule;
class Cfg_Tree_Group;


typedef List<VoiceDriver*> ListVoiceDriver;
typedef List<VoiceModule*> ListVoiceModule;

/** VoiceDriver: Each different voice board manufacturer
    is represented by an instance of this object
 */
class VoiceDriver : public ServiceProvider
{
public:
    VoiceDriver(const char *name,bool attachToTask,bool polled,Priority prio = Low_Priority);
    virtual ~VoiceDriver();

    virtual VoiceDevice *AllocVoiceDevice() = 0;
    virtual ConferenceRoom* AllocConferenceRoom(int room_size) { return NULL; }
    virtual EchoCanceller* AllocEchoCanceller() { return NULL; }

    virtual void FreeVoiceDevice(VoiceDevice *device) = 0;

};


/** VoiceModule: a base class that represents a physical group 
    of voice resources
 */
class VoiceModule : public ServiceProvider
{
public:
    /** VoiceModule's constructor/destructor
     */
    VoiceModule(const char* name);
    virtual ~VoiceModule();

    /** Show configuration for the specified entry.
	This function will be called by the framework whenever it needs to display the
	specified entry. Previously this instance should tell the entry that it wishes to
	handle displaying the entry by calling entry->RegisterShowHandler().
     */
    virtual bool OnShowConfig(
	const Cfg_Tree_Entry* entry,
	String& output, 
	const String& indent, 
	int indent_size) const = 0;

    Cfg_Tree_Group* GetConfigTree();

    /** Terminal command callback
     */
    virtual void DoTermCmd(Term_Session *session, bool no, Term_Cmd *cmd) = 0;

    /** Load: Called once at initialization time
     */
    virtual bool Load() = 0;

    /** Unload: Called once at termination time
     */
    virtual bool Unload() = 0;

    /** A voice module is enabled if a resource can be allocated
	from it
     */
    virtual void Enable();
    virtual void Disable();

    virtual bool SupportVoice() { return _support_voice; }
    virtual bool SupportConference() { return _support_conference; }
    virtual bool SupportEchoCanceller() { return _support_echo_canceller; }

    bool IsEnabled() const;

protected:
    void SetConfigTree(Cfg_Tree_Group* cfgTree);

    bool _support_voice;
    bool _support_conference;
    bool _support_echo_canceller;

    bool _enabled;


private:
    Cfg_Tree_Group* _cfgTree;
};


/** VoiceManager: This singleton serves as entry point between Kernel
    and all voice drivers
 */
class VoiceManager : public ServiceProvider
{
    friend class VoiceAPI;
public:
    /** Destructor
     */
    virtual ~VoiceManager();

    /** Create the only instance of this class
     */
    static VoiceManager *Create();

    /** Return a pointer to the only instance of this class
     */
    static VoiceManager *Instance();

    /** This function will be called by the service framework after 
        instantiation of this class, to allow the instance to initialize
	itself.
     */
    virtual bool Load();

    /** This function will be called by the service framework before it is
        destroyed/deleted, to allow the instance to cleanup itself.
     */
    virtual bool Unload();

    /** This function will be called by the service framework before it is
	destroyed/deleted, to allow the instance to cleanup itself.
     */
    virtual bool AttachTask(CTask *task);

    /** This function will be called whenever a task is about to be deleted.
     */
    virtual void DetachTask(CTask *task);

    /** This function will be called if this instance has a pending job for
	the task.
     */
    virtual bool CancelJob(CTask *task);

    /** Each instantiated voice driver is kept in a list. These functions are
	called at construction and destruction of voice driver descendent
     */
    void InsertVoiceDriver(VoiceDriver* voxdrv);
    void RemoveVoiceDriver(VoiceDriver* voxdrv);

    /** Each instantiated voice module is kept in a list. These functions are
	called at construction and destruction of voice module descendent
     */
    void InsertVoiceModule(VoiceModule* module);
    void RemoveVoiceModule(VoiceModule* module);
    Cfg_Tree_Group* GetConfigTree();

    STATUS SuspendTask(VoiceDevice *);
    STATUS ResumeTask(VoiceDevice *);
    void SetTaskLastError(VoiceDevice *,int error,int exterr = 0,int severity = 0);

    STATUS SetVoiceFormat(CTask *task,VoiceFormat format);
    STATUS GetVoiceFormat(CTask *task,VoiceFormat *format);
    STATUS EnableInterrupt(CTask *task);
    STATUS DisableInterrupt(CTask *task);
    STATUS SetMaxRecTime(CTask *task,int maxrectime);
    STATUS GetMaxRecTime(CTask *task,int *maxrectime);
    STATUS SetInterDigitTimeout(CTask *task,int maxidtime);
    STATUS GetInterDigitTimeout(CTask *task,int *maxidtime);
    STATUS SetDigitTimeout(CTask *task,int maxtime);
    STATUS GetDigitTimeout(CTask *task,int *maxtime);
    STATUS SetTermDigits(CTask *task,char *termdigs);
    STATUS GetTermDigits(CTask *task,char *termdigs);
    STATUS SetRecordMaxSilence(CTask *task,int maxsil); 
    STATUS GetRecordMaxSilence(CTask *task,int *maxsil);
    STATUS SetVolume(CTask *task, int dB);
    STATUS GetVolume(CTask *task, int *dB);
    STATUS SetAGC(CTask *task, int agc);
    STATUS GetAGC(CTask *task, int *agc);
    STATUS PlayFile(CTask *task,char *filename,bool wav_mode);
    STATUS PlayFileBkgnd(CTask *task,char *filename,int repeat_flag);
    STATUS StopPlayFileBkgnd(CTask *task,int waiteof_flag);
    STATUS PlayTone(CTask *task,int freq1,int db1,int freq2,int db2,int duration);
    STATUS PlayDtmf(CTask *task,char *dtmf);
    STATUS RecordFile(CTask *task,char *filename,bool wav_mode);
    STATUS EndRecordFile_t(CTask *task);
    STATUS GetDigits(CTask *task,int *retval,int maxlength,char *buffer);
    STATUS GetLastTermDigit(CTask *task,char *buffer);
    STATUS ClearDigits(CTask *task);
    STATUS PeekDigit(CTask *task,char *buffer);
    STATUS SetLanguage(CTask *task,const char* langID);
    STATUS SpellNumber(CTask *task,int number);
    STATUS Alloc(CTask *task);
    STATUS Free(CTask *task);
    STATUS GetDevName(CTask *task,char *devname);
    STATUS GetMedia(CTask *task,VoiceDevice **device);
    STATUS SetHostInfo(CTask *task, char *conf_id, int status); // T 1710
    STATUS GetHostInfo(CTask *task, int *retval,char *conf_id); // T 1710
    STATUS WaitForSilence(CTask *task, int max_wait_secs, int* remaining_time);

    STATUS ConfCreateRoom(CTask* task, int *room_handle, char *room_name, int room_size);
    //STATUS ConfDeleteRoom(CTask* task, int room_handle); // T 1710
    STATUS ConfDeleteRoom(CTask* task, int room_handle, char *conf_id); // T 1710
    STATUS ConfFindRoom(CTask* task, int *room_handle, char *room_name);
    STATUS ConfOpenMedia(CTask* task, VoiceDevice **device, int room_handle);
    STATUS ConfCloseMedia(CTask* task, int room_handle, VoiceDevice *device);
    STATUS ConfEnableMedia(CTask* task, int room_handle, VoiceDevice *device);
    STATUS ConfDisableMedia(CTask* task, int room_handle, VoiceDevice *device);
    STATUS ConfSetMediaInputVolAGC(CTask* task, VoiceDevice *device, int dB, int agc);
    STATUS ConfSetMediaOutputVolAGC(CTask* task, VoiceDevice *device, int dB, int agc);

    STATUS EcrAlloc(CTask* task, EchoCanceller** handle);
    STATUS EcrFree(CTask* task, EchoCanceller* handle);
    STATUS EcrGetMediaPrimary(CTask* task, VoiceDevice** device, EchoCanceller* handle);
    STATUS EcrGetMediaReference(CTask* task, VoiceDevice** device, EchoCanceller* handle);
    STATUS EcrStartCancelling(CTask* task, EchoCanceller* handle);
    STATUS EcrReinitCancelling(CTask* task, EchoCanceller* handle);
    STATUS EcrStopCancelling(CTask* task, EchoCanceller* handle);
    
    STATUS MixerAlloc(CTask* task, VoiceMixer** mixer, int volume);
    STATUS MixerGetMediaInout(CTask* task, SupportSwitching** inout, VoiceMixer* mixer);
    STATUS MixerGetMediaMixin(CTask* task, SupportSwitching** mixin, VoiceMixer* mixer);
    STATUS MixerFree(CTask* task, VoiceMixer* mixer);

	pthread_t _hPlayFileThread ; // added by IPPBX
private:
    enum CommandId {
	Cmd_Voice,
    };

    int _tlsId;
    VoiceAPI* _pAPI;
    ListVoiceDriver _listVoiceDriver;
    ListVoiceModule _listVoiceModule;
    static VoiceManager* _theVoiceManager;
    Cfg_Tree_Group* _voiceCfgTree;

    List<ConferenceRoom*> _conf_rooms;
    List<EchoCanceller*> _ecr_devices;

    //-------------------------------------------------------------------
    // Parameters specific to voice operations
    VoiceFormat _cfg_voxFileFormat;	// data digitization format for voice files
    int _cfg_maxRecordTime;		// max recording time in seconds
	int _cfg_recordMaxSilence;		// max detected silence time for recording
    int _cfg_maxInterDigitTime;		// max inter digit time in seconds
    int _cfg_maxGetDigitTime;		// max timeout of get digit in seconds
    char _cfg_terminalDigits[VOICE_MAXDIGITS+1]; // list of terminator digits

    VoiceDevice *GetTaskVoiceDevice(CTask *task,bool bWarning=true);
    const char *GetTaskLanguage(CTask *task,bool bWarning=true);
    void BuildFullVoiceFileName(char *fullpath,const char *basedir,const char *subdir,const char *lang,const char *filename);
    void ResetConfig(VoiceDevice *pdev);

    /** Terminal command callback
     */
    void DoTermCmd(Term_Session *session, bool no, Term_Cmd *cmd);

    /**
     Show configuration for the specified entry.
     This function will be called by the framework whenever it needs to display the
     specified entry. Previously this instance should tell the entry that it wishes to
     handle displaying the entry by calling entry->RegisterShowHandler().

     When this instance is deleted, it should call entry->UnregisterShowHandler()
     so that the handler will not be called (which will cause GPF).
     */
    virtual bool OnShowConfig(
	const Cfg_Tree_Entry* entry,
	String& output, 
	const String& indent, 
	int indent_size) const;

    /** Find a voice module by name
     */
    VoiceModule *FindModuleByName(const char* name);

    /** Make target device switch from source device
     */
    void Listen(SupportSwitching *target,SupportSwitching *source);

    // private constructor
    VoiceManager();
};


/** VoiceAPI: interface for user applications
 */
class VoiceAPI : CAPIProvider
{
    VoiceManager *_manager;

public:
    VoiceAPI(int provid,VoiceManager *manager);
    ~VoiceAPI();

    STATUS API_CALL SetVoiceFormat(CTask *task,VoiceFormat format);
    STATUS API_CALL GetVoiceFormat(CTask *task,VoiceFormat *format);
    STATUS API_CALL EnableInterrupt(CTask *task);
    STATUS API_CALL DisableInterrupt(CTask *task);
    STATUS API_CALL SetMaxRecTime(CTask *task,int maxrectime);
    STATUS API_CALL GetMaxRecTime(CTask *task,int *maxrectime);
    STATUS API_CALL SetInterDigitTimeout(CTask *task,int maxidtime);
    STATUS API_CALL GetInterDigitTimeout(CTask *task,int *maxidtime);
    STATUS API_CALL SetDigitTimeout(CTask *task,int maxtime);
    STATUS API_CALL GetDigitTimeout(CTask *task,int *maxtime);
    STATUS API_CALL SetTermDigits(CTask *task,char *termdigs);
    STATUS API_CALL GetTermDigits(CTask *task,char *termdigs);
	STATUS API_CALL SetRecordMaxSilence(CTask *task,int maxsil);
    STATUS API_CALL GetRecordMaxSilence(CTask *task,int *maxsil);
    STATUS API_CALL SetVolume(CTask *task, int dB);
    STATUS API_CALL GetVolume(CTask *task, int *dB);
    STATUS API_CALL SetAGC(CTask *task, int agc);
    STATUS API_CALL GetAGC(CTask *task, int *agc);
    STATUS API_CALL PlayFile(CTask *task,char *filename);
    STATUS thread_function_VoxPlayFile(CTask *task);
    STATUS API_CALL PlayFileBkgnd(CTask *task,char *filename,int repeat_flag) ;
    STATUS API_CALL StopPlayFileBkgnd(CTask *task,int waiteof_flag) ;
    STATUS API_CALL PlayTone(CTask *task,int freq1,int db1,int freq2,int db2,int duration);
    STATUS API_CALL PlayDtmf(CTask *task,char *dtmf);
    STATUS API_CALL RecordFile(CTask *task,char *filename);
    STATUS API_CALL EndRecordFile_t(CTask *task);
    STATUS API_CALL GetDigits(CTask *task,int *retval,int maxlength,char *buffer);
    STATUS API_CALL GetLastTermDigit(CTask *task,char *buffer);
    STATUS API_CALL ClearDigits(CTask *task);
    STATUS API_CALL PeekDigit(CTask *task,char *buffer);
    STATUS API_CALL SetLanguage(CTask *task,const char* langID);
    STATUS API_CALL SpellNumber(CTask *task,int number);
    STATUS API_CALL Alloc(CTask *task);
    STATUS API_CALL Free(CTask *task);
    STATUS API_CALL GetDevName(CTask *task,char *devname);
    STATUS API_CALL SetHostInfo(CTask *task, char *conf_id, int status); // T 1710
    STATUS API_CALL GetHostInfo(CTask *task, int *retval, char *conf_id); // T 1710
    STATUS API_CALL GetMedia(CTask *task,VoiceDevice **device);
    STATUS API_CALL WaitForSilence(CTask *task, int max_wait_secs, int* remaining_time);

    STATUS API_CALL PlayWAVFile(CTask *task,char *filename);
    STATUS API_CALL RecordWAVFile(CTask *task,char *filename);

    STATUS API_CALL ConfCreateRoom(CTask *task, int *room_handle, char *room_name, int room_size);
    //STATUS API_CALL ConfDeleteRoom(CTask *task, int room_handle); // T 1710
    STATUS API_CALL ConfDeleteRoom(CTask *task, int room_handle, char *conf_id); // T 1710
    STATUS API_CALL ConfFindRoom(CTask *task, int *room_handle, char *room_name);
    STATUS API_CALL ConfOpenMedia(CTask *task, VoiceDevice **device, int room_handle);
    STATUS API_CALL ConfCloseMedia(CTask *task, int room_handle, VoiceDevice *device);
    STATUS API_CALL ConfEnableMedia(CTask* task, int room_handle, VoiceDevice *device);
    STATUS API_CALL ConfDisableMedia(CTask* task, int room_handle, VoiceDevice *device);
    STATUS API_CALL ConfSetMediaInputVolAGC(CTask* task, VoiceDevice *device, int dB, int agc);
    STATUS API_CALL ConfSetMediaOutputVolAGC(CTask* task, VoiceDevice *device, int dB, int agc);

    STATUS API_CALL EcrAlloc(CTask* task, EchoCanceller** handle);
    STATUS API_CALL EcrFree(CTask* task, EchoCanceller* handle);
    STATUS API_CALL EcrGetMediaPrimary(CTask* task, VoiceDevice** device, EchoCanceller* handle);
    STATUS API_CALL EcrGetMediaReference(CTask* task, VoiceDevice** device, EchoCanceller* handle);
    STATUS API_CALL EcrStartCancelling(CTask* task, EchoCanceller* handle);
    STATUS API_CALL EcrReinitCancelling(CTask* task, EchoCanceller* handle);
    STATUS API_CALL EcrStopCancelling(CTask* task, EchoCanceller* handle);

    STATUS API_CALL MixerAlloc(CTask* task, VoiceMixer** mixer, int volume);
    STATUS API_CALL MixerGetMediaInout(CTask* task, SupportSwitching** inout, VoiceMixer* mixer);
    STATUS API_CALL MixerGetMediaMixin(CTask* task, SupportSwitching** mixin, VoiceMixer* mixer);
    STATUS API_CALL MixerFree(CTask* task, VoiceMixer* mixer);
};

#endif // !defined(_VOICEMANAGER_H__INCLUDED_)
