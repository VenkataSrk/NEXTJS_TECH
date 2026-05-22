#ifndef __RAPID_JSON_H__
#define __RAPID_JSON_H__

#include "../../../includes/rapidjson/document.h"
#include "../../../includes/rapidjson/error/en.h"
#include "../../../includes/rapidjson/writer.h"
#include "../../../includes/rapidjson/reader.h"
#include "../../../includes/rapidjson/stringbuffer.h"
#include "../../../includes/rapidjson/prettywriter.h"
#include "../../../includes/rapidjson/ostreamwrapper.h"

using namespace rapidjson;

namespace CA
{
   typedef enum RET_JSON_ERROR
   {
      JSON_RETURN_FAIL=0,
      JSON_RETURN_SUCCESS,
      JSON_PARSER_ERROR,
      JSON_RUNTIME_ERROR,
      JSON_STANDARD_ERROR,
      JSON_UNKNOWN_ERROR
   }JSON_RETURN_ERROR;

   class RapidJSON
   {
      public:
	 RapidJSON();
	 RapidJSON(const char* jsonString);
	 virtual ~RapidJSON();

	 void setParserString(const char* jsonString);
	 const char* getParserString();
	 rapidjson::Document& getDoc();
	 RET_JSON_ERROR parseDoc();
	 std::string getLastKnownError();
	 bool checkDocIsObject();
	 bool checkDocIsArray();
	 bool getIntDoc(const char* key, int &ret);
	 bool getStringDoc(const char* key, std::string &ret);
	 bool getValueDoc(const char* key, Value& val);
	 std::string convertValToString(Value& val);
	 Value setJsonParam(std::string &strValue);
	 Value setJsonParam(unsigned int iValue);

      private:
	 rapidjson::Document m_doc;
	 const char* m_strJSONData;
	 std::string m_strLastKnownError;
	 RET_JSON_ERROR m_enErrorId;

	 void setLastKnownError(RET_JSON_ERROR errid, std::string errstr);
   };
};
#endif

