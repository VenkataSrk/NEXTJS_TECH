/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

// insertSkillSchema
export const insertSkillSchema = {
  body: {
    type: 'object',
    properties: {
      skill_name: { type: 'string' },
      skill_description: { type: 'string' },
      status: { type: 'number', enum: [0, 1] },
      type: { type: 'string' },
    },
    required: [
      'skill_name',
    ],
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        insertSkillList: {
          type: 'object',
          properties: {
            sid: { type: 'number' },
            skill_name: { type: 'string' },
            skill_description: { type: 'string' },
            status: { type: 'number' },
            no_of_agnt_sk: { type: 'number' },
            type: { type: 'string' },
            createdAt: { type: 'string' },
            updateddAt: { type: 'string' },
          },
        },
      },
    },
  },
};

// getSkillSchema
export const getSkillSchema = {
  params: {
    type: 'object',
    properties: {
      sid: { type: 'number' },
    },
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getSkillNumberlist: {
          type: 'array',
          items: {
            properties: {
              sid: { type: 'number' },
              skill_name: { type: 'string' },
              skill_description: { type: 'string' },
              status: { type: 'number' },
              createdAt: { type: 'string' },
              updateddAt: { type: 'string' },
            },
          },
        },
      },
    },
  },
};

// updateSkillSchema
export const updateSkillSchema = {
  params: {
    type: 'object',
    properties: {
      sid: { type: 'number' },
    },
  },
  body: {
    type: 'object',
    properties: {
      skill_name: { type: 'string' },
      skill_description: { type: 'string' },
      status: { type: 'number', enum: [0, 1] },
      type: { type: 'string' },
    },
    required: [
      'skill_name',
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

// deleteSkillSchema
export const deleteSkillSchema = {
  params: {
    type: 'object',
    properties: {
      sid: { type: 'number' },
    },
    required: ['sid'],
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
// getSkilllistSchema
export const getSkilllistSchema = {
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getSkillNumberlist: {
          type: 'array',
          properties: {
            sid: { type: 'number' },
            skill_name: { type: 'string' },
            skill_description: { type: 'string' },
            status: { type: 'number' },
            createdAt: { type: 'string' },
            type: { type: 'string' },
            updateddAt: { type: 'string' },
            number_of_agent_skills: { type: 'number' }
          },
        },
      },
    },
  },
};

// skillStatusSchema
export const skillStatusSchema = {
  params: {
    type: 'object',
    properties: {
      sid: { type: 'string' },
    },
    required: ['sid'],
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

// updateStatusSchema
export const updateStatusSchema = {
  params: {
    type: 'object',
    properties: {
      sid: { type: 'number' },
    },
    required: ['sid'],
  },
  body: {
    type: 'object',
    properties: {
      status: { type: 'number', enum: [0, 1] },
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

// isValidSchema
export const isValidSchema = {
  body: {
    type: 'object',
    properties: {
      name: { type: 'string' },
    },
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        flag: { type: 'number' }
      },
    },
  },
};

// isEnableListSchema
export const isEnableListSchema = {
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getSkillEnableListRes: {
          type: 'array',
          properties: {
            sid: { type: 'number' },
            skill_name: { type: 'string' },
            skill_description: { type: 'string' },
            status: { type: 'number' },
            createdAt: { type: 'string' },
            updateddAt: { type: 'string' },
            number_of_agent_skills: { type: 'number' }
          },
        },
      },
    },
  },
};
