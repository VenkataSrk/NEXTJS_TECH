/*
********************************************************************************
*
*           Copyright  2020 by VECTONE MOBILE.
*           All rights reserved
*
* Description: This contains the class definition for the SipHandler Header
*           This file contains the definition of a method used to
*           log messages of different severity levels supported by ACE.
*
* Author:      Narrain Prithvi Dharuman
*
* Change History:
*
* Problem#         Fixed by                        Old Ver#
*
*******************************************************************************
*/

#ifndef __SIP_HANDLER_H
#define __SIP_HANDLER_H
extern "C"
{
#include <sofia-sip/nua.h>
#include <sofia-sip/sip_extra.h>
#include <sofia-sip/url.h>
#include <sofia-sip/sdp.h>
#include <sofia-sip/auth_module.h>
#include <sofia-sip/auth_plugin.h>

#include <sofia-sip/su_log.h>
}

#include "SipLogger.h"

extern su_log_t su_log_global[];

class SipHandler {
public:
    SipHandler();
    ~SipHandler();

    //SofiaLibrary init functions
    virtual bool SipLibInit();   //Create and initialize Sofia Sip
    virtual bool SipLibLoad();   //Load Share Sofia lib & create call back
    virtual bool SipLibUnload();
    virtual bool RunEventLoop();   // Get Message Queue by polling method

    virtual void SipSendInvite(nua_handle_t *nh);
    virtual void SipRespond(nua_handle_t *nh,int status, char const *phrase);
    virtual void SipRespond(nua_handle_t *nh,int status, char const *phrase,tagi_t tag[]);
    virtual void SipSendAck(nua_handle_t *nh);
    virtual void SipSendBye(nua_handle_t *nh);
    static void SipCallback(nua_event_t   event, int status, char const *phrase, nua_t *nua,
                  nua_magic_t  *magic, nua_handle_t *nh, nua_hmagic_t *hmagic, sip_t const  *sip,
                  tagi_t        tags[]);



#if 0
   virtual bool RespondWith1xx(void *pSipMgc);
   virtual bool RespondWith2xx(void *pSipMgc);
   virtual bool RespondWith3xx(void *pSipMgc);
   virtual bool RespondWith4xx(void *pSipMgc);
   virtual bool RespondWith5xx(void *pSipMgc);
   virtual bool RespondWith6xx(void *pSipMgc);
#endif

   static void OnInviteRequest(int status,char const *phrase,nua_t *nua,\
       nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,
       tagi_t        tags[]);
#if 0
   virtual void OnInfoRequest(int status,char const *phrase,nua_t *nua,\
       nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,
       tagi_t        tags[]);
   virtual void OnByeRequest(int status,char const *phrase,nua_t *nua,\
       nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,
       tagi_t        tags[]);

   virtual void OnStateChange(int status,char const *phrase,nua_t *nua,\
       nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,
       tagi_t        tags[]);
   virtual void OnRequestResponse(int status,char const *phrase,nua_t *nua,\
       nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,
       tagi_t        tags[]);
   virtual void OnFailureResponse(nua_hmagic_t *hmagic, int status);
   virtual void OnInviteCancel(int status,char const *phrase,nua_t *nua,\
       nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,
       tagi_t        tags[]);
   virtual void RespondToPeer(int status,xGateSofiaobj *sobj);
#endif

private:
   su_home_t        _mHomeMem;
   su_root_t        *_mRoot;
   nua_t            *_mNuaHandle;

};


#endif //__SIP_HANDLER_H
