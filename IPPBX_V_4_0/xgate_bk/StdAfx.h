// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__8285A1C6_F34D_11D2_B117_00A00CC00595__INCLUDED_)
#define AFX_STDAFX_H__8285A1C6_F34D_11D2_B117_00A00CC00595__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#pragma warning(disable:4786)	// indentifier truncated in debug information
#pragma warning(disable:4355)	// 'this' : used in base member initializer list
//#pragma warning(disable:4511 4512 4514 4710)
//#pragma warning(push, 1)
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers



//
// SIP precompiles
// This will include ACE library.
//
#include "../slsip/include/stdafx.h"


// Win32
#include <winsock2.h>
#include <windows.h>


// ANSI/stdlib
#include <assert.h>
#include <io.h>
#include <time.h>
#include <stdio.h>
#include <conio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h> 
#include <stdarg.h>
#include <process.h>
#include <sys/timeb.h>
#include <sys/stat.h>
#include <math.h>


// ODBC
#include <sql.h>
#include <sqlext.h>

// STL
#include <string>
#include <queue>
#include <list>
#include <vector>
#include <map>


/* Check FD_SETSIZE size.
   This is needed if SIP is to use TCP transport.
 */
#if FD_SETSIZE < 512
   #error "FD_SETSIZE is too small"
#endif

#include <ptlib.h>

#ifdef XGATE_HAS_OPENH323
	//openh323 stuff
	#undef List
	#undef EINPROGRESS
	#undef ECONNRESET
	#undef EISCONN
//	#pragma message ("including OpenH323...")
	#include <h323ep.h>
	#include <h323con.h>
	#include <channels.h>
	#include <mediafmt.h>
	#include <lid.h>//for OPAL LID capabilities
//	#pragma message ("defining List as std::list")
	#define List std::list
#endif //XGATE_HAS_OPENH323

#ifdef XGATE_HAS_LEAKDETECTOR
	#include <vld.h>//visual leak detector
	#include <vldapi.h>//visual leak detector api
#endif

//#define TRACE_ALLOC
//#include "memtrack.h"

/* xGate project identifier.
   Should be defined in the project.
 */
#ifndef __XGATE__
  #define __XGATE__
#endif



#endif // !defined(AFX_STDAFX_H__8285A1C6_F34D_11D2_B117_00A00CC00595__INCLUDED_)
