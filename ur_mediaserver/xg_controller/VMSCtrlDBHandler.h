#ifndef __VMSCTRLDBHANDLER_H__
#define __VMSCTRLDBHANDLER_H__

#include <sstream>

#include "VmsConfig.h"
#include "VmsControllerDefines.h"
#include "xGateHttpServiceMsg.h"
#include "VmsServerInfo.h"


class VMSCtrlDbHandler
{
	public:
    VMSCtrlDbHandler();
    ~VMSCtrlDbHandler();
  
  
    bool Handle_Request_srvr_info_msg(string &tIpInfo);
    bool Handle_Request_user_info_msg(string tcallID, string &tuserInfo);
    bool Handle_Request_save_vamil_info(xGateBaseMsg *pMsg, VMSVoiceMsgInfo &tinfo);
    bool Handle_Request_Validate_Extn(string tcallID, VMSUserPinInfo &tinfo);
    bool Handle_Request_Validate_Pin(string tcallID, VMSUserPinInfo &tinfo);
	bool Handle_response_srvr_info_msg(xGateHttpServiceMsg* pMsg, VMSServerInfo & tserverInfo);
	bool Handle_response_user_info_msg(xGateHttpServiceMsg*pMsg, VMSBoxUserInfo &tinfo, string &strID);
	bool Handle_Response_save_vamil_info(xGateHttpServiceMsg*pMsg, int &tStatus);
	bool Handle_response_Validate_Extn(xGateHttpServiceMsg*pMsg, VMSUserPinInfo &tinfo, string &strID);
	bool Handle_response_Validate_Pin(xGateHttpServiceMsg*pMsg, VMSUserPinInfo &tinfo, string &strID);
	xGateHttpServiceMsg * getHttpReuestMsg(string &strContext, 
											string &strURL,
											HttpReqMethod tHttpmethod,
											string stData);

	bool readServ_infofromJson(const char  *data, unsigned int dataLen, VMSServerInfo & tserverInfo);
	bool readUser_infofromJson(const char  *data, unsigned int dataLen, VMSBoxUserInfo &tinfo);
	bool readValidateExtn_infofromJson(const char  *data, unsigned int dataLen, VMSUserPinInfo &tinfo);
	bool readValidatePin_infofromJson(const char  *data, unsigned int dataLen, VMSUserPinInfo &tinfo);
	//bool readVMailPost_infofromJson(const char  *data, unsigned int dataLen, VMSServerInfo & tserverInfo);
    
	int getIntValue(std::string strValue) {
		if (strValue.empty())
			return 0;
		int val = 0;
		try {
			std::stringstream ss(strValue);
			ss >> val;
		}
		catch (...) {}
		return val;
	}
		
};

/*
case  VmsController::VMSCONTROLLER_CTRL_INT_SRV_INFO: // rquesting the server info to db
		case  VmsController::VMSCONTROLLER_CTRL_INT_USR_INFO: // get user infor from DB
		case  VmsController::VMSCONTROLLER_CTRL_INT_SAV_VMAIL_INFO:// this iwll be called directly when handling ivr save message notify.
		
create the json message
send http request.

get http request.
decode the json message put 

*/


#endif
