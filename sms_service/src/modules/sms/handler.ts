import {
  agentDetailsApi,
  assignCallBackSmsDao,
  autoAssignAgent,
  getAssingToApi,
  getBotDetail,
  getCompanyDetails,
  getConnectorTokenInfo,
  getCustomerSatisficationDao,
  getDispositinStatus,
  getKnowledgeBaseId,
  getPersonalDetails,
  getSupervisiorList,
  insertCustomerContactPhone,
  onbordUpdateSmsBot,
  sendSmsOutboundCompany,
  sessionIdApi,
  smsCcaasTableDB,
  SmsOutboundSessionHistory,
  SmsSessionHistory,
  updateCustomerId,
  updateDispositionForSms,
  updateReplyViaDispositionForSms,
  updateSentimentBot,
  updateSentimentScore,
  updateSmsBot,
} from '../../dao/smsDao';

import _ from 'lodash';
import { v4 as uuidv4 } from 'uuid';
// import { RESPONSE } from '../../helpers/constants';
import { CHANNELS, EMITTERS, RESPONSE } from '../../constants/sms';

import { createAndInsertNotification, handleCustomerSatisfaction } from '../../helpers/customerSatisfication/utils';
import { ioredisChat, otherRedisClient } from '../../plugins/redis';
import { loggerError, loggerTrace } from '../../utils/log';
import { logger } from '../../utils/logger';
import { callGoogleSheetSyncAPI, getSmsBot, sendSmsMessageCustomer, sendSmsMessageOutbound, sentimentalScore } from '../../utils/nlp';
/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function insertInboundSmsHandler(req: any, res: any, done: any) {
  try {
    const smsCcaasTable: any = await smsCcaasTableDB(req.body);
    if (!Array.isArray(smsCcaasTable) || smsCcaasTable.length === 0) {
      loggerError('insertInboundSmsHandler - no data', smsCcaasTable, {});
      return res.send({ statusCode: 422, message: RESPONSE.ERROR });
    }
    const record = smsCcaasTable[0];
    console.log('smsCcaasTable>>>>>>>', smsCcaasTable);
    const coid = await insertCustomerContactPhone(req.body, record);
    handleGoogleSheetSync(record, coid).catch(err => loggerError('GS sync err', err, {}));
    await createAndInsertNotification(record, coid[0]);
    await updateCustomerId(record, coid[0]);
    if (record?.errcode === 0) {
      res.send({ statusCode: 200, message: RESPONSE.SUCCESS });

      const getBotDetails: any = await getBotDetail(record);
      const getSupervisorExt: any = await getSupervisiorList(record);
      const comDetails = await getCompanyDetails(record);
      const getBot: any = await getSmsBot(record, comDetails, req?.body?.content, getBotDetails[0]);
      if (getBotDetails[0]?.isBot === 0 && getBot?.key === 'human_agent' && record?.IsUpdate === 0) {
        try {
          const autoAssign = await autoAssignAgent(record, getBot?.ext, getBot);
          ioredisChat.to(`${autoAssign[0]?.domainId}_${autoAssign[0]?.ext}`).emit(EMITTERS.ASSIGN_ACTIVITY);
          logger.info('assignActivityAgent', `${autoAssign[0]?.domainId}_${autoAssign[0]?.ext}`);
        } catch (error) {
          console.log(error);
        }
      } else if (getBotDetails[0]?.isBot === 1 && getBot?.key === 'human_agent' && record?.IsUpdate === 0) {
        try {
          const autoAssign = await autoAssignAgent(record, getBot?.ext, getBot);
          ioredisChat.to(`${autoAssign[0]?.domainId}_${autoAssign[0]?.ext}`).emit(EMITTERS.ASSIGN_ACTIVITY);
          console.log('assignActivityAgent', `${autoAssign[0]?.domainId}_${autoAssign[0]?.ext}`);
        } catch (error) {
          console.log(error);
        }
      } else if (getBotDetails[0]?.isBot === 1 && (getBotDetails[0]?.autoAssignAgent === 0 || getBotDetails[0]?.autoAssignAgent === null)) {
        await handleBotFlow(record, getSupervisorExt, getBot).catch(e => loggerError('handleBotFlow err', e, {}));
      } else {
        await handleSupervisorFlow(record, getSupervisorExt).catch(e => loggerError('handleSupervisorFlow err', e, {}));
      }
      if (record?.IsUpdate === 1) {
        await handleSessionUpdate(req.body, record, getSupervisorExt).catch(e => loggerError('handleSessionUpdate err', e, {}));
      }
      return;
    }

    if (record?.errcode === -1) {
      loggerError('ccaas_update_did_purchase_bot_status', RESPONSE?.SMS_ERROR, {});
      return res.send({ statusCode: 424, message: RESPONSE?.SMS_ERROR });
    }
    return res.send({ statusCode: 422, message: RESPONSE?.ERROR });
  } catch (err) {
    loggerError('ccaas_update_did_purchase_bot_status Error', err, {});
    return res.send({ statusCode: 500, result: [], message: RESPONSE.ERROR });
  }
}

async function handleGoogleSheetSync(record: any, coid: any) {
  // callback-based API; guard returned value
  getConnectorTokenInfo(record, (getConnect: any) => {
    if (Array.isArray(getConnect) && getConnect.length > 0) {
      callGoogleSheetSyncAPI({
        contactSyncDetails: getConnect[0]?.contactSyncDetails,
        newContacts: coid?.[0]?.coid,
        domainId: record?.domainId,
        token: getConnect[0]?.token,
      });
    }
  });
}

// async function handleAutoAssignAgent(record: any) {
//   const presence = await otherRedisClient.hgetall(`${record?.domainId}_presence`);
//   const ext = Object.keys(presence || {})
//     .filter((key) => {
//       try {
//         return JSON.parse(presence[key])?.statusName === 'Ready';
//       } catch {
//         return false;
//       }
//     })
//     .map(key => key.split('_')[1]);

//   const presenceStatus = ext?.join(',');
//   const autoAssign = await autoAssignAgent(record, presenceStatus);

//   if (Array.isArray(autoAssign) && autoAssign[0]) {
//     ioredisChat.to(`${autoAssign[0]?.domainId}_${autoAssign[0]?.ext}`).emit(EMITTERS.ASSIGN_ACTIVITY);
//     loggerTrace('assignActivityAgent', `${autoAssign[0]?.domainId}_${autoAssign[0]?.ext}`, {});
//     loggerError('ccaas_update_did_purchase_bot_status', autoAssign[0]?.errMsg, {});
//   }
// }

async function handleBotFlow(record: any, supervisors: any[] = [], content: any) {
  try {
    await sendSmsMessageCustomer(record, content?.message);
    const getDispositin: any = await getDispositinStatus(record);
    const closedDispositions = Array.isArray(getDispositin?.[0]) ? getDispositin[0].filter((item: any) => item.dispositionName === 'Closed') : [];
    const dispositionId = closedDispositions?.[0]?.did ?? null;

    const message: any = buildBotMessage(record, content?.message);
    const messageData: any = buildBotMessageData(record, message, dispositionId);

    await smsCcaasTableDB(messageData);
    await updateSentimentBot(record, content?.sentiment);

    const sentimentSupervisors: any = await getSupervisiorList(record);
    (sentimentSupervisors || []).forEach((ext: any) => {
      ioredisChat.to(`${record?.domainId}_${ext?.ext}`).emit(EMITTERS.SUPERVISOR_BOT_ACTIVITY);
      loggerTrace('sentimentWhatsappBot', `${record?.domainId}_${ext?.ext}`, {});
    });
    ioredisChat.to(`${record?.domainId}_${200}`).emit(EMITTERS.ADMIN_BOT_ACTIVITY);
  } catch (error) {
    loggerError('insertInboundSmsHandler Error', error, {});
  }
}

async function handleSupervisorFlow(record: any, supervisors: any[] = []) {
  (supervisors || []).forEach((ext: any) => {
    ioredisChat.to(`${record?.domainId}_${ext?.ext}`).emit(EMITTERS.SUPERVISOR_ACTIVITY);
    loggerTrace('SuperVisiorEmitter', `${record?.domainId}_${ext?.ext}`, {});
  });
  ioredisChat.to(`${record?.domainId}_${200}`).emit(EMITTERS.ADMIN_ACTIVITY);
}

async function handleSessionUpdate(body: any, record: any, supervisors: any[] = []) {
  const sessionId = await sessionIdApi(body);
  const agentDetails: any = await agentDetailsApi(sessionId?.[0]?.sessionId, record?.domainId);
  if (!Array.isArray(agentDetails) || agentDetails.length === 0) return;
  const assignedTo = agentDetails[0]?.assignedTo;
  const getAssingToExt: any = await getAssingToApi(assignedTo);
  if (Array.isArray(getAssingToExt) && getAssingToExt.length > 0) {
    const agentList = agentDetails[0]?.customerNumber;
    const getPersonalData: any = await getPersonalDetails(agentList, agentDetails[0]?.domainId);
    ioredisChat.to(`${agentDetails[0]?.domainId}_${getAssingToExt[0].ext}`).emit(EMITTERS.RECEIVED_SMS, { ...agentDetails[0], agentAssist: true, calledPersonDetails: getPersonalData?.[0] }, sessionId?.[0]?.sessionId);
    (supervisors || []).forEach((ext: any) => {
      ioredisChat.to(`${agentDetails[0]?.domainId}_${ext.ext}`).emit(EMITTERS.RECEIVED_SMS, { ...agentDetails[0], agentAssist: true, calledPersonDetails: getPersonalData?.[0] }, sessionId?.[0]?.sessionId);
    });
  }
}

/* Helpers to build bot message payloads */
function buildBotMessage(record: any, message: any) {
  return {
    to: record?.customerNumber,
    from: record?.calledNumber,
    messageID: uuidv4(),
    sessionId: record?.sessionId,
    message: message ?? '',
    sendAt: Date.now(),
    messageType: 'text',
    pdfurl: '',
    isBot: true,
    isCustomer: false,
    fileType: '',
    direction: 'OUT',
    isLike: 0,
  };
}

function buildBotMessageData(record: any, message: any, dispositionId: any) {
  return {
    dispositionId: dispositionId ?? null,
    id: record?.sessionId,
    message: { agentText: { message } },
    source: record?.customerNumber,
    destination: record?.calledNumber,
    IsBot: 1,
    IsSessionClosed: 1,
    Summary: 'Closed',
    sessionEndTime: Math.floor(Date.now()),
    type: 'Inbound',
  };
}

/**
 * update sms Bot Status
 * @param req
 * @param res
 * @param done
 */
export const handleUpdateSmsBotStatus = async (req: any, res: any, done: any) => {
  try {
    const data: any = {
      domainId: req.body.domainId,
      isBot: req?.body?.isBot ?? 0,
      isBotPurchased: req?.body?.isBotPurchased ?? 0,
      your_number: req?.body?.number ?? null
    };
    const result = await updateSmsBot(data);
    // log success trace
    loggerTrace('ccaas_update_did_purchase_bot_status', data, result);
    res.status(200).send({ statusCode: 200, message: RESPONSE.BOT_STATUS_UPDATED });
  } catch (err: any) {
    // log error
    loggerError('ccaas_update_did_purchase_bot_status', err, {});
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.ERROR });
  }
};

/**
 * onbord update sms Bot Status
 * @param req
 * @param res
 * @param done
 */
export const handleOnboardUpdateSmsBotStatus = async (req: any, res: any, done: any) => {
  try {
    const data: any = {
      domainId: req.body.domainId,
      isBot: req?.body?.isBot ?? 0,
      isBotPurchased: req?.body?.isBotPurchased ?? 0
    };
    const result = await onbordUpdateSmsBot(data);
    loggerTrace('ccaas_update_did_purchase_bot_status_onboard', data, result);
    res.status(200).send({ statusCode: 200, message: RESPONSE.BOT_STATUS_UPDATED });
  } catch (err) {
    loggerError('ccaas_update_did_purchase_bot_status_onboard', err, {});
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.ERROR });
  }
};

/**
 *
 * @param req
 * @param res
 * @param done
 * ccs assign call back api
 */
export async function handleAssignCallbackSms(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      assignedBy: req?.body?.assignedBy ?? null,
      assignedTo: req?.body?.assignedTo ?? null,
      assignedOn: Date.now(),
      SMSHistoryId: req.body.smsHistoryId,
      Istransferred: req?.body?.Istransferred ?? null
    };
    const assignRes: any = await assignCallBackSmsDao(data);
    if (assignRes) {
      loggerTrace('up_UpdateAssignAgentToSMS', data, assignRes);
      res
        .status(200)
        .send({ statusCode: 200, message: RESPONSE.CALLBACK_ASSIGNED });
    } else {
      loggerTrace('up_UpdateAssignAgentToSMS', data, 'No record found');
      res.status(200).send({ statusCode: 404, message: RESPONSE.NOT_FOUND });
    }
  } catch (err) {
    loggerError('up_UpdateAssignAgentToSMS', err, {});
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.ERROR });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function updateDispositionSmsHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const params = req.params;
    const data: any = {
      SMSHistoryId: params.SMSHistoryId,
      domainId: auth.domainId,
      dispositionId: req?.body?.dispositionId ?? 0,
      summary: req?.body?.summary ?? null,
      isSessionClosed: req.body?.isSessionClosed ?? 0,
      AfterSMSWorkTime: req?.body?.afterWorkTime ?? null,
      dispositionUpdatedBy: req?.body?.dispositionUpdatedBy ?? null,
      duration: req?.body?.duration ?? null,
      dispositionSubject: req?.body?.dispositionSubject ?? null,
      tags: req?.body?.tags ?? null,
    };

    loggerTrace('updateDispositionSmsHandler - Request Data:', data, {});

    let customerSatisfactionResult = null;
    try {
      customerSatisfactionResult = await getCustomerSatisficationDao({ domainId: data.domainId, channelTypeId: 2003004, historyId: data.SMSHistoryId });
      loggerTrace('updateDispositionSmsHandler - CustomerSatisfactionResult', {}, customerSatisfactionResult);
      if (Array.isArray(customerSatisfactionResult) && customerSatisfactionResult[0] && customerSatisfactionResult[0].insertResult?.[0]?.errcode !== -1) {
        try {
          await handleCustomerSatisfaction(
            customerSatisfactionResult[0],
            {
              domainId: data.domainId,
              channelTypeId: 2003004,
              historyId: data.SMSHistoryId,
            },
            req,
            auth
          );
        } catch (e) {
          loggerError('handleCustomerSatisfaction failed', e, {});
        }
      }
    } catch (e) {
      loggerError('getCustomerSatisficationDao failed', e, {});
    }
    let updateDispStatus = null;
    try {
      updateDispStatus = await updateDispositionForSms(data);
    } catch (e) {
      loggerError('updateDispositionForSms failed', e, {});
    }
    if (data?.isSessionClosed === 1) {
      try {
        const updateDispStatusSupervisor: any = await getSupervisiorList(data);
        updateDispStatusSupervisor?.forEach((ext: any) => {
          ioredisChat
            .to(`${data?.domainId}_${ext?.ext}`)
            .emit(EMITTERS.INTERACTIONCLOSE);
        });
      } catch (e) {
        loggerError('getSupervisiorList failed', e, {});
      }
    }
    try {
      const nlpData = {
        HistoryId: data?.SMSHistoryId,
        ChatDurationHistoryId: data?.ChatDurationHistoryId ?? 0,
        channelType: 'Sms',
        domainId: data?.domainId,
        ext: data?.ext ?? 0,
      };
      const updateScore: any = await sentimentalScore(nlpData);
      if (updateScore?.status === 200) {
        await updateSentimentScore(data, updateScore?.data);

        ioredisChat
          .to(`${auth.domainId}_${auth.ext}`)
          .emit(EMITTERS.SENTIMENTSCORE, {
            data,
            speedometerScore: updateScore?.data?.sentimentalScore ?? 0,
            sentimentalScoreUser:
              updateScore?.data?.sentimentalScoreUser ?? 0,
            sentimentalScoreAgent:
              updateScore?.data?.sentimentalScoreAgent ?? 0,
          });
      }
    } catch (e) {
      loggerError('sentimentalScore failed', e, {});
    }

    return res
      .status(200)
      .send({
        statusCode: 200,
        message: RESPONSE.DISPOSITION_UPDATED,
        result: data,
      });

  } catch (err) {
    loggerError('updateDispositionSmsHandler - Error', err, {});
    return res
      .status(500)
      .send({ statusCode: 500, message: RESPONSE.ERROR });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export const sendCaasSmsHandler = async (req: any, res: any, done: any) => {
  try {
    const data: any = {
      sessionId: req.body.sessionId,
      from: req.body.from,
      to: req.body.to,
      text: req.body.text,
      agentName:req.body.agentName
    };
    loggerTrace('sendCaasSmsHandler - Request', data, {});
    const messageList: any = {
      agentText: {
        message: {
          from: data?.from ?? '',
          to: data?.to ?? '',
          messageId: uuidv4(),
          sendAt: Date.now(),
          message: data?.text ?? '',
          isBot: false,
          agentName:data.agentName
        },
      },
    };
    const sendMessage: any = {
      message: messageList,
      from: data.from,
      to: data.to,
    };
    const smsOut = await SmsSessionHistory(sendMessage, data.sessionId);
    loggerTrace('sendCaasSmsHandler - SmsSessionHistory Response', sendMessage, smsOut);

    const getSupervisiorExt: any = await getSupervisiorList(smsOut[0]);
    loggerTrace('sendCaasSmsHandler - getSupervisiorList Response', smsOut[0], getSupervisiorExt);

    const getPersonalData: any = await getPersonalDetails(data?.to, smsOut[0]?.domainId);
    loggerTrace('sendCaasSmsHandler - getPersonalDetails Response', { to: data?.to, domainId: smsOut[0]?.domainId }, getPersonalData);

    getSupervisiorExt.forEach((ext: any) => {
      console.log('receivedSms', `${smsOut[0]?.domainId}_${ext.ext}`);
      ioredisChat.to(`${smsOut[0]?.domainId}_${ext.ext}`).emit(EMITTERS.RECEIVED_SMS, { ...smsOut[0], channelType: CHANNELS.SMS, agentAssist: false, calledPersonDetails: getPersonalData[0] }, data.sessionId);
    });
    if (smsOut[0].errcode === 0) {
      loggerTrace('sendCaasSmsHandler - SMS Sent', data, messageList);
      res.status(200).send({ statusCode: 200, message: RESPONSE.SMS_SENT, smsSendResponse: messageList });
    } else {
      loggerError('sendCaasSmsHandler - SMS Failed', data, smsOut[0]);
      res.status(200).send({ statusCode: 200, message: RESPONSE.NOT_FOUND });
    }
    // res.status(200).send({ statusCode: 200, message: RESPONSE.SMS_SENT, smsSendResponse: messageList });
  } catch (err) {
    loggerError('sendCaasSmsHandler - Unexpected Error', req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.ERROR });
  }
};

/**
 *
 * @param req
 * @param res
 * @param done
 */
export const sendSmsOutboundHandler = async (req: any, res: any, done: any) => {
  try {
    const auth: any = req.headers;
    const data: any = {
      SMSHistoryId: req?.body?.SMSHistoryId ?? 0,
      sessionId: uuidv4(),
      sessionStartTime: Math.floor(Date.now()),
      sessionEndTime: req?.body?.sessionEndTime ?? null,
      domainId: auth?.domainId,
      from: req?.body?.from ?? null,
      to: req?.body?.to ?? null,
      dispositionId: req?.body?.dispositionId ?? null,
      isInBound: 0,
      isSessionClosed: req?.body?.isSessionClosed ?? 0,
      isBot: req?.body?.isBot ?? 0,
      summary: req?.body?.summary ?? null,
      text: req.body.text,
    };
    loggerTrace('sendSmsOutboundHandler - Request', data, {});
    const messageList: any = {
      agentText: {
        message: {
          from: data?.from ?? '',
          to: data?.to ?? '',
          messageId: uuidv4(),
          sendAt: Date.now(),
          message: data?.text ?? ''
        },
      },
    };
    const sendMessage: any = {
      message: messageList,
      from: data.from,
      to: data.to,
    };
    await sendSmsMessageOutbound(data);
    const smsOut = await SmsOutboundSessionHistory(data, sendMessage);
    loggerTrace('sendSmsOutboundHandler - SmsOutboundSessionHistory Result', data, smsOut);
    const getSupervisiorExt: any = await getSupervisiorList(smsOut[0]);
    getSupervisiorExt.forEach((ext: any) => {
      loggerTrace('sendSmsOutboundHandler - Supervisor Emit', { ext }, {});
      console.log('SuperVisiorEmitterBot', `${smsOut[0]?.domainId}_${ext.ext}`);
      ioredisChat.to(`${smsOut[0]?.domainId}_${ext?.ext}`).emit(EMITTERS.SUPERVISOR_BOT_ACTIVITY);
    });
    res.status(200).send({ statusCode: 200, message: RESPONSE.SMS_SENT, smsSendResponse: smsOut[0] });
  } catch (err) {
    loggerError('sendSmsOutboundHandler - Unexpected Error', {}, err);
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.ERROR });
  }
};

/**
 *
 * @param req
 * @param res
 * @param done
 */
export const sendSmsOutboundCompanyHandler = async (req: any, res: any, done: any) => {
  try {
    const auth: any = req.headers;
    const data: any = {
      sessionId: uuidv4(),
      sessionStartTime: Math.floor(Date.now()),
      sessionEndTime: req?.body?.sessionEndTime ?? null,
      domainId: auth?.domainId,
      from: req?.body?.from ?? null,
      to: req?.body?.to ?? null,
      dispositionId: req?.body?.dispositionId ?? null,
      isInBound: 0,
      isSessionClosed: req?.body?.isSessionClosed ?? 0,
      isBot: req?.body?.isBot ?? 0,
      summary: req?.body?.summary ?? null
    };
    logger.info(`sendSmsOutboundCompanyHandler - Incoming request: ${JSON.stringify(data)}`);
    const inserSms = await sendSmsOutboundCompany(data);
    logger.info(`sendSmsOutboundCompanyHandler - SMS inserted successfully: ${JSON.stringify(inserSms[0])}`);
    res.status(200).send({ statusCode: 200, message: RESPONSE.SMS_SENT, getInserSms: inserSms[0] });
  } catch (err) {
    logger.error('sendSmsOutboundCompanyHandler - Unexpected Error:', err);
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.ERROR });
  }
};

export async function updateReplyviaDispositionSmsHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const params = req.params;
    const data: any = {
      SMSHistoryId: req?.body?.SMSHistoryId,
      domainId: auth.domainId,
      dispositionId: req?.body?.dispositionId ?? null,
      summary: req?.body?.summary ?? null,
      isSessionClosed: req.body?.isSessionClosed ?? 0,
      AfterSMSWorkTime: req?.body?.AfterSMSWorkTime ?? null,
      dispositionUpdatedBy: req?.body?.dispositionUpdatedBy ?? null,
      duration: req?.body?.duration ?? null,
      replyChannelType: req?.body?.responseChannelType ?? null,
      responseHistoryId: req?.body?.responseHistoryId ?? null,
      coid: req?.body?.coid ?? null,
    };

    const updateDispStatus: any = await updateReplyViaDispositionForSms(data);
    console.log('updateDispStatus sms ', updateDispStatus);
    loggerTrace('updateReplyviaDispositionSmsHandler - DB update result', {}, updateDispStatus);
    if (data?.isSessionClosed === 1) {
      const updateDispStatusSupervisor: any = await getSupervisiorList(data);
      loggerTrace('updateReplyviaDispositionSmsHandler - Supervisor list fetched', updateDispStatusSupervisor, {});
      updateDispStatusSupervisor.forEach((ext: any) => {
        console.log('emitterDispStatus', `${data?.domainId}_${ext?.ext}`);
        ioredisChat.to(`${data?.domainId}_${ext?.ext}`).emit(EMITTERS.INTERACTIONCLOSE);
      });
    }
    const nlpData: any = {
      HistoryId: data?.SMSHistoryId,
      ChatDurationHistoryId: data?.ChatDurationHistoryId ?? 0,
      channelType: CHANNELS.SMS,
      domainId: data?.domainId,
      ext: data?.ext ?? 0
    };
    const updateScore: any = await sentimentalScore(nlpData);
    loggerTrace('updateReplyviaDispositionSmsHandler - Sentimental score API response', {}, updateScore?.data);
    if (updateScore?.status === 200) {
      await updateSentimentScore(data, updateScore?.data);
      if (updateScore?.data) {
        ioredisChat.to(`${auth.domainId}_${auth.ext}`).emit(EMITTERS.SENTIMENTSCORE, { data, speedometerScore: updateScore.data.sentimentalScore, sentimentalScoreUser: updateScore.data.sentimentalScoreUser, sentimentalScoreAgent: updateScore.data.sentimentalScoreAgent });
      } else {
        ioredisChat.to(`${auth.domainId}_${auth.ext}`).emit(EMITTERS.SENTIMENTSCORE, { data, speedometerScore: 0, sentimentalScoreUser: 0, sentimentalScoreAgent: 0 });
      }
    } else {
      ioredisChat.to(`${auth.domainId}_${auth.ext}`).emit(EMITTERS.SENTIMENTSCORE, { data, speedometerScore: 0, sentimentalScoreUser: 0, sentimentalScoreAgent: 0 });
    }
    if (updateDispStatus) {
      loggerTrace('updateDispositionSmsHandler - Completed', data, updateDispStatus);
      res.status(200).send({ statusCode: 200, message: RESPONSE.DISPOSITION_UPDATED, result: data });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.NOT_FOUND });
    }
  } catch (err) {
    loggerError('updateReplyviaDispositionSmsHandler - Unexpected Error', err, {});
    res.status(500).send({ statusCode: 500, message: RESPONSE.ERROR });
  }
}
