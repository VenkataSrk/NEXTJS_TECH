/* $Header: /xGate/term.cpp 43    1/31/03 2:26p Bennylp $
 *   
 * CTENGINE
 * (C) 2001 Switchlab, Ltd
 *
 * Configuration terminal.
 *
 * $Log: /xGate/term.cpp $
 * 
 * 43    1/31/03 2:26p Bennylp
 * Command 'show user' shows privilege
 * 
 * 42    10/23/02 12:38p Bennylp
 * Replace thread function with the one from thread_process.h
 * 
 * 41    10/22/02 5:14p Bennylp
 * Keep track of thread creation ("show threads")
 * 
 * 40    22/05/02 17:59 Junanto
 * Application logging to terminal can now be filtered out
 * (to concentrate on system messages only) by a 
 * new parameter "application" in "term logging".
 * By default, application logging is displayed to terminal.
 * To disable it: "no term logging application".
 * 
 * 39    22/05/02 16:39 Junanto
 * added choice validator for "term verbosity" command
 * 
 * 38    22/05/02 12:45 Junanto
 * Slight modified log-in/off info messages
 * 
 * 37    22/05/02 12:11 Junanto
 * updated welcome message
 * 
 * 36    5/20/02 10:28p Bennylp
 * Added logging warning when authentication failed
 * 
 * 35    3/07/02 9:21p Bennylp
 * Warning if 'save file' is called when kernel is not running
 * 
 * 34    7/25/01 10:30a Bennylp
 * History size is 20.
 * 
 * 33    7/23/01 8:14p Bennylp
 * Fixed bug in 'term send' argument type.
 * 
 * 32    7/23/01 4:53p Bennylp
 * Changed the way command history works.
 * 
 * 31    7/18/01 7:24p Bennylp
 * Only admins can execute 'term exec'
 * 
 * 30    7/18/01 6:09p Bennylp
 * Commands: term exec, disconnect, show, send.
 * 
 * 29    7/17/01 11:55a Bennylp
 * Fixed bug in #ifndef protection for _vsnprintf.
 * 
 * 28    4/07/01 17:25 Junanto
 * Only admin can change user/password of other users
 * 
 * 27    3/07/01 17:53 Junanto
 * Added multiple users support (multi login/password/privilege)
 * 
 * 26    6/30/01 11:33p Bennylp
 * Bare bone framework for telnet (still buggy).
 * 
 * 25    6/08/01 10:17a Bennylp
 * Fixed pure virtual bug.
 * 
 * 24    6/07/01 5:48p Bennylp
 * Added more term commands
 * 
 * 23    6/06/01 8:01p Bennylp
 * Increase history and add command to show history.
 * 
 * 22    5/29/01 4:42p Bennylp
 * Telnet server runs in separate thread for more availability.
 * 
 * 21    5/26/01 5:48p Bennylp
 * Added 'show user' and fixed telnet command history.
 * 
 * 20    5/21/01 12:12p Bennylp
 * Rollback SIP changes to stable version.
 * 
 * 19    10/05/01 12:40 Junanto
 * Replaced "\r\n" sequence to "\n". Telnet translates
 * an orphaned "\n" into "\r\n" during output
 * 
 * 18    10/05/01 11:33 Junanto
 * 
 * 17    10/05/01 11:22 Junanto
 * Replaced all "\r\n" with a constant
 * 
 * 16    5/09/01 12:15p Bennylp
 * Detach from log when session is destroyed.
 * 
 * 15    7/05/01 18:18 Junanto
 * fixed bug in load/save file
 * 
 * 14    4/05/01 20:46 Junanto
 * Changed welcome message in telnet session
 * 
 * 13    4/05/01 20:35 Junanto
 * The command save has the same feel as
 * the command load. The default file name is
 * System.conf
 * 
 * 12    5/01/01 1:02a Bennylp
 * Added output redirection to log.
 * 
 * 11    24/04/01 14:09 Junanto
 * Corrected initial loading of configuration file
 * 
 * 10    4/23/01 10:47p Bennylp
 * Fixed std namespace conflicts.
 * 
 * 9     4/17/01 10:03p Bennylp
 * Added logging receptor.
 * 
 * 8     4/13/01 9:10p Bennylp
 * Make the display looks nicer.
 * 
 * 7     4/13/01 7:18p Bennylp
 * Fix to use VT100 by default (the VT100 arrows also work now)
 * 
 * 6     4/12/01 8:08p Bennylp
 * Integration.
 *
 */
#ifdef __linux__
#include <iostream>
#include "sl.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
using namespace std;
#define UINT_MAX (unsigned(-1))
#endif

//#include "stdafx.h"
#include "telnet.h"
#include "term.h"
//#include "contain.h"
#include "log.h"
#include "kernel.h"
#include "cfg.h"
#include "klog.h"
#include "thread_process.h"
//#include <ace/OS.h>
#ifdef new
    #undef new
    #include <fstream>
    #define new TRACE_NEW
#else
    #include <fstream>
#endif

const unsigned short DEF_SRV_PORT=23;
#define TERM "term"
#define NEWLINE "\n"
#define __linux__

#define stricmp strcasecmp
#define strnicmp strncasecmp
#define strcmpi strcasecmp


// Static Config_Controller 
Config_Controller *Config_Controller::configControllerInstance;


///////////////////////////////////////////////////////////////////////////////
/**
  Base class for Telnet terminals
  */
struct TelnetTerminalCapability : public TerminalCapability {

    enum Cmd {
	ESC=27,		// Escape
	LEFT=68,	// cursor LEFT movement
	RIGHT=67,	// cursor RIGHT movement
	DOWN=66,	// cursor DOWN movement
	UP=65,		// cursor UP movement
	HOME=72,	// cursor HOME movement
	END=70,		// cursor END movement

	CTRL_C=3,	// Break
	CR=13,		// Cariage-return
	BS=8,		// Backspace
	TAB=9,		// Horizontal tab
	QM='?',		// Question mark
	BELL=7,		// audible bell
	DEL=127,	// Delete
    };

    virtual void Init();
};

void TelnetTerminalCapability::Init()
{
    m_StartCmd=91;
    memset(m_NormalTranslation, 0, sizeof(m_NormalTranslation));
    memset(m_EscTranslation, 0, sizeof(m_EscTranslation));

    m_NormalTranslation[CTRL_C]=Keystroke::CTRL_C;
    m_NormalTranslation[CR]=Keystroke::CR;
    m_NormalTranslation[BS]=Keystroke::BS;
    m_NormalTranslation[TAB]=Keystroke::TAB;
    m_NormalTranslation[QM]=Keystroke::QM;
    m_NormalTranslation[BELL]=Keystroke::BELL;
    m_NormalTranslation[DEL]=Keystroke::DEL;

    m_EscTranslation[LEFT]=Keystroke::LEFT;
    m_EscTranslation[RIGHT]=Keystroke::RIGHT;
    m_EscTranslation[DOWN]=Keystroke::DOWN;
    m_EscTranslation[UP]=Keystroke::UP;
    m_EscTranslation[HOME]=Keystroke::HOME;
    m_EscTranslation[END]=Keystroke::END;
}   

/**
  VT-100 terminal
  */
struct VT100TerminalCapability : public TelnetTerminalCapability {
    virtual void Init() {
	TelnetTerminalCapability::Init();
	m_StartCmd=91;
    }
};

/**
  VT-52 terminal
  */
struct VT52TerminalCapability : public TelnetTerminalCapability {
    virtual void Init() {
	TelnetTerminalCapability::Init();
	m_StartCmd=79;
    }
};


static VT100TerminalCapability g_StaticVT100TerminalCapability;
static VT52TerminalCapability g_StaticVT52TerminalCapability;

TerminalCapability& Term_IO_Stream::m_DefaultCapability=g_StaticVT100TerminalCapability;
//TerminalCapability& Term_IO_Stream::m_DefaultCapability=g_StaticVT52TerminalCapability;

///////////////////////////////////////////////////////////////////////////////
/*
  Start id for terminal command id.
  Hope that this is unique to all ctengine commands.
  */
const int TERM_CMD_ID=0xABCD;

/* Terminal intrinsic commadns */
enum {
    CMD_CONFIGURE   = TERM_CMD_ID+1,	// configure
    CMD_EXIT_CONFIGURE,	// exit from configure mode, or exit from current directory
    CMD_EXIT,		// logout
    CMD_LOGIN,
    CMD_STARTUP,
    CMD_SUSPEND,
    CMD_SHUTDOWN,
    CMD_RESTART,
    CMD_LOAD_CFG,
    CMD_SHOW_CFG,
    CMD_SAVE_CFG,
    CMD_TBL_ADD,
    CMD_TERM,
    CMD_ECHO,
    CMD_TOP,

    CMD_NUM_CMD
};


///////////////////////////////////////////////////////////////////////////////
Config_Controller * Config_Controller::Create()
{
    assert (configControllerInstance == NULL);
    if (configControllerInstance == NULL)
	configControllerInstance = new Config_Controller;
    return configControllerInstance;
}

Config_Controller::Config_Controller()
: ServiceProvider ("Terminal", false, true, ServiceProvider::Very_Low_Priority), 
  isLoaded(false)
{
    m_WelcomeMsg = NEWLINE;
    m_WelcomeMsg += Kernel::Instance()->GetVersion();
    size_t len = m_WelcomeMsg.size();
    m_WelcomeMsg += NEWLINE "Copyright \xa9 1999-";
    m_WelcomeMsg += (__DATE__+7);
    m_WelcomeMsg += " Switchlab Limited" NEWLINE;
    m_WelcomeMsg += "All rights reserved" NEWLINE;
    m_WelcomeMsg.append (len, '-');
    m_WelcomeMsg += NEWLINE NEWLINE;

    g_StaticVT100TerminalCapability.Init();
    g_StaticVT52TerminalCapability.Init();
}


Config_Controller::~Config_Controller()
{
    configControllerInstance = NULL;

    while (m_TermCmd.size()) {
	delete m_TermCmd.back();
	m_TermCmd.pop_back();
    }
}

void Config_Controller::InitTermCmd()
{
    /*
      Standard terminal commands:
      - normal mode: configure, exit, login
      - configure mode: exit, login, suspend
      - init mode: configure, startup, exit
     */
    // load configuration
    Term_Cmd *cmd_load = new Term_Cmd( NULL, CMD_LOAD_CFG, 
		       "load", 
		       "Load a configuration script", 
		       PrivilegeLevelUser,
		       TermModeAll, false);

    CVariantChoiceValidator v( "file", 
			       "Get configuration from a file");
    cmd_load->Add(new Term_Cmd_Arg("source", 
			       "file", 
			       v));
    cmd_load->Add(new Term_Cmd_Arg("param", 
			       "", 
			       CVariantValidator("", "Filename"), 
			       TermArgOptional));

    m_TermCmd.push_back(cmd_load);

    // configure
    Term_Cmd * cmd_conf = new Term_Cmd( NULL, CMD_CONFIGURE,
		       "configure",
		       "Enter configuration mode",
		       PrivilegeLevelAdmin,
		       TermModeNormal, false);
    m_TermCmd.push_back(cmd_conf);

    // save configuration entry
    Term_Cmd *cmd_save = new Term_Cmd( NULL, CMD_SAVE_CFG,
		       "save",
		       "Save the current configuration to disk",
		       PrivilegeLevelAdmin,
		       TermModeAll, false);
    CVariantChoiceValidator vf( "file", 
			       "Save configuration to a file");
    cmd_save->Add(new Term_Cmd_Arg("file", 
			       "", 
			       vf));
    cmd_save->Add(new Term_Cmd_Arg(
			   "file-name",
			   "",
			   CVariantValidator("", "File name"),
			   TermArgOptional));
    m_TermCmd.push_back(cmd_save);

    // exit --> back to previous mode
    Term_Cmd * cmd_exit_conf = new Term_Cmd( NULL, CMD_EXIT_CONFIGURE,
			    "exit", 
			    "Exit configuration mode", 
			    PrivilegeLevelUser,
			    TermModeConfig|TermModeInit|TermModeSingle, false);
    m_TermCmd.push_back(cmd_exit_conf);

    // exit --> logout
    Term_Cmd *cmd_exit = new Term_Cmd( NULL, CMD_EXIT,
		       "exit", 
		       "Logout", 
		       PrivilegeLevelUser,
		       TermModeNormal, false);
    m_TermCmd.push_back(cmd_exit);

    // login --> acquire specific privilege
    Term_Cmd *cmd_login = new Term_Cmd( NULL, CMD_LOGIN, 
			"login", 
			"Acquire specific privilege", 
			PrivilegeLevelUser,
			TermModeNormal, false);
    m_TermCmd.push_back(cmd_login);


    // show configuration entry
    Term_Cmd * cmd_show = new Term_Cmd(  NULL, CMD_SHOW_CFG, 
			"show", 
			"Show configuration entry", 
			PrivilegeLevelAdmin,
			TermModeConfig | TermModeEntry, false);
    cmd_show->Add ( new Term_Cmd_Arg ("cfg", 
				      CVariant("config"), 
				      CVariantSingleValueValidator("config", "Show configuration"), 
				      TermArgOptional));
    m_TermCmd.push_back(cmd_show);

    // table-add
    Term_Cmd * cmd_tbl_add = new Term_Cmd( NULL, CMD_TBL_ADD,
					  "add-row",
					  "Add new row to the table, and make it current directory",
					  PrivilegeLevelAdmin,
					  TermTableCmd);
    cmd_tbl_add->Add(new Term_Cmd_Arg( "name",
			      "new-row",
			      CVariantValidator("new-row", "String used as the name of the row"),
			      TermArgRequired));
    m_TermCmd.push_back(cmd_tbl_add);

    // top
    Term_Cmd *cmd_top = new Term_Cmd( NULL, CMD_TOP, 
			    "top", 
			    "Go to top configuration entry", 
			    PrivilegeLevelUser,
			    TermModeConfig | TermModeEntry, false);
    m_TermCmd.push_back(cmd_top);

    // term
    Term_Cmd *cmd_term = new Term_Cmd( NULL, CMD_TERM,
				       "term",
				       "Terminal related commands",
				       PrivilegeLevelUser,
				       TermModeAll);
    CVariantChoiceValidator termVdtor("echo", "Set character echo on/off");
    termVdtor.Add ("logging",	"Set the terminal to receive logging messages on/off");
    termVdtor.Add ("show",	"Show terminal settings");
    termVdtor.Add ("history",	"Show command history");
    termVdtor.Add ("verbosity",	"Show/set logging verbosity to this terminal");
    termVdtor.Add ("exec",	"Execute operating system command");
    termVdtor.Add ("send",	"Send a message to other terminal user");
    termVdtor.Add ("disconnect","Disconnect other terminal");

    cmd_term->Add(new Term_Cmd_Arg("@0", CVariant(""), termVdtor));

    CVariantChoiceValidator termVerbVdtor("info", "display information and higher severity levels");
    termVerbVdtor.Add("warning", "display warning and higher severity levels");
    termVerbVdtor.Add("error", "display error and fatal severity levels");
    termVerbVdtor.Add("fatal", "display fatal errors only");
    termVerbVdtor.Add("debug", "display debug and above level messages only");
    termVerbVdtor.Add("detail", "display all level of debugging");
    cmd_term->AddDynamic(1, 0, "verbosity", 
			new Term_Cmd_Arg("@1", CVariant(""), 
					 termVerbVdtor,
					 TermArgOptional));
    
    CVariantChoiceValidator termLogVdtor("application", "hide/display application output");
    cmd_term->AddDynamic(1, 0, "logging",
			new Term_Cmd_Arg("@1", CVariant(""), 
					 termLogVdtor,
					 TermArgOptional));

    cmd_term->AddDynamic(1, 0, "exec",
			new Term_Cmd_Arg("@1", CVariant(""), 
					 CVariantValidator(CVariant(""), "Command line"),
					 TermArgOptional));
    cmd_term->AddDynamic(1, 0, "disconnect",
			 new Term_Cmd_Arg("@1", 1, CVariantValidator(1, "Terminal ID")));

    cmd_term->AddDynamic(1, 0, "send",
			 new Term_Cmd_Arg("@1", "", CVariantValidator("", "Terminal ID, or all to send to all terminals")));
    cmd_term->AddDynamic(2, 0, "send",
			 new Term_Cmd_Arg("@2", CVariant(""), CVariantValidator("", "Message")));

    m_TermCmd.push_back(cmd_term);


    // echo
    /*
    Term_Cmd cmd_echo(	NULL, CMD_ECHO,
			"echo",
			"Print to terminal",
			PrivilegeLevelUser,
			TermModeAll);
    cmd_echo.Add(Term_Cmd_Arg( "string",
			   "",
			   CVariantValidator("", "String to echo")));
    m_TermCmd.push_back(cmd_echo);
    */
}




bool Config_Controller::Load()
{
    // This ServiceProvider is unique because Kernel will manually Load() this
    // before any other SPs. And because this instance is registered with
    // ServiceProviderManager, it will be Load()-ed again when all SPs are
    // Load()-ed. So protect this with a flag.
    if (isLoaded) return true;

    InitTermCmd();

    char buf[128];
    if (ACE_OS::hostname(buf, 128) == 0)
	m_Hostname = buf;
    else
	m_Hostname = "localhost";

    Term_Telnet_Controller *telnet_controller=new Term_Telnet_Controller;
    if (telnet_controller->Init()!=OK) {
        printf("telnet_controller->Init()!=OK triggered \n");
	delete telnet_controller;
	return false;
    }
    m_TermControllers.push_back(telnet_controller);

#ifndef __linux__
    // 2nd listening thread in case of emergency
    telnet_controller=new Term_Telnet_Controller;
    if (telnet_controller->Init(2323)!=OK) {
	delete telnet_controller;
	return false;
    }
#endif
    //m_TermControllers.push_back(telnet_controller);	//rkv commened tor twice calling this statement
    
    isLoaded = true;
    return true;
}


bool Config_Controller::Unload()
{
    while (m_TermControllers.size()) {
	m_TermControllers.front()->Shutdown();
	delete m_TermControllers.front();
	m_TermControllers.pop_front();
    }
    isLoaded = false;
    return true;
}


void Config_Controller::HandleEvents()
{
    List<Term_Controller*>::iterator it, end;
    end = m_TermControllers.end();

    for (it=m_TermControllers.begin(); it!=end; ++it) {
	(*it)->PollEvents();
    }
}


STATUS Config_Controller::Lock(Term_Session *term)
{
    m_TermMutex.Lock();
    return OK;
}


void Config_Controller::Unlock()
{
    m_TermMutex.Unlock();
}


void Config_Controller::NotifyEntryDeleted ( Term_Session *sender, Cfg_Tree_Entry * entry )
{
    List<Term_Controller*>::iterator it;

    for (it=m_TermControllers.begin(); it!=m_TermControllers.end(); ++it)
	(*it)->NotifyEntryDeleted (sender, entry);
}


/** Allocate a unique terminal Id.
 */
unsigned Config_Controller::AllocTermId()
{
    static unsigned id;
    return id++;
}


/** Disconnect a terminal.
 */
void Config_Controller::DisconnectTerm(unsigned id, const String & msg)
{
    List<Term_Controller*>::iterator it;

    for (it=m_TermControllers.begin(); it!=m_TermControllers.end(); ++it)
	(*it)->DisconnectTerm(id, msg);
}


/** Send a message to other terminal/user.
 */
void Config_Controller::SendMessage(unsigned id, const String & msg)
{
    List<Term_Controller*>::iterator it;

    for (it=m_TermControllers.begin(); it!=m_TermControllers.end(); ++it)
	(*it)->SendMessage(id, msg);
}


void Config_Controller::ShowLoggedOnUsers ( Term_Session *term )
{
    List<Term_Controller*>::iterator it;

    for (it=m_TermControllers.begin(); it!=m_TermControllers.end(); ++it)
	(*it)->ShowLoggedOnUsers(term);
}


///////////////////////////////////////////////////////////////////////////////
Term_Controller::Term_Controller(const char *type)
: termType(type)
{
}

STATUS Term_Controller::Init()
{
    return OK;
}

STATUS Term_Controller::Shutdown()
{
    m_TtyMutex.Lock();
    for (List<Term_Session*>::iterator it=m_Ttys.begin(); it!=m_Ttys.end(); it++)
	(*it)->Disconnect();
    int n_term=m_Ttys.size();
    m_TtyMutex.Unlock();

    while (n_term>0) {
	sl::Sleep(100);
	m_TtyMutex.Lock();
	n_term=m_Ttys.size();
	m_TtyMutex.Unlock();
    }

    while(m_TtysToBeCleanup.size()) {
	delete m_TtysToBeCleanup.front();
	m_TtysToBeCleanup.pop_front();
    }
    return OK;
}


STATUS Term_Controller::PollEvents()
{
    if (m_TtysToBeCleanup.empty())
	return OK;

    m_TtyMutex.Lock();
    while(m_TtysToBeCleanup.size()) {
	m_TtysToBeCleanup.front()->WaitThread();
	delete m_TtysToBeCleanup.front();
	m_TtysToBeCleanup.pop_front();
    }
    m_TtyMutex.Unlock();

    return OK;
}

void Term_Controller::RemoveTerm(Term_Session& term)
{
    m_TtyMutex.Lock();
    m_Ttys.remove(&term);
    m_TtysToBeCleanup.push_back(&term);
    m_TtyMutex.Unlock();
}

void Term_Controller::AddTerm(Term_Session& term)
{
    m_TtyMutex.Lock();
    m_Ttys.push_back(&term);
    m_TtyMutex.Unlock();
}


void Term_Controller::NotifyEntryDeleted ( Term_Session *sender, Cfg_Tree_Entry * entry )
{
    List<Term_Session*>::iterator it;
    assert(entry);

    // Protect so that term is not deleted/added while we're traversing them.
    m_TtyMutex.Lock();

    for (it=m_Ttys.begin(); it!=m_Ttys.end(); ++it) {
	Term_Session *term = *it;
	if (entry->IsRootOfPath (term->GetCurrentRootEntry()->GetFullPath())) {
	    //
	    // It's not a good idea to send a message here, since caller might
	    // call this function while holding system lock.
	    //
	    /*
	    term->SendMessage ("Current directory is deleted by %s." NEWLINE 
			       "Directory is changed to root dir." NEWLINE,
			       sender->GetSourceAddress.c_str());
	    */

	    //
	    // NOTE: this is not thread safe since at the same time the terminal
	    // might modify the root. Unfortunately each terminal doesn't have
	    // a mutex.
	    //
	    term->SetRoot (Kernel::Instance()->RootConfig());
	}
    }
    m_TtyMutex.Unlock();
}


void Term_Controller::ShowLoggedOnUsers ( Term_Session *term )
{
    String temp;
    temp.reserve (1024);
    char s[128];

    time_t now = time(NULL);

    // Lock ttys so that tty is not deleted while we're traversing
    // them.
    m_TtyMutex.Lock();
    List<Term_Session*>::iterator it;
    for (it=m_Ttys.begin(); it!=m_Ttys.end(); ++it) {
	Term_Session *term = *it;
	sprintf (s, " % 6d %s user %s[%c] from %s [%s]" NEWLINE,
		    term->GetId(),
		    GetTermType(),
		    term->GetUserName(), 
		    (term->GetPrivilegeLevel()==PrivilegeLevelAdmin ? 'A' : 'U'),
		    term->GetSourceAddress().c_str(),
		    ConvertInterval2String(now-term->GetStartTime()).c_str());
	temp += s;
    }
    m_TtyMutex.Unlock();

    term->Send (temp);
}


/** Disconnect a terminal.
 */
void Term_Controller::DisconnectTerm(unsigned id, const String & msg)
{
    // Lock ttys 
    m_TtyMutex.Lock();
    List<Term_Session*>::iterator it;
    for (it=m_Ttys.begin(); it!=m_Ttys.end(); ++it) {
	Term_Session *term = *it;
	if (term->GetId() == id) {
	    term->Send(msg);
	    term->Disconnect();
	    break;
	}
    }
    m_TtyMutex.Unlock();
}


/** Send a message to other terminal/user.
 */
void Term_Controller::SendMessage(unsigned id, const String & msg)
{
    // Lock ttys 
    m_TtyMutex.Lock();
    List<Term_Session*>::iterator it;
    for (it=m_Ttys.begin(); it!=m_Ttys.end(); ++it) {
	Term_Session *term = *it;
	if (id==UINT_MAX || term->GetId() == id) {
	    term->Send(msg);
	    break;
	}
    }
    m_TtyMutex.Unlock();
}


///////////////////////////////////////////////////////////////////////////////
Term_Telnet_Controller::Term_Telnet_Controller()
: Term_Controller("telnet"), m_QuitFlag(false), m_ServerSock(-1),
  m_hThread(INVALID_HANDLE_VALUE)
{
}


Term_Telnet_Controller::~Term_Telnet_Controller()
{
    if (m_ServerSock!=-1)
	Shutdown();
}


STATUS Term_Telnet_Controller::Init(short port)
{
    int option=1;
    m_ServerSock = socket( AF_INET, SOCK_STREAM, 0);
    //printf("Creating socket in Term_Telnet_Controller::Init() =:%d\n", m_ServerSock); //rkv

    if (m_ServerSock == -1) {
	CLog::SysLastError( TERM, "Can't create server socket");
	return NOT_OK;
    }

    if(setsockopt(m_ServerSock,SOL_SOCKET, SO_REUSEADDR,(const char*)&option,sizeof(option)) < 0) {
      //  CLog::SysLastError( TERM, "Can't reuse port 3800");
        CLog::SysLastError( TERM, "Can't reuse port 23");
        return NOT_OK;
    }


    sockaddr_in saddr;
    memset( &saddr, 0, sizeof(saddr));
	
    saddr.sin_family = AF_INET;
#ifdef __linux__
    saddr.sin_addr.s_addr=htonl(INADDR_ANY);
    //saddr.sin_addr.s_addr=htonl("192.168.2.95");
  //  port=3800;
    port=23;
#endif
    saddr.sin_port = htons(port);
    int rc;

    if ( rc = bind( m_ServerSock, (struct sockaddr *) &saddr, sizeof(saddr)) != 0 ) {
#ifndef __linux__
	CLog::SysLastError( TERM, "Can't bind to port %d %d", port,rc);
	closesocket(m_ServerSock);
#else
   	cout << "Term_Telnet_Controller::INIT ,Can't bind to port " << port << " " << rc << endl;
	close(m_ServerSock);
#endif
	return NOT_OK;
    }

    if ( listen( m_ServerSock, 5)) {
	CLog::SysLastError( TERM, "Socket listen() error");
#ifndef __linux__
	closesocket(m_ServerSock);
#else
	close(m_ServerSock);
#endif
	return NOT_OK;
    }

    CLog::Info( TERM, "Listening on interface %s port %d",
		      inet_ntoa(saddr.sin_addr), ntohs(saddr.sin_port));

#ifndef __linux__
    if (SPAWN_THREAD( &thread_proc, this, THR_NEW_LWP | THR_JOINABLE, NULL, &m_hThread)) {
	CLog::SysLastError (TERM, "Unable to create thread");
	closesocket (m_ServerSock);
	return NOT_OK;
    }
#else
    if( pthread_create(&m_hThread,NULL,&thread_proc,(void *) this) != 0) {  // thread_process argument passed here is NULL, CHANGE according to the actual scenario
	CLog::SysLastError (TERM, "Unable to create thread");
        close(m_ServerSock);
        return NOT_OK;
    }
    	//printf("Calling pthread_create function here Term_Telnet_Controller::Init \n");	 //rkv testing
   


#endif 

    return OK;

}


STATUS Term_Telnet_Controller::Init()
{
    return Init(DEF_SRV_PORT);
}


STATUS Term_Telnet_Controller::Shutdown()
{
    assert(m_ServerSock!=-1);

    m_QuitFlag=1;

#ifndef __linux__
    ACE_Thread::join(m_hThread);
#else
    pthread_join(m_hThread,NULL);
#endif

    shutdown(m_ServerSock,2);
#ifndef __linux__
    closesocket(m_ServerSock);
#else
    close(m_ServerSock);

#endif
    m_ServerSock=-1;

    return Term_Controller::Shutdown();
}


void *Term_Telnet_Controller::thread_proc(void *this_instance)
{
    Term_Telnet_Controller *controller=(Term_Telnet_Controller*)this_instance;
    return (void*)controller->MainThread();
}


int  Term_Telnet_Controller::MainThread()
{
    assert(m_ServerSock!=-1);

    CLog::Detail (TERM, "Telnet thread started");

    fd_set read_fd;
    FD_ZERO(&read_fd);
    FD_SET(m_ServerSock, &read_fd);

    while (!m_QuitFlag) {
	timeval tv={1,0};
	fd_set fd = read_fd;

	int ret;
	if ((ret=::select(FD_SETSIZE, &fd, NULL, NULL, &tv))==1) {
	    OnNewConnection();
	} else if (ret==-1) {
	    ; // Error. What to do??
	}
    }

    CLog::Info (TERM, "Telnet thread exiting");
    return 0;
}


void Term_Telnet_Controller::OnNewConnection()
{
    int new_socket;
    sockaddr_in addr;
    int addrlen=sizeof(addr);

    new_socket=::accept(m_ServerSock, (sockaddr*)&addr, &addrlen);
    if (new_socket==-1) {
	CLog::Error(TERM, "Unable to accept new TELNET session");
	return;
    }

    Term_Telnet_Session *term=new Term_Telnet_Session(*this, new_socket);
    AddTerm(*term);
    cout << "******** term_Telnet_controller::OnNewConnection ****** " << endl;

    if (!term->StartThread()) {
	term->SendError("Failed to spawn a new thread, unable to open new TELNET session" NEWLINE);
	CLog::Error(TERM, "Failed to spawn a new thread, unable to open new TELNET session with %s", inet_ntoa(addr.sin_addr));
	#ifndef __linux__
	Sleep(40);
	#else
//	usleep(40000);
	sl::Sleep(40);
	#endif
	term->Disconnect();
	RemoveTerm(*term);
    }
}


///////////////////////////////////////////////////////////////////////////////
const Term_Command_Buffer& Term_Command_Buffer::operator=(const Term_Command_Buffer &x)
{
    if (m_BufferSize<x.m_BufferSize) {
	delete [] m_Buffer;
	m_Buffer=new char[x.m_BufferSize];
	m_BufferSize=x.m_BufferSize;
    }
    strncpy(m_Buffer, x.m_Buffer, x.m_BufferSize);
    m_Buffer[x.m_Size]='\0';
    m_Pos=x.m_Pos;
    m_Size=x.m_Size;
    
    return *this;
}

bool Term_Command_Buffer::Insert(char c)
{
    if (m_Size+1>=m_BufferSize-1)
	return false;

    int r=RightSize();
    
    if (r>0) {
	memmove(&m_Buffer[m_Pos+1], &m_Buffer[m_Pos], r+1);
	m_Buffer[m_Pos]=c;
    } else {
	m_Buffer[m_Pos]=c;
    }
    ++m_Size;
    ++m_Pos;
    
    return true;
}

bool Term_Command_Buffer::Backspace()
{
    if (m_Size==0 || m_Pos==0)
	return false;

    int r=RightSize();

    if (r>0) {
	memmove(&m_Buffer[m_Pos-1], &m_Buffer[m_Pos], r);
    } 
    --m_Pos;
    --m_Size;
    m_Buffer[m_Size]='\0';
    return true;
}

bool Term_Command_Buffer::Delete()
{
    if (m_Size==0 || m_Pos==m_Size)
	return false;
    
    int r=RightSize()-1;
    memmove(&m_Buffer[m_Pos], &m_Buffer[m_Pos+1], r);
    --m_Size;
    if (m_Size) m_Buffer[m_Size]='\0';
    
    return true;
}

///////////////////////////////////////////////////////////////////////////////
Term_Session::Term_Session(Term_Controller* con)
: m_Controller(con), 
  m_HistorySize(20), 
  m_EchoEnabled(true), 
  m_ExtendedKeystrokeEnabled(true),
  m_GlobEnabled(true),
  m_CharacterModeEnabled(true),
  m_CurrentCommand(1024),
  m_UserPrivilege(PrivilegeLevelNoAccess),
  m_CurrentRoot(Kernel::Instance()->RootConfig()),
  m_CurCmdCursor(m_History.end()), 
  m_NewCmdCursor(m_History.end()),
  m_TermMode(TermModeNormal),
  m_ErrHeader("%Error: "),
  m_IsAttachedToLog(false),
  m_CloseFlag(false),
  m_StartTime(time(NULL)),
  reader_hnd_(0),
  writer_hnd_(0)
{
    __m_Verbosity = LOG_INFO;
    m_CurrentKeystroke.SetCmd(Keystroke::CR);
    m_Id = Config_Controller::Instance()->AllocTermId();
}

Term_Session::~Term_Session()
{
    if (m_IsAttachedToLog) {
	KLog::Instance()->Detach(this);
	m_IsAttachedToLog = false;
    }
}

bool Term_Session::StartThread()
{
#ifndef __linux__
    return SPAWN_THREAD2(16384, thread_proc, this, THR_NEW_LWP|THR_JOINABLE, 0, &reader_hnd_)==0;
#else
//	 thread_proc((void *) this)
	//printf("Calling pthread_create function here Term_Session::StartThread()\n");	//rkv testing

	pthread_t term_session_t;
     int ret = pthread_create(&term_session_t,NULL,thread_proc,(void *)this);
	if(ret == 0)
		return 1;
	else
		return 0;

#endif 
}

void Term_Session::WaitThread()
{
    if (reader_hnd_ != NULL) {
	ACE_Thread::join(reader_hnd_);
	reader_hnd_ = NULL;
    }
}

void *Term_Session::thread_proc(void *p)
{
    Term_Session *session = (Term_Session*)p;
    return session->ThreadProc();
}

void *Term_Session::ThreadProc()
{
    //try {
	Run();
    //} catch (...) {
	//SendError("Exception detected in this TELNET session, disconnected" NEWLINE);
	//Sleep(40);
	//Disconnect();
    //}
    if (m_Controller) 
	m_Controller->RemoveTerm(*this);
    if (m_IsAttachedToLog) {
	KLog::Instance()->Detach(this);
	m_IsAttachedToLog = false;
    }
    return 0;
}

STATUS Term_Session::Shutdown()
{
    GetStream().Close();
    WaitThread();
    return OK;
}


STATUS Term_Session::Disconnect()
{
    cout << " ******************* Inside " << __func__ << " ***** " << endl;
    GetStream().Close();
    return OK;
}

void Term_Session::SetMode(Term_Mode m)
{
    m_TermMode=m;
}

void Term_Session::UpdatePrompt()
{
    Config_Controller * drv=Config_Controller::Instance();

    switch(GetTermMode()) {
    case TermModeLogin:
	m_Prompt="";
	break;
    case TermModeNormal:
	m_Prompt=drv->GetHostname();
	break;
    case TermModeConfig:
	m_Prompt=drv->GetHostname()+"(conf)-"+m_CurrentRoot->GetFullPath();
	break;
    case TermModeInit:
	m_Prompt=drv->GetHostname()+"(INIT)-"+m_CurrentRoot->GetFullPath();;
	break;
    }

    switch(GetPrivilegeLevel()) {
    case PrivilegeLevelUser:
	m_Prompt+="$ ";
	break;
    case PrivilegeLevelAdmin:
	m_Prompt+="# ";
	break;
    }
}

void Term_Session::SetEcho(bool on_off)
{
    m_EchoEnabled=on_off;
}

void Term_Session::SetExtendedKeystroke(bool on_off)
{
    m_ExtendedKeystrokeEnabled=on_off;
}

void Term_Session::SetHistorySize(size_t n)
{
    m_HistorySize=n;
}

Keystroke::Cmd Term_Session::GetCmdInCharMode(String& command)
{
    bool exit_loop=false;
    Keystroke ks;
    Term_IO_Stream& strm=GetStream();

    command.erase();
    ks.SetCmd(Keystroke::CTRL_C);
    
    bool stream_status=false;
    while(!exit_loop && (stream_status=strm.GetKeystroke(ks))==true) {
	m_CurrentKeystroke=ks;

	switch(ks.m_Type) {
	case Keystroke::TypeCmd:
	    ProcessCommandKey(ks.GetCmd());
	    if (ks.GetCmd()==Keystroke::CR || ks.GetCmd()==Keystroke::CTRL_C)
		exit_loop=true;
	    break;
	case Keystroke::TypeLetter:
	    if (m_CurrentCommand.IsFull(1)) {
		OnErrorKey();
		break;
	    }
	    m_CurrentCommand.Insert(ks.GetLetter());
	    OnAlphabeticKey(ks.GetLetter());
	    break;
	case Keystroke::TypeError:
	    OnErrorKey();
	    break;
	case Keystroke::TypeIgnored:
	    OnIgnoredKey();
	    break;
	}
    }

    if (!stream_status)
	ks.SetCmd(Keystroke::CTRL_C);

    return ks.GetCmd();
}

Keystroke::Cmd Term_Session::GetCmdInLineMode(String& command)
{
    bool read_result;
    Keystroke ks;
    while((read_result=GetStream().GetKeystroke(ks))==true) {
	if (ks.m_Type==Keystroke::TypeCmd && 
	    (ks.GetCmd()==Keystroke::CR || ks.GetCmd()==Keystroke::CTRL_C))
	    break;
	else if (ks.m_Type==Keystroke::TypeLetter)
	    m_CurrentCommand.Insert(ks.GetLetter());
    }

    m_CurrentKeystroke=ks;
    if (read_result && ks.GetCmd()==Keystroke::CR) {
	command=m_CurrentCommand.Buffer();
	return Keystroke::CR;
    } else
	return Keystroke::CTRL_C;
}

Keystroke::Cmd Term_Session::GetCmd(String& command)
{
    command.erase();
    Keystroke::Cmd ks;

    if (m_CharacterModeEnabled)
	ks=GetCmdInCharMode(command);
    else
	ks=GetCmdInLineMode(command);

    if (ks==Keystroke::CR) {
	command=m_CurrentCommand.Buffer();

	// Find out whether the first non-whitespace character is the comment.
	const char *p = command.c_str();
	while (*p && isspace(*p))
	    ++p;
	if (*p == '#')
	    command.erase((size_t)0);

	if (m_CurrentCommand.Size() > 0 && m_HistorySize>0 ) {

	    /* Find out whether there's an exact copy of this command in the history.
	       If so, move that command as the last command.
	     */
	    TermHist::iterator it;
	    for (it=m_History.begin(); it != m_History.end(); ++it) {
		if (command == it->Buffer()) {
		    m_History.erase(it);
		    break;
		}
	    }

	    m_History.push_back(m_CurrentCommand);

	    if (m_History.size() > m_HistorySize)
		m_History.erase(m_History.begin());
	    m_CurCmdCursor = m_History.end();
	}
    }

    m_CurrentCommand.Clear();
    return ks;
}

void Term_Session::OnAlphabeticKey(char)
{
}

void Term_Session::OnCommandKey(Keystroke::Cmd, Term_Command_Buffer&, const Term_Command_Buffer&, int)
{
}

void Term_Session::OnErrorKey()
{
}

void Term_Session::OnIgnoredKey()
{
}

Term_Session::TermHist::iterator Term_Session::MoveHistory(int m)
{
    if (m_History.empty() || m==0)
	return m_History.end();

    TermHist::iterator it;
    int i;

    if (m == 0)
	return m_CurCmdCursor;
    else if (m > 0) {
	it=m_CurCmdCursor;
	if (it == m_History.end())
	    it = m_History.begin();
	for (i=0; i<m; ++i) {
	    ++it;
	    if (it == m_History.end())
		it = m_History.begin();
	}
    } else if (m < 0) {
	for (i=0, it=m_CurCmdCursor; i>m; --i) {
	    if (it == m_History.begin()) {
		it = m_History.end();
	    }
	    --it;
	}
    }

    return it;
}

void Term_Session::ProcessCommandKey(Keystroke::Cmd cmd)
{
    // when extended keystroke is disabled, only accept several cmds
    if (!m_ExtendedKeystrokeEnabled && 
	!(cmd==Keystroke::BS || cmd==Keystroke::CR ||
	 cmd==Keystroke::CTRL_C || cmd==Keystroke::DEL ||
	 cmd==Keystroke::TAB || cmd==Keystroke::QM))
    {
	OnErrorKey();
	return;
    }

    if (!m_GlobEnabled && (cmd==Keystroke::TAB || cmd==Keystroke::QM)) {
	OnErrorKey();
	return;
    }

    switch(cmd) {
    case Keystroke::DOWN:
    case Keystroke::UP:
	m_CurCmdCursor=MoveHistory(cmd==Keystroke::UP? -1 : 1);
	if (m_CurCmdCursor==m_History.end()) {
	    m_CurCmdCursor=m_History.begin();
	} else {
	    OnCommandKey(cmd, *m_CurCmdCursor, m_CurrentCommand, 0);
	    m_CurrentCommand=*m_CurCmdCursor;
	}
	break;
    case Keystroke::LEFT:
	if (m_CurrentCommand.Move(-1)) {
	    OnCommandKey(cmd, m_CurrentCommand, m_CurrentCommand, 1);
	} else {
	    OnErrorKey();
	}
	break;
    case Keystroke::RIGHT:
	if (m_CurrentCommand.Pos()<m_CurrentCommand.Size()) {
	    m_CurrentCommand.Move(1);
	    OnCommandKey(cmd, m_CurrentCommand, m_CurrentCommand, 1);
	} else {
	    OnErrorKey();
	}
	break;
    case Keystroke::HOME:
	{
	    int movement=m_CurrentCommand.Home();
	    
	    if (movement==0) {
		OnErrorKey();
	    } else {
		OnCommandKey(cmd, m_CurrentCommand, m_CurrentCommand, movement);
	    }
	}
	break;
    case Keystroke::END:
	{
	    int movement=m_CurrentCommand.RightSize();
	    
	    if (movement==0) {
		OnErrorKey();
	    } else {
		m_CurrentCommand.End();
		OnCommandKey(cmd, m_CurrentCommand, m_CurrentCommand, movement);
	    }
	}
	break;
    case Keystroke::CR:
	// move cursor to the end
	m_CurrentCommand.End();
	OnCommandKey(cmd, m_CurrentCommand, m_CurrentCommand, 0);
	break;
    case Keystroke::TAB:
    case Keystroke::QM:
	OnCommandKey(cmd, m_CurrentCommand, m_CurrentCommand, 0);
	break;
    case Keystroke::CTRL_C:
	if (m_NewCmdCursor==m_CurCmdCursor)
	    m_CurrentCommand.Clear();
	else {
	    m_CurCmdCursor=m_NewCmdCursor;
	}
	OnCommandKey(cmd, m_CurrentCommand, m_CurrentCommand, 0);
	break;
    case Keystroke::BS:
	if (m_CurrentCommand.Size()==0 || m_CurrentCommand.Pos()==0) {
	    OnErrorKey();
	} else {
	    m_CurrentCommand.Backspace();
	    OnCommandKey(cmd, m_CurrentCommand, m_CurrentCommand, 1);
	}
	break;
    case Keystroke::DEL:
	if (m_CurrentCommand.Size()==0 || m_CurrentCommand.Pos()==m_CurrentCommand.Size()) {
	    OnErrorKey();
	} else {
	    m_CurrentCommand.Delete();
	    OnCommandKey(cmd, m_CurrentCommand, m_CurrentCommand, 0);
	}
	break;
    case Keystroke::BELL:
	break;
    }
}

void Term_Session::LogCommand(const char *cmd)
{
    FILE *f=NULL;
    time_t now;
    struct tm *tm;

    now = time(NULL);
    tm = localtime(&now);

    char fname[256];
    sprintf(fname, "/var/switchlab/etc/SESSION-%02d.LOG", tm->tm_mday);

    for (int i=0; i<3; i++) {
	f = fopen(fname, "at");
	if (f) break;
#ifndef __linux__
	Sleep(3);
#else
	sl::Sleep(3);
#endif
    }

    if (!f) return;

    fprintf(f, "%d-%02d-%02d %02d:%02d:%02d,%s,%s,%s\n",
	tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
	tm->tm_hour, tm->tm_min, tm->tm_sec,
	GetUserName(), GetSourceAddress().c_str(),
	cmd);

    fclose(f);
}

void Term_Session::ProcessCmd(const String& cmd)
{

    LogCommand(cmd.c_str());

    Term_Cmd_Scenario scenario(cmd, cmd.size(), this);
    
    if (scenario.GetStatus() == Term_Cmd_Scenario::CMD_ERROR) {
	Send(CreateErrMsg(scenario.GetErrorPos(), scenario.GetErrorMsg()));
	return;
    }
    
    if (scenario.GetTokenCount()>0) {
	switch (scenario.GetStatus()) {
	case Term_Cmd_Scenario::CMD_AWAIT_CMD_ARG:
	    Send(CreateErrMsg(cmd.size(), "incomplete command"));
	    break;
	case Term_Cmd_Scenario::CMD_AWAIT_COMMAND:
	case Term_Cmd_Scenario::CMD_AWAIT_CFG_ARG:
	case Term_Cmd_Scenario::CMD_COMPLETE:
	    RunCommand(scenario);
	    break;
	};
    }
}

void Term_Session::RunCommand(Term_Cmd_Scenario& scenario)
{
    assert(scenario.GetEntry());

    if (scenario.GetStatus()==Term_Cmd_Scenario::CMD_COMPLETE) {

	switch (scenario.GetCommandMode()) {
	case Term_Cmd_Scenario::CMD_CFG_SET:
	    //RunCfgSetCommand(scenario);
	    break;
	case Term_Cmd_Scenario::CMD_EXEC:
	    RunTermCommand(scenario);
	    break;
	}

    } else if (scenario.GetStatus()==Term_Cmd_Scenario::CMD_AWAIT_COMMAND) {
	RunCfgListCommand(scenario);
    }
}

void Term_Session::DoCmdConfigure(const Term_Cmd_Scenario&)
{
    SetMode(TermModeConfig);
    UpdatePrompt();
}

void Term_Session::DoCmdLoadConfig(const Term_Cmd_Scenario& scenario)
{
    const Term_Cmd& cmd=scenario.GetCommand();

    CVariant v_src_input;
    if (!cmd.GetArg((size_t)0)->GetValue(v_src_input)) {
	SendError("argument is not supplied" NEWLINE);
	return;
    }

    String src_input=v_src_input.ToString();
    if (!stricmp(src_input.c_str(), "file")) {
	CVariant v_file_name;

	const char *fname = NULL;
	cmd.GetArg((size_t)1)->GetValue(v_file_name);
	String s = Kernel::Instance()->GetBaseDirectory();
	String sfname = v_file_name.ToString();
	if (sfname == "") {
	    //SendError("you must specify filename as the second argument" NEWLINE);
	    //return;
	    // if nothing is specified, assume that users want to load from default file
	    s += KERNEL_CFG;
	    fname = s.c_str();
	} else {
	    fname = sfname.c_str();
	}

	Term_File_I_Stream ifstr(fname);
	if (!ifstr.IsOpen()) {
	    SendErrorf("unable to open file \"%s\"" NEWLINE,fname);
	    return;
	}

	Term_I_Stream *orig_input_stream=GetStream().GetInputStream();
	GetStream().AttachInputStream(&ifstr);

	String old_prompt(m_Prompt);

	while(ifstr.IsOpen()) {
	    m_Prompt = "file> ";
	    Send(m_Prompt);

	    // get command
	    String command_string;
	    Keystroke::Cmd ks=GetCmd(command_string);
	    
	    // execute command
	    if (ks==Keystroke::CR)
		ProcessCmd(command_string);
	}
	m_Prompt = old_prompt;
	GetStream().AttachInputStream(orig_input_stream);
    }
}

void Term_Session::DoCmdExitConfigure(const Term_Cmd_Scenario&)
{
    /*
    if (m_CurrentRoot != Kernel::Instance()->RootConfig())
	m_CurrentRoot=Kernel::Instance()->RootConfig();
    else
    */
    m_CurrentRoot=Kernel::Instance()->RootConfig();
    SetMode(TermModeNormal);
    UpdatePrompt();
}

void Term_Session::DoCmdExit(const Term_Cmd_Scenario&)
{
    //Send("Bye bye" NEWLINE);
    Disconnect();
}

void Term_Session::DoCmdLogin(const Term_Cmd_Scenario&)
{
    Login(1);
    UpdatePrompt();
}

void Term_Session::DoCmdStartup(const Term_Cmd_Scenario&)
{
    Send("Starting");
    for (int i=0; i<10; ++i) {
	sl::Sleep(200);
	Send(".");
    }
    SetMode(TermModeConfig);
    UpdatePrompt();
    sl::Sleep(500);
    Send(NEWLINE);
    Send("System Started" NEWLINE);
}

void Term_Session::DoCmdSuspend(const Term_Cmd_Scenario&)
{
    Send("Suspending");
    for (int i=0; i<10; ++i) {
	sl::Sleep(200);
	Send(".");
    }
    SetMode(TermModeInit);
    UpdatePrompt();
    sl::Sleep(500);
    Send(NEWLINE);
    Send("System Suspended" NEWLINE);
}

void Term_Session::DoCmdShutdown(const Term_Cmd_Scenario&)
{
    Send("Shutting down");
}

void Term_Session::DoCmdRestart(const Term_Cmd_Scenario&)
{
    Send("Restart" NEWLINE);
}

void Term_Session::DoCmdShowCfg(const Term_Cmd_Scenario& scenario)
{
    Term_Cfg_Writer exporter(scenario.GetEntry());
    Send(exporter.GetResult());
}

void Term_Session::DoCmdSaveCfg(const Term_Cmd_Scenario& scenario)
{
    if (!Kernel::Instance()->IsInRunningState()) {
	bool confirmed = 
	    PromptYesNo(false, "Kernel is not running. Saving configuration now "
			       "may result in lost of some configurations.\n"
			       "Do you want to continue with save ? (y/n)");
	if (!confirmed)
	    return;
    }

    Term_Cfg_Writer exporter(scenario.GetEntry());

    CVariant v_src_input;
    if (!scenario.GetCommand().GetArg((size_t)0)->GetValue(v_src_input)) {
	SendError("argument is not supplied" NEWLINE);
	return;
    }

    String src_input=v_src_input.ToString();
    if (!stricmp(src_input.c_str(), "file")) {
	CVariant v;
	scenario.GetCommand().GetArg((size_t)1)->GetValue(v);
	const char *fname = NULL;
	String sfname = v.ToString();
	String s = Kernel::Instance()->GetBaseDirectory();
	if (sfname == "") {
	    s += KERNEL_CFG;
	    fname = s.c_str();
	} else {
	    fname = sfname.c_str();
	}
	std::ofstream writer(fname);
    
	if (!writer.is_open()) {
	    SendErrorf("Unable to open file \"%s\"" NEWLINE,fname);
	    return;
	}

	cout << exporter.GetResult();
    
	writer << exporter.GetResult();
//	writer << "***************** This is a test ***********" << endl;

    }
}


void Term_Session::DoCmdTblAdd(const Term_Cmd_Scenario& scenario)
{
    if (scenario.GetEntry()->GetType()!=Cfg_Tree_Entry::CfgTypeTable) {
	SendError("the command can only be applied to table" NEWLINE);
	return;
    }
    Cfg_Tree_Table *tbl=static_cast<Cfg_Tree_Table*>(scenario.GetEntry());
    const Term_Cmd_Arg *arg=scenario.GetCommand().GetArg((size_t)0);
    if (!arg) {
	SendError("argument is not supplied" NEWLINE);
	return;
    }
    CVariant v;
    if (!arg->GetValue(v)) {
	SendError("argument is not supplied" NEWLINE);
	return;
    }

    if (GetTermMode()==TermModeInit || tbl->HasAccess(CfgAccessAdd)) {
	size_t row_num=tbl->AddRow();
	tbl->SetValue(row_num, 0, v);
    
	Cfg_Tree_Entry *e=tbl->GetChild(row_num);
	assert(e->GetType()==Cfg_Tree_Entry::CfgTypeGroupBase);
	m_CurrentRoot = static_cast<Cfg_Tree_Group_Base*>(e);
	UpdatePrompt();
    } else {
	SendError("table is read-only" NEWLINE);
    }
}

// exec command.
void Term_Session::ExecCmd(const char *cmd)
{


 cout << "************ Inside ExecCmd << "<< __FILE__ << ":" << __func__ << " *************"<< endl;

#ifndef __linux__
    char pipe_name[128];
    sprintf(pipe_name, "\\\\.\\pipe\\xgate-%p", this);
    HANDLE hNamedPipe = CreateNamedPipe (pipe_name, 
				PIPE_ACCESS_DUPLEX, 
				PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE |  PIPE_WAIT,
				PIPE_UNLIMITED_INSTANCES, 512, 512,
				1, NULL);
    if (!hNamedPipe || hNamedPipe==INVALID_HANDLE_VALUE) {
	SendErrorf ("Unable to create named pipe\n");
	return;
    }

    Sendf("Using pipe: '%s'\n", pipe_name);
#else
	
    char fifo_name[128];
    sprintf(fifo_name, "xgate-%p", this); // Confirm about the path of the pipe name
    if(mknod(fifo_name, S_IRUSR| S_IWUSR|S_IFIFO, 0) == -1){
        printf ("Unable to create named pipe\n");
        return;
    }

#endif 

    time_t now;
    time(&now);
    String snow = ctime(&now);
    snow.erase(snow.size()-1);


#ifndef __linux__
    char cmdline[256];
    sprintf(cmdline, "rmtexec.exe xgate-%p \"%s\" \"Telnet exec by %s from %s [%s]\"", 
		     this, cmd, this->GetUserName(), this->GetSourceAddress().c_str(), snow.c_str());
    
    STARTUPINFO si;
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    
    PROCESS_INFORMATION pi;
    memset(&pi, 0, sizeof(pi));

    if (!CreateProcess(NULL, cmdline, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
	SendErrorf ("Unable to execute rmtexec.exe\n");
	CloseHandle(hNamedPipe);
	return;
    }
#else
    char cmdline[256];
    sprintf(cmdline, "rmtexec.exe xgate-%p \"%s\" \"Telnet exec by %s from %s [%s]\"", 
		this, cmd, this->GetUserName(), this->GetSourceAddress().c_str(), snow.c_str());

    /* pid_t pid;
    pid = fork();
    if(pid == 0 ) // child_process
    {
	//
    }
    else{ // parent
    */ //this is commented by rkv trace 1
	

#endif

    char buffer[512];
    int len;
    unsigned long ulen;


#ifndef __linux__
    ::ConnectNamedPipe(hNamedPipe, NULL);
#else
	int hNamedPipe = open(fifo_name,O_WRONLY);
	if(hNamedPipe == -1){
		cout << "Unable to open the file" << endl;
	}

#endif


    fd_set fd;
    int sock = GetStream().GetHandle();
    FD_ZERO(&fd);

    for (;;) {
	FD_SET(sock, &fd);
	timeval timeout = {0, 10000};

	if (select(1, &fd, NULL, NULL, &timeout)!=0) {
	    // Receive data from socket, send to named pipes.
	    len = recv(sock, buffer, 512, 0);
	    if (len < 1) {
		// Send special character to notify the process that it
		// should quit.
		buffer[0] = buffer[1] = buffer[2] = 1;
		#ifndef __linux__
		::WriteFile(hNamedPipe, buffer, 3, &ulen, NULL);
		::FlushFileBuffers(hNamedPipe);
		#else
			write(hNamedPipe,buffer,3);
		#endif 
		goto on_return;
	    }
	#ifndef __linux__
	    if (!::WriteFile(hNamedPipe, buffer, len, &ulen, NULL))
		goto on_return;
	#else
	    int ret = write(hNamedPipe, buffer, len);
		if(ret == 0){
			cout << "Read end may be closed"<< endl;	
			goto on_return;
		}else if(ret < 0){
			cout << "Error in writing"<< endl;
		}else
			cout << "Successfully written" << endl;

	#endif 
	}
#ifndef __linux__
	if (::WaitForSingleObject(pi.hProcess, 100)==WAIT_OBJECT_0) {
	    // Process has terminated
	    goto on_return;
	}
#else

#endif

#ifndef __linux__
	if (::PeekNamedPipe(hNamedPipe, NULL, 0, NULL, &ulen, NULL) && ulen>0) {
	    if (::ReadFile(hNamedPipe, buffer, 512, &ulen, NULL) && ulen>0) {
		if (send(sock, buffer, ulen, 0) != ulen)
		    goto on_return;
	    }
	}
#endif // Need different linux implementation as linux has no bidirectional pipes
    }

on_return:

#ifndef __linux__
    ::DisconnectNamedPipe(hNamedPipe);
    ::CloseHandle(hNamedPipe);
    ::CloseHandle(pi.hThread);
    ::CloseHandle(pi.hProcess);
#else
	// NLI
    close(hNamedPipe);

#endif
    Sendf("\nExec session terminated.\n");

#ifdef __linux__
//} //commented rkv trace1
#endif
}

void Term_Session::DoCmdTerm(const Term_Cmd_Scenario &scenario)
{
    const Term_Cmd &term_cmd=scenario.GetCommand();

    CVariant v1;
    term_cmd.GetArg((size_t)0)->GetValue(v1);
    const IString cmd = v1.ToString();

    if (cmd == "echo") {
	m_EchoEnabled = !scenario.IsNo();
	Sendf( "Echo is %s" NEWLINE, m_EchoEnabled? "on" : "off");
    } else if (cmd == "logging") {
	if (!term_cmd.GetArg(1)->HasValue()) {
	    if (scenario.IsNo()) {
		// disable logging
		if (m_IsAttachedToLog) {
		    KLog::Instance()->Detach(this);
		    m_IsAttachedToLog = false;
		}
	    } else {
		// enable logging
		if (!m_IsAttachedToLog) {
		    KLog::Instance()->Attach(this);
		    m_IsAttachedToLog = true;
		}
	    }
	    Sendf("Terminal logging is %s" NEWLINE, m_IsAttachedToLog ? "enabled" : "disabled");
	} else {
	    if (scenario.IsNo()) {
		// disable application logging
		__m_AcceptAppLog = false;
	    } else {
		// enable application logging
		__m_AcceptAppLog = true;
	    }
	    Sendf("Application logging is %s" NEWLINE, __m_AcceptAppLog ? "enabled" : "disabled");
	}

    } else if (cmd == "show") {
	Sendf( " Show logging and debug messages to terminal: %s" NEWLINE, m_IsAttachedToLog ? "enabled" : "disabled");
	Sendf( " Show application logging to terminal: %s" NEWLINE, __m_AcceptAppLog ? "enabled" : "disabled");
	Sendf( " Echo: %s" NEWLINE, m_EchoEnabled? "on" : "off");

    } else if (cmd == "history") {
	int i;
	TermHist::iterator it, end;
	end = m_History.end();
	for (i=0, it=m_History.begin(); it!=end; ++it, ++i) {
	    Term_Command_Buffer & h = *it;
	    Sendf ("%2d %s\n", i, h.Buffer());
	}

    } else if (cmd == "verbosity") {
	CVariant v;
	term_cmd.GetArg(1)->GetValue(v);
	LogVerbosity level = Log_StrToLevel(v.AsString());
	/*
	if (level < LOG_INFO)
	    level = LOG_INFO;*/
	this->__m_Verbosity = level;
	Sendf("Terminal logging verbosity is %s\n", Log_LevelToName(level));
    } else if (cmd == "exec") {

	if (GetPrivilegeLevel() != PrivilegeLevelAdmin) {
	    SendErrorf("Only admins can execute system commands.\n");
	    return;
	}

	CVariant v;
	term_cmd.GetArg(1)->GetValue(v);

	ExecCmd(v.AsString());
    } else if (cmd == "disconnect") {
	if (GetPrivilegeLevel() != PrivilegeLevelAdmin) {
	    SendErrorf("Only admins can disconnect terminals\n");
	    return;
	}

	CVariant term_id;
	term_cmd.GetArg(1)->GetValue(term_id);

	char msg[80];
	_snprintf(msg, 80, "\n*** You've been disconnected by %s [%s]\n", GetUserName(), GetSourceAddress().c_str());
	Config_Controller::Instance()->DisconnectTerm(term_id.AsInt(), msg);

    } else if (cmd == "send") {
	CVariant term_id;
	term_cmd.GetArg(1)->GetValue(term_id);
	CVariant vmsg;
	term_cmd.GetArg(2)->GetValue(vmsg);
	
	char temp[80];
	_snprintf(temp, 80, "\n*** Message from %s [%s]: ", GetUserName(), GetSourceAddress().c_str());
	temp[79] = '\0';

	String msg(temp+vmsg.ToString()+"\n");
	if (!stricmp(term_id.AsString(), "all"))
	    Config_Controller::Instance()->SendMessage(UINT_MAX, msg);
	else {
	    unsigned id;
	    char *err;
	    id = strtoul(term_id.AsString(), &err, 10);
	    if (*err == '\0')
		Config_Controller::Instance()->SendMessage(id, msg);
	    else {
		SendErrorf ("Invalid terminal id.\n");
	    }
	}
    }
}

void Term_Session::DoCmdEcho(const Term_Cmd_Scenario &scenario)
{
    CVariant v;
    scenario.GetCommand().GetArg((size_t)0)->GetValue(v);
    Send(v.ToString()+NEWLINE);
}

void Term_Session::SetRoot ( Cfg_Tree_Group_Base * entry )
{
    if (m_TermMode == TermModeConfig || m_TermMode == TermModeInit) {
	m_CurrentRoot = entry;
	UpdatePrompt();
    }
}

void Term_Session::DoCmdTop(const Term_Cmd_Scenario &scenario)
{
    SetRoot ( Kernel::Instance()->RootConfig() );
}

void Term_Session::RunTermCommand(Term_Cmd_Scenario& scenario)
{
    Term_Cmd &term_cmd=scenario.GetCommand();
    if(!term_cmd.IsNoValid() && scenario.IsNo()) { // by MIM
	SendError("'no' command is not valid for this keyword" NEWLINE);
    } else {
    switch(term_cmd.GetId()) {
    case CMD_CONFIGURE:
	DoCmdConfigure(scenario);
	break;
    case CMD_EXIT_CONFIGURE:
	DoCmdExitConfigure(scenario);
	break;
    case CMD_EXIT:
	DoCmdExit(scenario);
	break;
    case CMD_LOGIN:
	DoCmdLogin(scenario);
	break;
    case CMD_STARTUP:
	DoCmdStartup(scenario);
	break;
    case CMD_SUSPEND:
	DoCmdSuspend(scenario);
    	break;
    case CMD_SHUTDOWN:
	DoCmdShutdown(scenario);
	break;
    case CMD_RESTART:
	DoCmdRestart(scenario);
	break;
    case CMD_LOAD_CFG:
	DoCmdLoadConfig(scenario);
	break;
    case CMD_SHOW_CFG:
	DoCmdShowCfg(scenario);
	break;
    case CMD_SAVE_CFG:
	DoCmdSaveCfg(scenario);
	break;
    case CMD_TBL_ADD:
	DoCmdTblAdd(scenario);
	break;
    case CMD_TERM:
	DoCmdTerm(scenario);
	break;
    case CMD_ECHO:
	DoCmdEcho(scenario);
	break;
    case CMD_TOP:
	DoCmdTop(scenario);
	break;
    default:
	/*
	if(scenario.GetEntry()->GetSubscriber()) {
	    Term_Cmd_Target &target=*scenario.GetEntry()->GetSubscriber();
	    
	    target.DoTermCmd(this, term_cmd);
	} else
	    SendError("Unrecognized command" NEWLINE);
	*/
	term_cmd.GetTarget()->DoTermCmd (this, scenario.IsNo(), & term_cmd);
	break;
    };
}
}

void Term_Session::RunCfgListCommand(Term_Cmd_Scenario& scenario)
{
    switch(GetTermMode()) {
    case TermModeInit:
    case TermModeConfig:
	{
	    Cfg_Tree_Entry &e=*scenario.GetEntry();
	    if (e.GetType()!=Cfg_Tree_Entry::CfgTypeGroup &&
		e.GetType()!=Cfg_Tree_Entry::CfgTypeGroupBase && 
		e.GetType()!=Cfg_Tree_Entry::CfgTypeTable ) {
		Send( CreateErrMsg(scenario.GetErrorPos(), "Missing argument"));
	    } else {
		m_CurrentRoot=static_cast<Cfg_Tree_Group_Base*>(&e);
		UpdatePrompt();
	    }
	}
	break;
    default:
	Send( CreateErrMsg(scenario.GetErrorPos(), "Missing argument"));
	break;
    }
}

STATUS Term_Session::Login(size_t retry_count)
{
    if (retry_count==0)
	return NOT_OK;


    size_t old_hist_size=m_HistorySize;
    bool old_echo_enabled=m_EchoEnabled;
    bool old_glob_enabled=m_GlobEnabled;
    bool old_char_mode_enabled=m_CharacterModeEnabled;
    Term_Mode old_mode=GetTermMode();
 
    SetMode(TermModeLogin);
    SetHistorySize(0);
    //SetEcho(false);
    SetExtendedKeystroke(false);
    SetGlob(false);

    STATUS login_result=NOT_OK;
    do {
	String login;
#if 1 // for_testing
	SendPrompt("login: ");
	Keystroke::Cmd cmd=GetCmd(login);

	SendPrompt("password: ");
	String passwd;
	SetEcho(false);
	cmd=GetCmd(passwd);
	Send(NEWLINE);
	SetEcho(old_echo_enabled);

	if (cmd!=Keystroke::CR) {
	    login_result=NOT_OK;
	    break;
	}
#endif

	PrivilegeLevel l=
	    CSecurityProvider::Instance()->AuthenticateUser(login, passwd, GetSourceAddress());

	/* narsim added for testing purpose */
//	l = PrivilegeLevelAdmin;
	if (l!=PrivilegeLevelNoAccess) //for_testing
	{ 
	    //m_UserPrivilege = PrivilegeLevelUser;
	//    m_UserPrivilege = PrivilegeLevelAdmin;  // privilege hard coded
	//    m_UserName = "umn";
	    m_UserPrivilege = l;
	    m_UserName = login;
	    
	    String success_msg;
	    success_msg = "Login success as " +  login + NEWLINE;
	    SendMessage(success_msg);

	    login_result=OK;
	    break;
	}

    } while (--retry_count>0);

    SetHistorySize(old_hist_size);
    //SetEcho(old_echo_enabled);
    SetExtendedKeystroke(true);
    SetMode(old_mode);
    SetGlob(old_glob_enabled);
    SetCharacterMode(old_char_mode_enabled);
    cout << "******* Login Successful ***** " << endl;

    if (login_result==OK) {
	CLog::Info (TERM, "%s has logged in from %s", 
			  GetUserName(), 
			  GetSourceAddress().c_str());
    } else
	SendError("Login failed" NEWLINE);

    UpdatePrompt();
    return login_result;
}


String Term_Session::CreateErrMsg(int location, const String& err_msg)
{
    if (location<0)
	location=0;

    String user_msg;
    String spaces((size_t)GetPrompt().size()+location, ' ');
    user_msg+=spaces;

    user_msg+="^" NEWLINE;
    user_msg+=m_ErrHeader;
    user_msg+=err_msg;
    user_msg+=NEWLINE;
    
    return user_msg;
}

String Term_Session::CreateAvailableChoices(const List<Term_Cmd_Scenario::NextWord> &nexts)
{
    if (nexts.empty()) {
	OnErrorKey();
	return "";
    }
    
    // calculate the longest word
    List<Term_Cmd_Scenario::NextWord>::const_iterator it;
    size_t longest_word_len=0;
    for (it=nexts.begin(); it!=nexts.end(); it++) {
	if (it->m_Word.size() > longest_word_len)
	    longest_word_len=it->m_Word.size();
    }

    if (longest_word_len + 10 >= GetTermWidth()) {
	SendError("Unable to display choices because one of the "
	          "choice has length that exceeds current terminal"
		  "width" NEWLINE);
	return "";
    } else if (longest_word_len < 10)
	longest_word_len = 10;


    String choice_line;
    choice_line.reserve(2048);

    for (it=nexts.begin(); it!=nexts.end(); it++) {
	const Term_Cmd_Scenario::NextWord & next_word = *it;

	// append command
	choice_line.append((size_t)3, ' ');
	choice_line += next_word.m_Word;
	
	// append space between command and description
	String spaces((size_t)3 + longest_word_len - next_word.m_Word.size(), ' ');
	choice_line+=spaces;

	// the rest is description.
	choice_line += next_word.m_Desc;
	//choice_line.append((size_t)1, '\r');
	choice_line.append((size_t)1, '\n');
    }

    return choice_line;
}

String Term_Session::CreateCmdWithPrompt(const Term_Command_Buffer& cur_buf)
{
    String cmd_with_prompt;

    cmd_with_prompt+=GetPrompt();
    cmd_with_prompt+=cur_buf.Buffer();
    
    if (cur_buf.Size()-cur_buf.Pos()>0) {
	String bs(cur_buf.Size()-cur_buf.Pos()+1, cur_buf.Size()-cur_buf.Pos()+1);
	bs.append((size_t)cur_buf.Size()-cur_buf.Pos(), '\b');
	cmd_with_prompt += bs;
    }

    return cmd_with_prompt;
}

void Term_Session::Run()
{
    Config_Controller * driver=Config_Controller::Instance();

    // send welcome msg
    GetStream().SendFormatted(driver->GetWelcomeMsg().c_str(), 
			      driver->GetWelcomeMsg().size());

    Term_Telnet_IO_Stream& stream = (Term_Telnet_IO_Stream&)GetStream();
    stream.SetReadTimeout(15*1000);

    // login
    if (Login(3)!=OK) {
	GetStream().Close();
	return;
    }

    stream.SetReadTimeout(0);

    // command loop
    String cmd;

    while(GetStream().IsOpen()) {

	// send prompt
	//cout << "*************** Going to send prompt ****************"<< endl;
	GetStream().SendRaw(GetPrompt().c_str(), GetPrompt().size());

	// get command
	//cout << "*************** Going to get the command  ****************"<< endl;
	Keystroke::Cmd ks=GetCmd(cmd);

	// execute command
	if (ks==Keystroke::CR)
	    ProcessCmd(cmd);
    }

    if (strlen(GetUserName())) {
	CLog::Info (TERM, "%s logged off from %s", GetUserName(), this->GetSourceAddress().c_str());
    } else {
	CLog::Info (TERM, "Terminal %s disconnected", this->GetSourceAddress().c_str());
    }
}


bool Term_Session::SendErrorf(const char *format, ...)
{
    if (!Send(m_ErrHeader.c_str(), m_ErrHeader.size()))
	return false;

    va_list va;
    int len;

    // Only protect in RELEASE mode, so that bugs can be detected early.
#ifdef NDEBUG
    try {
#endif
	va_start(va, format);
	len = _vsnprintf (m_Buffer, BUFFER_SIZE, format, va);
	va_end (va);
#ifdef NDEBUG
    } catch (...) {
	CLog::Fatal (TERM, "String format error in vsprintf. Format:'%s'", format);
	return false;
    }
#endif

    if (len < 0)
	return false;

    return Send(m_Buffer, len);
}

bool Term_Session::Sendf(const char *msg_format, ...)
{
    va_list va;
    int len;

    // Only protect in RELEASE mode, so that bugs can be detected early.
#ifdef NDEBUG
    try {
#endif
	va_start(va, msg_format);
	len = _vsnprintf (m_Buffer, BUFFER_SIZE, msg_format, va);
	va_end (va);
#ifdef NDEBUG
    } catch (...) {
	CLog::Fatal (TERM, "String format error in vsprintf. Format:'%s'", msg_format);
	return false;
    }
#endif

    if (len < 0)
	return false;

    return Send(m_Buffer, len);
}

const char* Term_Session::GetName( ) 
{
    return "term";
}

//STATUS Term_Session::__Write( LogVerbosity v, size_t length, const char *data )
bool Term_Session::__Write( LogVerbosity v, size_t length, const char *data )
{
    if (length) Send (data, length-1);
    return OK;
}

STATUS Term_Session::PromptInput(CVariant & result, const CVariant & value, const CVariantValidator & validator,
			  const char *msg_format, ...)
{
    // Send the prompt first.
    va_list va;
    int len;

    // Only protect in RELEASE mode, so that bugs can be detected early.
#ifdef NDEBUG
    try {
#endif
	va_start(va, msg_format);
	len = _vsnprintf (m_Buffer, BUFFER_SIZE, msg_format, va);
	va_end (va);

#ifdef NDEBUG
    } catch (...) {
	CLog::Fatal (TERM, "String format error in vsprintf. Format:'%s'", msg_format);
	return NOT_OK;
    }
#endif

    if (len < 0)
	return NOT_OK;

    String def_val = value.ToString();
    strncat(m_Buffer, "[", BUFFER_SIZE); 
    m_Buffer[BUFFER_SIZE-1] = '\0';
    strncat(m_Buffer, def_val.c_str(), BUFFER_SIZE); 
    m_Buffer[BUFFER_SIZE-1] = '\0';
    strncat(m_Buffer, "] ", BUFFER_SIZE); 
    m_Buffer[BUFFER_SIZE-1] = '\0';
    
    len += 2 + def_val.size();
    if (len > BUFFER_SIZE)
	len = BUFFER_SIZE;

    String cmd;
    do {
	if (Send(m_Buffer, len) == false)
	    return NOT_OK;

	Keystroke::Cmd ks=GetCmd(cmd);
	if (ks != Keystroke::CR)
	    return NOT_OK;

	if (cmd.empty()) {
	    result = value;
	    return OK;
	}

	if (CVariant::Factory (result, value.Type(), cmd.c_str()) == 0) {
	    if (validator.Validate(result))
		return OK;
	}

    } while (1);

    // not reached
    return NOT_OK;
}

bool Term_Session::PromptYesNo ( bool default_value, const char *msg_format, ...)
{
    // Send the prompt first.
    va_list va;
    int len;

    // Only protect in RELEASE mode, so that bugs can be detected early.
#ifdef NDEBUG
    try {
#endif
	va_start(va, msg_format);
	len = _vsnprintf (m_Buffer, BUFFER_SIZE, msg_format, va);
	va_end (va);

#ifdef NDEBUG
    } catch (...) {
	CLog::Fatal (TERM, "String format error in vsprintf. Format:'%s'", msg_format);
	return false;
    }
#endif

    if (len < 0)
	return false;

    if (default_value == true)
	strncat(m_Buffer+len, " [y] ", BUFFER_SIZE-len);
    else
	strncat(m_Buffer+len, " [n] ", BUFFER_SIZE-len);

    len += 5;
    if (len >= BUFFER_SIZE)
	len = BUFFER_SIZE-1;

    String cmd;
    do {
	if (Send(m_Buffer, len) == false)
	    return false;

	Keystroke::Cmd ks=GetCmd(cmd);
	if (ks != Keystroke::CR)
	    return false;

	if (cmd.empty())
	    return default_value;

	if (cmd.size()==1) {
	    if (tolower(cmd.c_str()[0]) == 'y')
		return true;
	    else if (tolower(cmd.c_str()[0]) == 'n')
		return false;
	}
    } while (1);

    // not reached
    return false;
}

///////////////////////////////////////////////////////////////////////////////
bool Term_I_Stream::GetKeystroke(Keystroke& ks)
{
    TerminalCapability &termcap=GetTermCaps();

    unsigned char c;
    if (!Read(&c, 1)) return false;
    
    if (c==27) {
	if (!Read(&c, 1)) return false;

	if (c!=termcap.GetStartCmd()) {
	    ks.SetError();
	    return true;
	}

	if (!Read(&c, 1)) return false;
	ks=termcap.TranslateEsc(c);

    } else
	ks=termcap.Translate(c);

    return true;
}

bool Term_O_Stream::SendRaw(const char *buf, int len)
{
    assert(buf);
    
    if (len<0)
	len=strlen(buf);

    return Write(buf, len);
}

bool Term_O_Stream::SendFormatted(const char* message, int len)
{
    return SendRaw(message, len);
}

Term_IO_Stream::Term_IO_Stream(Term_I_Stream *is, Term_O_Stream *os)
: Term_I_Stream(is ? is->IsInteractive() : false), m_InputStream(is), m_OutputStream(os)
{
}

void Term_IO_Stream::AttachInputStream(Term_I_Stream* strm)
{
    m_InputStream=strm;
    if (m_InputStream)
	isInteractive = strm->IsInteractive();
    else
	isInteractive = false;
}

void Term_IO_Stream::AttachOutputStream(Term_O_Stream* strm)
{
    m_OutputStream=strm;
}

bool Term_IO_Stream::IsOpen()
{
    return (m_InputStream && m_InputStream->IsOpen() &&
	    m_OutputStream && m_OutputStream->IsOpen());
}

void Term_IO_Stream::Close()
{
    if (m_InputStream)
	m_InputStream->Close();
    if (m_OutputStream)
	m_OutputStream->Close();
    m_InputStream = 0;
    m_OutputStream = 0;
}

bool Term_IO_Stream::Read(unsigned char *out, size_t len)
{
    if (!m_InputStream || !m_InputStream->IsOpen())
	return false;

    return m_InputStream->Read(out, len);
}

bool Term_IO_Stream::SendRaw(const char *buf, int len)
{
    if (!m_OutputStream)
	return false;

    return m_OutputStream->SendRaw(buf, len);
}

bool Term_IO_Stream::Write(const char *buf, size_t len)
{
    if (len==0)
	return true;

    if (buf && m_OutputStream && m_OutputStream->IsOpen())
	return m_OutputStream->Write(buf, len);
    else
	return false;
}

bool Term_IO_Stream::SendFormatted(const char* message, int len)
{
    if (!m_OutputStream)
	return false;

    return m_OutputStream->SendFormatted(message, len);
}


bool Term_IO_Stream::GetKeystroke(Keystroke& ks)
{
    if (!m_InputStream) {
	ks.SetCmd(Keystroke::CTRL_C);
	return false;
    } else
	return m_InputStream->GetKeystroke(ks);
}

TerminalCapability& Term_IO_Stream::GetTermCaps() const
{
    if (!m_InputStream)
	return Term_I_Stream::m_DefaultCapability;

    return m_InputStream->GetTermCaps();
}

///////////////////////////////////////////////////////////////////////////////
Term_Telnet_I_Stream::Term_Telnet_I_Stream(PTelnetSocket *p)
: Term_I_Stream(true), m_TelnetSocket(p), m_TermCaps(NULL)
{
}

Term_Telnet_I_Stream::~Term_Telnet_I_Stream()
{
}

bool Term_Telnet_I_Stream::IsOpen()
{
    return m_TelnetSocket && m_TelnetSocket->IsOpen();
}

void Term_Telnet_I_Stream::Close()
{
    if (m_TelnetSocket)
	m_TelnetSocket->Close();
}

bool Term_Telnet_I_Stream::Read(unsigned char *buf, size_t len)
{
    if (m_TelnetSocket)
	return m_TelnetSocket->Read(buf, len)!=0;
    else
	return false;
}

PTelnetSocket *Term_Telnet_I_Stream::GetSocket()
{
    return m_TelnetSocket;
}

TerminalCapability& Term_Telnet_I_Stream::GetTermCaps() const
{
    return Term_I_Stream::GetTermCaps();
    /*
    if (m_TermCaps == NULL) {
	if (m_TelnetSocket) {
	    const PString & ttype = m_TelnetSocket->GetTerminalType();
	    if (!stricmp(ttype.c_str(), "ansi") || !stricmp(ttype.c_str(), "vt100")) {
		m_TermCaps = &g_StaticVT100TerminalCapability;
	    } else if (!stricmp(ttype.c_str(), "vt52"))
		m_TermCaps = &g_StaticVT52TerminalCapability;
	    else
		m_TermCaps = &g_StaticVT100TerminalCapability;
	} else
	    m_TermCaps = &g_StaticVT100TerminalCapability;
    }
    return *m_TermCaps;
    */
}

Term_Telnet_O_Stream::Term_Telnet_O_Stream(PTelnetSocket *p)
: m_TelnetSocket(p)
{
}

Term_Telnet_O_Stream::~Term_Telnet_O_Stream()
{
}

bool Term_Telnet_O_Stream::IsOpen()
{
    return m_TelnetSocket && m_TelnetSocket->IsOpen();
}

void Term_Telnet_O_Stream::Close()
{
    if (m_TelnetSocket)
	m_TelnetSocket->Close();
}

/** Translates \n into \r\n
 */
bool Term_Telnet_O_Stream::WriteText(const char *buf, size_t len)
{
    if (m_TelnetSocket) {
	bool crSent = false;
	size_t start = 0;
	size_t partlen = 0;
	for (size_t i=0;i<len;i++) {
	    partlen++;
	    switch (buf[i]) {
	    case '\r':
		crSent = true;
		break;
	    case '\n':
		if (!crSent) {
		    if ((partlen-1 != 0) && (m_TelnetSocket->Write(buf+start, partlen-1) == 0)) return false;
		    if (m_TelnetSocket->Write("\r\n", 2) ==0) return false;
		} else {
		    if (partlen && (m_TelnetSocket->Write(buf+start, partlen) == 0)) return false;
		    crSent = false;
		}
		start += partlen;
		partlen = 0;
		break;
	    default:
		crSent = false;
		break;
	    }
	}
	if (partlen && (m_TelnetSocket->Write(buf+start, partlen) == 0)) return false;
	return true;
    } else {
	return false;
    }
}

bool Term_Telnet_O_Stream::Write(const char *buf, size_t len)
{
    return WriteText(buf,len);
}

PTelnetSocket *Term_Telnet_O_Stream::GetSocket()
{
    return m_TelnetSocket;
}


Term_Telnet_IO_Stream::Term_Telnet_IO_Stream(int sock)
: m_Sock(new PTelnetSocket(sock)), Term_IO_Stream(NULL, NULL)
{
    m_IS = new Term_Telnet_I_Stream(m_Sock);
    m_OS = new Term_Telnet_O_Stream(m_Sock);

    AttachInputStream(m_IS);
    AttachOutputStream(m_OS);
}

Term_Telnet_IO_Stream::~Term_Telnet_IO_Stream()
{
    Close();
    AttachInputStream(NULL);
    AttachOutputStream(NULL);
    delete m_IS;
    delete m_OS;
    delete m_Sock;
}

/**
  Get the associated handle.
 */
int Term_Telnet_IO_Stream::GetHandle() const 
{
#if 1
    //return m_Sock->GetHandle();
    return m_Sock->GetHandledummy();
   // return m_Sock;
#endif
}

void Term_Telnet_IO_Stream::SetReadTimeout(int msec) 
{
    m_Sock->SetReadTimeout(msec);
}

void Term_Telnet_IO_Stream::SendErrorIndication()
{
    char c=TelnetTerminalCapability::BELL;
    Write(&c, 1);
}

///////////////////////////////////////////////////////////////////////////////
Term_Telnet_Session::Term_Telnet_Session(Term_Controller& con, int sock)
: Term_Session(&con), m_SockStream(sock)
{
    sockaddr_in addr;
    int addrlen = sizeof(addr);

    if (getpeername(sock, (sockaddr*)&addr, &addrlen) == 0) {
	ipAddress = inet_ntoa (addr.sin_addr);
    } else
	ipAddress = "unknown";
}

Term_Telnet_Session::~Term_Telnet_Session()
{
    Shutdown();
}

Term_IO_Stream& Term_Telnet_Session::GetStream()
{
    return m_SockStream;
}


void Term_Telnet_Session::OnAlphabeticKey(char c)
{
    if (m_EchoEnabled==false)
	return;

    if (m_CurrentCommand.RightSize()==0) {
	m_SockStream.SendRaw(&c, 1);
    } else {
	char insert_cmd[]={0x1b, 0x5b, 0x31, 0x40, 0x0};
	insert_cmd[4]=c;
	m_SockStream.SendRaw(insert_cmd, sizeof(insert_cmd));
    }
}

void Term_Telnet_Session::OnCommandKey(Keystroke::Cmd cmd, 
			       Term_Command_Buffer& cur_buf, 
			       const Term_Command_Buffer& prev_buf,
			       int movement)
{
    if (m_EchoEnabled==false)
	return;

    switch(cmd) {
    case Keystroke::DOWN:
    case Keystroke::UP:
	{
	    size_t prev_size=prev_buf.Size();
	    size_t prev_pos=prev_buf.Pos();
	    
	    int last_bs_movement=cur_buf.Size()-cur_buf.Pos();
	    
	    if (cur_buf.Size()<prev_size) {
		int erase_tail=prev_pos+prev_size;
		int total_movement=erase_tail+ // erase tail
		    prev_size+	// backspace
		    cur_buf.Size()+	// the command
		    last_bs_movement;// position cursor
		char *movement=new char[total_movement];
		char *pos=movement;
		
		// erase tail
		if (prev_pos) {
		    memset(pos, '\b', prev_pos);
		    pos=pos+prev_pos;
		}
		memset(pos, ' ', prev_size);
		pos=pos+prev_size;
		
		// backspace
		memset(pos, '\b', prev_size);
		pos=pos+prev_size;
		
		// the command
		memcpy(pos, cur_buf.Buffer(), cur_buf.Size());
		pos=pos+cur_buf.Size();
		
		// position cursor
		if (last_bs_movement) {
		    memset(pos, '\b', last_bs_movement);
		}
		
		m_SockStream.SendRaw(movement, total_movement);
		delete [] movement;
	    } else {
		int total_movement=prev_pos+cur_buf.Size()+last_bs_movement;
		char *movement=new char[total_movement];
		char *pos=movement;
		
		// home
		memset(pos, '\b', prev_pos);
		pos=pos+prev_pos;
		
		// current command
		memcpy(pos, cur_buf.Buffer(), cur_buf.Size());
		pos=pos+cur_buf.Size();
		
		// position cursor
		if (last_bs_movement) {
		    memset(pos, '\b', last_bs_movement);
		}
		m_SockStream.SendRaw(movement, total_movement);
		delete [] movement;
	    }
	}
	break;
    case Keystroke::LEFT:
	{
	    char c='\b';
	    m_SockStream.SendRaw(&c, 1);
	}
	break;
    case Keystroke::RIGHT:
	{
	    char c=cur_buf.GetAt(cur_buf.Pos()-1)[0];
	    m_SockStream.SendRaw(&c, 1);
	}
	break;
    case Keystroke::HOME:
	{
	    char *bs=new char[movement];
	    memset(bs, '\b', movement);
	    m_SockStream.SendRaw(bs, movement);
	    delete [] bs;
	}
	break;
    case Keystroke::END:
	{
	    m_SockStream.SendRaw(cur_buf.GetAt(cur_buf.Pos()-movement), movement);
	}
	break;
    case Keystroke::CR:
	{
	    const char *newline=NEWLINE;
	    m_SockStream.SendRaw(newline, strlen(newline));
	}
	break;
    case Keystroke::TAB:
	{
	    Term_Cmd_Scenario exec_path(cur_buf.Buffer(), cur_buf.Pos()-1, this);

	    switch(exec_path.GetStatus()) {
	    case Term_Cmd_Scenario::CMD_COMPLETE:
		{
		    String msg= NEWLINE "   <CR>" NEWLINE;
		    msg += CreateCmdWithPrompt(cur_buf);
		    Send(msg);
		}
		break;
	    case Term_Cmd_Scenario::CMD_ERROR:
		{
		    String msg=NEWLINE;
		    msg += CreateErrMsg(exec_path.GetErrorPos(), exec_path.GetErrorMsg());
		    msg += CreateCmdWithPrompt(cur_buf);

		    Send(msg);
		}
		break;
	    case Term_Cmd_Scenario::CMD_AWAIT_COMMAND:
	    case Term_Cmd_Scenario::CMD_AWAIT_CMD_ARG:
	    case Term_Cmd_Scenario::CMD_AWAIT_CFG_ARG:
		if (exec_path.GetNextWords().size()==1) {
		    int prev_len=cur_buf.Pos();

		    const char *str=exec_path.GetWordCompletion().c_str();
		    while(*str)
			cur_buf.Insert(*str++);
		    cur_buf.Insert(' ');

		    String msg;
		    msg.append( (size_t)prev_len, '\b');
		    msg += cur_buf.Buffer();
		    
		    const move_left_len=cur_buf.Size()-cur_buf.Pos();
		    String bs;
		    bs.append( (size_t)move_left_len, '\b');

		    msg += bs;
		    Send(msg);
    
		} else {
		    String msg=NEWLINE;
		    if (cur_buf.Pos()==0)
			msg += "Available commands:" NEWLINE;

		    msg += CreateAvailableChoices(exec_path.GetNextWords());
		    msg += CreateCmdWithPrompt(cur_buf);
		    Send(msg);
		}
		break;
	    }
	}
	break;
    case Keystroke::QM:
	{
	    String msg=NEWLINE;
	    if (cur_buf.Pos()==0)
		msg += "Available commands:" NEWLINE;

	    Term_Cmd_Scenario exec_path(cur_buf.Buffer(), cur_buf.Pos()-1, this);

	    switch (exec_path.GetStatus()) {
	    case Term_Cmd_Scenario::CMD_COMPLETE:
		msg += "    <CR>" NEWLINE;
		break;
	    case Term_Cmd_Scenario::CMD_ERROR:
		msg += CreateErrMsg(exec_path.GetErrorPos(), exec_path.GetErrorMsg());
		break;
	    case Term_Cmd_Scenario::CMD_AWAIT_CMD_ARG:
	    case Term_Cmd_Scenario::CMD_AWAIT_CFG_ARG:
	    case Term_Cmd_Scenario::CMD_AWAIT_COMMAND:
		msg += CreateAvailableChoices(exec_path.GetNextWords());
		break;
	    } 

	    msg += CreateCmdWithPrompt(cur_buf);
	    Send(msg);

	} // case QM
	break;
    case Keystroke::CTRL_C:
	{
	    char *newline="^C" NEWLINE;
	    m_SockStream.SendRaw(newline, strlen(newline));
	}
	break;
    case Keystroke::BS:
	if (cur_buf.RightSize()>0) {
	    char backspace_reply[]={0x08, 0x1b, 0x5b, 0x31, 0x50};
	    m_SockStream.SendRaw(backspace_reply, sizeof(backspace_reply));
	} else {
	    char backspace_reply[]={0x08, 0x20, 0x08};
	    m_SockStream.SendRaw(backspace_reply, sizeof(backspace_reply));
	}
	break;
    case Keystroke::DEL:
	{
	    char backspace_reply[]={0x1b, 0x5b, 0x31, 0x50};
	    m_SockStream.SendRaw(backspace_reply, sizeof(backspace_reply));
	}
	break;
    case Keystroke::BELL:
	break;
    }
}

void Term_Telnet_Session::OnErrorKey()
{
    m_SockStream.SendErrorIndication();
}

///////////////////////////////////////////////////////////////////////////////
Term_File_I_Stream::Term_File_I_Stream(const String& filename)
: Term_I_Stream(false), m_Fhnd(NULL)
{
    if (!filename.empty())
	m_Fhnd = fopen(filename.c_str(), "rt");
}

bool Term_File_I_Stream::Read(unsigned char *out, size_t size)
{
    unsigned char c[2];

    if (size==1) {
	if (fgets((char*)c, 2, m_Fhnd)) {
	    *out = c[0];
	    return true;
	} else {
	    Close();
	    return false;
	}
    } else {
	if (fgets((char*)out, size, m_Fhnd))
	    return true;
	else {
	    Close();
	    return false;
	}
    }
}

bool Term_File_I_Stream::IsOpen()
{
    return m_Fhnd!=NULL;
}

void Term_File_I_Stream::Close()
{
    if (IsOpen()) {
	fclose(m_Fhnd);
	m_Fhnd=NULL;
    }
}

bool Term_File_I_Stream::GetKeystroke(Keystroke& ks)
{
    unsigned char s[2];
    if (!Read(s, 2)) return false;
    
    if (s[0]>=32 && s[0]<128)
	ks.SetLetter(s[0]);
    else if (s[0]=='\r')
	ks.SetIgnored();
    else if (s[0]=='\n')
	ks.SetCmd(Keystroke::CR);

    return true;
}

///////////////////////////////////////////////////////////////////////////////

/** Low-level write to the underlying output stream.
 */
bool Term_Log_O_Stream::Write(const char *msg, size_t size)
{
    //
    // Warning: be sure that all strings are null-terminated.
    //
    CLog::Log (verbosity, sender.c_str(), NULL, "%s", msg);
    return true;
}


///////////////////////////////////////////////////////////////////////////////
/** Constructor.
 */
Term_Generic_Session::Term_Generic_Session(Term_IO_Stream *io)
: Term_Session(NULL), iostrm(io), identity("internal")
{
    m_UserName = "admin";
    m_UserPrivilege = PrivilegeLevelAdmin;
}

/** Destructor.
 */
Term_Generic_Session::~Term_Generic_Session()
{
    delete iostrm;
}

/** The terminal command loop until EOF is reached on the input stream.
 */
void Term_Generic_Session::Run()
{
    // command loop
    String cmd;

    while(GetStream().IsOpen()) {
	m_Prompt = "init> ";

	// get command
	Keystroke::Cmd ks=GetCmd(cmd);

	// execute command
	if (ks==Keystroke::CR) {
	    // send prompt and command.
	//    Send (m_Prompt + cmd);

	    // process command
	    ProcessCmd(cmd);
	}
    }
}

/** The terminal command loop but
 */
void Term_Generic_Session::RunConfigure()
{
	ProcessCmd("configure");
	Run();
}


