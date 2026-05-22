#ifndef __URCBCALLCONTEXTHANDLER_H__
#define __URCBCALLCONTEXTHANDLER_H__

#include "URCBCallContext.h"
#include <map>
#include <string>

class URCBCallContextHandler
{
   typedef std::map<std::string, URCBCallContext*> URCB_CALL_CONTEXT_MAP;
   typedef URCB_CALL_CONTEXT_MAP::iterator IT_URCB_CALL_CONTEXT_MAP;


   public:
        URCBCallContextHandler()
        {
        }
        ~URCBCallContextHandler()
        {}
        
        URCB_CALL_CONTEXT_MAP m_tCallContext;

        bool insertURCBCallContext(std::string &sID, URCBCallContext* ptCtx);
        bool DeleteURCBCallContext(std::string &sId);
        bool getURCBCallContext(std::string &sID, URCBCallContext *& ptCtx);
};
#endif

   
