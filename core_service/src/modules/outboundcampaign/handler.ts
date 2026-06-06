/**
 *
 * @param req
 * @param res
 */

import { FastifyReply, FastifyRequest } from 'fastify';
import { getOutboundCampaignDraftDao, insertUpdateOutboundCampaignDao } from '../../../src/dao/outboundcampaignDao';
import { RESPONSE } from '../../helpers/constants';

export async function insertUpdateOutboundCampaign(req: FastifyRequest, res: FastifyReply) {
  try {
    const auth: any = req.headers;
    const payload: any = req.body;
    const body: any = {
      ...payload,
      domainId: auth.domainId
    };
    const response: any = await insertUpdateOutboundCampaignDao(body);
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

export async function getOutboundCampaignDraftChats(req: any, res: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
      offset:req?.body?.offset,
      limit:req?.body?.limit,
    };
    const response: any = await getOutboundCampaignDraftDao(data);
    if (Array.isArray(response?.[0]) && response[0]?.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: response[0] });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.failed , result: [] });
    }
  } catch (err) {
    res.log.error(err);
    res.send({ statusCode: 500, message: 'Internal server error' });
  }
}
