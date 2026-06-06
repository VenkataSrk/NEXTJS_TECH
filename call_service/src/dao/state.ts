/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */
import { mysqlPoolConnection } from '../plugins/db';
import { logger } from '../plugins/winston';

/**
 *
 * @param data
 * @returns
 */
export const createState = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_InsertUpdateStateTimer_info(?,?,?,?)', [
        data?.domainId,
        data?.startTimestamp,
        data?.timerStatusId,
        data?.ext
      ],                        (err: any, result: any) => {
        if (err) {
          logger.error('ccaas_InsertUpdateStateTimer_info',  { data, Err: err });
          reject(err);
        } else {
          resolve(result?.length > 0 ? result?.[0] : []);
        }
      });
    } catch (error) {
      logger.error('createState', { data, Err: error });
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getStateListBy = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_GetStateTimer_info(?,?)', [
        data?.domainId,
        data?.ext,
      ],                        (err, result) => {
        if (err) {
          logger.error('ccaas_GetStateTimer_info', { data, Err: err });
          reject(err);
        } else {
          resolve(result);
        }
      });
    } catch (error) {
      logger.error('getStateListBy',  { data, Err: error });
      reject(error);
    }
  });
};
