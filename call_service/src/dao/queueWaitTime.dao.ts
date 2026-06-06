import { logger } from '../../src/plugins/winston';
import { mysqlPoolConnection } from '../plugins/db';

/**
 * @description get queue wait time
 * @param data
 * @returns
 */
export const getQueueWaitTime = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_all_queue_wait_time(?,?)',
        [
          data.qid,
          data.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getQueueWaitTime err', { data, Err: err });

          } else {
            resolve(result?.length > 0 ? result?.[0] : []);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('getQueueWaitTime catch', { data, Err: error });
    }
  });
};

export const getAllQueueWaitTime = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_getAllQueueWaitTime_info(?)',
        [
          data?.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            console.log('getAllQueueWaitTime err: ', { data, Err: err });
            logger.error('getAllQueueWaitTime err', { data, Err: err });
            reject(err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log('getAllQueueWaitTime catch error: ', { data, Err: error });
      logger.error('getAllQueueWaitTime catch', { data, Err: error });
      reject(error);
    }
  });
};

export const insert = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaasInsertCallDurationHistoryWithoutAgent(?,?,?,?)',
        [
          data.callHistoryId,
          data.domainId,
          data.agentId,
          data.queueId
        ],
        (err: any, result: any) => {
          if (err) {
            console.log('insert err: ', { data, Err: err });
            logger.error('insert err', { data, Err: err });

            reject(err);
          }
          resolve(result?.length > 0 ? result?.[0] : []);
        }
      );
    } catch (error) {
      console.log('insert catch error: ', { data, Err: error });
      logger.error('insert catch', { data, Err: error });
      reject(error);
    }
  });
};
