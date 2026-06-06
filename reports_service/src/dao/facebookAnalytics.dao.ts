import { logger } from '../../src/plugins/log';
import { mysqlPoolConnection } from '../plugins/db';

/**
 *
 * @param data
 * @returns
 */
export const agentFacebookInboundMetricFilterDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_Agent_Facebook_Inbound_analysis_metrics_filter(?,?,?,?,?,?)', [
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
            logger.error('agentFacebookInboundMetricFilterDao - Unexpected Error:', err);
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
      logger.error('agentFacebookInboundMetricFilterDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const agentFacebookPerformanceMetricFilterDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_Facebook_agent_performances_analysis_metrics_filter(?,?,?,?,?,?)', [
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
            logger.error('agentFacebookPerformanceMetricFilterDao - Unexpected Error:', err);
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
      logger.error('agentFacebookPerformanceMetricFilterDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const agentFacebookServicelevelMetricFilterDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_Facebook_agent_servicelevel_analysis_metrics_filter(?,?,?,?,?,?)', [
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
            logger.error('agentFacebookServicelevelMetricFilterDao - Unexpected Error:', err);
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
      logger.error('agentFacebookServicelevelMetricFilterDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const createEditFaceBookAnalyticsDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      const editColumnNames = JSON.stringify(data.editColumnNames);
      mysqlPoolConnection.query(
        'call up_Facebook_Edit_Column_updateandinsert_Details(?,?,?,?)',
        [
          data.domainId,
          data.ext,
          editColumnNames,
          data.id,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('createEditFaceBookAnalyticsDao - Unexpected Error:', err);
            console.log(err);
          }
          resolve(result);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('createEditFaceBookAnalyticsDao - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCoulumnFacebookAnalyticsDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_Facebook_Edit_Column_Get_Details(?,?,?)',
        [
          data?.id,
          data?.domainId,
          data?.ext,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getCoulumnFacebookAnalyticsDao - Unexpected Error:', err);
            console.log(err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log(error);
      reject(error);
      logger.error('getCoulumnFacebookAnalyticsDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const facebookInboundTrendChartDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_Facebook_inbound_trendChart(?,?,?,?,?,?)',
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
            logger.error('facebookInboundTrendChartDao - Unexpected Error:', err);
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
      logger.error('facebookInboundTrendChartDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const facebookPerformancesTrendChartDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_Facebook_Performances_trendchart(?,?,?,?,?,?)',
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
            logger.error('facebookInboundTrendChartDao - Unexpected Error:', err);
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
      logger.error('facebookPerformancesTrendChartDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const facebookServicelevelTrendChartDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_Facebook_ServiceLevel_trendchart(?,?,?,?,?,?)',
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
            logger.error('facebookServicelevelTrendChartDao - Unexpected Error:', err);
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
      logger.error('facebookServicelevelTrendChartDao - Unexpected Error:', error);

    }
  });
};
