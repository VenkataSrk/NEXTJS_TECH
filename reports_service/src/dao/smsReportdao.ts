import { logger } from '../../src/plugins/log';
import { mysqlPoolConnection } from '../plugins/db';

export const getSmsProfileReportHandlerDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_sms_profile_report(?,?,?,?,?,?,?,?,?)',
        [
          data?.startTmpstmp,
          data?.endTmpstmp,
          data?.timeZone,
          data?.domainId,
          data?.queueName,
          data?.agentName,
          data?.disposition,
          data?.offset,
          data?.limit,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getSmsProfileReportHandlerDao - Unexpected Error:', err);
            console.log('getSmsEditReportColumn error: ', err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log('getSmsEditReportColumn catch error: ', error);
      reject(error);
      logger.error('getSmsProfileReportHandlerDao - Unexpected Error:', error);
    }
  });
};

export const getSmsProfileReportHandlerDaoForSchedule = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_sms_profile_report(?,?,?,?,?,?,?,?,?)',
        [
          data?.startTmpstmp,
          data?.endTmpstmp,
          data?.timeZone,
          data?.domainId,
          data?.queueName,
          data?.agentName,
          data?.disposition,
          data?.offset,
          data?.limit,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getSmsProfileReportHandlerDaoForSchedule - Unexpected Error:', err);
            console.log('getSmsEditReportColumn error: ', err);
          }
          resolve(result?.length > 0 ? result?.[3] : []);
        }
      );
    } catch (error) {
      console.log('getSmsEditReportColumn catch error: ', error);
      reject(error);
      logger.error('getSmsProfileReportHandlerDaoForSchedule - Unexpected Error:', error);
    }
  });
};

export const getSmsAgentReportHandlerDaO = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_sms_agent_report(?,?,?,?,?,?,?,?)',
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
            logger.error('getSmsAgentReportHandlerDaO - Unexpected Error:', err);
            console.log('getSmsAgentReportHandlerDaO error: ', err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log('getSmsAgentReportHandlerDaO catch error: ', error);
      reject(error);
      logger.error('getSmsProfileReportHandlerDaoForSchedule - Unexpected Error:', error);
    }
  });
};

export const getSmsAgentReportHandlerDaOForShedule = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_sms_agent_report(?,?,?,?,?,?,?,?)',
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
            logger.error('getSmsAgentReportHandlerDaOForShedule - Unexpected Error:', err);
            console.log('getSmsAgentReportHandlerDaO error: ', err);
          }
          resolve(result?.length > 0 ? result?.[2] : []);
        }
      );
    } catch (error) {
      console.log('getSmsAgentReportHandlerDaO catch error: ', error);
      reject(error);
      logger.error('getSmsAgentReportHandlerDaOForShedule - Unexpected Error:', error);
    }
  });
};

export const getSmsInterationReportDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_sms_interaction_report(?,?,?,?,?,?,?,?,?,?)',
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
            logger.error('getSmsInterationReportDao - Unexpected Error:', err);
            console.log('getSmsInterationReportDao error: ', err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log('getSmsInterationReportDao catch error: ', error);
      logger.error('getSmsInterationReportDao - Unexpected Error:', error);
    }
  });
};

export const getSmsInterationReportDaoForSchedule = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_sms_interaction_report(?,?,?,?,?,?,?,?,?,?)',
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
            logger.error('getSmsInterationReportDaoForSchedule - Unexpected Error:', err);
            console.log('getSmsInterationReportDao error: ', err);
          }
          resolve(result?.length > 0 ? result[4] : []);
        }
      );
    } catch (error) {
      console.log('getSmsInterationReportDao catch error: ', error);
      logger.error('getSmsAgentReportHandlerDaOForShedule - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getEmailConfigDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_emailConfig(?)',
        [
          data
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
            logger.error('getEmailConfigDao - Unexpected Error:', err);
          }
          resolve(result[0]);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('getEmailConfigDao - Unexpected Error:', error);
    }
  });
};
