import {
  WHATSAPPQUEUEANALYTICS,
} from '../../../helpers/constants';
import {
  createEditQueueWhatsappAnalyticsHandler,
  getCoulumnQueueWhatsappAnalyticsHandler,
  getQueueWhatsappInboundAnalysisHandler,
  getQueueWhatsappPerformanceAnalysisHandler,
  getQueueWhatsappServicelevelAnalysisHandler,
  getWhatsappInboundTrendChartHandler,
  getWhatsappPerformanceTrendChartHandler,
  getWhatsappServicelevelTrendChartHandler,
} from './handler';

export default function whatsappQueueAnalyticsHandler(server: any, options: any, next: any) {

  // TODO:get queue inboundchat analysis metrics
  server.post(
    WHATSAPPQUEUEANALYTICS.QUEUE_WHATSAPP_INBOUND_ANALYSIS_METRICS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get queue whatsapp inbound analysis metrics',
        description: 'get queue whatsapp inbound analysis metrics api',
        tags: ['Queue_Analytics'],
      },
    },
    getQueueWhatsappInboundAnalysisHandler
  );

  // TODO:get queue whatsapp performance analysis metrics
  server.post(
    WHATSAPPQUEUEANALYTICS.QUEUE_WHATSAPP_PERFORMANCE_ANALYSIS_METRICS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get queue whatsapp performance analysis metrics',
        description: 'get queue whatsapp performance analysis metrics api',
        tags: ['Queue_Analytics'],
      },
    },
    getQueueWhatsappPerformanceAnalysisHandler
  );

  // TODO:get queue whatsapp servicelevel analysis metrics
  server.post(
    WHATSAPPQUEUEANALYTICS.QUEUE_WHATSAPP_SERVICELEVEL_ANALYSIS_METRICS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get queue whatsapp servicelevel analysis metrics',
        description: 'get queue whatsapp servicelevel analysis metrics api',
        tags: ['Queue_Analytics'],
      },
    },
    getQueueWhatsappServicelevelAnalysisHandler
  );

  // TODO:Create Edit Column queue whatsapp Analysis
  server.post(
    WHATSAPPQUEUEANALYTICS.CREATE_EDIT_COLUMN_QUEUE_WHATSAPP_ANALYTICS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Create Edit Column queue whatsapp Analysis',
        description: 'Create Edit Column queue whatsapp Analysis api',
        tags: ['Queue_Analytics'],
      },
    },
    createEditQueueWhatsappAnalyticsHandler
  );

  // TODO:Get Column queue whatsapp Analysis
  server.get(
    `${WHATSAPPQUEUEANALYTICS.GET_COLUMN_QUEUE_WHATSAPP__ANALYTICS}/:id`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get Column queue whatsapp Analysis',
        description: 'get Column queue whatsapp Analysis api',
        tags: ['Queue_Analytics'],
      },
    },
    getCoulumnQueueWhatsappAnalyticsHandler
  );

  // TODO:queue whatsapp inbound trendchart
  server.post(
    WHATSAPPQUEUEANALYTICS.QUEUE_WHATSAPP_INBOUND_TRENDCHART,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get queue whatsapp inbound trendchart',
        description: 'get queue whatsapp inbound trendchart api',
        tags: ['Queue_Analytics'],
      },
    },
    getWhatsappInboundTrendChartHandler
  );

  // TODO: queue whatsapp performance trendchart
  server.post(
    WHATSAPPQUEUEANALYTICS.QUEUE_WHATSAPP_PERFORMANCE_TRENDCHART,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get queue whatsapp performance trendchart',
        description: 'get queue whatsapp performance trendchart api',
        tags: ['Queue_Analytics'],
      },
    },
    getWhatsappPerformanceTrendChartHandler
  );

  // TODO: queue whatsapp servicelevel trendchart
  server.post(
    WHATSAPPQUEUEANALYTICS.QUEUE_WHATSAPP_SERVICELEVEL_TRENDCHART,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get queue whatsapp servicelevel trendchart',
        description: 'get queue whatsapp servicelevel trendchart api',
        tags: ['Queue_Analytics'],
      },
    },
    getWhatsappServicelevelTrendChartHandler
  );

  next();
}
