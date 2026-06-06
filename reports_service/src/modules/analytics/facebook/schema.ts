export const agentFacebookInboundMetricsFilterScheme = {
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
        AgentFacebookInbound: {
          type: 'array',
          items: {
            properties: {
              TotalReceivedCount: { type: 'number' },
              DiffTotalReceivedPercent: { type: 'number' },
              CurrentRespondedCount: { type: 'number' },
              DiffRespondedPercent: { type: 'number' },
              CurrentAbandonedCount: { type: 'number' },
              DiffAbandonedPercent: { type: 'number' },
            }
          }
        },
        FacebookChartInbound: {
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
              InteractionWithAgent: { type: 'number', },
              totalInteraction: { type: 'number', },
            }
          }
        }
      },
    }
  }
};

export const agentFacebookPerformanceMetricsFilterScheme = {
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
        AgentFacebookPerformance: {
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
        FacebookChartPerformance: {
          type: 'array',
          items: {
            properties: {
              agentName: { type: 'string' },
              AgentIdleTime: { type: 'number' },
              averageResponseTime: { type: 'number' },
              averageHandledTime: { type: 'number' },
              MaximumHandledTime: { type: 'number' },
              totalACWTime: { type: 'number' },
              averageHoldtime: { type: 'number' },
              averageQueueWaitingtime: { type: 'number' },
              maximumQueueWaitingtime: { type: 'number' },
              occupancyrate: { type: 'number', },
              firstChatResolution: { type: 'number', },
              CustomerSatisfaction: { type: 'number', },
            }
          }
        }
      },
    }
  }
};

export const agentFacebookservicelevelMetricsFilterScheme = {
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
        AgentFacebookServicelevel: {
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
        FacebookChartServicelevel: {
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
              Total: { type: 'number' },
              MissedChats: { type: 'number', },
              SLAAchieved: { type: 'number', },
              SLABreached: { type: 'number', },
            }
          }
        }
      },
    }
  }
};
