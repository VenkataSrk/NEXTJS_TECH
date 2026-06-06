import axios from 'axios';
import { log } from 'console';
import { v4 as uuidv4 } from 'uuid';
import { logger } from '../.././src/plugins/log';
import { channelType, channelTypeCode } from '../helpers/constants';
import { executeMySQLWithRetry } from '../helpers/dbRetryUtil';
import { getWhatsAppMediaUrl, sendMessageCustomerCai, sendMessageSatisfication } from '../helpers/meta';
import { sendMessageCustomerCount } from '../modules/whatsapp/handler';
import { mysqlPoolConnection } from '../plugins/db';

/**
 *
 * @param data
 * @returns
 */
export const whatsappSendTemplate = async (data: any) => {
  const config: any = {
    method: 'post',
    maxBodyLength: Infinity,
    url: process.env.WHATSAPPURL,
    headers: {
      'Content-Type': 'application/json',
    },
    data: JSON.stringify(data),
  };
  try {
    const response = await axios.request(config);
    logger.info('whatsappSendTemplate response: ', response.data);
    return response.data;
  } catch (error) {
    logger.log('whatsappSendTemplate catch error:', error);
    throw error;
  }
};

export const insertWhatsResponseUser = (data: any) => {
  return new Promise((resolve1, reject) => {
    try {
      mysqlPoolConnection.query(
        'call whatsappIntegrationCreationDtl(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.requestId,
          data?.companyId,
          data?.agentNumber,
          data?.customerNumber,
          data?.enabled,
          data?.status,
          data?.sentTimestamp,
          data?.deliveredTimestamp,
          data?.readTimestamp,
          data?.mediajson,
          data?.msgtype,
          data?.conversation,
          data?.pricing,
          data?.customerName,
          data?.msgtext,
          data?.timestamp ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            logger.info('whatsappIntegrationCreationDtl reject err: ', err);
            reject(err);
          }
          logger.info('whatsappIntegrationCreationDtl result: ');
          resolve1(result);
        }
      );
    } catch (error) {
      logger.error('whatsappIntegrationCreationDtl catch error: ', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const whatsappFeedbackUpdateDB = async (data: any) => {
  try {
    const value: any = {
      historyId: data?.interactive?.list_reply?.id?.split('-')[1],
      satisfactionRate: data?.interactive?.list_reply?.id?.split('-')[0] ?? 0,
      channelTypeId: channelTypeCode?.Whatsapp,
      domainId: data?.companyId,
      sessionId: data?.sessionId ?? null,
      AgentId: data?.interactive?.list_reply?.id?.split('-')[2],
      ratingDesignType: data?.interactive?.list_reply?.id?.split('-')[3]
    };
    insertUpdateCustomerFeedbackDao(value);
    const getFeedbackMessage = await getCustomerSatisficationMsg(value);
    await sendMessageSatisfication(data, getFeedbackMessage);
    return;
  } catch (err) {
    logger.error('whatsappFeedbackUpdateDB catch error: ', err);
    return;
  }
};

/**
 *
 * @param data
 * @returns
 */
export const whatsappCcaasTableDB = async (data: any) => {
  let attachmentsMedia = null;
  let jsonValue = {};
  const replyContext = data?.context ? { replyToMessageId: data?.context?.id, replyFrom: `+${data?.context?.from}` } : null;
  if (['image', 'document', 'video', 'audio'].includes(data?.type)) {
    const media = data[data.type];
    const mediaUrl = await getWhatsAppMediaUrl(media?.id, data);
    logger.info('mediaUrl>>>>>>', mediaUrl);
    attachmentsMedia = [
      {
        url: mediaUrl?.previewUrl,
        mimeType: mediaUrl?.mime_type,
        size: mediaUrl?.size || null,
        filename: mediaUrl?.mediaId,
      },
    ];
  }
  if (data?.IsBot === 1 && data?.IsBot !== undefined) {
    jsonValue = { ...data.message };
  } else {
    jsonValue = {
      userText: {
        message: {
          replyContext,
          to: data?.agentNumber,
          from: `+${data?.customerNumber || data?.from || data?.source || data?.recepient_id}`,
          isBot: false,
          sendAt: new Date().getTime(),
          message: data?.text?.body || data?.interactive?.list_reply?.title || data?.image?.caption || data?.video?.caption || '',
          location: data?.location ?? null,
          attachments: attachmentsMedia,
          messageID: data?.id,
          messageId: data?.messageId
        },
      },
    };
  }
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL up_InsertUpdateWhatsapp(?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.id,
          new Date().getTime(),
          0,
          data?.companyId ?? null,
          JSON.stringify(jsonValue),
          `+${data?.customerNumber || data?.from || data?.source || data?.recepient_id}`,
          data?.agentNumber || data?.destination,
          data?.dispositionId ?? null,
          1,
          data?.IsSessionClosed ?? 0,
          data?.IsBot,
          data?.Summary ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
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
      logger.error('whatsappCcaasTableDB catch error: ', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const sessionIdApi = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetWhatsappHistory(?,?)',
        [Number(data?.from ?? data?.customerNumber), Number(data?.agentNumber)],
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
    } catch (error) {
      logger.error('sessionIdApi catch error: ', error);
      reject(error);
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
        [domainId, 'Whatsapp', sessionId],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log('agentDetailsApi reject err: ', err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      logger.error('agentDetailsApi catch error: ', err);
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
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      logger.error('getAssingToApi catch error: ', err);
      reject(err);
    }
  });
};

/**
 *
 * @param ph
 * @param domainId
 * @returns
 */
export const getPersonalDetails = (ph: any, domainId: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetPersonalDetails(?,?)',
        [ph, domainId],
        (err: any, result: any) => {
          if (err) {
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
      logger.error('getPersonalDetails catch error: ', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateDispositionForWhatsapp = async (data: any) => {
  try {
    const result = await executeMySQLWithRetry(
      mysqlPoolConnection,
      'call up_UpdateWhatsappHistoryDisposition(?,?,?,?,?,?,?,?,?,?)',
      [
        data?.WhatsappHistoryId,
        data?.domainId,
        data?.dispositionId ?? null,
        data?.summary ?? null,
        data?.isSessionClosed ?? 0,
        data?.afterWorkTime ?? null,
        data?.dispositionUpdatedBy ?? null,
        data?.duration ?? null,
        data?.dispositionSubject ?? null,
        data?.tags,
      ],
      {
        operationName: 'UpdateWhatsappDisposition',
        logContext: { WhatsappHistoryId: data?.WhatsappHistoryId, domainId: data?.domainId }
      }
    );

    if (result && result.length > 0) {
      return result;
    }
    return [];

  } catch (err) {
    logger.error('updateDispositionForWhatsapp - Unexpected Error:', err);
    throw err;
  }
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
      'call up_UpdateWhatsappHistoryDisposition(?,?,?,?,?,?,?,?,?)',
      [
        data?.WhatsappHistoryId,
        data?.domainId,
        closedDispositionsId?.did ?? null,
        closedDispositionsId?.dispositionName ?? null,
        1,
        data?.AfterEmailWorkTime ?? null,
        data?.dispositionUpdatedBy ?? null,
        data?.duration ?? null,
        data?.dispositionSubject ?? null,
      ],
      {
        operationName: 'UpdateWhatsappDispositionOnDisconnect',
        logContext: { WhatsappHistoryId: data?.WhatsappHistoryId, domainId: data?.domainId }
      }
    );

    if (result && result.length > 0) {
      return result[0];
    }
    return [];

  } catch (err) {
    logger.error('updateDispositionDiscnnect err', err);
    throw err;
  }
};

/**
 *
 * @param data
 * @returns
 */
export const updateDispositionBot = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_whatsappIsBotUpdate(?)',
        [
          data?.WhatsappHistoryId
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
      logger.error('updateDispositionBot err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const deleteWhatsappConfiguration = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_DeleteWhatsappConfiguration(?,?)', [
          data?.whatsappId,
          data?.domainId,
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
      logger.error('deleteWhatsappConfiguration err', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const assignCallBackWhatsapp = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const sessionId: any = data.Istransferred === 1 ? uuidv4() : null;
      mysqlPoolConnection.query(
        'call up_UpdateAssignAgentToWhatsapp(?,?,?,?,?,?)',
        [
          data?.assignedBy ?? null,
          data?.assignedTo ?? null,
          data?.assignedOn ?? null,
          data?.WhatsappHistoryId ?? null,
          data.Istransferred ?? null,
          sessionId ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (error) {
      logger.error('assignCallBackWhatsapp err', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @param sessionId
 * @returns
 */
export const whatsappSessionHistory = (data: any, sessionId: any) => {
  const message = JSON.stringify(data?.message);
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL up_InsertUpdateWhatsapp(?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          sessionId,
          null,
          null,
          null,
          message,
          data.to,
          data.from,
          null,
          null,
          0,
          0,
          null
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
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
      logger.error('whatsappSessionHistory err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateWhatsappBot = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_update_whatsapp_purchase_bot_status(?,?,?,?)',
        [
          data?.domainId,
          data?.isBot ?? 0,
          data?.isBotPurchased ?? 0,
          data?.your_number,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (error) {
      logger.error('updateWhatsappBot err', error);
      reject(error);
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
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (error) {
      logger.error('getSupervisiorDetails err', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getBotDetail = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_getWhatsappBot(?,?)',
        [
          data?.calledNumber,
          data?.domainId
        ],
        (err: any, result: any) => {
          if (err) {
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
      logger.error('getBotDetail err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const Satification = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_getWhatsappBot(?,?)',
        [
          data?.domainContact,
          data?.domainId
        ],
        (err: any, result: any) => {
          if (err) {
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
      logger.error('Satification err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getBotDetailWhatsapp = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_getWhatsappBot(?,?)',
        [
          data?.agentNumber,
          data?.companyId
        ],
        (err: any, result: any) => {
          if (err) {
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
      logger.error('getBotDetailWhatsapp err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getPlanDetail = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_getWhatsappBot(?,?)',
        [
          data?.agentNumber,
          data?.companyId
        ],
        (err: any, result: any) => {
          if (err) {
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
      logger.error('getPlanDetail err', err);
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
            resolve([]);
          }
        }
      );
    } catch (err) {
      logger.error('getSupervisiorList err', err);
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
            reject(err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      logger.error('getDispositinStatus err', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getDidPurchaseWhatsappBotDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetWhatsappDidPurchaseByDomainID(?)',
        [
          data?.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (error) {
      logger.error('getDidPurchaseWhatsappBotDao err', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateDidNumberPurchaseWhatsappBotDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_InsertUpdateWhatsappDidPurchase(?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.pid,
          data?.type,
          data?.yourNumber,
          data?.domainId,
          data?.whatsAppFlowId,
          data?.queueId,
          data?.outbound,
          data?.threshold,
          data?.defaultUpdated,
          data?.isBot,
          data?.isBotPurchased,
          data?.knowledgeBasedId,
          data?.autoAssignAgent,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      logger.error('updateDidNumberPurchaseWhatsappBotDao err', error);
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
          data?.WhatsappHistoryId ?? 0,
          data?.ChatDurationHistoryId ?? 0,
          channelType?.Whatsapp,
          data?.domainId ?? null,
          sentiment?.sentimentalScore ?? null,
          sentiment?.sentimentalScoreUser ?? null,
          sentiment?.sentimentalScoreAgent ?? null,
          data?.ext ?? null
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
export const updateSentimentScoreBot = (data: any, sentiment: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_updateSentimentalScore(?,?,?,?,?,?,?,?)',
        [
          data?.WhatsappHistoryId ?? 0,
          data?.ChatDurationHistoryId ?? 0,
          channelType?.Whatsapp,
          data?.domainId ?? null,
          sentiment ?? null,
          sentiment?.sentimentalScoreUser ?? null,
          sentiment?.sentimentalScoreAgent ?? null,
          data?.ext ?? null
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
      logger.error('updateSentimentScoreBot err', err);
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
            resolve([]);
          }
        }
      );
    } catch (err) {
      logger.error('getKnowledgeBaseId err', err);
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
        'call up_AutoAssignAgentToWhatsapp(?,?,?)',
        [
          data?.WhatsappHistoryId ?? 0,
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
            resolve([]);
          }
        }
      );
    } catch (err) {
      logger.error('autoAssignAgent err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertUpdateWhatsappConfig = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_InsertUpdateWhatsappConfiguration(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.WhatsappConfigurationId ?? null,
          data?.whatsappNumber ?? null,
          data?.profilePicture ?? null,
          data?.businessName ?? null,
          data?.aboutBusiness ?? null,
          data?.isBot ?? 0,
          data?.whatsappFlowId ?? null,
          data?.Industry ?? null,
          data?.businessAddress ?? null,
          data?.contactEmail ?? null,
          data?.websiteURL1 ?? null,
          data?.websiteURL2 ?? null,
          data?.isDisabled ?? null,
          data?.domainId ?? null,
          data?.planId ?? null,
          data?.isBotPurchased ?? null,
          data?.isConnector ?? null,
          data?.connectorName ?? null,
          data?.storeName ?? null,
          data?.callBackToggle ?? null,
          data?.callBackForm ?? null,
          data?.notifyAdmin ?? null,
          data?.notificationMessages ?? null,
          data?.companyLogo ?? null,
          data?.businessInformation ?? null,
          data?.statusMessage ?? null,
          data?.address ?? null,
          data?.emailID ?? null,
          data?.customerOptIn ?? null,
          data?.pricing ?? null,
          data?.whatsAppBusinessSolutionUseCases ?? null,
          data?.companyProfile ?? null,
          data?.locationOfCompanyHeadquarters ?? null,
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
      logger.error('insertUpdateWhatsappConfig err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getWhatsappConfig = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetWhatsappConfigurationList(?)',
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
            resolve([]);
          }
        }
      );
    } catch (err) {
      logger.error('getWhatsappConfig err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getWhatsappConfigDetails = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetWhatsappConfigurationDetails(?,?)',
        [
          data?.whatsappId ?? 0,
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
            resolve([]);
          }
        }
      );
    } catch (err) {
      logger.error('getWhatsappConfigDetails err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getDisableConfigDetails = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_EnableDisableWhatsappConfiguration(?,?,?)',
        [
          data?.WhatsappConfigurationId ?? null,
          data?.domainId ?? null,
          data?.isDisabled ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log(err);
          }
          resolve(result);
        }
      );
    } catch (err) {
      logger.error('getDisableConfigDetails err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getWhatsapChannel = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_getChatFlow(?,?)',
        [
          data?.domainId ?? null,
          data?.channelType ?? null,
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
      logger.error('getWhatsapChannel err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getWhatsappConfigData = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_CheckWhatsappBot(?,?)',
        [
          data?.agentNumber ?? null,
          data?.companyId ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log(err);
          }
          if (result[0]?.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      logger.error('getWhatsappConfigData err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getWhatsappConfigConnceter = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_CheckWhatsappBot(?,?)',
        [
          data?.calledNumber ?? null,
          data?.domainId ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log(err);
          }
          if (result[0]?.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      logger.error('getWhatsappConfigConnceter err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param cfid
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
export const updateChatMessage = (data: any) => {
  const message: any = JSON.stringify(data.message);
  const chat_id = parseInt(data?.WhatsappHistoryId, 10);
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_InserUpdateWhatsappMessage(?,?)',
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
      logger.error('updateChatMessage err', error);
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
      logger.error('getTransferAgentStatus err', error);
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
        'call up_UpdateNextFlowNodeForWhatsapp(?,?,?)', [
          data?.WhatsappHistoryId ?? null,
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
      logger.error('updateNextNode err', error);
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
        'call up_GetNextFlowNodeForWhatsapp(?,?)', [
          data?.WhatsappHistoryId ?? null,
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
      logger.error('getNextNode err', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertPhoneNumber = (data: any) => {
  const phoneNumbar = data?.text?.body?.replace(/\D/g, '');
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_CheckInsertCustomerContact(?,?,?)',
        [
          phoneNumbar,
          data?.email ?? null,
          data?.companyId ?? 0
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
      logger.error('insertPhoneNumber err', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertEmail = (data: any) => {
  const email = data?.text?.body;
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_CheckInsertCustomerContact(?,?,?)',
        [
          data?.phoneNumbar ?? null,
          email ?? null,
          data?.companyId ?? 0
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
      logger.error('insertEmail err', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateCustomerFirstName = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_UpdateCustomerName(?,?,?,?)', [
          data?.text?.body ?? null,
          null,
          data?.from ?? null,
          data?.companyId ?? 0,
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
      logger.error('insertEmail err', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateCustomerLastName = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_UpdateCustomerName(?,?,?,?)', [
          null,
          data?.text?.body ?? null,
          data?.from ?? null,
          data?.companyId ?? 0,
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
      logger.error('updateCustomerLastName err', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertCustomerContact = (data: any, checkEmail: any) => {
  const number = data?.from?.replace(/\D/g, '');
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
          data?.facebook ?? null,
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
          number ?? null,
          checkEmail ?? null
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
      logger.error('insertCustomerContact err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertCustomerContactPhone = (data: any) => {
  const number = data?.from?.replace(/\D/g, '');
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
          data?.facebook ?? null,
          data?.instagram ?? null,
          number ?? null,
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
          'WHATSAPP',
          data?.customField ?? null,
          data?.customValues ?? null,
          number ?? null,
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
      logger.error('insertCustomerContactPhone err', err);
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
        'call up_GetCustomerPrimaryEmailForPrimaryPhone(?,?)', [
          data?.customerNumber ?? null,
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
      logger.error('getCustomerEmailId err', error);
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
      logger.error('getAdminEmail err', error);
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
        'call up_get_customer_existing_status(?,?)', [
          data?.customerNumber ?? null,
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
      logger.error('getExistingCustomerContact err', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertWhatsappTemplate = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL up_InsertUpdateWhatsAppTemplate(?,?,?,?,?,?,?,?,?)',
        [
          data?.templateName,
          data?.templateType,
          data?.domainId,
          data?.buttonName,
          data?.buttonUrl,
          data?.content,
          data?.id,
          data?.number,
          data?.flowId
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
      logger.error('insertWhatsappTemplate err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getWhatsappTemplate = (data: any, id: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetWhatsAppTemplateById(?,?)', [
          id?.whatsappFlowId ?? null,
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
      logger.error('getWhatsappTemplate err', error);
      reject(error);
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
          data?.WhatsappHistoryId ?? null,
          flowId[0]?.whatsappFlowId ?? null,
          data?.customerNumber ?? null,
          data?.domainId ?? 0,
          2003005,
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
      logger.error('updateConnectData err', error);
      reject(error);
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
          data?.WhatsappHistoryId ?? null,
          data?.domainId ?? 0,
          2003005,
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
      logger.error('getconnectData err', error);
      reject(error);
    }
  });
};

/**
 *
 * @param url
 * @param token
 * @returns
 */
export const getIntegrationApiData = (url: any, token: any) => {
  return new Promise((resolve) => {
    try {
      const headers = {
        Authorization: token,
      };
      axios.get(url, { headers }).then((response: any) => {
        if (response?.data?.statusCode === 200 || response?.data?.statusCode === 401) {
          resolve(response);
        } else {
          resolve([]);
        }
      });
    } catch (error) {
      logger.error('getIntegrationApiData err', error);
      resolve([]);
    }
  });
};

/**
 *
 * @param data
 * @param datas
 * @param que
 * @returns
 */
export const getQuestionsAns = (data: any, datas: any, que: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_get_question_ans(?,?,?)',
        [data?.domainId, datas?.whatsappFlowId, que],
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
export const getCompanyName = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_get_companyName(?)',
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
      logger.error('getCompanyName err', error);
      reject(error);
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

/**
 *
 * @param data
 * @returns
 */
export const createEcompurchesFlow = (data: any, connecterDatas: any, jsonString: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_create_ecom_purchaseflow_log(?,?,?,?,?,?,?,?,?,?)',
        [
          channelType?.Whatsapp,
          connecterDatas?.connectorName ?? null,
          data?.WhatsappHistoryId ?? null,
          data?.domainId ?? null,
          data?.isPurchased ?? 0,
          jsonString,
          data?.customerNumber ?? null,
          data?.calledNumber ?? null,
          data?.errcode ?? 0,
          data?.errmsg ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log(err);
          }
          if (result[0]?.length > 0) {
            resolve(result[0][0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      logger.error('createEcompurchesFlow err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const createEcompurchesFlowOrder = (data: any, orderJson: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_create_ecom_purchaseflow_log(?,?,?,?,?,?)',
        [
          channelType?.Whatsapp,
          data?.WhatsappHistoryId ?? null,
          data?.domainId ?? null,
          2,
          JSON.stringify(orderJson?.order),
          data?.customerNumber ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log(err);
          }
          if (result[0]?.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      logger.error('createEcompurchesFlowOrder err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getEcomPurchaseFlow = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_ecom_purchaseflow_log(?,?)',
        [
          data?.tokenId ?? null,
          data?.domainId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log(err);
          }
          if (result[0]?.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      logger.error('getEcomPurchaseFlow err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateEcomPurchaseFlow = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_update_ecom_purchaseflow_log(?,?,?,?,?)',
        [
          data?.tokenId ?? null,
          data?.domainId ?? null,
          data?.isPurchased ?? 0,
          data?.errcode ?? null,
          data?.errmsg ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log(err);
          }
          if (result[0]?.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      logger.error('updateEcomPurchaseFlow err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateEcomIsPurchase = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_update_isPurchase_ecom_purchaseflow_log(?,?,?)',
        [
          channelType?.Whatsapp,
          data?.tokenId,
          data?.domainId,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log(err);
          }
          if (result[0]?.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      logger.error('updateEcomIsPurchase err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertUpdateMetaWhatsapp = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_meta_insert_update_whatsapp_company_dtl(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.profilePicture,
          data?.businessName,
          data?.planId,
          data?.isBot,
          data?.isBotPurchased,
          data?.whatsappNumber,
          data?.createdBy,
          data?.purchasedat,
          data?.aboutBusiness,
          data?.businessAddress,
          data?.Industry,
          data?.websiteURL1,
          data?.websiteURL2,
          data?.isRegistered,
          data?.is2faEnabled,
          data?.isVerifiedCode
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
      logger.error('insertUpdateMetaWhatsapp err', err);
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
        'CALL up_update_noOfRepeats_WhatsappHistory(?,?,?)',
        [
          data?.domainId,
          data?.WhatsappHistoryId,
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
      logger.error('updateNoOfRepeats err', err);
      reject(err);
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
        'CALL up_get_noOfRepeats_WhatsappHistory(?,?)',
        [
          data?.domainId,
          data?.WhatsappHistoryId
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
      logger.error('getNoOfRepeatsRes err', err);
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
          channelType?.Whatsapp,
          data?.WhatsappHistoryId,
          data?.domainId,
          data?.calledNumber ?? null,
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
      logger.error('insertFlowTracking err', err);
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
          channelType?.Whatsapp,
          data?.WhatsappHistoryId,
          data?.domainId,
          data?.calledNumber ?? null,
          null,
          data.processflow ?? null,
          datas?.companyId ?? null,
          datas?.appointmentId ?? null,
          datas?.userId ?? null,
          date?.serviceId ?? null,
          date?.title ?? null,
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
      logger.error('updateAppointmentdate err', err);
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
          channelType?.Whatsapp,
          data?.WhatsappHistoryId,
          data?.domainId,
          data?.calledNumber ?? null,
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
      logger.error('updateAppointmentDuration err', err);
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
          channelType?.Whatsapp,
          data?.WhatsappHistoryId,
          data?.domainId,
          data?.calledNumber ?? null,
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
      logger.error('updateAppointmentServiceId err', err);
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
          channelType?.Whatsapp,
          data?.WhatsappHistoryId,
          data?.domainId,
          data?.calledNumber ?? null,
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
      logger.error('updateAppointmentTime err', err);
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
          channelType?.Whatsapp,
          data?.WhatsappHistoryId,
          data?.domainId,
          data?.calledNumber ?? null,
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
      logger.error('updateAppointmentConsultant err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updatePhoneNumber = (data: any, datas: any) => {
  const number = datas?.body?.replace(/\D/g, '');
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_insert_update_appointment_flow_tracking_info(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          channelType?.Whatsapp,
          data?.WhatsappHistoryId,
          data?.domainId,
          data?.calledNumber ?? null,
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
      logger.error('updatePhoneNumber err', err);
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
          channelType?.Whatsapp,
          data?.WhatsappHistoryId,
          data?.domainId,
          data?.calledNumber ?? null,
          null,
          data.processflow ?? null,
          datas?.companyId ?? null,
          datas?.appointmentId ?? null,
          datas?.userId ?? null,
          date?.serviceId ?? null,
          data?.processDate ?? null,
          data?.processDuration ?? null,
          date?.processTime ?? null,
          date?.id ?? null,
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
      logger.error('updateAppointmentConsultantId err', err);
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
          channelType?.Whatsapp,
          data?.WhatsappHistoryId,
          data?.domainId,
          data?.calledNumber ?? null,
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
      logger.error('updateAppointmentUserDetails err', err);
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
          channelType?.Whatsapp,
          data?.WhatsappHistoryId,
          data?.domainId,
          data?.calledNumber ?? null,
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
      logger.error('updateAppointmentUserCreateBy err', err);
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
          channelType?.Whatsapp,
          data?.WhatsappHistoryId,
          data?.domainId,
          data?.calledNumber ?? null,
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
          date?.id ?? null
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
      logger.error('updateCategoryId err', err);
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
          channelType.Whatsapp,
          data?.WhatsappHistoryId,
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
      logger.error('getFlowTracking err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getContactData = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL up_GetPersonalDetails(?,?)',
        [
          data?.customerNumber,
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
      logger.error('getContactData err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateCallbackStatus = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_update_whatsapp_callback_status(?,?)',
        [
          data?.domainId,
          data?.WhatsappHistoryId,
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
      logger.error('updateCallbackStatus err', err);
      reject(err);
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
          data?.WhatsappHistoryId,
          channelType?.Whatsapp
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
      logger.error('updateMissedChat err', err);
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
        'call ccaas_get_coid_by_customerNumber(?,?)',
        [
          data?.customerNumber,
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
      logger.error('getCustomerCoId err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getDispositionStatusDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_getDispositinStatus_info(?)',
        [
          data?.domainId
        ],
        (err: any, result: any) => {
          if (err) {
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
      logger.error('getDispositionStatusDao err', err);
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
        'call ccaas_insert_whatsapp_callback_status(?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.sessionId,
          data?.IsInBound,
          data?.customerNumber,
          data?.calledNumber,
          data?.whatsappHistoryId
        ],
        (err: any, result: any) => {
          if (err) {
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
      logger.error('insertWhatsappCallbackDao err', err);
      reject(err);
    }
  });
};

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
      logger.error('getUserList err', error);
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
      logger.error('getConcurrentChat err', error);
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
      logger.error('insertStatus err', error);
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
      logger.error('getRoutingProfileUser err', error);
      reject(error);
    }
  });
};

/**
 *
 * @param chatId
 * @param domainId
 * @returns
 */
export const getPreviousStatus = (chatId: any, domainId: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      const sp = 'call ccaas_get_user_chat_tracking_log(?,?)';
      mysqlPoolConnection.query(sp, [domainId, chatId], (err: any, result: any) => {
        if (err) {
          reject(err);
        } else {
          resolve(JSON.parse(JSON.stringify(result[0])));
        }
      });
    } catch (error) {
      logger.error('getPreviousStatus err', error);
      reject(error);
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
      logger.error('getRoutingProfiles err', error);
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
            console.log('getUserListByRole reject err: ', err);
          } else {
            resolve(JSON.parse(JSON.stringify(result[0])));
          }
        }
      );
    } catch (error) {
      logger.error('getAgentsSkills err', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertBotMessage = (data: any, message: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_insert_whatsapp_message_flow_break_bot(?,?,?,?)',
        [
          data?.WhatsappHistoryId,
          data?.domainId,
          message ?? null,
          data?.customerNumber
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
      logger.error('insertBotMessage err', err);
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
        'CALL ccaas_get_whatsapp_message_flow_break_bot(?,?)',
        [
          data?.WhatsappHistoryId,
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
      logger.error('getBotMessageRes err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const deleteCustomInputTemplate = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_delete_whatsApp_template_dtl(?,?)',
        [
          data?.flowId,
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
      logger.error('deleteCustomInputTemplate err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getTemplateDetails = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetWhatsAppTemplateById(?,?)', [
          data?.flowId,
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
      logger.error('getTemplateDetails err', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getPlanInNlp = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call get_planId_for_channel_type(?,?)', [
          data?.domainId,
          data?.channelType
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
      logger.error('getPlanInNlp err', error);
      reject(error);
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
      logger.error('insertUpdateCustomerFeedbackDao err', err);
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
          channelTypeCode?.Whatsapp,
          data?.WhatsappHistoryId,
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
      logger.error('getCustomerSatisficationDao err', err);
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
          channelTypeCode?.Whatsapp,
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
      logger.error('getCustomerSatisficationMsg err', err);
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
        'CALL up_UpdateWhatsappQueueidBasedOnDomainId(?,?)',
        [
          data?.WhatsappHistoryId,
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
      logger.error('updateQueueId err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateIsBot = (HistoryId: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_update_flow_bot_check(?)',
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
      logger.error('updateIsBot err', err);
      reject(err);
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
          data?.WhatsappHistoryId,
          channelType?.Whatsapp
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
      logger.error('updateCustomerId err', err);
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
      logger.error('getCustomerConatctDetails err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateEmailid = (data: any, datas: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_appointment_update_emailId(?,?,?,?,?)',
        [
          data?.WhatsappHistoryId,
          data?.domainId ?? null,
          datas?.body ?? null,
          data?.channelType,
          data?.calledNumber
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
      logger.error('updateEmailid err', err);
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
          data?.WhatsappHistoryId,
          data?.domainId ?? null,
          datas?.body ?? null,
          data?.channelType,
          data?.calledNumber
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
      logger.error('updatePhNumber err', err);
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
          data?.WhatsappHistoryId,
          data?.domainId ?? null,
          datas?.body ?? null,
          null,
          data?.channelType,
          data?.calledNumber
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
      logger.error('updateCustomerFirstNameAppoinment err', err);
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
          data?.WhatsappHistoryId,
          data?.domainId ?? null,
          null,
          datas?.body ?? null,
          data?.channelType,
          data?.calledNumber
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
      logger.error('updateCustomerLastNameAppoinment err', err);
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
    } catch (err) {
      logger.error('getAllQueueWaitTime err', err);
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
          await sendMessageCustomerCai(datas, repMessage);
          sendMessageCustomerCount(datas, repMessage);
          resolve(result);
        }
      );
    } catch (err) {
      logger.error('insertUpdateQueueWaitTimeDao err', err);
      reject(err);
    }
  });
};

/**
 * @description Get queue wait time
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
    } catch (err) {
      logger.error('getUserExt err', err);
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
          data?.WhatsappHistoryId,
          channelType?.Whatsapp,
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
      logger.error('getRemaindeResponse err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertBotMessageIsRemainder = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL update_and_insert_message_all_channel_bot(?,?,?,?,?,?)',
        [
          data?.WhatsappHistoryId,
          channelType?.Whatsapp,
          data?.domainId,
          null,
          data?.customerNumber,
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
      logger.error('insertBotMessageIsRemainder err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertBotMessageRemainderWhatsapp = (data: any, message: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      const messageData = { message };
      mysqlPoolConnection.query(
        'CALL update_and_insert_message_all_channel_bot(?,?,?,?,?,?)',
        [
          data?.WhatsappHistoryId,
          channelType?.Whatsapp,
          data?.domainId,
          JSON.stringify(messageData) ?? null,
          data?.customerNumber,
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
      logger.error('insertBotMessageRemainderWhatsapp err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertMetaTemplate = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_insert_metaWhatsappTemplateFlow(?,?,?,?,?)',
        [
          data?.customInputId,
          data?.templateName,
          data?.domainId,
          data?.customerNumber,
          data?.flowId
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
      logger.error('insertMetaTemplate err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getMetaTemplateResponse = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_metaWhatsappTemplateFlow(?,?)', [
          data?.domainId,
          data?.flowId,

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
    } catch (err) {
      logger.error('getMetaTemplateResponse err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const deleteMetaTemplate = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_delete_metaWhatsappTemplateFlow(?,?)', [
          data?.domainId,
          data?.customInputId,
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
    } catch (err) {
      logger.error('deleteMetaTemplate err', err);
      reject(err);
    }
  });
};

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
          data?.companyId
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
    } catch (err) {
      logger.error('getRemainingSessionEmailSent err', err);
      reject(err);
    }
  });
};

/**
 * @description Get connector token info using SP
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

/**
 *
 * @param data
 * @returns
 */
export const updateCallbackstatus = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_update_customer_callBack_status(?,?,?)',
        [
          data?.domainId,
          'Whatsapp',
          data?.WhatsappHistoryId,

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
