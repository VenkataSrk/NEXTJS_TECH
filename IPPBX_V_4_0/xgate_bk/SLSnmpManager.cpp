// SLSnmpManager.cpp: implementation of the SLSnmpManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SLSnmpManager.h"
#include "SLInterfaceMonitor.h"
#include "SLVoiceMonitor.h"
#include "SLApplicationMonitor.h"
#include <stdio.h>
#include "kernel.h"
#include "term.h"
#define __linux__

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
SLSnmpManager* SLSnmpManager::slsnmpManager=NULL;

SLSnmpManager::SLSnmpManager()
{
	SLSnmpProvider::Create();

	Term_Cmd *cmd = new Term_Cmd(
	    this,
	    Term_Cmd_DumpMib,
	    "dump-mib", 
	    "Create MIB file to in the specified directory",
	    PrivilegeLevelAdmin,
	    TermModeAll,
	    false);

	cmd->Add(new Term_Cmd_Arg("filename",CVariant(""),CVariantValidator("","Parameter filename")));
	RegisterTermCmd(cmd,Kernel::Instance()->RootConfig());
}

SLSnmpManager::~SLSnmpManager()
{ 
    while(m_lstMonitors.size()){
	SLSnmpMonitor* p = m_lstMonitors.front();
	m_lstMonitors.pop_front();
	delete p;
    }
}

void SLSnmpManager::AddMonitor(SLSnmpMonitor *pmon)
{
	if(!pmon) return;
	m_lstMonitors.push_back(pmon);
	
	pmon->m_nIndex = m_lstMonitors.size();
	pmon->Init();
}

void SLSnmpManager::InitMonitors()
{
	SLInterfaceMonitor::Create();
	SLApplicationMonitor::Create();
	SLVoiceMonitor::Create();
}

SLSnmpManager* SLSnmpManager::Create()
{
	if(slsnmpManager==NULL)	
		slsnmpManager=new SLSnmpManager();
	
	return slsnmpManager;
}

SLSnmpManager* SLSnmpManager::Instance()
{
	return slsnmpManager;
}

SnmpMonitorList::iterator SLSnmpManager::GetFirstMonitorPos()
{
	return m_lstMonitors.begin();
}

SLSnmpMonitor* SLSnmpManager::GetNext(SnmpMonitorList::iterator &pos)
{

    if(pos != m_lstMonitors.end()){
	SLSnmpMonitor* res = *pos;
	pos++;

	return res;
    }

    return NULL;
}

void SLSnmpManager::CreateMIB(const char *filename)
{
	int counter = 0;
	SnmpMonitorList::iterator pos;
	SLSnmpMonitor* pmon;
	char buff[256];
#ifdef __linux__
	time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	
	
#else	
	SYSTEMTIME st;
#endif

	char* months[] = {"", "January", 
						"February", 
						"March", 
						"April",
						"May",
						"June",
						"July",
						"August",
						"September",
						"October",
						"November",
						"December"};

	char* days[] = { "Sunday",
					"Monday", 
					"Tuesday",
					"Wednesday",
					"Thursday",
					"Friday",
					"Saturday"};
						
	
	FILE* file = fopen(filename, "w");
	if(!file) return;	
#ifdef __linux__
	 sprintf(buff,"%s",asctime(timeinfo));

#else
	::GetLocalTime(&st);	
	sprintf(buff, "%d:%d:%d  %s, %s %d, %d", st.wHour, 
											st.wMinute, 
											st.wSecond,
											days[st.wDayOfWeek],
											months[st.wMonth],
											st.wDay, 
											st.wYear);
#endif
	
#define printmib(x) fputs(x "\n", file)

	printmib("XGATE-MIB	DEFINITIONS ::= BEGIN\n");
	fputs("-- Created = ", file);
	fputs(buff, file);
	fputs("\n\n", file);
	
	printmib("\tIMPORTS");
	printmib("\t\tenterprises");
	printmib("\t\t\tFROM RFC1155-SMI");
	printmib("\t\tOBJECT-TYPE");
	printmib("\t\t\tFROM RFC-1212");
	printmib("\t\tDisplayString");
	printmib("\t\t\tFROM RFC-1213;\n");

	printmib("\tswitchlab\t\tOBJECT IDENTIFIER ::= {enterprises 2013}");
	printmib("\txgate\t\tOBJECT IDENTIFIER ::= {switchlab 1}");
	printmib("\tothers\t\tOBJECT IDENTIFIER ::= {switchlab 99}\n");

	printmib("\txgDescription OBJECT-TYPE");
	printmib("\t  SYNTAX  DisplayString");
	printmib("\t  ACCESS  read-only");
	printmib("\t  STATUS  mandatory");
	printmib("\t  DESCRIPTION");
        printmib("\t      \"The description of current running xGate machine.\"");
	printmib("\t  ::= {xgate 1}\n");

	printmib("\txgServiceInfoCount OBJECT-TYPE");
  	printmib("\t   SYNTAX  INTEGER");
  	printmib("\t   ACCESS  read-only");
  	printmib("\t   STATUS  mandatory");
  	printmib("\t   DESCRIPTION");
	printmib("\t      \"The number of the available info table in the SNMP table.\"");
	printmib("\t   ::= {xgate 2}\n");

	printmib("\txgProcessorUtil OBJECT-TYPE");
  	printmib("\t   SYNTAX  INTEGER");
  	printmib("\t   ACCESS  read-only");
  	printmib("\t   STATUS  mandatory");
  	printmib("\t   DESCRIPTION");
	printmib("\t      \"Processor utilization\"");
	printmib("\t   ::= {xgate 3}\n");

	printmib("\txgMemoryUtil OBJECT-TYPE");
  	printmib("\t   SYNTAX  INTEGER");
  	printmib("\t   ACCESS  read-only");
  	printmib("\t   STATUS  mandatory");
  	printmib("\t   DESCRIPTION");
	printmib("\t      \"Memory utilization\"");
	printmib("\t   ::= {xgate 4}\n");

	printmib("\txgLeastAvailHD OBJECT-TYPE");
  	printmib("\t   SYNTAX  INTEGER");
  	printmib("\t   ACCESS  read-only");
  	printmib("\t   STATUS  mandatory");
  	printmib("\t   DESCRIPTION");
	printmib("\t      \"Memory utilization\"");
	printmib("\t   ::= {xgate 5}\n");

	pos = GetFirstMonitorPos();
	counter = 0;

	while(pos!=GetLastMonitorPost()){
	    pmon = GetNext(pos);
	    if(pmon){
		sprintf(buff, "\t%-20s\t\tOBJECT IDENTIFIER ::= {xgate %d}\n", pmon->Name(), ++counter+10);
		fputs(buff, file);
	    }
	}
	
	fputs("\n", file);
	SLPropEnum spe;

	pos = GetFirstMonitorPos();
	counter = 0;
	while(pos != GetLastMonitorPost())
	{
		pmon = GetNext(pos);
		{			
			fputs("\t", file);
			fputs(pmon->Prefix(), file);
			fputs("Number\tOBJECT-TYPE\n", file);
			fputs("\tSYNTAX\tINTEGER\n", file);	
			fputs("\tACCESS\tread-only\n", file);
			fputs("\tSTATUS\tmandatory\n", file);
			fputs("\tDESCRIPTION\n", file);
			fputs("\t\t\"\"\n", file);
			fputs("\t ::= { ", file);
			fputs(pmon->Name(), file);
			fputs(" 1 }\n\n", file);

			fputs("\t", file);
			fputs(pmon->Prefix(), file);
			fputs("Table\tOBJECT-TYPE\n", file);
			fputs("\tSYNTAX\tSEQUENCE OF ", file);	
			fputs(pmon->Prefix(), file);
			fputs("Entry\n", file);			
			fputs("\tACCESS\tnot-accessible\n", file);
			fputs("\tSTATUS\tmandatory\n", file);
			fputs("\tDESCRIPTION\n", file);
			fputs("\t\t\"\"\n", file);
			fputs("\t ::= { ", file);
			fputs(pmon->Name(), file);
			fputs(" 2 }\n\n", file);

			fputs("\t", file);
			fputs(pmon->Prefix(), file);
			fputs("Entry\tOBJECT-TYPE\n", file);
			fputs("\tSYNTAX\t", file);	
			fputs(pmon->Prefix(), file);
			fputs("Entry\n", file);
			fputs("\tACCESS\tnot-accessible\n", file);
			fputs("\tSTATUS\tmandatory\n", file);
			fputs("\tDESCRIPTION\n", file);
			fputs("\t\t\"\"\n", file);
			fputs("\tINDEX { ", file);
			fputs(pmon->Prefix(), file);
			fputs("Index }\n", file);
			fputs("\t ::= { ", file);
			fputs(pmon->Prefix(), file);
			fputs("Table 1 }\n\n", file);

			fputs("\t", file);
			fputs(pmon->Prefix(), file);
			fputs("Entry ::= \n", file);
			fputs("\t\tSEQUENCE { \n", file);
			
			pmon->EnumProperty((void*)file, SLSnmpManager::EnumProc);

			fputs("\n\t\t}\n\n", file);

			spe.counter = 1;
			spe.file = file;
			sprintf(spe.name, "%sEntry", pmon->Prefix());

			pmon->EnumProperty((void*)&spe, SLSnmpManager::EnumProcDetails);
		}
	}

	printmib("END\n\n--- end of file\n");

	fclose(file);
}

void SLSnmpManager::EnumProc(const char *name, int type, const char *desc, void *data, bool last)
{
    FILE* file = (FILE*) data;

    fputs("\t\t", file);
    fputs(name, file);
    fputs("\n", file);
    fputs("\t\t\t", file);
	
    switch(type)
    {
    case 1:
	fputs("INTEGER", file);
		break;
    case 2:
	fputs("DisplayString", file);
	break;
    }

    if(!last)
	fputs(",\n", file);
    else
	fputs("\n", file);
}

void SLSnmpManager::EnumProcDetails(const char *name, int type, const char *desc, void *data, bool last)
{
	SLSnmpManager::SLPropEnum* ptr = (SLSnmpManager::SLPropEnum*) data;
	if(!ptr) return;

	fputs("\t",ptr->file);
	fputs(name,ptr->file);
	fputs("\t",ptr->file);
	fputs("OBJECT-TYPE\n",ptr->file);
	fputs("\tSYNTAX",ptr->file);
	fputs("\t",ptr->file);
	switch(type)
	{
	case 1:
		fputs("INTEGER\n",ptr->file);
		break;
	case 2:
		fputs("DisplayString (SIZE(0..255))\n",ptr->file);
		break;
	}

	fputs("\tACCESS\tread-only\n", ptr->file);
	fputs("\tSTATUS\tmandatory\n", ptr->file);
	fputs("\tDESCRIPTION\n",ptr->file);
	fputs("\t",ptr->file);
	fputs("\t",ptr->file);
	fputs("\"",ptr->file);
	fputs(desc,ptr->file);
	fputs("\"\n",ptr->file);

	char buff[100];
	sprintf(buff, "\t\t::= { %s %d }\n\n", ptr->name, ptr->counter++);
	fputs(buff,ptr->file);
	fputs("\n\n",ptr->file);
}

void SLSnmpManager::DoTermCmd(Term_Session *session, bool no, Term_Cmd *cmd)
{
	if(session==NULL||cmd==NULL) return;

	switch(cmd->GetId())
	{
	case Term_Cmd_Snmp:
		session->SetRoot(_cfgTree);
		break;
	case Term_Cmd_DumpMib:
		{
			CVariant v1;
			cmd->GetArg((size_t)0)->GetValue(v1);
			String pname = v1.ToString();

			CreateMIB(pname.c_str());
		}
		break;
	}
}

void SLSnmpManager::Destroy()
{
    assert(slsnmpManager!=NULL);
    delete slsnmpManager;
}

SnmpMonitorList::iterator SLSnmpManager::GetLastMonitorPost()
{
    return m_lstMonitors.end();
}

int SLSnmpManager::GetMonitorCount()
{
    return m_lstMonitors.size();
}
