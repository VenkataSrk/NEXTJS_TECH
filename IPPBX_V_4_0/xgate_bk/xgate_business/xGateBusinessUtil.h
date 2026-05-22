#ifndef _XGATE_BUSINESS_UTIL_H
#define _XGATE_BUSINESS_UTIL_H

//system includes

//local includes
#include "xGateUtil.h"

#define SEND_TO_DB_PROCESSOR 1
#define SEND_TO_SIGNAL_PROCESSOR 2
#define FROM_DB_TO_BUSINESS_PROCESSOR 3
#define FROM_SCHEDULER_TO_BUSINESS_PROCESSOR 3

const unsigned int MAX_FILE_SIZE = 100;
const char* g_wav_files_path = "/root/wav_files/sounds/vectone/";
const char* g_rbt_file = "/root/wav_files/sounds/vectone/UK_ringback_tone-0001.wav";
const char* g_rbt_one_ring_file = "/root/wav_files/sounds/vectone/UK_ringback_tone_one_ring.wav";
const char* g_500ms_silence_file = "/root/wav_files/sounds/vectone/500ms_silence.wav";
const char* g_hold_stay_on_live_file = "/root/wav_files/sounds/vectone/hold_stay_on_line.wav";
const char* g_call_forwarded_file = "/root/wav_files/sounds/vectone/call_has_been_forwarded.wav";
const char* g_connecting_msg_file = "/root/wav_files/sounds/vectone/SB_hold_line_connecting.wav";
const char* g_connect_operator_file = "/root/wav_files/sounds/vectone/please_wait_connect_operator.wav";
const char* g_noinput_goodbye_file = "/root/wav_files/sounds/vectone/no_input_goodbye.wav";
const char* g_welcome_vectone_file = "/root/wav_files/sounds/vectone/SB_welcome_vectone.wav";
const char* g_record_start_file = "/root/wav_files/sounds/vectone/record_the_call.wav";
const char* g_record_stop_file = "/root/wav_files/sounds/vectone/call_recorded.wav";
const char* g_blocked_number_file = "/root/wav_files/sounds/vectone/1_blockedgreeting.wav";

typedef enum
{
  EN_LEG_NUMBER_TYPE_UNKNOWN=0,
  EN_LEG_NUMBER_TYPE_EXTERNAL=1,  //pstn number
  EN_LEG_NUMBER_TYPE_INTERNAL=2,  //normal extension number
  EN_LEG_NUMBER_TYPE_SWB=2,       //switch boards number
  EN_LEG_NUMBER_TYPE_VMS=3,       //voice mail number
  EN_LEG_NUMBER_TYPE_CONF=4,      //confiernce number
}LegNumberType;

typedef enum
{
  EN_AUDIO_RECORD_TYPE_UNKNOWN=0,
  EN_AUDIO_RECORD_TYPE_DEFAULT=1,
  EN_AUDIO_RECORD_TYPE_ONDEMAND=2,
}AudioRecordType;

typedef enum
{
  EN_USER_STATUS_DND=0,
  EN_USER_STATUS_AVAILABLE=1,
}UserAvailabilityStatus;

typedef enum
{
  EN_CALL_FEATURE_TYPE_INTERNAL=0,
  EN_CALL_FEATURE_TYPE_EXTERNAL=1,
  EN_CALL_FEATURE_TYPE_SWB=2,
  EN_CALL_FEATURE_TYPE_VMS=3,
  EN_CALL_FEATURE_TYPE_CONF=4,
  EN_CALL_FEATURE_TYPE_MAX
}CallFeatureType;

typedef enum
{
  EN_DEVICE_TYPE_DESKPHONE=0,
  EN_DEVICE_TYPE_IOS=1,
  EN_DEVICE_TYPE_ANDROID=2,
  EN_DEVICE_TYPE_WEB=3,
  EN_DEVICE_TYPE_WINDOWS=4,
  EN_DEVICE_TYPE_PSTN=5,
  EN_DEVICE_TYPE_MAX
}UserDeviceType;

typedef enum
{
  EN_IVR_RBT=0,
  EN_IVR_RBT_ONE_RING=1,
  EN_IVR_USER_GREETING=2,
  EN_IVR_SILENCE=3,
  EN_IVR_HOLD_STAY=4,
  EN_IVR_CALL_FORWARD=5,
  EN_IVR_CONNECT_MSG=6,
  EN_IVR_CONNECT_OPERATOR=7,
  EN_IVR_NOINPUT_GOODBYE=8,
  EN_IVR_WELCOME_VECTONE=9,
  EN_IVR_START_RECORD=10,
  EN_IVR_STOP_RECORD=11,
  EN_IVR_BLOCKED_NUMBER=12,
}IvrFileType;

typedef enum
{
  EN_BUSINESS_TIMER_APP_NOTIFY=0,
  EN_BUSINESS_TIMER_CALL_HANDLE=1,
  EN_BUSINESS_TIMER_DIAL_OUT=2,
  EN_BUSINESS_TIMER_SWB_MAIN_IVR=3,
  EN_BUSINESS_TIMER_CONF_DTMF_DIGIT=4,
  EN_BUSINESS_TIMER_MAX,
}BusinessTimerType;

struct BusinessTimerData
{
  BusinessTimerData(string &uid, BusinessTimerType type) : 
    m_uid(uid), m_timerType(type){};
  ~BusinessTimerData(){};
  BusinessTimerType m_timerType;
  string m_uid;
};

class DtmfPattern
{
  public:
    DtmfPattern();
    ~DtmfPattern();
    DtmfPattern(const DtmfPattern& rhs);
    DtmfPattern& operator= (const DtmfPattern& rhs);

    //member variables
    string m_startRecord;
    string m_stopRecord;
    string m_goMainMenu;
    string m_repeatMenu;
    string m_stopMenu;
    string m_confPin;
    string m_voiceMailPin;

    //member functions
};

class IvrFileDetail //voice play file detail structure
{
  public:
    IvrFileDetail();
    ~IvrFileDetail();
    IvrFileDetail(const IvrFileDetail& rhs);
    IvrFileDetail& operator= (const IvrFileDetail& rhs);

    //member variables
    string m_rbt;
    string m_rbtOneRing;
    string m_silence;
    string m_callHold;
    string m_callForward;
    string m_connectingMsg;
    string m_connectOperator;
    string m_noInput;
    string m_welcomeVectone;

    string m_userGreeting;
    string m_recordStart;
    string m_recordStop;
    string m_blockedNumber;
    string m_empty; //added for return empty string as reference

    //member functions
};

class RecordDetail 
{
  public:
    RecordDetail();
    ~RecordDetail();
    RecordDetail(const RecordDetail& rhs);
    RecordDetail& operator= (const RecordDetail& rhs);

    //member variables
    bool m_audioEnabled;
    string m_audiofileName;
    string m_audiofilePath;
    AudioRecordType m_audioRecordType;

    //member functions
};

class LegInfo 
{
  public:
    LegInfo();
    ~LegInfo();
    LegInfo(const LegInfo& rhs);
    LegInfo& operator= (const LegInfo& rhs);

    //member variables
    string m_legId;
    string m_number;
    LegNumberType m_numberType;
    string m_domainName;
    unsigned int m_domainId;
    UserDeviceType m_deviceType;        
    unsigned int m_isVoiceMailEnabled;
    UserAvailabilityStatus m_userStatus;
    DtmfPattern m_dtmfPattern;
    IvrFileDetail m_ivrFile;
    RecordDetail m_recordDetail;

    //member functions
    bool findNumberType();
    string getIvrFile(IvrFileType type);

};

class AddressOfRecord
{
  public:
    AddressOfRecord(){};
    AddressOfRecord(string &contactAddr, string &ipAddr, string &proxyUserName, string &sDeviceType); 
    ~AddressOfRecord(){};
    string m_contactAddr, m_ipAddr, m_proxyUserName, m_sDeviceType;
    UserDeviceType m_iDeviceType;
    UserDeviceType getDeviceType();
};

class xGateBusinessData
{
  public:
    xGateBusinessData(){};
    xGateBusinessData(string &uid, unsigned int event, string jsonData, int toWhom, unsigned int errcode = 1) : 
      m_uid(uid), m_event(event), m_jsonData(jsonData), m_toWhom(toWhom), m_errcode(errcode){};
    ~xGateBusinessData(){};
    string m_uid, m_jsonData;
    unsigned int m_event; 
    unsigned int m_errcode;
    unsigned int m_toWhom;
};

class xGateBusinessUtil 
{
  public:
    static bool post(const char* input, unsigned int len, unsigned int toWhom);
    static bool init();
    static bool sendBusinessData(xGateBusinessData &data);
    static bool sendAppNotification(const char* uri, const char* transId);
    static long int setTimer(string &uid, BusinessTimerType timerType, unsigned int timeoutSecond); 
    static bool cancelTimer(string &uid, long int timerId);
  private:
    xGateBusinessUtil();
    xGateBusinessUtil(const xGateBusinessUtil& rhs);
    xGateBusinessUtil& operator= (const xGateBusinessUtil& rhs);

    static Document m_doc;
    static StringBuffer m_strBuf;
    static bool readTemplate();
    static Value setJsonParam(string &strValue);
    static Value setJsonParam(unsigned int iValue);
    static bool createSubDoc(string &jsonStr, Document &doc);
};

#endif
