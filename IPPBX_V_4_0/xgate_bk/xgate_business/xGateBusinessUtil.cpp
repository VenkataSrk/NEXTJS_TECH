//local includes
#include "log.h"
#include "klog.h"
#include "xGateBusinessUtil.h"
#include "xGateBusinessMsg.h"
#include "xGateBusinessProcessor.h"
#include "xGateSipController.h"
#include "xGateSipSignalProcessor.h"
#include "xGateDBManager.h"
#include "xGateHttpHandler.h"
#include "xGateBusinessScheduler.h"

#define THISMODULE "BusUtil" //Business Manager Util

/* DtmfPattern Implementation - Start */
DtmfPattern::DtmfPattern() : m_startRecord(""), m_stopRecord(""), 
  m_goMainMenu(""), m_repeatMenu(""), m_stopMenu(""), m_confPin(""),
  m_voiceMailPin("")
{
}

DtmfPattern::~DtmfPattern()
{
}
/* DtmfPattern Implementation - End */

/* IvrFileDetail Implementation - Start */
IvrFileDetail::IvrFileDetail() : m_rbt(g_rbt_file), m_rbtOneRing(g_rbt_one_ring_file), 
  m_silence(g_500ms_silence_file), m_callHold(g_hold_stay_on_live_file),
  m_callForward(g_call_forwarded_file), m_connectingMsg(g_connecting_msg_file), 
  m_connectOperator(g_connect_operator_file), m_noInput(g_noinput_goodbye_file),
  m_welcomeVectone(g_welcome_vectone_file), m_userGreeting(""), 
  m_recordStart(g_record_start_file), m_recordStop(g_record_stop_file), 
  m_blockedNumber(g_blocked_number_file),m_empty("")
{
}

IvrFileDetail::~IvrFileDetail()
{
}

/* IvrFileDetail Implementation - End */

/* RecordDetail Implementation - Start */
RecordDetail::RecordDetail() : m_audioEnabled(false), m_audiofileName(""),
  m_audiofilePath(""), m_audioRecordType(EN_AUDIO_RECORD_TYPE_UNKNOWN) 
{
}

RecordDetail::~RecordDetail()
{
}
/* RecordDetail Implementation - End */

/* LegInfo Implementation - Start */
LegInfo::LegInfo() : m_legId(""), m_number(""),
  m_numberType(EN_LEG_NUMBER_TYPE_UNKNOWN), m_domainName(""), m_domainId(0),
  m_deviceType(EN_DEVICE_TYPE_DESKPHONE), m_userStatus(EN_USER_STATUS_AVAILABLE),
  m_isVoiceMailEnabled(0)
{
}

LegInfo::~LegInfo()
{
}

//TODO: assign this task to preethi
bool LegInfo::findNumberType()
{
  if(m_number.empty()) {
    CLog::Error(THISMODULE, "findNumberType failed. Given number value is empty !");
    return false;
  }
  int len = m_number.length();
  switch(len) {
    case 3:
    case 4:
    case 5:
      {
        m_numberType = EN_LEG_NUMBER_TYPE_INTERNAL;   
        break;
      }
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
      {
        if(m_domainName.empty()) {
          m_numberType = EN_LEG_NUMBER_TYPE_EXTERNAL;
        } else {
          m_numberType = EN_LEG_NUMBER_TYPE_SWB;
        }
        break;
      }
    default:
      {
        CLog::Error(THISMODULE, "findNumberType failed. Not able to identify supported number type \
            for number length: %d !", m_number.c_str());
        return false;
      }
  }  
}

string LegInfo::getIvrFile(IvrFileType type)
{
  switch(type) {
    case EN_IVR_RBT:
    {
      return m_ivrFile.m_rbt;
    }
    case EN_IVR_RBT_ONE_RING:
    {
      return m_ivrFile.m_rbtOneRing;
    }
    case EN_IVR_USER_GREETING:
    {
      return m_ivrFile.m_userGreeting; 
    }
    case EN_IVR_SILENCE:
    {
      return m_ivrFile.m_silence; 
    }
    case EN_IVR_HOLD_STAY:
    {
      return m_ivrFile.m_callHold; 
    }
    case EN_IVR_CALL_FORWARD:
    {
      return m_ivrFile.m_callForward; 
    }
    case EN_IVR_CONNECT_MSG:
    {
      return m_ivrFile.m_connectingMsg; 
    }
    case EN_IVR_CONNECT_OPERATOR:
    {
      return m_ivrFile.m_connectOperator; 
    }
    case EN_IVR_NOINPUT_GOODBYE:
    {
      return m_ivrFile.m_noInput; 
    }
    case EN_IVR_WELCOME_VECTONE:
    {
      return m_ivrFile.m_welcomeVectone;
    }
    case EN_IVR_START_RECORD:
    {
      return m_ivrFile.m_recordStart; 
    }
    case EN_IVR_STOP_RECORD:
    {
      return m_ivrFile.m_recordStop; 
    }
    case EN_IVR_BLOCKED_NUMBER: 
    {
      return m_ivrFile.m_blockedNumber; 
    }
    default:
    {
      CLog::Error(THISMODULE, "getIvrFile failed. Unknows ivr file type specified !");
      return "";
    }
  }
}
/* LegInfo Implementation - End */

/* AddressOfRecord Implementation - Start */
AddressOfRecord::AddressOfRecord(string &contactAddr, string &ipAddr, string &proxyUserName, string &sDeviceType) : 
  m_contactAddr(contactAddr), m_ipAddr(ipAddr), m_proxyUserName(proxyUserName), m_sDeviceType(sDeviceType)
{
  getDeviceType();
}

UserDeviceType AddressOfRecord::getDeviceType()
{
  if(m_sDeviceType == "URDESK:") {
    m_iDeviceType = EN_DEVICE_TYPE_DESKPHONE;
  } else if (m_sDeviceType == "URAPP:IOS") {
    m_iDeviceType = EN_DEVICE_TYPE_IOS;
  } else if (m_sDeviceType == "URAPP:AND") {
    m_iDeviceType = EN_DEVICE_TYPE_ANDROID;
  } else if (m_sDeviceType == "URWEB:WEB") {
    m_iDeviceType = EN_DEVICE_TYPE_WEB;
  } else if (m_sDeviceType == "PSTN") {
    m_iDeviceType = EN_DEVICE_TYPE_PSTN;
  } else {
    m_iDeviceType = EN_DEVICE_TYPE_DESKPHONE;
  }
  return m_iDeviceType;
}
/* AddressOfRecord Implementation - End */

Document xGateBusinessUtil::m_doc;
StringBuffer xGateBusinessUtil::m_strBuf;

bool xGateBusinessUtil::init()
{
  readTemplate();
  return true;
}

bool xGateBusinessUtil::readTemplate()
{
  //read the template and make it as json document
  ParseResult ok = m_doc.Parse(json_business_tmpl.c_str());
  if(!ok) {
    CLog::Error(THISMODULE, "xGateBusinessUtil::read template failed due to json parse error: %s !", \
        GetParseError_En(ok.Code()));
    return false;
  } else if(!m_doc.IsObject()) {
    CLog::Error(THISMODULE, "xGateBusinessUtil::read template failed. not able to create document object from json_business template !");
    return false;
  }
  CLog::Detail(THISMODULE, "parsed json_business template successfully");
  return true;
}

Value xGateBusinessUtil::setJsonParam(string &strValue)
{
  Value retVal;
  retVal.SetString(strValue.c_str(), strValue.length(), m_doc.GetAllocator());
  return retVal;
}

Value xGateBusinessUtil::setJsonParam(unsigned int iValue)
{
  Value retVal;
  retVal = iValue;
  return retVal;
}

bool xGateBusinessUtil::sendBusinessData(xGateBusinessData &data)
{
  m_strBuf.Clear();
  Writer<StringBuffer> writer(m_strBuf);
  m_doc["uid"] = setJsonParam(data.m_uid);
  m_doc["opcode"] = setJsonParam(data.m_event);
  m_doc["errcode"] = setJsonParam(data.m_errcode);

  Document subDoc(&m_doc.GetAllocator());
  if(!createSubDoc(data.m_jsonData, subDoc)) {
    CLog::Error(THISMODULE, "sendBusinessData failed. create sub document failed for uid: %s !", data.m_uid.c_str());
    return false;
  }
  switch(data.m_toWhom) {
    case SEND_TO_DB_PROCESSOR:
      {
        m_doc.RemoveMember("input"); //remove existing input object if any
        m_doc.AddMember("input", subDoc, m_doc.GetAllocator());
        break;
      } 
    case SEND_TO_SIGNAL_PROCESSOR:
    case FROM_DB_TO_BUSINESS_PROCESSOR:
      {
        m_doc.RemoveMember("output"); //remove existing input object if any
        m_doc.AddMember("output", subDoc, m_doc.GetAllocator());
        break;
      } 
    default:
      {
        CLog::Error(THISMODULE, "sendBusinessData failed. didn't mention to whom and \
            what kind sub document need to be prepared for uid: %s !", data.m_uid.c_str());
        return false;
      }
  }
  m_doc.Accept(writer);
  return post(m_strBuf.GetString(), m_strBuf.GetLength(), data.m_toWhom);
}

bool xGateBusinessUtil::createSubDoc(string &jsonStr, Document &doc)
{
  if(jsonStr.empty()) {
    CLog::Error(THISMODULE, "createSubDoc failed. given jsonStr vaule is empty!");
    return false;
  }

  ParseResult ok = doc.Parse(jsonStr.c_str());
  if(!ok) {
    CLog::Error(THISMODULE, "createSubDoc failed due to json parse error: %s !", \
        GetParseError_En(ok.Code()));
    return false;
  } else if(!doc.IsObject() && !doc.IsArray()) {
    CLog::Error(THISMODULE, "createSubDoc failed to create json object from given json data: %s!", \
        jsonStr.c_str());
    return false;
  }
  return true;
}

bool xGateBusinessUtil::post(const char* input, unsigned int len, unsigned int toWhom)
{
  char* data = new char[len+1];
  memset(data, 0, (len+1));
  memcpy (data, input, len);
  data[len] = '\0';

  xGateBusinessMsg *pMsg = new xGateBusinessMsg();
  pMsg->setMsgType(EN_XGATE_MSG_FROM_BUSINESS);
  pMsg->set_business_msg_type(EN_BUSINESS_DBLAYER_MSG);
  pMsg->set_data(data);
  pMsg->set_data_len(len);

  printf("***** business message - start ********\n");
  puts(data);
  printf("***** business message - end ********\n");
  CLog::Detail("posting business message: %s", data);

  ACE_Message_Block* pAmb = NULL;
  pAmb = static_cast<xGateBusinessMsg*>(pMsg);
  ACE_Time_Value tval ((time(NULL)));
  tval += ACE_Time_Value(0,1);

  string processorName("");
  ACE_Task<ACE_MT_SYNCH>* taskPtr = NULL;
  switch(toWhom) {
    case SEND_TO_DB_PROCESSOR:
      {
        processorName = "DBManager";
        taskPtr = xGateUtil::getDBManager(); 
        break;
      } 
    case SEND_TO_SIGNAL_PROCESSOR:
      {
        processorName = "SignalingProcessor";
        taskPtr = (xGateSipSignalProcessor*)GetSipPostHandler(XGATEPOSTTYPE::EN_PROCESS);
        break;
      } 
    case FROM_DB_TO_BUSINESS_PROCESSOR:
      {
        pMsg->setMsgType(EN_XGATE_MSG_TO_BUSINESS);
        pMsg->set_business_msg_type(EN_BUSINESS_DBLAYER_MSG);
        processorName = "FromDbToBusinessProcessor";
        taskPtr = xGateUtil::getBusinessProcessor();
        break;
      } 
    default:
      {
        CLog::Error(THISMODULE, "xGateBusinessUtil::post failed. didn't mention to whom the msg should post !");
        return false;
      }
  }

  if(taskPtr->putq(pAmb, &tval)) {
    CLog::Detail(THISMODULE, "successfully posted business message to %s", processorName.c_str());
  } else {
    CLog::Error(THISMODULE, "posting business message to %s failed !", processorName.c_str());
    return false;
  }

  return true;
}

bool xGateBusinessUtil::sendAppNotification(const char* uri, const char* transId)
{
  if(uri[0] == 0) {
    return false;
  }
  xGateHttpHandler *httpHandler = xGateHttpHandler::getInstance();
  if(!httpHandler) {
    CLog::Error(THISMODULE, "sendAppNotiication failed. invalid httphandler object !");
    return false;
  }

  HttpRequestInfo *httpReqInfo = new HttpRequestInfo();
  httpReqInfo->m_url = uri;
  httpReqInfo->m_requestId = transId;
  char jsonData[100]={0};
  sprintf(jsonData, "{\"trans_id\":\"%s\"}", transId);
  httpReqInfo->m_data = jsonData; 
  httpReqInfo->m_requestorType = EN_REQUESTOR_TYPE_BUSINESS_PROCESSOR;

  if(!httpHandler->doHttpRequest(httpReqInfo)) {
    CLog::Error(THISMODULE, "sendAppNotiication failed. send http request failed !");
    delete httpReqInfo;
    return false;
  }
  httpHandler->getHttpResponse();
  return true;
}

long int xGateBusinessUtil::setTimer(string &uid, BusinessTimerType timerType, unsigned int timeoutSecond) 
{
  BusinessTimerData *data = new BusinessTimerData(uid, timerType);
  xGateBusinessScheduler *scheduler = xGateUtil::getBusinessScheduler();
  long int timerId = scheduler->getReactor()->schedule_timer(scheduler, (void*)data, ACE_Time_Value(timeoutSecond, 0));
  CLog::Detail(THISMODULE, "xGateBusinessUtil::setTimerEvent success for uid %s timer_type:%d, with timer_id:%ld", \
    uid.c_str(), timerType, timerId);
  return timerId;
}

bool xGateBusinessUtil::cancelTimer(string &uid, long int timerId) 
{
  xGateBusinessScheduler *scheduler = xGateUtil::getBusinessScheduler();
  if(scheduler->getReactor()->cancel_timer(timerId)) {
    CLog::Detail(THISMODULE, "xGateBusinessUtil::cancelTimer success for uid %s and timer_id:%ld", \
        uid.c_str(), timerId);
    return true;
  }
  return false;
}
