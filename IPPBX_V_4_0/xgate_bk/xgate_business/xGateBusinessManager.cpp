
//local includes
#include "log.h"
#include "klog.h"

//self includes
#include "xGateBusinessManager.h"
#include "xGateBusinessUtil.h"
#include "xGatePBXCDR.h"
#define THISMODULE "BusMgr"

xGateBusinessManager* xGateBusinessManager::m_mgrInstance = NULL;

xGateBusinessManager::xGateBusinessManager() :
  m_uid(""), m_opcode(0), m_businessInfo(NULL) 
{
  CLog::Warning(THISMODULE, "xGateBusinessManager::xGateBusinessManager");
}

xGateBusinessManager::~xGateBusinessManager()
{
  CLog::Warning(THISMODULE, "xGateBusinessManager::~xGateBusinessManager");
}

xGateBusinessManager * xGateBusinessManager::getInstance()
{
  if(m_mgrInstance == NULL) {
      m_mgrInstance = new xGateBusinessManager();
      if(!m_mgrInstance->init()) {
        CLog::Error(THISMODULE, "initializing business manager class failed !");
        delete m_mgrInstance;
        m_mgrInstance = NULL;
      }
  }
  return m_mgrInstance;
}

void xGateBusinessManager::deleteInstance()
{
  if(m_mgrInstance != NULL) {
    if(m_mgrInstance != NULL) {
      CLog::Detail(THISMODULE, "de-initializing and destructing log module");
      m_mgrInstance->deInit();
      delete m_mgrInstance;
      m_mgrInstance = NULL;
    }
  }
}

//TODO: need to read common business values from system.conf file 
bool xGateBusinessManager::init()
{
  xGateBusinessUtil::init();
  return true;
}

bool xGateBusinessManager::deInit()
{
  return false;
}

bool xGateBusinessManager::parseJsonMsg(const char *jsonData, Document &doc)
{
  CLog::Detail(THISMODULE, "parsing business json data: %s", jsonData);
  ParseResult ok = doc.Parse(jsonData);
  if(!ok) {
    CLog::Error(THISMODULE, "parseJsonMsg failed. Encountered with json parse error !");
    return false;
  } else if(!doc.IsObject()) {
    CLog::Error(THISMODULE, "parseJsonMsg failed. Not able create documnet object !");
    return false;
  }
  
  //before filling new values reset the below member vairables.
  m_uid.clear();
  m_opcode = 0;
  m_businessInfo = NULL;

  CLog::Detail(THISMODULE, "successfully parsed business json data");
  if(doc.HasMember("uid")) {
    m_uid = doc["uid"].GetString();
  } else {
    CLog::Error(THISMODULE, "process_msg failed. 'uid' is missing in business json message !");
    return false;
  }

  if(doc.HasMember("opcode")) {
    m_opcode = doc["opcode"].GetInt();
  } else {
    CLog::Error(THISMODULE, "process_msg failed. 'opcode' is missing in json message !");
    return false;
  }
  find_business_info_entry(); 
  return true;
}


bool xGateBusinessManager::process_msg(xGateBusinessMsg *pMsg)
{
  const char  *data = 0;
  unsigned int dataLen = 0;
  pMsg->get_data(data);
  dataLen = pMsg->get_data_len();

  if(!data && dataLen <=0) {
    CLog::Error(THISMODULE, "process_msg failed. Invalid data in xGateBusinessMsg !");
    return false;
  }

  //step1 : parse json message
  Document doc;
  if(!parseJsonMsg(data, doc)) {
    CLog::Error(THISMODULE, "process_msg failed to parse business message !");
    return false;
  }

  //step2: check from which layer we got this business message
  switch(pMsg->get_business_msg_type()) {
    case EN_BUSINESS_SIGLAYER_MSG:
      {
        if(!process_siglayer_msg(doc)) {
          CLog::Error(THISMODULE, "process_msg failed. process_siglayer_msg failed !");
          return false;
        }
        break;
      }
    case EN_BUSINESS_DBLAYER_MSG:
      {
        if(!process_dblayer_msg(doc)) {
          CLog::Error(THISMODULE, "process_msg failed. process_dblayer_msg failed !");
          return false;
        }
        break;
      } 
    case EN_BUSINESS_SCHEDULER_MSG:
      {
        if(!process_scheduler_msg(doc)) {
          CLog::Error(THISMODULE, "process_msg failed. process_scheduler_msg failed !");
          return false;
        }
        break;
      } 
    case EN_BUSINESS_UNKNOWN_MSG:
    default:
      {
        CLog::Error(THISMODULE, "process_msg failed. Unknown business message !");
        return false;
      } 
  }

  return true;
}

/*
* handle business messages coming from signaling layer
*/
bool xGateBusinessManager::process_siglayer_msg(Document &doc)
{
  const Value inputVal;
  if(!getInputDoc(doc, inputVal)) {
    CLog::Error(THISMODULE, "do_business_init failed. not able to get 'input' object from main json object !");
    return false;
  }

  switch(m_opcode) {
    case EN_BUSINESS_EVENT_INIT:
      {
        if(!do_business_init(doc, inputVal)) {
          CLog::Error(THISMODULE, "process_siglayer_msg failed. do_business_init failed !");
          return false;
        }
        break;
      }
    case EN_BUSINESS_EVENT_FORKING:
      {
        if(!get_aor(doc, inputVal)) {
          CLog::Error(THISMODULE, "process_siglayer_msg failed. get_aor failed !");
          return false;
        }
        break;
      }
    case EN_BUSINESS_EVENT_CDRUPDATE:
    if(!update_CDR_Information(doc,inputVal)){
      CLog::Error(THISMODULE, "process_siglayer_msg failed. update_CDR_Information !");
	return false;
    }
    break;
    case EN_BUSINESS_UNKNOWN_MSG:
    default:
      {
        CLog::Error(THISMODULE, "process_siglayer_msg failed. Unknown business message from signalling layer !");
        return false;
      } 
  }
  return true;
}

bool xGateBusinessManager::getInputDoc(Document &doc, const Value &inputVal)
{
  if(doc.HasMember("input")) {
    inputVal = doc["input"].GetObject();
    return true;
  }
  return false;
}
/**/
bool xGateBusinessManager::update_CDR_Information(Document &doc, const Value &val){
StringBuffer sb;
Writer<StringBuffer> writer(sb);
val.Accept(writer);
std::string cdrString = sb.GetString();
CLog::Info(THISMODULE,"Update_CDR_Information Value %s", cdrString.c_str());
//call xGatePBXCDR function
xGatePBXCDR cdr;
cdr.postMessage(cdrString, doc["uid"].GetString());
return true;
}
bool xGateBusinessManager::do_business_init(Document &doc, const Value &val)
{
  string cli(""), ddi(""), domainName("");

  Value::ConstMemberIterator itr = val.MemberBegin();
  Value::ConstMemberIterator itrEnd = val.MemberEnd();

  for (itr; itr != itrEnd; ++itr) {
    if(!strcmp(itr->name.GetString(), "cli")) {
      cli = itr->value.GetString();
    } else if(!strcmp(itr->name.GetString(), "ddi")) {
      ddi = itr->value.GetString();
    } else if(!strcmp(itr->name.GetString(), "domain_name")) {
      domainName = itr->value.GetString();
    } 
  }
  
  if(cli.empty() || ddi.empty()) { //Note: for PSTN call domain name will be unknown
    CLog::Error(THISMODULE, "do_business_init failed. either cli/ddi/domainName value is empty !");
    return false;
  }

  CLog::Detail(THISMODULE, "do_business_init for cli:%s, ddi:%d, domain_name:%s", cli.c_str(), \
      ddi.c_str(), domainName.c_str());

  if(!m_businessInfo) {
    m_businessInfo = new xGateBusinessInfo(m_uid, cli, ddi, domainName);
    if(!m_businessInfo->init()) {
      CLog::Error(THISMODULE, "do_business_init failed due to businessInfo initialization failure for uid:%s !", \
          m_uid.c_str());
      return false;
    }
    //on success put this businessInfo entry into map
    add_business_info_entry();
  } else {
    CLog::Error(THISMODULE, "do_business_init already done for uid:%s !", m_uid.c_str());
    return false;
  }

  return true;
}

bool xGateBusinessManager::get_aor(Document &doc, const Value &val)
{
  if(!m_businessInfo) {
    CLog::Error(THISMODULE, "get_aor failed. businessInfo object not found for uid:%s !", \
        m_uid.c_str());
    return false;
  }

  if(m_businessInfo) {
    if(m_businessInfo->get_aor(val)) {
      CLog::Error(THISMODULE, "get_aor failed for uid:%s !", m_uid.c_str());
      return false;
    }
  }

  return true;
}

/*
* handle business messages coming from dblayer
*/
bool xGateBusinessManager::process_dblayer_msg(Document &doc)
{
  if(!m_businessInfo) {
    CLog::Error(THISMODULE, "process_dblayer_msg failed. invalid BusinessInfo object for uid: %s !", m_uid.c_str());
    return false;
  }

  const Value outputVal;
  if(!getOutputDoc(doc, outputVal)) {
    CLog::Error(THISMODULE, "process_dblayer_msg failed. not able to get 'output' object from main json object !");
    return false;
  }
  switch(m_opcode) {
    case EN_DB_EVENT_PREQUERY:
      {
        CLog::Detail(THISMODULE, "process_deblayer_msg EN_DB_EVENT_PREQUERY !");
        if(!m_businessInfo->use_prequery_result(outputVal)) {
          CLog::Error(THISMODULE, "process_dblayer_msg failed. use_prequery_result failed !");
          return false; 
        }
        break;
      }
    case EN_DB_EVENT_GET_AOR:
      {
        CLog::Detail(THISMODULE, "process_deblayer_msg EN_DB_EVENT_GET_AOR !");
        if(!m_businessInfo->use_aor_result(outputVal)) {
          CLog::Error(THISMODULE, "process_dblayer_msg failed. use_prequery_result failed !");
          return false; 
        }
        break;
      }
    case EN_DB_EVENT_UNKNOWN:
    default:
      {
        CLog::Error(THISMODULE, "process_dblayer_msg failed. Unknown business message from db layer !");
        return false;
      } 
  }
  return true;
}

bool xGateBusinessManager::process_scheduler_msg(Document &doc)
{
  if(!m_businessInfo) {
    CLog::Error(THISMODULE, "process_scheduler_msg failed. invalid BusinessInfo object for uid: %s !", m_uid.c_str());
    return false;
  }

  const Value inputVal;
  if(!getInputDoc(doc, inputVal)) {
    CLog::Error(THISMODULE, "process_scheduler_msg failed. not able to get 'output' object from main json object !");
    return false;
  }

  switch(m_opcode) {
    case EN_SCHEDULER_EVENT_TIMEOUT: //this event triggered from scheduler thread
      {
        CLog::Detail(THISMODULE, "process_scheduler_msg TIMEOUT_EVENT for uid: %s ", m_uid.c_str());
        if(!m_businessInfo->handle_timeout_event(inputVal)) {
          CLog::Error(THISMODULE, "process_scheduler_msg failed for uid: %s !", m_uid.c_str());
          return false; 
        }
        break;
      }
    case EN_SCHEDULER_EVENT_CANCEL: //this event triggered from signalling thread
      {
        CLog::Detail(THISMODULE, "process_scheduler_msg CANCEL_EVENT for uid: %s ", m_uid.c_str());
        if(!m_businessInfo->handle_cancel_event(inputVal)) {
          CLog::Error(THISMODULE, "process_scheduler_msg failed for uid: %s !", m_uid.c_str());
          return false; 
        }
        break;
      }
    case EN_SCHEDULER_EVENT_UNKNOWN:
    default:
      {
        CLog::Error(THISMODULE, "process_dblayer_msg failed. Unknown business message from db layer !");
        return false;
      } 
  }

  return true;
}

bool xGateBusinessManager::getOutputDoc(Document &doc, const Value &outputVal)
{
  if(doc.HasMember("output")) {
    outputVal = doc["output"].GetObject();
    return true;
  }
  return false;
}

/*
* hash map functionalities - start
*/
xGateRetVal xGateBusinessManager::find_business_info_entry()
{
  xGateRetVal retVal = EN_XGATE_STATUS_ERROR;
  m_businessInfo = NULL;
  if(m_businessInfoMap.find(m_uid.c_str(), m_businessInfo) != -1) {
    CLog::Detail(THISMODULE, "find_business_info_entry success for uid: %s", m_uid.c_str());
    retVal = EN_XGATE_STATUS_SUCCESS;
  } else {
    CLog::Error(THISMODULE, "find_business_info_entry not available for uid: %s", m_uid.c_str());
  }
  return retVal;
}

xGateRetVal xGateBusinessManager::add_business_info_entry()
{
  xGateRetVal retVal = EN_XGATE_STATUS_ERROR;
  int status = -1;

  status = m_businessInfoMap.bind(m_uid.c_str(), m_businessInfo);
  if(status == 0) {
    CLog::Detail(THISMODULE, "add_business_info_entry success for uid: %s", m_uid.c_str());
    retVal = EN_XGATE_STATUS_SUCCESS;
  } else if(status == 1) {
    CLog::Detail(THISMODULE, "add_business_info_entry attempt is made to bind an existing entry for uid: %s", m_uid.c_str());
  } else if(status == -1) {
    CLog::Error(THISMODULE, "add_business_info_entry failed for uid: %s", m_uid.c_str());
  }
  return retVal;
}

xGateRetVal xGateBusinessManager::erase_business_info_entry()
{
  CLog::Detail(THISMODULE, "erase_business_info_entry from map for uid: %s", m_uid.c_str());
  m_businessInfoMap.unbind(m_uid.c_str());
}
/*
* hash map functionalities - end
*/
