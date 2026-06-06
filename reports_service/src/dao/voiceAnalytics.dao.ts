
import { logger } from '../../src/plugins/log';
import { mysqlPoolConnection } from '../plugins/db';

/**
 * @description get Metrics Queue Inbound Call
 * @param data
 * @returns
 */
export const getMetricsQueueInboundCall = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_Queue_Metrics_Report_for_Inbound_Call(?,?,?,?)', [
        data.startTmpstmp,
        data.endTmpstmp,
        data.timeZone,
        data.domainId,
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('getMetricsQueueInboundCall - Unexpected Error:', err);
          console.log(err);
        }
        resolve(result.length > 0 ? result[0] : null);
      });
    } catch (err) {
      reject(err);
      logger.error('getMetricsQueueInboundCall - Unexpected Error:', err);
    }
  });
};

/**
 * ccaas Abandoned Call Profile Report
 * @param data
 * @returns
 */
export const getAbandonedCallProfileAnalytic = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_Abandoned_Call_Profile_Report(?,?,?,?,?,?)', [
        data.startTmpstmp,
        data.endTmpstmp,
        data.timeZone,
        data.domainId,
        data.p_Limit,
        data.p_offset
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('getAbandonedCallProfileAnalytic - Unexpected Error:', err);
          console.log(err);
        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
      logger.error('getAbandonedCallProfileAnalytic - Unexpected Error:', err);
    }
  });
};

export const editAnalyticsColumnDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    const editColumnName = JSON.stringify(data?.editColumn);
    try {
      mysqlPoolConnection.query(
        'call up_insertupdate_analytics_config(?,?,?,?,?)',
        [
          data?.analyticsId,
          data?.domainId,
          data?.ext,
          editColumnName,
          data?.analyticsType,
        ],
        (err: any, result: any) => {
          if (err) {
            console.log('up_insertupdate_analytics_config error: ', err);
            reject(err);
            logger.error('editAnalyticsColumnDao - Unexpected Error:', err);
          }
          resolve(result);
        }
      );
    } catch (err) {
      reject(err);
      console.log('up_InsertUpdatereport_config catch error: ', err);
      logger.error('editAnalyticsColumnDao - Unexpected Error:', err);
    }
  });
};

export const getAnalyticsColumnDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetReportEditColumn(?)',
        [
          data?.reportId
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getAnalyticsColumnDao - Unexpected Error:', err);
            console.log('up_GetReportEditColumn err: ', err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log('getEditReportColumnDao error catch: ', error);
      reject(error);
      logger.error('getAnalyticsColumnDao - Unexpected Error:', error);
    }
  });
};

/**
 * get Interv alAnalytics
 * @param data
 * @returns
 */
export const getIntervalAnalytics = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_Queue_Metrics_Interval_report(?,?,?,?,?,?)', [
        data.startTmpstmp,
        data.endTmpstmp,
        data.timeZone,
        data.domainId,
        data.p_Limit,
        data.p_offset
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('getIntervalAnalytics - Unexpected Error:', err);
          console.log(err);
        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
      logger.error('getIntervalAnalytics - Unexpected Error:', err);
    }
  });
};

/**
 * get Queue Disposition
 * @param data
 * @returns
 */
export const getQueueDispositionDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_Queue_Disposition_Status_report(?,?,?,?)', [
        data.startTmpstmp,
        data.endTmpstmp,
        data.timeZone,
        data.domainId,
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('getQueueDispositionDao - Unexpected Error:', err);
          console.log(err);
        }
        resolve(result.length > 0 ? result[0] : null);
      });
    } catch (err) {
      reject(err);
      logger.error('getQueueDispositionDao - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const agentInboundCallAnalysisDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_Agent_Inboundcall_analysis_metrics_filter(?,?,?,?,?,?)',
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
            logger.error('agentInboundCallAnalysisDao - Unexpected Error:', err);
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
      logger.error('agentInboundCallAnalysisDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const agentOutboundCallAnalysisDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_Agent_Outboundcall_analysis_metrics_filter(?,?,?,?,?,?)',
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
            logger.error('agentOutboundCallAnalysisDao - Unexpected Error:', err);
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
      logger.error('agentOutboundCallAnalysisDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const agentVoiceMailAnalysisDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_Agent_Voicemail_analysis_metrics_filter(?,?,?,?,?,?)',
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
            logger.error('agentVoiceMailAnalysisDao - Unexpected Error:', err);
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
      logger.error('agentVoiceMailAnalysisDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const agentCallBackAnalysisDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_voice_callback_report(?,?,?,?,?,?)',
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
            logger.error('agentCallBackAnalysisDao - Unexpected Error:', err);
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
      logger.error('agentCallBackAnalysisDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const agentServiceLevelCallAnalysisDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_call_agent_servicelevel_analysis_metrics_filter(?,?,?,?,?,?)',
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
            logger.error('agentServiceLevelCallAnalysisDao - Unexpected Error:', err);
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
      logger.error('agentServiceLevelCallAnalysisDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const createCallAnalyticsDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      const editColumnNames = JSON.stringify(data.editColumnNames);
      mysqlPoolConnection.query(
        'call up_InsertUpdateVoiceAnalytics_config(?,?,?,?)',
        [
          data.domainId,
          data.ext,
          editColumnNames,
          data.id,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('createCallAnalyticsDao - Unexpected Error:', err);
            console.log(err);
          }
          resolve(result);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('createCallAnalyticsDao - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCoulumnCallAnalyticsDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call Voice_Edit_Column_get_details(?,?,?)',
        [
          data?.id,
          data?.domainId,
          data?.ext,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getCoulumnCallAnalyticsDao - Unexpected Error:', err);
            console.log(err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log(error);
      reject(error);
      logger.error('getCoulumnCallAnalyticsDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const callInboundTrendChartDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_Inbound_call_trendchart(?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.datetype ?? null,
          data?.startTmpstmp ?? null,
          data?.endTmpstmp ?? null,
          data?.columnName ?? null,
          data?.agentId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('callInboundTrendChartDao - Unexpected Error:', err);
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
      logger.error('CallInboundTrendChartDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const callOutboundTrendChartDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_Outbound_call_trendchart(?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.datetype ?? null,
          data?.startTimeStamp ?? null,
          data?.endTimeStamp ?? null,
          data?.columnName ?? null,
          data?.agentId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('callOutboundTrendChartDao - Unexpected Error:', err);
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
      logger.error('CallOutboundTrendChartDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const callServicelevelTrendChartDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_Voice_agent_servicelevel_analysis_trendchart(?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.datetype ?? null,
          data?.startTimeStamp ?? null,
          data?.endTimeStamp ?? null,
          data?.columnName ?? null,
          data?.agentId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('callServicelevelTrendChartDao - Unexpected Error:', err);
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
      logger.error('CallServicelevelTrendChartDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const callBackTrendChartDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_Voice_Agent_callback_trendchart(?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.datetype ?? null,
          data?.startTimeStamp ?? null,
          data?.endTimeStamp ?? null,
          data?.columnName ?? null,
          data?.agentId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('callBackTrendChartDao - Unexpected Error:', err);
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
      logger.error('CallBackTrendChartDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const voiceMailTrendChartDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_VoiceMail_trendchart(?,?,?,?)',
        [
          data?.startTimeStamp ?? null,
          data?.endTimeStamp ?? null,
          data?.domainId,
          data?.columnName ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('voiceMailTrendChartDao - Unexpected Error:', err);
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
      logger.error('VoiceMailTrendChartDao - Unexpected Error:', error);
    }
  });
};
