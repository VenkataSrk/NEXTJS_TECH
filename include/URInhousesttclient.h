#ifndef __INHOUSE__H__
#define __INHOUSE__H__

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
#include "MrcpConfig.h"
#include "UrMrcpDbServiceMsg.h"

using namespace rapidjson;
using tcp = boost::asio::ip::tcp;               // from <boost/asio/ip/tcp.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
namespace websocket = boost::beast::websocket;  // from <boost/beast/websocket.hpp>

class URInhousesttclient
{
	public:
		URInhousesttclient(std::string, std::string,unsigned int);
		URInhousesttclient(ccaasVmsInfo* vmsInfo);
		URInhousesttclient& operator=(const URInhousesttclient &rhs)=default;
		URInhousesttclient(const URInhousesttclient &rhs)=default;
		~URInhousesttclient();

		std::string on_message(boost::beast::flat_buffer &buffer); // using in latest version
		//std::string on_message(boost::beast::multi_buffer &buffer); //Used in older version of boost
		void Inhouse_websocket_open(); // opening websocket connection  for the Inhouse Server
		static void Inhouse_ServerResponse(void *data); // Receiving Respnse from Inhouse Server
		void Inhouse_websocket_close(); //closing websocket connection for the Inhouse Server
    void stop();
		Value setJsonParam(std::string &strValue, Document &doc);
		Value setJsonParam(unsigned int iValue, Document &doc);
    static void close_handler( const boost::system::error_code & ec);     //Result of operation
		bool sendStartRecognitionReq();
		void stop_req();
		void send_file_audio(std::string fileName);
		bool send_end_of_transcript();
		bool send_end_of_stream();
		bool findAndTrim(std::string &text);
  		tcp::socket create_socket();
  		static void connect_handler(const boost::system::error_code& ec);
		bool send_message_to_socket(UrMrcpSessionDetails *sessionInfo);
		//private:

		boost::asio::io_context ioc;
		websocket::stream<tcp::socket> ws{ioc};
		tcp::resolver resolver{ioc};
		ssl::context ctx{ssl::context::sslv23_client};
		websocket::stream<ssl::stream<tcp::socket>> wss{ioc, ctx};
		boost::beast::flat_buffer buff;
		//boost::beast::multi_buffer buff;
		std::string m_uid;
		std::string m_callid;
		std::string m_convertedText;
		bool isRecogStarted;
		bool isRecogProcessing;
		bool isEndOfStream;
		ACE_thread_t m_tid;
		static SttTtsConnInfo m_sttConnInfo;
		ccaasVmsInfo *m_vmsInfo;
		bool m_bisSockMsgSent;
		xGatePortType m_enPortType;
};

#endif
