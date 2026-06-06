
import {
  agentInstagramInboundMetricFilterDao,
  agentInstagramPerformanceMetricFilterDao,
  agentInstagramServiceLevelMetricFilterDao,
  createEditInstagramAnalyticsDao,
  getCoulumnInstagramAnalyticsDao,
  instagramInboundTrendChartDao,
  instagramPerformanceTrendChartDao,
  instagramServicelevelTrendChartDao
} from '../../../dao/instagramAnalytics.dao';
import {
  RESPONSE
} from '../../../helpers/constants';
import { logger } from '../../../plugins/log';

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentInstagramInboundMetricsFilterHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: req?.headers?.domainId,
      offset: req?.body?.offset,
      limit: req?.body?.limit,
      agentId: req?.body?.agentId.length > 0 ? req?.body?.agentId?.join(',') : null,
    };
    const agentInstagramInbound: any = await agentInstagramInboundMetricFilterDao(data);
    if (agentInstagramInbound && agentInstagramInbound?.length > 0) {
      const AgentDetails = agentInstagramInbound[0] ?? [];
      const AgentInstagramInbound = agentInstagramInbound[1] ?? [];
      const InstagramChartInbound = agentInstagramInbound[2] ?? [];
      res.status(200).send({
        AgentDetails,
        AgentInstagramInbound,
        InstagramChartInbound,
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
    logger.error('agentInstagramInboundMetricFilter - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentInstagramPerformanceMetricsFilterHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: req?.headers?.domainId,
      offset: req?.body?.offset,
      limit: req?.body?.limit,
      agentId: req?.body?.agentId.length > 0 ? req?.body?.agentId?.join(',') : null,
    };
    const agentInstagramPerformance: any = await agentInstagramPerformanceMetricFilterDao(data);
    if (agentInstagramPerformance && agentInstagramPerformance?.length > 0) {
      const AgentDetails = agentInstagramPerformance[0] ?? [];
      const AgentInstagramPerformance = agentInstagramPerformance[1] ?? [];
      const InstagramChartPerformance = agentInstagramPerformance[2] ?? [];
      res.status(200).send({
        AgentDetails,
        AgentInstagramPerformance,
        InstagramChartPerformance,
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
    logger.error('agentInstagramPerformance - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentInstagramServiceLevelMetricsFilterHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: req?.headers?.domainId,
      offset: req?.body?.offset,
      limit: req?.body?.limit,
      agentId: req?.body?.agentId.length > 0 ? req?.body?.agentId?.join(',') : null,
    };
    const agentInstagramServiceLevel: any = await agentInstagramServiceLevelMetricFilterDao(data);
    if (agentInstagramServiceLevel && agentInstagramServiceLevel?.length > 0) {
      const AgentDetails = agentInstagramServiceLevel[0] ?? [];
      const AgentInstagramServicelevel = agentInstagramServiceLevel[1] ?? [];
      const InstagramChartServicelevel = agentInstagramServiceLevel[2] ?? [];
      res.status(200).send({
        AgentDetails,
        AgentInstagramServicelevel,
        InstagramChartServicelevel,
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
    logger.error('agentInstagramServiceLevelMetricFilter - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function createEditInstagramAnalyticsHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      ext: auth.ext,
      editColumnNames: req?.body?.editColumnNames ?? [],
      id: req?.body?.id ?? null,
    };
    const FacebookAnalyticsResult: any = await createEditInstagramAnalyticsDao(data);
    res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: FacebookAnalyticsResult[0] });
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('createEditInstagramAnalyticsDao - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getCoulumnInstagramAnalyticsHandler(req: any, res: any) {
  try {
    const auth = req.headers;
    const data: any = {
      id: req?.params?.id ?? 0,
      domainId: auth.domainId,
      ext: auth.ext,
    };
    const InstagramAnalyticsResult: any = await getCoulumnInstagramAnalyticsDao(data);
    res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: InstagramAnalyticsResult[0] });
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentInstagramInboundTrendChartHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      datetype: req?.body?.datetype,
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      columnName: req?.body?.columnName ?? null,
      agentIds: req?.body?.agentIds ?? null,
    };
    const InstagramTrendChat: any = await instagramInboundTrendChartDao(data);
    if (InstagramTrendChat && InstagramTrendChat?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        ReceivedCurrentChatCount: InstagramTrendChat[0],
        ReceivedPastChatCount: InstagramTrendChat[1]
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentInstagramPerformanceTrendChartHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      datetype: req?.body?.datetype,
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      columnName: req?.body?.columnName ?? null,
      agentIds: req?.body?.agentIds ?? null,
    };
    const InstagramTrendChat: any = await instagramPerformanceTrendChartDao(data);
    if (InstagramTrendChat && InstagramTrendChat?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        ReceivedCurrentChatCount: InstagramTrendChat[0],
        ReceivedPastChatCount: InstagramTrendChat[1]
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentInstagramServicelevelTrendChartHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      datetype: req?.body?.datetype,
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      columnName: req?.body?.columnName ?? null,
      agentIds: req?.body?.agentIds ?? null,
    };
    const InstagramTrendChat: any = await instagramServicelevelTrendChartDao(data);
    if (InstagramTrendChat && InstagramTrendChat?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        ReceivedCurrentChatCount: InstagramTrendChat[0],
        ReceivedPastChatCount: InstagramTrendChat[1]
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
