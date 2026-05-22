#ifndef _XGATE_AUDIT_MANAGER_H
#define _XGATE_AUDIT_MANAGER_H

//ace includes
#include <ace/Task.h>
#include <ace/Singleton.h>
#include <ace/Hash_Map_Manager.h>
#include <ace/Null_Mutex.h>

//local includes
#include "xGateAuditMsg.h"
#include "xGateSchedulerMsg.h"
#include "xGateAuditHandler.h"
#include "xGateAuditScheduler.h"

#define AUDITMANAGER  AuditManager::instance()
typedef ACE_Hash_Map_Manager<ACE_TString, xGateAuditHandler*, ACE_SYNCH_RW_MUTEX> HASH_AUDITOR_MAP;
class xGateAuditManager
{
  public:
    xGateAuditManager();
    virtual ~xGateAuditManager(void);

    bool init();
    bool deinit();
    bool processAuditMsg(xGateAuditMsg *pMsg);
    bool processSchedulerMsg(xGateSchedulerMsg *pMsg);
  private:
    //member functions
    bool readMsg(xGateAuditMsg *pMsg);
    bool readMsg(xGateSchedulerMsg *pMsg);
    bool doAuditInit(xGateAuditMsg *pMsg);
    bool checkClientStatusResponse(xGateAuditMsg *pMsg);
    bool deleteAuditEntry();

    bool addAuditEntry();
    bool eraseAuditEntry();
    bool findAuditEntry();

    //member variables
    string m_uid;
    string m_channelId;
    xGateAuditHandler *m_pHandler;
    HASH_AUDITOR_MAP m_auditorMap;
};

typedef ACE_Unmanaged_Singleton<xGateAuditManager, ACE_Null_Mutex> AuditManager;
#endif
