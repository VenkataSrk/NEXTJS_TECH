#ifndef _XGATE_MBCLINK_PROCESSOR_H
#define _XGATE_MBCLINK_PROCESSOR_H
#include <ace/Task.h>
#include <ace/Singleton.h>

//local includes
//local includes
#include "xGateUtil.h"
#include "MBCLinkServiceUtil.h"
#include "MBCLinkServiceDefine.h"
#include "xGateMBCLinkServiceMsg.h"

#define MBCLINKMSGPROCESSOR MBCLinkProcessor::instance()
class MBCLinkMsgProcessor : public ACE_Task<ACE_MT_SYNCH> {
  public:  
    MBCLinkMsgProcessor();
    ~MBCLinkMsgProcessor(void);

    bool init();
    virtual int svc(void);
    bool stop();

  private:
    bool m_run;

    bool handle_msg(ACE_Message_Block *pAmb);
    bool decode_msg(const char* data, size_t dataLen);
    bool create_doc_object(Document &doc, const char *jsonBuf);
    bool read_media_detail(const Value& val, MgMediaDetail& detail);
    bool read_sdpinfo(const Value& val, MgMediaDetail& detail); 
    bool read_audio_sdpinfo(const Value& val, MgMediaDetail& detail);
    bool read_video_sdpinfo(const Value& val, MgMediaDetail& detail);
    bool post(MgMediaDetail& detail);
};

typedef ACE_Unmanaged_Singleton<MBCLinkMsgProcessor, ACE_Null_Mutex> MBCLinkProcessor;

#endif
