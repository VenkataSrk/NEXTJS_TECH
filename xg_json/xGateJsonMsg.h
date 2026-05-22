#pragma once

//local includes
#include <iostream>
#include <memory>
#include <vector>

constexpr const char* jbegin {"{\""};
constexpr const char* jend{"\"}"};
constexpr const char* jcomma{"\",\""};
constexpr const char* jcolon{"\":\""};

enum class JsonOPRType
{
		EN_JSON_INVALID = 0,
		EN_JSON_REG_REQ = 1,
		EN_JSON_REG_RES = 2,
		EN_JSON_REG_RES_ERROR = 3,
		EN_JSON_REG_UPDATE_AUTH_SUCCESS = 4,
		EN_JSON_REG_UPDATE_AUTH_FAILED = 5,
		EN_JSON_INV_REQ = 6,
		EN_JSON_INV_RES = 7,
		EN_JSON_INV_RES_ERROR = 8,
		EN_JSON_INV_INB_PSTN_REQ = 9,
		EN_JSON_INV_INB_PSTN_RES = 10,
		EN_JSON_INV_INB_PSTN_RES_ERROR = 11,
		EN_JSON_INV_MS_TEAMS_REQ = 12,
		EN_JSON_INV_MS_TEAMS_RES = 13,
		EN_JSON_ADD_TRUNK_IP_TO_WL = 14,
		EN_JSON_REMOVE_TRUNK_IP_FROM_WL = 15,
		EN_JSON_ADD_MS_TEAMS_DOMAIN_TO_WL = 16,
		EN_JSON_REMOVE_MS_TEAMS_DOMAIN_FROM_WL = 17
};

class JsonRegMsg
{
	public:
		JsonRegMsg() = default;
		~JsonRegMsg() = default;

		std::string m_sTransId{""};
		std::string m_sCallid{""};
		std::string m_sDomainName{""};
		std::string m_sContactAdd{""};
		std::string m_sUserName{""};
		std::string m_sUserID{""};
		std::string m_sDeviceType{""};
		std::string m_sAppType{""};
		std::string m_sDeviceId{""};
		std::string m_sIpAddress{""};
		std::string m_sExpires{""};
		std::string m_sCseq{""};
		std::string m_PuName{""};
		std::string m_sTranstype{""};
		std::string m_sRealm{""};
		std::string m_sNonce{""};
		std::string m_sUri{""};
		std::string m_sAor{""};
		std::string m_sPassword{""};
};

class JsonInvMsg
{
	public:
		JsonInvMsg()=default;
		~JsonInvMsg()=default;

		std::string m_sTransId{""};
		std::string m_sCallid{""};
		std::string m_sDomainName{""};
		std::string m_sFromNumber{""};
		std::string m_sToNumber{""};
		std::string m_sPassword{""};
	 	std::string m_sRouteIP{""};
		std::string m_sKey{""};
		std::string m_sCallInfo{""};
		std::string m_sExt{""};
};

class JsonWLUpdateMsg
{
	public:
		JsonWLUpdateMsg()=default;
		~JsonWLUpdateMsg()=default;

		//Sip trunk
		int m_iTrunkId;
		std::string m_sDCType;
		std::vector<std::string> m_IPAddrList;

		//Ms Teams
		std::string m_sSbcFqdn;
};

class xGateJsonMsg
{
	public:
		xGateJsonMsg()=default;
		~xGateJsonMsg()=default;
		std::unique_ptr<JsonRegMsg> m_uptReg;
		std::unique_ptr<JsonInvMsg> m_uptInv;
		std::unique_ptr<JsonWLUpdateMsg> m_uptWL;
		void set_JOprType(JsonOPRType JOPRType)
		{
	 		m_JOprType = JOPRType;
		}

		JsonOPRType get_JOprType()
		{
			return m_JOprType;
		}
	private:
    	JsonOPRType m_JOprType;
};
