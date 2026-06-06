/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

// createCallRegisterSchema
export const createCallRegisterSchema = {
  body: {
    type: 'object',
    properties: {
      deviceToken: { type: 'string' },
      deviceType: { type: 'string', enum: ['CCAAS:AND', 'CCAAS:IOS'] }
    },
    required: ['deviceToken', 'deviceType'],
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

// in call status schema
export const inCallSchema = {
  query: {
    type: 'object',
    properties: {
      status: { type: 'number', enum: [0, 1] },
      agentId: { type: 'number' }
    },
    required: ['status', 'agentId'],
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
