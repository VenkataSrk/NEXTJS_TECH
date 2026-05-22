/*----------------------------------------------------------------* 
 * $Archive: /xGate/CTbusMgr.h $
 * $Date: 6/29/01 12:28p $
 * $Revision: 3 $
 *
 * $History: CTbusMgr.h $
 * 
 * *****************  Version 3  *****************
 * User: Bennylp      Date: 6/29/01    Time: 12:28p
 * Updated in $/xGate
 * Move Set/GetOwner to SwitchAddress from CTbusAddress.
 * 
 * *****************  Version 2  *****************
 * User: Junanto      Date: 26/05/01   Time: 16:12
 * Updated in $/xGate
 * Added CTbus owner's information and a new 
 * command 'show ctbus'
 * 
 * *****************  Version 1  *****************
 * User: Junanto      Date: 24/04/01   Time: 14:57
 * Created in $/xGate
 * First integration of TDM switching framework.
 * Fabric supported: Aculab local bus and CT bus (H.100/H.110)
 * 
 *----------------------------------------------------------------*/

/*----------------------------------------------------------------* 
 * Description:
 * It contains implementation of CTbus (H.100/H.110) specific switching
 * address and CTbus dynamic allocation manager
 *----------------------------------------------------------------*/


#if !defined(CT2_CTBUS_H__INCLUDED_)
#define CT2_CTBUS_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "serviceprovider.h"
#include "Switching.h"

// Forward declaration
class Term_Session;


/** CTbusAddress: Representation of CTbus switching address.
    A CTbus address is composed of stream (0-31 inclusive) and 
    timeslot (0-127 inclusive)
 */
class CTbusAddress : public SwitchAddress
{
public:
    /** CTbusAddress' constructor/destructor
     */
    CTbusAddress(int stream,int tslot);
    virtual ~CTbusAddress();

    /** Return the stream part of this address
     */
    int GetStream() {
	return _stream;
    }

    /** Return the timeslot part of this address
     */
    int GetTimeslot() {
	return _tslot;
    }

    /** Return the string representation of this address.
     */
    virtual const char *ToString();

private:
    int		_stream,_tslot;
    String	_name;
};


/** CTbusManager: A singleton which manages dynamic allocation
    of CTbus address
 */
class CTbusManager : public ServiceProvider
{
public:
    virtual ~CTbusManager();

    /** Create a singleton instance of AculabSignalling
     */
    static CTbusManager *Create();

    /** Instance: Return a pointer to this singleton instance
     */
    static CTbusManager *Instance();

    /** Allocate one free CTbus address
     */
    CTbusAddress *AllocateAddress(const char *by);

    /** Free one CTbus address and put it in the free pool
     */
    void FreeAddress(CTbusAddress *addr);

    /** Show CTbus usage
     */
    void ShowUsage(Term_Session *session);

    /** This function will be called by the service framework after 
        instantiation of this class, to allow the instance to initialize
	itself.
     */
    virtual bool Load();

    /** This function will be called by the service framework before it is
        destroyed/deleted, to allow the instance to cleanup itself.
     */
    virtual bool Unload();

    /** Reserve one CTbus address */
    CTbusAddress* ReserveAddress(const char* requestor,int ts,int st);

private:
    /** Private constructor
     */
    CTbusManager();

    struct Timeslot
    {
	bool busy;
	CTbusAddress* addr;
    };

    enum {
	MinStream = 0,
	MaxStream = 31,
	TslotPerStream = 128,
	MaxTslot = ((MaxStream-MinStream)+1)*TslotPerStream,
    };

    Timeslot pool[MaxTslot];
    int pool_idx;
    int tslot_usage_cnt;

    static CTbusManager	*theCTbusManager;
};

#endif // !defined(CT2_CTBUS_H__INCLUDED_)
