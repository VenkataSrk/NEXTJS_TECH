#include "BSCDS.h"

STR BSCDS::WriteToCds()
{
  STR cdsdata;
  std::ostringstream oss;
  oss << getSiteCode() << "," << getSwitchCode() << "," \
      << TmStrFormat( "%Y-%m-%d %H:%M:%S") << "," << getSessionId() << "," \
      << getTrunkName() << ","<< getTrunkOut() << "," << getLegAdev() << "," << getLegBdev() << "," \
      << getCli() << "," << getDdi() << "," \
      << getUserDialedNum() << "," << getHoldTime() << "," <<getTalkTime() << "," \
      << "0" << "," << getCallCause() << "," << TmStrFormat("%Y") << "," << TmStrFormat("%m") << "," \
      << TmStrFormat("%d") << "," << TmStrFormat("%H") << "," \
      << TmStrFormat("%M") << "," << getPrefixDest() << "," << getDestCode() << "," \
      << getConnectionState() << "," << getDisconnectReqBy() << "," << getSiteCode() << "," << getCostPrice() << "," \
      << getTimeCls() << "," << "1" << "," << getCrOpr() << "," << getSiteCode() << "," << "1" << ","\
      << getCurrCode() << "," << getRouteCls() << "," << getCostPrice() << "," << "0,0,0" << "\r\n";
cdsdata = oss.str();
//mTrace(getTrace(), cdsdata.c_str());
return cdsdata;
}


bool BSCDS::writeCDSToFile()
{
  STR data("");
  data = WriteToCds();
  return WriteToFile(data);
}
