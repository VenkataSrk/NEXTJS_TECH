#ifndef _XGATE_MEDIA_MSG_DISPATCHER_H
#define _XGATE_MEDIA_MSG_DISPATCHER_H
#include <ace/Singleton.h>

//local includes
#include "xGateUtil.h"
#include "NetIOServiceUtil.h"
#include "NetIOServiceDefine.h"
#include "xGateNetIOServiceMsg.h"

#define NETIOMSGDISPATCHER MsgDispatcher::instance()
class NetIOMsgDispatcher {
	public:  
		NetIOMsgDispatcher();
		~NetIOMsgDispatcher(void);

		bool init();
		bool process_msg(xGateNetIOServiceMsg *pNetIOMsg);

	private:

		bool fetch_fd(xGateNetIOServiceMsg *pNetIOMsg, std::string &remote_ip, unsigned short &remote_port, unsigned short &remote_fd);
		bool send_msg_on_tcp(xGateNetIOServiceMsg *pNetIOMsg, std::string &IpAddr, unsigned short &Port, unsigned short &Fd);

		unsigned short m_fd;

};
typedef ACE_Unmanaged_Singleton<NetIOMsgDispatcher, ACE_Null_Mutex> MsgDispatcher;
#endif
