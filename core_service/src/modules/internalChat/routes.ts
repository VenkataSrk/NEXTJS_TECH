/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import { INTERNAL_CHAT } from '../../helpers/constants';
import { createChat, deleteChat, edit, getChat, getInternalChatByHistoryId, getParticularChat, markAllAsRead } from './handler';
import { deleteInternalChatSchema, editInternalChatSchema, getInternalChatByHistoryIdSchema, getInternalChatSchema, getParticularInternalChatSchema, internalChatSchema, markAllInternalChatSchema } from './schema';

export default function internalChatHandler(
  server: any,
  options: any,
  next: any
) {
  server.post(
    INTERNAL_CHAT.CREATE,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Create internal chat',
        description: 'Create internal chat api',
        tags: ['Internal chat'],
        body: internalChatSchema.body,
        response: internalChatSchema.response,
      },
    },
    createChat
  );

  server.get(
    INTERNAL_CHAT.GET_CHAT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get internal chat',
        description: 'Get internal chat api',
        tags: ['Internal chat'],
        query: getInternalChatSchema.query,
        response: getInternalChatSchema.response,
      },
    },
    getChat
  );

  server.get(
    INTERNAL_CHAT.GET_PARTICULAR_CHAT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get particular internal chat',
        description: 'Get particular internal chat api',
        tags: ['Internal chat'],
        query: getParticularInternalChatSchema.query,
        response: getParticularInternalChatSchema.response,
      },
    },
    getParticularChat
  );

  server.post(
    INTERNAL_CHAT.EDIT_CHAT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Edit internal chat',
        description: 'Edit internal chat api',
        tags: ['Internal chat'],
        body: editInternalChatSchema.body,
        response: editInternalChatSchema.response,
      },
    },
    edit
  );

  server.post(
    INTERNAL_CHAT.DELETE_CHAT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Delete internal chat',
        description: 'Delete internal chat api',
        tags: ['Internal chat'],
        body: deleteInternalChatSchema.body,
        response: deleteInternalChatSchema.response,
      },
    },
    deleteChat
  );

  server.post(
    INTERNAL_CHAT.MARK_ALL_AS_READ_CHAT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Mark as read internal chat',
        description: 'Mark as read internal chat api',
        tags: ['Internal chat'],
        body: markAllInternalChatSchema.body,
        response: markAllInternalChatSchema.response,
      },
    },
    markAllAsRead
  );

  server.post(
    INTERNAL_CHAT.GET_CHAT_HISTORY_ID,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get internal chat by history id',
        description: 'Get internal chat by history id api',
        tags: ['Internal chat'],
        // body: getInternalChatByHistoryIdSchema.body,
        // response: getInternalChatByHistoryIdSchema.response,
      },
    },
    getInternalChatByHistoryId
  );
  next();
}
