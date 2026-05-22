// SLVoiceMonitor.cpp: implementation of the SLVoiceMonitor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SLVoiceMonitor.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SLVoiceMonitor* SLVoiceMonitor::slvmInstance=NULL;

const char* SLVoiceMonitor::m_arrPropNames[] = {    " ",
													"xgvoiIndex",
													"xgvoiName",
													"xgvoiDescr",
													"xgvoiHardwareType",
													"xgvoiTotalUsage",
													"xgvoiTotalHolding",
													"xgvoiTotalDetectedDTMF",
													"xgvoiTotalUseTime",
													"xgvoiCurrentUsage",
												};
const int  SLVoiceMonitor::m_arrPropType[] = { 0, 1, 2, 2, 1, 1, 1, 1, 1, 1};

SLVoiceMonitor::SLVoiceMonitor()
{

}

SLVoiceMonitor::~SLVoiceMonitor()
{

}

SLVoiceMonitor* SLVoiceMonitor::Instance()
{
	return slvmInstance;
}

SLVoiceMonitor* SLVoiceMonitor::Create()
{
	if(!slvmInstance)
	{
		slvmInstance = new SLVoiceMonitor();
	}

	return slvmInstance;
}

void SLVoiceMonitor::EnumProperty(void* data, ENUMPROPPROC lpCallback)
{
    for(int i=1;i<=PropertiesCount();i++)
	lpCallback(m_arrPropNames[i], m_arrPropType[i], " ", data, i==PropertiesCount());
}

const char* SLVoiceMonitor::Name()
{
	return "xgVoice";
}

int SLVoiceMonitor::PropertiesCount()
{
	return 9;
}

const char* SLVoiceMonitor::Prefix()
{
	return "xgvoi";
}
