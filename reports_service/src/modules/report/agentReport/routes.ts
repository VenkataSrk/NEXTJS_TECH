import { AGENTREPORT } from '../../../helpers/constants';
import {
  getAgentCallSummmaryReportHandler,
  getAgentFeedbackReportHandler,
  getAgentLoginAndLogoutReportHandler,
  getAgentPerformanceHandler,
  getAgentStatusReportReportHandler,
  getCsatAgentFeedbackReportHandler,
  getParticularAgentReportHandler,
  getQueryHandlingTimeReportHandler
} from './hadler';

export default function agentReportHandler(server: any, options: any, next: any) {

  server.post(
    AGENTREPORT.GET_AGENT_CALL_SUMMARY_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get agent call summary',
        description: 'get agent call summary',
        tags: ['agentReport'],
        // body: voiceReportScheme.body,
        // response: userinsertSchema.response,
      },
    },
    getAgentCallSummmaryReportHandler
  );

  server.post(
    AGENTREPORT.GET_AGENT_PERFORMANCE,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get agent call summary',
        description: 'get agent call summary',
        tags: ['agentReport'],
        // body: voiceReportScheme.body,
        // response: userinsertSchema.response,
      },
    },
    getAgentPerformanceHandler
  );

  server.post(
    AGENTREPORT.GET_LOGIN_AND_LOGOUT_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get agent login and logout report',
        description: 'get agent login and logout report',
        tags: ['agentReport'],
        // body: voiceReportScheme.body,
        // response: userinsertSchema.response,
      },
    },
    getAgentLoginAndLogoutReportHandler
  );

  server.post(
    AGENTREPORT.GET_QUERY_HANDING_TIME_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get Query Handling Time Report',
        description: 'get Query Handling Time Report',
        tags: ['agentReport'],
        // body: voiceReportScheme.body,
        // response: userinsertSchema.response,
      },
    },
    getQueryHandlingTimeReportHandler
  );

  server.post(
    AGENTREPORT.GET_AGENT_STATUS_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get Agent Status Report',
        description: 'get Agent Status Report',
        tags: ['agentReport'],
        // body: voiceReportScheme.body,
        // response: userinsertSchema.response,
      },
    },
    getAgentStatusReportReportHandler
  );

  server.post(
    AGENTREPORT.GET_AGENT_FEEDBACK_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get Agent Feedback Report',
        description: 'get Agent Feedback Report',
        tags: ['agentReport'],
        // body: voiceReportScheme.body,
        // response: userinsertSchema.response,
      },
    },
    getAgentFeedbackReportHandler
  );

  // get csat agent feedback report
  server.post(
    AGENTREPORT.GET_CSAT_AGENT_FEEDBACK_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get csat agent feedback report',
        description: 'get csat agent feedback report api',
        tags: ['agentReport'],
        // body: voiceReportScheme.body,
        // response: userinsertSchema.response,
      },
    },
    getCsatAgentFeedbackReportHandler
  );

    // get csat agent particular report
  server.post(
    AGENTREPORT.GET_PARTICULAR_AGENT_STATUS_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get csat particular agent  report',
        description: 'get csat particular agent report api',
        tags: ['agentReport'],
          // body: voiceReportScheme.body,
          // response: userinsertSchema.response,
      },
    },
    getParticularAgentReportHandler
    );

  next();
}
