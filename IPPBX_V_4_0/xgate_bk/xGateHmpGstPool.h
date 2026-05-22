#ifndef _XGATE_HMP_GST_POOL_H
#define _XGATE_HMP_GST_POOL_H

//system include
#include <iostream>
#include <sstream>

//local include
#include "log.h"
//#include "klog.h"
#include "xGateHmpGstBin.h"

#define THISMODULE "HmpPool"

//Note: pool size should not be less than 2
#define MAX_HMP_POOL_SIZE 3000
#define MAX_HMP_G711_POOL_SIZE 1000
#define MAX_HMP_G729_POOL_SIZE 1000
#define MAX_HMP_UDP_POOL_SIZE 5000

template <class T>
class HmpGstPool {
  public:
    //CTOR
    HmpGstPool(unsigned int poolSize) :
      m_poolSize(poolSize), m_nextAvailElementIndex(0)
    {
      m_poolSize = (m_poolSize > 0) ? m_poolSize : MAX_HMP_POOL_SIZE;
      m_elementIndexList.reserve(m_poolSize);
      m_elementList.reserve(m_poolSize);
      CLog::Detail(THISMODULE, "HmpGstPool::HmpGstPool constructing pool with size: %d", m_poolSize);
    }

    xGateRetVal create_pipeline_pool(HmpGstPipelineType pipelineType)
    {
      string pipelineName("");
      HmpGstPipelineFactory *pPipelineFactory = new HmpGstPipelineFactory();
      switch(pipelineType) {
        case EN_PIPELINE_SINGLE_CALL:
        {
          CLog::Detail(THISMODULE, "HmpGstPool::create_pipeline_pool for PIPELINE_SINGLE_CALL type!");
          pipelineName = "Single_Call_Pipeline";
          break;
        }
        case EN_PIPELINE_CONF_CALL:
        {
          pipelineName = "Conf_Call_Pipeline";
          CLog::Detail(THISMODULE, "HmpGstPool::create_pipeline_pool for PIPELINE_CONF_CALL type!");
          break;
        }
        case EN_PIPELINE_ADD_CALL:
        {
          pipelineName = "Add_Call_Pipeline";
          CLog::Detail(THISMODULE, "HmpGstPool::create_pipeline_pool for PIPELINE_ADD_CALL type!");
          break;
        }
        case EN_PIPELINE_UNKNOWN:
        default:
        {
          CLog::Error(THISMODULE, "HmpGstPool::create_pipeline_pool failed for UNKNOWN pipeline type!");
          EN_XGATE_STATUS_ERROR;
        }
      }

      std::stringstream sstm;
      char buff[30];
      //Note: for pipeline we need m_poolSize/2 size
      //m_poolSize = m_poolSize/2; //TODO: Yoga, for testing purpose only
      for (int i = 0; i < m_poolSize; i++ )
      {
        m_elementIndexList.push_back((unsigned short)true);
        memset(buff,'\0',30);
        snprintf(buff,30,"%s_%d", pipelineName.c_str(), i);
        string tmpStr(buff);
        T *t = pPipelineFactory->pipeline_create(tmpStr, pipelineType); 
        m_elementList.push_back(t);
      }
      /*m_elementIndexList[0] = false;
      m_nextAvailElementIndex = 1;*/
      m_nextAvailElementIndex = 0;

      return EN_XGATE_STATUS_SUCCESS; 
    }


#if 0
    xGateRetVal create_udpsrc_pool() {
      for (int i = 0; i < m_poolSize; i++ )
      {
        m_elementIndexList.push_back((unsigned short)true);
        T *t = new xGateHmpGstUdpSrc();
        if(t->create_udp_src())
        {
          m_elementList.push_back(t);
        }
      }
      m_nextAvailElementIndex = 0;
      return EN_XGATE_STATUS_SUCCESS; 
    }
  
    xGateRetVal create_udpsink_pool() {
      for (int i = 0; i < m_poolSize; i++ )
      {
        m_elementIndexList.push_back((unsigned short)true);
        T *t = new xGateHmpGstUdpSink();
        if(t->create_udp_sink())
        {
          m_elementList.push_back(t);
        }
      }
      m_nextAvailElementIndex = 0;
      return EN_XGATE_STATUS_SUCCESS; 
    }
#endif

    xGateRetVal create_bin_pool(HmpGstBinType binType)
    {
      string binName("");
      HmpGstBinFactory *pBinFactory = new HmpGstBinFactory();
      switch(binType) {
#if 0
        case EN_BIN_G711_SEND:
        {
          CLog::Detail(THISMODULE, "HmpGstPool::create_bin_pool for G711_SEND bin type!");
          binName = "G711_Send_Bin";
          break;
        }
        case EN_BIN_G711_RECV:
        {
          CLog::Detail(THISMODULE, "HmpGstPool::create_bin_pool for G711_RECV bin type!");
          binName = "G711_Recv_Bin";
          break;
        }
        case EN_BIN_G729_SEND:
        {
          CLog::Detail(THISMODULE, "HmpGstPool::create_bin_pool for G729_SEND bin type!");
          binName = "G729_Send_Bin";
          break;
        }
        case EN_BIN_G729_RECV:
        {
          CLog::Detail(THISMODULE, "HmpGstPool::create_bin_pool for G729_RECV bin type!");
          binName = "G729_Recv_Bin";
          break;
        }
#endif
        case EN_BIN_PLAY_FILE:
        {
          CLog::Detail(THISMODULE, "HmpGstPool::create_bin_pool for PLAY_FILE bin type!");
          binName = "Play_Back_Bin";
          break;
        }
        case EN_BIN_UNKNOWN:
        default:
        {
          CLog::Error(THISMODULE, "HmpGstPool::create_bin_pool failed for UNKNOWN bin type!");
          return EN_XGATE_STATUS_ERROR; 
        }
      }
      
      char buff[30];
      for (int i = 0; i < m_poolSize; i++ )
      {
        m_elementIndexList.push_back((unsigned short)true);
        memset(buff,'\0',30);
        snprintf(buff,30,"%s_%d", binName.c_str(), i);
        string tmpStr(buff);
        //T *t = pBinFactory->bin_create(tmpStr, binType);
        T *t = pBinFactory->bin_playfile_create(tmpStr, binType);
        m_elementList.push_back(t);
      }
      /*m_elementIndexList[0] = false;
      m_nextAvailElementIndex = 1;*/
      m_nextAvailElementIndex = 0;
      return EN_XGATE_STATUS_SUCCESS; 
    }

    //DTOR
    ~HmpGstPool()
    {
      
    }

    //get available element from pool
    T * acquire()
    {
      T *t = NULL;
      unsigned int count = 0;
      unsigned int i = 0;

      for (i = m_nextAvailElementIndex; i < m_poolSize; i++)
      {
        if(m_elementIndexList[i] == true)
        {
          m_nextAvailElementIndex = (i + 1) % m_poolSize;
          m_elementIndexList[i] = false;
          CLog::Detail(THISMODULE, "HmpGstPool::acquire successful for index: %d", i);
          t = m_elementList[i];  
          t->m_poolIndex = i;
          return t;
        } else {
          if(i+1 == m_poolSize)
          {
            i = 0;
          }
          if(++count == m_poolSize)
          {
            CLog::Error(THISMODULE, "HmpGstPool::acquire no free element available in pool, increase pool size");
            return NULL;
          }
        }
      }
      CLog::Error(THISMODULE, "HmpGstPool::acquire no free element available in pool, increase pool size");
      return NULL;
    }

    //release element back to pool
    void release(T *t)
    {
      unsigned short i = t->m_poolIndex;
      CLog::Detail(THISMODULE, "HmpGstPool::release successful for index: %d", i);
      m_elementIndexList[i] = true;
      t->m_poolIndex = 0;
    }

    //TODO: extend size of pool at runtime need to be implemented
    void extend_pool()
    {
    }

  private:
    unsigned int m_poolSize;
    unsigned int m_nextAvailElementIndex;
    vector<unsigned short> m_elementIndexList;
    vector<T*> m_elementList;
};

#endif
