/**
 * @createdBy <krishnakumar.s@worktual.com>
 * @createdOn <05/04/2023>
 */

import {
  AGENTANALYTICSVOICE,
  ANALYTICS,
  ANALYTICSVOICE
} from '../../../helpers/constants';
import {
  agentCallBackAnalysisMetricsFilterHandler,
  agentCallBackTrendChartHandler,
  agentCallInboundTrendChartHandler,
  agentCallOutboundTrendChartHandler,
  agentCallServicelevelTrendChartHandler,
  agentInboundCallAnalysisMetricsFilterHandler,
  agentOutboundCallAnalysisMetricsFilterHandler,
  agentServiceLevelCallAnalysisMetricsFilterHandler,
  agentVoiceMailAnalysisMetricsFilterHandler,
  agentVoiceMailTrendChartHandler,
  createCallAnalyticsHandler,
  editAnalyticsColumn,
  getAbandonedCallProfileHandler,
  getAnalyticsColumn,
  getCoulumnCallAnalyticsHandler,
  getIntervalAnalyticsHandler,
  getMetricsQueueInboundCallHandler,
  getQueueDispositionHandler
} from './handler';
import {
  agentCallbackAnalysisMetricsFilterScheme,
  agentInboundCallAnalysisMetricsFilterScheme,
  agentOutboundCallAnalysisMetricsFilterScheme,
  agentServiceLevelCallAnalysisMetricsFilterScheme,
  agentVoiceMailAnalysisMetricsFilterScheme,
  analyticsReportScheme
} from './schema';

export default function analyticsHandler(server: any, options: any, next: any) {
  // TODO :Queue Metrics Report for Inbound Call
  server.post(
    ANALYTICSVOICE.GET_QUEUE_BASE_INBOUND_CALL,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'queue metrics report for inbound call',
        description: 'queue metrics report for inbound call api',
        tags: ['voiceAnalytics'],
        body: analyticsReportScheme.body,
        // response: voiceReportScheme.response,
      },
    },
    getMetricsQueueInboundCallHandler
  );

  // TODO:Queue Disposition Status report
  server.post(
    ANALYTICSVOICE.GET_DISPOSITION_STATUS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'queue disposition Status report',
        description: 'queue disposition Status report api',
        tags: ['voiceAnalytics'],
        body: analyticsReportScheme.body,
        // response: userinsertSchema.response,
      },
    },
    getQueueDispositionHandler
  );

  // TODO:Queue Metrics Interval report
  server.post(
    ANALYTICSVOICE.GET_INTERVAL_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'queue interval report',
        description: 'queue interval report api',
        tags: ['voiceAnalytics'],
        body: analyticsReportScheme.body,
        // response: analyticsReportScheme.response,
      },
    },
    getIntervalAnalyticsHandler
  );

  // TODO:Abandoned Call Profile Report
  server.post(
    ANALYTICSVOICE.ABANDONED_CALL_PROFILE,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Abandoned Call Profile Report',
        description: 'Abandoned Call Profile Report api',
        tags: ['voiceAnalytics'],
        body: analyticsReportScheme.body,
        // response: analyticsReportScheme.response,
      },
    },
    getAbandonedCallProfileHandler
  );

  server.post(
    ANALYTICS.EDIT_COLUMNS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Edit analytics table columns',
        description: 'Edit analytics table columns api',
        tags: ['Analytics'],
        // body: analyticsReportScheme.body,
        // response: analyticsReportScheme.response,
      },
    },
    editAnalyticsColumn
  );

  server.get(
    ANALYTICS.GET_COLUMNS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Edit analytics table columns',
        description: 'Edit analytics table columns api',
        tags: ['Analytics'],
        // body: analyticsReportScheme.body,
        // response: analyticsReportScheme.response,
      },
    },
    getAnalyticsColumn
  );

  // TODO: Agent inbound call analytics metrics filter
  server.post(
    AGENTANALYTICSVOICE.AGENT_INBOUND_CALL_ANALYSIS_METRICS_FILTER,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Agent Inbound call analysis metrics filter',
        description: 'Agent Inbound call analysis metrics filter api',
        tags: ['Voice_Analytics'],
        body: agentInboundCallAnalysisMetricsFilterScheme.body,
        response: agentInboundCallAnalysisMetricsFilterScheme.response,
      },
    },
    agentInboundCallAnalysisMetricsFilterHandler
  );

  // TODO: Agent outbound call analytics metrics filter
  server.post(
    AGENTANALYTICSVOICE.AGENT_OUTBOUND_CALL_ANALYSIS_METRICS_FILTER,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Agent outbound call analysis metrics filter',
        description: 'Agent outbound call analysis metrics filter api',
        tags: ['Voice_Analytics'],
        body: agentOutboundCallAnalysisMetricsFilterScheme.body,
        response: agentOutboundCallAnalysisMetricsFilterScheme.response,
      },
    },
    agentOutboundCallAnalysisMetricsFilterHandler
  );

  // TODO: Agent Callback analytics metrics filter
  server.post(
    AGENTANALYTICSVOICE.AGENT_CALLBACK_ANALYSIS_METRICS_FILTER,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Agent callbak analysis metrics filter',
        description: 'Agent callback analysis metrics filter api',
        tags: ['Voice_Analytics'],
        body: agentCallbackAnalysisMetricsFilterScheme.body,
        response: agentCallbackAnalysisMetricsFilterScheme.response,
      },
    },
    agentCallBackAnalysisMetricsFilterHandler
  );

  // TODO: Agent voicemail analytics metrics filter
  server.post(
    AGENTANALYTICSVOICE.AGENT_VOICEMAIL_ANALYSIS_METRICS_FILTER,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Agent voicemail analysis metrics filter',
        description: 'Agent voicemail analysis metrics filter api',
        tags: ['Voice_Analytics'],
        body: agentVoiceMailAnalysisMetricsFilterScheme.body,
        response: agentVoiceMailAnalysisMetricsFilterScheme.response,
      },
    },
    agentVoiceMailAnalysisMetricsFilterHandler
  );

  // TODO: Agent servicelevel call analytics metrics filter
  server.post(
    AGENTANALYTICSVOICE.AGENT_SERVICELEVEL_CALL_ANALYSIS_METRICS_FILTER,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Agent servicelevel call analytics metrics filter',
        description: 'Agent servicelevel call analytics metrics filter api',
        tags: ['Voice_Analytics'],
        body: agentServiceLevelCallAnalysisMetricsFilterScheme.body,
        response: agentServiceLevelCallAnalysisMetricsFilterScheme.response,
      },
    },
    agentServiceLevelCallAnalysisMetricsFilterHandler
  );

  // TODO:Create Edit Column call Analytics
  server.post(
    AGENTANALYTICSVOICE.CREATE_EDIT_COLUMN_CALL_ANALYTICS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'create edit column  call analytics',
        description: 'create edit column  call analytics api',
        tags: ['Voice_Analytics'],
      },
    },
    createCallAnalyticsHandler
  );

  // TODO:Get Column Email Agent Analytics
  server.get(
    `${AGENTANALYTICSVOICE.GET_COLUMN_CALL_AGENT_ANALYTICS}/:id`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get column call analytics',
        description: 'get column call analytics api',
        tags: ['Voice_Analytics'],
      },
    },
    getCoulumnCallAnalyticsHandler
  );

  // TODO:agent call inbound trendchart
  server.post(
    AGENTANALYTICSVOICE.AGENT_CALL_INBOUND_TRENDCHART,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'agent call inbound trendchart',
        description: 'agent call inbound trendchart api',
        tags: ['Voice_Analytics'],
      },
    },
    agentCallInboundTrendChartHandler
  );

  // TODO:agent call outbound trendchart
  server.post(
    AGENTANALYTICSVOICE.AGENT_CALL_OUTBOUND_TRENDCHART,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'agent call outbound trendchart',
        description: 'agent call outbound trendchart api',
        tags: ['Voice_Analytics'],
      },
    },
    agentCallOutboundTrendChartHandler
  );

  // TODO:agent call servicelevel trendchart
  server.post(
    AGENTANALYTICSVOICE.AGENT_CALL_SERVICELEVEL_TRENDCHART,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'agent call servicelevel trendchart',
        description: 'agent call servicelevel trendchart api',
        tags: ['Voice_Analytics'],
      },
    },
    agentCallServicelevelTrendChartHandler
  );

  // TODO:agent call callback trendchart
  server.post(
    AGENTANALYTICSVOICE.AGENT_CALL_CALLBACK_TRENDCHART,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'agent call callback trendchart',
        description: 'agent call callback trendchart api',
        tags: ['Voice_Analytics'],
      },
    },
    agentCallBackTrendChartHandler
  );

  // TODO:agent call voicemail trendchart
  server.post(
    AGENTANALYTICSVOICE.AGENT_CALL_VOICEMAIL_TRENDCHART,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'agent call voicemail trendchart',
        description: 'agent call voicemail trendchart api',
        tags: ['Voice_Analytics'],
      },
    },
    agentVoiceMailTrendChartHandler
  );

  next();

}
