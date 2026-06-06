/**
 * @createdBy <kathiravan.r@worktual.com>
 * @createdOn <25/06/2024>
 */

import { getAiBotPerformancesBotAgentReportDao, getAiBotPerformancesReportDao, getAiCallQualityReportDao, getAiChatQualityReportDao } from '../../dao/aiBotReports.dao';
import { RESPONSE } from '../../helpers/constants';
import { logger } from '../../plugins/log';

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getAiCallQualityReportHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      timeZone: req?.body?.timezone ?? null,
      domainId: auth.domainId,
      queueId: req?.body?.queueId ? req?.body?.queueId : null,
      agentId:req?.body?.agentName ? req?.body?.agentName : null,
      direction: req?.body?.direction ? req?.body?.direction : null,
      limit: req?.body?.limit ?? 0,
      offset: req?.body?.offset ?? 0,
    };
    const getAiCallQualityReport: any = await getAiCallQualityReportDao(data);
    const totalRecords: any = getAiCallQualityReport[2][0]?.v_Total_Count;
    if (getAiCallQualityReport && getAiCallQualityReport.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        type: 'callQualityReport',
        totalCount: totalRecords,
        queueList: getAiCallQualityReport[0],
        agentList: getAiCallQualityReport[1],
        reportList: getAiCallQualityReport[3],
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getAiCallQualityReportRes: [] });
    }
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getAiCallQualityReportHandler - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getAiChatQualityReportHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      timeZone: req?.body?.timezone ?? null,
      domainId: auth.domainId,
      queueId: req?.body?.queueId ? req?.body?.queueId : null,
      agentId:req?.body?.agentName ? req?.body?.agentName : null,
      direction: req?.body?.direction ? req?.body?.direction : null,
      limit: req?.body?.limit ?? 0,
      offset: req?.body?.offset ?? 0,
    };
    const getAiChatQualityReport: any = await getAiChatQualityReportDao(data);
    const totalRecords: any = getAiChatQualityReport[2][0]?.v_Total_Count;
    if (getAiChatQualityReport && getAiChatQualityReport.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        type: 'chatQualityReport',
        totalCount: totalRecords,
        queueList: getAiChatQualityReport[0],
        agentList: getAiChatQualityReport[1],
        reportList: getAiChatQualityReport[3],
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getAiChatQualityReport: [] });
    }
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getAiChatQualityReportHandler - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getAiBotPerformancesReportHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      timeZone: req?.body?.timezone ?? null,
      domainId: auth.domainId,
      agentId:req?.body?.agentName ? req?.body?.agentName : null,
      limit: req?.body?.limit ?? 0,
      offset: req?.body?.offset ?? 0,
    };
    const getAiBotPerformancesReport: any = await getAiBotPerformancesReportDao(data);
    const totalRecords = getAiBotPerformancesReport[1][0]?.totalCount ?? 0;
    if (getAiBotPerformancesReport && getAiBotPerformancesReport.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        type: 'AiBotPerformancesReport',
        totalCount: totalRecords,
        agentList: getAiBotPerformancesReport[0],
        reportList: getAiBotPerformancesReport[2],
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getAiBotPerformancesReport: [] });
    }
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getAiBotPerformancesReportHandler - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getAiBotPerformancesBotAgentReportHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      timeZone: req?.body?.timezone ?? null,
      domainId: auth.domainId,
      agentId:req?.body?.agentName ? req?.body?.agentName : null,
      queueId: req?.body?.queueId ? req?.body?.queueId : null,
      limit: req?.body?.limit ?? 0,
      offset: req?.body?.offset ?? 0,
    };
    const getAiBotPerformancesBotAgentReport: any = await getAiBotPerformancesBotAgentReportDao(data);
    const totalRecords = getAiBotPerformancesBotAgentReport[2][0]?.v_Total_Count ?? 0;
    if (getAiBotPerformancesBotAgentReport && getAiBotPerformancesBotAgentReport.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        type: 'AiBotPerformancesBotAgentReport',
        totalCount: totalRecords,
        queueList: getAiBotPerformancesBotAgentReport[0],
        agentList: getAiBotPerformancesBotAgentReport[1],
        reportList: getAiBotPerformancesBotAgentReport[3],
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getAiBotPerformancesBotAgentReport: [] });
    }
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getAiBotPerformancesBotAgentReportHandler - Unexpected Error:', err);
  }
}
