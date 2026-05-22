#include <google/cloud/speech/v1/cloud_speech.grpc.pb.h>
#include <google/longrunning/operations.grpc.pb.h>
#include <google/cloud/storage/client.h>
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <cstring>
#include <thread>
#include <getopt.h>
#include <pthread.h>
#include <cstdlib>
#include <fstream>
#include <chrono>
#include <ctime>
#include <ace/Thread_Manager.h>
#include "URMRCPModuleBridge.h"
using google::cloud::speech::v1::LongRunningRecognizeRequest;
using google::cloud::speech::v1::LongRunningRecognizeResponse;
using google::cloud::speech::v1::Speech;
using google::cloud::speech::v1::StreamingRecognizeRequest;
using google::cloud::speech::v1::StreamingRecognizeResponse;
using google::cloud::speech::v1::RecognitionConfig;
using google::cloud::speech::v1::RecognitionMetadata;
using namespace std;

enum finaltext
{
  with_null = 0,
  with_append,
  without_append,
};

int internal_send_longRunning_request(string file_path,std::string &outputText);
bool PostToBridge(xGateDbConfigMgrServiceMsg *SttServicemsg, bool isCcaasAgentConnected);
bool send_start_stop_req_to_mg(xGateDbConfigMgrServiceMsg *SttServicemsg,unsigned int recog_state);

int internal_send_longRunning_request(string file_path,std::string &outputText){

  auto creds = grpc::GoogleDefaultCredentials();
  auto channel = grpc::CreateChannel("speech.googleapis.com", creds);
  std::unique_ptr<Speech::Stub> speech(Speech::NewStub(channel));
  // Create a long operations stub so we can check the progress of the async
  // request.
  std::unique_ptr<google::longrunning::Operations::Stub> long_operations(
      google::longrunning::Operations::NewStub(channel));
  // Parse command line arguments.
  LongRunningRecognizeRequest request;
  // char* file_path = ParseArguments(argc, argv, request.mutable_config());
  /*if (nullptr == file_path) {
    std::cerr << kUsage;
    return -1;
    }*/
  request.mutable_config()->set_language_code("en-UK");
  request.mutable_config()->set_sample_rate_hertz(8000);  // Default sample rate.
  request.mutable_config()->set_encoding(RecognitionConfig::LINEAR16);
  // Pass the Google Cloud Storage URI to the request.
  //std::cout<<"internal_send_longRunning_request,uri:"<<file_path<<std::endl;
  request.mutable_audio()->set_uri(file_path.c_str());
  // Call LongRunningRecognize().
  grpc::ClientContext context;
  google::longrunning::Operation op;
  grpc::Status rpc_status =
    speech->LongRunningRecognize(&context, request, &op);
  if (!rpc_status.ok()) {
    // Report the RPC failure.
    std::cerr << rpc_status.error_message() << std::endl;
    return -1;
  }
  // Wait for the operation to complete.  Check the status once per second.
  std::cout << "Waiting for operation " << op.name() << " to complete.";
  google::longrunning::GetOperationRequest get_op_request;
  auto start = std::chrono::system_clock::now();
  get_op_request.set_name(op.name());
  while (!op.done()) {
    std::cout << "." << std::flush;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    // usleep(300000);
    grpc::ClientContext op_context;
    auto current = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = current-start;
    int timedout = elapsed_seconds.count();
    std::cout<<"elapsed time: " << timedout <<"s"<<std::endl;
    if(timedout > 80){
      std::cout<<"Timeout happened !!!"<<std::endl;
      return 0;
      }
    rpc_status =
      long_operations->GetOperation(&op_context, get_op_request, &op);
    if (!rpc_status.ok()) {
      // Report the RPC failure.
      std::cerr << rpc_status.error_message() << std::endl;
      return -1;
    }
  }
  std::cout << std::endl;
  // Unpack the response.
  if (!op.response().Is<LongRunningRecognizeResponse>()) {
    std::cerr << "The operation completed, but did not contain a "
      << "LongRunningRecognizeResponse.";
    return -1;
  }
  LongRunningRecognizeResponse response;
  op.response().UnpackTo(&response);
  // Dump the transcript of all the results.
  for (int r = 0; r < response.results_size(); ++r) {
    const auto& result = response.results(r);
    for (int a = 0; a < result.alternatives_size(); ++a) {
      const auto& alternative = result.alternatives(a);
      outputText.append(alternative.transcript());
      std::cout << "\n\n"<<alternative.confidence() << "\t <<<<"<<"-" << alternative.transcript() << ">>>>\n\n"
        << std::endl;
    }
  }
  // [END speech_async_recognize_gcs]
  return 0;
}



void UplaodFile(std::string const file_name,std::string object_name)
{
  namespace gcs = ::google::cloud::storage;
  using ::google::cloud::StatusOr;
  
  std::string const bucket_name = "unifiedring-bucket";
  //std::string const object_name;

  auto Client = gcs::Client();

  StatusOr<gcs::ObjectMetadata> metadata = Client.UploadFile(
  file_name, bucket_name, object_name, gcs::IfGenerationMatch(0));

  if (!metadata) throw std::runtime_error(metadata.status().message());

std::cout << "Uploaded " << file_name << " to object " << metadata->name()
<< " in bucket " << metadata->bucket()
<< "\nFull metadata: " << *metadata << "\n";

}

void Deletefile(std::string const file_name)
{
  namespace gcs = ::google::cloud::storage;
  std::string const bucket_name = "unifiedring-bucket";
  std::string const object_name = file_name;

  auto client = gcs::Client();
 
  google::cloud::Status status = client.DeleteObject(bucket_name, object_name);

  if (!status.ok()) throw std::runtime_error(status.message());
  std::cout << "Deleted " << object_name << " in bucket " << bucket_name << "\n";
}
struct details{
   grpc::ClientReaderWriterInterface<StreamingRecognizeRequest,
    StreamingRecognizeResponse>* streamer;
    int readfd;
    gboolean isOver;
};
//static void MicrophoneThreadMain(
  //  grpc::ClientReaderWriterInterface<StreamingRecognizeRequest,
   // StreamingRecognizeResponse>* streamer,int read_fd) {
 void MicrophoneThreadMain(void * details){
  //struct details *data=(struct details *)details;
   UrMrcpSessionDetails* data = ( UrMrcpSessionDetails*) details;
grpc::ClientReaderWriterInterface<StreamingRecognizeRequest,
   StreamingRecognizeResponse>* streamer = data->m_streamer;
int read_fd = data->m_readFd;
 StreamingRecognizeRequest request;
time_t curtime;

  //const size_t chunk_size = 64 * 1024;
   const size_t chunk_size = 4 * 1024;
  std::vector<char> chunk(chunk_size);
  while (true) {
	  std::streamsize bytes_read = read(read_fd,&chunk[0], chunk.size());
    time(&curtime);
    if(bytes_read > 0)
    {
		    request.set_audio_content(&chunk[0], bytes_read);
		    if(streamer && !data->m_isConnClosed)
		    {
			    streamer->Write(request);
		    }else return;
    }
       //std::cout<<"!! bytes_read :"<<bytes_read<<" TIME-----"<<ctime(&curtime)<<std::endl;
 //     std::cout <<"!!! microphone------- "<< ctime(&curtime)<<std::endl;
      //std::cout<<"!! readfd during write - "<<read_fd<<std::endl;
    if(data->m_isConnClosed == TRUE)
    {
      XGLOG_INFO("!!! MicrophoneThreadMain Closing read fd - %d !!!",read_fd);
       if(read_fd > 3)
       close(read_fd);
       break;
    }

#if 0
    if (bytes_read == 0) {
      data->m_isConnClosed=TRUE;
      if(streamer)
      {
        streamer->WritesDone();
      }
      XGLOG_INFO("MicrophoneThreadMain closing fd - %d ",read_fd);
      //close(read_fd);
      break;
    }
    
 #endif   
  }

}
//int trigger_Streaming_recognize_requst(xGateDbConfigMgrServiceMsg *SttServicemsg){
void trigger_Streaming_recognize_requst(void *servdata){
  restart:
  xGateDbConfigMgrServiceMsg *SttServicemsg = static_cast<xGateDbConfigMgrServiceMsg *> (servdata);
  std::cout<<"! inside trigger_Streaming_recognize_requst ! "<<std::endl;
  XGLOG_INFO("trigger_Streaming_recognize_requst for callid - %s ",SttServicemsg->m_callId.c_str());
  auto creds = grpc::GoogleDefaultCredentials();
  auto channel = grpc::CreateChannel("speech.googleapis.com", creds);
  std::unique_ptr<Speech::Stub> speech(Speech::NewStub(channel));
  StreamingRecognizeRequest request;
  auto* streaming_config = request.mutable_streaming_config();
  UrMrcpSessionDetails* sessionDetails;
  std::map<std::string,UrMrcpSessionDetails*>::iterator it = URMRCPMODULEBRIDGE->m_sessionDetailMap.find(SttServicemsg->m_callId);
  if(it != URMRCPMODULEBRIDGE->m_sessionDetailMap.end())
  {
    sessionDetails = it->second;
  }
  else
  {
    XGLOG_ERROR("! Session not found for callid(%s)",SttServicemsg->m_callId.c_str());
    return;
  }
  bool dataPublished(false);
  bool AgentStatus(false);
  bool isRestart(false);
  AgentStatus = sessionDetails->m_isCcaasAgentConnected;
  std::string CallStatus = SttServicemsg->m_ccaasCallStatus.c_str();
  time_t current_time;
  current_time = time(NULL);
  std::cout<<"! inside trigger_Streaming_recognize_requst ! time - "<<ctime(&current_time)<<std::endl;

  RecognitionConfig* config=streaming_config->mutable_config();
  config->set_language_code("en-UK");
  config->set_sample_rate_hertz(48000);  // Default sam
  config->set_encoding(RecognitionConfig::LINEAR16);
  //config->set_max_alternatives(0);
  // config->set_model("latest_short");//phone_call model also fine for STT but cost is more than this one. 
  // config->set_model("latest_long");//phone_call model also fine for STT but cost is more than this one. 
  config->set_model("phone_call");//phone_call model also fine for STT but cost is more than this one. 
  // config->set_model("voice_search");//phone_call model also fine for STT but cost is more than this one. 
  // config->set_model("command_and_search");//phone_call model also fine for STT but cost is more than this one. 
   config->set_use_enhanced(true);// this for phone call
  //  config->interactionType(VOICE_SEARCH);
  // RecognitionMetadata metadata ;
  // metadata.set_interaction_type(6);
  // config->set_allocated_metadata(&metadata);
  // RecognitionMetadata::InteractionType(VOICE_SEARCH);
  //  config->set_enable_word_time_offsets(true);
  grpc::ClientContext context;
  auto streamer = speech->StreamingRecognize(&context);
  // streaming_config->set_single_utterance(true);
  streaming_config->set_interim_results(true); //this applicable only speech to text without AI
  // streaming_config->set_interim_results(false); //this applicable only speech to text without AI
  if(streamer)
  {
   streamer->Write(request);
  }
  
 sessionDetails->m_isConnClosed = false;

  sessionDetails->m_readFd = SttServicemsg->get_fileDescriptor();
std::cout<<"! trigger_Streaming_recognize_requst readfd() - "<<sessionDetails->m_readFd<<std::endl;
ACE_thread_t tid;

	//ACE_Thread_Manager::instance()->spawn((ACE_THR_FUNC)MicrophoneThreadMain,&st,THR_NEW_LWP|THR_JOINABLE ,&tid);
	ACE_Thread_Manager::instance()->spawn((ACE_THR_FUNC)MicrophoneThreadMain,sessionDetails,THR_NEW_LWP|THR_JOINABLE ,&tid);
	sessionDetails->m_resId = tid;
  XGLOG_INFO("! trigger_Streaming_recognize_requs thread id - %u ",tid);

  sessionDetails->m_streamer = streamer.get();
  //sessionDetails->m_readFd = SttServicemsg->get_fileDescriptor();

  // std::thread microphone_thread(&MicrophoneThreadMain, streamer.get(),
  //   readFd);
  // microphone_thread.detach();
  StreamingRecognizeResponse response;
//std::cout<<"!90 trigger_Streaming_recognize_requst readfd() - "<<st.readfd<<std::endl;
  unsigned int duration_time = 0;
  while (streamer->Read(&response)) {  // Returns false when no more to read.
    for (int r = 0; r < response.results_size(); ++r) {
      const auto& result = response.results(r);
        const auto& alternative = result.alternatives(0);
        string convertedText(""),converted_Text(""),final_text("");
        const auto& nanotime = result.result_end_time();
        int count(0),whichtext(0);

         if(result.is_final() && !dataPublished)
        {
          if(!alternative.transcript().empty())
          {
            current_time = time(NULL);
            std::cout <<"!!! current_time processing (3) - "<< current_time<<std::endl;
            final_text = alternative.transcript();
            cout<<"!!!final text - "<<final_text.c_str()<<endl;

          }
        }

        if(!final_text.empty() && !dataPublished)
        {
            SttServicemsg->m_convertedText = final_text.c_str();
            current_time = time(NULL);
              // std::cout <<"!!! current_time afte posting - "<< current_time<<std::endl;
                std::cout <<"!!! cCONVERSION DONE------- "<< ctime(&current_time)<<std::endl;
            XGLOG_INFO("trigger_Streaming_recognize_requst converted text ' %s ' for callid - %s ",SttServicemsg->m_convertedText.c_str(),SttServicemsg->m_callId.c_str());
            // ACE_Thread_Manager::instance()->cancel(tid,1);

              auto time=result.result_end_time();
	            duration_time = time.seconds();

              if(sessionDetails->m_isCcaasAgentConnected || sessionDetails->m_sessionSynthState == EN_SESSION_SYNTH_COMPLETED)
              {
                SttServicemsg->set_mrcp_command(EN_MRCP_MSG_POST_MSG_TO_DB);
                PostToBridge(SttServicemsg,sessionDetails->m_isCcaasAgentConnected);
                dataPublished = true;               
                if(duration_time >= 240)
                {
                  sessionDetails->m_isConnClosed = true;
                  isRestart = true;
                  break; // for sending restart request
                }
              }
        }

        if(result.is_final() && dataPublished)
        // if(dataPublished)
        {
          dataPublished = false;
          count = 0;
        }
    //  cout<<response.DebugString()<<endl;
    }
  }
  XGLOG_INFO("!! before stream finish "); 
  ACE_Thread_Manager::instance()->cancel(tid,1);
  grpc::Status status = streamer->Finish();
  XGLOG_INFO("!! after stream finish ");
  sessionDetails->m_streamer = NULL;
  //streamer = NULL;
  // microphone_thread.join();
 // ACE_Thread_Manager::instance()->join(tid);
  //auto time=result.result_end_time();
	// duration_time = time.seconds();
  cout<<"!!! ### time:"<<duration_time<<endl;

  cout<<" Microphone thraed EXIT status:"<<status.error_code()<<endl;
  cout<<"! Agent Status - "<<AgentStatus<<endl;
  cout<<"! isRestart Status - "<<isRestart<<endl;
  cout<<"! m_bisUserInCall - "<<sessionDetails->m_bisUserInCall<<" id - "<<SttServicemsg->m_callId.c_str()<<endl;
  cout<<"! CallStatus - "<<CallStatus.c_str();
  if(sessionDetails->m_bisUserInCall)
  {
    if(status.error_code() == 11 || isRestart) {
   // Report the RPC failure.
    //  if(duration_time >= 300)
      {
       XGLOG_ERROR("! either got error code(%d) or timeout happened",status.error_code());
       cout<<endl<<"!! restarted due to timeout or error code"<<endl;
       isRestart = false;
    //std::cerr << status.error_message() << std::endl;
       goto restart;
     //send_start_stop_req_to_mg(SttServicemsg,EN_GRPC_MSG_START_CAPTURE_REQ);
      }
     
    return -1;
    }
  }  
  return 0;
}

bool PostToBridge(xGateDbConfigMgrServiceMsg *SttServicemsg, bool isCcaasAgentConnected)
{
  cout<<"!!! PostToBridge Callid - "<< SttServicemsg->m_callId.c_str()<<endl;
  //SttServicemsg->get_set_CcaasAgentConnected(isCcaasAgentConnected);
  XGLOG_INFO("!!! PostToBridge Callid - %s , isCcaasAgentConnected - %d ",SttServicemsg->m_callId.c_str(),isCcaasAgentConnected);
  SttServicemsg->set_ccaasCallStatus("on_call");
	URMRCPMODULEBRIDGE->PostMessageToMrcpMg(SttServicemsg);
}

bool send_start_stop_req_to_mg(xGateDbConfigMgrServiceMsg *SttServicemsg,unsigned int recog_state)
{
          ACE_Task<ACE_MT_SYNCH>* taskPtr = NULL;
          taskPtr = (ACE_Task<ACE_MT_SYNCH>*)xGateUtil::getHmpProcessor();
          if(!taskPtr) {
                  XGLOG_ERROR( "URMRCPModuleBridge::process_grpc_stt_start_req HmpProcessor task not found");
                  return EN_XGATE_STATUS_ERROR;
          }

          xGateMRCPMsg* pMgMsg = NULL;
          pMgMsg = new xGateMRCPMsg();

          XGLOG_INFO("URMRCPModuleBridge::process_grpc_stt_start_req Sending Stop ");
          pMgMsg->set_mrcp_uid(SttServicemsg->m_uid);
          pMgMsg->set_mrcp_call_id(SttServicemsg->m_callId);
          pMgMsg->set_mrcp_command(recog_state);
          pMgMsg->setMsgType(EN_XGATE_MSG_STT_TTS_IN);

          ACE_Message_Block* pAmb = NULL;
          pAmb = static_cast<xGateMRCPMsg*> (pMgMsg);
          ACE_Time_Value tval ((time(NULL) ));
          tval += ACE_Time_Value(0,1);

          if (taskPtr->putq(pAmb, &tval)) {
                  XGLOG_INFO( "URMRCPModuleBridge::process_grpc_stt_start_req HmpProcessor enqueue message success");
          } else {
                  XGLOG_ERROR( "URMRCPModuleBridge::process_grpc_stt_start_req HmpProcessor enqueue message failed");
                  delete pMgMsg;
                  return true;
          }
}
















