
import { ANALYTICS, ANALYTICSCHAT } from '../../../helpers/constants';
import {
  agentChatPerformanceMetricsFilterHandler,
  agentChatServicelevelMetricsFilterHandler,
  agentInboundChatAnalysisMetrics,
  agentInboundChatAnalysisMetricsFilterHandler,
  agentInboundChatVolumeHandler,
  agentInboundTrendChatHandler,
  agentPerfomanceTrendChatHandler,
  agentPerformanceChatAnalysisMetrics,
  agentServiceLevelAgreementMetricsHandler,
  agentServiceLevelAnalysisMetrics,
  chatPerformanceMetrics,
  chatPerformanceMetricsFilter,
  chatPerformanceMetricsTrendChat,
  chatServiceLevelMetricsFilter,
  createAgentAnalyticsHandler,
  editQueueAgentHandler,
  getAnalyticsChatSessionReportHandler,
  getAnalyticsQueueChatReportHandler,
  getAnalyticsSkillChatReportHandler,
  getChatDispositionStatusHandler,
  getChatDispositionViewDetails,
  getCoulumnAgentAnalyticsHandler,
  getEditColumnAnalyticsHandler,
  getInboundAgentFieldHandler,
  getInboundAgentServiceLevelFieldHandlers,
  getInboundAgentServiceLevelSlaFieldHandler,
  getInboundPerformanceFieldHandler,
  getInboundQueueFieldHandler,
  getInboundQueuePerformanceFieldHandler,
  getInboundQueueServiceLevelFieldHandler,
  getInboundQueueServiceLevelSlaFieldHandler,
  getQueueAnalysisChatReportHandler,
  getVisitorInsightList,
  inboundChatAnalysisMetrics,
  inboundChatAnalysisMetricsFilter,
  inboundTrendChatHandler,
  insertUpdateVisitorInsight,
  queueInboundChatVolumeHandler,
  serviceLevelAgreementMetrics,
  serviceLevelTrendChatHandler
} from './handler';
import {
  agentChatPerformanceMetricsFilterScheme,
  agentChatServicelevelMetricsFilterScheme,
  agentInboundChatAnalysisMetricsFilterScheme
} from './schema';

export default function chatAnalyticsHandler(server: any, options: any, next: any) {
  server.post(
    ANALYTICSCHAT.GET_CHAT_DISPOSTION_STATUS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Chat analytics for report disposition status',
        description: 'Chat analytics for report disposition status api',
        tags: ['Chat analytics'],
        // body: analyticsReportScheme.body,
        // response: voiceReportScheme.response,
      },
    },
    getChatDispositionStatusHandler
  );

  server.post(
    ANALYTICSCHAT.GET_CHAT_DISPOSITION_VIEW_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Chat analytics for report disposition status for view details',
        description: 'Chat analytics for report disposition status api',
        tags: ['Chat analytics'],
        // body: analyticsReportScheme.body,
        // response: voiceReportScheme.response,
      },
    },
    getChatDispositionViewDetails
  );

  server.post(
    ANALYTICSCHAT.GET_QUEUE_CHAT_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'analytics queue chat report',
        description: 'analytics queue chat report',
        tags: ['chatAnalytics'],
        // body: analyticsReportScheme.body,
        // response: voiceReportScheme.response,
      },
    },
    getAnalyticsQueueChatReportHandler
  );

  server.post(
    ANALYTICSCHAT.GET_QUEUE_ANALYSIS_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'analytics for queue analysis chat report',
        description: 'analytics for queue analysis chat report',
        tags: ['chatAnalytics'],
        // body: analyticsReportScheme.body,
        // response: voiceReportScheme.response,
      },
    },
    getQueueAnalysisChatReportHandler
  );

  server.post(
    ANALYTICSCHAT.GET_SKILL_CHAT_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'analytics chat session report',
        description: 'analytics chat session report',
        tags: ['chatAnalytics'],
        // body: analyticsReportScheme.body,
        // response: voiceReportScheme.response,
      },
    },
    getAnalyticsSkillChatReportHandler
  );

  server.post(
    ANALYTICSCHAT.GET_CHAT_SESSION_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'analytics skill based chat report',
        description: 'analytics skill based chat report',
        tags: ['chatAnalytics'],
        // body: analyticsReportScheme.body,
        // response: voiceReportScheme.response,
      },
    },
    getAnalyticsChatSessionReportHandler
  );

  // TODO:inbound chat analytics metrics
  server.get(
    ANALYTICS.INBOUND_CHAT_ANALYSIS_METRICS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Inbound chat analysis metrics',
        description: 'Inbound chat analysis metrics api',
        tags: ['Analytics'],
        // body: analyticsReportScheme.body,
        // response: analyticsReportScheme.response,
      },
    },
    inboundChatAnalysisMetrics
  );

  // TODO:chat performance metrics
  server.get(
    ANALYTICS.CHAT_PERFORMANCE_METRICS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Chat performance metrics',
        description: 'Chat performance metrics api',
        tags: ['Analytics'],
        // body: analyticsReportScheme.body,
        // response: analyticsReportScheme.response,
      },
    },
    chatPerformanceMetrics
  );

  // TODO:service level agreement metrics
  server.get(
    ANALYTICS.SERVICE_LEVEL_AGREEMENT_METRICS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Chat performance metrics',
        description: 'Chat performance metrics api',
        tags: ['Analytics'],
        // body: analyticsReportScheme.body,
        // response: analyticsReportScheme.response,
      },
    },
    serviceLevelAgreementMetrics
  );

  // TODO: inbound chat analytics metrics filter
  server.post(
    ANALYTICS.INBOUND_CHAT_ANALYSIS_METRICS_FILTER,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Inbound chat analysis metrics filter',
        description: 'Inbound chat analysis metrics filter api',
        tags: ['Analytics'],
        // body: analyticsReportScheme.body,
        // response: analyticsReportScheme.response,
      },
    },
    inboundChatAnalysisMetricsFilter
  );

  // TODO:chat performance metrics filter
  server.post(
    ANALYTICS.CHAT_PERFORMANCE_METRICS_FILTER,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Chat performance metrics filter',
        description: 'Chat performance metrics filter api',
        tags: ['Analytics'],
        // body: analyticsReportScheme.body,
        // response: analyticsReportScheme.response,
      },
    },
    chatPerformanceMetricsFilter
  );

  // TODO:chat service level metrics filter
  server.post(
    ANALYTICS.CHAT_SERVICE_LEVEL_METRICS_FILTER,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Chat service level metrics filter',
        description: 'Chat service level metrics filter api',
        tags: ['Analytics'],
        // body: analyticsReportScheme.body,
        // response: analyticsReportScheme.response,
      },
    },
    chatServiceLevelMetricsFilter
  );

  // TODO:inbound trend chat
  server.post(
    ANALYTICS.INBOUND_TREND_CHAT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'inbound trend chat',
        description: 'inbound trend chat api',
        tags: ['Analytics'],
        // body: analyticsReportScheme.body,
        // response: analyticsReportScheme.response,
      },
    },
    inboundTrendChatHandler
  );

  // TODO:agent inbound trend chat
  server.post(
    ANALYTICS.INBOUND_AGENT_TREND_CHAT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'inbound agent trend chat',
        description: 'inbound agent trend chat api',
        tags: ['Analytics'],
        // body: analyticsReportScheme.body,
        // response: analyticsReportScheme.response,
      },
    },
    agentInboundTrendChatHandler
  );

  // TODO:agent performances trend chat
  server.post(
    ANALYTICS.AGENT_PERFORMANCES_TREND_CHAT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'agent performance trend chat',
        description: 'agent performance trend chat api',
        tags: ['Analytics'],
        // body: analyticsReportScheme.body,
        // response: analyticsReportScheme.response,
      },
    },
    agentPerfomanceTrendChatHandler
  );

  // TODO:chat performance metrics trend chat
  server.post(
    ANALYTICS.CHAT_PERFORMANCE_METRICS_TREND_CHAT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Chat performance metrics trend chat',
        description: 'Chat performance metrics trend chat api',
        tags: ['Analytics'],
        // body: analyticsReportScheme.body,
        // response: analyticsReportScheme.response,
      },
    },
    chatPerformanceMetricsTrendChat
  );

  // TODO:chat performance metrics trend chat
  server.post(
    ANALYTICS.CHAT_SERVICE_LEVEL_TREND_CHAT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'service level trend chat',
        description: 'service level trend chat api',
        tags: ['Analytics'],
        // body: analyticsReportScheme.body,
        // response: analyticsReportScheme.response,
      },
    },
    serviceLevelTrendChatHandler
  );

  // TODO:inbound chat analytics metrics for agent
  server.get(
    ANALYTICS.AGENT_INBOUND_CHAT_ANALYSIS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'agent inbound chat analysis metrics',
        description: 'agent inbound chat analysis metrics api',
        tags: ['Analytics'],
        // body: analyticsReportScheme.body,
        // response: analyticsReportScheme.response,
      },
    },
    agentInboundChatAnalysisMetrics
  );

  // TODO:agent performance chat analytics
  server.get(
    ANALYTICS.AGENT_PERFORMANCE_CHAT_METRICS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'agent Chat performance metrics',
        description: 'agent Chat performance metrics api',
        tags: ['Analytics'],
        // body: analyticsReportScheme.body,
        // response: analyticsReportScheme.response,
      },
    },
    agentPerformanceChatAnalysisMetrics
  );

  // TODO:agent service level chat analytics
  server.get(
    ANALYTICS.AGENT_SERVICE_LEVEL_METRICS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'agent service level metrics',
        description: 'agent service level  metrics api',
        tags: ['Analytics'],
        // body: analyticsReportScheme.body,
        // response: analyticsReportScheme.response,
      },
    },
    agentServiceLevelAnalysisMetrics
  );

  // TODO:get inbound queue field
  server.get(
    ANALYTICS.GET_INBOUND_QUEUE_FIELD,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get inbound queues field',
        description: 'get inbound queues field api',
        tags: ['Analytics'],
        // body: analyticsReportScheme.body,
        // response: analyticsReportScheme.response,
      },
    },
    getInboundQueueFieldHandler
  );

  // TODO:get inbound agent field
  server.get(
    ANALYTICS.GET_INBOUND_AGENT_FIELD,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get inbound agent field',
        description: 'get inbound agent field api',
        tags: ['Analytics'],
        // body: analyticsReportScheme.body,
        // response: analyticsReportScheme.response,
      },
    },
    getInboundAgentFieldHandler
  );

  // TODO:get inbound performance field
  server.get(
    ANALYTICS.GET_INBOUND_PERFORMANCE_FIELD,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get inbound agent field',
        description: 'get inbound agent field api',
        tags: ['Analytics'],
        // body: analyticsReportScheme.body,
        // response: analyticsReportScheme.response,
      },
    },
    getInboundPerformanceFieldHandler
  );

  // TODO:get inbound queue performance field
  server.get(
    ANALYTICS.GET_INBOUND_QUEUE_PERFORMANCE_FIELD,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get inbound queue performance field',
        description: 'get inbound queue performance field api',
        tags: ['Analytics'],
        // body: analyticsReportScheme.body,
        // response: analyticsReportScheme.response,
      },
    },
    getInboundQueuePerformanceFieldHandler
  );

  // TODO:get inbound queue service level field
  server.get(
    ANALYTICS.GET_INBOUND_QUEUE_SERVICE_LEVEL_FIELD,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get inbound queue service level field',
        description: 'get inbound queue service level field api',
        tags: ['Analytics'],
        // body: analyticsReportScheme.body,
        // response: analyticsReportScheme.response,
      },
    },
    getInboundQueueServiceLevelFieldHandler
  );

  // TODO:get inbound queue service level SLA field
  server.get(
    ANALYTICS.GET_INBOUND_QUEUE_SERVICE_LEVEL_SLA_FIELD,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get inbound queue service level field',
        description: 'get inbound queue service level field api',
        tags: ['Analytics'],
        // body: analyticsReportScheme.body,
        // response: analyticsReportScheme.response,
      },
    },
    getInboundQueueServiceLevelSlaFieldHandler
  );

  // TODO:get inbound agent service level field
  server.get(
    ANALYTICS.GET_INBOUND_AGENT_SERVICE_LEVEL_FIELD,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get inbound agent service level field',
        description: 'get inbound agent service level field api',
        tags: ['Analytics'],
        // body: analyticsReportScheme.body,
        // response: analyticsReportScheme.response,
      },
    },
    getInboundAgentServiceLevelFieldHandlers
  );

  // TODO:get inbound agent service level SLA field
  server.get(
    ANALYTICS.GET_INBOUND_AGENT_SERVICE_LEVEL_SLA_FIELD,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get inbound agent service level field',
        description: 'get inbound agent service level field api',
        tags: ['Analytics'],
        // body: analyticsReportScheme.body,
        // response: analyticsReportScheme.response,
      },
    },
    getInboundAgentServiceLevelSlaFieldHandler
  );

  // TODO:edit column queue agent analytics
  server.post(
    ANALYTICS.EDIT_COLUMN_QUEUE_AGENT_ANALYTICS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'edit column queue agents',
        description: 'edit column queue agents api',
        tags: ['Analytics'],
      },
    },
    editQueueAgentHandler
  );

  // TODO:get edit column queue agent analytics
  server.get(
    ANALYTICS.GET_EDIT_COLUMN_QUEUE_AGENT_ANALYTICS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get edit column analytics',
        description: 'get edit column  analytics api',
        tags: ['Analytics'],
      },
    },
    getEditColumnAnalyticsHandler
  );

  // TODO:queue inbound chat volume
  server.post(
    ANALYTICS.QUEUE_INBOUND_CHAT_VOLUME,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'queue inbound chat volume',
        description: 'queue inbound chat volume api',
        tags: ['Analytics'],
        // body: analyticsReportScheme.body,
        // response: analyticsReportScheme.response,
      },
    },
    queueInboundChatVolumeHandler
  );

  // TODO:agent inbound chat volume
  server.post(
    ANALYTICS.AGENT_INBOUND_CHAT_VOLUME,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'agent inbound chat volume',
        description: 'agent inbound chat volume api',
        tags: ['Analytics'],
        // body: analyticsReportScheme.body,
        // response: analyticsReportScheme.response,
      },
    },
    agentInboundChatVolumeHandler
  );

  // TODO: Agent inbound chat analytics metrics filter
  server.post(
    ANALYTICS.AGENT_INBOUND_CHAT_ANALYSIS_METRICS_FILTER,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Agent Inbound chat analysis metrics filter',
        description: 'Agent Inbound chat analysis metrics filter api',
        tags: ['Analytics'],
        body: agentInboundChatAnalysisMetricsFilterScheme.body,
        response: agentInboundChatAnalysisMetricsFilterScheme.response,
      },
    },
    agentInboundChatAnalysisMetricsFilterHandler
  );

  // TODO: Agent chat performance metrics filter
  server.post(
    ANALYTICS.AGENT_CHAT_PERFORMANCES_METRICS_FILTER,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Agent chat performance metrics filter',
        description: 'Agent chat performance metrics filter api',
        tags: ['Analytics'],
        body: agentChatPerformanceMetricsFilterScheme.body,
        response: agentChatPerformanceMetricsFilterScheme.response,
      },
    },
    agentChatPerformanceMetricsFilterHandler
  );

  // TODO: Agent chat servicelevel metrics filter
  server.post(
    ANALYTICS.AGENT_CHAT_SERVICELEVEL_METRICS_FILTER,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Agent chat servicelevel metrics filter',
        description: 'Agent chat servicelevel metrics filter api',
        tags: ['Analytics'],
        body: agentChatServicelevelMetricsFilterScheme.body,
        response: agentChatServicelevelMetricsFilterScheme.response,
      },
    },
    agentChatServicelevelMetricsFilterHandler
  );

  // TODO:Agent services level agreement metrics
  server.get(
    ANALYTICS.AGENT_SERVICES_LEVEL_AGREEMENT_METRIC,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'agent services level agreement metrics',
        description: 'agent services level agreement metrics api',
        tags: ['Analytics'],
        // body: analyticsReportScheme.body,
        // response: analyticsReportScheme.response,
      },
    },
    agentServiceLevelAgreementMetricsHandler
  );

  // TODO:Create Edit Column Agent Analytics
  server.post(
    ANALYTICS.CREATE_EDIT_COLUMN_AGENT_ANALYTICS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'create edit column  agent analytics',
        description: 'create edit column  agent analytics api',
        tags: ['Agent_Analytics'],
      },
    },
    createAgentAnalyticsHandler
  );

  // TODO:Get Column Agent Analytics
  server.get(
    `${ANALYTICS.GET_COLUMN_AGENT_ANALYTICS}/:id`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get column agent analytics',
        description: 'get column agent analytics api',
        tags: ['Agent_Analytics'],
      },
    },
    getCoulumnAgentAnalyticsHandler
  );

  server.post(
    ANALYTICS.INSERT_UPDATE_VISITOR_INSIGHT,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'insert update visitor insight',
        description: 'insert update visitor insight api',
        tags: ['Analytics'],
      },
    },
    insertUpdateVisitorInsight
  );

  server.get(
    ANALYTICS.GET_VISITOR_INSIGHT_DATA,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get visitor insight data',
        description: 'get visitor insight data api',
        tags: ['Analytics'],
      },
    },
    getVisitorInsightList
  );

  next();
}
