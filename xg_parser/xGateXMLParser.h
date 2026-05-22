#ifndef XGATE_XML_PARSER
#define XGATE_XML_PARSER
#include <string>
#include <iostream>
#include "xGateRapidXML.h"

struct SIP_INFO{
  std::string sipuri;
  std::string tagid;
};

struct CALL_INFO{
  std::string index;
  std::string callInfo; // call info entity
  std::string callStatus; // state of the call
  std::string callId; // call id of particular leg
  std::string confDisplayText;
  SIP_INFO toHeader; // to header info
  SIP_INFO fromHeader; // from header info
};

struct USER_INFO{
  std::string entity;
  std::string type;
  std::string index;
  std::string callCount;
  std::vector<CALL_INFO*> callInfo;
  //std::vector<CALL_INFO*> callInfo;
};

struct CONF_INFO{
  std::string confid; // unique conference id for host and particpant join
  std::string confState;// state of the conference
};

struct HOST_INFO{
  std::string entity;
  std::string confDisplayName;
};

class xGateXMLParser :public xGateRapidXML {
  public:
    xGateXMLParser();
    ~xGateXMLParser();
    bool LoadParser(std::string payload);
    bool parseXMLInfo();
    bool serializeXMLInfo();

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

    bool decodeConferenceInfo(rapidxml::xml_node<>*node,CONF_INFO &confInfo);
    bool decodeHostInfo(rapidxml::xml_node<>*node,HOST_INFO &hostInfo);
    bool decodeUsersInfo(rapidxml::xml_node<>*node,std::string &usersCount);
    bool decodeCallInfo(rapidxml::xml_node<>*node,std::string &index);
    bool decodeSipInfo(rapidxml::xml_node<>* node,SIP_INFO &sipInfo);
    bool decodeUserInfo(rapidxml::xml_node<>*node,USER_INFO &userInfo);
    bool decodeCallStatus(rapidxml::xml_node<>*node, std::string &callStatus);
    bool decodeDisplayText(rapidxml::xml_node<>*node, std::string &confdisplaytext);
    bool decodeCallId(rapidxml::xml_node<>*node,std::string &id);

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
