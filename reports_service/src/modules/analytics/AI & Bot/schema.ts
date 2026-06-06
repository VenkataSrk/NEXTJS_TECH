export const agentAnalysisBotInteractionScheme = {
  body: {
    type: 'object',
    properties: {
      startTimeStamp: { type: ['number', 'null'] },
      endTimeStamp: { type: ['number', 'null'] },
    },
    required: ['startTimeStamp', 'endTimeStamp']
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
        AIBotInteraction: {
          type: 'array',
          items: {
            properties: {
              activeSession: { type: ['number', 'null'] },
              handOverRate: { type: ['number', 'null'] },
              handledRate: { type: ['number', 'null'] },
              abandonmentRate: { type: ['number', 'null'] },
            }
          }
        }
      },
      required: ['statusCode', 'message', 'AIBotInteraction']
    }
  }
};

export const agentAnalysisBotAnalyticsScheme = {
  body: {
    type: 'object',
    properties: {
      startTimeStamp: { type: ['number', 'null'] },
      endTimeStamp: { type: ['number', 'null'] },
    },
    required: ['startTimeStamp', 'endTimeStamp']
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        AgentAssistAnalysis: {
          type: 'array',
          items: {
            properties: {
              usageRate: { type: ['number', 'null'] },
              accuracyRate: { type: ['number', 'null'] },
              FirstCallResolution: { type: ['number', 'null'] },
              churnPreventionRate: { type: ['number', 'null'] },
            }
          }
        }
      },
      required: ['statusCode', 'message', 'AgentAssistAnalysis']
    }
  }
};

// analyticsReportScheme
export const getAgentAnalyticReq = {
  body: {
    type: 'object',
    properties: {
      startTmpstmp: { type: 'number' },
      endTmpstmp: { type: 'number' },
      channelType: { type: 'string' },
    },
    required: ['startTmpstmp', 'endTmpstmp', 'channelType']
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getAgentAnalyticPerformance: {
          type: 'object',
          properties: {
            firstChatResolutionAgent: { type: 'number' },
            firstChatResolutionBot: { type: 'number' },
            customerSatisfactionRateAgent: { type: 'number' },
            customerSatisfactionRateBot: { type: 'number' },
            transferRateAgent: { type: 'number' },
            transferRateBot: { type: 'number' },
            averageHandlingTimeAgent: { type: 'number' },
            averageHandlingTimeBot: { type: 'number' },
            abandonRateAgent: { type: 'number' },
            abandonRateBot: { type: 'number' },
          }
        }
      },
    },
  },
};
