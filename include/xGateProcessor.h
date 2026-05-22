#ifndef _XGATE_PROCESSOR_H
#define _XGATE_PROCESSOR_H

//ace includes
#include <ace/Task.h>

//local includes
#include "xGateUtil.h"
#include "xGateBaseMsg.h"

class xGateProcessor :  public ACE_Task<ACE_MT_SYNCH>
{
   public:
      xGateProcessor(const char*);
      virtual ~xGateProcessor(void);

      xGateRetVal init(int);
      virtual int svc(void);
      xGateRetVal stop();

      virtual xGateRetVal handle_msg(ACE_Message_Block*);
      virtual xGateRetVal process_msg(xGateBaseMsg*);

   protected:
      char m_serverType[SERVER_TYPE_SIZE];
      char m_encodeBuffer[ENCODE_BUF_SIZE];
      bool m_run;
};

#endif

