/* $Header: /xGate/termcmd.h 8     6/21/01 6:58p Bennylp $
 *   
 * CTENGINE
 * (C) 2001 Switchlab, Ltd
 *
 * Terminal command.
 *
 * $Log: /xGate/termcmd.h $
 * 
 * 8     6/21/01 6:58p Bennylp
 * Support dynamic arguments.
 * 
 * 7     4/23/01 10:47p Bennylp
 * Fixed std namespace conflicts.
 * 
 * 6     4/12/01 8:08p Bennylp
 * Integration.
 *
 */
#ifndef __TERM_CMD__
#define __TERM_CMD__

#include "types.h"
#include "variant.h"
#include "security.h"
///#include "String.h"


class Term_Session;
class Term_Cmd_Target;
class Term_Cmd;
class Cfg_Tree_Entry;


///////////////////////////////////////////////////////////////////////////////
/**
  Terminal operation mode.
 */
enum Term_Mode {
    TermModeLogin=1,	// login mode (don't echo, don't put in history, disable ext. keys, etc)
    TermModeNormal=2,	// disable write access to configuration
    TermModeConfig=4,	// enable write access to configuration
    TermModeInit=8,	// enable write access to config, by pass config read-only flag
    TermModeAllConfig=TermModeConfig+TermModeInit,
    TermModeAll=TermModeNormal+TermModeConfig+TermModeInit
};

///////////////////////////////////////////////////////////////////////////////
/** 
  */
enum Terg_Arg_Status {
    TermArgRequired,	// the argument is required; if it's missing, error
    TermArgOptional,	// the argument is optional
};


///////////////////////////////////////////////////////////////////////////////
/**
 Individual argument to terminal command
 */
class Term_Cmd_Arg {
public:
    /**
     Create a command argument.
     Command argument will be added to Term_Cmd later.
     */
    Term_Cmd_Arg(
	const String& name,		    // name of the argument
	const CVariant& type,		    // type of the argument, also used as default value
	const CVariantValidator&,	    // descendant of CVariantValidator
	Terg_Arg_Status status=TermArgRequired// status whether required or optional
    );

    /** Destructor */
    ~Term_Cmd_Arg();

    /** 
     Get the argument name 
     */
    const String& GetName() const { 
	return m_Name; 
    }

    /** 
      Get the argument description (help) 
     */
    const String& GetDesc() const { 
	return m_Desc; 
    }

    /** 
     Get the argument status (whether this argument is required or optional) 
    */
    Terg_Arg_Status GetStatus() const { 
	return m_Status; 
    }

    /**
     Determine whether the value of this argument has been set.
     For optional arguments, upon executing the command, the argument's value
     might not be present.
     */
    bool HasValue() const {
	return m_IsSet;
    }

    /**
     Get the argument's value. The function will return true if the argument contains a valid
     value (i.e. supplied by the user, or supplied by the system if the argument is
     optional.

     @return
     true if the argument contains valid value
     false if the argument is not set
     */
    bool GetValue(CVariant&) const;

    /**
     Validate a string according to values accepted by this argument.
     This function will try to create a variant object according to the 
     argument type, then ask the validator to investigate whether the value
     can be accepted.

     @return
     true if the value is ok (the <code>result</code> argument is also set)
     false if the value is NOT ok (the <code>result</code> argument's value is undefined)
     */
    bool Validate(const char*, size_t len, CVariant& result) const;

    /**
     Suggest a value according to the clue supplied as the string argument.
     This function will try to create a variant object according to the 
     argument type, then ask the validator for value suggestion.

     @return
     true if a suggestion was generated
     false if a suggestion can NOT be generated
     */
    bool Suggest(const char*, size_t len, CVariant& result) const;

    /**
      Set the value of the argument
      */
    void  SetValue(const CVariant& value);

    /**
      Get the validator for this argument
      */
    const CVariantValidator& GetValidator() const { return *m_Validator; }

private:
    String	m_Name;
    CVariant	m_Value;
    String	m_Desc;
    Terg_Arg_Status m_Status;
    CVariantValidator *m_Validator;
    bool	m_IsSet;

    /** Not accessible: copy constructor */
    Term_Cmd_Arg(const Term_Cmd_Arg& rhs);

    /** Not accessible: assignment operator */
    const Term_Cmd_Arg& operator =(const Term_Cmd_Arg&);

    friend class Term_Cmd;
};


///////////////////////////////////////////////////////////////////////////////
/**
 Terminal command.
 */
class Term_Cmd {
public:
    /** Create a command.
     */
    Term_Cmd(
	Term_Cmd_Target *target,	// target of this command
	int id,			// integer for later identification upon executing Term_Cmd_Target::DoTermCmd
	const String& cmd_name,	// name of the command
	const String& desc,	// description
	PrivilegeLevel priv,	// privilege level required to execute
	unsigned term_mode,	// combinations of Term_Mode on which this cmd valid to execute
	bool is_no_valid=true	// specify whether user can use 'no' command prefix.
    );

    ~Term_Cmd();

    /**
     Add an argument to this command. The order of the argument
     is important. The argument SHALL NOT be deleted by caller after calling
     this function, because this class will take care of that.
     */
    void Add(
	Term_Cmd_Arg *	    // terminal argument
    );

    /**
     Add dynamic argument to this command.
     */
    bool AddDynamic (
	size_t index,		// the index to where this argument is added.
	size_t bound_index,	// index to the referenced argument
	CVariant bound_value,	// the value of the referenced argument
	Term_Cmd_Arg *arg	// the argument.
    );

    /** Get the target.
     */
    Term_Cmd_Target * GetTarget() {
	return m_Target;
    }

    /**
     Get command identifier.
     */
    int GetId() const { 
	return m_Id; 
    }

    /**
     Determine whether 'no' command prefix is valid for this command.
     */
    bool IsNoValid() const {
	return isNoValid;
    }

    /**
     Get command name
     */
    const String& GetName() const { 
	return m_Name; 
    }

    /**
     Get command description/help
     */
    const String& GetDesc() const { 
	return m_Desc; 
    }

    /** Get number of arguments */
    int GetArgCount() const { 
	return m_Args.size(); 
    }

    /** Get argument number <code>num</code> */
    Term_Cmd_Arg* GetArg(
	unsigned int num    // argument index (zero based)
    );

    /** Get argument number <code>num</code> */
    const Term_Cmd_Arg* GetArg(
	unsigned int num    // argument index (zero based)
    ) const;

    /**
     Get argument with specified name, or NULL if the argument is not
     found.
     */
    /*
    Term_Cmd_Arg* GetArg(
	const char*name	    // argument name
    );
    */

    /**
     Get argument with specified name, or NULL if the argument is not
     found.
     */
    /*
    const Term_Cmd_Arg* GetArg(
	const char*name	    // argument name
    ) const;
    */

    /**
      Get privilege level required to execute this command 
      */
    PrivilegeLevel GetPrivilegeLevel() const { 
	return m_PrivilegeLevel; 
    }

    /** 
      Investigate whether the command valid to execute in specified mode 
      */
    bool HasMode(Term_Mode m) const { 
	return (m_TermMode & (unsigned)m) != 0;
    }

    /** Copy.
     */
    void CopyTo (Term_Cmd & c) {
	c.operator = (*this);
    }

    /** Start the processing of argument number n.
     */
    bool StartArgument(size_t n);

    /**
      Attach command to an entry.
     */
    void AttachToEntry (Cfg_Tree_Entry *entry);

    /**
      Detach command from an entry.
     */
    void DetachFromEntry();

private:
    Term_Cmd_Target *m_Target;
    int m_Id;
    String m_Name;
    String m_Desc;
    Vector<Term_Cmd_Arg*> m_Args;
    PrivilegeLevel m_PrivilegeLevel;
    unsigned m_TermMode;
    Cfg_Tree_Entry *m_AttachedEntry;
    bool isNoValid;

    typedef std::pair<CVariant,Term_Cmd_Arg*> DynArgEntry_T;
    typedef Vector <DynArgEntry_T> DynArgEntry_A;
    Vector <DynArgEntry_A> m_DynArgs;
    unsigned m_DynArgReference[32];

    /** Not permitted: Copy constructor */
    Term_Cmd(const Term_Cmd& );

    /** Not permitted: Assignment */
    const Term_Cmd& operator=(const Term_Cmd&);

};


/**
 Terminal command target, base class for classes that want's to be able
 to accept terminal commands.
 */
class Term_Cmd_Target {
public:
    Term_Cmd_Target();

    /**
     Reqister a command. The command should not be deleted by caller after
     calling this function, because this class will automatically delete
     the command upon self-destruction.
     */
    void RegisterTermCmd(Term_Cmd *, Cfg_Tree_Entry *entry);

    /**
     Find a command.
     */
    const Term_Cmd * FindCommand(int id);

    /**
     Terminal command callback
     */
    virtual void DoTermCmd(Term_Session *, bool no, Term_Cmd *) {
	// This instance has registered a command, but failed to implement
	// this function correctly.
	assert(false);
    }

    /**
     Show configuration for the specified entry.
     This function will be called by the framework whenever it needs to display the
     specified entry. Previously this instance should tell the entry that it wishes to
     handle displaying the entry by calling entry->RegisterShowHandler().

     When this instance is deleted, it should call entry->UnregisterShowHandler()
     so that the handler will not be called (which will cause GPF).
     */
    //virtual STATUS OnShowConfig(
    virtual bool OnShowConfig(
	const Cfg_Tree_Entry *entry,
	String & output, 
	const String & indent, 
	int indent_size) const 
    {
	// This instance has registered with entry->RegisterShowHandler(), but
	// failed to implement this function correctly.
	assert(false);
	return NOT_OK;
    }

    /**
      Get the List of commands
      */
    const List <Term_Cmd*>& GetCmdList() const { return m_Cmds; };
    bool IsValid() 
    {
	try {
	    return (_signature == DEADCODE);
	} catch (...) {
	    return false;
	}
    }
protected:
    /**
     Should never delete this direcly.
     */
    ~Term_Cmd_Target();

private:
    List <Term_Cmd*> m_Cmds;

    /** Copy constructor is prohibited. */
    Term_Cmd_Target( const Term_Cmd_Target & rhs) {
	assert(false);
    }
    /** Assignment is prohibited. */
    const Term_Cmd_Target & operator = (const Term_Cmd_Target & rhs) {
	assert(false);
	return *this;
    }
    enum { DEADCODE = 0xDEADC0DE };
    unsigned _signature;
};


#endif	/* __TERM_CMD__ */
