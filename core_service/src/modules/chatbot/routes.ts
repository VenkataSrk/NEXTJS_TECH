/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import { CHATBOT } from '../../helpers/constants';
import { insertUpdateVistorHandler } from './handler';

export default function ChatBotRouter(server: any, options: any, next: any) {

  server.post(
    CHATBOT.INSERT_UPDATE_INSIGHT_VISITORS,
    {
      schema: {
        summary: 'insert and update the webchat insight vistor',
        description: 'insert and update the webchat insight vistor api',
        tags: ['VistorInsight'],
      },
    },
    insertUpdateVistorHandler
      );
  next();
}
