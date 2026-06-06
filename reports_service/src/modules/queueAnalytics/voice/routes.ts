import { VOICEQUEUEANALYTICS } from '../../../helpers/constants';
import { createEditQueueVoiceAnalyticsHandler, getCoulumnQueueVoiceAnalyticsHandler, getQueueCallBackAnalysisMetricsHandler, getQueueInboundcallAnalysisMetricsHandler, getQueueOutboundcallAnalysisMetricsHandler, getQueueOutboundCallBestTimeMetricsHandler, getQueueServicelevelAnalysisMetricsHandler, getQueueVoiceMailAnalysisMetricsHandler, getWebVoiceCallBackTrendChartHandler, getWebVoiceInboundTrendChartHandler, getWebVoiceOutboundBestTimeTrendChartHandler, getWebVoiceOutboundTrendChartHandler, getWebVoiceServiceLevelTrendChartHandler } from './handler';
import { queueOutboundCallBestTimeMetricsFilterScheme, queueServiceLevelMetricsFilterScheme, queueVoiceAnalyticsCallBackMetricsFilterScheme, queueVoiceAnalyticsInboundMetricsFilterScheme, queueVoiceAnalyticsOutboundMetricsFilterScheme, queueVoiceMailMetricsFilterScheme } from './schema';

export default function voiceQueueAnalyticsHandler(server: any, options: any, next: any) {

  // TODO:get queue inboundcall analysis metrics
  server.post(
    VOICEQUEUEANALYTICS.QUEUE_INBOUNDCALL_ANALYSIS_METRICS_FILTER,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get queue inboundcall analysis metrics',
        description: 'get queue inboundcall analysis metrics api',
        tags: ['Queue_Analytics'],
        body: queueVoiceAnalyticsInboundMetricsFilterScheme.body,
        response: queueVoiceAnalyticsInboundMetricsFilterScheme.response,
      },
    },
    getQueueInboundcallAnalysisMetricsHandler
  );

  // TODO:get queue outboundcall analysis metrics
  server.post(
    VOICEQUEUEANALYTICS.QUEUE_OUTBOUNDCALL_ANALYSIS_METRICS_FILTER,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get queue outboundcall analysis metrics',
        description: 'get queue outboundcall analysis metrics api',
        tags: ['Queue_Analytics'],
        body: queueVoiceAnalyticsOutboundMetricsFilterScheme.body,
        response: queueVoiceAnalyticsOutboundMetricsFilterScheme.response,
      },
    },
    getQueueOutboundcallAnalysisMetricsHandler
  );

  // TODO:get queue callback voice analysis metrics
  server.post(
    VOICEQUEUEANALYTICS.QUEUE_CALLBACK_VOICE_ANALYSIS_METRICS_FILTER,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get queue callback voice analysis metrics',
        description: 'get queue callback voice analysis metrics api',
        tags: ['Queue_Analytics'],
        body: queueVoiceAnalyticsCallBackMetricsFilterScheme.body,
        response: queueVoiceAnalyticsCallBackMetricsFilterScheme.response,
      },
    },
    getQueueCallBackAnalysisMetricsHandler
  );

  // TODO:get queue voicemail analysis metrics
  server.post(
    VOICEQUEUEANALYTICS.QUEUE_VOICEMAIL_ANALYSIS_METRICS_FILTER,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get queue voicemail analysis metrics',
        description: 'get queue voicemail analysis metrics api',
        tags: ['Queue_Analytics'],
        body: queueVoiceMailMetricsFilterScheme.body,
        response: queueVoiceMailMetricsFilterScheme.response,
      },
    },
    getQueueVoiceMailAnalysisMetricsHandler
  );

  // TODO:get queue servicelevel analysis metrics
  server.post(
    VOICEQUEUEANALYTICS.QUEUE_SERVICELEVEL_ANALYSIS_METRICS_FILTER,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get queue servicelevel analysis metrics',
        description: 'get queue servicelevel analysis metrics api',
        tags: ['Queue_Analytics'],
        body: queueServiceLevelMetricsFilterScheme.body,
        response: queueServiceLevelMetricsFilterScheme.response,
      },
    },
    getQueueServicelevelAnalysisMetricsHandler
  );

  // TODO:get queue outboundcall bestime all metrics
  server.post(
    VOICEQUEUEANALYTICS.QUEUE_OUTBOUNDCALL_BESTTIME_CALL_METRICS_FILTER,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get queue outboundcall bestime all metrics',
        description: 'get queue outboundcall bestime all metrics api',
        tags: ['Queue_Analytics'],
        body: queueOutboundCallBestTimeMetricsFilterScheme.body,
        response: queueOutboundCallBestTimeMetricsFilterScheme.response,
      },
    },
    getQueueOutboundCallBestTimeMetricsHandler
  );

  // TODO:Create Edit Column queue voice Analysis
  server.post(
    VOICEQUEUEANALYTICS.CREATE_EDIT_COLUMN_QUEUE_VOICE_ANALYTICS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Create Edit Column queue voice Analysis',
        description: 'Create Edit Column queue voice Analysispi',
        tags: ['Queue_Analytics'],
      },
    },
    createEditQueueVoiceAnalyticsHandler
  );

  // TODO:Get Column queue voice Analysis
  server.get(
    `${VOICEQUEUEANALYTICS.GET_COLUMN_QUEUE_VOICE__ANALYTICS}/:id`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get Column queue voice Analysis',
        description: 'get Column queue voice Analysisapi',
        tags: ['Queue_Analytics'],
      },
    },
    getCoulumnQueueVoiceAnalyticsHandler
  );

  // TODO:Get queue voice inbound trendchart
  server.post(
    VOICEQUEUEANALYTICS.QUEUE_VOICE_INBOUND_TRENDCHART,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get queue voice inbound trendchart',
        description: 'get queue voice inbound trendchart api',
        tags: ['Queue_Analytics'],
      },
    },
    getWebVoiceInboundTrendChartHandler
  );

  // TODO:Get queue voice Outbound trendchart
  server.post(
    VOICEQUEUEANALYTICS.QUEUE_VOICE_OUTBOUND_TRENDCHART,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get queue voice Outbound trendchart',
        description: 'get queue voice Outbound trendchart api',
        tags: ['Queue_Analytics'],
      },
    },
    getWebVoiceOutboundTrendChartHandler
  );

  // TODO:Get queue voice callback trendchart
  server.post(
    VOICEQUEUEANALYTICS.QUEUE_VOICE_CALLBACK_TRENDCHART,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get queue voice callback trendchart',
        description: 'get queue voice callback trendchart api',
        tags: ['Queue_Analytics'],
      },
    },
    getWebVoiceCallBackTrendChartHandler
  );

  // TODO:Get queue voice outbound best time trendchart
  server.post(
    VOICEQUEUEANALYTICS.QUEUE_VOICE_OUTBOUND_BEST_TIME_TRENDCHART,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get queue voice outbound best time trendchart',
        description: 'get queue voice outbound best time trendchart api',
        tags: ['Queue_Analytics'],
      },
    },
    getWebVoiceOutboundBestTimeTrendChartHandler
  );

  // TODO:Get queue voice servicelevel trendchart
  server.post(
    VOICEQUEUEANALYTICS.QUEUE_VOICE_SERVICELEVEL_TRENDCHART,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get queue voice servicelevel trendchart',
        description: 'get queue voice servicelevel trendchart api',
        tags: ['Queue_Analytics'],
      },
    },
    getWebVoiceServiceLevelTrendChartHandler
  );

  next();
}
