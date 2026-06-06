/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import { CALLFLOW } from '../../helpers/constants';
import { callflowMapHandler, callflowSettingHandler, deletecallFlowMapHandler, getcallflowMapHandler, getCallFlowMapListHandler, getcallflowSettingHandler, updatecallFlowMapHandler } from './handler';
import { callFlowMapSchema, callFlowSettingSchema, deletecallFlowMapSchema, getCallFlowMapListSchema, getcallFlowMapSchema, getcallFlowSettingSchema, updatecallFlowMapSchema } from './schema';

export default function callMapHandler(server: any, options: any, next: any) {
  // callFlow map api for ccs
  server.post(
    CALLFLOW.CALLFLOW_MAP,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'callflow insert',
        description: 'callflow insert api',
        tags: ['callFlow'],
        body: callFlowMapSchema.body,
        response: callFlowMapSchema.response,
      },
    },
    callflowMapHandler
  );
  // callFlow get api for ccs
  server.get(
    `${CALLFLOW.GET_CALLFLOW_MAP}/:cid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get callflow',
        description: 'Get callflow api',
        tags: ['callFlow'],
        response: getcallFlowMapSchema.response,
      },
    },
    getcallflowMapHandler
  );
  // update api for ccs
  server.put(
    `${CALLFLOW.UPDATE_CALLFLOW_MAP}/:cid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update callFlow',
        description: 'update callFlow api',
        tags: ['callFlow'],
        body: updatecallFlowMapSchema.body,
        params: updatecallFlowMapSchema.response,
        response: callFlowMapSchema.response,
      },
    },
    updatecallFlowMapHandler
  );
  // delete api for ccs
  server.delete(
    `${CALLFLOW.DELETE_CALLFLOW_MAP}/:cid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'delete callFlow',
        description: 'delete callFlow api',
        tags: ['callFlow'],
        params: deletecallFlowMapSchema.params,
      },
    },
    deletecallFlowMapHandler
  );
  // campiagn get api for ccs
  server.get(
    CALLFLOW.CALLFLOW_MAP_GET_LIST,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get callFlow list',
        description: 'Get callFlow api list',
        tags: ['callFlow'],
        response: getCallFlowMapListSchema.response,
      },
    },
    getCallFlowMapListHandler
  );
  server.post(
    CALLFLOW.CALLFLOW_SETTING,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'callflow insert',
        description: 'callflow setting api',
        tags: ['callFlow'],
        body: callFlowSettingSchema.body,
        response: callFlowSettingSchema.response,
      },
    },
    callflowSettingHandler
  );
  // callFlow get api for ccs
  server.get(
    CALLFLOW.GET_CALLFLOW_SETTING,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get callflow',
        description: 'Get callflow setting api',
        tags: ['callFlow'],
        response: getcallFlowSettingSchema.response,
      },
    },
    getcallflowSettingHandler
  );
  next();
}
