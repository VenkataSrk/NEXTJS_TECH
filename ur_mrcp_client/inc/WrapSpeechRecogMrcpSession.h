/*
	@Author: PraveenMS
	 Speech recognitions wrapper functions for unimrcp library
*/


#ifndef WRAP_MRCP_RECOG_SESSION_H
#define WRAP_MRCP_RECOG_SESSION_H

#include "mrcp_application.h"
#include "MrcpCliAppSpeechRecognizeScenario.h"
#include "xGateUtil.h"
#include "UrMrcpDefines.h"

class MrcpCliAppSpeechRecognizeScenario;
class WrapSpeechRecogMrcpSession;

class UmcRecogSessionEventHandler
{
public:
	virtual ~UmcRecogSessionEventHandler() {}

	virtual bool OnSessionTerminate(mrcp_sig_status_code_e status) = 0;
	virtual bool OnSessionUpdate(mrcp_sig_status_code_e status) = 0;
	virtual bool OnChannelAdd(mrcp_channel_t* pMrcpChannel, mrcp_sig_status_code_e status) = 0;
	virtual bool OnChannelRemove(mrcp_channel_t* pMrcpChannel, mrcp_sig_status_code_e status) = 0;
	virtual bool OnMessageReceive(mrcp_channel_t* pMrcpChannel, mrcp_message_t* pMrcpMessage) = 0;
	virtual bool OnTerminateEvent(mrcp_channel_t* pMrcpChannel) = 0;
	virtual bool OnResourceDiscover(mrcp_session_descriptor_t* pDescriptor, mrcp_sig_status_code_e status) = 0;
};

class UmcRecogSessionMethodProvider
{
public:
	virtual ~UmcRecogSessionMethodProvider() {}

	virtual void ExitSession(WrapSpeechRecogMrcpSession* pUmcSession) = 0;
};

class WrapSpeechRecogMrcpSession : protected UmcRecogSessionEventHandler
{
public:
	WrapSpeechRecogMrcpSession(const MrcpCliAppSpeechRecognizeScenario* pRecogScenario);
	virtual ~WrapSpeechRecogMrcpSession();

	virtual bool Run();
	virtual bool Stop();
	virtual bool Terminate();

	void SetMrcpProfile(const char* pMrcpProfile);
	void SetMrcpApplication(mrcp_application_t* pMrcpApplication);
	void SetMethodProvider(UmcRecogSessionMethodProvider* pMethodProvider);

	const MrcpCliAppSpeechRecognizeScenario* GetSpeechRecognizeScenario() const;
	apr_pool_t* GetSessionPool() const;

	const char* GetId() const;
	UrMrcpSessionDetails *m_tSdpDescriptor;
	mrcp_session_t*             m_pMrcpSession;
	mrcp_sig_status_code_e      m_sessionStatus;
	bool isRecogCompleted;
	bool isEOS;
//protected:
	virtual bool Start() = 0;

	bool CreateMrcpSession(const char* pProfileName);
	bool DestroyMrcpSession();

	bool AddMrcpChannel(mrcp_channel_t* pMrcpChannel);
	bool RemoveMrcpChannel(mrcp_channel_t* pMrcpChannel);
	bool SendMrcpRequest(mrcp_channel_t* pMrcpChannel, mrcp_message_t* pMrcpMessage);
	bool DiscoverResources();

	mrcp_channel_t* CreateMrcpChannel(
			mrcp_resource_id resource_id,
			mpf_termination_t* pTermination,
			mpf_rtp_termination_descriptor_t* pRtpDescriptor,
			void* pObj);
	mpf_termination_t* CreateAudioTermination(
			const mpf_audio_stream_vtable_t* pStreamVtable,
			mpf_stream_capabilities_t* pCapabilities,
			void* pObj);
	mrcp_message_t* CreateMrcpMessage(
			mrcp_channel_t* pMrcpChannel,
			mrcp_method_id method_id);

	virtual bool OnSessionTerminate(mrcp_sig_status_code_e status);
	virtual bool OnSessionUpdate(mrcp_sig_status_code_e status);
	virtual bool OnChannelAdd(mrcp_channel_t* pMrcpChannel, mrcp_sig_status_code_e status);
	virtual bool OnChannelRemove(mrcp_channel_t* pMrcpChannel, mrcp_sig_status_code_e status);
	virtual bool OnMessageReceive(mrcp_channel_t* pMrcpChannel, mrcp_message_t* pMrcpMessage);
	virtual bool OnTerminateEvent(mrcp_channel_t* pMrcpChannel);
	virtual bool OnResourceDiscover(mrcp_session_descriptor_t* pDescriptor, mrcp_sig_status_code_e status);

	const char* GetMrcpSessionId() const;
	mrcp_message_t* GetMrcpMessage() const;
	std::string CreateUniqueSessionId();
	const MrcpCliAppSpeechRecognizeScenario*          m_pRecogScenario;
	const char*                 m_pMrcpProfile;
	const char*                 m_Id;
	std::string                 sessionId;
private:
	apr_pool_t*                 m_Pool;
	UmcRecogSessionMethodProvider*   m_pMethodProvider;
	mrcp_application_t*         m_pMrcpApplication;
	mrcp_message_t*             m_pMrcpMessage; /* last message sent */
	bool                        m_Running;
	bool                        m_Terminating;
};


#endif 
