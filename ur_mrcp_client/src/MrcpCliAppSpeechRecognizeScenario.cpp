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

    Note: RTP is responsibility of MG. This client has RTP capability for testing purpose only
    To integrate STT server with MG,  configure MG and RTP port range under profiles "RTP-Settings-1"
    and "RTP-Factory-1" in configuration file "unimrcpclient.xml". MRCP client will share this information to 
    STT server during sdp and mrcp negotition process. 
    
*/


#include <stdlib.h>
#include "MrcpCliAppSpeechRecognizeScenario.h"
#include "MrcpCliAppSpeechRecognizeSession.h"
#include "mrcp_message.h"
#include "mrcp_generic_header.h"
#include "mrcp_recog_header.h"
#include "mrcp_recog_resource.h"
#include "apt_log.h"
#include "xglog.h"

MrcpCliAppSpeechRecognizeScenario::MrcpCliAppSpeechRecognizeScenario() :
	m_DefineGrammar(true),
	m_Recognize(true),
	m_ContentType("application/srgs+xml"),
	m_Content(NULL),
	m_ContentLength(0),
	m_AudioSource(NULL)
{
        XGLOG_FUNCTION_EN_EX("");
}

MrcpCliAppSpeechRecognizeScenario::~MrcpCliAppSpeechRecognizeScenario()
{
}

void MrcpCliAppSpeechRecognizeScenario::Destroy()
{
    XGLOG_FUNCTION_EN_EX("");
}

/*
    call this function from framework to load the configurations
*/
bool MrcpCliAppSpeechRecognizeScenario::LoadRecognize(bool t_Recognize, const char* t_ContentType, const char* t_Content, const char* t_AudioSource, apr_pool_t* pool)
{
    XGLOG_FUNCTION_EN_EX("");
    m_Recognize = t_Recognize;
    m_ContentType = t_ContentType;
    m_Content = t_Content;
    m_AudioSource = t_AudioSource;
    m_pool = pool; 
  
	return true;
}

/*
    call this function from framework to load the grammar configurations
*/

bool MrcpCliAppSpeechRecognizeScenario::LoadDefineGrammar(bool t_DefineGrammar, const char* t_ContentType, const char* t_Content)
{
    XGLOG_FUNCTION_EN_EX("");

    m_DefineGrammar = t_DefineGrammar;
    m_ContentType =  t_ContentType;
    m_Content = t_Content;
        
	return true;
}

/*
    call this function from framework to create the speech recognitions session
*/

WrapSpeechRecogMrcpSession* MrcpCliAppSpeechRecognizeScenario::CreateSession()
{
    XGLOG_FUNCTION_EN_EX("");
        return (WrapSpeechRecogMrcpSession*)new MrcpCliAppSpeechRecognizeSession(this);
}

/*
    Load the capabilities by reading configuration file "unimrcpclient.xml".
*/
bool MrcpCliAppSpeechRecognizeScenario::LoadCapabilities()
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
    parsing the codecs read from file
*/
int MrcpCliAppSpeechRecognizeScenario::ParseRates(const char* pStr, apr_pool_t* pool)
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
    initiaize the session with codec 
*/
bool MrcpCliAppSpeechRecognizeScenario::InitCapabilities(mpf_stream_capabilities_t* pCapabilities) const
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

apt_dir_layout_t* MrcpCliAppSpeechRecognizeScenario::GetDirLayout() const
{
        return m_pDirLayout;
}


const char* MrcpCliAppSpeechRecognizeScenario::GetContentType() const
{
	return m_ContentType;
}

const char* MrcpCliAppSpeechRecognizeScenario::GetContent() const
{
	return m_Content;
}

apr_size_t MrcpCliAppSpeechRecognizeScenario::GetContentLength() const
{
	return m_ContentLength;
}

const char* MrcpCliAppSpeechRecognizeScenario::GetAudioSource() const
{
	return m_AudioSource;
}

bool MrcpCliAppSpeechRecognizeScenario::IsDefineGrammarEnabled() const
{
	return m_DefineGrammar;
}

bool MrcpCliAppSpeechRecognizeScenario::IsRecognizeEnabled() const
{
	return m_Recognize;
}

void MrcpCliAppSpeechRecognizeScenario::SetMrcpProfile(const char* pMrcpProfile)
{
	m_pMrcpProfile = pMrcpProfile;
}

void MrcpCliAppSpeechRecognizeScenario::SetDirLayout(apt_dir_layout_t* pDirLayout)
{
    m_pDirLayout = pDirLayout;
}

void MrcpCliAppSpeechRecognizeScenario::SetName(const char* pName)
{
    m_pName = pName;
}


