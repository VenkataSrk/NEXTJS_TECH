import { CONVERSATION_TRIGGERS } from '../../helpers/constants';
import { deleteConversationTriggerHandler, getConversationTriggersHandler, insertconversationtriggerHandler, loggerDetailsHandler } from './handler';

export default function conversationTriggerHandler(server: any, options: any, next: any) {

  // insert update conversation triggers
  server.post(
    CONVERSATION_TRIGGERS.INSERT_UPDATE_CONVERSATION_TRIGGERS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'insert update conversation triggers ',
        description: 'insert update conversation triggers api',
        tags: ['conversation_triggers'],
      },
    },
    insertconversationtriggerHandler
  );

  // get conversation triggers
  server.get(
    CONVERSATION_TRIGGERS.GET_CONVERSATION_TRIGGERS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get conversation triggers',
        description: 'get conversation triggersapi',
        tags: ['conversation_triggers'],
      },
    },
    getConversationTriggersHandler
  );

  // delete conversation triggers
  server.delete(
    CONVERSATION_TRIGGERS.DELETE_CONVERSATION_TRIGGERS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'delete conversation triggers',
        description: 'delete conversation triggers api',
        tags: ['conversation_triggers'],
      },
    },
    deleteConversationTriggerHandler
  );

  // frontend logger error
  server.post(
    CONVERSATION_TRIGGERS.Frontend_Logger_Details,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'frontend logger error',
        description: 'frontend logger error api',
        tags: ['error_log'],
      },
    },
    loggerDetailsHandler
  );

  next();
}
