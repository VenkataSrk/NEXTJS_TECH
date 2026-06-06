/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import { SMS_ENDPOINTS } from '../../constants/sms';

import {
  handleAssignCallbackSms,
  handleOnboardUpdateSmsBotStatus,
  handleUpdateSmsBotStatus,
  insertInboundSmsHandler,
  sendCaasSmsHandler,
  sendSmsOutboundCompanyHandler,
  sendSmsOutboundHandler,
  updateDispositionSmsHandler,
  updateReplyviaDispositionSmsHandler,
} from './handler';
import { sendSmsSchema } from './schema';

export default function smsHandler(server: any, options: any, next: any) {

  // Get callflow details
  server.post(
    SMS_ENDPOINTS.INSERT_INBOUND,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'Insert SMS inbound',
        description: 'Handles inbound SMS and triggers bot/agent assignment',
        tags: ['sms'],
      },
    },
    insertInboundSmsHandler
  );

  // update bot status
  server.post(
    SMS_ENDPOINTS.UPDATE_BOT_STATUS,
    {
      preValidation: [server.validateSession],
      schema: {
        description: 'Update SMS bot status',
        tags: ['sms'],
      },
    },
    handleUpdateSmsBotStatus
  );

  server.post(
    SMS_ENDPOINTS.ONBOARD_UPDATE_BOT_STATUS,
    {
      preValidation: [server.validateSession],
      schema: {
        description: 'Onboard update SMS bot status',
        tags: ['sms'],
      },
    },
    handleOnboardUpdateSmsBotStatus
  );

  // assign callback sms
  server.post(
    SMS_ENDPOINTS.ASSIGN_CALLBACK,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Assign callback SMS',
        description: 'API to assign callback for SMS',
        tags: ['sms'],
      },
    },
    handleAssignCallbackSms
  );

  server.put(
    `${SMS_ENDPOINTS.UPDATE_DISPOSITION}/:SMSHistoryId`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Update disposition SMS',
        description: 'Update SMS disposition and trigger sentiment analysis',
        tags: ['sms'],
      },
    },
    updateDispositionSmsHandler
  );

  server.post(
    SMS_ENDPOINTS.CCAAS_OUT,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'send sms',
        description: 'send a new sms',
        tags: ['sms'],
        body: sendSmsSchema.body,
        response: sendSmsSchema.response,
      },
    },
    sendCaasSmsHandler
  );

  server.post(
    SMS_ENDPOINTS.SEND_OUTBOUND,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'send sms outbound',
        description: 'send sms outbound',
        tags: ['sms'],
      },
    },
    sendSmsOutboundHandler
  );

  server.post(
    SMS_ENDPOINTS.SEND_OUTBOUND_COMPANY,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'send sms outbound company',
        description: 'send sms company',
        tags: ['sms'],
      },
    },
    sendSmsOutboundCompanyHandler
  );

  server.put(
    `${SMS_ENDPOINTS.UPDATE_REPLYVIA_DISPOSITION}`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update replyvia  disposition sms',
        description: 'update replyvia disposition sms api',
        tags: ['sms'],
        // params: updateDispositionCallSchema.params,
        // body: updateDispositionCallSchema.body,
        // response: updateDispositionCallSchema.response,
      },
    },
    updateReplyviaDispositionSmsHandler
  );
  next();
}
