import {
  agentFacebookInboundMetricFilterDao,
  agentFacebookPerformanceMetricFilterDao,
  agentFacebookServicelevelMetricFilterDao,
  createEditFaceBookAnalyticsDao,
  facebookInboundTrendChartDao,
  facebookPerformancesTrendChartDao,
  facebookServicelevelTrendChartDao,
  getCoulumnFacebookAnalyticsDao
} from '../../../dao/facebookAnalytics.dao';
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
export async function agentFacebookInboundMetricsFilterHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: req?.headers?.domainId,
      offset: req?.body?.offset,
      limit: req?.body?.limit,
      agentId: req?.body?.agentId.length > 0 ? req?.body?.agentId?.join(',') : null,
    };
    const agentFacebookInbound: any = await agentFacebookInboundMetricFilterDao(data);
    if (agentFacebookInbound && agentFacebookInbound?.length > 0) {
      const AgentDetails = agentFacebookInbound[0] ?? [];
      const AgentFacebookInbound = agentFacebookInbound[1] ?? [];
      const FacebookChartInbound = agentFacebookInbound[2] ?? [];
      res.status(200).send({
        AgentDetails,
        AgentFacebookInbound,
        FacebookChartInbound,
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
    logger.error('agentFacebookInboundMetricFilter - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentFacebookPerformancesMetricsFilterHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: req?.headers?.domainId,
      offset: req?.body?.offset,
      limit: req?.body?.limit,
      agentId: req?.body?.agentId.length > 0 ? req?.body?.agentId?.join(',') : null,
    };
    const agentFacebookPerformance: any = await agentFacebookPerformanceMetricFilterDao(data);
    if (agentFacebookPerformance && agentFacebookPerformance?.length > 0) {
      const AgentDetails = agentFacebookPerformance[0] ?? [];
      const AgentFacebookPerformance = agentFacebookPerformance[1] ?? [];
      const FacebookChartPerformance = agentFacebookPerformance[2] ?? [];
      res.status(200).send({
        AgentDetails,
        AgentFacebookPerformance,
        FacebookChartPerformance,
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
    logger.error('agentFacebookPerformanceMetricFilter - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentFacebookServicelevelMetricsFilterHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: req?.headers?.domainId,
      offset: req?.body?.offset,
      limit: req?.body?.limit,
      agentId: req?.body?.agentId.length > 0 ? req?.body?.agentId?.join(',') : null,
    };
    const agentFacebookServicelevel: any = await agentFacebookServicelevelMetricFilterDao(data);
    if (agentFacebookServicelevel && agentFacebookServicelevel?.length > 0) {
      const AgentDetails = agentFacebookServicelevel[0] ?? [];
      const AgentFacebookServicelevel = agentFacebookServicelevel[1] ?? [];
      const FacebookChartServicelevel = agentFacebookServicelevel[2] ?? [];
      res.status(200).send({
        AgentDetails,
        AgentFacebookServicelevel,
        FacebookChartServicelevel,
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
    logger.error('agentFacebookServicelevelMetricFilter - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function createEditFacebookAnalyticsHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      ext: auth.ext,
      editColumnNames: req?.body?.editColumnNames ?? [],
      id: req?.body?.id ?? null,
    };
    const FacebookAnalyticsResult: any = await createEditFaceBookAnalyticsDao(data);
    res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: FacebookAnalyticsResult[0] });
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('createEditFacebookAnalytics - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getCoulumnFacebookAnalyticsHandler(req: any, res: any) {
  try {
    const auth = req.headers;
    const data: any = {
      id: req?.params?.id ?? 0,
      domainId: auth.domainId,
      ext: auth.ext,
    };
    const FacebookAnalyticsResult: any = await getCoulumnFacebookAnalyticsDao(data);
    res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: FacebookAnalyticsResult[0] });
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getCoulumnFacebookAnalytics - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentFacebookInboundTrendChartHandler(req: any, res: any, done: any) {
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
    const FacebookTrendChat: any = await facebookInboundTrendChartDao(data);
    if (FacebookTrendChat && FacebookTrendChat?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        // FacebookInboundTrendChart: FacebookTrendChat[0],
        ReceivedCurrentChatCount: FacebookTrendChat[0],
        ReceivedPastChatCount: FacebookTrendChat[1]
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('FacebookInboundTrendChart - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentFacebookPerformancesTrendChartHandler(req: any, res: any, done: any) {
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
    const FacebookTrendChat: any = await facebookPerformancesTrendChartDao(data);
    if (FacebookTrendChat && FacebookTrendChat?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        // PerformancesFacebookTrendChart: FacebookTrendChat[0],
        ReceivedCurrentChatCount: FacebookTrendChat[0],
        ReceivedPastChatCount: FacebookTrendChat[1]
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('FacebookPerformancesTrendChart - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentFacebookServicelevelTrendChartHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      datetype: req?.body?.datetype,
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      columnName: req?.body?.columnName ?? null,
      agentIds: req?.body?.agentIds ?? null,
      // agentIds: req?.body?.agentIds.length > 0 ? req?.body?.agentIds?.join(',') : null,
    };
    const FacebookTrendChat: any = await facebookServicelevelTrendChartDao(data);
    if (FacebookTrendChat && FacebookTrendChat?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        // ServicelevelFacebookTrendChart: FacebookTrendChat[0],
        ReceivedCurrentChatCount: FacebookTrendChat[0],
        ReceivedPastChatCount: FacebookTrendChat[1]
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('FacebookServicelevelTrendChart - Unexpected Error:', err);
  }
}
