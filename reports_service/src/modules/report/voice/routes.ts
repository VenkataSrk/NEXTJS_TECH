/**
 * @createdBy <krishnakumar.s@worktual.com>
 * @createdOn <05/04/2023>
 */

import { REPORTVOICE } from '../../../helpers/constants';
import {
  editReportColumn,
  getAnsweredCallReportHandler,
  getCallBackSummaryHandler,
  getEditReportColumn,
  getTranferCallReportHandler,
  inboundCallReportHandler,
  interActionReportHandler,
  missedCallReportHandler,
  outboundCallReportHandler
} from './handler';
import {
  voiceReportScheme
} from './schema';

export default function reportHandler(server: any, options: any, next: any) {
  // TODO:get answers call profile report
  server.post(
    REPORTVOICE.GET_ANSWERED_CALL_PROFILE_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get answers call profile summary',
        description: 'get answers call profile summary api',
        tags: ['voiceReport'],
        // body: voiceReportScheme.body,
        // response: userinsertSchema.response,
      },
    },
    getAnsweredCallReportHandler
  );

  // TODO:missed call list report
  server.post(
    REPORTVOICE.MISSED_CALL_LIST_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get missed call report',
        description: 'get missed call report api',
        tags: ['voiceReport'],
        // body: voiceReportScheme.body,
        // response: userinsertSchema.response,
      },
    },
    missedCallReportHandler
  );
  // TODO: interaction list report
  server.post(
    REPORTVOICE.INTERACTION_LIST_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get interaction list report',
        description: 'get interaction list report api',
        tags: ['voiceReport'],
        // body: voiceReportScheme.body,
        // response: userinsertSchema.response,
      },
    },
    interActionReportHandler
  );

  // TODO:edit column report
  server.post(
    REPORTVOICE.EDIT_COLUMN_FOR_REPORTS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'store reports column values',
        description: 'store reports column values api',
        tags: ['voiceReport'],
      },
    },
    editReportColumn
  );

  // get edit column report
  server.post(
    REPORTVOICE.GET_EDIT_COLUMN_LIST,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get reports column values',
        description: 'get reports column values api',
        tags: ['voiceReport'],
      },
    },
    getEditReportColumn
  );

  // get outbout call report
  server.post(
    REPORTVOICE.OUTBOUND_CALL_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get outbout call report',
        description: 'get outbout call report api',
        tags: ['voiceReport'],
        // body: voiceReportScheme.body,
        // response: userinsertSchema.response,
      },
    },
    outboundCallReportHandler
  );

  // TODO: transfer call report
  server.post(
    REPORTVOICE.GET_TRANSFER_CALL_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get transfer report',
        description: 'get transfer list report api',
        tags: ['voiceReport'],
        // body: voiceReportScheme.body,
        // response: userinsertSchema.response,
      },
    },
    getTranferCallReportHandler
  );

  server.post(
    REPORTVOICE.GET_CALL_BACK_SUMMARY_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get call back summary report',
        description: 'get call back summary report api',
        tags: ['voiceReport'],
        // body: voiceReportScheme.body,
        // response: userinsertSchema.response,
      },
    },
    getCallBackSummaryHandler
  );

    // get inbound call report
  server.post(
    REPORTVOICE.INBOUND_CALL_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get inbound call report',
        description: 'get inbound call report api',
        tags: ['voiceReport'],
          // body: voiceReportScheme.body,
          // response: userinsertSchema.response,
      },
    },
    inboundCallReportHandler
    );
  next();
}
