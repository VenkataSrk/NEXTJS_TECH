/*
* Call Agent utility Library for threads
*/
#include "CAThread.h"

/*
*/

/*
** Method Name : start()
** Description : starts a new thread and executes the self loop run function 
                 use this function for starting Event driven Thread
                 
** Parameter   : void
** return value: bool data type , on successful thread creation returns true esle false
                 on error exception will be captured on last known error variable.
*/
bool CA::CAThread::start(CA::CAThread& inst){
//std::thread stThread([=] { this->run();});
//m_stThread = std::move(stThread);

m_stThread = std::move(std::thread(ThreadBody, &inst));
	return true;
}
/*
** Method Name : join
** Description :  block the current execution on active thread 
** Parameter   : void
** return value: bool type if thread is active returns ture after join else return false
*/
bool CA::CAThread::join(){
    
    if(m_stThread.joinable()){
	    m_stThread.join();
	    return true;
    }else{
        return false;
    }
}
/*
** Method Name : pause
** Description : make the thread sleep for specified seconds if thread is active
                 block the current execution of the thread
                 the function is non-async call.
** Parameter   : int data type for seconds input.
** return value: bool data type on success return true else false.
*/
bool CA::CAThread::pause(int iseconds){
    if(m_iThreadState == CA::THREADSTATE::ACTIVE){
        m_iThreadState = CA::THREADSTATE::SLEEP;
        
	    return true;
    }else{
        return false;
    }
}
/*
** Method Name :
** Description :
** Parameter   :
** return Param:
*/
bool CA::CAThread::resume() {

	return true;
}
/*
** Method Name :
** Description :
** Parameter   :
** return Param:
*/
bool CA::CAThread::stop() {
	return true;
}
/*
** Method Name :
** Description :
** Parameter   :
** return Param:
*/
bool CA::CAThread::detach() {
	if(m_stThread.joinable())
	{
		m_stThread.detach();
	}
	return true;
}
/*
** Method Name :
** Description :
** Parameter   :
** return Param:
*/
bool CA::CAThread::setThreadState(CA::THREADSTATE ithreadState) {
	switch (ithreadState) {
	case CA::THREADSTATE::IDLE:
		m_iThreadState = CA::THREADSTATE::IDLE;
		break;
	case CA::THREADSTATE::ACTIVE:
		m_iThreadState = CA::THREADSTATE::ACTIVE;
		break;
	case CA::THREADSTATE::SLEEP:
		m_iThreadState = CA::THREADSTATE::SLEEP;
		break;
	default:
		m_iThreadState = CA::THREADSTATE::UNKNOWN;
		break;
	}
	return true;
}
/*
** Method Name :
** Description :
** Parameter   :
** return Param:
*/
bool CA::CAThread::spwan(CA::CAThread& inst) {
	if(getThreadName() == nullptr)
	return false;

	CA::CAObject::getInstance().activeThreadList.InsertToMap(getThreadName(),this);

	setThreadState(CA::THREADSTATE::IDLE);
	if (start(inst) == true) {
		return true;
	}
	else
		return false;
}
/*
** Method Name :
** Description :
** Parameter   :
** return Param:
*/
bool CA::CAThread::run() {


	return true;

}
/*
** Method Name :
** Description :
** Parameter   :
** return Param:
*/
std::thread::id CA::CAThread::getId() {
	if (m_stThread.joinable())
		return m_stThread.get_id();
	else
		return (std::thread::id)0; // need to have common error code return
}
/*
** Method Name :
** Description :
** Parameter   :
** return Param:
*/
bool CA::CAThread::PostMessage(CA::CAMessageBlock* msgBlock) {
	try {
		//std::unique_ptr<CA::CAMessageBlock> temp(std::move(msgBlock));
		m_cThreadQueue.enQ(msgBlock);

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
bool CA::CAThread::PostMessage(std::unique_ptr<CA::CAMessageBlock> ptrMsgBlock){
	try {
		CA::CAMessageBlock* ptrTemp = ptrMsgBlock.release();
		m_cThreadQueue.enQ(ptrTemp);
		return true;
	}
	catch (...) {
		return false;
	}
}
/*
** Method Name :
** Description :
** Parameter   :
** return Param:
*/
CA::CAMessageBlock* CA::CAThread::RecvMessage() {
	//std::unique_ptr<CA::CAMessageBlock> retValue(nullptr);
	CA::CAMessageBlock* retValue = nullptr;
	try {
		
		
		if (m_cThreadQueue.size() > 0)
			retValue = m_cThreadQueue.deQ();
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
CA::THREADSTATE CA::CAThread::getThreadState() {
	return m_iThreadState;
}
/*
** Method Name :
** Description :
** Parameter   :
** return Param:
*/
void CA::CAThread::clearQ(){
	m_cThreadQueue.clear();
}

