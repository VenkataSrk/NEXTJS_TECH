#ifndef _URCB_CONTROLLER_H__
#define _URCB_CONTROLLER_H__

#pragma once
#include<ace/Task.h>
#include <ace/Singleton.h>
#include <map>
#include <vector>

//local includes
#include "UREngineConfig.h"
#include "IURModule.h"
#include "xGateSchedulerMsg.h"
#include "xGateSchedulerService.h"
#include "xGateSchedulerServiceMsg.h"
#include "URCBCallContext.h"
#include "URCBCallContextHandler.h"
#include "URConfRoomContextHandler.h"
#include "SipMediahandler.h"
#include "xGateMBCLinkServiceMsg.h"
#include "URCBCtrlDefines.h"
#include <uuid/uuid.h>
//for active speaker
#include <string>
#include <unordered_map>
#include <chrono>
#include <iostream>
#include <deque>

#define MAX_RETRY_DIAL_ATTEMPT 1
#define URCBCONTROLLER URCB_CONTROLLER::instance()

class URCBController : public ACE_Task<ACE_MT_SYNCH>, public IURModuleCallBack
{
   public:
   URCBController();
   virtual ~URCBController();

   bool init(void);
   virtual int svc(void);
   bool stop();

	 void updateUserVolume(const std::string& userId, double volumeLevel, URConfRoom* urConfRoom);
	 double calculateAverageVolume(UserData& userData);
	 std::string findActiveSpeaker(URConfRoom* urConfRoom);
	 bool isSpeaking(const std::chrono::steady_clock::time_point& currentTime, UserData& userData);
	 void activeSpeakerOnDisconnect(const std::string& strConfRoomId, URConfRoom* urConfRoom, URCBSIOUserInfo* ptSioUserInfo);

   //Member functions to initialize modules
   int initSchedulerService();
   bool initModule();

   //Member functions to process msg from other modules
   bool handle_msg(ACE_Message_Block *pAmb);
   bool handleSipModuleMsg(xGateBaseMsg *pMsg);
   bool handleMBCLinkModuleMsg(xGateBaseMsg *pMsg);
   bool handleRedisModuleMsg(xGateBaseMsg *pMsg);
   bool handleRedisGetResponse(xGateBaseMsg *pMsg);
   bool handle_TimerResponse(xGateBaseMsg *pBaseMsg);
	 bool handleSchedulerUpdateMsg(xGateBaseMsg *pBaseMsg);
   bool handle_MBCResponse(xGateBaseMsg *pMsg);
   bool handle_sio_mbc_message(URCBSIOUserInfo * ptSioUserInfo);

   bool post_msg_to_socketio(URCBSIOUserInfo * ptSioUserInfo);
   bool post_msg_to_mbclink_mod(xGateMBCLinkServiceMsg *& pMBCLinkServiceMsg);

   bool set_timer(string callId, URCBTimerData *& pturcbTimerData, SchedulerType schedulerType, int timeOut);
   bool cancel_timer(string callId, SchedulerType schedulerType);

   Value setJsonParam(string &strValue,Document &doc);
   Value setJsonParam(unsigned int iValue, Document &doc);

   std::string form_socketIO_json_msg(URCBSIOUserInfo * ptSioUserInfo);
   bool decode_sio_json_msg_from_MBC(const char* jsonData,URCBSIOUserInfo *& ptSioUserInfo);
   bool fillConfUserInfoDetails(const Value &userInfoDetails, URCBSIOUserInfo *ptSioUserInfo);
   bool fillConfParticipantInfoDetails(const Value &participantInfoDetails, URCBSIOUserInfo *ptSioUserInfo);
   bool fillURMeetUserInfo(const Value &userInfoDetails, URCBSIOUserInfo *ptSioUserInfo);

   bool isEventSequenceUnique(URCBSIOUserInfo * ptSioUserInfo);
   bool fillURParticipantListInfo(URCBCallContext *& ptCtx, URConfRoom * urConfRoom, URCBSIOUserInfo * ptSioUserInfo);

   bool copyRequiredConfUserInfo(URCBCallContext * ptCtx, URConfCallUserInfo *& ptConfUserInfo);
   
   bool process_msg(URCBCallContext * ptCtx, xGateBaseMsg *pMsg=NULL);

   bool handleNewCallNotify(xGateBaseMsg *pMsg);
   bool sendConfPortAllocationRequest(URCBCallContext * ptCtx,xGateBaseMsg *pMsg);
   bool handleConfPortAllocationResponse(xGateBaseMsg *pMsg);
   bool sendCallAnswerCommand(URCBCallContext * ptCtx);
   bool handleCallConnectedNotify(xGateBaseMsg *pMsg);

   bool handleSfuConnClosedNotify(xGateBaseMsg *pMsg);
   bool handleDtlsConnFailedNotify(xGateBaseMsg *pMsg);
   bool sendErrorNotifyCommand(URCBCallContext *ptCtx);
   bool handleCallDisconnectedNotify(xGateBaseMsg *pMsg);
   bool handleCallClearing(URCBCallContext * ptCtx);
   bool sendConfMediaDeAllocationRequest(URCBCallContext * ptCtx);
   bool sendConfDeleteParticipantRequest(URCBCallContext * ptCtx);
   bool sendCallDisconnectRequest(URCBCallContext * ptCtx);

   bool sendConfUserSDPUpdateRequest(URCBCallContext * ptCtx);
   bool handleConfUserSDPUpdateResponse(xGateBaseMsg *pMsg);
   bool sendConfUserSDPUpdateResponse(URCBCallContext * ptCtx);

   bool sendConfUserTileChangeRequest(URCBCallContext * ptCtx);
   bool handleConfUserTileChangeResponse(xGateBaseMsg *pMsg);
   bool sendConfUserTileChangeResponse(URCBCallContext * ptCtx);

   bool sendConfUserRecStartReq(URCBCallContext * ptCtx);
   bool sendConfUserRecPauseReq(URCBCallContext * ptCtx);
   bool sendConfUserRecResumeReq(URCBCallContext * ptCtx);
   bool sendConfUserRecStopReq(URCBCallContext * ptCtx);

   bool sendScreenshareVideoRequest(URCBCallContext * ptCtx);
   bool sendActiveSpeakerVideoRequest(URCBCallContext * ptCtx);
   bool sendGridViewVideoRequest(URCBCallContext * ptCtx);
   bool sendConfRoomActiveSpeakerInfo(std::string strConfRoomId);
	 bool handleActiveSpeakerInfoNotify(xGateBaseMsg *pMsg);

   bool fillMediaDetailForMediaRequest(URCBCallContext * ptCtx, MgMediaDetail &mediaDetail);
   bool fillSDPInfoForMediaRequest(xGateSIPMsg *ptSipmsg,MgMediaDetail &mediaDetail);
   bool fillParticipantInfoForMediaRequest(MgMediaDetail &mediaDetail,std::string &strConfRoomId,URCBCallContext * ptCtx);
	 bool fillURMeetUserInfoForMediaRequest(MgMediaDetail &mediaDetail,std::string &strConfRoomId,URCBCallContext * ptCtx);
   bool getURConfInfoFromUnknownHeaderList(URCBCallContext * ptCtx, xGateSIPMsg *ptSipmsg);
   bool getConfSIOMsgType(string &strEventName,URConfSIOMsgType &msgType);

   bool update_callid_in_socketio_user_info(URCBCallContext *& ptCtx);

   std::string getCurrentUTCTime();
   bool fillAorDetails(URCBCallContext * ptCtx);
   bool getSipUNandDN( std::string sipURL, std::string& strUN, std::string& strDNS);
   void splitValues(std::vector<string> &Values, std::string strValue, std::string delim);

   //IUR module call back function
   bool handleModuleCallbackMsg(IURModuleMsg * pctrlMsg);
   vector< IURModule*> arrayModules;

   bool handle_call_redirection_verification(std::string strConfRoomId, std::string strCallId);
   bool sendCallRedirectionRequest(std::string strCallId, std::string strRedirectIp);

   bool handle_conf_bridge_broadcast_msg(URCBBroadCastMsg * ptURCBBroadCastMsg);
   bool decode_conf_bridge_bcast_msg_from_MBC(const char* jsonData, URCBBroadCastMsg *& ptURCBBroadCastMsg);

   std::string form_conf_room_info_json_msg(URCBBroadCastMsg *ptURCBBroadCastMsg);
   bool check_for_conf_room_create_info(URCBCallContext *ptCtx);
   bool send_confroom_created_info_to_mbc(URCBCallContext * ptCtx);
   bool send_confroom_deleted_info_to_mbc(URCBCallContext * ptCtx);
   bool post_msg_to_confbridge(string info);

   URCcaasBroadcastMsg* fill_ccaas_bcast_msg_info(URCBCallContext * ptCtx);
   bool post_ccaas_conf_info_to_acd(URCBCallContext * ptCtx);
   std::string encode_ccaas_conf_info_json_msg(URCcaasBroadcastMsg * ptURCcaasBcastMsg);
   URCcaasBroadcastMsg * decode_ccaas_info_json_message(const char* data, URCcaasBroadcastMsg *& ptURCcaasBcastMsg);
   bool handle_ccaas_acd_broadcast_msg(URCcaasBroadcastMsg * ptURCcaasBcastMsg);

   bool fetch_acd_channel_list();
   bool get_acd_channel_from_list(URCBCallContext *& ptCtx);
   bool frame_acd_channel_key(std::string strDomainId, std::string &strKey);
   bool get_acd_channel_from_redis(URCBCallContext * ptCtx);
   bool set_acd_channel(URCBCallContext * ptCtx);

   bool check_for_meeting_duration(std::string strConfRoomId);
	 bool frame_meeting_duration_key(std::string strConfRoomId, std::string &strKey);
   bool set_meeting_duration(std::string strConfRoomId, long int duration);

   std::string generateTransid(); 
   std::string UUIDToSessionId(const char *uuid, const size_t len);
   bool isValidSessionID(const char *uuid, const size_t len);

   bool handleMbcConnnectedMsg(xGateBaseMsg *pMsg);
   bool handleMbcConClosedMsg(xGateBaseMsg *pMsg);
   std::string chooseMbcIp(std::string strConfRoomId);

   private:
   URConfRoomContextHandler m_tURConfRoomCtxHndlr;
   URCBCallContextHandler m_tURCBCallCtxHndlr;
   std::vector<std::string> m_vAcdChannelList;
   vector<TcpConTuple> m_mbcConList;
};

typedef ACE_Singleton<URCBController, ACE_Recursive_Thread_Mutex> URCB_CONTROLLER;

#endif
