/* $Header: /xGate/registrar.h 4     7/26/04 12:11p Bennylp $
  
   Registrar.

   $Log: /xGate/registrar.h $
 * 
 * 4     7/26/04 12:11p Bennylp
 * Added API in registration to get source IP address of message
 * 
 * 3     3/16/04 10:46a Bennylp
 * Added user agent information
 * 
 * 2     3/08/04 7:05p Bennylp
 * Initial version
 */

#ifndef __REGISTRAR_H__
#define __REGISTRAR_H__

//#include "../slsip/include/sipmsg.h"
#include "sipmsg.h"
#include "serviceprovider.h"

class Registrar_API;

///////////////////////////////////////////////////////////////////////////////
/*
 *
 *  Authentication handle.
 *
 */
class XgSipAuthEntry : public SipAuthEntry
{
public:
    XgSipAuthEntry();
    ~XgSipAuthEntry();

    bool IsValid() const;

    XgSipAuthEntry(const XgSipAuthEntry &);
    XgSipAuthEntry & operator=(const XgSipAuthEntry &);
    XgSipAuthEntry & operator=(const SipAuthEntry &);

private:
    enum { SIGNATURE = 0x758201B8 };
    unsigned signature1_, signature2_;
};


//////////////////////////////////////////////////////////////////////////////
/**
 * This is the general authentication information.
 */
typedef XgSipAuthEntry	Auth_Info;
typedef String		Reg_Addr;	// ip[:port]
class   Registrar_Agent;
typedef int Reg_Token;

struct Reg_Entry
{
    String	user;
    Reg_Addr	addr;
    int		priority;
    int		expiration;

    Reg_Entry();
    Reg_Entry(const Reg_Entry & rhs);
    Reg_Entry & operator=(const Reg_Entry & rhs);
};

//////////////////////////////////////////////////////////////////////////////
/**
 * This is the general registration information used by all
 * registration agent.
 */
struct Reg_Info
{
    enum Status
    {
	STATUS_OK	    = 200,
	STATUS_NOT_FOUND    = 404,
	STATUS_UNAUTHORIZED = 401,
	STATUS_FORBIDDEN    = 403,
	STATUS_REQUEST_ERROR= 400,
	STATUS_SERVER_ERROR = 500,
    };

    Registrar_Agent    *agent;
    Reg_Token		token;

    String		domain;		// The registration domain

    Reg_Addr		src_addr;	// Address where the registration is coming from
    String		src_id;		// Id (i.e. phone #) which performs registration

    String		ses_id;		// Session ID
    unsigned		sequence;	// Session sequence number

    String		reg_rec_id;	// Record ID being queried/modified.
    Reg_Addr		reg_rec_addr;	// Record IP address being queried/modified.

    Vector<Reg_Entry>	regs;		// Registration contacts.

    bool		has_auth;	// Has authorization info?
    Auth_Info		auth;		// Authorization info.
	int sip_handle;

    String		user_agent;	// User agent information.
    String		extra_info;	// Additional information

    Reg_Info();
    Reg_Info(const Reg_Info & rhs);
    Reg_Info & operator=(const Reg_Info & rhs);
};



/* Sofia SIP Registration */
struct Sofia_Reg_Info
{
    enum Status
    {
        STATUS_OK           = 200,
        STATUS_NOT_FOUND    = 404,
        STATUS_UNAUTHORIZED = 401,
        STATUS_FORBIDDEN    = 403,
        STATUS_REQUEST_ERROR= 400,
        STATUS_SERVER_ERROR = 500,
    };

    String              domain;         // The registration domain

    String            src_addr;       // Address where the registration is coming from
    String              src_id;         // Id (i.e. phone #) which performs registration

    String              ses_id;         // Session ID
    unsigned            sequence;       // Session sequence number

    String              reg_rec_id;     // Record ID being queried/modified.
    String            reg_rec_addr;   // Record IP address being queried/modified.

    bool                has_auth;       // Has authorization info?
    int sip_handle;

    String              user_agent;     // User agent information.
    String              extra_info;     // Additional information

    String  authScheme;
    String  authRealm;
    String  authUser;
    String  authDomain;
    String  authUri;
    String  authAlgorithm;
    String  authNonce;
    String  authCNonce;
    String  authNonceCount;
    String authOpaque;
    int    authStale;
    String  authQop;
    String  authVersion;
    String  authResponse;
    String  authBasicString;
    

    Sofia_Reg_Info();
    Sofia_Reg_Info(const Sofia_Reg_Info & rhs);
    Sofia_Reg_Info & operator=(const Sofia_Reg_Info & rhs);
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

//////////////////////////////////////////////////////////////////////////////
class Registrar_Manager : public ServiceProvider
{
public:
    Registrar_Manager();
    ~Registrar_Manager();

    static Registrar_Manager *Create();
    static Registrar_Manager *Instance();

    bool OnRegistration( const Reg_Info *reg_info);
    bool AnswerRegistration( const Reg_Info *reg_info,
			     int status);

    virtual bool Load();
    virtual bool CommitConfiguration();
    virtual bool Unload();
    bool AttachTask ( CTask * );
    void DetachTask ( CTask * );


    // APIs
    STATUS API_RegGetSessionInfo(CTask *task, char *reg_method,
				 char *callid, int *sequence);
    STATUS API_RegGetAuthorization(CTask *task, int *auth);
    STATUS API_RegGetDomain(CTask *task, char *domain);
    STATUS API_RegGetSubject( CTask *task, 
			      char *src_addr, char *src_userid);
    STATUS API_RegGetAccount( CTask *task, 
			      char *addr, char *userid);
    STATUS API_RegGetSourceAddress( CTask *task, char *addr);
    STATUS API_RegGetContactCount(CTask *task, int *count);
    STATUS API_RegGetContact( CTask *task, int index, 
			      char *addr, char *user, float *priority,
			      int *expiration);
    STATUS API_RegClearContact(CTask *task);
    STATUS API_RegAddContact( CTask *task, 
			      const char *addr, const char *user,
			      float priority, int expiration);
    STATUS API_RegSetContact( CTask *task, int index,
			      const char *addr, const char *user,
			      float priority, int expiration);
    STATUS API_RegGetExtraInfo(CTask *task, char *extra_info);
    STATUS API_RegSetExtraInfo(CTask *task, const char *extra_info);
    STATUS API_RegAnswer(CTask *task, int status);
    STATUS API_RegGetUserAgent(CTask *task, char *user_agent);

private:
    static Registrar_Manager *registrarInstance;

    int m_TLSId;
    Registrar_API *m_API;
};

//////////////////////////////////////////////////////////////////////////////
class Registrar_Agent
{
public:
    Registrar_Agent(const char *method);
    const String & GetMethod() const;
    virtual void CancelRegistration(const Reg_Info *reg_info) = 0;
    virtual bool AnswerRegistration(const Reg_Info *reg_info,
				    int status) = 0;

private:
    String method_;
};


#endif	/* __REGISTRAR_H__ */
