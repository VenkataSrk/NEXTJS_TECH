import { OUTBOUNDCAMPAIGNREPORTS } from '../../../helpers/constants';
import {
    agentCampaignmetricsHandler, summaryCampaignmetricsHandler

} from './handler';

export default function outboundcampaignReportsHandler(server: any, options: any, next: any) {
  // TODO:get call to outbound campaign agent report handler
  server.post(
    OUTBOUNDCAMPAIGNREPORTS.GET_AGENT_OUTBOUNDCAMPAIGN_REPORT_METRICS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get agent outbound campaign report',
        description: 'get agent outbound campaign report api',
        tags: ['outboundcampaignreport'],
        // body: chatReportScheme.body,
        // response: chatReportScheme.response,
      },
    },
    agentCampaignmetricsHandler
  );
  // TODO:get call to outbound campaign agent report handler
  server.post(
    OUTBOUNDCAMPAIGNREPORTS.GET_SUMMARY_OUTBOUNDCAMPAIGN_REPORT_METRICS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get agent outbound campaign report',
        description: 'get agent outbound campaign report api',
        tags: ['outboundcampaignreport'],
        // body: chatReportScheme.body,
        // response: chatReportScheme.response,
      },
    },
    summaryCampaignmetricsHandler
  );

  next();

}
