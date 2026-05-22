

//local includes
#include "xglog.h"
#include "xGateHmpProcessor.h"
//#include "xGateMGDispatcher.h"
//#include "xGateSipUtil.h"
#include "xGateSipSignalProcessor.h"
#include "xGateHmpGstManager.h"
#define THISMODULE "HmpProc"

xGateHmpProcessor::xGateHmpProcessor(const char *serverType):xGateProcessor(serverType)
{
  XGLOG_INFO( "xGateHmpProcessor::xGateHmpProcessor");
}

xGateHmpProcessor::~xGateHmpProcessor(void)
{
  XGLOG_INFO( "xGateHmpProcessor::~xGateHmpProcessor");
}

xGateRetVal xGateHmpProcessor::process_msg(xGateBaseMsg* pMsg)
{
  XGLOG_INFO( "Enter xGateHmpProcessor::process_msg(%s)", m_serverType); 
  xGateRetVal retVal = EN_XGATE_STATUS_ERROR;
  switch(pMsg->getMsgType()) {
    case EN_XGATE_MSG_MGC_MG_IN:
      {
        XGLOG_INFO( "xGateHmpProcessor::process_msg (processing EN_XGATE_MSG_MGC_MG_IN message)"); 
        xGateMgMsg *pMgMsg = dynamic_cast<xGateMgMsg*>(pMsg);
        //TODO: handle channel request from client
        retVal = handle_channel_req(pMgMsg);
        if(retVal == EN_XGATE_STATUS_SUCCESS) {
          post_media_detail(pMgMsg);
          XGLOG_INFO( "Enter xGateHmpProcessor::process_msg Posted Msg successfully to SignalProcessor");
        } else if(retVal == EN_XGATE_STATUS_ERROR) {
          XGLOG_ERROR( "Enter xGateHmpProcessor::process_msg handle_channel_req failed");
        }
        break;
      }
    case EN_XGATE_MSG_MGC_MG_OUT:
      {
        //at present we don't need to support this from HmpProcessor thread.
        XGLOG_INFO( "xGateHmpProcessor::process_msg (processing EN_XGATE_MSG_MGC_MG_OUT message)");
        break;
      }
    case EN_XGATE_MSG_UNKNOWN:
    default:
      {
        XGLOG_ERROR( "Enter xGateHmpProcessor::process_msg is EN_XGATE_MSG_UNKNOWN");
        break;
      }      
  }
  return EN_XGATE_STATUS_SUCCESS; 
}

xGateRetVal xGateHmpProcessor::handle_channel_req(xGateMgMsg* pMgMsg) {
  XGLOG_INFO( "xGateHmpProcessor::handle_channel_req start");

  xGateHmpGstManager *pHmpGstManager = xGateUtil::getHmpGstManager();
  if(!pHmpGstManager) {
    XGLOG_ERROR( "handle_channel_req failed. Invalid HmpGstManager object !!");
    return EN_XGATE_STATUS_ERROR;
  }

  MgMediaDetail &mediaDetail = pMgMsg->get_media_detail();
  xGateNetConnection pbxConn; 
  pMgMsg->get_net_con_info(pbxConn);


  switch(pMgMsg->get_mg_msg_type()) {
    case EN_XGATE_MG_ALLOCATE_RECORD:   
      {
        XGLOG_INFO( "handle_channel_req processing ALLOCATE_RECORD: request");
        pHmpGstManager->allocate_receive_channel(mediaDetail, pbxConn); 
        break;
      }
    case EN_XGATE_MG_RELEASE_RECORD:
      {
        XGLOG_INFO( "handle_channel_req processing RELEASE_RECORD request");
        pHmpGstManager->release_receive_channel(mediaDetail);
        return EN_XGATE_STATUS_NOT_SUPPORTED; //Because, we don't need to send relase response to MGC server
        break;
      }
    case EN_XGATE_MG_CALL_RECORD:
      {
        XGLOG_INFO( "handle_channel_req processing CALL_RECORD");
        pHmpGstManager->handle_call_record(mediaDetail);
        break;
      }
    case EN_XGATE_MG_MEDIA_DELETE:
      {
        XGLOG_INFO( "handle_channel_req processing MG_MEDIA_DELETE");
        pHmpGstManager->media_delete_channel(mediaDetail); //deleting the client & channel
        return EN_XGATE_STATUS_NOT_SUPPORTED; //Because, we don't need to send delete response to PBX server
        break;
      }
    case EN_XGATE_MG_MP3_DETAIL:
     {
        XGLOG_INFO("handle_channel_req processing EN_XGATE_MG_MP3_DETAIL");
        XGLOG_INFO("mediaResource_id=%s,call_id=%s,mp3_duration=%u,mp3_filesize =%u,is_RecordingStatus=%d",(char*)ur_log_string(mediaDetail.mediaResource_id),(char*)ur_log_string(mediaDetail.call_id),mediaDetail.mp3_duration,mediaDetail.mp3_filesize,mediaDetail.isRecordingStatus);
         break;

      }
    case EN_XGATE_MG_TYPE_UNKNOWN:
    default:
      {
        XGLOG_ERROR( "handle_channel_req failed. Not able to process MG_TYPE_UNKNOWN request !!");
        return EN_XGATE_STATUS_NOT_SUPPORTED;
        break;
      }
  }
  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateHmpProcessor::post_media_detail(xGateMgMsg *pMgMsg)
{
  XGLOG_INFO( "Enter xGateHmpProcessor::post_media_detail");
  xGateMgMsgType msgType = pMgMsg->get_mg_msg_type();  
  xGateMgMsg* pMgMsg1 = NULL;
  xGateNetConnection netConInfo;
  pMgMsg1 = new xGateMgMsg();

  pMgMsg1->setMsgType(EN_XGATE_MSG_MGC_MG_IN);
  //printf("xGateHmpProcessor::post_media_detail (posting msg: %d)", pMgMsg->get_mg_msg_type());
  pMgMsg1->set_mg_msg_type(pMgMsg->get_mg_msg_type());
  pMgMsg1->set_media_detail(pMgMsg->get_media_detail());
  pMgMsg->get_net_con_info(netConInfo);
  pMgMsg1->set_net_con_info(netConInfo);

  ACE_Task<ACE_MT_SYNCH>* taskPtr = NULL;
  //taskPtr = xGateUtil::getMgDispatcher();
  taskPtr = (xGateSipSignalProcessor*)GetSipPostHandler(XGATEPOSTTYPE::EN_PROCESS);
  if(!taskPtr) {
    XGLOG_ERROR( "xGateHmpProcessor::post_media_detail MgDispatcher task not found");
    return EN_XGATE_STATUS_ERROR;
  }

  ACE_Message_Block* pAmb = NULL;
  pAmb = static_cast<xGateMgMsg*> (pMgMsg1);
  ACE_Time_Value tval ((time(NULL) ));
  tval += ACE_Time_Value(0,1);

  if (taskPtr->putq(pAmb, &tval)) {
    XGLOG_INFO( "xGateHmpProcessor::post_media_detail MgDispatcher enqueue message success");
  } else {
    XGLOG_ERROR( "xGateHmpProcessor::post_media_detail MgDispatcher enqueue message failed");
    delete pMgMsg;
    return EN_XGATE_STATUS_ERROR;
  }

  return EN_XGATE_STATUS_SUCCESS;
}
