/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import { CALL_NOTIFICATION } from '../../helpers/constants';
import { currentTime, getVoicemailDetailsHandle, inCall, registerDetails, removeCallResgister } from './handler';
import { createCallRegisterSchema, inCallSchema } from './schema';

export default function callNotificationHandler(server: any, options: any, next: any) {
  server.post(
    CALL_NOTIFICATION.CALL_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'call register details',
        description: 'call register details',
        tags: ['call'],
        body: createCallRegisterSchema.body,
        response: createCallRegisterSchema.response,
      },
    },
    registerDetails
  );

  server.get(
    CALL_NOTIFICATION.IN_CALL,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'In call',
        description: 'In call register',
        tags: ['call'],
        query: inCallSchema.query,
        response: inCallSchema.response,
      },
    },
    inCall
  );

  server.get(
    CALL_NOTIFICATION.SERVER_TIME,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'In call',
        description: 'In call register',
        tags: ['call'],
      },
    },
    currentTime
  );
  server.post(
    CALL_NOTIFICATION.REMOVE_CALL_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'remove call register details',
        description: 'remove call register details',
        tags: ['call'],
        // body: createCallRegisterSchema.body,
        // response: createCallRegisterSchema.response,
      },
    },
    removeCallResgister
  );

  // get voice mail list
  server.get(
    CALL_NOTIFICATION.GET_VOICEMAIL_OVERALL_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get voice mail details',
        description: 'get voice mail details',
        tags: ['call'],
      },
    },
    getVoicemailDetailsHandle
  );

  next();
}
