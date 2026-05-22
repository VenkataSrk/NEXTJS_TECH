/* $Header: /xGate/mapletree.cpp 34    12/07/04 2:28p Bennylp $
 *   
 * XGATE
 * (C) 2001 Switchlab, Ltd
 *
 * PPPoE framework
 *
 * $Log: /xGate/mapletree.cpp $
 * 
 * 34    12/07/04 2:28p Bennylp
 * Added dummy serial number registering
 * 
 * 33    12/15/03 6:12p Sonny
 * 
 * 32    10/16/03 12:56p Sonny
 * handle all upstream events if XGATE_HAS_MTNRTP is not defined
 * 
 * 31    6/06/03 4:07p Bennylp
 * Lots of changes to incorporate mapletree diagnostic (Version 54.4)
 * 
 * 30    6/04/03 3:44p Bennylp
 * Added mailbox status in show service output.
 * 
 * 29    4/10/03 11:14a Bennylp
 * Implement TDM disable RX/TX
 * 
 * 28    4/10/03 9:39a Bennylp
 * Changed switching to use preallocated addresses.
 * 
 * 27    4/08/03 9:27a Bennylp
 * Changed msg out statistics to use queue size
 * 
 * 26    4/04/03 11:59a Bennylp
 * Alloc and tx to TDM early when channel is created (cross-talk problem)
 * 
 * 25    2/19/03 11:30a Bennylp
 * Fixed errorneous dependency to XGATE_HAS_PPPOE
 * 
 * 24    12/06/02 11:51a Bennylp
 * Add TDM setting command.
 * 
 * 23    11/20/02 5:17p Bennylp
 * Remove DMA statistic
 * 
 * 22    11/04/02 7:02p Bennylp
 * Show resource name in switch device.
 * 
 * 21    11/04/02 5:29p Bennylp
 * Added info in show service Mapletree
 * 
 * 20    11/01/02 7:58p Bennylp
 * Bug fixes (47.1)
 * 
 * 19    10/31/02 4:55p Bennylp
 * Added hardware revision info.
 * 
 * 18    10/30/02 6:59p Bennylp
 * Moved switch device to header file.
 * 
 * 17    10/29/02 1:05p Bennylp
 * Stable version for MTN2100/1024P, tested.
 * 
 * 16    10/24/02 8:34p Bennylp
 * Latest stable checkin.
 * 
 * 15    10/23/02 5:01p Bennylp
 * Some more fixes
 * 
 * 14    10/21/02 6:40p Bennylp
 * Changes to adapt the new MTN framework changes.
 * 
 * 13    10/10/02 10:59a Bennylp
 * Added version in show status
 * 
 * 12    9/24/02 5:17p Bennylp
 * Added 'top' in MTN config
 * 
 * 11    9/24/02 5:07p Bennylp
 * Enable TDM configuration even when board is running.
 * 
 * 10    9/24/02 10:57a Bennylp
 * Added TDM command to module config.
 * 
 * 9     9/20/02 6:02p Bennylp
 * Completed Mapletree module configuration and status.
 * 
 * 8     8/19/02 10:55a Bennylp
 * Changed MTN SCSA API to require only timeslot.
 * 
 * 7     5/08/02 18:51 Adham
 * Mapletree_Module changed for scbus support 
 * 
 * 6     7/01/02 5:01p Bennylp
 * Changes related to new MTN library (5350)
 * 
 * 5     4/23/02 6:44p Bennylp
 * Supports for session ID managed object
 * 
 * 4     4/02/02 5:35p Bennylp
 * Added more info in 'show status/service'
 * 
 * 3     3/07/02 9:21p Bennylp
 * Support for PPPoE session ID, bunch of bug fixes, tested.
 * 
 * 2     2/19/02 12:07p Bennylp
 * Finished implementation of the new framework.
 * 
 * 1     2/15/02 3:36p Bennylp
 * Initial implementation
 * 
 * 1     2/08/02 6:10p Bennylp
 * Added to source control.
 */
#include "stdafx.h"
#include "xgconfig.h"

#if defined(XGATE_HAS_MTN)

#include <poll.h>
#include "../MTN_Lib/src/mtn_lib.h"
#include "../MTN_Lib/src/mtn_mod.h"
#include "../MTN_Lib/src/mtn_board.h"
#include "../MTN_Lib/src/mtn_mob.h"
#include "../MTN_Lib/src/mtn_msg.h"
#include "../MTN_Lib/src/mtn_channel.h"
#include "serviceprovider.h"
#include "mapletree.h"
#include "CTbusMgr.h"
#include "SCbusMgr.h"
#include "kernel.h"
#include "term.h"
#include "termcmd.h"
#include "licence.h"

#define THISMODULE	   "MTN"
Mapletree_Module_Manager * Mapletree_Module_Manager::instance_;

#ifdef XGATE_HAS_PPPOE
spt_sm_rec Mapletree_PPPoE_Channel::sm_;
#endif

class MTN_Module;
#define MTRACE		    Info

enum
{
    Cmd_Module = 1,
};

//////////////////////////////////////////////////////////////////////////////

static unsigned long GetTickCount()
{
  struct timeval tv;
  if( gettimeofday(&tv, NULL) != 0 )
  return 0;

  return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

void Sleep( unsigned long ms )
{
#ifndef __linux__
        ::Sleep(ms);
#else
        poll(0,0,ms); // __linux__
#endif
};


static const char *create_name(const char *name, MTN_Channel *ch)
{
    static char buf[128];
    sprintf(buf, "%s:%s", name, ch->get_channel_name());
    return buf;
}

Mapletree_Switch_Device::Mapletree_Switch_Device( const char *name, MTN_Channel *ch, 
						 SwitchFabric fabric, SwitchAddress *addr ) 
: SwitchDevice( create_name(name, ch) ), addr_(addr), channel_(ch),
  fabric_(fabric)
{
}

Mapletree_Switch_Device::~Mapletree_Switch_Device()
{
    /*
    free_address(fabric_);
    */
}

void Mapletree_Switch_Device::Initialize()
{
    /*
    assert(addr_ == NULL);
    if (addr_ == NULL) {
	alloc_address(fabric_);
	DeviceStartTransmit(fabric_, addr_);
    }
    */
    DeviceStartTransmit(fabric_, addr_);
}

void Mapletree_Switch_Device::DetachChannel()
{
    channel_ = NULL;
}

SwitchFabric Mapletree_Switch_Device::GetCompatibleFabric(SwitchDevice* dev) 
{
    if (fabric_ == SF_CTbus && dev->SupportCTbus(this))
	return SF_CTbus;
    else if (fabric_ == SF_SCbus && dev->SupportSCbus(this))
	return SF_SCbus;
    
    return SF_Invalid;
}

bool Mapletree_Switch_Device::SupportAculab(SwitchDevice* requester) 
{
    return false;
}

bool Mapletree_Switch_Device::SupportCTbus(SwitchDevice* requester) 
{
    return fabric_ == SF_CTbus;
}

bool Mapletree_Switch_Device::SupportSCbus(SwitchDevice* requester) 
{
    return fabric_ == SF_SCbus;
}

SwitchAddress* Mapletree_Switch_Device::AllocateAddress(SwitchFabric fabric) 
{
    return addr_;
}

void Mapletree_Switch_Device::FreeAddress(SwitchFabric fabric) 
{
    // Do nothing.
    // Address will be freed by destructor.
}

SwitchAddress* Mapletree_Switch_Device::GetAddress(SwitchFabric fabric) 
{
    return addr_;
}

void Mapletree_Switch_Device::DeviceStartTransmit(SwitchFabric fabric,SwitchAddress* a) 
{
#if 0
    if (!channel_)
	return;

    if (fabric_ == SF_CTbus) {
	CTbusAddress * addr = (CTbusAddress *) a;
	channel_->tdm_set_tx(addr->GetStream(), addr->GetTimeslot());
    } else {
	SCbusAddress * addr = (SCbusAddress *) a;
	channel_->tdm_set_tx(addr->GetTimeslot()/64, addr->GetTimeslot()%64);
    }
#endif
}

void Mapletree_Switch_Device::DeviceStartReceive(SwitchFabric fabric,SwitchAddress* a) 
{
    if (!channel_)
	return;

    if (fabric_ == SF_CTbus) {
	CTbusAddress * addr = (CTbusAddress *) a;
	channel_->tdm_set_rx(addr->GetStream(), addr->GetTimeslot());
    } else {
	SCbusAddress * addr = (SCbusAddress *) a;
	channel_->tdm_set_rx(addr->GetTimeslot()/64, addr->GetTimeslot()%64);
    }
}

void Mapletree_Switch_Device::DeviceStopTransmit(SwitchFabric fabric,SwitchAddress* a) 
{
    if (channel_) {
	int stream, tslot;

	if (fabric_ == SF_CTbus) {
	    CTbusAddress * addr = (CTbusAddress *) a;
	    stream = addr->GetStream();
	    tslot = addr->GetTimeslot();
	} else {
	    SCbusAddress * addr = (SCbusAddress *) a;
	    stream = addr->GetTimeslot() / 64;
	    tslot = addr->GetTimeslot() % 64;
	}
	channel_->tdm_disable_tx(stream, tslot);
    }
}

void Mapletree_Switch_Device::DeviceStopReceive(SwitchFabric fabric,SwitchAddress* a) 
{
    if (channel_) {
	int stream, tslot;

	if (fabric_ == SF_CTbus) {
	    CTbusAddress * addr = (CTbusAddress *) a;
	    stream = addr->GetStream();
	    tslot = addr->GetTimeslot();
	} else {
	    SCbusAddress * addr = (SCbusAddress *) a;
	    stream = addr->GetTimeslot() / 64;
	    tslot = addr->GetTimeslot() % 64;
	}
	channel_->tdm_disable_rx(stream, tslot);
    }
}

SwitchAddress *Mapletree_Switch_Device::alloc_address(SwitchFabric fabric)
{
    /*
    assert(addr_ == NULL);

    switch (fabric_) {
    case SF_CTbus:
	addr_ = CTbusManager::Instance()->AllocateAddress(GetName());
	break;
    case SF_SCbus:
	addr_ = SCbusManager::Instance()->AllocateAddress(GetName());
	break;
    }
    */
    return addr_;
}

void Mapletree_Switch_Device::free_address(SwitchFabric fabric)
{
    /*
    assert(addr_ != NULL);

    switch (fabric_) {
    case SF_CTbus:
	CTbusManager::Instance()->FreeAddress((CTbusAddress*)addr_);
	break;
    case SF_SCbus:
	SCbusManager::Instance()->FreeAddress((SCbusAddress*)addr_);
	break;
    }
    addr_ = NULL;
    */
}


//////////////////////////////////////////////////////////////////////////////
/** Mapletree PPPoE channel factory
 */
#ifdef XGATE_HAS_PPPOE
class Mapletree_PPPoE_Channel_Factory : public PPPoE_Channel_Factory {
public:
    Mapletree_PPPoE_Channel_Factory()
	: PPPoE_Channel_Factory("uam") {}

    virtual PPPoE_Channel * Allocate( const char *board_name, PPPoE_Pool *pool );
    virtual unsigned EnumBoardName( const char **board_name, unsigned maxnum );
};

/** Allocate a PPPoE_Channel from the specified board_name.
    The factory is responsible to do necessary pre-initialization for
    the channel, so that it is 'ready' to be used.
 */
PPPoE_Channel * 
Mapletree_PPPoE_Channel_Factory::Allocate( const char *board_name, 
					   PPPoE_Pool *pool )
{
    Mapletree_Module *module = Mapletree_Module_Manager::Instance()->FindModule(board_name);
    if (!module) {
	CLog::Error( THISMODULE, "Unable to find board '%s'", board_name);
	return NULL;
    }

    if (module->GetFreeChannel() == 0) {
	CLog::Error( THISMODULE, "Unable to create channel from board '%s': "
				 "no more channels available", board_name);
	return NULL;
    }

    MTN_Channel *mtnch = module->AllocateChannel();
    if (!mtnch) {
	CLog::Error( THISMODULE, "Unable to create channel from board '%s'", board_name);
	return NULL;
    }

    return new Mapletree_PPPoE_Channel( module, mtnch, pool );
}

/** Enumerate the list of boards that are available as the source of 
    PPPoE_Channel allocations.
 */
unsigned Mapletree_PPPoE_Channel_Factory::EnumBoardName( const char **board_name, 
						         unsigned maxnum )
{
    Mapletree_Module_Manager *mm = Mapletree_Module_Manager::Instance();
    int count = mm->GetModuleCount();
    count = maxnum < count ? maxnum : count;

    for (int i=0; i<count; ++i) {
	board_name[i] = mm->GetModule(i)->GetName();
    }

    return count;
}
#endif	/* XGATE_HAS_PPPOE */

//////////////////////////////////////////////////////////////////////////////

Mapletree_Module_Manager * Mapletree_Module_Manager::Create()
{
    if (!instance_)
	instance_ = new Mapletree_Module_Manager;
    return instance_;
}

Mapletree_Module_Manager * Mapletree_Module_Manager::Instance()
{
    return instance_;
}

Mapletree_Module_Manager::Mapletree_Module_Manager()
: ServiceProvider("Mapletree", false, false, Normal_Priority, "Mapletree Module Manager")
{
}

bool Mapletree_Module_Manager::Load()
{
    //
    // Create and initialize MTN_Library instance.
    // The MTN_Library will detect all the modules during initialize().
    //
    MTN_Library *lib = MTN_Library::create();
    if (!lib)
	return false;

    if (!lib->initialize()) {
	lib->shutdown();
	lib->destroy();
	return true;
    }

    // Create root entry for 'module'
    Cfg_Tree_Group *entry = new Cfg_Tree_Group( "module", "",
						Kernel::Instance()->RootConfig());
    cfg_entry_ = entry;

    //entry->RegisterShowHandler( this );
    //Kernel::Instance()->RootConfig()->RegisterShowHandler( this );

    //
    // Get the detected modules, and create an instance of 
    // Mapletree_Module for each of them.
    //
    int i;
    for (i=0; i<lib->get_board_count(); ++i) {
	Mapletree_Module *mod = new Mapletree_Module( lib->get_board(i) );
	modules_.push_back(mod);
    }

    // Make sure PPPoE_Manager has been created
#ifdef XGATE_HAS_PPPOE
    PPPoE_Manager *pppoe = PPPoE_Manager::Create();

    // Create and register channel factory
    pppoe->RegisterFactory( new Mapletree_PPPoE_Channel_Factory, true );

    // Initialize Mapletree PPPOE state machine
    Mapletree_PPPoE_Channel::InitStateMachine();
#endif

    // Register terminal command for module configuration.
    //
    Term_Cmd *cmd;
    cmd = new Term_Cmd( this, Cmd_Module, "module", "Configure Mapletree module",
			PrivilegeLevelAdmin, TermModeConfig, true );
    CVariantChoiceValidator mod_vdtor("", "");
    for (int i_mod=0; i_mod<modules_.size(); ++i_mod) {
	Mapletree_Module *mod = modules_[i_mod];
	char buf[80];
	sprintf(buf, "Configure Mapletree module '%s'", mod->GetName());
	mod_vdtor.Add(mod->GetName(), buf);
    }
    cmd->Add(new Term_Cmd_Arg("@0", CVariant(""), mod_vdtor));
    RegisterTermCmd( cmd, Kernel::Instance()->RootConfig() );


    return true;
}

bool Mapletree_Module_Manager::Unload()
{
    MTN_Library *lib = MTN_Library::instance();
    if (lib) {
	lib->shutdown();
	lib->destroy();
    }
    return true;
}

Mapletree_Module * Mapletree_Module_Manager::FindModule( const char *name )
{
    int i;
    for (i=0; i<GetModuleCount(); ++i) {
	Mapletree_Module *mod = GetModule(i);
	assert(mod);
	if (!strcasecmp(mod->GetName(), name))
	    return mod;
    }
    return NULL;
}

static const char *get_board_status(MTN_Board *board)
{
    if (board->is_running()) {
	if (board->get_total_port_count() != board->get_capacity())
	    return "RUNNING PARTIALLY";
	else
	    return "RUNNING";
    } else {
	return "NOT RUNNING";
    }
}

bool Mapletree_Module_Manager::OnShowStatus(String & output)
{
    char temp[120];

    MTN_Library *lib = MTN_Library::instance();
    output += "Mapletree Module Manager\n";
    if (!lib) {
	output += "  No Mapletree modules detected\n";
    } else {
	output += "  BOARD(S) SUMMARY:\n";
	output += "  No. Slot Name  Type     UAM Tot. Cap. Free Used TDM  Board Status\n";
	output += "  ======================================================================\n";

	MTN_TDM_Status tdm_status;

	for (int i=0; i<lib->get_board_count(); ++i) {
	    MTN_Board *board = lib->get_board(i);
	    board->tdm_get_status(&tdm_status);
	    
	    sprintf(temp, "   %-2d %-4d %s %-8s %-3d %-4d %-4d %-4d %-4d %s %s\n",
		i, 
		board->get_board_module()->get_slot(),
		board->get_name(),
		board->get_board_module()->get_type_name(),
		board->get_module_count(),
		board->get_total_port_count(),
		board->get_capacity(),
		board->get_free_count(),
		(board->get_capacity() - board->get_free_count()),
		(tdm_status.error_ == 0 ? " UP " : "DOWN"),
		get_board_status(board));

	    output += temp;
	}

	output += "\n";
    }
    return true;
}

void Mapletree_Module_Manager::DoTermCmd(Term_Session *session, bool no, Term_Cmd *cmd)
{
    Mapletree_Module *mod = NULL;
    CVariant arg0;

    switch (cmd->GetId()) {
    case Cmd_Module:
	cmd->GetArg((size_t)0)->GetValue(arg0);
	mod = FindModule(arg0.AsString());
	if (mod) {
	    session->SetRoot(mod->GetConfigTree());
	} else
	    session->SendErrorf("Module %s not found", arg0.AsString());
	break;
    }
}

size_t Mapletree_Module_Manager::GetModuleCount( ) const
{
  //  return modules_.size(); // simply commented 
}

Mapletree_Module * Mapletree_Module_Manager::GetModule( size_t index )
{
    if (index >= modules_.size()) {
	assert(false);
	return NULL;
    }
    return modules_[index];
}

//////////////////////////////////////////////////////////////////////////////
/** This class provides an interface and implementation for a single Mapletree
    board that is detected in the system.
 */

enum {
    Cmd_IP_Addr,
    Cmd_Tdm,
    Cmd_Module_Mask,
    Cmd_Firmware,
    Cmd_Status,
    Cmd_Shutdown,
};


Mapletree_Module::Mapletree_Module( MTN_Board * board )
: ServiceProvider( board->get_name(), false, true ), board_(board),
  has_eth_(false), enabled_(false), running_(false), tdm_mode_(MTN_TDM_CT_A_SLAVE),
  country_code_(CountryCode_INTERNAT_ALAW), cfg_entry_(NULL)
{
}

void str_replace_newline(String & str)
{
    size_t pos=0;
    while ((pos=str.find('\r', pos)) != String::npos) {
	str.replace(pos, 1, 1, ',');
	++pos;
    }
    pos = 0;
    while ((pos=str.find('\n', pos)) != String::npos) {
	str.replace(pos, 1, "\n                ");
	++pos;
    }
}

bool Mapletree_Module::Load()
{
    //
    // Register terminal commands
    //
    Cfg_Tree_Group *entry = new Cfg_Tree_Group( GetName(), "",
						Mapletree_Module_Manager::Instance()->GetCfgEntry());
    entry->RegisterShowHandler( this );
    Kernel::Instance()->RootConfig()->RegisterShowHandler( this );
    cfg_entry_ = entry;

    Term_Cmd *cmd;
    int i;

    //
    // IP address command
    //
    if (has_eth_) {
	CVariantChoiceValidator ipv( "address", "Configure IP address/mask" );
	ipv.Add( "gw", "Configure default IP gateway");

	cmd = new Term_Cmd( this, Cmd_IP_Addr, "ip", "IP configuration",
			    PrivilegeLevelAdmin, TermModeConfig, true);
	cmd->Add( new Term_Cmd_Arg("@0", CVariant(""), ipv) );
	cmd->AddDynamic( 1, 0, CVariant("adresss"), new Term_Cmd_Arg("@1", CVariant(""), CVariantValidator("", "IP address")) );
	cmd->AddDynamic( 2, 0, CVariant("address"), new Term_Cmd_Arg("@2", CVariant(""), CVariantValidator("", "Subnet mask")) );
	cmd->AddDynamic( 1, 0, CVariant("gw"), new Term_Cmd_Arg("@1", CVariant(""), CVariantValidator("", "IP address of the default gateway")) );
	RegisterTermCmd( cmd, entry );
    }

    //
    // TDM command
    //
    cmd = new Term_Cmd( this, Cmd_Tdm, "tdm", "TDM configuration",
			PrivilegeLevelAdmin, TermModeConfig, false);

    CVariantChoiceValidator vtdm("bus-type", "Configure TDM bus type");
    vtdm.Add( "law", "Set TDM ALAW/ULAW settings");
    cmd->Add( new Term_Cmd_Arg( "@0", CVariant(""), vtdm) );

    CVariantChoiceValidator vbustype("ctbus-master", "H.100/H.110 CT_A Master");
    vbustype.Add("ctbus-slave", "H.100/H.110 CT_A Slave");
    vbustype.Add("scbus-master", "SCSA bus master");
    vbustype.Add("scbus-slave", "SCSA bus slave");
    vbustype.Add("stand-alone", "No TDM bus");
    cmd->AddDynamic( 1, 0, "bus-type", new Term_Cmd_Arg( "@1", CVariant(""), vbustype));
   
    CVariantChoiceValidator vlaw("alaw", "ALAW");
    vlaw.Add("ulaw", "ULAW (United States)");
    cmd->AddDynamic( 1, 0, "law", new Term_Cmd_Arg("@1", CVariant(""), vlaw));

    RegisterTermCmd( cmd, entry );

    //
    // module-mask command.
    //
    cmd = new Term_Cmd( this, Cmd_Module_Mask, "module-mask", "Module enable mask",
			PrivilegeLevelAdmin, TermModeConfig, false);
    cmd->Add( new Term_Cmd_Arg( "@0", CVariant((int)0xffffffff), CVariantRangeValidator(0, 255, "Mask")));
    RegisterTermCmd(cmd, entry);
    

    //
    // firmware command.
    //
    int mod_types[UaaDevType_END_OF_ENUM];

    memset(mod_types, 0, sizeof(mod_types));
    mod_types[board_->get_board_module()->get_type()] = 1;
    for (i=0; i<board_->get_module_count(); ++i) {
	MTN_Module *mod = board_->get_module(i);
	mod_types[mod->get_type()] = 1;
    }

    cmd = new Term_Cmd( this, Cmd_Firmware, "firmware", "Set the firmware path",
			PrivilegeLevelAdmin, TermModeConfig, true);
    CVariantChoiceValidator vmod_type( MTN_Util::mod_type_name(mod_types[board_->get_board_module()->get_type()]), 
				       "Set firmware path for this module type");
    for (i=0; i<UaaDevType_END_OF_ENUM; ++i) {
	if (mod_types[i] && i!=board_->get_board_module()->get_type())
	    vmod_type.Add(MTN_Util::mod_type_name(i), "Set firmware path for this module type");
    }
    cmd->Add(new Term_Cmd_Arg( "@0", CVariant(""), vmod_type));
    cmd->Add(new Term_Cmd_Arg( "@1", CVariant(""), CVariantValidator("", "Firmware path"), TermArgOptional));
    RegisterTermCmd(cmd, entry);

    //
    // Status command.
    //
    cmd = new Term_Cmd( this, Cmd_Status, "status", "Show status",
			PrivilegeLevelAdmin, TermModeConfig, false );
    RegisterTermCmd( cmd, entry );

    //
    // Shutdown command.
    //
    cmd = new Term_Cmd( this, Cmd_Shutdown, "shutdown", "Shutdown the module",
			PrivilegeLevelAdmin, TermModeConfig, true );
    RegisterTermCmd( cmd, entry );
#ifndef __linux__
    InitializeCriticalSection(&upstream_mutex_);
#endif

    return true;
}

bool Mapletree_Module::Unload()
{
    enabled_ = running_ = false;
    board_->shutdown();

    Map<unsigned,SwitchAddress*>::iterator it, end;
    end = tdm_addr_map_.end();
    for (it=tdm_addr_map_.begin(); it!=tdm_addr_map_.end(); ++it) {
	SwitchAddress *addr = it->second;
	switch (GetTdmBusType()) {
	case SF_CTbus:
	    CTbusManager::Instance()->FreeAddress((CTbusAddress*)addr);
	    break;
	case SF_SCbus:
	    SCbusManager::Instance()->FreeAddress((SCbusAddress*)addr);
	    break;
	}
    }
    tdm_addr_map_.clear();
#ifndef __linux__
    DeleteCriticalSection(&upstream_mutex_);
#endif
    
    return true;
}

bool Mapletree_Module::acquire_upstream_mutex()
{
#ifndef __linux__
    EnterCriticalSection(&upstream_mutex_);
#endif
    return true;
}

bool Mapletree_Module::release_upstream_mutex()
{
#ifndef __linux__
    LeaveCriticalSection(&upstream_mutex_);
#endif
    return true;
}



SwitchAddress* Mapletree_Module::GetTdmAddress(unsigned module_id, unsigned ch_id)
{
    unsigned id = (module_id << 16) | (ch_id & 0xFFFF);
    SwitchAddress *addr = tdm_addr_map_[id];
    if (addr == NULL) {
	char request_description[32];
	sprintf(request_description, "uam%d[%d]", module_id, ch_id);

	switch (GetTdmBusType()) {
	case SF_CTbus:
	    addr = CTbusManager::Instance()->AllocateAddress(request_description);
	    break;
	case SF_SCbus:
	    addr = SCbusManager::Instance()->AllocateAddress(request_description);
	    break;
	}
    }
    tdm_addr_map_[id] = addr;
    return addr;
}

void Mapletree_Module::HandleEvents()
{
    enum 
    {
	FLAG = MTN_Module::HANDLE_Q_CMD | 
	       MTN_Module::HANDLE_Q_DATA | 
	       MTN_Module::HANDLE_Q_TRAP
    };

    if (enabled_ && running_ && board_->is_running()) {
#ifdef XGATE_HAS_MTNRTP
	board_->handle_events(FLAG);
#else
	board_->handle_events(MTN_Module::HANDLE_Q_ALL);
#endif
    }
}

void Mapletree_Module::HandleData()
{
    if (enabled_ && running_ && board_->is_running())
	board_->handle_events(MTN_Module::HANDLE_Q_UP_DATA);
}

bool Mapletree_Module::StartModule()
{
    //
    // Disable the board, to make sure.
    // Also this will prevent the main thread from polling the board.
    //
    enabled_ = false;
    running_ = false;

    //
    // If board is currently running, shut it down first to make sure that
    // the configuration is updated.
    //
    if (board_->is_running())
	board_->shutdown();

    //
    // Initialize and start the board.
    //
    if (!board_->initialize())
	return false;

    // Configure TDM.
    if (!board_->tdm_configure(tdm_mode_)) {
	board_->shutdown();
	return false;
    }

    //
    // Get the underlying module.
    //
    MTN_Module *board_module = board_->get_board_module();
    MTN_Module *first_module = NULL;
    
    if (board_module->get_total_port_count() > 0)
	first_module = board_module;
    else {
	int i, count = board_->get_module_count();
	for (i=0; i<count; ++i) {
	    MTN_Module *module = board_->get_module(i);
	    if (module && module->is_running() && module->get_total_port_count() > 0) {
		first_module = module;
		break;
	    }
	}
    }

    bool status;
    MTN_Event_Notification event;

    //
    // Get software versions loaded.
    //
    if (first_module) {
	MTN_System_Version_String_Mob_Data version_mob;

	version_mob.set_user_notification(&event, false);
	status = first_module->get_mob_async(0, &version_mob);

	if (status)
	    status = WaitEvent(&event, 2);
	    
	if (status && event.get_status()==0) {
	    risc_version_.assign(version_mob.get_risc_version_string(),
				 version_mob.get_risc_version_string_len());
	    dsp_version_.assign(version_mob.get_dsp_version_string(),
				version_mob.get_dsp_version_string_len());
	    str_replace_newline(risc_version_);
	    str_replace_newline(dsp_version_);
	} else {
	    board_->shutdown();
	    return false;
	}
    } else {
	risc_version_ = dsp_version_ = "Error: no modules loaded";
	board_->shutdown();
	return false;
    }

    
    //
    // Determine whether the module has an Ethernet interface in it
    // by getting the IP and Ethernet settings from the board.
    //
    ip_setting_.set_user_notification( &event );
    has_eth_ = board_module->get_mob_async(0, &ip_setting_);
    if (has_eth_)
	has_eth_ = WaitEvent(&event, 2);
    has_eth_ = has_eth_ && event.get_status() == 0;

    ip_setting_.set_user_notification( NULL );
    if (has_eth_) {
	eth_addr_.set_user_notification( &event );
	has_eth_ = board_module->get_mob_async( 0, &eth_addr_ );
	if (has_eth_)
	    has_eth_ = WaitEvent(&event, 2);
	has_eth_ = has_eth_ && event.get_status() == 0;
    }

    if (has_eth_) {
	eth_status_.set_user_notification( &event );
	has_eth_ = board_module->get_mob_async( 0, &eth_status_ );
	if (has_eth_)
	    has_eth_ = WaitEvent(&event, 2);
	has_eth_ = has_eth_ && event.get_status() == 0;
    }

    //
    // Done.
    //
    enabled_ = running_ = true;
    return true;
}

bool Mapletree_Module::StopModule()
{
    enabled_ = false;
    running_ = false;
    board_->shutdown();
    return true;
}

bool Mapletree_Module::CommitConfiguration()
{
#ifdef XGATE_HAS_LM
    /* Mapletree has no serial number, so we just register zeroes. */
    char serno[20];
    sprintf(serno, "MTN%02d", board_->get_board_id());
    LicenceManager::Instance()->RegisterSerialNum(Fg_Mapletree, board_->get_board_id(), 
						  serno, strlen(serno));
#endif

    if (enabled_ && !running_)
	StartModule();

    return true;
}

bool Mapletree_Module::OnShowStatus(String & output)
{
    int i;
    char temp[128];
    MTN_Module *board_module = board_->get_board_module();

    sprintf( temp, "  Hardware is %s in PCI bus %d slot %d, status is %s\n",
		   board_module->get_type_name(), 
		   board_module->get_pci_bus(),
		   board_module->get_slot(),
		   (enabled_ ? "running" : "disabled"));
    output += temp;

    //sprintf(temp, "  Hardware revision number: %d\n", board_module->get_hw_rev_info());
    //output += temp;
    output += "  RISC version: " + risc_version_ + "\n";
    output += "  DSP  version: " + dsp_version_ + "\n";

    sprintf( temp, "  Total Channel count %d, free: %d, used: %d\n",
		   board_->get_capacity(),
		   board_->get_free_count(),
		   (board_->get_capacity() - board_->get_free_count()));
    output += temp;

    //
    // Show individual module status and get module statistic.
    //
    
    board_module = board_->get_board_module();
    int count = board_->get_module_count();
    if (count) {
	output += "  Submodules:\n";
	sprintf(temp, "  Slot Name  Type    Rev Totl Free Used Status\n  ");
	output += temp;
	output.append(76, '-');
	output += "\n";

	for (i=0; i<count; ++i) {
	    MTN_Module *mod = board_->get_module(i);
	    MTN_Module::Statistic stat;
	    mod->get_statistic(&stat);

	    sprintf(temp, "  %-4d %-5s %-8s %-2d %-4d %-4d %-4d %s (mbox=%d)\n",
		    mod->get_slot(),
		    mod->get_mod_name(),
		    mod->get_type_name(),
		    mod->get_hw_rev_info(),
		    mod->get_capacity(),
		    mod->get_free_count(),
		    (mod->get_capacity() - mod->get_free_count()),
		    (mod->is_running() ? "running" : "not running"),
		    mod->get_mailbox_value());
	    output += temp;

	    output.append(40,' ');
	    sprintf(temp, "Msg: cmd=%u,dta=%u,out=%d,blog=%d\n",
		    stat.cmd_pool_free, stat.data_pool_free,
		    stat.cmd_pool_capacity + stat.data_pool_capacity - 
		      (stat.cmd_pool_free + stat.data_pool_free), 
		    stat.backlog_size);
	    output += temp;
	}
    }


    //
    // Show TDM settings.
    //
    MTN_TDM_Status tdm_status;
    board_module->tdm_get_status(&tdm_status);

    const char *clk_src;
    const char *clk_mode;

    switch (tdm_status.source_) {
    case MTN_TDM_SOURCE_UNKNOWN: clk_src = "UNKNOWN"; break;
    case MTN_TDM_LOCAL: clk_src = "LOCAL"; break;
    case MTN_TDM_CT_A:  clk_src = "CT_A"; break;
    case MTN_TDM_CT_B:  clk_src = "CT_B"; break;
    case MTN_TDM_SCBUS: clk_src = "SCBUS"; break;
    default: 		clk_src = "UNKNOWN"; break;
    }

    switch (tdm_status.mode_) {
    case MTN_TDM_STAND_ALONE:	clk_mode = "STAND-ALONE"; break;
    case MTN_TDM_CT_A_MASTER:	clk_mode = "CT_A PRIMARY MASTER"; break;
    case MTN_TDM_CT_B_MASTER:	clk_mode = "CT_B PRIMARY MASTER"; break;
    case MTN_TDM_CT_A_SLAVE:	clk_mode = "CT_A SLAVE"; break;
    case MTN_TDM_CT_B_SLAVE:	clk_mode = "CT_B SLAVE"; break;
    case MTN_TDM_SCBUS_MASTER:	clk_mode = "SCBUS MASTER"; break;
    case MTN_TDM_SCBUS_SLAVE:	clk_mode = "SCBUS SLAVE"; break;
    default:
	clk_mode = "UNKNOWN"; break;
    }
    sprintf(temp, "  TDM Clock source: %s, clock mode: %s\n",
		  clk_src, clk_mode);
    output += temp;

    sprintf(temp, "      Clock rate=%d KHz, CT auto switch is %s\n", 
		  1024*(1 << (tdm_status.clock_rate_+1)),
		  (tdm_status.ct_slave_auto_switch_ ? "ENABLED" : "DISABLED"));
    output += temp;

    if (tdm_status.error_== 0) {
	sprintf(temp, "      Clock status is UP\n");
    } else {
	sprintf(temp, "      Error(s): %s %s %s\n",
		      (tdm_status.error_ & MTN_TDM_CT_A_ERROR ? "CT_A_Down " : ""),
		      (tdm_status.error_ & MTN_TDM_CT_B_ERROR ? "CT_B_Down " : ""),
		      (tdm_status.error_ & MTN_TDM_SCBUS_ERROR ? "SCBUS_Down " : ""));
    }
    output += temp;


    //
    // Show Ethernet settings.
    //
    if (has_eth_) {
	MTN_Ethernet_Mob_Data eth;
	MTN_Event_Notification event;
	eth.set_user_notification( &event, false );

	if (!board_module->get_mob_async( 0, &eth )) {
	    output += "Error in getting Ethernet status\n";
	    return true;
	}

	RELEASE_SYSTEM_LOCK();
	bool signalled = WaitEvent(&event, 3);
	if (!ACQUIRE_SYSTEM_LOCK())
	    return true;

	if (!signalled) {
	    output += "Timeout in getting Ethernet status\n";
	    return true;
	}

	const unsigned char *addr = eth_addr_.get_mac_addr();
	sprintf( temp, "  Ethernet interface HW addr: %02x-%02x-%02x-%02x-%02x-%02x\n",
		       addr[0] & 0xFF, addr[1] & 0xFF, addr[2] & 0xFF, 
		       addr[3] & 0xFF, addr[4] & 0xFF, addr[5] & 0xFF);
	output += temp;

	sprintf( temp, "  Physical connection is %s, speed is %d Mbps, MRU/MTU %d bytes\n",
		       (eth.get_net_sync() ? "UP" : "DOWN"),
		       (eth.get_net_speed() ? 100 : 10),
		        eth.get_mru_mtu());
	output += temp;
    }

    return true;
}

const char * Mapletree_Module::GetName() const
{
    return board_->get_name();
}

size_t Mapletree_Module::GetFreeChannel() const
{
    return board_->get_free_count();
}

SwitchFabric Mapletree_Module::GetTdmBusType() const
{
    switch (tdm_mode_) {
    case MTN_TDM_CT_A_MASTER:
    case MTN_TDM_CT_B_MASTER:
    case MTN_TDM_CT_A_SLAVE:
    case MTN_TDM_CT_B_SLAVE:
	return SF_CTbus;
    case MTN_TDM_SCBUS_MASTER:
    case MTN_TDM_SCBUS_SLAVE:
	return SF_SCbus;
    default:
	return SF_Invalid;
    }
}

size_t Mapletree_Module::GetUsedChannel() const
{
    return board_->get_capacity() - board_->get_free_count();
}

MTN_Channel *Mapletree_Module::AllocateChannel()
{
    if (!enabled_) 
	return NULL;

    return board_->allocate_channel( MTN_Module::Round_Robin );
}

void Mapletree_Module::ReleaseChannel( MTN_Channel *ch )
{
    board_->release_channel(ch);
}

bool Mapletree_Module::GetIpSetting( MTN_Ip_Setting_Mob_Data * setting )
{
    return has_eth_ ? 
	( memcpy(setting, &ip_setting_, sizeof(ip_setting_)), true) : false;
}

bool Mapletree_Module::GetEthernetInfo( MTN_Ethernet_Mob_Data * info )
{
    return has_eth_ ?
	( memcpy(info, &eth_status_, sizeof(*info)), true) : false;
}

bool Mapletree_Module::WaitEvent(MTN_Event_Notification *event, int sec_timeout)
{
    int wait_count=0;
    bool signalled = false;
    bool passive_wait = ACE_Thread::self() != Kernel::Instance()->GetMainThreadId();
    unsigned timeout = GetTickCount() + sec_timeout * 1000;

    if (!enabled_)
	passive_wait = false;

    do {
	if (passive_wait)
	    ::Sleep(5);
	else
	    board_->handle_events();
    } while ((signalled=event->is_signalled())==false && GetTickCount() < timeout);

    return signalled;
}

void Mapletree_Module::DoTermCmd(Term_Session *session, bool no, Term_Cmd *cmd)
{
    MTN_Ip_Setting_Mob_Data ip;
    CVariant v0, v1;
    bool result = true;
    MTN_Event_Notification event;
    String output;

    switch (cmd->GetId()) {
    case Cmd_Module_Mask:
	cmd->GetArg((int)0)->GetValue(v0);
	board_->set_module_mask(v0.AsInt());
	break;

    case Cmd_Tdm:
	cmd->GetArg((int)0)->GetValue(v0);
	cmd->GetArg((int)1)->GetValue(v1);
	
	MTN_TDM_Clock_Mode mode;
	mode = MTN_TDM_MODE_UNKNOWN;

	if (!strcasecmp(v0.AsString(), "bus-type")) {
	    if (!strcasecmp(v1.AsString(), "ctbus-master")) {
		mode = MTN_TDM_CT_A_MASTER;
	    } else if (!strcasecmp(v1.AsString(), "ctbus-slave")) {
		mode = MTN_TDM_CT_A_SLAVE;
	    } else if (!strcasecmp(v1.AsString(), "scbus-master")) {
		mode = MTN_TDM_SCBUS_MASTER;
	    } else if (!strcasecmp(v1.AsString(), "scbus-slave")) {
		mode = MTN_TDM_SCBUS_SLAVE;
	    } else if (!strcasecmp(v1.AsString(), "stand-alone")) {
		mode = MTN_TDM_STAND_ALONE;
	    } else {
		mode = MTN_TDM_MODE_UNKNOWN;
		session->SendErrorf("Invalid TDM mode '%s'\n", v1.AsString());
	    }
	    
	    if (mode != MTN_TDM_MODE_UNKNOWN) {
		if (enabled_ || GetUsedChannel()) {
		    CLog::Error(THISMODULE,"Unable to change settings while board is running");
		    session->SendErrorf("Unable to change settings while board is running\n");
		    return;
		}

		ACQUIRE_SYSTEM_LOCK();
		assert(!enabled_);
		tdm_mode_ = mode;
		RELEASE_SYSTEM_LOCK();
	    }
	} else if (!strcasecmp(v0.AsString(), "law")) {
	    ACQUIRE_SYSTEM_LOCK();
	    if (!strcasecmp(v1.AsString(), "alaw")) {
		country_code_ = CountryCode_INTERNAT_ALAW;
	    } else {
		country_code_ = CountryCode_INTERNAT_MULAW;
	    }
	    RELEASE_SYSTEM_LOCK();
	}
	break;

    case Cmd_Firmware:
	cmd->GetArg((size_t)0)->GetValue( v0 );
	MTN_Module::Type mod_type;

	mod_type = (MTN_Module::Type) MTN_Util::mod_type(v0.AsString());
	if (mod_type == (MTN_Module::Type)-1) {
	    session->SendErrorf("Invalid module type\n");
	    return;
	}

	if (!cmd->GetArg((size_t)1)->HasValue()) {
	    no = true;
	}

	if (no) {
	    if (!ACQUIRE_SYSTEM_LOCK())
		return;
	    board_->set_firmware_path( mod_type, NULL );
	    RELEASE_SYSTEM_LOCK();
	} else {
	    cmd->GetArg((size_t)1)->GetValue( v1 );
	    if (!ACQUIRE_SYSTEM_LOCK())
		return;
	    board_->set_firmware_path(mod_type, v1.AsString());
	    RELEASE_SYSTEM_LOCK();
	}
	break;

    case Cmd_IP_Addr:
	cmd->GetArg((size_t)0)->GetValue( v0 );
	if (!strcasecmp(v0.AsString(), "address")) {
	    CVariant vaddr, vmask;
	    cmd->GetArg((size_t)1)->GetValue( vaddr );
	    cmd->GetArg((size_t)2)->GetValue( vmask );
	
	    if (!ACQUIRE_SYSTEM_LOCK()) {
		session->SendErrorf( "Unable to get system lock\n");
		return;
	    }

	    result = GetIpSetting(&ip);
	    RELEASE_SYSTEM_LOCK();

	    ip.set_ip_addr( vaddr.AsString() );
	    ip.set_ip_mask( vmask.AsString() );

	} else if (!strcasecmp(v0.AsString(), "gw")) {
	    CVariant vgw;
	    cmd->GetArg(1)->GetValue( vgw );

	    if (!ACQUIRE_SYSTEM_LOCK()) {
		session->SendErrorf( "Unable to get system lock\n");
		return;
	    }

	    result = GetIpSetting(&ip);
	    RELEASE_SYSTEM_LOCK();

	    ip.set_default_gw( vgw.AsString() );
	}

	ip.set_user_notification( &event );

	if (!ACQUIRE_SYSTEM_LOCK()) {
	    session->SendErrorf( "Unable to get system lock\n");
	    return;
	}

	MTN_Module *board_module;
	board_module = board_->get_board_module();
	result = result && board_module->set_mob_async( 0, &ip);
	RELEASE_SYSTEM_LOCK();

	if (result) {
	    result = WaitEvent(&event, 2);
	    result = result && event.get_status() == 0;
	}

	if (result) {
	    ACQUIRE_SYSTEM_LOCK();
	    memcpy( &ip_setting_, &ip, sizeof(ip));
	    RELEASE_SYSTEM_LOCK();
	} else
	    session->SendErrorf("Error in setting up IP parameters: %s\n",
				MTN_Util::msg_status_name(event.get_status()));

	break;

    case Cmd_Status:
	if (!ACQUIRE_SYSTEM_LOCK())
	    return;

	OnShowStatus(output);
	RELEASE_SYSTEM_LOCK();
	session->Send(output.c_str(), output.size());
	break;

    case Cmd_Shutdown:
	if (no) {
	    if (enabled_ && running_)
		session->SendErrorf("Board is already started.\n");
	    else if (!Kernel::Instance()->IsInRunningState()) {
		enabled_ = true;
	    } else {
		bool status = StartModule();
		if (!status)
		    session->SendErrorf("Unable to start module\n");
	    }
	} else {
	    if (GetUsedChannel() > 0)
		session->SendErrorf("Unable to shutdown while some channels are used.\n");
	    else {
		ACQUIRE_SYSTEM_LOCK();
		StopModule();
		RELEASE_SYSTEM_LOCK();
	    }
	}
	break;

    }
}

bool Mapletree_Module::OnShowConfig( const Cfg_Tree_Entry *entry,
				       String & output, 
				       const String & indent, 
				       int indent_size) const
{
    String newindent(indent), newline("\n");
    newindent.append( indent_size, ' ');
    char temp[80];

    ACQUIRE_SYSTEM_LOCK();
    output += indent + "module " + GetName() + newline;
    // IP setting.
    if (has_eth_) {
	if ( inet_addr(ip_setting_.get_ip_addr()) == inet_addr("127.0.0.1"))
	    output += newindent + "no ip address" + newline;
	else {
	    String addr = ip_setting_.get_ip_addr();
	    String mask = ip_setting_.get_ip_mask();
	    output += newindent + "ip address " + addr + " " + mask + newline;
	    output += newindent + "ip gw " + ip_setting_.get_default_gw() + newline;
	}
    }
    // Firmware paths
    int i;
    for (i=0; i<UaaDevType_END_OF_ENUM; ++i) {
	const char *fw = board_->get_firmware_path((MTN_Module::Type)i);
	if (fw) {
	    output += newindent + "firmware " + MTN_Util::mod_type_name(i) + " \"" + fw + "\"" + newline;
	}
    }

    // Module mask.
    sprintf(temp, "%u", board_->get_module_mask());
    output += newindent + "module-mask " + temp + newline;

    // TDM setting.
    output += newindent + "tdm bus-type ";
    switch (tdm_mode_) {
    case MTN_TDM_MODE_UNKNOWN:	output += "??\n"; break;
    case MTN_TDM_STAND_ALONE:	output += "stand-alone\n"; break;
    case MTN_TDM_CT_A_MASTER:	output += "ctbus-master\n"; break;
    case MTN_TDM_CT_B_MASTER:	output += "ctbus-b-master\n"; break;
    case MTN_TDM_CT_A_SLAVE:	output += "ctbus-slave\n"; break;
    case MTN_TDM_CT_B_SLAVE:	output += "ctbus-b-slave\n"; break;
    case MTN_TDM_SCBUS_MASTER:	output += "scbus-master\n"; break;
    case MTN_TDM_SCBUS_SLAVE:	output += "scbus-slave\n"; break;
    }

    // Alaw/Ulaw
    output += newindent + "tdm law " + 
	      (country_code_==CountryCode_INTERNAT_ALAW ? "alaw" : "ulaw") + 
	      "\n";

    // Shutdown
    if (enabled_)
	output += newindent+ "no shutdown" + newline;
    else
	output += newindent+ "shutdown" + newline;

    output += newindent+ "top" + newline;
    output += newline;

    RELEASE_SYSTEM_LOCK();
    return OK;
}




//////////////////////////////////////////////////////////////////////////////

Mapletree_Channel::Mapletree_Channel( Mapletree_Module *mod, MTN_Channel * channel )
: module_(mod), channel_(channel)
{
}

Mapletree_Channel::~Mapletree_Channel()
{
    ReleaseChannel();
}

SwitchDevice * Mapletree_Channel::CreateSwitchDevice(const char *name)
{
    Mapletree_Switch_Device *dev;
    Mapletree_Module *mod = GetModule();

    unsigned ch_id = channel_->get_channel_id();
    unsigned mod_id = channel_->get_module()->get_mod_id();

    dev = new Mapletree_Switch_Device( name, channel_, 
				       GetModule()->GetTdmBusType(),
				       GetModule()->GetTdmAddress( mod_id, ch_id ));
    dev->Initialize();
    return dev;
}

MTN_Channel * Mapletree_Channel::GetChannel()
{
    return channel_;
}

const MTN_Channel * Mapletree_Channel::GetChannel() const
{
    return channel_;
}

void Mapletree_Channel::ReleaseChannel()
{
    if (channel_) {
	module_->ReleaseChannel( channel_ );
	channel_ = NULL;
    }
}

Mapletree_Module *Mapletree_Channel::GetModule()
{
    return module_;
}


//////////////////////////////////////////////////////////////////////////////
#ifdef XGATE_HAS_PPPOE

Mapletree_PPPoE_Channel::Mapletree_PPPoE_Channel( Mapletree_Module *mod, 
						  MTN_Channel * channel, 
						  PPPoE_Pool *pool )
    : Mapletree_Channel( mod, channel ), 
      PPPoE_Channel( pool, CreateSwitchDevice(pool->Name().c_str())),
      pppoe_session_id_(-1)
{
    // Set default setting.
    MTN_Call_Types call_types;
    call_types.set_bit( CallType_MODEM );
    //call_types.set_bit( CallType_ISDN_HDLC );

    port_setting_.set_call_role( CallRole_ANSWER );
    port_setting_.set_call_types( call_types );
    port_setting_.set_country_code( CountryCode_INTERNAT_ALAW );
    port_setting_.set_data_path_to_eth( true );

    //data_call_setting_.set_auto_answer_mode( true );
    data_call_setting_.set_host_if_framing( HostIfFraming_PPP );
    data_call_setting_.set_answer_link_delay( MTN_Time_Value(0, 0) );

    // Setup channel callback
    channel->set_callback( new Callback(this), true );

    // Initialize state machine object.
    spt_so_init( &so_, &sm_, this);
}

const char * Mapletree_PPPoE_Channel::GetName() const
{
    return GetChannel() ? GetChannel()->get_channel_name() : "NULL";
}

int  Mapletree_PPPoE_Channel::on_message( MTN_Queue_Type queue_id, const MTN_Message *msg )
{
    MessageTypeT msgtype = msg->get_type();

    if (MTN_Util::is_trap_msg(msgtype)) {
	//
	// TRAPS notification.
	// We're only interested to process state change traps to
	// be processed by the state machine.
	//
	if (msg->get_arg0()==Trap_PORT_STATE_CHANGE) {
	    switch (msg->get_arg1()) {
	    case PortState_IDLE:
		spt_so_applyevent( &so_, spt_stateevent_in,
				   Event_IDLE, NULL);
		break;
	    case PortState_DATA:
		spt_so_applyevent( &so_, spt_stateevent_in,
				   Event_DATA, NULL);
		break;
	    }
	}
    } else if (MTN_Util::is_cmd_msg(msgtype)) {
	//
	// Command notification (as the result of set_man_obj_async()).
	// The only command that trigger this type of notification is
	// force_state_async(). More over, the only state that we're
	// interested in is only PortState_ALLOCATED, since it is the
	// only state that is manually set by our application.
	//
	if (msgtype == MsgType_C_SET_MAN_OBJ &&
	    msg->get_arg1() == ManObj_C_PORT_STATE) 
	{
	    switch (msg->get_arg2()) {
	    case PortState_ALLOCATED:
		if (msg->get_status() == 0)
		    spt_so_applyevent( &so_, spt_stateevent_in,
				       Event_ALLOCATED, NULL );
		else
		    GetChannel()->force_state_async( PortState_TERMINATING );
		break;
	    }
	} else if (msgtype == MsgType_Q_GET_MAN_OBJ &&
		   msg->get_arg1() == ManObj_S_PPPOE_SESSION_ID &&
		   msg->get_status() == MsgStatus_OK)
	{
	    CLog::Info(THISMODULE, "Received session ID: %d\n", msg->get_arg2() >> 16);
	    if (pppoe_session_id_ == -1) {
		pppoe_session_id_ = msg->get_arg2();
	    }
	}

    } else if (MTN_Util::is_mob_msg(msgtype)) {
	//
	// Command notification as the result of MOB operations.
	// We're interested in the result of previous set_user_mob()
	// functions. If the result of the operation was successfull,
	// then trigger the corresponding event. If failed, then set the
	// channel state to TERMINATING, which in turn will trigger
	// PortState_IDLE trap.
	//
	if (msgtype==MsgType_C_SET_PREDEF_MOB ||
	    msgtype==MsgType_C_SET_USER_MOB)
	{
	    if (msg->get_status() != MsgStatus_OK) {
		GetChannel()->force_state_async( PortState_TERMINATING );
	    } else {
		switch (msg->get_arg0()) {
		case MTN_MOB_PORT_SETTING:
		    spt_so_applyevent( &so_, spt_stateevent_in,
				       Event_PORT_SETTING, NULL);
		    break;
		case MTN_MOB_DATA_CALL_SETTING:
		    spt_so_applyevent( &so_, spt_stateevent_in,
				       Event_DATA_CALL_SETTING, NULL);
		    break;
		}
	    }
	}
    } else if (MTN_Util::is_telemetry_msg(msg->get_type()) &&
	       msg->get_arg1() == sizeof(risc_telemetry_log_t)) {
	const risc_telemetry_log_t *log = 
	    (const risc_telemetry_log_t *)msg->get_buffer();

	CLog::Detail( THISMODULE, "%p Telemetry: %s", this, log->message);

	// Only interested in PPPoE subsystem to get the
	// PPPoE session ID.
	if (pppoe_session_id_==-1 && log->subsystem == 50 && log->message) {
	    //
	    // Sample message: "Session established Session ID 25"
	    //
	    if (!strncmp(log->message, "Session established", 19)) {

		if (strlen(log->message) < 32) {
		    CLog::Warning( THISMODULE, "%p: unable to get session ID (buffer wraped)", this);
		} else {
		    const char *ses_id = log->message + 31;
		    assert( isxdigit(*ses_id) );
		    
		    char *err;
		    pppoe_session_id_ = strtol(ses_id, &err, 16);
		    if (*err) {
			pppoe_session_id_ = -1;
			CLog::Warning( THISMODULE, "Error parsing PPPoE session ID: '%s'");
		    } else
			CLog::Debug( THISMODULE, "PPPoE session ID=%d (0x%x)",
			pppoe_session_id_, pppoe_session_id_);
		}
	    }
	}
    }
    return 0;
}


/** Prepare the channel using the specified channel configuration.
    Upon finished, the implementation MUST call OnPrepareComplete() with
    suplying the status of the preparation. Should the preparation failed,
    implementation MUST put the channel back to IDLE state.
 */
void Mapletree_PPPoE_Channel::OnPrepare( const PPPoE_Channel_Config & config,
					 int call_type )
{
    CLog::MTRACE( THISMODULE, GetTask(), "%p: OnPrepare %d", this, call_type);

    // Reset session ID
    pppoe_session_id_ = -1;

    // Apply configuration.
    if (config.pcm_law_ == PPPoE_Channel_Config::Law_ULAW)
	port_setting_.set_country_code( CountryCode_INTERNAT_MULAW );
    else
	port_setting_.set_country_code( CountryCode_INTERNAT_ALAW );

    // Apply call type.
    MTN_Call_Types cts;
    cts.reset();
    if (call_type == Call_MODEM)
	cts.set_bit( CallType_MODEM );
    else if (call_type == Call_ISDN_HDLC) {
	cts.clear_bit( CallType_MODEM );
	cts.set_bit( CallType_ISDN_HDLC );
    } else
	cts.set_bit( CallType_MODEM );
    port_setting_.set_call_types( cts );

    // Check current state.
    // If current state != State_IDLE, then we must terminate the
    // channel first. Otherwise, proceed to state State_ALLOCATING.
    if (so_.state != State_IDLE) {
	if (!GetChannel()->force_state_async( PortState_TERMINATING )) {
	    OnPrepareComplete(-1);
	    return;
	}
	spt_so_setstate( &so_, State_TERMINATE_ALLOCATE, 1 );
    } else {
	if (!GetChannel()->force_state_async( PortState_ALLOCATED )) {
	    OnPrepareComplete(-1);
	    return;
	}
	spt_so_setstate( &so_, State_ALLOCATING, 1 );
    }
}

/** Start the channel operation.
    Upon finished starting the channel, the implementation must call
    OnStartComplete() to indicate the status of the operation.
 */
void Mapletree_PPPoE_Channel::OnStart( )
{
    CLog::MTRACE( THISMODULE, GetTask(), "%p: OnStart", this);

    assert( so_.state == State_PREPARED ||
	    so_.state == State_STARTED);

    if (so_.state == State_STARTED) {
	OnStartComplete(0);
	return;
    }

    if (so_.state != State_PREPARED) {
	OnStartComplete(-1);
	return;
    }

    spt_so_setstate( &so_, State_STARTING, 1);
    GetChannel()->force_state_async( PortState_INITIATING );
}

/** Stop the channel, and put the channel back in IDLE state.
    Upon finished stopping the channel, implementation MUST call
    OnDisconnected() to indicate that the channel is IDLE.
 */
void Mapletree_PPPoE_Channel::OnStop( )
{
    CLog::MTRACE( THISMODULE, GetTask(), "%p: OnStop", this);

    if (so_.state == State_STARTED)
	spt_so_setstate( &so_, State_DISCONNECTING, 1);

    // Setting the channel state to TERMINATING will trigger
    // closing channel, and upon disconnected, IDLE trap will be
    // generated.
    GetChannel()->force_state_async( PortState_TERMINATING );
}

/** Shutdown the channel before it is deleted.
 */
void Mapletree_PPPoE_Channel::OnShutdown()
{
    CLog::MTRACE( THISMODULE, GetTask(), "%p: OnShutdown", this);
    GetChannel()->tdm_disable_tx();
}

/** Get PPPoE session ID
 */
int Mapletree_PPPoE_Channel::OnGetSessionID() const
{
    CTask *task = (CTask*) GetTask();
    CLog::MTRACE( THISMODULE, task, "%p: OnGetSessionID = %d", this, pppoe_session_id_);

    if (pppoe_session_id_ == -1) {
	Mapletree_PPPoE_Channel *this_ = (Mapletree_PPPoE_Channel*)this;
	this_->GetChannel()->get_man_obj_async( ManObj_S_PPPOE_SESSION_ID);
    }
    return pppoe_session_id_;
}

/** Initialize state machine for PPPoE
 */
void Mapletree_PPPoE_Channel::InitStateMachine()
{
    // Initialize state machine.
    // In normal condition, the initial port state when a channel
    // is not used is IDLE.
    spt_sm_init( &sm_, 
		 State_IDLE,
		 &sm_notify,
		 &sm_invalid_notify);

    // State_TERMINATE_ALLOCATE
    // Purpose: 
    //	If the channel is not in PortState_IDLE when OnPrepare() is 
    //	called (this should not happen though), then it must be
    //  put in PortState_IDLE first.
    spt_sm_registerprogress( &sm_,
			     State_TERMINATE_ALLOCATE, State_ALLOCATING,
			     spt_stateevent_in, Event_IDLE);
    // State_TERMINATE_ALLOCATE_CANCEL_JOB
    // Purpose:
    //	When CancelJob() is called when the state is TERMINATE_ALLOCATE,
    //	a PortState_IDLE trap can be received. This needs to be 
    //	distinguished from 'normal' PortState_IDLE trap that is
    //  expected.
    spt_sm_registerprogress( &sm_,
			     State_TERMINATE_ALLOCATE_CANCEL_JOB, State_IDLE,
			     spt_stateevent_in, Event_IDLE);
    // State_ALLOCATING
    // Purpose:
    //	After channel state is set to PortState_ALLOCATED, the state
    //  is set to this state. When Event_ALLOCATED is received,
    //	the next stage is to do port setting. If Event_IDLE is 
    //  received, then go to State_IDLE after notifying the application
    //  with OnPrepareComplete(-1)
    spt_sm_registerprogress( &sm_,
			     State_ALLOCATING, State_PORT_SETTING,
			     spt_stateevent_in, Event_ALLOCATED);
    spt_sm_registerprogress( &sm_,
			     State_ALLOCATING, State_IDLE,
			     spt_stateevent_in, Event_IDLE);
    // State_PORT_SETTING
    // Purpose:
    //	Channel was asked to configure the port setting.
    spt_sm_registerprogress( &sm_,
			     State_PORT_SETTING, State_DATA_CALL_SETTING,
			     spt_stateevent_in, Event_PORT_SETTING);
    spt_sm_registerprogress( &sm_,
			     State_PORT_SETTING, State_IDLE,
			     spt_stateevent_in, Event_IDLE);
    // State_DATA_CALL_SETTING
    // Purpose:
    //	Channel was asked to configure the data call setting.
    //
    spt_sm_registerprogress( &sm_,
			     State_DATA_CALL_SETTING, State_PREPARED,
			     spt_stateevent_in, Event_DATA_CALL_SETTING);
    spt_sm_registerprogress( &sm_,
			     State_DATA_CALL_SETTING, State_IDLE,
			     spt_stateevent_in, Event_IDLE);
    // State_PREPARED
    // Purpose:
    //	Channel has been prepared.
    //	Note that after preparation has been completed, channel
    //	can get PortState_DATA trap at any time. If this happens,
    //	then go to State_STARTED.
    //
    spt_sm_registerprogress( &sm_,
			     State_PREPARED, State_IDLE,
			     spt_stateevent_in, Event_IDLE);
    spt_sm_registerprogress( &sm_,
			     State_PREPARED, State_STARTED,
			     spt_stateevent_in, Event_DATA);
    // State_STARTING
    // Purpose:
    //	Waiting until PortState_DATA trap is received.
    spt_sm_registerprogress( &sm_,
			     State_STARTING, State_STARTED,
			     spt_stateevent_in, Event_DATA);
    spt_sm_registerprogress( &sm_,
			     State_STARTING, State_IDLE,
			     spt_stateevent_in, Event_IDLE);
    // State_STARTED
    // Purpose:
    //	Channel is in PortState_DATA.
    //
    spt_sm_registerprogress( &sm_,
			     State_STARTED, State_IDLE,
			     spt_stateevent_in, Event_IDLE);
    spt_sm_registerprogress( &sm_,
			     State_STARTED, State_STARTED,
			     spt_stateevent_in, Event_DATA);

    // State_DISCONNECTING
    // Purpose:
    //	Channel was asked to terminate itself. We expect PortState_IDLE
    //	trap to be received.
    spt_sm_registerprogress( &sm_,
			     State_DISCONNECTING, State_IDLE,
			     spt_stateevent_in, Event_IDLE);

}


void Mapletree_PPPoE_Channel::sm_notify( State srcstate, 
					 State dststate, 
					 Event event)
{
    switch (dststate) {
    case State_IDLE:

	GetChannel()->tdm_disable_tx();

	switch (srcstate) {
	case State_IDLE:
	    // Nothing to do
	    break;
	case State_TERMINATE_ALLOCATE:
	    // Invalid
	    assert(0);
	    break;
	case State_TERMINATE_ALLOCATE_CANCEL_JOB:
	case State_ALLOCATING:
	case State_PORT_SETTING:
	case State_DATA_CALL_SETTING:
	    OnPrepareComplete( -1 );
	    break;
	case State_PREPARED:
	    OnDisconnected( NULL );
	    break;
	case State_STARTING:
	    OnStartComplete(-1);
	    break;
	case State_STARTED:
	    OnDisconnected( NULL );
	    break;
	case State_DISCONNECTING:
	    OnDisconnected( NULL );
	    break;
	}
	break;
    case State_TERMINATE_ALLOCATE:
	// Nothing to do
	break;
    case State_TERMINATE_ALLOCATE_CANCEL_JOB:
	// Nothing to do
	break;
    case State_ALLOCATING:
	switch (srcstate) {
	case State_TERMINATE_ALLOCATE:
	    GetChannel()->force_state_async( PortState_ALLOCATED );
	    break;
	}
	break;
    case State_PORT_SETTING:
	assert( srcstate == State_ALLOCATING);
	GetChannel()->set_mob_async( &port_setting_ );
	break;
    case State_DATA_CALL_SETTING:
	assert( srcstate == State_PORT_SETTING);
	GetChannel()->set_mob_async( &data_call_setting_ );
	break;
    case State_PREPARED:
	OnPrepareComplete(0);
	break;
    case State_STARTING:
	// Nothing to do
	break;
    case State_STARTED:
	//assert( srcstate == State_STARTING && event == Event_DATA);
	if (srcstate != State_STARTED)
	    OnStartComplete(0);
	break;
    case State_DISCONNECTING:
	// Nothing to do
	break;
    }
}


void Mapletree_PPPoE_Channel::sm_notify(struct spt_so_rec *obj, 
				        spt_state_t oldstate, 
				        spt_state_t newstate,
				        spt_stateevent_type eventtype,
				        spt_stateevent_dir  eventdir,
				        spt_stateevent_t event,
				        void *eventdata)
{
    Mapletree_PPPoE_Channel *channel = 
	(Mapletree_PPPoE_Channel*) obj->userdata;
    CLog::MTRACE( THISMODULE, channel->GetTask(), "%p: Event notification %s: %s-->%s",
			      channel, StrEvent(event), StrState(oldstate), StrState(newstate));

    channel->sm_notify( (Mapletree_PPPoE_Channel::State)oldstate, 
		        (Mapletree_PPPoE_Channel::State)newstate, 
			(Mapletree_PPPoE_Channel::Event)event );
}

void Mapletree_PPPoE_Channel::sm_invalid_notify (struct spt_so_rec *obj,
						 spt_state_t current,
						 spt_stateevent_dir eventdir,
						 spt_stateevent_t event,
						 void *eventdata)
{
    Mapletree_PPPoE_Channel *channel = 
	(Mapletree_PPPoE_Channel*) obj->userdata;
    CLog::Error( THISMODULE, channel->GetTask(), "%p: Spurious event %s in state %s", 
			     obj->userdata, StrEvent(event), StrState(current));
}


const char *Mapletree_PPPoE_Channel::StrState( int st )
{
#define STRUCT_ENTRY(a)	    {a, #a}
    static struct {
	State	    state;
	const char *str;
    } stmap[] = {
	STRUCT_ENTRY(State_IDLE),
	STRUCT_ENTRY(State_TERMINATE_ALLOCATE),
	STRUCT_ENTRY(State_TERMINATE_ALLOCATE_CANCEL_JOB),
	STRUCT_ENTRY(State_ALLOCATING),
	STRUCT_ENTRY(State_PORT_SETTING),
	STRUCT_ENTRY(State_DATA_CALL_SETTING),
	STRUCT_ENTRY(State_PREPARED),
	STRUCT_ENTRY(State_STARTING),
	STRUCT_ENTRY(State_STARTED),
	STRUCT_ENTRY(State_DISCONNECTING),
    };

    for (int i=0; i<sizeof(stmap)/sizeof(stmap[0]); ++i)
	if (stmap[i].state == st)
	    return stmap[i].str;
    return "UNKNOWN";
}

const char *Mapletree_PPPoE_Channel::StrEvent( int evt )
{
    static struct {
	Event	    event;
	const char *str;
    } evtmap[] = {
	STRUCT_ENTRY(Event_IDLE),
	STRUCT_ENTRY(Event_ALLOCATED),
	STRUCT_ENTRY(Event_DATA),
	STRUCT_ENTRY(Event_PORT_SETTING),
	STRUCT_ENTRY(Event_DATA_CALL_SETTING),
    };

    for (int i=0; i<sizeof(evtmap)/sizeof(evtmap[0]); ++i)
	if (evtmap[i].event == evt)
	    return evtmap[i].str;
    return "";
#undef STRUCT_ENTRY
}
#endif /* XGATE_HAS_PPPOE */

#endif	/* XGATE_HAS_MTN */

