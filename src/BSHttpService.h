#ifndef _BS_HTTP_SERVICE_
#define _BS_HTTP_SERVICE_

#include "CAUtilityInternal.h"
#include "BSOperations.h"
#define MAX_HTTP_CON 200
//#include "BSGmsc.h"


enum class MultipartContent
{
  EN_MULTIPART_CONTENT_TYPE_UNKNOWN=0,
  EN_MULTIPART_CONTENT_TYPE_DATA=1,
  EN_MULTIPART_CONTENT_TYPE_FILE=2,
};

enum class MultipartField
{
  EN_MULTIPART_FIELD_UNKNOWN=0,
  EN_MULTIPART_FIELD_NAME=1,
  EN_MULTIPART_FIELD_VALUE=2,
};



enum class HttpServiceEvent
{
  EN_HTTP_EVENT_UNKNOWN=0,
  EN_HTTP_EVENT_GET=1,
  EN_HTTP_EVENT_POST=2,
  EN_HTTP_EVENT_PUT=3,
  EN_HTTP_EVENT_MULTIPART=4,
};





class BSHttpService : public CA::CAThread
{
    public:
        BSHttpService()=default;
        virtual ~BSHttpService()=default;
        BSHttpService(const BSHttpService& rhs);
        BSHttpService& operator=(const BSHttpService& rhs);

        bool init();
        bool deInit();
        virtual bool run();
        bool handleMsg(CA::CAMessageBlock* pMsg);
        bool handle_hlr_request(CA::CAMessageBlock *pMsg);
        bool sendHttpRequest(BSOperation *bsp,BSHttpServiceMsg *pHttpMsg);
        bool getHttpResponse(BSOperation *bsp);
        bool fillHttpStatusInfo(BSHttpServiceMsg *httpMsg);
        static size_t write_cb(void *ptr, size_t size, size_t nmemb, void *data);
    
    private:
        bool m_threadStatus;
        CURLM *m_multiHandle;
        static int m_sentCount;
        static int m_recvCount;
        bool initEasyHandle(BSHttpServiceMsg *pHttpMsg);
        std::string getMcodeStr(CURLMcode code);
        std::string generateRequestId();
};

#endif
