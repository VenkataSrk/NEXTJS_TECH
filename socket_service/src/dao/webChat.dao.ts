import axios from 'axios';
import { v4 as uuidv4 } from 'uuid';
import { channelTypeCode } from '../helpers/constants';
import { ioredisChat, mysqlPoolConnection } from '../plugins/db';

export interface IQueueWaitTime {
  qid?: number;
  queueName?: string;
  queueType?: string;
  domainId?: number;
  queueList?: string;
  [key: string]: any;
}

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
          data?.ChatHistoryId,
          data?.sessionId ?? null,
          data?.sessionStartTime ?? null,
          data?.sessionEndTime ?? null,
          data?.chatFlowSourceId ?? null,
          data?.companyId ?? null,
          data?.domainId ?? null,
          data?.IsInBound ?? null,
          // data?.dialDuration ?? null,
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
            console.log(err);
            reject(err);
          } else {
            resolve(result);
          }
        }
      );
    } catch (error) {
      console.log(error);
    }
  });
};

export const updateAgentDetailsdata = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_InsertUpdateWhatappDurationHistory(?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.WhatsappDurationHistoryId ?? null,
          data?.WhatsappHistoryId ?? null,
          data?.StartTime ?? null,
          data?.EndTime ?? null,
          data?.Duration ?? null,
          data?.AgentId ?? null,
          data?.AgentSkillId ?? null,
          data?.IsBot ?? null,
          data?.BotId ?? null,
          data?.QueueId ?? null,
          data?.dialDuration ?? null,
          data?.IsMissed ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          } else {
            resolve(result);
          }
        }
      );
    } catch (error) {
      console.log(error);
    }
  });
};

export const updateAgentDetails = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_InserUpdateChatDurationHistory(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          (data?.ChatDurationHistoryId !== '' && data?.ChatDurationHistoryId) ? data?.ChatDurationHistoryId : null,
          data?.ChatHistoryId ? data.ChatHistoryId : null,
          data?.StartTime ?? null,
          data?.EndTime ?? null,
          data?.Duration ?? null,
          data?.AgentId ?? null,
          data?.AgentSkillId ?? null,
          data?.IsBot ?? null,
          data?.BotId ?? null,
          data?.transferredTime ?? null,
          data?.QueueId ?? null,
          data?.dispositionId ?? null,
          data?.transferedBy ?? null,
          data?.transferedTo ?? null,
          data?.ticketId ?? null,
          data?.Summary ?? null,
          data?.SentimentalScore ?? null,
          data?.sentimentalScoreCustomer ?? null,
          data?.sentimentalScoreAgent ?? null,
          data?.markAsRead ?? null,
          data?.afterChatWorkTime ?? null,
          data?.Istransferred ?? null,
          data?.IsMissed ?? null,
          data?.IsAbandoned ?? null,
          data?.answeredDuration ?? null,
          data?.chatDeflection ?? null,
          data?.isTrigger ?? 0,
        ],
        (err: any, result: any) => {
          if (err) {
            console.log('up_InserUpdateChatDurationHistory', err);
            reject(err);
          } else {
            resolve(result);
          }
        }
      );
    } catch (error) {
      console.log('up_InserUpdateChatDurationHistory error', error);
    }
  });
};

export const updateTiggerStatus = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_update_isTrigger(?,?,?)',
        [
          data?.durationHistoryId ?? null,
          data?.ChannelType,
          data?.isTrigger ?? 0,
        ],
        (err: any, result: any) => {
          if (err) {
            console.log('ccaas_update_isTrigger', err);
            reject(err);
          } else {
            resolve(result);
          }
        }
      );
    } catch (error) {
      console.log('ccaas_update_isTrigger error', error);
    }
  });
};

export const updateChatMessage = (data: any) => {
  const agentMsg = data?.message?.agentText;
  const userMsg = data?.message?.userText;
  const isCustomer: any = userMsg?.message?.isCustomer ? 1 : 0;
  const isAgent: any = agentMsg?.message?.agentMsgCount ? 1 : 0;
  const botCustomerMsgCount: any = isAgent ? 0 : (agentMsg?.message?.isLiveagent || userMsg?.message?.isLiveagent) ? 0 : 1;
  const botMsgCount = !isAgent && agentMsg && !agentMsg?.message?.isLiveagent && !agentMsg?.message?.isCustomer ? 1 : 0;
  const message: any = JSON.stringify(data.message);
  const chat_id = parseInt(data?.ChatHistoryId, 10) || 0;
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_InserUpdateChatMessage(?,?,?,?,?,?,?)',
        [
          data?.ChatDurationHistoryId ?? 0,
          chat_id ?? 0,
          message,
          isCustomer,
          isAgent,
          botCustomerMsgCount,
          botMsgCount
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          } else {
            resolve(result);
          }
        }
      );
    } catch (error) {
      console.log(error);
    }
  });
};

export const InsertUpdateAIChatMessage = (data: any) => {
  const message: any = JSON.stringify(data);
  // const chatHisId = parseInt(data?.ChatHistoryId);
  const chatHisId = parseInt(data?.ChatHistoryId, 10);
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_InsertUpdateChatAIMessage(?,?,?)',
        [chatHisId ?? 0, message, data?.sessionId],
        (err: any, result: any) => {
          if (err) {
            console.log('err::::::', err);
            // reject(err);
          } else {
            resolve(result);
          }
        }
      );
    } catch (error) {
      console.log(error);
    }
  });
};

export const getAIChatMessageDao = (data: any) => {
  console.log('getAIChatMessage---', data);
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetChatAIMessage(?)',
        [data?.ChatHistoryId ?? 0],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          } else {
            resolve(result);
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
 * @param data
 * @returns
 */

export const setChatMessage = (data: any) => {
  console.log('setChatMessage---', data);
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_SetChatMessage(?,?,?)',
        [data?.ChatDurationHistoryId,
          data?.ChatHistoryId,
          data?.message],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
          } else {
            resolve(result);
          }
        }
      );
    } catch (err) {
      console.log();
    }
  });
};
/*
 * @param userId
 * @param companyId
 * @returns
 */
export const getAgentStatus = (data: any) => {
  const channelCode = channelTypeCode[data?.channelType];
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_DeclinedUser(?,?,?,?)',
        [
          data?.sessionId ?? null,
          channelCode,
          data?.userId ?? null,
          data?.domainId ?? null,
        ],
        (errors: any, res: any) => {
          if (errors) {
            console.error(errors);
            reject([]);
          } else {
            if (res[0]?.length > 0) {
              resolve(res);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (error) {
      console.error(error);
      reject(error);
    }
  });
};

/**
 *
 * @param userId
 * @param companyId
 * @returns
 */
export const getTransferAgent = (data: any) => {
  const channelCode = channelTypeCode[data?.channelType];
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_CheckUserReadyForSocialMedia(?,?,?,?)', [
          -1,
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
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateWhatsappMessage = (data: any) => {
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
      console.log(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */

export const setTransferChatMessage = (data: any) => {
  console.log('setChatMessage---', data);
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_TransaferChatMessage(?,?)',
        [
          data?.SourceChatDurationHistoryId,
          data?.DestinationChatDurationHistoryId],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
          } else {
            resolve(result);
          }
        }
      );
    } catch (err) {
      console.log(err);
    }
  });
};
export const getBotDetail = (data: any) => {
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
          } else {
            if (result[0]?.length > 0) {
              resolve(result[0]);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (error) {
      console.log(error);
    }
  });
};

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
            console.log('ccaas_getDispositinStatus_info err: ', err);
            reject(err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log('ccaas_getDispositinStatus_info catch error: ', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateDispositionDiscnnect = (data: any, closedDispositionsId: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_UpdateWhatsappHistoryDisposition(?,?,?,?,?,?,?,?)',
        [
          data?.historyId,
          data?.domainId,
          closedDispositionsId?.did ?? null,
          closedDispositionsId?.dispositionName ?? null,
          1,
          data?.AfterEmailWorkTime ?? null,
          data?.dispositionUpdatedBy ?? null,
          data?.duration ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
          }
          if (result[0]?.length > 0) {
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
 * @param data
 * @returns
 */
export const updateDispositionBot = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_whatsappIsBotUpdate(?)',
        [
          data?.historyId
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
          }
          if (result[0]?.length > 0) {
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
 * @param data
 * @returns
 */
export const updateSentimentScore = (data: any, sentiment: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_updateSentimentalScore(?,?,?,?,?,?,?,?)',
        [
          data?.historyId ?? 0,
          data?.WhatsappDurationHistoryId ?? 0,
          'Whatsapp',
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
          if (result[0]?.length > 0) {
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
          if (result[0]?.length > 0) {
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

export const insertUpdateIrrelevantQaDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call up_InsertUpdateIrrelevantChatQuestionAnswer(?,?,?,?)', [
        data?.chatFlowId,
        data?.domainId,
        JSON.stringify(data?.questionAnswer) ?? null,
        data?.websiteId
      ],                        (err: any, result: any) => {
        console.log(result, 'resultresultresultresult');
        if (err) {
          console.log(err);
          reject(err);
        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
    }
  });
};

export const sendmailTranscript = async (datares: any) => {
  const blobFilePath = datares?.url;
  try {
    // Define email options
    const email = datares?.mail;
    const body = {
      id: uuidv4(),
      subject: 'Chat Transcript',
      from: 'support@worktual.com',
      to: email,
      text: 'Kindly have a look on the generated chat transcript report',
      sessionId: uuidv4(),
      attachments: [
        {
          filename: 'chattranscript.pdf', // Attached file name
          path: blobFilePath, // Attached file content
          // content: Buffer.from('hello word', 'utf-8')
        },
      ],
    };
    console.log('body-=-=-=-=-=-=-', body);
    await axios
      .post(
        'https://ccaasapi.worktual.co.uk/emailService/v1/send_mail_lead',
        body
      )
      .then((response: any) => {
        console.log(response.status);
      });
    // Send email
    // const info = await transporter.sendMail(mailOptions);
    // console.log('Email sent: ', info.response);

    // Remove the generated PDF file
    // fs.unlinkSync(res.filename);
  } catch (error) {
    console.error('Error sending email: ', error);
  }

};
export const updateSentimentScoreChat = (data: any, sentiment: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_updateSentimentalScore(?,?,?,?,?,?,?,?)',
        [
          data?.HistoryId ?? 0,
          data?.ChatDurationHistoryId ?? 0,
          'Chat',
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
          if (result[0]?.length > 0) {
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

export async function sentimentalScore(data: any) {
  try {
    const body = {
      HistoryId: data?.HistoryId,
      ChatDurationHistoryId: data?.ChatDurationHistoryId ?? 0,
      channelType: data?.channelType,
      domainId: data?.domainId,
      ext: parseInt(data?.ext, 10) || 0
    };

    const response: any = await axios.post(
      process?.env?.Sentimental,
      body,
      {
        headers: { 'Content-Type': 'application/json' },
        timeout: 5000
      }
    );
    return response ?? {};
  } catch (error) {
    return {};
  }
}

// chat deflection
export async function chatDeflectionData(data: any) {
  try {
    const body = {
      HistoryId: data?.HistoryId,
      ChatDurationHistoryId: data?.ChatDurationHistoryId ?? 0,
      channelType: data?.channelType,
      domainId: data?.domainId,
      ext: data?.ext ?? 0
    };
    console.log(' chat deflection::', body);
    return await axios.post(process?.env?.Deflection, body);
  } catch (error) {
    console.error('Error sending message:', error);
    // You can handle the error here, e.g., log it or return a specific response
  }
}

export const updateChatDeflectionChat = (data: any, chatDeflection: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_updateChatDeflection(?,?,?)',
        [
          data?.HistoryId ?? null,
          data?.ChatDurationHistoryId ?? 0,
          chatDeflection ?? 0,
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
export const updateAgentDetailsFB = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_UpdateFacebookHistory(?,?,?,?,?,?,?,?,?)', [
          data?.FacebookHistoryId,
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
export const getExt = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      if (data?.domainId) {
        const mysqlq = `select distinct(ext) from tb_User where roleid in (3,5) and domainId = ${data?.domainId}`;
        mysqlPoolConnection.query(mysqlq, (err: any, result: unknown) => {
          if (err) {
            reject(err);
          } else {
            resolve(result);
          }
        });
      }else {
        reject();
      }
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
export const updateDispositionDiscnnectFB = (data: any, closedDispositionsId: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_UpdateFacebookHistoryDisposition(?,?,?,?,?,?,?)',
        [
          data?.historyId,
          data?.domainId,
          closedDispositionsId?.did ?? null,
          closedDispositionsId?.dispositionName ?? null,
          1,
          data?.afterFacebookWorkTime ?? null,
          data?.dispositionUpdatedBy ?? null,
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
      console.log(err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateDispositionBotFB = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_FacebookIsBotUpdate(?)',
        [
          data?.historyId
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
      console.log(err);
      reject(err);
    }
  });
};

// senti mental Score facebook
export async function sentimentalScoreDataFB(data: any) {
  try {
    const body = {
      HistoryId: data?.historyId,
      ChatDurationHistoryId: data?.ChatDurationHistoryId ?? 0,
      channelType: data?.channelType,
      domainId: data?.domainId,
      ext: parseInt(data?.ext, 10) ?? 0
    };
    return await axios.post(process?.env?.Sentimental, body);
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
export const updateSentimentScoreFB = (data: any, sentiment: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_updateSentimentalScore(?,?,?,?,?,?,?,?)',
        [
          data?.historyId ?? 0,
          data?.ChatDurationHistoryId ?? 0,
          'Facebook',
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
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getWebchatLiveDashborad = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_chat_dashboard (?,?,?,?)',
        [
          data.startTmpstmp ?? null,
          data.endTmpstmp ?? null,
          data.domainId ?? null,
          parseInt(data.ext, 10) ? parseInt(data.ext, 10) : null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          } else {
            if (result && result.length > 0) {
              resolve(result);
            } else {
              reject(err);
            }
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
export const updateFacebookMessage = (data: any) => {
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
      console.log(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertUpdateChatIntentDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_InsertOrUpdateChatIntent(?,?,?,?,?)',
        [
          data.IntentName,
          data.IntentCount,
          data.domainId,
          data.ChatFlowId,
          data.ChatHistoryId,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log(err);
          }
          if (result && result[0]?.length > 0) {
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

export const getDurationHistoryIdDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        `select CallDurationHistoryId from tb_CallDurationHistory where CallHistoryId = ${data?.HistoryId} and AgentId = ${data?.userId}`,
        (err: any, result: any) => {
          if (err) {
            reject(err);
          } else {
            resolve(result[0]?.CallDurationHistoryId);
            console.log('result--->>', result);
          }
        }
      );
    } catch (error) {
      console.log(error);
    }
  });
};

export const insertUpdateAgentAssist = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_insert_update_chatAgentAssistMessage(?,?,?,?,?,?,?)',
        [
          parseInt(data?.HistoryId, 10) ?? null,
          data?.ChannelType === 'Call' ? data?.CallDurationHistoryId : data?.DurationHistoryId ?? null,
          data?.domainId ?? null,
          JSON.stringify(data?.AssistMsg) ?? null,
          data?.NoOfUsedAssistMsg ?? null,
          data?.totalNoOfassist ?? null,
          data?.ChannelType ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log(err);
          } else {
            resolve(result);
            console.log(result);
          }
        }
      );
    } catch (err) {
      reject(err);
      console.log(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const UpdateCallAnsweredDurationDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    const answeredDuration = typeof data.answeredDuration === 'string' ? data.answeredDuration.trim() || 0 : data.answeredDuration || 0;
    try {
      mysqlPoolConnection.query(
        'call ccaas_updateCallAnsweredDuration(?,?,?,?)',
        [
          data.CallHistoryId,
          data.domainId,
          data.AgentId,
          answeredDuration,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log(err);
          }
          if (result && result[0]?.length > 0) {
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
export const updateWhatsappChatMessage = (data: any) => {
  const message: any = JSON.stringify(data.message);
  const chat_id = parseInt(data?.historyId, 10);
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
      console.log(error);
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
          data?.historyId,
          'Whatsapp'
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
      console.log(err);
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
          data?.historyId,
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

export const audioMsg = async (data: any) => {
  try {

    return new Promise((resolve, reject) => {

      if (data?.ext) {
        mysqlPoolConnection.query(
          'CALL ccaas_call_create_message_details(?,?,?,?)', [
            data?.domainId,
            data?.sessionId,
            data?.message ?? null,
            data?.ext ?? null
          ],
          (err: any, response: any) => {

            if (err) {
              console.log('audioMsg reject error: ', err);
              reject(err);
            } else {
              resolve(response);
            }
          }
        );
      }
    });
  } catch (error) {
    console.log('audioMsg catch err: ', error);
  }
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
export const getCustomerCoIdFb = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_coid_by_customerId(?,?,?)',
        [
          data?.customerNumber,
          data?.domainId,
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
      console.log('getDispositinStatus catch err: ', err);
      reject(err);
    }
  });
};

// send Button fb
export async function sendCallbackRequestFb(req: any, buttonLink: any) {
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
    return await axios.post(process.env.postbackButton, body);
  } catch (error) {
    console.error('Error sending message:', error);
  }
}

/**
 *
 * @param domainId
 * @returns
 */
export const updateCallbackStatusFb = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_update_facebook_callback_status(?,?)',
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

/**
 *
 * @param data
 * @returns
 */
export const updateChatMessageFb = (data: any) => {
  const message: any = JSON.stringify(data.message);
  const chat_id = parseInt(data?.historyId, 10);
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
      console.log(error);
    }
  });
};

// send message fb
export async function sendMessageFb(req: any) {
  try {
    const body = {
      domainId: req?.domainId,
      customerId: req?.customerNumber,
      pageId: req?.agentNumber,
      message: 'Our agents are currently not available. Please feel free to leave a message with a brief summary of your inquiry along with your contact information, or you can get in touch with us at a later time.'
    };
    const response = await axios.post(process.env.FacebookOut, body);
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
export const updateMissedChatFb = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_update_socialMedia_missed_dtl(?,?,?)',
        [
          data?.domainId,
          data?.historyId,
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
      console.log(err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertUpdateVisitorDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_InsertUpdateVisitorInsight(?,?,?,?,?,?,?,?,?,?)',
        [
          data?.visitorId ?? null,
          data?.domainId || null,
          data?.customerId ?? null,
          data?.sessionStartTime ?? null,
          data?.sessionEndTimed ?? null,
          data?.location ?? null,
          data?.device ?? null,
          data?.ipAddress ?? null,
          data?.browser ?? null,
          data?.language ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result);
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
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateQueueIdFacebook = (data: any, queueId: any) => {
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
      reject(err);
    }
  });
};

/**
 *
 * @param data
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
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCustomerConatctDetails = (domainId: any, coid: any) => {
  return new Promise((resolve, reject) => {
    try {
      if (coid === null && coid === undefined) {
        reject(new Error('coid cannot be null'));
      } else {
        mysqlPoolConnection.query(
          'CALL ccaas_getCustomerConatctDetails(?,?)',
          [
            coid ?? 0,
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
      }
    } catch (err) {
      reject(err);
    }
  });
};

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
            console.log('getAllQueueWaitTime err: ', err);
            reject(err);
          }
          resolve(result[0]);
        }
      );
    } catch (error) {
      console.log('getAllQueueWaitTime catch error: ', error);
      reject(error);
    }
  });
};

export const insertUpdateQueueWaitTimeDao = (data: any) => {
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
        (err: any, result: any) => {
          if (err) {
            reject(err);
          }
          ioredisChat.emit('chatQueuePosition', data);
          resolve(result);
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
export const insertUpdateQueueWaitTimeWhatsapp = (data: any) => {
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
        (err: any, result: any) => {
          if (err) {
            reject(err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      reject(error);
    }
  });
};

/**
 * @description get queue wait time
 * @param data
 * @returns
 */

export const getQueueWaitTime = (data: any): Promise<IQueueWaitTime[]> => {
  return new Promise((resolve, reject) => {
    try {
      const mysqlq = `select * from queueWeighTime where qid = '${data.qid}' AND domainId = ${data.domainId} AND queueType='chat'`;
      mysqlPoolConnection.query(mysqlq, (err: any, result: any) => {
        if (err) {
          reject(err);
        } else {
          resolve(result as IQueueWaitTime[]);
        }
      });
    } catch (error) {
      reject(error);
    }
  });
};

/**
 * @description get queue wait time
 * @param data
 * @returns
 */

export const getQueueWaitTimeWhatsapp = (
  data: any
): Promise<IQueueWaitTime[]> => {
  return new Promise((resolve, reject) => {
    try {
      const mysqlq = `select * from queueWeighTime where qid = '${data.qid}' AND domainId = ${data.domainId} AND queueType='whatsapp'`;
      mysqlPoolConnection.query(mysqlq, (err: any, result: any) => {
        if (err) {
          reject(err);
        } else {
          resolve(result as IQueueWaitTime[]);
        }
      });
    } catch (error) {
      reject(error);
    }
  });
};

/**
 * @description get queue wait time
 * @param data
 * @returns
 */

export const getQueueWaitTimeFacebook = (
  data: any
): Promise<IQueueWaitTime[]> => {
  return new Promise((resolve, reject) => {
    try {
      const mysqlq = `select * from queueWeighTime where qid = '${data.qid}' AND domainId = ${data.domainId} AND queueType='facebook'`;
      mysqlPoolConnection.query(mysqlq, (err: any, result: any) => {
        if (err) {
          reject(err);
        } else {
          resolve(result as IQueueWaitTime[]);
        }
      });
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
export const getAgentAssistMessageDtl = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_agentAssistMessage_dtl(?,?,?)',
        [
          data?.HistoryId ?? null,
          data?.DurationHistoryId ?? null,
          data?.domainId ?? null,
        ],
        (err: any, result: any) => {
          console.log('result>>>', result);

          if (err) {
            reject(err);
          } else {
            resolve(result);
          }
        });
    } catch (error) {
      reject(error);
    }
  });
};

/**
 * @description get queue wait time
 * @param data
 * @returns
 */

export const getQueueWaitTimeInsta = (
  data: any
): Promise<IQueueWaitTime[]> => {
  return new Promise((resolve, reject) => {
    try {
      const mysqlq = `select * from queueWeighTime where qid = '${data.qid}' AND domainId = ${data.domainId} AND queueType='instagram'`;
      mysqlPoolConnection.query(mysqlq, (err: any, result: any) => {
        if (err) {
          reject(err);
        } else {
          resolve(result as IQueueWaitTime[]);
        }
      });
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
export const updateAssistMessage = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call update_assistMsg(?,?,?,?,?,?)',
        [
          data?.historyId ?? null,
          data?.durationHistoryId ?? null,
          data?.domainId ?? null,
          data?.assistMsg ?? null,
          data?.noOfUsedAssistMsg ?? null,
          data?.totalNoOfassist ?? null
        ],
        (err: any, result: any) => {

          if (err) {
            reject(err);
          } else {
            resolve(result);
          }
        });
    } catch (error) {
      reject(error);
    }
  });
};
