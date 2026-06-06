import { logger } from '../../src/plugins/log';
import { mysqlPoolConnection } from '../plugins/db';

export const getChatDispositionStatus = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_Queue_Status_chart_report(?,?,?,?)', [
        data.startTmpstmp,
        data.endTmpstmp,
        data.timeZone,
        data.domainId,
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('getChatDispositionStatus - Unexpected Error:', err);
          console.log(err);
        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
      logger.error('getChatDispositionStatus - Unexpected Error:', err);
    }
  });
};

export const getChatDispositionViewDetailsDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_Queue_Disposition_Status_chat_report(?,?,?,?,?,?)', [
        data.startTmpstmp,
        data.endTmpstmp,
        data.domainId,
        data.queueName,
        data.dispositionName,
        data.timeZone
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('getChatDispositionViewDetailsDao - Unexpected Error:', err);
        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
      logger.error('getChatDispositionViewDetailsDao - Unexpected Error:', err);
    }
  });
};

export const getAnalyticsQueueChatReport = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('call pros_get_analaytics_queue_chat_Report(?,?,?,?,?,?)', [
        data.startTmpstmp,
        data.endTmpstmp,
        data.domainId,
        data.queueName,
        data.limit,
        data.offset
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('getAnalyticsQueueChatReport - Unexpected Error:', err);
          console.log(err);
        }
        resolve(result);
      });
    } catch (error) {
      reject(error);
      logger.error('getAnalyticsQueueChatReport - Unexpected Error:', error);
    }
  });
};

export const getAnalysisQueueChatReportDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_Queue_Chat_Analysis_Chart_report(?,?,?,?)', [
        data.startTmpstmp,
        data.endTmpstmp,
        data.domainId,
        data.timeZone,
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('getAnalysisQueueChatReportDao - Unexpected Error:', err);
          console.log(err);
        }
        resolve(result);
      });
    } catch (error) {
      reject(error);
      logger.error('getAnalysisQueueChatReportDao - Unexpected Error:', error);
    }
  });
};

export const getAnalyticsSkillChatReport = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('call pros_get_analytics_skill_chat_Report(?,?,?,?,?,?,?,?,?)', [
        data.startTmpstmp,
        data.endTmpstmp,
        data.domainId,
        data.queueName,
        data.agentName,
        data.skillName,
        data.source,
        data.limit,
        data.offset
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('getAnalyticsSkillChatReport - Unexpected Error:', err);
          console.log(err);
        }
        resolve(result);
      });
    } catch (error) {
      reject(error);
      logger.error('getAnalyticsSkillChatReport - Unexpected Error:', error);
    }
  });
};

export const getAnalyticsChatSessionReport = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('call pros_agent_session_Report(?,?,?,?,?)', [
        data.startTmpstmp,
        data.endTmpstmp,
        data.domainId,
        data.limit,
        data.offset
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('getAnalyticsChatSessionReport - Unexpected Error:', err);
          console.log(err);
        }
        resolve(result);
      });
    } catch (error) {
      reject(error);
      logger.error('getAnalyticsChatSessionReport - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const inboundChatAnalysisMetricsApi = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_inboundchat_analysis_metrics(?,?,?)',
        [
          data?.domainId,
          data?.offset,
          data?.limit
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getAnalysisQueueChatReportDao - Unexpected Error:', err);
            console.log(err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log(error);
      reject(error);
      logger.error('inboundChatAnalysisMetricsApi - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const chatPerformanceMetricsApi = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_chat_performances_metrics_analysis(?,?,?)',
        [
          data?.domainId,
          data?.limit,
          data?.offset
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('chatPerformanceMetricsApi - Unexpected Error:', err);
            console.log(err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log(error);
      reject(error);
      logger.error('chatPerformanceMetricsApi - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const serviceLevelAgreementMetricsApi = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_services_level_agreement_metrics_analysis(?,?,?)',
        [
          data?.domainId,
          data?.limit,
          data?.offset
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('serviceLevelAgreementMetricsApi - Unexpected Error:', err);
            console.log(err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log(error);
      reject(error);
      logger.error('serviceLevelAgreementMetricsApi - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const inboundChatAnalysisDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_Inboundchat_analysis_metrics_filter(?,?,?,?,?,?)',
        [
          data?.startTimeStamp ?? null,
          data?.endTimeStamp ?? null,
          data?.domainId,
          data?.queueId ?? null,
          data?.offset ?? 0,
          data?.limit ?? 0
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('inboundChatAnalysisDao - Unexpected Error:', err);
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
      logger.error('inboundChatAnalysisDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const chatPerformanceDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_chat_performances_metrics_filter(?,?,?,?,?,?)',
        [
          data?.startTimeStamp ?? null,
          data?.endTimeStamp ?? null,
          data?.domainId,
          data?.queueId ?? null,
          data?.offset ?? 0,
          data?.limit ?? 0
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('chatPerformanceDao - Unexpected Error:', err);
            console.log(err);
          }
          if (result && result?.length > 0) {
            resolve(result);
          } else {
            reject(err);
          }
        }
      );
    } catch (error) {
      console.log(error);
      reject(error);
      logger.error('chatPerformanceDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const chatServiceLevel = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_chat_servicelevel_metrics_filter(?,?,?,?,?,?)',
        [
          data?.startTimeStamp ?? null,
          data?.endTimeStamp ?? null,
          data?.domainId,
          data?.queueId ?? null,
          data?.offset ?? 0,
          data?.limit ?? 0
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('chatServiceLevel - Unexpected Error:', err);
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
      logger.error('chatServiceLevel - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const inboundTrendChat = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_Inbound_trendchart(?,?,?,?)',
        [
          data?.startTimeStamp ?? null,
          data?.endTimeStamp ?? null,
          data?.domainId,
          data?.columnName ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('chatServiceLevel - Unexpected Error:', err);
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
      logger.error('inboundTrendChat - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const agentInboundTrendChat = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_agent_Inbound_trendchart(?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.dateType,
          data?.startTimeStamp ?? null,
          data?.endTimeStamp ?? null,
          data?.columnName ?? null,
          data?.agentId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('agentInboundTrendChat - Unexpected Error:', err);
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
      logger.error('agentInboundTrendChat - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const agentPerfomanceTrendChat = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_agent_chat_performances_metrics_Trend_Chart(?,?,?,?,?,?)',
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
            logger.error('agentPerfomanceTrendChat - Unexpected Error:', err);
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
      logger.error('agentPerfomanceTrendChat - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const chatPerformanceTrendChat = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call chat_performances_metrics_Trend_Chart(?,?,?,?)',
        [
          data?.startTimeStamp ?? null,
          data?.endTimeStamp ?? null,
          data?.domainId,
          data?.columnName ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('chatPerformanceTrendChat - Unexpected Error:', err);
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
      logger.error('chatPerformanceTrendChat - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const serviceLevelTrendChat = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_services_level_metrics_Trend_Chart(?,?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.dateType ?? null,
          data?.startTimeStamp ?? null,
          data?.endTimeStamp ?? null,
          data?.columnName ?? null,
          data?.agentId ?? null,
          data?.timeZone ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('serviceLevelTrendChat - Unexpected Error:', err);
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
      logger.error('inboundChatAnalysisMetricsApi - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const agentInboundChatAnalysis = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_inboundchat_agent_analysis_metrics(?,?,?)',
        [
          data?.domainId,
          +(data?.offset),
          +(data?.limit)
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('agentInboundChatAnalysis - Unexpected Error:', err);
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
      logger.error('agentInboundChatAnalysis - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const agentPerformanceChatAnalysis = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_chat_agent_performances_metrics_analysis(?,?,?)',
        [
          data?.domainId,
          +(data?.limit),
          +(data?.offset)
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('agentPerformanceChatAnalysis - Unexpected Error:', err);
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
      logger.error('agentPerformanceChatAnalysis - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const agentServiceLevelAnalysis = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_services_level_agent_agreement_metrics_analysis(?,?,?)',
        [
          data?.domainId,
          +(data?.offset),
          +(data?.limit)
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('agentServiceLevelAnalysis - Unexpected Error:', err);
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
      logger.error('agentServiceLevelAnalysis - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getInboundQueueField = () => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL up_getinbound_queue_field()', (err, result) => {
        if (err) {
          console.error(err);
          reject(err);
          logger.error('getInboundQueueField - Unexpected Error:', err);
        } else {
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      });
    } catch (error) {
      console.error(error);
      reject(error);
      logger.error('getInboundQueueField - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getInboundAgentField = () => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL up_getinbound_agent_field()', (err, result) => {
        if (err) {
          console.error(err);
          reject(err);
          logger.error('getInboundAgentField - Unexpected Error:', err);
        } else {
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      });
    } catch (error) {
      console.error(error);
      reject(error);
      logger.error('getInboundAgentField - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getInboundPerformance = () => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL up_getagentperformancefield()', (err, result) => {
        if (err) {
          console.error(err);
          reject(err);
          logger.error('getInboundPerformance - Unexpected Error:', err);
        } else {
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      });
    } catch (error) {
      console.error(error);
      reject(error);
      logger.error('getInboundPerformance - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getInboundQueuePerformance = () => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL get_queue_performancefield()', (err, result) => {
        if (err) {
          console.error(err);
          reject(err);
          logger.error('getInboundQueuePerformance - Unexpected Error:', err);
        } else {
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      });
    } catch (error) {
      console.error(error);
      reject(error);
      logger.error('getInboundQueuePerformance - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getInboundQueueServiceLevel = () => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL get_queue_servicelevelfield()', (err, result) => {
        if (err) {
          console.error(err);
          reject(err);
          logger.error('getInboundQueueServiceLevel - Unexpected Error:', err);
        } else {
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      });
    } catch (error) {
      console.error(error);
      reject(error);
      logger.error('getInboundQueueServiceLevel - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getInboundQueueServiceLevelSla = () => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL get_queue_servicelevelSLAfield()', (err, result) => {
        if (err) {
          console.error(err);
          reject(err);
          logger.error('getInboundQueueServiceLevelSla - Unexpected Error:', err);
        } else {
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      });
    } catch (error) {
      console.error(error);
      reject(error);
      logger.error('getInboundQueueServiceLevelSla - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getInboundAgentServiceLevel = () => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL up_get_agent_servicelevelfield()', (err, result) => {
        if (err) {
          console.error(err);
          reject(err);
          logger.error('getInboundAgentServiceLevel - Unexpected Error:', err);
        } else {
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      });
    } catch (error) {
      console.error(error);
      reject(error);
      logger.error('getInboundAgentServiceLevel - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getInboundAgentServiceLevelSla = () => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL up_getagentservicelevelSLAfield()', (err, result) => {
        if (err) {
          console.error(err);
          reject(err);
          logger.error('getInboundAgentServiceLevelSla - Unexpected Error:', err);
        } else {
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      });
    } catch (error) {
      console.error(error);
      reject(error);
      logger.error('getInboundAgentServiceLevelSla - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const editQueueAgent = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      const editColumnNames = JSON.stringify(data.editColumnNames);
      mysqlPoolConnection.query('CALL Edit_Column_updateandinsert_Details(?,?,?)', [
        data.domainId,
        data.ext,
        editColumnNames,
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('editQueueAgent - Unexpected Error:', err);
          console.log(err);
        }
        if (result && result.length > 0) {
          resolve(result[0]);
        } else {
          resolve([]);
        }
      });
    } catch (err) {
      reject(err);
      logger.error('editQueueAgent - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getEditColumnAnalytics = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL Edit_Column_get_details(?,?)', [
        data.domainId,
        data.ext,
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('editQueueAgent - Unexpected Error:', err);
          console.log(err);
        }
        if (result && result.length > 0) {
          resolve(result[0]);
        } else {
          resolve([]);
        }
      });
    } catch (err) {
      reject(err);
      logger.error('getEditColumnAnalytics - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const queueInboundChatVolume = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL up_ccaas_Inbound_chat_volume_Analysis(?,?,?,?)', [
        data?.startTimeStamp ?? null,
        data?.endTimeStamp ?? null,
        data?.domainId,
        data?.volumeName ?? null
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('queueInboundChatVolume - Unexpected Error:', err);
          console.log(err);
        }
        if (result && result.length > 0) {
          resolve(result[0]);
        } else {
          resolve([]);
        }
      });
    } catch (err) {
      reject(err);
      logger.error('queueInboundChatVolume - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const agentInboundChatVolume = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL up_ccaas_agent_Inbound_chat_volume_Analysis(?,?,?,?)', [
        data?.startTimeStamp ?? null,
        data?.endTimeStamp ?? null,
        data?.domainId,
        data?.volumeName ?? null
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('agentInboundChatVolume - Unexpected Error:', err);
          console.log(err);
        }
        if (result && result.length > 0) {
          resolve(result[0]);
        } else {
          resolve([]);
        }
      });
    } catch (err) {
      reject(err);
      logger.error('AgentInboundChatVolume - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const agentInboundChatAnalysisDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_Agent_Inboundchat_analysis_metrics_filter(?,?,?,?,?,?)',
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
            logger.error('agentInboundChatAnalysisDao - Unexpected Error:', err);
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
      logger.error('agentInboundChatAnalysisDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const agentChatPerformanceMetricFilterDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_chat_agent_performances_analysis_metrics_filter(?,?,?,?,?,?)',
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
            logger.error('agentChatPerformanceMetricFilterDao - Unexpected Error:', err);
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
      logger.error('agentChatPerformanceMetricFilterDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const agentChatServicelevelMetricFilterDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_chat_agent_servicelevel_analysis_metrics_filter(?,?,?,?,?,?)',
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
            logger.error('agentChatServicelevelMetricFilterDao - Unexpected Error:', err);
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
      logger.error('agentChatServicelevelMetricFilterDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const agentServiceLevelAgreementMetricsDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_agent_services_level_agreement_metrics_analysis(?,?,?)',
        [
          data?.domainId,
          +(data?.limit),
          +(data?.offset)
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('agentServiceLevelAgreementMetricsDao - Unexpected Error:', err);
            console.log(err);
          }
          if (result && result.length > 0) {
            console.log('agentPerformanceChatAnalysis:::', result);
            resolve(result);
          } else {
            reject(err);
          }
        }
      );
    } catch (error) {
      console.log(error);
      reject(error);
      logger.error('agentServiceLevelAgreementMetricsDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const createAgentAnalyticsDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      const editColumnNames = JSON.stringify(data.editColumnNames);
      mysqlPoolConnection.query(
        'CALL up_InsertUpdateAgentAnalytics_config(?,?,?,?)',
        [
          data.domainId,
          data.ext,
          editColumnNames,
          data.id,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('createAgentAnalyticsDao - Unexpected Error:', err);
            console.log(err);
          }
          resolve(result);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('createAgentAnalyticsDao - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCoulumnAgentAnalyticsDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetAgentAnalyticsEditColumn(?,?,?)',
        [
          data?.domainId,
          data?.ext,
          data?.id,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getCoulumnAgentAnalyticsDao - Unexpected Error:', err);
            console.log(err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log(error);
      reject(error);
      logger.error('getCoulumnAgentAnalyticsDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertUpdateVisitorDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_InsertUpdateVisitorInsight(?,?,?,?,?,?,?,?,?,?)',
        [
          data?.visitorId ?? null,
          data?.domainId ?? null,
          data?.customerId ?? null,
          data?.sessionStartTime ?? null,
          data?.sessionEndTime ?? null,
          data?.location ?? null,
          data?.device ?? null,
          data?.ipAddress ?? null,
          data?.browser ?? null,
          data?.language ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('insertUpdateVisitorDao - Unexpected Error:', err);
            console.log(err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log(error);
      reject(error);
      logger.error('insertUpdateVisitorDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getVisitorInsightDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_GetVisitorInsightsReports(?,?,?)',
        [
          data?.domainId,
          data?.limit,
          data?.offset,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getVisitorInsightDao - Unexpected Error:', err);
            console.log(err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log(error);
      reject(error);
      logger.error('getVisitorInsightDao - Unexpected Error:', error);
    }
  });
};
