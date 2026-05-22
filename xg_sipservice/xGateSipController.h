#ifndef XGATE_SIP_CONTROLLER
#define XGATE_SIP_CONTROLLER
#include <vector>  //DebugVM
#include <list>
#include <string>
#include <map>

#include <ace/Task.h>
#include <ace/Hash_Map_Manager.h>
#include <ace/Null_Mutex.h>


#include "xGateSipUtil.h"
#include "xGateBaseMsg.h"
#include "xGateUtil.h"

enum class CALL_TYPE
{
  EN_UNKNOWN_CALL = 0,
  EN_CLIENT_CALL = 1,
  EN_SERVER_CALL = 2,
  EN_INBOUND_PSTN_CALL = 3,
  EN_MS_TEAMS_CALL = 4
};

class AddressOfRecords
{
  public:
    AddressOfRecords()
    {
      m_strProxyusername  = "";
      m_strContactaddress = "";
      m_strIpaddress      = "";
      m_strDeviceType     = "";
    }

    ~AddressOfRecords()
    {
      m_strProxyusername  = "";
      m_strContactaddress = "";
      m_strIpaddress      = "";
      m_strDeviceType     = "";
      m_strTransportType  = "";
    }

    AddressOfRecords(const AddressOfRecords& rhs)
    {
      m_strProxyusername  = rhs.m_strProxyusername.c_str();
      m_strContactaddress = rhs.m_strContactaddress.c_str();
      m_strIpaddress      = rhs.m_strIpaddress.c_str();
      m_strDeviceType     = rhs.m_strDeviceType.c_str();
      m_strTransportType  = rhs.m_strTransportType.c_str();
    }
    AddressOfRecords& operator=(AddressOfRecords &rhs)
    {
      m_strProxyusername   = rhs.m_strProxyusername.c_str();
      m_strContactaddress  = rhs.m_strContactaddress.c_str();
      m_strIpaddress       = rhs.m_strIpaddress.c_str();
      m_strDeviceType      = rhs.m_strDeviceType.c_str();
       m_strTransportType  = rhs.m_strTransportType.c_str();
      return *this;
    }

  public:

    std::string m_strProxyusername;
    std::string m_strContactaddress;
    std::string m_strIpaddress;
    std::string m_strDeviceType;
    std::string m_strTransportType;

};

class AuthHeader
{
	public:
		AuthHeader()
		{
			m_sUsername = "";
			m_sRealm    = "";
			m_sNonce    = "";
			m_sURI      = "";
		}
		~AuthHeader()
		{
			m_sUsername = "";
			m_sRealm    = "";
			m_sNonce    = "";
			m_sURI      = "";
		}
		std::string m_sUsername;
		std::string m_sRealm;
		std::string m_sNonce;
		std::string m_sURI;

};

typedef struct callInfoParameter
{
  callInfoParameter()
  {
    routeAddr            = "";
    callingDeviceType    = "";
    calledDeviceType     = "";
    csaCalled	           = "";
    cnCalled	           = "";
    csaCalling           = "";
    cnCalling	           = "";
    call_id	             = "";
    uid                  = "";
    transportType        = "";
    callStream	         = ""; 
    csTransport	         = "";
    is302messagearrived  = false;
    forwardingmgcrouteIp = "";
    isServerFailure      = false;
    sdpStr               = "";
    m_sRespContact       = "";
    isInboundPstnCall    = false;
    isClientDialout      = false;
    isMsTeamsCall        = false;
  };
  //some of the params we are not using currently 
  std::string                       routeAddr;
  std::string                       callingDeviceType;
  std::string                       calledDeviceType;
  std::string                       csaCalled;
  std::string                       cnCalled;
  std::string                       csaCalling;
  std::string                       cnCalling;
  std::string                       call_id;
  std::string                       uid;
  std::string                       transportType;
  std::string                       callStream; 
  std::string                       csTransport;
  std::string                       sdpStr;
  std::string                       m_sRespContact;


  std::map<std::string,std::string> tUnKnownHeaderMap;
  AddressOfRecords                  m_AddrRecords;
  bool                              isServerFailure;
  bool                              isInboundPstnCall;
  bool                              isClientDialout;
  bool                              isMsTeamsCall;
  /*Redirecting based on 302 starts*/		
  bool                              is302messagearrived;
  std::string                       forwardingmgcrouteIp;
  callInfoParameter(const callInfoParameter &rhs);
  callInfoParameter &operator=(const callInfoParameter &rhs); 
}callInfoParameter;


typedef struct xGateSofiaSipRegobj
{
   xGateSofiaSipRegobj()
  {
    callid                = "";
    transid               = "";
    ipaddress             = "";
    contactaddress        = "";
    proxyusername         = "";
    domainname            = "";
    addresstype           = 4;
    username              = "";
    Addressofrecords      = "";
    password              = "";
    deviceid              = "";
    mobileno              = "";
    transporttype         = "";
    devicetype            = "DCAPP";
    apptype               ="IOS";
    message               = "";
    aaa                   = EN_AAA_NULL;
    expries               = 3600;//Default value as per RFC 3261
    cseq                  = 0;
    Reghandle             = nullptr;
    Regevent              = (XGATEREGEVENT)0;
    regsip                = nullptr;
    sofiamh               = nullptr;
    siptags               = nullptr;
    /*AOR Cache Requirement Start*/
    URAVConfRoomID        = "";
    URAVConfConnID        = "";
    /*AOR Cache Requirement End*/
    iuid                  = "";
    push_id               = "";
  };
  ~xGateSofiaSipRegobj()
  {
    callid                = "";
    transid               = "";
    ipaddress             = "";
    contactaddress        = "";
    proxyusername         = "";
    domainname            = "";
    addresstype           = 4;
    username              = "";
    Addressofrecords      = "";
    password              = "";
    deviceid              = "";
    transporttype         = "";
    mobileno              = "";
    devicetype            = "DCAPP";
    apptype               ="IOS";
    message               = "";
    aaa                   = EN_AAA_NULL;
    expries               = 3600;//Default value as per RFC 3261
    cseq                  = 0;
    Reghandle             = nullptr;
    Regevent              = (XGATEREGEVENT)0;
    regsip                = nullptr;
    sofiamh               = nullptr;
    siptags               = nullptr;
    /*AOR Cache Requirement Start*/
    URAVConfRoomID        = "";
    URAVConfConnID        = "";
    /*AOR Cache Requirement End*/
    iuid                  = "";
    push_id               = "";
  };
  std::string     callid;
  std::string     transid;
  std::string     ipaddress;
  std::string     contactaddress;
  std::string     domainname;
  std::string     username;
  std::string     password;
  std::string     deviceid;
  std::string     mobileno;
  std::string     devicetype;
  std::string     transporttype;
  std::string     apptype;
  std::string     proxyusername;
  std::string     macaddr;
  std::string     Addressofrecords;
  /*AOR Cache     Requirement Start*/
  std::string     URAVConfRoomID;
  std::string     URAVConfConnID;
  /*AOR Cache     Requirement End*/
  REGAAA          aaa; // Accounting-Authorising-Authenticating
  std::string     message; // message to pbx for dialout
  std::string     iuid;
  std::string     push_id;
  int             expries;
  int             cseq;
  int             addresstype;
  int             status;
  XGATEREGEVENT   Regevent;
	AuthHeader      m_authHeader;
  void            *Reghandle;
  void            *regsip;
  void            *sofiamh;
  void            *sofiahmsg;
  void            *siptags;

}xGateSofiaSipRegobj;



void PostToReceiver(xGateSofiaSipRegobj *obj);
xGateSofiaSipRegobj *PopFromReceiver();

typedef struct xGateCallObj 
{
  xGateCallObj()
  {
    callid = "";
    uid = "";
    m_sCallingNumber = "";
    m_sCalledNumber = "";
    m_sDomainName = "";
    m_sCallingExt = "";
    m_sPassword = "";
    m_sRouteIP = "";
    target = "";
    cause = "";
    m_sKey = "";
    m_sCallInfo = "";
    iTimerValue = 0;
    m_iTrunkId = 0;
    m_RespCode = 0;
    sipeventtype = XGATESIPEVENT::EN_NULL;
    m_regObj = nullptr;
  };

  ~xGateCallObj()
  {
    callid = "";
    uid = "";
    m_sCallingNumber = "";
    m_sCalledNumber = "";
    m_sDomainName = "";
    m_sCallingExt = "";
    m_sPassword = "";
    m_sRouteIP = "";
    target = "";
    cause = "";
    m_sKey = "";
    m_sCallInfo = "";
    iTimerValue = 0;
    m_iTrunkId = 0;
    m_RespCode = 0;
    sipeventtype = XGATESIPEVENT::EN_NULL;
    m_regObj = nullptr;
  };

  std::string         callid;     
  std::string         uid ;// Unique id 
  std::string         m_sCallingNumber;
  std::string         m_sCalledNumber;
  std::string         m_sDomainName;
  std::string         m_sCallingExt;
  std::string         m_sPassword;
  std::string         m_sRouteIP;
  std::string         target ;
  std::string         cause;
  std::string         m_sKey;
  std::string         m_sCallInfo;
  unsigned int        iTimerValue;
  int                 m_iTrunkId;
  int                 m_RespCode;
  callInfoParameter   cp;
  XGATESIPEVENT       sipeventtype; // represent in SIP state side
  void xGateCallObjInit(){
    sipeventtype = XGATESIPEVENT::EN_NULL;
  }
  xGateSofiaSipRegobj *m_regObj;
}xGateCallObj;

void WriteToReceiver(xGateCallObj *obj);
xGateCallObj* ReadFromReceiver();

struct Replaceinfo
{
  Replaceinfo()
  {
    from_tag = "";
    to_tag = "";
    Invitecallid = "";
    Responsecallid = "";
  };
  ~Replaceinfo()
  {
    from_tag = "";
    to_tag = "";
    Invitecallid = "";
    Responsecallid = "";
  };

  std::string from_tag;
  std::string to_tag;
  std::string Invitecallid;
  std::string Responsecallid;
};

typedef enum xGateSofiaLegIPAddFamily
{
	IP_ADDR_V4,
	IP_ADDR_V6
}XGATE_IPADDFAMILY;


typedef struct xGateSofiaobj
{
  std::string       callid;
  std::string       uid; //unique id for all group
  std::string       target; //unique id for all group
  std::string       cause; //unique id for all group
  std::string       dialcallid;
  std::string       m_sCallingNumber;
  std::string       m_sCalledNumber;
  std::string       m_sCallingExt;
  std::string       m_sCallingDeviceType;
  std::string       m_sDomainName;
  std::string       m_sPassword;
  std::string       m_sClientIP;
  std::string       m_sClientPort;
  std::string       m_sClientTransport;
  std::string       m_sOffSdp;//Offer sdp
  std::string       m_sAnsSdp;//answer sdp
  std::string       m_sRouteIP;//call routed ip from SGW to MGC/Conf Server
  std::string       m_sInfoContentType;//sip info content type;
  std::string       m_sTransportType;
  std::string       m_sKey;
  std::string       m_sCallInfo;
  int               m_iTrunkId;
  void              *sofiamh; // root memeory magic
  void              *sofianh; // dialog handle inside root
  void              *sofiahmsg;
  void              *callEntry; // HASH MAP Entry
  void              *siptag; // sip_t message
  void              *lsdp; // sdp session of local
  void              *rsdp; // sdp session of remote
  void              *InfoPayload;//sip info payload
  bool              isPeerInviteSent;
  bool              isInviteTimerStarted;
  bool              isCallReRouted;
  bool              isAckReceived;
  bool              isSipInfoPending;
  bool              isTrunkCall;
  bool              isPbxCall;

  xGateSofiaobj     *SofiaLegB;
  Replaceinfo       replaceinfo;

  XGATECALLSTREAM   callstream;
  XGATE_IPADDFAMILY callLegIpAddrFamily;
  std::string       siptransport;
  //  CallState callstate; // hold the Call object state after process reply
  XGATECALLSTATE    callstate;
  callInfoParameter cp;
  XGATESIPEVENT     sipeventtype;
  XGATEMEDIASTATE   mediastate;
  int               P2PMedia; // 1 represents P2P enabled 0 as disabled
  int               Sipcallstate;
  unsigned int      callauthstate;
  // Copy only Required information
  void xGateCopyPeerInfo(xGateSofiaobj &sobj)
  {
    callid                 = sobj.callid.c_str();
    dialcallid             = sobj.dialcallid.c_str();
    uid                    = sobj.uid.c_str();
    target                 = sobj.target.c_str();
    cause                  = sobj.cause.c_str();
    m_sCallingNumber       = sobj.m_sCallingNumber.c_str();
    m_sCalledNumber        = sobj.m_sCalledNumber.c_str();
    m_sCallingDeviceType   = sobj.m_sCallingDeviceType.c_str();
    m_sCallingExt          = sobj.m_sCallingExt.c_str();
    m_sDomainName          = sobj.m_sDomainName.c_str();
    m_sPassword            = sobj.m_sPassword.c_str();
    m_sClientIP            = sobj.m_sClientIP.c_str();
    m_sClientPort          = sobj.m_sClientPort.c_str();
    m_sClientTransport     = sobj.m_sClientTransport.c_str();
    m_sOffSdp              = sobj.m_sOffSdp.c_str();
    m_sAnsSdp              = sobj.m_sAnsSdp.c_str();
    m_sRouteIP             = sobj.m_sRouteIP.c_str();
    m_sInfoContentType     = sobj.m_sInfoContentType.c_str();
    m_sTransportType       = sobj.m_sTransportType.c_str();
    m_sKey                 = sobj.m_sKey.c_str();
    m_sCallInfo            = sobj.m_sCallInfo.c_str();
    m_iTrunkId             = sobj.m_iTrunkId;
    sofiamh                = sobj.sofiamh;
    sofiahmsg              = sobj.sofiahmsg;
    callEntry              = sobj.callEntry;
    siptag                 = sobj.siptag;
    lsdp                   = sobj.lsdp;
    rsdp                   = sobj.rsdp;
    InfoPayload            = sobj.InfoPayload;
    isPeerInviteSent       = sobj.isPeerInviteSent;
    callLegIpAddrFamily    = sobj.callLegIpAddrFamily;
    isInviteTimerStarted   = sobj.isInviteTimerStarted;
    isCallReRouted         = sobj.isCallReRouted;
    isAckReceived          = sobj.isAckReceived;
    isSipInfoPending       = sobj.isSipInfoPending;
    isTrunkCall            = sobj.isTrunkCall;
    isPbxCall              = sobj.isPbxCall;


    SofiaLegB              = &sobj;

    //DebugVM cp = sobj.cp;

  }
  xGateSofiaobj(){
    callid                   = "";
    uid                      = "";
    target                   = "";
    cause                    = "";
    dialcallid               = "";
    m_sCallingNumber         = "";
    m_sCalledNumber          = "";
    m_sCallingExt            = "";
    m_sCallingDeviceType     = "";
    m_sDomainName            = "";
    m_sPassword              = "";
    m_sClientIP              = "";
    m_sClientPort            = "";
    m_sClientTransport       = "";
    m_sOffSdp                = "";
    m_sAnsSdp                = "";
    m_sRouteIP               = "";
    m_sInfoContentType       = "";
    m_sTransportType         = "";
    m_sKey                   = "";
    m_sCallInfo              = "";
    m_iTrunkId               = 0;
    siptag                   = nullptr;
    sofiahmsg                = nullptr;
    InfoPayload              = nullptr;
    callstream               = EN_IDLE;
    mediastate               = EN_MED_NULL;
    callstate                = EN_CS_NULL;
    lsdp                     = nullptr;
    rsdp                     = nullptr;
    callauthstate            = 0;
    isPeerInviteSent         = false;
    callLegIpAddrFamily      = IP_ADDR_V4;
    isInviteTimerStarted     = false;
    isCallReRouted           = false;
    isAckReceived            = false;
    isSipInfoPending         = false;
    isTrunkCall              = false;
    isPbxCall                = false;

    SofiaLegB                = nullptr;
  }

  ~xGateSofiaobj(){
    callid                   = "";
    uid                      = "";
    target                   = "";
    cause                    = "";
    dialcallid               = "";
    m_sCallingNumber         = "";
    m_sCalledNumber          = "";
    m_sCallingExt            = "";
    m_sCallingDeviceType     = "";
    m_sDomainName            = "";
    m_sPassword              = "";
    m_sClientIP              = "";
    m_sClientPort            = "";
    m_sClientTransport       = "";
    m_sOffSdp                = "";
    m_sAnsSdp                = "";
    m_sRouteIP               = "";
    m_sInfoContentType       = "";
    m_sTransportType         = "";
    m_sKey                   = "";
    m_sCallInfo              = "";
    m_iTrunkId               = 0;
    siptag                   = nullptr;
    sofiahmsg                = nullptr;
    InfoPayload              = nullptr;
    callstream               = EN_IDLE;
    mediastate               = EN_MED_NULL;
    callstate                = EN_CS_NULL;
    lsdp                     = nullptr;
    rsdp                     = nullptr;
    callauthstate            = 0;
    isPeerInviteSent         = false;
    callLegIpAddrFamily      = IP_ADDR_V4;
    isInviteTimerStarted     = false;
    isCallReRouted           = false;
    isAckReceived            = false;
    isSipInfoPending         = false;
    isTrunkCall              = false;
    isPbxCall                = false;

    SofiaLegB                = nullptr;
  }

  void DeleteAll(){
    if(lsdp) 
      delete[] (char *)lsdp;
    if(rsdp)  
      delete[] (char *)rsdp;

    lsdp = nullptr;
    rsdp = nullptr;

  }

}xGateSofiaobj;  

typedef struct xGateCallPair{
  xGateSofiaobj* Orgcall;
  xGateSofiaobj* Dstcall;

  xGateCallPair(){
    Orgcall = nullptr;
    Dstcall = nullptr;
  }
  ~xGateCallPair(){
    Orgcall = nullptr;
    Dstcall = nullptr;

  }
}xGateCallPair;


/*----------------------------------------------------------------------------------
 *                       xGateSipMsgBlock Delecation
 ------------------------------------------------------------------------------------*/
class xGateSipMsgBlock : public xGateBaseMsg{
  public:
    //xGateSipMsgBlock():xGateBaseMsg(EN_XGATE_MSG_UNKNOWN){
    xGateSipMsgBlock():xGateBaseMsg(EN_XGATE_MSG_SIP_IN){
    }

    xGateSipMsgBlock(xGateMsgType mgtype) :xGateBaseMsg(mgtype){
    }
    virtual ~xGateSipMsgBlock(){

    }

    xGateCallObj *m_callObj;
};

/*AOR Cache Requirement Start*/
class ClientAOR
{
  public:
    ClientAOR(){
      m_contactAddr = "";	
      m_contactIpAddr = "";
      m_contactPort = "";
      m_meetingID = "";    
      m_transport = "";
    };
    ClientAOR(std::string &contactIpAddr, std::string &contactPort, std::string &meetingId);
    std::string m_contactAddr;
    std::string m_contactIpAddr;
    std::string m_contactPort;
    std::string m_meetingID; 
    std::string m_transport;
};
/*AOR Cache Requirement End*/

class SipHandlerParams
{
  public:
    SipHandlerParams()
    {
      m_iRespTimeOut    = 0;
      m_iRetryCount     = 0;
      m_iServerExpires  = 0;
      m_sPbxUA          = "";
      m_sMgcUA          = "";
      m_sPbxUA          = "";
      m_sMgcUA          = "";
      m_sSipURL         = "";
      m_sSipsURL        = "";
      m_sWssURL         = "";  
      m_sSipPort        = "";
      m_sWssPort        = "";
      m_sSipsPort       = "";
      m_sCertPath       = "";
      m_sDefaultPBXIP   = "";
      m_sPrivateIP      = "";
      m_isSbcEnabled    = false;
    };
    ~SipHandlerParams(){};
    int m_iRespTimeOut;
    int m_iRetryCount;
    unsigned int m_iServerExpires;
    std::string m_sPbxUA;
    std::string m_sMgcUA;
    std::string m_sSipURL;
    std::string m_sSipsURL;
    std::string m_sWssURL;
    std::string m_sSipPort;
    std::string m_sWssPort;
    std::string m_sSipsPort;
    std::string m_sCertPath;
    std::string m_sIPv4Add;
    std::string m_sIPv6Add;
    std::string m_sDefaultPBXIP;
    std::string m_sPrivateIP;
    bool m_isSbcEnabled;
};

class xGateSipController  {

  // Member vaiables

  private:    
    typedef ACE_Hash_Map_Manager<ACE_TString, xGateCallPair*,ACE_SYNCH_RW_MUTEX> CALL_CONNECTED_LIST;
    CALL_CONNECTED_LIST m_CallConnectedList;

    typedef ACE_Hash_Map_Manager<ACE_TString, std::map<std::string,xGateSofiaobj*>* , ACE_SYNCH_RW_MUTEX> CALL_LIST;
    CALL_LIST m_CallConferenceList;
    CALL_LIST m_CallConferenceJoinersList;
    CALL_LIST m_CallDialList;
    CALL_LIST m_ExtChannelList;
    // CALL_LIST m_dummy;
    CALL_LIST m_CallHuntGroupList;
  public:
    //std::vector <xGateSbcinfo*> sbcip;
    //std::vector<xGatePbxinfo*>pbxip;
    /*AOR Cache Requirement Start*/
    std::map <std::string, ClientAOR*> m_mapConfUserAor;  //Key is Connection ID
    /*AOR Cache Requirement End*/

    /* LWSS Round Robin Start */
    std::map <std::string, std::list<std::pair<std::string, std::string> > > m_domainRouteMap;
    /* LWSS Round Robin End */

    xGateSipController();
    virtual   ~xGateSipController() { }
  public:  //DebugVM
    // Pure derived functions
    virtual xGateReturn SipLibInit()= 0; // Create the intializing of  SIP socket
    virtual xGateReturn SipLibLoad()=0; // Loding SIP Library & creating call back
    virtual xGateReturn SipLibUnLoad()=0; // Releasing Library
    virtual xGateReturn RunEventLoop()=0; //  Event Polling 
    std::string CreateUniqueCallid(char const *user,char const *host, \
	char const *tag, char const *callid); // Creates call id for the call
    std::string CreateUniqueCallid(char const *callid, char const *tag);
    xGateCallPair*GetConnectedList(std::string key);
    bool InsertConnectedList(std::string key,xGateSofiaobj* sobj,XGATEOBJECT type );
    bool DeleteConnectedList(std::string key,xGateSofiaobj* sobj,XGATEOBJECT type);


    bool InsertCallList(std::string grpkey, xGateSofiaobj *sobj);
    std::map<std::string,xGateSofiaobj*>*GetCallList(std::string grpkey);
    bool RemoveCallListValues(std::string grpkey,std::map<std::string, xGateSofiaobj*>*);
    bool DeleteCallList(std::string grpkey);


    bool PostMessage(xGateCallObj *callObj,XGATEPOSTTYPE destType);
    std::string GetCurrentPbxInfo();
    bool CheckDomainForCallAuth(std::string cDomainName);// This is Busniess Logic for Auth Enable
    SipHandlerParams *m_pSipHndlrParams;
};
#endif
