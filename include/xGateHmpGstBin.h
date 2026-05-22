#ifndef _XGATE_HMP_GST_BIN_H
#define _XGATE_HMP_GST_BIN_H

//gstreamer include
#include <stdlib.h>
#include <stdio.h>
#include <glib.h>
extern "C"
{
#include <gst/gst.h>
#include <fcntl.h>
#include <gio/gio.h>
#include <gst/net/gstnet.h>
}
#include <ace/Guard_T.h>
#include <ace/Recursive_Thread_Mutex.h>


#include <map> 
//local include
#include "xGateUtil.h"
#include "xGateMgMsg.h"
#include "xGateHmpGstUtil.h"
#include "xGateHmpGstAudioBin.h"
#include "URSpeechmaticssttclient.h"
#include "URInhousesttclient.h" 

struct st
{
 st():c(0){}
 char *p;
 unsigned long long int c;
};

#define GST_DEBUG TRUE

class Client {
   public:
      Client();
      Client(ClientDetail &detail);
      ~Client();

      int m_audioPort;
      GSocket *m_audioSocket;
      int m_audiottsPort;
      GSocket *m_audiottsSocket;

      int m_tts_fd[2];

      //public member properties
      ClientDetail m_detail;

      HmpVoiceRePacketizeChannel *m_pHmpTTSVoiceRePacketizeChannel;
      HmpVoiceTranscodeChannel *m_pHmpTTSVoiceTranscodingChannel;
      HmpVoiceRelayChannel *m_pHmpTTSVoiceRelayChannel;
      HmpVoiceRePacketizeChannel *m_pHmpSTTVoiceRePacketizeChannel;
      HmpVoiceTranscodeChannel *m_pHmpSTTVoiceTranscodingChannel;
      HmpVoiceRelayChannel *m_pHmpSTTVoiceRelayChannel;
      HmpInhouseStreamingChannel *m_pHmpInhouseStreamingChannel; 
      HmpVoiceTransceiverChannel *m_pHmpVoiceTransceiverChannel;
      HmpWaveEncoderChannel *m_pHmpWaveEncoderChannel;

      ACE_thread_t m_threadID;

      unsigned int m_tts_codec;
      unsigned int m_tts_ptime;
      string m_tts_codecName;
      unsigned int is_ccaas;
      unsigned int ccaas_port_type;
      string m_base64File;


      //public member functions
      gboolean create_stt_tts_audio_channel(gint audioPort, GSocket *audioSock,gint audioTtsPort, GSocket *audioTtsSock,guint ssrc,Client *client);
      gboolean create_wave_encoder_channel(gint audioPort, GSocket *audioSock,Client *client);

      HmpChannel *get_active_tts_channel();
      HmpChannel *get_active_stt_channel();

      gboolean start_voice_channel(STT_TTS_STATE mrcpState,xGateMrcpCommand mrcpCommand);
      gboolean stop_voice_channel(STT_TTS_STATE mrcpState,xGateMrcpCommand mrcpCommand);
      gboolean start_stt_voice_channel();
      gboolean stop_stt_voice_channel();

      //gboolean start_base64_voice_capture_channel();
      gboolean start_base64_voice_capture_channel();
      gboolean stop_base64_voice_capture_channel();

      gboolean post_msg_stop_pipeline();
      gboolean post_media_event_result(xGateMediaEvent mediaEvent);
      gboolean post_media_event_result(xGateMediaEvent mediaEvent,string Ip,unsigned short);
      gboolean set_media_event_details(xGateMediaEvent mediaEvent, xGateMgMsg* pMgMsg);

      gboolean stop_active_channel();  

      //private member variable for SpeechMatics
      URSpeechmaticssttclient *speechMaticsClient;
      gboolean CreateSpeechmaticsClient(); // Added for creating speech matics client

      //private member variable for InHouse
      URInhousesttclient *inHouseClient;
      gboolean CreateInhouseClient(); // Added for creating Inhouse stt client

      struct st m_idset;
      map<GstElement *,struct st>m_IdMap; 

   private:
      //private member functions
      string form_channel_name(string name,gboolean codec=FALSE);
      string form_bin_name(string name,HmpCodecType codec);
      gboolean create_voice_transcode_channel(gint audioPort,GSocket *audioSock,gboolean isGRPC=FALSE);
      gboolean create_voice_relay_channel(gint audioPort,GSocket *audioSock,gboolean isGRPC=FALSE);
      gboolean create_voice_repacketize_channel(gint audioPort,GSocket *audioSock,gboolean isGRPC=FALSE);
      gboolean create_voice_transceiver_channel(gint audioPort, GSocket *audioSock);
      gboolean create_inhousestream_channel(gint audioPort, GSocket *audioSock);

      //private member variable
};
/* Client class - End
*/


/* xGateHmpCall class - Start
*/

typedef vector<Client*> CLIENT_LIST;
class xGateHmpCall {
   public:
      xGateHmpCall(string mgresourceId, xGateCallType callType = EN_XGATE_CALL_TYPE_SIMPLE); 
      ~xGateHmpCall();
      string m_mgresourceId;
      xGateCallType m_callType;
      CLIENT_LIST m_clientList;
      unsigned short m_clientCount;
      guint m_respPort;
      guint m_ttsrespPort;
      GSocket *m_audiottsSocket;

      xGateMediaEvent m_mediaEvent;   
      HmpCallState m_callState;


      //public member functions
      xGateRetVal create_stt_tts_channel(ClientDetail &clientDetail);
      xGateRetVal start_voice_channels();
      xGateRetVal stop_voice_channels();
      gboolean start_voice_capture_channel();

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
      GSocket * allocate_audio_port(gint &audioPort, unsigned short IpFamily = G_SOCKET_FAMILY_IPV4);

      xGateRetVal create_stt_tts_audio_channels(Client *client);
      xGateRetVal create_wave_encoder_audio_channels(Client *client);
      xGateRetVal create_audio_channels(Client *client);
};

/* xGateHmpCall class - End
*/
/*xGateHmpConference class implementation - Start
*/

typedef map<string, Client*> CLIENT_MAP;

class xGateHmpConference {
   public:
      //public member properties
      string m_conferenceId;
      xGateCallType m_confType;
      gboolean m_isHost;
      gboolean m_hostJoined;
      gboolean m_isRecStarted;
      guint m_respPort;

      CLIENT_MAP m_clientMap;
      map<string,unsigned short> m_pbx_map;
      unsigned int m_snd_pbx;
      unsigned int m_first_host;

      xGateHmpConference(string conferenceId, xGateCallType confType); 
      ~xGateHmpConference();
      xGateRetVal create_audio_channels(Client *client);
      xGateRetVal create_conference_channel(ClientDetail &clientDetail);
      xGateRetVal validate_params(ClientDetail &clientDetail);
      GSocket * allocate_audio_port(gint &audioPort, unsigned short IpFamily = G_SOCKET_FAMILY_IPV4);
      xGateRetVal start_stop_mrcp_voice_channel(ClientDetail &clientDetail);
      //CLIENT_MAP related functions
      xGateRetVal insert_participant_entry(string callId, Client *client);
      Client * get_participant_entry(string callId);
      xGateRetVal remove_participant_entry(string callId);
      xGateRetVal remove_participant_entries();

};

#endif
