/**
 * @param data
 * @returns
 */

import { mysqlPoolConnection } from '../../src/plugins/db';

export const insertUpdateHomeChatHistoryDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL ccaas_insert_update_home_search_history_log(?,?,?,?,?)', [
        data.sessionId,
        data.domainId,
        data.title,
        data.agentId,
        data.message,
      ],                        (err, result) => {
        if (err) {
          console.log('err:', err);
          resolve(err);
        } else {
          const rows =
                        Array.isArray(result) && Array.isArray(result[0])
                            ? result[0]
                            : result || [];

          resolve(rows);
        }
      });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

export const getHomeChatHistoryDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL ccaas_get_home_search_history_log(?,?,?)', [
        data.sessionId,
        data.domainId,
        data.agentId,
      ],                        (err, result) => {
        if (err) {
          console.log('err:', err);
          resolve(err);
        } else {
          const rows =
                        Array.isArray(result) && Array.isArray(result[0])
                            ? result
                            : result || [];

          resolve(rows);
        }
      });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

export const deleteHomeChatHistoryDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL ccaas_delete_home_search_history_log(?,?)', [
        data.sessionId,
        data?.domainId,
      ],                        (err, result) => {
        if (err) {
          console.log('err:', err);
          resolve(err);
        } else {
          const rows =
                        Array.isArray(result) && Array.isArray(result[0])
                            ? result[0]
                            : result || [];

          resolve(rows);
        }
      });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};
