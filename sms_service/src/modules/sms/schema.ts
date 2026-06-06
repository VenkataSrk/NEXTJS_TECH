export const callflowSchema = {
  body: {
    type: 'object',
    properties: {
      templateName: { type: 'string' },
      nodes: { type: 'array' },
      edges: { type: 'array' },
      status: { type: 'number', enum: [1, 2] },
      description: { type: 'string' }
    },
    required: ['templateName', 'nodes', 'edges', 'status'],
  },
  defaultRes: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        status: { type: 'string' },
        result: { type: 'object' },
        message: { type: 'string' },
      },
    },
  },
};

export const callflowlist = {
  defaultRes: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        status: { type: 'string' },
        result: { type: 'array' },
        message: { type: 'string' },
      },
    },
  },
};

export const callflowsdetails = {
  params: {
    type: 'object',
    properties: {
      cfid: { type: 'string' }
    },
    required: ['cfid'],
  },
  defaultRes: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        status: { type: 'string' },
        result: { type: 'array' },
        message: { type: 'string' },
      },
    },
  },
};

// sendSmsSchema
export const sendSmsSchema = {
  body: {
    type: 'object',
    properties: {
      sessionId: { type: 'string' },
      from: { type: 'string' },
      to: { type: 'string' },
      text: { type: 'string' },
      agentName:{ type:'string' }
    },
    required: ['sessionId', 'from', 'to', 'text'],
    additionalProperties: false, // disallow unexpected fields
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
      },
      required: ['statusCode', 'message'],
      additionalProperties: false,
    },
  },
};
