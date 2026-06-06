/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */
import { ROLES } from '../helpers/constants';
import { mysqlPoolConnection } from '../plugins/db';
import { logger } from '../plugins/winston';

/**
 *
 * @param data
 * @returns
 */
export const getSupervisorDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_supervisor_details_by_role_id(?,?)',
        [
          ROLES?.SUPERVISOR_ROLE_ID,
          data?.domainId,
        ],
        (err, result) => {
          if (err) {
            logger.error('ccaas_get_supervisor_details_by_role_id - Unexpected Error:', err);

            reject(err);
          } else {
            resolve(result?.[0]);
          }
        });
    } catch (error) {
      logger.error('ccaas_get_supervisor_details_by_role_id - Unexpected Error:', error);

      reject(error);
    }
  });
};
