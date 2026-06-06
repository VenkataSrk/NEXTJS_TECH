import { logger } from '../../src/plugins/winston';
import { mysqlPoolConnection } from '../plugins/db';

export const updateConfig = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_InsertUpdateCallConfiguration(?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.name,
          data?.contactCenterNumber,
          data?.domainId,
          data?.Language,
          data?.isBot,
          data?.callFlowId,
          data?.greetingMessage,
          data?.status,
          data?.intents.length > 0 ? JSON.stringify(data?.intents) : '[]',
          data?.virtualAgent,
          data?.assistPriority,
          data?.threshold,
          data?.isMultilingualStatus ?? 0
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('updateConfig err', { data, Err: err });
          }
          resolve(result?.length > 0 ? result?.[0] : []);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('updateConfig catch', { data, Err: err });
    }
  });
};

export const getCallConfig = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetCallConfigurationList(?)',
        [
          data?.domainId,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getCallConfig err', { data, Err: err });
          }
          resolve(result?.length > 0 ? result?.[0] : []);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('getCallConfig catch', { data, Err: err });
    }
  });
};

export const getByNumber = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetCallConfigurationLDetails(?,?)',
        [
          data.contactCenterNumber,
          data?.domainId,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getByNumber err', { data, Err: err });
          }
          resolve(result?.length > 0 ? result?.[0] : []);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('getByNumber catch', { data, Err: err });
    }
  });
};

export const deleteVoice = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_DeleteCallConfiguration(?,?)',
        [
          data.contactCenterNumber,
          data?.domainId,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('deleteVoice err', { data, Err: err });
          }
          resolve(result?.length > 0 ? result?.[0] : []);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('deleteVoice catch', { data, Err: err });
    }
  });
};

export const updateStatus = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_updateCallConfigStatus(?,?,?)',
        [
          data.contactCenterNumber,
          data?.domainId,
          data.status
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('updateStatus err', { data, Err: err });
          }
          resolve(result?.length > 0 ? result?.[0] : []);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('updateStatus catch', { data, Err: err });
    }
  });
};

/**
 *
 * @param data
 * @returns
 * SP: ccaas_insert_update_settings_voice_video_recording
 */
export const insertUpdateVoiceVideoRecordingDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_insert_update_settings_voice_video_recording(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.rid,
          data?.domainId,
          data?.isVoice,
          data?.isInbound,
          data?.isOutbound,
          data?.isVoiceConsentNotice,
          data?.isVoiceTextToSpeech,
          data?.voiceText,
          data?.isVoiceSelectPrompt,
          data?.voicePrompt,
          data?.isVoiceAgentAccess,
          data?.isVideo,
          data?.isVideoConsentNotice,
          data?.isVideoTextToSpeech,
          data?.videoText,
          data?.isVideoSelectPrompt,
          data?.videoPrompt,
          data?.isVideoAgentAccess,
          data?.voiceUrl,
          data?.videoUrl
        ],
        (err: any, result: any) => {
          if (err) {
            logger.error('insertUpdateVoiceVideoRecordingDao err', { data, Err: err });
            reject(err);
          } else {
            if (result?.[0]?.length > 0) {
              resolve(result?.[0]);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (err) {
      logger.error('insertUpdateVoiceVideoRecordingDao catch', { data, Err: err });
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 * SP: ccaas_get_settings_voice_video_recording
 */
export const getSettingVoiceVideoRecordingdao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_settings_voice_video_recording(?)',
        [data?.domainId],
        (err: any, result: any) => {
          if (err) {
            logger.error('getSettingVoiceVideoRecordingdao err', { data, Err: err });
            reject(err);
          } else {
            if (result?.[0]?.length > 0) {
              resolve(result?.[0]);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('getSettingVoiceVideoRecordingdao catch', { data, Err: err });

    }
  });
};
