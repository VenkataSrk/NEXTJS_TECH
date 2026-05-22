#ifndef _XGATE_AUDIT_MSG_H
#define _XGATE_AUDIT_MSG_H

//local includes
//#include "xGateUtil.h"
#include "../inc/xGateBaseMsg.h"
#include "../inc/xGateDefine.h"
#include "xGateAuditDefine.h"

class xGateAuditMsg : public xGateBaseMsg {
 
 public:
   xGateAuditMsg(string& uid, string& channelId) : xGateBaseMsg(EN_XGATE_MSG_AUDIT), 
   m_uid(uid), m_channelId(channelId), m_auditMsgType(EN_AUDIT_MSG_UNKNOWN),
   m_auditState(EN_AUDIT_STATE_UNKNOWN) {
     m_auditMsgType = EN_AUDIT_MSG_UNKNOWN;
   }

    virtual ~xGateAuditMsg() {
    }

    string& getUid() {
      return m_uid;
    }

    void setUid(string& uid) {
      m_uid = uid;
    }

    string& getChannelId() {
      return m_channelId;
    }

    void setChannelId(string& channelId) {
      m_channelId = channelId;
    }

    xGateAuditMsgType getAuditMsgType() {
      return m_auditMsgType;
    }

    void setAuditMsgType(xGateAuditMsgType type) {
      m_auditMsgType = type;
    }

    AuditState getAuditState() {
      return m_auditState;
    }

    void setAuditState(AuditState state) {
      m_auditState = state;
    }

  private:

    xGateAuditMsg(const xGateAuditMsg&);
    xGateAuditMsg& operator=(const xGateAuditMsg&);

    string m_uid;
    string m_channelId;
    xGateAuditMsgType m_auditMsgType;
    AuditState m_auditState;
};

#endif

