import { mysqlPoolConnection } from '../plugins/db';

/**
 *
 * @param data
 * @returns
 */
export const getCompareDashboard = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_dashboard_callandchat_piechart_feautre_details(?,?,?,?,?)',
        [data.startTmpstmp, data.endTmpstmp, data.timeZone, data.domainId, data.extNo],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          }
          resolve(result);
        });
    } catch (err) {
      reject(err);
    }
  });
};
