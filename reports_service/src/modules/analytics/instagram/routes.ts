import {
  AGENTINSTAGRAMANALYTIC
} from '../../../helpers/constants';
import {
  agentInstagramInboundMetricsFilterHandler,
  agentInstagramInboundTrendChartHandler,
  agentInstagramPerformanceMetricsFilterHandler,
  agentInstagramPerformanceTrendChartHandler,
  agentInstagramServiceLevelMetricsFilterHandler,
  agentInstagramServicelevelTrendChartHandler,
  createEditInstagramAnalyticsHandler,
  getCoulumnInstagramAnalyticsHandler
} from './handler';
import { agentInstagramInboundMetricsFilterScheme, agentInstagramPerformanceMetricsFilterScheme, agentInstagramServicelevelMetricsFilterScheme } from './schema';

export default function instagramAnalyticsHandler(server: any, options: any, next: any) {

  // TODO: Agent instagram inbound metrics filter
  server.post(
    AGENTINSTAGRAMANALYTIC.AGENT_INSTAGRAM_INBOUND_ANALYTICS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Agent instagram inbound metrics filter',
        description: 'Agent instagram inbound metrics filter api',
        tags: ['Instagram_Analytics'],
        body: agentInstagramInboundMetricsFilterScheme.body,
        response: agentInstagramInboundMetricsFilterScheme.response,
      },
    },
    agentInstagramInboundMetricsFilterHandler
  );

  // TODO: Agent instagram performances metrics filter
  server.post(
    AGENTINSTAGRAMANALYTIC.AGENT_INSTAGRAM_PERFORMANCES_ANALYTICS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Agent instagram performances metrics filter',
        description: 'Agent instagram inbound performances filter api',
        tags: ['Instagram_Analytics'],
        body: agentInstagramPerformanceMetricsFilterScheme.body,
        response: agentInstagramPerformanceMetricsFilterScheme.response,
      },
    },
    agentInstagramPerformanceMetricsFilterHandler
  );

  // TODO: Agent instagram servicelevel metrics filter
  server.post(
    AGENTINSTAGRAMANALYTIC.AGENT_INSTAGRAM_SERVICELEVEL_ANALYTICS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Agent instagram servicelevel metrics filter',
        description: 'Agent instagram inbound servicelevel filter api',
        tags: ['Instagram_Analytics'],
        body: agentInstagramServicelevelMetricsFilterScheme.body,
        response: agentInstagramServicelevelMetricsFilterScheme.response,
      },
    },
    agentInstagramServiceLevelMetricsFilterHandler
  );

  // TODO:Create Edit Column instagram Analytics
  server.post(
    AGENTINSTAGRAMANALYTIC.CREATE_EDIT_COLUMN_INSTAGRAM_ANALYTICS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'create edit column instagram analytics',
        description: 'create edit column  instagram analytics api',
        tags: ['Instagram_Analytics'],
      },
    },
    createEditInstagramAnalyticsHandler
  );

  // TODO:Get Column instagram Agent Analytics
  server.get(
    `${AGENTINSTAGRAMANALYTIC.GET_COLUMN_INSTAGRAM_AGENT_ANALYTICS}/:id`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get column instagram analytics',
        description: 'get column instagram analytics api',
        tags: ['Instagram_Analytics'],
      },
    },
    getCoulumnInstagramAnalyticsHandler
  );

  // TODO:agent instagram inbound trendchart
  server.post(
    AGENTINSTAGRAMANALYTIC.AGENT_INSTAGRAM_INBOUND_TRENDCHART,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'agent instagram inbound trendchart',
        description: 'agent instagram inbound trendchart api',
        tags: ['Instagram_Analytics'],
      },
    },
    agentInstagramInboundTrendChartHandler
  );

  // TODO:agent instagram performance trendchart
  server.post(
    AGENTINSTAGRAMANALYTIC.AGENT_INSTAGRAM_PERFORMANCES_TRENDCHART,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'agent instagram performance trendchart',
        description: 'agent instagram performance trendchart api',
        tags: ['Instagram_Analytics'],
      },
    },
    agentInstagramPerformanceTrendChartHandler
  );

  // TODO:agent instagram servicelevel trendchart
  server.post(
    AGENTINSTAGRAMANALYTIC.AGENT_INSTAGRAM_SERVICELEVEL_TRENDCHART,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'agent instagram servicelevel trendchart',
        description: 'agent instagram servicelevel trendchart api',
        tags: ['Instagram_Analytics'],
      },
    },
    agentInstagramServicelevelTrendChartHandler
  );

  next();
}
