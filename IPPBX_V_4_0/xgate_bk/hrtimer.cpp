/* $Header: /xGate/hrtimer.cpp 4     17/05/02 19:11 Junanto $ */
/* $Log: /xGate/hrtimer.cpp $
 * 
 * 4     17/05/02 19:11 Junanto
 * max --> __max and commented ACE_Reactor instantiation
 * 
 * 3     4/04/02 3:36p Bennylp
 * Added CPU_Timestamp_Clock_Timer
 * 
 * 2     10/17/01 12:44p Bennylp
 * Initial revision
 * 
 * 1     10/17/01 12:43p Bennylp
 * Initial revision
 */
//#include "stdafx.h"
#include <stdio.h>
#include "hrtimer.h"
#include <sys/time.h>
#include <sys/resource.h>
#include <poll.h>
#include <sched.h>
#include <pthread.h>
//#include <math.h>        // std::abs
#include <stdlib.h>

UINT MM_Timer::timerResolution;
bool MM_Timer::is_initialized;

void Sleep( unsigned long ms )
{
#ifndef __linux__
        ::Sleep(ms);
#else
        poll(0,0,ms); // __linux__
#endif
};


CPU_Timestamp_Clock_Timer::State CPU_Timestamp_Clock_Timer::state_ = CPU_Timestamp_Clock_Timer::State_Uninitialized;
double CPU_Timestamp_Clock_Timer::ticks_per_usec_ = 1;

bool MM_Timer::initialize() 
{
    // Class to automatically close the timer when the program exits
    static struct Shutdown {
	UINT resolution;
	~Shutdown() {
	    if (resolution != 0){
		//::timeEndPeriod(resolution);

		//linux porting
		struct itimerval itv;
  		/* Shut off the timer */
  		memset(&itv,0,sizeof(struct itimerval));
  		setitimer(ITIMER_REAL,&itv,NULL);
		}
	}
    } shut;

    // The desired resolution, in msec
    enum {
	time_resolution = 1
    };

    assert(is_initialized == false);
    is_initialized = true;

    TIMECAPS tc;
    //if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR)
	//return false;

     struct timespec ts;
     if (clock_getres(CLOCK_MONOTONIC, &ts)) { // it is not supported if this returns non zero
        printf("clock_getres returning false\n");
	//return false;
        }



    //timerResolution = __max(tc.wPeriodMin, time_resolution);
    timerResolution = _max(tc.wPeriodMin, time_resolution);

    /* if (timeBeginPeriod(timerResolution) != TIMERR_NOERROR) {	//linux porting need to re write
	timerResolution = 0;
	return false;
    } */

    shut.resolution = timerResolution;
    return true;
}


/*
CPU_Timestamp_Clock_Timer::State
CPU_Timestamp_Clock_Timer::initialize()
{
    DWORD processPriority;
    int threadPriority;

    int i;
    //unsigned __int64 nTicks;
    unsigned long nTicks;
    unsigned nTotalTicks = 0x10000000;
    double usec_elapsed = 0;
    //LARGE_INTEGER tFreq, tStart, tStop;
    unsigned long int tFreq, tStart, tStop;
    //LARGE_INTEGER_HR tFreq, tStart, tStop;

    enum { LOOP = 1 };

    // Boost process priority
    for (i=0; i<LOOP; ++i) {
	Sleep(100);

	//processPriority = GetPriorityClass( GetCurrentProcess() );
    	processPriority = getpriority( getpid(), _who );
	SetPriorityClass  ( GetCurrentProcess(), REALTIME_PRIORITY_CLASS);

	threadPriority = GetThreadPriority( GetCurrentThread() );
	//SetThreadPriority ( GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
        sched_getscheduler( pthread_self());

	// Get current time
	QueryPerformanceFrequency(&tFreq);
	QueryPerformanceCounter(&tStart);

	__asm 
	{
	    pushfd
	    pushad

	    rdtsc
	    mov edi, eax
	    mov ebx, edx
	    mov ecx, nTotalTicks
	here:
	    loop here

	    rdtsc
	    sub eax, edi
	    sbb edx, ebx

	    mov [DWORD PTR nTicks + 0], eax
	    mov [DWORD PTR nTicks + 4], edx

	    popad
	    popfd
	}
	QueryPerformanceCounter(&tStop);

	// Restore priority
	SetPriorityClass( GetCurrentProcess(), processPriority);
	//SetThreadPriority( GetCurrentThread(), threadPriority);
    	sched_getscheduler( pthread_self());

	usec_elapsed += (unsigned long)(1000000.0 / tFreq.QuadPart * (tStop.QuadPart-tStart.QuadPart));
    }

    ticks_per_usec_ = nTotalTicks * LOOP / usec_elapsed;
    return State_Ok;
}
*/



/* Get the CPU clocks (in Mhz)
 */
static double get_cpu_clock_mhz()
{
    unsigned nTotalTicks = 0x1000000;
    unsigned long ticks;
    unsigned long tFreq, tStart, tStop;
    //LARGE_INTEGER_HR tFreq, tStart, tStop;

    // Adjust priority to REALTIME!
    //DWORD processPriority;
    unsigned long processPriority;
    int threadPriority;
    int _who=0;
    //processPriority = GetPriorityClass( GetCurrentProcess() );
    processPriority = getpriority( getpid(), _who );
    //SetPriorityClass  ( GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
    //setpriority( getpid(), _who, REALTIME_PRIORITY_CLASS);

      // Increase the priority
        struct sched_param param;  // scheduling priority
        int policy = SCHED_RR;     // scheduling policy

        // Get the current thread id
        pthread_t thread_id = pthread_self();

        // To set the scheduling priority of the thread
        param.sched_priority = 90;
        pthread_setschedparam(thread_id, policy, &param);


    //threadPriority = GetThreadPriority( GetCurrentThread() );
    threadPriority = getpriority( pthread_self() , _who);
    //SetThreadPriority ( GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    sched_getscheduler( pthread_self());

    Sleep(10);

    // Calculate overhead.
    unsigned long ticks_overhead = 0;
    ticks = Xg_get_cpu_ticks_64();
    Xg_get_cpu_ticks_64();
    ticks_overhead += (Xg_get_cpu_ticks_64() - ticks);

    ticks = Xg_get_cpu_ticks_64();
    do {
	//LARGE_INTEGER dummy64;
	unsigned long dummy64;
	//LARGE_INTEGER_HR dummy64;
	::QueryPerformanceCounter(&dummy64);
	::QueryPerformanceCounter(&dummy64);
    } while (0);
    ticks_overhead += (Xg_get_cpu_ticks_64() - ticks);

    Sleep(10);

    // Do the timings
    //QueryPerformanceFrequency(&tFreq);
    //QueryPerformanceCounter(&tStart);
    /* __asm 
    {
	pushfd
	pushad

	rdtsc
	mov edi, eax
	mov ebx, edx
	mov ecx, nTotalTicks
    here:
	loop here

	rdtsc
	sub eax, edi
	sbb edx, ebx

	mov [DWORD PTR ticks + 0], eax
	mov [DWORD PTR ticks + 4], edx

	popad
	popfd
    } */
    //QueryPerformanceCounter(&tStop);

    // Restore priority
    //SetPriorityClass( GetCurrentProcess(), processPriority);
          // Increase the priority
        struct sched_param _param;  // scheduling priority
        int _policy = SCHED_RR;     // scheduling policy

        // Get the current thread id
        pthread_t _thread_id = pthread_self();

        // To set the scheduling priority of the thread
        _param.sched_priority = 90;
        pthread_setschedparam(_thread_id, _policy, &_param);

    //SetThreadPriority( GetCurrentThread(), threadPriority);
    sched_getscheduler( pthread_self());

    // Calculate
    /* ticks += ticks_overhead;
    long iticks = ticks;
    double hz =  1.0 * iticks * tFreq.QuadPart / (tStop.QuadPart-tStart.QuadPart);
    return hz / 1000000.0; */
}


/* Normalize CPU clock
 */
static int
normalize_cpu_clock_mhz(int mhz)
{
    int mhz_low100 = (mhz / 100) * 100;
    int speed_inc[] = { 0, 22, 33, 50, 60, 66, 75, 80, 100};

    for (int i=0; i<sizeof(speed_inc)/sizeof(speed_inc[0]); ++i) {
	if (abs( mhz-mhz_low100-speed_inc[i]) <= 2)
	    return mhz_low100 + speed_inc[i];
    }
    return mhz;
}



CPU_Timestamp_Clock_Timer::State
CPU_Timestamp_Clock_Timer::initialize()
{
    enum { LOOP = 10 };
    ticks_per_usec_ = 0;

    // Loop to get the best clocks!
    double largest_ticks = 0;
    for (int i=0; i<LOOP; ++i) {
	double ticks = get_cpu_clock_mhz();
	if (ticks > largest_ticks)
	    largest_ticks = ticks;
    }
    ticks_per_usec_ = largest_ticks;
    //ticks_per_usec_ = normalize_cpu_clock_mhz(ticks_per_usec_);
    return ticks_per_usec_==0 ? State_Error : State_Ok;
}
