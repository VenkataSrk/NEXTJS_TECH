import { insertUpdateVistorDao } from '../../dao/chatBot';
import { RESPONSE } from '../../helpers/constants';
import { logger } from '../../plugins/winston';

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function insertUpdateVistorHandler(req: any, res: any, done: any) {
  try {
    const data: any = {};
    data.visitorInsightId = req?.body?.visitorInsightId ?? null,
            data.customerId = req?.body?.customerId ?? null,
            data.visitorId = req?.body?.visitorId ?? null,
            data.isNew = req?.body?.isNew ?? null,
            data.statusTypeId = req?.body?.statusTypeId ?? null,
            data.startTime = req?.body?.startTime ?? null,
            data.endTime = req?.body?.endTime ?? null,
            data.domainId = req?.body?.domainId ?? null,
            data.ipAddress = req?.body?.ipAddress ?? null,
            data.location = req?.body?.location ?? null,
            data.browser = req?.body?.browser ?? null,
            data.device = req?.body?.device ?? null,
            data.entryPage = req?.body?.entryPage ?? null,
            data.isVisible  = req?.body?.isVisible ?? null;
    const insertUpdateVistorResult = await insertUpdateVistorDao(data);
    logger.info('insertUpdateVistorDao', insertUpdateVistorResult);
    res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: insertUpdateVistorResult });
  } catch (err) {

    logger.error('callflowMapHandler - Unexpected Error:', req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
