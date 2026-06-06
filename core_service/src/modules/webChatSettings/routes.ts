/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import { CHATS } from '../../helpers/constants';
import {
  addChatFlowlanguageHandler,
  AgentTrackerSession,
  assignCallBackChatHandlers,
  chatWidgetNameCheck,
  cloneWebChatConfigurationController,
  createChatCustomerHandler,
  createGroupCoChatHandler,
  createWebChatWidget,
  deleteWebChatWidgetConfiguration,
  GetAllChatSesssionMessageForNlp,
  getBotDetails,
  getCallbackListNotificationHandlers,
  getChatCustomerListHandler,
  getChatFlowlanguageHandler,
  GetChatFlowVariablesHandler,
  getChatLabelsById,
  getChatWidgetHandlerById,
  getClientChatWidgetHandlerById,
  getCochatDetails,
  getCochatDetailsByID,
  getCompanyDomainHandler,
  getConcurrentChatHandler,
  getCustomerContactFields,
  getCustomerLatestMsgHandler,
  getGroupCoChatHandler,
  getGroupCoChatIdHandler,
  getTriggerMessage,
  getWebChatConfigurationList,
  getwebChatQuickActions,
  insertBulkIrrelevantQuestion,
  InsertChatFlowVariablesHandler,
  insertUpdateConcurrentChatHandler,
  insertUpdateEmailFlowHistoryHandler,
  insertUpdateTriggerMessage,
  insertwebChatQuickActions,
  sendMessageToCoChatHandler,
  setChatFlowlanguageHandler,
  storeCochatMesssage,
  topUpGenerator,
  UpdateIsAgentConcurrentChatHandler,
  updateWebchatQuickActions,
  updateWebChatWidgetHandler,
  updateWebChatWidgetStatusHandler
} from './handler';
import {
  createChatCustomersSchema,
  createGroupCoChatSchema,
  getChatList,
  getGroupCoChatIdSchema,
  getGroupCoChatSchema
} from './schema';

export default function chatSettingsHandler(server: any, options: any, next: any) {
  // create chat customer api for chat
  server.post(
    CHATS.CREATE_CHAT_CUSTOMERS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'create chat customer',
        description: 'create chat customer api',
        tags: ['webChat'],
        body: createChatCustomersSchema.body,
        response: createChatCustomersSchema.response,
      },
    },
    createChatCustomerHandler
  );

  // get chat customer api for chat
  server.get(
    CHATS.GET_CHAT_CUSTOMERS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get chat customers list',
        description: 'Get chat customers list api',
        tags: ['webChat'],
        response: getChatList.response,
      },
    },
    getChatCustomerListHandler
  );

  server.get(
    CHATS.GET_WEBCHAT_WIDGET_LIST,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get chat primary field',
        description: 'Get chat primary field api',
        tags: ['webChat'],
        // response: getPrimaryField.response,
      },
    },
    getWebChatConfigurationList
  );

  server.post(
    CHATS.CREATE_WEBCHAT_WIDGET,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Create chat primary field',
        description: 'Create chat primary field api',
        tags: ['webChat'],
        // response: getPrimaryField.response,
      },
    },
    createWebChatWidget
  );

  // get data by qid

  server.get(
    `${CHATS.GET_CHAT_WIDGET_BY_ID}/:webchatid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get chat wideget',
        description: 'Get chat  widget api by aid',
        tags: ['webChat'],
        // params: getQueueSchema.params,
        // response: getQueueSchema.response,
      },
    },
    getChatWidgetHandlerById
  );

  // update webChat
  server.put(
    `${CHATS.UPDATE_WEB_CHAT_WIDGET}/:webchatid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update web chat configuration by webchatid',
        description: 'update web chat configuration api by webchatid',
        tags: ['webChat'],
        // body: updateQueueSchema.body,
        // params: updateQueueSchema.params,
        // response: updateQueueSchema.response,
      },
    },
    updateWebChatWidgetHandler
  );

  server.put(
    `${CHATS.UPDATE_WEB_CHAT_WIDGET_STATUS}/:webchatid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update web chat configuration by webchatid status',
        description: 'update web chat configuration api by webchatid status',
        tags: ['webChat'],
      },
    },
    updateWebChatWidgetStatusHandler
  );

  // create group co-chat
  server.post(
    CHATS.CREATE_GROUP_CO_CHATS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'create group co-chat',
        description: 'create group co-chat api',
        tags: ['co-chat'],
        body: createGroupCoChatSchema.body,
        response: createGroupCoChatSchema.response,
      }
    },
    createGroupCoChatHandler
  );

  // get group co-chat
  server.get(
    CHATS.GET_GROUP_CO_CHATS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get group co-chat',
        description: 'get group co-chat api',
        tags: ['co-chat'],
        response: getGroupCoChatSchema.response,
      },
    },
    getGroupCoChatHandler
  );

  // get group co-chat by id
  server.get(
    CHATS.GET_GROUP_CO_CHATS_ID,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get group co-chat by id ',
        description: 'get group co-chat by id api',
        tags: ['co-chat'],
        response: getGroupCoChatIdSchema.response,
      },
    },
    getGroupCoChatIdHandler
  );

  // update group co-chat by id
  server.put(
    CHATS.SEND_MESSAGE_TO_CO_CHATS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get group co-chat by id ',
        description: 'get group co-chat by id api',
        tags: ['co-chat'],
        // response: getGroupCoChatIdSchema.response,
      },
    },
    sendMessageToCoChatHandler
  );
  next();

  server.get(
    `${CHATS.CLIENT_CHAT_WIDGET_DETAILS}/:webchatid`,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'get chat widget for client  by id ',
        description: 'get chat widget for client by id api',
        tags: ['co-chat'],
        // response: getGroupCoChatIdSchema.response,
      },
    },
    getClientChatWidgetHandlerById
  );
  server.get(
    CHATS.GET_BOT_NAMES,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get chat widget for client  by id ',
        description: 'get chat widget for client by id api',
        tags: ['webchat'],
      },
    },
    getBotDetails
  );
  server.get(
    CHATS.GET_COCHAT_LIST,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get coChat list',
        description: 'get coChat list api',
        tags: ['coChat'],
      },
    },
    getCochatDetails
  );

  server.get(
    `${CHATS.GET_COCHAT_LIST_BY_ID}/:channelId`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get coChat by id list',
        description: 'get coChat list by id api',
        tags: ['coChat'],
      },
    },
    getCochatDetailsByID
  );

  server.post(
    CHATS.STORE_COCHAT_MESSAGE,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'store cochat message',
        description: 'store cochat message api',
        tags: ['coChat'],
      },
    },
    storeCochatMesssage
  );

  server.post(
    CHATS.IS_VALID_WEB_CHAT_WIDGET_NAME,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Validate the web chat name',
        description: 'Validate web cha name api',
        tags: ['coChat'],
      },
    },
    chatWidgetNameCheck
  );

  server.post(
    CHATS.INSERT_QUICK_ACTIONS_LIST,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Insert quick action list',
        description: 'Insert quick action list api',
        tags: ['coChat'],
      },
    },
    insertwebChatQuickActions
  );

  server.get(
    `${CHATS.GET_QUICK_ACTIONS_LIST}/:chatFlowId`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Insert quick action list',
        description: 'Insert quick action list api',
        tags: ['coChat'],
      },
    },
    getwebChatQuickActions
  );

  server.patch(
    CHATS.UPDATE_QUICK_ACTION_LIST,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Update quick action list',
        description: 'Update quick action list api',
        tags: ['coChat'],
      },
    },
    updateWebchatQuickActions
  );

  server.get(
    `${CHATS.GET_NODES_LABEL_LIST}/:chatId`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get the nodes label list',
        description: 'get the nodes label list api',
        tags: ['coChat'],
      },
    },
    getChatLabelsById
  );

  server.post(
    `${CHATS.INSERT_UPDATE_TRIGGER_MESSAGE}`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'insert update trigger messages',
        description: 'insert update trigger messages api',
        tags: ['webChat'],
      },
    },
    insertUpdateTriggerMessage
  );

  server.get(
    `${CHATS.GET_TRIGGER_MESSAGE}`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get trigger message',
        description: 'get trigger message api',
        tags: ['webChat'],
      },
    },
    getTriggerMessage
  );

  server.post(
    `${CHATS.TOP_UP_GENERATOR}`,
    {
      schema: {
        summary: 'top up generator',
        description: 'top up generator api',
        tags: ['webChat'],
      },
    },
    topUpGenerator
  );

  server.get(
    `${CHATS.GET_CUSTOMER_CONTACT_FIELD}`,
    {
      schema: {
        summary: 'get customer contact field',
        description: 'get customer contact field api',
        tags: ['webChat'],
      },
    },
    getCustomerContactFields
  );

  server.delete(
    `${CHATS.DELETE_CHAT_WIDGET_CONFIGURATION}/:aid`,
    {
      schema: {
        summary: 'delete chat widget configuration',
        description: 'delete chat widget configuration api',
        tags: ['webChat'],
      },
    },
    deleteWebChatWidgetConfiguration
  );
  server.post(
    `${CHATS.CLONE_WEB_CHAT_CONFIGURATION}`,
    {
      schema: {
        summary: 'clone web chat configuration field',
        description: 'clone web chat configuration field api',
        tags: ['webChat'],
      },
    },
    cloneWebChatConfigurationController
  );

  // insert and update concurrent Chat
  server.post(
    CHATS.INSERT_UPDATE_CONCURRENT_CHAT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'insert and update concurrent Chat',
        description: 'insert and update concurrent Chat api',
        tags: ['webChat']
      }
    },
    insertUpdateConcurrentChatHandler
  );

  // get agent tracker session
  server.get(
    CHATS.GET_AGENT_TRACKER,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get agent tracker session',
        description: 'Get agent tracker session api',
        tags: ['webChat']
      }
    },
    AgentTrackerSession
  );

  // get concurrent chat
  server.get(
    CHATS.GET_CONCURRENT_CHAT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get concurrent chat',
        description: 'get concurrent chat api',
        tags: ['webChat']
      },
    },
    getConcurrentChatHandler,
  );

  // update is agent specific
  server.post(
    CHATS.UPDATE_IS_AGENT_CONCURRENT_CHAT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update isAgent concurrent Chat',
        description: 'update isAgent update concurrent Chat api',
        tags: ['webChat']
      }
    },
    UpdateIsAgentConcurrentChatHandler
  );

  // insert and update email flow history Chat
  server.post(
    CHATS.INSERT_UPDATE_EMAILFLOW_HISTORY,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'insert and update email flow history',
        description: 'insert and update email flow history api',
        tags: ['webChat']
      }
    },
    insertUpdateEmailFlowHistoryHandler
  );

  // assign callback chat
  server.post(
    CHATS.ASSIGN_CALLBACK_CHAT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update assign callback chat',
        description: 'update assign callback chat api',
        tags: ['webChat'],
      },
    },
    assignCallBackChatHandlers
  );

  // assign callback chat
  server.post(
    CHATS.GET_CALLBACK_LIST_NOTIFICATION,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get callback chat notification',
        description: 'get callback notification chat api',
        tags: ['webChat'],
      },
    },
    getCallbackListNotificationHandlers
  );

  // insert update callback chat
  server.post(
    CHATS.INSERT_UPDATE_CHAT_VARIABLE,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'insert update chat variable',
        description: 'insert update chat variable api',
        tags: ['webChat']
      }
    },
    InsertChatFlowVariablesHandler
  );

  // get chat variables
  server.get(
    `${CHATS.GET_CHAT_VARIABLES}/:chatFlowId`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get chat variable',
        description: 'get chat variable api',
        tags: ['webChat']
      }
    },
    GetChatFlowVariablesHandler
  );

  server.get(
    CHATS.GET_ALL_SESSION_MESSAGE,
    {
      schema: {
        summary: 'get all chat history message list',
        description: 'get all chat history message list api',
        tags: ['webChat']
      }
    },
    GetAllChatSesssionMessageForNlp
  );

  server.get(
    `${CHATS.GET_COMPANY_DOMAIN_URL}/:domainId`,
    {
      schema: {
        summary: 'get company domain name by domain id',
        description: 'get company domain name buy domain id api',
        tags: ['webChat']
      }
    },
    getCompanyDomainHandler
  );

  server.post(
    `${CHATS.INSERT_BULK_IRRELEVANT_QUESTIONS}`,
    {
      schema: {
        summary: 'insert bulk irrelevant questions',
        description: 'insert bulk irrelevant questions api',
        tags: ['webChat']
      }
    },
    insertBulkIrrelevantQuestion
  );

  server.get(
    `${CHATS.GET_CUSTOMER_LATEST_MSG}`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get customer latest message by customer id',
        description: 'Get customer latest message by customer id api',
        tags: ['webChat']
      }
    },
    getCustomerLatestMsgHandler
  );
  server.post(
    `${CHATS.ADD_CHAT_FLOW_LANGUAGE}`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'insert chat flow language settings data',
        description: 'insert chat flow language settings data api',
        tags: ['webChat']
      }
    },
    addChatFlowlanguageHandler
  );
  server.get(
    `${CHATS.GET_CHAT_FLOW_LANGUAGE}/:flowId`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'insert bulk irrelevant questions',
        description: 'insert bulk irrelevant questions api',
        tags: ['webChat']
      }
    },
    getChatFlowlanguageHandler
  );
  server.post(
    `${CHATS.SET_CHAT_FLOW_LANGUAGE}`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'insert bulk irrelevant questions',
        description: 'insert bulk irrelevant questions api',
        tags: ['webChat']
      }
    },
    setChatFlowlanguageHandler
  );
  next();
}
