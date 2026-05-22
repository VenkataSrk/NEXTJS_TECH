/*
    Author: @PraveenMS
  	Speech synthesis framework object 
*/

#ifndef UNI_MRCP_CLI_APP_FW_H
#define UNI_MRCP_CLI_APP_FW_H


#include <apr_xml.h>
#include <apr_hash.h>
#include "apt_consumer_task.h"
#include "WrapMrcpSession.h"
#include "xGateUtil.h"
#include "UrMrcpDefines.h"

class UmcScenario;

class UniMrcpCliAppFw : public UmcSessionMethodProvider
{
public:
	UniMrcpCliAppFw();
	~UniMrcpCliAppFw();

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

	void ProcessSessionExit(WrapMrcpSession* pUmcSession);

	bool AddSession(WrapMrcpSession* pSession);
	bool RemoveSession(WrapMrcpSession* pSession);

	void ExitSession(WrapMrcpSession* pUmcSession);

	bool ProcessSynthRequest();

	bool ProcessSynthTerminationReq();

	bool ProcessSendSynthesisRequest(std::string text);

	bool ProcessSynthStopReq(std::string sessionId);

	WrapMrcpSession* GetSession(std::string sessionName);

	bool OnMrcpTtsRequestResponse(const mrcp_app_message_t* pMessage);

/*
 * Draft function declerations based on FreeSwitch implementation,
 *
 * */

	friend apt_bool_t UmcProcessMsg(apt_task_t* pTask, apt_task_msg_t* pMsg);
	friend void UmcOnStartComplete(apt_task_t* pTask);
	friend void UmcOnTerminateComplete(apt_task_t* pTask);
	friend apt_bool_t AppMessageHandler(const mrcp_app_message_t* pAppMessage);
	
	UrMrcpSessionDetails *m_tSessionDetails;
	std::string m_sessionId;
	std::string m_uid;

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
