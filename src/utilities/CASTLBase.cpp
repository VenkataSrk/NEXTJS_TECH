/*
* Call Agent utility Library for threads
*/
#include "CASTLBase.h"

/*-----------------------------------------------------------------------------------------------------------------------------------
***************************************************CAMessageBlock********************************************************************
------------------------------------------------------------------------------------------------------------------------------------*/
/*
** Method Name :
** Description :
** Parameter   :
** return Param:
*/
bool CA::CAMessageBlock::setMsgType(unsigned long msgType){
 m_ulMsgType = msgType;
return true;
}
/*
** Method Name :
** Description :
** Parameter   :
** return Param:
*/
unsigned long CA::CAMessageBlock::getMsgType(){
 return m_ulMsgType;
}
/*
** Method Name :
** Description :
** Parameter   :
** return Param:
*/
bool CA::CAMessageBlock::setEvent(unsigned long msgEvent){
 m_ulEvent = msgEvent;
return true;
}
/*
** Method Name :
** Description :
** Parameter   :
** return Param:
*/
unsigned long CA::CAMessageBlock::getEvent(){
 return m_ulEvent;
}

bool CA::CAMessageBlock::setDefaultMessage(std::string str){
	m_defaultMsg =  str;
	return true;
}

std::string CA::CAMessageBlock::getDefaultMessage(){
	return m_defaultMsg;
}
/*-----------------------------------------------------------------------------------------------------------------------------------
***************************************************CAObject***************************************************************************
------------------------------------------------------------------------------------------------------------------------------------*/

 CA::CAObject* CA::CAObject::m_pinstance =  nullptr;
/*
** Method Name :
** Description :
** Parameter   :
** return Param:
*/
CA::CAObject& CA::CAObject::getInstance(){
    if(!m_pinstance)
	 	{
             std::mutex _mutex;
             std::lock_guard<std::mutex> lock(_mutex);
             if(!m_pinstance){
                 m_pinstance = new CA::CAObject;
             }
         }
	return *m_pinstance;
}
/*
** Method Name :
** Description :
** Parameter   :
** return Param:
*/
bool CA::CAObject::postmessage(const char* threadName, CAMessageBlock* msgBlock){	
		if(threadName == nullptr)
			return false;
		std::cout << threadName << "\n";
		CAThreadInterface* msgthread = activeThreadList.FindValue(threadName);
		if(msgthread)
		msgthread->PostMessage(msgBlock);

		return true;

	}


