/*----------------------------------------------------------------* 
 * $Archive: /xGate/VoiceManager.cpp $
 * $Date: 18/02/05 18:26 $
 * $Revision: 35 $
 *
 * $History: VoiceManager.cpp $
 * 
 * *****************  Version 36  *****************
 * User: Oky        Date: 31/12/14   Time: 18:26
 * Updated in $/xGate
 * bypass allocating voice to task if media type=virtual or pipe

 * *****************  Version 35  *****************
 * User: Sonny        Date: 18/02/05   Time: 18:26
 * Updated in $/xGate
 * fixed bug: conference room will be deleted only if all devices are free
 * and the owner is detached
 * 
 * *****************  Version 34  *****************
 * User: Bennylp      Date: 2/01/05    Time: 5:48p
 * Updated in $/xGate
 * 
 * *****************  Version 33  *****************
 * User: Junanto      Date: 1/02/05    Time: 15:55
 * Updated in $/xGate
 * added API WaitForSilence (only for Prosody)
 * 
 * *****************  Version 32  *****************
 * User: Bennylp      Date: 4/23/03    Time: 5:34p
 * Updated in $/xGate
 * Administrivia: tidy-up the usage of API provider ID. Now the
 * enumeration in api.h should be used instead of bare constants.
 * 
 * *****************  Version 31  *****************
 * User: Bennylp      Date: 4/03/03    Time: 3:43p
 * Updated in $/xGate
 * Remove warning on VoxAlloc when task already has voice device
 * 
 * *****************  Version 30  *****************
 * User: Sonny        Date: 11/06/02   Time: 6:42p
 * Updated in $/xGate
 * DialogicVoice integration (in progress)
 * 
 * *****************  Version 29  *****************
 * User: Sonny        Date: 19/10/01   Time: 19:28
 * Updated in $/xGate
 * bug fixes on conference and echocanceller
 * 
 * *****************  Version 28  *****************
 * User: Sonny        Date: 19/10/01   Time: 15:46
 * Updated in $/xGate
 * 
 * *****************  Version 27  *****************
 * User: Sonny        Date: 18/10/01   Time: 11:59
 * Updated in $/xGate
 * add echocanceller
 * 
 * *****************  Version 26  *****************
 * User: Sonny        Date: 22/09/01   Time: 23:25
 * Updated in $/xGate
 * 
 * *****************  Version 25  *****************
 * User: Sonny        Date: 19/09/01   Time: 18:17
 * Updated in $/xGate
 * Conference : null device handling
 * 
 * *****************  Version 24  *****************
 * User: Sonny        Date: 9/19/01    Time: 12:36p
 * Updated in $/xGate
 * fixed bug in ConfDeleteRoom
 * 
 * *****************  Version 23  *****************
 * User: Sonny        Date: 13/09/01   Time: 16:28
 * Updated in $/xGate
 * changed conferencing api 
 * 
 * *****************  Version 22  *****************
 * User: Sonny        Date: 12/09/01   Time: 13:01
 * Updated in $/xGate
 * add conferencing capability
 * 
 * *****************  Version 21  *****************
 * User: Sonny        Date: 6/09/01    Time: 13:56
 * Updated in $/xGate
 * temporary checkin for kholis
 * 
 * *****************  Version 20  *****************
 * User: Bennylp      Date: 7/31/01    Time: 7:26p
 * Updated in $/xGate
 * Changed IsEnabled() to const.
 * 
 * *****************  Version 19  *****************
 * User: Junanto      Date: 8/06/01    Time: 10:37
 * Updated in $/xGate
 * Removed some CLog::Detail
 * 
 * *****************  Version 18  *****************
 * User: Junanto      Date: 4/06/01    Time: 13:53
 * Updated in $/xGate
 * VoiceDriver can choose priority
 * 
 * *****************  Version 17  *****************
 * User: Junanto      Date: 2/06/01    Time: 22:47
 * Updated in $/xGate
 * Added a guard against GPF
 * 
 * *****************  Version 16  *****************
 * User: Junanto      Date: 2/06/01    Time: 14:36
 * Updated in $/xGate
 * Fixed minor bugs during cancel job
 * 
 * *****************  Version 15  *****************
 * User: Junanto      Date: 31/05/01   Time: 18:05
 * Updated in $/xGate
 * Some functions changed to async
 * 
 * *****************  Version 14  *****************
 * User: Bennylp      Date: 5/30/01    Time: 3:29p
 * Updated in $/xGate
 * Fixed mem leak when task has no voice dev (TLS not freed)
 * 
 * *****************  Version 13  *****************
 * User: Junanto      Date: 27/05/01   Time: 22:14
 * Updated in $/xGate
 * 
 * *****************  Version 12  *****************
 * User: Junanto      Date: 27/05/01   Time: 20:51
 * Updated in $/xGate
 * Fixed bug in DetachTask
 * 
 * *****************  Version 11  *****************
 * User: Junanto      Date: 27/05/01   Time: 18:43
 * Updated in $/xGate
 * - Fixed bug during DetachTask resulting from "kill"
 * - Added parameter dtmf-detection
 * 
 * *****************  Version 10  *****************
 * User: Junanto      Date: 27/05/01   Time: 17:11
 * Updated in $/xGate
 * Fixed bug when a prosody is cancelled during 
 * GetDigits
 * 
 * *****************  Version 9  *****************
 * User: Junanto      Date: 21/05/01   Time: 14:43
 * Updated in $/xGate
 * Inidividual Prosody module is now configurable
 * 
 * *****************  Version 8  *****************
 * User: Junanto      Date: 17/05/01   Time: 12:42
 * Updated in $/xGate
 * Changed AutoConnectTimeslot only when 
 * there is an active incoming call
 * 
 * *****************  Version 7  *****************
 * User: Junanto      Date: 11/05/01   Time: 11:30
 * Updated in $/xGate
 * Switch voice and call automatically during incoming
 * call when a task needs automatic voice allocation
 * 
 * *****************  Version 6  *****************
 * User: Junanto      Date: 29/04/01   Time: 15:05
 * Updated in $/xGate
 * added parameter need-voice in 
 * application to configure voice
 * resource requirement per 
 * application
 * 
 * *****************  Version 5  *****************
 * User: Junanto      Date: 27/04/01   Time: 16:51
 * Updated in $/xGate
 * added DisconnectListeners() to force 
 * switch listeners to unlisten from a
 * dying device
 * 
 * *****************  Version 4  *****************
 * User: Junanto      Date: 24/04/01   Time: 20:04
 * Updated in $/xGate
 * added mecanism to deallocate voice device by VoiceManager 
 * and release the switching resource
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

//#include "stdafx.h"
#include <limits.h>
#include "xgconfig.h"
#include "kernel.h"
#include "task.h"
#include "log.h"
#include "vocab.h"
#include "MediaManager.h"
#include "callmanager.h"
#include "voicemanager.h"
#include "rtpmedia.h"
#include "PipeLine.h"
#include <libgen.h>
#include <setjmp.h>
#include <signal.h>

jmp_buf ex_buf__;
struct sigaction act;


#ifdef XGATE_HAS_PROSODY
	#include "prosody.h"
#endif

#ifdef XGATE_HAS_DIALOGIC
	#include "dialogic_voice.h"
#endif

#include "term.h"


//-------------------------------------------------------------------
static char __modname__[] = "VoxMgr";
#define THISMODULE		__modname__
#define NEWLINE			"\n"

const int VoiceProviderId = API_PROVIDER_ID_VOICE;
VoiceDevice *l_dev = NULL;

//-------------------------------------------------------------------
VoiceManager *VoiceManager::_theVoiceManager = NULL;

map <string, int> host_info; // T 1710 - To maintain host user login status in conference

//-------------------------------------------------------------------
/** TaskVoiceBlock: Specific information related to voice
    operation attached to the task
 */
class TaskVoiceBlock
{
public:
    VoiceDevice* _device;
    VoiceMixer*  _mixer;
    String	 _language;

    TaskVoiceBlock() {
	_device = 0;
	_mixer = 0;
	_language = "";
    }
};

/*----------------------------------------------------------------* 
 * Implementation of VoiceDriver
 *----------------------------------------------------------------*/

/** VoiceDriver's constructor
 */
VoiceDriver::VoiceDriver(const char *name,bool attachToTask,bool polled,Priority prio) : ServiceProvider(name,attachToTask,polled,prio)
{
    VoiceManager::Instance()->InsertVoiceDriver(this);
}

/** VoiceDriver's destructor
 */
VoiceDriver::~VoiceDriver()
{
    VoiceManager::Instance()->RemoveVoiceDriver(this);
}


/*----------------------------------------------------------------* 
 * Implementation of VoiceModule
 *----------------------------------------------------------------*/

/** VoiceModule's constructor
 */
VoiceModule::VoiceModule(const char *name) : ServiceProvider(name,false,false)
{
    _cfgTree = NULL;
    Enable();

    _support_voice = false;
    _support_conference = false;
    _support_echo_canceller = false;

    VoiceManager::Instance()->InsertVoiceModule(this);
}

/** VoiceModule's destructor
 */
VoiceModule::~VoiceModule()
{
    VoiceManager::Instance()->RemoveVoiceModule(this);
}

/** Enable this module, include the module in resource allocation
 */
void VoiceModule::Enable()
{
    _enabled = true;
}

/** Diable this module, exclude the module from resource allocation
 */
void VoiceModule::Disable()
{
    _enabled = false;
}

/** Return enable/disable state
 */
bool VoiceModule::IsEnabled() const
{
    return _enabled;
}

/** Return configuration tree associated to this module
 */
Cfg_Tree_Group* VoiceModule::GetConfigTree()
{
    return _cfgTree;
}


/** Set configuration tree associated to this module
 */
void VoiceModule::SetConfigTree(Cfg_Tree_Group* cfgTree)
{
    _cfgTree = cfgTree;
}


/*----------------------------------------------------------------* 
 * Implementation of VoiceManager
 *----------------------------------------------------------------*/

/** VoiceManager's constructor
 */
VoiceManager::VoiceManager() : ServiceProvider(THISMODULE,true,false)
{
    _pAPI = NULL;

    /** Runtime parameters default values
     */
    _cfg_voxFileFormat = VFmt_ADPCM_32K;
    _cfg_maxRecordTime = 45;
	_cfg_recordMaxSilence = 0;
    _cfg_maxInterDigitTime = 7;
    _cfg_maxGetDigitTime = 15;
    safe_strcpy(_cfg_terminalDigits, "*", VOICE_MAXDIGITS);

    Term_Cmd *cmd = new Term_Cmd(this, Cmd_Voice, "voice", "Configure voice module", PrivilegeLevelAdmin, TermModeConfig, false);
    cmd->Add(new Term_Cmd_Arg("arg1", "", CVariantValidator("", "Module name"),TermArgOptional));
    RegisterTermCmd(cmd,Kernel::Instance()->RootConfig());

    _voiceCfgTree = new Cfg_Tree_Group("voice","Module",Kernel::Instance()->RootConfig());
    Kernel::Instance()->RootConfig()->RegisterShowHandler(this);
}

/** VoiceManager's destructor
 */
VoiceManager::~VoiceManager()
{
}

/** Returns the configuration tree of voice manager
 */
Cfg_Tree_Group* VoiceManager::GetConfigTree()
{
    return _voiceCfgTree;
}

/** Find a voice module by name
 */
VoiceModule *VoiceManager::FindModuleByName(const char* name)
{
    ListVoiceModule::iterator it = _listVoiceModule.begin();
    ListVoiceModule::iterator end = _listVoiceModule.end();

    while (it != end) {
	if (strcasecmp(name,(*it)->Name().c_str()) == 0) return *it;
	it++;
    }
    return NULL;
}

/** Terminal command callback
 */
void VoiceManager::DoTermCmd(Term_Session *session, bool no, Term_Cmd *cmd)
{
    switch (cmd->GetId()) {
    case Cmd_Voice:
	{
	    CVariant v;
	    cmd->GetArg((unsigned)0)->GetValue(v);
	    VoiceModule *module = FindModuleByName(v.AsString());

	    if (module) {
		session->SetRoot(module->GetConfigTree());
	    } else {
		session->SendError("There is no such voice module" NEWLINE);
	    }
	}
	break;
    }
}

bool VoiceManager::OnShowConfig(const Cfg_Tree_Entry *entry,String & output, const String & indent, int indent_size) const
{
    String new_indent = indent;
    new_indent.append((size_t)indent_size, ' ');

    VoiceManager *This = (VoiceManager *)this;
    ListVoiceModule::iterator it = This->_listVoiceModule.begin();
    ListVoiceModule::iterator end = This->_listVoiceModule.end();
    while (it != end) {
	VoiceModule *module = *it;
	output += indent + "voice " + module->Name() + NEWLINE;
	module->OnShowConfig (entry, output, new_indent, indent_size);
	output += new_indent + "top" NEWLINE;
	output += NEWLINE;
	it++;
    }
    return OK;
}


/** Create the only instance of this class
 */
VoiceManager *VoiceManager::Create()
{
    if(!_theVoiceManager) {
    assert(_theVoiceManager == NULL);

    _theVoiceManager = new VoiceManager;

    // create the rest of voice driver
#ifdef XGATE_HAS_PROSODY
		ProsodyDriver::Create();
		ProsodyFactory::Create();
#endif


#ifdef XGATE_HAS_DIALOGIC
#ifdef XGATE_HAS_VOICE
    DialogicVoiceDriver::Create();
#endif
#endif

    }
    return Instance();
}

/** Return a pointer to the only instance of this class
 */
VoiceManager *VoiceManager::Instance()
{
    return _theVoiceManager;
}

/** It is called on creation of new instance of VoiceDriver descendent
 */
void VoiceManager::InsertVoiceDriver(VoiceDriver *voxdrv)
{
    _listVoiceDriver.push_back(voxdrv);
}

/** It is called on destruction of instance of VoiceDriver descendent
 */
void VoiceManager::RemoveVoiceDriver(VoiceDriver *voxdrv)
{
    _listVoiceDriver.remove(voxdrv);
}

/** It is called on creation of new instance of VoiceModule descendent
 */
void VoiceManager::InsertVoiceModule(VoiceModule* module)
{
    _listVoiceModule.push_back(module);
}

/** It is called on destruction of instance of VoiceModule descendent
 */
void VoiceManager::RemoveVoiceModule(VoiceModule* module)
{
    _listVoiceModule.remove(module);
}

/** Return the voice device assigned to a particular task
 */
VoiceDevice *VoiceManager::GetTaskVoiceDevice(CTask *task,bool bWarning)
{
    if (task) {
	TaskVoiceBlock *tvb = (TaskVoiceBlock*)task->GetTLSValue(_tlsId);

	if (bWarning && (!tvb || !tvb->_device)) {
	    CLog::Warning(THISMODULE,task,"This task has no voice resource");
	    if (task) task->SetLastError(VErr_NoVoice);
	}
	return tvb ? tvb->_device : NULL;
    } else {
	CLog::Fatal(THISMODULE,"(null) task specified");
	return NULL;
    }
}

/** Return the voice device assigned to a particular task
 */
const char *VoiceManager::GetTaskLanguage(CTask *task,bool bWarning)
{
    TaskVoiceBlock *tvb = (TaskVoiceBlock*)task->GetTLSValue(_tlsId);

    if (bWarning && !tvb) {
	CLog::Warning(THISMODULE,task,"This task has no voice resource");
	if (task) task->SetLastError(VErr_NoVoice);
    }
    return tvb ? tvb->_language.c_str() : "";
}

/** Build the fully specified path based on filename parameter given by user 
 */
void VoiceManager::BuildFullVoiceFileName(char *fullpath,const char *basedir,const char *subdir,const char *lang,const char *filename)
{
   char path_buffer[_MAX_PATH];
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];

   // _splitpath(filename,drive,dir,fname,ext);
    dirname(filename);	//linux porting
    if (strlen(ext) == 0) strcpy(ext,VOXFILEEXT);
    if ((strlen(drive) == 0) && (strlen(dir) == 0)) {
	if (strlen(lang)) sprintf(path_buffer,"%s%s%s/",basedir,subdir,lang);
	else sprintf(path_buffer,"%s%s",basedir,subdir);
    } else {
	sprintf(path_buffer,"%s%s",drive,dir);
    }
    sprintf(fullpath,"%s%s%s",path_buffer,fname,ext);
}

/** This function will be called by the service framework after 
    instantiation of this class, to allow the instance to initialize
    itself.
 */
bool VoiceManager::Load()
{
    CLog::Detail(THISMODULE,"Loading voice driver");

    _tlsId = Kernel::Instance()->AllocTLS();
    _pAPI = new VoiceAPI(VoiceProviderId,this);

     return true;
}

/** This function will be called by the service framework before it is
    destroyed/deleted, to allow the instance to cleanup itself.
 */
bool VoiceManager::Unload()
{
    if (_pAPI) {
	delete _pAPI;
	_pAPI = NULL;
    }

    Kernel::Instance()->FreeTLS(_tlsId);

    return true;
}

/** Make target device switch from source device
 */
void VoiceManager::Listen(SupportSwitching *target,SupportSwitching *source)
{
    SwitchDevice *devTarget = target->GetSwitchDevice();
    SwitchDevice *devSource = source->GetSwitchDevice();

    devTarget->Listen(devSource);
}

/** This function will be called whenever a new task is created.
 */
bool VoiceManager::AttachTask(CTask *task)
{
    //CLog::Detail(THISMODULE,task,"AttachTask");

    TaskVoiceBlock *tvb = new TaskVoiceBlock;
    task->SetTLSValue(_tlsId,(int)tvb);

    // no need for voice
    Call *call = CallManager::Instance()->GetCurrentIncomingCall();
    if (!task->GetApplication()->AppNeedVoice() || (call && !call->GetMedia())) {
	return true;
    }


	Rtp_Media* rtp_med=dynamic_cast<Rtp_Media*>(call->GetMedia());

	if(rtp_med && (rtp_med->GetChannel()->GetBoardType()==RTP_Board_Virtual)){
		return true;
	}


        /* Disabled by HMP code */
	/* if(call->GetMedia() && dynamic_cast<PipeInterface*>(call->GetMedia()->GetInterface()))
		return true;
         */

    VoiceDevice *pdev = NULL;
    for (ListVoiceDriver::iterator it=_listVoiceDriver.begin();it!=_listVoiceDriver.end();it++) {
	pdev = (*it)->AllocVoiceDevice();
	if (pdev) break;
    }

    if (!pdev) {
	CLog::Warning(THISMODULE,task,"Can not allocate voice device");
	return false;
    }

    ResetConfig(pdev);

    // Switch automatically between the two devices
    if (call) {
	Media *cmedia = call->GetMedia();
	Listen(cmedia,pdev);
	Listen(pdev,cmedia);
    }

    CLog::Detail(THISMODULE,task,"%s allocated",pdev->GetName());

    pdev->AttachTask(task);
    tvb->_device = pdev;

    return true;
}

static void sig_hdl (int sig, siginfo_t *siginfo, void *context)
{
    sleep(5);
    CLog::Detail(THISMODULE, "Crash on Detach Task %d", sig);
    longjmp(ex_buf__, 1);
}

static int handleDev(VoiceDevice *dev)
{
    memset (&act, '\0', sizeof(act));

    //printf("\n Inside Handle Device \n");

    /* Use the sa_sigaction field because the handles has two additional parameters */
    act.sa_sigaction = &sig_hdl;

    /* The SA_SIGINFO flag tells sigaction() to use the sa_sigaction field, not sa_handler. */
    //act.sa_flags = SA_SIGINFO;
    act.sa_flags = SA_NODEFER ;

    if (sigaction(SIGSEGV, &act, NULL) < 0)
    {
        perror ("sigaction");
        return 1;
    }

    if(!setjmp(ex_buf__))
    {
        //printf("\n Cancelling the JOB... \n");
        dev->CancelJob();
    }
}

/** This function will be called whenever a task is about to be deleted.
 */
void VoiceManager::DetachTask(CTask *task)
{
    //**CLog::Detail(THISMODULE,task,"DetachTask");
    // cancel the current job if any

    VoiceDevice *dev = GetTaskVoiceDevice(task,false);

    CLog::Detail(THISMODULE, "Detach Task %02x - %02x[dev]", task, dev);

    if (dev) {
        dev->CancelJob();
        STATUS rc = Free(task);
    }
    TaskVoiceBlock *tvb = (TaskVoiceBlock *)task->GetTLSValue(_tlsId);
    if (tvb) {
	if (tvb->_mixer) {
#ifdef XGATE_HAS_PROSODY
	    ProsodyDriver::Instance()->FreeMixer(tvb->_mixer);
#endif // XGATE_HAS_PROSODY
	    tvb->_mixer = 0;
	}

	task->SetTLSValue(_tlsId,0);
	delete tvb;
	tvb = NULL;
    }

    List<ConferenceRoom*>::iterator it;
    for (it = _conf_rooms.begin(); it != _conf_rooms.end(); ) {
	if ((*it)->DetachTask(task)) {
	    CLog::Detail(THISMODULE, "Deleting conference room %s", (*it)->_name.c_str());
	    delete (*it);
	    _conf_rooms.erase(it++);
	}
	else
	    it++;

    }

    List<EchoCanceller*>::iterator ecr_it;
    for (ecr_it = _ecr_devices.begin(); ecr_it != _ecr_devices.end(); ) {
	if ((*ecr_it)->DetachTask(task)) {
	    delete (*ecr_it);
	    _ecr_devices.erase(ecr_it++);
	}
	else
	    ecr_it++;

    }

}


//-------------------------------------------------------------------
STATUS VoiceManager::Alloc(CTask *task)
{
    //***CLog::Detail(THISMODULE,task,"VoxAlloc");

    // if task has already owned a voice resource
    VoiceDevice *pdev = GetTaskVoiceDevice(task,false);
    if (pdev) {
	//CLog::Warning(THISMODULE,task,"This task already has voice resource");
	//task->SetLastError(VErr_AlreadyAllocated);
	return OK;
    }

    // allocate one voice resource
    for (ListVoiceDriver::iterator it=_listVoiceDriver.begin();it!=_listVoiceDriver.end();it++) {
	pdev = (*it)->AllocVoiceDevice();
	if (pdev) break;
    }

    if (!pdev) {
	CLog::Warning(THISMODULE,task,"No voice device available");
	task->SetLastError(VErr_VoiceUnavailable);
	return OK;
    }

    ResetConfig(pdev);

    CLog::Detail(THISMODULE,task,"%s allocated",pdev->GetName());
    pdev->AttachTask(task);
    ((TaskVoiceBlock*)task->GetTLSValue(_tlsId))->_device = pdev;

    task->SetLastError(VErr_NoError);

    return OK;
}


//-------------------------------------------------------------------
STATUS VoiceManager::Free(CTask *task)
{
    //**CLog::Detail(THISMODULE,task,"VoxFree");

    // if task has already owned a voice resource
    VoiceDevice *pdev = GetTaskVoiceDevice(task,false);
    if (!pdev) return OK;

    //***CLog::Detail(THISMODULE,task,"%s detached",pdev->GetName());
    pdev->GetSwitchDevice()->DisconnectListeners();
    pdev->GetSwitchDevice()->Unlisten();
    pdev->DetachTask();
    pdev->Free();
    ((TaskVoiceBlock*)task->GetTLSValue(_tlsId))->_device = NULL;
    task->SetLastError(VErr_NoError);
    return OK;
}


//-------------------------------------------------------------------
void VoiceManager::ResetConfig(VoiceDevice *pdev)
{
    // set default values for this device
    pdev->EnableInterrupt();
    pdev->SetDigitTimeout(_cfg_maxGetDigitTime);
    pdev->SetInterDigitTimeout(_cfg_maxInterDigitTime);
    pdev->SetMaxRecTime(_cfg_maxRecordTime);
    pdev->SetTermDigits(_cfg_terminalDigits);
    pdev->SetVoiceFormat(_cfg_voxFileFormat);
#ifndef __linux__
    pdev->SetRecordMaxSilence(_cfg_recordMaxSilence);
#endif
}


//-------------------------------------------------------------------
bool VoiceManager::CancelJob(CTask *task)
{
    //CLog::Detail(THISMODULE,task,"Cancelling current job");

    VoiceDevice *pdev = GetTaskVoiceDevice(task);
    if (pdev) {
	//***CLog::Detail(THISMODULE,task,"Cancelling current job on %s for task %u",pdev->GetName(),task->GetPID());
	return pdev->CancelJob();
    } else {
	CLog::Fatal(THISMODULE,task,"Unable to cancel job on inexistant device for task %u",task->GetPID());
	return false;
    }
}


//-------------------------------------------------------------------
STATUS VoiceManager::SuspendTask(VoiceDevice *pdev)
{
    CTask *task = pdev->GetTask();

    assert(task != NULL);
    //***CLog::Detail(THISMODULE,task,"Suspending task");
    return task->Sleep(this);
}


//-------------------------------------------------------------------
STATUS VoiceManager::ResumeTask(VoiceDevice *pdev)
{
    CTask *task = pdev->GetTask();

    if (task) {
	//***CLog::Detail(THISMODULE,task,"Resuming task");
	return task->Wakeup();
    }
    return OK;
}


//-------------------------------------------------------------------
void VoiceManager::SetTaskLastError(VoiceDevice *pdev,int error,int exterr,int severity)
{
    CTask *task = pdev->GetTask();
    if (task) {
	if (error != VErr_NoError) {
	    //***CLog::Detail(THISMODULE,task,"LastTaskError: error=%d exterr=%d devname=%s",error,exterr,pdev->GetName());
	}
	task->SetLastError(error,exterr,severity);
    }
}


//-------------------------------------------------------------------
// API call: play from a file
STATUS VoiceManager::PlayFile(CTask *task,char *filename,bool wav_mode)
{
    CLog::Detail(THISMODULE,task,"VoxPlayFile: filename=%s",filename);

    VoiceDevice *pdev = GetTaskVoiceDevice(task);
    if (pdev) {
	if (!pdev->IsDeviceIdle()) {
	    CLog::Warning(THISMODULE,task,"Voice channel is busy at this moment");
	    task->SetLastError(VErr_VoiceBusy);
	    return OK;
	}

	if (wav_mode && !pdev->SupportPlayWav()) {
	    CLog::Warning(THISMODULE,task,"%s does not support play WAV file", pdev->GetName());
	    task->SetLastError(VErr_InvalidOperation);
	    return OK;
	}

	char fullpath[_MAX_PATH];
	char *fptr = fullpath;
#ifndef __linux__
	BuildFullVoiceFileName(fullpath,task->GetAppDirectory(),"prompts/",GetTaskLanguage(task),filename);
#else
	fptr = filename;
#endif

        	
	if (wav_mode) return pdev->PlayFileArrayWav(1,(const char**)&fptr,false,false,true);
	else return pdev->PlayFileArray(1,(const char**)&fptr,false,false);
    } else return OK;
}


//-------------------------------------------------------------------
// API call: play from a file (background)
STATUS VoiceManager::PlayFileBkgnd(CTask *task,char *filename,int repeat_flag)
{
    CLog::Detail(THISMODULE,task,"VoxPlayFileBkgnd: filename=%s repeat=%d",filename,repeat_flag);

    VoiceDevice *pdev = GetTaskVoiceDevice(task);
    if (pdev) {
	if (!pdev->IsDeviceIdle()) {
	    CLog::Warning(THISMODULE,task,"Voice channel is busy at this moment");
	    task->SetLastError(VErr_VoiceBusy);
	    return OK;
	}
	char fullpath[_MAX_PATH];
	char *fptr = fullpath;
#ifndef __linux__
	BuildFullVoiceFileName(fullpath,task->GetAppDirectory(),"prompts/",GetTaskLanguage(task),filename);
#else
	fptr = filename;
#endif
	return pdev->PlayFileArray(1,(const char**)&fptr,true,(repeat_flag != 0));
    } else return OK;
}


//-------------------------------------------------------------------
// API call: stop background play
STATUS VoiceManager::StopPlayFileBkgnd(CTask *task,int waiteof_flag)
{
    CLog::Detail(THISMODULE,task,"VoxStopPlayFileBkgnd: repeat=%d",waiteof_flag);

    VoiceDevice *pdev = GetTaskVoiceDevice(task);
    if (pdev) return pdev->StopPlayFileBkGnd(waiteof_flag != 0);
    else return OK;
}


//-------------------------------------------------------------------
// API call: record to a file
STATUS VoiceManager::RecordFile(CTask *task,char *filename,bool wav_mode)
{
    CLog::Detail(THISMODULE,task,"VoxRecordFile: filename=%s",filename);

    VoiceDevice *pdev = GetTaskVoiceDevice(task);
    if (pdev) {
	if (!pdev->IsDeviceIdle()) {
	    CLog::Warning(THISMODULE,task,"Voice channel is busy at this moment");
	    task->SetLastError(VErr_VoiceBusy);
	    return OK;
	}

	if (wav_mode && !pdev->SupportRecordWav()) {
	    CLog::Warning(THISMODULE,task,"%s does not support record WAV file", pdev->GetName());
	    task->SetLastError(VErr_InvalidOperation);
	    return OK;
	}

	char fullpath[_MAX_PATH];
#ifndef __linux__
	BuildFullVoiceFileName(fullpath,task->GetAppDirectory(),"prompts/",GetTaskLanguage(task),filename);
#else
	if (wav_mode) return pdev->RecordFileWav(filename,true);
        else return pdev->RecordFile(filename);
#endif
	if (wav_mode) return pdev->RecordFileWav(fullpath,true);
	else return pdev->RecordFile(fullpath);
    } else return OK;
}

// API to End RecordFile : T REC
STATUS VoiceManager::EndRecordFile_t(CTask *task)
{
  CLog::Detail(THISMODULE,task,"VoxEndRecordFile ");
  VoiceDevice *pdev = GetTaskVoiceDevice(task);
  if (pdev) {
     pdev->EndRecordFile_t();
     return OK;
    } else {
      CLog::Detail(THISMODULE,task,"VoxEndRecordFile: ELSE pdev is NULL.. ");
      return OK;
  }
}

//-------------------------------------------------------------------
// API call: play a single/dual tone
STATUS VoiceManager::PlayTone(CTask *task,int freq1,int db1,int freq2,int db2,int duration)
{
    CLog::Detail(THISMODULE,task,"VoxPlayTone: freq1=%d db1=%d freq2=%d db2=%d duration=%d",freq1,db1,freq2,db2,duration);

    VoiceDevice *pdev = GetTaskVoiceDevice(task);
    if (pdev) {
	if (!pdev->IsDeviceIdle()) {
	    CLog::Warning(THISMODULE,task,"Voice channel is busy at this moment");
	    task->SetLastError(VErr_VoiceBusy);
	    return OK;
	}
	return pdev->PlayTone(freq1,db1,freq2,db2,duration);
    } else return OK;
}


//-------------------------------------------------------------------
// API call: play a series of dtmf
STATUS VoiceManager::PlayDtmf(CTask *task,char *dtmf)
{
    CLog::Detail(THISMODULE,task,"VoxPlayDtmf: dtmf=%s",dtmf);

    VoiceDevice *pdev = GetTaskVoiceDevice(task);
    if (pdev) {
	if (!pdev->IsDeviceIdle()) {
	    CLog::Warning(THISMODULE,task,"Voice channel is busy at this moment");
	    task->SetLastError(VErr_VoiceBusy);
	    return OK;
	}
	return pdev->PlayDtmf(dtmf);
    } else return OK;
}


//-------------------------------------------------------------------
// API call: receive dtmf input from user
STATUS VoiceManager::GetDigits(CTask *task,int *retval,int maxlength,char *buffer)
{
    CLog::Detail(THISMODULE,task,"VoxGetDigits: maxlength=%d",maxlength);
    *retval = 1;
    VoiceDevice *pdev = GetTaskVoiceDevice(task);
    if (pdev) {
	if (!pdev->IsDeviceIdle()) {
            *retval = 0;
	    CLog::Warning(THISMODULE,task,"Voice channel is busy at this moment");
	    task->SetLastError(VErr_VoiceBusy);
	    return OK;
	}
        *retval = 1;
	return pdev->GetDigits(maxlength,buffer);
    } else {
        *retval = 1;
        return OK;
    }
}

//-------------------------------------------------------------------
// API call: wait until silence is detected or timeout
STATUS VoiceManager::WaitForSilence(CTask *task, int max_wait_secs, int* remaining_time)
{
    //***CLog::Detail(THISMODULE,task,"WaitForSilence: max_wait_secs=%d",max_wait_secs);

    VoiceDevice *pdev = GetTaskVoiceDevice(task);
    if (pdev) {
	if (!pdev->IsDeviceIdle()) {
	    CLog::Warning(THISMODULE,task,"Voice channel is busy at this moment");
	    task->SetLastError(VErr_VoiceBusy);
	    return OK;
	}
	return pdev->WaitForSilence(max_wait_secs, remaining_time);
    } else return OK;
}

//-------------------------------------------------------------------
// API call: read the last terminator digit if any
STATUS VoiceManager::GetLastTermDigit(CTask *task,char *digit)
{
    CLog::Detail(THISMODULE,task,"VoxGetLastTermDigit");

    VoiceDevice *pdev = GetTaskVoiceDevice(task);
    if (pdev) return pdev->GetLastTermDigit(digit);
    else return OK;
}


//-------------------------------------------------------------------
// API call: clear dtmf buffer digits
STATUS VoiceManager::ClearDigits(CTask *task)
{
    CLog::Detail(THISMODULE,task,"VoxClearDigits");

    VoiceDevice *pdev = GetTaskVoiceDevice(task);
    if (pdev) return pdev->ClearDigits();
    else return OK;
}


//-------------------------------------------------------------------
// API call: peek one dtmf without removing from the queue
STATUS VoiceManager::PeekDigit(CTask *task,char *buffer)
{
    CLog::Detail(THISMODULE,task,"VoxPeekDigits");

    VoiceDevice *pdev = GetTaskVoiceDevice(task);
    *buffer = '\0';
    if (pdev) return pdev->PeekDigit(buffer);
    else return OK;
}


//-------------------------------------------------------------------
// API call: get device name
STATUS VoiceManager::GetDevName(CTask *task,char *devname)
{
    CLog::Detail(THISMODULE,task,"VoxGetDevName");

    VoiceDevice *pdev = GetTaskVoiceDevice(task);
    *devname = '\0';
    if (pdev) safe_strcpy(devname,pdev->GetName(),MAXAPPSTRLEN);
    return OK;
}

// API call: get host_info
STATUS VoiceManager::GetHostInfo(CTask *task, int *retval, char *conf_id)
{
   if (host_info[conf_id] == 1)
   {
      *retval = 1;
   } else {
      *retval = 0;
   }
}

// API call: set host_info
STATUS VoiceManager::SetHostInfo(CTask *task, char *conf_id, int status)
{
   host_info[conf_id] = status ;
}

//-------------------------------------------------------------------
// API call: get/retrieve voice parameter
STATUS VoiceManager::SetVoiceFormat(CTask *task,VoiceFormat format)
{
    CLog::Detail(THISMODULE,task,"VoxSetVoiceFormat: format=%d",format);

    VoiceDevice *pdev = GetTaskVoiceDevice(task);
    if (pdev) return pdev->SetVoiceFormat(format);
    else return OK;
}


//-------------------------------------------------------------------
// API call: get/retrieve voice parameter
STATUS VoiceManager::GetVoiceFormat(CTask *task,VoiceFormat *format)
{
    CLog::Detail(THISMODULE,task,"VoxGetVoiceFormat");

    VoiceDevice *pdev = GetTaskVoiceDevice(task);
    if (pdev) return pdev->GetVoiceFormat(format);
    else return OK;
}


//-------------------------------------------------------------------
// API call: get/retrieve voice parameter
STATUS VoiceManager::EnableInterrupt(CTask *task)
{
    CLog::Detail(THISMODULE,task,"VoxEnableInterrupt");

    VoiceDevice *pdev = GetTaskVoiceDevice(task);
    if (pdev) return pdev->EnableInterrupt();
    else return OK;
}


//-------------------------------------------------------------------
// API call: get/retrieve voice parameter
STATUS VoiceManager::DisableInterrupt(CTask *task)
{
    CLog::Detail(THISMODULE,task,"VoxDisableInterrupt");

    VoiceDevice *pdev = GetTaskVoiceDevice(task);
    if (pdev) return pdev->DisableInterrupt();
    else return OK;
}


//-------------------------------------------------------------------
// API call: get/retrieve voice parameter
STATUS VoiceManager::SetMaxRecTime(CTask *task,int maxrectime)
{
    CLog::Detail(THISMODULE,task,"VoxSetMaxRecTime: time=%d",maxrectime);

    VoiceDevice *pdev = GetTaskVoiceDevice(task);
    if (pdev) return pdev->SetMaxRecTime(maxrectime);
    else return OK;
}


//-------------------------------------------------------------------
// API call: get/retrieve voice parameter
STATUS VoiceManager::GetMaxRecTime(CTask *task,int *maxrectime)
{
    CLog::Detail(THISMODULE,task,"VoxGetMaxRecTime");

    VoiceDevice *pdev = GetTaskVoiceDevice(task);
    if (pdev) return pdev->GetMaxRecTime(maxrectime);
    else return OK;
}


//-------------------------------------------------------------------
// API call: get/retrieve voice parameter
STATUS VoiceManager::SetInterDigitTimeout(CTask *task,int maxidtime)
{
    CLog::Detail(THISMODULE,task,"VoxSetInterDigitTimeout: timeout=%d",maxidtime);

    VoiceDevice *pdev = GetTaskVoiceDevice(task);
    if (pdev) return pdev->SetInterDigitTimeout(maxidtime);
    else return OK;
}


//-------------------------------------------------------------------
// API call: get/retrieve voice parameter
STATUS VoiceManager::GetInterDigitTimeout(CTask *task,int *maxidtime)
{
   CLog::Detail(THISMODULE,task,"VoxGetInterDigitTimeout: timeout=%d",maxidtime);

    VoiceDevice *pdev = GetTaskVoiceDevice(task);
    if (pdev) return pdev->GetInterDigitTimeout(maxidtime);
    else return OK;
}


//-------------------------------------------------------------------
// API call: get/retrieve voice parameter
STATUS VoiceManager::SetDigitTimeout(CTask *task,int maxtime)
{
    CLog::Detail(THISMODULE,task,"VoxSetDigitTimeout: timeout=%d",maxtime);

    VoiceDevice *pdev = GetTaskVoiceDevice(task);
    if (pdev) return pdev->SetDigitTimeout(maxtime);
    else return OK;
}


//-------------------------------------------------------------------
// API call: get/retrieve voice parameter
STATUS VoiceManager::GetDigitTimeout(CTask *task,int *maxtime)
{
    CLog::Detail(THISMODULE,task,"VoxGetDigitTimeout: timeout=%d",maxtime);

    VoiceDevice *pdev = GetTaskVoiceDevice(task);
    if (pdev) return pdev->GetDigitTimeout(maxtime);
    else return OK;
}


//-------------------------------------------------------------------
// API call: get/retrieve voice parameter
STATUS VoiceManager::SetTermDigits(CTask *task,char *termdigs)
{
    CLog::Detail(THISMODULE,task,"VoxSetTermDigits: digits=%s",termdigs);

    VoiceDevice *pdev = GetTaskVoiceDevice(task);
    if (pdev) return pdev->SetTermDigits(termdigs);
    else return OK;
}


//-------------------------------------------------------------------
// API call: get/retrieve voice parameter
STATUS VoiceManager::GetTermDigits(CTask *task,char *termdigs)
{
    CLog::Detail(THISMODULE,task,"VoxGetTermDigits");

    VoiceDevice *pdev = GetTaskVoiceDevice(task);
    if (pdev) return pdev->GetTermDigits(termdigs);
    else return OK;
}

//-------------------------------------------------------------------
// API call: get/retrieve voice parameter
STATUS VoiceManager::SetRecordMaxSilence(CTask *task,int maxsil)
{
    VoiceDevice *pdev = GetTaskVoiceDevice(task);
#ifndef __linux__
    if (pdev) return pdev->SetRecordMaxSilence(maxsil);
    else return OK;
#endif
    /* Narsim: return OK by default */
    return OK;
}


//-------------------------------------------------------------------
// API call: get/retrieve voice parameter
STATUS VoiceManager::GetRecordMaxSilence(CTask *task,int *maxsil)
{
    VoiceDevice *pdev = GetTaskVoiceDevice(task);
#ifndef __linux__
    if (pdev) return pdev->GetRecordMaxSilence(maxsil);
    else return OK;
#endif
    /* Narsim: OK by default */
    return OK;
}


//-------------------------------------------------------------------
// API call: get/retrieve voice parameter
STATUS VoiceManager::SetVolume(CTask *task, int dB)
{
    VoiceDevice *pdev = GetTaskVoiceDevice(task);
    if (pdev) return pdev->SetOutputVolume(dB);
    else return OK;
}


//-------------------------------------------------------------------
// API call: get/retrieve voice parameter
STATUS VoiceManager::GetVolume(CTask *task, int *dB)
{
    VoiceDevice *pdev = GetTaskVoiceDevice(task);
    if (pdev) return pdev->GetOutputVolume(dB);
    else return OK;
}


//-------------------------------------------------------------------
// API call: get/retrieve voice parameter
STATUS VoiceManager::SetAGC(CTask *task, int agc)
{
    VoiceDevice *pdev = GetTaskVoiceDevice(task);
    if (pdev) return pdev->SetOutputAGC(agc);
    else return OK;
}


//-------------------------------------------------------------------
// API call: get/retrieve voice parameter
STATUS VoiceManager::GetAGC(CTask *task, int *agc)
{
    VoiceDevice *pdev = GetTaskVoiceDevice(task);
    if (pdev) return pdev->GetOutputAGC(agc);
    else return OK;
}


//-------------------------------------------------------------------
// API call: set language
STATUS VoiceManager::SetLanguage(CTask *task,const char* langID)
{
    TaskVoiceBlock *tvb = (TaskVoiceBlock*)task->GetTLSValue(_tlsId);
    if (tvb) tvb->_language = langID;
    return OK;
}


//-------------------------------------------------------------------
// API call: vocabulary api
STATUS VoiceManager::SpellNumber(CTask *task,int number)
{
    CLog::Detail(THISMODULE,task,"VoxSpellNumber: number=%d",number);
    if (number < 0) {
	CLog::Warning(THISMODULE, task, "Can't spell negative number (%d)",number);
	return OK;
    }

    VoiceDevice *pdev = GetTaskVoiceDevice(task);
    if (pdev) {
	unsigned i;
	Vocabulary vocab(number);
	char filelist[VOICE_MAXVOCAB][_MAX_PATH];
	const char *fileptr[VOICE_MAXVOCAB];

	for (i=0;i<vocab.GetNumOfElts();i++) {
	    BuildFullVoiceFileName(filelist[i],Kernel::Instance()->GetBaseDirectory().c_str(),"vocab/",GetTaskLanguage(task),vocab.GetElement(i));
	    fileptr[i] = filelist[i];
	}

	return pdev->PlayFileArray(i,fileptr,false,false);
    } else return OK;
}


//-------------------------------------------------------------------
// API call: get voice media
STATUS VoiceManager::GetMedia(CTask *task,VoiceDevice **device)
{
    //***CLog::Detail(THISMODULE,task,"GetMedia");

    *device = GetTaskVoiceDevice(task);
    return OK;
}

//-------------------------------------------------------------------
// API call: create conference room
STATUS VoiceManager::ConfCreateRoom(CTask *task, int *room_handle, char *room_name, int room_size)
{


#if 0
    //***CLog::Detail(THISMODULE,task,"ConfCreateRoom");

    List<ConferenceRoom*>::iterator rooms_iterator;
    ConferenceRoom* conf_room = NULL;


    // check for existing conference room
    *room_handle = -1;
    for (rooms_iterator = _conf_rooms.begin(); rooms_iterator != _conf_rooms.end(); rooms_iterator++) {
	conf_room = *rooms_iterator;
	if (strcmp(conf_room->_name.c_str(), room_name) == 0) {
	    *room_handle = conf_room->GetHandle();
	    break;
	}
    }

    if (*room_handle != -1) {
	CLog::Debug(THISMODULE, "ConfCreateRoom: room %s is already created, returning handle to that room", room_name);
	return OK;
    }

    // allocate conference room
    conf_room = NULL;
    for (ListVoiceDriver::iterator it=_listVoiceDriver.begin();it!=_listVoiceDriver.end();it++) {
	conf_room = (*it)->AllocConferenceRoom(room_size);
	if (conf_room) break;
    }

    if (conf_room) {
	conf_room->_name = room_name;
	conf_room->SetTaskOwner(task);
	*room_handle = conf_room->GetHandle();
	_conf_rooms.push_back(conf_room);
	CLog::Detail(THISMODULE, "Room %s allocated", room_name);
    }
    else {
        *room_handle = -1;
    }
#endif 

	cout << "**** Inside Voicemanager.cpp, ConfCreateRoom **** " << endl;
    
    return OK;
}

//-------------------------------------------------------------------
// API call: delete conference room
//STATUS VoiceManager::ConfDeleteRoom(CTask *task, int room_handle)
STATUS VoiceManager::ConfDeleteRoom(CTask *task, int room_handle, char *conf_id) // T 1710
{
    //***CLog::Detail(THISMODULE,task,"ConfCreateRoom");

    List<ConferenceRoom*>::iterator it;
    ConferenceRoom* conf_room = NULL;

    if (room_handle == -1) {
	CLog::Warning(THISMODULE, "ConfDeleteRoom: invalid room handle");
	return OK;
    }    

    // allocate conference room
    for (it=_conf_rooms.begin();it!=_conf_rooms.end();it++) {
	if (room_handle == (*it)->_handle) {
            host_info.erase(conf_id); // T 1710
	    conf_room = *it;
	    _conf_rooms.erase(it);
	    CLog::Debug(THISMODULE, "Deleting conference room %s", conf_room->_name.c_str());
	    delete conf_room;
	    break;
	}
    }

    if (!conf_room) {
	CLog::Warning(THISMODULE, "ConfDeleteRoom: cannot find conference room with handle %d", room_handle);
    }    

    return OK;
}

//-------------------------------------------------------------------
// API call: find conference room
STATUS VoiceManager::ConfFindRoom(CTask *task, int *room_handle, char *room_name) {

    List<ConferenceRoom*>::iterator it;
    ConferenceRoom* conf_room;

    *room_handle = -1;
    for (it = _conf_rooms.begin(); it != _conf_rooms.end(); it++) {
	conf_room = *it;
	if (strcmp(conf_room->_name.c_str(), room_name) == 0) {
	    *room_handle = conf_room->GetHandle();
	    break;
	}
    }

    if (*room_handle == -1) {
	CLog::Warning(THISMODULE, "ConfFindRoom: cannot find conference room %s", room_name);
    }    

    return OK;
}

//-------------------------------------------------------------------
// API call: get conference device
STATUS VoiceManager::ConfOpenMedia(CTask *task, VoiceDevice **device, int room_handle) {
    //***CLog::Detail(THISMODULE,task,"ConfOpenMedia");

    List<ConferenceRoom*>::iterator it;
    ConferenceRoom* conf_room;

    if (room_handle == -1) {
	CLog::Warning(THISMODULE, "ConfDeleteRoom: invalid room handle");
	return OK;
    }    

    for (it = _conf_rooms.begin(); it != _conf_rooms.end(); it++) {
	conf_room = *it;
	if (room_handle == conf_room->_handle) {
	    if ((*device = conf_room->GetDevice()) != NULL) {
		(*device)->AttachTask(task);
		return OK;
	    }
	    CLog::Warning(THISMODULE, "VoiceManager: cannot open conference resource");
	    return OK;
	}  
    }

    CLog::Warning(THISMODULE, "VoiceManager: cannot find conference room with handle %d", room_handle);
    return OK;
}

//-------------------------------------------------------------------
// API call: release conference device
STATUS VoiceManager::ConfCloseMedia(CTask *task, int room_handle, VoiceDevice *device)
{
    //***CLog::Detail(THISMODULE,task,"ConfCloseMedia");

    List<ConferenceRoom*>::iterator it;
    ConferenceRoom* conf_room;

    if (device == NULL) {
	CLog::Warning(THISMODULE, "ConfDeleteRoom: invalid conference media");
	return OK;
    }
    
    if (room_handle == -1) {
	CLog::Warning(THISMODULE, "ConfDeleteRoom: invalid room handle");
	return OK;
    }    

    for (it = _conf_rooms.begin(); it != _conf_rooms.end(); it++) {
	conf_room = *it;
	if (room_handle == conf_room->_handle) {
	    if (conf_room->FreeDevice(device) == OK) {
		return OK;
	    }
	    CLog::Warning(THISMODULE, "VoiceManager: cannot close conference resource");
	    return OK;
	}  
    }

    CLog::Warning(THISMODULE, "VoiceManager: cannot find conference room with handle %d", room_handle);
    return OK;
}

//-------------------------------------------------------------------
// API call: add party to conference
STATUS VoiceManager::ConfEnableMedia(CTask* task, int room_handle, VoiceDevice *device)
{
    //***CLog::Detail(THISMODULE,task,"ConfAddParty");

    List<ConferenceRoom*>::iterator it;
    ConferenceRoom* conf_room;

    if (device == NULL) {
	CLog::Warning(THISMODULE, "ConfDeleteRoom: invalid conference media");
	return OK;
    }
    
    if (room_handle == -1) {
	CLog::Warning(THISMODULE, "ConfDeleteRoom: invalid room handle");
	return OK;
    }    

    for (it = _conf_rooms.begin(); it != _conf_rooms.end(); it++) {
	conf_room = *it;
	if (room_handle == conf_room->_handle) {
	    conf_room->AddParty(device); 
	    return OK;
	}  
    }

    CLog::Warning(THISMODULE, "VoiceManager: cannot find conference room with handle = %d", room_handle);
    return OK;
}

//-------------------------------------------------------------------
// API call: remove party from conference
STATUS VoiceManager::ConfDisableMedia(CTask* task, int room_handle, VoiceDevice *device)
{
    //***CLog::Detail(THISMODULE,task,"ConfRemoveParty");

    List<ConferenceRoom*>::iterator it;
    ConferenceRoom* conf_room;

    if (device == NULL) {
	CLog::Warning(THISMODULE, "ConfDeleteRoom: invalid conference media");
	return OK;
    }
    
    if (room_handle == -1) {
	CLog::Warning(THISMODULE, "ConfDeleteRoom: invalid room handle");
	return OK;
    }    

    for (it = _conf_rooms.begin(); it != _conf_rooms.end(); it++) {
	conf_room = *it;
	if (room_handle == conf_room->_handle) {
	    conf_room->RemoveParty(device); 
	    return OK;
	}  
    }

    
    return OK;
}


//-------------------------------------------------------------------
// API call: set conference participant input volume and AGC
STATUS VoiceManager::ConfSetMediaInputVolAGC(CTask* task, VoiceDevice *device, int dB, int agc)
{
    bool valid = false;
    try {
	valid = device->GetSwitchDevice()->IsValid();
    } catch (...) {
	valid = false;
    }

    if (!valid) {
	CLog::Warning(THISMODULE, "ConfSetMediaInputVolAGC: invalid conference media");
	return OK;
    }

    device->SetInputVolume(dB);
    device->SetInputAGC(agc);
    
    return OK;
}

//-------------------------------------------------------------------
// API call: set conference participant output volume and AGC
STATUS VoiceManager::ConfSetMediaOutputVolAGC(CTask* task, VoiceDevice *device, int dB, int agc)
{
    bool valid = false;
    try {
	valid = device->GetSwitchDevice()->IsValid();
    } catch (...) {
	valid = false;
    }

    if (!valid) {
	CLog::Warning(THISMODULE, "ConfSetMediaOutputVolAGC: invalid conference media");
	return OK;
    }

    device->SetOutputVolume(dB);
    device->SetOutputAGC(agc);
    
    return OK;
}

//-------------------------------------------------------------------
// API call: create echo canceller
STATUS VoiceManager::EcrAlloc(CTask* task, EchoCanceller** ecr_handle) {
    //***CLog::Detail(THISMODULE,task,"EcrAlloc");

    // allocate one echo canceller device
    *ecr_handle = NULL;
    for (ListVoiceDriver::iterator it=_listVoiceDriver.begin();it!=_listVoiceDriver.end();it++) {
	*ecr_handle = (*it)->AllocEchoCanceller();
	if (*ecr_handle) break;
    }

    if (!(*ecr_handle)) {
	CLog::Warning(THISMODULE, task, "No echo canceller device available");
	task->SetLastError(VErr_VoiceUnavailable);
	return OK;
    }

//    CLog::Detail(THISMODULE,task,"%s allocated",handle->GetName());

    _ecr_devices.push_back(*ecr_handle);
    (*ecr_handle)->AttachTask(task);
    task->SetLastError(VErr_NoError);

    return OK;
}

STATUS VoiceManager::EcrFree(CTask* task, EchoCanceller* ecr_handle) {
    List<EchoCanceller*>::iterator it;
    if (!ecr_handle) {
	CLog::Warning(THISMODULE, task, "EcrFree: invalid (null) device");
	return OK;
    }
    for (it = _ecr_devices.begin(); it != _ecr_devices.end(); it++) {
	if (*it == ecr_handle) {
	    delete ecr_handle;
	    break;
	}
    }
    if (it == _ecr_devices.end()) {
	CLog::Warning(THISMODULE, task, "Invalid echocanceller handle");
    }
    else {
	_ecr_devices.erase(it);
    }
    return OK;
}

STATUS VoiceManager::EcrGetMediaPrimary(CTask* task, VoiceDevice** device, EchoCanceller* ecr_handle) {
    List<EchoCanceller*>::iterator it;
    if (!ecr_handle) {
	CLog::Warning(THISMODULE, task, "EcrGetMediaPrimary: invalid (null) echocanceller handle");
	return OK;
    }
    for (it = _ecr_devices.begin(); it != _ecr_devices.end(); it++) {
	if (ecr_handle == *it) {
	    *device = ecr_handle->GetPrimaryChannel();
	    return OK;
	}
    }
    if (it == _ecr_devices.end())
	CLog::Warning(THISMODULE, task, "Invalid echocanceller handle");
    return OK;
}

STATUS VoiceManager::EcrGetMediaReference(CTask* task, VoiceDevice** device, EchoCanceller* ecr_handle) {
    List<EchoCanceller*>::iterator it;
    if (!ecr_handle) {
	CLog::Warning(THISMODULE, task, "EcrGetMediaReference: invalid (null) echocanceller handle");
	return OK;
    }
    for (it = _ecr_devices.begin(); it != _ecr_devices.end(); it++) {
	if (ecr_handle == *it) {
	    *device = ecr_handle->GetReferenceChannel();
	    return OK;
	}
    }
    if (it == _ecr_devices.end())
	CLog::Warning(THISMODULE, task, "Invalid echocanceller handle");
    return OK;
}

STATUS VoiceManager::EcrStartCancelling(CTask* task, EchoCanceller* ecr_handle) {
    List<EchoCanceller*>::iterator it;
    if (!ecr_handle) {
	CLog::Warning(THISMODULE, task, "EcrStartCancelling: invalid (null) echocanceller handle");
	return OK;
    }
    for (it = _ecr_devices.begin(); it != _ecr_devices.end(); it++) {
	if (ecr_handle == *it) {
	    ecr_handle->StartCancelling();
	    return OK;
	}
    }
    if (it == _ecr_devices.end())
	CLog::Warning(THISMODULE, task, "Invalid echocanceller handle");
    return OK;
}

STATUS VoiceManager::EcrReinitCancelling(CTask* task, EchoCanceller* ecr_handle) {
    List<EchoCanceller*>::iterator it;
    if (!ecr_handle) {
	CLog::Warning(THISMODULE, task, "EcrReinitCancelling: invalid (null) echocanceller handle");
	return OK;
    }
    for (it = _ecr_devices.begin(); it != _ecr_devices.end(); it++) {
	if (ecr_handle == *it) {
	    ecr_handle->ReinitCancelling();
	    return OK;
	}
    }
    if (it == _ecr_devices.end())
	CLog::Warning(THISMODULE, task, "Invalid echocanceller handle");
    return OK;
}

STATUS VoiceManager::EcrStopCancelling(CTask* task, EchoCanceller* ecr_handle) {
    List<EchoCanceller*>::iterator it;
    if (!ecr_handle) {
	CLog::Warning(THISMODULE, task, "EcrStopCancelling: invalid (null) echocanceller handle");
	return OK;
    }
    for (it = _ecr_devices.begin(); it != _ecr_devices.end(); it++) {
	if (ecr_handle == *it) {
	    ecr_handle->StopCancelling();
	    return OK;
	}
    }
    if (it == _ecr_devices.end())
	CLog::Warning(THISMODULE, task, "Invalid echocanceller handle");
    return OK;
}


/*----------------------------------------------------------------* 
 * Mixer: the following is very Prosody dependent
 *----------------------------------------------------------------*/

#ifdef XGATE_HAS_PROSODY

STATUS VoiceManager::MixerAlloc(CTask* task, VoiceMixer** mixer, int volume)
{
    TaskVoiceBlock *tvb = (TaskVoiceBlock *)task->GetTLSValue(_tlsId);

    // this task has already a mixer
    if (tvb->_mixer)
	return OK;

    *mixer = ProsodyDriver::Instance()->AllocateMixer(volume);
    if (!(*mixer)) {
	CLog::Warning(THISMODULE, task, "MixerAlloc: no mixer device available");
	task->SetLastError(VErr_VoiceUnavailable);
    } else {
	tvb->_mixer = *mixer;
    }

    return OK;
}

STATUS VoiceManager::MixerGetMediaInout(CTask* task, SupportSwitching** inout, VoiceMixer* mixer)
{
    if (!mixer->IsValid()) {
	CLog::Warning(THISMODULE, task, "MixerGetMediaInout: invalid mixer %x specified", mixer);
	return OK;
    }

    *inout = mixer->GetInoutDevice();

    return OK;
}

STATUS VoiceManager::MixerGetMediaMixin(CTask* task, SupportSwitching** mixin, VoiceMixer* mixer)
{
    if (!mixer->IsValid()) {
	CLog::Warning(THISMODULE, task, "MixerGetMediaInout: invalid mixer %x specified", mixer);
	return OK;
    }

    *mixin = mixer->GetMixinDevice();

    return OK;
}

STATUS VoiceManager::MixerFree(CTask* task, VoiceMixer* mixer)
{
    if (!mixer->IsValid()) {
	CLog::Warning(THISMODULE, task, "MixerGetMediaInout: invalid mixer %x specified", mixer);
	return OK;
    }

    ProsodyDriver::Instance()->FreeMixer(mixer);

    TaskVoiceBlock *tvb = (TaskVoiceBlock *)task->GetTLSValue(_tlsId);
    if (tvb)
	tvb->_mixer = 0;

    return OK;
}

#endif // XGATE_HAS_PROSODY



/*----------------------------------------------------------------* 
 * Implementation of VoiceAPI
 *----------------------------------------------------------------*/

/** Constructor
 */
VoiceAPI::VoiceAPI(int provid,VoiceManager *manager) : CAPIProvider(provid, THISMODULE, 100) 
{
    _manager = manager;

    if (API_Provider_Manager::Instance()->RegisterProvider(this) == NOT_OK) {
	CLog::Error( THISMODULE, "Unable to register provider");
	return;
    }

    enum API_Id {
	Id_SetVoiceFormat = 0,
	Id_GetVoiceFormat,
	Id_EnableInterrupt,
	Id_DisableInterrupt,
	Id_SetMaxRecTime,
	Id_GetMaxRecTime,
	Id_SetInterDigitTimeout,
	Id_GetInterDigitTimeout,
	Id_SetDigitTimeout,
	Id_GetDigitTimeout,
	Id_SetTermDigits /* 10 */,
	Id_GetTermDigits,
	Id_PlayFile,
	Id_PlayFileBkgnd,
	Id_StopPlayFileBkgnd,
	Id_PlayTone,
	Id_PlayDtmf,
	Id_RecordFile,
	Id_GetDigits,
	Id_GetLastTermDigit,
	Id_ClearDigits  /* 20 */,
	Id_PeekDigit,
	Id_SetLanguage,
	Id_SpellNumber,
	Id_WaitForSilence,
	Id_SetVolume,
	Id_GetVolume,
	Id_SetAGC,
	Id_GetAGC /* 28 */,
    Id_EndRecordFile, // T REC
	Id_EcrAlloc = 30,
	Id_EcrFree,
	Id_EcrGetMediaPrimary,
	Id_EcrGetMediaReference,
	Id_EcrStartCancelling,
	Id_EcrReinitCancelling,
	Id_EcrStopCancelling,

	Id_Alloc = 40,
	Id_Free,

	Id_GetMedia = 50,
	Id_GetDevName,
        Id_SetHostInfo,
        Id_GetHostInfo,


	Id_ConfCreateRoom = 56,
	Id_ConfDeleteRoom,
	Id_ConfFindRoom,
	Id_ConfOpenMedia,
	Id_ConfCloseMedia /* 60 */,
	Id_ConfEnableMedia,
	Id_ConfDisableMedia,
	Id_ConfSetMediaInputVolAGC,
	Id_ConfSetMediaOutputVolAGC,

	Id_MixerAlloc = 70,
	Id_MixerGetMediaInout,
	Id_MixerGetMediaMixin,
	Id_MixerFree,

	Id_PlayWAVFile = 80,
	Id_RecordWAVFile,

	Id_SetRecordMaxSilence =90,
	Id_GetRecordMaxSilence,
    };
    
    // STATUS SetVoiceFormat(CTask *task,VoiceFormat format)
    RegisterAPI("VoxSetVoiceFormat",Id_SetVoiceFormat,false,
	CVariant::VT_VOID,
	(CAPIProvider::APIPROC)&VoiceAPI::SetVoiceFormat,
	CAPIDef::CDECL_TYPE,
	2,
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("format",BYVAL,CVariant::VT_INT));
    
    // STATUS GetVoiceFormat(CTask *task,VoiceFormat *format)
    RegisterAPI("VoxGetVoiceFormat",Id_GetVoiceFormat,false,
	CVariant::VT_INT,
	(CAPIProvider::APIPROC)&VoiceAPI::GetVoiceFormat,
	CAPIDef::CDECL_TYPE,
	2,
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("format",BYREF,CVariant::VT_RET));
    
    // STATUS EnableInterrupt(CTask *task)
    RegisterAPI("VoxEnableInterrupt",Id_EnableInterrupt,false,
	CVariant::VT_VOID,
	(CAPIProvider::APIPROC)&VoiceAPI::EnableInterrupt,
	CAPIDef::CDECL_TYPE,
	1,
	CArgType("task",BYVAL,CVariant::VT_TASK));
    
    // STATUS DisableInterrupt(CTask *task)
    RegisterAPI("VoxDisableInterrupt",Id_DisableInterrupt,false,
	CVariant::VT_VOID,
	(CAPIProvider::APIPROC)&VoiceAPI::DisableInterrupt,
	CAPIDef::CDECL_TYPE,
	1,
	CArgType("task",BYVAL,CVariant::VT_TASK)); 
    
    // STATUS SetMaxRecTime(CTask *task,int maxrectime)
    RegisterAPI("VoxSetMaxRecTime",Id_SetMaxRecTime,false,
	CVariant::VT_VOID,
	(CAPIProvider::APIPROC)&VoiceAPI::SetMaxRecTime,
	CAPIDef::CDECL_TYPE,
	2,
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("maxrectime",BYVAL,CVariant::VT_INT));
    
    // STATUS GetMaxRecTime(CTask *task,int *maxrectime)
    RegisterAPI("VoxGetMaxRecTime",Id_GetMaxRecTime,false,
	CVariant::VT_INT,
	(CAPIProvider::APIPROC)&VoiceAPI::GetMaxRecTime,
	CAPIDef::CDECL_TYPE,
	2,
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("maxrectime",BYREF,CVariant::VT_RET));
    
    // STATUS SetInterDigitTimeout(CTask *task,int maxidtime)
    RegisterAPI("VoxSetInterDigitTimeout",Id_SetInterDigitTimeout,false,
	CVariant::VT_VOID,
	(CAPIProvider::APIPROC)&VoiceAPI::SetInterDigitTimeout,
	CAPIDef::CDECL_TYPE,
	2,
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("maxidtime",BYVAL,CVariant::VT_INT));
    
    // STATUS GetInterDigitTimeout(CTask *task,int *maxidtime)
    RegisterAPI("VoxGetInterDigitTimeout",Id_GetInterDigitTimeout,false,
	CVariant::VT_INT,
	(CAPIProvider::APIPROC)&VoiceAPI::GetInterDigitTimeout,
	CAPIDef::CDECL_TYPE,
	2,
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("maxidtime",BYREF,CVariant::VT_RET));
    
    // STATUS SetDigitTimeout(CTask *task,int maxtime)
    RegisterAPI("VoxSetDigitTimeout",Id_SetDigitTimeout,false,
	CVariant::VT_VOID,
	(CAPIProvider::APIPROC)&VoiceAPI::SetDigitTimeout,
	CAPIDef::CDECL_TYPE,
	2,
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("maxtime",BYVAL,CVariant::VT_INT));
    
    // STATUS GetDigitTimeout(CTask *task,int *maxtime)
    RegisterAPI("VoxGetDigitTimeout",Id_GetDigitTimeout,false,
	CVariant::VT_INT,
	(CAPIProvider::APIPROC)&VoiceAPI::GetDigitTimeout,
	CAPIDef::CDECL_TYPE,
	2,
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("maxtime",BYREF,CVariant::VT_RET));
    
    // STATUS SetTermDigits(CTask *task,char *termdigs)
    RegisterAPI("VoxSetTermDigits",Id_SetTermDigits,false,
	CVariant::VT_VOID,
	(CAPIProvider::APIPROC)&VoiceAPI::SetTermDigits,
	CAPIDef::CDECL_TYPE,
	2,
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("termdigs",BYVAL,CVariant::VT_STRING));
    
    // STATUS GetTermDigits(CTask *task,char *termdigs)
    RegisterAPI("VoxGetTermDigits",Id_GetTermDigits,false,
	CVariant::VT_STRING,
	(CAPIProvider::APIPROC)&VoiceAPI::GetTermDigits,
	CAPIDef::CDECL_TYPE,
	2,
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("termdigs",BYREF,CVariant::VT_RET));

	// STATUS SetRecordMaxSilence(CTask *task,int maxsil)
    RegisterAPI("VoxSetRecordMaxSil",Id_SetRecordMaxSilence,false,
	CVariant::VT_VOID,
	(CAPIProvider::APIPROC)&VoiceAPI::SetRecordMaxSilence,
	CAPIDef::CDECL_TYPE,
	2,
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("second",BYVAL,CVariant::VT_INT));
    
    // STATUS GetRecordMaxSilence(CTask *task,int *maxsil)

    RegisterAPI("VoxGetRecordMaxSil",Id_GetRecordMaxSilence,false,
	CVariant::VT_INT,
	(CAPIProvider::APIPROC)&VoiceAPI::GetRecordMaxSilence,
	CAPIDef::CDECL_TYPE,
	2,
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("second",BYREF,CVariant::VT_RET));

    
    // STATUS SetVolume(CTask *task, int dB)
    RegisterAPI("VoxSetVolume",Id_SetVolume,false,
	CVariant::VT_VOID,
	(CAPIProvider::APIPROC)&VoiceAPI::SetVolume,
	CAPIDef::CDECL_TYPE,
	2,
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("format",BYVAL,CVariant::VT_INT));
    
    // STATUS GetVolume(CTask *task, int *dB)
    RegisterAPI("VoxGetVolume",Id_GetVolume,false,
	CVariant::VT_INT,
	(CAPIProvider::APIPROC)&VoiceAPI::GetVolume,
	CAPIDef::CDECL_TYPE,
	2,
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("format",BYREF,CVariant::VT_RET));
    
    // STATUS SetAGC(CTask *task, int agc)
    RegisterAPI("VoxSetAGC",Id_SetAGC,false,
	CVariant::VT_VOID,
	(CAPIProvider::APIPROC)&VoiceAPI::SetAGC,
	CAPIDef::CDECL_TYPE,
	2,
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("format",BYVAL,CVariant::VT_INT));
    
    // STATUS GetAGC(CTask *task, int *agc)
    RegisterAPI("VoxGetAGC",Id_GetAGC,false,
	CVariant::VT_INT,
	(CAPIProvider::APIPROC)&VoiceAPI::GetAGC,
	CAPIDef::CDECL_TYPE,
	2,
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("format",BYREF,CVariant::VT_RET));
    
    // STATUS PlayFile(CTask *task,char *filename)
    RegisterAPI("VoxPlayFile",Id_PlayFile,true,
	CVariant::VT_VOID,
	(CAPIProvider::APIPROC)&VoiceAPI::PlayFile,
	CAPIDef::CDECL_TYPE,
	2,
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("filename",BYVAL,CVariant::VT_STRING));
    
    // STATUS API_CALL PlayFileBkgnd(CTask *task,char *filename,int repeat_flag)  { return _manager->PlayFileBkgnd(task,filename,repeat_flag); }
    RegisterAPI("VoxPlayFileBkgnd",Id_PlayFileBkgnd,true,
	CVariant::VT_VOID,
	(CAPIProvider::APIPROC)&VoiceAPI::PlayFileBkgnd,
	CAPIDef::CDECL_TYPE,
	3,
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("filename",BYVAL,CVariant::VT_STRING),
	CArgType("repeatflg",BYVAL,CVariant::VT_INT));
    
    // STATUS API_CALL StopPlayFileBkgnd(CTask *task,int waiteof_flag)  { return _manager->StopPlayFileBkgnd(task,waiteof_flag) }
    RegisterAPI("VoxStopPlayFileBkgnd",Id_StopPlayFileBkgnd,true,
	CVariant::VT_VOID,
	(CAPIProvider::APIPROC)&VoiceAPI::StopPlayFileBkgnd,
	CAPIDef::CDECL_TYPE,
	2,
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("waiteof",BYVAL,CVariant::VT_INT));
    
    // STATUS PlayTone(CTask *task,int freq1,int db1,int freq2,int db2,int duration)
    RegisterAPI("VoxPlayTone",Id_PlayTone,true,
	CVariant::VT_VOID,
	(CAPIProvider::APIPROC)&VoiceAPI::PlayTone,
	CAPIDef::CDECL_TYPE,
	6,
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("freq1",BYVAL,CVariant::VT_INT),
	CArgType("db1",BYVAL,CVariant::VT_INT),
	CArgType("freq2",BYVAL,CVariant::VT_INT),
	CArgType("db2",BYVAL,CVariant::VT_INT),
	CArgType("duration",BYVAL,CVariant::VT_INT));
    
    // STATUS PlayDtmf(CTask *task,char *dtmf)
    RegisterAPI("VoxPlayDtmf",Id_PlayDtmf,true,
	CVariant::VT_VOID,
	(CAPIProvider::APIPROC)&VoiceAPI::PlayDtmf,
	CAPIDef::CDECL_TYPE,
	2,
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("dtmf",BYVAL,CVariant::VT_STRING));
    
    // STATUS RecordFile(CTask *task,char *filename)
    RegisterAPI("VoxRecordFile",Id_RecordFile,true,
	CVariant::VT_VOID,
	(CAPIProvider::APIPROC) &VoiceAPI::RecordFile,
	CAPIDef::CDECL_TYPE,2,
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("filename",BYVAL,CVariant::VT_STRING));

    // STATUS EndRecordFile(CTask *task,char *filename) : T REC
    RegisterAPI("VoxEndRecordFile",Id_EndRecordFile,true,
    CVariant::VT_VOID,
    (CAPIProvider::APIPROC) &VoiceAPI::EndRecordFile_t,
    CAPIDef::CDECL_TYPE,1,
    CArgType("task",BYVAL,CVariant::VT_TASK));

    // STATUS WaitForSilence(CTask *task, int max_wait_secs, int* remaining_time)
    RegisterAPI("VoxWaitForSilence",Id_WaitForSilence,true,
	CVariant::VT_INT,
	(CAPIProvider::APIPROC) &VoiceAPI::WaitForSilence,
	CAPIDef::CDECL_TYPE,
	3,
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("max_wait_secs",BYVAL,CVariant::VT_INT),
	CArgType("remaining_time",BYREF,CVariant::VT_RET));

    // STATUS GetDigits(CTask *task,int maxlength,char *buffer)
    RegisterAPI("VoxGetDigits",Id_GetDigits,true,
	CVariant::VT_INT,
	(CAPIProvider::APIPROC) &VoiceAPI::GetDigits,
	CAPIDef::CDECL_TYPE,
	4,
	CArgType("task",BYVAL,CVariant::VT_TASK),
        CArgType("retval",BYREF,CVariant::VT_RET),
	CArgType("maxlength",BYVAL,CVariant::VT_INT),
	CArgType("buffer",BYREF,CVariant::VT_STRING));
    
    // STATUS GetLastTermDigit(CTask *task,char *buffer)
    RegisterAPI("VoxGetLastTermDigit",Id_GetLastTermDigit,false,
	CVariant::VT_VOID,
	(CAPIProvider::APIPROC) &VoiceAPI::GetLastTermDigit,
	CAPIDef::CDECL_TYPE,
	2,
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("buffer",BYREF,CVariant::VT_STRING));
    
    // STATUS ClearDigits(CTask *task)
    RegisterAPI("VoxClearDigits",Id_ClearDigits,false,
	CVariant::VT_VOID,
	(CAPIProvider::APIPROC) &VoiceAPI::ClearDigits,
	CAPIDef::CDECL_TYPE,1,
	CArgType("task",BYVAL,CVariant::VT_TASK));
    
    // STATUS PeekDigit(CTask *task,char *buffer)
    RegisterAPI("VoxPeekDigit",Id_PeekDigit,false,
	CVariant::VT_STRING,
	(CAPIProvider::APIPROC)&VoiceAPI::PeekDigit,
	CAPIDef::CDECL_TYPE,
	2,
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("digits",BYREF,CVariant::VT_RET));
    
    // STATUS SetLanguage(CTask *task,unsigned langID)
    RegisterAPI("VoxSetLanguage",Id_SetLanguage,false,
	CVariant::VT_VOID,
	(CAPIProvider::APIPROC)&VoiceAPI::SetLanguage,
	CAPIDef::CDECL_TYPE,
	2,
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("langID",BYVAL,CVariant::VT_STRING));
    
    // STATUS SpellNumber(CTask *task,unsigned number)
    RegisterAPI("VoxSpellNumber",Id_SpellNumber,true,
	CVariant::VT_VOID,
	(CAPIProvider::APIPROC)&VoiceAPI::SpellNumber,
	CAPIDef::CDECL_TYPE,
	2,
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("number",BYVAL,CVariant::VT_INT));
    
    // STATUS Alloc(CTask *task)
    RegisterAPI("VoxAlloc",Id_Alloc,false,
	CVariant::VT_VOID,
	(CAPIProvider::APIPROC)&VoiceAPI::Alloc,
	CAPIDef::CDECL_TYPE,
	1,
	CArgType("task",BYVAL,CVariant::VT_TASK));
    
    // STATUS Free(CTask *task)
    RegisterAPI("VoxFree",Id_Free,true,
	CVariant::VT_VOID,
	(CAPIProvider::APIPROC)&VoiceAPI::Free,
	CAPIDef::CDECL_TYPE,
	1,
	CArgType("task",BYVAL,CVariant::VT_TASK));

    // STATUS GetDevName(CTask *task,char *buffer)
    RegisterAPI("VoxGetDevName",Id_GetDevName,false,
	CVariant::VT_STRING,
	(CAPIProvider::APIPROC)&VoiceAPI::GetDevName,
	CAPIDef::CDECL_TYPE,
	2,
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("devname",BYREF,CVariant::VT_RET));

    // STATUS SetHostInfo(CTask *task, int conf_id)
    RegisterAPI("VoxSetHostInfo",Id_SetHostInfo,false,
    CVariant::VT_VOID,
    (CAPIProvider::APIPROC)&VoiceAPI::SetHostInfo,
    CAPIDef::CDECL_TYPE,
    3,
    CArgType("task",BYVAL,CVariant::VT_TASK),
    CArgType("conf_id",BYVAL,CVariant::VT_STRING),
    CArgType("status",BYVAL,CVariant::VT_INT));

    // STATUS GetHostInfo(CTask *task, int *retval, int conf_id)
    RegisterAPI("VoxGetHostInfo",Id_GetHostInfo,false,
    CVariant::VT_INT,
    (CAPIProvider::APIPROC)&VoiceAPI::GetHostInfo,
    CAPIDef::CDECL_TYPE,
    3,
    CArgType("task",BYVAL,CVariant::VT_TASK),
    CArgType("retval",BYREF,CVariant::VT_RET),
    CArgType("conf_id",BYVAL,CVariant::VT_STRING));

    // STATUS GetMedia(CTask *task,VoiceDevice **device)
    RegisterAPI("VoxGetMedia",Id_GetMedia,false,
	CVariant::VT_INT,
	(CAPIProvider::APIPROC)&VoiceAPI::GetMedia,
	CAPIDef::CDECL_TYPE,
	2,
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("device",BYREF,CVariant::VT_RET));

    // STATUS ConfCreateRoom(CTask *task, void* conf);
    RegisterAPI("ConfCreateRoom",Id_ConfCreateRoom,false,
	CVariant::VT_INT,
	(CAPIProvider::APIPROC)&VoiceAPI::ConfCreateRoom,
	CAPIDef::CDECL_TYPE,
	4,
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("room_handle",BYREF,CVariant::VT_RET),
	CArgType("room_name",BYVAL,CVariant::VT_STRING),
	CArgType("room_size",BYVAL,CVariant::VT_INT));

    // STATUS ConfDeleteRoom(CTask *task, int room_handle);
    RegisterAPI("ConfDeleteRoom",Id_ConfDeleteRoom,false,
	CVariant::VT_VOID,
	(CAPIProvider::APIPROC)&VoiceAPI::ConfDeleteRoom,
	CAPIDef::CDECL_TYPE,
	3,
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("room_handle",BYVAL,CVariant::VT_INT),
        CArgType("conf_id",BYVAL,CVariant::VT_STRING));

    // STATUS ConfFindRoom(CTask *task, int *room_handle, char *room_name);
    RegisterAPI("ConfFindRoom",Id_ConfFindRoom,false,
	CVariant::VT_INT,
	(CAPIProvider::APIPROC)&VoiceAPI::ConfFindRoom,
	CAPIDef::CDECL_TYPE,
	3,
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("room_handle",BYREF,CVariant::VT_RET),
	CArgType("room_name",BYVAL,CVariant::VT_STRING));

    // STATUS ConfOpenMedia(CTask *task, VoiceDevice **device, int room_handle);
    RegisterAPI("ConfOpenMedia",Id_ConfOpenMedia,false,
	CVariant::VT_INT,
	(CAPIProvider::APIPROC)&VoiceAPI::ConfOpenMedia,
	CAPIDef::CDECL_TYPE,
	3,
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("handle",BYREF,CVariant::VT_RET),
	CArgType("room_handle",BYVAL,CVariant::VT_INT));

    // STATUS ConfCloseMedia(CTask *task, int room_handle, VoiceDevice *device);
    RegisterAPI("ConfCloseMedia",Id_ConfCloseMedia,false,
	CVariant::VT_VOID,
	(CAPIProvider::APIPROC)&VoiceAPI::ConfCloseMedia,
	CAPIDef::CDECL_TYPE,
	3,
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("room_handle",BYVAL,CVariant::VT_INT),
	CArgType("handle",BYVAL,CVariant::VT_INT));

    // STATUS ConfAddParty(CTask* task, int room_handle, VoiceDevice *device);
    RegisterAPI("ConfEnableMedia",Id_ConfEnableMedia,false,
	CVariant::VT_VOID,
	(CAPIProvider::APIPROC)&VoiceAPI::ConfEnableMedia,
	CAPIDef::CDECL_TYPE,
	3,
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("room_handle",BYVAL,CVariant::VT_INT),
	CArgType("handle",BYVAL,CVariant::VT_INT));

    // STATUS ConfRemoveParty(CTask* task, int room_handle, VoiceDevice *device);
    RegisterAPI("ConfDisableMedia",Id_ConfDisableMedia,false,
	CVariant::VT_VOID,
	(CAPIProvider::APIPROC)&VoiceAPI::ConfDisableMedia,
	CAPIDef::CDECL_TYPE,
	3,
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("room_handle",BYVAL,CVariant::VT_INT),
	CArgType("handle",BYVAL,CVariant::VT_INT));

    // STATUS ConfSetMediaInputVolAGC(CTask* task, VoiceDevice *device, int dB, int agc);
    RegisterAPI("ConfSetMediaInputVolAGC",Id_ConfSetMediaInputVolAGC,false,
	CVariant::VT_VOID,
	(CAPIProvider::APIPROC)&VoiceAPI::ConfSetMediaInputVolAGC,
	CAPIDef::CDECL_TYPE,
	4,
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("handle",BYVAL,CVariant::VT_INT),
	CArgType("volume",BYVAL,CVariant::VT_INT),
	CArgType("agc",BYVAL,CVariant::VT_INT));

    // STATUS ConfSetMediaOutputVolAGC(CTask* task, VoiceDevice *device, int dB, int agc);
    RegisterAPI("ConfSetMediaOutputVolAGC",Id_ConfSetMediaOutputVolAGC,false,
	CVariant::VT_VOID,
	(CAPIProvider::APIPROC)&VoiceAPI::ConfSetMediaOutputVolAGC,
	CAPIDef::CDECL_TYPE,
	4,
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("handle",BYVAL,CVariant::VT_INT),
	CArgType("volume",BYVAL,CVariant::VT_INT),
	CArgType("agc",BYVAL,CVariant::VT_INT));

    // STATUS EcrAlloc(CTask* task, EchoCanceller** handle);
    RegisterAPI("EcrAlloc", Id_EcrAlloc, false,
	CVariant::VT_INT,
	(CAPIProvider::APIPROC)&VoiceAPI::EcrAlloc,
	CAPIDef::CDECL_TYPE,
	2,
	CArgType("task", BYVAL, CVariant::VT_TASK),
	CArgType("handle",BYREF, CVariant::VT_RET));

    // STATUS EcrFree(CTask* task, EchoCanceller* handle);
    RegisterAPI("EcrFree", Id_EcrFree, false,
	CVariant::VT_VOID,
	(CAPIProvider::APIPROC)&VoiceAPI::EcrFree,
	CAPIDef::CDECL_TYPE,
	2,
	CArgType("task", BYVAL, CVariant::VT_TASK),
	CArgType("handle",BYVAL, CVariant::VT_INT));


    // STATUS EcrGetMediaInput(CTask* task, VoiceDevice** device, EchoCanceller* handle);
    RegisterAPI("EcrGetMediaPrimary", Id_EcrGetMediaPrimary, false,
	CVariant::VT_INT,
	(CAPIProvider::APIPROC)&VoiceAPI::EcrGetMediaPrimary,
	CAPIDef::CDECL_TYPE,
	3,
	CArgType("task", BYVAL, CVariant::VT_TASK),
	CArgType("device",BYREF, CVariant::VT_RET),
	CArgType("handle",BYVAL, CVariant::VT_INT));

    // STATUS EcrGetMediaReference(CTask* task, VoiceDevice** device, EchoCanceller* handle);
    RegisterAPI("EcrGetMediaReference", Id_EcrGetMediaReference, false,
	CVariant::VT_INT,
	(CAPIProvider::APIPROC)&VoiceAPI::EcrGetMediaReference,
	CAPIDef::CDECL_TYPE,
	3,
	CArgType("task", BYVAL, CVariant::VT_TASK),
	CArgType("device",BYREF, CVariant::VT_RET),
	CArgType("handle",BYVAL, CVariant::VT_INT));

    // STATUS EcrStartCancelling(CTask* task, EchoCanceller* handle);
    RegisterAPI("EcrStartCancelling", Id_EcrStartCancelling, false,
	CVariant::VT_VOID,
	(CAPIProvider::APIPROC)&VoiceAPI::EcrStartCancelling,
	CAPIDef::CDECL_TYPE,
	2,
	CArgType("task", BYVAL, CVariant::VT_TASK),
	CArgType("handle",BYVAL, CVariant::VT_INT));

    // STATUS EcrReinitCancelling(CTask* task, EchoCanceller* handle);
    RegisterAPI("EcrReinitCancelling", Id_EcrReinitCancelling, false,
	CVariant::VT_VOID,
	(CAPIProvider::APIPROC)&VoiceAPI::EcrReinitCancelling,
	CAPIDef::CDECL_TYPE,
	2,
	CArgType("task", BYVAL, CVariant::VT_TASK),
	CArgType("handle",BYVAL, CVariant::VT_INT));

    // STATUS EcrStopCancelling(CTask* task, EchoCanceller* handle);
    RegisterAPI("EcrStopCancelling", Id_EcrStopCancelling, false,
	CVariant::VT_VOID,
	(CAPIProvider::APIPROC)&VoiceAPI::EcrStopCancelling,
	CAPIDef::CDECL_TYPE,
	2,
	CArgType("task", BYVAL, CVariant::VT_TASK),
	CArgType("handle",BYVAL, CVariant::VT_INT));

#ifdef XGATE_HAS_PROSODY

    // STATUS MixerAlloc(CTask* task, VoiceMixer** device, int volume);
    RegisterAPI("MixerAlloc", Id_MixerAlloc, false,
	CVariant::VT_INT,
	(CAPIProvider::APIPROC)&VoiceAPI::MixerAlloc,
	CAPIDef::CDECL_TYPE,
	3,
	CArgType("task", BYVAL, CVariant::VT_TASK),
	CArgType("mixer",BYREF, CVariant::VT_RET),
	CArgType("volume",BYVAL, CVariant::VT_INT));

    // STATUS MixerGetMediaInout(CTask* task, SupportSwitching** inout, VoiceMixer* mixer);
    RegisterAPI("MixerGetMediaInout", Id_MixerGetMediaInout, false,
	CVariant::VT_INT,
	(CAPIProvider::APIPROC)&VoiceAPI::MixerGetMediaInout,
	CAPIDef::CDECL_TYPE,
	3,
	CArgType("task", BYVAL, CVariant::VT_TASK),
	CArgType("inout",BYREF, CVariant::VT_RET),
	CArgType("mixer",BYVAL, CVariant::VT_INT));

    // STATUS MixerGetMediaMixin(CTask* task, SupportSwitching** mixin, VoiceMixer* mixer);
    RegisterAPI("MixerGetMediaMixin", Id_MixerGetMediaMixin, false,
	CVariant::VT_INT,
	(CAPIProvider::APIPROC)&VoiceAPI::MixerGetMediaMixin,
	CAPIDef::CDECL_TYPE,
	3,
	CArgType("task", BYVAL, CVariant::VT_TASK),
	CArgType("mixin",BYREF, CVariant::VT_RET),
	CArgType("mixer",BYVAL, CVariant::VT_INT));

    // STATUS MixerFree(CTask* task, VoiceMixer* mixer);
    RegisterAPI("MixerFree", Id_MixerFree, false,
	CVariant::VT_VOID,
	(CAPIProvider::APIPROC)&VoiceAPI::MixerFree,
	CAPIDef::CDECL_TYPE,
	2,
	CArgType("task", BYVAL, CVariant::VT_TASK),
	CArgType("mixer",BYVAL, CVariant::VT_INT));

    // STATUS PlayWAVFile(CTask *task,char *filename)
    RegisterAPI("VoxPlayWAVFile",Id_PlayWAVFile,true,
	CVariant::VT_VOID,
	(CAPIProvider::APIPROC)&VoiceAPI::PlayWAVFile,
	CAPIDef::CDECL_TYPE,
	2,
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("filename",BYVAL,CVariant::VT_STRING));
    
    // STATUS RecordWAVFile(CTask *task,char *filename)
    RegisterAPI("VoxRecordWAVFile",Id_RecordWAVFile,true,
	CVariant::VT_VOID,
	(CAPIProvider::APIPROC) &VoiceAPI::RecordWAVFile,
	CAPIDef::CDECL_TYPE,2,
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("filename",BYVAL,CVariant::VT_STRING));

#endif // XGATE_HAS_PROSODY

}

/** Destructor
 */
VoiceAPI::~VoiceAPI()
{
}

STATUS VoiceAPI::SetVoiceFormat(CTask *task,VoiceFormat format) 
{ 
    return _manager->SetVoiceFormat(task,format); 
}

STATUS VoiceAPI::GetVoiceFormat(CTask *task,VoiceFormat *format) 
{ 
    return _manager->GetVoiceFormat(task,format); 
}

STATUS VoiceAPI::EnableInterrupt(CTask *task) 
{ 
    return _manager->EnableInterrupt(task); 
}

STATUS VoiceAPI::DisableInterrupt(CTask *task) 
{ 
    return _manager->DisableInterrupt(task); 
}

STATUS VoiceAPI::SetMaxRecTime(CTask *task,int maxrectime) 
{ 
    return _manager->SetMaxRecTime(task,maxrectime); 
}

STATUS VoiceAPI::GetMaxRecTime(CTask *task,int *maxrectime) 
{ 
    return _manager->GetMaxRecTime(task,maxrectime); 
}

STATUS VoiceAPI::SetInterDigitTimeout(CTask *task,int maxidtime) 
{ 
    return _manager->SetInterDigitTimeout(task,maxidtime); 
}

STATUS VoiceAPI::GetInterDigitTimeout(CTask *task,int *maxidtime) 
{ 
    return _manager->GetInterDigitTimeout(task,maxidtime); 
}

STATUS VoiceAPI::SetDigitTimeout(CTask *task,int maxtime) 
{ 
    return _manager->SetDigitTimeout(task,maxtime); 
}

STATUS VoiceAPI::GetDigitTimeout(CTask *task,int *maxtime) 
{ 
    return _manager->GetDigitTimeout(task,maxtime); 
}

STATUS VoiceAPI::SetTermDigits(CTask *task,char *termdigs) 
{ 
    return _manager->SetTermDigits(task,termdigs); 
}

STATUS VoiceAPI::GetTermDigits(CTask *task,char *termdigs) 
{ 
    return _manager->GetTermDigits(task,termdigs); 
}

STATUS VoiceAPI::SetRecordMaxSilence(CTask *task,int maxsil) 
{ 
    return _manager->SetRecordMaxSilence(task,maxsil); 
}

STATUS VoiceAPI::GetRecordMaxSilence(CTask *task,int *maxsil) 
{ 
    return _manager->GetRecordMaxSilence(task,maxsil); 
}

STATUS VoiceAPI::SetVolume(CTask *task, int dB)
{ 
    return _manager->SetVolume(task, dB); 
}

STATUS VoiceAPI::GetVolume(CTask *task, int *dB) 
{ 
    return _manager->GetVolume(task, dB); 
}

STATUS VoiceAPI::SetAGC(CTask *task, int agc) 
{ 
    return _manager->SetAGC(task, agc); 
}

STATUS VoiceAPI::GetAGC(CTask *task, int *agc)
{ 
    return _manager->GetAGC(task, agc); 
}

STATUS VoiceAPI::PlayFile(CTask *task,char *filename) 
{ 
    return _manager->PlayFile(task,filename,false);
}

STATUS VoiceAPI::PlayWAVFile(CTask *task,char *filename) 
{ 
    return _manager->PlayFile(task,filename,true); 
}

STATUS VoiceAPI::PlayFileBkgnd(CTask *task,char *filename,int repeat_flag)  
{ 
    return _manager->PlayFileBkgnd(task,filename,repeat_flag); 
}

STATUS VoiceAPI::StopPlayFileBkgnd(CTask *task,int waiteof_flag) 
{ 
    return _manager->StopPlayFileBkgnd(task,waiteof_flag); 
}

STATUS VoiceAPI::PlayTone(CTask *task,int freq1,int db1,int freq2,int db2,int duration) 
{ 
    return _manager->PlayTone(task,freq1,db1,freq2,db2,duration); 
}

STATUS VoiceAPI::PlayDtmf(CTask *task,char *dtmf) 
{ 
    return _manager->PlayDtmf(task,dtmf); 
}

STATUS VoiceAPI::RecordFile(CTask *task,char *filename) 
{ 
    return _manager->RecordFile(task,filename,false); 
}

// T REC
STATUS VoiceAPI::EndRecordFile_t(CTask *task)
{
    return _manager->EndRecordFile_t(task);
}

STATUS VoiceAPI::RecordWAVFile(CTask *task,char *filename) 
{ 
    return _manager->RecordFile(task,filename,true);
}

STATUS VoiceAPI::GetDigits(CTask *task,int *retval,int maxlength,char *buffer) 
{ 
    return _manager->GetDigits(task,retval,maxlength,buffer); 
}

STATUS VoiceAPI::WaitForSilence(CTask *task, int max_wait_secs, int* remaining_time)
{
    return _manager->WaitForSilence(task, max_wait_secs, remaining_time);
}

STATUS VoiceAPI::GetLastTermDigit(CTask *task,char *buffer) 
{ 
    return _manager->GetLastTermDigit(task,buffer); 
}

STATUS VoiceAPI::ClearDigits(CTask *task)
{ 
    return _manager->ClearDigits(task); 
}

STATUS VoiceAPI::PeekDigit(CTask *task,char *buffer) 
{ 
    return _manager->PeekDigit(task,buffer); 
}

STATUS VoiceAPI::SetLanguage(CTask *task,const char* langID) 
{ 
    return _manager->SetLanguage(task,langID); 
}

STATUS VoiceAPI::SpellNumber(CTask *task,int number) 
{ 
    return _manager->SpellNumber(task,number); 
}

STATUS VoiceAPI::Alloc(CTask *task) 
{ 
    return _manager->Alloc(task); 
}

STATUS VoiceAPI::Free(CTask *task) 
{ 
    return _manager->Free(task); 
}

STATUS VoiceAPI::GetDevName(CTask *task,char *devname)
{ 
    return _manager->GetDevName(task,devname); 
}

STATUS VoiceAPI::GetHostInfo(CTask *task, int *retval, char *conf_id)
{
    return _manager->GetHostInfo(task,retval,conf_id);
}

STATUS VoiceAPI::SetHostInfo(CTask *task, char *conf_id, int status)
{
    return _manager->SetHostInfo(task,conf_id, status);
}

STATUS VoiceAPI::GetMedia(CTask *task,VoiceDevice **device)
{ 
    return _manager->GetMedia(task,device); 
}

STATUS VoiceAPI::ConfCreateRoom(CTask *task, int *room_handle, char *room_name, int room_size)
{
    return _manager->ConfCreateRoom(task, room_handle, room_name, room_size);
}

STATUS VoiceAPI::ConfDeleteRoom(CTask *task, int room_handle,  char *conf_id)
{
    return _manager->ConfDeleteRoom(task, room_handle, conf_id);
}

STATUS VoiceAPI::ConfFindRoom(CTask *task, int *room_handle, char *room_name)
{
    return _manager->ConfFindRoom(task, room_handle, room_name);
}

STATUS VoiceAPI::ConfOpenMedia(CTask *task, VoiceDevice **device, int room_handle)
{
    return _manager->ConfOpenMedia(task, device, room_handle);
}

STATUS VoiceAPI::ConfCloseMedia(CTask *task, int room_handle, VoiceDevice *device)
{
    return _manager->ConfCloseMedia(task, room_handle, device);
}

STATUS VoiceAPI::ConfEnableMedia(CTask* task, int room_handle, VoiceDevice *device)
{
    return _manager->ConfEnableMedia(task, room_handle, device);
}

STATUS VoiceAPI::ConfDisableMedia(CTask* task, int room_handle, VoiceDevice *device)
{
    return _manager->ConfDisableMedia(task, room_handle, device);
}

STATUS API_CALL VoiceAPI::ConfSetMediaInputVolAGC(CTask* task, VoiceDevice *device, int dB, int agc)
{
    return _manager->ConfSetMediaInputVolAGC(task, device, dB, agc);
}

STATUS API_CALL VoiceAPI::ConfSetMediaOutputVolAGC(CTask* task, VoiceDevice *device, int dB, int agc)
{
    return _manager->ConfSetMediaOutputVolAGC(task, device, dB, agc);
}

STATUS VoiceAPI::EcrAlloc(CTask* task, EchoCanceller** handle) {
    return _manager->EcrAlloc(task, handle);
}

STATUS VoiceAPI::EcrFree(CTask* task, EchoCanceller* handle) {
    return _manager->EcrFree(task, handle);
}

STATUS VoiceAPI::EcrGetMediaPrimary(CTask* task, VoiceDevice** device, EchoCanceller* handle) {
    return _manager->EcrGetMediaPrimary(task, device, handle);
}

STATUS VoiceAPI::EcrGetMediaReference(CTask* task, VoiceDevice** device, EchoCanceller* handle) {
    return _manager->EcrGetMediaReference(task, device, handle);
}

STATUS VoiceAPI::EcrStartCancelling(CTask* task, EchoCanceller* handle) {
    return _manager->EcrStartCancelling(task, handle);
}

STATUS VoiceAPI::EcrReinitCancelling(CTask* task, EchoCanceller* handle) {
    return _manager->EcrReinitCancelling(task, handle);
}

STATUS VoiceAPI::EcrStopCancelling(CTask* task, EchoCanceller* handle) {
    return _manager->EcrStopCancelling(task, handle);
}

#ifdef XGATE_HAS_PROSODY

STATUS VoiceAPI::MixerAlloc(CTask* task, VoiceMixer** mixer, int volume)
{
    return _manager->MixerAlloc(task, mixer, volume);
}

STATUS VoiceAPI::MixerGetMediaInout(CTask* task, SupportSwitching** inout, VoiceMixer* mixer)
{
    return _manager->MixerGetMediaInout(task, inout, mixer);
}


STATUS VoiceAPI::MixerGetMediaMixin(CTask* task, SupportSwitching** mixin, VoiceMixer* mixer)
{
    return _manager->MixerGetMediaMixin(task, mixin, mixer);
}


STATUS VoiceAPI::MixerFree(CTask* task, VoiceMixer* mixer)
{
    return _manager->MixerFree(task, mixer);
}

#endif // XGATE_HAS_PROSODY


