import { logger } from '../../src/plugins/log';
import { mysqlPoolConnection } from '../plugins/db';

/**
 * @param data
 * @returns
 */
export const queueInboundchatAnalysisMetricFilterDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_queue_inboundchat_analysis_metrics(?,?,?,?,?,?)',
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
            logger.error('queueInboundchatAnalysisMetricFilterDao - Unexpected Error:', err);
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
      logger.error('queueInboundchatAnalysisMetricFilterDao - Unexpected Error:', err);
    }
  });
};

/**
 * @param data
 * @returns
 */
export const queuechatPerformanceAnalysisMetricFilterDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_queue_chat_performances_analysis_metrics(?,?,?,?,?,?)',
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
            logger.error('queuechatPerformanceAnalysisMetricFilterDao - Unexpected Error:', err);
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
      logger.error('queuechatPerformanceAnalysisMetricFilterDao - Unexpected Error:', err);
    }
  });
};

/**
 * @param data
 * @returns
 */
export const queuechatServiceLevelAnalysisMetricFilterDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_chat_Queue_servicelevel_analysis_metrics_filter(?,?,?,?,?,?)',
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
          } else {
            if (result?.length > 0) {
              resolve(result);
              logger.error('queuechatServiceLevelAnalysisMetricFilterDao - Unexpected Error:', err);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (err) {
      console.log(err);
      reject(err);
      logger.error('queuechatServiceLevelAnalysisMetricFilterDao - Unexpected Error:', err);
    }
  });
};

/**
 * @param data
 * @returns
 */
export const createEditQueueWebAnalyticsDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      const editColumnNames = JSON.stringify(data.editColumnNames);
      mysqlPoolConnection.query(
        'call up_Queue_Chat_Edit_Column_insert_update_Details(?,?,?,?)',
        [
          data.id,
          data.domainId,
          data.ext,
          editColumnNames,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('createEditQueueWebAnalyticsDao - Unexpected Error:', err);
            console.log(err);
          }
          resolve(result);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('createEditQueueWebAnalyticsDao - Unexpected Error:', err);
    }
  });
};

/**
 * @param data
 * @returns
 */
export const getCoulumnQueueWebchatAnalyticsDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call QueueChat_Edit_Column_get_details(?,?,?)',
        [
          data?.id,
          data?.domainId,
          data?.ext,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getCoulumnQueueWebchatAnalyticsDao - Unexpected Error:', err);
            console.log(err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log(error);
      reject(error);
      logger.error('getCoulumnQueueWebchatAnalyticsDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getWebChatInboundTrendChartDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_Queue_Webchat_inbound_trendChart(?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.datetype ?? null,
          data?.startTmpstmp ?? null,
          data?.endTmpstmp ?? null,
          data?.columnName ?? null,
          data?.queueId
        ],
        (err: any, result: any) => {
          if (err) {
            console.log('err:', err);
            reject(err);
            logger.error('getWebChatInboundTrendChartDao - Unexpected Error:', err);
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
      logger.error('getWebChatInboundTrendChartDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getWebChatPerformancesTrendChartDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_Queue_Webchat_performances_trendChart(?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.datetype ?? null,
          data?.startTimeStamp ?? null,
          data?.endTimeStamp ?? null,
          data?.columnName ?? null,
          data?.queueId  ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            console.log('err:', err);
            reject(err);
            logger.error('getWebChatPerformancesTrendChartDao - Unexpected Error:', err);
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
      logger.error('getWebChatPerformancesTrendChartDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getWebChatServiceLevelTrendChartDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_Queue_Webchat_ServiceLevel_trendchart(?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.datetype ?? null,
          data?.startTimeStamp ?? null,
          data?.endTimeStamp ?? null,
          data?.columnName ?? null,
          data?.queueId  ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            console.log('err:', err);
            reject(err);
            logger.error('getWebChatServiceLevelTrendChartDao - Unexpected Error:', err);
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
      logger.error('getWebChatServiceLevelTrendChartDao - Unexpected Error:', error);
    }
  });
};
