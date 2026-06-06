/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import { logger } from '../../../src/plugins/winston';
import { callStatus, create, getVoicemailDetails, removeRegister } from '../../dao/callregister.dao';
import { RESPONSE } from '../../helpers/constants';
import { INcall, IRegisterDetails } from './interface';

/**
 * call register details
 * @param req
 * @param res
 * @param done
 */
export async function registerDetails(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const body: any = req.body;
    if (body.deviceToken === '') {
      res.status(200).send({ statusCode: 422, message: 'DeviceToken should not be empty' });
      return;
    }
    const data: IRegisterDetails = {
      ext: parseInt(auth.ext, 10),
      domainId: parseInt(auth.domainId, 10),
      domainName: auth.domainName,
      callerId: auth.callerId,
      deviceType: body.deviceType,
      deviceToken: body.deviceToken,
      authToken: auth.authorization
    };
    const result: any = await create(data);
    if (result && Array.isArray(result) && result?.[0]?.length > 0 && result?.[0]?.[0]?.errcode === 0) {
      res.status(200).send({ statusCode: 200, message: `Registration${result?.[0]?.[0]?.errmsg.split(':')[1]} Successfully` });
    } else {
      res.status(200).send({ statusCode: 422, message: 'Failed' });
    }
  } catch (err) {
    console.log(err);
    logger.error('registerDetails catch', { data: req?.body, Err: err, headers: req?.headers });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 * In call current available
 * @param req
 * @param res
 * @param done
 * @returns
 */
export async function inCall(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const body: any = req.query;
    const data: INcall = {
      status: parseInt(body.status, 10),
      agentId: parseInt(body.agentId, 10),
      domainId: parseInt(auth.domainId, 10),
    };
    const result: any = await callStatus(data);
    if (result && Array.isArray(result) && result?.[0]?.length > 0 && result?.[0]?.[0]?.errCode === 1) {
      res.status(200).send({ statusCode: 200, message: result?.[0]?.[0]?.errMsg });
    } else {
      res.status(200).send({ statusCode: 422, message: 'Failed' });
    }
  } catch (err) {
    console.log(err);
    logger.error('inCall catch', { query: req?.query, error: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 * Server Time
 * @param req
 * @param res
 * @param done
 * @returns
 */
export async function currentTime(req: any, res: any, done: any) {
  try {
    res.status(200).send({ statusCode: 200, message: 'Success', serverTime: Math.floor(Date.now() / 1000) });
  } catch (err) {
    console.log(err);
    logger.error('currentTime catch', 'error', { err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
export async function removeCallResgister(req: any, res: any) {
  try {
    const auth: any = req.headers;
    const body: any = req.body;

    const data = {
      domainId: parseInt(auth.domainId, 10),
      ext: parseInt(auth.ext, 10),
      deviceType: body?.deviceType
    };

    const response = await removeRegister(data);
    if (response && Array.isArray(response) && response?.[0]?.length > 0 && response?.[0]?.[0]?.Code === 1) {
      res.status(200).send({ statusCode: 200, message: `${response?.[0]?.[0]?.Msg}` });
    } else {
      res.status(200).send({ statusCode: 422, message: 'Failed' });
    }
  } catch (error) {
    console.log(error, 'removeCallResgister');
    logger.error('removeCallResgister catch', { data: req?.body, Err: error });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }

}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getVoicemailDetailsHandle(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    const getVoiceMail: any = await getVoicemailDetails(data);
    if (Array.isArray(getVoiceMail) && getVoiceMail[0]?.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getVoiceMailRes: getVoiceMail });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getVoiceMailRes: [] });
    }
  } catch (err) {
    logger.error('getVoicemailDetailsHandle catch', { data: req.headers?.domainId, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
