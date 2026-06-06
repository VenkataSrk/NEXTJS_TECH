import axios from 'axios';
import { mysqlPoolConnection } from '../plugins/db';
import { logger } from '../plugins/log';

/**
 *
 * @param data
 * @returns
 */
export const updateChatMessageInsta = (data: any) => {
  const message: any = JSON.stringify(data.message);
  const chat_id = parseInt(data?.historyId, 10);
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
      console.log(error);
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
      console.log(err);
      reject(err);
    }
  });
};

/**
 *
 * @param userId
 * @returns
 */
export const updateAgentDetailsInsta = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_UpdateInstagramHistory(?,?,?,?,?,?,?,?,?)', [
          data?.InstagramHistoryId,
          data?.AgentId ?? null,
          data?.domainId,
          data?.IsBot,
          data?.BotId ?? null,
          data?.agentStartTime ?? null,
          data?.agentEndTime ?? null,
          data?.duration ?? null,
          data?.dialDuration ?? null,
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
          data?.agentNumber ?? null,
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
          data?.customerNumber,
          data?.domainId,
          'Instagram'
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
      reject(err);
    }
  });
};

// send Button fb
export async function sendCallbackRequestInsta(req: any, buttonLink: any) {
  try {
    const body = {
      domainId: req?.domainId,
      customerId: req?.customerNumber,
      pageId: req?.agentNumber,
      postbackText: 'Our agents are currently not available. Please feel free to leave a message with a brief summary of your inquiry along with your contact information, or you can get in touch with us at a later time.',
      buttons: [
        {
          type: 'web_url',
          url: `${process.env.API_URL}/${buttonLink}`,
          title: 'callback',
          webview_height_ratio: 'full'
        }
      ]
    };
    return await axios.post(process.env.postbackButtonInsta, body);
  } catch (error) {
    console.error('Error sending message:', error);
  }
}

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
          data?.historyId
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
      reject(error);
    }
  });
};

// send message facebook
export async function sendMessageInstagram(req: any) {
  try {
    const body = {
      domainId: req?.domainId,
      customerId: req?.customerNumber,
      pageId: req?.agentNumber,
      message: 'Our agents are currently not available. Please feel free to leave a message with a brief summary of your inquiry along with your contact information, or you can get in touch with us at a later time.'
    };
    const response = await axios.post(process.env.InstagramOut, body);
    return body;
  } catch (error) {
    console.error('Error sending message:', error);
    // You can handle the error here, e.g., log it or return a specific response
  }
}

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
          data?.historyId,
          'Instagram'
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
      console.log(err);
      reject(err);
    }
  });
};

// Agent Assist SocialMedia
export async function AgentAssistSocialMedia(data: any, comDetails: any) {
  try {
    const message = '0';
    const body = {
      message,
      domainId: data?.domainId,
      ext: data?.agentExt,
      channelType: data?.channelType,
      channelId: data?.agentNumber?.toString() ?? '',
      sessionId: data?.sessionId,
      websiteId: '',
      type: '',
      company_name: comDetails?.companyName ?? '',
      industry_type: comDetails?.industryType ?? '',
      customerId:data?.customerNumber?.toString() ?? '',
    };
    console.log('Sending request to bot:', JSON.stringify(body, null, 2));
    const response = await axios.post(process.env.semiChannel, body);
    console.log('Response from nlp>>>>>>>>>>>', JSON.stringify(response.data, null, 2));
    return response;
  } catch (error) {
    console.error('Error sending message:', error);
    // You can handle the error here, e.g., log it or return a specific response
  }
}

// Agent Assist Email sms
export async function AgentAssistEmailSms(data: any, getWebsite: any) {
  try {
    const message = JSON.parse(data?.message);
    const messagess = Array.isArray(message) ? message?.map((item: any) => item?.userText?.message?.message)?.join(' , ') : message?.userText?.message?.message;
    const body = {
      message: messagess,
      domainId: data?.domainId,
      ext: data?.agentExt,
      channelType: data?.channelType,
      channelId: data?.agentNumber?.toString() ?? '',
      sessionId: data?.sessionId,
      websiteId: getWebsite.websiteId ?? '',
      type: '',
      company_name: (getWebsite?.baseName === null ? '' : getWebsite?.baseName) ?? '',
      industry_type: (getWebsite?.CompanyType === null ? '' : getWebsite?.CompanyType) ?? '',
      customerId:data?.customerNumber?.toString() ?? '',
      messageId:data?.messageId ?? ''
    };
    console.log('✅ Sending request to bot:', JSON.stringify(body, null, 2));
    const response = await axios.post(process.env.semiChannel, body);
    console.log('✅ Response from nlp>>>>>>>>>>>', JSON.stringify(response.data, null, 2));
    return response;
  } catch (error) {
    console.error('❌ Error sending message:', error);
  }
}

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
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const configurationDetails = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_getWhatsappBot(?,?)',
        [
          data?.agentNumber,
          data?.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log('getBotDetail whatsapp err: ', err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      console.log('getBotDetail catch err: ', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const configurationEmailDetails = (data: any, value: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call get_knowledgeBasedId_assist(?,?,?)',
        [
          data?.agentNumber,
          data?.domainId,
          value
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log('getBotDetail whatsapp err: ', err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      console.log('getBotDetail catch err: ', err);
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
export const configurationFbDetails = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_GetFacebookBot(?,?)',
        [
          data?.agentNumber,
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
      console.log(err);
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
export const configurationInstaDetails = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_GetInstagramBot(?,?)',
        [
          data?.agentNumber,
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
      console.log(err);
      reject(err);
    }
  });
};

/**
 * Get company details by domainId
 *
 * @param data
 * @returns Promise<any>
 */
export const getCompanyDetails = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL up_GetCompanyDomain(?)',
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

// send nlp facebook decline
export async function getDeclineBot(data:any) {
  try {
    const body = {
      domain_id: data?.domainId
    };
    logger.info('Sending request to bot:', JSON.stringify(body, null, 2));
    const response = await axios.post(process?.env?.agentDecline, body);
    logger.info('Response from nlp>>>>>>>>>>>', JSON.stringify(response.data, null, 2));
    return response?.data;
  } catch (error) {
    logger.error('getFacebookBot err', error);
  }
}
