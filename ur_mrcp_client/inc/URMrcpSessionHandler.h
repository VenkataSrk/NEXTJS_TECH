#ifndef UR_MRCP_SESSION_HANDLER_H
#define UR_MRCP_SESSION_HANDLER_H
#include <map>
#include "UrMrcpDefines.h"

class URMrcpSessionHandler
{
	typedef std::map<std::string, std::string> MRCP_CALL_MAP;
	typedef MRCP_CALL_MAP::iterator IT_MRCP_CALL_MAP;
	MRCP_CALL_MAP m_tMrcpCallInfoMap;
	
	typedef std::map<std::string, std::string> MRCP_SESSION_MAP;
	typedef MRCP_SESSION_MAP::iterator IT_MRCP_SESSION_MAP;
	MRCP_SESSION_MAP m_tMrcpSessionInfoMap;

	typedef std::map<std::string, UrMrcpSessionDetails*> MRCP_SESSION_DETAIL_MAP;
	typedef MRCP_SESSION_DETAIL_MAP::iterator IT_MRCP_SESSION_DETAIL_MAP;
	MRCP_SESSION_DETAIL_MAP m_tMrcpSessionDetailMap;

        typedef std::multimap<std::string, std::string> MRCP_CALL_SESSION_MAP;
        typedef MRCP_CALL_SESSION_MAP::iterator IT_MRCP_CALL_SESSION_MAP;
        MRCP_CALL_SESSION_MAP m_tMrcpSessionPairMap;

	public:

		URMrcpSessionHandler(){};
		~URMrcpSessionHandler(){};

		bool InsertMrcpCallUid(std::string callId,std::string uid);
		bool GetMrcpCallUid(std::string callId,std::string &uid);
		bool DeleteMrcpCallUid(std::string callId);

		bool InsertMrcpSesssionHandlerInfo(std::string callId,std::string uid);
		std::string GetMrcpSesssionHandlerInfo(std::string callId);
		bool DeleteMrcpSesssionHandlerInfo(std::string callId);

		bool InsertMrcpSessionDetails(std::string callId,UrMrcpSessionDetails *sessionDetails);
		bool DeleteMrcpSesssionDetails(std::string callId);
		UrMrcpSessionDetails* GetMrcpSesssionDetails(std::string callId);

                bool InsertMrcpSessionCall(std::string uid, std::string callId);
                bool DeleteMrcpSessionCall(std::string uid, std::string callId);
                std::string GetMrcpSesssionCall(std::string uid);

};

#endif
