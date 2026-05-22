#include "xglog.h"
#include "NetIOMsgDispatcher.h"
#include "xGateMultiSessionTracker.h"
#include <map>

NetIOMsgDispatcher::NetIOMsgDispatcher() 
{
}

NetIOMsgDispatcher::~NetIOMsgDispatcher(void)
{
}

bool NetIOMsgDispatcher::init()
{
	return true;
}

bool NetIOMsgDispatcher::process_msg(xGateNetIOServiceMsg *pMsg)
{
	std::string remote_ip{};
	unsigned short remote_port{};
	unsigned short remote_fd{};
	if(!pMsg){
		XGLOG_FATAL("NetIOMsgDispatcher::process_msg failed to get xGateNetIOServiceMsg " );
		return false;
	}
	if(!fetch_fd(pMsg, remote_ip, remote_port, remote_fd)) {
		XGLOG_ERROR("process_msg failed while fetching fd");
		return false;
	}

	if(!send_msg_on_tcp(pMsg, remote_ip, remote_port, remote_fd)) {
		XGLOG_ERROR("send_msg_on_tcp failed after fetching fd ");
		return false;
	}else{
		string uid = pMsg->getUid();
		IURDefines::MODULE_ID srcModuleId = pMsg->getSrcModuleId();
		SessionDetail *details = new SessionDetail(uid, srcModuleId, nullptr);
		SESSIONTRACKER->keepSession(details);
		delete details;
	}

	return true;

}

bool NetIOMsgDispatcher::fetch_fd(xGateNetIOServiceMsg *pMsg, std::string &remote_ip, unsigned short &remote_port, unsigned short &remote_fd)
{
	if(!pMsg){
		XGLOG_ERROR("NetIOMsgDispatcher::fetch_fd failed to get xGateMediaServiceMsg!");
		return false;
	}
	unsigned int connectionId = pMsg->getConnectionId();
	TcpConTuple connTuple;
	if(!NETIOUTIL->getConnTupleInfo(connectionId,connTuple))
	{
		XGLOG_ERROR("Could not find connectionInfo for connectionId:%d",connectionId);
		return false;
	}
	m_fd = connTuple.m_fd;
	remote_ip = connTuple.m_remoteIp;
	remote_port = connTuple.m_remotePort;
	remote_fd = connTuple.m_fd;
	return true;
}

bool NetIOMsgDispatcher::send_msg_on_tcp(xGateNetIOServiceMsg *pNetIOMsg, std::string &IpAddr, unsigned short &Port, unsigned short &Fd)
{
	if(Fd <= 0) {
		XGLOG_ERROR("send message  failed. invalid fd: %d for NetIoIp: %s",Fd,IpAddr.c_str());
		return false;
	}
	std::string str{};
	int error{};
	int len{};
	str = pNetIOMsg->getNetIOmsg();
	std::string delimiter {"\r\n\r\n"};
	str.append(delimiter.c_str(),delimiter.length());
	len = str.length();
	int retVal = send(Fd,str.c_str(),str.length(),MSG_DONTWAIT);
	if(retVal == -1) {
		XGLOG_ERROR("send message failed on fd :%d with error :%s",Fd, strerror(errno));
		close(Fd);//If send failed we can close the fd and try to reconnect using Ace callback
		return false;
	}

	XGLOG_INFO("successfully sent %d bytes of data to IP:(%s) Port:(%d) on fd: %d Data: %s", len, IpAddr.c_str(), Port, Fd, str.c_str());

	return true;
}

