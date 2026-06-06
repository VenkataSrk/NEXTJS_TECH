import { mysqlPoolConnection } from '../plugins/db';

export const remainderCronDao = () => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_task_reminder()',
        [],
        (err: any, result: any) => {
          console.log('remainderCronDao', result);

          if (err) {
            reject(err);
            return;
          }
          resolve(result[0]);
        });
    } catch (err) {
      reject(err);
      //   logger.error('getchatSessionReport - Unexpected Error:', err);
    }
  });
};

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
        data?.customerValue,
      ],                        (err: any, result: any) => {
        if (err) {
          console.log('err:', err);
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
    }
  });
};
