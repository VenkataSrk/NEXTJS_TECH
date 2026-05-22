
//local includes
#include "xglog.h"
#include "xGateHmpGstUtil.h"
#define THISMODULE "GstUtil"

guint16 xGateHmpGstUtil::m_dtmfLowDurationLimit = 0;

/*common utility functions start
*/
GSocket * xGateHmpGstUtil::create_udpsocket(gint udpPort)
{
  XGLOG_INFO( "create_udpsocket for udp port:%d", udpPort);
  GSocket* udpSocket = NULL;
  gboolean retVal = FALSE;
  GError *sockErr = NULL;

  udpSocket = g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_DATAGRAM, G_SOCKET_PROTOCOL_UDP, &sockErr);
  if(!udpSocket) {
    if(sockErr) {
      XGLOG_ERROR( "create_udpsocket g_socket_new failed with error: %s, for port: %d", sockErr->message, udpPort);
      g_error_free(sockErr);
      sockErr = NULL;
    } else {
      XGLOG_ERROR( "create_udpsocket g_socket_new failed for port: %d", udpPort);
    }
    return NULL;
  }

  GInetAddress* address = g_inet_address_new_any (G_SOCKET_FAMILY_IPV4);
  GSocketAddress* gsockaddress =g_inet_socket_address_new(address, udpPort);
  retVal = g_socket_bind(udpSocket, gsockaddress, TRUE, &sockErr);
  g_object_unref(gsockaddress);
  g_object_unref(address);
  if(sockErr) {
    XGLOG_ERROR( "create_udpsocket got socket bind error: %s, for port: %d", sockErr->message, udpPort);
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
  XGLOG_INFO("GENERATED UNIQUE STR %s\n",pout);
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
