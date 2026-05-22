#pragma once

#include "IURDefines.h"

class IURModuleMsg
{
public:
        IURModuleMsg();
        virtual ~IURModuleMsg();
        virtual void setSrcModuleId(IURDefines::MODULE_ID moduleId) = 0;
        virtual void setDstModuleId(IURDefines::MODULE_ID moduleId) = 0;
        virtual IURDefines::MODULE_ID getSrcModuleId() const = 0;
        virtual IURDefines::MODULE_ID getDstModuleId() const = 0;
};

