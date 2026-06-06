import { mysqlPoolConnection } from '../plugins/db';
import { logger } from '../plugins/log';

/**
 *
 * @param data
 * @returns
 */
export const agentSmsInboundMetricFilterDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_Agent_SMS_Inbound_analysis_metrics_filter(?,?,?,?,?,?)',
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
            console.log(err);
          }
          if (result && result.length > 0) {
            resolve(result);
            logger.error('agentSmsInboundMetricFilterDao - Unexpected Error:', err);
          } else {
            reject(err);
          }
        }
      );
    } catch (error) {
      console.log(error);
      reject(error);
      logger.error('agentSmsInboundMetricFilterDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const agentSmsPerformanceMetricFilterDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_sms_agent_performances_analysis_metrics_filter(?,?,?,?,?,?)',
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
            logger.error('agentSmsPerformanceMetricFilterDao - Unexpected Error:', err);
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
      logger.error('agentSmsPerformanceMetricFilterDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const agentSmsServiceLevelMetricFilterDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_sms_agent_servicelevel_analysis_metrics_filter(?,?,?,?,?,?)',
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
            logger.error('agentSmsServiceLevelMetricFilterDao - Unexpected Error:', err);
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
      logger.error('agentSmsServiceLevelMetricFilterDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const smsInboundTrendChartDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_SMS_Agent_Inbound_trendchart(?,?,?,?,?,?)',
        [
          data?.domainId ?? null,
          data?.dateType ?? null,
          data?.startTimeStamp ?? null,
          data?.endTimeStamp ?? null,
          data?.columnName ?? null,
          data?.agentId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('smsInboundTrendChartDao - Unexpected Error:', err);
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
      logger.error('SmsInboundTrendChartDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const smsServicelevelTrendChartDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_Sms_Agent_Servicelevel_trendchart(?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.dateType ?? null,
          data?.startTimeStamp ?? null,
          data?.endTimeStamp ?? null,
          data?.columnName ?? null,
          data?.agentIds ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('smsServicelevelTrendChartDao - Unexpected Error:', err);
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
      logger.error('SmsServicelevelTrendChartDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const smsPerformancesTrendChartDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_Sms_Agent_performances_trendchart(?,?,?,?,?,?)',
        [
          data?.domainId ?? null,
          data?.dateType ?? null,
          data?.startTimeStamp ?? null,
          data?.endTimeStamp ?? null,
          data?.columnName ?? null,
          data?.agentId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('smsPerformancesTrendChartDao - Unexpected Error:', err);
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
      logger.error('SmsPerformancesTrendChartDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const createSmsAnalyticsDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      const editColumnNames = JSON.stringify(data.editColumnNames);
      mysqlPoolConnection.query(
        'call up_InsertUpdateAgentSmsAnalytics_config(?,?,?,?)',
        [
          data.domainId,
          data.ext,
          editColumnNames,
          data.id,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('createSmsAnalyticsDao - Unexpected Error:', err);
            console.log(err);
          }
          resolve(result);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('createSmsAnalyticsDao - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCoulumnSmsAnalyticsDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetAgentSmsAnalyticsEditColumn(?,?,?)',
        [
          data?.domainId,
          data?.ext,
          data?.id,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getCoulumnSmsAnalyticsDao - Unexpected Error:', err);
            console.log(err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log(error);
      reject(error);
      logger.error('getCoulumnSmsAnalyticsDao - Unexpected Error:', error);

    }
  });
};
