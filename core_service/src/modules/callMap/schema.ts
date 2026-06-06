/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

// callFlowMapSchema
export const callFlowMapSchema = {
  body: {
    type: 'object',
    properties: {
      callFlowId: { type: 'string' },
      domainName: { type: 'string' },
      description: { type: 'string' },
    },
    required: ['channelName', 'callFlowId'],
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        insertCallFlowList: {
          type: 'object',
          properties: {
            cid: { type: 'number' },
            channelName: { type: 'string' },
            sourceId: { type: 'string' },
            callFlowId: { type: 'string' },
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
};
// getcallFlowMapSchema
export const getcallFlowMapSchema = {
  params: {
    type: 'object',
    properties: {
      cid: { type: 'number' },
    },
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getcallflowMapList: {
          type: 'array',
          items: {
            properties: {
              cid: { type: 'number' },
              sourceId: { type: 'string' },
              callFlowId: { type: 'string' },
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

// updatecallFlowMapSchema
export const updatecallFlowMapSchema = {
  params: {
    type: 'object',
    properties: {
      cid: { type: 'number' },
    },
  },
  body: {
    type: 'object',
    properties: {
      callFlowId: { type: 'string' },
    },
    required: ['callFlowId'],
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

// deletecallFlowMapSchema
export const deletecallFlowMapSchema = {
  params: {
    type: 'object',
    properties: {
      cid: { type: 'number' },
    },
    required: ['cid'],
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
// getCallFlowMapListSchema
export const getCallFlowMapListSchema = {
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getCallFlowMapList: {
          type: 'array',
          items: {
            properties: {
              cid: { type: 'number' },
              sourceId: { type: 'string' },
              callFlowId: { type: 'string' },
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

// callFlowSettingSchema
export const callFlowSettingSchema = {
  body: {
    type: 'object',
    properties: {
      callFlow: { type: 'string' },
      status: { type: 'string' },
      description: { type: 'string' },
      sourceId: { type: 'string' },
    },
    required: ['callFlow'],
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
// getcallFlowSettingSchema
export const getcallFlowSettingSchema = {
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getcallflowSettingList: {
          type: 'array',
          items: {
            properties: {
              cid: { type: 'number' },
              sourceId: { type: 'string' },
              callFlow: { type: 'string' },
              domainId: { type: 'number' },
              status: { type: 'string' },
              description: { type: 'string' },
              createdAt: { type: 'string' },
              updatedAt: { type: 'string' },
            },
          },
        },
      },
    },
  },
};
