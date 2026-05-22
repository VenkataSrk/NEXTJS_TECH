#include "BSCRR.h"

STR BSCRR::WriteToCrr()
{
  STR crrdata;
  std::ostringstream oss;
  oss << getSiteCode() << "," << getSwitchCode() << "," \
      << TmStrFormat( "%Y-%m-%d %H:%M:%S") << "," << getSessionId() << "," \
      << getTrunkName() << "," << getLegAdev() << "," << getCli() << "," << getDdi() << "," \
      << getUserDialedNum() << "," << getPrefixCode() << "," << "1" << "," << getChoice() << "," \
      << getCallDomain() << "," << getTrunkOut() << "," << getRlsIp() << ","  << getLegBdev() << "," \
      << TmStrFormat("%Y-%m-%d %H:%M:%S") << "," << "0" << "," \
      << "0" << "," << getCallCause() << "," << getConnectionState() << "," << getDisconnectReqBy() << "," \
      << getCrrCause() << "," << getTalkTime() << "," <<getTalkTime() << "," << getCostPrice() << "," \
      << getTariffCls() << "," << getTariff() << "," << getAccess() << "," << getPrefixCode() \
      << "," << "3" << "," << getDestCode() << "," << getHoldTime() << "\r\n";
    crrdata = oss.str();
    //mTrace(getTrace(), crrdata.c_str());
    return crrdata;
}


bool BSCRR::writeCRRToFile()
{
  STR data("");
  data = WriteToCrr();
  return WriteToFile(data);
}
