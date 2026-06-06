import { mysqlPoolConnection } from '../plugins/db';
import { logger } from '../plugins/winston';

/**
 *
 * @param data
 * @returns
 */
export const insertCustomerSatisficationDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call  up_ccaas_insertUpdateCustomerSatisfactionRatingDesign(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.caption ?? null,
          data?.domainId,
          data?.ratingDesignType ?? 0,
          data?.labelRatingType ?? 0,
          data?.followUpQuestion ?? 0,
          data?.thanksNote ?? null,
          data?.dontDisturbType ?? 0,
          data?.dontDistrubTime ?? 0,
          data?.additionalQuestionType ?? 0,
          data?.additionalQuestions ?? null,
          data?.isVoice ?? 0,
          data?.callFlows ?? null,
          data?.minimumDurationType ?? null,
          data?.minimumDuration ?? 0,
          data?.isWebWidget ?? 0,
          data?.Widgets ?? null,
          data?.isSms ?? 0,
          data?.smsNumbers ?? null,
          data?.smsNumberSend ?? null,
          data?.isSendSmsFromSameNumber ?? 0,
          data?.smsMessage ?? null,
          data?.isEmail ?? 0,
          data?.emails ?? null,
          data?.emailIdSend ?? null,
          data?.isSendEmailFromSameEmail ?? 0,
          data?.emailSubject ?? null,
          data?.emailMessage ?? null,
          data?.isWhatsApp ?? 0,
          data?.whatsAppChatFlows ?? null,
          data?.isFacebook ?? 0,
          data?.facebookChatFlows ?? null,
          data?.isInstagram ?? 0,
          data?.instagramChatFlows ?? null,
          data?.isVideo ?? 0,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('up_ccaas_insertUpdateCustomerSatisfactionRatingDesign - Unexpected Error:', err);
          } else {
            if (result?.length > 0) {
              resolve(result);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('up_ccaas_insertUpdateCustomerSatisfactionRatingDesign - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCustomerSatisficationRatingDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
      'call  up_ccaas_getCustomerSatisfactionRatingDesign(?)',
      [data?.domainId],
      (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('up_ccaas_getCustomerSatisfactionRatingDesign - Unexpected Error:', err);
        } else {
          if (result?.length > 0) {
            resolve(result);
          } else {
            resolve([]);
          }
        }
      }
      );
    } catch (err) {
      reject(err);
      logger.error('up_ccaas_getCustomerSatisfactionRatingDesign - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCustomerSatisficationChannelDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
      'call  up_ccaas_getCustomerSatisfactionChannelStatus(?)',
      [data?.domainId],
      (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('up_ccaas_getCustomerSatisfactionChannelStatus - Unexpected Error:', err);
        } else {
          if (result?.length > 0) {
            resolve(result);
          } else {
            resolve([]);
          }
        }
      }
      );
    } catch (err) {
      reject(err);
      logger.error('up_ccaas_getCustomerSatisfactionChannelStatus - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertUpdateCustomerFeedbackDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call  up_insertUpdateCustomerFeedback(?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.customerFeedbackId ?? null,
          data?.historyId ?? null,
          data?.channelTypeId ?? null,
          data?.satisfactionRate ?? 0,
          data?.domainId,
          data?.feedbackQuestionAnswer ?? null,
          data?.AgentId ?? null,
          data?.ratingType ?? null,
          data?.sessionId ?? null,
          data?.customerNumber ?? null,
          data?.videoMeetId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('up_insertUpdateCustomerFeedback - Unexpected Error:', err);
          } else {
            if (result?.length > 0) {
              resolve(result);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('up_insertUpdateCustomerFeedback - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCustomerSatisficationDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call  up_ccaas_GetCustomerSatisfactionForChannel(?,?,?,?)',
        [
          data?.domainId,
          data?.channelTypeId ?? null,
          data?.historyId ?? null,
          data?.videoMeetId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('up_ccaas_GetCustomerSatisfactionForChannel - Unexpected Error:', err);
          } else {
            if (result?.length > 0) {
              resolve(result);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('up_ccaas_GetCustomerSatisfactionForChannel - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertUpdateInteractionNotesDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call  ccaas_insertUpdate_interactionNotes(?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.historyId ?? null,
          data?.coid ?? null,
          data?.agentId ?? null,
          data?.channelType ?? null,
          JSON.stringify(data?.notes) ?? null

        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_insertUpdate_interactionNotes - Unexpected Error:', err);
          } else {
            if (result?.length > 0) {
              resolve(result);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaas_insertUpdate_interactionNotes - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getInteractionOverallDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call  ccaas_get_interactionOverallNotesInfo(?,?)',
        [
          data?.coid ?? null,
          data?.domainId,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_interactionOverallNotesInfo - Unexpected Error:', err);
          } else {
            if (result?.length > 0) {
              resolve(result);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaas_get_interactionOverallNotesInfo - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getAgentInteractionDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call  ccaas_get_agentInteractionNotesInfo(?,?,?)',
        [
          data?.historyId ?? null,
          data?.domainId,
          data?.agentId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_agentInteractionNotesInfo - Unexpected Error:', err);
          } else {
            if (result?.length > 0) {
              resolve(result);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaas_get_agentInteractionNotesInfo - Unexpected Error:', err);
    }
  });
};
