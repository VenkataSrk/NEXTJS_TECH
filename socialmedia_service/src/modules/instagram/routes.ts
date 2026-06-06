import { INSTAGRAM } from '../../helpers/constants';
import { chatQueueByAgentStatusInstaHandler } from '../../services/instagramChat_queue';
import {
  assignCallBackInstagramHandlers,
  deleteInstagramConfigDetailsHandler,
  getIntsagramCallbckConfigHandler,
  getIntsagramConfigDetailsHandler,
  getIntsagramConfigHandler,
  insertInstagramCallbackHandler,
  insertInstagramHandler,
  insertUpdateInstagramConfigHandler,
  sendAppoinmentPaymentInstaHandler,
  sendCaasInstagramHandler,
  updateDispositionInstagramCallbackHandler,
  updateDispositionInstagramHandler,
  updatePageIdInstagramHandler,
  updateStatusInstagramHandler
} from './handler';

export default function instagremHandler(server: any, options: any, next: any) {
  // instagram client insert
  server.post(
    INSTAGRAM.INSERT_INSTAGRAM_IN,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'insert instagram datas',
        description: 'insert instagram datas api',
        tags: ['instagram'],
      },
    },
    insertInstagramHandler
  );

  // send instagram
  server.post(
    INSTAGRAM.INSTAGRAM_OUT,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'send instagram',
        description: 'send instagram api',
        tags: ['instagram'],
        // body: sendInstagramSchema.body,
        // response: sendInstagramSchema.response,
      },
    },
    sendCaasInstagramHandler
  );

  // update dispositin status instagram
  server.put(
    `${INSTAGRAM.UPDATE_DISPOSITION_INSTAGRAM}/:InstagramHistoryId`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update disposition instagram',
        description: 'update disposition instagram api',
        tags: ['instagram'],
      },
    },
    updateDispositionInstagramHandler
  );

  // update dispositin status instagram
  server.put(
    `${INSTAGRAM.UPDATE_DISPOSITION_INSTAGRAM_CALLBACK}/:InstagramHistoryId`,
    {
      preValidation: [],
      schema: {
        summary: 'update disposition instagram',
        description: 'update disposition instagram api',
        tags: ['instagram'],
      },
    },
    updateDispositionInstagramCallbackHandler
  );

  // assign callback instagram
  server.post(
    INSTAGRAM.ASSIGN_CALLBACK_INSTAGRAM,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update assign callback instagram',
        description: 'update assign callback instagram api',
        tags: ['instagram'],
      },
    },
    assignCallBackInstagramHandlers
  );

  // insert and update instagram configuration
  server.post(
    INSTAGRAM.INSERT_UPDATE_INSTAGRAM_CONFIGURATION,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'insert instagram configuration',
        description: 'insert instagram configuration api',
        tags: ['instagram']
      }
    },
    insertUpdateInstagramConfigHandler
  );

  // get instagram configuration
  server.get(
    INSTAGRAM.GET_INSTAGRAM_CONFIGURATION,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get instagram configuration',
        description: 'get instagram configuration api',
        tags: ['instagram']
      },
    },
    getIntsagramConfigHandler,
  );

  // get instagram configuration callback
  server.get(
    INSTAGRAM.GET_INSTAGRAM_CONFIGURATION_CALLBACK,
    {
      preValidation: [],
      schema: {
        summary: 'get instagram configuration',
        description: 'get instagram configuration api',
        tags: ['instagram']
      },
    },
    getIntsagramCallbckConfigHandler,
  );

  // get instagram configuration details
  server.get(
    INSTAGRAM.GET_INSTAGRAM_CONFIGURATION_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get instagram configuration',
        description: 'get instagram configuration api',
        tags: ['instagram']
      },
    },
    getIntsagramConfigDetailsHandler,
  );

  // delete instagram configuration details
  server.delete(
    INSTAGRAM.DELETE_INSTAGRAM_CONFIGURATION_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get instagram configuration',
        description: 'get instagram configuration api',
        tags: ['instagram']
      },
    },
    deleteInstagramConfigDetailsHandler,
  );

  // instagram  configuration status
  server.put(
    INSTAGRAM.UPDATE_STATUS_INSTAGRAM,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update status instagram',
        description: 'update status instagram api',
        tags: ['instagram'],
      },
    },
    updateStatusInstagramHandler
  );

  // instagram update pageId
  server.put(
    INSTAGRAM.UPDATE_INSTAGRAM_PAGE_ID,
    {
      preValidation: [],
      schema: {
        summary: 'update instagram page id',
        description: 'update instagram page id api',
        tags: ['instagram'],
      },
    },
    updatePageIdInstagramHandler
  );

  // insert instagram callback status info
  server.post(
    INSTAGRAM.INSERT_INSTAGRAM_CALLBACK_STATUS,
    {
      preValidation: [],
      schema: {
        summary: 'insert instagram callback status info',
        description: 'insert instagram callback status info api',
        tags: ['instagram'],
      },
    },
    insertInstagramCallbackHandler
  );

  // send agent status queue for instagram
  server.post(
    INSTAGRAM.SEND_AGENT_STATUS_QUEUE_INSTAGRAM,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'send agent status queue',
        description: 'send agent status queue api',
        tags: ['instagram'],
      },
    },
    chatQueueByAgentStatusInstaHandler
  );

  // send appoinment payment
  server.post(
    INSTAGRAM.SEND_APPOINMENT_PAYMENT,
    {
      preValidation: [],
      schema: {
        summary: 'send appoinment payment',
        description: 'send appoinment payment api',
        tags: ['instagram'],
      },
    },
    sendAppoinmentPaymentInstaHandler
  );
  next();
}
