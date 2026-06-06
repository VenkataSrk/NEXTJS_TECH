/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

// insertCustomerTagSchema
export const insertCustomerTagSchema = {
  body: {
    type: 'object',
    properties: {
      customerTagName: { type: 'string' },
      description: { type: 'string' },
      status: { type: 'number', enum: [0, 1] }
    },
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        insertCustomerTagList: {
          type: 'object',
          properties: {
            cid: { type: 'number' },
            customerTagName: { type: 'string' },
            type: { type: 'string' },
            description: { type: 'string' },
            status: { type: 'number' },
            createdAt: { type: 'string' },
            updatedAt: { type: 'string' },
            domainId: { type: 'number' },
          },
        },
      },
    },
  },
};

// getCustomerTagSchema
export const getCustomerTagSchema = {
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getCustomerTagStatus: { type: 'number' },
        getCustomerTagListRes: {
          type: 'array',
          items: {
            properties: {
              cid: { type: 'number' },
              customerTagName: { type: 'string' },
              type: { type: 'string' },
              description: { type: 'string' },
              status: { type: 'number' },
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

// getCustomerTagDetailschema
export const getCustomerTagDetailschema = {
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getCustomerTagDetailsRes: {
          type: 'array',
          items: {
            properties: {
              cid: { type: 'number' },
              customerTagName: { type: 'string' },
              type: { type: 'string' },
              description: { type: 'string' },
              status: { type: 'number' },
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

// updateCustomerTagSchema
export const updateCustomerTagSchema = {
  params: {
    type: 'object',
    properties: {
      cid: { type: 'number' },
    },
  },
  body: {
    type: 'object',
    properties: {
      customerTagName: { type: 'string' },
      description: { type: 'string' },
      status: { type: 'number', enum: [0, 1] }
    },
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        updateCustomerTagRes: {
          type: 'object',
          properties: {
            cid: { type: 'number' },
            customerTagName: { type: 'string' },
            type: { type: 'string' },
            description: { type: 'string' },
            status: { type: 'number' },
            createdAt: { type: 'string' },
            domainId: { type: 'number' },
          },
        },
      },
    },
  },
};

// deleteCustomerTagSchema
export const deleteCustomerTagSchema = {
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

// customerTagSchema
export const customerTagSchema = {
  body: {
    type: 'object',
    properties: {
      customerTag: { type: 'number', enum: [0, 1] },
    },
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' }
      },
    },
  },
};

// getCustomerTagListSchema
export const getCustomerTagListSchema = {
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getCustomerTagRes: {
          type: 'array',
          items: {
            properties: {
              did: { type: 'number' },
              customerTag: { type: 'number' },
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
