// SLInterfaceEntry.cpp: implementation of the SLInterfaceEntry class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SLInterfaceEntry.h"
#include "SLInterfaceMonitor.h"

CallQualityParameter m_arrQCParam[20] = {    CQ_RX_PACKETS, 
    CQ_RX_BYTES, 
    CQ_RX_RTT, 
    CQ_RX_JITTER, 
    CQ_RX_FRACLOSS, 
    CQ_RX_LOSS, 
    CQ_RX_IS, 
    CQ_RX_ID, 
    CQ_RX_IE, 
    CQ_RX_ITOT, 
    CQ_TX_PACKETS, 
    CQ_TX_BYTES, 
    CQ_TX_RTT, 
    CQ_TX_JITTER, 
    CQ_TX_FRACLOSS, 
    CQ_TX_LOSS, 
    CQ_TX_IS, 
    CQ_TX_ID, 
    CQ_TX_IE, 
    CQ_TX_ITOT
};

CallStatistics m_arrCallStat[8] = {
    CALLSTAT_ATTEMPT , 
    CALLSTAT_SUCCESS, 
    CALLSTAT_FAILED,
    CALLSTAT_CONNECTED,
    CALLSTAT_SCR,
    CALLSTAT_ASR,
    CALLSTAT_AVG_CNKTIME,
    CALLSTAT_AVG_PDD
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SLInterfaceEntry::SLInterfaceEntry(Interface * pif)
{
	m_pInterface = pif;
	m_strName = pif->Name();
	SLInterfaceMonitor::Instance()->AddTableEntry(this);
}

SLInterfaceEntry::~SLInterfaceEntry()
{

}

SLInterfaceEntry* SLInterfaceEntry::Create(Interface * pif)
{
	if (pif==NULL) return NULL;

	SLInterfaceEntry* pie = new SLInterfaceEntry(pif);

	return pie;
}

bool SLInterfaceEntry::IsValid()
{
	if(!m_pInterface) return false;
	if(MediaManager::GetInstance()==NULL)return false;

	Interface* pif = MediaManager::GetInstance()->FindInterface(m_strName.c_str());

	if(pif && pif == m_pInterface)
		return true;

	return false;
}

int	SLInterfaceEntry::Type(int id)
{
	return SLInterfaceMonitor::GetTypeTemplate(id);
}

const char* SLInterfaceEntry::GetString(int id)
{
    static char res [3];
	if(!IsValid()) return "invalid object";

	switch(id)
	{
	case 2:
		return m_pInterface->Name().c_str ();
		break;
	case 3:
		return m_pInterface->GetDescription().c_str();
		break;
	case 4:
		return m_pInterface->GetGroupName();
		break;
	case 5:
		return m_pInterface->GetCircuitReference().c_str() ;
		break;
	case 55:
	    res[0] = (char) m_pInterface->GetDirection();
	    res[1] = (char) m_pInterface->GetDistribution();
	    res[3] = '\0';
	    return res;
		
	}

	return "#id-err#";
}

int	SLInterfaceEntry::GetInt(int id)
{
	if(!IsValid())
		return -1;

	CLog::Detail("SLInterfaceEntry","Get Int: %d", id);		

	if(id>=27 && id<=46){
	    time_t tfr, tto;
	    time(&tto);

	    tfr = tto - 3600;	    

	    float ret = m_pInterface->GetQuality(m_arrQCParam[id-27], 0, 0, true);

	    if(ret>=0)
		ret *= 10;

	    return ret;
	}

	if(id>=47 && id<=54){
	    time_t tfr, tto;
	    time(&tto);

	    tfr = tto - 3600;	    

	    float ret;
	    
	    if(id == 51)
		ret = m_pInterface->GetSCR();
	    else
	    {
		ret = m_pInterface->GetStatistics(m_arrCallStat[id-47], tfr, tto);

		if(ret >=0 && id==52)
		    ret *=100;
	    }

	    return ret;
	}
	
	switch(id)
	{
	case 1:
		return GetIndex();
	case 6:
		return m_pInterface->GetHardwareType();
	case 7:
		return m_pInterface->GetCurrentConnectedCount(CD_Incoming);
	case 8:
		return m_pInterface->GetCurrentBusyCount(CD_Incoming);
	case 9:
		return m_pInterface->GetCurrentConnectedCount(CD_Outgoing);
	case 10:
		return m_pInterface->GetCurrentBusyCount(CD_Outgoing);
	case 11:
		return m_pInterface->GetMediaCount();
	case 12:
	    if( m_pInterface->GetState()==OS_OutOfService)
		return m_pInterface->GetDisableCause()+10;
	    else
		return m_pInterface->GetState();
	case 13:
		return m_pInterface->IsEnabled();
	case 14:
		return m_pInterface->GetTotalCarrierTrans();
	case 15:
		return m_pInterface->GetTotalAttempts(CD_Incoming);
	case 16:
		return m_pInterface->GetTotalConnected(CD_Incoming);
	case 17:
		return m_pInterface->GetTotalSuccess(CD_Incoming);
	case 18:
		return m_pInterface->GetTotalFailed(CD_Incoming);
	case 19:
		return m_pInterface->GetTotalHoldingTime_Lo(CD_Incoming);
	case 20:
		return m_pInterface->GetTotalConnectedTime_Lo(CD_Incoming);
	case 21:
		return m_pInterface->GetTotalAttempts(CD_Outgoing);
	case 22:
		return m_pInterface->GetTotalConnected(CD_Outgoing);
	case 23:
		return m_pInterface->GetTotalSuccess(CD_Outgoing);
	case 24:
		return m_pInterface->GetTotalFailed(CD_Outgoing);
	case 25:
		return m_pInterface->GetTotalHoldingTime_Lo(CD_Outgoing);
	case 26:
		return m_pInterface->GetTotalConnectedTime_Lo(CD_Outgoing);
	case 2:
	case 3:
	case 4:
	case 5:
		return 0;
	}

	return 0;
}

const char* SLInterfaceEntry::Name()
{
	return m_strName.c_str();
}
