#ifndef CA_UTILITES_HEADER
#define CA_UTILITES_HEADER
#pragma once
// Include c++ = 1z version system used function 
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
#include <bits/stdc++.h>
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
// Exceptiona or accerction
#include <assert.h>
#include <exception>
// STL Data Structures
#include <queue>
#include <map>
#include<unordered_map>
#include <list>
#include <vector>
#include <regex>
#include <sys/stat.h>

// For Ace Libraries
#include <ace/Acceptor.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/Reactor.h>
#include <ace/SOCK_Stream.h>
#include <ace/Task_T.h>
#include "ace/IOStream.h"
#include "ace/OS_NS_string.h"
#include "ace/OS_NS_unistd.h"
#include "ace/OS_main.h"
#include "ace/Task.h"
#include "ace/Service_Config.h"
#include <ace/SOCK_Connector.h>
#include "ace/IOStream.h"
#include "ace/Log_Msg.h"
#include "ace/OS_NS_stdlib.h"
#include "ace/Message_Block.h"
#include <ace/Singleton.h>
#include "ace/OS.h"
#include <ace/Guard_T.h>
#include <ace/Null_Mutex.h>
#include <ace/Recursive_Thread_Mutex.h>
#include <ace/Thread_Manager.h>
#include <ace/Event_Handler.h>
#include <ace/SOCK_SEQPACK_Connector.h>
#include <ace/SOCK_SEQPACK_Acceptor.h>
#include <ace/SOCK_SEQPACK_Association.h>
// for MSSQL ODBC 
#include <sqlext.h>
#include <sqltypes.h>
#include <sql.h>
// curl
#include <curl/curl.h>
//system
#include <sys/inotify.h>


#define STR std::string
#define UNQPTR std::unique_ptr
#define MOVE std::move
#define VECTOR std::vector
//typedef std::string STR;

namespace CAGSP{
    #define rand32() ((rand() & 0x3) << 30) | ((rand() & 0x7fff) << 15) | (rand() & 0x7fff)
    static long m_counter = 0;
    static std::mutex m_muteX;

    static std::string getstrTimeHMSms(){
      auto now = std::chrono::system_clock::now();
      auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
      auto fraction = now - seconds;
      auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(fraction);
      std::time_t cnow =  std::chrono::system_clock::to_time_t(now);
      char buf[7] = {0};
      std::strftime(buf, sizeof(buf),"%H%M%S", std::localtime(&cnow));
      return std::string(buf)+std::to_string(milliseconds.count());
    }
    static std::string createGuid(){
      char dst[32] = "\0";
      std::srand(time(NULL));
      #if 0
        auto now = std::chrono::system_clock::now();
        auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
        auto fraction = now - seconds;
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(fraction);
        std::time_t cnow = std::chrono::system_clock::to_time_t(now);
        char buf[7] = {0};
        std::strftime(buf, sizeof(buf),"%H%M%S", std::localtime(&cnow));
      #endif
      
      std::snprintf(dst, sizeof (dst), "%08x-%04x%08x",rand32(), rand32() & 0xffff, rand32());
      return std::string(std::string(dst)+ getstrTimeHMSms());
    }
    
    static long getCounter(){
      std::lock_guard<std::mutex> lock(m_muteX);
      return m_counter++;
    }
};

#endif
