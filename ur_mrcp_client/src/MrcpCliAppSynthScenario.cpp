/*
    Author: @PraveenMS
    
    Speech synthesis session managment functions, they invoker wrapper functions. 

    Layers of function

    ------------------------------------
    | ur_mrcp_controller               | - interacts with business layer
    ------------------------------------
    | xGateMrcpCliAppHandler           | - interacts with controller later
    ------------------------------------
    | UniMrcpCliAppFw                  | - Framework of speech recognition
    ------------------------------------
    | MrcpCliAppSynthScenario          | - Store necessary infomation for the current session
    ------------------------------------
    | MrcpCliAppSynthSession           | - Responsible for session creation, modification and update
    ------------------------------------
    | WrapMrcpSession                  | - Wrapper functions for UniMrcp lib apis
    ------------------------------------
    | UniMRCP 1.7.0                    | - 3rd party library 
    ------------------------------------
    | SIP          | MRCPv2            |
    ------------------------------------

    Note: RTP is responsibility of MG. This client has RTP capability for testing purpose only. 
    To integrate TTS server with MG,  configure MG and RTP port range under profiles "RTP-Settings-1"
    and "RTP-Factory-1" in configuration file "unimrcpclient.xml"
    MRCP client will share this information to TTS server during sdp and mrcp negotition process. 
    
*/

#include <stdlib.h>
#include "MrcpCliAppSynthScenario.h"
#include "MrcpCliAppSynthSession.h"
#include "mrcp_message.h"
#include "mrcp_generic_header.h"
#include "mrcp_synth_header.h"
#include "mrcp_synth_resource.h"
#include "xglog.h"

MrcpCliAppSynthScenario::MrcpCliAppSynthScenario() :
	m_Speak(true),
	m_SpeechLanguage(NULL),
	m_ContentType("application/synthesis+ssml"),
	m_Content(NULL),
	m_ContentLength(0)
{
	XGLOG_FUNCTION_EN_EX("");
}

MrcpCliAppSynthScenario::~MrcpCliAppSynthScenario()
{
}

void MrcpCliAppSynthScenario::Destroy()
{
	XGLOG_FUNCTION_EN_EX("");
}

/*
  LoadSynth is called by framework layer for loading speech syntheis configurations  
*/
bool MrcpCliAppSynthScenario::LoadSynth(bool t_Speak, const char* t_SpeechLanguage, const char* t_ContentType, const char* t_Content, apr_pool_t* pool)
{
    XGLOG_FUNCTION_EN_EX("");
    m_pool = pool;
    m_Speak = t_Speak;
    m_SpeechLanguage = t_SpeechLanguage;
    m_ContentType = t_ContentType;
    m_Content = t_Content;    
    m_ContentLength = strlen(m_Content);
	
	return true;
}

/*
    creating speech session
*/
WrapMrcpSession* MrcpCliAppSynthScenario::CreateSession()
{
	XGLOG_FUNCTION_EN_EX("");
	return (WrapMrcpSession*)new MrcpCliAppSynthSession(this);    
}

/*
    load capabiilties, like code, sampling rate. it read from configuration file "unimrcpclient.xml"
*/
bool MrcpCliAppSynthScenario::LoadCapabilities()
{	
	/* Load Child Elements */
	m_pCapabilities = (mpf_codec_capabilities_t*) apr_palloc(m_pool,sizeof(mpf_codec_capabilities_t));
	mpf_codec_capabilities_init(m_pCapabilities,1,m_pool);

    /*load codec capabilities*/
    const char* pName = "LPCM";
	const char* pRates = "8000 16000"; 
    
	if(pName)
	{
		int rates = ParseRates(pRates,m_pool);
		mpf_codec_capabilities_add(m_pCapabilities,rates,pName);
	}

	return true;
}

/*
    parse the codecs configured in configuration file 
*/
int MrcpCliAppSynthScenario::ParseRates(const char* pStr, apr_pool_t* pool)
{
	int rates = 0;
	if(pStr)
	{
		char* pRateStr;
		char* pState;
		char* pRateListStr = apr_pstrdup(pool,pStr);
		do 
		{
			pRateStr = apr_strtok(pRateListStr, " ", &pState);
			if(pRateStr) 
			{
				apr_uint16_t rate = (apr_uint16_t)atoi(pRateStr);
				rates |= mpf_sample_rate_mask_get(rate);
			}
			pRateListStr = NULL; /* make sure we pass NULL on subsequent calls of apr_strtok() */
		} 
		while(pRateStr);
	}
	return rates;
}

/*
    load the capabilities into session
*/
bool MrcpCliAppSynthScenario::InitCapabilities(mpf_stream_capabilities_t* pCapabilities) const
{
	if(m_pCapabilities)
	{
		int i;
		mpf_codec_attribs_t *pAttribs;
		for(i=0; i<m_pCapabilities->attrib_arr->nelts; i++)
		{
			pAttribs = &APR_ARRAY_IDX(m_pCapabilities->attrib_arr,i,mpf_codec_attribs_t);
			mpf_codec_capabilities_add(
					&pCapabilities->codecs,
					pAttribs->sample_rates,
					pAttribs->name.buf);
		}
	}
	else
	{
		/* add default codec capabilities (Linear PCM) */
		mpf_codec_capabilities_add(
				&pCapabilities->codecs,
				MPF_SAMPLE_RATE_8000 | MPF_SAMPLE_RATE_16000,
				"LPCM");
	}

	return true;
}

apt_dir_layout_t* MrcpCliAppSynthScenario::GetDirLayout() const
{
	return m_pDirLayout;
}

const char* MrcpCliAppSynthScenario::GetSpeechLanguage() const
{
	return m_SpeechLanguage;
}

const char* MrcpCliAppSynthScenario::GetContentType() const
{
	return m_ContentType;
}

const char* MrcpCliAppSynthScenario::GetContent() const
{
	return m_Content;
}

apr_size_t MrcpCliAppSynthScenario::GetContentLength() const
{
	return m_ContentLength;
}

bool MrcpCliAppSynthScenario::IsSpeakEnabled() const
{
	return m_Speak;
}

bool MrcpCliAppSynthScenario::SetContent(const char* content)
{
	m_Content = content;
	m_ContentLength = strlen(m_Content); 
}

void MrcpCliAppSynthScenario::SetMrcpProfile(const char* pMrcpProfile)
{
	m_pMrcpProfile = pMrcpProfile;
}

void MrcpCliAppSynthScenario::SetDirLayout(apt_dir_layout_t* pDirLayout)
{
    m_pDirLayout = pDirLayout;
}

void MrcpCliAppSynthScenario::SetName(const char* pName)
{
    m_pName = pName;
}

