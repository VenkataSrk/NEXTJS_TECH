#ifndef XGATE_XML_PARSER
#define XGATE_XML_PARSER
#include <string>
#include <iostream>
#include "xGateRapidXML.h"
//#include "xGateParserUtil.h"
class xGateXMLParser :public xGateRapidXML{
public:
xGateXMLParser();
~xGateXMLParser();
void LoadParser(std::string payload);
void parseXMLInfo();
void serializeXMLInfo();

//Getter
CONF_INFO getConferenceInfo();
HOST_INFO getHostInfo();
USER_INFO getUserInfo(int index = -1);
std::string getUserCount();
CALL_INFO getCallInfo(int userindex = -1,int index=-1);
// setter 
void setConferenceInfo(CONF_INFO confInfo);
void setHostInfo(HOST_INFO hostInfo);
void setUserInfo(USER_INFO userInfo,int index = -1);
void setCallInfo(CALL_INFO callInfo, int index=-1);
void setUsersInfo(std::string count);
private:
CONF_INFO m_confInfo;
HOST_INFO m_hostInfo;
//CALL_INFO callInfo;
//std::vector<CALL_INFO*> m_callInfoList;
std::vector<USER_INFO*> m_userList;
//USER_INFO m_userInfo;
std::string m_usersCount;

void decodeConferenceInfo(rapidxml::xml_node<>*node,CONF_INFO &confInfo);
void decodeHostInfo(rapidxml::xml_node<>*node,HOST_INFO &hostInfo);
void decodeUsersInfo(rapidxml::xml_node<>*node,std::string &usersCount);
void decodeCallInfo(rapidxml::xml_node<>*node,std::string &index);
void decodeSipInfo(rapidxml::xml_node<>* node,SIP_INFO &sipInfo);
void decodeUserInfo(rapidxml::xml_node<>*node,USER_INFO &userInfo);
void decodeCallStatus(rapidxml::xml_node<>*node, std::string &callStatus);
void decodeDisplayText(rapidxml::xml_node<>*node, std::string &confdisplaytext);
void decodeCallId(rapidxml::xml_node<>*node,std::string &id);

void createXMLDeclaration();
void encodeConferenceInfo(CONF_INFO confInfo);
void encodeHostInfo(rapidxml::xml_node<>*node,HOST_INFO hostInfo);
void encodeUsersInfo(rapidxml::xml_node<>*node,std::string usersCount);
void encodeUserInfo(rapidxml::xml_node<>*node,USER_INFO userInfo);  
void encodeCallInfo(rapidxml::xml_node<>*node,CALL_INFO callInfo);
void encodeCallStatus(rapidxml::xml_node<>*node, std::string callStatus);
void encodeDisplayText(rapidxml::xml_node<>*node, std::string confdisplaytext);
void encodeCallId(rapidxml::xml_node<>*node,std::string id);
void encodeSipInfo(rapidxml::xml_node<>* node,SIP_INFO sipInfo,std::string nodeName);
};
#endif
