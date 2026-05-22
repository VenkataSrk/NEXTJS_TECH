#ifndef __RAPID_XML_H__
#define __RAPID_XML_H__

#include "../../../includes/rapidxml-1.13/rapidxml_ext.h"

namespace CA
{
   typedef enum RET_ERROR
   {
      RETURN_FAIL=0,
      RETURN_SUCCESS,
      PARSER_ERROR,
      RUNTIME_ERROR,
      STANDARD_ERROR,
      UNKNOWN_ERROR
   }RETURN_ERROR;

   class RapidXML
   {
      public:
	 RapidXML();
	 RapidXML(std::string xmlString);
	 virtual ~RapidXML();

	 void setParserString(std::string xmlString);
	 std::string getXMLAsString();
	 rapidxml::xml_document<>& getDoc();
	 RET_ERROR parseDoc();
	 RET_ERROR clearDoc();
	 std::string getLastKnownError();
	 rapidxml::xml_node<>* getRootNode();
	 rapidxml::xml_node<>* getRootNode(std::string nodeName, bool caseSensitive);
	 rapidxml::xml_node<>* getSubNode(std::string nodeName, rapidxml::xml_node<>*node);
	 rapidxml::xml_node<>* getLastNode();
	 rapidxml::xml_node<>* getNextSibling(std::string nodeName, rapidxml::xml_node<>*node);
	 rapidxml::xml_node<>* getPreviousSibling(std::string nodeName, rapidxml::xml_node<>*node);
	 RET_ERROR getNodeValue(rapidxml::xml_node<>* node, std::string &ret);
	 RET_ERROR getFirstAttributeValue(std::string attName, rapidxml::xml_node<>* node, std::string &ret);
	 RET_ERROR getLastAttributeValue(std::string attName, rapidxml::xml_node<>* node, std::string &ret);
	 rapidxml::xml_node<>* createNode(rapidxml::node_type type, std::string nodeName);
	 RET_ERROR encodeToXMLString();
	 RET_ERROR createAttributeValue(rapidxml::xml_node<>*node , std::string key, std::string value);
	 RET_ERROR appendNodeToDoc(rapidxml::xml_node<>*node);
	 RET_ERROR prependNodeToDoc(rapidxml::xml_node<>*node);
	 RET_ERROR appendNodeToNode(rapidxml::xml_node<>*parent, rapidxml::xml_node<>*child);
	 RET_ERROR insertNode(rapidxml::xml_node<>*parent, rapidxml::xml_node<>*child);
	 RET_ERROR removeFirstNode();
	 RET_ERROR removeLastNode();
	 RET_ERROR removeChildNode(rapidxml::xml_node<>*child);
	 RET_ERROR removeAllChildNodes();
	 RET_ERROR prependAttribute(rapidxml::xml_node<>*node, rapidxml::xml_attribute<>*attribute);
	 RET_ERROR appendAttribute(rapidxml::xml_node<>*node, rapidxml::xml_attribute<>*attribute);
	 RET_ERROR insertAttribute(rapidxml::xml_node<>*node, rapidxml::xml_attribute<>*where, rapidxml::xml_attribute<>*attribute);
	 RET_ERROR removeFirstAttribute(rapidxml::xml_node<>*node);
	 RET_ERROR removeLastAttribute(rapidxml::xml_node<>*node);
	 RET_ERROR removeAttribute(rapidxml::xml_node<>*node, rapidxml::xml_attribute<>*where);
	 RET_ERROR removeAllAttribute(rapidxml::xml_node<>*node);	

      private:
	 rapidxml::xml_document<> m_doc;
	 std::string m_strXMLData;
	 std::string m_strLastKnownError;
	 std::string m_strXMLAsString;
	 RET_ERROR m_enErrorId;

	 void setLastKnownError(RET_ERROR errid, std::string errstr);
   };
};
#endif


