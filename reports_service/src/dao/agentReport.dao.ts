import { logger } from '../../src/plugins/log';
import { mysqlPoolConnection } from '../plugins/db';

/**
 * get agent call summary report
 * @param data
 * @returns
 */
export const getAgentCallSummaryReport = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_getAgentCallSummaryReport(?,?,?,?,?,?,?,?)', [
        data?.startTmpstmp,
        data?.endTmpstmp,
        data?.domainId,
        data?.agentName,
        data?.routingId,
        data?.customerId,
        data?.limit,
        data?.offset,
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('getAgentCallSummaryReport - Unexpected Error:', err);
          console.log('getAgentCallSummaryReport error: ', err);
        }
        resolve(result);
      });
    } catch (err) {
      console.log('getAgentCallSummaryReport catch error: ', err);
      reject(err);
      logger.error('getAgentCallSummaryReport - Unexpected Error:', err);
    }
  });
};

/**
 * get agent call summary report
 * @param data
 * @returns
 */
export const getAgentCallSummaryReportForSchedule = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_getAgentCallSummaryReport(?,?,?,?,?,?,?,?)', [
        data?.startTmpstmp,
        data?.endTmpstmp,
        data?.domainId,
        data?.agentName,
        data?.routingId,
        data?.customerId,
        data?.limit,
        data?.offset,
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('getAgentCallSummaryReportForSchedule - Unexpected Error:', err);
          console.log('getAgentCallSummaryReport error: ', err);
        }
        resolve(result?.length > 0 ? result[2] : []);
      });
    } catch (err) {
      console.log('getAgentCallSummaryReport catch error: ', err);
      reject(err);
      logger.error('getAgentCallSummaryReportForSchedule - Unexpected Error:', err);
    }
  });
};

/**
 * get agent login and logout report
 * @param data
 * @returns
 */
export const getAgentLoginAndLogoutReportDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_getAgentLoginLogoutReport(?,?,?,?,?,?,?)', [
        data.startTmpstmp,
        data.endTmpstmp,
        data.timeZone,
        data.domainId,
        data.agentName,
        data.limit,
        data.offset
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('getAgentLoginAndLogoutReportDao - Unexpected Error:', err);
          console.log(err);
        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
      logger.error('getAgentLoginAndLogoutReportDao - Unexpected Error:', err);
    }
  });
};

export const getAgentLoginAndLogoutReportDaoForSchedule = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_getAgentLoginLogoutReport(?,?,?,?,?,?,?)', [
        data.startTmpstmp,
        data.endTmpstmp,
        data.timeZone,
        data.domainId,
        data.agentName,
        data.limit,
        data.offset
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('getAgentLoginAndLogoutReportDaoForSchedule - Unexpected Error:', err);
          console.log(err);
        }
        resolve(result?.length > 0 ? result?.[2] : []);
      });
    } catch (err) {
      reject(err);
      logger.error('getAgentLoginAndLogoutReportDaoForSchedule - Unexpected Error:', err);
    }
  });
};

export const getAgentPerformanceHandlerDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_getAgentPerformanceReport(?,?,?,?,?,?,?)', [
        data.startTmpstmp,
        data.endTmpstmp,
        data.timeZone,
        data.domainId,
        data.agentName,
        data.limit,
        data.offset,
      ],                        (err: any, result: any) => {
        if (err) {
          console.log('getAgentPerformanceHandlerDao reject err: ', err);
          reject(err);
          logger.error('getAgentPerformanceHandlerDao - Unexpected Error:', err);
        }
        resolve(result);
      });
    } catch (error) {
      console.log('getAgentPerformanceHandlerDao catch error: ', error);
      reject(error);
      logger.error('getAgentPerformanceHandlerDao - Unexpected Error:', error);
    }
  });
};

export const getAgentPerformanceHandlerDaoForSchedule = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_getAgentPerformanceReport(?,?,?,?,?,?,?)', [
        data.startTmpstmp,
        data.endTmpstmp,
        data.timeZone,
        data.domainId,
        data.agentName,
        data.limit,
        data.offset,
      ],                        (err: any, result: any) => {
        if (err) {
          console.log('getAgentPerformanceHandlerDao reject err: ', err);
          reject(err);
          logger.error('getAgentPerformanceHandlerDaoForSchedule - Unexpected Error:', err);
        }
        resolve(result?.length > 0 ? result?.[2] : []);
      });
    } catch (error) {
      console.log('getAgentPerformanceHandlerDao catch error: ', error);
      reject(error);
      logger.error('getAgentPerformanceHandlerDaoForSchedule - Unexpected Error:', error);
    }
  });
};

export const getQueryHandlingTimeReportDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_getQueryHandlingTimeReport(?,?,?,?,?,?,?)', [
        data.startTmpstmp,
        data.endTmpstmp,
        data.timeZone,
        data.domainId,
        data.agentName,
        data.limit,
        data.offset,
      ],                        (err: any, result: any) => {
        if (err) {
          console.log('getQueryHandlingTimeReportDao reject err: ', err);
          reject(err);
          logger.error('getQueryHandlingTimeReportDao - Unexpected Error:', err);
        }
        resolve(result);
      });
    } catch (error) {
      console.log('getQueryHandlingTimeReportDao catch error: ', error);
      reject(error);
      logger.error('getQueryHandlingTimeReportDao - Unexpected Error:', error);
    }
  });
};

export const getQueryHandlingTimeReportDaoForSchedule = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_getQueryHandlingTimeReport(?,?,?,?,?,?,?)', [
        data.startTmpstmp,
        data.endTmpstmp,
        data.timeZone,
        data.domainId,
        data.agentName,
        data.limit,
        data.offset,
      ],                        (err: any, result: any) => {
        if (err) {
          console.log('getQueryHandlingTimeReportDao reject err: ', err);
          reject(err);
          logger.error('getQueryHandlingTimeReportDaoForSchedule - Unexpected Error:', err);
        }
        resolve(result?.length > 0 ? result?.[2] : []);
      });
    } catch (error) {
      console.log('getQueryHandlingTimeReportDao catch error: ', error);
      reject(error);
      logger.error('getQueryHandlingTimeReportDaoForSchedule - Unexpected Error:', error);
    }
  });
};

export const getAgentFeedbackReportDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_getAgentFeedbackReport(?,?,?,?,?,?,?)', [
        data.startTmpstmp,
        data.endTmpstmp,
        data.timeZone,
        data.domainId,
        data.agentName,
        data.limit,
        data.offset,
      ],                        (err: any, result: any) => {
        if (err) {
          console.log('getAgentFeedbackReportDao reject err: ', err);
          reject(err);
          logger.error('getAgentFeedbackReportDao - Unexpected Error:', err);
        }
        resolve(result);
      });
    } catch (error) {
      console.log('getAgentFeedbackReportDao catch error: ', error);
      reject(error);
      logger.error('getAgentFeedbackReportDao - Unexpected Error:', error);
    }
  });
};

export const getAgentStatusReportReportDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_getAgentStatusReport(?,?,?,?,?,?,?)', [
        data.startTmpstmp,
        data.endTmpstmp,
        data.timeZone,
        data.domainId,
        data.agentName,
        data.limit,
        data.offset,
      ],                        (err: any, result: any) => {
        if (err) {
          console.log('getAgentStatusReportDao reject err: ', err);
          reject(err);
          logger.error('getAgentStatusReportReportDao - Unexpected Error:', err);
        }
        resolve(result);
      });
    } catch (error) {
      console.log('getAgentStatusReportDao catch error: ', error);
      reject(error);
      logger.error('getAgentStatusReportReportDao - Unexpected Error:', error);
    }
  });
};

export const getCsatAgentFeedbackReportDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_getcsatAgentFeedbackReport(?,?,?,?,?,?,?)',
        [
          data?.startTmpstmp,
          data?.endTmpstmp,
          data?.timeZone,
          data?.domainId,
          data?.agentId,
          data?.Limit,
          data?.Offset,
          // data?.customerFeedbackId
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getCsatAgentFeedbackReportDao - Unexpected Error:', err);
            console.log(err);
          }
          if (result && result.length > 0) {
            return resolve(result);
          }
          return resolve([]);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('getCsatAgentFeedbackReportDao - Unexpected Error:', err);
    }
  });
};

export const getAgentStatusReportReportDaoForSchedule = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_getAgentStatusReport(?,?,?,?,?,?,?)', [
        data.startTmpstmp,
        data.endTmpstmp,
        data.timeZone,
        data.domainId,
        data.agentName,
        data.limit,
        data.offset,
      ],                        (err: any, result: any) => {
        if (err) {
          console.log('getAgentStatusReportDao reject err: ', err);
          reject(err);
          logger.error('getAgentStatusReportReportDaoForSchedule - Unexpected Error:', err);
        }
        resolve(result?.length > 0 ? result[2] : []);
      });
    } catch (error) {
      console.log('getAgentStatusReportDao catch error: ', error);
      reject(error);
      logger.error('getAgentStatusReportReportDaoForSchedule - Unexpected Error:', error);
    }
  });
};

export const getParticularAgentReportDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_getparticularAgentStatusReport(?,?,?,?,?,?,?)',
        [
          data?.startTmpstmp,
          data?.endTmpstmp,
          data?.domainId,
          data?.agentId,
          data?.statusName,
          data?.Limit,
          data?.Offset,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getParticularAgentReportDao - Unexpected Error:', err);
            console.log(err);
          }
          if (result && result.length > 0) {
            return resolve(result);
          }
          return resolve([]);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('getParticularAgentReportDao - Unexpected Error:', err);
    }
  });
};
