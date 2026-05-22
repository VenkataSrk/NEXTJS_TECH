#ifndef _TIMED_STATISTIC_INCLUDED_
#define _TIMED_STATISTIC_INCLUDED_

#include <iostream>
#include <ostream>
#include <list>
#include <iterator>

using namespace std;
using std::list;

/** T  
    - must be signed
    - T = -1 -> to put T into invalid state
    - T < 0  -> to check if T is invalid
*/

template <class T, 
	  unsigned Hourly_Interval, 
	  unsigned Size>
//class timed_statistic
class timed_statistic : public List<std::pair<T,unsigned> > {
public:
    enum {
	Seconds_Interval = Hourly_Interval * 3600
    };
    typedef std::pair<T,unsigned> value_type;


    timed_statistic() {
	T T_INVALID;
	T_INVALID = -1;

	for (int i=0; i<Size; ++i) {
	    value.push_back(value_type(T_INVALID, 0));
	}

	t_last = time(NULL);
	struct tm* now;
	now = localtime(&t_last);
	now->tm_min = 0;
	now->tm_sec = 0;
	t_last = mktime(now);
    }

    ~timed_statistic() {
    }

    void set_current_hour() {
	int distance = (time(NULL) - t_last) / Seconds_Interval;

	t_last += distance * Seconds_Interval;

	// delete previous elements if necessary.
	if (distance >= Size)
	    value.clear();
	else {
	    for (int i=0; i<distance; ++i)
		value.pop_front();
	}

	// insert new elements if necessary
	T T_INVALID;
	T_INVALID = -1;
	if (distance >= Size) {
	    for (int i=0; i<Size; ++i) {
		value.push_back(value_type(T_INVALID, 0));
	    }
	} else {
	    for (int i=0; i<distance; ++i)
		value.push_back(value_type(T_INVALID, 0));
	}
    }

    void add(const T & val) {
	if (val < 0) return;

	set_current_hour();

	// update the value.
	value_type & current = value.back();
	if (current.first < 0)
	    current.first = val;
	else
	    current.first += val;
	++current.second;
    }

    void add_avg(const T & val) {
	if (val < 0) return;

	set_current_hour();

	// update the value.
	value_type & current = value.back();
	if (current.first < 0)
	    current.first = val;
	else
	    current.first = (current.first*current.second + val) / (current.second + 1);
	++current.second;
    }

    //The below code commented for linux porting need to re write linux equivalent
     typename List<value_type>::const_iterator get_iterator(time_t t) const {
	time_t t_front = t_last - ((Size - 1) * Seconds_Interval);
	if (t < t_front)
	    return value.end();
	if (t > t_last)
	    return value.end();

	int distance = (t - t_front) / Seconds_Interval;
	typename List<value_type>::const_iterator it = value.begin();
	std::advance(it, distance);
	return it;
    } 

     T calculate_sum(time_t t1, time_t t2)
    {
	T T_INVALID;
	T_INVALID = -1;
	if (t1 > t2) return T_INVALID;


	typename List<value_type>::const_iterator it = get_iterator(t1);
	if (it == value.end()) return T_INVALID;
	typename List<value_type>::const_iterator end = get_iterator(t2);

	value_type result(T_INVALID,0);

	for (;it!=end;it++) {
	    if ((*it).first < 0) continue;
	    if (result.first < 0)
		result.first = (*it).first;
	    else
		result.first = result.first + (*it).first;
	}

	return result.first;
    }

    T calculate_avg(time_t t1, time_t t2, unsigned* weight = 0) {

	T T_INVALID;
	T_INVALID = -1;
	if (t1 > t2) return T_INVALID;

	typename List<value_type>::const_iterator it = get_iterator(t1);
	if (it == value.end()) return T_INVALID;
	typename List<value_type>::const_iterator end = get_iterator(t2);

	value_type result(T_INVALID,0);
	for (;it!=end;it++) {
	    if ((*it).first < 0) continue;
	    if (result.first < 0)
		result.first = (*it).first;
	    else
		result.first = (result.first * result.second + (*it).first*(*it).second) / (result.second + (*it).second);
	    result.second += (*it).second;
	}
	if (weight) *weight = result.second;
	return result.first;

    } 

private:
    List<value_type> value;
    time_t   t_last;
};




#endif
