import { OUTBOUNDCAMPAIGNDASHBOARD } from '../../../helpers/constants';
import {
    callblastDashboardChartinfoHandler, callblastDashboardinfoHandler, callblastLeaderboardChartinfoHandler

} from './handler';

export default function outboundcampaignDashboardinfoHandler(server: any, options: any, next: any) {
  // TODO:get call to live dashboard handler
  server.post(
    OUTBOUNDCAMPAIGNDASHBOARD.GET_CALLBLAST_DASHBOARD_INFO,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get outbound campaign dashboard',
        description: 'get outbound campaign dashboard api',
        tags: ['outboundcampaigndashboard'],
        // body: chatReportScheme.body,
        // response: chatReportScheme.response,
      },
    },
    callblastDashboardinfoHandler
  );
    // TODO:get call to live dashboard handler
  server.post(
    OUTBOUNDCAMPAIGNDASHBOARD.GET_CALLBLAST_CHART,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get outbound campaign dashboard Chart',
        description: 'get outbound campaign dashboard Chart api',
        tags: ['outboundcampaigndashboard'],
          // body: chatReportScheme.body,
          // response: chatReportScheme.response,
      },
    },
    callblastDashboardChartinfoHandler
  );

      // TODO:get call to live dashboard handler
  server.post(
    OUTBOUNDCAMPAIGNDASHBOARD.GET_CALLBLAST_LEADERBOARD_INFO,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get outbound campaign Leaderboard Chart',
        description: 'get outbound campaign Leaderboard Chart api',
        tags: ['outboundcampaigndashboard'],
              // body: chatReportScheme.body,
              // response: chatReportScheme.response,
      },
    },
    callblastLeaderboardChartinfoHandler
  );
  next();

}
