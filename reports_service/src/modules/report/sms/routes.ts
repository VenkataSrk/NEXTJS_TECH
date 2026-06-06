import { SMS } from '../../../helpers/constants';
import {
  getSmsAgentReportHandler,
  getSmsInterationReport,
  getSmsProfileReportHandler
} from './handler';

export default function smsHandler(server: any, options: any, next: any) {

  // 3rd
  server.post(
    SMS.GET_SMS_PROFILE_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get sms profile summary',
        description: 'get sms profile summary api',
        tags: ['smsReport'],
      },
    },
    getSmsProfileReportHandler
  );

  // 4th
  server.post(
    SMS.GET_SMS_AGENT_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get sms agent profile summary',
        description: 'get sms agent profile summary api',
        tags: ['smsReport'],
      },
    },
    getSmsAgentReportHandler
  );

  // 5th
  server.post(
    SMS.GET_SMS_INTERATION_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get sms interation profile summary',
        description: 'get sms interation profile summary api',
        tags: ['smsReport'],
      },
    },
    getSmsInterationReport
  );

  next();
}
