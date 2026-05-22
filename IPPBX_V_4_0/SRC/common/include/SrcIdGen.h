/*
********************************************************************************
*
*           Copyright  2020 by VECTONE MOBILE.
*           All rights reserved
*
* Description: This contains a class and member function to create crypto algorithms
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

#ifndef _SRC_IDGEN_ID_H
#define _SRC_IDGEN_ID_H

#include <uuid/uuid.h>
#include <string>

static const char* B64chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const size_t B64index[256] =
{
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  62, 63, 62, 62, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 0,  0,  0,  0,  0,  0,
    0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0,  0,  0,  0,  63,
    0,  26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
};


class SrcIdGen{
public:
    SrcIdGen();
    ~SrcIdGen();
    std::string getSipSessionId();
    std::string getSessionId(const char *value);

private:
    std::string m_SipSessionId;
    void getUUIDRandom(uuid_t *uuid);
    std::string UUIDToSessionId(const char *uuid, const size_t len);
    bool isValidSessionID(const char *uuid, const size_t len);
    const std::string hmac_sha1(char *callid, char *key);

    /*Base64  Member  Functions*/
    const std::string b64encode(const void* data, const size_t &len);
    const std::string b64decode(const void* data, const size_t &len);
    std::string b64encode(const std::string& str);
    std::string b64decode(const std::string& str64);

};
#endif //_SRC_IDGEN_ID_H
