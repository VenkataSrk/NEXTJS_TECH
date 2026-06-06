import { logger } from '../../src/plugins/log';
import { mysqlPoolConnection } from '../plugins/db';

/**
 *
 * @param data
 * @returns
 */
export const getCallLiveDashboard = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_sesssion_history_dashboard_call_feautres (?,?,?,?)',
        [data.startTmpstmp, data.endTmpstmp, data.timeZone, data.domainId],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getCallLiveDashboard - Unexpected Error:', err);
          } else {
            if (result[0]?.length !== 0) {
              resolve(result[0]);
            } else {
              reject(err);
            }
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('getCallLiveDashboard - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getChatLiveDashboard = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_sesssion_history_dashboard_chat_feautres(?,?,?,?)',
        [data.startTmpstmp, data.endTmpstmp, data.timeZone, data.domainId],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getChatLiveDashboard - Unexpected Error:', err);
          } else {
            if (result[0]?.length !== 0) {
              resolve(result[0]);
            } else {
              reject(err);
            }
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('getChatLiveDashboard - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCallLiveDashboardPieChart = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_get_dashboard_call_piechart_feautre_details(?,?,?,?,?)',
        [
          data.startTmpstmp,
          data.endTmpstmp,
          data.timeZone,
          data.domainId,
          data.extNo
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getCallLiveDashboardPieChart - Unexpected Error:', err);
          } else {
            resolve(result);
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('getCallLiveDashboardPieChart - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getWebchatLiveDashborad = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_chat_dashboard (?,?,?,?)',
        [
          data.startTmpstmp,
          data.endTmpstmp,
          data.domainId,
          parseInt(data.ext, 10)
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getWebchatLiveDashborad - Unexpected Error:', err);
          } else {
            if (result && result.length > 0) {
              resolve(result);
            } else {
              reject(err);
            }
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('getWebchatLiveDashborad - Unexpected Error:', err);
    }
  });
};

export const roleInfoDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_role_info_dao (?,?)',
        [
          data.domainId,
          data.ext
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('roleInfoDao - Unexpected Error:', err);
          } else {
            if (result[0]?.length !== 0) {
              resolve(result[0][0]);
            } else {
              reject(result[0]);
            }
          }
        }
      );
    } catch (error) {
      reject(error);
      logger.error('roleInfoDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getLiveDashBoardFeautres = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_sesssion_history_dashboard_feautres (?,?,?,?,?)',
        [data.startTmpstmp, data.endTmpstmp, data.timeZone, data.domainId, data.extNo],
        (err: any, result: any) => {
          if (err) {
            resolve([]);
            logger.error('getLiveDashBoardFeautres - Unexpected Error:', err);
            // reject(err);
          } else {
            if (result[0]?.length !== 0) {
              resolve(result[0]);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (err) {
      // reject(err);
      resolve([]);
      logger.error('getLiveDashBoardFeautres - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getArrivalDashbordRate = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_call_arrival_dashboard_rate_details (?,?,?,?,?)',
        [data.startTmpstmp, data.endTmpstmp, data.timeZone, data.domainId, data.extNo],
        (err: any, result: any) => {
          if (err) {
            resolve([]);
            logger.error('getArrivalDashbordRate - Unexpected Error:', err);
            // reject(err);
          } else {
            if (result[0]?.length !== 0) {
              resolve(result);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (err) {
      // reject(err);
      resolve([]);
      logger.error('getArrivalDashbordRate - Unexpected Error:', err);
    }
  });
};

export const getChatBotLiveDashboardDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetChatInteractionSessions (?,?,?)',
        [data.domainId, data.startTmpstmp, data.endTmpstmp],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getChatBotLiveDashboardDao - Unexpected Error:', err);
          } else {
            resolve(result);
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('getChatBotLiveDashboardDao - Unexpected Error:', err);
    }
  });
};

export const getChatBotLiveAgentPerformanceDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetChatAgentPerformance(?,?,?)',
        [data.domainId, data.startTmpstmp, data.endTmpstmp],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getChatBotLiveAgentPerformanceDao - Unexpected Error:', err);
          } else {
            resolve(result);
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('getChatBotLiveAgentPerformanceDao - Unexpected Error:', err);
    }
  });
};

export const getTrendChartForcustomerDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetChatDuration(?,?,?)',
        [data.domainId, data.startTmpstmp, data.endTmpstmp],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getTrendChartForcustomerDao - Unexpected Error:', err);
          } else {
            resolve(result);
          }
        }
      );
    } catch (error) {
      console.log(error);
      logger.error('getTrendChartForcustomerDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getSmsLiveDashboardPieChart = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_dashboard_SMS_piechart_feautre_details(?,?,?,?,?)',
        [data.startTmpstmp, data.endTmpstmp, data.domainId, data.timeZone, data.extNo],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getSmsLiveDashboardPieChart - Unexpected Error:', err);
          } else {
            resolve(result);
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('getSmsLiveDashboardPieChart - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getEmailLiveDashboardPieChart = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_dashboard_Email_piechart_feautre_details(?,?,?,?,?)',
        [data.startTmpstmp, data.endTmpstmp, data.domainId, data.timeZone, data.extNo],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getEmailLiveDashboardPieChart - Unexpected Error:', err);
          } else {
            resolve(result);
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('getSmsLiveDashboardPieChart - Unexpected Error:', err);
    }
  });
};
export const getDashboardPiechartAgentStatusDetailsDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_dashboard_piechart_agent_status_details(?,?,?,?,?)',
        [data.startTmpstmp, data.endTmpstmp, data.timeZone, data.domainId, data.extNo],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getDashboardPiechartAgentStatusDetailsDao - Unexpected Error:', err);
          } else {
            if (result[0]?.length !== 0) {
              resolve(result[0]);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (err) {
      console.log(err);
      reject(err);
      logger.error('getDashboardPiechartAgentStatusDetailsDao - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getDashboardAgentStatusHandlerDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_dashboardAgentStatus(?,?)',
        [data.domainId, data.agentId],
        (err: any, result: any) => {
          if (err) {
            console.log('errr', err);
            reject(err);
            logger.error('getDashboardAgentStatusHandlerDao - Unexpected Error:', err);
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
      logger.error('getDashboardAgentStatusHandlerDao - Unexpected Error:', error);
    }
  });
};

export const getDashboardSocialMediaStatusHandlerDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_dashboard_SocialMedia_piechart_feautre_details(?,?,?,?,?,?)',
        [
          data?.startTmpstmp,
          data?.endTmpstmp,
          data?.domainId,
          data?.timezone,
          data?.ext,
          data?.channelType,
        ],
        (err: any, result: any) => {

          if (err) {
            console.log('getDashboardSocialMediaStatusHandlerDao only err: ', err);
            reject(err);
            logger.error('getDashboardSocialMediaStatusHandlerDao - Unexpected Error:', err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log('getDashboardSocialMediaStatusHandlerDao only catch error: ', error);
      reject(error);
      logger.error('getDashboardSocialMediaStatusHandlerDao - Unexpected Error:', error);
    }
  });
};

export const getDashboardAverageHandlerDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_DashboardAverage(?,?,?,?,?,?)',
        [
          data?.startTmpstmp,
          data?.endTmpstmp,
          data?.channelType,
          data?.domainId,
          data?.timezone,
          data?.agentId,
        ],
        (err: any, result: any) => {
          if (err) {
            console.log('getDashboardAverageHandlerDao only err: ', err);
            reject(err);
            logger.error('getDashboardAverageHandlerDao - Unexpected Error:', err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log('getDashboardAverageHandlerDao only catch error: ', error);
      reject(error);
      logger.error('getDashboardAverageHandlerDao - Unexpected Error:', error);
    }
  });
};

export const getCustomerSatisfactionHandlerDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_DashboardCustomerSatisfaction(?,?,?,?)',
        [
          data?.startTmpstmp,
          data?.endTmpstmp,
          data?.timezone,
          data?.domainId,

        ],
        (err: any, result: any) => {

          if (err) {
            console.log('getCustomerSatisfactionHandlerDao only err: ', err);
            reject(err);
            logger.error('getCustomerSatisfactionHandlerDao - Unexpected Error:', err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log('getCustomerSatisfactionHandlerDao only catch error: ', error);
      reject(error);
      logger.error('getCustomerSatisfactionHandlerDao - Unexpected Error:', error);
    }
  });
};

export const getDashboardUtilizationHandlerDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_DashboardUtilization(?,?,?,?,?)',
        [
          data?.startTmpstmp,
          data?.endTmpstmp,
          data?.domainId,
          data?.ext,
          data?.channeltype,
        ],
        (err: any, result: any) => {

          if (err) {
            console.log('getDashboardUtilizationHandlerDao only err: ', err);
            reject(err);
            logger.error('getDashboardUtilizationHandlerDao - Unexpected Error:', err);
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

export const getDashboardPerformanceComparisonHandlerDao = (data: any) => {
  return new Promise((resolve, reject) => {
    console.log('DashboardAgentBotComparePerform::::', data);
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
            logger.error('getDashboardPerformanceComparisonHandlerDao - Unexpected Error:', err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log('getDashboardUtilizationHandlerDao only catch error: ', error);
      reject(error);
      logger.error('getDashboardPerformanceComparisonHandlerDao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getUsedNoOfSessionDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call  up_get_UsedNoOfSession_v2(?,?)',
        [
          data?.domainId,
          data?.channelType ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            console.log('getUsedNoOfSessionDao only err: ', err);
            reject(err);
            logger.error('getUsedNoOfSessionDao - Unexpected Error:', err);
          }
          if (result?.length > 0) {
            resolve(result);
          } else {
            resolve([]);
          }
        }
      );
    } catch (error) {
      console.log('getUsedNoOfSessionDao only catch error: ', error);
      reject(error);
      logger.error('getUsedNoOfSessionDao - Unexpected Error:', error);
    }
  });
};

export const getDashboardResponseRating = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_live_dashboard_response_rating(?,?,?,?)',
        [
          data.p_startTmpstmp ?? null,
          data.p_endTmpstmp ?? null,
          data.p_domainId,
          data?.channelType
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getDashboardResponseRating - Unexpected Error:', err);
          }
          if (result?.length > 0) {
            resolve(result);
          } else {
            resolve([]);
          }
        });
    } catch (err) {
      resolve([]);
      logger.error('getDashboardResponseRating - Unexpected Error:', err);
    }
  });
};

export const getAgntAssiatanceDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_live_dashboard_agent_assistance(?,?,?,?,?)',
        [
          data.p_startTmpstmp ?? null,
          data.p_endTmpstmp ?? null,
          data.p_domainId,
          data.agentAssistChannelType ?? null,
          data.agentId
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getAgntAssiatanceDao - Unexpected Error:', err);
          }
          resolve(result);
        });
    } catch (err) {
      resolve([]);
      logger.error('getAgntAssiatanceDao - Unexpected Error:', err);
    }
  });
};

export const getSurveyResponseDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_live_dashboard_survey_response(?,?,?)',
        [
          data.p_startTmpstmp ?? null,
          data.p_endTmpstmp ?? null,
          data.p_domainId,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getSurveyResponseDao - Unexpected Error:', err);
          }
          if (result?.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        });
    } catch (err) {
      reject(err);
      logger.error('getSurveyResponseDao - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getSentimentalAnalysisDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_live_dashboard_customer_satisfaction(?,?,?,?)',
        [
          data?.startTmpstmp ?? null,
          data?.endTmpstmp ?? null,
          data?.domainId,
          data?.SentimentalAnalysischannelType ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getSentimentalAnalysisDao - Unexpected Error:', err);
          }
          if (result?.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        });
    } catch (err) {
      reject(err);
      logger.error('getSentimentalAnalysisDao - Unexpected Error:', err);
    }
  });
};

export const getComapreDashboardResponse = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_compare_dashboard(?,?,?,?)',
        [
          data.domainId,
          data.startTmpstmp ?? null,
          data.endTmpstmp ?? null,
          data.productId ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getComapreDashboardResponse - Unexpected Error:', err);
          }
          resolve(result);
        });
    } catch (err) {
      resolve([]);
      logger.error('getComapreDashboardResponse - Unexpected Error:', err);
    }
  });
};

export const getCompareDashbaordTrendChartResponse = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_compare_dashboard_trendChat(?,?,?,?,?,?,?)',
        [
          data.domainId,
          data.startTmpstmp ?? null,
          data.endTmpstmp ?? null,
          data.datetype ?? null,
          data.columnName ?? null,
          data.columnNameCompare ?? null,
          data.productId ?? null,

        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getCompareDashbaordTrendChartResponse - Unexpected Error:', err);
          }
          resolve(result);
        });
    } catch (err) {
      resolve([]);
      logger.error('getCompareDashbaordTrendChartResponse - Unexpected Error:', err);
    }
  });
};

export const getCustomerCountForYearDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_customer_count_for_year(?)',
        [
          data.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getCustomerCountForYearDao - Unexpected Error:', err);
          }
          resolve(result);
        });
    } catch (err) {
      resolve([]);
    }
  });
};

export const getVideoChannelDashMetricsDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_dashboard_video_call_details(?,?,?,?,?)',
        [
          data?.startTmpstmp,
          data?.endTmpstmp,
          data?.domainId,
          data?.timezone,
          data?.ext,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          }
          resolve(result);
        });
    } catch (err) {
      resolve([]);
    }
  });
};

export const getTrackerDataDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_live_interaction_dashboardCount(?)',
        [
          data?.domainId,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          }
          resolve(result);
        });
    } catch (err) {
      resolve([]);
    }
  });
};

export const getOverAllSlaAiDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_overall_sla_ai(?,?,?)',
        [
          data?.domainId,
          data?.startTmpstmp,
          data?.endTmpstmp,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          }
          resolve(result);
        });
    } catch (err) {
      resolve([]);
    }
  });
};
