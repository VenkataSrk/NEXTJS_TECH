#include "xGateHmpVP9Parser.h"
#include "xGateSFUtil.h"

// Picture ID:
//
//      +-+-+-+-+-+-+-+-+
// I:   |M| PICTURE ID  |   M:0 => picture id is 7 bits.
//      +-+-+-+-+-+-+-+-+   M:1 => picture id is 15 bits.
// M:   | EXTENDED PID  |
//      +-+-+-+-+-+-+-+-+
//
void VideoRtpDepacketizerVp9::ParsePictureId(uint8_t *b, VP9PayloadHeader *vp9hdr)
{
   vp9hdr->m_bit = GET_MBIT(b[0]);
   vp9hdr->PID = 0;

   if(vp9hdr->m_bit){
      vp9hdr->PID = GET_PICTURE_ID_2BYTES(b);
   }
   else {
      vp9hdr->PID = GET_PICTURE_ID_1BYTE(b[0]);
   }

   VP9_debug(("-----1. PictureID-----: %2x %2x\n", b[0],b[1]));
   VP9_debug(("\tm_bit: %d\n\tPID: %d\n",vp9hdr->m_bit, vp9hdr->PID));

}

// Layer indices :
//
//      +-+-+-+-+-+-+-+-+
// L:   |  T  |U|  S  |D|
//      +-+-+-+-+-+-+-+-+
//      |   TL0PICIDX   |  (non-flexible mode only)
//      +-+-+-+-+-+-+-+-+
//
void VideoRtpDepacketizerVp9::ParseLayerInfo(uint8_t *b, VP9PayloadHeader *vp9hdr)
{
  vp9hdr->TID = TEMPORAL_ID (b[0]); //parse temporal idx TID
  vp9hdr->u_bit = TEMPORAL_UP_SWITCH (b[0]);//parse temporal Up switch
  vp9hdr->SID  = SPATIAL_LAYER_ID (b[0]); //parse spatial idx SID
  vp9hdr->d_bit = INTER_LAYER_DEPENDENCY_BIT (b[0]); //parse inter layer predicted

  if(!vp9hdr->f_bit) {
     vp9hdr->TL0PICIDX = b[1];
  }

  VP9_debug(("-----2. Layer Info-----: %2x\n", b[0]));
  VP9_debug(("\tTID- Temporal Layer ID: %d\n", vp9hdr->TID));
  VP9_debug(("\tU- Switching Up Point: %d\n",vp9hdr->u_bit));
  VP9_debug(("\tSID- Spatial Layer ID: %d\n",vp9hdr->SID));
  VP9_debug(("\tD- current SID depends on SID-1: %d\n", vp9hdr->d_bit));

  if(!vp9hdr->f_bit)
      VP9_debug(("\tTL0PICIDX- Temporal Layer0 Idx (Non-Flex mode): %2x\n", vp9hdr->TL0PICIDX));

}

// Reference indices:
//
//      +-+-+-+-+-+-+-+-+                P=1,F=1: At least one reference index
// P,F: | P_DIFF      |N|  up to 3 times          has to be specified.
//      +-+-+-+-+-+-+-+-+                    N=1: An additional P_DIFF follows
//                                                current P_DIFF.
//
uint8_t VideoRtpDepacketizerVp9::ParseRefPids(uint8_t *b, VP9PayloadHeader *pVP9Hdr)
{
  uint8_t p_diff[MAX_REFERENCE_INDICES]={0,0,0};
  uint32_t ref_pid[MAX_REFERENCE_INDICES] = {0,0,0};
  uint8_t num_ref_pics =0;
  bool n_bit = 0;
  uint32_t scaled_pid = pVP9Hdr->PID;

  VP9_debug(("-----3. RefIds (Flex mode only)-----: %2x\n", b[0]));
  do {
     p_diff[num_ref_pics] = PICTURE_DIFF(b[num_ref_pics]);
     n_bit = GET_REFERENCE_PIC_BIT(b[num_ref_pics]);
     ref_pid[num_ref_pics] = pVP9Hdr->PID - p_diff[num_ref_pics];
     VP9_debug(("\tpid_diff[%d]: =%d\n", num_ref_pics, p_diff[num_ref_pics]));
     num_ref_pics++;
   } while (n_bit && (num_ref_pics <= MAX_REFERENCE_INDICES));

  VP9_debug(("\tnum ref pics: %2x\n",num_ref_pics));

  return num_ref_pics;
}


// Scalability structure (SS).
//
//      +-+-+-+-+-+-+-+-+
// V:   | N_S |Y|G|-|-|-|
//      +-+-+-+-+-+-+-+-+              -|
// Y:   |     WIDTH     | (OPTIONAL)    .
//      +               +               .
//      |               | (OPTIONAL)    .
//      +-+-+-+-+-+-+-+-+               . N_S + 1 times
//      |     HEIGHT    | (OPTIONAL)    .
//      +               +               .
//      |               | (OPTIONAL)    .
//      +-+-+-+-+-+-+-+-+              -|
// G:   |      N_G      | (OPTIONAL)
//      +-+-+-+-+-+-+-+-+                           -|
// N_G: |  T  |U| R |-|-| (OPTIONAL)                 .
//      +-+-+-+-+-+-+-+-+              -|            . N_G times
//      |    P_DIFF     | (OPTIONAL)    . R times    .
//      +-+-+-+-+-+-+-+-+              -|           -|
//
void VideoRtpDepacketizerVp9::ParseSS(uint8_t *b, VP9PayloadHeader *pVP9Hdr)
{
   pVP9Hdr->N_S = NUM_SPATIAL_LAYERS (b[0]);
   pVP9Hdr->y_bit = GET_Y_BIT (b[0]);
   pVP9Hdr->g_bit = GET_G_BIT (b[0]);
   pVP9Hdr->N_G = 0;
   uint16_t byte_offset = 0;
   uint8_t numRefPics[MAX_PIC_IN_PG] = {0};
   uint8_t PID_diff[MAX_PIC_IN_PG][MAX_REFERENCE_INDICES] = {0};

   VP9_debug(("-----4. Scalability Structure-----: %2x\n", b[0]));
   XGLOG_INFO("-----4. Scalability Structure-----: %2x\n", b[0]);
   VP9_debug(("\tSpatial layers count: %d\n", pVP9Hdr->N_S));
   if (pVP9Hdr->N_S > MAX_SPATIAL_LAYERS){
      XGLOG_INFO(" NUMBER OF spatial layers extracted: %d can't be > %d layers might be wrongly set to 1",
                       pVP9Hdr->N_S, MAX_SPATIAL_LAYERS);
      pVP9Hdr->N_S = 1;
   }
   VP9_debug(("\tSpat layer Resolution present:%d\n", pVP9Hdr->y_bit));
   VP9_debug(("\tG bit: %d\n", pVP9Hdr->g_bit));

   if(pVP9Hdr->y_bit) {
       for(int i = 0; i < pVP9Hdr->N_S; i++) {

          pVP9Hdr->width[i] = b[byte_offset+1]<<8 | b[byte_offset+2];
          if (pVP9Hdr->width[i] > MAX_IMAGE_WIDTH) {
             XGLOG_WARN ("Image width %d can't be > %d width", pVP9Hdr->width[i], MAX_IMAGE_WIDTH);
             pVP9Hdr->width[i] = 0;
          }
          pVP9Hdr->height[i] = b[byte_offset+3]<<8 | b[byte_offset+4];
          if (pVP9Hdr->height[i] > MAX_IMAGE_HEIGHT) {
             XGLOG_WARN ("Image height %d can't be > %d height", pVP9Hdr->height[i], MAX_IMAGE_HEIGHT);
             pVP9Hdr->height[i] = 0;
          }

          VP9_debug(("\tSpat Layer reso W X H= %d X %d \n",
                    pVP9Hdr->width[i], pVP9Hdr->height[i]));
          byte_offset += 4;
       }
   }

   if(pVP9Hdr->g_bit) {
      byte_offset++;
      pVP9Hdr->N_G = b[byte_offset];
      VP9_debug(("\tN_G- Picture count in Picture Group: %d\n", pVP9Hdr->N_G));

      for(int i=0; i < pVP9Hdr->N_G; i++) {
          byte_offset++;
          pVP9Hdr->tid[i] = TEMPORAL_ID (b[byte_offset]);
          pVP9Hdr->u[i] = TEMPORAL_UP_SWITCH (b[byte_offset]);
          pVP9Hdr->r[i] = NUM_REF_PICS (b[byte_offset]);

          VP9_debug(("\n\tTID- Temporal ID: %d\n", pVP9Hdr->tid[i]));
          VP9_debug(("\tU- Temporal Up switch:%d\n",pVP9Hdr->u[i]));
          VP9_debug(("\tR- Num of Ref Pics: %d\n", pVP9Hdr->r[i]));

          for(int k=0; k < pVP9Hdr->r[i]; k++) {
            byte_offset++;
            pVP9Hdr->PID_diff[i][k] = b[byte_offset];
            VP9_debug(("\tPID_diff[%d][%d]: %d\n",i,k, pVP9Hdr->PID_diff[i][k]));
          }
      }
   }

   VP9_debug(("\n"));
}

void VideoRtpDepacketizerVp9::parse(uint8_t *buff, VP9PayloadHeader *vp9hdr)
{
  uint8_t first_byte = buff[0];
  uint8_t buff_offset = 0;
  uint8_t num_refids = 0;
  VP9_debug(("\n-----0. VP9 Header parse-----: %2x \n", first_byte));
  //XGLOG_TRACE ("-----0. VP9 Header parse-----: %2x \n", first_byte);

  memset (vp9hdr, 0 , sizeof (VP9PayloadHeader));
  vp9hdr->i_bit = IS_PICTURE_ID_SET (first_byte); // PictureId present .
  vp9hdr->p_bit = IS_INTER_PIC_SET (first_byte); // Inter-picture predicted.
  vp9hdr->l_bit = IS_LAYER_INDICES_SET (first_byte); // Layer indices present.
  vp9hdr->f_bit = IS_FLEXIBLE_MODE_SET (first_byte); // Flexible mode.
  vp9hdr->b_bit = IS_BEGIN_FRAME_SET (first_byte); // Begins frame flag.
  vp9hdr->e_bit = IS_END_FRAME_SET (first_byte); // Ends frame flag.
  vp9hdr->v_bit = IS_SCALABILITY_STRUCT_SET (first_byte); // Scalability structure present.
  vp9hdr->z_bit = IS_NOT_REFERENCE_FRAME_SET (first_byte); // Not used for inter-layer prediction

  VP9_debug(("\tI- Picture Id present: %d\n", vp9hdr->i_bit));
  VP9_debug(("\tP- Inter-picture predicted frame: %d\n", vp9hdr->p_bit));
  VP9_debug(("\tL- Layer indices present: %d\n", vp9hdr->l_bit));
  VP9_debug(("\tF- Flexible mode: %d\n", vp9hdr->f_bit));
  VP9_debug(("\tB- Begins frame: %d\n", vp9hdr->b_bit));
  VP9_debug(("\tE- Ends frame: %d\n", vp9hdr->e_bit));
  VP9_debug(("\tV- Scalability structure present: %d\n", vp9hdr->v_bit));
  VP9_debug(("\tZ- Not a Ref Frame: %d\n", vp9hdr->z_bit));

  // 1.Parse Picture Ids.
  if (vp9hdr->i_bit) {
    buff_offset++;
    ParsePictureId(&(buff[buff_offset]) , vp9hdr);
  }
  if (vp9hdr->m_bit)
    buff_offset += 2;
  else
    buff_offset += 1;
  // 2.Parse Layer Info
  // if m_bit is set then layer info is present after picture ID. Advance the buff
  // offeset by 2 bytes otherwise by 1 byte
  if (vp9hdr->l_bit) {
    uint8_t* layer_byte;
    layer_byte = &(buff[buff_offset]);
    ParseLayerInfo(layer_byte, vp9hdr);
    if (!vp9hdr->f_bit)
      buff_offset += 2;
    else
      buff_offset += 1;
  }
  else{
    VP9_debug(("-----2. Layer Info absent\n"));
  }
  //3.Parse Reference PIDs only if Scalability structure is present . This parsing
  //is done only to know how many bytes to advance to get the Scalability structure
  //otherwise we don't require parsing RefPids since we are not decoding the picture
  if (vp9hdr->p_bit && vp9hdr->f_bit && vp9hdr->v_bit) {
     XGLOG_WARN("---P,F and v_bit are set ... calling ParseRefPids buff_offset: %d buff[buff_offset]: 0x%2x",
                buff_offset, buff[buff_offset]);
     num_refids = ParseRefPids( &(buff[buff_offset]), vp9hdr);
     buff_offset += num_refids; // correct this based on Flex mode N x P_DIFF
     XGLOG_INFO (" ===== P, F and v_bit SET .. After parsing num_refids : %d buff_offset: %d",
                  num_refids, buff_offset);
  }
  else
     VP9_debug(("-----3. Ref Indices absent in non-Flex Mode\n"));

  // 4. Parse Scalability Structure
  if(vp9hdr->v_bit) {  // SS present
      int i =0;
      XGLOG_TRACE ("vp9hdr->v_bit is set Scalability structure is present.. buffer_offset: %d", buff_offset);
      for (int j = 0; j < 3; j++){
        XGLOG_TRACE ("buff [i to i+7]: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x",
                 buff [i], buff [i+1], buff [i+2], buff [i+3], buff [i+4], buff [i+5], buff[i+6], buff [i+7]);
        i+=8;
      }

      ParseSS(&(buff[buff_offset]), vp9hdr); // Scalability Structure
  }
  else{
    VP9_debug(("-----4. Scalability Structure absent\n"));
  }
}

GstBufferList *VideoRtpDepacketizerVp9::ProcessVP9Payload(Client *client,
                                                mlineofferDetail *pmlineDetail,
                                                GstBufferList *pBufferList,
                                                GstClockTime     clocktime)
{
   GstRTPBuffer rtp = { NULL };
   uint8_t     *vp9_payload = NULL;
   VideoRtpDepacketizerVp9 vp9parseobj;
   VP9PayloadHeader vp9_hdr_details;
   GstBuffer        *pBuffer = NULL;
   offerDetail      *pofferDetail = &pmlineDetail->videoOfferDetails;
   unsigned int     uiNumBuffers = 0;
   unsigned int     uiIndex = 0;
   GstBufferList    *pOutList = NULL;
   gboolean         bPktLoss = FALSE;
   if (!pBufferList){
     XGLOG_ERROR ("Buffer list passed is NULL ..");
     return NULL;
   }
   uiNumBuffers = gst_buffer_list_length (pBufferList);
    /*
     * 1. Extract the buffers from the Picture List,
     * 2. Get the vp9 payload and parse the VP9 header.
     * 3. Prepare the buffer list of single layer and drop
     *    packets from other layers.
     */
   XGLOG_TRACE ("client: %s pmlineDetail->callId: %s buffer_list: %p uiNumBuffers: %d ",
                 client->m_detail.m_callId.c_str(),
                 pmlineDetail->callId_offer_generated.c_str(), pBufferList, uiNumBuffers);
   for (uiIndex = 0; uiIndex < uiNumBuffers; uiIndex++) {

      pBuffer = gst_buffer_list_get (pBufferList, uiIndex);
      if(!gst_rtp_buffer_map(pBuffer, GST_MAP_READWRITE, &rtp)){
        XGLOG_ERROR ("Failed to map rtp buffer");
        gst_buffer_list_remove (pBufferList, uiIndex, 1);
        continue;
      }
      XGLOG_TRACE ("Thread: %ld ssrc: %u buffer_list: %p  seqnum: %u uiNumBuffers: %d",
              syscall (SYS_gettid), gst_rtp_buffer_get_ssrc (&rtp),
              pBufferList,  gst_rtp_buffer_get_seq (&rtp), uiNumBuffers);
      /*get the vp9 payload */
      vp9_payload = (uint8_t *)gst_rtp_buffer_get_payload (&rtp);
      if (!vp9_payload){
        XGLOG_ERROR ("Failed to extract VP9 payload");
        gst_rtp_buffer_unmap (&rtp);
        gst_buffer_list_remove (pBufferList, uiIndex, 1);
        gst_buffer_unref (pBuffer);
        continue;
      }
      /*Parse the header and get the packet details*/
      vp9parseobj.parse(vp9_payload, &vp9_hdr_details);

      /*populate the svc structure based on vp9 hdr info */
      pOutList = vp9parseobj.PrepareLayerList (client, &vp9_hdr_details, pmlineDetail,
                                              pBuffer, &rtp, clocktime);

      gst_rtp_buffer_unmap (&rtp);

      /*Break if we got list prepared */
      if (pOutList){
        if (uiIndex != uiNumBuffers -1 ){
          pBuffer = gst_buffer_list_get (pBufferList, uiNumBuffers -1);
          if(!gst_rtp_buffer_map(pBuffer, GST_MAP_READWRITE, &rtp)){
            XGLOG_ERROR ("Failed to map rtp buffer");
           }else {
             pofferDetail->usPrevRcvdSeqNum = gst_rtp_buffer_get_seq (&rtp);
             gst_rtp_buffer_unmap (&rtp);
             XGLOG_TRACE ("pmline->callId: %s last seqnum in list: %u",
             pmlineDetail->callId_offer_generated.c_str(),
             pofferDetail->usPrevRcvdSeqNum);
           }
         }
    #if 0
        bPktLoss = check_missing_packets (pmlineDetail, pOutList);
        if (TRUE == bPktLoss){
          XGLOG_INFO ("Pkt loss detected.. dropping frame");
          gst_buffer_list_unref (pOutList);
          pOutList = NULL;
        }
    #endif
        break;
      }
   }
return_from_func:
   return pOutList;
}

GstBufferList *VideoRtpDepacketizerVp9::PrepareLayerList (Client *client,
                                 VP9PayloadHeader *pVP9Hdr,
                                 mlineofferDetail *pmlineDetail,
                                 GstBuffer        *pBuffer,
                                 GstRTPBuffer     *rtpbuffer,
                                 GstClockTime     clocktime )
{
   GstBufferList *pOutList = NULL;
   unsigned int  uiIndex = 0;
   Scalability_Struct_Info *pScalability = NULL;
   offerDetail   *pOfferDetails = &pmlineDetail->videoOfferDetails;
   unsigned short curr_seq_num = 0;
   short seq_diff = 0;
   gint     rtp_time_diff = 0;
   guint32 current_pkt_time = gst_rtp_buffer_get_timestamp (rtpbuffer);
   curr_seq_num = gst_rtp_buffer_get_seq (rtpbuffer);

   //check if it's duplicate packet and return if it's.
   //don't unref buffer here as it's done after function returns with
   //NULL output
   if ((pOfferDetails->uiPrevRTPTimestamp == current_pkt_time)
          && (curr_seq_num == pOfferDetails->usPrevRcvdSeqNum)){
     XGLOG_INFO ("client: %s Duplicate pkt with curr_time: %u seqnum: %u ",
             client->m_detail.m_callId.c_str(), current_pkt_time, curr_seq_num);
     return NULL;
   }
   /* If scalability structure is present, we will get resolution and how many
    * layers are sent. populate them
    */
   if (pVP9Hdr->v_bit){
      XGLOG_INFO ("Scalability structure Tx Client: %s num_layers: %d offer->mline: %s uiTxSSRCNum: %u resolution: %d",
                     client->m_detail.m_callId.c_str(),
                     pVP9Hdr->N_S, pmlineDetail->callId_offer_generated.c_str(),
                     pOfferDetails->uiTxSSRCNum, pVP9Hdr->y_bit);
      if (pVP9Hdr->y_bit){
         pScalability = &pOfferDetails->sSvcInfo.sScalabilitStructInfo;
         for (uiIndex = 0; uiIndex < pVP9Hdr->N_S; uiIndex++){
            XGLOG_INFO ("Layer: %d width: %hu x height: %hu",
                             uiIndex, pVP9Hdr->width[uiIndex], pVP9Hdr->height[uiIndex]);
            pScalability->asImageInfo[uiIndex].u16Width = pVP9Hdr->width[uiIndex];
            pScalability->asImageInfo[uiIndex].u16Height = pVP9Hdr->height[uiIndex];
         }

         if (pVP9Hdr->N_S > LAYER_ID_MAX)
           pVP9Hdr->N_S = LAYER_ID_1;
         pScalability->ucNumSpatialLayers = pVP9Hdr->N_S;
        // if SVC mode is L3T1 then all frames in picture needs to be sent before
        // switching to other layers. in this case initialize the variable to FALSE
        // if svc mode is S3T1 then frames are independent of other layers and all
        // frames in picture should not be sent. in that case ,set the keyFrame sent
        // flag to TRUE. Currently client is configured to L3T1 mode
         pOfferDetails->sSvcInfo.bKeyFrameAllLayersSent = FALSE;
         /* check if the current and is greater than number of layer currently received,
          */
         if (pOfferDetails->sSvcInfo.ucCurrentSpatialLayerID >= pScalability->ucNumSpatialLayers)
           pOfferDetails->sSvcInfo.ucCurrentSpatialLayerID = pScalability->ucNumSpatialLayers - 1;
         if (pOfferDetails->sSvcInfo.ucTargetSpatialLayerID >= pScalability->ucNumSpatialLayers - 1)
           pOfferDetails->sSvcInfo.ucTargetSpatialLayerID = pScalability->ucNumSpatialLayers - 1;

        /* if we got packet of the required layer
         * Use the begin frame and end frame indicators to add buffer to list.
         * if it's end of frame,mark the marker bit as 1
         */

        if ((pOfferDetails->sSvcInfo.ucCurrentSpatialLayerID !=
           pOfferDetails->sSvcInfo.ucTargetSpatialLayerID)
           && (pOfferDetails->sSvcInfo.ucTargetSpatialLayerID
              <= pScalability->ucNumSpatialLayers - 1)
           && (client->eDeviceType != XGATE_DEVICE_TYPE_MOBILE)) {
          /* As we are supporting Tile based, send layer_0 packets so that
            * n/w bandwidth can be saved since the display size at client side
            * would be <= 320 x 240. Sending base layer packets will be enough
            */
            if (0 != strcmp (pmlineDetail->callId_offer_generated.c_str() , "screenshare")){
               pOfferDetails->sSvcInfo.ucCurrentSpatialLayerID = LAYER_ID_1;
               pOfferDetails->sSvcInfo.ucTargetSpatialLayerID = LAYER_ID_1;
               XGLOG_INFO ("configured to current layer_id: %d target: %d num_layers: %u for pmline->callId: %s",
                    pOfferDetails->sSvcInfo.ucCurrentSpatialLayerID,
                    pOfferDetails->sSvcInfo.ucTargetSpatialLayerID,
                    pScalability->ucNumSpatialLayers,
                    pmlineDetail->callId_offer_generated.c_str());
            }else {
               XGLOG_INFO ("current: %d Target: %d num_of_layers: %u .. switching for pmline->callId: %s",
                    pOfferDetails->sSvcInfo.ucCurrentSpatialLayerID,
                    pOfferDetails->sSvcInfo.ucTargetSpatialLayerID,
                    pScalability->ucNumSpatialLayers,
                    pmlineDetail->callId_offer_generated.c_str());
               pOfferDetails->sSvcInfo.ucCurrentSpatialLayerID
                 = pOfferDetails->sSvcInfo.ucTargetSpatialLayerID;
            }
        }
     }
   } else {
     pVP9Hdr->N_S = pOfferDetails->sSvcInfo.sScalabilitStructInfo.ucNumSpatialLayers;
   }

   /*
    * Validate the SID extracted from payload. It's observed that from Android,
    * the first byte of vp9 payload is wrongly encoded which makes the program
    * to think that layer indices, scalability structures are present.
    * if Layer index greater than LAYER_ID_MAX , it means its wrongly encoded.
    * In those cases, assuming that there is no layer info or the SID is 0
    */
    if ((pVP9Hdr->SID > LAYER_ID_MAX)
         && (pVP9Hdr->SID > pOfferDetails->sSvcInfo.sScalabilitStructInfo.ucNumSpatialLayers)){
      XGLOG_WARN ("pVP9Hdr->SID: %d is > %d .. wrong SID when Tx to callId: %s uiTxSSRCNum: %u offer->mline: %s",
                       pVP9Hdr->SID,
                       pOfferDetails->sSvcInfo.sScalabilitStructInfo.ucNumSpatialLayers,
                       client->m_detail.m_callId.c_str(),
                       pOfferDetails->uiTxSSRCNum,
                       pmlineDetail->callId_offer_generated.c_str());
      pVP9Hdr->SID = LAYER_ID_0;
    }
#if 0
   /*
    * 1. send layer 0 packets if client is mobile as it would be smaller screen
    *   and layer 0 resoluton will suffice
    *
    * 2. send layer 0 packets if mid line is other than screen share since client
    *   would display in grid view and tile size will be less than layer_1 resolution
    */
   if ((client->eDeviceType == XGATE_DEVICE_TYPE_MOBILE)
      || (0 != strcmp(pmlineDetail->callId_offer_generated.c_str(), "screenshare")))
   {
      pOfferDetails->sSvcInfo.ucCurrentSpatialLayerID = LAYER_ID_0;
      pOfferDetails->sSvcInfo.ucTargetSpatialLayerID = LAYER_ID_0;
     // XGLOG_INFO ("pmlineDetail->callId: %s uiTxSSRCNum: %u configured ucCurrentID: %d ucTarget: %u",
     //           pmlineDetail->callId_offer_generated.c_str(), pOfferDetails->uiTxSSRCNum,
     //           pOfferDetails->sSvcInfo.ucCurrentSpatialLayerID, pOfferDetails->sSvcInfo.ucTargetSpatialLayerID);
   }
#endif
    /* if we haven't sent any packets till now, sequence diff is 0*/
   if (!pOfferDetails->ulNumPktSent){
      seq_diff = 1;
    }else{
      seq_diff = curr_seq_num + ~pOfferDetails->usPrevRcvdSeqNum + 1;
    }
     /* 1. if sequence number diff is more than half of the unsigned short , SRTP library
      *    In those case, reset the sequence number diff to 1
      * 2. If video to be sent on mid-3 line changes, there might be huge diffence in the
      *    previous sequmbers similar to rtp time stamp.In such case, consider it as
      *    next packet in sequence instead of considering as loss
      */
   rtp_time_diff = current_pkt_time - pOfferDetails->uiPrevRTPTimestamp;
   if (seq_diff >= MAX_RTP_SEQNUM/2)
     seq_diff = 1;

   if (((rtp_time_diff < 0) || (rtp_time_diff > 1 * VIDEO_SAMPLES_1SEC))
        && ((seq_diff < 0) || (seq_diff >= 100))) {
     seq_diff = 1;
   }
   if (seq_diff != 1)
   {
     guint   rcvd_ssrc = gst_rtp_buffer_get_ssrc (rtpbuffer);
     XGLOG_INFO ("RCVD ssrc: %u pkt seq num : %hu Prev seq num: %hu seq_diff: %d curr_Tx_seqnum: %u callId: %s uiTxSSRCNum: %u pVP9Hdr->SID: %d ucCurrentLayerID: %d Target_ID: %d num_of_layer: %d P_bit: %d Z_bit: %d u_bit: %d rcvd_size: %d b_bit: %d e_bit: %d d_bit: %d",
                           rcvd_ssrc,
                           curr_seq_num,
                           pOfferDetails->usPrevRcvdSeqNum,
                           seq_diff,
                           pOfferDetails->usSeqNum,
                           client->m_detail.m_callId.c_str(),
                                  pOfferDetails->uiTxSSRCNum,
                                  pVP9Hdr->SID,
                                  pOfferDetails->sSvcInfo.ucCurrentSpatialLayerID,
                                  pOfferDetails->sSvcInfo.ucTargetSpatialLayerID,
                                  pVP9Hdr->N_S,
                                 pVP9Hdr->p_bit,
                                 pVP9Hdr->z_bit,
                                 pVP9Hdr->u_bit,
                                 gst_buffer_get_size (pBuffer),
                                 pVP9Hdr->b_bit, pVP9Hdr->e_bit,
                                 pVP9Hdr->d_bit);
  #if 1
     SFUController *pSFUController = xGateSFUtil::getSFUController();
     xGateHmpConference * pxGateHmpConfObj
                      = pSFUController->find_conference_entry(client->m_detail.m_mgresourceId);
     if(!pxGateHmpConfObj){
       XGLOG_ERROR("Failed to find entry in conference");
       return FALSE;
     }
     Client *dst_client = pxGateHmpConfObj->get_participant_entry (pmlineDetail->callId_offer_generated);
     if (dst_client){
       HmpFrameBin *frameRecvBin = NULL;
       if (dst_client->m_FrameChannel)
         frameRecvBin = dst_client->m_FrameChannel->m_pHmpFrameBin;
       if (frameRecvBin){
         XGLOG_INFO ("Sending key frame request to client: %s pCient->callId: %s",
                                  pmlineDetail->callId_offer_generated.c_str(),
                                  pmlineDetail->pClient->m_detail.m_callId.c_str());
         frameRecvBin->SendKeyFrameRequest (dst_client);
       }
     }
   #endif
   }
   if ((pVP9Hdr->SID == pOfferDetails->sSvcInfo.ucCurrentSpatialLayerID)
        || (pVP9Hdr->SID < pOfferDetails->sSvcInfo.ucCurrentSpatialLayerID && !pVP9Hdr->z_bit)
        || (FALSE == pOfferDetails->sSvcInfo.bKeyFrameAllLayersSent)){

      if (clocktime != GST_CLOCK_TIME_NONE){
         GST_BUFFER_DTS (pBuffer) = clocktime;
         GST_BUFFER_PTS (pBuffer) = clocktime;
       }else{
         clocktime = GST_BUFFER_PTS (pBuffer);
       }

      #if ((!defined ENABLE_TWCC_TX_PATH) && (defined ENABLE_TWCC_RX_PATH))
      gst_rtp_buffer_remove_extension_data (rtpbuffer);
      #endif
     /* Take buffer reference so that the current buffer will not be
       * deleted when buffer_list_unref is called after returning from
       * this function
       */
      gst_rtp_buffer_set_ssrc (rtpbuffer, pOfferDetails->uiTxSSRCNum);
      gst_rtp_buffer_set_payload_type (rtpbuffer, pOfferDetails->uiPayloadNum);

      if ( (seq_diff > 1) || (seq_diff < 0)) {
        XGLOG_INFO ("client: %s seq_diff: %d  ssrc: %u current_Tx_seq num: %d Prev_RTP_time: %u rcvd_RTP_time: %u rtp_time_diff: %d current_uiTxTimestamp: %u",
                      client->m_detail.m_callId.c_str(),seq_diff, pOfferDetails->uiTxSSRCNum,
                       pOfferDetails->usSeqNum, pOfferDetails->uiPrevRTPTimestamp, current_pkt_time,
                       rtp_time_diff, pOfferDetails->uiTxTimestamp );
      }
      // check and modify the rtptimestamp if offer line is of screen share or active speaker
      #ifdef REUSE_MLINE_VIDEO_TX
         check_update_rtp_timestamp (rtpbuffer, pmlineDetail, client, clocktime);
      #endif
      HmpFrameBin *pFrameBin = client->m_FrameChannel->m_pHmpFrameBin;
      gst_buffer_ref (pBuffer);

     /*
       * End of frame bit set: frame complete and send buffer list
       * start of frame bit set: create new buffer list and add the packet to list
       * End and start bit not set: middle packet, add to list
       */
      if ((pVP9Hdr->e_bit && pVP9Hdr->b_bit) || (pVP9Hdr->e_bit)){
        if ((GstBufferList*) 0 == pOfferDetails->sSvcInfo.pPrevLayerBufferList){
          pOfferDetails->sSvcInfo.pPrevLayerBufferList = gst_buffer_list_new ();
          XGLOG_TRACE ("new_list: %p current seqnum: %hu size: %hu ",
                  pOfferDetails->sSvcInfo.pPrevLayerBufferList, pOfferDetails->usSeqNum,
                  gst_buffer_get_size (pBuffer));
          if ((GstBufferList*) 0 == pOfferDetails->sSvcInfo.pPrevLayerBufferList){
            XGLOG_ERROR ("Failed to allocate memory for buffer list ");
            return NULL;
          }
        }
        pOfferDetails->usSeqNum += seq_diff;
        gst_rtp_buffer_set_seq (rtpbuffer, pOfferDetails->usSeqNum);

        gst_buffer_list_add (pOfferDetails->sSvcInfo.pPrevLayerBufferList,
                              pBuffer);
        if (pVP9Hdr->SID == pOfferDetails->sSvcInfo.ucCurrentSpatialLayerID &&
             TRUE == pOfferDetails->sSvcInfo.bKeyFrameAllLayersSent) {
          XGLOG_TRACE ("Setting marker bit to true e_bit: %d b_bit: %d seqnum: %hu size: %d SID: %d",
                  pVP9Hdr->e_bit, pVP9Hdr->b_bit,
                  pOfferDetails->usSeqNum, gst_buffer_get_size (pBuffer), pVP9Hdr->SID);
          gst_rtp_buffer_set_marker (rtpbuffer, TRUE);
          pOutList = pOfferDetails->sSvcInfo.pPrevLayerBufferList;
          pOfferDetails->sSvcInfo.pPrevLayerBufferList = NULL;
        } else if (pVP9Hdr->SID
           == pOfferDetails->sSvcInfo.sScalabilitStructInfo.ucNumSpatialLayers - 1 ){
          XGLOG_TRACE ("Setting marker bit to true e_bit: %d b_bit: %d seqnum: %hu size: %d SID: %d",
                  pVP9Hdr->e_bit, pVP9Hdr->b_bit,
                  pOfferDetails->usSeqNum, gst_buffer_get_size (pBuffer), pVP9Hdr->SID);
           gst_rtp_buffer_set_marker (rtpbuffer, TRUE);
           if (pVP9Hdr->z_bit)
             pOfferDetails->sSvcInfo.bKeyFrameAllLayersSent = TRUE;
           pOutList = pOfferDetails->sSvcInfo.pPrevLayerBufferList;
           pOfferDetails->sSvcInfo.pPrevLayerBufferList = NULL;
        }
     }else if (pVP9Hdr->b_bit && !pVP9Hdr->e_bit){
        if (!pOfferDetails->sSvcInfo.pPrevLayerBufferList){
           pOfferDetails->sSvcInfo.pPrevLayerBufferList = gst_buffer_list_new ();
           if ((GstBufferList*) 0 == pOfferDetails->sSvcInfo.pPrevLayerBufferList){
             XGLOG_ERROR ("Failed to allocate memory for buffer list ");
             return NULL;
           }
        }
        // if it's beginning of layer frame and if there is a packet loss in other
        // layers, ignore as we are not sending other layer packets. the sequence
        // number should be increment by 1 rather than accounting the sequence number
        // difference caused due to other layers
        if (seq_diff > 1){
          XGLOG_INFO ("client: %si seq_diff: %d caused due to PKT loss from other layers.. ignoring",
                   client->m_detail.m_callId.c_str(), seq_diff);
          seq_diff = 1;
        }
        pOfferDetails->usSeqNum += seq_diff;
        gst_rtp_buffer_set_seq (rtpbuffer, pOfferDetails->usSeqNum);
        gst_buffer_list_add (pOfferDetails->sSvcInfo.pPrevLayerBufferList,
                              pBuffer);
      } else if (pOfferDetails->sSvcInfo.pPrevLayerBufferList){
        gst_buffer_list_add (pOfferDetails->sSvcInfo.pPrevLayerBufferList,
                              pBuffer);
        pOfferDetails->usSeqNum += seq_diff;
        gst_rtp_buffer_set_seq (rtpbuffer, pOfferDetails->usSeqNum);
      }else {
         gst_buffer_unref (pBuffer);
         XGLOG_ERROR ("unreffing buffer: %p pOutList: %p Rx_seqnum: %hu current Tx seq num:%hu b_bit: %d e_bit: %d",
                        pBuffer, pOutList, curr_seq_num,
                        pOfferDetails->usSeqNum,pVP9Hdr->b_bit, pVP9Hdr->e_bit);
      }
      pOfferDetails->ulNumPktSent++;
   }
   pOfferDetails->usPrevRcvdSeqNum = curr_seq_num;
   return pOutList;
}


void VideoRtpDepacketizerVp9::check_update_rtp_timestamp (
                                   GstRTPBuffer     *rtpbuffer,
                                   mlineofferDetail *pmlineDetail,
                                   Client           *client,
                                   GstClockTime     clocktime)
{
  guint32 current_pkt_time = gst_rtp_buffer_get_timestamp (rtpbuffer);
  offerDetail *pOfferDetails = &pmlineDetail->videoOfferDetails;
  gint32 rtp_time_diff = 0;
  GstClockTime  clocktime_diff_ms = 0;

  if (!pOfferDetails->uiPrevRTPTimestamp)
    pOfferDetails->uiPrevRTPTimestamp = current_pkt_time;

  if (!pOfferDetails->uiTxTimestamp)
    pOfferDetails->uiTxTimestamp = current_pkt_time;

  rtp_time_diff = current_pkt_time - pOfferDetails->uiPrevRTPTimestamp;

  // check if the rtp time difference is too much . this will happen when
  // video is stopped and started after a long time. In this case we need
  // to calculate the number of samples between the time gap and add it to
  // the rtp time stamp and send as RTP time stamp always shows running count
  // of the samples sent till now once the video is started. The count should
  // be incresing even though video is paused and started after a long time
  if ((rtp_time_diff < 0) || (rtp_time_diff > 1 * VIDEO_SAMPLES_1SEC)) {
    clocktime_diff_ms = clocktime - pOfferDetails->lastClockTime;
    clocktime_diff_ms = (clocktime_diff_ms ) / NANO_TO_MILLI_SEC;
    rtp_time_diff = (clocktime_diff_ms / MILLI_TO_SEC ) * VIDEO_SAMPLES_1SEC
                      + ((clocktime_diff_ms % MILLI_TO_SEC ) * VIDEO_SAMPLES_1SEC ) / MILLI_TO_SEC;
  }

  pOfferDetails->uiPrevRTPTimestamp = current_pkt_time;
  current_pkt_time = pOfferDetails->uiTxTimestamp + rtp_time_diff ;

  XGLOG_TRACE ("Final pkt_time: %u uiTxTimestamp: %u clocktime: %lld clocktime_diff_ms: %lld rtp_time_diff: %ld  ",
             current_pkt_time, pOfferDetails->uiTxTimestamp, clocktime, clocktime_diff_ms, rtp_time_diff );

  gst_rtp_buffer_set_timestamp (rtpbuffer, current_pkt_time);

  pOfferDetails->lastClockTime = clocktime;
  pOfferDetails->uiTxTimestamp = current_pkt_time;
}

gboolean VideoRtpDepacketizerVp9::check_missing_packets (mlineofferDetail *pmlineDetail,
                                                        GstBufferList  *pList)
{
  guint       uiIndex = 0;
  gboolean    bMarkerSet = TRUE;
  gboolean    bPktLoss = FALSE;
  GstBuffer   *buffer = NULL;
  short       seq_diff = 0;
  GstRTPBuffer rtp = { NULL };
  offerDetail      *pofferDetail = &pmlineDetail->videoOfferDetails;
  unsigned short curr_seq_num = 0;
  unsigned short prev_seq_num = 0;
  guint          uiNumBuffers = gst_buffer_list_length(pList);

  for (uiIndex = 0; uiIndex < uiNumBuffers; uiIndex++){
    buffer = gst_buffer_list_get (pList, uiIndex);
    if (buffer){
      if(!gst_rtp_buffer_map(buffer, GST_MAP_READ, &rtp)){
         XGLOG_ERROR ("Failed to map rtp buffer");
       }else{
          curr_seq_num = gst_rtp_buffer_get_seq (&rtp);
          bMarkerSet = gst_rtp_buffer_get_marker (&rtp);
          XGLOG_INFO ("Running seqnum: %u TxSSRC: %u prev_seq_num: %u curr_seq_num: %u bMarkerSet: %d",
                  pofferDetail->usSeqNum, pofferDetail->uiTxSSRCNum,
                  prev_seq_num, curr_seq_num, bMarkerSet);
          if ((prev_seq_num) && (unsigned short)(curr_seq_num + ~prev_seq_num + 1 ) != 1){
             bPktLoss = TRUE;
             seq_diff += curr_seq_num + ~prev_seq_num + 1;
             gst_rtp_buffer_unmap (&rtp);
             XGLOG_INFO ("Seq_num diff is !=1 .. prev_seq_num: %u curr_seq_num: %u Total seq_dif: %d",
                      prev_seq_num, curr_seq_num, seq_diff);
             break;
           }else if ((uiNumBuffers == 1)
               && ((unsigned short )(pofferDetail->usSeqNum + ~pofferDetail->usLastTxSeqNum + 1) != 1)) {
             bPktLoss = TRUE;
             seq_diff = curr_seq_num - pofferDetail->usLastTxSeqNum;
           }
           else{
             seq_diff +=1;
           }
          prev_seq_num = curr_seq_num;
          gst_rtp_buffer_unmap (&rtp);
       }
     }
  }
  // check if marker bit is set. if it's not set it means packet lost
  if (TRUE != bMarkerSet){
    XGLOG_INFO ("Marker bit is not TRUE .. marking frame as lost ");
    bPktLoss = TRUE;
  }
  if (bPktLoss){
    pofferDetail->usSeqNum = pofferDetail->usLastTxSeqNum;
    XGLOG_INFO ("Resetting the sequence num to: %u as packet is lost seq_diff: %d ",
                 pofferDetail->usSeqNum, seq_diff);
  }else{
    pofferDetail->usLastTxSeqNum = pofferDetail->usSeqNum;
  }
  return bPktLoss;
}
