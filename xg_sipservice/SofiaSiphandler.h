#ifndef SOFIA_SIP_HANDLER
#define SOFIA_SIP_HANDLER

//c++ includes
#include <iomanip>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <bits/stdc++.h>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <new>
#include <sstream>

//local includes
#include "xGateSipController.h"
#include "xGateUtil.h"
#include "xGateSipMsg.h"
#include "../xg_iurmodule/IURDefines.h"
#include "../inc/UREngineConfig.h"
#include "xglog.h"

//stack includes 
extern "C"
{
#include <sofia-sip/url.h>
#include <sofia-sip/nua.h>
#include <sofia-sip/sip.h>
#include <sofia-sip/sdp.h>
#include <sofia-sip/soa.h>
#include <sofia-sip/tport.h>
#include <sofia-sip/soa_tag.h>
#include <sofia-sip/sip_tag.h>
#include <sofia-sip/sip_header.h>
#include <sofia-sip/sip_status.h>
#include <sofia-sip/msg_addr.h>
#include <sofia-sip/su_log.h>
#include <sofia-sip/nua_tag.h>
#include <sofia-sip/sip_extra.h>
#include <sofia-sip/auth_module.h>
#include <sofia-sip/auth_plugin.h>
}

#define RETCALLBACK static void

// Added for Attend Transfer
class SofiaSiphandlerReplaceHeaderParser
{
public:
	char *token;
	char *str;
	char *rest;
	char *token2;
	// int counter=0;
	char *str2;
	char *replaceHdrFrmTag;
	char *FrmTagValue;
	char *replaceHdrToTag;
	char *toTageValue;
	char *replaceHerIP;
	char *replaceHdrCallID;
	char *remote_callid;

	SofiaSiphandlerReplaceHeaderParser()
	{
		/*Initialize Value to nullptr*/
		token = nullptr;
		str = nullptr;
		rest = nullptr;
		token2 = nullptr;
		replaceHdrFrmTag = nullptr;
		FrmTagValue = nullptr;
		replaceHdrToTag = nullptr;
		toTageValue = nullptr;
		replaceHerIP = nullptr;
		replaceHdrCallID = nullptr;
		remote_callid = nullptr;
	}
	char *parseReplaceHeader(const char *);

	~SofiaSiphandlerReplaceHeaderParser()
	{
	}
};

class SofiaSiphandler : public xGateSipController
{
public:
	SofiaSiphandler();
	~SofiaSiphandler();
	// virtual ~SofiaSiphandler();

	// Sofia Libary Initializing function
	virtual xGateReturn SipLibInit();	// Create the intializing of Sofia SIP
	virtual xGateReturn SipLibLoad();	// Loding SIP Library & creating call back
	virtual xGateReturn SipLibUnLoad(); // Releasing Library
	virtual xGateReturn RunEventLoop(); // Get Message Queue by polling method

	// Sofia Sip Response function
	virtual xGateReturn RespondWith1xx(xGateSofiaobj *pSipMgc); // Provisional Response
	virtual xGateReturn RespondWith2xx(xGateSofiaobj *pSipMgc); // Succefull Response
	virtual xGateReturn RespondWith3xx(xGateSofiaobj *pSipMgc); // Redirectional Response
	virtual xGateReturn RespondWith4xx(xGateSofiaobj *pSipMgc); //  Client Failure Response
	virtual xGateReturn RespondWith5xx(xGateSofiaobj *pSipMgc); // Server Failure Response
	virtual xGateReturn RespondWith6xx(xGateSofiaobj *pSipMgc); // Global Failure REsponse

	// Registart Response Function
	virtual xGateReturn RegistarResponse(xGateSofiaSipRegobj *regobj); // reply on registart request sending 401
	void SendRegister();
	void SendDeRegister(char *to_uri, char *from_uri, char *contact_uri);
	void SendRefreshRegister(char *to_uri, char *from_uri, char *contact_uri);
	bool ValidateSipMsgPointer(const char *p);
	bool handleRegResponse(xGateSofiaSipRegobj *regobj);
	bool handleRegResponseError(xGateSofiaSipRegobj *regobj);

private:
	// member variable
	// Memory home type.
	su_home_t m_homeMemory;
	// Root object type.
	su_root_t *m_root;
	// NUA agent.
	nua_t *m_nuaHandle;
	// Common data for authentication module
	auth_mod_t *mod;

	// Member Functions
	void SofiaInit();
	void SofiaDeInit();
	void SofiaInitMemory(su_home_t &);
	void SofiaDeInitMemory(su_home_t &);
	void SofiaRootStep(nua_t *sua_handle, su_root_t *nuaRoot);
	void SofiaRootRun(su_root_t *nuaRoot);
	su_root_t *SofiaInitRoot(su_root_magic_t *appInitance);
	void SofiaDeInitRoot(su_root_t *nuaRoot);
	auth_mod_t *SofiaInitAuth(su_root_t *nuaroot);
	void SofiaDeInitAuth(auth_mod_t *authmod);
	nua_t *SofiaCreate(su_root_t *root, nua_callback_f callback, nua_magic_t *magic);
	void SofiaSetParam(nua_t *nuaHandle);
	void SofiaShutdown(nua_t *nuaHandle);
	void SofiaSetLog(int level);
	void SofiaDestory(nua_t *nuaHandler);
	void SofiaHandleDestroy(nua_handle_t *nh);

	void SofiaRespond(nua_handle_t *nh, int status, char const *phrase);
	void SofiaRespond(nua_handle_t *nh, int status, char const *phrase, tagi_t tag[]);
	void SofiaRespond(nua_handle_t *nh, int status, char const *phrase, nua_saved_event_t const *savedEvent);
	void SofiaRespondWithSavedEvent(nua_handle_t *nh, int status, char const *phrase);
	nua_saved_event_t *CreateSavedEvent();
	void DestroySavedEvent(nua_saved_event_t *saved);
	void SofiaSendAck(nua_handle_t *nh);
	void SofiaSendInvite(nua_handle_t *nh);
	void SofiaSendBye(nua_handle_t *nh);
	void auth_MD5(nua_handle_t *nh, sip_t const *sip);
	void SofiaBindhandle(nua_handle_t *nh, nua_hmagic_t *sofiaobjhandle);
	void SofiaReBindhandle(nua_handle_t *Old_nh, nua_handle_t *New_nh, nua_hmagic_t *sofiaobjhandle);
	url_t *SofiaCreateMake(su_home_t &su_home, const char *dialstr);
	static void SofiaLoggerCallback(void *stream, char const *fmt, va_list ap);

	// DebugVM end
	void GetProcessRegisterEvent(); // Get Register event from queue for reply which is processed by DB Manager
	void GetProcessEvent();			// Get Invite event from queue
	void OnProcessEvent(xGateSofiaobj *sobj, xGateCallObj *callObj);
	void OnProcessEvent(xGateCallObj *callObj);

	nua_t *GetAppContext()
	{
		return m_nuaHandle;
	}
	RETCALLBACK SofiaCallBack(nua_event_t event, int status, char const *phrase, nua_t *nua,
							  nua_magic_t *magic, nua_handle_t *nh, nua_hmagic_t *hmagic, sip_t const *sip,
							  tagi_t tags[]);
	// Event Action functionality
	void OnStateChange(int status, char const *phrase, nua_t *nua,
					   nua_magic_t *magic, nua_handle_t *nh, nua_hmagic_t *hmagic, sip_t const *sip,
					   tagi_t tags[]);
	bool OnRegistrationRequest(int status, char const *phrase, nua_t *nua,
							   nua_magic_t *magic, nua_handle_t *nh, nua_hmagic_t *hmagic, sip_t const *sip,
							   tagi_t tags[]);
	void OnInviteRequest(int status, char const *phrase, nua_t *nua,
						 nua_magic_t *magic, nua_handle_t *nh, nua_hmagic_t *hmagic, sip_t const *sip,
						 tagi_t tags[]);
	void OnInviteCancel(int status, char const *phrase, nua_t *nua,
						nua_magic_t *magic, nua_handle_t *nh, nua_hmagic_t *hmagic, sip_t const *sip,
						tagi_t tags[]);
	void OnInfoRequest(int status, char const *phrase, nua_t *nua,
					   nua_magic_t *magic, nua_handle_t *nh, nua_hmagic_t *hmagic, sip_t const *sip,
					   tagi_t tags[]);
	void OnByeRequest(int status, char const *phrase, nua_t *nua,
					  nua_magic_t *magic, nua_handle_t *nh, nua_hmagic_t *hmagic, sip_t const *sip,
					  tagi_t tags[]);
	void OnNotifyRequest(int status, char const *phrase, nua_t *nua,
						 nua_magic_t *magic, nua_handle_t *nh, nua_hmagic_t *hmagic, sip_t const *sip,
						 tagi_t tags[]);
	void OnOptionsRequest(int status, char const *phrase, nua_t *nua,
						  nua_magic_t *magic, nua_handle_t *nh, nua_hmagic_t *hmagic, sip_t const *sip,
						  tagi_t tags[]);
	void OnAckRequest(int status, char const *phrase, nua_t *nua,
					  nua_magic_t *magic, nua_handle_t *nh, nua_hmagic_t *hmagic, sip_t const *sip,
					  tagi_t tags[]);
	void OnReferRequest(int status, char const *phrase, nua_t *nua, nua_magic_t *magic, nua_handle_t *nh,
						nua_hmagic_t *hmagic, sip_t const *sip, tagi_t tags[]);
	void OnRegisterResponse(int status, char const *phrase, nua_t *nua,
							nua_magic_t *magic, nua_handle_t *nh, nua_hmagic_t *hmagic, sip_t const *sip,
							tagi_t tags[]);
	void OnRequestResponse(int status, char const *phrase, nua_t *nua,
						   nua_magic_t *magic, nua_handle_t *nh, nua_hmagic_t *hmagic, sip_t const *sip,
						   tagi_t tags[]);
	void OnOptionsResponse(int status, char const *phrase, nua_t *nua,
						   nua_magic_t *magic, nua_handle_t *nh, nua_hmagic_t *hmagic, sip_t const *sip,
						   tagi_t tags[]);

						
    bool handleOptionsResponse(int status, xGateSofiaobj *sobj);
	void RespondToPeer(int status, xGateSofiaobj *sobj);
	void SofiaSendCancel(nua_handle_t *nh);
	void ResponseWithReasonHeader(nua_handle_t *nh,int status, char const *phrase , const char *reson_header);

	// Initializing Function
	bool LoadSipHandlerParams();
	bool CreateCallobjparam(xGateCallObj *callobj, xGateSofiaobj *sobj, nua_handle_t *nh,
							sip_t const *sip, tagi_t tags[]);

	xGateSofiaobj *CreateSofiaobjparam(std::string callid, nua_handle_t *nh,nua_magic_t *magic, 
										sip_t const *sip,CALL_TYPE callType);
	bool CreateSofiaRegobj(xGateSofiaSipRegobj &regparam, sip_t const *sip);
	xGateSofiaobj *CreateSofiaOptionsobj(std::string callid, nua_handle_t *nh,
										 nua_magic_t *magic, sip_t *siptag);
	void SofiaReadAuthHeader(sip_authorization_t *ptAuthorization,AuthHeader &authHeader);
	bool SetcallInfoParameter(callInfoParameter &cp, sip_t const *sip, tagi_t tags[]);
	nua_handle_t *CreateSofiaDialhandle(callInfoParameter cp, sip_t const *siptag, std::string &sTransProto, std::string &sRouteIP,std::string &sContact,std::string call_info);
	void SofiaSendMessageRequest(xGateSofiaSipRegobj *RegObj);
	void SofiaSendOptionsRequest(xGateSofiaobj *sobj);
	void ProcessInviteRequest(nua_handle_t *nh, sip_t const *sip, tagi_t tags[], xGateSofiaobj *sobj); // Process the incoming Request message
	/*LWSS Start*/
	bool handleClientRegRequestWOAuth(nua_handle_t *nh, sip_t const *sip);
	bool handleClientRegRequestWithAuth(nua_handle_t *nh, sip_t const *sip, nua_magic_t *magic, nua_hmagic_t *hmagic);
	bool handleMSTeamsInviteRequest(nua_handle_t *nh, sip_t const *sip,tagi_t tags[],nua_magic_t *magic, nua_hmagic_t *hmagic);
    	bool handleInviteRequest(std::string callId, nua_handle_t *nh, sip_t const *sip, nua_magic_t *magic, nua_hmagic_t *hmagic,tagi_t tags[]);
	bool handleClientInviteRequestWOAuth(std::string callId, nua_handle_t *nh, sip_t const *sip);
	bool handleClientInviteRequestWithAuth(std::string callId, nua_handle_t *nh, sip_t const *sip, nua_magic_t *magic, nua_hmagic_t *hmagic);
   	bool handleServerInviteRequest(std::string callId, nua_handle_t *nh, sip_t const *sip,tagi_t tags[],nua_magic_t *magic, nua_hmagic_t *hmagic);
	bool handleInboundPstnInviteRequest(std::string callId, nua_handle_t *nh, sip_t const *sip,tagi_t tags[],nua_magic_t *magic, nua_hmagic_t *hmagic);
	bool handleInviteRespFromCtrl(xGateSofiaobj *sobj,xGateCallObj *callObj);
	bool handleInviteInboundPstnRespFromCtrl(xGateSofiaobj *sobj,xGateCallObj *callObj);
	bool handleInviteMSTeamsRespFromCtrl(xGateSofiaobj *sobj,xGateCallObj *callObj);
	bool handleInviteRespErrFromCtrl(xGateSofiaobj *sobj,xGateCallObj *callObj);
	bool handleInviteInboundPstnRespErrFromCtrl(xGateSofiaobj *sobj,xGateCallObj *callObj);
	bool handleOptionsRequestFromMonitorService(xGateCallObj *callObj);
	void ProcessWssInviteRequest(nua_handle_t *nh, sip_t const *sip, tagi_t tags[], std::string callidstring, unsigned int auth_status);
	void ProcessOptionsRequest(xGateSofiaobj *sobj, sip_t const *sip, tagi_t tags[], std::string callidstring);
	bool VerifyForFraudCall(std::string host);
	void RedirectOnReceiving302(xGateSofiaobj *sobj);
	bool isMSTeamsCall(std::string host);
	bool validateDomainWithWildcardWhitelist(std::string recvd_addr);
	bool validateDomainWithWildcardRouteAddr(std::string recvd_addr, std::string conf_roomId_Route, std::string *bridge_ip);
	bool validateDomainWithWildcardRouteAddrSerialRouting(std::string recvd_addr, std::string *bridge_ip,
														  bool isReRoute = false, std::string sRoutedIP = "");
	std::string getRouteIPAddrForDomain(std::string domain_name, std::string conf_room_id);
	bool WildcardPatternMatching(std::string recv_addr, std::string pattern, int recv_length, int pattern_length);
	void ReRouteCallToOtherNode(xGateSofiaobj *sobj);
	bool FetchNextRoute(xGateSofiaobj *sobj, std::string &sRouteIP);
	xGateReturn ForwardReferRequest(xGateSofiaobj *sofiaobj, int status, char const *phrase, nua_t *nua, nua_magic_t *magic,
									nua_handle_t *nh, nua_hmagic_t *hmagic, sip_t const *sip, tagi_t tags[]);
	void OnReferRequestAttendent(xGateSofiaobj *sofiatempobj, int status, char const *phrase, nua_t *nua, nua_magic_t *magic,
								 nua_handle_t *nh, nua_hmagic_t *hmagic, sip_t const *sip, tagi_t tags[]);

	/*LWSS Ends */
	// Valaditing Function
	bool ValidateSipMessage(sip_t const *sip, SIPVALIDATE header); // For Inbound
	std::string EncodeSipHeader(url_t *url, callInfoParameter cp, SIPVALIDATE header,std::string sIPAddress,std::string sTransport,bool isServerCall = false, sip_p_asserted_identity_t *paidurl = nullptr);

	bool VerifyAuthorisationResponce(sip_t const *sip, std::string password, std::string ReqMethod); // Function creates MD5 Hash and test for responce tag
	bool AuthenticateSipSession(int status, char const *phrase, nua_t *nua,
								nua_magic_t *magic, nua_handle_t *nh, nua_hmagic_t *hmagic, sip_t const *sip,
								tagi_t tags[]);

	void SofiaSetProxyRoute(nua_handle_t *nh, sip_t const *sip, string callid);

	bool DeleteSofiaObject(xGateSofiaobj *sobj);
	xGateReturn PrepareHandleAndSdpMakeCall(xGateSofiaobj *sobj, callInfoParameter cp, xGateSofiaobj *dialsobj);
	xGateReturn ForwardSipInfo(xGateSofiaobj *sobj, sip_payload_t *payload);
	void ForwardReInvite(xGateSofiaobj *sofiaobj);
	char *deblank(char *input);
	// below function will create url string
	void prepare_url_string(std::string client_ip, std::string client_port, std::string client_transport, std::string &url);
	// Validate IP Address IPV4/IPV6
	int ip_validator(string ipAddress);
	bool is_valid_ip_address(string ipAddress);
	std::string GetValidIP(std::string ipAddress);
	string convert_uppercase_to_lowercase(const char *transport);
	void set_invite_timer(xGateSofiaobj *sobj);
	void cancel_invite_timer(xGateSofiaobj *sobj);
	void timeout_invite_timer(xGateSofiaobj *sobj);
	std::string generate_reg_contact(xGateSofiaSipRegobj *regobj );
	void set_resp_contact(xGateSofiaobj &sobj);
	bool is_public_ip(std::string ip);
	std::tuple<bool ,std::string ,std::string> extractCliandExt(std::string url);
	inline bool isPBXCall(sip_t const *sip);
};
#endif // SOFIA_SIP_HANDLER
