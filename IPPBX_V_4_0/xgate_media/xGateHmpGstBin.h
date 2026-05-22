#ifndef _XGATE_HMP_GST_BIN_H
#define _XGATE_HMP_GST_BIN_H

//gstreamer include
extern "C"
{
#include <stdlib.h>
#include <stdio.h>
#include <gst/gst.h>
#include <fcntl.h>
#include <glib.h>
#include <gio/gio.h>
#include <gst/net/gstnet.h>
}
#include <map>
#include <iostream>

//local include
#include "xGateUtil.h"
#include "xGateMgMsg.h"
#include "xGateHmpGstUtil.h"
#include "xGateHmpGstAudioBin.h"
#include "xGateHmpGstVideoBin.h"

#define GST_DEBUG TRUE

/* Client class - Start
*/
class Client {
  public:
    Client();
    Client(ClientDetail &detail);
    ~Client();

    //public member properties
    ClientDetail m_detail;
    HmpVoiceRecvChannel *m_pHmpVoiceRecvChannel;
    HmpFrameRecvChannel *m_pHmpFrameRecvChannel;
    HmpRecordAudioChannel *m_pHmpRecordAudioChannel;
    HmpRecordMP3Channel *m_pHmpRecordMP3Channel;

    gboolean m_hostJoined;
    string m_playFile;
    gboolean m_isHost;
    volatile int m_mp3_enc;
//    bool m_isRecordingStatus;
    unsigned int m_clientCount;
    int m_rec_started;
    int m_rec_del;
    string m_mgresourceId;
    string m_callId;
    //public member functions
    gboolean create_audio_channel();
    gboolean create_record_mp3_channel();
    gboolean create_video_channel(gint videoPort, GSocket *videoSock);
    gboolean start_voice_channel();
    gboolean start_frame_channel();
    gboolean stop_voice_channel();
    gboolean stop_frame_channel();
    gboolean set_record_attributes(string &recordFile);
    gboolean add_receive_channel();
    gboolean remove_receive_channel();
    gboolean stop_audio_record(Client *client);
    gboolean post_media_event_result(xGateMediaEvent mediaEvent);
    gboolean set_media_event_details(xGateMediaEvent mediaEvent, xGateMgMsg* pMgMsg);
    void  post_message_to_HmpProcessor(bool conversion_status);   
  private:
    //private member functions
    string form_channel_name(string name);
    string form_videochannel_name(string name);
    gboolean create_voice_channel(gint audioPort, GSocket *audioSock);
    gboolean create_frame_channel(gint videoPort, GSocket *videoSock);
    gboolean create_voice_recv_channel(gint audioPort, GSocket *audioSock);
    gboolean create_frame_recv_channel(gint videoPort, GSocket *videoSock);
    gboolean create_record_audio_channel();
    gboolean get_suitable_channel(HmpChannelType &channelType, string &channelName, HmpBinType &binType, string &binName);
    gboolean get_suitable_videochannel(HmpChannelType &channelType, string &channelName, HmpVideoBinType &vbinType, string &binName);
    gboolean get_suitable_bin(HmpChannelType &channelType, HmpBinType &binType, string &binName);
    gboolean get_suitable_videobin(HmpChannelType &channelType, HmpVideoBinType &binType, string &binName);
};
/* Client class - End
*/

/* xGateHmpCall class - Start
*/
typedef vector<Client*> CLIENT_LIST;
class xGateHmpCall {
  public:
    xGateHmpCall(string mgresourceId, xGateCallType callType = EN_XGATE_CALL_TYPE_AUDIO); 
    ~xGateHmpCall();

    //public member properties
    string m_mgresourceId;
    xGateCallType m_callType;
    CLIENT_LIST m_clientList;
    unsigned short m_clientCount;
    gint m_termAudioPort;
    gint m_termVideoPort;
    gboolean m_isRelay;
    xGateMediaEvent m_mediaEvent;   

    //public member functions
    xGateRetVal create_receive_channel(ClientDetail &clientDetail);
    xGateRetVal start_voice_channels();
    xGateRetVal stop_voice_channels();

    //call recording functions
    xGateRetVal start_call_record(string filePath, string callId);
    xGateRetVal stop_call_record(string callId);
    xGateRetVal hold_call_record(string callId);
    xGateRetVal unhold_call_record(string callId);
    xGateRetVal hold_conf_call_record(string callId);
    xGateRetVal unhold_conf_call_record(string callId);
    //CLIENT_LIST related functions
    gboolean insert_client_entry(Client *detail);
    Client * get_client_entry(string clientIp, gint clientPort, gint legId);
    Client * get_client_entry(string clientIp, gint clientPort, string callId);
    Client * get_client_entry(string callId);
    Client * get_client_entry(gint legId);
    gboolean remove_client_entry(Client *client);
    gboolean remove_client_fromlist(Client *client);
    gboolean remove_client_entries();

  private:
    GSocket * allocate_audio_port(gint &audioPort);
    GSocket * allocate_video_port(gint &videoPort);
    xGateRetVal create_audio_channels(Client *client);
    xGateRetVal create_video_channels(Client *client);

#if 1 //temporary function, will update the clientDetail based on media request from signalling layer
    void update_client_detail(ClientDetail &clientDetail);
    GSocket *m_termAudioSocket;
    GSocket *m_termVideoSocket;
#endif

};
/* xGateHmpCall class - End
*/
#endif
