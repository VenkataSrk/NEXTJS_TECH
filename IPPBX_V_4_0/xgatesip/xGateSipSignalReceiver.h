#ifndef SIP_SIGNAL_RECEIVER
#define SIP_SIGNAL_RECEIVER
//ace includes
/*#include <ace/Reactor.h>
#include <ace/Svc_Handler.h>
#include <ace/Synch.h>
#include <ace/OS.h>
#include <ace/Task.h>*/

#include <ace/Task.h>

//// added for ippbx
class HA_CommandHandler: public ACE_Task<ACE_MT_SYNCH> 
{ 
private: 
	const char* name_; 
public: 
	HA_CommandHandler(const char* name) { name_ = name; }
	virtual int svc() 
	{ 
		ACE_DEBUG((LM_DEBUG,"(%t) starting up %s\n", name_));
		ACE_OS::sleep(2);
		while(1) 
		{ 
			ACE_Message_Block *mb;
			int result = this->getq(mb);
			process_message(mb);
		}
		return 0;
	}
	void process_message(ACE_Message_Block *mb) 
	{ 
		ACE_DEBUG((LM_DEBUG, "(%t) Processing message %s\n", name_)); 
		for(int i =0 ; i < 100; i++); //simulate compute bound task. 
	}
};


//local includes
#include "xGateUtil.h"
#include "SofiaSiphandler.h"
class xGateSipSignalReceiver : public ACE_Event_Handler{
    public:
        xGateSipSignalReceiver(const char*, ACE_Task<ACE_MT_SYNCH>*);
        virtual ~xGateSipSignalReceiver();
	    int init(void);
        static void run (void* arguments);
        int stop_thread(void);
        xGateSipController *pSipController;
    private:
        bool m_running;
        char m_serverType[SERVER_TYPE_SIZE];
};
#endif
