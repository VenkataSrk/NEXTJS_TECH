
import moment from 'moment';
import { v4 as uuidv4 } from 'uuid';
import { createSurveyFeedback, deleteSurveyFeedback, getAllSurveyFeedback, getSurveyFeedbackById } from '../../dao/surveyAndFeedback.dao';
import { RESPONSE } from '../../helpers/constants';
import { logger } from '../../plugins/log';

export async function createSurveyFeedbackHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const request: any = req.body;
    const data: any = {
      domainId: auth.domainId,
      questionType: request.questionType ?? null,
      createdAt: moment(new Date()).format('YYYY-MM-DD HH:mm:ss'),
      uuid: uuidv4(),
      version: request?.status === 2 ? `Version ${moment(new Date()).format('DD-MM-YYYY HH:mm:ss')} : Published` : `Version ${moment(new Date()).format('DD-MM-YYYY HH:mm:ss')} : Save`,
      status: request?.status,
      question_details: JSON.stringify(request.surveyFeedback)
    };
    const result: any = await createSurveyFeedback(data);
    logger.info('createSurveyFeedback', result);
    if (result.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        data: result
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {
    logger.error('createSurveyFeedbackHandler', req.headers, req.body , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function deleteSurveyFeedbackHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const request: any = req.query;
    const data: any = {
      domainId: auth.domainId,
      feedbackId: request.feedbackId
    };
    const result: any = await deleteSurveyFeedback(data);
    logger.info('deleteSurveyFeedback', result);
    if (result.affectedRows > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.delete_successfully,
        data: result
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {

    logger.error('deleteSurveyFeedbackHandler', req.headers, req.query , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getSurveyFeedbackByIdHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const request: any = req.query;
    const result: any = await getSurveyFeedbackById(request);
    logger.info('getSurveyFeedbackById', result);
    if (result.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        data: result
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {

    logger.error('getSurveyFeedbackByIdHandler', req.headers, req.query , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getSurveyFeedbackListHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const request: any = req.query;
    // request.domainId = auth.domainId;
    const data: any = {
      domainId: auth.domainId,
      limit: request.limit,
      offset: request.offset
    };
    const result: any = await getAllSurveyFeedback(data);
    logger.info('getAllSurveyFeedback', result);
    if (result.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        data: result
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {

    logger.error('getSurveyFeedbackListHandler', req.headers, req.query , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
