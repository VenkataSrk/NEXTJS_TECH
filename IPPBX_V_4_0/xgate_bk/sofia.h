/* $Header: /xGate/sofia.h
 *   
 * XGATE
 * (C) 2013 Switchlab, Ltd
 *
 * Sofia SIP signalling glue
 *
 * $Log: /xGate/sofia.h $
 * 
 * 13    6/26/13 okY
 * Initial (alpha) version, only skeleton0
 * 
 */
#ifndef __XGATE_SOFIA_H__
#define __XGATE_SOFIA_H__

#include "call.h"
#include "SignallingDriver.h"
#include "sipmsg.h"
#include "compileinfo.h"
#include "registrar.h"

extern "C"
{
  #include <sofia-sip/nua.h>
  #include <sofia-sip/sip_extra.h>
  #include <sofia-sip/sdp.h>
}

#define SOFIA_SIP_PORT 5060
#define AUTH_EXP_TIME 300
#define SOFIADRV	"sofiadrv"
#define XGATE_USER_AGENT "xGate build"COMPILE_DATE" "COMPILE_TIME" "COMPILE_HOST

#define XGATE_SOFIA_HEADER_USER_INFO "X-XG-UI"
#define XGATE_SOFIA_HEADER_PRIVATE_INFO "X-XG-PI"

// HuntGroup definition
// Max 100 RegisteredUsers can be registred to the HuntGroupNumber.
//#define MAX_HUNTGROUP_USERS_REGISTERED_TO_HUNTGROUP_NUMBER	100
#define MAX_HUNTGROUP_USERS_REGISTERED_TO_HUNTGROUP_NUMBER	10
#define MAX_CALL_INFO                                           5000

/* dandi create temporary elementary API tests via command line */
enum Sofia_Term_Cmd {
    Sofia_Term_Cmd_Set_TestAPI = 0,
    Sofia_Term_Cmd_Conf,
};

typedef enum
{
   key_exist,
   key_not_exist
}key_state;

class SofiaSIPCall;
typedef std::map <std::string, SofiaSIPCall*> SofiaCallTable;
typedef std::map<std::string,SofiaSIPCall*>::iterator SofiaCallTableIterator;

void CreateSofiaCallParam(CallParameter &cparam, sip_t const *sip, tagi_t tags[]);

typedef enum SofiaPreconditionLevel{
    PreconditionLevel_None,
    PreconditionLevel_Optional,  // also as NO in current status
    PreconditionLevel_Mandatory  // also as YES in current status
}SofiaPreconditionLevel;

#define PreconditionLevel_No PreconditionLevel_Optional
#define PreconditionLevel_Yes PreconditionLevel_Mandatory

typedef enum SofiaPreconditionType{
    PreconditionType_e2e=0,
    PreconditionType_segmented
}SofiaPreconditionType;

typedef struct SofiaPreconditionTable{
    SofiaPreconditionLevel localReceive;
    SofiaPreconditionLevel localSend;
    SofiaPreconditionLevel remoteReceive;
    SofiaPreconditionLevel remoteSend;
}SofiaPreconditionTable;

class SofiaPrecondition {
public:
    SofiaPrecondition(SofiaPreconditionLevel recvstat=PreconditionLevel_No, 
		              SofiaPreconditionLevel sendstat=PreconditionLevel_No,
                      SofiaPreconditionLevel desiredrecv=PreconditionLevel_Mandatory,
                      SofiaPreconditionLevel desiredsend=PreconditionLevel_Mandatory);

    void setSendReserveStatus(SofiaPreconditionLevel stat);
    void setReceiveReserveStatus(SofiaPreconditionLevel stat);
    void setDesiredSendReserveStatus(SofiaPreconditionLevel stat);
    void setDesiredReceiveReserveStatus(SofiaPreconditionLevel stat);

	static SofiaPrecondition* parseAndCreateSofiaPrecondition(sdp_media_t*media);

    bool parseAndUpdate(sdp_media_t* media);

    String output(bool requestConfirm=false);

    bool isPreconditionMet();
    bool isConfirmRequired();

    ~SofiaPrecondition();

private:
    SofiaPreconditionTable current;
    SofiaPreconditionTable desired;
    SofiaPreconditionTable needconfirm;
    int preconditionType; //0-e2e  1-segmented
};

///////////////////////////////////////////////////////////////////////////////
class SofiaSIPSignalling: public SignallingDriver {
public:
    static SofiaSIPSignalling * Create();
    static SofiaSIPSignalling * Instance() {
	    return instance;
    }
	bool SofiaInitNua();
#ifndef __linux__
	static void __cdecl SofiaCallback(nua_event_t   event,  
                               int           status,  
                               char const   *phrase,  
                               nua_t *nua,  
                               nua_magic_t  *magic,  
                               nua_handle_t *nh,  
                               nua_hmagic_t *hmagic,  
                               sip_t const  *sip,  
                               tagi_t        tags[]);
                               //sip_expires_t* expire_header);
#else
        static void SofiaCallback(nua_event_t   event,
                               int           status,
                               char const   *phrase,
                               nua_t *nua,
                               nua_magic_t  *magic,
                               nua_handle_t *nh,
                               nua_hmagic_t *hmagic,
                               sip_t const  *sip,
                               tagi_t        tags[]);
                               //sip_expires_t* expire_header);
#endif


    virtual bool Load();
    virtual bool Unload();
    virtual void HandleEvents();

    /** SetupReq: Initiate a new outgoing call in a specific Interface
	using the specified call parameter
     */
    virtual Call *SetupReq(Interface *itf,SelectionStrategy selection,CallParameter *cparam);
    virtual Call *SetupReq_during_call_transfer(Interface *itf,SelectionStrategy selection,CallParameter *cparam, nua_handle_t * nh);

    /** CreateDefaultCallParameter: Returns a CallParameter structure containing
	default values for this signalling protocol
     */
    virtual CallParameter *CreateDefaultCallParameter();

    /** Initialize default call parameter.
     */
    void InitDefaultCallParameter( CallParameter & cparam );

    /** AttachInterface: A new Interface object is attached to this 
	SignallingDriver object
     */
    virtual bool AttachInterface(Interface *itf);

    /** DetachInterface: Remove association between a Interface object and this 
	SignallingDriver object
     */
    virtual void DetachInterface(Interface *itf);

    /** Show service status.
     */
    virtual bool OnShowStatus(String & output);

    virtual void DoTermCmd(Term_Session *session, bool no, Term_Cmd *cmd);
    virtual bool OnShowConfig(const Cfg_Tree_Entry *entry,
				    String & output, 
				    const String & indent, 
				    int indent_size) const;

	bool HandleIncomingInvitation(int status, 
                              nua_handle_t * nh,
                              CallParameter *c_param,
                              sip_t const *sip, 
                              tagi_t tags[]);
	bool HandleIncomingNotify(int status, 
                              nua_handle_t * nh,
                              CallParameter *c_param,
                              sip_t const *sip, 
                              tagi_t tags[]);
	bool HandleIncomingRegistration(int status, 
                              nua_handle_t * nh, 
                              sip_t const*sip, 
                              tagi_t tags[]);
                              //sip_expires_t *expire_header);
	bool HandleIncomingInfo(int status, 
                              nua_handle_t * nh,
                              sip_t const *sip, 
                              tagi_t tags[], SofiaSIPCall * sCall);
	char* switch_stristr(const char *instr, const char *str);
	char switch_rfc2833_to_char(int event);
	Call *InitiateInviteCall_to_transferTarget(int status, 
                              nua_handle_t * nh,
                              sip_t const *sip, 
                              tagi_t tags[]);
	bool HandleIncomingBye(int status, 
                              nua_handle_t * nh,
                              sip_t const *sip, 
                              tagi_t tags[]);
	bool HandleIncomingActive(int status, 
                              nua_handle_t * nh,
                              sip_t const *sip, 
                              tagi_t tags[]);
	bool HandleIncomingRefer(int status, 
                              nua_handle_t * nh,
                              sip_t const *sip, 
                              tagi_t tags[]);

	bool HandleIncomingMessage(int           status,
							   char const   *phrase,
							   nua_t        *nua,
							   nua_magic_t  *magic,
							   nua_handle_t *nh,
							   nua_hmagic_t *hmagic,
							   sip_t const  *sip,
							   tagi_t        tags[]);

	static void SofiaSIPSignalling::ChallengeRequest(nua_handle_t* nh, int status, const char* realm, bool stale=false, msg_t*ref=NULL);

/* Due to prosody issues, definition is moved to sofia file */
	nua_t * GetSofiaNua()
	{
		return sofia_nua;
	}

	String GetRealm() {return defaultRealm;};

	su_home_t * GetSofiaHome(){return &sofia_home;};

        CallParameter *getCparam(){return cparam_d;}

	SofiaSIPCall *FindFromIncomingCallTable(sip_addr_t *const from, sip_call_id_t *const call_id);
	void AddToIncomingCallTable(SofiaSIPCall* call, sip_addr_t *const from, sip_call_id_t *const  call_id);
	void RemoveFromIncomingCallTable(SofiaSIPCall * call);
	
	
private:
    static SofiaSIPSignalling* instance;
    bool   requireTimer;

	String defaultRealm; 


    SofiaSIPSignalling();
    ~SofiaSIPSignalling();
    void Sbc_Outgoing_SendRegister(char *to_uri,char *from_uri,char *contact_uri);

	void SendSIPMessage(String uri, String message);

	// Sofia-NUA related vars
	su_home_t sofia_home;
	su_root_t* sofia_root;
	nua_t * sofia_nua;	
//	ACE_Semaphore _sofia_thread_events;

	// table to trace if a new invite correspond to
	// the previous invite (not in dialog).
	// this is required for overlap call setup support
	// in SIP-I
	SofiaCallTable incomingCallTable;
	
};

///////////////////////////////////////////////////////////////////////////////
class SofiaSIPCall : public Call {
public:
    SofiaSIPCall(const CallParameter & cparam, CallDirection dir, const char *itf_name);
    ~SofiaSIPCall();

    /** Requests from CallManager.
     */
    virtual bool DeviceOverlapReq(const char* additionalDigits,bool complete);
    virtual bool DeviceProceedingReq(CallProgress progress, CallExtraInfo* exinfo);
    virtual bool DeviceProgressReq(CallProgress progress, CallExtraInfo* exinfo);
    virtual bool DeviceAlertingReq(CallProgress progress, CallExtraInfo* exinfo);
    virtual bool DeviceConnectReq(CallExtraInfo* exinfo);
    virtual bool DeviceSendInfo(CallExtraInfo* exinfo);
    virtual bool DeviceSuspendReq();
    virtual bool DeviceResumeReq();
    virtual bool DeviceDisconnectReq(CallCause cause,CallProgress progress);
    virtual void SetDisconnectMsg(const char *msg);
    //virtual int  GetAuthorizationHandle();

	// interface from Sofia/signalling driver
	virtual bool OnIncomingCall();
	virtual bool OnCallEstablished();
	virtual void OnCallDisconnected();
	virtual void OnCallSuspended();
	virtual void OnCallResumed();
	virtual bool SofiaSIPCall::OnCallProceeding(int status, sip_t const *sip, CallProgress progress);
        virtual bool OnCallTransfer();
        virtual bool OnCallUnattendTransfer();
        virtual bool OnCallPark();
        virtual bool OnCallFlip();
        virtual bool OnCallUnFlip();
        virtual bool OnCallHold(int ishold);
	virtual bool OnCallRec(int isrec);

	bool checkPayloadCompatibility(sdp_rtpmap_t * rtpMap);
	void SetLocalSDPParameter(Rtp_Media*med);
    void SetLocalSDPParameter(sdp_session_s const *  sdp);
    bool IsValid() const;
    bool IsUsingVirtualRtp() const;

	void attachSofiaNuaHandle(nua_handle_t* handle);
	void SofiaStateChanged(int status,
		                   char const   *phrase,
									 sip_t const *sip,
									 tagi_t tags[]);
	void HandleReInvite(int status, const sip_t *sip, tagi_t tags[]);
	bool StartChannel(ACE_INET_Addr * addr,
				    Rtp_Payload_Code payload_code);
	bool StopChannel();
	void AuthenticateCall(int status,sip_www_authenticate_t const *auth);

	void InviteCall(bool isHold=false, bool isResume=false);

	int SetCallCredential(CallCredential& credential);

	bool RequireAuthorization();
	void SetPrecondition(SofiaPrecondition*precond);

	void OverlapNuaHandle(int status, nua_handle_t*nh, sip_t const *sip, tagi_t tags[]);
	void MarkUpdate(){_update_received=true;};

        void requestSessionTimer(bool _timer){timerRequested=_timer;};

    friend Call * SofiaSIPSignalling::SetupReq(Interface *itf,SelectionStrategy selection,CallParameter *cparam);
    friend Call * SofiaSIPSignalling::SetupReq_during_call_transfer(Interface *itf,SelectionStrategy selection,CallParameter *cparam, nua_handle_t* nh);
	friend void SofiaSIPSignalling::AddToIncomingCallTable(SofiaSIPCall* call, sip_addr_t *const from, sip_call_id_t *const  call_id);
	friend void SofiaSIPSignalling::RemoveFromIncomingCallTable(SofiaSIPCall * call);
	int recv_offer_ptime;
        int recv_answer_ptime;


private:
    enum { SIGNATURE = 0x864BC52 };

    unsigned    signature1;
    
    // copy from xgsipcall
	bool	disconnectReq;
    CallCause	disconnectReqCause;
	CallCause  lastKnownCause;
    bool	cmIsNotified;
    String	disconnectMessage;
    unsigned	signature2;

    SofiaSIPCall	*peerSipCall;

    nua_handle_t*nua_handle;
	bool channelIsStarted;
	ACE_INET_Addr* remoteMediaAddr;
	Rtp_Payload_Code activePayloadCode;
	int		rfc2833_rx_pt, rfc2833_tx_pt;

	bool hasSipAuth;
        XgSipAuthEntry sipAuth;

	// required for virtual RTP
	sdp_session_s * savedRemoteSdp;
	
	// credential for anticipating authentification challenge
	// during outgoing requests
	bool hasCredential;
	CallCredential callCredential;

	int sofiaCallState;

	// to speed up incoming call table access
	SofiaCallTableIterator callTableIterator;

    // precondition state
    SofiaPrecondition * _precondition;

    // quick hack to tell whether it was INVITE or UPDATE
    // received
    bool _update_received;

    // timer is requested

    bool timerRequested;
};

/*
    Rule regarding registration:

  - src_addr and src_id contains the information about person (id) and address
    who is performing the registration. The value normally should be the same as
    the address of record, unless in third party registration.

  - reg_rec_id and reg_rec_addr is the address of record being queried/modified
    by the registration process.

  - regs contains array of contacts to be associated with the address of record.
    It is sorted by priority (higher first). For unregistration, this array is
    empty.

  - request_id is the request identification in the request.

  - has_auth and auth is the authorization information.
 */

///////////////////////////////////////////////////////////////////////////////
/*
 * SOFIA Registrar Call. 
 */
class SofiaRegisterRequest
{
public:
	SofiaRegisterRequest(nua_handle_t* nh);
	~SofiaRegisterRequest();
//	void RegistrationHandling(sip_t const *sip,tagi_t tags[],msg_t * msg,sip_expires_t *expire_header);
	void RegistrationHandling(sip_t const *sip,tagi_t tags[],msg_t * msg);
	msg_t * GetRegisterRequest(){return register_msg_;};
        nua_handle_t * GetNuaHandle(){return nua_handle_;};
	const sip_t * GetSofiaSip(){return sip_;};
	bool  IsValid() const;

	


private:
	enum { SIGNATURE = 0xA1B2C3D5 };
	nua_handle_t * nua_handle_;
	sip_t const * sip_;
 //       sip_expires_t * sipexpiry_;
	msg_t*   register_msg_;
    unsigned	    signature_;
    time_t	    start_time_;
    int		    last_answer_;
};

// new struct RegisterParameter will store user_id, domain, password, etc. required during registration process
// it will hold data specific to each user_id.
struct RegisterParam
{
	// following 10 are INPUT paramenters into database.
	// API names for these input parameters will start from prefix "Set".
	char *SipUri;
	char *ToAddr;
	char *Token;
	char *ContactUser;
	char* NetworkPort;
	char *ContactHost;
	unsigned int ExpireTime;
	char *NetworkProtocol;
	char *NetworkIP;
	bool HasAuth;
	//int extension;

	// following 7 are OUTPUT paramenters coming from database.
	// API names for these OUTPUT parameters will start from prefix "Populate".
	//char Passwd[500];
	String Passwd;
	bool Status;
	char* AuthScheme;
	char* Realm;
	char* Nounce;
	char* Qop;
	char* Algorithm;

	// global call-transfer variables start
	char *CallTransferExtension;
	bool CallTransferExtensionSent;
	// global call-transfer variables end

	// global hunt-group variables start
	char *HuntGroupNumber;
	char *HuntGroupCaller;
       	int  NoOfRegisteredUsers;
        
	string HuntGroupRegisteredUser[MAX_HUNTGROUP_USERS_REGISTERED_TO_HUNTGROUP_NUMBER];

	bool HuntGroupRegisteredUser_answered_call_sent;
	char *HuntGroupRegisteredUser_answered_call;

	int  CallLegOfRegisteredUser_answered_call;
	// global hunt-group variables end
};

//Rajesh, 27th july 2015

class RegistrationApi : public CAPIProvider
{
   	//SofiaRegistrar* _manager;
   	RegistrationApi* _manager;

public:
    RegistrationApi(int providerId,const char* name);

	// following 10 are INPUT paramenters into database.
	// API names for these input parameters will start from prefix "Get".
	const char *SipUri;
	const char *ToAddr;
	const char *Token;
	const char *ContactUser;
	const char* NetworkPort;
	const char *ContactHost;
	unsigned int ExpireTime;
	char *NetworkProtocol;
	char *NetworkIP;
	bool HasAuth;
	//int extension;

	// following 7 are OUTPUT paramenters coming from database.
	// API names for these OUTPUT parameters will start from prefix "Populate".
	String Passwd;
	bool Status;
	char* AuthScheme;
	char* Realm;
	char* Nounce;
	char* Qop;
	char*  Algorithm;

	const char *CallTransferExtension;
	bool CallTransferExtensionSent;

	bool SwitchBoard200_OK_received;
	String SwitchBoard_Number;
	String SwitchBoard_called_extension;
	bool is_SwitchBoard_Call;
	char* SwitchBoardExtension;

	// global hunt-group values
	char *HuntGroupNumber;
	char *HuntGroupCaller;
	int  NoOfRegisteredUsers;
	string HuntGroupRegisteredUser[MAX_HUNTGROUP_USERS_REGISTERED_TO_HUNTGROUP_NUMBER];

	bool HuntGroupRegisteredUser_answered_call_sent;
	char *HuntGroupRegisteredUser_answered_call;

	int  CallLegOfRegisteredUser_answered_call;
protected:
    bool ValidRegistrationParameter(CTask* task,RegisterParam* rparam,const char* funcname);

    /** Register parameter related
     */
    //STATUS API_CALL GetCallParam(CTask* task,CallParameter** cparam /*retval*/,int handle);
    //STATUS API_CALL CreateDefaultCallParam(CTask* task,CallParameter** cparam /*retval*/,const char* portId);


/**************
	// following 10 are INPUT paramenters into database.
	// API names for these input parameters will start from prefix "Get".
	const char *sipUri;
	const char *toAddr;
	const char *token;
	const char *contactUser;
	const char *networkPort;
	const char *contactHost;
	unsigned int expireTime;
	const char *networkProtocol;
	const char *networkIp;
	bool hasAuth;

	// following 7 are OUTPUT paramenters coming from database.
	// API names for these OUTPUT parameters will start from prefix "Populate".
	const char *password;
	bool status;
	const char* authScheme;
	const char* realm;
	const char* nounce;
	const char*  algorithm;
	const char* qop;

   SofiaRegistrar* registerManager;
   const char *sipUri;
   const char *toAddr;
   const char *sipToken;
   const char *ContactUsr;
   const char* networkPort;
   //unsigned int networkPort;
   const char *contactHost;
   unsigned int expireTime;
   const char *networkIp;
   const char *networkProto;

   bool hasAuth;
   const char *r_password;
   bool status;
   const char* authScheme;
   const char* realm;
   const char* nounce;
   const char*  algorithm;
   const char* qop;
*****************/

public:
    char* getsipUri();
    void setsipUri(const char* m_sipUri);

    char* getToAddr();
    void setToAddr(const char* m_ToAddr);

    char* getSipToken();
    void setSipToken(const char* m_SipToken);

    char* getContactUsr();
    void setContactUsr(const char *m_ContactUsr);

    char* getNetworkPort();
    void setNetworkPort(const char* m_NetworkPort);
    /*int getNetworkPort();
    void setNetworkPort(int m_NetworkPort);*/

    char* getHostName();
    void setHostName(const char *m_HostName);

    unsigned int getUsrExpiration();
    void setUsrExpiration(unsigned int m_expireTime);

    char* getNetworkIp();
    void setNetworkIp(const char *m_NetworkIp);

    char* getNetworkProto();
    void setNetworkProto(const char *m_NetworkProto);

    bool getHasAuth();
    void setHasAuth(bool m_hasAuth);

    bool getStatus();
    void setStatus(bool m_status);

    char* getPassword();
    void setPassword(const char *m_password);

    char* getAuthScheme();
    void setAuthScheme(const char *m_authScheme);

    char* getRealm();
    void setRealm(const char *m_realm);

    char* getNounce();
    void setNounce(const char *m_nounce);

    char* getQop();
    void setQop(const char *m_qop);

    char* getAlgorithm();
    void setAlgorithm(const char *m_algorithm);

    RegistrationApi();
    //RegistrationApi(int providerId,const char* name);
    RegistrationApi(RegistrationApi* var);
    RegistrationApi(RegistrationApi* var_manager, int providerId, const char* name);
    virtual ~RegistrationApi()
    {
    }
	RegistrationApi* GetRegistrationApiParam();
	//STATUS RegistrationApi::GetRegistrationParam(CTask*, RegistrationApi**, int);

	//void process_passwd(Reg_Info info, char* method);

protected :
    	bool ValidRegisterParameter(CTask* task,RegisterParam* rparam,const char* funcname);

	// 10 Input parameters sending data into database will have prefix as "Get".
	STATUS API_CALL API_GetSipUri(CTask* task, char* retval, RegisterParam* rparam);
	STATUS API_CALL API_GetToAddr(CTask* task,  char* retval, RegisterParam* rparam);
	STATUS API_CALL API_GetToken(CTask* task,  char* retval, RegisterParam* rparam); //storing as int whereas in DB data is string
	STATUS API_CALL API_GetContactUser(CTask* task,  char* retval, RegisterParam* rparam);
	STATUS API_CALL API_GetNetworkPort(CTask* task,  char* retval, RegisterParam* rparam);
	STATUS API_CALL API_GetContactHost(CTask* task,  char* retval, RegisterParam* rparam);
	STATUS API_CALL API_GetExpireTime(CTask* task,  unsigned int retval, RegisterParam* rparam);
	STATUS API_CALL API_GetNetworkProtocol(CTask* task,  char* retval, RegisterParam* rparam);
	STATUS API_CALL API_GetNetworkIP(CTask* task,  char* retval, RegisterParam* rparam);
	STATUS API_CALL API_GetHasAuth(CTask* task,  bool* retval, RegisterParam* rparam);

	// 7 Output parameters fetching data from database will have prefix as "Populate".
	STATUS API_CALL API_PopulatePasswd(CTask* task,  RegisterParam* rparam, char* value);
	STATUS API_CALL API_PopulateStatus(CTask* task,  RegisterParam* rparam, bool value);
	STATUS API_CALL API_PopulateAuthScheme(CTask* task,  RegisterParam* rparam, char* value);
	STATUS API_CALL API_PopulateRealm(CTask* task,  RegisterParam* rparam, char* value);
	STATUS API_CALL API_PopulateNounce(CTask* task,  RegisterParam* rparam, char* value);
	STATUS API_CALL API_PopulateQop(CTask* task,  RegisterParam* rparam, char* value);
	STATUS API_CALL API_PopulateAlgorithm(CTask* task,  RegisterParam* rparam, char* value);

    #ifndef CALL_TRANSFER
        STATUS API_CALL API_GetCallTransferExtension(CTask* task, char* retval, RegisterParam* rparam);
    #else
      //  STATUS API_CALL API_Delete_hg_data(CTask* task, int cparam);
        STATUS API_CALL API_Delete_hg_data(CTask* task, long cparam);// changes made for migration from 32 bit to 64 bit

     //   STATUS API_CALL RegistrationApi::API_SetConfHostUser(CTask* task, int cparam, char* host_number);
        STATUS API_CALL RegistrationApi::API_SetConfHostUser(CTask* task, long cparam, char* host_number);//changes made for migration from 32 bit to 64 bit
        //STATUS API_CALL RegistrationApi::API_DialIn_Participant_RegisteredUser(CTask* task, int cparam, int handle, char* DialInParticipantUser, int max_dialin_count, char* user_A);
 //STATUS API_CALL RegistrationApi::API_DialIn_Participant_RegisteredUser(CTask* task, long cparam, int  handle, char* DialInParticipantUser, int max_dialin_count, char* user_A);//changes made for migration for migration from 32 bit to 64 bit
 STATUS API_CALL RegistrationApi::API_DialIn_Participant_RegisteredUser(CTask* task, long cparam, int  handle, char* DialInParticipantUser, int max_dialin_count);//changes made for migration for migration from 32 bit to 64 bit

        //STATUS API_CALL RegistrationApi::API_Delete_conf_host_data(CTask* task, int cparam);
	 STATUS API_CALL RegistrationApi::API_Delete_conf_host_data(CTask* task, long cparam);//changes made for migration from  32 bit to 64 bit
       // STATUS API_CALL RegistrationApi::API_Clear_Dialin_data(CTask* task, int cparam, int handle);
         STATUS API_CALL RegistrationApi::API_Clear_Dialin_data(CTask* task, long cparam, long handle);//changes made for migration from 32 bit to 64 bit
       // STATUS API_CALL RegistrationApi::GetDialInDiscLeg(CTask *task, long *retval, int cparam);
        STATUS API_CALL RegistrationApi::GetDialInDiscLeg(CTask *task, long *retval, long cparam);//changes made for migration from 32 bit t0 64 bit

        STATUS API_CALL API_DelSuspendUsr(CTask* task, char *key);
        STATUS API_CALL API_GetSuspendUsr(CTask* task, char *retval, char *key);
        STATUS API_CALL RegistrationApi::API_UpdateMap(CTask* task, char *inputkey, char *value, int update_key);
        STATUS API_CALL RegistrationApi::API_UpdateCallParkMap(CTask* task, char *inputkey, char *value, int update_key);
        STATUS API_CALL RegistrationApi::API_GetCallParkId(CTask* task, int *retval);

        STATUS API_CALL API_GetCallTransferExtension(CTask* task, char* retval, char* bleg);
        STATUS API_CALL API_GetCallParkCode(CTask* task, char* retval, char* bleg);
        //STATUS API_CALL API_GetCallTransferReplaces(CTask* task, char* retval, char* key);
    #endif
	STATUS API_CALL API_GetCallTransferExtensionSent(CTask* task, bool* retval, RegisterParam* rparam);

	STATUS API_CALL API_SetHuntGroupNumberAndCaller(CTask* task, int cparam, RegisterParam* rparam, char* hunt_group_number, char* user_A);
	STATUS API_CALL API_GetHuntGroupNumber(CTask* task, char* retval,int cparam, RegisterParam* rparam,  char* user_A);
        // karthi - HG 01072016 - Modified below API API_SetHuntGroupCalledRegisteredUser to receive no_of_registered_hg_users from SL
	//STATUS API_CALL API_SetHuntGroupCalledRegisteredUser(CTask* task, int cparam, RegisterParam* rparam, char* hunt_group_registered_user, char* user_A);
    //    STATUS API_CALL API_SetHuntGroupCalledRegisteredUser(CTask* task, int cparam, RegisterParam* rparam, char* hunt_group_registered_user, int no_of_registered_hg_users, char* user_A);
 //   STATUS API_CALL API_SetHuntGroupCalledRegisteredUser(CTask* task, int cparam, int handle, char* hunt_group_registered_user, int no_of_registered_hg_users, char* user_A);
    STATUS API_CALL API_SetHuntGroupCalledRegisteredUser(CTask* task,long cparam, long handle, char* hunt_group_registered_user, int no_of_registered_hg_users);//changes made for migration from 32 bit to 64 bit


	STATUS API_CALL API_GetHuntGroupRegisteredUser_answered_call_sent(CTask* task, bool* retval, int cparam, RegisterParam* rparam, char* user_A);
	STATUS API_CALL API_GetHuntGroupRegisteredUser_answered_call(CTask* task, char* retval, int cparam, RegisterParam* rparam, char* user_A);
	STATUS API_CALL API_GetCallLegOfRegisteredUser_answered_call(CTask* task, int* retval, long cparam, RegisterParam* rparam,  char* user_A);
	STATUS API_CALL API_GetCallingSignalAddress_answered_call(CTask* task, char* retval, long cparam, int legval);
	STATUS API_CALL API_SetHuntGroupCallingSignalAddress(CTask* task, long cparam, int handle, char* callingsigaddress);
	STATUS API_CALL API_GetCalledSignalAddress_answered_call(CTask* task, char* retval, long cparam, int legval);
	STATUS API_CALL API_SetHuntGroupCalledSignalAddress(CTask* task, long cparam, int handle, char* calledsigaddress);

	STATUS API_CALL API_GetMyMediatrixIP(CTask* task, char* retval, char* keyparam);
        STATUS API_CALL API_RemoveMyMediatrixMapEntry(CTask* task, char* keyparam);

	STATUS API_CALL API_GetSwitchBoard200_OK_received(CTask* task, bool* retval, CallParameter* cparam);
	STATUS API_CALL API_GetSwitchBoardExtension(CTask* task, char* retval, RegisterParam* rparam);
};

class CallTransferApi : public CAPIProvider
{
   	//SofiaRegistrar* _manager;
   	CallTransferApi* _manager;

public:
    	CallTransferApi(int providerId,const char* name);

	// following 1 are INPUT paramenters into database.
	// API names for these input parameters will start from prefix "Get".
	const char *dialedDigits;

	// following 0 are OUTPUT paramenters coming from database.
	// API names for these OUTPUT parameters will start from prefix "Populate".

    	CallTransferApi();
    	CallTransferApi(CallTransferApi* var);
    	CallTransferApi(CallTransferApi* var_manager, int providerId, const char* name);
    	virtual ~CallTransferApi()
    	{
    	}

	CallTransferApi* GetCallTransferApiParam();

protected:
    	//bool ValidRegistrationParameter(CTask* task,RegisterParam* rparam,const char* funcname);

	// 1 Input parameters sending data into database will have prefix as "Get".
	STATUS API_CALL API_GetDialedDigits(CTask* task, char* retval);
};

//////

///////////////////////////////////////////////////////////////////////////////
/*
 * SOFIA Registrar Service. 
 * (attached to Sofia Signalling Driver)
 */
class SofiaRegistrar : public ServiceProvider
{
public:
    friend class RegisterAPI;
    SofiaRegistrar();
    ~SofiaRegistrar();
	static SofiaRegistrar *Instance(){return instance;};
	
    virtual bool Load();
    virtual bool CommitConfiguration();
    virtual bool Unload();
    virtual void HandleEvents();
    virtual bool OnShowStatus(String & output);

    void	  AddCall( SofiaRegisterRequest*);
    void	  RemoveCall( SofiaRegisterRequest*);
	
	void IncomingRegistration(SofiaRegisterRequest *request);
	String CreateChallenge();
	int VerifyChallenge(String& Nonce);

	void AddRegistration( SofiaRegisterRequest*);
        void RemoveRegistration( SofiaRegisterRequest*);
	void CancelRegistration(SofiaRegisterRequest *req,Sofia_Reg_Info);
        bool AnswerRegistration(SofiaRegisterRequest *req,Sofia_Reg_Info,int status);
        // Rajesh added to support expire header in Register message
        bool authResponse(char *username, char *realm,char *passwd,char *nonce,char *nc, char *cnonce, char *qop, const char* regstr,char *authUri,char *authRes);
	RegistrationApi* GetRegistrationApiParam() { return _registrationApi;}
	CallTransferApi* GetCallTransferApiParam() { return _callTransferApi;}
	
private:

	char auth_str[1024];	
	//RegistrationApi* _registrationApi = new RegistrationApi();
	RegistrationApi* _registrationApi;
	CallTransferApi* _callTransferApi;

	/////RegistrationApi* rparam;
	//friend void RegistrationApi::process_passwd(Reg_Info info, char* method);

	String CreateNonce();
	static SofiaRegistrar *instance;
	enum { CHECK_INTERVAL = 1, CALL_TIMEOUT_INTERVAL = 30 };
    time_t  time_to_check_;
    std::set<SofiaRegisterRequest*> calls_;
    unsigned total_requests_, total_answered_, total_success_, total_timeout_;
};

/* Narasimham commented */
/* 
class authDetails
{
private:
char *authScheme;
char *realm;
char *nounce;
char *algo;
char *qop;
int user_ID;
char *fromUser;
char *fromUser1;
char *fromUser2;
public:
public:
authDetails()
{
authScheme=new char[100];
memcpy(authScheme,"Digest",strlen("Digest"));
realm=new char[100];
memcpy(realm,"fsdevop2.uk01.com",strlen("fsdevop2.uk01.com"));
nounce=new char[100];
memcpy(nounce,"219354c8-39ef-11e5-aafd-5f3fafc8f1e6",strlen("219354c8-39ef-11e5-aafd-5f3fafc8f1e6"));
algo=new char[30];
memcpy(algo,"",strlen("219354c8-39ef-11e5-aafd-5f3fafc8f1e6"));
qop=new char[30];
memcpy(qop,"auth",strlen("auth"));
user_ID=0;
fromUser=new char[30];
memcpy(fromUser,"405",strlen("405"));
fromUser1=new char[30];
memcpy(fromUser1,"431",strlen("431"));
fromUser2=new char[30];
memcpy(fromUser2,"271",strlen("271"));
}
restAll()
{
 memset(authScheme,'\0',100);
 memset(realm,'\0',100);
 memset(nounce,'\0',100);
 memset(algo,'\0',30);
 memset(qop,'\0',30);
 memset(fromUser,'\0',30);
 memset(fromUser1,'\0',30);
 memset(fromUser2,'\0',30);
}
char *getAuthScheme()
{
return authScheme;
}

char *getRealm()
{
return realm;
}

char *getNounce()
{
return nounce;
}

char *getAlgo()
{
return algo;
}
char *getQop()
{
return qop;
}
int getUserID()
{
return user_ID;
}
char* getFromUser()
{
return fromUser;
}
char* getFromUser1()
{
return fromUser1;
}
char* getFromUser2()
{
return fromUser2;
}
};
*/

/*************************
// new struct CallConnectParam will store calledNumber and its binding required during establishing call between 2 users.
// it will hold data specific to each user_id or calledNumber.
struct CallConnectParam
{
	// following 1 are INPUT paramenters into database.
	// API names for these input parameters will start from prefix "Set".
	const char *CalledNumber;

	// following 1 are OUTPUT paramenters coming from database.
	// API names for these OUTPUT parameters will start from prefix "Populate".
	const char *Binding;
};
************************/
//////

#endif	/* __XGATE_SOFIA_H__ */
