#ifndef BS_REPORT_H
#define BS_REPORT_H

#include "CAUtilityInternal.h"
#include "BSBaseReport.h"

class BSReportingService : public CA::CAThread{

     public:
	   BSReportingService()=default;
	   virtual ~BSReportingService()=default;
       bool init(STR path);
	   virtual bool run();
	   bool handleMsg(CA::CAMessageBlock* pMsg);
	   BSReportingService * getInstance();
	   bool handleChargingReport(CA::CAMessageBlock* pMsg);
	   bool handleRoutingReport(CA::CAMessageBlock* pMsg);
	   bool handleSwitchingReport(CA::CAMessageBlock* pMsg);
	   bool handleGmscReport(CA::CAMessageBlock* pMsg);
       bool postReportToDB(CA::CAMessageBlock* pMsg);

	private:
       BSReportingService *m_ReportInstance;
	   STR m_strReportPath;
};

#endif
	 
