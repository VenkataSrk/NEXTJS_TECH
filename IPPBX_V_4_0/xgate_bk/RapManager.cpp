/*$Header: /xGate/RapManager.cpp 55    4/23/03 5:34p Bennylp $

  $Log: /xGate/RapManager.cpp $
 * 
 * 55    4/23/03 5:34p Bennylp
 * Administrivia: tidy-up the usage of API provider ID. Now the
 * enumeration in api.h should be used instead of bare constants.
 * 
 * 54    4/03/03 3:42p Bennylp
 * Remove timeout warning
 * 
 * 53    22/05/02 16:40 Junanto
 * Prevent error and informational message from
 * being displayed during configuration file loading
 * 
 * 52    25/10/01 10:50 Iwanj
 * remove static, again ;)
 * 
 * 51    23/10/01 19:24 Iwanj
 * use static copy ctor, fix new API
 * 
 * 50    10/18/01 7:25p Bennylp
 * Go back to the non-static version
 * 
 * 49    18/10/01 18:31 Iwanj
 * add new API ResourceAlloc
 * change Rap_Message to static
 * 
 * 48    17/10/01 15:01 Junanto
 * 
 * 47    16/10/01 19:34 Junanto
 * Changed RapMessage to static
 * 
 * 46    9/04/01 12:31p Iwanj
 * change lock
 * 
 * 45    8/21/01 12:16p Iwanj
 * modif OnShowStatus
 * 
 * 44    8/20/01 5:39p Iwanj
 * change Unload
 * 
 * 43    7/28/01 4:10p Iwanj
 * modif due to slrap thread rewrite
 * 
 * 42    7/19/01 9:05p Iwanj
 * modif IRQ handling to provide direction and distribution info
 * 
 * 41    7/17/01 3:30p Iwanj
 * add enable/no enable
 * 
 * 40    7/17/01 1:51p Iwanj
 * keep resourcename in lrq job
 * 
 * 39    7/13/01 8:00p Iwanj
 * implement CommitConfiguration
 * 
 * 38    7/02/01 3:26a Iwanj
 * cancel request during cancel job
 * 
 * 37    6/27/01 10:21p Iwanj
 * 
 * 36    6/02/01 7:00p Iwanj
 * fix bug in HandleRIC
 * 
 * 35    5/30/01 7:19p Iwanj
 * delete timer_
 * 
 * 34    5/28/01 4:21p Iwanj
 * change physicalAddress
 * 
 * 33    5/25/01 7:14p Iwanj
 * add updateJobWaitingTime to avoid blocking forever when reply lost
 * 
 * 32    5/25/01 3:56p Iwanj
 * change Load, using default physicall address
 * 
 * 31    5/18/01 12:07p Iwanj
 * change RII message (resourceStatus)
 * 
 * 30    5/15/01 6:16p Iwanj
 * change HandleTimeout
 * 
 * 29    5/14/01 6:27p Iwanj
 * modif. HandleLRJ
 * 
 * 28    5/09/01 6:55p Iwanj
 * add arj,lrj message, change HandleLRJ
 * 
 * 27    5/09/01 5:44p Iwanj
 * Change RRR, check whether interface is RAPEnabled
 * 
 * 26    5/03/01 10:42p Iwanj
 * change RII, add resource status field
 * 
 * 25    5/03/01 10:48a Iwanj
 * 
 * 24    5/02/01 7:21p Iwanj
 * modif LRQ and IRQ
 * 
 * 23    5/02/01 7:11p Iwanj
 * fix blocking problem in LRQ (due to server down)
 * 
 * 22    5/02/01 6:10p Iwanj
 * change handleLCF, lcf will always return non empty physical address
 * if this value equals with our own return empty
 * 
 * 21    2/05/01 15:47 Junanto
 * CancelJob changed
 * 
 * 20    5/02/01 11:36a Iwanj
 * implement AttachTask,DetachTask
 * 
 * 19    5/01/01 10:27a Iwanj
 * 
 * 18    4/30/01 10:26p Iwanj
 * 
 * 17    4/30/01 12:03p Iwanj
 * ResourceQuery API
 * 
 * 16    4/27/01 7:25p Iwanj
 * Added RRR handling
 * 
 * 15    4/27/01 5:41p Iwanj
 * 
 * 14    4/27/01 4:29p Iwanj
 * 
 * 13    4/26/01 4:30p Iwanj
 * 
 * 12    4/26/01 3:07p Iwanj
 * 
 * 11    4/25/01 9:26p Iwanj
 * 
 * 10    4/25/01 8:13p Iwanj
 * 
 * 9     4/25/01 7:32p Iwanj
 * 
 * 8     4/25/01 3:13p Iwanj
 * 
 * 7     4/24/01 8:36p Iwanj
 * 
 * 6     4/24/01 6:31p Iwanj
 * 
 * 5     4/24/01 5:59p Iwanj
 * Derive from ServiceProvider
 * 
 * 4     4/24/01 5:41p Iwanj
 * rapclient wrapper initial version

*/

#include "stdafx.h"
#pragma warning (disable:4786)

//#include <winsock2.h>
#include <assert.h>
#include <sys/timeb.h>
#include "types.h"
#include "/usr/local/include/ptlib.h"
#include "RapManager.h"
#include "RapClient.h"
#include "RapTimer.h"
#include "RapConfigurator.h"
#include "RapMessage.h"
#include "RapConfig.h"
#include "MediaManager.h"
#include "log.h"
#include "task.h"
#include <algorithm>
#include <ptlib/array.h>

const int RapAPIProviderId = API_PROVIDER_ID_RAP;

static const char __modname__[] = "RAP";
#define THISMODULE		__modname__


static const char* msginfo[] =
{
      "GRQ",
      "GCF",
      "RRQ",
      "RCF",
      "RRJ",
      "URQ",
      "UCF",
      "ARQ",
      "ACF",
      "ARJ",
      "DRQ",
      "DCF",
      "LRQ",
      "LCF",
      "LRJ",
      "SII",
      "SIC",
      "RRR",
      "RRC",
      "IRQ",
      "IRR",
      "RII",
      "RIC",
      "RIP"
};


// helper struct

// hold pointer to app's return value
RapResult::RapResult():
    retcode_(0),
    res1_(0),
    res2_(0)
{
}

RapResult::RapResult(int* retcode,char* res1,char* res2):
    retcode_(retcode),
    res1_(res1),
    res2_(res2)
{
}

// bookkeeping for lrq and irq request
RapJob::RapJob():
    seqnum_(0),
    task_(0),
    pending_(true),
    waitingTime_(MAX_WAIT_TIME),
    resname_("")
{
}

RapJob::RapJob(unsigned seqnum,CTask* task,const RapResult& result,bool pending,const string& res):
    seqnum_(seqnum),
    task_(task),
    result_(result),
    pending_(pending),
    waitingTime_(MAX_WAIT_TIME),
    resname_(res)
{
}


// the singleton
RapManager* RapManager::instance_ = 0;

RapManager::RapManager():
    ServiceProvider(THISMODULE,true,true), //attach_task=true,poll=true
    //rapClient_(RapClient::instance()),
    rapClient_(0),
    api_(0),
    //guard_(0),
    unitId_(""),
    physAddr_(""),
    timer_(new RapTimer(1000)),
    enabled_(true),
    initialized_(false)
{
    timer_->attach(this);
}

RapManager::~RapManager()
{
    timer_->detach(this);
    delete timer_;
}

RapManager* RapManager::Create()
{
    if (instance_==0)
	instance_ = new RapManager;

    return instance_;
}

RapManager* RapManager::Instance()
{
    return instance_;
}

bool RapManager::CommitConfiguration()
{
    // get pointer to rapclient instant
    rapClient_ = RapClient::instance();
    if (!rapClient_)
    {
	CLog::Error(THISMODULE,"Cannot instantiate RapClient");
	return true;
    }

    char hostname[64];
    gethostname(hostname,sizeof(hostname)-1);
    unitId_ = hostname;

    api_ = new RapAPI;

    rapClient_->setUnitId(unitId_);
    
    // register notification which we are interested in
    // generic

    rapClient_->setUserCallback(RapClient::e_onRequestError,onError_);
    rapClient_->setUserCallback(RapClient::e_onRequestTimeout,onTimeout_);

    // specific: notifiy on incoming RIC,ARJ,LCF,LRJ and RRR

    rapClient_->setUserCallback(RapClient::e_onAdmissionReject,onArj_);
    rapClient_->setUserCallback(RapClient::e_onLocationConfirm,onLcf_);
    rapClient_->setUserCallback(RapClient::e_onLocationReject,onLrj_);
    rapClient_->setUserCallback(RapClient::e_onResourceInfoConfirm,onRic_);
    rapClient_->setUserCallback(RapClient::e_onReregistrationRequest,onRrr_);
    rapClient_->setUserCallback(RapClient::e_onInfoResponse,onIrr_);
    
    timer_->start();

    
    if (!rapClient_->initialize())
    {
	CLog::Error(THISMODULE,"Cannot initialize RapClient");
	return true;
    }


    // retrieve physicalAddress from RapClient
    physAddr_ = rapClient_->getPhysicalAddress();
    initialized_ = true;

    return true;
}

void RapManager::onSubjectNotification(RapSubject* subject)
{
    if (subject==timer_)
    {
	// decrement waiting time for each pending job
	UpdateJobWaitingTime();
    }
}

// prevent task from blocking forever due to lost reply 
// during interdomain request
// 
// client ---LRQ-->  server1
//        <--RIP---          ---LRQ-----> server2
//       (reliable)         
//                           <--LCF/LRJ--          
//                            (reliable)
// ...
// ...
//        <-LCF/LRJ--
//        (unreliable!!!)
// 
//

void RapManager::UpdateJobWaitingTime()
{
    Lock l(critical_section_);

    for(List<RapJob*>::iterator it=jobList_.begin();it!=jobList_.end();++it)
    {
	RapJob* job = *it;
	if (job->pending_ && --job->waitingTime_<=0)
	{
	    CLog::Detail(THISMODULE, "Cancelling job from task %d", job->task_->GetPID());

	    // job exceeding max. wait time, set reason=timeout and pending=false
	    // the next time HandleEvents poll occur, this job will be removed
	    // and task will be waken up
	    job->pending_ = false;
	    *(job->result_.retcode_) = RapAPI::Rc_Timeout;
	}
	    
    }
}

bool RapManager::Load()
{
    return true;
}

bool RapManager::Unload()
{
    timer_->stop();
    if (rapClient_ && initialized_)
    {
	rapClient_->shutdown();
	initialized_ = false;
    }

    Lock l(critical_section_);
    // clean up pending job (if any)
    for(List<RapJob*>::iterator it=jobList_.begin();it!=jobList_.end();++it)
    {
	// delete struct pointed by list element
	delete *it;
    }
    jobList_.clear();

    delete api_;

    return true;
}

void RapManager::HandleEvents()
{
    if (jobList_.empty())
	return;

    // for each completed job, wake up
    Lock l(critical_section_);

    List<RapJob*>::iterator it = jobList_.begin();
    while (it!=jobList_.end())
    {
	if (!(*it)->pending_)
	{
	    (*it)->task_->Wakeup();
	    // delete struct pointed by this element
	    delete *it;
	    // delete list element
	    it = jobList_.erase(it);
	}
	else
	    ++it;
    }
}

bool RapManager::AttachTask(CTask *)
{
    return true;
}

void RapManager::DetachTask(CTask *task)
{
    CancelJob(task);
}

bool RapManager::CancelJob(CTask* task)
{
    CLog::Detail(THISMODULE, "Cancelling task %d", task->GetPID());

    Lock l(critical_section_);

    List<RapJob*>::iterator it = jobList_.begin();
    while(it!=jobList_.end())
    {
	if ((*it)->task_==task)
	{
	    // cancel pending request if it is still in sending queue
	    rapClient_->cancelRequest((*it)->seqnum_);

	    // delete struct pointed by this element
	    delete *it;
	    // delete list element
	    it = jobList_.erase(it);

	    task->Wakeup();
	    break;
	}
	else
	    ++it;
    }

    return true;
}

/*
    called by Interface: SetMediaCount,Start,Enable,Stop,Disable
*/
//void RapManager::RII(const RapParam::RII& p)
void RapManager::RII(const RapParam::RII& p, int resourceType)	//rkv
{
    if (!initialized_)
    {
	CLog::Debug(THISMODULE,"Cannot send RII, RapManager out of service");
	return;
    }

    if (!enabled_)
    {
	CLog::Warning(THISMODULE,"Cannot send RII, RapManager disabled");
	return;
    }

    Rap_Message request;
   
    Rap_ResourceInfoIndication& rii = rapClient_->prepareRII(request);

    rii.m_unitId = unitId_.c_str();

    // RII is done for each interface, one at a time,
    // -> size will always be 1
    rii.m_resourceList.SetSize(1);

    //rii.m_resourceList[0].m_resourceType = Rap_ResourceType::e_line;
    rii.m_resourceList[0].m_resourceType = resourceType; //rkv
    rii.m_resourceList[0].m_portId = p.portId_.c_str();
    rii.m_resourceList[0].m_quantity = p.quantity_;
    rii.m_resourceList[0].m_resourceStatus = p.status_;

    rapClient_->makeRequest(request);

}

/*
    called by Interface: DecMediaCount
*/
void RapManager::ARQ(const RapParam::ARQ& p)
{
    if (!initialized_)
    {
	CLog::Debug(THISMODULE,"Cannot send ARQ, RapManager out of service");
	return;
    }

    if (!enabled_)
    {
	CLog::Warning(THISMODULE,"Cannot send ARQ, RapManager disabled");
	return;
    }


    Rap_Message request;
    Rap_AdmissionRequest& arq = rapClient_->prepareARQ(request);

    arq.m_unitId = unitId_.c_str();
    arq.m_portId = p.portId_.c_str();
    arq.m_sessionId = p.sessionId_.c_str();
    arq.m_weight = 0;
    arq.m_direction = p.direction_==0?Rap_CallDirection::e_incoming:Rap_CallDirection::e_outgoing;
    arq.m_callParam.m_cli = p.cli_.c_str();
    arq.m_callParam.m_ddi = p.ddi_.c_str();
    
    rapClient_->makeRequest(request);
}

/*
    called by Interface: IncMediaCount
*/
void RapManager::DRQ(const RapParam::DRQ& p)
{
    if (!initialized_)
    {
	CLog::Debug(THISMODULE,"Cannot send DRQ, RapManager out of service");
	return;
    }

    if (!enabled_)
    {
	CLog::Warning(THISMODULE,"Cannot send DRQ, RapManager disabled");
	return;
    }


    Rap_Message request;
    Rap_DisengageRequest& drq = rapClient_->prepareDRQ(request);

    drq.m_unitId = unitId_.c_str();
    drq.m_portId = p.portId_.c_str();
    drq.m_sessionId = p.sessionId_.c_str();
    drq.m_direction = p.direction_==0?Rap_CallDirection::e_incoming:Rap_CallDirection::e_outgoing;
     
    drq.m_duration.m_holding = p.holdTime_;
    drq.m_duration.m_connected = p.connTime_;
    drq.m_callParam.m_cli = p.cli_.c_str();
    drq.m_callParam.m_ddi = p.ddi_.c_str();

    // DRQ  will always release resource at the moment
    drq.m_indication = Rap_DisengageIndication::e_releaseResource;
    drq.m_reason.SetValue(p.reason_);

    rapClient_->makeRequest(request);
}

/*
    called by RapAPI: ResourceLocate
*/
void RapManager::LRQ(CTask* task,const RapResult& result,const RapParam::LRQ& p)
{
    if (!initialized_)
    {
	CLog::Debug(THISMODULE,"Cannot send LRQ, RapManager out of service");
	return;
    }

    if (!enabled_)
    {
	CLog::Warning(THISMODULE,"Cannot send LRQ, RapManager disabled");
	return;
    }

    Rap_Message request;
    Rap_LocationRequest& lrq = rapClient_->prepareLRQ(request);

    // save seqnum for future reference
    unsigned seqnum = request.m_header.m_seqnum;
    // create job
    {
	Lock l(critical_section_);
	jobList_.push_back(new RapJob(seqnum,task,result,true,p.name_));
    }
    // block this task
    task->Sleep(this);

    lrq.m_unitId = unitId_.c_str();
    lrq.m_resourceName	= p.name_.c_str();

	

    switch(p.type_)
    {
    case 0:
	lrq.m_resourceType = Rap_ResourceType::e_line;
	break;
    case 1:
	lrq.m_resourceType = Rap_ResourceType::e_voice;
	break;
    default:
	lrq.m_resourceType = Rap_ResourceType::e_voip;
	break;
    }

    lrq.m_weight = p.weight_;
    lrq.m_hints	= p.hints_.c_str();

    // return error if we cannot make request
    if (!rapClient_->makeRequest(request))
	UpdateJob(seqnum,RapAPI::Rc_Error);
}

/*
    called by RapAPI: ResourceQuery
*/
void RapManager::IRQ(CTask* task,const RapResult& result,const RapParam::IRQ& p)
{
    if (!initialized_)
    {
	CLog::Debug(THISMODULE,"Cannot send IRQ, RapManager out of service");
	return;
    }

    if (!enabled_)
    {
	CLog::Warning(THISMODULE,"Cannot send IRQ, RapManager disabled");
	return;
    }

    Rap_Message request;
    Rap_InfoRequest& irq = rapClient_->prepareIRQ(request);

    // save for future ref.
    unsigned seqnum = request.m_header.m_seqnum;
    // create job
    {
	Lock l(critical_section_);
	jobList_.push_back(new RapJob(seqnum,task,result,true,""));
    }
    // block task
    task->Sleep(this);

    irq.m_unitId = p.unit_.c_str();
    irq.m_portId = p.port_.c_str();

    // return error if we cannot make request
    if (!rapClient_->makeRequest(request))
	UpdateJob(seqnum,RapAPI::Rc_Error);
}

// -------------------------------------------------------------------
// RAP callback

void RapManager::OnRIC::operator()(const Rap_Message& m)
{
    RapManager::Instance()->HandleRIC(m);
}

void RapManager::OnARJ::operator()(const Rap_Message& m)
{
    RapManager::Instance()->HandleARJ(m);
}

void RapManager::OnLRJ::operator()(const Rap_Message& m)
{
    RapManager::Instance()->HandleLRJ(m);
}

void RapManager::OnLCF::operator()(const Rap_Message& m)
{
    RapManager::Instance()->HandleLCF(m);
}

void RapManager::OnRRR::operator()(const Rap_Message& m)
{
    RapManager::Instance()->HandleRRR(m);
}

void RapManager::OnIRR::operator()(const Rap_Message& m)
{
    RapManager::Instance()->HandleIRR(m);
}

void RapManager::OnError::operator()(const Rap_Message& m)
{
    RapManager::Instance()->HandleError(m);
}

void RapManager::OnTimeout::operator()(const Rap_Message& m)
{
    RapManager::Instance()->HandleTimeout(m);
}

// ----------------------------------------------------------------------
// real callback handler

void RapManager::HandleRIC(const Rap_Message& m)
{
    const Rap_ResourceInfoConfirm& ric = m.m_data;
    int size = ric.m_resourceInfoList.GetSize();
    if (size<=0)
    {
	CLog::Warning(THISMODULE,"RIC error: some interface might not be configured in the RLS");
	return;
    }

    for(int i=0;i<size;++i)
    {
	Rap_ResourceInfo& ri = ric.m_resourceInfoList[i];
	string p =  ri.m_portId.GetValue();
	string g =  ri.m_groupId.GetValue();

	// set groupname of requesting interface
	Interface* iface = MediaManager::GetInstance()->FindInterface(p.c_str());
	if (iface)
	    iface->SetGroupName(g.c_str());
	else
	    CLog::Warning(THISMODULE,"Interface %s not found",p.c_str());
    }
}

void RapManager::HandleARJ(const Rap_Message& m)
{
    static const char* msg[] = {
	"",
	"",
	"",
	"unit not registered",
	"invalid interface name",
	"invalid call direction",
	"resource unavailable",
	"undefined reason"
    };

    const Rap_AdmissionReject& arj = m.m_data;
    //TBD:
    CLog::Warning(THISMODULE,"ARJ: %s",msg[arj.m_rejectReason]);
}

void RapManager::HandleLCF(const Rap_Message& m)
{
    //search task list by lrq seqnum
    //set result
    //flag job pending to false -> handle event poll will wake 'em up

    Lock l(critical_section_);
    for(List<RapJob*>::const_iterator ci = jobList_.begin();ci!=jobList_.end();++ci)
    {
	// find job by seqnum
	RapJob* job = *ci;
	if (job->seqnum_==m.m_header.m_seqnum)
	{
	    const Rap_LocationConfirm& lcf = m.m_data;
	    string addr =  lcf.m_physicalAddress.GetValue();
	    string port =  lcf.m_portId.GetValue();
		
	    if (job->result_.res1_)
	    {
		//deprecated ResourceLocate

		safe_strcpy(job->result_.res1_,port.c_str(),MAXAPPSTRLEN);
		//string pa = lcf.m_physicalAddress.GetValue();
		// within this box ? return empty
		if (addr==physAddr_)
		    strcpy(job->result_.res2_,"");
		else
		    safe_strcpy(job->result_.res2_,addr.c_str(),MAXAPPSTRLEN);
	    }
	    else
	    {
		//new API ResourceAlloc

		if (addr==physAddr_)
		    addr = "127.0.0.1";

		if (port!="")
		    addr = addr+":"+port;

		safe_strcpy(job->result_.res2_,addr.c_str(),MAXAPPSTRLEN);
	    
	    }

	    *(job->result_.retcode_) = RapAPI::Rc_Success;
	    job->pending_ = false;

	    return;
	}
    }
}

void RapManager::HandleLRJ(const Rap_Message& m)
{
    static const char* msg[] = {
	"",//0
	"",//1
	"",//2
	"unit not registered",//3
	"resource unavailable",//4
	"invalid resource name",//5
	"condition (hints) unsatisfied",//6
	"invalid domain name",//7
	"interdomain error (server might be too busy)",//8
	"interdomain timeout",//9
	"undefined reason"//10
    };

    //search task list by lrq seqnum
    //set retcode
    //flag job pending to false -> handle event poll will wake 'em up

    Lock l(critical_section_);
    for(List<RapJob*>::const_iterator ci = jobList_.begin();ci!=jobList_.end();++ci)
    {
	// find job by seqnum
	RapJob* job = *ci;

	if (job->seqnum_==m.m_header.m_seqnum)
	{
	    const Rap_LocationReject& lrj = m.m_data;
	    
	    if (job->result_.res1_)
		strcpy(job->result_.res1_,"");

	    strcpy(job->result_.res2_,"");

	    job->pending_ = false;
	    *(job->result_.retcode_) = lrj.m_rejectReason;

	    switch(lrj.m_rejectReason)
	    {
	    case 3:
	    case 8:
	    case 9:
	    case 10:
		CLog::Error(THISMODULE,"ResourceLocate:%s, status:%s",job->resname_.c_str(),msg[lrj.m_rejectReason]);
		break;
	    case 4:
	    case 6:
		CLog::Debug(THISMODULE,"ResourceLocate:%s, status:%s",job->resname_.c_str(),msg[lrj.m_rejectReason]);
		break;
	    case 5:
	    case 7:
		CLog::Warning(THISMODULE,"ResourceLocate:%s, status:%s",job->resname_.c_str(),msg[lrj.m_rejectReason]);
		break;
	    default:
		break;
	    }

	    return;
	}
    }
}

void RapManager::HandleRRR(const Rap_Message& m)
{
    MediaManager* mgr = MediaManager::GetInstance();

    // get all rap-enabled iface
    List<Interface*> rapIface;
    for(Interface* pi=mgr->ItGetFirstInterface();pi!=NULL;pi=mgr->ItGetNextInterface())
    {
    	if (pi->RAPEnabled())
	    rapIface.push_back(pi);
    }

    if (rapIface.empty())
    {
	CLog::Detail(THISMODULE,"Ignoring RRR, none of interface is RLS-enabled");
	return;
    }

    unsigned ifaceCount = rapIface.size();

    Rap_Message request;
    Rap_ResourceInfoIndication& rii = rapClient_->prepareRII(request);

    rii.m_unitId = unitId_.c_str();

    // bunch RII, send all interface info at once
    rii.m_resourceList.SetSize(ifaceCount);

    unsigned idx = 0;
    List<Interface*>::const_iterator ci = rapIface.begin();
    while (ci!=rapIface.end() && idx<ifaceCount)
    {
	Interface* iface = *ci;

	//rii.m_resourceList[idx].m_resourceType = Rap_ResourceType::e_line;
	rii.m_resourceList[idx].m_resourceType = iface->GetResourceType(); //rkv
	rii.m_resourceList[idx].m_portId = iface->Name().c_str();

	// always send media count, only status will be different
	rii.m_resourceList[idx].m_quantity = iface->GetMediaCount();
	rii.m_resourceList[idx].m_resourceStatus = iface->IsActive()?Rap_ResourceStatus::e_enabled:Rap_ResourceStatus::e_disabled;

	++ci;
	++idx;
    }

    rapClient_->makeRequest(request);

    // query interface direction,distribution
    for(ci=rapIface.begin();ci!=rapIface.end();++ci)
    {
	Rap_Message query;
	Rap_InfoRequest& irq = rapClient_->prepareIRQ(query);

	irq.m_unitId = unitId_.c_str();
	irq.m_portId = (*ci)->Name().c_str();

	rapClient_->makeRequest(query);
    }

}

void RapManager::HandleIRR(const Rap_Message& m)
{
    // result format:
    // Direction:Distribution:InterfaceName:GroupName
    // e.g: I:A:DAC00:TLS
    // return only groupname to application
    const Rap_InfoResponse& irr =    m.m_data;
    string gid="",pid="";
    char dir='?',dis='?';

    ParseInfo(irr.m_groupId.GetValue(),dir,dis,pid,gid);

    {
	Lock l(critical_section_);
	for(List<RapJob*>::const_iterator ci = jobList_.begin();ci!=jobList_.end();++ci)
	{
	    // find job by seqnum
	    RapJob* job = *ci;
	    if (job->seqnum_==m.m_header.m_seqnum)
	    {
		//const Rap_InfoResponse& irr = m.m_data;
		//safe_strcpy(job->result_.res1_,irr.m_groupId.GetValue(),MAXAPPSTRLEN);
		safe_strcpy(job->result_.res1_,gid.c_str(),MAXAPPSTRLEN);

		*(job->result_.retcode_) = RapAPI::Rc_Success;
		job->pending_ = false;

		return;
	    }
	}
    }

    // update xgate info
    Interface* iface = MediaManager::GetInstance()->FindInterface(pid.c_str());
    if (iface)
    {
	iface->SetDirection(dir);
	iface->SetDistribution(dis);
    }
}

void RapManager::HandleError(const Rap_Message& m)
{
    unsigned tag = m.m_data.GetTag();
    CLog::Error(THISMODULE,"Cannot send %s, too many request pending",msginfo[tag]);

    switch(tag)
    {
    case Rap_Data::e_locationRequest:
    case Rap_Data::e_infoRequest:
	UpdateJob(m.m_header.m_seqnum,RapAPI::Rc_Error);
	break;

    default:
	break;
    }
}

void RapManager::HandleTimeout(const Rap_Message& m)
{
    unsigned tag = m.m_data.GetTag();
    CLog::Debug(THISMODULE,"Timeout while sending %s",msginfo[tag]);

    switch(tag)
    {
    case Rap_Data::e_locationRequest:
    case Rap_Data::e_infoRequest:
	UpdateJob(m.m_header.m_seqnum,RapAPI::Rc_Timeout);
	break;

    default:
	break;
    }
}

void RapManager::UpdateJob(unsigned seqnum,RapAPI::ApiRetCode retcode)
{
    Lock l(critical_section_);
    for(List<RapJob*>::const_iterator ci = jobList_.begin();ci!=jobList_.end();++ci)
    {
	// find job by seqnum
	RapJob* job = *ci;
	if (job->seqnum_==seqnum)
	{
	    *(job->result_.retcode_) = retcode;
	    job->pending_ = false;

	    return;
	}
    }
}


bool RapManager::OnShowStatus(String & output)
{
    if (!initialized_)
    {
	CLog::Debug(THISMODULE,"Cannot show status, RapManager out of service");
	return false;
    }

    string ver = "  SLRAP version " + rapClient_->get_version()+"\n";
    ver += String("  server-address: ") + RapConfigurator::Instance()->rapConfig_.defaultRmAddress+"\n";
    ver += String("  local-address:  ") + RapConfigurator::Instance()->rapConfig_.unicastAddress+"\n";
    ver += "\n";
    string s;
    rapClient_->showStat(s);
    ;

    output += ver+s;
    return true;
}

bool RapManager::Enable()
{
    if (!initialized_)
    {
	//CLog::Debug(THISMODULE,"Enable failed, RapManager out of service");
	//return false;
	return true;
    }

    if (enabled_)
	return true;

    enabled_ = true;

    return true;
}

bool RapManager::Disable()
{
    if (!initialized_)
    {
	CLog::Debug(THISMODULE,"Disable failed, RapManager out of service");
	return false;
    }

    if (!enabled_)
	return true;

    enabled_ = false;

    return true;
}

bool RapManager::IsEnabled() const
{
    return enabled_;
}


void RapManager::ParseInfo(const char* info,char& dir,char& dis,string& pid,string& gid)
{
    string src = info;
    if (std::count(src.begin(),src.end(),':')!=3)
	return;

    // I:A:DAC00:TLS
    dir = src[0];
    dis = src[2];

    int pos = src.find_last_of(':');
    if (pos==string::npos)
	return;

    pid = src.substr(4,pos-4);
    gid = src.substr(pos+1,src.length());
}

// ------------------------------------------------------------------------------------
// RAP API Provider

RapAPI::RapAPI() :
    CAPIProvider(RapAPIProviderId,THISMODULE)
{
    if (API_Provider_Manager::Instance()->RegisterProvider(this) == NOT_OK) {
	CLog::Error(THISMODULE, "Unable to register provider");
	return;
    }

    /** Register API entry points
     */

    RegisterAPI(
	"ResourceLocate",			// external name
	Id_ResourceLocate,			// ordinal
	true,					// asynchronous flag
	CVariant::VT_INT,			// return type
	(CAPIProvider::APIPROC) &RapAPI::ResourceLocate,
	CAPIDef::CDECL_TYPE,			// calling convention
	8,					// number of arguments
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("retval",BYREF,CVariant::VT_RET),
	CArgType("resname",BYVAL,CVariant::VT_STRING),
	CArgType("restype",BYVAL,CVariant::VT_INT),
	CArgType("weight",BYVAL,CVariant::VT_INT),
	CArgType("hints",BYVAL,CVariant::VT_STRING),
	CArgType("port",BYREF,CVariant::VT_STRING),
	CArgType("address",BYREF,CVariant::VT_STRING)
	);

    RegisterAPI(
	"ResourceQuery",			// external name
	Id_ResourceQuery,			// ordinal
	true,					// asynchronous flag
	CVariant::VT_INT,			// return type
	(CAPIProvider::APIPROC) &RapAPI::ResourceQuery,
	CAPIDef::CDECL_TYPE,			// calling convention
	5,					// number of arguments
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("retval",BYREF,CVariant::VT_RET),
	CArgType("unitid",BYVAL,CVariant::VT_STRING),
	CArgType("portid",BYVAL,CVariant::VT_STRING),
	CArgType("groupid",BYREF,CVariant::VT_STRING)
	);

    RegisterAPI(
	"ResourceAlloc",			// external name
	Id_ResourceAlloc,			// ordinal
	true,					// asynchronous flag
	CVariant::VT_INT,			// return type
	(CAPIProvider::APIPROC) &RapAPI::ResourceAlloc,
	CAPIDef::CDECL_TYPE,			// calling convention
	9,					// number of arguments
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("retval",BYREF,CVariant::VT_RET),
	CArgType("resname",BYVAL,CVariant::VT_STRING),
	CArgType("restype",BYVAL,CVariant::VT_INT),
	CArgType("weight",BYVAL,CVariant::VT_INT),
	CArgType("hints",BYVAL,CVariant::VT_STRING),
	CArgType("reserved1",BYVAL,CVariant::VT_INT),
	CArgType("reserved2",BYVAL,CVariant::VT_STRING),
	CArgType("addresslist",BYREF,CVariant::VT_STRING)
	);

}

RapAPI::~RapAPI()
{
}

STATUS RapAPI::ResourceLocate(
    CTask *task,
    int *retcode,
    const char* name,
    int type,
    int weight,
    const char* hints,
    char* port,
    char* address)
{

    // save ptr to retcode,port,address to task storage for future ref.
    // we need to set value to those ptr when we receive RAP reply

    RapParam::LRQ param(
	name,
	type,
	weight,
	hints);

    RapResult result(retcode,port,address);

    RapManager::Instance()->LRQ(task,result,param);

    return OK;
}

STATUS RapAPI::ResourceQuery(
    CTask *task,
    int *retcode,
    const char* unitid,
    const char* portid,
    char* groupid)
{
    RapParam::IRQ param(unitid,portid);
    RapResult result(retcode,groupid,0);

    RapManager::Instance()->IRQ(task,result,param);

    return OK;
}


STATUS RapAPI::ResourceAlloc(
    CTask *task,
    int *retcode,
    const char* res_name,
    int res_type,
    int weight,
    const char* hints,
    int rsv1,
    const char* rsv2,
    char* addr_list)
{

    // save ptr to retcode,port,address to task storage for future ref.
    // we need to set value to those ptr when we receive RAP reply

    RapParam::LRQ param(
	res_name,
	res_type,
	weight,
	hints);

    RapResult result(retcode,0,addr_list);
    RapManager::Instance()->LRQ(task,result,param);

    return OK;
}
