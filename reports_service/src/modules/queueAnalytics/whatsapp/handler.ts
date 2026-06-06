import { createEditQueueWhatsappAnalyticsDao, getCoulumnQueueWhatsappAnalyticsDao, getWhatsappInboundTrendChartDao, getWhatsappPerformanceTrendChartDao, getWhatsappServicelevelTrendChartDao, queueWhatsappInboundAnalysisMetricDao, queueWhatsappPerformanceAnalysisMetricDao, queueWhatsappServcicelevelAnalysisMetricDao } from '../../../dao/queueWhatsappAnalytics.dao';
import { RESPONSE } from '../../../helpers/constants';
import { logger } from '../../../plugins/log';

/**
 * @param req
 * @param res
 * @param done
 */
export async function getQueueWhatsappInboundAnalysisHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      domainId: req?.headers?.domainId,
      queueId: req?.body?.queueId,
      offset: req?.body?.offset,
      limit: req?.body?.limit,
    };
    const queueWhatsappInboundAnalysis: any = await queueWhatsappInboundAnalysisMetricDao(data);
    if (queueWhatsappInboundAnalysis && queueWhatsappInboundAnalysis?.length > 0) {
      const QueueDetails = queueWhatsappInboundAnalysis[0] ?? [];
      const QueueWhatsappInbound = queueWhatsappInboundAnalysis[1] ?? [];
      const QueueWhatsappInboundTable = queueWhatsappInboundAnalysis[2] ?? [];
      res.status(200).send({
        QueueDetails,
        QueueWhatsappInbound,
        QueueWhatsappInboundTable,
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
    logger.error('queueInboundchatAnalysis - Unexpected Error:', err);
  }
}

/**
 * @param req
 * @param res
 * @param done
 */
export async function getQueueWhatsappPerformanceAnalysisHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      domainId: req?.headers?.domainId,
      queueId: req?.body?.queueId,
      offset: req?.body?.offset,
      limit: req?.body?.limit,
    };
    const queueWhatsappPerformanceAnalysis: any = await queueWhatsappPerformanceAnalysisMetricDao(data);
    if (queueWhatsappPerformanceAnalysis && queueWhatsappPerformanceAnalysis?.length > 0) {
      const QueueDetails = queueWhatsappPerformanceAnalysis[0] ?? [];
      const QueueWhatsappPerformance = queueWhatsappPerformanceAnalysis[1] ?? [];
      const QueueWhatsappPerformanceTable = queueWhatsappPerformanceAnalysis[2] ?? [];
      res.status(200).send({
        QueueDetails,
        QueueWhatsappPerformance,
        QueueWhatsappPerformanceTable,
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
    logger.error('queueWhatsappPerformanceAnalysis - Unexpected Error:', err);
  }
}

/**
 * @param req
 * @param res
 * @param done
 */
export async function getQueueWhatsappServicelevelAnalysisHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      domainId: req?.headers?.domainId,
      queueId: req?.body?.queueId,
      offset: req?.body?.offset,
      limit: req?.body?.limit,
    };
    const queueWhatsappServicelevelAnalysis: any = await queueWhatsappServcicelevelAnalysisMetricDao(data);
    if (queueWhatsappServicelevelAnalysis && queueWhatsappServicelevelAnalysis?.length > 0) {
      const QueueDetails = queueWhatsappServicelevelAnalysis[0] ?? [];
      const QueueWhatsappServicelevel = queueWhatsappServicelevelAnalysis[1] ?? [];
      const QueueWhatsappServicelevelTable = queueWhatsappServicelevelAnalysis[2] ?? [];
      res.status(200).send({
        QueueDetails,
        QueueWhatsappServicelevel,
        QueueWhatsappServicelevelTable,
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
    logger.error('queueWhatsappServicelevelAnalysis - Unexpected Error:', err);
  }
}

/**
 * @param req
 * @param res
 * @param done
 */
export async function createEditQueueWhatsappAnalyticsHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      id: req?.body?.id ?? null,
      domainId: auth.domainId,
      ext: auth.ext,
      editColumnNames: req?.body?.editColumnNames ?? [],
    };
    const QueueVoiceAnalyticsResult: any = await createEditQueueWhatsappAnalyticsDao(data);
    res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: QueueVoiceAnalyticsResult[0] });
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('createEditQueueWhatsappAnalytics - Unexpected Error:', err);
  }
}
/**
 * @param req
 * @param res
 * @param done
 */
export async function getCoulumnQueueWhatsappAnalyticsHandler(req: any, res: any) {
  try {
    const auth = req.headers;
    const data: any = {
      id: req?.params?.id ?? 0,
      domainId: auth.domainId,
      ext: auth.ext,
    };
    const QueueWebchatAnalyticsResult: any = await getCoulumnQueueWhatsappAnalyticsDao(data);
    res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: QueueWebchatAnalyticsResult[0] });
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getCoulumnQueueWhatsappAnalytics - Unexpected Error:', err);
  }
}

/**
 * @param req
 * @param res
 * @param done
 */
export async function getWhatsappInboundTrendChartHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      datetype: req?.body?.datetype,
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      columnName: req?.body?.columnName ?? null,
      queueId: req?.body?.queueId ?? null,
    };
    const WebWhatsappInboundTrendChart: any = await getWhatsappInboundTrendChartDao(data);
    if (WebWhatsappInboundTrendChart && WebWhatsappInboundTrendChart?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        ReceivedCurrentChatCount: WebWhatsappInboundTrendChart[0],
        ReceivedPastChatCount: WebWhatsappInboundTrendChart[1],
        WebWhatsappInboundTrendChart: WebWhatsappInboundTrendChart[2],
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
 * @param req
 * @param res
 * @param done
 */
export async function getWhatsappPerformanceTrendChartHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      datetype: req?.body?.datetype,
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      columnName: req?.body?.columnName ?? null,
      queueId: req?.body?.queueId ?? null,
    };
    const WebWhatsappPerformanceTrendChart: any = await getWhatsappPerformanceTrendChartDao(data);
    if (WebWhatsappPerformanceTrendChart && WebWhatsappPerformanceTrendChart?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        ReceivedCurrentChatCount: WebWhatsappPerformanceTrendChart[0],
        ReceivedPastChatCount: WebWhatsappPerformanceTrendChart[1],
        WebWhatsappPerformanceTrendChart: WebWhatsappPerformanceTrendChart[2],
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('WebWhatsappPerformanceTrendChart - Unexpected Error:', err);
  }
}

/**
 * @param req
 * @param res
 * @param done
 */
export async function getWhatsappServicelevelTrendChartHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      datetype: req?.body?.datetype,
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      columnName: req?.body?.columnName ?? null,
      queueId: req?.body?.queueId ?? null,
    };
    const WebWhatsappServicelevelTrendChart: any = await getWhatsappServicelevelTrendChartDao(data);
    if (WebWhatsappServicelevelTrendChart && WebWhatsappServicelevelTrendChart?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        ReceivedCurrentChatCount: WebWhatsappServicelevelTrendChart[0],
        ReceivedPastChatCount: WebWhatsappServicelevelTrendChart[1],
        WebWhatsappServicelevelTrendChart: WebWhatsappServicelevelTrendChart[2],
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('WebWhatsappServicelevelTrendChart - Unexpected Error:', err);
  }
}
