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

using namespace rapidjson;
using tcp = boost::asio::ip::tcp;               // from <boost/asio/ip/tcp.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
namespace websocket = boost::beast::websocket;  // from <boost/beast/websocket.hpp>


int AudioAddedCount = 0;

#if 1
boost::asio::io_context ioc;
ssl::context ctx{ssl::context::sslv23_client};
tcp::resolver resolver{ioc};
websocket::stream<ssl::stream<tcp::socket>> wsss{ioc, ctx};
boost::beast::multi_buffer b;
auto const rpcJson1 = R"({"message":"EndOfStream","last_seq_no": 5000})";
#endif

#if 1
enum SP_msg_value{	 
	 RecognitionStarted,
	 AudioAdded,
	 AddPartialTranscript,
	 AddTranscript,
	 EndOfTranscript,
	 Info,
	 Warning,
	 Error1,
};

static std::map<std::string, SP_msg_value> SP_MSG;

static void InitializeMapValues()
{
	SP_MSG["RecognitionStarted"] = RecognitionStarted;
	SP_MSG["AudioAdded"] = AudioAdded;
	SP_MSG["AddPartialTranscript"] = AddPartialTranscript;
	SP_MSG["AddTranscript"] = AddTranscript;
	SP_MSG["EndOfTranscript"] = EndOfTranscript;
	SP_MSG["Info"] = Info;
	SP_MSG["Warning"] = Warning;
	SP_MSG["Error"] = Error1;
}
#endif


#if 1
std::string on_message(boost::beast::multi_buffer &b)
{
	Document document;
	
	std::string s = boost::beast::buffers_to_string(b.data());
	
	if(document.ParseInsitu((char *)s.c_str()).HasParseError())
	{
		printf("parse json error");
		//return 1;
	}

	printf("\nParsing to document succeeded.\n");

	assert(document.IsObject()); 

	assert(document.HasMember("message"));

	assert(document["message"].IsString());

	printf("message = %s\n", document["message"].GetString());
	
	std::string k = document["message"].GetString();
	
	std::cout<<k<<std::endl;
	
	switch(SP_MSG[k])
	{
		case RecognitionStarted:
		{
				printf("\n\nRecognition Started\n\n");
				
				return k;
				//break;
		}
		case AudioAdded:
		{
				printf("\n\nAudio Added\n\n");
				
				if(document.HasMember("seq_no"))
				{
					assert(document["seq_no"].IsInt());
					
					AudioAddedCount = document["seq_no"].GetInt();
					
					if(AudioAddedCount == 5000)
					{
						wsss.binary(false);
						g_print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! %s\n",rpcJson1);
						wsss.write(boost::asio::buffer(std::string(rpcJson1)));
						g_print("AudioAddedCount : !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! %d\n",AudioAddedCount);
					}
 
					
				}
				
				return k;
				//break;
		}
		case AddPartialTranscript:
		{
				printf("Add Partial Transcript\n");
				
				return k;
				//break;
		}
		case AddTranscript:
		{
				printf("\n\nAdd Transcript\n\n");
				
				return k;
				//break;
		}
		case EndOfTranscript:
		{
				printf("\n\nEnd of transsript\n\n");
				
				return k;
				//break;
		}
		case Info:
		{
				printf("\n\n Info \n\n");
				
				return k;
				//break;
		}
		case Warning:
		{
				printf("\n\n Warning \n\n");
				
				return k;
				//break;
		}
		case Error1:
		{
				printf("\n\n Error \n\n");
				
				return k;
				//break;
		}		
		default:
		{
				printf("\n\n default message \n\n");
				break;
		}	
	}
}
#endif

void Initialise()
{

	auto const host = "rtthha-speech.unifiedring.co.uk";
	auto const port = "9000";

	auto const rpcJson = R"({"message": "StartRecognition","transcription_config": {"language": "en","output_locale": "en-US","enable_partials":false},"audio_format": {"type": "raw","encoding": "pcm_s16le","sample_rate": 8000}})";	

 //	auto const rpcJson = R"({"message": "StartRecognition","transcription_config": {"language": "en","output_locale": "en-US","max_delay": 3.5,"enable_partials": false},"audio_format": {"type": "file"}})";

//	boost::asio::io_context ioc;
//	ssl::context ctx{ssl::context::sslv23_client};
//	tcp::resolver resolver{ioc};
//	websocket::stream<ssl::stream<tcp::socket>> ws{ioc, ctx};
//	boost::beast::multi_buffer b;
	
	load_root_certificates(ctx);
	
	auto const results = resolver.resolve(host, port);

	boost::asio::connect(wsss.next_layer().next_layer(), results.begin(), results.end());

	wsss.next_layer().handshake(ssl::stream_base::client);

	wsss.handshake(host, "/v2");

	std::cout << "Connection Established" << std::endl;

	wsss.write(boost::asio::buffer(std::string(rpcJson)));

	wsss.read(b);

	std::string s = boost::beast::buffers_to_string(b.data());
	
	printf("\n %s \n", s.c_str());

	//std::cout << boost::beast::buffers(b.data()) << std::endl;		
		
}

void on_read()
{
	InitializeMapValues();
	
	while(1)
	{
		b.consume(b.size());
		wsss.read(b);
		
		std::string s = boost::beast::buffers_to_string(b.data());
	
                printf("\n %s \n", s.c_str());
		
		std::string k = on_message(b);
		
		if(strcmp(k.c_str(),"EndOfTranscript") == 0)
		{
			printf("end of transcript arrived \n");
			wsss.close(websocket::close_code::normal);
			break;
		}
	}
}
