
//ace include
#include <ace/Task.h>
#include <ace/Null_Mutex.h>
#include <ace/Recursive_Thread_Mutex.h>
#include <ace/Guard_T.h>
#include <pthread.h>
//local include
#include "xGateMGDispatcher.h"
#include "xGateHmpGstManager.h"
#ifdef XGATE_HAS_SELF_AUDIT
#include "xGateAuditMsg.h"
#include "xGateAuditUtil.h"
#endif
//self include
#include "xGateHmpGstBin.h"
#include "xGateHmpGstAudioBin.h"
#include "xGateUtil.h"
#include "Base64.h"
#include<fstream>
#include <iostream>
#include "URSpeechmaticssttclient.h"
#include "URInhousesttclient.h"

#define THISMODULE "HmpBin"

#define FILESTREAM 1
#define BASE64_ENABLE 1
#define Delay 2
#define INHOUSE_ENABLE 1
#define path "/volume/nfsshare/ccaas/voice_wav/"

/* Client class implementation - Start
 */
Client::Client() : m_pHmpTTSVoiceRePacketizeChannel(NULL),m_pHmpTTSVoiceTranscodingChannel(NULL),m_pHmpTTSVoiceRelayChannel(NULL),m_pHmpSTTVoiceRePacketizeChannel(NULL),m_pHmpSTTVoiceTranscodingChannel(NULL),m_pHmpSTTVoiceRelayChannel(NULL),m_pHmpVoiceTransceiverChannel(NULL), m_pHmpWaveEncoderChannel(NULL),m_pHmpInhouseStreamingChannel(NULL)
{
	 m_audioPort = 0;
	 m_audioSocket = NULL;
	 m_audiottsPort = 0;
	 m_audiottsSocket = NULL;
	 speechMaticsClient = NULL;
	 m_tts_fd[0]=0;
	 m_tts_fd[1]=0;
	 m_base64File="";
	 inHouseClient = NULL;
}

Client::Client(ClientDetail &detail) :	m_detail(detail), m_pHmpTTSVoiceRePacketizeChannel(NULL),m_pHmpTTSVoiceTranscodingChannel(NULL),m_pHmpTTSVoiceRelayChannel(NULL),  m_pHmpSTTVoiceRePacketizeChannel(NULL),m_pHmpSTTVoiceTranscodingChannel(NULL),m_pHmpSTTVoiceRelayChannel(NULL), m_pHmpVoiceTransceiverChannel(NULL),m_pHmpWaveEncoderChannel(NULL),m_pHmpInhouseStreamingChannel(NULL)
{
	 m_audioPort = 0;
	 m_audioSocket = NULL;
	 m_audiottsPort = 0;
	 m_audiottsSocket = NULL;
	 speechMaticsClient = NULL;
	 m_tts_fd[0]=0;
	 m_tts_fd[1]=0;
	 m_base64File="";
	 inHouseClient = NULL;
}

gboolean Client::stop_active_channel(){
	 XGLOG_INFO("Client::stop_active_channel start for %s",(char*)ur_log_string(m_detail.m_callId));

	 if(m_pHmpTTSVoiceRePacketizeChannel){
			XGLOG_INFO("BFR calling the m_pHmpTTSVoiceRePacketizeChannel stop for %s",(char*)ur_log_string(m_detail.m_callId));
			m_pHmpTTSVoiceRePacketizeChannel->stop();
			XGLOG_INFO("AFR calling the m_pHmpTTSVoiceRePacketizeChannel stop for %s",(char*)ur_log_string(m_detail.m_callId));
	 }

	 if(m_pHmpTTSVoiceTranscodingChannel){
			XGLOG_INFO("BFR calling the m_pHmpTTSVoiceTranscodingChannel stop for %s",(char*)ur_log_string(m_detail.m_callId));
			m_pHmpTTSVoiceTranscodingChannel->stop();
			XGLOG_INFO("AFR calling the m_pHmpTTSVoiceTranscodingChannel stop for %s",(char*)ur_log_string(m_detail.m_callId));
	 }

	 if(m_pHmpTTSVoiceRelayChannel){
			XGLOG_INFO("BFR calling the m_pHmpTTSVoiceRelayChannel stop for %s",(char*)ur_log_string(m_detail.m_callId));
			m_pHmpTTSVoiceRelayChannel->stop();
			XGLOG_INFO("AFR calling the m_pHmpTTSVoiceRelayChannel stop for %s",(char*)ur_log_string(m_detail.m_callId));
	 }

	 if(m_pHmpSTTVoiceRePacketizeChannel){
			XGLOG_INFO("BFR calling the m_pHmpSTTVoiceRePacketizeChannel stop for %s",(char*)ur_log_string(m_detail.m_callId));
			m_pHmpSTTVoiceRePacketizeChannel->stop();
			XGLOG_INFO("AFR calling the m_pHmpSTTVoiceRePacketizeChannel stop for %s",(char*)ur_log_string(m_detail.m_callId));
	 }

	 if(m_pHmpSTTVoiceTranscodingChannel){
			XGLOG_INFO("BFR calling the m_pHmpSTTVoiceTranscodingChannel stop for %s",(char*)ur_log_string(m_detail.m_callId));
			m_pHmpSTTVoiceTranscodingChannel->stop();
			XGLOG_INFO("AFR calling the m_pHmpSTTVoiceTranscodingChannel stop for %s",(char*)ur_log_string(m_detail.m_callId));
	 }

	 if(m_pHmpSTTVoiceRelayChannel){
			XGLOG_INFO("BFR calling the m_pHmpSTTVoiceRelayChannel stop for %s",(char*)ur_log_string(m_detail.m_callId));
			m_pHmpSTTVoiceRelayChannel->stop();
			XGLOG_INFO("AFR calling the m_pHmpSTTVoiceRelayChannel stop for %s",(char*)ur_log_string(m_detail.m_callId));
	 }

	 if(m_pHmpVoiceTransceiverChannel){
			XGLOG_INFO("BFR calling the m_pHmpVoiceTransceiverChannel stop for %s",(char*)ur_log_string(m_detail.m_callId));
			m_pHmpVoiceTransceiverChannel->stop();
			XGLOG_INFO("AFR calling the m_pHmpVoiceTransceiverChannel stop for %s",(char*)ur_log_string(m_detail.m_callId));
	 }

	 if(m_pHmpInhouseStreamingChannel){
			XGLOG_INFO("BFR calling the m_pHmpInhouseStreamingChannel stop for %s",(char*)ur_log_string(m_detail.m_callId));
			m_pHmpInhouseStreamingChannel->stop();
			XGLOG_INFO("AFR calling the m_pHmpInhouseStreamingChannel stop for %s",(char*)ur_log_string(m_detail.m_callId));
	 }

	 if(m_pHmpWaveEncoderChannel){
			XGLOG_INFO("BFR calling the m_pHmpWaveEncoderChannel stop for %s",(char*)ur_log_string(m_detail.m_callId));
			m_pHmpWaveEncoderChannel->stop();
			XGLOG_INFO("AFR calling the m_pHmpWaveEncoderChannel stop for %s",(char*)ur_log_string(m_detail.m_callId));
	 }

	 if(inHouseClient){
			XGLOG_INFO("BFR calling inHouseClient->stop for callId %s",(char*)ur_log_string(m_detail.m_callId));
			inHouseClient->stop();
			XGLOG_INFO("AFR calling inHouseClient->stop stop for %s",(char*)ur_log_string(m_detail.m_callId));
	 }

	 XGLOG_INFO("The Client::stop_active_channel() end for %s",(char*)ur_log_string(m_detail.m_callId));
	 return TRUE;
}

Client::~Client() 
{
	 XGLOG_INFO("Client:::~Client() destructor starts for %s",m_detail.m_callId.c_str());
	 stop_active_channel(); 

	 if(m_pHmpTTSVoiceRePacketizeChannel){
			XGLOG_INFO("BFR client destructor m_pHmpTTSVoiceRePacketizeChannel for %s",(char*)ur_log_string(m_detail.m_callId));
			delete m_pHmpTTSVoiceRePacketizeChannel;
			m_pHmpTTSVoiceRePacketizeChannel = NULL;
			XGLOG_INFO("AFR client destructor m_pHmpTTSVoiceRePacketizeChannel for %s",(char*)ur_log_string(m_detail.m_callId));
	 }

	 if(m_pHmpTTSVoiceTranscodingChannel){
			XGLOG_INFO("BFR client destructor m_pHmpTTSVoiceTranscodingChannel for %s",(char*)ur_log_string(m_detail.m_callId));
			delete m_pHmpTTSVoiceTranscodingChannel;
			m_pHmpTTSVoiceTranscodingChannel = NULL;
			XGLOG_INFO("AFR client destructor m_pHmpTTSVoiceTranscodingChannel for %s",(char*)ur_log_string(m_detail.m_callId));
	 }

	 if(m_pHmpTTSVoiceRelayChannel){
			XGLOG_INFO("BFR client destructor m_pHmpTTSVoiceRelayChannel for %s",(char*)ur_log_string(m_detail.m_callId));
			delete m_pHmpTTSVoiceRelayChannel;
			m_pHmpTTSVoiceRelayChannel = NULL;
			XGLOG_INFO("AFR client destructor m_pHmpTTSVoiceRelayChannel for %s",(char*)ur_log_string(m_detail.m_callId));
	 }

	 if(m_pHmpSTTVoiceRePacketizeChannel){
			XGLOG_INFO("BFR client destructor m_pHmpSTTVoiceRePacketizeChannel for %s",(char*)ur_log_string(m_detail.m_callId));
			delete m_pHmpSTTVoiceRePacketizeChannel;
			m_pHmpSTTVoiceRePacketizeChannel = NULL;
			XGLOG_INFO("AFR client destructor m_pHmpSTTVoiceRePacketizeChannel for %s",(char*)ur_log_string(m_detail.m_callId));
	 }

	 if(m_pHmpSTTVoiceTranscodingChannel){
			XGLOG_INFO("BFR client destructor m_pHmpSTTVoiceTranscodingChannel for %s",(char*)ur_log_string(m_detail.m_callId));
			delete m_pHmpSTTVoiceTranscodingChannel;
			m_pHmpSTTVoiceTranscodingChannel = NULL;
			XGLOG_INFO("AFR client destructor m_pHmpSTTVoiceTranscodingChannel for %s",(char*)ur_log_string(m_detail.m_callId));
	 }

	 if(m_pHmpSTTVoiceRelayChannel){
			XGLOG_INFO("BFR client destructor m_pHmpSTTVoiceRelayChannel for %s",(char*)ur_log_string(m_detail.m_callId));
			delete m_pHmpSTTVoiceRelayChannel;
			m_pHmpSTTVoiceRelayChannel = NULL;
			XGLOG_INFO("AFR client destructor m_pHmpSTTVoiceRelayChannel for %s",(char*)ur_log_string(m_detail.m_callId));
	 }

	 if(m_pHmpInhouseStreamingChannel){
			XGLOG_INFO("BFR client destructor m_pHmpInhouseStreamingChannel for %s",(char*)ur_log_string(m_detail.m_callId));
			delete m_pHmpInhouseStreamingChannel;
			m_pHmpInhouseStreamingChannel = NULL;
			XGLOG_INFO("AFR client destructor m_pHmpInhouseStreamingChannel for %s",(char*)ur_log_string(m_detail.m_callId));
			XGLOG_INFO("~client tts_fd is ::%s---> f[0]:%d,f[1]:%d",m_detail.m_callId.c_str(),m_tts_fd[0],m_tts_fd[1]);
			if(m_tts_fd[1] >=3)
				 close(m_tts_fd[1]); 
			if(m_tts_fd[0] >=3)
				 close(m_tts_fd[0]);
	 } 

	 if(speechMaticsClient){
			XGLOG_INFO("BFR client destructor speechMaticsClient for %s",(char*)ur_log_string(m_detail.m_callId));
			delete speechMaticsClient; 
			speechMaticsClient = NULL;
			XGLOG_INFO("AFR client destructor speechMaticsClient for %s",(char*)ur_log_string(m_detail.m_callId));
	 }

	 if(m_pHmpVoiceTransceiverChannel){
			XGLOG_INFO("BFR client destructor m_pHmpVoiceTransceiverChannel for %s",(char*)ur_log_string(m_detail.m_callId));
			delete m_pHmpVoiceTransceiverChannel;
			m_pHmpVoiceTransceiverChannel = NULL;
			XGLOG_INFO("AFR client destructor m_pHmpVoiceTransceiverChannel for %s",(char*)ur_log_string(m_detail.m_callId));
	 }

	 if(inHouseClient){
			XGLOG_INFO("BFR client destructor inHouseClient for %s",(char*)ur_log_string(m_detail.m_callId));
			//TODO: need to identify why calling the destructor causes free
			//called with pointer not allocated from malloc. error observed
			// when malloc is called
			//delete inHouseClient;
			inHouseClient = NULL;
			XGLOG_INFO("AFR client destructor inHouseClient for %s",(char*)ur_log_string(m_detail.m_callId));
	 }

	 if(m_pHmpWaveEncoderChannel){
			XGLOG_INFO("BFR client destructor m_pHmpWaveEncoderChannel for %s",(char*)ur_log_string(m_detail.m_callId));
			delete m_pHmpWaveEncoderChannel;
			m_pHmpWaveEncoderChannel = NULL;
			XGLOG_INFO("AFR client destructor m_pHmpWaveEncoderChannel for %s",(char*)ur_log_string(m_detail.m_callId));
	 }
  
   if(!(m_IdMap.empty()))
   m_IdMap.clear();

	 XGLOG_INFO("Client:::~Client() destructor end for %s",m_detail.m_callId.c_str());
}

string Client::form_channel_name(string name,gboolean codec=FALSE)
{
	 gchar channelName[CHANNEL_NAME_SIZE] = {0};
	 if(codec){
			snprintf(channelName, CHANNEL_NAME_SIZE, "%s_%s_%d", name.c_str(), m_detail.m_callId.c_str(), m_detail.m_mrcp_codec);
	 }else{
			snprintf(channelName, CHANNEL_NAME_SIZE, "%s_%s_%d", name.c_str(), m_detail.m_callId.c_str(),m_detail.audioDetail.m_codec);
	 }
	 return channelName; 
}

string Client::form_bin_name(string name,HmpCodecType codec){
  gchar binName[CHANNEL_NAME_SIZE] = {0};
  snprintf(binName, CHANNEL_NAME_SIZE, "%s_%s_%d", name.c_str(), m_detail.m_callId.c_str(), codec);
  return binName;
}

HmpChannel * Client::get_active_tts_channel(){
	 HmpChannel *pHmpChannel=NULL;

	 if(m_pHmpTTSVoiceRePacketizeChannel){
			pHmpChannel = m_pHmpTTSVoiceRePacketizeChannel;
	 }
	 else if(m_pHmpTTSVoiceRelayChannel){
			pHmpChannel = m_pHmpTTSVoiceRelayChannel;
	 }
	 else if(m_pHmpTTSVoiceTranscodingChannel){
			pHmpChannel = m_pHmpTTSVoiceTranscodingChannel;
	 }
	 return pHmpChannel;
}

HmpChannel * Client::get_active_stt_channel(){
	 HmpChannel *pHmpChannel=NULL;

	 if(m_pHmpSTTVoiceRePacketizeChannel){
			m_pHmpSTTVoiceRePacketizeChannel->stop();
			pHmpChannel = m_pHmpSTTVoiceRePacketizeChannel;
	 }
	 else if(m_pHmpSTTVoiceRelayChannel){
			pHmpChannel = m_pHmpSTTVoiceRelayChannel;
	 }
	 else if(m_pHmpSTTVoiceTranscodingChannel){
			pHmpChannel = m_pHmpSTTVoiceTranscodingChannel;
	 }
	 return pHmpChannel;
}

gboolean Client::create_stt_tts_audio_channel(gint audioPort, GSocket *audioSock,gint audioTtsPort, GSocket *audioTtsSock,guint ssrc, Client *client)
{
	 XGLOG_INFO( "Client::create_stt_tts_audio_channel for call_id: %s and client: %s:%d start", \
				 (char*)ur_log_string(m_detail.m_callId), (char*)ur_log_string(m_detail.audioDetail.m_clientIp), m_detail.audioDetail.m_clientPort);

	 m_tts_codec = EN_CODEC_PCMA;
	 m_tts_ptime = 20;

	 if(client->ccaas_port_type == EN_XGATE_TTS || client->ccaas_port_type == EN_XGATE_BOTH)
	 {
			XGLOG_INFO( "Client::create_stt_tts_audio_channel creating 'TTS' channel for call_id: %s and tts_type: %d ", \
						(char*)ur_log_string(m_detail.m_callId), m_detail.m_ttsType);

			unsigned int ttsType = m_detail.m_ttsType;

			switch(ttsType)
			{
				 case EN_XGATE_TTS_INHOUSE:
						{
							 cout<<"The Inhouse TTS transcoding channel triggered!!!"<<endl;
							 XGLOG_INFO( "Client::create_stt_tts_audio_channel, Inhouse TTS transcoding channel triggered for %s",(char*)ur_log_string(m_detail.m_callId));
							 if(!create_voice_transcode_channel(audioTtsPort,audioTtsSock,TRUE)){
									XGLOG_ERROR( "create_stt_tts_audio_channel, creating voice trancode channel for call_id:%s failed!!",(char*)ur_log_string(m_detail.m_callId));
									return FALSE;
							 }
							 m_pHmpTTSVoiceTranscodingChannel->add_client_addr(m_detail.m_mg_Ip,m_detail.m_mg_port);
							 break;
						}
				 case EN_XGATE_TTS_READ_SPEAKER:
						{
							 cout<<"The Read speaker TTS transcoding channel triggered!!!"<<endl;
							 if(m_detail.audioDetail.m_codec != m_tts_codec){
									XGLOG_INFO( "Client::create_stt_tts_audio_channel, Read speaker TTS transcoding channel triggered for %s",(char*)ur_log_string(m_detail.m_callId));
									if(!create_voice_transcode_channel(audioTtsPort,audioTtsSock,TRUE)){
										 XGLOG_ERROR( "create_stt_tts_audio_channel, creating voice trancode channel for call_id:%s failed!!",(char*)ur_log_string(m_detail.m_callId));
										 return FALSE;
									}
									m_pHmpTTSVoiceTranscodingChannel->add_client_addr(m_detail.m_mg_Ip,m_detail.m_mg_port);
							 }
							 else if(m_detail.audioDetail.m_ptime != m_tts_ptime){
									cout<<"The Read speaker TTS repacketization channel triggered!!!"<<endl;
									XGLOG_INFO( "Client::create_stt_tts_audio_channel, Read speaker TTS repacketization channel triggered for %s",(char*)ur_log_string(m_detail.m_callId));
									if(!create_voice_repacketize_channel(audioTtsPort,audioTtsSock,TRUE)){
										 XGLOG_ERROR( "create_stt_tts_audio_channel, creating voice repacketize channel for call_id:%s failed !!",\
													 (char*)ur_log_string(m_detail.m_callId));
										 return FALSE;
									}
									m_pHmpTTSVoiceRePacketizeChannel->add_client_addr(m_detail.m_mg_Ip,m_detail.m_mg_port);
							 }
							 else{
									cout<<"The Read TTS speaker relay channel triggered!!!"<<endl;
									XGLOG_INFO( "Client::create_stt_tts_audio_channel, Read speaker TTS relay channel triggered for %s",(char*)ur_log_string(m_detail.m_callId));
									if(!create_voice_relay_channel(audioTtsPort,audioTtsSock,TRUE)){
										 XGLOG_ERROR( "create_stt_tts_audio_channel, creating voice relay channel for call_id:%s failed!!", \
													 (char*)ur_log_string(m_detail.m_callId));
										 return FALSE;

									}
									m_pHmpTTSVoiceRelayChannel->add_client_addr(m_detail.m_mg_Ip,m_detail.m_mg_port);
							 }
							 break;
						}
				 case EN_XGATE_TTS_TYPE_UNKNOWN:
				 default:
						{
							 XGLOG_ERROR( "create_stt_tts_audio_channel, creating tts channel for call_id:%s failed !!",\
										 (char*)ur_log_string(m_detail.m_callId));
							 break;
						}
			}
	 }

	 if(client->ccaas_port_type == EN_XGATE_STT || client->ccaas_port_type == EN_XGATE_BOTH)
	 {
			XGLOG_INFO( "Client::create_stt_tts_audio_channel creating 'STT' channel for call_id: %s and client: %s:%d start", \
						(char*)ur_log_string(m_detail.m_callId), (char*)ur_log_string(m_detail.audioDetail.m_clientIp), m_detail.audioDetail.m_clientPort);

			if(!create_voice_transceiver_channel(audioPort,audioSock)){
				 XGLOG_ERROR( "create_voice_transceiver_channel, creating voice transceiver channel for call_id:%s failed!!",(char*)ur_log_string(m_detail.m_callId));
				 return FALSE;
			}
	 }
   
   if(client->ccaas_port_type == EN_XGATE_UNKNOWN)
   {
		 XGLOG_ERROR( "create_stt_tts_audio_channel, ccaas_port_type EN_XGATE_UNKNOWN for call_id:%s failed !!",\
										 (char*)ur_log_string(m_detail.m_callId));
     return FALSE;
   }

	 return TRUE;
}

gboolean Client::create_voice_transceiver_channel(gint audioPort, GSocket *audioSock){

	 string channelName(""),binName("");
	 HmpChannelType channelType = EN_PIPELINE_VOICE_CAPTURE;
	 HmpBinType binType = EN_BIN_UNKNOWN;
	 HmpCodecType codecType = m_detail.audioDetail.m_codec;
	 xGateASRType asrType =  m_detail.m_asrType; 

	 channelName = form_channel_name("VoiceTransceiverChannel");
	 binName = form_bin_name("VoiceTransceiverBin",codecType);

	 if(codecType ==EN_CODEC_PCMU) {
			binType = EN_BIN_G711U_RECV;
	 }else if(codecType == EN_CODEC_PCMA) {
			binType = EN_BIN_G711A_RECV;
	 } else if(codecType == EN_CODEC_G722) {
			binType = EN_BIN_G722_RECV;
	 } else if(codecType == EN_CODEC_G729) {
			binType = EN_BIN_G729_RECV;
	 } else if(codecType >= 96 && codecType <=127 ) {
			binType = EN_BIN_OPUS_RECV;
	 }

	 if(!m_pHmpVoiceTransceiverChannel){
			m_pHmpVoiceTransceiverChannel = new HmpVoiceTransceiverChannel(channelName,channelType);
			if(!m_pHmpVoiceTransceiverChannel->construct_bin(binName,binType,asrType,audioPort,audioSock,codecType)){
				 XGLOG_ERROR("create_voice_transcode_channel for client:%s:%d failed!!", \
							 (char*)ur_log_string(m_detail.audioDetail.m_dialIp), m_detail.audioDetail.m_dialPort);
				 return FALSE;
			}
			m_pHmpVoiceTransceiverChannel->register_handle_event_callback(this);
			m_pHmpVoiceTransceiverChannel->register_rtpbin_callback(this);
			if((asrType ==  EN_XGATE_ASR_SPEECH_MATICS) || (asrType == EN_XGATE_ASR_INHOUSE))
				 m_pHmpVoiceTransceiverChannel->register_appSink_callBack(this); 
	 }
	 else{
			XGLOG_ERROR("create_voice_transceiver_channel for client:%s:%d already been created!!", \
						(char*)ur_log_string(m_detail.audioDetail.m_dialIp), m_detail.audioDetail.m_dialPort);
			return FALSE;
	 }
	 return TRUE;
}

gboolean Client::create_wave_encoder_channel(gint audioPort, GSocket *audioSock,Client* client){

	 string channelName(""),binName("");
	 HmpChannelType channelType = EN_PIPELINE_WAVE_ENCODER;
	 HmpBinType binType = EN_BIN_UNKNOWN;
	 HmpCodecType codecType = m_detail.audioDetail.m_codec;

	 channelName = form_channel_name("WaveEncoderChannel");
	 binName = form_bin_name("WaveEncoderBin",codecType);

	 if(codecType ==EN_CODEC_PCMU) {
			binType = EN_BIN_G711U_RECV;
	 }else if(codecType == EN_CODEC_PCMA) {
			binType = EN_BIN_G711A_RECV;
	 } else if(codecType == EN_CODEC_G722) {
			binType = EN_BIN_G722_RECV;
	 } else if(codecType == EN_CODEC_G729) {
			binType = EN_BIN_G729_RECV;
	 } else if(codecType >= 96 && codecType <=127 ) {
			binType = EN_BIN_OPUS_RECV;
	 }
	 if(!m_pHmpWaveEncoderChannel){
			m_pHmpWaveEncoderChannel = new HmpWaveEncoderChannel(channelName,channelType);
			gst_element_set_state(m_pHmpWaveEncoderChannel->m_pipeline, GST_STATE_NULL);
			if(!m_pHmpWaveEncoderChannel->construct_bin(binName,binType,audioPort,audioSock,codecType)){
				 XGLOG_ERROR("create_wave_encoder_channel for client:%s:%d failed!!", \
							 (char*)ur_log_string(m_detail.audioDetail.m_dialIp), m_detail.audioDetail.m_dialPort);
				 return FALSE;
			}
			m_pHmpWaveEncoderChannel->register_handle_event_callback(this);


			m_base64File=path+m_detail.m_callId+".wav";
			g_object_set(G_OBJECT(m_pHmpWaveEncoderChannel->m_pHmpWaveEncoderBin->m_fileSink), "location", m_base64File.c_str(), NULL);

	 }
	 else{
			XGLOG_ERROR("create_wave_encoder_channel for client:%s:%d already been created!!", \
						(char*)ur_log_string(m_detail.audioDetail.m_dialIp), m_detail.audioDetail.m_dialPort);
			return FALSE;
	 }
	 m_pHmpWaveEncoderChannel->m_pHmpWaveEncoderBin->draw_dot_file();
	 return TRUE;
}

gboolean Client::create_inhousestream_channel(gint audioPort, GSocket *audioSock){

	 string channelName(""),binName("");
	 HmpChannelType channelType = EN_PIPELINE_INHOUSE_STREAM;
	 HmpBinType binType = EN_BIN_UNKNOWN;
	 HmpCodecType codecType = m_detail.audioDetail.m_codec;
	 string codecName = m_detail.audioDetail.m_codecName;

	 channelName = form_channel_name("InhouseStreamingChannel");
	 binName = form_bin_name("InhouseStreamingBin",codecType);

	 if(codecType ==EN_CODEC_PCMU) {
			binType = EN_BIN_G711U_RECV;
	 }else if(codecType == EN_CODEC_PCMA) {
			binType = EN_BIN_G711A_RECV;
	 } else if(codecType == EN_CODEC_G722) {
			binType = EN_BIN_G722_RECV;
	 } else if(codecType == EN_CODEC_G729) {
			binType = EN_BIN_G729_RECV;
	 } else if(codecType >= 96 && codecType <=127) {
			binType = EN_BIN_OPUS_RECV;
	 }

	 if(!m_pHmpInhouseStreamingChannel){
			m_pHmpInhouseStreamingChannel = new HmpInhouseStreamingChannel(channelName,channelType);
			gst_element_set_state(m_pHmpInhouseStreamingChannel->m_pipeline, GST_STATE_NULL);
			if(!m_pHmpInhouseStreamingChannel->construct_bin(binName,binType,codecType, codecName)){
				 XGLOG_ERROR("create_inhousestream_channel for client:%s:%d failed!!", \
							 (char*)ur_log_string(m_detail.audioDetail.m_dialIp), m_detail.audioDetail.m_dialPort);
				 return FALSE;
			}
			m_pHmpInhouseStreamingChannel->register_handle_event_callback(this);
			m_pHmpInhouseStreamingChannel->set_ptime(m_detail.audioDetail.m_ptime, m_detail.audioDetail.m_maxPtime);
	 }
	 else{
			XGLOG_ERROR("create_inhousestream_channel for client:%s:%d already been created!!", \
						(char*)ur_log_string(m_detail.audioDetail.m_dialIp), m_detail.audioDetail.m_dialPort);
			return FALSE;
	 }
	 return TRUE;
}

gboolean Client::start_base64_voice_capture_channel()
{
	 cout<<"starting the base 64 capture channel!!!!!!!!!!!!"<<endl;
	 gboolean ret;
	 if(m_pHmpWaveEncoderChannel)
			ret= m_pHmpWaveEncoderChannel->start();
	 return ret;
}

gboolean Client::stop_base64_voice_capture_channel()
{
	 gboolean ret;

	 cout<<"Stopping the base 64 capture channel!!!!!!"<<endl;

	 gst_element_send_event((GstElement*)m_pHmpWaveEncoderChannel->m_pHmpWaveEncoderBin->m_bin,gst_event_new_eos ());

	 xGateDbConfigMgrServiceMsg *pServicemsg = new xGateDbConfigMgrServiceMsg();
	 pServicemsg->set_mrcp_command(EN_POST_BASE64_MSG);
	 pServicemsg->m_uid =m_detail.m_mgresourceId;
	 pServicemsg->m_callId=m_detail.m_callId;
	 pServicemsg->m_filepath=m_base64File;
	 xGateUtil::getHmpProcessor()->PostMessageToMrcpClient(pServicemsg);

	 return ret;
}

gboolean Client::post_msg_stop_pipeline(){
	 gboolean ret;
	 if(m_pHmpWaveEncoderChannel)
			ret= m_pHmpWaveEncoderChannel->stop();

	 return TRUE;
}

gboolean Client::create_voice_transcode_channel(gint audioPort, GSocket *audioSock,gboolean isTTS=FALSE){

	 string channelName(""), recvbinName(""),sendbinName(""),binName("");
	 HmpChannelType channelType = EN_PIPELINE_VOICE_TRANSCODE;
	 HmpBinType recvbinType = EN_BIN_UNKNOWN;
	 HmpBinType sendbinType = EN_BIN_UNKNOWN;
	 m_tts_codec = EN_CODEC_PCMA;

	 HmpCodecType recvcodecType = EN_CODEC_UNKNOWN;
	 HmpCodecType sendcodecType = EN_CODEC_UNKNOWN;
	 unsigned int mediaModeIn = m_detail.m_mediaModeIn;
	 xGateTTSType TTStype = m_detail.m_ttsType;
	 std::cout << " TTS type ==> " << m_detail.m_ttsType << std::endl;

	 if(!isTTS){
			recvcodecType = m_detail.audioDetail.m_codec;
			sendcodecType = m_detail.m_mrcp_codec;
	 }
	 else{
			recvcodecType = m_tts_codec;
			sendcodecType = m_detail.audioDetail.m_codec;
	 }


	 channelName = form_channel_name("VoiceTranscodeChannel");
	 recvbinName = form_bin_name("VoiceDecoderBin",recvcodecType);
	 sendbinName = form_bin_name("VoiceEncoderBin",sendcodecType);


	 if(recvcodecType == EN_CODEC_PCMU) {
			recvbinType = EN_BIN_G711U_RECV;
	 } else if(recvcodecType == EN_CODEC_PCMA) {
			recvbinType = EN_BIN_G711A_RECV;
	 } else if(recvcodecType == EN_CODEC_G722) {
			recvbinType = EN_BIN_G722_RECV;
	 } else if(recvcodecType == EN_CODEC_G729) {
			recvbinType = EN_BIN_G729_RECV;
	 } else if(recvcodecType >= 96 && recvcodecType <= 127) {
			recvbinType = EN_BIN_OPUS_RECV;
	 }

	 if(sendcodecType == EN_CODEC_PCMU) {
			sendbinType = EN_BIN_G711U_SEND;
	 } else if(sendcodecType == EN_CODEC_PCMA) {
			sendbinType = EN_BIN_G711A_SEND;
	 } else if(sendcodecType == EN_CODEC_G722) {
			sendbinType = EN_BIN_G722_SEND;
	 } else if(sendcodecType == EN_CODEC_G729) {
			sendbinType = EN_BIN_G729_SEND;
	 } else if(sendcodecType >= 96 && sendcodecType <= 127){
			sendbinType = EN_BIN_OPUS_SEND;
	 }

	 if(!isTTS){
			if(!m_pHmpSTTVoiceTranscodingChannel){
				 m_pHmpSTTVoiceTranscodingChannel = new HmpVoiceTranscodeChannel(channelName,channelType);
				 if(!m_pHmpSTTVoiceTranscodingChannel->construct_bin(recvbinName,recvbinType,TTStype,sendbinName,sendbinType,sendcodecType,audioPort,audioSock)){
						XGLOG_ERROR("create_voice_transcode_channel for client:%s:%d failed!!", \
									(char*)ur_log_string(m_detail.audioDetail.m_dialIp), m_detail.audioDetail.m_dialPort);
						return FALSE;
				 }
				 m_pHmpSTTVoiceTranscodingChannel->register_handle_event_callback(this);

			}
			else{
				 XGLOG_ERROR("create_voice_transcode_channel for client:%s:%d already been created!!", \
							 (char*)ur_log_string(m_detail.audioDetail.m_dialIp), m_detail.audioDetail.m_dialPort);
				 return FALSE;
			}
	 }else{
			if(!m_pHmpTTSVoiceTranscodingChannel){
				 m_pHmpTTSVoiceTranscodingChannel = new HmpVoiceTranscodeChannel(channelName,channelType);
				 if(!m_pHmpTTSVoiceTranscodingChannel->construct_bin(recvbinName,recvbinType,TTStype,sendbinName,sendbinType,sendcodecType,audioPort,audioSock)){
						XGLOG_ERROR("create_voice_transcode_channel for client:%s:%d failed!!", \
									(char*)ur_log_string(m_detail.audioDetail.m_dialIp), m_detail.audioDetail.m_dialPort);
						return FALSE;
				 }
				 m_pHmpTTSVoiceTranscodingChannel->register_handle_event_callback(this);
				 m_pHmpTTSVoiceTranscodingChannel-> set_ptime(m_detail.audioDetail.m_ptime, m_detail.audioDetail.m_maxPtime);
         m_pHmpTTSVoiceTranscodingChannel->m_pHmpVoiceRecvDecoderBin->register_identity_callback(this);
         m_pHmpTTSVoiceTranscodingChannel->m_pHmpVoiceSendEncoderBin->register_identity_callback(this);
         m_pHmpTTSVoiceTranscodingChannel->m_pHmpVoiceRecvDecoderBin->set_handoff();
         m_pHmpTTSVoiceTranscodingChannel->m_pHmpVoiceSendEncoderBin->set_handoff();


				 //need to add MRCP server IP:Port to udpsink
			}
			else{
				 XGLOG_ERROR("create_voice_transcode_channel for client:%s:%d already been created!!", \
							 (char*)ur_log_string(m_detail.audioDetail.m_dialIp), m_detail.audioDetail.m_dialPort);
				 return FALSE;
			}
	 }

	 return TRUE;
}

gboolean Client::create_voice_repacketize_channel(gint audioPort, GSocket *audioSock,gboolean isTTS){

	 string channelName(""),binName("");
	 HmpChannelType channelType = EN_PIPELINE_VOICE_REPACKETIZE;
	 HmpBinType binType = EN_BIN_UNKNOWN;
	 HmpCodecType codecType = m_detail.audioDetail.m_codec;
	 string codecName = m_detail.audioDetail.m_codecName;

	 channelName = form_channel_name("VoiceRepacketizationChannel");
	 binName = form_bin_name("VoiceRepacketizationBin",codecType);

	 if(codecType == EN_CODEC_PCMU) {
			binType = EN_BIN_G711U_RECV;
	 }else if(codecType == EN_CODEC_PCMA) {
			binType = EN_BIN_G711A_RECV;
	 } else if(codecType == EN_CODEC_G722) {
			binType = EN_BIN_G722_RECV;
	 } else if(codecType == EN_CODEC_G729) {
			binType = EN_BIN_G729_RECV;
	 } else if(codecType == EN_CODEC_OPUS) {
			binType = EN_BIN_OPUS_RECV;
	 }

	 if(!isTTS){
			if(!m_pHmpSTTVoiceRePacketizeChannel){
				 m_pHmpSTTVoiceRePacketizeChannel = new HmpVoiceRePacketizeChannel(channelName,channelType);
				 if(!m_pHmpSTTVoiceRePacketizeChannel->construct_bin(binName,binType,codecType,audioPort,audioSock)){
						XGLOG_ERROR("create_voice_transcode_channel for client:%s:%d failed!!", \
									(char*)ur_log_string(m_detail.audioDetail.m_dialIp), m_detail.audioDetail.m_dialPort);
						return FALSE;
				 }
				 m_pHmpSTTVoiceRePacketizeChannel->register_handle_event_callback(this);
				 //need to add MRCP server IP:Port to udpsink

			}
			else{
				 XGLOG_ERROR("create_voice_transcode_channel for client:%s:%d already been created!!", \
							 (char*)ur_log_string(m_detail.audioDetail.m_dialIp), m_detail.audioDetail.m_dialPort);
				 return FALSE;
			}
	 }else{
			if(!m_pHmpTTSVoiceRePacketizeChannel){
				 m_pHmpTTSVoiceRePacketizeChannel = new HmpVoiceRePacketizeChannel(channelName,channelType);
				 if(!m_pHmpTTSVoiceRePacketizeChannel->construct_bin(binName,binType,codecType,audioPort,audioSock)){
						XGLOG_ERROR("create_voice_transcode_channel for client:%s:%d failed!!", \
									(char*)ur_log_string(m_detail.audioDetail.m_dialIp), m_detail.audioDetail.m_dialPort);
						return FALSE;
				 }
				 m_pHmpTTSVoiceRePacketizeChannel->register_handle_event_callback(this);
				 //need to add MRCP server IP:Port to udpsink
			}
			else{
				 XGLOG_ERROR("create_voice_transcode_channel for client:%s:%d already been created!!", \
							 (char*)ur_log_string(m_detail.audioDetail.m_dialIp), m_detail.audioDetail.m_dialPort);
				 return FALSE;
			}
	 }

	 return TRUE;
}


gboolean Client::create_voice_relay_channel(gint audioPort, GSocket *audioSock,gboolean isTTS){

	 string channelName(""),binName("");
	 HmpChannelType channelType = EN_PIPELINE_VOICE_RELAY;
	 HmpBinType binType = EN_BIN_UNKNOWN;
	 HmpCodecType codecType = m_detail.audioDetail.m_codec;

	 channelName = form_channel_name("VoiceRelayChannel");
	 binName = form_bin_name("VoiceRelayBin",codecType);

	 if(codecType == EN_CODEC_PCMU) {
			binType = EN_BIN_G711U_RECV;
	 }else if(codecType == EN_CODEC_PCMA) {
			binType = EN_BIN_G711A_RECV;
	 } else if(codecType == EN_CODEC_G722) {
			binType = EN_BIN_G722_RECV;
	 } else if(codecType == EN_CODEC_G729) {
			binType = EN_BIN_G729_RECV;
	 } else if(codecType == EN_CODEC_OPUS) {
			binType = EN_BIN_OPUS_RECV;
	 }


	 if(!isTTS){
			if(!m_pHmpSTTVoiceRelayChannel){
				 m_pHmpSTTVoiceRelayChannel = new HmpVoiceRelayChannel(channelName,channelType);
				 if(!m_pHmpSTTVoiceRelayChannel->construct_bin(binName,binType,audioPort,audioSock)){
						XGLOG_ERROR("create_voice_relay_channel for client:%s:%d failed!!", \
									(char*)ur_log_string(m_detail.audioDetail.m_dialIp), m_detail.audioDetail.m_dialPort);
						return FALSE;
				 }
				 m_pHmpSTTVoiceRelayChannel->m_pHmpVoiceRelayBin->m_isSTTpipeline = TRUE;
				 m_pHmpSTTVoiceRelayChannel->register_handle_event_callback(this);
				 //need to add MRCP server IP:Port to udpsink

			}
			else{
				 XGLOG_ERROR("create_voice_relay_channel for client:%s:%d already been created!!", \
							 (char*)ur_log_string(m_detail.audioDetail.m_dialIp), m_detail.audioDetail.m_dialPort);
				 return FALSE;
			}
	 }else{
			if(!m_pHmpTTSVoiceRelayChannel){
				 m_pHmpTTSVoiceRelayChannel = new HmpVoiceRelayChannel(channelName,channelType);
				 if(!m_pHmpTTSVoiceRelayChannel->construct_bin(binName,binType,audioPort,audioSock)){
						XGLOG_ERROR("create_voice_relay_channel for client:%s:%d failed!!", \
									(char*)ur_log_string(m_detail.audioDetail.m_dialIp), m_detail.audioDetail.m_dialPort);
						return FALSE;
				 }
				 m_pHmpTTSVoiceRelayChannel->register_handle_event_callback(this);
				 //need to add MRCP server IP:Port to udpsink
			}
			else{
				 XGLOG_ERROR("create_voice_relay_channel for client:%s:%d already been created!!", \
							 (char*)ur_log_string(m_detail.audioDetail.m_dialIp), m_detail.audioDetail.m_dialPort);
				 return FALSE;
			}
	 }

	 return TRUE;
}

gboolean Client::CreateSpeechmaticsClient()
{
	 speechMaticsClient = new URSpeechmaticssttclient;
	 if(!speechMaticsClient)
	 {
			return EN_XGATE_STATUS_ERROR;
	 }
	 return EN_XGATE_STATUS_SUCCESS;
}

gboolean Client::CreateInhouseClient()
{
	 if(inHouseClient)
	 {
			//TODO: need to identify why calling the destructor causes free
			// called with pointer not allocated from malloc. error observed
			// when malloc is called
			// delete inHouseClient;
			inHouseClient = NULL;
	 }

	 inHouseClient = new URInhousesttclient(m_detail.m_mgresourceId,m_detail.m_callId,m_detail.m_ccaas_port_type);
	 if(!inHouseClient)
	 {
			return EN_XGATE_STATUS_ERROR;
	 }
	 return EN_XGATE_STATUS_SUCCESS;
}

gboolean Client::start_voice_channel(STT_TTS_STATE mrcpState,xGateMrcpCommand mrcpCommand)
{
	 XGLOG_INFO( "Client::start_voice_channel for call_id: %s and client: %s:%d start", \
				 (char*)ur_log_string(m_detail.m_callId), (char*)ur_log_string(m_detail.audioDetail.m_clientIp), m_detail.audioDetail.m_clientPort);
	 gboolean retVal = FALSE;

	 XGLOG_INFO("Client::start_voice_channel %d ",is_ccaas);

	 //Starting the channel based on request from the PBX
	 switch(mrcpState){
			case STT_TTS_STATE::EN_STT_TTS_ALLOCATE_PORT:
				 {
						if(!is_ccaas || (is_ccaas && ccaas_port_type != EN_XGATE_STT)){
							 XGLOG_INFO("Client::start_voice_channel before activating tts channel %s",(char*)ur_log_string(m_detail.m_callId));
							 HmpChannel *pHmpTtsChannel = get_active_tts_channel();
							 if(pHmpTtsChannel){
									retVal = pHmpTtsChannel->start();
									cout<<"Starting the ccaas channel!!!!"<<endl;
							 }
							 XGLOG_INFO("Client::start_voice_channel after activating tts channel %s",(char*)ur_log_string(m_detail.m_callId));
						}
						break;
				 }
			case STT_TTS_STATE::EN_TTS_START:
				 { 
						if(!is_ccaas || (is_ccaas && ccaas_port_type != EN_XGATE_STT)){
							 XGLOG_INFO("Client::start_voice_channel before activating EN_TTS_START tts channel %s",(char*)ur_log_string(m_detail.m_callId));
							 HmpChannel *pHmpTtsChannel = get_active_tts_channel();
							 if(pHmpTtsChannel){
									retVal = pHmpTtsChannel->start();
							 }
							 XGLOG_INFO("Client::start_voice_channel after activating EN_TTS_START tts channel %s",(char*)ur_log_string(m_detail.m_callId));
						}
						break;
				 }
	 }

	 //Starting  the channel based on request from the ModeuleBrigde
	 if(mrcpCommand){
			switch(mrcpCommand)
			{
				 case EN_TTS_CHANNEL_START_REQ:
						{
							 if(m_pHmpInhouseStreamingChannel){
									cout<<"Starting the inhouse channel"<<endl;
									retVal = m_pHmpInhouseStreamingChannel->start();
							 }

						}
			}
	 }

	 return retVal;
}

gboolean Client::stop_voice_channel(STT_TTS_STATE mrcpState,xGateMrcpCommand mrcpCommand)
{
	 XGLOG_INFO( "Client::stop_voice_channel for call_id: %s and client: %s:%d start", \
				 (char*)ur_log_string(m_detail.m_callId), (char*)ur_log_string(m_detail.audioDetail.m_clientIp), m_detail.audioDetail.m_clientPort);
	 gboolean retVal = FALSE;

	 //stopping the channel based on the request from the bridge module

	 if(mrcpCommand){
			switch(mrcpCommand)
			{
				 case EN_TTS_CHANNEL_STOP_REQ:
						{
							 if(m_pHmpInhouseStreamingChannel){
									cout<<"Starting the inhouse channel"<<endl;
									retVal = m_pHmpInhouseStreamingChannel->stop();
							 }

						}
				 case EN_MSG_TRANSFER_REQ:
						{ 
							 XGLOG_INFO("Client::stop_voice_channel End for stt channel");
							 if(m_pHmpSTTVoiceRePacketizeChannel){
									m_pHmpSTTVoiceRePacketizeChannel->stop();
							 }
							 else if(m_pHmpSTTVoiceRelayChannel){
									m_pHmpSTTVoiceRelayChannel->stop();
							 }
							 else if(m_pHmpSTTVoiceTranscodingChannel){
									m_pHmpSTTVoiceTranscodingChannel->stop();
							 }
							 XGLOG_INFO("Client::stop_voice_channel End for stt channel");
						}
			}
	 }

	 switch(mrcpState){
			case STT_TTS_STATE::EN_STT_STOP:
				 {
						XGLOG_INFO("Client::stop_voice_channel  before EN_STT_STOP for HmpVoiceTransceiverChannel",(char*)ur_log_string(m_detail.m_callId));
						m_pHmpVoiceTransceiverChannel->stop();
						XGLOG_INFO("Client::stop_voice_channel after EN_STT_STOP for HmpVoiceTransceiverChannel",(char*)ur_log_string(m_detail.m_callId));
				 }
	 }

	 return retVal;
}

gboolean Client::start_stt_voice_channel()
{
	 XGLOG_INFO( "Client::start_stt_voice_channel for call_id: %s and client: %s:%d start", \
				 (char*)ur_log_string(m_detail.m_callId), (char*)ur_log_string(m_detail.audioDetail.m_clientIp), m_detail.audioDetail.m_clientPort);
	 gboolean retVal = FALSE;

	 if(m_detail.m_asrType == EN_XGATE_ASR_SPEECH_MATICS || m_detail.m_asrType == EN_XGATE_ASR_INHOUSE){
			if(m_pHmpVoiceTransceiverChannel)
				 retVal=m_pHmpVoiceTransceiverChannel->start();
	 }

	 XGLOG_INFO( "Client::start_stt_voice_channel for call_id: %s End",(char*)ur_log_string(m_detail.m_callId));
	 return retVal;
}

gboolean Client::stop_stt_voice_channel()
{
	 XGLOG_INFO( "Client::stop_stt_voice_channel for call_id: %s and client: %s:%d start", \
				 (char*)ur_log_string(m_detail.m_callId), (char*)ur_log_string(m_detail.audioDetail.m_clientIp), m_detail.audioDetail.m_clientPort);
	 gboolean retVal = FALSE;

	 if(m_detail.m_asrType == EN_XGATE_ASR_SPEECH_MATICS || m_detail.m_asrType == EN_XGATE_ASR_INHOUSE){
			if(m_pHmpVoiceTransceiverChannel)
				 retVal=m_pHmpVoiceTransceiverChannel->stop();
	 }

	 XGLOG_INFO( "Client::stop_stt_voice_channel for call_id: %s End",(char*)ur_log_string(m_detail.m_callId));
	 return retVal;
}


gboolean Client::set_media_event_details(xGateMediaEvent mediaEvent, xGateMgMsg* pMgMsg)
{
	 MgMediaDetail &mediaDetail = pMgMsg->get_media_detail();
	 mediaDetail.media_event = mediaEvent;
	 switch(mediaEvent) {
			//suve
			/*case EN_MEDIA_EVENT_TRANSFER:
				{
				pMgMsg->set_mg_msg_type(EN_XGATE_MRCP_TRANSFER_REL);
				mediaDetail.transfer_to=m_detail.m_transExt;
				break;
				}*/
			case EN_MEDIA_EVENT_PLAY_TEXT_EOS:
				 {
						pMgMsg->set_mg_msg_type(EN_XGATE_STT_TTS_PLAY_TEXT);
						break;
				 }
			case EN_MEDIA_EVENT_STT_TTS_ALLOC_RES:
				 {
						mediaDetail.mrcp_ip_info = m_detail.audioDetail.m_serverIp;
						mediaDetail.mrcp_port = to_string(m_detail.audioDetail.m_serverPort);
						mediaDetail.mrcp_state = STT_TTS_STATE::EN_STT_TTS_ALLOCATE_PORT;
						mediaDetail.ccaas_voice_recog_port = m_detail.audioDetail.m_server64Port;
						pMgMsg->set_mg_msg_type(EN_MEDIA_STT_TTS_PORT_ALLOCATE);
						break;
				 }
			case EN_MEDIA_EVENT_NULL:
			default:
				 {
						break;
				 }
	 }
#if 1 //TODO: we can optimize below code blocks by making ClientDetails and MgMediaDetail make common struct
	 mediaDetail.mgresource_id = m_detail.m_mgresourceId;
	 mediaDetail.call_type = m_detail.m_callType;
	 mediaDetail.call_id = m_detail.m_callId;
         mediaDetail.group_call_id = m_detail.m_group_callId;
         mediaDetail.switch_call_id = m_detail.m_switch_callId;
	 mediaDetail.leg_id = m_detail.m_legId;
	 mediaDetail.audioDetail.m_clientIp = m_detail.audioDetail.m_clientIp;
	 mediaDetail.audioDetail.m_clientPort = m_detail.audioDetail.m_clientPort;
	 mediaDetail.call_dir = m_detail.m_callDir;
	 mediaDetail.sig_type = m_detail.m_sigType;
#else
	 mediaDetail = m_detail;
#endif

	 if(m_detail.m_callDir == EN_CALL_DIR_IN) {
			mediaDetail.audioDetail.m_respIp = m_detail.audioDetail.m_serverIp;
			mediaDetail.audioDetail.m_respPort = m_detail.audioDetail.m_serverPort;
	 } else if(m_detail.m_callDir == EN_CALL_DIR_OUT) {
			mediaDetail.audioDetail.m_dialIp = m_detail.audioDetail.m_serverIp;
			mediaDetail.audioDetail.m_dialPort = m_detail.audioDetail.m_serverPort;
	 }

	 return TRUE;
}

gboolean Client::post_media_event_result(xGateMediaEvent mediaEvent,string ip,unsigned short port)
{
	 XGLOG_INFO( "Client::post_pbx_event for client: %s:%d and leg:%d start", \
				 (char*)ur_log_string(m_detail.audioDetail.m_clientIp), m_detail.audioDetail.m_clientPort, m_detail.m_legId);

	 xGateMgMsg* pMgMsg = new xGateMgMsg();
	 xGateNetConnection pbxConInfo;
	 pbxConInfo.recvIp_ = ip;
	 pbxConInfo.recvPort_ = port;
	 pMgMsg->set_net_con_info(pbxConInfo);

	 pMgMsg->setMsgType(EN_XGATE_MSG_MGC_MG_IN);
	 if(set_media_event_details(mediaEvent, pMgMsg)) {
			ACE_Task<ACE_MT_SYNCH>* taskPtr = NULL;
			taskPtr = xGateUtil::getMgDispatcher();
			if(!taskPtr) {
				 XGLOG_ERROR( "post_pbx_event MgDispatcher task not found");
				 return EN_XGATE_STATUS_ERROR;
			}

			ACE_Message_Block* pAmb = NULL;
			pAmb = static_cast<xGateMgMsg*> (pMgMsg);
			ACE_Time_Value tval ((time(NULL) ));
			tval += ACE_Time_Value(0,1);

			if (taskPtr->putq(pAmb, &tval)) {
				 XGLOG_INFO( "post_pbx_event MgDispatcher enqueue message success");
				 return TRUE;
			} else {
				 XGLOG_ERROR( "post_pbx_event MgDispatcher enqueue message failed");
				 delete pMgMsg;
			}
	 }
	 return FALSE;
}

gboolean Client::post_media_event_result(xGateMediaEvent mediaEvent)
{
	 XGLOG_INFO( "Client::post_pbx_event for client: %s:%d and leg:%d start", \
				 (char*)ur_log_string(m_detail.audioDetail.m_clientIp), m_detail.audioDetail.m_clientPort, m_detail.m_legId);
	 xGateMgMsg* pMgMsg = new xGateMgMsg();
	 xGateNetConnection pbxConInfo;
	 pbxConInfo.recvIp_ = m_detail.m_pbxIp;
	 pbxConInfo.recvPort_ = m_detail.m_pbxPort;
	 pMgMsg->set_net_con_info(pbxConInfo);

	 pMgMsg->setMsgType(EN_XGATE_MSG_MGC_MG_IN);
	 if(set_media_event_details(mediaEvent, pMgMsg)) {
			ACE_Task<ACE_MT_SYNCH>* taskPtr = NULL;
			taskPtr = xGateUtil::getMgDispatcher();
			if(!taskPtr) {
				 XGLOG_ERROR( "post_pbx_event MgDispatcher task not found");
				 return EN_XGATE_STATUS_ERROR;
			}

			ACE_Message_Block* pAmb = NULL;
			pAmb = static_cast<xGateMgMsg*> (pMgMsg);
			ACE_Time_Value tval ((time(NULL) ));
			tval += ACE_Time_Value(0,1);

			if (taskPtr->putq(pAmb, &tval)) {
				 XGLOG_INFO( "post_pbx_event MgDispatcher enqueue message success");
				 return TRUE;
			} else {
				 XGLOG_ERROR( "post_pbx_event MgDispatcher enqueue message failed");
				 delete pMgMsg;
			}
	 }
	 return FALSE;
}


/* Client class implementation - End
 */

/*xGateHmpCall class implementation - Start
 */
xGateHmpCall::xGateHmpCall(string mgresourceId, xGateCallType callType) : 
	 m_mgresourceId(mgresourceId), m_callType(callType), m_clientCount(0),
	 m_callState(EN_CALL_STATE_IN_PROGRESS)
{
	 m_clientList.clear();
}

xGateHmpCall::~xGateHmpCall()
{
	 remove_client_entries(); 
}

gboolean xGateHmpCall::insert_client_entry(Client *client)
{ 
	 if(!client) {
			XGLOG_ERROR( "insert_client_entry failed. Invalid client object !!");
			return FALSE;
	 }
	 ClientDetail &clientDetail = client->m_detail;
	 if(!get_client_entry(clientDetail.audioDetail.m_clientIp, clientDetail.audioDetail.m_clientPort, clientDetail.m_callId)) {
			m_clientList.push_back(client);
			++m_clientCount;
			XGLOG_INFO("xGateHmpCall::insert_client_entry count :%d ",m_clientCount);
	 } else {
			XGLOG_ERROR( "insert_client_entry failed. Client entry already available for call_id:%s and client:%s:%d !!", \
						(char*)ur_log_string(clientDetail.m_mgresourceId), (char*)ur_log_string(clientDetail.audioDetail.m_clientIp), clientDetail.audioDetail.m_clientPort); 
	 }
	 XGLOG_INFO( "insert_client_entry success for call_id:%s and client:%s:%d !!", \
				 (char*)ur_log_string(clientDetail.m_callId), (char*)ur_log_string(clientDetail.audioDetail.m_clientIp), clientDetail.audioDetail.m_clientPort);
	 return TRUE;
}

Client * xGateHmpCall::get_client_entry(string clientIp, gint clientPort, gint legId)
{
	 if(m_clientList.empty()) {
			XGLOG_ERROR( "get_client_entry by legId : failed for cliet ip:port %s;%d and legId:%d, becuase clientList is empty !!", \
						(char*)ur_log_string(clientIp), clientPort, legId);
			return NULL;
	 }
	 Client *client = NULL;
	 CLIENT_LIST::iterator it = m_clientList.begin();
	 for(it; it != m_clientList.end(); ++it) {
			client = (*it);   
			if(client) {
				 ClientDetail &clientDetail = client->m_detail;
				 if((clientIp == clientDetail.audioDetail.m_clientIp) && (clientPort == clientDetail.audioDetail.m_clientPort) && (legId == clientDetail.m_legId)) {
						return client; 
				 }
			}
	 }
	 return NULL;
}

Client * xGateHmpCall::get_client_entry(string clientIp, gint clientPort, string callId)
{
	 if(m_clientList.empty()) {
			XGLOG_ERROR( "get_client_entry by callId : failed for client ip:port %s;%d and callId:%s, becuase clientList is empty !!", \
						(char*)ur_log_string(clientIp), clientPort, (char*)ur_log_string(callId));
			return NULL;
	 }
	 Client *client = NULL;
	 CLIENT_LIST::iterator it = m_clientList.begin();
	 for(it; it != m_clientList.end(); ++it) {
			client = (*it);
			if(client) {
				 ClientDetail &clientDetail = client->m_detail;
				 if((clientIp == clientDetail.audioDetail.m_clientIp) && (clientPort == clientDetail.audioDetail.m_clientPort) && (callId == clientDetail.m_callId)) {
						return client;
				 }
			}
	 }
	 return NULL;
}

Client * xGateHmpCall::get_client_entry(string callId)
{
	 if(m_clientList.empty()) {
			XGLOG_ERROR( "get_client_entry by callId : failed for callId:%s, becuase clientList is empty !!", (char*)ur_log_string(callId));
			return NULL;
	 }
	 Client *client = NULL;
	 CLIENT_LIST::iterator it = m_clientList.begin();
	 for(it; it != m_clientList.end(); ++it) {
			client = (*it);
			if(client) {
				 ClientDetail &clientDetail = client->m_detail;
				 XGLOG_INFO( "get_client_entry by callId in list: %s, finding Callid (%s) !!",(char*)ur_log_string(clientDetail.m_callId),(char*)ur_log_string(callId));
				 if(callId == clientDetail.m_callId) {
						return client;
				 }
			}
	 }
	 return NULL;
}

Client * xGateHmpCall::get_client_entry(gint legId)
{
	 Client *client = NULL;
	 gint listSize = m_clientList.size();
	 if(m_clientList.size() > legId) {
			client = m_clientList[legId];
	 }
	 return client;
}

gboolean xGateHmpCall::remove_client_fromlist(Client *client)
{
	 if(m_clientList.empty()) {
			XGLOG_ERROR( "remove_client_fromlist failed for call_id:%s. clientList is empty !!", (char*)ur_log_string(client->m_detail.m_callId));
			return FALSE;
	 }
	 CLIENT_LIST::iterator itr = m_clientList.begin();
	 while(itr != m_clientList.end()) {
			if((*itr) == client) {
				 --m_clientCount;
				 XGLOG_INFO("remove_client_fromlist deleting object for call_id:%s. to delete from clientList !!", (char*)ur_log_string(client->m_detail.m_callId));
				 itr = m_clientList.erase(itr);
				 return TRUE;
			}
			itr++;
	 }
	 return FALSE;
}

gboolean xGateHmpCall::remove_client_entry(Client *client)
{
	 if(m_clientList.empty()) {
			XGLOG_ERROR( "remove_client_entry failed for call_id:%s. clientList is empty !!", (char*)ur_log_string(client->m_detail.m_callId));
			return FALSE;
	 }
	 CLIENT_LIST::iterator itr = m_clientList.begin();
	 while(itr != m_clientList.end()) {
			if((*itr) == client) {
				 --m_clientCount;
				 XGLOG_INFO("remove_client_entry deleting object for call_id:%s. to delete from clientList !!", (char*)ur_log_string(client->m_detail.m_callId));
#ifdef XGATE_HAS_SELF_AUDIT
				 //De-Register the Audit message after complete port clearing
				 xGateAuditMsg *auditMsg = new xGateAuditMsg(client->m_detail.m_callId, client->m_detail.m_mgresourceId);
				 auditMsg->setAuditMsgType(EN_AUDIT_MSG_DEREGSITER_CALL);
				 auditMsg->setAuditState(EN_AUDIT_STATE_CLIENT_ENTRY_DELETED);
				 AUDITUTIL->postToAudit(auditMsg); 
#endif
				 delete client;
				 client = NULL;
				 itr = m_clientList.erase(itr);
				 return TRUE;
			}
			itr++;
	 } 
	 return FALSE;
}

gboolean xGateHmpCall::remove_client_entries()
{
	 if(m_clientList.empty()) {
			XGLOG_ERROR( "remove_client_entries failed. clientList is empty !!");
			return TRUE;
	 }
	 if(m_callState == EN_CALL_STATE_IN_PROGRESS) {
			// Need to  handle the delete  port
	 }

	 CLIENT_LIST::iterator itr = m_clientList.begin();
	 while(m_clientCount != 0) {      
			Client *client = (*itr);
			if(client) {
				 XGLOG_INFO("remove_client_entries deleting object for call_id:%s. to delete from clientList !!", (char*)ur_log_string(client->m_detail.m_callId));
#ifdef XGATE_HAS_SELF_AUDIT
				 //De-Register the Audit message after complete port clearing
				 xGateAuditMsg *auditMsg = new xGateAuditMsg(client->m_detail.m_callId, client->m_detail.m_mgresourceId);
				 auditMsg->setAuditMsgType(EN_AUDIT_MSG_DEREGSITER_CALL);
				 auditMsg->setAuditState(EN_AUDIT_STATE_CLIENT_ENTRY_DELETED);
				 AUDITUTIL->postToAudit(auditMsg); 
#endif
				 XGLOG_INFO("Client deleted in destructor %x",client);
				 delete client;
				 client = NULL;
				 --m_clientCount;
			}
			itr++;
	 }
	 m_clientList.clear();
	 return TRUE;
}

GSocket * xGateHmpCall::allocate_audio_port(gint &audioPort, unsigned short IpFamily)
{
	 GSocket *audioSocket = NULL;
	 while(!audioSocket) {
			audioPort = HOSTGST_GET_AUDIO_PORT();
			audioSocket = xGateHmpGstUtil::create_udpsocket(audioPort,IpFamily);
	 }
	 return audioSocket;
}

xGateRetVal xGateHmpCall::create_stt_tts_audio_channels(Client *client)
{
	 /* Allocate udp port and socket for audio channel */

	 XGLOG_INFO(" xGateHmpCall::create_stt_tts_audio_channels, started !!!");
	 //gint audioPort = 0;
	 //gint audioTtsPort = 0;
	 guint ssrc = 0;
	 //GSocket *audioSocket = NULL;
	 //GSocket *audioTtsSocket=NULL;
	 m_mediaEvent = client->m_detail.m_mediaEvent;


	 //NOTE: Avoiding the IPV6 support in the STT/TTS MG

	 //allocate port for STT:

	 if(client->ccaas_port_type == EN_XGATE_STT || client->ccaas_port_type == EN_XGATE_BOTH)
	 {
			XGLOG_INFO("! xGateHmpCall::create_stt_tts_audio_channels, allocating 'STT' Port !!!");
			client->m_audioSocket = allocate_audio_port(client->m_audioPort);
			client->m_detail.audioDetail.m_serverPort=client->m_audioPort;
	 }

	 //allocate port for TTS:

	 if(client->ccaas_port_type == EN_XGATE_TTS || client->ccaas_port_type == EN_XGATE_BOTH)
	 {
			XGLOG_INFO("! xGateHmpCall::create_stt_tts_audio_channels, allocating 'TTS' Port !!!");
			client->m_audiottsSocket = allocate_audio_port(client->m_audiottsPort);
			client->m_detail.audioDetail.m_serverTtsPort=client->m_audiottsPort;
	 }

	 client->m_detail.audioDetail.m_serverIp=xGateUtil::getLocalIpAddr(AF_INET);

	 if(!INHOUSE_ENABLE)
	 {
			if(pipe(client->m_tts_fd) == -1){
				 return FALSE;
			}
			client->m_detail.m_tts_fd = client->m_tts_fd[1];
			cout<<"The tts_fd[0] is"<<client->m_tts_fd[0]<<endl;
			cout<<"The tts_fd[1] is"<<client->m_tts_fd[1]<<endl;
	 }

	 client->m_detail.m_mrcp_codec=8;
	 if((client->m_detail.audioDetail.m_codecName == "OPUS") || (client->m_detail.audioDetail.m_codecName == "opus"))
			client->m_detail.m_mrcp_ptime=20;
	 else
			client->m_detail.m_mrcp_ptime= client->m_detail.audioDetail.m_ptime;

	 if(client->m_detail.m_callType == EN_XGATE_CALL_TYPE_TRANSFER){
			XGLOG_INFO("xGateHmpCall::create_audio_channels EN_XGATE_CALL_TYPE_TRANSFER");
			return  EN_XGATE_STATUS_SUCCESS;
	 }

	 if(!client->create_stt_tts_audio_channel(client->m_audioPort,client->m_audioSocket,client->m_audiottsPort,client->m_audiottsSocket,ssrc,client)) {
			XGLOG_ERROR( "xGateHmpCall::create_stt_tts_audio_channels failed!!");
			return EN_XGATE_STATUS_ERROR;
	 }
	 XGLOG_INFO("xGateHmpCall::create_stt_tts_audio_channels Success");

	 return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateHmpCall::create_wave_encoder_audio_channels(Client *client)
{
	 /*allocate udp port and socket for wavencoder channel */
	 XGLOG_INFO(" xGateHmpCall::create_wave_encoder_audio_channels, started !!!");
	 gint audioPort = 0;
	 GSocket *audioSocket = NULL;

	 m_mediaEvent = client->m_detail.m_mediaEvent;

	 XGLOG_INFO("! xGateHmpCall::create_wave_encoder_audio_channels, allocating 'STT' Port !!!");
	 audioSocket = allocate_audio_port(audioPort);
	 client->m_detail.audioDetail.m_serverIp=xGateUtil::getLocalIpAddr(AF_INET);
	 client->m_detail.audioDetail.m_server64Port=audioPort;

	 cout<<"The wave encoder detail audio port is"<<client->m_detail.audioDetail.m_server64Port<<endl;

	 if(!client->create_wave_encoder_channel(audioPort,audioSocket,client)) {
			XGLOG_ERROR( "xGateHmpCall::create_wave_encoder_channel failed!!");
			return EN_XGATE_STATUS_ERROR;
	 }

	 XGLOG_INFO( "xGateHmpCall::create_wave_encoder_audio_channels Success");
	 return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateHmpCall::create_stt_tts_channel(ClientDetail &clientDetail)
{
	 XGLOG_INFO("xGateHmpCall::create_stt_tts_channel for m_mgresourceId: %s and client: %s:%d call-Id %s ", \
				 (char*)ur_log_string(clientDetail.m_mgresourceId), (char*)ur_log_string(clientDetail.audioDetail.m_clientIp), clientDetail.audioDetail.m_clientPort, (char*)ur_log_string(clientDetail.m_callId));

	 Client *client = NULL;
	 switch (m_callType) {
			case EN_XGATE_CALL_TYPE_SIMPLE:
			case EN_XGATE_CALL_TYPE_CCAAS:
			case EN_XGATE_CALL_TYPE_TRANSFER:
				 {
						client = get_client_entry(clientDetail.audioDetail.m_clientIp, clientDetail.audioDetail.m_clientPort, clientDetail.m_callId);
						XGLOG_INFO("xGateHmpCall::create_stt_tts_channel 'asr type' - %d, 'ccaas_port_type' - %d ",clientDetail.m_asrType,clientDetail.m_ccaas_port_type);

						if(!client) {
							 client = new Client(clientDetail);
							 client->is_ccaas = clientDetail.m_is_ccaas;
							 client->ccaas_port_type = clientDetail.m_ccaas_port_type;

							 // Allocate port for the STT and TTS:

							 if (create_stt_tts_audio_channels(client)== EN_XGATE_STATUS_ERROR) {
									XGLOG_ERROR( "create_stt_tts_audio_channel, creating audio channel for m_mgresourceId:%s and client:%s:%d failed!!", \
												(char*)ur_log_string(clientDetail.m_mgresourceId), (char*)ur_log_string(clientDetail.audioDetail.m_clientIp), clientDetail.audioDetail.m_clientPort);
									//Deleted the allocated ports for termination if creation of channels got failed

									if(client->m_audioPort > 0 && client->m_audioSocket){
										 g_object_unref(client->m_audioSocket);
										 client->m_audioSocket = NULL;
										 client->m_audioPort = 0; 
									}

									if(client->m_audiottsPort > 0 && client->m_audiottsSocket){
										 g_object_unref(client->m_audiottsSocket);
										 client->m_audiottsSocket = NULL;
										 client->m_audiottsPort = 0; 
									}

									delete client;
									client = NULL;
									return EN_XGATE_STATUS_ERROR;
							 }

							 if(BASE64_ENABLE){
									if(create_wave_encoder_audio_channels(client)== EN_XGATE_STATUS_ERROR) {
										 XGLOG_ERROR( "create_wave_encoder_audio_channels for m_mgresourceId:%s failed!!",(char*)ur_log_string(clientDetail.m_mgresourceId));
									}
									clientDetail.audioDetail.m_server64Port=client->m_detail.audioDetail.m_server64Port;
							 }

							 /*if(!client->is_ccaas || (client->is_ccaas && client->ccaas_port_type != EN_XGATE_TTS)){

									if(clientDetail.m_asrType == EN_XGATE_ASR_SPEECH_MATICS)
									{
										 cout<<"The SpeechmaticsClient triggred!!!!"<<endl;
										 XGLOG_INFO("xGateHmpCall::create_stt_tts_audio_channels CreateSpeechmaticsClient !!!!" );
										 if(!client->CreateSpeechmaticsClient())
										 {
												XGLOG_ERROR("xGateHmpCall::create_stt_tts_audio_channels CreateSpeechmaticsClient failed" );
												return EN_XGATE_STATUS_ERROR;
										 }
										 client->speechMaticsClient->m_uid = client->m_detail.m_mgresourceId;
										 client->speechMaticsClient->m_callid = client->m_detail.m_callId;
									} else if(clientDetail.m_asrType == EN_XGATE_ASR_INHOUSE)
									{
										 cout<<"The inhouse client triggred!!!!"<<endl;
										 if(!client->CreateInhouseClient())
										 {
												XGLOG_ERROR("xGateHmpCall::create_stt_tts_audio_channels CreateInhouseClient failed" );
												return EN_XGATE_STATUS_ERROR;
										 }
									} 
									else{
										 cout<<"The Grpc client triggred!!!!"<<endl;
									}
							 }*/

							 clientDetail.audioDetail.m_serverPort=client->m_detail.audioDetail.m_serverPort;
							 clientDetail.audioDetail.m_serverIp=client->m_detail.audioDetail.m_serverIp;
							 clientDetail.audioDetail.m_serverTtsPort=client->m_detail.audioDetail.m_serverTtsPort;
							 clientDetail.m_tts_fd=client->m_detail.m_tts_fd;

							 cout<<"The port details are"<<"clientDetail.m_ccaas_voice_recog_port:"<<clientDetail.m_ccaas_voice_recog_port<<"client->m_detail.m_ccaas_voice_recog_port"<<client->m_detail.m_ccaas_voice_recog_port<<endl;

							 insert_client_entry(client);
						} 
						else {
							 XGLOG_INFO( "xGateHmpCall::create_smatic_channel, client and channels already been created for client_id: %s in simple call: %s !", \
										 (char*)ur_log_string(clientDetail.audioDetail.m_clientIp), (char*)ur_log_string(clientDetail.m_mgresourceId));
						}

						break;
				 }
			case EN_XGATE_CALL_TYPE_UNKNOWN:
			default:
				 {
						XGLOG_INFO( "xGateHmpCall::create_smatic_channel failed for Unknown/Unsupported callType");
						return EN_XGATE_STATUS_ERROR;	  
						break; 
				 }
	 }
	 return EN_XGATE_STATUS_SUCCESS;
}

/*xGateHmpConference class implementation - Start
 */
xGateHmpConference::xGateHmpConference(string mconferenceId, xGateCallType confType) :
	 m_conferenceId(mconferenceId), m_confType(confType)/*, m_pHmpConferenceAudioChannel(NULL)*/
{
	 m_isHost = FALSE;
	 m_hostJoined = FALSE;
	 m_isRecStarted = FALSE;
	 m_snd_pbx = 0;
	 m_first_host = 0; 
}

xGateHmpConference::~xGateHmpConference()
{
	 remove_participant_entries();
	 m_pbx_map.clear();  
}

xGateRetVal xGateHmpConference::create_conference_channel(ClientDetail &clientDetail)
{
	 XGLOG_INFO( "xGateHmpConference::create_conference_channel for m_mgresourceId: %s and client: %s:%d call-Id %s ", \
				 (char*)ur_log_string(clientDetail.m_mgresourceId), (char*)ur_log_string(clientDetail.audioDetail.m_clientIp), clientDetail.audioDetail.m_clientPort, (char*)ur_log_string(clientDetail.m_callId));

	 //create client and channel objects for given client detail
	 Client *client = NULL;
	 switch (m_confType) {
			case EN_XGATE_CALL_TYPE_CONFERENCE:
				 {
						//check whether client object already been created
						client = get_participant_entry(clientDetail.m_callId);
						if(!client) {
							 client = new Client(clientDetail);
							 client->is_ccaas = clientDetail.m_is_ccaas;
							 client->ccaas_port_type = clientDetail.m_ccaas_port_type;
							 if (create_audio_channels(client)== EN_XGATE_STATUS_ERROR) {
									XGLOG_ERROR( "xGateHmpConference::create_conference_channel, creating conference audio channel for m_mgresourceId:%s and client:%s:%d failed!!", \
												(char*)ur_log_string(clientDetail.m_mgresourceId), (char*)ur_log_string(clientDetail.audioDetail.m_clientIp), clientDetail.audioDetail.m_clientPort);

									delete client;
									client = NULL;
									return EN_XGATE_STATUS_ERROR;
							 }
							 insert_participant_entry(clientDetail.m_callId.c_str(),client);
						} else {
							 XGLOG_INFO( "xGateHmpConference::create_conference_channel, client and chanels allready been created for client_id: %s in conferece call: %s !",(char*)ur_log_string(clientDetail.audioDetail.m_clientIp),(char*)ur_log_string(clientDetail.m_mgresourceId));
						}
						break;
				 }
			case EN_XGATE_CALL_TYPE_UNKNOWN:
			default:
				 {
						XGLOG_INFO( "xGateHmpConference::create_conference_channel failed for Unknown/Unsupported callType");
						return EN_XGATE_STATUS_ERROR;
				 }
	 }	 
	 return EN_XGATE_STATUS_SUCCESS;		 
}

xGateRetVal xGateHmpConference::create_audio_channels(Client *client)
{

	 XGLOG_INFO( "Enter the xGateHmpConference::create_audio_channels !!!");

	 client->m_audioSocket = allocate_audio_port(client->m_audioPort);
	 client->m_detail.audioDetail.m_serverIp = xGateUtil::getLocalIpAddr(AF_INET);   
	 client->m_detail.audioDetail.m_serverPort = client->m_audioPort;

	 if(!client->create_stt_tts_audio_channel(client->m_audioPort,client->m_audioSocket,0,NULL,0,client)) {
			XGLOG_ERROR( "xGateHmpConference::create_audio_channels failed!!");
			return EN_XGATE_STATUS_ERROR;
	 }

	 if(client->m_detail.m_asrType == EN_XGATE_ASR_SPEECH_MATICS){
			if(!client->CreateSpeechmaticsClient())
			{
				 XGLOG_ERROR("xGateHmpCall::create_smatic_audio_channels CreateSpeechmaticsClient failed" );
				 return EN_XGATE_STATUS_ERROR;
			}
			client->speechMaticsClient->m_uid = client->m_detail.m_mgresourceId;
			client->speechMaticsClient->m_callid = client->m_detail.m_callId;
	 }


	 if(client->m_detail.m_asrType == EN_XGATE_ASR_INHOUSE){
			if(!client->CreateInhouseClient())
			{
				 XGLOG_ERROR("xGateHmpCall::create_smatic_audio_channels CreateInHouseClient failed" );
				 return EN_XGATE_STATUS_ERROR;
			}
			client->inHouseClient->m_uid = client->m_detail.m_mgresourceId;
			client->inHouseClient->m_callid = client->m_detail.m_callId;
	 }

	 XGLOG_ERROR( "xGateHmpConference::create_audio_channels Success");
	 return EN_XGATE_STATUS_SUCCESS;
}

GSocket * xGateHmpConference ::allocate_audio_port(gint &audioPort, unsigned short IpFamily)
{
	 GSocket *audioSocket = NULL;
	 while(!audioSocket) {
			audioPort = HOSTGST_GET_AUDIO_PORT();
			audioSocket = xGateHmpGstUtil::create_udpsocket(audioPort,IpFamily);
	 }
	 return audioSocket;
}

// Start Conference participant MAP related interfaces
Client* xGateHmpConference::get_participant_entry(string callId)
{
	 Client* client = NULL;
	 CLIENT_MAP::iterator it= m_clientMap.find(callId);
	 if( it == m_clientMap.end() || NULL == it->second )
	 {
			XGLOG_ERROR( "find_participant_entry failed for call Id: %s", (char*)ur_log_string(callId));
			return NULL; // Return error
	 }

	 client = it->second;
	 if(client) {
			XGLOG_INFO( "find_participant_entry success for call Id: %s", (char*)ur_log_string(callId));
	 } else {
			XGLOG_ERROR( "find_participant_entry failed for call Id: %s", (char*)ur_log_string(callId));
	 }
	 return client;
}

xGateRetVal xGateHmpConference::insert_participant_entry(string callId, Client* client)
{
	 xGateRetVal retVal = EN_XGATE_STATUS_ERROR;

	 m_clientMap[callId]= client;

	 retVal = EN_XGATE_STATUS_SUCCESS;
	 return retVal;
}


xGateRetVal xGateHmpConference::remove_participant_entry(string callId)
{
	 xGateRetVal retVal = EN_XGATE_STATUS_ERROR;
	 XGLOG_INFO( "remove_participant_entry for callId: %s", (char*)ur_log_string(callId));

	 Client* client = NULL;

	 CLIENT_MAP::iterator it= m_clientMap.find(callId);
	 if( it == m_clientMap.end() || NULL == it->second )
	 {
			XGLOG_ERROR( "remove_participant_entry for callId: %s. Call ID not found. FATEL error.", (char*)ur_log_string(callId));
			return retVal;
	 }
	 client = it->second;
	 if(client) 
	 { 
#ifdef XGATE_HAS_SELF_AUDIT
			//De-Register the Audit message after complete port clearing
			xGateAuditMsg *auditMsg = new xGateAuditMsg(client->m_detail.m_callId, client->m_detail.m_mgresourceId);
			auditMsg->setAuditMsgType(EN_AUDIT_MSG_DEREGSITER_CALL);
			auditMsg->setAuditState(EN_AUDIT_STATE_CLIENT_ENTRY_DELETED);
			AUDITUTIL->postToAudit(auditMsg); 
#endif
			delete client;
			client = NULL;
	 }
	 m_clientMap.erase(callId);
	 retVal = EN_XGATE_STATUS_SUCCESS;
	 return retVal;
}

xGateRetVal xGateHmpConference::remove_participant_entries()
{
	 Client* client = NULL;

	 if(m_clientMap.empty()) {
			XGLOG_ERROR( "remove_participant_entries failed. clientMap is empty !!");
			return EN_XGATE_STATUS_SUCCESS;
	 } 

	 CLIENT_MAP::iterator itr = m_clientMap.begin();
	 while(itr != m_clientMap.end()) { 
			Client *client = itr->second; 
#ifdef XGATE_HAS_SELF_AUDIT
			//De-Register the Audit message after complete port clearing
			xGateAuditMsg *auditMsg = new xGateAuditMsg(client->m_detail.m_callId, client->m_detail.m_mgresourceId);
			auditMsg->setAuditMsgType(EN_AUDIT_MSG_DEREGSITER_CALL);
			auditMsg->setAuditState(EN_AUDIT_STATE_CLIENT_ENTRY_DELETED);
			AUDITUTIL->postToAudit(auditMsg); 
#endif
			delete client;
			client = NULL;
			++itr;
	 }
	 m_clientMap.clear();

	 return EN_XGATE_STATUS_SUCCESS;
}


