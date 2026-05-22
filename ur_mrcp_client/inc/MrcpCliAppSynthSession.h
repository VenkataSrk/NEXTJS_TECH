/*
	@Author: PraveenMS
	Speech synthesis session object
*/

#ifndef SYNTH_SESSION_H
#define SYNTH_SESSION_H

#include "WrapMrcpSession.h"
#include "xGateUtil.h"

class MrcpCliAppSynthScenario;
struct SynthChannel;

class MrcpCliAppSynthSession : public WrapMrcpSession
{
public:
	MrcpCliAppSynthSession(const MrcpCliAppSynthScenario* pScenario);
	virtual ~MrcpCliAppSynthSession();
	bool SendSynthesisRequest(mrcp_channel_t* pMrcpChannel);

protected:
	virtual bool Start();
	virtual bool Stop();

	SynthChannel* CreateSynthChannel();

	mrcp_message_t* CreateSpeakRequest(mrcp_channel_t* pMrcpChannel);
	FILE* GetAudioOut(const mpf_codec_descriptor_t* pDescriptor, apr_pool_t* pool) const;

	virtual bool OnSessionTerminate(mrcp_sig_status_code_e status);
	virtual bool OnChannelAdd(mrcp_channel_t* pMrcpChannel, mrcp_sig_status_code_e status);
	virtual bool OnMessageReceive(mrcp_channel_t* pMrcpChannel, mrcp_message_t* pMrcpMessage);

	const MrcpCliAppSynthScenario* GetScenario() const;

private:
	SynthChannel* m_pSynthChannel;
};



#endif 
