/*----------------------------------------------------------------* 
 * $Archive: /xGate/VoiceDevice.h $
 * $Date: 18/02/05 18:28 $
 * $Revision: 9 $
 *
 * $History: VoiceDevice.h $
 * 
 * *****************  Version 9  *****************
 * User: Sonny        Date: 18/02/05   Time: 18:28
 * Updated in $/xGate
 * fixed bug: conference room is deleted only if all devices are free and
 * the owner is detached
 * 
 * *****************  Version 8  *****************
 * User: Junanto      Date: 1/02/05    Time: 15:55
 * Updated in $/xGate
 * added API WaitForSilence (only for Prosody)
 * 
 * *****************  Version 7  *****************
 * User: Sonny        Date: 19/10/01   Time: 19:28
 * Updated in $/xGate
 * bug fixes on conference and echocanceller
 * 
 * *****************  Version 6  *****************
 * User: Sonny        Date: 18/10/01   Time: 11:59
 * Updated in $/xGate
 * add echocanceller
 * 
 * *****************  Version 5  *****************
 * User: Bennylp      Date: 7/31/01    Time: 6:07p
 * Updated in $/xGate
 * Changed char * to const char * in constructor.
 * 
 * *****************  Version 4  *****************
 * User: Junanto      Date: 24/04/01   Time: 20:04
 * Updated in $/xGate
 * added mecanism to deallocate a voice device by VoiceManager
 * and release the switching resource
 * 
 * *****************  Version 3  *****************
 * User: Junanto      Date: 24/04/01   Time: 18:25
 * Updated in $/xGate
 * first link between Prosody/Aculab Media and switching framework
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


#if !defined(_VOICEDEVICE_H__INCLUDED_)
#define _VOICEDEVICE_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "../../core/xgate/types.h"
//#include "../../core/xgate/Switching.h"

#include "types.h"
#include "Switching.h"

/** Forward declaration
 */
class CTask;

/** VoiceFormat:
 */
enum VoiceFormat
{
    VFmt_ADPCM_24K,
    VFmt_ADPCM_32K,
    VFmt_PCM_ALAW_48K,
    VFmt_PCM_MULAW_48K,
    VFmt_PCM_ALAW_64K,
    VFmt_PCM_MULAW_64K,
};

/** VoiceError:
 */
enum VoiceError
{
    VErr_NoError = 0,
    VErr_Hardware = -100,
    VErr_VoiceUnavailable = -101,
    VErr_AlreadyAllocated = -102,
    VErr_NoVoice = -103,
    VErr_File = -104,
    VErr_VoiceBusy = -105,
    VErr_InvalidOperation = -106,
    VErr_InvalidParameter = -107,
};


#define VOICE_MAXDIGITS				31
#define VOICE_MAXVOCAB				16


/** VoiceDevice: base class for all voice channel
 */
class VoiceDevice : public SupportSwitching
{
    String	_name;
    CTask	*_task;

public:

    int		_output_dB;
    int		_output_agc;
    int		_input_dB;
    int		_input_agc;

    VoiceDevice(const char *devname,SwitchDevice *swdev);
    virtual ~VoiceDevice();

    /** GetName: return the name of this device
     */
    const char *GetName();

    /** Task related
     */
    CTask *GetTask();
    void AttachTask(CTask *pTask);
    void DetachTask();

    // deallocate this voice device
    virtual void Free() = 0;

    // must be implemented in derived class
    virtual bool IsDeviceIdle() = 0;
    virtual bool CancelJob() = 0;

    // Telephony actions, must be implemented in derived class
    virtual STATUS SetVoiceFormat(VoiceFormat format) = 0;
    virtual STATUS GetVoiceFormat(VoiceFormat *format) = 0;
    virtual STATUS EnableInterrupt() = 0;
    virtual STATUS DisableInterrupt() = 0;
    virtual STATUS SetMaxRecTime(int maxrectime) = 0;
    virtual STATUS GetMaxRecTime(int *maxrectime) = 0;
    virtual STATUS SetInterDigitTimeout(int maxidtime) = 0;
    virtual STATUS GetInterDigitTimeout(int *maxidtime) = 0;
    virtual STATUS SetDigitTimeout(int maxidtime) = 0;
    virtual STATUS GetDigitTimeout(int *maxidtime) = 0;
    virtual STATUS SetTermDigits(char *termdigs) = 0;
    virtual STATUS GetTermDigits(char *termdigs) = 0;
#ifndef __linux__
    virtual STATUS SetRecordMaxSilence(int maxsil) = 0;
    virtual STATUS GetRecordMaxSilence(int *maxsil) = 0;
#endif
    
    virtual STATUS SetOutputVolume(int dB) {
	_output_dB = dB;
	return (STATUS)OK;
    }

    virtual STATUS GetOutputVolume(int *dB) {
	*dB = _output_dB;
	return (STATUS)OK;
    }

    virtual STATUS SetOutputAGC(int agc) {
	_output_agc = agc;
	return (STATUS)OK;
    }

    virtual STATUS GetOutputAGC(int *agc) {
	*agc = _output_agc;
	return (STATUS)OK;
    }

    virtual STATUS SetInputVolume(int dB) {
	_input_dB = dB;
	return (STATUS)OK;
    }

    virtual STATUS SetInputAGC(int agc) {
	_input_agc = agc;
	return (STATUS)OK;
    }

    virtual STATUS PlayFileArray(int nbfiles,char const **filename,bool bkgnd,bool autorepeat) = 0;
    virtual STATUS StopPlayFileBkGnd(bool waiteof) = 0;
    virtual STATUS PlayTone(int freq1,int db1,int freq2,int db2,int duration) = 0;
    virtual STATUS PlayDtmf(char *dtmf) = 0;
    virtual STATUS RecordFile(char *filename) = 0;
    virtual STATUS EndRecordFile_t() = 0;
    virtual STATUS GetDigits(int length,char *buffer) = 0;
    virtual STATUS GetLastTermDigit(char *buffer) = 0;
    virtual STATUS ClearDigits() = 0;
    virtual STATUS PeekDigit(char *buffer) = 0;
    virtual STATUS WaitForSilence(int max_wait_time, int* remaining_time) {
	return (STATUS)OK;
    }

    // Any device that support WAV files must override these member functions
    virtual bool SupportPlayWav() {
	return false;
    }
    virtual bool SupportRecordWav() {
	return false;
    }
    virtual STATUS PlayFileArrayWav(int nbfiles,char const **filename,bool bkgnd,bool autorepeat,bool wav_mode) {
	return NOT_OK;
    }
    virtual STATUS RecordFileWav(char *filename,bool wav_mode) {
	return NOT_OK;
    }
};
typedef List<VoiceDevice*> ListVoiceDevice;

/** ConferenceRoom: base class for all conference room
 */
class ConferenceRoom 
{
protected:
    String _name;
    int _handle;
    int _members;
    CTask* _task;

    struct conf_device {
	VoiceDevice* _device;
	bool _allocated;
	bool _conferenced;

	conf_device() {
	    _device = NULL;
	    _allocated = false;
	    _conferenced = false;
	}
    };

    List<conf_device*> _conf_devices;    


    ConferenceRoom() {
	_handle = 0;
	_members = 0;
    }

    virtual ~ConferenceRoom() {}

    int GetHandle() { return _handle; };
    void InsertDevice(VoiceDevice* dev);
    void SetTaskOwner(CTask *task) { _task = task; }
    void AttachTask(CTask* task);
    bool DetachTask(CTask* task);

    VoiceDevice* GetDevice();
    STATUS FreeDevice(VoiceDevice*);

    virtual STATUS AddParty(VoiceDevice*) { return NOT_OK; };
    virtual STATUS RemoveParty(VoiceDevice*) { return NOT_OK; };

    friend class VoiceManager;
};


/** EchoCanceller: base class for all echo canceller device
 */
class EchoCanceller {
public:

    EchoCanceller() { _task = NULL; };
    virtual ~EchoCanceller() {}

    virtual VoiceDevice* GetPrimaryChannel() { return NULL; };
    virtual VoiceDevice* GetReferenceChannel() { return NULL; };

    virtual void StartCancelling() {};
    virtual void ReinitCancelling() {};
    virtual void StopCancelling() {};

    virtual void AttachTask(CTask* task);
    virtual bool DetachTask(CTask* task);

protected:
    CTask* _task;

};


//-------------------------------------------------------------------
class VoiceMixer
{
    unsigned _signature;

    enum {
	SIGNATURE=0xdeadc0de,
    };

public:
    //----------------------------------------
    VoiceMixer()
    {
	_signature = SIGNATURE;
    }

    //----------------------------------------
    virtual ~VoiceMixer()
    {
	_signature = ~_signature;
    }

    //----------------------------------------
    bool IsValid()
    {
	try {
	    return _signature == SIGNATURE;
	} catch (...) {
	    return false;
	}
    }

    virtual SupportSwitching* GetInoutDevice() = 0;
    virtual SupportSwitching* GetMixinDevice() = 0;
};


#endif // !defined(_VOICEDEVICE_H__INCLUDED_)
