/* $Header: /xGate/announce.cpp 6     4/30/04 8:40p Bennylp $
 *   
 * XGATE
 * (C) 2001 Switchlab, Ltd
 *
 * Announcement resource
 *
 * $Log: /xGate/announce.cpp $
 * 
 * 6     4/30/04 8:40p Bennylp
 * Updated for changes in Mapletree interface
 * 
 * 5     4/24/03 5:58p Bennylp
 * Fix name inconsistency: XG_HAS_ changed to XGATE_HAS_
 * 
 * 4     4/23/03 5:34p Bennylp
 * Administrivia: tidy-up the usage of API provider ID. Now the
 * enumeration in api.h should be used instead of bare constants.
 * 
 * 3     11/04/02 7:02p Bennylp
 * Show resource name in switch device.
 * 
 * 2     11/04/02 5:29p Bennylp
 * Added device name.
 * 
 * 1     10/30/02 4:28p Bennylp
 * Initial version
 */

#include "stdafx.h"
#include "announce.h"

#ifdef XGATE_HAS_ANNOUNCE_RES

#include "log.h"
#include "kernel.h"
#include "term.h"

#ifdef XGATE_HAS_MTN
#include "mapletree.h"
#include "../MTN_Lib/src/mtn_channel.h"
#include "../MTN_Lib/src/mtn_msg.h"
#endif

#define THISMODULE  "announce"


Announce_Mgr *Announce_Mgr::instance_;


///////////////////////////////////////////////////////////////////////////////
/*
 * ANNOUNCEMENT DEVICE BASE CLASS
 */
Announce_Device::Announce_Device(Announce_Res *res, SwitchDevice *swdev)
: res_(res), SupportSwitching(swdev)
{
}

Announce_Device::~Announce_Device()
{
}

const Announce_Res *Announce_Device::get_res() const
{
    return res_;
}


///////////////////////////////////////////////////////////////////////////////
/*
 * SUPPORT FOR MAPLETREE DEVICE.
 */
#ifdef XGATE_HAS_MTN

class Mapletree_Announce_Device : public Announce_Device, public MTN_Message_Callback
{
public:
    Mapletree_Announce_Device(Announce_Res *res, MTN_Channel *ch, SwitchDevice *swdev);
    ~Mapletree_Announce_Device();

    MTN_Channel *get_channel();

    virtual bool play_voice(Voice_Codec codec, const char *path);
    virtual bool play_tone(Tone_Sig tone_sig, const char * digits);
    virtual void stop();
    virtual const char *get_name() const;
    virtual const char *get_status() const;

private:
    MTN_Channel *channel_;

    enum State
    {
	State_IDLE,
	State_ALLOCATING,
	State_SETTING_SIGNALING_TYPE,
	State_SETTING_TO_SIGNALING,
	State_SETTING,
	State_PLAYING,
	State_ERROR,
    } state_;

    int	    mtn_tone_sig_;
    String  mtn_tone_digits_;

    /** Play tone.
     */
    bool dev_play_tone();

    /** Upstream message notification.
     */
    virtual int on_message ( MTN_Queue_Type queue_id, const MTN_Message * msg);
};


Mapletree_Announce_Device::Mapletree_Announce_Device(Announce_Res *res, MTN_Channel *ch, SwitchDevice *swdev)
: Announce_Device(res, swdev), channel_(ch), state_(State_IDLE),
  mtn_tone_sig_()
{
}

Mapletree_Announce_Device::~Mapletree_Announce_Device()
{
    // Detach channel from switching, to prevent crash when switching framework
    // tries to call StopListen when this device has been deleted (e.g. when
    // the resource is shutdown.
    Mapletree_Switch_Device *mtn_sw = static_cast<Mapletree_Switch_Device *> (GetSwitchDevice());
    mtn_sw->DetachChannel();
}

MTN_Channel *Mapletree_Announce_Device::get_channel()
{
    return channel_;
}

const char *Mapletree_Announce_Device::get_name() const
{
    return channel_->get_channel_name();
}

const char *Mapletree_Announce_Device::get_status() const
{
    switch (state_) {
    case State_IDLE:
	return "IDLE";
    case State_ALLOCATING:
	return "ALLOCATING";
    case State_SETTING_SIGNALING_TYPE:
	return "SETTING_SIGNALING_TYPE";
    case State_SETTING_TO_SIGNALING:
	return "SETTING_TO_SIGNALING";
    case State_SETTING:
	return "SETTING";
    case State_PLAYING:
	return "PLAYING";
    case State_ERROR:
	return "ERROR";
    default:
	return "???";
    }
}

bool Mapletree_Announce_Device::play_voice(Voice_Codec codec, const char *path)
{
    CLog::Error(THISMODULE, "Unsupported announcement type (VOICE) for Mapletree device");
    return false;
}

bool Mapletree_Announce_Device::dev_play_tone()
{
    MTN_Tone_Gen_Mob_Data tone_gen_mob;
    tone_gen_mob.set_tones(mtn_tone_digits_.c_str());
    return channel_->set_mob_async(&tone_gen_mob);
}

bool Mapletree_Announce_Device::play_tone(Tone_Sig tone_sig, const char *digits)
{
    // Convert tone signaling to MTN
    switch (tone_sig) {
    case TONE_DTMF:	mtn_tone_sig_ = ToneSigType_DTMF; break;
    case TONE_MF:	mtn_tone_sig_ = ToneSigType_MF; break;
    case TONE_R1:	mtn_tone_sig_ = ToneSigType_R1; break;
    case TONE_R2_IN:	mtn_tone_sig_ = ToneSigType_R2_INCOMING; break;
    case TONE_R2_OUT:	mtn_tone_sig_ = ToneSigType_R2_OUTGOING; break;
    default:		mtn_tone_sig_ = ToneSigType_DTMF; break;
    }

    // Convert digits.
    // TODO:
    mtn_tone_digits_ = digits;

    // Start.
    channel_->set_callback(this);
    state_ = State_ALLOCATING;
    return channel_->force_state_async(PortState_ALLOCATED);
}

void Mapletree_Announce_Device::stop()
{
    channel_->set_callback(NULL);

    channel_->force_state_async(PortState_TERMINATING);
    DWORD start = GetTickCount();
    do {
	channel_->get_module()->handle_events();
	::Sleep(0);
    } while (channel_->get_state() != PortState_IDLE && 
	     channel_->get_state() != PortState_OUT_OF_SERVICE &&
	     ::GetTickCount() - start < 200);

    if (channel_->get_state() != PortState_IDLE) {
	CLog::Warning(THISMODULE, "Announcement device %s[%s]: unable to stop device (port state=%s)",
		      get_res()->get_name(), channel_->get_channel_name(), 
		      MTN_Util::port_state_name(channel_->get_state()));
    }

    state_ = State_IDLE;
}

int Mapletree_Announce_Device::on_message ( MTN_Queue_Type queue_id, const MTN_Message * msg)
{
    if (state_ == State_ERROR)
	return 0;
    if (msg->get_status() != MsgStatus_OK) {
	state_ = State_ERROR;
	CLog::Warning(THISMODULE, "Announcement device %s[%s]: message with error status: %s",
			  get_res()->get_name(), channel_->get_channel_name(), 
			  MTN_Util::msg_print(msg));
	return 0;
    }
 
    switch (state_) {
    case State_IDLE:
	CLog::Warning(THISMODULE, "Announcement device %s[%s]: received unexpected message in state IDLE: %s",
		      get_res()->get_name(), channel_->get_channel_name(), 
		      MTN_Util::msg_print(msg));
	break;
    case State_ALLOCATING:
	if (queue_id == MTN_Q_Cmd && msg->get_arg1()==ManObj_C_PORT_STATE) {
	    state_ = State_SETTING_TO_SIGNALING;
	    if (!channel_->force_state_async(PortState_SIGNALING)) {
		state_ = State_ERROR;
	    }
	} else {
	    CLog::Warning(THISMODULE, "Announcement device %s[%s]: received unexpected message in state ALLOCATING: %s",
			  get_res()->get_name(), channel_->get_channel_name(), 
			  MTN_Util::msg_print(msg));
	}
	break;
    case State_SETTING_TO_SIGNALING:
	if (queue_id == MTN_Q_Cmd && msg->get_arg1()==ManObj_C_PORT_STATE) {
	    state_ = State_SETTING_SIGNALING_TYPE;
	    if (!channel_->set_man_obj_async(ManObj_G_TONE_SIGNALING_TYPE, mtn_tone_sig_)) {
		state_ = State_ERROR;
	    }
	} else {
	    CLog::Warning(THISMODULE, "Announcement device %s[%s]: received unexpected message in state SETTING_TO_SIGNALING: %s",
			  get_res()->get_name(), channel_->get_channel_name(), 
			  MTN_Util::msg_print(msg));
	}
	break;
    case State_SETTING_SIGNALING_TYPE:
	if (queue_id == MTN_Q_Cmd && msg->get_arg1()==ManObj_G_TONE_SIGNALING_TYPE) {
	    state_ = State_SETTING;
	    if (!dev_play_tone()) {
		state_ = State_ERROR;
	    }
	} else {
	    CLog::Warning(THISMODULE, "Announcement device %s[%s]: received unexpected message in state SETTING_SIGNALING_TYPE: %s",
			  get_res()->get_name(), channel_->get_channel_name(), 
			  MTN_Util::msg_print(msg));
	}
	break;
    case State_SETTING:
	if (queue_id == MTN_Q_Cmd && (msg->get_type() == MsgType_C_SET_USER_MOB ||
				      msg->get_type() == MsgType_C_SET_PREDEF_MOB))
	{
	    state_ = State_PLAYING;
	} else if (queue_id == MTN_Q_Cmd && msg->get_arg1() == ManObj_G_MF_TRANSMIT_POWER) {

	} else  {
	    CLog::Warning(THISMODULE, "Announcement device %s[%s]: received unexpected message in state SETTING: %s",
			  get_res()->get_name(), channel_->get_channel_name(), 
			  MTN_Util::msg_print(msg));
	}

	break;
    case State_PLAYING:
	if (queue_id == MTN_Q_TrapNotification && msg->get_arg0() == Trap_TONE_GENERATION_COMPLETE) {
	    state_ = State_SETTING;
	    if (!dev_play_tone()) {
		state_ = State_ERROR;
	    }
	}else  {
	    CLog::Warning(THISMODULE, "Announcement device %s[%s]: received unexpected message in state PLAYINH: %s",
			  get_res()->get_name(), channel_->get_channel_name(), 
			  MTN_Util::msg_print(msg));
	}
	break;
    }

    return 0;
}

#endif

///////////////////////////////////////////////////////////////////////////////
/*
 * ANNOUNCEMENT DEVICE FACTORY
 */

class Announce_Device_Factory : public ServiceProvider
{
public:
    static Announce_Device_Factory *Create();
    static Announce_Device_Factory *Instance();

    /** Get list of boards capable for creating devices */
    void GetBoardList(List<String> & board_list);

    /** Allocate one device from the specified board */
    Announce_Device *AllocateDevice(Announce_Res *res, const char *board_name, Announce_Device::Type type);

    /** Release device */
    void ReleaseDevice(const char *board_name, Announce_Device *dev);

private:
    static Announce_Device_Factory *instance_;

    Announce_Device_Factory();
    ~Announce_Device_Factory();

    virtual bool Load() { return true; }
    virtual bool Unload() { return true; }
};

Announce_Device_Factory *Announce_Device_Factory::instance_;


Announce_Device_Factory::Announce_Device_Factory()
: ServiceProvider("Announce Device Factory", false, false)
{
    instance_ = this;
}

Announce_Device_Factory::~Announce_Device_Factory()
{
    instance_ = NULL;
}

Announce_Device_Factory *Announce_Device_Factory::Create()
{
    if (instance_)
	return instance_;

    new Announce_Device_Factory;
    return instance_;
}

Announce_Device_Factory *Announce_Device_Factory::Instance()
{
    return Create();
}

/** Get list of boards capable for creating devices */
void Announce_Device_Factory::GetBoardList(List<String> & board_list)
{
    /* Return list of Mapletree boards */
#ifdef XGATE_HAS_MTN
    Mapletree_Module_Manager *mtn_mgr = Mapletree_Module_Manager::Instance();
    if (mtn_mgr) {
	int i;
	for (i=0; i<mtn_mgr->GetModuleCount(); ++i) {
	    Mapletree_Module *mod = mtn_mgr->GetModule(i);
	    if (mod)
		board_list.push_back(mod->GetName());
	}
    }
#endif
}

/** Allocate one device from the specified board */
Announce_Device *Announce_Device_Factory::AllocateDevice(Announce_Res *res, 
							 const char *board_name, Announce_Device::Type type)
{
    Announce_Device *dev = NULL;

    /* Dummy 'if', to get rid of compilation error caused by ifdefs */
    if (0) {
    }
    /* See if the request is for Mapletree boards */
#ifdef XGATE_HAS_MTN
    else if (strnicmp(board_name, "mtn", 3) == 0) {
	Mapletree_Module_Manager *mtn_mgr = Mapletree_Module_Manager::Instance();
	if (mtn_mgr) {
	    Mapletree_Module *mod = mtn_mgr->FindModule(board_name);
	    if (mod) {
		MTN_Channel *ch = mod->AllocateChannel();
		if (ch) {
		    unsigned ch_id = ch->get_channel_id();
		    unsigned mod_id = ch->get_module()->get_mod_id();

		    dev = new Mapletree_Announce_Device(res, ch, new Mapletree_Switch_Device(res->get_name(), ch, mod->GetTdmBusType(),
							mod->GetTdmAddress(mod_id, ch_id)));
		} else {
		    CLog::Error(THISMODULE, "Unable to allocate MTN device from board %s", board_name);
		}
	    } else {
		CLog::Error(THISMODULE, "Unable to find board %s", board_name);
	    }
	} else {
	    CLog::Error(THISMODULE, "MTN subsystem is not initialized");
	}
    }
#endif

    if (!dev) {
	CLog::Error(THISMODULE, "Unable to allocate announcement resource from '%s'", board_name);
    }

    return dev;
}

/** Release device */
void Announce_Device_Factory::ReleaseDevice(const char *board_name, Announce_Device *dev)
{
    /* Dummy 'if', to get rid of compilation error caused by ifdefs */
    if (0) {
    }
    /* See if the request is for Mapletree boards */
#ifdef XGATE_HAS_MTN
    else if (strnicmp(board_name, "mtn", 3) == 0) {
	Mapletree_Announce_Device *mtn_dev = static_cast<Mapletree_Announce_Device *>(dev);
	Mapletree_Module_Manager *mtn_mgr = Mapletree_Module_Manager::Instance();
	if (mtn_mgr) {
	    Mapletree_Module *mod = mtn_mgr->FindModule(board_name);
	    if (mod)
		mod->ReleaseChannel(mtn_dev->get_channel());
	}
    }
#endif

    delete dev;
}

///////////////////////////////////////////////////////////////////////////////
/**  Implementation of validator, to be used to display list of available
     boards to allocate the resource from.
 */
class Announce_Board_Validator : public CVariantDynamicChoiceValidator {
public:
    Announce_Board_Validator() : initialized_(false) {}
    Announce_Board_Validator( const Announce_Board_Validator & rhs )
	: initialized_(rhs.initialized_), choice_(rhs.choice_) {}

private:
    mutable bool initialized_;
    mutable List<ValidatorChoice> choice_;

    virtual CVariantValidator *Clone() const
    {
	return new Announce_Board_Validator(*this);
    }
    virtual void AcquireLock() const
    {
	ACQUIRE_SYSTEM_LOCK();
    }
    virtual void ReleaseLock() const
    {
	RELEASE_SYSTEM_LOCK();
    }
    virtual const List<ValidatorChoice> & GetChoiceList() const
    {
	if (!initialized_) {
	    Announce_Device_Factory *factory = Announce_Device_Factory::Instance();
	    List<String> board_list;
	    factory->GetBoardList(board_list);

	    List<String>::const_iterator it, end=board_list.end();
	    for (it=board_list.begin(); it!=end; ++it) {
		const String & board_name = *it;
		choice_.push_back(ValidatorChoice(board_name.c_str(), ""));
	    }

	    initialized_ = true;
	}
	return choice_;
    }
};

///////////////////////////////////////////////////////////////////////////////
/*
 * ANNOUNCEMENT RESOURCE
 */
enum
{
    Cmd_Announce,
    Cmd_Allocate,
    Cmd_Shutdown,
    Cmd_Status,
};

Announce_Res::Announce_Res(const char *name)
: ServiceProvider(name, false, false), dev_(NULL), name_(name),
  type_(Announce_Device::TYPE_TONE_GEN), voice_codec_(Announce_Device::VOICE_G726), 
  tone_sig_(Announce_Device::TONE_DTMF), cfg_(NULL),
  is_enabled_(false), is_running_(false)
{
}

Announce_Res::~Announce_Res()
{
}

const char *Announce_Res::get_name() const
{
    return name_.c_str();
}

const char *Announce_Res::get_board() const
{
    return board_.c_str();
}

Announce_Device::Type Announce_Res::get_type() const
{
    return type_;
}

Announce_Device::Voice_Codec Announce_Res::get_voice_codec() const
{
    return voice_codec_;
}

const char *Announce_Res::get_voice_path() const
{
    return voice_path_.c_str();
}

Announce_Device::Tone_Sig Announce_Res::get_tone_sig() const
{
    return tone_sig_;
}

const char *Announce_Res::get_tone_digits() const
{
    return tone_digits_.c_str();
}

bool Announce_Res::is_enabled() const
{
    return is_enabled_;
}

bool Announce_Res::is_running() const
{
    return is_running_;
}

Announce_Device *Announce_Res::get_device()
{
    return dev_;
}

Cfg_Tree_Group *Announce_Res::GetConfig()
{
    return cfg_;
}

bool Announce_Res::Load()
{
    Term_Cmd *cmd;

    cfg_ = new Cfg_Tree_Group( name_, "", Announce_Mgr::Instance()->GetCfgEntry());
    cfg_->RegisterShowHandler( this );
    Kernel::Instance()->RootConfig()->RegisterShowHandler( this );

    /*
     * Allocation source.
     */
    cmd = new Term_Cmd( this, Cmd_Allocate, "allocate-from", "Configure module to allocate the resource from",
			PrivilegeLevelAdmin, TermModeConfig, false );
    cmd->Add(new Term_Cmd_Arg("@0", CVariant(""), Announce_Board_Validator()));
    RegisterTermCmd( cmd, cfg_ );

    /*
     * Announcement settings command.
     */
    cmd = new Term_Cmd( this, Cmd_Announce, "announce", "Set announcement settings",
			PrivilegeLevelAdmin, TermModeConfig, false );
    CVariantChoiceValidator v_type("tone", "Tone announcement");
    v_type.Add("voice", "Voice announcement");
    cmd->Add(new Term_Cmd_Arg("@0", CVariant(""), v_type));

    // Tone type.
    CVariantChoiceValidator v_tone_type("dtmf", "DTMF tone signaling");
    v_tone_type.Add("mf", "MF tone signaling");
    v_tone_type.Add("r1", "R1 tone signaling");
    v_tone_type.Add("r2-in", "R2 incoming tone signaling");
    v_tone_type.Add("r2-out", "R2 outgoing tone signaling");
    cmd->AddDynamic( 1, 0, CVariant("tone"), new Term_Cmd_Arg("@1", CVariant(""), v_tone_type) );
    cmd->AddDynamic( 2, 0, CVariant("tone"), new Term_Cmd_Arg("@1", CVariant(""), CVariantValidator("", "Tone digits")) );

    // Voice type.
    CVariantChoiceValidator v_voice_codec("pcma", "Codec is G.711/Alaw (PCMA)");
    v_voice_codec.Add("pcmu", "Codec is G.711/Ulaw (PCMU)");
    v_voice_codec.Add("g726", "Codec is G.726/ADPCM (default)");
    cmd->AddDynamic( 1, 0, CVariant("voice"), new Term_Cmd_Arg("@1", CVariant(""), v_voice_codec) );
    cmd->AddDynamic( 2, 0, CVariant("voice"), new Term_Cmd_Arg("@1", CVariant(""), CVariantValidator("", "Voice file path")) );

    RegisterTermCmd( cmd, cfg_ );

    /*
     * Status command.
     */
    cmd = new Term_Cmd( this, Cmd_Status, "status", "Show resource status",
			PrivilegeLevelAdmin, TermModeConfig, false);
    RegisterTermCmd( cmd, cfg_ );

    /*
     * Shutdown command.
     */
    cmd = new Term_Cmd( this, Cmd_Shutdown, "shutdown", "Stop/shutdown the resource",
			PrivilegeLevelAdmin, TermModeConfig, true);
    RegisterTermCmd( cmd, cfg_ );

    return true;
}

bool Announce_Res::Unload()
{
    cfg_->UnregisterShowHandler(this);
    Kernel::Instance()->RootConfig()->UnregisterShowHandler( this );

    Stop();
    return true;
}

bool Announce_Res::CommitConfiguration()
{
    if (is_enabled_ && !is_running_)
	Start();
    return true;
}

bool Announce_Res::Start()
{
    if (is_enabled_) {
	assert(dev_ == NULL);
	assert(!is_running_);

	if (board_.empty()) {
	    CLog::Error(THISMODULE, "Board source is not configured. Unable to allocate resource");
	    return false;
	}

	dev_ = Announce_Device_Factory::Instance()->AllocateDevice(this, board_.c_str(), type_);
	if (!dev_) {
	    return false;
	}
	bool status;
	if (type_ == Announce_Device::TYPE_TONE_GEN)
	    status = dev_->play_tone(tone_sig_, tone_digits_.c_str());
	else
	    status = dev_->play_voice(voice_codec_, voice_path_.c_str());

	if (!status) {
	    CLog::Error(THISMODULE, "Unable to start announcement resource '%s'", name_.c_str());
	    Announce_Device_Factory::Instance()->ReleaseDevice(board_.c_str(), dev_);
	    dev_ = NULL;
	}
	is_running_ = status;
    }
    return is_running;
}

bool Announce_Res::Stop()
{
    if (dev_) {
	if (is_running_)
	    dev_->stop();

	Announce_Device_Factory::Instance()->ReleaseDevice(board_.c_str(), dev_);
	dev_ = NULL;
    }
    is_running_ = false;
    return true;
}

bool Announce_Res::OnShowStatus(String & output)
{
    output += String("Announcement resource ") + name_ + " [" + 
	      (dev_ ? dev_->get_name() : "null" ) + "]: status is " +
	      (is_enabled_ ? (is_running_ ? "RUNNING" : "NOT RUNNING") : "DISABLED") + 
	      " (" + (dev_ ? dev_->get_status() : "NULL") +")\n";
    return true;
}

STATUS Announce_Res::OnShowConfig(const Cfg_Tree_Entry *entry, String &output, 
				  const String &indent, int indent_size) const
{
    String newindent(indent);
    newindent.append(indent_size, ' ');

    ACQUIRE_SYSTEM_LOCK();
    output += indent + "announcement " + name_ + "\n";

    if (board_.size())
	output += newindent + "allocate-from \"" + board_ + "\"\n";
    else
	output += newindent + "#warning: allocate-from is not specified!\n";

    if (type_ == Announce_Device::TYPE_VOICE) {
	output += newindent + "announce voice ";
	switch (voice_codec_) {
	case Announce_Device::VOICE_PCMA: output += "pcma "; break;
	case Announce_Device::VOICE_PCMU: output += "pcmu "; break;
	case Announce_Device::VOICE_G726: output += "g726 "; break;
	default: output += "g726 "; break;
	}
	output += "\"" + voice_path_ + "\"";
    } else {
	output += newindent + "announce tone ";
	switch (tone_sig_) {
	case Announce_Device::TONE_DTMF:     output += "dtmf "; break;
	case Announce_Device::TONE_MF:	     output += "mf "; break;
	case Announce_Device::TONE_R1:       output += "r1 "; break;
	case Announce_Device::TONE_R2_IN:    output += "r2-in "; break;
	case Announce_Device::TONE_R2_OUT:   output += "r2-out "; break;
	default: output += "dtmf"; break;
	}

	output += "\"" + tone_digits_ + "\"";
    }
    output += "\n";
    
    if (is_enabled_ && is_running_)
	output += newindent + "no shutdown\n";
    else
	output += newindent + "shutdown\n";
    
    output += newindent + "top\n\n";
    RELEASE_SYSTEM_LOCK();
    return OK;
}

void Announce_Res::DoTermCmd(Term_Session *session, bool no, Term_Cmd *cmd)
{
    CVariant v0, v1, v2;
    String show_status_output;

    switch (cmd->GetId()) {
    case Cmd_Announce:
	if (is_running_) {
	    session->SendErrorf("Can not configure while resource is running.\n");
	    return;
	}
	if (!ACQUIRE_SYSTEM_LOCK())
	    return;

	cmd->GetArg((size_t)0)->GetValue(v0);
	cmd->GetArg((size_t)1)->GetValue(v1);

	if (!stricmp(v0.AsString(), "tone")) {
	    type_ = Announce_Device::TYPE_TONE_GEN;

	    cmd->GetArg((size_t)2)->GetValue(v2);

	    const char *tone_sig = v1.AsString();
	    if (!stricmp(tone_sig, "dtmf"))
		tone_sig_ = Announce_Device::TONE_DTMF;
	    else if (!stricmp(tone_sig, "mf"))
		tone_sig_ = Announce_Device::TONE_MF;
	    else if (!stricmp(tone_sig, "r1"))
		tone_sig_ = Announce_Device::TONE_R1;
	    else if (!stricmp(tone_sig, "r2-in"))
		tone_sig_ = Announce_Device::TONE_R2_IN;
	    else if (!stricmp(tone_sig, "r2-out"))
		tone_sig_ = Announce_Device::TONE_R2_OUT;
	    else
		assert(false);

	    tone_digits_ = v2.AsString();

	} else if (!stricmp(v0.AsString(), "voice")) {
	    cmd->GetArg((size_t)2)->GetValue(v2);
	    type_ = Announce_Device::TYPE_VOICE;

	    const char *codec = v1.AsString();
	    if (!stricmp(codec, "pcma"))
		voice_codec_ = Announce_Device::VOICE_PCMA;
	    else if (!stricmp(codec, "pcmu"))
		voice_codec_ = Announce_Device::VOICE_PCMU;
	    else if (!stricmp(codec, "g726"))
		voice_codec_ = Announce_Device::VOICE_G726;
	    else
		assert(false);

	    voice_path_ = v2.AsString();

	} else 
	    assert(false);

	RELEASE_SYSTEM_LOCK();
	break;

    case Cmd_Allocate:
	if (is_running_) {
	    session->SendErrorf("Can not configure while resource is running.\n");
	    return;
	}
	if (!ACQUIRE_SYSTEM_LOCK())
	    return;

	cmd->GetArg((size_t)0)->GetValue(v0);
	board_ = v0.AsString();
	RELEASE_SYSTEM_LOCK();
	break;

    case Cmd_Shutdown:
	if (no) {
	    if (is_enabled_ && is_running_)
		session->SendErrorf("Resource is already running\n");
	    else if (!Kernel::Instance()->IsInRunningState()) {
		is_enabled_ = true;
	    } else {
		if (!ACQUIRE_SYSTEM_LOCK())
		    return;

		is_enabled_ = true;
		is_enabled_ = Start();

		RELEASE_SYSTEM_LOCK();

		if (!is_running_)
		    session->SendErrorf("Unable to start announcement resource\n");
	    }
	} else {
	    ACQUIRE_SYSTEM_LOCK();
	    Stop();
	    RELEASE_SYSTEM_LOCK();
	}
	break;

    case Cmd_Status:
	if (!ACQUIRE_SYSTEM_LOCK())
	    return;

	OnShowStatus(show_status_output);
	RELEASE_SYSTEM_LOCK();
	session->Send(show_status_output.c_str(), show_status_output.size());
	break;
    }
}


///////////////////////////////////////////////////////////////////////////////
/*
 * API PROVIDER
 */

class Announce_API : public CAPIProvider {
public:
    enum {
	PROVIDER_ID=API_PROVIDER_ID_ANNOUNCE,
    };

    Announce_API() : CAPIProvider(PROVIDER_ID, "announce") {}
    ~Announce_API() {}

    virtual STATUS Init();
    virtual STATUS Shutdown();
    STATUS API_CALL API_AnnIsRunning(CTask *task, int *ret, const char *ann_name);
    STATUS API_CALL API_AnnGetMedia(CTask *task, int *ret, const char *ann_name);
};


STATUS Announce_API::Init()
{
    if (API_Provider_Manager::Instance()->RegisterProvider(this) == NOT_OK)
	return NOT_OK;

    enum
    {
	API_ID_AnnIsRunning,
	API_ID_AnnGetMedia,
    };

    //STATUS API_CALL API_AnnIsRunning(CTask *task, int *ret, const char *ann_name);
    if (RegisterAPI( "ANNIsRunning", API_ID_AnnIsRunning, false, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &Announce_API::API_AnnIsRunning,
		     CAPIDef::CDECL_TYPE,
		     3,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "ret", BYREF, CVariant::VT_RET),
		     CArgType( "ann_name", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }

    //STATUS API_CALL API_AnnGetMedia(CTask *task, int *ret, const char *ann_name)
    if (RegisterAPI( "ANNGetMedia", API_ID_AnnGetMedia, false, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &Announce_API::API_AnnGetMedia,
		     CAPIDef::CDECL_TYPE,
		     3,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "ret", BYREF, CVariant::VT_RET),
		     CArgType( "ann_name", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }
    return OK;
}

STATUS Announce_API::Shutdown()
{
    API_Provider_Manager::Instance()->UnregisterProvider(this);
    return OK;
}

STATUS API_CALL Announce_API::API_AnnIsRunning(CTask *task, int *ret, const char *ann_name)
{
    *ret = 0;

    Announce_Mgr *mgr = Announce_Mgr::Instance();
    if (!mgr)
	return OK;

    Announce_Res *res= mgr->FindResource(ann_name);
    if (!res)
	return OK;

    *ret = res->is_running() ? 1 : 0;
    return OK;
}

STATUS API_CALL Announce_API::API_AnnGetMedia(CTask *task, int *ret, const char *ann_name)
{
    *ret = 0;

    Announce_Mgr *mgr = Announce_Mgr::Instance();
    if (!mgr) {
	CLog::Error(THISMODULE, task, "ANNGetMedia(%s): announcement subsystem is not running", ann_name);
	return OK;
    }

    Announce_Res *res= mgr->FindResource(ann_name);
    if (!res) {
	CLog::Error(THISMODULE, task, "ANNGetMedia: Unable to find announcement resource %s", ann_name);
	return OK;
    }

    *ret = (int) res->get_device();
    return OK;
}


///////////////////////////////////////////////////////////////////////////////
/*
 * ANNOUNCEMENT MANAGER
 */
enum
{
    Cmd_Announcement,
};

Announce_Mgr::Announce_Mgr()
: ServiceProvider("Announcement Manager", false, false), cfg_(NULL), api_(NULL)
{
    instance_ = this;
}

Announce_Mgr::~Announce_Mgr()
{
    instance_ = NULL;
}

Announce_Mgr *Announce_Mgr::Create()
{
    if (!instance_)
	new Announce_Mgr;
    
    return instance_;
}

Announce_Mgr *Announce_Mgr::Instance()
{
    return Create();
}


bool Announce_Mgr::Load()
{
    // Create API.
    api_ = new Announce_API;
    if (!api_->Init())
	return false;

    // Create root entry for 'module'
    cfg_ = new Cfg_Tree_Group( "announcement", "",
			       Kernel::Instance()->RootConfig());

    // Register terminal command for announcement configuration.
    //
    Term_Cmd *cmd;
    cmd = new Term_Cmd( this, Cmd_Announcement, "announcement", "Configure announcement",
			PrivilegeLevelAdmin, TermModeConfig, true );
    cmd->Add(new Term_Cmd_Arg("@0", CVariant(""), CVariantValidator("", "Announcement name")));
    RegisterTermCmd( cmd, Kernel::Instance()->RootConfig() );

    return true;
}

bool Announce_Mgr::Unload()
{
    if (api_) {
	api_->Shutdown();
	delete api_;
	api_ = NULL;
    }
    return true;
}

bool Announce_Mgr::CommitConfiguration()
{
    return true;
}

void Announce_Mgr::DoTermCmd(Term_Session *session, bool no, Term_Cmd *cmd)
{
    CVariant v0;

    switch (cmd->GetId()) {
    case Cmd_Announcement:
	cmd->GetArg((size_t)0)->GetValue(v0);

	if (!ACQUIRE_SYSTEM_LOCK())
	    return;

	Announce_Res **res_pos;
	res_pos = find_resource(v0.AsString());
	
	if (no) {
	    if (res_pos == res_.end()) {
		RELEASE_SYSTEM_LOCK();
		session->SendErrorf("The specified resource ('%s') is not found.\n", v0.AsString());
	    }
	    Announce_Res *res = *res_pos;
	    if (res->is_running()) {
		RELEASE_SYSTEM_LOCK();
		session->SendErrorf("Unable to release resource because it is running.\n");
		return;
	    }


	    res->Unload();
	    res_.erase(res_pos);
	    delete res;

	} else {
	    RELEASE_SYSTEM_LOCK();

	    if (res_pos == res_.end()) {
		bool yes;
		if (session->GetStream().IsInteractive()) {
		    yes = session->PromptYesNo(false, "Do you want to create new "
					       "announcement '%s'? [y/n] ", v0.AsString());
		} else {
		    yes = true;
		}
		if (!yes)
		    return;
	    }

	    if (!ACQUIRE_SYSTEM_LOCK())
		return;

	    res_pos = find_resource(v0.AsString());
	    Announce_Res *res;
	    if (res_pos == res_.end()) {
		res = new Announce_Res(v0.AsString());
		if (!res->Load()) {
		    RELEASE_SYSTEM_LOCK();
		    session->SendErrorf("Resource initialization error.\n");
		    delete res;
		    return;
		}

		res_.push_back(res);
	    } else
		res = *res_pos;

	    session->SetRoot(res->GetConfig());
	}

	RELEASE_SYSTEM_LOCK();
	break;
    }
}

Announce_Res **Announce_Mgr::find_resource(const char *name)
{
    Vector<Announce_Res *>::iterator it, end=res_.end();
    for (it = res_.begin(); it!=end; ++it) {
	Announce_Res *res = *it;
	if (!stricmp(res->get_name(), name))
	    return it;
    }
    return end;
}

Announce_Res *Announce_Mgr::FindResource(const char *name)
{
    Announce_Res **pos = find_resource(name);
    return pos == res_.end() ? NULL : *pos;
}

Cfg_Tree_Group *Announce_Mgr::GetCfgEntry()
{
    return cfg_;
}



#endif	/* XGATE_HAS_ANNOUNCE_RES */


