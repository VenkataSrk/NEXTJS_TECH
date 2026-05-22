/*
	@Author: PraveenMS
	Speech synthesis configuration object
*/

#ifndef SYNTH_SCENARIO_H
#define SYNTH_SCENARIO_H

#include "WrapMrcpSession.h"
#include "xGateUtil.h"

class WrapMrcpSession;

class MrcpCliAppSynthScenario
{
public:
	MrcpCliAppSynthScenario();
	virtual ~MrcpCliAppSynthScenario();

	virtual void Destroy();
	virtual WrapMrcpSession* CreateSession();
	virtual bool LoadSynth (bool t_Speak, const char* t_SpeechLanguage, const char* t_ContentType, const char* t_Content, apr_pool_t* pool);

	const char* GetSpeechLanguage() const;
	const char* GetContentType() const;
	const char* GetContent() const;
	apr_size_t GetContentLength() const;

	bool IsSpeakEnabled() const;
	bool InitCapabilities(mpf_stream_capabilities_t* pCapabilities) const;
	apt_dir_layout_t* GetDirLayout() const;
	void SetMrcpProfile(const char* pMrcpProfile);
	void SetDirLayout(apt_dir_layout_t* pDirLayout);
	void SetName(const char* pName);
	bool SetContent(const char* content);

protected:
	bool LoadCapabilities();
	int ParseRates(const char* pStr, apr_pool_t* pool);

/* SYNTH DATA */
	bool        m_Speak;
	const char* m_SpeechLanguage;
	const char* m_ContentType;
	const char* m_Content;
	apr_size_t  m_ContentLength;
	
/* CODEC DATA  */
	const char*                       m_pName;
	const char*                       m_pMrcpProfile;
	apt_dir_layout_t*                 m_pDirLayout;

	bool                              m_ResourceDiscovery;
	mpf_codec_capabilities_t*         m_pCapabilities;
	mpf_rtp_termination_descriptor_t* m_pRtpDescriptor;
	apr_pool_t* 					  m_pool;
};


#endif 
