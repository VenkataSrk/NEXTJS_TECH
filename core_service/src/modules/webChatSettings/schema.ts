/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

// createChatCustomersSchema
export const createChatCustomersSchema = {
  body: {
    type: 'object',
    properties: {
      domainName: { type: 'string' },
      description: { type: 'string' },
      codeSnippet: { type: 'string' },
    },
    required: ['description', 'codeSnippet', 'domainName'],
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
      },
    },
  },
};

// getChatList
export const getChatList = {
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getChatCustomersList: {
          type: 'array',
          items: {
            properties: {
              botId: { type: 'number' },
              domainId: { type: 'number' },
              domainName: { type: 'string' },
              description: { type: 'string' },
              codeSnippet: { type: 'string' },
              createdAt: { type: 'string' },
              updatedAt: { type: 'string' },
            },
          },
        },
      },
    },
  },
};

// getPrimaryField

export const getPrimaryField = {
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getPrimaryField: {
          type: 'array',
          items: {
            properties: {
              aid: { type: 'number' },
              title: { type: 'string' },
              domainId: { type: 'number' },
              domainName: { type: 'string' },
              description: { type: 'string' },
              callflow: { type: 'string' },
              chatIconLogo: { type: 'string' },
              domain: { type: 'string' },
              personalization: { type: 'string' },
              primaryForm: { type: 'string' },
              codeSnippet: { type: 'string' },
            },
          },
        },
      },
    },
  },
};

// createGroupCoChatSchema
export const createGroupCoChatSchema = {
  body: {
    type: 'object',
    properties: {
      channelName: { type: 'string' },
      subscribedUsers: { type: 'array' },
      channelIcon: { type: 'string' },
      lastMessageInfo: { type: 'object' },
      unseenCount: { type: 'number', enum: [0, 1] },
      allSeenStatus: { type: 'number', enum: [0, 1] },
      isChannelMuted: { type: 'number', enum: [0, 1] },
      messages: { type: 'array' },
      customerInfo: { type: 'object' }
    },
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        createGroupCoChat: {
          type: 'object',
          properties: {
            uid: { type: 'string' },
            channelId: { type: 'string' },
            channelName: { type: 'string' },
            subscribedUsers: { type: 'array' },
            channelIcon: { type: 'string' },
            lastMessageInfo: {},
            unseenCount: { type: 'number' },
            allSeenStatus: { type: 'string' },
            messages: { type: 'array' },
            isChannelMuted: { type: 'number' },
            domainId: { type: 'number' },
            customerInfo: {},
          },
        },
      },
    },
  },
};

// getGroupCoChatSchema
export const getGroupCoChatSchema = {
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getGroupChatRes: {
          type: 'array',
          items: {
            properties: {
              coId: { type: 'number' },
              uid: { type: 'string' },
              channelId: { type: 'string' },
              channelName: { type: 'string' },
              subscribedUsers: { type: 'string' },
              channelIcon: { type: 'string' },
              lastMessageInfo: {},
              unseenCount: { type: 'number' },
              allSeenStatus: { type: 'string' },
              domainId: { type: 'number' },
              customerInfo: {},
              messages: { type: 'string' },
            },
          },
        },
      },
    },
  },
};

// getGroupCoChatIdSchema
export const getGroupCoChatIdSchema = {
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getGroupChatIdRes: {
          type: 'array',
          items: {
            properties: {
              coId: { type: 'number' },
              uid: { type: 'string' },
              channelId: { type: 'string' },
              channelName: { type: 'string' },
              subscribedUsers: { type: 'string' },
              channelIcon: { type: 'string' },
              lastMessageInfo: {},
              unseenCount: { type: 'number' },
              allSeenStatus: { type: 'string' },
              domainId: { type: 'number' },
              customerInfo: {},
              messages: { type: 'string' },
            },
          },
        },
      },
    },
  },
};
