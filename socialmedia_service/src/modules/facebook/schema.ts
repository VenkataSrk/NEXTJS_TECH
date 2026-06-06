
// sendFacebookSchema
export const sendFacebookSchema = {
  body: {
    type: 'object',
    properties: {
      sessionId: { from: 'string' },
      from: { from: 'string' },
      to: { from: 'string' },
      text: { from: 'string' },
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
