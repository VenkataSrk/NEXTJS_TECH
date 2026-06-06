
/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import { mysqlPoolConnection } from '../plugins/db';
import { logger } from '../plugins/winston';

/**
 *
 * @param qdata
 * @returns
 */

export const insertUpdateVistorDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL up_InsertUpdateVisitorInsight(?,?,?,?,?,?,?,?,?,?,?,?,?,?)', [
        data?.visitorInsightId,
        data?.customerId,
        data?.visitorId,
        data?.isNew,
        data?.statusTypeId,
        data?.startTime,
        data?.endTime,
        data?.domainId,
        data?.ipAddress,
        data?.location,
        data?.browser,
        data?.device,
        data?.entryPage,
        data?.isVisible
      ],                        (err, response) => {
        if (err) {
          logger.error('up_InsertUpdateVisitorInsight - Unexpected Error:', err);
          reject(err);
        }
        resolve(response);
      });
    } catch (err) {
      logger.error('up_InsertUpdateVisitorInsight - Unexpected Error:', err);
      reject(err);
    }
  });
};
