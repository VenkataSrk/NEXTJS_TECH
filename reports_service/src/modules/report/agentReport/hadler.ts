import { CLIENT_RENEG_LIMIT } from 'tls';
import {
  getAgentCallSummaryReport,
  getAgentFeedbackReportDao,
  getAgentLoginAndLogoutReportDao,
  getAgentPerformanceHandlerDao,
  getAgentStatusReportReportDao,
  getCsatAgentFeedbackReportDao,
  getParticularAgentReportDao,
  getQueryHandlingTimeReportDao
} from '../../../dao/agentReport.dao';
import { RESPONSE } from '../../../helpers/constants';
import { logger } from '../../../plugins/log';

export async function getAgentCallSummmaryReportHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      domainId: req?.headers?.domainId,
      agentName: req?.body?.agentName,
      routingId: req?.body?.routingId,
      customerId: req?.body?.customerId,
      limit: req?.body?.limit,
      offset: req?.body?.offset,
    };

    const response: any = await getAgentCallSummaryReport(data);

    if (Array.isArray(response) && response.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        totalCount: response[1][0]?.v_Total_Count,
        agentList: response[0],
        reportList: response[2],
      });
    } else {
      res.status(200).send({
        statusCode: 200,
        message: 'No data found',
        reportList: [],
      });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getAgentCallSummmaryReportHandler - Unexpected Error:', err);
  }
}

export async function getAgentLoginAndLogoutReportHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      timeZone: null,
      domainId: req?.headers?.domainId,
      agentName: req?.body?.agentName,
      limit: req?.body?.limit,
      offset: req?.body?.offset
    };
    const getAgentLoginAndLogoutReportRes: any = await getAgentLoginAndLogoutReportDao(data);
    if (Array.isArray(getAgentLoginAndLogoutReportRes) && getAgentLoginAndLogoutReportRes.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        totalCount: getAgentLoginAndLogoutReportRes[1][0]?.v_Total_Count,
        agentList: getAgentLoginAndLogoutReportRes[0],
        reportList: getAgentLoginAndLogoutReportRes[2],
      });
    } else {
      res.status(200).send({
        statusCode: 200,
        message: 'No data found',
        reportList: [],
      });
    }

  } catch (error) {
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getAgentLoginAndLogoutReportHandler - Unexpected Error:', error);
  }
}
export async function getAgentStatusReportReportHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      timeZone: null,
      domainId: req?.headers?.domainId,
      agentName: req?.body?.agentName,
      limit: req?.body?.limit,
      offset: req?.body?.offset,
    };
    const getAgentStatusReportReportRes: any = await getAgentStatusReportReportDao(data);
    if (Array.isArray(getAgentStatusReportReportRes) && getAgentStatusReportReportRes.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        totalCount: getAgentStatusReportReportRes[1][0]?.v_Total_Count,
        agentList: getAgentStatusReportReportRes[0],
        reportList: getAgentStatusReportReportRes[2],
      });
    } else {
      res.status(200).send({
        statusCode: 200,
        message: 'No data found',
        reportList: [],
      });
    }

  } catch (error) {
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getAgentStatusReportReportHandler - Unexpected Error:', error);
  }
}
export async function getQueryHandlingTimeReportHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      timeZone: null,
      domainId: req?.headers?.domainId,
      agentName: req?.body?.agentName,
      limit: req?.body?.limit,
      offset: req?.body?.offset,
    };
    const getQueryHandlingTimeReportRes: any = await getQueryHandlingTimeReportDao(data);
    if (Array.isArray(getQueryHandlingTimeReportRes) && getQueryHandlingTimeReportRes.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        totalCount: getQueryHandlingTimeReportRes[1][0]?.v_Total_Count,
        agentList: getQueryHandlingTimeReportRes[0],
        reportList: getQueryHandlingTimeReportRes[2],
      });
    } else {
      res.status(200).send({
        statusCode: 200,
        message: 'No data found',
        reportList: [],
      });
    }

  } catch (error) {
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getQueryHandlingTimeReportHandler - Unexpected Error:', error);
  }
}

export async function getAgentFeedbackReportHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      timeZone: null,
      domainId: req?.headers?.domainId,
      agentName: req?.body?.agentName,
      limit: req?.body?.limit,
      offset: req?.body?.offset,
    };
    const getAgentFeedbackReportRes: any = await getAgentFeedbackReportDao(data);
    if (Array.isArray(getAgentFeedbackReportRes) && getAgentFeedbackReportRes.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        totalCount: getAgentFeedbackReportRes[1][0]?.v_Total_Count,
        agentList: getAgentFeedbackReportRes[0],
        reportList: getAgentFeedbackReportRes[2],
      });
    } else {
      res.status(200).send({
        statusCode: 200,
        message: 'No data found',
        reportList: [],
      });
    }

  } catch (error) {
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getAgentFeedbackReportHandler - Unexpected Error:', error);
  }
}

export async function getAgentPerformanceHandler(req: any, res: any) {
  try {
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      timeZone: null,
      domainId: req?.headers?.domainId,
      agentName: req?.body?.agentName,
      limit: req?.body?.limit,
      offset: req?.body?.offset,
    };

    const getAgentPerformanceHandlerRes: any = await getAgentPerformanceHandlerDao(data);

    if (Array.isArray(getAgentPerformanceHandlerRes) && getAgentPerformanceHandlerRes.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        totalCount: getAgentPerformanceHandlerRes[1][0]?.v_Total_Count,
        agentList: getAgentPerformanceHandlerRes[0],
        reportList: getAgentPerformanceHandlerRes[2],
      });
    } else {
      res.status(200).send({
        statusCode: 200,
        message: 'No data found',
        reportList: [],
      });
    }

  } catch (error) {
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getAgentPerformanceHandlerRes - Unexpected Error:', error);
  }
}

/**
 *
 * @param data
 * @returns
 */
export async function getCsatAgentFeedbackReportHandler(req: any, res: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      timeZone: null,
      domainId: auth?.domainId,
      agentId: req?.body?.agentName,
      Limit: req?.body?.limit,
      Offset: req?.body?.offset,
      customerFeedbackId:req?.body?.customerFeedbackId
    };
    const getCsatAgentFeedback: any = await getCsatAgentFeedbackReportDao(data);
    if (Array.isArray(getCsatAgentFeedback) && getCsatAgentFeedback.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        reportList: getCsatAgentFeedback[0],
      });
    } else {
      res.status(200).send({
        statusCode: 200,
        message: 'No data found',
        reportList: [],
      });
    }
  } catch (error) {
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getCsatAgentFeedbackReportHandler - Unexpected Error:', error);
  }
}

/**
 *
 * @param data
 * @returns
 */
export async function getParticularAgentReportHandler(req: any, res: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      domainId: auth?.domainId,
      agentId: req?.body?.agentId,
      statusName: req?.body?.statusName,
      Limit: req?.body?.limit,
      Offset: req?.body?.offset,
    };
    const getParticularAgentReport: any = await getParticularAgentReportDao(data);
    if (Array.isArray(getParticularAgentReport) && getParticularAgentReport.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        particularAgentstatus: getParticularAgentReport[0],
        overallAgentstatus: getParticularAgentReport[1],
        presenceStatus: getParticularAgentReport[2],
      });
    } else {
      res.status(200).send({
        statusCode: 200,
        message: 'No data found',
        reportList: [],
      });
    }
  } catch (error) {
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getParticularAgentReportHandler - Unexpected Error:', error);
  }
}
