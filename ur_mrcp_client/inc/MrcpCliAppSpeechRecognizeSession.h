/*
	@Author: PraveenMS
	Speech recognition session object
*/


#ifndef RECOG_SESSION_H
#define RECOG_SESSION_H

#include "WrapSpeechRecogMrcpSession.h"
#include "xGateUtil.h"

class MrcpCliAppSpeechRecognizeScenario;
struct RecogChannel;

class MrcpCliAppSpeechRecognizeSession : public WrapSpeechRecogMrcpSession
{
public:
	MrcpCliAppSpeechRecognizeSession(const MrcpCliAppSpeechRecognizeScenario* pScenario);
	virtual ~MrcpCliAppSpeechRecognizeSession();
	bool StartRecognition(mrcp_channel_t* pMrcpChannel);

protected:
	virtual bool Start();
	virtual bool Stop();

	RecogChannel* CreateRecogChannel();
	bool OnDefineGrammar(mrcp_channel_t* pMrcpChannel);

	mrcp_message_t* CreateDefineGrammarRequest(mrcp_channel_t* pMrcpChannel);
	mrcp_message_t* CreateRecognizeRequest(mrcp_channel_t* pMrcpChannel);

	static bool ParseNLSMLResult(mrcp_message_t* pMrcpMessage);
	FILE* GetAudioIn(const mpf_codec_descriptor_t* pDescriptor, apr_pool_t* pool) const;

	virtual bool OnSessionTerminate(mrcp_sig_status_code_e status);
	virtual bool OnChannelAdd(mrcp_channel_t* pMrcpChannel, mrcp_sig_status_code_e status);
	virtual bool OnMessageReceive(mrcp_channel_t* pMrcpChannel, mrcp_message_t* pMrcpMessage);

	const MrcpCliAppSpeechRecognizeScenario* GetSpeechRecognizeScenario() const;
	std::string decode_converted_text(std::string xml_str);

private:
	
	RecogChannel* m_pRecogChannel;
	const char*   m_ContentId;
};



#endif
