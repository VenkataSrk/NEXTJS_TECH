#include "BSLocalDBService.h"
#include "../BusinessServer.h"

/* Method Name: init
 ** Description: To spawn event thread
 ** Parameters: STR type, STR connString, STR user, STR pass
 ** return: boolean(true|false)
 **/
bool BSLocalDBService::init(STR type, STR connString, STR user, STR pass)
{
   if(!type.empty() && !connString.empty() && !user.empty() && !pass.empty())
   {
      m_ptBSLocalDBHandler = new BSLocalDBHandler();
      if(m_ptBSLocalDBHandler)
      {
         if(!strncmp(type.c_str(), "MYSQL", type.length()))
         {
            if(m_ptBSLocalDBHandler->connectMySqlDB(connString.c_str(), user.c_str(), pass.c_str()))
            {
               m_bConnStatus = true;
	       m_ptBSLocalDBHandler->createReportSchema();
            }
         }
      }
   }
   else
   {
      m_bConnStatus = false;
   }
   return m_bConnStatus;
}

/* Method Name: getConfigData
 ** Description: To send config request
 ** Parameters: nil
 ** return: boolean(true|false)
 **/
/*bool BSLocalDBService::getDBConfigData()
{
   std::unique_ptr<DBConfig> pConfigMsg(new DBConfig());
   if(pConfigMsg)
   {
      pConfigMsg->setMsgType(MSG_DB2CONF);
      Server::post(EN_BS_THREAD_NAME::CON, std::move(pConfigMsg));
      return true;
   }
   return false;
}*/

/* Method Name: run
 ** Description: run event loop
 ** Parameters: nil
 ** return: boolean(true|false)
 **/
bool BSLocalDBService::run()
{
   CA::CAMessageBlock* pMsg = nullptr;
   const std::string thread_name = "BSLocalDBSer";
   int rc = pthread_setname_np(pthread_self(), thread_name.c_str());
   if (rc != 0) {
	   std::cerr << "Failed to set name for thread: " << strerror(rc) << std::endl;
   } else {
	   std::cout << "Thread name set successfully to: " << thread_name << std::endl;
   }
   while(getThreadStatus())
   {
      pMsg = (CA::CAMessageBlock*)RecvMessage();
      if(pMsg)
      {
	 handleMsg(pMsg);
      }
      else
      {
	std::this_thread::sleep_for(std::chrono::milliseconds(10));  // Sleep to avoid busy-waiting
      }
   }
   m_cWaitEvent.SignalAll();
   if(pMsg)
   {
      delete pMsg;
   }
   return true;
}

/* Method Name: handleMsg
 ** Description: To handle message received and process it
 ** Parameters: CAMessageBlock* pMsg
 ** return: boolean(true|false)
 **/
bool BSLocalDBService::handleMsg(CA::CAMessageBlock* pMsg)
{
   unsigned long msgType = pMsg->getMsgType();
   switch(msgType)
   {
      case MESSAGEEVENT::MSG_INIT:
      {
         break;
      }
      case MESSAGEEVENT::MSG_REP2DB:
      {
         BSDBReportMsg* dbObj = static_cast<BSDBReportMsg*>(pMsg);
         if(!m_bConnStatus)
         {
            break;
         }
         if((dbObj)&&(m_ptBSLocalDBHandler))
         {
            handle_report_event(dbObj);
         }
         break;
      }
      default:
      {
         break;
      }
   }
   return true;
}

/* Method Name: handle_report_event
 ** Description: post exception info to exception module
 ** Parameters: BSDBReportMsg
 ** return: boolean(true|false)
 **/
bool BSLocalDBService::handle_report_event(BSDBReportMsg *dbMsg)
{
   EN_BS_REPORT_TYPE reportType = dbMsg->getReportType();
   switch(reportType)
   {
      case EN_BS_REPORT_TYPE::EN_BS_REPORT_TYPE_CDR:
      {
         m_ptBSLocalDBHandler->insertCDRInfo(dbMsg);
         break;
      }
      case EN_BS_REPORT_TYPE::EN_BS_REPORT_TYPE_CRR:
      {
         m_ptBSLocalDBHandler->insertCRRInfo(dbMsg);
         break;
      }
      case EN_BS_REPORT_TYPE::EN_BS_REPORT_TYPE_CDS:
      {
         m_ptBSLocalDBHandler->insertCDSInfo(dbMsg);
         break;
      }
      case EN_BS_REPORT_TYPE::EN_BS_REPORT_TYPE_DAT:
      {
         m_ptBSLocalDBHandler->insertDATInfo(dbMsg);
         break;
      }
      default:
      {
         break;
      }
   }
   return true;
}

/* Method Name: processException
 ** Description: post exception info to exception module
 ** Parameters: CA::CAException &ex
 ** return: boolean(true|false)
 **/
/*bool BSLocalDBService::processException(CA::CAException &ex)
{
   CAlog_INFO("CADBController::processException called");
   std::unique_ptr<CA::CAExceptionMsg> pMsg (new CA::CAExceptionMsg());
   pMsg->setExDate(ex.getExDate());
   pMsg->setExTime(ex.getExTime());
   pMsg->setExFile(ex.getExFile());
   pMsg->setExFunction(ex.getExFunction());
   pMsg->setExLine(ex.getExLine());
   pMsg->setExMsg(ex.what());
   pMsg->setExUid(ex.getExUid());
   pMsg->setExSrcModule(CA::EN_CA_EXCEPTION_MODULE_ID::EN_CA_LOCAL_DB);
   pMsg->setExType(ex.getExType());
   pMsg->setMsgType(MESSAGEEVENT::MSG_DB2EXP);
   pMsg->setCallObj(ex.getCallObj());
   Server::post(EN_CA_THREAD_NAME::EXP, std::move(pMsg));
   return true;
}*/
