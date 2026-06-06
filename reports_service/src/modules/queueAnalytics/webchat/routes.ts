import { WEBCHATQUEUEANALYTICS } from '../../../helpers/constants';
import { createEditQueueWebAnalyticsHandler, getCoulumnQueueWebchatAnalyticsHandler, getQueuechatPerformanceAnalysisMetricsHandler, getQueuechatServiceLevelAnalysisMetricsHandler, getQueueInboundchatAnalysisMetricsHandler, getWebChatInboundTrendChartHandler, getWebChatPerformancesTrendChartHandler, getWebChatServicelevelTrendChartHandler } from './handler';
import { queueWebchatAnalyticsInboundMetricsFilterScheme } from './schema';

export default function webchatQueueAnalyticsHandler(server: any, options: any, next: any) {

  // TODO:get queue inboundchat analysis metrics
  server.post(
    WEBCHATQUEUEANALYTICS.QUEUE_INBOUNDCHAT_ANALYSIS_METRICS_FILTER,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get queue inboundchat analysis metrics',
        description: 'get queue inboundchat analysis metrics api',
        tags: ['Queue_Analytics'],
        body: queueWebchatAnalyticsInboundMetricsFilterScheme.body,
        response: queueWebchatAnalyticsInboundMetricsFilterScheme.response,
      },
    },
    getQueueInboundchatAnalysisMetricsHandler
  );

  // TODO:get queue chatperformance analysis metrics
  server.post(
    WEBCHATQUEUEANALYTICS.QUEUE_CHATPERFORMANCES_ANALYSIS_METRICS_FILTER,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get queue chatperformance analysis metrics',
        description: 'get queue chatperformance analysis metrics api',
        tags: ['Queue_Analytics'],
        // body: queueWebchatAnalyticsInboundMetricsFilterScheme.body,
        // response: queueWebchatAnalyticsInboundMetricsFilterScheme.response,
      },
    },
    getQueuechatPerformanceAnalysisMetricsHandler
  );

  // TODO:get queue servicelevel analysis metrics
  server.post(
    WEBCHATQUEUEANALYTICS.QUEUE_SERVICELEVEL_ANALYSIS_METRICS_FILTER,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get queue servicelevel analysis metrics',
        description: 'get queue servicelevel analysis metrics api',
        tags: ['Queue_Analytics'],
        // body: queueWebchatAnalyticsInboundMetricsFilterScheme.body,
        // response: queueWebchatAnalyticsInboundMetricsFilterScheme.response,
      },
    },
    getQueuechatServiceLevelAnalysisMetricsHandler
  );

  // TODO:Create Edit Column queue voice Analysis
  server.post(
    WEBCHATQUEUEANALYTICS.CREATE_EDIT_COLUMN_QUEUE_WEBCHAT_ANALYTICS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Create Edit Column queue voice Analysis',
        description: 'Create Edit Column queue voice Analysispi',
        tags: ['Queue_Analytics'],
      },
    },
    createEditQueueWebAnalyticsHandler
  );

  // TODO:Get Column queue webchat Analysis
  server.get(
    `${WEBCHATQUEUEANALYTICS.GET_COLUMN_QUEUE_WEBCHAT__ANALYTICS}/:id`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get Column queue webchat Analysis',
        description: 'get Column queue webchat Analysisapi',
        tags: ['Queue_Analytics'],
      },
    },
    getCoulumnQueueWebchatAnalyticsHandler
  );

  // TODO:queue webchat inbound trendchart
  server.post(
    WEBCHATQUEUEANALYTICS.QUEUE_WEBCHAT_INBOUND_TRENDCHART,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get queue webchat inbound trendchart',
        description: 'get queue webchat inbound trendchart api',
        tags: ['Queue_Analytics'],
      },
    },
    getWebChatInboundTrendChartHandler
  );

  // TODO:queue webchat performance trendchart
  server.post(
    WEBCHATQUEUEANALYTICS.QUEUE_WEBCHAT_PERFORMANCES_TRENDCHART,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get queue webchat performance trendchart',
        description: 'get queue webchat performance trendchart api',
        tags: ['Queue_Analytics'],
      },
    },
    getWebChatPerformancesTrendChartHandler
  );

  // TODO:queue webchat servicelevel trendchart
  server.post(
    WEBCHATQUEUEANALYTICS.QUEUE_WEBCHAT_SERVICELEVEL_TRENDCHART,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get queue webchat servicelevel trendchart',
        description: 'get queue webchat servicelevel trendchart api',
        tags: ['Queue_Analytics'],
      },
    },
    getWebChatServicelevelTrendChartHandler
  );

  next();
}
