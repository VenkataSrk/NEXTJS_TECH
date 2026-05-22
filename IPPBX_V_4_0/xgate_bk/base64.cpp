/* $Header: /slsip/encryption/base64.c 1     6/02/00 6:25p Bennylp $

   Base64 encoder/decoder.
   (C) 2000 Switchlabs, Ltd.

   Based on http://src.openresources.com/debian/src/mail/HTML/R/base64_encode.html
   Copyright (C) 1994, 1995, 1996, 1997 Mew developing team.
   All rights reserved.

   $Log: /slsip/encryption/base64.c $
 * 
 * 1     6/02/00 6:25p Bennylp
 * Added to source control.

 */

#ifdef _cpluscplus
	extern "C"
{
#endif


#include <malloc.h>
#include "base64.h"

#ifndef NULL
#define	NULL	0
#endif

const char base64_char[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
    'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
    'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', '+', '/' };

#define PADDING	'='

#define OOB -1

const char base256_char[] = {
    OOB,OOB,OOB,OOB, OOB,OOB,OOB,OOB, OOB,OOB,OOB,OOB, OOB,OOB,OOB,OOB,

    OOB,OOB,OOB,OOB, OOB,OOB,OOB,OOB, OOB,OOB,OOB,OOB, OOB,OOB,OOB,OOB,
  /*                                                -                / */
    OOB,OOB,OOB,OOB, OOB,OOB,OOB,OOB, OOB,OOB,OOB, 62, OOB,OOB,OOB, 63,
  /*  0   1   2   3    4   5   6   7    8   9                =        */
     52, 53, 54, 55,  56, 57, 58, 59,  60, 61,OOB,OOB, OOB,OOB,OOB,OOB,
  /*      A   B   C    D   E   F   G    H   I   J   K    L   M   N   O*/
    OOB,  0,  1,  2,   3,  4,  5,  6,   7,  8,  9, 10,  11, 12, 13, 14,
  /*  P   Q   R   S    T   U   V   W    X   Y   Z                     */
     15, 16, 17, 18,  19, 20, 21, 22,  23, 24, 25,OOB, OOB,OOB,OOB,OOB,
  /*      a   b   c    d   e   f   g    h   i   j   k    l   m   n   o*/
    OOB, 26, 27, 28,  29, 30, 31, 32,  33, 34, 35, 36,  37, 38, 39, 40,
  /*  p   q   r   s    t   u   v   w    x   y   z                     */
     41, 42, 43, 44,  45, 46, 47, 48,  49, 50, 51,OOB, OOB,OOB,OOB,OOB, 
};


void base256to64(unsigned char c1, unsigned char c2, unsigned char c3, int padding, char *output)
{
    *output++ = base64_char[c1>>2];
    *output++ = base64_char[((c1 & 0x3)<< 4) | ((c2 & 0xF0) >> 4)];
    switch (padding) {
    case 0:
	*output++ = base64_char[((c2 & 0xF) << 2) | ((c3 & 0xC0) >>6)];
	*output = base64_char[c3 & 0x3F];
	break;
    case 1:
	*output++ = base64_char[((c2 & 0xF) << 2) | ((c3 & 0xC0) >>6)];
	*output = PADDING;
	break;
    case 2:
    default:
	*output++ = PADDING;
	*output = PADDING;
	break;
    }
}

char *base64_encode(const unsigned char *input, size_t length)
{
    const unsigned char *pi = input;
    unsigned char c1, c2, c3;
    unsigned line_len = 0;
    size_t len = 0;
    char *output = malloc(length * 3 / 2 + 3);
    char *po = output;

    if (!output)
	return NULL;

    while (len < length) {
	c1 = *pi++;
	++len;

	if (len == length) {
	    base256to64(c1, 0, 0, 2, po);
	    po += 4;
	    break;
	} else {
	    c2 = *pi++;
	    ++len;

	    if (len == length) {
		base256to64(c1, c2, 0, 1, po);
		po += 4;
		break;
	    } else {
		c3 = *pi++;
		++len;
		base256to64(c1, c2, c3, 0, po);
	    }
	}

	po += 4;
	line_len += 4;
	if (line_len >= 72) {
	    *po++ = '\r';
	    *po++ = '\n';
	    line_len = 0;
	}
    }
    *po = '\0';
    return output;
}


unsigned char *base64_decode(const char *input, size_t in_length, size_t *length)
{
#define ADV	while(*pi=='\r' || *pi=='\n') { ++pi; }

    unsigned char *output = malloc(in_length);
    int c1, c2, c3, c4;
    const char *pi = input;
    unsigned char *po = output;
    *length = 0;
    
    if (!output) {
	*length = 0;
	return NULL;
    }

    ADV;
    while ((c1 = *pi++) != 0) {
	if ((c1 = base256_char[c1]) == OOB) {
	    goto on_error;
	}

	ADV;
	if ((c2 = *pi++) == 0) {
	    goto on_error;
	} else if ((c2 = base256_char[c2]) == OOB) {
	    goto on_error;
	}

	*po++ = (unsigned char)((c1 << 2) | ((c2 & 0x30) >> 4));
	++*length;

	ADV;
	if ((c3 = *pi++) == 0) {
	    goto on_error;
	} else if (c3 == PADDING) {
	    break;
	} else if ((c3 = base256_char[c3]) == OOB) {
	    goto on_error;
	}
    
	*po++ = (unsigned char)(((c2 & 0XF) << 4) | ((c3 & 0x3C) >> 2));
	++*length;
    
	ADV;
	if ((c4 = *pi++) == 0) {
	    goto on_error;
	} else if (c4 == PADDING) {
	    break;
	} else if ((c4 = base256_char[c4]) == OOB) {
	    goto on_error;
	}
		
	*po++ = (unsigned char)(((c3 & 0x03) << 6) | c4);
	++*length;

	ADV;
    }

    *po = '\0';
    return output;

on_error:
    *po = '\0';
    return output;
}

#ifdef __cpluscplus
}
#endif
