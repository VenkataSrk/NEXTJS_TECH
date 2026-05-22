/*
********************************************************************************
*
*           Copyright  2020 by VECTONE MOBILE.
*           All rights reserved
*
* Description: This contains a sample application to test common ana xgate sip library
*
*
* Author:      Narrain Prithvi
*
* Change History:
*
* Problem#         Fixed by                        Old Ver#
*
*******************************************************************************
*/

#include "SipLogger.h"
#include "SrcIdGen.h"
#include "SipHandler.h"

//using namespace vectone;
#if 0
int main(int argc, char *argv[])
{
    SipLog::getLogInstance()->setLogLevels(0,0,0,1,0);
    SIP_LOG((LM_INFO, "Test Logger Message!\n"));
    DBG("Hello DBG!");
    return 0;
}
#endif
void SipSessionIDTest()
{
    SrcIdGen sessionID;
    std::string session_id = sessionID.getSipSessionId();
    SIP_LOG((SIP_INFO" Sip Session Id & Session-ID header = %s !\n", session_id.c_str()));
}

void SofiaStackTest()
{
    SipHandler siphandler;
    siphandler.SipLibInit();
    siphandler.SipLibLoad();
    siphandler.RunEventLoop();
}

void SessionIDTest()
{
    SrcIdGen sessionID;

    std::string session_id = sessionID.getSessionId("mysessiona");
    SIP_LOG((SIP_INFO" session_id is created = %s !\n", session_id.c_str()));

    std::string participant_id = sessionID.getSessionId("participanta");
    SIP_LOG((SIP_INFO" participant_id is created = %s !\n", participant_id.c_str()));

    std::string stream_id= sessionID.getSessionId("streamid");
    SIP_LOG((SIP_INFO" stream_id is created = %s !\n", stream_id.c_str()));

    std::string send_a = sessionID.getSessionId("senda");
    SIP_LOG((SIP_INFO" send_party_a is created = %s !\n", send_a.c_str()));

    std::string send_b = sessionID.getSessionId("sendb");
    SIP_LOG((SIP_INFO" send_party_b is created = %s !\n", send_b.c_str()));

    std::string recv_a = sessionID.getSessionId("recva");
    SIP_LOG((SIP_INFO" recv_party_a is created = %s !\n", recv_a.c_str()));

    std::string recv_b = sessionID.getSessionId("srecv");
    SIP_LOG((SIP_INFO" srecvparty_b is created = %s !\n", recv_b.c_str()));
}

int main()
{
    SipLog *_log = SipLog::getLogInstance("SipLog.log");
    int i = 100;
    _log -> setLogTargets( 10000, true, false);
    _log -> setLogLevels(1,1,1,1,1);
    //_log -> setWarn();
    _log -> setLogLineCount (5);
    SIP_LOG ((SIP_DEBUG" debug log = %d\n",i));
    SIP_LOG ((SIP_INFO" debug log = %d\n",i));
    SIP_LOG ((SIP_WARNING" debug log = %d\n",i));

    SipSessionIDTest();
    SessionIDTest();
    SofiaStackTest();
    return 0;
}
