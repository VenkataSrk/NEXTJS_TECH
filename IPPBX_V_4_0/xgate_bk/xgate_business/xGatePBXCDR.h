#ifndef XGATE_PBX_CDR
#define XGATE_PBX_CDR
#include "xGateUtil.h"
class xGatePBXCDR{
public:
xGatePBXCDR();
~xGatePBXCDR();
//void updateCDRInfo(callInfoParameter cp);
//std::string setJsonString(xGateCDRStruct cdrinfo);
//void fillCDRInfo(callInfoParameter cp);
//void postMessage(xGateCDRStruct cdrinfo);
void postMessage(std::string input, std::string uid);
};
#endif
