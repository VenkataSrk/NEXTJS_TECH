#ifndef _URMODULE_BRIDGE_MSG_H
#define _URMODULE_BRIDGE_MSG_H

//local includes
#include "xGateBaseMsg.h"

class URModuleBridgeMsg : public xGateBaseMsg
{
public:
   URModuleBridgeMsg() : xGateBaseMsg(EN_XGATE_MSG_TO_HTTP),
                         m_strTopicName(""), m_strMsgBody(""){};

   ~URModuleBridgeMsg()
   {
   }

   void set_topic_name(std::string topicName)
   {
      m_strTopicName = topicName.c_str();
   }

   string get_topic_name()
   {
      return m_strTopicName.c_str();
   }

   void set_msg(std::string msg)
   {
      m_strMsgBody = msg.c_str();
   }

   string get_msg()
   {
      return m_strMsgBody.c_str();
   }

private:
   std::string m_strTopicName;
   std::string m_strMsgBody;
};
#endif
