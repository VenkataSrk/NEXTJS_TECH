#ifndef CA_HASH_H_
#define CA_HASH_H_

#include "CAUtilites.h"

template<typename T>
class CAHashMap
{
   private:
      std::unordered_map<STR, T> m_tHashMap;
      mutable std::mutex m_lckMutex;
   public:
      CAHashMap()=default;
      CAHashMap(const CAHashMap<T>&)=delete;
      CAHashMap& operator=(const CAHashMap<T>&)=delete;

      CAHashMap(CAHashMap&& moveMap)
      {
         std::lock_guard<std::mutex> lock(m_lckMutex);
		   m_tHashMap = std::move(moveMap.m_tHashMap);
	   }

      bool insertToMap(STR key, T tval)
      {
         std::lock_guard<std::mutex> lock(m_lckMutex);
         m_tHashMap[key] = tval;
         return true;
      }
      T findInMap(STR key)
      {
         std::lock_guard<std::mutex> lock(m_lckMutex);
         auto tMap = m_tHashMap.find(key);
         if(tMap!=m_tHashMap.end())
         {
            return tMap->second;
         }
         return nullptr;
      }
      bool deleteFromMap(STR key)
      {
         std::lock_guard<std::mutex> lock(m_lckMutex);
         auto tMap = m_tHashMap.find(key);
         if(tMap!=m_tHashMap.end())
         {
            m_tHashMap.erase(key);
            delete tMap->second;
            tMap->second = nullptr;
            return true;
         }
         return false;
      }
      void clear()
      {
         std::lock_guard<std::mutex> lock(m_lckMutex);
         for(auto itr=m_tHashMap.begin();itr!=m_tHashMap.end();++itr)
         {
            if(itr->second)
            {
               delete itr->second;
               itr->second = nullptr;
            }
         }
         m_tHashMap.clear();
      }
      unsigned int sizeOfMap()
      {
         return m_tHashMap.size();
      }

};
#endif