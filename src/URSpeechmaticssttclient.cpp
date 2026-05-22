#include "URSpeechmaticssttclient.h"
#include "URMRCPModuleBridge.h"
#include "SttTtsServiceMsg.h"


URSpeechmaticssttclient::URSpeechmaticssttclient():
  Audio_Added(0),endoftranscript(0),isRecogStarted(false),isRecogProcessing(false),m_ccaasVmsInfo(nullptr)
{
  m_sttConnInfo = MRCP_CONFIG::instance()->m_sttProfile.m_smaticsConnInfo;
  try
  {
    std::thread(&URSpeechmaticssttclient::SPM_websocket_open,this).detach();
  }
  catch(std::exception const& e)
  {
	  std::cerr << "Error: " << e.what() << std::endl;
  }
}

URSpeechmaticssttclient::URSpeechmaticssttclient(ccaasVmsInfo *vmsInfo):
  m_ccaasVmsInfo(vmsInfo),Audio_Added(0),endoftranscript(0),isRecogStarted(false),isRecogProcessing(false),m_addAudio(0),m_convertedText("")
{
  m_sttConnInfo = MRCP_CONFIG::instance()->m_sttProfile.m_smaticsConnInfo;
  try
  {
    std::thread(&URSpeechmaticssttclient::SPM_websocket_open,this).detach();
  }
  catch(std::exception const& e)
  {
    std::cerr << "Error: " << e.what() << std::endl;
  }
}

URSpeechmaticssttclient::~URSpeechmaticssttclient()
{
  isRecogProcessing = false;
  ACE_Thread_Manager::instance()->cancel(m_tid,1);
  SPM_websocket_close();
}
//Initialising SPM Responses enum values into map
void URSpeechmaticssttclient::InitializeMapValues()
{
  SpmMsg["RecognitionStarted"] = RecognitionStarted;
  SpmMsg["AudioAdded"] = AudioAdded ;
  SpmMsg["AddPartialTranscript"] = AddPartialTranscript;
  SpmMsg["AddTranscript"] = AddTranscript;
  SpmMsg["EndOfTranscript"] = EndOfTranscript;
  SpmMsg["Info"] = Info;
  SpmMsg["Warning"] = Warning;
  SpmMsg["Error"] = Error;
}

//Switch case for hitting particular response of SPM
std::string URSpeechmaticssttclient::on_message(boost::beast::multi_buffer &b)
{
  XGLOG_FUNCTION_EN_EX("");
  Document document;

  std::string s = boost::beast::buffers_to_string(b.data());
  XGLOG_INFO("URSpeechmaticssttclient::on_message received responce (%s) for callid (%s)" , (char*)ur_log_string(s), (char*)ur_log_string(m_callid));
  if(document.ParseInsitu((char *)s.c_str()).HasParseError())
  {
    printf("parse json error");
    return ""; 
  }

 // printf("\nParsing to document succeeded.\n");

  assert(document.IsObject());

  assert(document.HasMember("message"));

  assert(document["message"].IsString());

  //printf("message = %s\n", document["message"].GetString());

  std::string k = document["message"].GetString();

  std::cout<<k<<std::endl;

  switch(SpmMsg[k])
  {
    case RecognitionStarted:
      {
        printf("\n\nRecognition Started\n\n");    
        break;
      }
    case AudioAdded:
      {
//        printf("\n\nAudio Added\n\n");
		
		Audio_Added++;
    if(Audio_Added == m_addAudio)
    {
    SPM_EndofStream();
    }

        break;
      }
    case AddPartialTranscript:
      {
        printf("Add Partial Transcript\n");
		const Value &Transcript = document["metadata"];
				if(Transcript.HasMember("transcript"))
				{
					std::string transcript = Transcript["transcript"].GetString();
					printf("%s", transcript.c_str());
				}	
        break;
      }
    case AddTranscript:
      {
         printf("\n\nAdd Transcript\n\n");
        //std::cout << boost::beast::buffers(b.data()) << std::endl;
        //std::cout << boost::beast::make_printable(b.data()) << std::endl;
        std::string transcript("");
        const Value &Transcript = document["metadata"];
        if(Transcript.HasMember("transcript"))
        {
          if(Transcript["transcript"].GetString())
          {
            transcript = Transcript["transcript"].GetString();
          }
        }
        if(m_ccaasVmsInfo)
        {
          m_convertedText.append(transcript);
          return k;
        }
        UrMrcpSessionDetails *sessionInfo = NULL;
        std::map<std::string,UrMrcpSessionDetails *>::iterator it = URMRCPMODULEBRIDGE->m_sessionDetailMap.find(m_callid);
        if(it!=URMRCPMODULEBRIDGE->m_sessionDetailMap.end() && it->second && it->second->m_sessionRecogState != EN_SESSION_RECOG_STOP)
        {
          sessionInfo = it->second;
        }
        else
        {
          return k;
        }
        if(!sessionInfo->m_isCcaasAgentConnected)
        {
          isRecogProcessing = false;
          sessionInfo->m_sessionRecogState = EN_SESSION_RECOG_COMPLETED;
          if(!sessionInfo->m_dtmf_digits.empty())
            return k;
        }

        xGateDbConfigMgrServiceMsg *mrcpservicemsg = new xGateDbConfigMgrServiceMsg;
        mrcpservicemsg->set_call_id(m_callid);
        // ws.binary(false);
        printf("%s", transcript.c_str());
        if(!transcript.empty())
        {
          mrcpservicemsg->m_convertedText = transcript.c_str();
          mrcpservicemsg->set_mrcp_command(EN_MRCP_MSG_POST_MSG_TO_DB);
          URMRCPMODULEBRIDGE->PostMessageToMrcpMg(mrcpservicemsg);
        }
        else if(!sessionInfo->m_isCcaasAgentConnected)
        {
          sendStartRecognitionReq();
        }
        /*else if(sessionInfo->m_sessionSynthState != EN_SESSION_SYNTH_START)
          {
          mrcpservicemsg->set_mrcp_command(EN_MRCP_MSG_SYNTHESIZE_REQ);
          mrcpservicemsg->m_displayText.push_back(MRCP_CONFIG::instance()->m_no_query_from_user_ivr.c_str());
          mrcpservicemsg->setDstModuleId(IURDefines::UR_MODULE_MRCP_CLI_APP_SERVICE);
          URMRCPMODULEBRIDGE->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(mrcpservicemsg));
          }*/

        break;
      }
    case EndOfTranscript:
      {
        printf("\n\n!!End of transsript\n\n");
        endoftranscript = 1;
        if(m_ccaasVmsInfo)
        {
          m_ccaasVmsInfo->set_data(m_convertedText);
          m_ccaasVmsInfo->set_vms_pub_type(EN_CCAAS_VMS_PUB_TYPE::EN_CCAAS_VMS_PUB_TYPE_VMS_NODE);
          std::thread(&URMRCPModuleBridge::postMessageToDbService,URMRCPMODULEBRIDGE,m_ccaasVmsInfo).detach();
        }
        break;
      }
    case Info:
      {
        printf("\n\n Info \n\n");

        
        break;
      }
    case Warning:
      {
        printf("\n\n Warning \n\n");

       
        break;
      }
    case Error:
      {
        printf("\n\n Error \n\n");

        
        break;
      }
    default:
      {
        printf("\n\n default message \n\n");
        break;
      }
  }
  
  return k;
}
// opening websocket connection  for the Speech matics Server
void URSpeechmaticssttclient::SPM_websocket_open()
{

  XGLOG_FUNCTION_EN_EX("");
  // auto const host = "rtthha-speech.unifiedring.co.uk";
  //auto const host = "newrt-speech.worktual.co.uk";
  // auto const port = "9000";
  std::string host(""), port("");
  host = m_sttConnInfo.m_strServerHostUrl.c_str();
  port = m_sttConnInfo.m_strServerPort.c_str();
try{
	
  load_root_certificates(ctx);

  auto const results = resolver.resolve(host, port);

  boost::asio::connect(ws.next_layer().next_layer(), results.begin(), results.end());

  ws.next_layer().handshake(ssl::stream_base::client);

  ws.handshake(host, "/v2");

  std::cout << "Connection Established" << std::endl;

  SPM_start_recognition();
}

catch(std::exception const& e)
{
	std::cerr << "Error: " << e.what() << std::endl;
}	

}

// Sending Start Recognition request to SPM Server
bool URSpeechmaticssttclient::SPM_start_recognition()
{

  XGLOG_FUNCTION_EN_EX("");
try{
  ws.write(boost::asio::buffer(std::string(rpcJsonMsg)));

  buff.consume(buff.size());
  ws.read(buff);
  std::string response = on_message(buff); 
  std::cout<<"! recog response "<<response<<std::endl;
  if(strncmp(response.c_str(),"RecognitionStarted",response.length()) == 0)
  {
    XGLOG_INFO("URSpeechmaticssttclient::SPM_start_recognition success");
    isRecogStarted = true;
    if(m_ccaasVmsInfo)
    {
      isRecogProcessing = true;
      ACE_Thread_Manager::instance()->spawn((ACE_THR_FUNC)&URSpeechmaticssttclient::SPM_ServerResponse,this,THR_NEW_LWP|THR_JOINABLE ,&m_tid);
      SPM_Add_audio(m_ccaasVmsInfo->get_file_name().c_str());
      return true;
    }
    UrMrcpSessionDetails *sessionInfo = NULL;
    std::map<std::string,UrMrcpSessionDetails *>::iterator it = URMRCPMODULEBRIDGE->m_sessionDetailMap.find(m_callid);
    if(it!=URMRCPMODULEBRIDGE->m_sessionDetailMap.end() && it->second && it->second->m_sessionRecogState != EN_SESSION_RECOG_STOP)
    {
	    sessionInfo = it->second;
	    if(sessionInfo->m_isCcaasAgentConnected)
	    {
		    sendStartRecognitionReq();
	    }
    }
    return true;
  }
  std::cout << boost::beast::make_printable(buff.data()) << std::endl;
  return false;
}
catch(std::exception const& e)
{
		std::cerr << "Error: " << e.what() << std::endl;
}
	
}

// Sending voice  packets to SPM Server
void URSpeechmaticssttclient::SPM_Add_audio(std::string fileName)
{
  XGLOG_FUNCTION_EN_EX("");
  std::ifstream file_stream(fileName.c_str(), std::ios::binary);
  auto constexpr kChunkSize = 64 * 1024;
  std::vector<char> chunk(kChunkSize);

  ws.binary(true);
  try{
    while(true)
    {
      file_stream.read(chunk.data(), chunk.size());
      auto const bytes_read = file_stream.gcount();

      if (bytes_read > 0)
      {
        if(endoftranscript == 0)
        {
          ws.binary(true);
          ws.write(boost::asio::buffer(chunk.data(), bytes_read));
          printf("Audio writing to ws\n");
          m_addAudio++;
        }
        else
        {
          printf("end of stream arrived, so not sending audio data \n");
          endoftranscript = 0;
          break;
        }
      }
      if(!file_stream)
      {
        ws.binary(false);
        break;
      }
    }
  }
  catch(std::exception const& e)
  {
    std::cerr << "Error: " << e.what() << std::endl;
  }

}

// Receiving Respnse from Speech matics Server
void URSpeechmaticssttclient::SPM_ServerResponse()
{

  XGLOG_FUNCTION_EN_EX("");
  InitializeMapValues();
 
 std::cout <<"thread started SPM_ServerResponse "<<std::endl;
  try
  {
    while(isRecogProcessing)
    {
      buff.consume(buff.size());
      ws.read(buff);

    //  std::cout << boost::beast::make_printable(buff.data()) << std::endl;

      std::string k = on_message(buff);
      if(strcmp(k.c_str(),"EndOfTranscript") == 0)
      {
        printf("end of transcript arrived \n");
        break;
      }
    }
  }
catch(std::exception const& e)
{
	std::cerr << "Error: " << e.what() << std::endl;
}
	
}



void URSpeechmaticssttclient::SPM_EndofStream()
{
	Document doc;
	//endofstream OBJ;
	//OBJ.message = "EndOfStream";
	//OBJ.seq_no = Audio_Added;
	//OBJ.seq_no = 40;
  if(!Audio_Added)
  Audio_Added = 0;
  std::string msg = "EndOfStream";
	StringBuffer strBuf;
	Writer<StringBuffer> writer(strBuf);
	Value outputVal(kObjectType);
	Document::AllocatorType& allocator = doc.GetAllocator();
	outputVal.AddMember("message", setJsonParam(msg,doc), allocator);
	outputVal.AddMember("last_seq_no", setJsonParam(Audio_Added,doc), allocator);
	outputVal.Accept(writer);
	std::string EndOfStream("");
	EndOfStream = strBuf.GetString();
//	printf("%s", EndOfStream.c_str());
  std::cout<<EndOfStream<<std::endl;
try{
	ws.binary(false);
	ws.write(boost::asio::buffer(std::string(EndOfStream)));
}
catch(std::exception const& e)
{
	std::cerr << "Error: " << e.what() << std::endl;
}	
}

//closing websocket connection for the Speech matics Server
void URSpeechmaticssttclient::SPM_websocket_close()
{
  XGLOG_FUNCTION_EN_EX("");
  XGLOG_INFO("URSpeechmaticssttclient::SPM_websocket_close closing connection for callid (%s)" , (char*)ur_log_string(m_callid));
try{
  ws.close(websocket::close_code::normal);
}
catch(std::exception const& e)
{
		std::cerr << "Error: " << e.what() << std::endl;
}	
}

Value URSpeechmaticssttclient::setJsonParam(std::string &strValue, Document &doc)
{
  Value retVal;
  retVal.SetString(strValue.c_str(), strValue.length(), doc.GetAllocator());
  return retVal;
}
 
Value URSpeechmaticssttclient::setJsonParam(unsigned int iValue, Document &doc)
{
  Value retVal;
  retVal = iValue;
  return retVal;
}

bool URSpeechmaticssttclient::sendStartRecognitionReq()
{
  try
  {
    if(isRecogStarted && !isRecogProcessing)
    {
      ws.binary(true);
      isRecogProcessing = true;
      ACE_Thread_Manager::instance()->spawn((ACE_THR_FUNC)&URSpeechmaticssttclient::SPM_ServerResponse,this,THR_NEW_LWP|THR_JOINABLE ,&m_tid);
      return true;
    }
  }
	catch(std::exception const& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
	}
  return false;
}

void URSpeechmaticssttclient::stop_req()
{
  XGLOG_FUNCTION_EN_EX("");
  XGLOG_INFO("URSpeechmaticssttclient::stop_req() for callid (%s)" , (char*)ur_log_string(m_callid));
  isRecogProcessing = false;
}
