import { OUTBPUNDCAMPAIGN } from '../../../src/helpers/constants';
import { getOutboundCampaignDraftChats, insertUpdateOutboundCampaign } from './handler';

export default function outboundcampaign_routes(server: any, options: any, next: any) {
  server.post(
    OUTBPUNDCAMPAIGN.INSERT_UPDATE_OUTBOUND_CAMPAIGN,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Insert update outbound campaign api',
        description: 'Insert update outbound campaign api',
        tags: ['OutboundCampiagn']
        // body: getBillServicePlanSchema.body,
        // response: getBillServicePlanSchema.response
      },
    },
    insertUpdateOutboundCampaign
  );

  server.post(
    OUTBPUNDCAMPAIGN.GET_OUTBOUND_CAMPAIGN_DRAFT_CHAT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get outbound campaign draft chat api',
        description: 'get outbound campaign draft chat api',
        tags: ['OutboundCampiagn']
      // body: getBillServicePlanSchema.body,
      // response: getBillServicePlanSchema.response
      },
    },
    getOutboundCampaignDraftChats
  );
  next();
}
