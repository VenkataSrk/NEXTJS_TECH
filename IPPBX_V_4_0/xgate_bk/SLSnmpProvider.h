// SLSnmpProvider.h: interface for the SLSnmpProvider class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _SLSNMPPROVIDER_H_INCLUDED_
#define _SLSNMPPROVIDER_H_INCLUDED_

#include "serviceprovider.h"
//#include "mediamanager.h"
#include "MediaManager.h"
#include "slsnmpcmp.h"
#include "SLSnmpManager.h"
#include "telnet.h"	// Added by ClassView
//#include "snmp.h" // __linux__

// MIB function actions.
#define MIB_ACTION_GET            ASN_RFC1157_GETREQUEST
#define MIB_ACTION_SET            ASN_RFC1157_SETREQUEST
#define MIB_ACTION_GETNEXT        ASN_RFC1157_GETNEXTREQUEST

//#define USE_AGENTPLUS

//UINT oid_prefix[] = {1, 3, 6, 1, 4, 1, 2013};
#define SLSNMP_ROOT		"1.3.6.1.4.1.2013"

#define LPVOID void*	//linux porting

class SLSnmpMonitor ;
class SLSnmpTableEntry : public ITableEntry
{
public:
	virtual bool		IsValid()=0;
    virtual int			Type(int id)=0;
    virtual const char* GetString(int id)=0;
    virtual int			GetInt(int id)=0; 

	virtual const char* Name()=0;

	SLSnmpTableEntry(){m_nIndex=m_nProperties = 0;};

	virtual int GetIndex(){return m_nIndex;};
	virtual int PropertiesCount() {return m_nProperties;};
protected:
	int m_nProperties;
	int m_nIndex;

	friend class SLSnmpMonitor;	
};

typedef void (*ENUMPROPPROC)(const char* name, int type, const char* desc, void* data, bool last);

typedef List<SLSnmpTableEntry*> LstSnmpTables;
class SLSnmpProvider; 
class SLSnmpMonitor  
{
public:
	int GetEntryCount();
	SLSnmpTableEntry* GetEntryByIndex(int id);
	virtual void ValidateIndex();
	void Init();
	virtual void AddTableEntry(SLSnmpTableEntry* pobj);
	SLSnmpMonitor();

	virtual ~SLSnmpMonitor()
	{
	    /*
	    if(m_tblCounter)
		delete m_tblCounter;
	    
	    if(m_tblEntry)
		delete m_tblEntry ;
	    */
	}

	virtual const char*	Name()=0;
	virtual const char*	Prefix()=0;

	virtual int		PropertiesCount()=0;
	virtual void	EnumProperty(void* data, ENUMPROPPROC lpCallback)=0;
protected:
	
	List<SLSnmpTableEntry*> m_lstTables;
	
	int		m_nIndex;
	String	m_sIndexOid;
	String  m_sEntryOid;

#ifdef USE_AGENTPLUS
	ISnmpTable*	m_tblCounter;
	ISnmpTable*	m_tblEntry;
#endif

	friend class SLSnmpProvider;
	friend class SLSnmpManager;
};

typedef List<SLSnmpMonitor*> LstSnmpMonitor;

class SLSnmpProvider : public ServiceProvider
{
public:
	BYTE GetVal(BYTE action, BYTE* oid, 
		    BYTE oidlen, DWORD* nval, 
		    char* ptrval, UINT valen,
		    BYTE* newoid, DWORD* newlen);

	ISLSnmpAgent* GetAgent();

	void SetRefreshInterval(int iv);
	//static void SNMPThread(LPVOID lpParameter);
	static void SNMPThread(LPVOID lpParameter);

	static SLSnmpProvider* Instance();
	static SLSnmpProvider* Create();

	virtual bool Unload();
	virtual bool Load();
	virtual void HandleEvents();
	virtual void DetachTask(CTask *pTask);
	virtual bool CancelJob(CTask* pTask);
	virtual bool AttachTask(CTask* pTask);

	SLSnmpProvider();
	virtual ~SLSnmpProvider();

/*	struct _netiface
	{
	    String name;
	    int refid;
	    int seqno;
	    Interface* m_pif;
	public:
		bool _valid();
	};
*/
	
	ISLSnmpAgent*	m_pifAgent;

protected:
	
	static int IncIndex();

	void ValidateIndex();
	bool Tick();
    
	static SLSnmpProvider* slsnmpProviderInstance;
	
	bool	m_bIsRunning;
	DWORD	m_uTick;
	int	m_nPollRes;
	DWORD	m_nThreadId;
	HANDLE  m_hThread;

	

private:
	static unsigned long m_uIndexCounter;   
};


#endif // _SLSNMPPROVIDER_H_INCLUDED_
