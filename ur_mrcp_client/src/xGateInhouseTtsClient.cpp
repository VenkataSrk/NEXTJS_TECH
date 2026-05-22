#include "xGateInhouseTtsClient.h"
#include "URMRCPModuleBridge.h"

SttTtsConnInfo xGateInhouseTtsClient::m_ttsConnInfo;

xGateInhouseTtsClient::xGateInhouseTtsClient(STR callid,STR uid,UrMrcpSessionDetails *tSessionDetails)
  :m_callid(callid),m_uid(uid),isSynthStarted(false),isSynthInprogress(false),m_tSessionDetails(tSessionDetails)
{
  XGLOG_FUNCTION_EN_EX("");
  m_ttsConnInfo = MRCP_CONFIG::instance()->m_ttsProfile.m_inhouseConnInfo;
  if(m_tSessionDetails)
  {
    m_fd = m_tSessionDetails->m_ttsWriteFd;
  }
  else
  {
    XGLOG_ERROR("xGateInhouseTtsClient received invalid Session Details for callid(%s)",(char*)ur_log_string(m_callid));
  }
  //To connect with server
  std::thread(&xGateInhouseTtsClient::tts_client_websocket_open,this).detach();
}

xGateInhouseTtsClient::~xGateInhouseTtsClient()
{
  //To cancel the websocket read thread
  ACE_Thread_Manager::instance()->cancel(m_tid,1);
  //To close the connection with server
  tts_client_websocket_close();
  isSynthStarted = false;
  isSynthInprogress = false;
  m_uid = "";
  m_callid = "";
}

VOID_TTSCLIENT::tts_client_websocket_open()
{
  XGLOG_FUNCTION_EN_EX("");
  XGLOG_INFO("xGateInhouseTtsClient::tts_client_websocket_open for callid(%s)", (char*)ur_log_string(m_callid));

   STR host(""), port(""), str("");
   host.assign(m_ttsConnInfo.m_strServerHostIp.c_str());
   port = m_ttsConnInfo.m_strServerPort.c_str();
   std::string initText = "{\"ip\":\""+m_tSessionDetails->m_mrcpMgIp+"\",\"port\":"+m_tSessionDetails->m_mrcpMgTtsPort+"}";
   XGLOG_INFO("xGateInhouseTtsClient::tts_client_websocket_open ConnectIp (%d)",m_ttsConnInfo.m_eConnectType);

  try{
    auto const results = resolver.resolve(host, port);
    if(m_ttsConnInfo.m_eConnectType == EN_ENGINE_CONNECT_TYPE::EN_ENGINE_CONNECT_TYPE_IP)
    {
      XGLOG_INFO("xGateInhouseTtsClient::tts_client_websocket_open using ip");
      auto endPoint = boost::asio::connect(ws.next_layer(), results.begin(),results.end());
      host.append(":");
      host.append(port);
      ws.handshake(host, "/");
      ws.write(boost::asio::buffer(std::string(initText)));
      buff.consume(buff.size());
      ws.read(buff);
      str = boost::beast::buffers_to_string(buff.data()).c_str();
    }
    else if(m_ttsConnInfo.m_eConnectType == EN_ENGINE_CONNECT_TYPE::EN_ENGINE_CONNECT_TYPE_URL)
    {
      XGLOG_INFO("xGateInhouseTtsClient::tts_client_websocket_open using url");
      auto endPoint = boost::asio::connect(wss.next_layer().next_layer(), results.begin(),results.end());
      host.append(":");
      host.append(port);
      wss.handshake(host, "/");
      wss.write(boost::asio::buffer(std::string(initText)));
      buff.consume(buff.size());
      wss.read(buff);
      str = boost::beast::buffers_to_string(buff.data()).c_str();
    }
    std::cout << "tts_client_websocket_open connection Established !!!" << std::endl;
    XGLOG_INFO("xGateInhouseTtsClient::tts_client_websocket_open connection Established for callid(%s)", (char*)ur_log_string(m_callid));
  
    std::cout<<"received msg "<<str<<std::endl;

    post_message_to_bridgemodule(EN_STT_TTS_MSG_ALLOCATE_RES);
  }
  catch(boost::system::system_error const& sysErr)
  {
    boost::system::error_code error = sysErr.code();
    std::cerr << "tts_client_websocket_open Error: "<< error.message() << std::endl;
    XGLOG_ERROR("xGateInhouseTtsClient::tts_client_websocket_open error message (%s)",(char*)ur_log_string(error.message()));
  }
}

BOOL_TTSCLIENT::send_synth_request(STR text)
{
  if(text.empty())
  {
    XGLOG_DEBUG("xGateInhouseTtsClient::send_synth_request received empty text for callid (%s)",(char*)ur_log_string(m_callid));
    return false;
  }

#if 0
  if(isSynthStarted)
  {
    XGLOG_DEBUG("xGateInhouseTtsClient::send_synth_request sending channel tts stop request for callid (%s)",(char*)ur_log_string(m_callid));
    post_message_to_bridgemodule(EN_TTS_CHANNEL_STOP_REQ);
  }
  else
  {
    isSynthStarted = true;
  }
#endif

  STR strReq("");
  strReq = encode_request_msg(text);
  try
  {
    ACE_Thread_Manager::instance()->cancel(m_tid,1);
    if(m_ttsConnInfo.m_eConnectType == EN_ENGINE_CONNECT_TYPE::EN_ENGINE_CONNECT_TYPE_IP)
    {
      ws.write(boost::asio::buffer(strReq));
    }
    else if(m_ttsConnInfo.m_eConnectType == EN_ENGINE_CONNECT_TYPE::EN_ENGINE_CONNECT_TYPE_URL)
    {
      wss.write(boost::asio::buffer(strReq));
    }
    XGLOG_INFO("xGateInhouseTtsClient::send_synth_request message (%s) sent for callid(%s)",(char*)ur_log_string(strReq),(char*)ur_log_string(m_callid));
    ACE_Thread_Manager::instance()->spawn((ACE_THR_FUNC)&xGateInhouseTtsClient::tts_server_response,this,THR_NEW_LWP|THR_JOINABLE ,&m_tid);
    return true;
  }
  catch(boost::system::system_error const& sysErr)
  {
    boost::system::error_code error = sysErr.code();
    std::cerr << "send_synth_request Error: "<< error.message() << std::endl;
    XGLOG_ERROR("xGateInhouseTtsClient::send_synth_request error message (%s)",(char*)ur_log_string(error.message()));
  }
  return false;
}

STR_TTSCLIENT::encode_request_msg(STR text)
{
  Document doc;
  StringBuffer strBuf;
  Writer<StringBuffer> writer(strBuf);
  Value outputVal(kObjectType);
  Document::AllocatorType& allocator = doc.GetAllocator();

  outputVal.AddMember("text",setJsonParam(text, doc),allocator);

  outputVal.Accept(writer);
  string oss("");
  oss = strBuf.GetString();
  return oss;
}

Value xGateInhouseTtsClient::setJsonParam(string &strValue, Document &doc)
{
  Value retVal;
  retVal.SetString(strValue.c_str(), strValue.length(), doc.GetAllocator());
  return retVal;
}

Value xGateInhouseTtsClient::setJsonParam(unsigned int iValue, Document &doc)
{
  Value retVal;
  retVal = iValue;
  return retVal;
}

VOID_TTSCLIENT::tts_server_response(void *data)
{
  if(!data)
  {
    XGLOG_ERROR("xGateInhouseTtsClient::tts_server_response invalid client obj");
    return;
  }
  xGateInhouseTtsClient *ttsCli = data;
  try{
    XGLOG_INFO("xGateInhouseTtsClient::tts_server_response read for callid(%s)",(char*)ur_log_string(ttsCli->m_callid));
    while(true)
    {
      ttsCli->buff.consume(ttsCli->buff.size());
      if(m_ttsConnInfo.m_eConnectType == EN_ENGINE_CONNECT_TYPE::EN_ENGINE_CONNECT_TYPE_IP)
      {
        ttsCli->ws.read(ttsCli->buff);
      }
      else if(m_ttsConnInfo.m_eConnectType == EN_ENGINE_CONNECT_TYPE::EN_ENGINE_CONNECT_TYPE_URL)
      {
        ttsCli->wss.read(ttsCli->buff);
      }
      //std::cout<<boost::beast::make_printable(ttsCli->buff.data())<<std::endl;
      cout<<"xGateInhouseTtsClient::tts_server_response received response "<<ttsCli->buff.size()<<endl;
      XGLOG_INFO("xGateInhouseTtsClient::tts_server_response received response buffer size is (%d) for callid(%s)",ttsCli->buff.size() ,(char*)ur_log_string(ttsCli->m_callid));
      if(ttsCli->process_message_from_server(ttsCli->buff))
      {
        break;
      }
    }
  }
  catch(boost::system::system_error const& sysErr)
  {
    boost::system::error_code error = sysErr.code();
    std::cerr << "tts_server_response Error: "<<error.message() << std::endl;
    XGLOG_ERROR("xGateInhouseTtsClient::tts_server_response error message (%s)", (char*)ur_log_string(error.message()));
  }
}

BOOL_TTSCLIENT::process_message_from_server(boost::beast::flat_buffer buffer)
{
  STR tMessage = boost::beast::buffers_to_string(buffer.data()).c_str();
  if(tMessage.find("status") != STR::npos)
  {
    XGLOG_INFO("xGateInhouseTtsClient::process_message_from_server received response(%s) for callid(%s)",(char*)ur_log_string(tMessage),(char*)ur_log_string(m_callid));
    ttsServerResponse responseMsg;
    if(decode_message_from_server(tMessage, responseMsg))
    {
      switch(responseMsg.refStatus())
      {
        case EN_SYNTH_STATUS::EN_SYNTH_COMPLETE:
          {
            XGLOG_DEBUG("xGateInhouseTtsClient::process_message_from_server received EN_SYNTH_COMPLETE for callid(%s)",(char*)ur_log_string(m_callid));
            post_message_to_bridgemodule(EN_TTS_SPEAK_COMPLETE);
            return true;
            UrMrcpSessionDetails *sessionInfo = NULL;
            std::map<std::string,UrMrcpSessionDetails *>::iterator it = URMRCPMODULEBRIDGE->m_sessionDetailMap.find(m_callid);
            if(it != URMRCPMODULEBRIDGE->m_sessionDetailMap.end() && it->second)
            {
              sessionInfo = it->second;
            }
            else
            {
              XGLOG_ERROR("xGateInhouseTtsClient::process_message_from_server session info not found for callid(%s)",(char*)ur_log_string(m_callid));
              return false;
            }
            if(!sessionInfo->m_avaamo_reply.empty())
            {
              std::list<bot_reply_msg>::iterator itr = sessionInfo->m_avaamo_reply.begin();
              if(itr != sessionInfo->m_avaamo_reply.end())
              {
                sessionInfo->m_playing = (*itr).m_bOrignal;
                send_synth_request((*itr).m_strMessage);
                sessionInfo->m_avaamo_reply.erase(itr);
                return false;
              }
            }
            XGLOG_DEBUG("xGateInhouseTtsClient::process_message_from_server sending EN_TTS_SPEAK_COMPLETE for callid(%s)",(char*)ur_log_string(m_callid));
            post_message_to_bridgemodule(EN_TTS_SPEAK_COMPLETE);
            return true;
            break;
          }
        case EN_SYNTH_STATUS::EN_SYNTH_INPROGRESS:
          {
            XGLOG_DEBUG("xGateInhouseTtsClient::process_message_from_server received EN_SYNTH_INPROGRESS for callid(%s)",(char*)ur_log_string(m_callid));
            //post_message_to_bridgemodule(EN_TTS_CHANNEL_START_REQ);
            isSynthInprogress = false;
            break;
          }
        case EN_SYNTH_STATUS::EN_SYNTH_PENDING:
          {
            XGLOG_DEBUG("xGateInhouseTtsClient::process_message_from_server received EN_SYNTH_PENDING for callid(%s)",(char*)ur_log_string(m_callid));
            break;
          }
      }
    }
    else
    {
      XGLOG_DEBUG("xGateInhouseTtsClient::process_message_from_server decode_message_from_server failed for callid(%s)",(char*)ur_log_string(m_callid));
    }
  }
  else
  {
    XGLOG_DEBUG("xGateInhouseTtsClient::process_message_from_server received data writing to fd for callid(%s)",(char*)ur_log_string(m_callid));
    std::cout <<"!! Writing the data !!! "<<isSynthInprogress<<std::endl;
#if 0
    if(!isSynthInprogress)
    {
      isSynthInprogress = true;
      std::cout<<"!!! xGateInhouseTtsClient::process_message_from_server received EN_SYNTH_INPROGRESS and sending EN_TTS_CHANNEL_START_REQ "<<std::endl;
      XGLOG_DEBUG("xGateInhouseTtsClient::process_message_from_server sending EN_TTS_CHANNEL_START_REQ for callid(%s)",(char*)ur_log_string(m_callid));
      post_message_to_bridgemodule(EN_TTS_CHANNEL_START_REQ);
    }
#endif
    //To send recieved data to the mg
    write(m_fd, boost::asio::buffer_cast<const char*>(boost::beast::buffers_front(buff.data())), boost::asio::buffer_size(buff.data()));
  }
  return false;
}

BOOL_TTSCLIENT::decode_message_from_server(STR jsonData, ttsServerResponse &resMsg)
{
  Document doc;
  ParseResult ok = doc.Parse(jsonData.c_str());
  if(!ok) {
    XGLOG_ERROR( "xGateInhouseTtsClient::decode_message_from_server parseJsonMsg failed. Encountered with json parse error !");
    return false;
  } else if(!doc.IsObject()) {
    XGLOG_ERROR( "xGateInhouseTtsClient::decode_message_from_server parseJsonMsg failed. Not able create documnet object !");
    return false;
  }

  if(doc.HasMember("status")) 
  {
    if(doc["status"].GetString())
    {
      STR tStatus  = doc["status"].GetString();
      if(tStatus == "Inprogress"){
        resMsg.refStatus() = EN_SYNTH_STATUS::EN_SYNTH_INPROGRESS;
      } else if(tStatus == "Completed"){
        resMsg.refStatus() = EN_SYNTH_STATUS::EN_SYNTH_COMPLETE;
      } else if(tStatus == "Pending"){
        resMsg.refStatus() = EN_SYNTH_STATUS::EN_SYNTH_PENDING;
      } else {
        XGLOG_ERROR( "xGateInhouseTtsClient::decode_message_from_server received unknown status(%s) for callid(%s)",(char*)ur_log_string(tStatus),(char*)ur_log_string(m_callid));
      }
    }
  }
  else
  {
    XGLOG_ERROR("xGateInhouseTtsClient::decode_message_from_server failed. 'status' detail is missing in json message !");
    return false;
  }

  if(doc.HasMember("error_code"))
  {
    resMsg.refErrorCode() = doc["error_code"].GetInt();
    if(doc.HasMember("error_message"))
    {
      resMsg.refErrorMessage() = doc["error_message"].GetString();
    }
    else 
    {
      XGLOG_ERROR("xGateInhouseTtsClient::decode_message_from_server failed. 'error_message' detail is missing in json message !");
      return false;
    }
  }
  else 
  {
    XGLOG_ERROR("xGateInhouseTtsClient::decode_message_from_server failed. 'error_code' detail is missing in json message !");
    return false;
  }

  return true;
}

BOOL_TTSCLIENT::stop_synth_request()
{
  return true;
}

VOID_TTSCLIENT::tts_client_websocket_close()
{
  XGLOG_FUNCTION_EN_EX("");
  XGLOG_INFO("xGateInhouseTtsClient::tts_client_websocket_close closing connection for callid (%s)" , (char*)ur_log_string(m_callid));
  try{
    if(m_ttsConnInfo.m_eConnectType == EN_ENGINE_CONNECT_TYPE::EN_ENGINE_CONNECT_TYPE_IP)
    {
      ws.close(websocket::close_code::normal);
    }
    else if(m_ttsConnInfo.m_eConnectType == EN_ENGINE_CONNECT_TYPE::EN_ENGINE_CONNECT_TYPE_URL)
    {
      wss.close(websocket::close_code::normal);
    }
  }
  catch(boost::system::system_error const& sysErr)
  {
    boost::system::error_code error = sysErr.code();
    std::cerr << "tts_client_websocket_close Error: "<<error.message() << std::endl;
    XGLOG_ERROR("xGateInhouseTtsClient::tts_client_websocket_close error message (%s)", (char*)ur_log_string(error.message()));
  }
}

BOOL_TTSCLIENT::post_message_to_bridgemodule(xGateMrcpCommand command)
{
  xGateDbConfigMgrServiceMsg *pServiceMsg = new xGateDbConfigMgrServiceMsg();
  if(!pServiceMsg)
  {
    XGLOG_ERROR("xGateInhouseTtsClient::post_message_to_bridgemodule failed to create service msg");
    return false;
  }
  pServiceMsg->m_callId = m_callid.c_str();
  pServiceMsg->set_mrcp_command(command);
  UR_MRCP_MODULE_BRIDGE::instance()->PostMessageToMrcpMg(pServiceMsg);
  return true;
}
