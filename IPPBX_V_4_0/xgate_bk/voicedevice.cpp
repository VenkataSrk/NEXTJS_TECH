/*----------------------------------------------------------------* 
 * $Archive: /xGate/VoiceDevice.cpp $
 * $Date: 18/02/05 18:26 $
 * $Revision: 9 $
 *
 * $History: VoiceDevice.cpp $
 * 
 * *****************  Version 9  *****************
 * User: Sonny        Date: 18/02/05   Time: 18:26
 * Updated in $/xGate
 * fixed bug: conference room will be deleted only if all devices are free
 * and the owner is detached
 * 
 * *****************  Version 8  *****************
 * User: Sonny        Date: 10/01/05   Time: 10:42
 * Updated in $/xGate
 * Fixed crash on ConferenceRoom::FreeDevice when device is not found
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
 * User: Junanto      Date: 24/04/01   Time: 18:25
 * Updated in $/xGate
 * first link between Prosody/Aculab Media and switching framework
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
#include "voicedevice.h"


/*----------------------------------------------------------------* 
 * Implementation of VoiceDevice
 *----------------------------------------------------------------*/

VoiceDevice::VoiceDevice(const char *name,SwitchDevice *swdev) :
    SupportSwitching(swdev)
{
    _name = name;
    _task = NULL;
    _output_dB = 0;
    _output_agc = 0;
    _input_dB = 0;
    _input_agc = 0;
}

VoiceDevice::~VoiceDevice()
{
}

const char *VoiceDevice::GetName()
{
    return _name.c_str();
}

CTask *VoiceDevice::GetTask()
{
    return _task;
}

void VoiceDevice::AttachTask(CTask *task)
{
    assert(!_task);
    _task = task;
}

void VoiceDevice::DetachTask()
{
    assert(_task);
    _task = NULL;
}

/*----------------------------------------------------------------* 
 * Implementation of ConferenceRoom
 *----------------------------------------------------------------*/

void ConferenceRoom::InsertDevice(VoiceDevice* dev) {
    conf_device* conf_dev  = new conf_device;
    conf_dev->_device = dev;
    _conf_devices.push_back(conf_dev); 
};

void ConferenceRoom::AttachTask(CTask* task) {
}

bool ConferenceRoom::DetachTask(CTask* task) {
    List<conf_device*>::iterator it;
    conf_device* conf_dev;
    VoiceDevice* dev;
    bool all_devices_free = true;
    bool task_is_owner = _task == task;

    if (task_is_owner) {
	 // the owner is detached, so there's no owner for this room anymore
	_task = NULL;
    }

    for (it = _conf_devices.begin(); it != _conf_devices.end(); it++) {
	conf_dev = *it;
	dev = conf_dev->_device;
	if (dev->GetTask() == task) {
	    dev->GetSwitchDevice()->DisconnectListeners();
	    dev->GetSwitchDevice()->Unlisten();
	    dev->DetachTask();
	    if ((*it)->_conferenced) 
		RemoveParty(dev);
	    conf_dev->_allocated = false;
	}
	if (conf_dev->_allocated) 
	    all_devices_free = false;
    }

    // room will be deleted if all devices are free and theres no owner task
    return all_devices_free && (task_is_owner || _task == NULL);
}

VoiceDevice* ConferenceRoom::GetDevice()
{
    List<conf_device*>::iterator it;
    conf_device* conf_dev;

    for (it = _conf_devices.begin(); it != _conf_devices.end(); it++) {
	conf_dev = *it;
	if (!conf_dev->_allocated) {
	    conf_dev->_allocated = true;
	    return conf_dev->_device;
	}
    }
    CLog::Detail("ConferenceRoom", "Room %s: cannot get conference device", _name.c_str());
    return NULL;
}

STATUS ConferenceRoom::FreeDevice(VoiceDevice* dev)
{
    List<conf_device*>::iterator it;
    conf_device* conf_dev;

    for (it = _conf_devices.begin(); it != _conf_devices.end(); it++) {
	conf_dev = *it;
	if (conf_dev->_device == dev) {
	    conf_dev->_allocated = false;
	    dev->GetSwitchDevice()->DisconnectListeners();
	    dev->GetSwitchDevice()->Unlisten();
	    dev->DetachTask();
	    RemoveParty(dev);
	    return OK;
	}
    }
    CLog::Error("ConferenceRoom", "Room %s: free device failed (not found)", _name.c_str());
    return NOT_OK;
}


/*----------------------------------------------------------------* 
 * Implementation of EchoCanceller
 *----------------------------------------------------------------*/

void EchoCanceller::AttachTask(CTask* task) {
    _task = task;
}

bool EchoCanceller::DetachTask(CTask* task) {
    return _task == task;
}

