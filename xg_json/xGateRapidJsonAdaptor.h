#pragma once

//c++ includes
#include<sstream>
#include <iostream>

//local includes
#include "xGateJson.h"
#include "xGateUtil.h"
#include "xglog.h"

class xGateRapidJsonAdaptor : public xGateJson
{
		public:
			xGateRapidJsonAdaptor()=default;
			~xGateRapidJsonAdaptor()=default;

			std::string Encode(std::unique_ptr<xGateJsonMsg> uptJMsg);
			std::unique_ptr<xGateJsonMsg> Decode(std::string sJson);
		private:
			bool CreateDocObject(std::string sJson,Document &doc);
			std::unique_ptr<xGateJsonMsg> ReadRegResMsg(std::string sJson,Document &doc);
			std::unique_ptr<xGateJsonMsg> ReadRegResErrorMsg(std::string sJson,Document &doc);
			std::unique_ptr<xGateJsonMsg> ReadInviteResMsg(std::string sJson,Document &doc);
			std::unique_ptr<xGateJsonMsg> ReadInviteResErrorMsg(std::string sJson, Document &doc);
			std::unique_ptr<xGateJsonMsg> ReadInviteInPstnResMsg(std::string sJson,Document &doc);
			std::unique_ptr<xGateJsonMsg> ReadInviteInPstnResErrorMsg(std::string sJson,Document &doc);
			std::unique_ptr<xGateJsonMsg> ReadInviteMSTeamsResMsg(std::string sJson,Document &doc);
			std::unique_ptr<xGateJsonMsg> ReadAddTrunkIPToWhiteListMsg(std::string sJson,Document &doc);
			std::unique_ptr<xGateJsonMsg> ReadRemoveTrunkIPFromWhiteListMsg(std::string sJson,Document &doc);
			std::unique_ptr<xGateJsonMsg> ReadAddMsTeamsDomainToWhiteListMsg(std::string sJson,Document &doc);
			std::unique_ptr<xGateJsonMsg> ReadRemoveMsTeamsDomainFromWhiteListMsg(std::string sJson,Document &doc);
			std::string ConvertJsonObjToString(Document &doc,std::string value);
};
