import { DASHBOARD } from '../../../helpers/constants';
import {  getAgentActivityDetailsHandler, getAgentActivityListDetailsHandler, getAgentReportDashboardListHandler, getAgentReportRecentListHandler, getDispositionForChannelstHandler, getLeaderBoardDashboardHandler, getLeaderBoardDashboardListHandler } from './handler';

export default function leaderboardRoute(server: any, options: any, next: any) {

  server.post(
    DASHBOARD.GET_LEADERBOARD_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get compare dashboard',
        description: 'get compare dashboard api',
        tags: ['compareDashboard'],
        // body: chatReportScheme.body,
        // response: chatReportScheme.response,
      },
    },
    getLeaderBoardDashboardHandler
  );

  server.post(
    DASHBOARD.GET_AGENT_ACTIVITY_LIST_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get agent activity list',
        description: 'get agent activity list api',
        tags: ['compareDashboard'],
        // body: chatReportScheme.body,
        // response: chatReportScheme.response,
      },
    },
    getAgentActivityListDetailsHandler
  );

  server.post(
    DASHBOARD.GET_AGENT_ACTIVITY_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get agent activity list',
        description: 'get agent activity list api',
        tags: ['compareDashboard'],
        // body: chatReportScheme.body,
        // response: chatReportScheme.response,
      },
    },
    getAgentActivityDetailsHandler
  );

  server.get(
    DASHBOARD.GET_LEADERBOARD_DASHBOARD_LIST,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get leaderboard dashboard details',
        description: 'get leaderboard dashboard details api',
        tags: ['leaderBoard'],
      },
    },
    getLeaderBoardDashboardListHandler
  );

  server.post(
    DASHBOARD.GET_AGENTREPORT_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get Agent Report dashboard details',
        description: 'get Agent Report dashboard details api',
        tags: ['agentReport'],
      },
    },
    getAgentReportDashboardListHandler
  );

  server.post(
    DASHBOARD.GET_AGENT_RECENT_ANSWERED_INFO,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get Agent Report Recent Answered Info dashboard details',
        description: 'get Agent Report Recent Answered Info dashboard details api',
        tags: ['agentReport'],
      },
    },
    getAgentReportRecentListHandler
  );

  server.post(
    DASHBOARD.GET_DISPOSITION_STATUS_CHANNEL,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get disposition details for channels dashboard details',
        description: 'get disposition details for channels dashboard details api',
        tags: ['agentReport'],
      },
    },
    getDispositionForChannelstHandler
  );

  next();
}
