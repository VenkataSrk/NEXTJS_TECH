#include "xGateInhouseTtsClient.h"
#include "URMRCPModuleBridge.h"

SttTtsConnInfo xGateInhouseTtsClient::m_ttsConnInfo;

xGateInhouseTtsClient::xGateInhouseTtsClient(STR callid,STR uid,UrMrcpSessionDetails *tSessionDetails)
  :m_callid(callid),m_uid(uid),isSynthStarted(false),isSynthInprogress(false),m_tSessionDetails(tSessionDetails),m_iRetryCount(0),ws(std::move(create_socket(uid, callid)))
{
  XGLOG_FUNCTION_EN_EX("");
  XGLOG_INFO("xGateInhouseTtsClient CONSTRUCTOR called with IP uId[%s] callId[%s]",m_uid.c_str(),m_callid.c_str());
  m_iRetryCount = getTtsConnectRetryCount(m_callid);
  m_ttsConnInfo = MRCP_CONFIG::instance()->m_ttsProfile.m_inhouseConnInfo;
  if(m_tSessionDetails)
  {
    m_fd = m_tSessionDetails->m_ttsWriteFd;
    //To connect with server
    if(m_bIsSocketConnected == true)
    {
        std::thread(&xGateInhouseTtsClient::tts_client_websocket_open,this).detach();
    }
    else
    {
        if(m_iRetryCount!=0)
        {
            XGLOG_INFO("xGateInhouseTtsClient CONSTRUCTOR called with IP trying to reconnect uId[%s] callId[%s] retryCount[%d]",m_uid.c_str(),m_callid.c_str(),m_iRetryCount);
            m_iRetryCount--;
            setTtsConnectRetryCount(m_callid,m_iRetryCount);
            // post_message_to_bridgemodule(EN_TTS_RECONNECTION);
            URMRCPMODULEBRIDGE->postMessageToSchedulerService(m_callid.c_str(), EN_SCHEDULER_TTS_RECONNECT_TIMER, SCHEDULER_FUNCTION_SET_TIMER);
        }
    }
  }
  else
  {
    XGLOG_ERROR("xGateInhouseTtsClient received invalid Session Details for callid(%s)",(char*)ur_log_string(m_callid));
  }
}

xGateInhouseTtsClient::xGateInhouseTtsClient(STR callid,STR uid,UrMrcpSessionDetails *tSessionDetails,EN_ENGINE_CONNECT_TYPE engineType)
  :m_callid(callid),m_uid(uid),isSynthStarted(false),isSynthInprogress(false),m_tSessionDetails(tSessionDetails),m_iRetryCount(0),wss(std::move(create_socket(uid, callid)),ctx)
{
  XGLOG_FUNCTION_EN_EX("");
  XGLOG_INFO("xGateInhouseTtsClient CONSTRUCTOR called with URL uId[%s] callId[%s]",m_uid.c_str(),m_callid.c_str());
  m_iRetryCount = getTtsConnectRetryCount(m_callid);
  m_ttsConnInfo = MRCP_CONFIG::instance()->m_ttsProfile.m_inhouseConnInfo;
  if(m_tSessionDetails)
  {
    m_fd = m_tSessionDetails->m_ttsWriteFd;
    //To connect with server
    if(m_bIsSocketConnected == true)
    {
        std::thread(&xGateInhouseTtsClient::tts_client_websocket_open,this).detach();
    }
    else
    {
        if(m_iRetryCount!=0)
        {
            // sleep(2);
            XGLOG_INFO("xGateInhouseTtsClient CONSTRUCTOR called with URL trying to reconnect uId[%s] callId[%s] retryCount[%d]",m_uid.c_str(),m_callid.c_str(),m_iRetryCount);
            m_iRetryCount--;
            setTtsConnectRetryCount(m_callid,m_iRetryCount);
            // post_message_to_bridgemodule(EN_TTS_RECONNECTION);
            URMRCPMODULEBRIDGE->postMessageToSchedulerService(m_callid.c_str(), EN_SCHEDULER_TTS_RECONNECT_TIMER, SCHEDULER_FUNCTION_SET_TIMER);
        }
    }
  }
  else
  {
    XGLOG_ERROR("xGateInhouseTtsClient received invalid Session Details for callid(%s)",(char*)ur_log_string(m_callid));
  }
}

tcp::socket xGateInhouseTtsClient::create_socket(STR uid, STR callid)
{
    try
    {
        m_ttsConnInfo = MRCP_CONFIG::instance()->m_ttsProfile.m_inhouseConnInfo;
        STR host("");
        tcp::socket socket(ioc);
        host.assign(MRCP_CONFIG::instance()->m_ttsProfile.m_strLocalIp.c_str());
        XGLOG_INFO("xGateInhouseTtsClient::create_socket() SttTtsMg localHost[%s]",host.c_str());
        tcp::endpoint localEndpoint(boost::asio::ip::address::from_string(host), 0);
        socket.open(localEndpoint.protocol());
        socket.bind(localEndpoint);

        STR serverPort{""};
        serverPort = getPort(callid);
        
        if(m_ttsConnInfo.m_eConnectType == EN_ENGINE_CONNECT_TYPE::EN_ENGINE_CONNECT_TYPE_IP)
        {
            XGLOG_INFO("xGateInhouseTtsClient::create_socket EN_ENGINE_CONNECT_TYPE_IP async_connect Host[%s] Port[%s]",m_ttsConnInfo.m_strServerHostIp.c_str(),serverPort.c_str());
            tcp::endpoint Endpoint(boost::asio::ip::address::from_string(m_ttsConnInfo.m_strServerHostIp.c_str()), atoi(serverPort.c_str()));
            socket.async_connect(Endpoint, connect_handler); // will connect to the tts socket asynchronously.
            m_bIsSocketConnected = true;
        }
        else if(m_ttsConnInfo.m_eConnectType == EN_ENGINE_CONNECT_TYPE::EN_ENGINE_CONNECT_TYPE_URL)
        {
            XGLOG_INFO("xGateInhouseTtsClient::create_socket EN_ENGINE_CONNECT_TYPE_URL async_connect Host[%s] Port[%s]",m_ttsConnInfo.m_strServerHostUrl.c_str(),serverPort.c_str());
            tcp::resolver::query query(m_ttsConnInfo.m_strServerHostUrl.c_str(), serverPort.c_str());
            tcp::endpoint Endpoint = *resolver.resolve(query);
            socket.async_connect(Endpoint, connect_handler); // will connect to the tts socket asynchronously.
            m_bIsSocketConnected = true;
        }

        XGLOG_DEBUG("xGateInhouseTtsClient::create_socket socket.local_endpoint().port() ==> %d, uId:%s callId:%s",socket.local_endpoint().port(),(char*)ur_log_string(uid),(char*)ur_log_string(callid));
        
        // Set required socket options
        socket.set_option(boost::asio::socket_base::reuse_address(true));  // Allows socket reuse
        socket.set_option(boost::asio::ip::tcp::no_delay(true));           // Disables Nagle's algorithm (low latency)
        socket.set_option(boost::asio::socket_base::keep_alive(true));     // Enables TCP keep-alive

        // Set SO_LINGER (force close after timeout)
        struct linger so_linger_option = {1, 10};  // (1 = enable, 0 = timeout in seconds)
        socket.set_option(boost::asio::socket_base::linger(so_linger_option.l_onoff, so_linger_option.l_linger));

        // socket.connect(Endpoint);
        return socket;
    }
    catch(std::exception const& exp)
    {
        XGLOG_INFO("xGateInhouseTtsClient::create_socket error message (%s) uId:%s callId:%s",exp.what(),(char*)ur_log_string(uid),(char*)ur_log_string(callid));
        std::cout << "xGateInhouseTtsClient::create_socket Error: " << exp.what() << std::endl;
        m_bIsSocketConnected = false;
        tcp::socket socket(ioc);
        return socket;
    }
}

void xGateInhouseTtsClient::connect_handler(const boost::system::error_code& ec) 
{
    std::cout << "!! Error code - " << ec << std::endl;
    if (!ec) 
    {
        // Connection succeeded^M
        std::cout << "!! tts Connected!" << std::endl;
        XGLOG_INFO("xGateInhouseTtsClient::connect_handler Connection successful");
    }
    else
    {
        // Connection failed^M
        std::cout << "!! Error: " << ec.message() << std::endl;
        XGLOG_INFO("xGateInhouseTtsClient::connect_handler Connection failed Error ==> [%s]",ec.message());
    }
}

void xGateInhouseTtsClient::stop()
{
    std::cout << "xGateInhouseTtsClient::stop()" << std::endl;
    XGLOG_INFO("xGateInhouseTtsClient::stop() uId[%s] callId[%s]",m_uid.c_str(),m_callid.c_str());
    //To cancel the websocket read thread
    m_bIsUserInCall = false;
    if(m_bIsSocketConnected == true)
    {
        tts_client_websocket_close();
    }
    isSynthStarted = false;
    isSynthInprogress = false;
    m_uid = "";
    m_callid = "";
    if (m_tid > 0)
    {
        XGLOG_INFO("xGateInhouseTtsClient: BFR cancelling ACE_thr for tid: %lu" , m_tid);
        // ACE_Thread_Manager::instance()->cancel(m_tid,1);
        XGLOG_INFO("xGateInhouseTtsClient: calling join for ACE_thr for tid: %lu" , m_tid);
        ACE_Thread_Manager::instance()->join(m_tid);
        XGLOG_INFO("xGateInhouseTtsClient: AFTER calling join for ACE_thr for tid: %lu" , m_tid);
    }
    //To close the connection with server
}

xGateInhouseTtsClient::~xGateInhouseTtsClient()
{
    //XGLOG_FUNCTION_EN_EX("");
//    ACE_THR_FUNC_RETURN status = 0;
   XGLOG_INFO("xGateInhouseTtsClient::Entered destructor () for callid (%s) ",(char*)ur_log_string(m_callid));
}

VOID_TTSCLIENT::tts_client_websocket_open()
{
  XGLOG_FUNCTION_EN_EX("");

   STR host(""), port(""), str("");
    host.assign((m_ttsConnInfo.m_eConnectType == EN_ENGINE_CONNECT_TYPE::EN_ENGINE_CONNECT_TYPE_IP) ? m_ttsConnInfo.m_strServerHostIp.c_str() : m_ttsConnInfo.m_strServerHostUrl.c_str());
   //std::string initText = "{\"ip\":\""+m_tSessionDetails->m_mrcpMgIp+"\",\"port\":"+m_tSessionDetails->m_mrcpMgTtsPort+"}";
   UrMrcpSessionDetails* sessionDetails = nullptr;
   if(!URMRCPMODULEBRIDGE->getSessionInfo(m_callid, sessionDetails))
   {
    XGLOG_ERROR("xGateInhouseTtsClient::tts_client_websocket_open SESSION NOT FOUND callId:%s",m_callid.c_str());
    return;
	 }
	 stringstream oss;
	 STR initText{""};
	 oss << "{\"" << "session_id" << "\":" << "\"" << m_uid << "\"," 
		 << "\"" << "ip" << "\":" << "\"" << m_tSessionDetails->m_mrcpMgIp << "\","
		 << "\"" << "port" << "\":" << m_tSessionDetails->m_mrcpMgTtsPort << ","
		//  << "\"" << "ccaasNumber" << "\":" << "\"" << ((sessionDetails->m_strCcaasDdi.empty())?"0":sessionDetails->m_strCcaasDdi.c_str()) << "\"}";
        << "\"" << "ccaasNumber" << "\":" << "\"" << ((sessionDetails->m_strCcaasDdi.empty())?"0":sessionDetails->m_strCcaasDdi.c_str()) << "\"}";		 
			 
	initText.assign(oss.str());
#if 0
	 unsigned short isBot(0);
	 isBot = (sessionDetails->m_bIsChatBotInit==true) ? 1 : (sessionDetails->m_iIsCcaasAutoAttendant==1) ? 1 : (sessionDetails->m_bIsCcaasVirtualAgent==true) ? 1 : 0;
	 XGLOG_INFO("##### ISBOT - %d",isBot);
	 std::string initText = "{\"sessionId\":\""+m_uid+"\",\"ip\":\""+m_tSessionDetails->m_mrcpMgIp+"\",\"port\":"+m_tSessionDetails->m_mrcpMgTtsPort+",\"domainId\":"+m_tSessionDetails->m_mrcpDomainId+",\"isBot\":"+to_string(isBot)+"}";
#endif
   if(sessionDetails)
   {
    port = sessionDetails->getTtsServerPort();
   }

   XGLOG_INFO("xGateInhouseTtsClient::tts_client_websocket_open ConnectType[%d] Host[%s] Port[%s] uId[%s] callId[%s]",m_ttsConnInfo.m_eConnectType,host.c_str(),port.c_str(),m_uid.c_str(),m_callid.c_str());

  try{
    auto const results = resolver.resolve(host, port);
    if(m_ttsConnInfo.m_eConnectType == EN_ENGINE_CONNECT_TYPE::EN_ENGINE_CONNECT_TYPE_IP)
    {
      XGLOG_INFO("xGateInhouseTtsClient::tts_client_websocket_open EN_ENGINE_CONNECT_TYPE_IP uId[%s] callId[%s] initText[%s]",m_uid.c_str(),m_callid.c_str(),initText.c_str());
      /* auto endPoint = boost::asio::connect(ws.next_layer(), results.begin(),results.end()); */
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
      XGLOG_INFO("xGateInhouseTtsClient::tts_client_websocket_open EN_ENGINE_CONNECT_TYPE_URL uId[%s] callId[%s]",m_uid.c_str(),m_callid.c_str());
      /* auto endPoint = boost::asio::connect(wss.next_layer().next_layer(), results.begin(),results.end()); */
      /* host.append(":");
      host.append(port); */
      wss.next_layer().handshake(ssl::stream_base::client);
      wss.handshake(host, "/");
      wss.write(boost::asio::buffer(std::string(initText)));
      buff.consume(buff.size());
      wss.read(buff);
      str = boost::beast::buffers_to_string(buff.data()).c_str();
    }
    std::cout << "tts_client_websocket_open connection Established !!!" << std::endl;
    XGLOG_INFO("xGateInhouseTtsClient::tts_client_websocket_open Connection Established for callid(%s) init_text(%s)", (char*)ur_log_string(m_callid),initText.c_str());
    URMRCPMODULEBRIDGE->postMessageToSchedulerService(m_callid.c_str(), EN_SCHEDULER_TTS_RECONNECT_TIMER, SCHEDULER_FUNCTION_CANCEL_TIMER);
    m_iRetryCount = MRCP_CONFIG::instance()->m_ttsProfile.m_iRetryCount; /* Reset Retry count after connection establishment for further reconnection */
    setTtsConnectRetryCount(m_callid,m_iRetryCount);
  
    std::cout<<"received msg "<<str<<std::endl;
    m_bIsUserInCall = true;

    if(sessionDetails)
    {
        if(false==sessionDetails->getIsTtsServerConnected())
        {
            XGLOG_INFO("xGateInhouseTtsClient::%s sending EN_TTS_MSG_ALLOCATE_RES to bridge module uID[%s], callID[%s]", __func__, (char *)ur_log_string(m_uid), (char *)ur_log_string(m_callid));
            post_message_to_bridgemodule(EN_TTS_MSG_ALLOCATE_RES);
        }
        if(true==sessionDetails->getIsPlayTextSent())
        {
            send_synth_request(sessionDetails->m_displayText.c_str(),sessionDetails->getUserLang());
        }
    }
  }
  catch(boost::system::system_error const& sysErr)
  {
    boost::system::error_code error = sysErr.code();
    std::cerr << "tts_client_websocket_open Error: "<< error.message() << std::endl;
    m_iRetryCount = getTtsConnectRetryCount(m_callid);
    XGLOG_INFO("xGateInhouseTtsClient::tts_client_websocket_open error message (%s) uId:%s callId:%s retryCount[%d]",(char*)ur_log_string(error.message()),(char*)ur_log_string(m_uid),(char*)ur_log_string(m_callid),m_iRetryCount);
    if(m_iRetryCount!=0)
    {
        XGLOG_INFO("xGateInhouseTtsClient::tts_client_websocket_open trying to reconnect uId[%s] callId[%s] retryCount[%d]",m_uid.c_str(),m_callid.c_str(),m_iRetryCount);
        m_iRetryCount--;
        setTtsConnectRetryCount(m_callid,m_iRetryCount);
        // post_message_to_bridgemodule(EN_TTS_RECONNECTION);
        URMRCPMODULEBRIDGE->postMessageToSchedulerService(m_callid.c_str(), EN_SCHEDULER_TTS_RECONNECT_TIMER, SCHEDULER_FUNCTION_SET_TIMER);
    }
  }
}

BOOL_TTSCLIENT::send_synth_request(STR text,STR userLang)
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
  strReq = encode_request_msg(text, userLang);
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
    /* std::thread(xGateInhouseTtsClient::tts_server_response,this).detach(); */
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

STR_TTSCLIENT::encode_request_msg(STR text,STR userLang)
{
  Document doc;
  StringBuffer strBuf;
  Writer<StringBuffer> writer(strBuf);
  Value outputVal(kObjectType);
  Document::AllocatorType& allocator = doc.GetAllocator();

  outputVal.AddMember("text",setJsonParam(text, doc),allocator);
  outputVal.AddMember("lang",setJsonParam(userLang, doc),allocator);

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
    while(ttsCli->m_bIsUserInCall)
    {
        ACE_Thread::testcancel();  // cooperative cancel point
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
      if(ttsCli->m_bIsUserInCall==false)
      {
        return;
      }
    }
  }
  catch(boost::system::system_error const& sysErr)
  {
    boost::system::error_code error = sysErr.code();
    std::cerr << "tts_server_response Error: "<<error.message() << std::endl;
    XGLOG_ERROR("xGateInhouseTtsClient::tts_server_response error message (%s)", (char*)ur_log_string(error.message()));
    ttsCli->m_iRetryCount = ttsCli->getTtsConnectRetryCount(ttsCli->m_callid);
    XGLOG_INFO("xGateInhouseTtsClient::tts_server_response error message (%s) uId:%s callId:%s retryCount[%d]",(char*)ur_log_string(error.message()),(char*)ur_log_string(ttsCli->m_uid),(char*)ur_log_string(ttsCli->m_callid),ttsCli->m_iRetryCount);
    if(ttsCli->m_iRetryCount!=0)
    {
        XGLOG_INFO("xGateInhouseTtsClient::tts_server_response trying to reconnect uId[%s] callId[%s] retryCount[%d]",ttsCli->m_uid.c_str(),ttsCli->m_callid.c_str(),ttsCli->m_iRetryCount);
        ttsCli->m_iRetryCount--;
        ttsCli->setTtsConnectRetryCount(ttsCli->m_callid,ttsCli->m_iRetryCount);
        // ttsCli->post_message_to_bridgemodule(EN_TTS_RECONNECTION);
        URMRCPMODULEBRIDGE->postMessageToSchedulerService(ttsCli->m_callid.c_str(), EN_SCHEDULER_TTS_RECONNECT_TIMER, SCHEDULER_FUNCTION_SET_TIMER);
    }
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
                send_synth_request((*itr).m_strMessage,sessionInfo->getUserLang());
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
    std::string msg{""};
    int isTtsStop{"1"};
    // msg = "{\"callId\":\""+m_callid+"\",\"text\":\"\",\"tts_event\":1}";
    msg = "{\"callId\":\""+m_callid+"\",\"tts_event\":1}";
    XGLOG_INFO("xGateInhouseTtsClient::stop_synth_request for callId[%s], msg[%s]",m_callid.c_str(),msg.c_str());
    try
    {
        if(m_ttsConnInfo.m_eConnectType == EN_ENGINE_CONNECT_TYPE::EN_ENGINE_CONNECT_TYPE_IP)
        {
            ws.write(boost::asio::buffer(std::string(msg)));
        }
        else if(m_ttsConnInfo.m_eConnectType == EN_ENGINE_CONNECT_TYPE::EN_ENGINE_CONNECT_TYPE_URL)
        {
            wss.write(boost::asio::buffer(std::string(msg)));
        }
        
        auto it = UR_MRCP_MODULE_BRIDGE::instance()->m_sessionDetailMap.find(m_callid);
        if(it != UR_MRCP_MODULE_BRIDGE::instance()->m_sessionDetailMap.end() && it->second)
		{
			it->second->m_sessionSynthState = EN_SESSION_SYNTH_COMPLETED;
		}
        else
        {
            XGLOG_ERROR("xGateInhouseTtsClient::stop_synth_request SESSION not found ID[%s]",(char*)ur_log_string(m_callid));
        }
    }
    catch(boost::system::system_error const& sysErr)
    {
        boost::system::error_code error = sysErr.code();
        std::cerr << "stop_synth_request Error: "<< error.message() << std::endl;
        XGLOG_ERROR("xGateInhouseTtsClient::stop_synth_request error message (%s)",(char*)ur_log_string(error.message()));
    }
  
    return true;
}

VOID_TTSCLIENT::tts_client_websocket_close()
{
  XGLOG_FUNCTION_EN_EX("");
  boost::system::error_code ec;
  std::cout << "##### tts_client_websocket_close :  ######" << (int)m_ttsConnInfo.m_eConnectType;
  XGLOG_INFO("xGateInhouseTtsClient::tts_client_websocket_close closing connection for callid (%s)" , (char*)ur_log_string(m_callid));
  try{
    if(m_ttsConnInfo.m_eConnectType == EN_ENGINE_CONNECT_TYPE::EN_ENGINE_CONNECT_TYPE_IP)
    {
      /* ws.close(websocket::close_code::normal); */
      ws.async_close(websocket::close_code::normal, close_handler);
      /* ws.next_layer().cancel();
      ws.next_layer().close(); */
      ws.next_layer().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        if (ec) 
        {
            XGLOG_ERROR("SSL shutdown failed:%s for callid:%s", ec.message(), (char*)ur_log_string(m_callid));
            std::cerr << "SSL shutdown failed: " << ec.message() << std::endl;
        }
        ws.next_layer().lowest_layer().cancel(ec);
        if (ec) 
        {
            XGLOG_ERROR("TCP cancel failed:%s for callid:%s", ec.message(), (char*)ur_log_string(m_callid));
            std::cerr << "TCP cancel failed: " << ec.message() << std::endl;
        }
    }
    else if(m_ttsConnInfo.m_eConnectType == EN_ENGINE_CONNECT_TYPE::EN_ENGINE_CONNECT_TYPE_URL)
    {
        std::cout << "##### closing using url TTS ######";
        /* wss.close(websocket::close_code::normal); */
        /* wss.next_layer().next_layer().cancel();
        wss.next_layer().next_layer().close(); */
        
         wss.async_close(websocket::close_code::normal, close_handler);
        wss.next_layer().shutdown(ec);
         if (ec) 
         {
            XGLOG_ERROR("SSL shutdown failed:%s for callid:%s", ec.message(), (char*)ur_log_string(m_callid));
            std::cerr << "SSL shutdown failed: " << ec.message() << std::endl;
         }
         wss.next_layer().lowest_layer().cancel(ec);
         if (ec) 
         {
            XGLOG_ERROR("TCP cancel failed:%s for callid:%s", ec.message(), (char*)ur_log_string(m_callid));
            std::cerr << "TCP cancel failed: " << ec.message() << std::endl;
         }

         wss.next_layer().lowest_layer().close(); /* working */

         /* wss.async_close(websocket::close_code::normal, 
            [&](boost::system::error_code ec) {
                std::cout << "##### inside async_close ####";
                if (ec) {
                    std::cerr << "WebSocket close failed: " << ec.message() << std::endl;
                    return;
                }
        
                // Proceed with SSL shutdown after WebSocket close
                wss.next_layer().shutdown(ec);
                if (ec) {
                    std::cerr << "SSL shutdown failed: " << ec.message() << std::endl;
                }

                wss.next_layer().lowest_layer().cancel(ec);
                if (ec) {
                    std::cerr << "TCP cancel failed: " << ec.message() << std::endl;
                }

                // Now close the TCP socket
                wss.next_layer().lowest_layer().close(ec);
                if (ec) {
                    std::cerr << "TCP close failed: " << ec.message() << std::endl;
                }
            }
        ); */

         // Step 1: Close the WebSocket session
        /* wss.async_close(websocket::close_code::normal, [&](boost::system::error_code ec) {
            if (ec) {
                std::cerr << "WebSocket close failed: " << ec.message() << std::endl;
            }

            // Step 2: Properly shutdown SSL before closing the socket
            wss.next_layer().async_shutdown([&](boost::system::error_code ec) {
                if (ec) {
                    std::cerr << "SSL shutdown failed: " << ec.message() << std::endl;
                }

                // Step 3: Shutdown the underlying TCP socket
                wss.next_layer().lowest_layer().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
                if (ec) {
                    std::cerr << "TCP shutdown failed: " << ec.message() << std::endl;
                }

                // Step 4: Cancel any pending operations
                wss.next_layer().lowest_layer().cancel(ec);
                if (ec) {
                    std::cerr << "TCP cancel failed: " << ec.message() << std::endl;
                }
            });
        }); */

         /* wss.async_close(websocket::close_code::normal, close_handler);

         // Proper SSL shutdown before TCP socket shutdown
         wss.next_layer().async_shutdown([&](boost::system::error_code ec) {
             if (ec) {
                 std::cerr << "SSL shutdown failed: " << ec.message() << std::endl;
             }
             
             // Now it is safe to shutdown the TCP layer
             wss.next_layer().lowest_layer().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
             if (ec) {
                 std::cerr << "TCP shutdown failed: " << ec.message() << std::endl;
             }
         
             // Cancel any pending operations
             wss.next_layer().lowest_layer().cancel(ec);
             if (ec) {
                 std::cerr << "TCP cancel failed: " << ec.message() << std::endl;
             }

         }); */
         

         /* wss.async_close(websocket::close_code::normal, 
            [&](boost::system::error_code ec) {
                std::cout << "##### Got response from async_close ####" << std::endl;
                if (ec) {
                    std::cerr << "WebSocket close failed: " << ec.message() << std::endl;
                    return;
                }
        
                // SSL shutdown after WebSocket close completes
                wss.next_layer().shutdown(ec);
                if (ec) {
                    std::cerr << "SSL shutdown failed: " << ec.message() << std::endl;
                }
        
                // Cancel TCP layer after SSL shutdown
                wss.next_layer().lowest_layer().cancel(ec);
                if (ec) {
                    std::cerr << "TCP cancel failed: " << ec.message() << std::endl;
                }
            } 
        );*/
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

VOID_TTSCLIENT::close_handler(const boost::system::error_code& ec) 
{
    std::cout << "close_handler called !! Error code - " << ec << std::endl;
    if (!ec)
    {
        // Connection succeeded^M
        std::cout << "!! Connected!" << std::endl;
        XGLOG_INFO("xGateInhouseTtsClient::close_handler called ");
    }
    else
    {
        // Connection failed^M
        std::cout << "!! Error: " << ec.message() << std::endl;
        XGLOG_INFO("xGateInhouseTtsClient::close_handler called with Error ==> [%s]",ec.message());
    }
}

INT_TTSCLIENT::getTtsConnectRetryCount(STR id)
{
    unsigned short int Count(0);
    
    auto it = URMRCPMODULEBRIDGE->m_sessionDetailMap.find(id);
    if(it != URMRCPMODULEBRIDGE->m_sessionDetailMap.end() && it->second)
    {
        Count = it->second->getTtsRetryCount();
    }
    else
    {
        XGLOG_INFO("xGateInhouseTtsClient::getTtsConnectRetryCount session not found id:%s",id.c_str());
    }
    XGLOG_INFO("xGateInhouseTtsClient::getTtsConnectRetryCount id:%s count:%d",id.c_str(),Count);
    return Count;
}

BOOL_TTSCLIENT::setTtsConnectRetryCount(STR id,unsigned short Count)
{
    XGLOG_INFO("xGateInhouseTtsClient::setTtsConnectRetryCount id:%s",id.c_str());
    
    auto it = URMRCPMODULEBRIDGE->m_sessionDetailMap.find(id);
    if(it != URMRCPMODULEBRIDGE->m_sessionDetailMap.end() && it->second)
    {
        it->second->setTtsRetryCount(Count);
    }
    else
    {
        XGLOG_INFO("xGateInhouseTtsClient::setTtsConnectRetryCount session not found id:%s",id.c_str());
    }
    return true;
}
STR xGateInhouseTtsClient::getPort(STR uid)
{
    STR strPort("");
    URMRCPMODULEBRIDGE->getSttTtsServerPort(uid, strPort, EN_XGATE_TTS);
    XGLOG_INFO("xGateInhouseTtsClient::getPort ==> %s Id:%s",strPort.c_str(),uid.c_str());
    return strPort;
}
