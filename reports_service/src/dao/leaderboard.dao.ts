import { logger } from '../../src/plugins/log';
import { mysqlPoolConnection } from '../plugins/db';

/**
 *
 * @param data
 * @returns
 */
export const getLeaderboardDetails = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_leaderboard_details(?,?,?,?,?,?)',
        [
          data.domain_id,
          data.agentId,
          data.roleid,
          data.search,
          data.startTmpstmp,
          data.endTmpstmp
        ],
        (err: any, result: any) => {
          if (err) {
            resolve([]);
            logger.error('getLeaderboardDetails - Unexpected Error:', err);
          } else {
            resolve(result);
          }

        });

    } catch (err) {
      resolve([]);
      logger.error('getLeaderboardDetails - Unexpected Error:', err);
    }
  });
};

export const getAgentActivityListDetailsModel = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_leaderboard_agent_activity_list_details(?,?,?)',
        [data.domain_id, data.uid, data.ext],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getAgentActivityListDetailsModel - Unexpected Error:', err);
          }
          resolve(result);
        });
    } catch (err) {
      reject(err);
      logger.error('getAgentActivityListDetailsModel - Unexpected Error:', err);
    }
  });
};

export const getAgentActivityDetailsModel = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_leaderboard_agent_activity_details(?,?,?)',
        [data.domain_id, data.uid, data.ext],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getAgentActivityDetailsModel - Unexpected Error:', err);
          }
          resolve(result);
        });
    } catch (err) {
      reject(err);
      logger.error('getAgentActivityDetailsModel - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getLeaderboardListDetails = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_GetDashboardLeaderboardDetails(?,?,?)',
        [
          data.startTime ?? null,
          data.endTime ?? null,
          data.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            resolve([]);
            logger.error('getLeaderboardListDetails - Unexpected Error:', err);
          }
          resolve(result);
        });
    } catch (err) {
      resolve([]);
      logger.error('getLeaderboardListDetails - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getAgentReportDetailsModel = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_leaderBoard_agent_dtl(?,?)',
        [
          data?.agentId ?? null,
          data?.domainId ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            resolve([]);
            logger.error('getAgentReportDetailsModel - Unexpected Error:', err);
          }
          resolve(result);
        });
    } catch (err) {
      resolve([]);
      logger.error('getAgentReportDetailsModel - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getAgentReportRecentChannelDetailsModel = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_leaderBoard_agent_recent_answered_info(?,?)',
        [
          data?.agentId ?? null,
          data?.domainId ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            resolve([]);
            logger.error('getAgentReportRecentChannelDetailsModel - Unexpected Error:', err);
          }
          resolve(result);
        });
    } catch (err) {
      resolve([]);
      logger.error('getAgentReportRecentChannelDetailsModel - Unexpected Error:', err);
    }
  });
};

export const getDispositionForChannelstHandlerDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call get_dispositionStatusDtl_Dashboard(?,?,?,?,?)',
        [
          data?.domainId ?? null,
          data?.channelType ?? null,
          data?.startTmpstmp ?? null,
          data?.endTmpstmp ?? null,
          data?.userId ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getDispositionForChannelstHandlerDao - Unexpected Error:', err);
            console.log('get_dispositionStatusDtl_Dashboard error: ', err);
          } else if (Array.isArray(result) && result.length > 0) {
            resolve(result[0]);
          }
          reject(err);
        }
      );
    } catch (error) {
      console.log('get_dispositionStatusDtl_Dashboard catch error: ', error);
      reject(error);
      logger.error('getDispositionForChannelstHandlerDao - Unexpected Error:', error);
    }
  });
};
