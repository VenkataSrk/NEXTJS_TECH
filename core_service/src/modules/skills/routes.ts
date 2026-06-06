import { SKILLS } from '../../helpers/constants';
import { deleteSkillHandler, getSkillHandler, getSkillListHandler, insertSkillHandler, isSkillEnableListHandler, isValidSkillNameHandler, skillStatusHandler, updateSkillHandler, updateSkillStatusHandler } from './handler';
import { deleteSkillSchema, getSkilllistSchema, getSkillSchema, insertSkillSchema, isEnableListSchema, isValidSchema, skillStatusSchema, updateSkillSchema, updateStatusSchema } from './schema';

export default function skillhandler(server: any, options: any, next: any) {
  // skill insert
  server.post(
    SKILLS.SKILL_INSERT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'insert skill',
        description: ' insert skill api',
        tags: ['skills'],
        body: insertSkillSchema.body,
        response: insertSkillSchema.response,
      },
    },
    insertSkillHandler
  );

  // skill get
  server.get(
    `${SKILLS.SKILL_GET}/:sid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get skill ',
        description: 'get skill api',
        tags: ['skills'],
        response: getSkillSchema.response,
      },
    },
    getSkillHandler
  );

  // skill update
  server.put(
    `${SKILLS.SKILL_UPDATE}/:sid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update skill ',
        description: 'update skill api',
        tags: ['skills'],
        body: updateSkillSchema.body,
        params: updateSkillSchema.response,
        response: updateSkillSchema.response,
      },
    },
    updateSkillHandler
  );
  // skill delete
  server.delete(
    `${SKILLS.SKILL_DELETE}/:sid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'delete skill',
        description: 'delete skill api',
        tags: ['skills'],
        params: deleteSkillSchema.params,
      },
    },
    deleteSkillHandler
  );
  // skill getList
  server.get(
    SKILLS.SKILL_GET_LIST,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get skill list ',
        description: 'get skill list api',
        tags: ['skills'],
        response: getSkilllistSchema.response,
      },
    },
    getSkillListHandler
  );

  // routing profile status
  server.get(
    `${SKILLS.SKILL_STATUS}/:sid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'skill status',
        description: 'skill status api',
        tags: ['skills'],
        params: skillStatusSchema.params,
      },
    },
    skillStatusHandler
  );

  // SKILLS update
  server.put(
    `${SKILLS.UPDATE_SKILL_STATUS}/:sid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update status',
        description: 'update status api',
        tags: ['skills'],
        params: updateStatusSchema.params,
        body: updateStatusSchema.body,
        response: updateStatusSchema.response,
      },
    },
    updateSkillStatusHandler
  );

  // is valid skill
  server.post(
    SKILLS.IS_VALID_SKILL_NAME,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'valid skill',
        description: ' valid skill api',
        tags: ['skills'],
        body: isValidSchema.body,
        response: isValidSchema.response,
      },
    },
    isValidSkillNameHandler
  );

  // get enabled list of skills
  server.get(
    SKILLS.IS_ENABLE_LIST,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get enabled list',
        description: 'get enabled list api',
        tags: ['skills'],
        response: isEnableListSchema.response,
      },
    },
    isSkillEnableListHandler
  );
  next();
}
