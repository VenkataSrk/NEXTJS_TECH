#include "xGateRapidJsonAdaptor.h"

/*
*Encoding the xGateJsonMsg to Json string using ostringstream
*/
std::string  xGateRapidJsonAdaptor::Encode(std::unique_ptr<xGateJsonMsg> uptJMsg)
{
    if (!uptJMsg)
    {
        XGLOG_ERROR("URRapdiJsonAdaptor::EncodeJson uptJMsg is Null");
        return "";
    }

    std::ostringstream oss{};

    switch (uptJMsg->get_JOprType())
    {
    case JsonOPRType::EN_JSON_REG_REQ:
    {

        oss << jbegin << "trans_id" << jcolon << uptJMsg->m_uptReg->m_sTransId << jcomma
            << "msg_type" << jcolon << "REGISTRATION_REQ" << jcomma
            << "call_id" << jcolon << uptJMsg->m_uptReg->m_sCallid << jcomma
            << "domain_name" << jcolon << uptJMsg->m_uptReg->m_sDomainName << jcomma
            << "contact_address" << jcolon << uptJMsg->m_uptReg->m_sContactAdd << jcomma
            << "user_name" << jcolon << uptJMsg->m_uptReg->m_sUserName << jcomma
            << "user_id" << jcolon << uptJMsg->m_uptReg->m_sUserID << jcomma
            << "device_type" << jcolon << uptJMsg->m_uptReg->m_sDeviceType << jcomma
            << "app_type" << jcolon << uptJMsg->m_uptReg->m_sAppType << jcomma
            << "device_id" << jcolon << uptJMsg->m_uptReg->m_sDeviceId << jcomma
            << "ip_address" << jcolon << uptJMsg->m_uptReg->m_sIpAddress << jcomma
            << "expires" << jcolon << uptJMsg->m_uptReg->m_sExpires << jcomma
            << "cseq" << jcolon << uptJMsg->m_uptReg->m_sCseq << jcomma
            << "proxy_user_name" << jcolon << uptJMsg->m_uptReg->m_PuName << jcomma
            << "transport_type" << jcolon << uptJMsg->m_uptReg->m_sTranstype << jend;

        break;
    }
    case JsonOPRType::EN_JSON_REG_UPDATE_AUTH_SUCCESS:
    {
        oss << jbegin << "trans_id" << jcolon << uptJMsg->m_uptReg->m_sTransId << jcomma
            << "msg_type" << jcolon << "REGISTRATION_UPDATE_AUTH_SUCCESS" << jcomma
            << "call_id" << jcolon << uptJMsg->m_uptReg->m_sCallid << jend;
        break;
    }
    case JsonOPRType::EN_JSON_REG_UPDATE_AUTH_FAILED:
    {
        oss << jbegin << "trans_id" << jcolon << uptJMsg->m_uptReg->m_sTransId << jcomma
            << "msg_type" << jcolon << "REGISTRATION_UPDATE_AUTH_FAILURE" << jcomma
            << "call_id" << jcolon << uptJMsg->m_uptReg->m_sCallid << jend;
        break;
    }
    case JsonOPRType::EN_JSON_INV_REQ:
    {
        oss << jbegin << "trans_id" << jcolon << uptJMsg->m_uptInv->m_sTransId << jcomma
            << "msg_type" << jcolon << "INVITE_REQ" << jcomma
            << "call_id" << jcolon << uptJMsg->m_uptInv->m_sCallid << jcomma
            << "key" << jcolon << uptJMsg->m_uptInv->m_sKey << jcomma
            << "calling_party" << jcolon << uptJMsg->m_uptInv->m_sFromNumber << jcomma
            << "called_party" << jcolon << uptJMsg->m_uptInv->m_sToNumber << jcomma
            << "calling_party_domain" << jcolon << uptJMsg->m_uptInv->m_sDomainName << jcomma
            << "called_party_domain" << jcolon << uptJMsg->m_uptInv->m_sDomainName << jend;
        break;
    }
    case JsonOPRType::EN_JSON_INV_INB_PSTN_REQ:
    {
        oss << jbegin << "trans_id" << jcolon << uptJMsg->m_uptInv->m_sTransId << jcomma
            << "msg_type" << jcolon << "INVITE_INBOUND_PSTN_REQ" << jcomma
            << "call_id" << jcolon << uptJMsg->m_uptInv->m_sCallid << jcomma
            << "calling_party" << jcolon << uptJMsg->m_uptInv->m_sFromNumber << jcomma
            << "called_party" << jcolon << uptJMsg->m_uptInv->m_sToNumber << jend;
        break;
    }
    case JsonOPRType::EN_JSON_INV_MS_TEAMS_REQ:
    {
        oss << jbegin << "trans_id" << jcolon << uptJMsg->m_uptInv->m_sTransId << jcomma
            << "msg_type" << jcolon << "INVITE_MS_TEAMS_REQ" << jcomma
            << "call_id" << jcolon << uptJMsg->m_uptInv->m_sCallid << jcomma
            << "key" << jcolon << uptJMsg->m_uptInv->m_sKey << jcomma
            << "calling_party" << jcolon << uptJMsg->m_uptInv->m_sFromNumber << jcomma
            << "called_party" << jcolon << uptJMsg->m_uptInv->m_sToNumber << jcomma
            << "calling_party_domain" << jcolon << uptJMsg->m_uptInv->m_sDomainName << jcomma
            << "called_party_domain" << jcolon << uptJMsg->m_uptInv->m_sDomainName << jcomma 
            << "calling_party_ext" << jcolon << uptJMsg->m_uptInv->m_sExt << jend;
        break;
    }
    default:
        break;
    }

    return oss.str();

}

/*
*Decoding the Json string to xGateJsonMsg using Rapidjson
*/
std::unique_ptr<xGateJsonMsg> xGateRapidJsonAdaptor::Decode(std::string sJson)
{
    std::unique_ptr<xGateJsonMsg> uptMsg = make_unique<xGateJsonMsg>();
    Document doc;
    if (!CreateDocObject(sJson, doc))
    {
        XGLOG_ERROR("DecodeJson failed while creating json document object !");
        uptMsg->set_JOprType(JsonOPRType::EN_JSON_INVALID);
        return std::move(uptMsg);
    }

    if (!doc.HasMember("msg_type"))
    {
        XGLOG_ERROR("DecodeJson failed. 'msg_type' field is missing !");
        uptMsg->set_JOprType(JsonOPRType::EN_JSON_INVALID);
        return std::move(uptMsg);
    }

    std::string sMsgType = doc["msg_type"].GetString();

    if (sMsgType == "REGISTRATION_RES")
    {
        uptMsg = ReadRegResMsg(sJson, doc);
    }
    else if (sMsgType == "REGISTRATION_RES_ERROR")
    {
        uptMsg = ReadRegResErrorMsg(sJson, doc);
    }
    else if (sMsgType == "INVITE_RES")
    {
        uptMsg = ReadInviteResMsg(sJson, doc);
    }
    else if (sMsgType == "INVITE_RES_ERROR")
    {
        uptMsg = ReadInviteResErrorMsg(sJson, doc);
    }
    else if(sMsgType == "INVITE_INBOUND_PSTN_RES")
    {
        uptMsg = ReadInviteInPstnResMsg(sJson, doc);
    }
    else if(sMsgType == "INVITE_INBOUND_PSTN_RES_ERROR")
    {
        uptMsg = ReadInviteInPstnResErrorMsg(sJson, doc);
    }
    else if(sMsgType == "INVITE_MS_TEAMS_RES")
    {
        uptMsg = ReadInviteMSTeamsResMsg(sJson, doc);
    }
    else if(sMsgType == "ADD_IP_TO_WHITE_LIST")
    {
        uptMsg = ReadAddTrunkIPToWhiteListMsg(sJson, doc);
    }
    else if(sMsgType == "REMOVE_IP_FROM_WHITE_LIST")
    {
        uptMsg = ReadRemoveTrunkIPFromWhiteListMsg(sJson, doc);
    }
    else if(sMsgType == "ADD_SBC_FQDN_TO_WHITE_LIST")
    {
        uptMsg = ReadAddMsTeamsDomainToWhiteListMsg(sJson, doc);
    }
    else if(sMsgType == "REMOVE_SBC_FQDN_FROM_WHITE_LIST")
    {
        uptMsg = ReadRemoveMsTeamsDomainFromWhiteListMsg(sJson, doc);
    }
    else
    {
        XGLOG_ERROR("xGateRapidJsonAdaptor::DecodeJson failed Message Type Not matching ");
    }
    return std::move(uptMsg);
}

/*
*Create Document object and parsing /validating the json string
*/
bool xGateRapidJsonAdaptor::CreateDocObject(std::string sJson,Document &doc)
{
    ParseResult ok = doc.Parse<kParseCommentsFlag>(sJson.c_str());
    if (!ok)
    {
        //XGLOG_ERROR("create_doc_object failed. due to json parse error: %s and jsondata:%s!", GetParseError_En(ok.Code()), sJson);
        return false;
    }
    else if (!doc.IsObject())
    {
        XGLOG_ERROR("create_doc_object failed. not able to create document object !");
        return false;
    }
    else if (!doc.HasMember("msg_type"))
    {
        XGLOG_ERROR("create_doc_object failed. 'msg_type' field is missing in json message !");
        return false;
    }
    return true;
}

/*
* Reading registeration response info from document
*/
std::unique_ptr<xGateJsonMsg> xGateRapidJsonAdaptor::ReadRegResMsg(std::string sJson,Document &doc)
{
    std::unique_ptr<xGateJsonMsg> uptrJMsg = make_unique<xGateJsonMsg>();
    std::unique_ptr<JsonRegMsg> uptReg = make_unique<JsonRegMsg>();

    uptrJMsg->set_JOprType(JsonOPRType::EN_JSON_REG_RES);

    if (doc.HasMember("trans_id")) {
         uptReg->m_sTransId = doc["trans_id"].GetString();    
    } else {
        XGLOG_ERROR("ReadRegResMsg failed. 'trans_id' field is missing !");
    }

    if(doc.HasMember("password")){
        uptReg->m_sPassword = doc["password"].GetString();
    } else {    
        XGLOG_ERROR("ReadRegResMsg failed. 'password' field is missing !");
    }
    
    uptrJMsg->m_uptReg = std::move(uptReg);
    return std::move(uptrJMsg);
}

/*
* Reading registeration error response info from document
*/
std::unique_ptr<xGateJsonMsg> xGateRapidJsonAdaptor::ReadRegResErrorMsg(std::string sJson, Document &doc)
{
    std::unique_ptr<xGateJsonMsg> uptrJMsg = make_unique<xGateJsonMsg>();
    std::unique_ptr<JsonRegMsg> uptReg = std::make_unique<JsonRegMsg>();

    uptrJMsg->set_JOprType(JsonOPRType::EN_JSON_REG_RES_ERROR);

    if (doc.HasMember("trans_id")){
         uptReg->m_sTransId = doc["trans_id"].GetString();
    } else {
        XGLOG_ERROR("ReadRegResMsg failed. 'trans_id' field is missing !");
    }

    uptrJMsg->m_uptReg = std::move(uptReg);
    return std::move(uptrJMsg);
}

/*
* Reading Invite response info from document
*/
std::unique_ptr<xGateJsonMsg> xGateRapidJsonAdaptor::ReadInviteResMsg(std::string sJson,Document &doc)
{
    std::unique_ptr<xGateJsonMsg> uptrJMsg = std::make_unique<xGateJsonMsg>();
    std::unique_ptr<JsonInvMsg>  uptrInv = std::make_unique<JsonInvMsg>();

    uptrJMsg->set_JOprType(JsonOPRType::EN_JSON_INV_RES);  

    if(doc.HasMember("trans_id")){
         uptrInv->m_sTransId = doc["trans_id"].GetString();
    } else {
        XGLOG_ERROR("ReadInviteResMsg failed. 'trans_id' field is missing !");
    }

    if(doc.HasMember("call_id")){
        uptrInv->m_sCallid = doc["call_id"].GetString();
    } else {
        XGLOG_ERROR("ReadInviteResMsg failed. 'callid' field is missing !");
    }

    if(doc.HasMember("password")){
        uptrInv->m_sPassword = doc["password"].GetString();
    } else {
        XGLOG_ERROR("ReadInviteResMsg failed. 'password' field is missing !");
    }

    if(doc.HasMember("route_pbx_ip")){
        uptrInv->m_sRouteIP = doc["route_pbx_ip"].GetString();
    } else {
         XGLOG_ERROR("ReadInviteResMsg failed. 'route_ip' field is missing !");
    }

    if(doc.HasMember("call-info") && doc["call-info"].IsObject()){
        uptrInv->m_sCallInfo = ConvertJsonObjToString(doc,"call-info");
    } else {
         XGLOG_ERROR("ReadInviteResMsg failed. 'call_info' field is missing !");
         uptrInv->m_sCallInfo = "";
    }
   
    uptrJMsg->m_uptInv = std::move(uptrInv);
    return std::move(uptrJMsg);
}

/*
* Reading Invite response error from document
*/
std::unique_ptr<xGateJsonMsg> xGateRapidJsonAdaptor::ReadInviteResErrorMsg(std::string sJson, Document &doc)
{
    std::unique_ptr<xGateJsonMsg> uptrJMsg = std::make_unique<xGateJsonMsg>();
    std::unique_ptr<JsonInvMsg> uptrInv = std::make_unique<JsonInvMsg>();

    uptrJMsg->set_JOprType(JsonOPRType::EN_JSON_INV_RES_ERROR);

    if(doc.HasMember("trans_id")){
        uptrInv->m_sTransId = doc["trans_id"].GetString();
    } else {
        XGLOG_ERROR("ReadInviteResMsg failed. 'trans_id' field is missing !");
    }

    if(doc.HasMember("call_id")){
        uptrInv->m_sCallid = doc["call_id"].GetString();
    } else {
        XGLOG_ERROR("ReadInviteResMsg failed. 'callid' field is missing !");
    }

    uptrJMsg->m_uptInv = std::move(uptrInv);
    return std::move(uptrJMsg);

}

/*
* Reading Invite Inbound PSTN response from document
*/
std::unique_ptr<xGateJsonMsg> xGateRapidJsonAdaptor::ReadInviteInPstnResMsg(std::string sJson,Document &doc)
{
    std::unique_ptr<xGateJsonMsg> uptrJMsg = std::make_unique<xGateJsonMsg>();
    std::unique_ptr<JsonInvMsg>  uptrInv = std::make_unique<JsonInvMsg>();

    uptrJMsg->set_JOprType(JsonOPRType::EN_JSON_INV_INB_PSTN_RES);  

    if(doc.HasMember("trans_id")){
         uptrInv->m_sTransId = doc["trans_id"].GetString();
    } else {
        XGLOG_ERROR("ReadInviteInPstnResMsg failed. 'trans_id' field is missing !");
    }

    if(doc.HasMember("call_id")){
        uptrInv->m_sCallid = doc["call_id"].GetString();
    } else {
        XGLOG_ERROR("ReadInviteInPstnResMsg failed. 'callid' field is missing !");
    }

    if(doc.HasMember("route_pbx_ip")){
        uptrInv->m_sRouteIP = doc["route_pbx_ip"].GetString();
    } else {
         XGLOG_ERROR("ReadInviteInPstnResMsg failed. 'route_ip' field is missing !");
    }

    if(doc.HasMember("call-info") && doc["call-info"].IsObject() && !doc["call-info"].Empty()){
        uptrInv->m_sCallInfo = ConvertJsonObjToString(doc,"call-info");
    } else {
         XGLOG_ERROR("ReadInviteInPstnResMsg failed. 'call_info' field is missing !");
         uptrInv->m_sCallInfo = "";
    }
   
    uptrJMsg->m_uptInv = std::move(uptrInv);
    return std::move(uptrJMsg);
}
/*
* Reading Invite Inbound PSTN response error from document
*/
std::unique_ptr<xGateJsonMsg> xGateRapidJsonAdaptor::ReadInviteInPstnResErrorMsg(std::string sJson,Document &doc)
{
    std::unique_ptr<xGateJsonMsg> uptrJMsg = std::make_unique<xGateJsonMsg>();
    std::unique_ptr<JsonInvMsg>  uptrInv = std::make_unique<JsonInvMsg>();

    uptrJMsg->set_JOprType(JsonOPRType::EN_JSON_INV_INB_PSTN_RES_ERROR);
    
    if(doc.HasMember("trans_id")){
         uptrInv->m_sTransId = doc["trans_id"].GetString();
    } else {
        XGLOG_ERROR("ReadInviteInPstnResMsg failed. 'trans_id' field is missing !");
    }

    if(doc.HasMember("msg_type")){
        uptrInv->m_sCallid = doc["msg_type"].GetString();
    } else {
        XGLOG_ERROR("ReadInviteInPstnResMsg failed. 'msg_type' field is missing !");
    }

    if(doc.HasMember("error_reason")){
        uptrInv->m_sRouteIP = doc["error_reason"].GetString();
    } else {
         XGLOG_ERROR("ReadInviteInPstnResMsg failed. 'error_reason' field is missing !");
    }

    uptrJMsg->m_uptInv = std::move(uptrInv);
    return std::move(uptrJMsg);
}

/*
* Reading Invite MS Teams response from document
*/
std::unique_ptr<xGateJsonMsg> xGateRapidJsonAdaptor::ReadInviteMSTeamsResMsg(std::string sJson,Document &doc)
{
    std::unique_ptr<xGateJsonMsg> uptrJMsg = std::make_unique<xGateJsonMsg>();
    std::unique_ptr<JsonInvMsg>  uptrInv = std::make_unique<JsonInvMsg>();

    uptrJMsg->set_JOprType(JsonOPRType::EN_JSON_INV_MS_TEAMS_RES);  

    if(doc.HasMember("trans_id")){
         uptrInv->m_sTransId = doc["trans_id"].GetString();
    } else {
        XGLOG_ERROR("ReadInviteMSTeamsResMsg failed. 'trans_id' field is missing !");
    }

    if(doc.HasMember("call_id")){
        uptrInv->m_sCallid = doc["call_id"].GetString();
    } else {
        XGLOG_ERROR("ReadInviteMSTeamsResMsg failed. 'callid' field is missing !");
    }

    if(doc.HasMember("route_pbx_ip")){
        uptrInv->m_sRouteIP = doc["route_pbx_ip"].GetString();
    } else {
         XGLOG_ERROR("ReadInviteMSTeamsResMsg failed. 'route_pbx_ip' field is missing !");
    }

    if(doc.HasMember("call-info") && doc["call-info"].IsObject() && !doc["call-info"].Empty()){
        uptrInv->m_sCallInfo = ConvertJsonObjToString(doc,"call-info");
    } else {
         XGLOG_ERROR("ReadInviteMSTeamsResMsg failed. 'call_info' field is missing !");
         uptrInv->m_sCallInfo = "";
    }
   
    uptrJMsg->m_uptInv = std::move(uptrInv);
    return std::move(uptrJMsg);
}

/*
* Reading Domain White List Update Msg from Interrogator
*/
std::unique_ptr<xGateJsonMsg> xGateRapidJsonAdaptor::ReadAddTrunkIPToWhiteListMsg(std::string sJson, Document &doc)
{
    std::unique_ptr<xGateJsonMsg> uptrJMsg = std::make_unique<xGateJsonMsg>();
    std::unique_ptr<JsonWLUpdateMsg> uptrWLMsg = std::make_unique<JsonWLUpdateMsg>();

    uptrJMsg->set_JOprType(JsonOPRType::EN_JSON_ADD_TRUNK_IP_TO_WL);

    if(doc.HasMember("trunk_id")) {
        uptrWLMsg->m_iTrunkId = doc["trunk_id"].GetInt(); 
    } else {
        XGLOG_ERROR("ReadAddTrunkIPToWhiteListMsg failed. 'trunk_id' field is missing !");
    }

    if(doc.HasMember("dc_type")) {
        uptrWLMsg->m_sDCType = doc["dc_type"].GetString(); 
    } else {
        XGLOG_ERROR("ReadAddTrunkIPToWhiteListMsg failed. 'dc_type' field is missing !");
    }

    if (doc.HasMember("ip_addr")) {
        const Value &ip_list = doc["ip_addr"];
        if (ip_list.IsArray() && ip_list.Size() > 0) {
            for (auto itr = ip_list.Begin(); itr != ip_list.End(); ++itr)
                uptrWLMsg->m_IPAddrList.push_back(itr->GetString());
        } else {
            XGLOG_ERROR("ReadAddTrunkIPToWhiteListMsg failed. 'ip_addr' is not a valid array!");
        }
    } else {
        XGLOG_ERROR("ReadAddTrunkIPToWhiteListMsg failed. 'ip_addr' field is missing !");
    }

    uptrJMsg->m_uptWL = std::move(uptrWLMsg);
    return std::move(uptrJMsg);
}

/*
* Reading Domain White List Update Msg from Interrogator
*/
std::unique_ptr<xGateJsonMsg> xGateRapidJsonAdaptor::ReadRemoveTrunkIPFromWhiteListMsg(std::string sJson, Document &doc)
{
    std::unique_ptr<xGateJsonMsg> uptrJMsg = std::make_unique<xGateJsonMsg>();
    std::unique_ptr<JsonWLUpdateMsg> uptrWLMsg = std::make_unique<JsonWLUpdateMsg>();

    uptrJMsg->set_JOprType(JsonOPRType::EN_JSON_REMOVE_TRUNK_IP_FROM_WL);

    if(doc.HasMember("trunk_id")) {
        uptrWLMsg->m_iTrunkId = doc["trunk_id"].GetInt(); 
    } else {
        XGLOG_ERROR("ReadRemoveTrunkIPFromWhiteListMsg failed. 'trunk_id' field is missing !");
    }

    if(doc.HasMember("dc_type")) {
        uptrWLMsg->m_sDCType = doc["dc_type"].GetString(); 
    } else {
        XGLOG_ERROR("ReadRemoveTrunkIPFromWhiteListMsg failed. 'dc_type' field is missing !");
    }

    if (doc.HasMember("ip_addr")) {
        const Value &ip_list = doc["ip_addr"];
        if (ip_list.IsArray() && ip_list.Size() > 0) {
            for (auto itr = ip_list.Begin(); itr != ip_list.End(); ++itr)
                uptrWLMsg->m_IPAddrList.push_back(itr->GetString());
        } else {
            XGLOG_ERROR("ReadRemoveTrunkIPFromWhiteListMsg failed. 'ip_addr' field is missing !");
        }
    } else {
        XGLOG_ERROR("ReadRemoveTrunkIPFromWhiteListMsg failed. 'ip_addr' field is missing !");
    }

    uptrJMsg->m_uptWL = std::move(uptrWLMsg);
    return std::move(uptrJMsg);
}

/*
* Convert Json Object to raw string
*/
std::string xGateRapidJsonAdaptor::ConvertJsonObjToString(Document &doc,std::string value)
{
    rapidjson::Value &call_info_obj = doc[value.c_str()];
    if(call_info_obj.ObjectEmpty())
        return "";
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    call_info_obj.Accept(writer);
    return buffer.GetString();
}

/*
* Reading MS TeamsDomain White List Update Msg from Interrogator
*/
std::unique_ptr<xGateJsonMsg> xGateRapidJsonAdaptor::ReadAddMsTeamsDomainToWhiteListMsg(std::string sJson, Document &doc)
{
    std::unique_ptr<xGateJsonMsg> uptrJMsg = std::make_unique<xGateJsonMsg>();
    std::unique_ptr<JsonWLUpdateMsg> uptrWLMsg = std::make_unique<JsonWLUpdateMsg>();

    uptrJMsg->set_JOprType(JsonOPRType::EN_JSON_ADD_MS_TEAMS_DOMAIN_TO_WL);

    if (doc.HasMember("sbc_fqdn"))
    {
        uptrWLMsg->m_sSbcFqdn = doc["sbc_fqdn"].GetString();
    }
    else
    {
        XGLOG_ERROR("ReadAddMsTeamsDomainToWhiteListMsg failed. 'sbc_fqdn' field is missing !");
    }

    uptrJMsg->m_uptWL = std::move(uptrWLMsg);
    return std::move(uptrJMsg);
}

/*
* Reading MS TeamsDomain White List Update Msg from Interrogator
*/
std::unique_ptr<xGateJsonMsg> xGateRapidJsonAdaptor::ReadRemoveMsTeamsDomainFromWhiteListMsg(std::string sJson, Document &doc)
{
    std::unique_ptr<xGateJsonMsg> uptrJMsg = std::make_unique<xGateJsonMsg>();
    std::unique_ptr<JsonWLUpdateMsg> uptrWLMsg = std::make_unique<JsonWLUpdateMsg>();

    uptrJMsg->set_JOprType(JsonOPRType::EN_JSON_REMOVE_MS_TEAMS_DOMAIN_FROM_WL);

    if (doc.HasMember("sbc_fqdn"))
    {
        uptrWLMsg->m_sSbcFqdn = doc["sbc_fqdn"].GetString();
    }
    else
    {
        XGLOG_ERROR("ReadRemoveMsTeamsDomainFromWhiteListMsg failed. 'sbc_fqdn' field is missing !");
    }

    uptrJMsg->m_uptWL = std::move(uptrWLMsg);
    return std::move(uptrJMsg);
}