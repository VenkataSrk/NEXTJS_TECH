#ifndef GMSC_PARAM_OPERATION
#define GMSC_PARAM_OPERATION

#include "CAUtilityInternal.h"

#define 	TRUE 1
#define 	FALSE 0
#define 	MAX_ERROR 3

enum class Status
{
    OK=0,
    NOT_OK=-1,
};
enum class CallRetCode
{
    CRC_OK=0
};
enum class Exception
{
    E_LINE_DROP=1,
    E_DIAL_STATUS,
    E_HANGUP
};
enum class CallState
{
    CS_NULL=0,
    CS_PROCEEDING=3,
    CS_ALERTING=4,
    CS_CONNECTED=6,
    CS_DISCONNECTED=8,
};
enum class ConnectionType
{
    CCT_SPEECH=0,
    CCT_3KAUDIO=1,
    CCT_64KUNRST=2
};
enum class SubscriberStatus
{
    SUBS_STATUS_UNKNOWN=-1,
    SUBS_STATUS_BLOCKED=0,
    SUBS_STATUS_ACTIVE=1,
    SUBS_STATUS_PURGED=2,
    SUBS_STATUS_NOTACTIVE=3,
};
enum class CallForwardType
{
    CFWD_CFU=33,
    CFWD_CFB=41,
    CFWD_CFNRY=42,
    CFWD_CFNRC=43,
};
enum class CallForwardReason
{
    CFCC_CFU=3,
    CFCC_CFNRC=6,
};
enum class BasicService
{
    TELE_SERVICE=131,
    BEARER_SERVICE=130,
};
enum class ImsiType
{
    IMSI_NONROAMING=1,
    IMSI_ROAMING_NORDIC=2,
    IMSI_ROAMING_INTL=3,
};
enum class NonSubsType
{
    NONSUBS_UNDEFINED=0,
    NONSUBS_STANDARD=1,
    NONSUBS_ONWARD=2,
};

class RApp_Info
{
    public:
        RApp_Info()=default;
        ~RApp_Info()=default;
        int taskid;
        std::string dbprofile;
        std::string app_name;
        std::string site_code;  
        std::string switch_code;  
        std::string trace_header;
        bool trace_query;
        int dial_tmout;
        bool logged;
        int dur;
        std::string cdrpath;
        std::string mnpdb;
        int msisdn_mapper_flag;
};
class RCall_Info
{
    public:
        RCall_Info()=default;
        ~RCall_Info()=default;
        int handle; //call handle 
        int start;
        int start_msec; 
        int cnxd;
        int cnxd_msec; 
        int end;
        bool answered;
        int disc_party;
        int cause;
        std::string ddi;  //ddi 
        std::string cli;  //cli 
        std::string xlated_cli;
        int ddi_nbtype;
        int cli_nbtype;
        std::string interface;
        std::string trunkgroup;   //incoming trunk name
        bool isCBcall;
};
class RCall_Param
{
    public:
        RCall_Param()=default;
        ~RCall_Param()=default;
        int handle;
		std::string sessionid;
		int nbcomplete;
		int nb_type;
		std::string OCN;
		std::string RN;
		int RR;
};
class RSubscriber
{
    public:
        RSubscriber()=default;
        ~RSubscriber()=default;
        std::string msisdn;
		std::string imsi;
		std::string msrn;
		std::string vlr;
		std::string msc;
		int imsitype;
		int isCBblock;
		bool isCRBT;
		std::string crbt_file;
        std::string MSRN;
        std::string MSISDN;
		int isRoaming;
		int act;
		int mnrf;
};
class RForwarding
{
    public:
        RForwarding()=default;
        ~RForwarding()=default;
        bool state; // forwarding status enable/disable based from q.p.r.a bit
		std::string ftn_nb;  // forward to number
		int ftn_noa;  // forward to number
		int max_nry;
};
class RAccount
{
    public:
        RAccount()=default;
        ~RAccount()=default;
        float incentive;
		std::string curr;
};
class RMultinumber
{
    public:
        RMultinumber()=default;
        ~RMultinumber()=default;
        int flag;
		std::string prompt;
		std::string errmsg;
		int errcode;
		std::string ussdmsg; 
		std::string prefixdest;
};

#endif
