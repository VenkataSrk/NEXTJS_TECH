export const deleteScheduleReportSchema = {
  params: {
    type: 'object',
    properties: {
      uid: { type: 'number' },
    },
    required: ['uid'],
  },
  response: {
    200: {
      type: 'object',
      properties: {
        ScheduleReport: {
          type: 'array',
          items: {
            type: 'object',
            properties: {
              errcode: { type: 'number' },
              errmsg: { type: 'string' },
            },
          },
        },
        statusCode: { type: 'number' },
      },
    },
  },
};
