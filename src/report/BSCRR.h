#ifndef CA_CRR_H
#define CA_CRR_H

#include "BSBaseReport.h"

class BSCRR : public BSBaseReport{

     public:
	   BSCRR()=default;
	   ~BSCRR()=default;

     public:
       SET(int, i, Choice);
       GET(i, Choice);
       SET(STR, str, CallDomain);
       GET(str, CallDomain);
       SET(STR, str, CrrCause);
       GET(str, CrrCause);
       SET(STR, str, Trace);
       GET(str, Trace);	 
       SET(STR, str, TariffCls);
       GET(str, TariffCls);
       SET(STR, str, Tariff);
       GET(str, Tariff);
       SET(STR, str, Access);
       GET(str, Access);
       SET(STR, str, RlsIp);
       GET(str, RlsIp);

     private:
	 int m_iChoice;
	 STR m_strCallDomain;
	 STR m_strCrrCause;
	 STR m_strTrace;
	 STR m_strTariffCls;
	 STR m_strTariff;
	 STR m_strAccess;
	 STR m_strRlsIp;

    public:
	bool writeCRRToFile();
	STR WriteToCrr();

      
};
#endif
