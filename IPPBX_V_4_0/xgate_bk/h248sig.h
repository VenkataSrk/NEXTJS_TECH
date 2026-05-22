/* $Header: /xGate/h248sig.h 
 *   
 * XGATE
 * (C) 2016 Mundio, Ltd
 * Initial coding.
 */
#ifndef __XGATE_H248SIG_H__
#define __XGATE_H248SIG_H__

#include "h248media.h"
#include "h248Callp.h"
#include "h248rls.h"
#include "Critical_Sectionfull.h"


#define EP_STATE_IDLE 1
#define EP_STATE_ACTIVE 2

#define GWY_STATE_IDLE 1
#define GWY_STATE_ACTIVE 2
#define GWY_STATE_MAINT  3
#define GWY_STATE_INACTIVE  4


#define ORIG_CALL 0
#define TERM_CALL 1

#define MAX_H248_CALL_CONTEXT 10000
#define START_H248_CALL_CONTEXT 1

#define CONTEXT_ID_START   1
#define CONTEXT_ID_END    20000000
static unsigned int sri_cntxId=1;
#define NEW_CONTEXT_ID() ((sri_cntxId>=CONTEXT_ID_END)?(sri_cntxId=CONTEXT_ID_START) : (sri_cntxId=sri_cntxId+1))


//SRI
//#define NEW_CONTEXT_ID(x)(START_H248_CALL_CONTEXT+(x++%MAX_H248_CALL_CONTEXT))
//SRI
/* Common Structure between MGC and MG */

typedef struct H248_MSG_HDR
{
  int msgType;
  unsigned int transId;
  int srcIpAddr;
  int srcPort;
  int dstIPAddr;
  int dstPort;
  int msgLen;
  int respHandle;
};

class InternalMsgHdr 
{
  public:
    static void InternalMsgHdr::prepareInternalMsgHdr(H248_MSG_HDR*, int, int, int);
    static void InternalMsgHdr::copyMsgHeader(H248_MSG_HDR *dHdr, H248_MSG_HDR *sHdr);
};


typedef  enum H248MsgTypes {
  LOCAL_MEDIA_ALLOCATE_REQUEST = 2,
  LOCAL_MEDIA_ALLOCATE_RESPONSE,
  MEDIAGATEWAY_MEDIA_CREATE,
  MEDIAGATEWAY_MEDIA_CREATE_RESPONSE,
  REMOTE_MEDIA_ALLOCATE_REQUEST,
  REMOTE_MEDIA_ALLOCATE_RESPONSE,
  MEDIA_LISTEN_REQUEST,
  MEDIA_LISTEN_RESPONSE,
  MEDIA_RELEASE_REQUEST,
  MEDIA_RELEASE_RESPONSE,
  GATEWAY_REGISTRATION_REQUEST,
  GATEWAY_REGISTRATION_RESPONSE,
  RESOURCE_RECORD_REQUEST,
  RESOURCE_RECORD_RESPONSE,
  KEEP_ALIVE_CHECK //SRI Added for Heart Beat
} H248_MSG_TYPES;

static char *h248MsgStr[] = {
  "LOCAL_MEDIA_ALLOCATE_REQUEST",
  "LOCAL_MEDIA_ALLOCATE_RESPONSE",
  "MEDIAGATEWAY_MEDIA_CREATE",
  "MEDIAGATEWAY_MEDIA_CREATE_RESPONSE",
  "REMOTE_MEDIA_ALLOCATE_REQUEST",
  "REMOTE_MEDIA_ALLOCATE_RESPONSE",
  "MEDIA_LISTEN_REQUEST",
  "MEDIA_LISTEN_RESPONSE",
  "MEDIA_RELEASE_REQUEST",
  "MEDIA_RELEASE_RESPONSE",
  "GATEWAY_REGISTRATION_REQUEST",
  "GATEWAY_REGISTRATION_RESPONSE",
  "RESOURCE_RECORD_REQUEST",
  "RESOURCE_RECORD_RESPONSE",
};

typedef struct GatewayRegistrationRequest
{
  H248_MSG_HDR msgHdr;
  char gwname[25];
  char gwProfile[25];
  int gwListenPort;
  int channels;
  int gwState;
  int rscCount;
  char rlsRecord[500];
  char padding_bits[2];//TO match the size of send and recv this is added 
};

#define REG_MSG_SIZE sizeof(struct GatewayRegistrationRequest)-500


typedef struct GatewayResourceRecord
{
  H248_MSG_HDR msgHdr;
  String rlsRecord;
};


typedef struct MediaAllocRequest
{
  H248_MSG_HDR msgHdr;
  ACE_INET_Addr rtpAddr;
  int codec; 
  SignallingType sigType;
  CallDirection dir;
  int  contextId;
  /* SwitchListen is merged into CalledParty Rtp Activate message, piggybacking
   * so no switchlisten message from MGC to MG explicitly */
  char origRtpEp[100];
  char termRtpEp[100];
  char padding_bits[94];//TO match the size of send and recv this is added
};


typedef struct MediaResponseMsg
{
  H248_MSG_HDR msgHdr;  
  int        result;
  int        localCodec;
  ACE_INET_Addr  rtpAddr;
  char  rtpEndpoint[100];
  char padding_bits[202];//TO match the size of send and recv this is added
};


typedef struct MediaReleaseRequest
{
  H248_MSG_HDR msgHdr;  
  int   contextId;
  char  rtpEndpoint[100];
  char padding_bits[222];//TO match the size of send and recv this is added
};

typedef struct MediaModifyRequest
{
  H248_MSG_HDR msgHdr;  
  char  sourceSwitching[50];
  char  targetSwitching[50];
};

typedef struct MediaReleaseResponse
{
  H248_MSG_HDR msgHdr;  
  int          result;
  char  rtpEndpoint[100];
  char padding_bits[222];//TO match the size of send and recv this is added
};



#ifdef H248_MG

typedef  Map<String, Rtp_Media *> GatewayMediaMap;
typedef  GatewayMediaMap::iterator GMItr;
typedef  Map<int , String> MGContextMap;
MGContextMap ContextMap;

class GatewayRegistrar  : public ServiceProvider
{
  public:
    GatewayRegistrar( String name ="MG", int listenPort=0, String profile="rtp0:prx00;rtp1:prx01");
    virtual ~GatewayRegistrar(){};
    static GatewayRegistrar *Instance(){return gwInstance;};
    static GatewayRegistrar *Create();

    virtual bool Load();
    virtual bool Initialize();
    virtual bool CommitConfiguration();
    virtual bool Unload();
    virtual void HandleEvents();
    virtual bool OnShowStatus(String & output);
    Rtp_Media * CreateRTPStream(ACE_INET_Addr &rtpAddr, int &codec, CallDirection dir,SignallingType sigType, int contextId, char *ep);
    bool CreateGatewayMedia(ACE_INET_Addr &rtpAddr, int &codec, CallDirection dir,SignallingType sigType, char *ep);
    bool DeleteRTPStream(Rtp_Media *media);
    bool StartListenToMedia(const char *target, const char *src);
    bool SendMediaResponse(void* reqMsg, ACE_INET_Addr& addr, int& port, int msgType, char *ep, int result);
    bool SendMediaResponse(void* reqMsg, int msgType, char *ep, int result);
    Rtp_Media *GetActiveMedia(char  *mediaName);
    bool StoreActiveMedia(char *mediaName, Rtp_Media *newMedia);
    bool RemoveActiveMedia(char *mediaName);

    CallParameter *CreateDefaultCallParam() {
      CallParameter *cparam;
      NEW_RETURN_(cparam, CallParameter);
      if (!cparam)
        return NULL;
      return cparam;
    }

    int   SendMessage(void *msg, int msgLen);
    bool  SendGatewayRegistration(int gwcAddr, int gwcPort );
    int   GetListenPort() { return gwyListenPort; }
    int   GetGwState() { return gwyState; }
    int   GetOwnIP() {  return gwyIP; }
    int   GetGwcIP()   {  return gwcAddr; }
    int   GetGwcPort() {  return gwcPort; }
    int   GetSocketId() {  return gwSocket; }
    int   GetChannelCount() {  return totalChan; }
    const String & GetGwName() {  return gwyName; }
    const String & GetGwProfile() {  return gwyProfile; }
    void  SetGwyAddress(int ipAddr ) { gwyIP = ipAddr; }
    void  SetGwcAddress(int ipAddr ) { gwcAddr = ipAddr; }
    void  SetGwyPort(int port) { gwyListenPort = port; }
    void  SetGwcPort(int port) { gwcPort = port; }
    void  DisplayGwyResourceVector();
    void  GetResourceRecordByteStreams(char *refString);
    GwRscVector GetResourceVector() { return gwyResource; }

  private:
    int _tlsId;
    static GatewayRegistrar *gwInstance;
    String gwyName;
    String gwyProfile;
    int    gwSocket;
    int    gwcAddr;
    int    gwcPort;
    int    gwyIP;
    int    totalChan;
    int    gwyListenPort;
    int    gwyState;
    GwRscVector gwyResource;
};

GatewayMediaMap MGMediaMap;

#endif


#ifdef H248_MGC
typedef std::list<MegacoTransport *> connList;
class GatewayControllerAPI ;
class H248CallAPI ;

class MGC  : public ServiceProvider 
{
  public : 
    bool Load();
    bool Unload(){}
    void HandleEvents() ;

    bool CheckForExistingConnections( int ipaddr, int port);
    bool AddNewConnection( MegacoTransport *newCon ,int readSocket);
    bool RemoveOldConnection( MegacoTransport *oldCon) ;
    static void gwcMsgHandler(void  *argIn );
    void gwcConnectionHandler(void  *argIn );
    static int AcceptNewConnection(int *ip, int *port);
    int GetSocketId(){ return gwcSocketId; }
    H248CallAPI *GetCallAPI(){ return _hCallAPI; };
    GatewayControllerAPI *GetControllerAPI(){ return _pAPI; };
    int Initialize();
    void SetOwnIP(int ipAddr) { ownIp =ipAddr;/* printf("GWC IP %d\n",ipAddr); */ }
    bool CommitConfiguration() ;
    void SetListenPort(int port) { listenPort = port; /* printf("LISTEN PORT %d\n",port);*/ }
    int GetOwnIP() { return ownIp; }
    int GetListenPort() { return listenPort; }
    int GetLastGwyId();
    int GetNextGwyId();
    int DecNbrOfGateways() { if(nGateways>0) --nGateways; }
    int GetNbrGateways() { return nGateways; }
    static MGC *GetInstance() ;
    static MGC *Create();
    int SendMessage( CTask *task, void *msg, int msgLen);


  private:
    int _tlsId;
    GatewayControllerAPI* _pAPI;
    H248CallAPI* _hCallAPI;
    MGC():ServiceProvider("MGC", false, false, Low_Priority){ }
    MGC( int ipAddr, int port): ServiceProvider("MGC", false, false, Low_Priority)
  {
    ownIp = ipAddr;
    listenPort = port;
    lastGwyUsed = -1;
    //printf("MGC Created \n");
  }

    static MGC *mgcInstance;
    int mgcState;
    int gwcSocketId;
    int ownIp;
    int listenPort;
    static int nGateways;
    int lastGwyUsed;
};

class H248CallAPI : public CAPIProvider
{
  MGC *_manager;
  Critical_Section critical_section_;
  public:

  H248CallAPI(int provid,MGC *manager);
  ~H248CallAPI(){};

  STATUS CallGetFreeGatewayId(CTask *, int *retVal);
  STATUS CallGetIdleEndpointId(CTask *,char *epId, char *iface, int gwyId, int handle);
  STATUS CallGetEndpointState(CTask *,  int *state , int retVal, int handle);
  STATUS CreateH248Call(CTask *task,int *newCall, char *endpoint,  ACE_INET_Addr rtpAddr, Rtp_Payload_Code codec, int handle);
  STATUS CreateDefaultH248CallParam(CTask*, CallParameter**, const char*);
  STATUS CallStoreH248RTPParam(CTask*, int cparam, ACE_INET_Addr rtpaddr, Rtp_Payload_Code codec);
  STATUS CallUpdateContext(CTask *task, int callId, int context, int handle);
  STATUS CallGetRTPAddressFromCall(CTask *task, int callId,ACE_INET_Addr &rtpAddr, Rtp_Payload_Code &codec);
  STATUS SendAddRequest(CTask *task, char *iface, ACE_INET_Addr addr, Rtp_Payload_Code codec, int legType, int *retVal);
  STATUS CallUpdateMediaAddress(CTask *task, ACE_INET_Addr addr, Rtp_Payload_Code codec, int handle);
  STATUS CallDeleteH248Context(CTask *task,int cntxId);

};

class GatewayControllerAPI: public CAPIProvider
{
  MGC *theManager;
  Critical_Section critical_section_;
  public:
  GatewayControllerAPI(int provid, MGC *mgr);
  ~GatewayControllerAPI(){}

  STATUS SendLocalMediaRequest(CTask *task,
      char *endPointRef,
      int contextId,
      ACE_INET_Addr rtpAddr,
      Rtp_Payload_Code codec,
      int gwyId,
      int cntxId,
      int handle);
  STATUS SendRemoteMediaRequest(CTask*, 
      char *endPointRef,
      int contextId,
      ACE_INET_Addr rtpAddr,
      Rtp_Payload_Code codec,
      int gwyId,
      int cntxId,
      int resphandle);
  STATUS SendMediaReleaseRequest(CTask *task, int retval, char *endpoint);
  STATUS SendMediaListen(CTask *task, String targetEp,String srcEp);
  STATUS CreateMediaGatewayMedia(CTask *task,
      char *endPointRef,
      int callparam,
      char *rtpAddr,
      char *cList,
      int gwyId);
};

typedef struct EphEndpoint
{
  ACE_INET_Addr addr;
  Rtp_Payload_Code codec;
  EphEndpoint()  { }
  EphEndpoint(ACE_INET_Addr address,int codecVal) { /*printf("\n\t\t\t#### SRI : EphEndpoint CTR \n");i*/addr = address; codec=codecVal; }
  EphEndpoint(const EphEndpoint &rtp) { addr = rtp.addr; codec = rtp.codec; }
  Rtp_Payload_Code GetCodec() { return codec; }
  void SetAddress(ACE_INET_Addr tAddr) { addr = tAddr;}
  ACE_INET_Addr GetIPAddress() { return addr;}
  void SetCodec(int tCodec) { codec = tCodec; }
  void Display() {}//{//  printf(" RTPIP/Port/Codec( %s : %u : %u )\n",addr.get_host_addr(),addr.get_port_number(),codec); }
~EphEndpoint() {
  //printf("\n\t\t\t#### SRI : DTR of EphEndpoint\n");
}
};

#if 0
class PhyEndpoint
{
  String epName;
  int    epState;
  EphEndpoint *ephEp;

  public:
  int GetEpState() { return epState; }
  void MarkEpBusy() { epState = EP_STATE_ACTIVE ; }
  PhyEndpoint(String name, int state): epName(name), epState(EP_STATE_IDLE), ephEp(NULL) {}
  virtual ~PhyEndpoint() { if(ephEp) delete ephEp; }
  void displayEndpoint() {
    cout << "EPname : "<< epName.c_str() << " EPState : "<< epState << endl;
  }

  void CreateEphEndpoint(int num, ACE_INET_Addr rtpAddr,int codec)
  {
    ephEp = new EphEndpoint(rtpAddr,codec);
    epState = EP_STATE_ACTIVE;
  }
  void DeleteEphEndpoint( PhyEndpoint &ep)
  {
    if( ep.ephEp ) 
    {
      delete ephEp;
    }
    epState = EP_STATE_IDLE;
  }
  const char *GetEpNameCstr() { return epName.c_str();}
  String GetEpName() { return epName;}
};
#endif

class MGId
{
  int    mgid;
  int    rSockid;
  int    mgIpAddr;
  String gwyName;
  int    listenPort;
  int    mgState;
  int    totalEps;
  int    curEps;
  friend class RtpInterfaceManager;
  friend class Rtp_Interface;
  GwRscVector mgSrcRec;  
  List <Rtp_Interface*> mgRtpInterfaceList;
  List <Rtp_Interface*>::iterator lastMgRtpInterfaceUsed;

  public:
  int SendMsgToGwy( void *msg, int len );
  int MGState() { return mgState; }
  int SetMGState(int newState) { mgState = newState; }
  int MediaGatewayId() {  
    return mgid; 
  }
  int GetMGIPAddress() { return mgIpAddr; }
  int GetMGPort() { return listenPort; }
  int GetMGSocket() { return rSockid; }
  MGId(int gwId, int ipAddr, int port, int sockid): 
    mgid(gwId), mgIpAddr(ipAddr), listenPort(port), rSockid(sockid),totalEps(50),mgState(GWY_STATE_ACTIVE) { 
      cout << "MG_GatewayID (gwId) : "<<gwId<<" GWIP : " <<ipAddr<<" GWPORT : "<<listenPort<< " SockFd :"<<rSockid <<endl;
    }
  virtual ~MGId(){ }
  void AddResourceRecord(RLS_RECORD *rec);
  void TokenizeByteStream(char *inStream);

  void showEndpoints(int gwId);
  /*
     PhyEndpoint *GetFreeEndpoint(int gwNum, char *iface );
     int GetEndPointState(int gwId,PhyEndpoint *ep);
     */
  void RegisterMGRtpInterface(Rtp_Interface *);
  Interface *GetMGRtpInterface(SignallingType sigType);
  void DisplayRecords() {
    //  cout << "MGId Resource Vector Size " <<  mgSrcRec.size();
    for (int i = 0 ; i < mgSrcRec.size(); i++ )
    {
      cout << mgSrcRec[i];
    }
  }
};

class H248DataInterface
{
  public:
    static int GetFreeMGId();
    static void GetIdleEndpoint(int gwNum, char *ep, char *iface);
    //static int GetEndpointStateFromMGList(int gwNum,PhyEndpoint *ep);
    static MGId *VerifyMGId(int gwNum);
};


class H248Call : public Call
{
  friend class H248Signalling;

  public:
  /** Default constructor and destructor
  */
  H248Call(CallDirection dir,const char* itfname): Call(dir,itfname) {
    //printf("\n H248Call CTR \n");
    _scheduled = false;
    _task = 0;
    //rtpEp = new  EphEndpoint(ACE_INET_Addr(0,0),0); //Yoga commented this part for compiler issue
    ACE_INET_Addr remoteAddr;
    rtpEp = new  EphEndpoint(remoteAddr,0);
    //  printf("\n\t\t\tSRI:  H248Call CTR : %s : \n",rtpEp);
    mediaSetupTimeout = 2;
  }

  /** Some funda we should follow blindly, no questions and answers pls */

  virtual bool DeviceOverlapReq(const char* additionalDigits,bool complete){}
  virtual bool DeviceProceedingReq(CallProgress progress, CallExtraInfo* exinfo){}
  virtual bool DeviceProgressReq(CallProgress progress, CallExtraInfo* exinfo){}
  virtual bool DeviceAlertingReq(CallProgress progress, CallExtraInfo* exinfo){}
  virtual bool DeviceConnectReq(CallExtraInfo* exinfo){}
  virtual bool DeviceSuspendReq(){}
  virtual bool DeviceResumeReq(){}
  virtual bool DeviceDisconnectReq(CallCause cause,CallProgress progress){}

  virtual ~H248Call() {
    /*H248 Changes */ 
    if(rtpEp)  {
      //        printf("\nSRI: H248Call DTR : %s : \n",rtpEp);
      delete rtpEp; 
      rtpEp = NULL;
    }
  }

  void SetRtpParam(ACE_INET_Addr rtpAddress, int codec )
  { 
    rtpEp->SetAddress(rtpAddress);
    rtpEp->SetCodec(codec);
    return ;
  }

  void DisplayRTPParam()
  {
    //rtpEp->Display();
  }

  EphEndpoint *GetRtpParam() {
    return rtpEp ;
  }

  void SetGwyId(int gNum)
  {
    gwyId = gNum; 
  }
  int GetMediaSetupTimer()
  {
    return mediaSetupTimeout;
  }
  void decMediaSetupTimer()
  {
    if(mediaSetupTimeout>0) mediaSetupTimeout--;
  }
  int GetGwyId() { return gwyId; }
  int GetRespHandle() { return resphandle; }
  void SetHandle(int handle) { resphandle = handle; }
  Call *GetTargetCall() { return targetCall; }
  CallParameter *GetTargetCallParameter() { return targetCall->GetCallParameter(); }
  void SetTargetCall(Call *call) { targetCall = call; }
  void SetTargetMediaState(CallMediaState newState) { targetCall->SetMediaState(newState);}
  void SetContextId( int ctxId) { cntxId = ctxId ; }
  int  GetContextId() { return cntxId; }

  private:
  bool	_scheduled;
  CTask*	_task;
  EphEndpoint  *rtpEp;
  Call         *targetCall;
  int         gwyId;
  int         cntxId;
  unsigned int mediaSetupTimeout;
  int         resphandle;
};


typedef List<H248Call*> H248CallList;
typedef std::map <int,H248CallList *> H248ContextMap; /* Context and H248Call Map */
typedef std::map<int, int>  TransMgIdMap;        /* Transaction and H248Call Map */
typedef std::map<String,int >  gwyEndpointMap;
typedef std::map<String, int>  EndpointCallMap;  
/*Endpoint and Call Map for C++ and SL Interface used in CreateH248Call*/
// MGC-MG STABILITY
typedef struct EndPointInfo
{
  char local_EndPoint[24];
  char remote_EndPoint[24];
};
typedef std::map<int , EndPointInfo *>  contextEndpointMap;
contextEndpointMap ctxEpMap;

gwyEndpointMap GwyEndpointMap;
TransMgIdMap   TransIdMap;
H248ContextMap   h248CntxMap;
EndpointCallMap  EpCallMap; 

/**Singleton object of MGC */
static MGC *MGC::mgcInstance = NULL;
static int MGC::nGateways = 0;

/** MGC holds multiple Media Gateway Connections in this connection list */
typedef std::list<MegacoTransport *> connList;
connList  mgcConList;
typedef connList::iterator  conIter;
conIter  mgcConIter;


/** On Registration, MGC adds every Media Gateway into this list  */
class MGId;
typedef std::list<MGId *> MGIdList;
MGIdList  mediaGatewayList;
typedef MGIdList::iterator MGIdIter;
MGIdIter  mgIter;

/* ContextMap for every call */
//typedef  Map <PhyEndpoint*, String> EndpointContextMap;
typedef  Map <int, String> EndpointContextMap;
EndpointContextMap EpcMap;


#endif
#endif
