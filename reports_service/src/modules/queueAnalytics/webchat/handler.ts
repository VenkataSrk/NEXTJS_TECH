
import { createEditQueueWebAnalyticsDao, getCoulumnQueueWebchatAnalyticsDao, getWebChatInboundTrendChartDao, getWebChatPerformancesTrendChartDao, getWebChatServiceLevelTrendChartDao, queuechatPerformanceAnalysisMetricFilterDao, queuechatServiceLevelAnalysisMetricFilterDao, queueInboundchatAnalysisMetricFilterDao } from '../../../dao/queueWebchatAnalytics.dao';
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
export async function getQueueInboundchatAnalysisMetricsHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      domainId: req?.headers?.domainId,
      queueId: Array.isArray(req?.body?.queueId) && req?.body?.queueId.length > 0 ? req?.body?.queueId.join(',') : req?.body?.queueId ? String(req?.body?.queueId) : null,
      offset: req?.body?.offset,
      limit: req?.body?.limit,
    };
    const queueInboundchatAnalysis: any = await queueInboundchatAnalysisMetricFilterDao(data);
    if (queueInboundchatAnalysis && queueInboundchatAnalysis?.length > 0) {
      const QueueDetails = queueInboundchatAnalysis[0] ?? [];
      const QueueWebchatInbound = queueInboundchatAnalysis[1] ?? [];
      const QueueWebchatInboundTable = queueInboundchatAnalysis[2] ?? [];
      res.status(200).send({
        QueueDetails,
        QueueWebchatInbound,
        QueueWebchatInboundTable,
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
 *
 * @param req
 * @param res
 * @param done
 */
export async function getQueuechatPerformanceAnalysisMetricsHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      domainId: req?.headers?.domainId,
      queueId: Array.isArray(req?.body?.queueId) && req?.body?.queueId.length > 0 ? req?.body?.queueId.join(',') : req?.body?.queueId ? String(req?.body?.queueId) : null,
      offset: req?.body?.offset,
      limit: req?.body?.limit,
    };
    const queuechatPerformanceAnalysis: any = await queuechatPerformanceAnalysisMetricFilterDao(data);
    if (queuechatPerformanceAnalysis && queuechatPerformanceAnalysis?.length > 0) {
      const QueueDetails = queuechatPerformanceAnalysis[0] ?? [];
      const QueueWebchatPerformance = queuechatPerformanceAnalysis[1] ?? [];
      const QueueWebchatPerformanceTable = queuechatPerformanceAnalysis[2] ?? [];
      res.status(200).send({
        QueueDetails,
        QueueWebchatPerformance,
        QueueWebchatPerformanceTable,
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
    logger.error('queuechatPerformanceAnalysis - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getQueuechatServiceLevelAnalysisMetricsHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      domainId: req?.headers?.domainId,
      queueId: Array.isArray(req?.body?.queueId) && req?.body?.queueId.length > 0 ? req?.body?.queueId.join(',') : req?.body?.queueId ? String(req?.body?.queueId) : null,
      offset: req?.body?.offset,
      limit: req?.body?.limit,
    };
    const queuechatServiceLevelAnalysis: any = await queuechatServiceLevelAnalysisMetricFilterDao(data);
    if (queuechatServiceLevelAnalysis && queuechatServiceLevelAnalysis?.length > 0) {
      const QueueDetails = queuechatServiceLevelAnalysis[0] ?? [];
      const QueueWebchatServiceLevel = queuechatServiceLevelAnalysis[1] ?? [];
      const QueueWebchatServiceLevelTable = queuechatServiceLevelAnalysis[2] ?? [];
      res.status(200).send({
        QueueDetails,
        QueueWebchatServiceLevel,
        QueueWebchatServiceLevelTable,
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
    logger.error('getQueuechatServiceLevelAnalysisMetrics - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function createEditQueueWebAnalyticsHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      id: req?.body?.id ?? null,
      domainId: auth.domainId,
      ext: auth.ext,
      editColumnNames: req?.body?.editColumnNames ?? [],
    };
    const QueueVoiceAnalyticsResult: any = await createEditQueueWebAnalyticsDao(data);
    res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: QueueVoiceAnalyticsResult[0] });
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('QueueVoiceAnalyticsResult - Unexpected Error:', err);
  }
}

/**
 * @param req
 * @param res
 * @param done
 */
export async function getCoulumnQueueWebchatAnalyticsHandler(req: any, res: any) {
  try {
    const auth = req.headers;
    const data: any = {
      id: req?.params?.id ?? 0,
      domainId: auth.domainId,
      ext: auth.ext,
    };
    const QueueWebchatAnalyticsResult: any = await getCoulumnQueueWebchatAnalyticsDao(data);
    res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: QueueWebchatAnalyticsResult[0] });
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getCoulumnQueueWebchatAnalytics - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getWebChatInboundTrendChartHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      datetype: req?.body?.datetype,
      domainId: auth.domainId,
      columnName: req?.body?.columnName ?? null,
      queueId: req?.body?.queueId ?? null
    };
    const WebChatInboundTrendChart: any = await getWebChatInboundTrendChartDao(data);
    if (WebChatInboundTrendChart && WebChatInboundTrendChart?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        ReceivedCurrentChatCount: WebChatInboundTrendChart[0],
        ReceivedPastChatCount: WebChatInboundTrendChart[1],
        WebChatInboundTrendChart: WebChatInboundTrendChart[2],
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('WebChatInboundTrendChart - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getWebChatPerformancesTrendChartHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      datetype: req?.body?.datetype,
      startTimeStamp: req?.body?.startTmpstmp,
      endTimeStamp: req?.body?.endTmpstmp,
      columnName: req?.body?.columnName ?? null,
      queueId: Array.isArray(req?.body?.queueId) ? req.body.queueId.join(',') : req?.body?.queueId
    };
    const WebChatPerformanceTrendChart: any = await getWebChatPerformancesTrendChartDao(data);
    if (WebChatPerformanceTrendChart && WebChatPerformanceTrendChart?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        ReceivedCurrentChatCount: WebChatPerformanceTrendChart[0],
        ReceivedPastChatCount: WebChatPerformanceTrendChart[1],
        WebChatPerformanceTrendChart: WebChatPerformanceTrendChart[2],
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('WebChatPerformanceTrendChart - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getWebChatServicelevelTrendChartHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      datetype: req?.body?.datetype,
      startTimeStamp: req?.body?.startTmpstmp,
      endTimeStamp: req?.body?.endTmpstmp,
      columnName: req?.body?.columnName ?? null,
      queueId: Array.isArray(req?.body?.queueId) ? req.body.queueId.join(',') : req?.body?.queueId
    };
    const WebChatServicelevelTrendChart: any = await getWebChatServiceLevelTrendChartDao(data);
    if (WebChatServicelevelTrendChart && WebChatServicelevelTrendChart?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        ReceivedCurrentChatCount: WebChatServicelevelTrendChart[0],
        ReceivedPastChatCount: WebChatServicelevelTrendChart[1],
        WebChatServiceLevelTrendChart: WebChatServicelevelTrendChart[2],
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('WebChatServicelevelTrendChart - Unexpected Error:', err);
  }
}
