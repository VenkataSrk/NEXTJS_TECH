#include "ace/OS_NS_stdio.h"
#include "ace/OS_NS_errno.h"
#include "ace/OS_NS_string.h"
#include "ace/OS_NS_sys_time.h"
#include "SFUClient.h"
#include "xGateUtil.h"
#include "SFUBridgeProcessor.h"
#include "SFUController.h"
#include <typeinfo>
#include <iostream>
#include <fstream>
#include "xGateSFUtil.h"
#include "xGateSFUServiceMsg.h"
#include "xGateSFUService.h"
#include "xglog.h"
#define THISMODULE "SFU_CLIENT"


bool SFUClient::send_sfu_id(unsigned int connection_id)
{
 m_connection_id = connection_id;
 xGateSFUServiceMsg *pmsg = new xGateSFUServiceMsg();
 pmsg->setDstModuleId(IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE);
 pmsg->setSrcModuleId(IURDefines::UR_MODULE_SFU_SERVICE);
 pmsg->set_connection_id(m_connection_id);
 pmsg->set_sfu_id(m_sfu_id);
 string jsonBuff("");
 int len = 0;
 int gatewayid = 0;
 MgMediaDetail mediaDetail;
 mediaDetail.mgmsg_type = EN_XGATE_MG_SFU_ID;
 mediaDetail.gateway_id = m_sfu_id;
 xGateNetConnection netConInfo;
 xGateMgMsg sMgMsg1;
 sMgMsg1.set_mg_msg_type(EN_XGATE_MG_SFU_ID);
 sMgMsg1.set_media_detail(mediaDetail);
 sMgMsg1.set_net_con_info(netConInfo);
 len = create_media_msg(&sMgMsg1, jsonBuff, gatewayid);
 if(len <= 0){
  XGLOG_ERROR("Invalid msg len in SFU ID message");
 }
 char *data = new char[len+1];
 memset(data,0,len+1);
 memcpy(data,jsonBuff.c_str(),len);
 pmsg->setMsg(data);
 pmsg->setMsgLen(len);
 pmsg->setMsgType((xGateMsgType)EN_XGATE_MSG_SFU_ID);
 XGLOG_INFO("Sending sfu_id: %d to UR_MODULE_SFU_SERVICE data ptr: %p",m_sfu_id, data);
 return SFUSERVICE->getConfig()->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(pmsg));
}

bool SFUClient::set_sfu_id(int sfu_id)
{
  m_sfu_id = sfu_id;
  return TRUE;
}

int SFUClient::get_sfu_id(void)
{
  return m_sfu_id;
}

bool SFUClient::get_mbc_msg(const char *mbc_msg,int len)
{
 decode_media_msg(mbc_msg,len);
 return TRUE;
}

int SFUClient:: send_msg_to_mbc(char *msg, int len)
{
 xGateSFUServiceMsg *pmsg = new xGateSFUServiceMsg();
 pmsg->setDstModuleId(IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE);
 pmsg->setSrcModuleId(IURDefines::UR_MODULE_SFU_SERVICE);
 pmsg->set_connection_id(m_connection_id);
 char *data = new char[len+1];
 memset(data,0,len+1);
 memcpy(data,msg,len);
 pmsg->setMsg(data);
 pmsg->setMsgLen(len);
 pmsg->setMsgType((xGateMsgType)EN_XGATE_MSG_SFU_CLIENT);
 SFUSERVICE->getConfig()->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(pmsg));
 return 0;
}

xGateRetVal SFUClient::decode_media_msg(const char *data,int dataLen)
{
    //XGLOG_INFO( "Enter SFUClient::decode_media_msg");
    xGateMgMsg *pMgMsg = new xGateMgMsg();


    if(!data && dataLen <=0) {
        XGLOG_ERROR( "SFUClient::decode_media_msg invalid data in xGateMgMsg.");
        return EN_XGATE_STATUS_ERROR;
    }


    Document doc;
    MgMediaDetail clientMediaDetail;
    xGateMgMsgType mgMsgType;
    Value dataMemObj;

    if(create_doc_object(doc, data, dataMemObj) != EN_XGATE_STATUS_SUCCESS) {
        XGLOG_INFO( "SFUClient::decode_media_msg DOC object creation failed");
        return EN_XGATE_STATUS_ERROR;
    }

    mgMsgType = doc["msg_type"].GetInt();

    /*Do not log msg_type: 21 i,,e key frame requests come frequently if there is PLI request*/
    if (mgMsgType != EN_XGATE_MG_MEDIA_REQUEST_KEY_FRAME)
      XGLOG_INFO( "SFUClient::decode_media_msg data: %s, len: %d\n", data, dataLen);
    switch(mgMsgType) {
        case EN_XGATE_MG_MEDIA_CREATE:
            {
                XGLOG_INFO( "decode_media_msg decoding EN_XGATE_MG_MEDIA_CREATE");
                handle_media_req(mgMsgType,dataMemObj, clientMediaDetail); //TODO: Yoga we can move this area to common place. why replicate this
		clientMediaDetail.asr_type = 1;
                break;
            }
        case EN_XGATE_MG_ALLOCATE_ORGINATE:
            {
                XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_ALLOCATE_ORGINATE");
                handle_media_req(mgMsgType,dataMemObj, clientMediaDetail);
                break;
            }
        case EN_XGATE_MG_ALLOCATE_TERMINATE:
            {
                XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_ALLOCATE_TERMINATE");
                handle_media_req(mgMsgType,dataMemObj, clientMediaDetail);
                break;
            }
        case EN_XGATE_MG_MEDIA_DELETE:
            {
                XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_MEDIA_DELETE");
                handle_media_req(mgMsgType,dataMemObj, clientMediaDetail);
                break;
            }
        case EN_XGATE_MG_RELEASE:
            {
                XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_RELEASE");
                handle_media_req(mgMsgType,dataMemObj, clientMediaDetail);
                break;
            }
        case EN_XGATE_MG_18X_RESPONSE:
            {
                XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_18X_RESPONSE");
                handle_media_req(mgMsgType,dataMemObj, clientMediaDetail);
                break;
            }
        case EN_XGATE_MG_PLAY_BACK:
            {
                XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_PLAY_BACK");
                handle_media_req(mgMsgType,dataMemObj, clientMediaDetail);
                break;
            }
        case EN_XGATE_MG_HOLD_RESUME:
            {
                XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_HOLD_RESUME");
                handle_media_req(mgMsgType,dataMemObj, clientMediaDetail);
                break;
            }
        case EN_XGATE_MG_DTMF_DIGITS:
            {
                XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_DTMF_DIGITS");
                handle_media_req(mgMsgType,dataMemObj, clientMediaDetail);
                break;
            }
        case EN_XGATE_MG_CALL_RECORD:
            {
                XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_CALL_RECORD");
                handle_media_req(mgMsgType,dataMemObj, clientMediaDetail);
                break;
            }
        case EN_XGATE_MG_SWITCH_CALLTO_CONFERENCE:
            {
                XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_SWITCH_CALLTO_CONFERENCE");
                handle_media_req(mgMsgType,dataMemObj, clientMediaDetail);
                break;
            }
        case EN_XGATE_MG_ADD_PARTICIPANT:
            {
                XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_ADD_PARTICIPANT");
                handle_media_req(mgMsgType,dataMemObj, clientMediaDetail);
                break;
            }
        case EN_XGATE_MG_SDP_UPDATE_REQUEST:
            {
                XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_SDP_UPDATE_REQUEST");
                handle_media_req(mgMsgType,dataMemObj, clientMediaDetail);
                break;
            }
        case EN_XGATE_MG_SDP_UPDATE_RESPONSE:
            {
                XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_SDP_UPDATE_RESPONSE");
                handle_media_req(mgMsgType,dataMemObj, clientMediaDetail);
                break;
            }
        case EN_XGATE_MG_TILE_CHANGE_REQUEST:
            {
                XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_TILE_CHANGE_REQUEST");
                handle_media_req(mgMsgType,dataMemObj, clientMediaDetail);
                break;
            }
        case EN_XGATE_MG_SCREENSHARE_VIDEO_REQUEST:
            {
                XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_SCREENSHARE_VIDEO_REQUEST");
                handle_media_req(mgMsgType,dataMemObj, clientMediaDetail);
                break;
            }
        case EN_XGATE_MG_ACTIVE_SPEAKER_VIDEO_REQUEST:
            {
                XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_ACTIVE_SPEAKER_VIDEO_REQUEST");
                handle_media_req(mgMsgType,dataMemObj, clientMediaDetail);
                break;
            }
        case EN_XGATE_MG_GRID_VIEW_VIDEO_REQUEST:
            {
                XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_GRID_VIEW");
                handle_media_req(mgMsgType,dataMemObj, clientMediaDetail);
                break;
            }

	case EN_XGATE_MG_MEDIA_REQUEST_KEY_FRAME:
	    {
                handle_media_req(mgMsgType,dataMemObj, clientMediaDetail);
	        break;
	    }
	case EN_XGATE_MG_MEDIA_ADD_CLIENT_CONF_LIST:
	    {
                XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_MEDIA_ADD_CLIENT_CONF_LIST");
                handle_media_req(mgMsgType,dataMemObj, clientMediaDetail);
	        break;
	    }
	case EN_XGATE_MG_MEDIA_REQUEST_CREATE_SEND_CHANNEL:
	    {
                XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_MEDIA_REQUEST_CREATE_SEND_CHANNEL");
                handle_media_req(mgMsgType,dataMemObj, clientMediaDetail);
	        break;
	    }
	case EN_XGATE_MG_RETRY_MEDIA_CONNECTION:
	    {
                XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_RETRY_MEDIA_CONNECTION");
	        break;
	    }
        case EN_XGATE_MG_VIDEO_REC_REQ:
            {
                XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_VIDEO_REC_REQ");
                handle_media_req(mgMsgType,dataMemObj, clientMediaDetail);
                break;
            }
        case EN_XGATE_MG_VIDEO_REC_RES:
            {
                XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_VIDEO_REC_RES");
                handle_media_req(mgMsgType,dataMemObj, clientMediaDetail);
                break;
            }
        case EN_XGATE_MG_VIDEO_KEY_FRAME_REQUEST_FROM_REC_SERVER:
            {
                XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_VIDEO_KEY_FRAME_REQUEST_FROM_REC_SERVER");
                handle_media_req(mgMsgType,dataMemObj, clientMediaDetail);
                break;
            }
        case EN_XGATE_MG_ACTIVE_SPEAKER_INFO:
            {
                XGLOG_INFO( "decode_mg_msg decoding EN_XGATE_MG_ACTIVE_SPEAKER_INFO");
                handle_media_req(mgMsgType,dataMemObj, clientMediaDetail);
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
    if (mgMsgType == EN_XGATE_MG_SDP_UPDATE_REQUEST || mgMsgType == EN_XGATE_MG_TILE_CHANGE_REQUEST
         || mgMsgType == EN_XGATE_MG_SCREENSHARE_VIDEO_REQUEST
         || mgMsgType == EN_XGATE_MG_GRID_VIEW_VIDEO_REQUEST
         || mgMsgType == EN_XGATE_MG_ACTIVE_SPEAKER_VIDEO_REQUEST){
       pMgMsg->set_participant_list (clientMediaDetail);
    }
    post_media_detail(pMgMsg);

    return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal SFUClient::create_doc_object(Document &doc, const char *jsonData, Value& dataMemObj)
{
    doc.Parse(jsonData);
    if (doc.HasParseError()) {
        XGLOG_ERROR( "SFUClient::create_doc_object json parse error");
        return EN_XGATE_STATUS_ERROR;
    } else if(!doc.IsObject()) {
        XGLOG_ERROR( "SFUClient::create_doc_object not able to created document object");
        return EN_XGATE_STATUS_ERROR;
    } else if(!doc.HasMember("msg_type")) {
        XGLOG_ERROR( "SFUClient::create_doc_object msg_type field is missing in json msg");
        return EN_XGATE_STATUS_ERROR;
    } else if(!doc.HasMember("data")) {
        XGLOG_ERROR( "xGateSrvrProcessor::create_doc_object data member not found in json msg");
        return EN_XGATE_STATUS_ERROR;
    }
    dataMemObj = doc["data"];
    return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal SFUClient::handle_media_req(xGateMgMsgType mgMsgType,
                                       Value& dataMemObj, MgMediaDetail& clientMediaDetail)
{
    XGLOG_INFO( "Enter SFUClient::handle_media_req mgMsgType: %d", mgMsgType);
    Value::ConstMemberIterator itr = dataMemObj.MemberBegin();
    Value::ConstMemberIterator itrEnd = dataMemObj.MemberEnd();

    for (itr; itr != itrEnd; ++itr) {
        if(!strcmp(itr->name.GetString(), "mgresource_id")) {
            clientMediaDetail.mgresource_id = itr->value.GetString();
        } else if(!strcmp(itr->name.GetString(), "call_type")) {
            clientMediaDetail.call_type = itr->value.GetUint();
        } else if(!strcmp(itr->name.GetString(), "call_id")) {
            clientMediaDetail.call_id = itr->value.GetString();
        } else if(!strcmp(itr->name.GetString(), "conn_id")) {
            clientMediaDetail.conn_id = itr->value.GetString();
        } else if(!strcmp(itr->name.GetString(), "device_id")) {
            clientMediaDetail.device_id = itr->value.GetString();
        } else if(!strcmp(itr->name.GetString(), "sip_id")) {
            clientMediaDetail.sip_id = itr->value.GetString();
        } else if(!strcmp(itr->name.GetString(), "context_id")) {
            clientMediaDetail.context_id = itr->value.GetInt();
        } else if(!strcmp(itr->name.GetString(), "is_ccaas")) {
            clientMediaDetail.is_ccaas = itr->value.GetInt();
        } else if(!strcmp(itr->name.GetString(), "leg_id")) {
            clientMediaDetail.leg_id = itr->value.GetUint();
        } else if(!strcmp(itr->name.GetString(), "retrieved_videossrc")) {
            clientMediaDetail.retrieved_videossrc = itr->value.GetUint();
        } else if(!strcmp(itr->name.GetString(), "media_proto")) {
            clientMediaDetail.media_proto = itr->value.GetString();
        } else if(!strcmp(itr->name.GetString(), "out_proto")) {
            clientMediaDetail.out_proto = itr->value.GetString();
        } else if(!strcmp(itr->name.GetString(), "meeting_name")) {
            clientMediaDetail.meeting_name = itr->value.GetString();
        } else if(!strcmp(itr->name.GetString(), "record_file")) {
            clientMediaDetail.record_file = itr->value.GetString();
        } else if(!strcmp(itr->name.GetString(), "browser_type")) {
            clientMediaDetail.browser_type = itr->value.GetString();
        } else if(!strcmp(itr->name.GetString(), "record_url")) {
            clientMediaDetail.record_url = itr->value.GetString();
        } else if(!strcmp(itr->name.GetString(), "sig_type")) {
            clientMediaDetail.sig_type = itr->value.GetUint();
            continue;
        } else if(!strcmp(itr->name.GetString(), "call_dir")) {
            clientMediaDetail.call_dir = itr->value.GetUint();
            continue;
        } else if(!strcmp(itr->name.GetString(), "d_r_e")) {
            clientMediaDetail.d_rtp_ep = itr->value.GetString();
            continue;
        } else if(!strcmp(itr->name.GetString(), "o_r_e")) {
            clientMediaDetail.o_rtp_ep = itr->value.GetString();
            continue;
        } else if(!strcmp(itr->name.GetString(), "call_Id_msg10")) {
            clientMediaDetail.call_Id_msg10 = itr->value.GetString();
            continue;
        } else if(!strcmp(itr->name.GetString(), "pClient")) {
            clientMediaDetail.pClient = itr->value.GetUint64();
            continue;
        } else if(!strcmp(itr->name.GetString(), "dtmf_digits")) {
            clientMediaDetail.dtmf_digits = itr->value.GetString();
            continue;
        } else if(!strcmp(itr->name.GetString(), "joiner_type")) {
            clientMediaDetail.joiner_type = itr->value.GetUint();
	    continue;
        } else if(!strcmp(itr->name.GetString(), "screenshare_id")) {
            clientMediaDetail.screenshare_id = itr->value.GetString();
            continue;
        } else if(!strcmp(itr->name.GetString(), "sdp_str")) {
            clientMediaDetail.sdp_str = itr->value.GetString();
            continue;
        } else if(!strcmp(itr->name.GetString(), "asr_type")) {
            clientMediaDetail.asr_type = itr->value.GetUint();
            continue;
        } else if(!strcmp(itr->name.GetString(), "media_state")) {
            clientMediaDetail.media_event = itr->value.GetUint();
        } else if(!strcmp(itr->name.GetString(),"sdpinfo")) {
            // Call a function will decode the array details
            handle_sdpinfo_process(itr->value,clientMediaDetail);
        } else if(!strcmp(itr->name.GetString(),"participant_list")
                 && (mgMsgType == EN_XGATE_MG_SDP_UPDATE_REQUEST || mgMsgType == EN_XGATE_MG_TILE_CHANGE_REQUEST
                     || mgMsgType == EN_XGATE_MG_SCREENSHARE_VIDEO_REQUEST
                     || mgMsgType == EN_XGATE_MG_GRID_VIEW_VIDEO_REQUEST
                     || mgMsgType == EN_XGATE_MG_ACTIVE_SPEAKER_VIDEO_REQUEST)) {
            // Call a function will decode the array details
            handle_participant_list(itr->value,clientMediaDetail);
        }  else if(!strcmp(itr->name.GetString(),"urmeet_user_list")) {
            handle_urmeet_userlist(itr->value, clientMediaDetail);
	}else if(!strcmp(itr->name.GetString(), "prev_act_spkr")) {
            clientMediaDetail.prev_act_spkr = itr->value.GetString();
            continue;
        }
    }
    return EN_XGATE_STATUS_SUCCESS;
}
xGateRetVal SFUClient::handle_sdpinfo_process(const Value& sdpObj, MgMediaDetail& clientMediaDetail) {
    if(!sdpObj.IsArray()){
        XGLOG_INFO( "xGateSrvrProcessor::handle_sdpinfo_resp Non-Array Object");
        return EN_XGATE_STATUS_ERROR;
    }
    for (SizeType i = 0; i < sdpObj.Size(); i++){
        if(sdpObj[i].IsObject()){
            if(i == 0) {
                handle_sdpinfo_audio(sdpObj, clientMediaDetail);
            } else if( i == 1) {
                handle_sdpinfo_video(sdpObj, clientMediaDetail);
            }
        }
    }
    return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal SFUClient::handle_participant_list(const Value& ParticipantListObj, MgMediaDetail& clientMediaDetail)
{
    if(!ParticipantListObj.IsArray()){
        XGLOG_INFO( "xGateSrvrProcessor::handle_participant_list Non-Array Object");
        return EN_XGATE_STATUS_ERROR;
    }
    for (SizeType i = 0; i < ParticipantListObj.Size(); i++){
        if(ParticipantListObj[i].IsObject()){
           ParticipantInfo pPInfo ;
             for (Value::ConstMemberIterator itr = ParticipantListObj[i].MemberBegin();
                                            itr != ParticipantListObj[i].MemberEnd(); itr++){
               if(!strcmp(itr->name.GetString(), "call_id")) {
                  pPInfo.call_id = itr->value.GetString();
                  XGLOG_INFO("Extracted Participant from:%s is callId: %s",clientMediaDetail.call_id.c_str(),
                               pPInfo.call_id.c_str());
                } else if(!strcmp(itr->name.GetString(), "joiner_type")) {
                  pPInfo.joiner_type = itr->value.GetInt ();
                }
             }
             clientMediaDetail.Participant_list.emplace(clientMediaDetail.Participant_list.begin() + i,
                                                       pPInfo);
           }
     }
     XGLOG_INFO( "SFUClient::handle_participant_list successfully decoded");
     return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal SFUClient::handle_urmeet_userlist(const Value& UrmeetistObj, MgMediaDetail& clientMediaDetail)
{
  if(!UrmeetistObj.IsArray()){
    XGLOG_INFO( "xGateSrvrProcessor::handle_urmeet_userlist Non-Array Object");
    return EN_XGATE_STATUS_ERROR;
  }
  for (SizeType i = 0; i < UrmeetistObj.Size(); i++){
    if(UrmeetistObj[i].IsObject()){
      URMeetUserInfo URUInfo;
      for (Value::ConstMemberIterator itr = UrmeetistObj[i].MemberBegin();itr != UrmeetistObj[i].MemberEnd(); itr++){
        if(!strcmp(itr->name.GetString(), "email")) {
          URUInfo.email = itr->value.GetString();
          //XGLOG_INFO("Extracted Participant: email: %s",URUInfo.email.c_str());
        } else if(!strcmp(itr->name.GetString(), "profile_img")) {
          URUInfo.profile_img = itr->value.GetString ();
          //XGLOG_INFO("Extracted Participant: profile_img: %s",URUInfo.profile_img.c_str());
        } else if(!strcmp(itr->name.GetString(), "sip_id")) {
          URUInfo.sip_id = itr->value.GetString ();
          //XGLOG_INFO("Extracted Participant: sip_id: %s",URUInfo.sip_id.c_str());
        }else if(!strcmp(itr->name.GetString(), "name")) {
          URUInfo.name = itr->value.GetString ();
          //XGLOG_INFO("Extracted Participant: name: %s",URUInfo.name.c_str());
        }else if(!strcmp(itr->name.GetString(), "ext")) {
          URUInfo.ext = itr->value.GetString ();
          //XGLOG_INFO("Extracted Participant: ext: %s",URUInfo.ext.c_str());
        }else if(!strcmp(itr->name.GetString(), "role_type")) {
          URUInfo.role_type = itr->value.GetInt ();
          //XGLOG_INFO("Extracted Participant: role_type: %d",URUInfo.role_type);
        }else if(!strcmp(itr->name.GetString(), "user_type")) {
          URUInfo.user_type = itr->value.GetString ();
        //   XGLOG_INFO("Extracted Participant: user_type: %s",URUInfo.user_type.c_str());
        }
      }
      clientMediaDetail.meetuser_info.emplace(clientMediaDetail.meetuser_info.begin() + i, URUInfo);
    }
  }
  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal SFUClient::handle_sdpinfo_audio(const Value& sdpObj, MgMediaDetail& clientMediaDetail)
{
    unsigned int i = 0;

    for (Value::ConstMemberIterator itr = sdpObj[i].MemberBegin(); itr != sdpObj[i].MemberEnd(); itr++){
        if(!strcmp(itr->name.GetString(), "media_type")) {
            clientMediaDetail.audioDetail.m_mediaType = itr->value.GetUint();
        }
        if(clientMediaDetail.audioDetail.m_mediaType == 0) {
            break;
        }

        if(!strcmp(itr->name.GetString(), "fmtp")) {
            clientMediaDetail.audioDetail.m_fmtp = itr->value.GetString();
            continue;
        } else if(!strcmp(itr->name.GetString(), "codec")) {
            clientMediaDetail.audioDetail.m_codec = itr->value.GetUint();
            continue;
        } else if(!strcmp(itr->name.GetString(), "codec_name")) {
            clientMediaDetail.audioDetail.m_codecName = itr->value.GetString();
            string s1("opus");
            string s2("OPUS");
            if(s1.compare(clientMediaDetail.audioDetail.m_codecName)== 0 || s2.compare(clientMediaDetail.audioDetail.m_codecName)== 0)
            {
                clientMediaDetail.audioDetail.m_dynamicCodec = clientMediaDetail.audioDetail.m_codec;
                //let internal mapping be same as old one i.e. for opus
                //hard code to "111" and for external interfaces with client like
                //udpSrc use dynamic codec 96-127 as received from JSON message in "m_codec"
                clientMediaDetail.audioDetail.m_codec = EN_CODEC_OPUS;
            }
            continue;
        } else if(!strcmp(itr->name.GetString(), "ptime")) {
            clientMediaDetail.audioDetail.m_ptime = itr->value.GetUint();
            continue;
        } else if(!strcmp(itr->name.GetString(), "play_file")) {
            clientMediaDetail.audioDetail.m_playFile = itr->value.GetString();
            continue;
        } else if(!strcmp(itr->name.GetString(), "ip_addr")) {
            clientMediaDetail.audioDetail.m_clientIp = itr->value.GetString();
            continue;
        } else if (!strcmp(itr->name.GetString(), "ip_port")) {
            clientMediaDetail.audioDetail.m_clientPort = itr->value.GetUint();
            continue;
        } else if(!strcmp(itr->name.GetString(), "relayip_addr")) {
            clientMediaDetail.audioDetail.m_relayIp = itr->value.GetString();
            continue;
        } else if (!strcmp(itr->name.GetString(), "relay_port")) {
            clientMediaDetail.audioDetail.m_relayPort = itr->value.GetUint();
            continue;
        } else if(!strcmp(itr->name.GetString(), "reflexip_addr")) {
            clientMediaDetail.audioDetail.m_rflxIp = itr->value.GetString();
            continue;
        } else if (!strcmp(itr->name.GetString(), "reflex_port")) {
            clientMediaDetail.audioDetail.m_rflxPort = itr->value.GetUint();
            continue;
        } else if(!strcmp(itr->name.GetString(), "ice_ufrag")) {
            clientMediaDetail.audioDetail.m_secureDetail.m_rIceUfrag = itr->value.GetString();
            continue;
        } else if(!strcmp(itr->name.GetString(), "ice_pwd")) {
            clientMediaDetail.audioDetail.m_secureDetail.m_rIcePwd = itr->value.GetString();
            continue;
        } else if(!strcmp(itr->name.GetString(), "fingerprint")) {
            clientMediaDetail.audioDetail.m_secureDetail.m_rFingerPrint = itr->value.GetString();
            continue;
        } else if(!strcmp(itr->name.GetString(), "ssrc")) {
            clientMediaDetail.audioDetail.m_secureDetail.m_rSsrc = itr->value.GetString();
            continue;
        } else if(!strcmp(itr->name.GetString(), "cname")) {
            clientMediaDetail.audioDetail.m_secureDetail.m_rCname = itr->value.GetString();
            continue;
        } else if(!strcmp(itr->name.GetString(), "mslable")) {
            clientMediaDetail.audioDetail.m_secureDetail.m_rMsLabel = itr->value.GetString();
            continue;
        } else if(!strcmp(itr->name.GetString(), "lable")) {
            clientMediaDetail.audioDetail.m_secureDetail.m_rLabel = itr->value.GetString();
            continue;
        } else if(!strcmp(itr->name.GetString(), "dial_ip")) {
            clientMediaDetail.audioDetail.m_dialIp = itr->value.GetString();
            continue;
        } else if(!strcmp(itr->name.GetString(), "dial_port")) {
            clientMediaDetail.audioDetail.m_dialPort = itr->value.GetUint();
            continue;
        } else if(!strcmp(itr->name.GetString(), "resp_ip")) {
            clientMediaDetail.audioDetail.m_respIp = itr->value.GetString();
            continue;
        } else if(!strcmp(itr->name.GetString(), "resp_port")) {
            clientMediaDetail.audioDetail.m_respPort = itr->value.GetUint();
            continue;
        }
    }
    return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal SFUClient::handle_sdpinfo_video(const Value& sdpObj, MgMediaDetail& clientMediaDetail){
    unsigned int i = 1;

    for (Value::ConstMemberIterator itr = sdpObj[i].MemberBegin(); itr != sdpObj[i].MemberEnd(); itr++){

        if(!strcmp(itr->name.GetString(), "media_type")) {
            clientMediaDetail.videoDetail.m_mediaType = itr->value.GetUint();
        }

        if(clientMediaDetail.videoDetail.m_mediaType == 0) {
            break;
        }

        if(!strcmp(itr->name.GetString(), "fmtp")) {
            clientMediaDetail.videoDetail.m_fmtp = itr->value.GetString();
            continue;
        } else if(!strcmp(itr->name.GetString(), "codec")) {
            clientMediaDetail.videoDetail.m_codec = itr->value.GetUint();
            continue;
        } else if(!strcmp(itr->name.GetString(), "codec_name")) {
            clientMediaDetail.videoDetail.m_codecName = itr->value.GetString();
            continue;
        } else if(!strcmp(itr->name.GetString(), "framerate")) {
            clientMediaDetail.videoDetail.m_frameRate = itr->value.GetUint();
            continue;
        } else if(!strcmp(itr->name.GetString(), "imageattr_x")) {
            clientMediaDetail.videoDetail.m_imageattr_x = itr->value.GetUint();
            continue;
        } else if(!strcmp(itr->name.GetString(), "imageattr_y")) {
            clientMediaDetail.videoDetail.m_imageattr_y = itr->value.GetUint();
            continue;
        } else if(!strcmp(itr->name.GetString(), "play_file")) {
            clientMediaDetail.videoDetail.m_playFile = itr->value.GetString();
            continue;
        } else if(!strcmp(itr->name.GetString(), "ip_addr")) {
            clientMediaDetail.videoDetail.m_clientIp = itr->value.GetString();
            continue;
        } else if (!strcmp(itr->name.GetString(), "ip_port")) {
            clientMediaDetail.videoDetail.m_clientPort = itr->value.GetUint();
            continue;
        } else if(!strcmp(itr->name.GetString(), "relayip_addr")) {
            clientMediaDetail.videoDetail.m_relayIp = itr->value.GetString();
            continue;
        } else if (!strcmp(itr->name.GetString(), "relay_port")) {
            clientMediaDetail.videoDetail.m_relayPort = itr->value.GetUint();
            continue;
        } else if(!strcmp(itr->name.GetString(), "reflexip_addr")) {
            clientMediaDetail.videoDetail.m_rflxIp = itr->value.GetString();
            continue;
        } else if (!strcmp(itr->name.GetString(), "reflex_port")) {
            clientMediaDetail.videoDetail.m_rflxPort = itr->value.GetUint();
            continue;
        } else if(!strcmp(itr->name.GetString(), "ice_ufrag")) {
            clientMediaDetail.videoDetail.m_secureDetail.m_rIceUfrag = itr->value.GetString();
            continue;
        } else if(!strcmp(itr->name.GetString(), "ice_pwd")) {
            clientMediaDetail.videoDetail.m_secureDetail.m_rIcePwd = itr->value.GetString();
            continue;
        } else if(!strcmp(itr->name.GetString(), "fingerprint")) {
            clientMediaDetail.videoDetail.m_secureDetail.m_rFingerPrint = itr->value.GetString();
            continue;
        } else if(!strcmp(itr->name.GetString(), "ssrc")) {
            clientMediaDetail.videoDetail.m_secureDetail.m_rSsrc = itr->value.GetString();
            continue;
        } else if(!strcmp(itr->name.GetString(), "cname")) {
            clientMediaDetail.videoDetail.m_secureDetail.m_rCname = itr->value.GetString();
            continue;
        } else if(!strcmp(itr->name.GetString(), "mslable")) {
            clientMediaDetail.videoDetail.m_secureDetail.m_rMsLabel = itr->value.GetString();
            continue;
        } else if(!strcmp(itr->name.GetString(), "lable")) {
            clientMediaDetail.videoDetail.m_secureDetail.m_rLabel = itr->value.GetString();
            continue;
        } else if(!strcmp(itr->name.GetString(), "dial_ip")) {
            clientMediaDetail.videoDetail.m_dialIp = itr->value.GetString();
            continue;
        } else if(!strcmp(itr->name.GetString(), "dial_port")) {
            clientMediaDetail.videoDetail.m_dialPort = itr->value.GetUint();
            continue;
        } else if(!strcmp(itr->name.GetString(), "resp_ip")) {
            clientMediaDetail.videoDetail.m_respIp = itr->value.GetString();
            continue;
        } else if(!strcmp(itr->name.GetString(), "resp_port")) {
            clientMediaDetail.videoDetail.m_respPort = itr->value.GetUint();
            continue;
        }
    }
  // Incase codec number does not match codec name
   if( clientMediaDetail.videoDetail.m_codecName.compare("H264") == 0 || clientMediaDetail.videoDetail.m_codecName.compare("h264") == 0){
     clientMediaDetail.videoDetail.m_dynamicCodec = clientMediaDetail.videoDetail.m_codec;
     clientMediaDetail.videoDetail.m_codec = EN_CODEC_H264;
   } else if( clientMediaDetail.videoDetail.m_codecName.compare("H265") == 0 || clientMediaDetail.videoDetail.m_codecName.compare("h265") == 0 ){
      clientMediaDetail.videoDetail.m_dynamicCodec = clientMediaDetail.videoDetail.m_codec;
      clientMediaDetail.videoDetail.m_codec = EN_CODEC_H265;
   } else if( clientMediaDetail.videoDetail.m_codecName.compare("VP8") == 0 || clientMediaDetail.videoDetail.m_codecName.compare("vp8") == 0 ){
      clientMediaDetail.videoDetail.m_dynamicCodec = clientMediaDetail.videoDetail.m_codec;
      clientMediaDetail.videoDetail.m_codec = EN_CODEC_VP8;
   } else if( clientMediaDetail.videoDetail.m_codecName.compare("VP9") == 0 || clientMediaDetail.videoDetail.m_codecName.compare("vp9") == 0){
      clientMediaDetail.videoDetail.m_dynamicCodec = clientMediaDetail.videoDetail.m_codec;
      clientMediaDetail.videoDetail.m_codec = EN_CODEC_VP9;
   }
   return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal SFUClient::send_msg(xGateMgMsg *pMgMsg)
{
    //Make jsonData for other messages
    string jsonBuff("");
    int len = 0;
    int gatewayid = 0;
    xGateMgMsgType msgType = pMgMsg->get_mg_msg_type();
    xGateMgMsg* pMgMsg1 = NULL;
    xGateNetConnection netConInfo;
    pMgMsg1 = new xGateMgMsg();

    pMgMsg1->setMsgType(EN_XGATE_MSG_MGC_MG_IN);
    pMgMsg1->set_mg_msg_type(pMgMsg->get_mg_msg_type());
    pMgMsg1->set_media_detail(pMgMsg->get_media_detail());
    pMgMsg->get_net_con_info(netConInfo);
    pMgMsg1->set_net_con_info(netConInfo);

    len = create_media_msg(pMgMsg1, jsonBuff, gatewayid);
    if(len <= 0) {
        XGLOG_ERROR( "xGateMGDispatcher::encode_msg create_media_response_msg failed");
        return EN_XGATE_STATUS_ERROR;
    }

    //XGLOG_ERROR( "xGateMGDispatcher::encode_msg create_media_response: %s",jsonBuff.c_str());
    send_msg_to_mbc(jsonBuff.c_str(), len);
    delete pMgMsg1;
    return EN_XGATE_STATUS_SUCCESS;
}
xGateRetVal SFUClient::post_media_detail(xGateMgMsg *pMgMsg)
{
    xGateMgMsg* pMgMsg1 = NULL;
    xGateNetConnection netConInfo;
    pMgMsg->setMsgType(EN_XGATE_MSG_MGC_MG_IN);
    ACE_Task<ACE_MT_SYNCH>* taskPtr = NULL;
    // IMPORTANT NOTE: Sending message to
    taskPtr = xGateSFUtil::getSFUBridgeProcessor();
    if(!taskPtr) {
        XGLOG_ERROR( "MBCControllerProcessor::post_media_detail_sfu_bridge SFUBridgeProcessor task not found");
        return EN_XGATE_STATUS_ERROR;
    }

    ACE_Message_Block* pAmb = NULL;
    //pAmb = static_cast<xGateMgMsg*> (pMgMsg1);
    pAmb = static_cast<xGateMgMsg*> (pMgMsg);
    ACE_Time_Value tval ((time(NULL) ));
    tval += ACE_Time_Value(0,1);

    if (taskPtr->putq(pAmb, &tval)) {
    } else {
        XGLOG_ERROR( "MBCControllerProcessor::post_media_detail_sfu_bridge SFUBridgeProcessor enqueue message failed");
        //delete pMgMsg1;
        delete pMgMsg;
        return EN_XGATE_STATUS_ERROR;
    }
    return EN_XGATE_STATUS_SUCCESS;
}

int SFUClient::create_media_msg(xGateMgMsg *pMgMsg, string &jsonBuff, int &gatewayId)
{
   // XGLOG_INFO( "@@SFU Enter SFUClient::create_media_msg");
    Document doc;
    StringBuffer s;
    Writer<StringBuffer> writer(s);
    MgMediaDetail &mediaDetail = pMgMsg->get_media_detail();

    int len = 0;
    len = sizeof(json_mg_media_tmpl);
    char buff[len];
    memcpy(buff, json_mg_media_tmpl, len);

    if (doc.ParseInsitu(buff).HasParseError()) {
        XGLOG_ERROR( "xGateMGDispatcher::create_media_msg json parse error");
        return EN_XGATE_STATUS_ERROR;
    }
    if(!doc.IsObject()) {
        XGLOG_ERROR( "xGateMGDispatcher::create_media_msg doc object creation failed");
        return EN_XGATE_STATUS_ERROR;
    }

    Value valMsgType;
    valMsgType = pMgMsg->get_mg_msg_type();
    doc["msg_type"] = valMsgType;
    doc["data"]["mgresource_id"] = setJsonParam(mediaDetail.mgresource_id,doc);
    doc["data"]["call_type"] = setJsonParam(mediaDetail.call_type,doc);
    doc["data"]["call_id"] = setJsonParam(mediaDetail.call_id,doc);
    doc["data"]["gateway_id"] = setJsonParam(mediaDetail.gateway_id,doc);
    doc["data"]["context_id"] = setJsonParam(mediaDetail.context_id,doc);
	doc["data"]["sip_id"] = setJsonParam(mediaDetail.sip_id,doc);
    doc["data"]["leg_id"] = setJsonParam(mediaDetail.leg_id,doc);
    doc["data"]["retrieved_audiossrc"]= setJsonParam(mediaDetail.retrieved_audiossrc,doc);
    doc["data"]["retrieved_videossrc"]= setJsonParam(mediaDetail.retrieved_videossrc,doc);
    doc["data"]["media_proto"] = setJsonParam(mediaDetail.media_proto,doc);
    doc["data"]["out_proto"] = setJsonParam(mediaDetail.out_proto,doc);
    doc["data"]["record_file"] = setJsonParam(mediaDetail.record_file,doc);
    doc["data"]["browser_type"] = setJsonParam(mediaDetail.browser_type,doc);
    doc["data"]["record_url"] = setJsonParam(mediaDetail.record_url,doc);
    doc["data"]["meeting_name"] = setJsonParam(mediaDetail.meeting_name,doc);
    doc["data"]["file_size"] = setJsonParam(mediaDetail.file_size,doc);
    doc["data"]["sig_type"] = setJsonParam(mediaDetail.sig_type,doc);
    doc["data"]["call_dir"]= setJsonParam(mediaDetail.call_dir,doc);
    doc["data"]["d_r_e"]= setJsonParam(mediaDetail.d_rtp_ep,doc);
    doc["data"]["o_r_e"]= setJsonParam(mediaDetail.o_rtp_ep,doc);
    doc["data"]["call_Id_msg10"]= setJsonParam(mediaDetail.call_Id_msg10,doc);
    doc["data"]["screenshare_id"]= setJsonParam(mediaDetail.screenshare_id,doc);
    doc["data"]["pClient"]= setJsonParam(mediaDetail.pClient,doc);
    doc["data"]["dtmf_digits"]= setJsonParam(mediaDetail.dtmf_digits,doc);
    doc["data"]["joiner_type"]= setJsonParam(mediaDetail.joiner_type,doc);
    doc["data"]["media_state"] = setJsonParam(mediaDetail.media_event,doc);
    doc["data"]["sdp_str"] = setJsonParam(mediaDetail.sdp_str,doc);
    doc["data"]["sdpinfo"] = setJsonParam(mediaDetail,doc);
    doc["data"]["urmeet_user_list"] = setJsonParam_meetinfo(mediaDetail,doc);
    doc["data"]["prev_act_spkr"] = setJsonParam(mediaDetail.prev_act_spkr,doc);
    doc.Accept(writer);
    //get the length of created message
    len = s.GetLength();

    if ( len > 99999) {
        doc["msg_len"] = len+3;
    } else if ( len > 9999){
        doc["msg_len"] = len+2;
    } else if ( len > 999){
        doc["msg_len"] = len+1;
    } else {
        doc["msg_len"] = len;
    }
    s.Clear();
    doc.Accept(writer);
    puts(s.GetString());
    jsonBuff = s.GetString();
    string delimiter("\r\n\r\n");
    jsonBuff.append(delimiter.c_str(), delimiter.length());
    XGLOG_INFO( "SFUClient::create_media_msg created media msg_type: %d  with length of (%d),",
                               pMgMsg->get_mg_msg_type(), jsonBuff.length());
    return jsonBuff.length();
}
#if 1
Value SFUClient::setJsonParam_meetinfo(MgMediaDetail& detail, Document& m_doc)
{
    Value vArray(kArrayType);
    Document::AllocatorType& allocator = m_doc.GetAllocator();

    for (const auto& userInfo : detail.meetuser_info) {
        Value objectValue(kObjectType);
        objectValue.AddMember("email", setJsonParam(userInfo.email, m_doc), allocator);
        objectValue.AddMember("profile_img", setJsonParam(userInfo.profile_img, m_doc), allocator);
        objectValue.AddMember("sip_id", setJsonParam(userInfo.sip_id, m_doc), allocator);
        objectValue.AddMember("name", setJsonParam(userInfo.name, m_doc), allocator);
        objectValue.AddMember("ext", setJsonParam(userInfo.ext, m_doc), allocator);
        objectValue.AddMember("user_type", setJsonParam(userInfo.user_type, m_doc), allocator);
        objectValue.AddMember("role_type", setJsonParam(userInfo.role_type, m_doc), allocator);
        vArray.PushBack(objectValue, allocator);
    }

    return vArray;
}
#endif
Value SFUClient::setJsonParam(string strValue,Document &doc ){
    Value retVal;
    retVal.SetString(strValue.c_str(), strValue.length(), doc.GetAllocator());
    return retVal;
}
Value SFUClient::setJsonParam(unsigned long iValue, Document &doc){
    Value retVal;
    retVal = iValue;
    return retVal;
}
#if 0
Value SFUClient::setJsonParam(uint64_t iValue, Document &doc){
    Value retVal;
    retVal = iValue;
    return retVal;
}
#endif

Value SFUClient::setJsonParam(MgMediaDetail& media, Document &doc){
    Value vArray(kArrayType);
    Document::AllocatorType& allocate =  doc.GetAllocator();
    for (int i =0 ; i< 2 ; i++){
        Value objectValue;
        objectValue.SetObject();
        if(i == 0 && media.audioDetail.m_mediaType == 2) {
            objectValue.AddMember("media_type",setJsonParam(media.audioDetail.m_mediaType,doc),allocate);
            objectValue.AddMember("fmtp",setJsonParam(media.audioDetail.m_fmtp,doc),allocate);
            objectValue.AddMember("codec",setJsonParam(media.audioDetail.m_codec,doc),allocate);
            objectValue.AddMember("codec_name",setJsonParam(media.audioDetail.m_codecName,doc),allocate);
            objectValue.AddMember("ptime",setJsonParam(media.audioDetail.m_ptime,doc),allocate);
            objectValue.AddMember("play_file",setJsonParam(media.audioDetail.m_playFile,doc),allocate);
            objectValue.AddMember("ip_addr",setJsonParam(media.audioDetail.m_clientIp,doc),allocate);
            objectValue.AddMember("ip_port",setJsonParam(media.audioDetail.m_clientPort,doc),allocate);
            objectValue.AddMember("relayip_addr",setJsonParam(media.audioDetail.m_relayIp,doc),allocate);
            objectValue.AddMember("relay_port",setJsonParam(media.audioDetail.m_relayPort,doc),allocate);
            objectValue.AddMember("reflexip_addr",setJsonParam(media.audioDetail.m_rflxIp,doc),allocate);
            objectValue.AddMember("reflex_port",setJsonParam(media.audioDetail.m_rflxPort,doc),allocate);
            objectValue.AddMember("ice_ufrag",setJsonParam(media.audioDetail.m_secureDetail.m_lIceUfrag,doc),allocate);
            objectValue.AddMember("ice_pwd",setJsonParam(media.audioDetail.m_secureDetail.m_lIcePwd,doc),allocate);
            objectValue.AddMember("fingerprint",setJsonParam(media.audioDetail.m_secureDetail.m_lFingerPrint,doc),allocate);
            objectValue.AddMember("ssrc",setJsonParam(media.audioDetail.m_secureDetail.m_lSsrc,doc),allocate);
            objectValue.AddMember("cname",setJsonParam(media.audioDetail.m_secureDetail.m_lCname,doc),allocate);
            objectValue.AddMember("mslable",setJsonParam(media.audioDetail.m_secureDetail.m_lMsLabel,doc),allocate);
            objectValue.AddMember("lable",setJsonParam(media.audioDetail.m_secureDetail.m_lLabel,doc),allocate);
            objectValue.AddMember("dial_ip",setJsonParam(media.audioDetail.m_dialIp,doc),allocate);
            objectValue.AddMember("dial_port",setJsonParam(media.audioDetail.m_dialPort,doc),allocate);
            objectValue.AddMember("resp_ip",setJsonParam(media.audioDetail.m_respIp,doc),allocate);
            objectValue.AddMember("resp_port",setJsonParam(media.audioDetail.m_respPort,doc),allocate);
            vArray.PushBack(objectValue,allocate);
        } else if(i == 1 && media.videoDetail.m_mediaType == 3) {
            objectValue.AddMember("media_type",setJsonParam(media.videoDetail.m_mediaType,doc),allocate);
            objectValue.AddMember("fmtp",setJsonParam(media.videoDetail.m_fmtp,doc),allocate);
            objectValue.AddMember("codec",setJsonParam(media.videoDetail.m_codec,doc),allocate);
            objectValue.AddMember("codec_name",setJsonParam(media.videoDetail.m_codecName,doc),allocate);
            objectValue.AddMember("framerate",setJsonParam(media.videoDetail.m_frameRate,doc),allocate);
            objectValue.AddMember("imageattr_x",setJsonParam(media.videoDetail.m_imageattr_x,doc),allocate);
            objectValue.AddMember("imageattr_y",setJsonParam(media.videoDetail.m_imageattr_y,doc),allocate);
            objectValue.AddMember("play_file",setJsonParam(media.videoDetail.m_playFile,doc),allocate);
            objectValue.AddMember("ip_addr",setJsonParam(media.videoDetail.m_clientIp,doc),allocate);
            objectValue.AddMember("ip_port",setJsonParam(media.videoDetail.m_clientPort,doc),allocate);
            objectValue.AddMember("relayip_addr",setJsonParam(media.videoDetail.m_relayIp,doc),allocate);
            objectValue.AddMember("relay_port",setJsonParam(media.videoDetail.m_relayPort,doc),allocate);
            objectValue.AddMember("reflexip_addr",setJsonParam(media.videoDetail.m_rflxIp,doc),allocate);
            objectValue.AddMember("reflex_port",setJsonParam(media.videoDetail.m_rflxPort,doc),allocate);
            objectValue.AddMember("ice_ufrag",setJsonParam(media.videoDetail.m_secureDetail.m_lIceUfrag,doc),allocate);
            objectValue.AddMember("ice_pwd",setJsonParam(media.videoDetail.m_secureDetail.m_lIcePwd,doc),allocate);
            objectValue.AddMember("fingerprint",setJsonParam(media.videoDetail.m_secureDetail.m_lFingerPrint,doc),allocate);
            objectValue.AddMember("ssrc",setJsonParam(media.videoDetail.m_secureDetail.m_lSsrc,doc),allocate);
            objectValue.AddMember("cname",setJsonParam(media.videoDetail.m_secureDetail.m_lCname,doc),allocate);
            objectValue.AddMember("mslable",setJsonParam(media.videoDetail.m_secureDetail.m_lMsLabel,doc),allocate);
            objectValue.AddMember("lable",setJsonParam(media.videoDetail.m_secureDetail.m_lLabel,doc),allocate);
            objectValue.AddMember("dial_ip",setJsonParam(media.videoDetail.m_dialIp,doc),allocate);
            objectValue.AddMember("dial_port",setJsonParam(media.videoDetail.m_dialPort,doc),allocate);
            objectValue.AddMember("resp_ip",setJsonParam(media.videoDetail.m_respIp,doc),allocate);
            objectValue.AddMember("resp_port",setJsonParam(media.videoDetail.m_respPort,doc),allocate);
            vArray.PushBack(objectValue,allocate);
        }
    }
    return vArray;
}

