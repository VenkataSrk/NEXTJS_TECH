import { STANDALONEAGENT } from '../../../helpers/constants';
import {
  getAgentACWReport,
  getAgentBotReport,
  getAgentChatReport,
  getChatRportHandler
} from './hadler';

export default function standAloneReportHandler(server: any, options: any, next: any) {

  server.post(
    STANDALONEAGENT.GET_CHAT_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get standalone chat report',
        description: 'get standalone chat report',
        tags: ['standAloneagentReport'],
        // body: voiceReportScheme.body,
        // response: userinsertSchema.response,
      },
    },
    getChatRportHandler
  );
  server.post(
    STANDALONEAGENT.GET_AGENT_BOT_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get standalone chat report',
        description: 'get standalone chat report',
        tags: ['standAloneagentReport'],
        // body: voiceReportScheme.body,
        // response: userinsertSchema.response,
      },
    },
    getAgentBotReport
  );
  server.post(
    STANDALONEAGENT.GET_AGENT_CHAT_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get standalone chat bot report',
        description: 'get standalone chat bot report',
        tags: ['standAloneagentReport'],
        // body: voiceReportScheme.body,
        // response: userinsertSchema.response,
      },
    },
    getAgentChatReport
  );
  server.post(
    STANDALONEAGENT.GET_AGENT_ACW_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get standalone chat bot report',
        description: 'get standalone chat bot report',
        tags: ['standAloneagentReport'],
        // body: voiceReportScheme.body,
        // response: userinsertSchema.response,
      },
    },
    getAgentACWReport
  );
  next();
}
