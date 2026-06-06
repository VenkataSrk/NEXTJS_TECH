import { logger } from '../../src/plugins/winston';
import { mysqlPoolConnection } from '../plugins/db';

/**
 *
 * WebChat Create session MySql Migration
 *
 */
export const createChatSession = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_InsertUpdateChatHistory(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          parseInt(data?.ChatHistoryId, 10),
          data?.sessionId ?? null,
          data?.sessionStartTime ?? null,
          data?.sessionEndTime ?? null,
          data?.chatFlowSourceId ?? null,
          data?.companyId ?? null,
          data?.domainId ?? null,
          data?.IsInBound ?? null,
          data?.disconnectedBy ?? null,
          data?.queueDuration ?? null,
          data?.assignedTo ?? null,
          data?.assignedBy ?? null,
          data?.assignedOn ?? null,
          data?.CustomerId ?? null,
          data?.Browser ?? null,
          data?.IPAdress ?? null,
          data?.ChatSubject ?? null,
          data?.Message ?? null,
          data?.StartTime ?? null,
          data?.EndTime ?? null,
          data?.Duration ?? null,
          data?.AgentId ?? null,
          data?.AgentSkillId ?? null,
          data?.IsBot ?? null,
          data?.BotId ?? null,
          data?.transferredTime ?? null,
          data?.QueueId ?? null,
          data?.location ?? null,
          data?.DeviceType ?? null,
          data?.CallBackStatus ?? null,
          data?.widgetId ?? null,
          data?.IsMissed ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('up_InsertUpdateChatHistory', { data, Err: err });
          } else {
            resolve(result);
          }
        }
      );
    } catch (error) {
      logger.error('createChatSession', { data, Err: error });
    }
  });
};

export const updateAgentDetails = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_InserUpdateChatDurationHistory(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          (data?.ChatDurationHistoryId !== '' || data?.ChatDurationHistoryId !== null) ? data?.ChatDurationHistoryId : 0,
          parseInt(data?.ChatHistoryId, 10) ?? 0,
          data?.StartTime,
          data?.EndTime,
          data?.Duration,
          data?.AgentId,
          data?.AgentSkillId,
          data?.IsBot,
          data?.BotId,
          data?.transferredTime,
          data?.QueueId,
          data?.dispositionId,
          data?.transferedBy,
          data?.transferedTo,
          data?.ticketId,
          data?.Summary,
          data?.SentimentalScore,
          data?.sentimentalScoreCustomer,
          data?.sentimentalScoreAgent,
          data?.markAsRead,
          data?.afterChatWorkTime,
          data?.Istransferred,
          data?.IsMissed,
          data?.IsAbandoned,
          data?.answeredDuration ?? null,
          data?.chatDeflection ?? null,
          data?.isTrigger ?? 0
        ],
        (err: any, result: any) => {
          if (err) {
            logger.error('up_InserUpdateChatDurationHistory', { data, Err: err });
            reject(err);
          } else {
            resolve(result);
          }
        }
      );
    } catch (error) {
      logger.error('updateAgentDetails', { data, Err: error });
    }
  });
};

export const updateChatMessage = (data: any) => {
  const isCustomer: any = data?.message?.userText?.message?.isCustomer ? 1 : 0;
  const isAgent: any = isCustomer ? 0 : 1;
  const message: any = JSON.stringify(data.message);
  const chat_id = parseInt(data?.ChatHistoryId, 10);
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_InserUpdateChatMessage(?,?,?,?,?)',
        [
          data?.ChatDurationHistoryId ?? 0,
          chat_id ?? 0,
          message,
          isCustomer,
          isAgent,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('up_InserUpdateChatMessage', { data, Err: err });
          } else {
            resolve(result);
          }
        }
      );
    } catch (error) {
      logger.error('updateChatMessage', { data, Err: error });
    }
  });
};

export const InsertUpdateAIChatMessage = (data: any) => {
  const message: any = JSON.stringify(data);
  const chatHisId = parseInt(data?.ChatHistoryId, 10);
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_InsertUpdateChatAIMessage(?,?,?)',
        [chatHisId ?? 0, message, data?.sessionId],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('up_InsertUpdateChatAIMessage', { data, Err: err });
          } else {
            resolve(result);
          }
        }
      );
    } catch (error) {
      logger.error('InsertUpdateAIChatMessage', { data, Err: error });
    }
  });
};

export const getAIChatMessageDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetChatAIMessage(?)',
        [data?.ChatHistoryId ?? 0],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('up_GetChatAIMessage', { data, Err: err });
          } else {
            resolve(result);
          }
        }
      );
    } catch (error) {
      logger.error('getAIChatMessageDao', { data, Err: error });
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const setChatMessage = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_SetChatMessage(?,?,?)',
        [parseInt(data?.ChatDurationHistoryId, 10),
          parseInt(data?.ChatHistoryId, 10),
          data?.message
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('up_SetChatMessage', { data, Err: err });
          } else {
            resolve(result);
          }
        }
      );
    } catch (error) {
      logger.error('setChatMessage', { data, Err: error });
    }
  });
};

/**
 *
 * @param data
 * @returns
 */

export const setTransferChatMessage = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_TransaferChatMessage(?,?)',
        [
          data?.SourceChatDurationHistoryId,
          data?.DestinationChatDurationHistoryId
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('up_TransaferChatMessage', { data, Err: err });
          } else {
            resolve(result);
          }
        }
      );
    } catch (error) {
      logger.error('setTransferChatMessage', { data, Err: error });
    }
  });
};

/**
 *
 * @param data
 * @param dataq
 * @returns
 */
export const getAgentAvailability = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_getUserListByRoleHandler(?,?)',
        [
          data?.domainId,
          data?.roleId
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_getUserListByRoleHandler', { data, Err: err });
          } else {
            if (result?.[0]?.length > 0) {
              resolve(result?.[0]);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (error) {
      reject(error);
      logger.error('getAgentAvailability', { data, Err: error });
    }
  });
};

/**
 *
 * @param data
 * @param dataq
 * @returns
 */
export const replaceWebchatDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_SetChatMessage(?,?,?)',
        [
          data?.chatDurationHistoryId,
          data?.chatHistoryId,
          JSON.stringify(data?.message)
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('up_SetChatMessage', { data, Err: err });
          } else {
            resolve(result);
          }
        });
    } catch (err) {
      reject(err);
      logger.error('replaceWebchatDao', { data, Err: err });
    }
  });
};
