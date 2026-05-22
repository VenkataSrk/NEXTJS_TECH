/*
    Author: @PraveenMS
  	Speech recognition framework object 
*/

#ifndef UNI_MRCP_CLI_APP_FW_RECOG_H
#define UNI_MRCP_CLI_APP_FW_RECOG_H


#include <apr_xml.h>
#include <apr_hash.h>
#include "apt_consumer_task.h"
#include "WrapSpeechRecogMrcpSession.h"
#include "xGateUtil.h"
#include "UrMrcpDefines.h"

class UmcScenario;

class UniMrcpCliAppFwSpeechRecog : public UmcRecogSessionMethodProvider
{
public:
	UniMrcpCliAppFwSpeechRecog();
	~UniMrcpCliAppFwSpeechRecog();

	/*
	 * Draft function declerations based on FreeSwitch implementation,
	 *
	 * */

	bool Create(apt_dir_layout_t* pDirLayout, apr_pool_t* pool);
	void Destroy();

	void StopSession(const char* id);
	void ShowScenarios();
	void ShowSessions();

	bool CreateMrcpClient();
	void DestroyMrcpClient();

	bool CreateTask();
	void DestroyTask();

	  /*
	*			   where pScenarioName = SPEECH_SYNTH, SPEECH_RECOGN
	*					   pProfileName  =	audio file path
	* */

	void ProcessStopRequest(const char* id);
	void ProcessKillRequest(const char* id);

	void ProcessSessionExit(WrapSpeechRecogMrcpSession* pUmcSession);

	bool AddSession(WrapSpeechRecogMrcpSession* pSession);
	bool RemoveSession(WrapSpeechRecogMrcpSession* pSession);

	void ExitSession(WrapSpeechRecogMrcpSession* pUmcSession);

	bool ProcessSpeechRecogRequest();

        bool ProcessStartRecogReq();
        bool ProcessSendRecogReq();
	bool ProcessPauseRecogReq();
	bool ProcessResumeRecogReq();
	bool ProcessStopRecogReq();
	bool ProcessTerminationReq();
	bool ProcessRemoveParticipantReq();

	void KillSession(const char* id);
        WrapSpeechRecogMrcpSession* GetSession(std::string sessionName);

/*
 * Draft function declerations based on FreeSwitch implementation,
 *
 * */

	friend apt_bool_t UmcRecogProcessMsg(apt_task_t* pTask, apt_task_msg_t* pMsg);
	friend void UmcRecogOnStartComplete(apt_task_t* pTask);
	friend void UmcRecogOnTerminateComplete(apt_task_t* pTask);
	friend apt_bool_t AppRecogMessageHandler(const mrcp_app_message_t* pAppMessage);
	UrMrcpSessionDetails *m_tSessionDetails;
	std::string m_sessionId;
	std::string m_uid;
	bool OnMrcpRequestResponse(const mrcp_app_message_t* pMessage);
private:
		   /*
	* for future use
	* Refrenece: FreeSwitch
	*	   */

	apr_pool_t*          m_pPool;
	apt_dir_layout_t*    m_pDirLayout;
	apt_consumer_task_t* m_pTask;

	mrcp_client_t*       m_pMrcpClient;
	mrcp_application_t*  m_pMrcpApplication;

//	apr_hash_t*          m_pScenarioTable;
	apr_hash_t*          m_pSessionTable;
};

#endif
