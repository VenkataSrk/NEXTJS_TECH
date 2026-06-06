// schema.ts
export const guestSchema = {
  body: {
    type: 'object',
    properties: {
      ext: { type: 'string' },
      domainId: { type: 'string' },
      domainName: { type: 'string' },
      password: { type: 'string' },
      callerName: { type: 'string' },
    },
    required: ['ext', 'domainId', 'domainName', 'password', 'callerName'],
    additionalProperties: false,
  }
};
