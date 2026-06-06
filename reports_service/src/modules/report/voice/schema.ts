
export const voiceReportScheme = {
  body: {
    type: 'object',
    properties: {
      startTmpstmp: { type: 'number' },
      endTmpstmp: { type: 'number' },
      timeZone: { type: 'string' },
      limit: { type: 'number' },
      offset: { type: 'number' },
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
