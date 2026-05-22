/*----------------------------------------------------------------* 
 * $Archive: /xGate/prosody.cpp $

 * *****************  Version 1  *****************
 * User: okY      Date: 12/01/15   Time: 23:55
 * Created in $/xGate
 * First day of gstreamer integration
 * 
 *----------------------------------------------------------------*/

/*----------------------------------------------------------------* 
 * Description:
 * This module contains gstreamer-based host media processing
 *----------------------------------------------------------------*/


#ifdef __linux__

#include "stdafx.h"

#endif

#include <sys/types.h> 
#include <sys/socket.h>
#include <vector>

//#ifndef GSTREAMER_TEST_MODE
#include "xgconfig.h"
//#endif

#ifdef XGATE_HAS_HMP

#ifndef GSTREAMER_TEST_MODE
#include "kernel.h"
#include "log.h"
#include "thread_process.h"
#endif

extern "C"
{
#include <stdlib.h>
#include <stdio.h>
#include <gst/gst.h>
#include <glib.h>
#include <gio/gio.h>
}
#include "hostmedia.h"
#include <cstring>
#include "call.h"
#include "task.h"


#define THISMODULE	"hostmedia"
#define LOCAL_IP_ADDRESS	"10.22.3.105" // added for multithreading_ippbx

#define MEMZERO(p) memset(&p,0,sizeof(p))

int debug = 1;

//#define XGATE_HAS_RTP

#define RTP_CAPS "application/x-rtp, media=(string)audio, clock-rate=(int)8000"
#define RTCP_CAPS "application/x-rtcp"

#define RAW_CAPS "audio/x-raw, channels=(int)1, rate=(int)8000"

#define PLAYBACK_DELAY_MS 40 

//////////////////////////////////////////////////////////////////////////
//
//  HostGSTBin
//
//////////////////////////////////////////////////////////////////////////

  HostGSTBin::HostGSTBin(const char* name, bool dtmfEvent)
:_binName(name), _dtmfIsEvent(dtmfEvent),_sendPipeline(NULL), _receivePipeline(NULL),_id(0), 
  _termSocket(NULL), _recordSocket(NULL), _receiveTerminalBin(NULL), _sendTerminalBin(NULL), 
  _multiUdpSink(NULL),_cnBin(NULL)
{
  CLog::Detail(THISMODULE,"\n HostGSTBin::HostGSTBin: construct HostGSTBin start \n");
  bool retVal;
  gboolean ret = TRUE;
  String tmpStr;
  char termHost[100];
  char Uri[50];
  GSocketAddress* gsockaddress;
  GstElement* audioConvertTx;
  GstElement* audioConvertRx;
  GstElement* capsFilter;
  GstElement* rtpL16Pay;
  GstElement* rtpL16Depay;

  GstElement* udpSrc;
  GstCaps * caps;
  GstPad* pad;

  // initialize socket
  _termSocket=g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_DATAGRAM, G_SOCKET_PROTOCOL_UDP, NULL);

  if(!_termSocket)
  {
    _termSocket=NULL;
    return;
  }
  //GInetAddress *address = g_inet_address_new_from_string("127.0.0.1");
  GInetAddress *address = g_inet_address_new_from_string("127.0.0.1");

  // search for available port for internal bus
  do 
  {
    _id = HOSTGST_BUS_GET_PORT();
    gsockaddress = g_inet_socket_address_new(address, _id);
    retVal = g_socket_bind(_termSocket, gsockaddress, TRUE, NULL);
    g_object_unref(gsockaddress);
  } while (!retVal);

  snprintf(termHost, sizeof(termHost), "%s:%u", "127.0.0.1", _id);
  CLog::Detail(THISMODULE,"\n HostGSTBin::HostGSTBin: binded in Host:Port [%s] Sockfd [%u]\n",termHost, _termSocket);
  //printf("\n HostGSTBin::HostGSTBin: binded in Host:Port [%s] Sockfd [%x]\n",termHost, _termSocket);

  g_object_unref(address);

  // initialize pipeline
  tmpStr=_binName+"_busSendPipeline";
  _sendPipeline= gst_pipeline_new (tmpStr.c_str());
  tmpStr=_binName+"_busReceivePipeline";
  _receivePipeline= gst_pipeline_new (tmpStr.c_str());
  tmpStr=_binName+"_busRbtPipeline";
  _rbtPipeline = gst_pipeline_new (tmpStr.c_str());
  tmpStr=_binName+"_busRecordPipeline";
  _recordPipeline= gst_pipeline_new (tmpStr.c_str());
  
  if(!_sendPipeline||!_receivePipeline)
  {
    CLog::Detail(THISMODULE, "Send or receive pipeline creation failed");
    return;
  }

  // initialize host's bus bin
  tmpStr=_binName+"_busSendBin";
  _sendTerminalBin=GST_BIN(gst_bin_new(tmpStr.c_str()));
  tmpStr=_binName+"_busReceiveBin";
  _receiveTerminalBin=GST_BIN(gst_bin_new(tmpStr.c_str()));

  // initialize terminal bin TX side(bin sink side)
  audioConvertTx=gst_element_factory_make("audioconvert", NULL);
  capsFilter=gst_element_factory_make("capsfilter", NULL);
  rtpL16Pay=gst_element_factory_make("rtpL16pay", NULL);
  _multiUdpSink=gst_element_factory_make("multiudpsink", NULL);

  magic=0xBEEFBABE;
  clear_listener();
  magic=0;

  g_object_set(G_OBJECT(_multiUdpSink),"async", FALSE, "sync", TRUE, NULL);

  caps = gst_caps_new_simple("audio/x-raw",
      "format",G_TYPE_STRING,"S16BE",
      "layout", G_TYPE_STRING,"interleaved",
      "rate",G_TYPE_INT,8000,
      "channels", G_TYPE_INT, 1,
      NULL);
  g_object_set(G_OBJECT(capsFilter),"caps",caps,NULL);
  gst_caps_unref(caps);

  if (_dtmfIsEvent) {
    gst_bin_add_many(_sendTerminalBin,_multiUdpSink, NULL);
    gst_element_link_many(GST_ELEMENT(_sendTerminalBin), _multiUdpSink, NULL);

    // add ghost/external pad
    pad = gst_element_get_static_pad (_multiUdpSink, "sink");
    gst_element_add_pad (GST_ELEMENT(_sendTerminalBin), gst_ghost_pad_new ("sink", pad));
    gst_object_unref (GST_OBJECT (pad));
  } else {
    gst_bin_add_many(_sendTerminalBin, audioConvertTx, capsFilter, rtpL16Pay, _multiUdpSink, NULL);
    gst_element_link_many(audioConvertTx, capsFilter, rtpL16Pay, _multiUdpSink, NULL);

    // add ghost/external pad
    pad = gst_element_get_static_pad (audioConvertTx, "sink");
    gst_element_add_pad (GST_ELEMENT(_sendTerminalBin), gst_ghost_pad_new ("sink", pad));
    gst_object_unref (GST_OBJECT (pad));
  }

  // initialize terminal bin RX side(bin src side)
  sprintf(Uri, "udp://%s",termHost);
  _rtptout = 20000000;
  audioConvertRx=gst_element_factory_make("audioconvert", NULL);
  rtpL16Depay=gst_element_factory_make("rtpL16depay", NULL);

  //commented RTCP_SUPPORT
  /*_rtpJitterBuffer_r=gst_element_factory_make("rtpjitterbuffer", NULL);
  g_object_set(G_OBJECT(_rtpJitterBuffer_r), "do-lost", TRUE, NULL);
    g_object_set(G_OBJECT(_rtpJitterBuffer_r), "mode", 1, NULL);*/

  udpSrc=gst_element_factory_make("udpsrc", NULL);

  g_object_set(G_OBJECT(udpSrc),"socket",_termSocket, "close-socket", FALSE, NULL);
  g_object_set(G_OBJECT(udpSrc),"uri", Uri, NULL);
  caps= gst_caps_from_string(RTP_CAPS);
  g_object_set(G_OBJECT(udpSrc),"caps", caps, NULL);
  g_object_set(G_OBJECT(udpSrc),"timeout", _rtptout, NULL); //Murali 
  gst_caps_unref(caps);

  if (_dtmfIsEvent) {
    gst_bin_add_many(_receiveTerminalBin,udpSrc, NULL);        
    gst_element_link_many(GST_ELEMENT(_receiveTerminalBin), udpSrc, NULL);

    // add ghost/external pad
    pad = gst_element_get_static_pad (udpSrc, "src");
    gst_element_add_pad (GST_ELEMENT(_receiveTerminalBin), gst_ghost_pad_new ("src", pad));
    gst_object_unref (GST_OBJECT (pad));
  } else {
    gst_bin_add_many(GST_BIN(_receiveTerminalBin), udpSrc, rtpL16Depay, audioConvertRx, NULL);
    gst_element_link_many(udpSrc, rtpL16Depay, audioConvertRx, NULL);
    /*gst_bin_add_many(GST_BIN(_receiveTerminalBin), udpSrc, _rtpJitterBuffer_r,rtpL16Depay, audioConvertRx, NULL);
      gst_element_link_many(udpSrc, _rtpJitterBuffer_r, rtpL16Depay, audioConvertRx, NULL);*/


    // add ghost/external pad
    pad = gst_element_get_static_pad (audioConvertRx, "src");
    gst_element_add_pad (GST_ELEMENT(_receiveTerminalBin), gst_ghost_pad_new ("src", pad));
    gst_object_unref (GST_OBJECT (pad));
  }

  // finally add terminal bin to the main pipeline
  gst_bin_add_many(GST_BIN(_sendPipeline), GST_ELEMENT(_sendTerminalBin), NULL);
  gst_bin_add_many(GST_BIN(_receivePipeline), GST_ELEMENT(_receiveTerminalBin), NULL);

  magic=0xBEEFBABE;
  CLog::Detail(THISMODULE,"\n HostGSTBin::HostGSTBin: construct HostGSTBin end \n");
}


  HostGSTBin::HostGSTBin(const char* name, bool dtmfEvent, int rtp_timeout)
:_binName(name), _dtmfIsEvent(dtmfEvent),_sendPipeline(NULL), _receivePipeline(NULL),_id(0), 
  _termSocket(NULL), _recordSocket(NULL), _receiveTerminalBin(NULL), _sendTerminalBin(NULL), 
  _multiUdpSink(NULL),_cnBin(NULL),_rtptout(rtp_timeout)
{
  CLog::Detail(THISMODULE,"\n HostGSTBin::HostGSTBin: construct HostGSTBin start \n");
  bool retVal;
  gboolean ret = TRUE;
  String tmpStr;
  char termHost[100];
  char Uri[50];
  GSocketAddress* gsockaddress;
  GstElement* audioConvertTx;
  GstElement* audioConvertRx;
  GstElement* capsFilter;
  GstElement* rtpL16Pay;
  GstElement* rtpL16Depay;

  GstElement* udpSrc;
  GstCaps * caps;
  GstPad* pad;

  // initialize socket
  _termSocket=g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_DATAGRAM, G_SOCKET_PROTOCOL_UDP, NULL);

  if(!_termSocket)
  {
    _termSocket=NULL;
    return;
  }
  //GInetAddress *address = g_inet_address_new_from_string("127.0.0.1");
  GInetAddress *address = g_inet_address_new_from_string("127.0.0.1");

  // search for available port for internal bus
  do 
  {
    _id = HOSTGST_BUS_GET_PORT();
    gsockaddress = g_inet_socket_address_new(address, _id);
    retVal = g_socket_bind(_termSocket, gsockaddress, TRUE, NULL);
    g_object_unref(gsockaddress);
  } while (!retVal);

  snprintf(termHost, sizeof(termHost), "%s:%u", "127.0.0.1", _id);
  CLog::Detail(THISMODULE,"\n HostGSTBin::HostGSTBin: binded in Host:Port [%s] Sockfd [%u]\n",termHost, _termSocket);
  //printf("\n HostGSTBin::HostGSTBin: binded in Host:Port [%s] Sockfd [%x]\n",termHost, _termSocket);

  g_object_unref(address);

  // initialize pipeline
  tmpStr=_binName+"_busSendPipeline";
  _sendPipeline= gst_pipeline_new (tmpStr.c_str());
  tmpStr=_binName+"_busReceivePipeline";
  _receivePipeline= gst_pipeline_new (tmpStr.c_str());
  tmpStr=_binName+"_busRbtPipeline";
  _rbtPipeline = gst_pipeline_new (tmpStr.c_str());
  tmpStr=_binName+"_busRecordPipeline";
  _recordPipeline= gst_pipeline_new (tmpStr.c_str());  

  if(!_sendPipeline||!_receivePipeline)
  {
    CLog::Detail(THISMODULE, "Send or receive pipeline creation failed");
    return;
  }

  // initialize host's bus bin
  tmpStr=_binName+"_busSendBin";
  _sendTerminalBin=GST_BIN(gst_bin_new(tmpStr.c_str()));
  tmpStr=_binName+"_busReceiveBin";
  _receiveTerminalBin=GST_BIN(gst_bin_new(tmpStr.c_str()));

  // initialize terminal bin TX side(bin sink side)
  audioConvertTx=gst_element_factory_make("audioconvert", NULL);
  capsFilter=gst_element_factory_make("capsfilter", NULL);
  rtpL16Pay=gst_element_factory_make("rtpL16pay", NULL);
  _multiUdpSink=gst_element_factory_make("multiudpsink", NULL);

  magic=0xBEEFBABE;
  clear_listener();
  magic=0;

  g_object_set(G_OBJECT(_multiUdpSink),"async", FALSE, "sync", TRUE, NULL);

  caps = gst_caps_new_simple("audio/x-raw",
      "format",G_TYPE_STRING,"S16BE",
      "layout", G_TYPE_STRING,"interleaved",
      "rate",G_TYPE_INT,8000,
      "channels", G_TYPE_INT, 1,
      NULL);
  g_object_set(G_OBJECT(capsFilter),"caps",caps,NULL);
  gst_caps_unref(caps);

  if (_dtmfIsEvent) {
    gst_bin_add_many(_sendTerminalBin,_multiUdpSink, NULL);
    gst_element_link_many(GST_ELEMENT(_sendTerminalBin), _multiUdpSink, NULL);

    // add ghost/external pad
    pad = gst_element_get_static_pad (_multiUdpSink, "sink");
    gst_element_add_pad (GST_ELEMENT(_sendTerminalBin), gst_ghost_pad_new ("sink", pad));
    gst_object_unref (GST_OBJECT (pad));
  } else {
    gst_bin_add_many(_sendTerminalBin, audioConvertTx, capsFilter, rtpL16Pay, _multiUdpSink, NULL);
    gst_element_link_many(audioConvertTx, capsFilter, rtpL16Pay, _multiUdpSink, NULL);

    // add ghost/external pad
    pad = gst_element_get_static_pad (audioConvertTx, "sink");
    gst_element_add_pad (GST_ELEMENT(_sendTerminalBin), gst_ghost_pad_new ("sink", pad));
    gst_object_unref (GST_OBJECT (pad));
  }

  // initialize terminal bin RX side(bin src side)
  sprintf(Uri, "udp://%s",termHost);
  _timeout = _rtptout * 1000 * 1000;

  cout << " ****** Inside HostGSTBin , rtp_timeout is " <<  _timeout  << endl;
  audioConvertRx=gst_element_factory_make("audioconvert", NULL);
  rtpL16Depay=gst_element_factory_make("rtpL16depay", NULL);
  //RTCP_SUPPORT commented
  /*_rtpJitterBuffer_r=gst_element_factory_make("rtpjitterbuffer", NULL);
  g_object_set(G_OBJECT(_rtpJitterBuffer_r), "do-lost", TRUE, NULL);
    g_object_set(G_OBJECT(_rtpJitterBuffer_r), "mode", 1, NULL);*/

  udpSrc=gst_element_factory_make("udpsrc", NULL);

  g_object_set(G_OBJECT(udpSrc),"socket",_termSocket, "close-socket", FALSE, NULL);
  g_object_set(G_OBJECT(udpSrc),"uri", Uri, NULL);
  caps= gst_caps_from_string(RTP_CAPS);
  g_object_set(G_OBJECT(udpSrc),"caps", caps, NULL);
  g_object_set(G_OBJECT(udpSrc),"timeout", _timeout, NULL); //Murali 
  gst_caps_unref(caps);

  if (_dtmfIsEvent) {
    gst_bin_add_many(_receiveTerminalBin,udpSrc, NULL);        
    gst_element_link_many(GST_ELEMENT(_receiveTerminalBin), udpSrc, NULL);

    // add ghost/external pad
    pad = gst_element_get_static_pad (udpSrc, "src");
    gst_element_add_pad (GST_ELEMENT(_receiveTerminalBin), gst_ghost_pad_new ("src", pad));
    gst_object_unref (GST_OBJECT (pad));
  } else {
    gst_bin_add_many(GST_BIN(_receiveTerminalBin), udpSrc, rtpL16Depay, audioConvertRx, NULL);
    gst_element_link_many(udpSrc, rtpL16Depay, audioConvertRx, NULL);
    /*gst_bin_add_many(GST_BIN(_receiveTerminalBin), udpSrc,_rtpJitterBuffer_r, rtpL16Depay, audioConvertRx, NULL);
      gst_element_link_many(udpSrc, _rtpJitterBuffer_r, rtpL16Depay, audioConvertRx, NULL);*/

    // add ghost/external pad
    pad = gst_element_get_static_pad (audioConvertRx, "src");
    gst_element_add_pad (GST_ELEMENT(_receiveTerminalBin), gst_ghost_pad_new ("src", pad));
    gst_object_unref (GST_OBJECT (pad));
  }

  // finally add terminal bin to the main pipeline
  gst_bin_add_many(GST_BIN(_sendPipeline), GST_ELEMENT(_sendTerminalBin), NULL);
  gst_bin_add_many(GST_BIN(_receivePipeline), GST_ELEMENT(_receiveTerminalBin), NULL);

  magic=0xBEEFBABE;
  CLog::Detail(THISMODULE,"\n HostGSTBin::HostGSTBin: construct HostGSTBin end \n");
}

HostGSTBin::~HostGSTBin()
{
  CLog::Detail(THISMODULE,"\n HostGSTBin::~HostGSTBin: destruct HostGSTBin start \n");
  if(_cnBin)
  {
    delete _cnBin;
    _cnBin=NULL;
  }

  if(_termSocket) {
    g_socket_close(_termSocket, NULL);
    g_object_unref(G_OBJECT(_termSocket));
    _termSocket=NULL; 
  }

  if(_recordSocket) {
    g_socket_close(_recordSocket, NULL);
    g_object_unref(G_OBJECT(_recordSocket));
    _recordSocket=NULL; 
  }


  if(_receivePipeline) {
    gst_element_set_state(_receivePipeline, GST_STATE_NULL);
    gst_object_unref (GST_OBJECT (_receivePipeline));
  }

  if(_sendPipeline) {
    gst_element_set_state(_sendPipeline, GST_STATE_NULL);
    gst_object_unref (GST_OBJECT (_sendPipeline));
  }

  if (_rbtPipeline){
    gst_element_set_state(_rbtPipeline, GST_STATE_NULL);
    gst_object_unref (GST_OBJECT (_rbtPipeline));
  }

  if (_recordPipeline) {
    gst_element_set_state(_recordPipeline, GST_STATE_NULL);
    gst_object_unref (GST_OBJECT (_recordPipeline));
  } 
 
  CLog::Detail(THISMODULE,"HostGSTBin::~HostGSTBin: destruct HostGSTBin end \n");
}

int HostGSTBin::allocate_socket()
{
  int usock = socket(AF_INET, SOCK_STREAM, IPPROTO_UDP);
  if (usock >= 0) {
    int optval = 1;
    setsockopt(usock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
  }
  return usock;
}

int HostGSTBin::check_port_available(int port)
{
  CLog::Detail(THISMODULE, "HostGSTBin::check_port_available \n");
  int error = -1;
  int usock = allocate_socket();
  if(usock >= 0){
    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = htonl(inet_addr("0.0.0.0"));
    error = bind(usock, (struct sockaddr*) &saddr, sizeof(saddr));
  }
  return error;
}

void HostGSTBin::clear_listener()
{
  if(!isValid()) { 
    CLog::Error(THISMODULE, "HostGSTBin::clear_listener, NOT VALID OBJTECT\n");
    return;
  }

  table.clear();
  apply_listener_table();   
}

bool HostGSTBin::add_listener(HostGSTBin* recvbin)
{
  if(!isValid()) {
    CLog::Error(THISMODULE, "HostGSTBin::add_listener, NOT VALID OBJTECT\n");
    return false;
  }
  //table[recvbin]=recvbin->get_bus_id();
  table.insert(std::pair<HostGSTBin*,HostBusId>(recvbin,recvbin->get_bus_id()));
  apply_listener_table();
  return TRUE;
}

bool HostGSTBin::remove_listener(HostGSTBin* recvBin)
{
  if(!isValid()) {
    CLog::Error(THISMODULE, "HostGSTBin::remove_listener, NOT VALID OBJTECT\n");
    return false;
  }
  if(table.erase(recvBin))
  {
    apply_listener_table();
    return TRUE;
  }
  return FALSE;
}

void HostGSTBin::apply_listener_table()
{
  char tmpBuff[100];
  String tmpStr;
  if(!isValid()) {
    CLog::Error(THISMODULE, "HostGSTBin::apply_listener_table, NOT VALID OBJTECT\n");
    return false;
  }
  GstElement *targetUdpSink = NULL;

  if(_cnBin)
  {
    sprintf(tmpBuff,"127.0.0.1:%u", HOSTGST_BUS_GET_PORT());
    g_object_set(G_OBJECT(_multiUdpSink),"clients",tmpBuff, NULL);
    targetUdpSink=((HostGSTBin*)_cnBin)->_multiUdpSink;
    CLog::Detail(THISMODULE, "HostGSTBin apply_listener_table _cnBin is true\n");

  }else {
    targetUdpSink=_multiUdpSink;
    CLog::Detail(THISMODULE, "HostGSTBin apply_listener_table _cnBin is false\n");
  }

  if(table.empty())
  {
    sprintf(tmpBuff,"127.0.0.1:%u", HOSTGST_BUS_GET_PORT());
    g_object_set(G_OBJECT(targetUdpSink),"clients",tmpBuff, NULL);
    //  g_object_set(G_OBJECT(_multiUdpSink),"clients",tmpBuff, NULL);
    CLog::Detail(THISMODULE, "HostGSTBin apply_listener_table table no data %s\n",tmpBuff);
  }else
  {
    HostGSTListenerIterator iter;
    tmpStr="";
    for(iter=table.begin();iter!=table.end();++iter)
    {
      sprintf(tmpBuff,"127.0.0.1:%u,",iter->second);
      tmpStr+=tmpBuff;
    }
    //delete comma
    tmpStr=tmpStr.substr(0, tmpStr.size()-1);
    g_object_set(G_OBJECT(targetUdpSink),"clients", tmpStr.c_str(), NULL);
    // g_object_set(G_OBJECT(_multiUdpSink),"clients", tmpStr.c_str(), NULL);
    CLog::Detail(THISMODULE, "HostGSTBin apply_listener_table table with data %s\n",tmpStr);
  }
  CLog::Detail(THISMODULE, "HostGSTBin apply_listener_table IP address : %s\n", tmpBuff);
}

void HostGSTBin::activate_comfort_noise()
{
  /*String tmpStr;
    tmpStr=_binName;
    tmpStr+="_child_cn";
    if(!_cnBin)
    {
    _cnBin=new HostGSTCnBin(tmpStr.c_str(), true);
    apply_listener_table();
    }*/
  _cnBin=NULL;
}

void HostGSTBin::stop_comfort_noise()
{
  if(_cnBin)
  {
    delete _cnBin;
    _cnBin=NULL;
    apply_listener_table();
  }
}


void HostGSTBin::indicate_dtmf(const char digit, gint volume)
{
  CLog::Detail(THISMODULE, "HostGSTBin::indicate_dtmf: dtmf digit is [%s] \n", digit);

  cout << "******* Got dtmf " <<  digit << " ********" << endl;

  if(!isValid()) {
    CLog::Error(THISMODULE, "HostGSTBin::indicate_dtmf, NOT VALID OBJTECT\n");
    return false;
  }

  HostGSTListenerIterator iter;
  // broadcast dtmf event to all listener
  for(iter=table.begin();iter!=table.end();++iter)
  {
    if(iter->first->isValid())
      iter->first->dial_dtmf(digit, volume);
  }
}

gboolean HostGSTBin::handle_event_dispatcher(GstBus* bus, GstMessage* msg, gpointer data)
{
  HostGSTBin* bin=(HostGSTBin*)data;
  if(bin->isValid())
    return bin->handle_event(bus, msg);

  CLog::Error(THISMODULE, "HostGSTBin::handle_event_dispatcher, not valid GST Bin\n");
  return FALSE;
}

void HostGSTBin::on_pad_added_dispatcher(GstElement *element, GstPad *pad, gpointer data)
{
  HostGSTBin* bin=(HostGSTBin*)data;
  if(bin->isValid())
    bin->on_pad_added(element, pad);
}

GstCaps* HostGSTBin::on_request_pt_map_dispatcher(GstElement *rtpbin, guint session, guint pt, gpointer data)
{
  HostGSTBin* bin=(HostGSTBin*)data;
  if(bin->isValid())
    return bin->on_request_pt_map(rtpbin, session, pt);
  CLog::Error(THISMODULE, "HostGSTBin::handle_event_dispatcher, not valid GST Bin\n");
  return  NULL;
}

gboolean HostGSTBin::sink_event_dispatcher(GstPad *pad, GstObject *parent, GstEvent *event, gpointer userData)
{
  HostGSTBin* bin=(HostGSTBin*)pad->eventdata;
  if(bin->isValid()) 
    return bin->sink_event_handler(pad, parent, event);

}

//////////////////////////////////////////////////////////////////////////
//
//  HostGSTRtpBin
//
//////////////////////////////////////////////////////////////////////////

HostGSTRtpBin::HostGSTRtpBin(const char* name,  bool dtmfEvent) :HostGSTBin(name, dtmfEvent)
{
  CLog::Detail(THISMODULE,"\n HostGSTRtpBin::HostGSTRtpBin: construct HostGSTRtpBin start \n");
  // receiver side
  udpSrc=NULL;
  depayloader=NULL;
  rtpDtmfDepay=NULL;
  rtpBin = NULL;
  fakeSink=NULL;
  dtmfSrc=NULL;
  //  selector=NULL;
  dtmfDetect=NULL;

  // sender side
  rtpDtmfSrc=NULL;
  dtmfMux=NULL;
  udpSink=NULL;
  invertIn=NULL;
  audioInvert=NULL;
  invertMixer=NULL;
  audioConvert=NULL;

  m_mixerout_src_pad_name = 0;
  m_audiomixer_sink_pad_name = 0;
  m_filesrc_name = 0;
  m_invertmixer_name = 0;
  m_mixerout_rec_src_pad = NULL;

  sbus=NULL;
  rbus=NULL;
  tbus=NULL;

  chanSocket=NULL;
  activated=false;
  _isAddConf=false;
  _isConfCall=false;
  rbtActive=false;
  rbtCreated=false;
  port=0;

  rx_2833=101;
  tx_2833=101;
  dtmfState=DTMF_Idle;

  sbus_watch_id=0;
  rbus_watch_id=0;
  tbus_watch_id=0;

  // first activate comfort noise until incoming RTP arrive
  activate_comfort_noise();
  mediaThread=NULL;
  ivrThread=NULL;
  mDtmfDigitBuffer="";
  mDtmfDigitTimeout=0;
  tmpCall = NULL;
  CLog::Detail(THISMODULE,"\n HostGSTRtpBin::HostGSTRtpBin: construct HostGSTRtpBin end \n");
}


HostGSTRtpBin::HostGSTRtpBin(const char* name,  bool dtmfEvent, int rtptimeout ) :HostGSTBin(name, dtmfEvent, rtptimeout )
{
  CLog::Detail(THISMODULE,"\n HostGSTRtpBin::HostGSTRtpBin: construct HostGSTRtpBin start \n");
  // receiver side
  udpSrc=NULL;
  depayloader=NULL;
  rtpDtmfDepay=NULL;
  rtpBin = NULL;
  fakeSink=NULL;
  dtmfSrc=NULL;
  //  selector=NULL;
  dtmfDetect=NULL;

  // sender side
  rtpDtmfSrc=NULL;
  dtmfMux=NULL;
  udpSink=NULL;
  invertIn=NULL;
  audioInvert=NULL;
  invertMixer=NULL;
  audioConvert=NULL;

  m_mixerout_src_pad_name = 0;
  m_audiomixer_sink_pad_name = 0;
  m_filesrc_name = 0;
  m_invertmixer_name = 0;
  m_mixerout_rec_src_pad = NULL;

  sbus=NULL;
  rbus=NULL;
  tbus=NULL;
 
  chanSocket=NULL;
  activated=false;
  _isAddConf=false;
  _isConfCall=false;
  rbtActive=false;
  rbtCreated=false;
  port=0;

  rx_2833=101;
  tx_2833=101;
  dtmfState=DTMF_Idle;

  sbus_watch_id=0;
  rbus_watch_id=0;
  tbus_watch_id=0;
 
  // first activate comfort noise until incoming RTP arrive
  activate_comfort_noise();
  mediaThread=NULL;
  ivrThread=NULL;
  mDtmfDigitBuffer="";
  mDtmfDigitTimeout=0;
  tmpCall = NULL;
  CLog::Detail(THISMODULE,"\n HostGSTRtpBin::HostGSTRtpBin: construct HostGSTRtpBin end \n");
}


HostGSTRtpBin::~HostGSTRtpBin()
{
  CLog::Detail(THISMODULE,"\n HostGSTRtpBin::HostGSTRtpBin: destruct HostGSTRtpBin \n");
  if(chanSocket)
    Close();

  if(sbus) {
    gst_bus_set_sync_handler(sbus, NULL, NULL, NULL);
    gst_object_unref(sbus);
  }

  if(sbus_watch_id){
    g_source_remove (sbus_watch_id);
  }

  if(rbus) {
    gst_bus_set_sync_handler(rbus, NULL, NULL, NULL);
    gst_object_unref(rbus);
  }

  if(rbus_watch_id){
    g_source_remove (rbus_watch_id);
  }

  if(tbus) {
    gst_bus_set_sync_handler(tbus, NULL, NULL, NULL);
    gst_object_unref(tbus);
  }

  if(tbus_watch_id){
    g_source_remove (tbus_watch_id);
  }

}

bool HostGSTRtpBin::Open(unsigned short _port)
{
  CLog::Detail(THISMODULE,"HostGSTRtpBin::Open: start \n");
  gboolean retVal;
  // initialize socket
  chanSocket=g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_DATAGRAM, G_SOCKET_PROTOCOL_UDP, NULL);

  if(!chanSocket)
  {
    CLog::Error(THISMODULE, "HostGSTRtpBin::Open: Failed to create chanSocket\n");
    return false;
  }

  //GInetAddress *address = g_inet_address_new_any (G_SOCKET_FAMILY_IPV4);
  //GInetAddress *address = g_inet_address_new_any (G_SOCKET_FAMILY_IPV4);
  //GInetAddress *address = g_inet_address_new_from_string("10.22.3.94");
  //GInetAddress *address = g_inet_address_new_from_string("10.22.3.105");
  GInetAddress *address = g_inet_address_new_from_string(LOCAL_IP_ADDRESS);
  port=_port;

  GSocketAddress* gsockaddress=g_inet_socket_address_new(address, port);

  retVal=g_socket_bind(chanSocket, gsockaddress, TRUE, NULL);

  CLog::Detail(THISMODULE,"HostGSTRtpBin::Open: Port[%u] \n",port);

  g_object_unref(gsockaddress);
  g_object_unref(address);

  _rtcp_socket = g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_DATAGRAM, G_SOCKET_PROTOCOL_UDP, NULL);
  if(!_rtcp_socket)
  {
    CLog::Detail(THISMODULE, "HostGSTRtpBin::Open: Failed to create _rtcp_socket\n");
    return false;
  }
  //address = g_inet_address_new_from_string("10.22.3.94");
  //address = g_inet_address_new_from_string("10.22.3.105");
  address = g_inet_address_new_from_string(LOCAL_IP_ADDRESS);
  gsockaddress=g_inet_socket_address_new(address, port+1);
  retVal=g_socket_bind(_rtcp_socket, gsockaddress, TRUE, NULL);
  CLog::Detail(THISMODULE,"HostGSTRtpBin::Open: RTCP Port[%u] \n", port+1);
  g_object_unref(gsockaddress);
  g_object_unref(address);

  if(!retVal)
    Close();
  CLog::Detail(THISMODULE,"HostGSTRtpBin::Open: end \n");
  return retVal!=0;
}

bool HostGSTRtpBin::Activate(const char* targetHost, int codec, int ptime)
{
  GstCaps * caps;
  GstPad *sinkpad;
  GstPad *srcpad;
  gchar Uri[50];
  GstPadLinkReturn res;
  gboolean ret = TRUE;

  //someone didnt use this function correctly, send them message
  if(isActive())
    return FALSE;

  // try to activate before socket opened? you kidding!
  if(!isOpen()){
    cout << "HostGSTRtpBin::Activate isOpen FAILS" << endl;
    return FALSE;
  }

  GstElement *sendTerminalBin = GST_ELEMENT(get_send_terminal_bin());
  GstElement *receiveTerminalBin = GST_ELEMENT(get_receive_terminal_bin());
  GstElement *sendPipeline = GST_ELEMENT(get_send_pipeline());
  GstElement *receivePipeline = GST_ELEMENT(get_receive_pipeline());

  // pclock = gst_pipeline_get_clock (GST_PIPELINE (GST_BIN(sendPipeline)));
  rtptout = _rtptout * 1000 * 1000; 

  encoder = gst_element_factory_make ("alawenc", NULL);
  payloader = gst_element_factory_make ("rtppcmapay", NULL);
  CLog::Detail(THISMODULE, "******** sofia PTIME value is %d\n ********", ptime);
  if(ptime) {
    mPtime = ptime*1000000; //this is very important to control tone breaking issue at terminal.
  } else {
    mPtime = 20*1000000; //default
  }
  CLog::Detail(THISMODULE, "******** calculated value is %d\n ********", mPtime);
  g_object_set(G_OBJECT(payloader), "min-ptime", mPtime, "max-ptime", mPtime, NULL);

  decoder= gst_element_factory_make ("alawdec", NULL);
  depayloader= gst_element_factory_make ("rtppcmadepay", NULL);

  activePT = codec;
  CLog::Detail(THISMODULE,"HostGSTRtpBin::Activate : targetHost[%s], codec[%d], dtmfEvent[%s] \n",
      targetHost, codec, (_dtmfIsEvent) ? "true" : "false" );

  printf("\n HostGSTRtpBin::Activate : targetHost[%s], codec[%d], dtmfEvent[%s] \n",
      targetHost, codec, (_dtmfIsEvent) ? "true" : "false" ); 

  udpSink = gst_element_factory_make ("multiudpsink", NULL);

  // Receiver side
  g_object_set(G_OBJECT(udpSink),"socket",chanSocket, "close-socket", FALSE, NULL);
  g_object_set(G_OBJECT(udpSink),"clients",targetHost, NULL);
  g_object_set(G_OBJECT(udpSink),"async", FALSE, "sync", TRUE, NULL);

  if (_dtmfIsEvent) {       
    CLog::Detail(THISMODULE, "\n HostGSTRtpBin::Activate 1 IF _dtmfIsEvent \n");
    gst_bin_add_many(GST_BIN(receivePipeline), udpSink, NULL);
    gst_element_link_many(receiveTerminalBin, udpSink, NULL);
  } else {
    CLog::Detail(THISMODULE, "\n HostGSTRtpBin::Activate 1 ELSE _dtmfIsEvent \n");
    gst_bin_add_many(GST_BIN(receivePipeline), encoder, payloader, udpSink, NULL);
    gst_element_link_many(receiveTerminalBin, encoder, payloader, udpSink, NULL);
  }

  // Send side
  sprintf(Uri, "udp://%s",targetHost);
  udpSrc = gst_element_factory_make("udpsrc", NULL);

  caps= gst_caps_from_string(RTP_CAPS);
  g_object_set(G_OBJECT(udpSrc),"socket",chanSocket, "close-socket", FALSE, NULL);
  g_object_set(G_OBJECT(udpSrc),"uri",Uri, NULL);
  g_object_set(G_OBJECT(udpSrc),"caps", caps, NULL);
  gst_caps_unref(caps);
  g_object_set(G_OBJECT(udpSrc),"timeout", rtptout, NULL); //Murali 
  g_object_set(G_OBJECT(udpSrc),"reuse", true, NULL);

  _rtcp_src = gst_element_factory_make("udpsrc", NULL);
  caps= gst_caps_from_string(RTCP_CAPS);
  g_object_set(G_OBJECT(_rtcp_src),"caps", caps, NULL);
  gst_caps_unref(caps);
  g_object_set(G_OBJECT(_rtcp_src),"reuse", true, NULL);
  g_object_set(G_OBJECT(_rtcp_src), "socket", _rtcp_socket, "close-socket", FALSE, NULL); 

  rtpBin= gst_element_factory_make ("rtpbin", NULL);
  rtpDtmfDepay = gst_element_factory_make("rtpdtmfdepay", NULL);
  fakeSink = gst_element_factory_make("fakesink", NULL);
  dtmfDetect = gst_element_factory_make ("dtmfdetect", NULL);
  //  printf("udpSrc %02x, caps %02x, rtpBin %02x, rtpDtmfDepay %02x, fakeSink %02x, dtmfDetect %02x \n", udpSrc, caps, rtpBin, rtpDtmfDepay, fakeSink, dtmfDetect);
  if (_dtmfIsEvent) {
    gst_bin_add_many(GST_BIN(sendPipeline), udpSrc, rtpBin, rtpDtmfDepay, fakeSink, NULL);
    gst_element_link_pads(udpSrc,"src", rtpBin,"recv_rtp_sink_0");
    gst_element_link(rtpDtmfDepay, fakeSink);
  } else {
    gst_bin_add_many(GST_BIN(sendPipeline), udpSrc, _rtcp_src, rtpBin, /*rtpDtmfDepay, fakeSink,*/ depayloader, decoder, dtmfDetect, NULL);
    gst_element_link_many(depayloader, decoder, dtmfDetect, sendTerminalBin, NULL);
    gst_element_link_pads(udpSrc,"src", rtpBin, "recv_rtp_sink_0");
    gst_element_link_pads(_rtcp_src,"src", rtpBin, "recv_rtcp_sink_0");
  }

  sbus = gst_pipeline_get_bus (GST_PIPELINE (sendPipeline));
  sbus_watch_id = gst_bus_add_watch (sbus, &HostGSTBin::handle_event_dispatcher, this);

  rbus = gst_pipeline_get_bus (GST_PIPELINE (receivePipeline));
  rbus_watch_id = gst_bus_add_watch (rbus, &HostGSTBin::handle_event_dispatcher, this);

  g_signal_connect(G_OBJECT(rtpBin), "pad-added", G_CALLBACK (&HostGSTBin::on_pad_added_dispatcher), this);
  g_signal_connect(G_OBJECT(rtpBin), "request-pt-map", G_CALLBACK (&HostGSTBin::on_request_pt_map_dispatcher), this);

  activated = true;

  if( debug ) {
    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN (receivePipeline), GST_DEBUG_GRAPH_SHOW_ALL, GST_ELEMENT_NAME (get_receive_pipeline()));
    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN (sendPipeline), GST_DEBUG_GRAPH_SHOW_ALL, GST_ELEMENT_NAME (get_send_pipeline()));
  }

  CLog::Detail(THISMODULE,"\n HostGSTRtpBin::Activate : end \n");

  gst_element_set_state(sendPipeline, GST_STATE_PLAYING);
  gst_element_set_state(receivePipeline, GST_STATE_PLAYING);

  strcpy(destHost,targetHost);
  return TRUE;
}

void HostGSTRtpBin::Stop()
{ 
  CLog::Detail(THISMODULE, "HostGSTRtpBin::Stop: Stopping .... the media, rbtActive to false\n");
  //printf("\n HostGSTRtpBin::Stop: Stopping .... the media\n");

  activated=false;
  rbtActive = false;
  if(rbtCreated) {
    rbtCreated = false;
    g_free(m_filesrc_name);
  }

  std::string room_name = get_room_name();

  if(!room_name.empty() && _isConfCall) { //conference call object removing and unlinking
    HostGSTConfBin *gstConfBin = NULL;
    gstConfBin = GetConfBinFromMap(room_name.c_str());

    if (gstConfBin) {
      GstElement *confPipeline, *audioMixer, *mixerOut = NULL;
      confPipeline = GST_ELEMENT(gstConfBin->get_conf_pipeline());
      audioMixer = GST_ELEMENT(gstConfBin->get_audio_mixer());
      mixerOut = GST_ELEMENT(gstConfBin->get_mixer_out());
      ConfRemoveParty(confPipeline, audioMixer, mixerOut); 

      if (gstConfBin->ConfRemoveRoom()) {
        RemoveConfBinFromMap(room_name.c_str());
        delete gstConfBin;
        _isConfCall = false;
        return;
      }
    }
  }

}

void HostGSTRtpBin::Close()
{
  CLog::Detail(THISMODULE, "HostGSTRtpBin::Close: Closeing .... chanSocket\n"); 
  // backup source with comfort noise
  //activate_comfort_noise();
  if(chanSocket)
  {
    g_socket_close(chanSocket, NULL);
    g_object_unref(G_OBJECT(chanSocket));
    chanSocket=NULL;
  }

  if(_rtcp_socket)
  {
    g_socket_close(_rtcp_socket, NULL);
    g_object_unref(G_OBJECT(_rtcp_socket));
    _rtcp_socket=NULL;
  }

  Stop();
}

bool HostGSTRtpBin::StopFileWav(int waitTime)
{
  CLog::Detail(THISMODULE, "HostGSTRtpBin::StopFileWav: Stop playing wave file\n");
  if(rbtActive){
    GstElement* rbtPipeline=GST_ELEMENT(get_rbt_pipeline());
    gst_element_set_state(rbtPipeline, GST_STATE_NULL);
    rbtActive = false;
    _autoRepeat = false;
    listen_conf_call();
    CLog::Detail(THISMODULE, "rtpMedia true HostGSTRtpBin::StopFileWav: Stop playing wave file, rbtActive is %d \n", rbtActive);
    return true;
  } else {
    CLog::Detail(THISMODULE, "rtpMedia false HostGSTRtpBin::StopFileWav: Stop playing wave file, rbtActive is %d \n", rbtActive);
    return false;
  }
}

void HostGSTRtpBin::listen_conf_call()
{
  if(_isAddConf) {
    GstElement* confPipeline = GST_ELEMENT(get_conf_pipeline());
    GstElement* confSendTerminalBin = get_conf_send_terminal_bin();
    if(confPipeline && confSendTerminalBin) {
      GstElement* invertMixer = gst_bin_get_by_name(GST_BIN(confPipeline), m_invertmixer_name);
      GstPad *src_pad = gst_element_get_static_pad(invertMixer, "src");
      if(src_pad) {
        GstPad *peer_sink_pad = gst_pad_get_peer(src_pad);
        if(peer_sink_pad) {
          GstElement* fakesink = gst_pad_get_parent_element(peer_sink_pad);
          GstPad *conf_termbin_sink_pad = gst_element_get_static_pad(confSendTerminalBin, "sink");
          gst_element_set_state(confPipeline, GST_STATE_PAUSED);
          gst_element_set_state(fakesink, GST_STATE_NULL);
          int ret = gst_pad_unlink (src_pad, peer_sink_pad);
          gst_bin_remove(GST_BIN(confPipeline), fakesink);
          ret = gst_pad_link (src_pad, conf_termbin_sink_pad);
          gst_element_set_state(confPipeline, GST_STATE_PLAYING);
          gst_object_unref (conf_termbin_sink_pad);
          gst_object_unref(fakesink);
          gst_object_unref (peer_sink_pad);
        }
        gst_object_unref (src_pad);
      }
    }
    if( debug ) {
      GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN (confPipeline), GST_DEBUG_GRAPH_SHOW_ALL, GST_ELEMENT_NAME (confPipeline));
    }
  }
}

void HostGSTRtpBin::unlisten_conf_call()
{
  if(_isAddConf) {
    GstElement* confPipeline = GST_ELEMENT(get_conf_pipeline());
    //GstElement* confSendTerminalBin = get_conf_send_terminal_bin();
    if(confPipeline) {
      GstElement* invertMixer = gst_bin_get_by_name(GST_BIN(confPipeline), m_invertmixer_name);
      //GstPad *sink_pad = gst_element_get_static_pad(confSendTerminalBin, "sink");
      GstPad *src_pad = gst_element_get_static_pad(invertMixer, "src");
      if(src_pad) {
        GstPad *peer_sink_pad = gst_pad_get_peer(src_pad);
        if(peer_sink_pad) {
          //if peer sink pad element is "fakesink" then return false and use existing fasksink
          GstElement* peer_element = GST_ELEMENT(gst_pad_get_parent(peer_sink_pad));
          gchar *peer_name = gst_element_get_name(peer_element);
          g_print("\nHostGSTRtpBin::unlisten_conf_call parent_name %s\n", peer_name);
          if(!strncmp((const char *)peer_name,"fakesink", 8))
          {
            g_free(peer_name);
            return;
          }
          g_free(peer_name);
          //else unlink peer element and link fakesink element
          GstElement *fakeSink = gst_element_factory_make("fakesink", NULL);
          GstPad *fake_sink_pad = gst_element_get_static_pad (fakeSink, "sink");
          gst_element_set_state(confPipeline, GST_STATE_PAUSED);
          int ret = gst_pad_unlink (src_pad, peer_sink_pad);
          gst_bin_add_many(GST_BIN(confPipeline), fakeSink, NULL);
          ret = gst_pad_link (src_pad, fake_sink_pad);
          gst_element_set_state(confPipeline, GST_STATE_PLAYING);
          if (ret != GST_PAD_LINK_OK) {
            CLog::Error(THISMODULE, "Error HostGSTRtpBin::unlisten_conf_call link failed \
                between audiomixer and fakesink for conf playback with error %d\n", ret);
          }
          gst_object_unref (fake_sink_pad);
          gst_object_unref (peer_sink_pad);
        }
        gst_object_unref (src_pad);
      }
    }
    if( debug ) {
      GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN (confPipeline), GST_DEBUG_GRAPH_SHOW_ALL, GST_ELEMENT_NAME (confPipeline));
    }
  }
}

bool HostGSTRtpBin::PlayFileWav(const char *filename, bool autorepeat)
{
  CLog::Detail(THISMODULE,"HostGSTRtpBin::PlayFileWav: Start \n");
  CLog::Detail(THISMODULE,"HostGSTRtpBin::PlayFileWav: targetHost[%s], filename[%s], autorepeat[%s]\n",
      destHost, filename, autorepeat ? "true" : "false");
  //check input file is valid
  /*
     FILE* fp = fopen(filename, "r");
     if(!fp) {
     CLog::Error(THISMODULE,"HostGSTRtpBin::PlayFileWav: %s file not able to read! \n", filename);
     return FALSE;
     } else {
     fclose(fp);
     }
     */
  if(!isOpen())
    return FALSE;

  unlisten_conf_call();

  GstElement* rbtPipeline = GST_ELEMENT(get_rbt_pipeline());

  if(rbtPipeline) {
    _autoRepeat = autorepeat;
    GstElement *fileSrc, *wavParse, *alawEnc, *rtpPcmaPay, *udpSink = NULL;
    gint64 ptime;

    //check rbtPipeline created succesfully, if so use existing one
    if(rbtCreated) {
      CLog::Detail(THISMODULE,"HostGSTRtpBin::PlayFileWav using existing rbtPipeline \n");
      if(m_filesrc_name) {
        CLog::Detail(THISMODULE,"HostGSTRtpBin::PlayFileWav m_filesrc_name \n");
        fileSrc = gst_bin_get_by_name(GST_BIN(rbtPipeline), m_filesrc_name);
        if(fileSrc) {
          CLog::Detail(THISMODULE,"HostGSTRtpBin::PlayFileWav fileSrc , rbtActive = %d \n", rbtActive);
          gst_element_set_state(rbtPipeline, GST_STATE_READY);
          g_object_set(G_OBJECT(fileSrc),"location", filename, NULL);
          gst_element_set_state(rbtPipeline, GST_STATE_PLAYING);
          rbtActive = true;
        }
      }
      return true;
    }

    //first time constructing rbtPipeline. 
    CLog::Detail(THISMODULE,"HostGSTRtpBin::PlayFileWav: constructing rbtPipeline \n ");
    fileSrc = gst_element_factory_make ("filesrc", NULL);
    m_filesrc_name = gst_element_get_name(fileSrc);
    g_object_set(G_OBJECT(fileSrc),"location", filename, NULL);
    wavParse = gst_element_factory_make ("wavparse", NULL);
    alawEnc = gst_element_factory_make ("alawenc", NULL);
    rtpPcmaPay = gst_element_factory_make("rtppcmapay", NULL);
    ptime=10*1000000; //this is very important to control tone breaking issue at terminal.
    g_object_set(G_OBJECT(rtpPcmaPay),"min-ptime",ptime,"max-ptime",ptime,NULL); 
    udpSink = gst_element_factory_make("multiudpsink", NULL);
    g_object_set(G_OBJECT(udpSink),"socket",chanSocket, "close-socket", FALSE, NULL);
    g_object_set(G_OBJECT(udpSink),"clients",destHost, NULL);
    g_object_set(G_OBJECT(udpSink),"async", FALSE, "sync", TRUE, NULL);

    gst_bin_add_many(GST_BIN(rbtPipeline), fileSrc, wavParse, alawEnc, rtpPcmaPay, udpSink, NULL);
    gst_element_link_many(fileSrc, wavParse, alawEnc, rtpPcmaPay, udpSink, NULL);

    tbus = gst_pipeline_get_bus (GST_PIPELINE (rbtPipeline));
    tbus_watch_id = gst_bus_add_watch (tbus, &HostGSTBin::handle_event_dispatcher, this);

    gst_element_set_state(rbtPipeline, GST_STATE_PLAYING);

    if( debug ) {
      GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN (rbtPipeline), GST_DEBUG_GRAPH_SHOW_ALL, GST_ELEMENT_NAME (rbtPipeline));
    }

    rbtActive = true;
    CLog::Detail(THISMODULE, "HostGSTRtpBin::PlayFileWav rbtActive set to true\n");
    rbtCreated = true;
    return TRUE;
  }
  return FALSE;
}

bool HostGSTRtpBin::RecordFile(const char *filePath, HostGSTRtpBin *gstRtpBin2)
{
  rtptout =  _rtptout * 1000 * 1000;
  if(gstRtpBin2) {
    GstElement *sendPipeline1 = get_send_pipeline();
    GstElement *sendPipeline2 = gstRtpBin2->get_send_pipeline();
    GstElement* recordPipeline=GST_ELEMENT(get_record_pipeline());

    if(sendPipeline1 && sendPipeline2 && recordPipeline) {
      gst_element_set_state(sendPipeline1, GST_STATE_NULL);
      gst_element_set_state(sendPipeline2, GST_STATE_NULL);

      GstElement* audioMixer = NULL;
      gboolean bCommonElementConstruted = false; 
      std::vector<HostGSTRtpBin *> gstRtpBinList;
      gstRtpBinList.push_back(this);
      gstRtpBinList.push_back(gstRtpBin2);

      Vector<HostGSTRtpBin *>::iterator it, end=gstRtpBinList.end();
      for (it = gstRtpBinList.begin(); it!=end; ++it) {
        GSocket *recordSocket = g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_DATAGRAM, G_SOCKET_PROTOCOL_UDP, NULL);
        if(!recordSocket)
        {
          recordSocket = NULL;
          return false;
        }
        gboolean retVal = FALSE;
        GInetAddress *address = g_inet_address_new_from_string("127.0.0.1");
        short port = 0;
        //try to bind a socket to record port to get stream from terminal 
        do { 
          port = HOSTGST_BUS_GET_PORT();
          GSocketAddress *gsockaddress = g_inet_socket_address_new(address, port);
          retVal = g_socket_bind(recordSocket, gsockaddress, TRUE, NULL);
          g_object_unref(gsockaddress);
        } while (!retVal);
        g_object_unref(address);

        //once record port successfully binded set that ip:port to multiudpsink as a client
        if(retVal) {
          //set binded record socket. So it will closed properly in destructor
          HostGSTRtpBin * gstRtpBin = (HostGSTRtpBin *)*it;
          gstRtpBin->set_record_socket(recordSocket); 

          //before setting, retrieve existing client info from multiudpSink
          GstElement* mudpSink = gstRtpBin->get_send_sink_element();
          gchar *tmpIpPort;
          g_object_get(G_OBJECT(mudpSink),"clients", &tmpIpPort, NULL);
          g_print("\n multi udpsink client ip:port %s \n" , tmpIpPort);
          string clientsIpPort = tmpIpPort;
          g_free(tmpIpPort);

          //merging new client with existing client info and set in multiudpsink
          char recordPort[10];
          sprintf(recordPort,"%u", port);
          clientsIpPort += ",127.0.0.1:";
          clientsIpPort += recordPort;
          cout << clientsIpPort << endl;
          g_object_set(G_OBJECT(mudpSink),"clients", clientsIpPort.c_str(), NULL);

          //construct recordPipeline now
          gboolean res;
          //GstBus* bus;
          //construct common elements in record pipeline first
          if(!bCommonElementConstruted)
          {
            audioMixer = gst_element_factory_make("audiomixer", NULL);
            GstElement* wavEnc=gst_element_factory_make("wavenc", NULL);
            GstElement* fileSink=gst_element_factory_make("filesink", NULL);
            g_object_set(G_OBJECT(fileSink),"location", filePath,NULL);

            gst_bin_add_many(GST_BIN(recordPipeline), audioMixer, wavEnc, fileSink, NULL);
            res = gst_element_link_many (audioMixer, wavEnc, fileSink, NULL);

            if(res)
              bCommonElementConstruted = true;
            else {
              gst_bin_remove_many(GST_BIN(recordPipeline), audioMixer,  wavEnc, fileSink, NULL);
              return FALSE;
            }
          }
          if(bCommonElementConstruted) {
            GstElement *udpSrc = gst_element_factory_make("udpsrc", NULL);
            g_object_set(G_OBJECT(udpSrc),"socket", recordSocket, "close-socket", FALSE, NULL);
            GstCaps* caps= gst_caps_from_string(RTP_CAPS);
            g_object_set(G_OBJECT(udpSrc),"caps", caps, NULL);
            g_object_set(G_OBJECT(udpSrc),"timeout", rtptout, NULL); //Murali 
            gst_caps_unref(caps);
            GstElement* rtpDepay = gst_element_factory_make ("rtpL16depay", NULL);
            GstElement* audioConvert = gst_element_factory_make("audioconvert", NULL);

            gst_bin_add_many(GST_BIN(recordPipeline), udpSrc, rtpDepay, audioConvert, NULL);
            gst_element_link_many(udpSrc, rtpDepay, audioConvert, NULL);

            gst_element_link_pads(audioConvert, NULL, audioMixer, NULL);
          }
        }
      }
      gst_element_set_state(recordPipeline, GST_STATE_PLAYING);
      gst_element_set_state(sendPipeline1, GST_STATE_PLAYING);
      gst_element_set_state(sendPipeline2, GST_STATE_PLAYING);
      if( debug ) {
        GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN (recordPipeline), GST_DEBUG_GRAPH_SHOW_ALL, GST_ELEMENT_NAME (get_record_pipeline()));
      }
      recActive = true;
      return TRUE;
    }
  }
  return FALSE;
}

void HostGSTRtpBin::EndRecordFile()
{
  GstElement* recordPipeline=GST_ELEMENT(get_record_pipeline());
  gst_element_set_state(recordPipeline, GST_STATE_NULL);
  CLog::Detail(THISMODULE, "HostGSTRtpBin::EndRecordFile pipeline set to NULL state");
  return;
}

bool HostGSTRtpBin::ConfAddParty(GstElement *confPipeline, GstElement *audioMixer, GstElement *mixerOut, const char *roomName)
{
  CLog::Detail(THISMODULE,"HostGSTRtpBin::ConfAddParty : start \n");
  //printf("\n HostGSTRtpBin::ConfAddParty : start \n");
  gint64 ptime;
  GstCaps * caps;
  GstPad *sinkpad;
  GstPad *srcpad;
  gfloat degree;
  int bufm;
  GstBus* bus;

  if(!_isConfCall) { //yv used this condition check for enableTransmission fix
    GstElement *sendPipeline = GST_ELEMENT(get_send_pipeline());
    GstElement *sendTerminalBin = GST_ELEMENT(get_send_terminal_bin());
    if(sendPipeline && sendTerminalBin) {
      gst_element_set_state(sendPipeline, GST_STATE_NULL);
      if(_dtmfIsEvent) {
        gst_element_unlink_many(udpSrc, rtpBin, rtpDtmfDepay, fakeSink, sendTerminalBin, NULL);
        gst_bin_remove_many(GST_BIN(sendPipeline), udpSrc, rtpBin, rtpDtmfDepay, fakeSink, sendTerminalBin, NULL);
      } else {
        gst_element_unlink_many(udpSrc, rtpBin, rtpDtmfDepay, fakeSink, depayloader, decoder, dtmfDetect, sendTerminalBin, NULL);
        gst_bin_remove_many(GST_BIN(sendPipeline), udpSrc, rtpBin, rtpDtmfDepay, fakeSink, depayloader, decoder, dtmfDetect, sendTerminalBin, NULL);
      }
      rtpDtmfDepay = NULL; //on_pad_added bug fix for 101 event
      GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN (sendPipeline), GST_DEBUG_GRAPH_SHOW_ALL, GST_ELEMENT_NAME (sendPipeline));
    }

    GstElement *receivePipeline = GST_ELEMENT(get_receive_pipeline());
    GstElement *receiveTerminalBin = GST_ELEMENT(get_receive_terminal_bin());
    if(receivePipeline && receiveTerminalBin) {
      gst_element_set_state(receivePipeline, GST_STATE_NULL);
      if(_dtmfIsEvent) {
        gst_element_unlink_many(udpSink, receiveTerminalBin, NULL);
        gst_bin_remove_many(GST_BIN(receivePipeline), udpSink, receiveTerminalBin, NULL);
      } else {
        gst_element_unlink_many(encoder, payloader, udpSink, receiveTerminalBin, NULL);
        gst_bin_remove_many(GST_BIN(receivePipeline), encoder, payloader, udpSink, receiveTerminalBin, NULL);
      }
      GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN (receivePipeline), GST_DEBUG_GRAPH_SHOW_ALL, GST_ELEMENT_NAME (receivePipeline));
    }
  }

  _isConfCall = true;
  if(_isAddConf)
    return FALSE;

  string tmpStr = get_name();
  tmpStr += "_busConfSendBin";
  confSendTermBin = GST_ELEMENT(GST_BIN(gst_bin_new(tmpStr.c_str())));

  audioConvert=gst_element_factory_make("audioconvert", NULL);
  encoder = gst_element_factory_make ("alawenc", NULL);
  payloader = gst_element_factory_make("rtppcmapay", NULL);
  udpSink = gst_element_factory_make("multiudpsink", NULL);

  g_object_set(G_OBJECT(udpSink),"socket",chanSocket, "close-socket", FALSE, NULL);
  g_object_set(G_OBJECT(udpSink),"clients",destHost, NULL);
  g_object_set(G_OBJECT(udpSink),"async", FALSE, "sync", TRUE, NULL);

  ptime=10*1000000;
  g_object_set(G_OBJECT(payloader),"min-ptime",ptime,"max-ptime",ptime,NULL);

  gst_bin_add_many(GST_BIN(confSendTermBin), audioConvert, encoder, payloader, udpSink, NULL);
  gst_element_link_many(audioConvert, encoder, payloader, udpSink, NULL);   

  sinkpad = gst_element_get_static_pad (audioConvert, "sink");
  gst_element_add_pad (GST_ELEMENT(confSendTermBin), gst_ghost_pad_new ("sink", sinkpad));
  gst_object_unref (GST_OBJECT (sinkpad));

  udpSrc = gst_element_factory_make("udpsrc", NULL);
  caps= gst_caps_from_string(RTP_CAPS);
  g_object_set(G_OBJECT(udpSrc),"socket",chanSocket, "close-socket", FALSE, NULL);
  g_object_set(G_OBJECT(udpSrc),"caps", caps, NULL);

  g_object_set(G_OBJECT(udpSrc),"timeout", 20000000*1000, NULL); //Murali 
  gst_caps_unref(caps);

  rtpBin= gst_element_factory_make ("rtpbin", NULL);
  decoder= gst_element_factory_make ("alawdec", NULL);
  depayloader= gst_element_factory_make ("rtppcmadepay", NULL);
  dtmfDetect = gst_element_factory_make ("dtmfdetect", NULL);
  //registering for dtmf event
  sinkpad = gst_element_get_static_pad(dtmfDetect, "sink"); 
  gst_pad_set_event_function_full(sinkpad, G_CALLBACK(&HostGSTBin::sink_event_dispatcher), this, NULL);
  gst_object_unref (sinkpad);
  invertIn = gst_element_factory_make ("tee", NULL);
  audioInvert = gst_element_factory_make ("audioinvert", NULL);
  invertMixer = gst_element_factory_make("audiomixer", NULL); 
  m_invertmixer_name = gst_element_get_name(invertMixer);

  bufm = 4;
  degree = 1.0;

  g_object_set(G_OBJECT(rtpBin),"latency", PLAYBACK_DELAY_MS, NULL);
  g_object_set(G_OBJECT(rtpBin),"buffer-mode", bufm , NULL);
  g_object_set(G_OBJECT(rtpBin),"drop-on-latency", false, NULL);
  //g_object_set(G_OBJECT(rtpBin),"do-sync-event", true, NULL);
  //g_object_set(G_OBJECT(rtpBin),"do-lost", true, NULL);
  g_object_set(G_OBJECT(audioInvert),"degree", degree, NULL);

  gst_bin_add_many(GST_BIN(confPipeline), udpSrc, rtpBin, depayloader, decoder, dtmfDetect, invertIn, audioInvert, invertMixer, confSendTermBin, NULL);

  gst_element_link_many(depayloader, decoder, dtmfDetect, invertIn, NULL);

  gst_element_link_pads(udpSrc,"src", rtpBin,"recv_rtp_sink_0");

  gst_element_link_many(audioInvert, invertMixer, confSendTermBin, NULL);
  //gst_element_link_many(invertMixer, confSendTermBin, NULL);
  gst_element_sync_state_with_parent(udpSrc);
  gst_element_sync_state_with_parent(rtpBin);
  gst_element_sync_state_with_parent(depayloader);
  gst_element_sync_state_with_parent(decoder);
  gst_element_sync_state_with_parent(dtmfDetect);
  gst_element_sync_state_with_parent(invertIn);
  gst_element_sync_state_with_parent(audioInvert);
  gst_element_sync_state_with_parent(invertMixer);
  gst_element_sync_state_with_parent(confSendTermBin);


  GstPadTemplate *invert_in_src_pad = NULL;
  GstPadTemplate *audio_mixer_sink_pad = NULL;
  GstPadTemplate *mixer_out_src_pad = NULL;
  GstPadTemplate *invert_mixer_sink_pad = NULL;
  GstPadTemplate *invert_mixer_src_pad = NULL;

  GstPad *audiomixer_sinkpad, *mixerout_srcpad = NULL;

  invert_in_src_pad = gst_element_class_get_pad_template (GST_ELEMENT_GET_CLASS (invertIn), "src_%u"); 
  srcpad = gst_element_request_pad (invertIn, invert_in_src_pad, NULL, NULL);
  audio_mixer_sink_pad = gst_element_class_get_pad_template (GST_ELEMENT_GET_CLASS (audioMixer),"sink_%u");
  audiomixer_sinkpad = gst_element_request_pad (audioMixer, audio_mixer_sink_pad, NULL, NULL);
  gst_pad_link (srcpad, audiomixer_sinkpad);
  m_audiomixer_sink_pad_name = gst_pad_get_name(audiomixer_sinkpad);
  gst_object_unref(audiomixer_sinkpad);
  gst_object_unref (srcpad);

  gst_element_link_pads(invertIn, NULL, audioInvert, NULL);

  mixer_out_src_pad = gst_element_class_get_pad_template (GST_ELEMENT_GET_CLASS (mixerOut), "src_%u");
  mixerout_srcpad = gst_element_request_pad (mixerOut, mixer_out_src_pad, NULL, NULL);
  invert_mixer_sink_pad = gst_element_class_get_pad_template (GST_ELEMENT_GET_CLASS (invertMixer),"sink_%u");
  sinkpad = gst_element_request_pad (invertMixer, invert_mixer_sink_pad, NULL, NULL);
  gst_pad_link (mixerout_srcpad, sinkpad);
  m_mixerout_src_pad_name = gst_pad_get_name(mixerout_srcpad);
  gst_object_unref (sinkpad);
  gst_object_unref (mixerout_srcpad);

  // gst_element_link_pads(udpSrc,"src", rtpBin,"recv_rtp_sink_0");
  // gst_element_link(rtpDtmfDepay, fakeSink);

  g_signal_connect(G_OBJECT(rtpBin), "pad-added", G_CALLBACK (&HostGSTBin::on_pad_added_dispatcher), this);
  g_signal_connect(G_OBJECT(rtpBin), "request-pt-map", G_CALLBACK (&HostGSTBin::on_request_pt_map_dispatcher), this);
  
  sbus = gst_pipeline_get_bus (GST_PIPELINE (confPipeline));
  sbus_watch_id = gst_bus_add_watch (sbus, &HostGSTBin::handle_event_dispatcher, this);
  
  set_conf_pipeline(confPipeline);

  gst_pipeline_use_clock (GST_PIPELINE (confPipeline), gst_system_clock_obtain());
  _isAddConf = true;

  gst_element_set_state(confPipeline, GST_STATE_PLAYING);
  if( debug ) {
    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN (confPipeline), GST_DEBUG_GRAPH_SHOW_ALL, GST_ELEMENT_NAME (confPipeline));
  }
  gst_element_set_state(confPipeline, GST_STATE_PLAYING);

  return true;
}

bool HostGSTRtpBin::ConfRemoveParty(GstElement *confPipeline, GstElement *audioMixer, GstElement *mixerOut)
{
  //printf("\n HostGSTRtpBin::ConfRemoveParty:  removing member from conference\n");
  CLog::Detail(THISMODULE, "HostGSTRtpBin::ConfRemoveParty removing participant from conference");
  if(_isAddConf) {
    gst_element_set_state(confPipeline, GST_STATE_NULL);
    //gst_element_set_state(udpSrc, GST_STATE_NULL);
    gst_element_unlink_many(udpSrc, rtpBin, depayloader, decoder, dtmfDetect, invertIn, audioInvert, invertMixer, confSendTermBin, NULL);
    /*GstIterator *iter = gst_element_iterate_src_pads(rtpBin);
      GValue vPad = G_VALUE_INIT;
      GstPad* ret = NULL;
      while(gst_iterator_next(iter, &vPad) == GST_ITERATOR_OK)
      {
      ret = GST_PAD(g_value_get_object(&vPad));
    //gst_pad_activate_mode(ret, GST_PAD_MODE_NONE, FALSE);
    if(gst_pad_set_active(ret, FALSE))
    {
    g_print("\n removing rtpbin src pad \n");
    //gst_element_remove_pad (rtpBin, ret); 
    }
    }
    gst_iterator_free(iter);
    gst_element_set_state(rtpBin, GST_STATE_NULL);
    gst_element_set_state(depayloader, GST_STATE_NULL);
    gst_element_set_state(decoder, GST_STATE_NULL);
    gst_element_set_state(dtmfDetect, GST_STATE_NULL);
    gst_element_set_state(invertIn, GST_STATE_NULL);
    gst_element_set_state(audioInvert, GST_STATE_NULL);
    gst_element_set_state(invertMixer, GST_STATE_NULL);
    gst_element_set_state(confSendTermBin, GST_STATE_NULL);  */

    if(m_audiomixer_sink_pad_name) {
      GstPad *audiomixer_sinkpad = NULL;
      audiomixer_sinkpad = gst_element_get_static_pad(audioMixer, m_audiomixer_sink_pad_name);
      gst_element_unlink(invertIn, audioMixer);
      gst_element_release_request_pad (audioMixer, audiomixer_sinkpad);
      gst_object_unref(audiomixer_sinkpad);
      g_free(m_audiomixer_sink_pad_name); 
    }

    if(m_mixerout_src_pad_name) {
      GstPad  *mixerout_srcpad = NULL;
      mixerout_srcpad = gst_element_get_static_pad(mixerOut, m_mixerout_src_pad_name);
      gst_element_release_request_pad (mixerOut, mixerout_srcpad);
      gst_object_unref(mixerout_srcpad);
      g_free(m_mixerout_src_pad_name);
    }

    gst_bin_remove_many(GST_BIN(confSendTermBin), audioConvert, encoder, payloader, udpSink, NULL);
    gst_bin_remove_many(GST_BIN(confPipeline), udpSrc, rtpBin, depayloader, decoder, dtmfDetect, invertIn, audioInvert, invertMixer, confSendTermBin, NULL);
    gst_element_set_state(confPipeline, GST_STATE_PLAYING);
    _isAddConf = false;

    if( debug ) {
      GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN (confPipeline), GST_DEBUG_GRAPH_SHOW_ALL, GST_ELEMENT_NAME (confPipeline));
    }
  }
}

bool HostGSTRtpBin::ConfRecordFile(GstElement *confPipeline,  GstElement *mixerOut, const char *filename)
{
  GstElement* audioConvert;
  GstElement* wavEnc;
  GstElement*  fileSink;
  GstPadTemplate *mixer_out_src_pad;
  GstPadTemplate *audio_convert_sink_pad;
  GstPad *sinkpad;
  GstPad *srcpad;
  GstBus* bus;

  audioConvert=gst_element_factory_make("audioconvert", NULL);
  wavEnc=gst_element_factory_make("wavenc", NULL);
  fileSink=gst_element_factory_make("filesink", NULL);
  g_object_set(G_OBJECT(fileSink),"location", filename,NULL);

  gst_element_set_state(confPipeline, GST_STATE_NULL);
  gst_bin_add_many(GST_BIN(confPipeline), audioConvert, wavEnc, fileSink, NULL);
  gst_element_link_many (audioConvert,  wavEnc, fileSink, NULL);

  mixer_out_src_pad = gst_element_class_get_pad_template (GST_ELEMENT_GET_CLASS (mixerOut), "src_%u");
  //srcpad = gst_element_request_pad (mixerOut, mixer_out_src_pad, NULL, NULL);
  m_mixerout_rec_src_pad = gst_element_request_pad (mixerOut, mixer_out_src_pad, NULL, NULL);

  sinkpad = gst_element_get_static_pad (audioConvert, "sink");    
  //gst_pad_link (srcpad, sinkpad);
  gst_pad_link (m_mixerout_rec_src_pad, sinkpad);
  gst_object_unref (sinkpad);
  // gst_object_unref (srcpad);

  gst_element_set_state(confPipeline, GST_STATE_PLAYING);
  if( debug ) {
    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN (confPipeline), GST_DEBUG_GRAPH_SHOW_ALL, GST_ELEMENT_NAME (get_conf_pipeline()));
  }

  return true;
}

double TimeSpecToSeconds(struct timespec* ts)
{
  return (double)ts->tv_sec + (double)ts->tv_nsec / 1000000000.0;
}

void HostGSTRtpBin::SetDtmfDigit(int dtmfDigit)
{
  char buffer[10];
  snprintf(buffer, sizeof(buffer), "%d", dtmfDigit);
  mDtmfDigitBuffer = buffer;
  CLog::Detail(THISMODULE, "HostGSTRtpBin::setDtmfDigit : [%s]\n", buffer);
}

int HostGSTRtpBin::GetDtmfDigit(char* dtmf_digits)
{
  //check time elapsed before getting digit
  struct timespec ts;
  int retval=0;
  int rc = clock_gettime(CLOCK_MONOTONIC, &ts);
  if(TimeSpecToSeconds(&ts) > mDtmfDigitTimeout)
  {
    CLog::Detail(THISMODULE, "HostGSTRtpBin::getDtmfDigit max wait time elapsed \n");
    retval = 1;
  }
  //  cout << " *********** Inside GetDtmfDigit **********"  << endl;
  safe_strcpy(dtmf_digits, mDtmfDigitBuffer.c_str(), MAXAPPSTRLEN);
  mDtmfDigitBuffer.erase(0, string::npos);
  CLog::Detail(THISMODULE, "HostGSTRtpBin::getDtmfDigit : [%s] \n", dtmf_digits);
  return retval;
}

void HostGSTRtpBin::SetDtmfDigitTimeout(int max_time)
{
  struct timespec ts;
  int rc = clock_gettime(CLOCK_MONOTONIC, &ts);
  mDtmfDigitTimeout = TimeSpecToSeconds(&ts) + (double)max_time;
  CLog::Detail(THISMODULE, "HostGSTRtpBin::setDtmfDigitTimeout calculated elapsed time is: [%d]\n", mDtmfDigitTimeout);
}

void HostGSTRtpBin::dial_dtmf(char digit, gint volume)
{
  CLog::Detail(THISMODULE, "HostGSTRtpBin::dial_dtmf : [%s] \n", digit);
  // inittiate DTMF sending cycle
  if(dtmfState==DTMF_Idle)
  {
    dtmfState=DTMF_Play;
    dial_dtmf_start((volume&0xFFFF)<<16| (digit&0xFF));
    g_timeout_add(DIAL_DTMF_DURATION, &HostGSTRtpBin::dial_dtmf_play_end_cb, this);
  }else
  {
    dtmfQueue.push((volume&0xFFFF)<<16| (digit&0xFF));
  }
}

gboolean HostGSTRtpBin::dial_dtmf_play_end_cb(void* binPtr)
{
  CLog::Detail(THISMODULE, "HostGSTRtpBin::dial_dtmf_play_end_cb \n");
  HostGSTRtpBin* bin=(HostGSTRtpBin*)binPtr;
  if(!bin->isValid())return FALSE;

  bin->dtmfState=DTMF_Gap;
  bin->dial_dtmf_stop();
  g_timeout_add(DIAL_DTMF_GAP, &HostGSTRtpBin::dial_dtmf_gap_end_cb, binPtr);

  return FALSE;
}

gboolean HostGSTRtpBin::dial_dtmf_gap_end_cb(void* binPtr)
{
  CLog::Detail(THISMODULE, "HostGSTRtpBin::dial_dtmf_gap_end_cb \n");
  HostGSTRtpBin* bin=(HostGSTRtpBin*)binPtr;
  if(!bin->isValid())return FALSE;

  if(bin->dtmfQueue.empty())
    bin->dtmfState=DTMF_Idle;
  else
  {
    DtmfDigitVolumePair pair;
    bin->dtmfState=DTMF_Play;
    pair=bin->dtmfQueue.front();
    bin->dtmfQueue.pop();
    bin->dial_dtmf_start(pair);
    g_timeout_add(DIAL_DTMF_DURATION, &HostGSTRtpBin::dial_dtmf_play_end_cb, binPtr);
  }
  return FALSE;
}

void HostGSTRtpBin::dial_dtmf_start(DtmfDigitVolumePair pair)
{
  CLog::Detail(THISMODULE, "HostGSTRtpBin::dial_dtmf_start \n");
  GstStructure *structure;
  GstEvent* event;
  gint digit=pair&0xFF;
  gint volume=(pair>>16) &0xFFFF;
  currentPlayedDtmf=pair;
  structure = gst_structure_new ("dtmf-event",
      "type", G_TYPE_INT, 1,
      "number", G_TYPE_INT, digit,
      "volume", G_TYPE_INT, volume,
      "start", G_TYPE_BOOLEAN, TRUE, NULL);

  event = gst_event_new_custom (GST_EVENT_CUSTOM_UPSTREAM, structure);
  gst_element_send_event (GST_ELEMENT(get_receive_pipeline()), event);
}

void HostGSTRtpBin::dial_dtmf_stop()
{
  CLog::Detail(THISMODULE, "HostGSTRtpBin::dial_dtmf_stop \n");
  GstStructure *structure;
  GstEvent* event;
  gint digit=currentPlayedDtmf&0xFF;
  gint volume=(currentPlayedDtmf>>16) &0xFFFF;
  currentPlayedDtmf=0;
  structure = gst_structure_new ("dtmf-event",
      "type", G_TYPE_INT, 1,
      "number", G_TYPE_INT, digit,
      "volume", G_TYPE_INT, volume,
      "start", G_TYPE_BOOLEAN, FALSE, NULL);

  event = gst_event_new_custom (GST_EVENT_CUSTOM_UPSTREAM, structure);
  gst_element_send_event (GST_ELEMENT(get_receive_pipeline()), event);
}

gboolean HostGSTRtpBin::handle_event(GstBus* bus, GstMessage* msg)
{
  const GstStructure* structure;
  Call *call_1 = NULL;
  Call *call_tmp = NULL;
  if(tmpCall != NULL)
    call_1 = (Call *)tmpCall;
  switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_EOS:
      {
        GstElement* rbtPipeline=GST_ELEMENT(get_rbt_pipeline());
        if(rbtPipeline) {
          if(_autoRepeat){
            gst_element_set_state(rbtPipeline, GST_STATE_NULL);
            gst_element_set_state(rbtPipeline, GST_STATE_PLAYING);
            CLog::Detail(THISMODULE, "HostGSTRtpBin::handle_event : EOS encountered. Playing file again. \n");
          } else {
            if(call_1 != NULL){
              call_1->ResumeTask();
              call_1 = NULL;
            } 
            gst_element_set_state(rbtPipeline, GST_STATE_NULL);
            listen_conf_call();
            CLog::Detail(THISMODULE, "HostGSTRtpBin::handle_event : EOS encountered. Dont need to play again. \n");
          }
        }
        break;
      }
    case GST_MESSAGE_ERROR:


      cout << "**************** GST_MESSAGE_ERROR ************************ " << endl;
      gchar *debug;
      GError *error;
      gst_message_parse_error (msg, &error, &debug);
      g_free (debug);
      CLog::Detail(THISMODULE, "Error HostGSTRtpBin::handle_event : error message [%s]\n", error->message);
      g_error_free (error);
      break;
    case GST_MESSAGE_ELEMENT:
      structure=gst_message_get_structure(msg);

      if(!strcmp(gst_structure_get_name (structure),"dtmf-event"))
      {
        gint number;
        gint volume;
        gst_structure_get_int(structure, "number", &number);
        gst_structure_get_int(structure, "volume", &volume);
        g_print("\n\n DTMF event came. digit %d volume %d\n\n",number,volume);
        CLog::Detail(THISMODULE, "HostGSTRtpBin::handle_event : DTMF event came. digit %d volume %d\n", number, volume);
        SetDtmfDigit(number);
        //indicate_dtmf(number, volume);
      }

      if(!strcmp(gst_structure_get_name (structure),"GstUDPSrcTimeout")){
        cout << "****** Got udpsrctimeout 1111******* " << endl;
        if(tmpCall != NULL){
          cout << "******** tmpCall is not NULL ************" << endl;
          call_tmp = (Call *)tmpCall;
          if( call_tmp->GetState() ==  CS_Connected)
          {
            if (call_tmp->GetTask())
            {
              if (call_tmp->GetCallParameter())
              {
                cout << " call_tmp->_callParam->confFlag= " << call_tmp->GetCallParameter()->confFlag << endl;
                if (!call_tmp->GetCallParameter()->confFlag)
                {
                  call_tmp->GetTask()->RaiseException(EX_LINE_DROP, "Hostmedia", "udp_timeout");
                }
              }
            }
          }
        }else{
          cout << "****** tmpCall is NULL *******" << endl;
        } 
      } 

      if(!strcmp(gst_structure_get_name (structure),"timeout")){
        cout << "****** Got udpsrctimeout 2222******* " << endl;
      } 

      break; 
    default:
      //CLog::Detail(THISMODULE, "HostGSTRtpBin::handle_event : default %d \n", GST_MESSAGE_TYPE (msg));
      break;
  }
  return TRUE;
}

void HostGSTRtpBin::on_pad_added(GstElement *element, GstPad *pad)
{
  GstPad *sinkpad;
  GstPadLinkReturn ret;
  unsigned int sn,ssrc,pt;
  gchar *name;
  GstElement *sendTerminalBin = NULL;

  name = gst_pad_get_name (pad);
  /* We can now link this pad with the vorbis-decoder sink pad */
  //sscanf(gst_pad_get_name(pad),"recv_rtp_src_%u_%u_%u", &sn, &ssrc, &pt);
  sscanf(name,"recv_rtp_src_%u_%u_%u", &sn, &ssrc, &pt);
  printf("\n HostGSTRtpBin::on_pad_added Pad name : %s \n",name); 
  //cout << "####### HostGSTRtpBin::on_pad_added Pad name : " << name << endl;
  CLog::Detail(THISMODULE, "HostGSTRtpBin::on_pad_added Pad name : %s \n", name);
  g_free(name);

  if(pt==rx_2833) //dtmf telephone 101 payload handling
  {
    CLog::Detail(THISMODULE, "HostGSTRtpBin::on_pad_added : 1st dtmf payload came\n");
    if(rtpDtmfDepay) {
      sinkpad = gst_element_get_static_pad (rtpDtmfDepay, "sink");
      //g_print("\n linking rtpbin and rtpDtmfDepay elements \n");
      ret = gst_pad_link (pad, sinkpad);
      gst_object_unref (sinkpad);
      if (ret != GST_PAD_LINK_OK) {
        CLog::Error(THISMODULE, "Error HostGSTRtpBin::on_pad_added pad link failed \
            between rtpbin and rtpdtmfdepay with %d\n", ret);
        //g_print("pad link failed between rtpbin and rtpdtmfdepay with %d", ret);
      }
    }
  } else if(pt==13) { //comfort noise payload handling
    GstElement *parentPipeline = NULL;
    if(_isConfCall) { //conference call comfor noise handling
      CLog::Detail(THISMODULE, "_isConfCall get_conf_pipeline \n");
      parentPipeline = GST_ELEMENT(get_conf_pipeline());
    } else { //one-one call comfort noise handling
      CLog::Detail(THISMODULE, "one-one call get_send_pipeline\n");
      parentPipeline = GST_ELEMENT(get_send_pipeline()); 
    }
    if(parentPipeline) {
      GstElement *fakeSink = gst_element_factory_make("fakesink", NULL);
      sinkpad = gst_element_get_static_pad (fakeSink, "sink");
      gst_bin_add_many(GST_BIN(parentPipeline), fakeSink, NULL);
      g_print("\n linking rtpbin and fakesink elements for comfort noise \n");
      ret = gst_pad_link (pad, sinkpad);
      if (ret != GST_PAD_LINK_OK) {
        CLog::Error(THISMODULE, "Error HostGSTRtpBin::on_pad_added link failed \
            between rtpbin and fakesink for comfort noise with error %d\n", ret);
        g_print("\n pad link failed between rtpbin and fakesink for comfort noise with %d\n", ret);
      }
      gst_object_unref (sinkpad);
      gst_element_set_state(fakeSink, GST_STATE_PLAYING);
    }
  } else if(pt == 8) { //G.711 PCMA payload handling
    if(_isConfCall) { //conference call linking
      g_print("linking rtpbin and depayloader elements for conference \n");
      if(depayloader) {
        sinkpad = gst_element_get_static_pad (depayloader, "sink");
        ret = gst_pad_link (pad, sinkpad);
        gst_object_unref (sinkpad);
        if (ret != GST_PAD_LINK_OK) {
          g_print("pad link failed between rtpbin and depayloader for conference with %d\n", ret);
          CLog::Error(THISMODULE, "Error HostGSTRtpBin::on_pad_added link failed \
              between rtpbin and depayloader for conference with error %d\n", ret);
        }
      } else {
        //g_print("invalid depayloader elements for conference \n");
      }
    } else { //one-one call linking
      if (!_dtmfIsEvent) {
        if(depayloader) {
          g_print("linking rtpbin and depayloader elements for one-one call \n");
          sinkpad = gst_element_get_static_pad (depayloader, "sink");
          ret = gst_pad_link (pad, sinkpad);
          if (ret != GST_PAD_LINK_OK) {
            g_print("pad link failed between rtpbin and depayloader for one-one call with error %d\n", ret);
            CLog::Error(THISMODULE, "Error HostGSTRtpBin::on_pad_added link failed \
                between rtpbin and depayloader for one-one call with error %d\n", ret);
            GstPad * peer_pad = NULL; 
            GstElement * peer_elem = NULL;
            switch (ret) {
              case GST_PAD_LINK_WAS_LINKED:
                g_print("depayloader already bin added with rtpbin src_pad!!!!! \n"); 
                peer_pad = gst_pad_get_peer(sinkpad); 
                if(peer_pad && gst_pad_unlink(peer_pad, sinkpad)) {
                  ret = gst_pad_link (pad, sinkpad);
                  g_print("depayloader sink pad added with newly created rtpbin src host pad!!! \n");
		  CLog::Error(THISMODULE, "depayloader sink pad added with newly created rtpbin src host pad!!! \n");

                  g_print ("Yoga rtpbin pad-> %s\n", gst_element_get_name (peer_pad));
                  peer_elem = GST_ELEMENT(gst_pad_get_parent_element(peer_pad));

                  gst_object_unref(peer_pad);
                }
                break;
              default:
                break;
            }
          }
          gst_object_unref (sinkpad);
        } else {
          //g_print("invalid depayloader elements for one-one call \n");
        }
      } else {
        g_print("linking rtpbin and sendTerminalBin for one-one call \n");
        sendTerminalBin = GST_ELEMENT(get_send_terminal_bin());
        g_print("1st audio payload came, getting the ghost pad from sendterminal bin\n");
        sinkpad = gst_element_get_static_pad (GST_ELEMENT(sendTerminalBin), "sink");
        ret = gst_pad_link (pad, sinkpad);
        gst_object_unref (sinkpad);
        if (ret != GST_PAD_LINK_OK) {
          g_print("pad link failed between rtpbin and multiudpsink(inside the sendterminal bin) with %d\n", ret);
          CLog::Error(THISMODULE, "Error HostGSTRtpBin::on_pad_added pad link failed \
              with sendTerminalBin %d\n", ret);
        }
      }
    }
  }
  if (debug)
  {
    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN (GST_ELEMENT(get_send_pipeline())), GST_DEBUG_GRAPH_SHOW_ALL, GST_ELEMENT_NAME (get_send_pipeline()));
    if(_isAddConf) {
      GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN (GST_ELEMENT(get_conf_pipeline())), GST_DEBUG_GRAPH_SHOW_ALL, GST_ELEMENT_NAME (get_conf_pipeline()));
    }
  }
}

GstCaps* HostGSTRtpBin::on_request_pt_map(GstElement *rtpbin, guint session, guint pt)
{
  CLog::Detail(THISMODULE, "HostGSTRtpBin::on_request_pt_map\n");
  if(pt==101)
  {
    GstCaps * caps=gst_caps_new_simple("application/x-rtp",
        "media",G_TYPE_STRING,"audio", 
        "clock-rate",G_TYPE_INT,8000,
        "encoding-name",G_TYPE_STRING, "TELEPHONE-EVENT",
        NULL);
    return caps;
  }else
    if(pt==8|| pt==0)
    {
      GstCaps * caps=gst_caps_new_simple("application/x-rtp",
          "media",G_TYPE_STRING,"audio", 
          "clock-rate",G_TYPE_INT,8000,
          NULL);
      return caps;
    }
    else return NULL;
}

gboolean HostGSTRtpBin::sink_event_handler(GstPad *pad, GstObject *parent, GstEvent *event)
{
  gchar *parent_name = gst_element_get_name(GST_ELEMENT(parent)); 
  gboolean ret = TRUE;
  const GstStructure *s = NULL;
  //g_print("\nHostGSTRtpBin::sink_event_handler %s\n",GST_EVENT_TYPE_NAME(event));
  switch (GST_EVENT_TYPE(event)) {
    case GST_EVENT_CUSTOM_DOWNSTREAM_OOB :
      s = gst_event_get_structure (event);
      if(s) {
        if(!strcmp(gst_structure_get_name (s),"dtmf-event"))
        {
          gint number;
          gint volume;
          gst_structure_get_int(s, "number", &number);
          gst_structure_get_int(s, "volume", &volume);
          g_print("\nHostGSTRtpBin::sink_event_handler %s element triggered dtmf event\n", parent_name);
          CLog::Detail(THISMODULE, "\nHostGSTRtpBin::sink_event_handler %s element triggered dtmf event\n", parent_name);
          g_print("\nHostGSTRtpBin::sink_event_handler DTMF event came. digit %d volume %d\n",number,volume);
          CLog::Detail(THISMODULE, "HostGSTBin::HostGSTRtpBin : DTMF event came. digit %d volume %d\n", number, volume);
          SetDtmfDigit(number);
        }
      }
      ret = gst_pad_event_default (pad, parent, event);
      break;
    default:
      /* just call the default handler */
      g_print("\nHostGSTRtpBin::sink_event_handler %s handled as default event \n", GST_EVENT_TYPE_NAME(event));
      ret = gst_pad_event_default (pad, parent, event);
      break;
  }
  g_free(parent_name);
  return ret;
}

void HostGSTRtpBin::enableTransmission(bool isRtpRxEnable, bool isRtpTxEnable)
{
  CLog::Detail(THISMODULE, "HostGSTRtpBin::enableTransmission\n");
  cout << "###### HostGSTRtpBin::enableTransmission isRtpRxEnable: " << isRtpRxEnable << " isRtpTxEnable: " << isRtpTxEnable << endl;
  if(!isActive()){
    CLog::Detail(THISMODULE, "HostGSTRtpBin::enableTransmission returning \n");
    return;
  }
#if 0
  if(_isAddConf) {
    //g_print("HostGSTRtpBin::enableTransmission hold status %s\n",(isHold) ? "true" : "false");
    GstElement* confPipeline = GST_ELEMENT(get_conf_pipeline());
    gst_element_set_state(confPipeline, GST_STATE_NULL);
    gst_element_set_state(confPipeline, GST_STATE_PLAYING);
    return;
  }
#endif

  std::string room_name = get_room_name();
  if(!room_name.empty() && _isConfCall) {
    g_print("HostGSTRtpBin::enableTransmission isRtpRxEnable %d and isRtpTxEnable %d \n", isRtpRxEnable, isRtpTxEnable);
    CLog::Detail(THISMODULE, "HostGSTRtpBin::enableTransmission hold status: isRtpRxEnable =  %d and isRtpTxEnable = %d \n",isRtpRxEnable , isRtpTxEnable);
    HostGSTConfBin *gstConfBin = NULL;
    gstConfBin = GetConfBinFromMap(room_name.c_str());

    if (gstConfBin) {
      if (isRtpRxEnable && isRtpTxEnable) {
        g_print("HostGSTRtpBin::enableTransmission adding party in conf room %s \n", room_name.c_str());
        GstClock * global_clock;
        GstElement *confPipeline, *audioMixer, *mixerOut = NULL;
        confPipeline = GST_ELEMENT(gstConfBin->get_conf_pipeline());
        audioMixer = GST_ELEMENT(gstConfBin->get_audio_mixer());
        mixerOut = GST_ELEMENT(gstConfBin->get_mixer_out());
        global_clock = gstConfBin->get_global_clock();
        SetConfParam(room_name.c_str(), global_clock);
        ConfAddParty(confPipeline, audioMixer, mixerOut, room_name.c_str());
        gstConfBin->_confMember+=1;
      } else {
        g_print("HostGSTRtpBin::enableTransmission removing party from conf room %s \n", room_name.c_str());
        if (gstConfBin) {
          GstElement *confPipeline, *audioMixer, *mixerOut = NULL;
          confPipeline = GST_ELEMENT(gstConfBin->get_conf_pipeline());
          audioMixer = GST_ELEMENT(gstConfBin->get_audio_mixer());
          mixerOut = GST_ELEMENT(gstConfBin->get_mixer_out());
          ConfRemoveParty(confPipeline, audioMixer, mixerOut);
        }
      }
    }
    return;
  }

  CLog::Detail(THISMODULE, "HostGSTRtpBin::enableTransmission starting \n");
  GstElement* sendPipeline=GST_ELEMENT(get_send_pipeline());
  GstElement* receivePipeline=GST_ELEMENT(get_receive_pipeline());

  if(isRtpTxEnable){
    CLog::Detail(THISMODULE, "HostGSTRtpBin::enableTransmission sending %d\n", isRtpTxEnable);
    gst_element_set_state(receivePipeline, GST_STATE_PLAYING);
  }
  else {
    CLog::Detail(THISMODULE, "HostGSTRtpBin::enableTransmission sending %d\n", isRtpTxEnable);
    gst_element_set_state(receivePipeline, GST_STATE_NULL);
  }
  if(isRtpRxEnable) {
    CLog::Detail(THISMODULE, "HostGSTRtpBin::enableTransmission receive %d\n", isRtpRxEnable);
    gst_element_set_state(sendPipeline, GST_STATE_PLAYING);
  }
  else {
    // todo: there is bug when sending is keep going but receive has none
    // (incoming packet), then the receiver UDP socket will failed
    CLog::Detail(THISMODULE, "HostGSTRtpBin::enableTransmission receive %d\n", isRtpRxEnable);
    gst_element_set_state(sendPipeline, GST_STATE_NULL);
  }
  GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN (receivePipeline), GST_DEBUG_GRAPH_SHOW_ALL, GST_ELEMENT_NAME (get_receive_pipeline()));
  GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN (sendPipeline), GST_DEBUG_GRAPH_SHOW_ALL, GST_ELEMENT_NAME (get_send_pipeline()));

}

void HostGSTRtpBin::SetConfParam(const char *room_name, GstClock * clock)
{  _room_name=room_name;
  _clock=clock;
}

HostGSTConfBin* HostGSTRtpBin::GetConfBinFromMap(char* room_name)
{
  std::map<string, HostGSTConfBin*>::iterator it;
  it = _hmp_conf_bin_map.find(room_name);
  if(it !=_hmp_conf_bin_map.end()) {
    return it->second;
  }
  return NULL;
}

bool HostGSTRtpBin::AddConfBinInMap(char* room_name, HostGSTConfBin *conf_room)
{
  if(conf_room) {
    _hmp_conf_bin_map[room_name] = conf_room;
    return true;
  }
  return false;
}

bool HostGSTRtpBin::RemoveConfBinFromMap(char* room_name)
{
  if(_hmp_conf_bin_map.erase(room_name))
    return true;
  return false;
}

//////////////////////////////////////////////////////////////////////////
////
////  HostGSTConfBin
////
////////////////////////////////////////////////////////////////////////////
HostGSTConfBin::HostGSTConfBin(const char* name,  bool dtmfEvent) :HostGSTBin(name, dtmfEvent)
{
  CLog::Detail(THISMODULE, "HostGSTConfBin::HostGSTConfBin: Constructor\n");
  String tmpStr;
  String binName;
  audioMixer = NULL;
  mixerOut = NULL;
  _confPipeline = NULL;
  binName =  get_name();
  tmpStr=binName+"_busConfPipeline";
  _confPipeline= gst_pipeline_new (tmpStr.c_str());
  global_clock = gst_system_clock_obtain ();
  _confActive = false;
  _confMember = 0;
}

HostGSTConfBin::~HostGSTConfBin()
{
  CLog::Detail(THISMODULE, "HostGSTConfBin::~HostGSTConfBin: Destructor\n");
  //printf("\nHostGSTConfBin::~HostGSTConfBin: Destructor\n");
}

bool HostGSTConfBin::ConfCreateRoom()
{
  CLog::Detail(THISMODULE, "HostGSTConfBin::ConfCreateRoom: \n");
  gboolean res;

  GstElement *confPipeline = GST_ELEMENT(get_conf_pipeline());

  audioMixer = gst_element_factory_make("audiomixer", NULL);
  mixerOut = gst_element_factory_make("tee", NULL);

  gst_bin_add_many(GST_BIN(confPipeline), audioMixer, mixerOut, NULL);
  res = gst_element_link_many (audioMixer, mixerOut, NULL);
  g_assert (res);

  //cbus = gst_pipeline_get_bus (GST_PIPELINE (confPipeline));
  //cbus_watch_id = gst_bus_add_watch (cbus, &HostGSTBin::handle_event_dispatcher, this);

  _confActive = true;
  CLog::Detail(THISMODULE, "\nHostGSTBin::ConfCreateRoom: Room Activated \n");

  return true;
}

bool HostGSTConfBin::ConfRemoveRoom()
{
  _confMember-=1;
  if(_confMember==0) {
    GstElement *confPipeline = GST_ELEMENT(get_conf_pipeline());
    if(cbus) {
      gst_bus_set_sync_handler(cbus, NULL, NULL, NULL);
      gst_object_unref(cbus);
    }

    if(cbus_watch_id) {
      g_source_remove(cbus_watch_id);
      cbus_watch_id = 0;
    }

    if (_confPipeline) {
      gst_element_set_state(_confPipeline, GST_STATE_NULL);
      gst_object_unref (GST_OBJECT (_confPipeline));
    }
    CLog::Detail(THISMODULE, "\nHostGSTConfBin::ConfRemoveRoom: Removed Room \n"); 
    return true;
  } else return false;
}

//////////////////////////////////////////////////////////////////////////
//
//  HostGSTCnBin
//
//////////////////////////////////////////////////////////////////////////

  HostGSTCnBin::HostGSTCnBin(const char* name, bool dtmfEvent)
:HostGSTBin(name, dtmfEvent)
{
  CLog::Detail(THISMODULE, "HostGSTCnBin::HostGSTCnBin: Constructor\n");
  GstElement* audioSrc;
  GstElement* sendTerminalBin=GST_ELEMENT(get_send_terminal_bin());
  GstElement* receiveTerminalBin=GST_ELEMENT(get_receive_terminal_bin());
  GstElement* sendPipeline=GST_ELEMENT(get_send_pipeline());
  GstElement* receivePipeline=GST_ELEMENT(get_receive_pipeline());

  audioSrc=gst_element_factory_make("audiotestsrc", NULL);


  g_object_set(G_OBJECT(audioSrc),"is-live", TRUE, NULL);
  g_object_set(G_OBJECT(audioSrc),"wave", 10, NULL);
  g_object_set(G_OBJECT(audioSrc),"volume", 0.004, NULL);


  gst_bin_add_many(GST_BIN(sendPipeline), audioSrc, NULL);
  gst_element_link_many(audioSrc, sendTerminalBin, NULL);

  gst_element_set_state (sendPipeline, GST_STATE_PLAYING);
  gst_element_set_state (receivePipeline, GST_STATE_NULL);

  clear_listener();
}

HostGSTCnBin::~HostGSTCnBin()
{
  CLog::Detail(THISMODULE, "HostGSTCnBin::~HostGSTCnBin: Destructor\n");
  //gst_element_set_state (GST_ELEMENT(get_send_pipeline()), GST_STATE_NULL);
}

//////////////////////////////////////////////////////////////////////////
//
//  HostSwitchDevice
//
//////////////////////////////////////////////////////////////////////////
#ifndef GSTREAMER_TEST_MODE
/** HostSwitchDevice's constructor
*/
  HostSwitchDevice::HostSwitchDevice(const char *name, HostGSTBin* gstBin)
: SwitchDevice(name), _addrHostBus(new HostSwitchAddress(gstBin))
{ 
  CLog::Detail(THISMODULE, "HostSwitchDevice::HostSwitchDevice: Constructor\n");
  _gstBin=gstBin;
}

/** HostSwitchDevice's destructor
*/
HostSwitchDevice::~HostSwitchDevice()
{
  CLog::Detail(THISMODULE, "HostSwitchDevice::~HostSwitchDevice: Destructor\n");
  if(HostMediaProcessingDriver::cnGeneratorBin)
    HostMediaProcessingDriver::cnGeneratorBin->remove_listener(_gstBin);
  if (_addrHostBus) delete _addrHostBus;
  try{
    if(_gstBin) delete _gstBin;
  }catch(int e)
  {
    CLog::Detail(THISMODULE, "HostSwitchDevice::~HostSwitchDevice: failed to release host media channel\n");
  }
}

/** The framework will query the device to find out which 
  switching fabric is supported by this device to exchange TDM
  data with another device.
  */
SwitchFabric HostSwitchDevice::GetCompatibleFabric(SwitchDevice *dev)
{
  CLog::Detail(THISMODULE, "HostSwitchDevice::GetCompatibleFabric\n");
  if(dynamic_cast<HostSwitchDevice*>(dev))
    return SF_Host;
  else
    return SF_Invalid;
}

/** This function is called when the framework decides that it is
  the first time the specific device transmits to a particular
  fabric.
  */
SwitchAddress *HostSwitchDevice::AllocateAddress(SwitchFabric fabric) 
{
  CLog::Detail(THISMODULE, "HostSwitchDevice::AllocateAddress\n");
  switch (fabric) {
    case SF_Host:
      return _addrHostBus;

    default:
      assert(false);
      return false;
  }
}

/** This function is called when the framework detects that no device
  is currently listening to this device.
  */
void HostSwitchDevice::FreeAddress(SwitchFabric fabric)
{
  CLog::Detail(THISMODULE, "HostSwitchDevice::FreeAddress\n");
  switch (fabric) {
    case SF_Host:
      break;

    default:
      assert(false);
  }
}

/** It returns the address used for switching in the specified fabric
*/
SwitchAddress *HostSwitchDevice::GetAddress(SwitchFabric fabric)
{
  CLog::Detail(THISMODULE, "HostSwitchDevice::GetAddress\n");
  switch (fabric) {
    case SF_Host:
      return _addrHostBus;

    default:
      assert(false);
      return NULL;
  }
}

/** Host media doesnt support other switch fabric
*/
bool HostSwitchDevice::SupportAculab(SwitchDevice *requester)
{
  return false;
}

/** This device supports CTbus (H.100/H.110)
*/
bool HostSwitchDevice::SupportCTbus(SwitchDevice *requester)
{
  return false;
}

/** This device supports SCbus
*/
bool HostSwitchDevice::SupportSCbus(SwitchDevice *requester)
{
  return false;
}

/** It instructs the device to start transmitting to a certain location 
  using the specified switch fabric.
  */
void HostSwitchDevice::DeviceStartTransmit(SwitchFabric fabric,SwitchAddress *addr)
{
  if(fabric!=SF_Host)
  {
    CLog::Detail(THISMODULE,"Cannot start transmit! expected SF_Host only");
    return;
  }else if(addr!=_addrHostBus)
  {
    CLog::Detail(THISMODULE,"Invalid Switchaddress for start transmit to(addr!=_addrHostBus)");
    return;
  }
}

HostGSTCnBin * HostMediaProcessingDriver::cnGeneratorBin;
/** It instructs the device to start listening from a certain location 
  using the specified switch fabric.
  */
void HostSwitchDevice::DeviceStartReceive(SwitchFabric fabric,SwitchAddress *addr)
{
#ifndef __linux__
  HostSwitchAddress *hostAddr=dynamic_cast<HostSwitchAddress*>(addr);
#else
  HostSwitchAddress *hostAddr= (HostSwitchAddress*)(addr);
#endif
  if(hostAddr)
  {
    hostAddr->GetHostGSTBin()->add_listener(_gstBin);
    if(HostMediaProcessingDriver::cnGeneratorBin)
    {
      HostMediaProcessingDriver::cnGeneratorBin->remove_listener(_gstBin);
    }
  }else
  {
    const char* swname = addr ? addr->ToString() : "(null)";
    CLog::Error(THISMODULE,"%s unable to receive from %s using invalid fabric %d",GetName(),swname,fabric);
    return;
  }
}

/** It instructs the device to stop listening from a certain location 
  using the specified switch fabric.
  */
void HostSwitchDevice::DeviceStopTransmit(SwitchFabric fabric,SwitchAddress *addr)
{
  // pretend it always success for HostSwitch
}

/** It instructs the device to stop transmitting to a certain location 
  using the specified switch fabric. It is called when the reference
  count of the specified switch fabric reach 0.
  */
void HostSwitchDevice::DeviceStopReceive(SwitchFabric fabric, SwitchAddress *addr)
{
  CLog::Detail(THISMODULE, "HostSwitchDevice::DeviceStopReceive\n");
  HostSwitchAddress *hostAddr = dynamic_cast<HostSwitchAddress*>(addr);

  if(hostAddr)
  {
    if(HostMediaProcessingDriver::cnGeneratorBin)
    {
      HostMediaProcessingDriver::cnGeneratorBin->add_listener(_gstBin);
    }
    hostAddr->GetHostGSTBin()->remove_listener(_gstBin);
  }
}

/** Host_Rtp_Channel constructor(called just before RTP media is instantiated).
  This should also init the lower gstreamer level via HostSwitchDevice(created here)
 **/
static char Host_local_address[500]={0};
Host_Rtp_Channel::Host_Rtp_Channel(const char* itf_name, bool dtmfEvent)
{
  CLog::Detail(THISMODULE, "Host_Rtp_Channel::Host_Rtp_Channel: Constructor - with 2 arguments\n");
  char tmpStr[100];
  snprintf(tmpStr,sizeof(tmpStr), "%s_%u",itf_name, rand()&0xFFFFF);

  //bool dtmfEvent = true; //Need to come from sofiam, Dtmf - true or Tone - false 
  _gstBin=new HostGSTRtpBin(tmpStr, dtmfEvent);
  snprintf(_channel_name, sizeof(_channel_name),"%s",tmpStr);

  strcpy(_itf_name, itf_name);

  MEMZERO(_config);
  MEMZERO(_stat);
  _init_state=0;

  // HostSwitchDevice instance will take ownership of _gstBin
  _swdev=new HostSwitchDevice(tmpStr, _gstBin);

  if(HostMediaProcessingDriver::cnGeneratorBin)
    HostMediaProcessingDriver::cnGeneratorBin->add_listener(_gstBin);

  int sock;	
  if(!Host_local_address[0])
  {

    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) > 0 ) 
    {
      char abuf[50] = "";
      struct sockaddr_in remote, out;

      memset(&remote, 0, sizeof(struct sockaddr_in));
      memset(&out, 0, sizeof(struct sockaddr_in));

      remote.sin_family = AF_INET;
      //remote.sin_addr.s_addr = inet_addr("8.8.8.8");
      //remote.sin_addr.s_addr = inet_addr("127.0.0.1");
      //remote.sin_addr.s_addr = inet_addr("10.22.3.94");
      //remote.sin_addr.s_addr = inet_addr("10.22.3.105");
      remote.sin_addr.s_addr = inet_addr(LOCAL_IP_ADDRESS);
      remote.sin_port = 53;

      if (connect(sock, (struct sockaddr *) &remote, sizeof(struct sockaddr_in)) == -1) 
      {
        // get default address
      }
      int len = sizeof(out);
      if (getsockname(sock, (struct sockaddr *) &out, &len) == -1) 
      {
        // get default address
      }
      if (out.sin_addr.s_addr != 0) 
      {
        getnameinfo((struct sockaddr *) &out, len, abuf, sizeof(out), NULL, 0, NI_NUMERICHOST);
        strncpy( Host_local_address, abuf, len);		
      }

#ifndef __linux__	
      closesocket(sock);
#else
      close(sock);
#endif
    }
  }
  bool retVal;
  unsigned short portNum;
  do{
    portNum=HOSTGST_RTP_GET_PORT();
    retVal=_gstBin->Open(portNum);
    //retVal=1;
  }while(!retVal);
  snprintf(tmpStr, sizeof(tmpStr), "%s:%u", Host_local_address, portNum);

  CLog::Detail(THISMODULE,"Host_Rtp_Channel::Host_Rtp_Channel Host_local_address : %s \
      Port : %u\n",Host_local_address, portNum);
  printf("*********** Host_Rtp_Channel::Host_Rtp_Channel Host_local_address : %s \
      Port : %u**************\n",Host_local_address, portNum);

  _local_addr.string_to_addr(tmpStr);

}




Host_Rtp_Channel::Host_Rtp_Channel(const char* itf_name, bool dtmfEvent, int rtp_timeout)
{
  CLog::Detail(THISMODULE, "Host_Rtp_Channel::Host_Rtp_Channel: Constructor - with 3 arguments\n");
  char tmpStr[100];
  snprintf(tmpStr,sizeof(tmpStr), "%s_%u",itf_name, rand()&0xFFFFF);

  //bool dtmfEvent = true; //Need to come from sofiam, Dtmf - true or Tone - false 
  //_gstBin=new HostGSTRtpBin(tmpStr, dtmfEvent);
  _gstBin=new HostGSTRtpBin(tmpStr, dtmfEvent,rtp_timeout);
  snprintf(_channel_name, sizeof(_channel_name),"%s",tmpStr);

  strcpy(_itf_name, itf_name);

  MEMZERO(_config);
  MEMZERO(_stat);
  _init_state=0;

  // HostSwitchDevice instance will take ownership of _gstBin
  _swdev=new HostSwitchDevice(tmpStr, _gstBin);

  if(HostMediaProcessingDriver::cnGeneratorBin)
    HostMediaProcessingDriver::cnGeneratorBin->add_listener(_gstBin);

  int sock;	
  if(!Host_local_address[0])
  {

    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) > 0 ) 
    {
      char abuf[50] = "";
      struct sockaddr_in remote, out;

      memset(&remote, 0, sizeof(struct sockaddr_in));
      memset(&out, 0, sizeof(struct sockaddr_in));

      remote.sin_family = AF_INET;
      //remote.sin_addr.s_addr = inet_addr("8.8.8.8");
      //remote.sin_addr.s_addr = inet_addr("127.0.0.1");
      //remote.sin_addr.s_addr = inet_addr("10.22.3.94");
      remote.sin_addr.s_addr = inet_addr(LOCAL_IP_ADDRESS);
      remote.sin_port = 53;

      if (connect(sock, (struct sockaddr *) &remote, sizeof(struct sockaddr_in)) == -1) 
      {
        // get default address
      }
      int len = sizeof(out);
      if (getsockname(sock, (struct sockaddr *) &out, &len) == -1) 
      {
        // get default address
      }
      if (out.sin_addr.s_addr != 0) 
      {
        getnameinfo((struct sockaddr *) &out, len, abuf, sizeof(out), NULL, 0, NI_NUMERICHOST);
        strncpy( Host_local_address, abuf, len);		
      }

#ifndef __linux__	
      closesocket(sock);
#else
      close(sock);
#endif
    }
  }
  bool retVal;
  unsigned short portNum;
  do{
  	CLog::Detail(THISMODULE,"Host_Rtp_Channel::Host_Rtp_Channel inside do-while loop");
    portNum=HOSTGST_RTP_GET_PORT();
    retVal=_gstBin->Open(portNum);
    //retVal=1;
    //retVal=1; // temporary-testing : to be removed later : added for multithreaded_ippbx
  }while(!retVal);
  snprintf(tmpStr, sizeof(tmpStr), "%s:%u", Host_local_address, portNum);

  CLog::Detail(THISMODULE,"Host_Rtp_Channel::Host_Rtp_Channel Host_local_address : %s \
      Port : %u\n",Host_local_address, portNum);
  printf("*********** Host_Rtp_Channel::Host_Rtp_Channel Host_local_address : %s \
      Port : %u**************\n",Host_local_address, portNum);

  _local_addr.string_to_addr(tmpStr);

}







Host_Rtp_Channel::~Host_Rtp_Channel()
{
  CLog::Detail(THISMODULE, "Host_Rtp_Channel::~Host_Rtp_Channel: Destructor\n");

  _gstBin->Close();
  if(_swdev)delete _swdev;
}


bool Host_Rtp_Channel::Open(RTP_Config& config)
{
  CLog::Detail(THISMODULE, "[%s] Open()", _channel_name);

  switch (_config.DTMF.Transport_Type)
  {
    case RTP_Config::DTMF_RFC2833:
    case RTP_Config::DTMF_Inband:
      break;

    default:
      CLog::Detail(THISMODULE, "[%s] Attempt to open with unsupported dtmf transfer mode %d", _channel_name, _config.DTMF.Transport_Type);
      //*** return false;
  }

  _config = config;
  _init_state = 1;

  // just pretending, actual socket opening already done sine constructor

  return TRUE;
}

/** Activate the rtp channel. Some code on the RTP_Media that 
  depends on AudioCode frame work extracted to RTP_Channel descendant.
  */
bool Host_Rtp_Channel::Activate(const ACE_INET_Addr& addr, Rtp_Payload_Code codec,bool low, int ptime)
{
  CLog::Detail(THISMODULE, "[%s] Activate()", _channel_name);
  switch (codec)
  {
    case RTP_PT_PCMA:
    case RTP_PT_PCMU:
    case RTP_PT_G7231:
    case RTP_PT_G729:
      break;

    default:
      CLog::Detail(THISMODULE, "[%s] Attempt to open with unsupported codec %d", _channel_name, codec);

  }
  char destAddr[200];
  snprintf(destAddr, sizeof(destAddr), "%s:%u", addr.get_host_addr(),addr.get_port_number());

  _gstBin->Activate(destAddr, codec, ptime);

  _init_state = 2;
  _config.RTP.Codec = codec;
  cout << "Host_Rtp_Channel::Activate get_host_addr is " <<  addr.get_host_addr() << " and addr.get_port_number is " << addr.get_port_number() << endl;
  return true;
}

/** Close the channel. This is extracted from RTP_Media. 
  The code will be placed in the RTP_Channel dependant.
  */
void Host_Rtp_Channel::Close()
{
  // just pretending, actual socket closing is on destructor
  GstElement* confPipeline;
  GstElement* audioMixer;
  GstElement* mixerOut;
  CLog::Detail(THISMODULE, "Host_Rtp_Channel::Close: channel [%s]\n", _channel_name);
  _init_state = 0;

  // but we stop channel anyway
  _gstBin->Stop();

}

bool Host_Rtp_Channel::PlayFileWav(char const *filename, bool autorepeat)
{
  CLog::Detail(THISMODULE, "Host_Rtp_Channel::PlayFileWav: filename=%s\n, autorepeat = %d",filename,autorepeat);
  cout << "Host_Rtp_Channel::PlayFileWav: filename= " << filename << ", autorepeat = "<< autorepeat << endl;
  _gstBin->PlayFileWav(filename, autorepeat);
  return true;
}

bool Host_Rtp_Channel::PlayFileWav(char const *filename, bool autorepeat, void *call)
{

  cout << "*************** Inside Host_Rtp_Channel::PlayFileWav *******************" << endl;
  _gstBin->tmpCall = call;
  CLog::Detail(THISMODULE, "Host_Rtp_Channel::PlayFileWav: filename=%s\n",filename);
  _gstBin->PlayFileWav(filename, autorepeat);
  return true;
}


bool Host_Rtp_Channel::AttachTask( void *call)
{

  cout << "*************** Inside Host_Rtp_Channel::AttachTask *******************" << endl;
  _gstBin->tmpCall = call;
  return true;
}

bool Host_Rtp_Channel::StopFileWav(int waitTime) 
{
  CLog::Detail(THISMODULE, "Host_Rtp_Channel::StopFileWav: stop playing file in %d seconds\n",waitTime);
  _gstBin->StopFileWav(waitTime);
  return true;
}


int Host_Rtp_Channel::GetDtmfDigit(char* dtmf_digits)
{
  CLog::Detail(THISMODULE, "Host_Rtp_Channel::getDtmfDigit\n");
  return _gstBin->GetDtmfDigit(dtmf_digits);
}

void Host_Rtp_Channel::SetDtmfDigitTimeout(int max_time)
{
  CLog::Detail(THISMODULE, "Host_Rtp_Channel::setDtmfDigitTimeout() Setting %d seconds timeout\n", max_time);  
  _gstBin->SetDtmfDigitTimeout(max_time);
}

//bool Host_Rtp_Channel::RecordFile(const char *filename, Host_Rtp_Channel *rtpchan2)
bool Host_Rtp_Channel::RecordFile(const char *filename, RTP_Channel *rtpchan2)
{
  CLog::Detail(THISMODULE, "Host_Rtp_Channel::RecordFile: filename=%s\n", filename);
  Host_Rtp_Channel *hostrtpchan2 = (Host_Rtp_Channel*)rtpchan2;
  return _gstBin->RecordFile(filename, hostrtpchan2->_gstBin);
}

void Host_Rtp_Channel::EndRecordFile()
{
  CLog::Detail(THISMODULE, "Host_Rtp_Channel::EndRecordFile\n");
  _gstBin->EndRecordFile();  
}

bool Host_Rtp_Channel::ConfCreateRoom(const char *room_name)
{
  CLog::Detail(THISMODULE, "Host_Rtp_Channel::ConfCreateRoom\n");
  char tmpStr[100];
  snprintf(tmpStr,sizeof(tmpStr), "%s",room_name);
  bool dtmfEvent = false;

  HostGSTConfBin *gstConfBin = NULL;
  gstConfBin = _gstBin->GetConfBinFromMap(room_name);
  CLog::Detail(THISMODULE, "Host_Rtp_Channel::ConfCreateRoom gstConfBin %02x \n", gstConfBin);
  if (!gstConfBin){
    gstConfBin = new HostGSTConfBin(tmpStr, dtmfEvent);
    CLog::Detail(THISMODULE, "Host_Rtp_Channel::ConfCreateRoom gstConfBin %02x \n", gstConfBin);
    if (!gstConfBin->ConfCreateRoom()) {
      CLog::Detail(THISMODULE, "Host_Rtp_Channel::ConfCreateRoom gstConfBin->ConfCreateRoom() %02x \n", gstConfBin->ConfCreateRoom());
      return false;
    }
    _gstBin->AddConfBinInMap(room_name,gstConfBin);
  }
  return true;
}

bool Host_Rtp_Channel::ConfAddParty(const char *room_name)
{
  CLog::Detail(THISMODULE, "Host_Rtp_Channel::ConfAddParty\n");
  //printf("Host_Rtp_Channel::ConfAddParty\n");
  GstElement* confPipeline;
  GstElement* audioMixer;
  GstElement* mixerOut;
  GstClock * global_clock;
  HostGSTConfBin *gstConfBin = NULL;
  gstConfBin = _gstBin->GetConfBinFromMap(room_name);
  if (gstConfBin) {
    //printf("Host_Rtp_Channel::ConfAddParty : Inside Add party\n");
    confPipeline = GST_ELEMENT(gstConfBin->get_conf_pipeline());
    audioMixer = GST_ELEMENT(gstConfBin->get_audio_mixer());
    mixerOut = GST_ELEMENT(gstConfBin->get_mixer_out());
    global_clock = gstConfBin->get_global_clock();
    _gstBin->SetConfParam(room_name, global_clock);
    _gstBin->ConfAddParty(confPipeline, audioMixer, mixerOut, room_name);
    gstConfBin->_confMember+=1;
    //printf("Host_Rtp_Channel::ConfAddParty : Members in Room [%d]\n",gstConfBin->_confMember); 
  } else return false;
  //printf("Host_Rtp_Channel::ConfAddParty : Added party\n");

  return true;
}

bool Host_Rtp_Channel::ConfRecordFile(const char *room_name, const char *filename)
{
  CLog::Detail(THISMODULE, "Host_Rtp_Channel::ConfCreateRoom\n");
  bool confRecActive = false;
  GstElement* confPipeline;
  GstElement* mixerOut;
  HostGSTConfBin *gstConfBin = NULL;
  gstConfBin = _gstBin->GetConfBinFromMap(room_name);
  CLog::Detail(THISMODULE, "Host_Rtp_Channel::ConfCreateRoom gstConfBin %02x \n", gstConfBin);
  if (gstConfBin) {
    //printf("Host_Rtp_Channel::ConfRecordFile : Inside recording\n");
    confPipeline = GST_ELEMENT(gstConfBin->get_conf_pipeline());
    mixerOut = GST_ELEMENT(gstConfBin->get_mixer_out());
    confRecActive = gstConfBin->get_conf_rec_active();
    CLog::Detail(THISMODULE, "Host_Rtp_Channel::ConfCreateRoom confRecActive %02x \n", confRecActive);
    if(!confRecActive) {
      CLog::Detail(THISMODULE, "Host_Rtp_Channel::ConfCreateRoom calling ConfRecordFile \n");
      gstConfBin->_confRecActive = true;
      _gstBin->ConfRecordFile(confPipeline, mixerOut, filename);
    }
  } else { 
    return false;
  }
  //printf("Host_Rtp_Channel::ConfAddParty : Added party\n");
  return true;
}

void HostGSTRtpBin::ConfEndRecordFile(GstElement *confPipeline,  GstElement *mixerOut)
{
  g_print("conference recording ended!!");
  if(m_mixerout_rec_src_pad) {
    GstPad* peer_sink_pad = NULL;
    GstPad* src_pad = NULL;
    GstElement* elem = NULL;

    gst_element_set_state(confPipeline, GST_STATE_NULL);

    peer_sink_pad = gst_pad_get_peer (m_mixerout_rec_src_pad);
    gst_pad_unlink(m_mixerout_rec_src_pad, peer_sink_pad);
    gst_object_unref (m_mixerout_rec_src_pad);
    m_mixerout_rec_src_pad = NULL;

    while(peer_sink_pad){
      elem = GST_ELEMENT(gst_pad_get_parent_element(peer_sink_pad));
      gst_object_unref (peer_sink_pad);
      peer_sink_pad = NULL;
      src_pad = gst_element_get_static_pad(elem, "src");
      if(src_pad) {
        peer_sink_pad = gst_pad_get_peer (src_pad);
        gst_pad_unlink(src_pad, peer_sink_pad);
        gst_object_unref(src_pad);
        src_pad = NULL;
      }
      gst_bin_remove(GST_BIN(confPipeline), elem);
      gst_object_unref (elem);
    }
    gst_element_set_state(confPipeline, GST_STATE_PLAYING);
  }
  if( debug ) {
    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN (confPipeline), GST_DEBUG_GRAPH_SHOW_ALL, GST_ELEMENT_NAME (get_conf_pipeline()));
  }
}

void Host_Rtp_Channel::ConfEndRecordFile(const char *room_name)
{
  CLog::Detail(THISMODULE, "Host_Rtp_Channel::ConfEndRecordFile\n");
  bool confRecActive = false;
  GstElement* confPipeline;
  GstElement* mixerOut;
  HostGSTConfBin *gstConfBin = NULL;
  gstConfBin = _gstBin->GetConfBinFromMap(room_name);
  if (gstConfBin) {
    confPipeline = GST_ELEMENT(gstConfBin->get_conf_pipeline());
    mixerOut = GST_ELEMENT(gstConfBin->get_mixer_out());
    confRecActive = gstConfBin->get_conf_rec_active();
    if(confRecActive) {
      gstConfBin->_confRecActive = false;
      _gstBin->ConfEndRecordFile(confPipeline, mixerOut);
    }
  }
  return;
}

// enable/disable specific direction
void Host_Rtp_Channel::enableTransmission(bool isRtpRxEnable, bool isRtpTxEnable)
{
  _gstBin->enableTransmission(isRtpRxEnable, isRtpTxEnable);
}

HostMediaProcessingDriver* HostMediaProcessingDriver::instance;

HostMediaProcessingDriver * HostMediaProcessingDriver::Create()
{
  if(!instance)
    instance=new HostMediaProcessingDriver();
  return instance;
}

bool HostMediaProcessingDriver::Load()
{
  // todo: actual GST init is unloaded to kernel
  CLog::Detail(THISMODULE, "HostMediaProcessingDriver::Load: Loading host media driver..\n");  

  return TRUE;
}


bool HostMediaProcessingDriver::Unload()
{
  g_main_loop_unref (HostMediaProcessingDriver::loop);
  CLog::Detail(THISMODULE, "HostMediaProcessingDriver::Unload: Unloading host media driver..\n");    

  return TRUE;
}

void HostMediaProcessingDriver::HandleEvents()
{		
  //CLog::Detail(THISMODULE, "HostMediaProcessingDriver::HandleEvents\n");    
  /*HostGSTRtpBin hostRtpBin("test");
    hostRtpBin.Open(40002);
  //linux porting
  hostRtpBin.Activate("192.168.1.1:40002",8);
  //hostRtpBin.Activate("10.32.2.31:40002",8);


  HostGSTRtpBin hostRtpBin2("test2");
  hostRtpBin2.Open(40005);
  //linux porting
  hostRtpBin2.Activate("192.168.1.98:40002",8);
  //hostRtpBin2.Activate("10.32.2.31:40002",8);

  hostRtpBin.add_listener(&hostRtpBin2);    

  while(1){
  Sleep(1);
  //g_main_context_iteration(g_main_loop_get_context(loop), FALSE);
  g_main_context_iteration(HostMediaProcessingDriver::context, FALSE);
  };*/
  //  g_main_context_iteration(g_main_loop_get_context(HostMediaProcessingDriver::loop), FALSE);
  //  g_main_context_iteration(g_main_loop_get_context(HostMediaProcessingDriver::loop), FALSE);
  //  g_main_context_iteration(g_main_loop_get_context(HostMediaProcessingDriver::loop), FALSE);
  //  g_main_context_iteration(g_main_loop_get_context(HostMediaProcessingDriver::loop), FALSE);
  g_main_context_iteration(g_main_loop_get_context(HostMediaProcessingDriver::loop), FALSE);
}


static const char base64_table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
#define B64BUFFLEN 1024
int b64_encode(unsigned char *in, int ilen, unsigned char *out, int olen)
{
  int y = 0, bytes = 0;
  size_t x = 0;
  unsigned int b = 0, l = 0;

  for (x = 0; x < ilen; x++) {
    b = (b << 8) + in[x];
    l += 8;

    while (l >= 6) {
      out[bytes++] = base64_table[(b >> (l -= 6)) % 64];
      if (bytes >= (int)olen - 1) {
        goto end;
      }
      if (++y != 72) {
        continue;
      }
      /* out[bytes++] = '\n'; */
      y = 0;
    }
  }

  if (l > 0) {
    out[bytes++] = base64_table[((b % 16) << (6 - l)) % 64];
  }
  if (l != 0) {
    while (l < 6 && bytes < (int)olen - 1) {
      out[bytes++] = '=', l += 2;
    }
  }

end:

  out[bytes] = '\0';

  return true;
}

int b64_decode(char *in, char *out, int olen)
{

  char l64[256];
  int b = 0, c, l = 0, i;
  char *ip, *op = out;
  size_t ol = 0;

  for (i = 0; i < 256; i++) {
    l64[i] = -1;
  }

  for (i = 0; i < 64; i++) {
    l64[(int) base64_table[i]] = (char) i;
  }

  for (ip = in; ip && *ip; ip++) {
    c = l64[(int) *ip];
    if (c == -1) {
      continue;
    }

    b = (b << 6) + c;
    l += 6;

    while (l >= 8) {
      op[ol++] = (char) ((b >> (l -= 8)) % 256);
      if (ol >= olen - 2) {
        goto end;
      }
    }
  }

end:

  op[ol++] = '\0';
  //printf("B64_DECODE : In %s OUt %s \n",in, out);

  return ol;
}


#endif //GSTREAMER_TEST_MODE
#endif //XGATE_HAS_HMP

