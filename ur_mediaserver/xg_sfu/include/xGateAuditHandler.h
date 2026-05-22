#ifndef _XGATE_AUDIT_HANDLER_H
#define _XGATE_AUDIT_HANDLER_H

//ace includes
#include <ace/Date_Time.h>

//local includes
#include "xGateUtil.h"
#include "xGateAuditUtil.h"
#include "xGateSchedulerMsg.h"
#include "xGateAuditMsg.h"

class xGateAuditHandler
{
  public:
    xGateAuditHandler(string& uid, string& channelId);
    virtual ~xGateAuditHandler(void);

    bool init();
    bool deinit();
    void setAuditState(AuditState state);
    AuditState getAuditState();
    bool handleSchedulerEvent(xGateSchedulerMsg *pMsg);
    bool checkReauditRequired(xGateAuditMsg *pMsg);
    bool cancelAuditTimer();

  private:
    //member functions
    const char* getRegisteredDataTime();
    bool setAuditTimer(AuditTimerType timerType);

    //member variables
    string m_uid; //in mg it is a call_id of each party/leg
    string m_channelId; //in mg it is a mgresource_id of each party/leg
    AuditState m_currentAuditState;
    ACE_Date_Time m_registeredTime;
    ACE_Date_Time m_lastAuditTime;
    unsigned short m_auditInterval; //TODO: should be configurable
    unsigned short m_numOfAuditDone;
    unsigned short m_maxAuditAttempt;
    long int m_auditTimerId;
};
#endif
