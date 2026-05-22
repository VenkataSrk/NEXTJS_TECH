#include "xGateXMLParser.h"
xGateXMLParser::xGateXMLParser(){
}

xGateXMLParser::~xGateXMLParser(){
}
/***************************************************************
* *Name : LoadParser
* *Description: load the document for the given string
* *Input: xml in string
* *Return: boolean true on success parsing fals on parsing failer
* *parameter info: std string type 
* ***************************************************************/
bool xGateXMLParser::LoadParser(std::string payload){
setParserString(payload);
if(loadDocument()!= parser::RETURN_SUCCESS)
 return false;
return true;
}
/***************************************************************
 * *Name : parseXMLInfo
 * *Description: decode the XML string to given structure
 * *Input: void 
 * *Return: return true on success or false on the decode faile
 * *parameter info: NA
 * ***************************************************************/
bool xGateXMLParser::parseXMLInfo(){
// Get the root Info;
rapidxml::xml_node<>*root = getRootNode();
if(decodeConferenceInfo(root,m_confInfo)!= parser::RETURN_SUCCESS) // conference-info
	return false;
if(decodeHostInfo(getSubNode("host-info",root),m_hostInfo) != parser::RETURN_SUCCESS) // host-inf
	return false;
if(decodeUsersInfo(getSubNode("users",root),m_usersCount) != parser::RETURN_SUCCESS)
	return false;
rapidxml::xml_node<>*users =  getSubNode("users",root);//root->first_node("users");
if(users == NULL)
	return false;
for(rapidxml::xml_node<>*user =  getSubNode("user",users); user; user = user->next_sibling()){//users->first_node("user");
 USER_INFO *userInfo = new USER_INFO(); 
 if(userInfo){ 
  decodeUserInfo(user,*userInfo);//users->first_node("user"));
  for (rapidxml::xml_node<>*callnode = getSubNode("call-info",user) ; callnode ; callnode= callnode->next_sibling()){
   CALL_INFO *callInfo = new CALL_INFO();
   if (callInfo){
    decodeCallInfo(callnode,callInfo->index);
    decodeCallStatus(getSubNode("call_status",callnode),callInfo->callStatus);
    decodeDisplayText(getSubNode("Display-text",callnode),callInfo->confDisplayText);
    decodeCallId(getSubNode("call-id",callnode),callInfo->callId);
    decodeSipInfo(getSubNode("to",callnode),callInfo->toHeader);
    decodeSipInfo(getSubNode("from",callnode),callInfo->fromHeader);
    userInfo->callInfo.push_back(callInfo);
   }
  }//callinfo
  m_userList.push_back(userInfo);
 }//userinfo
}
return true;
}
//Getter Functions Defination
CONF_INFO xGateXMLParser::getConferenceInfo(){
 return m_confInfo;
}
HOST_INFO xGateXMLParser::getHostInfo(){
return m_hostInfo;
}
USER_INFO xGateXMLParser::getUserInfo(int index){
if(index < 0)
index = 0;
return *m_userList.at(index);

}
std::string xGateXMLParser::getUserCount(){
return m_usersCount;
}
CALL_INFO xGateXMLParser::getCallInfo(int userindex ,int index){
if(userindex < 0)
userindex =0;

if(index < 0 )
 index =0;

return *getUserInfo(userindex).callInfo.at(index);
}
// setter functionality
void xGateXMLParser::setConferenceInfo(CONF_INFO confInfo){
m_confInfo = confInfo;
}
void  xGateXMLParser::setHostInfo(HOST_INFO hostInfo){
m_hostInfo = hostInfo;
}
void xGateXMLParser::setUserInfo(USER_INFO userInfo, int index){
USER_INFO *puserInfo = new USER_INFO();
if(puserInfo){
*puserInfo =  userInfo;
puserInfo->callInfo = userInfo.callInfo;
m_userList.push_back(puserInfo);
}
}
void xGateXMLParser::setCallInfo(CALL_INFO callInfo, int uindex){
CALL_INFO *tempcallInfo = new CALL_INFO();
if(tempcallInfo){
 *tempcallInfo =  callInfo;
getUserInfo(uindex).callInfo.push_back(tempcallInfo);
 }

}

void xGateXMLParser::setUsersInfo(std::string count){
m_usersCount  =  count;
}
/***************************************************************
 * *Name : decodeConferenceInfo
 * *Description : get the conference information from parsed document 
 * *Input: parent node and conference structure
 * *Return: ture on succes false on failre
 * *parameter info: parent node as input and CONF_INFO are output
 * ***************************************************************/ 
bool xGateXMLParser::decodeConferenceInfo(rapidxml::xml_node<>*node,CONF_INFO &confInfo){
// Conference Info
if(getAttributeValue("confid", node,confInfo.confid) != parser::RETURN_SUCCESS)
	return false;
if(getAttributeValue("state", node,confInfo.confState) != parser::RETURN_SUCCESS)
	return false;

return true;

}
/***************************************************************
 * *Name :
 * *Description:
 * *Input:
 * *Return:
 * *parameter info:
 * ***************************************************************/
bool xGateXMLParser::decodeHostInfo(rapidxml::xml_node<>*node,HOST_INFO &hostInfo){
if( getAttributeValue("entity", node,hostInfo.entity ) != parser::RETURN_SUCCESS)
return false;
if(getAttributeValue("confdisplayname",node,hostInfo.confDisplayName ) != parser::RETURN_SUCCESS)
return false;
return true;
}
/***************************************************************
 * *Name :
 * *Description:
 * *Input:
 * *Return:
 * *parameter info:
 * ***************************************************************/
bool xGateXMLParser::decodeUsersInfo(rapidxml::xml_node<>*node,std::string &usersCount){

if( getAttributeValue("count", node,usersCount) != parser::RETURN_SUCCESS)
return false;

return true;
}
/***************************************************************
 * *Name :
 * *Description:
 * *Input:
 * *Return:
 * *parameter info:
 * ***************************************************************/
bool xGateXMLParser::decodeUserInfo(rapidxml::xml_node<>*node,USER_INFO &userInfo){
 if(getAttributeValue("entity",node,userInfo.entity) != parser::RETURN_SUCCESS)
 return false;
 if(getAttributeValue("entity",node,userInfo.entity) != parser::RETURN_SUCCESS)
 return false;
 if(getAttributeValue("type",node,userInfo.type) != parser::RETURN_SUCCESS)
 return false;
 if(getAttributeValue("index",node,userInfo.index) != parser::RETURN_SUCCESS)
 return false;
  if(getAttributeValue("callcount",node,userInfo.callCount) != parser::RETURN_SUCCESS)
 return false;

return true;
}
/***************************************************************
 * *Name :
 * *Description:
 * *Input:
 * *Return:
 * *parameter info:
 * ***************************************************************/
bool xGateXMLParser::decodeCallInfo(rapidxml::xml_node<>*node,std::string &index){
if( getAttributeValue("index",node,index)!= parser::RETURN_SUCCESS)
return false;
return true;


}
/***************************************************************
 * *Name :
 * *Description:
 * *Input:
 * *Return:
 * *parameter info:
 * ***************************************************************/
bool xGateXMLParser::decodeCallStatus(rapidxml::xml_node<>*node,std::string &status){
if(getAttributeValue("status",node,status) != parser::RETURN_SUCCESS)
return false;
return true;

}
/***************************************************************
 * *Name :
 * *Description:
 * *Input:
 * *Return:
 * *parameter info:
 * ***************************************************************/
bool xGateXMLParser::decodeDisplayText(rapidxml::xml_node<>*node,std::string &value){
if(getAttributeValue("value",node,value) != parser::RETURN_SUCCESS)
return false;
return true;
}
/***************************************************************
 * *Name :
 * *Description:
 * *Input:
 * *Return:
 * *parameter info:
 * ***************************************************************/
bool xGateXMLParser::decodeCallId(rapidxml::xml_node<>*node,std::string &id){
if( getAttributeValue("id",node,id)!=parser::RETURN_SUCCESS)
return false;
return true;

}
/***************************************************************
 * *Name :
 * *Description:
 * *Input:
 * *Return:
 * *parameter info:
 * ***************************************************************/
bool xGateXMLParser::decodeSipInfo(rapidxml::xml_node<>*node,SIP_INFO &sipInfo){

if( getAttributeValue("sipuri",node,sipInfo.sipuri) != parser::RETURN_SUCCESS)
return false;
if(getAttributeValue("tagid",node,sipInfo.tagid) != parser::RETURN_SUCCESS)
return false;

return true;

}
// Encoder Implementation
/***************************************************************
 * *Name :
 * *Description:
 * *Input:
 * *Return:
 * *parameter info:
 * ***************************************************************/
void xGateXMLParser::createXMLDeclaration(){ // create the intial xml declaration 
rapidxml::xml_node<>* dec =  createNode(rapidxml::node_declaration, "");
createAttributewithValue(dec,"version", "1.0");
createAttributewithValue(dec,"encoding", "utf-8");
appendNodeToDoc(dec);
}
/***************************************************************
 * *Name :
 * *Description:
 * *Input:
 * *Return:
 * *parameter info:
 * ***************************************************************/
void xGateXMLParser::encodeConferenceInfo(CONF_INFO confInfo){
std::string str = "conference-info";
rapidxml::xml_node<>*confNode  = createNode(rapidxml::node_element,str);
createAttributewithValue(confNode,"confid",confInfo.confid);
createAttributewithValue(confNode,"state",confInfo.confState);
// Create Host Info
encodeHostInfo(confNode,m_hostInfo);
// Create Users Info
encodeUsersInfo(confNode,m_usersCount);
appendNodeToDoc(confNode);
 }
/***************************************************************
 * *Name :
 * *Description:
 * *Input:
 * *Return:
 * *parameter info:
 * ***************************************************************/
void xGateXMLParser::encodeHostInfo(rapidxml::xml_node<>*node,HOST_INFO hostInfo){
rapidxml::xml_node<>*hostNode  = createNode(rapidxml::node_element,"host-info");
createAttributewithValue(hostNode,"entity",hostInfo.entity);
createAttributewithValue(hostNode,"confDisplayName",hostInfo.confDisplayName);
appendNodeToNode(node,hostNode);
}
/***************************************************************
 * *Name :
 * *Description:
 * *Input:
 * *Return:
 * *parameter info:
 * ***************************************************************/
void xGateXMLParser::encodeUsersInfo(rapidxml::xml_node<>*node,std::string usersCount){
rapidxml::xml_node<>*usersNode  = createNode(rapidxml::node_element,"users");
createAttributewithValue(usersNode,"count",usersCount);
// create User Info Loop vector
for(std::vector<USER_INFO*>::iterator it = m_userList.begin(); it != m_userList.end(); it++){
encodeUserInfo(usersNode,*(*it));
}
appendNodeToNode(node,usersNode);

}
/***************************************************************
 * *Name :
 * *Description:
 * *Input:
 * *Return:
 * *parameter info:
 * ***************************************************************/
void xGateXMLParser::encodeUserInfo(rapidxml::xml_node<>*node,USER_INFO userInfo){
rapidxml::xml_node<>*userNode  = createNode(rapidxml::node_element,"user");
createAttributewithValue(userNode,"entity",userInfo.entity);
createAttributewithValue(userNode,"type",userInfo.type);
createAttributewithValue(userNode,"index",userInfo.index);
createAttributewithValue(userNode,"callcount",userInfo.callCount);
// create call Info vector
for(std::vector<CALL_INFO*>::iterator it = userInfo.callInfo.begin(); it != userInfo.callInfo.end() ; it++){
encodeCallInfo(userNode,*(*it));
}
appendNodeToNode(node,userNode);
}
/***************************************************************
 * *Name :
 * *Description:
 * *Input:
 * *Return:
 * *parameter info:
 * ***************************************************************/
void xGateXMLParser::encodeCallInfo(rapidxml::xml_node<>*node,CALL_INFO callInfo){
rapidxml::xml_node<>*callNode = createNode(rapidxml::node_element,"call-info");
createAttributewithValue(callNode,"index",callInfo.index);
// create other call info nodes
encodeCallStatus(callNode,callInfo.callStatus);
encodeDisplayText(callNode,callInfo.confDisplayText);
encodeCallId(callNode,callInfo.callId);
encodeSipInfo(callNode,callInfo.toHeader,"to");
encodeSipInfo(callNode,callInfo.fromHeader,"from");

appendNodeToNode(node,callNode);
}
/***************************************************************
 * *Name :
 * *Description:
 * *Input:
 * *Return:
 * *parameter info:
 * ***************************************************************/
void xGateXMLParser::encodeCallStatus(rapidxml::xml_node<>*node, std::string callStatus){
rapidxml::xml_node<>*callStatNode = createNode(rapidxml::node_element,"call_status");
createAttributewithValue(callStatNode,"status",callStatus);
appendNodeToNode(node,callStatNode);
}
/***************************************************************
 * *Name :
 * *Description:
 * *Input:
 * *Return:
 * *parameter info:
 * ***************************************************************/
void xGateXMLParser::encodeDisplayText(rapidxml::xml_node<>*node, std::string confdisplaytext){
rapidxml::xml_node<>*displayNode = createNode(rapidxml::node_element,"display-text");
createAttributewithValue(displayNode,"value",confdisplaytext);
appendNodeToNode(node,displayNode);
}
/***************************************************************
 * *Name :
 * *Description:
 * *Input:
 * *Return:
 * *parameter info:
 * ***************************************************************/
void xGateXMLParser::encodeCallId(rapidxml::xml_node<>*node,std::string id){
rapidxml::xml_node<>*callidNode = createNode(rapidxml::node_element,"call-id");
createAttributewithValue(callidNode,"id",id);
appendNodeToNode(node,callidNode);
}
/***************************************************************
 * *Name :
 * *Description:
 * *Input:
 * *Return:
 * *parameter info:
 * ***************************************************************/
void xGateXMLParser::encodeSipInfo(rapidxml::xml_node<>* node,SIP_INFO sipInfo,std::string nodeName){
rapidxml::xml_node<>*sipNode =  createNode(rapidxml::node_element,nodeName);
createAttributewithValue(sipNode,"sipuri",sipInfo.sipuri);
createAttributewithValue(sipNode,"tagid",sipInfo.tagid);
appendNodeToNode(node,sipNode);
}
/***************************************************************
 * *Name :
 * *Description:
 * *Input:
 * *Return:
 * *parameter info:
 * ***************************************************************/
bool xGateXMLParser::serializeXMLInfo(){
createXMLDeclaration();
encodeConferenceInfo(m_confInfo);
encodeToXMLString();
//testingEncode();
return true;
}
