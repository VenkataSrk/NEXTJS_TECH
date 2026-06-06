import { logger } from '../../src/plugins/winston';
import { mysqlPoolConnection } from '../plugins/db';

/**
 *
 * @param data
 * @returns get Message WebChat
 */
export const getMessageWebChat = (id: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetChatMessageById(?)',
        [
          id ?? 0,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getMessageWebChat err', { data: id, Err: err });
          }
          resolve(result);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('getMessageWebChat catch', { data: id, Err: error });

    }
  });
};

/**
 * @description get the chat session
 * @param data
 * @returns
 */
export const getChatSessionIdDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      console.log(data, 'getChatSessionIdDao payload');
      logger.info({ data }, 'getChatSessionIdDao payload');

      const chatHistoryId = parseInt(data, 10);
      mysqlPoolConnection.query(
        'call ccaass_get_chat_sessionid_dao(?)',
        [chatHistoryId],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('updateConfig err', { data, Err: err });
          } else {
            // result[0].dispositionId = null;
            console.log('result-------------', result);
            logger.info('getChatSessionIdDao result', result);
            resolve(result?.length > 0 ? result?.[0] : []);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('getChatSessionIdDao catch', { data, Err: error });

    }
  });
};

/**
 * @description get the chat session
 * @param data
 * @returns
 */
export const getChatDurationdao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const chatHistoryId = parseInt(data, 10);
      mysqlPoolConnection.query(
        'call ccaas_get_chat_duration_dao(?)',
        [chatHistoryId],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('getChatDurationdao err', { data, Err: err });
          } else {
            resolve(result?.[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('getChatDurationdao catch', { data, Err: error });

    }
  });
};

/**
 * @description get the Call session
 * @param data
 * @returns
 */
export const getCallSessionIdDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      console.log('getCallSessionId payload', data, 'Date.now()', Date.now());
      logger.info('getCallSessionIdDao payload', data, 'Date.now()', Date.now());

      mysqlPoolConnection.query(
        'call up_getCallHistoryBySessionId(?,?)',
        [
          data?.CallSessionId,
          data?.domainId,
        ],
        (err: any, result: any) => {
          console.log('getCallSessionIdDao result', data?.CallSessionId, result, 'Date.now()', Date.now());
          logger.info('getCallSessionIdDao result', data?.CallSessionId, result, 'Date.now()', Date.now());

          if (err) {
            reject(err);
            console.log('getCallSessionIdDao err', data?.CallSessionId, err, 'Date.now()', Date.now());
            logger.error('getCallSessionIdDao err', data?.CallSessionId, result, 'Date.now()', Date.now());
          }
          resolve(result);
        }
      );
    } catch (error) {
      logger.error('getCallSessionIdDao catch', data, error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getListenSessionIdDao = (data: any) => {
  return new Promise((resolve, reject) => {
    console.log(data);
    logger.info('getListenSessionIdDao data', data);

    try {
      mysqlPoolConnection.query(
        'call ccaas_getAgentStatusTracker(?,?)',
        [
          data?.sessionId,
          data?.domainId
        ],
        (error: any, result: any) => {
          if (error) {
            console.log(error);
            logger.error('getListenSessionIdDao err', error);
            reject(error);
          }
          resolve(result);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('getListenSessionIdDao catch', data, error);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getAgentStatusViewDao = (data: any) => {
  return new Promise((resolve, reject) => {
    console.log('data>>>>>>>', data);
    logger.info('getListenSessionIdDao data', data);

    try {
      mysqlPoolConnection.query(
        'call ccaas_getAgentStatusView(?)',
        [
          data.domainId,
        ],
        (error: any, result: any) => {
          if (error) {
            console.log(error);
            reject(error);
            logger.error('getAgentStatusViewDao err', error);

          }
          resolve(result);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('getAgentStatusViewDao catch', data, error);

    }
  });
};

/**
 * @description get chat number from
 * @param data
 * @returns
 */
// check with db team customerNumber
export const getChatByNumberDao = (data: { number?: string; domainId?: number }) => {
  return new Promise((resolve, reject) => {
    try {
      if (!data?.number) {
        return resolve([]);
      }

      mysqlPoolConnection.query(
        'CALL ccaas_get_call_details_by_customer_number(?, ?)',
        [data.number, data.domainId],
        (err: any, result: any) => {
          if (err) {
            logger.error('getChatByNumberDao err', data, err);
            return reject(err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      logger.error('getChatByNumberDao catch', data, error);
      reject(error);
    }
  });
};

export const saveChatFlowDao = (data: any, nodeData: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_InsertUpdatechatFlowHistory(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          parseInt(data?.cfid, 10),
          data?.templateName,
          nodeData?.node,
          nodeData?.edge,
          data?.version,
          data?.type,
          data?.status,
          data?.companyId,
          data?.domainId,
          data?.description,
          data?.publishedDataArr,
          data?.action,
          data?.category,
          data?.channelType ?? null,
          data?.isLanguage
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log(err);
            logger.error('saveChatFlowDao err', err);

          }
          resolve(result);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('saveChatFlowDao catch', data, err);

    }
  });
};
export const getChatFlowIdDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {

      mysqlPoolConnection.query(
        'call ccaas_get_chatflow_details(?,?)',
        [
          data?.chatFlowId,
          data?.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getChatFlowIdDao err', data, err);
          } else {
            resolve(result?.length > 0 ? result?.[0] : []);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('getChatFlowIdDao err', data, error);

    }
  });
};
export const getAllChatFlowIdDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_chatflow_details(?,?)',
        [
          null,
          data?.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getAllChatFlowIdDao err', data, err);

          } else {
            resolve(result?.length > 0 ? result?.[0] : []);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('getAllChatFlowIdDao catch', data, error);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const deleteChatBotChannelType = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL DeleteChatBotChannelType(?,?)',
        [
          data.chatFlowId,
          data.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            resolve(err);
            logger.error('deleteChatBotChannelType err', data, err);
          } else {
            resolve(result?.[0]);
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('deleteChatBotChannelType catch', data, err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateChatBotChannelType = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL updateChatBotChannelType(?,?,?,?,?,?)',
        [
          data.chatFlowId,
          data.channelType,
          data.templateName,
          data.description,
          data.isLanguage,
          data.domainId,
        ],
        (err: any, result: any) => {
          if (err) {
            resolve(err);
            logger.error('updateChatBotChannelType err', data, err);
          } else {
            resolve(result?.[0]);
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('updateChatBotChannelType catch', data, err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getChatflowByDomainDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL up_GetChatflowTemplate(?)',
        [
          data.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            resolve(err);
            logger.error('getChatflowByDomainDao err', data, err);
          } else {
            resolve(result?.[0]);
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('getChatflowByDomainDao catch', data, err);
    }
  });
};

export const getDefaultChatFlowIdDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_get_default_chat_flow_id(?)',
        [
          data
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getDefaultChatFlowIdDao err', data, err);
          } else {
            resolve(result?.length > 0 ? result?.[0] : []);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('getDefaultChatFlowIdDao catch', data, error);
    }
  });
};

export const getNodeDtlDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_get_chatFlowHistory_dtl(?,?)',
        [
          data.domainId ?? null,
          data.chatFlowId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getNodeDtlDao err', data, err);
          } else {
            resolve(result);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('getNodeDtlDao catch', data, error);
    }
  });
};
