/*
 * ******************************************************************************
 * Vectone Mobile
 * @file xGateSipRecording.cpp
 *
 * @brief This is Sip Recording header file.
 *
 * @author Narrain prithvi Dharuman
 * ******************************************************************************
 */

#ifndef XGATE_SIP_RECORDING
#define XGATE_SIP_RECORDING
#pragma once
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <sofia-sip/url.h>
#include <sofia-sip/nua.h>
#include <sofia-sip/sip_tag.h>

#include "xGateRSXMLParserUtil.h"
#include "xGateRSXMLParser.h"

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

struct SipRecEvent {
    typedef enum {START, STOP, HOLD, RESUME, NONE} State;
    std::string CallId;
    std::string DataMode;
    State       CallState;
    std::string CallType;
    std::string Domain;
    std::string From;
    std::string To;
    std::string QExtn;
};

struct find_callId : std::unary_function<NfsMetaData, bool> {
    NfsMetaData f_id;
    find_callId(NfsMetaData id) : f_id(id) { }
    bool operator()(NfsMetaData const& nfs) const {
        return nfs.CallId == f_id.CallId;
    }
};


class xGateSipRecording {
public:
    xGateSipRecording();
    ~xGateSipRecording();

    xGateSipRecording(const char* payload);
    xGateSipRecording(std::string payload);

    //get methods;
    std::string getXmlPayload();
    SipRec_RsMetadata getRecordingInfo();
    std::string base64Decode(const std::string &in);
    const std::string base64Decode(const void* data, const size_t &len);

    //XML Methods
    bool infoRequest(const char*);
    bool decodeXML();
    bool encodeXML();

    // XML parties info
    SipRecEvent getSipInfoAction();

    //Recording Related Methods
    bool sipecordingStart();
    bool sipecordingStop();
    bool sipecordingUpdate();
    void siprecSRSRegister(bool auth=false);

private:
    xGateRSXMLParser     m_xmlParser;
    SipRec_RsMetadata    m_confXML;
    std::string          m_payload;
    su_home_t            sofia_home;
    su_root_t*           sofia_root;
    nua_t *              sofia_nua;
    std::vector<std::string>  holdParties;
    //std::map<std::string, SipRecEvent>  holdParties;

public:
   void XMLParserTest();
   //To be moved to private after implementing APIs for these two
   std::vector<std::string>  StartParties;
   std::vector<std::string>  StartedParties;
   std::vector<NfsMetaData>  vNfsMetaData;
};
#endif
