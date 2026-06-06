/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import { QUEUE } from '../../helpers/constants';
import { isValidSchema } from '../skills/schema';
import {
  checkQueueMappingHandler,
  customHoursStatusHandler,
  deleteBusinessHolidaysHandler,
  deleteCustomHoursHandler,
  deleteQueueHandler,
  getBusinessHolidaysHandler,
  getCustomHoursByIdHandler,
  getCustomHoursDetailHandler,
  getCustomHoursListHandler,
  getQueueHandler,
  getQueueListHandler,
  insertCustomHoursHandler,
  insertQueueHandler, insertUpdateBusinessHolidaysHandler, isValidCustomHoursNameHandler, isValidQueueNameHandler, updateBusinessHolidaysStatusHandler, updateCustomHoursByIdHandler, updateCustomHoursHandler, updateQueueHandler, updateQueueStatusHandler, updateStatusHandler
} from './handler';
import { checkqueueSchema, customHoursSchema, deleteCustomHoursSchema, deleteQueueSchema, getCustomHoursDetailSchema, getCustomHoursListSchema, getQueueListSchema, getQueueSchema, insertCustomHoursSchema, insertQueueSchema, updateCustomSchema, updateQueueSchema, updateQueuesSchema } from './schema';

export default function queuehandler(server: any, options: any, next: any) {
  // insert api for ccs
  server.post(
    QUEUE.INSERT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Insert queue',
        description: 'Insert queue api',
        tags: ['queue'],
        body: insertQueueSchema.body,
        response: insertQueueSchema.response,
      },
    },
    insertQueueHandler
  );
  // get api for ccs
  server.get(
    `${QUEUE.GET}/:qid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get queue',
        description: 'Get queue api',
        tags: ['queue'],
        // params: getQueueSchema.params,
        // response: getQueueSchema.response,
      },
    },
    getQueueHandler
  );

  // update api for ccs
  server.put(
    `${QUEUE.UPDATE}/:qid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'updatequeue',
        description: 'update queue api',
        tags: ['queue'],
        body: updateQueueSchema.body,
        params: updateQueueSchema.params,
        response: updateQueueSchema.response,
      },
    },
    updateQueueHandler
  );

  // delete api for ccs
  server.delete(
    `${QUEUE.DELETE}/:qid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'delete queue',
        description: 'delete queue api',
        tags: ['queue'],
        params: deleteQueueSchema.params
      },
    },
    deleteQueueHandler
  );
  // get list for ccs
  server.get(
    QUEUE.GET_LIST,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get queue list',
        description: 'Get queue list api',
        tags: ['queue'],
        // response: getQueueListSchema.response,
      },
    },
    getQueueListHandler
  );
  // update api for ccs
  server.put(
    `${QUEUE.UPDATE_QUEUE}/:qid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update queue',
        description: 'update queue api',
        tags: ['queue'],
        // body: updateQueueStatusSchema.body,
        // params: updateQueueStatusSchema.params,
        // response: updateQueueStatusSchema.response,
      },
    },
    updateQueueStatusHandler
  );

  // update api for ccs
  server.put(
    `${QUEUE.UPDATE_STATUS}/:qid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update queue status',
        description: 'update queue status api',
        tags: ['queue'],
        body: updateQueuesSchema.body,
        params: updateQueuesSchema.params,
        response: updateQueuesSchema.response,
      },
    },
    updateStatusHandler
  );

  // is valid queue
  server.post(
    QUEUE.IS_VALID_QUEUE_NAME,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'valid queue name',
        description: ' valid queue name api',
        tags: ['queue'],
        body: isValidSchema.body,
        response: isValidSchema.response,
      },
    },
    isValidQueueNameHandler
  );

  // chech queue mapping
  server.get(
    `${QUEUE.CHECK_QUEUE_MAPPING}/:qid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'check queue mapping',
        description: 'check queue mapping api',
        tags: ['queue'],
        params: checkqueueSchema.params,
      },
    },
    checkQueueMappingHandler
  );

  // insert api for custom hours
  server.post(
    QUEUE.INSERT_CUSTOM_HOURS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Insert custom hours',
        description: 'Insert custom hours api',
        tags: ['queue'],
        body: insertCustomHoursSchema.body,
        response: insertCustomHoursSchema.response,
      },
    },
    insertCustomHoursHandler
  );

  // get list for custom hours
  server.get(
    QUEUE.GET_CUSTOM_HOURS_LIST,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get custom hours list',
        description: 'Get custom hours list api',
        tags: ['queue'],
        response: getCustomHoursListSchema.response,
      },
    },
    getCustomHoursListHandler
  );

  // update api for custom hours
  server.put(
    `${QUEUE.UPDATE_CUSTOM_HOURS}/:cid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update custom hours',
        description: 'update custom hours api',
        tags: ['queue'],
        body: updateCustomSchema.body,
        params: updateCustomSchema.params,
        response: updateCustomSchema.response,
      },
    },
    updateCustomHoursHandler
  );

  server.put(
    `${QUEUE.UPDATE_CUSTOM_HOURS_BY_ID}/:cid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update custom hours by id',
        description: 'update custom hours by id api',
        tags: ['queue'],
      },
    },
    updateCustomHoursByIdHandler
  );

  // delete api for ccs
  server.delete(
    `${QUEUE.DELETE_CUSTOM_HOURS}/:cid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'delete custom hours',
        description: 'delete custom hours api',
        tags: ['queue'],
        params: deleteCustomHoursSchema.params,
      },
    },
    deleteCustomHoursHandler
  );

  // get api for ccs
  server.get(
    `${QUEUE.CUSTOM_HOURS_STATUS}/:cid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'custom hours status',
        description: 'custom hours status api',
        tags: ['queue'],
        // params: deleteCustomHoursSchema.params,
      },
    },
    customHoursStatusHandler
  );

  // get custome hours details
  server.get(
    `${QUEUE.GET_CUSTOM_HOURS_DETAIL}/:cid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get custom hours details',
        description: 'Get custom hours details api',
        tags: ['queue'],
        response: getCustomHoursDetailSchema.response,
      },
    },
    getCustomHoursDetailHandler
  );

  server.get(
    `${QUEUE.GET_CUSTOM_HOURS}/:cid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get custom hours details by id',
        description: 'Get custom hours details by id api',
        tags: ['queue'],
      },
    },
    getCustomHoursByIdHandler
  );

  server.post(
    QUEUE.IS_VALID_CUSTOM_HOURS_NAME,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'valid custom',
        description: ' valid custom api',
        tags: ['queue'],
        body: customHoursSchema.body,
        response: customHoursSchema.response,
      },
    },
    isValidCustomHoursNameHandler
  );

  server.post(
    QUEUE.INSERT_UPDATE_BUSINESS_HOLIDAYS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Insert update business holidays',
        description: ' Insert update business holidays api',
        tags: ['queue'],
      },
    },
    insertUpdateBusinessHolidaysHandler
  );

  server.get(
    QUEUE.GET_BUSINESS_HOLIDAYS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get business holidays',
        description: ' get business holidays api',
        tags: ['queue'],
      },
    },
    getBusinessHolidaysHandler
  );

  server.get(
    QUEUE.DELETE_BUSINESS_HOLIDAYS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Delete business holidays',
        description: ' Delete business holidays api',
        tags: ['queue'],
      },
    },
    deleteBusinessHolidaysHandler
  );

  server.put(
    QUEUE.UPDATE_BUSINESS_HOLIDAYS_STATUS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Update business holidays status',
        description: ' Update business holidays api sta',
        tags: ['queue'],
      },
    },
    updateBusinessHolidaysStatusHandler
  );

  next();
}
