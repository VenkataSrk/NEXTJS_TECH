/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import { BROADCAST_CHAT } from '../../helpers/constants';
import { deleteBroadcastAgentDetails, deleteBroadcastGroupMsg, getAllGroupDetails, getBroadCastAgentDetails, gettBroadcastGroupMsg, insertBroadCastGroup, insertBroadcastGroupMsg } from './handler';

export default function boradCastHandler(
  server: any,
  options: any,
  next: any
) {
  server.post(
    BROADCAST_CHAT.INSERT_BROADCAST_GROUP,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Create broadcast chat',
        description: 'Create broadcast chat api',
        tags: ['Broadcast chat'],
        // body: internalChatSchema.body,
        // response: internalChatSchema.response,
      },
    },
    insertBroadCastGroup
  );

  server.post(
    BROADCAST_CHAT.GET_ALL_GROUP_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get broadcast chat',
        description: 'get broadcast chat api',
        tags: ['Broadcast chat'],
        // body: internalChatSchema.body,
        // response: internalChatSchema.response,
      },
    },
    getAllGroupDetails
  );

  server.post(
    BROADCAST_CHAT.GET_BROADCAST_AGENT_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get broadcast agent chat',
        description: 'get broadcast agent chat api',
        tags: ['Broadcast chat'],
        // body: internalChatSchema.body,
        // response: internalChatSchema.response,
      },
    },
    getBroadCastAgentDetails
  );

  server.post(
    BROADCAST_CHAT.DELETE_BROADCAST_AGENT_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'delete broadcast agent details',
        description: 'delete broadcast agent details api',
        tags: ['Broadcast chat'],
        // body: internalChatSchema.body,
        // response: internalChatSchema.response,
      },
    },
    deleteBroadcastAgentDetails
  );

  server.post(
    BROADCAST_CHAT.INSERT_BROADCAST_GROUP_MSG,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'insert broadcast msg details',
        description: 'insert broadcast msg details api',
        tags: ['Broadcast chat'],
        // body: internalChatSchema.body,
        // response: internalChatSchema.response,
      },
    },
    insertBroadcastGroupMsg
  );

  server.post(
    BROADCAST_CHAT.GET_BROADCAST_GROUP_MSG,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get broadcast msg details',
        description: 'get broadcast msg details api',
        tags: ['Broadcast chat'],
        // body: internalChatSchema.body,
        // response: internalChatSchema.response,
      },
    },
    gettBroadcastGroupMsg
  );

  server.post(
    BROADCAST_CHAT.DELETE_BROADCAST_GROUP_MSG,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'delete broadcast msg details',
        description: 'delete broadcast msg details api',
        tags: ['Broadcast chat'],
        // body: internalChatSchema.body,
        // response: internalChatSchema.response,
      },
    },
    deleteBroadcastGroupMsg
  );

  next();
}
