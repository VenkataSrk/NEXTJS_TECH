#ifndef __URMRCPModuleBridge_H__
#define __URMRCPModuleBridge_H__

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <ace/Task.h>
#include <ace/Singleton.h>

#include "URModuleFactory.h"
#include "IURModuleConfig.h"
#include "UrMrcpDefines.h"
#include "SttTtsServiceMsg.h"
#include "xGateMRCPMsg.h"
#include "URMrcpSessionHandler.h"
#include "MrcpConfig.h"
#include "UrMrcpDbServiceMsg.h"
#include "xGateSchedulerServiceMsg.h"
#include "URSpeechmaticssttclient.h"
#include "URInhousesttclient.h"

#define URMRCPMODULEBRIDGE UR_MRCP_MODULE_BRIDGE::instance()

class URMRCPConfig:  public IURModuleConfig{
        public:
        URMRCPConfig();
        ~URMRCPConfig();
        int  m_nMBCThreadPool;
        virtual void setCallBack(IURModuleCallBack* ptCallback);
        virtual IURModuleCallBack* getCallBack();
};

class URMRCPModuleBridge : public ACE_Task<ACE_MT_SYNCH>, public IURModuleCallBack
{
        std::string m_strNameServerDetails;
        IURModule * m_ptModule;
        public:
        URMRCPModuleBridge();
        ~URMRCPModuleBridge();
        bool    init();
        int     svc();

        bool stop(void);
        bool handle_msg(ACE_Message_Block *pAmb);
//	bool handle_httpmsg(URMRCPBaseMsg *pbasemsg); // Added to get Http response from Avamo

	bool    handleModuleCallbackMsg(IURModuleMsg* ptMsg );
	bool process_mrcp_port_allocate_req(xGateDbConfigMgrServiceMsg *mrcpServiceMsg);
	bool process_mrcp_recognition_start_req(xGateDbConfigMgrServiceMsg *mrcpServiceMsg);
	bool process_mrcp_recognition_send_req(xGateDbConfigMgrServiceMsg *mrcpServiceMsg);
	bool process_mrcp_recognition_pause_req(xGateDbConfigMgrServiceMsg *mrcpServiceMsg);
	bool process_mrcp_recognition_resume_req(xGateDbConfigMgrServiceMsg *mrcpServiceMsg);
	bool process_mrcp_recognition_stop_req(xGateDbConfigMgrServiceMsg *mrcpServiceMsg);
	bool process_mrcp_termination_req(xGateDbConfigMgrServiceMsg *pMsg);
	bool process_mrcp_remove_participant_req(xGateDbConfigMgrServiceMsg *pMsg);
	bool process_mrcp_add_participant_req(xGateDbConfigMgrServiceMsg *pMsg);
	bool process_mrcp_avaamo_response(UrMrcpDbServiceMsg* dbService);
	bool process_mrcp_tts_start_req(xGateDbConfigMgrServiceMsg *mrcpServiceMsg);
	bool process_mrcp_received_dtmf_digit(xGateDbConfigMgrServiceMsg *mrcpServiceMsg);
	bool process_mrcp_synthesize_stop_req(xGateDbConfigMgrServiceMsg *pMsg);
	bool process_mrcp_msg_from_db_service(UrMrcpDbServiceMsg *pMsg);
	bool process_mrcp_scheduler_msg(xGateSchedulerServiceMsg *pMsg);
	bool process_mrcp_stt_tts_allocate_req(xGateDbConfigMgrServiceMsg *pMsg);
 	bool process_synth_req(xGateDbConfigMgrServiceMsg *pMsg);
 	bool process_vms_transcript_request(ccaasVmsInfo* pMsg);
 	bool encode_vms_transcript_req_msg(ccaasVmsInfo* &pMsg);
	bool PostMessageToMrcpMg(xGateDbConfigMgrServiceMsg *mrcpServiceMsg);
  	bool postMessageToMediaService(xGateMRCPMsg* pMgMsg);
	bool send_stop_start_req_to_stt_mg(UrMrcpSessionDetails *serviceMsg);
	bool send_base64_filename(xGateDbConfigMgrServiceMsg *mrcpServiceMsg);

	bool add_user_to_display_list(xGateDbConfigMgrServiceMsg *pMsg);
	bool remove_user_from_display_list(std::string uID, std::string callID);
	bool insertSessionInfo(std::string ID, UrMrcpSessionDetails *sessionDetails);
	bool getSessionInfo(std::string ID,UrMrcpSessionDetails* &sessionDetails);
	bool insertUserToList(std::string ID,Display_Info displayInfo);
	bool eraseUserFromList(std::string ID,std::string userExt);
	bool handleTerminationRequest(xGateDbConfigMgrServiceMsg *pMsg);
	bool deleteSessionInfo(std::string ID);
	bool deleteUserSessionInfo(std::string ID);
	bool insertCallSessionId(STR key, STR value);
	bool handleSessionForTransferCall(STR uID, STR callID);
	bool getDisplayList(STR ID,std::list<Display_Info> &displayList);
	bool removeFromcallDetailsMap(STR uID,STR callID);
	bool getCallSessionId(STR key, STR &value);

	bool postMessageToSchedulerService(std::string callID, SchedulerType timerType, TimerFunction timerFunc);
  	bool postMessageToDbService(ccaasVmsInfo* pMsg);
  	bool postMessageToRedisService(UrMrcpDbServiceMsg* pDbServiceMsg);

	template <typename T> bool deleteServiceMsg(T *&pMsg); // template function to delete serviceMsg.

	Value setJsonParam(string &strValue, Document &doc);
	Value setJsonParam(unsigned int iValue, Document &doc);
	UrMrcpSessionDetails * decode_mrcp_sdp_message(const char* jsonData);
	std::string encode_mrcp_sdp_message(UrMrcpSessionDetails* sessionDetails);
	std::string frameDisplayInfo(string ext,string domain_id,string device);
	vector< IURModule*> arrayModules;

	URMrcpSessionHandler m_tMrcpSessionHandler;
	std::map<std::string ,UrMrcpSessionDetails *>m_sessionDetailMap;
	std::multimap<std::string, std::string> m_callDetailsMap;
//	std::list<std::string>m_displayUserList;
	std::map<std::string,std::list<std::string>>m_displayUserList;
  	std::map<std::string,URSpeechmaticssttclient*> m_tSttObj;
  	std::map<std::string,URInhousesttclient*> m_tSttInhouse;
  	std::list<std::string> m_vmsSttlist;
	std::map<std::string,std::list<Display_Info>> m_mapDisplay;

	std::mutex m_lksessionDetailMap;
	std::mutex m_lkmapDisplay;
	std::mutex m_lkcallDetailsMap;
};
typedef ACE_Singleton<URMRCPModuleBridge, ACE_Recursive_Thread_Mutex> UR_MRCP_MODULE_BRIDGE;
#endif
