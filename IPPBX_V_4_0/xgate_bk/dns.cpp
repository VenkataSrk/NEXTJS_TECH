/* $Header: /xGate/dns.cpp  $
*   
* XGATE
* (C) 2013 Switchlab, Ltd
*
* Small DNS Library.
*
* $Log: /xGate/dns.cpp $

* 5     29/01/13 10:41AM Oky
  First stable version. Changelist:
  -Revert code for DNS_Job, DNS_A_Job, and DNS_SRV_Job to
   original version and then remerge back to fix some unsynchronize
   logic with service provider
  -fix bug in ResolveDNS api
  -fixed API in/out parameters
  -Change API output from 32bit integer to string
  -Added separate API for fetching SRV result and A result
  -Added handle validity check in all API(avoid SL crashing xgate)
* 4     25/01/13 10:41AM Dandi
* Change handle from hostname to rethnd in API
* 3     22/01/13 10:41AM Dandi
* Added mechanism to associate DNS_job to the tasks
* 2     16/01/13 10:41AM Oky
* Added DNS_Srv_Job
* 1     16/01/13 10:41AM Oky
* Added Worker thread manager and DNS_Job
* 0     09/01/13 10:41AM Dandi
* Initial Version(SP and API skeleton)
*/


#include "kernel.h"
#include "task.h"
#include "log.h"
#include "apperror.h"
#include "sl.h"
#include "term.h"

#include <errno.h>
#include "dns.h"
#include "types.h"

DNS_Service* DNS_Service::dnsSvcInstance = 0;

DNS_Worker_Threads_Manager* DNS_Worker_Threads_Manager::_instance = NULL;

DNS_Worker_Threads_Manager* DNS_Worker_Threads_Manager::Instance()
{
    return DNS_Worker_Threads_Manager::_instance;
}

DNS_Worker_Threads_Manager::DNS_Worker_Threads_Manager()
{
    DNS_Worker_Threads_Manager::_instance = this;
    for (int i=0; i<DNS_MAX_REQUEST_THREADS; ++i)
        _request_threads_[i] = NULL;
    
    // clear shutdown signal by default
    _shutdownReq=0;
}

DNS_Worker_Threads_Manager::~DNS_Worker_Threads_Manager()
{
    DNS_Worker_Threads_Manager::_instance=NULL;
}

bool DNS_Worker_Threads_Manager::Create()
{
	DNS_Worker_Threads_Manager::_instance = NULL;

        pthread_t             threadId;	//linux porting
        pthread_attr_t        pthread_attr;	//linux porting


    if(_instance==NULL)
        _instance= new DNS_Worker_Threads_Manager;
    if(_instance==NULL) return false;
    
    CLog::Detail(DNSDRV, "Spawn worker thread..");

    // Spawn worker threads
    for (int i=0; i<DNS_MAX_REQUEST_THREADS; ++i) {
	DWORD id;
       	//_instance->_request_threads_[i] = CREATE_THREAD(NULL, 0, &ThreadStarter, _instance, 0, &id);
	int rc = pthread_create(&threadId, &pthread_attr, ThreadStarter, "thread1");   //linux porting
       	//if (_instance->_request_threads_[i] == NULL)
	if (rc != 0)
       	    return false;
       	sleep(1);
    }
    
    return true;
}

bool DNS_Worker_Threads_Manager::JobIsEmpty()
{
    bool isEmpty;
    _job_mutex.acquire();
    isEmpty=_PendingJobs.empty();
    _job_mutex.release();
    return isEmpty;
}

void DNS_Worker_Threads_Manager::Shutdown()
{
    // signal all worker thread to quit mainloop.
    // if there is active job, finish that job first
    _instance->_shutdownReq=1;
    
    _instance->_job_mutex.acquire();
    _instance->_PendingJobs.clear();
    _instance->_job_mutex.release();
    
    // give signal to all threads, that there is events,
    // shutdown events
    _instance->_job_events.release(DNS_MAX_REQUEST_THREADS);
    
    for (int i=0; i<DNS_MAX_REQUEST_THREADS;++i) {
	if (_instance->_request_threads_[i] != NULL) {
	    //while (WaitForSingleObject(_instance->_request_threads_[i],INFINITE));
	    while (pthread_mutex_lock(_instance->_request_threads_[i]));	//linux porting
	    //CloseHandle(_instance->_request_threads_[i]);
	    pthread_mutex_destroy(_instance->_request_threads_[i]);	//linux porting
	    _instance->_request_threads_[i] = NULL;
	}
    }
}

void DNS_Worker_Threads_Manager::SubmitJob(DNS_Job* job)
{
    // if(job->GetStatus()!=DNS_JOB_CREATED)
    // assert?
    
    _job_mutex.acquire();
    
    // flag job as pending
    job->_status=DNS_JOB_PENDING;
    
    // submit to pending queue
    _PendingJobs.push_front( job );

    // dandi add task handling
	if(job->_task!=NULL)
	{
        job->_task->Sleep( DNS_Service::Instance() );
        DNS_Service::Instance()->AddPendingTask( job->_task_pid );  
	}
    
    _job_mutex.release();    
    // increment event count to 1, hope 1 thread
    // will catch this
    _job_events.release();
}

void DNS_Worker_Threads_Manager::CancelJob(DNS_Job* job)
{
    _job_mutex.acquire();
        
    // if already in progress, wait for it to finish
    if(job->GetStatus()==DNS_JOB_ACTIVE)
    {
        job->_status=DNS_JOB_CANCELED;   
    }
	// still pending huh? remove from queue and die
	else if(job->GetStatus()==DNS_JOB_PENDING)
    {
        _PendingJobs.remove(job);
        delete job;
    }
	// otherwise could be DNS_JOB_CREATED, success, or
	// aborted. die.
	else if(job->GetStatus()!=DNS_JOB_CANCELED)
    {
        delete job;
    }
        
    _job_mutex.release();
}

// dandi
DNS_Job* DNS_Worker_Threads_Manager::FindJob(DNS_Job *rethnd)
{
    int index = 0;
    
    // See if still in queue
    for (List_DNS_Jobs::iterator it = DNS_Service::Instance()->registered_jobs.begin(); 	 
    it != DNS_Service::Instance()->registered_jobs.end(); it++) 
    {
	DNS_Job *job = *it;
	if ( job == (DNS_Job *) rethnd )
	    return job;
    }
    return NULL;
}


DWORD __stdcall DNS_Worker_Threads_Manager::ThreadStarter(void * arg)
{
    DNS_Worker_Threads_Manager::ThreadMain();
    return 0;
}

void DNS_Worker_Threads_Manager::ThreadMain()
{
    DNS_Job * job;

    while(!(_instance->_shutdownReq))
    {
	job=NULL;
	
	// wait and sleep until there's event
	_instance->_job_events.acquire();
	
	// oh, there is an event, let's check
	_instance->_job_mutex.acquire();
	
	// access to job queue below should be
	// atomic
	if(!(_instance->_PendingJobs.empty()))
	{
            job = _instance->_PendingJobs.back();
	    job->_status = DNS_JOB_ACTIVE;
	    _instance->_PendingJobs.pop_back();
	}
	// end of atomic lock
	_instance->_job_mutex.release();
	
	// it is a new job?

	
	if(job != NULL)
	{
	    DNS_Job_Status status;

	    status = job->doTheJob();

        _instance->_job_mutex.acquire();
        if(job->_status==DNS_JOB_CANCELED)
        {
			delete job;
			_instance->_job_mutex.release();
        }
        else
        {
			job->_status=status;
			_instance->_job_mutex.release();
	        // purging task handling
	        DNS_Service::Instance()->LockPendingTask();
	
	        if(job->_task!=NULL)
			if (DNS_Service::Instance()->HasPendingTask_NoLock(job->_task_pid)) {
	            //job->_task->SetLastError(job->GetStatus());
	            DNS_Service::Instance()->RemovePendingTask_NoLock(job->_task_pid);
	            DNS_Service::Instance()->AddCompletedTask_NoLock(job->_task);
		    }
	        DNS_Service::Instance()->UnlockPendingTask();
	    }
    }
	}
}


/*************************************************************************
 * DNS SERVICE
 *************************************************************************/

//-----------------------------------------------------------------------------
// Constructor

DNS_Service::DNS_Service()
: ServiceProvider(DNSDRV, true, true, Lower_Priority, "DNS Client API driver")
{
    m_Worker = new DNS_Worker_Threads_Manager;    
    
    CLog::Detail(DNSDRV, "Creating and starting DNS Service..");
}

DNS_Service::~DNS_Service()
{
    dnsSvcInstance = NULL;
    delete m_Worker;
    delete m_API;
}

DNS_Service * DNS_Service::Create()
{

    if (dnsSvcInstance == NULL)
	dnsSvcInstance = new DNS_Service;
    
    return dnsSvcInstance;
}


//-----------------------------------------------------------------------------
// Initialize the DNS Services
bool DNS_Service::Load()
{
    m_TLS = Kernel::Instance()->AllocTLS();
    
    CLog::Detail(DNSDRV, "<DANDI> Loading DNS Service..");    
    
    // create a new local configuration tree
    _cfgTree = new Cfg_Tree_Group("dns-service", "DNS Service", Kernel::Instance()->RootConfig());
    Kernel::Instance()->RootConfig()->RegisterShowHandler(this);
    
    /* Add command 'dns-set [hostname] [timeout]'
    **/
    Term_Cmd *cmd_setdns = new Term_Cmd (
	this,
	DNS_Term_Cmd_Set_DNS,
	"dns-set",
	"Set a DNS Hostname and its timeout",
	PrivilegeLevelAdmin, TermModeConfig, false);
    
    cmd_setdns->Add (new Term_Cmd_Arg ( "hostname", CVariant(""), CVariantValidator("", "Hostname of DNS Server")));
    cmd_setdns->Add (new Term_Cmd_Arg ( "timeout", CVariant(0), CVariantValidator(0, "Request Timeout")));
    
    RegisterTermCmd (cmd_setdns, Kernel::Instance()->RootConfig());
    
    /* Add command 'dns-remove [hostname]' 
    **/
    Term_Cmd *cmd_removedns = new Term_Cmd (
	this,
	DNS_Term_Cmd_Remove_DNS,	
	"dns-remove",
	"Remove a DNS Hostname on system configuration.",
	PrivilegeLevelAdmin, TermModeConfig, false);
    
    cmd_removedns->Add (new Term_Cmd_Arg ( "hostname", CVariant(""), CVariantValidator("", "Hostname of DNS Server")));    
    
    RegisterTermCmd (cmd_removedns, Kernel::Instance()->RootConfig());


    /* Add command 'dns-view-hosts' 
    **/
    Term_Cmd *cmd_viewdns = new Term_Cmd (
	this,
	DNS_Term_Cmd_View_DNS,	
	"dns-view-hosts",
	"List of registered hostname rule on system.",
	PrivilegeLevelAdmin, TermModeConfig, false);
    
    RegisterTermCmd (cmd_viewdns, Kernel::Instance()->RootConfig());
        

    /* create APIs once DNS Service initialized */
    m_API = new DNS_API(API_PROVIDER_ID_DNS, this);

    if (!m_API->Init()) {
	CLog::Error(DNSDRV, "<DANDI> Unable to initialize DNS API Service");
	return false;
    }

    CLog::Detail(DNSDRV, "DNS API initialization OK..");
    
    if (!m_Worker->Create()) {
	CLog::Error(DNSDRV, "<DANDI> Unable to initialize DNS Client Service");
	return false;
    }

    CLog::Detail(DNSDRV, "DNS Worker initialization OK..");
    
    CLog::Detail(DNSDRV, "Initialization DNS Client Service: succeed!");
    
    return true;
}

//-----------------------------------------------------------------------------
// Shutdown the DNS Services
bool DNS_Service::Unload()
{
    Kernel::Instance()->FreeTLS( m_TLS );
        
    CLog::Detail(DNSDRV, "Shutting down API");
    m_API->Shutdown();
    delete m_API;
    m_API = NULL;
    
    
    CLog::Detail(DNSDRV, "Shutting down DNS provider");
    m_Worker->Shutdown();
    
    CLog::Detail(DNSDRV, "Shutdown complete");
    return true;
}


void DNS_Service::AddPendingTask(unsigned pid)
{
    ACE_Guard <ACE_Thread_Mutex> guard(m_TaskMutex_);
    m_PendingTasks_.insert(pid);
}

void DNS_Service::AddCompletedTask_NoLock(CTask *task)
{
    m_CompletedTasks_.insert(task);
}

void DNS_Service::RemovePendingTask_NoLock(unsigned pid)
{
    m_PendingTasks_.erase(pid);
}

void DNS_Service::RemovePendingTask(unsigned pid)
{
    ACE_Guard <ACE_Thread_Mutex> guard(m_TaskMutex_);
    RemovePendingTask_NoLock(pid);
}

bool DNS_Service::HasPendingTask_NoLock(unsigned pid)
{
    return m_PendingTasks_.find(pid) != m_PendingTasks_.end();
}

bool DNS_Service::HasPendingTask(unsigned pid)
{
    ACE_Guard <ACE_Thread_Mutex> guard(m_TaskMutex_);
    return HasPendingTask_NoLock(pid);
}

void DNS_Service::LockPendingTask()
{
    m_TaskMutex_.acquire();
}

void DNS_Service::UnlockPendingTask()
{
    m_TaskMutex_.release();
}

//-----------------------------------------------------------------------------
/** HandleEvents: Descendent shall implement this abstract method.
It is called by Service Provider Manager
*/
void DNS_Service::HandleEvents()
{
    ACE_Guard <ACE_Thread_Mutex> guard(m_TaskMutex_);
    std::set<CTask*>::iterator it, end = m_CompletedTasks_.end();
    
    for (it = m_CompletedTasks_.begin(); it!=end; ++it) {
	    CTask *task = *it;
	    task->Wakeup();
    }
    
    m_CompletedTasks_.clear();
}


//-----------------------------------------------------------------------------
// Called  by kernel when new task created
//
bool DNS_Service::AttachTask(CTask *pTask)
{
    CTaskState *ts;
    NEW_RETURN_ (ts, CTaskState);
    if (ts == NULL)
	return false;

#ifdef __x86_64__ 
    pTask->SetTLSValue( m_TLS, ts);
#else
    pTask->SetTLSValue( m_TLS, (int)ts);
#endif
    return true;
}


//-----------------------------------------------------------------------------
// Called  by kernel when task is about to be destroyed
// When detached, close all opened file handles
//
void DNS_Service::DetachTask( CTask *task)
{
    CTaskState *ts = (CTaskState*)task->GetTLSValue( m_TLS );
	List_DNS_Jobs::iterator it;
    if (!ts)
	return;
    
    for (int i=0; i<DNS_MAX_REQUEST_THREADS; i++) {
	if (ts->m_Handle[i]) {	
	    // null-ize hostname
	    ts->m_Handle[i] = NULL;
	}
    }
    
    delete ts;
    
    LockPendingTask();
    RemovePendingTask_NoLock( task->GetPID() );
    m_CompletedTasks_.erase(task);
    UnlockPendingTask();

	for (it = registered_jobs.begin(); it!=registered_jobs.end(); ) 
	{
        DNS_Job *pJob=*it;

		if(pJob->_task==task)
		{
            CLog::Detail(DNSDRV, "Found 1 unclosed DNS_Job with task PID: %d", pJob->_task->GetPID());
			registered_jobs.erase(it++);
            DNS_Worker_Threads_Manager::Instance()->CancelJob(pJob);
		}else
            ++it;
	}
}

//-----------------------------------------------------------------------------
bool DNS_Service::CancelJob(CTask *task)
{
    unsigned pid = task->GetPID();
    
    ACE_Guard <ACE_Thread_Mutex> guard(ACE_Thread_Mutex);
    
    // If the job for the task has already completed, then do nothing, the task
    // will be woken up immediately by HandleEvents.
    // Otherwise cancel the job.
    //
    std::set<CTask*>::iterator it = m_CompletedTasks_.find(task);
    if (it == m_CompletedTasks_.end()) {
	if (HasPendingTask_NoLock(pid)) {
	    task->SetLastError(DNS_JOB_CANCELED);
		task->Wakeup();
	    RemovePendingTask_NoLock(pid);
	}
    }
    
    return true;
}
//-----------------------------------------------------------------------------
// Show status
//
bool DNS_Service::OnShowStatus(String & output)
{
    char s[1024];
    
    ACE_Guard <ACE_Thread_Mutex> guard(ACE_Thread_Mutex);
    
    sprintf(s, SHOW_STATUS_INDENT "Number of worker threads: %d\n", DNS_MAX_REQUEST_THREADS);
    output += s;
    sprintf(s, SHOW_STATUS_INDENT "Number of pending tasks: %d task(s)\n", m_PendingTasks_.size());
    output += s;
    return true;
}



/** Show configuration for the specified entry.
This function will be called by the framework whenever it needs to display the
specified entry. Previously this instance should tell the entry that it wishes to
handle displaying the entry by calling entry->RegisterShowHandler().
*/
//STATUS DNS_Service::OnShowConfig(
bool DNS_Service::OnShowConfig(
				 const Cfg_Tree_Entry* entry,
				 String & output, 
				 const String & indent, 
				 int indent_size) const
{
    
    String new_indent = indent;
    new_indent.append((size_t)indent_size, ' ');
    
    DNS_Service* This = (DNS_Service*)this;
    
    DNS_Worker_Threads_Manager* svc = m_Worker;
    output += indent + "http-server " + this->Name() + "\n";
   // svc->OnShowConfig (entry, output, new_indent, indent_size);
    output += new_indent + "top\n\n";
    
    return OK;
}
//-----------------------------------------------------------------------------
// Called whenever there are actions to our subscribed entry.
//
void DNS_Service::DoTermCmd(Term_Session *session, bool no, Term_Cmd *cmd)
{
    CVariant v1;
    
    String hostname;
    int timeout = 10;
    
    switch (cmd->GetId()) {
    case DNS_Term_Cmd_Set_DNS:
	
	cmd->GetArg((unsigned)0)->GetValue(v1);
	if( v1.IsEmpty() )
	    break;
	else hostname = v1.AsString();
	
	cmd->GetArg((unsigned)1)->GetValue(v1);
	if( v1.IsEmpty() )
	    break;
	else timeout = v1.AsInt();	

	this->InsertHostnameRule(session, hostname, timeout);
	break;

    case DNS_Term_Cmd_Remove_DNS:
	
	cmd->GetArg((unsigned)0)->GetValue(v1);
	if( v1.IsEmpty() )
	    break;
	else hostname = v1.AsString();

	this->DeleteHostnameRule(session, hostname);	
	break;

    case DNS_Term_Cmd_View_DNS:
	this->ViewHostnameRule(session, 0);
	break;

    default:
	CLog::Error( DNSDRV, "No DNS Request command found.");
	break;
    }
    
    return;
}

bool DNS_Service::InsertHostnameRule(Term_Session *session, String hostname, int timeout)
{    
    Hostdata data;
    
    /* initialize */
    data.hostname = hostname;
    data.timeout = timeout;
    
    CLog::Detail(DNSDRV, "Adding hostname: %s, timeout: %d", hostname.c_str(), timeout);
    dnshostname_list.push_back(data);
    session->Sendf("Adding hostname: %s, timeout: %d is done.\n", hostname.c_str(), timeout);
    
    return true;
}

bool DNS_Service::UpdateHostnameRule(Term_Session *session, String hostname, int timeout)
{
    List_DNS_Data::iterator it;
    
    for(it = dnshostname_list.begin(); it != dnshostname_list.end(); it++)
    {
	Hostdata ptr = *it;
	if(ptr.hostname == hostname)
	{
	    CLog::Detail(DNSDRV, "Update hostname: %s", ptr.hostname.c_str());
	    ptr.hostname = hostname;
	    ptr.timeout = timeout;
	    session->Sendf("Update hostname: %s, timeout: %d done.\n", hostname.c_str(), timeout);
	    return true;
	}
    }
    
    return false;
}

bool DNS_Service::DeleteHostnameRule(Term_Session *session, String hostname)
{
    
    List_DNS_Data::iterator it;

    CLog::Detail(DNSDRV, "Searching hostname: %s\n", hostname.c_str());
    
    for(it = dnshostname_list.begin(); it != dnshostname_list.end(); it++)
    {
	Hostdata ptr = *it;
	CLog::Detail(DNSDRV, "Found hostname: %s\n", ptr.hostname.c_str());

	if(ptr.hostname == hostname)
	{   
	    
	    CLog::Detail(DNSDRV, "---> Deleting hostname: %s\n", ptr.hostname.c_str());

	    it = dnshostname_list.erase(it);
	    session->Sendf("Delete hostname: %s done.\n", hostname.c_str());
	    break;
	}	
    }
    return true;
}


bool DNS_Service::ViewHostnameRule(Term_Session *session, int maxhost)
{
#if 0
    /*
    * dandi
    * disabled, I think we don't need to shown hostname rules 
    */
    List_DNS_Data::iterator it;

    CLog::Detail(DNSDRV, "List of hostname timeout:\n");
    
    for(it = dnshostname_list.begin(); it != dnshostname_list.end(); it++)
    {
	Hostdata ptr = *it;
	session->Sendf("host %s\t-\ttimeout %03d secs.\n", ptr.hostname.c_str(), ptr.timeout);


    }
    return true;
#endif

    List_DNS_Jobs::iterator it = DNS_Service::Instance()->registered_jobs.begin();

    if(it == DNS_Service::Instance()->registered_jobs.end())
    {
	session->Sendf("None of registered hostname found.\n");
	return true;
    }

    session->Sendf("List of registered hostname.\n");

    // see on registered job list.
    for (it; it != DNS_Service::Instance()->registered_jobs.end(); it++) 
    {
	DNS_Job *job = *it;
	if(job->GetType() == DNS_TYPE_A)
	{
	    DNS_A_Job *a = (DNS_A_Job*) job;
	    if(a->GetStatus() == DNS_JOB_SUCCESS)
		session->Sendf("Host %s, Status OK, Resolved IP: %d\n", a->GetHostname(), 
		    a->Get_IP_Address());
	    else
		session->Sendf("Host %s, Status On Queue, IP unresolved\n", a->GetHostname());
	} 
	else if(job->GetType() == DNS_TYPE_SRV)
	{
	    DNS_SRV_Job *a = (DNS_SRV_Job*) job;
	    if(a->GetStatus() == DNS_JOB_SUCCESS)
		session->Sendf("Host %s, Status OK, resolved.\n", a->GetHostname());
	    else
		session->Sendf("Host %s, Status On Queue, unresolved\n", a->GetHostname());
	}
    }
    return true;
}

STATUS DNS_Service::API_ResolveDNS(CTask *task, int* rethnd, const char *hostname, int type)
{
    DNS_Job *new_job;
    
    // validate TLS
    CTaskState *ts = (CTaskState*)task->GetTLSValue( m_TLS );
    if (!ts) {
	task->SetLastError( -1 );
	CLog::Error( DNSDRV, "Task %u does not have TLS", task->GetPID());
	return OK;
    }

    /* disabled temporary
    // allocate handle in the TLSfor the DNS Request
    int slot = ts->FindSlot();
    if (slot < 0) {
	CLog::Error( DNSDRV, task, "ResolveDNS: Too many DNS Request, limit reached!");
	task->SetLastError(-1);
	return OK;
    }
    
    CLog::Detail(DNSDRV, task, "...handle %d allocated", slot);
    */
    
    switch (type) 
    {
    case DNS_A_JOB: 
	new_job = new DNS_A_Job(hostname, task);
	registered_jobs.push_back(new_job);
	DNS_Worker_Threads_Manager::Instance()->SubmitJob(new_job);
	*rethnd = (int) new_job;
	break;
    case DNS_SRV_JOB:
	new_job = new DNS_SRV_Job(hostname, task);
	registered_jobs.push_back(new_job);
	DNS_Worker_Threads_Manager::Instance()->SubmitJob(new_job);
	*rethnd = (int) new_job;
	break;
    default:
	CLog::Error(DNSDRV, "Can't find type of DNS Request.");
	break;
    }
    
    CLog::Detail( DNSDRV, "<DANDI> INVOKING DNS_Service::API_ResolveDNS() ");
    
    return OK;
}

STATUS DNS_Service::API_StatusDNS(CTask *task, int rethnd, int *status)
{
    CLog::Detail( DNSDRV, "<DANDI> INVOKING DNS_Service::API_StatusDNS() ");

	/* find certain jobs in registered job list */
    DNS_Job *job  = DNS_Worker_Threads_Manager::FindJob((DNS_Job*)rethnd);
    
    if(job!=NULL)
	    *status = (int) job->GetStatus();        
    else
	{
		CLog::Detail( DNSDRV, "<API_StatusDNS>error, invalid supplied handle!");
		return NOT_OK;
	}
    
    
    return OK;
}

STATUS DNS_Service::API_FetchResolvedADNS(CTask *task, char *IPaddr, int rethnd)
{   
    DNS_A_Job *fetch_a_job ;
    
    IP4_ADDRESS result;

    
    /* find certain jobs in registered job list */
    DNS_Job *job  = DNS_Worker_Threads_Manager::FindJob((DNS_Job*)rethnd);
	if(job==NULL)
	{
		CLog::Detail( DNSDRV, "<API_FetchResolvedADNS>error, invalid supplied handle!");
		return NOT_OK;
	}else if(job->GetType()!=DNS_TYPE_A)
	{
		CLog::Detail( DNSDRV, "<API_FetchResolvedADNS>error, the supplied handle is not DNS A type!");
		return NOT_OK;
	}

	if(job->GetStatus()==DNS_JOB_SUCCESS)
	{
		fetch_a_job=(DNS_A_Job*)job;
	    result=fetch_a_job->Get_IP_Address();

	    sprintf(IPaddr, "%d.%d.%d.%d", 
		        result&0xFF,
			    (result>>8)&0xFF,
			    (result>>16)&0xFF,
			    (result>>24)&0xFF);
	}
    else
		IPaddr[0]=0;
    
    return OK;
}


STATUS DNS_Service::API_FetchResolvedSRVDNS (CTask *task, char *ret, int rethnd, int*port, int*prio, int*weight)
{
	DNS_SRV_Job *fetch_srv_job ;

    /* find certain jobs in registered job list */
    DNS_Job *job  = DNS_Worker_Threads_Manager::FindJob((DNS_Job*)rethnd);

	if(job==NULL)
	{
		CLog::Detail( DNSDRV, "<API_FetchResolvedSRVDNS>error, invalid supplied handle!");
		return NOT_OK;
	}else if(job->GetType()!=DNS_TYPE_SRV)
	{
		CLog::Detail( DNSDRV, "<API_FetchResolvedSRVDNS>error, the supplied handle is not DNS SRV type!");
		return NOT_OK;
	}

	if(job->GetStatus()==DNS_JOB_SUCCESS)
	{
		DNS_SRV_RESULT result;
		fetch_srv_job=(DNS_SRV_Job*)job;
        *ret=0;

		if(!fetch_srv_job->Results.empty())
		{
		    result=fetch_srv_job->Results.front();
		    fetch_srv_job->Results.pop_front();
		    sprintf(ret, "%d.%d.%d.%d", 
		            result.IPAddress&0xFF,
			        (result.IPAddress>>8)&0xFF,
			        (result.IPAddress>>16)&0xFF,
			        (result.IPAddress>>24)&0xFF);
		    *port=result.Port;
		    *prio=result.Priority;
		    *weight=result.Weight;
		}
	}
    else
		*ret=0;
    
    return OK;

}



STATUS DNS_Service::API_CloseDNS(CTask *task, int rethnd)
{
    
    /* find certain jobs in registered job list */
    DNS_Job *job  = DNS_Worker_Threads_Manager::FindJob((DNS_Job*)rethnd);
    
    if(job!=NULL)
	{
	    /* remove job on DNS service registered jobs */
        registered_jobs.remove(job);
        delete job;
	}
    
    return OK;
}

//-----------------------------------------------------------------------------
// Implementation of DNS_Service::CTaskState
//
DNS_Service::CTaskState::CTaskState() 
{ 
    memset(m_Handle, 0, DNS_MAX_REQUEST_THREADS* (sizeof(char) *DNS_MAX_HOSTNAME_LENGTH)); 
}


int DNS_Service::CTaskState::FindSlot()
{
    
    int i;
    for (i=0; i<DNS_MAX_SLOT_REQUEST; ++i) 
    {
	if (!m_Handle[i])
	    return i;
    }
    
    return -1;
}

void DNS_Service::CTaskState::ReleaseRequest(char *hostname, int *request_id)
{
    int i;
    for (i=0; i<DNS_MAX_REQUEST_THREADS; ++i) {
	if (m_Handle[i] == request_id) {
	    m_Handle[i] = NULL;
	}
    }
}


//-----------------------------------------------------------------------------
// Implementation of DNS_API 
//

#undef DNSDRV

//-----------------------------------------------------------------------------
// FILE DRIVER API
//-----------------------------------------------------------------------------

#define DNSAPI	"dnsapi"

DNS_API::DNS_API( int provider_id, DNS_Service *d)
: CAPIProvider(API_PROVIDER_ID_DNS, DNSAPI)
{
    m_Driver = d;
}

STATUS DNS_API::Init()
{
    CLog::Detail( DNSAPI, "Registering API");
    
    if (API_Provider_Manager::Instance()->RegisterProvider(this) == NOT_OK) {
	CLog::Error( DNSAPI, "Unable to register provider");
	return NOT_OK;
    }
    
    enum API_Id {
	    ResolveDNS = 1,
	    StatusDNS,
	    FetchResolvedADNS,
	    CloseDNS,
		FetchResolvedSRVDNS
    };
    
    if (RegisterAPI( "ResolveDNS", ResolveDNS, true,
	CVariant::VT_INT,
	(CAPIProvider::APIPROC) &DNS_API::API_ResolveDNS,
	CAPIDef::CDECL_TYPE, 4,
	CArgType( "task", BYVAL, CVariant::VT_TASK),
	CArgType( "rethnd", BYREF, CVariant::VT_RET),
	CArgType( "hostname", BYVAL, CVariant::VT_STRING),
	CArgType( "type", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }
    
    if (RegisterAPI( "StatusDNS", StatusDNS, false,
	CVariant::VT_INT,
	(CAPIProvider::APIPROC) &DNS_API::API_StatusDNS,
	CAPIDef::CDECL_TYPE, 3,
	CArgType( "task", BYVAL, CVariant::VT_TASK),		     		     
	CArgType( "rethnd", BYVAL, CVariant::VT_INT),
	CArgType( "status", BYREF, CVariant::VT_RET)) == NOT_OK) {
	return NOT_OK;
    }
    
    if (RegisterAPI( "FetchResolvedADNS", FetchResolvedADNS, false,
	CVariant::VT_STRING,
	(CAPIProvider::APIPROC) &DNS_API::API_FetchResolvedADNS,
	CAPIDef::CDECL_TYPE, 3,
	CArgType( "task", BYVAL, CVariant::VT_TASK),	     
	CArgType( "IPaddr", BYREF, CVariant::VT_RET),
	CArgType( "rethnd", BYVAL, CVariant::VT_INT)
	) == NOT_OK) {
	return NOT_OK;
    }
    
    if (RegisterAPI( "CloseDNS", CloseDNS, false,
	CVariant::VT_VOID,
	(CAPIProvider::APIPROC) &DNS_API::API_CloseDNS,
	CAPIDef::CDECL_TYPE, 2,
	CArgType( "task", BYVAL, CVariant::VT_TASK),		     
	CArgType( "rethnd", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

	if (RegisterAPI( "FetchResolvedSRVDNS", FetchResolvedSRVDNS, false,
	CVariant::VT_STRING,
	(CAPIProvider::APIPROC) &DNS_API::API_FetchResolvedSRVDNS,
	CAPIDef::CDECL_TYPE, 6,
	CArgType( "task", BYVAL, CVariant::VT_TASK),	     
	CArgType( "IPaddr", BYREF, CVariant::VT_RET),
	CArgType( "rethnd", BYVAL, CVariant::VT_INT),
	CArgType( "port", BYREF, CVariant::VT_INT),
	CArgType( "prio", BYREF, CVariant::VT_INT),
	CArgType( "weight", BYREF, CVariant::VT_INT)
	) == NOT_OK) {
	return NOT_OK;
    }
    
    CLog::Debug( DNSAPI, "DNS APIs Initialized.");
    
    return OK;
}

//-----------------------------------------------------------------------------
STATUS DNS_API::Shutdown()
{
    API_Provider_Manager::Instance()->UnregisterProvider(this);
    return OK;
}

STATUS DNS_API::API_ResolveDNS(CTask *task, int* rethnd, const char *hostname, int type)
{
    return m_Driver->API_ResolveDNS(task, rethnd, hostname, type);
}

STATUS DNS_API::API_StatusDNS(CTask *task, int hnd, int *status)
{
    return m_Driver->API_StatusDNS(task, hnd, status);
}

STATUS DNS_API::API_FetchResolvedADNS(CTask *task, char *IPaddr, int rethnd)
{
    return m_Driver->API_FetchResolvedADNS(task, IPaddr, rethnd);
}

STATUS DNS_API::API_CloseDNS(CTask *task, int rethnd)
{
    return m_Driver->API_CloseDNS(task, rethnd);
}

STATUS DNS_API::API_FetchResolvedSRVDNS (CTask *task, 
										 char *ret, 
										 int rethnd, 
										 int*port, 
										 int*prio, 
										 int*weight)
{
	return m_Driver->API_FetchResolvedSRVDNS(task, ret, rethnd, port, prio, weight);
}

#undef DNSAPI
