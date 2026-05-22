#ifndef SIP_SIGNAL_DISPATCHER
#define SIP_SIGNAL_DISPATCHER
#include "xGateDispatcher.h"

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
        void SendSipMessage(xGateCallObj *callobj);


};
#endif
