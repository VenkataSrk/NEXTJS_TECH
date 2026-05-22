/* $Header: /slsip/encryption/base64.h 1     6/02/00 6:25p Bennylp $

   Base64 encoder/decoder.
   (C) 2000 Switchlabs, Ltd.

   Based on http://src.openresources.com/debian/src/mail/HTML/R/base64_encode.html
   Copyright (C) 1994, 1995, 1996, 1997 Mew developing team.
   All rights reserved.

   $Log: /slsip/encryption/base64.h $
 * 
 * 1     6/02/00 6:25p Bennylp
 * Added to source control.

 */

#ifndef __BASE64_H__
#define __BASE64_H__


#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus

    char *base64_encode(const unsigned char *s, size_t length);
    unsigned char *base64_decode(const char *s, size_t in_length, size_t *length);

#ifdef __cplusplus
};
#endif	// __cplusplus



#endif
