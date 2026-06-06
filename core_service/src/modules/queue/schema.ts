/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

// insertQueueSchema
export const insertQueueSchema = {
  body: {
    type: 'object',
    properties: {
      name: { type: 'string' },
      description: { type: 'string' },
      OutboundCallerId: { type: 'string' },
      outBoundNumber: { type: 'string' },
      maxCallHandlingTimeMM: { type: 'number' },
      callWrapUpTime: { type: 'number' },
      hours_of_operation: { type: 'object' },
      outBoundCallStatus: { type: 'number' },
      settingAudio: { type: 'object' },
      announcement: { type: 'object' },
      maximumCallers: { type: 'object' },
      maximumWaitTime: { type: 'object' },
      hoursOfOperatioId: { type: 'number' },
      outOfBusinessHours: { type: 'object' },
      status: { type: 'number', enum: [0, 1] },
      type: { type: 'string' },
    },
    required: ['name', 'description', 'outBoundNumber', 'maxCallHandlingTimeMM', 'callWrapUpTime', 'hours_of_operation'],
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        insertQueueList: {
          type: 'object',
          properties: {
            qid: { type: 'number' },
            name: { type: 'string' },
            description: { type: 'string' },
            domainId: { type: 'number' },
            OutboundCallerId: { type: 'string' },
            outBoundNumber: { type: 'string' },
            maxCallHandlingTimeMM: { type: 'number' },
            callWrapUpTime: { type: 'number' },
            outBoundCallStatus: { type: 'number' },
            hours_of_operation: { type: 'string' },
            hoursOfOperatioId: { type: 'number' },
            outOfBusinessHours: { type: 'object' },
            status: { type: 'number' },
            maximum_caller_in_queue: { type: 'string' },
            custome_hours: { type: 'string' },
            queue_department_number: { type: 'string' },
            outbound_call_display_name: { type: 'string' },
            outbound_call_phone_number: { type: 'string' },
            outbound_call_flow: { type: 'string' },
            customer_type: { type: 'string' },
            time_zone: { type: 'string' },
            settingAudio: { type: 'string' },
            announcement: { type: 'string' },
            maximumCallers: { type: 'string' },
            maximumWaitTime: { type: 'string' },
            type: { type: 'string' },
            createdAt: { type: 'string' },
            updatedAt: { type: 'string' },
          },
        },
      },
    },
  },
};

// getQueueSchema
export const getQueueSchema = {
  params: {
    type: 'object',
    properties: {
      qid: { type: 'string' },
    },
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getQueueList: {
          type: 'array',
          items: {
            properties: {
              qid: { type: 'number' },
              name: { type: 'string' },
              description: { type: 'string' },
              hours_of_operation: { type: 'string' },
              OutboundCallerId: { type: 'string' },
              outBoundNumber: { type: 'string' },
              maxCallHandlingTimeMM: { type: 'number' },
              maximum_caller_in_queue: { type: 'string' },
              custome_hours: { type: 'string' },
              queue_department_number: { type: 'string' },
              status: { type: 'string' },
              outbound_call_display_name: { type: 'string' },
              outbound_call_phone_number: { type: 'string' },
              outbound_call_flow: { type: 'string' },
              customer_type: { type: 'string' },
              time_zone: { type: 'string' },
              settingAudio: { type: 'string' },
              announcement: { type: 'string' },
              maximumCallers: { type: 'string' },
              maximumWaitTime: { type: 'string' },
              type: { type: 'string' },
              createdAt: { type: 'string' },
              updatedAt: { type: 'string' },
            },
          },
        },
      },
    },
  },
};

// updateQueueSchema
export const updateQueueSchema = {
  params: {
    type: 'object',
    properties: {
      qid: { type: 'number' },
    },
  },
  body: {
    type: 'object',
    properties: {
      name: { type: 'string' },
      description: { type: 'string' },
      OutboundCallerId: { type: 'string' },
      outBoundNumber: { type: 'string' },
      maxCallHandlingTimeMM: { type: 'number' },
      callWrapUpTime: { type: 'number' },
      hours_of_operation: { type: 'object' },
      outBoundCallStatus: { type: 'number' },
      settingAudio: { type: 'object' },
      announcement: { type: 'object' },
      maximumCallers: { type: 'object' },
      maximumWaitTime: { type: 'object' },
      hoursOfOperatioId: { type: 'number' },
      outOfBusinessHours: { type: 'object' },
      status: { type: 'number', enum: [0, 1] },
      type: { type: 'string' },

    },
    required: ['name', 'description', 'outBoundNumber', 'maxCallHandlingTimeMM', 'callWrapUpTime', 'hours_of_operation', 'type'],
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        updateQueueList: {
          type: 'object',
          properties: {
            qid: { type: 'number' },
            name: { type: 'string' },
            description: { type: 'string' },
            domainId: { type: 'number' },
            OutboundCallerId: { type: 'string' },
            outBoundNumber: { type: 'string' },
            maxCallHandlingTimeMM: { type: 'number' },
            callWrapUpTime: { type: 'number' },
            outBoundCallStatus: { type: 'number' },
            hours_of_operation: { type: 'string' },
            hoursOfOperatioId: { type: 'number' },
            outOfBusinessHours: { type: 'object' },
            status: { type: 'number' },
            maximum_caller_in_queue: { type: 'string' },
            custome_hours: { type: 'string' },
            queue_department_number: { type: 'string' },
            outbound_call_display_name: { type: 'string' },
            outbound_call_phone_number: { type: 'string' },
            outbound_call_flow: { type: 'string' },
            customer_type: { type: 'string' },
            time_zone: { type: 'string' },
            settingAudio: { type: 'string' },
            announcement: { type: 'string' },
            maximumCallers: { type: 'string' },
            maximumWaitTime: { type: 'string' },
            createdAt: { type: 'string' },
            updatedAt: { type: 'string' },
            type: { type: 'string' },
          },
        },
      },
    },
  },
};

// updateQueuesSchema
export const updateQueuesSchema = {
  params: {
    type: 'object',
    properties: {
      qid: { type: 'number' },
    },
  },
  body: {
    type: 'object',
    properties: {
      status: { type: 'number', enum: [0, 1] }
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

// deleteQueueSchema
export const deleteQueueSchema = {
  params: {
    type: 'object',
    properties: {
      qid: { type: 'string' },
    },
    required: ['qid'],
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
// getQueueListSchema
export const getQueueListSchema = {
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getQueueLists: {
          type: 'object',
          properties: {
            list: {
              type: 'array',
              items: {
                properties: {
                  qid: { type: 'number' },
                  name: { type: 'string' },
                  description: { type: 'string' },
                  OutboundCallerId: { type: 'string' },
                  outBoundNumber: { type: 'number' },
                  maxCallHandlingTimeMM: { type: 'number' },
                  maxCallHandlingTimeSS: { type: 'number' },
                  callWrapUpTime: { type: 'number' },
                  hours_of_operation: { type: 'object' },
                  outBoundCallStatus: { type: 'number' },
                  settingAudio: { type: 'object' },
                  announcement: { type: 'object' },
                  maximumCallers: { type: 'object' },
                  maximumWaitTime: { type: 'object' },
                  hoursOfOperatioId: { type: 'number' },
                  createdAt: { type: 'string' },
                  updatedAt: { type: 'string' },
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
// updateQueueSchema
export const updateQueueStatusSchema = {
  params: {
    type: 'object',
    properties: {
      qid: { type: 'string' },
    },
  },
  body: {
    type: 'object',
    properties: {
      status: { type: 'string', enum: ['active', 'inactive'] }
    },
    required: [
      'status'
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

// insertCustomHoursSchema
export const insertCustomHoursSchema = {
  body: {
    type: 'object',
    properties: {
      name: { type: 'string' },
      description: { type: 'string' },
      hours: { type: 'array' },
      timeZone: { type: 'string' },
      type: { type: 'string' },
      default_hours: { type: 'string' },
      timeZoneValue: { type: 'string' },
    },
    required: [
      'name',
      'description',
      'hours',
      'timeZone'
    ],
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        insertCustomList: {
          type: 'object',
          properties: {
            cid: { type: 'number' },
            name: { type: 'string' },
            description: { type: 'string' },
            hours: { type: 'string' },
            timeZone: { type: 'string' },
            domainId: { type: 'number' },
            default_hours: { type: 'string' },
            type: { type: 'string' },
            timeZoneValue: { type: 'string' },
            createdAt: { type: 'string' },
            updatedAt: { type: 'string' },
          },
        },
      },
    },
  },
};

// getCustomHoursListSchema
export const getCustomHoursListSchema = {
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getCustomHoursListList: {
          type: 'array',
          items: {
            properties: {
              cid: { type: 'number' },
              name: { type: 'string' },
              description: { type: 'string' },
              hours: { type: 'string' },
              timeZone: { type: 'string' },
              domainId: { type: 'number' },
              type: { type: 'string' },
              default_hours: { type: 'string' },
              timeZoneValue: { type: 'string' },
              createdAt: { type: 'string' },
              updatedAt: { type: 'string' },
            },
          },
        },
      },
    },
  },
};

// getCustomHoursDetailSchema
export const getCustomHoursDetailSchema = {
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getCustomHoursListRes: {
          items: {
            properties: {
              cid: { type: 'number' },
              name: { type: 'string' },
              description: { type: 'string' },
              hours: { type: 'string' },
              timeZone: { type: 'string' },
              default_hours: { type: 'string' },
              timeZoneValue: { type: 'string' },
              type: { type: 'string' },
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

// updateCustomSchema
export const updateCustomSchema = {
  params: {
    type: 'object',
    properties: {
      cid: { type: 'string' },
    },
  },
  body: {
    type: 'object',
    properties: {
      name: { type: 'string' },
      description: { type: 'string' },
      hours: { type: 'array' },
      timeZone: { type: 'string' },
      default_hours: { type: 'string' },
      timeZoneValue: { type: 'string' },
      type: { type: 'string' }
    },
    required: [
      'name',
      'description',
      'hours',
      'timeZone',
      'type'
    ],
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        updateCustomList: {
          type: 'object',
          properties: {
            cid: { type: 'number' },
            name: { type: 'string' },
            description: { type: 'string' },
            hours: { type: 'string' },
            timeZone: { type: 'string' },
            domainId: { type: 'number' },
            default_hours: { type: 'string' },
            timeZoneValue: { type: 'string' },
            type: { type: 'string' },
            createdAt: { type: 'string' },
            updatedAt: { type: 'string' },
          },
        },
      },
    },
  },
};

// deleteCustomHoursSchema
export const deleteCustomHoursSchema = {
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

// checkqueueSchema
export const checkqueueSchema = {
  params: {
    type: 'object',
    properties: {
      qid: { type: 'number' },
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

// customHoursSchema
export const customHoursSchema = {
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

// isValidNameSchema
export const isValidNameSchema = {
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
