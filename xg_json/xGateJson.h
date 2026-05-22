#pragma once

//c++ includes
#include <memory>

//local includes
#include "xGateJson.h"
#include "xGateJsonMsg.h"

class xGateJson
{
	public:
		xGateJson()=default;
		~xGateJson()=default;

		virtual std::string Encode(std::unique_ptr<xGateJsonMsg> uptJMsg) = 0;
		virtual std::unique_ptr<xGateJsonMsg> Decode(std::string sJson) = 0;
};