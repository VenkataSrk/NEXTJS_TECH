#include "xGateWrapperHostMedia.h"

xGateWrapperHostMedia::xGateWrapperHostMedia(const char* RtpChanName, bool dtmfEvent)
{
	_structWrapperHM._hostGSTRtpBin = new HostGSTRtpBin(RtpChanName, dtmfEvent);
}

bool xGateWrapperHostMedia::Open(unsigned short port, char* RtpChanName, bool dtmfEvent)
{
  	CLog::Detail(THISMODULE, "xGateWrapperHostMedia::Open");

	_structWrapperHM._port = port;
	_structWrapperHM._RtpChanName = RtpChanName;
	_structWrapperHM._dtmfEvent = dtmfEvent;
	

	xGateWrapperHostMedia(RtpChanName, dtmfEvent);

	return ((_structWrapperHM._hostGSTRtpBin)->Open(port));
}

bool xGateWrapperHostMedia::Activate(const char* targetHost, int codec, int ptime)
{
  	CLog::Detail(THISMODULE, "xGateWrapperHostMedia::Activate");

	_structWrapperHM._codec = codec;
	_structWrapperHM._ptime = ptime;

	return ((_structWrapperHM._hostGSTRtpBin)->Activate(targetHost, codec, ptime));
}

void xGateWrapperHostMedia::Close()
{
  	CLog::Detail(THISMODULE, "xGateWrapperHostMedia::Close");

	(_structWrapperHM._hostGSTRtpBin)->Close();
}
