#ifndef _XGATE_CALL_INFO_ENCODER_H
#define _XGATE_CALL_INFO_ENCODER_H

#include <iostream>
#include <map>
#include <sstream>

#include "xglog.h"
#include "xGateUtil.h"
//#include "xGateSipRecording.h"
#include "xGateRSXMLParserUtil.h"
using namespace std;
constexpr const char* squoted {"\'"};
constexpr const char* comma {","};
constexpr const char* sqcomma {"\',\'"}; 

class xGateCallInfoEncoder
{
  public:
    xGateCallInfoEncoder();
    virtual ~xGateCallInfoEncoder();

    std::string encode_call_info_json_msg(NfsMetaData nfsmetadata);
    std::string encode_call_info_db_msg(NfsMetaData nfsmetadata);
    std::string encode_ccaas_call_info_db_msg(NfsMetaData nfsmetadata);
    std::string encode_crm_call_info_db_msg(NfsMetaData nfsmetadata);
    Value setJsonParam(string &strValue, Document &doc);
    Value setJsonParam(unsigned int iValue, Document &doc);

};

#endif

