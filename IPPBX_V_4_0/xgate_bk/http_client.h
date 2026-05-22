/*----------------------------------------------------------------* 
 * $Archive: /xGate/http_client.h $
 * $Date: 15/04/03 17:33 $
 * $Revision: 1 $
 *
 * $History: http_client.h $
 * 
 * *****************  Version 1  *****************
 * User: Junanto      Date: 15/04/03   Time: 17:33
 * Created in $/xGate
 * version 1
 *----------------------------------------------------------------*/

/*----------------------------------------------------------------* 
 * Description:
 *  
 *----------------------------------------------------------------*/

#if !defined(_HTTP_CLIENT_H__INCLUDED_)
#define _HTTP_CLIENT_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define HTTP_ERR_INVALID_HANDLE     -400
#define HTTP_ERR_INVALID_URL	    -401
#define HTTP_ERR_INVALID_SCHEME	    -402
#define HTTP_ERR_CANT_CONNECT	    -403
#define HTTP_ERR_EOF		    -404

#define HTTP_ERR_WINET_BASE	    -410

void CreateHttpClientManager();


#endif // !defined(_HTTP_CLIENT_H__INCLUDED_)
