import {
    callblastDashboardchartinfodao, callblastDashboardinfodao, callblastLeaderboardchartinfodao
} from '../../../dao/outboundcampaigndashboard.dao';
import { RESPONSE } from '../../../helpers/constants';
import { ioredisReport } from '../../../plugins/db';
import { logger } from '../../../plugins/log';

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function callblastDashboardinfoHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
      startDate:req?.body?.startDate ?? null,
      endDate:req?.body?.endDate ?? null,
      blastId:req?.body?.blastId ?? null,
      campaignType:req?.body?.campaignType,
      triggerType:req?.body?.triggerType,
      createdBy:req?.body?.createdBy,
      Status:req?.body?.Status,
    };
    const getoutboundDashboardDetails: any = await callblastDashboardinfodao(data);
    if (getoutboundDashboardDetails.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getoutboundDashboardRes: getoutboundDashboardDetails[0],
        CampaignName:getoutboundDashboardDetails[1],
        creatorName:getoutboundDashboardDetails[2],
        blastName:getoutboundDashboardDetails[3],
        dispositionStatus:getoutboundDashboardDetails[4],
        dispositionName:getoutboundDashboardDetails[5]

      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getoutboundDashboardRes: [] });
    }
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getoutboundDashboardDetails - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function callblastDashboardChartinfoHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
      datetype:req?.body?.datetype ?? null,
      startDateTime:req?.body?.startDateTime ?? null,
      endDateTime :req?.body?.endDateTime  ?? null,
      blastId :req?.body?.blastId  ?? null,
      campaignType :req?.body?.campaignType  ?? null,
      triggerType  :req?.body?.triggerType   ?? null,
      createdBy :req?.body?.createdBy  ?? null,
      Status  :req?.body?.Status   ?? null,

    };
    const getoutboundDashboardchartDetails: any = await callblastDashboardchartinfodao(data);
    if (getoutboundDashboardchartDetails.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getoutboundDashboardchartRes: getoutboundDashboardchartDetails,
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getoutboundDashboardchartRes: [] });
    }
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('callblastDashboardchartinfoHandler - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function callblastLeaderboardChartinfoHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
      startDate:req?.body?.startDate ?? null,
      endDate:req?.body?.endDate ?? null,
      blastId :req?.body?.blastId  ?? null,

    };
    const getoutboundDashboardchartDetails: any = await callblastLeaderboardchartinfodao(data);
    if (getoutboundDashboardchartDetails.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getoutboundDashboardchartRes: getoutboundDashboardchartDetails,
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getoutboundDashboardchartRes: [] });
    }
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('callblastLeaderboardchartinfoHandler - Unexpected Error:', err);
  }
}
