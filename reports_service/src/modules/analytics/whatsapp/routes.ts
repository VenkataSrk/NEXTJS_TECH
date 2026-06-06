import {
  AGENTWHATSAPPANALYTIC
} from '../../../helpers/constants';
import {
  agentWhatsappInboundMetricsFilterHandler,
  agentWhatsappInboundTrendChartHandler,
  agentWhatsappPerformancesMetricsFilterHandler,
  agentWhatsappPerformancesTrendChartHandler,
  agentWhatsappServicelevelMetricsFilterHandler,
  agentWhatsappServicelevelTrendChartHandler,
  createWhatsappAnalyticsHandler,
  getCoulumnWhatsappAnalyticsHandler
} from './handler';
import {
  agentWhatsappInboundMetricsFilterScheme,
  agentWhatsappPerformancesMetricsFilterScheme,
  agentWhatsappServicelevelMetricsFilterScheme
} from './schema';

export default function WhatsappAnalyticsHandler(server: any, options: any, next: any) {

  // TODO: Agent whatsapp inbound metrics filter
  server.post(
    AGENTWHATSAPPANALYTIC.AGENT_WHATSAPP_INBOUND_ANALYTICS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Agent whatsapp inbound metrics filter',
        description: 'Agent whatsapp inbound metrics filter api',
        tags: ['Whatsapp_Analytics'],
        body: agentWhatsappInboundMetricsFilterScheme.body,
        response: agentWhatsappInboundMetricsFilterScheme.response,
      },
    },
    agentWhatsappInboundMetricsFilterHandler
  );

  // TODO: Agent whatsapp inbound metrics filter
  server.post(
    AGENTWHATSAPPANALYTIC.AGENT_WHATSAPP_PERFORMANCES_ANALYTICS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Agent whatsapp Performances metrics filter',
        description: 'Agent whatsapp Performances metrics filter api',
        tags: ['Whatsapp_Analytics'],
        body: agentWhatsappPerformancesMetricsFilterScheme.body,
        response: agentWhatsappPerformancesMetricsFilterScheme.response,
      },
    },
    agentWhatsappPerformancesMetricsFilterHandler
  );

  // TODO: Agent whatsapp servicelevel metrics filter
  server.post(
    AGENTWHATSAPPANALYTIC.AGENT_WHATSAPP_SERVICELEVEL_ANALYTICS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Agent whatsapp servicelevel metrics filter',
        description: 'Agent whatsapp servicelevel metrics filter api',
        tags: ['Whatsapp_Analytics'],
        body: agentWhatsappServicelevelMetricsFilterScheme.body,
        response: agentWhatsappServicelevelMetricsFilterScheme.response,
      },
    },
    agentWhatsappServicelevelMetricsFilterHandler
  );

  // TODO:Create Edit Column whatsapp Analytics
  server.post(
    AGENTWHATSAPPANALYTIC.CREATE_EDIT_COLUMN_WHATSAPP_ANALYTICS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'create edit column whatsapp analytics',
        description: 'create edit column  whatsapp analytics api',
        tags: ['Whatsapp_Analytics'],
      },
    },
    createWhatsappAnalyticsHandler
  );

  // TODO:Get Column whatsapp Agent Analytics
  server.get(
    `${AGENTWHATSAPPANALYTIC.GET_COLUMN_WHATSAPP_AGENT_ANALYTICS}/:id`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get column call analytics',
        description: 'get column call analytics api',
        tags: ['Whatsapp_Analytics'],
      },
    },
    getCoulumnWhatsappAnalyticsHandler
  );

  // TODO:agent whatsapp inbound trendchart
  server.post(
    AGENTWHATSAPPANALYTIC.AGENT_WHATSAPP_INBOUND_TRENDCHART,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'agent whatsapp inbound trendchart',
        description: 'agent whatsapp inbound trendchart api',
        tags: ['Whatsapp_Analytics'],
      },
    },
    agentWhatsappInboundTrendChartHandler
  );

  // TODO:agent whatsapp performances trendchart
  server.post(
    AGENTWHATSAPPANALYTIC.AGENT_WHATSAPP_PERFORMANCES_TRENDCHART,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'agent whatsapp performances trendchart',
        description: 'agent whatsapp performances trendchart api',
        tags: ['Whatsapp_Analytics'],
      },
    },
    agentWhatsappPerformancesTrendChartHandler
  );

  // TODO:agent whatsapp servicelevel trendchart
  server.post(
    AGENTWHATSAPPANALYTIC.AGENT_WHATSAPP_SERVICELEVEL_TRENDCHART,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'agent whatsapp servicelevel trendchart',
        description: 'agent whatsapp servicelevel trendchart api',
        tags: ['Whatsapp_Analytics'],
      },
    },
    agentWhatsappServicelevelTrendChartHandler
  );

  next();
}
