import { v4 as uuidv4 } from 'uuid';
import { mysqlPoolConnection } from '../plugins/db';
import { logger } from '../plugins/winston';

/**
 *
 * @param data
 * @param nodeData
 * @returns
 */
export const savecallflow = (data: any, nodeData: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_call_Flow_details(?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.cfid,
          data?.templateName,
          nodeData?.node,
          nodeData?.edge,
          data?.version,
          data?.type,
          data?.createDate,
          data?.lastModifiedDate,
          data?.status,
          data?.companyId,
          data?.domainId,
          data?.description,
          data?.publishedDataArr,
          data?.callFlowId ?? 0
        ],
        (err: any, result: any) => {
          if (err) {
            logger.error('ccaas_call_Flow_details', [data?.cfid, data?.templateName, nodeData?.node, nodeData?.edge, data?.version, data?.type, data?.createDate, data?.lastModifiedDate, data?.status, data?.companyId, data?.domainId, data?.description, data?.publishedDataArr, data?.callFlowId ?? 0], err);
            reject(err);
          }
          resolve(result);
        }
      );
    } catch (err) {
      logger.error('savecallflow', [data?.cfid, data?.templateName, nodeData?.node, nodeData?.edge, data?.version, data?.type, data?.createDate, data?.lastModifiedDate, data?.status, data?.companyId, data?.domainId, data?.description, data?.publishedDataArr, data?.callFlowId ?? 0], err);
      reject(err);
    }
  });
};

/**
 * @description Get Call flow list
 * @param data
 * @returns
 */
export const callFlowList = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call callFlowList(?)',
        [data?.domainId ?? null],
        (err: any, result: any) => {
          if (err) {
            logger.error('callFlowList', [data?.domainId ?? null], err);
            reject(err);
          }
          resolve(result?.length > 0 ? result?.[0] : {});
        }
      );
    } catch (error) {
      logger.error('callFlowList', [data?.domainId ?? null], error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const callFlowDetails = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_call_get_flow_details(?,?)',
        [data?.domainId ?? null, data?.cfid ?? null],
        (err: any, result: any) => {
          if (err) {
            logger.error('ccaas_call_get_flow_details', [data?.domainId ?? null, data?.cfid ?? null], err);
            reject(err);
          }
          resolve(result?.length > 0 ? result?.[0] : []);
        }
      );
    } catch (error) {
      logger.error('callFlowDetails', [data?.domainId ?? null, data?.cfid ?? null], error);
      reject(error);
    }
  });
};

/**
 *
 * @param domainId
 * @param sessionId
 * @returns
 */
export const getSessionIdDao = (domainId: any, sessionId: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_sessionId_Dao(?,?)',
        [domainId ?? null, sessionId ?? null],
        (err: any, result: any) => {
          if (err) {
            logger.error('ccaas_get_sessionId_Dao', [domainId ?? null, sessionId ?? null], err);
            reject(err);
          }
          resolve(result?.length > 0 ? result?.[0] : []);
        }
      );
    } catch (error) {
      logger.error('getSessionIdDao', [domainId ?? null, sessionId ?? null], error);
      reject(error);
    }
  });
};

/**
 * get CallBack List
 * @param data
 * @returns
 */
export const getCallBackList = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccass_get_callback_session_history_details(?,?,?,?,?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.channelType,
          data?.fromDate,
          data?.toDate,
          '',
          data?.userID ?? null,
          data?.search,
          data?.offset,
          data?.limit,
          data?.queueId
        ],
        (err: any, result: any) => {
          if (err) {
            logger.error('ccass_get_callback_session_history_details', [data?.domainId, data?.channelType, data?.fromDate, data?.toDate, '', data?.userID ?? null, data?.search, data?.offset, data?.limit, data?.queueId], err);
            reject(err);
          }
          resolve(result?.length ? result : {});
        }
      );
    } catch (err) {
      logger.error('getCallBackList', [data?.domainId, data?.channelType, data?.fromDate, data?.toDate, '', data?.userID ?? null, data?.search, data?.offset, data?.limit, data?.queueId], err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCallSessionHistory = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetSessionHistoryDetails(?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.channelType,
          data?.dispositionId ?? null,
          data?.isInbound ?? null,
          data?.fromDate ?? null,
          data?.toDate ?? null,
          data?.userID ?? null,
          data?.offset ?? null,
          data?.limit ?? null,
          data?.search ?? null,
          data?.sentimentalFrom ?? null,
          data?.sentimentalTo ?? null,
          data?.queueId ?? null,
          data?.isQualifiedUser
        ],
        (err: any, result: any) => {
          if (err) {
            logger.error('up_GetSessionHistoryDetails', [data?.domainId, data?.channelType, data?.dispositionId ?? null, data?.isInbound ?? null, data?.fromDate ?? null, data?.toDate ?? null, data?.userID ?? null, data?.offset ?? null, data?.limit ?? null, data?.search ?? null, data?.sentimentalFrom ?? null, data?.sentimentalTo ?? null, data?.queueId ?? null], err);
            reject(err);
          }
          resolve(result?.length ? result?.[0] : []);
        }
      );
    } catch (err) {
      logger.error('getCallSessionHistory', [data?.domainId, data?.channelType, data?.dispositionId ?? null, data?.isInbound ?? null, data?.fromDate ?? null, data?.toDate ?? null, data?.userID ?? null, data?.offset ?? null, data?.limit ?? null, data?.search ?? null, data?.sentimentalFrom ?? null, data?.sentimentalTo ?? null, data?.queueId ?? null], err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateVoiceMailCallbackDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_update_voiceMail_callBack(?,?)',
        [
          data?.sessionId ?? null,
          data?.domainId ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            logger.error('ccaas_update_voiceMail_callBack', [data?.sessionId ?? null, data?.domainId ?? null], err);
            reject(err);
          } else {
            if (result?.[0]?.length > 0) {
              resolve(result?.[0]);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (err) {
      logger.error('updateVoiceMailCallbackDao', [data?.sessionId ?? null, data?.domainId ?? null], err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getRoleIdList = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      if (data?.emailId) {
        mysqlPoolConnection.query(
          'call getRoleIdList(?)',
          [
            data?.emailId ?? null
          ],
          (err: any, result: any) => {
            if (err) {
              logger.error('getRoleIdList', [data?.emailId ?? null], err);
              reject(err);
            } else if (result && result?.length > 0) {
              resolve(result?.[0]);
            } else {
              resolve([]);
            }
          }
        );
      }
    } catch (error) {
      logger.error('getRoleIdList', [data?.emailId ?? null], error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const assignCallBack = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const sessionId: any = data.Istransferred === 1 ? uuidv4() : null;
      mysqlPoolConnection.query(
        'call up_UpdateAssignAgentToEmail(?,?,?,?,?,?)',
        [
          data.assignedBy ?? null,
          data.assignedTo ?? null,
          data.assignedOn ?? null,
          data.EmailHistoryId ?? null,
          data.Istransferred ?? null,
          sessionId ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            logger.error('up_UpdateAssignAgentToEmail', [data.assignedBy ?? null, data.assignedTo ?? null, data.assignedOn ?? null, data.EmailHistoryId ?? null, data.Istransferred ?? null, sessionId ?? null], err);
            reject(err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      logger.error('assignCallBack', [data.assignedBy ?? null, data.assignedTo ?? null, data.assignedOn ?? null, data.EmailHistoryId ?? null, data.Istransferred ?? null], error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const assignCallBackCall = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_UpdateAssignAgentToCall(?,?,?,?)',
        [
          data.assignedBy,
          data.assignedTo,
          data.assignedOn,
          data.CallHistoryId,
        ],
        (err: any, result: any) => {
          if (err) {
            logger.error('up_UpdateAssignAgentToCall', [data.assignedBy, data.assignedTo, data.assignedOn, data.CallHistoryId], err);
            reject(err);
          }
          resolve(result?.length > 0 ? result?.[0] : []);
        }
      );
    } catch (error) {
      logger.error('assignCallBackCall', [data.assignedBy, data.assignedTo, data.assignedOn, data.CallHistoryId], error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateCallBackSessionId = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_UpdateCallBackCallHistoryId(?,?)',
        [
          data.CallHistoryId,
          data.callBackSessionId
        ],
        (err: any, result: any) => {
          if (err) {
            logger.error('up_UpdateCallBackCallHistoryId', [data.CallHistoryId, data.callBackSessionId], err);
            reject(err);
          }
          resolve(result?.length > 0 ? result?.[0] : []);
        }
      );
    } catch (error) {
      logger.error('updateCallBackSessionId', [data.CallHistoryId, data.callBackSessionId], error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateCallBackSessionIdDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_UpdateCallBacksessionId(?,?)',
        [
          data.ParentsessionId,
          data.CallBacksessionId
        ],
        (err: any, result: any) => {
          if (err) {
            logger.error('up_UpdateCallBacksessionId', [data.ParentsessionId, data.CallBacksessionId], err);
            reject(err);
          }
          resolve(result?.length > 0 ? result?.[0] : []);
        }
      );
    } catch (error) {
      logger.error('updateCallBackSessionIdDao', [data.ParentsessionId, data.CallBacksessionId], error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateChatBackSessionId = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_UpdateCallBackChatHistoryId(?,?)',
        [
          data.ChatHistoryId,
          data.callBackSessionId
        ],
        (err: any, result: any) => {
          if (err) {
            logger.error('up_UpdateCallBackChatHistoryId', [data.ChatHistoryId, data.callBackSessionId], err);
            reject(err);
          }
          resolve(result?.length > 0 ? result?.[0] : []);
        }
      );
    } catch (error) {
      logger.error('updateChatBackSessionId', [data.ChatHistoryId, data.callBackSessionId], error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateCallBackSocialmedia = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_UpdateSocialMediaCallBacksessionId(?,?,?)',
        [
          data.callBackSocialmediaHistoryId,
          data.callBackSessionId,
          data.channelType
        ],
        (err: any, result: any) => {
          if (err) {
            logger.error('up_UpdateSocialMediaCallBacksessionId', [data.callBackSocialmediaHistoryId, data.callBackSessionId, data.channelType], err);
            reject(err);
          }
          resolve(result?.length > 0 ? result?.[0] : []);
        }
      );
    } catch (error) {
      logger.error('updateCallBackSocialmedia', [data.callBackSocialmediaHistoryId, data.callBackSessionId, data.channelType], error);
      reject(error);
    }
  });
};

/**
 * get activity list
 * @param data
 * @returns
 */
export const getActivityList = (data: any) => {
  return new Promise(async (resolve, reject) => {
    const resultExt = data?.ext?.toString();
    if (data?.channelType === 'Chat,Call,Email') {
      data.channelType = 'All';
    }
    try {
      mysqlPoolConnection.query(
        'call ccass_get_session_history_details(?,?,?,?,?,?,?,?,?,?)',
        [
          data.domainId,
          data.channelType,
          data.disposition === 'All' ? 0 : data.disposition,
          data.callType,
          data.fromDate,
          data.toDate,
          resultExt,
          data.search,
          data.offset,
          data.limit
        ],
        (err: any, result: any) => {
          if (err) {
            logger.error('ccass_get_session_history_details', [data.domainId, data.channelType, data.disposition === 'All' ? 0 : data.disposition, data.callType, data.fromDate, data.toDate, resultExt, data.search, data.offset, data.limit], err);
            reject(err);
          }
          resolve(result?.length ? result?.[0] : {});
        }
      );
    } catch (err) {
      logger.error('getActivityList', [data.domainId, data.channelType, data.disposition === 'All' ? 0 : data.disposition, data.callType, data.fromDate, data.toDate, resultExt, data.search, data.offset, data.limit], err);
      reject(err);
    }
  });
};

/**
 *
 * @param sessionId
 * @param domainId
 * @returns
 */
export const getConversationList = (sessionId: any, domainId: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_conversation_List(?,?)',
        [
          sessionId ?? null,
          domainId ?? 0,
        ],
        (err: any, result: any) => {
          if (err) {
            logger.error('ccaas_get_conversation_List', [sessionId ?? null, domainId ?? 0], err);
            reject(err);
          }
          resolve(result?.length > 0 ? result?.[0] : []);
        }
      );
    } catch (error) {
      logger.error('getConversationList', [sessionId ?? null, domainId ?? 0], error);
      reject(error);
    }
  });
};

/**
 *
 * @param chatDurationHistoryId
 * @returns
 */
export const getCustomerChatHistory = (chatDurationHistoryId: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetChatMessageById(?)',
        [
          chatDurationHistoryId ?? 0,
        ],
        (err: any, result: any) => {
          if (err) {
            logger.error('up_GetChatMessageById', [chatDurationHistoryId ?? 0], err);
            reject(err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      logger.error('getCustomerChatHistory', [chatDurationHistoryId ?? 0], error);
      reject(error);
    }
  });
};

/**
 *
 * @param chatUniqIdentifier
 * @returns
 */
export const getCustomerChatHistoryByUid = (chatUniqIdentifier: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetChatMessageByUniqueIdentifier(?)',
        [
          chatUniqIdentifier ?? 0,
        ],
        (err: any, result: any) => {
          if (err) {
            logger.error('up_GetChatMessageByUniqueIdentifier', [chatUniqIdentifier ?? 0], err);
            reject(err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      logger.error('getCustomerChatHistoryByUid', [chatUniqIdentifier ?? 0], error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const roleInfoDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_get_agentid_role(?,?)',
                                [data?.ext ?? null, data?.domainId],
                                (err: any, result: any) => {
                                  if (err) {
                                    logger.error('ccaas_get_agentid_role', [data?.ext, data?.domainId], err);
                                    reject(err);
                                  } else {
                                    resolve(result?.length > 0 ? result?.[0]?.[0] : null);
                                  }
                                });
    } catch (err) {
      logger.error('roleInfoDao', [data?.ext, data?.domainId], err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const deleteCallFlowInfoDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL DeleteCallFlow(?,?)',
        [
          data?.callFlowId ?? null,
          data?.domainId,
        ],
        (err: any, result: any) => {
          if (err) {
            logger.error('DeleteCallFlow', [data?.callFlowId ?? null, data?.domainId], err);
            reject(err);
          } else {
            if (result?.[0]?.length > 0) {
              resolve(result?.[0]);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (err) {
      logger.error('deleteCallFlowInfoDao', [data?.callFlowId ?? null, data?.domainId], err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCustomerDndInfoDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {

      mysqlPoolConnection.query(
        'CALL ccaas_get_DND_customer_dtl(?,?)',
        [
          data?.domainId,
          data?.customerNumber ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            logger.error('ccaas_get_DND_customer_dtl', [data?.domainId, data?.customerNumber ?? null], err);
            reject(err);
          } else {
            if (result?.[0]?.length > 0) {
              resolve(result?.[0]);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (err) {
      logger.error('getCustomerDndInfoDao', [data?.domainId, data?.customerNumber ?? null], err);
      reject(err);
    }
  });
};
