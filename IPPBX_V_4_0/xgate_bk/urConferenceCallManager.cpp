/* ***********************************************************
 * $CPP: /xgate/urConferenceCallManager.cpp
 *
 * VECTONE MOBILE
 *
 * Conference controller functions implemetation 
 *
 * version 1
 *
 * @author Yadav Kumar 
 * 
 *
 **************************************************************/
#include "urConferenceCallManager.h"
// NULL the singleton object
URConferenceCallManager* URConferenceCallManager::urConfInstance = NULL;

URConferenceCallManager::URConferenceCallManager(){
}
URConferenceCallManager::~URConferenceCallManager(){
}
/***************************************************************
*Name :
*Description:
*Input:
*Return:
*parameter info:
***************************************************************/

/***************************************************************
*Name : getInstance
*Description: To get the current object instance 
*Input: NA
*Return : objects pointer
*parameter info: void
***************************************************************/
URConferenceCallManager* URConferenceCallManager::getInstance(){
	if(urConfInstance == NULL){
		urConfInstance =  new URConferenceCallManager();
	}
	return urConfInstance;
}

/***************************************************************
*Name :terminateInstance
*Description: to kill the object call this static method
*Input: null
*Return: void
*parameter info: void 
***************************************************************/
void URConferenceCallManager::terminateInstance(){
	if(urConfInstance){
		delete urConfInstance;
		urConfInstance = NULL;
	}
}
/***************************************************************
*Name :addtoConferenceRoom
*Description: new participant call info will inserted to map container
*Input: conferenceRoom struct with info and map key string
*Return: boolean if insertion success return true else false
*parameter info: pointer to confernceRoom structure and string value for key
***************************************************************/
bool URConferenceCallManager::addtoConferenceRoom(std::string confKey,conferenceRoom* confInfo){
	if(!confInfo)
		return false;
	if(confKey.empty())
		return false;
	std::pair<std::map<std::string,conferenceRoom*>::iterator,bool> ret;
	ret = urConferenceRooms.insert(std::pain<>(confKey,confInfo);
	return ret->second;
}
