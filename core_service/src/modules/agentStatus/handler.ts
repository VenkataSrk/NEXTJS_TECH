/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */
import {
  deleteAgentStatus,
  getAgentStatus,
  getAgentStatusDesc,
  getAgentStatusDetails,
  getEmailHistory,
  insertAgentStatus,
  isValidAgentName,
  updateAgentStatus,
  updateAgentStatusValues,
} from '../../dao/agentStatus';
import { ioredisWhatsapp } from '../../plugins/db';

import { RESPONSE } from '../../helpers/constants';
import { logger } from '../../plugins/winston';

/**
 *
 * @param req
 * @param res
 * @param done
 * agent status api
 */
export async function insertAgentStatusHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      aid: req?.body?.aid ?? null,
      statusName: req?.body?.statusName ?? null,
      type: 'Custom',
      description: req?.body?.description ?? null,
      domainId: req?.headers?.domainId,
      channels: req?.body?.channels ?? null,
      status: req?.body?.status ?? 0,
      isVoice: req?.body?.isVoice ?? 0,
      isVoiceCallTransfer: req?.body?.isVoiceCallTransfer ?? 0,
      isChat: req?.body?.isChat ?? 0,
      isChatTransfer: req?.body?.isChatTransfer ?? 0,
      isEmail: req?.body?.isEmail ?? 0,
      isEmailTransfer: req?.body?.isEmailTransfer ?? 0,
      colorCode: req?.body?.colorCode ?? null,
    };
    const insertStatus: any = await insertAgentStatus(data);
    logger.info('insertAgentStatus', insertStatus);
    if (insertStatus) {
      ioredisWhatsapp.to(`${data.domainId}`).emit('presenceStatusrestriction');
      res.status(200).send({ statusCode: 200, message: RESPONSE.agent_status, insertAgentStatus: insertStatus ?? {} });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {

    logger.error('insertAgentStatusHandler - Unexpected Error:', req?.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getAgentStatusHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      domainId: req?.headers?.domainId
    };
    const getAgent: any = await getAgentStatus(data);
    logger.info('getAgentStatus', getAgent);
    if (Array.isArray(getAgent) && getAgent.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getAgentListRes: getAgent });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getAgentListRes: [] });
    }
  } catch (err) {

    logger.error('getAgentStatusHandler - Unexpected Error:', req?.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getAgentStatusDescHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      domainId: req?.headers?.domainId
    };
    const getAgentDesc: any = await getAgentStatusDesc(data);
    logger.info('getAgentStatusDesc', getAgentDesc);
    if (Array.isArray(getAgentDesc) && getAgentDesc.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getAgentListRes: getAgentDesc });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getAgentListRes: [] });
    }
  } catch (err) {

    logger.error('getAgentStatusHandler - Unexpected Error:', req?.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getAgentStatusDetailsHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const aid = req.params.aid;
    const data: any = {
      domainId: auth.domainId,
    };
    if (aid !== '') {
      const getAgent: any = await getAgentStatusDetails(data, aid);
      logger.info('getAgentStatusDetails', getAgent);
      if (Array.isArray(getAgent) && getAgent.length > 0) {
        res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getAgentDetailsRes: getAgent });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getAgentDetailsRes: [] });
      }
    } else {
      res.status(200).send({ statusCode: 405, message: RESPONSE.aid });
    }
  } catch (err) {

    logger.error('getAgentStatusDetailsHandler - Unexpected Error:', req?.headers, req.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * agent status api
 */
export async function updateAgentStatusHandler(req: any, res: any, done: any) {
  try {
    const aid = req.params.aid;
    const data: any = {
      statusName: req?.body?.statusName ?? null,
      type: req?.body?.type ?? null,
      description: req?.body?.description ?? null,
      domainId: req?.headers?.domainId,
      channels: req?.body?.channels ?? null,
      status: req?.body?.status ?? 0,
      isVoice: req?.body?.isVoice ?? 0,
      isVoiceCallTransfer: req?.body?.isVoiceCallTransfer ?? 0,
      isChat: req?.body?.isChat ?? 0,
      isChatTransfer: req?.body?.isChatTransfer ?? 0,
      isEmail: req?.body?.isEmail ?? 0,
      isEmailTransfer: req?.body?.isEmailTransfer ?? 0,
      colorCode: req.body?.colorCode ?? null,
    };
    if (aid !== '') {
      const updateStatus: any = await updateAgentStatus(data, aid);
      logger.info('updateAgentStatus', updateStatus);
      if (Array.isArray(updateStatus) && updateStatus?.[0]?.errcode === 0) {
        ioredisWhatsapp.to(`${data.domainId}`).emit('presenceStatusrestriction');
        res.status(200).send({ statusCode: 200, message: RESPONSE.agent_statuss });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_update });
      }
    } else {
      res.status(200).send({ statusCode: 405, message: RESPONSE.aid });
    }
  } catch (err) {

    logger.error('updateAgentStatus - Unexpected Error:', req.params, req?.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function deleteAgentStatusHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const aid: any = req.params.aid;
    const domainId = auth.domainId;
    if (aid !== '') {
      const deleteAgent: any = await deleteAgentStatus(aid, domainId);
      logger.info('deleteAgentStatus', deleteAgent);
      if (deleteAgent) {
        ioredisWhatsapp.to(`${auth.domainId}`).emit('presenceStatusrestriction');
        res.status(200).send({ statusCode: 200, message: RESPONSE.agentt_statuss });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
      }
    } else {
      res.status(200).send({ statusCode: 405, message: RESPONSE.aid });
    }
  } catch (err) {

    logger.error('deleteAgentStatusHandler - Unexpected Error:', req.headers, req.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function isValidAgentNameHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      statusName: req?.body?.name ?? null,
      domainId: auth?.domainId,
    };
    const agentRes: any = await isValidAgentName(data);
    logger.info('isValidAgentName', agentRes);
    if (agentRes && agentRes[0]?.length > 0 && agentRes[0][0]?.flag === 0) {
      res.status(200).send({ statusCode: 403, message: RESPONSE.name_already_exist, flag: 0 });
    } else {
      res.status(200).send({ statusCode: 200, message: RESPONSE.available, flag: 1 });
    }
  } catch (err) {

    logger.error('isValidAgentNameHandler - Unexpected Error:', req.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function updateAgentStatusValueHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const emailId: any = req.params.email;
    const data: any = {
      domainId: auth.domainId,
      isVoice: req?.body?.isVoice ?? 0,
      isVoiceCallTransfer: req?.body?.isVoiceCallTransfer ?? 0,
      isChat: req?.body?.isChat ?? 0,
      isChatTransfer: req?.body?.isChatTransfer ?? 0,
      isEmail: req?.body?.isEmail ?? 0,
      isEmailTransfer: req?.body?.isEmailTransfer ?? 0,
      statusName: req?.body?.statusName ?? null,
    };
    if (emailId) {
      const updatequeue: any = await updateAgentStatusValues(data, emailId);
      logger.info('updateAgentStatusValues', updatequeue);
      if (updatequeue) {
        res.status(200).send({
          statusCode: 200,
          message: RESPONSE.agent_status_updated,
        });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_update });
      }
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.please_provider });
    }
  } catch (err) {

    logger.error('updateAgentStatusValueHandler - Unexpected Error:', req.headers, req.params, req?.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

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

    logger.error('getEmailHistoryHandler - Unexpected Error:', req.headers, req?.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
};
