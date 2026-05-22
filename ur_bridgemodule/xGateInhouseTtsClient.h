#ifndef __INHOUSE_TTSCLIENT__H__
#define __INHOUSE_TTSCLIENT__H__

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <cstdlib>
#include <iostream>
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include <cstdio>
#include <thread>

//local includes
#include "xGateUtil.h"
#include "UrMrcpDefines.h"
#include "MrcpConfig.h"

#define STR std::string
#define BOOL_TTSCLIENT bool xGateInhouseTtsClient
#define STR_TTSCLIENT STR xGateInhouseTtsClient
#define VOID_TTSCLIENT void xGateInhouseTtsClient
#define INT_TTSCLIENT int xGateInhouseTtsClient

#define GETREF(type, c, name) auto ref##name() -> type& {return m_##c##name;} \
                            auto ref##name() const -> const type& {return m_##c##name;}

using namespace rapidjson;
using tcp = boost::asio::ip::tcp;
namespace ssl = boost::asio::ssl;
namespace websocket = boost::beast::websocket;

enum class EN_SYNTH_STATUS
{
  EN_SYNTH_STATUS_UNKNOWN,
  EN_SYNTH_PENDING,
  EN_SYNTH_INPROGRESS,
  EN_SYNTH_COMPLETE
};

class ttsServerResponse 
{
  public:
    ttsServerResponse()
    {
      m_enStatus = EN_SYNTH_STATUS::EN_SYNTH_STATUS_UNKNOWN;
      m_iErrorCode = 0;
      m_strErrorMessage = "";
    }
    ~ttsServerResponse()
    {
      m_enStatus = EN_SYNTH_STATUS::EN_SYNTH_STATUS_UNKNOWN;
      m_iErrorCode = 0;
      m_strErrorMessage = "";
    }
    ttsServerResponse(const ttsServerResponse& rhs)
    {
      m_enStatus = rhs.m_enStatus;
      m_iErrorCode = rhs.m_iErrorCode;
      m_strErrorMessage = rhs.m_strErrorMessage;
    }
    ttsServerResponse& operator = (const ttsServerResponse& rhs)
    {
      m_enStatus = rhs.m_enStatus;
      m_iErrorCode = rhs.m_iErrorCode;
      m_strErrorMessage = rhs.m_strErrorMessage;
      return *this;
    }

    GETREF(STR, str, ErrorMessage);
    GETREF(EN_SYNTH_STATUS, en, Status);
    GETREF(int, i, ErrorCode);

  private:
    EN_SYNTH_STATUS m_enStatus;
    int m_iErrorCode;
    STR m_strErrorMessage;
};

class xGateInhouseTtsClient
{
  public:

    xGateInhouseTtsClient(STR ,STR,UrMrcpSessionDetails *);
    xGateInhouseTtsClient(STR ,STR,UrMrcpSessionDetails *,EN_ENGINE_CONNECT_TYPE engineType);
    ~xGateInhouseTtsClient();

    bool send_synth_request(STR text, STR userLang);
    bool stop_synth_request();
    void stop();
     
  private:
    void tts_client_websocket_open();
    static void tts_server_response(void *data);
    void tts_client_websocket_close();
    bool process_message_from_server(boost::beast::flat_buffer buff);
    bool decode_message_from_server(STR msg, ttsServerResponse &res);
    bool post_message_to_bridgemodule(xGateMrcpCommand command);
    static void close_handler(const boost::system::error_code& ec);
    tcp::socket create_socket(STR uid, STR callid);
    static void connect_handler(const boost::system::error_code& ec);
    int getTtsConnectRetryCount(STR);
    bool setTtsConnectRetryCount(STR id,unsigned short Count);
    STR getPort(STR uid);

    STR encode_request_msg(STR text, STR userLang);
    Value setJsonParam(STR &strValue, Document &doc);
    Value setJsonParam(unsigned int iValue, Document &doc);

    boost::asio::io_context ioc;
    websocket::stream<tcp::socket> ws{ioc};
    tcp::resolver resolver{ioc};
    ssl::context ctx{ssl::context::sslv23_client};
    websocket::stream<ssl::stream<tcp::socket>> wss{ioc, ctx};
    //boost::beast::multi_buffer buff;
    boost::beast::flat_buffer buff;
    STR m_uid;
    STR m_callid;
    bool isSynthStarted;
    bool isSynthInprogress;
    ACE_thread_t m_tid;
    int m_fd;
    UrMrcpSessionDetails *m_tSessionDetails;
    static SttTtsConnInfo m_ttsConnInfo;
    bool m_bIsUserInCall;
    bool m_bIsSocketConnected;
    unsigned short m_iRetryCount;
};

#endif
