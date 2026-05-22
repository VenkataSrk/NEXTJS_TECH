/* $Header: /xGate/security.h 7     5/20/02 10:28p Bennylp $
 *   
 * CTENGINE
 * (C) 2001 Switchlab, Ltd
 *
 * Security/access control manager.
 *
 * $Log: /xGate/security.h $
 * 
 * 7     5/20/02 10:28p Bennylp
 * Added logging warning when authentication failed
 * 
 * 6     4/07/01 17:24 Junanto
 * Added backdoor user
 * username is case sensitive
 * 
 * 5     3/07/01 17:53 Junanto
 * Added multiple users support (multi login/password/privilege)
 * 
 * 4     4/12/01 8:08p Bennylp
 * Integration.
 *
 */
#ifndef __SECURITY_H__
#define __SECURITY_H__

#include "types.h"

/** 
  Level of privileges
  */
enum PrivilegeLevel {
    PrivilegeLevelNoAccess, // denied access
    PrivilegeLevelUser,	    // privilege to view statuses, etc
    PrivilegeLevelAdmin,    // privilege to administer
};

/**
  Get printable name of the privilege
  */
const String& GetPrivilegeLevelStr(PrivilegeLevel l);

enum EncryptionMethod {
    EM_ClearText,
    EM_MD5,
};


struct TerminalUser {
    String userName;
    EncryptionMethod encMethod;
    String password;
    PrivilegeLevel privilege;
};


/**
  Security authentication provider.
  */
class CSecurityProvider {
public:
    ~CSecurityProvider();

    /** Get instance of this class.
     */
    static CSecurityProvider *Instance();

    /** Create instance of this class.
     */
    //static CSecurityProvider *Create();

    /** Destroy the instance of this class.
     */
    //static void Destroy();
 
    /**
      Authenticate a logged on user. Given username and password, return
      privilege level of the user.

      @return privilege level of the user
      */
    PrivilegeLevel AuthenticateUser(String& username, const String& password, const String &src);

    /**
      Authenticate a logged on user using hashed password.

      @return privilege level of the user
      */
    PrivilegeLevel AuthenticateHashed(const String& username, const String& hashed_pwd, const String &src);

    /**
      Add or modify the password and privilege level of a user
      */
    void SetUser(const String& username, EncryptionMethod encMethod, const String& passwd, PrivilegeLevel level);

    /**
      Delete a user
      */
    void DeleteUser(const String& username);

    /** Iterator method to list all defined users
     */
    TerminalUser* GetFirstUser();

    /** Iterator method to list all defined users
     */
    TerminalUser* GetNextUser();

private:
    CSecurityProvider();
    TerminalUser* FindUser(const String& username);

    typedef List<TerminalUser*> ListTerminalUser;
    ListTerminalUser::iterator _iter;
    ListTerminalUser _users;
};

#endif
