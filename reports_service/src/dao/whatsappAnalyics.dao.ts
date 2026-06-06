import { logger } from '../../src/plugins/log';
import { mysqlPoolConnection } from '../plugins/db';

/**
 *
 * @param data
 * @returns
 */
export const agentWhatsappInboundMetricFilterDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
          'call up_Agent_Whatapp_Inbound_analysis_metrics_filter(?,?,?,?,?,?)', [
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
              logger.error('agentWhatsappInboundMetricFilterDao - Unexpected Error:', err);
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
      logger.error('agentWhatsappInboundMetricFilterDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const agentWhatsappPerformancesMetricFilterDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
          'call up_ccaas_WhatApp_agent_performances_analysis_metrics_filter(?,?,?,?,?,?)', [
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
              logger.error('agentWhatsappPerformancesMetricFilterDao - Unexpected Error:', err);
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
      logger.error('agentWhatsappPerformancesMetricFilterDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const agentWhatsappServicelevelMetricFilterDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
          'call up_WhatApp_agent_servicelevel_analysis_metrics_filter(?,?,?,?,?,?)', [
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
              logger.error('agentWhatsappServicelevelMetricFilterDao - Unexpected Error:', err);
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
      logger.error('agentWhatsappServicelevelMetricFilterDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const createWhatsappAnalyticsDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      const editColumnNames = JSON.stringify(data.editColumnNames);
      mysqlPoolConnection.query(
        'call up_WhatApp_Edit_Column_updateandinsert_Details(?,?,?,?)',
        [
          data.domainId,
          data.ext,
          editColumnNames,
          data.id,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('createWhatsappAnalyticsDao - Unexpected Error:', err);
            console.log(err);
          }
          resolve(result);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('createWhatsappAnalyticsDao - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCoulumnWhatsappAnalyticsDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_WhatApp_Edit_Column_Get_Details(?,?,?)',
        [
          data?.id,
          data?.domainId,
          data?.ext,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getCoulumnWhatsappAnalyticsDao - Unexpected Error:', err);
            console.log(err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log(error);
      reject(error);
      logger.error('getCoulumnWhatsappAnalyticsDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const whatsappInboundTrendChartDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_WhatsApp_inbound_trendChart(?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.datetype ?? null,
          data?.startTimeStamp  ?? null,
          data?.endTimeStamp ?? null,
          data?.columnName ?? null,
          data?.agentId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('whatsappInboundTrendChartDao - Unexpected Error:', err);
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
      logger.error('whatsappInboundTrendChartDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const whatsappPerformancesTrendChartDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_WhatApp_Performances_trendchart(?,?,?,?,?,?)',
        [
          data?.startTimeStamp ?? null,
          data?.endTimeStamp ?? null,
          data?.domainId,
          data?.datetype ?? null,
          data?.columnName ?? null,
          data?.agentId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('whatsappPerformancesTrendChartDao - Unexpected Error:', err);
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
      logger.error('whatsappPerformancesTrendChartDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const whatsappServicelevelTrendChartDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_WhatApp_Service_Level_trendchart(?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.dateType ?? null,
          data?.startTimeStamp ?? null,
          data?.endTimeStamp ?? null,
          data?.columnName ?? null,
          data?.agentId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('whatsappServicelevelTrendChartDao - Unexpected Error:', err);
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
      logger.error('whatsappServicelevelTrendChartDao - Unexpected Error:', error);
    }
  });
};
