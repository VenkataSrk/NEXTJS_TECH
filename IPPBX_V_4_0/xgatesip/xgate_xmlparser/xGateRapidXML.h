#ifndef __XGATE_RAPID_XML__
#define __XGATE_RAPID_XML__

#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "rapidxml_print.hpp"

#include "xGateParserUtil.h"

class xGateRapidXML{
public:
    xGateRapidXML();
    ~xGateRapidXML();
    xGateRapidXML(std::string xmlString);

    xParserType loadDocument();
    rapidxml::xml_node<> *getRootNode();
    rapidxml::xml_document<> &getDoc();
    rapidxml::xml_node<>* getSubNode(std::string nodeName, rapidxml::xml_node<>*node);
    xParserType getAttributeValue(std::string , rapidxml::xml_node<>* node,std::string&);
    void setParserString(std::string xmlString);
    std::string getLastError();

    // Writter functions
    rapidxml::xml_node<>* createNode(rapidxml::node_type, std::string nodeName) ; //helps to create declaration , element
    xParserType createAttributewithValue(rapidxml::xml_node<>*node , std::string key, std::string value); // add key : value to element and doc
    xParserType appendNodeToDoc(rapidxml::xml_node<>*node); // add the node under doc tree
    xParserType appendNodeToNode(rapidxml::xml_node<>*parent, rapidxml::xml_node<>*child); // add child under parent
    xParserType encodeToXMLString();
    std::string getxmlAsString();
    void testingEncode();

private:
    rapidxml::xml_document<> m_doc;
    std::string m_parserString;
    std::string m_lastError;
    std::string m_xmlAsString;
    xParserType m_erid;

    void setError(xParserType errid, std::string errstr);
    //rapidxml::xml_node<> *getSubNode(rapidxml::xml_node<>* root);
    //rapidxml::xml_attribute<> *getAttribute(rapidxml::xml_node<>* child);
    //std::string getValue(rapidxml::xml_attribute<> *pAtt, std::string attName);

};
#endif
