import { ROUTING } from '../../helpers/constants';
import {
  deleteRoutingHandler,
  getRoutingHandler,
  getRoutingListHandler,
  isRoutingEnableListHandler,
  isValidRoutingNameHandler,
  routingProfileStatusHandler,
  routingQueueHandler,
  updateRoutingHandler,
  updateStatusHandler,
} from './handler';
import {
  deleteRoutingSchema,
  getRoutingSchema,
  insertRoutingSchema,
  isEnableListSchema,
  isValidNameSchema,
  routingProfileStatusSchema,
  updateStatusSchema,
} from './schema';

export default function routinghandler(server: any, options: any, next: any) {
  // routing insert
  server.post(
    ROUTING.INSERT_ROUTING,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'insert routing profile',
        description: 'routing profile api',
        tags: ['routing'],
        body: insertRoutingSchema.body,
        response: insertRoutingSchema.response,
      },
    },
    routingQueueHandler
  );

  // routing get
  server.get(
    `${ROUTING.GET_ROUTING}/:rid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get routing profile',
        description: 'routing profile api',
        tags: ['routing'],
        response: getRoutingSchema.response,
      },
    },
    getRoutingHandler
  );

  // routing update
  server.put(
    `${ROUTING.UPDATE_ROUTING}/:rid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update routing profile',
        description: 'routing profile api',
        tags: ['routing'],
        /// params: updateRoutingSchema.params,
        // body: updateRoutingSchema.body,
        // response: updateRoutingSchema.response,
      },
    },
    updateRoutingHandler
  );
  // routing delete
  server.delete(
    `${ROUTING.DELETE_ROUTING}/:rid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'delete routing profile',
        description: 'routing profile api',
        tags: ['routing'],
        params: deleteRoutingSchema.params,
      },
    },
    deleteRoutingHandler
  );

  // routing List get
  server.get(
    ROUTING.GET_ROUTING_LIST,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get routing profile list',
        description: 'routing profile list api',
        tags: ['routing'],
        // response: getRoutingListSchema.response,
      },
    },
    getRoutingListHandler
  );

  // routing profile status
  server.get(
    `${ROUTING.ROUTING_PROFILE_STATUS}/:rid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'routing profile status',
        description: 'routing profile status api',
        tags: ['routing'],
        params: routingProfileStatusSchema.params,
      },
    },
    routingProfileStatusHandler
  );

  // routing update
  server.put(
    `${ROUTING.UPDATE_STATUS}/:rid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update status',
        description: 'routing status api',
        tags: ['routing'],
        params: updateStatusSchema.params,
        body: updateStatusSchema.body,
        response: updateStatusSchema.response,
      },
    },
    updateStatusHandler
  );

  server.post(
    ROUTING.IS_VALID_ROUTING_NAME,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'valid routing',
        description: ' valid routing api',
        tags: ['routing'],
        body: isValidNameSchema.body,
        response: isValidNameSchema.response,
      },
    },
    isValidRoutingNameHandler
  );

  // get enabled list of Routing profile
  server.get(
    ROUTING.IS_ENABLE_LIST,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get enabled list',
        description: 'get enabled list api',
        tags: ['routing'],
        response: isEnableListSchema.response,
      },
    },
    isRoutingEnableListHandler
  );
  next();
}
