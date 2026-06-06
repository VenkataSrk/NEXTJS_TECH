import { mysqlPoolConnection } from '../plugins/db';
import { logger } from '../plugins/log';
import { loggerError, loggerTrace } from '../plugins/logger';

/**
 * delete queue wating
 * @param data
 * @returns
 */
export const deleteQueueWait = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_update_queue_list(?,?)',
        [
          data?.qid ?? null,
          data.sessionId ?? null
        ],
        (err: any, result: any) => {
          loggerTrace('ccaas_update_queue_list', [data?.qid ?? null, data.sessionId ?? null], result);
          if (err) {
            loggerError('ccaas_update_queue_list', [data?.qid ?? null, data.sessionId ?? null], err);
            reject(err);
          } else {
            resolve(result?.length > 0 ? result?.[0] : null);
          }
        }
      );
    } catch (err) {
      loggerError('ccaas_update_queue_list', [data?.qid ?? null, data.sessionId ?? null], err);
      reject(err);
    }
  });
};
