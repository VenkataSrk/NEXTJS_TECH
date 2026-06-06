import { logger } from '../../src/plugins/log';
import { mysqlPoolConnection } from '../plugins/db';

/**
 *
 * @param data
 * @returns
 */
export const agentInstagramInboundMetricFilterDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_Agent_Instagram_Inbound_analysis_metrics_filter(?,?,?,?,?,?)', [
          data?.startTimeStamp ?? null,
          data?.endTimeStamp ?? null,
          data?.domainId,
          data.agentId ?? null,
          data?.offset ?? 0,
          data?.limit ?? 0,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('agentInstagramInboundMetricFilterDao - Unexpected Error:', err);
            console.log(err);
          }
          if (result && result.length > 0) {
            resolve(result);
          } else {
            reject(err);
          }
        }
      );
    } catch (error) {
      console.log(error);
      reject(error);
      logger.error('agentInstagramInboundMetricFilterDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const agentInstagramPerformanceMetricFilterDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_Instagram_agent_performances_analysis_metrics_filter(?,?,?,?,?,?)',
        [
          data?.startTimeStamp ?? null,
          data?.endTimeStamp ?? null,
          data?.domainId,
          data.agentId ?? null,
          data?.offset ?? 0,
          data?.limit ?? 0,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('agentInstagramPerformanceMetricFilterDao - Unexpected Error:', err);
            console.log(err);
          }
          if (result && result.length > 0) {
            resolve(result);
          } else {
            reject(err);
          }
        }
      );
    } catch (error) {
      console.log(error);
      reject(error);
      logger.error('agentInstagramPerformanceMetricFilterDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const  agentInstagramServiceLevelMetricFilterDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_Instagram_agent_servicelevel_analysis_metrics_filter(?,?,?,?,?,?)',
        [
          data?.startTimeStamp ?? null,
          data?.endTimeStamp ?? null,
          data?.domainId,
          data.agentId ?? null,
          data?.offset ?? 0,
          data?.limit ?? 0,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('agentInstagramServiceLevelMetricFilterDao - Unexpected Error:', err);
            console.log(err);
          }
          if (result && result.length > 0) {
            resolve(result);
          } else {
            reject(err);
          }
        }
      );
    } catch (error) {
      console.log(error);
      reject(error);
      logger.error('agentInstagramServiceLevelMetricFilterDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const createEditInstagramAnalyticsDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      const editColumnNames = JSON.stringify(data.editColumnNames);
      mysqlPoolConnection.query(
        'call up_Instagram_Edit_Column_insertandupdate_Details(?,?,?,?)',
        [
          data.domainId,
          data.ext,
          editColumnNames,
          data.id,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('createEditInstagramAnalyticsDao - Unexpected Error:', err);
            console.log(err);
          }
          resolve(result);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('createEditInstagramAnalyticsDao - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCoulumnInstagramAnalyticsDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_Instagram_Edit_Column_Get_Details(?,?,?)',
        [
          data?.id,
          data?.domainId,
          data?.ext,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getCoulumnInstagramAnalyticsDao - Unexpected Error:', err);
            console.log(err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log(error);
      reject(error);
      logger.error('getCoulumnInstagramAnalyticsDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const instagramInboundTrendChartDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_Instagram_inbound_trendChart(?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.datetype ?? null,
          data?.startTimeStamp ?? null,
          data?.endTimeStamp ?? null,
          data?.columnName ?? null,
          data?.agentIds ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('instagramInboundTrendChartDao - Unexpected Error:', err);
            console.log(err);
          }
          if (result && result.length > 0) {
            resolve(result);
          } else {
            reject(err);
          }
        }
      );
    } catch (error) {
      console.log(error);
      reject(error);
      logger.error('instagramInboundTrendChartDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const instagramPerformanceTrendChartDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_Instagram_Performances_trendchart(?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.datetype ?? null,
          data?.startTimeStamp ?? null,
          data?.endTimeStamp ?? null,
          data?.columnName ?? null,
          data?.agentIds ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('instagramPerformanceTrendChartDao - Unexpected Error:', err);
            console.log(err);
          }
          if (result && result.length > 0) {
            resolve(result);
          } else {
            reject(err);
          }
        }
      );
    } catch (error) {
      console.log(error);
      reject(error);
      logger.error('instagramPerformanceTrendChartDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const instagramServicelevelTrendChartDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call  up_ccaas_Instagram_ServiceLevel_trendchart(?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.datetype ?? null,
          data?.startTimeStamp ?? null,
          data?.endTimeStamp ?? null,
          data?.columnName ?? null,
          data?.agentIds ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('instagramServicelevelTrendChartDao - Unexpected Error:', err);
            console.log(err);
          }
          if (result && result.length > 0) {
            resolve(result);
          } else {
            reject(err);
          }
        }
      );
    } catch (error) {
      console.log(error);
      reject(error);
      logger.error('instagramServicelevelTrendChartDao - Unexpected Error:', error);
    }
  });
};
