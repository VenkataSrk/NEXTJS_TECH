#ifndef __INHOUSE__H__
#define __INHOUSE__H__

#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <sstream>
#include <vector>
#include <ctime>    // For std::time()

#include <cstdlib>
#include <iostream>
#include <string>
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "xGateUtil.h"
#include "xglog.h"
#include "XGConfig.h"

#define WS_CONN_URL

class URInhousesttclient
{

public:
    URInhousesttclient(std::string,std::string,std::string,unsigned int isCcaas, void *client);
    ~URInhousesttclient();

    bool Inhouse_websocket_connect_ssl(const SttConnInfo& connInfo);
    bool perform_handshake_no_ssl(const std::string& host, int port);
    bool Inhouse_websocket_connect(); // opening websocket connection  for the Speech matics Server
    void Inhouse_websocket_close(); //closing websocket connection for the Speech matics Server
    Value setJsonParam(std::string &strValue, Document &doc);
    Value setJsonParam(unsigned int iValue, Document &doc);
    bool sendStartRecognitionReq();
    bool create_socket();
    SSL_CTX* create_ssl_context();
    void initialize_ssl();

    bool getIsRecogProcessing()
    {
      return m_isRecogProcessing;
    }

    SttConnInfo getConnectInfo()
    {
      return m_connInfo;
    }
   
    void *getClientPtr ()
    {
      return m_ClientPtr;
    }
    SSL* m_ssl_context; // SSL object
    int m_sockfd;    // Socket descriptor

private:

    std::string m_roomid;
    std::string m_connid;
    std::string m_deviceid;
    bool m_isRecogProcessing;
    ACE_thread_t m_connTid;
    SttConnInfo m_connInfo;
    std::string m_strLanguage;
    void *m_ClientPtr;
    unsigned int m_isCcaas;

};

#endif
