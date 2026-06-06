import {
  mysqlPoolConnection,
} from '../plugins/db';

/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */
import { logger } from '../plugins/winston';

/**
 *
 * @param data
 * @returns
 */
export const getOverallCallbackGroupInteractionDetailsDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL ccaas_ai_get_customer_overall_callback_interaction_group_user(?,?,?,?,?,?,?,?,?,?,?)', [
        data?.domainId,
        data?.isMissed,
        data?.isAbandoned,
        data?.offset,
        data?.limit,
        data?.channelName,
        data?.customerName,
        data?.QueueName,
        data?.fromDate,
        data?.toDate,
        data?.agentName,
      ],
                                (err, result) => {
                                  if (err) {
                                    reject(err);
                                    logger.error('ccaas_get_interaction_details_view - Unexpected Error:', err);

                                  } else {
                                    if (result[0]?.length > 0) {
                                      resolve(result);
                                    } else {
                                      resolve([]);
                                    }
                                  }
                                }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaas_get_interaction_details_view - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getOverallCallbackGroupInteractionPerticularDetailsDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {

      mysqlPoolConnection.query('CALL ccaas_ai_get_customer_overall_callback_interaction_all_user_dtl (?,?,?,?,?,?)', [
        data?.domainId,
        data?.coid,
        data?.isMissed,
        data?.isAbandoned,
        data?.offset,
        data?.limit,
      ],
                                (err, result) => {
                                  if (err) {
                                    reject(err);
                                    logger.error('ccaas_get_interaction_details_view - Unexpected Error:', err);
                                    console.log(err);
                                  } else {
                                    if (result[0]?.length > 0) {
                                      resolve(result);
                                    } else {
                                      resolve([]);
                                    }
                                  }
                                }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaas_get_interaction_details_view - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getSocialMediHistoryDetailsHandlerDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {

      mysqlPoolConnection.query('CALL ccaass_get_social_media_history_details (?,?)', [
        data?.historyId,
        data?.channelType,
      ],
                                (err, result) => {
                                  if (err) {
                                    reject(err);
                                    logger.error('ccaass_get_social_media_history_details - Unexpected Error:', err);
                                    console.log(err);
                                  } else {
                                    if (result[0]?.length > 0) {
                                      resolve(result);
                                    } else {
                                      resolve([]);
                                    }
                                  }
                                }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaass_get_social_media_history_details - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getSocialMediaMessageByIdDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {

      mysqlPoolConnection.query('CALL up_GetSocialMediaMessageById (?,?)', [
        data?.historyId,
        data?.channelType,
      ],
                                (err, result) => {
                                  if (err) {
                                    reject(err);
                                    logger.error('up_GetSocialMediaMessageById - Unexpected Error:', err);
                                    console.log(err);
                                  } else {
                                    if (result[0]?.length > 0) {
                                      resolve(result);
                                    } else {
                                      resolve([]);
                                    }
                                  }
                                }
      );
    } catch (err) {
      reject(err);
      logger.error('up_GetSocialMediaMessageById - Unexpected Error:', err);
    }
  });
};
