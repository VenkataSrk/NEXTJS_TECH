import { FACEBOOK } from '../../helpers/constants';
import { chatQueueByAgentStatusHandler } from '../../services/facebookChat_queue';
import {
  assignCallBackFacebookHandlers,
  deleteFacebookConfigDetailsHandler,
  getFacebookCallbackConfigHandler,
  getFacebookConfigDetailsHandler,
  getFacebookConfigHandler,
  insertFacebookCallbackHandler,
  insertFacebookInHandler,
  insertUpdateFacebookConfigHandler,
  sendAppoinmentPaymentHandler,
  sendCaasFacebookHandler,
  updateDispositionFacebookCallbackHandler,
  updateDispositionFacebookHandler,
  updatePageIdFacebookHandler,
  updateStatusFacebookHandler
} from './handler';
import { sendFacebookSchema } from './schema';

export default function facebookHandler(server: any, options: any, next: any) {
  // facebook client insert
  server.post(
    FACEBOOK.INSERT_FACEBOOK_IN,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'insert facebook datas',
        description: 'insert facebook datas api',
        tags: ['facebook'],
      },
    },
    insertFacebookInHandler
  );

  // send facebook
  server.post(
    FACEBOOK.FACEBOOK_OUT,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'send facebook',
        description: 'send facebook api',
        tags: ['facebook'],
        // body: sendFacebookSchema.body,
        // response: sendFacebookSchema.response,
      },
    },
    sendCaasFacebookHandler
  );

  // update dispositin status focebook
  server.put(
    `${FACEBOOK.UPDATE_DISPOSITION_FACEBOOK}/:FacebookHistoryId`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update disposition facebook',
        description: 'update disposition facebook api',
        tags: ['facebook'],
      },
    },
    updateDispositionFacebookHandler
  );

  // update dispositin status focebook callback
  server.put(
    `${FACEBOOK.UPDATE_DISPOSITION_FACEBOOK_CALLBACK}/:FacebookHistoryId`,
    {
      preValidation: [],
      schema: {
        summary: 'update disposition facebook',
        description: 'update disposition facebook api',
        tags: ['facebook'],
      },
    },
    updateDispositionFacebookCallbackHandler
  );

  // assign callback facebook
  server.post(
    FACEBOOK.ASSIGN_CALLBACK_FACEBOOK,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update assign callback facebook',
        description: 'update assign callback facebook api',
        tags: ['facebook'],
      },
    },
    assignCallBackFacebookHandlers
  );

  // insert and update facebook configuration
  server.post(
    FACEBOOK.INSERT_UPDATE_FACEBOOK_CONFIGURATION,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'insert facebook configuration',
        description: 'insert facebook configuration api',
        tags: ['facebook']
      }
    },
    insertUpdateFacebookConfigHandler
  );

  // get facebook configuration callback
  server.get(
    FACEBOOK.GET_FACEBOOK_CONFIGURATION_CALLBACK,
    {
      preValidation: [],
      schema: {
        summary: 'get facebook configuration',
        description: 'get facebook configuration api',
        tags: ['facebook']
      },
    },
    getFacebookCallbackConfigHandler,
  );

  // get facebook configuration
  server.get(
    FACEBOOK.GET_FACEBOOK_CONFIGURATION,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get facebook configuration',
        description: 'get facebook configuration api',
        tags: ['facebook']
      },
    },
    getFacebookConfigHandler,
  );

  // get facebook configuration details
  server.get(
    FACEBOOK.GET_FACEBOOK_CONFIGURATION_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get facebook configuration',
        description: 'get facebook configuration api',
        tags: ['facebook']
      },
    },
    getFacebookConfigDetailsHandler,
  );

  // delete facebook configuration details
  server.delete(
    FACEBOOK.DELETE_FACEBOOK_CONFIGURATION_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get facebook configuration',
        description: 'get facebook configuration api',
        tags: ['facebook']
      },
    },
    deleteFacebookConfigDetailsHandler,
  );

  // facebook  configuration status
  server.put(
    FACEBOOK.UPDATE_STATUS_FACEBOOK,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update status facebook',
        description: 'update status facebook api',
        tags: ['facebook'],
      },
    },
    updateStatusFacebookHandler
  );

  // facebook update pageId
  server.put(
    FACEBOOK.UPDATE_PAGE_ID,
    {
      preValidation: [],
      schema: {
        summary: 'update page id',
        description: 'update page id api',
        tags: ['facebook'],
      },
    },
    updatePageIdFacebookHandler
  );

  // insert facebook callback status info
  server.post(
    FACEBOOK.INSERT_FACEBOOK_CALLBACK_STATUS,
    {
      preValidation: [],
      schema: {
        summary: 'insert facebook callback status info',
        description: 'insert facebook callback status infoapi',
        tags: ['facebook'],
      },
    },
    insertFacebookCallbackHandler
  );

  // send agent status queue for facebook
  server.post(
    FACEBOOK.SEND_AGENT_STATUS_QUEUE_FACEBOOK,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'send agent status queue',
        description: 'send agent status queue api',
        tags: ['facebook'],
      },
    },
    chatQueueByAgentStatusHandler
  );

  // send appoinment payment
  server.post(
    FACEBOOK.SEND_APPOINMENT_PAYMENT,
    {
      preValidation: [],
      schema: {
        summary: 'send appoinment payment',
        description: 'send appoinment payment api',
        tags: ['facebook'],
      },
    },
    sendAppoinmentPaymentHandler
  );
  next();
}
