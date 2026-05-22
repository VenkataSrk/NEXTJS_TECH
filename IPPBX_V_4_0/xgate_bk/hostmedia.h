/*----------------------------------------------------------------* 
 * $Archive: /xGate/prosody.cpp $

 * *****************  Version 1  *****************
 * User: okY      Date: 12/01/15   Time: 23:55
 * Created in $/xGate
 * First day of Prosody and voice driver intergration
 * 
 *----------------------------------------------------------------*/

/*----------------------------------------------------------------* 
 * Description:
 * This module contains gstreamer-based host media processing
 *----------------------------------------------------------------*/


#ifndef HOSTMEDIA_H
#define HOSTMEDIA_H

extern "C"
{
#include <stdlib.h>
#include <stdio.h>
#include <gst/gst.h>
#include <fcntl.h>
#include <glib.h>
#include <gio/gio.h>
#include <gst/net/gstnet.h>
}


#ifndef GSTREAMER_TEST_MODE
#include "voicemanager.h"
#include "voicedevice.h"
#include "rtpchan.h"
#endif

#include <string>
#include <stdio.h>
#include <map>
#include <queue>

#ifndef String
#define String std::string
#endif

int b64_encode(unsigned char *in, int ilen, unsigned char *out, int olen);
int b64_decode(char *in, char *out, int olen);

#define FALSE 0
#define TRUE 1
#define HOSTGST_BUS_PORT_START   20000
#define HOSTGST_BUS_PORT_END    30000
//#define HOSTGST_BUS_GET_PORT() (HOSTGST_BUS_PORT_START+(rand()%HOSTGST_BUS_PORT_SPAN))
int gst_bus_port=HOSTGST_BUS_PORT_START;
#define HOSTGST_BUS_GET_PORT() ((gst_bus_port >= HOSTGST_BUS_PORT_END)?(gst_bus_port=HOSTGST_BUS_PORT_START):(gst_bus_port+=2))


#define RTP_PROTO 256
#define SRTP_PROTO 257
#define DTLS_SRTP_PROTO 259

#define HOSTGST_RTP_PORT_START   10000
//#define HOSTGST_RTP_PORT_SPAN    10000
#define HOSTGST_RTP_PORT_END    20000
//#define HOSTGST_RTP_GET_PORT() ((HOSTGST_RTP_PORT_START+(rand()%HOSTGST_RTP_PORT_SPAN))&0xFFFE)
int gst_rtp_port=HOSTGST_RTP_PORT_START;

#define HOSTGST_RTP_GET_PORT() ((gst_rtp_port >= HOSTGST_RTP_PORT_END)?(gst_rtp_port=HOSTGST_RTP_PORT_START):(gst_rtp_port+=2))

#define AES_CM_128     128
#define AES_128_ICM    129
#define HMAC_SHA1_80   80
#define HMAC_SHA1_32   32

class HostGSTBin;
typedef short HostBusId;
typedef std::map <HostGSTBin*, HostBusId> HostGSTListenerTable;
typedef HostGSTListenerTable::iterator HostGSTListenerIterator;

class HostGSTCnBin;

class HostGSTConfBin;

class HmpConfRoom;

// low level implementation of one Gstreamer media
class HostGSTBin
{
  public:
    HostGSTBin(const char* name, bool dtmfEvent);
    HostGSTBin(const char* name, bool dtmfEvent, int rtp_timeout);
    virtual ~HostGSTBin();

    int allocate_socket();
    int check_port_available(int port);

    // the main interface between HostGSTBin instances
    // to connect each other(always transmitter side)
    bool add_listener(HostGSTBin* recvBin);
    bool remove_listener(HostGSTBin* recvBin);
    void clear_listener();

    const char* get_name() const{return _binName.c_str();};
    std::string get_room_name() {return _room_name;};
    bool isValid()const{return magic==0xBEEFBABE;}

    virtual void dial_dtmf(char digit, gint volume){};

    //static method to connect with this instance's pipeline bus
    static gboolean  handle_event_dispatcher(GstBus* bus, GstMessage* msg, gpointer data);
    static void on_pad_added_dispatcher(GstElement *element, GstPad *pad, gpointer data);
    static GstCaps* on_request_pt_map_dispatcher(GstElement *rtpbin, guint session, guint pt, gpointer data);
    static gboolean sink_event_dispatcher(GstPad *pad, GstObject *parent, GstEvent *event, gpointer userData);

    // To show bus errorr
    void bus_msg(GstBus * bus, GstMessage * msg, gpointer pipe);

    // add_listener() and remove_listener will use this function
    HostBusId get_bus_id()const{return _id;};

    int bus_watch_id1;
    int bus_watch_id2; 
    bool _dtmfIsEvent;
    GstClock *  _clock;
    std::string _room_name;

  protected:
    // use by descendant to build full functional module
    GstBin* get_send_terminal_bin()const{return _sendTerminalBin;};
    GstBin* get_receive_terminal_bin()const{return _receiveTerminalBin;};
    GSocket* get_terminal_socket()const{return _termSocket;};
    GSocket* get_record_socket()const{return _recordSocket;};
    void set_record_socket(GSocket* recordSocket){_recordSocket=recordSocket;};
    GstElement* get_send_pipeline()const{return _sendPipeline;};
    GstElement* get_receive_pipeline()const{return _receivePipeline;};

    GstElement* get_rbt_pipeline()const{return _rbtPipeline;};
    GstElement* get_conf_pipeline()const{return _confPipeline;};
    void set_conf_pipeline(GstElement* confPipeline){_confPipeline = confPipeline;};
    GstElement* get_record_pipeline()const{return _recordPipeline;}
    GstClock * get_clock()const{return _clock;};

    void indicate_dtmf(const char digit, gint volume);
    void activate_comfort_noise();
    void stop_comfort_noise();
    GstElement* get_send_sink_element()const{return _multiUdpSink;};
    guint64 _rtptout;

  private:
    unsigned int magic;
    // instance's event handler. will be called via handle_event_dispatcher
    virtual gboolean handle_event(GstBus *bus, GstMessage *msg){return TRUE;};
    virtual void on_pad_added(GstElement *element, GstPad *pad){};

    virtual GstCaps* on_request_pt_map(GstElement *rtpbin, guint session, guint pt){return NULL;};
    virtual gboolean sink_event_handler(GstPad *pad, GstObject *parent, GstEvent *event){};

    // apply listener table list to terminal bin UDP transmistter
    void apply_listener_table();
    // send/receiveTerminalBin will be available after constructor
    /// descendant can then connect it to its custom gstreamer design.
    // this element is also automatically added to _pipeline
    GstBin* _sendTerminalBin;
    GstBin* _receiveTerminalBin;
    // udp sink to send internal bus stream
    GstElement* _multiUdpSink;
    GstElement* _multiUdpSinkrbt;
    GstElement* _multiUdpSinkCo;
    // socket for terminal bin
    GSocket* _termSocket;
    GSocket* _recordSocket;
    // Address for this bin
    HostBusId _id;
    // this bin name
    String _binName;
    // this bin pipelien, created in constructor(initially have only terminalBin element)
    GstElement* _sendPipeline;
    GstElement* _receivePipeline;
    GstElement* _rbtPipeline;
    GstElement* _recvBinPipeline;

    GstElement* _confPipeline;
    GstElement* _recordPipeline;

    GstElement* _rtpJitterBuffer_s;
    GstElement* _rtpJitterBuffer_r;


    guint64 _timeout;

    // listener table
    HostGSTListenerTable table;
    // backup comfort noise source
    HostGSTCnBin * _cnBin;
    //  int mediaProto;
};

typedef enum DialDtmfState
{
  DTMF_Idle,
  DTMF_Play,
  DTMF_Gap
}DialDtmfState;

//in ms
#define DIAL_DTMF_DURATION  50
#define DIAL_DTMF_GAP       1
typedef int DtmfDigitVolumePair;

// RTP variation of Gstreamer media
class HostGSTRtpBin: public HostGSTBin
{
  public:
    HostGSTRtpBin(const char* RtpChanName, bool dtmfEvent);		
    HostGSTRtpBin(const char* RtpChanName, bool dtmfEvent, int rtptimeout);		
    virtual ~HostGSTRtpBin();

    // open the socket for port allocation, return 0 if failed
    bool Open(unsigned short port);

    // get currently active port
    unsigned short GetLocalPort() const{return port;};

    GSocket * get_rtp_chansocket()const{ return chanSocket;};

    // activate socket transmission/receiption based on RTP mode
    bool Activate(const char* targetHost, int codec, int ptime);

    // stop socket transmission/receiption
    void Stop();
    // release socket
    void Close();

    bool PlayFileWav(char const *filename, bool autorepeat);

    bool StopFileWav(int waitTime);

    // dtmf setting(-1 means inband)
    void setDtmfMode(int _rx_2833, int _tx_2833){
      rx_2833=_rx_2833;
      tx_2833=_tx_2833;
    }

    //stats
    bool isActive()const{return activated;};
    bool isOpen()const{return (chanSocket!=NULL);};

    bool isRbtActive()const{return rbtActive;};

    // derived from HostGSTBin
    virtual void dial_dtmf(char digit, gint volume);

    void enableTransmission(bool isRtpRxEnable, bool isRtpTxEnable);
    static void starterFunc(HostGSTRtpBin* bin);
    static void starterIvrFunc(HostGSTRtpBin* bin);
    static void starterRecFunc(HostGSTRtpBin* bin);
    static void starterConfFunc(HostGSTRtpBin* bin);
    void SetDtmfDigit(int dtmfDigit);
    int  GetDtmfDigit(char* dtmf_digits);
    void SetDtmfDigitTimeout(int max_time);
    // bool RecordFile(const char *filePath, vector<GSocket*> socketList);
    bool RecordFile(const char*, HostGSTRtpBin*);
    void EndRecordFile();
    bool ConfAddParty(GstElement *confPipeline, GstElement *audioMixer, GstElement *mixerOut, const char * roomName);
    bool ConfRemoveParty(GstElement *confPipeline, GstElement *audioMixer, GstElement *mixerOut);
    bool ConfRecordFile(GstElement *confPipeline, GstElement *mixerOut, const char *filename);
    void ConfEndRecordFile(GstElement *confPipeline, GstElement *mixerOut);

    GstElement * get_rec_terminal_bin() { get_send_terminal_bin();}
    GstElement * get_conf_send_terminal_bin() { return confSendTermBin;};

    void listen_conf_call();
    void unlisten_conf_call();

    HostGSTConfBin* GetConfBinFromMap(char* room_name);
    bool AddConfBinInMap(char* room_name, HostGSTConfBin *conf_room);
    bool RemoveConfBinFromMap(char* room_name);
    void SetConfParam(const char *room_name, GstClock * clock);
    void *tmpCall;

  private:
    virtual gboolean handle_event(GstBus *bus, GstMessage *msg);
    virtual void on_pad_added(GstElement *element, GstPad *pad);
    virtual GstCaps* on_request_pt_map(GstElement *rtpbin, guint session, guint pt);
    virtual gboolean sink_event_handler(GstPad *pad, GstObject *parent, GstEvent *event);

    //DTMF cycle functions
    void dial_dtmf_start(DtmfDigitVolumePair);
    void dial_dtmf_stop();
    static gboolean dial_dtmf_play_end_cb(void* binPtr);
    static gboolean  dial_dtmf_gap_end_cb(void* binPtr);

    //receiver side
    GstElement * udpSrc;
    GstElement * _rtcp_src;
    GstElement * rtpBin;
    GstElement * depayloader;
    GstElement * decoder;
    GstElement * rtpDtmfDepay;
    GstElement * fakeSink;
    GstElement * dtmfSrc;
    GstElement * dtmfDetect;
    GstElement * selector;
    GstElement * vadSink;

    //sender side
    GstElement * payloader;
    GstElement * encoder;
    GstElement * rtpDtmfSrc;
    GstElement * dtmfMux;
    GstElement * sendRtpBin;
    GstElement * udpSink;
    GstElement* invertIn;
    GstElement* audioInvert;
    GstElement* invertMixer;
    GstElement* audioConvert;
    GstElement* confSendTermBin;

    GSocket * chanSocket;
    GSocket * _rtcp_socket; 
    GstClock *pclock;
    guint64 rtptout;

    gchar* m_audiomixer_sink_pad_name;
    gchar* m_mixerout_src_pad_name;
    GstPad* m_mixerout_rec_src_pad;
    gchar* m_filesrc_name;
    gchar* m_invertmixer_name;

    unsigned short port;
    short activePT;
    bool activated;
    bool rbtActive;
    bool rbtCreated;
    bool recActive;
    bool _isAddConf;
    bool _isConfCall;
    bool _autoRepeat;
    int rx_2833;
    int tx_2833;
    DialDtmfState dtmfState;

    std::queue <DtmfDigitVolumePair> dtmfQueue;
    DtmfDigitVolumePair currentPlayedDtmf;
    GThread * mediaThread;
    GThread * ivrThread;
    GThread * recThread;
    GThread * confThread;

    GstBus* tbus, *sbus, *rbus;
    int mProto;
    char destHost[200];
    char _rbtFile[200];
    string mDtmfDigitBuffer;
    double mDtmfDigitTimeout;
    int tbus_watch_id, sbus_watch_id, rbus_watch_id;
    gint64 mPtime;
};

// Rtp bin to generate conference mixer
class HostGSTConfBin: public HostGSTBin
{
  public:
    HostGSTConfBin(const char* RtpChanName, bool dtmfEvent);
    virtual ~HostGSTConfBin();

    GstElement* get_conf_pipeline()const{return _confPipeline;};
    GstElement* get_audio_mixer()const{return audioMixer;};
    GstElement* get_mixer_out()const{return mixerOut;};
    GstClock* get_global_clock()const{return global_clock;};
    bool get_conf_rec_active()const{return _confRecActive;};
    bool ConfCreateRoom();
    bool ConfRemoveRoom();

    GstElement* _confPipeline;
    GstElement* audioMixer;
    GstElement* mixerOut;

    bool _confActive;
    bool _confRecActive;
    GstClock *global_clock;
    int _confMember;
    GstBus* cbus;
    int cbus_watch_id;
};

std::map<string, HostGSTConfBin*> _hmp_conf_bin_map;


// Rtp bin to generate comfort noise
class HostGSTCnBin: public HostGSTBin
{
  public:
    HostGSTCnBin(const char* RtpChanName, bool dtmfEvent);     
    virtual ~HostGSTCnBin();
};


#ifndef GSTREAMER_TEST_MODE
// class represents a switch address for transmitting HostSwitchDevice.
// Each HostSwitchDevice in transmitting side own this object. All HostSwitchDevice
// in listening side will listen to transmitting HostSwitchDevice via this object.
// In gstreamer implementation, this switch address is actually a tee with srcpad number
// is same as listening HostSwitchDevice instances
class HostSwitchAddress: public SwitchAddress
{
  public:
    HostSwitchAddress(HostGSTBin* gstBin):SwitchAddress(SF_Host),_gstBin(gstBin){};
    HostGSTBin* GetHostGSTBin()const {return _gstBin;};
    virtual const char* ToString() {return _gstBin->get_name();};
  private:
    HostGSTBin* _gstBin;
};

// class represents one switch device of gstreamer.
// RTP_Media and HostVoiceDevice(as descendant of media) will have an instance
// of this switch device. In low level gstreamer layer, HostSwitchDevice
// owns an instance of HostGSTBin 
class HostSwitchDevice: public SwitchDevice
{
  public:
    /** AculabSwitch's constructor/destructor
    */
    HostSwitchDevice(const char *name, HostGSTBin* gstBin);
    virtual ~HostSwitchDevice();

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

  private:


    HostSwitchAddress * _addrHostBus;
    HostGSTBin * _gstBin;
};


// Xgate framework derivation for RTP Channel
class Host_Rtp_Channel: public RTP_Channel
{
  /** Determine the type of the current rtp board type
   **/
  virtual RTP_Board_Type GetBoardType() const
  {
    return RTP_Board_Host;
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

  virtual bool PlayFileWav(char const *filename, bool autorepeat);
  virtual bool PlayFileWav(char const *filename, bool autorepeat, void* call);
  virtual bool StopFileWav(int waitTime);
  virtual int GetDtmfDigit(char* dtmf_digits);
  virtual void SetDtmfDigitTimeout(int max_time);
  virtual bool RecordFile(const char *filePath, RTP_Channel *rtpchan2);
  virtual void EndRecordFile();
  virtual bool ConfCreateRoom(const char *room_name);
  virtual bool ConfAddParty(const char *room_name);
  virtual bool ConfRecordFile(const char *room_name, const char *filename);
  virtual void ConfEndRecordFile(const char *room_name);
  virtual bool AttachTask(void* call);


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
    delete this;
  }


  /** Compatibility with AcChannel.
  */
  virtual const sockaddr_in& GetLocalAddress()
  {
    const sockaddr_in *addr_in = (sockaddr_in *)_local_addr.get_addr();
    return *addr_in;
    //return *_local_addr.getsockaddr_in();
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


  /** Convert Prosody error code into meaningful message
   **/
  /*const char *errmsg(int rc)
    {
    MEMZERO(err);
    err.code = rc;
    return sm_error_name(&err);
    }*/

  // member variables
  RTP_Config _config;
  ACE_INET_Addr _local_addr;
  HostSwitchDevice* _swdev;
  int _init_state;
  char _itf_name[64];
  char _channel_name[64];
  AcRtcpStat	_stat;
  HostGSTRtpBin * _gstBin;
  //HostGSTConfBin * _gstConfBin;
  //HmpConfRoom *_hmpConf;
  public:

  /** Host_Rtp_Channel constructor
   **/
  Host_Rtp_Channel(const char* itf_name, bool dtmfEvent);
  Host_Rtp_Channel(const char* itf_name, bool dtmfEvent, int rtptimeout);

  /** Provided to support backward compatibility with AcChannel
  */
  virtual int GetMediaID()
  {
    return _local_addr.get_port_number();
  }

  /** Class destructor
   **/
  ~Host_Rtp_Channel();

};

typedef struct HostBusPair
{
  HostBusPair(GstPipeline* _pipeline, HostGSTBin* _bin):pipeline(_pipeline), bin(_bin){};
  GstPipeline* pipeline;
  HostGSTBin* bin;
}HostBusPair;

// manager for host media processing's voice driver
class HostMediaProcessingDriver: public ServiceProvider
{
  public:
    HostMediaProcessingDriver():ServiceProvider(
        "hostmediadriver",        // SP name
        FALSE,        // call AttachTask() for all new tasks?
        TRUE,       // automaticly polled? or manually via SPManager->RegisterPoll()
        Normal_Priority// polling priority/frequency
        ){};
    static HostMediaProcessingDriver * Create();
    static HostMediaProcessingDriver * Instance() {
      return instance;
    }

    /** This function will be called by the service framework after 
      instantiation of this class, to allow the instance to initialize
      itself.
      */
    virtual bool Load();

    /** This function will be called by the service framework once the 
      configuration file has been fully read.
      */
    virtual bool CommitConfiguration(){return TRUE;};

    /** This function will be called by the service framework before it is
      destroyed/deleted, to allow the instance to cleanup itself.
      */
    virtual bool Unload();

    /** This function will be called whenever a new task is created.
    */
    virtual bool AttachTask(CTask *) {
      return TRUE;
    }

    /** This function will be called whenever a task is about to be deleted.
    */
    virtual void DetachTask(CTask *) {
      return ;
    }

    /** This function will be called if this instance has a pending job for
      the task.
      */
    virtual bool CancelJob(CTask *) {
      return TRUE;
    }

    /** HandleEvents: Descendent shall implement this abstract method.
      It is called by Service Provider Manager
      */
    virtual void HandleEvents();

    /** OnShutdown: This method is called when the Kernel receives
      an instruction to shutdown. ServiceProvider can implement a specific
      behaviour that reflect this changes such as stop receiving further 
      calls, inform the network to stop sending further calls etc
      */
    virtual void OnShutdown(bool cancel) {
      // by default, do nothing
    }

    /** OnClearCounter is called by the framework to clear any statistic values
      maintained by this service provider.
      */
    virtual void OnClearCounter() {
      // by default, do nothing.
    }

    static GMainLoop * loop;
    //static GMainContext * context;
    static HostMediaProcessingDriver* instance;
    static std::queue<HostBusPair*> busToBeRegistered;
    static HostGSTCnBin * cnGeneratorBin;
};
#endif //GSTREAMER_TEST_MODE

#endif 
