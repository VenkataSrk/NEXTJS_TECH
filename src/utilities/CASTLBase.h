#ifndef CA_UTILITY_STL
#define CA_UTILITY_STL
#pragma once
//#include "CAUtilites.h"
#include <queue>
#include <map>
#include<unordered_map>
#include <list>
#include <vector>
#include <regex>
#include <sys/stat.h>
#include <cstdlib>
#include <csignal>
#include <cstring>
#include <ctime>
#include <iostream>
#include <atomic>
#include <fstream>  // for file operation
#include <sstream>
#include <memory>
#include <filesystem>
#include <iomanip>
// For Socket program to include below input
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <ifaddrs.h>

// Asyn member handling class
#include <condition_variable>
#include <thread>
#include <functional>
#include <mutex>
#include <chrono>
#include <future>

//try
#include <string>

namespace CA{
enum class THREADSTATE {IDLE=0,ACTIVE,SLEEP,STOP,UNKNOWN };   
class CAMessageBlock{
private:
unsigned long m_ulMsgType;
unsigned long m_ulEvent;
std::string m_defaultMsg;


public:
CAMessageBlock()= default;
virtual ~CAMessageBlock() = default;
virtual bool setMsgType(unsigned long msgType);
unsigned long getMsgType();
virtual bool setEvent(unsigned long msgEvent);
unsigned long getEvent();
virtual bool setDefaultMessage(std::string str);
virtual std::string getDefaultMessage();
};

template<typename T>
class CAQueue{
private:
std::queue<T> m_tQueue;
mutable std::mutex m_lckMutex;
public:
CAQueue() = default;
CAQueue(const CAQueue<T> &) = delete;
CAQueue& operator=(const CAQueue<T> &) = delete;

CAQueue(CAQueue&& moveQueue){
	std::lock_guard<std::mutex> lock(m_lckMutex); 
 	m_tQueue = std::move(moveQueue.m_tQueue); 
}
virtual ~CAQueue(){

}
 bool isEmpty () {return m_tQueue.empty();} 
 unsigned long size()  {return m_tQueue.size() ;}
 bool enQ( T value){
  std::lock_guard<std::mutex> lock(m_lckMutex);
  m_tQueue.push(std::move(value));
  return true;	
}

T  deQ(){
	std::lock_guard<std::mutex> lock(m_lckMutex);
	if(m_tQueue.empty()) {return nullptr;}
   	T retValue = std::move(m_tQueue.front());
  	m_tQueue.pop();

  return retValue;
 }
 void clear(){

	if(m_tQueue.empty())
	 	return;
	
		std::lock_guard<std::mutex> lock(m_lckMutex);
		std::queue<T> tempQ ;
		std::swap(m_tQueue,tempQ);
	
 }
};
template<typename T>
class CAPriorityQueue{
private:
std::priority_queue<T>m_tPriorityQueue;
mutable std::mutex m_lckMutex;
public:
CAPriorityQueue() = default;
CAPriorityQueue(const CAPriorityQueue<T> &) = delete;
CAPriorityQueue& operator=(const CAPriorityQueue<T> &) = delete;
CAPriorityQueue(CAPriorityQueue&& moveCAPriorityQueue){
std::lock_guard<std::mutex> lock(m_lckMutex);
m_tPriorityQueue = std::move(moveCAPriorityQueue.m_tPriorityQueue);
}
virtual ~CAPriorityQueue(){

}
 bool isEmpty () {return m_tPriorityQueue.empty();}
 unsigned long size()  {return m_tPriorityQueue.size() ;}
 bool enQ( T value){
  std::lock_guard<std::mutex> lock(m_lckMutex);
  m_tPriorityQueue.push(std::move(value));
  return true;
}

T  deQ(){
        std::lock_guard<std::mutex> lock(m_lckMutex);
        if(m_tPriorityQueue.empty()) {return nullptr;}
        T retValue = std::move(m_tPriorityQueue.front());
        m_tPriorityQueue.pop();

  return retValue;
 }
 void clear(){

        if(m_tPriorityQueue.empty())
                return;
		}
};

class ConditionalEventWait {
public:
	ConditionalEventWait() = default;
	ConditionalEventWait(ConditionalEventWait & obj) =  default;
	std::condition_variable m_evWait;
	std::mutex m_evLock; 
	std::string m_strThreadName;


	void waitOnLock() {
		std::unique_lock<std::mutex> ulLock(m_evLock);
		m_evWait.wait(ulLock);
	}
	void SignalAll() {
		m_evWait.notify_all();
	}
	void Signalone(){
		m_evWait.notify_one();
	}
	void waitfor( int idx){
		std::unique_lock<std::mutex> ulLock(m_evLock);
		m_evWait.wait_for(ulLock, std::chrono::milliseconds(idx*1000));
	}
};
template<typename T>
class CAMap {
private :
	std::map<std::string, T> m_tMap;
        mutable std::mutex m_lckMutex;
public :
using ca_itr = typename std::map<std::string, T>::iterator ;
public:
	CAMap() = default;
	CAMap(const CAMap<T> &) = delete;
        //CAMap(const CAMap<T> &) = default;
	CAMap& operator=(const CAMap<T>&) = delete;
	CAMap(CAMap&& moveMap) {
                std::lock_guard<std::mutex> lock(m_lckMutex);
		m_tMap = std::move(moveMap.m_tMap);
	}
	bool InsertToMap(std::string key, T value) {
               std::lock_guard<std::mutex> lock(m_lckMutex);
		m_tMap[key] = value;
		return true;
	}
	T FindValue(std::string key) {
		std::lock_guard<std::mutex> lock(m_lckMutex);
		auto findMap = m_tMap.find(key);
		if(findMap != m_tMap.end())
			return std::move(findMap->second);
		return nullptr;
	}
	bool DeleteFromMap(std::string key) {
                std::lock_guard<std::mutex> lock(m_lckMutex);
		auto findMap = m_tMap.find(key);
		if (findMap != m_tMap.end())
			m_tMap.erase(key);
		return true;
	}
	unsigned int GetMapSize(){
		return m_tMap.size();
	}
	bool Clear(){
		m_tMap.clear();
		return true;
	}
        
        ca_itr Begin(){
                return m_tMap.begin();
        }
        ca_itr End(){
                return m_tMap.end();
        }
         void Copy(CAMap<T> & tMap){
                m_tMap =  tMap.m_tMap;
         }

};
template<typename T>
class CAUnOrderedMap {
private :
        std::unordered_map<std::string, T> m_tUnOrderedMap;
         mutable std::mutex m_lckMutex;
public:
        CAUnOrderedMap() = default;
        CAUnOrderedMap(const CAUnOrderedMap<T> &) = delete;
        CAUnOrderedMap& operator=(const CAUnOrderedMap<T>&) = delete;
        CAUnOrderedMap(CAUnOrderedMap&& moveMap) {
                std::lock_guard<std::mutex> lock(m_lckMutex);
                m_tUnOrderedMap = std::move(moveMap.m_tMap);
        }
        bool InsertToMap(std::string key, T value) {
                std::lock_guard<std::mutex> lock(m_lckMutex);
                m_tUnOrderedMap[key] = value;
                return true;
        }
        T FindValue(std::string key) {
                std::lock_guard<std::mutex> lock(m_lckMutex);
                auto findMap = m_tUnOrderedMap.find(key);
                if(findMap != m_tUnOrderedMap.end())
                        return std::move(findMap->second);
                return nullptr;
        }
         bool DeleteFromMap(std::string key) {
                std::lock_guard<std::mutex> lock(m_lckMutex);
                auto findMap = m_tUnOrderedMap.find(key);
                if (findMap != m_tUnOrderedMap.end())
                        m_tUnOrderedMap.erase(key);
                return true;
        }
        unsigned int GetMapSize(){
                return m_tUnOrderedMap.size();
        }
        bool Clear(){
                m_tUnOrderedMap.clear();
                return true;
        }
};
class CAThreadInterface{
	public:
	CAThreadInterface() =  default;

	virtual bool PostMessage(CAMessageBlock* msgBlock) = 0;
        virtual bool PostMessage(std::unique_ptr<CAMessageBlock> upmsgBlock) = 0;
};
//global thread container as singleton 
class CAObject{
	private:
	static CAObject* m_pinstance  ;
	
	public :
	static CAObject& getInstance();
	CAMap<CAThreadInterface*> activeThreadList;	 
	bool postmessage(const char* threadName, CAMessageBlock* msgBlock);
};

template<typename T>
class CAList{
private:
std::list<T> m_tList;
mutable std::mutex m_lckMutex;

public:
CAList() = default;
CAList(const CAList<T> &) = default;
CAList& operator=(const CAList<T> &) = default;

CAList(CAList&& moveList)
{
std::lock_guard<std::mutex> lock(m_lckMutex);
m_tList = std::move(moveList.m_tList);
}
virtual ~CAList()
{}

bool isEmpty () {return m_tList.empty();} 
unsigned long size()  {return m_tList.size() ;}
T popFront()
{
  std::lock_guard<std::mutex> lock(m_lckMutex);
  if(m_tList.empty())
  {
    return nullptr;
  }
  return m_tList.pop_front();
}
T popBack()
{
  std::lock_guard<std::mutex> lock(m_lckMutex);
  if(m_tList.empty())
  {
    return nullptr;
  }
  return m_tList.pop_back();
}
bool pushFront(T value)
{
  std::lock_guard<std::mutex> lock(m_lckMutex);
  m_tList.push_front(value);
  return true;
}
bool pushBack(T value)
{
  std::lock_guard<std::mutex> lock(m_lckMutex);
  m_tList.push_back(value);
  return true;
}
bool clear()
{
  std::lock_guard<std::mutex> lock(m_lckMutex);
  if(m_tList.empty())
  {
    return false;
  }
  m_tList.clear();
  return true;
}
};
template<typename T>
class CAVector{
           private:
             std::vector<T>m_vector;
             mutable std::mutex m_lckMutex;
	   public:
	     CAVector()= default;
	     CAVector& operator=(const CAVector&) = delete;  
             CAVector(const CAVector&) = delete;
             CAVector(CAVector&& moveVector)
             {
             std::lock_guard<std::mutex> lock(m_lckMutex);
             m_vector = std::move(moveVector.m_vector);
             }
             virtual ~CAVector(){}
              bool PushBack(T value ){
              std::lock_guard<std::mutex> lock(m_lckMutex);
              m_vector.push_back(value);
                        return true;
            }

	     bool PopBack()
            {
            std::lock_guard<std::mutex> lock(m_lckMutex);
            if(m_vector.empty())
            {
            return false;
            }
           m_vector.pop_back();
           return true;
            }
	
       	    T Front(){
                   return  m_vector.front();
                   //return true;
            }
            T Back(){
                   return m_vector.back();
                   // return true;
            }
            T GetValueByPosition(int pos){
                   return m_vector.at(pos);
//                    return true;
            }
            bool InsertAt(T value, int pos){
                try{
                auto it = m_vector.begin() + pos;
                m_vector.insert(it,value);
                }catch(...){
                        return false;
                }
                return true;
            }

            bool UpdateAt(T Value, int pos){
                try{
                        if(pos > -1)
                                m_vector.at(pos) = Value;

                }catch(...){
                        return false;
                }
                return true;
            }
        
            bool RemoveByPosition(int pos){
                   if(m_vector.empty()){
                       return false;}
                   m_vector.erase(m_vector.begin()+pos);
                   return true;
            }

	    bool Empty(){
		    return m_vector.empty();
			    
		}
        unsigned int GetVectorSize(){
                return m_vector.size();
        }
        
        void Display(){
                for(int i=0;i<m_vector.size();i++){
                        std::cout<<GetValueByPosition(i)<<std::endl;
                }
        }
           bool RemoveAll()
            {
                std::lock_guard<std::mutex> lock(m_lckMutex);
                if(m_vector.empty())
                {
                return false;
                }
                m_vector.clear();
                return true;
            }
            bool copyAll(CAVector& copyVect){
                if(copyVect.GetVectorSize() > 0){
                        m_vector.insert(m_vector.begin(),copyVect.m_vector.begin(), copyVect.m_vector.end());
                        return true;
                }
                return false;
            }


};
}

#endif
