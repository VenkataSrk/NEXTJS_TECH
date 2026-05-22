#ifndef _XGATE_BUSINESS_HANDLER
#define _XGATE_BUSINESS_HANDLER

//local includes
#include "xGateSipController.h"
#include "xGateBusinessMsg.h"

class xGateBusinessHandler
{
  public:
    xGateBusinessHandler();
    ~xGateBusinessHandler();
    bool readTemplate();
    bool createRequest(callInfoParameter &cp, xGateBusinessSigEvent event);
    bool formInput(callInfoParameter &cp, xGateBusinessSigEvent event);
    bool getInputDoc(string &input, Document &doc);
    bool getOutputDoc(Document &doc, const Value &outputVal);
    bool postRequest();
    bool parseJsonMsg(const char *jsonData, Document &doc);
    bool processResponse(const char* data, xGateCallObj *callObj);
    bool fillDetail(Document &doc, xGateCallObj *callObj);
    bool fillBusinessInitDetail(const Value &val, xGateCallObj *callObj);
    bool fillForkingDetail(const Value &val, xGateCallObj *callObj);
    bool fillMappedUsersInfo(const Value &val, callInfoParameter &cp);
    bool fillMappedUserInfo(const Value &val, multiDevice*);
    bool handleTimeoutEvent(const Value &val, xGateCallObj *callObj);
    bool getCodec(string &deviceType, int &codec);
    bool getSipEvent(unsigned int &callFeatureType, xGateCallObj *callObj);

  private:
   Document *m_doc;
   StringBuffer *m_strBuf;
};
#endif
