
/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */
import { create, getHistoryId, getInternalChat, getParticularInternalChat, isDelete, isEdit, markAsRead } from '../../dao/internalChat';
import { RESPONSE } from '../../helpers/constants';
import { ioredisWhatsapp } from '../../plugins/db';
import { logger } from '../../plugins/log';
import { ICreateinternalChat, IDelete, IEdit, IGetinternalChat, IGetparticularInternalChat, IInternalChatByHistoryId, IReadInternalChat } from './interface';

/**
 *
 * @param req
 * @param res
 * @param reply
 */
export async function createChat(req: any, res: any, reply: any) {
  try {
    const body = req.body;
    const auth = req.headers;
    const data: ICreateinternalChat = {
      groupName: body.groupName,
      historyId: body.historyId,
      channelType: body.channelType,
      agentExt: body.agentExt,
      domainId: auth.domainId,
      agentUserId: body.agentUserId,
      queueId: !body?.queueId ? null : body?.queueId,
      customerId: body.customerId,
      hostUserId: body.hostUserId,
      internalChatGroupId: body?.internalChatGroupId ?? null
    };
    const response: any = await create(data);
    logger.info('create', response);
    if (Array.isArray(response) && response?.length > 0 && response[0]?.errCode === 1) {
      ioredisWhatsapp.to(data.agentExt).emit('create_internal_chat', { ...response[0], ...data });
      res.status(200).send({ statusCode: 200, message: response[0]?.errMsg, internalChatRes: response[0] });
    } else if (Array.isArray(response) && response?.length > 0 && response[0]?.errCode === 2) {
      ioredisWhatsapp.to(data.agentExt).emit('update_internal_chat', { ...response[0], ...data });
      res.status(200).send({ statusCode: 200, message: response[0]?.errMsg, internalChatRes: response[0] });
    } else if (Array.isArray(response) && response?.length > 0 && response[0]?.errCode === -2) {
      res.status(200).send({ statusCode: 422, message: response[0]?.errMsg, internalChatRes: response[0] });
    } else {
      res.status(200).send({ statusCode: 424, message: RESPONSE.failed });
    }
  } catch (err) {
    logger.error('createChat', reply.headers, req.body, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param reply
 */
export async function getChat(req: any, res: any, reply: any) {
  try {
    const body = req.query;
    const auth = req.headers;
    const data: IGetinternalChat = {
      hostUserId: body.hostUserId,
      domainId: auth.domainId,
    };
    const response: any = await getInternalChat(data);
    logger.info('getInternalChat', response);
    if (Array.isArray(response) && response?.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, internalChatRes: response });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, internalChatRes: [] });
    }
  } catch (err) {
    logger.error('getChat', req.headers, reply.query, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param reply
 */
export async function getParticularChat(req: any, res: any, reply: any) {
  try {
    const body = req.query;
    const auth = req.headers;
    const data: IGetparticularInternalChat = {
      hostUserId: body.hostUserId,
      domainId: auth.domainId,
      internalChatGroupId: body.internalChatGroupId
    };
    const response: any = await getParticularInternalChat(data);
    logger.info('getParticularInternalChat', response);
    if (Array.isArray(response) && response?.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, internalChatRes: response });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, internalChatRes: [] });
    }
  } catch (err) {
    logger.error('getParticularChat', req.headers, req.query, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param reply
 */
export async function edit(req: any, res: any, reply: any) {
  try {
    const body = req.body;
    const auth = req.headers;
    const data: IEdit = {
      agentId: body.agentId,
      domainId: auth.domainId,
      internalChatId: body.internalChatId,
      msg: body.msg,
      agentExt: body.agentExt
    };
    const response: any = await isEdit(data);
    logger.info('isEdit', response);
    if (Array.isArray(response) && response?.length > 0) {
      ioredisWhatsapp.to(data.agentExt).emit('edit_internal_chat', data);
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message });
    } else {
      res.status(200).send({ statusCode: 424, message: RESPONSE.failed });
    }
  } catch (err) {
    logger.error('edit', req.headers, req.body, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param reply
 */
export async function deleteChat(req: any, res: any, reply: any) {
  try {
    const body = req.body;
    const auth = req.headers;
    const data: IDelete = {
      agentId: body.agentId,
      domainId: auth.domainId,
      internalChatId: body.internalChatId,
      agentExt: body.agentExt
    };
    const response: any = await isDelete(data);
    logger.info('isDelete', response);
    if (Array.isArray(response) && response?.length > 0) {
      ioredisWhatsapp.to(data.agentExt).emit('delete_internal_chat', data);
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message });
    } else {
      res.status(200).send({ statusCode: 424, message: RESPONSE.failed });
    }
  } catch (err) {
    logger.error('deleteChat', req.headers, req.body, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param reply
 */
export async function markAllAsRead(req: any, res: any, reply: any) {
  try {
    const body = req.body;
    const auth = req.headers;
    const data: IReadInternalChat = {
      agentId: body.agentId,
      domainId: auth.domainId,
      internalChatGroupId: body.internalChatGroupId,
      agentExt: body.agentExt,
      msgStatus: body.msgStatus
    };
    const response: any = await markAsRead(data);
    logger.info('markAsRead', response);
    if (Array.isArray(response) && response?.length > 0) {
      ioredisWhatsapp.to(data.agentExt).emit('markAsRead_internal_chat', data);
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message });
    } else {
      res.status(200).send({ statusCode: 424, message: RESPONSE.failed });
    }
  } catch (err) {
    logger.error('markAllAsRead', req.headers, req.body, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param reply
 */
export async function getInternalChatByHistoryId(req: any, res: any, reply: any) {
  try {
    const body = req.body;
    const auth = req.headers;
    const data: IInternalChatByHistoryId = {
      agentId: body.agentId,
      domainId: auth.domainId,
      historyId: body.historyId,
      channelType: body.channelType
    };
    const response: any = await getHistoryId(data);
    logger.info('getHistoryId', response);
    if (Array.isArray(response) && response?.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, internalChatRes: response });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.failed, internalChatRes: [] });
    }
  } catch (err) {
    logger.error('getInternalChatByHistoryId', req.headers, req.body, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
