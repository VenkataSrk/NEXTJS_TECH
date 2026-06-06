// chatReportScheme
export const chatReportScheme = {
  body: {
    type: 'object',
    properties: {
      startTmpstmp: { type: 'number' },
      endTmpstmp: { type: 'number' },
      timeZone: { type: 'string' },
      offset: { type: 'number' },
      limit: { type: 'number' },
      search: { type: 'string' },
    },
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        totalCount: { type: 'number' },
        getChatSessionReportRes: {
          type: 'array',
          items: {
            properties: {
              Date: { type: 'string' },
              AgentName: { type: 'string' },
              ChatStartTime: { type: 'number' },
              ChatEndTime: { type: 'number' },
              Duration: { type: 'number' },
              CustomerName: { type: 'string' },
              ChatRoutedQueue: { type: 'string' },
              ActiveChatTime: { type: 'number' },
              ResponseTime: { type: 'number' },
              DispositionStatus: { type: 'string' },
              ChatStatus: { type: 'string' },
              QueueWaitingTime: { type: 'number' },
              AcwTime: { type: 'number' },
            },
          },
        },
      },
    },
  },
};

// getAgentChatScheme
export const getAgentChatScheme = {
  body: {
    type: 'object',
    properties: {
      startTmpstmp: { type: 'number' },
      endTmpstmp: { type: 'number' },
      timeZone: { type: 'string' },
      domainId: { type: 'number' },
      limit: { type: 'number' },
      offset: { type: 'number' },
      // search: { type: 'string' },
      queueName: { type: 'string' },
      agentName: { type: 'string' },
      callType: { type: 'string' },
      Disposition: { type: 'string' },
    },
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        totalCount: { type: 'number' },
        getAgentChatDetailsReportRes: {
          type: 'array',
          items: {
            properties: {
              Date: { type: 'string' },
              queueName: { type: 'string' },
              agentName: { type: 'string' },
              totalReceived: { type: 'number' },
              totalHandled: { type: 'number' },
              totalTransferred: { type: 'number' },
              totalMissed: { type: 'number' },
              ActiveChatTime: { type: 'number' },
              MaxActiveChatTime: { type: 'number' },
              AvgActiveChatTime: { type: 'number' },
              MaxResponseTime: { type: 'number' },
              AvgResponseChatTime: { type: 'number' },
            },
          },
        },
      },
    },
  },
};

// getACWChatSchema
export const getACWChatScheme = {
  body: {
    type: 'object',
    properties: {
      startTmpstmp: { type: 'number' },
      endTmpstmp: { type: 'number' },
      timeZone: { type: 'string' },
      domainId: { type: 'number' },
      queuName: { type: 'string' },
      agentName: { type: 'string' }
    },
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getAgentACWChatReportRes: {
          type: 'array',
          items: {
            properties: {
              date: { type: 'date' },
              agentName: { type: 'string' },
              queueName: { type: 'string' },
              totalInteraction: { type: 'number' },
              totalActiveChatTime: { type: 'date' },
              averageActiveChatTime: { type: 'date' },
              totalACWTime: { type: 'date' },
              averageACWTime: { type: 'date' }
            },
          },
        },
      },
    },
  },
};

// getAgentQueueChatSchema
export const getAgentQueueChatScheme = {
  body: {
    type: 'object',
    properties: {
      startTmpstmp: { type: 'number' },
      endTmpstmp: { type: 'number' },
      timeZone: { type: 'string' },
      domainId: { type: 'number' },
      queuName: { type: 'string' },
      agentName: { type: 'string' }
    },
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getAgentQueueChatReportRes: {
          type: 'array',
          items: {
            properties: {
              date: { type: 'date' },
              agentName: { type: 'string' },
              chatReceived: { type: 'number' },
              chatHandled: { type: 'number' },
              averageActiveChatTime: { type: 'date' },
              chatNoAnswer: { type: 'number' },
              averageResponseTime: { type: 'date' }
            },
          },
        },
      },
    },
  },
};

// getChatInteractionScheme
export const getChatInteractionScheme = {
  body: {
    type: 'object',
    properties: {
      startTmpstmp: { type: 'number' },
      endTmpstmp: { type: 'number' },
      timeZone: { type: 'string' },
      agentName: { type: 'string' },
      queueName: { type: 'string' },
      domainId: { type: 'number' },
      offset: { type: 'number' },
      limit: { type: 'number' }
    },
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getAgentChatDetailsReportRes: {
          type: 'array',
          items: {
            properties: {
              Date: { type: 'string' },
              source: { type: 'string' },
              CustomerName: { type: 'string' },
              AgentName: { type: 'string' },
              Direction: { type: 'string' },
              TotalDuration: { type: 'number' },
              QueueName: { type: 'string' },
              QueueWaitTime: { type: 'number' },
              DispositionStatus: { type: 'number' },
              MaxResponseTime: { type: 'string' },
              AcwTime: { type: 'number' },
            },
          },
        },
      },
    },
  },
};
