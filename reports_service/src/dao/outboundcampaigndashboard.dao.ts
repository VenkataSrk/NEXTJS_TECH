import { logger } from '../../src/plugins/log';
import { mysqlPoolConnection } from '../plugins/db';

/**
 *
 * @param data
 * @returns
 */
export const callblastDashboardinfodao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_callBlast_dashboard_info(?,?,?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.startDate,
          data?.endDate,
          data?.blastId,
          data?.campaignType,
          data?.triggerType,
          data?.createdBy,
          data?.Status
        ],
        (err: any, result: any) => {
          if (err) {
            console.log('callblastDashboardinfodao only err: ', err);
            reject(err);
            logger.error('callblastDashboardinfodao - Unexpected Error:', err);
          }
          resolve(result);
        }
        );
    } catch (error) {
      console.log('callblastDashboardinfodao only catch error: ', error);
      reject(error);
      logger.error('getAgentCallSummaryReport - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const callblastDashboardchartinfodao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_callBlast_dashboard_barChat(?,?,?,?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.datetype,
          data?.startDateTime,
          data?.endDateTime,
          data?.blastId,
          data?.campaignType,
          data?.triggerType,
          data?.createdBy,
          data?.Status

        ],
        (err: any, result: any) => {
          if (err) {
            console.log('callblastDashboardinfodao only err: ', err);
            reject(err);
            logger.error('callblastDashboardchartinfodao - Unexpected Error:', err);
          }
          resolve(result);
        }
        );
    } catch (error) {
      console.log('callblastDashboardinfodao only catch error: ', error);
      reject(error);
      logger.error('callblastDashboardchartinfodao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const callblastLeaderboardchartinfodao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_callBlast_leaderBoard_info(?,?,?,?)',
        [
          data?.domainId,
          data?.startDate,
          data?.endDate,
          data?.blastId,

        ],
        (err: any, result: any) => {
          if (err) {
            console.log('callblastDashboardinfodao only err: ', err);
            reject(err);
            logger.error('callblastLeaderboardchartinfodao - Unexpected Error:', err);
          }
          resolve(result);
        }
        );
    } catch (error) {
      console.log('callblastDashboardinfodao only catch error: ', error);
      reject(error);
      logger.error('callblastLeaderboardchartinfodao - Unexpected Error:', error);
    }
  });
};
