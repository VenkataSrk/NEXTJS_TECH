#include "CAThreadPoolManager.h"
/*
** Method Name :
** Description :
** Parameter   :
** return Param:
*/
CA::CAThreadPoolManager::CAThreadPoolManager(std::string threadName, unsigned int maxthread, unsigned int minthread) :
	m_imaxThread(maxthread),
	m_iminThread(minthread) {
	m_strThreadPoolName = threadName;
	
	}
/*
** Method Name :
** Description :
** Parameter   :
** return Param:
*/
CA::CAThreadPoolManager::~CAThreadPoolManager() {
	m_run = false;
	if(m_stThread.joinable())
		m_stThread.join();

}
/*
** Method Name :
** Description :
** Parameter   :
** return Param:
*/
unsigned int CA::CAThreadPoolManager::getConcurrentThreadCount() {
	return 0;
}
/*
** Method Name :
** Description :
** Parameter   :
** return Param:
*/
void CA::CAThreadPoolManager::setMaxThreadCount(unsigned int maxThread) {
	m_imaxThread = maxThread;
}
/*
** Method Name :
** Description :
** Parameter   :
** return Param:
*/
void CA::CAThreadPoolManager::setMinThreadCount(unsigned int minThread) {
	m_iminThread = minThread;
}
/*
** Method Name :
** Description :
** Parameter   :
** return Param:
*/
void CA::CAThreadPoolManager::setThreadPoolName(std::string threadName) {
	m_strThreadPoolName.assign(threadName);
	
}
/*
** Method Name :
** Description :
** Parameter   :
** return Param:
*/
void CA::CAThreadPoolManager::shutdown() {

}
/*
** Method Name :
** Description :
** Parameter   :
** return Param:
*/
bool CA::CAThreadPoolManager::StartThreadPool(std::string threadName, unsigned int maxthread, unsigned int minthread) {
	// Create Pool queue with minimal thread in wating state.
	m_run = true;
	// Create minimal job thread 
	for (unsigned int ncount = 0 ; ncount < minthread ; ncount++)
		createJobThread();

	 

	return true;
}
/*
** Method Name :
** Description :
** Parameter   :
** return Param:
*/
bool CA::CAThreadPoolManager::run() {

while(m_run){
	
	if(getMessageQueueSize() >  getPoolQueueSize() && getPoolQueueSize() < m_imaxThread)
		createJobThread();
	
	if((getPoolQueueSize() == m_imaxThread || getMessageQueueSize() < m_imaxThread) && 
		getPoolQueueSize() > m_iminThread){
		CA::CAJobThread* jobthread = findJobThread(CA::THREADSTATE::IDLE);
		if(jobthread)
			jobthread->setRunState(false);
	}
	//std::cout << "Get Message Q Size " <<  getMessageQueueSize() << "\n";
 	
	
}
// kill all the thread in the list
return true;
}
/*
** Method Name :
** Description :
** Parameter   :
** return Param:
*/
bool CA::CAThreadPoolManager::assignJob(bool asyncJob) {
	return true;
}
/*
** Method Name :
** Description :
** Parameter   :
** return Param:
*/
CA::CAJobThread* CA::CAThreadPoolManager::ManageThreadPool(){
	// get pool queue size
	
	if(getPoolQueueSize() == m_imaxThread){
		// get Idle state thread and assign the job to run queue
		
		
	}else if(getPoolQueueSize() == m_iminThread ){
		// get IDLE sate thread and assign the job to run queue
		// on no IDLE thread create new job thread and assign to run queue
		

	}else if(getPoolQueueSize() > m_iminThread &&  getPoolQueueSize() < m_imaxThread){
		// find IDLE state thread and assign the job to run queue
		// on no IDLE thread create new job thread and assign to run queue
		

	}else{
		// put to wating queue
	}


	return nullptr;
}
/*
** Method Name :
** Description :
** Parameter   :
** return Param:
*/
unsigned int CA::CAThreadPoolManager::getMessageQueueSize(){
return 0;
}
/*
** Method Name :
** Description :
** Parameter   :
** return Param:
*/
unsigned int CA::CAThreadPoolManager::getPoolQueueSize(){
return m_tJobthread.size();
}
/*
** Method Name :
** Description :
** Parameter   :
** return Param:
*/
bool CA::CAThreadPoolManager::createJobThread(){
	
	CA::CAJobThread* jobthread =  new CA::CAJobThread(this);
	m_tJobthread.push_back(jobthread);
	
	return  true;

	
}

/*
** Method Name :
** Description :
** Parameter   :
** return Param:
*/
CA::CAJobThread* CA::CAThreadPoolManager::findJobThread(THREADSTATE state){

	CA::CAJobThread* temp =  nullptr;
	for(auto it = m_tJobthread.begin() ; it != m_tJobthread.end(); ++it){
		if((*it)->getThreadState() == state)
			temp = std::move(*it);

	}
	return std::move(temp);
}
/*
** Method Name :
** Description :
** Parameter   :
** return Param:
*/
bool CA::CAThreadPoolManager::PostMessage(CA::CAMessageBlock* msgBlock) {
	try {
		//std::unique_ptr<CA::CAMessageBlock> temp(std::move(msgBlock));
		m_tprocessQueue.enQ(msgBlock);

	}
	catch (...) {
		return false;
	}
	return true;
}
/*
** Method Name :
** Description :
** Parameter   :
** return Param:
*/
bool CA::CAThreadPoolManager::PostMessage(std::unique_ptr<CA::CAMessageBlock> ptrMsgBlock){

	CA::CAMessageBlock* ptrTemp = ptrMsgBlock.release();
	m_tprocessQueue.enQ(ptrTemp);
	return true;

}
/*
** Method Name :
** Description :
** Parameter   :
** return Param:
*/
CA::CAMessageBlock* CA::CAThreadPoolManager::RecvMessage() {
	//std::unique_ptr<CA::CAMessageBlock> retValue(nullptr);
	CA::CAMessageBlock* retValue = nullptr;
	try {
		
		
		if (m_tprocessQueue.size() > 0)
			retValue = m_tprocessQueue.deQ();
			//retValue(std::move(m_cThreadQueue.deQ()));
		
		
	}
	catch (...) {
		return retValue;
		}
	return retValue;
}
/*
** Method Name :
** Description :
** Parameter   :
** return Param:
*/
bool CA::CAThreadPoolManager::spwan(CA::CAThreadPoolManager& threadpool){
	CA::CAObject::getInstance().activeThreadList.InsertToMap(m_strThreadPoolName.c_str(), &threadpool);
	StartThreadPool(m_strThreadPoolName,m_imaxThread,m_iminThread);
	std::cout << "Get Pool Size " <<  getPoolQueueSize() << "\n";
	m_stThread = std::move(std::thread(ThreadPoolBody,&threadpool));
	return true;
}

/*-----------------------------------------------------------------------------------------------------------------------------------
CAJobThread 
------------------------------------------------------------------------------------------------------------------------------------*/

CA::CAJobThread::CAJobThread(CA::CAThreadPoolManager* instance){
	if(instance){
		setRunState(true);
		m_cjobThread =  std::move(std::thread(JobTask,instance,this));
		//JobTask(instance,this);
	}
}