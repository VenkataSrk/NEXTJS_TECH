//local includes
#include "log.h"
#include "klog.h"
#include "xGateBusinessProcessor.h"
#include "xGateBusinessManager.h"

#define THISMODULE "BusProc" //Business Manager Processor

xGateBusinessProcessor::xGateBusinessProcessor(const char *processorType) : 
  xGateProcessor(processorType)
{
  CLog::Warning(THISMODULE, "xGateBusinessProcessor::xGateBusinessProcessor");
}

xGateBusinessProcessor::~xGateBusinessProcessor(void)
{
  CLog::Warning(THISMODULE, "xGateBusinessProcessor::~xGateBusinessProcessor");
}

xGateRetVal xGateBusinessProcessor::process_msg(xGateBaseMsg* pMsg)
{
  CLog::Detail(THISMODULE, "Enter xGateBusinessProcessor::process_msg"); 
  xGateBusinessManager *mgrInstance = xGateBusinessManager::getInstance(); 
  if(!mgrInstance) {
    CLog::Error(THISMODULE, "process_msg failed. Not able to get Business Manager Instance !");
    return EN_XGATE_STATUS_SUCCESS;
  }

  switch(pMsg->getMsgType()) {
    case EN_XGATE_MSG_TO_BUSINESS: //we got msg
      {
        CLog::Detail(THISMODULE, "processing message EN_XGATE_MSG_TO_BUSINESS");
        xGateBusinessMsg *pBusinessMsg = dynamic_cast<xGateBusinessMsg*>(pMsg);
        if(!mgrInstance->process_msg(pBusinessMsg)) {
          CLog::Error(THISMODULE, "process_msg failed. Business Manager failed to process message !");
        }
        break;
      }
    case EN_XGATE_MSG_UNKNOWN:
    default:
      {
        CLog::Error(THISMODULE, "process_msg is EN_XGATE_MSG_UNKNOWN");
        break;
      }
  }
  return EN_XGATE_STATUS_SUCCESS;
}

