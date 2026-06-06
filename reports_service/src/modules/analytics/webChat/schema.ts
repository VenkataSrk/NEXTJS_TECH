// getAnalyticsQueueChatSchema
export const getAnalyticsQueueChatReportScheme = {
  body: {
    type: 'object',
    properties: {
      startTmpstmp: { type: 'number' },
      endTmpstmp: { type: 'number' },
      timeZone: { type: 'string' },
      domainId: { type: 'number' },
      queuName: { type: 'string' },
    },
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getAnalyticsQueueChatReportRes: {
          type: 'array',
          items: {
            properties: {
              date: { type: 'date' },
              queueName: { type: 'string' },
              chatPresented: { type: 'number' },
              queueTimeAverage: { type: 'date' },
              queueTimeMax: { type: 'date' },
              chatHandled: { type: 'number' },
              averageHandledTime: { type: 'date' },
              maxHandledTime: { type: 'date' },
              chatsAbondened: { type: 'number' }
            },
          },
        },
      },
    },
  },
};

// getAnalyticsSkillChatSchema
export const getAnalyticsSkillChatReportScheme = {
  body: {
    type: 'object',
    properties: {
      startTmpstmp: { type: 'number' },
      endTmpstmp: { type: 'number' },
      timeZone: { type: 'string' },
      domainId: { type: 'number' },
      queuName: { type: 'string' },
      agentName: { type: 'string' },
      skillName: { type: 'string' },
      source: { type: 'string' },
    },
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getAnalyticsSkillChatReportRes: {
          type: 'array',
          items: {
            properties: {
              date: { type: 'date' },
              totalChat: { type: 'number' },
              queueName: { type: 'string' },
              averageQueueWaitingTime: { type: 'date' },
              agentName: { type: 'string' },
              totalInteractionTime: { type: 'date' },
              averageResponseTime: { type: 'date' },
              totalACW: { type: 'date' },
              source: { type: 'string' }
            },
          },
        },
      },
    },
  },
};

// getAnalyticsSkillChatSchema
export const getAnalyticsChatSessionReportScheme = {
  body: {
    type: 'object',
    properties: {
      startTmpstmp: { type: 'number' },
      endTmpstmp: { type: 'number' },
      domainId: { type: 'number' }
    },
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getAnalyticsChatSessionReportRes: {
          type: 'array',
          items: {
            properties: {
              date: { type: 'date' },
              totalChat: { type: 'number' },
              chatAnswered: { type: 'number' },
              chatMissed: { type: 'numnber' },
              chatTransferred: { type: 'numnber' },
              percentageOfChatsAnswered: { type: 'numnber' },
              percentageOfChatsMissed: { type: 'numnber' },
              totalMessageFromCustomer: { type: 'numnber' },
              totalMessageFromAgent: { type: 'numnber' },
              averageSessionTime: { type: 'date' }
            },
          },
        },
      },
    },
  },
};

// agentInboundChatAnalysisMetricsFilterScheme
export const agentInboundChatAnalysisMetricsFilterScheme = {
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
            }
          }
        },
        AllAgentInboundChat: {
          type: 'array',
          items: {
            properties: {
              TotalCount: { type: 'number' },
              DiffTotalPercent: { type: 'number' },
              CurrentResponseCount: { type: 'number' },
              DiffResponsePercent: { type: 'number' },
              CurrentAbandonedCount: { type: 'number' },
              DiffAbandonedPercent: { type: 'number' },
            }
          }
        },
        AgentInboundChatChart: {
          type: 'array',
          items: {
            properties: {
              agentName: { type: 'string' },
              receivedChat: { type: 'number' },
              respondedChat: { type: 'number' },
              abandonedChat: { type: 'number' },
              missedChat: { type: 'number' },
              transferredChat: { type: 'number' },
              totalChatDuration: { type: 'number' },
              avgChatDuration: { type: 'number' },
              customerMsgCount: { type: 'number' },
              agentMsgCount: { type: 'number' },
              totalMsgCount: { type: 'number' },
            }
          }
        }
      },
    }
  }
};

// agentChatPerformanceMetricsFilterScheme
export const agentChatPerformanceMetricsFilterScheme = {
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
        AllAgentChatPerformance: {
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
        AgentChartPerformance: {
          type: 'array',
          items: {
            properties: {
              agentName: { type: 'string' },
              agentIdleTimeCount: { type: 'number' },
              averageResponseTime: { type: 'number' },
              averageHandledTime: { type: 'number' },
              MaximumHandledTime: { type: 'number' },
              totalACWTime: { type: 'number' },
              averageHoldtime: { type: 'number' },
              averageQueueWaittime: { type: 'number' },
              maximumQueueWaittime: { type: 'number' },
              occupancyRate: { type: 'number' },
              firstChatResolution: { type: 'number' },
              SentimentalScore: { type: 'number' },
            }
          }
        }
      },
    }
  }
};

// agentChatServicelevelMetricsFilterScheme
export const agentChatServicelevelMetricsFilterScheme = {
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
            type: 'object',
            properties: {
              UserID: { type: 'number' },
              UserName: { type: 'string' }
            },
            required: ['UserID', 'UserName']
          }
        },
        AllagentChatServicelevel: {
          type: 'array',
          items: {
            type: 'object',
            properties: {
              CurrentAnsweredSLACount: { type: 'number' },
              DiffAnsweredSLAPercent: { type: 'number' },
              CurrentAbandonedSLACount: { type: 'number' },
              DiffAbandonedSLAPercent: { type: 'number' },
              CurrentSLAbreachedCount: { type: 'number' },
              DiffSLAbreachedPercent: { type: 'number' }
            }
          }
        },
        ChatServicelevelChart: {
          type: 'array',
          items: {
            type: 'object',
            properties: {
              agentName: { type: 'string' },
              ChatType: { type: 'string' },
              SL1: { type: 'number' },
              SL2: { type: 'number' },
              SL3: { type: 'number' },
              SL4: { type: 'number' },
              SL5: { type: 'number' },
              SL6: { type: 'number' },
              TotalSLA: { type: 'number' },
              MissedChats: { type: 'number' },
              SLAAchieved: { type: 'number' },
              SLAbreached: { type: 'number' },
            },
            required: ['agentName', 'SL1', 'SL2', 'SL3', 'SL4', 'SL5', 'SL6']
          }
        }
      }
    }
  }
};
