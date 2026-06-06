import { logger } from '../../src/plugins/log';
import { mysqlPoolConnection } from '../plugins/db';

/**
 *
 * @param data
 * @returns
 */
export const agentCampaignDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_Agent_Outbound_Campaign_Report_Metrics(?,?,?)',
        [
          data?.domainId,
          data?.startDate,
          data?.endDate,
        ],
        (err: any, result: any) => {
          if (err) {
            console.log('agentCampaignDao only err: ', err);
            reject(err);
            logger.error('agentCampaignDao - Unexpected Error:', err);
          }
          resolve(result);
        }
          );
    } catch (error) {
      console.log('agentCampaignDao only catch error: ', error);
      reject(error);
      logger.error('agentCampaignDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const summaryCampaignDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_Campaign_Summary_Report_Metrics(?,?,?)',
        [
          data?.domainId,
          data?.startDate,
          data?.endDate,
        ],
        (err: any, result: any) => {
          if (err) {
            console.log('agentCampaignDao only err: ', err);
            reject(err);
            logger.error('summaryCampaignDao - Unexpected Error:', err);
          }
          resolve(result);
        }
          );
    } catch (error) {
      console.log('agentCampaignDao only catch error: ', error);
      reject(error);
      logger.error('summaryCampaignDao - Unexpected Error:', error);
    }
  });
};
