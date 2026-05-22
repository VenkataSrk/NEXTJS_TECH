/*----------------------------------------------------------------* 
 * $Archive: /xGate/h248media.h $
 *
 * Description:
 * This module contains H248 media structure
 *----------------------------------------------------------------*/

#ifndef H248MEDIA_H
#define H248MEDIA_H

extern "C"
{
#include <stdlib.h>
#include <stdio.h>
#include <gst/gst.h>
#include <glib.h>
#include <gio/gio.h>
}

#include "rtpsig.h"
#include "rtpmedia.h"
#include "rtpchan.h"

#ifndef GSTREAMER_TEST_MODE
#include "voicemanager.h"
#include "voicedevice.h"
#include "rtpchan.h"
#endif

#include <string>
#include <stdio.h>
#include <map>
#include <queue>
#include "Switching.h"

#ifndef String
#define String std::string
#endif

#define FALSE 0
#define TRUE 1

// class represents a switch address for transmitting HostSwitchDevice.
// Each HostSwitchDevice in transmitting side own this object. All HostSwitchDevice
// in listening side will listen to transmitting HostSwitchDevice via this object.
// In gstreamer implementation, this switch address is actually a tee with srcpad number
// is same as listening HostSwitchDevice instances
class RemoteSwitchAddress: public SwitchAddress
{
  public: 
    RemoteSwitchAddress(String &swName):SwitchAddress(SF_Remote),itfName(swName){};
    //String &GetHostGSTBin()const {return itfName;};
    virtual const char* ToString() {return itfName.c_str();};
  private:
    String itfName;
};

// class represents one switch device of gstreamer.
// RTP_Media and HostVoiceDevice(as descendant of media) will have an instance
// of this switch device. In low level gstreamer layer, HostSwitchDevice
// owns an instance of HostGSTBin 
class RemoteSwitchDevice: public SwitchDevice
{
  public:
    /** AculabSwitch's constructor/destructor
    */
    RemoteSwitchDevice(String &name);
    virtual ~RemoteSwitchDevice();

    /** The framework will query the device to find out which 
      switching fabric is supported by this device to exchange TDM
      data with another device.
      */
    virtual SwitchFabric GetCompatibleFabric(SwitchDevice *dev);

    /** This function is called when the framework decides that it is
      the first time the specific device transmits to a particular
      fabric.
      */
    virtual SwitchAddress *AllocateAddress(SwitchFabric fabric);

    /** This function is called when the framework detects that no device
      is currently listening to this device.
      */
    virtual void FreeAddress(SwitchFabric fabric);

    /** It returns the address used for switching in the specified fabric
    */
    virtual SwitchAddress *GetAddress(SwitchFabric fabric);

    /** This device support Aculab switching fabric if another device
      (which must be Aculab) is located on the same board
      */
    virtual bool SupportAculab(SwitchDevice *requester);

    /** This device supports CTbus (H.100/H.110)
    */
    virtual bool SupportCTbus(SwitchDevice *requester);

    /** This device supports SCbus
    */
    virtual bool SupportSCbus(SwitchDevice *requester);

    /** It instructs the device to start transmitting to a certain location 
      using the specified switch fabric.
      */
    virtual void DeviceStartTransmit(SwitchFabric fabric,SwitchAddress *addr);

    /** It instructs the device to start listening from a certain location 
      using the specified switch fabric.
      */
    virtual void DeviceStartReceive(SwitchFabric fabric,SwitchAddress *addr);

    /** It instructs the device to stop transmitting to a certain location 
      using the specified switch fabric. It is called when the reference
      count of the specified switch fabric reach 0.
      */
    virtual void DeviceStopReceive(SwitchFabric fabric,SwitchAddress *addr);

    /** It instructs the device to stop listening from a certain location 
      using the specified switch fabric.
      */
    virtual void DeviceStopTransmit(SwitchFabric fabric,SwitchAddress *addr);
    virtual bool StartListen(SwitchDevice *source);
    virtual void StopListen(SwitchDevice *source);

  private:

    RemoteSwitchAddress * _addrHostBus;
};


// Xgate framework derivation for RTP Channel
class Remote_Rtp_Channel: public RTP_Channel
{

  /** Determine the type of the current rtp board type
   **/
  virtual RTP_Board_Type GetBoardType() const
  {
    return RTP_Board_Host;
    //return RTP_Board_Virtual;
  }

  /** Requiered for AudioCode interface event. 
    This will be discarded if not necessary.
   **/
  virtual void SetEventCallback(RTP_Channel_Callback*)
  {
    // not applicable
  }


  /** Set/get payload code, supported to provided backward 
    compatibility with current RTP_Media	
   **/
  virtual void SetCodec(Rtp_Payload_Code code)
  {
    //CLog::Info(THISMODULE, "[%s] SetCodec()", _channel_name);
    CLog::Warning("hostmedia", "[%s] Attempt to change codec with SetCodec() %d", _channel_name, code);
    _config.RTP.Codec = code;
  }


  /** Set/get payload code, supported to provided backward 
    compatibility with current RTP_Media	
   **/
  virtual Rtp_Payload_Code GetCodec()
  {
    //CLog::Info(THISMODULE, "[%s] GetCodec()", _channel_name);
    return _config.RTP.Codec;
  }


  /** Open the rtp channel. Some code on the RTP_Media::Open() that 
    depends on AudioCode frame work extracted to RTP_Channel decendant.
Notes:
From this state the channel ready to be activated
For Audiocodes, it sets certain param and switch device props
For Mapletree, it sets certain param and initialize the port
*/
  virtual bool Open(RTP_Config& config);


  /** Activate the rtp channel. Some code on the RTP_Media that 
    depends on AudioCode frame work extracted to RTP_Channel descendant.
    */
  virtual bool Activate(const ACE_INET_Addr& addr, Rtp_Payload_Code codec, bool low, int ptime);


  /** Create the switch device for the channel. 
    This is extracted from RTP_Media to remove the dependency to AudioCode.
    */
  virtual SwitchDevice *GetSwitchDevice(const char* name)
  {
    return _swdev;
  }


  /** Close the channel. This is extracted from RTP_Media. 
    The code will be placed in the RTP_Channel dependant.
    */
  virtual void Close();


  /** Compatibility with AcChannel.	
    Initially, the channel should return zero.
    After Open() is called and physical channel is allocated, the channel
    should return 1.
    After Activate() is called and bidirectional communication is
    active, the channel should return 2.
    */
  virtual int GetInitializationStatus() const
  {
    return _init_state;
  }


  /** Compatibility with AcChannel.
  */
  virtual time_t GetLastActivateTime()
  {
    // not applicable
    return 0;
  }


  /** Compatibility with AcChannel.
  */
  virtual void DialDtmf(const char*)
  {
    // do nothing
  }


  /** Release Rtp Channel. Code extracted to RTP_Media decendants 
    to remove dependency.
    */
  virtual void ReleaseChannel(CallDirection, CallParameter*, CallCause)
  {
    //CLog::Info(THISMODULE, "[%s] ReleaseChannel()", _channel_name);
    //printf("\n Remote_Rtp_Channel ReleaseChannel %u \n",pthread_self());
    delete this; 
  }


  /** Compatibility with AcChannel.
  */
  virtual const sockaddr_in& GetLocalAddress()
  {
    //return *_local_addr.getsockaddr_in(); //Yoga commented this part and modified with below code
    const sockaddr_in *addr_in = (sockaddr_in *)_local_addr.get_addr();
    return *addr_in;
  }


  /** Compatibility with AcChannel.
  */
  virtual const void GetLocalAddress(ACE_INET_Addr& addr)
  {
    addr = _local_addr;
  }

  /** Compatibility with AcChannel.
  */
  virtual Rtp_Payload_Code GetSelectedPayload()
  {
    return _config.RTP.Codec;
  }


  /** Extracted method from RTP_Media to prepare switching device.
  */
  virtual bool PrepareSwitching(RTP_Config*)
  {
    // not applicable
    return 0;
  }


  /** Get the channel number (only need to be unique in the same board)
  */
  virtual int GetChannelNumber()
  {
    return _local_addr.get_port_number();
  }



  /** Provided to support backward compatibility with AcChannel
  */
  virtual const AcRtcpStat& GetRtcpStat()
  {
    return _stat;
  }


  /** Get/Set interface to change the config parameteres
    the behaviour of this method is vary. AudioCodes implmentation
    might change some value instantly while Mapletree might
    only commit the changes when Activated.
    */
  virtual const RTP_Config& GetRtpConfig()
  {
    return _config;
  }


  /** Get/Set interface to change the config parameteres
    the behaviour of this method is vary. AudioCodes implmentation
    might change some value instantly while Mapletree might
    only commit the changes when Activated.
    */
  virtual void SetRtpConfig(RTP_Config& config)
  {
    //CLog::Info(THISMODULE, "[%s] SetRtpConfig()", _channel_name);
    _config = config;
  }

  // enable/disable specific direction
  virtual void enableTransmission(bool isRtpRxEnable, bool isRtpTxEnable);


  // member variables
  RTP_Config _config;
  ACE_INET_Addr _local_addr;
  int  codec;
  int _init_state;
  char _itf_name[64];
  char _channel_name[64];
  AcRtcpStat  _stat;
  RemoteSwitchDevice *_swdev;
  public:

  /** Host_Rtp_Channel constructor
   *     **/
  Remote_Rtp_Channel(const char* itf_name);

  /** Provided to support backward compatibility with AcChannel
  */
  virtual int GetMediaID()
  {
    return _local_addr.get_port_number();
  }


  /** Class destructor
   **/
  ~Remote_Rtp_Channel();

};


class H248Media : public Media
{
  public:

    H248Media(const char *name,
        MediaId id,
        Rtp_Interface *itf,
        Interface *phy_itf,
        RTP_Channel *channel);

    void   ReleaseChannel(CallDirection dir,CallParameter *cparam,CallCause cc);
    void   GetLocalAddress(ACE_INET_Addr & addr);
    void   GetLocalCodec(Rtp_Payload_Code &codec);
    void   SetLocalAddress(ACE_INET_Addr &addr, Rtp_Payload_Code codec);
    size_t GetSupportedPayload(Rtp_Payload_Code payloads[], size_t maxnum);

    /** H248Media's destructor
     * ***/
    virtual ~H248Media() {
      //printf("\n H248Media DTR\n");
    }

    RTP_Channel *GetChannel() {
      return acChannel;
    }

    /** Initialize media channel.
     *  If the remote_rtp parameter is not zero and payload is valid, then
     *  this will start RTP transmitter/receiver using the specified codec to the
     *  specified destination.
     *  */
    bool   Open();

    /** Start the RTP transmitter/receiver using the specified codec to the
     *  specified destination.
     *  */
    bool   Activate(const ACE_INET_Addr & remote_rtp,
        Rtp_Payload_Code payload);

    /** Close the channel.
     *  */
    bool   Close();



  private:

    void SetPeer(H248Media *peer) {
      _peer = peer;
    }

    H248Media           *_peer;
    bool                _useRAP;
    ACE_INET_Addr _local_ip_addr;
    int  localCodec;
    Rtp_Interface  *rtpItf;
    RTP_Channel *acChannel;
    Interface *phyInterface;
    void      *itfSource;
    Rtp_Payload_Code payloadCurrent;
    int     mState;
};


#endif 
