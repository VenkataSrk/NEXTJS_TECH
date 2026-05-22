/* $Header: /xGate/cfg.h 7     13/09/01 19:37 Adham $
 *   
 * CTENGINE
 * (C) 2001 Switchlab, Ltd
 *
 * Interfaces for manipulating folders in configuration tree.
 *
 * $Log: /xGate/cfg.h $
 * 
 * 7     13/09/01 19:37 Adham
 * 
 * 6     4/12/01 8:00p Bennylp
 * Integration.
 *
 */
#ifndef __CONFIG_TREE_H__
#define __CONFIG_TREE_H__

#include "types.h"
#include "variant.h"
#include "termcmd.h"
#include "string.h"
#include <algorithm>

class Cfg_Tree_Entry;
class Cfg_Tree_Leaf;
class Cfg_Tree_Group_Base;
class Cfg_Tree_Group;
class Cfg_Tree_Table;

typedef List <Cfg_Tree_Entry*>::const_iterator Cfg_Tree_Iterator;

/**
  Type of access for configuration entry.
  */
enum CfgAccess {
    CfgAccessRead=2,	// allowed to read
    CfgAccessWrite=4,	// allowed to write
    CfgAccessAdd=8,	// allowed to add child entry (or row of a table)
    CfgAccessDelete=16,	// allowed to delete child entry (or row of a table)
};


///////////////////////////////////////////////////////////////////////////////
/**
  Abstract configuration entry class.
  This class will be derived by leaf, group, and table configuration entry.
  */
class Cfg_Tree_Entry {

public:
    virtual ~Cfg_Tree_Entry();

    /**
      Configuration entry type
      */
    enum CfgType {
	CfgTypeGroupBase,   // group with limited capabilities
	CfgTypeGroup,	    // full group configuration entry
	CfgTypeLeaf,	    // leaf configuration entry
	CfgTypeTable,	    // table configuration entry
    };

    /** Get the name of configuration entry */
    virtual const String& GetName() const;

    /** Get the type of configuration entry */
    CfgType GetType() const;

    /**
      Get the parent configuration group, or NULL if this is the root 
      configuration entry 
      */
    Cfg_Tree_Group_Base* GetParent() const;

    /**
      Get the identification of this entry.
      The identification is unique for all entries under the same parent.
      */
    int	GetId() const { 
	return m_Id; 
    }

    /**
      Get the entry description
      */
    const String& GetDesc() const { 
	return m_Desc; 
    }

    /**
      Investigate whether the entry has a specific access
      */
    bool HasAccess(
	CfgAccess a
    ) const 
    { 
	return (m_Access & a)!=0; 
    }

    /**
      Get the access combination
      */
    unsigned GetAccess() const { 
	return m_Access; 
    }

    /**
      Get the full location/path of this entry
      */
    String GetFullPath() const;

    /**
      Detach this entry from its parent
      */
    void Detach();

    /**
      Clear the entry.
      The behaviour will be different if the entry is a group, leaf, or table.
      */
    virtual void Clear() = 0;

    /**
      Print the configuration entry
      */
    void Print(std::ostream&) const;

    /**
      Return true if the entry is the root entry of a specified path
      */
    bool IsRootOfPath(const String& path) const;

    /** Get the command list.
     */
    const List <Term_Cmd*> & GetCommandList() const {
	return m_Commands;
    }
    List <Term_Cmd*> & GetCommandList() {
	return m_Commands;
    }

    /** Get handler for the show command.
     */
    const List<Term_Cmd_Target *> & GetShowHandlerList() const {
	return m_ShowHandlerList;
    }

    /** Register show handler for this entry.
     */
    void RegisterShowHandler ( Term_Cmd_Target * handler ) {
	assert (handler);
	m_ShowHandlerList.remove(handler);
	m_ShowHandlerList.push_back(handler);
    }

    /** Unregister show handler for this entry.
     */
    void UnregisterShowHandler ( Term_Cmd_Target * handler ) {
	m_ShowHandlerList.remove(handler);
    }

#ifdef CFG_HAS_SNMP
    /**
      SNMP
      */
    bool IsUsingSnmp() const { 
	return m_IsUsingSnmp; 
    }

    /** Get the MIB */

    MibEntry *GetMib() { 
	return m_Mib; 
    }

    /**
      Export the entry attributes to a SNMP MIB file 
      */
    virtual STATUS  ExportMib(
	std::ostream&,		// output stream
	bool recursive=true,	// only apply to group
	bool with_header=true,	// include SNMP MIB file header
	bool with_footer=true	// include SNMP MIB file footer
    ) const = 0;

    /** Convert our access combination to SNMP access */
    static mib_access GetSnmpAccess(
	unsigned access	    // our access combination
    );

    /** Convert our access combination to SNMP access string */
    static const char* GetSnmpAccessString(
	unsigned acc
    );

    /** Convert our type to SNMP type */
    static const char *GetSnmpSyntaxString(
	CVariant::VType
    );
#else
    bool IsUsingSnmp() const { 
	return false;
    }
#endif


protected:
    Cfg_Tree_Group_Base	*m_Parent;
    bool	m_Modified;
#ifdef CFG_HAS_SNMP
    MibEntry	*m_Mib;
    String	m_Oid;
#endif
    int		m_Id;
    String	m_Desc;
    bool	m_IsUsingSnmp;
    unsigned	m_Access;
    String	m_Name;
    CfgType	m_Type;
    bool	m_Serializable;
    List <Term_Cmd_Target*> m_ShowHandlerList;
    List <Term_Cmd*> m_Commands;

    Cfg_Tree_Entry(
	CfgType type,
	const String& name,
	const String& desc,
	Cfg_Tree_Group_Base *parent,
	bool use_snmp,
	int id,
	unsigned access
    );

    virtual void Print(
	const String& indent, 
	std::ostream& output, 
	bool recursive=true
    ) const = 0;

#ifdef CFG_HAS_SNMP
    virtual MibEntry * CreateMib() = 0;

    String GetFullSnmpName() const;

    static STATUS ExportMibHeader(
	std::ostream&
    );

    static STATUS ExportMibFooter(
	std::ostream&
    );

    static STATUS ExportMib(
	const Cfg_Tree_Entry &,
	std::ostream&, 
	bool with_header, 
	bool with_footer
    );
#endif

private:
    /** unsupported assignment operator */
    const Cfg_Tree_Entry& operator = (const Cfg_Tree_Entry& ) {
	assert(false);
	return *this;
    }

    /**
      Attach a command to this entry.
     */
    void AttachCommand (Term_Cmd *cmd) {
	// make sure command has not been registered before
	assert (std::find(m_Commands.begin(), m_Commands.end(), cmd) == m_Commands.end());
	m_Commands.remove(cmd);

	m_Commands.push_back(cmd);
    }

    /**
      Detach a command.
     */
    void DetachCommand (Term_Cmd *cmd) {
	m_Commands.remove(cmd);
    }


    friend class Cfg_Tree_Group;
    friend class Cfg_Tree_Leaf;
    friend class Cfg_Tree_Table;
    friend class Cfg_Tree_Group_Base;
    friend class Term_Cmd;
};

///////////////////////////////////////////////////////////////////////////////
/**
  Internal SNMP class
  */
#ifdef CFG_HAS_SNMP
class CSnmpLeaf : public MibLeaf {
public:
    CSnmpLeaf(
	const Oidx& oid, 
	mib_access access, 
	CVariant &val
    );

    void SetValueRef(
	CVariant &val
    ) { 
	m_Value=val; 
    };

    void get_request(
	Request *req, 
	int ind
    );

    int set(
	const Vbx &vb
    );

    boolean value_ok(
	const Vbx& vb
    );

    MibEntryPtr	clone();

private:
    CVariant &	m_Value;

    SnmpSyntax *CreateSnmpSyntax(
	const CVariant&
    );

    const CSnmpLeaf& operator=(const CSnmpLeaf&) { 
	assert(false); return *this; 
    }
};
#endif

///////////////////////////////////////////////////////////////////////////////
/**
  Leaf configuration entry
  */
class Cfg_Tree_Leaf : public Cfg_Tree_Entry {
public:
    Cfg_Tree_Leaf(
	const String& name, 
	const String& desc,
	Cfg_Tree_Group_Base *parent, 
	const CVariant &value, 
	bool use_snmp=false,
	int id=0, 
	unsigned access=CfgAccessRead
    );

    /**
      Clear the value
      */
    virtual void Clear();

    /**
      Set the value.
      */
    void SetValue(const CVariant &v);

    /**
      Get the value
      */
    const CVariant& GetValue() const;

#ifdef CFG_HAS_SNMP
    /**
      Export the entry attributes to a SNMP MIB file 
      */
    virtual STATUS  ExportMib(
	std::ostream&,		// output stream
	bool recursive=true,	// only apply to group
	bool with_header=true,	// include SNMP MIB file header
	bool with_footer=true	// include SNMP MIB file footer
    ) const;
#endif

protected:
    CVariant	    m_Value;

    // inherited from Cfg_Tree_Entry
    virtual void Print(
	const String& indent, 
	std::ostream& output, 
	bool recursive=true
    ) const;

#ifdef CFG_HAS_SNMP
    MibEntry *	    CreateMib();
#endif

    friend class    Cfg_Tree_Entry;
    friend class    Cfg_Tree_Group;
    friend class    Cfg_Tree_Table;
};


///////////////////////////////////////////////////////////////////////////////
class Cfg_Tree_Group_Base : public Cfg_Tree_Entry {
public:
    Cfg_Tree_Group_Base(
	CfgType,
	const String& name,
	const String& desc,
	Cfg_Tree_Group_Base *parent,
	bool use_snmp,
	int id,
	unsigned access
    );

    /**
      destroy this and its children
      */
    virtual ~Cfg_Tree_Group_Base();

    /**
      Get pointer/iterator to the pointer to the first child
      */
    Cfg_Tree_Iterator GetFirstChild() const {
	return m_Children.begin(); 
    }

    /**
      Get the pointer/iterator to one element past the last child
      */
    Cfg_Tree_Iterator GetEndChild() const {
	return m_Children.end();
    }

    /**
      Find an entry with the specified path.
      The path should have this group as its root.
      */
    Cfg_Tree_Entry* FindEntry(
	const String& path
    );

    /**
      Detach an entry
      */
   void RemoveEntry(
	Cfg_Tree_Entry *
    );

    /**
      Deletes all children
      */
   void Clear();

    /**
      Get child number n (index is zero based)
      */
    const Cfg_Tree_Entry* GetChildConst(size_t n) const;

    /**
      Get child number n (index is zero based)
      */
    Cfg_Tree_Entry* GetChild(size_t n);

protected:
    List <Cfg_Tree_Entry*> m_Children;

    /** Print */
    virtual void Print(
	const String&, 
	std::ostream&, 
	bool
    ) const;

private:
    /** unsupported assignment operator */
    const Cfg_Tree_Group_Base& operator=(const Cfg_Tree_Group_Base&) {
	assert(false);
	return *this;
    }

    friend class Cfg_Tree_Entry;
};

///////////////////////////////////////////////////////////////////////////////
/**
  Group configuration entry
  */
class Cfg_Tree_Group : public Cfg_Tree_Group_Base {
public:
    /** Create a group entry */
    Cfg_Tree_Group(
	const String& name,		// name of the entry
	const String& desc,		// description
	Cfg_Tree_Group_Base *parent,		// the parent entry
	bool use_snmp=false,		// set false to disable SNMP
	int id=0,			// unique ID among the siblings
	unsigned access=CfgAccessRead	// access combination
    );

    /**
      Destructor
      This will delete all the children.
      */
    ~Cfg_Tree_Group() {}

    /**
      Get a child group.
      If create_if_nonexist flag is set to TRUE, then if the child is not
      exist, it will be created with the specified parameters.
      */
    Cfg_Tree_Group* GetGroup(
	const String& name,		// the name
	const String& desc,		// the description
	bool create_if_nonexist=false,	// true to create the group if it doesn't exist
	bool use_snmp=false,		// set false to disable SNMP
	int id=0,			// unique id among its siblings
	unsigned access=CfgAccessRead	// access combination
    );

    /**
      Get a child leaf, and return NULL if it doesn't exist
      */
    Cfg_Tree_Leaf* GetLeaf(
	const String& name	// name of the leaf entry to search
    );

    /**
      Get a child leaf.
      If the child leaf doesn't exist and dflt_val parameter is not
      VT_VOID, then a new leaf will be created according to the specified
      parameters
      */
    Cfg_Tree_Leaf* GetLeaf(
	const String& name,		// the name
	const String& desc,		// the description
	const CVariant &dflt_val,	// default value to use, or VT_VOID
	bool use_snmp=false,		// set false to disable SNMP
	int id=0,			// unique id among its siblings
	unsigned access=CfgAccessRead	// access combination
    );

    Cfg_Tree_Table* GetTable(
	const String& name
    );

    Cfg_Tree_Table* GetTable(
	const String& name,
	const String& desc,
	bool use_snmp=false,
	int id=0,
	unsigned access=CfgAccessRead
    );

#ifdef CFG_HAS_SNMP
    /**
      Export the entry attributes to a SNMP MIB file 
      */
    virtual STATUS  ExportMib(
	std::ostream&,		// output stream
	bool recursive=true,	// only apply to group
	bool with_header=true,	// include SNMP MIB file header
	bool with_footer=true	// include SNMP MIB file footer
    ) const;
#endif

private:
    // inherited from CCfGEntry
    virtual void Print(
	const String& indent, 
	std::ostream& output, 
	bool recursive=true
    ) const;

#ifdef CFG_HAS_SNMP
    virtual MibEntry *CreateMib();
#endif

    /** Unsupported assignment operator */
    const Cfg_Tree_Group& operator = (const Cfg_Tree_Group&) {
	assert(false);
	return *this;
    }

    friend class Cfg_Tree_Entry;
    friend class Cfg_Tree_Leaf;
    friend class Cfg_Tree_Table;
};


///////////////////////////////////////////////////////////////////////////////
/**
  Row of a table.
  Basically it is a group, but it can only hold leaf, while the original group
  can hold anything (leaf, table, or other group).
  */
class Cfg_Tree_Table_Row : public Cfg_Tree_Group_Base {
public:
    /**
      Create a row.
      The name parameter should be unique accross the table, and it is 
      used to identify the row.
      */
    Cfg_Tree_Table_Row(
	Cfg_Tree_Table *parent,  // parent table
	unsigned access	    // access combination
    );

    /**
      Destructor
      */
    ~Cfg_Tree_Table_Row();

    /**
      Create a duplicate
      */
    Cfg_Tree_Table_Row* Clone() const;

    /**
      Get the name.
      This will return the name of the first column.
      */
    virtual const String& GetName() const;

    /**
      Get number of columns (items) in this row
      */
    size_t GetColumnCount() const { return m_Children.size(); }

    /** Get an column/item number n. The item number is zero based */
    Cfg_Tree_Leaf* GetItem(
	size_t col_number   // the item/column number/index
    );

    /** Get an column/item number n. The item number is zero based */
    const Cfg_Tree_Leaf* GetItem(
	size_t col_number   // the item/column number/index
    ) const;


private:
     mutable String m_RowName;

    /** Unsupported assignment operator */
    const Cfg_Tree_Table_Row& operator = (const Cfg_Tree_Table_Row& ) {
	assert(false);
	return *this;
    }

#ifdef CFG_HAS_SNMP
    /** Unsupported export MIB
	The MIB will be exported by parent table
      */
    STATUS ExportMib(
	std::ostream&, 
	bool, 
	bool, 
	bool
    ) const {
	return NOT_OK;
    };

    /** Unsupported CreateMIB
	The MIB object for this class will be generated */
    MibEntry *CreateMib() {
	assert(false);
	return NULL;
    }
#endif
};

///////////////////////////////////////////////////////////////////////////////
/**
  Table configuration entry.
  */
class Cfg_Tree_Table : public Cfg_Tree_Group_Base {
public:

    /**
      Create an empty table
      */
    Cfg_Tree_Table(const String& name,		// table name
	      const String& desc,		// description
	      Cfg_Tree_Group_Base *parent,		// parent
	      bool use_snmp=false,		// set FALSE to disable SNMP
	      int id=0,				// unique SNMP id among its siblings
	      unsigned access=CfgAccessRead);	// access combination

    ~Cfg_Tree_Table();

    /**
      Add a column to the table. 
      The first column will be used as the index/identifier for the rows,
      thus the value must be unique for all rows. Columns can be added 
      only BEFORE any rows are created.
      */
    STATUS AddLeafColumn(
	const String& name,		// column name
	const String& desc,		// column description
	const CVariant& dflt_val,	// default value
	unsigned access=CfgAccessRead	// access combination
    );

    /**
      Return number of columns
      */
    size_t GetColumnCount() const;

    /**
      Return name of column n (index is zero based)
      */
    const String& GetColumnName(
	size_t n		// column number (zero based)
    ) const;
    
    /**
      Return column number for a particular column name
      */
    int	GetColumnNumber(
	const char *
    ) const;

    /**
      Add a new row to the table.
      The row will be initialized with the default value for each column

      @return
      int number of rows now
      */
    size_t AddRow();

    /**
      Get number of rows in the table
      */
    size_t GetRowCount() const;
  
    /**
      Get the value of specified item
      */
    const CVariant& GetValue(
	int row, 
	int col
    ) const;

    /**
      Set the value of the specified item
      */
    void SetValue(
	int row, 
	int col, 
	const CVariant&
    );

    /**
      Get item
      */
    const Cfg_Tree_Leaf* GetItem(
	size_t row, 
	size_t col
    ) const;

private:
    Cfg_Tree_Table_Row m_MasterRow;
    
    CVariant m_NullValue;

    const CVariant& FindItemConst(
	unsigned int row, 
	unsigned int col
    ) const;

    CVariant& FindItem(
	int row, 
	int col
    );

    void Clear() { /* TODO: NOT IMPLEMENTED */ };

    void Print(
	const String& indent, 
	std::ostream& output, 
	bool recursive=true
    ) const;

#ifdef CFG_HAS_SNMP
    MibEntry *		CreateMib();

    /**
      Export table definition to a SNMP MIB file
      */
    STATUS ExportMib(
	std::ostream&, 
	bool recursive=true, 
	bool with_header=true, 
	bool with_footer=true
    ) const;
#endif

    /** Unsupported assignment operator */
    const Cfg_Tree_Table& operator = (const Cfg_Tree_Table& ) {
	assert(false);
	return *this;
    }

    friend class	CCfgRegDevice;
};




#endif	// __CONFIG_TREE_H__

