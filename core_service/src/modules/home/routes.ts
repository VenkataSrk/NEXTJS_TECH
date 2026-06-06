import { HOME } from '../../helpers/constants';
import { deleteHomeChatHistory, getHomeChatHistory, insertUpdateHomeChatHistory } from './handler';

export default function home_routes(server: any, options: any, next: any) {
  server.post(
    HOME.INSERT_UPDATE_HOME_CHAT_HISTORY,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Insert update home chat history api',
        description: 'Insert update home chat history api',
        tags: ['Home']
      },
    },
    insertUpdateHomeChatHistory
  );

  server.post(
    HOME.GET_HOME_CHAT_HISTORY,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get home chat history api',
        description: 'Get home chat history api',
        tags: ['Home']
      },
    },
    getHomeChatHistory
  );

  server.delete(
    `${HOME.DELETE_HOME_CHAT_HISTORY}/:sessionId`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Delete home chat history api',
        description: 'Delete home chat history api',
        tags: ['Home']
      },
    },
    deleteHomeChatHistory
  );

  next();
}
