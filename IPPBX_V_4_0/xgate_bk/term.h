/* $Header: /xGate/term.h 20    10/23/02 12:38p Bennylp $
 *   
 * CTENGINE
 * (C) 2001 Switchlab, Ltd
 *
 * Configuration terminal.
 *
 * $Log: /xGate/term.h $
 * 
 * 20    10/23/02 12:38p Bennylp
 * Replace thread function with the one from thread_process.h
 * 
 * 19    7/23/01 4:53p Bennylp
 * Changed the way command history works.
 * 
 * 18    7/18/01 6:09p Bennylp
 * Commands: term exec, disconnect, show, send.
 * 
 * 17    3/07/01 17:53 Junanto
 * Added multiple users support (multi login/password/privilege)
 * 
 * 16    6/30/01 11:33p Bennylp
 * Bare bone framework for telnet (still buggy).
 * 
 * 15    5/29/01 4:42p Bennylp
 * Telnet server runs in separate thread for more availability.
 * 
 * 14    5/26/01 5:48p Bennylp
 * Added 'show user' and fixed telnet command history.
 * 
 * 13    10/05/01 12:41 Junanto
 * Replaced "\r\n" into "\n"
 * 
 * 12    8/05/01 18:30 Junanto
 * Fixed memory leak
 * 
 * 11    4/05/01 20:35 Junanto
 * The command save has the same feel as
 * the command load. The default file name is
 * System.conf
 * 
 * 10    5/01/01 1:02a Bennylp
 * Added output redirection to log.
 * 
 * 9     4/23/01 10:47p Bennylp
 * Fixed std namespace conflicts.
 * 
 * 8     4/23/01 5:40a Bennylp
 * Remove using namespace std.
 * 
 * 7     4/17/01 10:03p Bennylp
 * Added logging receptor.
 * 
 * 6     4/13/01 7:18p Bennylp
 * Fix to use VT100 by default (the VT100 arrows also work now)
 * 
 * 5     4/12/01 8:08p Bennylp
 * Integration.
 *
 */
#ifndef __CTTERMINAL_H__
#define __CTTERMINAL_H__


#include "serviceprovider.h"
#include "sl.h"
//#include "telnet.h"
#include "termsc.h"
///#include "String.h"
#include "log.h"
#include <ace/Thread.h>


class Term_Session;
class Term_Controller;
class Cfg_Tree_Entry;
class Term_IO_Stream;
class Cfg_Tree_Group_Base;
class PTelnetSocket;

//typedef pthread_t ACE_hthread_t;

///////////////////////////////////////////////////////////////////////////////
/**
  Internal flag that can be added to terminal command mode
  */
enum CommandMode {
    TermModeSingle=128,	// the command can be specified when no entry is selected
    TermModeEntry=256,	// the command applies to all entry
    TermTableCmd=512,	// the command applies to table
};


///////////////////////////////////////////////////////////////////////////////
/**
  Single keyboard keystroke representation. Descendants of class Term_IO_Stream
  should capture the input from the remote user, and translate it to known
  keystroke values defined here.
  */
struct Keystroke {
    /** Type of a keystroke */
    enum Type {
	TypeLetter, // normal alphabetic
	TypeCmd,    // command (e.g. cursor movement, etc)
	TypeError,  // invalid keystroke
	TypeIgnored,// ignored keystroke
    } m_Type;

    /** Type of command */
    enum Cmd {
	LEFT=1,	// cursor LEFT movement
	RIGHT,	// cursor RIGHT movement
	DOWN,	// cursor DOWN movement
	UP,	// cursor UP movement
	HOME,	// cursor HOME movement
	END,	// cursor END movement
	CTRL_C,	// Break
	CR,	// Cariage-return
	BS,	// Backspace
	TAB,	// Horizontal tab
	QM,	// Question mark
	BELL,	// audible bell
	DEL,	// Delete
    };

    /** Internal buffer */
    char m_Data[5];

    /** Extract the alphabetic keystroke (only apply if m_Type is letter) */
    char GetLetter() const
    { 
	return m_Data[0];
    }

    /** Extract the command (only apply if m_Type is cmd) */
    Cmd GetCmd() const 
    { 
	return (Cmd)*(int*)&m_Data[0]; 
    }

    /** Set the keystroke to be alphabetic */
    void SetLetter(char c) 
    { 
	m_Data[0]=c;
	m_Type=TypeLetter; 
    }
  
    /** Set the keystroke to be command */
    void SetCmd(Cmd c) 
    { 
	*(int*)&m_Data[0]=(int)c; m_Type=TypeCmd; 
    }

    /** Set the keystroke as error */
    void SetError() 
    { 
	m_Type=TypeError; 
    }

    /** Set to ignore the keystroke */
    void SetIgnored() 
    { 
	m_Type=TypeIgnored; 
    }

}; // class Keystroke


///////////////////////////////////////////////////////////////////////////////
/**
  Terminal capability description
  */
struct TerminalCapability {

    int m_NormalTranslation[256];
    int m_EscTranslation[256];

    char m_StartCmd;

    /** Initialize the translation table */
    virtual void Init() = 0;

    /** Get the start-cmd character */
    char GetStartCmd() const { return m_StartCmd; }

    /** Translate a character to the corresponding keystroke */
    Keystroke Translate(char c) const { 
	Keystroke ks;

	if (m_NormalTranslation[c])
	    ks.SetCmd((Keystroke::Cmd)m_NormalTranslation[c]);
	else if (c>=32 && c<=127)
	    ks.SetLetter(c);
	else
	    ks.SetIgnored();

	return ks;
    }

    /** Translate a character that's preceeded by escape and start-cmd
        to the corresponding keystroke */
    Keystroke TranslateEsc(char c) const { 
	Keystroke ks;
	if (m_EscTranslation[c])
	    ks.SetCmd((Keystroke::Cmd)m_EscTranslation[c]);
	else
	    ks.SetError();

	return ks;
    }
};


///////////////////////////////////////////////////////////////////////////////
/**
  Terminal driver, the manager of all terminal activities. Terminal driver
  creates and manages terminal controllers, and terminal controllers
  creates terminals.
  */
class Config_Controller : public ServiceProvider {
public:
    /** Create the singleton instance.
     */
    static Config_Controller * Create();

    /**
      Get the instance of terminal driver
      */
    static Config_Controller * Instance() { 
	return configControllerInstance; 
    }

    /** This function will be called by the service framework after 
        instantiation of this class, to allow the instance to initialize
	itself.
     */
    virtual bool Load();

    /** This function will be called by the service framework before it is
        destroyed/deleted, to allow the instance to cleanup itself.
     */
    virtual bool Unload();

    /**
      Event polling
      */
    void HandleEvents();

    /** Acquire lock, before modifying entries.
	Return NOT_OK if the lock can't be acquired.
     */
    STATUS Lock (Term_Session *term);

    /** Release lock.
     */
    void Unlock();

    /**
      Application MUST call this function when it deletes a configuration entry,
      so that current terminal users that are currently using the entry can be
      notified.
     */
    void NotifyEntryDeleted ( Term_Session *sender, Cfg_Tree_Entry * entry );

    /**
      Get the host name for prompt.
      The hostname will be used as the left-most part of the prompt.
      */
    const String & GetHostname() const { 
	return m_Hostname; 
    }

    /**
      Get the welcome message.
      Usually this will be displayed when a new user connects to a terminal.
      */
    const String & GetWelcomeMsg() const { 
	return m_WelcomeMsg; 
    }

    /**
      Get the standard terminal commands.
      These are commands that apply only to root configuration entry.
      */
    const List<Term_Cmd*> & GetTermCommand() const { 
	return m_TermCmd; 
    }

    /** Show the currently logged on users to the specified terminal.
     */
    void ShowLoggedOnUsers ( Term_Session *term );

    /** Allocate a unique terminal Id.
     */
    unsigned AllocTermId();

    /** Disconnect a terminal.
     */
    void DisconnectTerm(unsigned id, const String & msg);

    /** Send a message to other terminal/user.
     */
    void SendMessage(unsigned id, const String & msg);

protected:
    Config_Controller();
    ~Config_Controller();

private:
    static Config_Controller *configControllerInstance;
    List <Term_Controller*> m_TermControllers;
    String m_Hostname;
    String m_WelcomeMsg;
    sl::CMutex m_TermMutex;
    bool isLoaded;

    // list of commands
    List<Term_Cmd*>  m_TermCmd;	// commands that apply to root entry only

    /** Initialize commands.
     */
    void InitTermCmd();

    /**
      Shutdown: close all terminal controllers
      */
    STATUS Shutdown();

    /**
      On task attached, do nothing
      */
    bool AttachTask (CTask *) { 
	assert(false);
	return false; 
    }

    /**
      On task detached, do nothing
      */
    void DetachTask (CTask *) { 
	assert(false);
    }

    /**
      On cancel job, shouldn't be called in the first place 
      */
    bool CancelJob(CTask *) { 
	assert(false); 
	return false; 
    }

    /**
     Execute terminal commands (not used).
     */
    virtual void DoTermCmd(Term_Session *, bool no, Term_Cmd *) {
	assert(false);
    }

    /**
     Show config_controller configuration (not used).
     */
    virtual bool OnShowConfig(
	const Cfg_Tree_Entry *entry,
	String & output, 
	const String & indent, 
	int indent_size) const {
	assert(false);
	return NOT_OK;
    }
};


///////////////////////////////////////////////////////////////////////////////
/**
  Terminal controller base class.
  A terminal controller owns and manages one particular class of terminal; e.g.
  a telnet terminal controller spawns a server socket and create a new telnet
  terminal whenever a new connection arrives.
  */
class Term_Controller {
public:
    /** Constructor */
    Term_Controller (const char *type );

    /** Initialize */
    virtual STATUS Init();

    /** Shutdown */
    virtual STATUS Shutdown();

    /** Event polling */
    virtual STATUS PollEvents();

    /** Notify when a configuration entry is about to be deleted. */
    void NotifyEntryDeleted (Term_Session * sender, Cfg_Tree_Entry *entry);

    /** Show the currently logged on users to the specified terminal.
     */
    void ShowLoggedOnUsers ( Term_Session *term );

    /** Get terminal type.
     */
    const char *GetTermType() const { return termType; }

    /** Disconnect a terminal.
     */
    void DisconnectTerm(unsigned id, const String & msg);

    /** Send a message to other terminal/user.
     */
    void SendMessage(unsigned id, const String & msg);

protected:
    const char *termType;

    /** Callback, to be called by a terminal when it is disconnected */
    void RemoveTerm(Term_Session&);

    /** Call to add a terminal */
    void AddTerm(Term_Session&);

    List<Term_Session*> m_Ttys, m_TtysToBeCleanup;
    sl::CMutex m_TtyMutex;

    friend class Term_Session;
};

typedef List<const String> CmdList;


///////////////////////////////////////////////////////////////////////////////
/**
  Stream base
  */
class Term_Base_Stream {
public:
    virtual ~Term_Base_Stream() {};

    /**
      Determine whether the stream is open

      @return
      true if the stream is still open
      false if the stream is closed
      */
    virtual bool IsOpen() = 0;

    /**
      Close the stream.
      */
    virtual void Close() = 0;
};

/**
  Input stream
  */
class Term_I_Stream : public Term_Base_Stream {
public:
    /** Constructor.
     */
    Term_I_Stream(bool interactive)
    : isInteractive(interactive) {}

    virtual ~Term_I_Stream() {};

    /** Determine wheter the stream is interactive.
     */
    bool IsInteractive() const {
	return isInteractive;
    }

    /**
      Low-level read from the input stream.
      This function will block until the specified length has been received.

      @return
      true to indicate success
      false to indicate failure
      */
    virtual bool Read(unsigned char *, size_t) = 0;


protected:
    bool isInteractive;

    /**
      Get a single keystroke. 
      Any extended character input will be interpreted by this stream into a 
      keystroke command according to the terminal capability translation.

      @return
      true to indicate success
      false to indicate failure
      */
    virtual bool GetKeystroke(Keystroke&);

    /**
      Get the terminal capabilities.
      The default behaviour is to return a VT-100 terminal capability.
      */
    virtual TerminalCapability& GetTermCaps() const { return m_DefaultCapability; }


private:
    static TerminalCapability &m_DefaultCapability;

    friend class Term_IO_Stream;
};

/**
  Output stream
  */
class Term_O_Stream : public Term_Base_Stream  {
public:
    virtual ~Term_O_Stream() {};

    /**
      Low-level write to the underlying output stream.
      Instead of calling this function, user is encouraged to call SendRaw()
      or SendFormatted() instead.

      @return
      true if the write is success
      false if failed
      */
    virtual bool Write(const char *, size_t) = 0;

    /**
      Send unformatted string to output. The string will be sent as is. If the
      parameter <code>len</code> is negative, the string must be NULL 
      terminated.

      The default behaviour is to call Write.

      @return
      true to indicate success
      false to indicate failure
      */
    virtual bool SendRaw(const char *, int len=-1);

    /**
      Send the string, format the string as neccessary according to the
      capability/characteristic of the output device. If parameter
      <code>len</code> is negative, the string is considered to be NULL
      terminated.

      The default behaviour is to translate single newline ("\n") to
      cr-lf ("\r\n") pair. 

      @return
      true to indicate success
      false to indicate failure
      */
    virtual bool SendFormatted(const char*, int len=-1);
};

///////////////////////////////////////////////////////////////////////////////
/**
  Terminal stream, represents input and output stream to the remote user.
  */
class Term_IO_Stream : public Term_I_Stream, public Term_O_Stream {
public:
    /**
      Create a terminal stream by attaching an input and output stream.
      Note that the caller is responsible for deleting the input and output stream
      after deleting this stream.
      */
    Term_IO_Stream(Term_I_Stream *, Term_O_Stream *);

    /**
      Copy constructor
      */
    Term_IO_Stream(const Term_IO_Stream& iostrm)
    : Term_I_Stream(iostrm.IsInteractive()), 
      m_InputStream(iostrm.m_InputStream), m_OutputStream(iostrm.m_OutputStream) {}

    /**
      Assignment operator
      */
    const Term_IO_Stream& operator=(const Term_IO_Stream& iostrm) {
	m_InputStream=iostrm.m_InputStream;
	m_OutputStream=iostrm.m_OutputStream;
	return *this;
    }

    /**
      Delete this stream.
      */
    virtual ~Term_IO_Stream() {
	if (m_InputStream) delete m_InputStream;
	if (m_OutputStream) delete m_OutputStream;
    }

    /**
      Close both direction of the stream.
      */
    virtual void Close();

    /**
      Get the input stream
      */
    Term_I_Stream* GetInputStream() { 
	return m_InputStream; 
    }

    /**
      Get the output stream
      */
    Term_O_Stream* GetOutputStream() {
	return m_OutputStream;
    }

    /**
      Attach a Term_I_Stream device as the input stream.
      */
    void AttachInputStream(Term_I_Stream* strm);

    /**
      Attach a Term_O_Stream device as the output stream.
      */
    void AttachOutputStream(Term_O_Stream* strm);

    /**
      Determine whether the stream is open

      @return
      true if the stream is still open
      false if the stream is closed
      */
    virtual bool IsOpen() ;

    /**
      Low-level read from the input stream.
      This function will block until the specified length has been received.

      @return
      true to indicate success
      false to indicate failure
      */
    virtual bool Read(unsigned char *out, size_t len);

    /**
      Low-level write to the underlying output stream.
      Instead of calling this function, user is encouraged to call SendRaw()
      or SendFormatted() instead.

      @return
      true if the write is success
      false if failed
      */
    virtual bool Write(const char *, size_t);

    /**
      Send unformatted string to output. The string will be sent as is. If the
      parameter <code>len</code> is negative, the string must be NULL 
      terminated.

      The default behaviour is to call Write.

      @return
      true to indicate success
      false to indicate failure
      */
    bool SendRaw(const char *, int len=-1);

    /**
      Send the string, format the string as neccessary according to the
      capability/characteristic of the output device. If parameter
      <code>len</code> is negative, the string is considered to be NULL
      terminated.

      The default behaviour is to call output-stream's SendFormatted(). 

      @return
      true to indicate success
      false to indicate failure
      */
    virtual bool SendFormatted(const char*, int len=-1);


    /**
      Get a single keystroke. 
      Any extended character input will be interpreted by this stream into a 
      keystroke command according to the terminal capability translation.

      The default behaviour is to call input stream's GetKeystroke().

      @return
      true to indicate success
      false to indicate failure
      */
    virtual bool GetKeystroke(Keystroke&);

    /**
      Get the terminal capabilities.
      The default behaviour is call input-stream's GetTermCaps().
      */
    virtual TerminalCapability& GetTermCaps() const;

    /**
      Get the associated handle.
     */
    virtual int GetHandle() const {
	return -1;
    }

protected:
    Term_I_Stream    *m_InputStream;
    Term_O_Stream   *m_OutputStream;
};


///////////////////////////////////////////////////////////////////////////////
/**
  Positioned buffer for terminal. 
  This buffer has 'cursor' which point to location where next insert operation 
  will occur. This is useful for e.g. Telnet, which enable user to move the
  cursor left or right, which will effect the location of the character
  insertion.
  */
class Term_Command_Buffer {
public:
    /**
      Create the buffer. Parameter <code>size</code> indicates the buffer
      size
      */
    Term_Command_Buffer(size_t size=256)
    : m_Buffer(new char[size]), m_BufferSize(size), m_Size(0), m_Pos(0)
    {
	m_Buffer[0]='\0';
    }
    
    /**
      Destructor
      */
    ~Term_Command_Buffer()
    {
	delete [] m_Buffer;
    }
    
    /**
      Standard copy constructor 
      */
    Term_Command_Buffer(const Term_Command_Buffer& x)
    : m_Buffer(new char[x.m_BufferSize]), m_BufferSize(x.m_BufferSize), 
      m_Size(x.m_Size), m_Pos(x.m_Pos)
    {
	strncpy(m_Buffer, x.m_Buffer, m_BufferSize);
	m_Buffer[m_Size]='\0';
    }
    
    /**
      Standard assignment operator
      */
    const Term_Command_Buffer& operator=(const Term_Command_Buffer &x);
    
    /**
      Comparison operator, only compares the content, not the position etc 
      */
    bool operator==(const Term_Command_Buffer& x)
    {
	return !strcmp(m_Buffer, x.m_Buffer);
    }

    /**
      Clear the buffer, and reset cursor position to zero
      */
    void Clear()
    {
	m_Buffer[0]='\0';
	m_Pos=0;
	m_Size=0;
    };
    
    /**
      Returns TRUE if the buffer is empty
      */
    bool IsEmpty() const
    {
	return m_Size==0;
    }

    /**
      Returns TRUE if the buffer is full if <code>num</code> number of 
      chars added
      */
    bool IsFull(int num=0) const
    {
	return m_Size+num>=m_BufferSize-1;
    }

    /**
      Get the current length of the buffer.
      */
    size_t Size() const
    {
	return m_Size;
    }
    
    /**
      Get the position of the cursor.
      The position is where the next insertion will occur.
      */
    size_t Pos() const
    {
	return m_Pos;
    }
    
    /**
      Get the character under the cursor
      */
    char Get() const
    {
	return m_Buffer[m_Pos];
    }
    
    /**
      Get all the buffer content
      */
    const char* Buffer() const
    {
	m_Buffer[m_Size]='\0';
	return m_Buffer;
    }
    
    /**
      Get the characters under the cursor to the right
      */
    char* GetAt(size_t pos) const
    {
	return &m_Buffer[pos];
    }
    
    /**
      Get the length of the characters from current cursor position to the end
      */
    size_t RightSize() const
    {
	return m_Size-m_Pos;
    }
    
    /**
      Get the string from the cursor position to the end
      */
    const char* Right()
    {
	return &m_Buffer[m_Pos];
    }
    
    /**
      Move the cursor.
      If <code>n</code> is negative, the cursor will move left.
      
      @return
      TRUE the movement was success
      FALSE the movement was failed
      */
    bool Move(int n)
    {
	if (m_Pos+n<0 || m_Pos+n>m_Size)
	    return false;
	m_Pos+=n;
	return true;
    }
    
    /**
      Move the cursor to the beginning of the buffer.
      
      @return
      int previous position.
      */
    int Home()
    {
	int prev_pos=m_Pos;
	m_Pos=0;
	return prev_pos;
    }
    
    /**
      Move the cursor to the end of the buffer
      */
    bool End()
    {
	if (m_Size) {
	    m_Pos=m_Size;
	    return true;
	} else {
	    return false;
	}
    }
    
    /**
      Insert a character at current cursor position
      */
    bool Insert(char c);
    
    /**
      Backspace at current position (delete previous char)
      */
    bool Backspace();
    
    /**
      Delete character at current position
      */
    bool Delete();

    
private:
    char *m_Buffer;
    int m_BufferSize, m_Size, m_Pos;
};

///////////////////////////////////////////////////////////////////////////////
/**
  Terminal base class. This class provides management of string entered by
  remote user. The descendant of this class should provide the layout-ing
  of the terminal (by overriding On??Key notifications) and the main flow
  of the terminal (by filling Run() method)
  */
class Term_Session : public CLogDevice {
public:
    Term_Session(Term_Controller*);

    virtual ~Term_Session();

    /** Get the terminal ID.
     */
    unsigned GetId() const {
	return m_Id;
    }

    /**
      Disconnect remote user.
      This function will close the underlying stream, so any pending GetCmd()
      will return with keystroke CTRL-C. This also effectively stop the 
      command loop of this terminal.

      The default behaviour is to close the underlying stream.
      */
    virtual STATUS Disconnect();

    /**
      Shutdown the terminal.
      Typically will be called by the controller to disconnect (if any) 
      currently logged on user, and break the terminal command loop.

      The default behaviour is to close the underlying stream and wait for
      thread to terminate
      */
    virtual STATUS Shutdown();

    /** Start get command from remote user. 
        This procedure won't return until user press Enter or Ctrl-C. 
	Meanwhile, the notification procedure (OnAlphabeticKey, OnErrorKey etc)
	will be called whenever a keystroke arrive.
      */
    Keystroke::Cmd GetCmd(String&);

    /** 
      Get current keystroke (or we can say the last keystroke).
      */
    const Keystroke& GetCurrentKeystroke() const { return m_CurrentKeystroke; };

    /**
      Get the I/O stream associated with this terminal.
      */
    virtual Term_IO_Stream& GetStream()=0;

    /**
      Notification when an alphanumeric keystroke arrive.
      Upon receiving an alphabetic key, this class will automatically insert
      the key to the command buffer, and call this function. The default
      behaviour is simply do nothing, but descendants may override to
      implement terminal specific sequence, like echoing in Telnet
      */
    virtual void OnAlphabeticKey(char);

    /**
      Notification when a command keystroke arrive.
      Known key commands like UP, DOWN, LEFT, RIGHT, backspace, etc will be
      preprocessed by this class to manipulate current command buffer,
      and after that this class will call this function. Descendant can
      override this function to implement protocol specific behaviour, e.g.
      in Telnet terminal.
      */
    virtual void OnCommandKey(Keystroke::Cmd, 
			      Term_Command_Buffer& cur_buf, 
			      const Term_Command_Buffer& prev_buf,
			      int movement);

    /** 
      Notification when a keystroke is not accepted. 
      The behaviour varies, but the common one will be to send/play bell. Call
      this function to trigger the error/bell.
      */
    virtual void OnErrorKey();

    /**
      Notification when a keystroke is ignored.
      Typically this happen upon receiving unknown keyboard sequence. The
      common behaviour is simply to ignore the key, but descendant can do
      something else by overriding this function.
      */
    virtual void OnIgnoredKey();

    /**
      Set the number of commands to be saved in history.
      If the parameter is zero, then the history will be disabled. This is
      particularly useful e.g. when prompting for password, which shouldn't
      be saved in history.

      The default is 20.
      */
    void SetHistorySize(size_t);

    /**
      Enable/disable the terminal echo.
      On terminal that requires echo (like Telnet), this is useful e.g. when
      receiving user password.

      Note that this class doesn't do echoing; the echoing should be done in
      the descendants via OnAlphabeticKey notification.

      The default is true (echo==ON)
      */
    void SetEcho(bool);

    /**
      Enable/disable extended keystroke.
      When extended keystroke is disabled, only backspace, CR, and CTRL-C will
      be processed by the terminal. This is useful e.g. upon receiving 
      password or when connected to intelligent client program.

      The default is true (extended keystroke==TRUE)
      */
    void SetExtendedKeystroke(bool);

    /**
      Enable/disable keyword expansion (e.g. TAB and '?')
      This is useful e.g. when connecting with intelligent client program.

      The default is true (glob==ON).
      */
    void SetGlob(bool glob) { m_GlobEnabled=glob; }

    /**
      Enable/disable character input mode (character vs line input mode)
      When in line mode, all characters will be received as they are until 
      a CR or CTRL-C has been received, and all extended keystroke, TAB and '?'
      will be discarded.

      The default is true (character mode==ON)
      */
    void SetCharacterMode(bool chmode) { m_CharacterModeEnabled=chmode; }

    /**
      Set terminal width.
      Not implemented.
      */
    virtual void SetTermWidth(size_t) {}

    /**
      Set terminal height.
      Not implemented.
      */
    virtual void SetTermHeight(size_t) {}

    /**
      Get terminal width.
      Currently not implemented; it always returns 80.
      */
    size_t GetTermWidth() const { return 80; };

    /**
      Get terminal height.
      Currently not implemented; it always returns 25
      */
    size_t GetTermHeight() const { return 25; };

    /**
      Get privilege of logged on user.
      User can change his/her privilege by calling Login.
      */
    PrivilegeLevel GetPrivilegeLevel() const { return m_UserPrivilege; }

    /**
      Acquire specific privilege.
      This function will prompt for the user to enter his/her password,
      and then a new privilege will be assign to this terminal. Parameter
      <code>retry_count</code> specifies maximum retries when the password
      is wrong.
      */
    STATUS Login(size_t retry_count);

    /**
      Get the terminal prompt.
      */
    const String& GetPrompt() const { return m_Prompt; }

    /**
      Get current terminal mode.
      */
    Term_Mode GetTermMode() const { return m_TermMode; };

    /**
      Get current root configuration group.
      When the terminal is in TermModeConfig mode, the user can 
      'change directory' to a configuration group, from which any subsequent
      command will efect. This function will return the current 'working
      group directory'.
      */
    Cfg_Tree_Group_Base* GetCurrentRootEntry() { return m_CurrentRoot; }

    /**
      Send error message to the user.
      This function provides consistent look and feel of errors.
      */
    bool SendError(const String& msg) { 
	if (!Send(m_ErrHeader.c_str(), m_ErrHeader.size()))
	    return false;
	return Send(msg.c_str(), msg.size()); 
    }

    /**
      Send error message to the user (printf style).
      This function provides consistent look and feel of errors.
      */
    bool SendErrorf(const char *format, ...);

    /**
      Send informational message to the user
      This function provides consistent look and feel of messages
      */
    bool SendMessage(const String& msg) { String s=m_MsgHeader+msg; return Send(s.c_str(), s.size()); }

    /**
      Send important message that requires immediate attention of the user.
      Example of such messages is when user is required to enter some input
      (such as login)
      */
    bool SendPrompt(const String& msg) { String s=m_PromptHeader+msg; return Send(s.c_str(), s.size()); }

    /**
      Send string to the user.
      The use of this function is limited only to send a portion of a string,
      such as a new line ("\n")
      */
    bool Send(const String& msg) { return Send(msg.c_str(), msg.size()); }

    /** Send string.
     */
    bool Send(const char *msg, size_t size) {
	return GetStream().SendRaw(msg, size);
    }

    /** Send message (printf style).
     */
    bool Sendf(const char *msg_format, ...);

    /** Prompt a message, then ask an input from user. 
        Return NOT_OK if failed to read input (such as if connection was 
	disconnected, or user pressed Ctrl-C).
     */
    STATUS PromptInput(CVariant & result, const CVariant & value, const CVariantValidator & validator,
		       const char *msg_format, ...);

    /** Prompt a yes/no choice to user, and ask an input.
	Return true if the user said yes, or false when no or when an error occured.
     */
    bool  PromptYesNo ( bool default_value, const char *msg_format, ...);

    /**
      Send raw characters to the user.
      The characters will be transmitted as is.
      */
    virtual bool SendRaw(const String& msg) { return GetStream().SendRaw(msg.c_str(), msg.size()); }

    /**
      Set new root configuration entry.
      Only valid if called when term is in Config mode. Otherwise it will be ignored.
     */
    void SetRoot ( Cfg_Tree_Group_Base * entry );

    /** Get the privilege level that this terminal user currently logged on as.
     */
    PrivilegeLevel GetPrivilegeLevel() { return m_UserPrivilege; }

    /** Get the username associated with this terminal.
     */
    const char* GetUserName() { return m_UserName.c_str(); }

    /** Get the identity of the terminal.
	For example, for telnet sessions, this will return the IP address.
     */
    virtual const String & GetSourceAddress() const = 0;

    /** Get the time when the session was started.
     */
    time_t GetStartTime() const {
	return m_StartTime;
    }

    /** Start the thread.
     */
    bool StartThread();

    /** Wait thread to finish.
     */
    void WaitThread();

protected:
    Term_Controller*	m_Controller;
    Term_Command_Buffer	m_CurrentCommand;
    size_t		m_HistorySize;
    bool		m_EchoEnabled;
    bool		m_ExtendedKeystrokeEnabled;
    bool		m_GlobEnabled;
    bool		m_CharacterModeEnabled;
    String		m_Prompt;
    Keystroke		m_CurrentKeystroke;
    Term_Mode		m_TermMode;
    PrivilegeLevel	m_UserPrivilege;
    Cfg_Tree_Group_Base	*m_CurrentRoot;
    String              m_UserName;
    bool		m_CloseFlag;
    unsigned		m_Id;
    time_t	        m_StartTime;

    String		m_MsgHeader, m_PromptHeader, m_ErrHeader;

    //ACE_hthread_t	reader_hnd_;
    pthread_t	reader_hnd_;
    ACE_hthread_t	writer_hnd_;
    static void *thread_proc(void *);
    void *ThreadProc();

    /**
      The terminal command loop.
      The default behaviour is to prompt for a password, and run
      continously until the underlying stream is closed.
      */
    virtual void Run();

    /** Set terminal mode */
    void SetMode(Term_Mode);

    /** Determine whether terminal has been closed.
     */
    bool IsClosed() const {
	return m_CloseFlag;
    }
    
    /**
      Update prompt
      Should be called after events such as mode change, login, or 'change
      directory'.
      */
    void UpdatePrompt();

    /**
      Run a command.
      Typically this will be called after successful GetCmd().
      */
    virtual void RunCommand(Term_Cmd_Scenario&);

    /**
      Run a term command.
      Called by RunCommand() to execute a Term_Cmd
      */
    void RunTermCommand(Term_Cmd_Scenario&);

    /**
      Run a cfglist command.
      Called by RunCommand() to execute a configuration entry. The implementation
      is: if mode is TermModeNormal, then this will print the value of the
      entries and its children. If mode is TermModeConfig and entry is a group,
      this will 'change directory' to that entry.
      */
    void RunCfgListCommand(Term_Cmd_Scenario&);

    /** 
      Send a string to show the user the location of the error and 
      the error message.
      */
    String CreateErrMsg(int location, const String& msg);

    /**
      Send List of next available commands to remote user.
      */
    String CreateAvailableChoices(const List<Term_Cmd_Scenario::NextWord> &);

    /**
      Send current command preceeded by prompt
      */
    String CreateCmdWithPrompt(const Term_Command_Buffer&);

protected:
    typedef List<Term_Command_Buffer> TermHist;
    TermHist	        m_History;
    TermHist::iterator  m_CurCmdCursor, m_NewCmdCursor;
    bool		m_IsAttachedToLog;

    enum {
	BUFFER_SIZE = 1024
    };
    char		m_Buffer[BUFFER_SIZE];

    /** Scroll history */
    TermHist::iterator MoveHistory(int m);

    /** Unsupported assignment operator */
    const Term_Session& operator=(const Term_Session&) { 
	assert(false); 
	return *this; 
    }

    /** Get command */
    Keystroke::Cmd GetCmdInCharMode(String& command);
    Keystroke::Cmd GetCmdInLineMode(String& command);

    /** Process command */
    void ProcessCmd(const String& cmd);

    /** Log all entered commands **/
    void LogCommand(const char *cmd);

    /** Process extended keystroke */
    void ProcessCommandKey(Keystroke::Cmd);

    /* Intrinsic commands */
    void DoCmdConfigure(const Term_Cmd_Scenario&);
    void DoCmdExitConfigure(const Term_Cmd_Scenario&);
    void DoCmdExit(const Term_Cmd_Scenario&);
    void DoCmdLogin(const Term_Cmd_Scenario&);
    void DoCmdStartup(const Term_Cmd_Scenario&);
    void DoCmdSuspend(const Term_Cmd_Scenario&);
    void DoCmdShutdown(const Term_Cmd_Scenario&);
    void DoCmdRestart(const Term_Cmd_Scenario&);
    void DoCmdLoadConfig(const Term_Cmd_Scenario&);
    void DoCmdShowCfg(const Term_Cmd_Scenario&);
    void DoCmdSaveCfg(const Term_Cmd_Scenario&);
    void DoCmdTblAdd(const Term_Cmd_Scenario&);
    void DoCmdTerm(const Term_Cmd_Scenario&);
    void DoCmdEcho(const Term_Cmd_Scenario&);
    void DoCmdTop(const Term_Cmd_Scenario &scenario);

    // Logging.
    virtual const char* GetName( );

    // write interface
    virtual bool __Write( LogVerbosity v, size_t length, const char *data );

    // exec command.
    void  ExecCmd(const char *cmd);
}; 


///////////////////////////////////////////////////////////////////////////////
/**
  Telnet terminal controller
  */
class Term_Telnet_Controller : public Term_Controller {
public:
    Term_Telnet_Controller();
    ~Term_Telnet_Controller();

    /** Open a listening socket */
    STATUS Init();
    STATUS Init(short port);

    /** Close all terminals and close the server socket */
    STATUS Shutdown();

private:
    bool	  m_QuitFlag;
    int		  m_ServerSock;
    //ACE_hthread_t m_hThread;
    pthread_t  m_hThread;

    static void *thread_proc(void *this_instance);

    int  MainThread();
    void OnNewConnection();

    void CloseSocket();
    bool InitSocket (int port, bool reuse);
};


///////////////////////////////////////////////////////////////////////////////
/**
  Input stream for telnet socket.
  In this case, the PTelnetSocket object will be shared by both 
  Term_Telnet_I_Stream and Term_Telnet_O_Stream
  */
class Term_Telnet_I_Stream : public Term_I_Stream {
public:
    Term_Telnet_I_Stream(PTelnetSocket *);
    ~Term_Telnet_I_Stream();

    virtual bool IsOpen();
    virtual void Close();
    virtual bool Read(unsigned char *, size_t);

    PTelnetSocket *GetSocket();

    /**
      Get the terminal capabilities.
      The default behaviour is to return a VT-100 terminal capability.
      */
    virtual TerminalCapability& GetTermCaps() const;

private:
    PTelnetSocket *m_TelnetSocket;
    mutable TerminalCapability *m_TermCaps;
};

/**
  Output stream for telnet socket.
  In this case, the PTelnetSocket object will be shared by both 
  Term_Telnet_I_Stream and Term_Telnet_O_Stream.
  */
class Term_Telnet_O_Stream : public Term_O_Stream {
public:
    Term_Telnet_O_Stream(PTelnetSocket *);
    ~Term_Telnet_O_Stream();

    virtual bool IsOpen();
    virtual void Close();
    virtual bool Write(const char *, size_t);

    PTelnetSocket *GetSocket();

private:
    bool WriteText(const char *,size_t);    // translates \n into \r\n
    PTelnetSocket *m_TelnetSocket;
};

/**
  Terminal stream for telnet connection
  */
class Term_Telnet_IO_Stream : public Term_IO_Stream {
public:
    Term_Telnet_IO_Stream(int sock);
    ~Term_Telnet_IO_Stream();

    /** Send error indication (bell) to remote user */
    void SendErrorIndication();

    /**
      Get the associated handle.
     */
    virtual int GetHandle() const;

    virtual void SetReadTimeout(int msec);

private:
    PTelnetSocket *m_Sock;
    Term_I_Stream  *m_IS;
    Term_O_Stream *m_OS;
};


///////////////////////////////////////////////////////////////////////////////
/**
  Telnet terminal
  */
class Term_Telnet_Session : public Term_Session {
public:
    Term_Telnet_Session(Term_Controller&, int sock);
    ~Term_Telnet_Session();

    virtual Term_IO_Stream& GetStream();
    virtual void OnAlphabeticKey(char);
    virtual void OnCommandKey(Keystroke::Cmd, 
			      Term_Command_Buffer& cur_buf, 
			      const Term_Command_Buffer& prev_buf,
			      int movement);
    virtual void OnErrorKey();

    /** Get the identity of the terminal.
	For example, for telnet sessions, this will return the IP address.
     */
    virtual const String & GetSourceAddress() const {
	return ipAddress;
    }

private:
    Term_Telnet_IO_Stream m_SockStream;
    String  ipAddress;

    const Term_Telnet_Session& operator=(const Term_Telnet_Session&) { assert(false); return *this; }
};


///////////////////////////////////////////////////////////////////////////////
/**
  File input stream
  */
class Term_File_I_Stream : public Term_I_Stream {
public:
    Term_File_I_Stream(const String& filename);

    // inherited from Term_I_Stream
    virtual bool Read(unsigned char *, size_t);
    virtual bool IsOpen();
    virtual void Close();
    virtual bool GetKeystroke(Keystroke& ks);

private:
    FILE *m_Fhnd;
};


///////////////////////////////////////////////////////////////////////////////
/** Output stream to NULL.
 */
class Term_Null_O_Stream : public Term_O_Stream {
public:
    /** Constructor.
     */
    Term_Null_O_Stream() {}

    /** Low-level write to the underlying output stream.
     */
    virtual bool Write(const char *, size_t) {
	return true;
    }

    /**
      Determine whether the stream is open

      @return
      true if the stream is still open
      false if the stream is closed
      */
    virtual bool IsOpen() {
	return true;
    }

    /**
      Close the stream.
      */
    virtual void Close() {}

};


///////////////////////////////////////////////////////////////////////////////
/** Output stream to log.
 */
class Term_Log_O_Stream : public Term_O_Stream {
public:
    /** Constructor.
     */
    Term_Log_O_Stream (LogVerbosity v, const String & sndr) 
    : verbosity(v), sender(sndr) {}

    /** Low-level write to the underlying output stream.
     */
    virtual bool Write(const char *, size_t);

    /**
      Determine whether the stream is open

      @return
      true if the stream is still open
      false if the stream is closed
      */
    virtual bool IsOpen() {
	return true;
    }

    /**
      Close the stream.
      */
    virtual void Close() {}

private:
    LogVerbosity verbosity;
    String sender;
};


///////////////////////////////////////////////////////////////////////////////
/** This generic session will read its input from the specified input stream
    until EOF and writes all the output to the output stream.
 */
class Term_Generic_Session : public Term_Session {
public:
    /** Constructor.
     */
    Term_Generic_Session(Term_IO_Stream *iostrm);

    /** Destructor.
     */
    ~Term_Generic_Session();

    /** The terminal command loop until EOF is reached on the input stream.
     */
    virtual void Run();
    void RunConfigure();

    /** Get the I/O stream associated with this terminal.
      */
    virtual Term_IO_Stream& GetStream() {
	return *iostrm;
    }

    /** Get the identity of the terminal.
	For example, for telnet sessions, this will return the IP address.
     */
    virtual const String & GetSourceAddress() const {
	return identity;
    }

private:
    Term_IO_Stream *iostrm;
    String identity;
};


#endif
