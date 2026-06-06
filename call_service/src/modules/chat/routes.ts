/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import { CHAT } from '../../helpers/constants';
import { deleteChatBotChannelTypeHandler, getAgentStatusView, getAllChatFlowDetails, getCallSessionId, getChatByNumber, getChatDetailsByCfidNoAuth, getChatFlowDetailsByCfid, getChatFlowDetailsByDomain, getChatSessionId, getDefaultChatFlowId, getListenSessionId, getNodeDtlHandler, saveChatFlowHistory, updateChatBotChannelTypeHandler } from './handler';

export default function chatHandler(server: any, options: any, next: any) {

  server.get(
    `${CHAT.GET_SESSION_ID}/:chatSessionId`,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'Get chat session id',
        description: 'Get chat session List',
        tags: ['chat'],

      },
    },
    getChatSessionId
  );

  server.get(
    `${CHAT.GET_SESSION_CALL}/:chatSessionId`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get call session id',
        description: 'Get call session List',
        tags: ['IVR'],

      },
    },
    getCallSessionId
  );

  server.get(
    `${CHAT.GET_LISTEN_SESSION_ID}/:callSessionId`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get call listen session id',
        description: 'Get call listen session List',
        tags: ['Voice'],

      },
    },
    getListenSessionId
  );

  server.get(
    `${CHAT.GET_AGENT_STATUS_VIEW}/:calldomainId`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get call listen domain id',
        description: 'Get call listen domain List',
        tags: ['Voice'],

      },
    },
    getAgentStatusView
  );

  server.post(
    CHAT.GET_CHAT_BY_NUMBER,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get chat session',
        description: 'Get chat session List',
        tags: ['chat'],
      },
    },
    getChatByNumber
  );

  server.post(
    CHAT.SAVE_CHATFLOW_HISTORY,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'save chat flow history',
        description: 'save chat flow history api',
        tags: ['chat'],
      },
    },
    saveChatFlowHistory
  );
  server.get(
    `${CHAT.GET_CHATFLOW_BY_CFID}/:cfid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get chat flow history by chat flow id',
        description: 'get chat flow history by chat flow id api',
        tags: ['chat'],
      },
    },
    getChatFlowDetailsByCfid
  );
  server.get(
    CHAT.GET_ALL_CHATFLOW_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get all chat flow history ',
        description: 'get chat flow history api',
        tags: ['chat'],
      },
    },
    getAllChatFlowDetails
  );
  server.post(
    CHAT.GET_CHATFLOW_NODES_LIST,
    {
      schema: {
        summary: 'get chat flow history with no auth',
        description: 'get chat flow history with no auth api',
        tags: ['chat'],
      },
    },
    getChatDetailsByCfidNoAuth
  );
  // update ChatBot Channel Type
  server.put(
    CHAT.UPDATE_CHATBOTCHANNEL_TYPE,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update ChatBot Channel Type',
        description: 'update ChatBot Channel Type api',
        tags: ['chat'],
      },
    },
    updateChatBotChannelTypeHandler
  );

  // Delete ChatBot ChannelType
  server.delete(
    CHAT.DELETE_CHATBOTCHANNEL_TYPE,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Delete ChatBot ChannelType',
        description: 'Delete ChatBot ChannelType api',
        tags: ['chat'],
      },
    },
    deleteChatBotChannelTypeHandler
  );
  server.get(
    CHAT.GET_CHATFLOW_LIST_DOMAINID,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Delete ChatBot ChannelType',
        description: 'Delete ChatBot ChannelType api',
        tags: ['chat'],
      },
    },
    getChatFlowDetailsByDomain
  );
  server.get(
    `${CHAT.GET_DEFAULT_CHATFLOW_ID}/:flowId`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Delete ChatBot ChannelType',
        description: 'Delete ChatBot ChannelType api',
        tags: ['chat'],
      },
    },
    getDefaultChatFlowId
  );
  server.get(
    `${CHAT.GET_CHATFLOW_NODE_DTL}/:flowId`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get chatflow node details',
        description: 'get chatflow node details api',
        tags: ['chat'],
      },
    },
    getNodeDtlHandler
  );
  next();
}
