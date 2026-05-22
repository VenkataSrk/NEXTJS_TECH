#ifndef XGATE_PARSER_UTILITY
#define XGATE_PARSER_UTILITY
#include <string>
#include <iostream>
#include <vector>
 enum XPARSER_ERROR{
FAIL=0,
SUCCESS,
PARSER_ERROR,
RUNTIME_ERROR,
STANDARD_ERROR,
UNKNOWN_ERROR
};
 struct SIP_INFO{
std::string sipuri;
std::string tagid;
};
struct CALL_INFO{
std::string index;
std::string callInfo; // call info entity
std::string callStatus; // state of the call
std::string callId; // call id of particular leg
std::string confDisplayText;
SIP_INFO toHeader; // to header info
SIP_INFO fromHeader; // from header info
};

struct USER_INFO{
std::string entity;
std::string type;
std::string index;
std::string callCount;
std::vector<CALL_INFO*> callInfo;

//std::vector<CALL_INFO*> callInfo;
};
struct CONF_INFO{
std::string confid; // unique conference id for host and particpant join
std::string confState;// state of the conference
};

struct HOST_INFO{
std::string entity;
std::string confDisplayName;
};

#endif
