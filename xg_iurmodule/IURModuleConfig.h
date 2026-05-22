#pragma once


#include "IURModuleCallBack.h"


class IURModuleConfig
{
public:
	IURModuleConfig();
	virtual ~IURModuleConfig();
	IURModuleCallBack* moduleCallBack;
public:
	virtual void setCallBack(IURModuleCallBack* ) = 0;
	virtual IURModuleCallBack* getCallBack() = 0;
};

