/*
********************************************************************************
*
*           Copyright  2020 by VECTONE MOBILE.
*           All rights reserved
*
* Description: This contains a class and member function of crypto alg.
*
*
* Author:      Narrain Prithvi
*
* Change History:
*
* Problem#         Fixed by                        Old Ver#
*
*******************************************************************************
*/

#include "SrcIdGen.h"

#include <string.h>
#include <iostream>

SrcIdGen::SrcIdGen()
{}
SrcIdGen::~SrcIdGen()
{}
std::string SrcIdGen::getSipSessionId()
{
    uuid_t id;
    size_t    len;

    uuid_generate(id);

    char _string[100];
    uuid_unparse(id, _string);
    len = strlen(_string);

    std::cout << _string << std::endl;
    return UUIDToSessionId((char*)_string, len);
}


std::string SrcIdGen::getSessionId(const char *value)
{
    return b64encode( value, strlen(value) );
}

std::string SrcIdGen::UUIDToSessionId(const char *uuid, const size_t len)
{
    char session_uuid[33] = {0}; /*"local-uuid", per rfc7989*/
    if ((!uuid) || len != 36) return NULL;

    for(size_t i=0, j=0; i < len; i++)
    {
        if ( uuid[i] != '-' )
            session_uuid[j++] = uuid[i];
    }
    session_uuid[len+1] = '\0';

    if ( !isValidSessionID(session_uuid, strlen(session_uuid)) ) return NULL;
    std::string s(session_uuid);

    return s;
}

bool SrcIdGen::isValidSessionID(const char* uuid, const size_t len)
{
    for(size_t i=0; i < len; i++)
    {
        if( (uuid[i] < '0' || uuid[i] > '9' ) && ( uuid[i] < 'a' || uuid[i] > 'f')  ) return false;
    }
    return true;
}

void SrcIdGen::getUUIDRandom(uuid_t *uuid)
{

}

const std::string SrcIdGen::hmac_sha1(char *callid, char *key)
{
    return std::string("");
}

/*
 * Base64 Pvt Member Functions
 */

const std::string SrcIdGen::b64encode(const void* data, const size_t &len)
{
    std::string result((len + 2) / 3 * 4, '=');
    char *p = (char*) data, *str = &result[0];
    size_t j = 0, pad = len % 3;
    const size_t last = len - pad;

    for (size_t i = 0; i < last; i += 3)
    {
        size_t n = size_t(p[i]) << 16 | size_t(p[i + 1]) << 8 | p[i + 2];
        str[j++] = B64chars[n >> 18];
        str[j++] = B64chars[n >> 12 & 0x3F];
        str[j++] = B64chars[n >> 6 & 0x3F];
        str[j++] = B64chars[n & 0x3F];
    }
    if (pad)  /// set padding
    {
        size_t n = --pad ? size_t(p[last]) << 8 | p[last + 1] : p[last];
        str[j++] = B64chars[pad ? n >> 10 & 0x3F : n >> 2];
        str[j++] = B64chars[pad ? n >> 4 & 0x03F : n << 4 & 0x3F];
        str[j++] = pad ? B64chars[n << 2 & 0x3F] : '=';
    }
    return result;
}

const std::string SrcIdGen::b64decode(const void* data, const size_t &len)
{
    if (len == 0) return "";

    unsigned char *p = (unsigned char*) data;
    size_t j = 0,
        pad1 = len % 4 || p[len - 1] == '=',
        pad2 = pad1 && (len % 4 > 2 || p[len - 2] != '=');
    const size_t last = (len - pad1) / 4 << 2;
    std::string result(last / 4 * 3 + pad1 + pad2, '\0');
    unsigned char *str = (unsigned char*) &result[0];

    for (size_t i = 0; i < last; i += 4)
    {
        size_t n = B64index[p[i]] << 18 | B64index[p[i + 1]] << 12 | B64index[p[i + 2]] << 6 | B64index[p[i + 3]];
        str[j++] = n >> 16;
        str[j++] = n >> 8 & 0xFF;
        str[j++] = n & 0xFF;
    }
    if (pad1)
    {
        size_t n = B64index[p[last]] << 18 | B64index[p[last + 1]] << 12;
        str[j++] = n >> 16;
        if (pad2)
        {
            n |= B64index[p[last + 2]] << 6;
            str[j++] = n >> 8 & 0xFF;
        }
    }
    return result;
}

std::string SrcIdGen::b64encode(const std::string& str)
{
    return b64encode(str.c_str(), str.size());
}

std::string SrcIdGen::b64decode(const std::string& str64)
{
    return b64decode(str64.c_str(), str64.size());
}

