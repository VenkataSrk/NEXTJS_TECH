import { logger } from '../../src/plugins/winston';
import { mysqlPoolConnection } from '../plugins/db';

/**
 *  create a notification
 */
export const insertNotificationDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {

      mysqlPoolConnection.query('call ccaas_notification_details(?,?,?,?,?,?,?,?,?,?)', [
        data?.uuid,
        data?.domainId,
        data?.toExt,
        data?.notificationMsg,
        data?.markAsRead,
        data?.timeStamp,
        data?.type,
        data?.channelType,
        data?.coid,
        data?.customerValue
      ],                        (err: any, result: any) => {
        if (err) {
          logger.error('insertNotificationDao err', { data, Err: err });
          reject(err);
        } else {
          if (result?.length > 0) {
            resolve(result);
          } else {
            resolve([]);
          }
        }
      });
    } catch (err) {
      reject(err);
      logger.error('insertNotificationDao catch', { data, Err: err });
    }
  });
};

/**
 * @description Get notifications
 * @param data
 * @returns
 */
export const getNotificationDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_get_notification(?, ?, ?,?,?)',
        [
          data?.ext,
          data?.domainId,
          data?.timeStamp ?? null,
          data?.limit ?? null,
          data?.offset ?? null
        ],
        (err: any, result: any) => {
          // console.log('result', result);
          if (err) {
            logger.error('getNotificationDao err', { data, Err: err });
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
    } catch (error) {
      reject(error);
      logger.error('getNotificationDao catch', { data, Err: error });
    }
  });
};

/**
 * @description delete notifications
 * @param data
 * @returns
 */
export const deleteNotificationDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_delete_notification_by_nId(?, ?)',
        [
          data?.domainId,
          data?.nId,
        ],
        (err: any, result: any) => {
          if (err) {
            logger.error('deleteNotificationDao err', { data, Err: err });
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
      reject(err);
      logger.error('deleteNotificationDao catch', { data, Err: err });
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateMarkAsReadNotificationDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_update_read_notification_by_nId(?, ?)',
        [
          data?.domainId,
          data?.nId,
        ],
        (err: any, result: any) => {
          if (err) {
            logger.error('updateMarkAsReadNotificationDao err', { data, Err: err });
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
      reject(err);
      logger.error('updateMarkAsReadNotificationDao catch', { data, Err: err });
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateMarkAsReadAllNotificationDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_update_readall_notification_by_ext(?, ?)',
        [
          data?.domainId,
          data?.toExt,
        ],
        (err: any, result: any) => {
          // console.log('result', result);
          if (err) {
            logger.error('updateMarkAsReadAllNotificationDao err', { data, Err: err });
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
      logger.error('updateMarkAsReadAllNotificationDao catch', { data, Err: err });
      reject(err);
    }
  });
};
