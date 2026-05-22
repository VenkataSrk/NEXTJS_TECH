// $Header: /xGate/RapParam.h 5     5/03/01 10:42p Iwanj $

#ifndef INC_RAPPARAM
#define INC_RAPPARAM

#include <string>
using std::string;

struct RapParam
{
    enum ResourceStatus
    {
	Rs_Unknown = 0,
	Rs_Enabled = 1,
	Rs_Disabled = 2
    };

    struct RII
    {
	RII();
	RII(const string& pid,unsigned qty,unsigned status);

	string portId_;
	unsigned quantity_;
	unsigned status_;
    };

    struct ARQ
    {
	ARQ();
	ARQ(const string& pid,const string& sid,const string& cli,const string& ddi,int dir);

	string portId_,sessionId_,cli_,ddi_;
	int direction_;
    };

    struct DRQ
    {
	DRQ();
	DRQ(
	    const string& pid,
	    const string& sid,
	    const string& cli,
	    const string& ddi,
	    int dir,
	    unsigned holdTime,
	    unsigned connTime,
	    unsigned reason
	    );

	string portId_,sessionId_,cli_,ddi_;
	int direction_;
	unsigned holdTime_,connTime_,reason_;
    };

    struct LRQ
    {
	LRQ();
	LRQ(const char* name,int type,int weight,const char* hints);

	string name_,hints_;
	int weight_;
	int type_;
    };

    struct IRQ
    {
	IRQ();
	IRQ(const char* unit,const char* port);

	string unit_,port_;
    };
};

#endif
