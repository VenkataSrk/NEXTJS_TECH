/* $Header: /xGate/registrar.cpp 3     7/26/04 12:11p Bennylp $
   
   Registrar implementation

   $Log: /xGate/registrar.cpp $
 * 
 * 3     7/26/04 12:11p Bennylp
 * Added API in registration to get source IP address of message
 * 
 * 2     3/16/04 10:46a Bennylp
 * Added user agent information
 * 
 * 1     3/08/04 7:04p Bennylp
 * Initial version of:
 * - Registrar API (support spawning task for incoming registration)
 * - Authentication API
 */
#include "stdafx.h"
#include "registrar.h"
#include "api.h"
#include "log.h"
#include "kernel.h"
#include "task.h"
#include "sipauth.h"

#define THISMODULE  "Registrar"

struct Reg_TLS
{
    bool   answered_;
    struct Reg_Info info;
};

Registrar_Manager *Registrar_Manager::registrarInstance;

bool trace_string_op;

///////////////////////////////////////////////////////////////////////////////

Reg_Entry::Reg_Entry()
: priority(0), expiration(0)
{
}

Reg_Entry::Reg_Entry(const Reg_Entry & rhs)
: user(rhs.user), addr(rhs.addr), priority(rhs.priority), expiration(rhs.expiration)
{
}

Reg_Entry & Reg_Entry::operator=(const Reg_Entry & rhs)
{
    user = rhs.user;
    addr = rhs.addr;
    priority = rhs.priority;
    expiration = rhs.expiration;
    return *this;
}


////////////////////////////////////////////////////////////////////////////////

XgSipAuthEntry::XgSipAuthEntry()
: signature1_(SIGNATURE), signature2_(~SIGNATURE)
{
}

XgSipAuthEntry::~XgSipAuthEntry()
{
    signature1_ = 0;
    signature2_ = 0;
}

bool XgSipAuthEntry::IsValid() const
{
    try {
	return (signature1_ == SIGNATURE) && (signature2_ == ~SIGNATURE);
    } catch (...) {
	return false;
    }
}

XgSipAuthEntry::XgSipAuthEntry(const XgSipAuthEntry &rhs)
: SipAuthEntry(rhs), signature1_(rhs.signature1_), signature2_(rhs.signature2_)
{
}

XgSipAuthEntry & XgSipAuthEntry::operator=(const XgSipAuthEntry &rhs)
{
    SipAuthEntry::operator =(rhs);
    // No need to copy signature. We're still valid
    return *this;
}

XgSipAuthEntry & XgSipAuthEntry::operator=(const SipAuthEntry &rhs)
{
    SipAuthEntry::operator =(rhs);
    // No need to copy signature. We're still valid
    return *this;
}


///////////////////////////////////////////////////////////////////////////////

Reg_Info::Reg_Info()
: agent(NULL), token(0), sequence(0), has_auth(false)
{
}

Reg_Info::Reg_Info(const Reg_Info & rhs)
: agent(rhs.agent), token(rhs.token), domain(rhs.domain), src_addr(rhs.src_addr),
  src_id(rhs.src_id), ses_id(rhs.ses_id), sequence(rhs.sequence), 
  reg_rec_id(rhs.reg_rec_id), reg_rec_addr(rhs.reg_rec_addr), regs(rhs.regs),
  has_auth(rhs.has_auth), auth(rhs.auth), user_agent(rhs.user_agent),
  extra_info(rhs.extra_info)
{
}

Reg_Info & Reg_Info::operator=(const Reg_Info & rhs)
{
    agent = rhs.agent;
    token = rhs.token;
    domain = rhs.domain;
    src_addr = rhs.src_addr;
    src_id = rhs.src_id;
    ses_id = rhs.ses_id;
    sequence = rhs.sequence;
    reg_rec_id = rhs.reg_rec_id;
    reg_rec_addr = rhs.reg_rec_addr;
    regs = rhs.regs;
    has_auth = rhs.has_auth;
    auth = rhs.auth;
    user_agent = rhs.user_agent;
    extra_info = rhs.extra_info;
    return *this;
}

///////////////////////////////////////////////////////////////////////////////

Sofia_Reg_Info::Sofia_Reg_Info()
: sequence(0), has_auth(false)
{
}

Sofia_Reg_Info::Sofia_Reg_Info(const Sofia_Reg_Info & rhs)
: domain(rhs.domain), src_addr(rhs.src_addr),
  src_id(rhs.src_id), ses_id(rhs.ses_id), sequence(rhs.sequence),
  reg_rec_id(rhs.reg_rec_id), reg_rec_addr(rhs.reg_rec_addr), 
has_auth(rhs.has_auth), user_agent(rhs.user_agent), extra_info(rhs.extra_info),authScheme(rhs.authScheme),authRealm(rhs.authRealm),authUser(rhs.authUser),authDomain(rhs.authDomain),authUri(rhs.authUri),authAlgorithm(rhs.authAlgorithm),authNonce(rhs.authNonce),authCNonce(rhs.authCNonce),authNonceCount(rhs.authNonceCount),authOpaque(rhs.authOpaque),authStale(rhs.authStale),authQop(rhs.authQop),authVersion(rhs.authVersion),authResponse(rhs.authResponse),authBasicString(rhs.authBasicString)
{
}

Sofia_Reg_Info & Sofia_Reg_Info::operator=(const Sofia_Reg_Info & rhs)
{
    domain = rhs.domain;
    src_addr = rhs.src_addr;
    src_id = rhs.src_id;
    ses_id = rhs.ses_id;
    sequence = rhs.sequence;
    reg_rec_id = rhs.reg_rec_id;
    reg_rec_addr = rhs.reg_rec_addr;
    has_auth = rhs.has_auth;
    user_agent = rhs.user_agent;
    extra_info = rhs.extra_info;
    authScheme = rhs.authScheme;
    authRealm = rhs.authRealm;
    authUser = rhs.authUser;
    authDomain = rhs.authDomain;
    authUri = rhs.authUri;
    authAlgorithm = rhs.authAlgorithm;
    authNonce = rhs.authNonce;
    authCNonce = rhs.authCNonce;
    authNonceCount = rhs.authNonceCount;
    authOpaque = rhs.authOpaque;
    authStale = rhs.authStale;
    authQop = rhs.authQop;
    authVersion = rhs.authVersion;
    authResponse = rhs.authResponse;
    authBasicString = rhs.authBasicString;
    return *this;
}

///////////////////////////////////////////////////////////////////////////////

class Registrar_API : public CAPIProvider
{
public:
    Registrar_API();
    ~Registrar_API();

    //bool Init();
    STATUS Init();
    STATUS Shutdown();

private:
    // Registration API.
    STATUS API_CALL API_RegGetSessionInfo(CTask *task, char *reg_method,
					  char *callid, int *sequence);
    STATUS API_CALL API_RegGetAuthorization(CTask *task, int *auth);
    STATUS API_CALL API_RegGetSubject( CTask *task, 
				      char *src_addr, char *src_userid);
    STATUS API_CALL API_RegGetDomain( CTask *task, char *domain);
    STATUS API_CALL API_RegGetAccount( CTask *task, 
				       char *addr, char *userid);
    STATUS API_CALL API_RegGetSourceAddress( CTask *task, char *addr);
    STATUS API_CALL API_RegGetContactCount(CTask *task, int *count);
    STATUS API_CALL API_RegGetContact( CTask *task, int index, 
					    char *addr, char *user, float *priority,
					    int *expiration);
    STATUS API_CALL API_RegClearContact(CTask *task);
    STATUS API_CALL API_RegAddContact( CTask *task, 
					    const char *addr, const char *user,
					    float priority, int expiration);
    STATUS API_CALL API_RegSetContact( CTask *task, int index,
					    const char *addr, const char *user,
					    float priority, int expiration);
    STATUS API_CALL API_RegGetExtraInfo(CTask *task, char *extra_info);
    STATUS API_CALL API_RegSetExtraInfo(CTask *task, const char *extra_info);
    STATUS API_CALL API_RegAnswer(CTask *task, int status);
    STATUS API_CALL API_RegGetUserAgent(CTask *task, char *user_agent);

    // Authentication API
    STATUS API_CALL API_AuthGetScheme(CTask *task, char *scheme, int auth);
    STATUS API_CALL API_AuthGetURL(CTask *task, char *url, int auth);
    STATUS API_CALL API_AuthGetUser(CTask *task, char *user, int auth);
    STATUS API_CALL API_AuthGetRealm(CTask *task, char *realm, int auth);
    STATUS API_CALL API_AuthGetNonce(CTask *task, char *nonce, int auth);
    STATUS API_CALL API_AuthGetDigest(CTask *task, char *digest, int auth);
    STATUS API_CALL API_AuthCreateDigest(CTask *task, char *digest, const char *scheme,
					    const char *url, const char *method,
					    const char *user_name, const char *passwd,
					    const char *realm, const char *nonce);
	STATUS API_CALL API_AuthCreateDigestEx(CTask *task, char *digest, const char *scheme,
					    const char *url, const char *method,
					    const char *user_name, const char *passwd,
					    const char *realm, const char *nonce,
						const char *qop, const char *cnonce,
						const char *noncecount, const char *algorithm);
    STATUS API_CALL API_AuthAuthenticate(CTask *task, int auth, 
					 const char *method, const char *password, 
					 int *result);
};

///////////////////////////////////////////////////////////////////////////////
Registrar_Manager::Registrar_Manager()
: ServiceProvider("Registrar", true, false, Normal_Priority, 
		  "Registrar agents manager"), m_API(NULL)
{
    m_TLSId = Kernel::Instance()->AllocTLS();
    registrarInstance = this;
}

Registrar_Manager::~Registrar_Manager()
{
    Kernel::Instance()->FreeTLS(m_TLSId);
    registrarInstance = NULL;
}

Registrar_Manager *Registrar_Manager::Create()
{
    if (!registrarInstance) {
	new Registrar_Manager;
    }
    return registrarInstance;
}

Registrar_Manager *Registrar_Manager::Instance()
{
    return registrarInstance;
}

bool Registrar_Manager::AttachTask ( CTask *task )
{
    task->SetTLSValue(m_TLSId, 0);
    return true;
}

void Registrar_Manager::DetachTask ( CTask *task )
{
    Reg_TLS *reg_tls = (Reg_TLS*) task->GetTLSValue(m_TLSId);
    if (reg_tls) {
	if (reg_tls->answered_ == false) {
	    CLog::Info(THISMODULE, task, "Task exited with pending registration unanswered");
	    reg_tls->info.agent->CancelRegistration(&reg_tls->info);
	}
    }

    delete reg_tls;
    task->SetTLSValue(m_TLSId, 0);
}

bool Registrar_Manager::OnRegistration( const Reg_Info *reg_info)
{
    // Spawn new task.
    CTask *task = Kernel::Instance()->RouteIncomingRegistration(reg_info);
    if (!task) {
        printf("Registrar_Manager::OnRegistration : no task attached, return false\n");
	return false;
    }

    
	CLog::Warning(THISMODULE, task, "Registrar_Manager::OnRegistration reg_info address: %d", (int) reg_info);
	CLog::Warning(THISMODULE, task, "Registrar_Manager::OnRegistration User : %s", reg_info->auth.authUser.c_str());
	CLog::Warning(THISMODULE, task, "Registrar_Manager::OnRegistration URI  : %s", reg_info->auth.authUri.c_str());
	CLog::Warning(THISMODULE, task, "Registrar_Manager::OnRegistration Realm: %s", reg_info->auth.authRealm.c_str());
	CLog::Warning(THISMODULE, task, "Registrar_Manager::OnRegistration Nonce: %s", reg_info->auth.authNonce.c_str());	

    // Attach Reg_TLS object to task's TLS
    Reg_TLS *reg_tls = new Reg_TLS;
    reg_tls->answered_ = false;
    reg_tls->info = *reg_info;
    task->SetTLSValue(m_TLSId, (int)reg_tls);

    return true;
}

bool Registrar_Manager::Load()
{
    m_API = new Registrar_API;
    if (m_API->Init() != OK) {
	delete m_API;
	m_API = NULL;
	return false;
    }

    return true;
}

bool Registrar_Manager::CommitConfiguration()
{
    return true;
}

bool Registrar_Manager::Unload()
{
    m_API->Shutdown();
    delete m_API;
    m_API = NULL;
    return true;
}

STATUS Registrar_Manager::API_RegGetSessionInfo(CTask *task, char *reg_method,
						char *sesid, int *sequence)
{
    *reg_method = '\0';
    *sesid = '\0';
    *sequence = 0;

    Reg_TLS *tls = (Reg_TLS*)task->GetTLSValue(m_TLSId);
    if (!tls) {
	CLog::Warning(THISMODULE, task, "RegGetSessionInfo: task doesn't have registration request");
	return OK;
    }

    safe_strcpy(reg_method, tls->info.agent->GetMethod().c_str(), MAXAPPSTRLEN);
    safe_strcpy(sesid, tls->info.ses_id.c_str(), MAXAPPSTRLEN);
    *sequence = tls->info.sequence;

    return OK;
}

STATUS Registrar_Manager::API_RegGetDomain(CTask *task, char *domain)
{
    *domain = 0;

    Reg_TLS *tls = (Reg_TLS*)task->GetTLSValue(m_TLSId);
    if (!tls) {
	CLog::Warning(THISMODULE, task, "RegGetDomain: task doesn't have registration request");
	return OK;
    }

    safe_strcpy(domain, tls->info.domain.c_str(), MAXAPPSTRLEN);
    return OK;
}

STATUS Registrar_Manager::API_RegGetAuthorization(CTask *task, int *auth)
{
    *auth = 0;

    Reg_TLS *tls = (Reg_TLS*)task->GetTLSValue(m_TLSId);
    if (!tls) {
	CLog::Warning(THISMODULE, task, "RegGetAuthorization: task doesn't have registration request");
	return OK;
    }

    if (tls->info.has_auth) {
	*auth = (int) &tls->info.auth;

    } else {
	*auth = 0;
    }
    return OK;
}

STATUS Registrar_Manager::API_RegGetSubject( CTask *task, 
					     char *src_addr, char *src_userid)
{
    *src_addr = '\0';
    *src_userid = '\0';

    Reg_TLS *tls = (Reg_TLS*)task->GetTLSValue(m_TLSId);
    if (!tls) {
	CLog::Warning(THISMODULE, task, "RegGetSubject: task doesn't have registration request");
	return OK;
    }

    safe_strcpy(src_addr, tls->info.src_addr.c_str(), MAXAPPSTRLEN);
    safe_strcpy(src_userid, tls->info.src_id.c_str(), MAXAPPSTRLEN);

    return OK;
}

STATUS Registrar_Manager::API_RegGetAccount( CTask *task, 
					     char *addr, char *userid)
{
    *addr = '\0';
    *userid = '\0';

    Reg_TLS *tls = (Reg_TLS*)task->GetTLSValue(m_TLSId);
    if (!tls) {
	CLog::Warning(THISMODULE, task, "RegGetAccount: task doesn't have registration request");
	return OK;
    }

    safe_strcpy(addr, tls->info.reg_rec_addr.c_str(), MAXAPPSTRLEN);
    safe_strcpy(userid, tls->info.reg_rec_id.c_str(), MAXAPPSTRLEN);

    return OK;
}

STATUS Registrar_Manager::API_RegGetSourceAddress( CTask *task, char *addr)
{
    *addr = '\0';

    Reg_TLS *tls = (Reg_TLS*)task->GetTLSValue(m_TLSId);
    if (!tls) {
	CLog::Warning(THISMODULE, task, "RegGetAccount: task doesn't have registration request");
	return OK;
    }

    safe_strcpy(addr, tls->info.src_addr.c_str(), MAXAPPSTRLEN);
    return OK;
}

STATUS Registrar_Manager::API_RegGetContactCount(CTask *task, int *count)
{
    *count = 0;

    Reg_TLS *tls = (Reg_TLS*)task->GetTLSValue(m_TLSId);
    if (!tls) {
	CLog::Warning(THISMODULE, task, "RegGetContactCount: task doesn't have registration request");
	return OK;
    }

    *count = tls->info.regs.size();

    return OK;
}

STATUS Registrar_Manager::API_RegGetContact( CTask *task, int index, 
					     char *addr, char *user, float *priority,
					     int *expiration)
{
    *addr = '\0';
    *user = '\0';
    *priority = 0;
    *expiration = 0;

    Reg_TLS *tls = (Reg_TLS*)task->GetTLSValue(m_TLSId);
    if (!tls) {
	CLog::Warning(THISMODULE, task, "RegGetContact: task doesn't have registration request");
	return OK;
    }

    if (index < 1 || index > tls->info.regs.size()) {
	CLog::Warning(THISMODULE, task, "RegGetContact: invalid contact index %d (count=%d)", 
		      index, tls->info.regs.size());
	return OK;
    }

    index = index-1;

    Reg_Entry & ent = tls->info.regs[index];
    safe_strcpy(addr, ent.addr.c_str(), MAXAPPSTRLEN);
    safe_strcpy(user, ent.user.c_str(), MAXAPPSTRLEN);
    *priority = ent.priority;
    *expiration = ent.expiration;

    return OK;
}

STATUS Registrar_Manager::API_RegClearContact(CTask *task)
{
    Reg_TLS *tls = (Reg_TLS*)task->GetTLSValue(m_TLSId);
    if (!tls) {
	CLog::Warning(THISMODULE, task, "RegClearContact: task doesn't have registration request");
	return OK;
    }

    tls->info.regs.clear();
    return OK;
}

STATUS Registrar_Manager::API_RegAddContact( CTask *task, 
					     const char *addr, const char *user,
					     float priority, int expiration)
{
    Reg_TLS *tls = (Reg_TLS*)task->GetTLSValue(m_TLSId);
    if (!tls) {
	CLog::Warning(THISMODULE, task, "RegAddContact: task doesn't have registration request");
	return OK;
    }

    Reg_Entry ent;
    ent.user = user;
    ent.addr = addr;
    ent.priority = priority;
    ent.expiration = expiration;

    int i=0;
    for (i=0; i<tls->info.regs.size(); ++i) {
	if (priority > tls->info.regs[i].priority)
	    break;
    }

    tls->info.regs.insert(tls->info.regs.begin()+i, ent);
    return OK;
}

STATUS Registrar_Manager::API_RegSetContact( CTask *task, int index,
					     const char *addr, const char *user,
					     float priority, int expiration)
{
    Reg_TLS *tls = (Reg_TLS*)task->GetTLSValue(m_TLSId);
    if (!tls) {
	CLog::Warning(THISMODULE, task, "RegSetContact: task doesn't have registration request");
	return OK;
    }

    if (index < 1 || index > tls->info.regs.size()) {
	CLog::Warning(THISMODULE, task, "RegSetContact: invalid contact index %d (count=%d)", 
		      index, tls->info.regs.size());
	return OK;
    }

    --index;

    Reg_Entry & ent = tls->info.regs[index];
    ent.addr = addr;
    ent.user = user;
    ent.priority = priority;
    ent.expiration = expiration;

    return OK;
}

STATUS Registrar_Manager::API_RegGetExtraInfo(CTask *task, char *extra_info)
{
    *extra_info = '\0';

    Reg_TLS *tls = (Reg_TLS*)task->GetTLSValue(m_TLSId);
    if (!tls) {
	CLog::Warning(THISMODULE, task, "RegGetExtraInfo: task doesn't have registration request");
	return OK;
    }

    safe_strcpy(extra_info, tls->info.extra_info.c_str(), MAXAPPSTRLEN);
    return OK;
}

STATUS Registrar_Manager::API_RegSetExtraInfo(CTask *task, const char *extra_info)
{
    Reg_TLS *tls = (Reg_TLS*)task->GetTLSValue(m_TLSId);
    if (!tls) {
	CLog::Warning(THISMODULE, task, "RegSetExtraInfo: task doesn't have registration request");
	return OK;
    }
    
    if (*extra_info=='1')
	trace_string_op = true;
    else
	trace_string_op = false;

    tls->info.extra_info = extra_info;
    return OK;
}


STATUS Registrar_Manager::API_RegAnswer(CTask *task, int status)
{
    Reg_TLS *tls = (Reg_TLS*)task->GetTLSValue(m_TLSId);
    if (!tls) {
	CLog::Warning(THISMODULE, task, "RegAnswer: task doesn't have (more) registration request");
	return OK;
    }
    if (tls->answered_) {
	CLog::Warning(THISMODULE, task, "RegAnswer: task already respond to registration");
    }

    tls->info.agent->AnswerRegistration(&tls->info, status);
    tls->answered_ = true;
    return OK;
}

STATUS Registrar_Manager::API_RegGetUserAgent(CTask *task, char *user_agent)
{
    Reg_TLS *tls = (Reg_TLS*)task->GetTLSValue(m_TLSId);
    if (!tls) {
	CLog::Warning(THISMODULE, task, "RegAnswer: task doesn't have (more) registration request");
	return OK;
    }

    safe_strcpy(user_agent, tls->info.user_agent.c_str(), MAXAPPSTRLEN);
    return OK;
}

///////////////////////////////////////////////////////////////////////////////
Registrar_Agent::Registrar_Agent(const char *method)
: method_(method)
{
}

const String & Registrar_Agent::GetMethod() const
{
    return method_;
}

///////////////////////////////////////////////////////////////////////////////

Registrar_API::Registrar_API()
: CAPIProvider(API_PROVIDER_ID_REGISTRAR, "Registrar")
{
}

Registrar_API::~Registrar_API()
{
}

//bool Registrar_API::Init()
STATUS Registrar_API::Init()
{
    CLog::Detail( THISMODULE, "Registering API");

    if (API_Provider_Manager::Instance()->RegisterProvider(this) == NOT_OK) {
	CLog::Error( THISMODULE, "Unable to register Registrar API provider");
	return NOT_OK;
    }

    enum {
	RegGetSessionInfo,
	RegGetAuthorization,
	RegGetDomain,
	RegGetSubject,
	RegGetAccount,
	RegGetContactCount,
	RegGetContact,
	RegSetContact,
	RegClearContact,
	RegAddContact,
	RegGetExtraInfo,
	RegSetExtraInfo,
	RegAnswer,
	RegGetUserAgent,
	RegGetSrcAddr,

	AuthGetScheme = 20,
	AuthGetURL,
	AuthGetUser,
	AuthGetRealm,
	AuthGetNonce,
	AuthGetDigest,
	AuthCreateDigest,
	AuthAuthenticate,
	AuthCreateDigestEx,
    };

    // STATUS API_CALL API_RegGetSessionInfo(CTask *task, char *reg_method, char *ses, int *sequence);
    if (RegisterAPI( "RegGetSessionInfo", RegGetSessionInfo, false,
		     CVariant::VT_VOID,
		     (CAPIProvider::APIPROC) &Registrar_API::API_RegGetSessionInfo,
		     CAPIDef::CDECL_TYPE, 4,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "regmethod", BYREF, CVariant::VT_STRING),
		     CArgType( "sesid", BYREF, CVariant::VT_STRING),
		     CArgType( "sequence", BYREF, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS API_CALL API_RegGetAuthorization(CTask *task, int *auth);
    if (RegisterAPI( "RegGetAuthorization", RegGetAuthorization, false,
		     CVariant::VT_INT,
		     (CAPIProvider::APIPROC) &Registrar_API::API_RegGetAuthorization,
		     CAPIDef::CDECL_TYPE, 2,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "ret_hnd", BYREF, CVariant::VT_RET)) == NOT_OK) {
	return NOT_OK;
    }


    // STATUS API_CALL API_RegGetDomain( CTask *task, char *domain);
    if (RegisterAPI( "RegGetDomain", RegGetDomain, false,
		     CVariant::VT_STRING,
		     (CAPIProvider::APIPROC) &Registrar_API::API_RegGetDomain,
		     CAPIDef::CDECL_TYPE, 2,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "ret_hnd", BYREF, CVariant::VT_RET)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS API_CALL API_RegGetSubject( CTask *task, char *src_addr, char *src_userid);
    if (RegisterAPI( "RegGetSubject", RegGetSubject, false,
		     CVariant::VT_VOID,
		     (CAPIProvider::APIPROC) &Registrar_API::API_RegGetSubject,
		     CAPIDef::CDECL_TYPE, 3,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "address", BYREF, CVariant::VT_STRING),
		     CArgType( "id", BYREF, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }


    // STATUS API_CALL API_RegGetAccount( CTask *task, char *addr, char *userid);
    if (RegisterAPI( "RegGetAccount", RegGetAccount, false,
		     CVariant::VT_VOID,
		     (CAPIProvider::APIPROC) &Registrar_API::API_RegGetAccount,
		     CAPIDef::CDECL_TYPE, 3,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "address", BYREF, CVariant::VT_STRING),
		     CArgType( "id", BYREF, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }


    // STATUS API_CALL API_RegGetSourceAddress( CTask *task, char *addr)
    if (RegisterAPI( "RegGetSrcAddr", RegGetSrcAddr, false,
		     CVariant::VT_STRING,
		     (CAPIProvider::APIPROC) &Registrar_API::API_RegGetSourceAddress,
		     CAPIDef::CDECL_TYPE, 2,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "address", BYREF, CVariant::VT_RET)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS API_CALL API_RegGetContactCount(CTask *task, int *count);
    if (RegisterAPI( "RegGetContactCount", RegGetContactCount, false,
		     CVariant::VT_INT,
		     (CAPIProvider::APIPROC) &Registrar_API::API_RegGetContactCount,
		     CAPIDef::CDECL_TYPE, 2,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "retval", BYREF, CVariant::VT_RET)) == NOT_OK) {
	return NOT_OK;
    }


    //STATUS API_CALL API_RegGetContact( CTask *task, int index, 
    //					    char *addr, char *user, float *priority,
    //					    int *expiration);
    if (RegisterAPI( "RegGetContact", RegGetContact, false,
		     CVariant::VT_VOID,
		     (CAPIProvider::APIPROC) &Registrar_API::API_RegGetContact,
		     CAPIDef::CDECL_TYPE, 6,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "index", BYVAL, CVariant::VT_INT),
		     CArgType( "address", BYREF, CVariant::VT_STRING),
		     CArgType( "id", BYREF, CVariant::VT_STRING),
		     CArgType( "priority", BYREF, CVariant::VT_FLOAT),
		     CArgType( "expiration", BYREF, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS API_CALL API_RegClearContact(CTask *task);
    if (RegisterAPI( "RegClearContact", RegClearContact, false,
		     CVariant::VT_VOID,
		     (CAPIProvider::APIPROC) &Registrar_API::API_RegClearContact,
		     CAPIDef::CDECL_TYPE, 1,
		     CArgType( "task", BYVAL, CVariant::VT_TASK)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS API_CALL API_RegAddContact( CTask *task, 
    // 				       const char *addr, const char *user,
    // 				       float priority, int expiration);
    if (RegisterAPI( "RegAddContact", RegAddContact, false,
		     CVariant::VT_VOID,
		     (CAPIProvider::APIPROC) &Registrar_API::API_RegAddContact,
		     CAPIDef::CDECL_TYPE, 5,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "address", BYVAL, CVariant::VT_STRING),
		     CArgType( "id", BYVAL, CVariant::VT_STRING),
		     CArgType( "priority", BYVAL, CVariant::VT_FLOAT),
		     CArgType( "expiration", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }


    // STATUS API_CALL API_RegSetContact( CTask *task, int index,
    // 					    const char *addr, const char *user,
    // 					    float priority, int expiration);
    if (RegisterAPI( "RegSetContact", RegSetContact, false,
		     CVariant::VT_VOID,
		     (CAPIProvider::APIPROC) &Registrar_API::API_RegSetContact,
		     CAPIDef::CDECL_TYPE, 6,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "index", BYVAL, CVariant::VT_INT),
		     CArgType( "address", BYVAL, CVariant::VT_STRING),
		     CArgType( "id", BYVAL, CVariant::VT_STRING),
		     CArgType( "priority", BYVAL, CVariant::VT_FLOAT),
		     CArgType( "expiration", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS API_CALL API_RegGetExtraInfo(CTask *task, char *extra_info);
    if (RegisterAPI( "RegGetExtraInfo", RegGetExtraInfo, false,
		     CVariant::VT_STRING,
		     (CAPIProvider::APIPROC) &Registrar_API::API_RegGetExtraInfo,
		     CAPIDef::CDECL_TYPE, 2,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "retval", BYREF, CVariant::VT_RET)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS API_CALL API_RegSetExtraInfo(CTask *task, const char *extra_info);
    if (RegisterAPI( "RegSetExtraInfo", RegSetExtraInfo, false,
		     CVariant::VT_VOID,
		     (CAPIProvider::APIPROC) &Registrar_API::API_RegSetExtraInfo,
		     CAPIDef::CDECL_TYPE, 2,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "extra_info", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS API_CALL API_RegAnswer(CTask *task, int status);
    if (RegisterAPI( "RegAnswer", RegAnswer, false,
		     CVariant::VT_VOID,
		     (CAPIProvider::APIPROC) &Registrar_API::API_RegAnswer,
		     CAPIDef::CDECL_TYPE, 2,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "status", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS Registrar_Manager::API_RegGetUserAgent(CTask *task, char *user_agent)
    if (RegisterAPI( "RegGetUserAgent", RegGetUserAgent, false,
		     CVariant::VT_STRING,
		     (CAPIProvider::APIPROC) &Registrar_API::API_RegGetUserAgent,
		     CAPIDef::CDECL_TYPE, 2,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "retval", BYREF, CVariant::VT_RET)) == NOT_OK) {
	return NOT_OK;
    }

    // Authentication API

    // STATUS API_CALL API_AuthGetScheme(CTask *task, char *scheme, int auth);
    if (RegisterAPI( "AuthGetScheme", AuthGetScheme, false,
		     CVariant::VT_STRING,
		     (CAPIProvider::APIPROC) &Registrar_API::API_AuthGetScheme,
		     CAPIDef::CDECL_TYPE, 3,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "ret", BYREF, CVariant::VT_RET),
		     CArgType( "auth_hnd", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS API_CALL API_AuthGetURL(CTask *task, char *url, int auth);
    if (RegisterAPI( "AuthGetURL", AuthGetURL, false,
		     CVariant::VT_STRING,
		     (CAPIProvider::APIPROC) &Registrar_API::API_AuthGetURL,
		     CAPIDef::CDECL_TYPE, 3,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "ret", BYREF, CVariant::VT_RET),
		     CArgType( "auth_hnd", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS API_CALL API_AuthGetUser(CTask *task, char *user, int auth);
    if (RegisterAPI( "AuthGetUser", AuthGetUser, false,
		     CVariant::VT_STRING,
		     (CAPIProvider::APIPROC) &Registrar_API::API_AuthGetUser,
		     CAPIDef::CDECL_TYPE, 3,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "ret", BYREF, CVariant::VT_RET),
		     CArgType( "auth_hnd", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS API_CALL API_AuthGetRealm(CTask *task, char *realm, int auth);
    if (RegisterAPI( "AuthGetRealm", AuthGetRealm, false,
		     CVariant::VT_STRING,
		     (CAPIProvider::APIPROC) &Registrar_API::API_AuthGetRealm,
		     CAPIDef::CDECL_TYPE, 3,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "ret", BYREF, CVariant::VT_RET),
		     CArgType( "auth_hnd", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }


    // STATUS API_CALL API_AuthGetNonce(CTask *task, char *nonce, int auth);
    if (RegisterAPI( "AuthGetNonce", AuthGetNonce, false,
		     CVariant::VT_STRING,
		     (CAPIProvider::APIPROC) &Registrar_API::API_AuthGetNonce,
		     CAPIDef::CDECL_TYPE, 3,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "ret", BYREF, CVariant::VT_RET),
		     CArgType( "auth_hnd", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS API_CALL API_AuthGetDigest(CTask *task, char *digest, int auth);
    if (RegisterAPI( "AuthGetDigest", AuthGetDigest, false,
		     CVariant::VT_STRING,
		     (CAPIProvider::APIPROC) &Registrar_API::API_AuthGetDigest,
		     CAPIDef::CDECL_TYPE, 3,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "ret", BYREF, CVariant::VT_RET),
		     CArgType( "auth_hnd", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS API_CALL API_AuthCreateDigest(CTask *task, char *digest, const char *scheme,
    //					    const char *url, const char *method,
    //					    const char *user_name, const char *passwd,
    //					    const char *realm, const char *nonce);
    if (RegisterAPI( "AuthCreateDigest", AuthCreateDigest, false,
		     CVariant::VT_STRING,
		     (CAPIProvider::APIPROC) &Registrar_API::API_AuthCreateDigest,
		     CAPIDef::CDECL_TYPE, 9,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "ret", BYREF, CVariant::VT_RET),
		     CArgType( "scheme", BYVAL, CVariant::VT_STRING),
		     CArgType( "url", BYVAL, CVariant::VT_STRING),
		     CArgType( "method", BYVAL, CVariant::VT_STRING),
		     CArgType( "user_name", BYVAL, CVariant::VT_STRING),
		     CArgType( "passwd", BYVAL, CVariant::VT_STRING),
		     CArgType( "realm", BYVAL, CVariant::VT_STRING),
		     CArgType( "nonce", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }

	// STATUS API_CALL API_AuthCreateDigestEX(CTask *task, char *digest, const char *scheme,
	//				    const char *url, const char *method,
	//				    const char *user_name, const char *passwd,
	//				    const char *realm, const char *nonce,
	//					const char *qop, const char *cnonce,
	//					const char *noncecount, const char *algorithm)
    if (RegisterAPI( "AuthCreateDigestEx", AuthCreateDigestEx, false,
		     CVariant::VT_STRING,
		     (CAPIProvider::APIPROC) &Registrar_API::API_AuthCreateDigestEx,
		     CAPIDef::CDECL_TYPE, 13,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "ret", BYREF, CVariant::VT_RET),
		     CArgType( "scheme", BYVAL, CVariant::VT_STRING),
		     CArgType( "url", BYVAL, CVariant::VT_STRING),
		     CArgType( "method", BYVAL, CVariant::VT_STRING),
		     CArgType( "user_name", BYVAL, CVariant::VT_STRING),
		     CArgType( "passwd", BYVAL, CVariant::VT_STRING),
		     CArgType( "realm", BYVAL, CVariant::VT_STRING),
			 CArgType( "nonce", BYVAL, CVariant::VT_STRING),
			 CArgType( "qop", BYVAL, CVariant::VT_STRING),
			 CArgType( "cnonce", BYVAL, CVariant::VT_STRING),
			 CArgType( "algorithm", BYVAL, CVariant::VT_STRING),
		     CArgType( "noncecount", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS API_CALL API_AuthAuthenticate(CTask *task, int auth, 
    //					    const char *method, const char *password, int *result);
    if (RegisterAPI( "AuthAuthenticate", AuthAuthenticate, false,
		     CVariant::VT_INT,
		     (CAPIProvider::APIPROC) &Registrar_API::API_AuthAuthenticate,
		     CAPIDef::CDECL_TYPE, 5,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "auth_hnd", BYVAL, CVariant::VT_INT),
		     CArgType( "method", BYVAL, CVariant::VT_STRING),
		     CArgType( "password", BYVAL, CVariant::VT_STRING),
		     CArgType( "ret", BYREF, CVariant::VT_RET)) == NOT_OK) {
	return NOT_OK;
    }

    return OK;
}

STATUS Registrar_API::Shutdown()
{
    API_Provider_Manager::Instance()->UnregisterProvider(this);
    return OK;
}

STATUS API_CALL Registrar_API::API_RegGetSessionInfo(CTask *task, char *reg_method,
						     char *callid, int *sequence)
{
    return Registrar_Manager::Instance()->API_RegGetSessionInfo(task, reg_method, callid, sequence);
}

STATUS API_CALL Registrar_API::API_RegGetAuthorization(CTask *task, int *auth)
{
    return Registrar_Manager::Instance()->API_RegGetAuthorization(task, auth);
}

STATUS API_CALL Registrar_API::API_RegGetSubject( CTask *task, 
						  char *src_addr, char *src_userid)
{
    return Registrar_Manager::Instance()->API_RegGetSubject(task, src_addr, src_userid);
}

STATUS API_CALL Registrar_API::API_RegGetDomain( CTask *task, char *domain)
{
    return Registrar_Manager::Instance()->API_RegGetDomain(task, domain);
}

STATUS API_CALL Registrar_API::API_RegGetAccount( CTask *task, 
						  char *addr, char *userid)
{
    return Registrar_Manager::Instance()->API_RegGetAccount(task, addr, userid);
}

STATUS API_CALL Registrar_API::API_RegGetSourceAddress( CTask *task, char *addr)
{
    return Registrar_Manager::Instance()->API_RegGetSourceAddress(task, addr);
}

STATUS API_CALL Registrar_API::API_RegGetContactCount(CTask *task, int *count)
{
    return Registrar_Manager::Instance()->API_RegGetContactCount(task, count);
}

STATUS API_CALL Registrar_API::API_RegGetContact( CTask *task, int index, 
						  char *addr, char *user, 
						  float *priority,
						  int *expiration)
{
    return Registrar_Manager::Instance()->API_RegGetContact(task, index, addr, user, priority, expiration);
}

STATUS API_CALL Registrar_API::API_RegClearContact(CTask *task)
{
    return Registrar_Manager::Instance()->API_RegClearContact(task);
}

STATUS API_CALL Registrar_API::API_RegAddContact( CTask *task, 
						  const char *addr, const char *user,
						  float priority, int expiration)
{
    return Registrar_Manager::Instance()->API_RegAddContact(task, addr, user, priority, expiration);
}

STATUS API_CALL Registrar_API::API_RegSetContact( CTask *task, int index,
						  const char *addr, const char *user,
						  float priority, int expiration)
{
    return Registrar_Manager::Instance()->API_RegSetContact(task, index, addr, user, priority, expiration);
}

STATUS API_CALL Registrar_API::API_RegAnswer(CTask *task, int status)
{
    return Registrar_Manager::Instance()->API_RegAnswer(task, status);
}

STATUS API_CALL Registrar_API::API_RegGetExtraInfo(CTask *task, char *extra_info)
{
    return Registrar_Manager::Instance()->API_RegGetExtraInfo(task, extra_info);
}

STATUS API_CALL Registrar_API::API_RegSetExtraInfo(CTask *task, const char *extra_info)
{
    return Registrar_Manager::Instance()->API_RegSetExtraInfo(task, extra_info);
}

STATUS API_CALL Registrar_API::API_RegGetUserAgent(CTask *task, char *user_agent)
{
    return Registrar_Manager::Instance()->API_RegGetUserAgent(task, user_agent);
}
//STATUS API_CALL Registrar_API::API_AuthGetScheme(CTask *task, char *scheme,long auth)//changes made for migraation from 32 bit to 64 bit
STATUS API_CALL Registrar_API::API_AuthGetScheme(CTask *task, char *scheme, int auth)
{
    XgSipAuthEntry *e = (XgSipAuthEntry *)auth;

    *scheme = '\0';

    if (!e->IsValid()) {
	CLog::Error(THISMODULE, task, "Invalid handle %d in AuthGetScheme", auth);
	return OK;
    }

    safe_strcpy(scheme, e->authScheme.c_str(), MAXAPPSTRLEN);
    return OK;
}
//STATUS API_CALL Registrar_API::API_AuthGetURL(CTask *task, char *url, long auth)//changes made for migration from 32 bit to 64 bit
STATUS API_CALL Registrar_API::API_AuthGetURL(CTask *task, char *url, int auth)
{
    XgSipAuthEntry *e = (XgSipAuthEntry *)auth;

    *url = '\0';

    if (!e->IsValid()) {
	CLog::Error(THISMODULE, task, "Invalid handle %d in AuthGetURL", auth);
	return OK;
    }

    safe_strcpy(url, e->authUri.c_str(), MAXAPPSTRLEN);
    return OK;
}
//STATUS API_CALL Registrar_API::API_AuthGetUser(CTask *task, char *user, long auth)//changes made for migration from 32 bit to 64 bit 
STATUS API_CALL Registrar_API::API_AuthGetUser(CTask *task, char *user, int auth)
{
    XgSipAuthEntry *e = (XgSipAuthEntry *)auth;

    *user = '\0';

    if (!e->IsValid()) {
	CLog::Error(THISMODULE, task, "Invalid handle %d in AuthGetUser", auth);
	return OK;
    }

    safe_strcpy(user, e->authUser.c_str(), MAXAPPSTRLEN);
    return OK;

}
//STATUS API_CALL Registrar_API::API_AuthGetRealm(CTask *task, char *realm, long auth)//changes made for migration from 32 bit to 64 bit
STATUS API_CALL Registrar_API::API_AuthGetRealm(CTask *task, char *realm, int auth)
{
    XgSipAuthEntry *e = (XgSipAuthEntry *)auth;

    *realm = '\0';

    if (!e->IsValid()) {
	CLog::Error(THISMODULE, task, "Invalid handle %d in AuthGetRealm", auth);
	return OK;
    }

    safe_strcpy(realm, e->authRealm.c_str(), MAXAPPSTRLEN);
    return OK;

}
//STATUS API_CALL Registrar_API::API_AuthGetNonce(CTask *task, char *nonce, long auth)//changes made for migration from 32 bit to 64 bit
STATUS API_CALL Registrar_API::API_AuthGetNonce(CTask *task, char *nonce, int auth)
{
    XgSipAuthEntry *e = (XgSipAuthEntry *)auth;

    *nonce = '\0';

    if (!e->IsValid()) {
	CLog::Error(THISMODULE, task, "Invalid handle %d in AuthGetNonce", auth);
	return OK;
    }

    safe_strcpy(nonce, e->authNonce.c_str(), MAXAPPSTRLEN);
    return OK;

}
//STATUS API_CALL Registrar_API::API_AuthGetDigest(CTask *task, char *digest, long auth)//chnages made for migration from 32 bit to 64 bit
STATUS API_CALL Registrar_API::API_AuthGetDigest(CTask *task, char *digest, int auth)
{
    XgSipAuthEntry *e = (XgSipAuthEntry *)auth;

    *digest = '\0';

    if (!e->IsValid()) {
	CLog::Error(THISMODULE, task, "Invalid handle %d in AuthGetDigest", auth);
	return OK;
    }

    if (strcasecmp(e->authScheme.c_str(), "basic") == 0) {
	safe_strcpy(digest, e->authBasicString.c_str(), MAXAPPSTRLEN);
    } else {
	safe_strcpy(digest, e->authResponse.c_str(), MAXAPPSTRLEN);
    }
    return OK;

}

STATUS API_CALL Registrar_API::API_AuthCreateDigest( CTask *task, char *digest, const char *scheme,
						   const char *url, const char *method,
						   const char *user_name, const char *passwd,
						   const char *realm, const char *nonce)
{
    String str_digest;

    if (strcasecmp(scheme, "basic") == 0) {
#ifndef __linux__

	str_digest = SipBasicAuthScheme::EncodeCredential(user_name, passwd);
#else
     /* write linux code here */
#endif

    } else if (strcasecmp(scheme, "digest") == 0) {

#ifndef __linux__
	str_digest = SipDigestAuthScheme::CreateRequestDigest(url, method, user_name, 
							      passwd, realm, nonce);
#else
       /* Write CreateRequestDigest  here to avoid dependencies */
#endif


    } else {
	CLog::Error(THISMODULE, task, 
		    "Invalid authentication scheme '%s' in AuthCreateDigest", 
		    scheme);
    }

    safe_strcpy(digest, str_digest.c_str(), MAXAPPSTRLEN);
    return OK;
}

STATUS API_CALL Registrar_API::API_AuthCreateDigestEx(CTask *task, char *digest, const char *scheme,
					    const char *url, const char *method,
					    const char *user_name, const char *passwd,
					    const char *realm, const char *nonce,
						const char *qop, const char *cnonce,
						const char *noncecount, const char *algorithm)
{
	String str_digest;

    if (strcasecmp(scheme, "basic") == 0) {
#ifndef __linux__
	str_digest = SipBasicAuthScheme::EncodeCredential(user_name, passwd);
#endif

    } else if (strcasecmp(scheme, "digest") == 0) {

	//TODO:Dandi - complete this function
#ifndef __linux__

	str_digest = SipDigestAuthScheme::CreateRequestDigest(
		url,
		method,
		algorithm,
		user_name,
		passwd,
		realm,
		nonce,
		cnonce,
		noncecount,
		qop);
#else
      /* Write CreateRequestDigest  here to avoid dependencies */
#endif

    } else {
	CLog::Error(THISMODULE, task, 
		    "Invalid authentication scheme '%s' in AuthCreateDigest", 
		    scheme);
    }

    safe_strcpy(digest, str_digest.c_str(), MAXAPPSTRLEN);
    return OK;
}

STATUS API_CALL Registrar_API::API_AuthAuthenticate(CTask *task, int auth, 
						    const char *method, const char *password, 
						    int *result)
{
    String str_digest;

    XgSipAuthEntry *e = (XgSipAuthEntry *)auth;

    *result = 0;

    if (!e->IsValid()) {
	CLog::Error(THISMODULE, task, "Invalid handle %d in AuthAuthenticate", auth);
	return OK;
    }


    if (strcasecmp(e->authScheme.c_str(), "basic") == 0) {
#ifndef __linux__

	str_digest = SipBasicAuthScheme::EncodeCredential(e->authUser, password);
#endif
	*result = (str_digest == e->authBasicString);

    } else if (strcasecmp(e->authScheme.c_str(), "digest") == 0) {

		//TODO:Dandi - complete this part of function
		if(strcasecmp(e->authQop.c_str(), "auth") || strcasecmp(e->authQop.c_str(), "auth-int"))
		{
			/*
			String SipDigestAuthScheme::CreateRequestDigest(const String & req_uri,
						const String & method,
						const String & algorithm,
						const String & username,
						const String & password,
						const String & realm,
						const String & nonce,
						const String & cnonce,
						const String & noncecount,
						const String & qop)
			*/
#ifndef __linux__
			str_digest = SipDigestAuthScheme::CreateRequestDigest(e->authUri,
				method,
				e->authAlgorithm,
				e->authUser,
				password,
				e->authRealm,
				e->authNonce,
				e->authCNonce,
				e->authNonceCount,
				e->authQop);
#else
        /* Write CreateRequestDigest  here to avoid dependencies */
#endif

		}
		else
		{
#ifndef __linux__
			str_digest = SipDigestAuthScheme::CreateRequestDigest(e->authUri, method, 
							      e->authUser,  password, 
							      e->authRealm, e->authNonce);
#else
        /* Write CreateRequestDigest  here to avoid dependencies */
#endif
		}
		*result = (str_digest == e->authResponse);


    } else {
	CLog::Error(THISMODULE, task, 
		    "Invalid authentication scheme '%s' in AuthAuthenticate", 
		    e->authScheme.c_str());
    }

    return OK;
}
