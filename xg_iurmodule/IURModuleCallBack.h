#include "IURModuleMsg.h"

class IURModuleCallBack
{
public:
	IURModuleCallBack();
	virtual ~IURModuleCallBack();
        virtual bool handleModuleCallbackMsg(IURModuleMsg * ) = 0;
};


