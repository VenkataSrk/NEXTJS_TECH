#ifndef _XGATE_MEDIA_MSG_DISPATCHER_H
#define _XGATE_MEDIA_MSG_DISPATCHER_H
#include <ace/Singleton.h>

//local includes
#include "xGateUtil.h"
#include "MediaServiceUtil.h"
//#include "MediaServiceDefine.h"
#include "xGateMediaServiceMsg.h"

#define MEDIAMSGDISPATCHER MsgDispatcher::instance()
class MediaMsgDispatcher : public ACE_Task<ACE_MT_SYNCH> {
  public:  
    MediaMsgDispatcher();
    ~MediaMsgDispatcher(void);
    virtual int svc(void);
    bool stop();
    bool init();
    bool process_msg(ACE_Message_Block *pAmb);

  private:
    Document m_doc;
    StringBuffer m_strBuf;
    string m_uid;
    bool m_run;
    unsigned short m_fd;
    string m_strMsgTypeName;
    void reset();
    bool parseJson(const char*,Document &);
    Value setJsonParam(const string&,Document &);
    Value setJsonParam(unsigned int);   
    bool encode_msg(xGateMediaServiceMsg *pMsg);
    bool fill_media_detail(Value& val, MgMediaDetail& detail);
    bool fill_sdpinfo(Value& val, MgMediaDetail& detail);
    bool fill_audio_sdpinfo(Value& val, MgMediaDetail& detail);
    bool fetch_fd(xGateMediaServiceMsg *pMsg);
    bool send_msg_on_tcp(xGateMediaServiceMsg *pMsg);
};

typedef ACE_Unmanaged_Singleton<MediaMsgDispatcher, ACE_Null_Mutex> MsgDispatcher;

#endif
