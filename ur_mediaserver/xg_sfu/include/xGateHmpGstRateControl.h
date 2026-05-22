#ifndef HMP_GST_RATE_CONTROL_H
#define HMP_GST_RATE_CONTROL_H

#include <stdlib.h>
#include <stdio.h>
#include "xGateUtil.h"
#include "xGateHmpGstUtil.h"
#include "xGateHmpGstBin.h"
#include "xGateHmpGstVideoBin.h"

class RateControl {
   public:
      RateControl(guint start_bitrate);
      ~RateControl();
      xGateBandwidthUsage IdentifyBandWidthUsage(TWCCStats *sPrevTwccAvgStat, TWCCStats *sTwccAvgStat);
      guint MultiplicativeRateDecrease(guint current_bitrate);
      xGateLossLevel DetectLoss(TWCCStats *sTwccAvgStat);
      guint LossBasedChangeBitrate(TWCCStats *sTwccAvgStat);
      guint AdditiveRateIncrease(guint current_bitrate);
      xGateRateControlState Update(Client* client,TWCCStats *sTwccAvgStat);
      guint GetCurrentBitrate();
   private:
      guint m_start_bitrate;
      guint m_current_bitrate;
      guint m_latest_throughput_on_delay;
      guint m_latest_throughput_on_loss;
      guint m_prev_increased_bitrate;
      xGateBandwidthUsage eBandWidthUsage;
      xGateLossLevel loss_level;
      gfloat threshold;
      gfloat time_over_using;
      int overuse_counter;
      int underuse_counter;
      int underloss_counter;
      gfloat overusing_time_threshold;
      gfloat prev_offset;
      xGateBandwidthUsage m_band_usage; 

};

#endif
