#ifndef XGATE_MULTI_CONFERENCE
#define XGATE_MULTI_CONFERENCE
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "xGateSipUtil.h"
#include "log.h"
#include "klog.h"
#include "rapidxml-1.13/rapidxml.hpp"
#include "xGateXMLParser.h"
typedef struct CONF_XML{
CONF_INFO confInfo;
USER_INFO userInfo;
};
using namespace sipreturn;
class xGateMultiConference{
public:
xGateMultiConference();
~xGateMultiConference();
// Signal Event processing function
xGateReturn infoRequest(const char*);// Method used to handle sip info event
// Supporting function 
xGateReturn decodeXML(const char* payload, CONF_XML &xml); // decode the xml from paylos to conf xml structure
xGateReturn encodeXML(char *payload, CONF_XML xml); // encodes the CONF_XML structure data  to XML char pointer.
xGateReturn conferenceStart(CONF_XML xml);// on conference group creation function
xGateReturn conferenceStop(CONF_XML xml);// on conference particpant exit function 
xGateReturn conferenceUpdate(CONF_XML xml);// on conference new particpant joining
std::string getMultiConferenceId();
private:
xGateXMLParser m_xmlParser; // parser object
CONF_XML m_confXML; //  

public:
// Testing function 
void XMLParserTest();
};
#endif
