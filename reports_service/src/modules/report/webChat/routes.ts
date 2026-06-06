
import { REPORTCHAT } from '../../../helpers/constants';
import {
  getACWChatReportHandler,
  getAgentChatReportHandler,
  getAgentQueueChatReportHandler,
  getChatACWAgentReportHandler,
  getChatAgentReportHandler,
  getChatInteractionReportHandler,
  getChatProfileReportHandler,
  getChatSessionReportHandler
 } from './handler';
import { chatReportScheme, getAgentChatScheme } from './schema';

export default function reportChatHandler(server: any, options: any, next: any) {
  server.post(
    REPORTCHAT.GET_CHAT_REPORTS_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get chat session history',
        description: 'get chat session history api',
        tags: ['chatReport'],
        // body: chatReportScheme.body,
        // response: chatReportScheme.response,
      },
    },
    getChatSessionReportHandler
  );

  server.post(
    REPORTCHAT.GET_AGENT_CHAT_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get agent chat history',
        description: 'get agent chat history api',
        tags: ['chatReport'],
        // body: getAgentChatScheme.body,
        // response: getAgentChatScheme.response,
      },
    },
    getAgentChatReportHandler
  );

  server.post(
    REPORTCHAT.GET_ACW_CHAT_DETAILS, {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get acw chat report history',
        description: 'get acw chat report api',
        tags: ['chatReport'],
       // body: getAgentChatScheme.body,
        // response: getAgentChatScheme.response,
      },
    },
    getACWChatReportHandler
  );

  server.post(
    REPORTCHAT.GET_AGENT_QUEUE_CHAT_DETAILS, {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get queue chat report history',
        description: 'get queue chat report api',
        tags: ['chatReport'],
       // body: getAgentChatScheme.body,
        // response: getAgentChatScheme.response,
      },
    },
    getAgentQueueChatReportHandler
  );

  server.post(
    REPORTCHAT.GET_CHAT_INTERACTION_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get chat interaction report history',
        description: 'get chat interaction report api',
        tags: ['chatReport'],
        // body: getAgentChatScheme.body,
        // response: getAgentChatScheme.response,
      },
    },
    getChatInteractionReportHandler
  );

  server.post(
    REPORTCHAT.GET_CHAT_PROFILE_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get chat profile report history',
        description: 'get chat profile report api',
        tags: ['chatReport'],
      },
    },
    getChatProfileReportHandler
  );

  server.post(
    REPORTCHAT.GET_CHAT_AGENT_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get chat profile report history',
        description: 'get chat profile report api',
        tags: ['chatReport'],
      },
    },
    getChatAgentReportHandler
  );

  server.post(
    REPORTCHAT.GET_CHAT_AGENT_ACW_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get chat acw profile report history',
        description: 'get chat acw profile report api',
        tags: ['chatReport'],
      },
    },
    getChatACWAgentReportHandler
  );
  next();
}
