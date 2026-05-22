#ifndef XGATE_CALL_INTERNAL_OBJECT
#define XGATE_CALL_INTERNAL_OBJECT
#include "xGateCall.h"
class xGateInternalCall: public xGateCall{
public :
xGateInternalCall();
xGateInternalCall(const callInfoParamater &cp);
~xGateInternalCall();
};
#endif
