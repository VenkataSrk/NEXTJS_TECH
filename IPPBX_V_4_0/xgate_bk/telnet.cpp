/*
 * telnet.cxx
 *
 * TELNET socket I/O channel class.
 *
 * Portable Windows Library
 *
 * Copyright (c) 1993-2002 Equivalence Pty. Ltd.
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is Portable Windows Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: telnet.cxx,v $
 * Revision 1.10  2002/11/06 22:47:25  robertj
 * Fixed header comment (copyright etc)
 *
 * Revision 1.9  2002/09/18 06:38:59  robertj
 * Fixed initialisation of debug flag, thanks wolfboy@netease.com
 *
 * Revision 1.8  2001/09/10 02:51:23  robertj
 * Major change to fix problem with error codes being corrupted in a
 *   PChannel when have simultaneous reads and writes in threads.
 *
 * Revision 1.7  1998/11/30 04:52:11  robertj
 * New directory structure
 *
 * Revision 1.6  1998/09/23 06:22:47  robertj
 * Added open source copyright license.
 *
 * Revision 1.5  1998/01/26 02:49:23  robertj
 * GNU support.
 *
 * Revision 1.4  1997/07/14 11:47:18  robertj
 * Added "const" to numerous variables.
 *
 * Revision 1.3  1996/08/08 10:08:48  robertj
 * Directory structure changes for common files.
 *
 * Revision 1.2  1996/05/26 03:47:08  robertj
 * Compatibility to GNU 2.7.x
 *
 * Revision 1.1  1996/03/04 12:12:51  robertj
 * Initial revision
 *
 */

//
//THIS FILE HAS BEEN MODIFIED BY SWITCHLAB
//functions added:
//MyWrite(),close(),gethandle(),iopen(),myread/write, writeoutofband
//member variables added:
//lastreadcount,lastwritecount,os_handle
//

//#include <stdafx.h>

#ifdef __linux__
#include <iostream>
#include <sys/socket.h>
#include <sys/poll.h>
#include <errno.h>
#include "sl.h"
using namespace std;
using namespace sl;
#endif 

//#include <ptlib.h>
//#include <ptlib/sockets.h>
//#include <ptclib/telnet.h>
#ifdef __linux__
#define TRUE true
#define FALSE false
#include "Critical_Section.h"
//#include "contain.h"
//#include "/home/murali/xgate_sn/branches/Sofia/core/xgate/telnet.h"
#include "telnet.h"
#include "array.h"
#include "pstring.h"
#endif


#ifndef min
  #define min(a,b)    ((a) < (b) ? (a) : (b))
#endif

#if 0
PAbstractArray::PAbstractArray(int, void const*, int, int){

}
#endif

class Circular_Buffer
{
    enum {
	CIRCBUF_SIZE = 32768,
    };

    size_t head, tail, count;
    char buffer[CIRCBUF_SIZE];
    Critical_Section cs;

public:
    Circular_Buffer() {
	head = tail = count = 0;
    }

    size_t put(const char* buf, size_t len)
    {
	Lock l(cs);

	// adjust length to a maximum we can support
	len = min(len, CIRCBUF_SIZE-count);
	if (len == 0) return 0;

	// see if we go pass the end of buffer
	if (tail+len > CIRCBUF_SIZE) {
	    memcpy(buffer+tail, buf, CIRCBUF_SIZE-tail);
	    memcpy(buffer, buf+CIRCBUF_SIZE-tail, tail+len-CIRCBUF_SIZE);
	} else {
	    memcpy(buffer+tail, buf, len);
	}

	// adjust internal variables
	count += len;
	tail = (tail+len) % CIRCBUF_SIZE;

	return len;
    }

    size_t peek(const char** buf)
    {
	Lock l(cs);

	size_t len = min(count, CIRCBUF_SIZE-head);
	*buf = buffer + head;

	return len;
    }

    size_t consume(size_t len)
    {
	Lock l(cs);

	count -= len;
	head = (head + len) % CIRCBUF_SIZE;

	return count;
    }
};


//
//SWITCHLAB STUFF
//BOOL PTelnetSocket::MyWrite(const unsigned char* buf, int len)
bool PTelnetSocket::MyWrite(const unsigned char* buf, int len)
{
    if (!IsOpen())
	return FALSE;

    const char* errmsg = "\r\n\r\ndisplay too fast or terminal too slow, disconnected!\r\n";

    // if it is the TELNET thread then write it directly to the socket
   #ifndef __linux__
    if (self_thread_id == 0  || self_thread_id == GetCurrentThreadId()) {
	#else
    if (self_thread_id == 0  || self_thread_id == pthread_self()) {
	#endif
	for (int i=0; i<10; i++) {
	    lastWriteCount = send(os_handle, (const char*)buf, len, 0);
#ifndef __linux__
	    if (lastWriteCount != len) {
		if (WSAGetLastError() != WSAEWOULDBLOCK) {
		    Close();
		    return FALSE;
		}
#else
		
	    if (lastWriteCount != len) {
		int tmperrno = errno;
		if (tmperrno !=EWOULDBLOCK) {
		    Close();
		    return FALSE;
		}
#endif

#ifndef __linux__
		Sleep(100);
#else
		sl::Sleep(100);
#endif

		// instance might be closed in between, check again
		if (!IsOpen())
		    return FALSE;

	    } else {
		return TRUE;
	    }
	}

	send(os_handle, errmsg, strlen(errmsg), 0);
//	Sleep(100);
#ifndef __linux__
		Sleep(100);
#else
		sl::Sleep(100);
#endif

	Close();
	return FALSE;
    }

    // otherwise, use circular buffer
    for (int i=0; len && i<4; i++) {
	size_t l = cb->put((const char*)buf, len);
	if (l == len) {

#ifndef __linux__
	    SetEvent(events[1]);
#else
	// NLI	

#endif // __linux_
	    return TRUE;
	}

//	Sleep(100);
#ifndef __linux__
		Sleep(100);
#else
		sl::Sleep(100);
#endif
	len -= l;
	buf += l;

	// instance might be closed in between, check again
	if (!IsOpen())
	    return FALSE;
    }

    // instance might be closed in between, check again
    if (!IsOpen())
	return FALSE;

    send(os_handle, errmsg, strlen(errmsg), 0);

    Close();
    return FALSE;

}

//BOOL PTelnetSocket::WriteOutOfBand(const unsigned char *buf, int len)
bool PTelnetSocket::WriteOutOfBand(const unsigned char *buf, int len)
{
    if (!IsOpen())
	return FALSE;
    
    int written = send(os_handle, (char*)buf, len, MSG_OOB);
    if (written < 1) {
	Close();
	return FALSE;
    }
    return TRUE;
}

void PTelnetSocket::SetReadTimeout(unsigned newval)
{
#ifndef __linux__
    read_timeout = (newval == 0) ? WSA_INFINITE : newval;
#else
    read_timeout = (newval == 0) ? -1 : newval; // I have assumed, poll to be used for checking any events 
#endif
}


int PTelnetSocket::GetHandledummy(void) const
{
#ifndef __linux__
    read_timeout = (newval == 0) ? WSA_INFINITE : newval;
#else
//    read_timeout = (newval == 0) ? -1 : newval; // I have assumed, poll to be used for checking any events 
	 return os_handle;
#endif
}

//BOOL PTelnetSocket::MyRead(unsigned char *buf, int len)
bool PTelnetSocket::MyRead(unsigned char *buf, int len)
{
    if (!IsOpen())
	return FALSE;

#ifndef __linux__
    if (self_thread_id == 0)
	self_thread_id = GetCurrentThreadId();
#else
    if (self_thread_id == 0)
	self_thread_id = pthread_self();
#endif

    while (1) {

#ifndef __linux__
	// wait for either input becomes available or something needs to be sent
	DWORD rc = WSAWaitForMultipleEvents(2, events, 0, read_timeout, 0);
	if (!IsOpen())
	    return FALSE;

	if (rc == WSA_WAIT_TIMEOUT) {
	    const char* timeout = "\r\n\r\nTimeout expired!\r\n";
	    send(os_handle, timeout, strlen(timeout), 0);
	    Sleep(100);
	    Close();
	    return FALSE;
	}
#else
	bool need_read = false;
	bool need_write = false;

#if 0
	struct pollfd efds[2];
	efds[0].fd = events[0];
	efds[1].fd = events[1];
	efds[0].events = POLLIN | POLLPRI;
	efds[1].events = POLLOUT;
#else
	struct pollfd efds;
	efds.fd = os_handle;
//	efds[1].fd = events[1];
	efds.events = POLLIN | POLLPRI | POLLOUT;
#endif
	int res = poll(&efds,1,read_timeout);
	if(res  == 0){
	    const char* timeout = "\r\n\r\nTimeout expired!\r\n";
	    send(os_handle, timeout, strlen(timeout), 0);
	    Sleep(100);
	    Close();
	    return FALSE;
	}else if(res < 0){

		//cout << "Poll error " << errno << endl;
	}else if(res > 0){
		
#if 0
		if((efds[0].revents  & POLLIN) || (efds[1].revents  & POLLPRI))
			need_read = true;
		if((efds[1].revents & POLLOUT))
			need_write = true;
#else

		if((efds.revents  & POLLIN) || (efds.revents  & POLLPRI))
			need_read = true;
		if((efds.revents & POLLOUT))
			need_write = true;
	

#endif

	}
	
	

#endif


#ifndef __linux__
	bool need_read = false;
	bool need_write = false;
	int index = rc - WSA_WAIT_EVENT_0;
	switch (index) {
	case 0:
	    {
		WSANETWORKEVENTS ev;
		if (WSAEnumNetworkEvents(os_handle, events[0], &ev) != 0) {
		    Close();
		    return FALSE;
		}
		need_read = (ev.lNetworkEvents & (FD_READ|FD_CLOSE|FD_OOB)) != 0;
		need_write = (ev.lNetworkEvents & FD_WRITE) != 0;
	    }
	    break;

	case 1:
	    need_write = true;
	    break;

	default:
	    // strange!!!
	    Sleep(100);
	    continue;
	}
#endif

	// send buffer becomes available or there's pending output in circular buffer
	if (need_write) {
	    const char* buf;
	    size_t len = cb->peek(&buf);
	    if (len) {
		int written = send(os_handle, buf, len, 0);
		if (written < 1) {
#ifndef __linux__
		    if (WSAGetLastError() != WSAEWOULDBLOCK) {
#else
		int tmperrno = errno;
		    if (tmperrno  != EWOULDBLOCK) {
#endif
			Close();
			return FALSE;
		    }
		} else {
		    cb->consume(written);
		}
	    }
	}

	// some input is available to read
	if (need_read) {
	    lastReadCount = recv(os_handle, (char *)buf, len, 0);
	    if (lastReadCount<1) {
		Close();
		return FALSE;
	    }
	    return TRUE;
	}
	usleep(1000); //__linux__ //added to reduce the load of the server as POLLOUT is always returning true
    }
}

void PTelnetSocket::Close()
{
    opened = FALSE;

#ifndef __linux__
    // and wake up MyRead thread
    SetEvent(events[1]);
#else
    //cout << "****************** Inside PTelnetSocket ************"<< endl;
// NLI

#endif
}

PTelnetSocket::~PTelnetSocket()
{
 

//#ifndef __linux__
    //closesocket(os_handle);
    close(os_handle);
    os_handle = -1;

    delete cb;
    cb = 0;

    //CloseHandle(events[0]);
    //pthread_mutex_destroy(&events)
    events[0] = 0;

    ///CloseHandle(events[1]);
    events[1] = 0;

/* #else
    close(os_handle);
    os_handle = -1;

    delete cb;
    cb = 0;

#endif */
}

//BOOL PTelnetSocket::IsOpen()
bool PTelnetSocket::IsOpen()
{
    return opened;
}

PTelnetSocket::PTelnetSocket(int sock)
{
    int soctopt=1;
    setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (const char*)&soctopt, sizeof(soctopt));
    
    os_handle = sock;
    cb = new Circular_Buffer;

#ifndef __linux__ //__linux__
    events[0] = CreateEvent(0, 0, 0, 0);
    events[1] = CreateEvent(0, 0, 0, 0);
    self_thread_id = 0;
    read_timeout = WSA_INFINITE;
    WSAEventSelect(os_handle, events[0], FD_READ|FD_CLOSE|FD_OOB|FD_WRITE);
#else
    opened = true;

#endif
    
    Construct();
}

//
//end of SWITCHLAB STUFF
//

//////////////////////////////////////////////////////////////////////////////
// PTelnetSocket
//

void PTelnetSocket::SetOurOption(
      Options code,
//      unsigned char code,          // Option to check.
      bool state = true// New state for for option.
) {
    //cout << "***** Insisde SetOurOption ****"<< endl;  
    option[code].weCan = state; 
  }
//

void PTelnetSocket::SetTheirOption(
     unsigned char code,          // Option to check.
    // BOOL state = TRUE  // New state for for option.
      bool state = true// New state for for option.     //linux porting
){
   //cout << "**** Inside  SetTheirOPTION ***" << endl; 
    option[code].theyShould = state; 
 }


bool PTelnetSocket::IsOurOption(
      BYTE code    // Option to check.
    ) const { return option[code].ourState == OptionInfo::IsYes; }


bool PTelnetSocket::IsTheirOption(
    BYTE code    // Option to check.
    ) const{ return option[code].theirState == OptionInfo::IsYes; }




void PTelnetSocket::Construct()
{
  synchronising = 0;
//  terminalType = "UNKNOWN"; //SWITCHLAB
  terminalType = "VT100";
  windowWidth = windowHeight = 0;
  state = StateNormal;

  memset(option, 0, sizeof(option)); 

  SetOurOption(TransmitBinary);
  SetOurOption(SuppressGoAhead);
  SetOurOption(StatusOption);
  SetOurOption(TimingMark);
  SetOurOption(TerminalSpeed);
  SetOurOption(TerminalType);
  SetTheirOption(TransmitBinary);
  SetTheirOption(SuppressGoAhead);
  SetTheirOption(StatusOption);
  SetTheirOption(TimingMark);
  SetTheirOption(EchoOption); 


//SWITCHLAB
  //cout << "****** GTC SendDo SuppressGoAhead ****"<< endl;
  SendDo(SuppressGoAhead);
//  SendDo(RCTEOption);
  //SendDo(EchoOption);
  
  //cout << "****** GTC SendWill EchoOption ****"<< endl;
  SendWill(EchoOption);
//  SendWill(EchoOption);
  //SendWill(1);
  SendWill(StatusOption);

#ifdef _DEBUG
  debug = TRUE;
#else
  debug = FALSE;
#endif
}

#ifndef __linux__

#define PTelnetError if (debug) PError << "PTelnetSocket: "
#define PDebugError if (debug) PError

#else

//#define PTelnetError if (debug) PError << "PTelnetSocket: "
//#define PDebugError if (debug) PError
#define PTelnetError  cerr << "PTelnetSocket: "
#define PDebugError cerr

#endif


//SL MODIFIED
//BOOL PTelnetSocket::Accept(int & sock)
bool PTelnetSocket::Accept(int & sock)
{  
  sockaddr_in addr;
  int len=sizeof(addr);

  os_handle=accept(sock, (sockaddr*)&addr, &len);
  if (os_handle==-1)
      return FALSE;

  SendDo(SuppressGoAhead);
  SendWill(StatusOption);
  return TRUE;
}


//BOOL PTelnetSocket::Write(void const * buffer, PINDEX length)
bool PTelnetSocket::Write(void const * buffer, PINDEX length)
{
  const BYTE * base = (const BYTE *)buffer;
  const BYTE * next = base;
  int count = 0;

  while (length > 0) {

#ifdef __linux__
    if (*next == '\r' &&
            !(length > 1 && next[1] == '\n') && !IsOurOption(TransmitBinary)) {
#else
    if (*next == '\r' &&
            !(length > 1 && next[1] == '\n')) {
#endif
      // send the characters
      if (!MyWrite(base, (next - base) + 1))
        return FALSE;
      count += lastWriteCount;

      char null = '\0';
      if (!MyWrite((const unsigned char*)&null, 1))
        return FALSE;
      count += lastWriteCount;

      base = next+1;
    }

    if (*next == IAC) {
      // send the characters
      if (!MyWrite(base, (next - base) + 1))
        return FALSE;
      count += lastWriteCount;
      base = next;
    }

    next++;
    length--;
  }

  if (next > base) {
    if (!MyWrite(base, next - base))
      return FALSE;
    count += lastWriteCount;
  }

  lastWriteCount = count;
  return TRUE;
}


//BOOL PTelnetSocket::SendCommand(Command cmd, int opt)
bool PTelnetSocket::SendCommand(Command cmd, int opt)
{
  BYTE buffer[3];
  buffer[0] = IAC;
  buffer[1] = (BYTE)cmd;

  switch (cmd) {
    case DO :
    case DONT :
    case WILL :
    case WONT :
      buffer[2] = (BYTE)opt;
      return MyWrite(buffer, 3);

    case InterruptProcess :
    case Break :
    case AbortProcess :
    case SuspendProcess :
    case AbortOutput :
      if (opt) {
        // Send the command
        if (!MyWrite(buffer, 2))//SL
          return FALSE;
        // Send a TimingMark for output flush.
        buffer[1] = TimingMark;
        if (!MyWrite(buffer, 2))//SL
          return FALSE;
        // Send a DataMark for synchronisation.
        if (cmd != AbortOutput) {
          buffer[1] = DataMark;
          if (!MyWrite(buffer, 2))
            return FALSE;
          // Send the datamark character as the only out of band data byte.
          if (!WriteOutOfBand(&buffer[1], 1))
            return FALSE;
        }
        // Then flush any waiting input data.
        while (MyRead(buffer, sizeof(buffer)))//SL
          ;
      }
      break;

    default :
      return MyWrite(buffer, 2);
  }

  return TRUE;
}


static PString GetTELNETOptionName(PINDEX code)
{
  static const char * const name[] = {
    "TransmitBinary",
    "EchoOption",
    "ReconnectOption",
    "SuppressGoAhead",
    "MessageSizeOption",
    "StatusOption",
    "TimingMark",
    "RCTEOption",
    "OutputLineWidth",
    "OutputPageSize",
    "CRDisposition",
    "HorizontalTabsStops",
    "HorizTabDisposition",
    "FormFeedDisposition",
    "VerticalTabStops",
    "VertTabDisposition",
    "LineFeedDisposition",
    "ExtendedASCII",
    "ForceLogout",
    "ByteMacroOption",
    "DataEntryTerminal",
    "SupDupProtocol",
    "SupDupOutput",
    "SendLocation",
    "TerminalType",
    "EndOfRecordOption",
    "TACACSUID",
    "OutputMark",
    "TerminalLocation",
    "Use3270RegimeOption",
    "UseX3PADOption",
    "WindowSize",
    "TerminalSpeed",
    "FlowControl",
    "LineMode",
    "XDisplayLocation",
    "EnvironmentOption",
    "AuthenticateOption",
    "EncriptionOption"
  };

  if (code < PARRAYSIZE(name))
    return name[code];
  if (code == PTelnetSocket::ExtendedOptionsList)
    return "ExtendedOptionsList";
  return PString(PString::Printf, "Option #%u", code);
}


//BOOL PTelnetSocket::StartSend(const char * which, BYTE code)
bool PTelnetSocket::StartSend(const char * which, BYTE code)
{
  if (IsOpen())
    return TRUE;

  //NOT OPEN YET
  return FALSE;
}


//BOOL PTelnetSocket::SendDo(BYTE code)
bool PTelnetSocket::SendDo(BYTE code)
{
     //cout << "***** Start of SendDo *****"<< endl;

#if 1
  if (!StartSend("SendDo", code))
    return FALSE;

  OptionInfo & opt = option[code];

  switch (opt.theirState) {
    case OptionInfo::IsNo :
      PDebugError << "initiated.";
      SendCommand(DO, code);
      opt.theirState = OptionInfo::WantYes;
      break;

    case OptionInfo::IsYes :
      PDebugError << "already enabled." << endl;
      return FALSE;

    case OptionInfo::WantNo :
      PDebugError << "queued.";
      opt.theirState = OptionInfo::WantNoQueued;
      break;

    case OptionInfo::WantNoQueued :
      PDebugError << "already queued." << endl;
      opt.theirState = OptionInfo::IsNo;
      return FALSE;

    case OptionInfo::WantYes :
      PDebugError << "already negotiating." << endl;
      opt.theirState = OptionInfo::IsNo;
      return FALSE;

    case OptionInfo::WantYesQueued :
      PDebugError << "dequeued.";
      opt.theirState = OptionInfo::WantYes;
      break;
  }

  PDebugError << endl;
#endif 
  return TRUE;
}


//BOOL PTelnetSocket::SendDont(BYTE code)
bool PTelnetSocket::SendDont(BYTE code)
{
  if (!StartSend("SendDont", code))
    return FALSE;

  OptionInfo & opt = option[code];

  switch (opt.theirState) {
    case OptionInfo::IsNo :
      PDebugError << "already disabled." << endl;
      return FALSE;

    case OptionInfo::IsYes :
      PDebugError << "initiated.";
      SendCommand(DONT, code);
      opt.theirState = OptionInfo::WantNo;
      break;

    case OptionInfo::WantNo :
      PDebugError << "already negotiating." << endl;
      opt.theirState = OptionInfo::IsNo;
      return FALSE;

    case OptionInfo::WantNoQueued :
      PDebugError << "dequeued.";
      opt.theirState = OptionInfo::WantNo;
      break;

    case OptionInfo::WantYes :
      PDebugError << "queued.";
      opt.theirState = OptionInfo::WantYesQueued;
      break;

    case OptionInfo::WantYesQueued :
      PDebugError << "already queued." << endl;
      opt.theirState = OptionInfo::IsYes;
      return FALSE;
  }

  PDebugError << endl;
  return TRUE;
}


//BOOL PTelnetSocket::SendWill(BYTE code)
bool PTelnetSocket::SendWill(BYTE code)
{
     //cout << "***** Start of SendWill *****"<< endl;
  if (!StartSend("SendWill", code))
    return FALSE;

  if (!IsOpen())
    return FALSE;

  OptionInfo & opt = option[code];

  switch (opt.ourState) {
    case OptionInfo::IsNo :
      PDebugError << "initiated.";
      SendCommand(WILL, code);
      opt.ourState = OptionInfo::WantYes;
      break;

    case OptionInfo::IsYes :
      PDebugError << "already enabled." << endl;
      return FALSE;

    case OptionInfo::WantNo :
      PDebugError << "queued.";
      opt.ourState = OptionInfo::WantNoQueued;
      break;

    case OptionInfo::WantNoQueued :
      PDebugError << "already queued." << endl;
      opt.ourState = OptionInfo::IsNo;
      return FALSE;

    case OptionInfo::WantYes :
      PDebugError << "already negotiating." << endl;
      opt.ourState = OptionInfo::IsNo;
      return FALSE;

    case OptionInfo::WantYesQueued :
      PDebugError << "dequeued.";
      opt.ourState = OptionInfo::WantYes;
      break;
  }

  PDebugError << endl;
  return TRUE;
}


//BOOL PTelnetSocket::SendWont(BYTE code)
bool PTelnetSocket::SendWont(BYTE code)
{
  if (!StartSend("SendWont", code))
    return FALSE;

  OptionInfo & opt = option[code];

  switch (opt.ourState) {
    case OptionInfo::IsNo :
      PDebugError << "already disabled." << endl;
      return FALSE;

    case OptionInfo::IsYes :
      PDebugError << "initiated.";
      SendCommand(WONT, code);
      opt.ourState = OptionInfo::WantNo;
      break;

    case OptionInfo::WantNo :
      PDebugError << "already negotiating." << endl;
      opt.ourState = OptionInfo::IsNo;
      return FALSE;

    case OptionInfo::WantNoQueued :
      PDebugError << "dequeued.";
      opt.ourState = OptionInfo::WantNo;
      break;

    case OptionInfo::WantYes :
      PDebugError << "queued.";
      opt.ourState = OptionInfo::WantYesQueued;
      break;

    case OptionInfo::WantYesQueued :
      PDebugError << "already queued." << endl;
      opt.ourState = OptionInfo::IsYes;
      return FALSE;
  }

  PDebugError << endl;
  return TRUE;
}


//BOOL PTelnetSocket::SendSubOption(BYTE code,
bool PTelnetSocket::SendSubOption(BYTE code,
                                    const BYTE * info, PINDEX len, int subCode)
{
  if (!StartSend("SendSubOption", code))
    return FALSE;

  PDebugError << "with " << len << " bytes." << endl;

#ifdef __linux__
  PBYTEArray buffer(len + 6);
  buffer[0] = IAC;
  buffer[1] = SB;
  buffer[2] = code;
  PINDEX i = 3;
  if (subCode >= 0)
    buffer[i++] = (BYTE)subCode;
  while (len-- > 0) {
    if (*info == IAC)
      buffer[i++] = IAC;
    buffer[i++] = *info++;
  }
  buffer[i++] = IAC;
  buffer[i++] = SE;

  return MyWrite((const BYTE *)buffer, i);
#endif
}


void PTelnetSocket::SetTerminalType(const PString & newType)
{
  terminalType = newType;
}


void PTelnetSocket::SetWindowSize(WORD width, WORD height)
{
  windowWidth = width;
  windowHeight = height;
#ifndef __linux__
  if (IsOurOption(WindowSize)) {

    BYTE buffer[4];
    buffer[0] = (BYTE)(width >> 8);
    buffer[1] = (BYTE)width;
    buffer[2] = (BYTE)(height >> 8);
    buffer[3] = (BYTE)height;
    SendSubOption(WindowSize, buffer, 4);
  }
  else {

#else
    SetOurOption(WindowSize);  //__linux__
    SendWill(WindowSize);
#endif
//  }  //__linux__
}


void PTelnetSocket::GetWindowSize(WORD & width, WORD & height) const
{
  width = windowWidth;
  height = windowHeight;
}


//BOOL PTelnetSocket::Read(void * data, PINDEX bytesToRead)
bool PTelnetSocket::Read(void * data, PINDEX bytesToRead)
{
  PBYTEArray buffer(bytesToRead);
  PINDEX charsLeft = bytesToRead;
  BYTE * dst = (BYTE *)data;

  while (charsLeft > 0) {
    BYTE * src = buffer.GetPointer(charsLeft);
    if (!MyRead(src, charsLeft)) {
      lastReadCount = bytesToRead - charsLeft;
      return lastReadCount > 0;
    }

    while (lastReadCount > 0) {
      BYTE currentByte = *src++;
      lastReadCount--;
      switch (state) {
        case StateCarriageReturn :
          state = StateNormal;
          if (currentByte == '\0')
            break; // Ignore \0 after CR
          // Else, fall through for normal processing

        case StateNormal :
          if (currentByte == IAC)
            state = StateIAC;
          else {
            if (currentByte == '\r' && !IsTheirOption(TransmitBinary))
              state = StateCarriageReturn;
            *dst++ = currentByte;
            charsLeft--;
          }
          break;

        case StateIAC :
          switch (currentByte) {
            case IAC :
              state = StateNormal;
              *dst++ = IAC;
              charsLeft--;
              break;

            case DO :
              state = StateDo;
              break;

            case DONT :
              state = StateDont;
              break;

            case WILL :
              state = StateWill;
              break;

            case WONT :
              state = StateWont;
              break;

            case DataMark :    // data stream portion of a Synch
              /* We may have missed an urgent notification, so make sure we
                 flush whatever is in the buffer currently.
               */
              PTelnetError << "received DataMark" << endl;
              if (synchronising > 0)
                synchronising--;
              break;

            case SB :          // subnegotiation start
              state = StateSubNegotiations;
              subOption.SetSize(0);
              break;

            default:
              if (OnCommand(currentByte))
                state = StateNormal;
              break;
          }
          break;

        case StateDo :
          OnDo(currentByte);
          state = StateNormal;
          break;

        case StateDont :
          OnDont(currentByte);
          state = StateNormal;
          break;

        case StateWill :
          OnWill(currentByte);
          state = StateNormal;
          break;

        case StateWont :
          OnWont(currentByte);
          state = StateNormal;
          break;

        case StateSubNegotiations :
          if (currentByte == IAC)
            state = StateEndNegotiations;
          else
            subOption[subOption.GetSize()] = currentByte;
          break;

        case StateEndNegotiations :
          if (currentByte == SE)
            state = StateNormal;
          else if (currentByte != IAC) {
            /* This is an error.  We only expect to get "IAC IAC" or "IAC SE".
               Several things may have happend.  An IAC was not doubled, the
               IAC SE was left off, or another option got inserted into the
               suboption are all possibilities. If we assume that the IAC was
               not doubled, and really the IAC SE was left off, we could get
               into an infinate loop here.  So, instead, we terminate the
               suboption, and process the partial suboption if we can.
             */
            state = StateIAC;
            src--;  // Go back to character for IAC ccommand
          }
          else {
            subOption[subOption.GetSize()] = currentByte;
            state = StateSubNegotiations;
            break;  // Was IAC IAC, subnegotiation not over yet.
          }
          if (subOption.GetSize() > 1 && IsOurOption(subOption[0]))
            OnSubOption(subOption[0],
                            ((const BYTE*)subOption)+1, subOption.GetSize()-1);
          break;

        default :
          PTelnetError << "illegal state: " << (int)state << endl;
          state = StateNormal;
      }
      if (synchronising > 0) {
        charsLeft = bytesToRead;    // Flush data being received.
        dst = (BYTE *)data;
      }
    }
  }
  lastReadCount = bytesToRead;
  return TRUE;
}


void PTelnetSocket::OnDo(BYTE code)
{
//  PTelnetError << "OnDo " << GetTELNETOptionName(code) << ' ';
  PTelnetError << "OnDo "  << ' ';

  OptionInfo & opt = option[code];

  switch (opt.ourState) {
    case OptionInfo::IsNo :
      if (opt.weCan) {
        PDebugError << "WILL.";
        SendCommand(WILL, code);
        opt.ourState = OptionInfo::IsYes;
      }
      else {
        PDebugError << "WONT.";
        SendCommand(WONT, code);
      }
      break;

    case OptionInfo::IsYes :
      PDebugError << "ignored.";
      break;

    case OptionInfo::WantNo :
      PDebugError << "is answer to WONT.";
      opt.ourState = OptionInfo::IsNo;
      break;

    case OptionInfo::WantNoQueued :
      PDebugError << "impossible answer.";
      opt.ourState = OptionInfo::IsYes;
      break;

    case OptionInfo::WantYes :
      PDebugError << "accepted.";
      opt.ourState = OptionInfo::IsYes;
      break;

    case OptionInfo::WantYesQueued :
      PDebugError << "refused.";
      opt.ourState = OptionInfo::WantNo;
      SendCommand(WONT, code);
      break;
  }

  PDebugError << endl;

#ifdef __linux__
  if (IsOurOption(code)) {
    switch (code) {
      case TerminalSpeed : {
          static BYTE defSpeed[] = "38400,38400";
          SendSubOption(TerminalSpeed,defSpeed,sizeof(defSpeed)-1,SubOptionIs);
        }
        break;

      case TerminalType :
        SendSubOption(TerminalType,
                          terminalType, terminalType.GetLength(), SubOptionIs);
        break;

      case WindowSize :
        SetWindowSize(windowWidth, windowHeight);
        break;
    }
  }
#endif
}


void PTelnetSocket::OnDont(BYTE code)
{
//  PTelnetError << "OnDont " << GetTELNETOptionName(code) << ' ';
  PTelnetError << "OnDont " << ' ';

  OptionInfo & opt = option[code];

  switch (opt.ourState) {
    case OptionInfo::IsNo :
      PDebugError << "ignored.";
      break;

    case OptionInfo::IsYes :
      PDebugError << "WONT.";
      opt.ourState = OptionInfo::IsNo;
      SendCommand(WONT, code);
      break;

    case OptionInfo::WantNo :
      PDebugError << "disabled.";
      opt.ourState = OptionInfo::IsNo;
      break;

    case OptionInfo::WantNoQueued :
      PDebugError << "accepting.";
      opt.ourState = OptionInfo::WantYes;
      SendCommand(DO, code);
      break;

    case OptionInfo::WantYes :
      PDebugError << "queued disable.";
      opt.ourState = OptionInfo::IsNo;
      break;

    case OptionInfo::WantYesQueued :
      PDebugError << "refused.";
      opt.ourState = OptionInfo::IsNo;
      break;
  }

  PDebugError << endl;
}


void PTelnetSocket::OnWill(BYTE code)
{
  PTelnetError << "OnWill " << GetTELNETOptionName(code) << ' ';

  OptionInfo & opt = option[code];

  switch (opt.theirState) {
    case OptionInfo::IsNo :
      if (opt.theyShould) {
        PDebugError << "DO.";
        SendCommand(DO, code);
        opt.theirState = OptionInfo::IsYes;
      }
      else {
        PDebugError << "DONT.";
        SendCommand(DONT, code);
      }
      break;

    case OptionInfo::IsYes :
      PDebugError << "ignored.";
      break;

    case OptionInfo::WantNo :
      PDebugError << "is answer to DONT.";
      opt.theirState = OptionInfo::IsNo;
      break;

    case OptionInfo::WantNoQueued :
      PDebugError << "impossible answer.";
      opt.theirState = OptionInfo::IsYes;
      break;

    case OptionInfo::WantYes :
      PDebugError << "accepted.";
      opt.theirState = OptionInfo::IsYes;
      break;

    case OptionInfo::WantYesQueued :
      PDebugError << "refused.";
      opt.theirState = OptionInfo::WantNo;
      SendCommand(DONT, code);
      break;
  }

  PDebugError << endl;
}


void PTelnetSocket::OnWont(BYTE code)
{
  //PTelnetError << "OnWont " << GetTELNETOptionName(code) << ' ';
  PTelnetError << "OnWont " << ' ';

  OptionInfo & opt = option[code];

  switch (opt.theirState) {
    case OptionInfo::IsNo :
      PDebugError << "ignored.";
      break;

    case OptionInfo::IsYes :
      PDebugError << "DONT.";
      opt.theirState = OptionInfo::IsNo;
      SendCommand(DONT, code);
      break;

    case OptionInfo::WantNo :
      PDebugError << "disabled.";
      opt.theirState = OptionInfo::IsNo;
      break;

    case OptionInfo::WantNoQueued :
      PDebugError << "accepting.";
      opt.theirState = OptionInfo::WantYes;
      SendCommand(DO, code);
      break;

    case OptionInfo::WantYes :
      PDebugError << "refused.";
      opt.theirState = OptionInfo::IsNo;
      break;

    case OptionInfo::WantYesQueued :
      PDebugError << "queued refusal.";
      opt.theirState = OptionInfo::IsNo;
      break;
  }

  PDebugError << endl;
}


void PTelnetSocket::OnSubOption(BYTE code, const BYTE * info, PINDEX len)
{
  PTelnetError << "OnSubOption " << GetTELNETOptionName(code)
               << " of " << len << " bytes." << endl;
  switch (code) {
    case TerminalType :
      if (*info == SubOptionSend)
        SendSubOption(TerminalType,
                          terminalType, terminalType.GetLength(), SubOptionIs);
      break;

    case TerminalSpeed :
      if (*info == SubOptionSend) {
        static BYTE defSpeed[] = "38400,38400";
        SendSubOption(TerminalSpeed,defSpeed,sizeof(defSpeed)-1,SubOptionIs);
      }
      break;
  }
}


//BOOL PTelnetSocket::OnCommand(BYTE code)
bool PTelnetSocket::OnCommand(BYTE code)
{
  PTelnetError << "unknown command " << (int)code << endl;
  return TRUE;
}

void PTelnetSocket::OnOutOfBand(const void *, PINDEX length)
{
  PTelnetError << "out of band data received of length " << length << endl;
  synchronising++;
}


// End Of File ///////////////////////////////////////////////////////////////
