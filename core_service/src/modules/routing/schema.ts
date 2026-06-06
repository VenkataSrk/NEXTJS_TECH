/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

// insertRoutingSchema
export const insertRoutingSchema = {
  body: {
    type: 'object',
    properties: {
      name: { type: 'string' },
      description: { type: 'string' },
      routingMethod: { type: 'string' },
      concurrentChat: { type: 'number' },
      No_of_linked_queues: { type: 'number' },
      default_outbound_queue: { type: 'number' },
      status: { type: 'number' },
      routingqueuedetails: {
        type: 'array',
        items: {
          properties: {
            rqid: { type: 'number' },
            rqmid: { type: 'number' },
            qid: { type: 'number' },
            chat: { type: 'number' },
            priority: { type: 'number' },
            delay_in_seconds: { type: 'number' },
            createdAt: { type: 'string' },
            updateddAt: { type: 'string' },
            voice: { type: 'number' }
          },
        },
      }
    },
    required: [
      'name',
      'description',
      'No_of_linked_queues',
      'status',
      'routingMethod',
      'concurrentChat'
    ],
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
            rid: { type: 'number' },
            name: { type: 'string' },
            description: { type: 'string' },
            routingMethod: { type: 'string' },
            concurrentChat: { type: 'number' },
            No_of_linked_queues: { type: 'number' },
            default_outbound_queue: { type: 'number' },
            status: { type: 'number' },
            routingqueue: {
              type: 'array',
              items: {
                properties: {
                  rqid: { type: 'number' },
                  rqmid: { type: 'number' },
                  qid: { type: 'number' },
                  chat: { type: 'number' },
                  priority: { type: 'number' },
                  delay_in_seconds: { type: 'number' },
                  createdAt: { type: 'string' },
                  updateddAt: { type: 'string' },
                  voice: { type: 'number' }
                },
              },
            },
          },
        },
      },
    },
  },

};

// getRoutingSchema
export const getRoutingSchema = {
  params: {
    type: 'object',
    properties: {
      rid: { type: 'number' },
    },
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getRoutingReslist: {
          type: 'object',
          properties: {
            routing: {
              type: 'array',
              items: {
                properties: {
                  rid: { type: 'number' },
                  name: { type: 'string' },
                  description: { type: 'string' },
                  routingMethod: { type: 'string' },
                  concurrentChat: { type: 'number' },
                  No_of_linked_queues: { type: 'number' },
                  default_outbound_queue: { type: 'number' },
                  status: { type: 'number' },
                  type: { type: 'string' },
                  createdAt: { type: 'string' },
                  updatedAt: { type: 'string' },
                },
              },
            },
            routingqueuedetails: {
              type: 'array',
              items: {
                properties: {
                  rqid: { type: 'number' },
                  rqmid: { type: 'number' },
                  qid: { type: 'number' },
                  chat: { type: 'number' },
                  priority: { type: 'number' },
                  delay_in_seconds: { type: 'number' },
                  createdAt: { type: 'string' },
                  updateddAt: { type: 'string' },
                  voice: { type: 'number' }
                },
              },
            }
          }
        },
      },
    },
  },
};

// updateRoutingSchema
export const updateRoutingSchema = {
  params: {
    type: 'object',
    properties: {
      qid: { type: 'string' },
    },
  },
  body: {
    type: 'object',
    properties: {
      name: { type: 'string' },
      description: { type: 'string' },
      routingMethod: { type: 'string' },
      concurrentChat: { type: 'number' },
      No_of_linked_queues: { type: 'number' },
      default_outbound_queue: { type: 'number' },
      status: { type: 'number' },
      routing_type: { type: 'string' },
      routingqueue: {
        type: 'array',
        items: {
          properties: {
            qid: { type: 'number' },
            chat: { type: 'number' },
            priority: { type: 'number' },
            delay_in_seconds: { type: 'number' },
            voice: { type: 'number' }
          },
        },
      },
    },
    required: [
      'name',
      'description',
      'No_of_linked_queues',
      'status',
      'routingMethod',
      'concurrentChat'
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
// deleteRoutingSchema
export const deleteRoutingSchema = {
  params: {
    type: 'object',
    properties: {
      rid: { type: 'string' },
    },
    required: ['rid'],
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
  params: {
    type: 'object',
    properties: {
      accountId: { type: 'number' },
    },
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getQueueLists: {
          type: 'array',
          items: {
            properties: {
              qid: { type: 'number' },
              name: { type: 'string' },
              description: { type: 'string' },
              routingMethod: { type: 'string' },
              concurrentChat: { type: 'number' },
              hours_of_operation: { type: 'string' },
              maximum_caller_in_queue: { type: 'string' },
              custome_hours: { type: 'string' },
              queue_department_number: { type: 'string' },
              createdAt: { type: 'string' },
              updatedAt: { type: 'string' },
            },
          },
        },
      },
    },
  },
};
// getRoutingListSchema
export const getRoutingListSchema = {
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getRoutingListRes: {
          type: 'array',
          items: {
            properties: {
              rid: { type: 'number' },
              name: { type: 'string' },
              description: { type: 'string' },
              routingMethod: { type: 'string' },
              concurrentChat: { type: 'number' },
              default_outbound_queue: { type: 'string' },
              No_of_linked_queues: { type: 'string' },
              no_of_agent: { type: 'number' },
              status: { type: 'string' },
              routing_type: { type: 'string' },
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

// Work contacts functionalty
export const getWorkContactRes = {
  querystring: {
    type: 'object',
    properties: {
      timestamp: { type: 'number' },
    },
  },
  200: {
    type: 'object',
    properties: {
      statusCode: { type: 'number' },
      message: { type: 'string' },
      companyContacts: {
        type: 'array',
        items: {
          properties: {
            id: { type: 'number' },
            mobileno: { type: 'string' },
            ext: { type: 'number' },
            caller_id: { type: 'string' },
            login_user_name: { type: 'string' },
            sip_login_id: { type: 'number' },
            tmestmp: { type: 'number' },
            user_status: { type: 'string' },
            email_id: { type: 'string' },
            status_msg: { type: 'string' },
            company_id: { type: 'number' },
            direct_no: { type: 'string' },
            ImageURL: { type: 'string' },
            first_name: { type: 'string' },
            last_name: { type: 'string' },
            company_name: { type: 'string' },
            is_muted: { type: 'number' },
            is_blocked: { type: 'number' },
            is_favourite: { type: 'number' },
            department: { type: 'string' },
            role_name: { type: 'string' },
            status: { type: 'string' },
            is_archive: { type: 'number' },
            muted_info: { type: 'number' },
            is_notification: { type: 'number' },
            JID: { type: 'string' },
            is_notification_info: { type: 'string' },
            jid: { type: 'string' },
            createdate: { type: 'string' },
            mute_timestamp: { type: 'string' },
            status_indicator: { type: 'number' },
            time_duration: { type: 'number' },
            prev_status: { type: 'string' },
            userstatus_timestamp: { type: 'string' },
          },
        },
      },
    },
  },
};

// getUserList
export const getWorkContactReqSchema = {
  querystring: {
    type: 'object',
    properties: {
      roleId: { type: 'string' },
    },
    required: ['roleId'],
  },
};

// getUserList
export const getWorkContactDetailsReqSchema = {
  querystring: {
    type: 'object',
    properties: {
      username: { type: 'string' },
    },
    required: ['username'],
  },
};

// Work contacts functionalty
export const getWorkContactDetail = {
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        companyContacts: {
          type: 'object',
          properties: {
            id: { type: 'number' },
            mobileno: { type: 'string' },
            ext: { type: 'number' },
            caller_id: { type: 'string' },
            login_user_name: { type: 'string' },
            sip_login_id: { type: 'number' },
            tmestmp: { type: 'number' },
            user_status: { type: 'string' },
            email_id: { type: 'string' },
            status_msg: { type: 'string' },
            company_id: { type: 'number' },
            direct_no: { type: 'string' },
            ImageURL: { type: 'string' },
            first_name: { type: 'string' },
            last_name: { type: 'string' },
            company_name: { type: 'string' },
            is_muted: { type: 'number' },
            is_blocked: { type: 'number' },
            is_favourite: { type: 'number' },
            department: { type: 'string' },
            role_name: { type: 'string' },
            status: { type: 'string' },
            is_archive: { type: 'number' },
            muted_info: { type: 'number' },
            is_notification: { type: 'number' },
            JID: { type: 'string' },
            is_notification_info: { type: 'string' },
            jid: { type: 'string' },
            createdate: { type: 'string' },
            mute_timestamp: { type: 'string' },
            status_indicator: { type: 'number' },
            time_duration: { type: 'number' },
            prev_status: { type: 'string' },
            userstatus_timestamp: { type: 'string' },
          },
        },
      },
    },
  }
};

// routingProfileStatusSchema
export const routingProfileStatusSchema = {
  params: {
    type: 'object',
    properties: {
      rid: { type: 'string' },
    },
    required: ['rid'],
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        status: { type: 'number' }
      },
    },
  },
};

// updateStatusSchema
export const updateStatusSchema = {
  params: {
    type: 'object',
    properties: {
      rid: { type: 'number' },
    },
    required: ['rid'],
  },
  body: {
    type: 'object',
    properties: {
      status: { type: 'number', enum: [0, 1] },
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

// isEnableListSchema
export const isEnableListSchema = {
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getRoutingEnableListRes: {
          type: 'array',
          properties: {
            name: { type: 'string' },
            description: { type: 'string' },
            routingMethod: { type: 'string' },
            concurrentChat: { type: 'number' },
            No_of_linked_queues: { type: 'number' },
            default_outbound_queue: { type: 'number' },
            status: { type: 'number' },
          },
        },
      },
    },
  },
};
