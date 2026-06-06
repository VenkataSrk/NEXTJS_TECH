import { logger } from '../../src/plugins/log';
import { mysqlPoolConnection } from '../plugins/db';

/**s
 *
 * @param data
 * @returns
 */
export const agentAnalysisBotInteractionDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_analyticsBotInteraction(?,?,?)',
        [
          data?.startTimeStamp ?? null,
          data?.endTimeStamp ?? null,
          data?.domainId,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('agentAnalysisBotInteractionDao - Unexpected Error:', err);
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
      logger.error('agentAnalysisBotInteractionDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const agentAnalysisBotTrendChartDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_analytics_trend_chart_info(?,?,?)',
        [
          data?.startTimeStamp ?? null,
          data?.endTimeStamp ?? null,
          data?.domainId,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('agentAnalysisBotTrendChartDao - Unexpected Error:', err);
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
      logger.error('agentAnalysisBotTrendChartDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const agentBotInteractionChannelDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_analyticsBotInteractionChannel(?,?,?)',
        [
          data?.startTimeStamp ?? null,
          data?.endTimeStamp ?? null,
          data?.domainId,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('agentBotInteractionChannelDao - Unexpected Error:', err);
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
      logger.error('agentBotInteractionChannelDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const agentAnalyticPerformanceComparisonHandlerDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_DashboardAgentBotComparePerform(?,?,?,?)',
        [
          data?.startTimeStamp,
          data?.endTimeStamp,
          data?.domainId,
          data?.channelType,
        ],
        (err: any, result: any) => {
          if (err) {
            console.log('getDashboardPerformanceComparisonHandlerDao only err: ', err);
            reject(err);
            logger.error('agentAnalyticPerformanceComparisonHandlerDao - Unexpected Error:', err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log('getDashboardUtilizationHandlerDao only catch error: ', error);
      reject(error);
      logger.error('getDashboardUtilizationHandlerDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const agentAnalyticActiveHoursDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_analytics_interaction_overview_trend_chat(?,?,?)',
        [
          data?.startTimeStamp,
          data?.endTimeStamp,
          data?.domainId,
        ],
        (err: any, result: any) => {
          if (err) {
            console.log('AgentAnalyticActiveHoursDao err: ', err);
            reject(err);
            logger.error('agentAnalyticActiveHoursDao - Unexpected Error:', err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log('AgentAnalyticActiveHoursDao catch error: ', error);
      reject(error);
      logger.error('AgentAnalyticActiveHoursDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const agentAssistBotAnalysisDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_analyticsBotAgentAssist(?,?,?)',
        [
          data?.startTimeStamp ?? null,
          data?.endTimeStamp ?? null,
          data?.domainId,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('agentAssistBotAnalysisDao - Unexpected Error:', err);
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
      logger.error('agentAssistBotAnalysisDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const agentAssistAnalysisTrendChartDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_AgentAssist_analytics_trend_chart_info(?,?,?)',
        [
          data?.startTimeStamp ?? null,
          data?.endTimeStamp ?? null,
          data?.domainId,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('agentAssistAnalysisTrendChartDao - Unexpected Error:', err);
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
      logger.error('agentAssistAnalysisTrendChartDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const agentAssistInteractionChannelDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_analyticsAgentAssistChannel(?,?,?)',
        [
          data?.startTimeStamp ?? null,
          data?.endTimeStamp ?? null,
          data?.domainId,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('agentAssistInteractionChannelDao - Unexpected Error:', err);
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
      logger.error('agentAssistInteractionChannelDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const agentAssistEngagementRateTrendChartDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_AgentAssist_EngagementRate_trend_chart_info(?,?,?)',
        [
          data?.startTimeStamp ?? null,
          data?.endTimeStamp ?? null,
          data?.domainId,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('agentAssistEngagementRateTrendChartDao - Unexpected Error:', err);
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
      logger.error('agentAssistEngagementRateTrendChartDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const agentAssistOverriderateInfoDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_AgentAssist_OverrideRate_info(?,?,?)',
        [
          data?.startTimeStamp ?? null,
          data?.endTimeStamp ?? null,
          data?.domainId,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('agentAssistOverriderateInfoDao - Unexpected Error:', err);
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
      logger.error('agentAssistOverriderateInfoDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const knowledgbaseInteractionChannelDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_KnowledgBaseInteractionChannel(?,?,?)',
        [
          data?.startTimeStamp ?? null,
          data?.endTimeStamp ?? null,
          data?.domainId,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('knowledgbaseInteractionChannelDao - Unexpected Error:', err);
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
      logger.error('knowledgbaseInteractionChannelDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const knowledgbaseAveragebaseResponseDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_KnowledgeBase_AverageBaseResponsePerDay_info(?,?,?)',
        [
          data?.startTimeStamp ?? null,
          data?.endTimeStamp ?? null,
          data?.domainId,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('knowledgbaseAveragebaseResponseDao - Unexpected Error:', err);
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
      logger.error('knowledgbaseAveragebaseResponseDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const knowledgebaseUntilizationChartDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_KnowledgeBase_Utilization_trend_chart_info(?,?,?)',
        [
          data?.startTimeStamp ?? null,
          data?.endTimeStamp ?? null,
          data?.domainId,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('knowledgebaseUntilizationChartDao - Unexpected Error:', err);
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
      logger.error('knowledgebaseUntilizationChartDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const agentAnalyticBotNluPerformencesDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_analyticsBotNLUPerformences(?,?,?)',
        [
          data?.startTimeStamp ?? null,
          data?.endTimeStamp ?? null,
          data?.domainId,
        ],
        (err: any, result: any) => {
          if (err) {
            console.log('err:', err);
            reject(err);
            logger.error('agentAnalyticBotNluPerformencesDao - Unexpected Error:', err);
          } else {
            if (result[0]?.length > 0) {
              resolve(result[0]);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (error) {
      console.log(error);
      reject(error);
      logger.error('agentAnalyticBotNluPerformencesDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const botNluPerformencesMessageDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_BotNLU_Performences_Helpful_Message(?,?,?)',
        [
          data?.startTimeStamp ?? null,
          data?.endTimeStamp ?? null,
          data?.domainId,
        ],
        (err: any, result: any) => {
          if (err) {
            console.log('err:', err);
            reject(err);
            logger.error('botNluPerformencesMessageDao - Unexpected Error:', err);
          } else {
            if (result[0]?.length > 0) {
              resolve(result[0]);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (error) {
      console.log(error);
      reject(error);
      logger.error('botNluPerformencesMessageDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertUpdateChatIntentDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_InsertOrUpdateChatIntent(?,?,?,?,?,?)',
        [
          data.IntentId,
          data.IntentName,
          data.IntentCount,
          data.domainId,
          data.ChatFlowId,
          data.ChatHistoryId,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('insertUpdateChatIntentDao - Unexpected Error:', err);
            console.log(err);
          }
          if (result && result[0]?.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('insertUpdateChatIntentDao - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getChatIntentDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetChatIntent(?)',
        [
          data.domainId,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getChatIntentDao - Unexpected Error:', err);
            console.log(err);
          }
          if (result && result[0]?.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('getChatIntentDao - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getNluPerformencesTopIntentDistributionDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_analytics_NLUPerformences_Top_Intent_Distribution(?)',
        [
          data.domainId,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getNluPerformencesTopIntentDistributionDao - Unexpected Error:', err);
            console.log(err);
          }
          if (result && result[0]?.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('getNluPerformencesTopIntentDistributionDao - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getApiCallsDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_get_API_Calls(?)',
        [
          data?.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getApiCallsDao - Unexpected Error:', err);
            console.log(err);
          }
          if (result && result.length > 0) {
            resolve(result);
          } else {
            reject(err);
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('getApiCallsDao - Unexpected Error:', err);
    }
  });
};
