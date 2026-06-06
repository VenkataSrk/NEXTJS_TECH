/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */
import { createState, getStateListBy } from '../../dao/state';
import { RESPONSE } from '../../helpers/constants';
import { logger } from '../../plugins/winston';

/**
 *
 * @param req
 * @param res
 * @param done
 * ccs login api
 */
export async function createStateHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      ext: auth.ext,
      domainId: auth.domainId,
      startTimestamp: req?.body?.timestamp ?? 0,
      timerStatusId: req?.body?.state ?? null
    };
    const createStateRes: any = await createState(data);
    if (createStateRes) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.create_State });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {
    logger.error('createStateHandler', { data: req?.body, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getStateListByHandlers(req: any, res: any, done: any) {
  try {
    const data: any = {};
    data.ext = req.params.ext;
    data.domainId = req.headers.domainId;
    if (data.ext !== '') {
      const getStateList: any = await getStateListBy(data);
      if (Array.isArray(getStateList) && getStateList[0].length > 0) {
        res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getStateListRes: getStateList[0] });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getStateListRes: [] });
      }
    } else {
      res.status(200).send({ statusCode: 424, message: 'bid no should not be empty' });
    }
  } catch (err) {
    logger.error('getStateListByHandlers', { ext: req.params.ext, domainId: req.headers.domainId, error:err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
