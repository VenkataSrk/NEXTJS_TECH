import { EMAIL } from '../../../helpers/constants';
import {
  getEmailAgentReportHandler,
  getEmailInterationReport,
  getEmailReportHandler,
} from './handler';

export default function emailHandler(server: any, options: any, next: any) {
  // 3rd
  server.post(
    EMAIL.GET_EMAIL_PROFILE_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get email profile summary',
        description: 'get email profile summary api',
        tags: ['emailReport'],
      },
    },
    getEmailReportHandler
  );

  // 4th
  server.post(
    EMAIL.GET_EMAIL_AGENT_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get email agent profile summary',
        description: 'get email agent profile summary api',
        tags: ['emailReport'],
      },
    },
    getEmailAgentReportHandler
  );

  // 5th
  server.post(
    EMAIL.GET_EMAIL_INTERATION_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get email interation profile summary',
        description: 'get email interation profile summary api',
        tags: ['emailReport'],
      },
    },
    getEmailInterationReport
  );

  next();
}
