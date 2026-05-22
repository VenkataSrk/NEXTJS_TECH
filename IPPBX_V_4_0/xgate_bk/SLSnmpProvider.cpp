// SLSnmpProvider.cpp: implementation of the SLSnmpProvider class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SLSnmpProvider.h"
#include "SLInterfaceMonitor.h"
#include "SLSnmpManager.h"
//#include <snmp.h>
#include "SLsnmp.h" //__linux__
#include "sleepdrv.h"
#include "thread_process.h"
#include "commonutil.h"
#include "math.h"

static unsigned long GetTickCount()
{
	struct timeval tv;
	if( gettimeofday(&tv, NULL) != 0 )
		return 0;
        return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
} 



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



SKAManager g_skaManager;

SLSnmpProvider* SLSnmpProvider::slsnmpProviderInstance = NULL;
unsigned long SLSnmpProvider::m_uIndexCounter=0;

SLSnmpProvider::SLSnmpProvider()
: ServiceProvider ("SNMP", false, true, Lowest_Priority)
{
#ifdef _DEBUG
    m_nPollRes = 10; 
#else
    m_nPollRes = 15*60; // Poll for interface stat every 15 minutes
#endif
    m_pifAgent = NULL;
    m_hThread = NULL;
}

SLSnmpProvider::~SLSnmpProvider()
{
    SLSnmpManager::Destroy();
}

bool SLSnmpProvider::AttachTask(CTask *pTask)
{
	return true;
}

bool SLSnmpProvider::CancelJob(CTask *pTask)
{
	return true;
}

void SLSnmpProvider::DetachTask(CTask *pTask)
{
 
}

void SLSnmpProvider::HandleEvents()
{
#ifdef USE_AGENTPLUS
    if(!Tick()) return;

    ValidateIndex();
#endif
}

bool SLSnmpProvider::Load()
{
#ifdef USE_AGENTPLUS
    if(m_pifAgent) return true;

    g_skaManager.CreateComponent("slnmcom.SLSnmpAgent", (void**)&m_pifAgent);

    if(!m_pifAgent){
	CLog::Detail("SL SNMP Provider","Can not load SNMP agent component: %s", "slnmcom.dll");
	return true;
    }

    SLSnmpProvider::Instance()->m_pifAgent->Init();

    m_hThread = CREATE_THREAD(NULL, 0, 
		(LPTHREAD_START_ROUTINE)SLSnmpProvider::SNMPThread, 
		(LPVOID)this, 
		CREATE_SUSPENDED, &m_nThreadId);
	::SetThreadPriority(m_hThread, THREAD_PRIORITY_NORMAL);
    ResumeThread(m_hThread);
    HandleEvents();		
#endif

    SLSnmpManager::Instance()->InitMonitors();
    return true;
}

bool SLSnmpProvider::Unload()
{
    if(m_pifAgent)
	m_pifAgent->Release();

    m_pifAgent = NULL;

#if 0
    if (m_hThread) TerminateThread(m_hThread, 0);
#else
 //   if (m_hThread) pthread_kill(m_hThread,0);
#endif  //__linux__ Will Implement later

    return true;
}
    
SLSnmpProvider* SLSnmpProvider::Create()
{
    if(slsnmpProviderInstance)
	return slsnmpProviderInstance;

    slsnmpProviderInstance = new SLSnmpProvider();

    slsnmpProviderInstance->m_uTick = GetTickCount();
    return slsnmpProviderInstance;
}

SLSnmpProvider* SLSnmpProvider::Instance()
{
    return slsnmpProviderInstance;
}


#ifdef __cplusplus
extern "C" {
#endif

int GetDefaultComponent(void** piv)
{
	*piv=NULL;
	return SK_OK;
}

#ifdef __cplusplus
}
#endif

void SLSnmpProvider::SNMPThread(LPVOID lpParameter)
{
    if(SLSnmpProvider::Instance()==NULL) return;
    SLSnmpProvider::Instance()->m_pifAgent->Start();
    int i=0;
}


/** It gathers all of available interface and assign it 
    to a specific id to be used in the oid.
 */
bool SLSnmpProvider::Tick()
{
    DWORD t = GetTickCount();
    if(t > m_uTick){
	if((double)(t-m_uTick)/1000>= m_nPollRes){
	    m_uTick=t;   
	    return true;
	}
    }
    else
	m_uTick=t;


    return false;
}


int SLSnmpProvider::IncIndex()
{
    return ++m_uIndexCounter;
}

void SLSnmpProvider::ValidateIndex()
{
    SLSnmpManager* pmgr = SLSnmpManager::Instance();
    if(!pmgr) return;

    SnmpMonitorList::iterator pos = pmgr->GetFirstMonitorPos();

    SLSnmpMonitor* pmon = NULL;

    while(pos != pmgr->GetLastMonitorPost()){
	pmon = pmgr->GetNext(pos);
	pmon->ValidateIndex();
    }
}


void SLSnmpProvider::SetRefreshInterval(int iv)
{
    m_nPollRes = iv;
}

SLSnmpMonitor::SLSnmpMonitor()
{
#ifdef _DEBUG
    assert(SLSnmpManager::Instance());
#endif

#ifdef USE_AGENTPLUS
    m_tblCounter = NULL;
    m_tblEntry	 = NULL;
#endif

    SLSnmpManager::Instance()->AddMonitor(this);
}

void SLSnmpMonitor::AddTableEntry(SLSnmpTableEntry *pobj)
{
    if(!pobj) return;    

    m_lstTables.push_back(pobj);
	
    pobj->m_nIndex = m_lstTables.size();
    pobj->m_nProperties = PropertiesCount();

#ifdef USE_AGENTPLUS
    char oid[64];
    int type;

    if(m_tblEntry == NULL) return;
    
    for(int i=1;i<=pobj->PropertiesCount();i++){

	sprintf(oid, "%s.%d.2.1.%d.%d", SLSNMP_ROOT, m_nIndex, i, pobj->GetIndex());
		
	type = pobj->Type(i);
	
	if(type==1)
	    m_tblEntry->AddEntryInt(oid, 0,pobj);
	else if(type==2)
	    m_tblEntry->AddEntryString(oid, "null",pobj);
    }
    
    ValidateIndex();
#endif
}

ISLSnmpAgent* SLSnmpProvider::GetAgent()
{
	return m_pifAgent;
}

void SLSnmpMonitor::Init()
{
#ifdef USE_AGENTPLUS
    if(SLSnmpProvider::Instance()==NULL) return;
    ISLSnmpAgent* pa = SLSnmpProvider::Instance()->GetAgent();
    if(pa==NULL) return;
	
    char oid[128];
    sprintf(oid, "%s.%d.1", SLSNMP_ROOT, m_nIndex);
    m_tblCounter = pa->RequestNewTable(oid); 

    sprintf(oid, "%s.%d.2", SLSNMP_ROOT, m_nIndex);
    m_tblEntry = pa->RequestNewTable(oid);
#endif
}

void SLSnmpMonitor::ValidateIndex()
{
#ifdef USE_AGENTPLUS
    int count = 0;
    char oid[64];

    for (LstSnmpTables::iterator it = m_lstTables.begin();it != m_lstTables.end();it++) {
	SLSnmpTableEntry* pste = *it;
	if(pste && pste->IsValid()){
	    sprintf(oid, "%s.%d.1.%d", SLSNMP_ROOT, m_nIndex, ++count);
	    m_tblCounter->AddEntryInt(oid, pste->GetIndex(), NULL);
	}	
    }

    sprintf(oid, "%s.%d.1.%d", SLSNMP_ROOT, m_nIndex, 0);
    m_tblCounter->AddEntryInt(oid, count, NULL);
#endif
}

BYTE SLSnmpProvider::GetVal(BYTE action, 
			    BYTE *oid, 
			    BYTE oidlen, 
			    DWORD *nval, // will be filled with value if 
			                 // the type is integer or len of the string 
					 // if the type is string
			    char* ptrval, 
			    UINT vaLen, 
			    BYTE* newoid, 
			    DWORD* newlen)
{
    BYTE c, 
	mon = 1, 	
	prop = 1, 
	index= 1, 
	resType=0;    	

    SnmpMonitorList::iterator pos;
    SLSnmpManager* pmgr = SLSnmpManager::Instance();
    SLSnmpMonitor* pmon=NULL;

    /* Check if requested from the root tree, 2013.1
     * Move to the correct leaf
     */
    if(oidlen < 2)
	mon = 1;
    else
	mon = oid[1];

    /**
     * Check for system variable xGate
     */
    if(mon==1){
	if( action == MIB_ACTION_GETNEXT && oidlen >= 3){	    
	    mon = 2;
	    action = MIB_ACTION_GET;
	}
	else{
	    /* 
	     * Generate prefix for new OID 
	     */

	    *newlen = 0;
	    newoid[*newlen] = 1;    // switchlab.xgate

	    *newlen += 1;
	    newoid[*newlen] = mon;  // switchlab.xgate.xgDescription

	    *newlen += 1;
	    newoid[*newlen] = 0;    // switchlab.xgate.xgDescription.0
		    
	    *newlen += 1;	
	    
	    *nval = strlen("xGate");
	    
	    memcpy(ptrval, "xGate", *nval);

	    return ASN_OCTETSTRING;
	}
    }	
    if(mon==2){
	if( action == MIB_ACTION_GETNEXT && oidlen >= 3){
	    mon = 3;
	    action = MIB_ACTION_GET;
	}
	else{
	    /* 
	     * Generate prefix for new OID 
	     */

	    *newlen = 0;
	    newoid[*newlen] = 1;    // switchlab.xgate

	    *newlen += 1;
	    newoid[*newlen] = mon;  // switchlab.xgate.xgServiceInfoCount 

	    *newlen += 1;
	    newoid[*newlen] = 0;    // switchlab.xgate.xgServiceInfoCount .0
		    
	    *newlen += 1;	

	    *nval = pmgr->GetMonitorCount();

	    return ASN_INTEGER32;
	}
    }
    if(mon==3){
    	if( action == MIB_ACTION_GETNEXT && oidlen >= 3){
	    mon = 4;
	    action = MIB_ACTION_GET;
	}
	else{
	    /* 
	     * Generate prefix for new OID 
	     */

	    *newlen = 0;
	    newoid[*newlen] = 1;    // switchlab.xgate

	    *newlen += 1;
	    newoid[*newlen] = mon;  // switchlab.xgate.xgProcessorUtil 

	    *newlen += 1;
	    newoid[*newlen] = 0;    // switchlab.xgate.xgProcessorUtil.0
		    
	    *newlen += 1;	
	    
	    *nval = CPU_Load_Monitor::Instance()->GetPercentKernelTime(Load_Monitor::OneMinute) +
		    CPU_Load_Monitor::Instance()->GetPercentUserTime(Load_Monitor::OneMinute);

	    return ASN_INTEGER32;
	}
    }

    if(mon==4){
	if( action == MIB_ACTION_GETNEXT && oidlen >= 3){
	    mon = 5;
	    action = MIB_ACTION_GET;
	}
	else{
	    /* 
	     * Generate prefix for new OID 
	     */

	    *newlen = 0;
	    newoid[*newlen] = 1;    // switchlab.xgate

	    *newlen += 1;
	    newoid[*newlen] = mon;  // switchlab.xgate.xgMemAvailMB 

	    *newlen += 1;
	    newoid[*newlen] = 0;    // switchlab.xgate.xgMemAvailMB.0
		    
	    *newlen += 1;	

/*
	    MEMORYSTATUS memstat;
	    GlobalMemoryStatus(&memstat);
	    	    
	    *nval = 100-(int)(((double)memstat.dwAvailPhys/(double)memstat.dwTotalPhys)*100); */ //__linux__ need to implement later

	    return ASN_INTEGER32;
	}
    }	

    if(mon==5){
	if( action == MIB_ACTION_GETNEXT && oidlen >= 3){
	    oid[2] = 1;
	    mon    = 1;
	    index  = 1;
	    prop   = 1;
	}
	else{
	    /* 
	     * Generate prefix for new OID 
	     */

	    *newlen = 0;
	    newoid[*newlen] = 1;    // switchlab.xgate

	    *newlen += 1;
	    newoid[*newlen] = mon;  // switchlab.xgate.xgLeastAvailStorage

	    *newlen += 1;
	    newoid[*newlen] = 0;    // switchlab.xgate.xgLeastAvailStorage.0
		    
	    *newlen += 1;	

	    char drive[]="a:\\";
	    long min=20000;

/* 

	    for(int i=0;i<26;i++){
		drive[0] = 'a' + i;
		UINT drivetype = GetDriveType(drive);

		if(drivetype == DRIVE_FIXED){	    
		    ULARGE_INTEGER availcaller, totbyte, totfree;
			
		    GetDiskFreeSpaceEx(drive, &availcaller, &totbyte, &totfree);
		    
		  //  long availa = (_int64)availcaller.QuadPart/(_int64)pow(2,20);
		    long availa = (int64_t)availcaller.QuadPart/(int64_t)pow(2,20); //__linux__
		    
		    availa<min?min=availa:0;
		}
	    }
*/ //__linux__  Need to implement later

	    *nval = min;

	    return ASN_INTEGER32;
	}
    }	    
    else{
	if(oidlen<3)
	    return 0;        

	mon  = oid[1]-10;
	prop = 0;
    }
   
    //Get the requested interface
    pos = pmgr->GetFirstMonitorPos();
    for(c=0;c<mon;c++)
	pmon = pmgr->GetNext(pos);

    //Can not find monitor
    if(!pmon){

	return 0;
    }

    if(oidlen>=5){ // if it's asking for table entry
	index	= oid[5];	
	prop = oid[4];
    }

    if( action == MIB_ACTION_GETNEXT){

	if(oid[2]==1){
	    if(oidlen > 3){ // Number.0 has reached, move to table entry
		oid[2] = 2;
		index  = 1;	    
		prop   = 1;
	    }
	}
	else if(index < pmon->GetEntryCount())
	    index++;
	else if(prop < pmon->PropertiesCount()){
	    prop++;
	    index = 1;
	}
	else if(mon < pmgr->GetMonitorCount()){	    
	    pmon    = pmgr->GetNext(pos);	    
	    prop    = 1;
	    index   = 1;

	    mon++;
	}
    }	
    
    /* 
     * Generate prefix for new OID 
     */
    {
	*newlen = 0;
	newoid[*newlen] = 1;	    // switchlab.xgate

	*newlen += 1;
	newoid[*newlen] = mon+10;   // switchlab.xgate.[monitor index, xgInteface, xgApplication etc]

	*newlen += 1;
	newoid[*newlen] = oid[2];   // switchlab.xgate.[monitor index].index
    }


    /*
     * Check whether requested for table count or table entry
     */
    if(oid[2]==1){ //
	/*
	 * Complete the oid for Number property
	 */
	{
	    *newlen += 1;
	    newoid[*newlen] = 0; 
	    
	    *newlen += 1;
	}

	resType = ASN_INTEGER32;
	*nval = pmon->GetEntryCount();
    }else{
	/*
	 * Complete the oid for Table entry property
	 */
	{
	    *newlen += 1;
	    newoid[*newlen] = 1;    

	    *newlen += 1;
	    newoid[*newlen] = prop;

	    *newlen += 1;
	    newoid[*newlen] = index;

	    *newlen += 1;
	}

	SLSnmpTableEntry* pse = pmon->GetEntryByIndex(index);

	if(pse){
	    switch(pse->Type(prop))
	    {
	    case 1:
		{
		    resType = ASN_INTEGER32;
		    *nval = pse->GetInt(prop);
		}
		break;
	    case 2:
		{
		    resType = ASN_OCTETSTRING;

		    int nlen = 0;

		    if (pse->GetString(prop))
			nlen  = strlen(pse->GetString(prop));

		    *nval = nlen;

		    if(*nval <= vaLen)
			memcpy(ptrval, pse->GetString(prop), *nval);
		    else
			memcpy(ptrval, pse->GetString(index), vaLen);		
		}
		break;
	    }	
	}
	else
	    resType = 0;
    }
        
    return resType;
}

SLSnmpTableEntry* SLSnmpMonitor::GetEntryByIndex(int id)
{
    SLSnmpTableEntry* pste=NULL;

    for (LstSnmpTables::iterator it = m_lstTables.begin();
	    it != m_lstTables.end();it++){
	if(--id==0)
	    pste = *it;
    }

    if (pste)
	return pste;    

    return NULL;
}

int SLSnmpMonitor::GetEntryCount()
{
    return m_lstTables.size();
}
