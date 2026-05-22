#include "ReportingService.h"
#include "../BusinessConfigObject.h"
#include "../BusinessServer.h"
#include "BSCRR.h"
#include "BSCDS.h"
#include "BSDAT.h"

bool BSReportingService::init(STR path)
{
  m_strReportPath = path.empty()?"/var/cdr":path;
 return true;
}

bool BSReportingService::run(){
    const std::string thread_name = "BSReportServ";
    int rc = pthread_setname_np(pthread_self(), thread_name.c_str());
    if (rc != 0) {
            std::cerr << "Failed to set name for thread: " << strerror(rc) << std::endl;
    } else {
            std::cout << "Thread name set successfully to: " << thread_name << std::endl;
    }
     while(getThreadStatus()){
        CA::CAMessageBlock * pMsg = (CA::CAMessageBlock *)RecvMessage();
        if(pMsg){
            handleMsg(pMsg);
            if(pMsg){ delete pMsg; pMsg = nullptr;}
        }
	else
	{
        std::this_thread::sleep_for(std::chrono::milliseconds(10));  // Sleep to avoid busy-waiting
	}
        //usleep(10);
     }
     return true;
}

bool BSReportingService::handleMsg(CA::CAMessageBlock *pMsg)
{    
  if(!pMsg)
  {
    return false;
  }
  unsigned long msgType = pMsg->getMsgType();
  switch(msgType)
  {
    case MESSAGEEVENT::MSG_INIT:
    {
      std::unique_ptr<CA::CAMessageBlock> confMsg(new CA::CAMessageBlock());
      confMsg->setMsgType(MESSAGEEVENT::MSG_REP2CONF);
      BusinessServer::post(BUSINESS_THREADS::MVNO_CONF,std::move(confMsg));
      break;
    }
    case MESSAGEEVENT::MSG_SHUTDOWN:
    {
      break;
    }
    case MESSAGEEVENT::MSG_CHARG2REP:
    {
      handleChargingReport(pMsg);
      break;
    }
    case MESSAGEEVENT::MSG_ROUT2REP:
    {
      handleRoutingReport(pMsg);
      break;
    }
    case MESSAGEEVENT::MSG_SWITCH2REP:
    {
      handleSwitchingReport(pMsg);
      break;
    }
    case MESSAGEEVENT::MSG_GMSC2REP:
    {
      handleGmscReport(pMsg);
      break;
    }
    default:
    {
      break;
    }
  }
return true;
}
      
BSReportingService * BSReportingService::getInstance()
{
  if(m_ReportInstance == NULL) {
    m_ReportInstance = new BSReportingService();
  }
  return m_ReportInstance;
}

bool BSReportingService::handleChargingReport(CA::CAMessageBlock* pMsg){
    if(!pMsg)
        return false;
    BSCDR *pBscdrMsg = static_cast<BSCDR*>(pMsg);
    if(pBscdrMsg)
    {
      pBscdrMsg->setReportFilePath(m_strReportPath);
      pBscdrMsg->writeCDRToFile();
      postReportToDB(pBscdrMsg);
    }
    return true;
}

bool BSReportingService::handleRoutingReport(CA::CAMessageBlock* pMsg){
    if(!pMsg)
        return false;
    BSCRR *pBscrrMsg = static_cast<BSCRR*>(pMsg);
    if(pBscrrMsg)
    {
      pBscrrMsg->setReportFilePath(m_strReportPath);
      pBscrrMsg->writeCRRToFile();
    }
    return true;
}

bool BSReportingService::handleSwitchingReport(CA::CAMessageBlock* pMsg){
    if(!pMsg)
        return false;
    BSCDS *pBscdsMsg = static_cast<BSCDS*>(pMsg);
    if(pBscdsMsg)
    {
      pBscdsMsg->setReportFilePath(m_strReportPath);
      pBscdsMsg->writeCDSToFile();
    }
    return true;
}

bool BSReportingService::handleGmscReport(CA::CAMessageBlock* pMsg){
    if(!pMsg)
        return false;
    BSDAT *pBsdatMsg = static_cast<BSDAT*>(pMsg);
    if(pBsdatMsg)
    {
      pBsdatMsg->setReportFilePath(m_strReportPath);
      pBsdatMsg->writeDATToFile();
    }
    return true;
}


bool BSReportingService::postReportToDB(CA::CAMessageBlock* pMsg)
{
  if(!pMsg)
  {
    return false;
  }
  std::unique_ptr<BSDBReportMsg> pDbMsg(new BSDBReportMsg());
  if(pDbMsg)
  {
    unsigned long msgType = pMsg->getMsgType();
    switch(msgType)
    {
      case MESSAGEEVENT::MSG_CHARG2REP:
      {
        BSCDR *pBscdrMsg = static_cast<BSCDR*>(pMsg);
        pDbMsg->setTelcoCode(pBscdrMsg->getTelcoCode());
        pDbMsg->setCli(pBscdrMsg->getCli());
        pDbMsg->setDdi(pBscdrMsg->getCustCode());
        pDbMsg->setSiteCode(pBscdrMsg->getSiteCode());
        pDbMsg->setSwitchCode(pBscdrMsg->getSwitchCode());
        pDbMsg->setUserDialedNum(pBscdrMsg->getUserDialedNum());
        pDbMsg->setDdi(pBscdrMsg->getDdi());
        pDbMsg->setPrefixDest(pBscdrMsg->getPrefixDest());
        pDbMsg->setAccLangset(pBscdrMsg->getAccLangset());
        pDbMsg->setCardId(pBscdrMsg->getCardId());
        //pDbMsg->setCdrBalance(pBscdrMsg->getCdrBalance());
        pDbMsg->setBatchcode(pBscdrMsg->getBatchcode());
        pDbMsg->setSerialcode(pBscdrMsg->getSerialcode());
        pDbMsg->setIspackcnxcharge(pBscdrMsg->getIspackcnxcharge());
        pDbMsg->setUselimitDelay(pBscdrMsg->getUselimitDelay());
        pDbMsg->setPricePerUnit(pBscdrMsg->getPricePerUnit());
        pDbMsg->setCheckDest(pBscdrMsg->getCheckDest());
        pDbMsg->setBreakageChargeMode(pBscdrMsg->getBreakageChargeMode());
        pDbMsg->setReportType(EN_BS_REPORT_TYPE::EN_BS_REPORT_TYPE_CDR);
        break;
      }
      case MESSAGEEVENT::MSG_ROUT2REP:
      {
        BSCRR *pBscrrMsg = static_cast<BSCRR*>(pMsg);
        pDbMsg->setSiteCode(pBscrrMsg->getSiteCode());
        pDbMsg->setSwitchCode(pBscrrMsg->getSwitchCode());
        pDbMsg->setSessionId(pBscrrMsg->getSessionId());
        pDbMsg->setCli(pBscrrMsg->getCli());
        pDbMsg->setDdi(pBscrrMsg->getDdi());
        pDbMsg->setPrefixDest(pBscrrMsg->getPrefixDest());
        pDbMsg->setChoice(pBscrrMsg->getChoice());
        pDbMsg->setCallDomain(pBscrrMsg->getCallDomain());
        pDbMsg->setCallCause(pBscrrMsg->getCallCause());
        pDbMsg->setConnectionState(pBscrrMsg->getConnectionState());
        pDbMsg->setDisconnectReqBy(pBscrrMsg->getDisconnectReqBy());
        pDbMsg->setCrrCause(pBscrrMsg->getCrrCause());
        //pDbMsg->setCostPrice(pBscrrMsg->getCostPrice());
        pDbMsg->setHoldTime(pBscrrMsg->getHoldTime());
        pDbMsg->setReportType(EN_BS_REPORT_TYPE::EN_BS_REPORT_TYPE_CRR);
        break;
      }
      case MESSAGEEVENT::MSG_SWITCH2REP:
      {
        BSCDS *pBscdsMsg = static_cast<BSCDS*>(pMsg);
        pDbMsg->setSiteCode(pBscdsMsg->getSiteCode());
        pDbMsg->setSwitchCode(pBscdsMsg->getSwitchCode());
        pDbMsg->setSessionId(pBscdsMsg->getSessionId());
        pDbMsg->setCli(pBscdsMsg->getCli());
        pDbMsg->setDdi(pBscdsMsg->getDdi());
        pDbMsg->setHoldTime(pBscdsMsg->getHoldTime());
        pDbMsg->setPrefixDest(pBscdsMsg->getPrefixDest());
        pDbMsg->setUserInfo(pBscdsMsg->getUserInfo());
        pDbMsg->setConnectionState(pBscdsMsg->getConnectionState());
        pDbMsg->setDisconnectReqBy(pBscdsMsg->getDisconnectReqBy());
       // pDbMsg->setCostPrice(pBscdsMsg->getCostPrice());
        pDbMsg->setTimeCls(pBscdsMsg->getTimeCls());
        pDbMsg->setCrOpr(pBscdsMsg->getCrOpr());
        pDbMsg->setRouteCls(pBscdsMsg->getRouteCls());
        pDbMsg->setReportType(EN_BS_REPORT_TYPE::EN_BS_REPORT_TYPE_CDS);
        break;
      }
      case MESSAGEEVENT::MSG_GMSC2REP:
      {
        BSDAT *pBsdatMsg = static_cast<BSDAT*>(pMsg);
        pDbMsg->setSessionId(pBsdatMsg->getSessionId());
        pDbMsg->setSiteCode(pBsdatMsg->getSiteCode());
        pDbMsg->setSwitchCode(pBsdatMsg->getSwitchCode());
        pDbMsg->setCli(pBsdatMsg->getCli());
        pDbMsg->setDdi(pBsdatMsg->getDdi());
        pDbMsg->setMsisdn(pBsdatMsg->getMsisdn());
        pDbMsg->setIMSI(pBsdatMsg->getIMSI());
        pDbMsg->setMsrn(pBsdatMsg->getMsrn());
        pDbMsg->setAppDur(pBsdatMsg->getAppDur());
        pDbMsg->setDisconnectReqBy(pBsdatMsg->getDisconnectReqBy());
        pDbMsg->setCallCause(pBsdatMsg->getCallCause());
        pDbMsg->setFwdType(pBsdatMsg->getFwdType());
        pDbMsg->setFwDdi(pBsdatMsg->getFwDdi());
        pDbMsg->setConnectionState(pBsdatMsg->getConnectionState());
        pDbMsg->setVlr(pBsdatMsg->getVlr());
        pDbMsg->setMsc(pBsdatMsg->getMsc());
        pDbMsg->setReportType(EN_BS_REPORT_TYPE::EN_BS_REPORT_TYPE_CDR);
        break;
      }
      default:
      {
        break;
      }
    }
    if(pDbMsg)
    {
      pDbMsg->setMsgType(MESSAGEEVENT::MSG_REP2DB);
      BusinessServer::post(BUSINESS_THREADS::MVNO_BSLOCDB, std::move(pDbMsg));
    }
  }
  return true;
}



