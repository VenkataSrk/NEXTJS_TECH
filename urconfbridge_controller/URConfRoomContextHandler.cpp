#include "xglog.h"
#include "URConfRoomContextHandler.h"

bool URConfRoomContextHandler::insertConfRoomCallContext(std::string key, URConfRoom* urConfRoom)
{
  IT_CONF_ROOM_MAP it = m_tURConfRoomMap.find(key);
  if(it != m_tURConfRoomMap.end())
  {
    XGLOG_ERROR(" Inserting the ConfRoom Context failed. Context id already existis in the map!");
    return false;
  }
  m_tURConfRoomMap.insert(CONF_ROOM_MAP::value_type(key, urConfRoom));
  XGLOG_DEBUG(" Inserting the ConfRoom Context Id in the map!");
  return true;
}

bool URConfRoomContextHandler::deleteConfRoomCallContext(std::string key)
{
  IT_CONF_ROOM_MAP it = m_tURConfRoomMap.find(key);
  if(it == m_tURConfRoomMap.end())
  {
    XGLOG_ERROR(" Deleting the ConfRoom context is failed. Context Id does NOT existis in the map!");
    return false;
  }
  URConfRoom* urConfRoom = it->second;
  delete urConfRoom;
  urConfRoom = NULL;
  m_tURConfRoomMap.erase(key); 
  XGLOG_DEBUG(" Deleteing the ConfRoom context Id from the map!");
  return true;
}

bool URConfRoomContextHandler::getConfRoomCallContext(std::string key, URConfRoom *& urConfRoom)
{
  IT_CONF_ROOM_MAP it = m_tURConfRoomMap.find(key);
  if(it == m_tURConfRoomMap.end()) 
  {
    XGLOG_ERROR("Retrieving the ConfRoom context is failed. Context Id does NOT existis in the map!");
    return false;
  }
  urConfRoom = it->second;
  if(!urConfRoom) 
  {
    XGLOG_ERROR("Call Id. ConfRoom context is NULL");   
    return false;
  }
  XGLOG_DEBUG("Accessing  the ConfRoom context Id from the map!");
  return true;
}

bool URConfRoomContextHandler::add_user_to_conf_call_list(std::string urConfRoomId, std::string callid, URConfCallUserInfo* urConfCallUserInfo)
{
  if(!urConfCallUserInfo)
  {
    XGLOG_ERROR("add_user_to_conf_call_list urConfCallUserInfo is NULL");
    return false;
  }
  IT_CONF_ROOM_MAP it = m_tURConfRoomMap.find(urConfRoomId);
  if(it == m_tURConfRoomMap.end()) 
  {
    XGLOG_ERROR("Retrieving the ConfRoom context is failed. Context Id does NOT existis in the map!");
    return false;
  }
 
  ITR_CONF_CALL_LIST_MAP itr = it->second->m_URConfCallList.find(callid.c_str());
  if(itr == it->second->m_URConfCallList.end())
  {
    it->second->m_URConfCallList.insert(std::pair<std::string, URConfCallUserInfo*>(callid, urConfCallUserInfo));
    int callCount = it->second->m_URConfCallList.size();
    XGLOG_INFO("URConfRoomContextHandler::add_user_to_conf_call_list inserted user callid:%s successfully. CallCount:(%d)", callid.c_str(),callCount);
  } 
  else 
  {
    XGLOG_INFO("URConfRoomContextHandler::add_user_to_conf_call_list user already present in list");
  }
  return true;
}   

bool URConfRoomContextHandler::find_user_in_conf_call_list(std::string urConfRoomId, std::string callId)
{
  if(callId.empty())
  {
    XGLOG_ERROR("find_user_in_conf_call_list callId is empty");
    return false;
  }
  IT_CONF_ROOM_MAP it = m_tURConfRoomMap.find(urConfRoomId);
  if(it == m_tURConfRoomMap.end()) 
  {
    XGLOG_ERROR("Retrieving the ConfRoom context is failed. Context Id does NOT existis in the map!");
    return false;
  }

  ITR_CONF_CALL_LIST_MAP itr = it->second->m_URConfCallList.find(callId);
  if(itr != it->second->m_URConfCallList.end())
  {
    XGLOG_INFO("find_user_in_conf_call_list found Conf User in conf call list : %s !", callId.c_str());
    return true;
  } 
  else 
  {
    XGLOG_INFO("find_user_in_conf_call_list failed. Conf User CallId not found in conf call list : %s!", callId.c_str());
    return false;
  }
  return true;
} 

bool URConfRoomContextHandler::remove_user_from_conf_call_list(std::string urConfRoomId, std::string callId)
{
  if(callId.empty())
  {
    XGLOG_ERROR("remove_user_from_conf_call_list callId is empty");
    return false;
  }
  IT_CONF_ROOM_MAP it = m_tURConfRoomMap.find(urConfRoomId);
  if(it == m_tURConfRoomMap.end()) 
  {
    XGLOG_ERROR("Retrieving the ConfRoom context is failed. Context Id does NOT existis in the map!");
    return false;
  }
  ITR_CONF_CALL_LIST_MAP itr = it->second->m_URConfCallList.find(callId);
  if(itr != it->second->m_URConfCallList.end())
  {
    URConfCallUserInfo* pturConfCallUserInfo = itr->second;
    delete pturConfCallUserInfo;
    pturConfCallUserInfo = NULL;

    it->second->m_URConfCallList.erase(callId);
    int callCount = it->second->m_URConfCallList.size();
    XGLOG_INFO("remove_user_from_conf_call_list removed Conf User CallId:%s. CallCount:(%d)!", callId.c_str(), callCount);
    return true;
  } 
  else
  {
    XGLOG_INFO("remove_user_from_conf_call_list failed. Conf User CallId not present!");
  }
  return true;
} 

bool URConfRoomContextHandler::get_user_callid_from_conf_call_list(std::string urConfRoomId, ConfUserType userType, std::string &callId)
{
  if(urConfRoomId.empty())
  {
    XGLOG_ERROR("get_user_callid_from_conf_call_list confRoomId is empty");
    return false;
  }
  IT_CONF_ROOM_MAP it = m_tURConfRoomMap.find(urConfRoomId);
  if(it == m_tURConfRoomMap.end()) 
  {
    XGLOG_ERROR("Retrieving the ConfRoom context is failed. Context Id does NOT existis in the map!");
    return false;
  }

  ITR_CONF_CALL_LIST_MAP itr = it->second->m_URConfCallList.begin();
  for(itr; itr != it->second->m_URConfCallList.end(); itr++)
  {
    URConfCallUserInfo* pturConfCallUserInfo = itr->second;
    if(pturConfCallUserInfo->m_enumConfUserType == userType)
    {
	callId = pturConfCallUserInfo->m_strConfCallID.c_str();
    }

    XGLOG_INFO("get_user_callid_from_conf_call_list found Conf User:%d in conf call list : %s !", userType, callId.c_str());
    return true;
  } 
  return false;
}

bool URConfRoomContextHandler::get_conf_call_list_size(std::string urConfRoomId,int &size)
{
  IT_CONF_ROOM_MAP it = m_tURConfRoomMap.find(urConfRoomId);
  if(it == m_tURConfRoomMap.end()) 
  {
    XGLOG_ERROR("Retrieving the ConfRoom context is failed. Context Id does NOT existis in the map!");
    return false;
  } 
  else 
  {
    size = it->second->m_URConfCallList.size();
    XGLOG_INFO("get_conf_call_list_size for confRoom:%s - (%d) !",urConfRoomId.c_str(),size);
  }
  return true;
}

bool URConfRoomContextHandler::get_callid_from_connid(std::string urConfRoomId, std::string connId, std::string &callId)
{
  if(urConfRoomId.empty())
  {
    XGLOG_ERROR("get_callid_from_connid urConfRoomId is empty");
    return false;
  }
  IT_CONF_ROOM_MAP it = m_tURConfRoomMap.find(urConfRoomId);
  if(it == m_tURConfRoomMap.end()) 
  {
    XGLOG_ERROR("Retrieving the ConfRoom context is failed. Context Id does NOT existis in the map!");
    return false;
  }
  ITR_SIO_USER_INFO_MAP itr = it->second->m_URCBSIOUserInfoList.find(connId);
  if(itr != it->second->m_URCBSIOUserInfoList.end())
  {
    XGLOG_INFO("get_callid_from_connid fetching socketUserInfo for connId:%s from conf room:%s !", connId.c_str(),urConfRoomId.c_str());
	  callId = itr->second->m_strConfCallId.c_str();
    return true;   
  } 
  XGLOG_ERROR("get_callid_from_connid not found connId:%s in conf room:%s !", connId.c_str(),urConfRoomId.c_str());
  return false;
}

bool URConfRoomContextHandler::get_conf_socket_info_from_connid(std::string urConfRoomId, std::string connId,URCBSIOUserInfo *& ptSioUserInfo)
{
  if(urConfRoomId.empty())
  {
    XGLOG_ERROR("get_conf_socket_info_from_connid urConfRoomId is empty");
    return false;
  }
  IT_CONF_ROOM_MAP it = m_tURConfRoomMap.find(urConfRoomId);
  if(it == m_tURConfRoomMap.end()) 
  {
    XGLOG_ERROR("Retrieving the ConfRoom context is failed. Context Id does NOT existis in the map!");
    return false;
  }
  ITR_SIO_USER_INFO_MAP itr = it->second->m_URCBSIOUserInfoList.find(connId);
  if(itr != it->second->m_URCBSIOUserInfoList.end())
  {
    XGLOG_INFO("get_conf_socket_info_from_connid fetching socketUserInfo for connId:%s from conf room:%s !", connId.c_str(),urConfRoomId.c_str());
	  ptSioUserInfo = itr->second;
    return true;   
  } 
  else 
  {
    XGLOG_ERROR("get_conf_socket_info_from_connid not found connId:%s in conf room:%s !", connId.c_str(),urConfRoomId.c_str());
	  return false;
  }
  return true;
}

bool URConfRoomContextHandler::remove_connid_from_socket_user_info_list(std::string urConfRoomId, std::string connid)
{
  if(connid.empty())
  {
    XGLOG_ERROR("remove_connid_from_socket_user_info_list connid is empty");
    return false;
  }
  IT_CONF_ROOM_MAP it = m_tURConfRoomMap.find(urConfRoomId);
  if(it == m_tURConfRoomMap.end()) 
  {
    XGLOG_ERROR("Retrieving the ConfRoom context is failed. Context Id does NOT existis in the map!");
    return false;
  }
  ITR_SIO_USER_INFO_MAP itr = it->second->m_URCBSIOUserInfoList.find(connid);
  if(itr != it->second->m_URCBSIOUserInfoList.end())
  {
    std::string callId = itr->second->m_strConfCallId.c_str();
    it->second->m_URCBSIOUserInfoList.erase(connid);
    XGLOG_INFO("remove_connid_from_socket_user_info_list removed Conn-Id:(%s) of User CallId:%s !", connid.c_str(), callId.c_str());
    return true;
  } 
  else 
  {
    XGLOG_INFO("remove_connid_from_socket_user_info_list failed. ConnId not present!");
    return false;
  }
  return true;
}

bool URConfRoomContextHandler::get_connid_list_from_confroom(std::string urConfRoomId, std::map<std::string,std::string> &connIdList)
{
  if(urConfRoomId.empty())
  {
    XGLOG_ERROR("get_connid_list_from_confroom urConfRoomId is empty");
    return false;
  }
  IT_CONF_ROOM_MAP it = m_tURConfRoomMap.find(urConfRoomId);
  if(it == m_tURConfRoomMap.end()) 
  {
    XGLOG_ERROR("Retrieving the ConfRoom context is failed. Context Id does NOT existis in the map!");
    return false;
  }
  ITR_SIO_USER_INFO_MAP itr = it->second->m_URCBSIOUserInfoList.begin();
  for(itr; itr != it->second->m_URCBSIOUserInfoList.end(); itr++)
  {
    std::string connid = itr->first;
    std::string callid = itr->second->m_strConfCallId.c_str();
    XGLOG_INFO("get_connid_list_from_confroom inserting connid:(%s) with callid:(%s) from confRoom:(%s)", connid.c_str(),callid.c_str(),urConfRoomId.c_str());
    connIdList.insert(std::pair<std::string,std::string>(connid,callid));
  }
  return true;
}

bool URConfRoomContextHandler::set_acd_channel(std::string urConfRoomId, std::string strChannel)
{
  if(urConfRoomId.empty())
  {
    XGLOG_ERROR("set_acd_channel confroomid is empty");
    return false;
  }
  IT_CONF_ROOM_MAP it = m_tURConfRoomMap.find(urConfRoomId);
  if(it == m_tURConfRoomMap.end()) 
  {
    XGLOG_ERROR("Retrieving the ConfRoom context is failed. Context Id does NOT existis in the map!");
    return false;
  }
  URConfRoom * urConfRoom = it->second;
  if(urConfRoom)
  {
    urConfRoom->m_strAcdChannel = strChannel.c_str();
    return true;
  }
  return false;
} 

bool URConfRoomContextHandler::get_acd_channel(std::string urConfRoomId, std::string &strChannel)
{
  if(urConfRoomId.empty())
  {
    XGLOG_ERROR("set_acd_channel confroomid is empty");
    return false;
  }
  IT_CONF_ROOM_MAP it = m_tURConfRoomMap.find(urConfRoomId);
  if(it == m_tURConfRoomMap.end()) 
  {
    XGLOG_ERROR("Retrieving the ConfRoom context is failed. Context Id does NOT existis in the map!");
    return false;
  }
  URConfRoom * urConfRoom = it->second;
  if(urConfRoom)
  {
    strChannel = urConfRoom->m_strAcdChannel.c_str();
    return true;
  }
  return false;
} 

bool URConfRoomContextHandler::InsertRemoteConfRoomInfo(URCBBroadCastMsg * ptURCBBroadCastMsg)
{
  XGLOG_INFO("InsertRemoteConfRoomInfo called");
  std::string strConfRoomId = ptURCBBroadCastMsg->m_strMsgUuid;
  IT_REMOTE_CONF_ROOM_INFO_MAP itr = m_tRemoteConfRoomInfoMap.find(strConfRoomId);
  if(itr == m_tRemoteConfRoomInfoMap.end())
  {
    URCBRemoteConfRoomInfo remoteConfRoomInfo;
    remoteConfRoomInfo = ptURCBBroadCastMsg->m_tURConfRoomInfo;
    m_tRemoteConfRoomInfoMap.insert({strConfRoomId,remoteConfRoomInfo});
    XGLOG_INFO("InsertRemoteConfRoomInfo insert success for confroom : %s with bridge_server_ip:%s",\
                strConfRoomId.c_str(), remoteConfRoomInfo.m_strConfBridgeIp.c_str());
  }
  else
  {
    XGLOG_ERROR("InsertRemoteConfRoomInfo failed. Entry already present for confroom : %s", strConfRoomId.c_str());
  }
  return true;
}

bool URConfRoomContextHandler::findConfRoomIdInRemoteList(std::string strConfRoomId, std::string &strRedirectIp)
{
  XGLOG_INFO("findConfRoomIdInRemoteList called");
  IT_REMOTE_CONF_ROOM_INFO_MAP itr = m_tRemoteConfRoomInfoMap.find(strConfRoomId);
  if(itr != m_tRemoteConfRoomInfoMap.end())
  {
    URCBRemoteConfRoomInfo &t_remoteConfRoomInfo = itr->second;
    strRedirectIp = t_remoteConfRoomInfo.m_strConfBridgeIp;
    XGLOG_INFO("findConfRoomIdInRemoteList found confroom : %s with bridge_server_ip:%s in remoteConfRoomList",\
                strConfRoomId.c_str(), t_remoteConfRoomInfo.m_strConfBridgeIp.c_str());
    return true;
  }
  else
  {
    XGLOG_INFO("findConfRoomIdInRemoteList failed. Entry not found for confroom : %s", strConfRoomId.c_str());
  }
  return false;
}

bool URConfRoomContextHandler::DeleteRemoteConfRoomInfo(URCBBroadCastMsg * ptURCBBroadCastMsg)
{
  XGLOG_INFO("DeleteRemoteConfRoomInfo called");
  std::string strConfRoomId = ptURCBBroadCastMsg->m_strMsgUuid;
  IT_REMOTE_CONF_ROOM_INFO_MAP itr = m_tRemoteConfRoomInfoMap.find(strConfRoomId);
  if(itr != m_tRemoteConfRoomInfoMap.end())
  {
    m_tRemoteConfRoomInfoMap.erase(itr);
    XGLOG_INFO("DeleteRemoteConfRoomInfo delete entry success for confroom : %s with bridge_server_ip:%s",\
                strConfRoomId.c_str(), ptURCBBroadCastMsg->m_tURConfRoomInfo.m_strConfBridgeIp.c_str());
  }
  else
  {
    XGLOG_ERROR("DeleteRemoteConfRoomInfo failed. Entry not found for confroom : %s", strConfRoomId.c_str());
  }
  return true;
}
