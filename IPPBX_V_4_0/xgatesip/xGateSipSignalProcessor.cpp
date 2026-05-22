#include "xGateSipSignalProcessor.h"
#include "xGateMgMsg.h"
#include "xGateSipController.h"
#include "xglog.h"
#define THISMODULE "SPQ"
#include <cstdlib>
/*
*/
xGateSipSignalProcessor::xGateSipSignalProcessor() : 
  xGateProcessor("SipProcessor") //, m_businessHandler(NULL)  //commented for SRS/SRC
{
  XGLOG_ERROR( "multithreaded-ippbx : xGateSipSignalProcessor::xGateProcessor(%s)", "SPQ Thread Started");
  m_run = true;
}

/*
*/
xGateSipSignalProcessor::~xGateSipSignalProcessor()
{
}

/*
*/
int xGateSipSignalProcessor::svc( void )

{
  // Message block in which the message is read
  ACE_Message_Block *pAmb = 0;
  // Till the flag run_ is true this loop will continue and the thread will
  // exist
  while(m_run) {
    // Read the message from the queue. If no message is present then this
    // method will block. This methods returns when the message is read or
    // the queue is deactivated
    ACE_Time_Value tv ((time(NULL) + 20));

    // Get message from the queue
    int result = getq(pAmb, &tv);
    if((result == -1) && (errno == EWOULDBLOCK)) {
      continue;
    }

    // check the result
    if((result != -1)) {
      ///Process and Dispatch Message
      process_msg(pAmb);
    } else {
      // This is the case where the message queue is de-activated.
      // Need to stop the thread
      XGLOG_ERROR( "xGateSipSignalProcessor::svc(%s) Error in Message Queue", "SPQ");
      m_run = false;
    }

    // delete the message received
    if(pAmb) {
      delete pAmb;
      pAmb = NULL;
    }
  }
  return 0;
}

// #ifdef XGATE_HAS_BUSINESS_SUPPORT
// xGateRetVal xGateSipSignalProcessor::initBusinessHandler()
// {
//   if(!m_businessHandler) {
//     m_businessHandler = new xGateBusinessHandler();
//   }
//   return EN_XGATE_STATUS_SUCCESS;
// }
// #endif

/*
*/

xGateRetVal xGateSipSignalProcessor::initHTTPBridge()
{

        if(!URMODULE_BRIDGE::instance()->init())
        {
          cout<<"URModuleBridge init() failed"<<endl;
        }
	      string strSsrvURL =  XGCONFIG().m_srsServerInfoProfile.m_save_server_info_url;
        //string strSsrvURL("http://10.30.3.39:5002/v1/call_recorder/save_server");
        //string strSsrvURL("http://ursip-api.unifiedring.co.uk/v1/call_recorder/save_server");
        std::string ServerJason = EncodeSRSServerInfo() ; 
        std::string tmp_call_id ="123456";  
         
        //URMODULE_BRIDGE::instance()->PostMessagetoHTTP(tmp_call_id,strSsrvURL,ServerJason);
        #if 0
                cout<<"Message streaming starting"<<endl;
                //sleep(1);
                for(int i= 0; i< 5;i++)
                {
                        URMODULE_BRIDGE::instance()->testsendPublishMessage(1);
                        sleep(1);
                }
        #endif
        return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateSipSignalProcessor::process_msg(ACE_Message_Block *pAmb)
{
  xGateRetVal retval = EN_XGATE_STATUS_SUCCESS;
  if(pAmb) {
    XGLOG_INFO("multithreaded-ippbx : xGateSipSignalProcessor::process_msg(%s)", "Received Msg");
    xGateBaseMsg *pBaseMsg = NULL;
    pBaseMsg = dynamic_cast<xGateBaseMsg*>(pAmb);
    switch(pBaseMsg->getMsgType()) {
      case EN_XGATE_MSG_SIP_IN:
        {
          XGLOG_INFO( "xGateSipSignalProcessor::process_msg processing EN_XGATE_MSG_SIP_IN");
          xGateSipMsgBlock *pSipMsg = dynamic_cast<xGateSipMsgBlock*>(pAmb);
          if(pSipMsg) {
            // SendToMainProcessor(pSipMsg->m_callObj);
            processBusinessRequest(pSipMsg->m_callObj);
          }
          break;
        }
#if 0 //SRC
#if 0 //TODO: Yoga, disabled for business layer restructuring
      case EN_XGATE_MSG_DB_IN:
        {
          XGLOG_INFO( "xGateSipSignalProcessor::process_msg processing EN_XGATE_MSG_DB_IN");
          xGateSipMsgBlock *pDBMsg = dynamic_cast<xGateSipMsgBlock*>(pAmb);
          if(pDBMsg){
            processBusinessResponse(pDBMsg->m_dbObj);
          }
          break;
        }
#else
      case EN_XGATE_MSG_FROM_BUSINESS:
        {
          XGLOG_INFO( "process_msg processing EN_XGATE_MSG_FROM_BUSINESS");
          xGateBusinessMsg *pBusinessMsg = dynamic_cast<xGateBusinessMsg*>(pAmb);
          if(pBusinessMsg){
            processBusinessResponse(pBusinessMsg);
          }
          break;
        }
#endif
#endif
      case EN_XGATE_MSG_HOLDRESUME_IN:
      case EN_XGATE_MSG_P2PMEDIA_IN:
        {
          // Loop Back Logic
          XGLOG_INFO( "xGateSipSignalProcessor::process_msg processing EN_XGATE_MSG_HOLDRESUME_IN/EN_XGATE_MSG_P2PMEDIA_IN");
          xGateSipMsgBlock *pSipMsg = dynamic_cast<xGateSipMsgBlock*>(pAmb);
          if(pSipMsg){
            SendToReceiver(pSipMsg->m_callObj);
            // delete pSipMsg
          }
          else
            XGLOG_INFO( "xGateSipSignalProcessor::process_msg EN_XGATE_MSG_HOLDRESUME_IN : NOT SENDING to Receiver");

          break;
        }
      case EN_XGATE_MSG_MGC_MG_IN:
        {
          XGLOG_INFO( "xGateSipSignalProcessor::process_msg processing EN_XGATE_MSG_MGC_MG_IN");
          xGateMgMsg *pMgMsg = dynamic_cast<xGateMgMsg*>(pAmb);
          if(pMgMsg){
            //TODO: Yoga, need to develope logic to send to received thread
            handle_media_response(pMgMsg);
          }
        }
        break;
      case EN_XGATE_MSG_UNKNOWN:
      default:
        {
          XGLOG_ERROR( "multithreaded-ippbx : xGateSipSignalProcessor::process_msg failed due to EN_XGATE_MSG_UNKNOWN");
          break;
        }      

    }
  }
  return retval;
}

/*
*/
xGateRetVal xGateSipSignalProcessor::stop(){
  m_run = false;
  msg_queue()->deactivate();
  return EN_XGATE_STATUS_SUCCESS;
}

/*
*/
void xGateSipSignalProcessor::SendToMainProcessor(xGateCallObj *obj)
{
  // Send to MGC-MG Componet with obj info
  xGateEnQcallobj(obj);

  XGLOG_INFO("multithreaded-ippbx : xGateSipSignalProcessor::SenToMainProcessor(%s)",obj->callid.c_str());
}
void xGateSipSignalProcessor::SendToReceiver(xGateCallObj *callobj){
  // Create a queue // any mechanism to send to Receiver threadss
  XGLOG_INFO("xGateSipSignalProcessor::SendToReceiver(%s)",callobj->callid.c_str());
  if(callobj->sipeventtype == EN_HOLDRESUME_REQ)
    callobj->sipeventtype = EN_HOLDRESUME_REL;
  else if(callobj->sipeventtype == EN_P2PMEDIA_REQ)
    callobj->sipeventtype = EN_P2PMEDIA_REL;

  WriteToReceiver(callobj);

}
/*
 * Handle media response from MGC-MG component i.e from xGateSrvrProcessor
 */
//For PBX this part not required
xGateRetVal xGateSipSignalProcessor::handle_media_response(xGateMgMsg *pMgMsg)
{
  XGLOG_INFO("Enter xGateSipSignalProcessor::handle_media_response");
  MgMediaDetail &mediaDetail = pMgMsg->get_media_detail();
  if(!mediaDetail.call_id.empty()) {
    xGateCallObj *callObj =  new xGateCallObj;
    //CallParameter cp;
    //    callObj->callstate = CS_Null;
    //callObj->cp = cp;
    callObj->callid = mediaDetail.call_id;
    callObj->uid = mediaDetail.mediaResource_id;
    //callObj->sdpInfo[0] = mediaDetail.sdpinfo[0];
    //callObj->sdpInfo[1] = mediaDetail.sdpinfo[1];
    // need to change to array after testing compltes
    callObj->sdpInfo[0].media_dtmf_digits = "";
    callObj->sdpInfo[1].media_dtmf_digits = "";

    callObj->sdpInfo[0].ipaddress = mediaDetail.sdpinfo[0].ip_addr;
    callObj->sdpInfo[0].port = mediaDetail.sdpinfo[0].port;
    callObj->sdpInfo[0].codec = mediaDetail.sdpinfo[0].codec;
    //    callObj->calldir = mediaDetail.call_dir;
    callObj->sdpInfo[0].iceufrag = mediaDetail.sdpinfo[0].ice_ufrag;
    callObj->sdpInfo[0].icepwd = mediaDetail.sdpinfo[0].ice_pwd;
    callObj->sdpInfo[0].fingerprint = mediaDetail.sdpinfo[0].fingerprint;
    callObj->sdpInfo[0].ssrc = mediaDetail.sdpinfo[0].ssrc;
    callObj->sdpInfo[0].cname = mediaDetail.sdpinfo[0].cname;
    callObj->sdpInfo[0].mslabel = mediaDetail.sdpinfo[0].mslabel;
    callObj->sdpInfo[0].label =  mediaDetail.sdpinfo[0].label;
    callObj->sdpInfo[0].ptime = mediaDetail.sdpinfo[0].ptime;
    callObj->sdpInfo[0].mediadialip =  mediaDetail.sdpinfo[0].dial_ip;
    callObj->sdpInfo[0].mediadialport =  mediaDetail.sdpinfo[0].dial_port;
    callObj->sdpInfo[0].mediaresponseip = mediaDetail.sdpinfo[0].resp_ip;
    callObj->sdpInfo[0].mediaresponseport = mediaDetail.sdpinfo[0].resp_port;
    callObj->sdpInfo[0].mediatype = mediaDetail.sdpinfo[0].mediaType; 
    callObj->sdpInfo[1].ipaddress = mediaDetail.sdpinfo[1].ip_addr;
    callObj->sdpInfo[1].port = mediaDetail.sdpinfo[1].port;
    callObj->sdpInfo[1].codec = mediaDetail.sdpinfo[1].codec;
    //    callObj->calldir = mediaDetail.call_dir;
    callObj->sdpInfo[1].iceufrag = mediaDetail.sdpinfo[1].ice_ufrag;
    callObj->sdpInfo[1].icepwd = mediaDetail.sdpinfo[1].ice_pwd;
    callObj->sdpInfo[1].fingerprint = mediaDetail.sdpinfo[1].fingerprint;
    callObj->sdpInfo[1].ssrc = mediaDetail.sdpinfo[1].ssrc;
    callObj->sdpInfo[1].cname = mediaDetail.sdpinfo[1].cname;
    callObj->sdpInfo[1].mslabel = mediaDetail.sdpinfo[1].mslabel;
    callObj->sdpInfo[1].label =  mediaDetail.sdpinfo[1].label;
    callObj->sdpInfo[1].ptime = mediaDetail.sdpinfo[1].ptime;
    callObj->sdpInfo[1].mediadialip =  mediaDetail.sdpinfo[1].dial_ip;
    //    callObj->sdpInfo[1].mediadialip = ;    
    callObj->sdpInfo[1].mediadialport =  mediaDetail.sdpinfo[1].dial_port;
    callObj->sdpInfo[1].mediaresponseip = mediaDetail.sdpinfo[1].resp_ip;
    callObj->sdpInfo[1].mediaresponseport = mediaDetail.sdpinfo[1].resp_port;
    callObj->sdpInfo[1].mediatype = mediaDetail.sdpinfo[1].mediaType;


    //    printf("\n Digit received on Media in Signal Processor %d\n", mediaDetail.ptime);
    	XGLOG_INFO("mediaDetail.dtmf_digits = %s\n", mediaDetail.dtmf_digits.c_str());
    switch(pMgMsg->get_mg_msg_type()) {
      case EN_XGATE_MG_PLAY_BACK: 
      case EN_XGATE_MG_DTMF_DIGITS: 
        {
          if(mediaDetail.pb_state == EN_XGATE_PB_EOS) {
            callObj->sipeventtype = EN_PB_COMPLETE_REL;
          } else if(mediaDetail.pb_state == EN_XGATE_PB_RECVDIGIT) {
            callObj->sipeventtype = EN_MEDIA_RECVDIGIT_REL;
            callObj->sdpInfo[0].media_dtmf_digits = mediaDetail.dtmf_digits;
            XGLOG_INFO("mediaDetail.dtmf_digits = %s\n", mediaDetail.dtmf_digits.c_str());
          } else {
            callObj->sipeventtype = EN_NULL;
          }
          break;
        }
      case EN_XGATE_MG_ALLOCATE_ORGINATE:
        {
          callObj->sipeventtype = EN_SETMEDIA_OREL; // Originate Response
          break;
        }
      case EN_XGATE_MG_ALLOCATE_TERMINATE:
        {
          callObj->sipeventtype = EN_SETMEDIA_TREL; // Terminate Response
          break;
        }
      case EN_XGATE_MG_ALLOCATE_RECORD:
        {
          callObj->sipeventtype = EN_SETMEDIA_EREL; // Terminate Response
          break;
        }
      case EN_XGATE_MG_MP3_DETAIL:
       {
	callObj->sipeventtype = EN_MP3_DETAIL;
        callObj->mp3_duration = mediaDetail.mp3_duration;
        callObj->mp3_filesize = mediaDetail.mp3_filesize;
	callObj->isRecordingStatus= mediaDetail.isRecordingStatus;
	break;
       }	
      case EN_XGATE_MG_18X_RESPONSE: 
      case EN_XGATE_MG_MEDIA_CREATE: 
        {
	  callObj->sipeventtype = EN_SETMEDIA_EREL; // Early Response
          break; 
        }
      case EN_XGATE_MG_ADD_PARTICIPANT: 
        {
          callObj->sipeventtype = EN_CONF_EVENT_REL; // Early Response
          break; 
        }
       case EN_XGATE_MG_CONFERENCE_FOR_CLIENT:
	{
		callObj->sipeventtype = EN_MULTICONF_MEDIA_REL; // Ad-Hoc conference request
		callObj->uid = mediaDetail.call_id; // Since only port creation use old id Yadav 
		break;
	}
      default:
        {
          callObj->sipeventtype = EN_NULL;
          break;
        }
    }
    WriteToReceiver(callObj);
  } else {
    XGLOG_ERROR("xGateSipSignalProcessor::handle_media_response failed due to invalide media details");
    return EN_XGATE_STATUS_ERROR;
  }
  return EN_XGATE_STATUS_SUCCESS;
}

void xGateSipSignalProcessor::processBusinessRequest(xGateCallObj* callObj)
{
  if(!callObj) {
    XGLOG_INFO( " xGateSipSignalProcessor::processBusinessRequest Error callobj NULL");
    return ;
  }

  XGLOG_INFO( " xGateSipSignalProcessor::processBusinessRequest %s", callObj->callid.c_str());
  switch(callObj->sipeventtype)
  {
    case EN_INVITE_REQ:
      {
        if ((strlen(callObj->cp.cnCalled.c_str()) > 5 ||  strlen(callObj->cp.cnCalling.c_str())> 10)) {
          callObj->cp.isPstnNumber = true;
        }
#if 0 //SRC
#if 0 //TODO: Yoga disabled this
        xGateDBObj *dbobj = new xGateDBObj();
        dbobj->cp = callObj->cp;
        dbobj->dbevent =  EN_DB_PREQUERY_REQ;
        PostMessageToDBManager(dbobj);
#else //TODO: Yoga post the above event to business processor
        m_businessHandler->createRequest(callObj->cp, EN_BUSINESS_EVENT_INIT);
        //m_businessHandler->createRequest(callObj->cp, EN_BUSINESS_EVENT_CDRUPDATE);
#endif
#endif
        break;
      }
    case EN_FORKING_REQ:
      {
#if 0 //SRC
#if 0 //TODO: Yoga disabled this
        xGateDBObj *dbobj = new xGateDBObj();
        dbobj->cp = callObj->cp;
        dbobj->dbevent =  EN_DB_NOTIFYE_REQ;
        PostMessageToDBManager(dbobj);
#else
        m_businessHandler->createRequest(callObj->cp, EN_BUSINESS_EVENT_FORKING);
#endif
#endif
        break;
      }
    case EN_CDR_UPDATE_REQ:{ // CDR Updation
#if 0 //SRC
      m_businessHandler->createRequest(callObj->cp, EN_BUSINESS_EVENT_CDRUPDATE);
#endif
    break;
    }
    case EN_TRANSFER_REQ:
      {
        xGateDBObj *dbobj =  new xGateDBObj();
        dbobj->cp = callObj->cp;
        dbobj->dbevent = EN_DB_REGDEATILS_REQ;
        PostMessageToDBManager(dbobj);
        break;
      }
    case EN_APPREGCOMPLET_REQ:
      {
        xGateDBObj *dbobj =  new xGateDBObj();
        dbobj->cp = callObj->cp;
        dbobj->cp.m_mappedUserList = callObj->cp.m_mappedUserList;
        dbobj->cp.confinfo = callObj->cp.confinfo;
        dbobj->dbevent = EN_DB_GETREG_REQ;
        PostMessageToDBManager(dbobj);
        break;
      }
    case EN_SWBMAINMENUIVR_TIMEOUT_RES:
      {
        xGateDBObj *dbobj;
        dbobj = new xGateDBObj();
        dbobj->cp = callObj->cp;
        dbobj->cp.swbinfo = callObj->cp.swbinfo;
        dbobj->dbevent =  EN_DB_SWBMAINMENUIVRTIMEOUT_RES;
        PostMessageToDBManager(dbobj);
        break;
      }
    case EN_SWB_EVENT_REQ:
      {
        xGateDBObj *dbobj;
        dbobj = new xGateDBObj();

        dbobj->cp = callObj->cp;
        dbobj->cp.swbinfo = callObj->cp.swbinfo;
        dbobj->cp.swbinfo.SwbCallEvent = callObj->cp.swbinfo.SwbCallEvent;
        dbobj->dbevent =  EN_DB_SWB_EVENT_REQ;
        PostMessageToDBManager(dbobj);
        break;
      }
    case EN_CONFERENCE_DTMFDIGIT_TIMEOUT_RES:
      {
        xGateDBObj *dbobj;
        dbobj = new xGateDBObj();
        dbobj->cp = callObj->cp;
        dbobj->cp.confinfo = callObj->cp.confinfo;
        dbobj->dbevent =  EN_DB_CONFERENCEIVR_TIMEOUT_RES;
        PostMessageToDBManager(dbobj);
        break;
      }
    case EN_CONF_EVENT_REQ:
      {
        xGateDBObj *dbobj;
        dbobj = new xGateDBObj();
        dbobj->cp = callObj->cp;
        dbobj->cp.confinfo = callObj->cp.confinfo;
        dbobj->cp.confinfo.gm_conf_host_extension = callObj->cp.confinfo.gm_conf_host_extension;
        dbobj->dbevent =  EN_DB_CONF_EVENT_REQ;
        PostMessageToDBManager(dbobj);
        break;
      }
    default:
      {
        break;
      }
  }
}

#if 0 //SRC
#if 1
bool xGateSipSignalProcessor::processBusinessResponse(xGateBusinessMsg *pMsg)
{
  if(!pMsg) {
    return;
  }
  const char  *data = 0;
  unsigned int dataLen = 0;

  pMsg->get_data(data);
  dataLen = pMsg->get_data_len();

  if(!data && dataLen <=0) {
    XGLOG_ERROR( "processBusinessResponse failed. Invalid data in xGateBusinessMsg !");
    return false;
  }

  xGateCallObj *callobj = new xGateCallObj;
  if(!m_businessHandler->processResponse(data, callobj)) {
    XGLOG_ERROR( "processBusinessResponse failed !");
    return false;
  }
  WriteToReceiver(callobj);
}
#else
void xGateSipSignalProcessor::processBusinessResponse(xGateDBObj *dbobj)
{
  if(!dbobj) {
    // Send a Error message to Signale server
    return ;
  }

  XGLOG_INFO( " xGateSipSignalProcessor::processBusinessResponse %s ; dbobj->dbevent = %d", dbobj->cp.call_id.c_str(), dbobj->dbevent);
  xGateCallObj *callobj = new xGateCallObj;
  switch(dbobj->dbevent) {
    case EN_DB_PREQUERY_RES:
      {
                              callobj->callid = dbobj->cp.call_id;
                              callobj->uid = dbobj->cp.uid;
                              callobj->cp = dbobj->cp;
                              callobj->cp.m_mappedUserList = dbobj->cp.m_mappedUserList;
                              if ((callobj->cp.isSwitchBoardCall == true) || (callobj->cp.isSwitchBoardCall_duringDialledExtension)) {
                                callobj->sipeventtype = EN_SWB_EVENT_REL;
                                callobj->cp.swbinfo = dbobj->cp.swbinfo;
                                callobj->cp.swbinfo.SwbCallEvent = dbobj->cp.swbinfo.SwbCallEvent;
        } else if (callobj->cp.isconferenceCall == true) {
                                  callobj->sipeventtype = EN_CONF_EVENT_REL;
                                  callobj->cp.confinfo = dbobj->cp.confinfo;
                                  callobj->cp.confinfo.ConfCallEvent = dbobj->cp.confinfo.ConfCallEvent;
                                  callobj->cp.confinfo.gm_conf_host_extension = dbobj->cp.confinfo.gm_conf_host_extension;
                                  callobj->cp.confinfo.conf_callFlowNo = dbobj->cp.confinfo.conf_callFlowNo;
                                }else
                                  callobj->sipeventtype =  EN_DIALOUT_REL;
                            break;
      }
    case EN_DB_NOTIFYE_RES:
      {
                             callobj->callid = dbobj->cp.call_id;

                             callobj->uid = dbobj->cp.uid;
                             callobj->cp = dbobj->cp;
                             callobj->cp.m_mappedUserList = dbobj->cp.m_mappedUserList;
                             callobj->sipeventtype = EN_FOKINGDIAL_REL;
                           break;
      }
    case EN_DB_REGDEATILS_RES:
      {
                           callobj->callid = dbobj->cp.call_id;
                           callobj->uid = dbobj->cp.uid;
                           callobj->cp = dbobj->cp;
                           callobj->cp.m_mappedUserList = dbobj->cp.m_mappedUserList;
                           callobj->sipeventtype = EN_TRANSFER_REL;
                           break;
      }
    case EN_SWB_EVENT_REL:
      {
                            callobj->callid = dbobj->cp.call_id;
                            callobj->uid = dbobj->cp.uid;
                            callobj->cp.m_mappedUserList = dbobj->cp.m_mappedUserList;
                            callobj->cp = dbobj->cp;
                            callobj->cp.swbinfo = dbobj->cp.swbinfo;
                            callobj->cp.swbinfo.SwbCallEvent = dbobj->cp.swbinfo.SwbCallEvent;
                            callobj->sipeventtype =  EN_SWB_EVENT_REL;
                          break;
      }
    case EN_CONF_EVENT_REL:
      {
                             callobj->callid = dbobj->cp.call_id;
                             callobj->uid = dbobj->cp.uid;
                             callobj->cp.m_mappedUserList = dbobj->cp.m_mappedUserList;
                             callobj->cp = dbobj->cp;
                             callobj->cp.confinfo = dbobj->cp.confinfo;
                             callobj->cp.confinfo.gm_conf_host_extension = dbobj->cp.confinfo.gm_conf_host_extension;
                             callobj->sipeventtype =  EN_CONF_EVENT_REL;
                           break;
      }
    default:
      {
                           break;
  }
  }
  WriteToReceiver(callobj);
}
#endif
#endif

/*
 *  Loop back to recevir without proces
 */
void xGateSipSignalProcessor::PostMessageToDBManager(xGateDBObj* dbObj)
{
  xGateSipMsgBlock *pDBMsg = new xGateSipMsgBlock(EN_XGATE_MSG_DBCONNECT_IN);
  pDBMsg->m_dbObj = dbObj;
  ACE_Message_Block* pAmb = NULL;
  pAmb = static_cast<xGateSipMsgBlock*> (pDBMsg);
  ACE_Time_Value tval ((time(NULL) ));
  tval += ACE_Time_Value(0,1);
  ACE_Task<ACE_MT_SYNCH>* taskPtr = NULL;
//  //taskPtr = (xGateDBManager*)GetSipPostHandler(XGATEPOSTTYPE::EN_DBMANAGER);
//  if(taskPtr){
//    if(taskPtr->putq(pAmb,&tval))
//      XGLOG_INFO("xGateSipController::PostMessage(xGateDBManager) Successfully");
//
//    //  PostMessage(pSipRegMsg,XGATEPOSTTYPE::EN_DBMANAGER);
//		printf("test");
//  }
}
Value xGateSipSignalProcessor::setJsonParam(string &strValue, Document &doc)
{
  Value retVal;
  retVal.SetString(strValue.c_str(), strValue.length(), doc.GetAllocator());
  return retVal;
}

Value xGateSipSignalProcessor::setJsonParam(unsigned int iValue, Document &doc)
{
  Value retVal;
  retVal = iValue;
  return retVal;
}
#if 1 
std::string xGateSipSignalProcessor::EncodeSRSServerInfo()
{

    //SRSServerConfig *ConfigLoad = SRSServerConfig::instance();

        Document doc;
        StringBuffer strBuf;
        Writer<StringBuffer> writer(strBuf);
        Value outputVal(kObjectType);
        Document::AllocatorType& allocator = doc.GetAllocator();
  
        outputVal.AddMember("server_id",setJsonParam(XGCONFIG().m_srsServerInfoProfile.m_server_id,doc),allocator);        
        outputVal.AddMember("server_dns",setJsonParam(XGCONFIG().m_srsServerInfoProfile.m_server_dns,doc),allocator);
        outputVal.AddMember("server_ip",setJsonParam(XGCONFIG().m_srsServerInfoProfile.m_server_ip,doc),allocator);
        outputVal.AddMember("server_mac",setJsonParam(XGCONFIG().m_srsServerInfoProfile.m_server_mac,doc),allocator);
        outputVal.AddMember("present_call_load",setJsonParam(XGCONFIG().m_srsServerInfoProfile.m_present_call_load,doc),allocator);
        outputVal.AddMember("max_call_load",setJsonParam(XGCONFIG().m_srsServerInfoProfile.m_max_call_load,doc),allocator);
        outputVal.AddMember("callrec_srv_local_weburl",setJsonParam(XGCONFIG().m_srsServerInfoProfile.m_callrec_srv_local_weburl,doc),allocator);
        outputVal.AddMember("callrec_srv_sip_login_number",setJsonParam(XGCONFIG().m_srsServerInfoProfile.m_callrec_srv_sip_login_number,doc),allocator);
        outputVal.AddMember("callsrv_sip_login_password",setJsonParam(XGCONFIG().m_srsServerInfoProfile.m_callsrv_sip_login_password,doc),allocator);
        outputVal.AddMember("callsrv_registrar_sip_url",setJsonParam(XGCONFIG().m_srsServerInfoProfile.m_callsrv_registrar_sip_url,doc),allocator);
        outputVal.AddMember("callsrv_registration_enabled",setJsonParam(XGCONFIG().m_srsServerInfoProfile.m_callsrv_registration_enabled,doc),allocator);
        outputVal.AddMember("callsrv_server_enabled",setJsonParam(XGCONFIG().m_srsServerInfoProfile.m_callsrv_server_enabled,doc),allocator);
        outputVal.AddMember("callsrv_server_preference_order",setJsonParam(XGCONFIG().m_srsServerInfoProfile.m_callsrv_server_preference_order,doc),allocator);

        outputVal.Accept(writer);
        string srsinfo("");
        srsinfo = strBuf.GetString();
        
return srsinfo;
}
#endif


