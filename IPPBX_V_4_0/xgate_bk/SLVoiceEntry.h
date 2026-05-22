// SLVoiceEntry.h: interface for the SLVoiceEntry class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SLVOICEENTRY_H__CE0EE4BC_169F_4505_B016_CC517A6173F9__INCLUDED_)
#define AFX_SLVOICEENTRY_H__CE0EE4BC_169F_4505_B016_CC517A6173F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SLSnmpProvider.h"

class SLVoiceEntry : public SLSnmpTableEntry  
{
public:
	SLVoiceEntry();
	virtual ~SLVoiceEntry();

};

#endif // !defined(AFX_SLVOICEENTRY_H__CE0EE4BC_169F_4505_B016_CC517A6173F9__INCLUDED_)
