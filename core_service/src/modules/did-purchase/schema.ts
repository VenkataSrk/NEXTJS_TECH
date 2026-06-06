/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

// insertDidSchema
export const insertDidSchema = {
  body: {
    type: 'object',
    properties: {
      user_id: { type: 'number' },
      type: { type: 'string' },
      city: { type: 'string' },
      your_number: { type: 'number' },
      callFlowId: { type: 'object' },
      recording: { type: 'number', enum: [0, 1] },
      status: { type: 'number', enum: [0, 1] },
      queueName: { type: 'string' },
      isCall: { type: 'number', enum: [0, 1] },
      isSms: { type: 'number', enum: [0, 1] },
      supervisorName: { type: 'string' }
    },
    required: [
      'user_id',
      'type',
      'city',
      'your_number'
    ],
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

// getDidSchema
export const getDidSchema = {
  params: {
    type: 'object',
    properties: {
      pid: { type: 'number' },
    },
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getDidNumberlist: {
          type: 'array',
          items: {
            properties: {
              pid: { type: 'number' },
              user_id: { type: 'number' },
              type: { type: 'string' },
              city: { type: 'string' },
              your_number: { type: 'number' },
              callFlowId: { type: 'string' },
              recording: { type: 'number' },
              status: { type: 'number' },
              queueName: { type: 'string' },
              isCall: { type: 'number', enum: [0, 1] },
              isSms: { type: 'number', enum: [0, 1] },
              createdAt: { type: 'string' },
              updateddAt: { type: 'string' },
              smsThreshold: { type: 'number' },
              aliasName: { type: 'string' },
              outboundSms: { type: 'number' }
            },
          },
        },
      },
    },
  },
};

// updatedidSchema
export const updateDidSchema = {
  params: {
    type: 'object',
    properties: {
      phoneNumbar: { type: 'string' },
    },
  },
  body: {
    type: 'object',
    properties: {
      callFlowId: { type: 'string' },
      recording: { type: 'number', enum: [0, 1] },
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

// deleteDidSchema
export const deleteDidSchema = {
  params: {
    type: 'object',
    properties: {
      pid: { type: 'number' },
    },
    required: ['pid'],
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
// getDidListSchema
export const getDidListSchema = {
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getDidNumberlist: {
          type: 'object',
          properties: {
            list: {
              type: 'array',
              items: {
                properties: {
                  pid: { type: 'number' },
                  user_id: { type: 'number' },
                  type: { type: 'string' },
                  city: { type: 'string' },
                  your_number: { type: 'number' },
                  callFlowId: { type: 'string' },
                  recording: { type: 'number' },
                  status: { type: 'number' },
                  queueName: { type: 'string' },
                  isCall: { type: 'number', enum: [0, 1] },
                  isSms: { type: 'number', enum: [0, 1] },
                  createdAt: { type: 'string' },
                  updateddAt: { type: 'string' },
                  aliasName: { type: 'string' },
                  outboundSms: { type: 'number' },
                  smsThreshold: { type: 'number' },
                  callFlowName: { type: 'string' },
                  defaultUpdated: { type: 'number' },
                  supervisorName: { type: 'string' },
                  isCallIn: { type: 'number', enum: [0, 1] },
                  isCallOut: { type: 'number', enum: [0, 1] },
                  isSmsOut: { type: 'number', enum: [0, 1] },
                  isSmsIn: { type: 'number', enum: [0, 1] }
                },
              },
            },
            total_count: { type: 'number' },
          },
        },
      },
    },
  },
};

// getCountryListSchema
export const getCountryListSchema = {
  body: {
    type: 'object',
    properties: {
      country_id: { type: 'string' },
      landline_type: { type: 'string' },
    },
    required: [
      'country_id',
      'landline_type',
    ],
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getcountryList: {
          type: 'array',
          items: {
            properties: {
              area: { type: 'string' },
              country: { type: 'string' },
              landline_type: { type: 'string' },
              area_code: { type: 'string' },
              state_id: { type: 'number' },

            },
          },
        },
      },
    },
  },
};

// getMobileNumberSchema
export const getMobileNumberSchema = {
  body: {
    type: 'object',
    properties: {
      mobileno: { type: 'string' },
    },
    required: [
      'mobileno',
    ],
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getNumberDetailsList: {
          type: 'array',
          items: {
            properties: {
              last_bill_date: { type: 'string' },
              next_bill_date: { type: 'string' },
              landliPlan_Namene_type: { type: 'string' },
            },
          },
        },
      },
    },
  },
};

// getLandlineListSchema
export const getLandlineListSchema = {
  body: {
    type: 'object',
    properties: {
      country_id: { type: 'string' },
      area: { type: 'string' },
      landline_type: { type: 'string' },
      number_count: { type: 'number' },
      from_range: { type: 'string' },
      to_range: { type: 'string' },
    },
    required: [
      'area',
      'landline_type',
    ],
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getLandlineList: {
          type: 'array',
          items: {
            properties: {
              landline_number: { type: 'string' },
              landline_number_display: { type: 'string' },
            },
          },
        },
      },
    },
  },
};

// insertAvailableSchema
export const insertAvailableSchema = {
  body: {
    type: 'object',
    properties: {
      numbers: { type: 'string' },
      linked_to: { type: 'string' },
      number_type: { type: 'string' },
      activated: { type: 'number' }
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

// getAvailableNumberListSchema
export const getAvailableNumberListSchema = {
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getAvailableNumberList: {
          type: 'array',
          items: {
            properties: {
              aid: { type: 'number' },
              numbers: { type: 'string' },
              linked_to: { type: 'string' },
              number_type: { type: 'string' },
              activated: { type: 'number' },
              domainId: { type: 'number' },
              createdAt: { type: 'string' },
              updatedAt: { type: 'string' },
            },
          },
        },
      },
    },
  },
};
