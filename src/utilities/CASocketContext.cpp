#include "CASocketContext.h"
#include "LogHandler.h"

using namespace CA;

/* Method Name: insertConnectionInfo
 ** Description: To insert connection info into map
 ** Parameters: CAConnInfo urConnInfo
 ** return: boolean(true|false)
 **/
bool CASocketContext::insertConnectionInfo(CAConnInfo urConnInfo)
{
   loginfo("","CASocketContext::insertConnectionInfo");
   m_vecCAConnInfo.push_back(urConnInfo);
   return true;
}

/* Method Name: getConnectionInfo
 ** Description: To fetch connection info from map
 ** Parameters: std::string ipAddr, unsigned short port, int& handle
 ** return: boolean(true|false)
 **/
bool CASocketContext::getConnectionInfo(EN_CA_SOCKET_TYPE socType, std::string ipAddr, unsigned short port, int& handle)
{
   loginfo("","CASocketContext::getConnectionInfo called");
   unsigned int index = 0;
   for(index = 0;index < m_vecCAConnInfo.size();index++)
   {
      if(m_vecCAConnInfo[index].ipAddress == ipAddr)
      {
	 if(socType==EN_CA_SOCKET_TYPE::EN_CA_SOCKET_TYPE_CLIENT)
	 {
	    if(port == 0 || m_vecCAConnInfo[index].port == port)
	    {
	       handle = m_vecCAConnInfo[index].fd;
	       return true;
	    }
	 }
	 else if(socType==EN_CA_SOCKET_TYPE::EN_CA_SOCKET_TYPE_SERVER)
	 {
	    if(port == 0 || m_vecCAConnInfo[index].srcPort == port)
	    {
	       handle = m_vecCAConnInfo[index].fd;
	       return true;
	    }
	 }
      }
   }
   loginfo("","CASocketContext::getConnectionInfo connection info not found in map for Address: %s Port: %d", ipAddr.c_str(), port);
   return false;
}

/* Method Name: findConnectionInfo
 ** Description: To find the connection info based on ip address and port
 ** Parameters: std::string ipAddr, unsigned short port
 ** return: boolean(true|false)
 **/
bool CASocketContext::findConnectionInfo(EN_CA_SOCKET_TYPE socType, std::string ipAddr, unsigned short port)
{
   loginfo("","CASocketContext::findConnectionInfo called");
   for(unsigned int i = 0; i< m_vecCAConnInfo.size();i++)
   {
      if(m_vecCAConnInfo[i].ipAddress == ipAddr)
      {
	 if(socType==EN_CA_SOCKET_TYPE::EN_CA_SOCKET_TYPE_CLIENT)
	 {
	    if(m_vecCAConnInfo[i].port == port)
	    {
	       return true;
	    }
	 }
	 else if(socType==EN_CA_SOCKET_TYPE::EN_CA_SOCKET_TYPE_SERVER)
	 {
	    if(m_vecCAConnInfo[i].srcPort == port)
	    {
	       return true;
	    }
	 }
      }
   }
   loginfo("","CASocketContext::findConnectionInfo connection info not found in map for Address: %s Port: %d", ipAddr.c_str(), port);
   return false;
}

/* Method Name: eraseConnectionInfo
 ** Description: To erase connection info from map
 ** Parameters: CAConnInfo urConnInfo
 ** return: boolean(true|false)
 **/
bool CASocketContext::eraseConnectionInfo(CAConnInfo urConnInfo)
{
   loginfo("","CASocketContext::eraseConnectionInfo called");
   for(unsigned int i = 0; i<m_vecCAConnInfo.size();i++)
   {
      if(m_vecCAConnInfo[i] == urConnInfo)
      {
	 m_vecCAConnInfo.erase(m_vecCAConnInfo.begin()+i);
	 return true;
      }
   }
   loginfo("","CASocketContext::eraseConnectionInfo connection info not found in map");
   return false;
}

