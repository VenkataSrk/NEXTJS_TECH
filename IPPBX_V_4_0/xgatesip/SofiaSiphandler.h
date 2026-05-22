#ifndef SOFIA_SIP_HANDLER
#define SOFIA_SIP_HANDLER
#include "xGateSipController.h"
#include "xGateCallInfoEncoder.h"
#include "URModuleBridge.h"

extern "C"
{
#include <sofia-sip/nua.h>
#include <sofia-sip/sip_extra.h>
#include <sofia-sip/sdp.h>
#include <sofia-sip/auth_module.h>
#include <sofia-sip/auth_plugin.h>
}

#include <sstream>

#ifndef __linux__
#define RETCALLBACK static void __cdecl
#else
#define RETCALLBACK static void 
#endif
using namespace sipreturn;
class SofiaSiphandler: public xGateSipController{
  public:
    SofiaSiphandler();
    ~SofiaSiphandler();

    // Sofia Libary Initializing function 
    virtual xGateRetVal SipLibInit(); // Create the intializing of Sofia SIP
    virtual xGateRetVal SipLibLoad(); // Loding SIP Library & creating call back
    virtual xGateRetVal SipLibUnLoad(); // Releasing Library
    virtual xGateRetVal RunEventLoop(); // Get Message Queue by polling method

    // Sofia Sip Response function
    virtual xGateRetVal RespondWith1xx(xGateSofiaobj* pSipMgc);// Provisional Response
    virtual xGateRetVal RespondWith2xx(xGateSofiaobj* pSipMgc); // Succefull Response
    virtual xGateRetVal RespondWith3xx(xGateSofiaobj* pSipMgc); // Redirectional Response
    virtual xGateRetVal RespondWith4xx(xGateSofiaobj* pSipMgc); //  Client Failure Response
    virtual xGateRetVal RespondWith5xx(xGateSofiaobj* pSipMgc); // Server Failure Response
    virtual xGateRetVal RespondWith6xx(xGateSofiaobj* pSipMgc); // Global Failure REsponse
    virtual xGateRetVal RespondWithInvite(xGateSofiaobj* pSipMgc);// Provisional Response
    // Registart Response Function
#if 0 //SRC
    virtual xGateRetVal RegistarResponse(xGateSofiaSipRegobj *regobj); // reply on registart request sending 401
    // Sofia Call Action Functions 
    xGateRetVal DialOut(xGateSofiaobj *sobj);
    xGateRetVal ForkedDialOut(xGateSofiaobj *sobj); //Added for Forked DialOut
#endif

  private: // member variable 
    su_home_t m_homeMemory; //
    su_root_t* m_root; //
    nua_t * m_nuaHandle; //

    auth_mod_t *mod ;
    xGateCallInfoEncoder *m_callInfoEncoder;

  private: // Member Functions
    void SofiaInit();
    void SofiaDeInit();
    void SofiaInitMemory(su_home_t &);
    void SofiaDeInitMemory(su_home_t &);
    void SofiaRootStep(nua_t* sua_handle,su_root_t* nuaRoot);
    su_root_t* SofiaInitRoot(su_root_magic_t* appInitance);
    void SofiaDeInitRoot(su_root_t* nuaRoot);
    auth_mod_t* SofiaInitAuth(su_root_t* nuaroot);
    void SofiaDeInitAuth(auth_mod_t *authmod);
    nua_t* SofiaCreate(su_root_t *root,nua_callback_f callback, nua_magic_t *magic);
    void SofiaSetParam(nua_t* nuaHandle);
    void SofiaShutdown(nua_t* nuaHandle);
    RETCALLBACK SofiaCallBack(nua_event_t   event,int status,char const *phrase,nua_t *nua,\  
        nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,  
        tagi_t        tags[]);

    void SofiaDestoryhandle(nua_t *nuaHandler);
    void SofiaCallTerminate();
    void SofiaRespond(nua_handle_t *nh,int status, char const *phrase);
    void SofiaRespond(nua_handle_t *nh,int status, char const *phrase,tagi_t tag[]);
    void SofiaSendAck(nua_handle_t *nh);
    void SofiaSendInvite(nua_handle_t *nh);
    void SofiaSendBye(nua_handle_t *nh);
    void SofiaSetLog(int level);
    void SofiaBindhandle(nua_handle_t *nh,nua_hmagic_t* sofiaobjhandle);
    void SofiaReBindhandle(nua_handle_t *Old_nh,nua_handle_t *New_nh,nua_hmagic_t* sofiaobjhandle);
    url_t *SofiaCreateMake(su_home_t &su_home,const char* dialstr);

    // Initializing Function
    bool CreateCallobjparam(xGateCallObj *callobj,xGateSofiaobj *sobj,nua_handle_t *nh,\
        sip_t const  *sip,tagi_t  tags[]);

    xGateSofiaobj* CreateSofiaobjparam(std::string callid,nua_handle_t *nh,\
        nua_magic_t  *magic,sip_t *siptag);
    bool CreateSofiaRegobj(xGateSofiaSipRegobj &regparam,sip_t const  *sip);
    bool SetcallInfoParameter (callInfoParameter &cp,sip_t const *sip, tagi_t tags[] );
    //nua_handle_t *CreateSofiaDialhandle(xGateSofiaobj *sobj,UserDeviceType deviceType = XGATE_UT_DESKPHONE);
    //callInfoParameter
    nua_handle_t *CreateSofiaDialhandle(callInfoParameter cp,sip_t const *siptag);
    //bool CreateLocalSdpParam(xGateSofiaobj *sobj,nua_handle_t* handle=NULL);
    //sdp_session_t *CreateLocalSdpParam(XGATESDPMODE  sdpType=EN_SDP_NULL,xGateSofiaobj *sobj=NULL);
    std::string CreateLocalSdpParam(XGATESDPMODE  sdpType=EN_SDP_NULL,xGateSofiaobj *sobj=NULL);

    std::string CreateOpusCodec(xGateSDPInfo sdpinfo,xGateSofiaobj *sobj,XGATESDPMODE sdpmode);

    nua_t * GetAppContext() {
      return m_nuaHandle;

    }
    bool UpdateSofiaobjParam(xGateSDPInfo &sdpinfo, xGateCallObj *event,int avtype);
    // Event Action functionality
    void OnInviteRequest(int status,char const *phrase,nua_t *nua,\  
        nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,  
        tagi_t        tags[]);
    void OnInfoRequest(int status,char const *phrase,nua_t *nua,\  
        nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,  
        tagi_t        tags[]);
    void OnByeRequest(int status,char const *phrase,nua_t *nua,\  
        nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,  
        tagi_t        tags[]);

    void OnStateChange(int status,char const *phrase,nua_t *nua,\  
        nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,  
        tagi_t        tags[]);
    void OnRequestResponse(int status,char const *phrase,nua_t *nua,\  
        nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,  
        tagi_t        tags[]);
    //void OnForked2xxResponse(nua_hmagic_t *hmagic, int status);
    void OnFailureResponse(nua_hmagic_t *hmagic, int status);
    void OnInviteCancel(int status,char const *phrase,nua_t *nua,\
        nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,
        tagi_t        tags[]);
    void RespondToPeer(int status,xGateSofiaobj *sobj);
    bool CheckResponseRequired(int status, xGateSofiaobj *sobj); // Help to check response required or not
    bool ForkingResponse(int status, xGateSofiaobj *sobj);
    //void SetRemoteSDP(xGateSofiaobj sobj,sdp_session_t* sdp);
#if 0 //SRC

    void OnRegistrationRequest(int status,char const *phrase,nua_t *nua,\
        nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,
        tagi_t        tags[]);
    void OnMessageRequest(int status,char const *phrase,nua_t *nua,\
        nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,
        tagi_t        tags[]);
    void OnReferRequest(int status,char const *phrase,nua_t *nua,\
        nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,
        tagi_t        tags[]);
#endif
    void OnProcessEvent(xGateSofiaobj *sobj, xGateCallObj *event); // Reply Event from CallManager
    void ProcessInviteRequest(nua_handle_t *nh,sip_t const  *sip,tagi_t tags[],xGateSofiaobj *sobj);// Process the incoming Request message
    virtual xGateSofiaobj* GetProcessEvent();
    void GetProcessRegisterEvent(); // Get Register event from queue for reply which is processed by DB Manager
    void RequestTermination(xGateSofiaobj *sobj); // Terminate Request to SL App on abromal condation
    void SendReInvite(xGateSofiaobj *sobj); 

    // Valaditing Function
    bool ValidateSipMessage(sip_t const *sip, SIPVALIDATE header); // For Inbound 

    std::string ValidateDialParam(const char *pVal, SIPVALIDATE header,XGATESIPKEY key,callInfoParameter cp);

    void parsestringtokens(std::string Input, char delimit, std::string &token1, std::string &token2);

    // Auth functions
    std::string Authenticating(xGateSofiaobj *sobj, sip_www_authenticate_t const *auth); // send UAC authendication 
    bool VerifyAuthorisationResponce(sip_t const *sip, xGateSofiaSipRegobj *regobj,char requestmethod[15] ); // Function creates MD5 Hash and test for responce tag
    bool AuthenticateSipSession(int status,char const *phrase,nua_t *nua,\
        nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,
        tagi_t        tags[]);
    std::string CreateDigiestNonce();
    // Sip Header creation
    std::string FillHeader(url_t *url,callInfoParameter cp,SIPVALIDATE header);
    void CreateSDPParam(xGateSofiaobj *sobj);
    bool DeleteSofiaObject(xGateSofiaobj *sobj);
    // Media SDP 
    int GetCodecFromList(int payload);
    int GetCodecFromList(const char* codename, int mediaType);
    sdp_rtpmap_t *CheckCodecCompatablity(sdp_session_t *rsdp);
    bool CheckCodecCompatablity(xGateAVCodec *avcodec);
    bool CheckCodecCompatablity(std::vector<xGateAVCodec*> );
    void AddServerCodecToList(xGateSofiaobj *sobj);
    void SdpParser(sdp_session_t *rsdp,xGateSofiaobj *sobj);
    void IceCandidateParser(sdp_session_t *rsdp,xGateSofiaobj *sobj);
    void RtpAvpParser(sdp_session_t *rsdp,xGateSofiaobj *sobj);
    void MediaInfoParserSAVPF(sdp_media_t *media, xGateSDPInfo &sdpinfo);
    void MediaInfoParserAVP(sdp_media_t *media, xGateSDPInfo &sdpinfo);
    void AttributeInfoParser(sdp_attribute_t *attribute, xGateSDPInfo &sdpinfo);
    std::string CreateG711Codec(xGateSDPInfo sdpinfo,xGateSofiaobj *sobj, XGATESDPMODE sdpmode);
    std::string EncodeResponseSDP(xGateSofiaobj *sobj);
    std::string EncodeDialSDP(xGateSofiaobj *sobj);
    std::string EncodeSessionAttribute(xGateSDPInfo audio, xGateSDPInfo video,int DeviceType);
    std::string EncodeMediaAttribute(xGateSDPInfo sdpinfo, XGATESDPMODE sdpmode,int DeviceType);
    std::string EncodeConnectionInformation(xGateSDPInfo sdpinfo, XGATESDPMODE sdpmode,int DeviceType);
    std::string MultiCodecSAVPF(xGateSofiaobj *sobj,XGATESDPMODE sdpmode);
    std::string MultiCodecAVP(xGateSofiaobj *sobj,XGATESDPMODE sdpmode);
    std::string MultiCodecSAVP(xGateSofiaobj *sobj,XGATESDPMODE sdpmode);

    bool getSipContactURLbyIPAddrFamily(xGateSofiaobj *pSipMgc, string &strPbxIP);

    // MG Communication 
    void RequestForMedia(xGateSofiaobj *sobj,XGATEMGUPDATE mgMsgType=EN_MEDIA_CREATE);
    // PBX Impementation 
    void SofiaSiphandler::OnCallRecordProcess(xGateSofiaobj *sobj, const char* xmlinfo);
    xGateRetVal CallListCancle(std::string key);
    void updateCDR(xGateSofiaobj *sobj);
    // Feature functionality
    void LinkDstInfoWithOrgInfo(std::string uid);
};
#endif
