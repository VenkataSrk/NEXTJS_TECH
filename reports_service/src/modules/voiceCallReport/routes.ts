/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

// import { VOICECALL } from '../../helpers/constants';
// import { getAbandonedCallProfileReportHandler, getIntervalReportHandler, getQueueDispositionStatausHandler, getQueueInboundCallHandler } from './handler';
export default async function voiceCallReportHandler(server: any, options: any, next: any) {
  // Queue Metrics Report for Inbound Call
  // server.post(
  //   VOICECALL.GET_QUEUE_BASE_INBOUND_CALL,
  //   {
  //     preValidation: [server.validateSession],
  //     schema: {
  //       summary: 'queue metrics report for inbound call',
  //       description: 'queue metrics report for inbound call api',
  //       tags: ['voiceReport'],
  //       // body: userinsertSchema.body,
  //       // response: userinsertSchema.response,
  //     },
  //   },
  //   //  getQueueInboundCallHandler
  // );

  // // Queue Disposition Status report
  // server.post(
  //   VOICECALL.GET_DISPOSITION_STATUS,
  //   {
  //     preValidation: [server.validateSession],
  //     schema: {
  //       summary: 'queue disposition Status report',
  //       description: 'queue disposition Status report api',
  //       tags: ['voiceReport'],
  //       // body: userinsertSchema.body,
  //       // response: userinsertSchema.response,
  //     },
  //   },
  //   // getQueueDispositionStatausHandler
  // );

  // // Queue Metrics Interval report
  // server.post(
  //   VOICECALL.GET_INTERVAL_REPORT,
  //   {
  //     preValidation: [server.validateSession],
  //     schema: {
  //       summary: 'queue interval report',
  //       description: 'queue interval report api',
  //       tags: ['voiceReport'],
  //       // body: userinsertSchema.body,
  //       // response: userinsertSchema.response,
  //     },
  //   },
  //   //getIntervalReportHandler
  // );

  // // Abandoned Call Profile Report
  // server.post(
  //   VOICECALL.ABANDONED_CALL_PROFILE,
  //   {
  //     preValidation: [server.validateSession],
  //     schema: {
  //       summary: 'Abandoned Call Profile Report',
  //       description: 'Abandoned Call Profile Report api',
  //       tags: ['voiceReport'],
  //       // body: userinsertSchema.body,
  //       // response: userinsertSchema.response,
  //     },
  //   },
  //   //getAbandonedCallProfileReportHandler
  // );
  next();
}
