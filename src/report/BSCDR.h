#ifndef BS_CDR_H
#define BS_CDR_H

#include "BSBaseReport.h"


class BSCDR : public BSBaseReport
{

     public:
	   BSCDR()=default;
	   ~BSCDR()=default;
	 //const char* what() const throw();
     
	 public:
	   SET(int, i, CdrBalance);
       GET(i, CdrBalance);
	   SET(int, i, Batchcode);
       GET(i, Batchcode);
       SET(int, i, AccessCharge);
       GET(i, AccessCharge);
       SET(int, i, Serialcode);
       GET(i, Serialcode);	   
       SET(int, i, Ispackcnxcharge);
       GET(i, Ispackcnxcharge);
       SET(int, i, DiffCharge);
       GET(i, DiffCharge);
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
	   SET(STR, str, TelcoCode);
       GET(str, TelcoCode);
	   SET(STR, str, TariffClass);
       GET(str, TariffClass); 
       SET(int, i, CnxDelay);
       GET(i, CnxDelay);
	   SET(int, i, CnxUnit);
       GET(i, CnxUnit);
	   SET(int, i, SampleDelay);
       GET(i, SampleDelay);
	   SET(int, i, SampleUnit);
       GET(i, SampleUnit);
       SET(int, i, CdrTotalCons);
       GET(i, CdrTotalCons);
	   SET(int, i, PremiumDest);
       GET(i, PremiumDest);
       SET(int, i, TimePeriod);
       GET(i, TimePeriod);
       SET(STR, str, AccessType);
       GET(str, AccessType);
       SET(STR, str, FreeFlag);
       GET(str, FreeFlag);
       SET(STR, str, Trace);
       GET(str, Trace);
       SET(STR, str, AccountType);
       GET(str, AccountType);
       SET(STR, str, MaxAccess);
       GET(str,MaxAccess);
       SET(STR, str, CnxFactor);
       GET(str, CnxFactor);
       SET(STR, str, CliCategory);
       GET(str,CliCategory);
       SET(STR, str, MvnoCallmode);
       GET(str,MvnoCallmode);
       SET(STR, str, FreeCnxDelay);
       GET(str,FreeCnxDelay);
       SET(STR, str, FreeCnxCharge);
       GET(str,FreeCnxCharge);
       SET(STR, str, ActId);
       GET(str,ActId);
       SET(STR, str, Netratio);
       GET(str, Netratio);
       SET(STR, str, TaskId);
       GET(str, TaskId);

       public:
	    STR m_strTelcoCode;
		int m_iCdrBalance;
		int m_iCallMaxTalkTime;
		int m_iBatchcode;
		STR m_strTaskId;
		STR m_strFreeCnxDelay;
		STR m_strFreeCnxCharge;
		STR m_strActId;
		STR m_strNetratio;
		int m_iAccessCharge;
		int m_iSerialcode;
		int m_iIspackcnxcharge;
		int m_iUselimitDelay;
		double m_dPricePerUnit;
		int m_iCheckDest;
		int m_iBreakageChargeMode;
		STR m_strAccLangset;
		STR m_strTariffClass;
	    int m_iCnxDelay;
	        STR m_strCnxFactor;
		STR m_strCliCategory;
		STR m_strMvnoCallmode;
		int m_iCnxUnit;
		int m_iSampleDelay;
		int m_iSampleUnit;
		int m_iCdrTotalCons;
		int m_iPremiumDest;
		int m_iTimePeriod;
		STR m_strAccessType;
		STR m_strFreeFlag;
		STR m_strTrace;
		int m_iDiffCharge;
		STR m_strMaxAccess;
		STR m_strAccountType;

	 public:
	 	bool writeCDRToFile();
	 	STR WriteToCdr();

};
#endif
