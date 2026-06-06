import axios from 'axios';
import { v4 as uuidv4 } from 'uuid';
import { logger } from '../../src/plugins/log';
import { channelType, channelTypeCode, messageValue } from '../helpers/constants';
import { executeMySQLWithRetry } from '../helpers/dbRetryUtil';
import { sendMessageCustomer, sendMessageSatisfication } from '../helpers/fbmeta';
import { sendMessageCustomerCount } from '../modules/facebook/handler';
import { mysqlPoolConnection } from '../plugins/db';

/**
 *
 * @param data
 * @returns
 */
export const facebookFeedbackUpdateDB = async (data: any) => {
  try {
    const value: any = {
      historyId: data?.text?.body?.split('-')[1],
      satisfactionRate: data?.text?.body?.split('-')[0] ?? 0,
      channelTypeId: channelTypeCode?.Facebook,
      domainId: data?.text?.body?.split('-')[2],
      sessionId: data?.sessionId ?? null,
      AgentId: data?.text?.body?.split('-')[3],
      ratingDesignType: data?.text?.body?.split('-')[4],
    };
    insertUpdateCustomerFeedbackDao(value);
    const getFeedbackMessage = await getCustomerSatisficationMsg(value);
    await sendMessageSatisfication(data, getFeedbackMessage, value?.domainId);
    return;
  } catch (err) {
    logger.error('sendAppoinmentPaymentInsta err:', err);
    return;
  }
};

/**
 * @description facebook insert
 * @param data
 * @returns
 */
export const insertFacebookIn = async (data: any) => {
  let jsonValue: any = {};
  let mediaUrl: any = '';
  if (['audio'].includes(data?.type)) {
    mediaUrl = await getfbMediaUrl(data);
  }
  if (data?.IsBot === 1 && data?.IsBot !== undefined) {
    jsonValue = { ...data.message };
  } else {
    jsonValue = {
      userText: {
        message: {
          pageId: data?.pageId,
          customerId: data?.customerId,
          isBot: false,
          pdfurl: '',
          sendAt: Date.now(),
          sender: {},
          message: data?.type === 'quickReply' ? data?.text?.title : data?.text?.body || data?.text || '',
          fileType: data?.type ?? 'text',
          messageID: data?.id ?? uuidv4(),
          messageId: data?.messageId,
          sessionId: data?.sessionId,
          isCustomer: true,
          messageType: 'Facebook',
          attachments: data?.type === 'image'
            ? [{
              href: data?.media?.url,
              type: data?.type,
              size: '',
              filename: '',
              fileType: ['jpeg', 'jpg', 'gif', 'svg', 'png', 'tiff']
                .find(val => data?.media?.url.includes(val)) ? `image/${['jpeg', 'jpg', 'gif', 'svg', 'png', 'tiff'].find(val => data?.media?.url.includes(val))}` : ''
            }]
            : data?.type === 'file'
              ? [{
                href: data?.media?.url,
                type: data?.type,
                size: '',
                filename: '',
                fileType: ['pdf', 'doc', 'xls', 'xlsx', 'txt']
                  .find(val => data?.media?.url.includes(val)) ? `file/${['pdf', 'doc', 'xls', 'xlsx', 'txt', 'csv'].find(val => data?.media?.url.includes(val))}` : ''
              }]
              : data?.type === 'video'
                ? [{
                  href: data?.media?.url,
                  type: data?.type,
                  size: '',
                  filename: '',
                  fileType: ['mp4', 'wmv', 'mov', 'flv', 'mkv']
                    .find(val => data?.media?.url.includes(val)) ? `video/${['mp4', 'wmv', 'mov', 'flv', 'mkv'].find(val => data?.media?.url.includes(val))}` : ''
                }]
                : data?.type === 'audio'
                  ? [{
                    href: mediaUrl?.previewUrl,
                    type: data?.type,
                    size: '',
                    filename: mediaUrl?.fileName,
                    fileType: ['m4a', 'flac', 'mp3', 'mp4', 'wav', 'ogg']
                      .find(val => data?.media?.url.includes(val)) ? `audio/${['m4a', 'flac', 'mp3', 'mp4', 'wav', 'ogg'].find(val => data?.media?.url.includes(val))}` : ''
                  }]
                  : null
        },
      },
    };
  }
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL up_InsertUpdateFacebook(?,?,?,?,?,?,?,?,?,?,?,?,?)',
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
      logger.error('insertFacebookIn err:', err);
      reject(err);
    }
  });
};

/**
 * @description send Caas Facebook
 * @param data
 * @returns
 */
export const sendCaasFacebook = (data: any, sessionId: any) => {
  const message = JSON.stringify(data?.message);
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL up_InsertUpdateFacebook(?,?,?,?,?,?,?,?,?,?,?,?,?)',
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
      logger.error('sendCaasFacebook err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateDispositionFacebook = async (data: any) => {
  try {
    const result = await executeMySQLWithRetry(
      mysqlPoolConnection,
      'call up_UpdateFacebookHistoryDisposition(?,?,?,?,?,?,?,?,?,?)',
      [
        data?.FacebookHistoryId,
        data?.domainId,
        data?.dispositionId ?? null,
        data?.summary ?? null,
        data?.isSessionClosed ?? 0,
        data?.afterFacebookWorkTime ?? null,
        data?.dispositionUpdatedBy ?? null,
        data?.duration ?? null,
        data?.dispositionSubject ?? null,
        data?.tags,
      ],
      {
        operationName: 'UpdateFacebookDisposition',
        logContext: { FacebookHistoryId: data?.FacebookHistoryId, domainId: data?.domainId }
      }
    );

    if (result && result.length > 0) {
      return result;
    }
    throw new Error('No result returned from disposition update');

  } catch (err) {
    logger.error('updateDispositionFacebook err:', err);
    throw err;
  }
};

/**
 *
 * @param data
 * @returns
 */
export const assignCallBackFacebook = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const sessionId: any = data.Istransferred === 1 ? uuidv4() : null;
      mysqlPoolConnection.query(
        'call up_UpdateAssignAgentToFacebook(?,?,?,?,?,?)',
        [
          data?.assignedBy ?? null,
          data?.assignedTo ?? null,
          data?.assignedOn ?? null,
          data?.FacebookHistoryId ?? null,
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
    } catch (error) {
      logger.error('assignCallBackFacebook err:', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertUpdateFacebookConfig = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_InsertUpdateFacebookConfiguration(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.facebookConfigurationId ?? null,
          data?.pageName ?? null,
          data?.language ?? null,
          data?.domainId ?? null,
          data?.planId ?? null,
          data?.isBot ?? 0,
          data?.isBotPurchased ?? null,
          data?.faceBookFlowId ?? null,
          data?.faceBookPageId ?? null,
          data?.isActive ?? null,
          data?.facebookintegrationId,
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
      logger.error('insertUpdateFacebookConfig err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getFacebookConfig = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetFacebookConfigurationList(?)',
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
      logger.error('getFacebookConfig err:', err);
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
export const agentDetailsApi = (sessionId: any, domainId: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccass_getDetailsBySessionId(?,?,?)',
        [domainId, channelType?.Facebook, sessionId],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log('agentDetailsApi reject err: ', err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
          }
        }
      );
    } catch (err) {
      logger.error('agentDetailsApi err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param UserId
 * @returns
 */
export const getAssingToApi = (UserId: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetUserExt(?)',
        [UserId],
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
      logger.error('getAssingToApi err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param FacebookIds
 * @param domainId
 * @returns
 */
export const getPersonalDetails = (FacebookIds: any, domainId: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetPersonalDetailsForFacebook(?,?)',
        [FacebookIds, domainId],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log('getPersonalDetails reject err: ', err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
          }
        }
      );
    } catch (err) {
      logger.error('getPersonalDetails err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param domainId
 * @returns
 */
export const getSupervisiorDetails = (domainId: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_supervisor_ext(?)',
        [domainId],
        (err: any, result: any) => {
          if (err) {
            console.error(err);
            reject(err);
          } else {
            if (result && result.length > 0) {
              resolve(result[0]);
            } else {
              reject(err);
            }
          }
        }
      );
    } catch (error) {
      logger.error('getSupervisiorDetails err:', error);
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
export const getBotDetail = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_GetFacebookBot(?,?)',
        [
          data?.agentFacebookId,
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
      logger.error('getBotDetail err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getSupervisiorList = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_supervisor_ext(?)',
        [
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
      logger.error('getSupervisiorList err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getDispositinStatus = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_getDispositinStatus_info(?)',
        [
          data?.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      logger.error('getSupervisiorList err:', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateSentimentScore = (data: any, sentiment: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_updateSentimentalScore(?,?,?,?,?,?,?,?)',
        [
          data?.FacebookHistoryId ?? 0,
          data?.ChatDurationHistoryId ?? 0,
          channelType?.Facebook,
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
      logger.error('updateSentimentScore - Unexpected Error:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateSentimentScoreBot = (data: any, sentiment: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_updateSentimentalScore(?,?,?,?,?,?,?,?)',
        [
          data?.FacebookHistoryId ?? 0,
          data?.ChatDurationHistoryId ?? 0,
          channelType?.Facebook,
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
      logger.error('updateSentimentScoreBot err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param domainId
 * @returns
 */
export const getKnowledgeBaseId = (domainId: any, knowledgeBaseId: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call get_detailsWebsiteScraper(?,?)',
        [
          domainId,
          knowledgeBaseId
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
      logger.error('getKnowledgeBaseId err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const autoAssignAgent = (data: any, presenceStatus: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_AutoAssignAgentToFacebook(?,?,?)',
        [
          data?.FacebookHistoryId ?? 0,
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
      logger.error('autoAssignAgent err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getFacebookConfigDetails = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetFacebookConfigurationDetails(?,?)',
        [
          data?.fbId ?? null,
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
      logger.error('getFacebookConfigDetails err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const deleteFacebookConfigDetails = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_DeleteFacebookConfiguration(?,?)',
        [
          data?.fbConfigId,
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
      logger.error('deleteFacebookConfigDetails err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateStatusFacebook = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_SetFacebookConfigurationStatus(?,?,?)',
        [
          data?.fbConfigId,
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
      logger.error('updateStatusFacebook err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updatePageIdFacebook = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_Update_FacebookPageId(?,?,?)',
        [
          data?.domainId,
          data?.facebookPageId,
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
      logger.error('updatePageIdFacebook err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getFacebookConfigData = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_CheckFacebookBot (?,?)',
        [
          data?.agentFacebookId ?? null,
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
      logger.error('getFacebookConfigData err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param sid
 * @returns
 */
export const getnodesdata = (cfid: number) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_get_nodes_data_by_chatFlowId(?)',
        [cfid],
        (err: any, result: any) => {
          if (err) {
            logger.error('getnodesdata sp err', err);
            return reject(err);
          }
          if (result[0]?.length > 0) {
            resolve(result[0][0]?.nodes ?? null);
          } else {
            resolve(null);
          }
        }
      );
    } catch (error) {
      logger.error('getnodesdata catch err', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getNextNode = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetNextFlowNodeForFacebook(?,?)', [
          data?.FacebookHistoryId ?? null,
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
      logger.error('getNextNode err:', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateNextNode = (data: any, flowId: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_UpdateNextFlowNodeForFacebook(?,?,?)', [
          data?.FacebookHistoryId ?? null,
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
      logger.error('updateNextNode err:', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateChatMessage = (data: any) => {
  const message: any = JSON.stringify(data.message);
  const chat_id = parseInt(data?.FacebookHistoryId, 10);
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_InserUpdateFacebookMessage(?,?)',
        [chat_id ?? 0, message],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          } else {
            resolve(result);
          }
        }
      );
    } catch (error) {
      logger.error('updateChatMessage err:', error);
      console.log(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertCustomerContactFB = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_InsertCustomerContactFacebook(?,?)',
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
      logger.error('insertCustomerContactFB err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateCustomerContactFB = (data: any, datas: any, number: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_UpdateCustomerContactFacebook(?,?,?,?,?,?,?)',
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
      logger.error('updateCustomerContactFB err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertCustomerContactEmail = (data: any, datas: any, emailId: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_UpdateCustomerContactFacebook(?,?,?,?,?,?,?)',
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
      logger.error('insertCustomerContactEmail err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateCustomerName = (data: any, datas: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_UpdateCustomerContactFacebook(?,?,?,?,?,?,?)',
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
      logger.error('updateCustomerName err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateCustomerLocation = (data: any, datas: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_UpdateCustomerContactFacebook(?,?,?,?,?,?,?)',
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
      logger.error('updateCustomerLocation err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCustomerEmailId = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetCustomerPrimaryEmailForFacebook(?,?)', [
          data?.customerFacebookId ?? null,
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
      logger.error('getCustomerEmailId err:', error);
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
export const getExistingCustomerContact = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_get_customer_facebook_existing_status(?,?)', [
          data?.customerFacebookId ?? null,
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
      logger.error('getExistingCustomerContact err:', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateDispositionDiscnnect = async (data: any, closedDispositionsId: any) => {
  try {
    const result = await executeMySQLWithRetry(
      mysqlPoolConnection,
      'call up_UpdateFacebookHistoryDisposition(?,?,?,?,?,?,?,?,?)',
      [
        data?.FacebookHistoryId,
        data?.domainId,
        closedDispositionsId?.did ?? null,
        closedDispositionsId?.dispositionName ?? null,
        1,
        data?.afterFacebookWorkTime ?? null,
        data?.dispositionUpdatedBy ?? null,
        data?.duration ?? null,
        data?.dispositionSubject ?? null,
      ],
      {
        operationName: 'UpdateFacebookDispositionOnDisconnect',
        logContext: { FacebookHistoryId: data?.FacebookHistoryId, domainId: data?.domainId }
      }
    );

    if (result && result.length > 0) {
      return result[0];
    }
    throw new Error('No result returned from disposition update');

  } catch (err) {
    logger.error('updateDispositionDiscnnect err:', err);
    throw err;
  }
};

// senti mental Score
export async function sentimentalScoreData(data: any) {
  try {
    const body = {
      HistoryId: data?.FacebookHistoryId,
      ChatDurationHistoryId: data?.ChatDurationHistoryId ?? 0,
      channelType: data?.channelType,
      domainId: data?.domainId,
      ext: parseInt(data?.ext, 10) ?? 0
    };
    return await axios.post(process?.env?.Sentimental, body);
  } catch (error) {
    logger.error('sentimentalScoreData err:', error);
  }
}

/**
 *
 * @param data
 * @returns
 */
export const updateDispositionBot = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_FacebookIsBotUpdate(?)',
        [
          data?.FacebookHistoryId
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
      logger.error('sentimentalScoreData err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param userId
 * @param companyId
 * @returns
 */
export const getTransferAgentStatus = (userId: any, data: any) => {
  const channelCode = channelTypeCode[data?.channelType];
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_CheckUserReadyForSocialMedia(?,?,?,?)', [
          userId ?? null,
          data?.domainId ?? 0,
          data?.sessionId ?? null,
          channelCode
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
      logger.error('getTransferAgentStatus err:', error);
      reject(error);
    }
  });
};

// send Button
export async function sendCustomerInput(req: any, buttonData: any) {
  try {
    const bodyContants = buttonData?.message;
    const bodyContantsData = bodyContants?.replace(/<\/*s>/g, '~').replace(/<\/*strong>/g, '*').replace(/<\/*em>/g, '_').replace(/<div>/g, '').replace(/<\/div>|<br>/g, '\n').replace(/<[^>]*>/g, '').replace(/&nbsp;/g, ' ');
    const body = {
      domainId: req?.domainId,
      customerId: req?.customerFacebookId,
      pageId: req?.agentFacebookId,
      postbackText: bodyContantsData,
      buttons: []
    };
    buttonData?.getInputData?.map((val: any) => {
      body?.buttons.push(
        {
          type: 'postback',
          title: val?.value,
          payload: val?.value,
        }
      );
    });
    return await axios.post(process.env.postbackButton, body);
  } catch (error) {
    logger.error('sendCustomerInput err:', error);
  }
}

// send Button
export async function sendButtonLink(req: any, buttonData: any) {
  try {
    const bodyContants = buttonData?.message;
    const bodyContantsData = bodyContants?.replace(/<\/*s>/g, '~').replace(/<\/*strong>/g, '*').replace(/<\/*em>/g, '_').replace(/<div>/g, '').replace(/<\/div>|<br>/g, '\n').replace(/<[^>]*>/g, '').replace(/&nbsp;/g, ' ');
    const body = {
      domainId: req?.domainId,
      customerId: req?.customerFacebookId,
      pageId: req?.agentFacebookId,
      postbackText: bodyContantsData,
      buttons: []
    };
    buttonData?.getInputData?.map((val: any) => {
      body?.buttons.push(
        {
          type: 'web_url',
          url: val?.link,
          title: val?.value,
          webview_height_ratio: 'full'
        }
      );
    });
    return await axios.post(process.env.postbackButton, body);
  } catch (error) {
    logger.error('sendButtonLink err:', error);
  }
}

// send Image Customer
export async function sendImageCustomer(req: any, filesData: any) {
  try {
    const body = {
      domainId: req?.domainId,
      customerId: req?.customerFacebookId,
      pageId: req?.agentFacebookId,
      mediaUrl: filesData?.url,
      mediaType: filesData?.type
    };
    await axios.post(process?.env?.sendMedia, body);
    return body;
  } catch (error) {
    logger.error('sendImageCustomer err:', error);
  }
}

// send Button
export async function sendCarouselData(data: any, carouselData: any) {
  try {
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerFacebookId,
      pageId: data?.agentFacebookId,
      elements: carouselData
    };
    return await axios.post(process.env.sendCarousel, body);
  } catch (error) {
    logger.error('sendCarouselData err:', error);
  }
}

/**
 *
 * @param data
 * @param dataq
 * @returns
 */
export const getUserList = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_getUserListByRoleHandler(?,?)',
        [data?.domainId, '1,2,3,4,5,6'],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          } else {
            if (result && result[0]?.length > 0) {
              resolve(result[0]);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (error) {
      logger.error('getUserList err:', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getConcurrentChat = async (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetConcurrentAgentLimit(?)',
        [
          data
        ],
        (errors: any, res: any) => {
          if (errors) {
            console.log(errors);
            reject([]);
          } else {
            if (res[0]?.length > 0) {
              resolve(res[0][0]);
            } else {
              resolve('null');
            }
          }
        }
      );
    } catch (error: any) {
      logger.error('getConcurrentChat err:', error);
    }
  });
};

/**
 *
 * @param chatId
 * @param domainId
 * @param obj
 * @returns
 */
export const insertStatus = (chatId: any, domainId: any, obj: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      const sp = 'call ccaas_insert_update_user_chat_tracking_log(?,?,?)';
      mysqlPoolConnection.query(sp, [domainId, chatId, JSON.stringify(obj)], (err: any, result: any) => {
        if (err) {
          reject(err);
        } else {
          resolve(JSON.parse(JSON.stringify(result[0])));
        }
      });
    } catch (error) {
      logger.error('insertStatus err:', error);
      reject(error);
    }
  });
};

/**
 *
 * @param domainId
 * @returns
 */
export const getAgentByChat = (domainId: number) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_get_agent_by_chat(?)',
        [domainId],
        (errors: any, res: any) => {
          if (errors) {
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
      logger.error('getAgentByChat err', error);
      reject(error);
    }
  });
};

/**
 *
 * @param domainId
 * @returns
 */
export const getRoutingProfileUser = (domainId: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetUserConcurrentChat(?)',
        [
          domainId
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
      logger.error('getRoutingProfileUser err:', error);
      reject(error);
    }
  });
};

// send Button
export async function sendCallbackRequest(req: any, buttonLink: any) {
  try {
    const body = {
      domainId: req?.domainId,
      customerId: req?.customerFacebookId,
      pageId: req?.agentFacebookId,
      postbackText: messageValue?.agentBusy,
      buttons: [
        {
          type: 'web_url',
          url: `${process.env.API_URL}/${buttonLink}`,
          title: 'callback',
          webview_height_ratio: 'full'
        }
      ]
    };
    return await axios.post(process.env.postbackButton, body);
  } catch (error) {
    logger.error('sendCallbackRequest err:', error);
  }
}

/**
 *
 * @param domainId
 * @returns
 */
export const updateCallbackStatus = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_update_facebook_callback_status(?,?)',
        [
          data?.domainId,
          data?.FacebookHistoryId

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
      logger.error('updateCallbackStatus err:', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateMissedChat = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_update_socialMedia_missed_dtl(?,?,?)',
        [
          data?.domainId,
          data?.FacebookHistoryId,
          'Facebook'
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
      logger.error('updateMissedChat err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertWhatsappCallbackDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_insert_facebook_callback_status(?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.sessionId,
          data?.IsInBound,
          data?.customerFacebookId,
          data?.agentFacebookId,
          data?.FacebookHistoryId
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
      logger.error('insertWhatsappCallbackDao err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCustomerCoId = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_coid_by_customerId(?,?,?)',
        [
          data?.customerFacebookId,
          data?.domainId,
          channelType?.Facebook
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
      logger.error('getCustomerCoId err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param queueId
 * @param domainId
 * @returns
 */
export const getRoutingProfiles = (queueId: any, domainId: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_get_profile_mapping_priority_by_qid(?,?)', [domainId, queueId], (err: any, res: any) => {
        if (err) {
          reject(err);
        } else {
          if (res[0]?.length > 0) {
            resolve(res[0]);
          } else {
            resolve([]);
          }
        }
      });
    } catch (error: any) {
      logger.error('getRoutingProfiles err:', error);
      reject(error);
    }
  });
};

/**
 *
 * @param rqmid
 * @param domainId
 * @returns
 */
export const getRoutingDetails = (rqmid: number, domainId: number) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_get_routing_details(?, ?)',
        [rqmid, domainId],
        (errors: any, res: any) => {
          if (errors) {
            reject([]);
          } else {
            if (res[0]?.length > 0) {
              resolve(res[0][0]);
            } else {
              resolve(null);
            }
          }
        }
      );
    } catch (error) {
      logger.error('getRoutingDetails err', error);
      reject(error);
    }
  });
};

/**
 *
 * @param domainId
 * @param roleId
 * @returns
 */
export const getAgentsSkills = (domainId: any, roleId: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_getUserListByRoleHandler(?,?)',
        [domainId, roleId],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log('getAgentsSkills reject err: ', err);
          } else {
            resolve(JSON.parse(JSON.stringify(result[0])));
          }
        }
      );
    } catch (error) {
      logger.error('getAgentsSkills err:', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getNoOfRepeatsRes = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL up_get_noOfRepeats_FacebookHistory(?,?)',
        [
          data?.domainId,
          data?.FacebookHistoryId
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
      logger.error('getNoOfRepeatsRes err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateNoOfRepeats = (data: any, noOfRepeat: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL up_update_noOfRepeats_FacebookHistory(?,?,?)',
        [
          data?.domainId,
          data?.FacebookHistoryId,
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
      logger.error('updateNoOfRepeats err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateConnectData = (jsonData: any, data: any, flowId: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaasInsertUpdateSocialMediaConnectors(?,?,?,?,?,?)',
        [
          data?.FacebookHistoryId ?? null,
          flowId[0]?.FacebookFlowId ?? null,
          data?.customerFacebookId ?? null,
          data?.domainId ?? 0,
          2003006,
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
      logger.error('updateConnectData err:', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateCustomerFirstName = (data: any, datas: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_UpdateCustomerName(?,?,?,?)', [
          data?.text?.body ?? null,
          null,
          data?.customerId ?? null,
          datas?.domainId ?? 0,
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
      logger.error('updateCustomerFirstName err:', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateCustomerLastName = (data: any, datas: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_UpdateCustomerName(?,?,?,?)', [
          null,
          data?.text?.body ?? null,
          data?.customerId ?? null,
          datas?.domainId ?? 0,
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
      logger.error('updateCustomerLastName err:', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getQuestionsAns = (data: any, datas: any, que: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_get_question_ans(?,?,?)',
        [data?.domainId, datas?.FacebookFlowId, que],
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
export const insertCustomerContact = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_insertUpdateCustomerContact(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.coid ?? 0,
          data?.firstName ?? null,
          data?.lastName ?? null,
          data?.phoneNumber ? JSON.stringify(data.phoneNumber) : '[]',
          data?.email ? JSON.stringify(data.email) : '[]',
          data?.accountNumber ?? null,
          data?.tags ? JSON.stringify(data.tags) : '[]',
          data?.location ?? null,
          data?.address ?? null,
          data?.country ?? null,
          data?.state ?? null,
          data?.city ?? null,
          data?.postalCode ?? null,
          data?.customerId ?? null,
          data?.instagram ?? null,
          data?.whatsapp ?? null,
          data?.twitter ?? null,
          data?.voiceBiometric ?? null,
          data?.companyId,
          data?.mergeCustomer ?? null,
          data?.blockCustomer ?? 0,
          data?.doNotDisturb ?? 0,
          data?.blockCustomerReq ?? 0,
          data?.doNotDisturbReq ?? 0,
          data?.type ?? null,
          data?.currentCustomerNumber ?? null,
          data?.customerType ?? null,
          data?.timeZone ?? null,
          data?.cookiesId ?? null,
          data?.source ?? null,
          data?.customField ?? null,
          data?.customValues ?? null,
          data?.primaryPhoneNumber ?? null,
          data?.primaryEmail ?? null
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
      logger.error('insertCustomerContact err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getBotMessageRes = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_get_facebook_message_flow_break_bot(?,?)',
        [
          data?.FacebookHistoryId,
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
      logger.error('getBotMessageRes err:', err);
      reject(err);
    }
  });
};

// send Button
export async function sendFlowBreakInput(req: any) {
  try {
    const body = {
      domainId: req?.domainId,
      customerId: req?.customerFacebookId,
      pageId: req?.agentFacebookId,
      postbackText: messageValue?.disconnectFlow,
      buttons: [{
        type: 'postback',
        title: 'Yes',
        payload: '100'
      },
        {
          type: 'postback',
          title: 'No',
          payload: '200'
        }]
    };
    return await axios.post(process.env.postbackButton, body);
  } catch (error) {
    logger.error('sendFlowBreakInput err:', error);
  }
}

/**
 *
 * @param data
 * @returns
 */
export const insertBotMessage = (data: any, message: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_insert_facebook_message_flow_break_bot(?,?,?,?)',
        [
          data?.FacebookHistoryId,
          data?.domainId,
          message ?? null,
          data?.customerFacebookId
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
      logger.error('insertBotMessage err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateQueueId = (data: any, queueId: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL up_UpdateFacebookQueueidBasedOnDomainId(?,?)',
        [
          data?.FacebookHistoryId,
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
      logger.error('updateQueueId err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCustomerSatisficationDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call  up_ccaas_GetCustomerSatisfactionForChannel(?,?,?,?)',
        [
          data?.domainId,
          channelTypeCode?.Facebook,
          data?.FacebookHistoryId,
          data?.videoMeetId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
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
      logger.error('getCustomerSatisficationDao err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param HistoryId
 * @returns
 */
export const updateIsBotFb = (HistoryId: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_update_flow_bot_check_fb(?)',
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
      logger.error('updateIsBotFb err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertUpdateCustomerFeedbackDao = (value: any) => {
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
          value?.ratingDesignType ?? null,
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
      logger.error('insertUpdateCustomerFeedbackDao err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCustomerSatisficationMsg = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call  up_ccaas_GetCustomerSatisfactionForChannel(?,?,?,?)',
        [
          data?.domainId,
          channelTypeCode?.Facebook,
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
      logger.error('getCustomerSatisficationMsg err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getconnectDataFB = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaasGetSocialMediaConnectors(?,?,?)',
        [
          data?.FacebookHistoryId ?? null,
          data?.domainId ?? 0,
          channelTypeCode?.Facebook,
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
      logger.error('getconnectDataFB err:', error);
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
export const updateCustomerId = (data: any, id: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_updateCustomer_Id(?,?,?)',
        [
          id?.coid,
          data?.FacebookHistoryId,
          channelType?.Facebook
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
      logger.error('getconnectDataFB err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param qid
 * @param domainId
 * @returns
 */
export const getQueueSettingS = (qid: number, domainId: number) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_get_queue_setting(?,?)',
        [qid, domainId],
        (err: any, result: any) => {
          if (err) {
            console.error('getQueueSettingS SQL Error:', err);
            reject(err);
          } else {
            if (result && result[0]?.length > 0) {
              resolve(result[0][0]);
            } else {
              resolve(null);
            }
          }
        }
      );
    } catch (err) {
      logger.error('getQueueSettingS err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCustomerConatctDetails = (domainId: any, Id: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_getCustomerConatctDetails(?,?)',
        [
          Id?.coid ?? 0,
          domainId
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
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
      logger.error('getQueueSettingS err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param domainId
 * @returns
 */
export const getAllQueueWaitTime = (domainId: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_getAllQueueWaitTime_info(?)',
        [
          domainId
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          } else if (result && result?.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (error) {
      logger.error('getAllQueueWaitTime err:', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @param queueDetails
 * @returns
 */
export const insertUpdateQueueWaitTimeDao = (data: any, queueDetails: any, datas: any) => {
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
          await sendMessageCustomer(datas, repMessage);
          sendMessageCustomerCount(datas, repMessage);
          resolve(result);
        }
      );
    } catch (error) {
      logger.error('insertUpdateQueueWaitTimeDao err:', error);
      reject(error);
    }
  });
};

/**
 * @description get queue wait time
 * @param data
 * @returns
 */
export const getQueueWaitTime = (data: { qid: number; domainId: number }) => {
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
export const getUserExt = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call get_user_by_extension(?)',
        [
          data?.transferTo
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          } else if (result && result?.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (error) {
      logger.error('getUserExt err:', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updatePhoneNumberFB = (data: any, datas: any) => {
  const number = datas?.body?.replace(/\D/g, '');
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_insert_update_appointment_flow_tracking_info(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          'Facebook',
          data?.FacebookHistoryId,
          data?.domainId,
          data?.agentFacebookId ?? null,
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
      logger.error('updatePhoneNumberFB err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateAppointmentUserDetails = (data: any, appointmentId: any, companyId: any, userId: any, getUserData: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_insert_update_appointment_flow_tracking_info(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          channelType?.Facebook,
          data?.FacebookHistoryId,
          data?.domainId,
          data?.agentFacebookId ?? null,
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
      logger.error('updatePhoneNumberFB err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertFlowTracking = (data: any, processflow: any, datas: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_insert_update_appointment_flow_tracking_info(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          channelType?.Facebook,
          data?.FacebookHistoryId,
          data?.domainId,
          data?.agentFacebookId ?? null,
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
      logger.error('updatePhoneNumberFB err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateAppointmentUserCreateBy = (data: any, appointmentId: any, companyId: any, userId: any, getUserData: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_insert_update_appointment_flow_tracking_info(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          channelType?.Facebook,
          data?.FacebookHistoryId,
          data?.domainId,
          data?.agentFacebookId ?? null,
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
      logger.error('updateAppointmentUserCreateBy err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getFlowTracking = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_get_appointment_flow_tracking_info(?,?,?)',
        [
          channelType?.Facebook,
          data?.FacebookHistoryId,
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
      logger.error('getFlowTracking err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateCategoryId = (data: any, datas: any, date: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_insert_update_appointment_flow_tracking_info(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          channelType?.Facebook,
          data?.FacebookHistoryId,
          data?.domainId,
          data?.agentFacebookId ?? null,
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
      logger.error('updateCategoryId err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateAppointmentServiceId = (data: any, datas: any, date: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_insert_update_appointment_flow_tracking_info(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          channelType?.Facebook,
          data?.FacebookHistoryId,
          data?.domainId,
          data?.agentFacebookId ?? null,
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
      logger.error('updateAppointmentServiceId err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateAppointmentConsultant = (data: any, datas: any, date: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_insert_update_appointment_flow_tracking_info(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          channelType?.Facebook,
          data?.FacebookHistoryId,
          data?.domainId,
          data?.agentFacebookId ?? null,
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
      logger.error('updateAppointmentConsultant err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateAppointmentConsultantId = (data: any, datas: any, date: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_insert_update_appointment_flow_tracking_info(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          channelType?.Facebook,
          data?.FacebookHistoryId,
          data?.domainId,
          data?.agentFacebookId ?? null,
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
      logger.error('updateAppointmentConsultant err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateAppointmentdate = (data: any, datas: any, date: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_insert_update_appointment_flow_tracking_info(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          channelType?.Facebook,
          data?.FacebookHistoryId,
          data?.domainId,
          data?.agentFacebookId ?? null,
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
      logger.error('updateAppointmentdate err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateAppointmentDuration = (data: any, datas: any, date: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_insert_update_appointment_flow_tracking_info(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          channelType?.Facebook,
          data?.FacebookHistoryId,
          data?.domainId,
          data?.agentFacebookId ?? null,
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
      logger.error('updateAppointmentDuration err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateAppointmentTime = (data: any, datas: any, date: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_insert_update_appointment_flow_tracking_info(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          channelType?.Facebook,
          data?.FacebookHistoryId,
          data?.domainId,
          data?.agentFacebookId ?? null,
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
      logger.error('updateAppointmentTime err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getRemaindeFbResponse = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL get_message_all_channel_bot (?,?,?)',
        [
          data?.FacebookHistoryId,
          channelType?.Facebook,
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
      logger.error('getRemaindeFbResponse err:', err);
      reject(err);
    }
  });
};

// send Button
export async function sendFlowBreakInpuRemaindertFb(req: any) {
  try {
    const body = {
      domainId: req?.domainId,
      customerId: req?.customerFacebookId,
      pageId: req?.agentFacebookId,
      postbackText: messageValue?.continueFlow,
      buttons: [{
        type: 'postback',
        title: 'Yes',
        payload: '10001'
      },
        {
          type: 'postback',
          title: 'No',
          payload: '20002'
        }]
    };
    return await axios.post(process.env.postbackButton, body);
  } catch (error) {
    logger.error('sendFlowBreakInpuRemaindertFb err:', error);
  }
}

/**
 *
 * @param data
 * @returns
 */
export const insertBotMessageIsRemainderFb = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL update_and_insert_message_all_channel_bot(?,?,?,?,?,?)',
        [
          data?.FacebookHistoryId,
          channelType?.Facebook,
          data?.domainId,
          null,
          data?.customerFacebookId,
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
      logger.error('insertBotMessageIsRemainderFb err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertBotMessageRemainderfacebook = (data: any, message: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      const messageData = { message };
      mysqlPoolConnection.query(
        'CALL update_and_insert_message_all_channel_bot(?,?,?,?,?,?)',
        [
          data?.FacebookHistoryId,
          channelType?.Facebook,
          data?.domainId,
          JSON.stringify(messageData) ?? null,
          data?.customerFacebookId,
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
      logger.error('insertBotMessageRemainderfacebook err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateEmailIdFB = (data: any, datas: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_appointment_update_emailId(?,?,?,?,?)',
        [
          data?.FacebookHistoryId,
          data?.domainId ?? null,
          datas?.body ?? null,
          data?.channelType,
          data?.agentFacebookId
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
      logger.error('updateEmailIdFB err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updatePhNumber = (data: any, datas: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_appointment_update_phoneNumber(?,?,?,?,?)',
        [
          data?.FacebookHistoryId,
          data?.domainId ?? null,
          datas?.body ?? null,
          data?.channelType,
          data?.agentFacebookId
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
      logger.error('updatePhNumber err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateCustomerFirstNameAppoinment = (data: any, datas: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_appointment_update_name(?,?,?,?,?,?)',
        [
          data?.FacebookHistoryId,
          data?.domainId ?? null,
          datas?.body ?? null,
          null,
          data?.channelType,
          data?.agentFacebookId
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
      logger.error('updateCustomerFirstNameAppoinment err:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateCustomerLastNameAppoinment = (data: any, datas: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_appointment_update_name(?,?,?,?,?,?)',
        [
          data?.FacebookHistoryId,
          data?.domainId ?? null,
          null,
          datas?.body ?? null,
          data?.channelType,
          data?.agentFacebookId
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
      logger.error('updateCustomerLastNameAppoinment err:', err);
      reject(err);
    }
  });
};

// send booking details
export async function sendAppoinmentPayment(data: any) {
  try {
    const booking = JSON.parse(data?.p_book_form);
    const formattedLastName = booking[1]['Last name'] ? `*${booking[1]['Last name']}*` : '-';
    const formatNumber = booking[2]['Phone number'] !== null ? `*${booking[2]['Phone number']}*` : '-';
    let content: any = '';
    if (data?.p_app_type === 4) {
      content = `Thank You! Your appointment has been booked successfully.\n\n Appointment ID\n *${data?.app_booked_id}*\n\n Booking date\n *${data?.p_book_date}*\n\n Timing\n *${data?.p_book_starttime}* - *${data?.p_book_endtime}*\n\n Service\n *${data?.service_name}*\n\n Name\n *${booking[0]['First name']}* ${formattedLastName}\n\n Email\n *${booking[3]?.Email}*\n\n Phone\n ${formatNumber}\n\n`;
    } else {
      content = `Thank You! Your appointment has been booked successfully.\n\n Appointment ID\n *${data?.app_booked_id}*\n\n Booking date\n *${data?.p_book_date}*\n\n Timing\n *${data?.p_book_starttime}* - *${data?.p_book_endtime}*\n\n Name\n *${booking[0]['First name']}* ${formattedLastName}\n\n Email\n *${booking[3]?.Email}*\n\n Phone\n ${formatNumber}\n\n`;
    }
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerNumber,
      pageId: data?.calledNumber,
      type: 'text',
      message: content
    };
    await axios.post(process?.env?.FacebookOut, body);
    return body;
  } catch (error) {
    logger.error('sendAppoinmentPayment err:', error);
  }
}

// send Button
export async function sendPayment(data: any, appointmentUrl: any) {
  try {
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerFacebookId,
      pageId: data?.agentFacebookId,
      postbackText: messageValue?.checkHere,
      buttons: [
        {
          type: 'web_url',
          url: appointmentUrl,
          title: 'Pay Now',
          webview_height_ratio: 'full'
        }
      ]
    };
    return await axios.post(process.env.postbackButton, body);
  } catch (error) {
    logger.error('sendPayment err:', error);
  }
}

// send Button
export async function sendPaymentInsta(data: any, appointmentUrl: any) {
  try {
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerInstagramId,
      pageId: data?.agentInstagramId,
      postbackText: messageValue?.checkHere,
      buttons: [
        {
          type: 'web_url',
          url: appointmentUrl,
          title: 'Pay Now',
          webview_height_ratio: 'full'
        }
      ]
    };
    return await axios.post(process.env.postbackButtonInsta, body);
  } catch (error) {
    logger.error('sendPaymentInsta err:', error);
  }
}

// get appointment details
export async function AppointmentInfoPaymentCheckFb(data: any) {
  try {
    const body = {
      p_app_id: data?.appId,
      p_company_id: data?.companyId
    };
    const appointmentData: any = await axios.post(process?.env?.getAppointment, body);
    if (appointmentData?.data?.status_code === 200) {
      return appointmentData?.data?.finalresponse;
    }
    return [];

  } catch (error) {
    logger.error('AppointmentInfoPaymentCheckFb err:', error);
  }
}

// send Typing Indicator
export async function sendTypingIndicator(data: any): Promise<void> {
  return new Promise<void>(async (resolve, reject) => {
    try {
      const body = {
        domainId: data?.domainId,
        customerId: data?.customerFacebookId,
        pageId: data?.agentFacebookId,
        sender_action: 'typing_on'
      };
      const result: any = await axios.post(process?.env?.sendTyping, body);
      logger.info('sendTypingIndicator', result);
      resolve(result);
    } catch (error) {
      logger.error('sendTypingIndicator err:', error);
      reject(error);
    }
  });
}

// Agent Assist
export async function AgentAssist(data: any, comDetails: any, ext: any, message: any) {
  try {
    const body = {
      ext,
      message,
      domainId: data?.domainId,
      channelType: data?.channelType,
      channelId: data?.channelType === 'Facebook' ? data?.agentFacebookId : data?.agentInstagramId,
      sessionId: data?.sessionId,
      websiteId: '',
      type: '',
      company_name: comDetails?.companyName ?? '',
      industry_type: comDetails?.industryType ?? '',
    };
    logger.info('Sending request to assist for bot:', JSON.stringify(body, null, 2));
    const response = await axios.post(process.env.semiChannel, body);
    logger.info('Response from nlp>>>>>>>>>>>', JSON.stringify(response.data, null, 2));
    return response;
  } catch (error) {
    logger.error('AgentAssist err:', error);
  }
}

/**
 *
 * @param data
 * @returns
 */
export const getRemainingSessionEmailSent = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetRemainingSessionForEmailSent(?)', [
          data?.domainId
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
      logger.error('getRemainingSessionEmailSent err:', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @param callback
 */
export const getConnectorTokenInfo = (data: { domainId: number }, callback: (res: any) => void) => {
  const domainId = data?.domainId ?? 0;
  mysqlPoolConnection.query(
    'CALL ccaas_get_connector_token_info(?)',
    [domainId],
    (error: any, results: any) => {
      if (error) {
        logger.error('DB Error in getConnectorTokenInfo:', error);
        callback([]);
        return;
      }
      const rows = results && results[0] ? results[0] : [];
      callback(rows);
    }
  );
};

/**
 *
 * @param data
 * @param dataq
 * @returns
 */
export const getUserListByExt = (data: any, dataq: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_getUserListByExtHandler(?,?)',
        [data?.domainId, dataq?.ext],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          } else {
            if (result && result[0]?.length > 0) {
              resolve(result[0]);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (error) {
      reject(error);
    }
  });
};

export async function getfbMediaUrl(data: any) {
  try {
    const body = {
      customerId: data?.customerId,
      mediaUrl: data?.media?.url,
    };
    console.log('send media url', body);
    const response: any = await axios.post(process.env.getFbMedia, body);
    console.log('response media url', response?.data?.response);
    return response?.data?.response || null;
  } catch (error: any) {
    console.error('media error:', error?.response?.data || error.message);
    return null;
  }
}
