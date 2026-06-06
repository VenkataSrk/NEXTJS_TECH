import axios from 'axios';
import { log } from 'console';
import CryptoJS from 'crypto-js';
import { v4 as uuidv4 } from 'uuid';
import { logger } from '../../../src/plugins/log';
import { configs } from '../../config/app';
import {
  agentDetailsApi,
  assignCallBackWhatsapp,
  autoAssignAgent,
  createEcompurchesFlow,
  deleteCustomInputTemplate,
  deleteMetaTemplate,
  deleteWhatsappConfiguration,
  getAdminEmail,
  getAssingToApi,
  getBotDetail,
  getBotMessageRes,
  getCompanyDetails,
  getconnectData,
  getConnectorTokenInfo,
  getContactData,
  getCustomerCoId,
  getCustomerEmailId,
  getCustomerSatisficationDao,
  getDidPurchaseWhatsappBotDao,
  getDisableConfigDetails,
  getDispositinStatus,
  getDispositionStatusDao,
  getEcomPurchaseFlow,
  getExistingCustomerContact,
  getFlowTracking,
  getIntegrationApiData,
  getKnowledgeBaseId,
  getMetaTemplateResponse,
  getNextNode,
  getnodesdata,
  getNoOfRepeatsRes,
  getPersonalDetails,
  getPlanDetail,
  getPlanInNlp,
  getQuestionsAns,
  getQueueSettingS,
  getQueueWaitTime,
  getRemaindeResponse,
  getRemainingSessionEmailSent,
  getSupervisiorDetails,
  getSupervisiorList,
  getTemplateDetails,
  getTransferAgentStatus,
  getUserExt,
  getUserListByExt,
  getWhatsapChannel,
  getWhatsappConfig,
  getWhatsappConfigConnceter,
  getWhatsappConfigData,
  getWhatsappConfigDetails,
  getWhatsappTemplate,
  insertBotMessage,
  insertBotMessageIsRemainder,
  insertBotMessageRemainderWhatsapp,
  insertCustomerContact,
  insertCustomerContactPhone,
  insertFlowTracking,
  insertMetaTemplate,
  insertUpdateMetaWhatsapp,
  insertUpdateWhatsappConfig,
  insertWhatsappCallbackDao,
  insertWhatsappTemplate,
  insertWhatsResponseUser,
  sessionIdApi,
  updateAppointmentConsultant,
  updateAppointmentConsultantId,
  updateAppointmentdate,
  updateAppointmentDuration,
  updateAppointmentServiceId,
  updateAppointmentTime,
  updateAppointmentUserCreateBy,
  updateAppointmentUserDetails,
  updateCallbackstatus,
  updateCallbackStatus,
  updateCategoryId,
  updateChatMessage,
  updateConnectData,
  updateCustomerFirstName,
  updateCustomerFirstNameAppoinment,
  updateCustomerId,
  updateCustomerLastName,
  updateCustomerLastNameAppoinment,
  updateDidNumberPurchaseWhatsappBotDao,
  updateDispositionBot,
  updateDispositionDiscnnect,
  updateDispositionForWhatsapp,
  updateEcomIsPurchase,
  updateEcomPurchaseFlow,
  updateEmailid,
  updateIsBot,
  updateMissedChat,
  updateNextNode,
  updateNoOfRepeats,
  updatePhNumber,
  updatePhoneNumber,
  updateSentimentScore,
  updateSentimentScoreBot,
  updateWhatsappBot,
  whatsappCcaasTableDB,
  whatsappFeedbackUpdateDB,
  whatsappSessionHistory,
} from '../../dao/whatsapp';
import { AppointmentInfo, AppointmentInfoPaymentCheck, bookingPayment, createBooking, getAppointmentInfo, getAppointmentLable, getAppointmentUserName, getCalenderViewDetails, getConsultantData, getdurationDetails, getEndTimeData, getLeaveData } from '../../helpers/appoinment';
import { channelType, emitter, messageValue, RESPONSE } from '../../helpers/constants';
import { getDispositinDisconnectStatus, getWhatsAppMediaUrl, sendAllUserName, sendAppoinmentPayment, sendAppointmentDate, sendAppointmentDuration, sendAppointmentOneToOne, sendAppointmentRes, sendBookingDetails, sendBookingDetailsOneToOne, sendbookingDuration, sendButtonLink, sendButtonLinkPayNow, sendCallbackRequest, sendCatalogues, sendCategoryName, sendConnectLiveAgent, sendCustomerInput, sendCustomerSatification, sendDropdownDetails, sendFlowBreakInput, sendFlowBreakInputRemainder, sendImageCustomer, sendMediaCustomer, sendMessage, sendMessageCustomer, sendMessageCustomerCai, sendMessageCustomerCallbacks, sendMessageCustomerFlow, sendMessageCustomerUrl, sendMessageNumber, sendTransferAgent, sendTyping } from '../../helpers/meta';
import { AgentAssistWhatsapp, getWhatsappBot, nlpResponseData, sentimentalScore, sentimentalScoreData } from '../../helpers/nlp';
import {
  addContextVariable,
  buildMultiLinkMessage,
  callGoogleSheetSyncAPI,
  callSessionExpiryAPI,
  contextPattern,
  correctjsondata,
  fetchApi,
  getAgentAvailibility,
  getNodeDataById,
  getPlandetails,
  getProductFeedList,
  getProductsData,
  getResponseData,
  getStartNode,
  getStartNodeCondition,
  getStartNodeConnecter,
  insertUpdateCrmContact,
  replacechatPatternWithVariables,
  replacePatternWithVariables,
  returnVariableData,
  safeParseJson,
  sendEmail,
  sendEmailPlainText
} from '../../helpers/utils';
import { ioredisWhatsapp, ioredisWhatsappQueue } from '../../plugins/redis';
import {
  insertCustomerInQueue,
  transferChat,
  transferChatToAvailableAgent
} from '../../services/transferToQueue';

/**
 *
 * @param req
 * @param res
 * @returns
 */
export async function insertWhatsappInHandler(req: any, res: any) {
  logger.info('insertWhatsappInHandler request body:', req.body);
  try {
    req.body.messageId = uuidv4();
    const interactiveId = req?.body?.interactive?.list_reply?.id;
    const interactiveTitle = req?.body?.interactive?.list_reply?.title;
    if (typeof interactiveId === 'string' && interactiveId.includes('-') && interactiveTitle !== 'More') {
      await whatsappFeedbackUpdateDB(req.body);
      return res.send({ statusCode: 200, message: RESPONSE.success_message });
    }
    const whatsappCcaasTable = await whatsappCcaasTableDB(req.body);
    if (whatsappCcaasTable[0]?.assignedTo === null || whatsappCcaasTable[0]?.AgentId === null) {
      sendTyping(req.body);
    }
    logger.info('MysqlInsertData:', whatsappCcaasTable);
    const isEmail: any = await getRemainingSessionEmailSent(req?.body);
    logger.info('IsSendEmail:', isEmail);
    if (isEmail[0]?.isEmailSent !== '') {
      callSessionExpiryAPI(isEmail[0]);
      logger.info('Session expiry API called');
    }
    const getCoid = await insertCustomerContactPhone(req.body);
    logger.info('Inserted customer contact phone:', getCoid);
    getConnectorTokenInfo(whatsappCcaasTable[0], (getConnect) => {
      if (getConnect?.length > 0) {
        logger.info('Connector token found:', getConnect[0]);
        callGoogleSheetSyncAPI({ contactSyncDetails: getConnect[0]?.contactSyncDetails, newContacts: getCoid[0]?.coid, domainId: whatsappCcaasTable[0]?.domainId, token: getConnect[0]?.token });
      }
    });

    try {
      const body = { domainId: whatsappCcaasTable?.[0]?.domainId, phone: whatsappCcaasTable?.[0]?.customerNumber };
      insertUpdateCrmContact(body); // crm insert
    } catch (error) {
      logger.error('insertUpdateCrmContact ::: error:', error);
    }

    await updateCustomerId(whatsappCcaasTable[0], getCoid[0]);
    if (whatsappCcaasTable[0].errcode !== 0) {
      logger.error('Error in whatsappCcaasTable:', whatsappCcaasTable[0]);
      return res.send({ statusCode: 500, message: messageValue?.processing });
    }
    const [sessionId, botDetails] = await Promise.all([sessionIdApi(req.body), getPlanDetail(req.body)]);
    logger.info('SessionId:', sessionId, 'BotDetails:', botDetails);
    const planDetails = getPlandetails(botDetails[0]);
    logger.info('Resolved plan type:', planDetails);
    console.log('Resolved plan type:', planDetails);
    if (planDetails === 'Traditional' || whatsappCcaasTable[0].assignedTo !== null || whatsappCcaasTable[0].AgentId !== null) {
      await handleTraditionalFlow(sessionId[0], whatsappCcaasTable[0], getCoid, botDetails, req.body);
    } else if (planDetails === 'CAI') {
      await handleCAIFlow(req.body, whatsappCcaasTable[0], getCoid);
    } else if (planDetails === 'Flow') {
      await insertRoleWhatsappInHandler(req, whatsappCcaasTable);
    }
    logger.info('insertWhatsappInHandler finished successfully');
    return res.send({ statusCode: 200, message: RESPONSE.success_message });
  } catch (err) {
    logger.error('insertWhatsappInHandler error:', err);
    return res.status(500).send({ statusCode: 500, message: 'Internal server error' });
  }
}

/**
 *
 * @param bot
 * @param record
 * @param botDetails
 * Handle CAI Bot Message
 */

async function handleCaiBotResponse(bot: any, record: any, body: any) {
  try {
    let message = '';
    const saveToWhatsappDB = (msg: any, type: any, url: any, fileType: any, mimeType: any, data:any) => {
      const botData = {
        id: record.sessionId,
        message: {
          agentText: {
            message: {
              mimeType,
              message: msg,
              messageID: record?.id,
              sessionId: record.sessionId,
              sendAt: Date.now(),
              messageType: type,
              direction: 'OUT',
              isBot: true,
              isCustomer: false,
              isLike: 0,
              pdfurl: url ?? '',
              fileType: fileType ?? '',
              replayId:data?.id ?? ''
            }
          }
        },
        source: record.customerNumber,
        destination: record.calledNumber,
        IsBot: 1
      };
      whatsappCcaasTableDB(botData);
    };

    /** ---------------- LINK MESSAGE ---------------- */
    if (bot?.data?.link?.length > 0) {
      const messagee = buildMultiLinkMessage(bot);
      const ress = await sendMessageCustomerUrl(body, messagee);
      saveToWhatsappDB(messagee, 'link', bot?.link, '', '', ress?.data);
      return;
    }

    /** ---------------- MEDIA MESSAGE ---------------- */
    if (bot?.data?.mediaUri) {
      message = bot?.data?.message || '';
      const resp:any = await sendImageCustomer(record, message, bot?.data);
      saveToWhatsappDB(message, 'media', bot?.data?.mediaUri, bot?.data?.mediaName, bot?.data?.mimeType, resp?.data);
      return;
    }

    /** ---------------- TEXT MESSAGE ---------------- */
    message = bot?.data?.message || '';
    try {
      const res = await sendMessageCustomerCai(record, message);
      saveToWhatsappDB(message, 'text', '', '', '', res?.data);
    } catch (error) {
      console.log(error);
    }
  } catch (error) {
    logger.error('CAI bot response error:', error);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function updateDispositionWhatsappHandler(
  req: any,
  res: any,
  done: any
) {
  try {
    const auth: any = req?.headers;
    const params = req?.params;
    const data: any = {
      WhatsappHistoryId: params?.WhatsappHistoryId,
      domainId: req?.headers?.domainId,
      dispositionId: req?.body?.dispositionId ?? 0,
      summary: req?.body?.summary ?? null,
      isSessionClosed: req?.body?.isSessionClosed ?? 0,
      afterWorkTime: req?.body?.afterWorkTime ?? null,
      dispositionUpdatedBy: req?.body?.dispositionUpdatedBy ?? null,
      duration: req?.body?.duration ?? null,
      dispositionSubject: req?.body?.dispositionSubject ?? null,
      tags: req?.body?.tags ?? null,
    };
    logger.info('updateDispositionWhatsappHandler - Request Data:', data);
    const updateDispStatus: any = await updateDispositionForWhatsapp(data);
    if (data?.isSessionClosed === 1) {
      const getSuperVisorExt: any = await getSupervisiorDetails(data?.domainId);
      getSuperVisorExt.forEach((ext: any) => {
        logger.info(emitter?.emitterDispStatus, `${data?.domainId}_${ext.ext}`);
        ioredisWhatsapp.to(`${data?.domainId}_${ext.ext}`).emit(emitter?.interActionClosed);
      });
    }
    const nlpData: any = {
      HistoryId: data?.WhatsappHistoryId,
      ChatDurationHistoryId: data?.ChatDurationHistoryId ?? 0,
      channelType: channelType?.Whatsapp,
      domainId: data?.domainId,
      ext: data?.ext ?? 0
    };

    const updateScore: any = await sentimentalScore(nlpData);
    if (updateScore.status === 200) {
      await updateSentimentScore(data, updateScore.data);
      if (updateScore.data) {
        ioredisWhatsapp.to(`${auth.domainId}_${auth.ext}`).emit(emitter?.SentimentScoreAnalytics, { data, speedometerScore: updateScore.data.sentimentalScore, sentimentalScoreUser: updateScore.data.sentimentalScoreUser, sentimentalScoreAgent: updateScore.data.sentimentalScoreAgent });
      } else {
        ioredisWhatsapp.to(`${auth.domainId}_${auth.ext}`).emit(emitter?.SentimentScoreAnalytics, { data, speedometerScore: 0, sentimentalScoreUser: 0, sentimentalScoreAgent: 0 });
      }
    } else {
      ioredisWhatsapp.to(`${auth.domainId}_${auth.ext}`).emit(emitter?.SentimentScoreAnalytics, { data, speedometerScore: 0, sentimentalScoreUser: 0, sentimentalScoreAgent: 0 });
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
    logger.info('isTransferred::', isTransferred);
    if (isTransferred !== 1) {
      const customerSatisfactionResult: any = await getCustomerSatisficationDao(data);
      await sendCustomerSatification(customerSatisfactionResult[0]);
    }
  } catch (err) {
    console.log(err);
    logger.error('updateDispositionWhatsapp error:', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function updateDispositionWhatsappCallbackHandler(
  req: any,
  res: any,
  done: any
) {
  try {
    const params = req?.params;
    const data: any = {
      WhatsappHistoryId: params?.WhatsappHistoryId,
      domainId: req?.body?.domainId,
      dispositionId: req?.body?.dispositionId ?? 0,
      summary: req?.body?.summary ?? null,
      isSessionClosed: req?.body?.isSessionClosed ?? 0,
      afterWorkTime: req?.body?.afterWorkTime ?? null,
      dispositionUpdatedBy: req?.body?.dispositionUpdatedBy ?? null,
      duration: req?.body?.duration ?? null,
      dispositionSubject: req?.body?.dispositionSubject ?? null,
    };
    const updateDispStatus: any = await updateDispositionForWhatsapp(data);
    if (data?.isSessionClosed === 1) {
      const getSuperVisorExt: any = await getSupervisiorDetails(data?.domainId);
      getSuperVisorExt.forEach((ext: any) => {
        console.log(emitter?.emitterDispStatus, `${data?.domainId}_${ext.ext}`);
        ioredisWhatsapp.to(`${data?.domainId}_${ext.ext}`).emit(emitter?.interActionClosed);
      });
    }
    const nlpData: any = {
      HistoryId: data?.WhatsappHistoryId,
      ChatDurationHistoryId: data?.ChatDurationHistoryId ?? 0,
      channelType: channelType?.Whatsapp,
      domainId: data?.domainId,
      ext: data?.ext ?? 0
    };

    const updateScore: any = await sentimentalScore(nlpData);
    if (updateScore.status === 200) {
      await updateSentimentScore(data, updateScore.data);
      if (updateScore.data) {
        ioredisWhatsapp.to(`${data.domainId}`).emit(emitter?.SentimentScoreAnalytics, { data, speedometerScore: updateScore.data.sentimentalScore, sentimentalScoreUser: updateScore.data.sentimentalScoreUser, sentimentalScoreAgent: updateScore.data.sentimentalScoreAgent });
      } else {
        ioredisWhatsapp.to(`${data.domainId}`).emit(emitter?.SentimentScoreAnalytics, { data, speedometerScore: 0, sentimentalScoreUser: 0, sentimentalScoreAgent: 0 });
      }
    } else {
      ioredisWhatsapp.to(`${data.domainId}`).emit(emitter?.SentimentScoreAnalytics, { data, speedometerScore: 0, sentimentalScoreUser: 0, sentimentalScoreAgent: 0 });
    }
    logger.info('updateDispStatus', updateDispStatus);
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
    logger.error('updateDispositionWhatsapp error:', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function deleteWhatsappConfigurationHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data = {
      whatsappId: req?.params?.whatsappId,
      domainId: auth.domainId
    };
    const deleteFlow: any = await deleteWhatsappConfiguration(data);
    logger.info('deleteFlow', deleteFlow);
    if (deleteFlow[0]?.errcode === 0) {
      res.status(200).send({ statusCode: 200, message: deleteFlow[0].errmsg });
    } else {
      res.status(200).send({ statusCode: 404, message: deleteFlow[0].errmsg });
    }
  } catch (err) {
    logger.error('deleteWhatsappConfiguration error:', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function assignCallBackWhatsappHandlers(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      assignedBy: req?.body?.assignedBy ?? null,
      assignedTo: req?.body?.assignedTo ?? null,
      assignedOn: Date.now(),
      WhatsappHistoryId: req?.body?.WhatsappHistoryId,
      Istransferred: req?.body?.Istransferred ?? null,
      domainId: auth.domainId,
      channelType: req?.body?.ChannelType ?? channelType?.Whatsapp,
      calledPersonDetails: req?.body?.calledPersonDetails ?? null,
      ext: req?.body?.ext
    };
    const assignRes: any = await assignCallBackWhatsapp(data);
    logger.info('assignRes', assignRes);
    try {
      await sendTransferAgent(assignRes[0]);
      console.log(emitter?.invite, `${assignRes[0]?.domainId}_${data?.ext}`);
      ioredisWhatsapp.to(`${assignRes[0]?.domainId}_${data?.ext}`).emit(emitter?.invite, {
        channelType: data?.channelType, customerNumber: assignRes[0]?.customerNumber,
        agentNumber: assignRes[0]?.calledNumber, sessionId: assignRes[0]?.sessionId, historyId: assignRes[0]?.WhatsappHistoryId,
        calledPersonDetails: data?.calledPersonDetails ?? null,
        allAgent: true, WhatsappDurationHistoryId: assignRes[0]?.WhatsappDurationHistoryId
      });
    } catch (error) {
      logger.error('sendTransferAgent error:', error);
    }
    if (assignRes) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.update_asssign });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {
    logger.error('assignCallBackWhatsapp error:', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export const sendCaasWhatsappHandler = async (req: any, res: any, done: any) => {
  try {
    const data: any = {
      sessionId: req.body.sessionId,
      from: req.body.from,
      to: req.body.to,
      text: req.body.text,
      type: req.body.type ?? 'update',
      attachments: req.body.attachments ?? null,
      agentName: req.body?.agentName,
      replayId: req?.body?.replayId ?? '',
      whisper: req?.body?.whisper ?? false,
    };
    const messageList: any = {
      agentText: {
        message: {
          from: data?.from ?? '',
          to: data?.to ?? '',
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
    const sendMessages: any = {
      message: messageList,
      from: data.from,
      to: data.to,
    };
    const whatsapp = await whatsappSessionHistory(sendMessages, data.sessionId);
    const getSupervisiorExt: any = await getSupervisiorList(whatsapp[0]);
    const getPersonalData: any = await getPersonalDetails(data?.to, whatsapp[0]?.domainId);
    getSupervisiorExt.forEach((ext: any) => {
      console.log(emitter?.receivedWhatsapp, `${whatsapp[0]?.domainId}_${ext.ext}`);
      ioredisWhatsapp.to(`${whatsapp[0]?.domainId}_${ext.ext}`).emit(emitter?.receivedWhatsapp, { ...whatsapp[0], channelType: channelType?.Whatsapp, agentAssist: false, calledPersonDetails: getPersonalData[0] }, data.sessionId);
    });
    logger.info('whatsappSendResponse', whatsapp);
    res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, whatsappSendResponse: messageList });
  } catch (err) {
    logger.error('sendCaasWhatsapp error:', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
};

export const updateWhatsappBotStatus = async (req: any, res: any) => {
  try {
    const data: any = {
      domainId: req?.body?.domainId,
      isBot: req?.body?.isBot ?? 0,
      isBotPurchased: req?.body?.isBotPurchased ?? 0,
      your_number: req?.body?.number,
    };
    await updateWhatsappBot(data);
    res.status(200).send({ statusCode: 200, message: RESPONSE.bot_status });
  } catch (error) {
    logger.error('updateWhatsappBotStatus catch error:', error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
};

export const getDidPurchaseWhatsappBot = async (req: any, res: any) => {
  try {
    const data: any = {
      domainId: req?.headers?.domainId
    };
    const getWhatsappDidData: any = await getDidPurchaseWhatsappBotDao(data);
    logger.info('getWhatsappDidData', getWhatsappDidData);
    if (getWhatsappDidData && getWhatsappDidData.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getDidPurchaseDetails: getWhatsappDidData });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getDidPurchaseDetails: [] });
    }
  } catch (error) {
    logger.error('getDidPurchaseWhatsappBot catch error:', error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
};

export async function updateDidNumberPurchaseWhatsappBot(req: any, res: any) {
  try {
    const data = {
      whatsappDidPurchaseId: req?.body?.whatsappDidPurchaseId ?? null,
      type: req?.body?.type,
      yourNumber: req?.body?.yourNumber,
      domainId: req?.body?.domainId,
      whatsAppFlowId: req?.body?.whatsAppFlowId ?? null,
      queueId: req?.body?.queueId, // not needed
      outbound: req?.body?.outbound ?? 0, // not needed
      threshold: req?.body?.threshold ?? 0,
      defaultUpdated: req?.body?.defaultUpdated ?? null,
      isBot: req?.body?.isBot ?? 0,
      isBotPurchased: req?.body?.isBotPurchased ?? 0,
      supervisorId: req?.body?.supervisorId ?? null, // not needed
      knowledgeBasedId: req?.body?.knowledgeBasedId ?? 0,
      autoAssignAgent: req?.body?.autoAssignAgent ?? null,
      pid: req?.params?.pid,
    };
    const whatsappUpdateNumber: any = await updateDidNumberPurchaseWhatsappBotDao(data);
    logger.info('whatsappUpdateNumber', whatsappUpdateNumber);
    if (whatsappUpdateNumber[0][0]?.errCode === 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE?.numberUpdate,
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE?.numberNotUpdate });
    }
  } catch (err) {
    logger.error('updateDidNumberPurchaseWhatsappBot err:', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function insertUpdateWhatsappConfigHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      WhatsappConfigurationId: req?.body?.WhatsappConfigurationId ?? null,
      whatsappNumber: req?.body?.whatsappNumber ?? null,
      profilePicture: req?.body?.profilePicture ?? null,
      businessName: req?.body?.businessName ?? null,
      aboutBusiness: req?.body?.aboutBusiness ?? null,
      isBot: req?.body?.isBot ?? 0,
      whatsappFlowId: req?.body?.whatsappFlowId ?? null,
      Industry: req?.body?.Industry ?? null,
      businessAddress: req?.body?.businessAddress ?? null,
      contactEmail: req?.body?.contactEmail ?? null,
      websiteURL1: req?.body?.websiteURL1 ?? null,
      websiteURL2: req?.body?.websiteURL2 ?? null,
      isDisabled: req?.body?.isDisabled ?? null,
      domainId: auth.domainId,
      planId: req?.bod?.planId ?? null,
      isBotPurchased: req?.body?.isBotPurchased ?? null,
      isConnector: req?.body?.isConnector ?? null,
      connectorName: req?.body?.connectorName ?? null,
      storeName: req?.body?.storeName ?? null,
      callBackToggle: req?.body?.callBackToggle ?? null,
      callBackForm: req?.body?.callBackForm ?? null,
      notifyAdmin: req?.body?.notifyAdmin ?? null,
      notificationMessages: req?.body?.notificationMessages ?? null,
      companyLogo: req?.body?.companyLogo ?? null,
      businessInformation: req?.body?.businessInformation ?? null,
      statusMessage: req?.body?.statusMessage ?? null,
      address: req?.body?.address ?? null,
      emailID: req?.body?.emailID ?? null,
      customerOptIn: req?.body?.customerOptIn ?? null,
      pricing: req?.body?.pricing ?? null,
      whatsAppBusinessSolutionUseCases: req?.body?.whatsAppBusinessSolutionUseCases ?? null,
      companyProfile: req?.body?.companyProfile ?? null,
      locationOfCompanyHeadquarters: req?.body?.locationOfCompanyHeadquarters ?? null,
      KnowledgeBaseId: req?.body?.KnowledgeBaseId ?? null
    };
    const insertWhatsapp: any = await insertUpdateWhatsappConfig(data);
    logger.info('insertWhatsapp', insertWhatsapp);
    if (insertWhatsapp === 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.insertWhatsapp });
    } else {
      res.status(200).send({ statusCode: 200, message: RESPONSE.updateWhatsapp });
    }
  } catch (err) {
    logger.error('insertUpdateWhatsappConfigHandler err:', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function insertOnboardWhatsappConfigHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      WhatsappConfigurationId: req?.body?.WhatsappConfigurationId ?? null,
      whatsappNumber: req?.body?.whatsappNumber ?? null,
      profilePicture: req?.body?.profilePicture ?? null,
      businessName: req?.body?.businessName ?? null,
      aboutBusiness: req?.body?.aboutBusiness ?? null,
      isBot: req?.body?.isBot ?? 0,
      whatsappFlowId: req?.body?.whatsappFlowId ?? null,
      Industry: req?.body?.Industry ?? null,
      businessAddress: req?.body?.businessAddress ?? null,
      contactEmail: req?.body?.contactEmail ?? null,
      websiteURL1: req?.body?.websiteURL1 ?? null,
      websiteURL2: req?.body?.websiteURL2 ?? null,
      isDisabled: req?.body?.isDisabled ?? null,
      domainId: req?.body?.domainId,
      planId: req?.body?.planId ?? null,
      isBotPurchased: req?.body?.isBotPurchased ?? null,
      isConnector: req?.body?.isConnector ?? null,
      connectorName: req?.body?.connectorName ?? null,
      storeName: req?.body?.storeName ?? null,
      notificationMessages: req?.body?.notificationMessages ?? null,
      companyLogo: req?.body?.companyLogo ?? null,
      businessInformation: req?.body?.businessInformation ?? null,
      statusMessage: req?.body?.statusMessage ?? null,
      address: req?.body?.address ?? null,
      emailID: req?.body?.emailID ?? null,
      customerOptIn: req?.body?.customerOptIn ?? null,
      pricing: req?.body?.pricing ?? null,
      whatsAppBusinessSolutionUseCases: req?.body?.whatsAppBusinessSolutionUseCases ?? null,
      companyProfile: req?.body?.companyProfile ?? null,
      locationOfCompanyHeadquarters: req?.body?.locationOfCompanyHeadquarters ?? null,
      KnowledgeBaseId: req?.body?.KnowledgeBaseId ?? null
    };
    const insertWhatsapp: any = await insertUpdateWhatsappConfig(data);
    logger.info('insertWhatsapp', insertWhatsapp);
    if (insertWhatsapp === 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.insertWhatsapp });
    } else {
      res.status(200).send({ statusCode: 200, message: RESPONSE.updateWhatsapp });
    }
  } catch (err) {
    logger.error('insertOnboardWhatsappConfigHandler err:', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 */
export const getWhatsappConfigHandler = async (req: any, res: any) => {
  try {
    const data: any = {
      domainId: req?.headers?.domainId
    };
    const getWhatsappData: any = await getWhatsappConfig(data);
    logger.info('getWhatsappData', getWhatsappData);
    if (getWhatsappData && getWhatsappData.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getWhatsappDataRes: getWhatsappData });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getWhatsappDataRes: [] });
    }
  } catch (error) {
    logger.error('getWhatsappConfigHandler err:', error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
};

/**
 *
 * @param req
 * @param res
 */
export const getWhatsappCallbackConfigHandler = async (req: any, res: any) => {
  try {
    const data: any = {
      domainId: req?.params?.domainId
    };
    const getWhatsappData: any = await getWhatsappConfig(data);
    logger.info('getWhatsappData', getWhatsappData);
    if (getWhatsappData && getWhatsappData.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getWhatsappDataRes: getWhatsappData });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getWhatsappDataRes: [] });
    }
  } catch (error) {
    logger.error('getWhatsappCallbackConfigHandler err:', error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
};

/**
 *
 * @param req
 * @param res
 */
export const getWhatsappConfigDetailsHandler = async (req: any, res: any) => {
  try {
    const data: any = {
      domainId: req?.headers?.domainId,
      whatsappId: req?.params?.whatsappId ?? 0
    };
    const getWhatsappDetails: any = await getWhatsappConfigDetails(data);
    logger.info('getWhatsappDetails', getWhatsappDetails);
    if (getWhatsappDetails && getWhatsappDetails.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getWhatsappDetailsRes: getWhatsappDetails });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getWhatsappDetailsRes: [] });
    }
  } catch (error) {
    logger.error('getWhatsappConfigDetailsHandler', error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
};

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getDisableConfigDetailsHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      WhatsappConfigurationId: req?.params?.whatsappIds ?? null,
      domainId: auth.domainId,
      isDisabled: req?.body?.isDisabled ?? null
    };
    const insertIsDisable: any = await getDisableConfigDetails(data);
    logger.info('insertIsDisable', insertIsDisable);
    if (insertIsDisable) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.isDisabled });
    }
  } catch (err) {
    logger.error('getDisableConfigDetailsHandler', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getWhatsapChannelHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
      channelType: req?.body?.channelType ?? null
    };
    const getWhatsappDetails: any = await getWhatsapChannel(data);
    logger.info('getWhatsappDetails', getWhatsappDetails);
    if (getWhatsappDetails && getWhatsappDetails.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getWhatsappChannelRes: getWhatsappDetails });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getWhatsappDetailsRes: [] });
    }
  } catch (err) {
    logger.error('getWhatsapChannelHandler', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function insertWhatsappTemplateHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      templateName: req?.body?.templateName ?? null,
      templateType: req?.body?.type ?? null,
      domainId: auth.domainId,
      buttonName: req?.body?.buttonName ?? null,
      buttonUrl: req?.body?.buttonUrl ?? null,
      content: req?.body?.bodyContent ?? null,
      id: req?.body?.id ?? null,
      number: req?.body?.number ?? null,
      flowId: req?.body?.flowId ?? null
    };
    const insertWhatsapp: any = await insertWhatsappTemplate(data);
    logger.info('insertWhatsapp', insertWhatsapp);
    if (insertWhatsapp[0]?.errCode === 1) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.insertWhatsapptemplates });
    } else {
      res.status(200).send({ statusCode: 200, message: RESPONSE.updateWhatsapptemplates });
    }
  } catch (err) {
    logger.error('insertWhatsappTemplateHandler', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getEcomPurchaseFlowHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      tokenId: req?.params?.tokenId ?? null,
      domainId: req?.params?.domainId,
    };
    const getEcomDetails: any = await getEcomPurchaseFlow(data);
    logger.info('getEcomDetails', getEcomDetails);
    if (getEcomDetails && getEcomDetails.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getEcomPurchaseRes: getEcomDetails });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getEcomPurchaseRes: [] });
    }
  } catch (err) {
    logger.error('getEcomPurchaseFlowHandler', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function updateEcomPurchaseFlowHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      tokenId: req?.body?.tokenId ?? null,
      domainId: req?.body.domainId,
      isPurchased: req?.body?.isPurchased ?? 0,
      errcode: req?.body?.errcode ?? null,
      errmsg: req?.body?.errmsg ?? null
    };
    const upadateEcom: any = await updateEcomPurchaseFlow(data);
    logger.info('upadateEcom', upadateEcom);
    if (upadateEcom) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.update });
    }
  } catch (err) {
    logger.error('updateEcomPurchaseFlowHandler', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function updateEcomIsPurchaseHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      channelType: req?.body?.channelType ?? channelType?.Whatsapp,
      tokenId: req?.body?.tokenId ?? null,
      domainId: req?.body.domainId,
    };
    const upadateEcomIs: any = await updateEcomIsPurchase(data);
    logger.info('upadateEcomIs', upadateEcomIs);
    if (upadateEcomIs) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.update });
    }
  } catch (err) {
    logger.error('updateEcomIsPurchaseHandler', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function updateDispositionWhatsappPurchaseFlowHandler(req: any, res: any, done: any) {
  try {
    const getDispositin: any = await getDispositinStatus(req?.body);
    const closedDispositions = getDispositin[0]?.filter((item: { dispositionName: string; }) => item.dispositionName === 'Closed');
    const params = req?.params;
    const data: any = {
      WhatsappHistoryId: req?.body?.WhatsappHistoryId,
      domainId: req?.body?.domainId,
      dispositionId: closedDispositions[0]?.did ?? 0,
      summary: closedDispositions[0]?.dispositionName ?? null,
      isSessionClosed: 1,
      AfterEmailWorkTime: req?.body?.afterWorkTime ?? null,
      dispositionUpdatedBy: req?.body?.dispositionUpdatedBy ?? null,
      dispositionSubject: req?.body?.dispositionSubject ?? null,
    };
    const updateDispStatus: any = await updateDispositionForWhatsapp(data);
    logger.info('updateDispStatus', updateDispStatus);
    await updateDispositionBot(data);
    if (data?.isSessionClosed === 1) {
      const getSuperVisorExt: any = await getSupervisiorDetails(data?.domainId);
      getSuperVisorExt.forEach((ext: any) => {
        console.log(emitter?.emitterDispStatus, `${data?.domainId}_${ext.ext}`);
        ioredisWhatsapp.to(`${data?.domainId}_${ext.ext}`).emit(emitter.interActionClosed);
      });
    }
    const nlpData: any = {
      HistoryId: data?.WhatsappHistoryId,
      ChatDurationHistoryId: data?.ChatDurationHistoryId ?? 0,
      channelType: channelType?.Whatsapp,
      domainId: data?.domainId,
      ext: data?.ext ?? 0
    };
    const updateScore: any = await sentimentalScore(nlpData);
    if (updateScore.status === 200) {
      await updateSentimentScore(nlpData, updateScore.data);
      if (updateScore.data) {
        ioredisWhatsapp.to(`${data.domainId}`).emit(emitter?.SentimentScoreAnalytics, { data, speedometerScore: updateScore.data.sentimentalScore, sentimentalScoreUser: updateScore.data.sentimentalScoreUser, sentimentalScoreAgent: updateScore.data.sentimentalScoreAgent });
      } else {
        ioredisWhatsapp.to(`${data.domainId}`).emit(emitter?.SentimentScoreAnalytics, { data, speedometerScore: 0, sentimentalScoreUser: 0, sentimentalScoreAgent: 0 });
      }
    } else {
      ioredisWhatsapp.to(`${data.domainId}`).emit(emitter?.SentimentScoreAnalytics, { data, speedometerScore: 0, sentimentalScoreUser: 0, sentimentalScoreAgent: 0 });
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
    logger.error('updateDispositionWhatsappPurchase', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
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
    const uuid = uuidv4();
    for (const email of filesarry) {
      const url = `${configs.email_path.Email_fileGetUrl}${email.originalname}`;
      email.uuid = uuid;
      email.url = url;
      email.name = `${email.originalname}`;
      delete email.destination;
      delete email.originalname;
    }
    logger.info('filesarry', filesarry);
    res.send({ statusCode: 200, errCode: -1, message: RESPONSE?.success_message, fileList: filesarry });
  } catch (err) {
    logger.error('fileUploadController', err);
    res.send({ statusCode: 500, errCode: 1, message: RESPONSE?.internal_error });
  }
};

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function insertUpdateMetaWhatsappHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      domainId: req?.body?.domainId ?? null,
      profilePicture: req?.body?.profilePicture ?? null,
      businessName: req?.body?.businessName ?? null,
      planId: req?.body?.planId ?? null,
      isBot: req?.body?.isBot,
      isBotPurchased: 1,
      whatsappNumber: req?.body?.whatsappNumber ?? null,
      createdBy: req?.body?.createdBy ?? null,
      purchasedat: 4,
      aboutBusiness: req?.body?.aboutBusiness ?? null,
      businessAddress: req?.body?.businessAddress ?? null,
      Industry: req?.body?.Industry ?? null,
      websiteURL1: req?.body?.websiteURL1 ?? null,
      websiteURL2: req?.body?.websiteURL2 ?? null,
      isRegistered: req?.body?.isRegistered ?? null,
      is2faEnabled: req?.body?.is2faEnabled ?? null,
      isVerifiedCode: req?.body?.isVerifiedCode ?? null
    };
    const insertMeta: any = await insertUpdateMetaWhatsapp(data);
    logger.info('insertMeta', insertMeta);
    if (insertMeta[0]?.errcode === 0) {
      res.status(200).send({ statusCode: 200, message: insertMeta[0].errmsg });
    } else {
      res.status(200).send({ statusCode: 200, message: insertMeta[0].errmsg });
    }
  } catch (err) {
    logger.error('insertUpdateMetaWhatsappHandler', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getWhatsappProviderHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      calledNumber: req?.params?.whatsappNumber ?? null,
      domainId: auth?.domainId,
    };
    const getNumber: any = await getBotDetail(data);
    logger.info('getNumber', getNumber);
    if (getNumber && getNumber.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getProviderResponse: getNumber });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getProviderResponse: [] });
    }
  } catch (err) {
    logger.error('getWhatsappProviderHandler', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function insertMetaTemplateHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      customInputId: req?.body?.customInputId,
      templateName: req?.body?.templateName,
      domainId: auth?.domainId,
      customerNumber: req?.body?.customerNumber ?? 0,
      flowId: req?.body?.flowId ?? 0
    };
    const mete: any = await insertMetaTemplate(data);
    logger.info('mete', mete);
    if (mete[0]?.errcode === 0) {
      res.status(200).send({ statusCode: 200, message: mete[0]?.errmsg });
    } else {
      res.status(200).send({ statusCode: 404, message: mete[0]?.errmsg });
    }
  } catch (err) {
    logger.error('insertMetaTemplateHandler', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getMetaTemplateDetailsHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
      flowId: req?.params?.flowId
    };
    const getMetaTemplate: any = await getMetaTemplateResponse(data);
    logger.info('getMetaTemplate', getMetaTemplate);
    if (getMetaTemplate && getMetaTemplate.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getMetaTemplateResponse: getMetaTemplate });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getMetaTemplateResponse: [] });
    }
  } catch (err) {
    logger.error('getMetaTemplateDetailsHandler', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function deleteMetaTemplateHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data = {
      domainId: auth.domainId,
      customInputId: req?.params?.customInputId
    };
    const deleteFlow: any = await deleteMetaTemplate(data);
    logger.info('deleteFlow', deleteFlow);
    if (deleteFlow[0]?.errcode === 0) {
      res.status(200).send({ statusCode: 200, message: deleteFlow[0].errmsg });
    } else {
      res.status(200).send({ statusCode: 404, message: deleteFlow[0].errmsg });
    }
  } catch (err) {
    logger.error('deleteMetaTemplateHandler', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param session
 * @param record
 * @param coid
 * @param botDetails
 * @param supervisors
 * Handle Traditional Plan
 */
async function handleTraditionalFlow(session: any, record: any, coid: any, botDetails: any, message: any) {
  const agentDetails = await agentDetailsApi(session.sessionId, session.domainId);
  const agent = agentDetails?.[0];
  if (!agent) return;
  const [assignedExt, personalData]: any = await Promise.all([getAssingToApi(agent?.assignedTo), getPersonalDetails(agent?.customerNumber, agent?.domainId)]);
  if (record.IsUpdate === 0 || assignedExt?.length === 0) {
    const liveAgent = await transferChatToAvailableAgent(record);
    return connectToliveAgent(liveAgent, [record], botDetails, personalData, coid);
  }
  const assistId = message?.messageId;
  assignedExt.forEach((ext: any) => {
    console.log(RESPONSE?.appendMessage, `${agent?.domainId}_${ext?.ext}`);
    ioredisWhatsapp?.to(`${agent.domainId}_${ext.ext}`)?.emit(emitter?.receivedWhatsapp, {
      ...agent,
      agentAssist: true,
      calledPersonDetails: personalData[0],
      messageId: assistId,
    },                                                        session.sessionId);
  });
  const comDetails = await getCompanyDetails(record);
  const nlpRes: any = await AgentAssistWhatsapp(record, comDetails, assignedExt[0].ext, message?.text?.body, assistId);
  nlpRes.data = {
    ...nlpRes.data,
    channelId: record.calledNumber,
    messageId: assistId,
    timeStamp: Date.now(),
  };
  console.log(emitter?.agentAssistOmniChannel, `${record.domainId}_${assignedExt[0]?.ext}`);
  ioredisWhatsapp?.to(`${record.domainId}_${assignedExt[0]?.ext}`)?.emit(emitter?.agentAssistOmniChannel, nlpRes.data);
}

/**
 *
 * @param body
 * @param record
 * @param coid
 * @returns handle CAI
 */
async function handleCAIFlow(body: any, record: any, coid: any) {
  const [supervisors, whatsappConfig]: any = await Promise.all([getSupervisiorList(record), getWhatsappConfigData(body)]);
  supervisors.forEach((ext: any) => {
    console.log(emitter?.supervisiorActivityBot, `${record?.domainId}_${ext?.ext}`);
    ioredisWhatsapp?.to(`${record?.domainId}_${ext?.ext}`)?.emit(emitter?.supervisiorActivityBot);
  });
  const config = whatsappConfig?.[0];
  if (!config) {
    logger?.info('No WhatsApp config found for record:', record);
    return;
  }
  let mediaUrl = '';
  if (body?.type === 'audio' && body?.audio?.mime_type?.includes('audio/ogg')) {
    const media = body[body.type];
    mediaUrl = await getWhatsAppMediaUrl(media?.id, record);
  }
  console.log('body>>>>>>>>>', body);
  const comDetails = await getCompanyDetails(record);
  const bot: any = await getWhatsappBot(body.text, comDetails, config, record, coid, mediaUrl, body?.context);
  console.log('bot>>>>>', bot?.data);
  if (bot?.data?.intent === 'transfer_live_agent') {
    await updateCallbackstatus(record);
    const [personalData, liveAgent] = await Promise.all([getPersonalDetails(record?.customerNumber, record?.domainId), getUserListByExt(record, bot?.data)]);
    return connectToliveAgent(liveAgent, [record], whatsappConfig, personalData, coid);
  }
  await handleCaiBotResponse(bot, record, body);
}

/**
 *
 * @param req
 * @param res
 * @returns
 */
/* tslint:disable */
export async function insertRoleWhatsappInHandler(req: any, whatsappCcaasTable: any) {
  try {
    let currentNodeId = null;
    let nodes = null;
    let getNlpData: any = '';
    let nlpdata: any = null;
    let flow_data: any = [];
    const getWhatsappDetails = await getWhatsappConfigData(req?.body);
    if (getWhatsappDetails[0]?.whatsappFlowId) {
      const nodesString: any = await getnodesdata(getWhatsappDetails[0]?.whatsappFlowId);
      nodes = JSON.parse(nodesString);
      const startingNode = await getStartNode(nodes);
      currentNodeId = startingNode?.data?.children[0]?.childId ?? '';
    }
    let getNode = await getNextNode(whatsappCcaasTable[0]);
    const nodeDetails = await getNodeDataById(getNode[0]?.nextFlowNode, nodes);
    const parsedData = safeParseJson(nodeDetails?.data?.information?.data);
    const inputType: any = nodeDetails?.data?.information?.inputType;
    if (inputType === 14) {
      flow_data = parsedData?.getInputData?.options?.map((item: any) => item?.value) || [];
    } else if (inputType === 15 || inputType === 1) {
      flow_data = parsedData?.options?.map((item: any) => item?.value) || [];
    }
    const comDetails = await getCompanyDetails(whatsappCcaasTable[0]);
    logger.info('comDetails', comDetails);
    if (!['document', 'image', 'video', 'audio'].includes(req?.body?.type) && req?.body?.type !== 'order' && req?.body?.interactive?.type !== "list_reply" && req?.body?.interactive?.type !== "button_reply" && req?.body?.interactive?.button_reply?.id !== "100" && req?.body?.interactive?.button_reply?.id !== "200" && req?.body?.interactive?.button_reply?.id !== "10001" && req?.body?.interactive?.button_reply?.id !== "20002") {
      getNlpData = await nlpResponseData(req?.body, comDetails, getWhatsappDetails[0], whatsappCcaasTable[0], flow_data, inputType);
      nlpdata = JSON.parse(getNlpData?.data?.entity);
    }
    const getPersonalData: any = await getPersonalDetails(whatsappCcaasTable[0].customerNumber, whatsappCcaasTable[0].domainId);
    if (getNlpData?.data?.intent === 'transfer_live_agent') {
      let liveAgent = await transferChatToAvailableAgent(whatsappCcaasTable[0]);
      const getCoid = await getCustomerCoId(whatsappCcaasTable[0]);
      await connectToliveAgent(liveAgent, whatsappCcaasTable, getWhatsappDetails, getPersonalData, getCoid);
      return;
    }
    const currentNode_ref = await getNodeDataById(getNode[0].nextFlowNode, nodes);
    const getBotMessage: any = await getBotMessageRes(whatsappCcaasTable[0]);
    const getRemainder = await getRemaindeResponse(whatsappCcaasTable[0]);
    if (req?.body?.interactive?.button_reply?.title === 'Yes' && req?.body?.interactive?.button_reply?.id === "100") {  // yes
      await sendMessageCustomerCai(whatsappCcaasTable[0], getBotMessage[0]?.message);
      await sendMessageCustomerCaiMessage(whatsappCcaasTable, getBotMessage[0]?.message);
    } else if (req?.body?.interactive?.button_reply?.title === 'No' && req?.body?.interactive?.button_reply?.id === "200") { // no
      await updateNextNode(whatsappCcaasTable[0], getNode[0]?.nextFlowNode);
      return handleChatFlowType(null, whatsappCcaasTable, nodes, '', getNlpData, req);
    } else if (req?.body?.interactive?.button_reply?.title === 'Yes' && req?.body?.interactive?.button_reply?.id === '10001') { // Remainder Yes
      await updateNextNode(whatsappCcaasTable[0], getNode[0]?.nextFlowNode);
      return handleChatFlowType(null, whatsappCcaasTable, nodes, '', getNlpData, req);
    } else if (req?.body?.interactive?.button_reply?.title === 'No' && req?.body?.interactive?.button_reply?.id === '20002') { // // Remainder No
      const parsedMessages = JSON.parse(getRemainder[0].message)[4]?.message;
      await sendMessageCustomerCai(whatsappCcaasTable[0], parsedMessages);
      await sendMessageCustomerCaiMessage(whatsappCcaasTable, parsedMessages);
    } else if (currentNode_ref?.data?.information?.inputType === 16 && ['document', 'image', 'video', 'audio'].includes(req?.body.type)) {
      await uploadFilesFlow(req?.body, whatsappCcaasTable, currentNode_ref, nodes, getNlpData, req);
    } else if (req?.body?.type !== 'text' && req?.body?.type !== 'order' && req?.body?.interactive?.type !== "list_reply") {
      let nextData: any = null;
      if (currentNode_ref?.data?.children) {
        for (const child_data of currentNode_ref.data.children) {
          if (child_data?.type === req.body.text.body) {
            nextData = child_data;
            break; // Assuming you only want the first matching child_data
          }
        }
      }
      await updateNextNode(whatsappCcaasTable[0], nextData?.childId);
      handleChatFlowType(null, whatsappCcaasTable, nodes, '', getNlpData, req);
    } else if (req?.body?.type === 'order') {
      const startingNode = await getStartNodeConnecter(nodes);
      if (startingNode) {
        var connecterDatas = JSON.parse(startingNode?.data?.information?.integration_data);
      }
      const selectedProducds = req?.body?.order?.product_items;
      const updatedProducts = [];
      const getFeed: any = await getProductFeedList(whatsappCcaasTable[0]);
      for (const prod_item of selectedProducds) {
        let temp = { ...prod_item };
        for (const feed of getFeed?.response) {
          const getProducts: any = await getProductsData(whatsappCcaasTable[0], feed?.product_feed_id, feed?.product_feed_count);

          for (const item of getProducts?.response) {
            if (item?.retailer_id === prod_item?.product_retailer_id) {
              temp = { ...temp, ...item };
              updatedProducts.push(temp);
            }
          }
        }
      }
      const token: any = await createEcompurchesFlow(whatsappCcaasTable[0], connecterDatas?.connecter_data[0], JSON.stringify(updatedProducts));
      //const url = `${process.env.whatsAppWebView}?id=${token?.tokenId}&domainId=${whatsappCcaasTable[0]?.domainId}`;
      const PayNowUrl = `WhatsAppWebView?id=${token?.tokenId}&domainId=${whatsappCcaasTable[0]?.domainId}`;
      await sendButtonLinkPayNow(whatsappCcaasTable[0], PayNowUrl);
    } else if (req?.body?.type === 'interactive' && req?.body?.interactive?.type === "list_reply" && getNode[0]?.nextFlowNode?.split('_')[0] !== 'customInput') {
      const getAppointmentFlow: any = await getFlowTracking(whatsappCcaasTable[0]);
      const getAppDetails = await AppointmentInfo(getAppointmentFlow);
      const DurationType = await getAppointmentInfo(getAppointmentFlow?.appId, getAppointmentFlow?.companyId);
      const lable = await getAppointmentLable(getAppointmentFlow?.companyId);
      const ServiceName = await getAppointmentInfo(getAppointmentFlow?.appId, getAppointmentFlow?.companyId);
      if (getAppointmentFlow?.processflow === 'categories') {
        await updateCategoryId(whatsappCcaasTable[0], getAppointmentFlow, req?.body?.interactive?.list_reply);
        const getcategories: any = await getFlowTracking(whatsappCcaasTable[0]);
        const matchedCategory = ServiceName
          .filter((item: any) => item.categoryId === +(getcategories?.categoryId))
          .map((item: any) => ({
            serviceid: item.serviceid,
            service_name: item.service_name
          }));
        logger?.info("Select service", matchedCategory);
        await sendAppointmentRes(whatsappCcaasTable[0], matchedCategory, lable);
        let datas: any = {
          appointmentId: getAppointmentFlow?.appId,
          companyId: getAppointmentFlow?.companyId,
          userId: getAppointmentFlow?.userId
        }
        if (getAppDetails[0]?.consl_selection_id === 2) {
          await insertFlowTracking(whatsappCcaasTable[0], "consultant", getAppointmentFlow);
        } else {
          await insertFlowTracking(whatsappCcaasTable[0], "service", datas);
        }
      } else if (getAppointmentFlow?.processflow === 'service') {
        await updateAppointmentServiceId(whatsappCcaasTable[0], getAppointmentFlow, req?.body?.interactive?.list_reply.id);
        const getUserData = await getFlowTracking(whatsappCcaasTable[0]);
        const UserName = await getAppointmentUserName(getAppointmentFlow, getUserData);
        if (getAppDetails[0]?.consl_selection_id === 1) {
          const getUser = UserName.map((name: any) => ({ id: name?.UserId, title: name?.userName }));
          await sendAllUserName(whatsappCcaasTable[0], getUser);
        } else if (getAppDetails[0]?.consl_selection_id === 2) {
          const getConsultantDatas = await getConsultantData(getAppointmentFlow, getUserData, getAppDetails[0]);
          await updateAppointmentConsultant(whatsappCcaasTable[0], getAppointmentFlow, getConsultantDatas[0]);
        } else {
          const getUser = UserName.map((name: any) => ({ id: name?.UserId, title: name?.userName }));
          getUser?.unshift({ id: 1, title: "Any available" })
          await sendAllUserName(whatsappCcaasTable[0], getUser);
        }
        await insertFlowTracking(whatsappCcaasTable[0], "consultant", getAppointmentFlow);
      } else if (getAppointmentFlow?.processflow === 'consultant') {
        if (getAppDetails[0]?.consl_selection_id === 2) {
          await updateAppointmentServiceId(whatsappCcaasTable[0], getAppointmentFlow, req?.body?.interactive?.list_reply.id);
        }
        const getUserDatas = await getFlowTracking(whatsappCcaasTable[0]);
        const getLeaveDates = await getLeaveData(getUserDatas);
        const leaveDatas = getLeaveDates.filter((_l: any) => _l.is_leave === 1);
        const getConsultant = await getConsultantData(getAppointmentFlow, getUserDatas, getAppDetails[0]);
        await updateAppointmentConsultant(whatsappCcaasTable[0], getAppointmentFlow, getConsultant[0]);
        const dataRange = JSON.parse(getAppDetails[0]?.data_ranges);
        const fromDate = new Date().toISOString().split('T')[0];
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
          dateObjects.push({ id: `10 ${formattedNextDate}`, title: 'More' });
        } else {
          filteredData = dateObjects.filter((item: any) => item.title !== undefined);
        }
        logger.info("Select date", Records ? dateObjects : filteredData);
        const selectDate = Records ? dateObjects : filteredData
        await sendAppointmentDate(whatsappCcaasTable[0], selectDate);
        await insertFlowTracking(whatsappCcaasTable[0], "date", getAppointmentFlow);
      } else if (getAppointmentFlow?.processflow === "date") {
        if (req?.body?.interactive?.list_reply?.title === 'More') {
          const getUserDatas = await getFlowTracking(whatsappCcaasTable[0]);
          const getLeaveDates = await getLeaveData(getUserDatas);
          const leaveDatas = getLeaveDates.filter((_l: any) => _l.is_leave === 1);
          const dataRange = JSON.parse(getAppDetails[0]?.data_ranges);
          const fromDate = req?.body?.interactive?.list_reply?.title === 'More' ? req?.body?.interactive?.list_reply?.id?.split(' ')[1] : dataRange[0]?.fromDate;
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
            dateObjects.push({ id: `10 ${formattedNextDate}`, title: 'More' });
          } else {
            filteredData = dateObjects.filter((item: any) => item.title !== undefined);
          }
          logger.info("Select date", Records ? dateObjects : filteredData);
          const selectDate = Records ? dateObjects : filteredData
          await sendAppointmentDate(whatsappCcaasTable[0], selectDate);
          return;
        }
        await updateAppointmentdate(whatsappCcaasTable[0], getAppointmentFlow, req?.body?.interactive?.list_reply);
        if (getAppDetails[0]?.app_typeid === 1 || getAppDetails[0]?.app_typeid === 2 || getAppDetails[0]?.app_typeid === 3) {
          const durationData = JSON.parse(getAppDetails[0]?.serv_dur_pricing);
          const getDurationRes = durationData.map((d: any, index: Number) => ({ id: index, title: +(d.scheduleHours) === 1 ? `${+(d.service)} mins` : `${+(d.service / 60)} hours` }));
          await sendAppointmentOneToOne(whatsappCcaasTable[0], getDurationRes, lable);
        } else {
          const durationDetails = await getdurationDetails(getAppointmentFlow);
          const duration = durationDetails.filter((data: any) => data?.serviceid === +(getAppointmentFlow?.serviceId));
          const serviceIndex = duration.findIndex((item: any) => item.serviceid === +(getAppointmentFlow?.serviceId));
          const selectDuration = duration[0].amount_collected.split("|")[serviceIndex];
          await sendAppointmentDuration(whatsappCcaasTable[0], selectDuration, lable);
        }
        await insertFlowTracking(whatsappCcaasTable[0], "duration", getAppointmentFlow);
      } else if (getAppointmentFlow?.processflow === "duration") {
        if (req?.body?.interactive?.list_reply.title !== "More") {
          await updateAppointmentDuration(whatsappCcaasTable[0], getAppointmentFlow, req?.body?.interactive?.list_reply);
        }
        const getDurationData: any = await getFlowTracking(whatsappCcaasTable[0]);
        const calenderView: any = await getCalenderViewDetails(getDurationData, getAppDetails, DurationType, getAppointmentFlow);
        const { title, id } = req?.body?.interactive?.list_reply || {};
        let startIndex = title === 'More' ? parseInt(id, 10) || 0 : 0;
        const durationData = calenderView?.slice(startIndex, startIndex + 9)?.map((duration: any, index: number) => ({
          id: index,
          title: duration?.start
        })) || [];
        const hasMoreRecords = calenderView?.length > startIndex + 9;
        if (hasMoreRecords) {
          durationData.push({
            id: startIndex + 9,
            title: "More"
          });
        }
        logger.info("Select duration", durationData);
        if (durationData?.length === 0) {
          const text = RESPONSE?.slots
          await sendMessageCustomerCai(whatsappCcaasTable[0], text)
          await insertFlowTracking(whatsappCcaasTable[0], "date", getAppointmentFlow);
          return;
        }
        await sendbookingDuration(whatsappCcaasTable[0], durationData, lable);
        if (req?.body?.interactive?.list_reply.title !== "More" && durationData?.length > 0) {
          await insertFlowTracking(whatsappCcaasTable[0], "time", getAppointmentFlow);
        }
      } else if (getAppointmentFlow?.processflow === "time") {
        if (req?.body?.interactive?.list_reply.title === "More") {
          const getDurationData: any = await getFlowTracking(whatsappCcaasTable[0]);
          const calenderView: any = await getCalenderViewDetails(getDurationData, getAppDetails, DurationType, getAppointmentFlow);
          const { title, id } = req?.body?.interactive?.list_reply || {};
          let startIndex = title === 'More' ? parseInt(id, 10) || 0 : 0;
          const durationData = calenderView?.slice(startIndex, startIndex + 9)?.map((duration: any, index: number) => ({
            id: index,
            title: duration?.start
          })) || [];
          const hasMoreRecords = calenderView?.length > startIndex + 9;
          if (hasMoreRecords) {
            durationData.push({
              id: startIndex + 9,
              title: "More"
            });
          }
          logger.info("Select duration", durationData);
          await sendbookingDuration(whatsappCcaasTable[0], durationData, lable);
          return;
        }
        await updateAppointmentTime(whatsappCcaasTable[0], getAppointmentFlow, req?.body?.interactive?.list_reply);
        const contactData = await getContactData(whatsappCcaasTable[0]);
        const getProcessTime: any = await getFlowTracking(whatsappCcaasTable[0]);
        const inputData = [
          { "First name": getProcessTime?.firstName },
          { "Last name": getProcessTime?.lastName },
          { "Phone number": getProcessTime?.phoneNumber },
          { "Email": getProcessTime?.emailId }
        ];
        const endTimeData = await getEndTimeData(getProcessTime, getAppDetails, DurationType, getAppointmentFlow);
        let typeValue: any;
        if (
          getAppDetails[0]?.app_typeid === 1 ||
          getAppDetails[0]?.app_typeid === 2 ||
          getAppDetails[0]?.app_typeid === 3
        ) {
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
        const getPaymentData = await AppointmentInfoPaymentCheck(getAppointmentFlow);
        const FilterPayment = getPaymentData[1]?.filter((data: any) => data?.serviceid === +(getProcessTime?.serviceId));
        const pricingData = getPaymentData[0][0]?.app_typeid === 4 ? FilterPayment[0]?.is_enable_price === 1 : getAppDetails[0]?.is_enable_pricing === 1;
        if (pricingData) {
          await bookingPayment(inputData, getProcessTime, getAppDetails, endTimeData?.endTime, typeValue, whatsappCcaasTable[0], FilterPayment[0]);
        } else {
          const getBooking = await createBooking(inputData, getProcessTime, getAppDetails, endTimeData?.endTime, typeValue);
          logger.info("Appointment Booking successfully", getBooking);
          if (getBooking[0]?.errcode === 0) {
            if (getAppDetails[0]?.app_typeid === 1 || getAppDetails[0]?.app_typeid === 2 || getAppDetails[0]?.app_typeid === 3) {
              await sendBookingDetailsOneToOne(whatsappCcaasTable[0], getBooking[0]);
            } else {
              await sendBookingDetails(whatsappCcaasTable[0], getBooking[0]);
            }
          } else {
            const getDurationData: any = await getFlowTracking(whatsappCcaasTable[0]);
            const calenderView: any = await getCalenderViewDetails(getDurationData, getAppDetails, DurationType, getAppointmentFlow);
            const getDuration = calenderView.slice(0, 9).map((duration: any, index: number) => ({
              id: index,
              title: duration?.start
            }));
            await sendbookingDuration(whatsappCcaasTable[0], getDuration, lable);
            await insertFlowTracking(whatsappCcaasTable[0], "time", getAppointmentFlow);
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
        const getCustomerdata = await insertCustomerContact(req.body, null);
        matches = getCustomerdata[0]?.errcode !== 1;
      } else if (trigger[0]?.triggerKey[0] === "5") {
        const getCustomerdata = await insertCustomerContact(req.body, null);
        matches = getCustomerdata[0]?.errcode === 1;
      }
      if (matches || trigger?.length === 0) {
        const startingNode = await getStartNode(nodes);
        const currentNodeId = startingNode?.data?.children[0]?.childId ?? '';
        await updateNextNode(whatsappCcaasTable[0], currentNodeId);
        return handleChatFlowType(currentNodeId, whatsappCcaasTable, nodes, '', getNlpData, req.body);
      } else if ((getNlpData?.data?.ecommerce_intent === "Product Purchase" || getNlpData?.data?.ecommerce_intent === "Product Enquiry") && getNlpData?.data?.ecommerce_product && Object.keys(getNlpData?.data?.ecommerce_product).length > 0) {
        const startingNode = await getStartNodeConnecter(nodes);
        const parseData = JSON.parse(startingNode?.data?.information?.integration_data);
        if (parseData?.connector === 13) {
          if (!Object.keys(getNlpData?.data?.ecommerce_product)?.length && getNlpData?.data?.ecommerce_intent !== "others") {
            await sendMessageCustomerCallbacks(whatsappCcaasTable[0]);
          } else if (
            (getNlpData?.data?.ecommerce_intent === "Product Purchase" || getNlpData?.data?.ecommerce_intent === "Product Enquiry") &&
            getNlpData?.data?.ecommerce_product &&
            Object.keys(getNlpData?.data?.ecommerce_product).length > 0
          ) {
            await sendCatalogues(whatsappCcaasTable[0], getNlpData?.data, req?.body?.customerName);
          }
        } else {
          await sendMessageCustomerCai(whatsappCcaasTable[0], getNlpData?.data?.message);
        }
      } else if (getNlpData?.data?.intent === 'greetings') {
        await sendMessageCustomerCai(whatsappCcaasTable[0], getNlpData?.data?.message);
        sendMessageCustomerCaiMessage(whatsappCcaasTable, getNlpData?.data?.message);
      }
      if (isArry?.length === 0 && getNlpData?.data?.flow_key === 'query' && currentNode_ref?.data?.children.length !== 0) {
        const getBotMessage: any = await getBotMessageRes(whatsappCcaasTable[0]);
        const getRemainder = await getRemaindeResponse(whatsappCcaasTable[0]);
        const remainderMessage = getRemainder[0]?.message;
        const jsonData = remainderMessage ? JSON.parse(remainderMessage)?.length > 3 : false;
        if (jsonData && getRemainder[0]?.isReminder === 0) {
          await sendFlowBreakInputRemainder(whatsappCcaasTable[0]);
          await insertBotMessage(whatsappCcaasTable[0], getNlpData?.data?.message);
          await insertBotMessageIsRemainder(whatsappCcaasTable[0]);
          sendFlowBreakMessageRemainder(whatsappCcaasTable);
        } else {
          if (getBotMessage?.length > 0) {
            await sendMessageCustomerCai(whatsappCcaasTable[0], getNlpData?.data?.message);
            if (getNode[0]?.nextFlowNode !== null) {
              await insertBotMessageRemainderWhatsapp(whatsappCcaasTable[0], getNlpData?.data?.message);
            }
            sendMessageCustomerCaiMessage(whatsappCcaasTable, getNlpData?.data?.message);
          } else {
            if (getNode[0].nextFlowNode !== null) {
              await sendFlowBreakInput(whatsappCcaasTable[0]);
              await insertBotMessage(whatsappCcaasTable[0], getNlpData?.data?.message);
              sendFlowBreakMessage(whatsappCcaasTable);
              await insertBotMessageRemainderWhatsapp(whatsappCcaasTable[0], getNlpData?.data?.message);
            } else {
              await sendMessageCustomerCai(whatsappCcaasTable[0], getNlpData?.data?.message);
              sendMessageCustomerCaiMessage(whatsappCcaasTable, getNlpData?.data?.message);
            }
          }
        }
        return;
      } else if (isArry?.length > 0) {
        const FlowData: any = await getQuestionsAns(whatsappCcaasTable[0], getWhatsappDetails[0], isArry[0]);
        await updateNextNode(whatsappCcaasTable[0], FlowData[0]?.mapFlow);
        return handleChatFlowType(null, whatsappCcaasTable, nodes, '', getNlpData, req);
      } else {
        await sendMessageCustomerCai(whatsappCcaasTable[0], getNlpData?.data?.message);
        sendMessageCustomerCaiMessage(whatsappCcaasTable, getNlpData?.data?.message);
      }
    } else {
      const triggerData = nodes[0]?.data?.information?.trigger_data;
      const trigger = typeof triggerData === 'string' ? JSON.parse(triggerData) : [];
      await triggerDataHandler(trigger, req, nodes, whatsappCcaasTable, getNlpData);
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
        await updateNextNode(whatsappCcaasTable[0], checkData?.id);
        return handleChatFlowType(null, whatsappCcaasTable, nodes, req?.body?.text?.body, getNlpData, req);
      }
      const currentNodeData = await getNodeDataById(getNode[0].nextFlowNode, nodes);
      const json = isJson(getNlpData?.data?.intent) ? JSON.parse(getNlpData?.data?.intent) : getNlpData?.data?.intent;
      const isArry = Array.isArray(json) ? json : [];
      if (isArry?.length === 0 && getNlpData?.data?.flow_key === 'query' && currentNodeData?.data?.children.length !== 0) {
        const getBotMessage: any = await getBotMessageRes(whatsappCcaasTable[0]);
        const getRemainder = await getRemaindeResponse(whatsappCcaasTable[0]);
        const remainderMessage = getRemainder[0]?.message;
        const jsonData = remainderMessage ? JSON.parse(remainderMessage)?.length > 3 : false;
        if (jsonData && getRemainder[0]?.isReminder === 0) {
          await sendFlowBreakInputRemainder(whatsappCcaasTable[0]);
          await insertBotMessage(whatsappCcaasTable[0], getNlpData?.data?.message);
          await insertBotMessageRemainderWhatsapp(whatsappCcaasTable[0], getNlpData?.data?.message);
          await insertBotMessageIsRemainder(whatsappCcaasTable[0]);
          sendFlowBreakMessageRemainder(whatsappCcaasTable);
        } else {
          if (getBotMessage?.length > 0) {
            await sendMessageCustomerCai(whatsappCcaasTable[0], getNlpData?.data?.message);
            if (getNode[0]?.nextFlowNode !== null) {
              await insertBotMessageRemainderWhatsapp(whatsappCcaasTable[0], getNlpData?.data?.message);
            }
            sendMessageCustomerCaiMessage(whatsappCcaasTable, getNlpData?.data?.message);
          } else {
            if (getNode[0].nextFlowNode !== null) {
              await sendFlowBreakInput(whatsappCcaasTable[0]);
              await insertBotMessage(whatsappCcaasTable[0], getNlpData?.data?.message);
              sendFlowBreakMessage(whatsappCcaasTable);
              await insertBotMessageRemainderWhatsapp(whatsappCcaasTable[0], getNlpData?.data?.message);
            } else {
              await sendMessageCustomerCai(whatsappCcaasTable[0], getNlpData?.data?.message);
              sendMessageCustomerCaiMessage(whatsappCcaasTable, getNlpData?.data?.message);
            }
          }
        }
        return;
      } else if (isArry?.length > 0) {
        const FlowData: any = await getQuestionsAns(whatsappCcaasTable[0], getWhatsappDetails[0], isArry[0]);
        await updateNextNode(whatsappCcaasTable[0], FlowData[0]?.mapFlow);
        return handleChatFlowType(null, whatsappCcaasTable, nodes, '', getNlpData, req);
      }
      const currentNode_ref = await getNodeDataById(getNode[0].nextFlowNode, nodes);
      if (getNode[0].nextFlowNode.split('_')[0] === 'customInput' && currentNode_ref?.data?.information?.inputType === 5) {
        const saveContactPhoneNumber = JSON.parse(currentNode_ref?.data?.information?.data);
        const getNoOfRepeats: any = await getNoOfRepeatsRes(whatsappCcaasTable[0]);
        if (getNoOfRepeats?.noOfRepeats === 0 || getNoOfRepeats?.noOfRepeats === null) {
          await updateNoOfRepeats(whatsappCcaasTable[0], parseInt(saveContactPhoneNumber?.getInputData?.noOfRepeat) + 1);
        }
        const isValid = nlpdata?.mobile_number?.length > 0;
        if (isValid) {
          if (saveContactPhoneNumber?.getInputData?.answerSave) {

            try {
              const body = { phone: req?.body?.from?.replace(/\D/g, ''), domainId: whatsappCcaasTable?.[0]?.domainId }
              insertUpdateCrmContact(body); // crm insert
            } catch (error) {
              logger.error('insertUpdateCrmContact ::: error:', error);
            }

            await insertCustomerContact(req.body, null);
            await updatePhNumber(whatsappCcaasTable[0], req?.body?.text);
            const jsonData = {
              key: saveContactPhoneNumber?.getInputData?.contactSave,
              value: req?.body?.text?.body
            };
            const jsonDataList = JSON.stringify(jsonData);
            await updateConnectData(jsonDataList, whatsappCcaasTable[0], getWhatsappDetails);
          }
          await updateNoOfRepeats(whatsappCcaasTable[0], 0);
        } else {
          const getNoOfRepeat: any = await getNoOfRepeatsRes(whatsappCcaasTable[0]);
          if (getNoOfRepeat?.noOfRepeats !== 0) {
            await updateNoOfRepeats(whatsappCcaasTable[0], getNoOfRepeat?.noOfRepeats - 1);
          }
          if (getNoOfRepeat?.noOfRepeats - 1 !== 0) {
            const errorMessage = saveContactPhoneNumber?.getInputData?.errorMessage === '' ? RESPONSE?.customerInputError : saveContactPhoneNumber?.getInputData?.errorMessage;
            sendFlowMessageData(whatsappCcaasTable, errorMessage);
            await sendMessageCustomer(whatsappCcaasTable[0], errorMessage);
            return;
          } else {
            const nextData = currentNode_ref?.data?.children[1];
            await updateNextNode(whatsappCcaasTable[0], nextData?.childId);
            return handleChatFlowType(null, whatsappCcaasTable, nodes, req?.body?.text?.body, getNlpData, req);
          }
        }
        const nextData = currentNode_ref?.data?.children[0];
        await updateNextNode(whatsappCcaasTable[0], nextData?.childId);
        return handleChatFlowType(null, whatsappCcaasTable, nodes, req?.body?.text?.body, getNlpData, req);
      } else if (getNode[0].nextFlowNode.split('_')[0] === 'customInput' && currentNode_ref?.data?.information?.inputType === 6) {
        const saveContactEmail = JSON.parse(currentNode_ref?.data?.information?.data);
        const getNoOfRepeats: any = await getNoOfRepeatsRes(whatsappCcaasTable[0]);
        if (getNoOfRepeats?.noOfRepeats === 0 || getNoOfRepeats?.noOfRepeats === null) {
          await updateNoOfRepeats(whatsappCcaasTable[0], parseInt(saveContactEmail?.getInputData?.noOfRepeat) + 1);
        }
        const dynamicPattern = req?.body?.text?.body;
        const checkEmail = nlpdata?.email?.length > 0;
        if (checkEmail) {
          if (saveContactEmail?.getInputData?.answerSave) {
            try {
              const body = { phone: req?.body?.from?.replace(/\D/g, ''), email: checkEmail, domainId: whatsappCcaasTable?.[0]?.domainId }
              insertUpdateCrmContact(body); // crm insert
            } catch (error) {
              logger.error('insertUpdateCrmContact ::: error:', error);
            }
            await insertCustomerContact(req.body, dynamicPattern);
            await updateEmailid(whatsappCcaasTable[0], req?.body?.text);
            const jsonData = {
              key: saveContactEmail?.getInputData?.contactSave,
              value: req?.body?.text?.body
            };
            const jsonDataList = JSON.stringify(jsonData);
            await updateConnectData(jsonDataList, whatsappCcaasTable[0], getWhatsappDetails);
          }
          await updateNoOfRepeats(whatsappCcaasTable[0], 0);
        } else {
          const getNoOfRepeat: any = await getNoOfRepeatsRes(whatsappCcaasTable[0]);
          if (getNoOfRepeat?.noOfRepeats !== 0) {
            await updateNoOfRepeats(whatsappCcaasTable[0], getNoOfRepeat?.noOfRepeats - 1);
          }
          if (getNoOfRepeat?.noOfRepeats - 1 !== 0) {
            const errorMessage = saveContactEmail?.getInputData?.errorMessage === '' ? RESPONSE?.customerInputError : saveContactEmail?.getInputData?.errorMessage;
            sendFlowMessageData(whatsappCcaasTable, errorMessage);
            await sendMessageCustomer(whatsappCcaasTable[0], errorMessage);
            return;
          } else {
            const nextData = currentNode_ref?.data?.children[1];
            await updateNextNode(whatsappCcaasTable[0], nextData?.childId);
            return handleChatFlowType(null, whatsappCcaasTable, nodes, req?.body?.text?.body, getNlpData, req);
          }
        }
        const nextData = currentNode_ref?.data?.children[0];
        await updateNextNode(whatsappCcaasTable[0], nextData?.childId);
        return handleChatFlowType(null, whatsappCcaasTable, nodes, req?.body?.text?.body, getNlpData, req);
      } else if (getNode[0].nextFlowNode.split('_')[0] === 'customInput' && currentNode_ref?.data?.information?.inputType === 2) {
        const saveText = JSON.parse(currentNode_ref?.data?.information?.data);
        if (saveText?.getInputData?.answerSave) {
          const jsonData = {
            key: saveText?.getInputData?.contactSave,
            value: req?.body?.text?.body
          };
          const jsonDataList = JSON.stringify(jsonData);
          await updateConnectData(jsonDataList, whatsappCcaasTable[0], getWhatsappDetails);
        }
        const nextData = currentNode_ref?.data?.children[0];
        await updateNextNode(whatsappCcaasTable[0], nextData?.childId);
        return handleChatFlowType(null, whatsappCcaasTable, nodes, req?.body?.text?.body, getNlpData, req);
      } else if (getNode[0].nextFlowNode.split('_')[0] === 'customInput' && currentNode_ref?.data?.information?.inputType === 7) {
        const saveNumber = JSON.parse(currentNode_ref?.data?.information?.data);
        const getNoOfRepeats: any = await getNoOfRepeatsRes(whatsappCcaasTable[0]);
        if (getNoOfRepeats?.noOfRepeats === 0 || getNoOfRepeats?.noOfRepeats === null) {
          await updateNoOfRepeats(whatsappCcaasTable[0], parseInt(saveNumber?.getInputData?.noOfRepeat) + 1);
        }
        const isNumber = nlpdata?.count?.length > 0;
        if (isNumber) {
          if (saveNumber?.getInputData?.answerSave) {
            const jsonData = {
              key: saveNumber?.getInputData?.contactSave,
              value: req?.body?.text?.body
            };
            const jsonDataList = JSON.stringify(jsonData);
            await updateConnectData(jsonDataList, whatsappCcaasTable[0], getWhatsappDetails);
          }
          await updateNoOfRepeats(whatsappCcaasTable[0], 0);
        } else {
          const getNoOfRepeat: any = await getNoOfRepeatsRes(whatsappCcaasTable[0]);
          if (getNoOfRepeat?.noOfRepeats !== 0) {
            await updateNoOfRepeats(whatsappCcaasTable[0], getNoOfRepeat?.noOfRepeats - 1);
          }
          if (getNoOfRepeat?.noOfRepeats - 1 !== 0) {
            const errorMessage = saveNumber?.getInputData?.errorMessage === '' ? RESPONSE?.customerInputError : saveNumber?.getInputData?.errorMessage;
            await sendFlowMessageData(whatsappCcaasTable, errorMessage);
            await sendMessageCustomer(whatsappCcaasTable[0], errorMessage);
            return;
          } else {
            const nextData = currentNode_ref?.data?.children[1];
            await updateNextNode(whatsappCcaasTable[0], nextData?.childId);
            return handleChatFlowType(null, whatsappCcaasTable, nodes, req?.body?.text?.body, getNlpData, req);
          }
        }
        const nextData = currentNode_ref?.data?.children[0];
        await updateNextNode(whatsappCcaasTable[0], nextData?.childId);
        return handleChatFlowType(null, whatsappCcaasTable, nodes, req?.body?.text?.body, getNlpData, req.body);
      } else if (getNode[0].nextFlowNode.split('_')[0] === 'customInput' && currentNode_ref?.data?.information?.inputType === 8) {
        const saveUrl = JSON.parse(currentNode_ref?.data?.information?.data);
        const getNoOfRepeats: any = await getNoOfRepeatsRes(whatsappCcaasTable[0]);
        if (getNoOfRepeats?.noOfRepeats === 0 || getNoOfRepeats?.noOfRepeats === null) {
          await updateNoOfRepeats(whatsappCcaasTable[0], parseInt(saveUrl?.getInputData?.noOfRepeat) + 1);
        }
        const isUrl = nlpdata?.website?.length > 0;
        if (isUrl) {
          if (saveUrl?.getInputData?.answerSave) {
            const jsonData = {
              key: saveUrl?.getInputData?.contactSave,
              value: req?.body?.text?.body
            };
            const jsonDataList = JSON.stringify(jsonData);
            await updateConnectData(jsonDataList, whatsappCcaasTable[0], getWhatsappDetails);
          }
          await updateNoOfRepeats(whatsappCcaasTable[0], 0);
        } else {
          const getNoOfRepeat: any = await getNoOfRepeatsRes(whatsappCcaasTable[0]);
          if (getNoOfRepeat?.noOfRepeats !== 0) {
            await updateNoOfRepeats(whatsappCcaasTable[0], getNoOfRepeat?.noOfRepeats - 1);
          }
          if (getNoOfRepeat?.noOfRepeats - 1 !== 0) {
            const errorMessage = saveUrl?.getInputData?.errorMessage === '' ? RESPONSE?.customerInputUrl : saveUrl?.getInputData?.errorMessage;
            await sendFlowMessageData(whatsappCcaasTable, errorMessage);
            await sendMessageCustomer(whatsappCcaasTable[0], errorMessage);
            return;
          } else {
            const nextData = currentNode_ref?.data?.children[1];
            await updateNextNode(whatsappCcaasTable[0], nextData?.childId);
            return handleChatFlowType(null, whatsappCcaasTable, nodes, req?.body?.text?.body, getNlpData, req);
          }
        }
        const nextData = currentNode_ref?.data?.children[0];
        await updateNextNode(whatsappCcaasTable[0], nextData?.childId);
        return handleChatFlowType(null, whatsappCcaasTable, nodes, req?.body?.text?.body, getNlpData, req);
      } else if (getNode[0].nextFlowNode.split('_')[0] === 'customInput' && currentNode_ref?.data?.information?.inputType === 9) {
        const saveContactName = JSON.parse(currentNode_ref?.data?.information?.data);
        const getNoOfRepeats: any = await getNoOfRepeatsRes(whatsappCcaasTable[0]);
        if (getNoOfRepeats?.noOfRepeats === 0 || getNoOfRepeats?.noOfRepeats === null) {
          await updateNoOfRepeats(whatsappCcaasTable[0], parseInt(saveContactName?.getInputData?.noOfRepeat) + 1);
        }
        const isName = (nlpdata?.first_name?.length > 0) || (nlpdata?.last_name?.length > 0);
        if (isName) {
          if (saveContactName?.getInputData?.answerSave) {
            if (saveContactName?.getInputData?.contactSave === 'firstName') {
              await updateCustomerFirstName(req.body);
              await updateCustomerFirstNameAppoinment(whatsappCcaasTable[0], req?.body?.text);

              try {
                const body = { name: req?.body?.text?.body ?? "", phone: req?.body?.from?.replace(/\D/g, '') ?? "", domainId: whatsappCcaasTable?.[0]?.domainId }
                insertUpdateCrmContact(body); // crm insert
              } catch (error) {
                logger.error('insertUpdateCrmContact ::: error:', error);
              }

            } else if (saveContactName?.getInputData?.contactSave === 'lastName') {
              await updateCustomerLastName(req.body);
              await updateCustomerLastNameAppoinment(whatsappCcaasTable[0], req?.body?.text);
            }
            const jsonData = {
              key: saveContactName?.getInputData?.contactSave,
              value: req?.body?.text?.body
            };
            const jsonDataList = JSON.stringify(jsonData);
            await updateConnectData(jsonDataList, whatsappCcaasTable[0], getWhatsappDetails);
          }
          await updateNoOfRepeats(whatsappCcaasTable[0], 0);
        } else {
          const getNoOfRepeat: any = await getNoOfRepeatsRes(whatsappCcaasTable[0]);
          if (getNoOfRepeat?.noOfRepeats !== 0) {
            await updateNoOfRepeats(whatsappCcaasTable[0], getNoOfRepeat?.noOfRepeats - 1);
          }
          if (getNoOfRepeat?.noOfRepeats - 1 !== 0) {
            const errorMessage = saveContactName?.getInputData?.errorMessage === '' ? RESPONSE?.customerInputError : saveContactName?.getInputData?.errorMessage;
            await sendFlowMessageData(whatsappCcaasTable, errorMessage);
            await sendMessageCustomer(whatsappCcaasTable[0], errorMessage);
            return;
          } else {
            const nextData = currentNode_ref?.data?.children[1];
            await updateNextNode(whatsappCcaasTable[0], nextData?.childId);
            return handleChatFlowType(null, whatsappCcaasTable, nodes, req?.body?.text?.body, getNlpData, req);
          }
        }
        const nextData = currentNode_ref?.data?.children[0];
        await updateNextNode(whatsappCcaasTable[0], nextData?.childId);
        return handleChatFlowType(null, whatsappCcaasTable, nodes, req?.body?.text?.body, getNlpData, req);
      } else if (getNode[0].nextFlowNode.split('_')[0] === 'customInput' && currentNode_ref?.data?.information?.inputType === 14) {
        const nextData = currentNode_ref?.data?.children;
        const selectedTitle = req?.body?.interactive?.list_reply?.title;
        const match = nextData.find((option: any) => option?.type === selectedTitle);
        const childId = match?.childId;
        await updateNextNode(whatsappCcaasTable[0], childId);
        return handleChatFlowType(null, whatsappCcaasTable, nodes, req?.body?.text?.body, getNlpData, req);
      } else {
        const triggerData = nodes[0]?.data?.information?.trigger_data;
        const trigger = typeof triggerData === 'string' ? JSON.parse(triggerData) : [];
        await triggerDataSendMessageHandler(trigger, req, nodes, whatsappCcaasTable, getNlpData);
      }
    }
  } catch (err) {
    logger.info("insertRoleWhatsappInHandler err", err);
  }
}
async function handleChatFlowType(nodeId: any, whatsappCcaasTable: any, nodes: any, input: any, getNlpData: any, req: any) {
  const getWhatsapp = await getWhatsappConfigConnceter(whatsappCcaasTable[0]);
  const getNode = await getNextNode(whatsappCcaasTable[0]);
  let currentNode = null;
  let custom_input_update = false;
  if (getNode[0]?.nextFlowNode === null) {
    currentNode = await getNodeDataById(nodeId, nodes);
    if (nodeId !== null && nodeId.split('_')[0] === 'customInput') {
      custom_input_update = true;
    }
  } else {
    currentNode = await getNodeDataById(getNode[0].nextFlowNode, nodes);
  }
  logger.info('currentNodeType >>>>', currentNode?.type);
  switch (currentNode?.type) {
    case 'entryPoint': {
      if (currentNode && currentNode.data && currentNode.data.children && currentNode.data.children.at(0)) {
        const childId = currentNode.data.children.at(0).childId;
        await updateNextNode(whatsappCcaasTable[0], childId);
        return handleChatFlowType(childId, whatsappCcaasTable, nodes, input, getNlpData, req);
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
          temp = temp.replace(/<strong>/g, '*');
          temp = temp.replace(/<\/strong>/g, '*');
          temp = temp.replace("<em>", "_");
          temp = temp.replace("</em>", "_");
          temp = temp.replace("<s>", "~");
          temp = temp.replace("</s>", "~");
          temp = temp.replace(/<ol>/g, "\n").replace(/<\/ol>/g, "");
          temp = temp.replace(/<li>/g, () => `${counter++}. `).replace(/<\/li>/g, "\n");
        }
        temp = temp.replace(/&nbsp;/g, " ");
        const textData = temp?.replace(/<[^>]+>/g, '');
        const filesData: any = targetData[1];
        targetData?.filter((row: any) => row?.type === 'text');
        if (targetData?.length === 1 && targetData[0]?.type === 'text') {
          await sendMessageCustomerFlow(whatsappCcaasTable[0], textData);
        } else if (targetData?.length > 1 && targetData[1]?.type === 'image') {
          await sendImageCustomer(whatsappCcaasTable[0], textData, targetData[1]);
        } else if (targetData?.length > 1 && targetData[1]?.type === 'file' && targetData[1]?.mimetype === 'application/pdf') {
          await sendMediaCustomer(whatsappCcaasTable[0], textData, targetData[1], "document/pdf");
        } else if (targetData?.length > 1 && targetData[1]?.type === 'file' && targetData[1]?.mimetype === 'video/mp4') {
          await sendMediaCustomer(whatsappCcaasTable[0], textData, targetData[1], "video/mp4");
        } else if (targetData?.length > 1 && targetData[1]?.type === 'file' && targetData[1]?.mimetype === 'audio/mpeg') {
          await sendMediaCustomer(whatsappCcaasTable[0], textData, targetData[1], "audio/mp3");
        }
        const nextNode = currentNode?.data?.children?.find((nodes1: any) => nodes1?.type === 'success') ?? { childId: '' };
        let message: any = {};
        try {
          message = {
            messageID: uuidv4(),
            sessionId: whatsappCcaasTable[0]?.sessionId,
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
          stringifiedMessage.WhatsappHistoryId = whatsappCcaasTable[0]?.WhatsappHistoryId;
          await updateChatMessage(stringifiedMessage);
          updateIsBot(whatsappCcaasTable[0]?.WhatsappHistoryId);
          await updateNextNode(whatsappCcaasTable[0], nextNode?.childId);
          return await handleChatFlowType(nextNode, whatsappCcaasTable, nodes, input, getNlpData, req);
        } catch (error) {
          console.log(error);
          const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
          // return handleChatFlowType(nextNode?.childId ?? null, whatsappCcaasTable[0] ?? null, node, io);
        }
        return await handleChatFlowType(nextNode, whatsappCcaasTable, nodes, input, getNlpData, req);
      } catch (error) {
        const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
        return await handleChatFlowType(nextNode, whatsappCcaasTable, nodes, input, getNlpData, req);
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
            await sendCustomerInput(whatsappCcaasTable[0], parseData);
          } else if (parseData?.menuType === 'button') {
            const nextNode = currentNode?.data?.children?.find((nodes1: any) => nodes1?.childId) ?? { childId: '' };
            const getLink: any = await getWhatsappTemplate(whatsappCcaasTable[0], getWhatsapp[0]);
            await sendButtonLink(whatsappCcaasTable[0], getLink);
            let message = {};
            try {
              message = {
                messageID: uuidv4(),
                sessionId: whatsappCcaasTable[0]?.sessionId,
                message: getLink?.buttonUrl,
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
              stringifiedMessage.WhatsappHistoryId = whatsappCcaasTable[0]?.WhatsappHistoryId;
              await updateChatMessage(stringifiedMessage);
              updateIsBot(whatsappCcaasTable[0]?.WhatsappHistoryId);
              await updateNextNode(whatsappCcaasTable[0], nextNode?.childId);
              return handleChatFlowType(nextNode, whatsappCcaasTable, nodes, input, getNlpData, req);
            } catch (error) {
              logger.error('customInput', error);
            }
          }
        } else if (currentNode?.data?.information?.inputType === 5) {
          const phoneNumber = currentNode.data?.information?.message;
          await sendMessageNumber(whatsappCcaasTable[0], phoneNumber);
          let message: any = {};
          try {
            message = {
              messageID: uuidv4(),
              sessionId: whatsappCcaasTable[0]?.sessionId,
              message: phoneNumber?.replace(/<[^>]+>/g, ''),
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
            stringifiedMessage.WhatsappHistoryId = whatsappCcaasTable[0]?.WhatsappHistoryId;
            await updateChatMessage(stringifiedMessage);
            updateIsBot(whatsappCcaasTable[0]?.WhatsappHistoryId);
          } catch (error) {
            logger.error('customInput', error);
          }
        } else if (currentNode?.data?.information?.inputType === 6) {
          const emailId = currentNode.data?.information?.message;
          await sendMessageNumber(whatsappCcaasTable[0], emailId);
          let message: any = {};
          try {
            message = {
              messageID: uuidv4(),
              sessionId: whatsappCcaasTable[0]?.sessionId,
              message: emailId?.replace(/<[^>]+>/g, ''),
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
            stringifiedMessage.WhatsappHistoryId = whatsappCcaasTable[0]?.WhatsappHistoryId;
            await updateChatMessage(stringifiedMessage);
            updateIsBot(whatsappCcaasTable[0]?.WhatsappHistoryId);
          } catch (error) {
            logger.error('customInput', error);
          }
        } else if (currentNode?.data?.information?.inputType === 2) {
          const textMessage = currentNode.data?.information?.message;
          await sendMessageNumber(whatsappCcaasTable[0], textMessage);
          let message: any = {};
          try {
            message = {
              messageID: uuidv4(),
              sessionId: whatsappCcaasTable[0]?.sessionId,
              message: textMessage?.replace(/<[^>]+>/g, ''),
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
            stringifiedMessage.WhatsappHistoryId = whatsappCcaasTable[0]?.WhatsappHistoryId;
            await updateChatMessage(stringifiedMessage);
            updateIsBot(whatsappCcaasTable[0]?.WhatsappHistoryId);
          } catch (error) {
            logger.error('customInput', error);
          }
        } else if (currentNode?.data?.information?.inputType === 7) {
          const number = currentNode.data?.information?.message;
          await sendMessageNumber(whatsappCcaasTable[0], number);
          let message: any = {};
          try {
            message = {
              messageID: uuidv4(),
              sessionId: whatsappCcaasTable[0]?.sessionId,
              message: number?.replace(/<[^>]+>/g, ''),
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
            stringifiedMessage.WhatsappHistoryId = whatsappCcaasTable[0]?.WhatsappHistoryId;
            await updateChatMessage(stringifiedMessage);
            updateIsBot(whatsappCcaasTable[0]?.WhatsappHistoryId);
          } catch (error) {
            logger.error('customInput', error);
          }
        } else if (currentNode?.data?.information?.inputType === 8) {
          const url = currentNode.data?.information?.message;
          await sendMessageNumber(whatsappCcaasTable[0], url);
          let message: any = {};
          try {
            message = {
              messageID: uuidv4(),
              sessionId: whatsappCcaasTable[0]?.sessionId,
              message: url?.replace(/<[^>]+>/g, ''),
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
            stringifiedMessage.WhatsappHistoryId = whatsappCcaasTable[0]?.WhatsappHistoryId;
            await updateChatMessage(stringifiedMessage);
            updateIsBot(whatsappCcaasTable[0]?.WhatsappHistoryId);
          } catch (error) {
            logger.error('customInput', error);
          }
        } else if (currentNode?.data?.information?.inputType === 9) {
          const name = currentNode.data?.information?.message;
          await sendMessageNumber(whatsappCcaasTable[0], name);
          let message: any = {};
          try {
            message = {
              messageID: uuidv4(),
              sessionId: whatsappCcaasTable[0]?.sessionId,
              message: name?.replace(/<[^>]+>/g, ''),
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
            stringifiedMessage.WhatsappHistoryId = whatsappCcaasTable[0]?.WhatsappHistoryId;
            await updateChatMessage(stringifiedMessage);
            updateIsBot(whatsappCcaasTable[0]?.WhatsappHistoryId);
          } catch (error) {
            logger.error('customInput', error);
          }
        } else if (currentNode?.data?.information?.inputType === 16) {
          const FileMessage = currentNode.data?.information?.message;
          await sendMessageNumber(whatsappCcaasTable[0], FileMessage);
          let message: any = {};
          try {
            message = {
              messageID: uuidv4(),
              sessionId: whatsappCcaasTable[0]?.sessionId,
              message: FileMessage?.replace(/<[^>]+>/g, ''),
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
            stringifiedMessage.WhatsappHistoryId = whatsappCcaasTable[0]?.WhatsappHistoryId;
            await updateChatMessage(stringifiedMessage);
            updateIsBot(whatsappCcaasTable[0]?.WhatsappHistoryId);
          } catch (error) {
            logger.error('customInput', error);
          }
        } else if (currentNode?.data?.information?.inputType === 14) {
          const dropDownMessage = currentNode.data?.information?.message;
          const dropDownData = JSON.parse(currentNode.data?.information?.data);
          const dropDownMultipleData = dropDownData?.options?.map((item: any, index: any) => ({ id: index, title: item?.value }));
          await sendDropdownDetails(whatsappCcaasTable[0], dropDownMessage, dropDownData?.getInputData?.place_holder, dropDownMultipleData);
        }
        if (custom_input_update) {
          await updateNextNode(whatsappCcaasTable[0], nodeId);
        }
      } catch (error) {
        const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
        logger.info("customInput", error);
        return handleChatFlowType(nextNode, whatsappCcaasTable, nodes, input, getNlpData, req);
      }
      break;
    }
    case 'sendMail': {
      try {
        logger.info('sendMail>>>>>>>>>>>>>>>>>>>>>>>', currentNode);
        const templateData = JSON.parse(currentNode?.data?.information?.template_data);
        const getEmailData: any = await getCustomerEmailId(whatsappCcaasTable[0]);
        if (templateData?.type === '1') {
          await sendEmail(getEmailData, templateData);
        } else {
          const adminEmail: any = await getAdminEmail(whatsappCcaasTable[0])
          await sendEmailPlainText(getEmailData, templateData, whatsappCcaasTable[0], adminEmail);
        }
        const nextNode = currentNode?.data?.children?.find((nodes1: any) => nodes1?.type === RESPONSE?.success_message) ?? { childId: '' };
        const nextNodedetails: any = await getNodeDataById(nextNode?.childId, nodes);
        let message: any = {};
        try {
          message = {
            messageID: uuidv4(),
            sessionId: whatsappCcaasTable[0]?.sessionId,
            message: getEmailData?.type === '1' ? templateData?.data?.template_image : templateData?.message,
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
          stringifiedMessage.WhatsappHistoryId = whatsappCcaasTable[0]?.WhatsappHistoryId;
          await updateChatMessage(stringifiedMessage);
          updateIsBot(whatsappCcaasTable[0]?.WhatsappHistoryId);
          await updateNextNode(whatsappCcaasTable[0], nextNode?.childId);
          return handleChatFlowType(nextNode, whatsappCcaasTable, nodes, input, getNlpData, req);
        } catch (error) {
          const nextNodes = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
          // return handleChatFlowType(nextNode?.childId ?? null, whatsappCcaasTable[0] ?? null, node, io);
        }
        return handleChatFlowType(nextNode, whatsappCcaasTable, nodes, input, getNlpData, req);
      } catch (error) {
        const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
        return handleChatFlowType(nextNode, whatsappCcaasTable, nodes, input, getNlpData, req);
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
        const decrypted = CryptoJS.AES.decrypt(previewId?.replaceAll(" ", "+"), process.env.REACT_APP_ENCDEC_KEY).toString(CryptoJS.enc.Utf8);
        let value = decrypted?.split("-");
        let appointmentId = value[0];
        let companyId = value[1];
        let userId = value[2];
        const ServiceName = await getAppointmentInfo(appointmentId, companyId);
        const getAppointmentType = await AppointmentInfo({ appId: appointmentId, companyId: companyId });
        const getLeaveDates = await getLeaveData({ companyId: companyId, userId: userId });
        const leaveDatas = getLeaveDates.filter((_l: any) => _l.is_leave === 1);
        if (getAppointmentType[0]?.app_typeid === 1) {
          await updateAppointmentUserDetails(whatsappCcaasTable[0], appointmentId, companyId, userId, getAppointmentType[0]);
          const dataRange = JSON.parse(getAppointmentType[0]?.data_ranges);
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
            dateObjects.push({ id: `10 ${formattedNextDate}`, title: 'More' });
          } else {
            filteredData = dateObjects.filter((item: any) => item.title !== undefined);
          }
          logger.info("Select date", Records ? dateObjects : filteredData);
          const selectDate = Records ? dateObjects : filteredData;
          await sendAppointmentDate(whatsappCcaasTable[0], selectDate);
          await insertFlowTracking(whatsappCcaasTable[0], "date", { appId: appointmentId, companyId: companyId, userId: userId });
        } else if (getAppointmentType[0]?.app_typeid === 2) {
          await updateAppointmentUserDetails(whatsappCcaasTable[0], appointmentId, companyId, userId, getAppointmentType[0]);
          const dataRange = JSON.parse(getAppointmentType[0]?.data_ranges);
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
            dateObjects.push({ id: `10 ${formattedNextDate}`, title: 'More' });
          } else {
            filteredData = dateObjects.filter((item: any) => item.title !== undefined);
          }
          logger.info("Select date", Records ? dateObjects : filteredData);
          const selectDate = Records ? dateObjects : filteredData;
          await sendAppointmentDate(whatsappCcaasTable[0], selectDate);
          await insertFlowTracking(whatsappCcaasTable[0], "date", { appId: appointmentId, companyId: companyId, userId: userId });
        } else if (getAppointmentType[0]?.app_typeid === 3) {
          await updateAppointmentUserCreateBy(whatsappCcaasTable[0], appointmentId, companyId, userId, getAppointmentType[0]);
          const dataRange = JSON.parse(getAppointmentType[0]?.data_ranges);
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
            dateObjects.push({ id: `10 ${formattedNextDate}`, title: 'More' });
          } else {
            filteredData = dateObjects.filter((item: any) => item.title !== undefined);
          }
          logger.info("Select date", Records ? dateObjects : filteredData);
          const selectDate = Records ? dateObjects : filteredData;
          await sendAppointmentDate(whatsappCcaasTable[0], selectDate);
          await insertFlowTracking(whatsappCcaasTable[0], "date", { appId: appointmentId, companyId: companyId, userId: userId });
        } else {
          const categoryName = ServiceName.map((category: any) => ({ id: category?.categoryId, title: category?.categoryName }));
          const uniqueCategories = categoryName.filter((item: any, index: any, self: any) => index === self.findIndex((t: any) => t.id === item.id && t.title === item.title));
          logger.info("select category", uniqueCategories);
          await sendCategoryName(whatsappCcaasTable[0], uniqueCategories);
          let datas: any = {
            appointmentId,
            companyId,
            userId
          }
          await insertFlowTracking(whatsappCcaasTable[0], "categories", datas);
        }
      } catch (error) {
        console.log(error);
        const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
        return handleChatFlowType(nextNode, whatsappCcaasTable, nodes, input, getNlpData, req.body);
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
            const url = type === 'get' ? `${endPointUrl}${input} ` : `${endPointUrl} `;
            const method = currentNode?.data?.information?.getMethodApi ?? 'get';
            const selectedKeys: any = new Set(currentNode?.data?.information?.selectedKeyFromResponse ?? []);
            const body = currentNode?.data?.information?.apiBody;
            const headers = currentNode?.data?.information?.apiHeaders;
            const Params = currentNode?.data?.information?.apiParams;
            const apiResponse = await fetchApi({ urlEndPoint: url, urlMethod: method, urlBody: body, urlHeaders: headers, inputParams: Params });
            const targetRestructured: any = { sessionId: whatsappCcaasTable[0]?.sessionId };
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
                  await sendMessageNumber(whatsappCcaasTable[0], finalResponse);
                  const message: any = {
                    messageID: uuidv4(),
                    sessionId: whatsappCcaasTable[0]?.sessionId,
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
                  stringifiedMessage.WhatsappHistoryId = whatsappCcaasTable[0]?.WhatsappHistoryId;
                  updateIsBot(whatsappCcaasTable[0]?.WhatsappHistoryId);
                  await updateChatMessage(stringifiedMessage);
                } else if (nodeNext.childId.includes('messageFlow')) {
                  const nextNodedetails: any = await getNodeDataById(nodeNext.childId, nodes);
                  const text = nextNodedetails?.data?.information?.message;
                  const availableContexts = text?.match(contextPattern);
                  const updatedMessage = availableContexts?.length > 0 ? replacechatPatternWithVariables(availableContexts, text, _formattedSelectedKeys) : text;
                  const data = { ...nextNodedetails?.data?.information };
                  data.message = `${updatedMessage}`;
                  const targetRestructured: any = { nextNodedetails, data, sessionId: whatsappCcaasTable[0]?.sessionId };
                  targetRestructured.msg = updatedMessage;
                  const messagenextNode = nextNodedetails?.data?.children?.find((nodes: any) => nodes?.type === RESPONSE?.success_message) ?? { childId: '' };
                  let messagenextNodedetails: any = {};
                  if (messagenextNode.childId) {
                    messagenextNodedetails = await getNodeDataById(messagenextNode?.childId, nodes);
                  }
                  targetRestructured.currentNode = nextNodedetails;
                  const message: any = {
                    messageID: uuidv4(),
                    sessionId: whatsappCcaasTable[0]?.sessionId,
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
                  stringifiedMessage.WhatsappHistoryId = whatsappCcaasTable[0]?.WhatsappHistoryId;
                  await updateChatMessage(stringifiedMessage);
                  updateIsBot(whatsappCcaasTable[0]?.WhatsappHistoryId);
                  const body = {
                    id: 1,
                    from: whatsappCcaasTable[0]?.calledNumber,
                    to: `+${whatsappCcaasTable[0]?.customerNumber}`,
                    message: targetRestructured?.msg?.replace(/<[^>]+>/g, '')
                  };
                  const _cpynextNodedetails: any = { ...messagenextNodedetails };
                  targetRestructured.currentNode = _cpynextNodedetails;
                  await axios.post(process?.env?.whatsappOut, body);
                } else if (nodeNext.childId.includes('integration')) {
                  await updateNextNode(whatsappCcaasTable[0], nodeNext?.childId);
                  return handleChatFlowType(nodeNext, whatsappCcaasTable, nodes, input, getNlpData, req);
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
                  const targetRestructured: any = { nextNodedetails, data, sessionId: whatsappCcaasTable[0]?.sessionId };
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
                  stringifiedMessage.WhatsappHistoryId = whatsappCcaasTable[0]?.WhatsappHistoryId;
                  updateChatMessage(stringifiedMessage);
                  updateIsBot(whatsappCcaasTable[0]?.WhatsappHistoryId);
                  const body = {
                    id: 1,
                    from: whatsappCcaasTable[0]?.calledNumber,
                    to: `+${whatsappCcaasTable[0]?.customerNumber}`,
                    content: targetRestructured.msg,
                  };
                  await axios.post(process?.env?.whatsappOut, body);
                  const _cpynextNodedetails: any = { ...messagenextNodedetails };
                  targetRestructured.currentNode = _cpynextNodedetails;
                }
              }
            }
          } catch (error) {
            const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
            return handleChatFlowType(nextNode, whatsappCcaasTable, nodes, input, getNlpData, req);
          }
        } else if (type === 'connectors') {
          const parseData = JSON.parse(currentNode?.data?.information?.integration_data);
          const nextNode = currentNode?.data?.children?.find((nodes1: any) => nodes1?.type === RESPONSE?.success_message) ?? { childId: '' };
          if (parseData?.connector === 4) {
            let end_point = parseData?.end_point;
            const variableData = parseData?.variableData ?? null;
            const regex = /{([^{}]+)}/g;
            const matches = [];
            let match: any[];
            while ((match = regex.exec(parseData?.end_point)) !== null) {
              matches.push(match[1]);
            }
            end_point = end_point.replace(/{|}/g, '');
            const getConnectdata: any = await getconnectData(whatsappCcaasTable[0]);
            const checkArray = Array.isArray(JSON.parse(getConnectdata?.connectorData)) ? JSON.parse(getConnectdata?.connectorData) : [JSON.parse(getConnectdata?.connectorData)];
            for (let datas of checkArray) {
              end_point = end_point.replace(datas.key, datas.value);
            }
            const getInterationdata = await getIntegrationApiData(end_point, parseData?.connecter_data[0].token);
            const api_data = [];
            if (variableData !== null) {
              for (let var_data of variableData) {
                const temp_data: any = await returnVariableData(var_data, getInterationdata, false);
                if (temp_data[0] !== undefined) {
                  await updateConnectData(JSON.stringify({ "key": var_data.replace('${context.', '').replace('}', ''), "value": temp_data[0] }), whatsappCcaasTable[0], getWhatsapp)
                }
              }
            }
          } else if (parseData?.connector === 13) {
            if (parseData) {
              if (!Object.keys(getNlpData?.data?.ecommerce_product)?.length && getNlpData?.data?.ecommerce_intent !== "others") {
                await sendMessageCustomerCallbacks(whatsappCcaasTable[0]);
              } else if ((getNlpData?.data?.ecommerce_intent === "Product Purchase" || getNlpData?.data?.ecommerce_intent === "Product Enquiry") && getNlpData?.data?.ecommerce_product && Object.keys(getNlpData?.data?.ecommerce_product).length > 0) {
                await sendCatalogues(whatsappCcaasTable[0], getNlpData?.data, req?.body?.customerName);
                // await createEcompurchesFlow(whatsappCcaasTable[0], 1, JSON.stringify({ value: "ecomesInitiated" })
                // );
              }
            }
          }
        }
      } catch (error: any) {
        const nextNode = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'error') ?? { childId: '' };
        return handleChatFlowType(nextNode, whatsappCcaasTable, nodes, input, getNlpData, req);
      }
      break;
    }
    case 'payment': {
      try {
        logger.info('payment>>>>>>>>>>>>>>>>>>>>>>>', currentNode);
        const paymentData = currentNode?.data?.information;
        const nextNode = currentNode?.data?.children?.find((nodes1: any) => nodes1?.type === RESPONSE?.success_message) ?? { childId: '' };
        await updateNextNode(whatsappCcaasTable[0], nextNode?.childId);
        return handleChatFlowType(nextNode, whatsappCcaasTable, nodes, input, getNlpData, req);
      } catch (error) {
        console.log(error);
        const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
        return handleChatFlowType(nextNode, whatsappCcaasTable, nodes, input, getNlpData, req);
      }
      break;
    }
    case 'transferToQueue': {
      try {
        logger.info('transferToQueue>>>>>>>>>>>>>>>>>>', currentNode);
        const getBotDetails: any = await getBotDetail(whatsappCcaasTable[0]);
        const getPersonalData: any = await getPersonalDetails(whatsappCcaasTable[0].customerNumber, whatsappCcaasTable[0].domainId);
        const getCoid = await getCustomerCoId(whatsappCcaasTable[0]);
        if (currentNode?.data?.information?.type === 1) {
          insertCustomerInQueue(whatsappCcaasTable[0], currentNode, currentNode?.data?.information?.transferTo);
          const agentDetailRes: any = await transferChat(whatsappCcaasTable[0], currentNode);
          const agentDetails = agentDetailRes ? [agentDetailRes] : [];
          try {
            if (Array.isArray(agentDetails) && agentDetails?.length > 0) {
              console.log(emitter?.invite, `${agentDetails[0]?.domainId}_${agentDetails[0]?.ext}`);
              ioredisWhatsapp.to(`${agentDetails[0]?.domainId}_${agentDetails[0]?.ext}`).emit(emitter?.invite, {
                channelType: whatsappCcaasTable[0].channelType, customerNumber: whatsappCcaasTable[0].customerNumber,
                agentNumber: whatsappCcaasTable[0].calledNumber, sessionId: whatsappCcaasTable[0].sessionId, historyId: whatsappCcaasTable[0]?.WhatsappHistoryId,
                calledPersonDetails: getPersonalData[0] ?? null,
                allAgent: true, WhatsappDurationHistoryId: whatsappCcaasTable[0]?.WhatsappDurationHistoryId,
                isTransferChat: 0,
                currentNode: currentNode?.data?.information?.transferTo ?? 0
              });
              await sendConnectLiveAgent(whatsappCcaasTable[0]);
              sendConnectLiveAgentMessage(whatsappCcaasTable);
            }
          } catch (error) {
            console.log(error);
          }
        } else {
          const getUser = await getUserExt(currentNode?.data?.information);
          await connectToliveAgent(getUser, whatsappCcaasTable, getBotDetails, getPersonalData, getCoid);
        }
      } catch (error: any) {
        const nextNodeData = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
        return handleChatFlowType(nextNodeData, whatsappCcaasTable, nodes, input, getNlpData, req);
      }
      break;
    }
    case 'randomise': {
      try {
        logger.info('randomise>>>>>>>>>>>>>>>>>>>>>>>', currentNode);
        const randonIndex = Math.floor(Math.random() * currentNode?.data?.children?.length);
        const childIdData = currentNode?.data?.children[randonIndex];
        await updateNextNode(whatsappCcaasTable[0], childIdData?.childId);
        return handleChatFlowType(childIdData, whatsappCcaasTable, nodes, input, getNlpData, req);
      } catch (error) {
        const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
        return handleChatFlowType(nextNode, whatsappCcaasTable, nodes, input, getNlpData, req);
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
        await updateNextNode(whatsappCcaasTable[0], currentNodeId);
        return handleChatFlowType(currentNodeId, whatsappCcaasTable, nodes, input, getNlpData, req);
      } catch (error) {
        const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
        return handleChatFlowType(nextNode, whatsappCcaasTable, nodes, input, getNlpData, req);
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
          await sendMessageNumber(whatsappCcaasTable[0], useCode);
          const nextNode = currentNode?.data?.children?.find((nodes1: any) => nodes1?.type === RESPONSE?.success_message) ?? { childId: '' };
          const nextNodedetails: any = await getNodeDataById(nextNode?.childId, nodes);
          let message: any = {};
          try {
            message = {
              messageID: uuidv4(),
              sessionId: whatsappCcaasTable[0]?.sessionId,
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
            stringifiedMessage.WhatsappHistoryId = whatsappCcaasTable[0]?.WhatsappHistoryId;
            await updateChatMessage(stringifiedMessage)
            updateIsBot(whatsappCcaasTable[0]?.WhatsappHistoryId);
          } catch (error) {
            const nextNodes = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
            // return handleChatFlowType(nextNode?.childId ?? null, whatsappCcaasTable[0] ?? null, node, io);
          }
          await updateNextNode(whatsappCcaasTable[0], nextNode?.childId);
          return handleChatFlowType(nextNode, whatsappCcaasTable, nodes, input, getNlpData, req);
        }
      } catch (error) {
        const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
        return handleChatFlowType(nextNode, whatsappCcaasTable, nodes, input, getNlpData, req);
      }
      break;
    }
    case 'checkCondition': {
      try {
        logger.info('checkCondition>>>>>>>>>>>>>>>>>>>>>>>', currentNode);
        const conditionParseData = JSON.parse(currentNode?.data?.information?.condition_data);
        const AgentAvailibility = await getAgentAvailibility(whatsappCcaasTable[0]);
        const getEmailData: any = await getCustomerEmailId(whatsappCcaasTable[0]);
        const getExistingCustomer: any = await getExistingCustomerContact(whatsappCcaasTable[0]);
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
              if (description.includes(condition_data.description)) {
                isSuccess = true
              } else {
                isSuccess = false
              }
            }
            if (isSuccess) {
              const nextNode = currentNode?.data?.children?.find((nodes1: any) => nodes1?.type === RESPONSE?.success_message) ?? { childId: '' };
              const nextNodedetails: any = await getNodeDataById(nextNode?.childId, nodes);
              await updateNextNode(whatsappCcaasTable[0], nextNode?.childId);
              return handleChatFlowType(nextNode, whatsappCcaasTable, nodes, input, getNlpData, req);
            } else {
              const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
              await updateNextNode(whatsappCcaasTable[0], nextNode?.childId);
              return handleChatFlowType(nextNode, whatsappCcaasTable, nodes, input, getNlpData, req);
            }
          }
        }
      } catch (error) {
        const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
        return handleChatFlowType(nextNode, whatsappCcaasTable, nodes, input, getNlpData, req);
      }
      break;
    }
    case 'disconnect': {
      logger.info("disconnect>>>>>>>>>>>>>>>>>>>>>>>>", currentNode);
      await getDispositinDisconnectStatus(whatsappCcaasTable[0]);
      const getDispositin: any = await getDispositinStatus(whatsappCcaasTable[0]);
      const closedDispositions = getDispositin[0]?.filter((item: { dispositionName: string; }) => item.dispositionName === 'Closed');
      await updateDispositionDiscnnect(whatsappCcaasTable[0], closedDispositions[0]);
      await updateDispositionBot(whatsappCcaasTable[0]);
      const updateScore: any = await sentimentalScoreData(whatsappCcaasTable[0]);
      const updateScoreData = updateScore?.data?.sentimentalScore;
      await updateSentimentScoreBot(whatsappCcaasTable[0], updateScoreData);
      const sentimentSupervisorBot: any = await getSupervisiorList(whatsappCcaasTable[0]);
      sentimentSupervisorBot.forEach((ext: any) => {
        console.log(emitter?.supervisiorActivityBot, `${whatsappCcaasTable[0]?.domainId}_${ext?.ext}`);
        ioredisWhatsapp.to(`${whatsappCcaasTable[0]?.domainId}_${ext?.ext}`).emit(emitter?.supervisiorActivityBot);
      });
      await new Promise(resolve => setTimeout(resolve, 500));
      return;
    }
    default: {
      logger.info('Default calling');
      return;
    }
  }
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
export const triggerDataHandler = (trigger: any, req: any, nodes: any, whatsappCcaasTable: any, getNlpData: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      let matches: any = false;
      if (trigger[0]?.triggerKey[0] === "7") {
        matches = trigger[0]?.inputValues.some((triggerData: any) => {
          return triggerData?.value?.toLowerCase() === req?.body?.text?.body?.toLowerCase();
        });
      } else if (trigger[0]?.triggerKey[0] === "4") {
        const getCustomerdata = await insertCustomerContact(req.body, null);
        matches = getCustomerdata[0]?.errcode !== 1;
      } else if (trigger[0]?.triggerKey[0] === "5") {
        const getCustomerdata = await insertCustomerContact(req.body, null);
        matches = getCustomerdata[0]?.errcode === 1;
      }
      if (matches || trigger?.length === 0) {
        const startingNode = await getStartNode(nodes);
        const currentNodeId = startingNode?.data?.children[0]?.childId ?? '';
        await updateNextNode(whatsappCcaasTable[0], currentNodeId);
        handleChatFlowType(currentNodeId, whatsappCcaasTable, nodes, '', getNlpData, req.body);
      } else {
        resolve(false);
      }
    } catch (error) {
      logger?.error("triggerDataHandler", error)
      reject(error);
    }
  });
};



/**
 * 
 * @param trigger 
 * @param req 
 * @param nodes 
 * @param whatsappCcaasTable 
 * @param getNlpData 
 */
export const triggerDataSendMessageHandler = (trigger: any, req: any, nodes: any, whatsappCcaasTable: any, getNlpData: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      let matches: any = false;
      if (trigger[0]?.triggerKey[0] === "7") {
        matches = trigger[0]?.inputValues.some((triggerData: any) => {
          return triggerData?.value?.toLowerCase() === req?.body?.text?.body?.toLowerCase();
        });
      } else if (trigger[0]?.triggerKey[0] === "4") {
        const getCustomerdata = await insertCustomerContact(req.body, null);
        matches = getCustomerdata[0]?.errcode !== 1;
      } else if (trigger[0]?.triggerKey[0] === "5") {
        const getCustomerdata = await insertCustomerContact(req.body, null);
        matches = getCustomerdata[0]?.errcode === 1;
      }
      if (matches || trigger?.length === 0) {
        const startingNode = await getStartNode(nodes);
        const currentNodeId = startingNode?.data?.children[0]?.childId ?? '';
        await updateNextNode(whatsappCcaasTable[0], currentNodeId);
        handleChatFlowType(currentNodeId, whatsappCcaasTable, nodes, '', getNlpData, req.body);
      } else {
        await sendMessageCustomer(whatsappCcaasTable[0], getNlpData?.data?.message);
        let message: any = {};
        try {
          message = {
            messageID: uuidv4(),
            sessionId: whatsappCcaasTable[0]?.sessionId,
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
          stringifiedMessage.WhatsappHistoryId = whatsappCcaasTable[0]?.WhatsappHistoryId;
          await updateChatMessage(stringifiedMessage);
          updateIsBot(whatsappCcaasTable[0]?.WhatsappHistoryId);
        } catch (error) {
          console.log(error);
          reject(error);
        }
      }
    } catch (error) {
      logger?.error("triggerDataSendMessageHandler", error)
      reject(error);
    }
  });
};


/**
 * 
 * @param trigger 
 * @param req 
 * @param nodes 
 * @param whatsappCcaasTable 
 * @param getNlpData 
 */
export const triggerDataecommerceHandler = (trigger: any, req: any, nodes: any, whatsappCcaasTable: any, getNlpData: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      let matches: any = false;
      if (trigger[0]?.triggerKey[0] === "7") {
        matches = trigger[0]?.inputValues.some((triggerData: any) => {
          return triggerData?.value?.toLowerCase() === req?.body?.text?.body?.toLowerCase();
        });
      } else if (trigger[0]?.triggerKey[0] === "4") {
        const getCustomerdata = await insertCustomerContact(req.body, null);
        matches = getCustomerdata[0]?.errcode !== 1;
      } else if (trigger[0]?.triggerKey[0] === "5") {
        const getCustomerdata = await insertCustomerContact(req.body, null);
        matches = getCustomerdata[0]?.errcode === 1;
      }
      if (matches || trigger?.length === 0) {
        const startingNode = await getStartNode(nodes);
        const currentNodeId = startingNode?.data?.children[0]?.childId ?? '';
        await updateNextNode(whatsappCcaasTable[0], currentNodeId);
        handleChatFlowType(currentNodeId, whatsappCcaasTable, nodes, '', getNlpData, req.body);
      } else if (
        (getNlpData?.data?.ecommerce_intent === "Product Purchase" || getNlpData?.data?.ecommerce_intent === "Product Enquiry") &&
        getNlpData?.data?.ecommerce_product &&
        Object.keys(getNlpData?.data?.ecommerce_product).length > 0
      ) {
        const startingNode = await getStartNodeConnecter(nodes);
        const parseData = JSON.parse(startingNode?.data?.information?.integration_data);

        if (parseData?.connector === 13) {
          if (!Object.keys(getNlpData?.data?.ecommerce_product)?.length && getNlpData?.data?.ecommerce_intent !== "others") {
            await sendMessageCustomerCallbacks(whatsappCcaasTable[0]);
          } else if (
            (getNlpData?.data?.ecommerce_intent === "Product Purchase" || getNlpData?.data?.ecommerce_intent === "Product Enquiry") &&
            getNlpData?.data?.ecommerce_product &&
            Object.keys(getNlpData?.data?.ecommerce_product).length > 0
          ) {
            await sendCatalogues(whatsappCcaasTable[0], getNlpData?.data, req?.body?.customerName);
          }
        } else {
          await sendMessageCustomerCai(whatsappCcaasTable[0], getNlpData?.data?.message);
        }
      } else {
        await sendMessageCustomerCai(whatsappCcaasTable[0], getNlpData?.data?.message);
      }
    } catch (error) {
      logger?.error("triggerDataecommerceHandler", error)
      reject(error);
    }
  });
};

/**
 * 
 * @param whatsappCcaasTable 
 */
export const sendConnectLiveAgentMessage = async (whatsappCcaasTable: any) => {
  return new Promise(async (resolve, reject) => {
    let message: any = {};
    try {
      message = {
        messageID: uuidv4(),
        sessionId: whatsappCcaasTable[0]?.sessionId,
        message: messageValue?.liveAgent,
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
      stringifiedMessage.WhatsappHistoryId = whatsappCcaasTable[0]?.WhatsappHistoryId;
      await updateChatMessage(stringifiedMessage);
    } catch (error) {
      console.log(error);
      reject(error);
    }
  });
}

/**
 * 
 * @param whatsappCcaasTable 
 */
export const sendAgentAvailable = async (whatsappCcaasTable: any) => {
  return new Promise(async (resolve, reject) => {
    let message: any = {};
    try {
      message = {
        messageID: uuidv4(),
        sessionId: whatsappCcaasTable[0]?.sessionId,
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
      stringifiedMessage.WhatsappHistoryId = whatsappCcaasTable[0]?.WhatsappHistoryId;
      await updateChatMessage(stringifiedMessage);
    } catch (error) {
      console.log(error);
      reject(error);
    }
  });
}

/**
 * 
 * @param whatsappCcaasTable 
 * @param Message 
 */
export const sendAgentAvailableMissed = (whatsappCcaasTable: any, Message: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      let message: any = {
        messageID: uuidv4(),
        sessionId: whatsappCcaasTable[0]?.sessionId,
        message: Message,
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
      stringifiedMessage.WhatsappHistoryId = whatsappCcaasTable[0]?.WhatsappHistoryId;
      await updateChatMessage(stringifiedMessage);
      updateIsBot(whatsappCcaasTable[0]?.WhatsappHistoryId);
    } catch (error) {
      console.log(error);
      reject(error);
    }
  });
};


/**
 * 
 * @param whatsappCcaasTable 
 * @param message 
 */
export const sendMessageCustomerCaiMessage = async (whatsappCcaasTable: any, messageData: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      let message: any = {};
      message = {
        messageID: uuidv4(),
        sessionId: whatsappCcaasTable[0]?.sessionId,
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
      stringifiedMessage.WhatsappHistoryId = whatsappCcaasTable[0]?.WhatsappHistoryId;
      await updateChatMessage(stringifiedMessage);
      updateIsBot(whatsappCcaasTable[0]?.WhatsappHistoryId);
    } catch (error) {
      console.log(error);
      reject(error);
    }
  });
}


/**
 * 
 * @param whatsappCcaasTable 
 */
export const sendFlowBreakMessage = async (whatsappCcaasTable: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      let message: any = {};
      message = {
        messageID: uuidv4(),
        sessionId: whatsappCcaasTable[0]?.sessionId,
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
      stringifiedMessage.WhatsappHistoryId = whatsappCcaasTable[0]?.WhatsappHistoryId;
      await updateChatMessage(stringifiedMessage);
      updateIsBot(whatsappCcaasTable[0]?.WhatsappHistoryId);
    } catch (error) {
      console.log(error);
      reject(error);
    }
  });
}

/**
 * 
 * @param whatsappCcaasTable 
 * @param messageData 
 */
export const sendFlowMessageData = async (whatsappCcaasTable: any, messageData: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      let message: any = {};
      message = {
        messageID: uuidv4(),
        sessionId: whatsappCcaasTable[0]?.sessionId,
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
      stringifiedMessage.WhatsappHistoryId = whatsappCcaasTable[0]?.WhatsappHistoryId;
      await updateChatMessage(stringifiedMessage);
      updateIsBot(whatsappCcaasTable[0]?.WhatsappHistoryId);
    } catch (error) {
      console.log(error);
      reject(error);
    }
  });
}

/**
 * 
 * @param whatsappCcaasTable 
 * @param message 
 */
export const sendMessageCustomerCount = async (whatsappCcaasTable: any, messageData: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      let message: any = {};
      message = {
        messageID: uuidv4(),
        sessionId: whatsappCcaasTable?.sessionId,
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
      stringifiedMessage.WhatsappHistoryId = whatsappCcaasTable?.WhatsappHistoryId;
      await updateChatMessage(stringifiedMessage);
    } catch (error) {
      console.log(error);
      reject(error);
    }
  });
}

/**
 * 
 * @param whatsappCcaasTable 
 * @param message 
 */
export const sendMessageCustomerAppointment = async (whatsappCcaasTable: any, messageData: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      let message: any = {};
      message = {
        messageID: uuidv4(),
        sessionId: whatsappCcaasTable?.sessionId,
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
      stringifiedMessage.WhatsappHistoryId = whatsappCcaasTable?.WhatsappHistoryId;
      await updateChatMessage(stringifiedMessage);
    } catch (error) {
      console.log(error);
      reject(error);
    }
  });
}

/**
 * 
 * @param whatsappCcaasTable 
 * @returns 
 */
export const sendFlowBreakMessageRemainder = async (whatsappCcaasTable: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      let message: any = {};
      message = {
        messageID: uuidv4(),
        sessionId: whatsappCcaasTable[0]?.sessionId,
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
      stringifiedMessage.WhatsappHistoryId = whatsappCcaasTable[0]?.WhatsappHistoryId;
      await updateChatMessage(stringifiedMessage);
    } catch (error) {
      console.log(error);
      reject(error);
    }
  });
}

/**
 * 
 * @param liveAgent 
 * @param whatsappCcaasTable 
 * @param getBotDetails 
 * @param getPersonalData 
 * @param getCoid 
 * @returns 
 */
export const connectToliveAgent = async (liveAgent: any, whatsappCcaasTable: any, getBotDetails: any, getPersonalData: any, getCoid: any): Promise<void> => {
  try {
    const session = whatsappCcaasTable?.[0];
    if (!session) {
      logger.info("No WhatsApp CCAAS table session data provided.");
      return;
    }
    console.log("liveAgent>>>>>>>>>", liveAgent[0])
    if (Array.isArray(liveAgent) && liveAgent?.length > 0 && liveAgent[0]?.domainId && liveAgent[0]?.ext) {
      const { domainId, ext } = liveAgent[0];
      console.log(emitter?.invite, `${domainId}_${ext}`);
      logger.info(emitter?.invite, `${domainId}_${ext}`);
      ioredisWhatsapp.to(`${domainId}_${ext}`).emit(emitter?.invite, {
        channelType: session?.channelType,
        customerNumber: session?.customerNumber,
        agentNumber: session?.calledNumber,
        sessionId: session?.sessionId,
        historyId: session?.WhatsappHistoryId,
        calledPersonDetails: getPersonalData?.[0] ?? null,
        allAgent: true,
        WhatsappDurationHistoryId: session?.WhatsappDurationHistoryId,
        isTransferChat: 1,
        currentNode: null,
        ext: ext,
        domainId: domainId
      });
      await Promise.all([
        sendConnectLiveAgent(session),
        sendConnectLiveAgentMessage(whatsappCcaasTable)
      ]);
      return;
    }
    logger.info("No live agent found or data invalid, checking callback toggle...");
    // const callbackEnabled = Array.isArray(getBotDetails) && getBotDetails?.length > 0 && Number(getBotDetails[0]?.callBackToggle) === 1;
    // if (callbackEnabled) {
    //   const redirectUrlCallback = `WhatsappWebcallback?coId=${getCoid?.[0]?.coid}&domainId=${session.domainId}&phoneNumber=${session.customerNumber}&historyId=${session?.WhatsappHistoryId}`;
    //   await Promise.all([
    //     sendCallbackRequest(session, redirectUrlCallback),
    //     updateCallbackStatus(session),
    //     sendAgentAvailable(whatsappCcaasTable)
    //   ]);
    //   return;
    // }
    // const getMessage = await sendMessage(session);
    // await Promise.all([
    // sendAgentAvailableMissed(whatsappCcaasTable, getMessage?.content),
    //   updateMissedChat(session),
    //   (async () => {
    //     const supervisors: any = await getSupervisiorList(session);
    //     supervisors.forEach((ext: any) => {
    //       console.log(emitter?.supervisiorActivityBot, `${session?.domainId}_${ext?.ext}`);
    //       ioredisWhatsapp.to(`${session.domainId}_${ext?.ext}`).emit(emitter?.supervisiorActivityBot);
    //     });
    //   })()
    // ]);
  } catch (error) {
    logger.error("Error in connectToliveAgent:", error);
    throw error;
  }
};


/**
 * 
 * @param data 
 * @param whatsappCcaasTable 
 * @param currentNode_ref 
 * @param nodes 
 * @param getNlpData 
 * @param req 
 * @returns 
 */
export const uploadFilesFlow = async (data: any, whatsappCcaasTable: any, currentNode_ref: any, nodes: any, getNlpData: any, req: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      const nextData = currentNode_ref?.data?.children[0];
      await updateNextNode(whatsappCcaasTable[0], nextData?.childId);
      return handleChatFlowType(null, whatsappCcaasTable, nodes, '', getNlpData, req);
    } catch (error) {
      logger.error("uploadFilesFlow", error);
      reject(error);
    }
  });
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getDispositionStatusHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: req?.params?.domainId,
    };
    const getDispositionStatus: any = await getDispositionStatusDao(data);
    logger?.info("getDispositionStatus", getDispositionStatus)
    if (getDispositionStatus && getDispositionStatus.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getDispositionStatusResponse: getDispositionStatus });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getDispositionStatusResponse: [] });
    }
  } catch (err) {
    logger?.error("getDispositionStatusHandler", err)
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function insertWhatsappCallbackHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: req.body?.domainId,
      sessionId: req.body?.sessionId,
      IsInBound: req.body?.IsInBound,
      customerNumber: req.body?.customerNumber ?? null,
      calledNumber: req.body?.calledNumber ?? null,
      whatsappHistoryId: req?.body?.whatsappHistoryId ?? null
    };
    const insertWhatsappCallback: any = await insertWhatsappCallbackDao(data);
    logger?.info("insertWhatsappCallback", insertWhatsappCallback)
    if (insertWhatsappCallback && insertWhatsappCallback?.length > 0) {
      const getSupervisiorExt: any = await getSupervisiorList(data);
      getSupervisiorExt.forEach((ex: any) => {
        console.log(emitter?.supervisiorActivity, `${data?.domainId}_${ex.ext}`);
        ioredisWhatsapp.to(`${data?.domainId}_${ex.ext}`).emit(emitter?.supervisiorActivity);
      });
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getWhatsappCallBackResponse: insertWhatsappCallback });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getWhatsappCallBackResponse: [] });
    }
  } catch (err) {
    logger?.error("insertWhatsappCallbackHandler", err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function deleteCustomInputTemplateHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data = {
      flowId: req?.params?.flowId,
      domainId: auth.domainId,
    };
    const deleteFlow: any = await deleteCustomInputTemplate(data);
    logger?.info("deleteFlow", deleteFlow)
    if (deleteFlow[0]?.errcode === 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.delete_template });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.deleteFaild });
    }
  } catch (err) {
    logger?.error("deleteCustomInputTemplateHandler", err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getTemplateDetailsHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      flowId: req?.params?.flowId,
      domainId: auth.domainId
    };
    const getTemplate: any = await getTemplateDetails(data);
    logger?.info("getTemplate", getTemplate)
    if (getTemplate && getTemplate.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getTemplateDetailsResponse: getTemplate });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getTemplateDetailsResponse: [] });
    }
  } catch (err) {
    logger?.error("getTemplateDetailsHandler", err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getPlanInNlpHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      domainId: req?.body?.domainId,
      channelType: req?.body?.channelType,
    };
    const getPlan: any = await getPlanInNlp(data);
    logger?.info("getPlan", getPlan);
    if (getPlan && Array.isArray(getPlan) && getPlan.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getPlanRes: getPlan });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getPlanRes: [] });
    }
  } catch (err) {
    logger?.error("getPlanInNlpHandler", err);
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
    logger?.info("getPayment", getPayment);
    if (getPayment) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.faild });
    }
  } catch (err) {
    logger?.error("sendAppoinmentPaymentHandler", err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param agentData
 */
export async function chatQueueByAgentStatusHandler(req: any, res: any, done: any) {
  res.status(200).send({ statusCode: 200, message: RESPONSE.success_message });
  try {
    const datas: any = {
      ext: req?.body?.ext,
      domainId: req?.body.domainId
    };
    const redisQueueData: any = await ioredisWhatsappQueue.get(process.env.WHATSAPP_QUEUE);
    if (redisQueueData) {
      const redisData = JSON.parse(redisQueueData);
      const filterResult = redisData?.length > 0 && redisData?.filter((_f: any) => _f?.domainId === datas?.domainId);
      if (filterResult?.length > 0) {
        filterResult?.map(async (list: any) => {
          const getQueueList = await getQueueWaitTime(list);
          const queueResult = typeof getQueueList?.[0]?.queueList === 'string'
            ? (() => {
              try {
                return JSON.parse(getQueueList?.[0]?.queueList);
              } catch (error) {
                return [];
              }
            })()
            : [];
          if (queueResult?.length > 0) {
            let agentData: any = [];
            queueResult?.map(async (data: any) => {
              const getPersonalData: any = await getPersonalDetails(data?.channelId?.customerNumber, data?.channelId?.domainId);
              const agentDetailRes = await transferChat(data?.channelId, data?.currentNode);
              const agentDetails = agentDetailRes ? [agentDetailRes] : [];
              const queueSetting: any = await getQueueSettingS(getQueueList?.[0]?.qid, data?.channelId?.domainId);
              const agentFinalList = []
              agentDetails?.map((_l: any) => {
                if (!agentData.includes(parseInt(_l.ext))) {
                  agentFinalList.push(_l)
                }
              })
              if (agentFinalList && agentFinalList?.length > 0) {
                agentFinalList[0].queueId = data?.currentNode.data?.information?.transferTo;
                agentFinalList[0].queueSettings = queueSetting;
                let whatsappCcaasTable = [data?.channelId];
                try {
                  if (Array.isArray(agentFinalList) && agentFinalList?.length > 0) {
                    agentData.push(parseInt(agentFinalList[0]?.ext));
                    console.log(emitter?.invite, `${agentFinalList[0]?.domainId}_${agentFinalList[0]?.ext}`);
                    ioredisWhatsapp.to(`${agentFinalList[0]?.domainId}_${agentFinalList[0]?.ext}`).emit(emitter?.invite, {
                      channelType: whatsappCcaasTable[0].channelType, customerNumber: whatsappCcaasTable[0].customerNumber,
                      agentNumber: whatsappCcaasTable[0].calledNumber, sessionId: whatsappCcaasTable[0].sessionId, historyId: whatsappCcaasTable[0]?.WhatsappHistoryId,
                      calledPersonDetails: getPersonalData[0] ?? null,
                      allAgent: true, WhatsappDurationHistoryId: whatsappCcaasTable[0]?.WhatsappDurationHistoryId,
                      isTransferChat: 0,
                      currentNode: getQueueList?.[0]?.qid ?? 0
                    });
                  }
                } catch (error) {
                  logger?.error("chatQueueByAgentStatusHandler", error);
                }
              }
            });
          }
        });
      }
    }
  } catch (err) {
    logger?.error("chatQueueByAgentStatusHandler", err);
  }
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
    dateArray.push({ id: i + 1, title: dates[i] });
  }
  return dateArray;
};