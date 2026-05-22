#ifndef XGATE_RAPID_XML
#define XGATE_RAPID_XML
#include "../rapidxml-1.13/rapidxml.hpp"
#include "../rapidxml-1.13/rapidxml_utils.hpp"
#include "../rapidxml-1.13/rapidxml_print.hpp"
#include "xGateParserUtil.h"
class xGateRapidXML{
public:
xGateRapidXML();
~xGateRapidXML();
xGateRapidXML(std::string xmlString);

XPARSER_ERROR loadDocument();
rapidxml::xml_node<> *getRootNode();
 rapidxml::xml_document<> &getDoc();
rapidxml::xml_node<>* getSubNode(std::string nodeName, rapidxml::xml_node<>*node);
std::string getAttributeValue(std::string , rapidxml::xml_node<>* node);
void setParserString(std::string xmlString);
std::string getLastError();
// Writter functions
 rapidxml::xml_node<>* createNode(rapidxml::node_type, std::string nodeName) ; //helps to create declaration , element
XPARSER_ERROR createAttributewithValue(rapidxml::xml_node<>*node , std::string key, std::string value); // add key : value to element and doc
XPARSER_ERROR appendNodeToDoc(rapidxml::xml_node<>*node); // add the node under doc tree
XPARSER_ERROR appendNodeToNode(rapidxml::xml_node<>*parent, rapidxml::xml_node<>*child); // add child under parent
XPARSER_ERROR encodeToXMLString();
std::string getxmlAsString();
void testingEncode();
private:
rapidxml::xml_document<> m_doc;
std::string m_parserString;
std::string m_lastError;
std::string m_xmlAsString;
XPARSER_ERROR m_erid;

void setError(XPARSER_ERROR errid, std::string errstr);
//rapidxml::xml_node<> *getSubNode(rapidxml::xml_node<>* root);
//rapidxml::xml_attribute<> *getAttribute(rapidxml::xml_node<>* child);
//std::string getValue(rapidxml::xml_attribute<> *pAtt, std::string attName);

};
#endif
