/*----------------------------------------------------------------* 
 * $Archive: /xGate/SignallingDriver.h $
 * $Date: 7/14/01 5:23p $
 * $Revision: 10 $
 *
 * $History: SignallingDriver.h $
 * 
 * *****************  Version 10  *****************
 * User: Bennylp      Date: 7/14/01    Time: 5:23p
 * Updated in $/xGate
 * Framework for signalling driver configuration in SigDrvManager.
 * 
 * *****************  Version 9  *****************
 * User: Junanto      Date: 24/06/01   Time: 20:08
 * Updated in $/xGate
 * Changed AttachInterface to return bool
 * 
 * *****************  Version 8  *****************
 * User: Bennylp      Date: 6/22/01    Time: 10:25p
 * Updated in $/xGate
 * Added new signalling type for AudioCodes.
 * 
 * *****************  Version 7  *****************
 * User: Bennylp      Date: 5/23/01    Time: 8:14p
 * Updated in $/xGate
 * Added new signalling type constant (ATM).
 * 
 * *****************  Version 6  *****************
 * User: Junanto      Date: 29/04/01   Time: 17:35
 * Updated in $/xGate
 * added support for pipeline signalling
 * 
 * *****************  Version 5  *****************
 * User: Bennylp      Date: 4/23/01    Time: 10:44p
 * Updated in $/xGate
 * Fixed std namespace conflicts.
 * 
 * *****************  Version 4  *****************
 * User: Junanto      Date: 18/04/01   Time: 11:51
 * Updated in $/ct15
 * 
 * *****************  Version 3  *****************
 * User: Bennylp      Date: 4/12/01    Time: 9:00p
 * Updated in $/ct15
 * Integration and some bug fixes.
 * 
 * *****************  Version 2  *****************
 * User: Junanto      Date: 11/04/01   Time: 11:37
 * Updated in $/ct15
 * 
 * *****************  Version 1  *****************
 * User: Junanto      Date: 10/04/01   Time: 18:12
 * Created in $/ct15
 * 
 * *****************  Version 2  *****************
 * User: Junanto      Date: 8/04/01    Time: 19:44
 * Updated in $/ct2
 * 
 * *****************  Version 1  *****************
 * User: Junanto      Date: 4/04/01    Time: 17:40
 * Created in $/ct2
 * 
 *----------------------------------------------------------------*/

/*----------------------------------------------------------------* 
 * Description:
 *  
 *----------------------------------------------------------------*/

#if !defined(CT2_SIGNALLINGDRIVER_H__INCLUDED_)
#define CT2_SIGNALLINGDRIVER_H__INCLUDED_

#if _MSC__VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "call.h"
#include "MediaManager.h"
#include "serviceprovider.h"


/** Forward declaration
 */
class Interface;
class Signalling_Driver_Validator;


/** SignallingType:
 */
enum SignallingType
{
    SigType_Aculab,
    SigType_ISUP,
    SigType_H323,
    SigType_SIP,
	SigType_SOFIA,
    SigType_DialogicAnalog,
    SigType_DialogicDigital,
    SigType_Pipeline,
    SigType_ATM,
    SigType_Q931oIP,
    SigType_BSSAP
};


/** SignallingDriver: This abstract class serves as base class for all 
    signalling drivers
 */
class SignallingDriver : public ServiceProvider
{
public:
    virtual ~SignallingDriver();

    /** SetupReq: Initiate a new outgoing call in a specific Interface
	using the specified call parameter
     */
    virtual Call *SetupReq(Interface *itf,SelectionStrategy selection,CallParameter *cparam) = 0;
    virtual Call *SetupReq(CTask *task,Interface *itf,SelectionStrategy selection,CallParameter *cparam) 
    {
        cparam_d = cparam;
	return SetupReq(itf,selection,cparam);
    }

    /** CreateDefaultCallParameter: Returns a CallParameter structure containing
	default values for this signalling protocol
     */
    virtual CallParameter *CreateDefaultCallParameter() = 0;

    /** AttachInterface: A new Interface object is attached to this 
	SignallingDriver object
     */
    virtual bool AttachInterface(Interface *itf) = 0;

    /** DetachInterface: Remove association between a Interface object and this 
	SignallingDriver object
     */
    virtual void DetachInterface(Interface *itf) = 0;

    /** DisableInterface: an interface has been disabled, the signalling
	driver should take the necessary action if it can
     */
    virtual bool DisableInterface(Interface *itf) {
	return false;
    }

    /** EnableInterface: an interface has been enabled, the signalling
	driver should take the necessary action if it can
     */
    virtual bool EnableInterface(Interface *itf) {
	return false;
    }

    /** Get the signalling type.
     */
    SignallingType GetType() {
	return _type;
    }

    /** Get the description.
     */
    const String & GetDescription() const {
	return _description;
    }

    /** Get the configuration tree.
     */
    Cfg_Tree_Group *GetConfigTree() {
	return _configTree;
    }

protected:
    CallParameter *cparam_d;
    /** Constructor.
     */
    SignallingDriver(SignallingType type,
		     const char *name,
		     const char *description,
		     bool needs_polling,
		     Priority priority=Normal_Priority);

private:
    SignallingType   _type;
    Cfg_Tree_Group  *_configTree;
    String	     _description;
};


/** Singleton which keeps the list of all signalling driver
 */
class SignallingDriverManager : public ServiceProvider
{
public:
    ~SignallingDriverManager();

    /** Create the singleton instance.
     */
    static SignallingDriverManager *Create();

    /** Get the singleton instance of this class.
     */
    static SignallingDriverManager *Instance() {
	return _instance;
    }

    /** Finds a signalling driver.
     */
    SignallingDriver *FindDriverByName(const char *name);

    /** Register a new signalling driver.
     */
    void InsertSignallingDriver(SignallingDriver *sigdrv);

    /** Unregister a signalling driver.
     */
    void RemoveSignallingDriver(SignallingDriver *sigdrv);

    /** Get the configuration tree.
     */
    Cfg_Tree_Group *GetConfigTree() {
	return _configTree;
    }

    /** Initialize.
     */
    virtual bool Load() {
	return true;
    }

    /** Deinitialize.
     */
    virtual bool Unload() {
	return true;
    }

    /** Terminal command callback.
     */
    virtual void DoTermCmd(Term_Session *, bool no, Term_Cmd *);


    /** Show configuration for signalling entries.
     */
    virtual bool OnShowConfig(
	const Cfg_Tree_Entry *entry,
	String & output, 
	const String & indent, 
	int indent_size) const;

private:
    typedef List<SignallingDriver*> ListSignallingDriver;

    ListSignallingDriver _listDriver;
    Cfg_Tree_Group	*_configTree;
    List<ValidatorChoice> _sigNameList;

    static SignallingDriverManager *_instance;

    // private constructor
    SignallingDriverManager();

    friend class Signalling_Driver_Validator;
};


#endif // !defined(CT2_SIGNALLINGDRIVER_H__INCLUDED_)
