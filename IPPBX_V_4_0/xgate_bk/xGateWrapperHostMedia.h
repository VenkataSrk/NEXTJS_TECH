#include "hostmedia.h"
#define THISMODULE      "xGateWrapperHostMedia"

// xGateWrapper structure that will be used between ippbx and gstreamer threads
typedef struct xGateWrapper {
	HostGSTRtpBin* _hostGSTRtpBin;

	unsigned short _port;
	char* _RtpChanName;
	bool _dtmfEvent;

	int _rtptimeout;
	int _codec;
	int _ptime;
};

class xGateWrapperHostMedia
{
private:
	xGateWrapper _structWrapperHM;

public:
	// open the socket for port allocation, return 0 if failed
    	bool Open(unsigned short port, char* RtpChanName, bool dtmfEvent);

	// activate socket transmission/receiption based on RTP mode
    	bool Activate(const char* targetHost, int codec, int ptime);

	// release socket
	void Close();

	xGateWrapperHostMedia(const char* RtpChanName, bool dtmfEvent);
	~xGateWrapperHostMedia() {delete _structWrapperHM._hostGSTRtpBin;}
};
