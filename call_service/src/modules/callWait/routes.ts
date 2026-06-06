/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import { QUEUE } from '../../helpers/constants';
import {
  getAllQueueWaitTimeHandlers, getQueueWaitTimeHandlers, insertQueue,
} from './handler';

export default function queueWaitTimeHandler(server: any, options: any, next: any) {

  // get queue wait time by id
  server.get(
    `${QUEUE.GET_QUEUE_WAIT_TIME}/:qid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get queue wait time by id',
        description: 'Get queue wait time api',
        tags: ['queue'],
      },
    },
    getQueueWaitTimeHandlers
  );

  // get queue wait time by id
  server.get(
    QUEUE.GET_ALL_QUEUE_WAIT_TIME,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get queue wait time by id',
        description: 'Get queue wait time api',
        tags: ['queue'],
      },
    },
    getAllQueueWaitTimeHandlers
  );

  server.post(
    QUEUE.INSERT_QUEUE_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Insert queue details',
        description: 'Insert queue details api',
        tags: ['queue'],
      },
    },
    insertQueue
  );
  next();
}
