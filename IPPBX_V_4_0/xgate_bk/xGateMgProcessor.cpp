
//local includes
#include "log.h"
#include "klog.h"
#include "xGateMgProcessor.h"
#include "xGateMGDispatcher.h"
#include "xGateHmpProcessor.h"

#define THISMODULE "MgProc"


xGateMgProcessor::xGateMgProcessor(const char *serverType):xGateProcessor(serverType)
{
  CLog::Detail(THISMODULE, "xGateMgProcessor::xGateMgProcessor");
}

xGateMgProcessor::~xGateMgProcessor(void)
{
  CLog::Detail(THISMODULE, "xGateMgProcessor::~xGateMgProcessor");
}

xGateRetVal xGateMgProcessor::process_msg(xGateBaseMsg* pMsg)
{
  CLog::Detail(THISMODULE, "Enter xGateMgProcessor::process_msg(%s)", m_serverType); 
  switch(pMsg->getMsgType()) {
    case EN_XGATE_MSG_MGC_MG_IN: //we got msg from MG server
      {
        CLog::Detail(THISMODULE, "Enter process_msg processing EN_XGATE_MSG_MGC_MG_IN message"); 
        xGateMgMsg *pMgMsg = dynamic_cast<xGateMgMsg*>(pMsg);
        //decode and validate mg related message
        if(decode_media_msg(pMgMsg) != EN_XGATE_STATUS_SUCCESS) {
          CLog::Error(THISMODULE, "xGateMgProcessor::process_msg data mg_msg decode or validation failed");
        }
        break;
      }
    case EN_XGATE_MSG_MGC_MG_OUT:
      {
        CLog::Detail(THISMODULE, "Note: MG Dispatcher thread directly handle EN_XGATE_MSG_MGC_MG_OUT");
        break;
      }
    case EN_XGATE_MSG_UNKNOWN:
    default:
      {
        CLog::Error(THISMODULE, "Enter xGateMgProcessor::process_msg is EN_XGATE_MSG_UNKNOWN");
        break;
      }      
  }
  return EN_XGATE_STATUS_SUCCESS; 
}

xGateRetVal xGateMgProcessor::decode_media_msg(xGateMgMsg *pMgMsg)
{
  CLog::Detail(THISMODULE, "Enter xGateMgProcessor::decode_media_msg(%s)", m_serverType); 

  const char  *data = 0;
  unsigned int dataLen = 0;

  pMgMsg->get_data(data);
  dataLen = pMgMsg->get_data_len();

  if(!data && dataLen <=0) {
    CLog::Error(THISMODULE, "xGateMgProcessor::decode_media_msg invalid data in xGateMgMsg.");
    return EN_XGATE_STATUS_ERROR;
  }

  CLog::Detail(THISMODULE, "xGateMgProcessor::decode_media_msg data: %s, len: %d", data, dataLen);

  Document doc;
  MgMediaDetail clientMediaDetail;
  MgPlayBackDetail  playBackDetail;
  xGateMgMsgType mgMsgType; 
  Value dataMemObj;

  if(create_doc_object(doc, data, dataMemObj) != EN_XGATE_STATUS_SUCCESS) {
    CLog::Detail(THISMODULE, "xGateMgProcessor::decode_media_msg DOC object creation failed");
    return EN_XGATE_STATUS_ERROR;
  }

  mgMsgType = doc["msg_type"].GetInt();

  switch(mgMsgType) {
    case EN_XGATE_MG_PLAY_BACK:
      {
        CLog::Detail(THISMODULE, "decode_media_msg decoding EN_XGATE_MG_PLAY_BACK");
        handle_playback_req(dataMemObj, playBackDetail); //TODO: Yoga we can move this area to common place. why replicate this
	pMgMsg->set_playback_detail(playBackDetail);
	break;
      }
    case EN_XGATE_MG_ALLOCATE_ORGINATE:
      {
        CLog::Detail(THISMODULE, "decode_mg_msg decoding EN_XGATE_MG_ALLOCATE_ORGINATE");  
        handle_media_req(dataMemObj, clientMediaDetail);
	pMgMsg->set_media_detail(clientMediaDetail);
	break;
      }
    case EN_XGATE_MG_18X_RESPONSE:
      {
        CLog::Detail(THISMODULE, "decode_mg_msg decoding EN_XGATE_MG_18X_RESPONSE");
        handle_media_req(dataMemObj, clientMediaDetail);
        pMgMsg->set_media_detail(clientMediaDetail);
        break;
      }
    case EN_XGATE_MG_ALLOCATE_TERMINATE:
      {
        CLog::Detail(THISMODULE, "decode_mg_msg decoding EN_XGATE_MG_ALLOCATE_TERMINATE");  
        handle_media_req(dataMemObj, clientMediaDetail);
	pMgMsg->set_media_detail(clientMediaDetail);
	break;
      }
    case EN_XGATE_MG_RELEASE:
      {
        CLog::Detail(THISMODULE, "decode_mg_msg decoding EN_XGATE_MG_RELEASE");
        handle_media_req(dataMemObj, clientMediaDetail);
	pMgMsg->set_media_detail(clientMediaDetail);
	break;
      }
    case EN_XGATE_MSG_UNKNOWN:
    default:
      {
        CLog::Error(THISMODULE, "decode_mg_msg unable to decode unknow message");
        return EN_XGATE_STATUS_ERROR;
        break;
      }
  }    

  pMgMsg->set_mg_msg_type(mgMsgType);
  post_media_detail(pMgMsg);

  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateMgProcessor::create_doc_object(Document &doc, const char *jsonData, Value& dataMemObj)
{
  CLog::Detail(THISMODULE, "Enter xGateMgProcessor::create_doc_object(%s)", m_serverType); 
  if (doc.ParseInsitu(jsonData).HasParseError()) {
    CLog::Error(THISMODULE, "xGateMgProcessor::create_doc_object json parse error");
    return EN_XGATE_STATUS_ERROR;
  } else if(!doc.IsObject()) {
    CLog::Error(THISMODULE, "xGateMgProcessor::create_doc_object not able to created document object");
    return EN_XGATE_STATUS_ERROR;
  } else if(!doc.HasMember("msg_type")) { 
    CLog::Error(THISMODULE, "xGateMgProcessor::create_doc_object msg_type field is missing in json msg");
    return EN_XGATE_STATUS_ERROR;
  } else if(!doc.HasMember("data")) {
    CLog::Error(THISMODULE, "xGateSrvrProcessor::create_doc_object data member not found in json msg");
    return EN_XGATE_STATUS_ERROR;
  }
  dataMemObj = doc["data"];
  CLog::Detail(THISMODULE, "xGateMgProcessor::create_doc_object successfully created DOC object !!");
  return EN_XGATE_STATUS_SUCCESS;
}


xGateRetVal xGateMgProcessor::handle_playback_req(Value &dataMemObj, MgPlayBackDetail& playBackDetail)
{
  CLog::Detail(THISMODULE, "Enter xGateMgProcessor::handle_playback_req");
  Value::ConstMemberIterator itr = dataMemObj.MemberBegin();
  Value::ConstMemberIterator itrEnd = dataMemObj.MemberEnd();
  for (itr; itr != itrEnd; ++itr) {
    if(!strcmp(itr->name.GetString(), "call_id")) {
      playBackDetail.call_id = itr->value.GetString();
    } else if(!strcmp(itr->name.GetString(), "rbtstatus")) {
      playBackDetail.rbt_status = itr->value.GetInt();
    }
  }
  CLog::Detail(THISMODULE, "xGateMgProcessor::handle_playback_req successfully handled");
  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateMgProcessor::handle_media_req(Value &dataMemObj, MgMediaDetail& clientMediaDetail) 
{
  CLog::Detail(THISMODULE, "Enter xGateMgProcessor::handle_media_alloc_req(%s)", m_serverType); 
  Value::ConstMemberIterator itr = dataMemObj.MemberBegin();
  Value::ConstMemberIterator itrEnd = dataMemObj.MemberEnd();

  for (itr; itr != itrEnd; ++itr) {
    if(!strcmp(itr->name.GetString(), "call_id")) {
      clientMediaDetail.call_id = itr->value.GetString();
    } else if(!strcmp(itr->name.GetString(), "context_id")) {
      clientMediaDetail.context_id = itr->value.GetInt();  
/*
    } else if(!strcmp(itr->name.GetString(), "ip_addr")) {
      clientMediaDetail.ip_addr = itr->value.GetString(); 
    } else if (!strcmp(itr->name.GetString(), "ip_port")) {
      clientMediaDetail.port = itr->value.GetInt(); 
    } else if(!strcmp(itr->name.GetString(), "relayip_addr")) {
      clientMediaDetail.relayip_addr = itr->value.GetString();
    } else if (!strcmp(itr->name.GetString(), "relay_port")) {
      clientMediaDetail.relay_port = itr->value.GetInt();
    } else if(!strcmp(itr->name.GetString(), "reflexip_addr")) {
      clientMediaDetail.reflexip_addr = itr->value.GetString();
    } else if (!strcmp(itr->name.GetString(), "reflex_port")) {
      clientMediaDetail.reflex_port = itr->value.GetInt();
    } else if(!strcmp(itr->name.GetString(), "codec")) {
      clientMediaDetail.codec = itr->value.GetInt(); 
    } else if(!strcmp(itr->name.GetString(), "ice_ufrag")) {
      clientMediaDetail.ice_ufrag = itr->value.GetString();
    } else if(!strcmp(itr->name.GetString(), "ice_pwd")) {
      clientMediaDetail.ice_pwd = itr->value.GetString();
    } else if(!strcmp(itr->name.GetString(), "fingerprint")) {
      clientMediaDetail.fingerprint = itr->value.GetString();
    } else if(!strcmp(itr->name.GetString(), "ptime")) {
      clientMediaDetail.ptime = itr->value.GetInt();
*/
    } else if(!strcmp(itr->name.GetString(), "media_proto")) {
      clientMediaDetail.media_proto = itr->value.GetString(); 
    } else if(!strcmp(itr->name.GetString(), "out_proto")) {
      clientMediaDetail.out_proto = itr->value.GetString(); 
    } else if(!strcmp(itr->name.GetString(), "sig_type")) {
      clientMediaDetail.sig_type = itr->value.GetInt(); 
    } else if(!strcmp(itr->name.GetString(), "call_dir")) {
      clientMediaDetail.call_dir = itr->value.GetInt(); 
    } else if(!strcmp(itr->name.GetString(), "rtp_endpoint")) {
      clientMediaDetail.rtp_ep = itr->value.GetString(); 
    } 
  }
  
  //debugging purpose only
  /*cout << "handle_media_alloc_req json parsed values" << endl;
  cout << "call_id: " << clientMediaDetail.call_id << " context_id: " << clientMediaDetail.context_id \
    << " ip_addr: " << clientMediaDetail.ip_addr << " port: " << clientMediaDetail.port \
    << " codec: " << clientMediaDetail.codec << " sig_type: " << clientMediaDetail.sig_type \
    << " call_dir: " << clientMediaDetail.call_dir << " rtp_endpoint: " << clientMediaDetail.rtp_ep << endl;*/
  CLog::Detail(THISMODULE, "xGateMgProcessor::handle_media_alloc_req successfully handled");
  return EN_XGATE_STATUS_SUCCESS;
}

/*TODO: this will post media detail to HmpChannelHandler thread */
xGateRetVal xGateMgProcessor::post_media_detail(xGateMgMsg *pMgMsg)
{
  CLog::Detail(THISMODULE, "Enter xGateMgProcessor::post_media_detail(%s)", m_serverType);
  xGateMgMsgType msgType = pMgMsg->get_mg_msg_type();
  xGateMgMsg* pMgMsg1 = NULL;
  xGateNetConnection netConInfo;
  pMgMsg1 = new xGateMgMsg();

  pMgMsg1->setMsgType(EN_XGATE_MSG_MGC_MG_IN);
  //printf("xGateMgProcessor::post_media_detail (posting msg: %d)", pMgMsg->get_mg_msg_type());
  pMgMsg1->set_mg_msg_type(pMgMsg->get_mg_msg_type());
  if(msgType == EN_XGATE_MG_PLAY_BACK) {
    pMgMsg1->set_playback_detail(pMgMsg->get_playback_detail());
  } else {
    pMgMsg1->set_media_detail(pMgMsg->get_media_detail());
  }
  pMgMsg->get_net_con_info(netConInfo);
  pMgMsg1->set_net_con_info(netConInfo);

  ACE_Task<ACE_MT_SYNCH>* taskPtr = NULL;
  taskPtr = xGateUtil::getHmpProcessor();
  if(!taskPtr) {
    CLog::Error(THISMODULE, "xGateMgProcessor::post_media_detail HmpProcessor task not found");
    return EN_XGATE_STATUS_ERROR;
  }

  ACE_Message_Block* pAmb = NULL;
  pAmb = static_cast<xGateMgMsg*> (pMgMsg1);
  ACE_Time_Value tval ((time(NULL) ));
  tval += ACE_Time_Value(0,1);

  if (taskPtr->putq(pAmb, &tval)) {
    CLog::Detail(THISMODULE, "xGateMgProcessor::post_media_detail HmpProcessor enqueue message success");
  } else {
    CLog::Error(THISMODULE, "xGateMgProcessor::post_media_detail HmpProcessor enqueue message failed");
    delete pMgMsg1;
    return EN_XGATE_STATUS_ERROR;
  }

  return EN_XGATE_STATUS_SUCCESS;
}

