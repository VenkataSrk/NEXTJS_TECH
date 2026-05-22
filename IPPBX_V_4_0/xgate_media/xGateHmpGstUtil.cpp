
//local includes
#include "xglog.h"
#include "xGateHmpGstUtil.h"

#define THISMODULE "GstUtil"

/*common utility functions start
*/
GSocket * xGateHmpGstUtil::create_udpsocket(gint udpPort)
{
  XGLOG_INFO("create_udpsocket for udp port:%d", udpPort);
  GSocket* udpSocket = NULL;
  gboolean retVal = FALSE;
  GError *sockErr = NULL;

  udpSocket = g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_DATAGRAM, G_SOCKET_PROTOCOL_UDP, &sockErr);
  if(!udpSocket) {
    if(sockErr) {
      XGLOG_ERROR("create_udpsocket g_socket_new failed with error: %s, for port: %s", sockErr->message, udpPort);
      g_error_free(sockErr);
      sockErr = NULL;
    } else {
      XGLOG_ERROR("create_udpsocket g_socket_new failed for port: %s", udpPort);
    }
    return NULL;
  }

  GInetAddress* address = g_inet_address_new_any (G_SOCKET_FAMILY_IPV4);
  GSocketAddress* gsockaddress =g_inet_socket_address_new(address, udpPort);
  retVal = g_socket_bind(udpSocket, gsockaddress, TRUE, &sockErr);
  g_object_unref(gsockaddress);
  g_object_unref(address);
  if(sockErr) {
    XGLOG_ERROR("create_udpsocket got socket bind error: %s, for port: %d", sockErr->message, udpPort);
    g_error_free(sockErr);
    return NULL;
  }

  if(!udpSocket) {
    XGLOG_ERROR("create_udpsocket failed to create valide udp socket");
    return NULL;
  }

  XGLOG_INFO("create_udpsocket success for port: %d", udpPort);
  return udpSocket;
}

void xGateHmpGstUtil::generateUniqueStr(char *pout, int len)
{
  int i;
  for(i = 0; i < len; i++)
  {
    pout[i] = CONNECTSTR[rand()%CONNECTSTR_LEN];
  }
  pout[i]='\0';
  XGLOG_INFO("GENERATED UNIQUE STR %s\n",pout);
}
unsigned int xGateHmpGstUtil::find_wav_duration(string path){
  
        unsigned int file_size=0,byterate,duration=0;
        unsigned char buffer[4];
        char filepath[path.length()];
        strcpy(filepath,path.c_str());
        FILE *fptr = fopen(filepath,"rb");
        if(fptr==NULL){
                XGLOG_ERROR("xGateHmpGstUtil::find_wav_duration,doesn't exist %s ",(char*)ur_log_string(path));
                return 0;
        }
        fseek(fptr,0,SEEK_END);
        file_size=ftell(fptr);
        rewind(fptr);
        fseek(fptr,+28,SEEK_SET);
        fread(buffer,sizeof(buffer),1,fptr);
        byterate  = buffer[0] |(buffer[1] << 8) |(buffer[2] << 16) | (buffer[3] << 24);
        if(byterate)
        duration=file_size/byterate;
        fclose(fptr);
        return duration;
}

unsigned int xGateHmpGstUtil::file_size(string path){

        unsigned int file_size=0;
        char filepath[path.length()];
        strcpy(filepath,path.c_str());
        FILE *fptr=fopen(filepath,"rb");
        if(fptr==NULL){
                XGLOG_ERROR("xGateHmpGstUtil::find_wav_duration,doesn't exist %s ",(char*)ur_log_string(path));
                return 0;
        }
        fseek(fptr,0,SEEK_END);
        file_size=ftell(fptr);
        fclose(fptr);
        return (file_size/1024);

}


void xGateHmpGstUtil::makeDir(const char *dir)
{
  char tmp[256];
  char *p = NULL;
  size_t len;

  snprintf(tmp, sizeof(tmp),"%s",dir);
  len = strlen(tmp);
  if(tmp[len - 1] == '/')
    tmp[len - 1] = 0;
  for(p = tmp + 1; *p; p++)
    if(*p == '/') {
      *p = 0;
      mkdir(tmp, S_IRWXU);
      *p = '/';
    }
  mkdir(tmp, S_IRWXU);
}

AudioDetail::AudioDetail():
  m_mediaType(0), m_fmtp(""), m_codec(0), m_codecName(""), m_ptime(0),m_playFile(""),
  m_clientIp(""), m_clientPort(0), m_serverIp(""), m_serverPort(0), m_relayIp(""), m_relayPort(0),
  m_rflxIp(""), m_rflxPort(0), m_dialIp(""), m_dialPort(0), m_respIp(""), m_respPort(0)
{
}

AudioDetail::~AudioDetail()
{
}

AudioDetail& AudioDetail::operator=(AudioDetail &src)
{
  m_mediaType = src.m_mediaType;
  m_fmtp = src.m_fmtp;
  m_codec = src.m_codec;
  m_codecName = src.m_codecName;
  m_ptime = src.m_ptime;
  m_playFile = src.m_playFile;
  m_clientIp = src.m_clientIp;
  m_clientPort = src.m_clientPort;
  m_serverIp = src.m_serverIp;
  m_serverPort = src.m_serverPort;
  m_relayIp = src.m_relayIp;
  m_relayPort = src.m_relayPort;
  m_rflxIp = src.m_rflxIp;
  m_rflxPort = src.m_rflxPort;
  m_secureDetail = src.m_secureDetail;
  m_dialIp = src.m_dialIp;
  m_dialPort = src.m_dialPort;
  m_respIp = src.m_respIp;
  m_respPort = src.m_respPort;
  return *this;
}


VideoDetail::VideoDetail():
  m_mediaType(0), m_fmtp(""), m_codec(0), m_codecName(""), m_frameRate(0),
  m_imageattr_x(0), m_imageattr_y(0), m_playFile(""), m_clientIp(), m_clientPort(0),
  m_serverIp(""), m_serverPort(0), m_relayIp(""), m_relayPort(0), m_rflxIp(""), m_rflxPort(0),
  m_dialIp(""), m_dialPort(0), m_respIp(""), m_respPort(0)
{
}

VideoDetail::~VideoDetail()
{
}

VideoDetail& VideoDetail::operator=(VideoDetail &src)
{
  m_mediaType = src.m_mediaType;
  m_fmtp = src.m_fmtp;
  m_codec = src.m_codec;
  m_codecName = src.m_codecName;
  m_frameRate = src.m_frameRate;
  m_imageattr_x = src.m_imageattr_x;
  m_imageattr_y = src.m_imageattr_y;
  m_playFile = src.m_playFile;
  m_clientIp = src.m_clientIp;
  m_clientPort = src.m_clientPort;
  m_serverPort = src.m_serverPort;
  m_relayIp = src.m_relayIp;
  m_relayPort = src.m_relayPort;
  m_rflxIp = src.m_rflxIp;
  m_rflxPort = src.m_rflxPort;
  m_secureDetail = src.m_secureDetail;
  m_dialIp = src.m_dialIp;
  m_dialPort = src.m_dialPort;
  m_respIp = src.m_respIp;
  m_respPort = src.m_respPort;
  return *this;
}

MediaSecureDetail::MediaSecureDetail() :
  m_lIceUfrag(""), m_lIcePwd(""), m_rIceUfrag(""), m_rIcePwd(""),
  m_lFingerPrint(""), m_rFingerPrint(""), m_pemFile(""), m_lSsrc(""),
  m_rSsrc(""), m_lCname(""), m_rCname(""), m_lMsLabel(""), m_rMsLabel(""),
  m_lLabel(""), m_rLabel("")
{
}

MediaSecureDetail::~MediaSecureDetail()
{
}

MediaSecureDetail& MediaSecureDetail::operator=(MediaSecureDetail &src)
{
  m_candidateDetail = src.m_candidateDetail;
  m_pemFile = src.m_pemFile;
  m_lIceUfrag = src.m_lIceUfrag;
  m_lIcePwd = src.m_lIcePwd;
  m_rIceUfrag = src.m_rIceUfrag;
  m_rIcePwd = src.m_rIcePwd;
  m_lFingerPrint = src.m_lFingerPrint;
  m_rFingerPrint = src.m_rFingerPrint;
  m_pemFile = src.m_pemFile;
  m_lSsrc = src.m_lSsrc;
  m_rSsrc = src.m_rSsrc;
  m_lCname = src.m_lCname;
  m_rCname = src.m_rCname;
  m_lMsLabel = src.m_lMsLabel;
  m_rMsLabel = src.m_rMsLabel;
  m_lLabel = src.m_lLabel;
  m_rLabel = src.m_rLabel;
  return *this;
}

/*common utility functions end
*/

