#pragma once

#include "IURDefines.h"
#include <ace/Message_Block.h>

class IURModuleMsg : public ACE_Message_Block
{
public:
        IURModuleMsg();
        virtual ~IURModuleMsg();
        virtual void setSrcModuleId(IURDefines::MODULE_ID moduleId) = 0;
        virtual void setDstModuleId(IURDefines::MODULE_ID moduleId) = 0;
        virtual IURDefines::MODULE_ID getSrcModuleId() const = 0;
        virtual IURDefines::MODULE_ID getDstModuleId() const = 0;
};

