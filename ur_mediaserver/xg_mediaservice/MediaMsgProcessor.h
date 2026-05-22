#ifndef _XGATE_MEDIA_PROCESSOR_H
#define _XGATE_MEDIA_PROCESSOR_H
#include <ace/Task.h>
#include <ace/Singleton.h>

//local includes
//local includes
#include "xGateUtil.h"
#include "MediaServiceUtil.h"
//#include "MediaServiceDefine.h"
#include "xGateMediaServiceMsg.h"

#define MEDIAMSGPROCESSOR MediaProcessor::instance()
class MediaMsgProcessor : public ACE_Task<ACE_MT_SYNCH> {
  public:  
    MediaMsgProcessor();
    ~MediaMsgProcessor(void);

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

typedef ACE_Unmanaged_Singleton<MediaMsgProcessor, ACE_Null_Mutex> MediaProcessor;

#endif
