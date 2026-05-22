#ifndef __IURMODULE__H__
#define __IURMODULE__H__

#pragma once
#include <string>
#include "IURModuleMsg.h"
#include "IURModuleConfig.h"
#include "IURDefines.h"

class IURModule
{
public:
	IURModule();
	 ~IURModule();
        virtual int initModule(IURModuleConfig *) = 0;
        virtual int unitModule() = 0;
        virtual int pushModuleMsg(IURModuleMsg *) = 0;
        virtual void getModuleName(std::string& moduleName) = 0;
        virtual IURDefines::MODULE_ID getModuleID() = 0;

};
#endif
