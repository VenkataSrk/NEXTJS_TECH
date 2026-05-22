/* $Header: /xGate/mapletree.h 17    12/15/03 6:12p Sonny $
 *   
 * XGATE
 * (C) 2001 Switchlab, Ltd
 *
 * Mapletree boards support
 *
 * $Log: /xGate/mapletree.h $
 * 
 * 17    12/15/03 6:12p Sonny
 * 
 * 16    4/10/03 9:39a Bennylp
 * Changed switching to use preallocated addresses.
 * 
 * 15    4/04/03 11:59a Bennylp
 * Alloc and tx to TDM early when channel is created (cross-talk problem)
 * 
 * 14    2/19/03 11:30a Bennylp
 * Fixed errorneous dependency to XGATE_HAS_PPPOE
 * 
 * 13    12/06/02 11:51a Bennylp
 * Add TDM setting command.
 * 
 * 12    11/04/02 7:02p Bennylp
 * Show resource name in switch device.
 * 
 * 11    10/30/02 6:59p Bennylp
 * Moved switch device to header file.
 * 
 * 10    10/24/02 8:34p Bennylp
 * Latest stable checkin.
 * 
 * 9     10/23/02 5:01p Bennylp
 * Some more fixes
 * 
 * 8     10/21/02 6:40p Bennylp
 * Changes to adapt the new MTN framework changes.
 * 
 * 7     10/10/02 10:59a Bennylp
 * Added version in show status
 * 
 * 6     9/20/02 6:02p Bennylp
 * Completed Mapletree module configuration and status.
 * 
 * 5     7/01/02 5:01p Bennylp
 * Changes related to new MTN library (5350)
 * 
 * 4     3/07/02 9:21p Bennylp
 * Support for PPPoE session ID, bunch of bug fixes, tested.
 * 
 * 3     2/19/02 12:07p Bennylp
 * Finished implementation of the new framework.
 * 
 * 2     2/15/02 3:40p Bennylp
 * Mapletree integration to xGate
 * 
 * 1     2/08/02 6:10p Bennylp
 * Added to source control.
 */
#if !defined(__MAPLETREE_H__)
#define __MAPLETREE_H__

#include "xgconfig.h"
#include "serviceprovider.h"
#include "../MTN_Lib/src/mtn_mob.h"
#include "../MTN_Lib/src/mtn_mod.h"
//#include "../SPT/include/spt/spt1.h"
#include "Switching.h"
#include "cfg.h"

#ifdef XGATE_HAS_PPPOE
#include "pppoe.h"
#endif

class Mapletree_Module;
class Mapletree_Channel;
class SwitchDevice;
class MTN_Module;
class MTN_Channel;


//////////////////////////////////////////////////////////////////////////////
class Mapletree_Switch_Device : public SwitchDevice {
public:
    Mapletree_Switch_Device( const char *name, MTN_Channel *ch, 
			     SwitchFabric fabric, SwitchAddress *addr );
    ~Mapletree_Switch_Device();

    void Initialize();
    void DetachChannel();

    virtual SwitchFabric GetCompatibleFabric(SwitchDevice* dev);
    virtual bool SupportAculab(SwitchDevice* requester);
    virtual bool SupportCTbus(SwitchDevice* requester);
    virtual bool SupportSCbus(SwitchDevice* requester);
    virtual SwitchAddress* AllocateAddress(SwitchFabric fabric);
    virtual void FreeAddress(SwitchFabric fabric);
    virtual SwitchAddress* GetAddress(SwitchFabric fabric);
    virtual void DeviceStartTransmit(SwitchFabric fabric,SwitchAddress* a);
    virtual void DeviceStartReceive(SwitchFabric fabric,SwitchAddress* a);
    virtual void DeviceStopTransmit(SwitchFabric fabric,SwitchAddress* a);
    virtual void DeviceStopReceive(SwitchFabric fabric,SwitchAddress* a);

private:
    SwitchAddress *addr_;
    MTN_Channel * channel_;
    SwitchFabric  fabric_;

    SwitchAddress *alloc_address(SwitchFabric fabric);
    void	   free_address(SwitchFabric fabric);
};


//////////////////////////////////////////////////////////////////////////////
/** This singleton class acts as the manager for Mapletree support in xGate.
 */
class Mapletree_Module_Manager : public ServiceProvider {
public:
    static Mapletree_Module_Manager * Create();
    static Mapletree_Module_Manager * Instance();

    virtual bool Load();
    virtual bool Unload();
    virtual bool OnShowStatus(String & output);

    size_t	       GetModuleCount( ) const;
    Mapletree_Module * GetModule( size_t index );
    Mapletree_Module * FindModule( const char *name );
    Cfg_Tree_Group *   GetCfgEntry() { return cfg_entry_; }

private:
    static Mapletree_Module_Manager * instance_;
    Vector< Mapletree_Module* > modules_;
    Cfg_Tree_Group *	    cfg_entry_;

    Mapletree_Module_Manager();
    void DoTermCmd(Term_Session *session, bool no, Term_Cmd *cmd);
};


//////////////////////////////////////////////////////////////////////////////
/** This class provides an interface and implementation for a single Mapletree
    board that is detected in the system.
 */
class Mapletree_Module : public ServiceProvider {
public:
    Mapletree_Module( MTN_Board * board );

    virtual bool Load();
    virtual bool Unload();
    virtual bool CommitConfiguration();
    virtual bool OnShowStatus(String & output);

    virtual void HandleEvents();
    void	 HandleData();

    bool	 IsEnabled() const { return enabled_; }
    bool	 IsRunning() const { return running_; }

    size_t       GetFreeChannel() const;
    size_t       GetUsedChannel() const;

    MTN_Channel *AllocateChannel();
    void         ReleaseChannel( MTN_Channel *ch );

    CountryCodeT GetCountryCode() const { return country_code_; }

    const char * GetName() const;
    bool	 GetIpSetting( MTN_Ip_Setting_Mob_Data * setting );
    bool	 GetEthernetInfo( MTN_Ethernet_Mob_Data * info );
    SwitchFabric GetTdmBusType() const;
    Cfg_Tree_Group *GetConfigTree() {return cfg_entry_;}

    SwitchAddress* GetTdmAddress(unsigned module_id, unsigned ch_id);

    bool acquire_upstream_mutex();
    bool release_upstream_mutex();

private:
    MTN_Board		   *board_;
    bool		    has_eth_;
    MTN_Ip_Setting_Mob_Data ip_setting_;
    MTN_Ethernet_Addr_Mob_Data eth_addr_;
    MTN_Ethernet_Mob_Data   eth_status_;
    bool		    enabled_;
    bool		    running_;
    //CRITICAL_SECTION	    upstream_mutex_;
    pthread_mutex_t	    upstream_mutex_;	//linux porting

    MTN_TDM_Clock_Mode	    tdm_mode_;
    CountryCodeT	    country_code_;

    Cfg_Tree_Group *	    cfg_entry_;
    String		    risc_version_;
    String		    dsp_version_;
    
    Map<unsigned,SwitchAddress*> tdm_addr_map_;

    bool StartModule();
    bool StopModule();
    bool WaitEvent(MTN_Event_Notification *event, int sec_timeout);

    virtual void   DoTermCmd(Term_Session *, bool no, Term_Cmd *);
    virtual bool OnShowConfig( const Cfg_Tree_Entry *entry,
				 String & output, 
				 const String & indent, 
				 int indent_size) const;
};


//////////////////////////////////////////////////////////////////////////////
/** This class provides general functionality for a Mapletree channel. Among
    other things, it provides switching service.
 */
class Mapletree_Channel {
public:
    Mapletree_Channel( Mapletree_Module *mod, MTN_Channel * channel );
    virtual ~Mapletree_Channel();

    Mapletree_Module  * GetModule();
    SwitchDevice *	CreateSwitchDevice(const char *name);
    MTN_Channel *	GetChannel();
    const MTN_Channel * GetChannel() const;

protected:
    void	      ReleaseChannel();

private:
    Mapletree_Module *module_;
    MTN_Channel *channel_;

    friend class MTN_Modem_Channel;
};


#ifdef XGATE_HAS_PPPOE

//////////////////////////////////////////////////////////////////////////////
/** Mapletree PPPoE channel.
 */
class Mapletree_PPPoE_Channel : public Mapletree_Channel, public PPPoE_Channel {
public:
    Mapletree_PPPoE_Channel( Mapletree_Module *mod, 
			     MTN_Channel * channel, 
			     PPPoE_Pool *pool );

    static void InitStateMachine();

private:
    MTN_Port_Setting_Mob_Data	    port_setting_;
    MTN_Port_Status_Mob_Data	    port_status_;
    MTN_Data_Call_Setting_Mob_Data  data_call_setting_;
    MTN_Data_Call_Status_Mob_Data   data_call_status_;
    static spt_sm_rec		    sm_;
    spt_so_rec			    so_;
    int				    pppoe_session_id_;

    enum Event {
	Event_IDLE,
	Event_ALLOCATED,
	Event_DATA,
	Event_PORT_SETTING,
	Event_DATA_CALL_SETTING,
    };

    enum State {
	State_IDLE,
	State_TERMINATE_ALLOCATE,
	State_TERMINATE_ALLOCATE_CANCEL_JOB,
	State_ALLOCATING,
	State_PORT_SETTING,
	State_DATA_CALL_SETTING,
	State_PREPARED,
	State_STARTING,
	State_STARTED,
	State_DISCONNECTING,
    };

    bool error_;

    // Callback to receive channel's events.
    class Callback : public MTN_Message_Callback {
    public:
	Callback( Mapletree_PPPoE_Channel *ch ) : channel_(ch) {}
    private:
	Mapletree_PPPoE_Channel *channel_;
	virtual int on_message ( MTN_Queue_Type queue_id, const MTN_Message * msg) {
	    return channel_->on_message( queue_id, msg );
	}
    };

    int  on_message( MTN_Queue_Type queue_id, const MTN_Message *msg );

    // Overridden from PPPoE_Channel
    virtual const char * GetName() const;
    virtual void OnPrepare( const PPPoE_Channel_Config & config, int call_type );
    virtual void OnStart( );
    virtual void OnStop( );
    virtual void OnShutdown();
    virtual int OnGetSessionID() const;

    // Notification from state machine.
    void sm_notify( State srcstate, State dststate, Event event);
    static void sm_notify(struct spt_so_rec *obj, 
			  spt_state_t oldstate, 
			  spt_state_t newstate,
			  spt_stateevent_type eventtype,
			  spt_stateevent_dir  eventdir,
			  spt_stateevent_t event,
			  void *eventdata);
    static void sm_invalid_notify (struct spt_so_rec *obj,
				   spt_state_t current,
				   spt_stateevent_dir eventdir,
				   spt_stateevent_t event,
				   void *eventdata);

    static const char *StrState( int state );
    static const char *StrEvent( int event );

    friend class Callback;
    friend class Notification;
};

#endif	/* XGATE_HAS_PPPOE */

#endif	/* __MAPLETREE_H__ */
