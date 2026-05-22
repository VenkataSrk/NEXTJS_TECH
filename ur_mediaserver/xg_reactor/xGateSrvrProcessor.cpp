
//local includes
#include "log.h"
#include "klog.h"
#include "xGateSrvrProcessor.h"
#include "xGateMgcMgMedia.h"
#include "xGateMGDispatcher.h"
#include "xGateSipSignalProcessor.h"

#define THISMODULE "SrvrProc"

xGateSrvrProcessor::xGateSrvrProcessor(const char *serverType):xGateProcessor(serverType)
{
  CLog::Warning(THISMODULE, "xGateSrvrProcessor::xGateSrvrProcessor");
}

xGateSrvrProcessor::~xGateSrvrProcessor(void)
{
  CLog::Warning(THISMODULE, "xGateSrvrProcessor::~xGateSrvrProcessor");
}

xGateRetVal xGateSrvrProcessor::process_msg(xGateBaseMsg* pMsg)
{
  CLog::Detail(THISMODULE, "Enter xGateSrvrProcessor::process_msg(%s)", m_serverType); 
  const char *data;
  unsigned int dataLen;

  switch(pMsg->getMsgType()) {
    case EN_XGATE_MSG_MGC_MG_IN: //we got msg from MG server
      {
        CLog::Detail(THISMODULE, "Enter process_msg processing EN_XGATE_MSG_MGC_MG_IN message"); 
        xGateMgMsg *pMgMsg = dynamic_cast<xGateMgMsg*>(pMsg);
        //decode and validate mg related message
        if(decode_media_msg(pMgMsg) != EN_XGATE_STATUS_SUCCESS) {
          CLog::Error(THISMODULE, "xGateSrvrProcessor::process_msg data mg_msg decode or validation failed");
        }

        break;
      }
    case EN_XGATE_MSG_MGC_MG_OUT:
      {
        CLog::Warning(THISMODULE, "Note: MG Dispatcher thread directly handle EN_XGATE_MSG_MGC_MG_OUT");
        break;
      }
    case EN_XGATE_MSG_UNKNOWN:
    default:
      {
        CLog::Error(THISMODULE, "Enter xGateSrvrProcessor::process_msg is EN_XGATE_MSG_UNKNOWN");
        break;
      }      
  }
  return EN_XGATE_STATUS_SUCCESS;  
}

xGateRetVal xGateSrvrProcessor::dispatch_msg(xGateBaseMsg* pMsg)
{
  CLog::Warning(THISMODULE, "Enter xGateSrvrProcessor::dispatch_msg(%s)", m_serverType); 
  return EN_XGATE_STATUS_SUCCESS;  
}

//This function handle response message from MG server in json formate
xGateRetVal xGateSrvrProcessor::decode_media_msg(xGateMgMsg *pMgMsg)
{
  CLog::Detail(THISMODULE, "Enter xGateSrvrProcessor::decode_media_msg(%s)", m_serverType); 
  const char  *data = 0;
  unsigned int dataLen = 0;

  pMgMsg->get_data(data);
  dataLen = pMgMsg->get_data_len();

  if(!data && dataLen <=0) {
    CLog::Error(THISMODULE, "xGateSrvrProcessor::decode_media_msg invalid data in xGateMgMsg.");
    return EN_XGATE_STATUS_ERROR;
  }
  CLog::Error(THISMODULE, "xGateSrvrProcessor::decode_media_msg data: %s, len: %d", data, dataLen);

  Document doc;
  MgMediaDetail mediaDetail;
  MgRegistrationDetail gatewayRegDetail;
  xGateMgMsgType mgMsgType;
  Value dataMemObj;

  if(create_doc_object(doc, data, dataMemObj) != EN_XGATE_STATUS_SUCCESS) {
    CLog::Detail(THISMODULE, "xGateSrvrProcessor::decode_media_msg DOC object creation failed");
    return EN_XGATE_STATUS_ERROR;
  }
  mgMsgType = doc["msg_type"].GetInt();
pMgMsg->set_mg_msg_type(mgMsgType);
  switch(mgMsgType) {
    case EN_XGATE_MG_REGISTER:
      {
        CLog::Detail(THISMODULE, "decode_media_msg decoding EN_XGATE_MG_REGISTER");
        handle_gateway_reg(dataMemObj, gatewayRegDetail); 
        break;
      }
    case EN_XGATE_MG_ALLOCATE_ORGINATE:
    case EN_XGATE_MG_ALLOCATE_TERMINATE:
    case EN_XGATE_MG_ADD_PARTICIPANT:
    case EN_XGATE_MG_PLAY_BACK:
    case EN_XGATE_MG_DTMF_DIGITS:
    case EN_XGATE_MG_MEDIA_CREATE:
    case EN_XGATE_MG_SDP_UPDATE_RESPONSE:
    case EN_XGATE_MG_ACTIVE_SPEAKER_INFO:
      {
        CLog::Detail(THISMODULE, "decode_media_msg decoding %d response",mgMsgType);  
        handle_media_resp(dataMemObj, mediaDetail);
        if(post_media_detail(pMgMsg, mediaDetail) != EN_XGATE_STATUS_SUCCESS) {
          return EN_XGATE_STATUS_ERROR;
        }
        break;
      }
    case  EN_XGATE_MG_18X_RESPONSE:
    {
     CLog::Detail(THISMODULE, "decode_media_msg decoding EN_XGATE_MG_18X_RESPONSE response");
     handle_media_resp(dataMemObj, mediaDetail);
     if(post_media_detail(pMgMsg, mediaDetail) != EN_XGATE_STATUS_SUCCESS) {
       return EN_XGATE_STATUS_ERROR;
     }
    }
    break;
    case EN_XGATE_MSG_UNKNOWN:
    default:
      {
        CLog::Error(THISMODULE, "decode_media_msg unable to decode UNKNOWN message");
        return EN_XGATE_STATUS_ERROR;
        break;
      }
  }
  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateSrvrProcessor::create_doc_object(Document &doc, const char *jsonData, Value& dataMemObj)
{
  CLog::Detail(THISMODULE, "Enter xGateSrvrProcessor::create_doc_object(%s)", m_serverType);
  if (doc.ParseInsitu(jsonData).HasParseError()) {
    CLog::Error(THISMODULE, "xGateSrvrProcessor::create_doc_object json parse error");
    return EN_XGATE_STATUS_ERROR;
  } else if(!doc.IsObject()) {
    CLog::Error(THISMODULE, "xGateSrvrProcessor::create_doc_object not able to created document object");
    return EN_XGATE_STATUS_ERROR;
  } else if(!doc.HasMember("msg_type")) {
    CLog::Error(THISMODULE, "xGateSrvrProcessor::create_doc_object msg_type field is missing in json msg");
    return EN_XGATE_STATUS_ERROR;
  } else if(!doc.HasMember("data")) {
    CLog::Error(THISMODULE, "xGateSrvrProcessor::create_doc_object data member not found in json msg");
    return EN_XGATE_STATUS_ERROR;
  }
  dataMemObj = doc["data"];
  CLog::Detail(THISMODULE, "xGateSrvrProcessor::create_doc_object successfully created DOC object !!");
  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateSrvrProcessor::handle_media_resp(Value& dataMemObj, MgMediaDetail& serverMediaDetail)
{
  CLog::Detail(THISMODULE, "Enter xGateSrvrProcessor::handle_media_resp(%s)", m_serverType);
    Value::ConstMemberIterator itr = dataMemObj.MemberBegin();
    Value::ConstMemberIterator itrEnd = dataMemObj.MemberEnd();

    for (itr; itr != itrEnd; ++itr) {
      if(!strcmp(itr->name.GetString(), "call_id")) {
        serverMediaDetail.call_id = itr->value.GetString();
      } else if(!strcmp(itr->name.GetString(), "context_id")) {
        serverMediaDetail.context_id = itr->value.GetInt();
      } else if(!strcmp(itr->name.GetString(), "gateway_id")) {
        serverMediaDetail.gateway_id = itr->value.GetInt();
      } else if(!strcmp(itr->name.GetString(),"media_state")){
        serverMediaDetail.pb_state = itr->value.GetInt();
      } else if(!strcmp(itr->name.GetString(),"leg_id")){
        serverMediaDetail.leg_id = itr->value.GetInt();
      } else if(!strcmp(itr->name.GetString(),"mgresource_id")){
        serverMediaDetail.mediaResource_id = itr->value.GetString();	
      } else if(!strcmp(itr->name.GetString(), "sig_type")) {
        serverMediaDetail.sig_type = itr->value.GetInt();
      } else if(!strcmp(itr->name.GetString(), "call_dir")) {
        serverMediaDetail.call_dir = itr->value.GetInt();
      } else if(!strcmp(itr->name.GetString(), "d_r_e")) {
        serverMediaDetail.d_rtp_ep = itr->value.GetString();
      } else if(!strcmp(itr->name.GetString(), "call_Id_msg10")) {
        serverMediaDetail.call_Id_msg10 = itr->value.GetString();
      } else if(!strcmp(itr->name.GetString(), "pClient")) {
        serverMediaDetail.pClient = itr->value.GetUint64();
      } else if (!strcmp(itr->name.GetString(),"media_proto")){
        serverMediaDetail.media_proto = itr->value.GetString();
      }else if(!strcmp(itr->name.GetString(), "out_proto")) {
       serverMediaDetail.out_proto = itr->value.GetString();
     }else if(!strcmp(itr->name.GetString(), "dtmf_digits")) {
        serverMediaDetail.dtmf_digits = itr->value.GetString();
     } else if(!strcmp(itr->name.GetString(),"sdpinfo")){
//           Value& sdp =  itr->value;
         // Call a function will decode the array details
	handle_sdpinfo_resp(itr->value,serverMediaDetail);
	
      }



    }

  CLog::Detail(THISMODULE, "xGateSrvrProcessor::handle_media_resp successfully handled");
  return EN_XGATE_STATUS_SUCCESS;
}
xGateRetVal xGateSrvrProcessor::handle_sdpinfo_resp(const Value& sdpObj, MgMediaDetail& serverMediaDetail){
 if(!sdpObj.IsArray()){
  CLog::Detail(THISMODULE, "xGateSrvrProcessor::handle_sdpinfo_resp Non-Array Object");
  return EN_XGATE_STATUS_ERROR;
}
for (SizeType i = 0; i < sdpObj.Size(); i++){
 if(sdpObj[i].IsObject()){
  if(i == 0)
   serverMediaDetail.sdpinfo[i].mediaType =  2;//sdp_media_audio 
  if( i == 1)
   serverMediaDetail.sdpinfo[i].mediaType =  3;//sdp_media_video 
  for (Value::ConstMemberIterator itr = sdpObj[i].MemberBegin(); itr != sdpObj[i].MemberEnd(); itr++){
     if(!strcmp(itr->name.GetString(), "ip_addr")) {
        serverMediaDetail.sdpinfo[i].ip_addr = itr->value.GetString();
      } else if (!strcmp(itr->name.GetString(), "ip_port")) {
        serverMediaDetail.sdpinfo[i].port = itr->value.GetInt();
      } else if(!strcmp(itr->name.GetString(), "codec")) {
        serverMediaDetail.sdpinfo[i].codec = itr->value.GetInt();
      } else if (!strcmp(itr->name.GetString(),"ice_ufrag")){
        serverMediaDetail.sdpinfo[i].ice_ufrag =  itr->value.GetString();
      } else if (!strcmp(itr->name.GetString(),"ice_pwd")){
        serverMediaDetail.sdpinfo[i].ice_pwd = itr->value.GetString();
      } else if (!strcmp(itr->name.GetString(),"fingerprint")){
        serverMediaDetail.sdpinfo[i].fingerprint = itr->value.GetString();
      } else if (!strcmp(itr->name.GetString(),"ptime")){
        serverMediaDetail.sdpinfo[i].ptime = itr->value.GetInt();
      }  else if(!strcmp(itr->name.GetString(), "play_file")) {
       serverMediaDetail.sdpinfo[i].play_file = itr->value.GetString();
     } else if(!strcmp(itr->name.GetString(), "ssrc")) {
       serverMediaDetail.sdpinfo[i].ssrc = itr->value.GetString();
     } else if(!strcmp(itr->name.GetString(), "cname")) {
       serverMediaDetail.sdpinfo[i].cname = itr->value.GetString();
     } else if(!strcmp(itr->name.GetString(), "mslable")) {
       serverMediaDetail.sdpinfo[i].mslabel = itr->value.GetString();
     } else if(!strcmp(itr->name.GetString(), "lable")) {
       serverMediaDetail.sdpinfo[i].label = itr->value.GetString();
     } else if(!strcmp(itr->name.GetString(), "dial_ip")) {
       serverMediaDetail.sdpinfo[i].dial_ip = itr->value.GetString();
     } else if(!strcmp(itr->name.GetString(), "dial_port")) {
       serverMediaDetail.sdpinfo[i].dial_port = itr->value.GetInt();
     } else if(!strcmp(itr->name.GetString(), "resp_ip")) {
       serverMediaDetail.sdpinfo[i].resp_ip = itr->value.GetString();
     } else if(!strcmp(itr->name.GetString(), "resp_port")) {
       serverMediaDetail.sdpinfo[i].resp_port = itr->value.GetInt();
     } else if (!strcmp(itr->name.GetString(),"relayip_addr")){
	     serverMediaDetail.sdpinfo[i].relayip_addr = itr->value.GetString();
     } else if (!strcmp(itr->name.GetString(),"relay_port")){
	     serverMediaDetail.sdpinfo[i].relay_port = itr->value.GetInt();
     } else if (!strcmp(itr->name.GetString(),"reflexip_addr")){
	     serverMediaDetail.sdpinfo[i].reflexip_addr = itr->value.GetString();
     } else if(!strcmp(itr->name.GetString(),"reflex_port")){
	     serverMediaDetail.sdpinfo[i].reflex_port = itr->value.GetInt();
     } else if (!strcmp(itr->name.GetString(),"framerate")){
		   serverMediaDetail.sdpinfo[i].framerate = itr->value.GetInt();
		 } else if (!strcmp(itr->name.GetString(),"imageattr_x")){
       serverMediaDetail.sdpinfo[i].imageattr_x = itr->value.GetInt();
		 } else if (!strcmp(itr->name.GetString(),"imageattr_y")){
		   serverMediaDetail.sdpinfo[i].imageattr_y = itr->value.GetInt();
		 }
  }
 }
}

 CLog::Detail(THISMODULE, "xGateSrvrProcessor::handle_sdpinfo_resp successfully handled");
 return EN_XGATE_STATUS_SUCCESS;
} 
xGateRetVal xGateSrvrProcessor::handle_gateway_reg(Value& dataMemObj, MgRegistrationDetail& gatewayRegDetail)
{
#if 0
  CLog::Detail(THISMODULE, "Enter xGateSrvrProcessor::handle_gateway_reg(%s)", m_serverType);
  Value::ConstMemberIterator itr = dataMemObj.MemberBegin();
  Value::ConstMemberIterator itrEnd = dataMemObj.MemberEnd();

  for (itr; itr != itrEnd; ++itr) {
    if(!strcmp(itr->name.GetString(), "gw_name")) {
      gatewayRegDetail.gw_name = itr->value.GetString().c_str();
    } else if(!strcmp(itr->name.GetString(), "gw_profile")) {
      gatewayRegDetail.gw_profile = itr->value.GetString();
    } else if(!strcmp(itr->name.GetString(), "channels_count")) {
      gatewayRegDetail.channels_count = itr->value.GetString();
    } else if (!strcmp(itr->name.GetString(), "rls_count")) {
      gatewayRegDetail.rsc_count = itr->value.GetInt();
    } else if(!strcmp(itr->name.GetString(), "rls_record")) {
      gatewayRegDetail.rls_record = itr->value.GetString();
    }
  }
  CLog::Detail(THISMODULE, "xGateSrvrProcessor::handle_gateway_reg successfully handled");
#endif
  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateSrvrProcessor::post_media_detail(xGateMgMsg *pMgMsg, MgMediaDetail& mediaDetail)
{
  CLog::Detail(THISMODULE, "Enter xGateSrvrProcessor::post_media_detail(%s)", m_serverType);
  xGateMgMsg* pMgMsgNew = NULL;
  pMgMsgNew = new xGateMgMsg();

  pMgMsgNew->setMsgType(EN_XGATE_MSG_MGC_MG_IN);
 // printf("xGateSrvrProcessor::post_media_detail (posting msg: %d)", pMgMsg->get_mg_msg_type());
  pMgMsgNew->set_mg_msg_type(pMgMsg->get_mg_msg_type());
  pMgMsgNew->set_media_detail(mediaDetail);

  ACE_Task<ACE_MT_SYNCH>* taskPtr = NULL;
  taskPtr = (xGateSipSignalProcessor*)GetSipPostHandler(XGATEPOSTTYPE::EN_PROCESS);
  if(!taskPtr) {
    CLog::Error(THISMODULE, "xGateSrvrProcessor::post_media_detail SipProcessor task not found");
    return EN_XGATE_STATUS_ERROR;
  }

  ACE_Message_Block* pAmb = NULL;
  pAmb = static_cast<xGateMgMsg*> (pMgMsgNew);
  ACE_Time_Value tval ((time(NULL) ));
  tval += ACE_Time_Value(0,1);

  if (taskPtr->putq(pAmb, &tval)) {
    CLog::Detail(THISMODULE, "xGateSrvrProcessor::post_media_detail SipProcessor enqueue message success");
  } else {
    CLog::Error(THISMODULE, "xGateSrvrProcessor::post_media_detail SipProcessor enqueue message failed");
    delete pMgMsgNew;
    return EN_XGATE_STATUS_ERROR;
  }

  return EN_XGATE_STATUS_SUCCESS;
}

