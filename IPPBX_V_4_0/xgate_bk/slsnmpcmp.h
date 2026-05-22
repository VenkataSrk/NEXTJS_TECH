
#ifndef _SL_SNMP_COMPONENT_H_INCLUDED
#define _SL_SNMP_COMPONENT_H_INCLUDED
#include "skalib/include/ska.h"

#define stdcall __attribute__((stdcall))

interface IGarbageMan : IVoid
{
    virtual void DestroyDllMem(void* ptr)=0;
};

interface ITableEntry
{
	virtual int Type(int id)=0;
	virtual const char* GetString(int id)=0;
	virtual int    GetInt(int id)=0; 

	virtual int PropertiesCount()=0;
};

interface ISnmpTable
{	
	//virtual void __stdcall AddEntryInt(const char* oid, int i, ITableEntry*)=0;	//linux porting
	virtual void stdcall AddEntryInt(const char* oid, int i, ITableEntry*)=0;
	virtual void stdcall AddEntryString(const char* oid, const char* s, ITableEntry*)=0;
	virtual const char* stdcall GetOid()=0;
};

interface ISnmpTable2 : IVoid
{	
	virtual void stdcall AddEntryInt(const char* oid, int i, ITableEntry*)=0;
	virtual void stdcall AddEntryString(const char* oid, const char* s, ITableEntry*)=0;
	virtual const char* stdcall GetOid()=0;
};

interface ISLSnmpAgent : public IVoid
{
	virtual void stdcall Start()=0;
	virtual void stdcall Init()=0;
	virtual ISnmpTable* stdcall RequestNewTable(const char* oid) = 0;
};

interface ISLSnmpAgent2 : public IVoid
{
	virtual void stdcall Start()=0;
	virtual void stdcall Init()=0;
	virtual ISnmpTable* stdcall RequestNewTable(const u_char* oid_base) = 0;
};

#endif //_SL_SNMP_COMPONENT_H_INCLUDED
