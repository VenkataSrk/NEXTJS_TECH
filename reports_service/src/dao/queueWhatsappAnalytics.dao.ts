import { logger } from '../../src/plugins/log';
import { mysqlPoolConnection } from '../plugins/db';

/**
 * @param data
 * @returns
 */
export const queueWhatsappInboundAnalysisMetricDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_Queue_Whatapp_Inbound_analysis_metrics_filter(?,?,?,?,?,?)',
        [
          data?.startTmpstmp ?? null,
          data?.endTmpstmp ?? null,
          data?.domainId,
          data?.queueId ?? null,
          data?.offset ?? 0,
          data?.limit ?? 0,
        ],
        (err: any, result: any) => {
          if (err) {
            console.log('err:', err);
            reject(err);
            logger.error('queueWhatsappInboundAnalysisMetricDao - Unexpected Error:', err);
          } else {
            if (result?.length > 0) {
              resolve(result);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (err) {
      console.log(err);
      reject(err);
      logger.error('queueWhatsappInboundAnalysisMetricDao - Unexpected Error:', err);
    }
  });
};

/**
 * @param data
 * @returns
 */
export const queueWhatsappPerformanceAnalysisMetricDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_queue_whatsapp_performances_analysis_metrics(?,?,?,?,?,?)',
        [
          data?.startTmpstmp ?? null,
          data?.endTmpstmp ?? null,
          data?.domainId,
          data?.queueId ?? null,
          data?.offset ?? 0,
          data?.limit ?? 0,
        ],
        (err: any, result: any) => {
          if (err) {
            console.log('err:', err);
            reject(err);
            logger.error('queueWhatsappPerformanceAnalysisMetricDao - Unexpected Error:', err);
          } else {
            if (result?.length > 0) {
              resolve(result);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (err) {
      console.log(err);
      reject(err);
      logger.error('queueWhatsappPerformanceAnalysisMetricDao - Unexpected Error:', err);
    }
  });
};

/**
 * @param data
 * @returns
 */
export const queueWhatsappServcicelevelAnalysisMetricDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_Queue_Whatsapp_Servicelevel_Analysis_Metrics_Filter(?,?,?,?,?,?)',
        [
          data?.startTmpstmp ?? null,
          data?.endTmpstmp ?? null,
          data?.domainId,
          data?.queueId ?? null,
          data?.offset ?? 0,
          data?.limit ?? 0,
        ],
        (err: any, result: any) => {
          if (err) {
            console.log('err:', err);
            reject(err);
            logger.error('queueWhatsappServcicelevelAnalysisMetricDao - Unexpected Error:', err);
          } else {
            if (result?.length > 0) {
              resolve(result);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (err) {
      console.log(err);
      reject(err);
      logger.error('queueWhatsappServcicelevelAnalysisMetricDao - Unexpected Error:', err);
    }
  });
};

/**
 * @param data
 * @returns
 */
export const createEditQueueWhatsappAnalyticsDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      const editColumnNames = JSON.stringify(data.editColumnNames);
      mysqlPoolConnection.query(
        'call queue_Whatsapp_Edit_Column_Update_Insert_Details(?,?,?,?)',
        [
          data.id,
          data.domainId,
          data.ext,
          editColumnNames,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('createEditQueueWhatsappAnalyticsDao - Unexpected Error:', err);
            console.log(err);
          }
          resolve(result);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('createEditQueueWhatsappAnalyticsDao - Unexpected Error:', err);
    }
  });
};

/**
 * @param data
 * @returns
 */
export const getCoulumnQueueWhatsappAnalyticsDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_queue_Whatsapp_Edit_Column_Get_Details(?,?,?)',
        [
          data?.id,
          data?.domainId,
          data?.ext,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getCoulumnQueueWhatsappAnalyticsDao - Unexpected Error:', err);
            console.log(err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log(error);
      reject(error);
      logger.error('getCoulumnQueueWhatsappAnalyticsDao - Unexpected Error:', error);
    }
  });
};

/**
 * @param data
 * @returns
 */
export const getWhatsappInboundTrendChartDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_queue_whatsapp_inbound_trendChart(?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.datetype ?? null,
          data?.startTmpstmp ?? null,
          data?.endTmpstmp ?? null,
          data?.columnName ?? null,
          data?.queueId ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            console.log('err:', err);
            reject(err);
            logger.error('getWhatsappInboundTrendChartDao - Unexpected Error:', err);
          } else {
            if (result?.length > 0) {
              resolve(result);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (error) {
      console.log(error);
      reject(error);
      logger.error('getWhatsappInboundTrendChartDao - Unexpected Error:', error);
    }
  });
};

/**
 * @param data
 * @returns
 */
export const getWhatsappPerformanceTrendChartDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_queue_whatsapp_performances_trendChart(?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.datetype ?? null,
          data?.startTmpstmp ?? null,
          data?.endTmpstmp ?? null,
          data?.columnName ?? null,
          data?.queueId ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            console.log('err:', err);
            reject(err);
            logger.error('getWhatsappPerformanceTrendChartDao - Unexpected Error:', err);

          } else {
            if (result?.length > 0) {
              resolve(result);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (error) {
      console.log(error);
      reject(error);
      logger.error('getWhatsappPerformanceTrendChartDao - Unexpected Error:', error);
    }
  });
};

/**
 * @param data
 * @returns
 */
export const getWhatsappServicelevelTrendChartDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_queue_Whatsapp_Servicelevel_trendChart(?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.datetype ?? null,
          data?.startTmpstmp ?? null,
          data?.endTmpstmp ?? null,
          data?.columnName ?? null,
          data?.queueId ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            console.log('err:', err);
            reject(err);
            logger.error('getWhatsappServicelevelTrendChartDao - Unexpected Error:', err);
          } else {
            if (result?.length > 0) {
              resolve(result);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (error) {
      console.log(error);
      reject(error);
      logger.error('getWhatsappServicelevelTrendChartDao - Unexpected Error:', error);
    }
  });
};
