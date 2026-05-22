#ifndef __SIP_SIGNAL_DISPATCHER__
#define __SIP_SIGNAL_DISPATCHER__
#include "xGateDispatcher.h"
#include "xGateUtil.h"
#include <ace/Task.h> //DebugVM
#include <ace/Singleton.h> //DebugVM


#include "SofiaSiphandler.h"
class xGateSipSignalDispatcher: public xGateDispatcher{
    public:
        xGateSipSignalDispatcher();
        virtual ~xGateSipSignalDispatcher();
        //xGateRetVal init();
        virtual int svc( void );
        virtual xGateRetVal process_msg(ACE_Message_Block*);
        xGateRetVal stop();
        bool m_run;
        //DebugVM void SendSipMessage(xGateCallObj *callobj);


};
typedef ACE_Unmanaged_Singleton<xGateSipSignalDispatcher, ACE_Null_Mutex> SIP_SIGNAL_DISPATCHER;
#endif
