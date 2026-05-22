/*----------------------------------------------------------------* 
 * $Archive: /xGate/DevUsage.cpp $
 * $Date: 5/29/02 3:09p $
 * $Revision: 6 $
 *
 * $History: DevUsage.cpp $
 * 
 * *****************  Version 6  *****************
 * User: Sonny        Date: 5/29/02    Time: 3:09p
 * Updated in $/xGate
 * peak time is now the last peak time
 * 
 * *****************  Version 5  *****************
 * User: Sonny        Date: 5/23/02    Time: 7:12p
 * Updated in $/xGate
 * added peak time on "show device-usage"
 * 
 * *****************  Version 4  *****************
 * User: Bennylp      Date: 8/20/01    Time: 6:43p
 * Updated in $/xGate
 * Fixed bug: regular expression handle is not freed.
 * 
 * *****************  Version 3  *****************
 * User: Bennylp      Date: 8/20/01    Time: 6:41p
 * Updated in $/xGate
 * Some dev-usage command improvements/bug fixes.
 * 
 * *****************  Version 2  *****************
 * User: Junanto      Date: 17/08/01   Time: 14:09
 * Updated in $/xGate
 * DeviceUsageManager is allocated from heap
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


//#include "stdafx.h"
#include "devusage.h"
#include "kernel.h"
#include "term.h"
//#include "pcre-3.4/pcreposix.h"	// Regex
#include <regex.h>
//-----------------------------------------------------------------



/*----------------------------------------------------------------* 
 * Implementation of DeviceUsageManager
 *----------------------------------------------------------------*/
/** Constructor
 */
DeviceUsageManager::DeviceUsageManager() : ServiceProvider("devusage",false,false)
{
    _lastReset = 0;
}

/** Destructor
 */
DeviceUsageManager::~DeviceUsageManager()
{
    // do nothing
}

/** Returns a pointer to the singleton
 */
DeviceUsageManager* DeviceUsageManager::Instance()
{
    static DeviceUsageManager* theDeviceUsageManager = NULL;
    if (theDeviceUsageManager == NULL) theDeviceUsageManager = new DeviceUsageManager;
    return theDeviceUsageManager;
}

/** Add/remove an DeviceUsage instance to the list
 */
void DeviceUsageManager::InsertDeviceUsage(DeviceUsage* dev)
{
    _list.push_back(dev);
}

/** Remove an DeviceUsage instance from the list
 */
void DeviceUsageManager::RemoveDeviceUsage(DeviceUsage* dev)
{
    _list.remove(dev);
}

void DeviceUsageManager::OnClearCounter()
{
    ListDeviceUsage::iterator it = _list.begin();
    ListDeviceUsage::iterator end = _list.end();
    while (it != end) {
	DeviceUsage* usage = *it;
	usage->OnClearCounter();
	it++;
    }
    _lastReset = time(NULL);
}

/* Sort device usage based on the average.
 */
bool DeviceUsageManager::SortDev_Avg_Asc(const DeviceUsage *d1, const DeviceUsage *d2)
{
    unsigned av1 = d1->GetAvgDuration();
    unsigned av2 = d2->GetAvgDuration();
    return av1==av2 ? d1->GetTotalUsage() < d2->GetTotalUsage() : av1 < av2;
}

bool DeviceUsageManager::SortDev_Avg_Desc(const DeviceUsage *d1, const DeviceUsage *d2)
{
    unsigned av1 = d1->GetAvgDuration();
    unsigned av2 = d2->GetAvgDuration();
    return av1==av2 ? d1->GetTotalUsage() > d2->GetTotalUsage() : av1 > av2;
}

void DeviceUsageManager::ShowDeviceUsage(Term_Session* session, const char *namespec)
{
    String buffer;
    const String indent = " ";

    // Compile regular expression
    regex_t reg;
    memset(&reg, 0, sizeof(reg));

    if (regcomp(&reg, namespec, REG_ICASE|REG_EXTENDED)) {
	session->SendErrorf("Invalid regular expression '%s'\n", namespec);
	return;
    }

    ACQUIRE_SYSTEM_LOCK();

    if (_lastReset) {
	buffer += indent + "Device usage last reset: ";
	const char* s = ctime(&_lastReset);
	String temp = s ? s : "";
	if (temp.length()) {
	    temp.erase(temp.length()-1,1);
	    temp += " [" + ConvertInterval2String(time(NULL)-_lastReset) + " ago]\n";
	}
	buffer += temp;
    } else {
	buffer += indent + "Device usage last reset never\n";
    }
    buffer += indent + "Output sorted by: Average Duration -> Total Usage\n"
	      "\n";
    buffer += indent + "Device Name      Usage Counter           Duration             Peak Time\n";
    buffer += indent + "              Total  Curr.   Peak   Peak         Average\n";
    buffer += indent + "------------------------------------------------------------------------------\n";


    // Copy device usage list to a Vector so that it can be sorted.
    Vector<DeviceUsage*> dev_array;
    dev_array.resize(_list.size());
    std::copy(_list.begin(), _list.end(), dev_array.begin());
    std::sort(dev_array.begin(), dev_array.end(), &SortDev_Avg_Desc);



    Vector<DeviceUsage*>::iterator it = dev_array.begin();
    Vector<DeviceUsage*>::iterator end = dev_array.end();
    while (it != end) {
	char str_peak_time[100];
	char* peak_time;
	DeviceUsage* usage = *it++;

	if (regexec(&reg, usage->GetName(), 0, NULL, 0) != 0)
	    continue;

	char line[200];
	if (usage->_peakUsage > 1) {
	    strcpy(str_peak_time, ctime(&usage->_peakUsageTime));
	}
	else {
	    strcpy(str_peak_time, ctime(&usage->_peakDurationTime));
	}
	peak_time = &str_peak_time[4]; str_peak_time[19] = 0;
	sprintf(line,"%-12s %6u  %5u  %5u   %-11s  %-11s  %s\n",
	    usage->GetName(),
	    usage->_prevUsage + usage->_currentUsage,
	    usage->_currentUsage,
	    usage->_peakUsage,
	    ConvertInterval2String(usage->_peakDuration).c_str(),
	    ConvertInterval2String(usage->GetAvgDuration()).c_str(),
	    peak_time);
	buffer += indent + line;
    }
    buffer += indent + "------------------------------------------------------------------------------\n";
    buffer += "\n";

    RELEASE_SYSTEM_LOCK();

    ::regfree(&reg);
    session->Send(buffer.c_str());
}

/*----------------------------------------------------------------* 
 * Implementation of DeviceUsage
 *----------------------------------------------------------------*/
DeviceUsage::DeviceUsage(const String& name)
{
    DeviceUsageManager::Instance()->InsertDeviceUsage(this);
    _peakDuration = _prevUsage = _peakUsage = _currentUsage = 0;
    _peakDurationTime = _peakUsageTime = time(NULL);
    _totalDuration = 0;
    _name = name;
}

DeviceUsage::~DeviceUsage()
{
    DeviceUsageManager::Instance()->RemoveDeviceUsage(this);
}

const char* DeviceUsage::GetName() const
{
    return _name.c_str();
}

void DeviceUsage::OnClearCounter()
{
    _peakUsage = _currentUsage;
    _prevUsage = _peakDuration = _totalDuration = 0;
    _peakUsageTime = time(NULL);
    _peakDurationTime = time(NULL);
}

void DeviceUsage::StartUsage()
{
    _currentUsage++;
    if (_currentUsage >= _peakUsage) {
	_peakUsageTime = time(NULL);
	_peakUsage = _currentUsage;
    }
}

void DeviceUsage::StopUsage(unsigned duration)
{
    _prevUsage++;

    if (duration >= _peakDuration) {
	_peakDurationTime = time(NULL);
	_peakDuration = duration;
    }
    _currentUsage--;
    _totalDuration += duration;
}

