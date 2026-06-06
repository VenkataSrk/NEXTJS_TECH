import axios from 'axios';
import { v4 as uuidv4 } from 'uuid';
import { channelType, channelTypeCode } from '../helpers/constants';
import { executeMySQLWithRetry } from '../helpers/dbRetryUtil';
import { sendMessageCustomerInstagram, sendMessageSatisficationInsta } from '../helpers/instameta';
import { sendMessageCustomerCountInsta } from '../modules/instagram/handler';
import { mysqlPoolConnection } from '../plugins/db';
import { logger } from '../plugins/log';

/**
 *
 * @param data
 * @returns
 */
export const instagramFeedbackUpdateDB = async (data: any) => {
  try {
    const value: any = {
      historyId: data?.text?.body?.split('-')[1],
      satisfactionRate: data?.text?.body?.split('-')[0] ?? 0,
      channelTypeId: channelTypeCode?.Instagram,
      domainId: data?.text?.body?.split('-')[2],
      sessionId: data?.sessionId ?? null,
      AgentId: data?.text?.body?.split('-')[3],
      ratingDesignType: data?.text?.body?.split('-')[4],
    };
    insertUpdateCustomerFeedbackDaoInsta(value);
    const getFeedbackMessage = await getCustomerSatisficationMsgInstagram(value);
    await sendMessageSatisficationInsta(data, getFeedbackMessage, value?.domainId);
    return;
  } catch (err) {
    logger.error('instagramFeedbackUpdateDB err', err);
    return;
  }
};

/**
 * @description instagram insert
 * @param data
 * @returns
 */
export const insertInstagramIn = async (data: any) => {
  let jsonValue = {};
  const replyContext = data?.replyContext ? { replyToMessageId: data?.replyContext?.replyToMessageId, replyFrom: data?.replyContext?.replyFrom } : null;
  let mediaUrl: any = '';
  if (['audio'].includes(data?.type)) {
    mediaUrl = await getInstaMediaUrl(data);
  }
  if (data?.IsBot === 1 && data?.IsBot !== undefined) {
    jsonValue = { ...data.message };
  } else {
    jsonValue = {
      userText: {
        message: {
          replyContext,
          pageId: data?.pageId,
          customerId: data?.customerId,
          isBot: false,
          pdfurl: '',
          sendAt: Date.now(),
          sender: {},
          message: data?.type === 'quickReply' ? data?.text?.title : data?.text?.body || data?.text || '',
          fileType: '',
          messageID: uuidv4,
          messageId: data?.messageId,
          sessionId: data.sessionId,
          isCustomer: true,
          messageType: 'Instagram',
          attachments: data?.type === 'image'
            ? [{
              href: data?.media?.url,
              type: data?.type,
              size: '',
              filename: '',
              fileType: ['jpeg', 'jpg', 'gif', 'png']
                .find(val => data?.media?.url.includes(val)) ? `image/${['jpeg', 'jpg', 'gif', 'png'].find(val => data?.media?.url.includes(val))}` : ''
            }]
            : data?.type === 'video'
              ? [{
                href: data?.media?.url,
                type: data?.type,
                size: '',
                filename: '',
                fileType: ['mp4', 'ogg', 'avi', 'mov', 'webm']
                  .find(val => data?.media?.url.includes(val)) ? `video/${['mp4', 'ogg', 'avi', 'mov', 'webm'].find(val => data?.media?.url.includes(val))}` : ''
              }]
              : data?.type === 'audio'
                ? [{
                  href: mediaUrl?.previewUrl,
                  type: data?.type,
                  size: '',
                  filename: mediaUrl?.fileName,
                  fileType: ['m4a', 'acc', 'mp4', 'wav', 'ogg']
                    .find(val => data?.media?.url.includes(val)) ? `audio/${['m4a', 'acc', 'mp4', 'wav', 'ogg'].find(val => data?.media?.url.includes(val))}` : ''
                }]
                : null,
          id: data?.id ?? '',
          is_deleted: data?.is_deleted ?? false,
        },
      },
    };
  }
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL up_InsertUpdateInstagram(?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.sessionId ?? null,
          data?.sessionStartTime ?? new Date().getTime(),
          data?.sessionEndTime ?? 0,
          data?.domainId ?? null,
          JSON.stringify(jsonValue) ?? null,
          data?.customerId ?? null,
          data?.pageId ?? null,
          data?.dispositionId ?? null,
          1,
          data?.IsSessionClosed ?? 0,
          data?.IsBot ?? 0,
          data?.Summary ?? null,
          data?.status ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            resolve(err);
          } else {
            if (result && result.length > 0) {
              resolve(result[0]);
            } else {
              reject(err);
            }
          }
        }
      );
    } catch (err) {
      logger.error('insertInstagramIn err', err);
      reject(err);
    }
  });
};

/**
 * @description send ccaas Facebook
 * @param data
 * @returns
 */
export const sendCaasInstagram = (data: any, sessionId: any) => {
  const message = JSON.stringify(data?.message);
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL up_InsertUpdateInstagram(?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          sessionId,
          null,
          null,
          null,
          message,
          data.customerId,
          data.pageId,
          null,
          null,
          0,
          0,
          null,
          'sent'
        ],
        (err: any, result: any) => {
          if (err) {
            resolve(err);
          } else {
            if (result && result.length > 0) {
              resolve(result[0]);
            } else {
              reject(err);
            }
          }
        }
      );
    } catch (err) {
      logger.error('sendCaasInstagram err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @param data1
 * @returns
 */
export const getBotDetailInstagram = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_GetInstagramBot(?,?)',
        [
          data?.agentInstagramId,
          data?.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
          }
        }
      );
    } catch (err) {
      logger.error('getBotDetailInstagram err', err);
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
export const agentDetailsInstagramApi = (sessionId: any, domainId: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccass_getDetailsBySessionId(?,?,?)',
        [domainId, channelType?.Instagram, sessionId],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
          }
        }
      );
    } catch (err) {
      logger.error('agentDetailsInstagramApi err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param InstagramIds
 * @param domainId
 * @returns
 */
export const getPersonalDetailsInstagram = (InstagramIds: any, domainId: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetPersonalDetailsForInstagram(?,?)',
        [InstagramIds, domainId],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
          }
        }
      );
    } catch (err) {
      logger.error('getPersonalDetailsInstagram err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const autoAssignAgentInstagram = (data: any, presenceStatus: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_AutoAssignAgentToInstagram(?,?,?)',
        [
          data?.InstagramHistoryId ?? 0,
          data?.domainId,
          presenceStatus ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
          }
        }
      );
    } catch (err) {
      logger.error('autoAssignAgentInstagram err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateSentimentScoreInstagram = (data: any, sentiment: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_updateSentimentalScore(?,?,?,?,?,?,?,?)',
        [
          data?.InstagramHistoryId ?? 0,
          data?.ChatDurationHistoryId ?? 0,
          channelType?.Instagram,
          data?.domainId ?? null,
          sentiment?.sentimentalScore ?? null,
          sentiment?.sentimentalScoreUser ?? null,
          sentiment?.sentimentalScoreAgent ?? null,
          data?.ext ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
          }
        }
      );
    } catch (err) {
      logger.error('updateSentimentScoreInstagram err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateSentimentScoreInstagramBot = (data: any, sentiment: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_updateSentimentalScore(?,?,?,?,?,?,?,?)',
        [
          data?.InstagramHistoryId ?? 0,
          data?.ChatDurationHistoryId ?? 0,
          channelType?.Instagram,
          data?.domainId ?? null,
          sentiment ?? null,
          sentiment?.sentimentalScoreUser ?? null,
          sentiment?.sentimentalScoreAgent ?? null,
          data?.ext ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
          }
        }
      );
    } catch (err) {
      logger.error('updateSentimentScoreInstagramBot err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertUpdateInstagramConfig = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_InsertUpdateInstagramConfiguration(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.instagramConfigurationId ?? null,
          data?.pageName ?? null,
          data?.language ?? null,
          data?.domainId ?? null,
          data?.planId ?? null,
          data?.isBot ?? 0,
          data?.isBotPurchased ?? null,
          data?.instagramFlowId ?? null,
          data?.instagramPageId ?? null,
          data?.isActive ?? null,
          data?.instagramintegrationId,
          data?.callBackToggle ?? null,
          data?.callBackForm ?? null,
          data?.notifyAdmin ?? null,
          data?.KnowledgeBaseId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
          }
        }
      );
    } catch (err) {
      logger.error('insertUpdateInstagramConfig err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getIntsagramConfig = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetInstagramConfigurationList(?)',
        [
          data?.domainId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
          }
        }
      );
    } catch (err) {
      logger.error('getIntsagramConfig err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getIntsagramConfigDetails = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetInstagramConfigurationDetails(?,?)',
        [
          data?.instaId ?? null,
          data?.domainId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
          }
        }
      );
    } catch (err) {
      logger.error('getIntsagramConfigDetails err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const deleteInstagramConfigDetails = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_DeleteInstagramConfiguration(?,?)',
        [
          data?.instaId,
          data?.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
          }
        }
      );
    } catch (err) {
      logger.error('deleteInstagramConfigDetails err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateStatusInstagram = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_SetInstagramConfigurationStatus(?,?,?)',
        [
          data?.instaConfigId,
          data?.domainId,
          data?.isActive
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
          }
        }
      );
    } catch (err) {
      logger.error('updateStatusInstagram err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateDispositionInstagram = async (data: any) => {
  try {
    const result = await executeMySQLWithRetry(
      mysqlPoolConnection,
      'call up_UpdateInstagramHistoryDisposition(?,?,?,?,?,?,?,?,?,?)',
      [
        data?.InstagramHistoryId,
        data?.domainId,
        data?.dispositionId ?? null,
        data?.summary ?? null,
        data?.isSessionClosed ?? 0,
        data?.afterInstagramWorkTime ?? null,
        data?.dispositionUpdatedBy ?? null,
        data?.duration ?? null,
        data?.dispositionSubject ?? null,
        data?.tags,
      ],
      {
        operationName: 'UpdateInstagramDisposition',
        logContext: { InstagramHistoryId: data?.InstagramHistoryId, domainId: data?.domainId }
      }
    );

    if (result && result.length > 0) {
      return result;
    }
    throw new Error('No result returned from disposition update');

  } catch (err) {
    logger.error('updateDispositionInstagram err', err);
    throw err;
  }
};

/**
 *
 * @param data
 * @returns
 */
export const updateSentimentScore = (data: any, sentimentScore: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_updateSentimentalScore(?,?,?,?,?)',
        [
          data?.InstagramHistoryId ?? 0,
          channelType?.Instagram,
          data?.domainId ?? null,
          sentimentScore ?? null,
          null,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
          }
        }
      );
    } catch (err) {
      logger.error('updateSentimentScore err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const assignCallBackInstagram = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const sessionId: any = data.Istransferred === 1 ? uuidv4() : null;
      mysqlPoolConnection.query(
        'call up_UpdateAssignAgentToInstagram(?,?,?,?,?,?)',
        [
          data?.assignedBy ?? null,
          data?.assignedTo ?? null,
          data?.assignedOn ?? null,
          data?.InstagramHistoryId ?? null,
          data.Istransferred ?? null,
          sessionId ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
          }
        }
      );
    } catch (err) {
      logger.error('assignCallBackInstagram err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getInstagramConfigData = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_CheckInstagramBot (?,?)',
        [
          data?.agentInstagramId ?? null,
          data?.domainId ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
          }
        }
      );
    } catch (err) {
      logger.error('getInstagramConfigData err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updatePageIdInstagram = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_Update_InstagramPageId(?,?,?)',
        [
          data?.domainId,
          data?.InstagramPageId,
          data?.pageName ?? ''
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
          }
        }
      );
    } catch (err) {
      logger.error('updatePageIdInstagram err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getNextNodeInsta = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetNextFlowNodeForInstagram(?,?)', [
          data?.InstagramHistoryId ?? null,
          data?.domainId ?? 0
        ],
        (errors: any, res: any) => {
          if (errors) {
            console.log(errors);
            reject([]);
          } else {
            if (res[0]?.length > 0) {
              resolve(res[0]);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (error) {
      logger.error('getNextNodeInsta err', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateDispositionDiscnnectInsta = async (data: any, closedDispositionsId: any) => {
  try {
    const result = await executeMySQLWithRetry(
      mysqlPoolConnection,
      'call up_UpdateInstagramHistoryDisposition(?,?,?,?,?,?,?,?,?)',
      [
        data?.InstagramHistoryId,
        data?.domainId,
        closedDispositionsId?.did ?? null,
        closedDispositionsId?.dispositionName ?? null,
        1,
        data?.afterInstagramWorkTime ?? null,
        data?.dispositionUpdatedBy ?? null,
        data?.duration ?? null,
        data?.dispositionSubject ?? null,
      ],
      {
        operationName: 'UpdateInstagramDispositionOnDisconnect',
        logContext: { InstagramHistoryId: data?.InstagramHistoryId, domainId: data?.domainId }
      }
    );

    if (result && result.length > 0) {
      return result[0];
    }
    throw new Error('No result returned from disposition update');

  } catch (err) {
    logger.error('updateDispositionDiscnnectInsta err:', err);
    throw err;
  }
};

/**
 *
 * @param data
 * @returns
 */
export const updateDispositionBotInsta = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_InstagramIsBotUpdate(?)',
        [
          data?.InstagramHistoryId
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
          }
        }
      );
    } catch (err) {
      logger.error('updateDispositionBotInsta err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateChatMessageInsta = (data: any) => {
  const message: any = JSON.stringify(data.message);
  const chat_id = parseInt(data?.InstagramHistoryId, 10);
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_InserUpdateInstagramMessage(?,?)',
        [chat_id ?? 0, message],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          } else {
            if (result && result?.length > 0) {
              resolve(result);
            } else {
              reject(err);
            }
          }
        }
      );
    } catch (error) {
      logger.error('updateChatMessageInsta err:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateNextNodeInsta = (data: any, flowId: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_UpdateNextFlowNodeForInstagram(?,?,?)', [
          data?.InstagramHistoryId ?? null,
          data?.domainId ?? 0,
          flowId ?? null
        ],
        (errors: any, res: any) => {
          if (errors) {
            console.log(errors);
            reject([]);
          } else {
            if (res[0]?.length > 0) {
              resolve(res[0]);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (error) {
      logger.error('updateNextNodeInsta err:', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateCustomerContactInsta = (data: any, datas: any, number: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_UpdateCustomerContactInstagram(?,?,?,?,?,?,?)',
        [
          data?.customerId,
          datas?.domainId,
          number ?? null,
          null,
          null,
          null,
          null
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
          }
        }
      );
    } catch (err) {
      logger.error('updateNextNodeInsta err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertCustomerContactEmailInsta = (data: any, datas: any, emailId: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_UpdateCustomerContactInstagram(?,?,?,?,?,?,?)',
        [
          data?.customerId,
          datas?.domainId,
          null,
          emailId ?? null,
          null,
          null,
          null
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
          }
        }
      );
    } catch (err) {
      logger.error('insertCustomerContactEmailInsta err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateCustomerNameInsta = (data: any, datas: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_UpdateCustomerContactInstagram(?,?,?,?,?,?,?)',
        [
          data?.customerId,
          datas?.domainId,
          null,
          null,
          data?.text?.body,
          null,
          null,
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
          }
        }
      );
    } catch (err) {
      logger.error('updateCustomerNameInsta err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateCustomerLocationInsta = (data: any, datas: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_UpdateCustomerContactInstagram(?,?,?,?,?,?,?)',
        [
          data?.customerId,
          datas?.domainId,
          null,
          null,
          null,
          null,
          data?.text?.body,
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
          }
        }
      );
    } catch (err) {
      logger.error('updateCustomerLocationInsta err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCustomerEmailIdInsta = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetCustomerPrimaryEmailForInstagram(?,?)', [
          data?.customerInstagramId ?? null,
          data?.domainId ?? 0
        ],
        (errors: any, res: any) => {
          if (errors) {
            console.log(errors);
            reject([]);
          } else {
            if (res[0]?.length > 0) {
              resolve(res[0][0]);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (error) {
      logger.error('getCustomerEmailIdInsta err:', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getAdminEmail = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_getAdminDetailsByDomainId(?)', [
          data?.domainId ?? 0
        ],
        (errors: any, res: any) => {
          if (errors) {
            console.log(errors);
            reject([]);
          } else {
            if (res[0]?.length > 0) {
              resolve(res[0][0]);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (error) {
      logger.error('getAdminEmail err:', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getExistingCustomerContactInsta = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_get_customer_instagram_existing_status(?,?)', [
          data?.customerInstagramId ?? null,
          data?.domainId ?? 0
        ],
        (errors: any, res: any) => {
          if (errors) {
            console.log(errors);
            reject([]);
          } else {
            if (res[0]?.length > 0) {
              resolve(res[0][0]);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (error) {
      logger.error('getExistingCustomerContactInsta err:', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @param data1
 * @returns
 */
export const updateDeleteInstagramIn = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_DeleteInstagramMessage(?,?,?,?,?)',
        [
          data?.domainId ?? null,
          data?.customerId ?? null,
          data?.pageId ?? null,
          data?.instagramHistoryId ?? null,
          data?.id ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
          }
        }
      );
    } catch (err) {
      logger.error('updateDeleteInstagramIn err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertCustomerContactInsta = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_InsertCustomerContactInstagram(?,?)',
        [
          data?.customerId,
          data?.pageId
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
          }
        }
      );
    } catch (err) {
      logger.error('insertCustomerContactInsta err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param HistoryId
 * @returns
 */
export const updateIsBotInsta = (HistoryId: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_update_flow_bot_check_insta(?)',
        [
          HistoryId
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      logger.error('updateIsBotInsta err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param domainId
 * @returns
 */
export const updateCallbackStatusInsta = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_update_instagram_callback_status(?,?)',
        [
          data?.domainId,
          data?.InstagramHistoryId

        ],
        (errors: any, res: any) => {
          if (errors) {
            console.log(errors);
            reject([]);
          } else {
            if (res[0]?.length > 0) {
              resolve(res[0]);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (error) {
      logger.error('updateCallbackStatusInsta err:', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateMissedChatInsta = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_update_socialMedia_missed_dtl(?,?,?)',
        [
          data?.domainId,
          data?.InstagramHistoryId,
          channelType?.Instagram
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      logger.error('updateMissedChatInsta err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCustomerCoIdInsta = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_coid_by_customerId(?,?,?)',
        [
          data?.customerInstagramId,
          data?.domainId,
          channelType?.Instagram
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      logger.error('getCustomerCoIdInsta err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getBotMessageResInsta = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_get_instagram_message_flow_break_bot(?,?)',
        [
          data?.InstagramHistoryId,
          data?.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      logger.error('getBotMessageResInsta err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getQuestionsAnsInsta = (data: any, datas: any, que: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_get_question_ans(?,?,?)',
        [data?.domainId, datas?.InstagramFlowId, que],
        (err: any, results: any) => {
          if (err) {
            logger.error('getQuestionsAns sp err', err);
            return reject(err);
          }
          const rows = results?.[0] || [];
          resolve(rows.length > 0 ? rows : null);
        }
      );
    } catch (error) {
      logger.error('getQuestionsAns catch err', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertBotMessageInsta = (data: any, message: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_insert_instagram_message_flow_break_bot(?,?,?,?)',
        [
          data?.InstagramHistoryId,
          data?.domainId,
          message ?? null,
          data?.customerInstagramId
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      logger.error('sendFlowBreakInputInsta err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getNoOfRepeatsResInsta = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL up_get_noOfRepeats_InstagramHistory(?,?)',
        [
          data?.domainId,
          data?.InstagramHistoryId
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0][0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      logger.error('getNoOfRepeatsResInsta err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateNoOfRepeatsInsta = (data: any, noOfRepeat: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL up_update_noOfRepeats_InstagramHistory(?,?,?)',
        [
          data?.domainId,
          data?.InstagramFlowId,
          noOfRepeat
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      logger.error('updateNoOfRepeatsInsta err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateConnectDataInsta = (jsonData: any, data: any, flowId: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaasInsertUpdateSocialMediaConnectors(?,?,?,?,?,?)',
        [
          data?.InstagramHistoryId ?? null,
          flowId[0]?.InstagramFlowId ?? null,
          data?.customerInstagramId ?? null,
          data?.domainId ?? 0,
          2003007,
          jsonData ?? null
        ],
        (errors: any, res: any) => {
          if (errors) {
            console.log(errors);
            reject([]);
          } else {
            if (res[0]?.length > 0) {
              resolve(res[0][0]);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (error) {
      logger.error('updateConnectDataInsta err:', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateQueueIdInsta = (data: any, queueId: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL up_UpdateInstagramQueueidBasedOnDomainId(?,?)',
        [
          data?.InstagramHistoryId,
          queueId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      logger.error('updateQueueIdInsta err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertInstagramCallbackI = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_insert_Instagram_callback_status(?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.sessionId,
          data?.IsInBound,
          data?.customerInstagramId,
          data?.agentInstagramId,
          data?.InstagramHistoryId
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      logger.error('insertInstagramCallbackI err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertUpdateCustomerFeedbackDaoInsta = (value: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call  up_insertUpdateCustomerFeedback(?,?,?,?,?,?,?,?,?,?,?)',
        [
          value?.customerFeedbackId ?? null,
          value?.historyId ?? null,
          value?.channelTypeId ?? null,
          value?.satisfactionRate ?? 0,
          value?.domainId,
          value?.feedbackQuestionAnswer ?? null,
          value?.AgentId ?? null,
          value?.ratingType ?? null,
          value?.sessionId ?? null,
          value?.customerNumber ?? null,
          value?.videoMeetId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          } else {
            if (result?.length > 0) {
              resolve(result[0]);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (err) {
      logger.error('insertUpdateCustomerFeedbackDaoInsta err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCustomerSatisficationMsgInsta = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call  up_ccaas_GetCustomerSatisfactionForChannel(?,?,?,?)',
        [
          data?.domainId,
          channelTypeCode?.Instagram,
          data?.InstagramHistoryId,
          data?.videoMeetId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            console.log('err:', err);
            reject(err);
          } else {
            if (result?.length > 0) {
              resolve(result[0]);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (err) {
      logger.error('getCustomerSatisficationMsgInsta err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCustomerSatisficationMsgInstagram = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call  up_ccaas_GetCustomerSatisfactionForChannel(?,?,?,?)',
        [
          data?.domainId,
          channelTypeCode?.Instagram,
          data?.historyId,
          data?.videoMeetId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            console.log('err:', err);
            reject(err);
          } else {
            if (result?.length > 0) {
              resolve(result[0]);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (err) {
      logger.error('getCustomerSatisficationMsgInstagram err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getconnectData = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaasGetSocialMediaConnectors(?,?,?)',
        [
          data?.InstagramHistoryId ?? null,
          data?.domainId ?? 0,
          channelTypeCode?.Instagram,
        ],
        (errors: any, res: any) => {
          if (errors) {
            console.log(errors);
            reject([]);
          } else {
            if (res[0]?.length > 0) {
              resolve(res[0][0]);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (error) {
      logger.error('getconnectData err:', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @param coid
 * @returns
 */
export const updateCustomerIdInsta = (data: any, id: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_updateCustomer_Id(?,?,?)',
        [
          id?.coid,
          data?.InstagramHistoryId,
          'Instagram'
        ],
        (err: any, result: any) => {
          if (err) {
            resolve(err);
          } else {
            if (result && result.length > 0) {
              resolve(result[0]);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (err) {
      logger.error('updateCustomerIdInsta err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @param queueDetails
 * @returns
 */
export const insertUpdateQueueWaitTimeInsta = (data: any, queueDetails: any, datas: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_queue_Weigh_Time_details(?,?,?,?,?)',
        [
          data?.qid,
          data?.queueName,
          data?.queueType,
          data?.domainId ?? 0,
          data?.queueList
        ],
        async (err: any, result: any) => {
          if (err) {
            reject(err);
          }
          let repMessage: any;
          const waitingPosition = JSON.parse(data?.queueList);
          const message = JSON.parse(queueDetails?.announcement)?.textToSpeech;
          repMessage = message?.replace('$waiting_position$', waitingPosition?.length);
          await sendMessageCustomerInstagram(datas, repMessage);
          sendMessageCustomerCountInsta(datas, repMessage);
          resolve(result);
        }
      );
    } catch (error) {
      logger.error('insertUpdateQueueWaitTimeInsta err:', error);
      reject(error);
    }
  });
};

/**
 * @description get queue wait time
 * @param data
 * @returns
 */

export const getQueueWaitTimeInsta = (data: { qid: number; domainId: number }) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_get_queue_wait_time(?,?)',
        [data.qid, data.domainId],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          } else if (result && result[0]?.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      logger.error('getQueueWaitTime err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateAppointmentUserDetailsInsta = (data: any, appointmentId: any, companyId: any, userId: any, getUserData: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_insert_update_appointment_flow_tracking_info(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          channelType?.Instagram,
          data?.InstagramHistoryId,
          data?.domainId,
          data?.agentInstagramId ?? null,
          null,
          data.processflow ?? null,
          companyId ?? null,
          appointmentId ?? null,
          userId ?? null,
          data?.serviceId ?? null,
          data?.processDate ?? null,
          data?.processDuration ?? null,
          data?.processTime ?? null,
          getUserData?.user_id ?? null,
          data?.categoryId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0][0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      logger.error('updateAppointmentUserDetailsInsta err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertFlowTrackingInsta = (data: any, processflow: any, datas: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_insert_update_appointment_flow_tracking_info(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          channelType?.Instagram,
          data?.InstagramHistoryId,
          data?.domainId,
          data?.agentInstagramId ?? null,
          null,
          processflow ?? null,
          datas?.companyId ?? null,
          datas?.appointmentId ?? null,
          datas?.userId ?? null,
          data?.serviceId ?? null,
          data?.date ?? null,
          data?.duration ?? null,
          data?.processTime ?? null,
          data?.consultant ?? null,
          data?.categoryId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0][0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      logger.error('insertFlowTrackingInsta err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateAppointmentUserCreateByInsta = (data: any, appointmentId: any, companyId: any, userId: any, getUserData: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_insert_update_appointment_flow_tracking_info(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          channelType?.Instagram,
          data?.InstagramHistoryId,
          data?.domainId,
          data?.agentInstagramId ?? null,
          null,
          data.processflow ?? null,
          companyId ?? null,
          appointmentId ?? null,
          userId ?? null,
          data?.serviceId ?? null,
          data?.processDate ?? null,
          data?.processDuration ?? null,
          data?.processTime ?? null,
          getUserData?.createby ?? null,
          data?.categoryId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0][0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      logger.error('updateAppointmentUserCreateByInsta err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getFlowTrackingInsta = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_get_appointment_flow_tracking_info(?,?,?)',
        [
          channelType?.Instagram,
          data?.InstagramHistoryId,
          data?.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0][0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      logger.error('getFlowTrackingInsta err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateCategoryIdInsta = (data: any, datas: any, date: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_insert_update_appointment_flow_tracking_info(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          channelType?.Instagram,
          data?.InstagramHistoryId,
          data?.domainId,
          data?.agentInstagramId ?? null,
          null,
          data.processflow ?? null,
          datas?.companyId ?? null,
          datas?.appointmentId ?? null,
          datas?.userId ?? null,
          date?.serviceId ?? null,
          date?.processDate ?? null,
          date?.processDuration ?? null,
          data?.processTime ?? null,
          data?.consultant ?? null,
          date
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0][0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      logger.error('updateCategoryIdInsta err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateAppointmentServiceIdInsta = (data: any, datas: any, date: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_insert_update_appointment_flow_tracking_info(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          channelType?.Instagram,
          data?.InstagramHistoryId,
          data?.domainId,
          data?.agentInstagramId ?? null,
          null,
          data.processflow ?? null,
          datas?.companyId ?? null,
          datas?.appointmentId ?? null,
          datas?.userId ?? null,
          date ?? null,
          data?.title ?? null,
          data?.duration ?? null,
          data?.processTime ?? null,
          data?.consultant ?? null,
          data?.categoryId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0][0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      logger.error('updateAppointmentServiceIdInsta err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateAppointmentConsultantInsta = (data: any, datas: any, date: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_insert_update_appointment_flow_tracking_info(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          channelType?.Instagram,
          data?.InstagramHistoryId,
          data?.domainId,
          data?.agentInstagramId ?? null,
          null,
          data.processflow ?? null,
          datas?.companyId ?? null,
          datas?.appointmentId ?? null,
          datas?.userId ?? null,
          date?.serviceId ?? null,
          data?.processDate ?? null,
          data?.processDuration ?? null,
          date?.processTime ?? null,
          date?.user_id ?? null,
          data?.categoryId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0][0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      logger.error('updateAppointmentConsultantInsta err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateAppointmentConsultantIdInsta = (data: any, datas: any, date: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_insert_update_appointment_flow_tracking_info(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          channelType?.Instagram,
          data?.InstagramHistoryId,
          data?.domainId,
          data?.agentInstagramId ?? null,
          null,
          data.processflow ?? null,
          datas?.companyId ?? null,
          datas?.appointmentId ?? null,
          datas?.userId ?? null,
          date?.serviceId ?? null,
          data?.processDate ?? null,
          data?.processDuration ?? null,
          date?.processTime ?? null,
          date,
          data?.categoryId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0][0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      logger.error('updateAppointmentConsultantIdInsta err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateAppointmentdateInsta = (data: any, datas: any, date: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_insert_update_appointment_flow_tracking_info(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          channelType?.Instagram,
          data?.InstagramHistoryId,
          data?.domainId,
          data?.agentInstagramId ?? null,
          null,
          data.processflow ?? null,
          datas?.companyId ?? null,
          datas?.appointmentId ?? null,
          datas?.userId ?? null,
          date?.serviceId ?? null,
          date,
          data?.duration ?? null,
          data.processTime ?? null,
          data?.consultant ?? null,
          data?.categoryId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0][0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      logger.error('updateAppointmentdateInsta err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateAppointmentDurationInsta = (data: any, datas: any, date: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_insert_update_appointment_flow_tracking_info(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          channelType?.Instagram,
          data?.InstagramHistoryId,
          data?.domainId,
          data?.agentInstagramId ?? null,
          null,
          data.processflow ?? null,
          datas?.companyId ?? null,
          datas?.appointmentId ?? null,
          datas?.userId ?? null,
          date?.serviceId ?? null,
          date?.processDate ?? null,
          date?.title?.replace(/\s(mins|hours)/, '') ?? null,
          data?.processTime ?? null,
          data?.consultant ?? null,
          data?.categoryId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0][0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      logger.error('updateAppointmentdateInsta err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateAppointmentTimeInsta = (data: any, datas: any, date: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_insert_update_appointment_flow_tracking_info(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          channelType?.Instagram,
          data?.InstagramHistoryId,
          data?.domainId,
          data?.agentInstagramId ?? null,
          null,
          data.processflow ?? null,
          datas?.companyId ?? null,
          datas?.appointmentId ?? null,
          datas?.userId ?? null,
          date?.serviceId ?? null,
          data?.processDate ?? null,
          data?.processDuration ?? null,
          date?.title ?? null,
          data?.consultant ?? null,
          data?.categoryId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0][0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      logger.error('updateAppointmentTimeInsta err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updatePhoneNumberInsta = (data: any, datas: any) => {
  const number = datas?.body?.replace(/\D/g, '');
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_insert_update_appointment_flow_tracking_info(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          'Instagram',
          data?.InstagramHistoryId,
          data?.domainId,
          data?.agentInstagramId ?? null,
          number,
          null,
          null,
          null,
          null,
          null,
          null,
          null,
          null,
          null,
          null
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0][0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      logger.error('updatePhoneNumberInsta err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateEmailIdInstagram = (data: any, datas: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_appointment_update_phoneNumber(?,?,?,?,?)',
        [
          data?.InstagramHistoryId,
          data?.domainId ?? null,
          datas?.body ?? null,
          data?.channelType,
          data?.agentInstagramId
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
          }
        }
      );
    } catch (err) {
      logger.error('updateEmailIdInstagram err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateCustomerFirstNameAppoinmentInsta = (data: any, datas: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_appointment_update_name(?,?,?,?,?,?)',
        [
          data?.InstagramHistoryId,
          data?.domainId ?? null,
          datas?.body ?? null,
          null,
          data?.channelType,
          data?.agentInstagramId
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
          }
        }
      );
    } catch (err) {
      logger.error('updateCustomerFirstNameAppoinmentInsta err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateCustomerLastNameAppoinmentInsta = (data: any, datas: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_appointment_update_name(?,?,?,?,?,?)',
        [
          data?.InstagramHistoryId,
          data?.domainId ?? null,
          null,
          datas?.body ?? null,
          data?.channelType,
          data?.agentInstagramId
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
          }
        }
      );
    } catch (err) {
      logger.error('updateCustomerLastNameAppoinmentInsta err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertBotMessageRemainderInsta = (data: any, message: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      const messageData = { message };
      mysqlPoolConnection.query(
        'CALL update_and_insert_message_all_channel_bot(?,?,?,?,?,?)',
        [
          data?.InstagramHistoryId,
          channelType?.Instagram,
          data?.domainId,
          JSON.stringify(messageData) ?? null,
          data?.customerInstagramId,
          data?.isReminder ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      logger.error('insertBotMessageRemainderInsta err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertBotMessageIsRemainderInsta = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL update_and_insert_message_all_channel_bot(?,?,?,?,?,?)',
        [
          data?.InstagramHistoryId,
          channelType?.Instagram,
          data?.domainId,
          null,
          data?.customerInstagramId,
          1
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      logger.error('insertBotMessageIsRemainderInsta err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getRemaindeResponse = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL get_message_all_channel_bot (?,?,?)',
        [
          data?.InstagramHistoryId,
          channelType?.Instagram,
          data?.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      logger.error('getRemaindeResponse err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateEmailIdInsta = (data: any, datas: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_appointment_update_emailId(?,?,?,?,?)',
        [
          data?.InstagramHistoryId,
          data?.domainId ?? null,
          datas?.body ?? null,
          data?.channelType,
          data?.agentInstagramId
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
          }
        }
      );
    } catch (err) {
      logger.error('updateEmailIdInsta err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCompanyDetails = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetCompanyDomain(?)',
        [
          data?.domainId ?? 0
        ],
        (errors: any, res: any) => {
          if (errors) {
            console.log(errors);
            reject([]);
          } else {
            if (res[0]?.length > 0) {
              resolve(res[0][0]);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (error) {
      logger.error('getCompanyDetails err', error);
      reject(error);
    }
  });
};

export const updateCallbackstatus = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const historyId = data?.channelType === 'Facebook' ? data?.FacebookHistoryId : data?.InstagramHistoryId;
      mysqlPoolConnection.query(
        'call ccaas_update_customer_callBack_status(?,?,?)',
        [
          data?.domainId,
          data?.channelType,
          historyId,
        ],
        (errors: any, res: any) => {
          if (errors) {
            console.log(errors);
            reject([]);
          } else {
            if (res[0]?.length > 0) {
              resolve(res[0]);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (error) {
      logger.error('getCompanyDetails err', error);
      reject(error);
    }
  });
};

export async function getInstaMediaUrl(data: any) {
  try {
    const body = {
      customerId: data?.customerId,
      mediaUrl: data?.media?.url,
    };
    console.log('send media url', body);
    const response: any = await axios.post(process.env.getInstaMedia, body);
    console.log('response media url', response?.data?.response);
    return response?.data?.response || null;
  } catch (error: any) {
    console.error('media error:', error?.response?.data || error.message);
    return null;
  }
}
