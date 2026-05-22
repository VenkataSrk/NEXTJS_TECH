#pragma once

//local includes
#include "xGateRapidJsonAdaptor.h"


class xGateJsonMgr
{
	public:
		xGateJsonMgr()=default;  
	    ~xGateJsonMgr()=default;

		static std::unique_ptr<xGateJson> GenerateJsonObj()
		{
			std::unique_ptr<xGateJson> uptJson = std::make_unique<xGateRapidJsonAdaptor>();
			return std::move(uptJson);
		}
};
