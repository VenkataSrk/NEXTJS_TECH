// $Header: /xGate/RapParam.cpp 5     5/03/01 10:42p Iwanj $

/*
    $History: RapParam.cpp $
 * 
 * *****************  Version 5  *****************
 * User: Iwanj        Date: 5/03/01    Time: 10:42p
 * Updated in $/xGate
 * change RII, add resource status field
*/
#include "stdafx.h"
#include "RapParam.h"

RapParam::RII::RII():
    portId_(""),
    quantity_(0),
    status_(0)
{
}

RapParam::RII::RII(const string& pid,unsigned qty,unsigned status):
    portId_(pid),
    quantity_(qty),
    status_(status)
{
}

RapParam::ARQ::ARQ():
    portId_(""),
    sessionId_(""),
    cli_(""),
    ddi_(""),
    direction_(0)
{
}

RapParam::ARQ::ARQ(const string& pid,const string& sid,const string& cli,const string& ddi,int dir):
    portId_(pid),
    sessionId_(sid),
    cli_(cli),
    ddi_(ddi),
    direction_(dir)
{
}

RapParam::DRQ::DRQ():
    portId_(""),
    sessionId_(""),
    cli_(""),
    ddi_(""),
    direction_(0),
    holdTime_(0),
    connTime_(0),
    reason_(0)
{
}

RapParam::DRQ::DRQ(const string& pid,const string& sid,const string& cli,const string& ddi,int dir,unsigned holdTime,unsigned connTime,unsigned reason):
    portId_(pid),
    sessionId_(sid),
    cli_(cli),
    ddi_(ddi),
    direction_(dir),
    holdTime_(holdTime),
    connTime_(connTime),
    reason_(reason)
{
}

RapParam::LRQ::LRQ():
    name_(""),
    type_(0),
    weight_(0),
    hints_("")
{
}

RapParam::LRQ::LRQ(const char* name,int type,int weight,const char* hints):
    name_(name),
    type_(type),
    weight_(weight),
    hints_(hints)
{
}


RapParam::IRQ::IRQ():
    unit_(""),
    port_("")
{
}

RapParam::IRQ::IRQ(const char* unit,const char* port):
    unit_(unit),
    port_(port)
{
}
