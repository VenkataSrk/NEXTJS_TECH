//local include
#include "xglog.h"
//
#include "xGateHmpGstManager.h"
#include "xGateHmpGstBin.h"
#include "xGateHmpGstAudioBin.h"
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>
#include <gst/rtp/gstrtpbuffer.h>
#include <gst/rtp/gstrtcpbuffer.h>
#include <sys/stat.h>
#include "xGateUtil.h"
#include "types.h"

#define THISMODULE "GstAudio"
#define GST_BUFFER_DATA(buf) (GST_BUFFER(buf)->data)	
#define GST_BUFFER_SIZE(buf) (GST_BUFFER(buf)->size)

/* HmpBin Base class - Start
 */
//constructor and destuctor

using namespace std;

HmpBin::HmpBin(string binName, HmpBinType binType) : 
	 m_binName(binName), m_binType(binType), m_bin(NULL),
	 m_recvPort(0), m_sendPort(0), m_recvSocket(NULL), 
	 m_sendSocket(NULL), m_udpRecv(NULL), m_udpSend(NULL) 
{
	 m_bin = GST_BIN(gst_bin_new(binName.c_str()));
}

HmpBin::~HmpBin()
{
	 XGLOG_INFO( "HmpBin::~HmpBin start for bin: %s", (char*)ur_log_string (m_binName));

	 if(m_recvSocket) {
			XGLOG_INFO( "HmpBin destructing recv port: %d socket", m_recvPort);
			g_socket_close(m_recvSocket, NULL);
			g_object_unref(G_OBJECT(m_recvSocket));
			m_recvSocket = NULL;
			m_recvPort = 0;
	 }

	 if(m_sendSocket) {
			XGLOG_INFO( "HmpBin destructing send port: %d socket", m_sendPort);
			g_socket_close(m_sendSocket, NULL);
			g_object_unref(G_OBJECT(m_sendSocket));
			m_sendSocket = NULL;
			m_sendPort = 0;
	 }

	 XGLOG_INFO( "HmpBin::~HmpBin start for bin: %s", (char*)ur_log_string (m_binName));
}

gboolean HmpBin::create_udpsrc(gboolean isTTS)
{
	 XGLOG_INFO( "create_udpsrc start");
	 if(!m_udpRecv) {
			m_udpRecv = gst_element_factory_make("udpsrc", NULL);
			if(m_udpRecv) {
				 if(m_recvSocket) {
						g_object_set(G_OBJECT(m_udpRecv), "socket", m_recvSocket, NULL);
				 } else {
						XGLOG_ERROR( "create_udpsrc failed due to invalid udp socket !!"); 
						return FALSE;
				 }

				 GstCaps* caps = get_caps(isTTS);
				 if(caps) {
						g_object_set(G_OBJECT(m_udpRecv), "caps", caps, NULL);
						gst_caps_unref(caps);
				 } else {
						XGLOG_ERROR( "create_udpsrc setting caps for udpsrc failed !!"); 
						return FALSE;
				 }
				 g_object_set(G_OBJECT(m_udpRecv), "timeout", 0, "reuse", TRUE, "close-socket", FALSE, NULL);
			} else {
				 XGLOG_ERROR( "create_udpsrc gst_element_make udpsrc failed !!");
				 return FALSE;
			}
	 }
	 return TRUE;
}

gboolean HmpBin::create_udpsink(gboolean flag)
{
	 XGLOG_INFO( "HmpBin::create_udpsink start");
	 gint64 maxLateness = 40000000;
	 if(!m_udpSend) {
			m_udpSend = gst_element_factory_make("multiudpsink", NULL);
			if(m_udpSend) {
				 if(m_sendSocket) {
						if(flag == FALSE){
							 g_object_set(G_OBJECT(m_udpSend), "socket", m_sendSocket, NULL);
						}
						else
						{
							 g_object_set(G_OBJECT(m_udpSend),"socket-v6",m_sendSocket,NULL);
						} 
				 } else {
						XGLOG_ERROR( "create_udpsink, failed due to invalid udp socket !!"); 
						return FALSE;
				 }
				 g_object_set(G_OBJECT(m_udpSend),"async", FALSE, \
							 "sync", TRUE, "close-socket", FALSE, "send-duplicates", FALSE, NULL);
				 g_object_set(G_OBJECT(m_udpSend),"max-lateness", maxLateness, NULL);
			} else {
				 XGLOG_ERROR( "create_udpsink, gst_element_make multiudpsink failed !!");
				 return FALSE;
			}
	 }
	 return TRUE;
}

gboolean HmpBin::add_client_addr(string clientIp, gint clientPort, gboolean addExistingClients)
{
	 XGLOG_INFO( "HmpBin::add_client_addr setting new Address %s:%d to UDP socket", (char*)ur_log_string (clientIp), clientPort);
	 if(m_udpSend) {
			string allClientAddrs("");
			gchar *existingClientAddr = NULL;
			char clientAddr[256] = {0};

			if(addExistingClients) {
				 /* Getting existing client from m_udpSend */
				 g_object_get(G_OBJECT(m_udpSend),"clients", &existingClientAddr, NULL);
				 if(existingClientAddr[0] != 0) {
						allClientAddrs.append(existingClientAddr);
						allClientAddrs += ",";
				 }
				 g_free(existingClientAddr);
				 if(!allClientAddrs.empty()){
						XGLOG_INFO( "HmpBin::add_client_addr Having existing address : %s in Socket ", (char*)ur_log_string (allClientAddrs));
				 }
				 else
				 {
						XGLOG_INFO( "HmpBin::add_client_addr Having zero or no existing addr");
				 }
			}
			snprintf(clientAddr,256,"%s:%d", clientIp.c_str(), clientPort);
			allClientAddrs.append(clientAddr);

			/* Setting existing & new client into m_udpSend */
			g_object_set(G_OBJECT(m_udpSend),"clients", allClientAddrs.c_str(), NULL);
			XGLOG_INFO( "HmpBin::add_client_addr New address %s is setted in udp socket ", (char*)ur_log_string (allClientAddrs));

	 } else {
			XGLOG_ERROR( "HmpBin::add_client_addr Not able to set new address %s:%d in udp socket ", (char*)ur_log_string (clientIp), clientPort);
			return FALSE;
	 }
	 return TRUE;
}

gboolean HmpBin::clear_client_addr()
{
	 if(m_udpSend) {
			char clientAddr[40] = {0};
			snprintf(clientAddr, 40, "%s", "0.0.0.0:0");   
			g_object_set(G_OBJECT(m_udpSend),"clients", clientAddr, NULL); 
	 } else return FALSE;

	 return TRUE; 
}

string HmpBin::get_existing_addr()
{
	 string clientAddrs("");
	 gchar *existingAddr;
	 if(m_udpSend) {
			g_object_get(G_OBJECT(m_udpSend),"clients", &existingAddr, NULL);
			clientAddrs.append(existingAddr);
			g_free(existingAddr);
	 } 
	 return clientAddrs;
}

gboolean HmpBin::set_existing_addr(string existingAddr)
{
	 gboolean ret = FALSE;
	 if(m_udpSend) {
			g_object_set(G_OBJECT(m_udpSend),"clients", existingAddr.c_str(), NULL);   
			return TRUE;
	 }
	 return ret;
}

GstCaps * HmpBin::get_caps(gboolean isTTS=FALSE)
{
	 GstCaps *caps = NULL;
	 switch (m_binType) {
			case EN_BIN_G711U_RECV:
			case EN_BIN_G711U_SEND:
			case EN_BIN_DTLS_SEND_G711U:
				 {
						XGLOG_INFO( "HmpBin::get_caps for G711U_RECV bin from UDP_CAPS_G711U string");
						if(isTTS)
							 caps = gst_caps_from_string(UDP_CAPS_G711U_TTS);
						else
							 caps = gst_caps_from_string(UDP_CAPS_G711U);  
						break;
				 }
			case EN_BIN_G711A_RECV:
			case EN_BIN_G711A_SEND:
			case EN_BIN_DTLS_SEND_G711A:
				 {
						XGLOG_INFO( "HmpBin::get_caps for G711A_RECV bin from UDP_CAPS_G711A string");
						if(isTTS)
							 caps = gst_caps_from_string(UDP_CAPS_G711A_TTS);
						else
							 caps = gst_caps_from_string(UDP_CAPS_G711A);  
						break;
				 }
			case EN_BIN_G722_RECV:
			case EN_BIN_G722_SEND:
			case EN_BIN_DTLS_SEND_G722:
				 {
						XGLOG_INFO( "HmpBin::get_caps for G722_RECV bin from UDP_CAPS_G722 string");
						caps = gst_caps_from_string(UDP_CAPS_G722);  
						break;
				 }
			case EN_BIN_DTLS_RECV:
				 {
						XGLOG_INFO( "HmpBin::get_caps for DTLS_RECV bin from UDP_CAPS_DTLS string");
						caps = gst_caps_from_string(UDP_CAPS_DTLS);  
						break;
				 }
			case EN_BIN_OPUS_RECV:
				 {
						XGLOG_INFO( "HmpBin::get_caps for OPUS_RECV bin from UDP_CAPS_OPUS string");
						m_payload = UDP_CAPS_OPUS + m_payload;
						caps = gst_caps_from_string(m_payload.c_str());  
						break;
				 }
			case EN_BIN_G729_RECV:
			case EN_BIN_G729_SEND:
			case EN_BIN_DTLS_SEND_G729:
				 {
						XGLOG_INFO( "HmpBin::get_caps for G729_RECV bin from UDP_CAPS_G729 string");
						caps = gst_caps_from_string(UDP_CAPS_G729);  
						break;
				 }
			case EN_BIN_DTLS_SEND_OPUS:
				 {
						XGLOG_INFO( "HmpBin::get_caps for DTSL_SEND bin from UDP_CAPS_OPUS string");
						caps = gst_caps_from_string(UDP_CAPS_OPUS);  
						break;
				 }
			case EN_BIN_OPUS_SEND:  
				 {
						XGLOG_INFO( "HmpBin::get_caps for SEND bin from string UDP_CAPS_L16");
						caps = gst_caps_from_string(UDP_CAPS_L16);  
						break;
				 }
			default:
				 {
						XGLOG_INFO( "HmpBin::get_caps failed for Unknown/Unsupported bin");
						break;
				 }
	 }
	 return caps;
}

gboolean HmpBin::draw_dot_file()
{
#if GST_DEBUG_DRAW_GRAPH
	 if(GST_DEBUG && m_bin && !m_binName.empty()) {
			GST_DEBUG_BIN_TO_DOT_FILE(m_bin, GST_DEBUG_GRAPH_SHOW_ALL, m_binName.c_str());
			return TRUE;
	 }
	 return FALSE;
#endif 
	 return TRUE;
}
/* HmpBin Base class - End
 */

/* HmpChannel bass class - Start
 */
HmpChannel::HmpChannel(string pipelineName, HmpChannelType pipelineType) :
	 m_pipelineName(pipelineName), m_pipelineType(pipelineType), m_pipeline(NULL),
	 m_bus(NULL), m_busId(0),m_pipeline_status(0)
{
	 XGLOG_INFO( "HmpChannel::HmpChannel construction start");
	 m_pipeline = gst_pipeline_new(m_pipelineName.c_str());
	 if(!m_pipeline) {
			XGLOG_ERROR( "HmpChannel::HmpChannel, %s pipeline object creation failed!");
			return;
	 }
	 m_bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
}

HmpChannel::~HmpChannel()
{
	 XGLOG_INFO( "HmpChannel::~HmpChannel destructing %s start", (char*)ur_log_string (m_pipelineName));

	 if(m_bus) {
			XGLOG_INFO( "HmpChannel::~HmpChannel BFR  m_bus unref ");
			gst_bus_set_sync_handler(m_bus, NULL, NULL, NULL);
			gst_object_unref(GST_OBJECT (m_bus));
			m_bus = NULL;
			XGLOG_INFO( "HmpChannel::~HmpChannel AFR  m_bus unref ");
	 }

	 if(m_busId) {
			XGLOG_INFO( "HmpChannel::~HmpChannel BFR removing  m_busId ");
			g_source_remove (m_busId);
			m_busId = NULL;
			XGLOG_INFO( "HmpChannel::~HmpChannel AFR removing m_busId ");
	 }

#if 1 //TODO: Yoga who disabled this part
	 if(m_pipeline) {
			// gst_element_set_state(m_pipeline, GST_STATE_NULL);
			XGLOG_INFO( "HmpChannel::~HmpChannel BFR pipeline %s unref  !!!", m_pipelineName.c_str());
			gst_object_unref (GST_OBJECT (m_pipeline));
			m_pipeline = NULL;
			XGLOG_INFO( "HmpChannel::~HmpChannel AFR pipeline %s unref !!!", m_pipelineName.c_str());
			return;
	 }
#endif

}

gboolean HmpChannel::register_handle_event_callback(gpointer data)
{
	 if(m_pipeline && m_bus && data) {
			m_busId = gst_bus_add_watch(m_bus, &handle_event, data); 
			return (m_busId != nullptr);
	 } 
	 return FALSE;
}

gboolean HmpChannel::handle_event(GstBus* bus, GstMessage* msg, gpointer data)
{
	 Client *client = (Client *)data;
	 if(!client) {
			XGLOG_ERROR( "handle_event, invalid client object !!");
			return TRUE;
	 }

	 switch (GST_MESSAGE_TYPE(msg)) {
			case GST_MESSAGE_ERROR:
				 {
						XGLOG_INFO( "handle_event got GST_MESSAGE_ERROR message %s",client->m_detail.m_callId.c_str());
						GError *err = NULL;
						gchar *dbgInfo = NULL;
						if(GST_DEBUG) {
							 gst_message_parse_error(msg, &err, &dbgInfo);
							 if(err) {           
									XGLOG_ERROR( "ERROR from element %s: %s", (char*)ur_log_string (GST_OBJECT_NAME(msg->src)), (char*)ur_log_string (err->message));
									g_error_free(err);
							 }
							 if(dbgInfo) {
									// g_printerr("Debugging info: %s\n", (dbgInfo) ? dbgInfo : "none");
									XGLOG_ERROR( "Debugging info: %s\n", (char*)ur_log_string (dbgInfo));
									g_free(dbgInfo);
							 }
						}
						break;
				 }
			case GST_MESSAGE_EOS:
				 {
						cout<<"There is no packets in pipeline"<<endl;
						cout<<"The EOS has trigggered!!!!!!!!!!!"<<endl;

						if(client)
							 client->post_msg_stop_pipeline();

						XGLOG_INFO( "handle_event EOS encountered. EN_MEDIA_EVENT_NULL !!");
						break;
				 } 
			case GST_MESSAGE_ELEMENT:
				 {
						const GstStructure* gstStruct = gst_message_get_structure(msg);
						const gchar *structName = gst_structure_get_name(gstStruct);
						unsigned int curr_ssrc;
						unsigned int curr_seq_no;
						unsigned int curr_timestamp;

						if(!strcmp(structName, "GstFdSrcTimeout")){
							 cout<<"There is no packets in pipeline!!!!!"<<endl;
							 fflush(stdout);

							 client->m_pHmpInhouseStreamingChannel->stop();

							 g_object_get(client->m_pHmpInhouseStreamingChannel->m_pHmpInhouseStreamingBin->m_rtpPay,"seqnum",&curr_seq_no,NULL);     
							 g_object_get(client->m_pHmpInhouseStreamingChannel->m_pHmpInhouseStreamingBin->m_rtpPay,"ssrc",&curr_ssrc,NULL);     
							 g_object_get(client->m_pHmpInhouseStreamingChannel->m_pHmpInhouseStreamingBin->m_rtpPay,"timestamp",&curr_timestamp,NULL);     

							 g_object_set(client->m_pHmpInhouseStreamingChannel->m_pHmpInhouseStreamingBin->m_rtpPay,"ssrc",curr_ssrc ,NULL);
							 g_object_set(client->m_pHmpInhouseStreamingChannel->m_pHmpInhouseStreamingBin->m_rtpPay,"seqnum-offset",curr_seq_no ,NULL);
							 g_object_set(client->m_pHmpInhouseStreamingChannel->m_pHmpInhouseStreamingBin->m_rtpPay,"timestamp-offset",curr_timestamp,NULL);


							 xGateDbConfigMgrServiceMsg *pServicemsg = new xGateDbConfigMgrServiceMsg();
							 pServicemsg->set_mrcp_command(EN_TTS_SPEAK_COMPLETE);
							 pServicemsg->m_uid =client->m_detail.m_mgresourceId;
							 pServicemsg->m_callId=client->m_detail.m_callId;
							 xGateUtil::getHmpProcessor()->PostMessageToMrcpClient(pServicemsg);
						}
				 }
			case GST_MESSAGE_UNKNOWN:
			default:
				 {

						break;
				 }
	 }
	 return TRUE; //it's important to return TRUE from this function
}

gboolean HmpChannel::draw_dot_file()
{
#if GST_DEBUG_DRAW_GRAPH
	 gboolean retVal = FALSE;
	 if(GST_DEBUG && m_pipeline && !m_pipelineName.empty()) {
			GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(m_pipeline), GST_DEBUG_GRAPH_SHOW_ALL, m_pipelineName.c_str());
			retVal = TRUE;
	 }
	 return retVal;
#endif 
	 return TRUE;
}

gboolean HmpChannel::draw_pipeline_no_start()
{
	 gboolean retVal = FALSE;
	 if(m_pipeline && !m_pipelineName.empty()) {
			GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(m_pipeline), GST_DEBUG_GRAPH_SHOW_ALL, m_pipelineName.c_str());
			retVal = TRUE;
	 }
	 return retVal;
}

gboolean HmpChannel::start()
{
	 gboolean retVal = FALSE;
	 if(m_pipeline) {
			XGLOG_INFO("BFR calling pipeline start : %s" ,m_pipelineName.c_str());
			m_pipeline_status=gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
			XGLOG_INFO("sucessfully started pipeline : %s" ,m_pipelineName.c_str());
			if(m_pipeline_status==GST_STATE_CHANGE_FAILURE){
         XGLOG_INFO("Failed to start pipeline : %s" ,m_pipelineName.c_str());
				 retVal = FALSE;
			}else{
         XGLOG_INFO("sucessfully statrted pipeline : %s" ,m_pipelineName.c_str());
				 retVal = TRUE;
      }
	 }
	 draw_dot_file();
	 return retVal;
}

gboolean HmpChannel::stop()
{
	 gboolean retVal = FALSE;
	 if(m_pipeline) {
			XGLOG_INFO("BFR calling pipeline stop : %s" ,m_pipelineName.c_str());
			gst_element_set_state(m_pipeline, GST_STATE_NULL);
			XGLOG_INFO("sucessfully stopped pipeline : %s" ,m_pipelineName.c_str());
			retVal = TRUE;
	 }

	 return retVal;
}
/* HmpChannel bass class - End
 */

/* HmpVoiceTranscodeChannel derived class - Start
 */
HmpVoiceTranscodeChannel::HmpVoiceTranscodeChannel(string VoiceMRCPPipelineName, HmpChannelType VoiceMRCPPipelineType) :
	 HmpChannel(VoiceMRCPPipelineName, VoiceMRCPPipelineType), m_pHmpVoiceRecvDecoderBin(NULL), m_pHmpVoiceSendEncoderBin(NULL)
{
	 XGLOG_INFO( "HmpVoiceTranscodeChannel::HmpVoiceTranscodeChannel construction start");
}

HmpVoiceTranscodeChannel::~HmpVoiceTranscodeChannel()
{
	 XGLOG_INFO( "HmpVoiceTranscodeChannel::~HmpVoiceTranscodeChannel destruction start");
	 if(m_pHmpVoiceRecvDecoderBin) {
			XGLOG_INFO( "BFR deleting the VoiceRecvDecoderBin in the VoiceTranscodeChannel");
			delete m_pHmpVoiceRecvDecoderBin;
			m_pHmpVoiceRecvDecoderBin = NULL;
			XGLOG_INFO( "AFR deleting the VoiceRecvDecoderBin in the VoiceTranscodeChannel");
	 }
	 if(m_pHmpVoiceSendEncoderBin) {
			XGLOG_INFO( "BFR deleting the VoiceSendEncoderBin in the VoiceTranscodeChannel");
			delete m_pHmpVoiceSendEncoderBin;
			m_pHmpVoiceSendEncoderBin = NULL;
			XGLOG_INFO( "AFR deleting the VoiceSendEncoderBin in the VoiceTranscodeChannel");
	 }
}

gboolean HmpVoiceTranscodeChannel::construct_bin(string recvBinName, HmpBinType recvBinType, xGateTTSType ttsType,\
			string sendBinName, HmpBinType sendBinType,HmpCodecType sendCodecType,int recvPort, GSocket *recvSendSock)
{
	 XGLOG_INFO( "HmpVoiceTranscodeChannel::construct_bin %s start", (char*)ur_log_string (recvBinName));
	 XGLOG_INFO( "HmpVoiceTranscodeChannel::construct_bin %s start", (char*)ur_log_string (sendBinName));

	 gboolean isInhouseTTS = FALSE;
	 if(ttsType == EN_XGATE_TTS_INHOUSE)
			isInhouseTTS =TRUE;

	 if(!m_pHmpVoiceRecvDecoderBin) {
			m_pHmpVoiceRecvDecoderBin = new HmpVoiceRecvDecoderBin(recvBinName, recvBinType,ttsType, recvPort, recvSendSock,isInhouseTTS); 
			string codecType = to_string(111);
			m_pHmpVoiceRecvDecoderBin->m_payload = ",payload=(int)";
			m_pHmpVoiceRecvDecoderBin->m_payload.append(codecType.c_str());

			if(!m_pHmpVoiceRecvDecoderBin->construct()) {
				 XGLOG_ERROR( "construct_bin, VoiceRecvBin construction failed !!");
				 return FALSE;
			} else {
				 gst_bin_add_many(GST_BIN(m_pipeline), GST_ELEMENT(m_pHmpVoiceRecvDecoderBin->m_bin), NULL); 
			}
	 }	

	 if(!m_pHmpVoiceSendEncoderBin) {
			m_pHmpVoiceSendEncoderBin = new HmpVoiceSendEncoderBin(sendBinName,sendBinType,ttsType,sendCodecType, recvSendSock); 
			if(!m_pHmpVoiceSendEncoderBin->construct()) {
				 XGLOG_ERROR( "construct_bin, VoiceRecvBin construction failed !!");
				 return FALSE;
			} else {
				 gst_bin_add_many(GST_BIN(m_pipeline), GST_ELEMENT(m_pHmpVoiceSendEncoderBin->m_bin), NULL); 
			}
	 }
	 attach_audio_recv_send_bin();

	 return TRUE;
}

gboolean HmpVoiceTranscodeChannel::add_client_addr(string clientIp, gint clientPort, gboolean addExistingClients)
{
	 if(m_pHmpVoiceSendEncoderBin) {
			m_pHmpVoiceSendEncoderBin->add_client_addr(clientIp, clientPort, addExistingClients);
			return TRUE;
	 }
	 return FALSE;  
}

gboolean HmpVoiceTranscodeChannel::clear_client_addr()
{
	 if(m_pHmpVoiceSendEncoderBin) {
			m_pHmpVoiceSendEncoderBin->clear_client_addr();
			return TRUE;
	 }
	 return FALSE;
}

gboolean HmpVoiceTranscodeChannel::set_ptime(gint ptime, gint maxPtime)
{
	 if(m_pHmpVoiceSendEncoderBin) {
			return m_pHmpVoiceSendEncoderBin->set_ptime(ptime, maxPtime);
	 }
	 return FALSE;  
}

gboolean HmpVoiceTranscodeChannel::attach_audio_recv_send_bin()
{
	 GstPad *decoder_ghost_srcpad, *audioconvert_ghost_sinkpad = NULL;
	 decoder_ghost_srcpad = m_pHmpVoiceRecvDecoderBin->m_ghostRtpSrcPad;
	 audioconvert_ghost_sinkpad = m_pHmpVoiceSendEncoderBin->m_ghostRtpSinkPad;

	 if(decoder_ghost_srcpad && audioconvert_ghost_sinkpad) {
			GstPadLinkReturn isPadLinked;
			isPadLinked = gst_pad_link(decoder_ghost_srcpad, audioconvert_ghost_sinkpad);
			if(GST_PAD_LINK_OK != isPadLinked) {
				 XGLOG_ERROR( "attach_audio_recv_send_bin failed. not able to attach audiodecoder bin with audioencoder !!");
				 return FALSE;
			} else {
				 XGLOG_INFO( "attach_audio_recv_send_bin, attaching audiodecoder bin with audioencoder bin success");
				 return TRUE;
			}
	 }

	 m_pHmpVoiceRecvDecoderBin->draw_dot_file();
	 m_pHmpVoiceSendEncoderBin->draw_dot_file();  
	 GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN (m_pHmpVoiceRecvDecoderBin->m_bin), GST_DEBUG_GRAPH_SHOW_ALL, "Attach_VoiceDecoderChannel");
	 GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN (m_pHmpVoiceSendEncoderBin->m_bin), GST_DEBUG_GRAPH_SHOW_ALL, "Attach_VoiceEncoderChannel");
	 return FALSE;
}

/* HmpVoiceTranscodeChannel derived class - End
 */

/* HmpVoiceRecvDecoderBin derived class - Start
 */
HmpVoiceRecvDecoderBin::HmpVoiceRecvDecoderBin(string binName, HmpBinType binType, xGateTTSType ttsType, \
			gint recvPort, GSocket *recvSock,gboolean isTTS=FALSE) :
	 HmpBin(binName, binType),  m_decJitterBuffer(NULL), m_rtpDepay(NULL), 
	 m_audioDecoder(NULL), m_audioRate(NULL), m_audioConvert(NULL), m_audioResample(NULL),m_ttsType(ttsType),
   m_id1(NULL),m_id2(NULL),m_id3(NULL),m_id4(NULL)
{
	 XGLOG_INFO( "HmpVoiceRecvDecoderBin::HmpVoiceRecvDecoderBin construction start");
	 m_recvPort = recvPort;
	 m_recvSocket = recvSock;
	 m_ttsFlag=isTTS;
}

HmpVoiceRecvDecoderBin::~HmpVoiceRecvDecoderBin()
{
	 if(m_recvSocket){
			XGLOG_INFO( "HmpVoiceRecvDecoderBin destructing dtmf port: %d socket", m_recvSocket);
			g_socket_close(m_recvSocket, NULL);
			g_object_unref(G_OBJECT(m_recvSocket));
			m_recvSocket = NULL;
			/* Same socket is used for m_sendSocket and m_recvSocket to send and receive the stream, set other variable also to NULL */
			m_sendSocket = NULL;
			m_recvPort = 0;
	 }
}

gboolean HmpVoiceRecvDecoderBin::construct()
{
	 XGLOG_INFO( "HmpVoiceRecvDecoderBin::construct start");

	 if(!create_udpsrc(m_ttsFlag)) {
			XGLOG_ERROR( "construct HmpVoiceRecvDecoderBin, creating udpsrc failed !!");
			return FALSE;
	 }

	 //create required decode elements
	 if(!create_decode_elements()) {
			XGLOG_ERROR( "construct HmpVoiceRecvDecoderBin, creating decode elements failed !!");
			return FALSE;
	 }

	 //add and link elements in bin container
	 if(!add_and_link_elements()) {
			XGLOG_ERROR( "construct HmpVoiceRecvDecoderBin, add and linking elements failed !!");
			return FALSE;
	 }

	 return TRUE;
}

gboolean HmpVoiceRecvDecoderBin::create_decode_elements()
{
	 XGLOG_INFO( "HmpVoiceRecvDecoderBin::create_decode_elements start");
	 guint latency = 40;
	 guint quality = 8;
	 guint codec_g729 = 0;

	 if(m_ttsType == EN_XGATE_TTS_INHOUSE){
			guint latency = 100;
	 }

	 switch(m_binType) {
			case EN_BIN_G711U_RECV:
				 {
						XGLOG_INFO( "create_decode_elements for G711U_RECV bin");
						m_rtpDepay = gst_element_factory_make("rtppcmudepay", NULL);
						m_audioDecoder = gst_element_factory_make("mulawdec", NULL);
						break;
				 }
			case EN_BIN_G711A_RECV:
				 {
						XGLOG_INFO( "create_decode_elements for G711A_RECV bin");
						m_rtpDepay = gst_element_factory_make("rtppcmadepay", NULL);
						m_audioDecoder = gst_element_factory_make("alawdec", NULL);
						break;
				 }
			case EN_BIN_G722_RECV:
				 {
						XGLOG_INFO( "create_decode_elements for G722_RECV bin");
						m_rtpDepay = gst_element_factory_make("rtpg722depay", NULL);
						m_audioDecoder = gst_element_factory_make("avdec_g722", NULL);
						break;
				 }
			case EN_BIN_G729_RECV:
				 {
						XGLOG_INFO( "create_decode_elements for G729_RECV bin");
						m_rtpDepay = gst_element_factory_make("rtpg729depay", NULL);
						m_audioDecoder = gst_element_factory_make("avdec_g729", NULL);
						codec_g729 = 1;
						break;
				 }
			case EN_BIN_OPUS_RECV:
				 {
						XGLOG_INFO( "create_decode_elements for OPUS_RECV bin");
						m_rtpDepay = gst_element_factory_make("rtpopusdepay", NULL);
						m_audioDecoder = gst_element_factory_make("opusdec", NULL);
						break;
				 }
			case EN_BIN_DTLS_RECV:
				 {
						XGLOG_INFO( "create_decode_elements for DTLS_RECV bin");
						m_rtpDepay = gst_element_factory_make("rtpopusdepay", NULL);
						m_audioDecoder = gst_element_factory_make("opusdec", NULL);
						m_dtlssrtpDec = gst_element_factory_make("stundtlssrtpdec", NULL);
						break;
				 }
			default:
				 {
						XGLOG_ERROR( "create_decode_elements failed for Unknown/Unsupported voice recv bin !!");
						return FALSE;
				 }
	 }

	 m_decJitterBuffer = gst_element_factory_make("rtpjitterbuffer", NULL);
	 g_object_set(G_OBJECT(m_decJitterBuffer), "latency", latency, NULL);

   m_id1 = gst_element_factory_make("identity",NULL);
   m_id2 = gst_element_factory_make("identity",NULL);
   m_id3 = gst_element_factory_make("identity",NULL);
   m_id4 = gst_element_factory_make("identity",NULL);

   if(m_id1)
   g_object_set(G_OBJECT(m_id1),"signal-handoffs",FALSE,NULL);
   if(m_id2)
   g_object_set(G_OBJECT(m_id2),"signal-handoffs",FALSE,NULL);
   if(m_id3)
   g_object_set(G_OBJECT(m_id3),"signal-handoffs",FALSE,NULL);
   if(m_id4)
   g_object_set(G_OBJECT(m_id4),"signal-handoffs",FALSE,NULL);

	 if((xGateHmpGstUtil::getenabledecresampler()&& !codec_g729) || (xGateHmpGstUtil::getenableg729decresampler() && codec_g729)){
			m_audioResample = gst_element_factory_make("audioresample", NULL);
			g_object_set (G_OBJECT(m_audioResample), "quality", quality, NULL);
	 }

	 if((xGateHmpGstUtil::getenabledecconverter() && !codec_g729)|| (xGateHmpGstUtil::getenableg729decconverter() && codec_g729)){
			m_audioConvert = gst_element_factory_make("audioconvert", NULL);
	 }

	 return TRUE;
}

gboolean HmpVoiceRecvDecoderBin::set_handoff()
{
  if(m_id1)
  g_object_set(G_OBJECT(m_id1),"signal-handoffs",TRUE,NULL);
  if(m_id2)
  g_object_set(G_OBJECT(m_id2),"signal-handoffs",TRUE,NULL);
  if(m_id3)
  g_object_set(G_OBJECT(m_id3),"signal-handoffs",TRUE,NULL);
  if(m_id4)
  g_object_set(G_OBJECT(m_id4),"signal-handoffs",TRUE,NULL);

  return TRUE;
}

gboolean HmpVoiceRecvDecoderBin::add_and_link_elements()
{
	 GstPad *pad = NULL;
	 GstElement *m_src,*m_sink;
	 GstPad* srcpad = NULL;
	 GstPad* sinkpad = NULL;
	 GstPadLinkReturn lres;
	 XGLOG_INFO( "HmpVoiceRecvDecoderBin::add_and_link_elements start");
	 if(!m_decJitterBuffer || !m_rtpDepay || !m_audioDecoder) {
			XGLOG_ERROR( "add_and_link_elements failed. elements not creaed properly !!");
			return FALSE;
	 } 

	 switch(m_binType) {
			case EN_BIN_G711A_RECV:
			case EN_BIN_G711U_RECV:
			case EN_BIN_G722_RECV:
			case EN_BIN_OPUS_RECV:
				 {
						XGLOG_INFO( "add_and_link_elements for G711A/G711U RECV bin");      
						gst_bin_add_many(m_bin, m_udpRecv, m_id1, m_decJitterBuffer, m_id2, m_rtpDepay, m_id3, m_audioDecoder,NULL); 
						if(xGateHmpGstUtil::getenabledecconverter()){        
							 gst_bin_add(m_bin, m_audioConvert);
						}
						if(xGateHmpGstUtil::getenabledecresampler()){
							 gst_bin_add(m_bin,m_audioResample);
						}

						gst_element_link_many(m_udpRecv, m_id1, m_decJitterBuffer, m_id2, m_rtpDepay, m_id3, m_audioDecoder, NULL);


						if(m_ttsType == EN_XGATE_TTS_INHOUSE){
							 if(xGateHmpGstUtil::getenabledecconverter()){
									gst_element_link_many(m_audioDecoder,m_audioConvert,NULL);
									if(xGateHmpGstUtil::getenabledecresampler()){
										 gst_element_link_many(m_audioConvert, m_audioResample, NULL);
										 pad = gst_element_get_static_pad(m_audioResample, "src");
									}
							 }
						}
						else{
							 if(xGateHmpGstUtil::getenabledecconverter()){
									gst_element_link_many(m_audioDecoder,m_audioConvert,NULL);
									if(xGateHmpGstUtil::getenabledecresampler()){
                     gst_bin_add_many(m_bin, m_audioDecoder, m_audioResample, m_id4,NULL);
										 gst_element_link_many(m_audioConvert, m_audioResample, m_id4, NULL);
										 //pad = gst_element_get_static_pad(m_audioResample, "src");
										 pad = gst_element_get_static_pad(m_id4, "src");
									}
									else
									{
										 pad = gst_element_get_static_pad(m_audioConvert, "src");
									}
							 }
							 else
							 {
									if(xGateHmpGstUtil::getenabledecresampler()){
										 gst_element_link_many(m_audioDecoder, m_audioResample, NULL);
										 pad = gst_element_get_static_pad(m_audioResample, "src");
									}
									else
									{
										 pad = gst_element_get_static_pad(m_audioDecoder, "src");

									}   
							 }
						}
						break;
				 }
			case EN_BIN_DTLS_RECV:
				 {
						XGLOG_INFO( "add_and_link_elements for EN_BIN_DTLS_RECV bin");      
						GstPad *srcpad,*sinkpad;

						gst_bin_add_many(m_bin,m_udpRecv,m_dtlssrtpDec,m_rtpDepay, m_audioDecoder, NULL); 

						if(xGateHmpGstUtil::getenabledecconverter()){        
							 gst_bin_add(m_bin, m_audioConvert);
						}

						if(xGateHmpGstUtil::getenabledecresampler()){
							 gst_bin_add(m_bin,m_audioResample);
						}

						gst_element_link_many(m_udpRecv,m_dtlssrtpDec,m_rtpDepay, m_audioDecoder, NULL);

						if(xGateHmpGstUtil::getenabledecconverter()){
							 gst_element_link_many(m_audioDecoder,m_audioConvert,NULL);
							 if(xGateHmpGstUtil::getenabledecresampler()){
									gst_element_link_many(m_audioConvert, m_audioResample, NULL);
									pad = gst_element_get_static_pad(m_audioResample, "src");
							 }
							 else
							 {
									pad = gst_element_get_static_pad(m_audioConvert, "src");
							 }
						}
						else
						{
							 if(xGateHmpGstUtil::getenabledecresampler()){
									gst_element_link_many(m_audioDecoder,m_audioResample,NULL);
									pad = gst_element_get_static_pad(m_audioResample, "src");
							 }
							 else
							 {
									pad = gst_element_get_static_pad(m_audioDecoder, "src");

							 }   
						}
						break;
				 }
			default:
				 {
						XGLOG_ERROR( "create_decode_elements failed for Unknown/Unsupported voice recv bin !!");
						return FALSE;
				 }
	 }

	 if(pad) {
			m_ghostRtpSrcPad = gst_ghost_pad_new("src", pad);
			gst_element_add_pad(GST_ELEMENT(m_bin), m_ghostRtpSrcPad);
			gst_object_unref(pad);
	 }
	 return TRUE;
}

gboolean HmpVoiceRecvDecoderBin::register_identity_callback(gpointer data)
{
  Client *client=(Client *)data;
  map<GstElement*,struct st>m_IdMap;

  if(m_id1){
  client->m_idset.p="Identity element between m_udpRecv and m_decJitterBuffer";
  client->m_IdMap.insert(pair<GstElement *,struct st>(m_id1,client->m_idset));
  g_signal_connect(G_OBJECT(m_id1), "handoff", G_CALLBACK(&identity_callback), data);
  }

  if(m_id2){
  client->m_idset.p="Identity element between m_decJitterBuffer and m_rtpDepay";
  client->m_IdMap.insert(pair<GstElement *,struct st>(m_id2,client->m_idset));
  g_signal_connect(G_OBJECT(m_id2), "handoff", G_CALLBACK(&identity_callback), data);
  }

  if(m_id3){
  client->m_idset.p="Identity element between m_rtpDepay and m_audioDecoder";
  client->m_IdMap.insert(pair<GstElement *,struct st>(m_id3,client->m_idset));
  g_signal_connect(G_OBJECT(m_id3), "handoff", G_CALLBACK(&identity_callback), data);
  }

  if(m_id4){
  client->m_idset.p="Identity element between the last element of the VoiceRecvDecoderBin and the first element of the VoiceSendEncoderBin";
  client->m_IdMap.insert(pair<GstElement *,struct st>(m_id4,client->m_idset));
  g_signal_connect(G_OBJECT(m_id4), "handoff", G_CALLBACK(&identity_callback), data);
  }

  return TRUE;
}

void HmpVoiceRecvDecoderBin::identity_callback(GstElement * identity,GstBuffer * buffer,gpointer data)
{
   Client *client=(Client *)data;

   if(!data)
     return;

   map<GstElement *,struct st>::iterator itr;

   itr=client->m_IdMap.find(identity);
   if(itr != client->m_IdMap.end())
   {
     itr->second.c++;

     if((itr->second.c==1)||(itr->second.c%100==0))
     XGLOG_INFO("HmpVoiceRecvDecoderBin::identity_callback count:%u, %s---->callId:%s",itr->second.c, itr->second.p,client->m_detail.m_callId.c_str());
   }
     return;
}

/* HmpVoiceRecvDecoderBin derived class - End
 */

/* HmpVoiceSendEncoderBin derived class - Start
 */
HmpVoiceSendEncoderBin::HmpVoiceSendEncoderBin(string binName,HmpBinType binType,xGateTTSType ttsType,HmpCodecType codecType, GSocket *sendSocket) :
	 HmpBin(binName, binType), m_encoder(NULL), m_rtpPay(NULL),m_codecType(codecType),m_ttsType(ttsType),m_capsFilter(NULL),m_id5(NULL),m_id6(NULL),m_id7(NULL)
{
	 m_sendSocket = sendSocket;
	 XGLOG_INFO( "HmpVoiceSendEncoderBin::e HmpVoiceSendEncoderBin construction start");
}

HmpVoiceSendEncoderBin::~HmpVoiceSendEncoderBin()
{
	 //Note: We are purposely making it NULL to avoid crash. 
	 //it's not recommented and this will make socket open
	 if(m_sendSocket){
			XGLOG_INFO( "HmpVoiceSendEncoderBin::~HmpVoiceSendEncoderBin destruction start");
			m_sendSocket = NULL;
	 }
}

gboolean HmpVoiceSendEncoderBin::construct()
{
	 //allocate loopback port and updsink for internal packet routing
	 while(!m_sendSocket) {
			m_sendPort = HOSTGST_GET_LOOPBACK_PORT();
			m_sendSocket = xGateHmpGstUtil::create_udpsocket(m_sendPort);
	 }

	 if(!create_udpsink()) {
			XGLOG_ERROR( "construct HmpVoiceSendEncoderBin, creating udpsink failed !!");
			return FALSE;
	 }

	 //create required encoded elements
	 if(!create_encode_elements()) {
			XGLOG_ERROR( "construct HmpVoiceSendEncoderBin, creating encode elements failed !!");
			return FALSE;
	 }

	 //add and link elements in bin container
	 if(!add_and_link_elements()) {
			XGLOG_ERROR( "construct HmpVoiceSendEncoderBin, add and linking elements failed !!");
			return FALSE;
	 }

	 return TRUE;
}

gboolean HmpVoiceSendEncoderBin::create_encode_elements()
{
	 guint quality = 8;
	 GstCaps *caps = NULL;
	 XGLOG_INFO( "HmpVoiceSendEncoderBin::create_encode_elements start");

	 if(m_ttsType == EN_XGATE_TTS_INHOUSE){
			m_capsFilter = gst_element_factory_make("capsfilter", NULL);
	 }

	 switch(m_codecType) {
			case EN_CODEC_PCMU:
				 {
						XGLOG_INFO( "create_encode_elements for G711U_SEND bin");
						m_encoder = gst_element_factory_make("mulawenc", NULL);
						m_rtpPay = gst_element_factory_make("rtppcmupay", NULL);
						caps = gst_caps_from_string("audio/x-raw, channels=(int)1, rate=(int)8000");
						break;
				 }
			case EN_CODEC_PCMA:
				 {
						XGLOG_INFO( "create_encode_elements for G711A_SEND bin");
						m_encoder = gst_element_factory_make("alawenc", NULL);
						m_rtpPay = gst_element_factory_make("rtppcmapay", NULL);
						caps = gst_caps_from_string("audio/x-raw, channels=(int)1, rate=(int)8000");
						break;
				 }
			case EN_CODEC_G722:
				 {
						XGLOG_INFO( "create_encode_elements for G711A_SEND bin");
						m_encoder = gst_element_factory_make("avenc_g722", NULL);
						m_rtpPay = gst_element_factory_make("rtpg722pay", NULL);
						g_object_set(G_OBJECT(m_rtpPay),"pt", 9, NULL);
						caps = gst_caps_from_string("audio/x-raw, channels=(int)1, rate=(int)16000");
						break;
				 }
			case EN_CODEC_G729:
				 {
						XGLOG_INFO( "create_encode_elements for G729_SEND bin");
						m_encoder = gst_element_factory_make("g729enc", NULL);
						m_rtpPay = gst_element_factory_make("rtpg729pay", NULL);
						break;
				 }
			case 96 ... 127:
				 {
						XGLOG_INFO( "create_encode_elements for OPUS_SEND bin");
						m_encoder = gst_element_factory_make("opusenc", NULL);
						m_rtpPay = gst_element_factory_make("rtpopuspay", NULL);
						g_object_set(G_OBJECT(m_rtpPay),"pt",m_codecType, NULL);
						g_object_set(G_OBJECT(m_encoder), "audio-type", 2048, NULL);
						g_object_set(G_OBJECT(m_encoder), "inband-fec", true, NULL);
						g_object_set(G_OBJECT(m_encoder), "bandwidth", 1103, NULL);
						g_object_set(G_OBJECT(m_encoder),"bitrate",16000,NULL);
						caps = gst_caps_from_string("audio/x-raw, channels=(int)1, rate=(int)48000");
						break;
				 }
			default:
				 {
						XGLOG_ERROR( "create_encode_elements failed for Unknown/Unsupported voice send bin !!");
						return FALSE;
				 }
	 }

	 if(m_capsFilter)
			g_object_set(G_OBJECT(m_capsFilter), "caps",caps, NULL);
	 gst_caps_unref(caps);

   m_id5 = gst_element_factory_make("identity",NULL);
   m_id6 = gst_element_factory_make("identity",NULL);
   m_id7 = gst_element_factory_make("identity",NULL);

   if(m_id5)
   g_object_set(G_OBJECT(m_id5),"signal-handoffs",FALSE,NULL);

   if(m_id6)
   g_object_set(G_OBJECT(m_id6),"signal-handoffs",FALSE,NULL);

   if(m_id7)
   g_object_set(G_OBJECT(m_id7),"signal-handoffs",FALSE,NULL);

	 return TRUE;
}

gboolean HmpVoiceSendEncoderBin::set_handoff()
{
  if(m_id5)
  g_object_set(G_OBJECT(m_id5),"signal-handoffs",TRUE,NULL);
  if(m_id6)
  g_object_set(G_OBJECT(m_id6),"signal-handoffs",TRUE,NULL);
  if(m_id7)
  g_object_set(G_OBJECT(m_id7),"signal-handoffs",TRUE,NULL);
  return TRUE;
}

gboolean HmpVoiceSendEncoderBin::add_and_link_elements()
{
	 GstPad *pad = NULL;
	 XGLOG_INFO( "HmpVoiceSendEncoderBin::add_and_link_elements start");
	 if(!m_encoder || !m_rtpPay) {
			XGLOG_ERROR( "add_and_link_elements failed. elements not creaed properly for G711_SEND bin !!");
			return FALSE;
	 } else {
			XGLOG_INFO( "add_and_link_elements for G711_SEND bin");
			if(m_ttsType == EN_XGATE_TTS_INHOUSE){
				 if(m_capsFilter){
						gst_bin_add_many(m_bin, m_capsFilter,m_encoder, m_rtpPay, m_udpSend, NULL);
						gst_element_link_many(m_capsFilter,m_encoder, m_rtpPay, m_udpSend, NULL);
				 }
			} else{
				 gst_bin_add_many(m_bin, m_id5, m_encoder, m_id6, m_rtpPay, m_id7, m_udpSend, NULL);
				 gst_element_link_many(m_id5, m_encoder, m_id6, m_rtpPay, m_id7, m_udpSend, NULL);
			}
	 }

	 if(m_ttsType == EN_XGATE_TTS_INHOUSE){
			pad = gst_element_get_static_pad (m_capsFilter, "sink");
			if(pad) {
				 m_ghostRtpSinkPad = gst_ghost_pad_new("sink", pad);
				 gst_element_add_pad (GST_ELEMENT(m_bin), m_ghostRtpSinkPad);
				 gst_pad_set_active(m_ghostRtpSinkPad, TRUE);
				 gst_object_unref (GST_OBJECT (pad));
			}
	 }else{
			//pad = gst_element_get_static_pad (m_encoder, "sink");
			pad = gst_element_get_static_pad (m_id5, "sink");
			if(pad) {
				 m_ghostRtpSinkPad = gst_ghost_pad_new("sink", pad);
				 gst_element_add_pad (GST_ELEMENT(m_bin), m_ghostRtpSinkPad);
				 gst_pad_set_active(m_ghostRtpSinkPad, TRUE);
				 gst_object_unref (GST_OBJECT (pad));
			}
	 }

	 return TRUE;
}

gboolean HmpVoiceSendEncoderBin::register_identity_callback(gpointer data)
{
  Client *client=(Client *)data;
  map<GstElement *,struct st>m_IdMap;

  if(m_id5){
  client->m_idset.p="First identity element of the VoiceSendEncoderBin";
  client->m_IdMap.insert(pair<GstElement *,struct st>(m_id5,client->m_idset));
  g_signal_connect(G_OBJECT(m_id5), "handoff", G_CALLBACK(&identity_callback), data);
  }

  if(m_id6){
  client->m_idset.p="Identity element between m_encoder and m_rtpPay";
  client->m_IdMap.insert(pair<GstElement *,struct st>(m_id6,client->m_idset));
  g_signal_connect(G_OBJECT(m_id6), "handoff", G_CALLBACK(&identity_callback), data);
  }

  if(m_id7){
  client->m_idset.p="Identity element between m_rtpPay and m_udpsend";
  client->m_IdMap.insert(pair<GstElement *,struct st>(m_id7,client->m_idset));
  g_signal_connect(G_OBJECT(m_id7), "handoff", G_CALLBACK(&identity_callback), data);
  }

  return TRUE;
}

void HmpVoiceSendEncoderBin::identity_callback(GstElement * identity,GstBuffer * buffer,gpointer data)
{
   Client *client=(Client *)data;
   if(!data)
     return;

   map<GstElement *,struct st>::iterator itr;

   itr=client->m_IdMap.find(identity);
   if(itr != client->m_IdMap.end())
   {
     itr->second.c++;
     if((itr->second.c==1)||(itr->second.c%100==0))
     XGLOG_INFO("HmpVoiceSendEncoderBin::identity_callback count:%u, %s-->callId:%s",itr->second.c, itr->second.p,client->m_detail.m_callId.c_str());
   }
     return;
}

gboolean HmpVoiceSendEncoderBin::set_ptime(gint ptime, gint maxPtime)
{
	 if(m_rtpPay) {
			gint64 min_ptime, max_ptime = 0;
			if(ptime > 100) {
				 XGLOG_ERROR( "HmpVoiceSendEncoderBin::set_ptime, got invalide ptime: %d value so setting default ptime: 20 !!", ptime);
				 //ptime = 20;
				 max_ptime = 20*1000000;
				 min_ptime = 20*1000000;
			}
			else
			{
				 if (ptime > 0){
						max_ptime = maxPtime*1000000;
						min_ptime = ptime*1000000;
				 } else {
						max_ptime = 20*1000000;
						min_ptime = 20*1000000;
				 }

			}
			g_object_set(G_OBJECT(m_rtpPay),"min-ptime", min_ptime, "max-ptime", max_ptime, NULL);
			return TRUE;
	 }
	 return FALSE;
}
/* HmpVoiceSendEncoderBin derived class - End
 */


/* HmpVoiceRePacketizeChannel derived class - Start
 */
HmpVoiceRePacketizeChannel::HmpVoiceRePacketizeChannel(string pipelineName, HmpChannelType pipelineType) :
	 HmpChannel(pipelineName, pipelineType) , m_pHmpVoiceRePacketizeBin(NULL)
{
}

HmpVoiceRePacketizeChannel::~HmpVoiceRePacketizeChannel()
{
	 XGLOG_INFO( "HmpVoiceRePacketizeChannel::~HmpVoiceRePacketizeChannel destruction start");
	 if(m_pHmpVoiceRePacketizeBin) {
			XGLOG_INFO("BFR deleting the VoiceRePacketizeBin in the HmpVoiceRePacketizeChannel");
			delete m_pHmpVoiceRePacketizeBin;
			m_pHmpVoiceRePacketizeBin = NULL;
			XGLOG_INFO("AFR deleting the VoiceRePacketizeBin in the HmpVoiceRePacketizeChannel");
	 }
}


gboolean HmpVoiceRePacketizeChannel::add_client_addr(string clientIp,gint port,gboolean addExistingClients){
	 if(m_pHmpVoiceRePacketizeBin){
			m_pHmpVoiceRePacketizeBin->add_client_addr(clientIp,port,addExistingClients);
			return TRUE;
	 }
	 return FALSE;
}
gboolean HmpVoiceRePacketizeChannel::clear_client_addr()
{
	 if(m_pHmpVoiceRePacketizeBin){
			m_pHmpVoiceRePacketizeBin->clear_client_addr();  
	 } 
}


gboolean HmpVoiceRePacketizeChannel::set_ptime(gint min_ptime,gint max_ptime){
	 if(m_pHmpVoiceRePacketizeBin) {
			return m_pHmpVoiceRePacketizeBin->set_ptime(min_ptime, max_ptime);
	 }
	 return FALSE;  
}

gboolean HmpVoiceRePacketizeChannel::construct_bin(string binName, HmpBinType binType,HmpCodecType codecType, gint audioPort, GSocket *audioRecvSendSock)
{
	 XGLOG_INFO( "HmpVoiceRecvPacketizeChannel::construct_bin %s start", (char*)ur_log_string (binName));
	 if(!m_pHmpVoiceRePacketizeBin) {
			m_pHmpVoiceRePacketizeBin = new HmpVoiceRePacketizeBin(binName, binType, codecType, audioPort, audioRecvSendSock); 
			if(!m_pHmpVoiceRePacketizeBin->construct()) {
				 XGLOG_ERROR( "construct_bin, VoiceRePacketizeBin construction failed !!");
				 return FALSE;
			} else {
				 gst_bin_add_many(GST_BIN(m_pipeline), GST_ELEMENT(m_pHmpVoiceRePacketizeBin->m_bin), NULL); 
			}
	 }  
	 return TRUE;
} 
/*  HmpVoiceRePacketizeChannel derived class - End
 */

/* HmpVoiceRePacketizeBin derived class - Start
 */
HmpVoiceRePacketizeBin::HmpVoiceRePacketizeBin(string binName,HmpBinType binType,HmpCodecType codecType, gint recvPort, GSocket *recvSock):
	 HmpBin(binName,binType),m_codecType(codecType),m_rtpDepay(NULL),m_rtpPay(NULL),m_rtpJitterBuffer(NULL)
{
	 m_recvSocket = recvSock;
	 m_recvPort = recvPort;
	 m_sendSocket = recvSock;
}

HmpVoiceRePacketizeBin::~HmpVoiceRePacketizeBin()
{
	 if(m_recvSocket){
			XGLOG_INFO( "HmpVoiceRePacketizeBin destructing send socket: %d socket", m_sendSocket);
			g_socket_close(m_sendSocket, NULL);
			g_object_unref(G_OBJECT(m_sendSocket));
			m_recvSocket = NULL;
			/* Same socket is used for m_sendSocket and m_recvSocket to send and receive the stream, set other variable also to NULL */
			m_sendSocket = NULL;
	 }
}

gboolean HmpVoiceRePacketizeBin::set_ptime(gint ptime,gint maxPtime)
{
	 if(m_rtpPay) {
			gint64 min_ptime, max_ptime = 0;
			if(ptime > 100) {
				 XGLOG_ERROR( "HmpEncoderBin::set_ptime, got invalide ptime: %d value so setting default ptime: 20 !!", ptime);
				 //ptime = 20;
				 max_ptime = 20*1000000;
				 min_ptime = 20*1000000;
			}
			else
			{
				 if (ptime > 0){
						max_ptime = maxPtime*1000000;
						min_ptime = ptime*1000000;
				 } else {
						max_ptime = 20*1000000;
						min_ptime = 20*1000000;
				 }

			}
			g_object_set(G_OBJECT(m_rtpPay),"min-ptime", min_ptime, "max-ptime", max_ptime, NULL);
			return TRUE;
	 }
}
gboolean HmpVoiceRePacketizeBin::construct()
{
	 if(!create_udpsrc()) {
			XGLOG_ERROR( "construct HmpVoiceRePacketizeBin, creating udpsrc failed !!");
			return FALSE;
	 }

	 if(!create_udpsink()) {
			XGLOG_ERROR( "construct VoiceRecvBin, creating udpsink failed !!");
			return FALSE;
	 }

	 if(!create_elements()) {
			XGLOG_ERROR( "construct HmpVoiceRePacketizeBin, creating elements failed !!");
			return FALSE;
	 }
	 if(!add_and_link_elements()) {
			XGLOG_ERROR( "construct HmpVoiceRePacketizeBin, add and linking elements failed !!");
			return FALSE;
	 }
	 return TRUE; 
}

gboolean HmpVoiceRePacketizeBin::create_elements()
{
	 guint latency = 40;
	 m_rtpJitterBuffer = gst_element_factory_make("rtpjitterbuffer",NULL);
	 switch(m_codecType){
			case EN_CODEC_PCMA:
				 {
						m_rtpDepay = gst_element_factory_make("rtppcmadepay",NULL); 
						m_rtpPay = gst_element_factory_make("rtppcmapay",NULL);
						break;
				 }
			case EN_CODEC_PCMU:
				 {
						m_rtpDepay = gst_element_factory_make("rtppcmudepay",NULL);
						m_rtpPay = gst_element_factory_make("rtppcmupay",NULL);
						break;
				 }
			case EN_CODEC_G722:
				 {
						m_rtpDepay = gst_element_factory_make("rtpg722depay",NULL);
						m_rtpPay = gst_element_factory_make("rtpg722pay",NULL);
						g_object_set(G_OBJECT(m_rtpPay),"pt", 9, NULL);
						break;
				 }
			case EN_CODEC_G729:
				 {
						m_rtpDepay = gst_element_factory_make("rtpg729depay",NULL);
						m_rtpPay = gst_element_factory_make("rtpg729pay",NULL);
						break;
				 }
			case 96 ... 127:
				 {
						m_rtpDepay = gst_element_factory_make("rtpopusdepay",NULL);
						m_rtpPay = gst_element_factory_make("rtpopuspay",NULL);
						g_object_set(G_OBJECT(m_rtpPay),"pt",m_codecType, NULL);
						break;
				 }
			default:
				 {
						XGLOG_ERROR("Invalid codec found in repacketize bin"); 
						return FALSE;
				 }
	 }

	 if(m_rtpJitterBuffer)
	 {
			g_object_set(G_OBJECT(m_rtpJitterBuffer), "latency", latency, NULL);
	 }
	 return TRUE;
}

gboolean HmpVoiceRePacketizeBin::add_and_link_elements()
{
	 if(!m_bin || !m_udpRecv || !m_rtpDepay || !m_rtpPay || !m_rtpJitterBuffer || !m_udpSend) {
			XGLOG_ERROR( "add_and_link_elements failed in VoiceRePacketizeBin !!");
			return FALSE;
	 }

	 gst_bin_add_many(m_bin, m_udpRecv,m_rtpJitterBuffer, m_rtpDepay, m_rtpPay,m_udpSend,NULL);
	 gst_element_link_many(m_udpRecv, m_rtpJitterBuffer, m_rtpDepay, m_rtpPay, m_udpSend,NULL); 
	 return TRUE;
}
/* HmpVoiceRePacketizeBin derived class - End */




/* HmpVoiceRelayChannel derived class - Start*/
HmpVoiceRelayChannel::HmpVoiceRelayChannel(string pipelineName, HmpChannelType pipelineType) :
	 HmpChannel(pipelineName, pipelineType), m_pHmpVoiceRelayBin(NULL)
{
}

HmpVoiceRelayChannel::~HmpVoiceRelayChannel()
{
	 XGLOG_INFO( "HmpVoiceRelayChannel::~HmpVoiceRelayChannel destruction start");
	 if(m_pHmpVoiceRelayBin) {
			XGLOG_INFO( "BFR deleting the VoiceRelayBin in the HmpVoiceRelayChannel");
			delete m_pHmpVoiceRelayBin;
			m_pHmpVoiceRelayBin = NULL;
			XGLOG_INFO( "AFR deleting the VoiceRelayBin in the HmpVoiceRelayChannel");
	 }
}


gboolean HmpVoiceRelayChannel::add_client_addr(string clientIp,gint port,gboolean addExistingClients){
	 if(m_pHmpVoiceRelayBin){
			m_pHmpVoiceRelayBin->add_client_addr(clientIp,port,addExistingClients);
			return TRUE;
	 }

	 return FALSE;
}
gboolean HmpVoiceRelayChannel::clear_client_addr()
{
	 if(m_pHmpVoiceRelayBin){
			m_pHmpVoiceRelayBin->clear_client_addr();  
	 }

}

gboolean HmpVoiceRelayChannel::construct_bin(string binName, HmpBinType binType, gint audioPort, GSocket *audioRecvSendSock)
{
	 XGLOG_INFO("HmpVoiceRelayChannel::construct_bin %s start", (char*)ur_log_string (binName));
	 if(!m_pHmpVoiceRelayBin) {
			m_pHmpVoiceRelayBin = new HmpVoiceRelayBin(binName, binType, audioPort, audioRecvSendSock); 
			if(!m_pHmpVoiceRelayBin->construct()) {
				 XGLOG_ERROR( "construct_bin, VoiceRelayBin construction failed !!");
				 return FALSE;
			} else {
				 gst_bin_add_many(GST_BIN(m_pipeline), GST_ELEMENT(m_pHmpVoiceRelayBin->m_bin), NULL); 
			}
	 }

	 return TRUE;
} 
/*  HmpVoiceRelayChannel derived class - End
 *

/* HmpVoiceRelayBin derived class - Start
 */
HmpVoiceRelayBin::HmpVoiceRelayBin(string binName,HmpBinType binType, gint recvPort, GSocket *recvSock):
	 HmpBin(binName,binType)
{
	 m_recvSocket = recvSock;
	 m_recvPort = recvPort;
	 m_sendSocket = recvSock;
}

HmpVoiceRelayBin::~HmpVoiceRelayBin()
{
	 if(m_recvSocket){
			XGLOG_INFO( "HmpVoiceRelayBin destructing send socket: %d socket", m_sendSocket);
			g_socket_close(m_sendSocket, NULL);
			g_object_unref(G_OBJECT(m_sendSocket));
			m_recvSocket = NULL;
			/* Same socket is used for m_sendSocket and m_recvSocket to send and receive the stream, set other variable also to NULL */
			m_sendSocket = NULL;
			m_recvPort = 0;
	 }
}

gboolean HmpVoiceRelayBin::construct()
{
	 if(!create_udpsrc()) {
			XGLOG_ERROR( "construct HmpVoiceRelayBin, creating udpsrc failed !!");
			return FALSE;
	 }

	 if(!create_udpsink()) {
			XGLOG_ERROR( "construct VoiceRecvBin, creating udpsink failed !!");
			return FALSE;
	 }

	 if(!add_and_link_elements()) {
			XGLOG_ERROR( "construct HmpVoiceRelayBin, add and linking elements failed !!");
			return FALSE;
	 }
	 return TRUE; 
}

gboolean HmpVoiceRelayBin::add_and_link_elements()
{
	 if(!m_bin || !m_udpRecv  || !m_udpSend) {
			XGLOG_ERROR( "add_and_link_elements failed in HmpVoiceRelayBin !!");
			return FALSE;
	 }

	 gst_bin_add_many(m_bin, m_udpRecv, m_udpSend,NULL);
	 gst_element_link_many(m_udpRecv, m_udpSend,NULL); 
	 return TRUE;
}
/* HmpVoiceRelayBin derived class - End */


/* HmpInhouseStreamingChannel derived class - Start
 */

HmpInhouseStreamingChannel::HmpInhouseStreamingChannel(string pipelineName, HmpChannelType pipelineType) :
	 HmpChannel(pipelineName, pipelineType) , m_pHmpInhouseStreamingBin(NULL)
{
}

HmpInhouseStreamingChannel::~HmpInhouseStreamingChannel()
{
	 XGLOG_INFO( "HmpInhouseStreamingChannel::~HmpInhouseStreamingChannel destruction start");
	 if(m_pHmpInhouseStreamingBin) {
			delete m_pHmpInhouseStreamingBin;
			m_pHmpInhouseStreamingBin = NULL;
	 }
}

gboolean HmpInhouseStreamingChannel::construct_bin(string binName, HmpBinType binType,HmpCodecType codecType, string codecName)
{
	 XGLOG_INFO( "HmpInhouseStreamingChannel::construct_bin %s start", (char*)ur_log_string (binName));

	 if(!m_pHmpInhouseStreamingBin){
			m_pHmpInhouseStreamingBin = new HmpInhouseStreamingBin(binName, binType, codecType, codecName);

			if(!m_pHmpInhouseStreamingBin->construct()) {
				 XGLOG_ERROR("Construct_bin, HmpInhouseStreamingBin construction failed !!");
				 return FALSE;
			} else {
				 gst_bin_add_many(GST_BIN(m_pipeline), GST_ELEMENT(m_pHmpInhouseStreamingBin->m_bin), NULL);
			}
	 }
	 return TRUE;
}

gboolean HmpInhouseStreamingChannel::add_client_addr(string clientIp,gint port,gboolean addExistingClients){
	 if(m_pHmpInhouseStreamingBin){
			m_pHmpInhouseStreamingBin->add_client_addr(clientIp,port,addExistingClients);
			return TRUE;
	 }
	 return FALSE;
}

gboolean HmpInhouseStreamingChannel::set_ptime(gint ptime, gint maxPtime)
{
	 if(m_pHmpInhouseStreamingBin) {
			return m_pHmpInhouseStreamingBin->set_ptime(ptime, maxPtime);
	 }
	 return FALSE;
}



/*  HmpInhouseStreamingChannel derived class - End
 */

/* HmpInhouseStreamingBin derived class - Start
 */

HmpInhouseStreamingBin::HmpInhouseStreamingBin(string binName,HmpBinType binType,HmpCodecType codecType, string codecName):
	 HmpBin(binName, binType),m_codecType(codecType),m_codecName(codecName), m_fdSrc(NULL),m_audioParse(NULL),m_removeSilence(NULL),m_audioConvert(NULL),m_audioResample(NULL),m_payEnc(NULL),m_rtpPay(NULL),m_capsfilter(NULL),m_audioRate(NULL)
{
}

HmpInhouseStreamingBin::~HmpInhouseStreamingBin()
{
}

gboolean HmpInhouseStreamingBin::construct()
{
	 while(!m_sendSocket) {
			m_sendPort = HOSTGST_GET_LOOPBACK_PORT();
			m_sendSocket = xGateHmpGstUtil::create_udpsocket(m_sendPort);
	 }

	 if(!create_udpsink()) {
			XGLOG_ERROR( "construct PlayAudioBin, creating udpsink failed !!");
			return FALSE;
	 }

	 if(!create_decode_elements()) {
			XGLOG_ERROR( "construct HmpInhouseStreamingBin, creating elements failed !!");
			return FALSE;
	 }

	 if(!add_and_link_elements()) {
			XGLOG_ERROR( "construct HmpInhouseStreamingBin, add and linking elements failed !!");
			return FALSE;
	 }
	 return TRUE;
}

gboolean HmpInhouseStreamingBin::create_decode_elements()
{
	 guint quality = 8;
	 guint codec_g729 = 0;
	 GstCaps *cp;
	 switch(m_binType) {
			case EN_BIN_G711U_RECV:
				 {
						XGLOG_INFO( "create_elements for G711U_RECV bin");
						m_payEnc = gst_element_factory_make("rtppcmuay", NULL);
						m_rtpPay = gst_element_factory_make("mulawenc", NULL);
						break;
				 }
			case EN_BIN_G711A_RECV:
				 {
						XGLOG_INFO( "create_elements for G711U_RECV bin");
						m_payEnc = gst_element_factory_make("alawenc", NULL);
						m_rtpPay = gst_element_factory_make("rtppcmapay",NULL);
						break;
				 }
			case EN_BIN_G722_RECV:
				 {
						XGLOG_INFO( "create_elements for G722_RECV bin");
						m_payEnc = gst_element_factory_make("avenc_g722", NULL);
						m_rtpPay = gst_element_factory_make("rtpg722pay", NULL);
						break;
				 }
			case EN_BIN_G729_RECV:
				 {
						XGLOG_INFO( "create_decode_elements for G729_RECV bin");
						m_payEnc = gst_element_factory_make("g729enc", NULL);
						m_rtpPay = gst_element_factory_make("rtpg729pay",NULL);
						codec_g729 = 1;
						break;
				 }
			case EN_BIN_OPUS_RECV:
				 {
						XGLOG_INFO( "create_decode_elements for OPUS_RECV bin");
						m_payEnc = gst_element_factory_make("opusenc", NULL);
						m_rtpPay = gst_element_factory_make("rtpopuspay", NULL);
						g_object_set(G_OBJECT(m_payEnc), "bitrate-type",1, NULL);
						g_object_set(G_OBJECT(m_payEnc), "audio-type", 2048, NULL);
						g_object_set(G_OBJECT(m_payEnc), "inband-fec", true, NULL);
						g_object_set(G_OBJECT(m_payEnc), "bandwidth",-1000, NULL);
						g_object_set(G_OBJECT(m_rtpPay),"pt",m_codecType,NULL);
						g_object_set(G_OBJECT(m_payEnc),"bitrate",START_BANDWIDTH,NULL);
						g_object_set(G_OBJECT(m_payEnc),"perfect-timestamp",TRUE,NULL);
						break;
				 }
			default:
				 {
						XGLOG_ERROR( "create_decode_elements failed for Unknown/Unsupported voice recv bin !!");
						return FALSE;
				 }
	 }

	 m_fdSrc = gst_element_factory_make("fdsrc", NULL);
	 g_object_set (G_OBJECT(m_fdSrc), "blocksize",64000, NULL);
	 g_object_set (G_OBJECT(m_fdSrc), "timeout",5000,NULL);
	 m_audioParse=gst_element_factory_make("rawaudioparse",NULL);
	 g_object_set(G_OBJECT(m_audioParse), "pcm-format",4, NULL);
	 g_object_set(G_OBJECT(m_audioParse), "format",0, NULL);
	 g_object_set(G_OBJECT(m_audioParse), "sample-rate",22050, NULL);
	 g_object_set(G_OBJECT(m_audioParse), "num-channels",1, NULL);

	 GstCaps *caps = NULL;  
	 m_capsfilter=gst_element_factory_make("capsfilter",NULL);
	 m_audioRate=gst_element_factory_make("audiorate",NULL);
	 if(m_capsfilter){
			caps = gst_caps_from_string("audio/x-raw, channels=(int)1, rate=(int)8000");
			g_object_set(G_OBJECT(m_capsfilter), "caps",caps, NULL);
			gst_caps_unref(caps);
	 }

	 m_removeSilence=gst_element_factory_make("removesilence",NULL);
	 g_object_set(G_OBJECT(m_removeSilence), "silent",FALSE, NULL);
	 g_object_set(G_OBJECT(m_removeSilence), "threshold",-10, NULL);


	 if((xGateHmpGstUtil::getenabledecconverter() && !codec_g729)|| (xGateHmpGstUtil::getenableg729decconverter() && codec_g729)){
			m_audioConvert = gst_element_factory_make("audioconvert", NULL);
	 }

	 m_audioResample = gst_element_factory_make("audioresample", NULL);
	 g_object_set (G_OBJECT(m_audioResample), "quality", quality, NULL);

	 return TRUE;
}


gboolean HmpInhouseStreamingBin::add_and_link_elements()
{
	 if(!m_bin || !m_fdSrc || !m_audioParse || !m_removeSilence || !m_audioResample || !m_audioConvert || !m_payEnc || !m_rtpPay || !m_udpSend || !m_capsfilter) {
			XGLOG_ERROR( "add_and_link_elements failed in HmpInhouseStreamingBin !!");
			return FALSE;
	 }

	 gst_bin_add_many(m_bin,m_fdSrc,m_audioParse,m_audioRate,m_audioConvert,m_audioResample,m_capsfilter, m_removeSilence, m_payEnc, m_rtpPay, m_udpSend,NULL);


	 if(!gst_element_link_many(m_fdSrc,m_audioParse,m_audioRate,m_audioResample, m_capsfilter,m_payEnc, m_rtpPay, m_udpSend,NULL)){
			XGLOG_ERROR( "HmpInhouseStreamingBin->add_and_link_elements: failed to link elements!!\n");
			return FALSE;
	 }

	 XGLOG_INFO("HmpInhouseStreamingBin:linked element sucessfully");

	 return TRUE;
}

gboolean HmpInhouseStreamingBin::set_ptime(gint ptime, gint maxPtime)
{

	 if(m_rtpPay) {
			gint64 min_ptime, max_ptime = 0;
			if(ptime > 100) {
				 XGLOG_ERROR( "HmpPlayAudioBin::set_ptime, got invalide ptime: %d value so setting default ptime: 20 !!", ptime);
				 min_ptime = 20*1000000;
				 max_ptime = 20*1000000;
			}
			else
			{
				 if (ptime > 0){
						max_ptime = maxPtime*1000000;
						min_ptime = ptime*1000000;
				 } else {
						max_ptime = 20*1000000;
						min_ptime = 20*1000000;
				 }
#if 1
				 if (m_codecType == EN_CODEC_OPUS) {
						min_ptime = 20*1000000;
						max_ptime = 20*1000000;
				 }
#endif
			}
			g_object_set(G_OBJECT(m_rtpPay),"min-ptime", min_ptime, "max-ptime", max_ptime, NULL);
			return TRUE;
	 }
	 return FALSE;
}

/* HmpInhouseStreamingBin derived class - End */


HmpVoiceTransceiverBin::HmpVoiceTransceiverBin(string binName,HmpBinType binType, xGateASRType asrType, gint recvPort, GSocket *recvSock):
	 HmpBin(binName, binType),m_capSet(NULL), m_rtpDepay(NULL),m_rtpBin(NULL),m_reSampler(NULL),
	 m_audioDecoder(NULL), m_fdSink(NULL),m_fakeSink(NULL),m_asrType(asrType),m_rtpjitterbuffer(NULL),m_id(NULL),m_audioSplit(NULL),m_audioConvert(NULL)
{
	 m_recvSocket = recvSock;
	 m_recvPort = recvPort;
}

HmpVoiceTransceiverBin::~HmpVoiceTransceiverBin()
{
	 if(m_recvSocket){
			XGLOG_INFO( "HmpVoiceTransceiverBin destructing send socket: %d socket", m_recvSocket);
			g_socket_close(m_recvSocket, NULL);
			g_object_unref(G_OBJECT(m_recvSocket));
			m_recvSocket = NULL;
			m_recvPort = 0;
	 }

}

void HmpVoiceTransceiverBin::identity_callback(GstElement * identity,GstBuffer * buffer,gpointer data)
{
	 static int count = 0;
	 count ++;
	 cout<<"The identity element is:"<<count<<endl;
}

gboolean HmpVoiceTransceiverBin::construct()
{
	 if(!create_udpsrc()) {
			XGLOG_ERROR( "construct HmpVoiceTransceiverBin, creating udpsrc failed !!");
			return FALSE;
	 }
	 if(!create_decode_elements()) {
			XGLOG_ERROR( "construct HmpVoiceTransceiverBin, creating elements failed !!");
			return FALSE;
	 }

	 if(!add_and_link_elements()) {
			XGLOG_ERROR( "construct HmpVoiceTransceiverBin, add and linking elements failed !!");
			return FALSE;
	 }
	 return TRUE;
}

gboolean HmpVoiceTransceiverBin::create_decode_elements()
{
	 guint latency = 200;
	 guint codec_g729 = 0;
	 GstCaps *audioCaps;
	 switch(m_binType) {
			case EN_BIN_G711U_RECV:
				 {
						XGLOG_INFO( "create_decode_elements for G711U_RECV bin");
						m_rtpDepay = gst_element_factory_make("rtppcmudepay", NULL);
						m_audioDecoder = gst_element_factory_make("mulawdec", NULL);
						break;
				 }
			case EN_BIN_G711A_RECV:
				 {
						XGLOG_INFO( "create_decode_elements for G711A_RECV bin");
						m_rtpDepay = gst_element_factory_make("rtppcmadepay", NULL);
						m_audioDecoder = gst_element_factory_make("alawdec", NULL);
						break;
				 }
			case EN_BIN_G722_RECV:
				 {
						XGLOG_INFO( "create_decode_elements for G722_RECV bin");
						m_rtpDepay = gst_element_factory_make("rtpg722depay", NULL);
						m_audioDecoder = gst_element_factory_make("avdec_g722", NULL);
						break;
				 }
			case EN_BIN_G729_RECV:
				 {
						XGLOG_INFO( "create_decode_elements for G729_RECV bin");
						m_rtpDepay = gst_element_factory_make("rtpg729depay", NULL);
						m_audioDecoder = gst_element_factory_make("avdec_g729", NULL);
						codec_g729 = 1;
						break;
				 }
			case EN_BIN_OPUS_RECV:
				 {
						XGLOG_INFO( "create_decode_elements for OPUS_RECV bin");
						m_rtpDepay = gst_element_factory_make("rtpopusdepay", NULL);
						m_audioDecoder = gst_element_factory_make("opusdec", NULL);
						break;
				 }
			default:
				 {
						XGLOG_ERROR( "create_decode_elements failed for Unknown/Unsupported voice recv bin !!");
						return FALSE;
				 }
	 }

	 m_rtpBin = gst_element_factory_make("rtpbin", NULL);
	 //g_object_set(G_OBJECT(m_rtpBin),"autoremove", TRUE, NULL);
	 g_object_set(G_OBJECT(m_rtpBin), "latency", latency, NULL);


	 m_rtpjitterbuffer = gst_element_factory_make("rtpjitterbuffer",NULL);
	 g_object_set(G_OBJECT(m_rtpjitterbuffer), "latency",latency,NULL);


	 m_fakeSink = gst_element_factory_make("fakesink", NULL);

	 m_reSampler = gst_element_factory_make("audioresample", NULL);
	 g_object_set (G_OBJECT(m_reSampler), "quality",8, NULL);

	 m_audioSplit = gst_element_factory_make("audiobuffersplit", NULL);
	 g_object_set(G_OBJECT(m_audioSplit),"strict-buffer-size",TRUE,NULL); 

	 m_capSet = gst_element_factory_make("capsfilter", NULL);

	 m_fdSink = gst_element_factory_make("appsink",NULL);

	 if(m_asrType == EN_XGATE_ASR_INHOUSE){
      m_audioConvert = gst_element_factory_make("audioconvert", NULL);
			audioCaps = gst_caps_from_string("audio/x-raw,channels=(int)1, rate=(int)16000");
	 }else{
			audioCaps = gst_caps_from_string("audio/x-raw,channels=(int)1, rate=(int)8000");
	 }

	 if(m_capSet){
			g_object_set(G_OBJECT(m_capSet), "caps", audioCaps, NULL);
	 }
	 gst_caps_unref(audioCaps);

	 return TRUE;
}


gboolean HmpVoiceTransceiverBin::register_rtpbin_callback(gpointer data)
{
	 if(m_rtpBin) {
			g_signal_connect(G_OBJECT(m_rtpBin), "pad-added", G_CALLBACK(&on_pad_added), data);
			g_signal_connect(G_OBJECT(m_rtpBin), "request-pt-map", G_CALLBACK(&on_request_pt_map), data);
			return TRUE;
	 }
	 return FALSE;
}
void HmpVoiceTransceiverBin::on_pad_added(GstElement *rtpBin, GstPad *pad, gpointer data)
{
	 gchar *padName = NULL;
	 GstPadDirection padDir = GST_PAD_UNKNOWN;
	 GstPad *sinkPad, *peerPad = NULL;
	 GstPadLinkReturn isPadLinked;
	 unsigned int sn, ssrc = 0;
	 gint pt = -1;
	 unsigned int codec = 0;

	 padName = gst_pad_get_name(pad);
	 XGLOG_INFO( "HmpVoiceTransceiverBin::on_pad_added triggered for pad: %s", (char*)ur_log_string (padName));
	 sscanf(padName, "recv_rtp_src_%u_%u_%u", &sn, &ssrc, &pt);
	 Client *client = (Client *)data;
	 if(!client){
			XGLOG_ERROR("HmpVoiceTransceiverBin::on_pad_added, client is empty !!!");
			return;
	 }
	 unsigned int dtmf_pt = client->m_detail.audioDetail.m_dtmfpt;
	 HmpVoiceTransceiverChannel *pHmpVoiceTransceiverChannel=client->m_pHmpVoiceTransceiverChannel;
	 HmpVoiceTransceiverBin *pHmpVoiceTransceiverBin=pHmpVoiceTransceiverChannel->m_pHmpVoiceTransceiverBin;
	 padDir = gst_pad_get_direction(pad);

	 switch(padDir) {
			case GST_PAD_SRC:
				 {
						switch(pt) {
							 case EN_CODEC_PCMU:
							 case EN_CODEC_PCMA:
							 case EN_CODEC_G722:
							 case EN_CODEC_G729:
									{
										 XGLOG_INFO( "on_pad_added triggered for codec: %d packets", pt);
										 if(pHmpVoiceTransceiverBin){
												sinkPad = gst_element_get_static_pad(pHmpVoiceTransceiverBin->m_rtpDepay, "sink");
												isPadLinked = gst_pad_link(pad, sinkPad);
												if(GST_PAD_LINK_WAS_LINKED == isPadLinked) {
													 peerPad = gst_pad_get_peer(sinkPad);
													 if(gst_pad_unlink(peerPad, sinkPad)) {
															isPadLinked = gst_pad_link(pad, sinkPad);
													 }
												}
												gst_object_unref(sinkPad);
												if(isPadLinked != GST_PAD_LINK_OK) {
													 XGLOG_ERROR("on_pad_added, %s pad failed to add with rtpDepay in VoiceCaptureBin: %s!!", \
																 padName, (char*)ur_log_string(pHmpVoiceTransceiverBin->m_binName));
												}
												else{
													 XGLOG_INFO("on_pad_added, %s pad successfully added with rtpDepay in VoiceCaptureBin : %s!!", \
																 (char*)ur_log_string (padName), (char*)ur_log_string (pHmpVoiceTransceiverBin->m_binName));
													 gst_element_sync_state_with_parent(pHmpVoiceTransceiverBin->m_rtpDepay);
													 gst_element_sync_state_with_parent(pHmpVoiceTransceiverBin->m_audioDecoder);
													 gst_element_sync_state_with_parent(pHmpVoiceTransceiverBin->m_audioConvert);
													 gst_element_sync_state_with_parent(pHmpVoiceTransceiverBin->m_reSampler);
													 gst_element_sync_state_with_parent(pHmpVoiceTransceiverBin->m_capSet);
													 gst_element_sync_state_with_parent(pHmpVoiceTransceiverBin->m_audioSplit);
													 gst_element_sync_state_with_parent(pHmpVoiceTransceiverBin->m_fdSink);
												}
										 }
										 break;
									}
							 case EN_CODEC_CN:
									{
										 XGLOG_INFO( "on_pad_added triggered for CN 'comfort noise' packets");
										 sinkPad = gst_element_get_static_pad(pHmpVoiceTransceiverBin->m_fakeSink, "sink");
										 isPadLinked = gst_pad_link(pad, sinkPad);
										 if(GST_PAD_LINK_WAS_LINKED == isPadLinked) {
												peerPad = gst_pad_get_peer(sinkPad);
												if(gst_pad_unlink(peerPad, sinkPad)) {
													 isPadLinked = gst_pad_link(pad, sinkPad);
												}
										 }
										 gst_object_unref(sinkPad);
										 if(isPadLinked != GST_PAD_LINK_OK) {
												XGLOG_ERROR("on_pad_added, %s pad failed to add with rtpDepay in VoiceCaptureBin: %s!!", \
															padName, (char*)ur_log_string(pHmpVoiceTransceiverBin->m_binName));
										 }
										 else{
												gst_element_sync_state_with_parent(pHmpVoiceTransceiverBin->m_fakeSink);
										 }
										 break;
									}
							 case 96 ... 127:
									{
										 XGLOG_INFO( "on_pad_added triggered for codec: %d packets", pt);
										 if(pt == dtmf_pt){
												XGLOG_ERROR("on_pad_added triggered for codec:%d equal dtmf-pt:%d",pt,dtmf_pt);
										 }
										 if(pHmpVoiceTransceiverBin){
												sinkPad = gst_element_get_static_pad(pHmpVoiceTransceiverBin->m_rtpDepay, "sink");
												isPadLinked = gst_pad_link(pad, sinkPad);
												if(GST_PAD_LINK_WAS_LINKED == isPadLinked) {
													 peerPad = gst_pad_get_peer(sinkPad);
													 if(gst_pad_unlink(peerPad, sinkPad)) {
															isPadLinked = gst_pad_link(pad, sinkPad);
													 }
												}
												gst_object_unref(sinkPad);
												if(isPadLinked != GST_PAD_LINK_OK) {
													 XGLOG_ERROR("n_pad_added, %s pad failed to add with rtpDepay in VoiceCaptureBin: %s!!", \
																 padName, (char*)ur_log_string(pHmpVoiceTransceiverBin->m_binName));
												}
												else{
													 XGLOG_INFO("on_pad_added, %s pad successfully added with rtpDepay in VoiceCaptureBin : %s!!", \
																 (char*)ur_log_string (padName), (char*)ur_log_string (pHmpVoiceTransceiverBin->m_binName));
													 gst_element_sync_state_with_parent(pHmpVoiceTransceiverBin->m_rtpDepay);
													 gst_element_sync_state_with_parent(pHmpVoiceTransceiverBin->m_audioDecoder);
													 gst_element_sync_state_with_parent(pHmpVoiceTransceiverBin->m_audioConvert);
													 gst_element_sync_state_with_parent(pHmpVoiceTransceiverBin->m_reSampler);
													 gst_element_sync_state_with_parent(pHmpVoiceTransceiverBin->m_capSet);
													 gst_element_sync_state_with_parent(pHmpVoiceTransceiverBin->m_fdSink);
												}
										 }
										 break;
									}
						}
				 }
			case GST_PAD_SINK:
				 {


				 }
			case GST_PAD_UNKNOWN:
				 {

				 }

	 }
	 g_free(padName);
	 pHmpVoiceTransceiverBin->draw_dot_file();

}
GstCaps * HmpVoiceTransceiverBin::on_request_pt_map(GstElement *rtpBin, guint session, guint pt, gpointer data)
{
	 XGLOG_INFO( "HmpVoiceTransceiverBin::on_request_pt_map triggered for pt :%d", pt);
	 GstCaps * caps = NULL;
	 Client *client = (Client*)data;
	 unsigned int dtmf_pt = client->m_detail.audioDetail.m_dtmfpt;
	 string codecName  = client->m_detail.audioDetail.m_codecName;
	 XGLOG_INFO( "HmpVoiceTransceiverBin::on_request_pt_map triggered for pt :%d and codecName:%s", pt,(char*)ur_log_string (client->m_detail.audioDetail.m_codecName));
	 switch(pt) {
			case 0:
				 {
						caps = gst_caps_new_simple("application/x-rtp",
									"media",G_TYPE_STRING,"audio",
									"clock-rate",G_TYPE_INT,8000,
									NULL);
						break;
				 }
			case 8:
				 {
						caps = gst_caps_new_simple("application/x-rtp",
									"media",G_TYPE_STRING,"audio",
									"clock-rate",G_TYPE_INT,8000,
									NULL);
						break;
				 }
			case 9:
				 {
						caps = gst_caps_new_simple("application/x-rtp",
									"media",G_TYPE_STRING,"audio",
									"clock-rate",G_TYPE_INT,8000,
									NULL);
						break;
				 }
			case 13:
				 {
						if((strcmp(codecName.c_str(),"opus") == 0) || (strcmp(codecName.c_str(),"OPUS") == 0))
						{
							 caps = gst_caps_new_simple("application/x-rtp",
										 "media",G_TYPE_STRING,"audio",
										 "clock-rate",G_TYPE_INT,48000,
										 NULL);
						}
						else
						{
							 caps = gst_caps_new_simple("application/x-rtp",
										 "media",G_TYPE_STRING,"audio",
										 "clock-rate",G_TYPE_INT,8000,
										 NULL);

						}
						break; 
				 }
			case 96 ... 127:
				 {
						if(pt == dtmf_pt){
							 caps = gst_caps_new_simple("application/x-rtp", 
										 "media",G_TYPE_STRING,"audio",
										 "clock-rate",G_TYPE_INT,8000,
										 "encoding-name",G_TYPE_STRING, "TELEPHONE-EVENT",
										 NULL);
						}
						else
						{
							 if((strcmp(codecName.c_str(),"opus") == 0) || (strcmp(codecName.c_str(),"OPUS") == 0)){
									caps = gst_caps_new_simple("application/x-rtp",
												"media",G_TYPE_STRING,"audio",
												"clock-rate",G_TYPE_INT,48000,
												"encoding-name",G_TYPE_STRING,"X-GST-OPUS-DRAFT-SPITTKA-00",NULL);
							 }
							 else
							 {
									XGLOG_INFO("Invalid codec received in the dynamic payload");
							 }
						} 
						break;
				 }
			default:
				 {
						caps = gst_caps_new_simple("application/x-rtp",
									"media",G_TYPE_STRING,"audio",
									"clock-rate",G_TYPE_INT,8000,
									NULL);
						XGLOG_INFO( "on_request_pt_map, no caps setting for unsupported payload type: %u, pt: %u", session, pt);
						break;
				 }
	 }
	 return caps;
}
gboolean HmpVoiceTransceiverBin::add_and_link_elements()
{
	 if(!m_bin || !m_udpRecv || !m_rtpjitterbuffer || !m_rtpBin || !m_rtpDepay || !m_audioDecoder || !m_audioSplit || !m_reSampler || !m_capSet || !m_fdSink || !m_fakeSink) {
	 }

	 if(m_asrType != EN_XGATE_ASR_INHOUSE){
			XGLOG_INFO( "HmpVoiceTransceiverBin->add_and_link_elements: EN_XGATE_ASR_GOOGLE or EN_XGATE_ASR_SPEECH MATICS!\n");
			gst_bin_add_many(m_bin, m_udpRecv,m_rtpBin,m_rtpDepay, m_audioDecoder,m_reSampler,m_capSet,m_fakeSink,m_fdSink,NULL);
			if(!gst_element_link_many(m_udpRecv,m_rtpBin,NULL)){
				 XGLOG_ERROR( "HmpVoiceTransceiverBin->add_and_link_elements: failed to link elements!!\n");
				 return FALSE;
			}
			if(!gst_element_link_many(m_rtpDepay, m_audioDecoder, m_reSampler,m_capSet, m_fdSink,NULL)){
				 XGLOG_ERROR( "HmpVoiceTransceiverBin->add_and_link_elements: failed to link elements!!\n");
				 return FALSE;
			}
	 }

	 if(m_asrType == EN_XGATE_ASR_INHOUSE){

      if(!m_audioConvert){
			XGLOG_ERROR( "add_and_link_elements failed for m_audioConvert in HmpVoiceTransceiverBin !!");
			return FALSE;
      }

			switch(m_binType) {
				 XGLOG_INFO( "HmpVoiceTransceiverBin->add_and_link_elements: EN_XGATE_ASR_INHOUSE!\n");
				 case EN_BIN_G722_RECV:
				 case EN_BIN_OPUS_RECV:
				 {
						XGLOG_INFO( " EN_XGATE_ASR_INHOUSE -> EN_BIN_G722_RECV or EN_BIN_OPUS_RECV!\n");
						gst_bin_add_many(m_bin, m_udpRecv,m_rtpBin,m_rtpDepay, m_audioDecoder,m_audioConvert, m_reSampler,m_capSet,m_fakeSink,m_fdSink,NULL);
						if(!gst_element_link_many(m_udpRecv,m_rtpBin,NULL)){
							 XGLOG_ERROR( "HmpVoiceTransceiverBin->add_and_link_elements: failed to link elements!!\n");
							 return FALSE;
						}
						if(!gst_element_link_many(m_rtpDepay, m_audioDecoder,m_audioConvert, m_reSampler,m_capSet, m_fdSink,NULL)){
							 XGLOG_ERROR( "HmpVoiceTransceiverBin->add_and_link_elements: failed to link elements!!\n");
							 return FALSE;
						}
						break;
				 }
				 case EN_BIN_G711U_RECV:
				 case EN_BIN_G711A_RECV:
				 {
						XGLOG_INFO( "EN_XGATE_ASR_INHOUSE -> EN_BIN_G711U_RECV or EN_BIN_G711A_RECV!\n");
						gst_bin_add_many(m_bin, m_udpRecv,m_rtpBin,m_rtpDepay, m_audioDecoder,m_audioConvert, m_reSampler,m_capSet,m_audioSplit,m_fdSink,NULL);

						if(!gst_element_link_many(m_udpRecv,m_rtpBin,NULL)){
							 XGLOG_ERROR( "HmpVoiceTransceiverBin->add_and_link_elements: failed to link elements!!\n");
							 return FALSE;
						}
						if(!gst_element_link_many(m_rtpDepay, m_audioDecoder,m_audioConvert,m_reSampler,m_capSet,m_audioSplit,m_fdSink,NULL)){
							 XGLOG_ERROR( "HmpVoiceTransceiverBin->add_and_link_elements: failed to link elements!!\n");
							 return FALSE;
						}
						break;
				 } 
			}
	 }

	 return TRUE;
}



gboolean HmpVoiceTransceiverBin::register_appSink_callBack(gpointer data)
{
	 if(m_fdSink){
			g_object_set(G_OBJECT(m_fdSink),"emit-signals",TRUE,NULL);
			g_signal_connect(G_OBJECT(m_fdSink),"new-sample",G_CALLBACK(&on_setPayload),data);
			XGLOG_INFO("Recv new data on appsink with callId!!!!!!!!\n");
			return TRUE;
	 }
	 return FALSE;
}

gboolean HmpVoiceTransceiverBin::on_setPayload(GstElement* appsink,gpointer data)
{
	 Client *client = (Client*)data;
	 static int count = 0;
	 GstBuffer* buffer;
	 GstSample* sample = NULL;

	 sample = gst_app_sink_pull_sample(GST_APP_SINK(appsink));
	 buffer = gst_sample_get_buffer(sample);

   if(client->inHouseClient){
   GST_BUFFER_PTS(buffer) = 20;
	 //cout<<"Setting ptime to 20"<<"!!!!! for call id : "<< client->m_detail.m_callId.c_str()<<endl; 
   }

	 GstMapInfo info;

	 gst_buffer_map(buffer,&info,GST_MAP_READ);

	 cout<<"The packets write done  !!!!!"<<info.size<<"!!!!! for call id : "<< client->m_detail.m_callId.c_str()<<endl; 


	 if(info.size < 640)
			cout<<"The packets write done  !!!!!"<<info.size<<"!!!!! for call id : "<<client->m_detail.m_callId.c_str()<<endl;

	 if(client && client->speechMaticsClient && client->speechMaticsClient->isRecogStarted && client->speechMaticsClient->isRecogProcessing)
	 {
			try
			{
				 client->speechMaticsClient->ws.write(boost::asio::buffer(info.data, info.size));
			}

			catch(std::exception const& e)
			{
				 std::cerr << "Error: " << e.what() << std::endl;
			}
	 }

	 if(client && client->inHouseClient && client->inHouseClient->isRecogStarted && client->inHouseClient->isRecogProcessing)
	 {
			try
			{
				 //   XGLOG_INFO("The packets write done:%d !!!!! for call id :%s",info.size, (char*)ur_log_string(client->m_detail.m_callId));
				 client->inHouseClient->ws.write(boost::asio::buffer(info.data, info.size));
			}

			catch(std::exception const& e)
			{
				 std::cerr << "Error: " << e.what() << std::endl;
			}
	 }

	 gst_buffer_ref(buffer);
	 gst_sample_unref(sample);

	 gst_buffer_unref(buffer);

	 return GST_FLOW_OK;
}



HmpVoiceTransceiverChannel::HmpVoiceTransceiverChannel(string pipelineName, HmpChannelType pipelineType) :
	 HmpChannel(pipelineName, pipelineType) , m_pHmpVoiceTransceiverBin(NULL)
{
}

HmpVoiceTransceiverChannel::~HmpVoiceTransceiverChannel()
{
	 XGLOG_INFO( "HmpVoiceTransceiverChannel::~HmpVoiceTransceiverChannel destruction start");
	 if(m_pHmpVoiceTransceiverBin) {
			XGLOG_INFO( "BFR deleting the VoiceTransceiverBin in the VoiceTransceiverChannel");
			delete m_pHmpVoiceTransceiverBin;
			m_pHmpVoiceTransceiverBin = NULL;
			XGLOG_INFO( "AFR deleting the VoiceTransceiverBin in the VoiceTransceiverChannel");
	 }
}
gboolean HmpVoiceTransceiverChannel::construct_bin(string binName, HmpBinType binType, xGateASRType asrType, gint audioPort, GSocket *audioRecvSock,guint pt)
{
	 XGLOG_INFO( "HmpTransceiverChannel::construct_bin %s start", (char*)ur_log_string (binName));
	 gchar tmp[50]={0};
	 bzero(tmp,50);
	 if(!m_pHmpVoiceTransceiverBin) {
			m_pHmpVoiceTransceiverBin = new HmpVoiceTransceiverBin(binName, binType, asrType, audioPort, audioRecvSock);
			sprintf(tmp,",payload=(int)%d",pt);
			//sprintf(m_pHmpMRCPVoiceCaptureBin->m_payload.c_str,",payload=(int)%d",pt);
			m_pHmpVoiceTransceiverBin->m_payload=tmp;
			if(!m_pHmpVoiceTransceiverBin->construct()) {
				 XGLOG_ERROR( "construct_bin, HmpVoiceTransceiverBin construction failed !!");
				 return FALSE;
			} else {
				 gst_bin_add_many(GST_BIN(m_pipeline), GST_ELEMENT(m_pHmpVoiceTransceiverBin->m_bin), NULL);
			}
	 }
	 return TRUE;
}
gboolean HmpVoiceTransceiverChannel::register_rtpbin_callback(gpointer data){
	 if(m_pHmpVoiceTransceiverBin)
	 {
			m_pHmpVoiceTransceiverBin->register_rtpbin_callback(data);
			XGLOG_INFO("m_pHmpVoiceTransceiverBin::register_rtpbin_callback %p registered",data);
			return TRUE;
	 }
	 return FALSE;
}

gboolean HmpVoiceTransceiverChannel::register_appSink_callBack(gpointer data)
{
	 if(m_pHmpVoiceTransceiverBin){
			XGLOG_INFO("m_pHmpVoiceTransceiverChannel::register_appsink_callback %p registered",data);
			return m_pHmpVoiceTransceiverBin->register_appSink_callBack(data);
	 }

	 return FALSE;
}

/* HmpWaveEncoderBin start  class - End */

HmpWaveEncoderBin::HmpWaveEncoderBin(string binName,HmpBinType binType, gint recvPort, GSocket *recvSock):
	 HmpBin(binName, binType),m_capsFilter1(NULL), m_rtpDepay(NULL),m_reSampler1(NULL),
	 m_audioDecoder(NULL), m_fileSink(NULL),m_wavEnc(NULL),m_rtpjitterbuffer(NULL),m_webrtcDsp(NULL),m_reSampler2(NULL),
	 m_capsFilter2(NULL),m_alawenc(NULL),m_alawdec(NULL)
{
	 m_recvSocket = recvSock;
	 m_recvPort = recvPort;
}

HmpWaveEncoderBin::~HmpWaveEncoderBin()
{
	 if(m_recvSocket){
			XGLOG_INFO( "HmpWaveEncoderBin destructing send socket: %d socket", m_recvSocket);
			g_socket_close(m_recvSocket, NULL);
			g_object_unref(G_OBJECT(m_recvSocket));
			m_recvSocket = NULL;
			m_recvPort = 0;
	 }
}

gboolean HmpWaveEncoderBin::construct()
{
	 if(!create_udpsrc()) {
			XGLOG_ERROR( "construct HmpWaveEncoderBin, creating udpsrc failed !!");
			return FALSE;
	 }

	 if(!create_decode_elements()) {
			XGLOG_ERROR( "construct HmpWaveEncoderBin, creating elements failed !!");
			return FALSE;
	 }

	 if(!add_and_link_elements()) {
			XGLOG_ERROR( "construct HmpWaveEncoderBin, add and linking elements failed !!");
			return FALSE;
	 }
	 return TRUE;

}

gboolean HmpWaveEncoderBin::create_decode_elements()
{
	 guint latency = 40;
	 guint codec_g729 = 0;
	 GstCaps *cp;
	 switch(m_binType) {
			case EN_BIN_G711U_RECV:
				 {
						XGLOG_INFO( "create_decode_elements for G711U_RECV bin");
						m_rtpDepay = gst_element_factory_make("rtppcmudepay", NULL);
						m_audioDecoder = gst_element_factory_make("mulawdec", NULL);
						break;
				 }
			case EN_BIN_G711A_RECV:
				 {
						XGLOG_INFO( "create_decode_elements for G711A_RECV bin");
						m_rtpDepay = gst_element_factory_make("rtppcmadepay", NULL);
						m_audioDecoder = gst_element_factory_make("alawdec", NULL);
						break;
				 }
			case EN_BIN_G722_RECV:
				 {
						XGLOG_INFO( "create_decode_elements for G722_RECV bin");
						m_rtpDepay = gst_element_factory_make("rtpg722depay", NULL);
						m_audioDecoder = gst_element_factory_make("avdec_g722", NULL);
						break;
				 }
			case EN_BIN_G729_RECV:
				 {
						XGLOG_INFO( "create_decode_elements for G729_RECV bin");
						m_rtpDepay = gst_element_factory_make("rtpg729depay", NULL);
						m_audioDecoder = gst_element_factory_make("avdec_g729", NULL);
						codec_g729 = 1;
						break;
				 }
			case EN_BIN_OPUS_RECV:
				 {
						XGLOG_INFO( "create_decode_elements for OPUS_RECV bin");
						m_rtpDepay = gst_element_factory_make("rtpopusdepay", NULL);
						m_audioDecoder = gst_element_factory_make("opusdec", NULL);
						break;
				 }
			default:
				 {
						XGLOG_ERROR( "create_decode_elements failed for Unknown/Unsupported voice recv bin !!");
						return FALSE;
				 }
	 }


	 m_rtpjitterbuffer = gst_element_factory_make("rtpjitterbuffer",NULL);
	 g_object_set(G_OBJECT(m_rtpjitterbuffer), "latency",latency,NULL);
	 m_wavEnc = gst_element_factory_make("wavenc", NULL);
	 m_fileSink = gst_element_factory_make("filesink", NULL);
	 m_reSampler2 = gst_element_factory_make("audioresample", NULL);
	 m_capsFilter2 = gst_element_factory_make("capsfilter", NULL);
	 GstCaps *audioCaps2= gst_caps_from_string("audio/x-raw, channels=(int)1, rate=(int)16000");
	 g_object_set(G_OBJECT(m_capsFilter2), "caps",audioCaps2, NULL);
	 gst_caps_unref(audioCaps2);



	 m_webrtcDsp = gst_element_factory_make("webrtcdsp", NULL);
	 g_object_set(G_OBJECT(m_webrtcDsp), "noise-suppression",TRUE, NULL);
	 g_object_set(G_OBJECT(m_webrtcDsp), "noise-suppression-level",1,NULL);
	 g_object_set(G_OBJECT(m_webrtcDsp), "echo-cancel",FALSE, NULL);
	 g_object_set(G_OBJECT(m_webrtcDsp), "gain-control",FALSE, NULL);


	 if((m_binType ==EN_BIN_OPUS_RECV ) ||(m_binType == EN_BIN_G722_RECV)){
			m_alawenc = gst_element_factory_make("alawenc", NULL);
			m_alawdec = gst_element_factory_make("alawdec", NULL);
			m_reSampler1 = gst_element_factory_make("audioresample", NULL);
			m_capsFilter1 = gst_element_factory_make("capsfilter", NULL);
			GstCaps *audioCaps1 = gst_caps_from_string("audio/x-raw, channels=(int)1, rate=(int)8000");
			g_object_set(G_OBJECT(m_capsFilter1), "caps",audioCaps1, NULL);
			gst_caps_unref(audioCaps1);
	 }

	 return TRUE;
}



gboolean HmpWaveEncoderBin::add_and_link_elements()
{

	 if((m_binType ==EN_BIN_OPUS_RECV ) ||(m_binType == EN_BIN_G722_RECV)){

			if(!m_bin || !m_udpRecv ||!m_rtpDepay || !m_rtpjitterbuffer || !m_audioDecoder || !m_reSampler1 ||  !m_capsFilter1 || !m_alawenc  || !m_alawdec || !m_reSampler2 ||  !m_capsFilter2 || !m_webrtcDsp || !m_wavEnc || !m_fileSink) {
				 XGLOG_ERROR( "add_and_link_elements failed in HmpWavEncoderBin !!");
				 return FALSE;
			}
			gst_bin_add_many(m_bin, m_udpRecv,m_rtpjitterbuffer,m_rtpDepay, m_audioDecoder,m_reSampler1, m_capsFilter1,m_alawenc,m_alawdec,m_reSampler2,m_capsFilter2,m_webrtcDsp,m_wavEnc,m_fileSink,NULL);

			if(!gst_element_link_many(m_udpRecv,m_rtpjitterbuffer,m_rtpDepay,m_audioDecoder,m_reSampler1,m_capsFilter1,m_alawenc,m_alawdec,m_reSampler2,m_capsFilter2,m_webrtcDsp,m_wavEnc,m_fileSink,NULL)){
				 XGLOG_ERROR("HmpWaveEncoderBin->add_and_link_elements: failed to link elements!!\n");
				 return FALSE;
			}
	 }else {

			if(!m_bin || !m_udpRecv ||!m_rtpDepay || !m_rtpjitterbuffer || !m_audioDecoder || !m_reSampler2 ||  !m_capsFilter2 || !m_webrtcDsp || !m_wavEnc || !m_fileSink) {
				 XGLOG_ERROR( "add_and_link_elements failed in HmpWavEncoderBin !!");
				 return FALSE;
			}

			gst_bin_add_many(m_bin, m_udpRecv,m_rtpjitterbuffer,m_rtpDepay, m_audioDecoder,m_reSampler2,m_capsFilter2,m_webrtcDsp,m_wavEnc,m_fileSink,NULL);

			if(!gst_element_link_many(m_udpRecv,m_rtpjitterbuffer,m_rtpDepay,m_audioDecoder,m_reSampler2,m_capsFilter2,m_webrtcDsp,m_wavEnc,m_fileSink,NULL)){
				 XGLOG_ERROR("HmpWaveEncoderBin->add_and_link_elements: failed to link elements!!\n");
				 return FALSE;
			}
	 }

	 return TRUE;
}
/* HmpWaveEncoderBin start  class - End */




/* HmpWaveEncoderChannel start  class - start */

HmpWaveEncoderChannel::HmpWaveEncoderChannel(string pipelineName, HmpChannelType pipelineType) :
	 HmpChannel(pipelineName, pipelineType) , m_pHmpWaveEncoderBin(NULL)
{
}

HmpWaveEncoderChannel::~HmpWaveEncoderChannel()
{
	 XGLOG_INFO( "HmpWaveEncoderChannel::~HmpWaveEncoderChannel destruction start");
	 if(m_pHmpWaveEncoderBin) {
			delete m_pHmpWaveEncoderBin;
			m_pHmpWaveEncoderBin = NULL;
	 }
}

gboolean HmpWaveEncoderChannel::construct_bin(string binName, HmpBinType binType, gint audioPort, GSocket *audioRecvSock,guint pt)
{
	 XGLOG_INFO( "HmpWaveEncoderChannel::construct_bin %s start", (char*)ur_log_string (binName));
	 gchar tmp[50]={0};
	 bzero(tmp,50);
	 if(!m_pHmpWaveEncoderBin) {
			m_pHmpWaveEncoderBin = new HmpWaveEncoderBin(binName, binType, audioPort, audioRecvSock);
			sprintf(tmp,",payload=(int)%d",pt);
			m_pHmpWaveEncoderBin->m_payload=tmp;
			if(!m_pHmpWaveEncoderBin->construct()) {
				 XGLOG_ERROR( "construct_bin, HmpWaveEncoderBin construction failed !!");
				 return FALSE;
			} else {
				 gst_bin_add_many(GST_BIN(m_pipeline), GST_ELEMENT(m_pHmpWaveEncoderBin->m_bin), NULL);
			}
	 }
	 return TRUE;
}

/* HmpWaveEncoderChannel start  class - End */

