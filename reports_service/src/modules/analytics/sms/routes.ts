/**
 * @createdBy <kathiravan.r@worktual.com>
 * @createdOn <02/04/2024>
 */

import {
  AGENTANALYTICSMS
} from '../../../helpers/constants';
import {
  agentSmsInboundMetricsFilterHandler,
  agentSmsInboundTrendChartHandler,
  agentSmsPerformanceMetricsFilterHandler,
  agentSmsPerformancesTrendChartHandler,
  agentSmsServiceLevelMetricsFilterHandler,
  agentSmsServicelevelTrendChartHandler,
  createSmsAnalyticsHandler,
  getCoulumnSmsAnalyticsHandler
} from './handler';
import {
  agentSmsInboundMetricsFilterScheme,
  agentSmsPerformanceMetricsFilterScheme,
  agentSmsServicelevelMetricsFilterScheme
} from './schema';

export default function smsAnalyticsHandler(server: any, options: any, next: any) {

  // TODO: Agent sms inbound metrics filter
  server.post(
    AGENTANALYTICSMS.AGENT_SMS_INBOUND_ANALYSIS_METRICS_FILTER,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Agent sms inbound metrics filter',
        description: 'Agent sms inbound metrics filter api',
        tags: ['Sms_Analytics'],
        body: agentSmsInboundMetricsFilterScheme.body,
        response: agentSmsInboundMetricsFilterScheme.response,
      },
    },
    agentSmsInboundMetricsFilterHandler
  );

  // TODO: Agent sms performance metrics filter
  server.post(
    AGENTANALYTICSMS.AGENT_SMS_PERFORMANCES_ANALYSIS_METRICS_FILTER,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Agent sms performance metrics filter',
        description: 'Agent sms performance metrics filter api',
        tags: ['Sms_Analytics'],
        body: agentSmsPerformanceMetricsFilterScheme.body,
        response: agentSmsPerformanceMetricsFilterScheme.response,
      },
    },
    agentSmsPerformanceMetricsFilterHandler
  );

  // TODO: Agent sms servicelevel metrics filter
  server.post(
    AGENTANALYTICSMS.AGENT_SMS_SERVICELVEL_ANALYSIS_METRICS_FILTER,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Agent sms servicelevel metrics filter',
        description: 'Agent sms servicelevel metrics filter api',
        tags: ['Sms_Analytics'],
        body: agentSmsServicelevelMetricsFilterScheme.body,
        response: agentSmsServicelevelMetricsFilterScheme.response,
      },
    },
    agentSmsServiceLevelMetricsFilterHandler
  );

  // TODO:agent sms inbound trendchart
  server.post(
    AGENTANALYTICSMS.AGENT_SMS_INBOUND_TRENDCHART,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'agent sms inbound trendchart',
        description: 'agent sms inbound trendchart api',
        tags: ['Sms_Analytics'],
      },
    },
    agentSmsInboundTrendChartHandler
  );

  // TODO:agent sms servicelevel trendchart
  server.post(
    AGENTANALYTICSMS.AGENT_SMS_SERVICELEVEL_TRENDCHART,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'agent sms servicelevel trendchart',
        description: 'agent sms servicelevel trendchart api',
        tags: ['Sms_Analytics'],
      },
    },
    agentSmsServicelevelTrendChartHandler
  );

  // TODO:agent sms performances trendchart
  server.post(
    AGENTANALYTICSMS.AGENT_SMS_PERFORMANCES_TRENDCHART,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'agent sms performances trendchart',
        description: 'agent sms performances trendchart api',
        tags: ['Sms_Analytics'],
      },
    },
    agentSmsPerformancesTrendChartHandler
  );

  // TODO:Create Edit Column sms Analytics
  server.post(
    AGENTANALYTICSMS.CREATE_EDIT_COLUMN_SMS_ANALYTICS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'create edit column  sms analytics',
        description: 'create edit column  sms analytics api',
        tags: ['Sms_Analytics'],
      },
    },
    createSmsAnalyticsHandler
  );

  // TODO:Get Column sms Agent Analytics
  server.get(
    `${AGENTANALYTICSMS.GET_COLUMN_SMS_AGENT_ANALYTICS}/:id`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get column call analytics',
        description: 'get column call analytics api',
        tags: ['Sms_Analytics'],
      },
    },
    getCoulumnSmsAnalyticsHandler
  );

  next();
}
