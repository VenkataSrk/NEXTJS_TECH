/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import { BOT } from '../../helpers/constants';
import {
  createBotIntentHandler,
  getBotDefaultImages,
  getBotIntentHandler,
  getFallbackMessageHandler,
  insertDefaultBotImages,
  insertDefaultFallbackMessage,
  insertFallbackMessageHandler,
  updateBotTemplateList,
} from './handler';

export default function botHandler(server: any, options: any, next: any) {
  server.post(
    BOT.CREATE_BOT_INTENT,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'create bot intent',
        description: 'create bot intent api',
        tags: ['bot'],
      },
    },
    createBotIntentHandler
  );
  server.get(
    `${BOT.GET_BOT_INTENT}/:bid`,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'get bot intent',
        description: 'get bot intent api',
        tags: ['bot'],
      },
    },
    getBotIntentHandler
  );
  server.post(
    BOT.INSERT_DEFAULT_CHAT_IMAGES,
    {
      schema: {
        summary: 'Insert default chat images',
        description: 'Insert default chat images api',
        tags: ['bot'],
      },
    },
    insertDefaultBotImages
  );
  server.get(
    BOT.GET_CHAT_IMAGE_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Insert default chat images',
        description: 'Insert default chat images api',
        tags: ['bot'],
      },
    },
    getBotDefaultImages
  );

  server.patch(
    BOT.UPDATE_BOT_TEMPLATE_LIST,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update bot template list ',
        description: 'update bot template list api',
        tags: ['bot'],
      },
    },
    updateBotTemplateList
  );

  server.post(
    BOT.INSERT_FALLBACK_MESSAGE_LIST,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Insert and update fallback message list',
        description: 'Insert and update fallback message list api',
        tags: ['bot'],
      },
    },
    insertFallbackMessageHandler
  );

  server.get(
    BOT.GET_FALLBACK_MESSAGE_LIST,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get fallback message list',
        description: 'Get fallback message list api',
        tags: ['bot'],
      },
    },
    getFallbackMessageHandler
  );

  server.get(
    `${BOT.GET_FALLBACK_MESSAGE_LIST_NO_AUTH}/:domainId`,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'Get fallback message list',
        description: 'Get fallback message list api',
        tags: ['bot'],
      },
    },
    getFallbackMessageHandler
  );

  server.post(
    BOT.DEFAULT_FALLBACK_MESSAGE_LIST,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Insert default fallback message list',
        description: 'Insert default fallback message list api',
        tags: ['bot'],
      },
    },
    insertDefaultFallbackMessage
  );
  next();
}
