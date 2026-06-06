import { result } from 'lodash';
import {
  getAgentInteractionDao,
  getCustomerSatisficationChannelDao,
  getCustomerSatisficationDao,
  getCustomerSatisficationRatingDao,
  getInteractionOverallDao,
  insertCustomerSatisficationDao,
  insertUpdateCustomerFeedbackDao,
  insertUpdateInteractionNotesDao
} from '../../dao/customerSatisfaction';
import { RESPONSE } from '../../helpers/constants';
import { ioredisWhatsapp } from '../../plugins/db';
import { logger } from '../../plugins/winston';

/**
 *
 * @param req
 * @param res
 * @param done
 * Customer Satisfication api
 */
export async function insertCustomerSatisficationHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      caption: req?.body?.caption ?? null,
      domainId: auth.domainId,
      ratingDesignType: req?.body?.ratingDesignType ?? 0,
      labelRatingType: req?.body?.labelRatingType ?? 0,
      followUpQuestion: req?.body?.followUpQuestion ?? 0,
      thanksNote: req?.body?.thanksNote ?? null,
      dontDisturbType: req?.body?.dontDisturbType ?? 0,
      dontDistrubTime: req?.body?.dontDistrubTime ?? 0,
      additionalQuestionType: req?.body?.additionalQuestionType ?? 0,
      additionalQuestions: req?.body?.additionalQuestions ? JSON.stringify(req.body.additionalQuestions) : null,
      isVoice: req?.body?.isVoice ?? 0,
      callFlows: req?.body?.callFlows ?? null,
      minimumDurationType: req?.body?.minimumDurationType ?? 0,
      minimumDuration: req?.body?.minimumDuration ?? 0,
      isWebWidget: req?.body?.isWebWidget ?? 0,
      Widgets: req?.body?.Widgets ?? null,
      isSms: req?.body?.isSms ?? 0,
      smsNumbers: req?.body?.smsNumbers ?? null,
      smsNumberSend: req?.body?.smsNumberSend ?? null,
      isSendSmsFromSameNumber: req?.body?.isSendSmsFromSameNumber ?? 0,
      smsMessage: req?.body?.smsMessage ? JSON.stringify(req.body.smsMessage) : null,
      isEmail: req?.body?.isEmail ?? 0,
      emails: req?.body?.emails ?? null,
      emailIdSend: req?.body?.emailIdSend ?? null,
      isSendEmailFromSameEmail: req?.body?.isSendEmailFromSameEmail ?? 0,
      emailSubject: req?.body?.emailSubject ?? null,
      emailMessage: req?.body?.emailMessage ? JSON.stringify(req.body.emailMessage) : null,
      isWhatsApp: req?.body?.isWhatsApp ?? 0,
      whatsAppChatFlows: req?.body?.whatsAppChatFlows ?? null,
      isFacebook: req?.body?.isFacebook ?? 0,
      facebookChatFlows: req?.body?.facebookChatFlows ?? null,
      isInstagram: req?.body?.isInstagram ?? 0,
      instagramChatFlows: req?.body?.instagramChatFlows ?? null,
      isVideo: req?.body?.isVideo ?? 0,
    };
    const insertResult: any = await insertCustomerSatisficationDao(data);
    logger.info('insertCustomerSatisficationDao', insertResult);
    if (insertResult && insertResult.length > 0) {
      res.status(200).send({ statusCode: 200, insertResult: insertResult[0], message: RESPONSE.CustomerSatisfication });
    } else {
      res.status(400).send({ statusCode: 400, message: 'Bad Request: Invalid or missing data', result: [] });
    }
  } catch (err) {

    logger.error('insertCustomerSatisficationHandler - Unexpected Error:', req.headers, req?.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * get Customer Satisfication api
 */
export async function getCustomerSatisficationRatingHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
    };
    const getCustomerSatisfactionRating: any = await getCustomerSatisficationRatingDao(data);
    logger.info('getCustomerSatisficationRatingDao', getCustomerSatisfactionRating);
    if (Array.isArray(getCustomerSatisfactionRating) && getCustomerSatisfactionRating.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        result: getCustomerSatisfactionRating[0]
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        result: []
      });
    }
  } catch (err) {

    logger.error('getCustomerSatisficationRatingHandler - Unexpected Error:', req.headers, err);
    res.status(500).send({
      statusCode: 500,
      message: RESPONSE.internal_error
    });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * get Customer Satisfication channel status api
 */
export async function getCustomerSatisficationChannelStatusHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    const getCustomerSatisficationChannel: any = await getCustomerSatisficationChannelDao(data);
    logger.info('getCustomerSatisficationChannelDao', getCustomerSatisficationChannel);
    if (Array.isArray(getCustomerSatisficationChannel) && getCustomerSatisficationChannel.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: getCustomerSatisficationChannel[0] });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {

    logger.error('getCustomerSatisficationChannelStatusHandler - Unexpected Error:', req.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * Customer Satisfication api
 */
export async function insertupdateCustomerFeedbackHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      customerFeedbackId: req?.body?.customerFeedbackId ?? null,
      historyId: req?.body?.historyId ?? null,
      channelTypeId: req?.body?.channelTypeId ?? null,
      satisfactionRate: req?.body?.satisfactionRate ?? 0,
      domainId: req?.body?.domainId ?? null,
      feedbackQuestionAnswer: req?.body?.feedbackQuestionAnswer ?? null,
      AgentId :req?.body?.AgentId ?? null,
      ratingType   :req?.body?.ratingType   ?? null,
      sessionId: req?.body?.sessionId ?? null,
      customerNumber:req?.body?.customerNumber ?? null,
      videoMeetId:req?.body?.videoMeetId ?? null,
    };
    const insertResult: any = await insertUpdateCustomerFeedbackDao(data);
    logger.info('insertUpdateCustomerFeedbackDao', insertResult);
    if (insertResult && insertResult.length > 0) {
      ioredisWhatsapp.to(`${data?.domainId}`).emit('insertSuccesfully');
      res.status(200).send({ statusCode: 200, insertResult: insertResult[0], message: RESPONSE.Customerfeedback });
    } else {
      res.status(404).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {

    logger.error('insertupdateCustomerFeedbackHandler - Unexpected Error:', req.headers, req?.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * Customer Satisfication api
 */
export async function getCustomerSatisficationCHannelHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: req?.body?.domainId,
      channelTypeId: req?.body?.channelTypeId ?? null,
      historyId: req?.body?.historyId ?? null,
      videoMeetId:req?.body?.videoMeetId ?? null,
    };
    const insertResult: any = await getCustomerSatisficationDao(data);
    logger.info('getCustomerSatisficationDao', insertResult);
    if (insertResult && insertResult.length > 0) {
      res.status(200).send({ statusCode: 200, insertResult: insertResult[0], message: 'getCustomerSatisfication' });
    } else {
      res.status(404).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {

    logger.error('getCustomerSatisficationCHannelHandler - Unexpected Error:', req.headers, req?.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * Customer Satisfication api
 */
export async function insertUpdateInteractionNotesHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
      historyId:req?.body?.historyId,
      coid :req?.body?.coid,
      agentId  :req?.body?.agentId ?? null,
      channelType: req?.body?.channelType ?? null,
      notes: req?.body?.notes ?? null,
    };
    const insertResult: any = await insertUpdateInteractionNotesDao(data);
    logger.info('insertUpdateInteractionNotesDao', insertResult);
    if (insertResult && insertResult.length > 0) {
      res.status(200).send({ statusCode: 200, insertResult: insertResult[0], message: 'getCustomerSatisfication' });
    } else {
      res.status(404).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {

    logger.error('insertUpdateInteractionNotesHandler - Unexpected Error:', req.headers, req?.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * Customer Satisfication api
 */
export async function getInteractionOverAllNotesHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      coid: req?.body?.coid ?? null,
      domainId: auth?.domainId,
    };
    const insertResult: any = await getInteractionOverallDao(data);
    logger.info('getInteractionOverallDao', insertResult);
    if (insertResult && insertResult.length > 0) {
      res.status(200).send({ statusCode: 200, insertResult: insertResult[0], message: 'getCustomerSatisfication' });
    } else {
      res.status(404).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {

    logger.error('getInteractionOverAllNotesHandler - Unexpected Error:', req.headers, req?.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * Customer Satisfication api
 */
export async function getAgentInteractionNotesHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      historyId: req?.body?.historyId ?? null,
      domainId: auth?.domainId,
      agentId: req?.body?.agentId ?? null,

    };
    const insertResult: any = await getAgentInteractionDao(data);
    logger.info('getAgentInteractionDao', insertResult);
    if (insertResult && insertResult.length > 0) {
      res.status(200).send({ statusCode: 200, insertResult: insertResult[0], message: 'getCustomerSatisfication' });
    } else {
      res.status(404).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {

    logger.error('getAgentInteractionNotesHandler - Unexpected Error:', req.headers, req?.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
