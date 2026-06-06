import { logger } from '../../src/plugins/log';
import { mysqlPoolConnection } from '../plugins/db';

/**
 * get agent call summary report
 * @param data
 * @returns
 */
export const getStandAloneChatReportDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call GetAgentBasedChatReport(?,?,?,?,?,?,?,?,?)', [
        data.startTmpstmp,
        data.endTmpstmp,
        data.timeZone,
        data.domainId,
        data.agentName,
        data.queueName,
        data.disposition,
        data.pageLimit,
        data.pageOffset
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('getStandAloneChatReportDao - Unexpected Error:', err);
          console.log(err);
        }
        resolve(result[0]);
      });
    } catch (err) {
      reject(err);
      logger.error('getStandAloneChatReportDao - Unexpected Error:', err);
    }
  });
};

export const getStandAloneChatReportDaoForSchedule = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call GetAgentBasedChatReport(?,?,?,?,?,?,?,?,?)', [
        data.startTmpstmp,
        data.endTmpstmp,
        data.timeZone,
        data.domainId,
        data.agentName,
        data.queueName,
        data.disposition,
        data.pageLimit,
        data.pageOffset
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('getStandAloneChatReportDaoForSchedule - Unexpected Error:', err);
          console.log(err);
        }
        resolve(result[0]);
      });
    } catch (err) {
      reject(err);
      logger.error('getStandAloneChatReportDaoForSchedule - Unexpected Error:', err);
    }
  });
};

export const getStandAloneBotChatReportDao = (data:any) => {
  return new Promise<any>(async(resolve, reject) => {
    try {
      mysqlPoolConnection.query('call GetAgentChatReport(?,?,?,?,?,?,?,?)', [
        data.startTmpstmp,
        data.endTmpstmp,
        data.timeZone,
        data.domainId,
        data.pageLimit,
        data.pageOffset,
        data.queueName,
        data.agentName
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('getStandAloneBotChatReportDao - Unexpected Error:', err);
          console.log(err);
        }
        resolve(result[0]);
      });

    } catch (error) {
      console.log(error);
      logger.error('getStandAloneBotChatReportDao - Unexpected Error:', error);

    }
  });
};

export const getStandAloneBotChatReportDaoForSchedule = (data:any) => {
  return new Promise<any>(async(resolve, reject) => {
    try {
      mysqlPoolConnection.query('call GetAgentChatReport(?,?,?,?,?,?,?,?)', [
        data.startTmpstmp,
        data.endTmpstmp,
        data.timeZone,
        data.domainId,
        data.pageLimit,
        data.pageOffset,
        data.queueName,
        data.agentName
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('getStandAloneBotChatReportDaoForSchedule - Unexpected Error:', err);
          console.log(err);
        }
        resolve(result[0]);
      });

    } catch (error) {
      console.log(error);
      logger.error('getStandAloneBotChatReportDaoForSchedule - Unexpected Error:', error);

    }
  });
};

export const getStandAloneACWReportDao = (data:any) => {
  return new Promise<any>(async(resolve, reject) => {
    try {
      mysqlPoolConnection.query('call GetAgentACWReport(?,?,?,?,?,?,?,?)', [
        data.startTmpstmp,
        data.endTmpstmp,
        data.domainId,
        data.timeZone,
        data.queueName,
        data.agentName,
        data.pageLimit,
        data.pageOffset,
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('getStandAloneACWReportDao - Unexpected Error:', err);
          console.log(err);
        }
        resolve(result[0]);
      });

    } catch (error) {
      console.log(error);
      logger.error('getStandAloneACWReportDao - Unexpected Error:', error);
    }
  });
};
