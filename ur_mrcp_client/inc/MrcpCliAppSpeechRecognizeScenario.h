/*
	@Author: PraveenMS
	Speech recognition configuration object
*/

#ifndef RECOG_SCENARIO_H
#define RECOG_SCENARIO_H

#include "WrapSpeechRecogMrcpSession.h"
#include "xGateUtil.h"

class WrapSpeechRecogMrcpSession;

class MrcpCliAppSpeechRecognizeScenario 
{
public:
	MrcpCliAppSpeechRecognizeScenario();
	virtual ~MrcpCliAppSpeechRecognizeScenario();
	virtual void Destroy();
	bool LoadRecognize(bool t_Recognize, const char* t_ContentType, const char* t_Content, const char* t_AudioSource, apr_pool_t* pool);
	bool LoadDefineGrammar(bool t_DefineGrammar, const char* t_ContentType, const char* t_Content);
	virtual WrapSpeechRecogMrcpSession* CreateSession();	
	const char* GetContentType() const;
	const char* GetContent() const;
	apr_size_t GetContentLength() const;
	const char* GetAudioSource() const;

	bool IsDefineGrammarEnabled() const;
	bool IsRecognizeEnabled() const;

        bool LoadCapabilities();
        int ParseRates(const char* pStr, apr_pool_t* pool);
        bool InitCapabilities(mpf_stream_capabilities_t* pCapabilities) const;
	apt_dir_layout_t* GetDirLayout() const;
	void SetDirLayout(apt_dir_layout_t* pDirLayout);
	void SetName(const char* pName);
	void SetMrcpProfile(const char* pMrcpProfile);
	
protected:

	bool        m_DefineGrammar;
	bool        m_Recognize;
	const char* m_ContentType;
	const char* m_Content;
	apr_size_t  m_ContentLength;
	const char* m_AudioSource;

	apt_dir_layout_t*                 m_pDirLayout;
	mpf_codec_capabilities_t*         m_pCapabilities;
	apr_pool_t*                                       m_pool;
	const char* m_pName;
	const char* m_pMrcpProfile;
};


#endif 
