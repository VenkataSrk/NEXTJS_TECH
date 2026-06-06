
export const queueWebchatAnalyticsInboundMetricsFilterScheme = {
  body: {
    type: 'object',
    properties: {
      startTmpstmp: { type: ['number', 'null'] },
      endTmpstmp: { type: ['number', 'null'] },
      offset: { type: 'number' },
      limit: { type: 'number' },
    },
    required: ['offset', 'limit', 'startTmpstmp', 'endTmpstmp']
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        QueueDetails: {
          type: 'array',
          items: {
            properties: {

              qid: { type: 'number' },
              queueName: { type: 'string' },
            }
          }
        },
        QueueWebchatInbound: {
          type: 'array',
          items: {
            properties: {
              TotalCount: { type: 'number' },
              DiffTotalPercent: { type: 'number' },
              CurrentResponseCount: { type: 'number' },
              DiffResponsePercent: { type: 'number' },
              CurrentAbondedchatsCount: { type: 'number' },
              DiffAbondedchatsPercent: { type: 'number' },
            }
          }
        },
        QueueWebchatInboundTable: {
          type: 'array',
          items: {
            properties: {
              queueName: { type: 'string' },
              receivedchats: { type: 'number' },
              responseTime: { type: 'number' },
              abondedchats: { type: 'number' },
              MissedChat: { type: 'number' },
              transferredchat: { type: 'number' },
              totalChatduration: { type: 'number' },
              avgChatduration: { type: 'number' },
              interactionfromcustomer: { type: 'number' },
              interactionfromagent: { type: 'number' },
              totalinteraction: { type: 'number' },
              peakchats: { type: ['number', 'null'] },
            }
          }
        }
      }
    }
  }
};
