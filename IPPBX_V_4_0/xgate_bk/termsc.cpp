/* $Header: /xGate/termsc.cpp 15    6/12/04 12:04 Irwan $
 *   
 * CTENGINE
 * (C) 2001 Switchlab, Ltd
 *
 * Terminal command parser/config. writer/scenario builder.
 *
 * $Log: /xGate/termsc.cpp $
 * 
 * 15    6/12/04 12:04 Irwan
 * Remove '//' style coments from supported commands
 * 
 * 14    8/02/01 7:57p Bennylp
 * Don't print header if it is not the root config.
 * 
 * 13    7/26/01 11:55a Bennylp
 * Added header on configuration output.
 * 
 * 12    6/24/01 7:44p Bennylp
 * Bug fixes.
 * 
 * 11    6/21/01 6:58p Bennylp
 * Support dynamic arguments.
 * 
 * 10    10/05/01 11:00 Junanto
 * Fixed an out-of-bound read
 * 
 * 9     5/01/01 3:28a Bennylp
 * Fixed optional arguments display.
 * 
 * 8     4/23/01 10:47p Bennylp
 * Fixed std namespace conflicts.
 * 
 * 7     4/13/01 8:34p Bennylp
 * Fix bug in command searching.
 * 
 * 6     4/12/01 8:08p Bennylp
 * Integration.
 *
 */
#include "stdafx.h"
#include "termsc.h"
#include "log.h"
#include "kernel.h"
#include "cfg.h"
#include "term.h"

using namespace std;
#define TERM "term"
#define LIST_SEPARATOR "----"
#define __linux__

#define stricmp strcasecmp
#define strnicmp strncasecmp
#define strcmpi strcasecmp


///////////////////////////////////////////////////////////////////////////////

Term_Cmd_Tokenizer::Term_Cmd_Tokenizer(bool & no, vector<Term_Cmd_Token>& output, const String& input)
: m_Separators((const char*)" \r\n\t"), 
  m_Quotes((const char*)"\"'`"), 
  m_Token(output), 
  m_Input(NULL),
  m_Ok(true), 
  m_ErrorPos(0),
  m_No(false)
{
    // copy string input.
    m_Input = new char[input.size()+1];
    memcpy(m_Input, input.c_str(), input.size());
    m_Input[input.size()]='\0';

    Parse();

    no = m_No;
}

Term_Cmd_Tokenizer::~Term_Cmd_Tokenizer()
{
    delete [] m_Input;
}

void Term_Cmd_Tokenizer::Parse()
{
    char *end_ptr=m_Input;
    bool first = true;
 
    while(*end_ptr && IsOk()) {
	Term_Cmd_Token token(0, NULL, 0);

	// ignore first whitespaces
	for ( ; *end_ptr && strchr(m_Separators, *end_ptr); end_ptr++)
	    ;

	if (*end_ptr == 0)
	    break;

	bool parse_result;
	if (strchr(m_Quotes, *end_ptr))
	    parse_result=GetNextQuotedToken(token, end_ptr);
	else
	    parse_result=GetNextToken(token, end_ptr);

	//if (parse_result )
	if (parse_result) {
	    if (first && !strcasecmp(token.token, "no"))
		m_No = true;
	    else
		m_Token.push_back(token);
	    first = false;
	} else
	    break;
    }
}

bool Term_Cmd_Tokenizer::GetNextToken(Term_Cmd_Token& token, char *& end_ptr)
{
    bool comment_started=false;

    for( char *start=end_ptr; 
	 *end_ptr && !strchr(m_Separators, *end_ptr);
	 ++end_ptr) {

	//if (*end_ptr=='/') {
	if (*end_ptr=='\xff') {
	    if (comment_started) {
		*(end_ptr-1)='\0';
		*end_ptr--='\0';
		return false;
	    } else {
		comment_started=true;
	    }
	} else if (comment_started)
	    comment_started=false;
    }

    token.token=start;
    token.flag=0;
    token.pos = start-m_Input;

    if (*end_ptr)
	*end_ptr++='\0';

    return m_Ok = true;
}

bool Term_Cmd_Tokenizer::GetNextQuotedToken(Term_Cmd_Token& token, char *& end_ptr)
{
    assert(strchr(m_Quotes, *end_ptr));
    
    bool comment_started=false;
    char quote_chr=*end_ptr;
    ++end_ptr;

    for( char *start=end_ptr;
	 *end_ptr && *end_ptr!=quote_chr;
	 ++end_ptr) {
/*
	if (*end_ptr=='/') {
	    if (comment_started) {
		*(end_ptr-1)='\0';
		*end_ptr--='\0';
		return false;
	    } else {
		comment_started=true;
	    }
	} else if (comment_started)
	    comment_started=false;
*/
    }

    if (*end_ptr != quote_chr) {
	m_ErrorPos=end_ptr-m_Input;
	m_ErrorMsg="end of quote expected";
	m_Ok = false;
	return false;
    }

    token.token=start;
    token.pos = start-m_Input;
    switch(*end_ptr) {
    case '\'':
	token.flag = Term_Cmd_Token::InSingleQuote;
	break;
    case '"':
	token.flag = Term_Cmd_Token::InDoubleQuote;
	break;
    case '`':
	token.flag = Term_Cmd_Token::InBackQuote;
	break;
    default:
	assert(false);  // unknown quote
	token.flag = 0;
	break;
    }

    if (*end_ptr) {
	*end_ptr++='\0';
    }

    return m_Ok = true;
}

///////////////////////////////////////////////////////////////////////////////

//
// Create a command scenario builder, and parse the input.
//
Term_Cmd_Scenario::Term_Cmd_Scenario( const String & command, int cursor_pos, 
				      Term_Session* term)
: m_Status(CMD_ERROR), 
  m_Entry(NULL), 
  m_Command( NULL, 0, "_", "dummy", PrivilegeLevelAdmin, TermModeLogin), 
  m_ErrorPos(0),
  m_CommandMode(CMD_EXEC),
  m_TokenCount(0),
  m_Term(term),
  m_No(false)
{
    ParseCommand(command, cursor_pos);
}


//
// Parse commands, and depending on the last keystroke pressed by user,
// and the cursor position (in the case when user pressed '?' or tab, 
// determine the next available word(s).
//
// If the user pressed ENTER, then the command (if found) will be placed
// in m_Command member var.
//
void Term_Cmd_Scenario::ParseCommand(const String& s, int cursor_pos)
{
    String cmd(s.c_str(), s.c_str()+(cursor_pos+2-1));
    
    //
    // parse the command into List of tokens
    //
    vector<Term_Cmd_Token> token;
    Term_Cmd_Tokenizer tokenizer(m_No, token, cmd);
    if (!tokenizer.IsOk()) {
	m_ErrorPos=tokenizer.GetErrorPos();
	m_ErrorMsg=tokenizer.GetErrorMsg();
	m_Status=CMD_ERROR;
	return;
    }

    m_TokenCount = token.size();

    //
    // Get last keystroke pressed by user.
    //
    char last_char= cursor_pos>=0 ? s[cursor_pos] : ' ';
    Keystroke::Cmd key_cmd=m_Term->GetCurrentKeystroke().m_Type==Keystroke::TypeCmd?
			   m_Term->GetCurrentKeystroke().GetCmd() : Keystroke::BELL;

    //
    // Determine parsing mode based on cursor position and last keystroke
    //
    if (last_char!=' ' && (key_cmd==Keystroke::QM || key_cmd==Keystroke::TAB))
	m_ParseMode=ParseCompletion;
    else if (last_char==' ' && (key_cmd==Keystroke::QM || key_cmd==Keystroke::TAB))
	m_ParseMode=ParseNextAvailable;
    else if (key_cmd==Keystroke::CR)
	m_ParseMode=ParseAcceptance;
    else {
	assert(false);
	return;
    }

    //
    // Special case when user didn't type anything but hit '?' or TAB immediately;
    // in this case, just print all available commands.
    //
    if (token.size()==0 || token[0].token==NULL) {
	
	m_Entry = m_Term->GetCurrentRootEntry();

	if (m_ParseMode==ParseNextAvailable) {
	    AddAvailableEntries(m_Term->GetCurrentRootEntry());
	    //AddNextWord( LIST_SEPARATOR , "");
	    AddAvailableTermCommands();
	    AddAvailableCommands(m_Entry);
	    if(m_NextWords.size()==1) { // by MIM
		m_WordCompletion = m_NextWords.front().m_Word.c_str();
	    }
	}

	m_Status=CMD_AWAIT_COMMAND;
	return;
    }

    //
    // Do the parsing
    //
    switch(m_ParseMode) {
    case ParseCompletion:
	FindCompletion(token);
	break;
    case ParseNextAvailable:
	FindNextAvailable(token);
	break;
    case ParseAcceptance:
	FindAcceptance(token);
	break;
    };
}


//
// This is the top-most function which will be called when the user hit
// ENTER key. In this mode, we'll find out whether the command can be
// accepted, and fill out m_Command in the way.
//
void Term_Cmd_Scenario::FindAcceptance(const vector<Term_Cmd_Token>& token)
{
    //
    // Get the entry/directory where the user is currently active.
    //
    size_t token_index=0;
    ParseEntryPart(token, token_index);
    if (m_Status==CMD_ERROR)
	return;

    /*
    if (m_NextWords.size()>1) {
	m_Status=CMD_ERROR;
	return;
    }

    if (token_index==token.size()) {
	m_ErrorMsg = "Command not found";
	m_ErrorPos = token[token_index-1].pos+
		     strlen(token[token_index-1].token)+1;
	return;
    }
    */

    //
    // find matched command(s)
    //
    ParseCmdPart(token, token_index);

    // if this is not a valid command, then try this as an argument
    /*
    if (m_Status==CMD_ERROR) {
	if (m_Term->GetTermMode() >= TermModeConfig && token_index==token.size()-1) {
	    ParseEntryValuePart(token, token_index);
	}
	return;
    } else if (m_Status==CMD_AWAIT_COMMAND && 
	       m_Term->GetTermMode()>=TermModeConfig &&
	       token_index==token.size()-1) {
	ParseEntryValuePart(token, token_index);
	return;
    }
    */

    if (m_Status == CMD_ERROR)
	return;

    //
    // If all the string tokens has been parsed, check whether the command
    // doesn't require arguments.
    //
    if (token_index==token.size()) {
	for (int i=0; i<m_Command.GetArgCount(); i++) {
	    if (m_Command.StartArgument(i)==false)
		continue;
	    if (m_Command.GetArg(i)->GetStatus()!=TermArgOptional)
		return;
	}
	m_Status = CMD_COMPLETE;
	m_CommandMode = CMD_EXEC;
	return;
    }

    //
    // Parse arguments, and fill out the argument value of the m_Command.
    //
    ParseArgPart(token, token_index);

}


//
// This is the top-most function to find the next available commands/words
// for the current command specified by user. 
//
void Term_Cmd_Scenario::FindNextAvailable(vector<Term_Cmd_Token>& token)
{
    if (token.empty())
	token.push_back(Term_Cmd_Token(0, NULL));
    else
	token.push_back(Term_Cmd_Token(token.back().pos+strlen(token.back().token), NULL));

    FindCompletion(token);

    //
    // avoid completion by adding empty choice
    //
    if (m_NextWords.size()==1) AddNextWord( String(), String());
}


//
// This is the top-most function to complete the current word typed by user. 
//
void Term_Cmd_Scenario::FindCompletion(const vector<Term_Cmd_Token>& token)
{
    size_t token_index=0;

    //
    // Get the current entry/dir where the user is currently active.
    //
    ParseEntryPart(token, token_index);
    if (m_Status==CMD_ERROR) return;

    if (token_index==token.size()) {
	if (token[token_index-1].token==NULL) {
	    //AddNextWord( LIST_SEPARATOR , "");
	    /*
	    if (m_Term->GetTermMode()>=TermModeConfig &&
		(m_Entry->GetType()==Cfg_Tree_Entry::CfgTypeGroupBase ||
		 m_Entry->GetType()==Cfg_Tree_Entry::CfgTypeGroup ||
		 m_Entry->GetType()==Cfg_Tree_Entry::CfgTypeTable)) {

		AddNextWord(  "<CR>" , "Make as current config directory");
	    }
	    */
	    AddAvailableTermCommands();
	    AddAvailableCommands(m_Entry, NULL);
	}
	ComputeCompletion(token.back().token);
	return;
    }

    // find matched command(s)
    ParseCmdPart(token, token_index);

    // if this is not availd command, then try this as an argument
    if (m_Term->GetTermMode()>=TermModeConfig &&
	(m_Status==CMD_ERROR || m_Entry->GetType()==Cfg_Tree_Entry::CfgTypeLeaf)) {
	if (token_index>=token.size()-1) {
	    token_index=token.size()-1;
	    ParseEntryValuePart(token, token_index);
	}
	ComputeCompletion(token.back().token);
	return;
    }

    if (token_index==token.size()) return;

    // parse arguments
    ParseArgPart(token, token_index);
    ComputeCompletion(token.back().token);
}

void Term_Cmd_Scenario::ParseEntryPart(const vector<Term_Cmd_Token>& token, size_t &token_index)
{
    m_Status=CMD_AWAIT_COMMAND;
    m_Entry = m_Term->GetCurrentRootEntry();
}


void Term_Cmd_Scenario::ParseCmdPart(const vector<Term_Cmd_Token>& token, size_t &token_index)
{
    List<Term_Cmd*> cmd_entries;

    /** jnt: check all commands to avoid ambiguity
    for (int step=0; step<2; ++step) {

	cmd_entries.clear();

	if (step==0) {
	    // first step: find available commands of the configuration entry
	    assert(m_Entry);
	    const List<Term_Cmd*>& tgt_cmd_list=m_Entry->GetCommandList();
	    FindAvailableCommands(cmd_entries, tgt_cmd_list, token[token_index].token);
	} else {
	    // second step: find available terminal commands
	    FindAvailableTermCommands(cmd_entries, token[token_index].token);
	}
	
	if (cmd_entries.empty()) {

	    if (step==0) continue;

	    m_Status = CMD_ERROR;
	    m_ErrorMsg = "command not found";
	    m_ErrorPos = token[token_index].pos+
			 (token[token_index].token ? 0 : 1);
	    
	} else if (cmd_entries.size()>1) {
	    m_ErrorMsg = "ambigous command";
	    m_ErrorPos = token[token_index].pos;

	    if (token_index!=token.size()-1 || m_ParseMode==ParseAcceptance) {
		m_Status = CMD_ERROR;
	    } else {
		AddAvailableCommands(cmd_entries);
		++token_index;
	    }
	    return;
	} else {
	    m_WordCompletion = cmd_entries.front()->GetName().c_str()+
			       (token[token_index].token ? 
				strlen(token[token_index].token) : 0);

	    m_Status = CMD_AWAIT_CMD_ARG;
	    //m_Command = *cmd_entries.front();
	    cmd_entries.front()->CopyTo (m_Command);
	    ++token_index;

	    if (token_index==token.size()) {
		AddAvailableCommands(cmd_entries);
	    } else if (token[token_index].token==NULL) {
		Term_Cmd * the_command=cmd_entries.front();
		if (the_command->GetArgCount()==0) {
		    m_Status = CMD_COMPLETE;
		    m_CommandMode = CMD_EXEC;
		}
	    }
	    return;
	}
    }
    jnt: end **/

    // first step: find available commands of the configuration entry
    assert(m_Entry);
    const List<Term_Cmd*>& tgt_cmd_list=m_Entry->GetCommandList();
    FindAvailableCommands(cmd_entries, tgt_cmd_list, token[token_index].token);

    // second step: find available terminal commands
    FindAvailableTermCommands(cmd_entries, token[token_index].token);
    
    if (cmd_entries.empty()) {

	m_Status = CMD_ERROR;
	m_ErrorMsg = "command not found";
	m_ErrorPos = token[token_index].pos+
		     (token[token_index].token ? 0 : 1);
	
    } else if (cmd_entries.size()>1) {
	m_ErrorMsg = "ambigous command";
	m_ErrorPos = token[token_index].pos;

	if (token_index!=token.size()-1 || m_ParseMode==ParseAcceptance) {
	    m_Status = CMD_ERROR;
	} else {
	    AddAvailableCommands(cmd_entries);
	    ++token_index;
	}
    } else {
	m_WordCompletion = cmd_entries.front()->GetName().c_str()+
			   (token[token_index].token ? 
			    strlen(token[token_index].token) : 0);

	m_Status = CMD_AWAIT_CMD_ARG;
	//m_Command = *cmd_entries.front();
	cmd_entries.front()->CopyTo (m_Command);
	++token_index;

	if (token_index==token.size()) {
	    AddAvailableCommands(cmd_entries);
	} else if (token[token_index].token==NULL) {
	    Term_Cmd * the_command=cmd_entries.front();
	    if (the_command->GetArgCount()==0) {
		m_Status = CMD_COMPLETE;
		m_CommandMode = CMD_EXEC;
	    }
	}
    }
}


void Term_Cmd_Scenario::ParseArgPart(const vector<Term_Cmd_Token>& token, size_t &token_index)
{
    for (int arg_idx=0; 
	 arg_idx<m_Command.GetArgCount() && token_index<token.size() && token[token_index].token!=NULL; 
	 ++arg_idx) 
    {
	if (m_Command.StartArgument(arg_idx) == false) {
	    m_Status = CMD_ERROR;
	    m_ErrorMsg = "invalid argument value";
	    m_ErrorPos = token[token_index].pos;
	    break;
	}

	Term_Cmd_Arg* arg=m_Command.GetArg(arg_idx);
	assert(arg);
	
	CVariant result;
	const char *buf=token[token_index].token;
	int buf_len=strlen(token[token_index].token);

	if (arg->Suggest(buf, buf_len, result)) {
	    ++token_index;
	    arg->SetValue(result);

	    // Only add if this is the last arguments
	    if (m_ParseMode==ParseCompletion && token_index==token.size())
	    	AddNextWord(result.ToString(), "");
	} 
	else 
	if (arg->GetStatus()!=TermArgRequired && arg_idx!=m_Command.GetArgCount()-1) {
	    if (m_ParseMode==ParseAcceptance) {
		m_Status = CMD_ERROR;
		m_ErrorMsg = "invalid argument value";
		m_ErrorPos = token[token_index].pos;
	    } else {
		++token_index;
	    }
	} else {
	    m_Status = CMD_ERROR;
	    m_ErrorMsg = "invalid argument value";
	    m_ErrorPos = token[token_index].pos;
	    break;
	}
    }
    
    if (m_Status==CMD_ERROR)
	return;

    if (token_index==token.size() || token[token_index].token==NULL) {
	if (arg_idx < m_Command.GetArgCount() && m_Command.StartArgument(arg_idx)==true) {

	    if (m_ParseMode==ParseNextAvailable)
		AddAvailableValues(m_Command.GetArg(arg_idx)->GetValidator());

	    // scan next arguments to find argument that are not optional
	    for ( ; arg_idx<m_Command.GetArgCount(); ++arg_idx) {
		if (m_Command.StartArgument(arg_idx) == false)
		    continue;
		if (m_Command.GetArg(arg_idx)->GetStatus()!=TermArgOptional) {
		    break;
		}
	    }


	    if (arg_idx==m_Command.GetArgCount()) {
		if (m_ParseMode==ParseNextAvailable)
		    AddNextWord("<CR>", "");
		else if (m_ParseMode==ParseAcceptance)
		    m_Status = CMD_COMPLETE;
	    }

	} else if (m_ParseMode==ParseAcceptance) {
	    m_Status = CMD_COMPLETE;
	    m_CommandMode = CMD_EXEC;
	}
    } else {
	m_Status = CMD_ERROR;
	m_ErrorMsg = "invalid identifier";
	m_ErrorPos = token[token_index].pos;
    }
}


void Term_Cmd_Scenario::ParseEntryValuePart(const vector<Term_Cmd_Token>& token, size_t &token_index)
{
    if (token[token_index].token==NULL) {
	if (m_Entry->GetType()==Cfg_Tree_Entry::CfgTypeLeaf) {
	    Cfg_Tree_Leaf *leaf=static_cast<Cfg_Tree_Leaf*>(m_Entry);
	    const CVariant& value=leaf->GetValue();
	    CVariantValidator validator(value, "Value");
	    
	    AddAvailableValues(validator);
	    m_Status = CMD_AWAIT_CFG_ARG;
	}
    } else {
	if (m_Entry->GetType()==Cfg_Tree_Entry::CfgTypeLeaf) {
	    Cfg_Tree_Leaf *leaf=static_cast<Cfg_Tree_Leaf*>(m_Entry);
	    const CVariant& value=leaf->GetValue();
	    
	    if (CVariant::Factory(m_Value, value.Type(), token[token_index].token))
		return;
	    
	    m_Status = CMD_COMPLETE;
	    m_CommandMode = CMD_CFG_SET;
	} 
    }
}


void Term_Cmd_Scenario::AddAvailableEntries(const List<Cfg_Tree_Entry*>& entries)
{
    /*
    for(List<Cfg_Tree_Entry*>::const_iterator it=entries.begin(); it!=entries.end(); it++)
	AddNextWord( (*it)->GetName(), (*it)->GetDesc());
    */
}


void Term_Cmd_Scenario::AddAvailableCommands(const List<Term_Cmd*>& cmds)
{
    for(List<Term_Cmd*>::const_iterator it=cmds.begin(); it!=cmds.end(); it++)
    {
	if(!(*it)->IsNoValid() && m_No) // by MIM
	    continue;
	AddNextWord((*it)->GetName(), (*it)->GetDesc());
}
}


void Term_Cmd_Scenario::AddAvailableEntries(Cfg_Tree_Entry *e, const char *prefix)
{
    /*
    List<Cfg_Tree_Entry*> entries;
    FindAvailableEntries(entries, e, prefix);
    AddAvailableEntries(entries);
    */
}


void Term_Cmd_Scenario::AddAvailableCommands(Cfg_Tree_Entry *e, const char *prefix)
{
    List<Term_Cmd*> cmd_list;
    const List<Term_Cmd*>& tgt_cmd_list = e->GetCommandList();

    FindAvailableCommands(cmd_list, tgt_cmd_list, prefix);
    AddAvailableCommands(cmd_list);
}

void Term_Cmd_Scenario::AddAvailableTermCommands()
{
    List<Term_Cmd*> cmds;
    FindAvailableTermCommands(cmds, NULL);

    for (List<Term_Cmd*>::const_iterator it=cmds.begin(); it!=cmds.end(); it++) {
	if(!(*it)->IsNoValid() && m_No) // by MIM
	    continue;
	AddNextWord ( (*it)->GetName(), (*it)->GetDesc());
    }
}

void Term_Cmd_Scenario::FindAvailableTermCommands(List<Term_Cmd*> &cmds, const char *prefix)
{
    const List<Term_Cmd*>& tgt_cmd_list = Config_Controller::Instance()->GetTermCommand();

    size_t prefixlen=prefix? strlen(prefix) : 0;

    for (List<Term_Cmd*>::const_iterator it=tgt_cmd_list.begin(); it!=tgt_cmd_list.end(); it++) {
	if ((*it)->GetPrivilegeLevel() <= m_Term->GetPrivilegeLevel() &&
	    (*it)->HasMode(m_Term->GetTermMode())) {

	    //if (!strnicmp((*it)->GetName().c_str(), prefix, prefixlen)) {
	    if (!strncasecmp((*it)->GetName().c_str(), prefix, prefixlen)) {	//linux porting
		
		if (m_Entry==Kernel::Instance()->RootConfig() || (*it)->HasMode((Term_Mode)TermModeEntry))
		    cmds.push_back(*it);
		else if (m_Entry==m_Term->GetCurrentRootEntry() && (*it)->HasMode((Term_Mode)TermModeSingle))
		    cmds.push_back(*it);
	    }
	} else if (m_Term->GetTermMode() >= TermModeConfig &&
		   m_Entry->GetType()==Cfg_Tree_Entry::CfgTypeTable && 
		   (*it)->HasMode((Term_Mode)TermTableCmd)) {
	    //if (!strnicmp((*it)->GetName().c_str(), prefix, prefixlen))
	    if (!strncasecmp((*it)->GetName().c_str(), prefix, prefixlen))	//linux porting
		cmds.push_back(*it);
	}
    }
}

void Term_Cmd_Scenario::FindAvailableEntries(List<Cfg_Tree_Entry*>& entries, Cfg_Tree_Entry *e, const char *prefix)
{
    int prefix_len=prefix? strlen(prefix) : 0;

    if (e->GetType()==Cfg_Tree_Entry::CfgTypeGroupBase ||
	e->GetType()==Cfg_Tree_Entry::CfgTypeGroup || 
	e->GetType()==Cfg_Tree_Entry::CfgTypeTable) {

	Cfg_Tree_Group_Base *g=static_cast<Cfg_Tree_Group_Base*>(e);
	for (Cfg_Tree_Iterator it=g->GetFirstChild(); it!=g->GetEndChild(); it++) {
	    if (prefix_len==0) {
		entries.push_back(*it);
	    } else {
		//if ( !strnicmp((*it)->GetName().c_str(), prefix, prefix_len) )
		if ( !strncasecmp((*it)->GetName().c_str(), prefix, prefix_len) )	//linux porting
		    entries.push_back(*it);
	    }
	}

    }
}

void Term_Cmd_Scenario::FindAvailableCommands(List<Term_Cmd*>& cmd_list, 
					     const List<Term_Cmd*>& tgt_cmd_list, 
					     const char *prefix)
{
    int prefix_len=prefix ? strlen(prefix) : 0;

    for (List<Term_Cmd*>::const_iterator it=tgt_cmd_list.begin(); it!=tgt_cmd_list.end(); it++) {
	if ((*it)->GetPrivilegeLevel() > m_Term->GetPrivilegeLevel() ||
	    !(*it)->HasMode(m_Term->GetTermMode()))
	    continue;

	if (prefix_len==0)
	    cmd_list.push_back(*it);
	//else if (!strnicmp((*it)->GetName().c_str(), prefix, prefix_len))
	else if (!strncasecmp((*it)->GetName().c_str(), prefix, prefix_len))	//linux porting
	    cmd_list.push_back(*it);
    }
}


void Term_Cmd_Scenario::AddAvailableValues(const CVariantValidator& validator)
{
    List<NextWord> values;
    FindAvailableValues(values, validator);

    for(List<NextWord>::iterator it=values.begin(); it!=values.end(); it++)
	AddNextWord( it->m_Word, it->m_Desc);
}


void Term_Cmd_Scenario::FindAvailableValues(List<NextWord>& values, const CVariantValidator& validator)
{
    List<v_string_pair> vals;

    validator.GetPrintableValues(vals);
    for(List<v_string_pair>::iterator it=vals.begin(); it!=vals.end(); it++)
	values.push_back(NextWord(it->first, it->second));
}

void Term_Cmd_Scenario::ComputeCompletion(const char* typed_str)
{
    if (m_NextWords.empty() || m_NextWords.size()!=1 || typed_str==NULL)
	return;

    m_WordCompletion = m_NextWords.front().m_Word.c_str()+strlen(typed_str);
}

void Term_Cmd_Scenario::AddNextWord (const String & word, const String & desc)
{
    List<NextWord>::iterator it;
    for (it = m_NextWords.begin(); it != m_NextWords.end(); ++it) {
	if (strcasecmp(word.c_str(), it->m_Word.c_str()) < 0) {
	    m_NextWords.insert( it, NextWord (word, desc));
	    return;
	}
    }
    m_NextWords.push_back ( NextWord (word, desc) );
}

///////////////////////////////////////////////////////////////////////////////

Term_Cfg_Writer::Term_Cfg_Writer(const Cfg_Tree_Entry* start_entry, bool use_comment)
: m_IndentSize(" "),
  m_UsingComment(use_comment)
{
    m_Result.reserve(8000);

    assert(start_entry); // start_entry can't be NULL
    if (!start_entry)
	return;

    // Generate header.
    if (start_entry==Kernel::Instance()->RootConfig()) {
	char temp[128];
	char hostname[64];
	char now_print[64];
	time_t now;

	if (gethostname(hostname, sizeof(hostname)))
	    strcpy(hostname, "unknown-host");
	time(&now);
	strcpy(now_print, ctime(&now));
	now_print[strlen(now_print)-1] = '\0';

	sprintf(temp, "  # xGate configuration on %s at %s\n", hostname, now_print);	//rkv
        //printf("# xGate configuration on %s at %s\n", hostname, now_print);

	m_Result += temp;
    }

    String indent((size_t)2, ' ');
    ExportEntry(indent, start_entry);
}

void Term_Cfg_Writer::ExportEntry(const String& indent, const Cfg_Tree_Entry* e)
{
    String output;
    output.reserve(2000);
    int loop=0;

    //printf("Beginning of Term_Cfg_Writer::ExportEntry\n");
    const List<Term_Cmd_Target *> & handler_list = e->GetShowHandlerList();
    List<Term_Cmd_Target *>::const_iterator it, end;
    end = handler_list.end();
//    printf("Term_Cfg_Writer::ExportEntry =%d\n",end);
    
    for (it = handler_list.begin(); it!=end; ++it) {
	//cout << "****** Inside handler_list loop ************" << endl;
	const Term_Cmd_Target * handler = *it;
	output.erase();
	if(!handler || !(*it)->IsValid()) { // by MIM
	    m_Result += "show is not valid within this context\n";
	} else {
	    if (handler->OnShowConfig( e, output, indent, m_IndentSize.size()) == OK) {
	    m_Result += output;
		}
    	}
//	usleep(50000);

	
#if 0
      loop++;
      printf(" loop=%d\n",loop);
//     m_Result += "************** For Testing **************\n";
      if ( loop == 1) break; //Linux compatable
#endif
	}
    //printf("End of Term_Cfg_Writer::ExportEntry\n");
}

