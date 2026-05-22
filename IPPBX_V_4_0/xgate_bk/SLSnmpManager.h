// SLSnmpManager.h: interface for the SLSnmpManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_SLSNMPMANAGER_H_INCLUDED_)
#define _SLSNMPMANAGER_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//#include "skalib\include\ska.h"
#include "skalib/include/ska.h"	//linux porting
#include "termcmd.h"
#include "cfg.h"

class SLSnmpMonitor;
typedef List<SLSnmpMonitor*> SnmpMonitorList;

class SLSnmpManager  : public Term_Cmd_Target
{
public:
	int GetMonitorCount();
	SnmpMonitorList::iterator GetLastMonitorPost();
	static void Destroy();
	void DoTermCmd(Term_Session *session, bool no, Term_Cmd *cmd);
	static void EnumProcDetails(const char *name, int type, const char *desc, void *data, bool last);
	static void EnumProc(const char *name, int type, const char *desc, void *data, bool last);
	void CreateMIB(const char* filename);
	SLSnmpMonitor* GetNext(SnmpMonitorList::iterator& pos);
	SnmpMonitorList::iterator GetFirstMonitorPos();
	static SLSnmpManager* Instance();
	static SLSnmpManager* Create();
	void InitMonitors();
	void AddMonitor(SLSnmpMonitor *pmon);
	SLSnmpManager();
	virtual ~SLSnmpManager();

	class SLPropEnum
	{
	public:
		int counter;
		char name[16];
		FILE* file;
	};

protected:

	SnmpMonitorList m_lstMonitors;

	static SLSnmpManager* slsnmpManager;
private:
	enum {
	Term_Cmd_Snmp,
	Term_Cmd_DumpMib,		
    };
	Cfg_Tree_Group* _cfgTree;
};

#endif // !defined(_SLSNMPMANAGER_H_INCLUDED_)
