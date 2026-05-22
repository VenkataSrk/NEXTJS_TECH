// $Header: /xGate/RapConfigurator.h 3     7/14/01 5:08p Iwanj $

#ifndef INC_RAPCONFIGURATOR
#define INC_RAPCONFIGURATOR

//#include "term.h"
#include "serviceprovider.h"


// forward decl.
class Cfg_Tree_Group;
class RapConfig;
class Cfg_Tree_Entry;
class Term_Cmd;

//class RapConfigurator: public Term_Cmd_Target
class RapConfigurator: public ServiceProvider
{
public:
    virtual ~RapConfigurator();

    static RapConfigurator* Create();
    static RapConfigurator* Instance();

    virtual bool Load();
    virtual bool Unload();

    //virtual STATUS OnShowConfig(const Cfg_Tree_Entry*,String&,const String&,int) const;
    virtual bool OnShowConfig(const Cfg_Tree_Entry*,String&,const String&,int) const;
    void DoTermCmd(Term_Session*,bool,Term_Cmd*);
    inline Cfg_Tree_Group* GetConfigTree();

    RapConfig& rapConfig_;

private:
    RapConfigurator();
    RapConfigurator(const RapConfigurator&);
    RapConfigurator& operator=(const RapConfigurator&);

    enum
    {
	Term_Cmd_Rap_AutoDiscovery,
	Term_Cmd_Rap_Config,
	Term_Cmd_Rap_Enable,
	Term_Cmd_Rap_RlsAddress,
	Term_Cmd_Rap_RlsPort,
	Term_Cmd_Rap_MulticastAddress,
	Term_Cmd_Rap_MulticastPort,
	Term_Cmd_Rap_LocalAddress,
	Term_Cmd_Rap_LocalPort,
	Term_Cmd_Rap_MaxRequestAttempt,
	Term_Cmd_Rap_RequestTimeout,
	Term_Cmd_Rap_ResolverThread,
	Term_Cmd_Rap_MaxRequestPending,
	Term_Cmd_Rap_MaxRetryPending,
	Term_Cmd_Rap_MaxReplyPending
    };

    inline void SendErrorMessage(Term_Session*,const char*);
    inline bool SetAutoDiscovery(int);
    inline bool SetRlsAddress(const String&);
    inline bool SetRlsPort(int);
    inline bool SetMulticastAddress(const String&);
    inline bool SetMulticastPort(int);
    inline bool SetLocalAddress(const String&);
    inline bool SetLocalPort(int);
    inline bool SetMaxRequestAttempt(int);
    inline bool SetRequestTimeout(int);
    inline bool SetResolverThread(int);
    inline bool SetMaxRequestPending(int);
    inline bool SetMaxRetryPending(int);
    inline bool SetMaxReplyPending(int);
    inline bool EnableRap();
    inline bool DisableRap();

    Cfg_Tree_Group* cfgTree_;
    static RapConfigurator* instance_;
};

#endif

