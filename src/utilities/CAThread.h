#ifndef CA_UTILITY_THREADS
#define CA_UTILITY_THREADS
#include "CAUtilites.h"
#include "CASTLBase.h"
#pragma once
namespace CA
{
 
 class CAThread : public CAThreadInterface {
    private: //member variables
     std::string m_strLastError;
     THREADSTATE m_iThreadState;
     int m_bEnableLazLoading; // for using future STL
	 std::thread m_stThread;
     CAQueue<CAMessageBlock*> m_cThreadQueue;

     std::string m_strThreadName;
     
     
		 
    public : //member variable
        ConditionalEventWait m_cWaitEvent;

    private: //member function
        bool start(CAThread& );
        bool join();
        bool pause(int iseconds); //put to sleep 
        bool resume(); // wake up
        bool stop(); // stop the thread grafully
        bool detach();
     // enable or disable the thread 
       bool setThreadState(THREADSTATE);
       bool m_threadStatus = false;
     
     
     public: // member functions
     CAThread() = default;
     
    
     virtual ~CAThread() {
         try {
             if (m_stThread.joinable())
                 m_stThread.join();
             
         }
         catch (...) {
             
         }

     }
     void setThreadStatus(bool status){
        m_threadStatus = status;
    }
    bool getThreadStatus(){
        return m_threadStatus;  
    }
	  template<class Function, class ... Argument>
      explicit CAThread(Function&& function, Argument&& ... arg){
//				std::thread workerThread(function, arg ...);
                m_stThread = std::move(std::thread(function, arg ...));
                detach();
                
			}

     template<class Function, class ... Argument>
      bool initialize(Function&& function, Argument&& ... arg){
//				std::thread workerThread(function, arg ...);
				m_stThread = std::move(std::thread(function,arg ...));
                setThreadState(THREADSTATE::IDLE);
				return true;
      }

	virtual  bool run()  ; 
   virtual bool spwan(CAThread&);
    std::thread::id getId();
    virtual bool PostMessage(CAMessageBlock* msgBlock);
    virtual bool PostMessage(std::unique_ptr<CAMessageBlock> upmsgBlock);
    CAMessageBlock* RecvMessage();
    void clearQ();
    
    THREADSTATE getThreadState();
    void setThreadName(const char* threadName){
        m_strThreadName.assign(threadName);
    }
    const char* getThreadName(){
        return m_strThreadName.c_str();
    }
    
    
  };

 static void ThreadBody(CAThread* instance) {
     //try {
         if (instance)
             instance->run();
     //}
     //catch (...) {

     //}
 }
}
#endif
