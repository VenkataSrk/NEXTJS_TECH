import { mysqlPoolConnection } from '../plugins/db';

import moment from 'moment';
import { v4 as uuidv4 } from 'uuid';
import { logger } from '../../src/plugins/winston';
import { ROLES } from '../helpers/constants';
import { insertNotificationDao } from './notification.dao';

/**
 *
 * @param data
 * @returns
 */
export const insertUpdateDispositionStatus = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_myacc_insert_update_disposition_status(?,?,?,?,?,?,?,?)',
        [
          data?.did ?? null,
          data.domainId,
          data.dispositionName,
          data.description,
          data.type,
          data.status,
          data.colorCode,
          data.accessType
        ],
        (err: any, result: any[]) => {
          if (err) {
            reject(err);
            logger.error('insertUpdateDispositionStatus err', { data, Err: err });
          } else {
            resolve(result?.length > 0 ? result?.[0] : []);
          }
        });
    } catch (error) {
      logger.error('insertUpdateDispositionStatus catch', { data, Err: error });
      reject(error);
    }
  });
};

export const getDispositinStatus = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_getDispositinStatus_info(?)',
        [
          data?.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            console.log('ccaas_getDispositinStatus_info err: ', { data, Err: err });
            reject(err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log('getDispositinStatus catch error: ', { data, Err: error });
      logger.error('getDispositinStatus catch', { data, Err: error });
      reject(error);
    }
  });
};

/**
 *
 * @param did
 * @param domainId
 * @returns
 */
export const deleteDisposition = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_delete_disposition (?,?)',
        [
          data?.did,
          data?.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('deleteDisposition err', { data, Err: err });
          } else {
            resolve(result?.length > 0 ? result?.[0] : []);
          }
        });
    } catch (error) {
      logger.error('deleteDisposition catch', { data, Err: error });
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const isValidDispositionName = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_get_check_is_disposition_name(?,?)', [data?.dispositionName, data?.domainId],
                                (err: any, result: any[]) => {
                                  if (err) {
                                    reject(err);
                                    logger.error('isValidDispositionName err', { data, Err: err });
                                  } else {
                                    resolve(result?.length > 0 ? result?.[0] : []);
                                  }
                                });
    } catch (error) {
      logger.error('isValidDispositionName catch', { data, Err: error });
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateDispositionForCall = async (data: any) => {

  try {
    console.log('updateDispositionForCall>>>>>>', data);
    logger.info('updateDispositionForCall payload -', { data });
    return new Promise(async (resolve, reject) => {
      mysqlPoolConnection.query(
        'call ccaas_UpdateSessionDisposition_details(?,?,?,?,?,?,?,?,?,?)',
        [
          data.domainId,
          data.sessionId,
          data.summary,
          data.disposition.did,
          data.afterCallWorkTime,
          data.calledPersonDetails.coid,
          data.subject,
          data?.markAsRead,
          data?.ext,
          data?.tags],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log('updateDispositionForCall reject err: ', data?.sessionId, err);
          } else {
            resolve(result?.length > 0 ? result?.[0] : null);
          }
        }
      );
    });
  } catch (err: any) {
    console.log('updateDispositionForCall err:', { session: data?.sessionId, payload: data, error: err });
    logger.error('updateDispositionForCall - Unexpected Error:', { session: data?.sessionId, payload: data, error: err });
  }
};

/**
 *
 * @param data
 * @returns
 */
export const updateDispositionForChat = async (data: any) => {
  try {
    return new Promise(async (resolve, reject) => {
      data.markAsRead = 0;
      mysqlPoolConnection.query(
        'call ccaas_UpdateChatSessionDisposition_details(?,?,?,?,?,?,?,?,?)',
        [
          data?.ChatDurationHistoryId,
          data?.domainId,
          data?.summary,
          data?.dispositionId,
          data?.afterCallWorkTime,
          data?.markAsRead,
          data?.agentOccupyDetails ? JSON.stringify(data?.agentOccupyDetails) : null,
          data?.subject,
          data?.tags
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('updateDispositionForChat - reject Error:', { data, Err: err });
            console.log('updateDispositionForChat reject err: ', { data, Err: err });
          } else {
            resolve(result?.length > 0 ? result?.[0] : null);
          }
        }
      );
    });
  } catch (err: any) {
    console.log(err);
    logger.error('updateDispositionForChat - Unexpected Error:', { data, Err: err });
  }
};

/**
 *
 * @param data
 * @returns
 */
export const updateDispositionForWhatsapp = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_UpdateWhatsappHistoryDisposition(?,?,?,?,?,?,?,?,?)',
        [
          data?.WhatsappHistoryId,
          data?.domainId,
          data?.dispositionId ?? null,
          data?.summary ?? null,
          data?.isSessionClosed ?? 0,
          data?.afterWorkTime ?? null,
          data?.dispositionUpdatedBy ?? null,
          data?.duration ?? null,
          data?.dispositionSubject ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            logger.error('updateDispositionForWhatsapp - Unexpected Error:', { data, Err: err });
            reject(err);
          }
          if (result && result?.length > 0) {
            resolve(result);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      console.log('updateDispositionForWhatsapp - Unexpected Error:', { data, Err: err });
      reject(err);
      logger.error('updateDispositionForWhatsapp - Unexpected Error:', { data, Err: err });
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateDispositionFacebook = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_UpdateFacebookHistoryDisposition(?,?,?,?,?,?,?,?,?)',
        [
          data?.FacebookHistoryId,
          data?.domainId,
          data?.dispositionId ?? null,
          data?.summary ?? null,
          data?.isSessionClosed ?? 0,
          data?.afterFacebookWorkTime ?? null,
          data?.dispositionUpdatedBy ?? null,
          data?.duration ?? null,
          data?.dispositionSubject ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            logger.error('updateDispositionFacebook - Unexpected Error:', { data, Err: err });
            reject(err);
          }
          if (result && result?.length > 0) {
            resolve(result);
          } else {
            reject(err);
          }
        }
      );
    } catch (err) {
      console.log('updateDispositionFacebook - Unexpected Error:', { data, Err: err });
      reject(err);
      logger.error('updateDispositionFacebook - Unexpected Error:', { data, Err: err });
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateDispositionInstagram = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_UpdateInstagramHistoryDisposition(?,?,?,?,?,?,?,?,?)',
        [
          data?.InstagramHistoryId,
          data?.domainId,
          data?.dispositionId ?? null,
          data?.summary ?? null,
          data?.isSessionClosed ?? 0,
          data?.afterInstagramWorkTime ?? null,
          data?.dispositionUpdatedBy ?? null,
          data?.duration ?? null,
          data?.dispositionSubject ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            logger.error('updateDispositionInstagram - Unexpected Error:', { data, Err: err });
            reject(err);
          }
          if (result && result?.length > 0) {
            resolve(result);
          } else {
            reject(err);
          }
        }
      );
    } catch (err) {
      console.log('updateDispositionInstagram - Unexpected Error:', { data, Err: err });
      reject(err);
      logger.error('updateDispositionInstagram - Unexpected Error:', { data, Err: err });

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateCalledPersonDetailsCall = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_UpdateCalledPersonDetails(?,?,?)',
        [data?.calledPersonDetails?.coid, data?.domainId, data?.sessionId],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('updateCalledPersonDetailsCall - Unexpected Error:', { data, Err: err });
          }
          resolve(result);
        }
      );
    } catch (err) {
      console.log('updateCalledPersonDetailsCall - Unexpected Error:', { data, Err: err });
      reject(err);
      logger.error('updateCalledPersonDetailsCall - Unexpected Error:', { data, Err: err });

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateSentimentScore = (data: any, sentiment: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_updateSentimentalScore(?,?,?,?,?,?,?,?)',
        [
          data?.HistoryId ?? 0,
          data?.ChatDurationHistoryId ?? 0,
          'Call',
          data?.domainId ?? null,
          sentiment?.sentimentalScore ?? null,
          sentiment?.sentimentalScoreUser ?? null,
          sentiment?.sentimentalScoreAgent ?? null,
          data?.ext ?? null,
        ],
        (err: any, result: any) => {
          console.log('updateSentimentScore - result ', result, 'err', err);
          if (err) {
            reject(err);
            logger.error('updateSentimentScore - Unexpected Error:', { data, error: err });
          }
          resolve(result?.[0]);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('updateSentimentScore - Unexpected Error:', { data, error: err });

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateSentimentScoreChat = (data: any, sentiment: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_updateSentimentalScore(?,?,?,?,?,?,?,?)',
        [
          data?.HistoryId ?? 0,
          data?.ChatDurationHistoryId ?? 0,
          'Chat',
          data?.domainId ?? null,
          sentiment?.sentimentalScore ?? null,
          sentiment?.sentimentalScoreUser ?? null,
          sentiment?.sentimentalScoreAgent ?? null,
          data?.ext ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('updateSentimentScoreChat - Unexpected Error:', { data, Err: err });
          }
          resolve(result?.[0]);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('updateSentimentScoreChat - Unexpected Error:', { data, Err: err });
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateSentimentScoreVideo = (data: any, sentiment: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_updateSentimentalScore(?,?,?,?,?,?,?,?)',
        [
          data?.HistoryId ?? 0,
          data?.ChatDurationHistoryId ?? 0,
          'Video',
          data?.domainId ?? null,
          sentiment?.sentimentalScore ?? null,
          sentiment?.sentimentalScoreUser ?? null,
          sentiment?.sentimentalScoreAgent ?? null,
          data?.ext ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('updateSentimentScoreVideo - Unexpected Error:', { data, Err: err });
          }
          resolve(result?.[0]);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('updateSentimentScoreVideo - Unexpected Error:', { data, Err: err });
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertCompanyPurchasedAddonDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_InsertCompanyPurchasedAddon(?,?,?,?,?,?,?,?)',
        [
          data?.companyId,
          data?.domainId,
          data?.planId,
          data?.productId,
          data?.addonId,
          data?.addonName,
          data?.addonprice,
          data?.addonqty,
        ],
        (err: any, result: any) => {
          if (err) {
            logger.error('insertCompanyPurchasedAddonDao - Unexpected Error:', { data, Err: err });
            reject(err);
          }
          if (result?.[0]?.length > 0) {
            resolve(result?.[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (error) {
      reject(error);
      logger.error('insertCompanyPurchasedAddonDao - Unexpected Error:', { data, Err: error });
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateDispositionStatusVideoDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_UpdateVideoSessionDisposition_details(?,?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.meetingId,
          data?.summary,
          data?.disposition,
          data?.afterCallWorkTime,
          data?.subject,
          data?.agentExtn,
        ],
        (err: any, result: any) => {
          if (err) {
            logger.error('updateDispositionStatusVideoDao - Unexpected Error:', { data, Err: err });
            reject(err);
          }
          if (result?.[0]?.length > 0) {
            resolve(result?.[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (error) {
      reject(error);
      logger.error('updateDispositionStatusVideoDao - Unexpected Error:', { data, Err: error });

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getVideoRecordDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_videomeet_user_get_record_info(?,?,?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.search,
          data?.direction,
          data?.startDate,
          data?.endDate,
          data?.agentId ?? null,
          data?.offset ?? 0,
          data?.limit ?? 10,

        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log('getVideoRecordDao - Unexpected Error:', { data, Err: err });
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log('getVideoRecordDao - Unexpected Error:', { data, Err: error });
      reject(error);
      logger.error('getVideoRecordDao - Unexpected Error:', { data, Err: error });
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getVoiceRecordDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {

      mysqlPoolConnection.query(
        'call ccaas_voice_user_get_record_info(?,?,?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.search,
          data?.direction,
          data?.startDate,
          data?.endDate,
          data?.agentId ?? null,
          data?.limit ?? 10,
          data?.offset ?? 0,

        ],
        (err: any, result: any) => {
          logger.info('getVoiceRecordDao - Unexpected Error:', { data, Err: err });
          if (err) {
            reject(err);
            logger.error('getVoiceRecordDao - Unexpected Error:', { data, Err: err });
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log('getVoiceRecordDao - Unexpected Error:', { data, Err: error });
      reject(error);
      logger.error('getVoiceRecordDao - Unexpected Error:', { data, Err: error });
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getVideoSessionIdDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_videomeet_user_get_specific_record_info(?,?)',
        [
          data?.domainId,
          data?.sessionId,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getVideoSessionIdDao - Unexpected Error:', { data, Err: err });
          }
          if (result) {
            resolve(result?.length > 0 ? result?.[0] : []);
          } else {
            resolve([]);
          }
        }
      );
    } catch (error) {
      console.log('getVideoSessionIdDao - Unexpected Error:', { data, Err: error });
      reject(error);
      logger.error('getVideoSessionIdDao - Unexpected Error:', { data, Err: error });
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getVoiceSessionIdDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_voice_user_get_specific_record_info(?,?)',
        [
          data?.domainId,
          data?.sessionId,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getVoiceSessionIdDao - Unexpected Error:', { data, Err: err });
          }
          if (result) {
            resolve(result?.length > 0 ? result?.[0] : []);
          } else {
            resolve([]);
          }
        }
      );
    } catch (error) {
      console.log('getVoiceSessionIdDao - Unexpected Error:', { data, Err: error });
      reject(error);
      logger.error('getVoiceSessionIdDao - Unexpected Error:', { data, Err: error });
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getVideoChannelHistoryId = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_video_history_id_by_meeting_id(?,?,?)',
        [
          data?.domainId,
          data?.meetingId ?? null,
          data?.agentId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log('ccaas_get_video_history_id_by_meeting_id err>>>>', { data, Err: err });
            logger.error('ccaas_get_video_history_id_by_meeting_id err>>>>', { data, Err: err });
          }
          if (result) {
            resolve(result?.length > 0 ? result?.[0] : []);
          } else {
            resolve([]);
          }
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_video_history_id_by_meeting_id - Unexpected Error:', { data, Err: error });

    }
  });
};

/**
 *
 * @param did
 * @param domainId
 * @returns
 */
export const deleteVoiceRecording = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_voice_user_delete_record_info (?,?)',
        [
          data?.domainId,
          data?.rid,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log('deleteVoiceRecordings', err);
            logger.error('deleteVoiceRecordings err', { data, Err: err });
          } else {
            resolve(result?.length > 0 ? result?.[0] : []);
          }
        });
    } catch (error) {
      logger.error('deleteVoiceRecordings catch', { data, Err: error });
      console.log('deleteVoiceRecordings', error);
      reject(error);
    }
  });
};
