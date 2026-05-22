/* $Header: /xGate/termsc.h 5     4/23/01 10:47p Bennylp $
 *   
 * CTENGINE
 * (C) 2001 Switchlab, Ltd
 *
 * Terminal command parser/config. writer/scenario builder.
 *
 * $Log: /xGate/termsc.h $
 * 
 * 5     4/23/01 10:47p Bennylp
 * Fixed std namespace conflicts.
 * 
 * 4     4/12/01 8:09p Bennylp
 * Integration.
 *
 */
#ifndef __TERMSC_H__
#define __TERMSC_H__

#include "variant.h"
#include "termcmd.h"
#include "security.h"

class Term_Session;
class Cfg_Tree_Entry;
class Cfg_Tree_Group;
class Cfg_Tree_Group_Base;
class Cfg_Tree_Table;
class Cfg_Tree_Leaf;

///////////////////////////////////////////////////////////////////////////////
/**
  String token
  */
struct Term_Cmd_Token {
    int	pos;		// token position in the input string
    const char* token;	// the token
    unsigned flag;	// the flag. You can also use HasFlag().

    /**
      Flags
      */
    enum TokenFlag {
	InSingleQuote=1,    // this token is surrounded by single quote ('\'')
	InDoubleQuote=2,    // surrounded by double quote  ('\"')
	InBackQuote=4,	    // surrounded by back quote ('`')
    };

    /**
      Constructor
      */
    Term_Cmd_Token(int p, const char *t, unsigned f=0) : pos(p), token(t), flag(f) {}

    /**
      Determine if the token has a flag
      */
    bool HasFlag(TokenFlag) const {}
};


///////////////////////////////////////////////////////////////////////////////
/**
  This function behaves similar to LIBC's strtok, except that it recognizes
  quoted strings, comments (starts with "//"), backslash literals (not
  yet done), etc.

  Warning:
  Be aware that the returned tokens contains pointer to internal buffer of 
  this tokenizer, so those pointers will be INVALID if this tokenizer is
  deleted.
  */
class Term_Cmd_Tokenizer {

public:
    /**
      Parse the string input into the output token Vector. The resulted
      tokens will be added to the Vector (the Vector will not be reset)
      */
    Term_Cmd_Tokenizer(
		bool & no, 
		Vector<Term_Cmd_Token>& output, 
		const String& input);

    /** Destructor */
    ~Term_Cmd_Tokenizer();

    /** Investigate the parsing result */
    bool IsOk() const { return m_Ok; }

    /** Get the error location, if any */
    size_t GetErrorPos() const { return m_ErrorPos; }

    /** Get the error message, if any */
    const String& GetErrorMsg() const { return m_ErrorMsg; }

private:
    const char	*m_Separators,
		*m_Quotes;
    Vector<Term_Cmd_Token>& m_Token;
    char    *m_Input;
    
    bool    m_Ok;
    size_t  m_ErrorPos;
    String   m_ErrorMsg;
    bool    m_No;


    void Parse();

    bool GetNextToken(Term_Cmd_Token& token, char *& end_ptr);
    bool GetNextQuotedToken(Term_Cmd_Token&, char *& end_ptr);

    /** Unsupported assignment operator */
    const Term_Cmd_Tokenizer& operator=(const Term_Cmd_Tokenizer&) {
	assert(false);
	return *this;
    }
};


///////////////////////////////////////////////////////////////////////////////
/**
  Scenario parser (given a complete command String, parse the possible execution
  path)
  */
struct Term_Cmd_Scenario {

    /** 
       Build scenario with specified command and cursor positon, terminal mode,
       current config location (pwd), and privilege of the connected user
      */
    Term_Cmd_Scenario(const String& command, int cursor_pos, Term_Session*);

    /**
      Status of current command
      */
    enum Status {
	CMD_AWAIT_COMMAND,  // entry has been specified, awaiting command
	CMD_AWAIT_CMD_ARG,  // entry and command has been specified, awaiting cmd argument
	CMD_AWAIT_CFG_ARG,  // awaiting arguments to configuration leaf entry
	CMD_ERROR,	    // error (call GetErrorPos to get the error location)
	CMD_COMPLETE,	    // complete (call GetNextToken to examine optional arguments)
    };

    /** Command mode */
    enum CommandMode {
	CMD_CFG_SET,	    // command to change config value. The command syntax should be : <entry> <value>
	CMD_EXEC,	    // execute Term_Cmd. The command syntax should be: <entry> <command> <arguments>
    };

    /** Get current scenario status */
    Status GetStatus() const { 
	return m_Status; 
    }

    /** Get number of token in the command */
    size_t GetTokenCount() const { 
	return m_TokenCount; 
    }

    /** Get entry to which this command will affect */
    Cfg_Tree_Entry *GetEntry() const { 
	return m_Entry; 
    }

    /** Determine whether "no" prefix is set.
     */
    bool IsNo() const {
	return m_No;
    }

    /** Get the command part */
    Term_Cmd& GetCommand() { 
	return m_Command; 
    }

    /** Get the command part */
    const Term_Cmd& GetCommand() const { 
	return m_Command; 
    }

    /** Get the error location */
    int	GetErrorPos() const { 
	return m_ErrorPos; 
    }

    /** Get the error message */
    const String& GetErrorMsg() const { 
	return m_ErrorMsg; 
    }

    /**
       Structure to represent next available word choice 
      */
    struct NextWord {
	String m_Word;	// next word
	String m_Desc;	// description

	NextWord() {};
	NextWord(const String& w, const String& d) : m_Word(w), m_Desc(d) {};
	NextWord(const NextWord& w) : m_Word(w.m_Word), m_Desc(w.m_Desc) {};
	const NextWord& operator=(const NextWord& w)
	{
	    m_Word = w.m_Word;
	    m_Desc = w.m_Desc;
	}
    };

    /** Get next available choice */
    List<NextWord>& GetNextWords() { return m_NextWords; }

    /** 
      If only one next word match, this function will return characters
      required to complete that word. 
    */
    const String&   GetWordCompletion() const { return m_WordCompletion; }

    /** Get the command mode */
    CommandMode GetCommandMode() const { return m_CommandMode; }

    /** If the command is CMD_CFG_SET, get the value supplied by the user */
    const CVariant& GetValue() const { return m_Value; };

private:
    Status	    m_Status;
    Cfg_Tree_Entry*	    m_Entry;
    Term_Cmd	    m_Command;
    int		    m_ErrorPos;
    List<NextWord>  m_NextWords;
    String	    m_ErrorMsg;
    String	    m_WordCompletion;
    CommandMode	    m_CommandMode;
    CVariant	    m_Value;
    size_t	    m_TokenCount;
    Term_Session*	    m_Term;
    bool	    m_No;

    enum ParseMode {
	ParseNextAvailable,
	ParseCompletion,
	ParseAcceptance,
    };

    ParseMode	    m_ParseMode;

    /** Parse the command */
    void ParseCommand(const String& cmd, int cursor_pos);

    /** Parsing mode is to find a word completion */
    void FindCompletion(const Vector<Term_Cmd_Token>& token);

    /** Parsing mode is to find next available commands */
    void FindNextAvailable(Vector<Term_Cmd_Token>& token);

    /** Parsing mode is to find acceptance of a command */
    void FindAcceptance(const Vector<Term_Cmd_Token>& token);

    /** Get/parse the entry part from the command */
    void ParseEntryPart(const Vector<Term_Cmd_Token>& token, size_t &token_index);

    /** Get/parse the command part from the command */
    void ParseCmdPart(const Vector<Term_Cmd_Token>& token, size_t &token_index);

    /** Get/parse the command arguments part from the command */
    void ParseArgPart(const Vector<Term_Cmd_Token>& token, size_t &token_index);
    
    /** Get/parse the value argument in the case of CFG_SET */
    void ParseEntryValuePart(const Vector<Term_Cmd_Token>& token, size_t &token_index);

    /** Add List of entries to available words */
    void AddAvailableEntries(const List<Cfg_Tree_Entry*>& entries);

    /** Add List of commands to available words */
    void AddAvailableCommands(const List<Term_Cmd*>& cmds);

    /** Add all available cfg entries below the specified cfg entry
        which name starts with the specified prefix */
    void AddAvailableEntries(Cfg_Tree_Entry *g, const char *prefix=NULL);

    /** Add all available commands for the specified cfg entry 
        which name starts with the specified prefix */
    void AddAvailableCommands(Cfg_Tree_Entry *e, const char *prefix=NULL);

    /** Add List of terminal commands to available words */
    void AddAvailableTermCommands();

    /** Add all available values */
    void AddAvailableValues(const CVariantValidator& validator);

    /** Find all available cfg entries below the specified cfg entry
        which name starts with the specified prefix, store in <code>entries</code>
        parameter */
    void FindAvailableEntries(List<Cfg_Tree_Entry*>& entries, Cfg_Tree_Entry *e, const char *prefix=NULL);

    /** Find all available terminal commands for the current entry 
        which name starts with the specified prefix, store in <code>cmds</code>
        parameter */
    void FindAvailableTermCommands(List<Term_Cmd*>&, const char *prefix=NULL);

    /** Find all available commands for the specified cfg entry 
        which name starts with the specified prefix, store in <code>cmds</code>
        parameter */
    void FindAvailableCommands(List<Term_Cmd*> &, const List<Term_Cmd*>&, const char *prefix=NULL);

    /** Find all available values that can be applied to a variant */
    void FindAvailableValues(List<NextWord>& values, const CVariantValidator& validator);

    /** Compute similar characters in the next words. This will be used to
        complete current command */
    void ComputeCompletion(const char* typed_str);

    /** Add one next available word/description.
        This will put the word sorted in the List.
     */
    void AddNextWord (const String & word, const String & desc);
};


///////////////////////////////////////////////////////////////////////////////
/**
  Class to export configuration entry to sequence of terminal commands
  */
class Term_Cfg_Writer {
public:
    /**
      Export the configuration entry, started with start_entry
      */
    Term_Cfg_Writer(const Cfg_Tree_Entry* start_entry, bool use_comment=false);

    /**
      Get the string containing the commands required to rebuild the configs
      */
    const String& GetResult() const { return m_Result; }

private:
    String m_Result;
    const String m_IndentSize;
    bool m_UsingComment;

    /* Unsupported copy constructor  */
    Term_Cfg_Writer(const Term_Cfg_Writer &) {
	assert(false);
    }

    /* Unsupported assignment operator */
    const Term_Cfg_Writer& operator = (const Term_Cfg_Writer&) {
	assert(false); //unsupported
	return *this;
    }

    /* Function to export entry */
    void ExportEntry(const String& indent, const Cfg_Tree_Entry* e);
};



#endif
