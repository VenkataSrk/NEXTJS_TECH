#include "RapidXML.h"

using namespace CA;

RapidXML::RapidXML()
{}

RapidXML::~RapidXML()
{clearDoc();}

RapidXML::RapidXML(std::string xmlString)
{
	 m_strXMLData = xmlString ;
}

/* Method Name: setParserString
 ** Description: To set the xml data
 ** Parameters: xml data
 ** return: void
 **/
void RapidXML::setParserString(std::string xmlString)
{
	 m_strXMLData = xmlString;
}

/* Method Name: getXMLAsString
 ** Description: get XML string
 ** Parameters: nil
 ** return: XML string
 **/
std::string RapidXML::getXMLAsString()
{
	 return m_strXMLAsString;
}

/* Method Name: getDoc
 ** Description: To get the xml document
 ** Parameters: nil
 ** return: xml_document
 **/
rapidxml::xml_document<>& RapidXML::getDoc()
{
	 return m_doc;
}

/* Method Name: parseDoc
 ** Description: To parse the xml data
 ** Parameters: nil
 ** return: success|failure
 **/
RET_ERROR RapidXML::parseDoc()
{
	 if(m_strXMLData.empty())
	 {
			setLastKnownError(RETURN_FAIL, "XML String empty");
			return m_enErrorId;
	 }
	 try 
	 {
			m_doc.parse<0>(&m_strXMLData[0]);
			setLastKnownError(RETURN_SUCCESS, "XML String empty");
	 }
	 catch (const std::runtime_error& e) 
	 {
			setLastKnownError(RUNTIME_ERROR,e.what());
	 }
	 catch (const rapidxml::parse_error& e) 
	 {
			setLastKnownError(PARSER_ERROR,e.what());
	 }
	 catch (const std::exception& e) 
	 {
			setLastKnownError(STANDARD_ERROR,e.what());
	 }
	 catch (...) 
	 {
			setLastKnownError(UNKNOWN_ERROR,"Unknown Error");
	 }
	 return m_enErrorId;
}

/* Method Name: clearDoc
 ** Description: To clear the xml_document
 ** Parameters: nil
 ** return: success|failure
 **/
RET_ERROR RapidXML::clearDoc()
{
	 try
	 {
			m_doc.clear();
	 }
	 catch(...)
	 {
			setLastKnownError(UNKNOWN_ERROR,"Unknown Error");
	 }
	 return m_enErrorId;
}

/* Method Name: setLastKnownError
 ** Description: To set error id and string
 ** Parameters: RET_ERROR errid, std::string errstr
 ** return: void
 **/
void RapidXML::setLastKnownError(RET_ERROR errid, std::string errstr)
{
	 m_enErrorId = errid;
	 m_strLastKnownError =  errstr;
}

/* Method Name: getLastKnownError
 ** Description: To get error string
 ** Parameters: nil
 ** return: string
 **/
std::string RapidXML::getLastKnownError()
{
	 return m_strLastKnownError;
}

/* Method Name: getRootNode
 ** Description: Gets first child node
 ** Parameters: nil
 ** return: xml_node
 **/
rapidxml::xml_node<>* RapidXML::getRootNode()
{
	 setLastKnownError(RETURN_SUCCESS,"RapidXML::getRootNode success");
	 if(getDoc().first_node()) 
	 {
			return getDoc().first_node();
	 } 
	 else 
	 {
			setLastKnownError(RETURN_FAIL,"Root Node Invalid");
	 }
	 return NULL;
}

/* Method Name: getRootNode
 ** Description: Gets first child node
 ** Parameters: 
 ** return: xml_node
 **/
rapidxml::xml_node<>* RapidXML::getRootNode(std::string nodeName, bool caseSensitive=true)
{
	 setLastKnownError(RETURN_SUCCESS,"RapidXML::getRootNode success");
	 if(getDoc().first_node(nodeName.c_str(), caseSensitive))
	 {
			return getDoc().first_node(nodeName.c_str(), caseSensitive);
	 }
	 else
	 {
			setLastKnownError(RETURN_FAIL,"Root Node Invalid");
	 }
	 return NULL;
}

/* Method Name: getSubNode
 ** Description: Gets first child node based on node name of given parent node
 ** Parameters: std::string nodeName, rapidxml::xml_node<>*node
 ** return: xml_node
 **/
rapidxml::xml_node<>* RapidXML::getSubNode(std::string nodeName, rapidxml::xml_node<>*node)
{
	 setLastKnownError(RETURN_SUCCESS,"RapidXML::getSubNode success");
	 rapidxml::xml_node<>*subnode = NULL;
	 if(nodeName.empty()) 
	 {
			setLastKnownError(RETURN_FAIL,"RapidXML::getSubNode nodeName empty");
			return NULL;
	 }

	 if(node == NULL) 
	 {
			setLastKnownError(RETURN_FAIL,"RapidXML::getSubNode parent node is null");
			return NULL;
	 }

	 try
	 {
			subnode = node->first_node(nodeName.c_str());
	 }
	 catch (const std::runtime_error& e)
	 {
			setLastKnownError(RUNTIME_ERROR, e.what());
	 }
	 catch (const rapidxml::parse_error& e)
	 {
			setLastKnownError(PARSER_ERROR,e.what());
	 }
	 catch (const std::exception& e)
	 {
			setLastKnownError(STANDARD_ERROR,e.what());
	 }
	 catch(...)
	 {
			setLastKnownError(UNKNOWN_ERROR,"Unknown Error occured getSubNode");
	 }
	 if(subnode == NULL) 
	 {
			setLastKnownError(RETURN_FAIL,"RapidXML::getSubNode sub node is null");
	 }
	 return subnode;
}

/* Method Name: getLastNode
 ** Description: Gets last child node
 ** Parameters: nil
 ** return: xml_node
 **/
rapidxml::xml_node<>* RapidXML::getLastNode()
{
	 setLastKnownError(RETURN_SUCCESS,"RapidXML::getLastNode success");
	 if(getDoc().last_node())
	 {
			return getDoc().last_node();
	 }
	 else
	 {
			setLastKnownError(RETURN_FAIL,"Last Node Invalid");
	 }
	 return NULL;
}

/* Method Name: getNextSibling
 ** Description: Gets next sibling
 ** Parameters: std::string nodeName, rapidxml::xml_node<>*node
 ** return: xml_node
 **/
rapidxml::xml_node<>* RapidXML::getNextSibling(std::string nodeName, rapidxml::xml_node<>*node)
{
	 setLastKnownError(RETURN_SUCCESS,"RapidXML::getNextSibling success");
	 rapidxml::xml_node<>* retNode = NULL;

	 if(node == NULL)
	 {
			setLastKnownError(RETURN_FAIL,"RapidXML::getNextSibling node is null");
	 }

	 if(node->parent())
	 {
			try
			{
				 if(!nodeName.empty())
				 {
						retNode = node->next_sibling(nodeName.c_str());
				 }
				 else
				 {
						retNode = node->next_sibling();
				 }
				 if(retNode == NULL)
				 {
						setLastKnownError(RETURN_FAIL,"RapidXML::getNextSibling node is null");
				 }
			}
			catch (const std::runtime_error& e)
			{
				 setLastKnownError(RUNTIME_ERROR, e.what());
			}
			catch (const rapidxml::parse_error& e)
			{
				 setLastKnownError(PARSER_ERROR,e.what());
			}
			catch (const std::exception& e)
			{
				 setLastKnownError(STANDARD_ERROR,e.what());
			}
			catch(...)
			{
				 setLastKnownError(UNKNOWN_ERROR,"Unknown Error occured getNextSibling");
			}

	 }
	 else
	 {
			setLastKnownError(RETURN_FAIL,"Node has no parent");
	 }
	 return retNode;
}

/* Method Name: getPreviousSibling
 ** Description: Gets previous sibling
 ** Parameters: std::string nodeName, rapidxml::xml_node<>*node
 ** return: xml_node
 **/
rapidxml::xml_node<>* RapidXML::getPreviousSibling(std::string nodeName, rapidxml::xml_node<>*node)
{
	 setLastKnownError(RETURN_SUCCESS,"RapidXML::getPreviousSibling success");
	 rapidxml::xml_node<>* retNode = NULL;
	 if(node == NULL)
	 {
			setLastKnownError(RETURN_FAIL,"RapidXML::getPreviousSibling node is null");
	 }
	 if(node->parent())
	 {
			try
			{
				 if(!nodeName.empty())
				 {
						retNode = node->previous_sibling(nodeName.c_str());
				 }
				 else
				 {
						retNode = node->previous_sibling();
				 }
				 if(retNode == NULL)
				 {
						setLastKnownError(RETURN_FAIL,"RapidXML::getPreviousSibling node is null");
				 }
			}
			catch (const std::runtime_error& e)
			{
				 setLastKnownError(RUNTIME_ERROR, e.what());
			}
			catch (const rapidxml::parse_error& e)
			{
				 setLastKnownError(PARSER_ERROR,e.what());
			}
			catch (const std::exception& e)
			{
				 setLastKnownError(STANDARD_ERROR,e.what());
			}
			catch(...)
			{
				 setLastKnownError(UNKNOWN_ERROR,"Unknown Error occured getPreviousSibling");
			}
	 }
	 else
	 {
			setLastKnownError(RETURN_FAIL,"Node has no parent");
	 }
	 return retNode;
}

/* Method Name: getNodeValue
 ** Description: Gets value from node
 ** Parameters: rapidxml::xml_node<>* node, std::string &ret
 ** return: success|failure
 **/
RET_ERROR RapidXML::getNodeValue(rapidxml::xml_node<>* node, std::string &ret)
{
  setLastKnownError(RETURN_SUCCESS,"RapidXML::getNodeValue success");
  ret ="";
  if(node == NULL)
   {
      setLastKnownError(RETURN_FAIL,"RapidXML::getNodeValue node is null");
      return m_enErrorId;
   }
   try
   {
      ret = node->value();
   }
   catch (const std::runtime_error& e)
   {
      setLastKnownError(RUNTIME_ERROR, e.what());
      ret = "";
   }
   catch (const rapidxml::parse_error& e)
   {
      setLastKnownError(PARSER_ERROR,e.what());
      ret = "";
   }
   catch (const std::exception& e)
   {
      setLastKnownError(STANDARD_ERROR,e.what());
      ret = "";
   }
   catch(...)
   {
      setLastKnownError(UNKNOWN_ERROR,"Unknown Error occured getNodeValue");
      ret = "";
   }
   return m_enErrorId;
}

/* Method Name: getFirstAttributeValue
 ** Description: Gets first attribute of node, matching attribute name.
 ** Parameters: std::string attName, rapidxml::xml_node<>* node, std::string &ret
 ** return: success|failure
 **/
RET_ERROR RapidXML::getFirstAttributeValue(std::string attName, rapidxml::xml_node<>* node, std::string &ret)
{
	 setLastKnownError(RETURN_SUCCESS,"RapidXML::getFirstAttributeValue success");
	 ret ="";
	 if(attName.empty()) 
	 {
			setLastKnownError(RETURN_FAIL,"RapidXML::getAttributeValue attribute name empty");
			return m_enErrorId;
	 }

	 if(node == NULL) 
	 {
			setLastKnownError(RETURN_FAIL,"RapidXML::getAttributeValue node is null");
			return m_enErrorId;
	 }

	 try 
	 {
			//ret = node->first_attribute(attName.c_str())->value();
			(node->first_attribute(attName.c_str())) ? ret = node->first_attribute(attName.c_str())->value()	: ret = "";	
	 }
	 catch (const std::runtime_error& e) 
	 {
			setLastKnownError(RUNTIME_ERROR, e.what());
			ret = "";
	 }
	 catch (const rapidxml::parse_error& e) 
	 {
			setLastKnownError(PARSER_ERROR,e.what());
			ret = "";
	 }
	 catch (const std::exception& e) 
	 {
			setLastKnownError(STANDARD_ERROR,e.what());
			ret = "";
	 }
	 catch(...) 
	 {
			setLastKnownError(UNKNOWN_ERROR,"Unknown Error occured getFirstAttributeValue");
			ret = "";
	 }
	 return m_enErrorId;
}

/* Method Name: getLastAttributeValue
 ** Description: Gets last attribute of node, matching attribute name.
 ** Parameters: std::string attName, rapidxml::xml_node<>* node, std::string &ret
 ** return: success|failure
 **/
RET_ERROR RapidXML::getLastAttributeValue(std::string attName, rapidxml::xml_node<>* node, std::string &ret)
{
	 setLastKnownError(RETURN_SUCCESS,"RapidXML::getLastAttributeValue success");
	 ret ="";
	 if(attName.empty())
	 {
			setLastKnownError(RETURN_FAIL,"RapidXML::getLastAttributeValue attribute name empty");
			return m_enErrorId;
	 }

	 if(node == NULL)
	 {
			setLastKnownError(RETURN_FAIL,"RapidXML::getLastAttributeValue node is null");
			return m_enErrorId;
	 }
	 try
	 {
			ret = node->last_attribute(attName.c_str())->value();
	 }
	 catch (const std::runtime_error& e)
	 {
			setLastKnownError(RUNTIME_ERROR, e.what());
			ret = "";
	 }
	 catch (const rapidxml::parse_error& e)
	 {
			setLastKnownError(PARSER_ERROR,e.what());
			ret = "";
	 }
	 catch (const std::exception& e)
	 {
			setLastKnownError(STANDARD_ERROR,e.what());
			ret = "";
	 }
	 catch(...)
	 {
			setLastKnownError(UNKNOWN_ERROR,"Unknown Error occured getLastAttributeValue");
			ret = "";
	 }
	 return m_enErrorId;
}

/* Method Name: createNode
 ** Description: Allocates a new node from the pool, and optionally assigns name and value to it
 ** Parameters: rapidxml::node_type type, std::string nodeName
 ** return: xml_node
 **/
rapidxml::xml_node<>* RapidXML::createNode(rapidxml::node_type type, std::string nodeName)
{
	 setLastKnownError(RETURN_SUCCESS,"RapidXML::createNode successfull");
	 rapidxml::xml_node<>*retNode = NULL;
	 try 
	 {
			switch(type) 
			{
				 case rapidxml::node_declaration:
						{
							 retNode =  getDoc().allocate_node(rapidxml::node_declaration);
							 break;
						}
				 case rapidxml::node_element:
						{
							 if(nodeName.empty())
							 {
									retNode = getDoc().allocate_node(rapidxml::node_element);
							 }
							 else
							 {
									char* pnodeName = getDoc().allocate_string(nodeName.c_str());
									retNode = getDoc().allocate_node(rapidxml::node_element,pnodeName);
							 }
							 break;
						}
				 default:
						{
							 setLastKnownError(RETURN_FAIL,"RapidXML::createNode default case failed");
							 break;
						}
			}
	 } 
	 catch(...) 
	 {
			setLastKnownError(UNKNOWN_ERROR, "Unknown error occured RapidXML::createNode failed");
	 }
	 return retNode;
}

/* Method Name: encodeToXMLString
 ** Description: Prints XML to given output stream
 ** Parameters: nil
 ** return: success|failure
 **/
RET_ERROR RapidXML::encodeToXMLString()
{
	 setLastKnownError(RETURN_SUCCESS,"RapidXML::encodeToXMLString successfull");
	 try
	 {
			rapidxml::print(std::back_inserter(m_strXMLAsString), getDoc());
	 } 
	 catch(...) 
	 {
			setLastKnownError(UNKNOWN_ERROR, "Unknown error occured RapidXML::encodeToXMLString");
	 }
	 return m_enErrorId;
}

/* Method Name: createAttributeValue
 ** Description: Allocates a new attribute from the pool, and optionally assigns name and value to. 
 ** Parameters: rapidxml::xml_node<>*node , std::string key, std::string value
 ** return: success|failure
 **/
RET_ERROR RapidXML::createAttributeValue(rapidxml::xml_node<>*node , std::string key, std::string value)
{
	 setLastKnownError(RETURN_SUCCESS,"RapidXML::createAttributeValue successfull");
	 if (node == NULL) 
	 {
			setLastKnownError(RETURN_FAIL,"RapidXML::createAttributeValue Node is Null failed");
			return m_enErrorId;
	 }
	 if(key.empty() || value.empty())
	 {
			setLastKnownError(RETURN_FAIL,"RapidXML::createAttributeValue key or value is empty failed");
			return m_enErrorId;
	 }

	 try
	 {
			char*pKey =  getDoc().allocate_string(key.c_str());
			char*pValue = getDoc().allocate_string(value.c_str());
			node->append_attribute(getDoc().allocate_attribute(pKey,pValue));
	 }
	 catch(...)
	 {
			setLastKnownError(UNKNOWN_ERROR, "Unknown error occured RapidXML::createAttributeValue failed");
	 }
	 return m_enErrorId;
}

/* Method Name: appendNodeToDoc
 ** Description: Appends a new child node. The appended child becomes the last child
 ** Parameters: rapidxml::xml_node<>*child
 ** return: success|failure
 **/
RET_ERROR RapidXML::appendNodeToDoc(rapidxml::xml_node<>*node)
{
	 setLastKnownError(RETURN_SUCCESS,"RapidXML::appendNodeToDoc successfull");
	 try
	 {
			if(node == NULL)
			{
				 setLastKnownError(RETURN_FAIL,"RapidXML::appendNodeToDoc Node is Null failed");
				 return m_enErrorId;
			}
			getDoc().append_node(node);
	 }
	 catch(...)
	 {
			setLastKnownError(UNKNOWN_ERROR, "Unknown error occured RapidXML::appendNodeToDoc failed");
	 }
	 return m_enErrorId;
}

/* Method Name: prependNodeToDoc
 ** Description: Prepends a new child node. The prepended child becomes the first child, and all existing children are moved one position back
 ** Parameters: rapidxml::xml_node<>*child
 ** return: success|failure
 **/
RET_ERROR RapidXML::prependNodeToDoc(rapidxml::xml_node<>*node)
{
	 setLastKnownError(RETURN_SUCCESS,"RapidXML::prependNodeToDoc successfull");
	 try
	 {
			if(node == NULL)
			{
				 setLastKnownError(RETURN_FAIL,"RapidXML::prependNodeToDoc Node is Null failed");
				 return m_enErrorId;
			}
			getDoc().prepend_node(node);
	 }
	 catch(...)
	 {
			setLastKnownError(UNKNOWN_ERROR, "Unknown error occured RapidXML::prependNodeToDoc failed");
	 }
	 return m_enErrorId;
}

/* Method Name: appendNodeToNode
 ** Description: Inserts a new child node at specified place inside the node. All children after and including the specified node are moved one position back
 ** Parameters: rapidxml::xml_node<>*parent, rapidxml::xml_node<>*child
 ** return: success|failure
 **/
RET_ERROR RapidXML::appendNodeToNode(rapidxml::xml_node<>*parent, rapidxml::xml_node<>*child)
{
	 setLastKnownError(RETURN_SUCCESS,"RapidXML::appendNodeToNode successfull");
	 if(parent == NULL)
	 {
			setLastKnownError(RETURN_FAIL,"RapidXML::appendNodeToNode parent is NULL failed");
			return m_enErrorId;
	 }
	 if(child == NULL)
	 {
			setLastKnownError(RETURN_FAIL,"RapidXML::appendNodeToNode child is NULL failed");
			return m_enErrorId;
	 }
	 try
	 {
			parent->append_node(child);
	 }
	 catch(...)
	 {
			setLastKnownError(UNKNOWN_ERROR, "Unknown error occured RapidXML::appendNodeToNode failed");
	 }
	 return m_enErrorId;
}

/* Method Name: insertNode
 ** Description: Inserts a new child node at specified place inside the node. All children after and including the specified node are moved one position back
 ** Parameters: rapidxml::xml_node<>*parent, rapidxml::xml_node<>*child
 ** return: success|failure
 **/
RET_ERROR RapidXML::insertNode(rapidxml::xml_node<>*parent, rapidxml::xml_node<>*child)
{
	 setLastKnownError(RETURN_SUCCESS,"RapidXML::insertNode successfull");
	 if(parent == NULL)
	 {
			setLastKnownError(RETURN_FAIL,"RapidXML::insertNode parent is NULL failed");
			return m_enErrorId;
	 }
	 if(child == NULL)
	 {
			setLastKnownError(RETURN_FAIL,"RapidXML::insertNode child is NULL failed");
			return m_enErrorId;
	 }
	 try
	 {
			getDoc().insert_node(parent, child);
	 }
	 catch(...)
	 {
			setLastKnownError(UNKNOWN_ERROR, "Unknown error occured RapidXML::insertNode failed");
	 }
	 return m_enErrorId;
}

/* Method Name: removeFirstNode
 ** Description: Removes first child node
 ** Parameters: nil
 ** return: success|failure
 **/
RET_ERROR RapidXML::removeFirstNode()
{
	 setLastKnownError(RETURN_SUCCESS,"RapidXML::removeFirstNode successfull");
	 try
	 {
			if(getDoc().first_node())
			{
				 getDoc().remove_first_node();
			}
			else
			{
				 setLastKnownError(RETURN_FAIL, "Node has no children");
			}
	 }
	 catch(...)
	 {
			setLastKnownError(UNKNOWN_ERROR, "Unknown error occured RapidXML::removeFirstNode failed");
	 }
	 return m_enErrorId;
}

/* Method Name: removeLastNode
 ** Description: Removes last child of the node
 ** Parameters: nil
 ** return: success|failure
 **/
RET_ERROR RapidXML::removeLastNode()
{
	 setLastKnownError(RETURN_SUCCESS,"RapidXML::removeLastNode successfull");
	 try
	 {
			if(getDoc().first_node())
			{
				 getDoc().remove_last_node();
			}
			else
			{
				 setLastKnownError(RETURN_FAIL, "Node has no children");
			}
	 }
	 catch(...)
	 {
			setLastKnownError(UNKNOWN_ERROR, "Unknown error occured RapidXML::removeLastNode failed");
	 }
	 return m_enErrorId;
}

/* Method Name: removeChildNode
 ** Description: Removes specified child from the node
 ** Parameters: rapidxml::xml_node<>*child
 ** return: success|failure
 **/
RET_ERROR RapidXML::removeChildNode(rapidxml::xml_node<>*child)
{
	 setLastKnownError(RETURN_SUCCESS,"RapidXML::removeChildNode successfull");
	 try
	 {
			getDoc().remove_node(child);
	 }
	 catch(...)
	 {
			setLastKnownError(UNKNOWN_ERROR, "Unknown error occured RapidXML::removeChildNode failed");
	 }
	 return m_enErrorId;
}

/* Method Name: removeAllChildNodes
 ** Description: Removes all child nodes (but not attributes)
 ** Parameters: nil
 ** return: success|failure
 **/
RET_ERROR RapidXML::removeAllChildNodes()
{
	 setLastKnownError(RETURN_SUCCESS,"RapidXML::removeAllChildNodes successfull");
	 try
	 {
			getDoc().remove_all_nodes();
	 }
	 catch(...)
	 {
			setLastKnownError(UNKNOWN_ERROR, "Unknown error occured RapidXML::removeAllChildNodes failed");
	 }
	 return m_enErrorId;
}

/* Method Name: prependAttribute
 ** Description: Prepends a new attribute to the node
 ** Parameters: rapidxml::xml_node<>*child, rapidxml::xml_attribute<>*attribute
 ** return: success|failure
 **/
RET_ERROR RapidXML::prependAttribute(rapidxml::xml_node<>*node, rapidxml::xml_attribute<>*attribute)
{
	 setLastKnownError(RETURN_SUCCESS,"RapidXML::prependAttribute successfull");
	 try
	 {
			node->prepend_attribute(attribute);
	 }
	 catch(...)
	 {
			setLastKnownError(UNKNOWN_ERROR, "Unknown error occured RapidXML::prependAttribute failed");
	 }
	 return m_enErrorId;
}

/* Method Name: appendAttribute
 ** Description: Appends a new attribute to the node
 ** Parameters: rapidxml::xml_node<>*child, rapidxml::xml_attribute<>*attribute
 ** return: success|failure
 **/
RET_ERROR RapidXML::appendAttribute(rapidxml::xml_node<>*node, rapidxml::xml_attribute<>*attribute)
{
	 setLastKnownError(RETURN_SUCCESS,"RapidXML::appendAttribute successfull");
	 try
	 {
			node->append_attribute(attribute);
	 }
	 catch(...)
	 {
			setLastKnownError(UNKNOWN_ERROR, "Unknown error occured RapidXML::appendAttribute failed");
	 }
	 return m_enErrorId;
}

/* Method Name: insertAttribute
 ** Description: Inserts a new attribute at specified place inside the node. All attributes after and including the specified attribute are moved one position back
 ** Parameters: rapidxml::xml_node<>*child, rapidxml::xml_attribute<>*where, rapidxml::xml_attribute<>*attribute
 ** return: success|failure
 **/
RET_ERROR RapidXML::insertAttribute(rapidxml::xml_node<>*node, rapidxml::xml_attribute<>*where, rapidxml::xml_attribute<>*attribute)
{
	 setLastKnownError(RETURN_SUCCESS,"RapidXML::insertAttribute successfull");
	 try
	 {
			node->insert_attribute(where, attribute);
	 }
	 catch(...)
	 {
			setLastKnownError(UNKNOWN_ERROR, "Unknown error occured RapidXML::insertAttribute failed");
	 }
	 return m_enErrorId;
}

/* Method Name: removeFirstAttribute
 ** Description: Removes first attribute of the node
 ** Parameters: rapidxml::xml_node<>*node
 ** return: success|failure
 **/
RET_ERROR RapidXML::removeFirstAttribute(rapidxml::xml_node<>*node)
{
	 setLastKnownError(RETURN_SUCCESS,"RapidXML::removeFirstAttribute successfull");
	 try
	 {
			if(node->first_attribute())
			{
				 node->remove_first_attribute();
			}
			else
			{
				 setLastKnownError(RETURN_FAIL, "Node has no attribute");
			}
	 }
	 catch(...)
	 {
			setLastKnownError(UNKNOWN_ERROR, "Unknown error occured RapidXML::removeFirstAttribute failed");
	 }
	 return m_enErrorId;
}

/* Method Name: removeLastAttribute
 ** Description: Removes last attribute of the node
 ** Parameters: rapidxml::xml_node<>*node
 ** return: success|failure
 **/
RET_ERROR RapidXML::removeLastAttribute(rapidxml::xml_node<>*node)
{
	 setLastKnownError(RETURN_SUCCESS,"RapidXML::removeLastAttribute successfull");
	 try
	 {
			if(node->first_attribute())
			{
				 node->remove_last_attribute();
			}
			else
			{
				 setLastKnownError(RETURN_FAIL, "Node has no attribute");
			}
	 }
	 catch(...)
	 {
			setLastKnownError(UNKNOWN_ERROR, "Unknown error occured RapidXML::removeLastAttribute failed");
	 }
	 return m_enErrorId;
}

/* Method Name: removeAttribute
 ** Description: Removes specified attribute from node
 ** Parameters: rapidxml::xml_node<>*node, rapidxml::xml_attribute<>*where
 ** return: success|failure
 **/
RET_ERROR RapidXML::removeAttribute(rapidxml::xml_node<>*node, rapidxml::xml_attribute<>*where)
{
	 setLastKnownError(RETURN_SUCCESS,"RapidXML::removeAttribute successfull");
	 try
	 {
			node->remove_attribute(where);
	 }
	 catch(...)
	 {
			setLastKnownError(UNKNOWN_ERROR, "Unknown error occured RapidXML::removeAttribute failed");
	 }
	 return m_enErrorId;
}

/* Method Name: removeAllAttributes
 ** Description: Removes all attributes of node
 ** Parameters: rapidxml::xml_node<>*node
 ** return: success|failure
 **/
RET_ERROR RapidXML::removeAllAttribute(rapidxml::xml_node<>*node)
{
	 setLastKnownError(RETURN_SUCCESS,"RapidXML::removeAllAttributes successfull");
	 try
	 {
			node->remove_all_attributes();
	 }
	 catch(...)
	 {
			setLastKnownError(UNKNOWN_ERROR, "Unknown error occured RapidXML::removeAllAttributes failed");
	 }
	 return m_enErrorId;
}

