#ifndef _MBCLINK_SERVICE_DEFINE_H
#define _MBCLINK_SERVICE_DEFINE_H

#include "xGateDefine.h"
#include "IURDefines.h"
#include "xGateUtil.h"

//#define XGATE_TCP_READ_SIZE  2048
#define MBCLINK_TCP_PORT 32779
//#define TCP_MAX_MESSAGE_SIZE 8192

const unsigned short g_mbclink_gateway_port = 32779;
static const char json_mbclink_tmpl[] = " { \"msg_type\" : 0, \
                                          \"msg_len\" : 0 , \
                                          \"data\" : { \
                                          \"mgresource_id\": \"\", \
                                          \"call_type\": 1, \
                                          \"call_id\": \"\", \
																					\"conn_id\": \"\", \
																					\"device_id\": \"\", \
																					\"prev_act_spkr\": \"\", \
																					\"meeting_name\": \"\", \
																					\"sip_id\": \"\", \
                                          \"is_ccaas\" : 0, \
                                          \"gateway_id\" : 0, \
                                          \"context_id\" : 0, \
                                          \"leg_id\" : 0, \
                                          \"sdpinfo\":[{ \
                                          \"media_type\": 0,\
                                          \"ip_addr\": \"\", \
                                          \"ip_port\": 0, \
                                          \"dial_ip\": \"\", \
                                          \"dial_port\": 0, \
                                          \"resp_ip\": \"\", \
                                          \"resp_port\": 0, \
                                          \"relayip_addr\" : \"\",\
                                          \"relay_port\" : 0,\
                                          \"reflexip_addr\" : \"\",\
                                          \"reflex_port\" : 0,\
                                          \"codec\": 0, \
                                          \"codec_name\": \"\",\
                                          \"fmtp\": \"\", \
                                          \"ice_ufrag\" : \"\",\
                                          \"ice_pwd\" : \"\",\
                                          \"fingerprint\" : \"\",\
                                          \"ptime\" : 0,\
                                          \"framerate\" : 0,\
                                          \"imageattr_x\" : 0,\
                                          \"imageattr_y\" : 0,\
                                          \"play_file\" : \"\",\
                                          \"ssrc\": \"\", \
                                          \"cname\": \"\", \
                                          \"mslable\": \"\", \
                                          \"lable\": \"\"}],\
                                          \"participant_list\":[{ \
                                          \"call_id\": \"\",\
					  \"joiner_type\": \"\"}], \
					  \"urmeet_user_list\":[{ \
					  \"email\": \"\",\
					  \"profile_img\": \"\", \
					  \"sip_id\": \"\", \
					  \"name\": \"\", \
					  \"ext\": \"\", \
					  \"role_type\": \"\", \
					  \"user_type\": \"\"}], \
					  \"screenshare_id\": \"\", \
                                          \"sdp_str\" : \"\",\
                                          \"joiner_type\": 0, \
                                          \"plan_type\": 0, \
                                          \"asr_type\": 0, \
                                          \"browser_type\": \"\", \
                                          \"media_proto\" : \"\",\
                                          \"out_proto\": \"\", \
                                          \"record_file\": \"\", \
                                          \"record_url\": \"\", \
                                          \"file_size\": 0, \
                                          \"dtmf_digits\" : \"\",\
                                          \"sig_type\" : 0, \
                                          \"call_dir\" : 0, \
                                          \"media_state\": \"\", \
                                          \"rtp_endpoint\": \"\"} } ";

typedef enum
{
  EN_MBC_LINK_EVENT_UNKNOWN = 0,
  EN_MBC_LINK_EVENT_CONF_PORT_ALLOCATION_REQ = 1,
  EN_MBC_LINK_EVENT_CONF_PORT_ALLOCATION_RES = 2,
  EN_MBC_LINK_EVENT_CONF_ADD_PARTICIPANT_REQ = 3,
  EN_MBC_LINK_EVENT_CONF_ADD_PARTICIPANT_RES = 4,
  EN_MBC_LINK_EVENT_CONF_DELETE_REQ = 5,
  EN_MBC_LINK_EVENT_CONF_DEL_PARTICIPANT_REQ = 6,
  EN_MBC_LINK_EVENT_SFU_CONN_CLOSED = 7,
  EN_MBC_LINK_EVENT_CONF_USER_SDP_UPDATE_REQ = 8,
  EN_MBC_LINK_EVENT_CONF_USER_SDP_UPDATE_RES = 9,
  EN_MBC_LINK_EVENT_CONF_USER_MEDIA_UPDATE_REQ = 10,
  EN_MBC_LINK_EVENT_CONF_USER_MEDIA_UPDATE_RES = 11,
  EN_MBC_LINK_EVENT_DTLS_CONN_FAILED = 12,
  EN_MBC_LINK_EVENT_DTLS_RECONNECT_REQ = 13,
  EN_MBC_LINK_EVENT_DTLS_RECONNECT_RES = 14,
  EN_MBC_LINK_EVENT_CONF_USER_TILE_CHANGE_REQ = 15,
  EN_MBC_LINK_EVENT_CONF_USER_TILE_CHANGE_RES = 16,
  EN_MBC_LINK_EVENT_SCREENSHARE_VIDEO_REQ = 17,
  EN_MBC_LINK_EVENT_ACTIVE_SPEAKER_VIDEO_REQ = 18,
  EN_MBC_LINK_EVENT_ACTIVE_SPEAKER_INFO = 19,
  EN_MBC_LINK_EVENT_CONF_USER_RECORDING_REQ = 20,
  EN_MBC_LINK_EVENT_GRID_VIEW_VIDEO_REQ = 21,
  EN_MBC_LINK_EVENT_CLIENT_CONNECTED = 22,
  EN_MBC_LINK_EVENT_CLIENT_CONNECTION_CLOSED = 23,
} MBCLinkServiceEvent;


#endif
