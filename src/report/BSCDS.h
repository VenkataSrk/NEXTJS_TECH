#ifndef BS_CDS_H
#define BS_CDS_H

#include "BSBaseReport.h"

class BSCDS : public BSBaseReport{

	 public:
	   BSCDS()=default;
	   ~BSCDS()=default;

     public:
       SET(STR, str, UserInfo);
       GET(str, UserInfo);
	   SET(STR, str, TimeCls);
       GET(str, TimeCls);
       SET(STR, str, CrOpr);
       GET(str, CrOpr);    
       SET(STR, str, RouteCls);
       GET(str, RouteCls);     
       SET(STR, str, CurrCode);
       GET(str, CurrCode);
       SET(STR, str, Trace);
       GET(str, Trace);

	 private:
	   STR m_strUserInfo;
	   STR m_strTimeCls;
	   STR m_strCrOpr;
	   STR m_strRouteCls;
	   STR m_strCurrCode;
           STR m_strTrace;
     public:
	   bool writeCDSToFile();
	   STR WriteToCds();
      
};
#endif
