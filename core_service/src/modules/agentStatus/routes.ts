import { AGENT } from '../../helpers/constants';
import { deleteAgentStatusHandler, getAgentStatusDescHandler, getAgentStatusDetailsHandler, getAgentStatusHandler, getEmailHistoryHandler, insertAgentStatusHandler, isValidAgentNameHandler, updateAgentStatusHandler, updateAgentStatusValueHandler } from './handler';
import { deleteAgentSchema, getAgentStatuschema, getAgentStatusDetailschema, insertAgentStatusSchema, isValidSchema, updateAgentSchema, updateAgentStatusSchema } from './schema';

export default function agentStatusHandler(server: any, options: any, next: any) {
  // insert agent status
  server.post(
    AGENT.INSERT_AGENT_STATUS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'insert agent status',
        description: 'insert agent status api',
        tags: ['agent'],
        // body: insertAgentStatusSchema.body,
        // response: insertAgentStatusSchema.response,
      },
    },
    insertAgentStatusHandler
  );

  // get agent status
  server.get(
    AGENT.GET_AGENT_STATUS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get agent status',
        description: 'get agent status api',
        tags: ['agent'],
        response: getAgentStatuschema.response,
      },
    },
    getAgentStatusHandler
  );

  // get agent desc
  server.get(
    AGENT.GET_AGENT_STATUS_DESC,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get agent status',
        description: 'get agent status api',
        tags: ['agent'],
        // response: getAgentStatuschema.response,
      },
    },
    getAgentStatusDescHandler
  );

  // get agent status details
  server.get(
    `${AGENT.GET_AGENT_STATUS_DETAILS}/:aid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get agent status details',
        description: 'get agent status details api',
        tags: ['agent'],
        response: getAgentStatusDetailschema.response,
      },
    },
    getAgentStatusDetailsHandler
  );

  // update agent status
  server.put(
    `${AGENT.UPDATE_AGENT_STATUS}/:aid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update agent status',
        description: 'update agent status api',
        tags: ['agent'],
        params: updateAgentSchema.params,
        body: updateAgentSchema.body,
        response: updateAgentSchema.response,
      },
    },
    updateAgentStatusHandler
  );

  // delete agent status
  server.delete(
    `${AGENT.DELETE_AGENT_STATUS}/:aid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'delete agent status',
        description: 'delete agent status api',
        tags: ['agent'],
        params: deleteAgentSchema.params,
        response: deleteAgentSchema.response,
      },
    },
    deleteAgentStatusHandler
  );

  // is valid agent
  server.post(
    AGENT.IS_VALID_AGENT_NAME,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'valid agent',
        description: ' valid agent api',
        tags: ['agent'],
        // body: isValidSchema.body,
        // response: isValidSchema.response,
      },
    },
    isValidAgentNameHandler
  );
  // update the agent values  by state
  // update webChat
  server.put(
    `${AGENT.UPDATE_AGENT_STATUS_VALUE}/:email`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'agent status update',
        description: 'agent status update by id',
        tags: ['agentStatus'],
        body: updateAgentStatusSchema.body,
        params: updateAgentStatusSchema.params,
        response: updateAgentStatusSchema.response,
      },
    },

    updateAgentStatusValueHandler
  );

  // get email Email History
  server.post(
    AGENT.GET_EMAIL_HISTORY_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        description: 'get_email_history_details',
        tags: ['EMAIL']
      }
    },
    getEmailHistoryHandler
  );

  next();
}
