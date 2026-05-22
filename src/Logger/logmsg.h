#ifndef _XGATE_LOG_MSG_H
#define _XGATE_LOG_MSG_H
//system includes
#include <string>
//ace includes
#include "../../utilities/CAUtilites.h"
//local includes
#include "CAlog.h"
#include "CAExceplog.h"

class xGateLogMsg : public ACE_Message_Block
{
  public:
    xGateLogMsg(LogLevel type, const char* fmt, const char* uid)
      : m_LogLevel(type), m_fmt(fmt), m_uid(uid)
    {
      this->msg_priority(0); //all message are in same priority FIFO manner
    }
    virtual ~xGateLogMsg(){};

    const LogLevel& getLogLevel(void) const {
      return m_LogLevel;
    }

    void setLogLevel(LogLevel type) {
      m_LogLevel = type;
    }

    std::string& getFmt(void) {
      return m_fmt;
    }

    std::string getUid(void) {
      return m_uid;
    }

  protected:
    xGateLogMsg& operator=(const xGateLogMsg&);

  private:
    xGateLogMsg(void);
    xGateLogMsg(const xGateLogMsg&);

    LogLevel m_LogLevel;
    std::string m_fmt;
    std::string m_uid;
};
#endif
