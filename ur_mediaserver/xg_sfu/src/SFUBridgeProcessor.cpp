//local includes
#include "SFUBridgeProcessor.h"
#include "SFUController.h"
#include <string>
#include "SFUController.h"
#include "xGateUtil.h"
#include "xGateSFUtil.h"
#include "SFUClient.h"
#include "xglog.h"
#include "MBCSForwarderController.h"
#include "xGateHmpGstBin.h"
using namespace std;
#define THISMODULE "SFUctrl"

extern SFUClient sfu_client;
int SFUBridgeProcessor::svc(void)
{
  XGLOG_INFO( "SFUBridgeProcessor::svc(%s)", m_serverType);
  printf("\nSFUBridgeProcessor::svc\n");
  ACE_Message_Block *pAmb = 0;

  while(m_run)
  {
    // Get message from the queue
    int result = getq(pAmb, NULL);
    if((result == -1) && (errno == EWOULDBLOCK))
    {
      continue;
    }

    if ((result != -1))
    {
      process_msg(pAmb);
    }
    else
    {
      // This is the case where the message queue is de-activated.
      // Need to stop the thread
      XGLOG_INFO( "SFUBridgeProcessor::svc(%s) Error in Message Queue", m_serverType);
      m_run = false;
    }

    // delete the message received
    if(pAmb)
    {
      delete pAmb;
      pAmb = NULL;
    }
  }
  XGLOG_INFO( "SFUBridgeProcessor::svc(%s) Stopped Dispatch Task Thread", m_serverType);
  return 0;
}

SFUBridgeProcessor::SFUBridgeProcessor(const char *serverType):xGateProcessor(serverType) 
{
  XGLOG_INFO( "@@SFU->SFUBridgeProcessor::SFUBridgeProcessor");
}
SFUBridgeProcessor::~SFUBridgeProcessor(void)
{
  XGLOG_INFO( "SFUBridgeProcessor::~SFUBridgeProcessor");
}
xGateRetVal SFUBridgeProcessor::process_msg(xGateBaseMsg* pMsg)
{
  
  xGateRetVal retVal = EN_XGATE_STATUS_ERROR;
  switch(pMsg->getMsgType()) {
    case EN_XGATE_MSG_MGC_MG_IN:
      {
        xGateMgMsg *pMgMsg = dynamic_cast<xGateMgMsg*>(pMsg);
        //TODO: handle channel request from client
        retVal = handle_channel_req(pMgMsg);
        if((retVal == EN_XGATE_STATUS_SUCCESS)
           && (pMgMsg->get_mg_msg_type() != EN_XGATE_MG_ADD_PARTICIPANT)
	   && (pMgMsg->get_mg_msg_type() != EN_XGATE_MG_MEDIA_REQUEST_KEY_FRAME)
	   && (pMgMsg->get_mg_msg_type() != EN_XGATE_MG_MEDIA_CREATE)
	   && (pMgMsg->get_mg_msg_type() != EN_XGATE_MG_RELEASE)
	   && (pMgMsg->get_mg_msg_type() != EN_XGATE_MG_MEDIA_ADD_CLIENT_CONF_LIST)
	   && (pMgMsg->get_mg_msg_type() != EN_XGATE_MG_MEDIA_REQUEST_CREATE_SEND_CHANNEL)
	   && (pMgMsg->get_mg_msg_type() != EN_XGATE_MG_SDP_UPDATE_REQUEST)
	   && (pMgMsg->get_mg_msg_type() != EN_XGATE_MG_SDP_UPDATE_RESPONSE)
	   && (pMgMsg->get_mg_msg_type() != EN_XGATE_MG_MEDIA_REQUEST_CREATE_SEND_CHANNEL)
	   && (pMgMsg->get_mg_msg_type() != EN_XGATE_MG_RETRY_MEDIA_CONNECTION)
	   && (pMgMsg->get_mg_msg_type() != EN_XGATE_MG_VIDEO_REC_REQ)
	   && (pMgMsg->get_mg_msg_type() != EN_XGATE_MG_VIDEO_REC_RES)
	   && (pMgMsg->get_mg_msg_type() != EN_XGATE_MG_VIDEO_KEY_FRAME_REQUEST_FROM_REC_SERVER)
           && (pMgMsg->get_mg_msg_type() != EN_XGATE_MG_TILE_CHANGE_REQUEST)
           && (pMgMsg->get_mg_msg_type() != EN_XGATE_MG_SCREENSHARE_VIDEO_REQUEST)
           && (pMgMsg->get_mg_msg_type() != EN_XGATE_MG_ACTIVE_SPEAKER_VIDEO_REQUEST)
           && (pMgMsg->get_mg_msg_type() != EN_XGATE_MG_GRID_VIEW_VIDEO_REQUEST)
	   && (pMgMsg->get_mg_msg_type() != EN_XGATE_MG_ACTIVE_SPEAKER_INFO)) {
          SFUClient *sfu_client = xGateSFUtil::getSFUClient();
	  XGLOG_INFO( "SFUBridgeProcessor::Sending RESPONSE to msg_type: %d SFU client after handling message ",
			  pMgMsg->get_mg_msg_type());
          sfu_client->send_msg(pMgMsg);
        } else if(retVal == EN_XGATE_STATUS_ERROR) {
          XGLOG_ERROR( "Enter SFUBridgeProcessor::process_msg handle_channel_req failed");
        }
        break;
      }
    case EN_XGATE_MSG_MGC_MG_OUT:
      {
        //at present we don't need to support this from HmpProcessor thread.
        XGLOG_INFO( "SFUBridgeProcessor::process_msg (processing EN_XGATE_MSG_MGC_MG_OUT message)");
        break;
      }
    case EN_XGATE_MSG_UNKNOWN:
    default:
      {
        XGLOG_ERROR( "Enter SFUBridgeProcessor::process_msg is EN_XGATE_MSG_UNKNOWN");
        break;
      }      
  }
  return EN_XGATE_STATUS_SUCCESS; 

}

xGateRetVal SFUBridgeProcessor::handle_channel_req(xGateMgMsg* pMgMsg) {
  XGLOG_TRACE( "SFUBridgeProcessor::handle_channel_req start");

  SFUController *p_sfuController = xGateSFUtil::getSFUController();
  if(!p_sfuController) {
    XGLOG_ERROR( "handle_channel_req failed. Invalid HmpGstManager object !!");
    return EN_XGATE_STATUS_ERROR;
  }

  MgMediaDetail &mediaDetail = pMgMsg->get_media_detail();
  xGateNetConnection pbxConn; 
  pMgMsg->get_net_con_info(pbxConn);


  switch(pMgMsg->get_mg_msg_type()) {
   case EN_XGATE_MG_MEDIA_CREATE:
      {
        XGLOG_INFO( "handle_channel_req  for client:%s processing MG_MEDIA_CREATE",mediaDetail.call_id.c_str());
        p_sfuController->allocate_conf_channel(mediaDetail, pbxConn); //conference channel allocation
        break;
      }
    case EN_XGATE_MG_MEDIA_DELETE:
      {
        XGLOG_INFO( "handle_channel_req for client:%s processing MG_MEDIA_DELETE",mediaDetail.call_id.c_str());
        p_sfuController->media_delete_channel(mediaDetail); //deleting the client & channel
        return EN_XGATE_STATUS_NOT_SUPPORTED; //Because, we don't need to send delete response to PBX server
        break;
      }
   case EN_XGATE_MG_SDP_UPDATE_REQUEST:
     {
        XGLOG_INFO( "handle_channel_req for client:%s processing EN_XGATE_MG_SDP_UPDATE_REQUEST",mediaDetail.call_id.c_str());
        //SDP update Request
        p_sfuController->handle_sdp_update_request_msg(mediaDetail);
        break;
     }
    case EN_XGATE_MG_SDP_UPDATE_RESPONSE:
     {
        XGLOG_INFO( "handle_channel_req for client:%s processing EN_XGATE_MG_SDP_UPDATE_RESPONSE",mediaDetail.call_id.c_str());
        //SDP update Response
        //p_sfuController->handle_sdp_update_response_msg(mediaDetail);
        break;
     }
   case EN_XGATE_MG_CALL_RECORD:
      {
        XGLOG_INFO( "handle_channel_req processing CALL_RECORD");
        break;
      }
    case EN_XGATE_MG_MEDIA_REQUEST_KEY_FRAME:
      {
        p_sfuController->send_key_frame_request(mediaDetail);
        break;
      }
   case EN_XGATE_MG_RETRY_MEDIA_CONNECTION:
      {
         XGLOG_INFO( "handle_channel_req processing for client:%s EN_XGATE_MG_RETRY_MEDIA_CONNECTION ",
			 mediaDetail.call_id.c_str());
	 p_sfuController->retry_media_connection_request (mediaDetail);
         break;
      }
    case EN_XGATE_MG_TILE_CHANGE_REQUEST:
     {
        XGLOG_INFO( "handle_channel_req processing EN_XGATE_MG_TILE_CHANGE_REQUEST for user:%s",mediaDetail.call_id.c_str());
        p_sfuController->handle_tile_change_request_msg(mediaDetail);
        break;
     }
    case EN_XGATE_MG_SCREENSHARE_VIDEO_REQUEST:
     {
        XGLOG_INFO( "handle_channel_req processing EN_XGATE_MG_SCREENSHARE_VIDEO_REQUEST for user:%s",mediaDetail.call_id.c_str());
        p_sfuController->handle_screenshare_request_msg(mediaDetail);
        break;
     }
    case EN_XGATE_MG_ACTIVE_SPEAKER_VIDEO_REQUEST:
     {
        XGLOG_INFO( "handle_channel_req processing EN_XGATE_MG_ACTIVE_SPEAKER_VIDEO_REQUEST for user:%s",
                           mediaDetail.call_id.c_str());
        p_sfuController->handle_active_speaker_request_msg(mediaDetail);
        break;
     }
    case EN_XGATE_MG_GRID_VIEW_VIDEO_REQUEST:
     {
        XGLOG_INFO( "handle_channel_req processing EN_XGATE_MG_GRID_VIEW_VIDEO_REQUEST for user:%s",
                           mediaDetail.call_id.c_str());
        p_sfuController->handle_grid_view_request_msg(mediaDetail);
        break;
     }
    case EN_XGATE_MG_RELEASE:
      {
        XGLOG_INFO( "handle_channel_req for client:%s processing EN_XGATE_MG_RELEASE",
                  mediaDetail.mgresource_id.c_str());
        p_sfuController->handle_conference_release(mediaDetail); //conference channel allocation
        exit(0);
      }
#ifdef RECORD_ENABLED
    case EN_XGATE_MG_VIDEO_REC_REQ:
     {
        XGLOG_INFO( "handle_channel_req for client:%s processing EN_XGATE_MG_VIDEO_REC_REQ",mediaDetail.call_id.c_str());
        //Record Event Requests
        p_sfuController->handle_record_event_req(mediaDetail);
        break;
     }
    case EN_XGATE_MG_VIDEO_REC_RES:
     {
        XGLOG_INFO( "handle_channel_req for client:%s  processing EN_XGATE_MG_VIDEO_REC_RES",mediaDetail.call_id.c_str());
        //Record Event Requests
        p_sfuController->handle_record_event_res(mediaDetail);
        break;
     }
    case EN_XGATE_MG_VIDEO_KEY_FRAME_REQUEST_FROM_REC_SERVER:
     {
        XGLOG_INFO( "handle_channel_req for client:%s  processing EN_XGATE_MG_VIDEO_KEY_FRAME_REQUEST_FROM_REC_SERVER",
                                                                                          mediaDetail.call_id.c_str());
        p_sfuController->handle_record_keyframe_request(mediaDetail);
        break;
     }
#endif
#if 0
    case EN_XGATE_MG_ACTIVE_SPEAKER_INFO:
     {
        XGLOG_INFO( "handle_channel_req for client:%s processing EN_XGATE_MG_ACTIVE_SPEAKER_INFO",mediaDetail.call_id.c_str());
        //Active Speaker Info
        //p_sfuController->handle_active_speaker_info(mediaDetail);
        break;
     }
#endif
    case EN_XGATE_MG_TYPE_UNKNOWN:
    default:
      {
        XGLOG_ERROR( "handle_channel_req failed. Not able to process MG_TYPE_UNKNOWN request !!");
        return EN_XGATE_STATUS_NOT_SUPPORTED;
        break;
      }
  }
  XGLOG_TRACE( "SFUBridgeProcessor::handle_channel_req ended");
  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal SFUBridgeProcessor::decode_media_msg(xGateMgMsg *pMgMsg)
{
  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal SFUBridgeProcessor::post_media_detail_mbc_fwd_ctlr(xGateMgMsg *pMgMsg)
{
  XGLOG_INFO( "Enter MBCControllerProcessor::post_media_detail_mbc_fwd_ctlr(%s)", m_serverType);

  xGateMgMsg* pMgMsg1 = NULL;
  xGateNetConnection netConInfo;
  pMgMsg1 = new xGateMgMsg();

  pMgMsg1->setMsgType(EN_XGATE_MSG_MGC_MG_IN);
  pMgMsg1->set_mg_msg_type(pMgMsg->get_mg_msg_type());
  pMgMsg1->set_media_detail(pMgMsg->get_media_detail());
  pMgMsg->get_net_con_info(netConInfo);
  pMgMsg1->set_net_con_info(netConInfo);

  ACE_Task<ACE_MT_SYNCH>* taskPtr = NULL;
  // IMPORTANT NOTE: Sending message to 
  taskPtr = xGateSFUtil::getMbcSfuCtl();
  if(!taskPtr) {
    XGLOG_ERROR( "SFUBridgeProcessor::post_media_detail_mbc_fwd_ctlr SFUBridgeProcessor task not found");
    return EN_XGATE_STATUS_ERROR;
  }

  ACE_Message_Block* pAmb = NULL;
  pAmb = static_cast<xGateMgMsg*> (pMgMsg1);
  ACE_Time_Value tval ((time(NULL) ));
  tval += ACE_Time_Value(0,1);

  if (taskPtr->putq(pAmb, &tval)) {
    XGLOG_INFO( "MBCControllerProcessor::post_media_detail_mbc_fwd_ctlr SFUBridgeProcessor enqueue message success");
  } else {
    XGLOG_ERROR( "MBCControllerProcessor::post_media_detail_mbc_fwd_ctlr SFUBridgeProcessor enqueue message failed");
    delete pMgMsg1;
    return EN_XGATE_STATUS_ERROR;
  }
  return EN_XGATE_STATUS_SUCCESS;
}

