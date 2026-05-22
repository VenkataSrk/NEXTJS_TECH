// SLInterfaceMonitor.h: interface for the SLInterfaceMonitor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_SLINTERFACEMONITOR_H_INCLUDED_)
#define _SLINTERFACEMONITOR_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SLSnmpProvider.h"
#include "SLInterfaceEntry.h"

class SLInterfaceMonitor : public SLSnmpMonitor  
{
public:
	static int GetTypeTemplate(int id);
	const char* Prefix();
	static SLInterfaceEntry* CreateTableEntry(Interface* pif);
	virtual void EnumProperty(void* data, ENUMPROPPROC lpCallback);
	virtual int PropertiesCount();
	virtual const char* Name();
	static SLInterfaceMonitor* Create();
	virtual ~SLInterfaceMonitor();

	static SLInterfaceMonitor* Instance(){return slifmonInstance;};

protected:
	static const char* m_arrPropNames[];
	static const int  m_arrPropType[];

	SLInterfaceMonitor();
	static SLInterfaceMonitor* slifmonInstance;
	static List<SLInterfaceEntry*> entries_;

};

#endif // !defined(_SLINTERFACEMONITOR_H_INCLUDED_)
