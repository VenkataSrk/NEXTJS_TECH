import { mysqlPoolConnection } from '../../src/plugins/db';
import { logger } from '../../src/plugins/log';

/**
 *
 * @param data
 * @returns
 */
export const getAiCallQualityReportDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_AI_Call_Quality_report(?,?,?,?,?,?,?,?,?)',
        [
          data.startTmpstmp,
          data.endTmpstmp,
          data.timeZone,
          data.domainId,
          data.queueId,
          data.agentId,
          data.direction,
          data.limit,
          data.offset,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getAiCallQualityReportDao - Unexpected Error:', err);
          } else {
            if (result?.length !== 0) {
              resolve(result);
            } else {
              resolve([]);
            }
          }
        }
        );
    } catch (err) {
      reject(err);
      logger.error('getAiCallQualityReportDao - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getAiCallQualityReportDaoForSchedule = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_AI_Call_Quality_report(?,?,?,?,?,?,?,?,?)',
        [
          data.startTmpstmp,
          data.endTmpstmp,
          data.timeZone,
          data.domainId,
          data.queueId,
          data.agentId,
          data.direction,
          data.limit,
          data.offset,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getAiCallQualityReportDaoForSchedule - Unexpected Error:', err);
          } else {
            if (result?.length !== 0) {
              resolve(result?.[3]);
            } else {
              resolve([]);
            }
          }
        }
        );
    } catch (err) {
      reject(err);
      logger.error('getAiCallQualityReportDaoForSchedule - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getAiChatQualityReportDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_AI_Chat_Quality_report(?,?,?,?,?,?,?,?,?)',
        [
          data.startTmpstmp,
          data.endTmpstmp,
          data.timeZone,
          data.domainId,
          data.queueId,
          data.agentId,
          data.direction,
          data.limit,
          data.offset,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getAiChatQualityReportDao - Unexpected Error:', err);
          } else {
            if (result?.length !== 0) {
              resolve(result);
            } else {
              resolve([]);
            }
          }
        }
        );
    } catch (err) {
      reject(err);
      logger.error('getAiChatQualityReportDao - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getAiChatQualityReportDaoForSchedule = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_AI_Chat_Quality_report(?,?,?,?,?,?,?,?,?)',
        [
          data.startTmpstmp,
          data.endTmpstmp,
          data.timeZone,
          data.domainId,
          data.queueId,
          data.agentId,
          data.direction,
          data.limit,
          data.offset,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getAiChatQualityReportDaoForSchedule - Unexpected Error:', err);
          } else {
            if (result?.length !== 0) {
              resolve(result?.[3]);
            } else {
              resolve([]);
            }
          }
        }
        );
    } catch (err) {
      reject(err);
      logger.error('getAiChatQualityReportDaoForSchedule - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getAiBotPerformancesReportDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_AI_Bot_Performances_report(?,?,?,?,?,?,?)',
        [
          data.startTmpstmp,
          data.endTmpstmp,
          data.timeZone,
          data.domainId,
          data.agentId,
          data.limit,
          data.offset,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getAiBotPerformancesReportDao - Unexpected Error:', err);
          } else {
            if (result?.length !== 0) {
              resolve(result);
            } else {
              resolve([]);
            }
          }
        });
    } catch (err) {
      reject(err);
      logger.error('getAiBotPerformancesReportDao - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getAiBotPerformancesReportDaoForSchedule = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_AI_Bot_Performances_report(?,?,?,?,?,?,?)',
        [
          data.startTmpstmp,
          data.endTmpstmp,
          data.timeZone,
          data.domainId,
          data.agentId,
          data.limit,
          data.offset,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getAiBotPerformancesReportDaoForSchedule - Unexpected Error:', err);
          } else {
            if (result?.length !== 0) {
              resolve(result?.[2]);
            } else {
              resolve([]);
            }
          }
        });
    } catch (err) {
      reject(err);
      logger.error('getAiBotPerformancesReportDaoForSchedule - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getAiBotPerformancesBotAgentReportDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_AI_Bot_Performances_Between_Bot_Agent_Report(?,?,?,?,?,?,?,?)',
        [
          data.startTmpstmp,
          data.endTmpstmp,
          data.timeZone,
          data.domainId,
          data.agentId,
          data.queueId,
          data.limit,
          data.offset,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getAiBotPerformancesBotAgentReportDao - Unexpected Error:', err);
          } else {
            if (result?.length !== 0) {
              resolve(result);
            } else {
              resolve([]);
            }
          }
        });
    } catch (err) {
      reject(err);
      logger.error('getAiBotPerformancesBotAgentReportDao - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getAiBotPerformancesBotAgentReportDaoForSchedule = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_AI_Bot_Performances_Between_Bot_Agent_Report(?,?,?,?,?,?,?,?)',
        [
          data.startTmpstmp,
          data.endTmpstmp,
          data.timeZone,
          data.domainId,
          data.agentId,
          data.queueId,
          data.limit,
          data.offset,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getAiBotPerformancesBotAgentReportDaoForSchedule - Unexpected Error:', err);
          } else {
            if (result?.length !== 0) {
              resolve(result?.[3]);
            } else {
              resolve([]);
            }
          }
        });
    } catch (err) {
      reject(err);
      logger.error('getAiBotPerformancesBotAgentReportDaoForSchedule - Unexpected Error:', err);
    }
  });
};
