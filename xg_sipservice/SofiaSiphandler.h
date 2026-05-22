#ifndef SOFIA_SIP_HANDLER
#define SOFIA_SIP_HANDLER
#include "xGateSipController.h"

extern "C"
{
#include <sofia-sip/nua.h>
#include <sofia-sip/sip_extra.h>
#include <sofia-sip/sdp.h>
#include <sofia-sip/auth_module.h>
#include <sofia-sip/auth_plugin.h>
}

//DebugVM start
//DebugVM #ifndef __linux__
//#define RETCALLBACK static void __cdecl
//DebugVM #else
#define RETCALLBACK static void 
//DebugVM #endif
//DebugVM end

class SofiaSiphandler: public xGateSipController{
  public:
    SofiaSiphandler();
    ~SofiaSiphandler();
    //virtual ~SofiaSiphandler();

    // Sofia Libary Initializing function 
    virtual xGateReturn SipLibInit(); // Create the intializing of Sofia SIP
    virtual xGateReturn SipLibLoad(); // Loding SIP Library & creating call back
    virtual xGateReturn SipLibUnLoad(); // Releasing Library
    virtual xGateReturn RunEventLoop(); // Get Message Queue by polling method

    //DebugVM REGISTER
    void SendRegister();
    void SendDeRegister(char *to_uri,char *from_uri,char *contact_uri);
    void SendRefreshRegister(char *to_uri,char *from_uri,char *contact_uri);

    // Sofia Sip Response function
    virtual xGateReturn RespondWith1xx(xGateSofiaobj* pSipMgc);// Provisional Response
    virtual xGateReturn RespondWith2xx(xGateSofiaobj* pSipMgc); // Succefull Response
    virtual xGateReturn SendReInvite(xGateSofiaobj* pSipMgc);
    virtual xGateReturn SendReInviteForConfUser(xGateSofiaobj* sobj);
    virtual xGateReturn SendRefer(xGateSofiaobj* pSipMgc, xGateCallObj *callObj);
    // Registart Response Function
    virtual xGateReturn RegistarResponse(xGateSofiaSipRegobj *regobj); // reply on registart request sending 401
    virtual xGateReturn RespondWith3xx(xGateSofiaobj* pSipMgc); // Redirectional Response
    virtual xGateReturn RespondWith4xx(xGateSofiaobj* pSipMgc); //  Client Failure Response
    virtual xGateReturn RespondWith5xx(xGateSofiaobj* pSipMgc); // Server Failure Response
    virtual xGateReturn RespondWith6xx(xGateSofiaobj* pSipMgc); // Global Failure REsponse
#if 0 //DebugVM start
    virtual xGateReturn RespondWithInvite(xGateSofiaobj* pSipMgc);// Provisional Response
    // Sofia Call Action Functions 
    xGateReturn DialOut(xGateSofiaobj *sobj);
    xGateReturn ForkedDialOut(xGateSofiaobj *sobj); //Added for Forked DialOut
    //xGateReturn DisconnectCall();
    //xGateReturn AcceptCall();
    //xGateReturn PutOnHold();
    //xGateReturn ReleaseHold();
    //xGateReturn MakeTransfer();
    //xGateReturn CreateConference();
    //xGateReturn ReleaseConference();
#endif //DebugVM end
//DebugVM start
    bool ValidateSipMsgPointer(const char *p);
//DebugVM end


  private: // member variable 
    su_home_t m_homeMemory; //
    su_root_t* m_root; //
    nua_t * m_nuaHandle; //

    auth_mod_t *mod ;

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
    RETCALLBACK SofiaCallBack(nua_event_t   event,int status,char const *phrase,nua_t *nua,  
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
    //DebugVM start
    virtual xGateSofiaobj* GetProcessEvent();
    void GetProcessRegisterEvent(); // Get Register event from queue for reply which is processed by DB Manager
    //DebugVM end

    // Initializing Function
    bool CreateCallobjparam(xGateCallObj *callobj,xGateSofiaobj *sobj,nua_handle_t *nh,\
        sip_t const  *sip,tagi_t  tags[]);

    xGateSofiaobj* CreateSofiaobjparam(std::string callid,nua_handle_t *nh,\
        nua_magic_t  *magic,sip_t *siptag);
    bool CreateSofiaRegobj(xGateSofiaSipRegobj &regparam,sip_t const  *sip);
    bool SetcallInfoParameter (callInfoParameter &cp,sip_t const *sip, tagi_t tags[] );
    nua_handle_t *CreateSofiaDialhandle(callInfoParameter cp,sip_t const *siptag);

    //Added for IPv6 Support
    bool getSipContactURLbyIPAddrFamily(xGateSofiaobj *pSipMgc, string &strPbxIP);
    bool ip_validator(string ipAddress);

#if 0 //DebugVM start
    //nua_handle_t *CreateSofiaDialhandle(xGateSofiaobj *sobj,UserDeviceType deviceType = XGATE_UT_DESKPHONE);
    //callInfoParameter
    //bool CreateLocalSdpParam(xGateSofiaobj *sobj,nua_handle_t* handle=NULL);
    //sdp_session_t *CreateLocalSdpParam(XGATESDPMODE  sdpType=EN_SDP_NULL,xGateSofiaobj *sobj=NULL);
#endif //DebugVM end
    std::string CreateLocalSdpParam(XGATESDPMODE  sdpType=EN_SDP_NULL,xGateSofiaobj *sobj=NULL);
#if 0 //DebugVM start

    std::string CreateOpusCodec(xGateSDPInfo sdpinfo,xGateSofiaobj *sobj,XGATESDPMODE sdpmode);

#endif //DebugVM end
    nua_t * GetAppContext() {
      return m_nuaHandle;

    }
    bool UpdateSofiaobjParam(xGateSDPInfo &sdpinfo, xGateCallObj *event,int avtype);
    // Event Action functionality
    void OnInviteRequest(int status,char const *phrase,nua_t *nua, 
        nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,  
        tagi_t        tags[]);
    void OnInfoRequest(int status,char const *phrase,nua_t *nua, 
        nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,  
        tagi_t        tags[]);
    void OnByeRequest(int status,char const *phrase,nua_t *nua,  
        nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,  
        tagi_t        tags[]);
    void OnNotifyRequest(int status,char const *phrase,nua_t *nua,
        nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,  
        tagi_t        tags[]);

    void OnStateChange(int status,char const *phrase,nua_t *nua,
        nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,  
        tagi_t        tags[]);
    char* deblank(char* input);
    void auth_MD5(nua_handle_t *nh,sip_t const *sip);
    void OnRegisterResponse(int status,char const *phrase,nua_t *nua,
        nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,  
        tagi_t        tags[]);
    void OnRequestResponse(int status,char const *phrase,nua_t *nua,  
        nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,  
        tagi_t        tags[]);
#if 0 //DebugVM start
    //void OnForked2xxResponse(nua_hmagic_t *hmagic, int status);
    void OnFailureResponse(nua_hmagic_t *hmagic, int status);
    void OnInviteCancel(int status,char const *phrase,nua_t *nua,
        nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,
        tagi_t        tags[]);
#endif //DebugVM end
    void RespondToPeer(int status,xGateSofiaobj *sobj);
    bool OnRegistrationRequest(int status,char const *phrase,nua_t *nua,
        nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,
        tagi_t        tags[]);

#if 0 //DebugVM start
    bool CheckResponseRequired(int status, xGateSofiaobj *sobj); // Help to check response required or not
    bool ForkingResponse(int status, xGateSofiaobj *sobj);
    //void SetRemoteSDP(xGateSofiaobj sobj,sdp_session_t* sdp);

    void OnMessageRequest(int status,char const *phrase,nua_t *nua,
        nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,
        tagi_t        tags[]);
    void OnReferRequest(int status,char const *phrase,nua_t *nua,
        nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,
        tagi_t        tags[]);
#endif //DebugVM end
    void OnProcessEvent(xGateSofiaobj *sobj, xGateCallObj *event); // Reply Event from CallManager
#if 0 //DebugVM start
#ifdef XGATE_HAS_BUSINESS_SUPPORT
    //bool copyRequiredCallParam(XGATESIPEVENT sipevent, callInfoParameter &src, callInfoParameter &dst);
    bool copyRequiredCallParam(XGATESIPEVENT sipevent, xGateSofiaobj *sobj, xGateCallObj *callObj);
#endif
    void OnSwitchBoardCallProcess(xGateSofiaobj *sobj); // To handle IVR
    void OnConferenceCallProcess(xGateSofiaobj *sobj); // To handle IVR
#endif //DebugVM end
    void ProcessInviteRequest(nua_handle_t *nh,sip_t const  *sip,tagi_t tags[],xGateSofiaobj *sobj);// Process the incoming Request message
    bool ProcessAuthCallInviteRequest(nua_handle_t *nh,sip_t const  *sip,tagi_t tags[],xGateSofiaobj *sobj);

    bool formUnknownHeaderForAor(xGateSofiaobj *sobj,std::string &aor);

    //Added to copy SDP data based on content length
    void copyPayloadData(std::string strPayloadData, int payloadLen, std::string &strSDP);

#if 0 //DebugVM start
    virtual xGateSofiaobj* GetProcessEvent();
    void GetProcessRegisterEvent(); // Get Register event from queue for reply which is processed by DB Manager
    void RequestTermination(xGateSofiaobj *sobj); // Terminate Request to SL App on abromal condation

#endif //DebugVM end
    // Valaditing Function
    bool ValidateSipMessage(sip_t const *sip, SIPVALIDATE header); // For Inbound 
    std::string ValidateDialParam(const char *pVal, SIPVALIDATE header,XGATESIPKEY key,callInfoParameter cp);
    void parsestringtokens(std::string Input, char delimit, std::string &token1, std::string &token2);
    std::string FillHeader(url_t *url,callInfoParameter cp,SIPVALIDATE header);

    bool ProcessDialoutForMakeCall(xGateSofiaobj *sobj,xGateCallObj *callObj);
    bool VerifyAuthorisationResponce(sip_t const *sip, xGateSofiaSipRegobj *regobj,std::string reg); // Function creates MD5 Hash and test for responce tag
    bool handleErrorNotifyResponse(xGateSofiaobj *sobj, xGateCallObj *callObj);
    bool AuthenticateSipSession(int status,char const *phrase,nua_t *nua,
        nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,
        tagi_t        tags[]);

    //bool VerifyAuthorisationResponce(sip_t const *sip, xGateSofiaSipRegobj *regobj,char requestmethod[15]="REGISTER"); // Function creates MD5 Hash and test for responce tag
#if 0 //DebugVM start



    // Auth functions
    std::string Authenticating(xGateSofiaobj *sobj, sip_www_authenticate_t const *auth); // send UAC authendication 
    std::string CreateDigiestNonce();
    // Sip Header creation
    void CreateSDPParam(xGateSofiaobj *sobj);
#endif //DebugVM end
    bool DeleteSofiaObject(xGateSofiaobj *sobj);
#if 0 //DebugVM start
    bool DeleteForkedSofiaObject(xGateSofiaobj *sobj);
    // Media SDP 
    int GetCodecFromList(int payload);
#endif //DebugVM end
    int GetCodecFromList(const char* codename, int mediaType);
    void AddServerCodecToList(xGateSofiaobj *sobj);
#if 0//DebugVM start
    sdp_rtpmap_t *CheckCodecCompatablity(sdp_session_t *rsdp);
    bool CheckCodecCompatablity(xGateAVCodec *avcodec);
    bool CheckCodecCompatablity(std::vector<xGateAVCodec*> );
#endif //DebugVM end
    void SDPPharser(sdp_session_t *rsdp,xGateSofiaobj *sobj);
    void IceCandidatePharser(sdp_session_t *rsdp,xGateSofiaobj *sobj);
    void RtpAvpPharser(sdp_session_t *rsdp,xGateSofiaobj *sobj);
    void MediaInfoParserSAVPF(sdp_media_t *media, xGateSDPInfo &sdpinfo);
    void MediaInfoParserAVP(sdp_media_t *media, xGateSDPInfo &sdpinfo);
    void AttributeInfoParser(sdp_attribute_t *attribute, xGateSDPInfo &sdpinfo);
#if 0//DebugVM start
    std::string CreateG711Codec(xGateSDPInfo sdpinfo,xGateSofiaobj *sobj, XGATESDPMODE sdpmode);
#endif //DebugVM end
    std::string EncodeResponseSDP(xGateSofiaobj *sobj);
#if 0 //DebugVM start
    std::string EncodeDialSDP(xGateSofiaobj *sobj);
    std::string EncodeSessionAttribute(xGateSDPInfo audio, xGateSDPInfo video,int DeviceType);
#endif //DebugVM end
    std::string MultiCodecSAVPF(xGateSofiaobj *sobj,XGATESDPMODE sdpmode);
    std::string EncodeMediaAttribute(xGateSDPInfo sdpinfo, XGATESDPMODE sdpmode,int DeviceType);
    std::string EncodeConnectionInformation(xGateSDPInfo sdpinfo, XGATESDPMODE sdpmode,int DeviceType);
    std::string MultiCodecAVP(xGateSofiaobj *sobj,XGATESDPMODE sdpmode);
#if 0 //DebugVM start
    std::string MultiCodecSAVP(xGateSofiaobj *sobj,XGATESDPMODE sdpmode);

    //void SetSdpMediaIP_Port(sdp_session_t &dup,xGateSofiaobj *sobj);
    //void AppendSdpAttribute(sdp_session_t &dup,const char* name, const char* value,int type );
    //void AppendSdpMediaFormat(sdp_list_t &dup, const char*value,int type);
    //sdp_session_t* EncodeDialSDP(xGateSofiaobj *sobj,int payloadtype);
    //sdp_session_t* EncodeResponseSDP(xGateSofiaobj *sobj);
    //std::string EncodeDialStringSDP(xGateSofiaobj *sobj);
    //void SetSDPAnswer(sdp_session_t &dup, std::string tagname);// Just for Webrtc Peer to Peer support

    //void CreateSdpOpus(sdp_session_t &dup,xGateSofiaobj *sobj);
    //void CreateSdpGCodec(sdp_session_t &dup,xGateSofiaobj *sobj);
    // MG Communication 
    void RequestForMedia(xGateSofiaobj *sobj,XGATEMGUPDATE mgMsgType=EN_MEDIA_CREATE);
#endif //DebugVM end
    void MediaRequest(xGateSofiaobj *sobj, sip_t const  *sip, XGATEMGUPDATE mgMsgType); //DebugVM
    xGateReturn PrepareHandleAndSdpMakeCall(xGateSofiaobj *sobj,callInfoParameter cp,xGateSofiaobj *dialsobj);
#if 0 //DebugVM start
    // PBX Impementation 
    void OnHold_Resume(xGateSofiaobj *sobj);
    xGateReturn ForwardReInvite(xGateSofiaobj *sobj,  bool onhold);
    xGateReturn music_on_hold(xGateSofiaobj *sobj, bool onhold);
    xGateReturn ForwardSipInfo(xGateSofiaobj *sobj, sip_payload_t *payload);
    xGateReturn CallForking(xGateSofiaobj *sobj);
    xGateReturn CallForkingApp(xGateSofiaobj *sobj, std::string proxyusername, std::string devtype);
    xGateReturn CallListCancle(std::string key);
    xGateReturn CallDefault(xGateSofiaobj *sobj);
    xGateReturn PlayResponse(int status,xGateSofiaobj *sobj);
    xGateReturn PlayOnConnect(xGateSofiaobj* sobj, XGATEPLAYFILE fileType);
    xGateReturn CallTransfer(xGateSofiaobj* sobj,sip_t const *sip,int transferType);
    void updateCDR(xGateSofiaobj *sobj);
    // Feature functionality
    void AppDialOnWake(std::string uid);
    void OnConferenceHostJoin(xGateSofiaobj *sobj);
    void OnConferenceParticipantJoin(xGateSofiaobj *sobj);
#endif //DebugVM end
    xGateReturn dtmfHandler(const char* dtmf, xGateSofiaobj* sofiaobj );
    void LinkDstInfoWithOrgInfo(std::string uid);
#if 0 //DebugVM start
    //xGateReturn SofiaSiphandler::processHostControlCommand(xGateSofiaobj *sobj);
    void ProcessMessageRequest(sip_t const  *sip);// Process the incoming Request message
#endif //DebugVM end
};
#endif
