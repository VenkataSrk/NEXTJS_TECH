
// sendEmailSchema
export const sendEmailSchema = {
  body: {
    type: 'object',
    properties: {
      sessionId: { from: 'string' },
      from: { from: 'string' },
      to: { from: 'string' },
      subject: { from: 'string' },
      text: { from: 'string' },
      fileName: { from: 'string' },
      filePath: { from: 'string' },
      messageId: { from: 'string' },
      agentName:{ from: 'string' },
      graphMessageId:{ from: 'string' }
    }
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
