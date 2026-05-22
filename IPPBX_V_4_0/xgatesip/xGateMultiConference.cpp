#include "xGateMultiConference.h"
//using namespace rapidxml;
using namespace sipreturn;
xGateMultiConference::xGateMultiConference(){
}
xGateMultiConference::~xGateMultiConference(){
}
std::string xGateMultiConference::getMultiConferenceId(){
return m_confXML.confInfo.confid;
}
/*
 * Method used to decode sip info payload
 * input: const char pointer 
 * return BOOLEN type
 * */
xGateReturn xGateMultiConference::infoRequest(const char* payLoad){
if(payLoad == NULL)
 return FAIL;

// start the decoding
return decodeXML(payLoad, m_confXML);

}

xGateReturn xGateMultiConference::decodeXML(const char* payload , CONF_XML &xml){

if(m_xmlParser.LoadParser(payload) == false)// Will load the DOM entity
 return FAIL;
if(m_xmlParser.parseXMLInfo() == false) // decode the DOM to parser structurie
 return FAIL;
// Get the CONf XML filled form parser
xml.confInfo =  m_xmlParser.getConferenceInfo();
			     
xml.userInfo  =  m_xmlParser.getUserInfo(0); // Currenlty get for one user info 

return SUCCESS;
}

xGateReturn xGateMultiConference::encodeXML( char* paylod, CONF_XML xml){

return SUCCESS;
}
xGateReturn xGateMultiConference::conferenceStart(CONF_XML xml){
return SUCCESS;
}
xGateReturn xGateMultiConference::conferenceUpdate(CONF_XML xml){
return SUCCESS;
}
xGateReturn xGateMultiConference::conferenceStop(CONF_XML xml){
return SUCCESS;
}
void xGateMultiConference::XMLParserTest(){
/*xml_document<> doc;
xml_node<> * root_node;
const char *test = "<?xml version=\"1.0\" encoding=\"utf-8\"?><Company><Employees><Employee Name=\"John\" Age=\"30\"/><Employee Name=\"Harry\" Age=\"27\"/><Employee Name=\"Kate\" Age=\"29\"/></Employees><Departments><Department Name=\"Sales\" TeamSize=\"4\"/><Department Name=\"Marketing\" TeamSize=\"10\"/><Department Name=\"Developemnt\" Head=\"kapil\" TeamSize=\"5\"/><Department Name=\"Testing\" Head=\"John\"></Department></Departments></Company>";
std::string testxml =  test;
doc.parse<0>((char*) testxml.c_str());
root_node = doc.first_node();
for(xml_node<>* currNode = root_node->first_node(); currNode != NULL; currNode = currNode->next_sibling())
   {
      string nodeName = currNode->name();
      xml_attribute<>* nameAttr = currNode->first_attribute("Name");
      xml_attribute<>* valueAttr = currNode->first_attribute("Age");
printf("NodeName %s \t", currNode->name());
if(nodeName == "Employees" && nameAttr != NULL && valueAttr != NULL){
printf("AttributeName %s \t", nameAttr->value());
printf("ValueName %s \n",valueAttr->value());
}
   }
*/
}
