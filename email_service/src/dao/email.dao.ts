import axios from 'axios';
import moment from 'moment';
import { v4 as uuidv4 } from 'uuid';
import { logger } from '../../src/plugins/log';
import { CHANNEL_TYPE, ROLES } from '../helpers/constants';
import { mysqlEmailConnection, mysqlMyaccPoolConnection, mysqlPoolConnection } from '../plugins/db';

/**
 *
 * @param qdata
 * @returns
 */
export const createEmailConfigDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_create_email_config_details(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.EmailConfigId ?? 0,
          data?.emailAddress ?? null,
          data?.aliasName ?? null,
          data?.supervisorName ?? null,
          data?.emailThreshold ?? null,
          data?.isEmail ?? null,
          data?.domainId,
          data?.primaryEmail ?? null,
          data?.isBot ?? null,
          data?.knowledgeBasedId ?? null,
          data?.autoAssignAgent ?? null,
          data?.serverType ?? null,
          data?.productId ?? null,
          data?.emailType ?? null,
          data?.domainName ?? null,
          data?.forwardEmail ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          }
          resolve(result[0]);
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
export const getEmailConfigDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_emailConfig(?)',
        [
          data.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          }
          if (result && result?.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
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
 * @param qdata
 * @returns
 */
export const getEmailAddress = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_getEmailAddres(?)',
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
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const emailDeleteFunction = (data: any, emailConfigId: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_getEmailAddres(?,?)',
        [
          emailConfigId ?? 0,
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
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @param EmailHistoryId
 * @param sessionId
 * @returns
 */
export const createEmailSessionHistory = (data: any, EmailHistoryId: any, sessionId: any) => {
  return new Promise(async (resolve, reject) => {
    const message = JSON.stringify(data?.message);
    try {
      mysqlPoolConnection.query(
        'call up_InsertUpdateEmailHistory(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          EmailHistoryId ?? 0,
          sessionId ?? null,
          data?.sessionStartTime ?? null,
          data?.SessionEndTime ?? null,
          data?.FromEmail ?? null,
          data?.ToEmail ?? null,
          data?.EmailSubject ?? null,
          data?.EmailFlowSourceId ?? null,
          data?.CompanyId ?? null,
          data?.domainId ?? null,
          data?.DispositionId ?? null,
          data?.Summary ?? null,
          data?.IsInBound ?? null,
          data?.AssignedTo ?? null,
          data?.AssignedBy ?? null,
          data?.AssignedOn ?? null,
          data?.AfterEmailWorkTime ?? null,
          data?.Istransferred ?? null,
          data?.TransferredTypeId ?? null,
          data?.IsSessionClosed ?? null,
          data?.CustomerId ?? null,
          data?.TicketId ?? null,
          data?.SupervisorName ?? null,
          message ?? null,
          data?.AgentId ?? null,
          data?.AgentSkillId ?? null,
          data?.IsBot ?? null,
          data?.BotId ?? null,
          data?.QueueId ?? null,
          data?.messageId ?? null,
          data?.graphMessageId ?? null,
          data?.conversationId ?? null
        ],
        (err: any, result2: any) => {
          if (err) {
            reject(err);
          }
          if (result2 && result2.length > 0) {
            resolve(result2[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (error) {
      logger.info(error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @param sendMessage
 * @returns
 */
export const createEmailSessionHistoryOutbound = (data: any, sendMessage: any) => {
  return new Promise(async (resolve, reject) => {
    const message = JSON.stringify(sendMessage?.message);
    try {
      mysqlPoolConnection.query(
        'call up_InsertUpdateEmailHistory(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.EmailHistoryId ?? 0,
          data?.sessionId ?? null,
          data?.sessionStartTime ?? null,
          data?.SessionEndTime ?? null,
          data?.ToEmail ?? null,
          data?.FromEmail ?? null,
          data?.EmailSubject ?? null,
          data?.EmailFlowSourceId ?? null,
          data?.CompanyId ?? null,
          data?.domainId ?? null,
          data?.DispositionId ?? null,
          data?.Summary ?? null,
          data?.IsInBound ?? null,
          data?.AssignedTo ?? null,
          data?.AssignedBy ?? null,
          data?.AssignedOn ?? null,
          data?.AfterEmailWorkTime ?? null,
          data?.Istransferred ?? null,
          data?.TransferredTypeId ?? null,
          data?.IsSessionClosed ?? null,
          data?.CustomerId ?? null,
          data?.TicketId ?? null,
          data?.SupervisorName ?? null,
          message ?? null,
          data?.AgentId ?? null,
          data?.AgentSkillId ?? null,
          data?.IsBot ?? null,
          data?.BotId ?? null,
          data?.QueueId ?? null,
          data?.messageId ?? null,
          data?.graphMessageId ?? null,
          data?.conversationId ?? null
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
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @param sendMessage
 * @returns
 */
export const sendEmailOutboundCompany = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_InsertUpdateEmailHistory(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.EmailHistoryId ?? null,
          data?.sessionId,
          data?.sessionStartTime,
          data?.SessionEndTime,
          data?.ToEmail,
          data?.FromEmail,
          data?.EmailSubject,
          data?.EmailFlowSourceId,
          data?.CompanyId,
          data?.domainId,
          data?.DispositionId,
          data?.Summary,
          data?.IsInBound,
          data?.AssignedTo,
          data?.AssignedBy,
          data?.AssignedOn,
          data?.AfterEmailWorkTime,
          data?.Istransferred,
          data?.TransferredTypeId,
          data?.IsSessionClosed,
          data?.CustomerId,
          data?.TicketId,
          data?.SupervisorName,
          '[]',
          data?.AgentId,
          data?.AgentSkillId,
          data?.IsBot,
          data?.BotId,
          data?.QueueId,
          data?.messageId ?? null,
          data?.graphMessageId ?? null,
          data?.conversationId ?? null
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
      reject(error);
    }
  });
};

/**
 * Extract email address from various formats:
 * - "Name <email@example.com>" -> "email@example.com"
 * - "email@example.com" -> "email@example.com"
 * - "<email@example.com>" -> "email@example.com"
 */
function extractEmailAddress(value: string | undefined): string {
  if (!value) return '';
  const trimmed = value.trim();
  const match = trimmed.match(/<([^>]+)>/);
  return (match ? match[1] : trimmed).trim().toLowerCase();
}

/**
 *
 * @param data
 * @returns
 */
export const checkExistUser = (data: any) => {
  const extractedEmail = extractEmailAddress(data?.toData);
  const FromEmail = extractEmailAddress(data?.fromData);
  const EmailSubject = data?.subjectData;
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call GetToCheckExistingEmailHistory(?,?,?)',
        [
          FromEmail ?? null,
          extractedEmail ?? null,
          EmailSubject ?? null,
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
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const findEmailRoutedQueue = async (data: any) => {
  return new Promise((resolve, reject) => {
    let email: any;
    const field = data?.toData ?? null;
    if (field.includes('<') && field.includes('>')) {
      const startIndex = field.indexOf('<') + 1;
      const endIndex = field.indexOf('>');
      email = field.substring(startIndex, endIndex);
    } else {
      email = field;
    }
    try {
      mysqlPoolConnection.query(
        'call ccaas_getknowledgeBasedIdByEmailId(?)',
        [
          email ?? null
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
      reject(err);
      return err;
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const setPrimaryEmailDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_SetPrimaryEmail(?,?)',
        [
          data?.domainId,
          data?.EmailConfigId
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
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const deleteEmailAddressDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_DeletePrimaryEmail(?,?)',
        [
          data?.domainId,
          data?.EmailConfigId
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
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getSupervisorDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_supervisor_list(?)',
        [
          data?.domainId,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          } else {
            resolve(result[0]);
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
export const getMultipleEmail = () => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetAllEmailAddress()',
        (err: any, result: any) => {
          if (err) {
            reject(err);
          }
          resolve(result?.length > 0 ? result[0] : []);
        }
      );
    } catch (error) {
      reject(error);
    }
  });
};

/**
 *
 * @param sessionId
 * @returns
 */
export const getSessionHistory = (sessionId: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_getAssignToDetails(?)',
        [
          sessionId ?? null
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
      reject(err);
    }
  });
};

/**
 *
 * @param qdata
 * @returns
 */
export const getPersonalDetails = (email: any, domainId: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_getPersonalDetails(?,?)',
        [
          email,
          domainId
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          } else if (result && result[0]) {
            resolve(result[0]);
          } else {
            reject(new Error('No personal details found'));
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
export const getEmailextList = (UserId: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetUserExt(?)',
        [
          UserId
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          }
          resolve(result[0]);
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
export const getEmailHistory = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccass_getDetailsBySessionId(?,?,?)',
        [
          data.domainId,
          data?.channelType ?? null,
          data?.EmailSessionId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          }
          resolve(result[0]);
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
export const getEmailHistoryDetails = (session_Id: any, DomainId: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccass_getDetailsBySessionId(?,?,?)',
        [
          DomainId,
          CHANNEL_TYPE?.EMAIL,
          session_Id ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          }
          resolve(result[0]);
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
export const updateDispositionForEmail = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_UpdateEmailHistoryDisposition(?,?,?,?,?,?,?,?,?,?)',
        [
          data?.EmailHistoryId,
          data?.domainId,
          data?.dispositionId ?? null,
          data?.summary ?? null,
          data?.isSessionClosed ?? 0,
          data?.AfterEmailWorkTime ?? null,
          data?.dispositionUpdatedBy ?? null,
          data?.duration ?? null,
          data?.dispositionSubject ?? null,
          data?.tags,
        ],
        (err: any, result: any) => {
          if (err) {
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
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateEmailBot = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_update_email_bot_status(?,?,?,?)',
        [
          data?.domainId,
          data?.isBot ?? 0,
          data?.isBotPurchased ?? 0,
          data?.emailAddress ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          }
          resolve(result[0]);
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
export const onbordUpdateEmailBot = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_update_email_bot_status_onboard(?,?,?)',
        [
          data?.domainId,
          data?.isBot ?? 0,
          data?.isBotPurchased ?? 0
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          }
          resolve(result[0]);
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
          }
          resolve(result[0]);
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
export const getSupervisiorEmitter = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_supervisor_ext(?)',
        [
          data?.DomainId
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
          data?.DomainId
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
 *
 * @param data
 * @returns
 */
export const autoAssignAgent = (data: any, ext: any, queue: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_AutoAssignAgentToEmail(?,?,?,?)',
        [
          data?.EmailHistoryId ?? 0,
          data?.DomainId,
          ext ?? null,
          Number(queue?.qId) ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          }
          if (result && result?.length > 0) {
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
export const updateSentimentScore = (data: any, sentiment: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_updateSentimentalScore(?,?,?,?,?,?,?,?)',
        [
          data?.EmailHistoryId ?? 0,
          data?.ChatDurationHistoryId ?? 0,
          CHANNEL_TYPE?.EMAIL,
          data?.domainId ?? null,
          sentiment?.sentimentalScore ?? null,
          sentiment?.sentimentalScoreUser ?? null,
          sentiment?.sentimentalScoreAgent ?? null,
          data?.ext ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          }
          resolve(result[0]);
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
export const updatesentimentScoreBot = (data: any, sentiment: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_updateSentimentalScore(?,?,?,?,?,?,?,?)',
        [
          data?.EmailHistoryId ?? 0,
          data?.ChatDurationHistoryId ?? 0,
          CHANNEL_TYPE?.EMAIL,
          data?.domainId ?? null,
          sentiment ?? null,
          sentiment?.sentimentalScoreUser ?? null,
          sentiment?.sentimentalScoreAgent ?? null,
          data?.ext ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          }
          resolve(result[0]);
        }
      );
    } catch (err) {
      reject(err);
    }
  });
};

/**
 *
 * @param domainId
 * @returns
 */
export const getKnowledgeBaseId = (domainId: any, knowledgeBaseType: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_CategoryArticleCount(?,?)',
        [
          domainId,
          knowledgeBaseType ?? null
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
      reject(err);
    }
  });
};

/**
 *
 * @param domainId
 * @returns
 */
export const getKnowledgeBaseIdNew = (domainId: any) => {
  return new Promise(async (resolve, reject) => {

    try {
      mysqlPoolConnection.query(
        'call up_GetCompanyDomain(?)',
        [
          domainId,
        ],
        (err: any, result: any) => {
          console.log('getWebsite------', result);
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
 * @param qdata
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
 *
 * @param data
 * @returns
 */
export const getEmailFlowHistoryList = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetEmailFlowHistoryList(?)',
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
export const createCrmEmailConfigDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_crm_create_email_config_details(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.emailConfigId ?? 0,
          data?.emailAddress ?? null,
          data?.aliasName ?? null,
          data?.supervisorName ?? null,
          data?.emailThreshold ?? null,
          data?.isEmail ?? null,
          data?.domainId ?? null,
          data?.primaryEmail ?? null,
          data?.isBot ?? null,
          data?.knowledgeBasedId ?? null,
          data?.autoAssignAgent ?? null,
          data?.product_id ?? null,
          data?.is_smtp ?? null,
          data?.smtp_json ?? null,
          data?.emailtype ?? null,
          data?.isRepliedEmail ?? null,
          data?.repliedEmail ?? null
        ],
        (err: any, result: any) => {
          if (err) {
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
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertDomainConfiguration = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_insert_email_domain_configuration(?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.domainId ?? null,
          data?.companyId ?? null,
          data?.domainName ?? null,
          data?.status ?? null,
          data?.remarks ?? null,
          data?.host1 ?? null,
          data?.value1 ?? null,
          data?.host2 ?? null,
          data?.value2 ?? null,
          data?.host3 ?? null,
          data?.value3 ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          }
          resolve(result?.[0]);
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
export const getDomainConfigurationList = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_email_domain_configuration(?)',
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
export const deleteDomainDetails = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_delete_email_domain_configuration(?,?)',
        [
          data.id,
          data.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
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
export const getDomainConfigurationDetails = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_specific_email_domain_configuration(?,?)',
        [
          data.id,
          data.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
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
export const getDomainAlreayExist = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_check_email_domain_name(?,?)',
        [
          data.domainId,
          data.domainName
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
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
export const updateDomainVerification = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_update_email_domain_status(?,?,?)',
        [
          data.id,
          data.domainId,
          data.status
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
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
export const createDomain = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlEmailConnection.query(
        'call ccaas_domain_create_info(?,?,?,?,?,?,?,?,?,?,?)',
        [
          data.domainName,
          data.description,
          data.aliases,
          data.mailboxes,
          data.maxquota,
          data.quota,
          data.transport,
          data.backupmx,
          data.active,
          data.passwordExpiry,
          data.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
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
export const createMailBox = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlEmailConnection.query(
        'call ccaas_domain_mailbox_create_info(?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.domainName,
          data.username,
          data.password,
          data.name,
          data.maildir,
          data.quota,
          data.localPart,
          data.active,
          data.phone,
          data.emailOther,
          data.token,
          data.processtype
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
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
export const getVerificationDomainList = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetVerifiedDomainList(?)',
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
export const getSmptList = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      console.log([
        data?.ToEmail
      ]);
      mysqlPoolConnection.query(
        'call up_GetEmaildetails(?)',
        [
          data?.ToEmail
        ],
        (err: any, result: any) => {
          if (err) {
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
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getSmptSend = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetEmaildetails(?)',
        [
          data?.FromEmail
        ],
        (err: any, result: any) => {
          if (err) {
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

/**
 *
 * @param data
 * @returns
 */
export const insertCustomerContactPhone = (data: any) => {
  const Email = data?.FromEmail;
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
          data?.domainId,
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
          CHANNEL_TYPE?.EMAIL,
          data?.customField ?? null,
          data?.customValues ?? null,
          data?.primaryPhoneNumber ?? null,
          Email
        ],
        (err: any, result: any) => {
          if (err) {
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
          data?.EmailHistoryId,
          CHANNEL_TYPE?.EMAIL
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
          if (err) {
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

/**
 *
 * @param domainId
 * @param channelType
 * @returns
 */
export const getDomainLevelKnowledgeBaseId = (domainId: any, channelType: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_domainLevel_knowledgeBaseId(?,?)',
        [
          domainId,
          channelType
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
      reject(err);
    }
  });
};

/**
 *
 * @param domainId
 * @returns
 */
export const getKnowledgeBase = (domainId: any, knowledgeBaseId: any) => {
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
export const updateReplyViaDispositionForEmail = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_UpdateReplayViaEmailHistoryDisposition(?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.EmailHistoryId,
          data?.domainId,
          data?.dispositionId ?? null,
          data?.summary ?? null,
          data?.isSessionClosed ?? 0,
          data?.AfterEmailWorkTime ?? null,
          data?.dispositionUpdatedBy ?? null,
          data?.duration ?? null,
          data?.replyChannelType ?? null,
          data?.responseHistoryId ?? null,
          data?.coid ?? null,

        ],
        (err: any, result: any) => {
          if (err) {
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
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertUnsubscribeDetails = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_insert_email_subscription(?,?,?)',
        [
          data?.from ?? null,
          data?.to ?? null,
          data?.isSubscribed ?? null
        ],
        (err: any, result: any) => {
          if (err) {
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
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getUnsubscribeUser = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_email_subscription_check(?,?)',
        [
          data?.FromEmail ?? null,
          data?.ToEmail ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
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
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertTicketingEmailConfigDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_TicketCreateEmailConfig(?,?,?,?,?,?)',
        [
          data?.EmailConfigId ?? null,
          data?.emailAddress ?? null,
          data?.domainId ?? null,
          data?.companyId ?? null,
          data?.is_smtp ?? null,
          data?.emailtype ?? null
        ],
        (err: any, result: any) => {
          if (err) {
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
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const deleteTicketingEmailConfigDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_deleteTicketEmailConfig(?)',
        [
          data?.EmailConfigId ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
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
  try {
    const domainId = data?.domainId ?? 0;
    mysqlPoolConnection.query(
      'call ccaas_get_connector_token_info(?)',
      [
        domainId,
      ],
      (error: any, results: any) => {
        if (error) {
          logger.error('getConnectorTokenInfo - Unexpected Error:', error);
          callback([]);
          return;
        }
        const result = Array.isArray(results) && results.length > 0 ? results[0] : [];
        callback(result);
      });
  } catch (error) {
    logger.error('getConnectorTokenInfo - Unexpected Error:', error);
  }
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
        [data?.DomainId ?? 0],
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
export const getSessionDetails = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlMyaccPoolConnection.query(
        'call ur_get_plan_details_by_email(?)',
        [
          data?.ToEmail ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
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
          data?.domain_id
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
 *
 * @param data
 * @returns
 */
export const UpdateEmailSentForSessions = (data: any, countData: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_UpdateEmailSentForSessions(?,?)', [
          countData?.domain_id,
          data?.isEmailSent
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
      logger.error('UpdateEmailSentForSessions err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const UpdateEmailSentForPlans = (countData: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_UpdateEmailSentForSessions(?,?)', [
          countData?.domain_id,
          '1'
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
      logger.error('UpdateEmailSentForPlans err', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getMicrosoftDBdata = (tenantId:any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_email_config_details_for_microsoft(?)',
        [
          tenantId
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0] || []);
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
