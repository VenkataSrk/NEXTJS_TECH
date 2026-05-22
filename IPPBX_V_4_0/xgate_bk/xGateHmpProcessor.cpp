

//local includes
#include "log.h"
#include "klog.h"
#include "xGateHmpProcessor.h"
#include "xGateMGDispatcher.h"
#include "xGateHmpGstManager.h"
#include "xGateSipSignalProcessor.h"
#define THISMODULE "HmpProc"

xGateHmpProcessor::xGateHmpProcessor(const char *serverType):xGateProcessor(serverType)
{
  CLog::Detail(THISMODULE, "xGateHmpProcessor::xGateHmpProcessor");
  xGateUtil::SetLocalIPAddress();
}

xGateHmpProcessor::~xGateHmpProcessor(void)
{
  CLog::Detail(THISMODULE, "xGateHmpProcessor::~xGateHmpProcessor");
}

xGateRetVal xGateHmpProcessor::process_msg(xGateBaseMsg* pMsg)
{
  CLog::Detail(THISMODULE, "Enter xGateHmpProcessor::process_msg(%s)", m_serverType); 
  xGateRetVal retVal = EN_XGATE_STATUS_ERROR;
  switch(pMsg->getMsgType()) {
    case EN_XGATE_MSG_MGC_MG_IN:
      {
        CLog::Detail(THISMODULE, "xGateHmpProcessor::process_msg (processing EN_XGATE_MSG_MGC_MG_IN message)"); 
        xGateMgMsg *pMgMsg = dynamic_cast<xGateMgMsg*>(pMsg);
	//TODO: handle channel request from client
        retVal = handle_channel_req(pMgMsg);
        if(retVal == EN_XGATE_STATUS_SUCCESS) {
            post_media_detail(pMgMsg);
        } else if(retVal == EN_XGATE_STATUS_ERROR) {
          CLog::Error(THISMODULE, "Enter xGateHmpProcessor::process_msg handle_channel_req failed");
        }
        break;
      }
    case EN_XGATE_MSG_MGC_MG_OUT:
      {
        //at present we don't need to support this from HmpProcessor thread.
        CLog::Detail(THISMODULE, "xGateHmpProcessor::process_msg (processing EN_XGATE_MSG_MGC_MG_OUT message)");
        break;
      }
    case EN_XGATE_MSG_UNKNOWN:
    default:
      {
        CLog::Error(THISMODULE, "Enter xGateHmpProcessor::process_msg is EN_XGATE_MSG_UNKNOWN");
        break;
      }      
  }
  return EN_XGATE_STATUS_SUCCESS; 
}

xGateRetVal xGateHmpProcessor::handle_channel_req(xGateMgMsg* pMgMsg) {
  CLog::Detail(THISMODULE, "xGateHmpProcessor::handle_channel_req");

  xGateHmpGstManager *pHmpGstManager = xGateUtil::getHmpGstManager();
  //xGateHmpGstManager *pHmpGstManager = NULL;
  switch(pMgMsg->get_mg_msg_type()) {

    case EN_XGATE_MG_PLAY_BACK:
	{
         MgMediaDetail &mediaDetail = pMgMsg->get_media_detail();
         CLog::Detail(THISMODULE, "xGateHmpProcessor::process_msg processing media PLAY_BACK request");
         if(pHmpGstManager) {
          //pHmpGstManager->handle_ivr_request(mediaDetail);
          pHmpGstManager->handle_play_back(mediaDetail);
          return EN_XGATE_STATUS_NOT_SUPPORTED; //Because, we don't need to send relase response to MGC server
        }
        break;
      }
    case EN_XGATE_MG_HOLD_RESUME:
      {
        MgMediaDetail &mediaDetail = pMgMsg->get_media_detail();
        CLog::Detail(THISMODULE, "xGateHmpProcessor::handle_channel_req processing media HOLD_RESUME request");
        if(pHmpGstManager) {
          //pHmpGstManager->handle_hold_resume(mediaDetail);
          pHmpGstManager->handle_play_back(mediaDetail);
          return EN_XGATE_STATUS_NOT_SUPPORTED; //Because, we don't need to send relase response to MGC server
         }
         break;
        }
    case EN_XGATE_MG_DTMF_DIGITS:
      {
        MgMediaDetail &mediaDetail = pMgMsg->get_media_detail();
        CLog::Detail(THISMODULE, "xGateHmpProcessor::handle_channel_req processing media DTMF_DIGITS request");
        if(pHmpGstManager) {
           pHmpGstManager->handle_dtmf_request(mediaDetail);
        }
        break;
      }
    case EN_XGATE_MG_ALLOCATE_TERMINATE:   //Invite Received with SDP from Leg-A  [Msg-3]
        {
        MgMediaDetail &mediaDetail = pMgMsg->get_media_detail();
        CLog::Detail(THISMODULE, "xGateHmpProcessor::process_msg processing media ALLOCATE_TERMINATE request");
        if(pHmpGstManager) {
          pHmpGstManager->allocate_orig_channel(mediaDetail);  // Origination (Leg-A) side media channel.
        }
        break;
        }

    case EN_XGATE_MG_18X_RESPONSE:
       {
         MgMediaDetail &mediaDetail = pMgMsg->get_media_detail();
         CLog::Detail(THISMODULE, "xGateHmpProcessor::process_msg processing media 18X_RESPONSE request");
         if(pHmpGstManager) {
          //TODO: Yoga, don't need to call allocate_playback_channel any more
          pHmpGstManager->handle_18X_response(mediaDetail);   //Creating Orgination (leg-A) side port and IP.
          pHmpGstManager->handle_play_back(mediaDetail);
         }
         break;
       }
    case EN_XGATE_MG_ALLOCATE_ORGINATE:     // 200 OK Received with SDP from leg-B [Msg-2]
       {
        MgMediaDetail &mediaDetail = pMgMsg->get_media_detail();
        CLog::Detail(THISMODULE, "xGateHmpProcessor::process_msg processing media ALLOCATE_ORGINATE request");
        if(pHmpGstManager) {
          pHmpGstManager->allocate_term_channel(mediaDetail);   //Termination (leg-B) side media channel.
          pHmpGstManager->connect_media_channel(mediaDetail);
        }
        break; 
       } 
     
    case EN_XGATE_MG_RELEASE:
       {
        MgMediaDetail &mediaDetail = pMgMsg->get_media_detail();
        CLog::Detail(THISMODULE, "xGateHmpProcessor::process_msg processing media RELEASE request");
        if(pHmpGstManager) {
          pHmpGstManager->release_channel(mediaDetail);
          return EN_XGATE_STATUS_NOT_SUPPORTED; //Because, we don't need to send relase response to MGC server
        }
        break;
      }
  }
  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateHmpProcessor::post_media_detail(xGateMgMsg *pMgMsg)
{
  CLog::Detail(THISMODULE, "Enter xGateHmpProcessor::post_media_detail");
  xGateMgMsgType msgType = pMgMsg->get_mg_msg_type();  
  xGateMgMsg* pMgMsg1 = NULL;
  xGateNetConnection netConInfo;
  pMgMsg1 = new xGateMgMsg();

  pMgMsg1->setMsgType(EN_XGATE_MSG_MGC_MG_IN);
  //printf("xGateHmpProcessor::post_media_detail (posting msg: %d)", pMgMsg->get_mg_msg_type());
  pMgMsg1->set_mg_msg_type(pMgMsg->get_mg_msg_type());
  if(msgType == EN_XGATE_MG_PLAY_BACK) {
    pMgMsg1->set_playback_detail(pMgMsg->get_playback_detail());
  } else {
    pMgMsg1->set_media_detail(pMgMsg->get_media_detail());
  }
  pMgMsg->get_net_con_info(netConInfo);
  pMgMsg1->set_net_con_info(netConInfo);

  ACE_Task<ACE_MT_SYNCH>* taskPtr = NULL;
  //taskPtr = xGateUtil::getMgDispatcher();
  taskPtr = (xGateSipSignalProcessor*)GetSipPostHandler(XGATEPOSTTYPE::EN_PROCESS);
  if(!taskPtr) {
    CLog::Error(THISMODULE, "xGateHmpProcessor::post_media_detail MgDispatcher task not found");
    return EN_XGATE_STATUS_ERROR;
  }

  ACE_Message_Block* pAmb = NULL;
  pAmb = static_cast<xGateMgMsg*> (pMgMsg1);
  ACE_Time_Value tval ((time(NULL) ));
  tval += ACE_Time_Value(0,1);

  if (taskPtr->putq(pAmb, &tval)) {
    CLog::Detail(THISMODULE, "xGateHmpProcessor::post_media_detail MgDispatcher enqueue message success");
  } else {
    CLog::Error(THISMODULE, "xGateHmpProcessor::post_media_detail MgDispatcher enqueue message failed");
    delete pMgMsg;
    return EN_XGATE_STATUS_ERROR;
  }

  return EN_XGATE_STATUS_SUCCESS;
}
