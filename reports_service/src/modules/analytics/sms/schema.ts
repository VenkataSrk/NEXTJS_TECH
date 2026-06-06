// agentSmsPerformanceMetricsFilterScheme
export const agentSmsPerformanceMetricsFilterScheme = {
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
        AllAgentSmsPerformance: {
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
        SmsChartPerformance: {
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
              SentimentalScore: { type: 'number' },
            }
          }
        }
      },
    }
  }
};

// agentSmsServicelevelMetricsFilterScheme
export const agentSmsServicelevelMetricsFilterScheme = {
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
        AgentSmsServiceLevel: {
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
        SmsChartServiceLevel: {
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
              SLAAchieved: { type: 'number' },
              SLABreached: { type: 'number' },
            }
          }
        }
      },
    }
  }
};

// agentSmsInboundMetricsFilterScheme
export const agentSmsInboundMetricsFilterScheme = {
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
        AllAgentSmsInbound: {
          type: 'array',
          items: {
            properties: {
              TotalReceivedSMS: { type: 'number' },
              DiffTotalPercent: { type: 'number' },
              TotalResponseSMS:  { type: 'number' },
              DiffResponsePercent: { type: 'number' },
              OverdueCount: { type: 'number' },
              DiffOverduePercent: { type: 'number' },
            }
          }
        },
        SmsChartInbound: {
          type: 'array',
          items: {
            properties: {
              agentName: { type: 'string' },
              receivedSMS: { type: 'number' },
              respondedSMS: { type: 'number' },
              Overdue: { type: 'number' },
              missedSMS: { type: 'number' },
              discardedSMS: { type: 'number' },
              transferredSMS: { type: 'number' },
              MaximumSmsHandling: { type: 'number' },
              AverageSmsHandling: { type: 'number' },
              InteractionWithCustomer: { type: 'number' },
              InteractionWithAgent: { type: 'number' },
              totalInteraction: { type: 'number' },
              CustomerSatisfaction: { type: 'number' },
            }
          }
        }
      },
    }
  }
};
