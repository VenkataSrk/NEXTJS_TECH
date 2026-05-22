// SLInterfaceMonitor.cpp: implementation of the SLInterfaceMonitor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SLInterfaceMonitor.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
const int SLInterfaceMonitor::m_arrPropType[] = 
	{0, 1, 2, 2, 2, 2, 1, 1, 1, 1, 
	 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	 1, 1, 1, 1, 1, 2
	};		    

const char* SLInterfaceMonitor::m_arrPropNames[]= { "",
		"xgifIndex",
		"xgifName",
		"xgifDesc",
		"xgifGroup",
		"xgifCircuitRef",
		"xgifHardwareType",
		"xgifIC",
		"xgifIB",
		"xgifOC",
		"xgifOB",
		"xgifIdle",
		"xgifState",
		"xgifEnabled",
		"xgifCrTrans",
		"xgifTotNumberIn",
		"xgifTotConnIn",
		"xgifTotSuccIn",
		"xgifTotFailedIn",
		"xgifHoldingTimeIn",
		"xgifConnTimeIn",
		"xgifTotNumberOut",
		"xgifTotConnOut",
		"xgifTotSuccOut",
		"xgifTotFailedOut",
		"xgifHoldingTimeOut",
		"xgifConnTimeOut",
		"xgifRxPackets",
		"xgifRxBytes", 
		"xgifRxRTT", 
		"xgifRxJitter", 
		"xgifRxFracLoss",
		"xgifRxLoss", 
		"xgifRxIS", 
		"xgifRxID",
		"xgifRxIE",
		"xgifRxItot",
		"xgifTxPackets",
		"xgifTxBytes", 
		"xgifTxRTT", 
		"xgifTxJitter", 
		"xgifTxFracLoss",
		"xgifTxLoss", 
		"xgifTxIS", 
		"xgifTxID",
		"xgifTxIE",
		"xgifTxItot",
		"xgifAttempt", 
		"xgifSuccess", 
		"xgifFailed",
		"xgifConnected",
		"xgifSCR",
		"xgifASR",
		"xgifConnectedTime", 
		"xgifAvgPDD",		
		"xgifDD"

	}; 

SLInterfaceMonitor::SLInterfaceMonitor()
{
	
}

SLInterfaceMonitor::~SLInterfaceMonitor()
{
    while(entries_.size()) {
	SLInterfaceEntry * ie = entries_.front();
	entries_.pop_front();
	delete ie;
    }
}
SLInterfaceMonitor* SLInterfaceMonitor::slifmonInstance=NULL;

List<SLInterfaceEntry*> SLInterfaceMonitor::entries_;

SLInterfaceMonitor* SLInterfaceMonitor::Create()
{
    if(slifmonInstance)
	return slifmonInstance;

    slifmonInstance = new SLInterfaceMonitor();

    return slifmonInstance;
}

const char* SLInterfaceMonitor::Name()
{
    return "xgInterface";
}

int SLInterfaceMonitor::PropertiesCount()
{
    return 55;
}

void SLInterfaceMonitor::EnumProperty(void* data, ENUMPROPPROC lpCallback)
{
    for(int i=1;i<=PropertiesCount();i++)
        lpCallback(m_arrPropNames[i], m_arrPropType[i], " ", data, i==PropertiesCount());
}

SLInterfaceEntry* SLInterfaceMonitor::CreateTableEntry(Interface *pif)
{
    SLInterfaceMonitor* pim = Instance();
    if(!pim)return NULL;
    if(pif==NULL) return NULL;

    for (LstSnmpTables::iterator it = pim->m_lstTables.begin();
    	 it != pim->m_lstTables.end();
	 it++) 
    {
	SLInterfaceEntry* pste = (SLInterfaceEntry*)*it;
	if(pste->Name()==pif->Name())
	{
		pste->m_pInterface = pif;
		return pste;
	}
    }
    
    SLInterfaceEntry * ie = SLInterfaceEntry::Create(pif);

    if (ie) {
	entries_.push_back(ie);
    }

    return ie;
}

const char* SLInterfaceMonitor::Prefix()
{
	return "xgif";
}

int SLInterfaceMonitor::GetTypeTemplate(int id)
{
	if(Instance()==NULL) return 0;

	if(id>Instance()->PropertiesCount()) return 0;

	return m_arrPropType[id];

}

