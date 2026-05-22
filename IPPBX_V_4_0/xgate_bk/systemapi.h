/* $Header: /ct15/systemapi.h 2     4/13/01 11:18p Bennylp $
 *   
 * CTENGINE
 * (C) 2001 Switchlab, Ltd
 *
 * Provides miscelanous API to the applications.
 *
 * $Log: /ct15/systemapi.h $
 * 
 * 2     4/13/01 11:18p Bennylp
 * Integration.
 */
#ifndef __SYSTEMAPI_H__
#define __SYSTEMAPI_H__

#  define _ftime ftime
#  define _timeb timeb



/* Forward decl.
 */
class CSystemAPI;


#include "serviceprovider.h"

class System_API_Provider : public ServiceProvider {
public:
    /** Create the instance of this class.
     */
    static System_API_Provider * Create();

    /** This function will be called by the service framework after 
        instantiation of this class, to allow the instance to initialize
	itself.
     */
    virtual bool Load();

    /** This function will be called by the service framework before it is
        destroyed/deleted, to allow the instance to cleanup itself.
     */
    virtual bool Unload();


private:
    static System_API_Provider *sysapiInstance;
    CSystemAPI *api;

    System_API_Provider();
    ~System_API_Provider();
};

//wrapper function
char* str_toupper(char* s) 
{ 
  assert(s != (void*)0); 
  while(*s) 
  { 
    *s = toupper((unsigned char)*s); 
     s++; 
  } 
  return s; 
}

#endif
