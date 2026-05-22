// $Header: /slrap/slrap/Include/Critical_Section.h 1     8/29/01 4:01p Iwanj $

#ifndef INC_CRITICAL_SECTION
#define INC_CRITICAL_SECTION

//#include <windows.h>

//#include <pch1.h>

typedef struct _RTL_CRITICAL_SECTION RTL_CRITICAL_SECTION;

class Critical_Section
{
public:
    Critical_Section()
    {
      //  InitializeCriticalSection(&handle_);
          //pthread_mutex_t handle_;

	// setup recursive mutex for mutex attribute
	pthread_mutexattr_settype(&mAttr, PTHREAD_MUTEX_RECURSIVE_NP);

	// Use the mutex attribute to create the mutex
	pthread_mutex_init(&handle_, &mAttr);

    }

    ~Critical_Section()
    {
     //   DeleteCriticalSection(&handle_);
           //pthread_mutex_t handle_;
	pthread_mutexattr_destroy(&mAttr);
	pthread_mutex_destroy(&handle_);
    }

    void acquire()
    {
     //    EnterCriticalSection(&handle_);
            //pthread_mutex_t handle_;
	 while( pthread_mutex_trylock(&handle_) != 0)
         {
             static int cnt; 
             int *ptr=NULL; 
             printf("\n Waiting to UnLock %02x\n", this);
             cnt++; 
             if(cnt == 20)
             {
                ptr=1000;
                cnt = 0;
             }
         }
	 //if( pthread_mutex_trylock(&handle_) != 0){ printf("\n Waiting to UnLock \n"); };
//         printf("\n Trylock Ok %02x \n", this);
    }

    void release()
    {
       // LeaveCriticalSection(&handle_);
       //pthread_mutex_t handle_;
         printf("\n TryUnlock Ok %02x \n", this);
	  pthread_mutex_unlock(&handle_);
    }

private:
    //CRITICAL_SECTION handle_;
    pthread_mutex_t handle_;

    //create mutex attribute variable
    pthread_mutexattr_t mAttr;

};

class Lock
{
public:
    Lock(Critical_Section& critical_section):
        critical_section_(critical_section)
    {
        critical_section_.acquire();	
    }

    ~Lock()
    {
        critical_section_.release();
    }

private:
    Critical_Section& critical_section_;
};

#endif
