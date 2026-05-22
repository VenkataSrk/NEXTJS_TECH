#ifndef _XGATE_HTTP_UTIL_H
#define _XGATE_HTTP_UTIL_H
//ace includes
#include <ace/Task.h>
#include <ace/Singleton.h>

//rapidjson includes
#include "document.h"
#include "error/en.h"
#include "writer.h"
#include "reader.h"
#include "stringbuffer.h"
#include "prettywriter.h"
#include "ostreamwrapper.h"
using namespace rapidjson;

//local includes
//#include "xGateDefine.h"
#include "xGateUtil.h"
#include "xGBaseMsg.h"

#define XGHTTPUTIL XGHTTPUtil::instance()
class xGateHTTPUtil : public ACE_Task<ACE_MT_SYNCH> {
  public:  
    xGateHTTPUtil();
    ~xGateHTTPUtil(void);
    bool post(xGBaseMsg *pBaseMsg);

    //rapidjson utilily functions - start 
    bool parseJson(const char *jsonData, Document &doc);
    Value setJsonParam(const string &strValue, Document &doc);
    Value setJsonParam(unsigned int iValue);
    //rapidjson utilily functions - end
};

typedef ACE_Unmanaged_Singleton<xGateHTTPUtil, ACE_Null_Mutex> XGHTTPUtil;
#endif
