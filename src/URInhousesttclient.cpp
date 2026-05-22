#include "URInhousesttclient.h"
#include "URMRCPModuleBridge.h"
#include "SttTtsServiceMsg.h"
#include "xglog.h"

SttTtsConnInfo URInhousesttclient::m_sttConnInfo;

URInhousesttclient::URInhousesttclient(std::string uid, std::string callid,unsigned int portType)
  :m_vmsInfo(nullptr),m_uid(uid),m_callid(callid),isRecogStarted(false),isRecogProcessing(false), m_convertedText(""),isEndOfStream(false),m_enPortType(portType),
  ws(std::move(create_socket()))
{
   XGLOG_FUNCTION_EN_EX("");
   m_sttConnInfo = MRCP_CONFIG::instance()->m_sttProfile.m_inhouseConnInfo;
   std::thread(&URInhousesttclient::Inhouse_websocket_open,this).detach();
}

URInhousesttclient::URInhousesttclient(ccaasVmsInfo* vmsInfo)
  :m_vmsInfo(vmsInfo),isRecogStarted(false),isRecogProcessing(false), m_convertedText(""),isEndOfStream(false)
{
   XGLOG_FUNCTION_EN_EX("");
   m_uid = m_callid = vmsInfo->get_session_id().c_str();
   m_sttConnInfo = MRCP_CONFIG::instance()->m_sttProfile.m_inhouseConnInfo;
   std::thread(&URInhousesttclient::Inhouse_websocket_open,this).detach();
}


tcp::socket URInhousesttclient::create_socket()
{
  try
  {
    XGLOG_INFO("$$$$$ local public address --> [%s]",MRCP_CONFIG::instance()->m_strPublicAddr.c_str());
    tcp::socket socket(ioc);
    tcp::endpoint localEndpoint(boost::asio::ip::address::from_string(MRCP_CONFIG::instance()->m_strPublicAddr), 0);
    socket.open(localEndpoint.protocol());
    socket.bind(localEndpoint);
    std::cout<<"$$$$$ local endpoint1 construct --> "<<socket.local_endpoint()<<std::endl;
    XGLOG_INFO("$$$$$ local endpoint1 construct --> [%s]",socket.local_endpoint());

		tcp::endpoint Endpoint(boost::asio::ip::address::from_string(MRCP_CONFIG::instance()->m_sttProfile.m_inhouseConnInfo.m_strServerHostIp), atoi(MRCP_CONFIG::instance()->m_sttProfile.m_inhouseConnInfo.m_strServerPort.c_str()));
    // tcp::resolver::query query(MRCP_CONFIG::instance()->m_sttProfile.m_inhouseConnInfo.m_strServerHostIp, MRCP_CONFIG::instance()->m_sttProfile.m_inhouseConnInfo.m_strServerPort);
    // tcp::endpoint Endpoint = *resolver.resolve(query);
    cout<<"$$$$$  Endpoint  ==> "<<Endpoint<<std::endl;

    // socket.connect(Endpoint);
    socket.async_connect(Endpoint, connect_handler); // will connect to the stt socket asynchronously.
    XGLOG_INFO("URInhousesttclient::create_socket resolved endpoint ==> [%s]",Endpoint);
    cout<<"$$$$$  Endpoint  ==> "<<Endpoint<<std::endl;
    return socket;
  }
  catch(std::exception const& exp)
  {
    XGLOG_INFO("URInhousesttclient::create_socket connection error(%s)",exp.what());
    std::cout << "URInhousesttclient::create_socket Error: " << exp.what() << std::endl;
    tcp::socket socket(ioc);
    return socket;
  }
}

void URInhousesttclient::connect_handler(const boost::system::error_code& ec) {
  std::cout << "!! Error code - " << ec << std::endl;
    if (!ec) {
        // Connection succeeded^M
        std::cout << "!! Connected!" << std::endl;
        XGLOG_INFO("URInhousesttclient::connect_handler Connection successful");
    } else {
        // Connection failed^M
        std::cout << "!! Error: " << ec.message() << std::endl;
        XGLOG_INFO("URInhousesttclient::connect_handler Connection failed Error ==> [%s]",ec.message());
    }
}

void URInhousesttclient::close_handler(const boost::system::error_code& ec) {
  std::cout << "close_handler called !! Error code - " << ec << std::endl;
    if (!ec) {
        // Connection succeeded^M
        std::cout << "!! Connected!" << std::endl;
        XGLOG_INFO("URInhousesttclient::close_handler called ");
    } else {
        // Connection failed^M
        std::cout << "!! Error: " << ec.message() << std::endl;
        XGLOG_INFO("URInhousesttclient::close_handler called with Error ==> [%s]",ec.message());
    }
}

URInhousesttclient::~URInhousesttclient()
{
   //XGLOG_FUNCTION_EN_EX("");
   ACE_THR_FUNC_RETURN status = 0;
   XGLOG_INFO("URInhousesttclient::Entered destructor () for callid (%s) ",(char*)ur_log_string(m_callid));
}

void URInhousesttclient::stop()
{
   ACE_THR_FUNC_RETURN status = 0;
   XGLOG_INFO("URInhousesttclient::Entered stop () for callid (%s)" , (char*)ur_log_string(m_callid));
   isRecogStarted = false;
   isRecogProcessing = false;
   Inhouse_websocket_close();
   if (m_tid > 0){
     XGLOG_INFO("URInhousesttclient: BFR cancelling ACE_thr for tid: %lu" , m_tid);
     ACE_Thread_Manager::instance()->cancel(m_tid,1);
     XGLOG_INFO("URInhousesttclient: calling join for ACE_thr for tid: %lu" , m_tid);
     ACE_Thread_Manager::instance()->join(m_tid);
     XGLOG_INFO("URInhousesttclient: AFTER calling join for ACE_thr for tid: %lu" , m_tid);
   }
   XGLOG_INFO("URInhousesttclient::Exiting stop () for callid (%s)" , (char*)ur_log_string(m_callid));
}

// opening websocket connection  for the Speech matics Server
void URInhousesttclient::Inhouse_websocket_open()
{

   XGLOG_FUNCTION_EN_EX("");
   XGLOG_INFO("URInhousesttclient::Inhouse_websocket_open() for callid (%s)" , (char*)ur_log_string(m_callid));

   std::string  host(""),port("");
   host.assign(m_sttConnInfo.m_strServerHostIp.c_str());
   port = m_sttConnInfo.m_strServerPort.c_str();
   XGLOG_INFO("URInhousesttclient::Inhouse_websocket_open() for callid (%s) hostIp(%s) port(%s)" , (char*)ur_log_string(m_callid),(char*)ur_log_string(host),(char*)ur_log_string(port));

   try{
      std::cout << "!! Inhouse_websocket_open stt going to connect " << std::endl;
      auto const results = resolver.resolve(host, port);

      if(m_sttConnInfo.m_eConnectType == EN_ENGINE_CONNECT_TYPE::EN_ENGINE_CONNECT_TYPE_IP)
      {
         XGLOG_INFO("URInhousesttclient::stt_client_websocket_open using ip for callid (%s)",(char*)ur_log_string(m_callid));
         // Make the connection on the IP address we get from a lookup   
         //auto ep = boost::asio::connect(ws.next_layer(), results.begin(),results.end());
         host.append(":");
         host.append(port);
         ws.handshake(host, "/");
         ws.binary(true);
      }
      else if(m_sttConnInfo.m_eConnectType == EN_ENGINE_CONNECT_TYPE::EN_ENGINE_CONNECT_TYPE_URL)
      {
         XGLOG_INFO("URInhousesttclient::stt_client_websocket_open using url for callid (%s)",(char*)ur_log_string(m_callid));
        // auto ep = boost::asio::connect(wss.next_layer().next_layer(), results.begin(),results.end());
         host.append(":");
         host.append(port);
				 wss.next_layer().handshake(ssl::stream_base::client);
         wss.handshake(host, "/");
         wss.binary(true);
      }
      std::cout << "!! Inhouse_websocket_open stt Connection Established successfully for callid -  " << m_callid.c_str() << std::endl;
      XGLOG_INFO("!! Inhouse_websocket_open stt Connection Established successfully for callid [%s]",(char*)ur_log_string(m_callid));

      isRecogStarted = true;

        if(m_enPortType == EN_XGATE_STT)
        {
            XGLOG_INFO("URInhousesttclient::Inhouse_websocket_open sending EN_STT_TTS_MSG_ALLOCATE_RES to bridge module uID[%s], callID[%s]",(char*)ur_log_string(m_uid),(char*)ur_log_string(m_callid));
            xGateDbConfigMgrServiceMsg *mrcpServiceMsg = new xGateDbConfigMgrServiceMsg();
            mrcpServiceMsg->m_callId = m_callid;
            mrcpServiceMsg->set_mrcp_command(EN_STT_TTS_MSG_ALLOCATE_RES); // Sending msg 23 response if porttype is only STT
            UR_MRCP_MODULE_BRIDGE::instance()->PostMessageToMrcpMg(mrcpServiceMsg);
        }
        
      UrMrcpSessionDetails *sessionInfo = NULL;
      std::map<std::string,UrMrcpSessionDetails *>::iterator it = URMRCPMODULEBRIDGE->m_sessionDetailMap.find(m_uid);
      if(it!=URMRCPMODULEBRIDGE->m_sessionDetailMap.end() && it->second)
      {
        if(it->second->m_bisReconnect)
        {
            XGLOG_INFO("URInhousesttclient::Inhouse_websocket_open sendStartRecognitionReq as Reconnect is true for uID[%s] callID[%s]",(char*)ur_log_string(m_uid),(char*)ur_log_string(m_callid));
            sendStartRecognitionReq();  /* Need to check */
        }
      }
      else
      {
        XGLOG_ERROR("URInhousesttclient::Inhouse_websocket_open() Session not found for callid [%s]",(char*)ur_log_string(m_callid));
      }

      if(m_vmsInfo)
      {
        isRecogProcessing = true;
        ACE_Thread_Manager::instance()->spawn((ACE_THR_FUNC)&URInhousesttclient::Inhouse_ServerResponse,this,THR_NEW_LWP|THR_JOINABLE ,&m_tid);
        send_file_audio(m_vmsInfo->get_file_name().c_str());
      }
   }
   catch(boost::system::system_error const& se)
   {
      boost::system::error_code error = se.code();
      std::cerr << "Inhouse_websocket_open $$$$$$$$$$$$$$$$$System Error: $$$$$$$$$$$$$$$$$$$$$$$"<< error.message() << std::endl;
      XGLOG_INFO("Inhouse_websocket_open $$$$$$$$$$$$$$$$$System Error: $$$$$$$$$$$$$$$$$$$$$$$ [%s]",(char*)ur_log_string(error.message()));
   }
}

void  URInhousesttclient::send_file_audio(std::string fileName)
{
  XGLOG_FUNCTION_EN_EX("");
  std::ifstream file_stream(fileName.c_str(), std::ios::binary);
  auto constexpr kChunkSize = 640;
  std::vector<char> chunk(kChunkSize);

  try
  {
    if(m_sttConnInfo.m_eConnectType == EN_ENGINE_CONNECT_TYPE::EN_ENGINE_CONNECT_TYPE_IP)
    {
      ws.binary(true);
    }
    else if(m_sttConnInfo.m_eConnectType == EN_ENGINE_CONNECT_TYPE::EN_ENGINE_CONNECT_TYPE_URL)
    {
      wss.binary(true);
    }

    while(isRecogProcessing)
    {
      file_stream.read(chunk.data(), chunk.size());
      auto const bytes_read = file_stream.gcount();

      if (bytes_read > 0)
      {
        if(m_sttConnInfo.m_eConnectType == EN_ENGINE_CONNECT_TYPE::EN_ENGINE_CONNECT_TYPE_IP)
        {
         ws.write(boost::asio::buffer(chunk.data(), bytes_read));
        }
        else if(m_sttConnInfo.m_eConnectType == EN_ENGINE_CONNECT_TYPE::EN_ENGINE_CONNECT_TYPE_URL)
        {
         wss.write(boost::asio::buffer(chunk.data(), bytes_read));
        }
      }
      else
      {
        send_end_of_stream();
        break;
      }
      if(!file_stream)
      {
        send_end_of_stream();
        break;
      }
    }
  }
  catch(std::exception const& e)
  {
    std::cerr << "Error: " << e.what() << std::endl;
    XGLOG_ERROR("URInhousesttclient::send_file_audio Error: (%s) ", (char*)ur_log_string(e.what()));
    isRecogProcessing = false;
  }

  return;
}

bool URInhousesttclient::send_end_of_stream()
{
  std::string message("{\"message\":\"endOfStream\"}");
  try
  {
    if(m_sttConnInfo.m_eConnectType == EN_ENGINE_CONNECT_TYPE::EN_ENGINE_CONNECT_TYPE_IP)
    {
      ws.binary(false);
      ws.write(boost::asio::buffer(message));
    }
    else if(m_sttConnInfo.m_eConnectType == EN_ENGINE_CONNECT_TYPE::EN_ENGINE_CONNECT_TYPE_URL)
    {
     wss.binary(false);
     wss.write(boost::asio::buffer(message));
    }
    isEndOfStream = true;
  }
  catch(std::exception const& e)
  {
    XGLOG_ERROR("URInhousesttclient:send_end_of_transcript Error: (%s) ", (char*)ur_log_string(e.what()));
  }
  return true;
}

// Receiving Respnse from Speech matics Server
void URInhousesttclient::Inhouse_ServerResponse(void *data)
{
   URInhousesttclient *client= static_cast<URInhousesttclient *> (data);
   XGLOG_FUNCTION_EN_EX("");
   if(!client)
   {
      XGLOG_INFO("URInhousesttclient::Inhouse_ServerResponse invalid client !!!");
      return;
   }
   try
   {
        if(client->isRecogStarted)
        {
            while(client->isRecogProcessing)
            {
                //std::cout<<"!!!!!!!!!!!!!!!!Inhouse_ServerResponse waiting for server responce "<<std::endl;
                client->buff.consume(client->buff.size());
                XGLOG_INFO ("!!!!!!!!!!!!!!!!Inhouse_ServerResponse waiting for server responce BFR calling read");
                if(m_sttConnInfo.m_eConnectType == EN_ENGINE_CONNECT_TYPE::EN_ENGINE_CONNECT_TYPE_IP)
                {
                    if(client->ws.is_open() && client->isRecogProcessing)
                    {
                        XGLOG_INFO ("Before Inhouse_ServerResponse buffer size %d call _ID: %s",
                                                        client->buff.size(),client->m_callid.c_str());
                        client->ws.read(client->buff);
                        XGLOG_INFO ("After Inhouse_ServerResponse buffer size %d call _ID: %s",
                                                        client->buff.size(),client->m_callid.c_str());
                        if (!client->isRecogProcessing)
                        {
                            XGLOG_INFO ("isRecogProcessing: %d returning ", client->isRecogProcessing);
                            goto return_from_func; 
                        }
                    }
                }
                else if(m_sttConnInfo.m_eConnectType == EN_ENGINE_CONNECT_TYPE::EN_ENGINE_CONNECT_TYPE_URL)
                {
                    if(client->ws.is_open() && client->isRecogProcessing)
                    {
                        XGLOG_INFO ("Before Inhouse_ServerResponse buffer size %d call _ID: %s",client->buff.size(),client->m_callid.c_str());
                        client->wss.read(client->buff);
                        XGLOG_INFO ("After Inhouse_ServerResponse buffer size %d call _ID: %s",client->buff.size(),client->m_callid.c_str());
                        if (!client->isRecogProcessing)
                        {
                            XGLOG_INFO ("isRecogProcessing: %d returning ", client->isRecogProcessing);
                            goto return_from_func; 
                        }
                    }
                }

                std::string transcript = client->on_message(client->buff);
                std::cout<<"received text = "<<transcript <<" for callid "<<client->m_callid<<std::endl;
                if(client->m_vmsInfo)
                {
                    XGLOG_INFO("URInhousesttclient::Inhouse_ServerResponse posting vms transcript text to bridge isEndOfStream(%d) ",client->isEndOfStream);
                    if(client->isEndOfStream)
                    {
                        std::string resp_message = boost::beast::buffers_to_string(client->buff.data());
                        if(strstr(resp_message.c_str(),"EndOfTranscript"))
                        {
                           std::cout<<"!!! Got EndOfTranscript"<<std::endl;
                           client->m_vmsInfo->set_data(client->m_convertedText);
                           XGLOG_INFO("URInhousesttclient::Inhouse_ServerResponse converted text %s ",(char*)ur_log_string(client->m_convertedText));
                           std::thread(&URMRCPModuleBridge::postMessageToDbService,URMRCPMODULEBRIDGE,client->m_vmsInfo).detach();
                           break;
                        }
                    }
                    if(!transcript.empty())
                    {
                     client->m_convertedText.append(transcript);
                    }
                    break;
                }
                if(!transcript.empty())
                {
                    UrMrcpSessionDetails *sessionInfo = NULL;
                    std::map<std::string,UrMrcpSessionDetails *>::iterator it = URMRCPMODULEBRIDGE->m_sessionDetailMap.find(client->m_callid);
                    if(it!=URMRCPMODULEBRIDGE->m_sessionDetailMap.end() && it->second && it->second->m_sessionRecogState != EN_SESSION_RECOG_STOP)
                    {
                        sessionInfo = it->second;
                        if(sessionInfo->m_sessionRecogState != EN_SESSION_RECOG_START)
                        {
                            XGLOG_INFO("URInhousesttclient::Inhouse_ServerResponse RecogState not in EN_SESSION_RECOG_START ");
                            std::cout<<"received text4 = "<<transcript <<" for callid "<<client->m_callid<<std::endl;
                            /* continue; */
                        }
                        if(sessionInfo->m_isCcaas && !sessionInfo->m_isCcaasAgentConnected)
                        {
                            /* Continuous STT Recognition in BOT Scenario Starts */
                            /* client->isRecogProcessing = false;
                            sessionInfo->m_sessionRecogState = EN_SESSION_RECOG_COMPLETED; */
                            /* Continuous STT Recognition in BOT Scenario ends */
                            if(!sessionInfo->m_dtmf_digits.empty())
                            {
                                XGLOG_INFO("URInhousesttclient::Inhouse_ServerResponse received dtmf input from user");
                                continue;
                            }
                        }

                        unsigned short sendCcToWorkerNode = MRCP_CONFIG::instance()->m_sttProfile.m_iSendCcToWorkerNode;
                        bool isSendTranscript(false);
                        isSendTranscript = ((sessionInfo->m_isCcaas==false) /*check if UCAAS Call*/  || sessionInfo->m_bisccaasChatBotInit) ? true : (sendCcToWorkerNode ? true : false);

                        if(isSendTranscript)
                        {
                            xGateDbConfigMgrServiceMsg *mrcpservicemsg = new xGateDbConfigMgrServiceMsg;
                            mrcpservicemsg->set_call_id(client->m_callid);
                            mrcpservicemsg->m_convertedText = transcript.c_str();
                            mrcpservicemsg->set_ccaasCallStatus("on_call");
                            if(sessionInfo->m_bisccaasChatBotInit)
                            {
                                sessionInfo->m_pubChannelId = xGatePubChannelId::EN_CCAAS_BOT;
                                sessionInfo->m_eventId = xGateMgAcdEvent::EN_CCAAS_ACD_MG_UNKNOWN;
                            }
                            if(sessionInfo->getIsPlayTextSent()==true)
                            {
                                URMRCPMODULEBRIDGE->process_mrcp_synthesize_stop_req(mrcpservicemsg);
                            }
                            else
                            {
                                if(sessionInfo->m_bisccaasChatBotInit || sessionInfo->getIsSmartAssist())
                                {
                                    XGLOG_INFO("URMRCPModuleBridge::PostMessageToMrcpMg cancelling EN_SCHEDULER_CUSTOMER_RESP_WAIT_TIMER as got response from customer uID[%s]",sessionInfo->m_mgResourceId.c_str());
                                    URMRCPMODULEBRIDGE->postMessageToSchedulerService(mrcpservicemsg->m_callId.c_str(), EN_SCHEDULER_CUSTOMER_RESP_WAIT_TIMER, SCHEDULER_FUNCTION_CANCEL_TIMER);
                                }
                            }
                            mrcpservicemsg->set_mrcp_command(EN_MRCP_MSG_POST_MSG_TO_DB);
                            URMRCPMODULEBRIDGE->PostMessageToMrcpMg(mrcpservicemsg);
                        }
                        // break;
                        else
                        {
                            XGLOG_ERROR("either not a UCAAS or CCAAS Bot or sendCcToWorkerNode is disabled for callid (%s)",(char*)ur_log_string(client->m_callid));
                        }
                    }
                    else
                    {
                        XGLOG_ERROR("Session details not found for callid (%s)",(char*)ur_log_string(client->m_callid));
                    }
                }
            }
            XGLOG_INFO("!!!!!!!!!!!!!!!Inhouse_ServerResponse waiting for server responce AFR calling read");
        }
    }
    catch(boost::system::system_error const& se)
    {
       boost::system::error_code error = se.code();
       //std::cerr << "URInhousesttclient::Inhouse_ServerResponse $$$$$$$$$$$$$$$$$System Error: $$$$$$$$$$$$$$$$$$$$$$$"<< error.message() << std::endl;
       XGLOG_INFO("URInhousesttclient::Inhouse_ServerResponse $$$$$$$$$$$$$$$$$System Error: $$$$$$$$$$$$$$$$$$$$$$$ (%s) ", (char*)ur_log_string(error.message()));
       if(client)
       client->isRecogProcessing = false;
    }
    return_from_func:   
    XGLOG_INFO ("Exiting from thread");
}

//Switch case for hitting particular response of Inhouse
std::string URInhousesttclient::on_message(boost::beast::flat_buffer &buffer)
{
   XGLOG_FUNCTION_EN_EX("");


   std::string resp_msg(""),text("");
   std::string resp_json_message = text = boost::beast::buffers_to_string(buffer.data());
   findAndTrim(text);
   XGLOG_INFO("URInhousesttclient::on_message received responce (%s) for callid (%s)" , (char*)ur_log_string(text), (char*)ur_log_string(m_callid));
   Document doc;
   ParseResult ok = doc.Parse(resp_json_message.c_str());

   if(!ok) {
     XGLOG_ERROR( "parseJsonMsg failed. Encountered with json parse error !");
     return resp_msg;
   } else if(!doc.IsObject()) {
     XGLOG_ERROR( "parseJsonMsg failed. Not able create documnet object !");
     return resp_msg;
   }

   assert(doc.IsObject());

   if(doc.HasMember("text"))
   {
     resp_msg = text = doc["text"].GetString();
   }
   findAndTrim(text);
   XGLOG_INFO("URInhousesttclient::on_message received text %s",(char*)ur_log_string(text));
   return resp_msg;
}

bool URInhousesttclient::findAndTrim(std::string &text)
{
  XGLOG_INFO("URInhousesttclient::findAndTrim");
  std::string del("%");
  size_t found = text.find(del);
  while(found != string::npos)
  {
      text.erase(text.begin() + found);
      found = text.find(del);
  }
}

//closing websocket connection for the Speech matics Server
void URInhousesttclient::Inhouse_websocket_close()
{
   XGLOG_INFO("URInhousesttclient::Inhouse_websocket_close Entered for callid (%s)" , (char*)ur_log_string(m_callid));
   try{
      if(m_sttConnInfo.m_eConnectType == EN_ENGINE_CONNECT_TYPE::EN_ENGINE_CONNECT_TYPE_IP)
      {
        //ws.close(websocket::close_code::normal);
         ws.async_close(websocket::close_code::normal, close_handler);
      }
      else if(m_sttConnInfo.m_eConnectType == EN_ENGINE_CONNECT_TYPE::EN_ENGINE_CONNECT_TYPE_URL)
      {
         //wss.close(websocket::close_code::normal);
         wss.async_close(websocket::close_code::normal, close_handler);
      }
   }
   catch(boost::system::system_error const& se)
   {
      boost::system::error_code error = se.code();
      std::cerr << "URInhousesttclient::Inhouse_websocket_close $$$$$$$$$$$$$$$$$System Error: $$$$$$$$$$$$$$$$$$$$$$$"<< error.message() << std::endl;
   }
   XGLOG_INFO("URInhousesttclient::Inhouse_websocket_close Exiting for callid (%s)" , (char*)ur_log_string(m_callid));
}

Value URInhousesttclient::setJsonParam(std::string &strValue, Document &doc)
{
   Value retVal;
   retVal.SetString(strValue.c_str(), strValue.length(), doc.GetAllocator());
   return retVal;
}

Value URInhousesttclient::setJsonParam(unsigned int iValue, Document &doc)
{
   Value retVal;
   retVal = iValue;
   return retVal;
}

bool URInhousesttclient::sendStartRecognitionReq()
{
    XGLOG_FUNCTION_EN_EX("");
    std::string call_id = m_callid.c_str();
    XGLOG_INFO("URInhousesttclient::sendStartRecognitionReq for callid (%s)" , (char*)ur_log_string(call_id));
    try
    {
        isRecogProcessing = true;
        ACE_Thread_Manager::instance()->cancel(m_tid,1);
        ACE_Thread_Manager::instance()->spawn((ACE_THR_FUNC)&URInhousesttclient::Inhouse_ServerResponse,this,THR_NEW_LWP|THR_JOINABLE ,&m_tid);

        std::map<std::string,UrMrcpSessionDetails *>::iterator it = URMRCPMODULEBRIDGE->m_sessionDetailMap.find(m_callid);
        if(it!=URMRCPMODULEBRIDGE->m_sessionDetailMap.end() && it->second)
        {
            if(it->second->m_isCcaas)
            {
                it->second->m_sessionRecogState = EN_SESSION_RECOG_START;
                if(!m_bisSockMsgSent)
                {
                    send_message_to_socket(it->second);
                }
            }
        }
        std::cout<<"Start thread is spawned %s "<<m_callid<<std::endl;
        return true;
    }
    catch(boost::system::system_error const& se)
    {
        boost::system::error_code error = se.code();
        std::cerr << "URInhousesttclient::sendStartRecognitionReq $$$$$$$$$$$$$$$$$System Error: $$$$$$$$$$$$$$$$$$$$$$$"<< error.message() << std::endl;
    }

    return false;
}

bool URInhousesttclient::send_message_to_socket(UrMrcpSessionDetails *sessionInfo)
{
    XGLOG_FUNCTION_EN_EX("");
	XGLOG_INFO("URInhousesttclient::send_message_to_socket ID [%s]",(char*)ur_log_string(sessionInfo->m_callId));
    STR docString(""),message(""),ID("");
    int isBot(0);
    Document doc;
    StringBuffer strBuf;
    Writer<StringBuffer> writer(strBuf);
    std::map<std::string,UrMrcpSessionDetails *>::iterator it = URMRCPMODULEBRIDGE->m_sessionDetailMap.find(sessionInfo->m_mgResourceId);
    if(it!=URMRCPMODULEBRIDGE->m_sessionDetailMap.end() && it->second)
    {
        docString = it->second->m_strDocSocketMessage.c_str();
    }
    else
    {
        XGLOG_ERROR("URInhousesttclient::send_message_to_socket SESSION not found ID [%s]",(char*)ur_log_string(sessionInfo->m_mgResourceId));
        return false;
    }

    unsigned short sendCcToWorkerNode = MRCP_CONFIG::instance()->m_sttProfile.m_iSendCcToWorkerNode;
    isBot = sessionInfo->m_bisccaasChatBotInit ? 1 : (sendCcToWorkerNode ? 1 : 0); //Setting isBot as 1 if sendCcToWorkerNode is 1
    std::string redisHost(""),redisPwd(""),redisChannel(""),custNumber(""),custFirstName(""),custLastName("");
    unsigned short redisPort(0),pubType(0);
    
    if(docString.empty())
    {
        std::ostringstream oss("");
        Value outputVal(kObjectType);
        Document::AllocatorType& allocator = doc.GetAllocator();
        RedisConnInfo* m_tccaasBotRedisInfo;    
        UrMrcpSessionDetails* sInfo = nullptr;
        if(!isBot)
        {
            m_tccaasBotRedisInfo = MRCPCONFIGSERVICE->m_redisProfile.m_mapPubRedisConnInfo[EN_REDIS_TYPE::EN_REDIS_TYPE_CCAAS_CONV];
            redisHost = m_tccaasBotRedisInfo->m_strServerHost.c_str();
            redisPort = m_tccaasBotRedisInfo->m_iServerPort;
            redisPwd = m_tccaasBotRedisInfo->m_strPassword.c_str();
            redisChannel = m_tccaasBotRedisInfo->m_strChannel.c_str();
            pubType = (int)m_tccaasBotRedisInfo->m_enMsgType;
        }

        std::multimap<std::string,std::string>::iterator itr = URMRCPMODULEBRIDGE->m_callDetailsMap.find(sessionInfo->m_mgResourceId);
        
        if(itr == URMRCPMODULEBRIDGE->m_callDetailsMap.end())
        {
            XGLOG_ERROR("URInhousesttclient::send_message_to_socket ID not found in m_callDetailsMap ID [%s]",(char*)ur_log_string(sessionInfo->m_mgResourceId));
            return false;
        }

        auto range = URMRCPMODULEBRIDGE->m_callDetailsMap.equal_range(sessionInfo->m_mgResourceId);

        if(range.first != range.second)
        {
            for(auto mapItr = range.first; mapItr != range.second; mapItr++)
            {
                ID = mapItr->second.c_str();
                if(0 == strncmp(ID.c_str(),sessionInfo->m_mgResourceId.c_str(),ID.length()))
                {
                    continue;
                }
                else if(0 == strncmp(ID.c_str(),sessionInfo->m_callId.c_str(),ID.length()))
                {
                    break;
                }
                else
                {
                    continue;
                }
            }
        }
        
        std::map<std::string,UrMrcpSessionDetails *>::iterator IT = URMRCPMODULEBRIDGE->m_sessionDetailMap.find(ID);
        if(IT != URMRCPMODULEBRIDGE->m_sessionDetailMap.end() && IT->second)
        {
            if(it->second->m_ccaasUsertype == EN_CCAAS_USER_TYPE_CUSTOMER)
            {
                sInfo = it->second;
            }
            else
            if(IT->second->m_ccaasUsertype == EN_CCAAS_USER_TYPE_CUSTOMER)
            {
                sInfo = IT->second;
            }
        }
        else
        {
            XGLOG_ERROR("URInhousesttclient::send_message_to_socket SESSION not found ID [%s]",(char*)ur_log_string(ID));
            return false;
        }

        custNumber = sInfo->m_mrcpExtNum.c_str();
        custFirstName = sInfo->m_strCcaasCustomerFirstName;
        custLastName = sInfo->m_strCcaasCustomerLastName;

        outputVal.AddMember("session_id",setJsonParam(sessionInfo->m_mgResourceId,doc),allocator);
        outputVal.AddMember("is_bot",setJsonParam(isBot,doc),allocator);
        outputVal.AddMember("speaker",setJsonParam(sessionInfo->m_mrcpExtNum,doc),allocator);
        outputVal.AddMember("bot_id",setJsonParam(sessionInfo->m_iCcaasBotId,doc),allocator);
        outputVal.AddMember("domain_id",setJsonParam(sessionInfo->m_mrcpDomainId,doc),allocator);
        outputVal.AddMember("redis_ip",setJsonParam(redisHost,doc),allocator);
        outputVal.AddMember("redis_port",setJsonParam(redisPort,doc),allocator);
        outputVal.AddMember("redis_pwd",setJsonParam(redisPwd,doc),allocator);
        outputVal.AddMember("redis_channel",setJsonParam(redisChannel,doc),allocator);
        outputVal.AddMember("redis_pub_type",setJsonParam(pubType,doc),allocator);
        outputVal.AddMember("customer_first_name",setJsonParam(custFirstName,doc),allocator);
        outputVal.AddMember("customer_last_name",setJsonParam(custLastName,doc),allocator);
        outputVal.AddMember("customer_number",setJsonParam(custNumber,doc),allocator);

        Value agentDetails(kArrayType);
        Value agentVal(kObjectType);

        if(IT->second->m_ccaasUsertype == EN_CCAAS_USER_TYPE_AGENT)
        {
            sInfo = IT->second;
        }
        else if(it->second->m_ccaasUsertype == EN_CCAAS_USER_TYPE_AGENT)
        {
            sInfo = it->second;
        }
        
        
        if(sInfo && sInfo->m_ccaasUsertype == EN_CCAAS_USER_TYPE_AGENT)
        {
            agentVal.AddMember("name",setJsonParam(sInfo->m_strCcaasAgentName,doc),allocator);
            agentVal.AddMember("ext",setJsonParam(sInfo->m_mrcpExtNum,doc),allocator);
            agentVal.AddMember("userId",setJsonParam(sInfo->m_strCcaasAgentUserId,doc),allocator);
            agentVal.AddMember("email",setJsonParam(sInfo->m_strCcaasAgentEmail,doc),allocator);
            agentDetails.PushBack(agentVal, allocator);
        }
        
        outputVal.AddMember("agent",agentDetails, allocator);

        outputVal.Accept(writer);
        message = strBuf.GetString();

        it->second->m_strDocSocketMessage = message.c_str();
    }
    else
    {
        XGLOG_INFO("docString is not empty [%s]",docString.c_str());

        ParseResult ok = doc.Parse(docString.c_str());
        Document::AllocatorType& allocator = doc.GetAllocator();
        if(!ok) 
        {
            XGLOG_ERROR( "parseJsonMsg encountered with json parse error !");
            return false;
        }

        if(!isBot)
        {
            RedisConnInfo* m_tccaasBotRedisInfo;
            m_tccaasBotRedisInfo = MRCPCONFIGSERVICE->m_redisProfile.m_mapPubRedisConnInfo[EN_REDIS_TYPE::EN_REDIS_TYPE_CCAAS_CONV];
            redisHost = m_tccaasBotRedisInfo->m_strServerHost.c_str();
            redisPort = m_tccaasBotRedisInfo->m_iServerPort;
            redisPwd = m_tccaasBotRedisInfo->m_strPassword.c_str();
            redisChannel = m_tccaasBotRedisInfo->m_strChannel.c_str();
            pubType = (int)m_tccaasBotRedisInfo->m_enMsgType;

            if(doc.HasMember("is_bot"))
            {
                const Value& val = doc["is_bot"];
                val.SetInt(isBot);
            }

            if(doc.HasMember("redis_ip"))
            {
                const Value& val = doc["redis_ip"];
                val.SetString(redisHost.c_str(),redisHost.length());
            }

            if(doc.HasMember("redis_port"))
            {
                const Value& val = doc["redis_port"];
                val.SetInt(redisPort);
            }

            if(doc.HasMember("redis_pwd"))
            {
                const Value& val = doc["redis_pwd"];
                val.SetString(redisPwd.c_str(),redisPwd.length());
            }

            if(doc.HasMember("redis_channel"))
            {
                const Value& val = doc["redis_channel"];
                val.SetString(redisChannel.c_str(),redisChannel.length());
            }

            if(doc.HasMember("redis_pub_type"))
            {
                const Value& val = doc["redis_pub_type"];
                val.SetInt(pubType);
            }
        }

        if(doc.HasMember("speaker"))
        {
            const Value& val = doc["speaker"];
            val.SetString(sessionInfo->m_mrcpExtNum.c_str(),sessionInfo->m_mrcpExtNum.length());
        }

        if(sessionInfo->m_ccaasUsertype != EN_CCAAS_USER_TYPE_CUSTOMER)
        {
            if(doc.HasMember("agent"))
            {
                if(doc["agent"].IsArray())
                {
                    unsigned short sendCcToWorkerNode = MRCP_CONFIG::instance()->m_sttProfile.m_iSendCcToWorkerNode;
                    bool docClear = sendCcToWorkerNode ? ((sessionInfo->m_ccaasUsertype==EN_CCAAS_USER_TYPE_AGENT) ? true : false) : true;
                    if(docClear)
                    {
						XGLOG_INFO("URInhousesttclient::send_message_to_socket Clearing the doc ID [%s]",(char*)ur_log_string(sessionInfo->m_callId));
                     	doc["agent"].Clear();
                    }
                    Value agentVal(kObjectType);
                    agentVal.AddMember("name",setJsonParam(sessionInfo->m_strCcaasAgentName,doc),allocator);
                    agentVal.AddMember("ext",setJsonParam(sessionInfo->m_mrcpExtNum,doc),allocator);
                    agentVal.AddMember("userId",setJsonParam(sessionInfo->m_strCcaasAgentUserId,doc),allocator);
                    agentVal.AddMember("email",setJsonParam(sessionInfo->m_strCcaasAgentEmail,doc),allocator);
                    doc["agent"].PushBack(agentVal, allocator);
                }
            }
        }
        doc.Accept(writer);
        message = strBuf.GetString();
		it->second->m_strDocSocketMessage = message.c_str();
    }

   try
   {
    if(message.empty())
    {
        message = docString.c_str();
    }
    /* Commented for not sending socket message to inhouse socket server */
    /* if(m_sttConnInfo.m_eConnectType == EN_ENGINE_CONNECT_TYPE::EN_ENGINE_CONNECT_TYPE_IP)
    {
         ws.write(boost::asio::buffer(message));
         ws.binary(true);
    }
    else if(m_sttConnInfo.m_eConnectType == EN_ENGINE_CONNECT_TYPE::EN_ENGINE_CONNECT_TYPE_URL)
    {
         wss.write(boost::asio::buffer(message));
         wss.binary(true);
    } */
   }
   catch(boost::system::system_error const& se)
   {
      boost::system::error_code error = se.code();
      std::cerr << "URInhousesttclient::send_message_to_socket $$$$$$$$$$$$$$$$$System Error: $$$$$$$$$$$$$$$$$$$$$$$"<< error.message() << std::endl;
   }
   m_bisSockMsgSent = true;

   std::cout<<"!!"<<message<<std::endl;   
   XGLOG_INFO("URInhousesttclient::send_message_to_socket message [%s]",(char*)ur_log_string(message));
}

void URInhousesttclient::stop_req()
{
   XGLOG_FUNCTION_EN_EX("");
   XGLOG_INFO("URInhousesttclient::stop_req for callid (%s)" , (char*)ur_log_string(m_callid));
   isRecogProcessing = false;
}

