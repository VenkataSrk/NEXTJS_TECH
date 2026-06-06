import axios from 'axios';
import { configure, getLogger } from 'log4js';
import moment from 'moment';
import { deleteConversationTriggerDao, getConversationTriggersDao, insertConversationTriggerDao } from '../../../src/dao/conversationTrigger.dao';
import { RESPONSE } from '../../helpers/constants';
import { Errorlogger } from '../../plugins/errorLog';
import { logger } from '../../plugins/winston';

/**
 *
 * @param req
 * @param res
 * @param done
 * Conversation Triggers api
 */
export async function insertconversationtriggerHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      triggerid: req?.body?.triggerid ?? null,
      domainId: auth.domainId,
      triggerName: req?.body?.triggerName ?? null,
      description: req?.body?.description ?? null,
      keywords: req?.body?.keywords ?? null,
    };
    const insertResult: any = await insertConversationTriggerDao(data);
    logger.info('insertConversationTriggerDao', insertResult);
    if (insertResult && insertResult.length > 0) {
      res.status(200).send({ statusCode: 200, insertResult: insertResult[0], message: RESPONSE.ConversationTriggers });
    } else {
      res.status(400).send({ statusCode: 400, message: 'Bad Request: Invalid or missing data', result: [] });
    }
  } catch (err) {
    logger.error('insertconversationtriggerHandler - Unexpected Error:', req.headers, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * Conversation Triggers api
 */
export async function getConversationTriggersHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      triggerid: req?.params?.triggerid ?? null,
      domainId: auth.domainId,
    };
    const insertResult: any = await getConversationTriggersDao(data);
    logger.info('getConversationTriggersDao', insertResult);
    if (insertResult && insertResult.length > 0) {
      res.status(200).send({ statusCode: 200, insertResult: insertResult[0], message: RESPONSE.success_message });
    } else {
      res.status(404).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    logger.error('getConversationTriggersHandler - Unexpected Error:', req.headers, req.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * Conversation Triggers api
 */
export async function deleteConversationTriggerHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      triggerid: req?.params?.triggerid,
      domainId: auth?.domainId,
    };
    const deleteType: any = await deleteConversationTriggerDao(data);
    logger.info('deleteConversationTriggerDao', deleteType);
    if (deleteType[0]?.errcode === 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.delete_conversation_trigger });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {
    logger.error('deleteConversationTriggerHandler - Unexpected Error:', req.headers, req.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * error log
 */
export async function loggerDetailsHandler(req: any, res: any) {
  try {
    const { message, stack, url, domainId } = req.body;

    const timestamp = moment().format('YYYY-MM-DD hh:mm A');

    Errorlogger.info('FRONTEND ERROR START');
    Errorlogger.info(`Timestamp : ${timestamp}`);
    Errorlogger.info(`Message   : ${message}`);
    Errorlogger.info(`Stack     : ${stack}`);
    Errorlogger.info(`URL       : ${url}`);
    Errorlogger.info(`Domain ID : ${domainId}`);
    Errorlogger.info('FRONTEND ERROR END');

    res.status(200).send({ statusCode: 200, message: 'Error logged successfully' });
  } catch (err) {
    Errorlogger.error('Logger failed: ' , err);
    logger.error('loggerDetailsHandler - Unexpected Error:', req.body, err);
    res.status(500).send({ statusCode: 500, message: 'Internal Server Error' });
  }
}
