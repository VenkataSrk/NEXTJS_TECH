import {
  AGENTFACEBOOKANALYTIC
} from '../../../helpers/constants';
import {
  agentFacebookInboundMetricsFilterHandler,
  agentFacebookInboundTrendChartHandler,
  agentFacebookPerformancesMetricsFilterHandler,
  agentFacebookPerformancesTrendChartHandler,
  agentFacebookServicelevelMetricsFilterHandler,
  agentFacebookServicelevelTrendChartHandler,
  createEditFacebookAnalyticsHandler,
  getCoulumnFacebookAnalyticsHandler
} from './handler';
import {
  agentFacebookInboundMetricsFilterScheme,
  agentFacebookPerformanceMetricsFilterScheme,
  agentFacebookservicelevelMetricsFilterScheme
} from './schema';

export default function facebookAnalyticsHandler(server: any, options: any, next: any) {

  // TODO: Agent facebook inbound metrics filter
  server.post(
    AGENTFACEBOOKANALYTIC.AGENT_FACEBOOK_INBOUND_ANALYTICS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Agent facebook inbound metrics filter',
        description: 'Agent facebook inbound metrics filter api',
        tags: ['FaceBook_Analytics'],
        body: agentFacebookInboundMetricsFilterScheme.body,
        response: agentFacebookInboundMetricsFilterScheme.response,
      },
    },
    agentFacebookInboundMetricsFilterHandler
  );

  // TODO: Agent facebook performance metrics filter
  server.post(
    AGENTFACEBOOKANALYTIC.AGENT_FACEBOOK_PERFORMANCES_ANALYTICS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Agent facebook performance metrics filter',
        description: 'Agent facebook performance metrics filter api',
        tags: ['FaceBook_Analytics'],
        body: agentFacebookPerformanceMetricsFilterScheme.body,
        response: agentFacebookPerformanceMetricsFilterScheme.response,
      },
    },
    agentFacebookPerformancesMetricsFilterHandler
  );

  // TODO: Agent facebook servicelevel metrics filter
  server.post(
    AGENTFACEBOOKANALYTIC.AGENT_FACEBOOK_SERVICELEVEL_ANALYTICS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Agent facebook servicelevel metrics filter',
        description: 'Agent facebook servicelevel metrics filter api',
        tags: ['FaceBook_Analytics'],
        body: agentFacebookservicelevelMetricsFilterScheme.body,
        response: agentFacebookservicelevelMetricsFilterScheme.response,
      },
    },
    agentFacebookServicelevelMetricsFilterHandler
  );

  // TODO:Create Edit Column facebook Analytics
  server.post(
    AGENTFACEBOOKANALYTIC.CREATE_EDIT_COLUMN_FACEBOOK_ANALYTICS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'create edit column facebook analytics',
        description: 'create edit column  facebook analytics api',
        tags: ['FaceBook_Analytics'],
      },
    },
    createEditFacebookAnalyticsHandler
  );

  // TODO:Get Column facebook Agent Analytics
  server.get(
    `${AGENTFACEBOOKANALYTIC.GET_COLUMN_FACEBOOK_AGENT_ANALYTICS}/:id`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get column facebook analytics',
        description: 'get column facebook analytics api',
        tags: ['FaceBook_Analytics'],
      },
    },
    getCoulumnFacebookAnalyticsHandler
  );

  // TODO:agent facebook inbound trendchart
  server.post(
    AGENTFACEBOOKANALYTIC.AGENT_FACEBOOK_INBOUND_TRENDCHART,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'agent facebook inbound trendchart',
        description: 'agent facebook inbound trendchart api',
        tags: ['FaceBook_Analytics'],
      },
    },
    agentFacebookInboundTrendChartHandler
  );

  // TODO:agent facebook performances trendchart
  server.post(
    AGENTFACEBOOKANALYTIC.AGENT_FACEBOOK_PERFORMANCES_TRENDCHART,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'agent facebook performances trendchart',
        description: 'agent facebook performances trendchart api',
        tags: ['FaceBook_Analytics'],
      },
    },
    agentFacebookPerformancesTrendChartHandler
  );

  // TODO:agent facebook servicelevel trendchart
  server.post(
    AGENTFACEBOOKANALYTIC.AGENT_FACEBOOK_SERVICELEVEL_TRENDCHART,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'agent facebook servicelevel trendchart',
        description: 'agent facebook servicelevel trendchart api',
        tags: ['FaceBook_Analytics'],
      },
    },
    agentFacebookServicelevelTrendChartHandler
  );

  next();
}
