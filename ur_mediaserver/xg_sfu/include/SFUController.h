#ifndef _SFU_CONTROLLER_
#define _SFU_CONTROLLER_

/* 1. This thread object will manage gstreamer events, pipelines, bin and elements
 *  2. full name of SFUController
 *  3. It will manage gstreamer sources by using xGateHmpGstBin class
 */
#include <ace/Hash_Map_Manager.h>
#include <ace/Null_Mutex.h>

#include "xGateUtil.h"
#include "xGateHmpGstBin.h"
#include "xGateSFUService.h"

//hash map for conference entries & related functions
typedef ACE_Hash_Map_Manager<ACE_TString, xGateHmpConference*,  ACE_SYNCH_RW_MUTEX> HASH_HMP_CONF_MAP;

class SFUController
{
	public:
		SFUController(const char*);
		SFUController()
		{

		}
		virtual ~SFUController(void);
		/**
		 * Initialize the Hmp Gstreamer Manager class and thread
		 */
		xGateRetVal init(void);
		/**
		 * Start the event loop for processing messages/notifications. This method
		 * must be static in order to start event loop processing in a newly spawned thread.
		 * Also, this is an infinitely blocking call.
		 */
		static void run(void* arguments);
		/**
		 *  stopThread( void );
		 */ 
		xGateRetVal stopThread( void );
		xGateRetVal release_channel(MgMediaDetail &mediaDetail);
		xGateRetVal decode_media_msg(xGateMgMsg *pMgMsg);
		xGateRetVal add_participant(MgMediaDetail &mediaDetail);
		xGateRetVal allocate_conf_channel(MgMediaDetail &mediaDetail, xGateNetConnection &pbxConn); 
		xGateRetVal insert_conference_entry(string conferenceId, xGateHmpConference* pHmpConference);
		xGateRetVal remove_conference_entry(string conferenceId);
		xGateRetVal handle_record_event_msg(MgMediaDetail &mediaDetail);
		xGateHmpConference* find_conference_entry(string conferenceId);
		void add_callId_client_conf_map_list (MgMediaDetail &mediaDetail);
		xGateRetVal send_msg_sfu_client(MgMediaDetail *pmg_mediaDetail,
		                               ClientDetail &clientDetail, xGateMgMsgType eMsgType);
		void send_create_send_channel_request (MgMediaDetail &mediaDetail);
		void handle_conference_release (MgMediaDetail &mediaDetail);
	private:
		/**
		 * Private copy constructor - disallow copies and automatic methods.
		 **/
		SFUController(const SFUController& rhs);

		/**
		 * Private assignment operator - disallow assignment and automatic methods.
		 **/
		SFUController& operator= (const SFUController& rhs);

		/**
		 * Init Gstreamer Library
		 */
		xGateRetVal initGstreamerLib();

		bool m_running;
		bool m_gst_init_complete;
		char m_serverType[SERVER_TYPE_SIZE];

		GMainLoop *gMainLoop;

		//hash map for call entries & related functions
		HASH_HMP_CONF_MAP m_hmpConfMap;
		xGateRetVal set_media_details(MgMediaDetail &mediaDetail, ClientDetail &clientDetail);
		xGateRetVal update_media_details(ClientDetail &clientDetail, MgMediaDetail &mediaDetail);
		xGateRetVal create_doc_object(Document &doc, const char *jsonData, Value& dataMemObj);
};

#endif // _SFU_CONTROLLER_
