export const agentWhatsappInboundMetricsFilterScheme = {
  body: {
    type: 'object',
    properties: {
      startTimeStamp: { type: ['number', 'null'] },
      endTimeStamp: { type: ['number', 'null'] },
      offset: { type: 'number' },
      limit: { type: 'number' },
    },
    required: ['offset', 'limit', 'startTimeStamp', 'endTimeStamp']
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        AgentDetails: {
          type: 'array',
          items: {
            properties: {
              UserID: { type: 'number' },
              UserName: { type: 'string' },
              ext: { type: 'number' }
            }
          }
        },
        AgentWhatsappInbound: {
          type: 'array',
          items: {
            properties: {
              TotalReceivedCount: { type: 'number' },
              DiffTotalReceivedPercent: { type: 'number' },
              CurrentAnsweredeCount: { type: 'number' },
              DiffAnsweredPercent: { type: 'number' },
              CurrentAbandonedCount: { type: 'number' },
              DiffAbandonedPercent: { type: 'number' },
            }
          }
        },
        WhatsappChartInbound: {
          type: 'array',
          items: {
            properties: {
              agentName: { type: 'string' },
              receivedChat: { type: 'number' },
              respondedChat: { type: 'number' },
              abandonedChat: { type: 'number' },
              missedChat: { type: 'number' },
              transferredChat: { type: 'number' },
              TotalChatDuration: { type: 'number' },
              AverageChatDuration: { type: 'number' },
              InteractionWithCustomer: { type: 'number' },
              InteractionWithAgent: { type: 'number' },
              totalInteraction: { type: 'number' },
            }
          }
        }
      },
    }
  }
};

export const agentWhatsappPerformancesMetricsFilterScheme = {
  body: {
    type: 'object',
    properties: {
      startTimeStamp: { type: ['number', 'null'] },
      endTimeStamp: { type: ['number', 'null'] },
      offset: { type: 'number' },
      limit: { type: 'number' },
    },
    required: ['offset', 'limit', 'startTimeStamp', 'endTimeStamp']
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        AgentDetails: {
          type: 'array',
          items: {
            properties: {
              UserID: { type: 'number' },
              UserName: { type: 'string' },
              ext: { type: 'number' }
            }
          }
        },
        AgentWhatsappPerformances: {
          type: 'array',
          items: {
            properties: {
              CurrentaverageResponseCount: { type: 'number' },
              DiffAverageResponsePercent: { type: 'number' },
              CurrentAverageHandledCount: { type: 'number' },
              DiffAverageHandledPercent: { type: 'number' },
              CurrentCustomerSatisfactionCount: { type: 'number' },
              DiffCustomerSatisfactionPercent: { type: 'number' },
            }
          }
        },
        WhatsappChartPerformances: {
          type: 'array',
          items: {
            properties: {
              agentName: { type: 'string' },
              averageResponseTime: { type: 'number' },
              averageHandledTime: { type: 'number' },
              maximumHandleTime: { type: 'number' },
              totalACWTime: { type: 'number' },
              averageHoldtime: { type: 'number' },
              firstChatResolution: { type: 'number' },
              CustomerSatisfaction: { type: 'number' },
              AgentIdleTime: { type: 'number' },
              averageQueueWaitingtime: { type: 'number' },
              maximumQueueWaitingtime: { type: 'number' },
              occupancyrate:  { type: 'number' },
            }
          }
        }
      },
    }
  }
};

export const agentWhatsappServicelevelMetricsFilterScheme = {
  body: {
    type: 'object',
    properties: {
      startTimeStamp: { type: ['number', 'null'] },
      endTimeStamp: { type: ['number', 'null'] },
      offset: { type: 'number' },
      limit: { type: 'number' },
    },
    required: ['offset', 'limit', 'startTimeStamp', 'endTimeStamp']
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        AgentDetails: {
          type: 'array',
          items: {
            properties: {
              UserID: { type: 'number' },
              UserName: { type: 'string' },
            }
          }
        },
        AgentWhatsappServicelevel: {
          type: 'array',
          items: {
            properties: {
              CurrentAnsweredSLACount: { type: 'number' },
              DiffAnsweredSLAPercent: { type: 'number' },
              CurrentAbandonedSLACount: { type: 'number' },
              DiffAbandonedSLAPercent: { type: 'number' },
              CurrentSLAbreachedCount: { type: 'number' },
              DiffSLAbreachedPercent: { type: 'number' },
            }
          }
        },
        WhatsappChartServicelevel: {
          type: 'array',
          items: {
            properties: {
              agentName: { type: 'string' },
              ChatType: { type: 'string' },
              SL1: { type: 'number' },
              SL2: { type: 'number' },
              SL3: { type: 'number' },
              SL4: { type: 'number' },
              SL5: { type: 'number' },
              SL6: { type: 'number' },
              Total:  { type: 'number' },
              MissedChats:  { type: 'number' },
              SLAAchieved:  { type: 'number' },
              SLABreached:  { type: 'number' },
            }
          }
        }
      },
    }
  }
};
