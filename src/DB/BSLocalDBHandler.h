#ifndef BS_DB_HANDLER_H
#define BS_DB_HANDLER_H

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

#include "CAUtilityInternal.h"
#include "CAlog.h"
#include "CAExceplog.h"

class BSDBReportMsg : public CA::CAMessageBlock
{
   public:
      BSDBReportMsg()=default;
      virtual ~BSDBReportMsg()=default;

      SET(EN_BS_REPORT_TYPE, en, ReportType);
      GET(en, ReportType);
      SET(STR, str, TelcoCode);
      GET(str, TelcoCode);
      SET(STR, str, Cli);
      GET(str, Cli); 
      SET(STR,str,Ddi);
      GET(str,Ddi);
      SET(STR,str,TrunkName);
      GET(str,TrunkName);
      SET(STR,str,SiteCode);
      GET(str,SiteCode);
      SET(STR,str,SwitchCode);
      GET(str,SwitchCode);
      SET(STR,str,CustCode);
      GET(str,CustCode);
      SET(STR,str,UserDialedNum);
      GET(str,UserDialedNum);
      SET(STR,str,PrefixDest);
      GET(str,PrefixDest);
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
      SET(STR,str,CostPrice);
      GET(str,CostPrice);	
      SET(float,f,HoldTime);
      GET(f,HoldTime);	
      SET(int,i,CallCause);
      GET(i,CallCause);
      SET(STR, str, ReportFilePath);
      GET(str, ReportFilePath);
      SET(double, d, Balance);
      GET(d, Balance);
	   SET(int, i, Batchcode);
      GET(i, Batchcode);
      SET(int, i, Serialcode);
      GET(i, Serialcode);	   
      SET(int, i, Ispackcnxcharge);
      GET(i, Ispackcnxcharge);
	   SET(int, i, UselimitDelay);
      GET(i, UselimitDelay);
	   SET(double, d, PricePerUnit);
      GET(d, PricePerUnit);
	   SET(int, i, CheckDest);
      GET(i, CheckDest);	   
	   SET(int, i, BreakageChargeMode);
      GET(i, BreakageChargeMode);
	   SET(STR, str, AccLangset);
      GET(str, AccLangset);
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
      SET(int, i, Choice);
      GET(i, Choice);
	   SET(STR, str, CallDomain);
      GET(str, CallDomain);
	   SET(STR, str, CrrCause);
      GET(str, CrrCause);
      SET(STR, str, Msisdn);
      GET(str, Msisdn);
      SET(STR, str, IMSI);
      GET(str, IMSI);
      SET(STR, str, Msrn);
      GET(str, Msrn);
      SET(int, i, AppDur);
      GET(i, AppDur);
      SET(int, i, FwdType);
      GET(i, FwdType);
      SET(STR, str, FwDdi);
      GET(str, FwDdi);
      SET(STR, str, Vlr);
      GET(str, Vlr);
      SET(STR, str, Msc);
      GET(str, Msc);

   private:
      EN_BS_REPORT_TYPE m_enReportType;
      STR m_strTelcoCode;
      STR m_strCli;
      STR m_strDdi;
      STR m_strUserDialedNum;
      STR m_strTrunkName;
      STR m_strSiteCode;
      STR m_strSwitchCode;
      STR m_strCustCode;
	   STR m_strPrefixDest;
	   int m_iCardId;
	   STR m_strSessionId;
      int m_iConnectionState;
	   int m_iDisconnectReqBy;
	   STR m_strCostPrice;
	   float m_fHoldTime;
      int m_iCallCause;
      STR m_strFileName;
      STR m_strReportFilePath;
      STR m_strFileFormat; 
      double m_dBalance;
		int m_iCallMaxTalkTime;
		int m_iBatchcode;
		int m_iSerialcode;
		int m_iIspackcnxcharge;
		int m_iUselimitDelay;
		double m_dPricePerUnit;
		int m_iCheckDest;
		int m_iBreakageChargeMode;
		STR m_strAccLangset;
      STR m_strUserInfo;
	   STR m_strTimeCls;
	   STR m_strCrOpr;
	   STR m_strRouteCls;
	   STR m_strCurrCode;
      int m_iChoice;
	   STR m_strCallDomain;
	   STR m_strCrrCause;
      STR m_strMsisdn;
	   STR m_strIMSI;
	   STR m_strMsrn;
	   int m_iAppDur;
	   int m_iFwdType;
	   STR m_strFwDdi;
	   STR m_strVlr;
	   STR m_strMsc;

   public:
      STR TmStrFormat(const char *format);
};
class BSDBMsg : public CA::CAMessageBlock
{
   public:
      BSDBMsg()=default;
      virtual ~BSDBMsg()=default;

      SET(EN_BS_DB_EVENT, en, DBEvent);
      GET(en, DBEvent);
      SET(STR, str, CallId);
      GETSTR(str, CallId);
      SET(STR, str, Uid);
      GETSTR(str, Uid);
      SET(int, i, AddrFamily);
      GET(i, AddrFamily);
      SET(STR, str, MgIpAddr);
      GETSTR(str, MgIpAddr);
      SET(int, i, MgPort);
      GET(i, MgPort);
      SET(int, i, Codec);
      GET(i, Codec);
      SET(int, i, CallStream);
      GET(i, CallStream);
      SET(int, i, CallState);
      GET(i, CallState);
      SET(STR, str, CnCalled);
      GETSTR(str, CnCalled);
      SET(STR, str, CnCalling);
      GETSTR(str, CnCalling);
      SET(STR, str, CsaCalled);
      GETSTR(str, CsaCalled);
      SET(STR, str, CsaCalling);
      GETSTR(str, CsaCalling);
      SET(int, i, Cause);
      GET(i, Cause);
      SET(int, i, CallStartTime);
      GET(i, CallStartTime);
      SET(int, i, CallConnectedTime);
      GET(i, CallConnectedTime);
      SET(int, i, CallDisconnectedTime);
      GET(i, CallDisconnectedTime);
      SET(int, i, CallDuration);
      GET(i, CallDuration);
      SET(int, i, MediaState);
      GET(i, MediaState);
      SET(int, i, P2PMedia);
      GET(i, P2PMedia);
      SET(STR, str, group);
      GETSTR(str, group);
      SET(STR, str, fwdreason);
      GETSTR(str, fwdreason);
      SET(STR, str, routeAddr);
      GETSTR(str, routeAddr);
      SET(STR, str, proxyUserName);
      GETSTR(str, proxyUserName);
      SET(STR, str, ivrfile);
      GETSTR(str, ivrfile);
      SET(STR, str, CalledNumberPlan);
      GETSTR(str, CalledNumberPlan);
      SET(STR, str, CalledNumberType);
      GETSTR(str, CalledNumberType);
      SET(STR, str, UserInfo);
      GETSTR(str, UserInfo);
      SET(STR, str, CallingNumberPlan);
      GETSTR(str, CallingNumberPlan);
      SET(STR, str, CallingNumberType);
      GETSTR(str, CallingNumberType);
      SET(STR, str, PrivateInfo);
      GETSTR(str, PrivateInfo);
      SET(STR, str, RedirectingNumber);
      GETSTR(str, RedirectingNumber);
      SET(STR, str, cui);
      GETSTR(str, cui);
      SET(STR, str, tableName);
      GETSTR(str, tableName);
      SET(int, i, play_state);
      GET(i, play_state);
      SET(int, i, Legid);
      GET(i, Legid);
      SET(int, i, gatewayId);
      GET(i, gatewayId);
      SET(int, i, csCallingCodec);
      GET(i, csCallingCodec);
      SET(int, i, csCalledCodec);
      GET(i, csCalledCodec);
      SET(int, i, mgRequestTime);
      GET(i, mgRequestTime);
      SET(int, i, mgResponceTime);
      GET(i, mgResponceTime);
      SET(int, i, businessServerRequestTime);
      GET(i, businessServerRequestTime);
      SET(int, i, businessServerResponceTime);
      GET(i, businessServerResponceTime);
      SET(int, i, reportServerRequestTime);
      GET(i, reportServerRequestTime);
      SET(int, i, reportServerResponceTime);
      GET(i, reportServerResponceTime);


   private:
      EN_BS_DB_EVENT m_enDBEvent;
      STR m_strCallId;
      STR m_strUid;
      int m_iAddrFamily;
      STR m_strMgIpAddr;
      int m_iMgPort;
      int m_iCodec;
      int m_iCallStream;
      int m_iCallState;
      STR m_strCnCalled;
      STR m_strCnCalling;
      STR m_strCsaCalled;
      STR m_strCsaCalling;
      int m_iCause;
      int m_iCallStartTime;
      int m_iCallConnectedTime;
      int m_iCallDisconnectedTime;
      int m_iCallDuration;
      int m_iMediaState;
      int m_iP2PMedia;
      STR m_strgroup;
      STR m_strfwdreason; // for diversion header for reason
      STR m_strrouteAddr;
      STR m_strproxyUserName;
      STR m_strivrfile;
      STR m_strCalledNumberPlan;
      STR m_strCalledNumberType;
      STR m_strUserInfo;
      STR m_strCallingNumberPlan;
      STR m_strCallingNumberType;
      STR m_strPrivateInfo;
      STR m_strtableName;
      STR m_strRedirectingNumber;
      STR m_strOriginalCalledNumber;
      STR m_strOriginalCalledNumberPlan;
      STR m_strOriginalCalledNumberType;
      STR m_strLocationNumber;
      STR m_strLocationNumberPlan;
      STR m_strLocationNumberType;
      STR m_strLocationNumberPresentation;
      STR m_strcnRedirectingNumber;
      STR m_strcui; // call user info
      int m_iplay_state; // For play state responce
      int m_iLegid;
      int m_igatewayId;
      int m_icsCallingCodec;
      int m_icsCalledCodec;
      int m_imgRequestTime;
      int m_imgResponceTime;
      int m_ibusinessServerRequestTime;
      int m_ibusinessServerResponceTime;
      int m_ireportServerRequestTime;
      int m_ireportServerResponceTime;      
      
};

class BSLocalDBHandler
{
   public:
      BSLocalDBHandler()=default;
      virtual ~BSLocalDBHandler()=default;

      bool connectMySqlDB(const char* connStr, const char* user, const char* pass);
      bool createReportSchema();
      bool clearConnection();
      bool insertCallInfo(BSDBMsg* dbObj);
      bool getCallInfo(BSDBMsg* dbObj);
      bool updateCallInfo(BSDBMsg* dbObj);
      bool deleteCallInfo(BSDBMsg* dbObj);
      //bool dropTable(STR tableName);
      bool dropTable(BSDBMsg* dbObj);
      bool insertCDRInfo(BSDBReportMsg* dbObj);
      bool insertCRRInfo(BSDBReportMsg* dbObj);
      bool insertCDSInfo(BSDBReportMsg* dbObj);
      bool insertDATInfo(BSDBReportMsg* dbObj);
      STR getDate();

   private:
      sql::Driver *m_driver;
      sql::Connection *m_conn;
      sql::Statement *m_stmt;
      sql::ResultSet *m_res;
};
#endif
