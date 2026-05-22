
#ifndef _SLAPPLICATIONENTRY_H_INCLUDED_
#define _SLAPPLICATIONENTRY_H_INCLUDED_

//#include "slsnmpprovider.h"
#include "SLSnmpProvider.h"
#include "application.h"

class SLApplicationMonitor;

/**
 SL application entry, entry table for Application SNMP
 */
class SLApplicationEntry : public  SLSnmpTableEntry  
{
public:
	int Type(int id);
	const char* Name();
	const char* GetString(int id);
	virtual int GetInt(int id);
	bool IsValid();
	static SLApplicationEntry* Create(CT_Application* ptask);
	SLApplicationEntry();
	virtual ~SLApplicationEntry();

protected:
	CT_Application * m_pApp;
	String m_strName;

	friend class SLApplicationMonitor;
};

#endif // _SLAPPLICATIONENTRY_H_INCLUDED_
