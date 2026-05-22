// $Header: /xGate/RapConfigurator.cpp 8     8/08/01 6:37p Bennylp $

/*
    $History: RapConfigurator.cpp $
 * 
 * *****************  Version 8  *****************
 * User: Bennylp      Date: 8/08/01    Time: 6:37p
 * Updated in $/xGate
 * Changed include files related to ostringstream.
 * 
 * *****************  Version 7  *****************
 * User: Iwanj        Date: 7/17/01    Time: 3:30p
 * Updated in $/xGate
 * add enable/no enable
 * 
 * *****************  Version 6  *****************
 * User: Iwanj        Date: 7/14/01    Time: 5:08p
 * Updated in $/xGate
 * add autodiscovery mode
 * 
 * *****************  Version 5  *****************
 * User: Iwanj        Date: 7/14/01    Time: 4:46p
 * Updated in $/xGate
 * 
 * *****************  Version 4  *****************
 * User: Iwanj        Date: 7/13/01    Time: 8:33p
 * Updated in $/xGate
 * 
 * *****************  Version 3  *****************
 * User: Iwanj        Date: 7/13/01    Time: 8:30p
 * Updated in $/xGate
 * 
 * *****************  Version 2  *****************
 * User: Iwanj        Date: 7/13/01    Time: 8:01p
 * Updated in $/xGate
*/

#include <iostream>//DG changed
//#include "stdafx.h"
#include "RapConfigurator.h"
#include "term.h"
#include "kernel.h"
#include "cfg.h"
#include "RapConfig.h"
#include "RapManager.h"
#include <sstream>

using std::ostringstream;

#define NEWLINE "\n"

static const char __modname__[] = "RAP";
#define THISMODULE		__modname__

RapConfigurator* RapConfigurator::instance_ = 0;

RapConfigurator::RapConfigurator():
    //Term_Cmd_Target(),
    ServiceProvider(THISMODULE,false,false), //attach_task=true,poll=true
    cfgTree_(0),
    rapConfig_(RapConfig::instance())
{

}

RapConfigurator::~RapConfigurator()
{
}

RapConfigurator* RapConfigurator::Create()
{
    if (instance_==0)
	instance_ = new RapConfigurator;

    return instance_;
}

RapConfigurator* RapConfigurator::Instance()
{
    return instance_;
}

bool RapConfigurator::Load()
{
    cfgTree_ = new Cfg_Tree_Group("resource-locator","RLS Configuration",Kernel::Instance()->RootConfig());
    // config command:
    //
    // enable(no enable)
    // auto-discovery <int-value>
    // server-address <string-value>
    // server-port <int-value>
    // multicast-address <string-value>
    // multicast-port <int-value>
    // local-address <string-value>
    // local-port <int-value>

    // request-attempt-max <int-value>
    // request-timeout <int-value>

    // request-pending-max <int-value>
    // retry-pending-max <int-value>
    // reply-pending-max <int-value>

    Term_Cmd* cmd = 0;

    cmd = new Term_Cmd (this, Term_Cmd_Rap_Config, "resource-locator", "Configure RLS", PrivilegeLevelAdmin, TermModeConfig, false);
    RegisterTermCmd(cmd, Kernel::Instance()->RootConfig());

    cmd = new Term_Cmd (this, Term_Cmd_Rap_Enable, "enable", "Enable RAP", PrivilegeLevelAdmin, TermModeConfig,true);
    RegisterTermCmd(cmd,cfgTree_);

    /* no multicast
    cmd = new Term_Cmd(this,Term_Cmd_Rap_AutoDiscovery,"auto-discovery","Set auto discovery mode",PrivilegeLevelAdmin,TermModeConfig,false);
    cmd->Add(new Term_Cmd_Arg("value",CVariant(0),CVariantValidator(0,"config value")));
    RegisterTermCmd(cmd,cfgTree_);
    */

    cmd = new Term_Cmd(this,Term_Cmd_Rap_RlsAddress,"server-address","Set server address",PrivilegeLevelAdmin,TermModeConfig,false);
    cmd->Add(new Term_Cmd_Arg("value",CVariant(""),CVariantValidator("","config value")));
    RegisterTermCmd(cmd,cfgTree_);

    cmd = new Term_Cmd(this,Term_Cmd_Rap_RlsPort,"server-port","Set server port",PrivilegeLevelAdmin,TermModeConfig,false);
    cmd->Add(new Term_Cmd_Arg("value",CVariant(0),CVariantValidator(0,"config value")));
    RegisterTermCmd(cmd,cfgTree_);

    /* no multicast
    cmd = new Term_Cmd(this,Term_Cmd_Rap_MulticastAddress,"multicast-address","Set multicast address",PrivilegeLevelAdmin,TermModeConfig,false);
    cmd->Add(new Term_Cmd_Arg("value",CVariant(""),CVariantValidator("","config value")));
    RegisterTermCmd(cmd,cfgTree_);

    cmd = new Term_Cmd(this,Term_Cmd_Rap_MulticastPort,"multicast-port","Set multicast port",PrivilegeLevelAdmin,TermModeConfig,false);
    cmd->Add(new Term_Cmd_Arg("value",CVariant(0),CVariantValidator(0,"config value")));
    RegisterTermCmd(cmd,cfgTree_);
    */

    cmd = new Term_Cmd(this,Term_Cmd_Rap_LocalAddress,"local-address","Set local address",PrivilegeLevelAdmin,TermModeConfig,false);
    cmd->Add(new Term_Cmd_Arg("value",CVariant(""),CVariantValidator("","config value")));
    RegisterTermCmd(cmd,cfgTree_);

    cmd = new Term_Cmd(this,Term_Cmd_Rap_LocalPort,"local-port","Set local port",PrivilegeLevelAdmin,TermModeConfig,false);
    cmd->Add(new Term_Cmd_Arg("value",CVariant(0),CVariantValidator(0,"config value")));
    RegisterTermCmd(cmd,cfgTree_);

    cmd = new Term_Cmd(this,Term_Cmd_Rap_MaxRequestAttempt,"request-attempt-max","Set max. attempt per request",PrivilegeLevelAdmin,TermModeConfig,false);
    cmd->Add(new Term_Cmd_Arg("value",CVariant(0),CVariantValidator(0,"config value")));
    RegisterTermCmd(cmd,cfgTree_);

    cmd = new Term_Cmd(this,Term_Cmd_Rap_RequestTimeout,"request-timeout","Set request timeout",PrivilegeLevelAdmin,TermModeConfig,false);
    cmd->Add(new Term_Cmd_Arg("value",CVariant(0),CVariantValidator(0,"config value")));
    RegisterTermCmd(cmd,cfgTree_);

    cmd = new Term_Cmd(this,Term_Cmd_Rap_ResolverThread,"resolver-thread","Set number of resolver worker thread",PrivilegeLevelAdmin,TermModeConfig,false);
    cmd->Add(new Term_Cmd_Arg("value",CVariant(0),CVariantValidator(0,"config value")));
    RegisterTermCmd(cmd,cfgTree_);

    cmd = new Term_Cmd(this,Term_Cmd_Rap_MaxRequestPending,"request-pending-max","Set max. request pending",PrivilegeLevelAdmin,TermModeConfig,false);
    cmd->Add(new Term_Cmd_Arg("value",CVariant(0),CVariantValidator(0,"config value")));
    RegisterTermCmd(cmd,cfgTree_);

    cmd = new Term_Cmd(this,Term_Cmd_Rap_MaxRetryPending,"retry-pending-max","Set max. retry pending",PrivilegeLevelAdmin,TermModeConfig,false);
    cmd->Add(new Term_Cmd_Arg("value",CVariant(0),CVariantValidator(0,"config value")));
    RegisterTermCmd(cmd,cfgTree_);

    cmd = new Term_Cmd(this,Term_Cmd_Rap_MaxReplyPending,"reply-pending-max","Set max. reply pending",PrivilegeLevelAdmin,TermModeConfig,false);
    cmd->Add(new Term_Cmd_Arg("value",CVariant(0),CVariantValidator(0,"config value")));
    RegisterTermCmd(cmd,cfgTree_);

    cfgTree_->RegisterShowHandler(this);
    Kernel::Instance()->RootConfig()->RegisterShowHandler(this);

    return true;
}

bool RapConfigurator::Unload()
{
    delete cfgTree_;
    return true;
}

//STATUS RapConfigurator::OnShowConfig(
bool RapConfigurator::OnShowConfig(
			const Cfg_Tree_Entry*,
			String& output,
			const String& indent,
			int indent_size) const
{
    ostringstream oss;
    string sval;
    int ival;

    oss << indent << "resource-locator" << NEWLINE;

    /* no multicast
    rapConfig_.get("autoDiscovery",ival);
    oss << indent << " auto-discovery" << " " << ival << NEWLINE;
    */

    rapConfig_.get("defaultRmAddress",sval);
    oss << indent << " server-address" << " " << sval << NEWLINE;

    rapConfig_.get("defaultRmPort",ival);
    oss << indent << " server-port" << " " << ival << NEWLINE;

    /** no multicast
    rapConfig_.get("multicastAddress",sval);
    oss << indent << " multicast-address" << " " << sval << NEWLINE;

    rapConfig_.get("multicastPort",ival);
    oss << indent << " multicast-port" << " " << ival << NEWLINE;
    **/

    rapConfig_.get("unicastAddress",sval);
    oss << indent << " local-address" << " " << sval << NEWLINE;

    rapConfig_.get("unicastPort",ival);
    oss << indent << " local-port" << " " << ival << NEWLINE;

    rapConfig_.get("requestRetry",ival);
    oss << indent << " request-attempt-max" << " " << ival << NEWLINE;

    rapConfig_.get("responseTimeout",ival);
    oss << indent << " request-timeout" << " " << ival << NEWLINE;

    rapConfig_.get("numOfNotifierThread",ival);
    oss << indent << " resolver-thread" << " " << ival << NEWLINE;

    rapConfig_.get("maxOutgoingPending",ival);
    oss << indent << " request-pending-max" << " " << ival << NEWLINE;

    rapConfig_.get("maxRepeatPending",ival);
    oss << indent << " retry-pending-max" << " " << ival << NEWLINE;

    rapConfig_.get("maxIncomingPending",ival);
    oss << indent << " reply-pending-max" << " " << ival << NEWLINE;

    oss << indent << (RapManager::Instance()->IsEnabled()?" ":" no ") << "enable" << NEWLINE;

    oss << indent << " top" << NEWLINE << NEWLINE;

    output += oss.str();

    return OK;
}

void RapConfigurator::SendErrorMessage(Term_Session* session,const char* msg)
{
    //ACQUIRE_SYSTEM_LOCK();
    session->SendErrorf(msg);
    //RELEASE_SYSTEM_LOCK();
}

void RapConfigurator::DoTermCmd(Term_Session* session,bool no,Term_Cmd* cmd)
{
/*
    if (no)
    {
	SendErrorMessage(session,"Cannot delete resource-locator configuration" NEWLINE);
	return;
    }
*/

    CVariant v;
    if (cmd->GetArgCount() > 0)
	cmd->GetArg((size_t)0)->GetValue(v);

    switch(cmd->GetId())
    {
    case Term_Cmd_Rap_Config:
	session->SetRoot(cfgTree_);
	break;

    case Term_Cmd_Rap_Enable:
	if (no)
	{
	    if (!DisableRap())
		SendErrorMessage(session,"Cannot disable RAP" NEWLINE);
	}
	else
	{
	    if (!EnableRap())
		SendErrorMessage(session,"Cannot enable RAP" NEWLINE);
	}
	break;

    case Term_Cmd_Rap_AutoDiscovery:
	if (!SetAutoDiscovery(v.AsInt()))
	    SendErrorMessage(session,"Cannot set auto discovery mode" NEWLINE);
	break;

    case Term_Cmd_Rap_RlsAddress:
	if (!SetRlsAddress(v.ToString()))
	    SendErrorMessage(session,"Cannot set server address" NEWLINE);
	break;

    case Term_Cmd_Rap_RlsPort:
	if (!SetRlsPort(v.AsInt()))
	    SendErrorMessage(session,"Cannot set server port" NEWLINE);
	break;

    case Term_Cmd_Rap_MulticastAddress:
	if (!SetMulticastAddress(v.ToString()))
	    SendErrorMessage(session,"Cannot set multicast address" NEWLINE);
	break;

    case Term_Cmd_Rap_MulticastPort:
	if (!SetMulticastPort(v.AsInt()))
	    SendErrorMessage(session,"Cannot set multicast port" NEWLINE);
	break;

    case Term_Cmd_Rap_LocalAddress:
	if (!SetLocalAddress(v.ToString()))
	    SendErrorMessage(session,"Cannot set local address" NEWLINE);
	break;

    case Term_Cmd_Rap_LocalPort:
	if (!SetLocalPort(v.AsInt()))
	    SendErrorMessage(session,"Cannot set local port" NEWLINE);
	break;

    case Term_Cmd_Rap_MaxRequestAttempt:
	if (!SetMaxRequestAttempt(v.AsInt()))
	    SendErrorMessage(session,"Cannot set max. request attempt" NEWLINE);
	break;

    case Term_Cmd_Rap_RequestTimeout:
	if (!SetRequestTimeout(v.AsInt()))
	    SendErrorMessage(session,"Cannot set request timeout" NEWLINE);
	break;

    case Term_Cmd_Rap_ResolverThread:
	if (!SetResolverThread(v.AsInt()))
	    SendErrorMessage(session,"Cannot set resolver thread" NEWLINE);
	break;


    case Term_Cmd_Rap_MaxRequestPending:
	if (!SetMaxRequestPending(v.AsInt()))
	    SendErrorMessage(session,"Cannot set max. request pending" NEWLINE);
	break;

    case Term_Cmd_Rap_MaxRetryPending:
	if (!SetMaxRetryPending(v.AsInt()))
	    SendErrorMessage(session,"Cannot set max. retry pending" NEWLINE);
	break;

    case Term_Cmd_Rap_MaxReplyPending:
	if (!SetMaxReplyPending(v.AsInt()))
	    SendErrorMessage(session,"Cannot set max. reply pending" NEWLINE);
	break;
    }
}

Cfg_Tree_Group* RapConfigurator::GetConfigTree()
{
    return cfgTree_;
}

bool RapConfigurator::SetRlsAddress(const String& val)
{
    return rapConfig_.set("defaultRmAddress",val.c_str());
}

bool RapConfigurator::SetRlsPort(int val)
{
    return rapConfig_.set("defaultRmPort",val);
}

bool RapConfigurator::SetMulticastAddress(const String& val)
{
    return rapConfig_.set("multicastAddress",val.c_str());
}

bool RapConfigurator::SetMulticastPort(int val)
{
    return rapConfig_.set("multicastPort",val);
}

bool RapConfigurator::SetLocalAddress(const String& val)
{
    return rapConfig_.set("unicastAddress",val.c_str());
}

bool RapConfigurator::SetLocalPort(int val)
{
    return rapConfig_.set("unicastPort",val);
}

bool RapConfigurator::SetMaxRequestAttempt(int val)
{
    return rapConfig_.set("requestRetry",val);
}

bool RapConfigurator::SetRequestTimeout(int val)
{
    return rapConfig_.set("responseTimeout",val);
}

bool RapConfigurator::SetMaxRequestPending(int val)
{
    return rapConfig_.set("maxOutgoingPending",val);
}

bool RapConfigurator::SetMaxRetryPending(int val)
{
    return rapConfig_.set("maxRepeatPending",val);
}

bool RapConfigurator::SetMaxReplyPending(int val)
{
    return rapConfig_.set("maxIncomingPending",val);
}

bool RapConfigurator::SetResolverThread(int val)
{
    return rapConfig_.set("numOfNotifierThread",val);
}

bool RapConfigurator::SetAutoDiscovery(int val)
{
    return rapConfig_.set("autoDiscovery",val);
}

bool RapConfigurator::EnableRap()
{
    return RapManager::Instance()->Enable();
}

bool RapConfigurator::DisableRap()
{
    return RapManager::Instance()->Disable();
}
