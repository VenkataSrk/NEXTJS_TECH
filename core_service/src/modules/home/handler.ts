/**
 *
 * @param req
 * @param res
 */

import { FastifyReply, FastifyRequest } from 'fastify';
import { deleteHomeChatHistoryDao, getHomeChatHistoryDao, insertUpdateHomeChatHistoryDao } from '../../dao/home';

export async function insertUpdateHomeChatHistory(req: FastifyRequest, res: FastifyReply) {
  try {
    const auth: any = req.headers;
    const payload: any = req.body;

    const body: any = {
      sessionId: payload?.sessionId ?? null,
      domainId: auth.domainId,
      title: payload?.title ?? null,
      agentId: payload?.agentId ?? null,
      message: payload?.message ? JSON.stringify(payload?.message) : null
    };
    const response: any = await insertUpdateHomeChatHistoryDao(body);
    if (response && (response[0]?.errcode === 0 || response[0]?.errcode === 1)) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: response[0] });
    } else {
      res.status(200).send({ statusCode: 404, message: response[0]?.errMsg || 'Insert/Update failed', result: [] });
    }
  } catch (err) {
    res.log.error(err);
    res.send({ statusCode: 500, message: 'Internal server error' });
  }
}

export async function getHomeChatHistory(req: FastifyRequest, res: FastifyReply) {
  try {
    const auth: any = req.headers;
    const payload: any = req.body;

    const body: any = {
      sessionId: payload?.sessionId ?? null,
      domainId: auth.domainId,
      agentId: payload?.agentId ?? null,
    };
    const response: any = await getHomeChatHistoryDao(body);
    if (response && response?.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: response[0] });
    } else {
      res.status(200).send({ statusCode: 404, message: 'No data Found', result: [] });
    }
  } catch (err) {
    res.log.error(err);
    res.send({ statusCode: 500, message: 'Internal server error' });
  }
}

export async function deleteHomeChatHistory(req: FastifyRequest, res: FastifyReply) {
  try {
    const params: any = req?.params;
    const auth: any = req.headers;

    const body: any = {
      sessionId: params?.sessionId ?? null,
      domainId: auth.domainId,
    };

    const response: any = await deleteHomeChatHistoryDao(body);
    if (response) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: response[0] });
    } else {
      res.status(200).send({ statusCode: 404, message: 'No data Found', result: [] });
    }
  } catch (err) {
    res.log.error(err);
    res.send({ statusCode: 500, message: 'Internal server error' });
  }
}
