#ifndef __URCONFROOMCONTEXTHANDLER__
#define __URCONFROOMCONTEXTHANDLER__

#include "URConfRoom.h"
#include <map>
#include <string>
using namespace std;

class URConfRoomContextHandler
{
  typedef std::map<std::string, URConfRoom*> CONF_ROOM_MAP;
  typedef CONF_ROOM_MAP::iterator IT_CONF_ROOM_MAP;
  typedef std::map<std::string, URCBRemoteConfRoomInfo> REMOTE_CONF_ROOM_INFO_MAP;
  typedef REMOTE_CONF_ROOM_INFO_MAP::iterator IT_REMOTE_CONF_ROOM_INFO_MAP;

  CONF_ROOM_MAP m_tURConfRoomMap;
  REMOTE_CONF_ROOM_INFO_MAP m_tRemoteConfRoomInfoMap;

  public:
    URConfRoomContextHandler(){};
    ~URConfRoomContextHandler(){};

    bool insertConfRoomCallContext(std::string key, URConfRoom* urConfRoom);
    bool deleteConfRoomCallContext(std::string key);
    bool getConfRoomCallContext(std::string key, URConfRoom *& urConfRoom);
    bool add_user_to_conf_call_list(std::string urConfRoomId, std::string callid, URConfCallUserInfo* urConfCallUserInfo);
    bool find_user_in_conf_call_list(std::string urConfRoomId, std::string callId);
    bool remove_user_from_conf_call_list(std::string urConfRoomId, std::string callId);
    bool get_user_callid_from_conf_call_list(std::string urConfRoomId, ConfUserType userType, std::string &callId);

    bool get_conf_call_list_size(std::string urConfRoomId,int &size);
    bool get_callid_from_connid(std::string urConfRoomId, std::string connId, std::string &callId);
    bool get_conf_socket_info_from_connid(std::string urConfRoomId, std::string connId,URCBSIOUserInfo *& ptUrcbSIOUserInfo);
    bool remove_connid_from_socket_user_info_list(std::string urConfRoomId, std::string connid);
    bool get_connid_list_from_confroom(std::string urConfRoomId, std::map<std::string,std::string> &connIdList);

    bool set_acd_channel(std::string urConfRoomId, std::string strChannel);
    bool get_acd_channel(std::string urConfRoomId, std::string &strChannel);

    bool InsertRemoteConfRoomInfo(URCBBroadCastMsg * ptURCBBroadCastMsg);
    bool findConfRoomIdInRemoteList(std::string strConfRoomId, std::string &strRedirectIp);
    bool DeleteRemoteConfRoomInfo(URCBBroadCastMsg * ptURCBBroadCastMsg);

};
#endif 
