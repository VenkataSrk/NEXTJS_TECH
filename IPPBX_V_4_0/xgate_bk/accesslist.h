/* $Header: /xGate/accesslist.h 7     6/11/02 3:07p Bennylp $
 *   
 * XGATE
 * (C) 2001 Switchlab, Ltd
 *
 * ACCESS LIST interface.
 *
 * $Log: /xGate/accesslist.h $
 * 
 * 7     6/11/02 3:07p Bennylp
 * Changed ACL_Manager::Compile to static.
 * 
 * 6     8/09/01 6:44p Bennylp
 * Added bypass, dest sig addr, and changed commands.
 * 
 * 5     8/03/01 3:34p Bennylp
 * Split access list execution into several stages.
 * 
 * 4     8/02/01 7:56p Bennylp
 * Validator will accept anything now (problem in system initialization).
 * 
 * 3     8/02/01 6:21p Bennylp
 * Added access list commands.
 * 
 * 2     8/01/01 6:39p Bennylp
 * Barebones for access list management.
 * 
 * 1     8/01/01 10:40a Bennylp
 * Added to source control.
 */

#ifndef __XGATE_ACCESS_H__
#define __XGATE_ACCESS_H__

#include "call.h"
#include <in.h>

/* Forward decl.
 */
class ACL_Entry;
class Cfg_Tree_Group;



///////////////////////////////////////////////////////////////////////////////
/** A compiled ACL token.
    The tokens contains formatted/compiled expressions that will be used for
    later matching. The compiled expressions are divided into several parts,
    that is standard, extended, and extra. The matching process will use
    these compiled expressions in several stages. First it will try to match
    the standard, then if matched, it will try to match the extended,
    and if it still match, it will try to match the extra expression. By using
    several stages, non-matching input will be detected as early as possible.
 */
struct ACL_Token {
    // Compiled standard information.
    // Contains compiled expression that most often be used for matching, such as
    // DDI and CLI. By putting only these informations, non-matching expression
    // can be detected as early as possible.
    char std_compiled[256];
    bool std_bypass;

    // Compiled extended information
    // Contains call direction, media name, group name, and session ID.
    char extd_compiled[1024];
    bool extd_bypass;

    // Compiled extra information.
    // Contains compiled expression of informations that rarely be used, such
    // as user info, and private info.
    char extra_compiled[1024];
    bool extra_bypass;

    // Source signal address
    unsigned long src_addr;

    // Destination signal address.
    unsigned long dst_addr;

    ACL_Token() 
    : std_bypass(false), extd_bypass(false), extra_bypass(false),
      src_addr((unsigned long)-1), dst_addr((unsigned long)-1) 
    {}
};


///////////////////////////////////////////////////////////////////////////////
/** Access list manager manages access lists and provides matching service
    of some parameters (such as CallParameter) against an access list entry.
 */
class ACL_Manager : public ServiceProvider {
public:
    /** Create the singleton instance.
     */
    static ACL_Manager * Create();

    /** Get the singleton instance.
     */
    static ACL_Manager * Instance() {
	return theInstance;
    }

    /** Destructor
     */
    ~ACL_Manager();

    /** Initialize.
     */
    virtual bool Load();

    /** Deinitialize.
     */
    virtual bool Unload();

    /** Determine whether the specified ACL name exists.
     */
    bool IsExist(const String & acl_name) const {
	return aclEntries.find(acl_name) != aclEntries.end();
    }

    /** Build a string token for later matching from the arguments.
	This is efficient if the same token is used to match against several ACL
	tokens.
     */
    static bool Compile(
	ACL_Token & result,		// output token
	CallDirection dir,		// call direction
	const CallParameter & cparam,	// call parameter
	const char *media_name,		// media name
	const char *group_name		// group name
    );

    /** Match the specified access list against a match token.
	The match token is built using Compile() function.
     */
    bool IsMatch(
	const char *acl_name,
	const ACL_Token & token
    ) const;

    /** Match the arguments against an ACL.
     */
    bool IsMatch(
	const char *acl_name,
	CallDirection dir,		// call direction
	const CallParameter & cparam,	// call parameter
	const char *media_name,		// media name
	const char *group_name		// group name
    ) const;

    /** Terminal command callback
     */
    virtual void DoTermCmd(Term_Session *, bool no, Term_Cmd *);

    /** Show configuration for all ACL entries.
     */
    virtual bool OnShowConfig(
	const Cfg_Tree_Entry *entry,
	String & output, 
	const String & indent, 
	int indent_size) const;

    /** Show the configuration of an ACL entry.
     */
    void ShowAccessList(const IString & acl_name, Term_Session *session);

    /** Get list of choices for validator.
     */
    const List<ValidatorChoice> & GetValidatorChoiceList() const {
	return aclChoiceList;
    }

    /** Get the ACL entry (internal use only).
     */
    ACL_Entry * FindEntry(const IString & name) {
	Map<IString, ACL_Entry *>::iterator it = aclEntries.find(name);
	if (it!=aclEntries.end())
	    return it->second;
	return NULL;
    }

private:
    static ACL_Manager		*theInstance;
    static in_addr		 inaddrDontCare;
    Map <IString, ACL_Entry *>	 aclEntries;
    Cfg_Tree_Group		*aclConfigTree;
    List<ValidatorChoice>	 aclChoiceList;

    ACL_Manager();
};


///////////////////////////////////////////////////////////////////////////////
/** This class validates choices given by terminal user against the access 
    lists managed in Access_List_Manager.
 */
class ACL_Validator : public CVariantDynamicChoiceValidator {
public:
    /** Default constructor.
     */
    ACL_Validator() {}

    /** This will always return true.
     */
    virtual bool Validate(const CVariant&) const {
	return true;
    }

    /** This will always return true.
     */
    virtual bool Suggest(CVariant & value) const {
	CVariantDynamicChoiceValidator::Suggest(value);
	return true;
    }

    /** Acquire mutex lock.
	This function will be called by the framework BEFORE calling
	GetChoiceList(). The lock will be kept until this class is finished
	processing all the choices in the choice list.
     */
    virtual void AcquireLock() const;

    /** Release mutex lock.
	This function will be called by the framework AFTER calling
	GetChoiceList().
     */
    virtual void ReleaseLock() const;

    /** Get list of choices available.
     */
    virtual const List<ValidatorChoice> & GetChoiceList() const {
	return ACL_Manager::Instance()->GetValidatorChoiceList();
    }

    /** Create a duplicate */
    virtual CVariantValidator *Clone() const {
	return new ACL_Validator;
    }
};


#endif
