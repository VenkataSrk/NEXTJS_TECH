import { v4 as uuidv4 } from 'uuid';
import { configs } from '../../config/app';
import {
  assignCallBack,
  autoAssignAgent,
  checkExistUser,
  createCrmEmailConfigDao,
  createDomain,
  createEmailConfigDao,
  createEmailSessionHistory,
  createEmailSessionHistoryOutbound,
  createMailBox,
  deleteDomainDetails,
  deleteEmailAddressDao,
  deleteTicketingEmailConfigDao,
  emailDeleteFunction,
  findEmailRoutedQueue,
  getCompanyDetails,
  getConnectorTokenInfo,
  getCustomerSatisficationDao,
  getDispositinStatus,
  getDomainAlreayExist,
  getDomainConfigurationDetails,
  getDomainConfigurationList,
  getDomainLevelKnowledgeBaseId,
  getEmailAddress,
  getEmailConfigDao,
  getEmailextList,
  getEmailFlowHistoryList,
  getEmailHistory,
  getEmailHistoryDetails,
  getKnowledgeBase,
  getKnowledgeBaseId,
  getKnowledgeBaseIdNew,
  getMicrosoftDBdata,
  getMultipleEmail,
  getPersonalDetails,
  getRemainingSessionEmailSent,
  getSessionDetails,
  getSmptList,
  getSmptSend,
  getSupervisiorEmitter,
  getSupervisiorList,
  getSupervisorDao,
  getUnsubscribeUser,
  getVerificationDomainList,
  insertCustomerContactPhone,
  insertDomainConfiguration,
  insertTicketingEmailConfigDao,
  insertUnsubscribeDetails,
  onbordUpdateEmailBot,
  sendEmailOutboundCompany,
  setPrimaryEmailDao,
  updateCustomerId,
  updateDispositionForEmail,
  updateDomainVerification,
  updateEmailBot,
  updateReplyViaDispositionForEmail,
  updateSentimentScore,
  updatesentimentScoreBot,
} from '../../dao/email.dao';
import { CHANNEL_TYPE, emitters, RESPONSE } from '../../helpers/constants';
import { createAndInsertNotification, handleCustomerSatisfaction } from '../../helpers/customerSatisfication/utils';
import { agentAssistChannel, callGoogleSheetSyncAPI, getEmailBot, lookupCNAME, lookupSPFRecord, sentimentalScore, verifyDkim } from '../../helpers/utils';
import { ioredisChat, otherRedisClient } from '../../plugins/db';
import { logger } from '../../plugins/log';
import { sendMailAudracare } from '../../service/audracareSmtp';
import { imapSmtpInboxHandler } from '../../service/imapSmtp';
import { imapInboxDomianHandler } from '../../service/imapTech';
import { sendEmailSessionExpiry, sendMailForLead, sendMailPlainText, sendMailRouteMobile, sendMailTemplate } from '../../service/leadsmtp';
import { ensureMicrosoftSubscription, fetchMicrosoftInbox } from '../../service/microsoftGraph';
import { sendMailGraph } from '../../service/microsoftGraphSend';
import { sendMail } from '../../service/smtp';
import { sendMailSmtp } from '../../service/smtpNew';
const forge = require('node-forge');
const argon2 = require('argon2');

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function createEmailConfiguration(req: any, res: any) {
  try {
    const data: any = {};
    const auth: any = req.headers;
    data.EmailConfigId = req?.body?.emailConfigId,
      data.emailAddress = req?.body?.emailId ?? null,
      data.aliasName = req?.body?.aliasName ?? null,
      data.supervisorName = req?.body?.supervisorName ?? null,
      data.emailThreshold = req?.body?.emailThreshold ?? null,
      data.isEmail = req.body?.isEmail ?? null,
      data.domainId = auth.domainId;
    data.primaryEmail = req?.body?.primaryEmail ?? null,
      data.isBot = req?.body?.isBot ?? null,
      data.knowledgeBasedId = req?.body?.knowledgeBasedId ?? null,
      data.autoAssignAgent = req?.body?.autoAssignAgent ?? null,
      data.serverType = req?.body?.serverType ?? null,
      data.emailType = req?.body?.emailType ?? null,
      data.domainName = req?.body?.domainName ?? null,
      data.forwardEmail = req?.body?.forwardEmail ?? null;
    const emailData = await createEmailConfigDao(data);
    logger.info('createEmailConfigDao', emailData);
    if (emailData[0]?.errmsg === 'inserted successfully') {
      res.status(200).send({ statusCode: 200, message: RESPONSE.email_insert });
    } else if (emailData[0].errcode === -1) {
      res.status(200).send({ statusCode: 404, message: RESPONSE.email_duplicating });
    } else {
      res.status(200).send({ statusCode: 200, message: RESPONSE.email_update });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('createEmailConfiguration - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function createEmailOnboarding(req: any, res: any) {
  try {
    const data: any = {};
    data.EmailConfigId = req?.body?.emailConfigId,
      data.emailAddress = req?.body?.emailId ?? null,
      data.aliasName = req?.body?.aliasName ?? null,
      data.supervisorName = req?.body?.supervisorName ?? null,
      data.emailThreshold = req?.body?.emailThreshold ?? null,
      data.isEmail = req?.body?.isEmail ?? null,
      data.domainId = req?.body?.domainId;
    data.primaryEmail = req?.body?.primaryEmail ?? null,
      data.isBot = req?.body?.isBot ?? null,
      data.knowledgeBasedId = req?.body?.knowledgeBasedId ?? null,
      data.autoAssignAgent = req?.body?.autoAssignAgent ?? null;
    data.serverType = req?.body?.serverType ?? null;
    data.productId = req?.body?.productId ?? null;
    data.emailType = req?.body?.emailType ?? null;
    const emailData = await createEmailConfigDao(data);
    logger.info('createEmailConfigDao', emailData);
    if (emailData[0]?.errmsg === 'inserted successfully') {
      res.status(200).send({ statusCode: 200, message: RESPONSE.email_insert });
    } else if (emailData[0].errcode === -1) {
      res.status(200).send({ statusCode: 404, message: RESPONSE.email_duplicating });
    } else {
      res.status(200).send({ statusCode: 200, message: RESPONSE.email_update });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('createEmailOnboarding - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getEmailConfiguration(req: any, res: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
    };
    const resultEmail: any = await getEmailConfigDao(data);
    logger.info('getEmailConfigDao', resultEmail);
    if (resultEmail.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: resultEmail });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getEmailConfiguration - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function onbordGetEmailConfiguration(req: any, res: any) {
  try {
    const data: any = {
      domainId: req?.body?.domainId,
    };
    const resultEmail: any = await getEmailConfigDao(data);
    logger.info('getEmailConfigDao', resultEmail);
    if (resultEmail.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: resultEmail });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('onbordGetEmailConfiguration - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 *
 */

export const getDefaultEmailHandler = async (req: any, res: any, done: any) => {
  try {
    const auth: any = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    const getEmail: any = await getEmailAddress(data);
    logger.info('getEmailAddress', getEmail);
    if (getEmail) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getEmailData: getEmail,
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getList: [] });
    }
  } catch (err) {
    req.log.error(err);
    logger.error('getDefaultEmailHandler - Unexpected Error:', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
};

/**
 *
 * @param req
 * @param res
 * @param done
 */

export const postDefaultEmailHandler = async (req: any, res: any, done: any) => {
  try {
    const auth: any = req.headers;
    const data: any = {
      emailAddress: req?.body?.emailId ?? null,
      aliasName: req?.body?.aliasName ?? null,
      supervisorName: req?.body?.supervisorName ?? null,
      emailThreshold: req?.body?.emailThreshold ?? 0,
      isEmail: req.body?.isEmail ?? 0,
      domainId: auth.domainId,
      primaryEmail: req?.body?.primaryEmail ?? 0,
    };
    const newEmail = await createEmailConfigDao(data);
    logger.info('createEmailConfigDao', newEmail);
    res.send({
      status_code: 200,
      err_code: -1,
      affected_rows: 1,
      message: RESPONSE.success_message,
      insertedEmail: newEmail,
    });
  } catch (err) {
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('postDefaultEmailHandler - Unexpected Error:', err);
  }
};

/**
 *
 * @param req
 * @param res
 * @param done
 */

export const deleteDefaultEmailHandler = async (req: any, res: any, done: any) => {
  try {
    const auth: any = req.headers;
    const uid: any = req.params.uid;
    const data: any = {
      domainId: auth.domainId,
    };
    const emailToDelete: any = await emailDeleteFunction(data, uid);
    logger.info('emailDeleteFunction', emailToDelete);
    if (emailToDelete) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.email_deleted_successfully });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('deleteDefaultEmailHandler - Unexpected Error:', err);
  }
};

setInterval(imapInboxDomianHandler, 600000);
imapInboxDomianHandler();

setInterval(imapSmtpInboxHandler, 600000);
imapSmtpInboxHandler();

const createMessageList = (list: any, includeIsBot = false, isBotValue = false) => {
  const message: any = {
    messageId: list?.messageId,
    sendAt: Date.now(),
    fromData: list?.fromData,
    toData: list?.toData,
    message: list?.textData,
    subject: list?.subjectData,
    attachments: list?.attachments,
  };
  if (includeIsBot) {
    message.isBot = isBotValue;
  }
  return {
    userText: { message },
  };
};

const extractEmail = (value: any) => {
  if (!value) return '';
  if (Array.isArray(value)) {
    for (const item of value) {
      const candidate =
        item?.emailAddress?.address || item?.address || item?.email || item?.value || item;
      const found = extractEmail(candidate);
      if (found) return found;
    }
    return '';
  }
  const str = `${value}`.trim();
  if (!str) return '';
  const first = str?.split(/[;,]/g)?.map((v) => {
    return v.trim();
  })?.filter(Boolean)[0] || '';

  const match = first.match(/<([^>]+)>/);
  return (match ? match[1] : first).trim();
};

const isMicrosoftProvider = (smtp: any) => {
  try {
    const jsonData = JSON.parse(smtp?.smtp_json || '{}');
    return jsonData?.provider === 'MICROSOFT';
  } catch (error) {
    logger.error('Invalid smtp_json:', error);
    return false;
  }
};

const sendMailByProvider = async (data: any, smtp: any, text: any) => {
  try {
    if (isMicrosoftProvider(smtp)) {
      await sendMailGraph(data, smtp, text);
    } else {
      await sendMailSmtp(data, smtp, text);
    }
  } catch (err: any) {
    logger.error('sendMailByProvider failed', {
      provider: isMicrosoftProvider(smtp) ? 'MICROSOFT' : 'SMTP',
      message: err?.message,
      status: err?.response?.status,
      data: err?.response?.data,
    });
    throw err;
  }
};

// create Email Session
export async function createEmailSession(imapInox: any) {
  try {
    const list: any = imapInox?.customerData;
    const routedQueue = await findEmailRoutedQueue(list);
    await checkExistUser(list).then(async (checkUser: any) => {
      logger.info({ checkUser });
      let session_Id: any;
      let historyId = null;
      if (checkUser?.length === 0 || routedQueue[0]?.isBot === 1) {
        if (checkUser?.length === 0) {
          session_Id = uuidv4();
        } else {
          session_Id = checkUser[0]?.SessionId;
          historyId = checkUser[0]?.EmailHistoryId;
        }
        const customerImapMsg: any = [];
        customerImapMsg.push(list.customerData);
        list.message = JSON.stringify(customerImapMsg);
        list.domainId = routedQueue[0]?.domainId;
        list.sessionId = session_Id;
        const messageList = createMessageList(list, false, routedQueue[0]?.isBot === 1);
        const toData = list?.toData;
        const to_email = toData.split('<');
        let extractedEmail = '';
        if (to_email?.length > 1) {
          extractedEmail = to_email[1]?.substring(0, to_email[1]?.length - 1);
        } else {
          extractedEmail = toData;
        }
        let fromData = list?.fromData.split('<');
        fromData = fromData[1]?.substring(0, fromData[1]?.length - 1);
        const messageData: any = {
          sessionStartTime: Math.floor(Date.now()),
          domainId: routedQueue[0]?.domainId,
          supervisorName: routedQueue[0]?.supervisorName,
          IsInBound: 1,
          customerData: list,
          ToEmail: extractedEmail,
          FromEmail: fromData,
          EmailSubject: list?.subjectData,
          message: messageList,
          messageId: list?.messageId,
          IsBot: 0,
          IsSessionClosed: 0
        };

        await createEmailSessionHistory(messageData, historyId, session_Id).then(async (res: any) => {
          const coid = await insertCustomerContactPhone(messageData);
          getConnectorTokenInfo(routedQueue[0], (getConnect) => {
            if (getConnect?.length > 0) {
              callGoogleSheetSyncAPI({
                contactSyncDetails: getConnect[0]?.contactSyncDetails,
                newContacts: coid[0]?.coid,
                domainId: routedQueue[0]?.domainId,
                token: getConnect[0]?.token
              });
            }
          });
          updateCustomerId(res[0], coid[0]);
          logger.info(res[0]?.errmsg);
          await createAndInsertNotification(res, coid[0]);

          const nlpResult = res[0];
          const comDetails = await getCompanyDetails(res[0]);
          const getEmailData: any = await getEmailBot(nlpResult, comDetails, list?.textData, routedQueue[0]);
          const text = getEmailData?.message ?? null;

          if (routedQueue[0]?.isBot === 0 && getEmailData?.key === 'human_agent') {
            try {
              const autoAssign = await autoAssignAgent(res[0], getEmailData?.ext, getEmailData);
              ioredisChat.to(`${autoAssign[0]?.domainId}_${autoAssign[0]?.ext}`).emit(emitters?.assignActivityListener);
              logger.info('assignActivityAgent', `${autoAssign[0]?.domainId}_${autoAssign[0]?.ext}`);
            } catch (error) {
              console.log(error);
            }

          } else if (routedQueue[0]?.isBot === 0 && getEmailData?.key === 'human_agent') {
            try {
              const autoAssign = await autoAssignAgent(res[0], getEmailData?.ext, getEmailData);
              ioredisChat.to(`${autoAssign[0]?.domainId}_${autoAssign[0]?.ext}`).emit(emitters?.assignActivityListener);
              logger.info('assignActivityAgent', `${autoAssign[0]?.domainId}_${autoAssign[0]?.ext}`);
            } catch (error) {
              console.log(error);
            }
          } else if (routedQueue[0]?.isBot === 1 && getEmailData?.key === 'human_agent') {
            try {
              const autoAssign = await autoAssignAgent(res[0], getEmailData?.ext, getEmailData);
              ioredisChat.to(`${autoAssign[0]?.domainId}_${autoAssign[0]?.ext}`).emit(emitters?.assignActivityListener);
              logger.info('assignActivityAgent', `${autoAssign[0]?.domainId}_${autoAssign[0]?.ext}`);
            } catch (error) {
              console.log(error);
            }
          } else if (routedQueue[0]?.isBot === 1 && (routedQueue[0]?.autoAssignAgent === 0 || routedQueue[0]?.autoAssignAgent === null)) {
            try {
              const getSmtp: any = await getSmptList(res[0]);
              if (getSmtp?.length > 0) {
                await sendMailByProvider(res[0], getSmtp[0], text);
              } else {
                await sendMail({
                  ...res[0],
                  text,
                  FromEmail: res[0]?.ToEmail,
                  ToEmail: res[0]?.FromEmail
                });
              }
              const getDispositin: any = await getDispositinStatus(nlpResult);
              const closedDispositions = getDispositin[0]?.filter((item: { dispositionName: string }) => item?.dispositionName === 'Closed');
              let message: any = {};
              try {
                message = {
                  to: nlpResult?.FromEmail,
                  from: nlpResult?.ToEmail,
                  messageID: uuidv4(),
                  sessionId: nlpResult?.SessionId,
                  message: text,
                  EmailSubject: nlpResult?.EmailSubject,
                  attachments: [],
                  sendAt: Date.now(),
                  messageType: 'text',
                  pdfurl: '',
                  isBot: true,
                  isCustomer: false,
                  fileType: '',
                  direction: 'OUT',
                  isLike: 0,
                  cc: '',
                  bcc: ''
                };
                const messageListData: any = { message };
                const messageType: any = { agentText: messageListData };
                const messageDatas: any = {
                  message: messageType,
                  IsBot: 1,
                  IsSessionClosed: 1,
                  DispositionId: closedDispositions[0]?.did,
                  Summary: 'Closed',
                  SessionEndTime: Math.floor(Date.now())
                };
                await createEmailSessionHistory(messageDatas, nlpResult?.EmailHistoryId, null);
                const sentimentSupervisorBot: any = await getSupervisiorEmitter(nlpResult);
                logger.info('getSupervisiorEmitter', sentimentSupervisorBot);
                sentimentSupervisorBot.forEach((ext: any) => {
                  logger.info('sentimentEmailBot', `${nlpResult?.DomainId}_${ext?.ext}`);
                  ioredisChat.to(`${nlpResult?.DomainId}_${ext?.ext}`).emit(emitters?.supervisiorActivityBot);
                });
                ioredisChat.to(`${routedQueue[0]?.domainId}_${200}`).emit(emitters?.adminActivityBot);
              } catch (error) {
                logger.info(error);
              }
            } catch (error) {
              logger.error('Error parsing JSON:', error);
            }
          } else {
            const routedQueueItem = routedQueue[0];
            if (routedQueueItem?.ext) {
              const getExtData = routedQueueItem.ext.split(',').filter(Boolean);
              getExtData.forEach((ext: any) => {
                logger.info('SuperVisiorEmitter', `${messageData?.domainId}_${ext}`);
                ioredisChat
                  .to(`${messageData?.domainId}_${ext}`)
                  .emit(emitters?.supervisiorActivity);
              });
            } else {
              logger.info('ext data found');
            }
          }
        });
      } else {
        if (list.messageId !== checkUser[0].messageId) {
          logger.info('✅ received Email append message...');
          session_Id = checkUser[0]?.SessionId;
          const EmailHistoryId = checkUser[0]?.EmailHistoryId;
          const messageList = createMessageList(list);
          const messageData: any = { message: messageList };
          await createEmailSessionHistory(messageData, EmailHistoryId, null).then(async (res: any) => {
            const getSession: any = await getEmailHistoryDetails(session_Id, checkUser[0]?.DomainId);
            let mailId = list?.fromData.split('<');
            mailId = mailId[1]?.substring(0, mailId[1]?.length - 1);
            const getPersonalData: any = await getPersonalDetails(mailId, getSession[0]?.DomainId);
            const getAssignedTo = getSession[0]?.AssignedTo;
            const getEmailExt = await getEmailextList(getAssignedTo);
            const getSupervisiorExt: any = await getSupervisiorEmitter(getSession[0]);
            getSupervisiorExt.forEach((ext: any) => {
              logger.info('SuperVisiorEmitter', `${getSession[0]?.DomainId}_${ext.ext}`);
              ioredisChat.to(`${getSession[0]?.DomainId}_${ext?.ext}`).emit(emitters?.supervisiorActivity);
              ioredisChat.to(`${getSession[0]?.DomainId}_${ext?.ext}`).emit(emitters?.receivedEmail, { ...getSession[0], agentAssist: true, calledPersonDetails: getPersonalData?.length > 0 ? getPersonalData[0] : null }, session_Id);
            });
            logger.info('emitter', `${getSession[0]?.DomainId}_${getEmailExt[0]?.ext}`);
            ioredisChat.to(`${getSession[0]?.DomainId}_${getEmailExt[0]?.ext}`).emit(emitters?.receivedEmail, { ...getSession[0], agentAssist: true, calledPersonDetails: getPersonalData?.length > 0 ? getPersonalData[0] : null }, session_Id);
          });

        } else {
          logger.info('❌ received Email Not append message...');
        }
      }
    });
  } catch (err) {
    logger.error('createEmailSession - Unexpected Error:', err);
  }
}

process.setMaxListeners(25);
// createEmailSession();
// setInterval(createEmailSession, 12000);

// create Email Session
export async function createEmailSessionGraph(imapInox: any) {
  try {
    const list: any = imapInox?.customerData;
    const routedQueue = await findEmailRoutedQueue(list);
    await checkExistUser(list).then(async (checkUser: any) => {
      logger.info({ checkUser });
      let session_Id: any;
      let historyId = null;
      if (checkUser?.length === 0 || routedQueue[0]?.isBot === 1) {
        if (checkUser?.length === 0) {
          session_Id = uuidv4();
        } else {
          session_Id = checkUser[0]?.SessionId;
          historyId = checkUser[0]?.EmailHistoryId;
        }
        const customerImapMsg: any = [];
        customerImapMsg.push(list);
        list.message = JSON.stringify(customerImapMsg);
        list.domainId = routedQueue[0]?.domainId;
        list.sessionId = session_Id;
        const messageList = createMessageList(list, false, routedQueue[0]?.isBot === 1);
        const extractedEmail = extractEmail(list?.toData);
        const fromData = extractEmail(list?.fromData);
        const messageData: any = {
          sessionStartTime: Math.floor(Date.now()),
          domainId: routedQueue[0]?.domainId,
          supervisorName: routedQueue[0]?.supervisorName,
          IsInBound: 1,
          customerData: list,
          ToEmail: extractedEmail,
          FromEmail: fromData,
          EmailSubject: list?.subjectData,
          message: messageList,
          messageId: list?.messageId,
          IsBot: 0,
          IsSessionClosed: 0,
          graphMessageId: list?.graphMessageId || null,
          conversationId: list?.conversationId || null,
        };
        await createEmailSessionHistory(messageData, historyId, session_Id).then(async (res: any) => {
          const coid = await insertCustomerContactPhone(messageData);
          getConnectorTokenInfo(routedQueue[0], (getConnect) => {
            if (getConnect?.length > 0) {
              callGoogleSheetSyncAPI({
                contactSyncDetails: getConnect[0]?.contactSyncDetails,
                newContacts: coid[0]?.coid,
                domainId: routedQueue[0]?.domainId,
                token: getConnect[0]?.token
              });
            }
          });
          updateCustomerId(res[0], coid[0]);
          logger.info(res[0]?.errmsg);
          await createAndInsertNotification(res, coid[0]);

          const nlpResult = res[0];
          const comDetails = await getCompanyDetails(res[0]);
          const getEmailData: any = await getEmailBot(nlpResult, comDetails, list?.textData, routedQueue[0]);
          const text = getEmailData?.message ?? null;

          if (routedQueue[0]?.isBot === 0 && getEmailData?.key === 'human_agent') {
            try {
              const autoAssign = await autoAssignAgent(res[0], getEmailData?.ext, getEmailData);
              ioredisChat.to(`${autoAssign[0]?.domainId}_${autoAssign[0]?.ext}`).emit(emitters?.assignActivityListener);
              logger.info('assignActivityAgent', `${autoAssign[0]?.domainId}_${autoAssign[0]?.ext}`);
            } catch (error) {
              console.log(error);
            }
          } else if (routedQueue[0]?.isBot === 1 && getEmailData?.key === 'human_agent') {
            try {
              const autoAssign = await autoAssignAgent(res[0], getEmailData?.ext, getEmailData);
              ioredisChat.to(`${autoAssign[0]?.domainId}_${autoAssign[0]?.ext}`).emit(emitters?.assignActivityListener);
              logger.info('assignActivityAgent', `${autoAssign[0]?.domainId}_${autoAssign[0]?.ext}`);
            } catch (error) {
              console.log(error);
            }
          } else if (routedQueue[0]?.isBot === 1 && (routedQueue[0]?.autoAssignAgent === 0 || routedQueue[0]?.autoAssignAgent === null)) {
            try {
              const getSmtp: any = await getSmptList(res[0]);
              if (getSmtp?.length > 0) {
                await sendMailByProvider({ ...res[0], graphMessageId: messageData.graphMessageId }, getSmtp[0], text);
              } else {
                await sendMail({
                  ...res[0],
                  text,
                  FromEmail: res[0]?.ToEmail,
                  ToEmail: res[0]?.FromEmail
                });
              }
              const getDispositin: any = await getDispositinStatus(nlpResult);
              const closedDispositions = getDispositin[0]?.filter((item: { dispositionName: string }) => item?.dispositionName === 'Closed');
              let message: any = {};
              try {
                message = {
                  to: nlpResult?.FromEmail,
                  from: nlpResult?.ToEmail,
                  messageID: uuidv4(),
                  sessionId: nlpResult?.SessionId,
                  message: text,
                  EmailSubject: nlpResult?.EmailSubject,
                  attachments: [],
                  sendAt: Date.now(),
                  messageType: 'text',
                  pdfurl: '',
                  isBot: true,
                  isCustomer: false,
                  fileType: '',
                  direction: 'OUT',
                  isLike: 0,
                  cc: '',
                  bcc: ''
                };
                const messageListData: any = { message };
                const messageType: any = { agentText: messageListData };
                const messageDatas: any = {
                  message: messageType,
                  IsBot: 1,
                  IsSessionClosed: 1,
                  DispositionId: closedDispositions[0]?.did,
                  Summary: 'Closed',
                  SessionEndTime: Math.floor(Date.now())
                };
                await createEmailSessionHistory(messageDatas, nlpResult?.EmailHistoryId, null);
                const sentimentSupervisorBot: any = await getSupervisiorEmitter(nlpResult);
                logger.info('getSupervisiorEmitter', sentimentSupervisorBot);
                sentimentSupervisorBot.forEach((ext: any) => {
                  logger.info('sentimentEmailBot', `${nlpResult?.DomainId}_${ext?.ext}`);
                  ioredisChat.to(`${nlpResult?.DomainId}_${ext?.ext}`).emit(emitters?.supervisiorActivityBot);
                });
                ioredisChat.to(`${routedQueue[0]?.domainId}_${200}`).emit(emitters?.adminActivityBot);
              } catch (error) {
                logger.info(error);
              }
            } catch (error) {
              logger.error('Error parsing JSON:', error);
            }
          } else {
            const routedQueueItem = routedQueue[0];
            if (routedQueueItem?.ext) {
              const getExtData = routedQueueItem.ext.split(',').filter(Boolean);
              getExtData.forEach((ext: any) => {
                logger.info('SuperVisiorEmitter', `${messageData?.domainId}_${ext}`);
                ioredisChat
                  .to(`${messageData?.domainId}_${ext}`)
                  .emit(emitters?.supervisiorActivity);
              });
            } else {
              logger.info('ext data found');
            }
          }
        });
      } else {
        if (list.messageId !== checkUser[0].messageId) {
          logger.info('✅ received Email append message...');
          session_Id = checkUser[0]?.SessionId;
          const EmailHistoryId = checkUser[0]?.EmailHistoryId;
          const messageList = createMessageList(list);
          const messageData: any = { message: messageList };
          await createEmailSessionHistory(messageData, EmailHistoryId, null).then(async (res: any) => {
            const getSession: any = await getEmailHistoryDetails(session_Id, checkUser[0]?.DomainId);
            const mailId = extractEmail(list?.fromData);
            const getPersonalData: any = await getPersonalDetails(mailId, getSession[0]?.DomainId);
            const getAssignedTo = getSession[0]?.AssignedTo;
            const getEmailExt = await getEmailextList(getAssignedTo);
            const getSupervisiorExt: any = await getSupervisiorEmitter(getSession[0]);
            getSupervisiorExt.forEach((ext: any) => {
              logger.info('SuperVisiorEmitter', `${getSession[0]?.DomainId}_${ext.ext}`);
              ioredisChat.to(`${getSession[0]?.DomainId}_${ext?.ext}`).emit(emitters?.supervisiorActivity);
              ioredisChat.to(`${getSession[0]?.DomainId}_${ext?.ext}`).emit(emitters?.receivedEmail, { ...getSession[0], agentAssist: true, calledPersonDetails: getPersonalData?.length > 0 ? getPersonalData[0] : null }, session_Id);
            });
            logger.info('emitter', `${getSession[0]?.DomainId}_${getEmailExt[0]?.ext}`);
            ioredisChat.to(`${getSession[0]?.DomainId}_${getEmailExt[0]?.ext}`).emit(emitters?.receivedEmail, { ...getSession[0], agentAssist: true, calledPersonDetails: getPersonalData?.length > 0 ? getPersonalData[0] : null }, session_Id);
          });

        } else {
          logger.info('❌ received Email Not append message...');
        }
      }
    });
  } catch (err) {
    logger.error('createEmailSession - Unexpected Error:', err);
  }
}

/**
 * set primary email
 * @param req
 * @param res
 * @param done
 */
export const setPrimaryEmailAddress = async (req: any, res: any, done: any) => {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
      EmailConfigId: req.body.EmailConfigId,
    };
    const setEmail = await setPrimaryEmailDao(data);
    logger.info('setPrimaryEmailDao', setEmail);
    if (setEmail[0].errcode === 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.Primary_set_successfully });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.set_primary_email_field });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('setPrimaryEmailAddress - Unexpected Error:', err);
  }
};

/**
 * Delete email
 * @param req
 * @param res
 * @param done
 */
export const deleteEmailAddress = async (req: any, res: any, done: any) => {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
      EmailConfigId: req.params.EmailConfigId,
    };
    await deleteEmailAddressDao(data);
    res.status(200).send({ statusCode: 200, message: RESPONSE.email_deleted_successfully });
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('deleteEmailAddress - Unexpected Error:', err);
  }
};

/**
 * Send Mail
 * @param req
 * @param res
 * @param done
 */

export const sendEmailHandler = async (req: any, res: any, done: any) => {
  try {
    const auth: any = req.headers;
    const fileName: any = req?.body?.fileName ?? '';
    const filePath: any = req?.body?.filePath ?? '';
    const data: any = {
      fileName,
      filePath,
      sessionId: req.body.sessionId,
      EmailHistoryId: req.body.emailHistoryId,
      EmailSubject: req.body.subject,
      FromEmail: req.body.from,
      ToEmail: req.body.to,
      cc: req?.body?.cc ?? '',
      bcc: req?.body?.bcc ?? '',
      text: req.body.text,
      attachments: req?.body?.attachments ?? [],
      messageId: req?.body?.messageId,
      agentName: req?.body?.agentName,
      graphMessageId: req?.body?.graphMessageId ?? null,
    };
    const messageList: any = {
      agentText: {
        message: {
          FromEmail: data?.FromEmail ?? '',
          ToEmail: data?.ToEmail ?? '',
          cc: data?.cc ?? '',
          bcc: data?.bcc ?? '',
          messageId: uuidv4(),
          sendAt: Date.now(),
          message: data?.text ?? '',
          EmailSubject: data?.EmailSubject ?? '',
          attachments: data?.attachments ?? [],
          isBot: false,
          agentName: data?.agentName,
          graphMessageId: data?.graphMessageId
        },
      },
    };
    const sendMessage: any = {
      message: messageList,
      sessionEndTime: Date.now(),
    };
    const getSmtp: any = await getSmptSend(data);
    (getSmtp?.length > 0) ? await sendMailByProvider(data, getSmtp[0], data?.text) : await sendMail(data);
    const sendmail = await createEmailSessionHistory(sendMessage, data.EmailHistoryId, data.sessionId);
    logger.info('createEmailSessionHistory', sendmail);
    const getPersonalData: any = await getPersonalDetails(data.ToEmail, sendmail[0]?.DomainId);
    const getSupervisiorExt: any = await getSupervisiorEmitter(sendmail[0]);
    logger.info('getSupervisiorEmitter', getSupervisiorExt);
    getSupervisiorExt.forEach((ext: any) => {
      logger.info('receivedEmail', `${sendmail[0]?.DomainId}_${ext.ext}`);
      ioredisChat.to(`${sendmail[0]?.DomainId}_${ext?.ext}`).emit(emitters?.receivedEmail, { ...sendmail[0], channelType: CHANNEL_TYPE?.EMAIL, sessionId: data.sessionId, agentAssist: false, calledPersonDetails: getPersonalData?.length > 0 ? getPersonalData[0] : null }, data.sessionId);
    });
    res.status(200).send({ statusCode: 200, message: RESPONSE.email_send, emailSendResponse: messageList });
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('sendEmailHandler - Unexpected Error:', err);
  }
};

/**
 * send Email Outbound Handler
 * @param req
 * @param res
 * @param done
 */

export const sendEmailOutboundHandler = async (req: any, res: any, done: any) => {
  try {
    const auth: any = req.headers;
    const fileName: any = req?.body?.fileName ?? '';
    const filePath: any = req?.body?.filePath ?? '';
    const data: any = {
      fileName,
      filePath,
      EmailHistoryId: req?.body?.emailHistoryId ?? 0,
      sessionId: uuidv4(),
      sessionStartTime: Math.floor(Date.now()),
      SessionEndTime: req?.body?.SessionEndTime ?? null,
      FromEmail: req?.body?.from ?? null,
      ToEmail: req?.body?.to ?? null,
      EmailSubject: req?.body?.subject ?? null,
      EmailFlowSourceId: req?.body?.emailFlowSourceId ?? null,
      CompanyId: req?.body?.companyId ?? null,
      domainId: auth?.domainId ?? null,
      DispositionId: req?.body?.dispositionId ?? null,
      Summary: req?.body?.summary ?? null,
      IsInBound: 0,
      AssignedTo: req?.body?.assignedTo ?? null,
      AssignedBy: req?.body?.assignedBy ?? null,
      AssignedOn: req?.body?.assignedOn ?? null,
      AfterEmailWorkTime: req?.body?.afterEmailTime ?? null,
      Istransferred: req?.body?.Istransferred ?? null,
      TransferredTypeId: req?.body?.transferredTypeId ?? null,
      IsSessionClosed: req?.body?.isSessionClosed ?? null,
      CustomerId: req?.body?.customerId ?? null,
      TicketId: req?.body?.ticketId ?? null,
      SupervisorName: req?.body?.supervisorName ?? null,
      AgentId: req?.body?.agentId ?? null,
      AgentSkillId: req?.body?.agentSkillId ?? null,
      IsBot: req?.body?.isBot ?? null,
      BotId: req?.body?.botId ?? null,
      QueueId: req?.body?.queueId ?? null,
      cc: req?.body?.cc ?? '',
      bcc: req?.body?.bcc ?? '',
      text: req.body.text,
      attachments: req?.body?.attachments ?? []
    };
    const messageList: any = {
      agentText: {
        message: {
          FromEmail: data?.FromEmail ?? '',
          ToEmail: data?.ToEmail ?? '',
          cc: data?.cc ?? '',
          bcc: data?.bcc ?? '',
          messageId: uuidv4(),
          sendAt: Date.now(),
          message: data?.text ?? '',
          EmailSubject: data?.EmailSubject ?? '',
          attachments: data?.attachments ?? [],
          isBot: false
        },
      },
    };
    const sendMessage: any = {
      message: messageList,
    };
    const getSmtp: any = await getSmptSend(data);
    (getSmtp?.length > 0) ? await sendMailByProvider(data, getSmtp[0], data?.text) : await sendMail(data);
    const sendOutbound = await createEmailSessionHistoryOutbound(data, sendMessage);
    const getSupervisiorExt: any = await getSupervisiorEmitter(sendOutbound[0]);
    logger.info('getSupervisiorEmitter:', getSupervisiorExt);
    getSupervisiorExt.forEach((ext: any) => {
      logger.info('SuperVisiorEmitter', `${sendOutbound[0]?.DomainId}_${ext.ext}`);
      ioredisChat.to(`${sendOutbound[0]?.DomainId}_${ext.ext}`).emit(emitters?.supervisiorActivity);
    });
    res.status(200).send({ statusCode: 200, message: RESPONSE.email_send, sendOutboundResponse: sendOutbound[0] });
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('sendEmailOutboundHandler - Unexpected Error:', err);
  }
};

/**
 * send Email Outbound Handler
 * @param req
 * @param res
 * @param done
 */

export const sendEmailOutboundCompanyHandler = async (req: any, res: any, done: any) => {
  try {
    const auth: any = req.headers;
    const fileName: any = req?.body?.fileName ?? '';
    const filePath: any = req?.body?.filePath ?? '';
    const data: any = {
      fileName,
      filePath,
      sessionId: uuidv4(),
      sessionStartTime: Math.floor(Date.now()),
      SessionEndTime: req?.body?.SessionEndTime ?? null,
      FromEmail: req?.body?.from ?? null,
      ToEmail: req?.body?.to ?? null,
      EmailSubject: req?.body?.subject ?? null,
      EmailFlowSourceId: req?.body?.emailFlowSourceId ?? null,
      CompanyId: auth?.companyId ?? null,
      domainId: auth?.domainId ?? null,
      DispositionId: req?.body?.dispositionId ?? null,
      Summary: req?.body?.summary ?? null,
      IsInBound: 0,
      AssignedTo: req?.body?.assignedTo ?? null,
      AssignedBy: req?.body?.assignedBy ?? null,
      AssignedOn: req?.body?.assignedOn ?? null,
      AfterEmailWorkTime: req?.body?.afterEmailTime ?? null,
      Istransferred: req?.body?.Istransferred ?? null,
      TransferredTypeId: req?.body?.transferredTypeId ?? null,
      IsSessionClosed: req?.body?.isSessionClosed ?? 0,
      CustomerId: req?.body?.customerId ?? null,
      TicketId: req?.body?.ticketId ?? null,
      SupervisorName: req?.body?.supervisorName ?? null,
      AgentId: req?.body?.agentId ?? null,
      AgentSkillId: req?.body?.agentSkillId ?? null,
      IsBot: req?.body?.isBot ?? 0,
      BotId: req?.body?.botId ?? null,
      QueueId: req?.body?.queueId ?? null,
      cc: req?.body?.cc ?? '',
      bcc: req?.body?.bcc ?? '',
      attachments: req?.body?.attachments ?? []
    };
    const sendOutbound = await sendEmailOutboundCompany(data);
    logger.info('sendEmailOutboundCompany:', sendOutbound);
    res.status(200).send({ statusCode: 200, message: RESPONSE.email_send, sendOutboundResponse: sendOutbound[0] });
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('sendEmailOutboundCompanyHandler - Unexpected Error:', err);
  }
};

/**
 * Send Mail
 * @param req
 * @param res
 * @param done
 */

export const sendEmailBotHandler = async (req: any, res: any, done: any) => {
  try {
    const auth: any = req.headers;
    const fileName: any = req?.body?.fileName ?? '';
    const filePath: any = req?.body?.filePath ?? '';
    const data: any = {
      fileName,
      filePath,
      EmailSubject: req?.body?.subject ?? '',
      FromEmail: req?.body?.from ?? '',
      ToEmail: req?.body?.to ?? '',
      cc: req?.body?.cc ?? '',
      bcc: req?.body?.bcc ?? '',
      text: req?.body?.text ?? '',
      attachments: req?.body?.attachments ?? [],
      messageId: req?.body?.messageId ?? null
    };
    const getSmtp: any = await getSmptSend(data);
    logger.info('getSmptList:', getSmtp);
    (getSmtp?.length > 0) ? await sendMailByProvider(data, getSmtp[0], data?.text) : await sendMail(data);
    res.status(200).send({ statusCode: 200, message: RESPONSE.email_send });
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('sendEmailBotHandler - Unexpected Error:', err);
  }
};

/**
 * Send Mail
 * @param req
 * @param res
 * @param done
 */
export const sendEmailAudracareHandler = async (req: any, res: any, done: any) => {
  try {
    const fileName: any = req?.body?.fileName ?? '';
    const filePath: any = req?.body?.filePath ?? '';
    const data: any = {
      fileName,
      filePath,
      EmailSubject: req?.body?.subject ?? '',
      FromEmail: process.env.AUDRACARE_FROM_EMAIL,
      ToEmail: req?.body?.to ?? '',
      cc: req?.body?.cc ?? '',
      bcc: req?.body?.bcc ?? '',
      text: req?.body?.text ?? '',
      attachments: req?.body?.attachments ?? []
    };
    await sendMailAudracare(data);
    res.status(200).send({ statusCode: 200, message: RESPONSE.email_send });
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('sendEmailAudracareHandler - Unexpected Error:', err);
  }
};

/**
 * Send Mail
 * @param req
 * @param res
 * @param done
 */

export const sendEmailTemplateHandler = async (req: any, res: any, done: any) => {
  try {
    const data: any = {
      FromEmail: process.env.SEND_FROM_EMAIL,
      ToEmail: req?.body?.to ?? '',
      EmailSubject: req?.body?.subject ?? '',
      text: req?.body?.css ?? '',
    };
    const html = req?.body?.html ?? '';
    await sendMailTemplate(data, html);
    res.status(200).send({ statusCode: 200, message: RESPONSE.email_template });
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('sendEmailTemplateHandler - Unexpected Error:', err);
  }
};

/**
 * get Supervisior List Handler
 * @param req
 * @param res
 */
export async function getSupervisiorListHandler(req: any, res: any) {
  try {
    const data: any = {};
    const auth: any = req.headers;
    data.domainId = auth.domainId;
    const supervisorList: any = await getSupervisorDao(data);
    logger.info('getSupervisorDao', supervisorList);
    if (supervisorList.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: supervisorList });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getSupervisiorListHandler - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export const fileUploadController = async (req: any, res: any, done: any) => {
  try {
    const filesarry: any = req.files.doc;
    const authorization = req.headers;
    const domainId = authorization.domainId;
    const uuid = uuidv4();
    for (const email of filesarry) {
      const url = `${configs.email_path.Email_fileGetUrl}${domainId}/${email.originalname}`;
      email.uuid = uuid;
      email.url = url;
      email.name = `${email.originalname}`;
      email.domainId = domainId;
      delete email.destination;
      delete email.originalname;
    }
    res.send({ statusCode: 200, errCode: -1, message: 'success', fileList: filesarry });
  } catch (err) {
    res.send({ statusCode: 500, errCode: 1, message: 'internal server error' });
    logger.error('fileUploadController - Unexpected Error:', err);
  }
};

/**
 *
 * @param req
 * @param res
 * @param done
 */

export const getEmailHistoryHandler = async (req: any, res: any, done: any) => {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
      channelType: req?.body?.channelType ?? null,
      EmailSessionId: req?.body?.sessionId ?? null
    };
    const getEmail: any = await getEmailHistory(data);
    logger.info('getEmailHistory', getEmail);
    if (getEmail.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getEmailListRes: getEmail });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getEmailHistoryHandler - Unexpected Error:', err);
  }
};

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function updateDispositionEmailHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const params = req.params;
    const data: any = {
      EmailHistoryId: params.EmailHistoryId,
      domainId: auth.domainId,
      dispositionId: req?.body?.dispositionId ?? 0,
      summary: req?.body?.summary ?? null,
      isSessionClosed: req.body?.isSessionClosed ?? 0,
      AfterEmailWorkTime: req?.body?.afterWorkTime ?? null,
      dispositionUpdatedBy: req?.body?.dispositionUpdatedBy ?? null,
      duration: req?.body?.duration ?? null,
      dispositionSubject: req?.body?.dispositionSubject ?? null,
      tags: req?.body?.tags ?? null,
    };
    logger.info('updateDispositionEmailHandler - Request Data:', data);

    const customerSatisfactionData: any = {
      domainId: data.domainId,
      channelTypeId: 2003003,
      historyId: data.EmailHistoryId,
    };
    logger.info('customerSatisfactionData', customerSatisfactionData);
    const customerSatisfactionResult: any = await getCustomerSatisficationDao(customerSatisfactionData);
    logger.info('customerSatisfactionResult', customerSatisfactionResult[0]);
    if (!Array.isArray(customerSatisfactionResult) || customerSatisfactionResult.length === 0 || !customerSatisfactionResult[0]) {
      return res.status(404).send({ statusCode: 404, message: RESPONSE?.csat });
    }
    const insertResult = customerSatisfactionResult[0]?.insertResult?.[0];
    if (insertResult && insertResult.errcode === -1) {
      return res.status(200).send({
        statusCode: 404,
        message: insertResult.errmsg || RESPONSE?.csat,
        result: []
      });
    }
    await handleCustomerSatisfaction(customerSatisfactionResult[0], customerSatisfactionData, req, auth);
    const updateDispStatus: any = await updateDispositionForEmail(data);
    logger.info('updateDispositionForEmail', updateDispStatus);
    if (data?.isSessionClosed === 1) {
      const updateDispStatusSupervisor: any = await getSupervisiorList(data);
      updateDispStatusSupervisor.forEach((ext: any) => {
        logger.info('emitterDispStatus', `${data?.domainId}_${ext?.ext}`);
        ioredisChat.to(`${data?.domainId}_${ext?.ext}`).emit(emitters?.interActionClosed);
      });
    }

    const nlpData: any = {
      HistoryId: data?.EmailHistoryId,
      ChatDurationHistoryId: data?.ChatDurationHistoryId ?? 0,
      channelType: CHANNEL_TYPE?.EMAIL,
      domainId: data?.domainId,
      ext: data?.ext ?? 0
    };

    const updateScore: any = await sentimentalScore(nlpData);
    logger.info('sentimentalScore', updateScore);
    if (updateScore?.status === 200) {
      await updateSentimentScore(data, updateScore?.data);
      if (updateScore?.data) {
        ioredisChat.to(`${auth.domainId}_${auth.ext}`).emit(emitters?.SentimentScoreAnalytics, { data, speedometerScore: updateScore.data.sentimentalScore, sentimentalScoreUser: updateScore.data.sentimentalScoreUser, sentimentalScoreAgent: updateScore.data.sentimentalScoreAgent });
      } else {
        ioredisChat.to(`${auth.domainId}_${auth.ext}`).emit(emitters?.SentimentScoreAnalytics, { data, speedometerScore: 0, sentimentalScoreUser: 0, sentimentalScoreAgent: 0 });
      }
    } else {
      ioredisChat.to(`${auth.domainId}_${auth.ext}`).emit(emitters?.SentimentScoreAnalytics, { data, speedometerScore: 0, sentimentalScoreUser: 0, sentimentalScoreAgent: 0 });
    }

    if (updateDispStatus) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.disposition_statuss, result: data });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('updateDispositionEmailHandler - Unexpected Error:', err);
  }
}

/**
 * update Email Bot Status
 * @param req
 * @param res
 * @param done
 */
export const updateEmailBotStatus = async (req: any, res: any, done: any) => {
  try {
    const data: any = {
      domainId: req.body.domainId,
      isBot: req?.body?.isBot ?? 0,
      isBotPurchased: req?.body?.isBotPurchased ?? 0,
      emailAddress: req?.body?.emailId ?? null
    };
    await updateEmailBot(data);
    res.status(200).send({ statusCode: 200, message: RESPONSE.bot_status });
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('updateEmailBotStatus - Unexpected Error:', err);
  }
};

/**
 * onbord update Email Bot Status
 * @param req
 * @param res
 * @param done
 */
export const onbordUpdateEmailBotStatus = async (req: any, res: any, done: any) => {
  try {
    const data: any = {
      domainId: req.body.domainId,
      isBot: req?.body?.isBot ?? 0,
      isBotPurchased: req?.body?.isBotPurchased ?? 0
    };
    await onbordUpdateEmailBot(data);
    res.status(200).send({ statusCode: 200, message: RESPONSE.bot_status });
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('onbordUpdateEmailBotStatus - Unexpected Error:', err);
  }
};

/**
 * get agent assist information
 * @param req
 * @param res
 * @param done
 */
export const getAgentAssistHandler = async (req: any, res: any, done: any) => {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth?.domainId,
      ext: parseInt(auth?.ext, 10),
      channelType: req?.body?.channelType ?? null,
      channelId: req?.body?.channelId ?? null,
      message: req?.body?.message ?? null,
      sessionId: req?.body?.sessionId ?? null,
      type: req?.body?.type ?? '',
      customerId: req?.body?.customerId ?? '',
      messageId: req?.body?.messageId ?? null,
    };

    if (data.channelType === CHANNEL_TYPE?.EMAIL || data.channelType === CHANNEL_TYPE?.SMS) {
      const getWebsite: any = await getKnowledgeBaseIdNew(data.domainId);
      const agentAssistData: any = await agentAssistChannel(data, getWebsite);

      res.status(200).send({ statusCode: 200, message: RESPONSE.rpush });
      logger.info('agentAssistOmniChannel', `${data.domainId}_${data.ext}`);

      agentAssistData.data.channelId = data.channelId;
      // agentAssistData.data.messageId = uuidv4();
      agentAssistData.data.timeStamp = new Date().getTime();
      ioredisChat.to(`${data.domainId}_${data.ext}`).emit(emitters?.agentAssistOmniChannel, agentAssistData.data);

    } else if (data.channelType === CHANNEL_TYPE?.WHATSAPP || data.channelType === CHANNEL_TYPE?.FACEBOOK || data.channelType === CHANNEL_TYPE?.INSTAGRAM) {
      const knowledgeBaseId: any = await getDomainLevelKnowledgeBaseId(data?.domainId, data?.channelType);
      const getWebsite: any = await getKnowledgeBase(data.domainId, knowledgeBaseId[0].knowledgeBaseId);
      const agentAssistData: any = await agentAssistChannel(data, getWebsite);
      logger.info('agentAssistData----', agentAssistData.data);
      res.status(200).send({ statusCode: 200, message: RESPONSE.rpush });
      logger.info('agentAssistOmniChannel', `${data.domainId}_${data.ext}`);

      agentAssistData.data.channelId = data.channelId;
      // agentAssistData.data.messageId = uuidv4();
      agentAssistData.data.timeStamp = new Date().getTime();
      ioredisChat.to(`${data.domainId}_${data.ext}`).emit(emitters?.agentAssistOmniChannel, agentAssistData.data);

    } else if (data.channelType === CHANNEL_TYPE?.CHAT || data.channelType === CHANNEL_TYPE?.CALL) {
      const getWebsite: any = await getKnowledgeBaseIdNew(data?.domainId);
      const AgentAssist: any = await agentAssistChannel(data, getWebsite);
      res.status(200).send({ statusCode: 200, message: RESPONSE.rpush });
      logger.info('agentAssistOmniChannel', `${data?.domainId}_${data?.ext}`);
      AgentAssist.data.channelId = data?.channelId;
      // AgentAssist.data.messageId = uuidv4();
      AgentAssist.data.timeStamp = new Date().getTime();
      ioredisChat.to(`${data?.domainId}_${data?.ext}`).emit(emitters?.agentAssistOmniChannel, AgentAssist?.data);

    } else {
      res.status(400).send({ statusCode: 400, message: RESPONSE?.invalid_channel });
    }

  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getAgentAssistHandler - Unexpected Error:', err);
  }
};

/**
 *
 * @param req
 * @param res
 * @param done
 * ccs assign call back api
 */
export async function assignCallBackHandlers(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      assignedBy: req?.body?.assignedBy ?? null,
      assignedTo: req?.body?.assignedTo ?? null,
      assignedOn: Date.now(),
      EmailHistoryId: req?.body?.emailHistoryId,
      Istransferred: req?.body?.Istransferred ?? null
    };
    const assignRes: any = await assignCallBack(data);
    logger.info('assignCallBack', assignRes);
    if (assignRes) {
      res
        .status(200)
        .send({ statusCode: 200, message: RESPONSE.update_asssign });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('assignCallBackHandlers - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function sendMailForLeadGeneration(req: any, res: any, done: any) {
  try {
    const data: any = {
      FromEmail: req.body?.from,
      ToEmail: req.body?.to,
      EmailSubject: req.body?.subject,
      text: req.body?.text,
      attachments: req.body?.attachments ?? []
    };
    await sendMailForLead(data);
    res.status(200).send({ statusCode: 200, message: RESPONSE.success_message });
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('sendMailForLeadGeneration - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 */
export const getEmailFlowHistoryListHandler = async (req: any, res: any) => {
  try {
    const data: any = {
      domainId: req?.headers?.domainId
    };
    const getEmailFlowHistory: any = await getEmailFlowHistoryList(data);
    logger.info('getEmailFlowHistoryList', getEmailFlowHistory);
    if (getEmailFlowHistory && getEmailFlowHistory.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getEmailFlowHistoryListRes: getEmailFlowHistory });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getEmailFlowHistoryListRes: [] });
    }
  } catch (error) {
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getEmailFlowHistoryListHandler - Unexpected Error:', error);
  }
};

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function sendMailRouteMobileHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      FromEmail: req.body?.from,
      ToEmail: req.body?.to,
      businessName: req?.body?.businessName ?? null,
      industry: req?.body?.industry ?? null,
      websiteURL1: req?.body?.websiteURL1 ?? null,
      businessUnits: req?.body?.businessUnits ?? null,
      contactName: req?.body?.contactName ?? null,
      contactDesignation: req?.body?.contactDesignation ?? null,
      contactEmailAddress: req?.body?.contactEmailAddress ?? null,
      callbackURL: req?.body?.callbackURL ?? null,
      launchRegions: req?.body?.launchRegions ?? null,
      whatsappPhoneNumber: req?.body?.whatsappPhoneNumber ?? null,
      whatsappConversationVolume: req?.body?.whatsappConversationVolume ?? null,
      downloadLinkForSignedExhibitBdocument: req?.body?.downloadLinkForSignedExhibitBdocument ?? null
    };
    await sendMailRouteMobile(data);
    res.status(200).send({ statusCode: 200, message: RESPONSE.success_message });
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('sendMailRouteMobileHandler - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export const createCrmEmailConfigHandler = async (req: any, res: any, done: any) => {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
      emailConfigId: req.body.emailConfigId,
      emailAddress: req.body.emailAddress,
      aliasName: req.body.aliasName,
      supervisorName: req.body.supervisorName,
      emailThreshold: req.body.emailThreshold,
      isEmail: req.body.isEmail,
      primaryEmail: req.body.primaryEmail,
      isBot: req.body.isBot,
      knowledgeBasedId: req.body.knowledgeBasedId,
      autoAssignAgent: req.body.autoAssignAgent,
      product_id: req.body.product_id,
      is_smtp: req.body.is_smtp,
      smtp_json: req.body.smtp_json,
      emailtype: req.body.emailtype,
      isRepliedEmail: req.body.isRepliedEmail,
      repliedEmail: req.body.repliedEmail,
    };
    const setEmailCongfig = await createCrmEmailConfigDao(data);
    logger.info('createCrmEmailConfigDao', setEmailCongfig);
    if (setEmailCongfig[0]?.errcode === 0) {
      res.status(200).send({ statusCode: 200, message: setEmailCongfig[0]?.errmsg, result: setEmailCongfig });
    } else {
      res.status(200).send({ statusCode: 404, message: setEmailCongfig[0]?.errmsg });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('createCrmEmailConfigHandler - Unexpected Error:', err);
  }
};

/**
 *
 * @param req
 * @param res
 * @param done
 */
export const insertDomainConfigurationHandler = async (req: any, res: any, done: any) => {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
      companyId: auth.companyId,
      domainName: req.body.domainName ?? null,
      status: req.body.status ?? null,
      remarks: req.body.remarks ?? null,
      host1: req.body.host1 ?? null,
      value1: req.body.value1 ?? null,
      host2: req.body.host2 ?? null,
      value2: req.body.value2 ?? null,
      host3: req.body.host3 ?? null,
      value3: req.body.value3 ?? null
    };
    const insertDomain = await insertDomainConfiguration(data);
    logger.info('insertDomainConfiguration', insertDomain);
    if (insertDomain[0]?.errcode === 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.insert_domain });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.failed_domain });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('insertDomainConfigurationHandler - Unexpected Error:', err);
  }
};

/**
 *
 * @param req
 * @param res
 */
export const getDomainConfigurationListHandler = async (req: any, res: any) => {
  try {
    const data: any = {
      domainId: req?.headers?.domainId
    };
    const getDomain: any = await getDomainConfigurationList(data);
    logger.info('getDomainConfigurationList', getDomain);
    if (getDomain && getDomain.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getDomainConfigurationListRes: getDomain });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getDomainConfigurationListRes: [] });
    }
  } catch (error) {
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getDomainConfigurationListHandler - Unexpected Error:', error);
  }
};

/**
 *
 * @param req
 * @param res
 */
export const getDkimGenerateKeyHandler = async (req: any, res: any) => {
  try {
    const keypair = forge.pki.rsa.generateKeyPair(2048);
    const privateKeyPem = forge.pki.privateKeyToPem(keypair.privateKey);
    const publicKeyPem = forge.pki.publicKeyToPem(keypair.publicKey);
    const publicKey = `v=DKIM1;t=s;p=${publicKeyPem}`;
    res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, publicKeyRes: publicKey, privateKeyRes: privateKeyPem });
  } catch (error) {
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getDkimGenerateKeyHandler - Unexpected Error:', error);
  }
};

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function deleteDomainDetailsHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      id: req.params.id,
      domainId: auth.domainId
    };
    const deleteDomain: any = await deleteDomainDetails(data);
    logger.info('deleteDomainDetails', deleteDomain);
    if (deleteDomain[0]?.errcode === 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.delete_domain });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.delete_domain_field });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('deleteDomainDetailsHandler - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 */
export const getDomainConfigurationDetailsHandler = async (req: any, res: any) => {
  try {
    const data: any = {
      id: req.params.id,
      domainId: req?.headers?.domainId
    };
    const getDomain: any = await getDomainConfigurationDetails(data);
    logger.info('getDomainConfigurationDetails', getDomain);
    if (getDomain && getDomain.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getDomainConfigurationDetailsRes: getDomain });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getDomainConfigurationDetailsRes: [] });
    }
  } catch (error) {
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getDomainConfigurationDetailsHandler - Unexpected Error:', error);
  }
};

/**
 *
 * @param req
 * @param res
 */
export const getDomainAlreayExistHandler = async (req: any, res: any) => {
  try {
    const data: any = {
      domainId: req?.headers?.domainId,
      domainName: req.params.domainName
    };
    const getDomain: any = await getDomainAlreayExist(data);
    if (getDomain[0].errcode === -2) {
      res.status(200).send({ statusCode: 403, message: RESPONSE.name_already_exist, flag: 1 });
    } else {
      res.status(200).send({ statusCode: 200, message: RESPONSE.available_domain, flag: 0 });
    }
  } catch (error) {
    logger.info(error);
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getDomainAlreayExistHandler - Unexpected Error:', error);
  }
};

/**
 *
 * @param req
 * @param res
 * @param done
 */
export const updateDomainVerificationHandler = async (req: any, res: any, done: any) => {
  try {
    const auth: any = req.headers;
    const data: any = {
      id: req?.params?.id,
      domainId: auth.domainId,
      status: req?.body?.status ?? null,
      domainName: req?.body?.domainName ?? null,
    };
    const lookupSPF = await lookupSPFRecord(data?.domainName);
    const CNAME = await lookupCNAME(data?.domainName);
    const dkim = await verifyDkim(data?.domainName);
    const verifySuccee = await Promise.all([lookupSPF, CNAME, dkim]);
    if (verifySuccee.every(value => value === true)) {
      const updateDomain = await updateDomainVerification(data);
      logger.info('updateDomainVerification', updateDomain);
      if (updateDomain[0]?.errcode === 1) {
        res.status(200).send({ statusCode: 200, message: RESPONSE.update_domain });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.status_domain });
      }
      const body: any = {
        domainName: data?.domainName,
        description: `${updateDomain[0]?.companyName} company`,
        aliases: 10,
        mailboxes: 10,
        maxquota: '10',
        quota: '2048',
        transport: 'virtual',
        backupmx: 0,
        active: 1,
        passwordExpiry: 365,
        domainId: data?.domainId
      };
      await createDomain(body);
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_verify_domain });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('updateDomainVerificationHandler - Unexpected Error:', err);
  }
};

/**
 *
 * @param req
 * @param res
 */
export const getVerificationDomainListHandler = async (req: any, res: any) => {
  try {
    const data: any = {
      domainId: req?.headers?.domainId
    };
    const getDomain: any = await getVerificationDomainList(data);
    logger.info('getVerificationDomainList', getDomain);
    if (Array.isArray(getDomain) && getDomain.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getVerificationDomainListRes: getDomain });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getVerificationDomainListRes: [] });
    }
  } catch (error) {
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getVerificationDomainListHandler - Unexpected Error:', error);
  }
};

/**
 *
 * @param req
 * @param res
 * @param done
 */
export const createMailboxHandler = async (req: any, res: any, done: any) => {
  try {
    const hash = await argon2.hash('admin123');
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
      domainName: req?.body?.domainName,
      username: req?.body?.username,
      password: hash,
      name: 'test',
      maildir: req?.body?.maildir,
      quota: '512000',
      localPart: 'support',
      active: 1,
      phone: '',
      emailOther: '123',
      token: '',
      processtype: 1
    };
    const insertDomain = await createMailBox(data);
    logger.info('createMailBox', insertDomain);
    if (insertDomain[0]?.errcode === 0) {
      res.status(200).send({ statusCode: 200, message: insertDomain[0]?.errmsg });
    } else {
      res.status(200).send({ statusCode: 404, message: insertDomain[0]?.errmsg });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('createMailboxHandler - Unexpected Error:', err);
  }
};

export async function updateReplyViaDispositionEmailHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const params = req.params;
    const data: any = {
      EmailHistoryId: req?.body?.EmailHistoryId,
      domainId: auth.domainId,
      dispositionId: req?.body?.dispositionId ?? null,
      summary: req?.body?.summary ?? null,
      isSessionClosed: req.body?.isSessionClosed ?? 0,
      AfterEmailWorkTime: req?.body?.afterWorkTime ?? null,
      dispositionUpdatedBy: req?.body?.dispositionUpdatedBy ?? null,
      duration: req?.body?.duration ?? null,
      replyChannelType: req?.body?.responseChannelType ?? null,
      responseHistoryId: req?.body?.responseHistoryId ?? null,
      coid: req?.body?.coid ?? null,
    };

    const nlpData: any = {
      HistoryId: data?.EmailHistoryId,
      ChatDurationHistoryId: data?.ChatDurationHistoryId ?? 0,
      channelType: CHANNEL_TYPE.EMAIL,
      domainId: data?.domainId,
      ext: data?.ext ?? 0
    };

    const updateScore: any = await sentimentalScore(nlpData);
    if (updateScore?.status === 200) {
      await updateSentimentScore(data, updateScore?.data);
      if (updateScore?.data) {
        ioredisChat.to(`${auth.domainId}_${auth.ext}`).emit(emitters?.SentimentScoreAnalytics, { data, speedometerScore: updateScore.data.sentimentalScore, sentimentalScoreUser: updateScore.data.sentimentalScoreUser, sentimentalScoreAgent: updateScore.data.sentimentalScoreAgent });
      } else {
        ioredisChat.to(`${auth.domainId}_${auth.ext}`).emit(emitters?.SentimentScoreAnalytics, { data, speedometerScore: 0, sentimentalScoreUser: 0, sentimentalScoreAgent: 0 });
      }
    } else {
      ioredisChat.to(`${auth.domainId}_${auth.ext}`).emit(emitters?.SentimentScoreAnalytics, { data, speedometerScore: 0, sentimentalScoreUser: 0, sentimentalScoreAgent: 0 });
    }
    const updateDispStatus: any = await updateReplyViaDispositionForEmail(data);
    if (data?.isSessionClosed === 1) {
      const updateDispStatusSupervisor: any = await getSupervisiorList(data);
      updateDispStatusSupervisor.forEach((ext: any) => {
        logger.info('emitterDispStatus', `${data?.domainId}_${ext?.ext}`);
        ioredisChat.to(`${data?.domainId}_${ext?.ext}`).emit(emitters?.interActionClosed);
      });
    }

    logger.info('updateDispStatus>>>>>>>>>>>>>>>>>>>', updateDispStatus);
    if (updateDispStatus) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.disposition_statuss, result: data });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('updateReplyViaDispositionEmailHandler - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export const insertUnsubscribeDetailsHandler = async (req: any, res: any, done: any) => {
  try {
    const data: any = {
      from: req.body.from ?? null,
      to: req.body.to ?? null,
      isSubscribed: req.body.isSubscribed ?? null
    };
    const isSubscribed = await insertUnsubscribeDetails(data);
    logger.info('insertUnsubscribeDetails', isSubscribed);
    if (isSubscribed[0]?.errcode === 0) {
      res.status(200).send({ statusCode: 200, message: isSubscribed[0]?.errmsg });
    } else {
      res.status(200).send({ statusCode: 404, message: isSubscribed[0]?.errmsg });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('insertUnsubscribeDetailsHandler - Unexpected Error:', err);
  }
};

/**
 *
 * @param req
 * @param res
 * @param done
 */
export const insertTicketingEmailConfig = async (req: any, res: any, done: any) => {
  try {
    const auth: any = req.headers;
    const data: any = {
      EmailConfigId: req.body.EmailConfigId ?? null,
      emailAddress: req.body.emailAddress ?? null,
      domainId: auth.domainId,
      companyId: auth.companyId,
      is_smtp: req.body.is_smtp ?? null,
      emailtype: req.body.emailtype ?? null,
    };
    const result = await insertTicketingEmailConfigDao(data);
    logger.info('insertTicketingEmailConfigDao', result);
    if (result[0]?.errcode === 0) {
      res.status(200).send({ statusCode: 200, message: result[0] });
    } else {
      res.status(200).send({ statusCode: 404, message: result[0]?.errmsg });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('insertTicketingEmailConfig - Unexpected Error:', err);
  }
};

/**
 *
 * @param req
 * @param res
 * @param done
 */
export const deleteTicketingEmailConfig = async (req: any, res: any, done: any) => {
  try {
    const data: any = {
      EmailConfigId: req.body.EmailConfigId ?? null,
    };
    const result = await deleteTicketingEmailConfigDao(data);
    logger.info('deleteTicketingEmailConfigDao', result);
    if (result[0]?.errcode === 0) {
      res.status(200).send({ statusCode: 200, message: result });
    } else {
      res.status(200).send({ statusCode: 404, message: result[0]?.errmsg });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('deleteTicketingEmailConfig - Unexpected Error:', err);
  }
};

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function sendEmailSessionExpiryHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      FromEmail: process.env.SESSION_EXPIRY_EMAIL,
      ToEmail: req?.body?.adminEmail,
      EmailSubject: req?.body?.isEmailSent === '2' ? 'Heads Up: All Sessions Have Been Consumed! ' : req?.body?.isEmailSent === '0' ? "Session Alert: You've Reached 50% Consumption!" : "Session Alert: You've Reached 80% Consumption!",
      isEmailSent: req?.body?.isEmailSent ?? null,
    };
    const countData = await getSessionDetails(data);
    const getDatas = await getRemainingSessionEmailSent(countData[0]);
    await sendEmailSessionExpiry(data, countData[0], getDatas[0]);
    res.status(200).send({ statusCode: 200, message: RESPONSE.success_message });
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('sendEmailSessionExpiryHandler - Unexpected Error:', err);
  }
}

/**
 * Send Mail
 * @param req
 * @param res
 * @param done
 */

export const sendEmailPlainTextHandler = async (req: any, res: any, done: any) => {
  try {
    const data: any = {
      FromEmail: process.env.SEND_FROM_EMAIL,
      ToEmail: req?.body?.to ?? '',
      EmailSubject: req?.body?.subject ?? '',
      text: req?.body?.text ?? '',
    };
    await sendMailPlainText(data);
    res.status(200).send({ statusCode: 200, message: RESPONSE.email_template });
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('sendEmailPlainTextHandler - Unexpected Error:', err);
  }
};

// Lock to prevent concurrent processing per mailbox
const mailboxProcessingLock: Set<string> = new Set();
const mailboxPendingSync: Set<string> = new Set();

// Deduplication cache for webhook notifications (resource-based)
const processedNotifications: Map<string, number> = new Map();
const NOTIFICATION_CACHE_TTL = 5 * 60 * 1000; // 5 minutes TTL

// Cleanup old notifications periodically
setInterval(() => {
  const now = Date.now();
  for (const [key, timestamp] of processedNotifications) {
    if (now - timestamp > NOTIFICATION_CACHE_TTL) {
      processedNotifications.delete(key);
    }
  }
},          60 * 1000);

/**
 *
 * @param req
 * @param res
 * @returns
 */
export async function graphWebhookHandler(req: any, res: any) {
  const validationToken = req?.query?.validationToken;
  if (validationToken) {
    res.header('Content-Type', 'text/plain');
    return res.status(200).send(validationToken);
  }
  res.status(202).send('OK');

  try {
    const notifications = req.body?.value || [];
    const getNotificationKey = (notify: any): string => {
      const tenantId = notify?.tenantId || '';
      const resource = notify?.resource || '';
      const resourceItemId = notify?.resourceData?.id || notify?.id || '';
      const changeType = notify?.changeType || '';
      if (!resourceItemId) return '';
      return [tenantId, resource, resourceItemId, changeType]
        .filter(Boolean)
        .join(':');
    };

    const newNotifications = notifications.filter((notify: any) => {
      const resource: string = notify?.resource;
      const tenantId = notify?.tenantId;
      if (!resource || !tenantId) return false;
      const notificationKey = getNotificationKey(notify);
      if (!notificationKey) return true;
      if (processedNotifications.has(notificationKey)) {
        return false;
      }
      processedNotifications.set(notificationKey, Date.now());
      return true;
    });

    const duplicateCount = notifications.length - newNotifications.length;
    if (duplicateCount > 0) {
      logger.info('Skipped duplicate webhook notifications', {
        duplicateCount,
        total: notifications.length,
      });
    }

    if (newNotifications.length === 0) {
      logger.info('All webhook notifications were duplicates, skipping');
      return;
    }

    logger.info('Webhook mail received:', {
      total: notifications.length,
      new: newNotifications.length
    });

    const tenantRouteMap: Map<string, Set<string>> = new Map();
    for (const notify of newNotifications) {
      const tenantId = notify?.tenantId;
      const resource = notify?.resource || '';
      const mailbox = parseResourceMailbox(resource);
      if (!tenantRouteMap.has(tenantId)) {
        tenantRouteMap.set(tenantId, new Set<string>());
      }
      const routeSet = tenantRouteMap.get(tenantId)!;
      if (mailbox) {
        routeSet.add(mailbox);
      } else if (routeSet.size === 0) {
        routeSet.add('*');
      }
      logger.info('Processing webhook for tenantId:', tenantId);
      logger.info('MS webhook resource', { tenantId, resource });
    }

    const mailboxRouteKeys: string[] = [];
    for (const [tenantId, routeSet] of tenantRouteMap.entries()) {
      const specificMailboxes = Array.from(routeSet).filter(v => v !== '*');
      if (specificMailboxes.length > 0) {
        for (const mailbox of specificMailboxes) {
          mailboxRouteKeys.push(`${tenantId}:${mailbox}`);
        }
      } else {
        mailboxRouteKeys.push(`${tenantId}:*`);
      }
    }

    await Promise.all(
      mailboxRouteKeys.map(async (routeKey: string) => {
        const sepIndex = routeKey.indexOf(':');
        const tenantId = sepIndex > -1 ? routeKey.slice(0, sepIndex) : routeKey;
        const mailbox = sepIndex > -1 ? routeKey.slice(sepIndex + 1) : '';
        const resource = mailbox && mailbox !== '*' ? `/users/${mailbox}/messages` : '';
        await handleMicrosoftWebhookEmail(tenantId, resource);
      })
    );
  } catch (err) {
    logger.error('Webhook error', err);
  }
}

function parseResourceMailbox(resource: string): string {
  if (!resource) return '';
  const normalized = decodeURIComponent(resource)?.replace(/^https?:\/\/graph\.microsoft\.com\/v1\.0\//i, '')?.replace(/^\/?v1\.0\//i, '')?.replace(/^\/+/, '');
  const direct = normalized.match(/users\/([^/]+)/i);
  if (direct?.[1]) {
    return direct[1].replace(/^['"]|['"]$/g, '').toLowerCase();
  }
  const bracketed = normalized.match(/users\(([^)]+)\)/i);
  if (bracketed?.[1]) {
    return bracketed[1].replace(/^['"]|['"]$/g, '').toLowerCase();
  }
  return '';
}

async function handleMicrosoftWebhookEmail(tenantId: string, resource: string) {
  try {
    const toMicrosoftTenantUsers = (items: any[]) => items.filter((u: any) => {
      if (!u || !u?.emailAddress || u.is_smtp !== 1) return false;
      let cfg: any = {};
      try {
        cfg = JSON.parse(u?.smtp_json || '{}');
      } catch (error) {
        logger.error('Invalid smtp_json in Microsoft user config', {
          tenantId,
          emailAddress: u?.emailAddress,
        });
        return false;
      }
      return cfg.provider === 'MICROSOFT' && cfg.tenantId === tenantId;
    });

    const usersByEmail = new Map<string, any>();
    const tenantUsers: any = await getMicrosoftDBdata(tenantId);
    const allUsers: any = await getMultipleEmail();
    const mergedUsers = []
      .concat(Array.isArray(tenantUsers) ? tenantUsers : [])
      .concat(Array.isArray(allUsers) ? allUsers : []);

    for (const user of toMicrosoftTenantUsers(mergedUsers)) {
      const email = (user?.emailAddress || '').toLowerCase();
      if (!email) continue;
      if (!usersByEmail.has(email)) {
        usersByEmail.set(email, user);
      }
    }

    const microsoftUsers = Array.from(usersByEmail.values());
    if (microsoftUsers.length === 0) return;
    const resourceMailbox = parseResourceMailbox(resource);
    let targetUsers = microsoftUsers;
    if (resourceMailbox) {
      const matched = microsoftUsers.filter((u: any) => (u?.emailAddress || '').toLowerCase() === resourceMailbox);
      if (matched.length > 0) {
        targetUsers = matched;
      }
    }
    logger.info('MS webhook mailbox routing', {
      tenantId,
      resource,
      resourceMailbox,
      totalTenantMailboxes: microsoftUsers.length,
      selectedMailboxes: targetUsers.map((u: any) => u?.emailAddress),
    });

    const syncMailbox = async (user: any) => {
      const mailbox = (user?.emailAddress || '').toLowerCase();
      if (!mailbox) return;

      const mailboxLockKey = `${tenantId}:${mailbox}`;
      if (mailboxProcessingLock.has(mailboxLockKey)) {
        const wasAlreadyPending = mailboxPendingSync.has(mailboxLockKey);
        mailboxPendingSync.add(mailboxLockKey);
        if (!wasAlreadyPending) {
          logger.info('Mailbox sync already running; marked pending', { tenantId, mailbox });
        }
        return;
      }

      mailboxProcessingLock.add(mailboxLockKey);
      try {
        do {
          mailboxPendingSync.delete(mailboxLockKey);
          try {
            const jsonData = JSON.parse(user?.smtp_json || '{}');
            await fetchMicrosoftInbox(user, jsonData);
          } catch (fetchErr: any) {
            logger.error('fetchMicrosoftInbox failed, will retry if pending', {
              tenantId,
              mailbox,
              status: fetchErr?.response?.status,
              message: fetchErr?.message,
            });
          }
        } while (mailboxPendingSync.has(mailboxLockKey));
      } finally {
        mailboxPendingSync.delete(mailboxLockKey);
        mailboxProcessingLock.delete(mailboxLockKey);
      }
    };

    await Promise.all(targetUsers.map((user: any) => syncMailbox(user)));
  } catch (err) {
    logger.error('handleMicrosoftWebhookEmail error', err);
  }
}
