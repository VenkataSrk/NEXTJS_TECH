/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

// insertChannelSchema
export const voiceMailInsertSchema = {
  body: {
    type: 'object',
    properties: {
      uuid: { type: 'string' },
      companyId: { type: 'string' },
      domainId: { type: 'string' },
      filePath: { type: 'string' },
      agent: { type: 'string' },
      fileUrl: { type: 'string' },
      channelId: { type: 'string' },
      ChannelName: { type: 'string' },
      queue: { type: 'string' },
      customer: { type: 'string' },
      assignedTo: { type: 'string' },
      assigned: { type: 'string' },
      status: { type: 'string' },

    },
    required: [
      'uuid',
      'companyId',
      'domainId',
      'filePath',
      'agent',
      'channelId',
      'ChannelName',
      'fileUrl',
      'queue',
      'customer',
      'assignedTo',
      'assigned',
      'status',
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

export const getVoiceMailListSchema = {
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message:{ type:'string' },
        result: {
          type: 'array',
          items: {
            type: 'object',
            properties: {
              uuid:{ type: 'string' },
              filePath: { type: 'string' },
              companyId: { type: 'string' },
              domainId: { type: 'string' },
              createdDAte: { type: 'number' },
              agent: { type: 'number' },
              fileUrl:{ type: 'number' },
              queue: { type: 'string' },
              customer: { type: 'string' },
              assigned:{ type: 'string' },
              assignedTo:{ type: 'string' },
              status:{ type: 'string' },
            }
          }
        }
      },
    },
  },
};
export const getVoiceMailLogSchema = {
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message:{ type:'string' },
        result: {
          type: 'array',
          items: {
            type: 'object',
            properties: {
              uuid:{ type: 'string' },
              filePath: { type: 'string' },
              companyId: { type: 'string' },
              domainId: { type: 'string' },
              createdDAte: { type: 'number' },
              agent: { type: 'number' },
              fileUrl:{ type: 'number' },
              queue: { type: 'string' },
              customer: { type: 'string' },
              assigned:{ type: 'string' },
              assignedTo:{ type: 'string' },
              status:{ type: 'string' },
            }
          }
        }
      },
    },
  },
};
