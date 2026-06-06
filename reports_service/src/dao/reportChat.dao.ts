import { rejects } from 'assert';
import { logger } from '../../src/plugins/log';
import { mysqlPoolConnection } from '../plugins/db';

/**
 * @description ccass get session history chat details
 * @param data
 * @returns
 */
export const getchatSessionReport = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccass_get_session_history_chat_details(?,?,?,?,?,?,?,?,?)', [
        data.startTmpstmp,
        data.endTmpstmp,
        data.timeZone,
        data.domainId,
        data.p_limit,
        data.p_offset,
        data.queueName,
        data.agentName,
        data.dispositionName

      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
      logger.error('getchatSessionReport - Unexpected Error:', err);
    }
  });
};

/**
 * @description ccass get session history agent chat details
 * @param data
 * @returns
 */
export const getagentChatReport = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_get_session_history_agent_chat_details(?,?,?,?,?,?,?,?)', [
        data.startTmpstmp,
        data.endTmpstmp,
        data.timeZone,
        data.domainId,
        data.limit,
        data.offset,
        data.queueName,
        data.agentName
      ],                        (err: any, result: any) => {
        if (err) {
          logger.error('getagentChatReport - Unexpected Error:', err);
          reject(err);
        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
      logger.error('getagentChatReport - Unexpected Error:', err);
    }
  });
};

export const getACWChatReport = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call pros_get_chat_history_details_acw(?,?,?,?,?,?,?)', [
        data.startTmpstmp,
        data.endTmpstmp,
        data.domainId,
        data.queueName,
        data.agentName,
        data.limit,
        data.offset
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('getagentChatReport - Unexpected Error:', err);
        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
      logger.error('getACWChatReport - Unexpected Error:', err);
    }
  });
};

export const getAgentQueueChatReport = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('call pros_get_agent_queue_chat_Report(?,?,?,?,?,?,?)', [
        data.startTmpstmp,
        data.endTmpstmp,
        data.domainId,
        data.queueName,
        data.agentName,
        data.limit,
        data.offset
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('getAgentQueueChatReport - Unexpected Error:', err);
        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
      logger.error('getAgentQueueChatReport - Unexpected Error:', err);
    }
  });
};

export const getChatInteractionReport = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_get_session_history_chat_interaction_details(?,?,?,?,?,?,?,?,?,?)', [
        data.startTmpstmp,
        data.endTmpstmp,
        data.timeZone,
        data.domainId,
        data.limit,
        data.offset,
        data.queueName,
        data.agentName,
        data.callType,
        data.disposition,

      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('getChatInteractionReport - Unexpected Error:', err);
        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
      logger.error('getChatInteractionReport - Unexpected Error:', err);
    }
  });
};

export const getChatProfileReportHandlerDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call up_ccaas_chat_profile_report(?,?,?,?,?,?,?,?,?)', [
        data?.startTmpstmp,
        data?.endTmpstmp,
        data?.timeZone,
        data?.domainId,
        data?.queueName,
        data?.agentName,
        data?.disposition,
        data?.offset,
        data?.limit,
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('getChatProfileReportHandlerDao - Unexpected Error:', err);
        }
        resolve(result);
      });
    } catch (err) {
      console.log('up_ccaas_chat_profile_report err: ', err);
      reject(err);
      logger.error('getChatProfileReportHandlerDao - Unexpected Error:', err);
    }
  });
};

export const getChatAgentReportHandlerDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call up_ccaas_chat_agent_report(?,?,?,?,?,?,?,?)', [
        data?.startTmpstmp,
        data?.endTmpstmp,
        data?.timeZone,
        data?.domainId,
        data?.queueName,
        data?.agentName,
        data?.offset,
        data?.limit,
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('getChatAgentReportHandlerDao - Unexpected Error:', err);
        }
        resolve(result);
      });
    } catch (error) {
      console.log('getChatAgentReportHandlerDao catch error: ', error);
      reject(error);
      logger.error('getChatAgentReportHandlerDao - Unexpected Error:', error);
    }
  });
};

export const getChatACWAgentReportHandlerDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call up_ccaas_chat_agent_ACW_report(?,?,?,?,?,?,?,?)', [
        data?.startTmpstmp,
        data?.endTmpstmp,
        data?.timeZone,
        data?.domainId,
        data?.queueName,
        data?.agentName,
        data?.offset,
        data?.limit,
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('getChatACWAgentReportHandlerDao - Unexpected Error:', err);
        }
        resolve(result);
      });
    } catch (error) {
      console.log('getChatACWAgentReportHandlerDao catch error: ', error);
      reject(error);
      logger.error('getChatACWAgentReportHandlerDao - Unexpected Error:', error);
    }
  });
};

export const getChatACWAgentReportHandlerDaoForSchedule = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call up_ccaas_chat_agent_ACW_report(?,?,?,?,?,?,?,?)', [
        data?.startTmpstmp,
        data?.endTmpstmp,
        data?.timeZone,
        data?.domainId,
        data?.queueName,
        data?.agentName,
        data?.offset,
        data?.limit,
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('getChatACWAgentReportHandlerDaoForSchedule - Unexpected Error:', err);
        }
        resolve(result?.length > 0 ? result?.[4] : []);
      });
    } catch (error) {
      console.log('getChatACWAgentReportHandlerDao catch error: ', error);
      reject(error);
      logger.error('getChatACWAgentReportHandlerDaoForSchedule - Unexpected Error:', error);
    }
  });
};
