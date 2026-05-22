#ifndef __SFU_CLIENT_DATA_HANDLER_H_
#define __SFU_CLIENT_DATA_HANDLER_H_

#define MAX_RCVD_VIDEO_HANDLER_THREADS 1
#define MAX_RCVD_AUDIO_HANDLER_THREADS 1
#define MAX_RECORD_VIDEO_HANDLER_THREADS 1
#define MAX_RECORD_AUDIO_HANDLER_THREADS 1
#ifdef RECORD_ENABLED
class SFUClientMediaHandler {
	public:
		SFUClientMediaHandler(gpointer userdata, gint maxThreads, gboolean exclusive);
		virtual ~SFUClientMediaHandler();
		virtual gboolean init() = 0;

	protected:
		GThreadPool *m_tpool;
		gpointer m_userdata;
		gint m_maxThreads;
		gboolean m_exclusive;

	private:
		SFUClientMediaHandler(const SFUClientMediaHandler&);
		SFUClientMediaHandler& operator=(const SFUClientMediaHandler&);
};

/* SFUClientRcvdVideoHandler class implmentation - start */
class SFUClientRcvdVideoHandler : public SFUClientMediaHandler {
	public:
		SFUClientRcvdVideoHandler(gpointer userdata, gint maxThreads, gboolean exclusive);
		SFUClientRcvdVideoHandler();
		~SFUClientRcvdVideoHandler();
		gboolean init();

		GThreadPool *get_rcvd_video_handler_pool();
		gboolean push_to_recorder(xGateHmpConference *hmpConf,
                                         Client *client, GstBufferList *list);

	private:
		SFUClientRcvdVideoHandler(const SFUClientRcvdVideoHandler&);
		SFUClientRcvdVideoHandler& operator=(const SFUClientRcvdVideoHandler&);
		static void rcvd_video_handler(gpointer data, gpointer userdata);
		static gboolean push_to_record_video_handler(xGateHmpConference *hmpConf, Client *client, GstBufferList *list);
		static gboolean push_to_client_thread(Client *client, GstBufferList *list);
};
/* SFUClientRcvdVideoHandler class implmentation - end */

/* SFUClientRecordVideoHandler class implmentation - start */
class SFUClientRecordVideoHandler : public SFUClientMediaHandler {
	public:
		SFUClientRecordVideoHandler(gpointer userdata, gint maxThreads, gboolean exclusive);
		SFUClientRecordVideoHandler();
		~SFUClientRecordVideoHandler();
		gboolean init();

		GThreadPool *get_record_video_handler_pool();

	private:
		SFUClientRecordVideoHandler(const SFUClientRecordVideoHandler&);
		SFUClientRecordVideoHandler& operator=(const SFUClientRecordVideoHandler&);
		static void record_video_handler(gpointer data, gpointer userdata);
		static gboolean send_to_recorder(HmpRecordSendChannel *channel,
                                                GstBufferList *list);
		static gboolean push_to_recorder(xGateHmpConference *hmpConf,
                                         Client *client, GstBufferList *list);
};
/* SFUClientRecordVideoHandler class implmentation - end */

/* SFUClientRcvdAudioHandler class implmentation - start */
class SFUClientRcvdAudioHandler : public SFUClientMediaHandler {
	public:
		SFUClientRcvdAudioHandler(gpointer userdata, gint maxThreads, gboolean exclusive);
		SFUClientRcvdAudioHandler();
		~SFUClientRcvdAudioHandler();
		gboolean init();

		GThreadPool *get_rcvd_audio_handler_pool();
		static gboolean push_to_mrcp(Client *client, void *vObj);

		gboolean push_to_recorder(xGateHmpConference *hmpConf,
                                                 Client *client, void *vObj);
	private:
		SFUClientRcvdAudioHandler(const SFUClientRcvdAudioHandler&);
		SFUClientRcvdAudioHandler& operator=(const SFUClientRcvdAudioHandler&);
		static void rcvd_audio_handler(gpointer data, gpointer userdata);
		static gboolean push_to_record_audio_handler(xGateHmpConference *hmpConf,
                                               Client *client, GstBufferList *list);
		static gboolean push_to_client_thread(Client *client, GstBufferList *list);
};
/* SFUClientRcvdAudioHandler class implmentation - end */

/* SFUClientRecordAudioHandler class implmentation - start */
class SFUClientRecordAudioHandler : public SFUClientMediaHandler {
	public:
		SFUClientRecordAudioHandler(gpointer userdata, gint maxThreads, gboolean exclusive);
		SFUClientRecordAudioHandler();
		~SFUClientRecordAudioHandler();
		gboolean init();

		GThreadPool *get_record_audio_handler_pool();

		static gboolean push_to_recorder(xGateHmpConference *hmpConf,
                                                 Client *client, GstBufferList *list);
	private:
		SFUClientRecordAudioHandler(const SFUClientRecordAudioHandler&);
		SFUClientRecordAudioHandler& operator=(const SFUClientRecordAudioHandler&);
		static void record_audio_handler(gpointer data, gpointer userdata);
};
/* SFUClientRecordAudioHandler class implmentation - end */
#endif
#endif
