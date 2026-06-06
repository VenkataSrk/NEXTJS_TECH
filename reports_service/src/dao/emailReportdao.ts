import { logger } from '../../src/plugins/log';
import { mysqlPoolConnection } from '../plugins/db';

export const getEmailReportHandlerDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_email_profile_report(?,?,?,?,?,?,?,?,?)',
        [
          data?.startTmpstmp,
          data?.endTmpstmp,
          data?.timeZone,
          data?.domainId,
          data?.agentName,
          data?.agentName,
          data?.disposition,
          data?.offset,
          data?.limit,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getEmailReportHandlerDao - Unexpected Error:', err);
            console.log('getEmailReportHandlerDao error: ', err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log('getEmailReportHandlerDao catch error: ', error);
      reject(error);
      logger.error('getEmailReportHandlerDao - Unexpected Error:', error);
    }
  });
};

export const getEmailReportHandlerDaoForSchedule = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_email_profile_report(?,?,?,?,?,?,?,?,?)',
        [
          data?.startTmpstmp,
          data?.endTmpstmp,
          data?.timeZone,
          data?.domainId,
          data?.agentName,
          data?.agentName,
          data?.disposition,
          data?.offset,
          data?.limit,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getEmailReportHandlerDaoForSchedule - Unexpected Error:', err);
            console.log('getEmailReportHandlerDao error: ', err);
          }
          resolve(result?.length > 0 ? result[3] : []);
        }
      );
    } catch (error) {
      console.log('getEmailReportHandlerDao catch error: ', error);
      reject(error);
      logger.error('getEmailReportHandlerDaoForSchedule - Unexpected Error:', error);
    }
  });
};

export const getEmailAgentReportHandlerDaO = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_email_agent_report(?,?,?,?,?,?,?,?)',
        [
          data?.startTmpstmp,
          data?.endTmpstmp,
          data?.timeZone,
          data?.domainId,
          data?.queueName,
          data?.agentName,
          data?.offset,
          data?.limit,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getEmailAgentReportHandlerDaO - Unexpected Error:', err);
            console.log('getEmailAgentReportHandlerDaO error: ', err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log('getEmailAgentReportHandlerDaO catch error: ', error);
      reject(error);
      logger.error('getEmailAgentReportHandlerDaO - Unexpected Error:', error);
    }
  });
};

export const getEmailAgentReportHandlerDaOForSchedule = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_email_agent_report(?,?,?,?,?,?,?,?)',
        [
          data?.startTmpstmp,
          data?.endTmpstmp,
          data?.timeZone,
          data?.domainId,
          data?.queueName,
          data?.agentName,
          data?.offset,
          data?.limit,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getEmailAgentReportHandlerDaOForSchedule - Unexpected Error:', err);
            console.log('getEmailAgentReportHandlerDaO error: ', err);
          }
          resolve(result?.length > 0 ? result[3] : []);
        }
      );
    } catch (error) {
      console.log('getEmailAgentReportHandlerDaO catch error: ', error);
      reject(error);
      logger.error('getEmailAgentReportHandlerDaOForSchedule - Unexpected Error:', error);
    }
  });
};

export const getEmailInterationReportDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_email_interaction_report(?,?,?,?,?,?,?,?,?,?)',
        [
          data?.startTmpstmp ?? null,
          data?.endTmpstmp ?? null,
          data?.timeZone ?? null,
          data?.domainId ?? null,
          data?.queueName ?? null,
          data?.agentName ?? null,
          data?.direction ?? null,
          data?.disposition ?? null,
          data?.offset ?? null,
          data?.limit ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getEmailInterationReportDao - Unexpected Error:', err);
            console.log('getEmailInterationReportDao error: ', err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log('getSmsInterationReportDao catch error: ', error);
      logger.error('getEmailInterationReportDao - Unexpected Error:', error);
    }
  });
};

export const getEmailInterationReportDaoForSchedule = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_email_interaction_report(?,?,?,?,?,?,?,?,?,?)',
        [
          data?.startTmpstmp ?? null,
          data?.endTmpstmp ?? null,
          data?.timeZone ?? null,
          data?.domainId ?? null,
          data?.queueName ?? null,
          data?.agentName ?? null,
          data?.direction ?? null,
          data?.dispositionIds ?? null,
          data?.offset ?? null,
          data?.limit ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getEmailInterationReportDaoForSchedule - Unexpected Error:', err);
            console.log('getEmailInterationReportDao error: ', err);
          }
          resolve(result?.length > 0 ? result[4] : []);
        }
      );
    } catch (error) {
      console.log('getSmsInterationReportDao catch error: ', error);
      logger.error('getEmailInterationReportDaoForSchedule - Unexpected Error:', error);
    }
  });
};
