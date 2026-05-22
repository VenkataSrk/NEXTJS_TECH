/* $Header: /xGate/termcmd.cpp 11    3/01/03 7:03p Bennylp $
 *   
 * CTENGINE
 * (C) 2001 Switchlab, Ltd
 *
 * Terminal command.
 *
 * $Log: /xGate/termcmd.cpp $
 * 
 * 11    3/01/03 7:03p Bennylp
 * Make BugSlayer happy
 * 
 * 10    9/18/02 11:57a Bennylp
 * Fixed compilation errors on MSVC.NET
 * 
 * 9     6/30/01 11:34p Bennylp
 * Fixed memory leak is some cases with dynamic arguments.
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
#include "stdafx.h"
#include "termcmd.h"
#include "cfg.h"

///////////////////////////////////////////////////////////////////////////////
Term_Cmd_Arg::Term_Cmd_Arg(const String& name, 
		   const CVariant& type, 
		   const CVariantValidator& validator,
		   Terg_Arg_Status status)
: m_Name(name), m_Value(type), m_Validator(validator.Clone()), m_Status(status),
  m_IsSet(false)
{
    assert (m_Validator);
}

Term_Cmd_Arg::~Term_Cmd_Arg()
{
    delete m_Validator;
}

Term_Cmd_Arg::Term_Cmd_Arg(const Term_Cmd_Arg& rhs)
: m_Name(rhs.m_Name), m_Value(rhs.m_Value), m_Desc(rhs.m_Desc), m_Status(rhs.m_Status), 
  m_Validator(rhs.m_Validator->Clone()), m_IsSet(rhs.m_IsSet)
{
}

const Term_Cmd_Arg& Term_Cmd_Arg::operator =(const Term_Cmd_Arg& rhs)
{
    if (this == &rhs) {
	assert(false);
	return *this;
    }

    m_Name=rhs.m_Name;
    m_Value=rhs.m_Value;
    m_Desc=rhs.m_Desc;
    m_Status=rhs.m_Status;
    delete m_Validator;
    m_Validator=rhs.m_Validator->Clone();
    m_IsSet=rhs.m_IsSet;

    return *this;
}

bool Term_Cmd_Arg::GetValue(CVariant &value) const
{
    if (m_IsSet || m_Status==TermArgOptional) {
	value=m_Value;
	return true;
    } else {
	return false;
    }
}

bool Term_Cmd_Arg::Validate(const char* str_value, size_t, CVariant& value) const
{
    if (CVariant::Factory(value, m_Value.Type(), str_value))
	return false;

    return m_Validator->Validate(value);
}

bool Term_Cmd_Arg::Suggest(const char* str_value, size_t, CVariant& value) const
{
    if (CVariant::Factory(value, m_Value.Type(), str_value))
	return false;

    return m_Validator->Suggest(value);
}

void Term_Cmd_Arg::SetValue(const CVariant& value)
{
    m_Value=value;
    m_IsSet=true;
}


///////////////////////////////////////////////////////////////////////////////
Term_Cmd::Term_Cmd(Term_Cmd_Target *target, 
		   int identification, const String& cmd_name, const String& desc, 
		   PrivilegeLevel l, unsigned term_modes, bool is_no_valid)
: m_Target(target), m_Id(identification), m_Name(cmd_name), m_Desc(desc), m_PrivilegeLevel(l),
  m_TermMode(term_modes), m_AttachedEntry(NULL), isNoValid(is_no_valid)
{
    /* To make BugSlayer happy. */
    memset(m_DynArgReference, 0, sizeof(m_DynArgReference));
}

Term_Cmd::~Term_Cmd()
{
    // delete static arguments.
    unsigned i;
    for (i=0; i<m_Args.size(); ++i) {
	// don't delete if we have dynamic arguments.
	if (m_DynArgs.size() <= i || m_DynArgs[i].empty())
	    delete m_Args[i];
    }
    m_Args.clear();

    // delete dynamic arguments.
    for (i=0; i<m_DynArgs.size(); ++i) {
	DynArgEntry_A & a = m_DynArgs[i];
	if (a.size()) {
	    for (unsigned j=0; j<a.size(); ++j)
		delete a[j].second;
	}
    }
    m_DynArgs.clear();
}

Term_Cmd::Term_Cmd(const Term_Cmd& x)
: m_Target(x.m_Target), m_Id(x.m_Id), m_Name(x.m_Name), m_Desc(x.m_Desc), 
  m_PrivilegeLevel(x.m_PrivilegeLevel), m_TermMode(x.m_TermMode), 
  m_AttachedEntry(x.m_AttachedEntry), isNoValid(x.isNoValid)
{
    assert (false);
    memcpy(m_DynArgReference, x.m_DynArgReference, sizeof(m_DynArgReference));
}

const Term_Cmd& Term_Cmd::operator=(const Term_Cmd & rhs)
{
    if (this == &rhs) {
	assert(false);
	return *this;
    }

    m_Target = rhs.m_Target;
    m_Id = rhs.m_Id;
    m_Name = rhs.m_Name;
    m_Desc = rhs.m_Desc;
    m_PrivilegeLevel = rhs.m_PrivilegeLevel;
    m_TermMode = rhs.m_TermMode;
    m_AttachedEntry = rhs.m_AttachedEntry;
    isNoValid = rhs.isNoValid;
    memcpy(m_DynArgReference, rhs.m_DynArgReference, sizeof(m_DynArgReference));

    // Delete static arguments.
    while (m_Args.size()) {
	delete m_Args.back();
	m_Args.pop_back();
    }
    assert (m_Args.empty());
    // Assign static arguments from parameter
    unsigned i, size;
    for (i=0, size=rhs.m_Args.size(); i<size; ++i) {
	Term_Cmd_Arg *copy_arg = rhs.m_Args[i];
	m_Args.push_back(NULL);
	if (copy_arg)
	    m_Args[i] =  new Term_Cmd_Arg(*copy_arg);
	else
	    m_Args[i] = NULL;
    }

    // Delete dynamic arguments
    while (m_DynArgs.size()) {
	DynArgEntry_A & a = m_DynArgs.back();
	while (a.size()) {
	    DynArgEntry_T & e = a.back();
	    delete e.second;
	    a.pop_back();
	}
	assert(a.empty());
	m_DynArgs.pop_back();
    }
    assert(m_DynArgs.empty());
    // Assign dynamic arguments from parameter
    for (i=0,size=rhs.m_DynArgs.size(); i<size; ++i) {
	m_DynArgs.push_back(DynArgEntry_A());
	DynArgEntry_A & a = m_DynArgs[i];
	const DynArgEntry_A & rhs_a = rhs.m_DynArgs[i];
	for (unsigned j=0; j<rhs_a.size(); ++j) {   
	    const DynArgEntry_T & e = rhs_a[j];
	    if (e.second)
		a.push_back(std::make_pair(e.first, new Term_Cmd_Arg(*e.second)));
	}
    }
    
    return *this;
}

void Term_Cmd::Add(Term_Cmd_Arg * arg)
{
    m_Args.push_back(arg);
}


/**
 Add dynamic argument to this command.
 */
bool Term_Cmd::AddDynamic ( size_t index,	    // the index to where this argument is added.
			    size_t bound_index,
			    CVariant bound_value,   // the value of the reference argument
			    Term_Cmd_Arg *arg)	    // the argument.
{
    assert (arg);
    assert (index && index <= 20); // index must be > 0

    if (index > 20) {
	// do you really have this many arguments ??
	return false;
    }

    // Insert values in the m_DynArgs if necessary
    while (m_DynArgs.size() <= index)
	m_DynArgs.push_back(DynArgEntry_A());

    // Insert a new entry in the m_DynArgs[index]
    DynArgEntry_T dyn_arg = std::make_pair(bound_value, arg);
    DynArgEntry_A & dyn_arglist = m_DynArgs[index];
    dyn_arglist.push_back(dyn_arg);

    // Now insert entries in the m_Args if necessary.
    while (m_Args.size() <= index)
	m_Args.push_back(NULL);

    m_DynArgReference[index] = bound_index;
    return true;
}


/** Start the processing of argument number n.
 */
bool Term_Cmd::StartArgument(size_t n)
{
    if (n > 0 && m_DynArgs.size() > n) {
	// Get the vector to dynamic argument n
	DynArgEntry_A & dyn_arglist = m_DynArgs[n];
	if (dyn_arglist.size()) {
	    // Get the value of the bound argument.
	    size_t bound_index = m_DynArgReference[n];
	    CVariant bound_value;

	    if (m_Args[bound_index]->HasValue())
		m_Args[bound_index]->GetValue(bound_value);

	    // Find the dynamic argument that matches the bound value.
	    unsigned i, size=dyn_arglist.size();
	    for (i=0; i<size; ++i) {
		DynArgEntry_T & dyn_arg = dyn_arglist[i];
		if (dyn_arg.first == bound_value) {
		    m_Args[n] = dyn_arg.second;
		    return true;
		}
	    }

	    // The specified value not found.
	    // Use the first value to protect against application trying to
	    // access this argument.
	    if (i==size) {
		if(!m_Args[n]) {
		m_Args[n] = dyn_arglist[0].second;
		return false;
	    }
		return true;
	    }
	} else
	    assert (m_Args[n]);	// arg-n must be a static argument.
    }
    return true;
}


Term_Cmd_Arg *Term_Cmd::GetArg(unsigned int n)
{
    if (n>m_Args.size())
	return NULL;
    return m_Args[n];
}

const Term_Cmd_Arg *Term_Cmd::GetArg(unsigned int n) const 
{
    if (n>m_Args.size())
	return NULL;
    return m_Args[n];
}

/*
Term_Cmd_Arg *Term_Cmd::GetArg(const char*name)
{
    for (unsigned int i=0; i<m_Args.size(); ++i)
	if (!stricmp(m_Args[i]->GetName().c_str(), name))
	    return m_Args[i];

    return NULL;
}

const Term_Cmd_Arg *Term_Cmd::GetArg(const char*name) const 
{
    for (unsigned int i=0; i<m_Args.size(); ++i)
	if (!stricmp(m_Args[i]->GetName().c_str(), name))
	    return m_Args[i];

    return NULL;
}
*/

void Term_Cmd::AttachToEntry (Cfg_Tree_Entry *entry)
{
    entry->AttachCommand(this);
    m_AttachedEntry = entry;
}

void Term_Cmd::DetachFromEntry()
{
    if (m_AttachedEntry) {
	m_AttachedEntry->DetachCommand(this);
	m_AttachedEntry = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////
Term_Cmd_Target::Term_Cmd_Target()
{
    _signature = DEADCODE;
}

Term_Cmd_Target::~Term_Cmd_Target()
{
    while (m_Cmds.size()) {
	m_Cmds.back()->DetachFromEntry();
	delete m_Cmds.back();
	m_Cmds.pop_back();
    }
    _signature = ~DEADCODE;
}

void Term_Cmd_Target::RegisterTermCmd(Term_Cmd * cmd, Cfg_Tree_Entry *entry)
{
    m_Cmds.push_back(cmd);
    cmd->AttachToEntry (entry);
}

const Term_Cmd * Term_Cmd_Target::FindCommand(int id)
{
    List<Term_Cmd*>::iterator it;
    for (it=m_Cmds.begin(); it!=m_Cmds.end(); ++it) {
	if ((*it)->GetId() == id)
	    return *it;
    }
    return NULL;
}

