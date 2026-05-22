#ifndef __XGATE_H248CALLP_H__
#define __XGATE_H248CALLP_H__

/*----------------------------------------------------------------* 
 * $Archive: /xGate/h248Callp.h $
 * 
 *----------------------------------------------------------------*/

#include "callmanager.h"
#include "call.h"
#include "SignallingDriver.h"
#include "megacotransport.h"
#include "Critical_Sectionfull.h"
#include "ace/INET_Addr.h"
#include "quality.h"
#include <pthread.h>  //T 19022018

typedef int MediaGatewayId;
struct EphEndpoint;

/** H248Signalling: A singleton class which handles call control 
    signalling specific to pipelines.
 */

#ifdef H248_MGC
class H248Signalling : public SignallingDriver
{
public:
    virtual void DoTermCmd(Term_Session *, bool no, Term_Cmd *) {}

    /** Create a singleton instance of H248Signalling
     */
    static H248Signalling *Create();

    /** Instance: Returns a pointer to this singleton instance, if necessary
	create one instance first before returning a pointer to it
     */
    static H248Signalling *Instance();


    /** SetupReq: Initiate a new outgoing call in a specific Interface
	using the specified call parameter
     */
    virtual Call *SetupReq(Interface *itf,SelectionStrategy selection,CallParameter *cparam) {}

    /** CreateDefaultCallParameter: Returns a CallParameter structure containing
	default values for this signalling protocol
     */
    virtual CallParameter *CreateDefaultCallParameter();

    /** AttachInterface: A new Interface object is attached to this 
	SignallingDriver object
     */
    virtual bool AttachInterface(Interface *itf);

    /** DetachInterface: Remove association between a Interface object and this 
	SignallingDriver object
     */
    virtual void DetachInterface(Interface *itf);

    /** DisableInterface: an interface has been disabled, the signalling
	driver should take the necessary action if it can
     */
    virtual bool DisableInterface(Interface *itf);

    /** EnableInterface: an interface has been enabled, the signalling
	driver should take the necessary action if it can
     */
    virtual bool EnableInterface(Interface *itf);

    /** This function will be called by the service framework after 
        instantiation of this class, to allow the instance to initialize
	itself.
     */
    virtual bool Load();

    /** This function will be called by the service framework before it is
        destroyed/deleted, to allow the instance to cleanup itself.
     */
    virtual bool Unload();

    /** HandleEvents: It is called by Service Provider Manager
     */

    virtual void HandleEvents();

    /** Schedule a call to be initiated at the next HandleEvents
     */
    void ScheduleNewCall(Call *call);
    bool CheckCallList(Call *call); // MGC-MG STABILITY

    /** Cancel a call from a scheduling list
     */
    void CancelNewCall(Call *call);
    void UpdateMediaState(unsigned long transId, char *ep, CallMediaState newState,ACE_INET_Addr rtpAddr, int codec);
    void insertActiveMedia(Media *med,char *ep);
    void removeActiveMedia(char *ep);
    void updateActiveMedia(ACE_INET_Addr addr, Rtp_Payload_Code codec,char *ep);
    Media * findActiveMedia(char *ep);

private:
    static H248Signalling *theH248Signalling;

    /** Private constructor
     */
    H248Signalling();
    typedef Map<String , Media *> MediaMap;
    MediaMap     CallMediaMap;

    typedef List<Call*> ListCall;
    ListCall _newCall;
    pthread_mutex_t newcall_mutex;  //T 19022018
    Critical_Section critical_section_;
};
#endif 

#endif
