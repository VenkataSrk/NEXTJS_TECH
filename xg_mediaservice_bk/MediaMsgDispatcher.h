#ifndef _XGATE_MEDIA_MSG_DISPATCHER_H
#define _XGATE_MEDIA_MSG_DISPATCHER_H
#include <ace/Singleton.h>

//local includes
#include "xGateUtil.h"
#include "MediaServiceUtil.h"
#include "MediaServiceDefine.h"
#include "xGateMediaServiceMsg.h"

#define MEDIAMSGDISPATCHER MsgDispatcher::instance()
class MediaMsgDispatcher {
  public:  
    MediaMsgDispatcher();
    ~MediaMsgDispatcher(void);
    
    bool init();
    bool process_msg(xGateMediaServiceMsg *pMsg);

  private:
    Document m_doc;
    StringBuffer m_strBuf;
    string m_uid;
    unsigned short m_fd;
	string m_strMsgTypeName;

    void reset();
    bool encode_msg(xGateMediaServiceMsg *pMsg);
    bool fill_media_detail(Value& val, MgMediaDetail& detail);
    bool fill_sdpinfo(Value& val, MgMediaDetail& detail);
    bool fill_audio_sdpinfo(Value& val, MgMediaDetail& detail);
    bool fetch_fd(xGateMediaServiceMsg *pMsg);
    bool send_msg_on_tcp();
};

typedef ACE_Unmanaged_Singleton<MediaMsgDispatcher, ACE_Null_Mutex> MsgDispatcher;

#endif
