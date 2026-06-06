import axios from 'axios';
import CryptoJS from 'crypto-js';
import { v4 as uuidv4 } from 'uuid';
import {
  agentDetailsApi,
  AppointmentInfoPaymentCheckFb,
  assignCallBackFacebook,
  deleteFacebookConfigDetails,
  facebookFeedbackUpdateDB,
  getAdminEmail,
  getAllQueueWaitTime,
  getAssingToApi,
  getBotDetail,
  getBotMessageRes,
  getConnectorTokenInfo,
  getCustomerCoId,
  getCustomerConatctDetails,
  getCustomerEmailId,
  getCustomerSatisficationDao,
  getDispositinStatus,
  getExistingCustomerContact,
  getFacebookConfig,
  getFacebookConfigData,
  getFacebookConfigDetails,
  getfbMediaUrl,
  getFlowTracking,
  getNextNode,
  getnodesdata,
  getNoOfRepeatsRes,
  getPersonalDetails,
  getQuestionsAns,
  getQueueSettingS,
  getRemaindeFbResponse,
  getRemainingSessionEmailSent,
  getSupervisiorDetails,
  getSupervisiorList,
  getUserExt,
  getUserListByExt,
  insertBotMessage,
  insertBotMessageIsRemainderFb,
  insertBotMessageRemainderfacebook,
  insertCustomerContact,
  insertCustomerContactEmail,
  insertCustomerContactFB,
  insertFacebookIn,
  insertFlowTracking,
  insertUpdateFacebookConfig,
  insertUpdateQueueWaitTimeDao,
  insertWhatsappCallbackDao,
  sendButtonLink,
  sendCaasFacebook,
  sendCallbackRequest,
  sendCarouselData,
  sendCustomerInput,
  sendImageCustomer,
  sentimentalScoreData,
  updateAppointmentConsultant,
  updateAppointmentConsultantId,
  updateAppointmentdate,
  updateAppointmentDuration,
  updateAppointmentServiceId,
  updateAppointmentTime,
  updateAppointmentUserCreateBy,
  updateAppointmentUserDetails,
  updateCallbackStatus,
  updateCategoryId,
  updateChatMessage,
  updateConnectData,
  updateCustomerContactFB,
  updateCustomerFirstName,
  updateCustomerFirstNameAppoinment,
  updateCustomerId,
  updateCustomerLastName,
  updateCustomerLastNameAppoinment,
  updateCustomerLocation,
  updateCustomerName,
  updateDispositionBot,
  updateDispositionDiscnnect,
  updateDispositionFacebook,
  updateEmailIdFB,
  updateIsBotFb,
  updateMissedChat,
  updateNextNode,
  updateNoOfRepeats,
  updatePageIdFacebook,
  updatePhNumber,
  updatePhoneNumberFB,
  updateSentimentScore,
  updateSentimentScoreBot,
  updateStatusFacebook
} from '../../dao/facebook';
import { getCompanyDetails, updateCallbackstatus } from '../../dao/instagram';
import {
  AppointmentInfo,
  bookingPayment,
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
import {
  appoinment,
  channelType,
  emiiter,
  messageValue,
  RESPONSE
} from '../../helpers/constants';
import { getDispositinDisconnectStatus, sendAllUserName, sendAppoinmentPayment, sendAppointmentDateFb, sendAppointmentDuration, sendAppointmentOneToOne, sendAppointmentRes, sendBookingDetails, sendBookingDetailsOneToOne, sendbookingDuration, sendButtonFb, sendCategoryName, sendConnectLiveAgent, sendFlowBreakInpuRemaindertFb, sendFlowBreakInput, sendMessage, sendMessageCustomer, sendMessageCustomerFbFlow, sendTransferAgent, sendTypingIndicator } from '../../helpers/fbmeta';
import { sendCustomerSatificationFb, sendCustomerSatificationFbDropdown } from '../../helpers/instameta';
import { AgentAssist, getFacebookBot, nlpResponseData, sentimentalScore } from '../../helpers/nlp';
import {
  addContextVariable,
  callGoogleSheetSyncAPI,
  callSessionExpiryAPI,
  contextPattern,
  correctjsondata,
  fetchApi,
  getAgentAvailibility,
  getNodeDataById,
  getPlandetails,
  getResponseData,
  getStartNode,
  getStartNodeCondition,
  insertUpdateCrmContact,
  replacechatPatternWithVariables,
  replacePatternWithVariables,
  safeParseJson,
  sendEmail,
  sendEmailPlainText,
  sendEmailPlainTextFB
} from '../../helpers/utils';
import { logger } from '../../plugins/log';
import { ioredisFacebookQueue, ioredisSocialMedia } from '../../plugins/redis';
import { transferChat, transferChatToAvailableAgent } from '../../services/transferToQueue';

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function insertFacebookInHandler(req: any, res: any, done: any) {
  logger.info('insertFacebookInHandlerEnter>>>>>', req.body);
  try {
    req.body.sessionId = uuidv4();
    req.body.messageId = uuidv4();
    if (req?.body?.type === 'quickReply' && req?.body?.text?.body?.includes('-') && req?.body?.text?.title !== 'More') {
      facebookFeedbackUpdateDB(req?.body);
      res.send({ statusCode: 200, message: RESPONSE.success_message });
      return;
    }
    const facebookCcaasTable = await insertFacebookIn(req?.body);
    logger.info('MysqlInsertData>>>>>', facebookCcaasTable);
    const isEmail: any = await getRemainingSessionEmailSent(facebookCcaasTable[0]);
    logger.info('IsSendEmail>>>>>>>>>', isEmail);
    if (isEmail[0]?.isEmailSent !== '') {
      callSessionExpiryAPI(isEmail[0]);
    }
    const [getCoid, getBotDetails] = await Promise.all([insertCustomerContactFB(req?.body), getBotDetail(facebookCcaasTable[0])]);
    getConnectorTokenInfo(facebookCcaasTable[0], (getConnect) => {
      if (getConnect?.length > 0) {
        callGoogleSheetSyncAPI({ contactSyncDetails: getConnect[0]?.contactSyncDetails, newContacts: getCoid[0]?.coid, domainId: facebookCcaasTable[0]?.domainId, token: getConnect[0]?.token });
      }
    });
    updateCustomerId(facebookCcaasTable[0], getCoid[0]);
    if (facebookCcaasTable[0].errcode !== 0) {
      logger.error('Error in facebookCcaasTable:', facebookCcaasTable[0]);
      return res.send({ statusCode: 500, message: messageValue?.processing });
    }
    if (facebookCcaasTable[0]?.errcode !== 0) {
      return res.send({ statusCode: 200, message: RESPONSE.success_message });
    }
    logger.info('getConfigurationDetails>>>>>>', getBotDetails);
    const planDetails = getPlandetails(getBotDetails[0]);
    logger.info('checkPlan>>>>>>', planDetails);
    if (planDetails === 'Traditional' || facebookCcaasTable[0].assignedTo !== null || facebookCcaasTable[0]?.AgentId !== null) {
      await handleTraditionalPlan(facebookCcaasTable, getBotDetails, getCoid, req?.body);
    } else if (planDetails === 'CAI') {
      await handleCAIPlan(facebookCcaasTable, getBotDetails, getCoid, req?.body);
    } else if (planDetails === 'Flow') {
      await insertRoleFacebookInHandler(req, facebookCcaasTable);
    }
    res.send({ statusCode: 200, message: RESPONSE.success_message });
  } catch (err) {
    logger.info('insertFacebookInHandler err', req.body);
    res.send({ statusCode: 500, message: RESPONSE?.internal_error });
  }
}

/**
 *
 * @param facebookCcaasTable
 * @param getBotDetails
 * @param getCoid
 * @param requestBody
 * @returns
 */
async function handleTraditionalPlan(facebookCcaasTable: any, getBotDetails: any, getCoid: any, requestBody: any) {
  const fbData = facebookCcaasTable[0];
  if (!fbData) return;
  const [agentDetails]: any = await agentDetailsApi(fbData?.sessionId, fbData?.domainId);
  if (!agentDetails) return;
  const [getAssingToEXt, getPersonalData]: any = await Promise.all([
    getAssingToApi(agentDetails?.assignedTo),
    getPersonalDetails(agentDetails?.customerFacebookId, agentDetails?.domainId)
  ]);
  if (fbData?.IsUpdate === 0) {
    const liveAgent = await transferChatToAvailableAgent(fbData);
    return connectToliveAgent(liveAgent, facebookCcaasTable, getBotDetails, getCoid);
  }
  if (fbData?.IsUpdate === 1 && (!getAssingToEXt || getAssingToEXt?.length === 0)) {
    const liveAgent = await transferChatToAvailableAgent(fbData);
    return connectToliveAgent(liveAgent, facebookCcaasTable, getBotDetails, getCoid);
  }
  if (getAssingToEXt && getAssingToEXt.length > 0) {
    const assignedExt = getAssingToEXt[0]?.ext;
    const assistId = requestBody?.messageId;
    logger.info(messageValue?.appendMessage, `${fbData?.domainId}_${getAssingToEXt[0]?.ext}`);
    ioredisSocialMedia?.to(`${agentDetails?.domainId}_${assignedExt}`).emit(emiiter?.receivedFacebook, { ...agentDetails, agentAssist: true, calledPersonDetails: getPersonalData[0], messageId: assistId, }, fbData.sessionId);
    const comDetails = await getCompanyDetails(fbData);
    const nlpRes: any = await AgentAssist(fbData, comDetails, assignedExt, requestBody?.text?.body, assistId);
    if (nlpRes?.data) {
      Object.assign(nlpRes?.data, {
        channelId: fbData?.agentFacebookId,
        messageId: assistId,
        timeStamp: Date.now()
      });
      ioredisSocialMedia?.to(`${fbData?.domainId}_${assignedExt}`)?.emit(emiiter?.agentAssistOmniChannel, nlpRes.data);
    }

  }
}

/**
 *
 * @param facebookCcaasTable
 * @param getBotDetails
 * @param getCoid
 * @param requestBody
 */
async function handleCAIPlan(facebookCcaasTable: any, getBotDetails: any, getCoid: any, requestBody: any) {
  const fbData = facebookCcaasTable[0];
  if (!fbData) return;
  const supervisors = getSupervisiorList(fbData);
  if (Array.isArray(supervisors) && supervisors.length > 0) {
    supervisors?.forEach((ext: any) => {
      if (ext?.ext) {
        ioredisSocialMedia?.to(`${fbData?.domainId}_${ext?.ext}`)?.emit(emiiter?.supervisiorActivityBot);
      }
    });
  }
  let mediaUrl: any = '';
  if (['audio'].includes(requestBody?.type)) {
    const mediaRes = await getfbMediaUrl(requestBody);
    if (mediaRes?.mime_type?.includes('audio/ogg')) {
      mediaUrl = mediaRes;
    }
  }
  const comDetails = await getCompanyDetails(facebookCcaasTable[0]);
  const getBot: any = await getFacebookBot(requestBody?.text, getBotDetails[0], fbData, comDetails, getCoid, mediaUrl);
  if (getBot?.data?.intent === 'transfer_live_agent') {
    // const liveAgent = await transferChatToAvailableAgent(fbData);
    await updateCallbackstatus(facebookCcaasTable[0]);
    const liveAgent: any = await getUserListByExt(facebookCcaasTable[0], getBot?.data);
    return connectToliveAgent(liveAgent, facebookCcaasTable, getBotDetails, getCoid);
  }
  const botMessage = getBot?.data?.message ? String(getBot?.data?.message) : '';
  const cleanedMessage = botMessage.replace(/<\/?(p|div|br|li|ul|ol|h[1-6])[^>]*>/gi, '\n').replace(/<[^>]+>/g, '').replace(/\n\s*\n+/g, '\n').trim();
  if (getBot?.data?.link && getBot?.data?.link_name) {
    await sendButtonFb(facebookCcaasTable[0], getBot?.data?.link, getBot?.data?.link_name, cleanedMessage);
  } else {
    await sendMessageCustomer(fbData, botMessage);
  }
  const message = {
    messageID: uuidv4(),
    sessionId: fbData?.sessionId,
    message: botMessage,
    sendAt: Date.now(),
    messageType: 'text',
    pdfurl: '',
    isBot: true,
    isCustomer: false,
    fileType: '',
    direction: 'OUT',
    isLike: 0
  };
  await insertFacebookIn({
    sessionId: fbData?.sessionId,
    message: { agentText: { message } },
    customerId: fbData?.customerFacebookId,
    pageId: fbData?.agentFacebookId,
    IsBot: 1,
    status: 'received'
  });
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export const sendCaasFacebookHandler = async (req: any, res: any, done: any) => {
  try {
    const data: any = {
      sessionId: req.body.sessionId,
      customerId: req.body.customerId,
      pageId: req.body.pageId,
      text: req.body.text,
      type: req.body.type ?? 'update',
      attachments: req.body.attachments ?? null,
      agentName: req?.body?.agentName,
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
          whisper: data?.whisper,
        },
      },
    };
    const sendMessages: any = {
      message: messageList,
      customerId: data.customerId,
      pageId: data.pageId,
    };
    const facebookOut = await sendCaasFacebook(sendMessages, data.sessionId);
    logger.info('facebookOut', facebookOut);
    const getSupervisiorExt: any = await getSupervisiorList(facebookOut[0]);
    const getPersonalData: any = await getPersonalDetails(data?.customerId, facebookOut[0]?.domainId);
    getSupervisiorExt.forEach((ext: any) => {
      console.log(emiiter?.receivedFacebook, `${facebookOut[0]?.domainId}_${ext.ext}`);
      ioredisSocialMedia.to(`${facebookOut[0]?.domainId}_${ext.ext}`).emit(emiiter?.receivedFacebook, { ...facebookOut[0], channelType: channelType?.Facebook, agentAssist: false, calledPersonDetails: getPersonalData[0] }, data.sessionId);
    });
    if (facebookOut[0].errcode === 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.Facebook_send, FacebookSendResponse: messageList });
    } else {
      res.status(200).send({ statusCode: 200, message: RESPONSE.not_found });
    }
    res.status(200).send({ statusCode: 200, message: RESPONSE.Facebook_send, FacebookSendResponse: messageList });
  } catch (err) {
    logger.error('sendCaasFacebookHandler err', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
};

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function updateDispositionFacebookHandler(
  req: any,
  res: any,
  done: any
) {
  try {
    const auth: any = req.headers;
    const params = req?.params;
    const data: any = {
      FacebookHistoryId: params?.FacebookHistoryId,
      domainId: req?.headers?.domainId,
      dispositionId: req?.body?.dispositionId ?? 0,
      summary: req?.body?.summary ?? null,
      isSessionClosed: req?.body?.isSessionClosed ?? 0,
      afterFacebookWorkTime: req?.body?.afterWorkTime ?? null,
      dispositionUpdatedBy: req?.body?.dispositionUpdatedBy ?? null,
      duration: req?.body?.duration ?? null,
      dispositionSubject: req?.body?.dispositionSubject ?? null,
      tags: req?.body?.tags ?? null,
    };
    logger.info('updateDispositionFacebookHandler - Request Data:', data);
    const updateDispStatus: any = await updateDispositionFacebook(data);
    logger.info('updateDispStatus', updateDispStatus);
    if (data?.isSessionClosed === 1) {
      const getSuperVisorExt: any = await getSupervisiorDetails(data?.domainId);
      getSuperVisorExt.forEach((ext: any) => {
        console.log(emiiter?.interActionClosed, `${data?.domainId}_${ext.ext}`);
        ioredisSocialMedia.to(`${data?.domainId}_${ext.ext}`).emit(emiiter?.interActionClosed);
      });
    }
    const nlpData: any = {
      HistoryId: data?.FacebookHistoryId,
      ChatDurationHistoryId: data?.ChatDurationHistoryId ?? 0,
      channelType: channelType?.Facebook,
      domainId: data?.domainId,
      ext: data?.ext ?? 0
    };
    const updateScore: any = await sentimentalScore(nlpData);
    if (updateScore?.status === 200) {
      await updateSentimentScore(data, updateScore?.data);
      if (updateScore?.data) {
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
      const customerSatisfactionResult: any = await getCustomerSatisficationDao(data);
      await sendCustomerSatificationFb(customerSatisfactionResult[0]);
    }
  } catch (err) {
    logger.error('updateDispositionFacebookHandler err', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function updateDispositionFacebookCallbackHandler(
  req: any,
  res: any,
  done: any
) {
  try {
    const auth: any = req.headers;
    const params = req?.params;
    const data: any = {
      FacebookHistoryId: params?.FacebookHistoryId,
      domainId: req?.body?.domainId,
      dispositionId: req?.body?.dispositionId ?? 0,
      summary: req?.body?.summary ?? null,
      isSessionClosed: req?.body?.isSessionClosed ?? 0,
      afterFacebookWorkTime: req?.body?.afterWorkTime ?? null,
      dispositionUpdatedBy: req?.body?.dispositionUpdatedBy ?? null,
      duration: req?.body?.duration ?? null,
      dispositionSubject: req?.body?.dispositionSubject ?? null,
    };
    const updateDispStatus: any = await updateDispositionFacebook(data);
    logger.info('updateDispStatus', updateDispStatus);
    if (data?.isSessionClosed === 1) {
      const getSuperVisorExt: any = await getSupervisiorDetails(data?.domainId);
      getSuperVisorExt.forEach((ext: any) => {
        console.log(emiiter?.interActionClosed, `${data?.domainId}_${ext.ext}`);
        ioredisSocialMedia.to(`${data?.domainId}_${ext.ext}`).emit(emiiter?.interActionClosed);
      });
    }
    const nlpData: any = {
      HistoryId: data?.FacebookHistoryId,
      ChatDurationHistoryId: data?.ChatDurationHistoryId ?? 0,
      channelType: channelType?.Facebook,
      domainId: data?.domainId,
      ext: data?.ext ?? 0
    };
    const updateScore: any = await sentimentalScore(nlpData);
    if (updateScore.status === 200) {
      await updateSentimentScore(data, updateScore.data);
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
    logger.error('updateDispositionFacebookCallbackHandler err', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function assignCallBackFacebookHandlers(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      assignedBy: req?.body?.assignedBy ?? null,
      assignedTo: req?.body?.assignedTo ?? null,
      assignedOn: Date.now(),
      FacebookHistoryId: req?.body?.FacebookHistoryId,
      Istransferred: req?.body?.Istransferred ?? null,
      channelType: req?.body?.ChannelType ?? channelType?.Facebook,
      calledPersonDetails: req?.body?.calledPersonDetails ?? null,
      ext: req?.body?.ext
    };
    const assignRes: any = await assignCallBackFacebook(data);
    logger.info('assignRes', assignRes);
    try {
      await sendTransferAgent(assignRes[0]);
      console.log(emiiter?.sendInvite, `${assignRes[0]?.domainId}_${data?.ext}`);
      ioredisSocialMedia.to(`${assignRes[0]?.domainId}_${data?.ext}`).emit(emiiter?.sendInvite, {
        channelType: data?.channelType, customerNumber: assignRes[0]?.customerFacebookId,
        agentNumber: assignRes[0]?.agentFacebookId, sessionId: assignRes[0]?.sessionId, historyId: assignRes[0]?.FacebookHistoryId,
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
    logger.error('assignCallBackFacebookHandlers err', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function insertUpdateFacebookConfigHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      facebookConfigurationId: req?.body?.facebookConfigurationId ?? null,
      pageName: req?.body?.pageName ?? null,
      language: req?.body?.language ?? null,
      domainId: auth.domainId,
      planId: req?.body?.planId ?? null,
      isBot: req?.body?.isBot ?? 0,
      isBotPurchased: req?.body?.isBotPurchased ?? null,
      faceBookFlowId: req?.body?.faceBookFlowId ?? null,
      faceBookPageId: req?.body?.faceBookPageId ?? null,
      isActive: req?.body?.isActive ?? null,
      facebookintegrationId: req?.body?.facebookintegrationId ?? 1,
      callBackToggle: req?.body?.callBackToggle ?? null,
      callBackForm: req?.body?.callBackForm ?? null,
      notifyAdmin: req?.body?.notifyAdmin ?? null,
      KnowledgeBaseId: req?.body?.KnowledgeBaseId ?? null
    };
    const insertFacebook: any = await insertUpdateFacebookConfig(data);
    logger.info('insertFacebook', insertFacebook);
    if (insertFacebook[0]?.errCode === 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.insertFacebook });
    } else {
      res.status(200).send({ statusCode: 200, message: RESPONSE.updateFacebook });
    }
  } catch (err) {
    logger.error('insertUpdateFacebookConfigHandler err', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 */
export const getFacebookConfigHandler = async (req: any, res: any) => {
  try {
    const data: any = {
      domainId: req?.headers?.domainId
    };
    const getFacebookData: any = await getFacebookConfig(data);
    logger.info('getFacebookData', getFacebookData);
    if (getFacebookData && getFacebookData.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getFacebookDataRes: getFacebookData });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getWhatsappDataRes: [] });
    }
  } catch (err) {
    logger.error('getFacebookConfigHandler err', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
};

/**
 *
 * @param req
 * @param res
 */
export const getFacebookCallbackConfigHandler = async (req: any, res: any) => {
  try {
    const data: any = {
      domainId: req?.params?.domainId
    };
    const getFacebookData: any = await getFacebookConfig(data);
    logger.info('getFacebookData', getFacebookData);
    if (getFacebookData && getFacebookData.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getFacebookDataRes: getFacebookData });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getWhatsappDataRes: [] });
    }
  } catch (err) {
    logger.error('getFacebookCallbackConfigHandler err', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
};

/**
 *
 * @param req
 * @param res
 */
export const getFacebookConfigDetailsHandler = async (req: any, res: any) => {
  try {
    const data: any = {
      domainId: req?.headers?.domainId,
      fbId: req?.params?.fbId ?? 0
    };
    const getFacebookDetails: any = await getFacebookConfigDetails(data);
    logger.info('getFacebookDetails', getFacebookDetails);
    if (getFacebookDetails && getFacebookDetails.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getFacebookDetailsRes: getFacebookDetails });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getFacebookDetailsRes: [] });
    }
  } catch (err) {
    logger.error('getFacebookConfigDetailsHandler err', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
};

/**
 * Delete email
 * @param req
 * @param res
 * @param done
 */
export const deleteFacebookConfigDetailsHandler = async (req: any, res: any, done: any) => {
  try {
    const auth: any = req.headers;
    const data: any = {
      fbConfigId: req.params.fbConfigId,
      domainId: auth.domainId
    };
    const deleteFacebook = await deleteFacebookConfigDetails(data);
    logger.info('deleteFacebook', deleteFacebook);
    if (deleteFacebook[0]?.errcode === 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.facebookConfigId });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.facebookNot });
    }
  } catch (err) {
    logger.error('deleteFacebookConfigDetailsHandler err', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
};

/**
 * Delete email
 * @param req
 * @param res
 * @param done
 */
export const updateStatusFacebookHandler = async (req: any, res: any, done: any) => {
  try {
    const auth: any = req.headers;
    const data: any = {
      fbConfigId: req?.body?.fbConfigId,
      domainId: auth.domainId,
      isActive: req?.body?.isActive
    };
    const statusFacebook = await updateStatusFacebook(data);
    logger.info('statusFacebook', statusFacebook);
    if (statusFacebook[0]?.Enable === 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.status });
    } else {
      res.status(200).send({ statusCode: 200, message: RESPONSE.statusDisabled });
    }
  } catch (err) {
    logger.error('updateStatusFacebookHandler err', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
};

/**
 * update page id
 * @param req
 * @param res
 * @param done
 */
export const updatePageIdFacebookHandler = async (req: any, res: any, done: any) => {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: req?.body?.domainId,
      facebookPageId: req?.body?.facebookPageId,
      pageName: req?.body?.pageName
    };
    const facebookPage = await updatePageIdFacebook(data);
    logger.info('facebookPage', facebookPage);
    if (facebookPage[0]?.errcode === 1) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.facebookPage });
    } else {
      res.status(200).send({ statusCode: 200, message: RESPONSE.facebookNotUpdate });
    }
  } catch (err) {
    logger.error('updatePageIdFacebookHandler err', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
};

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function insertFacebookCallbackHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      domainId: req.body?.domainId,
      sessionId: req.body?.sessionId,
      IsInBound: req.body?.IsInBound,
      customerFacebookId: req.body?.customerFacebookId ?? null,
      agentFacebookId: req.body?.agentFacebookId ?? null,
      FacebookHistoryId: req?.body?.FacebookHistoryId ?? null
    };
    const insertFacebookCallback: any = await insertWhatsappCallbackDao(data);
    logger.info('insertFacebookCallback', insertFacebookCallback);
    if (insertFacebookCallback && insertFacebookCallback?.length > 0) {
      const getSupervisiorExt: any = await getSupervisiorList(data);
      getSupervisiorExt.forEach((ex: any) => {
        console.log(emiiter?.supervisiorActivity, `${data?.domainId}_${ex.ext}`);
        ioredisSocialMedia.to(`${data?.domainId}_${ex.ext}`).emit(emiiter?.supervisiorActivity);
      });
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getFacebookCallBackResponse: insertFacebookCallback });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getFacebookCallBackResponse: [] });
    }
  } catch (err) {
    logger.error('insertFacebookCallbackHandler err', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function sendAppoinmentPaymentHandler(req: any, res: any, done: any) {
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
    const getPayment: any = await sendAppoinmentPayment(data);
    logger.info('getPayment', getPayment);
    if (getPayment) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.faild });
    }
  } catch (err) {
    logger.error('sendAppoinmentPaymentHandler err', err);
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
export async function insertRoleFacebookInHandler(req: any, facebookCcaasTable: any) {
  try {
    let currentNodeId = null;
    let nodes = null;
    let getNlpData: any = '';
    let nlpdata: any = '';
    let flow_data: any = [];
    const getFacebookDetails = await getFacebookConfigData(facebookCcaasTable[0]);
    if (getFacebookDetails[0]?.FacebookFlowId) {
      const nodesString: any = await getnodesdata(getFacebookDetails[0]?.FacebookFlowId);
      nodes = JSON.parse(nodesString);
      const startingNode = await getStartNode(nodes);
      currentNodeId = startingNode?.data?.children[0]?.childId ?? '';
    }
    // sendTypingIndicator(facebookCcaasTable[0]);
    const getNode = await getNextNode(facebookCcaasTable[0]);
    const nodeDetails = await getNodeDataById(getNode[0]?.nextFlowNode, nodes);
    const parsedData = safeParseJson(nodeDetails?.data?.information?.data);
    const inputType: any = nodeDetails?.data?.information?.inputType;
    if (inputType === 14) {
      flow_data = parsedData?.getInputData?.options?.map((item: any) => item?.value) || [];
    } else if (inputType === 15 || inputType === 1) {
      flow_data = parsedData?.options?.map((item: any) => item?.value) || [];
    }
    const comDetails = await getCompanyDetails(facebookCcaasTable[0]);
    logger.info('comDetails', comDetails);
    if (!['file', 'document', 'image', 'video', 'audio'].includes(req?.body?.type) && req?.body?.type !== "quickReply" && req?.body?.type !== "facebookPostbackButton" && req?.body?.payload !== '100' && req?.body?.payload !== '200' && req?.body?.payload !== '10001' && req?.body?.payload !== '20002') {
      getNlpData = await nlpResponseData(req?.body, comDetails, facebookCcaasTable[0], getFacebookDetails[0], flow_data, inputType);
      nlpdata = JSON.parse(getNlpData?.data?.entity);
    }
    if (getNlpData?.data?.intent === 'transfer_live_agent') {
      try {
        const liveAgent = await transferChatToAvailableAgent(facebookCcaasTable[0]);
        const getCoid = await getCustomerCoId(facebookCcaasTable[0]);
        await connectToliveAgent(liveAgent, facebookCcaasTable, getFacebookDetails, getCoid);
        return;
      } catch (err) {
        logger.error("insertRoleFacebookInHandler err", err)
      }
    }
    const currentNode_ref = await getNodeDataById(getNode[0].nextFlowNode, nodes);
    const getBotMessage: any = await getBotMessageRes(facebookCcaasTable[0]);
    const getRemainder = await getRemaindeFbResponse(facebookCcaasTable[0]);
    if (req?.body?.text?.body === 'Yes' && req?.body?.payload === '100') {         // Yes
      await sendMessageCustomer(facebookCcaasTable[0], getBotMessage[0]?.message);
      sendMessageCustomerCaiMessage(facebookCcaasTable, getBotMessage[0]?.message);
    } else if (req?.body?.text?.body === 'No' && req?.body?.payload === '200') {   // No
      await updateNextNode(facebookCcaasTable[0], getNode[0]?.nextFlowNode);
      return handleChatFlowType(null, facebookCcaasTable, nodes, '', req);
    } else if (req?.body?.text?.body === 'Yes' && req?.body?.payload === '10001') { // Remainder Yes
      await updateNextNode(facebookCcaasTable[0], getNode[0]?.nextFlowNode);
      return handleChatFlowType(null, facebookCcaasTable, nodes, '', req);
    } else if (req?.body?.text?.body === 'No' && req?.body?.payload === '20002') { // // Remainder No
      const parsedMessages = JSON.parse(getRemainder[0].message)[4]?.message;
      await sendMessageCustomer(facebookCcaasTable[0], parsedMessages);
      sendMessageCustomerCaiMessage(facebookCcaasTable, parsedMessages);
    } else if (currentNode_ref?.data?.information?.inputType === 16 && ['file', 'document', 'image', 'video', 'audio'].includes(req?.body.type)) {
      await uploadFilesFlow(req?.body, facebookCcaasTable, currentNode_ref, nodes, getNlpData, req);
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
      await updateNextNode(facebookCcaasTable[0], nextData?.childId);
      handleChatFlowType(null, facebookCcaasTable, nodes, '', req);
    } else if (req?.body?.type === 'facebookCarousel') {
      const currentNode_FB = await getNodeDataById(req?.body?.payload, nodes);
      const nextData = currentNode_FB?.data?.children[0];
      await updateNextNode(facebookCcaasTable[0], nextData?.childId);
      return handleChatFlowType(null, facebookCcaasTable, nodes, '', req);
    } else if (req?.body?.type === "quickReply" && getNode[0]?.nextFlowNode?.split('_')[0] !== 'customInput') {
      const getAppointmentFlow: any = await getFlowTracking(facebookCcaasTable[0]);
      const getAppDetails = await AppointmentInfo(getAppointmentFlow);
      const DurationType = await getAppointmentInfo(getAppointmentFlow?.appId, getAppointmentFlow?.companyId);
      const lable = await getAppointmentLable(getAppointmentFlow?.companyId);
      const ServiceName = await getAppointmentInfo(getAppointmentFlow?.appId, getAppointmentFlow?.companyId);
      if (getAppointmentFlow?.processflow === 'categories') {
        await updateCategoryId(facebookCcaasTable[0], getAppointmentFlow, req.body.text.body);
        const getcategories: any = await getFlowTracking(facebookCcaasTable[0]);
        const matchedCategory = ServiceName
          .filter((item: any) => item.categoryId === +(getcategories?.categoryId))
          .map((item: any) => ({
            serviceid: item.serviceid,
            service_name: item.service_name
          }));
        logger.info("Select service", matchedCategory);
        await sendAppointmentRes(facebookCcaasTable[0], matchedCategory, lable);
        let datas: any = {
          appointmentId: getAppointmentFlow?.appId,
          companyId: getAppointmentFlow?.companyId,
          userId: getAppointmentFlow?.userId
        }
        if (getAppDetails[0]?.consl_selection_id === 2) {
          await insertFlowTracking(facebookCcaasTable[0], "consultant", getAppointmentFlow);
        } else {
          await insertFlowTracking(facebookCcaasTable[0], "service", datas);
        }
      } else if (getAppointmentFlow?.processflow === 'service') {
        await updateAppointmentServiceId(facebookCcaasTable[0], getAppointmentFlow, req.body.text.body);
        const getUserData = await getFlowTracking(facebookCcaasTable[0]);
        const UserName = await getAppointmentUserName(getAppointmentFlow, getUserData);
        if (getAppDetails[0]?.consl_selection_id === 1) {
          const getUser = UserName.map((name: any) => ({ content_type: "text", title: name?.userName, payload: name?.UserId }));
          await sendAllUserName(facebookCcaasTable[0], getUser);
        } else if (getAppDetails[0]?.consl_selection_id === 2) {
          const getConsultantDatas = await getConsultantData(getAppointmentFlow, getUserData, getAppDetails[0]);
          await updateAppointmentConsultant(facebookCcaasTable[0], getAppointmentFlow, getConsultantDatas[0]);
        } else {
          const getUser = UserName.map((name: any) => ({ content_type: "text", title: name?.userName, payload: name?.UserId, }));
          getUser?.unshift({ content_type: "text", title: "Any available", payload: 1, })
          await sendAllUserName(facebookCcaasTable[0], getUser);
        }
        await insertFlowTracking(facebookCcaasTable[0], "consultant", getAppointmentFlow);
      } else if (getAppointmentFlow?.processflow === 'consultant') {
        if (getAppDetails[0]?.consl_selection_id === 2) {
          await updateAppointmentServiceId(facebookCcaasTable[0], getAppointmentFlow, req.body.text.body);
        }
        const getUserDatas = await getFlowTracking(facebookCcaasTable[0]);
        const getLeaveDates = await getLeaveData(getUserDatas);
        const leaveDatas = getLeaveDates.filter((_l: any) => _l.is_leave === 1);
        const getConsultant = await getConsultantData(getAppointmentFlow, getUserDatas, getAppDetails[0]);
        await updateAppointmentConsultant(facebookCcaasTable[0], getAppointmentFlow, getConsultant[0]);
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
        await sendAppointmentDateFb(facebookCcaasTable[0], selectDate);
        await insertFlowTracking(facebookCcaasTable[0], "date", getAppointmentFlow);
      } else if (getAppointmentFlow?.processflow === "date") {
        if (req?.body?.text?.title === 'More') {
          const getUserDatas = await getFlowTracking(facebookCcaasTable[0]);
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
          await sendAppointmentDateFb(facebookCcaasTable[0], selectDate);
          return;
        }
        await updateAppointmentdate(facebookCcaasTable[0], getAppointmentFlow, req.body.text.title);
        if (getAppDetails[0]?.app_typeid === 1 || getAppDetails[0]?.app_typeid === 2 || getAppDetails[0]?.app_typeid === 3) {
          const durationData = JSON.parse(getAppDetails[0]?.serv_dur_pricing);
          const getDurationRes = durationData.map((d: any, index: Number) => ({ content_type: "text", title: +(d.scheduleHours) === 1 ? `${+(d.service)} mins` : `${+(d.service / 60)} hours`, payload: index }));
          await sendAppointmentOneToOne(facebookCcaasTable[0], getDurationRes, lable);
        } else {
          const durationDetails = await getdurationDetails(getAppointmentFlow);
          const duration = durationDetails.filter((data: any) => data?.serviceid === +(getAppointmentFlow?.serviceId));
          const serviceIndex = duration.findIndex((item: any) => item.serviceid === +(getAppointmentFlow?.serviceId));
          const selectDuration = duration[0].amount_collected.split("|")[serviceIndex];
          await sendAppointmentDuration(facebookCcaasTable[0], selectDuration, lable);
        }
        await insertFlowTracking(facebookCcaasTable[0], "duration", getAppointmentFlow);
      } else if (getAppointmentFlow?.processflow === "duration") {
        if (req?.body?.text.title !== "More") {
          await updateAppointmentDuration(facebookCcaasTable[0], getAppointmentFlow, req?.body?.text);
        }
        const getDurationData: any = await getFlowTracking(facebookCcaasTable[0]);
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
          await sendMessageCustomer(facebookCcaasTable[0], text);
          await insertFlowTracking(facebookCcaasTable[0], "date", getAppointmentFlow);
          return;
        }
        await sendbookingDuration(facebookCcaasTable[0], durationData, lable);
        if (req?.body?.text?.title !== 'More' && durationData?.length > 0) {
          await insertFlowTracking(facebookCcaasTable[0], "time", getAppointmentFlow);
        }
      } else if (getAppointmentFlow?.processflow === "time") {
        if (req?.body?.text?.title === 'More') {
          const getDurationData: any = await getFlowTracking(facebookCcaasTable[0]);
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
          await sendbookingDuration(facebookCcaasTable[0], durationData, lable);
          return;
        }
        await updateAppointmentTime(facebookCcaasTable[0], getAppointmentFlow, req?.body?.text);
        const contactData = await getPersonalDetails(facebookCcaasTable[0]?.customerFacebookId, facebookCcaasTable[0]?.domainId);
        const getProcessTime: any = await getFlowTracking(facebookCcaasTable[0]);
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
        const getPaymentData = await AppointmentInfoPaymentCheckFb(getAppointmentFlow);
        const FilterPayment = getPaymentData[1]?.filter((data: any) => data?.serviceid === +(getProcessTime?.serviceId));
        const pricingData = getPaymentData[0][0]?.app_typeid === 4 ? FilterPayment[0]?.is_enable_price === 1 : getAppDetails[0]?.is_enable_pricing === 1;
        if (pricingData) {
          await bookingPayment(inputData, getProcessTime, getAppDetails, endTimeData?.endTime, typeValue, facebookCcaasTable[0], FilterPayment[0]);
        } else {
          const getBooking = await createBooking(inputData, getProcessTime, getAppDetails, endTimeData?.endTime, typeValue);
          logger.info("Appointment Booking successfully", getBooking);
          if (getBooking[0]?.errcode === 0) {
            if (getAppDetails[0]?.app_typeid === 1 || getAppDetails[0]?.app_typeid === 2 || getAppDetails[0]?.app_typeid === 3) {
              await sendBookingDetailsOneToOne(facebookCcaasTable[0], getBooking[0]);
            } else {
              await sendBookingDetails(facebookCcaasTable[0], getBooking[0], getProcessTime);
            }
          } else {
            const getDurationData: any = await getFlowTracking(facebookCcaasTable[0]);
            const calenderView: any = await getCalenderViewDetails(getDurationData, getAppDetails, DurationType, getAppointmentFlow);
            const getDuration = calenderView.slice(0, 10).map((duration: any, index: number) => ({
              content_type: "text",
              title: duration?.start,
              payload: index
            }));
            await sendbookingDuration(facebookCcaasTable[0], getDuration, lable);
            await insertFlowTracking(facebookCcaasTable[0], "time", getAppointmentFlow);
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
      }
      if (matches || trigger?.length === 0) {
        const startingNode = await getStartNode(nodes);
        const currentNodeId = startingNode?.data?.children[0]?.childId ?? '';
        await updateNextNode(facebookCcaasTable[0], currentNodeId);
        return handleChatFlowType(currentNodeId, facebookCcaasTable, nodes, req?.body?.text?.body, req);
      } else if (getNlpData?.data?.intent === 'greetings') {
        await sendMessageCustomer(facebookCcaasTable[0], getNlpData?.data?.message);
        sendMessageCustomerCaiMessage(facebookCcaasTable, getNlpData?.data?.message);
      }
      if (isArry?.length === 0 && getNlpData?.data?.flow_key === 'query' && currentNode_ref?.data?.children.length !== 0) {
        const getBotMessage: any = await getBotMessageRes(facebookCcaasTable[0]);
        const getRemainder = await getRemaindeFbResponse(facebookCcaasTable[0]);
        const remainderMessage = getRemainder[0]?.message;
        const jsonData = remainderMessage ? JSON.parse(remainderMessage)?.length > 3 : false;
        if (jsonData && getRemainder[0]?.isReminder === 0) {
          await sendFlowBreakInpuRemaindertFb(facebookCcaasTable[0]);
          await insertBotMessage(facebookCcaasTable[0], getNlpData?.data?.message);
          await insertBotMessageIsRemainderFb(facebookCcaasTable[0]);
          sendFlowBreakMessageRemainderFb(facebookCcaasTable);
        } else {
          if (getBotMessage?.length > 0) {
            await sendMessageCustomer(facebookCcaasTable[0], getNlpData?.data);
            if (getNode[0]?.nextFlowNode !== null) {
              await insertBotMessageRemainderfacebook(facebookCcaasTable[0], getNlpData?.data?.message);
            }
            sendMessageCustomerCaiMessage(facebookCcaasTable, getNlpData?.data?.message);
          } else {
            if (getNode[0].nextFlowNode !== null) {
              await sendFlowBreakInput(facebookCcaasTable[0]);
              await insertBotMessage(facebookCcaasTable[0], getNlpData?.data?.message);
              sendFlowBreakMessage(facebookCcaasTable);
              await insertBotMessageRemainderfacebook(facebookCcaasTable[0], getNlpData?.data?.message);
            } else {
              await sendMessageCustomer(facebookCcaasTable[0], getNlpData?.data?.message);
              sendMessageCustomerCaiMessage(facebookCcaasTable, getNlpData?.data?.message);
            }
          }
        }
        return;
      } else if (isArry?.length > 0) {
        const FlowData: any = await getQuestionsAns(facebookCcaasTable[0], getFacebookDetails[0], isArry[0]);
        await updateNextNode(facebookCcaasTable[0], FlowData[0]?.mapFlow);
        return handleChatFlowType(null, facebookCcaasTable, nodes, req?.body?.text?.body, req);
      } else {
        await sendMessageCustomer(facebookCcaasTable[0], getNlpData?.data?.message);
        sendMessageCustomerCaiMessage(facebookCcaasTable, getNlpData?.data?.message);
      }
    } else {
      const triggerData = nodes[0]?.data?.information?.trigger_data;
      const trigger = typeof triggerData === 'string' ? JSON.parse(triggerData) : [];
      await triggerDataHandler(trigger, req, nodes, facebookCcaasTable, getNlpData);
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
        await updateNextNode(facebookCcaasTable[0], checkData?.id);
        return handleChatFlowType(null, facebookCcaasTable, nodes, req?.body?.text?.body, req);
      }
      const currentNodeData = await getNodeDataById(getNode[0].nextFlowNode, nodes);
      const json = isJson(getNlpData?.data?.intent) ? JSON.parse(getNlpData?.data?.intent) : getNlpData?.data?.intent;
      const isArry = Array.isArray(json) ? json : [];
      if (isArry?.length === 0 && getNlpData?.data?.flow_key === 'query' && currentNodeData?.data?.children.length !== 0) {
        const getBotMessage: any = await getBotMessageRes(facebookCcaasTable[0]);
        const getRemainder = await getRemaindeFbResponse(facebookCcaasTable[0]);
        const remainderMessage = getRemainder[0]?.message;
        const jsonData = remainderMessage ? JSON.parse(remainderMessage)?.length > 3 : false;
        if (jsonData && getRemainder[0]?.isReminder === 0) {
          await sendFlowBreakInpuRemaindertFb(facebookCcaasTable[0]);
          await insertBotMessage(facebookCcaasTable[0], getNlpData?.data?.message);
          await insertBotMessageRemainderfacebook(facebookCcaasTable[0], getNlpData?.data?.message);
          await insertBotMessageIsRemainderFb(facebookCcaasTable[0]);
          sendFlowBreakMessageRemainderFb(facebookCcaasTable);
        } else {
          if (getBotMessage?.length > 0) {
            await sendMessageCustomer(facebookCcaasTable[0], getNlpData?.data);
            if (getNode[0]?.nextFlowNode !== null) {
              await insertBotMessageRemainderfacebook(facebookCcaasTable[0], getNlpData?.data?.message);
            }
            sendMessageCustomerCaiMessage(facebookCcaasTable, getNlpData?.data?.message);
          } else {
            if (getNode[0].nextFlowNode !== null) {
              await sendFlowBreakInput(facebookCcaasTable[0]);
              await insertBotMessage(facebookCcaasTable[0], getNlpData?.data?.message);
              sendFlowBreakMessage(facebookCcaasTable);
              await insertBotMessageRemainderfacebook(facebookCcaasTable[0], getNlpData?.data?.message);
            } else {
              await sendMessageCustomer(facebookCcaasTable[0], getNlpData?.data?.message);
              sendMessageCustomerCaiMessage(facebookCcaasTable, getNlpData?.data?.message);
            }
          }
        }
        return;
      } else if (isArry?.length > 0) {
        const FlowData: any = await getQuestionsAns(facebookCcaasTable[0], getFacebookDetails[0], isArry[0]);
        await updateNextNode(facebookCcaasTable[0], FlowData[0]?.mapFlow);
        return handleChatFlowType(null, facebookCcaasTable, nodes, req?.body?.text?.body, req);
      }
      const currentNode_ref = await getNodeDataById(getNode[0].nextFlowNode, nodes);
      if (getNode[0].nextFlowNode?.split('_')[0] === 'customInput' && currentNode_ref?.data?.information?.inputType === 5) {
        const saveContactPhoneNumber = JSON.parse(currentNode_ref?.data?.information?.data);
        const getNoOfRepeats: any = await getNoOfRepeatsRes(facebookCcaasTable[0]);
        if (getNoOfRepeats?.noOfRepeats === 0 || getNoOfRepeats?.noOfRepeats === null) {
          await updateNoOfRepeats(facebookCcaasTable[0], parseInt(saveContactPhoneNumber?.getInputData?.noOfRepeat) + 1);
        }
        const isValid = nlpdata?.mobile_number?.length > 0;
        if (isValid) {
          if (saveContactPhoneNumber?.getInputData?.answerSave) {
            try {
              const body = {
                phone: req?.body?.text?.body?.replace(/\D/g, ''),
                domainId: facebookCcaasTable?.[0]?.domainId,
                customerFacebookId: facebookCcaasTable?.[0]?.customerFacebookId
              }
              insertUpdateCrmContact(body); // crm insert
            } catch (error) {
              logger.error('insertUpdateCrmContact ::: error:', error);
            }
            await updateCustomerContactFB(req.body, facebookCcaasTable[0], req?.body?.text?.body);
            await updatePhNumber(facebookCcaasTable[0], req?.body?.text);
            const jsonData = {
              key: saveContactPhoneNumber?.getInputData?.contactSave,
              value: req?.body?.text?.body
            };
            const jsonDataList = JSON.stringify(jsonData);
            await updateConnectData(jsonDataList, facebookCcaasTable[0], getFacebookDetails);
          }
          await updateNoOfRepeats(facebookCcaasTable[0], 0);
        } else {
          const getNoOfRepeat: any = await getNoOfRepeatsRes(facebookCcaasTable[0]);
          if (getNoOfRepeat?.noOfRepeats !== 0) {
            await updateNoOfRepeats(facebookCcaasTable[0], getNoOfRepeat?.noOfRepeats - 1);
          }
          if (getNoOfRepeat?.noOfRepeats - 1 !== 0) {
            const errorMessage = saveContactPhoneNumber?.getInputData?.errorMessage === '' ? messageValue?.customerInputError : saveContactPhoneNumber?.getInputData?.errorMessage;
            sendFlowMessageData(facebookCcaasTable, errorMessage);
            await sendMessageCustomer(facebookCcaasTable[0], errorMessage);
            return;
          } else {
            const nextData = currentNode_ref?.data?.children[1];
            await updateNextNode(facebookCcaasTable[0], nextData?.childId);
            return handleChatFlowType(null, facebookCcaasTable, nodes, req?.body?.text?.body, req);
          }
        }
        const nextData = currentNode_ref?.data?.children[0];
        await updateNextNode(facebookCcaasTable[0], nextData?.childId);
        return handleChatFlowType(null, facebookCcaasTable, nodes, req?.body?.text?.body, req);
      } else if (getNode[0].nextFlowNode?.split('_')[0] === 'customInput' && currentNode_ref?.data?.information?.inputType === 6) {
        const saveContactEmail = JSON.parse(currentNode_ref?.data?.information?.data);
        const getNoOfRepeats: any = await getNoOfRepeatsRes(facebookCcaasTable[0]);
        if (getNoOfRepeats?.noOfRepeats === 0 || getNoOfRepeats?.noOfRepeats === null) {
          await updateNoOfRepeats(facebookCcaasTable[0], parseInt(saveContactEmail?.getInputData?.noOfRepeat) + 1);
        }
        const dynamicPattern = req?.body?.text?.body;
        const checkEmail = nlpdata?.email?.length > 0;
        if (checkEmail) {
          if (saveContactEmail?.getInputData?.answerSave) {
            await insertCustomerContactEmail(req.body, facebookCcaasTable[0], dynamicPattern);
            await updateEmailIdFB(facebookCcaasTable[0], req?.body?.text);
            const jsonData = {
              key: saveContactEmail?.getInputData?.contactSave,
              value: req?.body?.text?.body
            };
            const jsonDataList = JSON.stringify(jsonData);
            await updateConnectData(jsonDataList, facebookCcaasTable[0], getFacebookDetails);
          }
          await updateNoOfRepeats(facebookCcaasTable[0], 0);

          try {
            const body = {
              email: req?.body?.text?.body,
              domainId: facebookCcaasTable?.[0]?.domainId,
              customerFacebookId: facebookCcaasTable?.[0]?.customerFacebookId
            }
            insertUpdateCrmContact(body); // crm insert
          } catch (error) {
            logger.error('insertUpdateCrmContact ::: error:', error);
          }

        } else {
          const getNoOfRepeat: any = await getNoOfRepeatsRes(facebookCcaasTable[0]);
          if (getNoOfRepeat?.noOfRepeats !== 0) {
            await updateNoOfRepeats(facebookCcaasTable[0], getNoOfRepeat?.noOfRepeats - 1);
          }
          if (getNoOfRepeat?.noOfRepeats - 1 !== 0) {
            const errorMessage = saveContactEmail?.getInputData?.errorMessage === '' ? messageValue?.customerInputError : saveContactEmail?.getInputData?.errorMessage;
            sendFlowMessageData(facebookCcaasTable, errorMessage);
            await sendMessageCustomer(facebookCcaasTable[0], errorMessage);
            return;
          } else {
            const nextData = currentNode_ref?.data?.children[1];
            await updateNextNode(facebookCcaasTable[0], nextData?.childId);
            return handleChatFlowType(null, facebookCcaasTable, nodes, req?.body?.text?.body, req);
          }
        }
        const nextData = currentNode_ref?.data?.children[0];
        await updateNextNode(facebookCcaasTable[0], nextData?.childId);
        return handleChatFlowType(null, facebookCcaasTable, nodes, req?.body?.text?.body, req);
      } else if (getNode[0].nextFlowNode?.split('_')[0] === 'customInput' && currentNode_ref?.data?.information?.inputType === 2) {
        const saveText = JSON.parse(currentNode_ref?.data?.information?.data);
        if (saveText?.getInputData?.answerSave) {
          const jsonData = {
            key: saveText?.getInputData?.contactSave,
            value: req?.body?.text?.body
          };
          const jsonDataList = JSON.stringify(jsonData);
          await updateConnectData(jsonDataList, facebookCcaasTable[0], getFacebookDetails);
        }
        const nextData = currentNode_ref?.data?.children[0];
        await updateNextNode(facebookCcaasTable[0], nextData?.childId);
        return handleChatFlowType(null, facebookCcaasTable, nodes, req?.body?.text?.body, req);
      } else if (getNode[0].nextFlowNode?.split('_')[0] === 'customInput' && currentNode_ref?.data?.information?.inputType === 7) {
        const saveNumber = JSON.parse(currentNode_ref?.data?.information?.data);
        const getNoOfRepeats: any = await getNoOfRepeatsRes(facebookCcaasTable[0]);
        if (getNoOfRepeats?.noOfRepeats === 0 || getNoOfRepeats?.noOfRepeats === null) {
          await updateNoOfRepeats(facebookCcaasTable[0], parseInt(saveNumber?.getInputData?.noOfRepeat) + 1);
        }
        const isNumber = nlpdata?.count?.length > 0;
        if (isNumber) {
          if (saveNumber?.getInputData?.answerSave) {
            const jsonData = {
              key: saveNumber?.getInputData?.contactSave,
              value: req?.body?.text?.body
            };
            const jsonDataList = JSON.stringify(jsonData);
            await updateConnectData(jsonDataList, facebookCcaasTable[0], getFacebookDetails);
          }
          await updateNoOfRepeats(facebookCcaasTable[0], 0);
        } else {
          const getNoOfRepeat: any = await getNoOfRepeatsRes(facebookCcaasTable[0]);
          if (getNoOfRepeat?.noOfRepeats !== 0) {
            await updateNoOfRepeats(facebookCcaasTable[0], getNoOfRepeat?.noOfRepeats - 1);
          }
          if (getNoOfRepeat?.noOfRepeats - 1 !== 0) {
            const errorMessage = saveNumber?.getInputData?.errorMessage === '' ? messageValue?.customerInputError : saveNumber?.getInputData?.errorMessage;
            sendFlowMessageData(facebookCcaasTable, errorMessage);
            await sendMessageCustomer(facebookCcaasTable[0], errorMessage);
            return;
          } else {
            const nextData = currentNode_ref?.data?.children[1];
            await updateNextNode(facebookCcaasTable[0], nextData?.childId);
            return handleChatFlowType(null, facebookCcaasTable, nodes, req?.body?.text?.body, req);
          }
        }
        const nextData = currentNode_ref?.data?.children[0];
        await updateNextNode(facebookCcaasTable[0], nextData?.childId);
        return handleChatFlowType(null, facebookCcaasTable, nodes, req?.body?.text?.body, req);
      } else if (getNode[0].nextFlowNode?.split('_')[0] === 'customInput' && currentNode_ref?.data?.information?.inputType === 8) {
        const saveUrl = JSON.parse(currentNode_ref?.data?.information?.data);
        const getNoOfRepeats: any = await getNoOfRepeatsRes(facebookCcaasTable[0]);
        if (getNoOfRepeats?.noOfRepeats === 0 || getNoOfRepeats?.noOfRepeats === null) {
          await updateNoOfRepeats(facebookCcaasTable[0], parseInt(saveUrl?.getInputData?.noOfRepeat) + 1);
        }
        const isUrl = nlpdata?.website?.length > 0;
        if (isUrl) {
          if (saveUrl?.getInputData?.answerSave) {
            const jsonData = {
              key: saveUrl?.getInputData?.contactSave,
              value: req?.body?.text?.body
            };
            const jsonDataList = JSON.stringify(jsonData);
            await updateConnectData(jsonDataList, facebookCcaasTable[0], getFacebookDetails);
          }
          await updateNoOfRepeats(getFacebookDetails[0], 0);
        } else {
          const getNoOfRepeat: any = await getNoOfRepeatsRes(facebookCcaasTable[0]);
          if (getNoOfRepeat?.noOfRepeats !== 0) {
            await updateNoOfRepeats(facebookCcaasTable[0], getNoOfRepeat?.noOfRepeats - 1);
          }
          if (getNoOfRepeat?.noOfRepeats - 1 !== 0) {
            const errorMessage = saveUrl?.getInputData?.errorMessage === '' ? messageValue?.customerInputUrl : saveUrl?.getInputData?.errorMessage;
            sendFlowMessageData(facebookCcaasTable, errorMessage);
            await sendMessageCustomer(facebookCcaasTable[0], errorMessage);
          } else {
            const nextData = currentNode_ref?.data?.children[1];
            await updateNextNode(facebookCcaasTable[0], nextData?.childId);
            return handleChatFlowType(null, facebookCcaasTable, nodes, req?.body?.text?.body, req);
          }
        }
        const nextData = currentNode_ref?.data?.children[0];
        await updateNextNode(facebookCcaasTable[0], nextData?.childId);
        return handleChatFlowType(null, facebookCcaasTable, nodes, req?.body?.text?.body, req);
      } else if (getNode[0].nextFlowNode?.split('_')[0] === 'customInput' && currentNode_ref?.data?.information?.inputType === 9) {
        const saveContactName = JSON.parse(currentNode_ref?.data?.information?.data);
        const getNoOfRepeats: any = await getNoOfRepeatsRes(facebookCcaasTable[0]);
        if (getNoOfRepeats?.noOfRepeats === 0 || getNoOfRepeats?.noOfRepeats === null) {
          await updateNoOfRepeats(facebookCcaasTable[0], parseInt(saveContactName?.getInputData?.noOfRepeat) + 1);
        }
        const isName = (nlpdata?.first_name?.length > 0) || (nlpdata?.last_name?.length > 0);
        if (isName) {
          if (saveContactName?.getInputData?.answerSave) {
            if (saveContactName?.getInputData?.contactSave === 'firstName') {
              await updateCustomerFirstName(req.body, facebookCcaasTable[0]);
              await updateCustomerFirstNameAppoinment(facebookCcaasTable[0], req?.body?.text);

              try {
                const body = {
                  firstName: req?.body?.text?.body,
                  domainId: facebookCcaasTable?.[0]?.domainId,
                  customerFacebookId: facebookCcaasTable?.[0]?.customerFacebookId
                }
                insertUpdateCrmContact(body); // crm insert
              } catch (error) {
                logger.error('insertUpdateCrmContact ::: error:', error);
              }

            } else if (saveContactName?.getInputData?.contactSave === 'lastName') {
              await updateCustomerLastName(req.body, facebookCcaasTable[0]);
              await updateCustomerLastNameAppoinment(facebookCcaasTable[0], req?.body?.text);

              try {
                const body = {
                  lastName: req?.body?.text?.body,
                  domainId: facebookCcaasTable?.[0]?.domainId,
                  customerFacebookId: facebookCcaasTable?.[0]?.customerFacebookId
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
            await updateConnectData(jsonDataList, facebookCcaasTable[0], getFacebookDetails);
          }
          await updateNoOfRepeats(facebookCcaasTable[0], 0);
        } else {
          const getNoOfRepeat: any = await getNoOfRepeatsRes(facebookCcaasTable[0]);
          if (getNoOfRepeat?.noOfRepeats !== 0) {
            await updateNoOfRepeats(facebookCcaasTable[0], getNoOfRepeat?.noOfRepeats - 1);
          }
          if (getNoOfRepeat?.noOfRepeats - 1 !== 0) {
            const errorMessage = saveContactName?.getInputData?.errorMessage === '' ? messageValue?.customerInputError : saveContactName?.getInputData?.errorMessage;
            sendFlowMessageData(facebookCcaasTable, errorMessage);
            await sendMessageCustomer(facebookCcaasTable[0], errorMessage);
            return;
          } else {
            const nextData = currentNode_ref?.data?.children[1];
            await updateNextNode(facebookCcaasTable[0], nextData?.childId);
            return handleChatFlowType(null, facebookCcaasTable, nodes, req?.body?.text?.body, req);
          }
        }
        const nextData = currentNode_ref?.data?.children[0];
        await updateNextNode(facebookCcaasTable[0], nextData?.childId);
        return handleChatFlowType(null, facebookCcaasTable, nodes, req?.body?.text?.body, req);
      } else if (getNode[0].nextFlowNode.split('_')[0] === 'customInput' && currentNode_ref?.data?.information?.inputType === 14) {
        const nextData = currentNode_ref?.data?.children;
        const selectedTitle = req?.body?.text?.title;
        const match = nextData.find((option: any) => option?.type === selectedTitle);
        const childId = match?.childId;
        await updateNextNode(facebookCcaasTable[0], childId);
        return handleChatFlowType(null, facebookCcaasTable, nodes, req?.body?.text?.body, req);
      } else {
        const triggerData = nodes[0]?.data?.information?.trigger_data;
        const trigger = typeof triggerData === 'string' ? JSON.parse(triggerData) : [];
        await triggerDataSendMessageHandler(trigger, req, nodes, facebookCcaasTable, getNlpData);
      }
    }
  } catch (err) {
    console.log("insertRoleFacebookInHandler err", err)
    // res.send({ statusCode: 500, message: 'internal server error' });
  }
}

async function handleChatFlowType(nodeId: any, facebookCcaasTable: any, nodes: any, input: any, req: any) {
  try {
    const getNode = await getNextNode(facebookCcaasTable[0]);
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
          await updateNextNode(facebookCcaasTable[0], childId);
          return handleChatFlowType(childId, facebookCcaasTable, nodes, input, req);
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
            temp = temp.replace("<strong>", "*").replace("</strong>", "*");
            temp = temp.replace(/<em>/g, "_").replace(/<\/em>/g, "_");
            temp = temp.replace(/<s>/g, "~").replace(/<\/s>/g, "~");
            temp = temp.replace(/<ol>/g, "\n").replace(/<\/ol>/g, "");
            temp = temp.replace(/<li>/g, () => `${counter++}. `).replace(/<\/li>/g, "\n");
          }
          const textData = temp?.replace(/<[^>]+>/g, '');
          const filesData: any = targetData[1];
          targetData.filter((row: any) => row?.type === 'text');
          await sendMessageCustomerFbFlow(facebookCcaasTable[0], textData);
          if (targetData.length > 1 && targetData[0]?.type === 'text') {
            await sendImageCustomer(facebookCcaasTable[0], targetData[1]);
          } else if (targetData.length > 1 && targetData[0]?.type !== 'text') {
            await sendImageCustomer(facebookCcaasTable[0], targetData[0]);
          }
          const nextNode = currentNode?.data?.children?.find((nodes1: any) => nodes1?.type === 'success') ?? { childId: '' };
          const nextNodedetails: any = await getNodeDataById(nextNode?.childId, nodes);
          let message: any = {};
          try {
            message = {
              messageID: uuidv4(),
              sessionId: facebookCcaasTable[0]?.sessionId,
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
            stringifiedMessage.FacebookHistoryId = facebookCcaasTable[0]?.FacebookHistoryId;
            await updateChatMessage(stringifiedMessage);
            updateIsBotFb(facebookCcaasTable[0]?.FacebookHistoryId);
            await updateNextNode(facebookCcaasTable[0], nextNode?.childId);
            if (entryCarousal?.length !== 0) {
              await sendCarouselData(facebookCcaasTable[0], entryCarousal);
            }
          } catch (error) {
            const nextNodes = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
            // return handleChatFlowType(nextNode?.childId ?? null, whatsappCcaasTable[0] ?? null, node, io);
          }
          return handleChatFlowType(nextNode, facebookCcaasTable, nodes, input, req);
        } catch (err) {
          logger.error("handleChatFlowType err", err)
          const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
          return handleChatFlowType(nextNode, facebookCcaasTable, nodes, input, req);
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
              await sendCustomerInput(facebookCcaasTable[0], parseData);
            } else if (parseData?.menuType === 'button') {
              const nextNode = currentNode?.data?.children?.find((nodes1: any) => nodes1?.childId) ?? { childId: '' };
              await sendButtonLink(facebookCcaasTable[0], parseData);
              try {
                await updateMessageFlow(facebookCcaasTable, parseData);
                await updateNextNode(facebookCcaasTable[0], nextNode?.childId);
                if (entryCarousal?.length !== 0) {
                  await sendCarouselData(facebookCcaasTable[0], entryCarousal);
                }
                return handleChatFlowType(nextNode, facebookCcaasTable, nodes, input, req);
              } catch (err) {
                logger.error("handleChatFlowType err", err)
              }
            }
          } else if (currentNode?.data?.information?.inputType === 5) {
            const phoneNumber = currentNode.data?.information?.message;
            await sendMessageCustomer(facebookCcaasTable[0], phoneNumber);
            try {
              await updateMessageFlowData(facebookCcaasTable, phoneNumber);
              if (entryCarousal?.length !== 0) {
                await sendCarouselData(facebookCcaasTable[0], entryCarousal);
              }
            } catch (err) {
              logger.error("handleChatFlowType err", err)
            }
          } else if (currentNode?.data?.information?.inputType === 6) {
            const emailId = currentNode.data?.information?.message;
            await sendMessageCustomer(facebookCcaasTable[0], emailId);
            try {
              await updateMessageFlowData(facebookCcaasTable, emailId);
              if (entryCarousal?.length !== 0) {
                await sendCarouselData(facebookCcaasTable[0], entryCarousal);
              }
            } catch (err) {
              logger.error("handleChatFlowType err", err)
            }
          } else if (currentNode?.data?.information?.inputType === 2) {
            const textMessage = currentNode.data?.information?.message;
            await sendMessageCustomer(facebookCcaasTable[0], textMessage);
            try {
              await updateMessageFlowData(facebookCcaasTable, textMessage);
              if (entryCarousal?.length !== 0) {
                await sendCarouselData(facebookCcaasTable[0], entryCarousal);
              }
            } catch (err) {
              logger.error("handleChatFlowType err", err)
            }
          } else if (currentNode?.data?.information?.inputType === 7) {
            const number = currentNode.data?.information?.message;
            await sendMessageCustomer(facebookCcaasTable[0], number);
            try {
              await updateMessageFlowData(facebookCcaasTable, number);
              if (entryCarousal?.length !== 0) {
                await sendCarouselData(facebookCcaasTable[0], entryCarousal);
              }
            } catch (err) {
              logger.error("handleChatFlowType err", err)
            }
          } else if (currentNode?.data?.information?.inputType === 8) {
            const url = currentNode.data?.information?.message;
            await sendMessageCustomer(facebookCcaasTable[0], url);
            try {
              await updateMessageFlowData(facebookCcaasTable, url);
              if (entryCarousal?.length !== 0) {
                await sendCarouselData(facebookCcaasTable[0], entryCarousal);
              }
            } catch (err) {
              logger.error("handleChatFlowType err", err);
            }
          } else if (currentNode?.data?.information?.inputType === 9) {
            const name = currentNode.data?.information?.message;
            await sendMessageCustomer(facebookCcaasTable[0], name);
            try {
              await updateMessageFlowData(facebookCcaasTable, name);
              if (entryCarousal?.length !== 0) {
                await sendCarouselData(facebookCcaasTable[0], entryCarousal);
              }
            } catch (err) {
              logger.error("handleChatFlowType err", err);
            }
          } else if (currentNode?.data?.information?.inputType === 16) {
            const FileMessage = currentNode.data?.information?.message;
            await sendMessageCustomer(facebookCcaasTable[0], FileMessage);
            try {
              await updateMessageFlowData(facebookCcaasTable, FileMessage);
              if (entryCarousal?.length !== 0) {
                await sendCarouselData(facebookCcaasTable[0], entryCarousal);
              }
            } catch (err) {
              logger.error("handleChatFlowType err", err);
            }
          } else if (currentNode?.data?.information?.inputType === 14) {
            const dropDownMessage = currentNode.data?.information?.message;
            const dropDownData = JSON.parse(currentNode.data?.information?.data);
            const dropDownMultipleData = dropDownData?.options?.map((item: any) => ({ payload: item?.value, content_type: 'text', title: item?.value }));
            await sendCustomerSatificationFbDropdown(facebookCcaasTable[0], dropDownMessage, dropDownMultipleData);
          }
          if (custom_input_update) {
            await updateNextNode(facebookCcaasTable[0], nodeId);
          }
        } catch (err) {
          const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
          logger.error("handleChatFlowType err", err);
          return handleChatFlowType(nextNode, facebookCcaasTable, nodes, input, req);
        }
        break;
      }
      case 'sendMail': {
        try {
          logger.info('sendMail>>>>>>>>>>>>>>>>>>>>>>>', currentNode);
          const templateData = JSON.parse(currentNode?.data?.information?.template_data);
          const getEmailData = await getCustomerEmailId(facebookCcaasTable[0]);
          if (templateData?.type === '1') {
            await sendEmail(getEmailData, templateData);
          } else {
            const adminEmail: any = await getAdminEmail(facebookCcaasTable[0])
            await sendEmailPlainTextFB(getEmailData, templateData, facebookCcaasTable[0], adminEmail);
          }
          const nextNode = currentNode?.data?.children?.find((nodes1: any) => nodes1?.type === 'success') ?? { childId: '' };
          const nextNodedetails: any = await getNodeDataById(nextNode?.childId, nodes);
          let message: any = {};
          try {
            message = {
              messageID: uuidv4(),
              sessionId: facebookCcaasTable[0]?.sessionId,
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
            stringifiedMessage.FacebookHistoryId = facebookCcaasTable[0]?.FacebookHistoryId;
            await updateChatMessage(stringifiedMessage);
            updateIsBotFb(facebookCcaasTable[0]?.FacebookHistoryId);
            await updateNextNode(facebookCcaasTable[0], nextNode?.childId);
            if (entryCarousal?.length !== 0) {
              await sendCarouselData(facebookCcaasTable[0], entryCarousal);
            }
            return handleChatFlowType(nextNode, facebookCcaasTable, nodes, input, req);
          } catch (error) {
            const nextNodes = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
            // return handleChatFlowType(nextNode?.childId ?? null, whatsappCcaasTable[0] ?? null, node, io);
          }
          return handleChatFlowType(nextNode, facebookCcaasTable, nodes, input, req);
        } catch (err) {
          logger.error("sendMail err", err);
          const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
          return handleChatFlowType(nextNode, facebookCcaasTable, nodes, input, req);
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
            await updateAppointmentUserDetails(facebookCcaasTable[0], appointmentId, companyId, userId, getAppointmentType[0]);
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
            await sendAppointmentDateFb(facebookCcaasTable[0], selectDate);
            await insertFlowTracking(facebookCcaasTable[0], "date", { appId: appointmentId, companyId: companyId, userId: userId });
          } else if (getAppointmentType[0]?.app_typeid === 2) {
            await updateAppointmentUserDetails(facebookCcaasTable[0], appointmentId, companyId, userId, getAppointmentType[0]);
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
            await sendAppointmentDateFb(facebookCcaasTable[0], selectDate);
            await insertFlowTracking(facebookCcaasTable[0], "date", { appId: appointmentId, companyId: companyId, userId: userId });
          } else if (getAppointmentType[0]?.app_typeid === 3) {
            await updateAppointmentUserCreateBy(facebookCcaasTable[0], appointmentId, companyId, userId, getAppointmentType[0]);
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
            await sendAppointmentDateFb(facebookCcaasTable[0], selectDate);
            await insertFlowTracking(facebookCcaasTable[0], "date", { appId: appointmentId, companyId: companyId, userId: userId });
          } else {
            const categoryName = ServiceName.map((category: any) => ({ content_type: "text", title: category?.categoryName, payload: category?.categoryId }));
            const uniqueCategories = categoryName.filter((item: any, index: any, self: any) => index === self.findIndex((t: any) => t.id === item.id && t.title === item.title));
            logger.info("select category", uniqueCategories);
            await sendCategoryName(facebookCcaasTable[0], uniqueCategories);
            let datas: any = {
              appointmentId,
              companyId,
              userId
            }
            await insertFlowTracking(facebookCcaasTable[0], "categories", datas);
          }
        } catch (err) {
          logger.error("appointment err", err);
          const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
          return handleChatFlowType(nextNode, facebookCcaasTable, nodes, input, req);
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
              const targetRestructured: any = { sessionId: facebookCcaasTable[0]?.sessionId };
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
                    await sendMessageCustomer(facebookCcaasTable[0], finalResponse);
                    const message: any = {
                      messageID: uuidv4(),
                      sessionId: facebookCcaasTable[0]?.sessionId,
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
                    stringifiedMessage.FacebookHistoryId = facebookCcaasTable[0]?.FacebookHistoryId;
                    await updateChatMessage(stringifiedMessage);
                    updateIsBotFb(facebookCcaasTable[0]?.FacebookHistoryId);
                    if (entryCarousal?.length !== 0) {
                      await sendCarouselData(facebookCcaasTable[0], entryCarousal);
                    }
                  } else if (nodeNext.childId.includes('messageFlow')) {
                    const nextNodedetails: any = await getNodeDataById(nodeNext.childId, nodes);
                    const text = nextNodedetails?.data?.information?.message;
                    const availableContexts = text?.match(contextPattern);
                    const updatedMessage = availableContexts?.length > 0 ? replacechatPatternWithVariables(availableContexts, text, _formattedSelectedKeys) : text;
                    const data = { ...nextNodedetails?.data?.information };
                    data.message = `${updatedMessage}`;
                    const targetRestructured: any = { nextNodedetails, data, sessionId: facebookCcaasTable[0]?.sessionId };
                    targetRestructured.msg = updatedMessage;
                    const messagenextNode = nextNodedetails?.data?.children?.find((nodes: any) => nodes?.type === RESPONSE?.success_message) ?? { childId: '' };
                    let messagenextNodedetails: any = {};
                    if (messagenextNode.childId) {
                      messagenextNodedetails = await getNodeDataById(messagenextNode?.childId, nodes);
                    }
                    targetRestructured.currentNode = nextNodedetails;
                    const message: any = {
                      messageID: uuidv4(),
                      sessionId: facebookCcaasTable[0]?.sessionId,
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
                    stringifiedMessage.FacebookHistoryId = facebookCcaasTable[0]?.FacebookHistoryId;
                    await updateChatMessage(stringifiedMessage);
                    updateIsBotFb(facebookCcaasTable[0]?.FacebookHistoryId);
                    if (entryCarousal?.length !== 0) {
                      await sendCarouselData(facebookCcaasTable[0], entryCarousal);
                    }
                    const body = {
                      domainId: facebookCcaasTable[0]?.domainId,
                      customerId: facebookCcaasTable[0]?.customerFacebookId,
                      pageId: facebookCcaasTable[0]?.agentFacebookId,
                      type: 'text',
                      message: targetRestructured?.msg?.replace(/<[^>]+>/g, '')
                    };
                    const _cpynextNodedetails: any = { ...messagenextNodedetails };
                    targetRestructured.currentNode = _cpynextNodedetails;
                    await axios.post(process?.env?.FacebookOut, body);
                  } else if (nodeNext.childId.includes('integration')) {
                    await updateNextNode(facebookCcaasTable[0], nodeNext?.childId);
                    if (entryCarousal?.length !== 0) {
                      await sendCarouselData(facebookCcaasTable[0], entryCarousal);
                    }
                    return handleChatFlowType(nodeNext, facebookCcaasTable, nodes, input, req);
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
                    const targetRestructured: any = { nextNodedetails, data, sessionId: facebookCcaasTable[0]?.sessionId };
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
                    stringifiedMessage.FacebookHistoryId = facebookCcaasTable[0]?.FacebookHistoryId;
                    await updateChatMessage(stringifiedMessage);
                    updateIsBotFb(facebookCcaasTable[0]?.FacebookHistoryId);
                    const body = {
                      domainId: facebookCcaasTable[0]?.domainId,
                      customerId: facebookCcaasTable[0]?.customerFacebookId,
                      pageId: facebookCcaasTable[0]?.agentFacebookId,
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
              return handleChatFlowType(nextNode, facebookCcaasTable, nodes, input, req);
            }
          } else if (type === 'connectors') {
            logger.info('not implemented');
          }
        } catch (error: any) {
          logger.info('integration err', error);
          const nextNode = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'error') ?? { childId: '' };
          return handleChatFlowType(nextNode, facebookCcaasTable, nodes, input, req);
        }
        break;
      }
      case 'transferToQueue': {
        try {
          logger.info('transferToQueue>>>>>>>>>>>>>>>>>>', currentNode);
          const getBotDetails: any = await getBotDetail(facebookCcaasTable[0]);
          const getCoid = await getCustomerCoId(facebookCcaasTable[0]);
          if (currentNode?.data?.information?.type === 1) {
            insertCustomerInQueue(facebookCcaasTable[0], currentNode, currentNode?.data?.information?.transferTo);
            const agentDetailRes: any = await transferChat(facebookCcaasTable[0], currentNode);
            const agentDetails = agentDetailRes ? [agentDetailRes] : [];
            try {
              if (Array.isArray(agentDetails) && agentDetails?.length > 0) {
                const getPersonalData: any = await getPersonalDetails(facebookCcaasTable[0]?.customerFacebookId, facebookCcaasTable[0]?.domainId);
                console.log(emiiter?.sendInvite, `${agentDetails[0]?.domainId}_${agentDetails[0]?.ext}`);
                ioredisSocialMedia.to(`${agentDetails[0]?.domainId}_${agentDetails[0]?.ext}`).emit(emiiter?.sendInvite, {
                  channelType: facebookCcaasTable[0].channelType, customerNumber: facebookCcaasTable[0].customerFacebookId,
                  agentNumber: facebookCcaasTable[0].agentFacebookId, sessionId: facebookCcaasTable[0].sessionId, historyId: facebookCcaasTable[0]?.FacebookHistoryId,
                  calledPersonDetails: getPersonalData[0] ?? null,
                  allAgent: true,
                  isTransferChat: 0,
                  currentNode: currentNode?.data?.information?.transferTo ?? 0
                });
                await sendConnectLiveAgent(facebookCcaasTable[0]);
                sendConnectLiveAgentMessage(facebookCcaasTable);
              }
            } catch (error) {
              logger.error('transferToQueue err', error);
            }
          } else {
            const getUser = await getUserExt(currentNode?.data?.information);
            await connectToliveAgent(getUser, facebookCcaasTable, getBotDetails, getCoid);
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
          await updateNextNode(facebookCcaasTable[0], childIdData?.childId);
          if (entryCarousal?.length !== 0) {
            await sendCarouselData(facebookCcaasTable[0], entryCarousal);
          }
          return handleChatFlowType(childIdData, facebookCcaasTable, nodes, input, req);
        } catch (error) {
          logger.error('randomise err', error);
          const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
          return handleChatFlowType(nextNode, facebookCcaasTable, nodes, input, req);
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
          await updateNextNode(facebookCcaasTable[0], currentNodeId);
          if (entryCarousal?.length !== 0) {
            await sendCarouselData(facebookCcaasTable[0], entryCarousal);
          }
          return handleChatFlowType(currentNodeId, facebookCcaasTable, nodes, input, req);
        } catch (error) {
          logger.error('addFlow err', error);
          const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
          return handleChatFlowType(nextNode, facebookCcaasTable, nodes, input, req);
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
            await sendMessageCustomer(facebookCcaasTable[0], useCode);
            const nextNode = currentNode?.data?.children?.find((nodes1: any) => nodes1?.type === RESPONSE?.success_message) ?? { childId: '' };
            const nextNodedetails: any = await getNodeDataById(nextNode?.childId, nodes);
            try {
              await updateMessageFlowData(facebookCcaasTable, useCode);
            } catch (error) {
              const nextNodes = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
              // return handleChatFlowType(nextNode?.childId ?? null, whatsappCcaasTable[0] ?? null, node, io);
            }
            await updateNextNode(facebookCcaasTable[0], nextNode?.childId);
            if (entryCarousal?.length !== 0) {
              await sendCarouselData(facebookCcaasTable[0], entryCarousal);
            }
            return handleChatFlowType(nextNode, facebookCcaasTable, nodes, input, req);
          }
        } catch (error) {
          logger.error('coupon err', error)
          const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
          return handleChatFlowType(nextNode, facebookCcaasTable, nodes, input, req);
        }
        break;
      }
      case 'checkCondition': {
        try {
          logger.info('checkCondition>>>>>>>>>>>>>>>>>>>>>>>', currentNode);
          const conditionParseData = JSON.parse(currentNode?.data?.information?.condition_data);
          const AgentAvailibility = await getAgentAvailibility(facebookCcaasTable[0]);
          const getEmailData: any = await getCustomerEmailId(facebookCcaasTable[0]);
          const getExistingCustomer: any = await getExistingCustomerContact(facebookCcaasTable[0]);
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
                const nextNode = currentNode?.data?.children?.find((nodes1: any) => nodes1?.type === RESPONSE?.success_message) ?? { childId: '' };
                const nextNodedetails: any = await getNodeDataById(nextNode?.childId, nodes);
                await updateNextNode(facebookCcaasTable[0], nextNode?.childId);
                if (entryCarousal?.length !== 0) {
                  await sendCarouselData(facebookCcaasTable[0], entryCarousal);
                }
                return handleChatFlowType(nextNode, facebookCcaasTable, nodes, input, req);
              } else {
                const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
                await updateNextNode(facebookCcaasTable[0], nextNode?.childId);
                return handleChatFlowType(nextNode, facebookCcaasTable, nodes, input, req);
              }
            }
          }
        } catch (error) {
          logger.error('checkCondition err', error)
          const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
          return handleChatFlowType(nextNode, facebookCcaasTable, nodes, input, req);
        }
        break;
      }
      case 'disconnect': {
        console.log("disconnect>>>>>>>>>>>>>>>>>>>>>>>>", currentNode);
        await getDispositinDisconnectStatus(facebookCcaasTable[0]);
        const getDispositin: any = await getDispositinStatus(facebookCcaasTable[0]);
        const closedDispositions = getDispositin[0]?.filter((item: { dispositionName: string; }) => item.dispositionName === 'Closed');
        await updateDispositionDiscnnect(facebookCcaasTable[0], closedDispositions[0]);
        await updateDispositionBot(facebookCcaasTable[0]);
        const updateScore: any = await sentimentalScoreData(facebookCcaasTable[0]);
        const updateScoreData = updateScore?.data?.sentimentalScore;
        await updateSentimentScoreBot(facebookCcaasTable[0], updateScoreData);
        const sentimentSupervisorBot: any = await getSupervisiorList(facebookCcaasTable[0]);
        sentimentSupervisorBot.forEach((ext: any) => {
          console.log(emiiter?.supervisiorActivityBot, `${facebookCcaasTable[0]?.domainId}_${ext?.ext}`);
          ioredisSocialMedia.to(`${facebookCcaasTable[0]?.domainId}_${ext?.ext}`).emit(emiiter?.supervisiorActivityBot);
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
    logger.error('disconnect err', error)
  }
}

/**
 * Connect to a live agent or fallback to bot-based handling if no agent is available
 * @param liveAgent 
 * @param facebookCcaasTable 
 * @param getBotDetails 
 * @param getCoid 
 * @returns Promise<void>
 */
export const connectToliveAgent = async (liveAgent: any, facebookCcaasTable: any, getBotDetails: any, getCoid: any): Promise<void> => {
  try {
    const fbData = facebookCcaasTable[0];
    if (!fbData) return;
    const { customerFacebookId, domainId, channelType, agentFacebookId, sessionId, FacebookHistoryId } = fbData;
    if (Array.isArray(liveAgent) && liveAgent?.length > 0 && liveAgent[0]?.domainId && liveAgent[0]?.ext) {
      const personalData = await getPersonalDetails(customerFacebookId, domainId);
      logger.info(emiiter?.sendInvite, `${liveAgent[0]?.domainId}_${liveAgent[0]?.ext}`);
      ioredisSocialMedia.to(`${liveAgent[0]?.domainId}_${liveAgent[0]?.ext}`).emit(emiiter?.sendInvite, {
        channelType,
        customerNumber: customerFacebookId,
        agentNumber: agentFacebookId,
        sessionId,
        historyId: FacebookHistoryId,
        calledPersonDetails: personalData?.[0] ?? null,
        allAgent: true,
        isTransferChat: 1,
        ext: liveAgent[0]?.ext,
        domainId: domainId
      });
      await Promise.all([
        sendConnectLiveAgent(fbData),
        sendConnectLiveAgentMessage(facebookCcaasTable)
      ]);
      return;
    }
    logger.info("No live agent found. Checking bot fallback...");
    // if (Array.isArray(getBotDetails) && getBotDetails?.length > 0 && Number(getBotDetails[0]?.callBackToggle) === 1) {
    //   const redirectUrlCallback = `FacebookWebcallback?coId=${getCoid?.[0]?.coid}&domainId=${domainId}&facebookId=${customerFacebookId}&historyId=${FacebookHistoryId}`;
    //   await Promise.all([
    //     sendCallbackRequest(fbData, redirectUrlCallback),
    //     updateCallbackStatus(fbData),
    //     sendAgentAvailable(facebookCcaasTable)
    //   ]);
    // } else {
    //   await Promise.all([
    //     sendMessage(fbData),
    //     sendAgentAvailableMissed(facebookCcaasTable),
    //     updateMissedChat(fbData)
    //   ]);
    //   const supervisors = await getSupervisiorList(fbData);
    //   if (Array.isArray(supervisors) && supervisors?.length > 0) {
    //     supervisors.forEach((ext: any) => {
    //       if (ext?.ext) {
    //         ioredisSocialMedia.to(`${domainId}_${ext?.ext}`).emit(emiiter?.supervisiorActivityBot);
    //       }
    //     });
    //   }
    // }
  } catch (error) {
    logger.error('Error in connectToliveAgent (Facebook):', error);
    throw error;
  }
};



/**
 * 
 * @param facebookCcaasTable 
 * @returns 
 */
export const sendAgentAvailable = async (facebookCcaasTable: any) => {
  return new Promise(async (resolve, reject) => {
    let message: any = {};
    try {
      message = {
        messageID: uuidv4(),
        sessionId: facebookCcaasTable[0]?.sessionId,
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
      stringifiedMessage.FacebookHistoryId = facebookCcaasTable[0]?.FacebookHistoryId;
      await updateChatMessage(stringifiedMessage);
    } catch (error) {
      logger.error('sendAgentAvailable err', error);
      reject(error);
    }
  });
}

/**
 * 
 * @param facebookCcaasTable 
 * @returns 
 */
export const sendAgentAvailableMissed = (facebookCcaasTable: any) => {
  return new Promise(async (resolve, reject) => {
    let message: any = {};
    try {
      message = {
        messageID: uuidv4(),
        sessionId: facebookCcaasTable[0]?.sessionId,
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
      stringifiedMessage.FacebookHistoryId = facebookCcaasTable[0]?.FacebookHistoryId;
      await updateChatMessage(stringifiedMessage);
      updateIsBotFb(facebookCcaasTable[0]?.FacebookHistoryId);
    } catch (error) {
      logger.error('sendAgentAvailableMissed err', error);
      reject(error);
    }
  });
};

/**
 * 
 * @param facebookCcaasTable 
 * @param messageData 
 * @returns 
 */
export const sendMessageCustomerCaiMessage = async (facebookCcaasTable: any, messageData: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      let message: any = {};
      message = {
        messageID: uuidv4(),
        sessionId: facebookCcaasTable[0]?.sessionId,
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
      stringifiedMessage.FacebookHistoryId = facebookCcaasTable[0]?.FacebookHistoryId;
      await updateChatMessage(stringifiedMessage);
      updateIsBotFb(facebookCcaasTable[0]?.FacebookHistoryId);
    } catch (error) {
      logger.error('sendMessageCustomerCaiMessage err', error);
      reject(error);
    }
  });
}

/**
 * 
 * @param facebookCcaasTable 
 * @returns 
 */
export const sendFlowBreakMessage = async (facebookCcaasTable: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      let message: any = {};
      message = {
        messageID: uuidv4(),
        sessionId: facebookCcaasTable[0]?.sessionId,
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
      stringifiedMessage.FacebookHistoryId = facebookCcaasTable[0]?.FacebookHistoryId;
      await updateChatMessage(stringifiedMessage);
    } catch (error) {
      logger.error('sendFlowBreakMessage err', error);
      reject(error);
    }
  });
}

/**
 * 
 * @param facebookCcaasTable 
 * @returns 
 */
export const sendFlowBreakMessageRemainderFb = async (facebookCcaasTable: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      let message: any = {};
      message = {
        messageID: uuidv4(),
        sessionId: facebookCcaasTable[0]?.sessionId,
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
      stringifiedMessage.FacebookHistoryId = facebookCcaasTable[0]?.FacebookHistoryId;
      await updateChatMessage(stringifiedMessage);
    } catch (error) {
      logger.error('sendFlowBreakMessageRemainderFb err', error);
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
 * @returns 
 */
export const triggerDataHandler = (trigger: any, req: any, nodes: any, facebookCcaasTable: any, getNlpData: any) => {
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
        await updateNextNode(facebookCcaasTable[0], currentNodeId);
        handleChatFlowType(currentNodeId, facebookCcaasTable, nodes, '', req);
      } else {
        resolve(false);
      }
    } catch (error) {
      logger.error('triggerDataHandler err', error);
      reject(error);
    }
  });
};

/**
 * 
 * @param facebookCcaasTable 
 * @param messageData 
 * @returns 
 */
export const sendFlowMessageData = async (facebookCcaasTable: any, errorMessage: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      let message: any = {};
      message = {
        messageID: uuidv4(),
        sessionId: facebookCcaasTable[0]?.sessionId,
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
      stringifiedMessage.FacebookHistoryId = facebookCcaasTable[0]?.FacebookHistoryId;
      await updateChatMessage(stringifiedMessage);
      updateIsBotFb(facebookCcaasTable[0]?.FacebookHistoryId);
    } catch (error) {
      logger.error('sendFlowMessageData err', error);
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
export const triggerDataSendMessageHandler = (trigger: any, req: any, nodes: any, facebookCcaasTable: any, getNlpData: any) => {
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
        await updateNextNode(facebookCcaasTable[0], currentNodeId);
        return handleChatFlowType(currentNodeId, facebookCcaasTable, nodes, req?.body?.text?.body, req);
      } else {
        await sendMessageCustomer(facebookCcaasTable[0], getNlpData?.data?.message);
        let message: any = {};
        try {
          message = {
            messageID: uuidv4(),
            sessionId: facebookCcaasTable[0]?.sessionId,
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
          stringifiedMessage.FacebookHistoryId = facebookCcaasTable[0]?.FacebookHistoryId;
          await updateChatMessage(stringifiedMessage);
          updateIsBotFb(facebookCcaasTable[0]?.FacebookHistoryId);
        } catch (error) {
          logger.error('triggerDataSendMessageHandler err', error);
        }
      }
    } catch (error) {
      logger.error('triggerDataSendMessageHandler err', error);
      reject(error);
    }
  });
};

/**
 * 
 * @param data 
 * @param facebookCcaasTable 
 * @param currentNode_ref 
 * @param nodes 
 * @param getNlpData 
 * @param req 
 * @returns 
 */
export const uploadFilesFlow = async (data: any, facebookCcaasTable: any, currentNode_ref: any, nodes: any, getNlpData: any, req: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      const nextData = currentNode_ref?.data?.children[0];
      await updateNextNode(facebookCcaasTable[0], nextData?.childId);
      return handleChatFlowType(null, facebookCcaasTable, nodes, '', req);
    } catch (error) {
      logger.error('uploadFilesFlow err', error);
      reject(error);
    }
  });
}

/**
 * 
 * @param facebookCcaasTable 
 * @param messageData 
 * @returns 
 */
export const updateMessageFlow = async (facebookCcaasTable: any, messageData: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      let message = {};
      message = {
        messageID: uuidv4(),
        sessionId: facebookCcaasTable[0]?.sessionId,
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
      stringifiedMessage.FacebookHistoryId = facebookCcaasTable[0]?.FacebookHistoryId;
      await updateChatMessage(stringifiedMessage);
      updateIsBotFb(facebookCcaasTable[0]?.FacebookHistoryId);
    } catch (error) {
      logger.error('updateMessageFlow err', error);
      reject(error);
    }
  });
}

/**
 * 
 * @param facebookCcaasTable 
 * @param messageData 
 * @returns 
 */
export const updateMessageFlowData = async (facebookCcaasTable: any, messageData: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      let message = {};
      message = {
        messageID: uuidv4(),
        sessionId: facebookCcaasTable[0]?.sessionId,
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
      stringifiedMessage.FacebookHistoryId = facebookCcaasTable[0]?.FacebookHistoryId;
      await updateChatMessage(stringifiedMessage);
      updateIsBotFb(facebookCcaasTable[0]?.FacebookHistoryId);
    } catch (error) {
      logger.error('updateMessageFlowData err', error);
      reject(error);
    }
  });
}

/**
 * 
 * @param facebookCcaasTable 
 * @param messageData 
 * @returns 
 */
export const updateMessageFlowAppoinment = async (facebookCcaasTable: any, messageData: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      let message = {};
      message = {
        messageID: uuidv4(),
        sessionId: facebookCcaasTable?.sessionId,
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
      stringifiedMessage.FacebookHistoryId = facebookCcaasTable?.FacebookHistoryId;
      await updateChatMessage(stringifiedMessage);
    } catch (error) {
      logger.error('updateMessageFlowAppoinment err', error);
      reject(error);
    }
  });
}

/**
 * 
 * @param facebookCcaasTable 
 */
export const sendConnectLiveAgentMessage = async (facebookCcaasTable: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      let message = {};
      message = {
        messageID: uuidv4(),
        sessionId: facebookCcaasTable[0]?.sessionId,
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
      stringifiedMessage.FacebookHistoryId = facebookCcaasTable[0]?.FacebookHistoryId;
      await updateChatMessage(stringifiedMessage);
    } catch (error) {
      logger.error('sendConnectLiveAgentMessage err', error);
      reject(error);
    }
  });
}

/**
 * 
 * @param facebookCcaasTable 
 * @param message 
 */
export const sendMessageCustomerCount = async (facebookCcaasTable: any, messageData: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      let message: any = {};
      message = {
        messageID: uuidv4(),
        sessionId: facebookCcaasTable?.sessionId,
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
      stringifiedMessage.FacebookHistoryId = facebookCcaasTable?.FacebookHistoryId;
      await updateChatMessage(stringifiedMessage);
    } catch (error) {
      logger.error('sendMessageCustomerCount err', error);
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
export const insertCustomerInQueue = async (data: any, currentData: any, queueId: any) => {
  try {
    const getCId = await getCustomerCoId(data);
    const queueDetails: any = await getQueueSettingS(queueId, data?.domainId);
    const customerDetails: any = await getCustomerConatctDetails(data?.domainId, getCId[0]);
    const queueWeightTime: any = await getAllQueueWaitTime(data?.domainId);
    const result = queueWeightTime?.filter((list: any) => list?.qid === queueId && list?.queueName === queueDetails?.name && list?.queueType === channelType?.facebook);
    const queueListData = [];
    const waitTimmer: any = JSON.parse(queueDetails?.maximumCallers);
    const waitQueueListData: any = result?.length > 0 ? JSON.parse(result[0]?.queueList) : [];
    if (waitTimmer?.type !== 'Set Max Caller limit' || waitTimmer?.queueLimit > waitQueueListData?.length) {
      const queueTypeData = {
        firstName: customerDetails[0]?.firstName ?? null,
        lastName: customerDetails[0]?.lastName ?? null,
        sessionId: data?.sessionId,
        queuedTime: Math.floor(Date.now() / 1000),
        type: channelType?.Facebook,
        chatHistoryId: data?.FacebookHistoryId,
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
        queueType: channelType?.facebook,
        domainId: data?.domainId,
        queueList: JSON.stringify(queueListData)
      };
      try {
        await insertUpdateQueueWaitTimeDao(payload, queueDetails, data);
      } catch (err) {
        logger.error('insertCustomerInQueue err', err);
      }
      ioredisSocialMedia.emit(emiiter?.queueWait, payload);
      const redisData = {
        qid: queueId,
        queueType: channelType?.facebook,
        domainId: data?.domainId,
      };
      const redisQueueData: any = await ioredisFacebookQueue.get(process.env.FACEBOOK_QUEUE);
      if (redisQueueData) {
        const redisMap = JSON.parse(redisQueueData);
        const hasQid = redisMap?.some((item: any) => item.qid === queueId);
        if (!hasQid) {
          redisMap.push(redisData);
          ioredisFacebookQueue.set(process.env.FACEBOOK_QUEUE, JSON.stringify(redisMap));
        }
      } else {
        ioredisFacebookQueue.set(process.env.FACEBOOK_QUEUE, JSON.stringify([redisData]));
      }
    } else {
      data.queueId = waitTimmer?.overFlowQueue;
      insertCustomerInQueue(data, currentData, waitTimmer?.overFlowQueue);
    }
  } catch (err) {
    logger.error('insertCustomerInQueue err', err);
  }
};

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
