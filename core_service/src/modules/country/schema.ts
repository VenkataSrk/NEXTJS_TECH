// getCountrySchema
export const getCountrySchema = {
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getList: {
          type: 'array',
          items: {
            properties: {
              id: { type: 'number' },
              iso: { type: 'string' },
              name: { type: 'string' },
              nicename: { type: 'string' },
              iso3: { type: 'string' },
              phonecode: { type: 'number' },
              phonecode_len: { type: 'number' },
              is_free_call: { type: 'number' },
            },
          },
        },
      },
    },
  },
};
