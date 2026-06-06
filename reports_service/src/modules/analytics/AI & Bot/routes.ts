import {
  AGENTAIBOTANALYTIC
} from '../../../helpers/constants';
import {
  agentAnalysisBotInteractionHandler,
  agentAnalysisBotTrendChartHandler,
  agentAnalyticBotNluPerformencesHandler,
  agentAssistAnalysisTrendChartHandler,
  agentAssistBotAnalysisHandler,
  agentAssistEngagementRateTrendChartHandler,
  agentAssistInteractionChannelHandler,
  agentAssistOverriderateHandler,
  agentBotInteractionChannelHandler,
  botNluPerformencesMessageHandler,
  getAgentAnalyticActiveHoursHandler,
  getAgentAnalyticPerformanceComparisonHandler,
  getApiCallsHandler,
  getChatIntentHandler,
  getNluPerformencesTopIntentDistributionHandler,
  insertUpdateChatIntentHandler,
  knowledgbaseAveragebaseResponseperdayHandler,
  knowledgbaseInteractionChannelHandler,
  knowledgebaseUntilizationTrendChartHandler
} from './handler';
import {
  agentAnalysisBotAnalyticsScheme,
  agentAnalysisBotInteractionScheme,
  getAgentAnalyticReq
} from './schema';

export default function aiBotAnalyticsHandler(server: any, options: any, next: any) {

  // TODO: Agent Analysis Bot Interaction Overview
  server.post(
    AGENTAIBOTANALYTIC.AGENT_ANALYTIC_BOT_INTERACTIONS_OVERVIEW,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Agent Analysis Bot Interaction',
        description: 'Agent Analysis Bot Interaction',
        tags: ['AI_BOT_Analytics'],
        body: agentAnalysisBotInteractionScheme.body,
        response: agentAnalysisBotInteractionScheme.response,
      },
    },
    agentAnalysisBotInteractionHandler
  );

  // TODO: Agent Analysis Bot TrendChart
  server.post(
    AGENTAIBOTANALYTIC.AGENT_ANALYTIC_BOT_TREND_CHART,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Agent Analysis Bot TrendChart',
        description: 'Agent Analysis Bot TrendChart',
        tags: ['AI_BOT_Analytics'],
        // body: agentAnalysisBotTrendChartScheme.body,
        // response: agentAnalysisBotTrendChartScheme.response,
      },
    },
    agentAnalysisBotTrendChartHandler
  );

  // TODO: Agent Bot Interaction channel piechart
  server.post(
    AGENTAIBOTANALYTIC.AGENT_BOT_INTERACTION_CHANNEL,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Agent  Bot Interaction channel',
        description: 'Agent  Bot Interaction channel api',
        tags: ['AI_BOT_Analytics'],
      },
    },
    agentBotInteractionChannelHandler
  );

  // TODO: Agent Analytic Performance Agent Agent_Bot
  server.post(
    AGENTAIBOTANALYTIC.AGENT_ANALYTIC_PERFORMANCE_AGENT_BOT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Agent Analytic performance comparison between agent & bot pie chat',
        description: 'Agent Analytic performance comparison between agent & bot pie chat api',
        tags: ['AI_BOT_Analytics'],
        body: getAgentAnalyticReq.body,
        response: getAgentAnalyticReq.response,
      },
    },
    getAgentAnalyticPerformanceComparisonHandler
  );

  // TODO: Agent Analytic Active Hours
  server.post(
    AGENTAIBOTANALYTIC.AGENT_ANALYTIC_ACTIVE_HOURS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Agent Analytic Active Hours Graph',
        description: 'Agent Analytic Active Hours Graph api',
        tags: ['AI_BOT_Analytics'],
      },
    },
    getAgentAnalyticActiveHoursHandler
  );

  // TODO: Agent Assist Bot Analytics
  server.post(
    AGENTAIBOTANALYTIC.AGENT_ASSIST_BOT_ANALYTICS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Agent Assist Bot Analytics',
        description: 'Agent Assist Bot Analytics Api',
        tags: ['AI_BOT_Analytics'],
        // body: agentAnalysisBotAnalyticsScheme.body,
        // response: agentAnalysisBotAnalyticsScheme.response,
      },
    },
    agentAssistBotAnalysisHandler
  );

  // TODO: Agent Assist Analysis TrendChart
  server.post(
    AGENTAIBOTANALYTIC.AGENT_ASSIST_ANALYTICS_TREND_CHART,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Agent Assist Analysis Bot TrendChart',
        description: 'Agent Assist Analysis Bot TrendChart',
        tags: ['AI_BOT_Analytics'],
      },
    },
    agentAssistAnalysisTrendChartHandler
  );

  // TODO: Agent Assist Interaction channel piechart
  server.post(
    AGENTAIBOTANALYTIC.AGENT_ASSIST_INTERACTION_CHANNEL,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Agent Assist Interaction channel',
        description: 'Agent Assist Interaction channel api',
        tags: ['AI_BOT_Analytics'],
      },
    },
    agentAssistInteractionChannelHandler
  );

  // TODO: Agent Assist EngagementRate trend chart
  server.post(
    AGENTAIBOTANALYTIC.AGENT_ASSIST_ENGAGEMENTRATE_TREND_CHART,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Agent Assist EngagementRate trend chart',
        description: 'Agent Assist EngagementRate trend chart api',
        tags: ['AI_BOT_Analytics'],
      },
    },
    agentAssistEngagementRateTrendChartHandler
  );

  // TODO: Agent Assist overriderate info
  server.post(
    AGENTAIBOTANALYTIC.AGENT_ASSIST_OVERRIDERATE_INFO,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Agent Assist  overriderate info',
        description: 'Agent Assist  overriderate info api',
        tags: ['AI_BOT_Analytics'],
      },
    },
    agentAssistOverriderateHandler
  );

  // TODO: knowledgebase interaction channel info
  server.post(
    AGENTAIBOTANALYTIC.KNOWLEDGEBASE_INTERACTION_CHANNEL,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'knowledgebase interaction channel info',
        description: 'knowledgebase interaction channel info api',
        tags: ['AI_BOT_Analytics'],
      },
    },
    knowledgbaseInteractionChannelHandler
  );

  // TODO: knowledgebase averagebase responseperday info
  server.post(
    AGENTAIBOTANALYTIC.KNOWLEDGEBASE_AVERAGEBASE_RESPONSEPERDAY,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'knowledgebase averagebase responseperday info',
        description: 'knowledgebase averagebase responseperday info api',
        tags: ['AI_BOT_Analytics'],
      },
    },
    knowledgbaseAveragebaseResponseperdayHandler
  );

  // TODO: knowledgebase untilization trendchart
  server.post(
    AGENTAIBOTANALYTIC.KNOWLEDGEBASE_UTILIZATION_TRENDCHART,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'knowledgebase untilization trendchart info',
        description: 'knowledgebase untilization trendchartinfo api',
        tags: ['AI_BOT_Analytics'],
      },
    },
    knowledgebaseUntilizationTrendChartHandler
  );

  // TODO: Agent Analytic Bot NluPerformences
  server.post(
    AGENTAIBOTANALYTIC.AGENT_ANALYTIC_BOT_NLUPERFORMENCES,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Agent Analytic Bot Nlu Performences info',
        description: 'Agent Analytic Bot Nlu Performences info api',
        tags: ['AI_BOT_Analytics'],
      },
    },
    agentAnalyticBotNluPerformencesHandler
  );

  // TODO: Agent Bot NluPerformences Helpful Message
  server.post(
    AGENTAIBOTANALYTIC.BOT_NLU_PERFORMENCES_HELPFUL_MESSAGE,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Bot NluPerformences Helpful Message info',
        description: 'Bot NluPerformences Helpful Message info api',
        tags: ['AI_BOT_Analytics'],
      },
    },
    botNluPerformencesMessageHandler
  );

  // TODO:insert update chat intent
  server.post(
    AGENTAIBOTANALYTIC.INSERT_UPDATE_CHAT_INTENT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'insert update chat intent',
        description: 'insert update chat intent api',
        tags: ['AI_BOT_Analytics'],
      },
    },
    insertUpdateChatIntentHandler
  );

  // TODO:get chat intent
  server.get(
    AGENTAIBOTANALYTIC.GET_CHAT_INTENT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get chat intent',
        description: 'get chat intent api',
        tags: ['AI_BOT_Analytics'],
      },
    },
    getChatIntentHandler
  );

  // TODO:get analytics NLUPerformences Top Intent Distribution
  server.get(
    AGENTAIBOTANALYTIC.NLU_PERFORMENCES_TOP_INTENT_DISTRIBUTION,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get analytics NLUPerformences Top Intent Distribution',
        description: 'get analytics NLUPerformences Top Intent Distribution api',
        tags: ['AI_BOT_Analytics'],
      },
    },
    getNluPerformencesTopIntentDistributionHandler
  );

  // TODO:get api calls info
  server.get(
    AGENTAIBOTANALYTIC.GET_API_CALLS_INFO,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get api calls info',
        description: 'get api calls info api',
        tags: ['AI_BOT_Analytics'],
      },
    },
    getApiCallsHandler
  );

  next();

}
