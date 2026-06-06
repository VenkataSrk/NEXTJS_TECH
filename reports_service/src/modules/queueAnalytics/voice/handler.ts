import {
  createEditQueueVoiceAnalyticsDao,
  getCoulumnQueueVoiceAnalyticsDao,
  getWebVoiceCallBackTrendChartDao,
  getWebVoiceInboundTrendChartDao,
  getWebVoiceOutboundBestTimeTrendChartDao,
  getWebVoiceOutboundTrendChartDao,
  getWebVoiceServiceLevelTrendChartDao,
  queueCallBackAnalysisMetricFilterDao,
  queueInboundcallAnalysisMetricFilterDao,
  queueOutboundcallAnalysisMetricFilterDao,
  queueOutboundCallBestTimeMetricsFilterDao,
  queueServiceLevelAnalysisMetricFilterDao,
  queueVoiceMailAnalysisMetricFilterDao
} from '../../../dao/queueVoiceAnalytics.dao';
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
export async function getQueueInboundcallAnalysisMetricsHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      domainId: req?.headers?.domainId,
      queueId: Array.isArray(req?.body?.queueId) && req?.body?.queueId.length > 0 ? req?.body?.queueId.join(',') : req?.body?.queueId ? String(req?.body?.queueId) : null,
      offset: req?.body?.offset,
      limit: req?.body?.limit,
    };
    const queueInboundcallAnalysis: any = await queueInboundcallAnalysisMetricFilterDao(data);
    if (queueInboundcallAnalysis && queueInboundcallAnalysis?.length > 0) {
      const QueueDetails = queueInboundcallAnalysis[0] ?? [];
      const QueueVoiceInboundCall = queueInboundcallAnalysis[1] ?? [];
      const QueueVoiceInboundTable = queueInboundcallAnalysis[2] ?? [];
      res.status(200).send({
        QueueDetails,
        QueueVoiceInboundCall,
        QueueVoiceInboundTable,
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
    logger.error('createEditInstagramAnalytics - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getQueueOutboundcallAnalysisMetricsHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      domainId: req?.headers?.domainId,
      queueId: Array.isArray(req?.body?.queueId) && req?.body?.queueId.length > 0 ? req?.body?.queueId.join(',') : req?.body?.queueId ? String(req?.body?.queueId) : null,
      offset: req?.body?.offset,
      limit: req?.body?.limit,
    };
    const queueOutboundcallAnalysis: any = await queueOutboundcallAnalysisMetricFilterDao(data);
    if (queueOutboundcallAnalysis && queueOutboundcallAnalysis?.length > 0) {
      const QueueDetails = queueOutboundcallAnalysis[0] ?? [];
      const QueueVoiceOutboundCall = queueOutboundcallAnalysis[1] ?? [];
      const QueueVoiceOutboundTable = queueOutboundcallAnalysis[2] ?? [];
      res.status(200).send({
        QueueDetails,
        QueueVoiceOutboundCall,
        QueueVoiceOutboundTable,
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
    logger.error('queueOutboundcallAnalysis - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getQueueCallBackAnalysisMetricsHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      domainId: req?.headers?.domainId,
      queueId: Array.isArray(req?.body?.queueId) && req?.body?.queueId.length > 0 ? req?.body?.queueId.join(',') : req?.body?.queueId ? String(req?.body?.queueId) : null,
      offset: req?.body?.offset,
      limit: req?.body?.limit,
    };
    const queueCallBackAnalysis: any = await queueCallBackAnalysisMetricFilterDao(data);
    if (queueCallBackAnalysis && queueCallBackAnalysis?.length > 0) {
      const QueueDetails = queueCallBackAnalysis[0] ?? [];
      const QueueVoiceCallBack = queueCallBackAnalysis[1] ?? [];
      const QueueVoiceCallBackTable = queueCallBackAnalysis[2] ?? [];
      res.status(200).send({
        QueueDetails,
        QueueVoiceCallBack,
        QueueVoiceCallBackTable,
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
    logger.error('getQueueCallBackAnalysisMetrics - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getQueueVoiceMailAnalysisMetricsHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      domainId: req?.headers?.domainId,
      queueId: Array.isArray(req?.body?.queueId) && req?.body?.queueId.length > 0 ? req?.body?.queueId.join(',') : req?.body?.queueId ? String(req?.body?.queueId) : null,
      offset: req?.body?.offset,
      limit: req?.body?.limit,
    };
    const queueVoiceMailAnalysis: any = await queueVoiceMailAnalysisMetricFilterDao(data);
    if (queueVoiceMailAnalysis && queueVoiceMailAnalysis?.length > 0) {
      const QueueDetails = queueVoiceMailAnalysis[0] ?? [];
      const QueueVoiceMail = queueVoiceMailAnalysis[1] ?? [];
      const QueueVoiceMailTable = queueVoiceMailAnalysis[2] ?? [];
      res.status(200).send({
        QueueDetails,
        QueueVoiceMail,
        QueueVoiceMailTable,
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
    logger.error('queueVoiceMailAnalysis - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getQueueServicelevelAnalysisMetricsHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      domainId: req?.headers?.domainId,
      queueId: Array.isArray(req?.body?.queueId) && req?.body?.queueId.length > 0 ? req?.body?.queueId.join(',') : req?.body?.queueId ? String(req?.body?.queueId) : null,
      offset: req?.body?.offset,
      limit: req?.body?.limit,
    };
    const queueServiceLevelAnalysis: any = await queueServiceLevelAnalysisMetricFilterDao(data);
    if (queueServiceLevelAnalysis && queueServiceLevelAnalysis?.length > 0) {
      const QueueDetails = queueServiceLevelAnalysis[0] ?? [];
      const QueueVoiceServiceLevel = queueServiceLevelAnalysis[1] ?? [];
      const QueueVoiceServiceLevelTable = queueServiceLevelAnalysis[2] ?? [];
      res.status(200).send({
        QueueDetails,
        QueueVoiceServiceLevel,
        QueueVoiceServiceLevelTable,
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
    logger.error('queueServiceLevelAnalysis - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getQueueOutboundCallBestTimeMetricsHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      domainId: req?.headers?.domainId,
      queueId: Array.isArray(req?.body?.queueId) && req?.body?.queueId.length > 0 ? req?.body?.queueId.join(',') : req?.body?.queueId ? String(req?.body?.queueId) : null,
      offset: req?.body?.offset,
      limit: req?.body?.limit,
    };
    const queueOutboundCallBestTime: any = await queueOutboundCallBestTimeMetricsFilterDao(data);
    if (queueOutboundCallBestTime && queueOutboundCallBestTime?.length > 0) {
      const QueueDetails = queueOutboundCallBestTime[0] ?? [];
      const QueueOutboundCallBestTime = queueOutboundCallBestTime[1] ?? [];
      const QueueOutboundCallBestTimeTable = queueOutboundCallBestTime[2] ?? [];
      res.status(200).send({
        QueueDetails,
        QueueOutboundCallBestTime,
        QueueOutboundCallBestTimeTable,
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
    logger.error('queueOutboundCallBestTime - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function createEditQueueVoiceAnalyticsHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      id: req?.body?.id ?? null,
      domainId: auth.domainId,
      ext: auth.ext,
      editColumnNames: req?.body?.editColumnNames ?? [],
    };
    const QueueVoiceAnalyticsResult: any = await createEditQueueVoiceAnalyticsDao(data);
    res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: QueueVoiceAnalyticsResult[0] });
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('createEditQueueVoiceAnalytics - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getCoulumnQueueVoiceAnalyticsHandler(req: any, res: any) {
  try {
    const auth = req.headers;
    const data: any = {
      id: req?.params?.id ?? 0,
      domainId: auth.domainId,
      ext: auth.ext,
    };
    const QueueVoiceAnalyticsResult: any = await getCoulumnQueueVoiceAnalyticsDao(data);
    res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: QueueVoiceAnalyticsResult[0] });
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getCoulumnQueueVoiceAnalytics - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getWebVoiceInboundTrendChartHandler(req: any, res: any, done: any) {
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
    const WebVoiceInboundTrendChart: any = await getWebVoiceInboundTrendChartDao(data);
    if (WebVoiceInboundTrendChart && WebVoiceInboundTrendChart?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        ReceivedCurrentChatCount: WebVoiceInboundTrendChart[0],
        ReceivedPastChatCount: WebVoiceInboundTrendChart[1],
        WebVoiceInboundTrendChart: WebVoiceInboundTrendChart[2],
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('WebVoiceInboundTrendChart - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getWebVoiceOutboundTrendChartHandler(req: any, res: any, done: any) {
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
    const WebVoiceOutboundTrendChart: any = await getWebVoiceOutboundTrendChartDao(data);
    if (WebVoiceOutboundTrendChart && WebVoiceOutboundTrendChart?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        ReceivedCurrentChatCount: WebVoiceOutboundTrendChart[0],
        ReceivedPastChatCount: WebVoiceOutboundTrendChart[1],
        WebVoiceOutboundTrendChart: WebVoiceOutboundTrendChart[2],
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getWebVoiceOutboundTrendChart - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getWebVoiceCallBackTrendChartHandler(req: any, res: any, done: any) {
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
    const WebVoiceCallBackTrendChart: any = await getWebVoiceCallBackTrendChartDao(data);
    if (WebVoiceCallBackTrendChart && WebVoiceCallBackTrendChart?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        ReceivedCurrentChatCount: WebVoiceCallBackTrendChart[0],
        ReceivedPastChatCount: WebVoiceCallBackTrendChart[1],
        WebVoiceCallBackTrendChart: WebVoiceCallBackTrendChart[2],
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getWebVoiceCallBackTrendChart - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getWebVoiceOutboundBestTimeTrendChartHandler(req: any, res: any, done: any) {
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
    const WebVoiceOutboundBestTimeTrendChart: any = await getWebVoiceOutboundBestTimeTrendChartDao(data);
    if (WebVoiceOutboundBestTimeTrendChart && WebVoiceOutboundBestTimeTrendChart?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        ReceivedCurrentChatCount: WebVoiceOutboundBestTimeTrendChart[0],
        ReceivedPastChatCount: WebVoiceOutboundBestTimeTrendChart[1],
        WebVoiceOutboundBestTimeTrendChart: WebVoiceOutboundBestTimeTrendChart[2],
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getWebVoiceOutboundBestTimeTrendChart - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getWebVoiceServiceLevelTrendChartHandler(req: any, res: any, done: any) {
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
    const WebVoiceServiceLevelTrendChart: any = await getWebVoiceServiceLevelTrendChartDao(data);
    if (WebVoiceServiceLevelTrendChart && WebVoiceServiceLevelTrendChart?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        ReceivedCurrentChatCount: WebVoiceServiceLevelTrendChart[0],
        ReceivedPastChatCount: WebVoiceServiceLevelTrendChart[1],
        WebVoiceServiceLevelTrendChart: WebVoiceServiceLevelTrendChart[2],
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getWebVoiceServiceLevelTrendChart - Unexpected Error:', err);
  }
}
