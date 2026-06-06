import { v4 as uuidv4 } from 'uuid';
import { loggerError, loggerTrace } from '../../src/utils/log';
import { logger } from '../../src/utils/logger';
import { mysqlPoolConnection } from '../plugins/sql';

/**
 * @description sms insert
 * @param data
 * @returns
 */
export const smsCcaasTableDB = (data: any) => {
  let jsonValue = {};
  if (data?.IsBot === 1 && data?.IsBot !== undefined) {
    jsonValue = { ...data.message };
  } else {
    jsonValue = {
      userText: {
        message: {
          to: data.destination,
          from: data.source,
          isBot: false,
          pdfurl: '',
          sendAt: Date.now(),
          sender: {},
          message: data.content,
          fileType: '',
          messageID: uuidv4,
          sessionId: data.id,
          isCustomer: true,
          messageType: 'SMS',
        },
      },
    };
  }
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL up_InsertUpdateSMS(?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data.id,
          Date.now(),
          data?.sessionEndTime ?? null,
          null,
          JSON.stringify(jsonValue),
          data?.source ?? null,
          data?.destination ?? null,
          data?.dispositionId ?? null,
          data?.type === 'Inbound' ? 1 : 0,
          data?.IsSessionClosed ?? 0,
          data?.IsBot ?? 0,
          data?.Summary ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            resolve(err);
          } else {
            resolve(result[0]);
          }
        }
      );
    } catch (err) {
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const smsTableDB = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL did_number_sms_Integration_get_dtl(?)',
        [
          data
        ],
        (err: any, result: any) => {
          if (err) {
            loggerError('smsTableDB - DB Error', err, { data });
            resolve(err);
          } else {
            loggerTrace('smsTableDB - Result', { data }, result[0]);
            resolve(result[0]);
          }
        }
      );
    } catch (err) {
      loggerError('smsTableDB - Exception', err, { data });
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getAssingToApi = (UserId: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetUserExt(?)',
        [
          UserId
        ],
        (err: any, result: any) => {
          if (err) {
            loggerError('up_GetUserExt - DB Error', { UserId }, err);
            reject(err);
          }
          const rows = Array.isArray(result?.[0]) ? result[0] : [];
          loggerTrace('up_GetUserExt - Success', { UserId }, rows);
          console.log('rowsrowsrowsrowsrows', rows);
          return resolve(rows); // ✅ resolve with array always
        }
      );
    } catch (err) {
      loggerError('up_GetUserExt - Exception', { UserId }, err);
      reject(err);
    }
  });
};

/**
 *
 * @param qdata
 * @returns
 */
export const sessionIdApi = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetSMSHistory(?,?)',
        [
          data.source ?? null,
          data.destination ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
          }
          resolve(result[0]);
        }
      );
    } catch (error) {
      loggerError('up_GetSMSHistory - Unexpected Error', data, error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const agentDetailsApi = (sessionId: any, domainId: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccass_getDetailsBySessionId(?,?,?)',
        [
          domainId,
          'Sms',
          sessionId
        ],
        (err: any, result: any) => {
          if (err) {
            loggerError('ccass_getDetailsBySessionId - Unexpected Error', {}, err);
            reject(err);
            console.log(err);
          }
          loggerTrace('ccass_getDetailsBySessionId - Success', {}, result?.[0]);
          resolve(result[0]);
        }
      );
    } catch (err) {
      loggerError('ccass_getDetailsBySessionId - Unexpected Error', {}, err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getPersonalDetails = (ph: any, domainId: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetPersonalDetails(?,?)',
        [
          ph,
          domainId
        ],
        (err: any, result: any) => {
          if (err) {
            loggerError('getPersonalDetails - DB Error', {}, err);
            reject(err);
            console.log(err);
          }
          loggerTrace('getPersonalDetails - Success', {}, result?.[0]);
          resolve(result[0]);
        }
      );
    } catch (err) {
      loggerError('getPersonalDetails - Unexpected Error', { ph, domainId }, err);
      reject(err);
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
        'call ccaas_getIsBot(?,?)',
        [
          data.calledNumber,
          data.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log(err);
          }
          resolve(result[0]);
        }
      );
    } catch (err) {
      loggerError('ccaas_getIsBot', err, {});
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateSmsBot = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_update_did_purchase_bot_status(?,?,?,?)',
        [
          data?.domainId,
          data?.isBot ?? 0,
          data?.isBotPurchased ?? 0,
          data?.your_number ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            loggerError('ccaas_update_did_purchase_bot_status', err, {});
            return reject(err);
          }
          const response = result?.[0] ?? [];
          loggerTrace('ccaas_update_did_purchase_bot_status', response, {});
          resolve(response);
        },
      );
    } catch (err) {
      loggerError('ccaas_update_did_purchase_bot_status', data, err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const onbordUpdateSmsBot = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_update_did_purchase_bot_status_onboard(?,?,?)',
        [
          data?.domainId,
          data?.isBot ?? 0,
          data?.isBotPurchased ?? 0
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log(err);
          }
          const response = result?.[0] ?? [];
          loggerTrace('ccaas_update_did_purchase_bot_status_onboard', data, response);
          resolve(response);
        }
      );
    } catch (err) {
      loggerError('ccaas_update_did_purchase_bot_status_onboard', data, err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const autoAssignAgent = (data: any, presenceStatus: any, queue:any) => {
  return new Promise(async (resolve, reject) => {
    try {
      console.log([
        data?.SMSHistoryId ?? 0,
        data?.domainId,
        presenceStatus ?? null,
        Number(queue?.qId) ?? null
      ]);
      mysqlPoolConnection.query(
        'call up_AutoAssignAgentToSMS(?,?,?,?)',
        [
          data?.SMSHistoryId ?? 0,
          data?.domainId,
          presenceStatus ?? null,
          Number(queue?.qId) ?? null
        ],
        (err: any, result: any) => {
          loggerError('up_AutoAssignAgentToSMS', err, {});
          if (err) {
            reject(err);
            console.log(err);
          }
          loggerTrace('up_AutoAssignAgentToSMS', data, result);
          if (result && result?.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      loggerError('up_AutoAssignAgentToSMS', err, {});
      reject(err);
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
      const normalizeScore = (value: any) => {
        if (value === null || value === undefined || value === '') return null;
        const num = Number(value);
        if (Number.isNaN(num)) return null;
        return Math.max(-1, Math.min(1, num));
      };
      const params = [
        data?.SMSHistoryId ?? 0,
        data?.ChatDurationHistoryId ?? 0,
        'Sms',
        data?.domainId ?? null,
        normalizeScore(sentiment?.sentimentalScore),
        normalizeScore(sentiment?.sentimentalScoreUser),
        normalizeScore(sentiment?.sentimentalScoreAgent),
        data?.ext ?? null,
      ];
      mysqlPoolConnection.query(
        'call ccaas_updateSentimentalScore(?,?,?,?,?,?,?,?)', params,
        (err: any, result: any) => {
          if (err) {
            loggerError('updateSentimentScore - DB Error', data, err);
            return resolve(null);
          }
          resolve(result[0]);
        }
      );
    } catch (err) {
      loggerError('updateSentimentScore - Unexpected Error', data, err);
      resolve(null);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateSentimentBot = (data: any, sentiment: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      const normalizeScore = (value: any) => {
        if (value === null || value === undefined || value === '') return null;
        const num = Number(value);
        if (Number.isNaN(num)) return null;
        return Math.max(-1, Math.min(1, num));
      };
      mysqlPoolConnection.query(
        'call ccaas_updateSentimentalScore(?,?,?,?,?,?,?,?)',
        [
          data?.SMSHistoryId ?? 0,
          data?.ChatDurationHistoryId ?? 0,
          'Sms',
          data?.domainId ?? null,
          normalizeScore(sentiment),
          normalizeScore(sentiment?.sentimentalScoreUser),
          normalizeScore(sentiment?.sentimentalScoreAgent),
          data?.ext ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            loggerError('updateSentimentBot - DB Error', data, err);
            return resolve(null);
          }
          resolve(result[0]);
        }
      );
    } catch (err) {
      loggerError('updateSentimentScore - Unexpected Error', data, err);
      resolve(null);
    }
  });
};

/**
 *
 * @param domainId
 * @returns
 */
export const getKnowledgeBaseId = (domainId: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_CategoryArticleCount(?,?)',
        [
          domainId,
          null
        ],
        (err: any, result: any) => {
          loggerError('up_CategoryArticleCount', err, {});
          if (err) {
            reject(err);
            console.log(err);
          }
          if (result && result.length > 0) {
            loggerTrace('up_CategoryArticleCount', {}, result[0]);
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      loggerError('ccaas_update_did_purchase_bot_status', err, {});
      reject(err);
    }
  });
};

/**
 *
 * @param qdata
 * @returns
 */
export const assignCallBackSmsDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const sessionId: any = data.Istransferred === 1 ? uuidv4() : null;
      mysqlPoolConnection.query(
        'call up_UpdateAssignAgentToSMS(?,?,?,?,?,?)',
        [
          data.assignedBy ?? null,
          data.assignedTo ?? null,
          data.assignedOn ?? null,
          data.SMSHistoryId ?? null,
          data.Istransferred ?? null,
          sessionId ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            loggerError('up_UpdateAssignAgentToSMS', data, err);
            reject(err);
          }
          loggerTrace('up_UpdateAssignAgentToSMS', data, result);
          resolve(result);
        }
      );
    } catch (error) {
      loggerError('up_UpdateAssignAgentToSMS', data, error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateDispositionForSms = (data: any): Promise<any> => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_UpdateSMSHistoryDisposition(?,?,?,?,?,?,?,?,?,?)',
        [
          data?.SMSHistoryId,
          data?.domainId,
          data?.dispositionId ?? null,
          data?.summary ?? null,
          data?.isSessionClosed ?? 0,
          data?.AfterSMSWorkTime ?? null,
          data?.dispositionUpdatedBy ?? null,
          data?.duration ?? null,
          data?.dispositionSubject ?? null,
          data?.tags,
        ],
        (err: any, result: any) => {
          if (err) {
            loggerError('updateDispositionForSms - DB Error', err, data);
            reject(err);
          } else {
            loggerTrace('updateDispositionForSms - Success', data, result?.[0]);
            resolve(result?.[0]);
          }
        }
      );
    } catch (err) {
      loggerError('updateDispositionForSms - Exception', err, data);
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
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_supervisor_ext(?)',
        [data?.domainId],
        (err: any, result: any) => {
          if (err) {
            loggerError('getSupervisiorList - DB Error', err, data);
            reject(err);
          } else {
            resolve(result?.[0]);
          }
        }
      );
    } catch (err) {
      loggerError('getSupervisiorList - Exception', err, data);
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
      loggerError('ccaas_getDispositinStatus_info:', error, {});
      console.log(error);
      reject(error);
    }
  });
};

/**
 * @description sms session history
 * @param data
 * @returns
 */
export const SmsSessionHistory = (data: any, sessionId: any) => {
  const message = JSON.stringify(data?.message);
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL up_InsertUpdateSMS(?,?,?,?,?,?,?,?,?,?,?,?)',
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
            loggerError('SmsSessionHistory - DB Error', {}, err);
            resolve(err);
          } else {
            loggerTrace('SmsSessionHistory - Success', {}, result?.[0]);
            resolve(result[0]);
          }
        }
      );
    } catch (err) {
      loggerError('SmsSessionHistory - Unexpected Error', { sessionId, data }, err);
      reject(err);
    }
  });
};

/**
 * @description sms outbound session history
 * @param data
 * @returns
 */
export const SmsOutboundSessionHistory = (data: any, sendMessage: any) => {
  const message = JSON.stringify(sendMessage?.message);
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL up_InsertUpdateSMS(?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.sessionId ?? null,
          data?.sessionStartTime ?? null,
          data?.sessionEndTime ?? null,
          data?.domainId,
          message ?? null,
          data.to ?? null,
          data.from ?? null,
          data?.dispositionId ?? null,
          data?.isInBound ?? 0,
          data?.isSessionClosed ?? 0,
          data?.isBot ?? 0,
          data?.summary ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            loggerError('SmsOutboundSessionHistory - DB Error', {}, err);
            resolve(err);
          } else {
            if (result && result.length > 0) {
              loggerTrace('SmsOutboundSessionHistory - Success', {}, result[0]);
              resolve(result[0]);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (err) {
      loggerError('SmsOutboundSessionHistory - Unexpected Error', { data }, err);
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
          data?.channelTypeId ?? null,
          data?.historyId ?? null,
          data?.videoMeetId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            loggerError('up_ccaas_GetCustomerSatisfactionForChannel Error', err, data);
            reject(err);
          } else {
            if (result?.length > 0) {
              loggerTrace('up_ccaas_GetCustomerSatisfactionForChannel- Success', data, result);
              resolve(result);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (err) {
      console.log(err);
      reject(err);
      loggerError('up_ccaas_GetCustomerSatisfactionForChannel Error', err, data);
    }
  });
};

/**
 * @description sms outbound session history
 * @param data
 * @returns
 */
export const sendSmsOutboundCompany = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL up_InsertUpdateSMS(?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.sessionId ?? null,
          data?.sessionStartTime ?? null,
          data?.sessionEndTime ?? null,
          data?.domainId,
          '[]',
          data.to ?? null,
          data.from ?? null,
          data?.dispositionId ?? null,
          data?.isInBound ?? 0,
          data?.isSessionClosed ?? 0,
          data?.isBot ?? 0,
          data?.summary ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            logger.error('sendSmsOutboundCompany - MySQL Error:', err);
            resolve(err);
          } else {
            if (result && result.length > 0) {
              loggerTrace(`SmsOutboundSessionHistory - Success: ${JSON.stringify(result[0])}`, {}, {});
              resolve(result[0]);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (err) {
      loggerError('sendSmsOutboundCompany - Unexpected Exception:', { data }, err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertCustomerContactPhone = (data: any, datas: any) => {
  const number = data?.source;
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
          datas?.domainId,
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
          'SMS',
          data?.customField ?? null,
          data?.customValues ?? null,
          number ?? null,
          data?.primaryEmail ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            loggerError('ccaas_insertUpdateCustomerContact', err, {});
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
      loggerError('ccaas_insertUpdateCustomerContact', err, {});
      console.log(err);
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
          data?.SMSHistoryId,
          'Sms'
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
      loggerError('ccaas_updateCustomer_Id', err, {});
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
export const insertNotificationDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_notification_details(?,?,?,?,?,?,?,?,?,?)',
        [
          data?.uuid,
          data?.domainId,
          data?.toExt,
          data?.notificationMsg,
          data?.markAsRead,
          data?.timeStamp,
          data?.type,
          data?.channelType,
          data?.coid,
          data?.customerValue,
        ],
        (err: any, result: any) => {
          console.log('result', result);
          if (err) {
            console.log('err:', err);
            reject(err);
          } else {
            if (result?.length > 0) {
              resolve(result);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (err) {
      reject(err);
    }
  });
};
export const updateReplyViaDispositionForSms = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_UpdateReplayViaSMSHistoryDisposition(?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.SMSHistoryId,
          data?.domainId,
          data?.dispositionId ?? null,
          data?.summary ?? null,
          data?.isSessionClosed ?? 0,
          data?.AfterSMSWorkTime ?? null,
          data?.dispositionUpdatedBy ?? null,
          data?.duration ?? null,
          data?.replyChannelType ?? null,
          data?.responseHistoryId ?? null,
          data?.coid ?? null,

        ],
        (err: any, result: any) => {
          if (err) {
            loggerError('updateReplyViaDispositionForSms - SQL Error', err, {});
            reject(err);
            console.log(err);
          }
          resolve(result[0]);
          loggerTrace('updateReplyViaDispositionForSms - SQL Result', {}, result[0]);
        }
      );
    } catch (err) {
      loggerError('updateReplyViaDispositionForSms - Unexpected Error', err, {});
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @param callback
 */
export const getConnectorTokenInfo = (data: any, callback: (res: any) => void) => {
  const domainId = data?.domainId ?? 0;

  const query = 'CALL ccaas_get_crm_connector_token_info_by_domain_id(?)';

  mysqlPoolConnection.query(query, [domainId], (error: any, results: any) => {
    if (error) {
      loggerError('DB Error in getConnectorTokenInfo:', error, {});
      callback([]);
      return;
    }
    loggerTrace('getConnectorTokenInfo - SQL Result', results, {});

    // MySQL SP returns results inside [ [rows], fields ]
    const rows = Array.isArray(results) && Array.isArray(results[0]) ? results[0] : Array.isArray(results) ? results : [];
    callback(rows);
  });
};

export const getCompanyDetails = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetCompanyDomain(?)',
        [data?.domainId ?? 0],
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
