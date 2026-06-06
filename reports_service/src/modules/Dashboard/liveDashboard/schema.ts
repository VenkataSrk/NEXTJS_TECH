// getCallLiveDashbordScheme
export const getCallLiveDashbordScheme = {
  body: {
    type: 'object',
    properties: {
      startTmpstmp: { type: 'number' },
      endTmpstmp: { type: 'number' },
      Timezone: { type: 'string' }
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

// analyticsReportScheme
export const getDashBoardReq = {
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
        getChatLiveDashboardDetails: {
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

// Response Rating
export const getResponseRating = {
  body: {
    type: 'object',
    properties: {
      startTmpstmp: { type: ['number', 'null'] },
      endTmpstmp: { type: ['number', 'null'] },
      channelType: { type: ['string', 'null'] },
    },
    required: ['startTmpstmp', 'endTmpstmp']
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        responseRating: {
          type: 'array',
          items: {
            type: 'object',
            properties: {
              result: { type: 'string' },
              ratings: { type: 'number' },
              percentage: { type: 'number' },
            }
          }
        }
      },
    },
  },
};

// Agent Assistance
export const getAgentAssistance = {
  body: {
    type: 'object',
    properties: {
      startTmpstmp: { type: ['number', 'null'] },
      endTmpstmp: { type: ['number', 'null'] },
      channel: { type: ['string', 'null'] },
    },
    required: ['startTmpstmp', 'endTmpstmp', 'channel']
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        responseRating: {
          type: 'array',
          items: {
            type: 'object',
            properties: {
              result: { type: 'string' },
              ratings: { type: 'number' },
              percentage: { type: 'number' },
            },
            required: ['result', 'ratings', 'percentage']
          }
        }
      },
    }
  }
};

export const getSurveyResponse = {
  body: {
    type: 'object',
    properties: {
      startTmpstmp: { type: ['number', 'null'] },
      endTmpstmp: { type: ['number', 'null'] },
    },
    required: ['startTmpstmp', 'endTmpstmp']
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        SurveyResponse: {
          type: 'array',
          items: {
            type: 'object',
            properties: {
              totalSent: { type: 'number' },
              responded: { type: 'number' },
              notResponded: { type: 'number' },
              responsePercentage: { type: 'number' },
            },
            required: ['totalSent', 'responded', 'notResponded', 'responsePercentage']
          }
        }
      },
    }
  }
};
