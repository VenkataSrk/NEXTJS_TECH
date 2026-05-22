
//local include
#include "xglog.h"

//ace include
#include <ace/Task.h>

#include "xGateMGDispatcher.h"

//self include
#include "xGateHmpGstBin.h"

#define THISMODULE "HmpBin"

/* Client class implementation - Start
*/
Client::Client() : 
  m_pHmpVoiceRecvChannel(NULL), m_pHmpFrameRecvChannel(NULL), m_pHmpRecordAudioChannel(NULL),m_rec_started(0), 
  m_mp3_enc(0),m_rec_del(0),m_pHmpRecordMP3Channel(NULL),m_mgresourceId(""),m_callId("")
{
}

Client::Client(ClientDetail &detail) : 
  m_detail(detail), m_pHmpVoiceRecvChannel(NULL), m_pHmpFrameRecvChannel(NULL),
  m_pHmpRecordAudioChannel(NULL),m_rec_started(0),m_mp3_enc(0),m_rec_del(0),m_pHmpRecordMP3Channel(NULL) 
{
  m_playFile = "";
  m_mgresourceId="";
  m_callId="";
  m_clientCount = 0;
}

Client::~Client() 
{
  if(m_pHmpVoiceRecvChannel) {
    delete m_pHmpVoiceRecvChannel;
    m_pHmpVoiceRecvChannel  = NULL;
  }
  if(m_pHmpFrameRecvChannel) {
    delete m_pHmpFrameRecvChannel;
    m_pHmpFrameRecvChannel = NULL;
  }
  if(m_pHmpRecordAudioChannel) {
    delete m_pHmpRecordAudioChannel;
    m_pHmpRecordAudioChannel = NULL;
  }
  if(m_pHmpRecordMP3Channel){
    delete m_pHmpRecordMP3Channel;
    m_pHmpRecordMP3Channel = NULL;
  }
}

string Client::form_channel_name(string name)
{
  gchar channelName[CHANNEL_NAME_SIZE] = {0};
  snprintf(channelName, CHANNEL_NAME_SIZE, "%s_%s_%d", name.c_str(), m_detail.m_callId.c_str(), m_detail.audioDetail.m_codec);
  return channelName; 
}

string Client::form_videochannel_name(string name)
{
  gchar channelName[60] = {0};
  //sprintf(channelName, "%s_%d_%d_%d", name.c_str(), m_detail.m_legId, m_detail.m_codec, m_detail.m_clientPort);
  sprintf(channelName, "%s_%s_%d", name.c_str(), m_detail.m_callId.c_str(), m_detail.videoDetail.m_codec);
  return channelName;
}

gboolean Client::get_suitable_bin(HmpChannelType &channelType, HmpBinType &binType, string &binName)
{
  gchar binname[CHANNEL_NAME_SIZE] = {0};

  HmpMediaMode mediaModeIn = m_detail.m_mediaModeIn; 
	std::cout<<"m_detail.audioDetail.m_codec"<<m_detail.audioDetail.m_codec<<endl;
   unsigned int pt=m_detail.audioDetail.m_codec;
 
    switch(pt) {
    case EN_CODEC_PCMU:
      {
        if(channelType == EN_PIPELINE_VOICE_RECV) {
          if(mediaModeIn == EN_MEDIA_RTP) {
            binType = EN_BIN_G711U_RECV; 
          } else if(mediaModeIn == EN_MEDIA_DTLS) {
            binType = EN_BIN_DTLS_RECV;
          }
          //binName = form_channel_name("VoiceRecvBin");
          snprintf(binname, CHANNEL_NAME_SIZE, "%s_%s_%d","VoiceRecvBin", m_detail.m_callId.c_str(), m_detail.audioDetail.m_codec);
          binName = binname; 
        } else if(channelType == EN_PIPELINE_VOICE_SEND) {
          if(mediaModeIn == EN_MEDIA_RTP) {
            binType = EN_BIN_G711U_SEND; 
          } else if(mediaModeIn == EN_MEDIA_DTLS) {
            binType = EN_BIN_DTLS_SEND;
          }
          //binName = form_channel_name("VoiceSendBin");
          snprintf(binname, CHANNEL_NAME_SIZE, "%s_%s_%d","VoiceSendBin", m_detail.m_callId.c_str(), m_detail.audioDetail.m_codec);
          binName = binname;
        } else if(channelType == EN_PIPELINE_VOICE_DEC) {
          binType = EN_BIN_G711U_RECV; 
          //binName = form_channel_name("VoiceDecodeBin");
          snprintf(binname, CHANNEL_NAME_SIZE, "%s_%s_%d","VoiceDecodeBin", m_detail.m_callId.c_str(), m_detail.audioDetail.m_codec);
          binName = binname;
        } else if(channelType == EN_PIPELINE_VOICE_ENC) {
          binType = EN_BIN_G711U_SEND;
          //binName = form_channel_name("VoiceEncodeBin");
          snprintf(binname, CHANNEL_NAME_SIZE,"%s_%s_%d","VoiceEncodeBin", m_detail.m_callId.c_str(), m_detail.audioDetail.m_codec);
          binName = binname;
        }
        break;
      }
    case EN_CODEC_PCMA:
      {
        if(channelType == EN_PIPELINE_VOICE_RECV) {
          if(mediaModeIn == EN_MEDIA_RTP) {
            binType = EN_BIN_G711A_RECV; 
          } else if(mediaModeIn == EN_MEDIA_DTLS) {
            binType = EN_BIN_DTLS_RECV;
          }
          //binName = form_channel_name("VoiceRecvBin");
          snprintf(binname, CHANNEL_NAME_SIZE, "%s_%s_%d","VoiceRecvBin", m_detail.m_callId.c_str(), m_detail.audioDetail.m_codec);
          binName = binname;
        } else if(channelType == EN_PIPELINE_VOICE_SEND) {
          if(mediaModeIn == EN_MEDIA_RTP) {
            binType = EN_BIN_G711A_SEND; 
          } else if(mediaModeIn == EN_MEDIA_DTLS) {
            binType = EN_BIN_DTLS_SEND;
          }
          //binName = form_channel_name("VoiceSendBin");
          snprintf(binname, CHANNEL_NAME_SIZE, "%s_%s_%d","VoiceSendBin", m_detail.m_callId.c_str(), m_detail.audioDetail.m_codec);
          binName = binname;
        } else if(channelType == EN_PIPELINE_VOICE_DEC) {
          binType = EN_BIN_G711A_RECV; 
          //binName = form_channel_name("VoiceDecodeBin");
          snprintf(binname, CHANNEL_NAME_SIZE, "%s_%s_%d","VoiceDecodeBin", m_detail.m_callId.c_str(), m_detail.audioDetail.m_codec);
          binName = binname;
        } else if(channelType == EN_PIPELINE_VOICE_ENC) {
          binType = EN_BIN_G711A_SEND;
          //binName = form_channel_name("VoiceEncodeBin");
          snprintf(binname, CHANNEL_NAME_SIZE, "%s_%s_%d","VoiceEncodeBin", m_detail.m_callId.c_str(), m_detail.audioDetail.m_codec);
          binName = binname;
        }
        break;
      }
    case EN_CODEC_G722:
      {
        if(channelType == EN_PIPELINE_VOICE_RECV) {
          if(mediaModeIn == EN_MEDIA_RTP) {
            binType = EN_BIN_G722_RECV; 
          } else if(mediaModeIn == EN_MEDIA_DTLS) {
            binType = EN_BIN_DTLS_RECV;
          }
          //binName = form_channel_name("VoiceRecvBin");
          snprintf(binname, CHANNEL_NAME_SIZE, "%s_%s_%d","VoiceRecvBin", m_detail.m_callId.c_str(), m_detail.audioDetail.m_codec);
          binName = binname;
        } else if(channelType == EN_PIPELINE_VOICE_SEND) {
          if(mediaModeIn == EN_MEDIA_RTP) {
            binType = EN_BIN_G722_SEND; 
          } else if(mediaModeIn == EN_MEDIA_DTLS) {
            binType = EN_BIN_DTLS_SEND;
          }
          //binName = form_channel_name("VoiceSendBin");
          snprintf(binname, CHANNEL_NAME_SIZE, "%s_%s_%d","VoiceSendBin", m_detail.m_callId.c_str(), m_detail.audioDetail.m_codec);
          binName = binname;
        } else if(channelType == EN_PIPELINE_VOICE_DEC) {
          binType = EN_BIN_G722_RECV; 
          //binName = form_channel_name("VoiceDecodeBin");
          snprintf(binname, CHANNEL_NAME_SIZE, "%s_%s_%d","VoiceDecodeBin", m_detail.m_callId.c_str(), m_detail.audioDetail.m_codec);
          binName = binname;
        } else if(channelType == EN_PIPELINE_VOICE_ENC) {
          binType = EN_BIN_G722_SEND;
          //binName = form_channel_name("VoiceEncodeBin");
          snprintf(binname, CHANNEL_NAME_SIZE, "%s_%s_%d","VoiceEncodeBin", m_detail.m_callId.c_str(), m_detail.audioDetail.m_codec);
          binName = binname;
        }
        break;
      }
    case EN_CODEC_G729:
      {
        if(channelType == EN_PIPELINE_VOICE_RECV) {
          if(mediaModeIn == EN_MEDIA_RTP) {
            binType = EN_BIN_G729_RECV; 
          } else if(mediaModeIn == EN_MEDIA_DTLS) {
            binType = EN_BIN_DTLS_RECV;
          }
          //binName = form_channel_name("VoiceRecvBin");
          snprintf(binname, CHANNEL_NAME_SIZE, "%s_%s_%d","VoiceRecvBin", m_detail.m_callId.c_str(), m_detail.audioDetail.m_codec);
          binName = binname;
        } else if(channelType == EN_PIPELINE_VOICE_SEND) {
          if(mediaModeIn == EN_MEDIA_RTP) {
            binType = EN_BIN_G729_SEND; 
          } else if(mediaModeIn == EN_MEDIA_DTLS) {
            binType = EN_BIN_DTLS_SEND;
          }
          //binName = form_channel_name("VoiceSendBin");
          snprintf(binname, CHANNEL_NAME_SIZE, "%s_%s_%d","VoiceSendBin", m_detail.m_callId.c_str(), m_detail.audioDetail.m_codec);
          binName = binname;
        } else if(channelType == EN_PIPELINE_VOICE_DEC) {
          binType = EN_BIN_G729_RECV; 
          //binName = form_channel_name("VoiceDecodeBin");
          snprintf(binname, CHANNEL_NAME_SIZE, "%s_%s_%d","VoiceDecodeBin", m_detail.m_callId.c_str(), m_detail.audioDetail.m_codec);
          binName = binname;
        } else if(channelType == EN_PIPELINE_VOICE_ENC) {
          binType = EN_BIN_G729_SEND;
          //binName = form_channel_name("VoiceEncodeBin");
          snprintf(binname, CHANNEL_NAME_SIZE, "%s_%s_%d","VoiceEncodeBin", m_detail.m_callId.c_str(), m_detail.audioDetail.m_codec);
          binName = binname;
        }
        break;
      }
    case 96 ... 127:
      {
        cout<<"m_detail.audioDetail.m_codec"<<m_detail.audioDetail.m_codec<<endl;
        if(channelType == EN_PIPELINE_VOICE_RECV) {
          if(mediaModeIn == EN_MEDIA_RTP) {
            binType = EN_BIN_OPUS_RECV; 
          } else if(mediaModeIn == EN_MEDIA_DTLS) {
            binType = EN_BIN_DTLS_RECV;
          }
          //binName = form_channel_name("VoiceRecvBin");
          snprintf(binname, CHANNEL_NAME_SIZE, "%s_%s_%d","VoiceRecvBin", m_detail.m_callId.c_str(), m_detail.audioDetail.m_codec);
          binName = binname;
        } else if(channelType == EN_PIPELINE_VOICE_SEND) {
          if(mediaModeIn == EN_MEDIA_RTP) {
            binType = EN_BIN_OPUS_SEND; 
          } else if(mediaModeIn == EN_MEDIA_DTLS) {
            binType = EN_BIN_DTLS_SEND;
          }
          //binName = form_channel_name("VoiceSendBin");
          snprintf(binname, CHANNEL_NAME_SIZE, "%s_%s_%d","VoiceSendBin", m_detail.m_callId.c_str(), m_detail.audioDetail.m_codec);
          binName = binname;
        } else if(channelType == EN_PIPELINE_VOICE_DEC) {
          binType = EN_BIN_OPUS_RECV; 
          //binName = form_channel_name("VoiceDecodeBin");
          snprintf(binname, CHANNEL_NAME_SIZE, "%s_%s_%d","VoiceDecodeBin", m_detail.m_callId.c_str(), m_detail.audioDetail.m_codec);
          binName = binname;
        } else if(channelType == EN_PIPELINE_VOICE_ENC) {
          binType = EN_BIN_OPUS_SEND;
          //binName = form_channel_name("VoiceEncodeBin");
          snprintf(binname, CHANNEL_NAME_SIZE, "%s_%s_%d","VoiceEncodeBin", m_detail.m_callId.c_str(), m_detail.audioDetail.m_codec);
          binName = binname;
        }
        break;
      }
    case EN_CODEC_UNKNOWN:
    default:
      {
        XGLOG_ERROR("get_suitable_bin failed, unknown/unsupported codec:%d type for client:%s:%d !!", \
            m_detail.audioDetail.m_codec, (char*)ur_log_string(m_detail.audioDetail.m_clientIp), m_detail.audioDetail.m_clientPort);
        return FALSE;
      }
  }
  return TRUE; 
}

gboolean Client::get_suitable_videobin(HmpChannelType &channelType, HmpVideoBinType &binType, string &binName)
{
  HmpMediaMode mediaModeIn = m_detail.m_mediaModeIn;
  string codecname = m_detail.videoDetail.m_codecName;
  HmpCodecType codecType = EN_CODEC_UNKNOWN;
  if (codecname == "VP9") {
    codecType = EN_CODEC_VP9;
  } else if (codecname == "VP8") {
    codecType = EN_CODEC_VP8;
  } else if (codecname == "H264") {
    codecType = EN_CODEC_H264;
  } else if (codecname == "H265") {
    codecType = EN_CODEC_H265;
  }

  //switch(m_detail.videoDetail.m_codec) {
  switch(codecType) {
    case EN_CODEC_H265:
      {
        if(channelType == EN_PIPELINE_FRAME_RECV) {
          if(mediaModeIn == EN_MEDIA_RTP) { 
            binType = EN_BIN_VID_H265_RECV;
          } else if(mediaModeIn == EN_MEDIA_DTLS) {
            binType = EN_BIN_VID_DTLS_RECV;
          }
          binName = form_videochannel_name("FrameH265RecvBin");
        } else if(channelType == EN_PIPELINE_FRAME_SEND) {
          if(mediaModeIn == EN_MEDIA_RTP) {
            binType = EN_BIN_VID_H265_SEND;
          } else if(mediaModeIn == EN_MEDIA_DTLS) {
            binType = EN_BIN_VID_DTLS_SEND;
          }
          binName = form_videochannel_name("Frameh265SendBin");
        } else if(channelType == EN_PIPELINE_FRAME_DEC) {
          binType = EN_BIN_VID_H265_RECV;
          binName = form_videochannel_name("FrameH265DecodeBin");
        } else if(channelType == EN_PIPELINE_FRAME_ENC) {
          binType = EN_BIN_VID_H265_SEND;
          binName = form_videochannel_name("FrameH265EncodeBin");
        }
        break;
      }
    case EN_CODEC_H264:
      {
        if(channelType == EN_PIPELINE_FRAME_RECV) {
          if(mediaModeIn == EN_MEDIA_RTP) {
            binType = EN_BIN_VID_H264_RECV;
          } else if(mediaModeIn == EN_MEDIA_DTLS) {
            binType = EN_BIN_VID_DTLS_RECV;
          }
          binName = form_videochannel_name("FrameH264RecvBin");
        } else if(channelType == EN_PIPELINE_FRAME_SEND) {
          if(mediaModeIn == EN_MEDIA_RTP) {
            binType = EN_BIN_VID_H264_SEND;
          } else if(mediaModeIn == EN_MEDIA_DTLS) {
            binType = EN_BIN_VID_DTLS_SEND;
          }
          binName = form_videochannel_name("FrameH264SendBin");
        } else if(channelType == EN_PIPELINE_FRAME_DEC) {
          binType = EN_BIN_VID_H264_RECV;
          binName = form_videochannel_name("FrameH264DecodeBin");
        } else if(channelType == EN_PIPELINE_FRAME_ENC) {
          binType = EN_BIN_VID_H264_SEND;
          binName = form_videochannel_name("FrameH264EncodeBin");
        }
        break;
      }
    case EN_CODEC_VP9:
      {
        if(channelType == EN_PIPELINE_FRAME_RECV) {
          if(mediaModeIn == EN_MEDIA_RTP) {
            binType = EN_BIN_VID_VP9_RECV;
          } else if(mediaModeIn == EN_MEDIA_DTLS) {
            binType = EN_BIN_VID_DTLS_RECV;
          }
          binName = form_videochannel_name("FrameVp9RecvBin");
        } else if(channelType == EN_PIPELINE_FRAME_SEND) {
          if(mediaModeIn == EN_MEDIA_RTP) {
            binType = EN_BIN_VID_VP9_SEND;
          } else if(mediaModeIn == EN_MEDIA_DTLS) {
            binType = EN_BIN_VID_DTLS_SEND;
          }
          binName = form_videochannel_name("FrameVp9SendBin");
        } else if(channelType == EN_PIPELINE_FRAME_DEC) {
          binType = EN_BIN_VID_VP9_RECV;
          binName = form_videochannel_name("FrameVP9DecodeBin");
        } else if(channelType == EN_PIPELINE_FRAME_ENC) {
          binType = EN_BIN_VID_VP9_SEND;
          binName = form_videochannel_name("FrameVP9EncodeBin");
        }
        break;
      }
    case EN_CODEC_VP8:
      {
        if(channelType == EN_PIPELINE_FRAME_RECV) {
          if(mediaModeIn == EN_MEDIA_RTP) {
            binType = EN_BIN_VID_VP8_RECV;
          } else if(mediaModeIn == EN_MEDIA_DTLS) {
            binType = EN_BIN_VID_DTLS_RECV; 
          }
          binName = form_videochannel_name("FrameVp8RecvBin");
        } else if(channelType == EN_PIPELINE_FRAME_SEND) {
          if(mediaModeIn == EN_MEDIA_RTP) {
            binType = EN_BIN_VID_VP8_SEND;
          } else if(mediaModeIn == EN_MEDIA_DTLS) {
            binType = EN_BIN_VID_DTLS_SEND;
          }
          binName = form_videochannel_name("FrameVp8SendBin");
        } else if(channelType == EN_PIPELINE_FRAME_DEC) {
          binType = EN_BIN_VID_VP8_RECV;
          binName = form_videochannel_name("FrameVP8DecodeBin");
        } else if(channelType == EN_PIPELINE_FRAME_ENC) {
          binType = EN_BIN_VID_VP8_SEND;
          binName = form_videochannel_name("FrameVP8EncodeBin");
        }
        break;
      }
    case EN_CODEC_UNKNOWN:
    default:
      {
        XGLOG_ERROR("get_suitable_videobin failed, unknown/unsupported codec:%d type for client:%s:%d !!", \
            m_detail.videoDetail.m_codec, (char*)ur_log_string(m_detail.videoDetail.m_clientIp), m_detail.videoDetail.m_clientPort);
        return FALSE;
      }
  }
  return TRUE;
}

void  Client::post_message_to_HmpProcessor(bool conversion_status)
{
       MgMediaDetail m_media_detail;
       xGateMgMsg* pMgMsg = NULL;

       ACE_Task<ACE_MT_SYNCH>* taskptr = NULL;
       taskptr  =  xGateUtil::getHmpProcessor();
       pMgMsg = new xGateMgMsg();
       if(!taskptr){
         XGLOG_ERROR("handle_event m_HmpProcessor not found");
         return;
       }
       if(!pMgMsg) {
         XGLOG_ERROR("handle_event xgateMgMsg creation failed");
         return;
       }
       pMgMsg->set_mg_msg_type(EN_XGATE_MG_MP3_DETAIL);

       m_media_detail.call_id=m_callId;
       m_media_detail.mediaResource_id=m_mgresourceId;
       if(! m_pHmpRecordMP3Channel->m_pHmpRecordMP3Bin->m_mp3File.empty()){
       m_media_detail.mp3_filesize=xGateHmpGstUtil::file_size(m_pHmpRecordMP3Channel->m_pHmpRecordMP3Bin->m_mp3File);
       }
       else{
       m_media_detail.mp3_filesize=0;
       XGLOG_INFO("Client::post_message_to_HmpProcessor,cannot find size of m_mp3File ,m_mp3File is empty for %s",(char*)ur_log_string(m_callId));
       }
       if( !m_pHmpRecordMP3Channel->m_pHmpRecordMP3Bin->m_filePath1.empty()){
       m_media_detail.mp3_duration=xGateHmpGstUtil::find_wav_duration(m_pHmpRecordMP3Channel->m_pHmpRecordMP3Bin->m_filePath1);
       }
       else{
       m_media_detail.mp3_duration=0;
       XGLOG_INFO("Client::post_message_to_HmpProcessor,cannot find mp3_duration ,m_filePath1 is empty for %s",(char*)ur_log_string(m_callId));
        }
        m_media_detail.isRecordingStatus=conversion_status;
      // m_isRecordingStatus=conversion_status;
       pMgMsg->set_media_detail(m_media_detail);

       ACE_Message_Block* pAmb = NULL;
       pAmb = static_cast<xGateMgMsg*> (pMgMsg);
       ACE_Time_Value tval ((time(NULL) ));
       tval += ACE_Time_Value(0,1);
       if (taskptr->putq(pAmb, &tval)) {
        XGLOG_INFO("handle_event postMessageToHmpProcessor enqueue message success ");
        } else {
        XGLOG_ERROR("handle_event PostMessageToHmpProcessor  enqueue message failed");
        delete pMgMsg;
        return;
        }
}


gboolean Client::get_suitable_channel(HmpChannelType &channelType, string &channelName, HmpBinType &binType, string &binName)
{
  gchar channelname[CHANNEL_NAME_SIZE] = {0};
  switch(channelType) {

    case EN_PIPELINE_VOICE_RECV:
      {
        //channelName = form_channel_name("VoiceRecvChannel");
        snprintf(channelname, CHANNEL_NAME_SIZE, "%s_%s_%d","VoiceRecvChannel", m_detail.m_callId.c_str(), m_detail.audioDetail.m_codec);
        channelName = channelname;
        return get_suitable_bin(channelType, binType, binName);
      }
    case EN_PIPELINE_VOICE_SEND:
      {
        //channelName = form_channel_name("VoiceSendChannel");
        snprintf(channelname, CHANNEL_NAME_SIZE, "%s_%s_%d","VoiceSendChannel", m_detail.m_callId.c_str(), m_detail.audioDetail.m_codec);
        channelName = channelname;
        return get_suitable_bin(channelType, binType, binName);
      }
    case EN_PIPELINE_VOICE_DEC: 
      {
        //channelName = form_channel_name("VoiceDecodeChannel");
        snprintf(channelname, CHANNEL_NAME_SIZE, "%s_%s_%d","VoiceDecodeChannel", m_detail.m_callId.c_str(), m_detail.audioDetail.m_codec);
        channelName = channelname;
        return get_suitable_bin(channelType, binType, binName);
      }
    case EN_PIPELINE_PLAY_AUDIO:
      {
        //channelName = form_channel_name("PlayAudioChannel");
        snprintf(channelname, CHANNEL_NAME_SIZE, "%s_%s_%d","PlayAudioChannel", m_detail.m_callId.c_str(), m_detail.audioDetail.m_codec);
        channelName = channelname;
        binName = form_channel_name("PlayAudioBin");
        binType = EN_BIN_PLAY_AUDIO; 
        return TRUE; 
      }
    case EN_PIPELINE_RECORD_AUDIO:
      {
        //channelName = form_channel_name("RecordAudioChannel");
        snprintf(channelname, CHANNEL_NAME_SIZE, "%s_%s_%d","RecordAudioChannel", m_detail.m_callId.c_str(), m_detail.audioDetail.m_codec);
        channelName = channelname;
        binName = form_channel_name("RecordAudioBin");
        binType = EN_BIN_RECORD_AUDIO; 
        return TRUE;
      }
    case EN_PIPELINE_RECORD_MP3:
     {
       snprintf(channelname, CHANNEL_NAME_SIZE, "%s_%s_%d","RecordMP3Channel", m_detail.m_callId.c_str(), m_detail.audioDetail.m_codec);
       channelName = channelname;
       binName = form_channel_name("RecordMP3Bin");
       binType = EN_BIN_RECORD_MP3;
      return TRUE;
     }
    default:
    case EN_PIPELINE_UNKNOWN:
      {
        XGLOG_ERROR("get_suitable_channel failed, unknown channel type for client:%s:%d !!", \
            (char*)ur_log_string(m_detail.audioDetail.m_clientIp), m_detail.audioDetail.m_clientPort);
        break;
      }
  }
  return FALSE;
}

gboolean Client::get_suitable_videochannel(HmpChannelType &channelType, string &channelName, HmpVideoBinType &vbinType, string &binName)
{
  switch(channelType) {
    case EN_PIPELINE_FRAME_RECV:
      {
        channelName = form_channel_name("FrameRecvChannel");
        return get_suitable_videobin(channelType, vbinType, binName);
      }
    case EN_PIPELINE_FRAME_SEND:
      {
        channelName = form_channel_name("FrameSendChannel");
        return get_suitable_videobin(channelType, vbinType, binName);
      }
    case EN_PIPELINE_FRAME_DEC:
      {
        channelName = form_channel_name("FrameDecodeChannel");
        return get_suitable_videobin(channelType, vbinType, binName);
      }
    case EN_PIPELINE_FRAME_ENC:
      {
        channelName = form_channel_name("FrameEncodeChannel");
        return get_suitable_videobin(channelType, vbinType, binName);
      }
    default:
    case EN_PIPELINE_UNKNOWN:
      {
        XGLOG_ERROR("get_suitable_video_channel failed, unknown channel type for client:%s:%d !!", \
            (char*)ur_log_string(m_detail.videoDetail.m_clientIp), m_detail.videoDetail.m_clientPort);
        break;
      }
  }
  return FALSE;
}

//gboolean Client::create_audio_channel(gint audioPort, GSocket *audioSock)
gboolean Client::create_audio_channel()
{
  XGLOG_INFO( "Client::create_audio_channel for call_id: %s and client: %s:%d start", \
      (char*)ur_log_string(m_detail.m_callId), (char*)ur_log_string(m_detail.audioDetail.m_clientIp), m_detail.audioDetail.m_clientPort);
  gint audioPort = 0;
  GSocket *audioSocket = NULL;

  while(!audioSocket) {
     audioPort = HOSTGST_GET_AUDIO_PORT();
     audioSocket = xGateHmpGstUtil::create_udpsocket(audioPort);
  }

  m_detail.audioDetail.m_serverPort = audioPort;
  m_detail.audioDetail.m_serverIp = xGateUtil::getLocalIpAddr();

  XGLOG_INFO( "Client::create_audio_channel for client: %s:",(char*)ur_log_string(m_detail.audioDetail.m_serverIp));
  //creating VoiceChannel
  if(!create_voice_channel(audioPort, audioSocket)) {
    XGLOG_ERROR("create_audio_channel, creating voice channel for call_id:%s failed!!", \
        (char*)ur_log_string(m_detail.m_callId));
    return FALSE;
  }
 
  //TODO: creating RecordAudioChannel
  if(!create_record_audio_channel()) {
    XGLOG_ERROR("create_audio_channel, record audio channel for call_id:%s failed!!", \
        (char*)ur_log_string(m_detail.m_callId));
    return FALSE;
  } else {
  }
  //Create MP3 Record channel 
  if(!create_record_mp3_channel()){
    XGLOG_ERROR("create_audio_channel, record audio channel for call_id:%s failed!!", \
        (char*)ur_log_string(m_detail.m_callId));
    return FALSE;
  }
  return TRUE;
}

gboolean Client::create_record_mp3_channel(){
  string channelName(""), binName("");
  HmpChannelType channelType = EN_PIPELINE_RECORD_MP3; 
  HmpBinType binType = EN_BIN_RECORD_MP3; 
     if(!m_pHmpRecordMP3Channel) {
        if(get_suitable_channel(channelType, channelName, binType, binName)) {
          m_pHmpRecordMP3Channel = new HmpRecordMP3Channel(channelName, channelType); 
          if(!m_pHmpRecordMP3Channel->construct_bin(binName, binType)) {
            XGLOG_ERROR("create_record_mp3_channel for client:%s:%d failed!!", \
                (char*)ur_log_string(m_detail.audioDetail.m_clientIp), m_detail.audioDetail.m_clientPort);
            return FALSE;
          } else {
            //on successfull channel creation register the handle_event callback
            m_pHmpRecordMP3Channel->register_handle_mp3_event_callback(this);
          }
        } else {
          XGLOG_ERROR("create_record_mp3_channel, failed to get suitable play audio channel for codec:mp3 and client:%s:%d !!", \
             (char*)ur_log_string(m_detail.audioDetail.m_clientIp), m_detail.audioDetail.m_clientPort);
          return FALSE;
        }
      } else {
        XGLOG_ERROR("create_record_mp3_channel for client:%s:%d already been created!!", \
            (char*)ur_log_string(m_detail.audioDetail.m_clientIp), m_detail.audioDetail.m_clientPort);
        return FALSE;
      }

  //start_voice_channel();
}

gboolean Client::create_voice_recv_channel(gint audioPort, GSocket *audioSock)
{
  string channelName(""), binName("");
  HmpChannelType channelType = EN_PIPELINE_VOICE_RECV; 
  HmpBinType binType = EN_BIN_UNKNOWN; 
  HmpCodecType codecType = m_detail.audioDetail.m_codec;
  gboolean retVal = FALSE;

  if(!m_pHmpVoiceRecvChannel) {
    if(get_suitable_channel(channelType, channelName, binType, binName)) {
      m_pHmpVoiceRecvChannel = new HmpVoiceRecvChannel(channelName, channelType); 
      if(!m_pHmpVoiceRecvChannel->construct_bin(binName, binType, codecType, audioPort, audioSock)) {
        XGLOG_ERROR("create_voice_recv_channel, construct voice recv bin failed for codec:%d and client:%s:%d !!", \
            m_detail.audioDetail.m_codec, (char*)ur_log_string(m_detail.audioDetail.m_clientIp), m_detail.audioDetail.m_clientPort);
        return FALSE;
      }  
        m_detail.audioDetail.m_serverIp = xGateUtil::getLocalIpAddr(); 
        m_detail.audioDetail.m_serverPort = m_pHmpVoiceRecvChannel->get_recv_port();
      } else {
        XGLOG_ERROR("create_voice_recv_channel failed to get suitable channel for codec:%d and client:%s:%d !!", \
            m_detail.audioDetail.m_codec, (char*)ur_log_string(m_detail.audioDetail.m_clientIp), m_detail.audioDetail.m_clientPort);
        return FALSE;
      }
    } else {
      XGLOG_ERROR("create_voice_recv_channel for codec:%d and client:%s:%d already been created!!", \
          m_detail.audioDetail.m_codec, (char*)ur_log_string(m_detail.audioDetail.m_clientIp), m_detail.audioDetail.m_clientPort);
      return FALSE;
    }
    XGLOG_INFO( "create_voice_recv_channel for codec:%d and client:%s:%d success", \
        m_detail.audioDetail.m_codec, (char*)ur_log_string(m_detail.audioDetail.m_clientIp), m_detail.audioDetail.m_clientPort);
    return TRUE;
  }

    gboolean Client::create_video_channel(gint videoPort, GSocket *videoSock)
    {
      XGLOG_INFO( "Client::create_video_channel for call_id: %s and client: %s:%d start", \
          m_detail.m_callId.c_str(), (char*)ur_log_string(m_detail.videoDetail.m_clientIp), m_detail.videoDetail.m_clientPort);

      /*creating frame Channel */
      if(!create_frame_channel(videoPort, videoSock)) {
        XGLOG_ERROR("create_video_channel, creating frame channel for call_id:%s failed!!", \
            (char*)ur_log_string(m_detail.m_callId));
        return FALSE;
      } 
      start_frame_channel();
    }


    gboolean Client::create_frame_recv_channel(gint videoPort, GSocket *videoSock)
    {
      string channelName(""), binName("");
      HmpChannelType channelType = EN_PIPELINE_FRAME_RECV;
      HmpVideoBinType vbinType = EN_BIN_VID_UNKNOWN;
      gboolean retVal = FALSE;

      if(!m_pHmpFrameRecvChannel) {
        if(get_suitable_videochannel(channelType, channelName, vbinType, binName)) {
          m_pHmpFrameRecvChannel = new HmpFrameRecvChannel(channelName, channelType);
          if(!m_pHmpFrameRecvChannel->construct_bin(binName, vbinType, videoPort, videoSock)) {
            XGLOG_ERROR("create_frame_recv_channel, construct frame recv bin failed for codec:%d and client:%s:%d !!", \
                m_detail.videoDetail.m_codec, (char*)ur_log_string(m_detail.videoDetail.m_clientIp), m_detail.videoDetail.m_clientPort);
            return FALSE;
          } else {
            m_detail.videoDetail.m_serverIp = xGateUtil::getLocalIpAddr();
            m_detail.videoDetail.m_serverPort = m_pHmpFrameRecvChannel->get_recv_port();
          }
        } else{
          XGLOG_ERROR("create_frame_recv_channel failed to get suitable channel for codec:%d and client:%s:%d !!", \
              m_detail.videoDetail.m_codec, (char*)ur_log_string(m_detail.videoDetail.m_clientIp), m_detail.videoDetail.m_clientPort);
          return FALSE;
        }
      } else {
        XGLOG_ERROR("create_frame_recv_channel for codec:%d and client:%s:%d already been created!!", \
            m_detail.videoDetail.m_codec, (char*)ur_log_string(m_detail.videoDetail.m_clientIp), m_detail.videoDetail.m_clientPort);
        return FALSE;
      }
      XGLOG_INFO( "create_frame_recv_channel for codec:%d and client:%s:%d success", \
          m_detail.videoDetail.m_codec, (char*)ur_log_string(m_detail.videoDetail.m_clientIp), m_detail.videoDetail.m_clientPort);
      return TRUE;
    }

    gboolean Client::create_voice_channel(gint audioPort, GSocket *audioSock)
    {
      //creating VoiceRecvChannel
      if(!create_voice_recv_channel(audioPort, audioSock)) {
        XGLOG_ERROR("create_voice_channel, creating voice recv channel for codec:%d and client:%s:%d failed!!", \
            m_detail.audioDetail.m_codec, (char*)ur_log_string(m_detail.audioDetail.m_clientIp), m_detail.audioDetail.m_clientPort);
        return FALSE;
      }
      XGLOG_INFO( "create_voice_channel for codec:%d and client:%s:%d success", \
          m_detail.audioDetail.m_codec, (char*)ur_log_string(m_detail.audioDetail.m_clientIp), m_detail.audioDetail.m_clientPort);

      return TRUE;
    }

    gboolean Client::create_frame_channel(gint videoPort, GSocket *videoSock)
    {
      /* creating FrameRecvChannel */
      if(!create_frame_recv_channel(videoPort, videoSock)) {
        XGLOG_ERROR("create_frame_channel, creating frame recv channel for codec:%d and client:%s:%d failed!!", \
            m_detail.videoDetail.m_codec, (char*)ur_log_string(m_detail.videoDetail.m_clientIp), m_detail.videoDetail.m_clientPort);
        return FALSE;
      }
      //TODO: In case of video conference 
      XGLOG_INFO( "create_frame_channel for codec:%d and client:%s:%d success", \
          m_detail.videoDetail.m_codec, (char*)ur_log_string(m_detail.videoDetail.m_clientIp), m_detail.videoDetail.m_clientPort);
      return TRUE;
    }

    gboolean Client::create_record_audio_channel()
    {
      string channelName(""), binName("");
      HmpChannelType channelType = EN_PIPELINE_RECORD_AUDIO; 
      HmpBinType binType = EN_BIN_RECORD_AUDIO; 
      HmpCodecType codecType = m_detail.audioDetail.m_codec;

      if(!m_pHmpRecordAudioChannel) {
        if(get_suitable_channel(channelType, channelName, binType, binName)) {
          m_pHmpRecordAudioChannel = new HmpRecordAudioChannel(channelName, channelType); 
          if(!m_pHmpRecordAudioChannel->construct_bin(binName, binType,codecType)) {
            XGLOG_ERROR("create_record_audio_channel for client:%s:%d failed!!", \
                (char*)ur_log_string(m_detail.audioDetail.m_clientIp), m_detail.audioDetail.m_clientPort);
            return FALSE;
          } else {
            //on successfull channel creation register the handle_event callback
             m_pHmpRecordAudioChannel->register_handle_wav_event_callback(this);
          }
        } else {
          XGLOG_ERROR("create_record_audio_channel, failed to get suitable play audio channel for codec:%d and client:%s:%d !!", \
              m_detail.audioDetail.m_codec, (char*)ur_log_string(m_detail.audioDetail.m_clientIp), m_detail.audioDetail.m_clientPort);
          return FALSE;
        }
      } else {
        XGLOG_ERROR("create_record_audio_channel for client:%s:%d already been created!!", \
            (char*)ur_log_string(m_detail.audioDetail.m_clientIp), m_detail.audioDetail.m_clientPort);
        return FALSE;
      }
      return TRUE;
    }

    gboolean Client::start_voice_channel()
    {
      XGLOG_INFO( "Client::start_voice_channel for call_id: %s and client: %s:%d start", \
          (char*)ur_log_string(m_detail.m_callId), (char*)ur_log_string(m_detail.audioDetail.m_clientIp), m_detail.audioDetail.m_clientPort);
      gboolean retVal = FALSE;
      if(m_pHmpVoiceRecvChannel) {
        retVal = m_pHmpVoiceRecvChannel->start();
      }
      return retVal;
    }

    gboolean Client::start_frame_channel()
    {
      XGLOG_INFO( "Client::start_video_channel for call_id: %s and client: %s:%d start", \
          (char*)ur_log_string(m_detail.m_callId), (char*)ur_log_string(m_detail.videoDetail.m_clientIp), m_detail.videoDetail.m_clientPort);
      gboolean retVal = FALSE;
      if(m_pHmpFrameRecvChannel) {
        retVal = m_pHmpFrameRecvChannel->start();
      }
      return retVal;
    }

    gboolean Client::stop_voice_channel()
    {
      XGLOG_INFO( "Client::stop_voice_channel for call_id: %s and client: %s:%d start", \
          (char*)ur_log_string(m_detail.m_callId), (char*)ur_log_string(m_detail.audioDetail.m_clientIp), m_detail.audioDetail.m_clientPort);
      gboolean retVal = FALSE;
      if(m_pHmpVoiceRecvChannel) {
        retVal = m_pHmpVoiceRecvChannel->stop();
      }
      return retVal;
    }

    gboolean Client::stop_frame_channel()
    {
      XGLOG_INFO( "Client::stop_frame_channel for call_id: %s and client: %s:%d start", \
          (char*)ur_log_string(m_detail.m_callId), (char*)ur_log_string(m_detail.videoDetail.m_clientIp), m_detail.videoDetail.m_clientPort);
      gboolean retVal = FALSE;
      if(m_pHmpFrameRecvChannel) {
        retVal = m_pHmpFrameRecvChannel->stop();
      }
      return retVal;
    }

    gboolean Client::set_record_attributes(string &recordFile)
    {
      XGLOG_INFO( "Client::set_record_attributes for client: %s:%d start", \
          (char*)ur_log_string(m_detail.audioDetail.m_clientIp), m_detail.audioDetail.m_clientPort);
      gboolean retVal = FALSE;
      if(m_pHmpRecordAudioChannel && !recordFile.empty()) {
        XGLOG_INFO( "Client::set_record_attributes for client: %s ", (char*)ur_log_string(recordFile));
        retVal = m_pHmpRecordAudioChannel->set_record_file(recordFile);
      }
      return retVal; 
    }

    //gboolean Client::add_receive_channel(Client *client)
    gboolean Client::add_receive_channel()
    {
      XGLOG_INFO( "Client::add_receive_channel for client: %s:%d and callId:%s start", \
          (char*)ur_log_string(m_detail.audioDetail.m_clientIp), m_detail.audioDetail.m_clientPort, (char*)ur_log_string(m_detail.m_callId));

      gboolean retVal = FALSE;
      HmpVoiceRecvBin *hmpVoiceRecvBin = NULL;
      HmpRecordAudioBin *hmpRecordAudioBin = NULL;

      hmpVoiceRecvBin = m_pHmpVoiceRecvChannel->m_pHmpVoiceRecvBin;

      if(!m_pHmpRecordAudioChannel) {
        XGLOG_ERROR("Client::add_receive_channel failed. record audio channel not created for client: %s:%d and callId: %s !!", \
            (char*)ur_log_string(m_detail.audioDetail.m_clientIp), m_detail.audioDetail.m_clientPort, (char*)ur_log_string(m_detail.m_callId));
        return FALSE;
      }
      hmpRecordAudioBin = m_pHmpRecordAudioChannel->m_pHmpRecordAudioBin;

      gst_bin_add_many(GST_BIN(m_pHmpRecordAudioChannel->m_pipeline), GST_ELEMENT(hmpVoiceRecvBin->m_bin), NULL);
      gst_element_link_many(GST_ELEMENT(hmpVoiceRecvBin->m_bin), GST_ELEMENT(hmpRecordAudioBin->m_bin), NULL);

      XGLOG_INFO( "Client::add_receive_channel for client: %s:%d and leg: %s success", \
          (char*)ur_log_string(m_detail.audioDetail.m_clientIp), m_detail.audioDetail.m_clientPort, (char*)ur_log_string(m_detail.m_callId));
      return TRUE;
    }

    gboolean Client::remove_receive_channel()
    {
      XGLOG_INFO( "Client::remove_receive_channel for client: %s:%d and callId:%s start", \
          (char*)ur_log_string(m_detail.audioDetail.m_clientIp), m_detail.audioDetail.m_clientPort, (char*)ur_log_string(m_detail.m_callId));

      HmpVoiceRecvBin *hmpVoiceRecvBin = NULL;
      HmpRecordAudioBin *hmpRecordAudioBin = NULL;

      hmpVoiceRecvBin = m_pHmpVoiceRecvChannel->m_pHmpVoiceRecvBin;

      if(!m_pHmpRecordAudioChannel) {
        XGLOG_ERROR("Client::remove_receive_channel failed. record audio channel not created for client: %s:%d and callId: %s !!", \
            (char*)ur_log_string(m_detail.audioDetail.m_clientIp), m_detail.audioDetail.m_clientPort, (char*)ur_log_string(m_detail.m_callId));
        return FALSE;
      }
      hmpRecordAudioBin = m_pHmpRecordAudioChannel->m_pHmpRecordAudioBin;

      GstPad *srcpad, *sinkpad = NULL;
      srcpad = hmpVoiceRecvBin->m_ghostRtpSrcPad;
      sinkpad = hmpRecordAudioBin->m_ghostRtpSinkPad;

      if(srcpad && sinkpad){
	 if(gst_pad_unlink(srcpad,sinkpad)){
            XGLOG_INFO( "Unlinking Recv channel and record audio channel successfully");
	    gst_element_remove_pad(GST_ELEMENT(hmpVoiceRecvBin->m_bin), srcpad);
	    gst_element_remove_pad(GST_ELEMENT(hmpRecordAudioBin->m_bin), sinkpad);
	 }
      }

      gst_element_unlink_many(GST_ELEMENT(hmpVoiceRecvBin->m_bin), GST_ELEMENT(hmpRecordAudioBin->m_bin), NULL);
      //gst_bin_remove_many(GST_BIN(m_pHmpRecordAudioChannel->m_pipeline), GST_ELEMENT(hmpVoiceRecvBin->m_bin), NULL);

      XGLOG_INFO( "Client::remove_receive_channel for client: %s:%d and leg: %s success", \
          (char*)ur_log_string(m_detail.audioDetail.m_clientIp), m_detail.audioDetail.m_clientPort, (char*)ur_log_string(m_detail.m_callId));
      return TRUE;
    }

    gboolean Client::set_media_event_details(xGateMediaEvent mediaEvent, xGateMgMsg* pMgMsg)
    {
      MgMediaDetail &mediaDetail = pMgMsg->get_media_detail();
      mediaDetail.rbtState = mediaEvent;
      switch(mediaEvent) {

        case EN_MEDIA_EVENT_EOS:
          {
            pMgMsg->set_mg_msg_type(EN_XGATE_MG_PLAY_BACK);
            break;
          }
        case EN_MEDIA_EVENT_RECORD_INFO:
          {
            pMgMsg->set_mg_msg_type(EN_XGATE_MG_RELEASE_RECORD);
            mediaDetail.file_size = m_detail.m_fileSize;
            mediaDetail.record_file = m_detail.m_filePath;
            break;
          }
        case EN_MEDIA_EVENT_NULL:
        default:
          {
            break;
          }
      }
#if 1 //TODO: we can optimize below code blocks by making ClientDetails and MgMediaDetail make common struct
      mediaDetail.mediaResource_id = m_detail.m_mgresourceId;
      mediaDetail.call_type = m_detail.m_callType;
      mediaDetail.call_id = m_detail.m_callId;
      mediaDetail.leg_id = m_detail.m_legId;
      mediaDetail.sdpinfo[0].ip_addr = m_detail.audioDetail.m_clientIp;
      mediaDetail.sdpinfo[0].port = m_detail.audioDetail.m_clientPort;
      mediaDetail.call_dir = m_detail.m_callDir;
      mediaDetail.sig_type = m_detail.m_sigType;
#else
      mediaDetail = m_detail;
#endif

      if(m_detail.m_callDir == EN_CALL_DIR_IN) { 
        mediaDetail.sdpinfo[0].resp_ip = m_detail.audioDetail.m_serverIp;
        mediaDetail.sdpinfo[0].resp_port = m_detail.audioDetail.m_serverPort;
      } else if(m_detail.m_callDir == EN_CALL_DIR_OUT) {
        mediaDetail.sdpinfo[0].dial_ip = m_detail.audioDetail.m_serverIp;
        mediaDetail.sdpinfo[0].dial_port = m_detail.audioDetail.m_serverPort;
      }

      return TRUE;
    }

    gboolean Client::post_media_event_result(xGateMediaEvent mediaEvent)
    {
      XGLOG_INFO( "Client::post_pbx_event for client: %s:%d and leg:%d start", \
          (char*)ur_log_string(m_detail.audioDetail.m_clientIp), m_detail.audioDetail.m_clientPort, m_detail.m_legId);
      m_detail.m_mediaEvent = mediaEvent;

      xGateMgMsg* pMgMsg = new xGateMgMsg();
      xGateNetConnection pbxConInfo;
      pbxConInfo.recvIp_ = m_detail.m_pbxIp;
      pbxConInfo.recvPort_ = m_detail.m_pbxPort;
      pMgMsg->set_net_con_info(pbxConInfo);

      pMgMsg->setMsgType(EN_XGATE_MSG_MGC_MG_IN);
      if(set_media_event_details(mediaEvent, pMgMsg)) {
        ACE_Task<ACE_MT_SYNCH>* taskPtr = NULL;
        taskPtr = xGateUtil::getMgDispatcher();
        if(!taskPtr) {
          XGLOG_ERROR("post_pbx_event MgDispatcher task not found");
          return EN_XGATE_STATUS_ERROR;
        }

        ACE_Message_Block* pAmb = NULL;
        pAmb = static_cast<xGateMgMsg*> (pMgMsg);
        ACE_Time_Value tval ((time(NULL) ));
        tval += ACE_Time_Value(0,1);

        if (taskPtr->putq(pAmb, &tval)) {
          XGLOG_INFO( "post_pbx_event MgDispatcher enqueue message success");
          return TRUE;
        } else {
          XGLOG_ERROR("post_pbx_event MgDispatcher enqueue message failed");
          delete pMgMsg;
        }
      }
      return FALSE;
    }
    /* Client class implementation - End
    */

    /*xGateHmpCall class implementation - Start
    */
    xGateHmpCall::xGateHmpCall(string mgresourceId, xGateCallType callType) : 
      m_mgresourceId(mgresourceId), m_callType(callType), m_clientCount(0),
      m_termAudioPort(0), m_termAudioSocket(NULL) 
    {
      m_clientList.clear();
      //m_clientList.reserve(10);
    }

    xGateHmpCall::~xGateHmpCall()
    {
      remove_client_entries(); 
      //TODO: do we need to release the m_termAudioSocket object?
    }

    gboolean xGateHmpCall::insert_client_entry(Client *client)
    { 
      if(!client) {
        XGLOG_ERROR("insert_client_entry failed. Invalid client object !!");
        return FALSE;
      }
      ClientDetail &clientDetail = client->m_detail;
      if(!get_client_entry(clientDetail.audioDetail.m_clientIp, clientDetail.audioDetail.m_clientPort, clientDetail.m_callId)) {
        m_clientList.push_back(client);
        ++m_clientCount;
      } else {
        XGLOG_ERROR("insert_client_entry failed. Client entry already available for call_id:%s and client:%s:%d !!", \
            (char*)ur_log_string(clientDetail.m_mgresourceId), (char*)ur_log_string(clientDetail.audioDetail.m_clientIp), clientDetail.audioDetail.m_clientPort);
      }
      XGLOG_INFO( "insert_client_entry success for call_id:%s and client:%s:%d !!", \
          (char*)ur_log_string(clientDetail.m_callId), (char*)ur_log_string(clientDetail.audioDetail.m_clientIp), clientDetail.audioDetail.m_clientPort);
      return TRUE;
    }

    Client * xGateHmpCall::get_client_entry(string clientIp, gint clientPort, gint legId)
    {
      if(m_clientList.empty()) {
        XGLOG_ERROR("get_client_entry by legId : failed for cliet ip:port %s;%d and legId:%d, becuase clientList is empty !!", \
            (char*)ur_log_string(clientIp), clientPort, legId);
        return NULL;
      }
      Client *client = NULL;
      CLIENT_LIST::iterator it = m_clientList.begin();
      for(it; it != m_clientList.end(); ++it) {
        client = (*it);   
        if(client) {
          ClientDetail &clientDetail = client->m_detail;
          if((clientIp == clientDetail.audioDetail.m_clientIp) && (clientPort == clientDetail.audioDetail.m_clientPort) && (legId == clientDetail.m_legId)) {
            return client; 
          }
        }
      }
      return NULL;
    }

    Client * xGateHmpCall::get_client_entry(string clientIp, gint clientPort, string callId)
    {
      if(m_clientList.empty()) {
        XGLOG_ERROR("get_client_entry by callId : failed for client ip:port %s;%d and callId:%s, becuase clientList is empty !!", \
            (char*)ur_log_string(clientIp), clientPort, (char*)ur_log_string(callId));
        return NULL;
      }
      Client *client = NULL;
      CLIENT_LIST::iterator it = m_clientList.begin();
      for(it; it != m_clientList.end(); ++it) {
        client = (*it);
        if(client) {
          ClientDetail &clientDetail = client->m_detail;
          if((clientIp == clientDetail.audioDetail.m_clientIp) && (clientPort == clientDetail.audioDetail.m_clientPort) && (callId == clientDetail.m_callId)) {
            return client;
          }
        }
      }
      return NULL;
    }

    Client * xGateHmpCall::get_client_entry(string callId)
    {
      if(m_clientList.empty()) {
        XGLOG_ERROR("get_client_entry by callId : failed for callId:%s, becuase clientList is empty !!", (char*)ur_log_string(callId));
        return NULL;
      }
      Client *client = NULL;
      CLIENT_LIST::iterator it = m_clientList.begin();
      for(it; it != m_clientList.end(); ++it) {
        client = (*it);
        if(client) {
          ClientDetail &clientDetail = client->m_detail;
          if(!client->m_detail.m_callId.empty())
          if(callId == clientDetail.m_callId) {
            return client;
          }
        }
      }
      return NULL;
    }


    Client * xGateHmpCall::get_client_entry(gint legId)
    {
      Client *client = NULL;
      gint listSize = m_clientList.size();
      if(m_clientList.size() > legId) {
        client = m_clientList[legId];
      }
      return client;
    }

    gboolean xGateHmpCall::remove_client_fromlist(Client *client)
    {
      if(m_clientList.empty()) {
        XGLOG_ERROR("remove_client_fromlist failed for call_id:%s. clientList is empty !!", (char*)ur_log_string(client->m_detail.m_callId));
        return FALSE;
      }
      CLIENT_LIST::iterator itr = m_clientList.begin();
      while(itr != m_clientList.end()) {
        if((*itr) == client) {
          --m_clientCount;
          XGLOG_INFO("remove_client_fromlist deleting object for call_id:%s. to delete from clientList !!", (char*)ur_log_string(client->m_detail.m_callId));
          itr = m_clientList.erase(itr);
          return TRUE;
        }
        itr++;
      }
      return FALSE;
    }

    gboolean xGateHmpCall::remove_client_entry(Client *client)
    {
      if(m_clientList.empty()) {
        XGLOG_ERROR("remove_client_entry failed for call_id:%s. clientList is empty !!", (char*)ur_log_string(client->m_detail.m_callId));
        return FALSE;
      }
      CLIENT_LIST::iterator itr = m_clientList.begin();
      while(itr != m_clientList.end()) {
        if((*itr) == client) {
          --m_clientCount;
          XGLOG_INFO("remove_client_entry deleting object for call_id:%s. to delete from clientList !!", (char*)ur_log_string(client->m_detail.m_callId));
          if(client->m_mp3_enc == 1 ) {
          delete client;
	  client = NULL;
          }
          itr = m_clientList.erase(itr);
          return TRUE;
        }
        itr++;
      } 
      return FALSE;
    }

    gboolean xGateHmpCall::remove_client_entries()
    {
      if(m_clientList.empty()) {
        XGLOG_ERROR("remove_client_entry failed. clientList is empty !!");
        return TRUE;
      }
      CLIENT_LIST::iterator itr = m_clientList.begin();
      while(itr != m_clientList.end()) {
        Client *client = (*itr);
        if(client) {
          XGLOG_INFO("remove_client_entry deleting object for call_id:%s. to delete from clientList !!", (char*)ur_log_string(client->m_detail.m_callId));
         stop_call_record(client->m_detail.m_callId);     
       if(client->m_mp3_enc == 1){
          delete client;
          client = NULL;
          }
          --m_clientCount;
        }
        itr++;
      }
      m_clientList.clear();
      return TRUE;
    }

    //allocate udp port and socket for video channel
    GSocket * xGateHmpCall::allocate_video_port(gint &videoPort) 
    {
      GSocket *videoSocket = NULL;
      if(m_callType == EN_XGATE_CALL_TYPE_VIDEO) {
        if((m_termVideoPort > 0) && m_termVideoSocket) {
          videoPort = m_termVideoPort;
          videoSocket = m_termVideoSocket;
          return videoSocket;
        } else { //allocate both orignation and termination port for ONE2ONE call
          while(!videoSocket) {
            videoPort = HOSTGST_GET_VIDEO_PORT();
            videoSocket = xGateHmpGstUtil::create_udpsocket(videoPort);
          }
          while(!m_termVideoSocket) {
            m_termVideoPort = HOSTGST_GET_VIDEO_PORT();
            m_termVideoSocket = xGateHmpGstUtil::create_udpsocket(m_termVideoPort);
          }
          return videoSocket;
        }
      } else { //this might be a CONFERENCE
        while(!videoSocket) {
          videoPort = HOSTGST_GET_VIDEO_PORT();
          videoSocket = xGateHmpGstUtil::create_udpsocket(videoPort);
        }
        return videoSocket;
      }
      return NULL;
    }

    xGateRetVal xGateHmpCall::create_video_channels(Client *client)
    {
      /*allocate udp port and socket for video channel */
      gint videoPort = 0;
      GSocket *videoSocket = allocate_video_port(videoPort);
      if((videoPort > 0) && videoSocket) {
        if(!client->create_video_channel(videoPort, videoSocket)) {
          XGLOG_ERROR("xGateHmpCall::create_video_channels failed!!");
          return EN_XGATE_STATUS_ERROR;
        }
      } else {
        XGLOG_ERROR("xGateHmpCall::create_video_channels failed!!");
        return EN_XGATE_STATUS_ERROR;
      }
      return EN_XGATE_STATUS_SUCCESS;
    }

    xGateRetVal xGateHmpCall::create_receive_channel(ClientDetail &clientDetail)
    {
      XGLOG_INFO( "xGateHmpCall::create_channel for m_mgresourceId: %s and client: %s:%d call-Id %s ", \
          (char*)ur_log_string(clientDetail.m_mgresourceId), (char*)ur_log_string(clientDetail.audioDetail.m_clientIp), clientDetail.audioDetail.m_clientPort, (char*)ur_log_string(clientDetail.m_callId));

      //create client and channel objects for given client detail
      Client *client = NULL;
      client = get_client_entry(clientDetail.m_callId);
      if(client) {
         remove_client_entry(client);
      }

      switch (clientDetail.m_callType) {
        case EN_XGATE_CALL_TYPE_AUDIO:
        case EN_XGATE_CALL_TYPE_AUDIO_CONF:	
          {
            client = new Client(clientDetail);
            if (client->create_audio_channel()== EN_XGATE_STATUS_ERROR) {
              XGLOG_ERROR("create_channel, creating audio channel for m_mgresourceId:%s and client:%s:%d failed!!", \
                  (char*)ur_log_string(clientDetail.m_mgresourceId), (char*)ur_log_string(clientDetail.audioDetail.m_clientIp), clientDetail.audioDetail.m_clientPort);
              delete client;
	      client = NULL;
              return EN_XGATE_STATUS_ERROR;
            }
             client->m_mgresourceId = clientDetail.m_mgresourceId;
             client->m_callId=clientDetail.m_callId;  
            insert_client_entry(client);
            clientDetail = client->m_detail;
            break;
          }
        case EN_XGATE_CALL_TYPE_UNKNOWN:
        default:
          {
            XGLOG_INFO( "xGateHmpCall::create_channel failed for Unknown/Unsupported callType");
            return EN_XGATE_STATUS_ERROR;         
            break; 
          }
      }

      return EN_XGATE_STATUS_SUCCESS;
    }

    xGateRetVal xGateHmpCall::start_voice_channels()
    {
      XGLOG_INFO( "xGateHmpCall::start_voice_channels for m_mgresourceId: %s start", \
          (char*)ur_log_string(m_mgresourceId));
      Client *client = NULL;
      CLIENT_LIST::iterator it;
      for(it = m_clientList.begin(); it != m_clientList.end(); ++it) {
        client = (*it);   
        if(client) {
          client->start_voice_channel();
        }
      }
      return EN_XGATE_STATUS_SUCCESS;
    }

    xGateRetVal xGateHmpCall::stop_voice_channels()
    {
      XGLOG_INFO( "xGateHmpCall::stop_voice_channels for m_mgresourceId: %s start", \
          (char*)ur_log_string(m_mgresourceId));
      Client *client = NULL;
      CLIENT_LIST::iterator it;
      for(it = m_clientList.begin(); it != m_clientList.end(); ++it) {
        client = (*it);   
        if(client) {
          client->stop_voice_channel();
        }
      }
      return EN_XGATE_STATUS_SUCCESS;
    }

    xGateRetVal xGateHmpCall::start_call_record(string recordFile, string callId)
    {
      Client *recClient = NULL;
      Client *client = NULL;
      CLIENT_LIST::iterator it;

      XGLOG_INFO( "xGateHmpCall::start_call_record for m_mgresourceId: %s and call_id:%s start", \
          (char*)ur_log_string(m_mgresourceId), (char*)ur_log_string(callId));

      recClient = get_client_entry(callId); 
      if(!recClient) {
        XGLOG_ERROR("start_call_record, client not available for call_id: %s !!", (char*)ur_log_string(callId));
        return EN_XGATE_STATUS_ERROR;
      }

      if(!recClient->m_pHmpRecordAudioChannel) {
        XGLOG_ERROR("start_call_record, record audio channel not created for call_id: %s !!", (char*)ur_log_string(callId));
        return EN_XGATE_STATUS_ERROR;
      }

      if(!recClient->set_record_attributes(recordFile)) {
        XGLOG_ERROR("start_call_record, setting record attributes for call_id: %s failed !!", (char*)ur_log_string(callId));
        return EN_XGATE_STATUS_ERROR;
      }
     
      //recClient->add_receive_channel(recClient);
      recClient->add_receive_channel();

      //start the record channel
      recClient->m_pHmpRecordAudioChannel->start();
      recClient->m_rec_started = 1;
      recClient->m_rec_del = 1;
  #if 0
      for(it = m_clientList.begin(); it != m_clientList.end(); ++it) {
        client = (*it);
        if(client) {
          XGLOG_INFO( "start_call_record, for call_id: %s ", client->m_detail.m_callId.c_str());
          client->m_pHmpRecordAudioChannel->start();
        }
      }
  #endif
      //schedule the max timeout for call recording
      //recClient->m_pHmpRecordAudioChannel->schedule_timeout(recClient);
      return EN_XGATE_STATUS_SUCCESS;
    }

    xGateRetVal xGateHmpCall::stop_call_record(string callId)
    {
      Client *recClient = NULL;
      XGLOG_INFO( "xGateHmpCall::stop_call_record for m_mgresourceId: %s and call_leg:%d start", \
          (char*)ur_log_string(m_mgresourceId), (char*)ur_log_string(callId));

      recClient = get_client_entry(callId);
      if(!recClient) {
        XGLOG_ERROR("stop_call_record, client not available for call_id: %s !!", (char*)ur_log_string(callId));
        return EN_XGATE_STATUS_ERROR;
      }

      if(!recClient->m_pHmpRecordAudioChannel) {
        XGLOG_ERROR("stop_call_record, record audio channel not created for call_leg: %s !!", (char*)ur_log_string(callId));
        return EN_XGATE_STATUS_ERROR;
      }

      //stop the record channel
     //recClient->m_pHmpRecordAudioChannel->send_eos();
     if(recClient->m_rec_started == 1){    
        recClient->m_pHmpRecordAudioChannel->stop();
        recClient->m_rec_started = 0;
       }
     if(recClient->m_rec_del == 1){
       recClient->m_pHmpRecordAudioChannel->get_record_details(recClient);
      
       if(recClient->m_detail.m_fileSize == 0) {
	 XGLOG_ERROR("stop_call_record, call not connected or zero size deleting the file for call_leg: %s !!", (char*)ur_log_string(callId));
         remove(recClient->m_detail.m_filePath.c_str());
       }
       recClient->remove_receive_channel();
       // recClient->m_detail.m_fileSize=0;
       //if(recClient->m_detail.m_fileSize != 0){
         recClient->m_pHmpRecordMP3Channel->set_record_file(recClient->m_detail.m_filePath);
         recClient->m_pHmpRecordMP3Channel->start();
       /*}
       else{
         XGLOG_INFO("stop_call_record,unable to paly MP3 pipeline ,wav file size is zero %s",callId.c_str());
         recClient->post_message_to_HmpProcessor(false);
    //    delete recClient;
         recClient->m_mp3_enc=1;
        } */
    }
     else
     {
       recClient->m_mp3_enc = 1;
     }       
      return EN_XGATE_STATUS_SUCCESS;
    }


    xGateRetVal xGateHmpCall::hold_call_record(string callId)
    {
      XGLOG_INFO( "xGateHmpCall::hold_call_record for m_mgresourceId: %s and call_id:%s start", \
          (char*)ur_log_string(m_mgresourceId), (char*)ur_log_string(callId));
      Client *client = NULL;
        client = get_client_entry(callId);
        if(client) {
        XGLOG_INFO( "hold_call_record, stoping record receive channel for call_id: %s ", (char*)ur_log_string(client->m_detail.m_callId));
          if(client->m_rec_started == 1){
             client->m_pHmpRecordAudioChannel->hold();
             client->m_rec_started = 0;
          }
       } 
      return EN_XGATE_STATUS_SUCCESS;
    }
   
    xGateRetVal xGateHmpCall::unhold_call_record(string callId)
    {
      XGLOG_INFO( "xGateHmpCall::unhold_call_record for m_mgresourceId: %s and call_id:%s start", \
          (char*)ur_log_string(m_mgresourceId), (char*)ur_log_string(callId));
         Client *client = NULL;
         client = get_client_entry(callId);
        if(client) {
          XGLOG_INFO( "unhold_call_record, starting record receive channel for call_id: %s ", (char*)ur_log_string(client->m_detail.m_callId));
          if(client->m_rec_started == 0){
             client->m_pHmpRecordAudioChannel->start();
             client->m_rec_started = 1;
           }   
        }
      return EN_XGATE_STATUS_SUCCESS;
    }
    
    xGateRetVal xGateHmpCall::hold_conf_call_record(string callId)
    {
      XGLOG_INFO( "xGateHmpCall::hold_conf_call_record for m_mgresourceId: %s and call_id:%s start", \
          (char*)ur_log_string(m_mgresourceId), (char*)ur_log_string(callId));
      Client *client = NULL;
      client = get_client_entry(callId);
      if(client) {
          XGLOG_INFO( "hold_conf_call_record, stoping record receive channel for call_id: %s ", (char*)ur_log_string(client->m_detail.m_callId));
          if(client->m_rec_started == 1){
            client->m_pHmpRecordAudioChannel->hold();
            client->m_rec_started = 0;
          }     
      }
      return EN_XGATE_STATUS_SUCCESS;
    }

    xGateRetVal xGateHmpCall::unhold_conf_call_record(string callId)
    {
      XGLOG_INFO( "xGateHmpCall::unhold_conf_call_record for m_mgresourceId: %s and call_id:%s start", \
          (char*)ur_log_string(m_mgresourceId), (char*)ur_log_string(callId));
      Client *client = NULL;
      client = get_client_entry(callId);
      if(client) {
          XGLOG_INFO( "unhold_call_conf_record, starting record receive channel for call_id: %s ", (char*)ur_log_string(client->m_detail.m_callId));
          if(client->m_rec_started == 0){
            client->m_pHmpRecordAudioChannel->start();
            client->m_rec_started = 1;
           }
      }
      return EN_XGATE_STATUS_SUCCESS;
    }

    /*xGateHmpCall class implementation - End
    */
