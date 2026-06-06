import { cloneDeep } from 'sequelize/types/utils';
import {
  agentEmailInboundMetricFilterDao,
  agentEmailPerformanceMetricFilterDao,
  agentEmailServicelevelMetricFilterDao,
  createEmailcolumnAnalyticsDao,
  emailInboundTrendChart,
  emailPerformancesTrendChart,
  emailServicelevelTrendChart,
  getCoulumnEmailAnalyticsDao
} from '../../../dao/emailAnalytics.dao';
import { RESPONSE } from '../../../helpers/constants';
import { logger } from '../../../plugins/log';

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentEmailInboundMetricsFilterHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: req?.headers?.domainId,
      offset: req?.body?.offset,
      limit: req?.body?.limit,
      agentId: req?.body?.agentId.length > 0 ? req?.body?.agentId : null,
    };
    const agentEmailInbound: any = await agentEmailInboundMetricFilterDao(data);
    if (agentEmailInbound && agentEmailInbound?.length > 0) {
      const AgentDetails = agentEmailInbound[0] ?? [];
      const AllAgentEmailInbound = agentEmailInbound[1] ?? [];
      const EmailInboundChart = agentEmailInbound[2] ?? [];
      res.status(200).send({
        AgentDetails,
        AllAgentEmailInbound,
        EmailInboundChart,
        statusCode: 200,
        message: RESPONSE.success_message,
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        result: [],
      });
    }
  } catch (err) {
    req.log.error(err);
    console.log(err);
    res.status(500).send({
      statusCode: 500,
      message: RESPONSE.internal_error,
    });
    logger.error('agentEmailInbound - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentEmailPerformanceMetricsFilterHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: req?.headers?.domainId,
      offset: req?.body?.offset,
      limit: req?.body?.limit,
      agentId: req?.body?.agentId.length > 0 ? req?.body?.agentId : null,
    };
    const agentEmailPerformance: any = await agentEmailPerformanceMetricFilterDao(data);
    if (agentEmailPerformance && agentEmailPerformance?.length > 0) {
      const AgentDetails = agentEmailPerformance[0] ?? [];
      const AllAgentEmailPerformance = agentEmailPerformance[1] ?? [];
      const EmailChartPerformance = agentEmailPerformance[2] ?? [];
      res.status(200).send({
        AgentDetails,
        AllAgentEmailPerformance,
        EmailChartPerformance,
        statusCode: 200,
        message: RESPONSE.success_message,
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        result: [],
      });
    }
  } catch (err) {
    req.log.error(err);
    console.log(err);
    res.status(500).send({
      statusCode: 500,
      message: RESPONSE.internal_error,
    });
    logger.error('agentEmailPerformance - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentEmailServicelevelMetricsFilterHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: req?.headers?.domainId,
      offset: req?.body?.offset,
      limit: req?.body?.limit,
      agentId: req?.body?.agentId.length > 0 ? req?.body?.agentId : null,
    };
    const agentEmailServicelevel: any = await agentEmailServicelevelMetricFilterDao(data);
    if (agentEmailServicelevel && agentEmailServicelevel?.length > 0) {
      const AgentDetails = agentEmailServicelevel[0] ?? [];
      const AllagentEmailServicelevel = agentEmailServicelevel[1] ?? [];
      const EmailServicelevelChart = agentEmailServicelevel[2] ?? [];
      res.status(200).send({
        AgentDetails,
        AllagentEmailServicelevel,
        EmailServicelevelChart,
        statusCode: 200,
        message: RESPONSE.success_message,
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        result: [],
      });
    }
  } catch (err) {
    req.log.error(err);
    console.log(err);
    res.status(500).send({
      statusCode: 500,
      message: RESPONSE.internal_error,
    });
    logger.error('agentEmailServicelevel - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function createEmailAnalyticColumnsHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      ext: auth.ext,
      editColumnNames: req?.body?.editColumnNames ?? [],
      id: req?.body?.id ?? null,
    };
    const EmailAnalyticsResult: any = await createEmailcolumnAnalyticsDao(data);
    res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: EmailAnalyticsResult[0] });
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('EmailAnalyticsResult - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getCoulumnEmailAnalyticsHandler(req: any, res: any) {
  try {
    const auth = req.headers;
    const data: any = {
      id: req?.params?.id ?? 0,
      domainId: auth.domainId,
      ext: auth.ext,
    };
    const EmailAnalyticsResult: any = await getCoulumnEmailAnalyticsDao(data);
    res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: EmailAnalyticsResult[0] });

  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('EmailAnalyticsResult - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentEmailInboundTrendChartHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: req?.headers?.domainId,
      columnName: req?.body?.columnName,
      dateType: req?.body?.datetype,
      agentId: req?.body?.agentId
    };
    const EmailTrendChat: any = await emailInboundTrendChart(data);
    if (EmailTrendChat && EmailTrendChat?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        ReceivedCurrentChatCount: EmailTrendChat[0],
        ReceivedPastChatCount: EmailTrendChat[1]
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('EmailInboundTrendChart - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentEmailPerformancesTrendChartHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp ?? null,
      endTimeStamp: req?.body?.endTimeStamp ?? null,
      domainId: req?.headers?.domainId ?? null,
      columnName: req?.body?.columnName ?? null,
      dateType: req?.body?.datetype ?? null,
      agentId: req?.body?.agentId ?? null
    };
    const EmailTrendChat: any = await emailPerformancesTrendChart(data);
    if (EmailTrendChat && EmailTrendChat?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        ReceivedCurrentChatCount: EmailTrendChat[0],
        ReceivedPastChatCount: EmailTrendChat[1]
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('EmailPerformancesTrendChart - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentEmailServicelevelTrendChartHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp ?? null,
      endTimeStamp: req?.body?.endTimeStamp ?? null,
      domainId: req?.headers?.domainId ?? null,
      columnName: req?.body?.columnName ?? null,
      dateType: req?.body?.datetype ?? null,
      agentId: req?.body?.agentId ?? null
    };
    const EmailTrendChat: any = await emailServicelevelTrendChart(data);
    if (EmailTrendChat && EmailTrendChat?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        ReceivedCurrentChatCount: EmailTrendChat[0],
        ReceivedPastChatCount: EmailTrendChat[1],
        AllagentEmailServicelevel: EmailTrendChat[2],
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('EmailServicelevelTrendChart - Unexpected Error:', err);
  }
}
