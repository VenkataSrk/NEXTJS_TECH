//local includes
#include "xglog.h"
#include "xGateHmpGstUtil.h"
#include "xGateAuditMsg.h"
#include "xGateAuditService.h"
#include "xGateUtil.h"
#include "xGateHmpGstBin.h"
#define THISMODULE "GstUtil"

guint16 xGateHmpGstUtil::m_dtmfLowDurationLimit = 0;
guint16 xGateHmpGstUtil::m_enabledecresampler = 0;
guint16 xGateHmpGstUtil::m_enabledecconverter = 0;
guint16 xGateHmpGstUtil::m_enableg729decresampler = 0;
guint16 xGateHmpGstUtil::m_enableg729decconverter = 0;
guint16 xGateHmpGstUtil::m_enableg729decrate = 0;

/*common utility functions start
*/
GSocket * xGateHmpGstUtil::create_udpsocket(gint udpPort,  unsigned short IpFamily = G_SOCKET_FAMILY_IPV4)
{
  XGLOG_INFO( "create_udpsocket for udp port:%d", udpPort);
  GSocket* udpSocket = NULL;
  gboolean retVal = FALSE;
  GError *sockErr = NULL;

  udpSocket = g_socket_new(IpFamily, G_SOCKET_TYPE_DATAGRAM, G_SOCKET_PROTOCOL_UDP, &sockErr);
  if(!udpSocket) {
    if(sockErr) {
      XGLOG_ERROR( "create_udpsocket g_socket_new failed with error: %s, for port: %d",(char*)ur_log_string(sockErr->message), udpPort);
      g_error_free(sockErr);
      sockErr = NULL;
    } else {
      XGLOG_ERROR( "create_udpsocket g_socket_new failed for port: %d", udpPort);
    }
    return NULL;
  }

  GInetAddress* address = g_inet_address_new_any (IpFamily);
  GSocketAddress* gsockaddress =g_inet_socket_address_new(address, udpPort);
  retVal = g_socket_bind(udpSocket, gsockaddress, TRUE, &sockErr);
  g_object_unref(gsockaddress);
  g_object_unref(address);
  if(sockErr) {
    XGLOG_ERROR( "create_udpsocket got socket bind error: %s, for port: %d",(char*)ur_log_string(sockErr->message), udpPort);
    g_error_free(sockErr);
    return NULL;
  }

  if(!udpSocket) {
    XGLOG_ERROR( "create_udpsocket failed to create valide udp socket");
    return NULL;
  }

  XGLOG_INFO( "create_udpsocket success for port: %d", udpPort);
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
  XGLOG_INFO("GENERATED UNIQUE STR %s\n",(char*)ur_log_string (pout));
}
/*common utility functions end
*/

guint16 xGateHmpGstUtil::getDtmfLowDurationLimit()
{
  return m_dtmfLowDurationLimit; 
}

void xGateHmpGstUtil::setDtmfLowDurationLimit(unsigned int value)
{
  m_dtmfLowDurationLimit = value;
}


void xGateHmpGstUtil::setenabledecresampler(unsigned int value)
{
 m_enabledecresampler = value;
}

guint16 xGateHmpGstUtil::getenabledecresampler()
{
   return m_enabledecresampler;
}

void xGateHmpGstUtil::setenabledecconverter(unsigned int value)
{
  m_enabledecconverter = value;
}

guint16 xGateHmpGstUtil::getenabledecconverter()
{
  return m_enabledecconverter;
}

void xGateHmpGstUtil::setenableg729decresampler(unsigned int value)
{
 m_enableg729decresampler = value;
}

guint16 xGateHmpGstUtil::getenableg729decresampler()
{
   return m_enableg729decresampler;
}

void xGateHmpGstUtil::setenableg729decconverter(unsigned int value)
{
  m_enableg729decconverter = value;
}

guint16 xGateHmpGstUtil::getenableg729decconverter()
{
  return m_enableg729decconverter;
}

void xGateHmpGstUtil::setenableg729decrate(unsigned int value)
{
  m_enableg729decrate = value;
}

guint16 xGateHmpGstUtil::getenableg729decrate()
{
  return m_enableg729decrate;
}
gboolean xGateHmpGstUtil::is_regular_file(string path)
{
  struct stat statbuf;
  stat(path.c_str(),&statbuf);
  if(S_ISREG(statbuf.st_mode))
     return TRUE;
 
   return FALSE;

}
guint xGateHmpGstUtil::file_size(string path)
{
  struct stat statbuf;
  stat(path.c_str(),&statbuf);
  return statbuf.st_size; 
}


