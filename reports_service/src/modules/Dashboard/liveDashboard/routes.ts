
import { DASHBOARD } from '../../../helpers/constants';
import {
  getAgentAssistanceHandler,
  getArrivalDashbordRateHandler,
  getCallLiveDashboardHandler,
  getCallLiveDashboardPieChartHandler,
  getChatBotLiveDashboard,
  getChatBotLiveDashboardAgentPerformance,
  getChatLiveDashboardHandler,
  getComapreDashboardHandler,
  getCompareDashbaordTrendChartHandler,
  getCustomerChartRate,
  getCustomerCountForYearHandler,
  getCustomerSatisfactionHandler,
  getDashboardAgentStatusHandler,
  getDashboardAverageHandler,
  getDashboardPerformanceComparisonHandler,
  getDashboardPiechartAgentStatusDetails,
  getDashboardSocialMediaStatusHandler,
  getDashboardUtilizationHandler,
  getEmailLiveDashboardPieChartHandler,
  getLiveDashBoardFeautresHandler,
  getOverAllSlaAi,
  getResponseRatingHandler,
  getSentimentalAnalysisHandler,
  getSmsLiveDashboardPieChartHandler,
  getSurveyResponseHandler,
  getTrackerData,
  getUsedNoOfSessionHandler,
  getVideoChannelDashMetrics,
  getWebchatLiveDashboradHandler,

} from './handler';
import { getAgentAssistance, getCallLiveDashbordScheme, getDashBoardReq, getResponseRating, getSurveyResponse } from './schema';

export default function liveDashboardHandler(server: any, options: any, next: any) {
  // TODO:get call to live dashboard handler
  server.post(
    DASHBOARD.GET_CALL_LIVE_DASHBOARD,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get call live dashboard',
        description: 'get call live dashboard api',
        tags: ['liveDashboard'],
        // body: chatReportScheme.body,
        // response: chatReportScheme.response,
      },
    },
    getCallLiveDashboardHandler
  );

  // TODO:get call to live dashboard handler
  server.post(
    DASHBOARD.GET_CHAT_LIVE_DASHBOARD,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get Chat live dashboard',
        description: 'get Chat live dashboard api',
        tags: ['liveDashboard'],
        // body: chatReportScheme.body,
        // response: chatReportScheme.response,
      },
    },
    getChatLiveDashboardHandler
  );

  // TODO:get dashboard call piechart
  server.post(
    DASHBOARD.GET_CALL_LIVE_DASHBOARD_PIECHART,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get call live dashboard piechart',
        description: 'get call live dashboard piechart api',
        tags: ['liveDashboard'],
        body: getCallLiveDashbordScheme.body,
        // response: getCallLiveDashbordScheme.response,
      },
    },
    getCallLiveDashboardPieChartHandler
  );

  // TODO:get dashboard chat piechart
  server.post(
    DASHBOARD.GET_WEBCHAT_LIVE_DASHBOARD,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get chat live dashboard',
        description: 'get chat live dashboard api',
        tags: ['liveDashboard'],
        // body: getCallLiveDashbordScheme.body,
        // response: getCallLiveDashbordScheme.response,
      },
    },
    getWebchatLiveDashboradHandler
  );

  // TODO:get dashboard feautres
  server.post(
    DASHBOARD.GET_LIVE_DASHBOARD_FEAUTRES,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get live dashboard feautres',
        description: 'get live dashboard feautres api',
        tags: ['liveDashboard'],
        body: getCallLiveDashbordScheme.body,
        // response: getCallLiveDashbordScheme.response,
      },
    },
    getLiveDashBoardFeautresHandler
  );

  // TODO:get call arrival dashesboard rate
  server.post(
    DASHBOARD.GET_ARRIVAL_DASHBOARD_RATE,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get call arrival dashesboard rate',
        description: 'get call arrival dashesboard rate api',
        tags: ['liveDashboard'],
        body: getCallLiveDashbordScheme.body,
        // response: getCallLiveDashbordScheme.response,
      },
    },
    getArrivalDashbordRateHandler
  );

  // Live chatBot Dashboard
  server.post(
    DASHBOARD.GET_LIVE_CHATBOT_DASHBOARD,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get live chat bot dashboard',
        description: 'get live chat bot dashboard  api',
        tags: ['liveDashboard'],
      },
    },
    getChatBotLiveDashboard
  );

  server.post(
    DASHBOARD.GET_AGENT_PERFORMANCE_DASHBOARD,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get live chat bot agent performance dashboard',
        description: 'get live chat bot agent performance dashboard  api',
        tags: ['liveDashboard'],
      },
    },
    getChatBotLiveDashboardAgentPerformance
  );
  server.post(
    DASHBOARD.GET_CUSTOMER_CHART_RATE,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get live chat bot customer trend Chart',
        description: 'get live chat bot customer trend Chart  api',
        tags: ['liveDashboard'],
      },
    },
    getCustomerChartRate
  );
  server.post(
    DASHBOARD.GET_SMS_LIVE_DASHBOARD_PIECHART,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get live dashboard sms pie chat',
        description: 'get live dashboard sms pie chat api',
        tags: ['liveDashboard'],
      },
    },
    getSmsLiveDashboardPieChartHandler
  );

  server.post(
    DASHBOARD.GET_EMAIL_LIVE_DASHBOARD_PIECHART,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get live dashboard sms pie chat',
        description: 'get live dashboard sms pie chat api',
        tags: ['liveDashboard'],
      },
    },
    getEmailLiveDashboardPieChartHandler
  );

  server.post(
    DASHBOARD.GET_DASHBOARD_AGENT_STATUS_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get live dashboard sms pie chat',
        description: 'get live dashboard sms pie chat api',
        tags: ['liveDashboard'],
      },
    },
    getDashboardPiechartAgentStatusDetails
  );

  // TODO: agent status details
  server.get(
    DASHBOARD.GET_DASHBOARD_AGENT_STATUS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get agent dashboard status pie chat',
        description: 'get agent dashboard status api',
        tags: ['liveDashboard'],
      },
    },
    getDashboardAgentStatusHandler
  );

  server.post(
    DASHBOARD.GET_DASHBOARD_SOCIAL_MEDIA_STATUS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get live dashboard social media pie chat',
        description: 'get live dashboard social media pie chat api',
        tags: ['liveDashboard'],
      },
    },
    getDashboardSocialMediaStatusHandler
  );

  server.post(
    DASHBOARD.GET_DASHBOARD_AVERAGE,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get live dashboard social media pie chat',
        description: 'get live dashboard social media pie chat api',
        tags: ['liveDashboard'],
      },
    },
    getDashboardAverageHandler
  );

  server.post(
    DASHBOARD.GET_CUSTOMER_SATISFACTION,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get live dashboard social media pie chat',
        description: 'get live dashboard social media pie chat api',
        tags: ['liveDashboard'],
      },
    },
    getCustomerSatisfactionHandler
  );

  server.post(
    DASHBOARD.GET_DASHBOARD_UTILIZATION,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get live dashboard Utilization pie chat',
        description: 'get live dashboard Utilization pie chat api',
        tags: ['liveDashboard'],
      },
    },
    getDashboardUtilizationHandler
  );

  server.post(
    DASHBOARD.GET_DASHBOARD_PERFORMANCE_AGENT_BOT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get live dashboard performance comparison between agent & bot pie chat',
        description: 'get live dashboard performance comparison between agent & bot pie chat api',
        tags: ['liveDashboard'],
        body: getDashBoardReq.body,
        response: getDashBoardReq.response,
      },
    },
    getDashboardPerformanceComparisonHandler
  );

  // TODO: used no of session
  server.post(
    DASHBOARD.GET_USED_NO_OF_SESSION,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get used no of session',
        description: 'get used no of session api',
        tags: ['liveDashboard'],
      },
    },
    getUsedNoOfSessionHandler
  );

  // Response Rating in Live Dashboard
  server.post(
    DASHBOARD.GET_LIVE_DASHBOARD_RESPONSE_RATING,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get live dashboard response rating details',
        description: 'get live dashboard response rating details api',
        tags: ['liveDashboard'],
        body: getResponseRating.body,
        response: getResponseRating.response,
      },
    },
    getResponseRatingHandler
  );
  server.post(
    DASHBOARD.COMPARE_DASHBOARD_INFO,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get compare dashboard details',
        description: 'get compare dashboard details api',
        tags: ['compare'],

      },
    },
    getComapreDashboardHandler
  );
  server.post(
    DASHBOARD.GET_COMPARE_DASHBOAARD_TRENDCHART,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get compare dashboard trend chat',
        description: 'get compare dashboard trend chat api',
        tags: ['compare'],

      },
    },
    getCompareDashbaordTrendChartHandler
  );

  // live dashboard agent assistance
  server.post(
    DASHBOARD.GET_LIVE_DASHBOARD_AGENT_ASSISTANCE,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get live dashboard agent assistance details',
        description: 'get live dashboard agent assistance details api',
        tags: ['liveDashboard'],
        // body: getAgentAssistance.body,
        // response: getAgentAssistance.response,
      },
    },
    getAgentAssistanceHandler
  );
  // live dashboard survey response
  server.post(
    DASHBOARD.GET_LIVE_DASHBOARD_SURVEY_RESPONSE,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get live dashboard survey response details',
        description: 'get live dashboard survey response details api',
        tags: ['liveDashboard'],
        // body: getSurveyResponse.body,
        // response: getSurveyResponse.response,
      },
    },
    getSurveyResponseHandler
  );
  // live dashboard Sentimental Analysis
  server.post(
    DASHBOARD.GET_LIVE_DASHBOARD_SENTIMENTL_ANALYSIS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get live dashboard Sentimental Analysis',
        description: 'get live dashboard Sentimental Analysis api',
        tags: ['liveDashboard'],
        // body: getSurveyResponse.body,
        // response: getSurveyResponse.response,
      },
    },
    getSentimentalAnalysisHandler
  );

  // live dashboard customer count for year
  server.get(
    DASHBOARD.GET_CUSTOMER_COUNT_FOR_YEAR,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get live dashboard customer count for year',
        description: 'get live dashboard customer count for year api',
        tags: ['liveDashboard'],
      },
    },
    getCustomerCountForYearHandler
  );

  // live dashboard video channel dashboard metrics
  server.post(
    DASHBOARD.GET_VIDEO_CHANNEL_DASH_METRICS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'live dashboard video channel dashboard metrics',
        description: 'live dashboard video channel dashboard metrics api',
        tags: ['liveDashboard'],
      },
    },
    getVideoChannelDashMetrics
  );

  // live dashboard get tracker data
  server.get(
    DASHBOARD.GET_TRACKER_DATA,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'live dashboard get tracker',
        description: 'live dashboard get tracker api',
        tags: ['liveDashboard'],
      },
    },
    getTrackerData
  );

  // live dashboard get tracker data
  server.post(
    DASHBOARD.GET_OVERALL_SLA_AI,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'live dashboard get overall sla ai',
        description: 'live dashboard get overall sla ai api',
        tags: ['liveDashboard'],
      },
    },
    getOverAllSlaAi
  );

  next();

}
