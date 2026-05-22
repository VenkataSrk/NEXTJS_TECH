/* $Header: /xGate/dns.h  $
*   
* XGATE
* (C) 2013 Switchlab, Ltd
*
* Small DNS Library.
*
* $Log: /xGate/dns.h $
* 
* 0     09/01/13 10:41AM Dandi/Oky
* Initial Version
*/


#ifndef __XGATE_DNS_H__
#define __XGATE_DNS_H__
//#endif

#ifdef __GNUC__
#pragma implementation "pdns.h"
#endif

/* #if P_DNS	commented for linux porting
#ifndef _PDNS_H
#define _PDNS_H
#endif */

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptlib/sockets.h>

#include <ptclib/random.h>
#include <ptclib/url.h>

//#include "ServiceProvider.h"
#include "serviceprovider.h"
//#include "windns.h"
#include "cfg.h"
#include "api.h"
#include "termcmd.h"
#include "task.h"
#include <set>

#include <pdns.h>

#include <ace/Synch.h>
#include <ptclib/pdns.h>
#include "commonutil.h"
//#define __stdcall __attribute__((stdcall))
//#include "types.h"

#define DNS_TYPE_A  1	//linux porting
#define DNS_TYPE_SRV	33	//linux porting

#define XGATE_HAS_DNSSERVICE

//-----------------------------------------------------------------------------
// Maximum opened DNS Request handles by a task
// TODO: make it configurable
//
#define DNS_MAX_REQUEST_THREADS 10
#define DNS_MAX_HOSTNAME_LENGTH 400
#define DNS_MAX_SLOT_REQUEST 10000

#define DNSDRV	"dnsdriver"

//typedef unsigned long DWORD, *PDWORD, *LPDWORD;
//typedef unsigned long DWORD;
//typedef DWORD IP4_ADDRESS;	//linux porting
typedef unsigned long IP4_ADDRESS;	//linux porting
//typedef ACE_Token ACE_Thread_Mutex;
//typedef ACE_Mutex ACE_Thread_Mutex;


/* Forward declaration.
*/

class DnsRecord;
typedef DnsRecord * PDNS_RECORD;
//extern void DnsRecordListFree(PDNS_RECORD rec, int FreeType);


class ACE_Semaphore;

class CTask;

class DNS_API;
class DNS_SRV_Job;
class DNS_A_Job;
class DNS_Job;
class DNS_Worker_Threads_Manager;


enum DNS_Job_Status{
    DNS_JOB_CREATED =0,
	DNS_JOB_PENDING,
	DNS_JOB_ACTIVE,
	DNS_JOB_CANCELED,
	DNS_JOB_SUCCESS,
	DNS_JOB_ABORTED
};


/* 
*/
typedef struct _dnshost {
    String hostname;
    int timeout;
} Hostdata;

typedef List<Hostdata> List_DNS_Data;

// struct for holding one SRV result
typedef struct DNS_SRV_RESULT
{
    char HostName[DNS_MAX_HOSTNAME_LENGTH];
    IP4_ADDRESS IPAddress;
    DWORD Port;
    DWORD Priority;
    DWORD Weight;
} DNS_SRV_RESULT;

// list for holding SRV results from single SRV request
typedef List<DNS_SRV_RESULT> DNS_SRV_RESULT_LIST;
typedef List<DNS_Job*> List_DNS_Jobs;


#define DNS_MAX_REQUEST_THREADS 10
#define DNS_MAX_HOSTNAME_LENGTH 400


#define DNS_A_JOB 1
#define DNS_SRV_JOB 2


enum DNS_Term_Cmd {
    DNS_Term_Cmd_Set_DNS = 0,
    DNS_Term_Cmd_Remove_DNS,
    DNS_Term_Cmd_View_DNS,
    DNS_Term_Cmd_Conf,
};


// This function for linux porting linking issue and 
// relative function in "xgate/branches/Sofia/full/pwlib/src/ptclib/pdns.cxx"
static void _DnsRecordListFree(PDNS_RECORD rec, int /* FreeType */)
{
  while (rec != NULL) {
    PDNS_RECORD next = rec->pNext;
    free(rec);
    rec = next;
  }
}


// One object of this class represents one DNS query job(eg. A query, SRV query).
// To implement a type of DNS query, just inherit this class and do actual process
// in the virtual function called doTheJob() -okY
class DNS_Job
{
public:

    CTask *_task;
    int _task_pid;
    int _request_id;
    
    // constructor, set status to DNS_JOB_CREATED
    DNS_Job(CTask *task)
    {
	    _status = DNS_JOB_CREATED;
		if(task!=NULL)
	        _task_pid = task->GetPID();
	    _task=task;
    }

    // return the status
    DNS_Job_Status GetStatus()
    {
	    return _status;
    }
    
  
    // descendant should implement this(return type based on DNS query
    // type, eg. DNS_TYPE_A, DNS_TYPE_SRV, etc
    virtual BYTE GetType()=0;
    
    // destroy object, assert error if Job destroyed when
    // _status=[DNS_JOB_PENDING, DNS_JOB_ACTIVE, DNS_JOB_CANCELED]
    virtual ~DNS_Job()
    {
	//if((_status!=DNS_JOB_SUCCESS)&&(_status!=DNS_JOB_ABORT))
	// assert?
    }
    
protected:
    // this virtual method should be implemented by descendant.
    // be aware that this function will be called by worker thread, thus data
    // access synchronization must be noted
    virtual DNS_Job_Status doTheJob(){return DNS_JOB_SUCCESS;}
    
private:
    // this status field will be updated directly from thread manager
    DNS_Job_Status _status;
    
    friend class DNS_Worker_Threads_Manager;
};


// Class represents concrete implementation of A query job
class DNS_A_Job: public DNS_Job
{
public:  
    
    DNS_A_Job(const char *hostname, CTask *task):DNS_Job(task)
    {
	strcpy(_hostname, hostname);
	pDnsRecord=NULL;
    }
    
    IP4_ADDRESS Get_IP_Address()
    {
	    assert(pDnsRecord!=NULL);
	
	    return pDnsRecord->Data.A.IpAddress;
    }
    
    virtual char * GetHostname()
    {
	    return _hostname;
    }
    
    virtual BYTE GetType()
    {
	    return DNS_TYPE_A;
    }
    
    virtual ~DNS_A_Job()
    {
	//if(GetStatus()==DNS_JOB_SUCCESS)
	    //DnsRecordListFree(pDnsRecord, DnsFreeRecordList);

	if(GetStatus()==DNS_JOB_SUCCESS)
	    _DnsRecordListFree(pDnsRecord, DnsFreeRecordList);
    }
    
protected:
   DnsRecord* PDNS_RECORD; 	//linux porting
    //PDNS_RECORD pDnsRecord;
    DnsRecord	*pDnsRecord;	//linux porting
    //DnsRecord * PDNS_RECORD;
    
    // do the specified job
    virtual DNS_Job_Status doTheJob()
    {
	    int result;
	
	    // call synchronous DNS request
	    //result=DnsQuery(_hostname, DNS_TYPE_A, DNS_QUERY_STANDARD, NULL, &pDnsRecord, NULL); //commented for linux porting
	    //result=res_query(_hostname, DNS_TYPE_A, DNS_QUERY_STANDARD, pDnsRecord,sizeof(pDnsRecord));
		//len = res_query(host, C_IN, T_MX, &answer, sizeof(answer));
	    //int res_query(const char *dname, int class, int type,unsigned char *answer, int anslen);
	
	    if(result)
	        return DNS_JOB_ABORTED;
	    else
	        return DNS_JOB_SUCCESS;
    }
    
private:
    char _hostname[DNS_MAX_HOSTNAME_LENGTH];    
};

// Class represents concrete implementation of SRV query job
class DNS_SRV_Job: public DNS_Job
{
public:
    DNS_SRV_RESULT_LIST Results;
    
    
    DNS_SRV_Job(const char *hostname, CTask *task):DNS_Job(task)
    {
	    strcpy(_hostname, hostname);
    }
    
    virtual BYTE GetType()
    {
	    return DNS_TYPE_SRV;
    }
    
    virtual char * GetHostname()
    {
	    return _hostname;
    }
    
    virtual ~DNS_SRV_Job()
    {
	    Results.clear();
    }
    
protected:
    // do the specified job
    virtual DNS_Job_Status doTheJob()
    {
	    int result;
	    PDNS_RECORD pDnsRecord,p,pA=NULL;
	
	    // synchronous SRV request
	    //result=DnsQuery(_hostname, DNS_TYPE_SRV, DNS_QUERY_STANDARD, NULL, &pDnsRecord, NULL);	//commented for linux porting
	
	    if(result)
	        return DNS_JOB_ABORTED;
	
	    // lets iterate all results
	    p=pDnsRecord;
	    while(p!=NULL)
		{
	        // assert(p->wType==DNS_TYPE_SRV);
			if(p->wType==DNS_TYPE_SRV)
			{
	        DNS_SRV_RESULT res;
	        strcpy(res.HostName, p->Data.SRV.pNameTarget);
	        res.Port=p->Data.SRV.wPort;
	        res.Priority=p->Data.SRV.wPriority;
	        res.Weight=p->Data.SRV.wWeight;
	        // resolve host name to IP address
	        // (another in-thread synchronous process)
	        //result=DnsQuery(p->Data.SRV.pNameTarget, DNS_TYPE_A, DNS_QUERY_STANDARD, NULL, &pA, NULL);
	        if(!result)
			{
		        res.IPAddress=pA->Data.A.IpAddress;
		        _DnsRecordListFree(pA, DnsFreeRecordList);
			}
	        else
		        res.IPAddress=0;// IP address unresolveable
	    
	        // store the result
	        Results.push_front(res);

			}
			// next data, please
	        p=p->pNext;
		}
	
	    // destroy record list
	    _DnsRecordListFree(pDnsRecord, DnsFreeRecordList);
	
	    return DNS_JOB_SUCCESS;
    }
    
    
    
private:    
    char _hostname[DNS_MAX_HOSTNAME_LENGTH];
};



/** DNS_Service: the singleton entity that manages all DNS 
driver instances
*/

class DNS_Service : public ServiceProvider
{
public:
    
    enum DNS_Term_Cmd {
	DNS_Term_Cmd_Set_DNS = 0,
	DNS_Term_Cmd_Remove_DNS,

    };

    List_DNS_Data dnshostname_list;
    List_DNS_Jobs registered_jobs;
    
    // public destructor
    virtual ~DNS_Service();
    
    /** Create a singleton instance of DNS_Service
    */
    static DNS_Service* Create();
    
    /** Returns the only instance of DNS_Service. 
    The service manager must have been instantiated with Create() prior
    to calling this function.
    */
    static DNS_Service* Instance()
    {
	return dnsSvcInstance;
    }
    
    /** GetConfigTree: Returns a pointer to the configuration tree of
    interface sub commands
    */
    Cfg_Tree_Group* GetConfigTree() 
    {
	return _cfgTree; 
    }
    
    /** HandleEvents: Descendent shall implement this abstract method.
    It is called by Service Provider Manager
    */
    virtual void HandleEvents();
    
    /** This function will be called whenever a new task is created.
    */
    bool AttachTask(CTask *);
    
    /** This function will be called whenever a task is about to be deleted.
    */
    void DetachTask(CTask *);
    
    /** Cancelling function. */
    virtual bool CancelJob(CTask *);
    
    /** Show status. */
    virtual bool OnShowStatus(String & output);
    
    void AddPendingTask(unsigned pid);
    void RemovePendingTask(unsigned pid);
    void RemovePendingTask_NoLock(unsigned pid);
    bool HasPendingTask(unsigned pid);
    bool HasPendingTask_NoLock(unsigned pid);
    void AddCompletedTask_NoLock(CTask *task);
    
    void LockPendingTask();
    void UnlockPendingTask();
    
        
    STATUS API_ResolveDNS(CTask *task, int* rethnd, const char *hostname, int type);
    STATUS API_FetchResolvedADNS(CTask *task, char *IPaddr, int rethnd);
    STATUS API_StatusDNS(CTask *task, int rethnd, int *status);
    STATUS API_CloseDNS(CTask *task, int rethnd);
    STATUS API_FetchResolvedSRVDNS (CTask *task, char *ret, int rethnd, int*port, int*prio, int*weight); 

private:
    
    static DNS_Service *dnsSvcInstance;
    int	   m_TLS;    
    
    /**/
    DNS_Worker_Threads_Manager *m_Worker;
    DNS_API *m_API;

    
    ACE_Thread_Mutex m_TaskMutex_;
    std::set<unsigned> m_PendingTasks_;
    std::set<CTask*> m_CompletedTasks_;
    Cfg_Tree_Group* _cfgTree;
    
    // Internal representation of task's opened request handles
    class CTaskState {
	CTaskState();
	
	int		FindSlot();
	void		ReleaseRequest(char *hostaname, int *request_id);
	int*		m_Handle[DNS_MAX_SLOT_REQUEST];
	friend class	DNS_Service;
    };
    
    // private constructor
    DNS_Service();
    
    /** This function will be called by the service framework after 
    instantiation of this class, to allow the instance to initialize
    itself.
    */
    virtual bool Load();
    
    /** This function will be called by the service framework before it is
    destroyed/deleted, to allow the instance to cleanup itself.
    */
    virtual bool Unload();
    
    
    /** Show configuration for the specified entry.
    This function will be called by the framework whenever it needs to display the
    specified entry. Previously this instance should tell the entry that it wishes to
    handle displaying the entry by calling entry->RegisterShowHandler().
    */
    //virtual STATUS OnShowConfig(
    virtual bool OnShowConfig(
	const Cfg_Tree_Entry* entry,
	String & output, 
	const String & indent, 
	int indent_size) const;
    
	/** Terminal command handler
    */
    void DoTermCmd(Term_Session* session, bool no, Term_Cmd* cmd);


    bool InsertHostnameRule(Term_Session *session, String hostname, int timeout);
    bool UpdateHostnameRule(Term_Session *session, String hostname, int timeout);
    bool DeleteHostnameRule(Term_Session *session, String hostname);
    bool ViewHostnameRule(Term_Session *session, int maxhost);

    
    private:
	friend class CTaskState;
};



// singleton class that manage all DNS working threads.
// the only instance of this class will distribute worker threads
// needed by DNS service provider by submit it(in form of DNS_Job) via 
// SubmitJob() -okY
class DNS_Worker_Threads_Manager
{
public:
    // return singleton instance
    static DNS_Worker_Threads_Manager* Instance();
    
    // constructor, publish itself to the world
    DNS_Worker_Threads_Manager();
    
    ~DNS_Worker_Threads_Manager();
    
    // singleton instantiation and threads creation,
    // should be called in xgate initialization(kernel?)
    static bool Create();
    
    // return true if there is no job
    bool JobIsEmpty();

	// This static method cancel the specified job.
    // from memory management point of view, this function equiv to delete job;
    // After return, the caller must not reference the object anymore.
    void CancelJob(DNS_Job* job);
    
    // call this function to shutdown all threads.
    // object will be ready for deletion then.
    static void Shutdown();
    
    // Submit a DNS_Job, will be stored to PendingJobs list, and then eventually
    // will be follow up by one of request_threads(moved to ActiveJobs list)
    void SubmitJob(DNS_Job* job);

    static DNS_Job *FindJob(DNS_Job *rethnd);
    
    // ThreadStarter and ThreadMain will be executed will be executed by
    // (DNS_MAX_REQUEST_THREADS) separate threads
    static DWORD __stdcall ThreadStarter(void * arg);
    
    // main loop for worker threads. will quit after a job completion and
    // assertion of _shutdownReq. _job_event semaphore is used to minimize
    // polling(minimizing loop).
    static void ThreadMain();
    
private:
    // mutex for synchronize access to PendingJobs, ActiveJobs, and CanceledJobs
    // list by worker threads
    ACE_Thread_Mutex _job_mutex;
    
    // signal to shutdown all the worker threads
    bool  _shutdownReq;
    
    // HANDLE for each worker threads
    HANDLE _request_threads_[DNS_MAX_REQUEST_THREADS];
    
    // list for recent submited DNS_Job(s)
    // when idle, a worker thread while fetch a job from this list.
    List_DNS_Jobs  _PendingJobs;
    
    // signal for any new job submitted or shutdown requested
    ACE_Semaphore _job_events;
    
    // singleton instance
    static DNS_Worker_Threads_Manager *_instance;
};

//DNS_Service* DNS_Service::dnsSvcInstance = 0;
//DNS_Worker_Threads_Manager* DNS_Worker_Threads_Manager::_instance = NULL;



/* 
* DNS API provider class.
*
*/
class DNS_API : public CAPIProvider {
    
public:

    STATUS API_CALL API_ResolveDNS (CTask *task, int* rethnd, const char *hostname, int type);
    STATUS API_CALL API_StatusDNS (CTask *task, int rethnd, int *status);
    STATUS API_CALL API_FetchResolvedADNS (CTask *task, char *ret, int rethnd);
    STATUS API_CALL API_CloseDNS (CTask *task, int rethnd);
    STATUS API_CALL API_FetchResolvedSRVDNS (CTask *task, char *ret, int rethnd, int*port, int*prio, int*weight);
    
private:
    DNS_Service	*m_Driver;
    
    DNS_API( int provider_id, DNS_Service *d);
    
    STATUS  Init();
    STATUS  Shutdown();
    
    friend class DNS_Service;
};




#endif
