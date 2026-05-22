#pragma once

//ace includes
#include <ace/Message_Block.h>

//local includes
#include "xGateUtil.h"
#include "xGateDBHandler.h"


class xGateDBServiceMsg : public xGateBaseMsg
{
  public:
    xGateDBServiceMsg(void) : xGateBaseMsg(IURDefines::UR_MODULE_DB_SERVICE)
    {
        m_dbMsgType = DB_SRV_MSG_REQ;
    };

    virtual ~xGateDBServiceMsg(){};

    DBRequestInfo& get_db_request_info() {
      return m_dbReqInfo;
    }

    void set_db_request_info(DBRequestInfo &dbReqInfo) {
      m_dbReqInfo = dbReqInfo;
    }
    typedef enum db_srv_msg_type
    {
      DB_SRV_MSG_REQ = 0xfff1,
      DB_SRV_MSG_RES = 0xfff2
    } DB_SRV_MSG;

    void setDbMsgType(DB_SRV_MSG msgType)
    {
      m_dbMsgType = msgType;
    }
    DB_SRV_MSG getDbMsgType() const
    {
      return m_dbMsgType;
    }

  protected:
    xGateDBServiceMsg& operator=(const xGateDBServiceMsg&);

  private:
    // Not Implemented Methods
    xGateDBServiceMsg(const xGateDBServiceMsg&);
    DBRequestInfo m_dbReqInfo;
    DB_SRV_MSG m_dbMsgType;
};

