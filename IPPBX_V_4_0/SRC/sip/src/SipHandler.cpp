/*
********************************************************************************
*
*           Copyright  2020 by VECTONE MOBILE.
*           All rights reserved
*
* Description: This contains the class definition for the SipLogger
*           This file contains the definition of a method used to
*           log messages of different severity levels supported by ACE.
*
* Author:      Narrain Prithvi
*
* Change History:
*
* Problem#         Fixed by                        Old Ver#
*
*******************************************************************************
*/

#include "SipHandler.h"

/***
 *
 */
SipHandler::SipHandler()
{
}

/***
 *
 */
SipHandler::~SipHandler()
{
}

/***
 *
 */
void SipHandler::SipSendInvite(nua_handle_t *nh)
{
     nua_invite(nh, TAG_END());
}

/***
 *
 */
bool SipHandler::SipLibInit()
{
    try{
        su_init();                                    // Initltilizing sofia Libary
        su_home_init(&_mHomeMem);                    // Creating Memory for sofia Libary
        _mRoot = su_root_create(this);

        if( _mRoot == NULL ) return false;         // Creating Root Instance

    } catch(...){
        SIP_LOG((SIP_ERROR "SipHandler::SipLibInit(%s)", "Exception Occurred"));
        return false;
    }

    SIP_LOG((SIP_INFO"SipHandler::SipLibInit(%s)", "Success-Init"));
    return false;
}

/***
 *
 */
bool SipHandler::SipLibLoad()
{
    int level = 9;
    try{
        su_log_set_level(su_log_global, level);        // set log level for sofia library
        sip_update_default_mclass(sip_extend_mclass(NULL));
        _mNuaHandle =  nua_create(_mRoot, SipCallback, this, NUTAG_URL("sip:*:5070"),  TAG_END());

        if ( _mNuaHandle ) {
            SIP_LOG((SIP_INFO"Siphandler::SipLibLoad(%s)", "Sofia Stack Successfully Loaded"));
            return true;
        }

        //Add nua_set_param with necessary tags if you would like to supprt SDP

    }catch(...){
        SIP_LOG((SIP_ERROR"Siphandler::SipLibLoad(%s)", "Load library failed exception"));
        return false;
    }

    SIP_LOG((SIP_ERROR"Siphandler::SipLibLoad(%s)", "Sofia Library successfully laoded"));
    return true;
}

/***
 *
 */
bool SipHandler::RunEventLoop() //its dummy
{
    if(_mNuaHandle)
        su_root_run(_mRoot);
    else return false;

    nua_destroy(_mNuaHandle);
    return true;
}

/**
 *
 */
bool SipHandler::SipLibUnload()
{
    return true;
}

/**
 *
 */
void SipHandler::SipCallback(nua_event_t   event, int status, char const *phrase,
    nua_t *nua, nua_magic_t  *magic, nua_handle_t *nh, nua_hmagic_t *hmagic,
    sip_t const  *sip, tagi_t  tags[])
{
  switch (event) {
  case nua_i_invite:
    OnInviteRequest(status, phrase, nua, magic, nh, hmagic, sip, tags);
    break;

//   case nua_r_invite:
//     app_r_invite(status, phrase, nua, magic, nh, hmagic, sip, tags);
//     break;

  /* and so on ... */

  default:
    /* unknown event -> print out error message */
    if (status > 100) {
      SIP_LOG((SIP_ERROR"unknown event %d: %03d %s\n", event, status, phrase));
    }
    else {
      SIP_LOG((SIP_ERROR"unknown event %d\n", event));
    }
    SIP_LOG((SIP_INFO "%s ", tags));
    break;
  }
}


void SipHandler::OnInviteRequest(int status,char const *phrase,nua_t *nua,\
       nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,
       tagi_t        tags[])
{
    SIP_LOG((SIP_INFO "Invite Recived!" ));
    nua_respond(nh, 200, "OK", TAG_END());
}

/***
 *
 */
void SipHandler::SipRespond(nua_handle_t *nh,int status, char const *phrase)
{
/***
 *
 */
}
void SipHandler::SipRespond(nua_handle_t *nh,int status, char const *phrase,tagi_t tag[])
{
}
/***
 *
 */
void SipHandler::SipSendAck(nua_handle_t *nh)
{
}
/***
 *
 */
void SipHandler::SipSendBye(nua_handle_t *nh)
{
}

