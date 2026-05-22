/*----------------------------------------------------------------* 
 * $Archive: /xGate/SignallingDriver.cpp $
 * $Date: 7/14/01 5:23p $
 * $Revision: 7 $
 *
 * $History: SignallingDriver.cpp $
 * 
 * *****************  Version 7  *****************
 * User: Bennylp      Date: 7/14/01    Time: 5:23p
 * Updated in $/xGate
 * Framework for signalling driver configuration in SigDrvManager.
 * 
 * *****************  Version 6  *****************
 * User: Junanto      Date: 21/04/01   Time: 23:05
 * Updated in $/xGate
 * shorten external name to SigDrv
 * 
 * *****************  Version 5  *****************
 * User: Junanto      Date: 18/04/01   Time: 11:51
 * Updated in $/ct15
 * 
 * *****************  Version 4  *****************
 * User: Bennylp      Date: 4/12/01    Time: 9:00p
 * Updated in $/ct15
 * Integration and some bug fixes.
 * 
 * *****************  Version 3  *****************
 * User: Junanto      Date: 12/04/01   Time: 19:55
 * Updated in $/ct15
 * 
 * *****************  Version 2  *****************
 * User: Junanto      Date: 11/04/01   Time: 11:37
 * Updated in $/ct15
 * 
 * *****************  Version 1  *****************
 * User: Junanto      Date: 10/04/01   Time: 18:12
 * Created in $/ct15
 * 
 * *****************  Version 1  *****************
 * User: Junanto      Date: 4/04/01    Time: 17:40
 * Created in $/ct2
 * 
 * *****************  Version 1  *****************
 * User: Junanto      Date: 4/04/01    Time: 16:02
 * Created in $/ct2
 * Created
 * 
 *----------------------------------------------------------------*/

/*----------------------------------------------------------------* 
 * Description:
 * It is a thin layer between Call Manager and the actual hardware. 
 * It translates the standard interface understood by the Call Manager 
 * to the protocol required by the hardware. Signalling Driver controls 
 * media allocation in the case of incoming call, or when the driver 
 * supports media allocation signalling such as block request in ISUP.
 *----------------------------------------------------------------*/

#include "stdafx.h"
#include "SignallingDriver.h"
#include "log.h"
#include "kernel.h"
#include "term.h"


static char __modname__[] = "SigDrv";
#define THISMODULE	    __modname__

SignallingDriverManager *SignallingDriverManager::_instance;


//-----------------------------------------------------------------------------
class Signalling_Driver_Validator : public CVariantDynamicChoiceValidator {
public:
    /** Default constructor. */
    Signalling_Driver_Validator()
    {}

    /** Destructor. */
    virtual ~Signalling_Driver_Validator() {}

    /** Copy constructor */
    Signalling_Driver_Validator ( const Signalling_Driver_Validator & rhs)
	: CVariantDynamicChoiceValidator(rhs)
    {}

    /** Assignment operator */
    virtual const Signalling_Driver_Validator & operator =(const Signalling_Driver_Validator & rhs)
    {
	CVariantValidator::operator = (rhs);
	return *this;
    }

    /** Create a duplicate */
    virtual CVariantValidator *Clone() const
    {
	return new Signalling_Driver_Validator(*this);
    }

    /** Acquire mutex lock.
     */
    virtual void AcquireLock() const {
	ACQUIRE_SYSTEM_LOCK();
    }

    /** Release mutex lock.
     */
    virtual void ReleaseLock() const {
	RELEASE_SYSTEM_LOCK();
    }

    /** Get list of choices available.
     */
    virtual const List<ValidatorChoice> & GetChoiceList() const {
	return SignallingDriverManager::Instance()->_sigNameList;
    }
};

/*----------------------------------------------------------------* 
 * Implementation of SignallingDriverManager
 *----------------------------------------------------------------*/

enum {
    Term_Cmd_Signalling = 0,
};

SignallingDriverManager::SignallingDriverManager()
: ServiceProvider("SignallingDriver", false, false)
{
    _configTree = new Cfg_Tree_Group("signalling", "", Kernel::Instance()->RootConfig());
    Kernel::Instance()->RootConfig()->RegisterShowHandler(this);

    Term_Cmd *cmd = new Term_Cmd(this, Term_Cmd_Signalling,
				 "signalling", "Configure signalling",
				 PrivilegeLevelAdmin, TermModeConfig, false);
    cmd->Add (new Term_Cmd_Arg("@0", "", Signalling_Driver_Validator()));
    RegisterTermCmd (cmd, Kernel::Instance()->RootConfig());
}

SignallingDriverManager::~SignallingDriverManager()
{
    delete _configTree;
}

SignallingDriverManager *SignallingDriverManager::Create()
{
    if (!_instance)
	_instance = new SignallingDriverManager;
    return _instance;
}

SignallingDriver *SignallingDriverManager::FindDriverByName(const char *name)
{
    for (ListSignallingDriver::iterator it=_listDriver.begin();it!=_listDriver.end();it++) {
	//if (strcmpi((*it)->Name().c_str(),name) == 0) return *it;
	if (strcmp((*it)->Name().c_str(),name) == 0) return *it;	//linux porting
    }
    return NULL;
}

void SignallingDriverManager::InsertSignallingDriver(SignallingDriver *sigdrv)
{
    CLog::Detail(THISMODULE,"Signalling %s created",sigdrv->Name().c_str());
    _listDriver.push_back(sigdrv);
    _sigNameList.push_back(ValidatorChoice(sigdrv->Name().c_str(), sigdrv->GetDescription()));
}

void SignallingDriverManager::RemoveSignallingDriver(SignallingDriver *sigdrv)
{
    CLog::Detail(THISMODULE,"Signalling %s removed",sigdrv->Name().c_str());
    _listDriver.remove(sigdrv);

    // Remove signalling name from validator list.
    CVariant vname(sigdrv->Name().c_str());
    List<ValidatorChoice>::iterator it, end;
    end = _sigNameList.end();
    for (it=_sigNameList.begin(); it!=end; ++it) {
	CVariant & it_name = it->first;
	if (it_name == vname) {
	    _sigNameList.erase(it);
	    break;
	}
    }
}


/** Terminal command callback.
 */
void SignallingDriverManager::DoTermCmd(Term_Session *session, bool no, Term_Cmd *cmd)
{
    CVariant arg0;

    switch (cmd->GetId()) {
    case Term_Cmd_Signalling:
	cmd->GetArg((size_t)0)->GetValue(arg0);
	ACQUIRE_SYSTEM_LOCK();
	SignallingDriver *drv;
	drv = FindDriverByName(arg0.AsString());

	if (drv == NULL) {
	    session->SendErrorf ("Signalling '%s' not found\n", arg0.AsString());
	} else {
	    session->SetRoot(drv->GetConfigTree());
	}
	RELEASE_SYSTEM_LOCK();
	break;
    }
}


/** Show configuration for signalling entries.
 */
//STATUS SignallingDriverManager::OnShowConfig( const Cfg_Tree_Entry *entry,
bool SignallingDriverManager::OnShowConfig( const Cfg_Tree_Entry *entry,
					      String & output, 
					      const String & indent, 
					      int indent_size) const
{
    return NOT_OK;
}


/*----------------------------------------------------------------* 
 * Implementation of SignallingDriver
 *----------------------------------------------------------------*/

SignallingDriver::SignallingDriver(SignallingType type,
				   const char *name,
				   const char *desc,
				   bool needs_polling,
				   Priority priority) 
: ServiceProvider(name, false, needs_polling ,priority, name, IC_Signalling),
  _type(type),_description(desc)
{
    _configTree = new Cfg_Tree_Group(Name(), _description, SignallingDriverManager::Instance()->GetConfigTree());
    SignallingDriverManager::Instance()->InsertSignallingDriver(this);
}

SignallingDriver::~SignallingDriver()
{
    SignallingDriverManager::Instance()->RemoveSignallingDriver(this);
    delete _configTree;
}
