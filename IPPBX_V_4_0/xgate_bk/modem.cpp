/* $Header: /xGate/modem.cpp 4     6/22/04 9:48a Bennylp $
 *   
 * XGATE
 * (C) 2001 Switchlab, Ltd
 *
 * Modem framework
 *
 * $Log: /xGate/modem.cpp $
 * 
 * 4     6/22/04 9:48a Bennylp
 * Replace NULLs with some characters
 * 
 * 3     6/04/03 3:45p Bennylp
 * Preliminary modem support.
 * 
 * 2     4/24/03 5:59p Bennylp
 * Initial testing (ok)
 * 
 * 1     4/24/03 2:02p Bennylp
 * Initial implementation, compiles OK.
 *
 */
#include "stdafx.h"
#include "xgconfig.h"

#ifdef XGATE_HAS_MODEM

#include "modem.h"
#include "kernel.h"
#include "term.h"
#include "task.h"

#define THIS_MODULE	"modem"

Modem_Pool_Manager *Modem_Pool_Manager::instance_;
static int modem_tls_id_;

#ifdef XGATE_HAS_MTN
#  include "mtnmodem.h"
#endif

///////////////////////////////////////////////////////////////////////////////

#define MAX_MODEM_PER_TASK  4

struct Modem_TLS
{
    Modem_Channel *modem[MAX_MODEM_PER_TASK];
};


///////////////////////////////////////////////////////////////////////////////

enum
{
    Cmd_Modem_Pool,
};

Modem_Pool_Manager *Modem_Pool_Manager::Create()
{
    assert(instance_ == NULL);
    instance_ = new Modem_Pool_Manager;
    return instance_;
}

Modem_Pool_Manager *Modem_Pool_Manager::Instance()
{
    return instance_;
}

Modem_Pool_Manager::Modem_Pool_Manager()
: ServiceProvider("mdm-mgr", true, false, Normal_Priority, "Modem pool manager"),
  api_(NULL)
{
}

Modem_Pool_Manager::~Modem_Pool_Manager()
{
    for (unsigned i=0; i<factories_.size(); ++i) {
	delete factories_[i];
    }
}

bool Modem_Pool_Manager::Load()
{
    Cfg_Tree_Group *parent_tree;
    Cfg_Tree_Group *entry;
    Term_Cmd *cmd;

#ifdef XGATE_HAS_MTN
    factories_.push_back(new MTN_Modem_Factory);
#endif
    api_ = new Modem_API;
    if (api_->Init() != OK) {
	delete api_;
	api_ = NULL;
	return false;
    }

    parent_tree = Kernel::Instance()->RootConfig();
    entry = new Cfg_Tree_Group(Name(), "", parent_tree);
    entry->RegisterShowHandler(this);

    cfg_tree_ = entry;

    cmd = new Term_Cmd(this, Cmd_Modem_Pool, "modem-pool", "Configure modem-pool",
		       PrivilegeLevelAdmin, TermModeConfig, true);
    cmd->Add( new Term_Cmd_Arg("@0", CVariant(""), CVariantValidator("", "Modem pool name")) );
    RegisterTermCmd(cmd, parent_tree);
    return true;
}

bool Modem_Pool_Manager::CommitConfiguration()
{
    return true;
}

bool Modem_Pool_Manager::Unload()
{
    if (api_) {
	api_->Shutdown();
	delete api_;
	api_ = NULL;
    }
    return true;
}

bool Modem_Pool_Manager::AttachTask(CTask *task) 
{
    task->SetTLSValue(modem_tls_id_, 0);
    return true;
}

void Modem_Pool_Manager::DetachTask(CTask *task) 
{
    if (task->GetTLSValue(modem_tls_id_)) {
	Modem_TLS *tls = (Modem_TLS*) task->GetTLSValue(modem_tls_id_);
	for (unsigned i=0; i<MAX_MODEM_PER_TASK; ++i) {
	    if (tls->modem[i]) {
		tls->modem[i]->FreeDevice();
	    }
	    tls->modem[i] = NULL;
	}
	delete tls;
	task->SetTLSValue(modem_tls_id_, 0);
    }
}

Modem_Pool *Modem_Pool_Manager::FindPool(const IString &name)
{
    for (unsigned i=0; i<pool_.size(); ++i) {
	Modem_Pool *p = pool_[i];
	if (name == p->Name()) {
	    return p;
	}
    }
    return NULL;
}

bool Modem_Pool_Manager::OnShowStatus(String & output)
{
    return false;
}

void Modem_Pool_Manager::DoTermCmd(Term_Session *session, bool no, Term_Cmd *cmd)
{
    CVariant arg0;
    IString istr;

    switch (cmd->GetId()) {
    case Cmd_Modem_Pool:
	cmd->GetArg((size_t)0)->GetValue(arg0);
	istr = arg0.AsString();

	if (no) {

	} else {
	    Modem_Pool *pool;

	    if (!ACQUIRE_SYSTEM_LOCK()) {
		return;
	    }
	    
	    pool = FindPool(istr);
	    if (pool != NULL) {
		session->SetRoot( pool->GetConfigTree() );
	    } else {

		if (session->GetStream().IsInteractive()) {
		    RELEASE_SYSTEM_LOCK();

		    bool confirm;
		    confirm = session->PromptYesNo(false, "Create new modem pool %s? (Y/N) [N] ", 
						   istr.c_str());
		    if (!confirm) {
			return;
		    }

		    if (!ACQUIRE_SYSTEM_LOCK()) {
			return;
		    }
		}
		NEW_RETURN_(pool, Modem_Pool(istr));
		if (pool) {
		    pool_.push_back(pool);
		    session->SetRoot( pool->GetConfigTree() );
		}
	    }
	    RELEASE_SYSTEM_LOCK();
	}
	break;
    }
}

///////////////////////////////////////////////////////////////////////////////

Modem_Channel::Modem_Channel(const String &name, SwitchDevice* device)
: SupportSwitching(device), status_(STATUS_NULL)
{
}

Modem_Channel::~Modem_Channel()
{
    assert(status_ == STATUS_NULL);
}

void Modem_Channel::PoolInit(Modem_Pool *pool, Modem_Factory *f, 
			     List<Modem_Source>::iterator & source_data)
{
    pool_ = pool;
    factory_ = f;
    source_iterator_ = source_data;
}

void Modem_Channel::FreeDevice()
{
    if (status_ != STATUS_NULL) {
	Close();
    }
    pool_->FreeChannel(this);
}

Modem_Factory *Modem_Channel::GetFactory()
{
    return factory_;
}

List<Modem_Source>::iterator & Modem_Channel::GetSourceData()
{
    return source_iterator_;
}

bool Modem_Channel::Open(const Modem_Config *config)
{
    if (DeviceOpen(config)) {
	status_ = STATUS_IDLE;
	return true;
    } else {
	return false;
    }
}

bool Modem_Channel::Activate(Role role, const char *nb_to_dial)
{
    if (DeviceActivate(role, nb_to_dial)) {
	status_ = STATUS_NEGOTIATING;
	return true;
    } else {
	return false;
    }
}

bool Modem_Channel::Close()
{
    status_ = STATUS_NULL;
    return DeviceClose();
}

bool Modem_Channel::Send(const void *data, size_t size)
{
    return DeviceSend(data, size);
}

int  Modem_Channel::Receive(void *data, size_t max)
{
    return DeviceReceive(data, max);
}

Modem_Channel::Status Modem_Channel::GetStatus()
{
    status_ = DeviceGetStatus();
    return status_;
}


///////////////////////////////////////////////////////////////////////////////

Modem_Factory::~Modem_Factory()
{
}


///////////////////////////////////////////////////////////////////////////////

enum
{
    Cmd_Shutdown,
    Cmd_Data_Path,
    Cmd_Modulation_Restriction,
    Cmd_Data_Compression,
    Cmd_Framing,
    Cmd_Source,
};

#define NAME_DATA_PATH_HOST		"host"
#define NAME_DATA_PATH_ETHERNET		"ethernet"

#define NAME_MODULATION_UNSPECIFIED	"none"
#define NAME_MODULATION_V21		"v.21"
#define NAME_MODULATION_V22		"v.22"
#define NAME_MODULATION_V22BIS		"v.22-bis"
#define NAME_MODULATION_V23		"v.23"
#define NAME_MODULATION_V32		"v.32"
#define NAME_MODULATION_VFC		"v.fc"
#define NAME_MODULATION_V34		"v.34"
#define NAME_MODULATION_K56		"k.56"
#define NAME_MODULATION_V90		"v.90"

#define NAME_COMPRESSION_NONE		"none"
#define NAME_COMPRESSION_MNP5		"mnp5"
#define NAME_COMPRESSION_V42BIS		"v.42-bis"
#define NAME_COMPRESSION_ALL		"all"

#define NAME_FRAMING_NONE		"none"
#define NAME_FRAMING_SLIP		"slip"
#define NAME_FRAMING_PPP		"ppp"


class Modem_Source_Validator : public CVariantDynamicChoiceValidator {
public:
    /** Default constructor. */
    Modem_Source_Validator()
    {}

    /** Destructor. */
    virtual ~Modem_Source_Validator() {}

    /** Copy constructor */
    Modem_Source_Validator ( const Modem_Source_Validator & rhs)
	: CVariantDynamicChoiceValidator(rhs)
    {}

    /** Assignment operator */
    virtual const Modem_Source_Validator & operator =(const Modem_Source_Validator & rhs)
    {
	CVariantValidator::operator = (rhs);
	return *this;
    }

    /** Create a duplicate */
    virtual CVariantValidator *Clone() const
    {
	return new Modem_Source_Validator(*this);
    }

    /** Acquire mutex lock.
	This function will be called by the framework BEFORE calling
	GetChoiceList(). The lock will be kept until this class is finished
	processing all the choices in the choice list.
     */
    virtual void AcquireLock() const {
	ACQUIRE_SYSTEM_LOCK();
    }

    /** Release mutex lock.
	This function will be called by the framework AFTER calling
	GetChoiceList().
     */
    virtual void ReleaseLock() const {
	RELEASE_SYSTEM_LOCK();
    }

    /** Get list of choices available.
     */
    virtual const List<ValidatorChoice> & GetChoiceList() const {
	Modem_Pool_Manager *mgr = Modem_Pool_Manager::Instance();
	Vector<Modem_Factory*> *factories = mgr->GetFactories();
	
	choice_list_.clear();
	for (unsigned i=0; i<factories->size(); ++i) {
	    Modem_Factory *f = factories->operator[](i);
	    Vector<String> sources;
	    f->EnumSource(sources);

	    for (unsigned j=0; j<sources.size(); ++j) {
		String & source = sources[j];
		choice_list_.push_back(std::make_pair( CVariant(source.c_str()), String("") ));
	    }
	}
	return choice_list_;
    }

private:
    mutable List<ValidatorChoice> choice_list_;
};

Modem_Pool::Modem_Pool(const String &name)
: ServiceProvider(name, false, false, Normal_Priority, "Modem pool"),
  is_shutdown_(true)
{
    memset(&config_, 0, sizeof(config_));
    config_.data_path = MODEM_DATA_PATH_HOST;
    config_.restrict_modulation = MODEM_MODULATION_UNSPECIFIED;
    config_.compression = MODEM_COMPRESSION_ALL;
    config_.framing = MODEM_FRAMING_NONE;

    Cfg_Tree_Group *parent_tree;
    Cfg_Tree_Group *entry;
    Term_Cmd *cmd;

    parent_tree = Modem_Pool_Manager::Instance()->GetConfigTree();
    entry = new Cfg_Tree_Group(Name(), "", parent_tree);
    entry->RegisterShowHandler(this);
    Kernel::Instance()->RootConfig()->RegisterShowHandler( this );

    config_tree_ = entry;

    // Shutdown command.
    cmd = new Term_Cmd( this, Cmd_Shutdown, "shutdown", "Enable/disable pool", 
		        PrivilegeLevelAdmin, TermModeConfig, true);
    RegisterTermCmd(cmd, entry);

    // Data path command
    cmd = new Term_Cmd( this, Cmd_Data_Path, "data-path", "Configure data path",
		        PrivilegeLevelAdmin, TermModeConfig, false);
    CVariantChoiceValidator data_path_choice( NAME_DATA_PATH_HOST, 
					      "Data is sent/received to/from host");
    data_path_choice.Add( NAME_DATA_PATH_ETHERNET, 
			  "Data is sent/received to/from Ethernet interface");
    cmd->Add( new Term_Cmd_Arg("@0", CVariant("host"), data_path_choice) );
    RegisterTermCmd(cmd, entry);

    // Modulation restriction command.
    cmd = new Term_Cmd(this, Cmd_Modulation_Restriction, "modulation-restriction",
		       "Restrict modulation to use to this type",
		       PrivilegeLevelAdmin, TermModeConfig, false);
    CVariantChoiceValidator mod_choice( NAME_MODULATION_UNSPECIFIED, "No restriction");
    mod_choice.Add( NAME_MODULATION_V21,    "Restrict connection to V.21 only");
    mod_choice.Add( NAME_MODULATION_V22,    "Restrict connection to V.22 only");
    mod_choice.Add( NAME_MODULATION_V22BIS, "Restrict connection to V.21-bis only");
    mod_choice.Add( NAME_MODULATION_V23,    "Restrict connection to V.23 only");
    mod_choice.Add( NAME_MODULATION_V32,    "Restrict connection to V.32 only");
    mod_choice.Add( NAME_MODULATION_VFC,    "Restrict connection to V.FC only");
    mod_choice.Add( NAME_MODULATION_V34,    "Restrict connection to V.34 only");
    mod_choice.Add( NAME_MODULATION_K56,    "Restrict connection to K.56 only");
    mod_choice.Add( NAME_MODULATION_V90,    "Restrict connection to V.90 only");
    cmd->Add( new Term_Cmd_Arg("@0", CVariant("none"), mod_choice) );
    RegisterTermCmd(cmd, entry);

    // Data compression
    cmd = new Term_Cmd(this, Cmd_Data_Compression, "data-compression", "Modem data compression",
		       PrivilegeLevelAdmin, TermModeConfig, false);
    CVariantChoiceValidator comp_choice( NAME_COMPRESSION_NONE, "No compression");
    comp_choice.Add( NAME_COMPRESSION_V42BIS, "Offer V.42-bis compression");
    comp_choice.Add( NAME_COMPRESSION_MNP5, "Offer MNP5 compression");
    comp_choice.Add( NAME_COMPRESSION_ALL, "Offer all compression supported");
    cmd->Add( new Term_Cmd_Arg("@0", CVariant("v42bis"), comp_choice) );
    RegisterTermCmd(cmd, entry);

    // Framing
    cmd = new Term_Cmd(this, Cmd_Framing, "framing", "Framing type",
		       PrivilegeLevelAdmin, TermModeConfig, false);
    CVariantChoiceValidator framing_choice( NAME_FRAMING_NONE, "No framing");
    framing_choice.Add( NAME_FRAMING_SLIP, "Serial Line IP (SLIP) framing");
    framing_choice.Add( NAME_FRAMING_PPP, "PPP framing");
    cmd->Add( new Term_Cmd_Arg("@0", CVariant("none"), framing_choice) );
    RegisterTermCmd(cmd, entry);

    // Modem_Source
    cmd = new Term_Cmd(this, Cmd_Source, "source", "Specify source board",
		       PrivilegeLevelAdmin, TermModeConfig, true);
    Modem_Source_Validator src_choice;
    cmd->Add( new Term_Cmd_Arg("@0", CVariant(""), src_choice) );
    cmd->Add( new Term_Cmd_Arg("@1", CVariant(""), CVariantSingleValueValidator("channel-count", "")) );
    cmd->Add( new Term_Cmd_Arg("@2", CVariant(1), CVariantRangeValidator(1, 1024, "Channel count") ) );
    RegisterTermCmd(cmd, entry);
}

Modem_Pool::~Modem_Pool()
{
}


bool Modem_Pool::Load()
{
    return true;
}

bool Modem_Pool::Unload()
{
    return true;
}

List<Modem_Source>::iterator Modem_Pool::FindSource(const char *name)
{
    List<Modem_Source>::iterator it, end;
    end = sources_.end();
    for (it = sources_.begin(); it!=end; ++it) {
	if (!stricmp(it->name, name)) {
	    return it;
	}
    }
    return it;
}

bool Modem_Pool::CommitConfiguration()
{
    return true;
}

bool Modem_Pool::OnShowStatus(String & output)
{
    return false;
}

void Modem_Pool::DoTermCmd(Term_Session *session, bool no, Term_Cmd *cmd)
{
    CVariant v0;
    IString arg0;

    switch (cmd->GetId()) {
    case Cmd_Shutdown:
	if (no) {
	    is_shutdown_ = false;
	} else {
	    is_shutdown_ = true;
	}
	break;
    case Cmd_Data_Path:
	cmd->GetArg((size_t)0)->GetValue(v0);
	arg0 = v0.AsString();
	if (arg0 == NAME_DATA_PATH_HOST) {
	    config_.data_path = MODEM_DATA_PATH_HOST;
	} else if (arg0 == NAME_DATA_PATH_ETHERNET) {
	    config_.data_path = MODEM_DATA_PATH_ETHERNET;
	} else {
	    assert(false);
	}
	break;
    case Cmd_Modulation_Restriction:
	cmd->GetArg((size_t)0)->GetValue(v0);
	arg0 = v0.AsString();
	if (arg0==NAME_MODULATION_UNSPECIFIED) {
	    config_.restrict_modulation = MODEM_MODULATION_UNSPECIFIED;
	} else if (arg0==NAME_MODULATION_V21) {
	    config_.restrict_modulation = MODEM_MODULATION_V21;
	} else if (arg0==NAME_MODULATION_V22) {
	    config_.restrict_modulation = MODEM_MODULATION_V22;
	} else if (arg0==NAME_MODULATION_V22BIS) {
	    config_.restrict_modulation = MODEM_MODULATION_V22BIS;
	} else if (arg0==NAME_MODULATION_V23) {
	    config_.restrict_modulation = MODEM_MODULATION_V23;
	} else if (arg0==NAME_MODULATION_V32) {
	    config_.restrict_modulation = MODEM_MODULATION_V32;
	} else if (arg0==NAME_MODULATION_VFC) {
	    config_.restrict_modulation = MODEM_MODULATION_VFC;
	} else if (arg0==NAME_MODULATION_V34) {
	    config_.restrict_modulation = MODEM_MODULATION_V34;
	} else if (arg0==NAME_MODULATION_K56) {
	    config_.restrict_modulation = MODEM_MODULATION_K56;
	} else if (arg0==NAME_MODULATION_V90) {
	    config_.restrict_modulation = MODEM_MODULATION_V90;
	} else {
	    assert(false);
	}
	break;
    case Cmd_Data_Compression:
	cmd->GetArg((size_t)0)->GetValue(v0);
	arg0 = v0.AsString();
	if (arg0==NAME_COMPRESSION_NONE) {
	    config_.compression = MODEM_COMPRESSION_NONE;
	} else if (arg0==NAME_COMPRESSION_MNP5) {
	    config_.compression = MODEM_COMPRESSION_MNP5;
	} else if (arg0==NAME_COMPRESSION_V42BIS) {
	    config_.compression = MODEM_COMPRESSION_V42BIS;
	} else if (arg0==NAME_COMPRESSION_ALL) {
	    config_.compression = MODEM_COMPRESSION_ALL;
	} else {
	    assert(false);
	}
	break;
    case Cmd_Framing:
	cmd->GetArg((size_t)0)->GetValue(v0);
	arg0 = v0.AsString();
	if (arg0==NAME_FRAMING_NONE) {
	    config_.framing = MODEM_FRAMING_NONE;
	} else if (arg0==NAME_FRAMING_SLIP) {
	    config_.framing = MODEM_FRAMING_SLIP;
	} else if (arg0==NAME_FRAMING_PPP) {
	    config_.framing = MODEM_FRAMING_PPP;
	} else {
	    assert(false);
	}
	break;
    case Cmd_Source:
	cmd->GetArg((size_t)0)->GetValue(v0);
	arg0 = v0.AsString();

	int count;
	List<Modem_Source>::iterator src_it;
	CVariant arg2;

	cmd->GetArg(2)->GetValue(arg2);
	count = arg2.AsInt();

	if (no) {
	    count = 0;
	}

	if (!ACQUIRE_SYSTEM_LOCK()) {
	    return;
	}
	src_it = FindSource(arg0.c_str());
	if (src_it == sources_.end()) {
	    if (count == 0) {
		// Attempt to delete but source is not found
		session->SendErrorf("Invalid name %s\n", arg0.c_str());
	    } else {
		// Create new source
		Modem_Source src;
		memset(&src, 0, sizeof(src));
		strcpy(src.name, arg0.c_str());
		src.max_count = count;
		sources_.push_back(src);
		session->Sendf("New source created from board %s\n", arg0.c_str());
	    }

	} else {
	    if (count == 0) {
		// Delete existing
		src_it->max_count = count;
		if (src_it->used_count == 0) {
		    sources_.erase(src_it);
		    session->Sendf("Modem_Source %s deleted\n", arg0.c_str());
		} else {
		    session->Sendf("Modem_Source %s will be deleted when all channels cleared\n", 
				   arg0.c_str());
		}
	    } else {
		// Modify existing source
		src_it->max_count = count;
		session->Sendf("Modem_Source modified for board %s\n", arg0.c_str());
	    }
	}
	RELEASE_SYSTEM_LOCK();
	break;
    }
}

bool Modem_Pool::OnShowConfig( const Cfg_Tree_Entry *entry,
				 String & output, 
				 const String & old_indent, 
				 int indent_size) const
{
    String new_indent = old_indent + String("  ");
    String newline("\r\n");

    output += old_indent + "modem-pool " + Name() + newline;

    // data path
    const char *data_path = "??";
    switch (config_.data_path) {
    case MODEM_DATA_PATH_HOST:
	data_path = NAME_DATA_PATH_HOST; break;
    case MODEM_DATA_PATH_ETHERNET:
	data_path = NAME_DATA_PATH_ETHERNET; break;
    }
    output += new_indent + "data-path " + data_path + newline;

    // modulation restriction.
    const char *mod_res = "??";
    switch (config_.restrict_modulation) {
    case MODEM_MODULATION_UNSPECIFIED:
	mod_res = NAME_MODULATION_UNSPECIFIED; break;
    case MODEM_MODULATION_V21:
	mod_res = NAME_MODULATION_V21; break;
    case MODEM_MODULATION_V22:
	mod_res = NAME_MODULATION_V22; break;
    case MODEM_MODULATION_V22BIS:
	mod_res = NAME_MODULATION_V22BIS; break;
    case MODEM_MODULATION_V23:
	mod_res = NAME_MODULATION_V23; break;
    case MODEM_MODULATION_V32:
	mod_res = NAME_MODULATION_V32; break;
    case MODEM_MODULATION_VFC:
	mod_res = NAME_MODULATION_VFC; break;
    case MODEM_MODULATION_V34:
	mod_res = NAME_MODULATION_V34; break;
    case MODEM_MODULATION_K56:
	mod_res = NAME_MODULATION_K56; break;
    case MODEM_MODULATION_V90:
	mod_res = NAME_MODULATION_V90; break;
    }
    output += new_indent + "modulation-restriction " + mod_res + newline;

    // data compression
    const char *data_comp = "??";
    switch (config_.compression) {
    case MODEM_COMPRESSION_NONE:
	data_comp = NAME_COMPRESSION_NONE; break;
    case MODEM_COMPRESSION_MNP5:
	data_comp = NAME_COMPRESSION_MNP5; break;
    case MODEM_COMPRESSION_V42BIS:
	data_comp = NAME_COMPRESSION_V42BIS; break;
    case MODEM_COMPRESSION_ALL:
	data_comp = NAME_COMPRESSION_ALL; break;
    }
    output += new_indent + "data-compression " + data_comp + newline;

    // framing
    const char *framing = "??";
    switch (config_.framing) {
    case MODEM_FRAMING_NONE:
	framing = NAME_FRAMING_NONE; break;
    case MODEM_FRAMING_SLIP:
	framing = NAME_FRAMING_SLIP; break;
    case MODEM_FRAMING_PPP:
	framing = NAME_FRAMING_PPP; break;
    }
    output += new_indent + "framing " + framing + newline;

    // Source
    List<Modem_Source>::const_iterator src_it, src_end;
    src_end = sources_.end();
    for (src_it=sources_.begin(); src_it!=src_end; ++src_it) {
	const Modem_Source & src = *src_it;
	char str_channel_count[20];
	sprintf(str_channel_count, "%d", src.max_count);
	output += new_indent + "source " + src.name + " channel-count " + 
		  str_channel_count + newline;
    }

    // Shutdown
    if (is_shutdown_) {
	output += new_indent + "shutdown" + newline;
    } else {
	output += new_indent + "no shutdown" + newline;
    }

    // Stop
    output += new_indent + "top" + newline;

    return OK;
}

void Modem_Pool::OnClearCounter()
{
}

Modem_Channel *Modem_Pool::AllocChannel()
{
    unsigned least_count = 0xFFFFFFFF;
    List<Modem_Source>::iterator it, end, src_it;
    unsigned i;

    if (is_shutdown_) {
	CLog::Warning(THIS_MODULE, "Unable to allocate modem from pool %s: pool is shutdown",
		      Name().c_str());
	return NULL;
    }

    end = sources_.end();
    src_it = sources_.end();
    for (it = sources_.begin(); it!=end; ++it) {
	if (it->tot_usage_count < least_count) {
	    least_count = it->tot_usage_count;
	    src_it = it;
	}
    }
    
    if (src_it == end) {
	CLog::Warning(THIS_MODULE, "Unable to allocate modem channel from pool %s",
		      Name().c_str());
	return NULL;
    }

    String board_name = src_it->name;
    src_it->tot_usage_count++;

    Vector<Modem_Factory*> *factories = Modem_Pool_Manager::Instance()->GetFactories();
    Modem_Factory *factory = NULL;

    for (i=0; i<factories->size(); ++i) {
	if (factories->operator[](i)->CanAllocate(board_name)) {
	    factory = factories->operator[](i);
	    break;
	}
    }

    assert(factory != NULL);
    if (factory == NULL) {
	CLog::Fatal(THIS_MODULE, "Error in finding board for modem pool %s",
		    Name().c_str());
	return NULL;
    }

    Modem_Channel *ch = factory->AllocChannel(board_name);
    if (ch) {
	src_it->used_count++;
	ch->PoolInit(this, factory, src_it);
    }
    return ch;
}

void Modem_Pool::FreeChannel(Modem_Channel *ch)
{
    List<Modem_Source>::iterator src_it = ch->GetSourceData();
    src_it->used_count--;
    if (src_it->used_count == 0 && src_it->max_count == 0) {
	sources_.erase(src_it);
    }
    ch->GetFactory()->FreeChannel( ch );
}

///////////////////////////////////////////////////////////////////////////////

Modem_API::Modem_API()
: CAPIProvider(API_PROVIDER_ID_MODEM, "modem-api")
{
}

Modem_API::~Modem_API()
{
}

bool Modem_API::Init()
{
    modem_tls_id_ = Kernel::Instance()->AllocTLS();
    if (modem_tls_id_ == -1) {
	CLog::Error( THIS_MODULE, "Unable to allocate TLS");
	return NOT_OK;
    }

    if (API_Provider_Manager::Instance()->RegisterProvider(this) == NOT_OK) {
	CLog::Error( THIS_MODULE, "Unable to register provider");
	return NOT_OK;
    }

    enum
    {
	API_ID_MODEM_ALLOC,
	API_ID_MODEM_DIAL,
	API_ID_MODEM_ANSWER,
	API_ID_MODEM_CLOSE,
	API_ID_MODEM_SEND,
	API_ID_MODEM_RECV,
	API_ID_MODEM_GET_STATUS,
	API_ID_MODEM_GET_MEDIA,
    };

    // STATUS API_CALL API_ModemAlloc( CTask *task, const char *pool, int *ret );
    if (RegisterAPI("ModemAlloc", API_ID_MODEM_ALLOC, false,
		    CVariant::VT_INT,
		    (CAPIProvider::APIPROC) &Modem_API::API_ModemAlloc,
		    CAPIDef::CDECL_TYPE, 3,
		    CArgType( "task", BYVAL, CVariant::VT_TASK),
		    CArgType( "pool", BYVAL, CVariant::VT_STRING),
		    CArgType( "rethnd", BYREF, CVariant::VT_RET)) == NOT_OK)
    {
	return NOT_OK;
    }

    // STATUS API_CALL API_ModemDial( CTask *task, int handle, const char *nb );
    if (RegisterAPI("ModemDial", API_ID_MODEM_DIAL, false,
		    CVariant::VT_VOID,
		    (CAPIProvider::APIPROC) &Modem_API::API_ModemDial,
		    CAPIDef::CDECL_TYPE, 3,
		    CArgType( "task", BYVAL, CVariant::VT_TASK),
		    CArgType( "handle", BYVAL, CVariant::VT_INT),
		    CArgType( "number", BYVAL, CVariant::VT_STRING)) == NOT_OK)
    {
	return NOT_OK;
    }

    // STATUS API_CALL API_ModemAnswer( CTask *task, int handle );
    if (RegisterAPI("ModemAnswer", API_ID_MODEM_ANSWER, false,
		    CVariant::VT_VOID,
		    (CAPIProvider::APIPROC) &Modem_API::API_ModemAnswer,
		    CAPIDef::CDECL_TYPE, 2,
		    CArgType( "task", BYVAL, CVariant::VT_TASK),
		    CArgType( "handle", BYVAL, CVariant::VT_INT)) == NOT_OK)
    {
	return NOT_OK;
    }

    // STATUS API_CALL API_ModemClose( CTask *task, int handle );
    if (RegisterAPI("ModemClose", API_ID_MODEM_CLOSE, false,
		    CVariant::VT_VOID,
		    (CAPIProvider::APIPROC) &Modem_API::API_ModemClose,
		    CAPIDef::CDECL_TYPE, 2,
		    CArgType( "task", BYVAL, CVariant::VT_TASK),
		    CArgType( "handle", BYVAL, CVariant::VT_INT)) == NOT_OK)
    {
	return NOT_OK;
    }

    // STATUS API_CALL API_ModemSend( CTask *task, int handle, const char *data );
    if (RegisterAPI("ModemSend", API_ID_MODEM_SEND, false,
		    CVariant::VT_VOID,
		    (CAPIProvider::APIPROC) &Modem_API::API_ModemSend,
		    CAPIDef::CDECL_TYPE, 3,
		    CArgType( "task", BYVAL, CVariant::VT_TASK),
		    CArgType( "handle", BYVAL, CVariant::VT_INT),
		    CArgType( "data", BYVAL, CVariant::VT_STRING)) == NOT_OK)
    {
	return NOT_OK;
    }

    // STATUS API_CALL API_ModemReceive( CTask *task, char *data, int handle );
    if (RegisterAPI("ModemReceive", API_ID_MODEM_RECV, false,
		    CVariant::VT_STRING,
		    (CAPIProvider::APIPROC) &Modem_API::API_ModemReceive,
		    CAPIDef::CDECL_TYPE, 3,
		    CArgType( "task", BYVAL, CVariant::VT_TASK),
		    CArgType( "data", BYREF, CVariant::VT_RET),
		    CArgType( "handle", BYVAL, CVariant::VT_INT)) == NOT_OK)
    {
	return NOT_OK;
    }

    // STATUS API_CALL API_ModemGetStatus( CTask *task, int handle, int *ret );
    if (RegisterAPI("ModemGetStatus", API_ID_MODEM_GET_STATUS, false,
		    CVariant::VT_INT,
		    (CAPIProvider::APIPROC) &Modem_API::API_ModemGetStatus,
		    CAPIDef::CDECL_TYPE, 3,
		    CArgType( "task", BYVAL, CVariant::VT_TASK),
		    CArgType( "handle", BYVAL, CVariant::VT_INT),
		    CArgType( "retstatus", BYREF, CVariant::VT_RET)) == NOT_OK)
    {
	return NOT_OK;
    }

    // STATUS API_CALL API_ModemGetMedia( CTask *task, const char *pool, int *ret );
    if (RegisterAPI("ModemGetMedia", API_ID_MODEM_GET_MEDIA, false,
		    CVariant::VT_INT,
		    (CAPIProvider::APIPROC) &Modem_API::API_ModemGetMedia,
		    CAPIDef::CDECL_TYPE, 3,
		    CArgType( "task", BYVAL, CVariant::VT_TASK),
		    CArgType( "handle", BYVAL, CVariant::VT_INT),
		    CArgType( "ret", BYREF, CVariant::VT_RET)) == NOT_OK)
    {
	return NOT_OK;
    }

    return OK;
}

STATUS Modem_API::Shutdown()
{
    API_Provider_Manager::Instance()->UnregisterProvider(this);
    return OK;
}

STATUS API_CALL Modem_API::API_ModemAlloc( CTask *task, const char *pool_name, int *ret )
{
    Modem_TLS *tls;
    Modem_Channel *modem = NULL;

    CLog::Detail(THIS_MODULE, task, "API_ModemAlloc, pool=%s", pool_name);

    *ret = -1;

    tls = (Modem_TLS*) task->GetTLSValue(modem_tls_id_);
    if (tls == NULL) {
	NEW_RETURN_(tls, Modem_TLS);
	memset(tls, 0, sizeof(*tls));
	task->SetTLSValue(modem_tls_id_, (int)tls);
    }

    Modem_Pool *pool;
    pool = Modem_Pool_Manager::Instance()->FindPool(pool_name);
    if (pool == NULL) {
	CLog::Error(THIS_MODULE, task, "API ModemAlloc: unable to find pool %s", pool_name);
	return OK;
    }

    modem = pool->AllocChannel();
    if (modem == NULL) {
	return OK;
    }

    for (*ret=0; *ret<MAX_MODEM_PER_TASK; ++*ret) {
	if (tls->modem[*ret] == NULL) {
	    break;
	}
    }
    if (*ret == MAX_MODEM_PER_TASK) {
	*ret = -1;
	CLog::Error(THIS_MODULE, task, "API ModemAlloc: too many modem handles to this task");
	modem->FreeDevice();
	return OK;
    }

    tls->modem[*ret] = modem;
    modem->Open( pool->GetConfig() );
    return OK;
}

STATUS API_CALL Modem_API::API_ModemAnswer( CTask *task, int handle )
{
    Modem_TLS *tls = (Modem_TLS*) task->GetTLSValue(modem_tls_id_);
    if (tls == NULL) {
	CLog::Error(THIS_MODULE, task, "ModemActivate: invalid modem handle %d for task", handle);
	return OK;
    }
    if (handle<0 || handle >= MAX_MODEM_PER_TASK || tls->modem[handle] == NULL) {
	CLog::Error(THIS_MODULE, task, "ModemActivate: invalid modem handle %d for task", handle);
	return OK;
    }

    tls->modem[handle]->Activate( Modem_Channel::ROLE_ANSWER, NULL);
    return OK;
}

STATUS API_CALL Modem_API::API_ModemDial( CTask *task, int handle, const char *nb )
{
    Modem_TLS *tls = (Modem_TLS*) task->GetTLSValue(modem_tls_id_);
    if (tls == NULL) {
	CLog::Error(THIS_MODULE, task, "ModemActivate: invalid modem handle %d for task", handle);
	return OK;
    }
    if (handle<0 || handle >= MAX_MODEM_PER_TASK || tls->modem[handle] == NULL) {
	CLog::Error(THIS_MODULE, task, "ModemActivate: invalid modem handle %d for task", handle);
	return OK;
    }

    tls->modem[handle]->Activate( Modem_Channel::ROLE_CALL, nb);
    return OK;
}

STATUS API_CALL Modem_API::API_ModemClose( CTask *task, int handle )
{
    Modem_TLS *tls = (Modem_TLS*) task->GetTLSValue(modem_tls_id_);
    if (tls == NULL) {
	CLog::Error(THIS_MODULE, task, "ModemClose: invalid modem handle %d for task", handle);
	return OK;
    }
    if (handle<0 || handle >= MAX_MODEM_PER_TASK || tls->modem[handle] == NULL) {
	CLog::Error(THIS_MODULE, task, "ModemClose: invalid modem handle %d for task", handle);
	return OK;
    }

    tls->modem[handle]->Close();
    tls->modem[handle]->FreeDevice();

    tls->modem[handle] = NULL;
    return OK;
}

STATUS API_CALL Modem_API::API_ModemSend( CTask *task, int handle, const char *data )
{
    Modem_TLS *tls = (Modem_TLS*) task->GetTLSValue(modem_tls_id_);
    if (tls == NULL) {
	CLog::Error(THIS_MODULE, task, "ModemSend: invalid modem handle %d for task", handle);
	return OK;
    }
    if (handle<0 || handle >= MAX_MODEM_PER_TASK || tls->modem[handle] == NULL) {
	CLog::Error(THIS_MODULE, task, "ModemSend: invalid modem handle %d for task", handle);
	return OK;
    }

    tls->modem[handle]->Send(data, strlen(data));
    return OK;
}

STATUS API_CALL Modem_API::API_ModemReceive( CTask *task, char *data, int handle )
{
    Modem_TLS *tls = (Modem_TLS*) task->GetTLSValue(modem_tls_id_);
    if (tls == NULL) {
	CLog::Error(THIS_MODULE, task, "ModemReceive: invalid modem handle %d for task", handle);
	return OK;
    }
    if (handle<0 || handle >= MAX_MODEM_PER_TASK || tls->modem[handle] == NULL) {
	CLog::Error(THIS_MODULE, task, "ModemReceive: invalid modem handle %d for task", handle);
	return OK;
    }

    int len = tls->modem[handle]->Receive(data, MAXAPPSTRLEN);
    if (len > 0) {
	data[len] = '\0';
    } else {
	data[0] = '\0';
    }

    /* Replace NULLs with some character. */
    char *s = (char *)data;
    while (*s) {
	if (!*s) {
	    *s = (char)255;
	    CLog::Warning(THIS_MODULE, "Found NULL character at offset %d (data=%s)", s-data, data);
	}
	++s;
    }

    return OK;
}

STATUS API_CALL Modem_API::API_ModemGetStatus( CTask *task, int handle, int *ret )
{
    Modem_TLS *tls = (Modem_TLS*) task->GetTLSValue(modem_tls_id_);
    if (tls == NULL) {
	CLog::Error(THIS_MODULE, task, "ModemGetStatus: invalid modem handle %d for task", handle);
	return OK;
    }
    if (handle<0 || handle >= MAX_MODEM_PER_TASK || tls->modem[handle] == NULL) {
	CLog::Error(THIS_MODULE, task, "ModemGetStatus: invalid modem handle %d for task", handle);
	return OK;
    }

    *ret = tls->modem[handle]->GetStatus();
    return OK;
}

STATUS API_CALL Modem_API::API_ModemGetMedia( CTask *task, int handle, int *ret )
{
    Modem_TLS *tls = (Modem_TLS*) task->GetTLSValue(modem_tls_id_);
    if (tls == NULL) {
	CLog::Error(THIS_MODULE, task, "ModemGetMedia: invalid modem handle %d for task", handle);
	return OK;
    }
    if (handle<0 || handle >= MAX_MODEM_PER_TASK || tls->modem[handle] == NULL) {
	CLog::Error(THIS_MODULE, task, "ModemGetMedia: invalid modem handle %d for task", handle);
	return OK;
    }

    *ret = (int)tls->modem[handle];
    return OK;
}


///////////////////////////////////////////////////////////////////////////////


#endif	/* #ifdef XGATE_HAS_MODEM */

