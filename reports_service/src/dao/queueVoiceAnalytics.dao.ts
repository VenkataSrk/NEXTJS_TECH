import { logger } from '../../src/plugins/log';
import { mysqlPoolConnection } from '../plugins/db';

/**
 *
 * @param data
 * @returns
 */
export const queueInboundcallAnalysisMetricFilterDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_Queue_Inboundcall_analysis_metrics_filter(?,?,?,?,?,?)', [
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
            logger.error('queueInboundcallAnalysisMetricFilterDao - Unexpected Error:', err);
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
      logger.error('queueInboundcallAnalysisMetricFilterDao - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const queueOutboundcallAnalysisMetricFilterDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call  up_Queue_Outboundcall_analysis_metrics_filter(?,?,?,?,?,?)', [
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
            logger.error('queueOutboundcallAnalysisMetricFilterDao - Unexpected Error:', err);
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
      logger.error('queueOutboundcallAnalysisMetricFilterDao - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const queueCallBackAnalysisMetricFilterDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call  ccaas_get_queue_voice_callback_report(?,?,?,?,?,?)', [
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
            logger.error('queueCallBackAnalysisMetricFilterDao - Unexpected Error:', err);
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
      logger.error('queueCallBackAnalysisMetricFilterDao - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const queueVoiceMailAnalysisMetricFilterDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call  up_Queue_Voicemail_analysis_metrics_filter(?,?,?,?,?,?)', [
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
            logger.error('queueVoiceMailAnalysisMetricFilterDao - Unexpected Error:', err);
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
      logger.error('queueVoiceMailAnalysisMetricFilterDao - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const queueServiceLevelAnalysisMetricFilterDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call  up_Call_Queue_Servicelevel_Analysis_Metrics_Filter(?,?,?,?,?,?)', [
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
            logger.error('queueServiceLevelAnalysisMetricFilterDao - Unexpected Error:', err);
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
      logger.error('queueServiceLevelAnalysisMetricFilterDao - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const queueOutboundCallBestTimeMetricsFilterDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call  up_Queue_Outboundcall_BestTime_Call_Analysis_Metrics_Filter(?,?,?,?,?,?)', [
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
            logger.error('queueOutboundCallBestTimeMetricsFilterDao - Unexpected Error:', err);
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
      logger.error('queueOutboundCallBestTimeMetricsFilterDao - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const createEditQueueVoiceAnalyticsDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      const editColumnNames = JSON.stringify(data.editColumnNames);
      mysqlPoolConnection.query(
        'call queue_Voice_Edit_Column_Update_Insert_Details(?,?,?,?)',
        [
          data.id,
          data.domainId,
          data.ext,
          editColumnNames,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('createEditQueueVoiceAnalyticsDao - Unexpected Error:', err);
            console.log(err);
          }
          resolve(result);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('createEditQueueVoiceAnalyticsDao - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCoulumnQueueVoiceAnalyticsDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_queue_Voice_Edit_Column_Get_Details(?,?,?)',
        [
          data?.id,
          data?.domainId,
          data?.ext,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getCoulumnQueueVoiceAnalyticsDao - Unexpected Error:', err);
            console.log(err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log(error);
      reject(error);
      logger.error('getCoulumnQueueVoiceAnalyticsDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getWebVoiceInboundTrendChartDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_queue_call_inbound_trendChart(?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.datetype ?? null,
          data?.startTmpstmp ?? null,
          data?.endTmpstmp ?? null,
          data?.columnName ?? null,
          data?.queueId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            console.log('err:', err);
            reject(err);
            logger.error('getWebVoiceInboundTrendChartDao - Unexpected Error:', err);
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
      logger.error('getWebVoiceInboundTrendChartDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getWebVoiceOutboundTrendChartDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_queue_call_Outboundcall_trendChart(?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.datetype ?? null,
          data?.startTmpstmp ?? null,
          data?.endTmpstmp ?? null,
          data?.columnName ?? null,
          data?.queueId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            console.log('err:', err);
            reject(err);
            logger.error('getWebVoiceOutboundTrendChartDao - Unexpected Error:', err);
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
      logger.error('getWebVoiceOutboundTrendChartDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getWebVoiceCallBackTrendChartDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_queue_call_callBack_trendChart(?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.datetype ?? null,
          data?.startTmpstmp ?? null,
          data?.endTmpstmp ?? null,
          data?.columnName ?? null,
          data?.queueId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            console.log('err:', err);
            reject(err);
            logger.error('getWebVoiceCallBackTrendChartDao - Unexpected Error:', err);
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
      logger.error('getWebVoiceCallBackTrendChartDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getWebVoiceOutboundBestTimeTrendChartDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_Queue_call_Outbound_Best_Time_Call_trendChart(?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.datetype ?? null,
          data?.startTmpstmp ?? null,
          data?.endTmpstmp ?? null,
          data?.columnName ?? null,
          data?.queueId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            console.log('err:', err);
            reject(err);
            logger.error('getWebVoiceOutboundBestTimeTrendChartDao - Unexpected Error:', err);
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
      logger.error('getWebVoiceOutboundBestTimeTrendChartDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getWebVoiceServiceLevelTrendChartDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_queue_call_Servicelevel_trendChart(?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.datetype ?? null,
          data?.startTmpstmp ?? null,
          data?.endTmpstmp ?? null,
          data?.columnName ?? null,
          data?.queueId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            console.log('err:', err);
            reject(err);
            logger.error('getWebVoiceServiceLevelTrendChartDao - Unexpected Error:', err);
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
      logger.error('getWebVoiceServiceLevelTrendChartDao - Unexpected Error:', error);
    }
  });
};
