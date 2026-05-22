// SLApplicationEntry.cpp: implementation of the SLApplicationEntry class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SLApplicationEntry.h"
#include "kernel.h"

#include "SLApplicationMonitor.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SLApplicationEntry::SLApplicationEntry()
{
	SLApplicationMonitor::Instance()->AddTableEntry(this);
}

SLApplicationEntry::~SLApplicationEntry()
{

}

/**
 Creates the instance os SLApplicationEntry class
 */
SLApplicationEntry* SLApplicationEntry::Create(CT_Application* papp)
{
	SLApplicationEntry* pae = new SLApplicationEntry();
	pae->m_pApp = papp;
	
	pae->m_strName = papp->Name();

	return pae;
}

/**
 Validates the member pointer to CT_Application to avoid invalid
 CT_Application pointer
 */
bool SLApplicationEntry::IsValid()
{
    CT_Application* papp = CT_App_Manager::Instance()->GetApp(IString(m_strName));

    if(papp && papp == m_pApp)
	return true;

    return false;
}

/**
 Get integer value
 */
int SLApplicationEntry::GetInt(int id)
{
    if(!IsValid()) return 0;
    int retval = 0;

    switch(id)
    {
    case 1:
	return 0;
    case 3:
	{
	    unsigned tasks, images;
	    m_pApp->GetAppInfo(images, tasks);
	    return tasks;
	}
	break;
    case 4:
	//ACQUIRE_SYSTEM_LOCK();
	retval = m_pApp->GetTotalBlockTime();
	//RELEASE_SYSTEM_LOCK();
    case 5:
	//ACQUIRE_SYSTEM_LOCK();
	retval = m_pApp->GetTotalDurationTime();
	//RELEASE_SYSTEM_LOCK();
    }

    return retval;
}

/**
 Get string value for specified prop id
 */
const char* SLApplicationEntry::GetString(int id)
{
    if(!IsValid()) return 0;

    switch(id)
    {
    case 2:
	return Name();
    }
	
    return NULL;
}

/**
 The name taken from CT_Application instance
 */
const char* SLApplicationEntry::Name()
{
    return m_strName.c_str();
}

/**
 The type of the value specified by given index 
 */
int SLApplicationEntry::Type(int id)
{
    return SLApplicationMonitor::GetTypeTemplate(id);
}
