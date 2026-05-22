//system includes

//local includes
#include "log.h"
#include "klog.h"
#include "xGateBusinessMsg.h"
#include "xGateBusinessProcessor.h"

//self includes
#include "xGateBusinessScheduler.h"

#define THISMODULE "BusSchedul"

xGateBusinessScheduler::xGateBusinessScheduler(const char *schedulerType) : 
  xGateScheduler(schedulerType) 
{
  CLog::Detail(THISMODULE, "xGateBusinessScheduler::xGateBusinessScheduler(%s)", schedulerType);
  readTemplate();
}

xGateBusinessScheduler::~xGateBusinessScheduler (void)
{
}//end destructor

bool xGateBusinessScheduler::readTemplate()
{
  //read the template and make it as json document
  ParseResult ok = m_doc.Parse(json_business_tmpl.c_str());
  if(!ok) {
    CLog::Error(THISMODULE, "xGateBusinessScheduler::read template failed due to json parse error: %s !", \
        GetParseError_En(ok.Code()));
    return false;
  } else if(!m_doc.IsObject()) {
    CLog::Error(THISMODULE, "xGateBusinessScheduler::read template failed. not able to create document object from json_business template !");
    return false;
  }
  CLog::Detail(THISMODULE, "parsed json_business template successfully");
  return true;
}

int xGateBusinessScheduler::handle_timeout(const ACE_Time_Value&, const void *arg)
{
  CLog::Detail(THISMODULE, "xGateBusinessScheduler::handle_timeout");
  BusinessTimerData *data = (BusinessTimerData*)arg;
  if (!data) {
    CLog::Error(THISMODULE, "xGateBusinessScheduler::handle_timeout Timerdata is not present");
    return 0;
  }

  switch(data->m_timerType) {
    case EN_BUSINESS_TIMER_APP_NOTIFY:
      {
        CLog::Detail(THISMODULE, "APP_NOTIFY timeout event triggered for uid: %s", data->m_uid.c_str());
        break;
      }
    case EN_BUSINESS_TIMER_CALL_HANDLE:
      {
        CLog::Detail(THISMODULE, "CALL_HANDLE timeout event triggered for uid: %s", data->m_uid.c_str());
        break;
      }
    case EN_BUSINESS_TIMER_DIAL_OUT:
      {
        CLog::Detail(THISMODULE, "DIAL_OUT timeout event triggered for uid: %s", data->m_uid.c_str());
        break;
      }
    case EN_BUSINESS_TIMER_SWB_MAIN_IVR:
      {
        CLog::Detail(THISMODULE, "SWB_MAIN_IVR timeout event triggered for uid: %s", data->m_uid.c_str());
        break;
      }
    case EN_BUSINESS_TIMER_CONF_DTMF_DIGIT: 
      {
        CLog::Detail(THISMODULE, "CONF_DTMF_DIGIT timeout event triggered for uid: %s", data->m_uid.c_str());
        break;
      }
    default:
      {
        CLog::Detail(THISMODULE, "Unknown/Unsupported timeout event triggered for uid: %s", data->m_uid.c_str());
        break;
      }
  }
  sendTimeEventResult(data);
  delete data;
  return 0;
}

Value xGateBusinessScheduler::setJsonParam(string &strValue)
{
  Value retVal;
  retVal.SetString(strValue.c_str(), strValue.length(), m_doc.GetAllocator());
  return retVal;
}

Value xGateBusinessScheduler::setJsonParam(unsigned int iValue)
{
  Value retVal;
  retVal = iValue;
  return retVal;
}

bool xGateBusinessScheduler::sendTimeEventResult(BusinessTimerData *data)
{
  m_strBuf.Clear();
  Writer<StringBuffer> writer(m_strBuf);
  m_doc["uid"] = setJsonParam(data->m_uid);
  m_doc["opcode"] = setJsonParam(EN_SCHEDULER_EVENT_TIMEOUT);

  
  char output[100] ={0};
  sprintf(output, "%stimer_type\":%d}", jbegin, data->m_timerType);

  Document subDoc(&m_doc.GetAllocator());
  if(!createSubDoc(output, subDoc)) {
    CLog::Error(THISMODULE, "sendTimeEventResult failed. create sub document failed for uid: %s !", data->m_uid.c_str());
    return false;
  }

  m_doc.RemoveMember("input"); //remove existing input object if any
  m_doc.AddMember("input", subDoc, m_doc.GetAllocator());
  m_doc.Accept(writer);
  //TODO: Yoga, pls check whether we could send this timeout result directly to signalling processor
  return post(m_strBuf.GetString(), m_strBuf.GetLength(), FROM_SCHEDULER_TO_BUSINESS_PROCESSOR);
}

bool xGateBusinessScheduler::createSubDoc(char *jsonStr, Document &doc)
{
  if(jsonStr[0] == 0) {
    CLog::Error(THISMODULE, "createSubDoc failed. given jsonStr vaule is empty!");
    return false;
  }

  ParseResult ok = doc.Parse(jsonStr);
  if(!ok) {
    CLog::Error(THISMODULE, "createSubDoc failed due to json parse error: %s !", \
        GetParseError_En(ok.Code()));
    return false;
  } else if(!doc.IsObject() && !doc.IsArray()) {
    CLog::Error(THISMODULE, "createSubDoc failed to create json object from given json data: %s!", \
        jsonStr);
    return false;
  }
  return true;
}

bool xGateBusinessScheduler::post(const char* input, unsigned int len, unsigned int toWhom)
{
  char* data = new char[len+1];
  memset(data, 0, (len+1));
  memcpy (data, input, len);
  data[len] = '\0';

  xGateBusinessMsg *pMsg = new xGateBusinessMsg();
  pMsg->setMsgType(EN_XGATE_MSG_TO_BUSINESS);
  pMsg->set_business_msg_type(EN_BUSINESS_SCHEDULER_MSG);
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
    case FROM_SCHEDULER_TO_BUSINESS_PROCESSOR:
      {
        pMsg->setMsgType(EN_XGATE_MSG_TO_BUSINESS);
        pMsg->set_business_msg_type(EN_BUSINESS_SCHEDULER_MSG);
        processorName = "FromSchedulerToBusinessProcessor";
        taskPtr = xGateUtil::getBusinessProcessor();
        break;
      } 
    default:
      {
        CLog::Error(THISMODULE, "xGateBusinessScheduler::post failed. didn't mention to whom the msg should post !");
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
