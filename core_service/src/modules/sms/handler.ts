/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import {
  getSupervisorDao,
} from '../../dao/sms';
import { logger } from '../../plugins/log';

import { RESPONSE } from '../../helpers/constants';

/**
 * get Supervisior List Handler
 * @param req
 * @param res
 */
export async function getSupervisiorListHandler(req: any, res: any) {
  try {
    const data: any = {};
    const auth: any = req.headers;
    data.domainId = auth.domainId;
    const supervisorList: any = await getSupervisorDao(data);
    logger.info('getSupervisorDao', supervisorList);
    if (supervisorList.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: supervisorList });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {

    logger.error('getSupervisiorListHandler', req.headers , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
