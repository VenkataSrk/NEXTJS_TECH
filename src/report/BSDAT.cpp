#include "BSDAT.h"

STR BSDAT::WriteToDat()
{
  STR datdata;
  std::ostringstream oss;
  oss << TmStrFormat("%Y-%m-%d %H:%M:%S") << "," \
      << getSessionId() << "," \
      << getSiteCode() << "," \
      << getSwitchCode() << "," \
      << getTrunkName() << ","\
      << getLegAdev() << "," \
      << getCli() << "," \
      << getDdi() << "," \
      << getMsisdn() << "," \
      << getIMSI() << "," \
      << getMsrn() << "," \
      << getAppDur() << "," \
      << getLegAConnect() << "," \
      << getDdi() << "," \
      << getTrunkOut() << "," \
      << getLegBdev() << "," \
      << getLegBConnect() << "," \
      << getDisconnectReqBy() << "," \
      << getCallCause() << "," \
      << getFwdType() << "," \
      << getFwDdi() << "," \
      << "" << "," \
      << "" << "," \
      << "" << "," \
      << getConnectionState() << "," \
      << (getTalkTime()*1000) << ","  
      << "0" << "," \
      << "" << "," \
      << getVlr() << "," \
      << getMsc() 
      << "\r\n";
      datdata = oss.str();
      //mTrace(getTrace(), datdata.c_str());
    return datdata;

}


bool BSDAT::writeDATToFile()
{
  STR data("");
  data = WriteToDat();
  return WriteToFile(data);
}
