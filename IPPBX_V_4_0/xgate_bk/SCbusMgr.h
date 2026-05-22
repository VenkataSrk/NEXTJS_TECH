/*----------------------------------------------------------------* 
 * $Archive: /xGate/SCbusMgr.h $
 * $Date: 2/07/01 15:34 $
 * $Revision: 3 $
 *
 * $History: SCbusMgr.h $
 * 
 * *****************  Version 3  *****************
 * User: Sonny        Date: 2/07/01    Time: 15:34
 * Updated in $/xGate
 * add ReserveAddress
 * 
 * *****************  Version 2  *****************
 * User: Bennylp      Date: 6/29/01    Time: 12:28p
 * Updated in $/xGate
 * Add 'show scbus' command.
 * 
 * *****************  Version 1  *****************
 * User: Junanto      Date: 30/04/01   Time: 12:25
 * Created in $/xGate
 * Added SCbus address and dynamic allocaltion
 * 
 *----------------------------------------------------------------*/

/*----------------------------------------------------------------* 
 * Description:
 * It contains implementation of SCbus specific switching
 * address and SCbus dynamic allocation manager
 *----------------------------------------------------------------*/


#if !defined(CT2_SCBUS_H__INCLUDED_)
#define CT2_SCBUS_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "serviceprovider.h"
#include "Switching.h"
#include <set>


/** SCbusAddress: Representation of SCbus switching address.
    A SCbus address is composed of stream (0-31 inclusive) and 
    timeslot (0-127 inclusive)
 */
class SCbusAddress : public SwitchAddress
{
public:
    /** SCbusAddress' constructor/destructor
     */
    SCbusAddress(int tslot);
    virtual ~SCbusAddress();

    /** Return the timeslot part of this address
     */
    int GetTimeslot() const {
	return _tslot;
    }

    /** Return the string representation of this address.
     */
    virtual const char *ToString();

private:
    int		_tslot;
    String	_name;
};

struct SCbus_Cmp {
    bool operator() (const SCbusAddress *ad1, const SCbusAddress *ad2) const {
	return ad1->GetTimeslot() < ad2->GetTimeslot();
    }
};


/** SCbusManager: A singleton which manages dynamic allocation
    of SCbus address
 */
class SCbusManager : public ServiceProvider
{
public:
    virtual ~SCbusManager();

    /** Create a singleton instance of SCbusManager
     */
    static SCbusManager *Create();

    /** Instance: Return a pointer to this singleton instance
     */
    static SCbusManager *Instance();

    /** Reserve one specified SCbus address
     */
    SCbusAddress* ReserveAddress(const char *requestor, const int ts);
    /** Allocate one free SCbus address
     */
    SCbusAddress *AllocateAddress(const char *requestor);

    /** Free one SCbus address and put it in the free pool
     */
    void FreeAddress(SCbusAddress *addr);

    /** This function will be called by the service framework after 
        instantiation of this class, to allow the instance to initialize
	itself.
     */
    virtual bool Load();

    /** This function will be called by the service framework before it is
        destroyed/deleted, to allow the instance to cleanup itself.
     */
    virtual bool Unload();

    /** Show SCbus usage
     */
    void ShowUsage(Term_Session *session);

private:
    /** Private constructor
     */
    SCbusManager();

    typedef List<SCbusAddress*>	    ListSCbusAddress;

    static SCbusManager	*theSCbusManager;
    ListSCbusAddress	_freeAddress;
    std::set<SCbusAddress*,SCbus_Cmp> _usedAddress;
};

#endif // !defined(CT2_SCBUS_H__INCLUDED_)
