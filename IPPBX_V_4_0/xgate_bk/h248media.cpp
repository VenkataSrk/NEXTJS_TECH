/*----------------------------------------------------------------* 
 * $Archive: /xGate/h248media.cpp $
 * Description:
 * This module contains H248 media structure, which is a place holder 
 * for H.248 Call processing to function.
 *----------------------------------------------------------------*/
#ifdef __linux__
#include "stdafx.h"
#endif

#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/time.h>

#include "xgconfig.h"
#include "rtpmedia.h"
#include "h248media.h"
#include "h248sig.h"
#include "rtpchan.h"

extern int debug;
#ifdef XGATE_HAS_H248
#define THISMODULE	"H248M"
#define MEMZERO(p) memset(&p,0,sizeof(p))

//////////////////////////////////////////////////////////////////////////
//
//  RemoteSwitchDevice
//
//////////////////////////////////////////////////////////////////////////
/** HostSwitchDevice's constructor
*/
  RemoteSwitchDevice::RemoteSwitchDevice(String &name)
: SwitchDevice(name.c_str()), _addrHostBus(new RemoteSwitchAddress(name))
{ 
  //SRI : printf("\n RemoteSwitchDevice CTR\n");
}

/** HostSwitchDevice's destructor
*/
RemoteSwitchDevice::~RemoteSwitchDevice()
{
  //SRI : printf("\n RemoteSwitchDevice DTR\n");
}

/** The framework will query the device to find out which 
  switching fabric is supported by this device to exchange TDM
  data with another device.
  */
SwitchFabric RemoteSwitchDevice::GetCompatibleFabric(SwitchDevice *dev)
{
  if(dynamic_cast<RemoteSwitchDevice*>(dev))
  {
    return SF_Remote;
  }
  else
  {
    return SF_Invalid;
  }
}

/** This function is called when the framework decides that it is
  the first time the specific device transmits to a particular
  fabric.
  */
SwitchAddress *RemoteSwitchDevice::AllocateAddress(SwitchFabric fabric) 
{
  switch (fabric) {
    case SF_Remote:
      return _addrHostBus;

    default:
      assert(false);
      return false;
  }
}

/** This function is called when the framework detects that no device
  is currently listening to this device.
  */
void RemoteSwitchDevice::FreeAddress(SwitchFabric fabric)
{
  switch (fabric) {
    case SF_Remote:
      break;

    default:
      assert(false);
  }
}

/** It returns the address used for switching in the specified fabric
*/
SwitchAddress *RemoteSwitchDevice::GetAddress(SwitchFabric fabric)
{
  switch (fabric) {
    case SF_Remote:
      return _addrHostBus;

    default:
      assert(false);
      return NULL;
  }
}

/** Host media doesnt support other switch fabric
*/
bool RemoteSwitchDevice::SupportAculab(SwitchDevice *requester)
{
  return false;
}

/** This device supports CTbus (H.100/H.110)
*/
bool RemoteSwitchDevice::SupportCTbus(SwitchDevice *requester)
{
  return false;
}

/** This device supports SCbus
*/
bool RemoteSwitchDevice::SupportSCbus(SwitchDevice *requester)
{
  return false;
}

/** It instructs the device to start transmitting to a certain location 
  using the specified switch fabric.
  */
void RemoteSwitchDevice::DeviceStartTransmit(SwitchFabric fabric,SwitchAddress *addr)
{
  if(fabric!=SF_Remote)
  {
    CLog::Warning(THISMODULE,"Cannot start transmit! expected SF_Remote only");
    return;
  }else if(addr!=_addrHostBus)
  {
    CLog::Warning(THISMODULE,"Invalid Switchaddress for start transmit to(addr!=_addrHostBus)");
    return;
  }
}

/** It instructs the device to start listening from a certain location 
  using the specified switch fabric.
  */
void RemoteSwitchDevice::DeviceStartReceive(SwitchFabric fabric,SwitchAddress *addr)
{
#ifndef __linux__
  RemoteSwitchAddress *hostAddr=dynamic_cast<RemoteSwitchAddress*>(addr);
#else
  RemoteSwitchAddress *hostAddr= (RemoteSwitchAddress*)(addr);
#endif
  if(hostAddr)
  {
  }else
  {
    const char* swname = addr ? addr->ToString() : "(null)";
    CLog::Warning(THISMODULE,"%s unable to receive from %s using invalid fabric %d",GetName(),swname,fabric);
    return;
  }
}

/** It instructs the device to stop listening from a certain location 
  using the specified switch fabric.
  */
void RemoteSwitchDevice::DeviceStopTransmit(SwitchFabric fabric,SwitchAddress *addr)
{
  // pretend it always success for HostSwitch
}

/** It instructs the device to stop transmitting to a certain location 
  using the specified switch fabric. It is called when the reference
  count of the specified switch fabric reach 0.
  */
void RemoteSwitchDevice::DeviceStopReceive(SwitchFabric fabric,SwitchAddress *addr)
{
}

bool RemoteSwitchDevice::StartListen(SwitchDevice *source)
{
  return true;
}

void RemoteSwitchDevice::StopListen(SwitchDevice *source)
{
}

/** Host_Rtp_Channel constructor(called just before RTP media is instantiated).
  This should also init the lower gstreamer level via HostSwitchDevice(created here)
 **/
//Remote_Rtp_Channel::Remote_Rtp_Channel(const char* itf_name, ACE_INET_Addr addr, Rtp_Payload_Code& codecIn)
Remote_Rtp_Channel::Remote_Rtp_Channel(const char* itf_name)
{
  char tmpStr[25]={0};
  String itf;
  MEMZERO(_config);
  MEMZERO(_stat);
  _init_state=0;

  CLog::Warning(THISMODULE,"Remote_Rtp_Channel name: %s\n",itf_name);
  sprintf(tmpStr,"%s", itf_name);

  itf += String(tmpStr);
  _swdev=new RemoteSwitchDevice(itf);
  strcpy(_channel_name,tmpStr);
  //printf("\n!!!! Srini: Remote_Rtp_Channel ThreadId : %u\n",pthread_self());
  // printf("\n Remote_Rtp_Channel CTR: %s\n",tmpStr);
}

Remote_Rtp_Channel::~Remote_Rtp_Channel()
{
  //printf("\n Remote_Rtp_Channel DTR threadid : %u\n",pthread_self());
  if(_swdev)  {delete _swdev;  
    _swdev = NULL;
  }
}


bool Remote_Rtp_Channel::Open(RTP_Config& config)
{
  CLog::Info(THISMODULE, "[%s] Open()", _channel_name);

  switch (_config.DTMF.Transport_Type)
  {
    case RTP_Config::DTMF_RFC2833:
    case RTP_Config::DTMF_Inband:
      break;

    default:
      CLog::Warning(THISMODULE, "[%s] Attempt to open with unsupported dtmf transfer mode %d", _channel_name, _config.DTMF.Transport_Type);
      break;
  }

  _config = config;
  _init_state = 1;

  return TRUE;
}

/** Activate the rtp channel. Some code on the RTP_Media that 
  depends on AudioCode frame work extracted to RTP_Channel descendant.
  */
bool Remote_Rtp_Channel::Activate(const ACE_INET_Addr& addr, Rtp_Payload_Code codec, bool low,int ptime)
{
  CLog::Info(THISMODULE, "[%s] Activate()", _channel_name);
  switch (codec)
  {
    case RTP_PT_PCMA:
    case RTP_PT_PCMU:
      break;

    default:
      CLog::Warning(THISMODULE, "[%s] Attempt to open with unsupported codec %d", _channel_name, codec);

  }
  char destAddr[200];
  snprintf(destAddr, sizeof(destAddr), "%s:%u", addr.get_host_addr(),addr.get_port_number());

  _init_state = 2;
  _config.RTP.Codec = codec;
  _local_addr = addr;
  return true;
}

/** Close the channel. This is extracted from RTP_Media. 
  The code will be placed in the RTP_Channel dependant.
  */
void Remote_Rtp_Channel::Close()
{
  // just pretending, actual socket closing is on destructor
  CLog::Info(THISMODULE, "[%s] Close()", _channel_name);
  _init_state = 0;
}

// enable/disable specific direction
void Remote_Rtp_Channel::enableTransmission(bool isRtpRxEnable, bool isRtpTxEnable)
{
}


void H248Media::SetLocalAddress(ACE_INET_Addr &addr, Rtp_Payload_Code codec)
{
  _local_ip_addr  = addr; 
  localCodec = codec; 
  CLog::Info(THISMODULE, "SetLocalAddress obj(%x) IpAddress: %u and port %u and codec : %d",this,addr.get_ip_address(), addr.get_port_number(),codec);

};

H248Media:: H248Media(const char *name,
    MediaId id,
    Rtp_Interface *itf,
    Interface *phy_itf,
    RTP_Channel *channel):
  Media(name, (MediaId)channel->GetMediaID(), itf, channel->GetSwitchDevice(name)), acChannel(channel) 
{
  CLog::Info(THISMODULE,"H248Media : Address %x\n",this); 
}

void H248Media::ReleaseChannel(CallDirection dir,CallParameter *cparam,CallCause cc) {
  acChannel->ReleaseChannel(dir, cparam, cc);
  acChannel = NULL;
}

void H248Media::GetLocalAddress(ACE_INET_Addr & addr)
{
  addr =_local_ip_addr;
}

void H248Media::GetLocalCodec(Rtp_Payload_Code &codec)
{
  codec = localCodec;
}

bool H248Media::Open()
{
  assert (acChannel && acChannel->GetInitializationStatus() < 2);

  Rtp_Interface *itf = (Rtp_Interface *)GetInterface();
  AcChannelParam param = itf->GetChannelParameter();

  param.GetParam().TransportSettings.UseNIorPCI = 1;
  param.GetParam().Active = 1;

  RTP_Config config = acChannel->GetRtpConfig();
  itf->GetRtpChannelConfig(config);
  acChannel->Open(config);
  return true;
}

bool   H248Media::Activate(const ACE_INET_Addr & remote_rtp,
    Rtp_Payload_Code payload)
{
  //printf("\n\n SRI : H248Media::Activate will activate the acChannel also\n\n");
  if (!acChannel) {
    assert(false);
    CLog::Fatal(THISMODULE, "Channel is not present in H248Media::Activate [%s]", this->GetName());
    return false;
  }

  Rtp_Interface *itf = (Rtp_Interface *)GetInterface();

  Rtp_Payload_Code payload_list[RTP_MAX_CODEC_LIST];
  unsigned ncodec = itf->GetSupportedPayload(payload_list, RTP_MAX_CODEC_LIST);
  if (ncodec==0 || std::find(payload_list, payload_list+ncodec, payload)==payload_list+ncodec) {
    CLog::Warning(THISMODULE, "Unsupported payload %d (remote RTP addr:%s)",
        payload, remote_rtp.get_host_addr());
    return false;
  }

  payloadCurrent = payload;
  acChannel->Activate(remote_rtp,payload, false);
  return true;
}




size_t H248Media::GetSupportedPayload(Rtp_Payload_Code payloads[], size_t maxnum)
{
  if (acChannel && acChannel->GetInitializationStatus() >= 2) {
    const RTP_Config& conf = acChannel->GetRtpConfig();
    payloads[0] = conf.RTP.Codec;
    if (payloads[0] != RTP_PT_INVALID)
      return 1;
    else
      assert(false);
  }
  Rtp_Interface *rtp_itf = (Rtp_Interface *)GetInterface();
  char rtp_itf_size[20];
  sprintf(rtp_itf_size,"%p",rtp_itf);
  if(rtp_itf && (strlen(rtp_itf_size) >= 9) && (strlen(rtp_itf_size) <= 10)){
    //assert(rtp_itf != 0);
    return rtp_itf->GetSupportedPayload(payloads, maxnum);
  }
  else
  {
    return 1;
  }
}
#endif
