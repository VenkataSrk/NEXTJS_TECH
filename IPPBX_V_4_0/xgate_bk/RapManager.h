// $Header: /xGate/RapManager.h 18    18/10/01 18:29 Iwanj $

/*
    $History: RapManager.h $
 * 
 * *****************  Version 18  *****************
 * User: Iwanj        Date: 18/10/01   Time: 18:29
 * Updated in $/xGate
 * add new API ResourceAlloc
 * 
 * *****************  Version 17  *****************
 * User: Iwanj        Date: 9/04/01    Time: 12:31p
 * Updated in $/xGate
 * change lock
 * 
 * *****************  Version 16  *****************
 * User: Iwanj        Date: 7/19/01    Time: 9:05p
 * Updated in $/xGate
 * modif IRQ handling to provide direction and distribution info
 * 
 * *****************  Version 15  *****************
 * User: Iwanj        Date: 7/17/01    Time: 3:30p
 * Updated in $/xGate
 * add enable/no enable
 * 
 * *****************  Version 14  *****************
 * User: Iwanj        Date: 7/17/01    Time: 1:51p
 * Updated in $/xGate
 * keep resourcename in lrq job
 * 
 * *****************  Version 13  *****************
 * User: Iwanj        Date: 7/13/01    Time: 8:01p
 * Updated in $/xGate
 * implement CommitConfiguration
 * 
 * *****************  Version 12  *****************
 * User: Iwanj        Date: 6/27/01    Time: 10:21p
 * Updated in $/xGate
 * 
 * *****************  Version 11  *****************
 * User: Iwanj        Date: 5/25/01    Time: 7:14p
 * Updated in $/xGate
 * add updateJobWaitingTime to avoid blocking forever when reply lost
*/

#ifndef INC_RAPMANAGER
#define INC_RAPMANAGER

#include "serviceprovider.h"
#include "RapCallback.h"
#include "RapParam.h"
#include "RapObserver.h"
#include "api.h"
#include "Critical_Section.h"

class RapClient;
class Rap_Message;
//class RapResourceGuard;
class RapTimer;
class RapSubject;

struct RapResult
{
    RapResult();
    RapResult(int* retcode,char* res1,char* res2);

    int* retcode_;
    char* res1_;
    char* res2_;
};

struct RapJob
{
    enum
    {
	MAX_WAIT_TIME = 7 // 7 seconds
    };

    RapJob();
    RapJob(unsigned seqnum,CTask* task,const RapResult& result,bool pending,const string&);

    unsigned seqnum_;
    CTask* task_;
    RapResult result_;
    bool pending_;
    int waitingTime_;
    string resname_;
};

struct RapAPI : public CAPIProvider
{
    enum ApiId
    {
	Id_ResourceLocate = 0,
	Id_ResourceQuery = 1,
	Id_ResourceAlloc = 2
    };

    enum ApiRetCode
    {
	Rc_Success=0,
	Rc_Timeout=1,
	Rc_Error=2
    };

    RapAPI();
    virtual ~RapAPI();

    // async
    STATUS API_CALL ResourceLocate(    
	CTask *task,
	int *retcode,
	const char* name,
	int type,
	int weight,
	const char* hints,
	char* port,
	char* address);

    // async
    STATUS API_CALL ResourceQuery(
	CTask* task,
	int* retcode,
	const char* unitid,
	const char* portid,
	char* groupid);

    // async
    STATUS API_CALL ResourceAlloc(    
	CTask *task,
	int *retcode,
	const char* res_name,
	int res_type,
	int weight,
	const char* hints,
	int rsv1,
	const char* rsv2,
	char* addr_list);
};

class RapManager:public ServiceProvider,public RapObserver
{
public:
    ~RapManager();

    // interface from ServiceProvider
    virtual bool Load();
    virtual bool Unload();
    virtual void HandleEvents();
    virtual bool CancelJob(CTask*);
    virtual bool AttachTask(CTask*);
    virtual void DetachTask(CTask*);
    virtual bool CommitConfiguration();

    // ctengine-way
    static RapManager* Create();
    static RapManager* Instance();

    // callback for RapTimer
    virtual void onSubjectNotification(RapSubject*);

    // available service
    //void RII(const RapParam::RII&);
    void RII(const RapParam::RII&, int resourceType=0); //rkv
    void ARQ(const RapParam::ARQ&);
    void DRQ(const RapParam::DRQ&);
    void LRQ(CTask*,const RapResult&,const RapParam::LRQ&);
    void IRQ(CTask*,const RapResult&,const RapParam::IRQ&);

    // entry point from callback functor
    void HandleRIC(const Rap_Message&);
    void HandleARJ(const Rap_Message&);
    void HandleLCF(const Rap_Message&);
    void HandleLRJ(const Rap_Message&);
    void HandleRRR(const Rap_Message&);
    void HandleIRR(const Rap_Message&);
    void HandleError(const Rap_Message&);
    void HandleTimeout(const Rap_Message&);

    // show stat
    bool OnShowStatus(String & output);
    bool Enable();
    bool Disable();
    bool IsEnabled() const;

private:
    RapManager();
    RapManager(const RapManager&);
    RapManager& operator=(const RapManager&);

    void UpdateJob(unsigned,RapAPI::ApiRetCode);
    void UpdateJobWaitingTime();
    void ParseInfo(const char*,char&,char&,string&,string&);

    // RAP callback
    struct OnError:public RapCallback 
    {
	void operator()(const Rap_Message& m);
    } onError_;

    struct OnTimeout:public RapCallback
    {
	void operator()(const Rap_Message& m);
    } onTimeout_;

    struct OnRIC:public RapCallback
    {
	void operator()(const Rap_Message& m);
    } onRic_;

    struct OnARJ:public RapCallback
    {
	void operator()(const Rap_Message& m);
    } onArj_;

    struct OnLCF:public RapCallback
    {
	void operator()(const Rap_Message& m);
    } onLcf_;

    struct OnLRJ:public RapCallback
    {
	void operator()(const Rap_Message& m);
    } onLrj_;

    struct OnRRR:public RapCallback
    {
	void operator()(const Rap_Message& m);
    } onRrr_;

    struct OnIRR:public RapCallback
    {
	void operator()(const Rap_Message& m);
    } onIrr_;


    RapClient* rapClient_;
    string unitId_,physAddr_;
    RapAPI* api_;
    List<RapJob*> jobList_;
    static RapManager* instance_;
    Critical_Section critical_section_;
    RapTimer* timer_;
    bool enabled_,initialized_;
};

#endif
