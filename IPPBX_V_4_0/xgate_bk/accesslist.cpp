/* $Header: /xGate/accesslist.cpp 10    10/21/02 6:38p Bennylp $
 *   
 * XGATE
 * (C) 2001 Switchlab, Ltd
 *
 * ACCESS LIST implementation.
 *
 * $Log: /xGate/accesslist.cpp $
 * 
 * 11    01/09/15 6:38p okY
 * Improve accesslist parsin algorithm
 *
 * 10    10/21/02 6:38p Bennylp
 * Added comment for try-catch block.
 * 
 * 9     6/11/02 3:07p Bennylp
 * Changed ACL_Manager::Compile to static.
 * 
 * 8     8/28/01 1:57p Bennylp
 * Fixed bug in IP address parsing
 * 
 * 7     8/09/01 6:44p Bennylp
 * Added bypass, dest sig addr, and changed commands.
 * 
 * 6     8/03/01 3:34p Bennylp
 * Split access list execution into several stages.
 * 
 * 5     8/02/01 7:57p Bennylp
 * Miscelanous bug fixes.
 * 
 * 4     8/02/01 6:21p Bennylp
 * Added access list commands.
 * 
 * 3     8/01/01 7:00p Bennylp
 * Missing theInstance definition.
 * 
 * 2     8/01/01 6:39p Bennylp
 * Barebones for access list management.
 * 
 * 1     8/01/01 10:40a Bennylp
 * Added to source control.
 */
//#include "stdafx.h"
#include "accesslist.h"
#include "cfg.h"
#include "kernel.h"
#include "term.h"
//#include "pcre-3.4/pcreposix.h"	// Regex
#include <regex.h>

#define THISMODULE  "ACL"
#define __linux__


enum CallDialType {
    Dial_Enblock,
    Dial_Overlap,
    Dial_Anything
};

#define ACL_DEFAULT_STRING  ".*"


///////////////////////////////////////////////////////////////////////////////
/** Tool function to get the IP address from hostname/IP address string.
 */
static unsigned long get_ip_addr(const char *host)
{
    if (!*host)
	return 0xFFFFFFFF;

    unsigned long ip = inet_addr(host);
    if (ip!=INADDR_NONE)
	return ip;

    hostent *he = gethostbyname(host);
    if (!he)
	return 0xFFFFFFFF;
    return *(unsigned long*)he->h_addr;
}


///////////////////////////////////////////////////////////////////////////////
/** Tool function to normalize pattern.
 */
const char *NormalizePattern(const char *pattern)
{
    static char normalized[1024];
    const char *src = pattern;
    char *dst = normalized;

    *dst = '\0';

    // remove leading whitespaces
    while (*src && isspace(*src))
	++src;

    if (!*src)
	return normalized;

    // if the first char is not '^', then append ".*" if it is not ".*" already.
    // otherwise remove the first '^' char.
    if (*src=='^') {
	//skip
	++src;
    } else if (*src=='.' && *(src+1)=='*') {
	*dst++ = *src++;
	*dst++ = *src++;
    } else {
	*dst++ = '.';
	*dst++ = '*';
    }

    // copy the rest of the pattern until end or until we find '$'
    while (*src) {
	if (*src=='$' && src!=pattern && *(src-1)!='\\')
	    break;
	*dst++ = *src++;
    }

    // If the end of the pattern is '$', remove it.
    // Otherwise append ".*" if the previous chars is not ".*"
    if (*src=='$' && src!=pattern && *(src-1)!='\\') {
	// do nothing
    } else if (src > pattern+2 && *(src-1)!='*' && *(src-2)!='.') {
	*dst++ = '.';
	*dst++ = '*';
    }

    *dst = '\0';
    return normalized;
}


///////////////////////////////////////////////////////////////////////////////

inline bool expr_tokenize(const char *name, const char *value,
			  char *end_buffer, char * & buffer)
{
#define BEGIN_TOKEN	"%<"
#define BEGIN_TOKEN_LEN	2
#define END_TOKEN	"%>;"
#define END_TOKEN_LEN	3

    int name_len = strlen(name);
    int val_len = strlen(value);

    // Format of the token is:
    //	name=%<value%>
    //
    // length of the resulted string, not including the terminating zero
    int compiled_len = name_len + val_len + BEGIN_TOKEN_LEN + END_TOKEN_LEN + 1;

    // check that compiled_len plus terminating zero does not exceed buffer length
    if (compiled_len+1 > end_buffer-buffer)
	return false;

    strcpy(buffer, name);
    buffer += name_len;
    *buffer++ = '=';
    strcpy(buffer, BEGIN_TOKEN);
    buffer += BEGIN_TOKEN_LEN;
    strcpy(buffer, value);
    buffer += val_len;
    strcpy(buffer, END_TOKEN);
    buffer += END_TOKEN_LEN;
    *buffer = '\0';    

    return true;

#undef BEGIN_TOKEN
#undef BEGIN_TOKEN_LEN
#undef END_TOKEN
#undef END_TOKEN_LEN
}

static const String ExpAll(".*");

/** Compile standard info.
 */
bool CompileExpr_Std(  ACL_Token & token,	    // token result
		       CallDirection dir,	    // call direction
		       const CallParameter & cparam,// call parameter
		       CallDialType dial_type,	    // enblock / overlap, overriding settings in cparam
		       const char *media_name,	    // media name
		       const char *group_name,	    // group name
		       bool is_pattern
		    )
{
    char *p = token.std_compiled, *endbuf = token.std_compiled+sizeof(token.std_compiled);
    const char *normalized;
    
    // If all input patterns is equal to ".*", then set the bypass flag
    if (is_pattern && cparam.cnCalled==ExpAll && cparam.cnCalling==ExpAll) {
	token.std_bypass = true;
	*p = '\0';
    } else {
	token.std_bypass = false;

	// DDI, come first because it is the most often used.
	normalized = is_pattern ? NormalizePattern(cparam.cnCalled.c_str()) : cparam.cnCalled.c_str();
	if (!expr_tokenize("ddi", normalized, endbuf, p))
	    goto on_error;

	// CLI
	normalized = is_pattern ? NormalizePattern(cparam.cnCalling.c_str()) : cparam.cnCalling.c_str();
	if (!expr_tokenize("cli", normalized, endbuf, p))
	    goto on_error;
    }
    return true;

on_error:
    CLog::Error(THISMODULE, "ACL: compiled expiression is too long");
    return false;
}


/** Compile extended info.
 */
bool CompileExpr_Extd( ACL_Token & token,	    // token result
		       CallDirection dir,	    // call direction
		       const CallParameter & cparam,// call parameter
		       CallDialType dial_type,	    // enblock / overlap, overriding settings in cparam
		       const char *media_name,	    // media name
		       const char *group_name,	    // group name
		       bool is_pattern
		     )
{
    char *p = token.extd_compiled, *endbuf = token.extd_compiled+sizeof(token.extd_compiled);
    const char *normalized;

    // If all input patterns is equal to ".*", then set the bypass flag
    if (is_pattern && ExpAll==media_name && ExpAll==group_name && ExpAll==cparam.csi && dir==CD_Both) {
	token.extd_bypass = true;
	*p = '\0';
    } else {
	token.extd_bypass = false;

	// Media
	normalized = is_pattern ? NormalizePattern(media_name) : media_name;
	if (!expr_tokenize("med", normalized, endbuf, p))
	    goto on_error;

	// Group
	normalized = is_pattern ? NormalizePattern(group_name) : group_name;
	if (!expr_tokenize("grp", normalized, endbuf, p))
	    goto on_error;

	// Session id
	normalized = is_pattern ? NormalizePattern(cparam.csi.c_str()) : cparam.csi.c_str();
	if (!expr_tokenize("sid", normalized, endbuf, p))
	    goto on_error;

	// Direction
	if (!expr_tokenize("dir", (dir==CD_Both ? ".*" : (dir==CD_Incoming ? "i" : "o")),
			   endbuf, p))
	    goto on_error;
    }

    return true;

on_error:
    CLog::Error(THISMODULE, "ACL: compiled expiression is too long");
    return false;
}

/** Compile extra info.
 */
bool CompileExpr_Extra(ACL_Token & token,		    // token result
		       CallDirection dir,		    // call direction
		       const CallParameter & cparam,    // call parameter
		       CallDialType dial_type,	    // enblock / overlap, overriding settings in cparam
		       const char *media_name,	    // media name
		       const char *group_name,	    // group name
		       bool is_pattern
		       )
{
    char *p = token.extra_compiled, *endbuf = token.extra_compiled+sizeof(token.extra_compiled);
    const char *normalized;

    // If all input patterns is equal to ".*", then set the bypass flag
    if (is_pattern && ExpAll==cparam.cui && ExpAll==cparam.cpinfo && dial_type==Dial_Anything && ExpAll==cparam.csCalling && ExpAll==cparam.csCalled) {
	token.extra_bypass = true;
	*p = '\0';
    } else {
	token.extra_bypass = false;

	// User-info
	normalized = is_pattern ? NormalizePattern(cparam.cui.c_str()) : cparam.cui.c_str();
	if (!expr_tokenize("ui", normalized, endbuf, p))
	    goto on_error;

	// Private-info
	normalized = is_pattern ? NormalizePattern(cparam.cpinfo.c_str()) : cparam.cpinfo.c_str();
	if (!expr_tokenize("pi", normalized, endbuf, p))
	    goto on_error;
    
	// Complete/overlap/either
	normalized = (dial_type==Dial_Anything ? ".*" : (dial_type==Dial_Enblock ? "e" : "o"));
	if (!expr_tokenize("enb", normalized, endbuf, p))
	    goto on_error;

	// Src-subaddr
	normalized = is_pattern ? NormalizePattern(cparam.csCalling.c_str()) : cparam.csCalling.c_str();
	if (!expr_tokenize("ssub", normalized, endbuf, p))
	    goto on_error;

	// Dst-subaddr
	normalized = is_pattern ? NormalizePattern(cparam.csCalled.c_str()) : cparam.csCalled.c_str();
	if (!expr_tokenize("dsub", normalized, endbuf, p))
	    goto on_error;
    }
    return true;

on_error:
    CLog::Error(THISMODULE, "ACL: compiled expiression is too long");
    return false;
}


///////////////////////////////////////////////////////////////////////////////
/** Individual ACL entry.
 */
class ACL_Entry : public Term_Cmd_Target {
public:
    ACL_Entry(const IString & name);
    ~ACL_Entry();

    /** Initialize.
     */
    bool Init (Cfg_Tree_Group *parent);

    /** Match the token input.
     */
    bool MatchToken (const ACL_Token & token) const;

    /** Match the IP addresses part.
     */
    bool MatchIP(unsigned long src_addr, unsigned long dst_addr) const {
	// Match the source address, unless it's equal to 255.255.255.255
	if (src_addr != (unsigned long)-1) {
	    unsigned long masked_addr = src_addr & aclSrcAddrMask.s_addr;
	    if (masked_addr != (aclSrcAddr.s_addr & aclSrcAddrMask.s_addr))
		return false;
	}

	// Match destination address, unless it's equal to 255.255.255.255
	if (dst_addr != (unsigned long)-1) {
	    unsigned long masked_addr = dst_addr & aclDstAddrMask.s_addr;
	    if (masked_addr != (aclDstAddr.s_addr & aclDstAddrMask.s_addr))
		return false;
	}
	return true;
    }

    /** Get the name.
     */
    const IString & Name() const {
	return aclName;
    }

    /** Get the ACL token.
     */
    const ACL_Token & GetToken() const {
	return aclToken;
    }

    /** Determine whether there are chained ACLs.
     */
    bool HasChains() const {
	return aclChain.size() != 0;
    }

    /** Terminal command callback
     */
    virtual void DoTermCmd(Term_Session *, bool no, Term_Cmd *);

    /** Show configuration for the specified entry.
     */
    virtual bool OnShowConfig(
	const Cfg_Tree_Entry *entry,
	String & output, 
	const String & indent, 
	int indent_size) const;

    /** Get the config tree.
     */
    Cfg_Tree_Group * GetConfigTree() {
	return aclConfigTree;
    }

    /** Show configuration entry without acquiring system lock.
     */
    bool OnShowConfig_NoLock(
	const Cfg_Tree_Entry *entry,
	String & output, 
	const String & indent, 
	int indent_size,
	bool display_all) const;

private:
    IString	    aclName;
    String	    aclDesc;
    ACL_Token	    aclToken;
    Cfg_Tree_Group *aclConfigTree;
    mutable regex_t aclStdRegex, aclExtdRegex, aclExtraRegex;
    CallDirection   aclDir;
    CallDialType    aclDialType;
    CallParameter   aclParam;
    String	    aclMedia, aclGroup;
    in_addr	    aclSrcAddr, aclSrcAddrMask;
    in_addr	    aclDstAddr, aclDstAddrMask;

    struct Chained_ACL {
	bool    is_accept;
	IString	name;

	Chained_ACL() : is_accept(true) {}
	Chained_ACL(const Chained_ACL & rhs)
	    : is_accept(rhs.is_accept), name(rhs.name) {}
	Chained_ACL & operator = (const Chained_ACL & rhs) {
	    is_accept = rhs.is_accept;
	    name = rhs.name;
	    return *this;
	}
	bool operator == (const Chained_ACL & rhs) {
	    return is_accept==rhs.is_accept && name==rhs.name;
	}
    };

    List<Chained_ACL> aclChain;

    enum {
	Cmd_AccessList,
	Cmd_Desc,
	Cmd_Dir,
	Cmd_Media,
	Cmd_Group,
	Cmd_SessionId,
	Cmd_CLI,
	Cmd_DDI,
	Cmd_UserInfo,
	Cmd_PrivateInfo,
	Cmd_Subaddr,
	Cmd_IP,
	Cmd_DisplayExpr,
    };

    bool Recompile() {
	ACL_Token acl_token;
	if (!::CompileExpr_Std(acl_token, aclDir, aclParam, aclDialType, aclMedia.c_str(), aclGroup.c_str(), true))
	    return false;

	if (!::CompileExpr_Extd(acl_token, aclDir, aclParam, aclDialType, aclMedia.c_str(), aclGroup.c_str(), true))
	    return false;

	if (!::CompileExpr_Extra(acl_token, aclDir, aclParam, aclDialType, aclMedia.c_str(), aclGroup.c_str(), true))
	    return false;

	regex_t std_reg, extd_reg, extra_reg;
	if (::regcomp(&std_reg, acl_token.std_compiled, REG_ICASE | REG_EXTENDED)) {
	    CLog::Warning(THISMODULE, "Error compiling regular expression");
	    return false;
	}
	if (::regcomp(&extd_reg, acl_token.extd_compiled, REG_ICASE | REG_EXTENDED)) {
	    CLog::Warning(THISMODULE, "Error compiling regular expression");
	    ::regfree(&std_reg);
	    return false;
	}
	if (::regcomp(&extra_reg, acl_token.extra_compiled, REG_ICASE | REG_EXTENDED)) {
	    CLog::Warning(THISMODULE, "Error compiling regular expression");
	    ::regfree(&std_reg);
	    ::regfree(&extd_reg);
	    return false;
	}

	::regfree(&aclStdRegex);
	::regfree(&aclExtdRegex);
	::regfree(&aclExtraRegex);

	aclStdRegex = std_reg;
	aclExtdRegex = extd_reg;
	aclExtraRegex = extra_reg;

	aclToken = acl_token;
	return true;
    }

    bool IsCircular(const List<IString> & previous) const;
};



ACL_Entry::ACL_Entry(const IString & name)
: aclName(name), aclConfigTree(NULL)
{
    
    aclSrcAddr.s_addr = aclSrcAddrMask.s_addr = aclDstAddr.s_addr = aclDstAddrMask.s_addr = 0;

    aclDialType = Dial_Anything;
    aclDir = CD_Both;
    aclMedia = aclGroup = aclParam.csi = aclParam.cnCalled = aclParam.cnCalling =
    aclParam.cui = aclParam.cpinfo = aclParam.csCalled = aclParam.csCalling =  ACL_DEFAULT_STRING;
    
    memset(&aclStdRegex, 0, sizeof(aclStdRegex));
    memset(&aclExtdRegex, 0, sizeof(aclExtdRegex));
    memset(&aclExtraRegex, 0, sizeof(aclExtraRegex));

    Recompile();
} 


ACL_Entry::~ACL_Entry()
{
    ::regfree(&aclStdRegex);
    ::regfree(&aclExtdRegex);
    ::regfree(&aclExtraRegex);
}


bool ACL_Entry::Init (Cfg_Tree_Group *parent)
{
    aclConfigTree = new Cfg_Tree_Group(aclName, "", parent);
    aclConfigTree->RegisterShowHandler(this);

    Term_Cmd *cmd;
    
    // Description
    cmd = new Term_Cmd(this, Cmd_Desc,
		       "description",
		       "Description of this access list",
		       PrivilegeLevelAdmin,
		       TermModeConfig,
		       true);
    cmd->Add(new Term_Cmd_Arg("@0", "", CVariantValidator("", "Description to this access list")));
    RegisterTermCmd(cmd, aclConfigTree);

    // Access list
    cmd = new Term_Cmd(this, Cmd_AccessList,
		       "access-list",
		       "Specify chained access lists to permit/deny",
		       PrivilegeLevelAdmin,
		       TermModeConfig,
		       true);
    cmd->Add(new Term_Cmd_Arg("@0", "", ACL_Validator()));
    CVariantChoiceValidator accVdtor("permit", "Accept the access list if matched");
    accVdtor.Add("deny", "Deny the access list if matched");
    cmd->Add(new Term_Cmd_Arg("@1", "", accVdtor));
    RegisterTermCmd(cmd, aclConfigTree);

    // Direction
    CVariantChoiceValidator dirVdtor("incoming", "Incoming call");
    dirVdtor.Add("outgoing", "Outgoing call");
    dirVdtor.Add("either", "Both incoming or outgoing call");
    cmd = new Term_Cmd (this, Cmd_Dir,
		        "direction",
			"Direction of the call",
			PrivilegeLevelAdmin,
			TermModeConfig,
			false);
    cmd->Add(new Term_Cmd_Arg("@0", "", dirVdtor));
    RegisterTermCmd(cmd, aclConfigTree);

    // Media
    cmd = new Term_Cmd(this, Cmd_Media,
		       "media",
		       "Pattern to be matched in the media name or interface name",
		       PrivilegeLevelAdmin,
		       TermModeConfig,
		       false);
    cmd->Add(new Term_Cmd_Arg("@0", "", CVariantValidator("", "Pattern to be matched in the media name or interface name")));
    RegisterTermCmd(cmd, aclConfigTree);

    // Group
    cmd = new Term_Cmd(this, Cmd_Group,
		       "group",
		       "Pattern to be matched in group name",
		       PrivilegeLevelAdmin,
		       TermModeConfig,
		       false);
    cmd->Add(new Term_Cmd_Arg("@0", "", CVariantValidator("", "Pattern to be matched in group name")));
    RegisterTermCmd(cmd, aclConfigTree);

    // Session ID
    cmd = new Term_Cmd(this, Cmd_SessionId,
		       "session-id",
		       "Pattern to be matched in session ID",
		       PrivilegeLevelAdmin,
		       TermModeConfig,
		       false);
    cmd->Add(new Term_Cmd_Arg("@0", "", CVariantValidator("", "Pattern to be matched in session ID")));
    RegisterTermCmd(cmd, aclConfigTree);

    // CLI
    cmd = new Term_Cmd(this, Cmd_CLI,
		       "cli",
		       "Pattern to be matched in calling/source number",
		       PrivilegeLevelAdmin,
		       TermModeConfig,
		       false);
    cmd->Add(new Term_Cmd_Arg("@0", "", CVariantValidator("", "Pattern to be matched in calling/source number")));
    RegisterTermCmd(cmd, aclConfigTree);

    // DDI
    cmd = new Term_Cmd(this, Cmd_DDI,
		       "ddi",
		       "Pattern to be matched in called/destination number",
		       PrivilegeLevelAdmin,
		       TermModeConfig,
		       false);
    cmd->Add(new Term_Cmd_Arg("@0", "", CVariantValidator("", "Pattern to be matched in called/destination number")));
    RegisterTermCmd(cmd, aclConfigTree);

    // User info
    cmd = new Term_Cmd(this, Cmd_UserInfo,
		       "user-info",
		       "Pattern to be matched in user info",
		       PrivilegeLevelAdmin,
		       TermModeConfig,
		       false);
    cmd->Add(new Term_Cmd_Arg("@0", "", CVariantValidator("", "Pattern to be matched in user info")));
    RegisterTermCmd(cmd, aclConfigTree);

    // Private info
    cmd = new Term_Cmd(this, Cmd_PrivateInfo,
		       "private-info",
		       "Pattern to be matched in private info",
		       PrivilegeLevelAdmin,
		       TermModeConfig,
		       false);
    cmd->Add(new Term_Cmd_Arg("@0", "", CVariantValidator("", "Pattern to be matched in private info")));
    RegisterTermCmd(cmd, aclConfigTree);

    CVariantChoiceValidator addrVdtor("source", "Source address specification");
    addrVdtor.Add("destination", "Destination address specification");

    // subaddr info
    cmd = new Term_Cmd(this, Cmd_Subaddr,
		       "sub-address",
		       "Pattern to be matched in subaddress",
		       PrivilegeLevelAdmin,
		       TermModeConfig,
		       false);
    cmd->Add(new Term_Cmd_Arg("@0", "", addrVdtor));
    cmd->Add(new Term_Cmd_Arg("@1", "", CVariantValidator("", "Pattern to be matched in subaddress")));
    RegisterTermCmd(cmd, aclConfigTree);

    // IP address
    cmd = new Term_Cmd(this, Cmd_IP,
		       "ip",
		       "IP address specification",
		       PrivilegeLevelAdmin,
		       TermModeConfig,
		       false);
    cmd->Add(new Term_Cmd_Arg("@0", "", addrVdtor));
    cmd->Add(new Term_Cmd_Arg("@1", "", CVariantValidator("", "IP address")));
    cmd->Add(new Term_Cmd_Arg("@2", "", CVariantValidator("", "Address mask")));
    RegisterTermCmd(cmd, aclConfigTree);

    // Display expression
    cmd = new Term_Cmd(this, Cmd_DisplayExpr,
		       "display",
		       "Display the compiled expression",
		       PrivilegeLevelAdmin,
		       TermModeConfig,
		       false);
    RegisterTermCmd(cmd, aclConfigTree);
    return true;
}


/* Match this access list against a precompiled token.
 */
bool ACL_Entry::MatchToken (const ACL_Token & acl_token) const
{
    if (!MatchIP(acl_token.src_addr, acl_token.dst_addr))
	return false;

    // Match the standard part.
    if (!acl_token.std_bypass && regexec(&aclStdRegex, acl_token.std_compiled, 0, NULL, 0) == REG_NOMATCH)
	return false;

    // Match the extended part.
    if (!acl_token.extd_bypass && regexec(&aclExtdRegex, acl_token.extd_compiled, 0, NULL, 0) == REG_NOMATCH)
	return false;

    // Match the extra part.
    if (!acl_token.extra_bypass && regexec(&aclExtraRegex, acl_token.extra_compiled, 0, NULL, 0) == REG_NOMATCH)
	return false;

    // Match the chained list.
    if (!HasChains())
	return true;

    List<Chained_ACL>::const_iterator it, end;
    end = aclChain.end();

    for (it=aclChain.begin(); it!=end; ++it) {
	ACL_Entry *child = ACL_Manager::Instance()->FindEntry(it->name);
	if (child) {
	    bool success = child->MatchToken(acl_token);
	    if (it->is_accept) {
		if (!success)
		    return false;
	    } else {
		if (success)
		    return false;
	    }
	} else if (it->is_accept) {
	    // The entry is not found.
	    CLog::Warning(THISMODULE, "Access-list '%s' is not found (referenced by access list '%s')",
				      it->name.c_str(), aclName.c_str());
	    return false;
	}
    }

    return true;
}


/* Determine that a chained accept list specification will generate a circular
   dependency.
 */
bool ACL_Entry::IsCircular(const List<IString> & previous) const
{
    // Make sure we're not in the list.
    List<IString>::const_iterator it, end;
    end = previous.end();
    for (it=previous.begin(); it!=end; ++it) {
	if (*it == aclName)
	    return true;
    }

    // Copy list (not optimal!!) and put ourself in the new list.
    List<IString> next = previous;
    next.push_back(aclName);

    List<Chained_ACL>::const_iterator cit, cend;
    cend = aclChain.end();

    for (cit=aclChain.begin(); cit!=cend; ++cit) {
	ACL_Entry *child = ACL_Manager::Instance()->FindEntry(cit->name);
	if (child==NULL) {
	    CLog::Warning (THISMODULE, "Access list '%s' not found (referenced by access list '%s')",
				       cit->name.c_str(), aclName.c_str());
	    continue;
	}
	if (child->IsCircular(next))
	    return true;
    }
    return false;
}


void ACL_Entry::DoTermCmd(Term_Session *session, bool no, Term_Cmd *cmd)
{
    CVariant arg, arg1, arg2;
    IString is;
    String s;

    switch(cmd->GetId()) {
    case Cmd_AccessList:
	if (ACQUIRE_SYSTEM_LOCK()) {
	    Chained_ACL chain;

	    cmd->GetArg((size_t)0)->GetValue(arg);
	    chain.name = arg.AsString();

	    cmd->GetArg(1)->GetValue(arg);
	    if (!strcasecmp(arg.AsString(), "permit")) {
		chain.is_accept = true;
	    } else
		chain.is_accept = false;

	    if (no) {
		List<Chained_ACL>::iterator it = std::find(aclChain.begin(), aclChain.end(), chain);
		if (it==aclChain.end()) {
		    RELEASE_SYSTEM_LOCK();
		    session->SendErrorf("The specified access list was not found\n");
		    return;
		}
		aclChain.erase(it);
	    } else {
		// Give warning if access list name is not found.
		bool acl_found = true;

		// Verify circular dependency in the entry.
		ACL_Entry *entry = ACL_Manager::Instance()->FindEntry(chain.name);
		if (entry) {
		    List<IString> previous1;
		    previous1.push_back(aclName);
		    if (entry->IsCircular(previous1)) {
			RELEASE_SYSTEM_LOCK();
			session->SendErrorf("Access list '%s' is already in the chain.\n", chain.name.c_str());
			return;
		    }
		} else {
		    acl_found = false;
		}

		// Verify circular dependency in us.
		List<IString> previous2;
		previous2.push_back(chain.name);
		if (IsCircular(previous2)) {
		    RELEASE_SYSTEM_LOCK();
		    session->SendErrorf("Access list '%s' is already in the chain.\n", chain.name.c_str());
		    return;
		}

		// Add to chain
		aclChain.push_back(chain);

		if (!acl_found) {
		    RELEASE_SYSTEM_LOCK();
		    session->Sendf("Warning: access list '%s' is not found\n", chain.name.c_str());
		    ACQUIRE_SYSTEM_LOCK();
		}
	    }
	    RELEASE_SYSTEM_LOCK();
	}
	break;
    case Cmd_Desc:
	if (ACQUIRE_SYSTEM_LOCK()) {
	    if (no)
		aclDesc.erase();
	    else {
		CVariant arg;
		cmd->GetArg((size_t)0)->GetValue(arg);
		aclDesc = arg.AsString();
	    }
	    RELEASE_SYSTEM_LOCK();
	}
	break;
    case Cmd_Dir:
	cmd->GetArg((size_t)0)->GetValue(arg);
	is = arg.AsString();

	if (ACQUIRE_SYSTEM_LOCK()) {
	    CallDirection old_dir = aclDir;
	    if (is=="incoming") {
		aclDir = CD_Incoming;
	    } else if (is=="outgoing") {
		aclDir = CD_Outgoing;
	    } else
		aclDir = CD_Both;

	    if (!Recompile())
		aclDir = old_dir;

	    RELEASE_SYSTEM_LOCK();
	}
	break;
    case Cmd_Media:
	cmd->GetArg((size_t)0)->GetValue(arg);

	if (ACQUIRE_SYSTEM_LOCK()) {
	    String old_media = aclMedia;
	    aclMedia = arg.AsString();
	    if (!Recompile())
		aclMedia = old_media;
	    RELEASE_SYSTEM_LOCK();
	}
        break;
    case Cmd_Group:
	cmd->GetArg((size_t)0)->GetValue(arg);

	if (ACQUIRE_SYSTEM_LOCK()) {
	    String old_group = aclGroup;
	    aclGroup = arg.AsString();
	    if (!Recompile())
		aclGroup = old_group;
	    RELEASE_SYSTEM_LOCK();
	}
        break;
    case Cmd_SessionId:
	cmd->GetArg((size_t)0)->GetValue(arg);

	if (ACQUIRE_SYSTEM_LOCK()) {
	    String old_sid = aclParam.csi;
	    aclParam.csi = arg.AsString();
	    if (!Recompile())
		aclParam.csi = old_sid;
	    RELEASE_SYSTEM_LOCK();
	}
        break;
    case Cmd_CLI:
	cmd->GetArg((size_t)0)->GetValue(arg);

	if (ACQUIRE_SYSTEM_LOCK()) {
	    String old_cli = aclParam.cnCalling;
	    aclParam.cnCalling = arg.AsString();
	    if (!Recompile())
		aclParam.cnCalling = old_cli;
	    RELEASE_SYSTEM_LOCK();
	}
        break;
    case Cmd_DDI:
	cmd->GetArg((size_t)0)->GetValue(arg);

	if (ACQUIRE_SYSTEM_LOCK()) {
	    String old_ddi = aclParam.cnCalled;
	    aclParam.cnCalled = arg.AsString();
	    if (!Recompile())
		aclParam.cnCalled = old_ddi;
	    RELEASE_SYSTEM_LOCK();
	}
        break;
    case Cmd_UserInfo:
	cmd->GetArg((size_t)0)->GetValue(arg);

	if (ACQUIRE_SYSTEM_LOCK()) {
	    String old_ui = aclParam.cui;
	    aclParam.cui = arg.AsString();
	    if (!Recompile())
		aclParam.cui = old_ui;
	    RELEASE_SYSTEM_LOCK();
	}
        break;
    case Cmd_PrivateInfo:
	cmd->GetArg((size_t)0)->GetValue(arg);

	if (ACQUIRE_SYSTEM_LOCK()) {
	    String old_pi = aclParam.cpinfo;
	    aclParam.cpinfo = arg.AsString();
	    if (!Recompile())
		aclParam.cpinfo = old_pi;
	    RELEASE_SYSTEM_LOCK();
	}
        break;
    case Cmd_Subaddr:
	cmd->GetArg((size_t)0)->GetValue(arg);
	cmd->GetArg((size_t)1)->GetValue(arg1);

	if (ACQUIRE_SYSTEM_LOCK()) {
	    if (!strcasecmp(arg.AsString(), "source")) {
		String old_sub = aclParam.csCalling;
		aclParam.csCalling = arg1.AsString();
		if (!Recompile())
		    aclParam.csCalling = old_sub;
	    } else {
		String old_sub = aclParam.csCalled;
		aclParam.csCalled = arg1.AsString();
		if (!Recompile())
		    aclParam.csCalled = old_sub;
	    }
	    RELEASE_SYSTEM_LOCK();
	}
        break;
    case Cmd_IP:
	cmd->GetArg((size_t)0)->GetValue(arg);
	cmd->GetArg((size_t)1)->GetValue(arg1);
	cmd->GetArg((size_t)2)->GetValue(arg2);

	if (ACQUIRE_SYSTEM_LOCK()) { 
	    if (!strcasecmp(arg.AsString(), "source")) {
		aclSrcAddr.s_addr = get_ip_addr(arg1.AsString());
		aclSrcAddrMask.s_addr = inet_addr(arg2.AsString());
	    } else {
		aclDstAddr.s_addr = get_ip_addr(arg1.AsString());
		aclDstAddrMask.s_addr = inet_addr(arg2.AsString());
	    }
	    RELEASE_SYSTEM_LOCK();
	}
        break;
    case Cmd_DisplayExpr:
	ACL_Token token;
	String output;
	String indent(4, ' ');

	ACQUIRE_SYSTEM_LOCK();
	token = aclToken;
	OnShowConfig_NoLock(aclConfigTree, output, indent, 2, true);
	RELEASE_SYSTEM_LOCK();

	session->Sendf("Complete parameters:\n");
	session->Send(output);

	session->Sendf("Compiled expressions:\n");
	session->Sendf("  Std  : %s\n", token.std_bypass ? "(bypass)" : token.std_compiled);
	session->Sendf("  Extnd: %s\n", token.extd_bypass ? "(bypass)" : token.extd_compiled);
	session->Sendf("  Extra: %s\n", token.extra_bypass ? "(bypass)" : token.extra_compiled);
	session->Send("\n", 1);
	break;
    }
}


bool ACL_Entry::OnShowConfig(const Cfg_Tree_Entry *entry,
			       String & output, 
			       const String & indent, 
			       int indent_size) const
{
    ACQUIRE_SYSTEM_LOCK();
    bool rc = OnShowConfig_NoLock(entry, output, indent, indent_size, false);
    RELEASE_SYSTEM_LOCK();
    return rc;
}


bool ACL_Entry::OnShowConfig_NoLock(const Cfg_Tree_Entry *entry,
				      String & output, 
				      const String & indent, 
				      int indent_size,
				      bool display_all) const
{
    // Permit and deny list.
    List<Chained_ACL>::const_iterator it, end;
    end = aclChain.end();
    for (it=aclChain.begin(); it!=end; ++it) {
	output += indent + "access-list " + it->name + (it->is_accept ? " permit\n" : " deny\n");
    }

    // CLI
    if (display_all || aclParam.cnCalling != ACL_DEFAULT_STRING)
	output += indent + "cli " + aclParam.cnCalling + "\n";

    // DDI
    if (display_all || aclParam.cnCalled != ACL_DEFAULT_STRING)
	output += indent + "ddi " + aclParam.cnCalled + "\n";

    // Description
    if (display_all || aclDesc.size())
	output += indent + "description \"" + aclDesc + "\"\n";

    // Direction
    if (display_all || aclDir != CD_Both) {
	switch (aclDir) {
	case CD_Incoming:
	    output += indent + "direction incoming\n";
	    break;
	case CD_Outgoing:
	    output += indent + "direction outgoing\n";
	    break;
	case CD_Both:
	    output += indent + "direction either\n";
	    break;
	}
    }

    // Group
    if (display_all || aclGroup != ACL_DEFAULT_STRING)
	output += indent + "group " + aclGroup + "\n";

    // Dst signal addr
    if (display_all || aclDstAddr.s_addr != 0 || aclDstAddrMask.s_addr != 0) {
	char addr[32], mask[32];
	strcpy(addr, inet_ntoa(aclDstAddr));
	strcpy(mask, inet_ntoa(aclDstAddrMask));

	output += indent + "ip destination " + addr + " " + mask + "\n";
    }
    // Src signal addr
    if (display_all || aclSrcAddr.s_addr != 0 || aclSrcAddrMask.s_addr != 0) {
	char addr[32], mask[32];
	strcpy(addr, inet_ntoa(aclSrcAddr));
	strcpy(mask, inet_ntoa(aclSrcAddrMask));

	output += indent + "ip source " + addr + " " + mask + "\n";
    }

    // Media
    if (display_all || aclMedia != ACL_DEFAULT_STRING)
	output += indent + "media " + aclMedia + "\n";

    // Private info
    if (display_all || aclParam.cpinfo != ACL_DEFAULT_STRING)
	output += indent + "private-info \"" + aclParam.cpinfo + "\"\n";

    // Session-ID
    if (display_all || aclParam.csi != ACL_DEFAULT_STRING)
	output += indent + "session-id " + aclParam.csi + "\n";

    // Dst subaddr
    if (display_all || aclParam.csCalled != ACL_DEFAULT_STRING)
	output += indent + "sub-address destination \"" + aclParam.csCalled + "\"\n";

    // Src subaddr
    if (display_all || aclParam.csCalling != ACL_DEFAULT_STRING)
	output += indent + "sub-address source \"" + aclParam.csCalling + "\"\n";

    // User info
    if (display_all || aclParam.cui != ACL_DEFAULT_STRING)
	output += indent + "user-info \"" + aclParam.cui + "\"\n";

    output += indent + "top\n\n";
    return OK;
}


///////////////////////////////////////////////////////////////////////////////

ACL_Manager * ACL_Manager::theInstance;
in_addr ACL_Manager::inaddrDontCare;

enum {
    Mgr_Cmd_Acl,
};


ACL_Manager * ACL_Manager::Create()
{
    if (theInstance==NULL)
	theInstance = new ACL_Manager;
    return theInstance;
}


ACL_Manager::ACL_Manager()
: ServiceProvider("ACL-Mgr", false, false), aclConfigTree(NULL)
{
    aclConfigTree = new Cfg_Tree_Group("access-list", "", Kernel::Instance()->RootConfig());
    Kernel::Instance()->RootConfig()->RegisterShowHandler(this);
    inaddrDontCare.s_addr = 0xFFFFFFFF;
}

ACL_Manager::~ACL_Manager()
{
    theInstance = NULL;
}


bool ACL_Manager::Load()
{

    Term_Cmd *cmd;
    cmd = new Term_Cmd (this, Mgr_Cmd_Acl,
			"access-list",
			"Access list filter",
			PrivilegeLevelAdmin,
			TermModeConfig,
			true);
    cmd->Add(new Term_Cmd_Arg("@0", "", CVariantValidator("", "Access list name")));
    RegisterTermCmd(cmd, Kernel::Instance()->RootConfig());
    return true;
}


bool ACL_Manager::Unload()
{
    // Delete access lists
    Map<IString,ACL_Entry*>::iterator it, end;
    end = aclEntries.end();
    for (it=aclEntries.begin(); it!=end; ++it) {
	delete it->second;
    }
    aclEntries.clear();

    while (aclChoiceList.size())
	aclChoiceList.pop_back();

    return true;
}


/** Match the specified access list against a match token.
    The match token is built using BuildMatchToken function.
 */
bool ACL_Manager::IsMatch( const char *acl_name,
			   const ACL_Token & acl_token) const
{
    Map <IString, ACL_Entry *>::const_iterator it = aclEntries.find(acl_name);
    if (it == aclEntries.end()) {
	CLog::Warning(THISMODULE, "Access list '%s' is not found", acl_name);
	return false;
    }
    return it->second->MatchToken(acl_token);
}


/** Match the arguments against an ACL.
 */
bool ACL_Manager::IsMatch(  const char *acl_name,
			    CallDirection dir,		    // call direction
			    const CallParameter & cparam,   // call parameter
			    const char *media_name,	    // media name
			    const char *group_name	    // group name
			 ) const
{
    Map <IString, ACL_Entry *>::const_iterator it = aclEntries.find(acl_name);
    if (it == aclEntries.end()) {
	CLog::Warning(THISMODULE, "Access list '%s' is not found", acl_name);
	return false;
    }
    
    const ACL_Entry *entry = it->second;
    const ACL_Token & entry_token = entry->GetToken();

    // Check if bypass flag are all set and entry doesn't have ACL chains.
    // If so, then don't need to compile the arguments, just check the IP
    // addresses.
    if (entry_token.std_bypass && entry_token.extd_bypass && entry_token.extra_bypass && !entry->HasChains()) {
	const char *s;
	const char *src = ((s=strchr(cparam.csaCalling.c_str(), ':')) != NULL) ? s : cparam.csaCalling.c_str();
	const char *dst = ((s=strchr(cparam.csaCalled.c_str(), ':')) != NULL) ? s : cparam.csaCalled.c_str();

	unsigned long src_addr = get_ip_addr(src);
	unsigned long dst_addr = get_ip_addr(dst);

	return entry->MatchIP(src_addr, dst_addr);
    } else {
	ACL_Token input;
	if (!Compile(input, dir, cparam, media_name, group_name))
	    return false;
	return it->second->MatchToken(input);
    }
}

/** Build a string token for later matching from the arguments.
    This is useful if the same arguments are used to match against multiple
    access list, to make the matching more efficient.
 */
bool ACL_Manager::Compile(  ACL_Token & token,
			    CallDirection dir,			// call direction
			    const CallParameter & cparam,	// call parameter
			    const char *media_name,		// media name
			    const char *group_name		// group name
			    )
{
    CallDialType dial_type = cparam.complete ? Dial_Enblock : Dial_Overlap;

    String src_ip = cparam.csaCalling;
    String dst_ip = cparam.csaCalled;
	
    src_ip=src_ip.substr(src_ip.find_first_not_of("sip:"), String::npos);
    src_ip=src_ip.substr(src_ip.find_first_not_of("sips:"), String::npos);
    src_ip=src_ip.substr(0, src_ip.find(':'));

    dst_ip=dst_ip.substr(dst_ip.find_first_not_of("sip:"), String::npos);
    dst_ip=dst_ip.substr(dst_ip.find_first_not_of("sips:"), String::npos);
    dst_ip=dst_ip.substr(0, dst_ip.find(':'));


    //String src_ip = cparam.csaCalling.substr(0, cparam.csaCalling.find(':'));
    //String dst_ip = cparam.csaCalled.substr(0, cparam.csaCalled.find(':'));

    token.src_addr = get_ip_addr(src_ip.c_str());
    token.dst_addr = get_ip_addr(dst_ip.c_str());
    CLog::Debug(THISMODULE, "ACL_Manager::Compile: src_ip: %s, dst_ip: %s, token.src_addr: %d, token.dst_addr: %d",
      src_ip.c_str(), dst_ip.c_str(), token.src_addr, token.dst_addr);

    if (!CompileExpr_Std(token, dir, cparam, dial_type, media_name, group_name, false))
	return false;
    if (!CompileExpr_Extd(token, dir, cparam, dial_type, media_name, group_name, false))
	return false;
    if (!CompileExpr_Extra(token, dir, cparam, dial_type, media_name, group_name, false))
	return false;

    return true;
}


/** Terminal command callback
 */
void ACL_Manager::DoTermCmd(Term_Session *session, bool no, Term_Cmd *cmd)
{
    CVariant arg;
    IString acl_name;

    switch (cmd->GetId()) {
    case Mgr_Cmd_Acl:
	cmd->GetArg((size_t)0)->GetValue(arg);
	acl_name = arg.AsString();
	if (ACQUIRE_SYSTEM_LOCK()) {
	    if (no) {
		Map <IString, ACL_Entry *>::iterator it = aclEntries.find(acl_name);
		bool found = (it != aclEntries.end());
		if (!found) {
		    RELEASE_SYSTEM_LOCK();
		    session->SendErrorf("ACL entry '%s' was not found\n", acl_name.c_str());
		    return;
		}

		RELEASE_SYSTEM_LOCK();
		bool confirmed = !session->GetStream().IsInteractive() ||
				 session->PromptYesNo(false, "Delete ACL entry '%s' ? (y/n)", acl_name.c_str());
		ACQUIRE_SYSTEM_LOCK();

		if (confirmed) {
		    //aclMutex.acquire();
		    it = aclEntries.find(acl_name);
		    if (it != aclEntries.end()) {
			delete it->second;
			aclEntries.erase(it);

			// delete validator choice
			List<ValidatorChoice>::iterator vit, vend;
			vend = aclChoiceList.end();
			for (vit=aclChoiceList.begin(); vit!=vend; ++vit) {
			    if (acl_name == vit->first.AsString()) {
				aclChoiceList.erase(vit);
				break;
			    }
			}
		    }
		    //aclMutex.release();
		}

	    } else {
		Map <IString, ACL_Entry*>::iterator it = aclEntries.find(acl_name);
		if (it != aclEntries.end()) {
		    session->SetRoot(it->second->GetConfigTree());
		} else {
		    RELEASE_SYSTEM_LOCK();
		    bool confirmed = !session->GetStream().IsInteractive() ||
				     session->PromptYesNo(false, "Create ACL entry '%s' ? (y/n)", acl_name.c_str());
		    ACQUIRE_SYSTEM_LOCK();

		    if (confirmed) {
			ACL_Entry *acl;
			NEW_RETURN_(acl, ACL_Entry(acl_name));
			if (acl==NULL) {
			    RELEASE_SYSTEM_LOCK();
			    return;
			}

			bool success = false;
			/* This try/catch block is used to prevent crash during
			   low-memory situation. It's not intended as guard against bugs.
			 */
			try {
			    success = acl->Init(aclConfigTree);
			} catch(...) {
			    success = false;
			}

			if (!success) {
			    delete acl;
			    RELEASE_SYSTEM_LOCK();
			    session->SendErrorf("Error initializing ACL entry\n");
			    return;
			}

			aclEntries[acl_name] = acl;
			aclChoiceList.push_back(ValidatorChoice(acl_name.c_str(), ""));

			session->SetRoot(acl->GetConfigTree());
		    }
		}
	    }
	    RELEASE_SYSTEM_LOCK();
	}
	break;
    }
}


/** Show configuration for the specified entry.
 */
bool ACL_Manager::OnShowConfig(
    const Cfg_Tree_Entry *entry,
    String & output, 
    const String & indent, 
    int indent_size) const
{
   // pthread_mutex_t acl_lock;
 
//#ifdef __linux__
    ACQUIRE_SYSTEM_LOCK();
/*#else
    pthread_mutex_lock(&acl_lock);
#endif */

    Map<IString, ACL_Entry*>::const_iterator it, end;
    end = aclEntries.end();
    String newindent(indent);
    newindent.append(indent_size, ' ');

    for (it=aclEntries.begin(); it!=end; ++it) {
	output += indent + "access-list " + it->first + "\n";
	it->second->OnShowConfig_NoLock(NULL, output, newindent, indent_size, false);
    }
//#ifdef __linux__
    RELEASE_SYSTEM_LOCK();
/*#else
    pthread_mutex_unlock(&acl_lock);
#endif */  
    return OK;
}


/** Show the configuration of an ACL entry.
 */
void ACL_Manager::ShowAccessList(const IString & acl_name, Term_Session *session)
{
    ACQUIRE_SYSTEM_LOCK();
    ACL_Entry *entry = FindEntry(acl_name);
    if (entry == NULL) {
	RELEASE_SYSTEM_LOCK();
	session->SendErrorf("Access list '%s' was not found\n");
	return;
    }

    String output;
    String newindent;
    newindent.append(4, ' ');
    output += String(2, ' ') + "access-list " + entry->Name() + "\n";
    entry->OnShowConfig_NoLock(NULL, output, newindent, 4, true);
    RELEASE_SYSTEM_LOCK();

    session->Send(output);
}


///////////////////////////////////////////////////////////////////////////////

/** Acquire mutex lock.
 */
void ACL_Validator::AcquireLock() const
{
    ACQUIRE_SYSTEM_LOCK();
}


/** Release mutex lock.
 */
void ACL_Validator::ReleaseLock() const
{
    RELEASE_SYSTEM_LOCK();
}

