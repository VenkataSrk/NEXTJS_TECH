#ifndef __SPEECHMATICS__H__
#define __SPEECHMATICS__H__

#include "root_certificates.hpp"

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include <cstdio>
#include <thread>
#include <map>
#include "xGateUtil.h"
#include "UrMrcpDbServiceMsg.h"
#include "MrcpConfig.h"

using namespace rapidjson;
using tcp = boost::asio::ip::tcp;               // from <boost/asio/ip/tcp.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
namespace websocket = boost::beast::websocket;  // from <boost/beast/websocket.hpp>

//need to remove
auto const rpcJsonMsg = R"({"message": "StartRecognition","transcription_config": {"language": "en","output_locale": "en-US"},"audio_format": {"type": "raw","encoding": "pcm_s16le","sample_rate": 8000}})";
auto const rpcJsonMsg1 = R"({"message":"EndOfStream","last_seq_no": 40})";
//

class URSpeechmaticssttclient
{

public:
    URSpeechmaticssttclient();
    URSpeechmaticssttclient(ccaasVmsInfo *vmsInfo);
    ~URSpeechmaticssttclient();
    enum spmRespMsg{
                 RecognitionStarted,
                 AudioAdded,
                 AddPartialTranscript,
                 AddTranscript,
                 EndOfTranscript,
                 Info,
                 Warning,
                 Error,
    };

    void InitializeMapValues();
    std::string on_message(boost::beast::multi_buffer &b);
    void SPM_websocket_open(); // opening websocket connection  for the Speech matics Server
    bool SPM_start_recognition(); // Sending Start Recognition request to SPM Server
    void SPM_Add_audio(std::string fileName); // Sending voice  packets to SPM Server
    void SPM_ServerResponse(); // Receiving Respnse from Speech matics Server
    void SPM_EndofStream(); //For sending end of stream message
    void SPM_websocket_close(); //closing websocket connection for the Speech matics Server
    Value setJsonParam(std::string &strValue, Document &doc);
    Value setJsonParam(unsigned int iValue, Document &doc);
    bool sendStartRecognitionReq();
    void stop_req();
//private:

    boost::asio::io_context ioc;
    ssl::context ctx{ssl::context::sslv23_client};
    websocket::stream<ssl::stream<tcp::socket>> ws{ioc, ctx};
    tcp::resolver resolver{ioc};
    boost::beast::multi_buffer buff;
    std::map<std::string, spmRespMsg> SpmMsg;
    int endoftranscript;
    int Audio_Added;
    std::string m_uid;
    std::string m_callid;
    bool isRecogStarted;
    bool isRecogProcessing;
    ACE_thread_t m_tid;
    ccaasVmsInfo *m_ccaasVmsInfo;
    unsigned int m_addAudio;
    std::string m_convertedText;
	SttTtsConnInfo m_sttConnInfo;
};

#endif
