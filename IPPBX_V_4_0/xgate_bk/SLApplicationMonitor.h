// SLApplicationMonitor.h: interface for the SLApplicationMonitor class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _SLAPPLICATIONMONITOR_H_INCLUDED_
#define _SLAPPLICATIONMONITOR_H_INCLUDED_


#include "SLSnmpProvider.h"
#include "application.h"

class SLApplicationEntry;

class SLApplicationMonitor : public SLSnmpMonitor  
{
public:
	static int GetTypeTemplate(int id);
	static SLApplicationEntry* CreateTableEntry(CT_Application *papp);
	const char* Prefix();
	void EnumProperty(void* data, ENUMPROPPROC lpCallback);
	int PropertiesCount();
	const char* Name();
	static SLApplicationMonitor* Create();
	static SLApplicationMonitor* Instance();
	SLApplicationMonitor();
	virtual ~SLApplicationMonitor();
	static List<SLApplicationEntry*> entries_;

protected:
	static SLApplicationMonitor* slsnmpApplication;

	static const char* m_arrPropNames[];
	static const int  m_arrPropType[];

};

#endif // !defined(_SLAPPLICATIONMONITOR_H_INCLUDED_)
