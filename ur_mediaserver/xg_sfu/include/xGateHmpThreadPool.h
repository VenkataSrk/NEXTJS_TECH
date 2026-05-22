#ifndef _XGATE_HMP_THREAD_POOL_
#define _XGATE_HMP_THREAD_POOL_

extern "C"
{
  #include <stdlib.h>
  #include <stdio.h>
}
#include <iostream>
#define MAX_EVENT_HANDLING_THREADS 5
#define MAX_AUDIO_DEC_THREADS 5
#define MAX_AUDIO_ENC_THREADS 5
#define MAX_RX_DATA_RCV_THREADS 5

typedef enum _XGATE_HMP_THREAD_POOL_TYPE{
  THREAD_POOL_UNKNOWN = 0,
  THREAD_POOL_EVENT ,
  THREAD_POOL_RX_DATA,
  THREAD_POOL_AUDIO_ENC,
  THREAD_POOL_AUDIO_DEC,
}XGATE_HMP_THREAD_POOL_TYPE;

class ThreadPool {
  
public:
  GThreadPool *m_event_pool;
  GThreadPool *m_decoder_pool;
  GThreadPool *m_encoder_pool;
  GThreadPool *m_rx_data_pool;

  xGateRetVal create_thread_pool (void *ptr, XGATE_HMP_THREAD_POOL_TYPE ePoolType, bool bExclusive);
  static void event_thread_pool_handler (gpointer data, gpointer gpData);
  static void audio_dec_thread_pool_handler(gpointer data, gpointer gpData);
  static void audio_enc_thread_pool_handler(gpointer data, gpointer gpData);
  static void rcv_data_thread_pool_handler(gpointer data, gpointer gpData);

};
#endif //_XGATE_HMP_THREAD_POOL_
