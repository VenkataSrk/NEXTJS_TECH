/* $Header: /xGate/cfg.cpp 8     2/20/04 10:26a Bennylp $
 *   
 * CTENGINE
 * (C) 2001 Switchlab, Ltd
 *
 * Interfaces for manipulating folders in configuration tree.
 *
 * $Log: /xGate/cfg.cpp $
 * 
 * 8     2/20/04 10:26a Bennylp
 * Added project configuration Hoard and Hoard Debug, to use STLPORT and
 * Hoard memory allocator.
 * 
 * 7     4/05/01 18:46 Junanto
 * Corrected bug when a Cfg_Tree is deleted
 * 
 * 6     5/01/01 4:23a Bennylp
 * Disable warning "this is used in member initialization list"
 * 
 * 5     4/23/01 10:42p Bennylp
 * Fixed std namespace conflicts.
 * 
 * 4     4/12/01 8:00p Bennylp
 * Integration.
 *
 */

#pragma warning(disable: 4786; once: 4786)

#include "stdafx.h"
#include "cfg.h"
#include "log.h"
#include <iostream>

const char *CFG_SEPARATOR = "/";
const char *CFGTOR = "cfg";
const char *NEW_LINE = "\n";
const char *ROOT_ENTRY = "enterprises 1000";
const char *ROOT_OID = "1.3.6.1.4.1.1000";


// construct Cfg_Tree_Entry
Cfg_Tree_Entry::Cfg_Tree_Entry( CfgType type,
		      const String& name,
		      const String& desc,
		      Cfg_Tree_Group_Base *parent,
		      bool use_snmp,
		      int id,
		      unsigned access)
: m_Parent(parent), 
  m_Modified(false),
#ifdef CFG_HAS_SNMP
  m_Mib(NULL),
#endif
  m_Id(id),
  m_Desc(desc),
  m_Access(access), 
  m_Name(name), 
  m_Type(type),
  m_Serializable(true)
{ 
    if (m_Parent) {
	m_Parent->m_Children.push_back(this);
	if (id<=0) {
	    m_Id = m_Parent->m_Children.size();
	}

	m_IsUsingSnmp=use_snmp? m_Parent->IsUsingSnmp() : false;
    } else {
	m_IsUsingSnmp=use_snmp;
    }
}

// destruct Cfg_Tree_Entry
Cfg_Tree_Entry::~Cfg_Tree_Entry()
{ 
    Detach();

    // unregister all commands.
    while (m_Commands.size()) {
	m_Commands.front()->DetachFromEntry();
    }

#ifdef CFG_HAS_SNMP
    /* this will be deleted automatically as long as registered to MIB
    delete m_Mib;
    */
    m_Mib = NULL;
#endif
}


// get the name
const String& Cfg_Tree_Entry::GetName() const
{ 
    return m_Name; 
}


// get the entry type
Cfg_Tree_Entry::CfgType Cfg_Tree_Entry::GetType() const
{
    return m_Type;
}


// remove entry from its parent
void Cfg_Tree_Entry::Detach()
{ 
    if (m_Parent) {
	m_Parent->RemoveEntry(this); 
    }
}


// get the parent
Cfg_Tree_Group_Base* Cfg_Tree_Entry::GetParent() const
{ 
    return m_Parent; 
}



// determine whether the path begins with this entry's name
bool Cfg_Tree_Entry::IsRootOfPath(const String& path) const
{
    //return !GetName().strnicmp(path, GetName().size());
    //return !strnicmp (GetName().c_str(), path.c_str(), GetName().size());
    //return !strnicmp (GetFullPath().c_str(), path.c_str(), GetFullPath().size());
    return !strncasecmp(GetFullPath().c_str(), path.c_str(), GetFullPath().size());	//linux porting
}


// Build a full path name of this entry
String Cfg_Tree_Entry::GetFullPath() const
{
    String full_path;

    if (m_Parent){
	String ss = m_Parent->GetFullPath();
	full_path =  ss + String(CFG_SEPARATOR) + GetName();
    } else {
	full_path = String(CFG_SEPARATOR) + GetName();
    };

    return full_path;
}


// Build full path to be used as Snmp name
#ifdef CFG_HAS_SNMP
String Cfg_Tree_Entry::GetFullSnmpName() const
{
    String full_snmp_name;

    if (m_Parent){
	String ss = m_Parent->GetFullSnmpName();
	full_snmp_name =  ss + String("_") + GetName();
    } else {
	full_snmp_name = GetName();
	full_snmp_name.tolower();
    };

    return full_snmp_name;
}

const char *Cfg_Tree_Entry::GetSnmpSyntaxString(CVariant::VType type)
{
    switch(type) {
    case CVariant::VT_INT:
	return "INTEGER";
    case CVariant::VT_STRING:
    case CVariant::VT_FLOAT:
    case CVariant::VT_BINARY:
	return "DisplayString";
    default:
	return "DisplayString";
    }
}


STATUS Cfg_Tree_Entry::ExportMibHeader(std::ostream& os)
{
    // header
    os << "-- Mib definition" << endl;
    os << "--" << endl;

    os << CFGTOR << " DEFINITIONS ::= BEGIN" << endl;
    os << endl;
    os << "--    IMPORTS" << endl;
    os << "--         OBJECT-TYPE, enterprises" << endl;
    os << "--    FROM RFC1065-SMI;" << endl;
    os << endl;
    os << "    DisplayString ::= OCTET STRING" << endl;
    os << endl;
    return OK;
}


STATUS Cfg_Tree_Entry::ExportMibFooter(std::ostream& os)
{
    os << "END" << endl;
    return OK;
}


STATUS Cfg_Tree_Entry::ExportMib(const Cfg_Tree_Entry &e,
			    std::ostream& os, 
			    bool with_header, 
			    bool with_footer)
{
    assert(e.IsUsingSnmp());

    if (with_header)
	ExportMibHeader(os);

    os << "-- Full path: " << e.GetFullPath().c_str() << endl;
    if (e.GetType()==CfgTypeGroup) {

	os << e.GetFullSnmpName().c_str() 
	   << "  OBJECT IDENTIFIER ::= { ";

    } else {
	if (e.GetType()==CfgTypeLeaf) {

	    os << e.GetFullSnmpName().c_str() 
	       << "    OBJECT-TYPE" 
	       << endl;

	    const Cfg_Tree_Leaf &l=static_cast<const Cfg_Tree_Leaf&>(e);
	    os << "    SYNTAX " 
	       << GetSnmpSyntaxString(l.m_Value.Type()) 
	       << endl;
	    os << "    ACCESS " 
	       << GetSnmpAccessString(e.m_Access) 
	       << endl;

	} else if (e.GetType()==CfgTypeTable) {

	    os << e.GetFullSnmpName().c_str() 
	       << "Table    OBJECT-TYPE" 
	       << endl;
	    os << "    SYNTAX SEQUENCE OF " 
	       << (e.GetFullSnmpName()+"Entry").c_str() 
	       << endl;
	    os << "    ACCESS not-accessible" 
	       << endl;

	} else
	    assert(false);
	
	os << "    STATUS  mandatory" << endl;
	os << "    DESCRIPTION" << endl;
	os << "       \"" << e.GetDesc() << "\"" << endl;
	os << "    ::= { ";
    }
    
    if (e.m_Parent)
	os << e.m_Parent->GetFullSnmpName().c_str() << " " << e.m_Id;
    else
	os << ROOT_ENTRY;

    os << " }" << endl;
    os << endl;

    if (with_footer)
	ExportMibFooter(os);

    return OK;
}


const char* Cfg_Tree_Entry::GetSnmpAccessString(unsigned acc)
{
    if (acc & CfgAccessRead) {
	if (acc & CfgAccessWrite)
	    return "read-write";
	else
	    return "read-only";
    }
    
    if (acc & CfgAccessWrite)
	return "write-only";

    return "not-accessible";
}

mib_access Cfg_Tree_Entry::GetSnmpAccess(unsigned acc)
{
    if (acc & CfgAccessRead) {
	if (acc & CfgAccessWrite)
	    return READWRITE;
	else
	    return READONLY;
    }
    
    return NOACCESS;
}
#endif


// print the entry
void Cfg_Tree_Entry::Print(std::ostream& os) const
{
    Print(String(), os, true);
}


//=============================================================================
#ifdef CFG_HAS_SNMP
CSnmpLeaf::CSnmpLeaf(const Oidx& oid, mib_access access, CVariant &val)
: MibLeaf(oid, access, CreateSnmpSyntax(val)),
  m_Value(val)
{
}


// SNMP get
void CSnmpLeaf::get_request(Request *req, int ind)
{
    switch(m_Value.Type()) {
    case CVariant::VT_INT:
	{
	    *((SnmpInt32*)value) = m_Value.AsInt();
	}
	break;
    case CVariant::VT_FLOAT:
	{
	    char s[64];
	    //sprintf( s, "%f", m_Value.AsFloat());
            printf(""%f", m_Value.AsFloat());
	    *((OctetStr*)value) = s;
	}
	break;
    case CVariant::VT_STRING:
	{
	    *((OctetStr*)value) = m_Value.AsString();
	}
	break;
    case CVariant::VT_BINARY:
	{
	    OctetStr *o=(OctetStr*)value;
	    o->set_data( (unsigned char*)m_Value.Value(), m_Value.Size());
	}
	break;
    default:
	assert(false);
    }

    MibLeaf::get_request(req, ind);
}


// SNMP set
int CSnmpLeaf::set(const Vbx &vb)
{
    switch(m_Value.Type()) {
    case CVariant::VT_INT:
	{
	    int i_val;

	    vb.get_value(i_val);
	    m_Value = i_val;
	}
	break;
    case CVariant::VT_FLOAT:
	{
	    unsigned char s[64];
	    unsigned long len;

	    vb.get_value( s, len, 64);
	    s[len] = '\0';

	    char *err;
	    m_Value = (float)strtod((const char*)s, &err);
	}
	break;
    case CVariant::VT_STRING:
	{
	    unsigned char s[1024];
	    unsigned long len;

	    vb.get_value( s, len, 1024);
	    s[len] = '\0';
	    m_Value = (const char*)s;
	}
	break;
    case CVariant::VT_BINARY:
	{
	    unsigned char s[1024];
	    unsigned long len;

	    vb.get_value( s, len, 1024);
	    s[len] = '\0';

	    m_Value.SetValue(s, len);
	}
	break;
    default:
	assert(false);
    }

    return MibLeaf::set(vb);
}


// validate value before SNMP set
boolean CSnmpLeaf::value_ok(const Vbx& vb)
{
    switch(m_Value.Type()) {
    case CVariant::VT_STRING:
    case CVariant::VT_BINARY:
	return true;
    case CVariant::VT_FLOAT:
	{
	    unsigned char s[128];
	    unsigned long len;
	    
	    s[0] = '\0';
	    vb.get_value(s, len, 128);
	    s[len]='\0';
	    
	    char *err;
	    strtod( (char*)s, &err);
	    if (err && *err)
		return false;
	    
	    return true;
	}
	break;
    case CVariant::VT_INT:
	{
	    int i_val;
	    vb.get_value(i_val);
	    return true;
	}
	break;
    default:
	return false;
    };

    return false;
}


// create a SNMP value/syntax object
SnmpSyntax *CSnmpLeaf::CreateSnmpSyntax(const CVariant &val)
{
    switch(val.Type()) {
    case CVariant::VT_INT:
	return new SnmpInt32(val.AsInt());
    case CVariant::VT_STRING:
	return new OctetStr(val.AsString());
    case CVariant::VT_FLOAT:
	{
	    char s[50];
	    //sprintf( s, "%f", val.AsFloat());
            printf(""%f", val.AsFloat());
	    return new OctetStr(s);
	}
	break;
    case CVariant::VT_BINARY:
	return new OctetStr( (unsigned char*)val.Value(), val.Size() );
    }
    assert(false);
    return NULL;
}


// clone the SNMP leaf
MibEntryPtr CSnmpLeaf::clone()
{
    return new CSnmpLeaf(*key(), get_access(), m_Value);
};
#endif


//=============================================================================
Cfg_Tree_Leaf::Cfg_Tree_Leaf( const String& name, 
		     const String& desc,
		     Cfg_Tree_Group_Base *parent, 
		     const CVariant &value, 
		     bool use_snmp,
		     int id, 
		     unsigned access)
: Cfg_Tree_Entry( CfgTypeLeaf, name, desc, parent, use_snmp, id, access),
  m_Value(value)
{
#ifdef CFG_HAS_SNMP
    if (IsUsingSnmp()) {
	m_Mib = CreateMib();
    }
#endif
}


// Set value. Value must be of the same type as the entry.
void Cfg_Tree_Leaf::SetValue(const CVariant &v)
{ 
    if(m_Value.Type()!=v.Type()) {
	String full_path=GetFullPath();
	CLog::Error( CFGTOR, "%s: different type in SetValue ("
			     "type=%s param=%s)", full_path.c_str(),
			     m_Value.GetTypeStr(), v.GetTypeStr());
	return;
    };

    m_Value=v; 
}


// Retrieve the value.
const CVariant& Cfg_Tree_Leaf::GetValue() const
{ 
    return m_Value; 
}


// Clear the value
void Cfg_Tree_Leaf::Clear()
{
    m_Value.Clear();
    m_Modified = false;
}


// Print entry fullname and value
void Cfg_Tree_Leaf::Print( const String& indent, std::ostream& os, bool /*recursive*/) const
{
    os << indent << GetName().c_str() 
       << " (" << GetValue().GetTypeStr() << ") = ";
    GetValue().Print(os);
    os << NEW_LINE;
}


// Instantiate MIB leaf object
#ifdef CFG_HAS_SNMP
MibEntry *Cfg_Tree_Leaf::CreateMib()
{
    assert(IsUsingSnmp());

    if (m_Mib) {
	delete m_Mib;
    }

    if (m_Oid.empty()) {
	if (m_Parent) {
	    String s;
	    s.sprintf( "%s.%d.0", m_Parent->m_Oid.c_str(), m_Id);
	    m_Oid = s.c_str();
	} else
	    m_Oid = ROOT_OID;
    }

    m_Mib = new CSnmpLeaf(m_Oid.c_str(), GetSnmpAccess(m_Access), m_Value);
    if (m_Parent && m_Parent->GetType()==CfgTypeGroup) {
	static_cast<MibGroup*>(m_Parent->m_Mib)->add(m_Mib);
    }

    return m_Mib;
}


STATUS Cfg_Tree_Leaf::ExportMib(std::ostream& os, bool /*recursive*/, 
			   bool with_header, 
			   bool with_footer) const
{
    if (!IsUsingSnmp())
	return NOT_OK;

    return Cfg_Tree_Entry::ExportMib(*this, os, with_header, with_footer);
}
#endif

///////////////////////////////////////////////////////////////////////////////
Cfg_Tree_Group_Base::Cfg_Tree_Group_Base(
	CfgType type, 
	const String& name,
	const String& desc,
	Cfg_Tree_Group_Base *parent,
	bool use_snmp,
	int id,
	unsigned access
)
: Cfg_Tree_Entry( type, name, desc, parent, use_snmp, id, access)
{
}


Cfg_Tree_Group_Base::~Cfg_Tree_Group_Base()
{
    while (!m_Children.empty()) {
	Cfg_Tree_Entry *e = m_Children.front();
#ifdef CFG_HAS_SNMP
	e->m_Mib = NULL;
#endif
	delete e;
    }
}

Cfg_Tree_Entry* Cfg_Tree_Group_Base::FindEntry( const String& full_path )
{
    const char *path=full_path.c_str();

    if ((*path==*CFG_SEPARATOR))
	++path;

    for (Cfg_Tree_Iterator pe=GetFirstChild(); pe!=GetEndChild(); pe++) {
	if ((*pe)->IsRootOfPath(path)) {
	    char *next_name = strchr(path, *CFG_SEPARATOR);
	    //if (!next_name) next_name = strchr(path, *CFG_SEPARATOR2);
	    if (!next_name || !*(next_name+1)) {
		return *pe;
	    } else if ((*pe)->GetType()==CfgTypeGroupBase || 
		       (*pe)->GetType()==CfgTypeGroup) {
		Cfg_Tree_Group_Base *g=static_cast<Cfg_Tree_Group_Base*>(*pe);
		return g->FindEntry(next_name+1);
	    }
	}
    }

    return 0;
}

void Cfg_Tree_Group_Base::RemoveEntry(Cfg_Tree_Entry *e)
{
    m_Children.remove(e);
}

void Cfg_Tree_Group_Base::Clear()
{
    while (!m_Children.empty()) {
	Cfg_Tree_Entry *e=m_Children.front();
	m_Children.pop_front();

	delete e;
    }
    m_Modified=false;
}

const Cfg_Tree_Entry* Cfg_Tree_Group_Base::GetChildConst(size_t n) const
{
    if (n>m_Children.size())
	return NULL;

    size_t i=0;
    for (List<Cfg_Tree_Entry*>::const_iterator it=m_Children.begin(); 
	 it!=m_Children.end() && i<n; 
	 it++, i++)
	;

    if (i==n)
	return *it;
    else
	return NULL;
}

Cfg_Tree_Entry* Cfg_Tree_Group_Base::GetChild(size_t n)
{
    const Cfg_Tree_Entry *e=GetChildConst(n);
    return const_cast<Cfg_Tree_Entry*>(e);
}

/*
void Cfg_Tree_Group_Base::AddItem(Cfg_Tree_Entry *l)
{
    l->Detach();
    l->m_Parent = this;
    m_Children.push_back(l);
}
*/

void Cfg_Tree_Group_Base::Print(const String& indent, std::ostream& os, bool) const
{
    for(List<Cfg_Tree_Entry*>::const_iterator it=m_Children.begin();
        it!=m_Children.end();
	it++) {

	os << indent;
	(*it)->Print(os);
    }
}


///////////////////////////////////////////////////////////////////////////////
Cfg_Tree_Group::Cfg_Tree_Group( const String& name, 
		      const String& desc,
		      Cfg_Tree_Group_Base *parent,
		      bool use_snmp,
		      int id,
		      unsigned access)
: Cfg_Tree_Group_Base(CfgTypeGroup, name, desc, parent, use_snmp, id, access)
{
#ifdef CFG_HAS_SNMP
    if (IsUsingSnmp()) {
	m_Mib = CreateMib();
    }
#endif
}


// get a group with the specified name. If create_if_inexist flag
// is true and the group is not found, a new one will be created.
Cfg_Tree_Group* Cfg_Tree_Group::GetGroup(const String& name, 
			       const String& desc,
			       bool create_if_notexist,
			       bool use_snmp,
			       int id,
			       unsigned access)
{
    for (Cfg_Tree_Iterator pe=GetFirstChild(); pe!=GetEndChild(); pe++) {
	if (!strcasecmp(name.c_str(), (*pe)->GetName().c_str()) && (*pe)->GetType()==CfgTypeGroup) {
		return static_cast<Cfg_Tree_Group*>(*pe);
	}
    }

    if (create_if_notexist) {
	Cfg_Tree_Group *e = new Cfg_Tree_Group(name, desc, this, use_snmp, id, access);
	return e;
    }

    return NULL;
}



// Get a child table with specified name
Cfg_Tree_Table* Cfg_Tree_Group::GetTable(const String& name)
{
    for (Cfg_Tree_Iterator pe=GetFirstChild(); pe!=GetEndChild(); pe++) {
	if (!strcasecmp(name.c_str(), (*pe)->GetName().c_str()) && (*pe)->GetType()==CfgTypeTable) {
	    return static_cast<Cfg_Tree_Table*>(*pe);
	}
    }

    return NULL;
}

Cfg_Tree_Table* Cfg_Tree_Group::GetTable(	const String& name,
				const String& desc,
				bool use_snmp,
				int id,
				unsigned access)
{
    Cfg_Tree_Table *tb=GetTable(name);
    if (!tb) {
	tb = new Cfg_Tree_Table(name, desc, this, use_snmp, id, access);
    }
    return tb;
}


// Get a child leaf with specified name
Cfg_Tree_Leaf* Cfg_Tree_Group::GetLeaf(const String& name)
{
    for (Cfg_Tree_Iterator pe=GetFirstChild(); pe!=GetEndChild(); pe++) {
	if (!strcasecmp(name.c_str(), (*pe)->GetName().c_str()) && (*pe)->GetType()==CfgTypeLeaf) {
	    return static_cast<Cfg_Tree_Leaf*>(*pe);
	}
    }

    return NULL;
}

// Get a child entry with specified name. and create a new child 
// with that name and value if it is NOT exist
Cfg_Tree_Leaf* Cfg_Tree_Group::GetLeaf( const String& name,
			      const String& desc,
			      const CVariant &dflt_val,
			      bool use_snmp,
			      int id,
			      unsigned access)
{
    for (Cfg_Tree_Iterator pe=GetFirstChild(); pe!=GetEndChild(); pe++) {
	if (!strcasecmp(name.c_str(), (*pe)->GetName().c_str()) && 
	    (*pe)->GetType()==CfgTypeLeaf) 
	{
		return static_cast<Cfg_Tree_Leaf*>(*pe);
	}
    }

    Cfg_Tree_Leaf *e = new Cfg_Tree_Leaf( name, desc, this, dflt_val, use_snmp, id, access);
    return e;
}


// Print entry fullname and value to log
void Cfg_Tree_Group::Print( const String& indent, std::ostream& os, bool recursive ) const
{
    os <<  indent << GetName().c_str() << NEW_LINE;

    if (recursive) {
	String s;

	s=indent+"\t";

	for (Cfg_Tree_Iterator ce=GetFirstChild(); ce!=GetEndChild(); ce++) {
	    (*ce)->Print( s, os, recursive);
	};
    };
}


// create MIB group for this entry and its children
#ifdef CFG_HAS_SNMP
MibEntry *Cfg_Tree_Group::CreateMib()
{
    assert(IsUsingSnmp());

    if (m_Mib) {
	delete m_Mib;
    }

    if (m_Oid.empty()) {
	if (m_Parent) {
	    String s;
	    s.sprintf( "%s.%d", m_Parent->m_Oid.c_str(), m_Id);
	    m_Oid = s.c_str();
	} else
	    m_Oid = ROOT_OID;
    }

    MibGroup *mg = new MibGroup(m_Oid.c_str());
    m_Mib = mg;

    int child_index=0;
    for (List<Cfg_Tree_Entry*>::iterator it=m_Children.begin(); it!=m_Children.end(); it++) {
	(*it)->m_Id=child_index++;
	MibEntry *e = (*it)->CreateMib();
	mg->add(e);
    }
    if (m_Parent) {
	static_cast<MibGroup*>(m_Parent->m_Mib)->add(m_Mib);
    }

    return m_Mib;
}


STATUS Cfg_Tree_Group::ExportMib(std::ostream& os, 
			    bool recursive, 
			    bool with_header, 
			    bool with_footer) const 
{
    if (!IsUsingSnmp())
	return NOT_OK;

    Cfg_Tree_Entry::ExportMib(*this, os, with_header, false);
    if (recursive) {
	for (List<Cfg_Tree_Entry*>::const_iterator it=m_Children.begin(); 
	     it!=m_Children.end(); it++)
	{
	    (*it)->ExportMib(os, recursive, false, false);
	}
    }
    if (with_footer)
	Cfg_Tree_Entry::ExportMibFooter(os);

    return OK;
}
#endif

///////////////////////////////////////////////////////////////////////////////

Cfg_Tree_Table_Row::Cfg_Tree_Table_Row(Cfg_Tree_Table *parent, unsigned access)
: Cfg_Tree_Group_Base(CfgTypeGroupBase, String(), String(), parent, false, 0, access)
{
}

Cfg_Tree_Table_Row::~Cfg_Tree_Table_Row()
{
}

const String& Cfg_Tree_Table_Row::GetName() const
{
    if (m_Children.empty())
	return m_Name;

    Cfg_Tree_Leaf *l=static_cast<Cfg_Tree_Leaf*>(m_Children.front());
    if (l->GetValue().Type() == CVariant::VT_STRING) {
	m_RowName = l->GetValue().AsString();
    } else {
	m_RowName = "undefined";
    }

    return m_RowName;
}

Cfg_Tree_Table_Row* Cfg_Tree_Table_Row::Clone() const
{
    assert(GetParent()->GetType()==CfgTypeTable);
    Cfg_Tree_Table_Row *row=new Cfg_Tree_Table_Row(static_cast<Cfg_Tree_Table*>(GetParent()), 
				       GetAccess());

    for (List<Cfg_Tree_Entry*>::const_iterator it=m_Children.begin();
	 it!=m_Children.end();
	 it++) {
	
	assert((*it)->GetType()==CfgTypeLeaf);
	const Cfg_Tree_Leaf *src=static_cast<const Cfg_Tree_Leaf*>(*it);
	
	new Cfg_Tree_Leaf(src->GetName(),
		     src->GetDesc(),
		     row,
		     src->GetValue(),
		     src->IsUsingSnmp(),
		     0,
		     src->GetAccess());

	//row->m_Children.push_back(l);
    }

    return row;
}

Cfg_Tree_Leaf* Cfg_Tree_Table_Row::GetItem(size_t col_number)
{
    Cfg_Tree_Entry *e=GetChild(col_number);
    assert(e->GetType()==CfgTypeLeaf);

    return static_cast<Cfg_Tree_Leaf*>(e);
}

const Cfg_Tree_Leaf* Cfg_Tree_Table_Row::GetItem(size_t col_number) const
{
    const Cfg_Tree_Entry *e=GetChildConst(col_number);
    assert(e->GetType()==CfgTypeLeaf);

    return static_cast<const Cfg_Tree_Leaf*>(e);
}

///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#pragma warning ( disable : 4355 )  // this is used in constructor
#endif

Cfg_Tree_Table::Cfg_Tree_Table(const String& name,
		     const String& desc,
		     Cfg_Tree_Group_Base *parent, 
		     bool use_snmp,
		     int id,
		     unsigned access)
: Cfg_Tree_Group_Base(CfgTypeTable, name, desc, parent, use_snmp, id, access),
  m_MasterRow(this, access)
{ 
#ifdef CFG_HAS_SNMP
    if (IsUsingSnmp()) {
	m_Mib = CreateMib();
    }
#endif

    // delete the master row from the list of children
    m_Children.pop_front();
}

#ifdef _MSC_VER
#pragma warning ( default : 4355 )  // this is used in constructor
#endif


Cfg_Tree_Table::~Cfg_Tree_Table()
{
}


size_t Cfg_Tree_Table::GetColumnCount() const
{ 
    return m_MasterRow.GetColumnCount(); 
}


const String& Cfg_Tree_Table::GetColumnName(size_t n) const
{ 
    static String empty_string;

    if (n>=m_MasterRow.GetColumnCount())
	return empty_string;

    return m_MasterRow.GetItem(n)->GetName(); 
}


size_t Cfg_Tree_Table::AddRow() 
{ 
    Cfg_Tree_Table_Row *new_row=m_MasterRow.Clone();
    //m_Children.push_back(new_row);

#ifdef CFG_HAS_SNMP
    if (IsUsingSnmp()) {
	char oid[20];
	//sprintf( oid, "%d", m_Children.size()-1);
    printf("%d", m_Children.size()-1);
	MibTableRow *r=static_cast<MibTable*>(m_Mib)->add_row(oid);

	int i=1;
	for (List<Cfg_Tree_Entry*>::const_iterator it=new_row->GetFirstChild(); 
	     it!=new_row->GetEndChild(); it++, i++) {
	    Cfg_Tree_Leaf *leaf=static_cast<Cfg_Tree_Leaf*>(*it);
	    leaf->m_Mib=r->get_nth(i);
	    CSnmpLeaf *mib=static_cast<CSnmpLeaf*>(leaf->m_Mib);
	    mib->SetValueRef(leaf->m_Value);
	}
    }
#endif

    return m_Children.size()-1;
}


size_t Cfg_Tree_Table::GetRowCount() const
{ 
    return m_Children.size(); 
}


const CVariant& Cfg_Tree_Table::GetValue( int row, int col) const
{ 
    return FindItemConst(row, col);
}


void Cfg_Tree_Table::SetValue( int row, int col, const CVariant& val)
{ 
    CVariant& item = FindItem(row, col);
    if (item.Type()==CVariant::VT_VOID) {
	CLog::Error(CFGTOR, "SetValue in table %s row %d col %d: "
			    "item type is VT_VOID",
			    GetName().c_str(), row, col);
	assert(false);
	return;
    }
    if (item.Type()!=val.Type()) {
	CLog::Error(CFGTOR, "SetValue in table %s row %d col %d: "
			    "new value has different type",
			    GetName().c_str(), row, col);
	assert(false);
	return;
    }
    item=val;
}


const CVariant& Cfg_Tree_Table::FindItemConst(unsigned int row, unsigned int col) const
{
    if (row>=m_Children.size() || col>=m_MasterRow.GetColumnCount())
	return m_NullValue;

    const Cfg_Tree_Entry* e=GetChildConst(row);
    const Cfg_Tree_Group_Base *b=static_cast<const Cfg_Tree_Group_Base*>(e);

    e=b->GetChildConst(col);
    const Cfg_Tree_Leaf *l=static_cast<const Cfg_Tree_Leaf*>(e);

    return l->GetValue();
}


CVariant& Cfg_Tree_Table::FindItem(int row, int col)
{
    return const_cast<CVariant&>(FindItemConst(row,col));
}

const Cfg_Tree_Leaf* Cfg_Tree_Table::GetItem(size_t row, size_t col) const
{
    if (row>=m_Children.size() || col>=m_MasterRow.GetColumnCount())
	return NULL;

    const Cfg_Tree_Entry* e=GetChildConst(row);
    const Cfg_Tree_Group_Base *b=static_cast<const Cfg_Tree_Group_Base*>(e);

    e=b->GetChildConst(col);
    const Cfg_Tree_Leaf *l=static_cast<const Cfg_Tree_Leaf*>(e);

    return l;
}

STATUS Cfg_Tree_Table::AddLeafColumn(const String& name, 
				const String& desc,
				const CVariant& dflt_val,
				unsigned access)
{
    Cfg_Tree_Leaf *leaf=new Cfg_Tree_Leaf(name, 
				desc, 
				&m_MasterRow, 
				dflt_val, 
				false, 
				GetColumnCount()+2, access);
#ifdef CFG_HAS_SNMP
    if (IsUsingSnmp()) {
	leaf->m_IsUsingSnmp = true;
	leaf->CreateMib();

	static_cast<MibTable*>(m_Mib)->add_col(static_cast<MibLeaf*>(leaf->m_Mib));
    }
#endif

    return OK;
}


void Cfg_Tree_Table::Print(const String& indent, std::ostream& os, bool /*recursive*/) const
{
    size_t i, j;

    os << indent << GetName() << " (table) = " << NEW_LINE;

    String new_indent;
    new_indent=indent + "\t";

    // print table header (names)
    os << new_indent << "No\t";
    for (i=0; i<GetColumnCount(); i++) {
	os << GetColumnName(i) << (i==GetColumnCount()-1 ? NEW_LINE : "\t");
    }

    // print table header (types)
    os << new_indent << "\t";
    for (i=0; i<GetColumnCount(); i++) {
	os << "(" << m_MasterRow.GetItem(i)->GetValue().GetShortTypeStr() << ")"
	   << (i==GetColumnCount()-1 ? NEW_LINE : "\t");
    }

    // print each row
    for (i=0; i<GetRowCount(); ++i) {

	os << new_indent << i << "\t";
	for (j=0; j<GetColumnCount(); ++j) {
	    GetValue(i,j).Print(os);
	    os << (j==GetColumnCount()-1 ? NEW_LINE : "\t");
	}
    }
}


#ifdef CFG_HAS_SNMP
MibEntry *Cfg_Tree_Table::CreateMib()
{
    assert(IsUsingSnmp());

    if (m_Mib) {
	delete m_Mib;
    }

    if (m_Parent) {
	String s(128, 128);
	s.sprintf("%s.%d", m_Parent->m_Oid.c_str(), m_Id);
	m_Oid = s;
    } else
	m_Oid = ROOT_OID;

    MibTable *mg = new MibTable(m_Oid.c_str(), 1, true);
    m_Mib = mg;

    if (m_Parent) {
	static_cast<MibGroup*>(m_Parent->m_Mib)->add(m_Mib);
    }

    static_cast<MibTable*>(m_Mib)->add_col(new MibLeaf("1", READONLY, new SnmpInt32(0)));
    return m_Mib;
}


STATUS Cfg_Tree_Table::ExportMib(std::ostream& os, 
			    bool /*recursive*/, 
			    bool with_header, 
			    bool with_footer) const 
{
    if (!IsUsingSnmp())
	return OK;

    // table definition
    Cfg_Tree_Entry::ExportMib(*this, os, with_header, false);

    // table entry
    os << (GetFullSnmpName()+"Entry").c_str() 
       << "   OBJECT-TYPE" 
       << endl;

    os << "    SYNTAX " 
       << (GetFullSnmpName()+"Type").c_str() 
       << endl;
    os << "    ACCESS not-accessible" << endl;
    os << "    STATUS mandatory" << endl;
    os << "    DESCRIPTION \"\"" << endl;
    os << "    INDEX" << endl
       << "      { " 
       << (GetFullSnmpName()+"Index").c_str() 
       << " }" 
       << endl;
    os << "    ::= { " 
       << GetFullSnmpName().c_str() 
       << "Table 1 }" 
       << endl;
    os << endl;


    // table type
    // blahblahType ::= SEQUENCE {
    //    blahblahIndex  INTEGER,
    //	  column1	 DisplayString,
    //	  column2        X
    //    }
    //
    os << (GetFullSnmpName()+"Type").c_str() 
       << " ::= SEQUENCE {" 
       << endl;

    os << "    " 
       << (GetFullSnmpName()+"Index").c_str() 
       << "  INTEGER," 
       << endl;

    for (size_t i=0; i<GetColumnCount(); ++i) {
	os << "    " 
	   << GetFullSnmpName()+"_"+GetColumnName(i) 
	   << "   " 
	   << GetSnmpSyntaxString(m_MasterRow.GetItem(i)->GetValue().Type())
	   << (i!=GetColumnCount()-1 ? "," : "")
	   << endl;
    }
    os << "    }" << endl;
    os << endl;


    // index definition
    os << "-- Table " << GetName().c_str() << ": index column" << endl;

    os << (GetFullSnmpName()+"Index").c_str() 
       << "    OBJECT-TYPE" 
       << endl;
    os << "    SYNTAX INTEGER" << endl;
    os << "    ACCESS read-only" << endl;
    os << "    STATUS mandatory" << endl;
    os << "    DESCRIPTION \" Index \"" << endl;
    os << "    ::= { " 
       << (GetFullSnmpName()+"Entry").c_str() 
       << " 1 } " 
       << endl;
    os << endl;

    // each column definition
    for (i=0; i<GetColumnCount(); ++i) {
	os << "-- Table " << GetName().c_str() << ": column " << i+2 << endl;

	os << GetFullSnmpName()+"_"+GetColumnName(i) 
	   << "    OBJECT-TYPE" 
	   << endl;
	os << "    SYNTAX " 
	   << GetSnmpSyntaxString(m_MasterRow.GetItem(i)->GetValue().Type()) 
	   << endl;
	os << "    ACCESS " 
	   << GetSnmpAccessString(m_Access) 
	   << endl;
	os << "    STATUS mandatory" << endl;
	os << "    DESCRIPTION \"\"" << endl;
	os << "    ::= { "
	   << (GetFullSnmpName()+"Entry").c_str() 
	   << " " << m_MasterRow.GetItem(i)->GetId()
	   << " } " 
	   << endl;
	os << endl;
    }

    if (with_footer)
	ExportMibFooter(os);

    return OK;
}
#endif

