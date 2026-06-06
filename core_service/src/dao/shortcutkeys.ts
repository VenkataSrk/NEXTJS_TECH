import { v4 as uuidv4 } from 'uuid';
import { mysqlPoolConnection } from '../plugins/db';
import { logger } from '../plugins/winston';

/**
 *
 * @param data
 * @returns
 */
export const insertShortcutKeysDao = (data: any, auth: any) => {
  return new Promise(async (resolve, reject) => {
    const keyValues = JSON.stringify(data);
    const defaultKeys = JSON.stringify(data);
    const type = 'Custom';
    const uuid = uuidv4();
    try {
      mysqlPoolConnection.query(
        'CALL ccass_create_short_cut_key_details(?,?,?,?,?,?)',
        [
          uuid,
          keyValues,
          defaultKeys,
          auth.domainId,
          auth.ext,
          type,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccass_create_short_cut_key_details - Unexpected Error:', err);

          }
          resolve(result[0]);
        });
    } catch (err) {
      reject(err);
      logger.error('ccass_create_short_cut_key_details - Unexpected Error:', err);

    }
  });
};

/**
 * @description
 * @param data
 * @returns
 */
export const getShortcutKeysDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_get_shortcut_keys_dao(?,?)',
        [
          data.domainId,
          data.ext
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_shortcut_keys_dao - Unexpected Error:', err);

          } else {
            resolve(result[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_shortcut_keys_dao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getShortcutKeysValuesDao = async (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_get_shortcut_key_keyValues(?)',
        [data.keyId],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_shortcut_key_keyValues - Unexpected Error:', err);

          } else {
            resolve(result[0]);
          }
        });
    } catch (err) {
      reject(err);
      logger.error('ccaas_get_shortcut_key_keyValues - Unexpected Error:', err);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateStatusShortcutKeysDao = async (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_update_shortcut_key_keyValues(?,?)',
        [
          data.keyId,
          JSON.stringify(data.response)
        ],
        (error, response) => {
          if (error) {
            reject(error);
            logger.error('ccaas_update_shortcut_key_keyValues - Unexpected Error:', error);
          } else {
            resolve(response);
          }
        });
    } catch (err) {
      reject(err);
      logger.error('ccaas_update_shortcut_key_keyValues - Unexpected Error:', err);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertGlobalShortcutKeysDao = (data: any, auth: any) => {
  return new Promise(async (resolve, reject) => {
    const uuid = uuidv4();
    const keyValues = JSON.stringify(data);
    const type = 'Default';
    try {
      mysqlPoolConnection.query('CALL ccass_create_short_cut_key_details(?,?,?,?,?,?)', [
        uuid,
        keyValues,
        keyValues,
        auth.domainId,
        auth.ext,
        type,
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('ccass_create_short_cut_key_details - Unexpected Error:', err);

        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
      logger.error('ccass_create_short_cut_key_details - Unexpected Error:', err);

    }
  });
};

/**
 * @description
 * @param data
 * @returns
 */
export const resetAllShortcutKeysByUuidDao = (data: any, keyId: any) => {
  return new Promise((resolve, reject) => {
    try {

      mysqlPoolConnection.query(
        'CALL ccaas_resetall_shortcut_keys_by_uuid_dao(?)',
        [keyId],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_resetall_shortcut_keys_by_uuid_dao - Unexpected Error:', err);

          } else {
            resolve(result);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_resetall_shortcut_keys_by_uuid_dao - Unexpected Error:', error);
    }
  });
};

/**
 * @description
 * @param data
 * @returns
 */
export const getGlobalShortcutKeysDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_get_global_shortcut_keys_dao(?,?)',
        [
          data?.domainId,
          data?.ext
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_global_shortcut_keys_dao - Unexpected Error:', err);

          } else {
            resolve(result[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_global_shortcut_keys_dao - Unexpected Error:', error);
    }
  });
};
