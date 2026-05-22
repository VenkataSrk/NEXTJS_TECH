/* $Header: /xGate/hrtimer.h 10    9/17/02 6:08p Bennylp $
 *   
 * XGATE PROJECT
 * (C) 2001 Switchlab, Ltd
 *
 * Time and timer classes.
 *
 * $Log: /xGate/hrtimer.h $
 * 
 * 10    9/17/02 6:08p Bennylp
 * Added proper typecast to reduce warnings.
 * 
 * 9     4/04/02 3:36p Bennylp
 * Added CPU_Timestamp_Clock_Timer
 * 
 * 8     10/17/01 8:15p Bennylp
 * Fixed bug in MM_Timer get_elapsed() calculation
 * 
 * 7     10/17/01 12:44p Bennylp
 * Added multimedia timer.
 * 
 * 6     6/13/01 8:32p Bennylp
 * Changed return values of all operator to boolean.
 * 
 * 5     5/26/01 7:47p Bennylp
 * Fixed bug in Time_Value::operator >
 * 
 * 4     5/03/01 11:51a Bennylp
 * Added more operations to Time_Value.
 * 
 * 3     5/02/01 7:11p Bennylp
 * Added Time_Value class.
 * 
 * 2     4/28/01 6:33a Bennylp
 * Added service profiling and database driver (testing)
 */
#ifndef __XGATE_TIMER_H__
#define __XGATE_TIMER_H__
#include <sys/timeb.h>
#include <sys/time.h>
#include <sqltypes.h>
#include <string.h>
#include <unistd.h>
//#include <signal.h>
//#include <sys/types.h>
#include <time.h>
#include <cassert>
#include <stdint.h>



//linux porting

#define _max(a,b) (((a)>(b))?(a):(b))

typedef struct timecaps_tag TIMECAPS, *PTIMECAPS, *NPTIMECAPS, *LPTIMECAPS;

/* timer device capabilities data structure */
typedef struct timecaps_tag {
  UINT    wPeriodMin;     /* minimum period supported  */
  UINT    wPeriodMax;     /* maximum period supported  */
} TIMECAPS;

//} TIMECAPS, *PTIMECAPS, NEAR *NPTIMECAPS, FAR *LPTIMECAPS;

typedef union _LARGE_INTEGER LARGE_INTEGER_HR;



/* typedef union _LARGE_INTEGER {
   struct {
   unsigned long LowPart;
   long HighPart;
   };
   struct {
   unsigned long LowPart;
   long HighPart;
   } u;

   unsigned long QuadPart;
   } LARGE_INTEGER; */


//
// Utility functions:
//

/** Get the microseconds interval between two times.
*/
inline long timer_diff_time_ms( const timeb *start, const timeb *end)
{
  return (end->time - start->time - 1) * 1000 +
    end->millitm - start->millitm + 1000;
}

/** Get the seconds interval between two times.
*/
inline long timer_diff_time_sec( const timeb *start, const timeb *end)
{
  return timer_diff_time_ms( start, end)/1000;
}


///////////////////////////////////////////////////////////////////////////////
/** This class represents a time value.
*/
class Time_Value {
  public:
    /** Constructor. */
    Time_Value () 
    {
      memset( &timeval_, 0, sizeof(timeval_));
    }
    /** Constructor. */
    explicit Time_Value (const struct timeval & tv)
      : timeval_ (tv)
    {}
    /** Constructor. */
    Time_Value (long sec, long usec)
    { 
      timeval_.tv_sec = sec;
      timeval_.tv_usec = usec;
    }
    /** Copy constructor. */
    Time_Value (const Time_Value & rhs)
      : timeval_ (rhs.timeval_) 
    {}

    static Time_Value get()
    {
      struct timeb tb;
      ftime(&tb);
      return Time_Value(tb.time, tb.millitm*1000);

    }
    /** Get current time. */
    static void get(Time_Value *tv)
    {
      struct timeb tb;
      ftime(&tb);
      tv->timeval_.tv_sec = tb.time;
      tv->timeval_.tv_usec = tb.millitm * 1000;

    }
    /** Set the time. */
    void set(long sec_part, long usec_part)
    {
      timeval_.tv_sec = sec_part;
      timeval_.tv_usec = usec_part;
    }
    /** Get the seconds part. */
    long sec() const 
    {
      return timeval_.tv_sec;
    }
    /** Get the miliseconds part. */
    long msec() const
    {
      return timeval_.tv_usec / 1000;
    }
    /** Get the microseconds part. */
    long usec() const
    {
      return timeval_.tv_usec;
    }
    /** Convert total time interval to miliseconds. */
    double to_msec() const
    {
      double total = sec();
      total *= 1000;
      total += msec();
      return total;
    }
    /** Convert total time interval to microseconds. */
    double to_usec() const
    {
      double total = sec();
      total *= 1000000;
      total += usec();
      return total;
    }
    /** Assignment operator */
    const Time_Value & operator = (const Time_Value & rhs)
    {
      timeval_ = rhs.timeval_;
      return *this;
    }
    /** Addition operator. */
    const Time_Value & operator += (const Time_Value & rhs)
    {
      timeval_.tv_sec += rhs.timeval_.tv_sec;
      timeval_.tv_usec += rhs.timeval_.tv_usec;
      if (timeval_.tv_usec > 1000000) {
        timeval_.tv_sec += (timeval_.tv_usec / 1000000);
        timeval_.tv_usec %= 1000000;
      }
      return *this;
    }
    /** Substraction operator. */
    const Time_Value & operator -= (const Time_Value & rhs)
    {
      timeval_.tv_sec -= rhs.timeval_.tv_sec;
      timeval_.tv_usec -= rhs.timeval_.tv_usec;
      if (timeval_.tv_usec < 0) {
        --timeval_.tv_sec;
        timeval_.tv_usec += 1000000;
      }
      return *this;
    }
    /** Equality operator. */
    bool operator == (const Time_Value & rhs) const
    {
      return timeval_.tv_sec == rhs.timeval_.tv_sec &&
        timeval_.tv_usec == rhs.timeval_.tv_usec;
    }
    /** Not equal operator. */
    bool operator != (const Time_Value & rhs) const
    {
      return !operator==(rhs);
    }
    /** Less than operator. */
    bool operator < (const Time_Value & rhs) const
    {
      return timeval_.tv_sec < rhs.timeval_.tv_sec ||
        ((timeval_.tv_sec == rhs.timeval_.tv_sec) && (timeval_.tv_usec < rhs.timeval_.tv_usec));
    }
    /** Less than or equal operator. */
    bool operator <= (const Time_Value & rhs) const
    {
      return operator==(rhs) || operator<(rhs);
    }
    /** Greater than operator. */
    bool operator > (const Time_Value & rhs) const
    {
      return !operator <=(rhs);
    }
    /** Greater than or equal operator. */
    bool operator >= (const Time_Value & rhs) const
    {
      return !operator<(rhs);
    }

  private:
    struct timeval timeval_;
};


/** Addition operator. */
inline Time_Value  operator + (const Time_Value & lhs, const Time_Value & rhs)
{
  Time_Value result(lhs);
  return result.operator += (rhs);
}

/** Substraction operator. */
inline Time_Value  operator - (const Time_Value & lhs, const Time_Value & rhs)
{
  Time_Value result(lhs);
  return result.operator -= (rhs);
}

///////////////////////////////////////////////////////////////////////////////
/** This class represents low resolution timer (maximum resolution is 
  miliseconds)
  */
class Low_Res_Timer {
  public:
    /** Start the timer.
    */
    void start() { 
      ftime(&timerStartTime); 
    }

    /** Stop the timer.
    */
    void stop() { 
      ftime(&timerStopTime); 
    }

    /** Call this function after stop() to get the time difference, in second
      precision.
      */
    long get_elapsed_sec() const {
      return get_elapsed_msec()/1000;
    }

    /** Call this function after stop() to get the time difference, in miliseconds
      precision.
      */
    long get_elapsed_msec() const {
      return timer_diff_time_ms( &timerStartTime, &timerStopTime);
    }

    /** Get the elapsed time.
    */
    Time_Value get_elapsed() const {
      long msec_elapsed = get_elapsed_msec();
      return Time_Value(msec_elapsed/1000, (msec_elapsed % 1000)*1000);
    }

    /** Get the elapsed time.
    */
    void get_elapsed(Time_Value *elapsed) const {
      long msec_elapsed = get_elapsed_msec();
      elapsed->set(msec_elapsed/1000, (msec_elapsed % 1000)*1000);
    }

    /** Call this function after start() to get the current elapsed time, in
      seconds precision.
      */
    long get_current_sec() const {
      timeb current;
      ftime(&current);
      return timer_diff_time_sec( &timerStartTime, &current);
    }

    /** Call this function after start() to get the current elapsed time, in
      miliseconds precision.
      */
    long get_current_msec() const {
      timeb current;
      ftime(&current);
      return timer_diff_time_ms( &timerStartTime, &current);
    }

    /** Get the current time elapsed.
    */
    Time_Value get_current() const {
      long msec_current = get_current_msec();
      return Time_Value (msec_current/1000, (msec_current % 1000)*1000);
    }

    /** Get the current time elapsed.
    */
    void get_current(Time_Value *current) const {
      long msec_current = get_current_msec();
      current->set(msec_current/1000, (msec_current % 1000)*1000);
    }

  private:
    struct timeb   timerStartTime, timerStopTime;
};

/* Linux Porting */

/* Helpful conversion constants. */
static const unsigned usec_per_sec = 1000000;
static const unsigned usec_per_msec = 1000;

/* These functions are written to match the win32
 *    signatures and behavior as closely as possible.
 *    */
bool QueryPerformanceFrequency(unsigned long *frequency)
{
  /* Sanity check. */
  assert(frequency != NULL);

  /* gettimeofday reports to microsecond accuracy. */
  *frequency = usec_per_sec;

  return true;
}

bool QueryPerformanceCounter(unsigned long *performance_count)
{
  struct timeval time;

  /* Sanity check. */
  assert(performance_count != NULL);

  /* Grab the current time. */
  gettimeofday(&time, NULL);
  *performance_count = time.tv_usec + /* Microseconds. */
    time.tv_sec * usec_per_sec; /* Seconds. */

  return true;
}

///////////////////////////////////////////////////////////////////////////////
/** This class describes a high resolution timer. The maximum resolution 
  supported is dependent on the processor and operating system.
  The number of concurrent high resolutin instance that can be running
  simultaneously is limited.
  */
class High_Res_Timer {
  public:
    /** Default constructor.
    */
    High_Res_Timer() {
      timerFreq = timerStartTime = timerStopTime = 0;
    }

    /** Initialize the timer.
      This will retrieve the frequency of the high resolution timer.
      @return false if the timer can't be created.
      */
    bool init(){
      //return ::QueryPerformanceFrequency(&timerFreq) ? true : false;
      return QueryPerformanceFrequency(&timerFreq) ? true : false;
    }

    /** Call this function after init() to get the timer resolution, in
      microseconds.
      */
    unsigned get_resolution_usec() const {
      return (unsigned)(1000000.0 / timerFreq);
    }

    /** Start the timer.
    */
    void start() {
      ::QueryPerformanceCounter(&timerStartTime);
    }

    /** Stop the timer.
    */
    void stop() {
      ::QueryPerformanceCounter(&timerStopTime);
    }

    /** Call this function after stop() to get the elapsed time, in miliseconds
      precision.
      */
    unsigned long get_elapsed_msec() const {	
      return (unsigned long)(1000.0 / timerFreq *
          (timerStopTime-timerStartTime));
    }

    /** Call this function after stop() to get the elapsed time, in microseconds
      precision.
      */
    unsigned long get_elapsed_usec() const {
      return (unsigned long)(1000000.0 / timerFreq *
          (timerStopTime-timerStartTime));
    }

    /** Call this function after stop() to get the elapsed time, in nanoseconds
      precision.
      */
    unsigned long get_elapsed_nsec() const {
      return (unsigned long)(1000000.0 * 1000.0 / timerFreq * 
          (timerStopTime - timerStartTime));
    }

    /** Get the elapsed time.
    */
    Time_Value get_elapsed() const {
      unsigned long usec_elapsed = get_elapsed_usec();
      return Time_Value (usec_elapsed / 1000000, usec_elapsed % 1000000);
    }

    /** Get the elapsed time.
    */
    void get_elapsed(Time_Value *elapsed) const {
      unsigned long usec_elapsed = get_elapsed_usec();
      elapsed->set(usec_elapsed / 1000000, usec_elapsed % 1000000);
    }

  private:
    unsigned long timerFreq;
    unsigned long timerStartTime, timerStopTime;
};


///////////////////////////////////////////////////////////////////////////////
/** High resolution timer using multimedia timer.
  Typical resolution is one milisecond.
  */
//#include <mmsystem.h>

class MM_Timer {
  public:
    /** Initialize the timer.
      This will just check whether the static initialization has been performed.
      All instances of MM_Timer will have the same resolution.
      */
    bool init() {
      if (!is_initialized)
        return initialize();
      return timerResolution != 0;
    }
    /** Call this function after init() to get the timer resolution, in
      microseconds.
      */
    unsigned get_resolution_usec() const {
      return timerResolution * 1000;
    }
    /** Start timing.
    */
    void start() {
      //Linux Porting
      //timerStart = timeGetTime();
      struct timeval tim;
      gettimeofday(&tim, NULL);
      timerStart=tim.tv_sec+(tim.tv_usec/1000000.0); 

    }
    /** End timing.
    */
    void stop() {
      //timerEnd = timeGetTime();
      struct timeval tim;
      gettimeofday(&tim, NULL);
      timerEnd=tim.tv_sec+(tim.tv_usec/1000000.0);
    }
    /** Get the elapsed time.
    */
    void get_elapsed(Time_Value * elapsed) const {
      elapsed->set( (timerEnd-timerStart)/1000, ((timerEnd-timerStart) % 1000) * 1000 );
    }
    /** Get the elapsed time, in miliseconds.
    */
    unsigned long get_elapsed_msec() const {	
      return timerEnd - timerStart;
    }
    /** Get the elapsed time, in microseconds.
    */
    unsigned long get_elapsed_usec() const {	
      return (timerEnd - timerStart) * 1000;
    }

  private:
    DWORD timerStart, timerEnd;

    static UINT timerResolution;
    static bool is_initialized;
    static bool initialize();
};

/* assembly code to read the TSC */
static inline uint64_t RDTSC()
{
  unsigned int hi, lo;
  __asm__ volatile("rdtsc" : "=a" (lo), "=d" (hi));
  return ((uint64_t)hi << 32) | lo;
}

//inline unsigned __int64 Xg_get_cpu_ticks_64() 
inline unsigned long Xg_get_cpu_ticks_64() 
{
  //Linux Porting
  unsigned long long ts;
#if 0
  __asm {
    rdtsc
      mov [DWORD PTR ts + 0], eax
      mov [DWORD PTR ts + 4], edx
  }
#endif
  ts = RDTSC(); 
  return ts;
}


inline unsigned long Xg_get_cpu_ticks_32() 
{
  unsigned long long ts;
  //Linux Porting
#if 0
  __asm {
    rdtsc
      mov ts, eax
  }
#endif
  ts = RDTSC();
  return ts;
}

///////////////////////////////////////////////////////////////////////////////
/** Very very high resolution timing.
  This uses Read-Timestamp-Clock (rdtsc) instruction found on Intel Pentium 
  or above processor, which returns the number of cycles (!) that has elapsed
  since the CPU was started.
  */
class CPU_Timestamp_Clock_Timer {
  public:
    CPU_Timestamp_Clock_Timer()
      : tick_start_(0), tick_stop_(0) {}


    /** Initialize the timer.
      This will retrieve the frequency of the high resolution timer.
      @return false if the timer can't be created.
      */
    static bool init() {
      if (state_ == State_Ok)
        return true;
      else if (state_ == State_Uninitialized)
        state_ = initialize();

      state_ == State_Ok;
      //return state_ == State_Ok;
      return true;
    }

    /** Call this function after init() to get the timer resolution, in
      microseconds.
      */
    unsigned get_resolution_usec() const {
      return 1;
    }

    /** Get the clock cycles per micro-seconds.
    */
    static double get_clocks_per_usec() {
      if (init() == false)
        return 0;
      return ticks_per_usec_;
    }

    /** Start the timer.
    */
    void start() {
      tick_start_ = Xg_get_cpu_ticks_64();	
    }

    /** Stop the timer.
    */
    void stop() {
      tick_stop_ = Xg_get_cpu_ticks_64();
    }

    /** Call this function after stop() to get the elapsed time, in miliseconds
      precision.
      */
    unsigned long get_elapsed_msec() const {	
      return get_elapsed_usec() / 1000;
    }

    /** Call this function after stop() to get the elapsed time, in microseconds
      precision.
      */
    unsigned long get_elapsed_usec() const {
      return (unsigned)((tick_stop_ - tick_start_) / ticks_per_usec_);
    }

    /** Call this function after stop() to get the elapsed time, in nanoseconds
      precision.
      */
    unsigned long get_elapsed_nsec() const {
      return (unsigned long)((tick_stop_ - tick_start_) / (ticks_per_usec_ / 1000));
    }

    /** Call this function after stop() to get the elapsed time in CPU ticks.
    */
    unsigned long get_elapsed_ticks32() const {
      return (unsigned long)(tick_stop_ - tick_start_);
    }

    /** Call this function after stop() to get the elapsed time in CPU ticks.
    */
    unsigned long get_elapsed_ticks64() const {
      return tick_stop_ - tick_start_;
    }

    /** Get the elapsed time.
    */
    Time_Value get_elapsed() const {
      unsigned long us_elapsed = get_elapsed_usec();
      return Time_Value( us_elapsed/1000000, us_elapsed % 1000000 );
    }

    /** Get the elapsed time.
    */
    void get_elapsed(Time_Value *elapsed) const {
      unsigned long us_elapsed = get_elapsed_usec();
      elapsed->set( us_elapsed/1000000, us_elapsed % 1000000 );
    }

  private:
    enum State {
      State_Uninitialized,
      State_Error,
      State_Ok,
    };

    static State state_;
    static double ticks_per_usec_;
    unsigned long tick_start_, tick_stop_;

    static State initialize();

};

typedef CPU_Timestamp_Clock_Timer Highest_Res_Timer;

#endif
