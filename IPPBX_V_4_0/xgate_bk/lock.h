#pragma once
#ifndef __LOCK__H__INCLUDED__
#define __LOCK__H__INCLUDED__

/*
 *
 */
class Critical_Section
{
public:
    Critical_Section()
    {
	InitializeCriticalSection(&cs);
    }

    ~Critical_Section()
    {
	DeleteCriticalSection(&cs);
    }

    CRITICAL_SECTION cs;
};

/*
 *
 */
class Lock
{
public:
    Lock(Critical_Section* cs)
    {
	pcs = cs;
	EnterCriticalSection(&pcs->cs);
    }

    ~Lock()
    {
	LeaveCriticalSection(&pcs->cs);
    }

    void lock()
    {
	EnterCriticalSection(&pcs->cs);
    }

    void unlock()
    {
	LeaveCriticalSection(&pcs->cs);
    }

private:
    Critical_Section* pcs;

};

#endif __LOCK__H__INCLUDED__