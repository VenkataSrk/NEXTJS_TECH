#ifndef __SFU_CLIENT_H_
#define __SFU_CLIENT_H_

#include "ace/Synch_Traits.h"
#include <ace/Null_Condition.h>
#include <ace/Null_Mutex.h>

#include <ace/Reactor.h>
#include <ace/INET_Addr.h>
#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Connector.h>
#include <ace/Connector.h>
#include <ace/Svc_Handler.h>
#include <ace/Reactor_Notification_Strategy.h>
#include "xGateBaseMsg.h"
#include "xGateMgMsg.h"
#include "xGateUtil.h"

#define MG_IP_ADDR "10.22.2.62"
#define MG_PORT "30000"

typedef enum
{
  MSG_TYPE_SFU_ID =0,
  MSG_TYPE_JSON
}MsgType_t;

typedef struct MBC_SFU_MsgSt_t
{
    MsgType_t msg_type;
    int sfu_id;
    char msg[10000];
}MBC_SFU_MsgSt_t;

class SFUClient
{

    public:
    SFUClient ()
    {}


    // Called when input is available from the client.

    // Called when output is possible.

    // Called when a timer expires.
    int send_msg_to_mbc(char *msg, int len);
    xGateRetVal decode_media_msg(const char*,int);
    xGateRetVal create_doc_object(Document &doc, const char *jsonData, Value& dataMemObj);
    xGateRetVal handle_media_req(xGateMgMsgType mgMsgType, Value& dataMemObj, MgMediaDetail& clientMediaDetail);
    xGateRetVal handle_sdpinfo_process(const Value& sdpObj, MgMediaDetail& clientMediaDetail);
    xGateRetVal SFUClient::handle_participant_list(const Value& ParticipantListObj,
                                                   MgMediaDetail& clientMediaDetail);
    xGateRetVal SFUClient::handle_urmeet_userlist(const Value& ParticipantListObj,
                                                   MgMediaDetail& clientMediaDetail);
    xGateRetVal handle_sdpinfo_audio(const Value& sdpObj, MgMediaDetail& clientMediaDetail);
    xGateRetVal handle_sdpinfo_video(const Value& sdpObj, MgMediaDetail& clientMediaDetail);
    xGateRetVal post_media_detail(xGateMgMsg *pMgMsg);
    xGateRetVal send_msg(xGateMgMsg *pMgMsg);
    int create_media_msg(xGateMgMsg *pMgMsg, string &jsonBuff, int &gatewayId);
    Value setJsonParam(string strValue,Document &doc);
    Value setJsonParam(unsigned long iValue,Document &doc);
    Value setJsonParam(MgMediaDetail& media, Document &doc);
    Value setJsonParam_meetinfo(MgMediaDetail& media, Document &doc);
    bool set_sfu_id(int sfu_id);
    bool send_sfu_id(unsigned int);
    int get_sfu_id(void);
    bool get_mbc_msg(const char*,int len);
    private :
     int m_sfu_id; 
     unsigned int m_connection_id;
};

#endif /* __SFU_CLIENT_H_ */
