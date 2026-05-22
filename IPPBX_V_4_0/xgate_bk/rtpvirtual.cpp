/* $Header: /xGate/rtpvirtual.cpp 2     3/16/04 10:46a Bennylp $
 *   
 * XGATE
 * (C) 2001 Switchlab, Ltd
 *
 * RTP virtual channel.
 *
 * $Log: /xGate/rtpvirtual.cpp $
 * 
 * 2     3/16/04 10:46a Bennylp
 * Removed nasty CLog::Info (used for debugging)
 * 
 * 1     3/08/04 7:11p Bennylp
 * Initial version
 */
#include "stdafx.h"
#include "rtpvirtual.h"
#include "CTbusMgr.h"
#include "Switching.h"
#include "log.h"
//#include <winsock2.h>

#define THISMODULE  "vrtp"

///////////////////////////////////////////////////////////////////////////////

class RTP_Virtual_Switch_Device : public SwitchDevice
{
public:
    RTP_Virtual_Switch_Device(const char *name);
    ~RTP_Virtual_Switch_Device();

    virtual SwitchFabric GetCompatibleFabric(SwitchDevice* dev);
    virtual bool SupportAculab(SwitchDevice* requester);
    virtual bool SupportCTbus(SwitchDevice* requester);
    virtual bool SupportSCbus(SwitchDevice* requester);
    virtual SwitchAddress* AllocateAddress(SwitchFabric fabric);
    virtual void FreeAddress(SwitchFabric fabric);
    virtual SwitchAddress* GetAddress(SwitchFabric fabric);
    virtual void DeviceStartTransmit(SwitchFabric fabric,SwitchAddress* addr);
    virtual void DeviceStartReceive(SwitchFabric fabric,SwitchAddress* addr);
    virtual void DeviceStopTransmit(SwitchFabric fabric,SwitchAddress* addr);
    virtual void DeviceStopReceive(SwitchFabric fabric,SwitchAddress* addr);

private:
    CTbusAddress addr_;
};

RTP_Virtual_Switch_Device::RTP_Virtual_Switch_Device(const char *name)
: SwitchDevice(name), addr_(33, 0)
{
}

RTP_Virtual_Switch_Device::~RTP_Virtual_Switch_Device()
{
    CLog::Debug(THISMODULE, "SwitchDevice deleted");
}

SwitchFabric RTP_Virtual_Switch_Device::GetCompatibleFabric(SwitchDevice* dev)
{
    return SF_CTbus;
}

bool RTP_Virtual_Switch_Device::SupportAculab(SwitchDevice*)
{
    return false;
}

bool RTP_Virtual_Switch_Device::SupportCTbus(SwitchDevice*)
{
    return true;
}

bool RTP_Virtual_Switch_Device::SupportSCbus(SwitchDevice*)
{
    return false;
}

SwitchAddress* RTP_Virtual_Switch_Device::AllocateAddress(SwitchFabric fabric)
{
    return &addr_;
}

void RTP_Virtual_Switch_Device::FreeAddress(SwitchFabric fabric)
{
}

SwitchAddress* RTP_Virtual_Switch_Device::GetAddress(SwitchFabric fabric)
{
    return &addr_;
}

void RTP_Virtual_Switch_Device::DeviceStartTransmit(SwitchFabric,SwitchAddress*)
{
}

void RTP_Virtual_Switch_Device::DeviceStartReceive(SwitchFabric,SwitchAddress*)
{
}

void RTP_Virtual_Switch_Device::DeviceStopTransmit(SwitchFabric,SwitchAddress*)
{
}

void RTP_Virtual_Switch_Device::DeviceStopReceive(SwitchFabric,SwitchAddress*)
{
}

///////////////////////////////////////////////////////////////////////////////
static ACE_INET_Addr resolve_local_addr()
{
    static ACE_INET_Addr local_addr;

    if (local_addr.get_ip_address() != 0)
	return local_addr;

    char hostname[128];
    memset(hostname,0,sizeof(hostname));
    gethostname(hostname ,128);

    hostent *he = gethostbyname(hostname);
    if ( he )
        local_addr.set_addr(he->h_addr, 4);

    return local_addr;
}

///////////////////////////////////////////////////////////////////////////////
RTP_Virtual_Channel::RTP_Virtual_Channel()
: status_(0), activate_time_(0)
{
    config_.RTP.Codec = RTP_PT_PCMU;
    local_addr_ = resolve_local_addr();
}

RTP_Virtual_Channel::~RTP_Virtual_Channel()
{
    CLog::Debug(THISMODULE, "RTP Virtual Channel deleted");
}

RTP_Board_Type RTP_Virtual_Channel::GetBoardType() const
{
    return RTP_Board_Virtual;
}

void RTP_Virtual_Channel::SetEventCallback(RTP_Channel_Callback * /*callback*/)
{
}

void RTP_Virtual_Channel::SetCodec(Rtp_Payload_Code code)
{
    config_.RTP.Codec = code;
}

Rtp_Payload_Code RTP_Virtual_Channel::GetCodec()
{
    return config_.RTP.Codec;
}

bool RTP_Virtual_Channel::Open(RTP_Config& config)
{
    config_ = config;
    status_ = 1;
    return true;
}

bool RTP_Virtual_Channel::Activate(const ACE_INET_Addr& addr, 
				   Rtp_Payload_Code codec, 
				   bool low)
{
    status_ = 2;
    activate_time_ = time(NULL);
    return true;
}

SwitchDevice *RTP_Virtual_Channel::GetSwitchDevice(const char* name)
{
    return new RTP_Virtual_Switch_Device(name);
}

void RTP_Virtual_Channel::Close()
{
    status_ = 0;
}

int RTP_Virtual_Channel::GetInitializationStatus() const
{
    return status_;
}

time_t RTP_Virtual_Channel::GetLastActivateTime()
{
    return activate_time_;
}

void RTP_Virtual_Channel::DialDtmf(const char* dtmf)
{
}

void RTP_Virtual_Channel::ReleaseChannel(CallDirection cd, 
					 CallParameter* cp, 
					 CallCause cc)
{
    status_ = 0;
}

const sockaddr_in& RTP_Virtual_Channel::GetLocalAddress()
{
    //return *local_addr_.getsockaddr_in();
    const sockaddr_in *addr_in = (sockaddr_in *)local_addr_.get_addr();
    return *addr_in;

}

const void RTP_Virtual_Channel::GetLocalAddress(ACE_INET_Addr& addr)
{
    addr = local_addr_;
}

Rtp_Payload_Code RTP_Virtual_Channel::GetSelectedPayload()
{
    return config_.RTP.Codec;
}

bool RTP_Virtual_Channel::PrepareSwitching(RTP_Config* conf)
{
    return true;
}

int RTP_Virtual_Channel::GetMediaID()
{
    return 1;
}

const AcRtcpStat& RTP_Virtual_Channel::GetRtcpStat()
{
    static AcRtcpStat stat;
    return stat;
}

const RTP_Config& RTP_Virtual_Channel::GetRtpConfig()
{
    return config_;
}

void RTP_Virtual_Channel::SetRtpConfig(RTP_Config& config)
{
    config_ = config;
}
