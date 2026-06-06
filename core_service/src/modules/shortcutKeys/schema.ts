/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

export const updateShortcutKeysSchema = {
  body: {
    type: 'object',
    properties: {
      uuid: { type: 'string' },
      keyValues: { type: 'object' },
    },
    required: [
      'uuid', 'keyValues',
    ],
  },
};
