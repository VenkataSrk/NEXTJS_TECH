#include "BSCDR.h"

STR BSCDR::WriteToCdr()
{
 STR cdrline;

 std::ostringstream oss;
 oss << TmStrFormat("%Y-%m-%d %H:%M:%S") << "," << TmStrFormat("%Y-%m-%d %H:%M:%S") << "," \
     << getTelcoCode() << "," << getCustCode() << "," << getSiteCode() << "," << getSwitchCode() << "," \
     << getDdi() << "," << getCli() << "," << getUserDialedNum() << "," << getDestCode() << "," << getAccLangset() << ","\
     << getCardId() << "," << getTariffClass() << "," << getTrunkName() << "," << getTrunkOut() << "," << getProvideStr() << "," \
     << getCdrBalance() << "," << getTalkTime() << "," << getBatchcode() << "," << getSerialcode() << "," << "3" << "," \
     << getAccessCharge() << "," <<getTimePeriod() << "," << getDiffCharge() << "," << getIspackcnxcharge() << ","  \
     << getCnxDelay() << "," << getCnxUnit() << "," << getSampleDelay() << "," << getSampleUnit() << "," << getCdrTotalCons()<< "," \
     << "0.0" << "," << getAccountType() << "," << getMaxAccess() << ","  
     << TmStrFormat("%Y-%m-%d %H:%M:%S") << "," << "0" << "," << TmStrFormat("%Y-%m-%d %H:%M:%S") << "," \
     << "0" << "," << getCheckDest() << "," << getFreeCnxDelay() << "," << getFreeCnxCharge() << "," \
     << "0" << "," << "0" << "," << getLegAdev() << "," << getLegBdev() << "," << getTalkTime() << "," \
     << getTaskId() << "," << getSessionId() << "," << getSessionId() << "," << getActId() << "," << getNetratio() << "," \
     << "0.0" << "," << "0.0" << "," << getBreakageChargeMode() << "," << getCnxFactor() << "," \
     << getAccessType() << "," << TmStrFormat("%Y") << "," \
     << TmStrFormat("%m") << "," << TmStrFormat("%d") << "," << TmStrFormat("%H") << "," << TmStrFormat("%M") << "," \
     << getCurrCode() << "," << getFreeFlag() << "," << getCliCategory() << "," << getMvnoCallmode() << "\r\n";
 
 cdrline = oss.str();
  //mTrace(getTrace(), cdrline.c_str());
 return cdrline;
}


bool BSCDR::writeCDRToFile()
{
  STR data("");
  data = WriteToCdr();
  return WriteToFile(data);
}

