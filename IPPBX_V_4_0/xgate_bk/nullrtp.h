/* $Header: /xGate/nullrtp.h 5     5/02/01 12:02p Bennylp $
 *   
 * CTENGINE
 * (C) 2001 Switchlab, Ltd
 *
 * Dummy VoiceOverIP media interface.
 *
 * $Log: /xGate/nullrtp.h $
 * 
 * 5     5/02/01 12:02p Bennylp
 * Added show interface framework.
 * 
 * 4     4/26/01 4:14a Bennylp
 * Splitting SIP signalling into SIP and XGATE call signalling.
 * 
 * 3     4/24/01 7:12a Bennylp
 * Changed to new channel interface that supports async.
 * 
 * 2     4/23/01 10:44p Bennylp
 * Fixed std namespace conflicts.
 * 
 * 1     4/19/01 5:29p Bennylp
 * Initially created.
 * 
 */
#ifndef __NULL_RTP_H__
#define __NULL_RTP_H__

#include "sipmedia.h"


/* Forward decl.
 */
class SipCapability;
class Null_RTP_Media;

///////////////////////////////////////////////////////////////////////////////
/** This class describes a dummy RTP media interface.
 */
class Null_RTP_Interface : public Interface {
public:
    /** Constructor.
     */
    Null_RTP_Interface (const char *fullName,int port);

    /** Destructor.
     */
    virtual ~Null_RTP_Interface();

    /** This function will be called by the service framework after 
        instantiation of this class, to allow the instance to initialize
	itself.
         */
    virtual bool Load();

    /** This function will be called by the service framework before it is
        destroyed/deleted, to allow the instance to cleanup itself.
     */
    virtual bool Unload();

    /** Start: put the interface in service.
     */
    virtual bool Start();

    /** Stop: stop the service.
     */
    virtual bool Stop();

    /** AllocateMedia: Called by CallManager to allocate a media in this itf.
	If a free media can be allocated, it may be returned synchronously or
	asynhronously (by calling MediaAllocatedInd method of Call object).
	NULL may be specified in call argument in the case of synchronous
	allocation.
	It returns false when there is no free media available
     */
    virtual bool AllocateMedia(Call *call,Media *&media,SelectionStrategy sel,CallDirection dir,CallParameter *cparam);

    /** AllocateMedia: A variant of AllocateMedia that specify a particular media
	to be allocated. It returns false when there is such media is not available
     */
    virtual bool AllocateMedia(Call *call,Media *&media,MediaId id,CallDirection dir,CallParameter *cparam);

    /** ReleaseMedia: Called by Call object when the call becomes idle. The underlying
	channel is then ready to use for another call.
     */
    virtual void ReleaseMedia(Media *media,CallDirection dir,CallParameter *cparam,CallCause cc);

    /** Get capability list supported by this media.
     */
    const List <SipCapability*> & GetCapabilityList () const {
	return capabilityList;
    }

    /** Get the local IP address.
     */
    const String & GetLocalAddress() const {
	return ipAddress;
    }

    /**
     Terminal command callback
     */
    virtual void DoTermCmd(Term_Session *, bool no, Term_Cmd *);

    /**
     Show configuration for the specified entry.
     This function will be called by the framework whenever it needs to display the
     specified entry. Previously this instance should tell the entry that it wishes to
     handle displaying the entry by calling entry->RegisterShowHandler().

     When this instance is deleted, it should call entry->UnregisterShowHandler()
     so that the handler will not be called (which will cause GPF).
     */
    virtual bool OnShowConfig(
	const Cfg_Tree_Entry *entry,
	String & output, 
	const String & indent, 
	int indent_size) const;

    /** Display interface specific information to the terminal user.
     */
    virtual void OnShowInterface ( Term_Session * session );

private:
    String	ipAddress;
    unsigned	startPort, portIncrement;
    unsigned    nChannels;
    bool	isShutdown;

    Vector <Null_RTP_Media *> freeMedia;
    Vector <Null_RTP_Media *> usedMedia;

    List <SipCapability *>  capabilityList;

    /* Create the media channels.
     */
    bool CreateChannels();

    /* Destroy the media channels.
     */
    void DestroyChannels();
};


///////////////////////////////////////////////////////////////////////////////
/** This class describes the ATM manager.
 */
class Null_RTP_Manager : public SIP_Media_Interface_Manager {
public:
    /** Create the instance of this class.
     */
    static Null_RTP_Manager * Create();

    /** Get the singleton instance of this class.
     */
    static Null_RTP_Manager * Instance() {
	return nullRTPManager;
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

    /** Allocate a media interface based on the information in the incoming call
	description.
     */
    virtual SIP_Media_Channel *AllocateSIPMedia (
	XGATE_SIP_Call *call,		    // the incoming call.
	const String & sig_addr,	    // source signalling address
	int sig_port,			    // source signalling port number
	const String & med_addr,	    // source media address
	int med_port			    // source media port number
    );

private:
    static Null_RTP_Manager *nullRTPManager;
    List <Null_RTP_Interface *> itfList;

    Null_RTP_Manager();
    ~Null_RTP_Manager();
};


#endif	/* __NULL_RTP_H__ */

