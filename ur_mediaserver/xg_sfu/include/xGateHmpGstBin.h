#ifndef _XGATE_HMP_GST_BIN_H
#define _XGATE_HMP_GST_BIN_H

#define DEFAULT_RECORD_SCREEN_WIDTH 640
#define DEFAULT_RECORD_SCREEN_HEIGHT 360

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
#ifdef GST_WEBRTC
#include <gst/sdp/sdp.h>
#include <gst/webrtc/webrtc.h>
#endif
}
#include <iostream>
#include <arpa/inet.h>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

#include <map> 
#include <ace/Task.h>
#include <ace/Singleton.h>

//Added for thread lock mechanism
#include <mutex>
#include <condition_variable>
//local include
#include "xGateUtil.h"
#include "xGateMgMsg.h"
#include "xGateHmpGstUtil.h"
#include "xGateHmpGstAudioBin.h"
#include "xGateDBServiceMsg.h"

#define HAVE_GST_DEBUG_PRINTS TRUE

typedef enum {
	XGATE_CLIENT_CREATE_RECV_CHANNEL = 0,
	XGATE_CLIENT_CREATE_SEND_CHANNEL,
	XGATE_CLIENT_CREATE_CHANNEL,
	XGATE_CLIENT_CREATE_OFFER,
	XGATE_CLIENT_DELETE_RECV_CHANNEL,
	XGATE_CLIENT_DELETE_SEND_CHANNEL,
	XGATE_CLIENT_DELETE_ALL_CHANNEL,
	XGATE_CLIENT_SEND_RTP_DATA,
	XGATE_CLIENT_MAX_MSG,
	XGATE_CLIENT_REC_EVENT_REQUEST,
	XGATE_CLIENT_SSRC_REMOVE_REQUEST,
} xGateHmpClientMsgType;

// class that stores SSRC, payload, seq_num
class offerDetail {
	public:
		unsigned int uiTxSSRCNum;
		unsigned int uiRTXTxSSRCNum;
		unsigned short usSeqNum;
		unsigned short usRTXSeqNum;
		unsigned short usPrevRcvdSeqNum;
		guint32 uiPrevRcvdTimestamp;
		guint32 uiTxTimestamp;
		int uiPayloadNum;
		int uiRTXPayloadNum;
		string MediaLineIdx;
		GstElement  *m_appsrc;
		string  sinkPadName;
		gboolean    bSendPkt;
		unsigned long ulNumPktSent;
		GstBufferList  *pOldBufferList;
		GstBufferList  *pNewBufferList;
};

// Client offer details for audio and video media
class mlineofferDetail : public ACE_Task<ACE_MT_SYNCH>
{
	public:
		offerDetail audioOfferDetails;
		offerDetail videoOfferDetails;
		Client      *pClient;
		string      callId_offer_generated;
		gboolean    bPopulated;
		xGateJoinerType eJoinerType;
		~mlineofferDetail() {
			if (callId_offer_generated.size())
				callId_offer_generated.clear();
			if (audioOfferDetails.sinkPadName.size())
				audioOfferDetails.sinkPadName.clear();
			if (videoOfferDetails.sinkPadName.size())
				videoOfferDetails.sinkPadName.clear();
			pClient = NULL;
			bPopulated = FALSE;
		}
};

class xGateHmpClientThreadMsg : public ACE_Message_Block
{
	public:
		xGateHmpClientMsgType m_msgType;
		void *param1;
		void *param2;
		void *param3;
		char ucName[40];
		string str_param;
};

/* Client class - Start
*/
class Client : public ACE_Task<ACE_MT_SYNCH> {
	public:
		Client();
		Client(ClientDetail &detail);
		~Client();

		//public member properties
		ClientDetail m_detail;

		HmpRecordAudioRecvChannel *m_pHmpRecordAudioRecvChannel;
		HmpRecordVideoRecvChannel *m_pHmpRecordVideoRecvChannel;
		HmpAudioVideoMuxRecorderChannel *m_pHmpAudioVideoMuxRecorderChannel;

		gboolean m_isConference;
		unsigned int m_clientCount;
		GstClockTime pausedTimestamp = GST_CLOCK_TIME_NONE;
		GstClockTime totalPauseDuration = GST_CLOCK_TIME_NONE;
		GstClockTime start_or_resume_timestamp;
		map<GstElement*, GstElement*> proxy_pairs;

		//public member functions
		gboolean create_audioconference_channel();

		gboolean post_media_event_result(xGateMediaEvent mediaEvent);
		gboolean set_media_event_details(xGateMediaEvent mediaEvent, xGateMgMsg* pMgMsg);
		void handle_send_updated_sdp_msg (Client *client);
		gboolean handle_rec_event_request();
		gboolean handle_client_ssrc_remove_request();
		gboolean remove_elements_from_recv_channel(GstPad *pad, GstBin *bin);
		GstPad* get_rtpbin_ssrc_pad(GstElement *rtpBin, string ssrc);
		gboolean Set_Record_file();
		string Get_Record_Client_Info(string data, string duration);
		string Encode_meet_record_client_info();
		string Encode_ccaas_meet_record_client_info();
		void Fetch_recordfile_info(Client *client);
		bool Post_record_info_to_DB(Client *client);
		string encode_meet_user_info_record();
		xGateDBServiceMsg* FormDBRequestMsg(string &strContext, string &spName, string &strData, DBServiceEvent event);
		void set_thread_state (gboolean);
		void handle_client_stop_channel (char *callId_str);
		static int client_thr_func(void *arg);
		bool process_client_msg(ACE_Message_Block *pAmb);
		gboolean Iterate_over_CompositorSinkpads(HmpRecordVideoRecvChannel *m_pHmpRecordVideoRecvChannel);
		gboolean remove_pads_and_elements(GstElement* audio_rtpbin, GstElement* video_rtpbin, unsigned int audio_ssrc, unsigned int video_ssrc);
    void set_pipeline_to_pause();
    void set_pipeline_to_playing();
    void handle_send_resume_response(Client *client);
    void send_keyframe_request(string element_name);
	private:
		//private member variables
		gboolean m_run;

		//private member functions
		string form_channel_name(string name);
		GSocket * allocate_audio_port(gint &audioPort);
		GSocket * allocate_video_port(gint &videoPort);
		gboolean create_recording_channel(GSocket *audioSocket, gint audioPort, GSocket *videoSocket, gint videoPort);
		gboolean create_audiovideo_mix_channel();
};
/* Client class - End
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
		guint    m_webrtcCount;
		CLIENT_MAP m_clientMap;
		map<string,Client*> m_confClientMap;

		xGateHmpConference(string conferenceId, xGateCallType confType); 
		~xGateHmpConference();
		Client* get_client(string);
		xGateRetVal add_client(string,Client*); 
		xGateRetVal create_channel(ClientDetail &clientDetail);
		xGateRetVal create_conference_channel(Client *client);
		xGateRetVal create_video_channels(Client *client);
		xGateRetVal add_route_participant(Client *client, MgMediaDetail &mediaDetail);
		xGateRetVal activate_play_audio_clientall(MgMediaDetail &mediaDetail);
		xGateRetVal activate_play_audio_client(Client *client, MgMediaDetail &mediaDetail);
		xGateRetVal remove_participant(Client *client);
		xGateRetVal remove_conf_participant(Client *client);
		xGateRetVal remove_send_channel(Client *client,
				string callId);

		//CLIENT_MAP related functions
		xGateRetVal insert_participant_entry(string callId, Client *client);
		Client * get_participant_entry(string callId);
		xGateRetVal remove_participant_entry(string callId);
		xGateRetVal remove_participant_entries();
		//HmpConferenceAudioChannel *m_pHmpConferenceAudioChannel;
};


#endif
