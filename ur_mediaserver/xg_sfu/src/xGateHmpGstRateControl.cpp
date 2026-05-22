#include <stdio.h>
#include <iostream>
#include "xglog.h"
#include "xGateSFUtil.h"
#include "SFUClient.h"
#include "xGateHmpGstRateControl.h"

const int kMaxNumDeltas = 60;

RateControl::RateControl(guint start_bitrate):
   m_current_bitrate(0),m_latest_throughput_on_delay(0),
   m_prev_increased_bitrate(0),m_latest_throughput_on_loss(0),
   threshold(12.5),time_over_using(-1),overuse_counter(0),
   overusing_time_threshold(100),prev_offset(0.0),
   underuse_counter(0),underloss_counter(0)
    {
       m_start_bitrate = start_bitrate;
    }

RateControl::~RateControl()
    {
    }

xGateRateControlState RateControl::Update(Client *client,TWCCStats *sTwccAvgStat)
{
   eBandWidthUsage = IdentifyBandWidthUsage(&client->sTwccAvgStat, sTwccAvgStat);
   loss_level = DetectLoss(sTwccAvgStat);

   if(eBandWidthUsage == EN_XGATE_BW_OVERUSING || loss_level == EN_XGATE_MAXLOSS)
   {
      if(loss_level == EN_XGATE_MAXLOSS){
         m_current_bitrate = LossBasedChangeBitrate(sTwccAvgStat);
      }
      if(eBandWidthUsage == EN_XGATE_BW_OVERUSING){
         if(overuse_counter >= 5){
            m_current_bitrate = MultiplicativeRateDecrease(sTwccAvgStat->bitrate_sent);
         }
      }
      if(eBandWidthUsage == EN_XGATE_BW_OVERUSING && loss_level == EN_XGATE_MAXLOSS){
         if(overuse_counter >= 5){
            m_current_bitrate = std::min(m_latest_throughput_on_delay,m_latest_throughput_on_loss);
            if(m_current_bitrate < m_prev_increased_bitrate || m_prev_increased_bitrate == 0){
               m_prev_increased_bitrate = m_current_bitrate;
               XGLOG_INFO("OverUse and Max Loss Detected reducing current Bit rate %u bitrate %u for client %s",
                   sTwccAvgStat->bitrate_sent,m_current_bitrate,
                   client->m_detail.m_callId.c_str());
               overuse_counter = 0;
               return EN_XGATE_RCDECREASE;
            }else{
               return EN_XGATE_RCHOLD;
            }
         }else{
            if(m_current_bitrate < m_prev_increased_bitrate || m_prev_increased_bitrate == 0){
               m_prev_increased_bitrate = m_current_bitrate;
               XGLOG_INFO("Max Loss Detected reducing current Bit rate %u to bitrate %u for client %s",
                   sTwccAvgStat->bitrate_sent,m_current_bitrate,
                   client->m_detail.m_callId.c_str());
               return EN_XGATE_RCDECREASE;
            }else{
               return EN_XGATE_RCHOLD;
            }
         }
      }else if(loss_level == EN_XGATE_MAXLOSS){
         if(m_current_bitrate < m_prev_increased_bitrate || m_prev_increased_bitrate == 0){
            m_prev_increased_bitrate = m_current_bitrate;
            XGLOG_INFO("Max Loss Detected, reducing current Bit rate %u to %u for client %s",
                       sTwccAvgStat->bitrate_sent,m_current_bitrate,
                       client->m_detail.m_callId.c_str());
            return EN_XGATE_RCDECREASE;
         }else{
            return EN_XGATE_RCHOLD;
         }
      }else if(eBandWidthUsage == EN_XGATE_BW_OVERUSING){
         if(overuse_counter >= 5){
            if(m_current_bitrate < m_prev_increased_bitrate || m_prev_increased_bitrate == 0){
               m_prev_increased_bitrate = m_current_bitrate;
               XGLOG_INFO("OverUse Detected, reducing current Bit rate %u to %u for client %s",
                         sTwccAvgStat->bitrate_sent,m_current_bitrate,
                         client->m_detail.m_callId.c_str());
               overuse_counter = 0;
               return EN_XGATE_RCDECREASE;
            }else{
               return EN_XGATE_RCHOLD;
            }
         }else{
            return EN_XGATE_RCHOLD;
         }
      }
   }
   if(eBandWidthUsage == EN_XGATE_BW_UNDERUSING || loss_level == EN_XGATE_MINLOSS){
      XGLOG_TRACE ("Tx To Client : %s Bandwidth = %d loss_level: %d",
                            client->m_detail.m_callId.c_str(), eBandWidthUsage, loss_level);
      if(eBandWidthUsage == EN_XGATE_BW_UNDERUSING){
         if(underuse_counter < 2){
            underuse_counter++;
         }
      }
      if(loss_level == EN_XGATE_MINLOSS){
         if(underloss_counter < 2){
            underloss_counter++;
         }
      }
      /* should be based on available bandwidth or recent sent bit rate
       */
      m_current_bitrate = AdditiveRateIncrease(sTwccAvgStat->bitrate_sent);
      XGLOG_TRACE ("To Client: %s underuse: %d underloss: %d",
                  client->m_detail.m_callId.c_str(),
                  underuse_counter, underloss_counter);
      if (underuse_counter >= 2 && underloss_counter >= 2){
         {
            m_prev_increased_bitrate = m_current_bitrate;
            XGLOG_TRACE ("UnderUse Detected, increasing from %u TO %u for client %s",
                       sTwccAvgStat->bitrate_sent,m_current_bitrate,
                       client->m_detail.m_callId.c_str());
            underuse_counter = 0;
            underloss_counter = 0;
            return EN_XGATE_RCINCREASE;
         }
      }else if(underuse_counter >= 2){
         {
            XGLOG_TRACE ("UnderUse Detected, increasing from  %u TO %u previous_inc_bitrate: %dfor client %s",
                         sTwccAvgStat->bitrate_sent,m_current_bitrate,
                         m_prev_increased_bitrate,
                         client->m_detail.m_callId.c_str());
            m_prev_increased_bitrate = m_current_bitrate;
            underuse_counter = 0;
            return EN_XGATE_RCINCREASE;
         }
      }else if(underloss_counter >= 2){
         {
            m_prev_increased_bitrate = m_current_bitrate;
            XGLOG_TRACE ("Under_Use Detected, increasing from %u TO  %u bitrate for client %s",
                           sTwccAvgStat->bitrate_sent,m_current_bitrate,
                           client->m_detail.m_callId.c_str());
            underloss_counter = 0;
            return EN_XGATE_RCINCREASE;
         }
      }else {
         XGLOG_TRACE ("client: %s status HOLD m_current: %d m_prev: %d underuse: %d underloss: %d ",
                     client->m_detail.m_callId.c_str(),
                     m_current_bitrate, m_prev_increased_bitrate,
                     underuse_counter, underloss_counter);
         return EN_XGATE_RCHOLD;
      }
   }else if(eBandWidthUsage == EN_XGATE_BW_NORMAL){
     XGLOG_TRACE ("client: %s Usage == NORMAL m_current: %d m_prev: %d underuse: %d underloss: %d return HOLD",
                     client->m_detail.m_callId.c_str(),
                     m_current_bitrate, m_prev_increased_bitrate,
                     underuse_counter, underloss_counter);
      return EN_XGATE_RCHOLD;
   }
   return EN_XGATE_RCHOLD;
}

guint RateControl::GetCurrentBitrate(){
   return m_current_bitrate;
}

guint RateControl::MultiplicativeRateDecrease(guint current_bitrate){

   m_latest_throughput_on_delay = current_bitrate * 0.85;//0.85 - beta
   return m_latest_throughput_on_delay;
}

guint RateControl::LossBasedChangeBitrate(TWCCStats *sTwccAvgStat){
   guint lost_bitrate = 0;
   gfloat bitrate_loss_ratio = 0;

   lost_bitrate = (sTwccAvgStat->packet_loss_pct/100)*sTwccAvgStat->bitrate_sent;
   bitrate_loss_ratio = (gfloat)lost_bitrate/(gfloat)sTwccAvgStat->bitrate_sent;
   m_latest_throughput_on_loss = sTwccAvgStat->bitrate_sent*(1-(0.5*bitrate_loss_ratio));

   return m_latest_throughput_on_loss;
}

guint RateControl::AdditiveRateIncrease(guint current_bitrate){
   return current_bitrate * 1.05;
}

xGateBandwidthUsage RateControl::IdentifyBandWidthUsage(TWCCStats *sPrevTwccAvgStat,
                                                        TWCCStats *sCurrTwccAvgStat){
   gfloat ts_delta = sCurrTwccAvgStat->avg_delta_of_delta;
   gfloat offset = sCurrTwccAvgStat->avg_delta_of_delta/8;
   /*Get the mean of delta change of the current and previous rtp packet block*/
   gfloat T = (sPrevTwccAvgStat->avg_delta_of_delta_change + sCurrTwccAvgStat->avg_delta_of_delta_change )/2;
   XGLOG_TRACE ("threshold: %lf T: %lf Curr_avg_delta: %lld Curr_avg_delta-change: %lf",
               threshold, T, sCurrTwccAvgStat->avg_delta_of_delta, sCurrTwccAvgStat->avg_delta_of_delta_change);
   if (T > threshold) {
     if (time_over_using == -1) {
       // Initialize the timer. Assume that we've been
       // over-using half of the time since the previous
       // sample.
       time_over_using = ts_delta / 2;
     } else {
       // Increment timer
       time_over_using += ts_delta;
     }
     overuse_counter++;
     if (time_over_using > overusing_time_threshold && overuse_counter > 1) {
       if (offset >= prev_offset) {
         time_over_using = 0;
         overuse_counter = 0;
         XGLOG_TRACE ("IdentifyBandWidthUsage () EN_XGATE_BW_OVERUSING");
         m_band_usage = EN_XGATE_BW_OVERUSING;
       }
     }
   } else if (T < -threshold) {
     time_over_using = -1;
     overuse_counter = 0;
     XGLOG_TRACE ("IdentifyBandWidthUsage() EN_XGATE_BW_UNDERUSING");
     m_band_usage = EN_XGATE_BW_UNDERUSING;
   } else {
     time_over_using = -1;
     overuse_counter = 0;
     XGLOG_TRACE ("IdentifyBandWidthUsage () EN_XGATE_BW_NORMAL");
     m_band_usage = EN_XGATE_BW_NORMAL;
   }
   prev_offset = offset;

   return m_band_usage;

}

xGateLossLevel RateControl::DetectLoss(TWCCStats *sTwccAvgStat){

   if(sTwccAvgStat->packet_loss_pct < 2){
      //XGLOG_INFO("DetectLoss EN_XGATE_MINLOSS");
      return EN_XGATE_MINLOSS;
   }else if(sTwccAvgStat->packet_loss_pct >= 2 && sTwccAvgStat->packet_loss_pct < 10){
      //XGLOG_INFO("DetectLoss EN_XGATE_MEDIUMLOSS");
      return EN_XGATE_MEDIUMLOSS;
   }else if(sTwccAvgStat->packet_loss_pct >= 10){
      if(sTwccAvgStat->packet_loss_pct > 80){
         //XGLOG_WARN ("DetectLoss getting max Loss pct: %f and setting  EN_XGATE_MINLOSS",
         //             sTwccAvgStat->packet_loss_pct);
         return EN_XGATE_MINLOSS;
      }
      XGLOG_INFO("DetectLoss EN_XGATE_MAXLOSS packet_loss_pct %f",sTwccAvgStat->packet_loss_pct);
      return EN_XGATE_MAXLOSS;
   }
   return EN_XGATE_MINLOSS;
}
