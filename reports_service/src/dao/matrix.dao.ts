/**
 * @description get Agent Count
 * @param data
 * @returns
 */

// import moment from 'moment';
import { ROLES } from '../helpers/constants';
// import { sessionHistoryModel } from '../models/sessionHistoey';
import { mysqlPoolConnection } from '../plugins/db';
import { logger } from '../plugins/log';
export const dashBoardReport = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_report_dropddown(?)',
        [data],
        (err: any, result: any) => {
          if (err) {
            console.log('ccaas_report_dropdown err: ', err);
            reject(err);
            logger.error('dashBoardReport - Unexpected Error:', err);
          } else {
            if (result[0]?.length !== 0) {
              resolve(result);
            }
          }
        }
      );
    } catch (error) {
      console.log('dashBoardReport catch error: ', error);
      reject(error);
      logger.error('dashBoardReport - Unexpected Error:', error);
    }
  });
};
