/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

// createBotSchema
export const createBotSchema = {
  body: {
    type: 'object',
    properties: {
      botName: { type: 'string' },
      botChannelId: { type: 'string' },
      botChannelUrlId: { type: 'string' },
    },
    required: ['botName', 'botChannelId'],
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

// getBotList
export const getBotList = {
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getBotList: {
          type: 'array',
          items: {
            properties: {
              bid: { type: 'number' },
              botName: { type: 'string' },
              botChannelId: { type: 'string' },
              callSessionId: { type: 'string' },
              botChannelUrlId: { type: 'string' },
              createdAt: { type: 'string' },
              updatedAt: { type: 'string' },
              domainId: { type: 'number' },
            },
          },
        },
      },
    },
  },
};

// getBotListById
export const getBotListById = {
  params: {
    type: 'object',
    properties: {
      bid: { type: 'number' },
    },
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getBotListByRes: {
          type: 'array',
          items: {
            properties: {
              bid: { type: 'number' },
              botName: { type: 'string' },
              botChannelId: { type: 'string' },
              callSessionId: { type: 'string' },
              botChannelUrlId: { type: 'string' },
              createdAt: { type: 'string' },
              updatedAt: { type: 'string' },
              domainId: { type: 'number' },
            },
          },
        },
      },
    },
  },
};

// updateBotSchema
export const updateBotSchema = {
  params: {
    type: 'object',
    properties: {
      bid: { type: 'number' },
    },
  },
  body: {
    type: 'object',
    properties: {
      botName: { type: 'string' },
      botChannelId: { type: 'string' },
      callSessionId: { type: 'string' },
      botChannelUrlId: { type: 'string' },
    },
    required: ['botName', 'botChannelId', 'callSessionId'],
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

// deleteBotSchema
export const deleteBotSchema = {
  params: {
    type: 'object',
    properties: {
      bid: { type: 'number' },
    },
    required: ['bid'],
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
