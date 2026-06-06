import { botMysqlPoolConnections, mysqlPoolConnection } from '../plugins/db';
import { logger } from '../plugins/winston';

/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

export const insertBotIntentDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      botMysqlPoolConnections.query('call worktual_bot_create_intent_info(?,?,?,?)', [data.id, data.bot_id, data.intent, data.status], (err: any, result: any) => {
        if (err) {
          logger.error('worktual_bot_create_intent_info - Unexpected Error:', err);
          reject(err);
        } else {
          resolve(result);
        }
      });
    } catch (error) {
      logger.error('worktual_bot_create_intent_info - Unexpected Error:', error);
      reject(error);
    }
  });
};

export const getBotIntentDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      botMysqlPoolConnections.query('call worktual_bot_get_intent_info(?)', [data.bot_id], (err: any, result: any) => {
        if (err) {
          logger.error('worktual_bot_get_intent_info - Unexpected Error:', err);
          reject(err);
        } else {
          resolve(result);
        }
      });
    } catch (error) {
      logger.error('worktual_bot_get_intent_info - Unexpected Error:', error);
      reject(error);
    }
  });
};

export const insertBotImagesDao = (data: any, list: any) => {
  return new Promise((resolve, reject) => {
    try {
      const iconImage = JSON.stringify(list.imageUrl);
      const bgImage = JSON.stringify(list.backgroundImage);

      mysqlPoolConnection.query(
        'CALL ccaas_insert_bot_images_dao(?, ?, ?)',
        [data.domainId, iconImage, bgImage],
        (err, result) => {
          if (err) {
            logger.error('ccaas_insert_bot_images_dao - Unexpected Error:', err);
            reject(err);
          } else {
            resolve(result[0][0]);
          }
        }
      );
    } catch (error) {
      logger.error('ccaas_insert_bot_images_dao - Unexpected Error:', error);
      reject(error);
    }
  });
};

export const getbotImagesDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const query = `CALL ccaas_get_bot_images_dao(${data.domainId});`;
      mysqlPoolConnection.query(query, (err, result) => {
        if (err) {
          logger.error('ccaas_get_bot_images_dao - Unexpected Error:', err);
          reject(err);
        } else {
          resolve(result[0]);
        }
      });
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_bot_images_dao - Unexpected Error:', error);
    }
  });
};

export const updateBotTemplateDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_bot_topics(?,?,?,?,?,?)',
        [data.uuid, data.title ?? null, data.discription ?? null, data.botTemplate ?? null, data.domainId, data.status ?? null],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_bot_topics - Unexpected Error:', err);
          }
          resolve(result);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaas_bot_topics - Unexpected Error:', err);
    }
  });
};

export const insertFallbackMessageDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_fallback_message(?,?,?,?)',
        [data.domainId, data.uuid, data.fallBackMessage ?? null, data.mapId ?? null],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_fallback_message - Unexpected Error:', err);
          }
          resolve(result);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaas_fallback_message - Unexpected Error:', err);
    }
  });
};

export const getFallbackMessageDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_get_fall_back_message_dao(?)',
        [
          data.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_fall_back_message_dao - Unexpected Error:', err);
          } else {
            resolve(result[0]); // SP returns [rows, fields]
          }
        });
    } catch (err) {
      reject(err);
      logger.error('ccaas_get_fall_back_message_dao - Unexpected Error:', err);
    }
  });
};

export const saveChatFlowDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call up_InsertDefaulChatflowForDomain(?,?,?)', [
        data?.version,
        data?.domainId,
        data?.companyId
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('up_InsertDefaulChatflowForDomain - Unexpected Error:', err);
        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
      logger.error('up_InsertDefaulChatflowForDomain - Unexpected Error:', err);
    }
  });
};

export const insertBotDetails = (data: any, list: any) => {
  return new Promise((resolve, reject) => {
    try {
      const query = 'CALL ccaas_insert_bot_details(?, ?, ?);';
      mysqlPoolConnection.query(
        query,
        [list.botName, data.domainId, list.bot_id],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_insert_bot_details - Unexpected Error:', err);
          } else {
            resolve(result[0][0]);
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaas_insert_bot_details - Unexpected Error:', err);
    }
  });
};
