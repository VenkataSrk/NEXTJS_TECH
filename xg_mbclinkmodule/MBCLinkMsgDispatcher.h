#ifndef _XGATE_MBCLINK_MSG_DISPATCHER_H
#define _XGATE_MBCLINK_MSG_DISPATCHER_H
#include <ace/Singleton.h>

//local includes
#include "xGateUtil.h"
#include "MBCLinkServiceUtil.h"
#include "MBCLinkServiceDefine.h"
#include "xGateMBCLinkServiceMsg.h"

#define MBCLINKMSGDISPATCHER MsgDispatcher::instance()
class MBCLinkDispatcher {
  public:  
    MBCLinkDispatcher();
    ~MBCLinkDispatcher(void);
    
    bool init();
    bool process_msg(xGateMBCLinkServiceMsg *pMsg);

  private:
    Document m_doc;
    StringBuffer m_strBuf;
    string m_uid;
    unsigned short m_fd;
	string m_strMsgTypeName;

    void reset();
    bool encode_msg(xGateMBCLinkServiceMsg *pMsg);
		Value setSdpInfoParam(MgMediaDetail& media,Document &doc);
    bool fill_media_detail(Value& val, MgMediaDetail& detail);
    bool fill_sdpinfo(Value& val, MgMediaDetail& detail);
    Value fill_audio_sdpinfo(MgMediaDetail& detail,Document &doc,Document::AllocatorType& allocate);
    Value fill_video_sdpinfo(MgMediaDetail& detail,Document &doc,Document::AllocatorType& allocate);
    Value setParticipantInfoParam(MgMediaDetail& detail,Document &m_doc);
		Value setURMeetUserListParam(MgMediaDetail& detail,Document &m_doc);
    bool fetch_fd(xGateMBCLinkServiceMsg *pMsg);
    bool send_msg_on_tcp();
};

typedef ACE_Unmanaged_Singleton<MBCLinkDispatcher, ACE_Null_Mutex> MsgDispatcher;

#endif
