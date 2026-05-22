/*----------------------------------------------------------------* 
 * $Archive: /xGate/DevUsage.h $
 * $Date: 5/23/02 7:12p $
 * $Revision: 3 $
 *
 * $History: DevUsage.h $
 * 
 * *****************  Version 3  *****************
 * User: Sonny        Date: 5/23/02    Time: 7:12p
 * Updated in $/xGate
 * added peak time on "show device-usage"
 * 
 * *****************  Version 2  *****************
 * User: Bennylp      Date: 8/20/01    Time: 6:41p
 * Updated in $/xGate
 * Some dev-usage command improvements/bug fixes.
 * 
 * *****************  Version 1  *****************
 * User: Junanto      Date: 16/08/01   Time: 21:10
 * Created in $/xGate
 * 
 *----------------------------------------------------------------*/

/*----------------------------------------------------------------* 
 * Description:
 * It contains various counters to track device usage. Device is
 * the smallest hardware which can be physically and easily localized
 * such as DSP module, interface 
 *----------------------------------------------------------------*/


#if !defined(CT2_DEVUSAGE_H__INCLUDED_)
#define CT2_DEVUSAGE_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <sys/types.h>
#include "serviceprovider.h"

class DeviceUsage;

class Term_Session;

/** DeviceUsageManager: This singleton entity maintains the list of all
    DeviceUsage instances
 */
class DeviceUsageManager : public ServiceProvider
{
    friend class DeviceUsage;
    typedef List<DeviceUsage*> ListDeviceUsage;
public:
    /** Destructor
     */
    virtual ~DeviceUsageManager();

    /** Returns a pointer to the singleton
     */
    static DeviceUsageManager* Instance();

    void ShowDeviceUsage(Term_Session* session, const char *namespec);

private:
    /** Constructor
     */
    DeviceUsageManager();

    /** Add/remove an DeviceUsage instance to/from the list
     */
    void InsertDeviceUsage(DeviceUsage* dev);
    void RemoveDeviceUsage(DeviceUsage* dev);

    /** Clear counter.
     */
    virtual void OnClearCounter();

    /** Must be implemented for ServiceProvider
     */
    virtual bool Load() { return true; }
    virtual bool Unload() { return true; }

    ListDeviceUsage _list;
    time_t _lastReset;

    /* Sort device usage based on the average.
     */
    static bool SortDev_Avg_Asc(const DeviceUsage *d1, const DeviceUsage *d2);
    static bool SortDev_Avg_Desc(const DeviceUsage *d1, const DeviceUsage *d2);
};


/** DeviceUsage: This object maintains various counters to track device usage
 */
class DeviceUsage
{
    friend class DeviceUsageManager;
public:
    DeviceUsage(const String& name);
    virtual ~DeviceUsage();

    void StartUsage();
    void StopUsage(unsigned duration);

    const char* GetName() const;

    unsigned GetAvgDuration() const {
	return _prevUsage ? _totalDuration / _prevUsage : 0;
    }
    unsigned GetTotalUsage() const {
	return _prevUsage + _currentUsage;
    }

private:
    //typedef __int64 LARGE_INT;
    typedef int64_t LARGE_INT;

    /** Clear counter.
     */
    void OnClearCounter();

    String _name;

    /** various counters
     */
    unsigned _prevUsage, 
	     _currentUsage, 
	     _peakUsage;
    LARGE_INT _totalDuration;
    unsigned _peakDuration;

    time_t _peakUsageTime, _peakDurationTime;
};

#endif // !defined(CT2_DEVUSAGE_H__INCLUDED_)
