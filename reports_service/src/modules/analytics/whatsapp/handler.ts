import {
  agentWhatsappInboundMetricFilterDao,
  agentWhatsappPerformancesMetricFilterDao,
  agentWhatsappServicelevelMetricFilterDao,
  createWhatsappAnalyticsDao,
  getCoulumnWhatsappAnalyticsDao,
  whatsappInboundTrendChartDao,
  whatsappPerformancesTrendChartDao,
  whatsappServicelevelTrendChartDao
} from '../../../dao/whatsappAnalyics.dao';
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
export async function agentWhatsappInboundMetricsFilterHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: req?.headers?.domainId,
      offset: req?.body?.offset,
      limit: req?.body?.limit,
      agentId: req?.body?.agentId.length > 0 ? req?.body?.agentId : null,
    };
    const agentWhatsappInbound: any = await agentWhatsappInboundMetricFilterDao(data);
    if (agentWhatsappInbound && agentWhatsappInbound?.length > 0) {
      const AgentDetails = agentWhatsappInbound[0] ?? [];
      const AgentWhatsappInbound = agentWhatsappInbound[1] ?? [];
      const WhatsappChartInbound = agentWhatsappInbound[2] ?? [];
      res.status(200).send({
        AgentDetails,
        AgentWhatsappInbound,
        WhatsappChartInbound,
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
    logger.error('agentWhatsappInboundMetricsFilter - Unexpected Error:', err);

  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentWhatsappPerformancesMetricsFilterHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: req?.headers?.domainId,
      offset: req?.body?.offset,
      limit: req?.body?.limit,
      agentId: req?.body?.agentId.length > 0 ? req?.body?.agentId : null,
    };
    const agentWhatsappPerformances: any = await agentWhatsappPerformancesMetricFilterDao(data);
    if (agentWhatsappPerformances && agentWhatsappPerformances?.length > 0) {
      const AgentDetails = agentWhatsappPerformances[0] ?? [];
      const AgentWhatsappPerformances = agentWhatsappPerformances[1] ?? [];
      const WhatsappChartPerformances = agentWhatsappPerformances[2] ?? [];
      res.status(200).send({
        AgentDetails,
        AgentWhatsappPerformances,
        WhatsappChartPerformances,
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
    logger.error('agentWhatsappPerformancesMetricsFilter - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentWhatsappServicelevelMetricsFilterHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: req?.headers?.domainId,
      offset: req?.body?.offset,
      limit: req?.body?.limit,
      agentId: req?.body?.agentId.length > 0 ? req?.body?.agentId : null,
    };
    const agentWhatsappServicelevel: any = await agentWhatsappServicelevelMetricFilterDao(data);
    if (agentWhatsappServicelevel && agentWhatsappServicelevel?.length > 0) {
      const AgentDetails = agentWhatsappServicelevel[0] ?? [];
      const AgentWhatsappServicelevel = agentWhatsappServicelevel[1] ?? [];
      const WhatsappChartServicelevel = agentWhatsappServicelevel[2] ?? [];
      res.status(200).send({
        AgentDetails,
        AgentWhatsappServicelevel,
        WhatsappChartServicelevel,
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
    logger.error('agentWhatsappServicelevelMetricFilter - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function createWhatsappAnalyticsHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      ext: auth.ext,
      editColumnNames: req?.body?.editColumnNames ?? [],
      id: req?.body?.id ?? null,
    };
    const WhatsappAnalyticsResult: any = await createWhatsappAnalyticsDao(data);
    res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: WhatsappAnalyticsResult[0] });
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('WhatsappAnalyticsResult - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getCoulumnWhatsappAnalyticsHandler(req: any, res: any) {
  try {
    const auth = req.headers;
    const data: any = {
      id: req?.params?.id ?? 0,
      domainId: auth.domainId,
      ext: auth.ext,
    };
    const WhatsappAnalyticsResult: any = await getCoulumnWhatsappAnalyticsDao(data);
    res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: WhatsappAnalyticsResult[0] });
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getCoulumnWhatsappAnalytics - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentWhatsappInboundTrendChartHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      domainId: req?.headers?.domainId,
      datetype: req?.body?.dateType,
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      columnName: req?.body?.columnName ?? null,
      agentId: req?.body?.agentId,
    };
    const WhatsappTrendChat: any = await whatsappInboundTrendChartDao(data);
    if (WhatsappTrendChat && WhatsappTrendChat?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        ReceivedCurrentChatCount: WhatsappTrendChat[0],
        ReceivedPastChatCount: WhatsappTrendChat[1],
        // learnMoreAboutTrendChatRes: WhatsappTrendChat[2]
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('WhatsappInboundTrendChart - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentWhatsappPerformancesTrendChartHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: req?.headers?.domainId,
      datetype: req?.body?.dateType,
      columnName: req?.body?.columnName,
      agentId: req?.body?.agentId
    };
    const WhatsappTrendChat: any = await whatsappPerformancesTrendChartDao(data);
    if (WhatsappTrendChat && WhatsappTrendChat?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        ReceivedCurrentChatCount: WhatsappTrendChat[0],
        ReceivedPastChatCount: WhatsappTrendChat[1],
        // learnMoreAboutTrendChatRes: WhatsappTrendChat[2]
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('WhatsappPerformancesTrendChart - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentWhatsappServicelevelTrendChartHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      domainId: req?.headers?.domainId,
      dateType: req?.body?.dateType,
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      columnName: req?.body?.columnName ?? null,
      agentId: req?.body?.agentId
    };
    const WhatsappTrendChat: any = await whatsappServicelevelTrendChartDao(data);
    if (WhatsappTrendChat && WhatsappTrendChat?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        ReceivedCurrentChatCount: WhatsappTrendChat[0],
        ReceivedPastChatCount: WhatsappTrendChat[1],
        // learnMoreAboutTrendChatRes: WhatsappTrendChat[2]
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('WhatsappServicelevelTrendChart - Unexpected Error:', err);
  }
}
