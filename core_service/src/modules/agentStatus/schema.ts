/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

// insertAgentStatusSchema
export const insertAgentStatusSchema = {
  body: {
    type: 'object',
    properties: {
      statusName: { type: 'string' },
      description: { type: 'string' },
      channels: { type: 'array' },
      isVoice: { type: 'number', enum: [0, 1] },
      isVoiceCallTransfer: { type: 'number', enum: [0, 1] },
      isChat: { type: 'number', enum: [0, 1] },
      isChatTransfer: { type: 'number', enum: [0, 1] },
      isEmail: { type: 'number', enum: [0, 1] },
      isEmailTransfer: { type: 'number', enum: [0, 1] },
      colorCode: { type: 'string' }
    },
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        insertAgentStatus: {
          type: 'object',
          properties: {
            aid: { type: 'number' },
            statusName: { type: 'string' },
            description: { type: 'string' },
            channels: { type: 'string' },
            status: { type: 'string' },
            type: { type: 'string' },
            isVoice: { type: 'number' },
            isVoiceCallTransfer: { type: 'number' },
            isChat: { type: 'number' },
            isChatTransfer: { type: 'number' },
            isEmail: { type: 'number' },
            isEmailTransfer: { type: 'number' },
            createdAt: { type: 'string' },
            updatedAt: { type: 'string' },
            domainId: { type: 'number' },
            colorCode: { type: 'string' },
          },
        },
      },
    },
  },
};

// getAgentStatuschema
export const getAgentStatuschema = {
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getAgentListRes: {
          type: 'array',
          items: {
            properties: {
              aid: { type: 'number' },
              statusName: { type: 'string' },
              description: { type: 'string' },
              channels: { type: 'string' },
              status: { type: 'string' },
              type: { type: 'string' },
              isVoice: { type: 'number' },
              isVoiceCallTransfer: { type: 'number' },
              isChat: { type: 'number' },
              isChatTransfer: { type: 'number' },
              isEmail: { type: 'number' },
              isEmailTransfer: { type: 'number' },
              createdAt: { type: 'string' },
              updatedAt: { type: 'string' },
              domainId: { type: 'number' },
              colorCode: { type: 'string' }
            },
          },
        },
      },
    },
  },
};

// getAgentStatusDetailschema
export const getAgentStatusDetailschema = {
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getAgentDetailsRes: {
          type: 'array',
          items: {
            properties: {
              aid: { type: 'number' },
              statusName: { type: 'string' },
              description: { type: 'string' },
              channels: { type: 'string' },
              status: { type: 'string' },
              type: { type: 'string' },
              isVoice: { type: 'number' },
              isVoiceCallTransfer: { type: 'number' },
              isChat: { type: 'number' },
              isChatTransfer: { type: 'number' },
              isEmail: { type: 'number' },
              isEmailTransfer: { type: 'number' },
              createdAt: { type: 'string' },
              updatedAt: { type: 'string' },
              domainId: { type: 'number' },
              colorCode: { type: 'string' }
            },
          },
        },
      },
    },
  },
};

// updateAgentSchema
export const updateAgentSchema = {
  params: {
    type: 'object',
    properties: {
      aid: { type: 'number' },
    },
  },
  body: {
    type: 'object',
    properties: {
      statusName: { type: 'string' },
      description: { type: 'string' },
      channels: { type: 'array' },
      isVoice: { type: 'number', enum: [0, 1] },
      isVoiceCallTransfer: { type: 'number', enum: [0, 1] },
      isChat: { type: 'number', enum: [0, 1] },
      isChatTransfer: { type: 'number', enum: [0, 1] },
      isEmail: { type: 'number', enum: [0, 1] },
      isEmailTransfer: { type: 'number', enum: [0, 1] },
      colorCode: { type: 'string' }
    },
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

// deleteAgentSchema
export const deleteAgentSchema = {
  params: {
    type: 'object',
    properties: {
      aid: { type: 'number' },
    },
    required: ['aid'],
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

// isValidSchema
export const isValidSchema = {
  body: {
    type: 'object',
    properties: {
      name: { type: 'string' },
    },
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        flag: { type: 'number' }
      },
    },
  },
};

// updateAgentStatusSchema
export const updateAgentStatusSchema = {
  params: {
    type: 'object',
    properties: {
      email: { type: 'string' },
    },
  },
  body: {
    type: 'object',
    properties: {
      statusName: { type: 'string' },
      stateTime: { type: 'string' },
      isVoice: { type: 'number', enum: [0, 1] },
      isVoiceCallTransfer: { type: 'number', enum: [0, 1] },
      isChat: { type: 'number', enum: [0, 1] },
      isChatTransfer: { type: 'number', enum: [0, 1] },
      isEmail: { type: 'number', enum: [0, 1] },
      isEmailTransfer: { type: 'number', enum: [0, 1] },
    },
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
