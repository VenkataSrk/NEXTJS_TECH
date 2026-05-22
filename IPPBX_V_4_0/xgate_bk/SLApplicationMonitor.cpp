// SLApplicationMonitor.cpp: implementation of the SLApplicationMonitor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SLApplicationMonitor.h"
//#include "slapplicationentry.h"
#include "SLApplicationEntry.h"
#include "task.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SLApplicationMonitor* SLApplicationMonitor::slsnmpApplication=NULL;
List<SLApplicationEntry *> SLApplicationMonitor::entries_;


const char* SLApplicationMonitor::m_arrPropNames[] = { " ",
						   "xgappIndex",
						   "xgappName",
						   "xgappTaskCreated",
						   "xgappTotalDuration",
						   "xgappBlockedTime",
						};

const int  SLApplicationMonitor::m_arrPropType[]= { 0, 1, 2, 1, 1, 1};


/**
 
 */
SLApplicationMonitor::SLApplicationMonitor()
{

}

SLApplicationMonitor::~SLApplicationMonitor()
{
    while(entries_.size()) {
	SLApplicationEntry * ae = entries_.front();
	entries_.pop_front();
	delete ae;
    }
}

/**
 Gets the singleton instance of this class
 */
SLApplicationMonitor* SLApplicationMonitor::Instance()
{
    return slsnmpApplication;
}

/**
 Create the singleton instance of this class
 */
SLApplicationMonitor* SLApplicationMonitor::Create()
{
    if(slsnmpApplication==NULL)
	slsnmpApplication = new SLApplicationMonitor();

    return slsnmpApplication;
}

/**
 The name of the application to used for the MIB entry
 */
const char* SLApplicationMonitor::Name()
{
    return "xgApplication";
}

/**
 Templates for the number or properties for this kind of MIB entry
 */
int SLApplicationMonitor::PropertiesCount()
{
    return 5;
}

/**
 Enumarates propeties for propterty names, values
 */
void SLApplicationMonitor::EnumProperty(void* data, ENUMPROPPROC lpCallback)
{
    for(int i=1;i<=PropertiesCount();i++)
	lpCallback(m_arrPropNames[i], m_arrPropType[i], " ", data, i==PropertiesCount());
}

/**
 Get prefix for MIB entry
 */
const char* SLApplicationMonitor::Prefix()
{
    return "xgapp";
}

/**
 Create table entry for this monitor/table
 */
SLApplicationEntry* SLApplicationMonitor::CreateTableEntry(CT_Application *papp)
{
    SLApplicationMonitor* pim = Instance();
    if(!pim)return NULL;

    for (LstSnmpTables::iterator it = pim->m_lstTables.begin();
	it != pim->m_lstTables.end();
	it++) {
	SLApplicationEntry* pste = (SLApplicationEntry*)*it;
	if(pste->Name()==papp->Name()){
	    pste->m_pApp = papp;
	    return pste;
	}
    }
    
    SLApplicationEntry * ae = SLApplicationEntry::Create(papp);	

    if (ae) {
	entries_.push_back(ae);
    }

    return ae;
}

int SLApplicationMonitor::GetTypeTemplate(int id)
{
    if(id>Instance()->PropertiesCount()) return 0;

    return m_arrPropType[id];
}
