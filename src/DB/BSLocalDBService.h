#ifndef BS_DB_CONTROLLER_H
#define BS_DB_CONTROLLER_H

#include "BSLocalDBHandler.h"
//#include "BSException.h"

class BSLocalDBService : public CA::CAThread
{
   public:
      BSLocalDBService()=default;
      virtual ~BSLocalDBService()=default;

      bool init(STR type, STR connString, STR user, STR pass);
      virtual bool run();

      bool handleMsg(CA::CAMessageBlock* pMsg);
    //  bool processException(CA::CAException &ex);
      bool getDBConfigData();
      bool handle_report_event(BSDBReportMsg *dbMsg);

   private:
      BSLocalDBHandler* m_ptBSLocalDBHandler;
      bool m_bConnStatus;
};
#endif
