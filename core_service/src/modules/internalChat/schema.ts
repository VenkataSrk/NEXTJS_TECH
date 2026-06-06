export const internalChatSchema = {
  body: {
    type: 'object',
    properties: {
      groupName: { type: 'string' },
      historyId: { type: 'number' },
      channelType: { type: 'number' },
      agentExt: { type: 'array', items: {} },
      agentUserId: { type: 'string' },
      // queueId: { type: 'number' },
      customerId: { type: 'number' },
      hostUserId: { type: 'number' },
      internalChatGroupId: { type: 'string' },
    },
    required: ['groupName', 'historyId', 'channelType', 'agentExt', 'agentUserId', 'queueId', 'customerId', 'hostUserId', 'internalChatGroupId']
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        internalChatRes: {
          type: 'object',
          properties: {
            internalChatGroupId: { type: 'number' },
            customerName: { type: 'string' },
            groupName: { type: 'string' },
          },
        },
      },
    },
  },
};

export const getInternalChatSchema = {
  query: {
    type: 'object',
    properties: {
      hostUserId: { type: 'number' },
    },
    required: ['hostUserId']
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        internalChatRes: {
          type: 'array',
          items: {
            properties: {
              internalChatGroupId: { type: 'number' },
              groupName: { type: 'string' },
              customerId: { type: 'number' },
              customerName: { type: 'string' },
              queueId: { type: 'number' },
              queueName: { type: 'string' },
              internalChatId: { type: 'number' },
              lastMsg: { type: 'string' },
              sendTimestamp: { type: 'number' },
              msgStatus: { type: 'number' },
              unreadCount: { type: 'number' },
              groupCreatedTimestamp: { type: 'number' },
              isGroupClosed: { type: 'number' },
              phoneNumber: { type: 'string' },
              email: { type: 'string' },
              channelType: { type: 'string' },
              agentIds: { type: 'string' },
              directNumber: { type: 'string' },
              isAgentDeleted: { type: 'number' }
            },
          },
        },
      },
    },
  },
};

export const getParticularInternalChatSchema = {
  query: {
    type: 'object',
    properties: {
      hostUserId: { type: 'number' },
      internalChatGroupId: { type: 'number' },
    },
    required: ['hostUserId', 'internalChatGroupId']
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        internalChatRes: {
          type: 'array',
          items: {
            properties: {
              internalChatId: { type: 'number' },
              internalChatGroupId: { type: 'number' },
              domainId: { type: 'number' },
              msg: { type: 'string' },
              sendTimestamp: { type: 'number' },
              senderId: { type: 'number' },
              senderName: { type: 'string' },
              agentIds: { type: 'string' },
              isAllRead: { type: 'number' },
            },
          },
        },
      },
    },
  },
};

export const editInternalChatSchema = {
  body: {
    type: 'object',
    properties: {
      agentId: { type: 'number' },
      internalChatId: { type: 'number' },
      msg: { type: 'object' },
      agentExt: { type: 'array', items: {} },
    },
    required: ['agentId', 'internalChatId', 'msg', 'agentExt']
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

export const deleteInternalChatSchema = {
  body: {
    type: 'object',
    properties: {
      agentId: { type: 'number' },
      internalChatId: { type: 'number' },
      agentExt: { type: 'array', items: {} },
    },
    required: ['agentId', 'internalChatId', 'agentExt']
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

export const markAllInternalChatSchema = {
  body: {
    type: 'object',
    properties: {
      agentId: { type: 'number' },
      internalChatGroupId: { type: 'string' },
      agentExt: { type: 'array', items: {} },
      msgStatus: { type: 'number', enum: [0, 1, 2] },
    },
    required: ['agentId', 'internalChatGroupId', 'agentExt', 'msgStatus']
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

export const getInternalChatByHistoryIdSchema = {
  body: {
    type: 'object',
    properties: {
      agentId: { type: 'number' },
      historyId: { type: 'number' },
      channelType: { type: 'number' },
    },
    required: ['agentId', 'historyId', 'channelType']
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        internalChatRes: {
          type: 'array',
          items: {
            properties: {
              internalChatId: { type: 'number' },
              internalChatGroupId: { type: 'number' },
              msg: { type: 'string' },
              agentIds: { type: 'string' },
              senderId: { type: 'number' },
              isAllRead: { type: 'number' },
            },
          },
        },
      },
    },
  },
};
