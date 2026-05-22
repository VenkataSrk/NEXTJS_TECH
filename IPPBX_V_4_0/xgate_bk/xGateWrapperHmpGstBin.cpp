#include "xGateWrapperHmpGstBin.h"

xGateWrapperHmpGstBin::xGateWrapperHmpGstBin(const char* RtpChanName, bool dtmfEvent)
{
	_structWrapperHM._hostGSTRtpBin = new HostGSTRtpBin(RtpChanName, dtmfEvent);
}

bool xGateWrapperHmpGstBin::Open(unsigned short port, char* RtpChanName, bool dtmfEvent)
{
  	CLog::Detail(THISMODULE, "xGateWrapperHmpGstBin::Open");

	_structWrapperHM._port = port;
	_structWrapperHM._RtpChanName = RtpChanName;
	_structWrapperHM._dtmfEvent = dtmfEvent;
	

	xGateWrapperHmpGstBin(RtpChanName, dtmfEvent);

	return ((_structWrapperHM._hostGSTRtpBin)->Open(port));
}

bool xGateWrapperHmpGstBin::Activate(const char* targetHost, int codec, int ptime)
{
  	CLog::Detail(THISMODULE, "xGateWrapperHmpGstBin::Activate");

	_structWrapperHM._codec = codec;
	_structWrapperHM._ptime = ptime;

	return ((_structWrapperHM._hostGSTRtpBin)->Activate(targetHost, codec, ptime));
}

void xGateWrapperHmpGstBin::Close()
{
  	CLog::Detail(THISMODULE, "xGateWrapperHmpGstBin::Close");

	(_structWrapperHM._hostGSTRtpBin)->Close();
}
