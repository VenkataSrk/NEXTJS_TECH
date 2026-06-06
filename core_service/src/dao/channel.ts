/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */
import { mysqlPoolConnectionRole } from '../plugins/db';
import { logger } from '../plugins/winston';

/**
 *
 * @param qdata
 * @returns
 */

export const forgotPassword = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnectionRole.query('CALL Ur_app_login_forgot_pwd(?,?,?,?,?)', [
        data.email,
        data.new_pwd,
        data.type,
        data.called_by,
        data.log_id
      ],                            (err, response) => {
        if (err) {
          logger.error('Ur_app_login_forgot_pwd - Unexpected Error:', err);
          reject(err);
        }
        const result = JSON.parse(JSON.stringify(response?.[0]));
        resolve(result);
      });
    } catch (err) {
      logger.error('Ur_app_login_forgot_pwd - Unexpected Error:', err);
      reject(err);
    }
  });
};
