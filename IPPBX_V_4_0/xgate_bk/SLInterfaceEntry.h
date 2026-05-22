// SLInterfaceEntry.h: interface for the SLInterfaceEntry class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_SLINTERFACEENTRY_H_INCLUDED_)
#define _SLINTERFACEENTRY_H_INCLUDED_

#include "SLSnmpProvider.h"

class SLInterfaceEntry : public SLSnmpTableEntry  
{
public:
	static SLInterfaceEntry* Create(Interface* pif);	
	virtual ~SLInterfaceEntry();

	virtual bool		IsValid();
    virtual int			Type(int id);
    virtual const char* GetString(int id);
    virtual int			GetInt(int id); 
	virtual const char* Name();

protected:
	SLInterfaceEntry(Interface * pif);
	Interface* m_pInterface;
	String	   m_strName;

	friend class SLInterfaceMonitor;
};

#endif // !defined(_SLINTERFACEENTRY_H_INCLUDED_)
