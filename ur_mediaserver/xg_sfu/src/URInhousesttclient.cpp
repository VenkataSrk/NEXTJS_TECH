#include "URInhousesttclient.h"
#include "xGateSFUtil.h"
#include "xGateHmpGstBin.h"

URInhousesttclient::URInhousesttclient(std::string roomid,std::string connid,std::string deviceid,unsigned int isCcaas, void *client):
  m_roomid(roomid),m_connid(connid),m_deviceid(deviceid),m_isRecogProcessing(false),m_strLanguage(""),m_isCcaas(isCcaas),
  m_ClientPtr(client),m_ssl_context(NULL),m_sockfd(-1)
{
  XGLOG_INFO ("URInhousesttclient called with this_ptr: %p client_ptr: %p", this, client);
  m_connInfo = CONFIGSERVICE->m_sttProfile.m_inhouseConnInfo;
}

URInhousesttclient::~URInhousesttclient()
{
  m_isRecogProcessing = false;
  Inhouse_websocket_close();
}

bool URInhousesttclient::create_socket()
{
  SttConnInfo connInfo = CONFIGSERVICE->m_sttProfile.m_inhouseConnInfo;
  struct sockaddr_in server_addr;
  struct hostent* server;

  // Create socket
  m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (m_sockfd < 0) {
    XGLOG_ERROR("URInhousesttclient::create_socket Error opening socket");
    return false;
  }

  // Resolve hostname to IP address
  server = gethostbyname(connInfo.m_strServerHost.c_str());
  if (server == nullptr) {
    XGLOG_ERROR("URInhousesttclient::create_socket No such host : %s",connInfo.m_strServerHost.c_str());
    close(m_sockfd);
    return false;
  }
  // Set up server address structure
  std::memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  std::memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);
  server_addr.sin_port = htons(atoi(connInfo.m_strServerPort.c_str()));

  // Connect to server
  int resp= connect(m_sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if(resp < 0){
    XGLOG_ERROR("URInhousesttclient::create_socket Error connecting : %d",resp);
    close(m_sockfd);
    return false;
  }
  return true;
}

inline std::string createWebSocketHandshakeRequest(const std::string& host) {
  return "GET / HTTP/1.1\r\n"
         "Host: " + host + "\r\n"
         "Upgrade: websocket\r\n"
         "Connection: Upgrade\r\n"
         "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
         "Sec-WebSocket-Version: 13\r\n"
         "Origin: http://" + host + "\r\n"
         "\r\n";
}

// Create SSL context
SSL_CTX* URInhousesttclient::create_ssl_context() {
  const SSL_METHOD* method = TLS_client_method();
  SSL_CTX* ctx = SSL_CTX_new(method);
  SSL_CTX_set_options(ctx, SSL_OP_NO_TICKET);
  if (!ctx) {
    XGLOG_ERROR("URInhousesttclient::create_ssl_context Unable to create SSL context");
    return NULL;
  }
  return ctx;
}

// Initialize OpenSSL
void URInhousesttclient::initialize_ssl() {
  SSL_load_error_strings();
  OpenSSL_add_ssl_algorithms();
}

bool URInhousesttclient::Inhouse_websocket_connect_ssl(const SttConnInfo& connInfo) {
  // Initialize SSL
  initialize_ssl();
  m_ssl_context = SSL_new(create_ssl_context());

  if (!m_ssl_context) {
    XGLOG_ERROR("URInhousesttclient::Inhouse_websocket_connect_ssl SSL context is NULL!!!");
    return false;
  }

  if (!create_socket()) {
    XGLOG_ERROR("URInhousesttclient::Inhouse_websocket_connect_ssl Socket creation failed!!!");
    return false;
  }

  SSL_set_fd(m_ssl_context, m_sockfd);

  // Perform SSL handshake
  if (SSL_connect(m_ssl_context) <= 0) {
    XGLOG_ERROR("URInhousesttclient::Inhouse_websocket_connect_ssl Error during SSL handshake");
    SSL_free(m_ssl_context);
    close(m_sockfd);
    return false;
  }

  // Send WebSocket handshake request over SSL
  std::string request = createWebSocketHandshakeRequest(connInfo.m_strServerHost);
  if (SSL_write(m_ssl_context, request.c_str(), request.size()) <= 0) {
    XGLOG_ERROR("URInhousesttclient::Inhouse_websocket_connect_ssl Error writing to SSL socket");
    ERR_print_errors_fp(stderr);
    SSL_free(m_ssl_context);
    close(m_sockfd);
    return false;
  }

  // Read WebSocket handshake response over SSL
  char buffer[1024];
  int n = SSL_read(m_ssl_context, buffer, sizeof(buffer) - 1);
  if (n < 0) {
    XGLOG_ERROR("URInhousesttclient::Inhouse_websocket_connect_ssl Error reading from SSL socket");
    SSL_free(m_ssl_context);
    close(m_sockfd);
    return false;
  }
  buffer[n] = '\0'; // Null-terminate the buffer
  std::string response(buffer, n);

  // Check for successful WebSocket handshake
  if (response.find("HTTP/1.1 101 Switching Protocols") == std::string::npos && 
    response.find("HTTP/1.1 200 OK") == std::string::npos) {
    XGLOG_ERROR("Failed to establish SSL WebSocket connection");
    return false;
  }

  return true;
}

bool URInhousesttclient::perform_handshake_no_ssl(const std::string& host, int port) {
  // Create and connect socket
  if (!create_socket()) {
    XGLOG_ERROR("SURInhousesttclient::perform_handshake_no_ssl socket creation failed!!!");
    return false;
  }

  // Send WebSocket handshake request
  std::string request = createWebSocketHandshakeRequest(host);

  if (send(m_sockfd, request.c_str(), request.size(), 0) < 0) {
    XGLOG_ERROR("URInhousesttclient::perform_handshake_no_ssl Error sending request: %s ",strerror(errno));
    close(m_sockfd);
    return false;
  }

  return true;
}

bool URInhousesttclient::Inhouse_websocket_connect()
{
  SttConnInfo connInfo = CONFIGSERVICE->m_sttProfile.m_inhouseConnInfo;
  Client             *client = NULL;
  client = getClientPtr();

  std::string  host(""), port("");
  host = m_connInfo.m_strServerHost;
  port = m_connInfo.m_strServerPort;
#ifdef WS_CONN_URL
  if(!Inhouse_websocket_connect_ssl(connInfo)){
    XGLOG_ERROR("URInhousesttclient::Inhouse_websocket_connect Failed to make ssl connection");
    return false;
  }
#else
  if(!perform_handshake_no_ssl(host, stoi(port))){
    XGLOG_ERROR("URInhousesttclient::Inhouse_websocket_connect Failed to make non-ssl connection");
    return false; 
  }
#endif
  XGLOG_INFO("Inhouse_websocket_connect Connection Established for connection Id(%s) roomId(%s) ",
            m_connid.c_str(),m_roomid.c_str());

  if(!sendStartRecognitionReq()){
    XGLOG_ERROR("URInhousesttclient::Inhouse_websocket_connect Failed to send start recognition request");
    return false;
  }

  if(client){
    XGLOG_INFO ("creating voice capture channel for call_Id: %s",
                    client->m_detail.m_callId.c_str());
    if(!client->create_voice_capture_channel()){
      XGLOG_ERROR( "creating voice capture channel for call_id: %s failed!!",
                             client->m_detail.m_callId.c_str());
      return false;
    }
  }
  return true;
}

bool URInhousesttclient::sendStartRecognitionReq() {
  std::string message;
  std::ostringstream oss;
  oss << "{\"meeting_id\":\"" << m_roomid << "\",\"connection_id\":\"" << m_connid
      << "\",\"device_id\":\"" << m_deviceid << "\",\"is_ccaas\":" << m_isCcaas << "}";
  message = oss.str();

  // Create WebSocket frame (text frame)
  std::string frame;
  frame += '\x81';  // FIN bit set, text frame (opcode 0x1)

  // Determine payload length field
  if (message.size() <= 125) {
    frame += static_cast<char>(0x80 | message.size()); // MASK bit set + payload length
  }
  else {
    frame += static_cast<char>(0x80 | 0x7E); // MASK bit set + extended payload length indicator
    frame += static_cast<char>((message.size() >> 8) & 0xFF); // 2 bytes length
    frame += static_cast<char>(message.size() & 0xFF);
  }

  // Generate a random 4-byte masking key
  uint8_t masking_key[4];
  std::srand(static_cast<unsigned int>(std::time(nullptr))); // Seed for randomness
  for (int i = 0; i < 4; ++i) {
    masking_key[i] = std::rand() % 256;
    frame += static_cast<char>(masking_key[i]); // Append the masking key to the frame
  }

  // Mask the payload data
  std::vector<uint8_t> masked_data(message.begin(), message.end());
  for (size_t i = 0; i < masked_data.size(); ++i) {
    masked_data[i] ^= masking_key[i % 4];  // Apply masking key
  }

  // Append masked data to the frame
  frame.append(reinterpret_cast<const char*>(masked_data.data()), masked_data.size());

#ifdef WS_CONN_URL
  // Send WebSocket frame over SSL
  if (SSL_write(m_ssl_context, frame.c_str(), frame.size()) <= 0) {
    ERR_print_errors_fp(stderr);
    XGLOG_ERROR("URInhousesttclient::sendStartRecognitionReq Error writing frame to SSL socket");
    XGLOG_ERROR("Failed to send data. Reason: %s", strerror(errno));
    SSL_free(m_ssl_context);
    close(m_sockfd);
    return false;
  }

#else
  // Send WebSocket frame over plain TCP
  ssize_t bytes_sent = send(m_sockfd, frame.c_str(), frame.size(), 0);
  if (bytes_sent < 0) {
    XGLOG_ERROR("URInhousesttclient::sendStartRecognitionReq Error writing frame to plain TCP socket");
    XGLOG_ERROR("Failed to send data. Reason: %s", strerror(errno));
    close(m_sockfd);
    return false;
  }

#endif
  XGLOG_INFO("URInhousesttclient::sendStartRecognitionReq Sent Message: %s",message.c_str());
  m_isRecogProcessing = true;
  XGLOG_INFO("URInhousesttclient::sendStartRecognitionReq recognition started for connection id (%s)", m_connid.c_str());
  return true;
}

//closing websocket connection for the Speech matics Server
void URInhousesttclient::Inhouse_websocket_close()
{
  XGLOG_INFO("URInhousesttclient::Inhouse_websocket_close closing connection for connection id (%s)" , m_connid.c_str());
  try{
    if (m_ssl_context) {
      SSL_shutdown(m_ssl_context);
      SSL_free(m_ssl_context);
    }
    if (m_sockfd >= 0) {
      close(m_sockfd);
    }
  }
  catch(std::exception const& exp)
  {
    XGLOG_ERROR("URInhousesttclient::Inhouse_websocket_close error(%s) for connection id(%s)",exp.what(),m_connid.c_str());
    std::cout << "URInhousesttclient::Inhouse_websocket_close Error: " << exp.what() << std::endl;
  }
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

