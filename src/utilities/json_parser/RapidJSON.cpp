#include "RapidJSON.h"

using namespace CA;

RapidJSON::RapidJSON()
{}

RapidJSON::~RapidJSON()
{}

RapidJSON::RapidJSON(const char* jsonString)
{
   m_strJSONData = jsonString;
}

/* Method Name: setParserString
 ** Description: To set JSON string
 ** Parameters: const char* jsonString
 ** return: void
 **/
void RapidJSON::setParserString(const char* jsonString)
{
   m_strJSONData = jsonString;
}

/* Method Name: getParserString
 ** Description: To get JSON string
 ** Parameters: nil
 ** return: JSON string
 **/
const char* RapidJSON::getParserString()
{
   return m_strJSONData;
}

/* Method Name: getDoc
 ** Description: To get Document
 ** Parameters: nil
 ** return: rapidjson document
 **/
rapidjson::Document& RapidJSON::getDoc()
{
   return m_doc;
}

/* Method Name: setLastKnownError
 ** Description: To set error id and string
 ** Parameters: RET_ERROR errid, std::string errstr
 ** return: void
 **/
void RapidJSON::setLastKnownError(RET_JSON_ERROR errid, std::string errstr)
{
   m_enErrorId = errid;
   m_strLastKnownError =  errstr;
}

/* Method Name: getLastError
 ** Description: To get error string
 ** Parameters: nil
 ** return: string
 **/
std::string RapidJSON::getLastKnownError()
{
   return m_strLastKnownError;
}

/* Method Name: parseDoc
 ** Description: To parse JSON data
 ** Parameters: nil
 ** return: success|failure
 **/
RET_JSON_ERROR RapidJSON::parseDoc()
{
   try
   {
      //rapidjson::ParseResult ok = m_doc.Parse(getParserString());
      char buffer[sizeof(getParserString())];
      memcpy (buffer , getParserString(),sizeof(getParserString()));
         if(m_doc.ParseInsitu(buffer).HasParseError())
        {
	      setLastKnownError(JSON_PARSER_ERROR,"Parsing Error");
	      return RET_JSON_ERROR::JSON_RETURN_FAIL;
         }
      setLastKnownError(JSON_RETURN_SUCCESS, "JSON String empty");
      if(!m_doc.IsObject())
      return RET_JSON_ERROR::JSON_RETURN_FAIL;
   }
   catch (const std::runtime_error& e)
   {
      setLastKnownError(JSON_RUNTIME_ERROR,e.what());
   }
   catch (const std::exception& e)
   {
      setLastKnownError(JSON_STANDARD_ERROR,e.what());
   }
   catch (...)
   {
      setLastKnownError(JSON_UNKNOWN_ERROR,"Unknown Error");
   }
   return m_enErrorId;
}

/* Method Name: checkDocIsObject
 ** Description: To check whether Document is of type object
 ** Parameters: nil
 ** return: true|false
 **/
bool RapidJSON::checkDocIsObject()
{
   if(!m_doc.IsObject())
   {
      return false;
   }
   return true;
}

/* Method Name: checkDocIsArray
 ** Description: To check whether Document is of type array
 ** Parameters: nil
 ** return: true|false
 **/
bool RapidJSON::checkDocIsArray()
{
   if(!m_doc.IsArray())
   {
      return false;
   }
   return true;
}

/* Method Name: getIntDoc
 ** Description: To get interger value from document for given key
 ** Parameters: const char* key, int &ret
 ** return: true|false
 **/
bool RapidJSON::getIntDoc(const char* key, int &ret)
{
   ret = 0;
   if(m_doc.HasMember(key))
   {
      ret = m_doc[key].GetInt();
   }
   else
   {
      return false;
   }
   return true;
}

/* Method Name: getStringDoc
 ** Description: To get string value from document for given key
 ** Parameters: const char* key, std::string &ret
 ** return: true|false
 **/
bool RapidJSON::getStringDoc(const char* key, std::string &ret)
{
   ret = "";
   if(m_doc.HasMember(key))
   {
      ret = m_doc[key].GetString();
   }
   else
   {
      return false;
   }
   return true;
}

/* Method Name: getValueDoc
 ** Description: To get value from document for given key
 ** Parameters: const char* key, Value& val
 ** return: true|false
 **/
bool RapidJSON::getValueDoc(const char* key, Value& val)
{
   if(!m_doc.HasMember(key))
   {
      return false;
   }
   else
   {
      val = m_doc[key];
      return true;
   }
}

/* Method Name: convertValToString
 ** Description: To convert Val to string
 ** Parameters: Value& val
 ** return: string
 **/
std::string RapidJSON::convertValToString(Value& val)
{
   StringBuffer strBuf;
   Writer<StringBuffer> writer(strBuf);
   val.Accept(writer);
   return strBuf.GetString();
}

/* Method Name: setJsonParam
 ** Description: To set string data in document
 ** Parameters: string &strValue
 ** return: Value
 **/
Value RapidJSON::setJsonParam(std::string &strValue)
{
   Value retVal;
   retVal.SetString(strValue.c_str(), strValue.length(), m_doc.GetAllocator());
   return retVal;
}

/* Method Name: setJsonParam
 ** Description: To set integer data in document
 ** Parameters: unsigned int iValue
 ** return: Value
 **/
Value RapidJSON::setJsonParam(unsigned int iValue)
{
   Value retVal;
   retVal = iValue;
   return retVal;
}

