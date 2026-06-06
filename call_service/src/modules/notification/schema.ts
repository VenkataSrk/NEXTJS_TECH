
// insertGetNotificationSchema
export const insertGetNotificationSchema = {
  body: {
    type: 'object',
    properties: {
      uuid: { type: 'string' },
      toExt: { type: ['number', 'string'] },
      notificationMsg: { type: 'string' },
      channelType: { type: 'string' },
      markAsRead: { type: 'number' },
      type: { type: 'string' },
      timeStamp: { type: 'number' },
      coid: { type: 'number' },
      customerValue: { type: 'string' },
    },
    required: [
      'uuid',
      'toExt',
      'notificationMsg',
      'channelType',
      'markAsRead',
      'type',
      'timeStamp',
      'coid',
      'customerValue',
    ],
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        result: {
          type: 'array',
          items: {
            type: 'object',
            properties: {
              errcode: { type: 'number' },
              errmsg: { type: 'string' },
            },
            required: ['errcode', 'errmsg'],
          },
        },
      },
      required: ['statusCode', 'message', 'result'],
    },
  },
};

// getNotificationSchema
export const getNotificationSchema = {
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        read: {
          type: 'array',
          items: {
            type: 'object',
            properties: {
              nId: { type: 'number' },
              uuid: { type: 'string' },
              domainId: { type: 'number' },
              toExt: { type: 'number' },
              notificationMsg: { type: 'string' },
              markAsRead: { type: 'number' },
              timestamp: { type: 'number' },
              type: { type: 'string' },
              createdAt: { type: 'string', format: 'date-time' },
              updatedAt: { type: 'string', format: 'date-time' },
              channelType: { type: 'string' },
              coid: { type: ['number', 'null'] },
              customerValue: { type: ['string', 'null'] },
              customerName: { type: 'string' },
              primaryPhoneNumber: { type: ['string', 'null'] },
            },
          },
        },
        unRead: {
          type: 'array',
          items: {
            type: 'object',
            properties: {
              nId: { type: 'number' },
              uuid: { type: 'string' },
              domainId: { type: 'number' },
              toExt: { type: 'number' },
              notificationMsg: { type: 'string' },
              markAsRead: { type: 'number' },
              timestamp: { type: 'number' },
              type: { type: 'string' },
              createdAt: { type: 'string', format: 'date-time' },
              updatedAt: { type: 'string', format: 'date-time' },
              channelType: { type: 'string' },
              coid: { type: ['number', 'null'] },
              customerValue: { type: ['string', 'null'] },
              customerName: { type: 'string' },
              primaryPhoneNumber: { type: ['string', 'null'] },
            },
          },
        },
        unReadCount: {
          type: 'array',
          items: {
            type: 'object',
            properties: {
              unReadCount: { type: 'number' },
            },
            required: ['unReadCount'],
          },
        },
        readCount: {
          type: 'array',
          items: {
            type: 'object',
            properties: {
              totalCount: { type: 'number' },
            },
            required: ['totalCount'],
          },
        },
      },
      required: ['statusCode', 'message', 'read', 'unRead', 'unReadCount', 'readCount'],
    },
  },
};

// deleteNotificationSchema
export const deleteNotificationSchema = {
  body: {
    type: 'object',
    properties: {
      nId: { type: 'string' }
    },
    required: ['nId']
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        result: {
          type: 'array',
          items: {
            type: 'object',
            properties: {
              errcode: { type: 'number' },
              errmsg: { type: 'string' }
            },
            required: ['errcode', 'errmsg']
          }
        }
      },
      required: ['statusCode', 'message', 'result']
    }
  }
};

// updatemarkasReadNotificationSchema
export const updatemarkasReadNotificationSchema = {
  body: {
    type: 'object',
    properties: {
      nId: { type: 'number' }
    },
    required: ['nId']
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        result: {
          type: 'array',
          items: {
            type: 'object',
            properties: {
              errcode: { type: 'number' },
              errmsg: { type: 'string' }
            },
            required: ['errcode', 'errmsg']
          }
        }
      },
      required: ['statusCode', 'message', 'result']
    }
  }
};

// updatemarkasReadAllNotificationSchema
export const updatemarkasReadAllNotificationSchema = {
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        result: {
          type: 'array',
          items: {
            type: 'object',
            properties: {
              errcode: { type: 'number' },
              errmsg: { type: 'string' }
            },
            required: ['errcode', 'errmsg']
          }
        }
      },
      required: ['statusCode', 'message', 'result']
    }
  }
};
