#ifndef _XGATE_HMP_VIDEO_VP9_PARSE_H
#define _XGATE_HMP_VIDEO_VP9_PARSE_H


//local include
#include "xGateUtil.h"
#include "xGateHmpGstUtil.h"
#include "xGateMgMsg.h"
#include "xGateHmpGstBin.h"
#include "xGateHmpGstVideoBin.h"

#define VP9_PARSE_MAX_PKTS 20

#ifdef VP9_DEBUG_PRINTF
   #define VP9_debug(a) printf a
#else
   #define VP9_debug(a) (void)0
#endif

#define MAX_REFID_CNT 3
#define MAX_SPATIAL_LAYERS 3
#define MAX_PIC_IN_PG 255
#define MAX_REFERENCE_INDICES 3
#define MAX_IMAGE_WIDTH 3840
#define MAX_IMAGE_HEIGHT 2160
/* As per RFC num of spatial layes = x + 1 where x is stored first 3 bits*/
#define NUM_SPATIAL_LAYERS(ns)          ((((uint8_t)ns >> 5 ) & 0x7) + 1)
#define TEMPORAL_ID(b)                  (((uint8_t)b >> 5 ) & 0x7)
#define NUM_REF_PICS(p)                 (((uint8_t)p >> 2) & 0x3)
#define TEMPORAL_UP_SWITCH(u)           (((uint8_t)u >> 4) & 0x1)
#define SPATIAL_LAYER_ID(sid)           (((uint8_t)sid >> 1) & 0x7)
#define INTER_LAYER_DEPENDENCY_BIT(d)   ((uint8_t)d & 0x1)
#define PICTURE_DIFF(pdiff)             (((uint8_t)pdiff >> 1) & 0x7F)

#define GET_PICTURE_ID_2BYTES(pid)      (((pid[0] & 0x7F) << 8) | pid[1])
#define GET_PICTURE_ID_1BYTE(pid)       ((uint8_t)pid & 0x7F)
#define GET_MBIT(m)                     (((uint8_t)m & 0x80) > 0 ? 1 : 0)
#define GET_REFERENCE_PIC_BIT(r)        ((uint8_t)r & 0x1)
#define GET_Y_BIT(y)                    (((uint8_t)y >> 4 )& 0x1)
#define GET_G_BIT(g)                    (((uint8_t)g >> 3 )& 0x1)

#define IS_PICTURE_ID_SET(id)           (((uint8_t)id >> 7 )& 0x1)
#define IS_INTER_PIC_SET(id)            (((uint8_t)id >> 6 )& 0x1)
#define IS_LAYER_INDICES_SET(id)        (((uint8_t)id >> 5 )& 0x1)
#define IS_FLEXIBLE_MODE_SET(id)        (((uint8_t)id >> 4 )& 0x1)
#define IS_BEGIN_FRAME_SET(id)          (((uint8_t)id >> 3 )& 0x1)
#define IS_END_FRAME_SET(id)            (((uint8_t)id >> 2 )& 0x1)
#define IS_SCALABILITY_STRUCT_SET(id)   (((uint8_t)id >> 1 )& 0x1)
#define IS_NOT_REFERENCE_FRAME_SET(id)  ((uint8_t)id & 0x1)

//VP9 Payload Descriptor
typedef struct _VP9PayloadHeader {

    // 1st Byte
    bool i_bit ;  // PictureId present .
    bool p_bit ;  // Inter-picture predicted.
    bool l_bit ;  // Layer indices present.
    bool f_bit ;  // Flexible mode.
    bool b_bit ;  // Begins frame flag.
    bool e_bit ;  // Ends frame flag.
    bool v_bit ;  // Scalability structure present.
    bool z_bit ;  // Not used for inter-layer prediction

   // 2nd Byte PID
    bool    m_bit ;  // 7 or 15 bit pid
    int16_t PID ; // Picture ID

    // 3/4 Byte Layer Info
    uint8_t TID;// Temporal Layer ID
    bool    u_bit; //switching up point;
    uint8_t SID; // Spatial Layer ID;
    bool    d_bit; //Inter-layer dependency used
    uint8_t TL0PICIDX;  // non-flex mode only

   // 5/6 Byte Reference PIDs
   //Only Delta frames in Flexible Mode, have these
    uint8_t  p_diff[MAX_REFID_CNT];
    uint32_t ref_pid[MAX_REFID_CNT];
    uint8_t  num_ref_pics;
    uint32_t scaled_pid;

    // Scalability Structure
    uint8_t  N_S; // ns+1 = No of Spatial Layers
    bool     y_bit; // resolution present
    bool     g_bit; // PG description present
    uint16_t width[MAX_SPATIAL_LAYERS];
    uint16_t height[MAX_SPATIAL_LAYERS];
    uint8_t  N_G;   // No of Pictures in PG, max 0xFF, 255
    uint8_t  tid[MAX_PIC_IN_PG]; // N_G or 255
    bool     u[MAX_PIC_IN_PG];
    uint8_t  r[MAX_PIC_IN_PG]; // No of P_diff (Ref Indices)
    uint8_t  PID_diff[MAX_PIC_IN_PG][MAX_REFERENCE_INDICES];
} VP9PayloadHeader;


class VideoRtpDepacketizerVp9 {
   public:
   void ParsePictureId(uint8_t *b, VP9PayloadHeader *pVP9Hdr);
   void ParseLayerInfo(uint8_t *b, VP9PayloadHeader *pVP9Hdr);
   uint8_t ParseRefPids(uint8_t* , VP9PayloadHeader *pVP9Hdr);
   void ParseSS(uint8_t *b, VP9PayloadHeader *pVP9Hdr);
   void parse(uint8_t *a, VP9PayloadHeader *vp9hdr);
   void process_rtp_vp9_hdr(GstMapInfo *rtppktInfo, VP9PayloadHeader *vp9hdr);
   GstBufferList *ProcessVP9Payload(Client *client, mlineofferDetail *mlineDetails,
                           GstBufferList *pBufferList,
                           GstClockTime     clocktime);
   GstBufferList *PrepareLayerList (Client *client, VP9PayloadHeader *pVP9Hdr,
                                   mlineofferDetail *mlineDetails,
                                   GstBuffer    *pBuffer,
                                   GstRTPBuffer  *rtpbuffer,
                                   GstClockTime     clocktime);

  void check_update_rtp_timestamp (GstRTPBuffer     *rtpbuffer,
                                   mlineofferDetail *pmlineDetail,
                                   Client           *client,
                                   GstClockTime     clocktime);
  gboolean check_missing_packets (mlineofferDetail *pmlineDetail, GstBufferList  *pList);
};
#endif

