
//local includes
#include "xGateMgcMgMedia.h"


extern int debug;
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
   XGLOG_WARN("Cannot start transmit! expected SF_Remote only");
    return;
  }else if(addr!=_addrHostBus)
  {
   XGLOG_WARN("Invalid Switchaddress for start transmit to(addr!=_addrHostBus)");
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
   XGLOG_WARN("%s unable to receive from %s using invalid fabric %d",GetName(),swname,fabric);
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

 XGLOG_WARN("Remote_Rtp_Channel name: %s\n",itf_name);
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
 XGLOG_INFO( "[%s] Open()", _channel_name);

  switch (_config.DTMF.Transport_Type)
  {
    case RTP_Config::DTMF_RFC2833:
    case RTP_Config::DTMF_Inband:
      break;

    default:
     XGLOG_WARN( "[%s] Attempt to open with unsupported dtmf transfer mode %d", _channel_name, _config.DTMF.Transport_Type);
      break;
  }

  _config = config;
  _init_state = 1;

  return TRUE;
}

/** Activate the rtp channel. Some code on the RTP_Media that 
  depends on AudioCode frame work extracted to RTP_Channel descendant.
  */
bool Remote_Rtp_Channel::Activate(const ACE_INET_Addr& addr, Rtp_Payload_Code codec, bool low)
{
 XGLOG_INFO( "[%s] Activate()", _channel_name);
  switch (codec)
  {
    case RTP_PT_PCMA:
    case RTP_PT_PCMU:
      break;

    default:
     XGLOG_WARN( "[%s] Attempt to open with unsupported codec %d", _channel_name, codec);

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
 XGLOG_INFO( "[%s] Close()", _channel_name);
  _init_state = 0;
}

// enable/disable specific direction
void Remote_Rtp_Channel::enableTransmission(bool isRtpRxEnable, bool isRtpTxEnable)
{
}


