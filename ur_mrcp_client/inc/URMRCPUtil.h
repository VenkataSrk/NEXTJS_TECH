#ifndef _XGATE_MBC_UTIL_H
#define _XGATE_MBC_UTIL_H
//ace includes
#include <ace/Task.h>
#include <ace/Singleton.h>

//rapidjson includes
#include "rapidjson/document.h"
#include "rapidjson/error/en.h"
#include "rapidjson/writer.h"
#include "rapidjson/reader.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/ostreamwrapper.h"
using namespace rapidjson;

//local includes
#include "xGateUtil.h"
#include "URMRCPBaseMsg.h"

#define XGMBCUTIL URMRCP_Util::instance()   // creating instance for URMRCPUtil singleton class
struct URTimerData
{
  URTimerData(std::string &uid, unsigned int type, int retryCount, IURDefines::MODULE_ID requestorModule, void * ctxData = NULL) :
    m_uid(uid), m_timerType(type), m_retryCount(retryCount), m_requestorModule(requestorModule), m_pCtxdata(ctxData){};
  ~URTimerData(){};
  std::string m_uid;
  unsigned int m_timerType;
  int m_retryCount;
  IURDefines::MODULE_ID m_requestorModule;
  void* m_pCtxdata;
};

class URMRCPUtil : public ACE_Task<ACE_MT_SYNCH> {
  public:  
    URMRCPUtil();
    ~URMRCPUtil(void);
    bool post(URMRCPBaseMsg *pBaseMsg);

    //rapidjson utilily functions - start 
    bool parseJson(const char *jsonData, Document &doc);
    Value setJsonParam(const string &strValue, Document &doc);
    Value setJsonParam(unsigned int iValue);
    //rapidjson utilily functions - end
};

typedef ACE_Unmanaged_Singleton<URMRCPUtil, ACE_Null_Mutex> URMRCP_Util; // creating alias for URMRCPUtil single ton class
#endif
