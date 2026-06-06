import { mysqlPoolConnection } from '../plugins/db';
import { logger } from '../plugins/winston';

/**
 *  create a notification
 */
export const insertNotificationDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_notification_details(?,?,?,?,?,?,?,?,?,?)', [
        data.uuid,
        data.domainId,
        data.toExt,
        data.notificationMsg,
        data.markAsRead,
        data.timeStamp,
        data.type,
        data.channelType,
        data?.coid,
        data?.customerValue
      ],                        (err: any, result: any) => {
        if (err) {
          logger.error('ccaas_notification_details - Unexpected Error:', err);
          reject(err);
        }
        resolve(result);
      });
    } catch (err) {
      logger.error('ccaas_notification_details - Unexpected Error:', err);
      reject(err);
    }
  });
};
