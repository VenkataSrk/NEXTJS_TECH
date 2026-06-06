import { mysqlPoolConnection } from '../plugins/db';
import { logger } from '../plugins/winston';

/**
 *
 * @param data
 * @returns
 */
export const insertConversationTriggerDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_insert_update_conversation_triggers(?,?,?,?,?)',
        [
          data?.triggerid ?? null,
          data?.domainId,
          data?.triggerName ?? null,
          data?.description ?? null,
          data?.keywords ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            logger.error('ccaas_insert_update_conversation_triggers - Unexpected Error:', err);
            reject(err);
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
      logger.error('ccaas_insert_update_conversation_triggers - Unexpected Error:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getConversationTriggersDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_conversation_triggers(?,?)',
        [
          data?.triggerid ?? null,
          data?.domainId,
        ],
        (err: any, result: any) => {
          if (err) {
            logger.error('ccaas_get_conversation_triggers - Unexpected Error:', err);
            reject(err);
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
      logger.error('ccaas_get_conversation_triggers - Unexpected Error:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const deleteConversationTriggerDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_delete_conversation_triggers(?,?)',
        [
          data.triggerid ?? null,
          data.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            logger.error('ccaas_delete_conversation_triggers - Unexpected Error:', err);
            reject(err);
          }
          if (result && result[0]?.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      logger.error('ccaas_delete_conversation_triggers - Unexpected Error:', err);
      reject(err);
    }
  });
};
