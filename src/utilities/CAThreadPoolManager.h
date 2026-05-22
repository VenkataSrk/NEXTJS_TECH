#ifndef  CA_UTILITY_THREADPOOLMANAGER
#define  CA_UTILITY_THREADPOOLMANAGER
#include "CAUtilites.h"
#include "CASTLBase.h"
#pragma once
namespace CA {

	class CAThreadPoolManager;
	class CAJobThread  {
		private :
		THREADSTATE m_enuThreadState;
		std::thread m_cjobThread;
		bool runstate =  false;
		public :
		 CAJobThread() =  default;
		CAJobThread(CAThreadPoolManager*instance) ;
		
		~CAJobThread(){
			setRunState(false);
			if(m_cjobThread.joinable())
			m_cjobThread.join();
		}
		
		void setThreadState(THREADSTATE state){
			m_enuThreadState = state;
		}
		THREADSTATE getThreadState(){
			return m_enuThreadState;
		}
		void setRunState(bool rState){
			runstate = rState;
		}
		bool getRunstate(){
			return runstate;
		}
	};

	class CAThreadPoolManager : public CAThreadInterface  {
	private: //member variable
		unsigned int m_imaxThread = 0;
		unsigned int m_iminThread = 0;
		std::string m_strThreadPoolName;
		std::thread m_stThread;
		std::vector<CAJobThread*> m_tJobthread;
		CAQueue<CA::CAMessageBlock*>m_tprocessQueue;
		 
	private : // member function
		CAJobThread* ManageThreadPool();
		
		
		unsigned int getMessageQueueSize();
		unsigned int getPoolQueueSize();
		CAJobThread* findJobThread(THREADSTATE state);
		bool createJobThread();
		bool StartThreadPool(std::string threadPoolName, unsigned int maxthread, unsigned int minthread);
	public: // member variable
	bool m_run = false;
	ConditionalEventWait m_cjobEvent;
	public: // member function 
		CAThreadPoolManager() = delete;
		CAThreadPoolManager(std::string threadPoolName,unsigned int maxthread, unsigned int minthread);
		virtual ~CAThreadPoolManager();
		void setMaxThreadCount(unsigned int maxThread);
		void setMinThreadCount(unsigned int minThread);
		void setThreadPoolName(std::string threadName);
		unsigned int getConcurrentThreadCount();
		void shutdown();
		bool run() ;
		CAMessageBlock* RecvMessage();
		bool assignJob(bool asyncJob);
		virtual bool spwan(CAThreadPoolManager&);
	
		virtual bool jobRun(){
			std::cout << "Base Class \n";
			return true;
		};
		virtual bool PostMessage(CAMessageBlock* msgBlock);
		virtual bool PostMessage(std::unique_ptr<CAMessageBlock> upmsgBlock);
		 
	};

	static void JobTask(CAThreadPoolManager *inst, CAJobThread* jobinst){
		if(inst && jobinst){
			while(jobinst->getRunstate()){
				inst->m_cjobEvent.waitfor(1);
				jobinst->setThreadState(CA::THREADSTATE::ACTIVE);
				inst->jobRun();
				jobinst->setThreadState(CA::THREADSTATE::IDLE);

			}
		}
	}

	static void ThreadPoolBody(CAThreadPoolManager *inst){
		if(inst){
			
			inst->run();
			
		}
	}

	

	
};
#endif