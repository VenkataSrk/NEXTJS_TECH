#ifndef CA_DAT_H
#define CA_DAT_H

#include "BSBaseReport.h"

class BSDAT : public BSBaseReport{

     public:
	   BSDAT()=default;
	   ~BSDAT()=default;

	 public:
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
       SET(int, i, LegAConnect);
       GET(i, LegAConnect);
       SET(int, i, LegBConnect);
       GET(i, LegBConnect);
       SET(STR, str, FwDdi);
       GET(str, FwDdi);
       SET(STR,str,LegAdev);
       GET(str,LegAdev);
       SET(STR,str,LegBdev);
       GET(str,LegBdev);
       SET(STR, str, Vlr);
       GET(str, Vlr);
       SET(STR, str, Msc);
       GET(str, Msc);
       SET(STR, str, Trace);
       GET(str, Trace);
	 
	 private:
     STR m_strMsisdn;
	   STR m_strIMSI;
	   STR m_strMsrn;
	   int m_iAppDur;
	   int m_iFwdType;
	   STR m_strFwDdi;
	   STR m_strVlr;
	   STR m_strMsc;
	   int m_iLegAConnect;
	   int m_iLegBConnect;
	   STR m_strLegAdev;
	   STR m_strLegBdev;
	   STR m_strTrace;

   public:
	   bool writeDATToFile();
	   STR WriteToDat();
};
#endif
