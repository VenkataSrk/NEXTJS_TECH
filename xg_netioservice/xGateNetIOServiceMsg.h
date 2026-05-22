#ifndef _XGATE_MEDIA_SERVICE_MSG_H
#define _XGATE_MEDIA_SERVICE_MSG_H

//ace includes
#include <ace/Message_Block.h>

//local includes
#include "xGateUtil.h"
#include "NetIOServiceDefine.h"
class xGateNetIOServiceMsg : public xGateBaseMsg
{
	public:
		xGateNetIOServiceMsg(void) :
			xGateBaseMsg(IURDefines::UR_MODULE_NETIO_SERVICE){};
		virtual ~xGateNetIOServiceMsg(){};

		void setNetIOMsgType(xGateNetIOMsgType msgType)
		{
			m_netioMsgType = msgType;
		}
		xGateNetIOMsgType& getNetIOMsgType(void)
		{
			return m_netioMsgType;
		}
		void setNetIOmsg(std::string msg)
		{
			m_sMsg = msg;
		}
		std::string getNetIOmsg(void)
		{
			return m_sMsg;
		}
		void set_netConnInfo(xGateNetConnection netInfo)
		{
			m_netConInfo = netInfo;
		}
		xGateNetConnection get_netConnInfo(void)
		{
			return m_netConInfo;
		}
		int getConnectionId(void)
		{
		  return m_connectionId;
		}
		void setConnectionId(int connection_id)
		{
		  m_connectionId = connection_id; 
		}

	protected:
		xGateNetIOServiceMsg& operator=(const xGateNetIOServiceMsg&);

	private:
		// Not Implemented Methods
		xGateNetIOServiceMsg(const xGateNetIOServiceMsg&);
		//MgMediaDetail m_mediaDetail;
		xGateNetIOMsgType m_netioMsgType;
		xGateNetConnection m_netConInfo;
		unsigned int m_connectionId;
		std::string m_sMsg;
};
#endif
