/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import { CALL } from '../../helpers/constants';
import {
  assignCallBackCallHandlers,
  assignCallBackHandlers,
  deleteCallFlow,
  getActivityListHandlers,
  getCallBackListHandler,
  getCallFlowDetail,
  getCallFlowList,
  getConversationListHandler,
  getCustomerDndInfo,
  getSessionHistoryDetails,
  getSessionIdList,
  saveCallFlowData,
  updateCallBackCallHandlers,
  updateCallBackCallNewHandlers,
  updateCallBackChatHandlers,
  updateCallBackSocialmediaHandlers,
  updateCallFlowDataForPublish,
  updateVoiceMailCallback,
} from './handler';

import {
  callflowlist,
  callflowSchema,
  callflowsdetails
} from './schema';

export default function callFlowhandler(server: any, options: any, next: any) {
  // Save abd publish callflow
  server.post(
    CALL.SAVE_CALL_FLOW,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Save and Publish',
        description: 'Save or Publish call flow',
        tags: ['IVR'],
        body: callflowSchema.body,

      },
    },
    saveCallFlowData
  );

  server.put(
    CALL.UPDATE_CALL_FLOW,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Save and Publish',
        description: 'Save or Publish call flow',
        tags: ['IVR'],
        body: callflowSchema.body,
        // response: callflowSchema.defaultRes,
      },
    },
    updateCallFlowDataForPublish
  );

  // Get callflow list
  server.get(
    CALL.GET_CALL_FLOW_LIST,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'CallFlow List',
        description: 'Get Call flow List',
        tags: ['IVR'],
        response: callflowlist.defaultRes
      },
    },
    getCallFlowList
  );

  // Get callflow list
  server.get(
    CALL.GET_CALL_FLOW_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Call flow details',
        description: 'Get Call flow details',
        tags: ['IVR'],
        response: callflowsdetails.defaultRes
      },
    },
    getCallFlowDetail
  );

  // Get sessionId list
  server.get(
    `${CALL.GET_SESSIONID_LIST}/:sessionId`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get sessionid list',
        description: 'get sessionid list api',
        tags: ['IVR'],
      },
    },
    getSessionIdList
  );

  // Get call back list
  server.post(
    CALL.GET_CALLBACK_LIST,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get call back list',
        description: 'get callback list api',
        tags: ['IVR'],
      },
    },
    getCallBackListHandler
  );

  // Get session history details
  server.post(
    CALL.GET_SESSION_HISTORY_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get history details',
        description: 'get get history details api',
        tags: ['IVR'],
      },
    },
    getSessionHistoryDetails
  );

  // update voiceMail callBack
  server.post(
    CALL.UPDATE_VOICEMAIL_CALLBACK,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update voiceMail callBack',
        description: 'update voiceMail callBack api',
        tags: ['IVR'],
      },
    },
    updateVoiceMailCallback
  );

  // assign callback
  server.post(
    CALL.ASSIGN_CALLBACK,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update assign callback',
        description: 'update assign callback api',
        tags: ['IVR'],
      },
    },
    assignCallBackHandlers
  );

  server.post(
    CALL.ASSIGN_CALLBACK_CALL,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update assign callback call',
        description: 'update assign callback call api',
        tags: ['Call'],
      },
    },
    assignCallBackCallHandlers
  );

  server.post(
    CALL.UPDATE_CALLBACK_CALL,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update callback session',
        description: 'update callback session api',
        tags: ['Call'],
      },
    },
    updateCallBackCallHandlers
  );

  server.post(
    CALL.UPDATE_CALLBACK_SESSIONID,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update callback sessionId',
        description: 'update callback sessionId api',
        tags: ['Call'],
      },
    },
    updateCallBackCallNewHandlers
  );

  server.post(
    CALL.UPDATE_CALLBACK_CHAT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update callback session',
        description: 'update callback session api',
        tags: ['Chat'],
      },
    },
    updateCallBackChatHandlers
  );

  // update callback SocialMedia
  server.post(
    CALL.UPDATE_CALLBACK_SOCIAL_MEDIA,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update callback session',
        description: 'update callback session api',
        tags: ['Chat'],
      },
    },
    updateCallBackSocialmediaHandlers
  );

  // Get activity list
  server.post(
    CALL.GET_ACTIVITY_LIST,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get activity list',
        description: 'get activity list api',
        tags: ['IVR'],
      },
    },
    getActivityListHandlers
  );

  // Get voice call back list
  /*   server.post(
      CALL.GET_VOICE_CALLBACK_LIST,
      {
        preValidation: [server.validateSession],
        schema: {
          summary: 'get voice call back list',
          description: 'get voice callback list api',
          tags: ['IVR'],
        },
      },
      getVoiceCallBackListHandler
    ); */

  // Get conversation History
  server.get(
    `${CALL.GET_CONVERSATION_LIST}/:sessionId`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get conversation list',
        description: 'get conversation list api',
        tags: ['IVR'],
      },
    },
    getConversationListHandler
  );
  server.post(
    CALL.DELETE_CALL_FLOW,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Delete call flow',
        description: 'Delete call flow api',
        tags: ['call flow'],
      },
    },
    deleteCallFlow
  );
  server.post(
    CALL.GET_CUSTOMER_DND_INFO,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get customer dnd',
        description: 'get customer dnd api',
        tags: ['dnd'],
      },
    },
    getCustomerDndInfo
  );
  next();
}
