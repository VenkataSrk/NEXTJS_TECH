
//local includes
//#include "log.h"
//#include "klog.h"
#include "xglog.h"
#include "xGateMgProcessor.h"
//#include "xGateMGDispatcher.h"
#include "xGateHmpProcessor.h"
#include "xGateUtil.h"
#define THISMODULE "MgProc"

xGateMgProcessor::xGateMgProcessor(const char *serverType):xGateProcessor(serverType)
{
  XGLOG_INFO( "xGateMgProcessor::xGateMgProcessor");
}

xGateMgProcessor::~xGateMgProcessor(void)
{
  XGLOG_INFO( "xGateMgProcessor::~xGateMgProcessor");
}


xGateRetVal xGateMgProcessor::process_msg(xGateBaseMsg* pMsg)
{
  XGLOG_INFO( "Enter xGateMgProcessor::process_msg(%s)", m_serverType); 
  switch(pMsg->getMsgType()) {
    case EN_XGATE_MSG_MGC_MG_IN: //we got msg from MG server
      {
        XGLOG_INFO( "Enter process_msg processing EN_XGATE_MSG_MGC_MG_IN message"); 
        xGateMgMsg *pMgMsg = dynamic_cast<xGateMgMsg*>(pMsg);
        //decode and validate mg related message
        if(decode_media_msg(pMgMsg) != EN_XGATE_STATUS_SUCCESS) {
          XGLOG_ERROR( "xGateMgProcessor::process_msg data mg_msg decode or validation failed");
        }
        break;
      }
    case EN_XGATE_MSG_MGC_MG_OUT:
      {
        XGLOG_INFO( "Note: MG Dispatcher thread directly handle EN_XGATE_MSG_MGC_MG_OUT");
        break;
      }
    case EN_XGATE_MSG_UNKNOWN:
    default:
      {
        XGLOG_ERROR( "Enter xGateMgProcessor::process_msg is EN_XGATE_MSG_UNKNOWN");
        break;
      }      
  }
  return EN_XGATE_STATUS_SUCCESS; 
}

xGateRetVal xGateMgProcessor::decode_media_msg(xGateMgMsg *pMgMsg)
{
  XGLOG_INFO( "Enter xGateMgProcessor::decode_media_msg(%s)", (char*)ur_log_string (m_serverType)); 

  const char  *data = 0;
  unsigned int dataLen = 0;

  pMgMsg->get_data(data);
  dataLen = pMgMsg->get_data_len();

  if(!data && dataLen <=0) {
    XGLOG_ERROR( "xGateMgProcessor::decode_media_msg invalid data in xGateMgMsg.");
    return EN_XGATE_STATUS_ERROR;
  }

  XGLOG_INFO( "xGateMgProcessor::decode_media_msg data: %s, len: %d",(char*)ur_log_string(data), dataLen);

  Document doc;
  MgMediaDetail clientMediaDetail;
  xGateMgMsgType mgMsgType; 
  Value dataMemObj;

  if(create_doc_object(doc, data, dataMemObj) != EN_XGATE_STATUS_SUCCESS) {
    XGLOG_INFO( "xGateMgProcessor::decode_media_msg DOC object creation failed");
    return EN_XGATE_STATUS_ERROR;
  }

  mgMsgType = doc["msg_type"].GetInt();

  switch(mgMsgType) {

      case EN_MEDIA_STT_TTS_PORT_ALLOCATE:
      {
        XGLOG_INFO( "decode_mg_msg decoding EN_MEDIA_STT_TTS_PORT_ALLOCATE");
        handle_media_req(dataMemObj, clientMediaDetail);
        break;
      }
      case EN_XGATE_STT_TTS_RELEASE:
      {
        XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_RELEASE");
        handle_media_req(dataMemObj, clientMediaDetail);
        break;
      }
      case EN_XGATE_STT_TTS_REMOVE_PARTICIPANT:
      {
        XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_MEDIA_DELETE");
        handle_media_req(dataMemObj, clientMediaDetail);
        break;
      }
      case EN_XGATE_STT_TTS_TRANSFER:
      {
        XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_TRANSFER");
        handle_media_req(dataMemObj, clientMediaDetail);
        break;
      }
      case EN_XGATE_STT_TTS_MEDIA_CREATE:
      { 
        XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_STT_TTS_MEDIA_CREATE");
        handle_media_req(dataMemObj, clientMediaDetail);
        break;
      }
      case EN_XGATE_STT_TTS_RECONNECTION:
      {
        XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_STT_TTS_RECONNECTION");
        handle_media_req(dataMemObj, clientMediaDetail);
        break;
      }
      case EN_XGATE_STT_TTS_PLAY_TEXT:
      {
        XGLOG_INFO( "decode_mg_msg decoding EN_MEDIA_MRCP_PLAY_TEXT");
        handle_media_req(dataMemObj, clientMediaDetail);
        break;
      }
      case EN_XGATE_MSG_UNKNOWN:
      default:
      {
        XGLOG_ERROR( "decode_mg_msg unable to decode EN_XGATE_MSG_UNKNOWN message");
        return EN_XGATE_STATUS_ERROR;
        break;
      }
  }    

  pMgMsg->set_mg_msg_type(mgMsgType);
  pMgMsg->set_media_detail(clientMediaDetail);
  post_media_detail(pMgMsg);

  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateMgProcessor::create_doc_object(Document &doc, const char *jsonData, Value& dataMemObj)
{
  XGLOG_INFO( "Enter xGateMgProcessor::create_doc_object(%s)",(char*)ur_log_string(m_serverType)); 
  ParseResult ok = doc.Parse(jsonData);
  if(!ok){
  //if (doc.ParseInsitu(js).HasParseError()) {
    XGLOG_ERROR( "xGateMgProcessor::create_doc_object json parse error");
    return EN_XGATE_STATUS_ERROR;
  } else if(!doc.IsObject()) {
    XGLOG_ERROR( "xGateMgProcessor::create_doc_object not able to created document object");
    return EN_XGATE_STATUS_ERROR;
  } else if(!doc.HasMember("msg_type")) { 
    XGLOG_ERROR( "xGateMgProcessor::create_doc_object msg_type field is missing in json msg");
    return EN_XGATE_STATUS_ERROR;
  } else if(!doc.HasMember("data")) {
    XGLOG_ERROR( "xGateSrvrProcessor::create_doc_object data member not found in json msg");
    return EN_XGATE_STATUS_ERROR;
  }
  dataMemObj = doc["data"];
  XGLOG_INFO( "xGateMgProcessor::create_doc_object successfully created DOC object !!");
  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateMgProcessor::handle_media_req(Value& dataMemObj, MgMediaDetail& clientMediaDetail) 
{
  XGLOG_INFO( "Enter xGateMgProcessor::handle_media_alloc_req(%s)",(char*)ur_log_string(m_serverType)); 
  Value::ConstMemberIterator itr = dataMemObj.MemberBegin();
  Value::ConstMemberIterator itrEnd = dataMemObj.MemberEnd();

  for (itr; itr != itrEnd; ++itr) {
    
    if(!strcmp(itr->name.GetString(), "mgresource_id")) {
      clientMediaDetail.mgresource_id = itr->value.GetString();
    } else if(!strcmp(itr->name.GetString(), "call_type")) {
      clientMediaDetail.call_type = itr->value.GetInt();
      continue;
    } else if(!strcmp(itr->name.GetString(), "call_id")) {
      clientMediaDetail.call_id = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "old_mgresource_id")) {
      clientMediaDetail.old_mgresource_id = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "old_call_id")) {
      clientMediaDetail.old_call_id = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "group_call_id")) {
      clientMediaDetail.group_call_id = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "switch_call_id")) {
      clientMediaDetail.switch_call_id = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "context_id")) {
      clientMediaDetail.context_id = itr->value.GetInt();  
      continue;
    } else if(!strcmp(itr->name.GetString(), "leg_id")) {
      clientMediaDetail.leg_id = itr->value.GetInt();  
      continue;
    } else if(!strcmp(itr->name.GetString(), "media_proto")) {
      clientMediaDetail.media_proto = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "out_proto")) {
      clientMediaDetail.out_proto = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "record_file")) {
      clientMediaDetail.record_file = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "record_type")) {
      clientMediaDetail.record_type = itr->value.GetInt();
      continue;
    } else if(!strcmp(itr->name.GetString(), "dtmf_pt")){
      clientMediaDetail.audioDetail.m_dtmfpt = itr->value.GetInt();
      continue;
    } else if(!strcmp(itr->name.GetString(),"inband_dtmf")){
      clientMediaDetail.audioDetail.m_inBandDtmf = itr->value.GetInt();
      continue;
    } else if(!strcmp(itr->name.GetString(), "sig_type")) {
      clientMediaDetail.sig_type = itr->value.GetInt(); 
      continue;
    } else if(!strcmp(itr->name.GetString(), "call_dir")) {
      clientMediaDetail.call_dir = itr->value.GetInt(); 
      continue;
    } else if(!strcmp(itr->name.GetString(), "rtp_endpoint")) {
      clientMediaDetail.rtp_ep = itr->value.GetString(); 
      continue;
    } else if(!strcmp(itr->name.GetString(), "dtmf_digits")) {
      clientMediaDetail.dtmf_digits = itr->value.GetString(); 
      continue;
    } else if(!strcmp(itr->name.GetString(), "joiner_type")) {
      clientMediaDetail.joiner_type = itr->value.GetInt();
      continue;
    } else if(!strcmp(itr->name.GetString(), "reset_flag")) {
      clientMediaDetail.reset_flag = itr->value.GetInt();
      continue;
    } else if(!strcmp(itr->name.GetString(), "media_state")) {
      clientMediaDetail.media_event = itr->value.GetInt(); 
      continue;
    } else if(!strcmp(itr->name.GetString(), "mrcp_port_allocate")) {
      clientMediaDetail.mrcp_port_allocate = itr->value.GetInt();
      continue;
    } else if(!strcmp(itr->name.GetString(), "mrcp_port_activate")) {
      clientMediaDetail.mrcp_port_activate = itr->value.GetInt();
      continue;
    } else if(!strcmp(itr->name.GetString(), "mrcp_ip_info")) {
      clientMediaDetail.mrcp_ip_info = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "mrcp_port")) {
      clientMediaDetail.mrcp_port = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "mrcpUid")) {
      clientMediaDetail.mrcpUid = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "mrcpCallid")) {
      clientMediaDetail.mrcpCallid = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "mrcpDomainid")) {
      clientMediaDetail.mrcpDomainid = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "mrcp_Devicetype")) {
      clientMediaDetail.mrcp_Devicetype = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "mrcp_Ext")) {
      clientMediaDetail.mrcp_Ext = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "mrcp_isSttEnabled")) {
      clientMediaDetail.mrcp_isSttEnabled = itr->value.GetInt();
      continue;
    } else if(!strcmp(itr->name.GetString(), "mrcp_state")) {
      clientMediaDetail.mrcp_state = (STT_TTS_STATE)itr->value.GetInt();
      continue;
    } else if(!strcmp(itr->name.GetString(), "transcript_mode")) {
      clientMediaDetail.mrcp_state = (STT_TTS_STATE)itr->value.GetInt();
      continue;
    } else if(!strcmp(itr->name.GetString(), "tts_enabled")) {
      clientMediaDetail.tts_enabled = itr->value.GetInt();
      continue;
    } else if(!strcmp(itr->name.GetString(), "mg_Ip")) {
      clientMediaDetail.mg_Ip = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "mg_port")) {
      clientMediaDetail.mg_port = itr->value.GetInt();
      continue;
    } else if(!strcmp(itr->name.GetString(), "asr_type")) {
      clientMediaDetail.asr_type = itr->value.GetInt();
      clientMediaDetail.asr_type = 1;
      continue;
    } else if(!strcmp(itr->name.GetString(), "tts_type")) {
      clientMediaDetail.tts_type = itr->value.GetInt();
      clientMediaDetail.tts_type = 1;
      continue;
    } else if(!strcmp(itr->name.GetString(), "tts_text")) {
      clientMediaDetail.tts_text = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "ccaas_block_id")) {
      clientMediaDetail.ccaas_block_id=itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "ccaas_block_type")) {
      clientMediaDetail.ccaas_block_type=itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "ccaas_channel_id")) {
      clientMediaDetail.ccaas_channel_id=itr->value.GetString();
      continue; 
    } else if(!strcmp(itr->name.GetString(), "ccaas_agent_id")) {
      clientMediaDetail.ccaas_agent_id=itr->value.GetString();
      continue; 
    } else if(!strcmp(itr->name.GetString(), "ccaas_bot_url")) {
      clientMediaDetail.ccaas_bot_url=itr->value.GetString();
      continue; 
    } else if(!strcmp(itr->name.GetString(), "is_ccaas")) {
      clientMediaDetail.is_ccaas = itr->value.GetInt();
      continue;
    } else if(!strcmp(itr->name.GetString(), "ccaas_port_type")){
      clientMediaDetail.ccaas_port_type = itr->value.GetInt();
      continue;
    } else if(!strcmp(itr->name.GetString(), "ccaas_user_agent")){
      clientMediaDetail.ccaas_user_agent = itr->value.GetInt();
      continue;
    } else if(!strcmp(itr->name.GetString(), "ccaas_chatbot_init")){
      clientMediaDetail.ccaas_chatbot_init = itr->value.GetInt();
      continue;
    } else if(!strcmp(itr->name.GetString(), "ccaas_bot_first")){
      clientMediaDetail.ccaas_bot_first = itr->value.GetInt();
      continue;
    } else if(!strcmp(itr->name.GetString(), "ccaas_bot_id")){
      clientMediaDetail.ccaas_bot_id = itr->value.GetInt();
      continue;
    } else if(!strcmp(itr->name.GetString(), "ccaas_voice_recog_port")){
      clientMediaDetail.ccaas_voice_recog_port = itr->value.GetInt();
      continue;
    } else if(!strcmp(itr->name.GetString(), "ccaas_agent_name")) {
      clientMediaDetail.ccaas_agent_name=itr->value.GetString();
      continue; 
    } else if(!strcmp(itr->name.GetString(), "ccaas_agent_email")) {
      clientMediaDetail.ccaas_agent_email=itr->value.GetString();
      continue; 
    } else if(!strcmp(itr->name.GetString(), "ccaas_cust_firstname")) {
      clientMediaDetail.ccaas_cust_firstname=itr->value.GetString();
      continue; 
    } else if(!strcmp(itr->name.GetString(), "ccaas_user_id")) {
      clientMediaDetail.ccaas_agent_user_id=itr->value.GetString();
      continue; 
    } else if(!strcmp(itr->name.GetString(), "ccaas_cust_lastname")) {
      clientMediaDetail.ccaas_cust_lastname=itr->value.GetString();
      continue; 
    } else if(!strcmp(itr->name.GetString(), "ccaas_auto_attendant")) {
      clientMediaDetail.ccaas_auto_attendant=itr->value.GetInt();
      continue;
    } else if(!strcmp(itr->name.GetString(), "ccaas_virtual_agent")) {
      clientMediaDetail.ccaas_virtual_agent=itr->value.GetInt();
      continue;
    } else if(!strcmp(itr->name.GetString(), "ccaas_cli")) {
      clientMediaDetail.ccaas_cli=itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "ccaas_ddi")) {
      clientMediaDetail.ccaas_ddi=itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(),"sdpinfo")) {
	// Call a function will decode the array details
      handle_sdpinfo_process(itr->value,clientMediaDetail);
      continue;
    } else if(!strcmp(itr->name.GetString(), "ivr_list")) {
      std::string ivrList = itr->value.GetString();
      continue;
    }else if(!strcmp(itr->name.GetString(),"is_smart_assist")){
      clientMediaDetail.is_smart_assist = itr->value.GetInt();
      continue;
    }
			
  }

  XGLOG_INFO( "xGateMgProcessor::handle_media_alloc_req successfully handled");
  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateMgProcessor::handle_sdpinfo_process(const Value& sdpObj, MgMediaDetail& clientMediaDetail) {
  if(!sdpObj.IsArray()){
    XGLOG_INFO( "xGateSrvrProcessor::handle_sdpinfo_resp Non-Array Object");
    return EN_XGATE_STATUS_ERROR;
  }
  for (SizeType i = 0; i < sdpObj.Size(); i++){
    if(sdpObj[i].IsObject()){
      if(i == 0) {
        handle_sdpinfo_audio(sdpObj, clientMediaDetail);
      } else if( i == 1) {
      }
    }
  }
  XGLOG_INFO( "xGateMgProcessor::handle_sdpinfo_resp successfully handled");
  return EN_XGATE_STATUS_SUCCESS;
}



xGateRetVal xGateMgProcessor::handle_sdpinfo_audio(const Value& sdpObj, MgMediaDetail& clientMediaDetail)
{
  unsigned int i = 0;

  for (Value::ConstMemberIterator itr = sdpObj[i].MemberBegin(); itr != sdpObj[i].MemberEnd(); itr++){
     if(!strcmp(itr->name.GetString(), "media_type")) {
      clientMediaDetail.audioDetail.m_mediaType = itr->value.GetInt();
    }
    if(clientMediaDetail.audioDetail.m_mediaType == 0) {
      break;
    }

    if(!strcmp(itr->name.GetString(), "fmtp")) {
      clientMediaDetail.audioDetail.m_fmtp = itr->value.GetString();
			if(clientMediaDetail.audioDetail.m_fmtp == ""){
				continue;
			}else{
				string Fmtp = itr->value.GetString();
				//Get_Opus_Fmtp(Fmtp,clientMediaDetail.audioDetail);
				printf("FMTP");
			}
      continue;
    } else if(!strcmp(itr->name.GetString(), "dtmf_fmtp")) {
      clientMediaDetail.audioDetail.m_dtmfFmtp = itr->value.GetString();
      continue;  
    } else if(!strcmp(itr->name.GetString(), "rtcp_fp_mechanism")) {
      clientMediaDetail.audioDetail.m_rtcpFpMechanism = itr->value.GetString();
      continue;  
    } else if(!strcmp(itr->name.GetString(), "codec")) {
      clientMediaDetail.audioDetail.m_codec = itr->value.GetInt();
      continue;
    } else if(!strcmp(itr->name.GetString(), "codec_name")) {
      clientMediaDetail.audioDetail.m_codecName = itr->value.GetString();
      continue;  
    } else if(!strcmp(itr->name.GetString(), "sampling_rate")) {
      clientMediaDetail.audioDetail.m_samplingRate = itr->value.GetInt();
      continue;  
    } else if(!strcmp(itr->name.GetString(), "dtmf_sampling_rate")) {
      clientMediaDetail.audioDetail.m_dtmfSamplingRate = itr->value.GetInt();
      continue;  
    } else if(!strcmp(itr->name.GetString(), "ptime")) {
      clientMediaDetail.audioDetail.m_ptime = itr->value.GetInt();
      continue;
    } else if(!strcmp(itr->name.GetString(), "max_ptime")) {
      clientMediaDetail.audioDetail.m_maxPtime = itr->value.GetInt();
      continue;
    } else if(!strcmp(itr->name.GetString(), "dtmf_pt")){
      clientMediaDetail.audioDetail.m_dtmfpt = itr->value.GetInt();
      continue;   
    } else if(!strcmp(itr->name.GetString(), "cn_pt")){
      clientMediaDetail.audioDetail.m_cnPt = itr->value.GetInt();
      continue;   
    } else if(!strcmp(itr->name.GetString(), "cn_sampling_rate")){
      clientMediaDetail.audioDetail.m_cnSamplingRate = itr->value.GetInt();
      continue;   
    } else if(!strcmp(itr->name.GetString(), "rtcp_fp_payload")){
      clientMediaDetail.audioDetail.m_rtcpFpPayload = itr->value.GetInt();
      continue;   
    } else if(!strcmp(itr->name.GetString(), "ip_addr")) {
      clientMediaDetail.audioDetail.m_clientIp = itr->value.GetString();
      continue;
    } else if (!strcmp(itr->name.GetString(), "ip_port")) {
      clientMediaDetail.audioDetail.m_clientPort = itr->value.GetInt();
      continue;
    } else if(!strcmp(itr->name.GetString(), "relayip_addr")) {
      clientMediaDetail.audioDetail.m_relayIp = itr->value.GetString();
      continue;
    } else if (!strcmp(itr->name.GetString(), "relay_port")) {
      clientMediaDetail.audioDetail.m_relayPort = itr->value.GetInt();
      continue;
    } else if(!strcmp(itr->name.GetString(), "reflexip_addr")) {
      clientMediaDetail.audioDetail.m_rflxIp = itr->value.GetString();
      continue;
    } else if (!strcmp(itr->name.GetString(), "reflex_port")) {
      clientMediaDetail.audioDetail.m_rflxPort = itr->value.GetInt();
      continue;
    } else if(!strcmp(itr->name.GetString(), "dial_ip")) {
      clientMediaDetail.audioDetail.m_dialIp = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "dial_port")) {
      clientMediaDetail.audioDetail.m_dialPort = itr->value.GetInt();
      continue;
    } else if(!strcmp(itr->name.GetString(), "resp_ip")) {
      clientMediaDetail.audioDetail.m_respIp = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "resp_port")) {
      clientMediaDetail.audioDetail.m_respPort = itr->value.GetInt();
      continue;
     }  
   }
 }


/*TODO: this will post media detail to HmpChannelHandler thread */
xGateRetVal xGateMgProcessor::post_media_detail(xGateMgMsg *pMgMsg)
{
  XGLOG_INFO( "Enter xGateMgProcessor::post_media_detail(%s)", (char*)ur_log_string(m_serverType));

  xGateMgMsg* pMgMsg1 = NULL;
  xGateNetConnection netConInfo;
  pMgMsg1 = new xGateMgMsg();

  pMgMsg1->setMsgType(EN_XGATE_MSG_MGC_MG_IN);
  //printf("xGateMgProcessor::post_media_detail (posting msg: %d)", pMgMsg->get_mg_msg_type());
  pMgMsg1->set_mg_msg_type(pMgMsg->get_mg_msg_type());
  pMgMsg1->set_media_detail(pMgMsg->get_media_detail());
  pMgMsg->get_net_con_info(netConInfo);
  pMgMsg1->set_net_con_info(netConInfo);

  XGLOG_INFO("The xGateMgProcessor::post_media_detail pMgMsg->get_mg_msg_type (%d)",pMgMsg->get_mg_msg_type());
  cout<<"The xGateMgProcessor::post_media_detail pMgMsg->get_mg_msg_type "<<pMgMsg->get_mg_msg_type()<<endl;
  ACE_Task<ACE_MT_SYNCH>* taskPtr = NULL;
  taskPtr = xGateUtil::getHmpProcessor();
  if(!taskPtr) {
    XGLOG_ERROR( "xGateMgProcessor::post_media_detail HmpProcessor task not found");
    return EN_XGATE_STATUS_ERROR;
  }

  ACE_Message_Block* pAmb = NULL;
  pAmb = static_cast<xGateMgMsg*> (pMgMsg1);
  ACE_Time_Value tval ((time(NULL) ));
  tval += ACE_Time_Value(0,1);

  if (taskPtr->putq(pAmb, &tval)) {
    XGLOG_INFO( "xGateMgProcessor::post_media_detail HmpProcessor enqueue message success");
  } else {
    XGLOG_ERROR( "xGateMgProcessor::post_media_detail HmpProcessor enqueue message failed");
    delete pMgMsg1;
    return EN_XGATE_STATUS_ERROR;
  }

  return EN_XGATE_STATUS_SUCCESS;
}
