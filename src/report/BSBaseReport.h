#ifndef __BS_REPORT_H
#define __BS_REPORT_H

#include "CAUtilityInternal.h"

//#include "../BusinessServer.h"
//#include "../BusinessConfigObject.h" 


class BSBaseReport : public CAFileHandle, public CA::CAMessageBlock
{
    public:
    BSBaseReport()=default;
    ~BSBaseReport()=default;

      SET(STR, str, Cli);
      GET(str, Cli); 
      SET(STR, str, UserDialedNum);
      GET(str, UserDialedNum);
      SET(STR,str,Ddi);
      GET(str,Ddi);
      SET(STR,str,DestCode);
      GET(str,DestCode);
      SET(STR,str,TrunkName);
      GET(str,TrunkName);
      SET(STR,str,SiteCode);
      GET(str,SiteCode);
      SET(STR,str,SwitchCode);
      GET(str,SwitchCode);
      SET(STR,str,CustCode);
      GET(str,CustCode);
      SET(int,i,TalkTime);
      GET(i,TalkTime);
      SET(STR,str,PrefixDest);
      GET(str,PrefixDest);
      SET(STR,str,PrefixCode);
      GET(str,PrefixCode);
      SET(int,i,CardId);
      GET(i,CardId);	
      SET(STR,str,FileFormat);
      GET(str,FileFormat);
      SET(STR,str,SessionId);
      GET(str,SessionId);
      SET(int,i,ConnectionState);
      GET(i,ConnectionState);    
      SET(int,i,DisconnectReqBy);
      GET(i,DisconnectReqBy);   
      SET(double,d,CostPrice);
      GET(d,CostPrice);	
      SET(float,f,HoldTime);
      GET(f,HoldTime);	
      SET(int,i,CallCause);
      GET(i,CallCause);
      SET(STR, str, CurrCode);
      GET(str, CurrCode);
      SET(STR, str, ReportFilePath);
      GET(str, ReportFilePath);
      SET(STR, str, TrunkOut);
      GET(str, TrunkOut);
      SET(STR, str, LegBdev);
      GET(str, LegBdev);
      SET(STR, str, LegAdev);
      GET(str, LegAdev);
      SET(STR, str, ProvideStr);
      GET(str, ProvideStr);

    private:
      STR m_strCli;
      STR m_strDdi;
      STR m_strTrunkName;
      STR m_strSiteCode;
      STR m_strSwitchCode;
     STR m_strCustCode;
	  STR m_strPrefixDest;
	  STR m_strPrefixCode;
	  int m_iCardId;
	  STR m_strSessionId;
     int m_iConnectionState;
	  int m_iDisconnectReqBy;
	  double m_dCostPrice;
	  float m_fHoldTime;
     int m_iCallCause;
     int m_iTalkTime;
     STR m_strFileName;
     STR m_strReportFilePath;
     STR m_strFileFormat;
     STR m_strUserDialedNum;
     STR m_strDestCode;
     STR m_strCurrCode;
     STR m_strTrunkOut;
     STR m_strLegBdev;
     STR m_strLegAdev;
     STR m_strProvideStr;
   
     public:
     STR TmStrFormat(const char *);
     bool WriteToFile(std::string);
     STR createFilename();
     
     private:
     CAFileHandle *m_fileHandler;
	    
};

#endif
