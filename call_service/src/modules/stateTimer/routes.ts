/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import { STATE } from '../../helpers/constants';
import { createStateHandler, getStateListByHandlers } from './handler';
import { createStateSchema, getStateListById } from './schema';

export default function stateHandler(server: any, options: any, next: any) {
  // create api for state
  server.post(
    STATE.CREATE_STATE,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'create state ',
        description: 'create state api',
        tags: ['state'],
        body: createStateSchema.body,
        response: createStateSchema.response,
      },
    },
    createStateHandler
  );

  // get state list by id
  server.get(
    `${STATE.GET_SATATE_LIST_BY}/:ext`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get state list by id',
        description: 'Get state list api',
        tags: ['state'],
        params: getStateListById.params,
        response: getStateListById.response,
      },
    },
    getStateListByHandlers
  );

  next();
}
