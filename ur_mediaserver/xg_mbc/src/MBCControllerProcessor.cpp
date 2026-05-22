

//local includes
//#include "MBCControllerProcessor.h"
//#include "xGateTCPSocketHandler.h"
#include "ClientService.h"
#include "xGateHmpGstUtil.h"
#include "xGateMBCUtil.h"
#include "xGateUtil.h"
#include "xglog.h"
#define THISMODULE "MBCctrl"

extern ClientService *pClntService;

MBCControllerProcessor::MBCControllerProcessor(const char *serverType):xGateProcessor(serverType)
{
  XGLOG_INFO( "MBCControllerProcessor::MBCControllerProcessor");
}
MBCControllerProcessor::~MBCControllerProcessor(void)
{
  XGLOG_INFO( "MBCControllerProcessor::~MBCControllerProcessor");
}

xGateRetVal MBCControllerProcessor::process_msg(xGateBaseMsg* pMsg)
{
  XGLOG_INFO( "Enter MBCControllerProcessor::process_msg(%s)", m_serverType);
  xGateMBCServiceMsg *ptmediaBCastMsg = dynamic_cast<xGateMBCServiceMsg*>(pMsg);

  if(decode_media_msg(ptmediaBCastMsg) != EN_XGATE_STATUS_SUCCESS) {
    XGLOG_ERROR( "MBCControllerProcessor::process_msg data mg_msg decode or validation failed");
  }

  if(ptmediaBCastMsg) {
    delete ptmediaBCastMsg;
    ptmediaBCastMsg = NULL;
  }

  return EN_XGATE_STATUS_SUCCESS;
}
xGateRetVal MBCControllerProcessor::decode_media_msg(xGateMBCServiceMsg *ptmediaBCastMsg)
{
  XGLOG_INFO( "Enter MBCControllerProcessor::decode_media_msg(%s)", m_serverType); 

  const char  *data = 0;
  unsigned int dataLen = 0;

  data = ptmediaBCastMsg->getMsg();
  dataLen = ptmediaBCastMsg->getMsgLen();

  if(!data && dataLen <=0) {
    XGLOG_ERROR( "MBCControllerProcessor::decode_media_msg invalid data in xGateMgMsg.");
    return EN_XGATE_STATUS_ERROR;
  }

  XGLOG_INFO( "MBCControllerProcessor::extracted data ptr %p , len: %d", data, dataLen);
  XGLOG_INFO( "MBCControllerProcessor::decode_media_msg data: [%s], len: %d", data, dataLen);
  Document doc;
  MgMediaDetail clientMediaDetail;
  xGateMgMsgType mgMsgType; 
  Value dataMemObj;

  if(create_doc_object(doc, data, dataMemObj) != EN_XGATE_STATUS_SUCCESS) {
    XGLOG_INFO( "MBCControllerProcessor::decode_media_msg DOC object creation failed");
    return EN_XGATE_STATUS_ERROR;
  }

  mgMsgType = (xGateMgMsgType)doc["msg_type"].GetInt();
  switch(mgMsgType) {
    case EN_XGATE_MG_MEDIA_CREATE:
      {
        XGLOG_INFO( "decode_media_msg decoding EN_XGATE_MG_MEDIA_CREATE");
        xGateMBCUtil::handle_media_req(dataMemObj, clientMediaDetail);
        break;
      }
    case EN_XGATE_MG_ALLOCATE_ORGINATE:
      {
        XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_ALLOCATE_ORGINATE");  
        xGateMBCUtil::handle_media_req(dataMemObj, clientMediaDetail);
        break;
      }
    case EN_XGATE_MG_ALLOCATE_TERMINATE:
      {
        XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_ALLOCATE_TERMINATE");  
        xGateMBCUtil::handle_media_req(dataMemObj, clientMediaDetail);

        break;
      }
    case EN_XGATE_MG_MEDIA_DELETE:
      {
        XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_MEDIA_DELETE");
        xGateMBCUtil::handle_media_req(dataMemObj, clientMediaDetail);
        break;
      }
    case EN_XGATE_MG_RELEASE:
      {
        XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_RELEASE");
        xGateMBCUtil::handle_media_req(dataMemObj, clientMediaDetail);
        break;
      }
    case EN_XGATE_MG_18X_RESPONSE:
      {
        XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_18X_RESPONSE");
        xGateMBCUtil::handle_media_req(dataMemObj, clientMediaDetail);
        break;
      }
    case EN_XGATE_MG_PLAY_BACK:
      {
        XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_PLAY_BACK");
        xGateMBCUtil::handle_media_req(dataMemObj, clientMediaDetail);
        break;
      }
    case EN_XGATE_MG_HOLD_RESUME:
      {
        XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_HOLD_RESUME");
        xGateMBCUtil::handle_media_req(dataMemObj, clientMediaDetail);
        break;
      }
    case EN_XGATE_MG_DTMF_DIGITS:
      {
        XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_DTMF_DIGITS");
        xGateMBCUtil::handle_media_req(dataMemObj, clientMediaDetail);
        break;
      }
    case EN_XGATE_MG_CALL_RECORD:
      {
        XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_CALL_RECORD");
        xGateMBCUtil::handle_media_req(dataMemObj, clientMediaDetail);
        break;
      }
    case EN_XGATE_MG_SWITCH_CALLTO_CONFERENCE:
      {
        XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_SWITCH_CALLTO_CONFERENCE");
        xGateMBCUtil::handle_media_req(dataMemObj, clientMediaDetail);
        break;
      }    
    case EN_XGATE_MG_ADD_PARTICIPANT:
      {
        XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_ADD_PARTICIPANT");
        xGateMBCUtil::handle_media_req(dataMemObj, clientMediaDetail);
        break;
      }
    case EN_XGATE_MG_MEDIA_ALLOCATE_121_CLIENT1:
      {
        XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_MEDIA_ALLOCATE_121_CLIENT1");
        xGateMBCUtil::handle_media_req(dataMemObj, clientMediaDetail);
        break;
      }
    case EN_XGATE_MG_MEDIA_ALLOCATE_121_CLIENT2:
      {
        XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_MEDIA_ALLOCATE_121_CLIENT2");
        xGateMBCUtil::handle_media_req(dataMemObj, clientMediaDetail);
        break;
      }
    case EN_XGATE_MG_MEDIA_121_CLIENT1_TERMINATE:
      {
        XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_MEDIA_121_CLIENT1_TERMINATE");
        xGateMBCUtil::handle_media_req(dataMemObj, clientMediaDetail);
        break;
      }
    case EN_XGATE_MG_MEDIA_121_CLIENT2_TERMINATE:
      {
        XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_MEDIA_121_CLIENT2_TERMINATE");
        xGateMBCUtil::handle_media_req(dataMemObj, clientMediaDetail);
        break;
      }
    case EN_XGATE_MG_MEDIA_REQUEST_KEY_FRAME:
      {
        XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_MEDIA_REQUEST_KEY_FRAME");
        xGateMBCUtil::handle_media_req(dataMemObj, clientMediaDetail);
        break;
      }
    case EN_XGATE_MG_MEDIA_ADD_CLIENT_CONF_LIST:
      {
        XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_MEDIA_ADD_CLIENT_CONF_LIST");
        xGateMBCUtil::handle_media_req(dataMemObj, clientMediaDetail);
        break;
      }
    case EN_XGATE_MG_MEDIA_REQUEST_CREATE_SEND_CHANNEL:
      {
        XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_MEDIA_REQUEST_CREATE_SEND_CHANNEL");
        xGateMBCUtil::handle_media_req(dataMemObj, clientMediaDetail);
        break;
      }
      break;
    case EN_XGATE_MG_RETRY_MEDIA_CONNECTION:
      {
        XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_RETRY_MEDIA_CONNECTION");
        xGateMBCUtil::handle_media_req(dataMemObj, clientMediaDetail);
      }
      break;
    case EN_XGATE_MG_SDP_UPDATE_REQUEST:
      {
        XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_SDP_UPDATE_REQUEST");
        xGateMBCUtil::handle_media_req(dataMemObj, clientMediaDetail);
      }
      break;
     case EN_XGATE_MG_SDP_UPDATE_RESPONSE:
      {
        XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_SDP_UPDATE_RESPONSE");
        xGateMBCUtil::handle_media_req(dataMemObj, clientMediaDetail);
      }
      break;
    case EN_XGATE_MSG_UNKNOWN:
    default:
      {
        XGLOG_ERROR( "decode_mg_msg unable to decode EN_XGATE_MSG_UNKNOWN message");
        return EN_XGATE_STATUS_ERROR;
        break;
      }
  }
  xGateRetVal retVal;
  retVal = send_media_msg_to_sfu(ptmediaBCastMsg,clientMediaDetail,mgMsgType);
  return retVal;
}

xGateRetVal MBCControllerProcessor::send_media_msg_to_sfu(xGateMBCServiceMsg *ptmediaBCastMsg,\
		                               MgMediaDetail& clientMediaDetail,xGateMgMsgType mgMsgType)
{
  XGLOG_INFO( "MBCControllerProcessor::send_media_msg_to_sfu for msg_type(%d)", mgMsgType);
  string jsonBuff("");
  xGateRetVal retVal = EN_XGATE_STATUS_ERROR;
  int len = ptmediaBCastMsg->getMsgLen();
  char* msg = (char*)ptmediaBCastMsg->getMsg();
  char* data = new char[len+4];
  memcpy(data,(char*)ptmediaBCastMsg->getMsg(),len-1);
  sprintf(&data[len-1],"\r\n\r\n");
  ptmediaBCastMsg->setMsg(data);
  ptmediaBCastMsg->setMsgLen(len+3);
  retVal = post_media_detail(ptmediaBCastMsg,mgMsgType,clientMediaDetail);
  XGLOG_INFO ("send_media_msg_to_sfu deleting data ptr: %p len: %d newly allocated data ptr: %p", msg, len, data);
  delete msg;
  return retVal;
}

xGateRetVal MBCControllerProcessor::post_media_detail(xGateMBCServiceMsg *ptmediaBCastMsg,\
    xGateMgMsgType mgMsgType,MgMediaDetail& clientMediaDetail)
{
  XGLOG_INFO( "MBCControllerProcessor::post_media_detail(%s)", m_serverType);

  xGateMBCServiceMsg* ptmediaBCastMsg1 = new xGateMBCServiceMsg();
  int sfuId = 0;

  ptmediaBCastMsg1->setMsgType(EN_XGATE_MSG_XG_MBC_MODULE);
  ptmediaBCastMsg1->set_mbc_event_type(EN_XG_MBC_EVENT_SEND);
  ptmediaBCastMsg1->setSrcModuleId(IURDefines::UR_MODULE_XG_MBC_SERVICE);
  ptmediaBCastMsg1->setDstModuleId(IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE);
  ptmediaBCastMsg1->setMsg(ptmediaBCastMsg->getMsg());
  ptmediaBCastMsg1->setMsgLen(ptmediaBCastMsg->getMsgLen());

  if((mgMsgType == EN_XGATE_MG_ALLOCATE_TERMINATE)
	  || (mgMsgType == EN_XGATE_MG_MEDIA_CREATE))
  {
    bool isEmptySlotFound = FALSE;
    sfuId = xGateMBCUtil::getsfuIdFromResIdMap(clientMediaDetail.mgresource_id);
    if (sfuId == EN_XGATE_STATUS_NOT_PRESENT){
       isEmptySlotFound  = xGateMBCUtil::getSfuId(sfuId);
      XGLOG_INFO( "MBCControllerProcessor:: isEmptySlotFound: %d sfuId: %d ",isEmptySlotFound, sfuId);
    }
    if( isEmptySlotFound == TRUE)
    {
      XGLOG_INFO( "MBCControllerProcessor:: Creating SFU thread with id: %d as empty slot found == TRUE",
		       sfuId);
      createSfu(sfuId);
    }
    int sfuConnId = -1;
    xGateMBCUtil::get_sfuConnId_from_sfuId(sfuId,sfuConnId);
    if (sfuConnId < 0){
         XGLOG_INFO ("sfuConnId not yet received. calling insert_sfuId_alloc_msg_map to store");
      //get mg resource Id
      char* data = new char[ptmediaBCastMsg->getMsgLen()];
      memcpy(data,ptmediaBCastMsg->getMsg(),ptmediaBCastMsg->getMsgLen());
      xGateMBCUtil::insert_sfuId_alloc_msg_map(sfuId,ptmediaBCastMsg->getMsgLen(),data);
      xGateMBCUtil::insert_sfuId_pbxConnId_map(sfuId,ptmediaBCastMsg->getConnectionId());
      xGateMBCUtil::addToMgSfuResMap(clientMediaDetail.mgresource_id,sfuId);
      xGateMBCUtil::addToMgResSfuMap(sfuId,clientMediaDetail.mgresource_id);
      char* msg = ptmediaBCastMsg->getMsg();
        XGLOG_INFO ("Deleting data data ptr: %p ", msg);
      delete msg;
      delete ptmediaBCastMsg1;
      return EN_XGATE_STATUS_SUCCESS;
    }
  }
  else
  {
    sfuId = xGateMBCUtil::getsfuIdFromResIdMap(clientMediaDetail.mgresource_id);
  }
  if(sfuId < 0){
    XGLOG_ERROR( "MBCControllerProcessor::post_media_detail SfuId not found for resource id: %s",clientMediaDetail.mgresource_id.c_str());
    delete ptmediaBCastMsg1;
    return EN_XGATE_STATUS_ERROR;
  }
  int sfuConnId = 0;
  XGLOG_INFO("MBCControllerProcessor::post_media_detail Message sending to SFU");
  xGateMBCUtil::get_sfuConnId_from_sfuId(sfuId,sfuConnId);
  ptmediaBCastMsg1->setConnectionId(sfuConnId);
  XGMBCSERVICE->getConfig()->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(ptmediaBCastMsg1)); 

  if(mgMsgType == EN_XGATE_MG_RELEASE){
		xGateMBCUtil::removeResIdFromSfuIdMap(sfuId);	
    xGateMBCUtil::removeSfuIdFromSfuConnIdMap(sfuId);
		xGateMBCUtil::removeSfuIdFromResIdMap(sfuId);
  }
  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal MBCControllerProcessor::create_doc_object(Document &doc, const char *jsonData, Value& dataMemObj)
{
  XGLOG_INFO( "Enter MBCControllerProcessor::create_doc_object(%s)", m_serverType); 
  doc.Parse(jsonData);
  if(doc.HasParseError()){
    XGLOG_ERROR( "MBCControllerProcessor::create_doc_object json parse error");
    return EN_XGATE_STATUS_ERROR;
  } else if(!doc.IsObject()) {
    XGLOG_ERROR( "MBCControllerProcessor::create_doc_object not able to created document object");
    return EN_XGATE_STATUS_ERROR;
  } else if(!doc.HasMember("msg_type")) { 
    XGLOG_ERROR( "MBCControllerProcessor::create_doc_object msg_type field is missing in json msg");
    return EN_XGATE_STATUS_ERROR;
  } else if(!doc.HasMember("data")) {
    XGLOG_ERROR( "MBCControllerProcessor::create_doc_object data member not found in json msg");
    return EN_XGATE_STATUS_ERROR;
  }
  dataMemObj = doc["data"];
  XGLOG_INFO( "MBCControllerProcessor::create_doc_object successfully created DOC object !!");
  return EN_XGATE_STATUS_SUCCESS;
}
