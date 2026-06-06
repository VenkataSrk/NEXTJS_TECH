import axios from 'axios';
import CryptoJS from 'crypto-js';
import { v4 as uuidv4 } from 'uuid';
import { logger } from '../../../src/plugins/log';
import {
  getAllQueueWaitTime,
  getAssingToApi,
  getConnectorTokenInfo,
  getCustomerConatctDetails,
  getDispositinStatus,
  getnodesdata,
  getQueueSettingS,
  getRemainingSessionEmailSent,
  getSupervisiorDetails,
  getSupervisiorList,
  getUserExt,
  getUserListByExt,
  insertCustomerContact,
  updateCustomerFirstName,
  updateCustomerLastName,
} from '../../dao/facebook';
import {
  agentDetailsInstagramApi,
  assignCallBackInstagram,
  deleteInstagramConfigDetails,
  getAdminEmail,
  getBotDetailInstagram,
  getBotMessageResInsta,
  getCompanyDetails,
  getCustomerCoIdInsta,
  getCustomerEmailIdInsta,
  getCustomerSatisficationMsgInsta,
  getExistingCustomerContactInsta,
  getFlowTrackingInsta,
  getInstagramConfigData,
  getInstaMediaUrl,
  getIntsagramConfig,
  getIntsagramConfigDetails,
  getNextNodeInsta,
  getNoOfRepeatsResInsta,
  getPersonalDetailsInstagram,
  getQuestionsAnsInsta,
  getRemaindeResponse,
  insertBotMessageInsta,
  insertBotMessageIsRemainderInsta,
  insertBotMessageRemainderInsta,
  insertCustomerContactEmailInsta,
  insertCustomerContactInsta,
  insertFlowTrackingInsta,
  insertInstagramCallbackI,
  insertInstagramIn,
  insertUpdateInstagramConfig,
  insertUpdateQueueWaitTimeInsta,
  instagramFeedbackUpdateDB,
  sendCaasInstagram,
  updateAppointmentConsultantInsta,
  updateAppointmentdateInsta,
  updateAppointmentDurationInsta,
  updateAppointmentServiceIdInsta,
  updateAppointmentTimeInsta,
  updateAppointmentUserCreateByInsta,
  updateAppointmentUserDetailsInsta,
  updateCallbackstatus,
  updateCallbackStatusInsta,
  updateCategoryIdInsta,
  updateChatMessageInsta,
  updateConnectDataInsta,
  updateCustomerContactInsta,
  updateCustomerFirstNameAppoinmentInsta,
  updateCustomerIdInsta,
  updateCustomerLastNameAppoinmentInsta,
  updateDeleteInstagramIn,
  updateDispositionBotInsta,
  updateDispositionDiscnnectInsta,
  updateDispositionInstagram,
  updateEmailIdInsta,
  updateEmailIdInstagram,
  updateIsBotInsta,
  updateMissedChatInsta,
  updateNextNodeInsta,
  updateNoOfRepeatsInsta,
  updatePageIdInstagram,
  updatePhoneNumberInsta,
  updateSentimentScoreInstagram,
  updateSentimentScoreInstagramBot,
  updateStatusInstagram
} from '../../dao/instagram';
import {
  AppointmentInfo,
  AppointmentInfoPaymentCheckInsta,
  bookingPaymentInsta,
  createBooking,
  getAppointmentInfo,
  getAppointmentLable,
  getAppointmentUserName,
  getCalenderViewDetails,
  getConsultantData,
  getdurationDetails,
  getEndTimeData,
  getLeaveData
} from '../../helpers/appointment';
import { appoinment, channelType, emiiter, messageValue, RESPONSE } from '../../helpers/constants';
import {
  addContextVariable,
  callGoogleSheetSyncAPI,
  callSessionExpiryAPI,
  contextPattern,
  correctjsondata,
  fetchApi,
  getAgentAvailibility,
  getNodeDataById,
  getPlandetailsInsta,
  getResponseData,
  getStartNode,
  getStartNodeCondition,
  insertUpdateCrmContact,
  replacechatPatternWithVariables,
  replacePatternWithVariables,
  safeParseJson,
  sendEmail,
  sendEmailPlainText,
} from '../../helpers/utils';

import { getDispositinDisconnectStatusInsta, sendAllUserNameInsta, sendAppoinmentPaymentInsta, sendAppointmentDateInsta, sendAppointmentDurationInsta, sendAppointmentOneToOneInsta, sendAppointmentResInsta, sendBookingDetailsInsta, sendBookingDetailsOneToOneInsta, sendbookingDurationInsta, sendButtonInsta, sendButtonLinkInsta, sendCallbackRequestInsta, sendCarouselDataInsta, sendCategoryNameInsta, sendConnectLiveAgentInsta, sendCustomerInputInsta, sendCustomerSatificationInsta, sendCustomerSatificationInstaDropdown, sendFlowBreakInpuRemaindertInsta, sendFlowBreakInputInsta, sendImageCustomerInsta, sendImageCustomerInstaBot, sendInstaPdf, sendMessageCustomerInstagram, sendMessageCustomerInstagramFlow, sendMessageInsta, sendTransferAgentInsta } from '../../helpers/instameta';
import { AgentAssist, getInstagramBot, nlpResponse, sentimentalScore, sentimentalScoreDataInsta } from '../../helpers/nlp';
import { ioredisInstagramQueue, ioredisSocialMedia } from '../../plugins/redis';
import {
  transferChat,
  transferChatToAvailableAgent
} from '../../services/transferToQueue';

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function insertInstagramHandler(req: any, res: any, done: any) {
  logger.info('insertInstagramHandler>>>>>', req.body);
  try {
    req.body.sessionId = uuidv4();
    req.body.messageId = uuidv4();
    if (req?.body?.type === 'quickReply' && req?.body?.text?.body?.includes('-') && req?.body?.text?.title !== 'More') {
      instagramFeedbackUpdateDB(req?.body);
      res.send({ statusCode: 200, message: RESPONSE.success_message });
      return;
    }
    const instagramCcaasTable: any = req?.body?.is_deleted ? await updateDeleteInstagramIn(req?.body) : await insertInstagramIn(req?.body);
    logger.info('MysqlInsertData>>>>>', instagramCcaasTable);
    const isEmail: any = await getRemainingSessionEmailSent(instagramCcaasTable[0]);
    logger.info('IsSendEmail>>>>>>>>>', isEmail);
    if (isEmail[0]?.isEmailSent !== '') {
      callSessionExpiryAPI(isEmail[0]);
    }
    const [getCoid, getBotDetails] = await Promise.all([insertCustomerContactInsta(req?.body), getBotDetailInstagram(instagramCcaasTable[0])]);
    logger.info('getConfigurationDetails>>>>>>', getBotDetails);
    getConnectorTokenInfo(instagramCcaasTable[0], (getConnect) => {
      if (getConnect?.length > 0) {
        callGoogleSheetSyncAPI({ contactSyncDetails: getConnect[0]?.contactSyncDetails, newContacts: getCoid[0]?.coid, domainId: instagramCcaasTable[0]?.domainId, token: getConnect[0]?.token });
      }
    });
    updateCustomerIdInsta(instagramCcaasTable[0], getCoid[0]);
    if (instagramCcaasTable[0]?.errcode !== 0) {
      logger.error('Error in instagramCcaasTable:', instagramCcaasTable[0]);
      return res.send({ statusCode: 500, message: messageValue?.processing });
    }
    if (instagramCcaasTable[0]?.errcode !== 0) {
      return res.send({ statusCode: 200, message: RESPONSE.success_message });
    }
    const plan_details = getPlandetailsInsta(getBotDetails[0]);
    logger.info('checkPlan>>>>>>', plan_details);
    if (plan_details === 'Traditional' || instagramCcaasTable[0]?.assignedTo !== null || instagramCcaasTable[0]?.AgentId !== null) {
      await handleTraditionalPlan(instagramCcaasTable, getBotDetails, getCoid, req);
    } else if (plan_details === 'CAI') {
      await handleCAIPlan(instagramCcaasTable, getBotDetails, getCoid, req);
    } else if (plan_details === 'Flow') {
      await insertRoleInstagramHandler(req, instagramCcaasTable);
    }
    return res.send({ statusCode: 200, message: RESPONSE.success_message });
  } catch (err) {
    logger.error('insertInstagramHandler error:', err);
    res.send({ statusCode: 500, message: RESPONSE?.internal_error });
  }
}

/**
 *
 * @param instagramCcaasTable
 * @param getBotDetails
 * @param getCoid
 * @param req
 * @returns
 */
async function handleTraditionalPlan(instagramCcaasTable: any, getBotDetails: any, getCoid: any, req: any) {
  const [{ sessionId, domainId, IsUpdate, agentFacebookId }] = instagramCcaasTable;
  const [agentDetails, comDetails] = await Promise.all([
    agentDetailsInstagramApi(sessionId, domainId),
    getCompanyDetails(instagramCcaasTable[0])
  ]);
  const { assignedTo, customerInstagramId } = agentDetails[0] || {};
  const getAssingToEXt: any = assignedTo ? await getAssingToApi(assignedTo) : [];
  const redisChannel = `${domainId}_${getAssingToEXt?.[0]?.ext || ''}`;
  if (IsUpdate === 0 || !getAssingToEXt?.length) {
    const liveAgent = await transferChatToAvailableAgent(instagramCcaasTable[0]);
    return connectToliveAgent(liveAgent, instagramCcaasTable, getBotDetails, getCoid);
  }
  const assistId = req?.body?.messageId;
  const [getPersonalData, nlpRes]: any = await Promise.all([
    getPersonalDetailsInstagram(customerInstagramId, domainId),
    AgentAssist(instagramCcaasTable[0], comDetails, getAssingToEXt[0]?.ext, req?.body?.text?.body, assistId)
  ]);
  logger.info(messageValue?.appendMessage, `${domainId}_${getAssingToEXt[0]?.ext}`);
  ioredisSocialMedia.to(redisChannel).emit(emiiter?.receivedInstagram, { ...agentDetails[0], agentAssist: true, calledPersonDetails: getPersonalData?.[0], messageId: assistId, }, sessionId);
  const enrichedData = {
    ...nlpRes.data,
    channelId: agentFacebookId,
    messageId: assistId,
    timeStamp: Date.now(),
  };
  ioredisSocialMedia.to(redisChannel).emit(emiiter?.agentAssistOmniChannel, enrichedData);
}

/**
 *
 * @param instagramCcaasTable
 * @param getBotDetails
 * @param getCoid
 * @param req
 * @returns
 */
async function handleCAIPlan(instagramCcaasTable: any, getBotDetails: any, getCoid: any, req: any) {
  try {
    const [{ domainId, sessionId, customerInstagramId, agentInstagramId }] = instagramCcaasTable;
    const supervisorList: any = await getSupervisiorList(instagramCcaasTable[0]);
    const redisChannelBase = `${domainId}_`;
    supervisorList.forEach((ext: any) => {
      ioredisSocialMedia?.to(`${redisChannelBase}${ext?.ext}`).emit(emiiter?.supervisiorActivityBot);
    });
    let mediaUrl: any = '';
    if (['audio'].includes(req?.body?.type)) {
      const mediaRes = await getInstaMediaUrl(req?.body);
      if (mediaRes?.mime_type?.includes('audio/ogg')) {
        mediaUrl = mediaRes;
      }
    }
    console.log('mediaUrl>>>>>>', mediaUrl);
    const comDetails = await getCompanyDetails(instagramCcaasTable[0]);
    const botResponse: any = await getInstagramBot(req?.body?.text, getBotDetails[0], instagramCcaasTable[0], comDetails, getCoid, mediaUrl);
    if (botResponse?.data?.intent === 'transfer_live_agent') {
      // const liveAgent = await transferChatToAvailableAgent(instagramCcaasTable[0]);
      await updateCallbackstatus(instagramCcaasTable[0]);
      const liveAgent: any = await getUserListByExt(instagramCcaasTable[0], botResponse?.data);
      return connectToliveAgent(liveAgent, instagramCcaasTable, getBotDetails, getCoid);
    }
    const rawBotMessage = botResponse?.data?.message ? String(botResponse.data.message) : '';
    const cleanedMessage = rawBotMessage?.replace(/<\/?(p|div|br|li|ul|ol|h[1-6])[^>]*>/gi, '\n')?.replace(/<[^>]+>/g, '')?.replace(/\n\s*\n+/g, '\n')?.trim();
    const hasLink = Boolean(botResponse?.data?.link?.length > 0);
    const hasMedia = Boolean(botResponse?.data?.mediaUri && botResponse?.data?.mimeType.startsWith('image/'));
    const hasPdf = botResponse?.data?.mediaUri && botResponse?.data?.mimeType === 'application/pdf' || botResponse?.data?.mimeType === 'document/pdf';
    const messageType = hasMedia ? 'media' : hasLink ? 'link' : 'text';
    let response:any = '';
  /** ---------------- SEND MESSAGE ---------------- */
    if (hasLink) {
      response = await sendButtonInsta(instagramCcaasTable[0], botResponse?.data?.link, cleanedMessage);
    } else if (hasMedia) {
      response = await sendImageCustomerInstaBot(instagramCcaasTable[0], botResponse?.data);
    } else if (hasPdf) {
      response = await sendInstaPdf(instagramCcaasTable[0], botResponse?.data, cleanedMessage);
    } else {
      response = await sendMessageCustomerInstagram(instagramCcaasTable[0], rawBotMessage);
    }

  /** ---------------- SAVE TO DB ---------------- */
    const messagePayload = {
      sessionId,
      message: {
        agentText: {
          message: {
            sessionId,
            messageType,
            messageID: uuidv4(),
            message: rawBotMessage,
            sendAt: Date.now(),
            isBot: true,
            isCustomer: false,
            direction: 'OUT',
            id: req?.body?.id ?? '',
            is_deleted: false,
            sendMediaUrl: hasMedia ? botResponse.data.mediaUri : '',
            mimeType:botResponse?.data?.mimeType ?? '',
            replayId: response?.data?.id ?? ''
          }
        }
      },
      customerId: customerInstagramId,
      pageId: agentInstagramId,
      IsBot: 1,
      status: 'sent'
    };
    await insertInstagramIn(messagePayload);
  } catch (error) {
    logger.error('Error in handleCAIPlan (Instagram):', error);
    throw error;
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export const sendCaasInstagramHandler = async (req: any, res: any, done: any) => {
  try {
    const data: any = {
      sessionId: req.body.sessionId,
      customerId: req.body.customerId,
      pageId: req.body.pageId,
      text: req.body.text,
      type: req.body.type ?? 'update',
      attachments: req.body.attachments ?? null,
      agentName: req?.body?.agentName,
      replayId: req?.body?.replayId ?? '',
      whisper: req?.body?.whisper ?? false,
    };
    const messageList: any = {
      agentText: {
        message: {
          customerId: data?.customerId ?? '',
          pageId: data?.pageId ?? '',
          messageId: uuidv4(),
          sendAt: Date.now(),
          message: data?.text ?? '',
          attachments: data?.attachments ?? null,
          isBot: false,
          agentName: data?.agentName,
          replayId: data?.replayId,
          whisper: data?.whisper,
        },
      },
    };
    const sendMessage: any = {
      message: messageList,
      customerId: data.customerId,
      pageId: data.pageId,
    };
    const instagramOut = await sendCaasInstagram(sendMessage, data.sessionId);
    logger.info('instagramOut', instagramOut);
    const getSupervisiorExt: any = await getSupervisiorList(instagramOut[0]);
    const getPersonalData: any = await getPersonalDetailsInstagram(data?.customerId, instagramOut[0]?.domainId);
    getSupervisiorExt.forEach((ext: any) => {
      console.log(emiiter?.receivedInstagram, `${instagramOut[0]?.domainId}_${ext.ext}`);
      ioredisSocialMedia.to(`${instagramOut[0]?.domainId}_${ext.ext}`).emit(emiiter?.receivedInstagram, { ...instagramOut[0], channelType: channelType?.Instagram, agentAssist: false, calledPersonDetails: getPersonalData[0] }, data.sessionId);
    });
    if (instagramOut[0].errcode === 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.Instagram_send, InstagramSendResponse: messageList });
    } else {
      res.status(200).send({ statusCode: 200, message: RESPONSE.not_found });
    }
    res.status(200).send({ statusCode: 200, message: RESPONSE.Instagram_send, InstagramSendResponse: messageList });
  } catch (err) {
    logger.error('sendCaasInstagramHandler: err', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
};

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function updateDispositionInstagramHandler(
  req: any,
  res: any,
  done: any
) {
  try {
    const auth: any = req.headers;
    const params = req?.params;
    const data: any = {
      InstagramHistoryId: params?.InstagramHistoryId,
      domainId: req?.headers?.domainId,
      dispositionId: req?.body?.dispositionId ?? 0,
      summary: req?.body?.summary ?? null,
      isSessionClosed: req?.body?.isSessionClosed ?? 0,
      afterInstagramWorkTime: req?.body?.afterWorkTime ?? null,
      dispositionUpdatedBy: req?.body?.dispositionUpdatedBy ?? null,
      duration: req?.body?.duration ?? null,
      dispositionSubject: req?.body?.dispositionSubject ?? null,
      tags: req?.body?.tags ?? null,
    };
    logger.info('updateDispositionInstagramHandler - Request Data:', data);
    const updateDispStatus: any = await updateDispositionInstagram(data);
    logger.info('updateDispStatus', updateDispStatus);
    if (data?.isSessionClosed === 1) {
      const getSuperVisorExt: any = await getSupervisiorDetails(data?.domainId);
      getSuperVisorExt.forEach((ext: any) => {
        logger.info(emiiter?.interActionClosed, `${data?.domainId}_${ext.ext}`);
        ioredisSocialMedia.to(`${data?.domainId}_${ext.ext}`).emit(emiiter?.interActionClosed);
      });
    }
    const nlpData: any = {
      HistoryId: data?.InstagramHistoryId,
      ChatDurationHistoryId: data?.ChatDurationHistoryId ?? 0,
      channelType: channelType?.Instagram,
      domainId: data?.domainId,
      ext: data?.ext ?? 0
    };
    const updateScore: any = await sentimentalScore(nlpData);
    if (updateScore.status === 200) {
      await updateSentimentScoreInstagram(data, updateScore.data);
      if (updateScore.data) {
        ioredisSocialMedia.to(`${auth.domainId}_${auth.ext}`).emit(emiiter?.SentimentScoreAnalytics, { data, speedometerScore: updateScore.data.sentimentalScore, sentimentalScoreUser: updateScore.data.sentimentalScoreUser, sentimentalScoreAgent: updateScore.data.sentimentalScoreAgent });
      } else {
        ioredisSocialMedia.to(`${auth.domainId}_${auth.ext}`).emit(emiiter?.SentimentScoreAnalytics, { data, speedometerScore: 0, sentimentalScoreUser: 0, sentimentalScoreAgent: 0 });
      }
    } else {
      ioredisSocialMedia.to(`${auth.domainId}_${auth.ext}`).emit(emiiter?.SentimentScoreAnalytics, { data, speedometerScore: 0, sentimentalScoreUser: 0, sentimentalScoreAgent: 0 });
    }
    if (updateDispStatus) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.disposition_statuss,
        result: data,
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
    const isTransferred = updateDispStatus?.[1][0]?.Istransferred;
    if (isTransferred !== 1) {
      const customerSatisfactionResult: any = await getCustomerSatisficationMsgInsta(data);
      await sendCustomerSatificationInsta(customerSatisfactionResult[0]);
    }
  } catch (err) {
    logger.error('updateDispositionInstagramHandler: err', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function updateDispositionInstagramCallbackHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const params = req?.params;
    const data: any = {
      InstagramHistoryId: params?.InstagramHistoryId,
      domainId: req?.body?.domainId,
      dispositionId: req?.body?.dispositionId ?? 0,
      summary: req?.body?.summary ?? null,
      isSessionClosed: req?.body?.isSessionClosed ?? 0,
      afterInstagramWorkTime: req?.body?.afterWorkTime ?? null,
      dispositionUpdatedBy: req?.body?.dispositionUpdatedBy ?? null,
      duration: req?.body?.duration ?? null,
      dispositionSubject: req?.body?.dispositionSubject ?? null,
    };
    const updateDispStatus: any = await updateDispositionInstagram(data);
    logger.info('updateDispStatus', updateDispStatus);
    if (data?.isSessionClosed === 1) {
      const getSuperVisorExt: any = await getSupervisiorDetails(data?.domainId);
      getSuperVisorExt.forEach((ext: any) => {
        console.log(emiiter?.interActionClosed, `${data?.domainId}_${ext.ext}`);
        ioredisSocialMedia.to(`${data?.domainId}_${ext.ext}`).emit(emiiter?.interActionClosed);
      });
    }
    const nlpData: any = {
      HistoryId: data?.InstagramHistoryId,
      ChatDurationHistoryId: data?.ChatDurationHistoryId ?? 0,
      channelType: channelType?.Instagram,
      domainId: data?.domainId,
      ext: data?.ext ?? 0
    };
    const updateScore: any = await sentimentalScore(nlpData);
    if (updateScore.status === 200) {
      await updateSentimentScoreInstagram(data, updateScore.data);
      if (updateScore.data) {
        ioredisSocialMedia.to(`${data?.domainId}`).emit(emiiter?.SentimentScoreAnalytics, { data, speedometerScore: updateScore.data.sentimentalScore, sentimentalScoreUser: updateScore.data.sentimentalScoreUser, sentimentalScoreAgent: updateScore.data.sentimentalScoreAgent });
      } else {
        ioredisSocialMedia.to(`${data?.domainId}`).emit(emiiter?.SentimentScoreAnalytics, { data, speedometerScore: 0, sentimentalScoreUser: 0, sentimentalScoreAgent: 0 });
      }
    } else {
      ioredisSocialMedia.to(`${data?.domainId}`).emit(emiiter?.SentimentScoreAnalytics, { data, speedometerScore: 0, sentimentalScoreUser: 0, sentimentalScoreAgent: 0 });
    }
    if (updateDispStatus) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.disposition_statuss,
        result: data,
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {
    logger.error('updateDispositionInstagramCallbackHandler: err', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function insertUpdateInstagramConfigHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      instagramConfigurationId: req?.body?.instagramConfigurationId ?? null,
      pageName: req?.body?.pageName ?? null,
      language: req?.body?.language ?? null,
      domainId: auth.domainId,
      planId: req?.body?.planId ?? null,
      isBot: req?.body?.isBot ?? 0,
      isBotPurchased: req?.body?.isBotPurchased ?? null,
      instagramFlowId: req?.body?.instagramFlowId ?? null,
      instagramPageId: req?.body?.instagramPageId ?? null,
      isActive: req?.body?.isActive ?? null,
      instagramintegrationId: req?.body?.instagramintegrationId ?? 1,
      callBackToggle: req?.body?.callBackToggle ?? null,
      callBackForm: req?.body?.callBackForm ?? null,
      notifyAdmin: req?.body?.notifyAdmin ?? null,
      KnowledgeBaseId: req?.body?.KnowledgeBaseId ?? null
    };
    const insertInstagram: any = await insertUpdateInstagramConfig(data);
    logger.info('insertInstagram', insertInstagram);
    if (insertInstagram[0]?.errCode === 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.insertInstagram });
    } else {
      res.status(200).send({ statusCode: 200, message: RESPONSE.updateInstagram });
    }
  } catch (err) {
    logger.error('insertUpdateInstagramConfigHandler: err', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 */
export const getIntsagramConfigHandler = async (req: any, res: any) => {
  try {
    const data: any = {
      domainId: req?.headers?.domainId
    };
    const getInstagramData: any = await getIntsagramConfig(data);
    logger.info('getInstagramData', getInstagramData);
    if (getInstagramData && getInstagramData.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getInstagramDataRes: getInstagramData });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getInstagramDataRes: [] });
    }
  } catch (err) {
    logger.error('getIntsagramConfigHandler: err', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
};

/**
 *
 * @param req
 * @param res
 */
export const getIntsagramCallbckConfigHandler = async (req: any, res: any) => {
  try {
    const data: any = {
      domainId: req?.params?.domainId
    };
    const getInstagramData: any = await getIntsagramConfig(data);
    logger.info('getInstagramData', getInstagramData);
    if (getInstagramData && getInstagramData.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getInstagramDataRes: getInstagramData });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getInstagramDataRes: [] });
    }
  } catch (err) {
    logger.error('getIntsagramCallbckConfigHandler: err', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
};

/**
 *
 * @param req
 * @param res
 */
export const getIntsagramConfigDetailsHandler = async (req: any, res: any) => {
  try {
    const data: any = {
      domainId: req?.headers?.domainId,
      instaId: req?.params?.instaId ?? 0
    };
    const getInstagramDetails: any = await getIntsagramConfigDetails(data);
    logger.info('getInstagramDetails', getInstagramDetails);
    if (getInstagramDetails && getInstagramDetails.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getInstagramDetailsRes: getInstagramDetails });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getInstagramDetailsRes: [] });
    }
  } catch (err) {
    logger.error('getIntsagramConfigDetailsHandler: err', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
};

/**
 * Delete email
 * @param req
 * @param res
 * @param done
 */
export const deleteInstagramConfigDetailsHandler = async (req: any, res: any, done: any) => {
  try {
    const auth: any = req.headers;
    const data: any = {
      instaId: req.params.instaId,
      domainId: auth.domainId
    };
    const deleteInstagram = await deleteInstagramConfigDetails(data);
    logger.info('deleteInstagram', deleteInstagram);
    if (deleteInstagram[0]?.errcode === 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.instagramConfigId });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.instagramNot });
    }
  } catch (err) {
    logger.error('deleteInstagramConfigDetailsHandler: err', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
};

/**
 * Delete email
 * @param req
 * @param res
 * @param done
 */
export const updateStatusInstagramHandler = async (req: any, res: any, done: any) => {
  try {
    const auth: any = req.headers;
    const data: any = {
      instaConfigId: req?.body?.instaConfigId,
      domainId: auth.domainId,
      isActive: req?.body?.isActive
    };
    const statusInstagram = await updateStatusInstagram(data);
    logger.info('statusInstagram', statusInstagram);
    if (statusInstagram[0]?.Enable === 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.status });
    } else {
      res.status(200).send({ statusCode: 200, message: RESPONSE.statusDisabled });
    }
  } catch (err) {
    logger.error('updateStatusInstagramHandler: err', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
};

/**
 * update page id
 * @param req
 * @param res
 * @param done
 */
export const updatePageIdInstagramHandler = async (req: any, res: any, done: any) => {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: req?.body?.domainId,
      InstagramPageId: req?.body?.InstagramPageId,
      pageName: req?.body?.pageName
    };
    const instagramPage = await updatePageIdInstagram(data);
    logger.info('instagramPage', instagramPage);
    if (instagramPage[0]?.errcode === 1) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.instagramPage });
    } else {
      res.status(200).send({ statusCode: 200, message: RESPONSE.instagramNotUpdate });
    }
  } catch (err) {
    logger.error('updatePageIdInstagramHandler: err', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
};

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function assignCallBackInstagramHandlers(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      assignedBy: req?.body?.assignedBy ?? null,
      assignedTo: req?.body?.assignedTo ?? null,
      assignedOn: Date.now(),
      InstagramHistoryId: req?.body?.InstagramHistoryId,
      Istransferred: req?.body?.Istransferred ?? null,
      channelType: req?.body?.ChannelType ?? channelType?.Instagram,
      calledPersonDetails: req?.body?.calledPersonDetails ?? null,
      ext: req?.body?.ext
    };
    const assignRes: any = await assignCallBackInstagram(data);
    logger.info('assignRes', assignRes);
    try {
      await sendTransferAgentInsta(assignRes[0]);
      ioredisSocialMedia.to(`${assignRes[0]?.domainId}_${data?.ext}`).emit(emiiter?.sendInvite, {
        channelType: data?.channelType, customerNumber: assignRes[0]?.customerInstagramId,
        agentNumber: assignRes[0]?.agentInstagramId, sessionId: assignRes[0]?.sessionId, historyId: assignRes[0]?.InstagramHistoryId,
        calledPersonDetails: data?.calledPersonDetails ?? null,
        allAgent: true
      });
    } catch (error) {
      console.log(error);
    }
    if (assignRes) {
      res
        .status(200)
        .send({ statusCode: 200, message: RESPONSE.update_asssign });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {
    logger.error('assignCallBackInstagramHandlers: err', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function insertInstagramCallbackHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      domainId: req.body?.domainId,
      sessionId: req.body?.sessionId,
      IsInBound: req.body?.IsInBound,
      customerInstagramId: req.body?.customerInstagramId ?? null,
      agentInstagramId: req.body?.agentInstagramId ?? null,
      InstagramHistoryId: req?.body?.InstagramHistoryId ?? null
    };
    const insertInstagramCallback: any = await insertInstagramCallbackI(data);
    logger.info('insertInstagramCallback', insertInstagramCallback);
    if (insertInstagramCallback && insertInstagramCallback?.length > 0) {
      const getSupervisiorExt: any = await getSupervisiorList(data);
      getSupervisiorExt.forEach((ex: any) => {
        console.log(emiiter?.supervisiorActivity, `${data?.domainId}_${ex.ext}`);
        ioredisSocialMedia.to(`${data?.domainId}_${ex.ext}`).emit(emiiter?.supervisiorActivity);
      });
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getInstagramCallBackResponse: insertInstagramCallback });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getInstagramCallBackResponse: [] });
    }
  } catch (err) {
    logger.error('insertInstagramCallbackHandler: err', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function sendAppoinmentPaymentInstaHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      domainId: req?.body?.domainId,
      calledNumber: req?.body?.calledNumber,
      customerNumber: req?.body?.customerNumber,
      p_book_form: req?.body?.p_book_form ?? '',
      app_id: req?.body?.app_id ?? '-',
      p_book_date: req?.body?.p_book_date ?? '-',
      p_book_starttime: req?.body?.p_book_starttime ?? '-',
      p_book_endtime: req?.body?.p_book_endtime ?? '-',
      service: req?.body?.service ?? '-',
      service_name: req?.body?.service_name ?? '-',
      p_app_type: req?.body?.p_app_type
    };
    const getPayment: any = await sendAppoinmentPaymentInsta(data);
    logger.info('getPayment', getPayment);
    if (getPayment) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.faild });
    }
  } catch (err) {
    logger.error('sendAppoinmentPaymentInstaHandler: err', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @returns
 */
/* tslint:disable */
export async function insertRoleInstagramHandler(req: any, instagramCcaasTable: any) {
  try {
    let currentNodeId = null;
    let nodes = null;
    let getNlpData: any = '';
    let nlpdata: any = '';
    let flow_data: any = [];
    const getInstagramDetails = await getInstagramConfigData(instagramCcaasTable[0]);
    if (getInstagramDetails[0]?.InstagramFlowId) {
      const nodesString: any = await getnodesdata(getInstagramDetails[0]?.InstagramFlowId);
      nodes = JSON.parse(nodesString);
      const startingNode = await getStartNode(nodes);
      currentNodeId = startingNode?.data?.children[0]?.childId ?? '';
    }
    const getNode = await getNextNodeInsta(instagramCcaasTable[0]);
    const nodeDetails = await getNodeDataById(getNode[0]?.nextFlowNode, nodes);
    const parsedData = safeParseJson(nodeDetails?.data?.information?.data);
    const inputType: any = nodeDetails?.data?.information?.inputType;
    if (inputType === 14) {
      flow_data = parsedData?.getInputData?.options?.map((item: any) => item?.value) || [];
    } else if (inputType === 15 || inputType === 1) {
      flow_data = parsedData?.options?.map((item: any) => item?.value) || [];
    }
    const comDetails = await getCompanyDetails(instagramCcaasTable[0]);
    logger.info('comDetails', comDetails);
    if ((!['file', 'document', 'image', 'video', 'audio'].includes(req?.body?.type) && req?.body?.type !== "quickReply" && req?.body?.type !== "facebookPostbackButton" && req?.body?.payload !== '100' && req?.body?.payload !== '200' && req?.body?.payload !== '10001' && req?.body?.payload !== '20002')) {
      getNlpData = await nlpResponse(req?.body, comDetails, instagramCcaasTable[0], getInstagramDetails[0], flow_data, inputType);
      nlpdata = JSON.parse(getNlpData?.data?.entity);
    }
    if (getNlpData?.data?.intent === 'transfer_live_agent') {
      try {
        const liveAgent = await transferChatToAvailableAgent(instagramCcaasTable[0]);
        const getCoid = await getCustomerCoIdInsta(instagramCcaasTable[0]);
        await connectToliveAgent(liveAgent, instagramCcaasTable, getInstagramDetails, getCoid);
        return;
      } catch (error) {
        console.log(error);
      }
    }
    const currentNode_ref = await getNodeDataById(getNode[0].nextFlowNode, nodes);
    const getBotMessage: any = await getBotMessageResInsta(instagramCcaasTable[0]);
    const getRemainder = await getRemaindeResponse(instagramCcaasTable[0]);
    if (req?.body?.text?.body === 'Yes' && req?.body?.payload === '100') { // Yes
      await sendMessageCustomerInstagram(instagramCcaasTable[0], getBotMessage[0]?.message);
      sendMessageCustomerCaiMessageInsta(instagramCcaasTable, getBotMessage[0]?.message);
    } else if (req?.body?.text?.body === 'No' && req?.body?.payload === '200') { // No
      await updateNextNodeInsta(instagramCcaasTable[0], getNode[0]?.nextFlowNode);
      return handleChatFlowType(null, instagramCcaasTable, nodes, '', req);
    } else if (req?.body?.text?.body === 'Yes' && req?.body?.payload === '10001') { // Remainder Yes
      await updateNextNodeInsta(instagramCcaasTable[0], getNode[0]?.nextFlowNode);
      return handleChatFlowType(null, instagramCcaasTable, nodes, '', req);
    } else if (req?.body?.text?.body === 'No' && req?.body?.payload === '20002') { // // Remainder No
      const parsedMessages = JSON.parse(getRemainder[0].message)[4]?.message;
      await sendMessageCustomerInstagram(instagramCcaasTable[0], parsedMessages);
      sendMessageCustomerCaiMessageInsta(instagramCcaasTable, parsedMessages);
    } else if (currentNode_ref?.data?.information?.inputType === 16 && ['image', 'video', 'audio'].includes(req?.body.type)) {
      await uploadFilesFlowInsta(req?.body, instagramCcaasTable, currentNode_ref, nodes, getNlpData, req);
    } else if (req?.body?.type !== 'text' && req?.body?.type !== 'facebookCarousel' && req?.body?.type !== "quickReply") {
      let nextData: any = null;
      if (currentNode_ref?.data?.children) {
        for (const child_data of currentNode_ref.data.children) {
          if (child_data?.type === req.body.text.body) {
            nextData = child_data;
            break;
          }
        }
      }
      await updateNextNodeInsta(instagramCcaasTable[0], nextData?.childId);
      handleChatFlowType(null, instagramCcaasTable, nodes, '', req);
    } else if (req?.body?.type === 'facebookCarousel') {
      const currentNode_Insta = await getNodeDataById(req?.body?.payload, nodes);
      const nextData = currentNode_Insta?.data?.children[0];
      await updateNextNodeInsta(instagramCcaasTable[0], nextData?.childId);
      return handleChatFlowType(null, instagramCcaasTable, nodes, '', req);
    } else if (req?.body?.type === "quickReply" && getNode[0]?.nextFlowNode?.split('_')[0] !== 'customInput') {
      const getAppointmentFlow: any = await getFlowTrackingInsta(instagramCcaasTable[0]);
      const getAppDetails = await AppointmentInfo(getAppointmentFlow);
      const DurationType = await getAppointmentInfo(getAppointmentFlow?.appId, getAppointmentFlow?.companyId);
      const lable = await getAppointmentLable(getAppointmentFlow?.companyId);
      const ServiceName = await getAppointmentInfo(getAppointmentFlow?.appId, getAppointmentFlow?.companyId);
      if (getAppointmentFlow?.processflow === 'categories') {
        await updateCategoryIdInsta(instagramCcaasTable[0], getAppointmentFlow, req.body.text.body);
        const getcategories: any = await getFlowTrackingInsta(instagramCcaasTable[0]);
        const matchedCategory = ServiceName
          .filter((item: any) => item.categoryId === +(getcategories?.categoryId))
          .map((item: any) => ({
            serviceid: item.serviceid,
            service_name: item.service_name
          }));
        logger.info("Select service", matchedCategory);
        await sendAppointmentResInsta(instagramCcaasTable[0], matchedCategory, lable);
        let datas: any = {
          appointmentId: getAppointmentFlow?.appId,
          companyId: getAppointmentFlow?.companyId,
          userId: getAppointmentFlow?.userId
        }
        if (getAppDetails[0]?.consl_selection_id === 2) {
          await insertFlowTrackingInsta(instagramCcaasTable[0], "consultant", getAppointmentFlow);
        } else {
          await insertFlowTrackingInsta(instagramCcaasTable[0], "service", datas);
        }
      } else if (getAppointmentFlow?.processflow === 'service') {
        await updateAppointmentServiceIdInsta(instagramCcaasTable[0], getAppointmentFlow, req.body.text.body);
        const getUserData = await getFlowTrackingInsta(instagramCcaasTable[0]);
        const UserName = await getAppointmentUserName(getAppointmentFlow, getUserData);
        if (getAppDetails[0]?.consl_selection_id === 1) {
          const getUser = UserName.map((name: any) => ({ content_type: "text", title: name?.userName, payload: name?.UserId }));
          await sendAllUserNameInsta(instagramCcaasTable[0], getUser);
        } else if (getAppDetails[0]?.consl_selection_id === 2) {
          const getConsultantDatas = await getConsultantData(getAppointmentFlow, getUserData, getAppDetails[0]);
          await updateAppointmentConsultantInsta(instagramCcaasTable[0], getAppointmentFlow, getConsultantDatas[0]);
        } else {
          const getUser = UserName.map((name: any) => ({ content_type: "text", title: name?.userName, payload: name?.UserId, }));
          getUser?.unshift({ content_type: "text", title: "Any available", payload: 1, })
          await sendAllUserNameInsta(instagramCcaasTable[0], getUser);
        }
        await insertFlowTrackingInsta(instagramCcaasTable[0], "consultant", getAppointmentFlow);
      } else if (getAppointmentFlow?.processflow === 'consultant') {
        if (getAppDetails[0]?.consl_selection_id === 2) {
          await updateAppointmentServiceIdInsta(instagramCcaasTable[0], getAppointmentFlow, req.body.text.body);
        }
        const getUserDatas = await getFlowTrackingInsta(instagramCcaasTable[0]);
        const getLeaveDates = await getLeaveData(getUserDatas);
        const leaveDatas = getLeaveDates.filter((_l: any) => _l.is_leave === 1);
        const getConsultant = await getConsultantData(getAppointmentFlow, getUserDatas, getAppDetails[0]);
        await updateAppointmentConsultantInsta(instagramCcaasTable[0], getAppointmentFlow, getConsultant[0]);
        const dataRange = JSON.parse(getAppDetails[0]?.data_ranges);
        // const fromDate = dataRange[0]?.fromDate;
        const fromDate = new Date().toISOString().split('T')[0];
        const toDate = dataRange[0]?.toDate;
        const result = getDateRange(fromDate, toDate);
        const filteredDates = result.filter((item: any) => !leaveDatas.some((leave: any) => leave?.leaveDate === item));
        let filteredData: any = [];
        let dateObjects: any = getNext10Days(filteredDates);
        const Records = filteredDates?.length > 10;
        if (Records) {
          const lastDate = dateObjects[dateObjects.length - 1];
          const lastDateObj = new Date(lastDate?.title);
          const nextDate = new Date(lastDateObj);
          nextDate.setDate(lastDateObj.getDate() + 1);
          const formattedNextDate = nextDate.toISOString().split('T')[0];
          dateObjects.push({ content_type: "text", title: 'More', payload: `10 ${formattedNextDate}` })
        } else {
          filteredData = dateObjects.filter((item: any) => item.title !== undefined);
        }
        logger.info("Select date", Records ? dateObjects : filteredData);
        const selectDate = Records ? dateObjects : filteredData;
        logger.info("Select date", dateObjects);
        await sendAppointmentDateInsta(instagramCcaasTable[0], selectDate);
        await insertFlowTrackingInsta(instagramCcaasTable[0], "date", getAppointmentFlow);
      } else if (getAppointmentFlow?.processflow === "date") {
        if (req?.body?.text?.title === 'More') {
          const getUserDatas = await getFlowTrackingInsta(instagramCcaasTable[0]);
          const getLeaveDates = await getLeaveData(getUserDatas);
          const leaveDatas = getLeaveDates.filter((_l: any) => _l.is_leave === 1);
          const dataRange = JSON.parse(getAppDetails[0]?.data_ranges);
          const fromDate = req?.body?.text?.title === 'More' ? req?.body?.text?.body.split(' ')[1] : dataRange[0]?.fromDate;
          const toDate = dataRange[0]?.toDate;
          const result = getDateRange(fromDate, toDate);
          const filteredDates = result.filter((item: any) => !leaveDatas.some((leave: any) => leave?.leaveDate === item));
          let filteredData: any = [];
          const dateObjects = getNext10Days(filteredDates);
          const Records = filteredDates?.length > 10;
          if (Records) {
            const lastDate = dateObjects[dateObjects.length - 1];
            const lastDateObj = new Date(lastDate?.title);
            const nextDate = new Date(lastDateObj);
            nextDate.setDate(lastDateObj.getDate() + 1);
            const formattedNextDate = nextDate.toISOString().split('T')[0];
            dateObjects.push({ content_type: "text", title: 'More', payload: `10 ${formattedNextDate}` })
          } else {
            filteredData = dateObjects.filter((item: any) => item.title !== undefined);
          }
          logger.info("Select date", Records ? dateObjects : filteredData);
          const selectDate = Records ? dateObjects : filteredData
          await sendAppointmentDateInsta(instagramCcaasTable[0], selectDate);
          return;
        }
        await updateAppointmentdateInsta(instagramCcaasTable[0], getAppointmentFlow, req.body.text.title);
        if (getAppDetails[0]?.app_typeid === 1 || getAppDetails[0]?.app_typeid === 2 || getAppDetails[0]?.app_typeid === 3) {
          const durationData = JSON.parse(getAppDetails[0]?.serv_dur_pricing);
          const getDurationRes = durationData.map((d: any, index: Number) => ({ content_type: "text", title: +(d.scheduleHours) === 1 ? `${+(d.service)} mins` : `${+(d.service / 60)} hours`, payload: index }));
          await sendAppointmentOneToOneInsta(instagramCcaasTable[0], getDurationRes, lable);
        } else {
          const durationDetails = await getdurationDetails(getAppointmentFlow);
          const duration = durationDetails.filter((data: any) => data?.serviceid === +(getAppointmentFlow?.serviceId));
          const serviceIndex = duration.findIndex((item: any) => item.serviceid === +(getAppointmentFlow?.serviceId));
          const selectDuration = duration[0].amount_collected.split("|")[serviceIndex];
          await sendAppointmentDurationInsta(instagramCcaasTable[0], selectDuration, lable);
        }
        await insertFlowTrackingInsta(instagramCcaasTable[0], "duration", getAppointmentFlow);
      } else if (getAppointmentFlow?.processflow === "duration") {
        if (req?.body?.text.title !== "More") {
          await updateAppointmentDurationInsta(instagramCcaasTable[0], getAppointmentFlow, req?.body?.text);
        }
        const getDurationData: any = await getFlowTrackingInsta(instagramCcaasTable[0]);
        const calenderView: any = await getCalenderViewDetails(getDurationData, getAppDetails, DurationType, getAppointmentFlow);
        const { title, body } = req?.body?.text || {};
        let startIndex = title === 'More' ? parseInt(body, 10) || 0 : 0;
        const durationData = calenderView?.slice(startIndex, startIndex + 9)?.map((duration: any, index: number) => ({
          content_type: "text",
          title: duration?.start,
          payload: index,
        })) || [];
        const hasMoreRecords = calenderView?.length > startIndex + 9;
        if (hasMoreRecords) {
          durationData.push({
            content_type: "text",
            title: "More",
            payload: startIndex + 9,
          });
        }
        logger.info("Select duration", durationData);
        if (durationData?.length === 0) {
          const text = appoinment?.selectDifferent
          await sendMessageCustomerInstagram(instagramCcaasTable[0], text);
          await insertFlowTrackingInsta(instagramCcaasTable[0], "date", getAppointmentFlow);
          return;
        }
        await sendbookingDurationInsta(instagramCcaasTable[0], durationData, lable);
        if (req?.body?.text?.title !== 'More' && durationData?.length > 0) {
          await insertFlowTrackingInsta(instagramCcaasTable[0], "time", getAppointmentFlow);
        }
      } else if (getAppointmentFlow?.processflow === "time") {
        if (req?.body?.text?.title === 'More') {
          const getDurationData: any = await getFlowTrackingInsta(instagramCcaasTable[0]);
          const calenderView: any = await getCalenderViewDetails(getDurationData, getAppDetails, DurationType, getAppointmentFlow);
          const { title, body } = req?.body?.text || {};
          let startIndex = title === 'More' ? parseInt(body, 10) || 0 : 0;
          const durationData = calenderView?.slice(startIndex, startIndex + 9)?.map((duration: any, index: number) => ({
            content_type: "text",
            title: duration?.start,
            payload: index,
          })) || [];
          const hasMoreRecords = calenderView?.length > startIndex + 9;
          if (hasMoreRecords) {
            durationData.push({
              content_type: "text",
              title: "More",
              payload: startIndex + 9,
            });
          }
          logger.info("Select duration", durationData);
          await sendbookingDurationInsta(instagramCcaasTable[0], durationData, lable);
          return;
        }
        await updateAppointmentTimeInsta(instagramCcaasTable[0], getAppointmentFlow, req?.body?.text);
        const contactData = await getPersonalDetailsInstagram(instagramCcaasTable[0]?.customerInstagramId, instagramCcaasTable[0]?.domainId);
        const getProcessTime: any = await getFlowTrackingInsta(instagramCcaasTable[0]);
        const inputData = [
          { "First name": getProcessTime?.firstName },
          { "Last name": getProcessTime?.lastName },
          { "Phone number": getProcessTime?.phoneNumber },
          { "Email": getProcessTime?.emailId }
        ];
        const endTimeData = await getEndTimeData(getProcessTime, getAppDetails, DurationType, getAppointmentFlow);
        let typeValue: any;
        if (getAppDetails[0]?.app_typeid === 1 || getAppDetails[0]?.app_typeid === 2 || getAppDetails[0]?.app_typeid === 3) {
          const value = JSON.parse(getAppDetails[0]?.serv_dur_pricing || "[]");
          const filterValue = value?.filter((type: any) => {
            if (type?.scheduleHours === 1) {
              return +(type?.service) === +(getProcessTime?.processDuration);
            } else {
              return +(type?.service) / 60 === +(getProcessTime?.processDuration);
            }
          });
          typeValue = filterValue?.[0]?.scheduleHours ? +(filterValue[0]?.scheduleHours) : null;
        } else {
          const duration = DurationType.filter((data: any) => data?.serviceid === +(getAppointmentFlow?.serviceId));
          const serviceIndex = duration.findIndex((item: any) => item.serviceid === +(getAppointmentFlow?.serviceId));
          const selectDuration = duration[0].amount_collected.split("|")[serviceIndex];
          const parseData = JSON.parse(selectDuration);
          const filterValue = parseData?.filter((type: any) => {
            if (type?.type === 1) {
              return +(type?.duration) === +(getProcessTime?.processDuration);
            } else {
              return +(type?.duration) / 60 === +(getProcessTime?.processDuration);
            }
          });
          typeValue = filterValue[0]?.type;
        }
        const getPaymentData = await AppointmentInfoPaymentCheckInsta(getAppointmentFlow);
        const FilterPayment = getPaymentData[1]?.filter((data: any) => data?.serviceid === +(getProcessTime?.serviceId));
        const pricingData = getPaymentData[0][0]?.app_typeid === 4 ? FilterPayment[0]?.is_enable_price === 1 : getAppDetails[0]?.is_enable_pricing === 1;
        if (pricingData) {
          await bookingPaymentInsta(inputData, getProcessTime, getAppDetails, endTimeData?.endTime, typeValue, instagramCcaasTable[0], FilterPayment[0]);
        } else {
          const getBooking = await createBooking(inputData, getProcessTime, getAppDetails, endTimeData?.endTime, typeValue);
          logger.info("Appointment Booking successfully", getBooking);
          if (getBooking[0]?.errcode === 0) {
            if (getAppDetails[0]?.app_typeid === 1 || getAppDetails[0]?.app_typeid === 2 || getAppDetails[0]?.app_typeid === 3) {
              await sendBookingDetailsOneToOneInsta(instagramCcaasTable[0], getBooking[0]);
            } else {
              await sendBookingDetailsInsta(instagramCcaasTable[0], getBooking[0]);
            }
          } else {
            const getDurationData: any = await getFlowTrackingInsta(instagramCcaasTable[0]);
            const calenderView: any = await getCalenderViewDetails(getDurationData, getAppDetails, DurationType, getAppointmentFlow);
            const getDuration = calenderView.slice(0, 10).map((duration: any, index: number) => ({
              content_type: "text",
              title: duration?.start,
              payload: index
            }));
            await sendbookingDurationInsta(instagramCcaasTable[0], getDuration, lable);
            await insertFlowTrackingInsta(instagramCcaasTable[0], "time", getAppointmentFlow);
          }
        }
      }
    } else if (req?.body?.type === 'text' && getNode[0].nextFlowNode === null) {
      const isJson = (data: any) => {
        try {
          JSON.parse(data);
          return true;
        } catch (e) {
          return false;
        }
      };
      const json = isJson(getNlpData?.data?.intent) ? JSON.parse(getNlpData?.data?.intent) : getNlpData?.data?.intent;
      const isArry = Array.isArray(json) ? json : [];
      const triggerData = nodes[0]?.data?.information?.trigger_data;
      const trigger = typeof triggerData === 'string' ? JSON.parse(triggerData) : [];
      let matches: any = false;
      if (trigger[0]?.triggerKey[0] === "7") {
        matches = trigger[0]?.inputValues.some((triggerData: any) => {
          return triggerData?.value?.toLowerCase() === req?.body?.text?.body?.toLowerCase();
        });
      } else if (trigger[0]?.triggerKey[0] === "4") {
        const getCustomerdata = await insertCustomerContact(req.body);
        matches = getCustomerdata[0]?.errcode === 1 ? false : true;
      } else if (trigger[0]?.triggerKey[0] === "5") {
        const getCustomerdata = await insertCustomerContact(req.body);
        matches = getCustomerdata[0]?.errcode === 1 ? true : false;
      } if (matches || trigger?.length === 0) {
        const startingNode = await getStartNode(nodes);
        const currentNodeId = startingNode?.data?.children[0]?.childId ?? '';
        await updateNextNodeInsta(instagramCcaasTable[0], currentNodeId);
        return handleChatFlowType(currentNodeId, instagramCcaasTable, nodes, req?.body?.text?.body, req);
      } else if (getNlpData?.data?.intent === 'greetings') {
        await sendMessageCustomerInstagram(instagramCcaasTable[0], getNlpData?.data?.message);
        sendMessageCustomerCaiMessageInsta(instagramCcaasTable, getNlpData?.data?.message);
      }
      if (isArry?.length === 0 && getNlpData?.data?.flow_key === 'query' && currentNode_ref?.data?.children.length !== 0) {
        const getBotMessage: any = await getBotMessageResInsta(instagramCcaasTable[0]);
        const getRemainder = await getRemaindeResponse(instagramCcaasTable[0]);
        const remainderMessage = getRemainder[0]?.message;
        const jsonData = remainderMessage ? JSON.parse(remainderMessage)?.length > 3 : false;
        if (jsonData && getRemainder[0]?.isReminder === 0) {
          await sendFlowBreakInpuRemaindertInsta(instagramCcaasTable[0]);
          await insertBotMessageInsta(instagramCcaasTable[0], getNlpData?.data?.message);
          await insertBotMessageIsRemainderInsta(instagramCcaasTable[0]);
          sendFlowBreakMessageRemainderInsta(instagramCcaasTable);
        } else {
          if (getBotMessage?.length > 0) {
            await sendMessageCustomerInstagram(instagramCcaasTable[0], getNlpData?.data);
            if (getNode[0]?.nextFlowNode !== null) {
              await insertBotMessageRemainderInsta(instagramCcaasTable[0], getNlpData?.data?.message);
            }
            sendMessageCustomerCaiMessageInsta(instagramCcaasTable, getNlpData?.data?.message);
          } else {
            if (getNode[0]?.nextFlowNode !== null) {
              await sendFlowBreakInputInsta(instagramCcaasTable[0]);
              await insertBotMessageInsta(instagramCcaasTable[0], getNlpData?.data?.message);
              sendFlowBreakMessageInsta(instagramCcaasTable);
              await insertBotMessageRemainderInsta(instagramCcaasTable[0], getNlpData?.data?.message);
            } else {
              await sendMessageCustomerInstagram(instagramCcaasTable[0], getNlpData?.data?.message);
              sendMessageCustomerCaiMessageInsta(instagramCcaasTable, getNlpData?.data?.message);
            }

          }
        }
        return;
      } else if (isArry?.length > 0) {
        const FlowData: any = await getQuestionsAnsInsta(instagramCcaasTable[0], getInstagramDetails[0], isArry[0]);
        await updateNextNodeInsta(instagramCcaasTable[0], FlowData[0]?.mapFlow);
        return handleChatFlowType(null, instagramCcaasTable, nodes, req?.body?.text?.body, req);
      } else {
        await sendMessageCustomerInstagram(instagramCcaasTable[0], getNlpData?.data?.message);
        sendMessageCustomerCaiMessageInsta(instagramCcaasTable, getNlpData?.data?.message);
      }
    } else {
      const triggerData = nodes[0]?.data?.information?.trigger_data;
      const trigger = typeof triggerData === 'string' ? JSON.parse(triggerData) : [];
      await triggerDataHandlerInsta(trigger, req, nodes, instagramCcaasTable, getNlpData);
      const isJson = (data: any) => {
        try {
          JSON.parse(data);
          return true;
        } catch (e) {
          return false;
        }
      };
      const checkData = getStartNodeCondition(nodes);
      if (checkData) {
        await updateNextNodeInsta(instagramCcaasTable[0], checkData?.id);
        return handleChatFlowType(null, instagramCcaasTable, nodes, req?.body?.text?.body, req);
      }
      const currentNodeData = await getNodeDataById(getNode[0].nextFlowNode, nodes);
      const json = isJson(getNlpData?.data?.intent) ? JSON.parse(getNlpData?.data?.intent) : getNlpData?.data?.intent;
      const isArry = Array.isArray(json) ? json : [];
      if (isArry?.length === 0 && getNlpData?.data?.flow_key === 'query' && currentNodeData?.data?.children.length !== 0) {
        const getBotMessage: any = await getBotMessageResInsta(instagramCcaasTable[0]);
        const getRemainder = await getRemaindeResponse(instagramCcaasTable[0]);
        const remainderMessage = getRemainder[0]?.message;
        const jsonData = remainderMessage ? JSON.parse(remainderMessage)?.length > 3 : false;
        if (jsonData && getRemainder[0]?.isReminder === 0) {
          await sendFlowBreakInpuRemaindertInsta(instagramCcaasTable[0]);
          await insertBotMessageInsta(instagramCcaasTable[0], getNlpData?.data?.message);
          await insertBotMessageRemainderInsta(instagramCcaasTable[0], getNlpData?.data?.message);
          await insertBotMessageIsRemainderInsta(instagramCcaasTable[0]);
          sendFlowBreakMessageRemainderInsta(instagramCcaasTable);
        } else {
          if (getBotMessage?.length > 0) {
            await sendMessageCustomerInstagram(instagramCcaasTable[0], getNlpData?.data);
            if (getNode[0]?.nextFlowNode !== null) {
              await insertBotMessageRemainderInsta(instagramCcaasTable[0], getNlpData?.data?.message);
            }
            sendMessageCustomerCaiMessageInsta(instagramCcaasTable, getNlpData?.data?.message);
          } else {
            if (getNode[0]?.nextFlowNode !== null) {
              await sendFlowBreakInputInsta(instagramCcaasTable[0]);
              await insertBotMessageInsta(instagramCcaasTable[0], getNlpData?.data?.message);
              sendFlowBreakMessageInsta(instagramCcaasTable);
              await insertBotMessageRemainderInsta(instagramCcaasTable[0], getNlpData?.data?.message);
            } else {
              await sendMessageCustomerInstagram(instagramCcaasTable[0], getNlpData?.data?.message);
              sendMessageCustomerCaiMessageInsta(instagramCcaasTable, getNlpData?.data?.message);
            }
          }
        }
        return;
      } else if (isArry?.length > 0) {
        const FlowData: any = await getQuestionsAnsInsta(instagramCcaasTable[0], getInstagramDetails[0], isArry[0]);
        await updateNextNodeInsta(instagramCcaasTable[0], FlowData[0]?.mapFlow);
        return handleChatFlowType(null, instagramCcaasTable, nodes, req?.body?.text?.body, req);
      }
      const currentNode_ref = await getNodeDataById(getNode[0].nextFlowNode, nodes);
      if (getNode[0].nextFlowNode?.split('_')[0] === 'customInput' && currentNode_ref?.data?.information?.inputType === 5) {
        const saveContactPhoneNumber = JSON.parse(currentNode_ref?.data?.information?.data);
        const getNoOfRepeats: any = await getNoOfRepeatsResInsta(instagramCcaasTable[0]);
        if (getNoOfRepeats?.noOfRepeats === 0 || getNoOfRepeats?.noOfRepeats === null) {
          await updateNoOfRepeatsInsta(instagramCcaasTable[0], parseInt(saveContactPhoneNumber?.getInputData?.noOfRepeat) + 1);
        }
        const isValid = nlpdata?.mobile_number?.length > 0;
        if (isValid) {
          if (saveContactPhoneNumber?.getInputData?.answerSave) {
            try {
              const body = {
                phone: req?.body?.text?.body?.replace(/\D/g, ''),
                domainId: instagramCcaasTable?.[0]?.domainId,
                customerInstagramId: instagramCcaasTable?.[0]?.customerInstagramId
              }
              insertUpdateCrmContact(body); // crm insert
            } catch (error) {
              logger.error('insertUpdateCrmContact ::: error:', error);
            }
            await updateCustomerContactInsta(req.body, instagramCcaasTable[0], req?.body?.text?.body);
            await updateEmailIdInstagram(instagramCcaasTable[0], req?.body?.text); // appointment
            const jsonData = {
              key: saveContactPhoneNumber?.getInputData?.contactSave,
              value: req?.body?.text?.body
            };
            const jsonDataList = JSON.stringify(jsonData);
            await updateConnectDataInsta(jsonDataList, instagramCcaasTable[0], getInstagramDetails);
          }
          await updateNoOfRepeatsInsta(instagramCcaasTable[0], 0);
        } else {
          const getNoOfRepeat: any = await getNoOfRepeatsResInsta(instagramCcaasTable[0]);
          if (getNoOfRepeat?.noOfRepeats !== 0) {
            await updateNoOfRepeatsInsta(instagramCcaasTable[0], getNoOfRepeat?.noOfRepeats - 1);
          }
          if (getNoOfRepeat?.noOfRepeats - 1 !== 0) {
            const errorMessage = saveContactPhoneNumber?.getInputData?.errorMessage === '' ? messageValue?.customerInputError : saveContactPhoneNumber?.getInputData?.errorMessage;
            sendFlowMessageDataInsta(instagramCcaasTable, errorMessage);
            await sendMessageCustomerInstagram(instagramCcaasTable[0], errorMessage);
            return;
          } else {
            const nextData = currentNode_ref?.data?.children[1];
            await updateNextNodeInsta(instagramCcaasTable[0], nextData?.childId);
            return handleChatFlowType(null, instagramCcaasTable, nodes, req?.body?.text?.body, req);
          }
        }
        const nextData = currentNode_ref?.data?.children[0];
        await updateNextNodeInsta(instagramCcaasTable[0], nextData?.childId);
        return handleChatFlowType(null, instagramCcaasTable, nodes, req?.body?.text?.body, req);
      } else if (getNode[0].nextFlowNode?.split('_')[0] === 'customInput' && currentNode_ref?.data?.information?.inputType === 6) {
        const saveContactEmail = JSON.parse(currentNode_ref?.data?.information?.data);
        const getNoOfRepeats: any = await getNoOfRepeatsResInsta(instagramCcaasTable[0]);
        if (getNoOfRepeats?.noOfRepeats === 0 || getNoOfRepeats?.noOfRepeats === null) {
          await updateNoOfRepeatsInsta(instagramCcaasTable[0], parseInt(saveContactEmail?.getInputData?.noOfRepeat) + 1);
        }
        const dynamicPattern = req?.body?.text?.body;
        const checkEmail = nlpdata?.email?.length > 0;
        if (checkEmail) {
          if (saveContactEmail?.getInputData?.answerSave) {
            await insertCustomerContactEmailInsta(req.body, instagramCcaasTable[0], dynamicPattern);
            await updateEmailIdInsta(instagramCcaasTable[0], req?.body?.text); // appointment
            const jsonData = {
              key: saveContactEmail?.getInputData?.contactSave,
              value: req?.body?.text?.body
            };
            const jsonDataList = JSON.stringify(jsonData);
            await updateConnectDataInsta(jsonDataList, instagramCcaasTable[0], getInstagramDetails);

            try {
              const body = {
                email: req?.body?.text?.body,
                domainId: instagramCcaasTable?.[0]?.domainId,
                customerInstagramId: instagramCcaasTable?.[0]?.customerInstagramId
              }
              insertUpdateCrmContact(body); // crm insert
            } catch (error) {
              logger.error('insertUpdateCrmContact ::: error:', error);
            }

          }
          await updateNoOfRepeatsInsta(instagramCcaasTable[0], 0);
        } else {
          const getNoOfRepeat: any = await getNoOfRepeatsResInsta(instagramCcaasTable[0]);
          if (getNoOfRepeat?.noOfRepeats !== 0) {
            await updateNoOfRepeatsInsta(instagramCcaasTable[0], getNoOfRepeat?.noOfRepeats - 1);
          }
          if (getNoOfRepeat?.noOfRepeats - 1 !== 0) {
            const errorMessage = saveContactEmail?.getInputData?.errorMessage === '' ? messageValue?.customerInputError : saveContactEmail?.getInputData?.errorMessage;
            sendFlowMessageDataInsta(instagramCcaasTable, errorMessage);
            await sendMessageCustomerInstagram(instagramCcaasTable[0], errorMessage);
            return;
          } else {
            const nextData = currentNode_ref?.data?.children[1];
            await updateNextNodeInsta(instagramCcaasTable[0], nextData?.childId);
            return handleChatFlowType(null, instagramCcaasTable, nodes, req?.body?.text?.body, req);
          }
        }
        const nextData = currentNode_ref?.data?.children[0];
        await updateNextNodeInsta(instagramCcaasTable[0], nextData?.childId);
        return handleChatFlowType(null, instagramCcaasTable, nodes, req?.body?.text?.body, req);
      } else if (getNode[0].nextFlowNode?.split('_')[0] === 'customInput' && currentNode_ref?.data?.information?.inputType === 2) {
        const saveText = JSON.parse(currentNode_ref?.data?.information?.data);
        if (saveText?.getInputData?.answerSave) {
          const jsonData = {
            key: saveText?.getInputData?.contactSave,
            value: req?.body?.text?.body
          };
          const jsonDataList = JSON.stringify(jsonData);
          await updateConnectDataInsta(jsonDataList, instagramCcaasTable[0], getInstagramDetails);
        }
        const nextData = currentNode_ref?.data?.children[0];
        await updateNextNodeInsta(instagramCcaasTable[0], nextData?.childId);
        return handleChatFlowType(null, instagramCcaasTable, nodes, req?.body?.text?.body, req);
      } else if (getNode[0].nextFlowNode?.split('_')[0] === 'customInput' && currentNode_ref?.data?.information?.inputType === 7) {
        const saveNumber = JSON.parse(currentNode_ref?.data?.information?.data);
        const getNoOfRepeats: any = await getNoOfRepeatsResInsta(instagramCcaasTable[0]);
        if (getNoOfRepeats?.noOfRepeats === 0 || getNoOfRepeats?.noOfRepeats === null) {
          await updateNoOfRepeatsInsta(instagramCcaasTable[0], parseInt(saveNumber?.getInputData?.noOfRepeat) + 1);
        }
        const isNumber = nlpdata?.count?.length > 0;
        if (isNumber) {
          if (saveNumber?.getInputData?.answerSave) {
            const jsonData = {
              key: saveNumber?.getInputData?.contactSave,
              value: req?.body?.text?.body
            };
            const jsonDataList = JSON.stringify(jsonData);
            await updateConnectDataInsta(jsonDataList, instagramCcaasTable[0], getInstagramDetails);
          }
          await updateNoOfRepeatsInsta(instagramCcaasTable[0], 0);
        } else {
          const getNoOfRepeat: any = await getNoOfRepeatsResInsta(instagramCcaasTable[0]);
          if (getNoOfRepeat?.noOfRepeats !== 0) {
            await updateNoOfRepeatsInsta(instagramCcaasTable[0], getNoOfRepeat?.noOfRepeats - 1);
          }
          if (getNoOfRepeat?.noOfRepeats - 1 !== 0) {
            const errorMessage = saveNumber?.getInputData?.errorMessage === '' ? messageValue?.customerInputError : saveNumber?.getInputData?.errorMessage;
            sendFlowMessageDataInsta(instagramCcaasTable, errorMessage);
            await sendMessageCustomerInstagram(instagramCcaasTable[0], errorMessage);
            return;
          } else {
            const nextData = currentNode_ref?.data?.children[1];
            await updateNextNodeInsta(instagramCcaasTable[0], nextData?.childId);
            return handleChatFlowType(null, instagramCcaasTable, nodes, req?.body?.text?.body, req);
          }
        }
        const nextData = currentNode_ref?.data?.children[0];
        await updateNextNodeInsta(instagramCcaasTable[0], nextData?.childId);
        return handleChatFlowType(null, instagramCcaasTable, nodes, req?.body?.text?.body, req);
      } else if (getNode[0].nextFlowNode?.split('_')[0] === 'customInput' && currentNode_ref?.data?.information?.inputType === 8) {
        const saveUrl = JSON.parse(currentNode_ref?.data?.information?.data);
        const getNoOfRepeats: any = await getNoOfRepeatsResInsta(instagramCcaasTable[0]);
        if (getNoOfRepeats?.noOfRepeats === 0 || getNoOfRepeats?.noOfRepeats === null) {
          await updateNoOfRepeatsInsta(instagramCcaasTable[0], parseInt(saveUrl?.getInputData?.noOfRepeat) + 1);
        }
        const isUrl = nlpdata?.website?.length > 0;
        if (isUrl) {
          if (saveUrl?.getInputData?.answerSave) {
            const jsonData = {
              key: saveUrl?.getInputData?.contactSave,
              value: req?.body?.text?.body
            };
            const jsonDataList = JSON.stringify(jsonData);
            await updateConnectDataInsta(jsonDataList, instagramCcaasTable[0], getInstagramDetails);
          }
          await updateNoOfRepeatsInsta(instagramCcaasTable[0], 0);
        } else {
          const getNoOfRepeat: any = await getNoOfRepeatsResInsta(instagramCcaasTable[0]);
          if (getNoOfRepeat?.noOfRepeats !== 0) {
            await updateNoOfRepeatsInsta(instagramCcaasTable[0], getNoOfRepeat?.noOfRepeats - 1);
          }
          if (getNoOfRepeat?.noOfRepeats - 1 !== 0) {
            const errorMessage = saveUrl?.getInputData?.errorMessage === '' ? messageValue?.customerInputUrl : saveUrl?.getInputData?.errorMessage;
            sendFlowMessageDataInsta(instagramCcaasTable, errorMessage);
            await sendMessageCustomerInstagram(instagramCcaasTable[0], errorMessage);
          } else {
            const nextData = currentNode_ref?.data?.children[1];
            await updateNextNodeInsta(instagramCcaasTable[0], nextData?.childId);
            return handleChatFlowType(null, instagramCcaasTable, nodes, req?.body?.text?.body, req);
          }
        }
        const nextData = currentNode_ref?.data?.children[0];
        await updateNextNodeInsta(instagramCcaasTable[0], nextData?.childId);
        return handleChatFlowType(null, instagramCcaasTable, nodes, req?.body?.text?.body, req);
      } else if (getNode[0].nextFlowNode?.split('_')[0] === 'customInput' && currentNode_ref?.data?.information?.inputType === 9) {
        const saveContactName = JSON.parse(currentNode_ref?.data?.information?.data);
        const getNoOfRepeats: any = await getNoOfRepeatsResInsta(instagramCcaasTable[0]);
        if (getNoOfRepeats?.noOfRepeats === 0 || getNoOfRepeats?.noOfRepeats === null) {
          await updateNoOfRepeatsInsta(instagramCcaasTable[0], parseInt(saveContactName?.getInputData?.noOfRepeat) + 1);
        }
        const isName = (nlpdata?.first_name?.length > 0) || (nlpdata?.last_name?.length > 0);
        if (isName) {
          if (saveContactName?.getInputData?.answerSave) {
            if (saveContactName?.getInputData?.contactSave === 'firstName') {
              await updateCustomerFirstName(req.body, instagramCcaasTable[0]);
              await updateCustomerFirstNameAppoinmentInsta(instagramCcaasTable[0], req?.body?.text);
              try {
                const body = {
                  firstName: req?.body?.text?.body,
                  domainId: instagramCcaasTable?.[0]?.domainId,
                  customerInstagramId: instagramCcaasTable?.[0]?.customerInstagramId
                }
                insertUpdateCrmContact(body); // crm insert
              } catch (error) {
                logger.error('insertUpdateCrmContact ::: error:', error);
              }
            } else if (saveContactName?.getInputData?.contactSave === 'lastName') {
              await updateCustomerLastName(req.body, instagramCcaasTable[0]);
              await updateCustomerLastNameAppoinmentInsta(instagramCcaasTable[0], req?.body?.text);
              try {
                const body = {
                  lastName: req?.body?.text?.body,
                  domainId: instagramCcaasTable?.[0]?.domainId,
                  customerInstagramId: instagramCcaasTable?.[0]?.customerInstagramId
                }
                insertUpdateCrmContact(body); // crm insert
              } catch (error) {
                logger.error('insertUpdateCrmContact ::: error:', error);
              }
            }
            const jsonData = {
              key: saveContactName?.getInputData?.contactSave,
              value: req?.body?.text?.body
            };
            const jsonDataList = JSON.stringify(jsonData);
            await updateConnectDataInsta(jsonDataList, instagramCcaasTable[0], getInstagramDetails);
          }
          await updateNoOfRepeatsInsta(instagramCcaasTable[0], 0);
        } else {
          const getNoOfRepeat: any = await getNoOfRepeatsResInsta(instagramCcaasTable[0]);
          if (getNoOfRepeat?.noOfRepeats !== 0) {
            await updateNoOfRepeatsInsta(instagramCcaasTable[0], getNoOfRepeat?.noOfRepeats - 1);
          }
          if (getNoOfRepeat?.noOfRepeats - 1 !== 0) {
            const errorMessage = saveContactName?.getInputData?.errorMessage === '' ? messageValue?.customerInputError : saveContactName?.getInputData?.errorMessage;
            sendFlowMessageDataInsta(instagramCcaasTable, errorMessage);
            await sendMessageCustomerInstagram(instagramCcaasTable[0], errorMessage);
            return;
          } else {
            const nextData = currentNode_ref?.data?.children[1];
            await updateNextNodeInsta(instagramCcaasTable[0], nextData?.childId);
            return handleChatFlowType(null, instagramCcaasTable, nodes, req?.body?.text?.body, req);
          }
        }
        const nextData = currentNode_ref?.data?.children[0];
        await updateNextNodeInsta(instagramCcaasTable[0], nextData?.childId);
        return handleChatFlowType(null, instagramCcaasTable, nodes, req?.body?.text?.body, req);
      } else if (getNode[0].nextFlowNode.split('_')[0] === 'customInput' && currentNode_ref?.data?.information?.inputType === 14) {
        const nextData = currentNode_ref?.data?.children;
        const selectedTitle = req?.body?.text?.title;
        const match = nextData.find((option: any) => option?.type === selectedTitle);
        const childId = match?.childId;
        await updateNextNodeInsta(instagramCcaasTable[0], childId);
        return handleChatFlowType(null, instagramCcaasTable, nodes, req?.body?.text?.body, req);
      } else {
        const triggerData = nodes[0]?.data?.information?.trigger_data;
        const trigger = typeof triggerData === 'string' ? JSON.parse(triggerData) : [];
        await triggerDataSendMessageHandlerInsta(trigger, req, nodes, instagramCcaasTable, getNlpData);
      }
    }
  } catch (err) {
    console.log(err);
    // res.send({ statusCode: 500, message: 'internal server error' });
  }
}

async function handleChatFlowType(nodeId: any, instagramCcaasTable: any, nodes: any, input: any, req: any) {
  try {
    const getNode = await getNextNodeInsta(instagramCcaasTable[0]);
    let currentNode = null;
    let custom_input_update = false;
    if (getNode[0].nextFlowNode === null) {
      currentNode = await getNodeDataById(nodeId, nodes);
      if (nodeId !== null && nodeId?.split('_')[0] === 'customInput') {
        custom_input_update = true;
      }
    } else {
      currentNode = await getNodeDataById(getNode[0].nextFlowNode, nodes);
    }
    const entryCarousal: any = [];
    if (currentNode?.data?.children !== undefined) {
      for (const data of currentNode?.data?.children) {
        const child = data?.childId.split('_')[0];
        if (child === 'carousel') {
          const currentNode_ref = await getNodeDataById(data?.childId, nodes);
          if (Object.keys(currentNode_ref?.data?.information).length > 0) {
            const carousel_data = JSON.parse(currentNode_ref?.data?.information?.carousel_data) ?? null;
            entryCarousal.push({
              title: carousel_data?.title, image_url: carousel_data?.file_url,
              subtitle: carousel_data?.description, default_action: {
                type: 'web_url',
                url: carousel_data?.file_url,
                webview_height_ratio: 'tall'
              },
              buttons: carousel_data?.buttons.map((row: any) => {
                if (row?.link !== '') {
                  return {
                    type: 'web_url',
                    url: row?.link,
                    title: row?.value,
                    // "payload": data?.childId
                  };
                }
                return {
                  type: 'postback',
                  title: row?.value,
                  payload: data?.childId
                };

              })
            });
          }
        }
      }
    }
    switch (currentNode?.type) {
      case 'entryPoint': {
        if (currentNode && currentNode.data && currentNode.data.children && currentNode.data.children.at(0)) {
          const childId = currentNode.data.children.at(0).childId;
          await updateNextNodeInsta(instagramCcaasTable[0], childId);
          return handleChatFlowType(childId, instagramCcaasTable, nodes, input, req);
        }
        break;
      }
      case 'messageFlow': {
        try {
          logger.info('messageFlow>>>>>>>>>>>>>>>>>>>>>>>', currentNode);
          const text = currentNode?.data?.information?.message;
          const targetData = JSON.parse(text);
          let temp = targetData[0]?.data;
          if (temp?.includes("<ul>") && temp?.includes("</ul>")) {
            temp = temp.replace(/<ul>/g, "\n").replace(/<\/ul>/g, "");
            temp = temp.replace(/<li>/g, "• ").replace(/<\/li>/g, "\n");
          } else {
            let counter = 1;
            temp = temp.replace(/<p>/g, "\n");
            temp = temp.replace(/<strong>/g, "").replace(/<\/strong>/g, "");
            temp = temp.replace(/<em>/g, "_").replace(/<\/em>/g, "_");
            temp = temp.replace(/<s>/g, "~").replace(/<\/s>/g, "~");
            temp = temp.replace(/<ol>/g, "\n").replace(/<\/ol>/g, "");
            temp = temp.replace(/<li>/g, () => `${counter++}. `).replace(/<\/li>/g, "\n");
          }
          const textData = temp?.replace(/<[^>]+>/g, '');
          const filesData: any = targetData[1];
          targetData.filter((row: any) => row?.type === 'text');
          await sendMessageCustomerInstagramFlow(instagramCcaasTable[0], textData);
          const mimeType = targetData[1]?.mimetype?.split('/')[0];
          const image = mimeType ? `${mimeType[0]?.toUpperCase()}mage` : '';
          const audio = mimeType ? `${mimeType[0]?.toUpperCase()}udio` : '';
          const video = mimeType ? `${mimeType[0]?.toUpperCase()}ideo` : '';
          if (targetData.length > 1 && targetData[1]?.type === 'image') {
            await sendImageCustomerInsta(instagramCcaasTable[0], targetData[1], image);
          } else if (targetData.length > 1 && targetData[1]?.type === 'file' && targetData[1]?.mimetype?.split('/')[0] === 'audio') {
            await sendImageCustomerInsta(instagramCcaasTable[0], targetData[1], audio);
          } else if (targetData.length > 1 && targetData[1]?.type === 'file' && targetData[1]?.mimetype?.split('/')[0] === 'video') {
            await sendImageCustomerInsta(instagramCcaasTable[0], targetData[1], video);
          }
          const nextNode = currentNode?.data?.children?.find((nodes1: any) => nodes1?.type === RESPONSE?.success_message) ?? { childId: '' };
          const nextNodedetails: any = await getNodeDataById(nextNode?.childId, nodes);
          let message: any = {};
          try {
            message = {
              messageID: uuidv4(),
              sessionId: instagramCcaasTable[0]?.sessionId,
              message: textData,
              sendAt: Date.now(),
              messageType: targetData?.length === 1 ? 'text' : 'files',
              sendMediaUrl: filesData?.url ?? '',
              isBot: true,
              isCustomer: false,
              fileType: '',
              direction: 'OUT',
              isLike: 0
            };
            const messageList: any = { message };
            const messageType: any = { agentText: messageList };
            const stringifiedMessage: any = { message: messageType };
            stringifiedMessage.InstagramHistoryId = instagramCcaasTable[0]?.InstagramHistoryId;
            await updateChatMessageInsta(stringifiedMessage);
            await updateNextNodeInsta(instagramCcaasTable[0], nextNode?.childId);
            updateIsBotInsta(instagramCcaasTable[0]?.InstagramHistoryId);
            if (entryCarousal?.length !== 0) {
              ///   await sendCarouselData(instagramCcaasTable[0], entryCarousal);
            }
          } catch (error) {
            const nextNodes = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
            // return handleChatFlowType(nextNode?.childId ?? null, whatsappCcaasTable[0] ?? null, node, io);
          }
          return handleChatFlowType(nextNode, instagramCcaasTable, nodes, input, req);
        } catch (error) {
          const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
          return handleChatFlowType(nextNode, instagramCcaasTable, nodes, input, req);
        }
        break;
      }
      case 'customInput': {
        try {
          logger.info('customInput>>>>>>>>>>>>>>>>>>>>>>>', currentNode);
          if (currentNode?.data?.information?.inputType === 1 || currentNode?.data?.information?.inputType === 15) {
            const btninputCount = currentNode?.data?.information?.data;
            const parseData = await correctjsondata(btninputCount);
            if (parseData?.menuType === 'quick_reply') {
              await sendCustomerInputInsta(instagramCcaasTable[0], parseData);
            } else if (parseData?.menuType === 'button') {
              const nextNode = currentNode?.data?.children?.find((nodes1: any) => nodes1?.childId) ?? { childId: '' };
              await sendButtonLinkInsta(instagramCcaasTable[0], parseData);
              try {
                await updateMessageFlowInsta(instagramCcaasTable, parseData);
                await updateNextNodeInsta(instagramCcaasTable[0], nextNode?.childId);
                if (entryCarousal?.length !== 0) {
                  await sendCarouselDataInsta(instagramCcaasTable[0], entryCarousal);
                }
                return handleChatFlowType(nextNode, instagramCcaasTable, nodes, input, req);
              } catch (error) {
                logger.error('customInput', error);
              }
            }
          } else if (currentNode?.data?.information?.inputType === 5) {
            const phoneNumber = currentNode.data?.information?.message;
            await sendMessageCustomerInstagram(instagramCcaasTable[0], phoneNumber);
            try {
              await updateMessageFlowDataInsta(instagramCcaasTable, phoneNumber);
              if (entryCarousal?.length !== 0) {
                await sendCarouselDataInsta(instagramCcaasTable[0], entryCarousal);
              }
            } catch (error) {
              logger.error('customInput', error);
            }
          } else if (currentNode?.data?.information?.inputType === 6) {
            const emailId = currentNode.data?.information?.message;
            await sendMessageCustomerInstagram(instagramCcaasTable[0], emailId);
            try {
              await updateMessageFlowDataInsta(instagramCcaasTable, emailId);
              if (entryCarousal?.length !== 0) {
                await sendCarouselDataInsta(instagramCcaasTable[0], entryCarousal);
              }
            } catch (error) {
              logger.error('customInput', error);
            }
          } else if (currentNode?.data?.information?.inputType === 2) {
            const textMessage = currentNode.data?.information?.message;
            await sendMessageCustomerInstagram(instagramCcaasTable[0], textMessage);
            try {
              await updateMessageFlowDataInsta(instagramCcaasTable, textMessage);
              if (entryCarousal?.length !== 0) {
                await sendCarouselDataInsta(instagramCcaasTable[0], entryCarousal);
              }
            } catch (error) {
              logger.error('customInput', error);
            }
          } else if (currentNode?.data?.information?.inputType === 7) {
            const number = currentNode.data?.information?.message;
            await sendMessageCustomerInstagram(instagramCcaasTable[0], number);
            try {
              await updateMessageFlowDataInsta(instagramCcaasTable, number);
              if (entryCarousal?.length !== 0) {
                await sendCarouselDataInsta(instagramCcaasTable[0], entryCarousal);
              }
            } catch (error) {
              logger.error('customInput', error);
            }
          } else if (currentNode?.data?.information?.inputType === 8) {
            const url = currentNode.data?.information?.message;
            await sendMessageCustomerInstagram(instagramCcaasTable[0], url);
            try {
              await updateMessageFlowDataInsta(instagramCcaasTable, url);
              if (entryCarousal?.length !== 0) {
                await sendCarouselDataInsta(instagramCcaasTable[0], entryCarousal);
              }
            } catch (error) {
              logger.error('customInput', error);
            }
          } else if (currentNode?.data?.information?.inputType === 9) {
            const name = currentNode.data?.information?.message;
            await sendMessageCustomerInstagram(instagramCcaasTable[0], name);
            try {
              await updateMessageFlowDataInsta(instagramCcaasTable, name);
              if (entryCarousal?.length !== 0) {
                await sendCarouselDataInsta(instagramCcaasTable[0], entryCarousal);
              }
            } catch (error) {
              logger.error('customInput', error);
            }
          } else if (currentNode?.data?.information?.inputType === 16) {
            const FileMessage = currentNode.data?.information?.message;
            await sendMessageCustomerInstagram(instagramCcaasTable[0], FileMessage);
            try {
              await updateMessageFlowDataInsta(instagramCcaasTable, FileMessage);
              if (entryCarousal?.length !== 0) {
                await sendCarouselDataInsta(instagramCcaasTable[0], entryCarousal);
              }
            } catch (error) {
              logger.error('customInput', error);
            }
          } else if (currentNode?.data?.information?.inputType === 14) {
            const dropDownMessage = currentNode.data?.information?.message;
            const dropDownData = JSON.parse(currentNode.data?.information?.data);
            const dropDownMultipleData = dropDownData?.options?.map((item: any) => ({ payload: item?.value, content_type: 'text', title: item?.value }));
            await sendCustomerSatificationInstaDropdown(instagramCcaasTable[0], dropDownMessage, dropDownMultipleData);
          }
          if (custom_input_update) {
            await updateNextNodeInsta(instagramCcaasTable[0], nodeId);
          }
        } catch (error) {
          const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
          logger.error('customInput', error);
          return handleChatFlowType(nextNode, instagramCcaasTable, nodes, input, req);
        }
        break;
      }
      case 'sendMail': {
        try {
          logger.info('sendMail>>>>>>>>>>>>>>>>>>>>>>>', currentNode);
          const templateData = JSON.parse(currentNode?.data?.information?.template_data);
          const getEmailData = await getCustomerEmailIdInsta(instagramCcaasTable[0]);
          if (templateData?.type === '1') {
            await sendEmail(getEmailData, templateData);
          } else {
            const adminEmail: any = await getAdminEmail(instagramCcaasTable[0])
            await sendEmailPlainText(getEmailData, templateData, instagramCcaasTable[0], adminEmail);
          }
          const nextNode = currentNode?.data?.children?.find((nodes1: any) => nodes1?.type === 'success') ?? { childId: '' };
          const nextNodedetails: any = await getNodeDataById(nextNode?.childId, nodes);
          let message: any = {};
          try {
            message = {
              messageID: uuidv4(),
              sessionId: instagramCcaasTable[0]?.sessionId,
              message: templateData?.data?.template_image,
              sendAt: Date.now(),
              messageType: 'text',
              sendMediaUrl: templateData?.data?.template_image ?? '',
              isBot: true,
              isCustomer: false,
              fileType: '',
              direction: 'OUT',
              isLike: 0
            };
            const messageList: any = { message };
            const messageType: any = { agentText: messageList };
            const stringifiedMessage: any = { message: messageType };
            stringifiedMessage.InstagramHistoryId = instagramCcaasTable[0]?.InstagramHistoryId;
            await updateChatMessageInsta(stringifiedMessage);
            updateIsBotInsta(instagramCcaasTable[0]?.InstagramHistoryId);
            await updateNextNodeInsta(instagramCcaasTable[0], nextNode?.childId);
            if (entryCarousal?.length !== 0) {
              await sendCarouselDataInsta(instagramCcaasTable[0], entryCarousal);
            }
            return handleChatFlowType(nextNode, instagramCcaasTable, nodes, input, req);
          } catch (error) {
            const nextNodes = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
            // return handleChatFlowType(nextNode?.childId ?? null, whatsappCcaasTable[0] ?? null, node, io);
          }
          return handleChatFlowType(nextNode, instagramCcaasTable, nodes, input, req);
        } catch (error) {
          logger.error('sendMail', error);
          const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
          return handleChatFlowType(nextNode, instagramCcaasTable, nodes, input, req);
        }
        break;
      }
      case 'appointment': {
        try {
          logger.info('appointment>>>>>>>>>>>>>>>>>>>>>>>', currentNode);
          const getAppointment = JSON.parse(currentNode?.data?.information?.appointment_data);
          const getUrl = getAppointment?.eventUrl;
          const urlObj = new URL(getUrl);
          const previewId: any = urlObj.searchParams.get('id');
          const sanitizedPreviewId = String(previewId).replace(/ /g, "+");
          const decrypted = CryptoJS.AES.decrypt(sanitizedPreviewId, process.env.REACT_APP_ENCDEC_KEY).toString(CryptoJS.enc.Utf8);
          let value = decrypted?.split("-");
          let appointmentId = value[0];
          let companyId = value[1];
          let userId = value[2];
          const ServiceName = await getAppointmentInfo(appointmentId, companyId);
          const getAppointmentType = await AppointmentInfo({ appId: appointmentId, companyId: companyId });
          const getLeaveDates = await getLeaveData({ companyId: companyId, userId: userId });
          const leaveDatas = getLeaveDates.filter((_l: any) => _l.is_leave === 1);
          if (getAppointmentType[0]?.app_typeid === 1) {
            await updateAppointmentUserDetailsInsta(instagramCcaasTable[0], appointmentId, companyId, userId, getAppointmentType[0]);
            const dataRange = JSON.parse(getAppointmentType[0]?.data_ranges);
            // const fromDate = dataRange[0]?.fromDate;
            const fromDate = new Date().toISOString().split('T')[0];
            const toDate = dataRange[0]?.toDate;
            const result = getDateRange(fromDate, toDate);
            const filteredDates = result.filter((item: any) => !leaveDatas.some((leave: any) => leave?.leaveDate === item));
            let filteredData: any = [];
            let dateObjects: any = getNext10Days(filteredDates);
            const Records = filteredDates?.length > 10;
            if (Records) {
              const lastDate = dateObjects[dateObjects.length - 1];
              const lastDateObj = new Date(lastDate?.title);
              const nextDate = new Date(lastDateObj);
              nextDate.setDate(lastDateObj.getDate() + 1);
              const formattedNextDate = nextDate.toISOString().split('T')[0];
              dateObjects.push({ content_type: "text", title: 'More', payload: `10 ${formattedNextDate}` })
            } else {
              filteredData = dateObjects.filter((item: any) => item.title !== undefined);
            }
            logger.info("Select date", Records ? dateObjects : filteredData);
            const selectDate = Records ? dateObjects : filteredData;
            await sendAppointmentDateInsta(instagramCcaasTable[0], selectDate);
            await insertFlowTrackingInsta(instagramCcaasTable[0], "date", { appId: appointmentId, companyId: companyId, userId: userId });
          } else if (getAppointmentType[0]?.app_typeid === 2) {
            await updateAppointmentUserDetailsInsta(instagramCcaasTable[0], appointmentId, companyId, userId, getAppointmentType[0]);
            const dataRange = JSON.parse(getAppointmentType[0]?.data_ranges);
            // const fromDate = dataRange[0]?.fromDate;
            const fromDate = new Date().toISOString().split('T')[0];
            const toDate = dataRange[0]?.toDate;
            const result = getDateRange(fromDate, toDate);
            const filteredDates = result.filter((item: any) => !leaveDatas.some((leave: any) => leave?.leaveDate === item));
            let filteredData: any = [];
            let dateObjects: any = getNext10Days(filteredDates);
            const Records = filteredDates?.length > 10;
            if (Records) {
              const lastDate = dateObjects[dateObjects.length - 1];
              const lastDateObj = new Date(lastDate?.title);
              const nextDate = new Date(lastDateObj);
              nextDate.setDate(lastDateObj.getDate() + 1);
              const formattedNextDate = nextDate.toISOString().split('T')[0];
              dateObjects.push({ content_type: "text", title: 'More', payload: `10 ${formattedNextDate}` })
            } else {
              filteredData = dateObjects.filter((item: any) => item.title !== undefined);
            }
            logger.info("Select date", Records ? dateObjects : filteredData);
            const selectDate = Records ? dateObjects : filteredData;
            await sendAppointmentDateInsta(instagramCcaasTable[0], selectDate);
            await insertFlowTrackingInsta(instagramCcaasTable[0], "date", { appId: appointmentId, companyId: companyId, userId: userId });
          } else if (getAppointmentType[0]?.app_typeid === 3) {
            await updateAppointmentUserCreateByInsta(instagramCcaasTable[0], appointmentId, companyId, userId, getAppointmentType[0]);
            const dataRange = JSON.parse(getAppointmentType[0]?.data_ranges);
            // const fromDate = dataRange[0]?.fromDate;
            const fromDate = new Date().toISOString().split('T')[0];
            const toDate = dataRange[0]?.toDate;
            const result = getDateRange(fromDate, toDate);
            const filteredDates = result.filter((item: any) => !leaveDatas.some((leave: any) => leave?.leaveDate === item));
            let filteredData: any = [];
            let dateObjects: any = getNext10Days(filteredDates);
            const Records = filteredDates?.length > 10;
            if (Records) {
              const lastDate = dateObjects[dateObjects.length - 1];
              const lastDateObj = new Date(lastDate?.title);
              const nextDate = new Date(lastDateObj);
              nextDate.setDate(lastDateObj.getDate() + 1);
              const formattedNextDate = nextDate.toISOString().split('T')[0];
              dateObjects.push({ content_type: "text", title: 'More', payload: `10 ${formattedNextDate}` })
            } else {
              filteredData = dateObjects.filter((item: any) => item.title !== undefined);
            }
            logger.info("Select date", Records ? dateObjects : filteredData);
            const selectDate = Records ? dateObjects : filteredData;
            await sendAppointmentDateInsta(instagramCcaasTable[0], selectDate);
            await insertFlowTrackingInsta(instagramCcaasTable[0], "date", { appId: appointmentId, companyId: companyId, userId: userId });
          } else {
            const categoryName = ServiceName.map((category: any) => ({ content_type: "text", title: category?.categoryName, payload: category?.categoryId }));
            const uniqueCategories = categoryName.filter((item: any, index: any, self: any) => index === self.findIndex((t: any) => t.id === item.id && t.title === item.title));
            logger.info("select category", uniqueCategories);
            await sendCategoryNameInsta(instagramCcaasTable[0], uniqueCategories);
            let datas: any = {
              appointmentId,
              companyId,
              userId
            }
            await insertFlowTrackingInsta(instagramCcaasTable[0], "categories", datas);
          }
        } catch (error) {
          logger.error('appointment', error);
          const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
          return handleChatFlowType(nextNode, instagramCcaasTable, nodes, input, req);
        }
        break;
      }
      case 'integration': {
        try {
          logger.info('integration>>>>>>>>>>>>>>>>>>>>>>>>', currentNode);
          const type = currentNode?.data?.information?.method;
          if (type === 'api_block') {
            try {
              const rawEndURL = currentNode?.data?.information?.endPointUrl.lastIndexOf('$');
              const endPointUrl = type === 'get' ? currentNode?.data?.information?.endPointUrl.substring(0, rawEndURL) : currentNode?.data?.information?.endPointUrl;
              const url = type === 'get' ? `${endPointUrl}${input}` : `${endPointUrl}`;
              const method = currentNode?.data?.information?.getMethodApi ?? 'get';
              const selectedKeys: any = new Set(currentNode?.data?.information?.selectedKeyFromResponse ?? []);
              const body = currentNode?.data?.information?.apiBody;
              const headers = currentNode?.data?.information?.apiHeaders;
              const Params = currentNode?.data?.information?.apiParams;
              const apiResponse = await fetchApi({ urlEndPoint: url, urlMethod: method, urlBody: body, urlHeaders: headers, inputParams: Params });
              const targetRestructured: any = { sessionId: instagramCcaasTable[0]?.sessionId };
              const nameAPI = currentNode?.data?.information?.nameAPI;
              let context: any = {};
              if (apiResponse?.results) {
                if (apiResponse?.results?.status === 200 || (nameAPI === 'Sim order' && apiResponse?.results?.data?.statusCode !== 500)) {
                  let finalResponse: any = await getResponseData(apiResponse?.results);
                  const nodeNext = currentNode?.data?.children?.find((nodes1: any) => nodes1?.type === RESPONSE?.success_message) ?? { childId: '' };
                  if (Array.isArray(finalResponse) && finalResponse.length > 0) {
                    finalResponse = finalResponse[0] ?? { childId: '' };
                  }
                  const _formattedSelectedKeys = [];
                  for (const [key, value] of Object.entries(finalResponse)) {
                    context[key] = value;
                    if (selectedKeys.has(key)) {
                      /* tslint:disable */
                      _formattedSelectedKeys.push({ tempkey: '${context.' + key + '}', originalkey: key, [key]: value });
                      context = addContextVariable(key, value);
                    }
                  }
                  const finalKeys: any = context;
                  targetRestructured.selectedKeys = finalKeys;
                  if (nodeNext.childId.includes('customInput')) {
                    const nextNodedetails: any = await getNodeDataById(nodeNext.childId, nodes);
                    const _cpynextNodedetails: any = { ...nextNodedetails };
                    const _childrens = _cpynextNodedetails?.data?.children || {};
                    for (let _index = 0; _index < _childrens.length; _index++) {
                      const element = _childrens[_index];
                      for (let _tempi = 0; _tempi < _formattedSelectedKeys.length; _tempi++) {
                        const tempelement = _formattedSelectedKeys[_tempi];
                        if (element.type === tempelement.tempkey) {
                          _childrens[_index].type = tempelement.value;

                        }
                      }
                    }
                    _cpynextNodedetails.data.children = _childrens;
                    targetRestructured.currentNode = _cpynextNodedetails;
                    targetRestructured.result = finalResponse;
                    await sendMessageCustomerInstagram(instagramCcaasTable[0], finalResponse);
                    const message: any = {
                      messageID: uuidv4(),
                      sessionId: instagramCcaasTable[0]?.sessionId,
                      message: finalResponse,
                      sendAt: Date.now(),
                      messageType: 'text',
                      sendMediaUrl: '',
                      isBot: true,
                      isCustomer: false,
                      fileType: '',
                      direction: 'OUT',
                      isLike: 0
                    };
                    const messageType: any = { agentText: message }
                    const stringifiedMessage: any = { message: messageType };
                    stringifiedMessage.InstagramHistoryId = instagramCcaasTable[0]?.InstagramHistoryId;
                    await updateChatMessageInsta(stringifiedMessage);
                    updateIsBotInsta(instagramCcaasTable[0]?.InstagramHistoryId);
                    if (entryCarousal?.length !== 0) {
                      await sendCarouselDataInsta(instagramCcaasTable[0], entryCarousal);
                    }
                  } else if (nodeNext.childId.includes('messageFlow')) {
                    const nextNodedetails: any = await getNodeDataById(nodeNext.childId, nodes);
                    const text = nextNodedetails?.data?.information?.message;
                    const availableContexts = text?.match(contextPattern);
                    const updatedMessage = availableContexts?.length > 0 ? replacechatPatternWithVariables(availableContexts, text, _formattedSelectedKeys) : text;
                    const data = { ...nextNodedetails?.data?.information };
                    data.message = `${updatedMessage}`;
                    const targetRestructured: any = { nextNodedetails, data, sessionId: instagramCcaasTable[0]?.sessionId };
                    targetRestructured.msg = updatedMessage;
                    const messagenextNode = nextNodedetails?.data?.children?.find((nodes: any) => nodes?.type === RESPONSE?.success_message) ?? { childId: '' };
                    let messagenextNodedetails: any = {};
                    if (messagenextNode.childId) {
                      messagenextNodedetails = await getNodeDataById(messagenextNode?.childId, nodes);
                    }
                    targetRestructured.currentNode = nextNodedetails;
                    const message: any = {
                      messageID: uuidv4(),
                      sessionId: instagramCcaasTable[0]?.sessionId,
                      message: targetRestructured?.msg,
                      sendAt: Date.now(),
                      messageType: 'text',
                      sendMediaUrl: '',
                      isBot: true,
                      isCustomer: false,
                      fileType: '',
                      direction: 'OUT',
                      isLike: 0
                    };
                    const messageList: any = { message: message };
                    const messageType: any = { agentText: messageList }
                    const stringifiedMessage: any = { message: messageType };
                    stringifiedMessage.InstagramHistoryId = instagramCcaasTable[0]?.InstagramHistoryId;
                    await updateChatMessageInsta(stringifiedMessage);
                    updateIsBotInsta(instagramCcaasTable[0]?.InstagramHistoryId);
                    if (entryCarousal?.length !== 0) {
                      await sendCarouselDataInsta(instagramCcaasTable[0], entryCarousal);
                    }
                    const body = {
                      domainId: instagramCcaasTable[0]?.domainId,
                      customerId: instagramCcaasTable[0]?.customerInstagramId,
                      pageId: instagramCcaasTable[0]?.agentInstagramId,
                      type: 'text',
                      message: targetRestructured?.msg?.replace(/<[^>]+>/g, '')
                    };
                    const _cpynextNodedetails: any = { ...messagenextNodedetails };
                    targetRestructured.currentNode = _cpynextNodedetails;
                    await axios.post(process?.env?.InstagramOut, body);
                  } else if (nodeNext.childId.includes('integration')) {
                    await updateNextNodeInsta(instagramCcaasTable[0], nodeNext?.childId);
                    if (entryCarousal?.length !== 0) {
                      await sendCarouselDataInsta(instagramCcaasTable[0], entryCarousal);
                    }
                    return handleChatFlowType(nodeNext, instagramCcaasTable, nodes, input, req);
                  }
                } else {
                  const nodeNext = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'error') ?? { childId: '' };
                  if (nodeNext.childId.includes('messageFlow')) {
                    const nextNodedetails: any = await getNodeDataById(nodeNext.childId, nodes);
                    const text = nextNodedetails?.data?.information?.message;
                    const availableContexts = text?.match(contextPattern);
                    const updatedMessage = availableContexts?.length > 0 ? replacePatternWithVariables(availableContexts, text, context) : text;
                    const data = { ...nextNodedetails?.data?.information };
                    data.message = `${updatedMessage}`;
                    const targetRestructured: any = { nextNodedetails, data, sessionId: instagramCcaasTable[0]?.sessionId };
                    targetRestructured.msg = nextNodedetails?.data?.information.message;
                    const messagenextNode = nextNodedetails?.data?.children?.find((nodes: any) => nodes?.type === RESPONSE?.success_message) ?? { childId: '' };
                    let messagenextNodedetails: any = {};
                    if (messagenextNode.childId) {
                      messagenextNodedetails = await getNodeDataById(messagenextNode?.childId, nodes);
                    }
                    const message: any = {
                      messageID: uuidv4(),
                      message: `${targetRestructured?.msg}`,
                      sendAt: Date.now(),
                      messageType: 'text',
                      sendMediaUrl: '',
                      isBot: true,
                      isCustomer: false,
                      fileType: '',
                      direction: 'OUT',
                      isLike: 0
                    };
                    const messageType: any = { agentText: message }
                    const stringifiedMessage: any = { message: messageType };
                    stringifiedMessage.InstagramHistoryId = instagramCcaasTable[0]?.InstagramHistoryId;
                    await updateChatMessageInsta(stringifiedMessage);
                    updateIsBotInsta(instagramCcaasTable[0]?.InstagramHistoryId);
                    const body = {
                      domainId: instagramCcaasTable[0]?.domainId,
                      customerId: instagramCcaasTable[0]?.customerInstagramId,
                      pageId: instagramCcaasTable[0]?.agentInstagramId,
                      type: 'text',
                      message: targetRestructured?.msg?.replace(/<[^>]+>/g, '')
                    };
                    await axios.post(process?.env?.whatsappOut, body);
                    const _cpynextNodedetails: any = { ...messagenextNodedetails };
                    targetRestructured.currentNode = _cpynextNodedetails;
                  }
                }
              }
            } catch (error) {
              const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
              return handleChatFlowType(nextNode, instagramCcaasTable, nodes, input, req);
            }
          } else if (type === 'connectors') {
            logger.info('not implemented');
          }
        } catch (error: any) {
          const nextNode = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'error') ?? { childId: '' };
          return handleChatFlowType(nextNode, instagramCcaasTable, nodes, input, req);
        }
        break;
      }
      case 'transferToQueue': {
        try {
          logger.info('transferToQueue>>>>>>>>>>>>>>>>>>', currentNode);
          const getBotDetails: any = await getBotDetailInstagram(instagramCcaasTable[0]);
          const getCoid = await getCustomerCoIdInsta(instagramCcaasTable[0]);
          if (currentNode?.data?.information?.type === 1) {
            insertCustomerInQueueInsta(instagramCcaasTable[0], currentNode, currentNode?.data?.information?.transferTo);
            const agentDetailRes: any = await transferChat(instagramCcaasTable[0], currentNode);
            const agentDetails = agentDetailRes ? [agentDetailRes] : [];
            try {
              if (Array.isArray(agentDetails) && agentDetails.length > 0) {
                const getPersonalData: any = await getPersonalDetailsInstagram(instagramCcaasTable[0].customerInstagramId, instagramCcaasTable[0].domainId);
                logger.info(emiiter?.sendInvite, `${agentDetails[0]?.domainId}_${agentDetails[0]?.ext}`);
                ioredisSocialMedia.to(`${agentDetails[0]?.domainId}_${agentDetails[0]?.ext}`).emit(emiiter?.sendInvite, {
                  channelType: instagramCcaasTable[0].channelType,
                  customerNumber: instagramCcaasTable[0].customerInstagramId,
                  agentNumber: instagramCcaasTable[0].agentInstagramId,
                  historyId: instagramCcaasTable[0]?.InstagramHistoryId,
                  sessionId: instagramCcaasTable[0].sessionId,
                  calledPersonDetails: getPersonalData[0] ?? null,
                  allAgent: true,
                  isTransferChat: 0,
                  currentNode: currentNode?.data?.information?.transferTo ?? 0
                });
                await sendConnectLiveAgentInsta(instagramCcaasTable[0]);
                sendConnectLiveAgentMessage(instagramCcaasTable);
              }
            } catch (error) {
              logger.error('transferToQueue err', error);
            }
          } else {
            const getUser = await getUserExt(currentNode?.data?.information);
            await connectToliveAgent(getUser, instagramCcaasTable, getBotDetails, getCoid);
          }
        } catch (error: any) {
          logger.error('transferToQueue err', error);
          const nextNodeData = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
          return handleChatFlowType(nextNodeData, ioredisSocialMedia, nodes, input, req);
        }
        break;
      }
      case 'randomise': {
        try {
          logger.info('randomise>>>>>>>>>>>>>>>>>>>>>>>', currentNode);
          const randonIndex = Math.floor(Math.random() * currentNode?.data?.children?.length);
          const childIdData = currentNode?.data?.children[randonIndex];
          await updateNextNodeInsta(instagramCcaasTable[0], childIdData?.childId);
          if (entryCarousal?.length !== 0) {
            await sendCarouselDataInsta(instagramCcaasTable[0], entryCarousal);
          }
          return handleChatFlowType(childIdData, instagramCcaasTable, nodes, input, req);
        } catch (error) {
          logger.error('randomise>>>>>>>>>>>>>>>>>>>>>>>', error);
          const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
          return handleChatFlowType(nextNode, instagramCcaasTable, nodes, input, req);
        }
        break;
      }
      case 'addFlow': {
        try {
          logger.info('addFlow>>>>>>>>>>>>>>>>>>>>>>>', currentNode);
          const nodesString: any = await getnodesdata(currentNode?.data?.information?.flow_id);
          nodes = JSON.parse(nodesString);
          const startingNode = await getStartNode(nodes);
          const currentNodeId = startingNode?.data?.children[0]?.childId ?? '';
          await updateNextNodeInsta(instagramCcaasTable[0], currentNodeId);
          if (entryCarousal?.length !== 0) {
            await sendCarouselDataInsta(instagramCcaasTable[0], entryCarousal);
          }
          return handleChatFlowType(currentNodeId, instagramCcaasTable, nodes, input, req);
        } catch (error) {
          logger.error('addFlow>>>>>>>>>>>>>>>>>>>>>>>', error);
          const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
          return handleChatFlowType(nextNode, instagramCcaasTable, nodes, input, req);
        }
        break;
      }
      case 'coupon': {
        try {
          logger.info('coupon>>>>>>>>>>>>>>>>>>>>>>>', currentNode);
          const couponCode = currentNode?.data?.information?.type === 'coupon';
          if (couponCode) {
            const couponMessage = JSON.parse(currentNode?.data?.information?.coupon_data);
            const messageCode = couponMessage?.message;
            const couponCodeMessage = `*${couponMessage?.coupon}*`;
            const useCode = messageCode + " " + couponCodeMessage;
            await sendMessageCustomerInstagram(instagramCcaasTable[0], useCode);
            const nextNode = currentNode?.data?.children?.find((nodes1: any) => nodes1?.type === RESPONSE?.success_message) ?? { childId: '' };
            const nextNodedetails: any = await getNodeDataById(nextNode?.childId, nodes);
            let message: any = {};
            try {
              message = {
                messageID: uuidv4(),
                sessionId: instagramCcaasTable[0]?.sessionId,
                message: useCode?.replace(/<[^>]+>/g, ''),
                sendAt: Date.now(),
                messageType: 'text',
                sendMediaUrl: '',
                isBot: true,
                isCustomer: false,
                fileType: '',
                direction: 'OUT',
                isLike: 0
              };
              const messageList: any = { message };
              const messageType: any = { agentText: messageList };
              const stringifiedMessage: any = { message: messageType };
              stringifiedMessage.InstagramHistoryId = instagramCcaasTable[0]?.InstagramHistoryId;
              await updateChatMessageInsta(stringifiedMessage);
            } catch (error) {
              const nextNodes = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
              // return handleChatFlowType(nextNode?.childId ?? null, whatsappCcaasTable[0] ?? null, node, io);
            }
            await updateNextNodeInsta(instagramCcaasTable[0], nextNode?.childId);
            if (entryCarousal?.length !== 0) {
              await sendCarouselDataInsta(instagramCcaasTable[0], entryCarousal);
            }
            return handleChatFlowType(nextNode, instagramCcaasTable, nodes, input, req);
          }
        } catch (error) {
          logger.error('coupon>>>>>>>>>>>>>>>>>>>>>>>', error);
          const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
          return handleChatFlowType(nextNode, instagramCcaasTable, nodes, input, req);
        }
        break;
      }
      case 'checkCondition': {
        try {
          logger.info('checkCondition>>>>>>>>>>>>>>>>>>>>>>>', currentNode);
          const conditionParseData = JSON.parse(currentNode?.data?.information?.condition_data);
          const AgentAvailibility = await getAgentAvailibility(instagramCcaasTable[0]);
          const getEmailData: any = await getCustomerEmailIdInsta(instagramCcaasTable[0]);
          const getExistingCustomer: any = await getExistingCustomerContactInsta(instagramCcaasTable[0]);
          for (let groupsIndex = 0; groupsIndex < conditionParseData?.groups?.length; groupsIndex++) {
            const currentGroupData = conditionParseData?.groups[groupsIndex]?.data;
            let description: any = "";
            let isSuccess = false;
            for (let condition_data of currentGroupData) {
              if (condition_data?.content === 7) {
                description = AgentAvailibility;
              } else if (condition_data?.content === 9) {
                description = req?.body?.text?.body?.toLowerCase();
              } else if (condition_data?.content === 10) {
                description = req?.body?.text?.body;
              } else if (condition_data?.content === 11) {
                description = req?.body?.text?.body;
              } else if (condition_data?.content === 12) {
                description = getExistingCustomer?.result;
              } else if (condition_data?.content === 14) {
                var daysOfWeek = ['Sunday', 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday'];
                description = daysOfWeek[new Date().getDay()]
              }
              if (condition_data.condition === 1) {
                if (condition_data.description === description) {
                  isSuccess = true
                } else {
                  isSuccess = false
                }
              } else if (condition_data.condition === 2) {
                if (condition_data.description?.split(" ")[0] === description?.split(" ")[0]) {
                  isSuccess = true
                } else {
                  isSuccess = false
                }
              } else if (condition_data.condition === 3) {
                const lastWordData = condition_data?.description?.split(" ")[condition_data?.description.split(" ").length - 1];
                const lastWordDatas = description?.split(" ")[description?.split(" ").length - 1];
                if (lastWordData === lastWordDatas) {
                  isSuccess = true
                } else {
                  isSuccess = false
                }
              } else if (condition_data.condition === 4) {
                if (condition_data.description.includes(description)) {
                  isSuccess = true
                } else {
                  isSuccess = false
                }
              }
              if (isSuccess) {
                const nextNode = currentNode?.data?.children?.find((nodes1: any) => nodes1?.type === 'success') ?? { childId: '' };
                const nextNodedetails: any = await getNodeDataById(nextNode?.childId, nodes);
                await updateNextNodeInsta(instagramCcaasTable[0], nextNode?.childId);
                return handleChatFlowType(nextNode, instagramCcaasTable, nodes, input, req);
              } else {
                const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
                await updateNextNodeInsta(instagramCcaasTable[0], nextNode?.childId);
                return handleChatFlowType(nextNode, instagramCcaasTable, nodes, input, req);
              }
            }
          }
        } catch (error) {
          logger.error('checkCondition err', error);
          const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
          return handleChatFlowType(nextNode, instagramCcaasTable, nodes, input, req);
        }
        break;
      }
      case 'disconnect': {
        logger.info("disconnect>>>>>>>>>>>>>>>>>>>>>>>>", currentNode);
        await getDispositinDisconnectStatusInsta(instagramCcaasTable[0]);
        const getDispositin: any = await getDispositinStatus(instagramCcaasTable[0]);
        const closedDispositions = getDispositin[0]?.filter((item: { dispositionName: string; }) => item.dispositionName === 'Closed');
        await updateDispositionDiscnnectInsta(instagramCcaasTable[0], closedDispositions[0]);
        await updateDispositionBotInsta(instagramCcaasTable[0]);
        const updateScore: any = await sentimentalScoreDataInsta(instagramCcaasTable[0]);
        const updateScoreData = updateScore?.data?.sentimentalScore;
        await updateSentimentScoreInstagramBot(instagramCcaasTable[0], updateScoreData);
        const sentimentSupervisorBot: any = await getSupervisiorList(instagramCcaasTable[0]);
        sentimentSupervisorBot.forEach((ext: any) => {
          console.log('sentimentWhatsappBot', `${instagramCcaasTable[0]?.domainId}_${ext?.ext}`);
          ioredisSocialMedia.to(`${instagramCcaasTable[0]?.domainId}_${ext?.ext}`).emit('supervisiorActivityBot');
        });
        await new Promise(resolve => setTimeout(resolve, 500));
        return;
      }
      default: {
        logger.info('Default calling');
        return;
      }
    }
  } catch (error) {
    logger.error('disconnect err', error);
  }
}

/**
 * 
 * @param liveAgent 
 * @param instagramCcaasTable 
 * @param getBotDetails 
 * @param getCoid 
 * @returns 
 */
export const connectToliveAgent = async (liveAgent: any, instagramCcaasTable: any, getBotDetails: any, getCoid: any): Promise<void> => {
  try {
    const [{ customerInstagramId: customerId, domainId, channelType, agentInstagramId, InstagramHistoryId, sessionId }] = instagramCcaasTable;
    if (Array.isArray(liveAgent) && liveAgent[0]?.domainId && liveAgent[0]?.ext) {
      const agent = liveAgent[0];
      const [personalData]: any = await getPersonalDetailsInstagram(customerId, domainId);
      const redisChannel = `${agent?.domainId}_${agent?.ext}`;
      logger.info(emiiter?.sendInvite, redisChannel);
      ioredisSocialMedia.to(redisChannel).emit(emiiter?.sendInvite, {
        channelType,
        customerNumber: customerId,
        agentNumber: agentInstagramId,
        historyId: InstagramHistoryId,
        sessionId,
        calledPersonDetails: personalData ?? null,
        allAgent: true,
        isTransferChat: 1,
      });
      await Promise.all([await sendConnectLiveAgentInsta(instagramCcaasTable[0]), sendConnectLiveAgentMessage(instagramCcaasTable)]);
      return;
    }
    logger.info("No live agent found. Checking bot fallback...");
    // if (Array.isArray(getBotDetails) && getBotDetails?.length > 0 
    // && Number(getBotDetails[0]?.callBackToggle) === 1) {
    //   const redirectUrlCallbck = `InstagramWebcallback?coId=${getCoid[0]?.coid}&domainId=${domainId}&instagramId=${customerId}&historyId=${InstagramHistoryId}`;
    //   await Promise.all([
    //     sendCallbackRequestInsta(instagramCcaasTable[0], redirectUrlCallbck),
    //     updateCallbackStatusInsta(instagramCcaasTable[0]),
    //     sendAgentAvailable(instagramCcaasTable)
    //   ]);
    // } else {
    //   await Promise.all([sendMessageInsta(instagramCcaasTable[0]),
    //   (async () => {
        // sendAgentAvailableMissedInsta(instagramCcaasTable);
    //     updateMissedChatInsta(instagramCcaasTable[0]);
    //   })(),
    //   (async () => {
    //     const sentimentSupervisorBot: any = await getSupervisiorList(instagramCcaasTable[0]);
    //     sentimentSupervisorBot?.forEach((ext: any) => {
    //       const redisChannel = `${domainId}_${ext?.ext}`;
    //       logger.info(emiiter?.supervisiorActivityBot, redisChannel);
    //       ioredisSocialMedia.to(redisChannel).emit(emiiter?.supervisiorActivityBot);
    //     });
    //   })()
    //   ]);
    // }
  } catch (error) {
    logger.error('Error in connectToliveAgent (Instagram):', error);
    throw error;
  }
};


/**
 * 
 * @param instagramCcaasTable 
 */
export const sendConnectLiveAgentMessage = async (instagramCcaasTable: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      let message = {};
      message = {
        messageID: uuidv4(),
        sessionId: instagramCcaasTable[0]?.sessionId,
        message: messageValue?.liveAgent,
        sendAt: Date.now(),
        messageType: 'text',
        sendMediaUrl: '',
        isBot: true,
        isCustomer: false,
        fileType: '',
        direction: 'OUT',
        isLike: 0
      };
      const messageList: any = { message };
      const messageType: any = { agentText: messageList };
      const stringifiedMessage: any = { message: messageType };
      stringifiedMessage.InstagramHistoryId = instagramCcaasTable[0]?.InstagramHistoryId;
      await updateChatMessageInsta(stringifiedMessage);
    } catch (error) {
      logger.error('sendConnectLiveAgentMessage err', error);
      reject(error);
    }
  });
}

/**
 * 
 * @param instagramCcaasTable 
 * @returns 
 */
export const sendAgentAvailable = async (instagramCcaasTable: any) => {
  return new Promise(async (resolve, reject) => {
    let message: any = {};
    try {
      message = {
        messageID: uuidv4(),
        sessionId: instagramCcaasTable[0]?.sessionId,
        message: messageValue?.agentBusy,
        sendAt: Date.now(),
        messageType: 'text',
        pdfurl: '',
        isBot: true,
        isCustomer: false,
        fileType: '',
        direction: 'OUT',
        isLike: 0
      };
      const messageList: any = { message };
      const messageType: any = { agentText: messageList };
      const stringifiedMessage: any = { message: messageType };
      stringifiedMessage.InstagramHistoryId = instagramCcaasTable[0]?.InstagramHistoryId;
      await updateChatMessageInsta(stringifiedMessage);
    } catch (error) {
      logger.error('sendAgentAvailable err', error);
      reject(error);
    }
  });
}

/**
 * 
 * @param instagramCcaasTable 
 * @returns 
 */
export const sendAgentAvailableMissedInsta = (instagramCcaasTable: any) => {
  return new Promise(async (resolve, reject) => {
    let message: any = {};
    try {
      message = {
        messageID: uuidv4(),
        sessionId: instagramCcaasTable[0]?.sessionId,
        message: messageValue?.agentBusy,
        sendAt: Date.now(),
        messageType: 'text',
        pdfurl: '',
        isBot: true,
        isCustomer: false,
        fileType: '',
        direction: 'OUT',
        isLike: 0
      };
      const messageList: any = { message };
      const messageType: any = { agentText: messageList };
      const stringifiedMessage: any = { message: messageType };
      stringifiedMessage.InstagramHistoryId = instagramCcaasTable[0]?.InstagramHistoryId;
      await updateChatMessageInsta(stringifiedMessage);
      updateIsBotInsta(instagramCcaasTable[0]?.InstagramHistoryId);
    } catch (error) {
      logger.error('sendAgentAvailableMissedInsta err', error);
      reject(error);
    }
  });
};

/**
 * 
 * @param instagramCcaasTable 
 * @param messageData 
 * @returns 
 */
export const sendMessageCustomerCaiMessageInsta = async (instagramCcaasTable: any, messageData: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      let message: any = {};
      message = {
        messageID: uuidv4(),
        sessionId: instagramCcaasTable[0]?.sessionId,
        message: messageData ?? '',
        sendAt: Date.now(),
        messageType: 'text',
        sendMediaUrl: '',
        isBot: true,
        isCustomer: false,
        fileType: '',
        direction: 'OUT',
        isLike: 0
      };
      const messageList: any = { message };
      const messageType: any = { agentText: messageList };
      const stringifiedMessage: any = { message: messageType };
      stringifiedMessage.InstagramHistoryId = instagramCcaasTable[0]?.InstagramHistoryId;
      await updateChatMessageInsta(stringifiedMessage);
      updateIsBotInsta(instagramCcaasTable[0]?.InstagramHistoryId);
    } catch (error) {
      logger.error('sendMessageCustomerCaiMessageInsta err', error);
      reject(error);
    }
  });
}

/**
 * 
 * @param data 
 * @param instagramCcaasTable 
 * @param currentNode_ref 
 * @param nodes 
 * @param getNlpData 
 * @param req 
 * @returns 
 */
export const uploadFilesFlowInsta = async (data: any, instagramCcaasTable: any, currentNode_ref: any, nodes: any, getNlpData: any, req: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      const nextData = currentNode_ref?.data?.children[0];
      await updateNextNodeInsta(instagramCcaasTable[0], nextData?.childId);
      return handleChatFlowType(null, instagramCcaasTable, nodes, '', req);
    } catch (error) {
      logger.error('uploadFilesFlowInsta err', error);
      reject(error);
    }
  });
}

/**
 * 
 * @param trigger 
 * @param req 
 * @param nodes 
 * @param instagramCcaasTable 
 * @param getNlpData 
 * @returns 
 */
export const triggerDataHandlerInsta = (trigger: any, req: any, nodes: any, instagramCcaasTable: any, getNlpData: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      let matches: any = false;
      if (trigger[0]?.triggerKey[0] === "7") {
        matches = trigger[0]?.inputValues.some((triggerData: any) => {
          return triggerData?.value?.toLowerCase() === req?.body?.text?.body?.toLowerCase();
        });
      } else if (trigger[0]?.triggerKey[0] === "4") {
        const getCustomerdata = await insertCustomerContact(req.body);
        matches = getCustomerdata[0]?.errcode !== 1;
      } else if (trigger[0]?.triggerKey[0] === "5") {
        const getCustomerdata = await insertCustomerContact(req.body);
        matches = getCustomerdata[0]?.errcode === 1;
      }
      if (matches || trigger?.length === 0) {
        const startingNode = await getStartNode(nodes);
        const currentNodeId = startingNode?.data?.children[0]?.childId ?? '';
        await updateNextNodeInsta(instagramCcaasTable[0], currentNodeId);
        handleChatFlowType(currentNodeId, instagramCcaasTable, nodes, '', req);
      } else {
        resolve(false);
      }
    } catch (error) {
      logger.error('triggerDataHandlerInsta err', error);
      reject(error);
    }
  });
};

/**
 * 
 * @param instagramCcaasTable 
 * @returns 
 */
export const sendFlowBreakMessageInsta = async (instagramCcaasTable: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      let message: any = {};
      message = {
        messageID: uuidv4(),
        sessionId: instagramCcaasTable[0]?.sessionId,
        message: messageValue?.disconnectFlow,
        sendAt: Date.now(),
        messageType: 'text',
        sendMediaUrl: '',
        isBot: true,
        isCustomer: false,
        fileType: '',
        direction: 'OUT',
        isLike: 0
      };
      const messageList: any = { message };
      const messageType: any = { agentText: messageList };
      const stringifiedMessage: any = { message: messageType };
      stringifiedMessage.InstagramHistoryId = instagramCcaasTable[0]?.InstagramHistoryId;
      await updateChatMessageInsta(stringifiedMessage);
      updateIsBotInsta(instagramCcaasTable[0]?.InstagramHistoryId);
    } catch (error) {
      logger.error('sendFlowBreakMessageInsta err', error);
      reject(error);
    }
  });
}

/**
 * 
 * @param instagramCcaasTable 
 * @returns 
 */
export const sendFlowBreakMessageRemainderInsta = async (instagramCcaasTable: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      let message: any = {};
      message = {
        messageID: uuidv4(),
        sessionId: instagramCcaasTable[0]?.sessionId,
        message: messageValue?.continueFlow,
        sendAt: Date.now(),
        messageType: 'text',
        sendMediaUrl: '',
        isBot: true,
        isCustomer: false,
        fileType: '',
        direction: 'OUT',
        isLike: 0
      };
      const messageList: any = { message };
      const messageType: any = { agentText: messageList };
      const stringifiedMessage: any = { message: messageType };
      stringifiedMessage.InstagramHistoryId = instagramCcaasTable[0]?.InstagramHistoryId;
      await updateChatMessageInsta(stringifiedMessage);
      updateIsBotInsta(instagramCcaasTable[0]?.InstagramHistoryId);
    } catch (error) {
      logger.error('sendFlowBreakMessageRemainderInsta err', error);
      reject(error);
    }
  });
}

/**
 * 
 * @param instagramCcaasTable 
 * @param errorMessage 
 * @returns 
 */
export const sendFlowMessageDataInsta = async (instagramCcaasTable: any, errorMessage: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      let message: any = {};
      message = {
        messageID: uuidv4(),
        sessionId: instagramCcaasTable[0]?.sessionId,
        message: errorMessage,
        sendAt: Date.now(),
        messageType: 'text',
        sendMediaUrl: '',
        isBot: true,
        isCustomer: false,
        fileType: '',
        direction: 'OUT',
        isLike: 0
      };
      const messageList: any = { message };
      const messageType: any = { agentText: messageList };
      const stringifiedMessage: any = { message: messageType };
      stringifiedMessage.InstagramHistoryId = instagramCcaasTable[0]?.InstagramHistoryId;
      await updateChatMessageInsta(stringifiedMessage);
      updateIsBotInsta(instagramCcaasTable[0]?.InstagramHistoryId);
    } catch (error) {
      logger.error('sendFlowMessageDataInsta err', error);
      reject(error);
    }
  });
}

/**
 * 
 * @param trigger 
 * @param req 
 * @param nodes 
 * @param whatsappCcaasTable 
 * @param getNlpData 
 */
export const triggerDataSendMessageHandlerInsta = (trigger: any, req: any, nodes: any, instagramCcaasTable: any, getNlpData: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      let matches: any = false;
      if (trigger[0]?.triggerKey[0] === "7") {
        matches = trigger[0]?.inputValues.some((triggerData: any) => {
          return triggerData?.value?.toLowerCase() === req?.body?.text?.body?.toLowerCase();
        });
      } else if (trigger[0]?.triggerKey[0] === "4") {
        const getCustomerdata = await insertCustomerContact(req.body);
        matches = getCustomerdata[0]?.errcode === 1 ? false : true;
      } else if (trigger[0]?.triggerKey[0] === "5") {
        const getCustomerdata = await insertCustomerContact(req.body);
        matches = getCustomerdata[0]?.errcode === 1 ? true : false;
      }
      if (matches || trigger?.length === 0) {
        const startingNode = await getStartNode(nodes);
        const currentNodeId = startingNode?.data?.children[0]?.childId ?? '';
        await updateNextNodeInsta(instagramCcaasTable[0], currentNodeId);
        return handleChatFlowType(currentNodeId, instagramCcaasTable, nodes, req?.body?.text?.body, req);
      } else {
        await sendMessageCustomerInstagram(instagramCcaasTable[0], getNlpData?.data?.message);
        let message: any = {};
        try {
          message = {
            messageID: uuidv4(),
            sessionId: instagramCcaasTable[0]?.sessionId,
            message: getNlpData?.data?.message,
            sendAt: Date.now(),
            messageType: 'text',
            pdfurl: '',
            isBot: true,
            isCustomer: false,
            fileType: '',
            direction: 'OUT',
            isLike: 0
          };
          const messageList: any = { message };
          const messageType: any = { agentText: messageList };
          const stringifiedMessage: any = { message: messageType };
          stringifiedMessage.InstagramHistoryId = instagramCcaasTable[0]?.InstagramHistoryId;
          await updateChatMessageInsta(stringifiedMessage);
          updateIsBotInsta(instagramCcaasTable[0]?.InstagramHistoryId);
        } catch (error) {
          logger.error('triggerDataSendMessageHandlerInsta err', error);
        }
      }
    } catch (error) {
      logger.error('triggerDataSendMessageHandlerInsta err', error);
      reject(error);
    }
  });
};

/**
 * 
 * @param instagramCcaasTable 
 * @param messageData 
 * @returns 
 */
export const updateMessageFlowInsta = async (instagramCcaasTable: any, messageData: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      let message = {};
      message = {
        messageID: uuidv4(),
        sessionId: instagramCcaasTable[0]?.sessionId,
        message: messageData?.getInputData[0]?.link,
        sendAt: Date.now(),
        messageType: 'text',
        sendMediaUrl: '',
        isBot: true,
        isCustomer: false,
        fileType: '',
        direction: 'OUT',
        isLike: 0
      };
      const messageList: any = { message };
      const messageType: any = { agentText: messageList };
      const stringifiedMessage: any = { message: messageType };
      stringifiedMessage.InstagramHistoryId = instagramCcaasTable[0]?.InstagramHistoryId;
      await updateChatMessageInsta(stringifiedMessage);
      updateIsBotInsta(instagramCcaasTable[0]?.InstagramHistoryId);
    } catch (error) {
      logger.error('updateMessageFlowInsta err', error);
      reject(error);
    }
  });
}

/**
 * 
 * @param instagramCcaasTable 
 * @param messageData 
 * @returns 
 */
export const updateMessageFlowDataInsta = async (instagramCcaasTable: any, messageData: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      let message = {};
      message = {
        messageID: uuidv4(),
        sessionId: instagramCcaasTable[0]?.sessionId,
        message: messageData?.replace(/<[^>]+>/g, ''),
        sendAt: Date.now(),
        messageType: 'text',
        sendMediaUrl: '',
        isBot: true,
        isCustomer: false,
        fileType: '',
        direction: 'OUT',
        isLike: 0
      };
      const messageList: any = { message };
      const messageType: any = { agentText: messageList };
      const stringifiedMessage: any = { message: messageType };
      stringifiedMessage.InstagramHistoryId = instagramCcaasTable[0]?.InstagramHistoryId;
      await updateChatMessageInsta(stringifiedMessage);
      updateIsBotInsta(instagramCcaasTable[0]?.InstagramHistoryId);
    } catch (error) {
      logger.error('updateMessageFlowDataInsta err', error);
      reject(error);
    }
  });
}

/**
 * 
 * @param instagramCcaasTable 
 * @param message 
 */
export const sendMessageCustomerCountInsta = async (instagramCcaasTable: any, messageData: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      let message: any = {};
      message = {
        messageID: uuidv4(),
        sessionId: instagramCcaasTable?.sessionId,
        message: messageData ?? '',
        sendAt: Date.now(),
        messageType: 'text',
        sendMediaUrl: '',
        isBot: true,
        isCustomer: false,
        fileType: '',
        direction: 'OUT',
        isLike: 0
      };
      const messageList: any = { message };
      const messageType: any = { agentText: messageList };
      const stringifiedMessage: any = { message: messageType };
      stringifiedMessage.InstagramHistoryId = instagramCcaasTable?.InstagramHistoryId;
      await updateChatMessageInsta(stringifiedMessage);
    } catch (error) {
      logger.error('sendMessageCustomerCountInsta err', error);
      reject(error);
    }
  });
}

/**
 * 
 * @param data 
 * @param currentData 
 * @param queueId 
 */
export const insertCustomerInQueueInsta = async (data: any, currentData: any, queueId: any) => {
  try {
    const getCId = await getCustomerCoIdInsta(data);
    const queueDetails: any = await getQueueSettingS(queueId, data?.domainId);
    const customerDetails: any = await getCustomerConatctDetails(data?.domainId, getCId[0]);
    const queueWeightTime: any = await getAllQueueWaitTime(data?.domainId);
    const result = queueWeightTime?.filter((list: any) => list?.qid === queueId && list?.queueName === queueDetails?.name && list?.queueType === channelType?.instagram);
    const queueListData = [];
    const waitTimmer: any = JSON.parse(queueDetails?.maximumCallers);
    const waitQueueListData: any = result?.length > 0 ? JSON.parse(result[0]?.queueList) : [];
    if (waitTimmer?.type !== 'Set Max Caller limit' || waitTimmer?.queueLimit > waitQueueListData?.length) {
      const queueTypeData = {
        firstName: customerDetails[0]?.firstName ?? null,
        lastName: customerDetails[0]?.lastName ?? null,
        sessionId: data?.sessionId,
        queuedTime: Math.floor(Date.now() / 1000),
        type: channelType?.Instagram,
        chatHistoryId: data?.InstagramHistoryId,
        queueSetting: queueDetails?.maximumWaitTime,
        queueUserCount: queueDetails?.maximumCallers,
        channelId: data,
        currentNode: currentData
      };
      if (result?.length > 0) {
        const preQueueData: any = JSON.parse(result[0]?.queueList);
        preQueueData?.push(queueTypeData);
        queueListData.push(...preQueueData);
      } else {
        queueListData.push(queueTypeData);
      }
      const payload = {
        qid: queueId ?? 0,
        queueName: queueDetails?.name,
        queueType: channelType?.instagram,
        domainId: data?.domainId,
        queueList: JSON.stringify(queueListData)
      };
      try {
        await insertUpdateQueueWaitTimeInsta(payload, queueDetails, data);
      } catch (err) {
        console.log(err);
      }
      ioredisSocialMedia.emit(emiiter?.queueWait, payload);
      const redisData = {
        qid: queueId,
        queueType: channelType?.instagram,
        domainId: data?.domainId,
      };
      const redisQueueData: any = await ioredisInstagramQueue.get(process.env.INSTA_QUEUE);
      if (redisQueueData) {
        const redisMap = JSON.parse(redisQueueData);
        const hasQid = redisMap?.some((item: any) => item.qid === queueId);
        if (!hasQid) {
          redisMap.push(redisData);
          ioredisInstagramQueue.set(process.env.INSTA_QUEUE, JSON.stringify(redisMap));
        }
      } else {
        ioredisInstagramQueue.set(process.env.INSTA_QUEUE, JSON.stringify([redisData]));
      }
    } else {
      data.queueId = waitTimmer?.overFlowQueue;
      insertCustomerInQueueInsta(data, currentData, waitTimmer?.overFlowQueue);
    }
  } catch (err) {
    console.log(err);
  }
};

/**
 * 
 * @param instagramCcaasTable 
 * @param messageData 
 * @returns 
 */
export const updateMessageFlowAppoinmentInsta = async (instagramCcaasTable: any, messageData: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      let message = {};
      message = {
        messageID: uuidv4(),
        sessionId: instagramCcaasTable?.sessionId,
        message: messageData,
        sendAt: Date.now(),
        messageType: 'text',
        sendMediaUrl: '',
        isBot: true,
        isCustomer: false,
        fileType: '',
        direction: 'OUT',
        isLike: 0
      };
      const messageList: any = { message };
      const messageType: any = { agentText: messageList };
      const stringifiedMessage: any = { message: messageType };
      stringifiedMessage.InstagramHistoryId = instagramCcaasTable?.InstagramHistoryId;
      await updateChatMessageInsta(stringifiedMessage);
    } catch (error) {
      logger.error('updateMessageFlowAppoinmentInsta err', error);
      reject(error);
    }
  });
}

/**
 * 
 * @param fromDate 
 * @param toDate 
 * @returns get Date Range
 */

function getDateRange(fromDate: any, toDate: any) {
  const startDate = new Date(fromDate);
  const endDate = new Date(toDate);
  const dateRange = [];
  for (let d = new Date(startDate); d <= endDate; d.setDate(d.getDate() + 1)) {
    dateRange.push(new Date(d).toISOString().split('T')[0]);
  }
  return dateRange;
}

/**
 * 
 * @param dates 
 * @returns get Next 10Days
 */
function getNext10Days(dates: any) {
  const dateArray = [];
  for (let i = 0; i < 9; i++) {
    dateArray.push({ content_type: "text", title: dates[i], payload: i + 1 });
  }
  return dateArray;
};
