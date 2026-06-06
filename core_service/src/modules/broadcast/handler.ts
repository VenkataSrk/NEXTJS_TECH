import { deleteBroadcastAgentDetailsDao, deleteBroadcastGroupMsgDao, getAllGroupDetailsDao, getBroadCastGroupDao, getBroadcastGroupMsgDao, insertBroadCastDao, insertBroadcastGroupMsgDao } from '../../../src/dao/broadcast';
import { RESPONSE } from '../../helpers/constants';
import { ioredisWhatsapp } from '../../plugins/db';
import { logger } from '../../plugins/winston';
import { ICreateBroadCast, IDeleteAgentDtl, IDeleteBoardCastMsg, IGetAllGroup, IGetBoardCastMsg, IGetBroadCastGroup, IInsertBoardCastMsg } from './interface';

/**
 *
 * @param req
 * @param res
 * @param reply
 */
export async function insertBroadCastGroup(req: any, res: any, reply: any) {
  try {
    const body = req.body;
    const auth = req.headers;
    const roleRoom = `${auth?.domainId}`;
    const data: ICreateBroadCast = {
      domainId: auth.domainId,
      bcHistoryId:body?.bcHistoryId,
      bcGroupName: body?.bcGroupName,
      bcMembers: body?.bcMembers,
      createBy: body?.createBy,
    };
    const response: any = await insertBroadCastDao(data);
    logger.info('insertBroadCastDao', response);
    if (Array.isArray(response) && response?.length > 0 && response[0]?.errCode === 1) {
      ioredisWhatsapp.to(roleRoom).emit('admincreate_group', data);
      res.status(200).send({ statusCode: 200, message: response[0]?.errMsg, broadCastRes: response[0] });
    } else {
      res.status(200).send({ statusCode: 424, message: response[0]?.errMsg, broadCastRes: response[0] });
    }
  } catch (err) {
    logger.error('insertBroadCastGroup - Unexpected Error:', req.body, req.headers, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param reply
 */
export async function getAllGroupDetails(req: any, res: any, reply: any) {
  try {
    const auth = req.headers;
    const data: IGetAllGroup = {
      domainId: auth.domainId,
      userId:req.body.userId
    };
    const response: any = await getAllGroupDetailsDao(data);
    logger.info('getAllGroupDetailsDao:', response);
    if (Array.isArray(response) && response?.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, broadCastRes: response });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, broadCastRes: [] });
    }
  } catch (err) {
    logger.error('getAllGroupDetails - Unexpected Error:', req.headers, req.body, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param reply
 */
export async function getBroadCastAgentDetails(req: any, res: any, reply: any) {
  try {
    const body = req.body;
    const auth = req.headers;
    const data: IGetBroadCastGroup = {
      domainId: auth.domainId,
      bcGroupName: body.bcGroupName,
      bcMembers:  Array.isArray(body?.bcMembers) ? body.bcMembers.join(',') : body.bcMembers,
    };
    const response: any = await getBroadCastGroupDao(data);
    logger.info('Get particular internal chat response:', response);
    const filteredResponse = response.filter(item => Array.isArray(item));

    if (Array.isArray(response) && response?.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, broadCastRes: filteredResponse });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, broadCastRes: [] });
    }
  } catch (err) {
    logger.error('getBroadCastAgentDetails - Unexpected Error:', req.body, req.headers, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param reply
 */
export async function deleteBroadcastAgentDetails(req: any, res: any, reply: any) {
  try {
    const body = req.body;
    const auth = req.headers;
    const data: IDeleteAgentDtl = {
      domainId: auth.domainId,
      bcGroupName: body?.bcGroupName,
      bcMembers:  Array.isArray(body?.bcMembers) ? body.bcMembers.join(',') : body.bcMembers,
    };
    const response: any = await deleteBroadcastAgentDetailsDao(data);
    logger.info('deleteBroadcastAgentDetailsDao', response);
    if (Array.isArray(response) && response?.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, broadCastRes: response });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, broadCastRes: [] });
    }
  } catch (err) {
    logger.error('deleteBroadcastAgentDetails - Unexpected Error:', req.body, req.headers, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 *
 * @param req
 * @param res
 * @param reply
 */
export async function gettBroadcastGroupMsg(req: any, res: any, reply: any) {
  try {
    const body = req.body;
    const auth = req.headers;

    const data: IGetBoardCastMsg = {
      domainId:auth?.domainId,
      bcHistoryId: body?.bcHistoryId,
      AgentId: Array.isArray(body?.AgentId) ? body.AgentId.join(',') : body.AgentId,
    };
    const response: any = await getBroadcastGroupMsgDao(data);
    logger.info('getBroadcastGroupMsgDao', response);
    if (Array.isArray(response) && response?.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, broadCastRes: response });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, broadCastRes: [] });
    }
  } catch (err) {
    logger.error('gettBroadcastGroupMsg - Unexpected Error:', req.body, req.headers, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 *
 * @param req
 * @param res
 * @param reply
 */
export async function insertBroadcastGroupMsg(req: any, res: any, reply: any) {
  try {
    const body = req.body;
    const auth = req.headers;
    const agentExt = Array.isArray(body?.agentExts) ? body.agentExts.join(',') : body.agentExts;

    const data: IInsertBoardCastMsg = {
      bcHistoryId: body?.bcHistoryId,
      domainId:auth?.domainId,
      message: body?.message,
      AgentId: Array.isArray(body?.AgentId) ? body.AgentId.join(',') : body.AgentId,
      expiryDate: body?.expiryDate ?? null,
      sendTimestamp:body?.sendTimestamp ?? null
    };
    const response: any = await insertBroadcastGroupMsgDao(data);
    logger.info('insertBroadcastGroupMsgDao', response);
    if (Array.isArray(response) && response?.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, broadCastRes: response });
      const finalData = { ...data, bcDurationHistoryId : response[0]?.bcDurationHistoryId };
      const agentIds = agentExt.split(',');
      agentIds.forEach((agentId: string) => {
        const room = `${auth?.domainId}_${agentId}`;
        const roleRoom = `${auth?.domainId}`;
        ioredisWhatsapp.to(room).emit('broadcast_message', finalData);
        ioredisWhatsapp.to(roleRoom).emit('broadcast_message', finalData);

      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, broadCastRes: [] });
    }
  } catch (err) {
    logger.error('insertBroadcastGroupMsg - Unexpected Error:', req.body, req.headers, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 *
 * @param req
 * @param res
 * @param reply
 */
export async function deleteBroadcastGroupMsg(req: any, res: any, reply: any) {
  try {
    const body = req.body;
    const auth = req.headers;
    const roleRoom = `${auth?.domainId}`;
    const data: IDeleteBoardCastMsg = {
      bcDurationHistoryId: body?.bcDurationHistoryId,
      bcHistoryId: body?.bcHistoryId,
      domainId: auth.domainId,
    };
    const response: any = await deleteBroadcastGroupMsgDao(data);
    logger.info('deleteBroadcastGroupMsgDao', response);
    if (Array.isArray(response) && response?.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, broadCastRes: response });
      ioredisWhatsapp.to(roleRoom).emit('admindelete_message', data);
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, broadCastRes: [] });
    }
  } catch (err) {
    logger.error('deleteBroadcastGroupMsg - Unexpected Error:', req.body, req.headers, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
