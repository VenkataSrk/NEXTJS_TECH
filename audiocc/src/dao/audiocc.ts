import { logger } from '../plugin/log';
import { mysqlPoolConnection } from '../plugin/db';

/**
 * Insert message
 * @param data
 * @returns
 */
export const audioMsg = async (data: any) => {
  try {
    const message = JSON.stringify(data?.message);
    return new Promise((resolve, reject) => {
      mysqlPoolConnection.query(
        'call ccaas_call_create_message_details_bot(?,?,?,?,?)', [
          data?.domain_id,
          data?.session_id,
          message ?? null,
          data?.ext ?? null,
          data?.bot ?? null
        ],
        (err: any, response: any) => {
          logger.trace('ccaas_call_create_message_details_bot', data);
          if (err) {
            console.log('audioMsg reject error: ', err);
            logger.error('ccaas_call_create_message_details_bot', data, err);
            reject(err);
          } else {
            resolve(response);
          }
        }
      );
    });
  } catch (error) {
    console.log('audioMsg catch err: ', error);
    logger.error('ccaas_call_create_message_details_bot', data, error);
  }
};
