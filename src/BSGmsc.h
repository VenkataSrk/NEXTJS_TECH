#ifndef BUSINESS_GMSC
#define BUSINESS_GMSC


#include "BSDAT.h"
#include "BSRouting.h"


enum class HttpContentType
{
  EN_CONTENT_TYPE_UNKNOWN=0,
  EN_CONTENT_TYPE_TEXT=1,
  EN_CONTENT_TYPE_JSON=2,
  EN_CONTENT_TYPE_XML=3,
};
enum class HttpReqMethod
{
  EN_HTTP_METHOD_UNKNOWN=0,
  EN_HTTP_METHOD_GET=1,
  EN_HTTP_METHOD_POST=2,
  EN_HTTP_METHOD_PUT=3,
  EN_HTTP_METHOD_MULTIPART=4,
  EN_HTTP_METHOD_DELETE=5,
};
enum class HttpResponseStatus
{
  EN_HTTP_STATUS_UNKNOWN=0,
  EN_HTTP_STATUS_SUCCESS=1,
  EN_HTTP_STATUS_FAILED=2,
};

class BSHttpServiceMsg {
    public:
        BSHttpServiceMsg() : m_easyHandle(nullptr), m_strUrl(""), m_strRequestId(""), 
            m_strContextId(""), m_strOutput(""), m_iStatusCode(0),
            m_enContentType(HttpContentType::EN_CONTENT_TYPE_JSON), 
            m_enReqMethod(HttpReqMethod::EN_HTTP_METHOD_UNKNOWN),
            m_enRespStatus(HttpResponseStatus::EN_HTTP_STATUS_UNKNOWN) 
            {
                memset(m_err, '\0', CURL_ERROR_SIZE * sizeof(char)); 
            }

        ~BSHttpServiceMsg()
        {
            if(m_easyHandle)
            {
                curl_easy_cleanup(m_easyHandle);
                m_easyHandle = nullptr;
            }
	    if(m_cBody)
	    {
		    delete m_cBody;
		    m_cBody = nullptr;
	    }
        }

        void setUrl(std::string url){m_strUrl = url;}
        void setHeader(std::string header){m_strHeader = header;}
        void setContextId(std::string id){m_strContextId = id;}
        void setRequestId(std::string reqId){m_strRequestId = reqId;}
        void setContextType(HttpContentType type){m_enContentType = type;}
        void setOutput(std::string out){m_strOutput = out;}
        void setStatusCode(int code){m_iStatusCode = code;}
        void setReqMethod(HttpReqMethod method){m_enReqMethod = method;}
        void setRespStatus(HttpResponseStatus res){m_enRespStatus = res;}
        void setHandle(CURL* hndl){m_easyHandle = hndl;}
        void setAppID( int id){ m_appId = id;}
	void setBody(std::string str)
	{
		m_cBody = new char[strlen(str.c_str())+1];
		memset(m_cBody, '\0', strlen(m_cBody));
		strcpy(m_cBody, str.c_str());
	}

        std::string getUrl(){return m_strUrl;}
        std::string getHeader(){return m_strHeader;}
        std::string getContextId(){return m_strContextId;}
        std::string getRequestId(){return m_strRequestId;}
        HttpContentType getContextType(){return m_enContentType;}
        std::string getOutput(){return m_strOutput;}
        int getStatusCode(){return m_iStatusCode;}
        HttpReqMethod getReqMethod(){return m_enReqMethod;}
        HttpResponseStatus getRespStatus(){return m_enRespStatus;}
        char m_err[CURL_ERROR_SIZE];
        CURL* getHandle(){return m_easyHandle;}
        int getAppId(){return m_appId;}
	char* getBody(){return m_cBody;}

    private:
        CURL *m_easyHandle;
        std::string m_strUrl;
        std::string m_strHeader;
        std::string m_strContextId;
        std::string m_strRequestId;
        std::string m_strOutput;
        HttpContentType m_enContentType;
        HttpReqMethod m_enReqMethod;
        HttpResponseStatus m_enRespStatus;
        unsigned int m_iStatusCode;
        int m_appId;
	char *m_cBody;

        std::vector<std::pair<std::string, std::string>> m_multipartFileInfo;
        std::vector<std::pair<std::string, std::string>> m_multipartDataInfo;
        
};
class BSGmsc: public BSInterface{
    private:
        std::string m_strSiteCode;
        int m_strSwitchCode;
		std::string m_strCli;
		std::string m_strDdi;
        std::string m_strDBProfile;
        std::string m_strHlrUrl;
        std::string m_strHlrUssd;
        std::string local_ip_address;
        std::string m_strDBMnpProfile;
        std::string m_traceFilter;
        std::string m_xlate_in_legA,m_xlate_in_legB;
        std::string xlate_out;
		std::string m_strIwmscPrefix;
		std::string m_strMvnoPrefix;
		std::string m_strTrunkName;
		std::string m_strAppID;
		CallNumberType m_enCntCalled;
		CallNumberType m_enCntCalling;
        //multi number
        RMultinumber multinb;
	    int m_subs_state;
		BSGmsc* pObj;
		RApp_Info the_app;	
		RCall_Info	the_legA;
		RCall_Info	the_legB;
		RCall_Info	the_legFwd;
		RSubscriber the_subs;
		RForwarding	fwd_Busy;
		RForwarding	fwd_NoReach;
		RForwarding	fwd_NoReply;
		RForwarding	fwd_Uncond;
		RCall_Param	the_callparam;
		RAccount	the_acc;
		int min_did_len;
		int 	fwd_mode;
		int fwd_type;
		int	fwd_ctr;
		bool isForwarding;
		bool isNonSub;
		bool isRoaming;
		bool isBConnected;

		//vox_file
		bool is_voiceresource_exist;
		bool is_dynamic_voice_alloc;
		std::string vox_detached_subs;
		std::string vox_unknown_subs;
		std::string vox_off_subs;
		std::string vox_callbarring_subs;
		std::string vox_unreach_subs;
		std::string vox_busy_subs;
		NonSubsType nonsubs_type;
		std::string CRBTtest;

		//special routing
		bool isNeedDeleteSR;

		std::string m_strCRF; //call ref ID
		std::unique_ptr<BSRouting> m_rsInterface;
		std::unique_ptr<BSHttpServiceMsg> m_pHttpMsg;
	public:

		retCode Load(CA::CAMap<CA::DBInterface*>& dblist, BSParam* bsp, Application app);
		retCode UnLoad(BSParam *bsp, std::time_t connecttime, std::time_t disconnecttime, std::time_t starttime);
		std::string XlatIn(CA::DBInterface* dbInt, BSParam* bsp, int type);
		std::string XlatConcat(std::string nb, std::string pfx, std::string xin);
		std::string XlatMNP(CA::DBInterface* dbMnpInt, std::string &np, int &is_onward, int &is_ported);
		std::string DDI2MSISDN(CA::DBInterface* dbInt, std::string ddi);
		std::string XlatOut(CA::DBInterface* dbInt, BSParam *bsp , std::string nb);
		SubscriberStatus checkSubscriber(CA::DBInterface* dbInt, std::string &errmsg);
		retCode checkNonSubscriberNB(CA::DBInterface* dbInt, CA::DBInterface* dbMnpInt, std::string &nb);
		retCode ConnectNonSubs(CA::DBInterface* dbInt, BSParam *bsp);
		retCode SIMBasedRouting(CA::DBInterface* dbInt, BSParam* bsp, std::string &simbased_nb);
		std::string XlatBySpecialRouting(CA::DBInterface* dbInt, BSParam* bsp, std::string &nb);
		retCode Connect(CA::DBInterface *dbInt, BSParam* bsp);
		retCode CallForward(CA::DBInterface *dbInt, BSParam *bsp);
		retCode setForward(CA::DBInterface *dbInt, BSParam *bsp, RForwarding fwd);
		retCode HLRRequest(BSParam *bsp);
		//retCode HLRResponse(CA::CAMap<CA::DBInterface*>& dblist,BSHttpServiceMsg *httpMsg, BSParam *bsp);
		//retCode HLRResponse(CA::CAMap<CA::DBInterface*>& dblist,BSOperation *bsOpr, BSParam *bsp);
		retCode HLRResponse(CA::CAMap<CA::DBInterface*>& dblist, BSParam *bsp,int iHttpStatusCode,std::string strHttpOutput, Application app);
		std::unique_ptr<BSHttpServiceMsg> getHttpServiceMsg(){ return std::move(m_pHttpMsg);}
		//std::string getTime();
		std::string& ltrim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
		{
			str.erase(0, str.find_first_not_of(chars));
			return str;
		}
		std::string& rtrim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
		{
			str.erase(str.find_last_not_of(chars) + 1);
			return str;
		}
		std::string& trim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
		{
			return ltrim(rtrim(str, chars), chars);
		}
    
		BSGmsc()=default;
		virtual ~BSGmsc();
		BSGmsc(BSParam* bsp, Application app);
		

	public:
		BSDAT *m_datUpdate;
		void postToDAT();
};
#endif
