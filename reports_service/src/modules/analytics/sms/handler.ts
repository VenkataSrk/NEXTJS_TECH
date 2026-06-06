import { cloneDeep } from 'sequelize/types/utils';
import {
  agentSmsInboundMetricFilterDao,
  agentSmsPerformanceMetricFilterDao,
  agentSmsServiceLevelMetricFilterDao,
  createSmsAnalyticsDao,
  getCoulumnSmsAnalyticsDao,
  smsInboundTrendChartDao,
  smsPerformancesTrendChartDao,
  smsServicelevelTrendChartDao
} from '../../../dao/smsAnalytics.dao';
import { RESPONSE } from '../../../helpers/constants';
import { logger } from '../../../plugins/log';

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentSmsInboundMetricsFilterHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: req?.headers?.domainId,
      offset: req?.body?.offset,
      limit: req?.body?.limit,
      agentId: req?.body?.agentId.length > 0 ? req?.body?.agentId : null,
    };
    const agentSmsInbound: any = await agentSmsInboundMetricFilterDao(data);
    if (agentSmsInbound && agentSmsInbound?.length > 0) {
      const AgentDetails = agentSmsInbound[0] ?? [];
      const AllAgentSmsInbound = agentSmsInbound[1] ?? [];
      const SmsChartInbound = agentSmsInbound[2] ?? [];
      res.status(200).send({
        AgentDetails,
        AllAgentSmsInbound,
        SmsChartInbound,
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
    logger.error('agentSmsInboundMetricsFilter - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentSmsPerformanceMetricsFilterHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: req?.headers?.domainId,
      offset: req?.body?.offset,
      limit: req?.body?.limit,
      agentId: req?.body?.agentId.length > 0 ? req?.body?.agentId : null,
    };
    const agentSmsPerformance: any = await agentSmsPerformanceMetricFilterDao(data);
    if (agentSmsPerformance && agentSmsPerformance?.length > 0) {
      const AgentDetails = agentSmsPerformance[0] ?? [];
      const AllAgentSmsPerformance = agentSmsPerformance[1] ?? [];
      const SmsChartPerformance = agentSmsPerformance[2] ?? [];
      res.status(200).send({
        AgentDetails,
        AllAgentSmsPerformance,
        SmsChartPerformance,
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
    logger.error('agentSmsPerformanceMetricsFilter - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentSmsServiceLevelMetricsFilterHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: req?.headers?.domainId,
      offset: req?.body?.offset,
      limit: req?.body?.limit,
      agentId: req?.body?.agentId.length > 0 ? req?.body?.agentId : null,
    };
    const agentSmsServiceLevel: any = await agentSmsServiceLevelMetricFilterDao(data);
    if (agentSmsServiceLevel && agentSmsServiceLevel?.length > 0) {
      const AgentDetails = agentSmsServiceLevel[0] ?? [];
      const AgentSmsServiceLevel = agentSmsServiceLevel[1] ?? [];
      const SmsChartServiceLevel = agentSmsServiceLevel[2] ?? [];
      res.status(200).send({
        AgentDetails,
        AgentSmsServiceLevel,
        SmsChartServiceLevel,
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
    logger.error('agentSmsServiceLevelMetricFilter - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentSmsInboundTrendChartHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp ?? null,
      endTimeStamp: req?.body?.endTimeStamp ?? null,
      domainId: req?.headers?.domainId ?? null,
      columnName: req?.body?.columnName ?? null,
      dateType: req?.body?.datetype ?? null,
      agentId: req?.body?.agentId ?? null
    };
    const SmsTrendChat: any = await smsInboundTrendChartDao(data);
    if (SmsTrendChat && SmsTrendChat?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        SmsInboundTrendChart: SmsTrendChat[0],
        learnMoreAboutTrendChatRes: SmsTrendChat[1]
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('SmsInboundTrendChart - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentSmsServicelevelTrendChartHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp ?? null,
      endTimeStamp: req?.body?.endTimeStamp ?? null,
      domainId: req?.headers?.domainId ?? null,
      columnName: req?.body?.columnName ?? null,
      dateType: req?.body?.datetype ?? null,
      agentId: req?.body?.agentId ?? null
    };
    const SmsTrendChat: any = await smsServicelevelTrendChartDao(data);
    if (SmsTrendChat && SmsTrendChat?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        ReceivedCurrentChatCount: SmsTrendChat[0],
        ReceivedPastChatCount: SmsTrendChat[1],
        AllagentSMSServicelevel: SmsTrendChat[2],
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('SmsServicelevelTrendChart - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentSmsPerformancesTrendChartHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp ?? null,
      endTimeStamp: req?.body?.endTimeStamp ?? null,
      domainId: req?.headers?.domainId ?? null,
      columnName: req?.body?.columnName ?? null,
      dateType: req?.body?.datetype ?? null,
      agentId: req?.body?.agentId ?? null
    };
    const SmsTrendChat: any = await smsPerformancesTrendChartDao(data);
    if (SmsTrendChat && SmsTrendChat?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        SmsPerformancesTrendChart: SmsTrendChat[0],
        learnMoreAboutTrendChatRes: SmsTrendChat[1]
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('SmsPerformancesTrendChart - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function createSmsAnalyticsHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      ext: auth.ext,
      editColumnNames: req?.body?.editColumnNames ?? [],
      id: req?.body?.id ?? null,
    };
    const SmsAnalyticsResult: any = await createSmsAnalyticsDao(data);
    res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: SmsAnalyticsResult[0] });
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('createSmsAnalytics - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getCoulumnSmsAnalyticsHandler(req: any, res: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      ext: auth.ext,
      id: req?.params?.id ?? 0,
    };
    const SmsAnalyticsResult: any = await getCoulumnSmsAnalyticsDao(data);
    res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: SmsAnalyticsResult[0] });

  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getCoulumnSmsAnalytics - Unexpected Error:', err);
  }
}
