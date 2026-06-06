import {
    agentCampaignDao, summaryCampaignDao
} from '../../../dao/outboundCampaignReport.dao';
import { RESPONSE } from '../../../helpers/constants';
import { ioredisReport } from '../../../plugins/db';
import { logger } from '../../../plugins/log';

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentCampaignmetricsHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
      startDate:req?.body?.startDate ?? null,
      endDate:req?.body?.endDate ?? null,
    };
    const getoutboundReportDetails: any = await agentCampaignDao(data);
    if (getoutboundReportDetails.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getoutboundReportRes: getoutboundReportDetails[0]
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getoutboundReportRes: [] });
    }
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('agentcampaignmetricshandler - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function summaryCampaignmetricsHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
      startDate:req?.body?.startDate ?? null,
      endDate:req?.body?.endDate ?? null,
    };
    const getoutboundReportDetails: any = await summaryCampaignDao(data);
    if (getoutboundReportDetails.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getoutboundReportRes: getoutboundReportDetails[0]
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getoutboundReportRes: [] });
    }
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('summarycampaignmetricshandler - Unexpected Error:', err);
  }
}
