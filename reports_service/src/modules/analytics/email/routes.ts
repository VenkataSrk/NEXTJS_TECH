/**
 * @createdBy <kathiravan.r@worktual.com>
 * @createdOn <02/04/2024>
 */

import {
  AGENTANALYTICSEMAIL
} from '../../../helpers/constants';
import {
  agentEmailInboundMetricsFilterHandler,
  agentEmailInboundTrendChartHandler,
  agentEmailPerformanceMetricsFilterHandler,
  agentEmailPerformancesTrendChartHandler,
  agentEmailServicelevelMetricsFilterHandler,
  agentEmailServicelevelTrendChartHandler,
  createEmailAnalyticColumnsHandler,
  getCoulumnEmailAnalyticsHandler
} from './handler';
import {
  agentEmailInboundMetricsFilterScheme,
  agentEmailPerformanceMetricsFilterScheme,
  agentEmailServicelevelMetricsFilterScheme
} from './schema';

export default function emailAnalyticsHandler(server: any, options: any, next: any) {

  // TODO: Agent email inbound analytics metrics filter
  server.post(
    AGENTANALYTICSEMAIL.AGENT_EMAIL_INBOUND_ANALYSIS_METRICS_FILTER,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Agent email inbound metrics filter',
        description: 'Agent email inbound metrics filter api',
        tags: ['Email_Analytics'],
        body: agentEmailInboundMetricsFilterScheme.body,
        response: agentEmailInboundMetricsFilterScheme.response,
      },
    },
    agentEmailInboundMetricsFilterHandler
  );

  // TODO: Agent email performance metrics filter
  server.post(
    AGENTANALYTICSEMAIL.AGENT_EMAIL_PERFORMANCES_ANALYSIS_METRICS_FILTER,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Agent email performance metrics filter',
        description: 'Agent email performance metrics filter api',
        tags: ['Email_Analytics'],
        body: agentEmailPerformanceMetricsFilterScheme.body,
        response: agentEmailPerformanceMetricsFilterScheme.response,
      },
    },
    agentEmailPerformanceMetricsFilterHandler
  );

  // TODO: agent email servicelevel metrics filter
  server.post(
    AGENTANALYTICSEMAIL.AGENT_EMAIL_SERVICELEVEL_ANALYSIS_METRICS_FILTER,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Agent email servicelevel metrics filter',
        description: 'Agent email servicelevel metrics filter api',
        tags: ['Email_Analytics'],
        body: agentEmailServicelevelMetricsFilterScheme.body,
        response: agentEmailServicelevelMetricsFilterScheme.response,
      },
    },
    agentEmailServicelevelMetricsFilterHandler
  );

  // TODO:Create Edit Column Email Agent Analytics
  server.post(
    AGENTANALYTICSEMAIL.CREATE_EDIT_COLUMN_EMAIL_AGENT_ANALYTICS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'create edit column email agent analytics',
        description: 'create edit column email agent analytics api',
        tags: ['Email_Analytics'],
      },
    },
    createEmailAnalyticColumnsHandler
  );

  // TODO:Get Column Email Agent Analytics
  server.get(
    `${AGENTANALYTICSEMAIL.GET_COLUMN_EMAIL_AGENT_ANALYTICS}/:id`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get column agent analytics',
        description: 'get column agent analytics api',
        tags: ['Email_Analytics'],
      },
    },
    getCoulumnEmailAnalyticsHandler
  );

  // TODO:agent Email inbound trendchart
  server.post(
    AGENTANALYTICSEMAIL.AGENT_EMAIL_INBOUND_TRENDCHART,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'agent Email inbound trendchart',
        description: 'agent Email inbound trendchart api',
        tags: ['Email_Analytics'],
      },
    },
    agentEmailInboundTrendChartHandler
  );

  // TODO:agent Email performances trendchart
  server.post(
    AGENTANALYTICSEMAIL.AGENT_EMAIL_PERFORMANCES_TRENDCHART,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'agent Email performances trendchart',
        description: 'agent Email performances trendchart api',
        tags: ['Email_Analytics'],
      },
    },
    agentEmailPerformancesTrendChartHandler
  );

  // TODO:agent Email servicelevel trendchart
  server.post(
    AGENTANALYTICSEMAIL.AGENT_EMAIL_SERVICELEVEL_TRENDCHART,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'agent Email servicelevel trendchart',
        description: 'agent Email servicelevel trendchart api',
        tags: ['Email_Analytics'],
      },
    },
    agentEmailServicelevelTrendChartHandler
  );

  next();
}
