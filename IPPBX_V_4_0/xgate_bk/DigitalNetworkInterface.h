#ifndef DIGITALNETWORKINTERFACE_H
#define DIGITALNETWORKINTERFACE_H

#include "callmanager.h"
#include "MediaManager.h"
#include "SignallingDriver.h"

#include <commonutil.h>	//linux porting

#define MAX_TIMESLOT_IN_INTERFACE   32

/** Interface that represent the Digital Network Telephony
All Digital network telophony interface shoud be inherited from this one
*/
class DigitalNetworkInterface : public Interface
{
public:
    explicit DigitalNetworkInterface(const String& name, bool needPoll=false)
	:Interface(ITF_E1_T1,// InterfaceType name
		   name.c_str(),// Interface name
		   needPoll)	// Need a pool
	, _timeslotmask(0)
	, _interfacegroup(0)
    {}
    
    /** Block Timeslot*/
    virtual void BlockTimeslot(int timeslot,bool blocked) = 0;

    /** Get E1/T1 group for this interface  */
    inline int GetInterfaceGroup() {return _interfacegroup;}
    
    /** Get Timeslot Mask for this interface */
    inline UINT32 GetTimeslotMask(){return _timeslotmask;}

protected:
    /** Time slot mask related to the interface*/
    UINT32 _timeslotmask;

    /** Digital network interface group*/
    int _interfacegroup;
};

/** Media object that represent timeslot for each Digital Interface*/
class DigitalNetworkMedia: public Media
{
public:
    DigitalNetworkMedia(const char* tsdevname, int ts,Interface* pitf,SwitchDevice* swdev)
	:Media(tsdevname,(MediaId) ts,pitf,swdev)
	, _blocked(false)

    {}

    inline int GetTimeSlot(){return (int)_id;}
    bool IsBlocked(){return _blocked;}
    void SetBlocked(bool blocked){_blocked = blocked;}
private:
    bool _blocked;
};

#endif
