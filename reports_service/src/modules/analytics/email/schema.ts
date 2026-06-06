// agentEmailInboundMetricsFilterScheme
export const agentEmailInboundMetricsFilterScheme = {
  body: {
    type: 'object',
    properties: {
      startTimeStamp: { type: ['number', 'null'] },
      endTimeStamp: { type: ['number', 'null'] },
      offset: { type: 'number' },
      limit: { type: 'number' },
      UserName: { type: ['string', 'null'] }
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
        AllAgentEmailInbound: {
          type: 'array',
          items: {
            properties: {
              TotalCount: { type: 'number' },
              DiffTotalPercent: { type: 'number' },
              CurrentResponseCount: { type: 'number' },
              DiffResponsePercent: { type: 'number' },
              CurrentOverdueCount: { type: 'number' },
              DiffOverduePercent: { type: 'number' },
            }
          }
        },
        EmailInboundChart: {
          type: 'array',
          items: {
            properties: {
              agentName: { type: 'string' },
              receivedEmail: { type: 'number' },
              OpenedEmail: { type: 'number' },
              respondedEmail: { type: 'number' },
              missedEmail: { type: 'number' },
              OverdueCount: { type: 'number' },
              DiscardedEmail: { type: 'number' },
              transferredEmail: { type: 'number' },
              maximumEmailHandlingTime: { type: 'number' },
              AverageEmailHandlingTime: { type: 'number', },
              InteractionsFromCustomer: { type: 'number', },
              InteractionsFromAgent: { type: 'number', },
              totalInteractions: { type: 'number', },
              CustomerSatisfaction: { type: 'number', },
            }
          }
        }
      },
    }
  }
};

// agentEmailPerformanceMetricsFilterScheme
export const agentEmailPerformanceMetricsFilterScheme = {
  body: {
    type: 'object',
    properties: {
      startTimeStamp: { type: ['number', 'null'] },
      endTimeStamp: { type: ['number', 'null'] },
      offset: { type: 'number' },
      limit: { type: 'number' },
      UserName: { type: ['string', 'null'] }
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
        AllAgentEmailPerformance: {
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
        EmailChartPerformance: {
          type: 'array',
          items: {
            properties: {
              agentName: { type: 'string' },
              averageResponseTime: { type: 'number' },
              averageHandledTime: { type: 'number' },
              MaximumHandledTime: { type: 'number' },
              totalACWTime: { type: 'number' },
              averageHoldtime: { type: 'number' },
              firstChatResolution: { type: 'number' },
              CustomerSatisfaction: { type: 'number' },
            }
          }
        }
      },
    }
  }
};

// agentEmailServicelevelMetricsFilterScheme
export const agentEmailServicelevelMetricsFilterScheme = {
  body: {
    type: 'object',
    properties: {
      startTimeStamp: { type: ['number', 'null'] },
      endTimeStamp: { type: ['number', 'null'] },
      offset: { type: 'number' },
      limit: { type: 'number' },
      UserName: { type: ['string', 'null'] }
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
        AllagentEmailServicelevel: {
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
        EmailServicelevelChart: {
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
              Overdue: { type: 'number' },
              SLAAchieved: { type: ['number', 'null'] },
              SLABreached: { type: ['number', 'null'] },
            }
          }
        }
      },
    }
  }
};
