/*
    Author: @PraveenMS
    
    Speech recognition session managment functions, they invoker wrapper functions. 

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


#include "WrapSpeechRecogMrcpSession.h"
#include "mrcp_message.h"
#include "apt_pool.h"
#include "xglog.h"
#include <sstream>

std::string WrapSpeechRecogMrcpSession::CreateUniqueSessionId()
{
  std::string UniqueCallid = "";

  std::srand(static_cast<unsigned int>(std::time(NULL)));
  ostringstream t_uuid;
  unsigned long int uid =  std::rand();
  t_uuid << uid++;
  UniqueCallid = t_uuid.str();

  XGLOG_INFO("xGateDbConfigMgr::CreateUniqueCallid(%s)",UniqueCallid.c_str());

  return UniqueCallid;
}

WrapSpeechRecogMrcpSession::WrapSpeechRecogMrcpSession(const MrcpCliAppSpeechRecognizeScenario* pRecogScenario) :
	m_pRecogScenario(pRecogScenario),
	m_pMrcpProfile(NULL),
	m_pMethodProvider(NULL),
	m_pMrcpApplication(NULL),
	m_pMrcpSession(NULL),
	m_pMrcpMessage(NULL),
	m_Running(false),
	isEOS(false),
	isRecogCompleted(false),
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

WrapSpeechRecogMrcpSession::~WrapSpeechRecogMrcpSession()
{
	apr_pool_destroy(m_Pool);
}

bool WrapSpeechRecogMrcpSession::Run()
{
	XGLOG_FUNCTION_EN_EX("");
	if(m_Running) {
		XGLOG_ERROR("WrapSpeechRecogMrcpSession::Run already running");
		return false;
	}
	/*
	if(!m_pMrcpProfile)
		m_pMrcpProfile = m_pRecogScenario->GetMrcpProfile();
	*/
	if(!m_pMrcpProfile || !m_pMrcpApplication){
		XGLOG_ERROR("WrapSpeechRecogMrcpSession::Run failed 1");
		return false;
	}

	/* create session */
	if(!CreateMrcpSession(m_pMrcpProfile)){
		XGLOG_ERROR("WrapSpeechRecogMrcpSession::Run failed 2");
		return false;
	}
	
	m_Running = true;
	
	bool ret = false;
	/*	
	if(m_pRecogScenario->IsDiscoveryEnabled())
		ret = DiscoverResources();
	else*/
		ret = Start();

	if(!ret)
	{
		XGLOG_ERROR("WrapSpeechRecogMrcpSession::Run Start failed");
		m_Running = false;
		DestroyMrcpSession();
	}
	return ret;
}

bool WrapSpeechRecogMrcpSession::Stop()
{
	XGLOG_FUNCTION_EN_EX("");
	if(m_Terminating)
		return false;

	return true;
}

bool WrapSpeechRecogMrcpSession::Terminate()
{
	XGLOG_FUNCTION_EN_EX("");
	if(m_Terminating)
		return false;

	m_Running = false;
	m_Terminating = true;
	return (mrcp_application_session_terminate(m_pMrcpSession) == TRUE);
}

bool WrapSpeechRecogMrcpSession::OnSessionTerminate(mrcp_sig_status_code_e status)
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

bool WrapSpeechRecogMrcpSession::OnSessionUpdate(mrcp_sig_status_code_e status)
{
	XGLOG_FUNCTION_EN_EX("");
	return m_Running;
}

bool WrapSpeechRecogMrcpSession::OnChannelAdd(mrcp_channel_t* pMrcpChannel, mrcp_sig_status_code_e status)
{
	XGLOG_FUNCTION_EN_EX("");
	return m_Running;
}

bool WrapSpeechRecogMrcpSession::OnChannelRemove(mrcp_channel_t* pMrcpChannel, mrcp_sig_status_code_e status)
{
	XGLOG_FUNCTION_EN_EX("");
	return m_Running;
}

bool WrapSpeechRecogMrcpSession::OnMessageReceive(mrcp_channel_t* pMrcpChannel, mrcp_message_t* pMrcpMessage)
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

bool WrapSpeechRecogMrcpSession::OnTerminateEvent(mrcp_channel_t* pMrcpChannel)
{
	XGLOG_FUNCTION_EN_EX("");
	if(!m_Running)
		return false;

	return Terminate();
}

bool WrapSpeechRecogMrcpSession::OnResourceDiscover(mrcp_session_descriptor_t* pDescriptor, mrcp_sig_status_code_e status)
{
	XGLOG_FUNCTION_EN_EX("");
	if(!m_Running)
		return false;

	if(!Start())
		Terminate();
	return true;
}

bool WrapSpeechRecogMrcpSession::CreateMrcpSession(const char* pProfileName)
{
	XGLOG_FUNCTION_EN_EX("");
	m_pMrcpSession = mrcp_application_session_create_ex(m_pMrcpApplication,pProfileName,this,FALSE,m_Pool);
	if(!m_pMrcpSession)
		return false;
        //std::string uniqueId = CreateUniqueSessionId();
        //m_Id = uniqueId.c_str();
	XGLOG_INFO("WrapSpeechRecogMrcpSession::CreateMrcpSession m_Id (%s)",m_Id);
//	mrcp_application_session_name_set(m_pMrcpSession,m_tSdpDescriptor->m_callId.c_str());
	mrcp_application_session_name_set(m_pMrcpSession,sessionId.c_str());
	#if 0
	const char* name = apr_psprintf(m_Pool,"umc-%s",m_Id);
	mrcp_application_session_name_set(m_pMrcpSession,name);
	#endif
	return true;
}

bool WrapSpeechRecogMrcpSession::DestroyMrcpSession()
{
	XGLOG_FUNCTION_EN_EX("");
	if(!m_pMrcpSession)
		return false;

	mrcp_application_session_destroy(m_pMrcpSession);
	m_pMrcpSession = NULL;
	return true;
}

bool WrapSpeechRecogMrcpSession::AddMrcpChannel(mrcp_channel_t* pMrcpChannel)
{
	XGLOG_FUNCTION_EN_EX("");
	if(!m_Running)
		return false;

	return (mrcp_application_channel_add(m_pMrcpSession,pMrcpChannel) == TRUE);
}

bool WrapSpeechRecogMrcpSession::RemoveMrcpChannel(mrcp_channel_t* pMrcpChannel)
{
	XGLOG_FUNCTION_EN_EX("");
	if(!m_Running)
		return false;

	return (mrcp_application_channel_remove(m_pMrcpSession,pMrcpChannel) == TRUE);
}

bool WrapSpeechRecogMrcpSession::SendMrcpRequest(mrcp_channel_t* pMrcpChannel, mrcp_message_t* pMrcpMessage)
{
	XGLOG_FUNCTION_EN_EX("");
	if(!m_Running)
		return false;

	m_pMrcpMessage = pMrcpMessage;
	return (mrcp_application_message_send(m_pMrcpSession,pMrcpChannel,pMrcpMessage) == TRUE);
}

bool WrapSpeechRecogMrcpSession::DiscoverResources()
{
	XGLOG_FUNCTION_EN_EX("");
	if(!m_Running)
		return false;

	return (mrcp_application_resource_discover(m_pMrcpSession) == TRUE);
}

mrcp_channel_t* WrapSpeechRecogMrcpSession::CreateMrcpChannel(
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

mpf_termination_t* WrapSpeechRecogMrcpSession::CreateAudioTermination(
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

mrcp_message_t* WrapSpeechRecogMrcpSession::CreateMrcpMessage(
		mrcp_channel_t* pMrcpChannel, 
		mrcp_method_id method_id)
{
	return mrcp_application_message_create(m_pMrcpSession,pMrcpChannel,method_id);
}

const char* WrapSpeechRecogMrcpSession::GetMrcpSessionId() const
{
	XGLOG_FUNCTION_EN_EX("");
	if(!m_pMrcpSession)
		return NULL;

	const apt_str_t *pId = mrcp_application_session_id_get(m_pMrcpSession);
	return pId->buf;
}

const MrcpCliAppSpeechRecognizeScenario* WrapSpeechRecogMrcpSession::GetSpeechRecognizeScenario() const
{
	return m_pRecogScenario;
}

const char* WrapSpeechRecogMrcpSession::GetId() const
{
	return m_Id;
}

apr_pool_t* WrapSpeechRecogMrcpSession::GetSessionPool() const
{
	return m_Pool;
}

void WrapSpeechRecogMrcpSession::SetMrcpApplication(mrcp_application_t* pMrcpApplication)
{
	m_pMrcpApplication = pMrcpApplication;
}

void WrapSpeechRecogMrcpSession::SetMrcpProfile(const char* pMrcpProfile)
{
	m_pMrcpProfile = pMrcpProfile;
}


void WrapSpeechRecogMrcpSession::SetMethodProvider(UmcRecogSessionMethodProvider* pMethodProvider)
{
	m_pMethodProvider = pMethodProvider;
}

mrcp_message_t* WrapSpeechRecogMrcpSession::GetMrcpMessage() const
{
	return m_pMrcpMessage;
}

