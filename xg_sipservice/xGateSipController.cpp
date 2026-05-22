#include "xglog.h" //DebugVM
#include "xGateSipController.h"
#include "xGateSipSignalProcessor.h"
#define THISMODULE "SofSip"
#include <fstream>

#include "xGateSipService.h" //DebugVM 


std::list<xGateCallObj*> callObjList;
ACE_Recursive_Thread_Mutex rmutex_;

void WriteToReceiver(xGateCallObj *obj){
  {
    ACE_Guard<ACE_Recursive_Thread_Mutex>gaurd(rmutex_);
    callObjList.push_back(obj);
  }
}

xGateCallObj* ReadFromReceiver(){
  xGateCallObj *temp = nullptr;
  {
    ACE_Guard<ACE_Recursive_Thread_Mutex>gaurd(rmutex_);
    if(!callObjList.empty()){
      temp = callObjList.front();
      callObjList.pop_front();
    }

  }
  return temp;
}

std::list<xGateSofiaSipRegobj*> xGateRegQ;
ACE_Recursive_Thread_Mutex regmutex_;

void PostToReceiver(xGateSofiaSipRegobj *obj){
  {
    ACE_Guard<ACE_Recursive_Thread_Mutex>gaurd(regmutex_);
    xGateRegQ.push_back(obj);
  }
}

xGateSofiaSipRegobj *PopFromReceiver(){
  xGateSofiaSipRegobj *temp = nullptr;
  {
    ACE_Guard<ACE_Recursive_Thread_Mutex>gaurd(regmutex_);
    if(!xGateRegQ.empty()){
      temp = xGateRegQ.front();
      xGateRegQ.pop_front();
    }

  }
  return temp;
}

//DebugVM end
xGateSipController::xGateSipController(){
  XGLOG_INFO("xateSipController", "CONSTRUCTOR-%s","CALLED " );
  //DebugVM ReadProxyConfigFile();
}
callInfoParameter::callInfoParameter(const callInfoParameter &rhs)
{
   routeAddr           = rhs.routeAddr.c_str();
   callingDeviceType   = rhs.callingDeviceType.c_str();
   calledDeviceType    = rhs.calledDeviceType.c_str();
   csaCalled           = rhs.csaCalled.c_str();
   cnCalled            = rhs.cnCalled.c_str();
   csaCalling          = rhs.csaCalling.c_str();
   cnCalling           = rhs.cnCalling.c_str();
   call_id             = rhs.call_id.c_str();
   uid                 = rhs.uid.c_str();
   transportType       = rhs.transportType.c_str();
   callStream          = rhs.callStream.c_str(); 
   csTransport         = rhs.csTransport.c_str();
   sdpStr              = rhs.sdpStr.c_str();
  
  m_AddrRecords.m_strProxyusername = rhs.m_AddrRecords.m_strProxyusername.c_str();
  m_AddrRecords.m_strIpaddress = rhs.m_AddrRecords.m_strIpaddress.c_str(); 
  m_AddrRecords.m_strDeviceType = rhs.m_AddrRecords.m_strDeviceType.c_str();
  m_AddrRecords.m_strContactaddress = rhs.m_AddrRecords.m_strContactaddress.c_str();

  is302messagearrived  = rhs.is302messagearrived;  
  forwardingmgcrouteIp = rhs.forwardingmgcrouteIp.c_str();
  isServerFailure      = rhs.isServerFailure;
  isInboundPstnCall    = rhs.isInboundPstnCall;
  isClientDialout      = rhs.isClientDialout;
  isMsTeamsCall        = rhs.isMsTeamsCall;
  
}
callInfoParameter &callInfoParameter::operator=(const callInfoParameter &rhs)
{
 
   routeAddr           = rhs.routeAddr.c_str();
   callingDeviceType   = rhs.callingDeviceType.c_str();
   calledDeviceType    = rhs.calledDeviceType.c_str();
   csaCalled           = rhs.csaCalled.c_str();
   cnCalled            = rhs.cnCalled.c_str();
   csaCalling          = rhs.csaCalling.c_str();
   cnCalling           = rhs.cnCalling.c_str();
   call_id             = rhs.call_id.c_str();
   uid                 = rhs.uid.c_str();
   transportType       = rhs.transportType.c_str();
   callStream          = rhs.callStream.c_str(); 
   csTransport         = rhs.csTransport.c_str();
   sdpStr              = rhs.sdpStr.c_str();

  m_AddrRecords.m_strProxyusername = rhs.m_AddrRecords.m_strProxyusername.c_str();
  m_AddrRecords.m_strIpaddress = rhs.m_AddrRecords.m_strIpaddress.c_str(); 
  m_AddrRecords.m_strDeviceType = rhs.m_AddrRecords.m_strDeviceType.c_str();
  m_AddrRecords.m_strContactaddress = rhs.m_AddrRecords.m_strContactaddress.c_str();

  is302messagearrived  = rhs.is302messagearrived;  
  forwardingmgcrouteIp = rhs.forwardingmgcrouteIp.c_str();
  isServerFailure      = rhs.isServerFailure;
  isInboundPstnCall    = rhs.isInboundPstnCall;
  isClientDialout      = rhs.isClientDialout;
  isMsTeamsCall        = rhs.isMsTeamsCall;

  return *this;
}


bool xGateSipController::CheckDomainForCallAuth(std::string sDomainName)
{
	if(sDomainName.find("delightapp.vectone.com")!= string::npos)
		return true;
	else if (sDomainName.find("vectoneapp.vectone.com")!= string::npos)
		return true;
	else if (sDomainName.find("mundio.com")!= string::npos)
		return true;
	else if (sDomainName.find("urcall.unifiedring.co.uk")!= string::npos)
		return true;
	else if(sDomainName.find("urvideo.unifiedring.co.uk")!= string::npos)
		return true;
  else if(sDomainName.find("worktual.co.uk")!= string::npos)
    return true;

	return false;
}

std::string xGateSipController::CreateUniqueCallid(char const *user,char const *host,char const *tag, char const *callid){
  std::string UniqueCallid = "";
  if( user && host && tag && callid){
    UniqueCallid=user;
    /*    UniqueCallid=user;
          UniqueCallid+="@";
          UniqueCallid+=host;
          UniqueCallid+=";";
          UniqueCallid+=tag;
          UniqueCallid+="-";*/
    UniqueCallid+=callid;
    XGLOG_INFO(THISMODULE,"xGateSipController::CreateUniqueCallid(%s)",UniqueCallid.c_str());
  }
  return UniqueCallid;
}

/*
 * Create the Unique Invite CallId
 */
std::string xGateSipController::CreateUniqueCallid(char const *callid, char const *tag)
{
  std::string UniqueInvCallid("");
  if (callid && tag)
  {
    UniqueInvCallid += callid;
    UniqueInvCallid += "-";
    UniqueInvCallid += tag;
    XGLOG_INFO(THISMODULE, "xGateSipController::CreateUniqueCallid(%s)", UniqueInvCallid.c_str());
  }
  return UniqueInvCallid;
}

xGateCallPair* xGateSipController::GetConnectedList(std::string key){
     XGLOG_FUNCTION_EN_EX("");

  CALL_CONNECTED_LIST::ITERATOR it(m_CallConnectedList);
  xGateCallPair *callpair = nullptr;
  if(m_CallConnectedList.find(key.c_str(),callpair) != -1){
    XGLOG_INFO("xGateSipController::GetConnectLsit(Unique ID = %s Object FOUND )",key.c_str());
    if (!callpair)
         XGLOG_INFO("xGateSipController::GetConnectLsit BUT callpair is nullptr");
    else 
        XGLOG_INFO("xGateSipController::GetConnectLsit BUT callpair is NOT nullptr");
            
  }
  return callpair;
}
bool xGateSipController::InsertConnectedList(std::string key,xGateSofiaobj* sobj,XGATEOBJECT type){
  XGLOG_FUNCTION_EN_EX("");
  if(!sobj)
    return false;

  xGateCallPair *callpair = nullptr;
  CALL_CONNECTED_LIST::ENTRY *callEntry = 0;
  switch(type)
  {
    case EN_OT_ORIGINATION:
      callpair =  new xGateCallPair();
      if(callpair) callpair->Orgcall =  sobj;
      break;
    case EN_OT_DESTINATION:
      callpair =  GetConnectedList(key);
      if(callpair) callpair->Dstcall = sobj;
      callEntry =  (CALL_CONNECTED_LIST::ENTRY*)sobj->callEntry;
      break;
  }
  if(!callpair){
    XGLOG_INFO("xGateSipController::InsertConnectedCall(%s- Object Type %d ) (callpair nullptr) failed",key.c_str(),type);
    return false;
  }
  int status = m_CallConnectedList.bind(key.c_str(),callpair,callEntry);
  if(status == 0 && callEntry){
    XGLOG_INFO("xGateSipController::InsertConnectedCall(%s- Object Type %d ) Success",key.c_str(),type);
    sobj->callEntry = (void*)callEntry;
    sobj->SofiaLegB->callEntry = (void*)callEntry;

    CALL_CONNECTED_LIST::ITERATOR it(m_CallConnectedList);
    xGateCallPair *callpair = nullptr;
    if(m_CallConnectedList.find(key.c_str(),callpair) != -1){
      XGLOG_INFO("xGateSipController::InsertConnectedCall(Unique ID = %s Object FOUND )",key.c_str());
    }
    else {
      XGLOG_INFO("xGateSipController::InsertConnectedCall(Unique ID = %s Object NOT FOUND )",key.c_str());

    }

  }
  else if(status == 1 && callEntry){
    XGLOG_INFO("xGateSipController::InsertConnectedCall(%s- Object Type %d ) Modified",key.c_str(),type);
    sobj->callEntry = (void*)callEntry;

  }else
    return false;
  return true;
}

bool xGateSipController::DeleteConnectedList(std::string key,xGateSofiaobj* sobj,XGATEOBJECT type){
  XGLOG_FUNCTION_EN_EX("");
  xGateCallPair * callpair =  GetConnectedList(key);
  if(callpair){
    callpair->Orgcall = nullptr;
    callpair->Dstcall = nullptr;
    m_CallConnectedList.unbind(key.c_str());
    XGLOG_INFO("xGateSipController::DeleteConnectList(%s)",key.c_str());
    if (callpair) delete callpair;
    callpair = nullptr;
    return true;
  }

  return false;
}

std::map<std::string,xGateSofiaobj*> *xGateSipController::GetCallList(std::string grpkey){
  std::map<std::string,xGateSofiaobj*> *calllist = nullptr;
  CALL_LIST::ITERATOR it(m_CallDialList);
  if(m_CallDialList.find(grpkey.c_str(),calllist) != -1)
    XGLOG_INFO("xGateSipController::GetCallList(Group ID = %s Group Found )",grpkey.c_str());

  return calllist;
}

bool xGateSipController::InsertCallList(std::string grpkey,xGateSofiaobj *sobj){
  std::map<std::string,xGateSofiaobj*> *calllist = GetCallList(grpkey);
  if(!calllist)
    calllist = new std::map<std::string,xGateSofiaobj*>;

  calllist->insert(std::pair<std::string,xGateSofiaobj*>(sobj->dialcallid,sobj));
  XGLOG_INFO("xGateSipController::InsertCallList %s insert count %d",grpkey.c_str(),calllist->size());
  CALL_LIST::ENTRY *callEntry = 0;
  int status = m_CallDialList.bind(grpkey.c_str(),calllist,callEntry);
  if(status == 0 && callEntry)
    XGLOG_INFO("xGateSipController::InsertCallList(%s) inserted success",grpkey.c_str());
  else if(status == 1 && callEntry)
    XGLOG_INFO("xGateSipController::InsertCallList(%s) inserted Modified",grpkey.c_str());
  else{
    XGLOG_ERROR("xGateSipController::InsertCallList(%s) inserted failed",grpkey.c_str());
    return false;
  }
  return true;
}

bool xGateSipController::DeleteCallList(std::string grpkey){
  CALL_LIST::ENTRY* callEntry = nullptr;
  std::map<std::string, xGateSofiaobj*>* calllist = GetCallList(grpkey);
  if(calllist){
    RemoveCallListValues(grpkey, calllist);
    delete calllist;
  }
  if(! m_CallDialList.find(grpkey.c_str(), callEntry)) {
    if(callEntry) {
      m_CallDialList.unbind(callEntry);
      XGLOG_INFO(THISMODULE,"xGateSipController::DeleteCallList(%s)",grpkey.c_str());
    }
  }
  return true;
}

bool xGateSipController::RemoveCallListValues(std::string grpkey, std::map<std::string, xGateSofiaobj*>* calllist){
  XGLOG_INFO("xGateSipController::RemoveCallListValues(%s)",grpkey.c_str());
  if(calllist == nullptr)
    return false;
  std::map<std::string, xGateSofiaobj*>::iterator itr;
  for(itr=calllist->begin();itr!=calllist->end();++itr){
    XGLOG_INFO("xGateSipController::RemoveCallListValues(%s) call-id %s", grpkey.c_str(), itr->first.c_str());
    //calllist->erase(itr);
  }
  calllist->clear();
  return true;
}

bool  xGateSipController::PostMessage(xGateCallObj *callobj,XGATEPOSTTYPE destType)
{
  try{
    xGateSipMsgBlock *pSipEventMsg = new xGateSipMsgBlock(); 
    pSipEventMsg->m_callObj = callobj;
    // Specify for Hold and Resume
    if(callobj->sipeventtype == EN_HOLDRESUME_REQ)
      pSipEventMsg->setMsgType(EN_XGATE_MSG_HOLDRESUME_IN);
    else if(callobj->sipeventtype == EN_P2PMEDIA_REQ)   // Peer to Peer to call
      pSipEventMsg->setMsgType(EN_XGATE_MSG_P2PMEDIA_IN);
    /*LWSS Code Start*/
    // else if(callobj->sipeventtype == EN_INVITE_REQ){ 
    //   pSipEventMsg->setMsgType(EN_XGATE_MSG_SIP_MODULE); 

    //  }   
     /*LWSS Code End*/

    ACE_Message_Block* pAmb = nullptr;
    pAmb = static_cast<xGateSipMsgBlock*> (pSipEventMsg);
    ACE_Time_Value tval ((time(nullptr) ));
    tval += ACE_Time_Value(0,1);
    //ACE_Task<ACE_MT_SYNCH>* taskPtr = nullptr;
	  bool bMsgSend =  false;
    std::string handleName;
    switch(destType){
      case XGATEPOSTTYPE::EN_DISPATCH:
        handleName = "SipSignalDispatcher";
       // taskPtr = (xGateSipSignalDispatcher*)GetSipPostHandler(XGATEPOSTTYPE::EN_DISPATCH);
       // SIP_SIGNAL_DISPATCHER::instance()->putq(pAmb,&tval);
	   bMsgSend = true;
        break;
      case XGATEPOSTTYPE::EN_PROCESS:
        handleName = "SipSignalProcessor";
		   SIP_SIGNAL_MODULE::instance()->putq(pAmb,&tval);
		bMsgSend = true;
      //  taskPtr = (xGateSipSignalProcessor*)GetSipPostHandler(XGATEPOSTTYPE::EN_PROCESS);
        break;

    }   
   
	if(bMsgSend)
		XGLOG_INFO(THISMODULE,"xGateSipController::PostMessage(%s) Successfully",handleName.c_str());
	else
		XGLOG_INFO(THISMODULE,"xGateSipController::PostMessage(%s) Failed",handleName.c_str());

  }catch(...){
    XGLOG_INFO(THISMODULE,"xGateSipController::PostMessage(%s)","Exception Raised");
  }

  return true;
}



