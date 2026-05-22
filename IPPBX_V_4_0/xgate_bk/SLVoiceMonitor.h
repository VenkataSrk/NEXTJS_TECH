// SLVoiceMonitor.h: interface for the SLVoiceMonitor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SLVOICEMONITOR_H__8AB78A24_061F_4D34_9E49_F44DDEE378FA__INCLUDED_)
#define AFX_SLVOICEMONITOR_H__8AB78A24_061F_4D34_9E49_F44DDEE378FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SLSnmpProvider.h"

class SLVoiceMonitor : public SLSnmpMonitor  
{
public:
	const char* Prefix();
	virtual int PropertiesCount();
	virtual const char* Name();
	virtual void EnumProperty(void* data, ENUMPROPPROC lpCallback);
	static SLVoiceMonitor* Create();
	static SLVoiceMonitor* Instance();
	SLVoiceMonitor();
	virtual ~SLVoiceMonitor();

protected:
	static SLVoiceMonitor* slvmInstance;

	static const char* m_arrPropNames[];
	static const int  m_arrPropType[];

};

#endif // !defined(AFX_SLVOICEMONITOR_H__8AB78A24_061F_4D34_9E49_F44DDEE378FA__INCLUDED_)
