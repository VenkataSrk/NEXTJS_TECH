#include "xGateRapidXML.h"

xGateRapidXML::xGateRapidXML(){
}

xGateRapidXML::~xGateRapidXML(){
}
xGateRapidXML::xGateRapidXML(std::string xmlString){
m_parserString = xmlString ;
}
XPARSER_ERROR xGateRapidXML::loadDocument(){

setError(SUCCESS,"xGateRapidXML::loadDocument execute successfully");

if(m_parserString.empty()){
 setError(FAIL,"XML String empty");
 return m_erid;
}
try{
m_doc.parse<0>(&m_parserString[0]);
}
catch (const std::runtime_error& e){
setError(RUNTIME_ERROR,e.what());
}
catch (const rapidxml::parse_error& e){
setError(PARSER_ERROR,e.what());
}
catch (const std::exception& e){
setError(STANDARD_ERROR,e.what());
}
catch (...){
setError(UNKNOWN_ERROR,"Unknown Error");
}
return m_erid;

}
// Set the last errro set by each function 
void xGateRapidXML::setError(XPARSER_ERROR errid, std::string errstr){
m_erid = errid;
m_lastError =  errstr;
}
 rapidxml::xml_document<>& xGateRapidXML::getDoc(){
return m_doc;
}
std::string xGateRapidXML::getLastError(){
return m_lastError;
}

rapidxml::xml_node<>* xGateRapidXML::getRootNode(){
setError(SUCCESS,"xGateRapidXML::getRootNode succesfully");
if(getDoc().first_node()) 
 return getDoc().first_node(); 
else 
 setError(FAIL,"Root Node Invalid"); 

return NULL ;
}
rapidxml::xml_node<>* xGateRapidXML::getSubNode(std::string nodeName, rapidxml::xml_node<>*node){
setError(SUCCESS,"xGateRapidXML::getSubNode success");
if(nodeName.empty()){
setError(FAIL,"xGateRapidXML::getSubNode nodeName empty");
return NULL;
}
if(node == NULL){
setError(FAIL,"xGateRapidXML::getSubNode parent node is null");
return NULL;
}
rapidxml::xml_node<>*subnode = node->first_node(nodeName.c_str());
if(subnode == NULL){
setError(FAIL,"xGateRapidXML::getSubNode sub node is null");
}
return subnode;
}

std::string xGateRapidXML::getAttributeValue(std::string attName, rapidxml::xml_node<>* node){
setError(SUCCESS,"xGateRapidXML::getAttributeValue success");
std::string ret ="";
if(attName.empty()){
setError(FAIL,"xGateRapidXML::getAttributeValue attribute name empty");
return ret;
}
if(node == NULL){
setError(FAIL,"xGateRapidXML::getAttributeValue  node is null");
return ret;
}
try{
ret = node->first_attribute(attName.c_str())->value();
}
catch (const std::runtime_error& e){
setError(RUNTIME_ERROR, e.what());
ret = "";
}
catch (const rapidxml::parse_error& e){
setError(PARSER_ERROR,e.what());
ret = "";
}
catch (const std::exception& e){
setError(STANDARD_ERROR,e.what());
ret = "";
}
catch(...){
setError(UNKNOWN_ERROR,"Unknown Error occured getAttributeValue");
ret = "";
}
return ret;
}
void xGateRapidXML::setParserString(std::string xmlString){
m_parserString = xmlString;
}
// writter implementation
 rapidxml::xml_node<>* xGateRapidXML::createNode(rapidxml::node_type typ, std::string nodeName){
setError(SUCCESS,"xGateRapidXML::createNode successfull");
 rapidxml::xml_node<>*retNode =NULL;
try{
 switch(typ){
  case rapidxml::node_declaration:
   retNode =  getDoc().allocate_node(rapidxml::node_declaration);
  break;
  case rapidxml::node_element:
    if(nodeName.empty())
     retNode = getDoc().allocate_node(rapidxml::node_element);
    else{ 
     char* pnodeName = getDoc().allocate_string(nodeName.c_str());
     retNode = getDoc().allocate_node(rapidxml::node_element,pnodeName);
    }
  break;
  default:
   setError(FAIL,"xGateRapidXML::createNode default case failed");
  break;
 }
}catch(...){
  setError(UNKNOWN_ERROR, "Unknown error occured xGateRapidXML::createNode successfull");
}
return retNode;
}
XPARSER_ERROR xGateRapidXML::encodeToXMLString(){
setError(SUCCESS,"xGateRapidXML::encodeToXMLString successfull");
try{
 rapidxml::print(std::back_inserter(m_xmlAsString), getDoc());
 }catch(...){
  setError(UNKNOWN_ERROR, "Unknown error occured xGateRapidXML::encodeToXMLString");
 }
 return m_erid;
}
XPARSER_ERROR xGateRapidXML::createAttributewithValue(rapidxml::xml_node<>*node , std::string key, std::string value){
setError(SUCCESS,"xGateRapidXML::createAttributewithValue successfull");
if (node == NULL){
 setError(FAIL,"xGateRapidXML::createAttributewithValue Node is Null failed");
 return m_erid;
 }
 if(key.empty() || value.empty()){
  setError(FAIL,"xGateRapidXML::createAttributewithValue key or value is empty failed");
  return m_erid;
  }

try{
  char*pKey =  getDoc().allocate_string(key.c_str());
  char*pValue = getDoc().allocate_string(value.c_str());
  node->append_attribute(getDoc().allocate_attribute(pKey,pValue));
}catch(...){
  setError(UNKNOWN_ERROR, "Unknown error occured xGateRapidXML::createAttributewithValue failed");
}
return m_erid;
}
XPARSER_ERROR xGateRapidXML::appendNodeToDoc(rapidxml::xml_node<>*node){
setError(SUCCESS,"xGateRapidXML::appendNodeToDoc successfull");
try{
if(node == NULL){
 setError(FAIL,"xGateRapidXML::appendNodeToDoc Node is Null failed");
 return m_erid;
}
getDoc().append_node(node);
}catch(...){
  setError(UNKNOWN_ERROR, "Unknown error occured xGateRapidXML::appendNodeToDoc successfull");
}
return m_erid;
}

XPARSER_ERROR xGateRapidXML::appendNodeToNode(rapidxml::xml_node<>*parent, rapidxml::xml_node<>*child){
setError(SUCCESS,"xGateRapidXML::appendNodeToNode successfull");
if(parent == NULL){
  setError(FAIL,"xGateRapidXML::appendNodeToNode parent is NULL failed");
 return m_erid;
 }
 if(child == NULL){
 setError(FAIL,"xGateRapidXML::appendNodeToNode child is NULL failed");
 return m_erid;
 }
try{
 parent->append_node(child);

}catch(...){
  setError(UNKNOWN_ERROR, "Unknown error occured xGateRapidXML::appendNodeToNode successfull");
}
return m_erid;
}
std::string xGateRapidXML::getxmlAsString(){
return m_xmlAsString;
}

void xGateRapidXML::testingEncode(){
using namespace rapidxml;

xml_document<> doc ; 

// xml declaration
xml_node<>* decl = doc.allocate_node(node_declaration);
decl->append_attribute(doc.allocate_attribute("version", "1.0"));
decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
doc.append_node(decl);

// root node
/*xml_node<>* root = doc.allocate_node(node_element, "rootnode");
root->append_attribute(doc.allocate_attribute("version", "1.0"));
root->append_attribute(doc.allocate_attribute("type", "example"));
doc.append_node(root);

// child node
xml_node<>* child = doc.allocate_node(node_element, "childnode");
root->append_node(child);
*/
std::string xml_as_string;
// watch for name collisions here, print() is a very common function name!
print(std::back_inserter(xml_as_string), doc);
// xml_as_string now contains the XML in string form, indented
// (in all its angle bracket glory)
m_xmlAsString = xml_as_string;
std::string xml_no_indent;
// print_no_indenting is the only flag that print() knows about
print(std::back_inserter(xml_no_indent), doc, print_no_indenting);
// xml_no_indent now contains non-indented XML
}
