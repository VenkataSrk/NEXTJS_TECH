
//local include

//ace include
#include <ace/Task.h>
#include <ace/Message_Queue.h>

//self include
#include "xGateHmpGstBin.h"
#include "xglog.h"
#include "xGateSFUtil.h"
#include "SFUClient.h"
#include "xGateSFUtil.h"
#define THISMODULE "HmpBin"
#define CHANNEL_NAME_SIZE 20
extern SFUClient sfu_client;
/* Client class implementation - Start
*/
Client::Client() :
  m_FrameChannel(NULL), m_isConference(FALSE),
  m_webrtcType(EN_WEBRTC_TYPE_SENDRECV),m_RateControl(NULL),
  m_bitrate_is_initialized(false),
  m_inhouseClient(NULL),m_pHmpMRCPVoiceCaptureChannel(NULL),
  m_asrType(EN_XGATE_ASR_TYPE_UNKNOWN)
{
  m_asrType = EN_XGATE_ASR_TYPE_UNKNOWN;
  m_audioPayload = 0;

#ifdef RECORD_ENABLED
  m_rcvd_audio_queue = g_async_queue_new_full((GDestroyNotify)free_rcvd_audio_queue);
  m_rcvd_video_queue = g_async_queue_new_full((GDestroyNotify)free_rcvd_video_queue);
#endif
}

Client::Client(ClientDetail &detail) :
  m_detail(detail), m_FrameChannel(NULL),
  m_RateControl(NULL),m_bitrate_is_initialized(false),
  m_inhouseClient(NULL),
  m_pHmpMRCPVoiceCaptureChannel(NULL),m_asrType(EN_XGATE_ASR_TYPE_UNKNOWN)
{
  m_isHost = FALSE;
  m_isConference = FALSE;
  m_clientCount = 0;
  m_asrType = m_detail.m_asrType;
  m_audioPayload = 0;

#ifdef RECORD_ENABLED
  m_rcvd_audio_queue = g_async_queue_new_full((GDestroyNotify)free_rcvd_audio_queue);
  m_rcvd_video_queue = g_async_queue_new_full((GDestroyNotify)free_rcvd_video_queue);
#endif
}

Client::~Client()
{
  if(m_FrameChannel) {
    delete m_FrameChannel;
  }
  if(m_inhouseClient){
    delete m_inhouseClient;
  }
  if(m_pHmpMRCPVoiceCaptureChannel){
    delete m_pHmpMRCPVoiceCaptureChannel;
  }
}

void Client::create_data_thread (Client *client,xGateHmpClientMsgType eMsgType, guint eMediaType)
{
  char *thr_name = NULL;
  ACE_THR_FUNC thr_func;
  ACE_thread_t *thr_id;
  if (eMsgType == XGATE_CLIENT_RCVD_RTP_DATA && eMediaType == MEDIA_CAPS_FOR_AUDIOVIDEO){
    thr_name = "rcv_data_thr";
    thr_func = client->client_rcv_data_thr_func;
    thr_id = &client->rcv_data_thrId;
  }else if (eMsgType == XGATE_CLIENT_SEND_RTP_DATA && eMediaType == MEDIA_CAPS_FOR_AUDIO){
    thr_name = "send_audio_data_thr";
    thr_func = client->client_send_audio_data_thr_func;
    thr_id = &client->send_audio_data_thrId;
  }else if (eMsgType == XGATE_CLIENT_SEND_RTP_DATA && eMediaType == MEDIA_CAPS_FOR_VIDEO){
    thr_name = "send_video_data_thr";
    thr_func = client->client_send_video_data_thr_func;
    thr_id = &client->send_video_data_thrId;
  }else if (eMsgType == XGATE_CLIENT_SEND_RTP_DATA && eMediaType == MEDIA_CAPS_FOR_AUDIOVIDEO){
    thr_name = "send_data_thr";
    thr_func = client->client_send_data_thr_func;
    thr_id = &client->send_data_thrId;
  }
  if (ACE_Thread::spawn(thr_func,
			     client,
			     THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, //THR_SCHED_RR
			     thr_id,
			     0,
			     5, /* priority */
			     0,
			     ACE_DEFAULT_THREAD_STACKSIZE,
			     0,
			     &thr_name) == -1)
  {
    XGLOG_ERROR("Client:: failed to start client thread !");
    return EN_XGATE_STATUS_ERROR;
  }
  else
  {
    XGLOG_INFO("Spawning data thread for client: %s mediaType: %d success thr_id: %d ",
                           client->m_detail.m_callId.c_str(), eMediaType,thr_id);
  }
}

void Client::client_rcv_data_thr_func (Client *client)
{
  xGateHmpClientThreadMsg *pMsg = NULL;
  guint8       ulen = 0;
  SFUController *pSFUController = xGateSFUtil::getSFUController();
  guint8       thread_name[17] = {'\0'};
  xGateHmpConference *pHmpConference = nullptr;

  XGLOG_INFO ("Entered client_rcv_data_thr_func() ");
  if(!(client->rcv_data_msg_q
        = new ACE_Message_Queue <ACE_MT_SYNCH, ACE_System_Time_Policy > ())){
    XGLOG_ERROR("Error in initializating audio send message queue ");
    return;
  }
  sprintf (thread_name, "%s", "rx_data_");
  ulen = strlen (thread_name);
  pHmpConference = pSFUController->find_conference_entry(client->m_detail.m_mgresourceId);
  if(!pHmpConference){
    XGLOG_ERROR("Failed to find entry in conference");
  }
  else {
    sprintf(&thread_name[ulen], "%u", pHmpConference->m_webrtcCount);
  }

  SET_THREAD_NAME (pthread_self (), thread_name);
  while (FALSE == client->m_StopSendRecvData)
  {
    ACE_Message_Block *pAmb = 0;
    int result = client->rcv_data_msg_q->dequeue_head(pAmb, NULL);
    if ( ( result == -1 ) && ( errno == EWOULDBLOCK ) )
    {
      continue;
    }
    pMsg = static_cast<xGateHmpClientThreadMsg*>(pAmb);
    switch (pMsg->m_msgType){
      case XGATE_CLIENT_RCVD_RTP_DATA:
      {
        gchar *element_name = static_cast <char *> (pMsg->ucName);
        GstBufferList *buffer_list = static_cast <GstBufferList *> (pMsg->param2);
        HmpFrameBin *src_frameRecvBin = NULL;
        Client *client = static_cast <Client *> (pMsg->param1);
        src_frameRecvBin = client->m_FrameChannel->m_pHmpFrameBin;
        src_frameRecvBin->on_new_data_rcvd_srcPad (client, element_name, buffer_list);
      }
      break;
      case XGATE_CLIENT_REC_EVENT_REQUEST:
      {
        Client *client = static_cast <Client *> (pMsg->param1);
        xGateMediaEvent recEvent = *(xGateMediaEvent *)pMsg->param2;
        HmpRecordSendChannel *channel = static_cast <HmpRecordSendChannel *> (pMsg->param3);
         XGLOG_INFO ("rcv_data_thr received XGATE_CLIENT_REC_EVENT_REQUEST for client: %s record event: %d",
                     client->m_detail.m_callId.c_str(), recEvent);
         if (recEvent == EN_MEDIA_EVENT_RECORD_STOP){
            if (channel){
              XGLOG_INFO ("stopping the recording channel for call_id: %s", client->m_detail.m_callId.c_str());
              delete channel;
              channel = nullptr;
            }
         }
         if (pMsg->param2)
            delete pMsg->param2;
      }
      break;
      case XGATE_CLIENT_TILE_CHANGE_REQUEST:
      {
        Client *client = static_cast <Client *> (pMsg->param2);
        PARTICIPANT_LIST *client_list = static_cast <PARTICIPANT_LIST *>(pMsg->param1);
        SFUController *pSFUController = xGateSFUtil::getSFUController();
        if(!client_list){
          XGLOG_ERROR("client list passed is NULL in TILE_CHANGE_REQUEST");
          break;
        }
        XGLOG_INFO ("Received XGATE_CLIENT_TILE_CHANGE_REQUEST from SFU controller");
        if(FALSE == client->handle_tile_change_request(client_list)){
          XGLOG_INFO("Failed to handle Tile Change Request with callID:%s",client->m_detail.m_callId.c_str());
        }
        if(client_list->size()){
          client_list->clear();
        }
        delete client_list;
      }
      break;

      case XGATE_CLIENT_THREAD_STOP:
      {
        XGLOG_INFO ("rcv_data_thr received XGATE_CLIENT_THREAD_STOP msg.. Exiting from loop");
        delete pMsg;
        flush_msg_queue (client->rcv_data_msg_q);
        goto flush_msg_q;
      }
      break;
      default:
        XGLOG_WARN ("Unknown message sent to rcv thread");
        break;
    }
    delete pMsg;
  }
  flush_msg_q:
    client->rcv_data_msg_q->flush();
    client->rcv_data_msg_q->close();
    delete client->rcv_data_msg_q;
    client->rcv_data_msg_q = NULL;

  XGLOG_INFO ("Exiting client_rcv_data_thr_func() ");
}

void Client::client_send_audio_data_thr_func (Client *client)
{
  xGateHmpClientThreadMsg *pMsg = NULL;
  XGLOG_INFO ("Entered client_send_audio_data_thr_func() ");
  if(!(client->send_audio_msg_q
        = new ACE_Message_Queue <ACE_MT_SYNCH, ACE_System_Time_Policy > ())){
    XGLOG_ERROR("Error in initializating audio send message queue ");
    return;
  }
  while (FALSE == client->m_StopSendRecvData)
  {
    ACE_Message_Block *pAmb = 0;
    int result = client->send_audio_msg_q->dequeue_head(pAmb, NULL);
    if ( ( result == -1 ) && ( errno == EWOULDBLOCK ) )
    {
      continue;
    }
    pMsg = static_cast<xGateHmpClientThreadMsg*>(pAmb);
    switch (pMsg->m_msgType){
      case XGATE_CLIENT_SEND_RTP_DATA:
      {
        gchar *element_name = static_cast <char *> (pMsg->ucName);
        GstBuffer *buffer = static_cast <GstBuffer *> (pMsg->param2);
        HmpFrameBin *src_frameRecvBin = NULL;
        src_frameRecvBin = client->m_FrameChannel->m_pHmpFrameBin;
        void *mlineDetails = (pMsg->param3);
        if (mlineDetails){
          src_frameRecvBin->handle_data_rcvd_rx_thread(client,
                            element_name, buffer, mlineDetails);
        }
      }
      break;
      default:
        XGLOG_WARN ("Unknown message sent to audio send thread");
        break;
    }
    delete pMsg;
  }
  client->send_audio_msg_q->flush();
  client->send_audio_msg_q->close();
  delete client->send_audio_msg_q;
  client->send_audio_msg_q = NULL;

  XGLOG_INFO ("Exiting client_send_audio_data_thr_func() ");
}

void Client::client_send_video_data_thr_func (Client *client)
{
  xGateHmpClientThreadMsg *pMsg = NULL;
  XGLOG_INFO ("Entered client_send_video_data_thr_func () ");
  if(!(client->send_video_msg_q
        = new ACE_Message_Queue <ACE_MT_SYNCH, ACE_System_Time_Policy > ())){
    XGLOG_ERROR("Error in initializating video send message queue ");
    return;
  }
  while (FALSE == client->m_StopSendRecvData)
  {
    ACE_Message_Block *pAmb = 0;
    int result = 0;
    result = client->send_video_msg_q->dequeue_head(pAmb, NULL);
    if ( ( result == -1 ) && ( errno == EWOULDBLOCK ) )
    {
      continue;
    }
    pMsg = static_cast<xGateHmpClientThreadMsg*>(pAmb);
    switch (pMsg->m_msgType){
      case XGATE_CLIENT_SEND_RTP_DATA:
      {
        gchar *element_name = static_cast <char *> (pMsg->ucName);
        GstBuffer *buffer = static_cast <GstBuffer *> (pMsg->param2);
        HmpFrameBin *src_frameRecvBin = NULL;
        src_frameRecvBin = client->m_FrameChannel->m_pHmpFrameBin;
        void *mlineDetails = (void*) pMsg->param3;
        if(mlineDetails){
          src_frameRecvBin->handle_data_rcvd_rx_thread(client,element_name,
                                                    buffer, mlineDetails);
        } else {
          XGLOG_ERROR ("mlineDetails are null for video detail");
        }
      }
      break;
      default:
        XGLOG_WARN ("Unknown message sent to audio send thread");
      break;
    }
    delete pMsg;
  }
  client->send_video_msg_q->flush();
  client->send_video_msg_q->close();
  delete client->send_video_msg_q;
  client->send_video_msg_q = NULL;
  XGLOG_INFO ("Exiting client_send_video_data_thr_func() ");
}

void Client::client_send_data_thr_func (Client *client)
{
  xGateHmpClientThreadMsg *pMsg = NULL;
  xGateHmpConference *pHmpConference = nullptr;
  SFUController *pSFUController = xGateSFUtil::getSFUController();
  guint8       thread_name[17] = {'\0'};
  guint8       ulen = 0;
  XGLOG_INFO ("Entered client_send_data_thr_func() ");
  if(!(client->send_data_msg_q
           = new ACE_Message_Queue <ACE_MT_SYNCH, ACE_System_Time_Policy > ())){
     XGLOG_ERROR("Error in initializating audio send message queue ");
     return;
  }
  sprintf (thread_name, "%s", "tx_data_");
  ulen = strlen (thread_name);
  pHmpConference = pSFUController->find_conference_entry(client->m_detail.m_mgresourceId);
  if(!pHmpConference){
    XGLOG_ERROR("Failed to find entry in conference");
  }
  else {
    sprintf(&thread_name[ulen], "%u", pHmpConference->m_webrtcCount);
  }
  SET_THREAD_NAME (pthread_self (), thread_name);

  while (FALSE == client->m_StopSendRecvData)
  {
    ACE_Message_Block *pAmb = 0;
    int result = client->send_data_msg_q->dequeue_head(pAmb, NULL);
    if ( ( result == -1 ) && ( errno == EWOULDBLOCK ) )
    {
      continue;
    }
    pMsg = static_cast<xGateHmpClientThreadMsg*>(pAmb);
    switch (pMsg->m_msgType){
      case XGATE_CLIENT_SEND_RTP_DATA:
      {
        gchar *element_name = static_cast <char *> (pMsg->ucName);
        GstBuffer *buffer = static_cast <GstBuffer *> (pMsg->param2);
        HmpFrameBin *src_frameRecvBin = NULL;
        src_frameRecvBin = client->m_FrameChannel->m_pHmpFrameBin;
        void *mlineDetails = (void*) pMsg->param3;
        if (mlineDetails){
           src_frameRecvBin->handle_data_rcvd_rx_thread(client,element_name, buffer, mlineDetails);
        }
      }
      break;
      case XGATE_CLIENT_THREAD_STOP:
      {
        XGLOG_INFO ("send_data_thr received XGATE_CLIENT_THREAD_STOP msg.. Exiting from loop");
        delete pMsg;
        flush_msg_queue (client->send_data_msg_q);
        goto flush_msg_q;
      }
      break;
      default:
      XGLOG_WARN ("Unknown message sent to send data thread");
      break;
    }
    pAmb->next (0);
    delete pMsg;
  }
flush_msg_q:
  client->send_data_msg_q->flush();
  client->send_data_msg_q->close();
  delete client->send_data_msg_q;
  client->send_data_msg_q = NULL;

  XGLOG_INFO ("Exiting client_send_data_thr_func() ");
}

void Client::flush_msg_queue (ACE_Message_Queue<ACE_MT_SYNCH, ACE_System_Time_Policy>  *msg_q)
{
  ACE_Message_Block *pAmb = 0;
  xGateHmpClientThreadMsg *pMsg = NULL;
  int result = 0;
  /* check if there are any messages and free the buffer list*/
  do {
    ACE_Time_Value tval ((time(NULL) ));
    tval += ACE_Time_Value(0,20);
    result = msg_q->dequeue_head(pAmb, &tval);
    if (pAmb){
      pMsg = static_cast<xGateHmpClientThreadMsg*>(pAmb);
      if (pMsg){
        void *vBuffObj = pMsg->param2;
        GstBufferList *buffer_list = NULL;
        GstBuffer     *buffer = NULL;

       if (GST_IS_BUFFER_LIST (vBuffObj))
         buffer_list = GST_BUFFER_LIST_CAST (vBuffObj);
       else
         buffer = GST_BUFFER_CAST (vBuffObj);

        if (buffer_list){
          XGLOG_INFO ("Flushing buffer_list: %p", buffer_list);
          gst_buffer_list_unref (buffer_list);
        }else if (buffer){
          XGLOG_INFO ("Flushing buffer: %p", buffer);
          gst_buffer_unref (buffer);
        }
        delete pMsg;
        pMsg = NULL;
      }
      pAmb = NULL;
    }
  } while ( result > 0 );
  XGLOG_INFO ("Exiting Client::flush_msg_queue () ");
}


void Client::stop_data_threads (Client *client)
{
  ACE_Message_Block *pAmb = 0;
  xGateHmpClientThreadMsg *pMsg = NULL;
  ACE_Time_Value tval ((time(NULL) ));
  tval += ACE_Time_Value(0,1);

  if(client->rcv_data_thrId) {
    /* Send message to data thread to stop */
    pMsg = new xGateHmpClientThreadMsg ();
    pMsg->m_msgType = XGATE_CLIENT_THREAD_STOP;
    pMsg->param1 = static_cast <void*> (client);
    pAmb = static_cast<xGateHmpClientThreadMsg*> (pMsg);
    if(client->rcv_data_msg_q != NULL && !client->rcv_data_msg_q->enqueue_prio(pAmb, &tval)){
      XGLOG_ERROR("Failed to push XGATE_CLIENT_THREAD_STOP msg to Client thread");
    }
  }

  if(client->send_data_thrId) {
    pMsg = new xGateHmpClientThreadMsg ();
    pMsg->m_msgType = XGATE_CLIENT_THREAD_STOP;
    pMsg->param1 = static_cast <void*> (client);
    pAmb = static_cast<xGateHmpClientThreadMsg*> (pMsg);
    if(client->send_data_msg_q != NULL && !client->send_data_msg_q->enqueue_prio(pAmb, &tval)){
      XGLOG_ERROR("Failed to push XGATE_CLIENT_THREAD_STOP msg to Client thread");
    }
  }
#ifndef USE_GST_AUDIO_MIXER_PLUGIN
  if(client->mixer_msg_q) {
    pMsg = new xGateHmpClientThreadMsg ();
    pMsg->m_msgType = XGATE_CLIENT_THREAD_STOP;
    pMsg->param1 = static_cast <void*> (client);
    pAmb = static_cast<xGateHmpClientThreadMsg*> (pMsg);
    if(client->mixer_msg_q != NULL && !client->mixer_msg_q->enqueue_prio(pAmb, &tval)){
      XGLOG_ERROR("Failed to push XGATE_CLIENT_THREAD_STOP msg to Client thread");
    }
  }
#endif
}

void Client::on_twcc_stats_recv (GstElement *rtpsession,
                                 GstStructure *pTwccStats,
                                 gpointer data)
{
   Client          *pClient = (Client *)data;
   TWCCStats       CurrentTwccStat = {0};
   RTPRecvReportStats sRTCPStat = {0};
   if (pClient->m_mlineOffer_vector.size () > 0){
     memset (&CurrentTwccStat, 0, sizeof(CurrentTwccStat));
     memset (&sRTCPStat, 0, sizeof (sRTCPStat));
     pClient->get_twcc_stats (pClient->m_FrameChannel->m_pHmpFrameBin,
                             &CurrentTwccStat);
     pClient->get_rtp_stats ();

     pClient->accumulate_rtcp_stats (pClient, &sRTCPStat);

     pClient->process_rcvd_stats (pClient, &CurrentTwccStat, &sRTCPStat);

   }
}

void Client::get_twcc_stats(HmpFrameBin *pFrameBin,
                           TWCCStats *pCurrentTwccStat)
{
   GstStructure    *pTwccGstStruct = NULL;

  if(pFrameBin->gst_rtp_session){
    g_object_get (pFrameBin->gst_rtp_session, "twcc-stats", &pTwccGstStruct, NULL);
  }
  if(pTwccGstStruct){
    gst_structure_get (pTwccGstStruct,
                     "bitrate-sent", G_TYPE_UINT, &pCurrentTwccStat->bitrate_sent,
                     "bitrate-recv", G_TYPE_UINT, &pCurrentTwccStat->bitrate_recv,
                     "packets-sent", G_TYPE_UINT, &pCurrentTwccStat->packets_sent,
                     "packets-recv", G_TYPE_UINT, &pCurrentTwccStat->packets_recv,
                     "packet-loss-pct", G_TYPE_DOUBLE, &pCurrentTwccStat->packet_loss_pct,
                     "avg-delta-of-delta", G_TYPE_INT64, &pCurrentTwccStat->avg_delta_of_delta,
                     "avg-delta-of-delta-change", G_TYPE_DOUBLE, &pCurrentTwccStat->avg_delta_of_delta_change,
                      NULL );
    XGLOG_TRACE ("TWCC packet-loss-pct: %f avg-delta-of-delta: %" GST_STIME_FORMAT ","
                      "avg-delta-of-delta-change: %f",
                      pCurrentTwccStat->packet_loss_pct,
                      GST_STIME_ARGS (pCurrentTwccStat->avg_delta_of_delta),
                      pCurrentTwccStat->avg_delta_of_delta_change);

    gst_structure_free (pTwccGstStruct);
  }
}

void Client::get_rtp_stats ()
{
   GstStructure    *rtp_stats = NULL;
   GValueArray     *ga_rtp_source_stats = NULL;
   GValue         *val = NULL;
   guint           uiIndex = 0;
   GstStructure    *ssrc_stats = NULL;
   guint           uissrc = 0;
   guint           rb_ssrc = 0;
   gboolean        bIsSender = FALSE;
   gboolean        bIsInternal = FALSE;
   gint            clockrate = 0;
   offerDetail    *pOfferDetails = NULL;
   RTPRecvReportStats *pRRStats = NULL;
   gboolean        bIsRRBlock = FALSE;
   Client          *pClient = this;
   mlineofferDetail *mlineDetails = NULL;
   HmpFrameBin     *pHmpFrameBin = pClient->m_FrameChannel->m_pHmpFrameBin;

   if (pClient->m_mlineOffer_vector.size () > 0){
     g_object_get (pHmpFrameBin->rtp_session, "stats", &rtp_stats, NULL);
     gst_structure_get (rtp_stats, "source-stats", G_TYPE_VALUE_ARRAY,
                                 &ga_rtp_source_stats, NULL);
     if (ga_rtp_source_stats){
       /*
        * Not all the fields provided in RTP stats are relevant to
        * calculate the bit rate based on lost based algorithm
        *
        * Stats required to determine packets loss on Tx side,
        * for a specific m-line can  be determined from Receiver
        * report block (RR report).
        *
        * remote end sends rtcp RR report with ssrc_1 field configured with
        * the ssrc value of what MG configures when it sends RTP packets
        *
        * Accesing fields from report block fields of rtpstats structure
        * REFER rtpmanager/rtpsource.c of gstreamer good plugin
        *
        * is-sender, internal fields will convey if the ssrc sends
        * data and whether ssrc belongs to local or remote
        *
        * filter internal: 0 and is-sender: 0 have-rb: 1 (this gives
        * statistics of remote end sending receiving status in RTCP RR
        * report for the packets sent by MG)
        */
       for (uiIndex = 0; uiIndex < ga_rtp_source_stats->n_values; uiIndex++){
         val = g_value_array_get_nth (ga_rtp_source_stats, uiIndex);
         ssrc_stats = gst_value_get_structure (val);
         bIsSender = bIsInternal = FALSE;
         gst_structure_get (ssrc_stats, "ssrc", G_TYPE_UINT, &uissrc,
                                        "is-sender", G_TYPE_BOOLEAN, &bIsSender,
                                        "internal", G_TYPE_BOOLEAN, &bIsInternal,
                                        "clock-rate", G_TYPE_INT, &clockrate,
                                        "have-rb", G_TYPE_BOOLEAN, &bIsRRBlock,
                                        "rb-ssrc", G_TYPE_UINT, &rb_ssrc,
                                        NULL);
         /* Loop in the offer map list to find the report for which ssrc it is*/
         if (!bIsInternal && !bIsSender && bIsRRBlock){
           for (int i = 0; i < pClient->m_mlineOffer_vector.size(); i++){
              mlineDetails = pClient->m_mlineOffer_vector.at (i);
              if (mlineDetails){

            /*XGLOG_INFO ("comparing ssrc: %u with audio.uiTxSSRCNum: %u , videoTx SSRC: %u",uissrc,
                             mlineDetails->audioOfferDetails.uiTxSSRCNum,
                             mlineDetails->videoOfferDetails.uiTxSSRCNum);*/
                pRRStats = NULL;
                if (mlineDetails->audioOfferDetails.uiTxSSRCNum == rb_ssrc){
                  pRRStats = &mlineDetails->audioOfferDetails.sCurrentRTPStats;
                } else if (mlineDetails->videoOfferDetails.uiTxSSRCNum == rb_ssrc) {
                  pRRStats = &mlineDetails->videoOfferDetails.sCurrentRTPStats;
                }
                if (pRRStats){
                  guint64 rtptime = 0;
                  guint  sr_octet_count = 0 ;
                  guint  sr_pkt_count = 0;
                  guint  rb_ssrc  = 0;
                  gint    packetslost;
                  memset (pRRStats, 0, sizeof (RTPRecvReportStats));
                  gst_structure_get (ssrc_stats,
                  "rb-fractionlost", G_TYPE_UINT, &pRRStats->uiLastRRFractLost,
                  "rb-packetslost", G_TYPE_INT, &pRRStats->uiLastRRPacketLost,
                  "rb-exthighestseq", G_TYPE_UINT, &pRRStats->uiLastRRExtendedSeqNum,
                  "rb-jitter", G_TYPE_UINT, &pRRStats->uiInterArrivalJitter,
                  "rb-round-trip", G_TYPE_UINT, &pRRStats->uiRTT,
                  "rb-lsr", G_TYPE_UINT, pRRStats->uiLastSR,
                  "rb-dlsr", G_TYPE_UINT, pRRStats->uiDelayLastSR,
                  NULL );
                }
              }
           }
         }
       }
       g_value_array_free (ga_rtp_source_stats);
       gst_clear_structure (&rtp_stats);
     }
   }
}

void Client::process_rcvd_stats(Client *client,
                                TWCCStats *pTwccStat,
                                RTPRecvReportStats *pRRStat)
{
  HmpFrameBin *pHmpFrameBin = client->m_FrameChannel->m_pHmpFrameBin;
  xGateRateControlState control_state;
  RateControl *rate_control;
  std::map<string, HmpFrameChannel*>::iterator itr;
  gfloat gTotalPktLostPerct = 0.0;

  /*copy the current twcc stats*/
  client->sTwccAvgStat.bitrate_sent = pTwccStat->bitrate_sent;
  client->sTwccAvgStat.bitrate_recv = pTwccStat->bitrate_recv;
  client->sTwccAvgStat.packets_sent = pTwccStat->packets_sent;
  client->sTwccAvgStat.packets_recv = pTwccStat->packets_recv;
  client->sTwccAvgStat.avg_delta_of_delta = pTwccStat->avg_delta_of_delta;
  client->sTwccAvgStat.avg_delta_of_delta_change = pTwccStat->avg_delta_of_delta_change;

  gTotalPktLostPerct = pTwccStat->packet_loss_pct + pRRStat->uiLastRRFractLost;
  client->sTwccAvgStat.packet_loss_pct = gTotalPktLostPerct;

  XGLOG_TRACE ("client: %s uiLastRRFractLost: %lu twcc_packet_loss_pct: %f total_loss: %f avg-delta: %lld",
      client->m_detail.m_callId.c_str(),
      pRRStat->uiLastRRFractLost, pTwccStat->packet_loss_pct, gTotalPktLostPerct,
      pTwccStat->avg_delta_of_delta);

  if(client->m_bitrate_is_initialized == false){
    client->m_RateControl = new RateControl(sTwccAvgStat.bitrate_recv);
    client->m_bitrate_is_initialized = true;
  }

  if(client->m_RateControl){
    rate_control = client->m_RateControl;
    control_state = rate_control->Update(client, pTwccStat);
  }
  if(control_state == EN_XGATE_RCHOLD){
    XGLOG_TRACE ("Rate Control State is EN_XGATE_RCHOLD for %s",client->m_detail.m_callId.c_str());
  }else if(control_state == EN_XGATE_RCINCREASE){
    //m_ace_lock.lock();
    client->sent_bitrate = client->m_RateControl->GetCurrentBitrate();
   // client->send_tmmbr_message_set = TRUE;
    XGLOG_TRACE ("Rate Control State is EN_XGATE_RC_INCREASE for %s to bitrate %u",
                client->m_detail.m_callId.c_str(),client->sent_bitrate);
    //m_ace_lock.release();
  }else if(control_state == EN_XGATE_RCDECREASE){
    //m_ace_lock.lock();
    client->sent_bitrate = client->m_RateControl->GetCurrentBitrate();
  //  client->send_tmmbr_message_set = TRUE;
    XGLOG_TRACE ("Rate Control State is EN_XGATE_RC_DECREASE for %s to bitrate %u",
                client->m_detail.m_callId.c_str(),client->sent_bitrate);
    //m_ace_lock.release();
  }
  /* Call the below function to check and change the resolution, bitrate*/
  check_update_transmit_resolution (client, control_state);
}

void Client::accumulate_rtcp_stats(Client *pClient,
                                   RTPRecvReportStats *pRRStat)
{
  mlineofferDetail *mlineDetails = NULL;
  offerDetail      *pOfferDetails = NULL;
  HmpFrameBin     *pHmpFrameBin = pClient->m_FrameChannel->m_pHmpFrameBin;
  memset (pRRStat, 0, sizeof(RTPRecvReportStats));
  if (pClient->m_mlineOffer_vector.size () > 0){
     for (int i = 0; i < pClient->m_mlineOffer_vector.size(); i++){
       mlineDetails = pClient->m_mlineOffer_vector.at (i);
       /*
        * compare the previous and current rtcp parameters and
        * add the losses and delay if any for all the m-lines
        */
       if (mlineDetails){
         pOfferDetails = &mlineDetails->audioOfferDetails;
         pClient->compare_rtcp_params (mlineDetails, MEDIA_CAPS_FOR_AUDIO, pRRStat);

         pOfferDetails = &mlineDetails->videoOfferDetails;
         pClient->compare_rtcp_params (mlineDetails, MEDIA_CAPS_FOR_VIDEO, pRRStat);
       }
     }
  }
}

void Client::compare_rtcp_params (mlineofferDetail *pmlineDetails,
                                  guint uiMediaType,
                                  RTPRecvReportStats *pRRStats)
{
  offerDetail *pOfferDetails = NULL;
  Client      *pClient = NULL;
  if (uiMediaType == MEDIA_CAPS_FOR_AUDIO)
    pOfferDetails = &pmlineDetails->audioOfferDetails;
  else
    pOfferDetails = &pmlineDetails->videoOfferDetails;

  /* return if there is no change in rtcp statistics.
   */
  if (pOfferDetails->sPrevRTPStats.uiLastRRExtendedSeqNum
      == pOfferDetails->sCurrentRTPStats.uiLastRRExtendedSeqNum){
    return;
  }
  pClient = pmlineDetails->pClient;
  XGLOG_TRACE ("From Client: %s Tx client: %s ssrc: %u rb-fractionlost: %u"
                              " rb-packetslost: %d rb-exthighestseq: %u "
                              " rb-jitter: %u last SR: %u last DLSR: %u rtt: %u",
                              pClient->m_detail.m_callId.c_str(),
                              pmlineDetails->callId_offer_generated.c_str(),
                              pOfferDetails->uiTxSSRCNum,
                              pRRStats->uiLastRRFractLost,
                              pRRStats->uiLastRRPacketLost,
                              pRRStats->uiLastRRExtendedSeqNum,
                              pRRStats->uiInterArrivalJitter,
                              pRRStats->uiLastSR, pRRStats->uiDelayLastSR,
                              pRRStats->uiRTT);

  pRRStats->uiLastRRFractLost += pOfferDetails->sCurrentRTPStats.uiLastRRFractLost;
  /*replace the current status with previous statistics */
  memcpy (&pOfferDetails->sPrevRTPStats,
         &pOfferDetails->sCurrentRTPStats, sizeof(RTPRecvReportStats));
}

void Client::check_update_transmit_resolution (Client *pClient,
                                               xGateRateControlState new_control_state)
{
  mlineofferDetail *mlineDetails = NULL;
  offerDetail      *pOfferDetails = NULL;
  HmpFrameBin      *pHmpFrameBin = pClient->m_FrameChannel->m_pHmpFrameBin;
  RateControl      *rate_control = pClient->m_RateControl;
  Scalability_Struct_Info *pScalabilityStruct = NULL;

  if (!rate_control){
    return;
  }

 /* if control state is decrease or increase, iterate through the
  * list of offer details to find the m-line whose Tx bit rate is more than
  * other m-lines and try to reduce it by changing the scaler ID
  */
  if (new_control_state == EN_XGATE_RCHOLD){
    return;
  }

  if (pClient->m_mlineOffer_vector.size () > 0){
    for (int i = 0; i < pClient->m_mlineOffer_vector.size(); i++){
      mlineDetails = pClient->m_mlineOffer_vector.at (i);
       /*
        * compare the previous and current rtcp parameters and
        * add the losses and delay if any for all the m-lines
        */
      if (mlineDetails){
        pOfferDetails = &mlineDetails->videoOfferDetails;
        if (0 == strncmp (pOfferDetails->encoding_name, "VP9", 3)){
           pScalabilityStruct = &pOfferDetails->sSvcInfo.sScalabilitStructInfo;
           /*check the current spatial ID, num of layers*/
           if (pScalabilityStruct->ucNumSpatialLayers > 1) {
             if ((pOfferDetails->sSvcInfo.ucTargetSpatialLayerID > 0 )
                 && (new_control_state == EN_XGATE_RCDECREASE)){

                 if (pOfferDetails->sSvcInfo.ucTargetSpatialLayerID > 0)
                   pOfferDetails->sSvcInfo.ucTargetSpatialLayerID--;

                   XGLOG_INFO ("callid: %s ==> call_id: %s state decrease from : %d to layer: %d num_layer: %d",
                   pClient->m_detail.m_callId.c_str(),
                   mlineDetails->callId_offer_generated.c_str(),
                   pOfferDetails->sSvcInfo.ucCurrentSpatialLayerID,
                   pOfferDetails->sSvcInfo.ucTargetSpatialLayerID,
                   pScalabilityStruct->ucNumSpatialLayers);
              } else if ((pOfferDetails->sSvcInfo.ucTargetSpatialLayerID
                          < pScalabilityStruct->ucNumSpatialLayers - 1)
                        && (new_control_state == EN_XGATE_RCINCREASE)){

                 /*
                   * 1. send layer 0 packets if client is mobile as it would be smaller screen
                   *   and layer 1 resoluton will be sufficient
                   *
                   * 2. if mid line is other than screen share and number of participants displayed
                   *    in tile is less than 4 , send layer 1 packets other wise send layer 0 resolution
                   */
                   if (pClient->eDeviceType == XGATE_DEVICE_TYPE_MOBILE)
                   {
                      pOfferDetails->sSvcInfo.ucTargetSpatialLayerID = LAYER_ID_1;
                   }else if (0 != strcmp(mlineDetails->callId_offer_generated.c_str(), "screenshare")){
                     if (pClient->m_mlineOffer_vector.size() - 2 >= 4)
                        pOfferDetails->sSvcInfo.ucTargetSpatialLayerID = LAYER_ID_0;
                      else
                        pOfferDetails->sSvcInfo.ucTargetSpatialLayerID = LAYER_ID_1;
                      XGLOG_TRACE ("callId: %s uiTxSSRCNum: %u vect_size: %d configured ucCurrentID: %d ucTarget: %u",
                                mlineDetails->callId_offer_generated.c_str(), pOfferDetails->uiTxSSRCNum,
                                pClient->m_mlineOffer_vector.size() - 2,
                                pOfferDetails->sSvcInfo.ucCurrentSpatialLayerID,
                                pOfferDetails->sSvcInfo.ucTargetSpatialLayerID);
                   }
                   else {
                      pOfferDetails->sSvcInfo.ucTargetSpatialLayerID++;
                      XGLOG_INFO ("call_id: %s ===> offer_id: %s state: increase layer: %d to layer: %d num_layer: %d",
                      pClient->m_detail.m_callId.c_str(),
                      mlineDetails->callId_offer_generated.c_str(),
                      pOfferDetails->sSvcInfo.ucCurrentSpatialLayerID,
                      pOfferDetails->sSvcInfo.ucTargetSpatialLayerID,
                      pScalabilityStruct->ucNumSpatialLayers);

                   }
              }
           }
        } else {
          continue;
        }
      }
    }
  }
}

int Client::client_thr_func (void *arg)
{
  ACE_Message_Block *pAmb = 0;
  Client *client = (Client *)arg;
  XGLOG_INFO("Entered Client::client_thr_func client->m_run: %d", client->m_run);
  while(client->m_run)
  {
    int result = client->getq(pAmb, NULL);
    if ( ( result == -1 ) && ( errno == EWOULDBLOCK ) )
    {
      continue;
    }
    if ((result != -1))
    {
      client->process_client_msg(pAmb);
    }
    else
    {
      XGLOG_ERROR("Client::client_thread_function () Error in Message Queue");
      client->m_run = false;
    }
    // delete the message received
    if(pAmb)
    {
      delete pAmb;
      pAmb = NULL;
    }
  }
  /*TODO: Delete the client objects when conference RELEASE msg is received */
  //delete client;
  XGLOG_INFO("Client::client_thr_func stopped");
  return 0;
}


//bool Client:: process_client_msg(ACE_Message_Block *pAmb)
void Client:: process_client_msg (xGateHmpClientThreadMsg *pMsg)
{
 // xGateHmpClientThreadMsg *pMsg = static_cast<xGateHmpClientThreadMsg*>(pAmb);
  switch (pMsg->m_msgType){
    case XGATE_CLIENT_CREATE_CHANNEL:
    {
      XGLOG_INFO("Received msg to create XGATE_CLIENT_CREATE_CHANNEL");
      Client *client = static_cast <Client *> (pMsg->param2);
      guint uiCount = *(static_cast <int *>(pMsg->param3));
      HmpFrameBin *pHmpFrameBin = NULL;
      const char *thr_name = "send_data_thr";
      SFUController *pSFUController = xGateSFUtil::getSFUController();
      if(this != client){
        XGLOG_ERROR("this pointer and client pointer passed do not match");
        break;
      }
      ClientDetail *clientDetail = &client->m_detail;
      if(!create_frame_channel(uiCount)) {
        XGLOG_ERROR( "create_frame_channel, creating frame recv channel failed!!");
        return;
      } else{
        pHmpFrameBin = client->m_FrameChannel->m_pHmpFrameBin;
        pHmpFrameBin->handle_sdp_offer(client);
        pHmpFrameBin->eResponseMsgType = *((xGateMgMsgType*) pMsg->param1);
        free (pMsg->param1);
        pMsg->param1 = NULL;
        if (pMsg->param3){
          free (pMsg->param3);
          pMsg->param3 = NULL;
        }
      }
      if ((clientDetail->m_joinerType != EN_JOINER_TYPE_SCREENSHARE) &&
           ((pHmpFrameBin->m_negotiated_audio_payload_num >=0) ||
            (pHmpFrameBin->m_negotiated_video_payload_num > 0))){
          XGLOG_INFO ("creating send data thread for call: %s",
                clientDetail->m_callId.c_str());
          client->create_data_thread (client,
                  XGATE_CLIENT_SEND_RTP_DATA, MEDIA_CAPS_FOR_AUDIOVIDEO);
      }
      if (NULL == pSFUController->m_ThreadPool_info.m_rx_data_pool)
        client->create_data_thread (client, XGATE_CLIENT_RCVD_RTP_DATA, MEDIA_CAPS_FOR_AUDIOVIDEO);

      //creating channel for STT
      if ((pHmpFrameBin->m_negotiated_audio_payload_num >=0) &&
         (clientDetail->m_joinerType != EN_JOINER_TYPE_SCREENSHARE)) {
        if(m_asrType == EN_XGATE_ASR_INHOUSE){
         //Creating inhouse client
          if(!CreateInhouseClient(client->m_detail.m_mgresourceId,
                                client->m_detail.m_conn_id,
                                client->m_detail.m_device_id,
                                client->m_detail.is_ccaas))
          {
       	    XGLOG_ERROR( "creating inhouse client for call_id:%s failed!!",m_detail.m_callId);
          }
        }
      }
    }
    break;
    case XGATE_CLIENT_SCREENSHARE_REQUEST:
      {
        Client *client = static_cast <Client *> (pMsg->param2);
        PARTICIPANT_LIST *client_list = static_cast <PARTICIPANT_LIST *>(pMsg->param1);
        SFUController *pSFUController = xGateSFUtil::getSFUController();
        if(this != client){
          XGLOG_ERROR("this pointer: %p and client pointer: %p passed do not match", this, client);
          break;
        }
        if(!client_list){
          XGLOG_ERROR("client list passed is NULL in TILE_CHANGE_REQUEST");
          break;
        }
        XGLOG_INFO ("Received XGATE_CLIENT_SCREENSHARE_REQUEST from SFU controller");
        if(FALSE == handle_screenshare_request(client_list)){
          XGLOG_INFO("Failed to handle Screenshare Request with callID:%s",client->m_detail.m_callId.c_str());
        }
        if(client_list->size()){
          client_list->clear();
        }
        delete client_list;
      }
      break;
    case XGATE_CLIENT_SDP_UPDATE_REQUEST:
    {
      Client *client = static_cast <Client *> (pMsg->param2);
      string remote_sdp = pMsg->str_param;
      HmpFrameBin *pHmpFrameBin = NULL;
      HmpFrameChannel *pHmpFrameChannel = NULL;
      PARTICIPANT_LIST *client_list = static_cast <PARTICIPANT_LIST *>(pMsg->param1);
      if(this != client){
        XGLOG_ERROR("this pointer: %p and client pointer: %p passed do not match", this, client);
        break;
      }
      if(!client_list){
        XGLOG_ERROR("client list passed is NULL");
        break;
      }
      XGLOG_INFO ("Received XGATE_CLIENT_SDP_UPDATE_REQUEST from SFU controller");
      for (int participant_index = 0; participant_index < client_list->size(); participant_index++){
        ParticipantInfo sPartInfo = client_list->at(participant_index);
        XGLOG_INFO ("sPartInfo: %p call_id: %s", sPartInfo,sPartInfo.call_id.c_str());
      }

      if(FALSE == handle_sdp_update_request (client_list, remote_sdp)){
        XGLOG_INFO("Failed to answer offer with callID:%s",
       	client->m_detail.m_callId.c_str());
      }
      if(remote_sdp.size()){
        remote_sdp.clear();
      }
      if (client_list->size()){
        client_list->clear();
      }
      delete client_list;
    }
    break;
    case XGATE_CLIENT_ICE_CANDIDATE_GENERATED:
    {
      HmpFrameChannel *pHmpFrameChannel = static_cast <HmpFrameChannel *> (pMsg->param1);
      Client *client = static_cast <Client *> (pMsg->param2);
      if (pHmpFrameChannel && client){
        HmpFrameBin *frameBin = pHmpFrameChannel->m_pHmpFrameBin;
        XGLOG_INFO ("ICE candidate generted by client: %p webrtcbin: %p ",
       	client->m_detail.m_callId.c_str(), frameBin);
        frameBin->handle_ice_candidate_msg (frameBin, frameBin->m_webRtcBinType, client);
      }
    }
    break;
    case XGATE_CLIENT_SEND_UPDATED_SDP:
    {
      HmpFrameChannel *pHmpFrameChannel = static_cast <HmpFrameChannel *> (pMsg->param1);
      Client *client = static_cast <Client *> (pMsg->param2);
      if (pHmpFrameChannel && client){
        HmpFrameBin *frameBin = pHmpFrameChannel->m_pHmpFrameBin;
        frameBin->handle_send_updated_sdp_msg(frameBin, client);
      }
    }
    break;
#ifdef RECORD_ENABLED
    case XGATE_CLIENT_REC_EVENT_REQUEST:
    {
      Client *client = static_cast <Client *> (pMsg->param1);
      xGateMediaEvent recEvent = client->m_detail.m_mediaEvent;
      if(this != client){
        XGLOG_ERROR("this pointer: %p and client pointer: %p passed do not match", this, client);
        break;
      }
      XGLOG_INFO ("Received XGATE_CLIENT_REC_EVENT_REQUEST from SFU controller");

      if(FALSE == handle_recording_request(recEvent)){
        XGLOG_INFO("Failed to handle record request:%d for callID:%s",
                       recEvent,client->m_detail.m_callId.c_str());
        break;
      }
    }
    break;
    case XGATE_CLIENT_REC_EVENT_RESPONSE:
    {
      Client *client = static_cast <Client *> (pMsg->param1);
      xGateMediaEvent recEvent = client->m_detail.m_mediaEvent;
      if(this != client){
        XGLOG_ERROR("this pointer: %p and client pointer: %p passed do not match", this, client);
        break;
      }
      XGLOG_INFO ("Received XGATE_CLIENT_REC_EVENT_REQUEST from SFU controller");

      if(FALSE == handle_recording_response(recEvent)){
      XGLOG_INFO("Failed to handle record request:%d for callID:%s",
                     recEvent,client->m_detail.m_callId.c_str());
      }
    }
    break;
#endif
    case XGATE_CLIENT_DELETE_ALL_CHANNEL:
    {
      Client *client = static_cast <Client *> (pMsg->param1);
      ACE_THR_FUNC_RETURN exitStatus = 0;
      if(this != client){
        XGLOG_ERROR("this pointer and client pointer passed do not match");
        break;
      }
      XGLOG_INFO("Received XGATE_CLIENT_DELETE_ALL_CHANNEL for client: %s waiting for thr to join",
       	 client->m_detail.m_callId.c_str());
      /* Wait for recv and send threads to quit*/
      ACE_THR_FUNC_RETURN status = 0;
      if (client->rcv_audio_data_thrId){
        ACE_Thread::cancel (client->rcv_audio_data_thrId);
        ACE_Thread::join(client->rcv_audio_data_thrId, &exitStatus);
      }
      if (client->rcv_video_data_thrId){
        ACE_Thread::cancel (client->rcv_video_data_thrId);
      }
      if (client->rcv_data_thrId){
        ACE_Thread::join(client->rcv_data_thrId, &exitStatus);
        XGLOG_INFO ("rcv_data_thrId: joined. exit status: %d", exitStatus);
      }
      if (client->send_audio_data_thrId){
        ACE_Thread::cancel (client->send_audio_data_thrId);
      }
      if (client->send_video_data_thrId){
        ACE_Thread::cancel (client->send_video_data_thrId);
      }
      #ifndef USE_GST_AUDIO_MIXER_PLUGIN
      if (client->audio_mixer_thrId){
        ACE_Thread::join(client->audio_mixer_thrId, &exitStatus);
        XGLOG_INFO ("Mixer Audio thread joined. exit status: %d", exitStatus);
      }
      #endif
      if (client->send_data_thrId){
        ACE_Thread::join(client->send_data_thrId, &exitStatus);
        XGLOG_INFO ("send_data_thrId joined. exit status: %d", exitStatus);
      }
      XGLOG_INFO ("client: %s All data threads cancelled",
                         client->m_detail.m_callId.c_str());
      #ifdef RECORD_ENABLED
      if(m_rcvd_audio_queue){
        XGLOG_INFO("Client::Stopping recoridng rcvd audio queue");
        g_async_queue_unref(m_rcvd_audio_queue);
        m_rcvd_audio_queue = NULL;
      }
      if(m_rcvd_video_queue){
        XGLOG_INFO("Client::Stopping recoridng rcvd video queue");
        g_async_queue_unref(m_rcvd_video_queue);
        m_rcvd_video_queue = NULL;
      }
      #endif
      char *callId_str = client->m_detail.m_callId.c_str();
      handle_client_delete_channel (XGATE_CLIENT_DELETE_ALL_CHANNEL,callId_str,NULL);
      /* flush and close the message Q */
      client->flush();
      client->close();
      m_run = FALSE;
    }
    break;
    case XGATE_CLIENT_DELETE_SEND_CHANNEL:
    {
      Client *client = static_cast <Client *> (pMsg->param2);
      char *callId_str = static_cast <char *> (pMsg->param3);
      if(this != client){
        XGLOG_ERROR("this pointer and client pointer passed do not match");
        break;
      }
      handle_client_delete_channel (XGATE_CLIENT_DELETE_SEND_CHANNEL,
                                   callId_str,
                                   NULL);
      /* Free the memory allocated for call string */
      free(callId_str);
    }
    break;
    case XGATE_CLIENT_DELETE_RECV_CHANNEL:
    {
      Client *client = static_cast <Client *> (pMsg->param1);
      if(this != client){
        XGLOG_ERROR("this pointer and client pointer passed do not match");
        break;
      }
      XGLOG_INFO("Received XGATE_CLIENT_DELETE_RECV_CHANNEL for callID: %s",
       	  client->m_detail.m_callId.c_str());
      handle_client_delete_channel (XGATE_CLIENT_DELETE_RECV_CHANNEL,NULL,NULL);
    }
    break;
    case XGATE_CLIENT_SEND_KEY_FRAME_REQUEST:
    {
      Client *client = static_cast <Client *> (pMsg->param1);
      HmpFrameBin *frameRecvBin = NULL;
      if(this != client){
        XGLOG_ERROR("this pointer and client pointer passed do not match");
        break;
      }
      if(client->m_FrameChannel){
        frameRecvBin = client->m_FrameChannel->m_pHmpFrameBin;
        frameRecvBin->SendKeyFrameRequest (client);
      }
    }
    break;
    case XGATE_CLIENT_PEER_CONNECTION_FAILED:
    {
      XGLOG_INFO ("Client::XGATE_CLIENT_PEER_CONNECTION_FAILED");
      HmpFrameChannel *pHmpFrameChannel = static_cast <HmpFrameChannel *> (pMsg->param1);
      Client *client = static_cast <Client *> (pMsg->param2);
      if (pHmpFrameChannel && client){
        HmpFrameBin *frameBin = pHmpFrameChannel->m_pHmpFrameBin;
        frameBin->handle_connection_failed_msg(frameBin, frameBin->m_webRtcBinType, client);
      }else {
        XGLOG_INFO ("Failed to intimate ice connecion failed for client: %p",
             client->m_detail.m_callId.c_str());
      }
      break;
    }
    case XGATE_CLIENT_SEND_EVENT_SINK_PAD:
    {
      guint8  u8StreamId [100] = {'\0'};
      guint uPadType = *(static_cast <int *>(pMsg->param3));
      offerDetail *offerDetails = static_cast <offerDetail*> (pMsg->param2);
      Client      *client = static_cast <Client*> (pMsg->param1);
      HmpFrameBin *frameRecvBin = this->m_FrameChannel->m_pHmpFrameBin;
      gboolean     bSendEvent = FALSE;
      sprintf (u8StreamId, "%s_%s_%u_%u", offerDetails->sinkPadName.c_str(),
                               offerDetails->encoding_name,
                               offerDetails->uiTxSSRCNum,
                               offerDetails->uiPayloadNum);
       XGLOG_INFO ("RCVD SEND_EVENT_SINK_PAD for client: %s  pad: %s stream_id: %s ",
                           this->m_detail.m_callId.c_str(), pMsg->ucName, u8StreamId);
      if (!client && !pMsg->str_param.empty() &&
        (0 != strcmp(pMsg->str_param.c_str(),"audio_only" )) &&
        (0 != strcmp(pMsg->str_param.c_str(),"screenshare" ))){
         SFUController *pSFUController = xGateSFUtil::getSFUController();
         XGLOG_INFO ("dst_client ptr is null fetching detail from conference list for callId: %s",
                    pMsg->str_param.c_str());
         xGateHmpConference *pHmpConference =
              pSFUController->find_conference_entry(this->m_detail.m_mgresourceId);
         if(!pHmpConference){
            XGLOG_ERROR("Failed to find entry in conference");
            break;
         }
         client = pHmpConference->get_participant_entry (pMsg->str_param);
         if (!client){
           XGLOG_ERROR("Failed to get client entry for callId: %s", pMsg->str_param.c_str());
           break;
         }
         pMsg->str_param.erase();
       }
#ifdef AUDIO_MCU_ENABLED
      // if we are using gstreamer mixer, for every input audio stream, one pad
      // should be created. Since we are mixing  audio and transmitting, there will
      // be only 1 sink pad event. However video is transmitting on different m-lines
      // and multiple video sink pad events will be created .
      //
      // if Gsteamer mixer is used then when video sink pad event gets triggered,
      // create one audio mixer sink pad
  #ifdef USE_GST_AUDIO_MIXER_PLUGIN
      if(uPadType == MEDIA_CAPS_FOR_VIDEO && client){
        XGLOG_INFO ("RCVD SEND_EVENT_SINK_PAD pad: %s stream_id: %s client: %s src_client: %s",
                           pMsg->ucName, u8StreamId, this->m_detail.m_callId.c_str(),
                           client->m_detail.m_callId.c_str());
        frameRecvBin->add_audiomixer_sinkpad(this, client,offerDetails->uiTxSSRCNum);
      }
  #else
      // For MG audio mixer event, when one sink pad event is created, start the
      // mixer thread and send start stream event
      if (!this->audio_mixer_thrId && uPadType == MEDIA_CAPS_FOR_AUDIO){
        client = this;
        bSendEvent = TRUE;
        XGLOG_INFO ("Spawning audio mixer thread for client: %s",
                 client->m_detail.m_callId.c_str());
        if (EN_XGATE_STATUS_SUCCESS == spawn_audio_mixer ()){
          XGLOG_INFO ("Audio mixer thread successfully spawned ");
        }else {
          XGLOG_ERROR ("Failed to spawn audio mixer thread");
        }
      }
  #endif // USE_GST_AUDIO_MIXER_PLUGIN

     // send stream and segment event is sent inside add_audiomixer_sinkpad function
     // if gstream mixer is defined. so no need to send the events here
     if (TRUE == bSendEvent){
        HmpAudioEncoderBin *AudioEncoderBin = client->m_FrameChannel->m_pHmpAudioEncoderBin;
        GstElement    *element = AudioEncoderBin->m_opusenc_filter;
        GstPad        *sinkpad = NULL;
        sinkpad = gst_element_get_static_pad (element, "sink");
        sprintf (u8StreamId, "enc_filter_pad" );
        if (!AudioEncoderBin->send_stream_start_event (sinkpad,
                                     STREAM_START_EVENT_PAD, u8StreamId)){
          XGLOG_ERROR ("Failed to send stream start event");
        }
        if (!AudioEncoderBin->send_segment_event (sinkpad, SEGMENT_EVENT_TIME_PAD)){
          XGLOG_ERROR ("Failed to send segment start event");
        }
     }
#endif // AUDIO_MCU_ENABLED

      if (!frameRecvBin->send_stream_start_event (offerDetails->sinkPad,
                                   STREAM_START_EVENT_PAD, u8StreamId)) {
         XGLOG_ERROR ("Failed to send stream start event");
      }

      GstCaps *sinkPad_caps = gst_pad_query_caps (offerDetails->sinkPad,NULL);
      GstEvent    *caps_event = NULL;
      caps_event = gst_event_new_caps (sinkPad_caps);
      if (FALSE == gst_pad_send_event (offerDetails->sinkPad, caps_event)){
        XGLOG_ERROR ("Failed to send caps_event to sink pad");
      }
      if (!frameRecvBin->send_segment_event (offerDetails->sinkPad,
                                   SEGMENT_EVENT_TIME_PAD)) {
        XGLOG_ERROR ("Failed to send segment start event");
      }
      /* video blank screen issue is seen with firefox browser
       * some times if rtp packet is sent as soon as SDP update
       * response is sent. This issue is observed if rtp packets
       * are received before the SDP update response
       * Since SDP UPDATE reponse is sent to pbx and pbx sends to
       * browser.delay sending the rtp packets by 100 ms
       * */
      usleep (MICROSEC_SLEEP_400);
      offerDetails->bSendPkt = TRUE;
    }
    break;
#ifdef AUDIO_MCU_ENABLED
#ifdef USE_GST_AUDIO_MIXER_PLUGIN
    case XGATE_CLIENT_ADD_AUDIO_MIXER_PAD:
    {
      Client *src_client = static_cast <Client*> (pMsg->param1);
      HmpFrameBin *frameRecvBin = this->m_FrameChannel->m_pHmpFrameBin;
      XGLOG_INFO ("received ADD_AUDIO_MIXER_PAD this->call_id: %s  from src_client: %s",
                  this->m_detail.m_callId.c_str(), src_client->m_detail.m_callId.c_str());
      frameRecvBin->add_audiomixer_sinkpad(this, src_client, RANDOM_SSRC_ID );
    }
    break;
#endif
#endif
    default:
      XGLOG_ERROR ("Msg type: %d  not handled",pMsg->m_msgType);
    break;
  }
}

void Client::handle_client_delete_channel (xGateHmpClientMsgType eMsgType,
		                          char *callId_str,
		                          HmpFrameChannel *pHmpFrameChannel)
{
  Client *client = this;
  mlineofferDetail *mlineDetail = NULL;
  if(XGATE_CLIENT_DELETE_ALL_CHANNEL == eMsgType){
    XGLOG_INFO( "Stopping client with callId: %s ",client->m_detail.m_callId.c_str());
    #ifdef RECORD_ENABLED
    delete_recording_channel(client);
    #endif
    if(client->m_FrameChannel){
      XGLOG_INFO( "Stopping Frame channel for client with callId: %s ",client->m_detail.m_callId.c_str());
      client->m_FrameChannel->stop();
      delete client->m_FrameChannel;
      client->m_FrameChannel = NULL;
      for (unsigned int uiIndex = 0; uiIndex < client->m_mlineOffer_vector.size (); uiIndex++){
        mlineDetail = client->m_mlineOffer_vector.at(uiIndex);
        if (mlineDetail){
          XGLOG_INFO ("deleting mlineDetail: %p ", mlineDetail);
          delete mlineDetail;
        }
        mlineDetail = NULL;
      }
      if (!client->m_detail.m_screenshare_id.empty())
        client->m_detail.m_screenshare_id.clear();
      client->m_mlineOffer_vector.clear();
      if (client->m_mlineOffer_map.size ()){
        client->m_mlineOffer_map.clear();
      }
    }
    if (client->m_pHmpMRCPVoiceCaptureChannel){
       XGLOG_INFO("Stopping Voice capture channel");
       client->m_pHmpMRCPVoiceCaptureChannel->stop();
       delete client->m_pHmpMRCPVoiceCaptureChannel;
       client->m_pHmpMRCPVoiceCaptureChannel = NULL;
    }
    if (client->m_inhouseClient){
      XGLOG_INFO("calling delete->m_inhouseClient");
      delete client->m_inhouseClient;
      client->m_inhouseClient = NULL;
    }
  } else if (XGATE_CLIENT_DELETE_SEND_CHANNEL == eMsgType){
  #ifdef AUDIO_MCU_ENABLED
     HmpAudioEncoderBin *AudioEncoderBin = NULL;
     HmpFrameChannel  *pFrameChannel = client->m_FrameChannel;
     if (pFrameChannel)
        AudioEncoderBin = pFrameChannel->m_pHmpAudioEncoderBin;
     if (AudioEncoderBin){
#ifdef USE_GST_AUDIO_MIXER_PLUGIN
       XGLOG_INFO("client: %s Received DELETE_SEND_CHANNEL. Removing audiomixer sinkpad of client: %s",
               client->m_detail.m_callId.c_str(), callId_str);
       string delete_callID = callId_str;
       AudioEncoderBin->remove_audiomixer_sinkpad (client, delete_callID);
       delete_callID.clear();
#endif
       /* check the number of participants in the conference . if there is only this
        * only one participant other than dummy client then set the marker bit in
        * rtpopuspay element so that when a new client joins it will send the first
        * RTP packet with marker bit set
        */
       SFUController *pSFUController = xGateSFUtil::getSFUController();
       xGateHmpConference *pHmpConference =
              pSFUController->find_conference_entry(client->m_detail.m_mgresourceId);
       if(!pHmpConference){
          XGLOG_ERROR("Failed to find entry in conference");
        } else {
         #if 1
           /*Dummy client will always be there. so the decrement current size with 1 and compare*/
           if (1 == pHmpConference->m_clientMap.size () - 1){
              XGLOG_INFO ("Number of clients in conference: %s is 1 . configuring rtpopuspay maker to true",
                  client->m_detail.m_mgresourceId.c_str());
              g_object_set (G_OBJECT (AudioEncoderBin->m_opuspay), "marker", TRUE, NULL);
            }
         #endif
        }
      }
  #endif
    return;
    MLINE_OFFER_MAP::iterator itr_conf;
    XGLOG_INFO("Deleting the m-line and sending new offer client: %s deleted o_rtp_ep callId: %s ",
                client->m_detail.m_callId.c_str(), callId_str);
    itr_conf = client->m_mlineOffer_map.find(callId_str);
    if(itr_conf != m_mlineOffer_map.end()){
      XGLOG_INFO("Erasing m-line details present in client: %s deleted m-line callId: %s",
      client->m_detail.m_callId.c_str(), callId_str);
      mlineDetail = itr_conf->second;
      /* Remove the m-line in local SDP and configure the SDP in webrtc*/
      if (mlineDetail){
        XGLOG_INFO ("deleting mlineDetail: %p ", mlineDetail);
        delete mlineDetail;
        client->m_mlineOffer_map.erase(itr_conf);
      }else {
        XGLOG_ERROR ("Failed to find mlineDetails ..");
      }
    }
  }
}

#ifdef RECORD_ENABLED
void Client::delete_recording_channel(Client *pClient){
  string id = pClient->m_detail.m_callId;
  guint audio_ssrc = pClient->m_FrameChannel->m_pHmpFrameBin->retrieved_audiossrc;
  guint video_ssrc = pClient->m_FrameChannel->m_pHmpFrameBin->retrieved_videossrc;
  SFUController *pSFUController = xGateSFUtil::getSFUController();
  HmpRecordSendChannel *channel = pSFUController->m_hmpConf->get_record_send_channel(id);
  if(channel) {
    XGLOG_INFO("Stopping and deleting Recording channel for client: '%s'", id.c_str());
    delete channel;
    channel = nullptr;
  }

  for (auto itr = pSFUController->m_hmpConf->m_recorderMap.begin();
             itr != pSFUController->m_hmpConf->m_recorderMap.end(); itr++)
  {
     channel = itr->second;
    if( channel && itr != pSFUController->m_hmpConf->m_recorderMap.end() && (channel->m_clientId != id))
    {
     Client *client = pSFUController->m_hmpConf->get_participant_entry(channel->m_clientId);
      XGLOG_INFO("$$$$$ AUDIO_SSRC [%u] & VIDEO_SSEC [%u] of client [%s] removed from Recorder Enabled client [%s] \n", audio_ssrc, video_ssrc, id.c_str(), channel->m_clientId.c_str());
      if(client)
        client->handle_send_stop_ssrc_recording(audio_ssrc, video_ssrc, client);
    }
    XGLOG_INFO("$$$$$$$$ m_recorderMap size [%d] when handle_send_stop_ssrc_recording \n",
              pSFUController->m_hmpConf->m_recorderMap.size());
  }

}

void Client::handle_send_stop_ssrc_recording(guint audio_ssrc, guint video_ssrc, Client *client)
{
  xGateMgMsgType eMsgType = EN_XGATE_MG_TYPE_UNKNOWN;
  SFUController *pSFUController = xGateSFUtil::getSFUController();
  MgMediaDetail sMediaDetail;
  if (pSFUController){
    sMediaDetail.mgresource_id = client->m_detail.m_mgresourceId;
    sMediaDetail.call_type = EN_XGATE_CALL_TYPE_CONFERENCE_VIDEO;
    sMediaDetail.joiner_type = client->m_detail.m_joinerType;
    sMediaDetail.call_id = client->m_detail.m_callId;
    sMediaDetail.media_event = EN_MEDIA_EVENT_RECORD_SSRC_REMOVE;
    sMediaDetail.retrieved_audiossrc = audio_ssrc;
    sMediaDetail.retrieved_videossrc = video_ssrc;

    eMsgType = EN_XGATE_MG_VIDEO_REC_REQ;
    XGLOG_INFO("Sending Recording EN_MEDIA_EVENT_RECORD_SSRC_REMOVE msg to client: '%s' ",
               client->m_detail.m_callId.c_str());
    pSFUController->send_msg_sfu_client(&sMediaDetail, client->m_detail, eMsgType);
  }
}

void Client::handle_send_stop_recording(Client *client, HmpRecordSendChannel *channel)
{
  xGateMgMsgType eMsgType = EN_XGATE_MG_TYPE_UNKNOWN;
  SFUController *pSFUController = xGateSFUtil::getSFUController();
  MgMediaDetail sMediaDetail;
  if (pSFUController){
    sMediaDetail.mgresource_id = client->m_detail.m_mgresourceId;
    sMediaDetail.call_type = EN_XGATE_CALL_TYPE_CONFERENCE_VIDEO;
    sMediaDetail.joiner_type = client->m_detail.m_joinerType;
    sMediaDetail.call_id = client->m_detail.m_callId;
    sMediaDetail.media_event = EN_MEDIA_EVENT_RECORD_STOP;
    sMediaDetail.record_url = client->m_detail.m_record_url;
    sMediaDetail.meeting_name = client->m_detail.m_meeting_name;
    sMediaDetail.record_file = client->m_detail.m_record_file;
    sMediaDetail.sip_id = client->m_detail.m_sip_id;
    sMediaDetail.meetuser_info.assign(client->m_detail.meet_userinfo.begin(),
                                     client->m_detail.meet_userinfo.end());

    ACE_Time_Value tval ((time(NULL) ));
    xGateHmpClientThreadMsg *pMsg = NULL;
    XGLOG_INFO ("Sending EN_MEDIA_EVENT_RECORD_STOP msg to client: %s",
                      client->m_detail.m_callId.c_str());
    xGateMediaEvent *pEvent = new xGateMediaEvent;
    if (!pEvent){
      XGLOG_ERROR ("Failed to allocate memory to send event");
      return;
    }
    *pEvent = EN_MEDIA_EVENT_RECORD_STOP;

    pMsg = new xGateHmpClientThreadMsg ();
    tval += ACE_Time_Value(0,1);
    pMsg->param1 = (void *) client;
    pMsg->param2 = (void * )pEvent;
    pMsg->param3 = (void * )channel;
    pMsg->m_msgType = XGATE_CLIENT_REC_EVENT_REQUEST;

    if (client->rcv_data_thrId){
      if (client->rcv_data_msg_q != NULL && !client->rcv_data_msg_q->enqueue_prio(pMsg, &tval)){
         XGLOG_ERROR("Failed to push Stop Video Record  to Client rcv Data thread");
      }
    } else if (pSFUController->m_ThreadPool_info.m_event_pool) {
      pMsg->pClient = client;
      GError **error = nullptr;
      g_thread_pool_push (pSFUController->m_ThreadPool_info.m_rx_data_pool, pMsg, error);
      if (error != nullptr){
        XGLOG_ERROR ("Failed to push event to thread pool");
      }
    }
    eMsgType = EN_XGATE_MG_VIDEO_REC_REQ;
    XGLOG_INFO("Sending Message STOP to recording Server client: '%s' and meetuserinfo '%p'",
                      client->m_detail.m_callId.c_str(),sMediaDetail.meetuser_info);
    pSFUController->send_msg_sfu_client(&sMediaDetail, client->m_detail, eMsgType);
  }
}
#endif

void Client::set_thread_state (gboolean run_state)
{
  m_run = run_state;
}

string Client::form_channel_name(string name)
{
  string channelName = "";
  channelName += name;
  channelName += "_";
  channelName += m_detail.m_callId;
  channelName += "_";
  channelName += m_detail.audioDetail.m_codec;
  return channelName;
}

gboolean Client::create_frame_channel(guint uiCount)
{
  string channelName = "", binName = "";
  HmpChannelType channelType = EN_PIPELINE_FRAME_RECV;
  HmpVideoBinType vbinType = EN_BIN_VID_DTLS_RECV;
  gboolean retVal = FALSE;
  if(!m_FrameChannel) {
    string webrtc_name = "webrtc_";
    webrtc_name.insert (strlen(webrtc_name.c_str()), m_detail.m_callId, 0, 12);
    channelName = webrtc_name;
    channelName += "pipeline";
    binName = webrtc_name;
    binName += "bin";
    webrtc_name += to_string (uiCount);
    m_FrameChannel = new HmpFrameChannel(channelName, channelType);
    retVal = m_FrameChannel->construct_bin(binName, vbinType, m_detail.videoDetail.m_codec, \
      m_detail.videoDetail.m_dynamicCodec, m_detail.m_sdpStr,
      m_detail.m_callDir, EN_WEBRTC_TYPE_SENDRECV,webrtc_name, this);
    if(retVal == FALSE) {
      XGLOG_ERROR( "create_frame_channel, failed for codec:%d and client:%s:%d !!", \
          m_detail.videoDetail.m_codec,
          m_detail.videoDetail.m_clientIp.c_str(), m_detail.videoDetail.m_clientPort);
      retVal = FALSE;
    } else {
      /*on successfull channel creation register the callbacks */
      m_FrameChannel->register_webrtcbin_callback(this);
      m_FrameChannel->register_handle_event_callback(this);
      retVal = m_FrameChannel->start();

      XGLOG_INFO( "create_frame_channel for WebRTC %s codec:%d and client_ip:%s:%d success", \
      webrtc_name.c_str(), m_detail.videoDetail.m_codec,
      m_detail.videoDetail.m_clientIp.c_str(), m_detail.videoDetail.m_clientPort);
    }
    webrtc_name.clear ();
    channelName.clear ();
    binName.clear ();
    retVal = TRUE;
  } else {
    XGLOG_ERROR( "create_frame_channel for codec:%d and client:%s:%d already been created!!", \
        m_detail.videoDetail.m_codec, m_detail.videoDetail.m_clientIp.c_str(),
        m_detail.videoDetail.m_clientPort);
    retVal = FALSE;
  }
  return retVal;
}

gboolean Client::create_voice_capture_channel(){
  string channelName(""),binName("");
  HmpChannelType channelType = EN_PIPELINE_VOICE_CAPTURE;
  HmpBinType binType = EN_BIN_UNKNOWN;
  HmpCodecType codecType = EN_CODEC_UNKNOWN;
  bool ret = FALSE;
  gchar binname[100] = {0};
  GstEvent    *pStreamEvent = NULL;
  GstEvent    *caps_event = NULL;
  gchar u8StreamId [100] = {'\0'};
  HmpMRCPVoiceCaptureBin     *pMRCPVoiceBin = NULL;
  GstElement *element = NULL;

  codecType = EN_CODEC_OPUS;
  m_detail.audioDetail.m_codec = codecType;

  if(codecType == EN_CODEC_OPUS) {
    binType = EN_BIN_OPUS_RECV;
  }

  channelName = form_channel_name("VoiceCaptureChannel");
  sprintf(binname, "%s_%s_%d","VoiceCaptureBin",
                  m_detail.m_callId.c_str(), m_detail.audioDetail.m_codec);
  binName = binname;
  if(!m_pHmpMRCPVoiceCaptureChannel){
    m_pHmpMRCPVoiceCaptureChannel = new HmpMRCPVoiceCaptureChannel(channelName,channelType);
    if(!m_pHmpMRCPVoiceCaptureChannel->construct_bin(binName,binType,m_asrType,m_audioPayload)){
      XGLOG_ERROR("create_voice_transcode_channel for client:%s:%d failed!!", \
                m_detail.audioDetail.m_dialIp, m_detail.audioDetail.m_dialPort);
      return FALSE;
    }
    XGLOG_INFO("Register_handle_event_callback for MRCPVoiceCaptureChannel");
    m_pHmpMRCPVoiceCaptureChannel->register_handle_event_callback(this);
    if (m_pHmpMRCPVoiceCaptureChannel->m_pHmpMRCPVoiceCaptureBin)
#ifndef AUDIO_MCU_ENABLED
      m_pHmpMRCPVoiceCaptureChannel->register_buffer_probe_callBack (this,
                      m_pHmpMRCPVoiceCaptureChannel->m_pHmpMRCPVoiceCaptureBin->m_filter);
#else
      m_pHmpMRCPVoiceCaptureChannel->register_buffer_probe_callBack (this,
                m_pHmpMRCPVoiceCaptureChannel->m_pHmpMRCPVoiceCaptureBin->m_mrcp_filter);
#endif

    m_pHmpMRCPVoiceCaptureChannel->start();

    //Sending stream start event to mrcp sink pad
    if (m_pHmpMRCPVoiceCaptureChannel)
      pMRCPVoiceBin = m_pHmpMRCPVoiceCaptureChannel->m_pHmpMRCPVoiceCaptureBin;

    element = pMRCPVoiceBin->m_mrcp_filter;

    GstPad* sink_pad = gst_element_get_static_pad(element, "sink");
    sprintf(u8StreamId, "m_mrcp_filter_%s",gst_pad_get_name(sink_pad));

    if (!m_FrameChannel->m_pHmpFrameBin->send_stream_start_event (sink_pad, STREAM_START_EVENT_PAD, u8StreamId)) {
       XGLOG_ERROR ("Failed to send stream start event");
    }

    GstCaps *sinkPad_caps = gst_pad_query_caps (sink_pad, NULL);
    caps_event = gst_event_new_caps (sinkPad_caps);

    if (FALSE == gst_pad_send_event (sink_pad, caps_event)){
      XGLOG_ERROR ("Failed to send caps_event to sink pad");
    }
    if (!m_FrameChannel->m_pHmpFrameBin->send_segment_event (sink_pad, SEGMENT_EVENT_TIME_PAD)){
      XGLOG_ERROR ("Failed to send segment start event");
    }
  }
  else{
    XGLOG_ERROR("create_voice_capture_channel for client:already been created!!!");
    return FALSE;
  }
  XGLOG_INFO("create_voice_capture_channel for client");
  return TRUE;
}

gboolean Client::handle_sdp_update_request (PARTICIPANT_LIST *client_list, string remote_sdp)
{
  Client *pClient = this;
  HmpFrameChannel *pFrameChannel = pClient->m_FrameChannel;
  HmpFrameBin *pFrameBin = pFrameChannel->m_pHmpFrameBin;
  XGLOG_INFO ("handle_sdp_update_request () rcvd for client: %s Participants: %d num_mline: %d mline_map: %d sdp: %s",
                        pClient->m_detail.m_callId.c_str(), client_list->size(),
                        pClient->m_mlineOffer_vector.size(), pClient->m_mlineOffer_map.size(),
                        remote_sdp.c_str());
  /*
   * 1. Check if any participants are deleted and make transceivers inactive
   * 2. Check if new participants are added and if so, add transceivers
   * 3. Generate answer
   */
  pFrameBin->Check_Inactivate_mline (pClient, client_list, remote_sdp);

  pFrameBin->generate_mline_sdp_update_request (pClient, client_list, remote_sdp);

  pFrameBin->configure_sdp_to_generate_offerOrAnswer(pClient, pFrameChannel,
                                              remote_sdp, pClient->m_detail.m_callId,
                                              CONFIGURE_REMOTE_DESCRIPTION);

  pFrameBin->configure_sdp_to_generate_offerOrAnswer(pClient, pFrameChannel,
                                              remote_sdp, pClient->m_detail.m_callId,
                                              GENERATE_ANSWER);
  return TRUE;
}

gboolean Client::handle_screenshare_request(PARTICIPANT_LIST *client_list)
{
  Client *pClient = this;
  string call_id;
  mlineofferDetail *mlineDetail = NULL;
  ParticipantInfo sPartInfo;
  Client *dst_client = nullptr;
  xGateHmpConference *pHmpConference = NULL;
  int index = 0;

  XGLOG_INFO ("handle_screenshare_request rcvd participant:%s with clientlist_size: %d mline_vector: %d mline_map: %d ",
     pClient->m_detail.m_callId.c_str(),client_list->size(),
     pClient->m_mlineOffer_vector.size(), pClient->m_mlineOffer_map.size());

  index = client_list->size();
  if(index != 1){
    XGLOG_INFO ("Screen_share Request failed for call_Id %s, participant list is %d not equal to 1",
                pClient->m_detail.m_callId.c_str(),index);
    return FALSE;
  }
  sPartInfo = client_list->at(0);
  SFUController *pSFUController = xGateSFUtil::getSFUController();
  pHmpConference = pSFUController->find_conference_entry(pClient->m_detail.m_callId);

  if(!pHmpConference){
    XGLOG_ERROR("Failed to find entry in conference");
    return EN_XGATE_STATUS_ERROR;
  }

  for (auto itr = pHmpConference->m_clientMap.begin ();
                itr != pHmpConference->m_clientMap.end (); ++itr){
    dst_client = itr->second;
    if (dst_client){
     // copy the call_id of the screen share client in all other client's object
      if (!dst_client->m_mid3CallId.empty())
         dst_client->m_mid3CallId.erase();
         dst_client->m_mid3CallId = sPartInfo.call_id;
         dst_client->m_mid3CallType = XGATE_MID3_CALL_TYPE_SCREENSHARE;
      }
  }

  XGLOG_INFO ("ScreenShare Request for m_mid3CallId with: %s for participant: %s",
              sPartInfo.call_id.c_str(),pClient->m_detail.m_callId.c_str());

  return TRUE;
}

gboolean Client::handle_active_speaker_request(PARTICIPANT_LIST *client_list)
{
  Client *pClient = this;
  Client *client = this;
  SFUController *pHmpGstManager = NULL;
  xGateHmpConference *pxGateHmpConfObj = NULL;
  gboolean bSendMsg = TRUE;
  XGLOG_INFO("handle_active_speaker_request rcvd participant: %s mline_vector: %d mline_map: %d ",
    pClient->m_detail.m_callId.c_str(),
    pClient->m_mlineOffer_vector.size(), pClient->m_mlineOffer_map.size());

  pHmpGstManager = xGateSFUtil::getSFUController();

  if(pHmpGstManager)
    pxGateHmpConfObj = pHmpGstManager->find_conference_entry(pClient->m_detail.m_mgresourceId);
  if(!pxGateHmpConfObj){
    XGLOG_ERROR("Failed to find entry in conference");
    return FALSE;
  }
  if (!pClient->m_mid3CallId.empty())
    pClient->m_mid3CallId.erase ();

  /*
   * check if screen share is active and if so, give preference
   * to the screen share id. Assign screen share Id as mid3CallId
   */
  Client *dst_client = nullptr;
  for (auto itr = pxGateHmpConfObj->m_clientMap.begin ();
             itr != pxGateHmpConfObj->m_clientMap.end (); ++itr){

    dst_client = itr->second;
    if (dst_client){
      // copy the call_id of the screen share client in all other client's object
        if ((dst_client->m_detail.m_joinerType == EN_JOINER_TYPE_SCREENSHARE)
            && (dst_client != pClient)){
          pClient->m_mid3CallId = dst_client->m_detail.m_callId;
          pClient->m_mid3CallType = XGATE_MID3_CALL_TYPE_SCREENSHARE;
             XGLOG_INFO ("Screen share is started ... Preference to Screen for client: %s  mid3callID : %s curr_view_type: %u",
              pClient->m_detail.m_callId.c_str(),
              pClient->m_mid3CallId.c_str(), pClient->m_viewType);
          /* if previous view type is grid view, then send key frame request to
           * the screen share client and assign the view type as active speaker
           */
          if (pClient->m_viewType == XGATE_VIEW_TYPE_GRID){
            HmpFrameBin *pframeRecvBin = nullptr;
            XGLOG_INFO ("Previous ViewType is Grid ... sending key frame req to screen share client: %s",
                     dst_client->m_detail.m_callId.c_str());
            if (dst_client->m_FrameChannel)
               pframeRecvBin = dst_client->m_FrameChannel->m_pHmpFrameBin;
            if(pframeRecvBin){
              pframeRecvBin->send_key_frame_request_to_mbc (pClient, dst_client->m_detail.m_callId);
            }
            pClient->m_viewType = XGATE_VIEW_TYPE_ACTIVE_SPEAKER;
          }
          return TRUE;
       }
    }
  }
  pClient->m_viewType = XGATE_VIEW_TYPE_ACTIVE_SPEAKER;

  if (pxGateHmpConfObj->m_activeSpeakerId.empty())
  {
    XGLOG_WARN ("Active speakerId is empty.. Assigning the last connected participant");
    pxGateHmpConfObj->fetch_last_connected_entry ((void*)pxGateHmpConfObj);
    bSendMsg = TRUE;
  }

  if (pClient->m_detail.m_callId != pxGateHmpConfObj->m_activeSpeakerId) {
     pClient->m_mid3CallId.erase();
     pClient->m_mid3CallId = pxGateHmpConfObj->m_activeSpeakerId;
   }
   else if (pClient->m_detail.m_callId != pxGateHmpConfObj->m_Prev_activeSpeakerId){
     pClient->m_mid3CallId.erase();
     pClient->m_mid3CallId = pxGateHmpConfObj->m_Prev_activeSpeakerId;
   }else{
       XGLOG_INFO ("Fetching connected entry other than ActiveSpkr");
       for(auto itr = pxGateHmpConfObj->m_clientMap.begin();
           itr != pxGateHmpConfObj->m_clientMap.end();++itr){
         client = itr->second;
         if (client){
           if ((0 != strcmp (client->m_detail.m_callId.c_str(), DUMMY_CALLID))
              && (client->m_detail.m_callId != pxGateHmpConfObj->m_activeSpeakerId)
              && (client->m_detail.m_callId != pxGateHmpConfObj->m_Prev_activeSpeakerId)){

             pClient->m_mid3CallId.erase ();
             pClient->m_mid3CallId = client->m_detail.m_callId ;
             XGLOG_INFO ("Found other than Active or prev active spkr mid3_callId: %s for client: %s",
                      pClient->m_mid3CallId.c_str(),
                      pClient->m_detail.m_callId.c_str());
             break;
           }
        }
      }
   }
   pClient->m_mid3CallType = XGATE_MID3_CALL_TYPE_ACTIVE_SPEAKER;
   XGLOG_INFO("client: %s mid3CallId: %s viewType: %u", pClient->m_detail.m_callId.c_str(),
                            pClient->m_mid3CallId.c_str(), pClient->m_viewType);
   if(TRUE == bSendMsg){
      MgMediaDetail mediaDetail;
      SFUController *pSFUController = xGateSFUtil::getSFUController();
      mediaDetail.mgresource_id = pxGateHmpConfObj->m_conferenceId;
      mediaDetail.joiner_type = pClient->m_detail.m_joinerType;
      mediaDetail.call_type = EN_XGATE_CALL_TYPE_CONFERENCE_VIDEO;
      mediaDetail.call_id = pxGateHmpConfObj->m_activeSpeakerId;
      if (pSFUController){
        XGLOG_INFO( "Sending Active speaker callID: %s to PBX ", mediaDetail.call_id.c_str());
        pSFUController->send_msg_sfu_client(&mediaDetail, pClient->m_detail,
                                  EN_XGATE_MG_ACTIVE_SPEAKER_INFO);
      }
  }
  return TRUE;
}

gboolean Client::handle_tile_change_request(PARTICIPANT_LIST *client_list)
{
  Client *pClient = this;
  MLINE_OFFER_MAP::iterator itr;
  mlineofferDetail *mlineDetail = NULL;
  mlineofferDetail *new_mlineDetail = NULL;
  ParticipantInfo sPartInfo;
  PARTICIPANT_LIST sClientNotInList;
  MLINE_OFFER_MAP::iterator it;
  int index=0;
  int vector_index=0;
  int empty_count = 1;
  int list_size = 0;
  string call_id;

  XGLOG_INFO ("handle_tile_change_request rcvd participant:%s with clientlist_size: %d mline_vector: %d mline_map: %d ",
      pClient->m_detail.m_callId.c_str(),client_list->size(),
      pClient->m_mlineOffer_vector.size(), pClient->m_mlineOffer_map.size());

  if(client_list->size() < 1){
    XGLOG_ERROR ("Tile_change Request failed for call_Id %s, participant list is 0",
                           pClient->m_detail.m_callId.c_str());
    return FALSE;
  }

  if(pClient->m_mlineOffer_vector.size() < client_list->size()){
    XGLOG_ERROR ("Tile_change Request failed... particiapant list is >  mlines generated for client %s",
                                            pClient->m_detail.m_callId.c_str());
    return FALSE;
  }

  if(pClient->m_mlineOffer_vector.size() != pClient->m_mlineOffer_map.size()){
    XGLOG_INFO (" Tile Change request failed client mline map is not equal to mline vector of client:%s",
                                             pClient->m_detail.m_callId.c_str());
    return FALSE;
  }
  pClient->m_mlineOffer_tile_map.clear();
  index = 0;
  vector_index = 0;
  for(itr = pClient->m_mlineOffer_map.begin(); itr != pClient->m_mlineOffer_map.end(); itr++){
    mlineDetail = pClient->m_mlineOffer_vector.at(vector_index);
    if ((0 == strcmp (mlineDetail->callId_offer_generated.c_str () , "audio_only"))
       || (0 == strcmp (mlineDetail->callId_offer_generated.c_str () , "screenshare"))) {
      call_id = mlineDetail->callId_offer_generated;
      pClient->m_mlineOffer_tile_map.insert({call_id,mlineDetail});
      XGLOG_INFO ("Inserting in Tile map at vector_index [%d] ==> callId: %s",
                                vector_index,call_id.c_str());
      vector_index++;
      continue;
    }
    if(index >= client_list->size()){
      call_id = EMPTY_DUMMY_CALLID + to_string(empty_count);
      empty_count++;
    }else{
      sPartInfo = client_list->at(index);
      call_id = sPartInfo.call_id;
      mlineDetail->videoOfferDetails.bTileChanged = TRUE;
    }
    XGLOG_INFO ("Replacing @vector_index: %d with call_id: %s",
                   vector_index,call_id.c_str());
    mlineDetail->callId_offer_generated.clear();
    mlineDetail->callId_offer_generated = call_id;
    mlineDetail->videoOfferDetails.bSendPkt = TRUE;
    pClient->m_mlineOffer_vector.erase(m_mlineOffer_vector.begin()+ vector_index);
    pClient->m_mlineOffer_vector.emplace(pClient->m_mlineOffer_vector.begin() + vector_index, mlineDetail);
    pClient->m_mlineOffer_tile_map.insert({call_id,mlineDetail});
    index++;
    vector_index++;
  }
  std::swap(pClient->m_mlineOffer_map,pClient->m_mlineOffer_tile_map);
  XGLOG_INFO ("Displaying mline Vector info after tile change ===");
  mlineofferDetail  *pTmpmline = NULL;
  for (guint32 uiIndex = 0 ; uiIndex < pClient->m_mlineOffer_vector.size(); uiIndex++){
    pTmpmline = pClient->m_mlineOffer_vector.at (uiIndex);
    XGLOG_INFO ("pTmpmline->callId_offer_generated: %s ",
                  pTmpmline->callId_offer_generated.c_str());
  }
  return TRUE;
}


#ifdef RECORD_ENABLED
HmpRecordSendChannel* Client::get_client_record_channel()
{
  //get the proper recorder channel for this client from HmpConference recorder map
  string id = m_detail.m_callId;
  SFUController *pSFUController = xGateSFUtil::getSFUController();
  HmpRecordSendChannel *channel = pSFUController->m_hmpConf->get_record_send_channel(id);

  if(!channel) {
    XGLOG_ERROR( "recorder channel not found for client id: %s", id.c_str());
    return NULL;
  }
  return channel;
}

gboolean Client::handle_recording_request(xGateMediaEvent recEvent)
{
  Client *pClient = this;
  string id = m_detail.m_callId;
  SFUController *pSFUController = xGateSFUtil::getSFUController();
  HmpRecordSendChannel *channel = pSFUController->m_hmpConf->get_record_send_channel(id);

  if(!channel && (recEvent != EN_MEDIA_EVENT_RECORD_START)) {
    XGLOG_INFO("handle rec event req for client '%s' failed. RecordSendChannel not found !", id.c_str());
    return FALSE;
  }

  switch(recEvent) {
    case EN_MEDIA_EVENT_RECORD_START:
      {
        HmpRecordSendChannel *channel = NULL;
        XGLOG_INFO("handle rec req for client:%s with event EN_MEDIA_EVENT_RECORD_START", id.c_str());
        channel = create_record_send_channel();
        if(!channel){
          XGLOG_INFO("Failed to handle record request:%d for callID:%s create record send channel failed",
                                                          recEvent,pClient->m_detail.m_callId.c_str());
          break;
        }
        if(!channel->construct_record_send_channel(pClient)){
          XGLOG_INFO("construct recorder audio/video send channel for client '%s' failed !", id.c_str());
          return FALSE;
        }
        pSFUController->m_hmpConf->insert_record_send_channel(m_detail.m_callId, channel);
        channel->handle_send_recording_msg(pClient);
        break;
      }
    case EN_MEDIA_EVENT_RECORD_PAUSE:
      {
        XGLOG_INFO("handle rec req for client:%s with event EN_MEDIA_EVENT_RECORD_PAUSE", id.c_str());
        if(channel->m_recState == RECORD_STATE_START){
          channel->stop_record();
          channel->handle_send_recording_msg(pClient);
          channel->m_recState = RECORD_STATE_PAUSE;
        }else{
          XGLOG_INFO("handle recored pause event for client:%s failed record not started", id.c_str());
        }
        break;
      }
    case EN_MEDIA_EVENT_RECORD_RESUME:
      {
        XGLOG_INFO("handle rec req for client:%s with event EN_MEDIA_EVENT_RECORD_RESUME", id.c_str());
        if(channel->m_recState == RECORD_STATE_PAUSE){
          channel->handle_send_recording_msg(pClient);
        }else{
          XGLOG_INFO("handle recored resume event for client:%s failed record not in PAUSE state", id.c_str());
        }
        break;
      }
    case EN_MEDIA_EVENT_RECORD_STOP:
      {
        XGLOG_INFO("handle rec req for client:%s with event EN_MEDIA_EVENT_RECORD_STOP", id.c_str());
        break;
      }
    case EN_MEDIA_EVENT_NULL:
    default:
      {
        XGLOG_INFO("handle rec req for client:%s with EN_MEDIA_EVENT_NULL", id.c_str());
        break;
      }
  }
  return TRUE;
}

gboolean Client::handle_recording_response(xGateMediaEvent recEvent)
{
  Client *pClient = this;
  string id = m_detail.m_callId;
  SFUController *pSFUController = xGateSFUtil::getSFUController();
  HmpRecordSendChannel *channel = pSFUController->m_hmpConf->get_record_send_channel(id);

  if(!channel) {
    XGLOG_INFO("handle rec event res for client '%s' failed. RecordSendChannel not found !", id.c_str());
    return FALSE;
  }

  XGLOG_INFO("handle rec event res for client:%s ",pClient->m_detail.m_callId.c_str());
  switch(recEvent){
    case EN_MEDIA_EVENT_RECORD_START:
      {
        XGLOG_INFO("handle rec event res for client:%s with EN_MEDIA_EVENT_RECORD_START",pClient->m_detail.m_callId.c_str());
        channel->handle_record_start_res(pClient);
        break;
      }
    case EN_MEDIA_EVENT_RECORD_PAUSE:
      {
        XGLOG_INFO("handle rec event res for client:%s with EN_MEDIA_EVENT_RECORD_PAUSE",pClient->m_detail.m_callId.c_str());
        break;
      }
    case EN_MEDIA_EVENT_RECORD_RESUME:
      {
        XGLOG_INFO("handle rec event res for client:%s with EN_MEDIA_EVENT_RECORD_RESUME",pClient->m_detail.m_callId.c_str());
        channel->handle_record_resume_res(pClient);
        break;
      }
    case EN_MEDIA_EVENT_RECORD_STOP:
      {
        XGLOG_INFO("handle rec event res for client:%s with EN_MEDIA_EVENT_RECORD_STOP",pClient->m_detail.m_callId.c_str());
        break;
      }
    case EN_MEDIA_EVENT_NULL:
    default:
      {
        XGLOG_INFO("handle rec event res for client:%s with EN_MEDIA_EVENT_NULL",pClient->m_detail.m_callId.c_str());
        break;
      }
  }
  return TRUE;
}

HmpRecordSendChannel *Client::create_record_send_channel()
{
  Client *pClient = this;
  string id = pClient->m_detail.m_callId;
  SFUController *pSFUController = xGateSFUtil::getSFUController();
  HmpRecordSendChannel *channel = pSFUController->m_hmpConf->get_record_send_channel(id);

  if(!channel) {
    //creating first instance of recorder send channel for this client
    channel = new HmpRecordSendChannel();
    channel->m_clientId = m_detail.m_callId;
    return channel;
  } else {
    XGLOG_ERROR( "record_send_channel for client:%s already been created !",
                 m_detail.m_callId.c_str());
  }
  return nullptr;
}

#endif
  /*xGateHmpConference class implementation - Start
  */
xGateHmpConference::xGateHmpConference(string mconferenceId, xGateCallType confType) :
  m_conferenceId(mconferenceId), m_confType(confType),m_activeSpeakerId("")
  {
    m_activeSpeakerId="";
    m_isHost = FALSE;
    m_hostJoined = FALSE;
#ifdef RECORD_ENABLED
    m_recorder_audio_queue = g_async_queue_new_full((GDestroyNotify)free_recorder_audio_queue);
    m_recorder_video_queue = g_async_queue_new_full((GDestroyNotify)free_recorder_video_queue);
#endif
    //spawn one thread to detect active speaker
    if (EN_XGATE_STATUS_SUCCESS != spawn_active_spkr_detect ()){
       XGLOG_ERROR ("Failed to spawn Active speaker detect thread");
    } else{
       XGLOG_INFO ("Spawned active detect thread successfully");
       m_StopConference = FALSE;
    }
  }

xGateHmpConference::~xGateHmpConference()
{
  remove_participant_entries();
}
xGateRetVal xGateHmpConference::create_channel(ClientDetail &clientDetail)
{
  HmpFrameBin *pHmpFrameBin = NULL;
  XGLOG_INFO( "xGateHmpConference::create_channel for m_mgresourceId: %s call-Id %s ", \
      clientDetail.m_mgresourceId.c_str(), clientDetail.m_callId.c_str());
  Client *client = NULL;
  switch(m_confType){
   case EN_XGATE_CALL_TYPE_CONFERENCE_VIDEO:
    {
      const char *thr_name = "client";
      SFUController *pSFUController = xGateSFUtil::getSFUController();
      xGateHmpConference *pHmpConference = NULL;
      if (!clientDetail.m_callId.c_str() || !clientDetail.m_mgresourceId.c_str()){
        XGLOG_ERROR("Client details are invalid calliD: %s or resource Id: %s is null",
                     clientDetail.m_callId.c_str(), clientDetail.m_mgresourceId.c_str());
        return EN_XGATE_STATUS_ERROR;
      }
      pHmpConference = pSFUController->find_conference_entry(clientDetail.m_mgresourceId);
      if(!pHmpConference){
        XGLOG_ERROR("Failed to find entry in conference");
        return EN_XGATE_STATUS_ERROR;
      }

      client = new Client(clientDetail);
      if(!client){
        XGLOG_ERROR("Failed to allocate memory for new client object");
        return EN_XGATE_STATUS_ERROR;
      }
      client->set_thread_state (TRUE);
      client->m_StopSendRecvData = FALSE;
      client->rcv_audio_data_thrId = 0;
      client->rcv_video_data_thrId = 0;
      client->rcv_data_thrId = 0;
      client->send_audio_data_thrId = 0;
      client->send_video_data_thrId = 0;
      client->send_data_thrId = 0;
      client->m_data_rcv_thr_stop = FALSE;
      client->send_audio_msg_q = NULL;
      client->send_video_msg_q = NULL;
      client->send_data_msg_q = NULL;
      client->rcv_data_msg_q = NULL;
#ifndef USE_GST_AUDIO_MIXER_PLUGIN
      client->audio_mixer_thrId = 0;
      client->mixer_msg_q = NULL;
#endif
      client->m_last_index = 0;
      g_mutex_init (&client->m_data_process_lock);
      g_mutex_init (&client->m_audio_decode_process_lock);
      g_mutex_init (&client->m_audio_encode_process_lock);
      client->ullNextAudioMsgNum = 0;
      client->ullNextVideoMsgNum = 0;
      client->ullLastProcessedAudioMsgNum = 0;
      client->ullLastProcessedVideoMsgNum = 0;
      client->ullLastAudioDecodedMsgNum = 0;
      client->ullNextAudioDecodedMsgNum = 0;
      client->ullLastAudioEncodeMsgNum = 0;
      client->ullNextAudioEncodeMsgNum = 0;
      client->m_viewType = XGATE_VIEW_TYPE_ACTIVE_SPEAKER;
    #ifdef AUDIO_MIXER_FILE_DUMP
      client->fp = NULL;
      client->in_fp1 = NULL;
      client->in_fp2 = NULL;
      client->dec_fp1 = NULL;
      client->dec_fp2 = NULL;
    #endif
      for (guint32 index = 0; index < MAX_NUM_SAMPLES; index++){
        client->m_sample_info[index].rms_dB = SILENCE_SAMPLE_VALUE_DB;
        client->m_sample_info[index].peak_dB = SILENCE_SAMPLE_VALUE_DB;
        client->m_sample_info[index].decay_dB = SILENCE_SAMPLE_VALUE_DB;
      }
      if (client->m_mlineOffer_map.size()){
        client->m_mlineOffer_map.clear();
      }
      if (client->m_mlineOffer_vector.size()){
        client->m_mlineOffer_vector.clear();
      }
      XGLOG_INFO("Created new client for conf client %p callId: %s mg_resource_id: %s",
              client,client->m_detail.m_callId.c_str(), clientDetail.m_mgresourceId.c_str());
      // post the messages in thread pool which process the messages
      xGateHmpClientThreadMsg *pMsg = new xGateHmpClientThreadMsg;
      pMsg->m_msgType = XGATE_CLIENT_CREATE_CHANNEL;
      pMsg->param1 = malloc (sizeof(xGateMgMsgType));
      *((xGateMgMsgType *)pMsg->param1) = EN_XGATE_MG_MEDIA_CREATE;
      pMsg->param2 = static_cast <void*> (client);
      pMsg->param3 = malloc (sizeof(guint));
      *((guint *)pMsg->param3) = pHmpConference->m_webrtcCount;
      pMsg->pClient = client;
      if (pSFUController->m_ThreadPool_info.m_event_pool) {
        GError **error = nullptr;
        g_thread_pool_push (pSFUController->m_ThreadPool_info.m_event_pool, pMsg, error);
        if (error != nullptr){
          XGLOG_ERROR ("Failed to push event to thread pool");
        }
      }
      else{
        XGLOG_ERROR ("event pool thread is null... can't push event");
      }
      pHmpConference->m_webrtcCount++;

      // if number of participants earlier to this new client is one,
      // update the MarkerSet flag of the existing participant so that
      // when the older participant sets the marker bit for the first
      // audio packet to the newly joined client
      if (1 == pHmpConference->m_clientMap.size () - 1 ){
        Client *dst_client = nullptr;
        for (auto itr = pHmpConference->m_clientMap.begin ();
                itr != pHmpConference->m_clientMap.end (); ++itr){

          dst_client = itr->second;
          if (dst_client && (dst_client->m_detail.m_callId != DUMMY_CALLID)
                  && (nullptr != dst_client->m_FrameChannel)){
            HmpAudioEncoderBin *AudioEncoderBin = dst_client->m_FrameChannel->m_pHmpAudioEncoderBin;
            if (AudioEncoderBin) {
              AudioEncoderBin->bSetMarkbit = TRUE;
            }
          }
        }
      }
      pHmpConference->insert_participant_entry(clientDetail.m_callId, client);
      /* check if the new client connected is a screen share client
       * if it is then extract the original client and send message to update
       * screen share id
       */
      if (EN_JOINER_TYPE_SCREENSHARE == clientDetail.m_joinerType){
        Client *org_client = NULL;
        org_client = pHmpConference->get_participant_entry(clientDetail.m_screenshare_id);
        if(!org_client){
           XGLOG_ERROR("get_participant_entry () failed for call_id: %s",
                               clientDetail.m_screenshare_id.c_str());
        }else{
          if (!org_client->m_detail.m_screenshare_id.empty())
             org_client->m_detail.m_screenshare_id.clear();
          org_client->m_detail.m_screenshare_id = client->m_detail.m_callId;
        }
        Client *dst_client = nullptr;
        for (auto itr = pHmpConference->m_clientMap.begin ();
                itr != pHmpConference->m_clientMap.end (); ++itr){

         dst_client = itr->second;
         if (dst_client){
           // copy the call_id of the screen share client in all other client's object
           if (!dst_client->m_mid3CallId.empty())
               dst_client->m_mid3CallId.erase();
             dst_client->m_mid3CallId = client->m_detail.m_callId;
             dst_client->m_mid3CallType = XGATE_MID3_CALL_TYPE_SCREENSHARE;
             dst_client->m_viewType = XGATE_VIEW_TYPE_ACTIVE_SPEAKER;
          }
       }
     }
     break;
    }
    default:
    {
      XGLOG_ERROR("Invalid callType recvd in create conference client");
      return EN_XGATE_STATUS_ERROR;
    }
  }
  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateHmpConference::remove_conf_participant(Client *client)
{
  map<string,HmpFrameChannel*>::iterator itr_conf;
  HmpFrameChannel *pHmpFrameChannel = NULL;
  SFUController *pSFUController = xGateSFUtil::getSFUController();
  ACE_Message_Block *pAmb = 0;
  string callId = client->m_detail.m_callId;
  xGateHmpClientThreadMsg *pMsg = NULL;
  list<map<string,Client*>::iterator> list_conf;
  if(!client){
    XGLOG_ERROR("No valid client recvd in remove_conf_participant");
    EN_XGATE_STATUS_ERROR;
  }
  /* Stop receiving and transmitting Data */
  client->stop_data_threads (client);
  client->m_StopSendRecvData = TRUE;
  client->m_data_rcv_thr_stop = TRUE;
  /* Fourthly delete the entry for this client from the conference */
  XGLOG_INFO( "Removing the client entry for callId: %s", callId.c_str());
  remove_participant_entry(callId);
  XGLOG_INFO("Sending XGATE_CLIENT_DELETE_ALL_CHANNEL msg to client: %s",
              client->m_detail.m_callId.c_str());
  ACE_Time_Value tval ((time(NULL) ));
  tval += ACE_Time_Value(0,1);
  /* Send delete all channel message from client */
  pMsg = new xGateHmpClientThreadMsg ();
  pMsg->m_msgType = XGATE_CLIENT_DELETE_ALL_CHANNEL;
  pMsg->param1 = static_cast <void*> (client);
  pMsg->pClient = client;
  if (pSFUController->m_ThreadPool_info.m_event_pool) {
     GError **error = nullptr;
     g_thread_pool_push (pSFUController->m_ThreadPool_info.m_event_pool, pMsg, error);
     if (error != nullptr){
        XGLOG_ERROR ("Failed to push event to thread pool");
     }
  }
  else{
    XGLOG_ERROR ("event pool thread is null... can't push event");
  }
  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateHmpConference::remove_send_channel(Client *client,
                                                    string callId)
{
  xGateHmpClientThreadMsg *pMsg = NULL;
  SFUController *pSFUController = xGateSFUtil::getSFUController();
  ACE_Message_Block *pAmb = 0;
  ACE_Time_Value tval ((time(NULL) ));
  tval += ACE_Time_Value(0,1);
  XGLOG_INFO("Sending XGATE_CLIENT_DELETE_SEND_CHANNEL msg to client: %s",
              client->m_detail.m_callId.c_str());
  pMsg = new xGateHmpClientThreadMsg ();
  char *callId_str = malloc (strlen(callId.c_str()) + 1);
  strcpy (callId_str, callId.c_str());
  pMsg->m_msgType = XGATE_CLIENT_DELETE_SEND_CHANNEL;
  pMsg->param2 = static_cast <void*> (client);
  pMsg->param3 = static_cast <void*> (callId_str);
  pMsg->pClient = client;
  if (pSFUController->m_ThreadPool_info.m_event_pool) {
     GError **error = nullptr;
     g_thread_pool_push (pSFUController->m_ThreadPool_info.m_event_pool, pMsg, error);
     if (error != nullptr){
        XGLOG_ERROR ("Failed to push event to thread pool");
     }
  }
  else{
    XGLOG_ERROR ("event pool thread is null... can't push event");
  }

  return EN_XGATE_STATUS_SUCCESS;
}
    // Start Conference participant MAP related interfaces
Client* xGateHmpConference::get_participant_entry(string callId)
{
  Client* client = NULL;
  CLIENT_MAP::iterator itr = m_clientMap.begin();

  for(itr = m_clientMap.begin(); itr != m_clientMap.end(); itr++) {
    client = itr->second;
    if(client){
      XGLOG_TRACE ( "Client %p client->m_detail.m_callId.c_str(): %s passed callId: %s",
          client,client->m_detail.m_callId.c_str(), callId.c_str());
      if (client->m_detail.m_callId == callId)
        break;
    }
  }
  if(itr != m_clientMap.end()) {
     XGLOG_TRACE ( "get_participant_entry success for call Id: %s", callId.c_str());
  } else {
     return NULL;
  }
  return client;
}

Client* xGateHmpConference::get_ssrc_participant_entry(guint ssrc)
{
  Client* client = NULL;
  CLIENT_MAP::iterator itr = m_clientMap.begin();

  for(itr = m_clientMap.begin(); itr != m_clientMap.end(); itr++) {
    client = itr->second;
    if(client){
      if(client->m_FrameChannel && client->m_FrameChannel->m_pHmpFrameBin){
        HmpFrameBin *pframeRecvBin = client->m_FrameChannel->m_pHmpFrameBin;
        XGLOG_TRACE ( "Client %p client->m_detail.m_retrieved_videossrc: %u passed ssrc: %u",
          client, pframeRecvBin->retrieved_videossrc, ssrc);
        if (pframeRecvBin->retrieved_videossrc == ssrc)
          break;
      }
    }
  }
  if(itr != m_clientMap.end()) {
     XGLOG_INFO ( "get_ssrc_participant_entry success for ssrc: %u, client : %s", ssrc, client->m_detail.m_callId.c_str());
  } else {
     XGLOG_ERROR( "get_ssrc_participant_entry failed for ssrc: %u", ssrc);
     return NULL;
  }
  return client;
}

xGateRetVal xGateHmpConference::insert_participant_entry(string callId, Client* client)
{
  xGateRetVal retVal = EN_XGATE_STATUS_ERROR;
  /* Insert dummy callId as precaution to avoid overrun to the end of the map
   * while iterating the elements of map
   */
  if (m_clientMap.empty()){
     string dummy = DUMMY_CALLID;
     m_clientMap[dummy] = nullptr;
  }
  m_clientMap[callId]= client;
  m_clientVector.push_back (client);
  retVal = EN_XGATE_STATUS_SUCCESS;
  return retVal;
}

xGateRetVal xGateHmpConference::remove_participant_entry(string callId)
{
  if (!callId.empty ()){
    guint uiIndex = 0;
    Client   *client = nullptr;
    XGLOG_INFO( "remove_participant_entry for callId: %s", callId.c_str());
    m_clientMap.erase(callId);

    for (uiIndex = 0; uiIndex < m_clientVector.size(); uiIndex++){
      client = m_clientVector[uiIndex];
      if (client && (client->m_detail.m_callId == callId)){
        m_clientVector.erase (m_clientVector.begin() + uiIndex);
        break;
      }
    }
  }
  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateHmpConference::remove_participant_entries()
{
  m_clientMap.clear();

  return EN_XGATE_STATUS_SUCCESS;
}

#ifdef RECORD_ENABLED
HmpRecordSendChannel* xGateHmpConference::get_record_send_channel(string clientId)
{
  HmpRecordSendChannel *channel = NULL;

  for(auto itr = m_recorderMap.begin(); itr != m_recorderMap.end(); itr++) {
    channel = itr->second;
    if (channel)
      XGLOG_INFO ("Checking clientId: %s with channel->m_clientId: %s",
               clientId.c_str(), channel->m_clientId.c_str());
    if((itr != m_recorderMap.end()) && (nullptr != channel) && (channel->m_clientId == clientId)) {
      XGLOG_DEBUG( "RecordSendChannel found for client '%s'", clientId.c_str());
      return channel;
    }
  }

  XGLOG_ERROR( "RecordSendChannel not found for client '%s'", clientId.c_str());
  return NULL;
}

xGateRetVal xGateHmpConference::insert_record_send_channel(string clientId, HmpRecordSendChannel *channel)
{
  XGLOG_INFO( "inserted RecordSendChannel for client: '%s' in map", clientId.c_str());
  /* Insert dummy callId as precaution to avoid overrun to the end of the map
   * while iterating the elements of map
   */
  if (m_recorderMap.empty()){
    string dummy = DUMMY_CALLID;
    m_recorderMap[dummy] = nullptr;
  }
  m_recorderMap[clientId]= channel;
  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateHmpConference::remove_record_send_channel(string clientId)
{
  m_recorderMap.erase(clientId);
  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateHmpConference::remove_record_send_channels()
{
  m_recorderMap.clear();
  return EN_XGATE_STATUS_SUCCESS;
}

void xGateHmpConference::free_recorder_audio_queue(gpointer data)
{
  XGLOG_INFO("unreffering a remaining buffer list from recorder_audio_queue");
  GstBufferList *list = (GstBufferList *) data;
  gst_buffer_list_unref(list);
}

void xGateHmpConference::free_recorder_video_queue(gpointer data)
{
  XGLOG_INFO("unreffering a remaining buffer list from recorder_video_queue");
  GstBufferList *list = (GstBufferList *) data;
  gst_buffer_list_unref(list);
}
#endif

gboolean Client::CreateInhouseClient(std::string roomId,std::string connId,std::string deviceId,unsigned int isCcaas)
{

  m_inhouseClient = new URInhousesttclient(roomId.c_str(),connId.c_str(),deviceId.c_str(),isCcaas, this);
  if(!m_inhouseClient)
  {
     return EN_XGATE_STATUS_ERROR;
  }
  m_inhouseClient->Inhouse_websocket_connect();
  return EN_XGATE_STATUS_SUCCESS;
}

gboolean Client::restartRecognization()
{
   if(m_inhouseClient)
   {
      delete m_inhouseClient;
      m_inhouseClient = NULL;
   }
   else
   {
      return EN_XGATE_STATUS_ERROR;
   }

   m_inhouseClient = new URInhousesttclient(m_detail.m_mgresourceId.c_str(),
                         m_detail.m_conn_id.c_str(),m_detail.m_device_id.c_str(),m_detail.is_ccaas, this);
   if(!m_inhouseClient)
   {
      return EN_XGATE_STATUS_ERROR;
   }
   m_inhouseClient->Inhouse_websocket_connect();
   return EN_XGATE_STATUS_SUCCESS;
}

#ifdef RECORD_ENABLED
void Client::free_rcvd_audio_queue(gpointer data)
{
  XGLOG_INFO("unreffering a remaining buffer list from rcvd_audio_queue");
  GstBufferList *list = (GstBufferList *) data;
  gst_buffer_list_unref(list);
}

void Client::free_rcvd_video_queue(gpointer data)
{
  XGLOG_INFO("unreffering a remaining buffer list from rcvd_video_queue");
  GstBufferList *list = (GstBufferList *) data;
  gst_buffer_list_unref(list);
}
#endif
