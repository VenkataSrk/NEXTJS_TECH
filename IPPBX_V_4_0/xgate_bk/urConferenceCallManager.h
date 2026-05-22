/* $Header: /xGate/urConferenceCallManager.h
 *
 * VECTONE
 *
 * Conference controller functions
 *
 * version 1
 *
 * @author Yadav Kumar 
 * 
 *
 */

#ifndef UR_CONFERENCE_CALLMANAGER
#define UR_CONFERENCE_CALL_MANAGER
typedef enum URBRCallStatus{
	URBRCALLSTATUS_CONNECTED=0,
	URBRCALLSTATUS_DISCONNECTED
};
class URPBXInfo
{
public:
std::string PBXDNSname;
string PBXIP;
};

class URPBXCalls{
public:
	std::string callid;
	std::string toTag;
	std::string fromTag;
	std::string toUti;
	std::string fromUri;
	URBRCallStatus callStatus;
	URPBXInfo fromPBX;
	
};
class ConferenceRoom{
public:
	std::string ConfRoomId;
	std::string hostURI;
	MediaGateway mediaGatewayInfo;
};
class MediaGateway
{
public:
	std::string MGIP;
	std::short MediaPort;
	
};
class URPbxBroadCastMsg
{
public:
	std::string msgUUID;
	URBRMsgType messageType;
	std::list<URPBXCalls> callDetails;
	ConferenceRoom confRoomDetails;
};
class URConferenceCallManager {
private:
	static URConferenceCallManager *urConfInstance;
	URConferenceCallManager();
	~URConferenceCallManager();	
public:
	static URConferenceCallManager *getInstance();
	static void terminateInstance();
	std::map<std::string confKey, ConferenceRoom*> urConferenceRooms;
};
#endif

