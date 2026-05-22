#ifndef _XGATE_MEDIA_PROCESSOR_H
#define _XGATE_MEDIA_PROCESSOR_H
#include <ace/Task.h>
#include <ace/Singleton.h>

//local includes
//local includes
#include "xGateUtil.h"
#include "NetIOServiceUtil.h"
#include "NetIOServiceDefine.h"
#include "xGateNetIOServiceMsg.h"

#define NETIOMSGPROCESSOR NetIOProcessor::instance()
class NetIOMsgProcessor : public ACE_Task<ACE_MT_SYNCH> {
  public:  
    NetIOMsgProcessor();
    ~NetIOMsgProcessor(void);

    bool init();
    virtual int svc(void);
    bool stop();

  private:
    bool m_run;

    bool handle_msg(ACE_Message_Block *pAmb);
    //bool decode_msg(const char* data, size_t dataLen);
    //bool create_doc_object(Document &doc, const char *jsonBuf);
    //bool read_media_detail(const Value& val, MgMediaDetail& detail);
    //bool read_sdpinfo(const Value& val, MgMediaDetail& detail); 
    //bool read_audio_sdpinfo(const Value& val, MgMediaDetail& detail);
    //bool read_video_sdpinfo(const Value& val, MgMediaDetail& detail);
    //bool post(MgMediaDetail& detail);
};

typedef ACE_Unmanaged_Singleton<NetIOMsgProcessor, ACE_Null_Mutex> NetIOProcessor;

#endif
