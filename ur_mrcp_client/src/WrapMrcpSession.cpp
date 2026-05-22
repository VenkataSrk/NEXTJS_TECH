/*
    Author: @PraveenMS
    
    Speech synthesis session managment functions, they invoker wrapper functions. 

    Layers of function

    ------------------------------------
    | ur_mrcp_controller               | - interacts with business layer
    ------------------------------------
    | xGateMrcpCliAppHandler           | - interacts with controller later
    ------------------------------------
    | UniMrcpCliAppFwSpeechRecog       | - Framework of speech recognition
    ------------------------------------
    | MrcpCliAppSpeechRecognizeScenario| - Store necessary infomation for the current session
    ------------------------------------
    | MrcpCliAppSpeechRecognizeSession | - Responsible for session creation, modification and update
    ------------------------------------
    | WrapSpeechRecogMrcpSession       | - Wrapper functions for UniMrcp lib apis
    ------------------------------------
    | UniMRCP 1.7.0                    | - 3rd party library 
    ------------------------------------
    | SIP          | MRCPv2            |
    ------------------------------------

    This layer holds wrapper functions for unimrcp library,
    do not modify unless it is necessary. 

    Note: RTP is responsibility of MG. This client has RTP capability for testing purpose only
    To integrate STT server with MG,  configure MG and RTP port range under profiles "RTP-Settings-1"
    and "RTP-Factory-1" in configuration file "unimrcpclient.xml". MRCP client will share this information to 
    STT server during sdp and mrcp negotition process. 
    
*/


#include "WrapMrcpSession.h"
#include "mrcp_message.h"
#include "apt_pool.h"
#include "xglog.h"
#include <sstream>

WrapMrcpSession::WrapMrcpSession(const MrcpCliAppSynthScenario* pScenario) :
	m_pScenario(pScenario),
	m_pMrcpProfile(NULL),
	m_pMethodProvider(NULL),
	m_pMrcpApplication(NULL),
	m_pMrcpSession(NULL),
	m_pMrcpMessage(NULL),
	m_Running(false),
	m_Terminating(false)
{
	XGLOG_FUNCTION_EN_EX("");
	static int id = 0;
	if(id == INT_MAX)
		id = 0;
	id++;

	m_Pool = apt_pool_create();
	m_Id = apr_psprintf(m_Pool,"%d",id);
}

WrapMrcpSession::~WrapMrcpSession()
{
	apr_pool_destroy(m_Pool);
}

bool WrapMrcpSession::Run()
{
	XGLOG_FUNCTION_EN_EX("");
	if(m_Running)
		return false;
	/*
	if(!m_pMrcpProfile)
		m_pMrcpProfile = m_pScenario->GetMrcpProfile();
	*/
	if(!m_pMrcpProfile || !m_pMrcpApplication)
		return false;

	/* create session */
	if(!CreateMrcpSession(m_pMrcpProfile))
		return false;
	
	m_Running = true;
	
	bool ret = false;
	/*	
	if(m_pScenario->IsDiscoveryEnabled())
		ret = DiscoverResources();
	else*/
		ret = Start();

	if(!ret)
	{
		m_Running = false;
		DestroyMrcpSession();
	}
	return ret;
}

bool WrapMrcpSession::Stop()
{
	XGLOG_FUNCTION_EN_EX("");
	if(m_Terminating)
		return false;

	return true;
}

bool WrapMrcpSession::Terminate()
{
	XGLOG_FUNCTION_EN_EX("");
	if(m_Terminating)
		return false;

	m_Running = false;
	m_Terminating = true;
	return (mrcp_application_session_terminate(m_pMrcpSession) == TRUE);
}

bool WrapMrcpSession::OnSessionTerminate(mrcp_sig_status_code_e status)
{
	XGLOG_FUNCTION_EN_EX("");
	if(!m_Terminating)
		return false;

	m_Terminating = false;
	DestroyMrcpSession();
	if(m_pMethodProvider)
		m_pMethodProvider->ExitSession(this);
	return true;
}

bool WrapMrcpSession::OnSessionUpdate(mrcp_sig_status_code_e status)
{
	XGLOG_FUNCTION_EN_EX("");
	return m_Running;
}

bool WrapMrcpSession::OnChannelAdd(mrcp_channel_t* pMrcpChannel, mrcp_sig_status_code_e status)
{
	XGLOG_FUNCTION_EN_EX("");
	return m_Running;
}

bool WrapMrcpSession::OnChannelRemove(mrcp_channel_t* pMrcpChannel, mrcp_sig_status_code_e status)
{
	XGLOG_FUNCTION_EN_EX("");
	return m_Running;
}

bool WrapMrcpSession::OnMessageReceive(mrcp_channel_t* pMrcpChannel, mrcp_message_t* pMrcpMessage)
{
	XGLOG_FUNCTION_EN_EX("");
	if(!m_Running)
		return false;

	if(!m_pMrcpMessage)
		return false;

	/* match request identifiers */
	if(m_pMrcpMessage->start_line.request_id != pMrcpMessage->start_line.request_id)
		return false;

	return true;
}

bool WrapMrcpSession::OnTerminateEvent(mrcp_channel_t* pMrcpChannel)
{
	XGLOG_FUNCTION_EN_EX("");
	if(!m_Running)
		return false;

	return Terminate();
}

bool WrapMrcpSession::OnResourceDiscover(mrcp_session_descriptor_t* pDescriptor, mrcp_sig_status_code_e status)
{
	XGLOG_FUNCTION_EN_EX("");
	if(!m_Running)
		return false;

	if(!Start())
		Terminate();
	return true;
}

bool WrapMrcpSession::CreateMrcpSession(const char* pProfileName)
{
	XGLOG_FUNCTION_EN_EX("");
	m_pMrcpSession = mrcp_application_session_create_ex(m_pMrcpApplication,pProfileName,this,FALSE,m_Pool);
	if(!m_pMrcpSession)
		return false;

//	const char* name = apr_psprintf(m_Pool,"umc-%s",m_Id);
	mrcp_application_session_name_set(m_pMrcpSession,sessionId.c_str());
	return true;
}

bool WrapMrcpSession::DestroyMrcpSession()
{
	XGLOG_FUNCTION_EN_EX("");
	if(!m_pMrcpSession)
		return false;

	mrcp_application_session_destroy(m_pMrcpSession);
	m_pMrcpSession = NULL;
	return true;
}

bool WrapMrcpSession::AddMrcpChannel(mrcp_channel_t* pMrcpChannel)
{
	XGLOG_FUNCTION_EN_EX("");
	if(!m_Running)
		return false;

	return (mrcp_application_channel_add(m_pMrcpSession,pMrcpChannel) == TRUE);
}

bool WrapMrcpSession::RemoveMrcpChannel(mrcp_channel_t* pMrcpChannel)
{
	XGLOG_FUNCTION_EN_EX("");
	if(!m_Running)
		return false;

	return (mrcp_application_channel_remove(m_pMrcpSession,pMrcpChannel) == TRUE);
}

bool WrapMrcpSession::SendMrcpRequest(mrcp_channel_t* pMrcpChannel, mrcp_message_t* pMrcpMessage)
{
	XGLOG_FUNCTION_EN_EX("");
	if(!m_Running)
		return false;

	m_pMrcpMessage = pMrcpMessage;
	return (mrcp_application_message_send(m_pMrcpSession,pMrcpChannel,pMrcpMessage) == TRUE);
}

bool WrapMrcpSession::DiscoverResources()
{
	XGLOG_FUNCTION_EN_EX("");
	if(!m_Running)
		return false;

	return (mrcp_application_resource_discover(m_pMrcpSession) == TRUE);
}

mrcp_channel_t* WrapMrcpSession::CreateMrcpChannel(
						mrcp_resource_id resource_id, 
						mpf_termination_t* pTermination, 
						mpf_rtp_termination_descriptor_t* pRtpDescriptor, 
						void* pObj)
{
	XGLOG_FUNCTION_EN_EX("");
	return mrcp_application_channel_create(
			m_pMrcpSession,            /* session, channel belongs to */
			resource_id,               /* MRCP resource identifier */
			pTermination,              /* media termination, used to terminate audio stream */
			pRtpDescriptor,                      /* RTP descriptor, used to create RTP termination (NULL by default) */
			pObj);                     /* object to associate */
}

mpf_termination_t* WrapMrcpSession::CreateAudioTermination(
						const mpf_audio_stream_vtable_t* pStreamVtable,
						mpf_stream_capabilities_t* pCapabilities,
						void* pObj)
{
	XGLOG_FUNCTION_EN_EX("");
	return mrcp_application_audio_termination_create(
			m_pMrcpSession,  /* session, termination belongs to */
			pStreamVtable,   /* virtual methods table of audio stream */
			pCapabilities,   /* capabilities of audio stream */
			pObj);           /* object to associate */
}

mrcp_message_t* WrapMrcpSession::CreateMrcpMessage(
		mrcp_channel_t* pMrcpChannel, 
		mrcp_method_id method_id)
{
	return mrcp_application_message_create(m_pMrcpSession,pMrcpChannel,method_id);
}

const char* WrapMrcpSession::GetMrcpSessionId() const
{
	XGLOG_FUNCTION_EN_EX("");
	if(!m_pMrcpSession)
		return NULL;

	const apt_str_t *pId = mrcp_application_session_id_get(m_pMrcpSession);
	return pId->buf;
}

const MrcpCliAppSynthScenario* WrapMrcpSession::GetScenario() const
{
	return m_pScenario;
}

const char* WrapMrcpSession::GetId() const
{
	return m_Id;
}

apr_pool_t* WrapMrcpSession::GetSessionPool() const
{
	return m_Pool;
}

void WrapMrcpSession::SetMrcpApplication(mrcp_application_t* pMrcpApplication)
{
	m_pMrcpApplication = pMrcpApplication;
}

void WrapMrcpSession::SetMrcpProfile(const char* pMrcpProfile)
{
	m_pMrcpProfile = pMrcpProfile;
}


void WrapMrcpSession::SetMethodProvider(UmcSessionMethodProvider* pMethodProvider)
{
	m_pMethodProvider = pMethodProvider;
}

mrcp_message_t* WrapMrcpSession::GetMrcpMessage() const
{
	return m_pMrcpMessage;
}

std::string WrapMrcpSession::FrameXml(std::string text)
{
	XGLOG_FUNCTION_EN_EX("");
	std::ostringstream oss;
	oss<< "<?xml version=\"1.0\"?>\n<speak version=\"1.1\" xml:lang=\"en-UK\">\n    <voice name=\"Alice\">\n        ";
		oss<<"<s> "<<text<<" </s>\n    </voice>\n</speak>";
	return oss.str();
}

